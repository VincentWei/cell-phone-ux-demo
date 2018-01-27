#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "register.h"
#include "mwidgethostpiece.h"
#include "ResourceManager.hh"

//#define HOSTDBG

#ifdef HOSTDBG
#ifndef dbg
#define dbg() printf("[%s]:%d\n", __FUNCTION__, __LINE__)
#endif
#else
#ifndef dbg
#define dbg()
#endif
#endif

static mWidget *createHostedWindow(mWidgetHostPiece *, NCS_WND_TEMPLATE *);
static int onUpdate(HWND, HDC, HDC, const RECT *, const RECT *, const RECT *);
static void destroyHostedWindow(mWidgetHostPiece *);

static void mWidgetHostPiece_construct(mWidgetHostPiece *self, DWORD add_data){
	Class(mPanelPiece).construct((mPanelPiece*)self, add_data);

    self->hosted = NULL;
	self->bCaptureMouse = FALSE;
    self->main_hwnd = (HWND)add_data;
    assert(self->main_hwnd);
}

static void mWidgetHostPiece_setHosted(mWidgetHostPiece *self, NCS_WND_TEMPLATE *tmpl){
    assert(self->hosted == NULL);

    self->hosted = createHostedWindow(self, tmpl);
    _c(self->hosted)->addRef(self->hosted);

    self->memDC = (GetGDCapability(Get32MemDC(), GDCAP_AMASK) == 0)
        ? CreateMemDC(tmpl->w, tmpl->h, 32, MEMDC_FLAG_SWSURFACE, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)
        : CreateCompatibleDCEx(Get32MemDC(), tmpl->w, tmpl->h);

	SetSecondaryDC(self->hosted->hwnd, self->memDC, onUpdate);
	InvalidateRect(self->hosted->hwnd, NULL, FALSE);
}

static void mWidgetHostPiece_paint(mWidgetHostPiece *self, HDC hdc, mObject *owner, DWORD add_data){
	BitBlt(self->memDC, 0, 0, 0, 0, hdc, 0, 0, 0);
	
	Class(mPanelPiece).paint((mPanelPiece*)self, hdc, owner, add_data);
}

static int mWidgetHostPiece_processMessage(mWidgetHostPiece *self, int message, WPARAM wparam, LPARAM lparam, mObject *owner){
	mWidget *hosted;
    mWidget *host;

    host = (mWidget*)owner;
    hosted = _c(self)->getHosted(self);
    if(message == MSG_LBUTTONDOWN){
        SendMessage(hosted->hwnd, message, wparam, lparam);

        //hostpiece capture the mouse
        if((self->hostedPiece = mWidget_getCapturedHotPiece(hosted))){
            self->bCaptureMouse = TRUE;
            mWidget_captureHotPiece(host, (mObject*)self);
        }
    }
    else if(message == MSG_MOUSEMOVE){
        mPanelPiece *cur;
        if(self->bCaptureMouse){
            int x,y;
            mPieceItem *item;
            mPanelPiece *parent;

            dbg();
        #if 0
            mWidget_captureHotPiece(hosted, self->hostedPiece);
        #endif

            x = LOWORD(lparam);
            y = HIWORD(lparam);

            //trans coor
            cur = (mPanelPiece *)self;
            parent = (mPanelPiece*)cur->parent;
            while(parent){
                dbg();
                item = _c(parent)->searchItem(parent, (mHotPiece*)cur);
                if(item){
                    x -= item->x;
                    y -= item->y;
                }
                cur = parent;
                parent = (mPanelPiece*)cur->parent;
            }
            x += self->tmpl->x;
            y += self->tmpl->y;

            lparam = MAKELONG(x, y);
        }
#if 0
        SendMessage(hosted->hwnd, message, wparam, lparam);
#else
        if (self->hostedPiece != NULL)
        {
            mHotPiece *temp = (mHotPiece *)self->hostedPiece;
            _c(temp)->processMessage(temp,message,wparam, lparam,(mObject*)hosted); 
        }
#endif
        if(self->bCaptureMouse){
            mWidget_captureHotPiece(host, (mObject*)self);
        }
    }
    else if(message == MSG_LBUTTONUP){
        if(self->bCaptureMouse){
            self->bCaptureMouse = FALSE;
            mWidget_captureHotPiece(hosted, self->hostedPiece);
        }
        SendMessage(hosted->hwnd, message, wparam, lparam);
    }
    else
        ;

	return 0;
}

static mWidget *mWidgetHostPiece_getHosted(mWidgetHostPiece *self){
	mWidget *hosted;
	NCS_WND_TEMPLATE *ctrl_tmpl;

	ctrl_tmpl = self->tmpl->ctrls;
	hosted = self->hosted;
	return (mWidget*)_c(hosted)->getChild(hosted, ctrl_tmpl->id);
}

static void mWidgetHostPiece_destroy(mWidgetHostPiece *self){
	//add destroy
    SetSecondaryDC(self->hosted->hwnd, self->memDC, ON_UPDSECDC_DONOTHING);
    DeleteSecondaryDC(self->hosted->hwnd);
	destroyHostedWindow(self);
	
	Class(mPanelPiece).destroy((mPanelPiece*)self);
}

BEGIN_MINI_CLASS(mWidgetHostPiece, mPanelPiece)
	CLASS_METHOD_MAP(mWidgetHostPiece, construct)
	CLASS_METHOD_MAP(mWidgetHostPiece, paint)
	CLASS_METHOD_MAP(mWidgetHostPiece, setHosted)
	CLASS_METHOD_MAP(mWidgetHostPiece, processMessage)
	CLASS_METHOD_MAP(mWidgetHostPiece, getHosted)
	CLASS_METHOD_MAP(mWidgetHostPiece, destroy)
END_MINI_CLASS

static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static BOOL main_onCommand(mWidget *self, int id, int nc, HWND hCtrl){
	mWidgetHostPiece *host;

	host = (mWidgetHostPiece *)GetWindowAdditionalData(self->hwnd);
    if (_c(host)->getOwner(host) == NULL)
    {
        return FALSE;
    }
	ncsRaiseEvent((mObject*)host, nc, (DWORD)hCtrl);
	return TRUE;
}

static NCS_EVENT_HANDLER main_handlers[] = {
    {MSG_ERASEBKGND, main_onEraseBk},
	{MSG_COMMAND, main_onCommand},
    {0, NULL},
};

#define COLORKEY(dc) RGBA2Pixel(dc, 0xff,0xff,0xff, 0x00)
static BOOL ctrl_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    SetBrushColor(hdc, COLORKEY(hdc));
    FillBox(hdc, 0, 0, GetGDCapability(hdc, GDCAP_MAXX)+1, GetGDCapability(hdc, GDCAP_MAXY)+1);

    return TRUE;
}
static NCS_EVENT_HANDLER ctrl_handler[] = {
    {MSG_ERASEBKGND, ctrl_onEraseBk},
    {0, NULL},
};
static mWidget *createHostedWindow(mWidgetHostPiece *self, NCS_WND_TEMPLATE *ctrl_tmpl){
	NCS_MNWND_TEMPLATE *tmpl;

    if(ctrl_tmpl->handlers == NULL)
        ctrl_tmpl->handlers = ctrl_handler;

	tmpl = malloc(sizeof(NCS_MNWND_TEMPLATE));
	assert(tmpl);
	memset(tmpl, 0, sizeof(NCS_MNWND_TEMPLATE));

	self->tmpl = tmpl;

	tmpl->class_name = NCSCTRL_DIALOGBOX;
	tmpl->id = (intptr_t)tmpl;
	tmpl->x = -ctrl_tmpl->w;
	tmpl->y = -ctrl_tmpl->h;
	tmpl->w = ctrl_tmpl->w;
	tmpl->h = ctrl_tmpl->h;
	tmpl->style = WS_VISIBLE;
	tmpl->ex_style = WS_EX_NONE;
	tmpl->caption = "hosted";
	tmpl->handlers = main_handlers;
	tmpl->ctrls = ctrl_tmpl;
	tmpl->count = 1;
	tmpl->user_data = (DWORD)self;
#if WIDGETHOST_FROM_DESKTOP
	return ncsCreateMainWindowIndirect(tmpl, HWND_DESKTOP);
#else
	return ncsCreateMainWindowIndirect(tmpl, self->main_hwnd);
#endif
}

static void destroyHostedWindow(mWidgetHostPiece *self){
	free(self->tmpl);
    
	_c(self->hosted)->release(self->hosted);
}

static int onUpdate(HWND hwnd, HDC sec_dc, HDC real_dc,
		const RECT *sec_rc, const RECT *real_rc, const RECT *main_update_rc){
    mWidget *owner;
	mWidgetHostPiece *host;
    mPieceItem *item;
    mPanelPiece *parent;
    mPanelPiece *cur;
    RECT rc;

	host = (mWidgetHostPiece*)GetWindowAdditionalData(GetMainWindowHandle(hwnd));

    if (!(owner = _c(host)->getOwner(host)))
        return 0;

    //offset rect with x&y which used by addContent(..., x, y)
    rc = *main_update_rc;
    cur = (mPanelPiece*)host;
    parent = (mPanelPiece*)cur->parent;
    while(parent){
        item = _c(parent)->searchItem(parent, (mHotPiece*)cur);
		if(item){
            OffsetRect(&rc, item->x, item->y);
        }
        cur = parent;
        parent = (mPanelPiece*)cur->parent;
    }

    InvalidateRect(owner->hwnd, &rc, FALSE);
	return 0;
}

