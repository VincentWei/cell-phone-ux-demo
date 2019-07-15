///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// START_OF_INCS
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>
#include <mgncs4touch/mgncs4touch.h>
#include <mgplus/mgplus.h>
#include "register.h"

#include "mspeedmeter.h"

//ctnr
#define CH_IDC_CTNR			201
#define CH_IDC_TIMER        202

//wnd
#define WND_X	0
#define WND_Y	0
#define WND_W	ACTIVITY_W
#define WND_H	ACTIVITY_H

#define SCR_COUNT   50
#define SCR_ITEM    10
#define SCR_TXTLEN  30

static char txt[SCR_COUNT][SCR_ITEM][SCR_TXTLEN];
static mTextPiece *ct[SCR_COUNT][SCR_ITEM];
PLOGFONT lf;

static BOOL onkeydown(mWidget *widget, int message, int code, DWORD keyStatus) {
    mScrollViewPiece *view = (mScrollViewPiece *)((mContainerCtrl *)widget)->body;
    RECT rc;
    int offset_x = 0;
    int offset_y = 0;

    _c(view)->getViewport(view, &rc);
    switch (code) {
        case SCANCODE_J:
            offset_y = 10;
            break;
        case SCANCODE_K:
            offset_y = -10;
            break;
        case SCANCODE_L:
            offset_x = 10;
            break;
        case SCANCODE_H:
            offset_x = -10;
            break;
        case SCANCODE_S:
            {
                static BOOL show = TRUE;
                _c(view)->showScrollBar(view, show);
                show = !show;
            }
            break;
        default:
            return FALSE;
    }
    _c(view)->moveViewport(view, rc.left + offset_x, rc.top + offset_y);
    return TRUE;
}


static int s_onMouseEvent(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    printf("%s(message=%d)\n", __FUNCTION__, message);
    return -1;
}

static BOOL timerhandle_view(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mScrollViewPiece *view;
    int offset_y = 20;
    RECT rc;
    static int isDown = 1;

    view = (mScrollViewPiece*)listener;
    _c(view)->getViewport(view, &rc);

    if(isDown == 0){
        if(rc.top > 0)
            offset_y = -10;
        else{
            isDown = 1;
            offset_y = 10;
        }
    }
    else if(isDown){
        if(rc.bottom < WND_H * SCR_COUNT)
            offset_y = 10;
        else{
            isDown = 0;
            offset_y = -10;
        }
    }

    _c(view)->moveViewport(view, rc.left, rc.top + offset_y);

    return TRUE;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    int i, j;
    mContainerCtrl *ctnr;
    mPanelPiece *content;
    mScrollViewPiece *view;
    RECT rc;
    RECT txtRc;
    mHotPiece *back;
    //mShapeTransRoundPiece *item[SCR_COUNT];
    mTimer *timer;

    lf = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, 
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            26, 0);

    ctnr = (mContainerCtrl*)ncsGetChildObj(self->hwnd, CH_IDC_CTNR);
    GetClientRect(ctnr->hwnd, &rc);

    view = NEWPIECE(mScrollViewPiece);
    _c(view)->setRect(view, &rc);
    _c(ctnr)->setBody(ctnr, (mHotPiece*)view);

    content = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(content)->appendEventHandler(content, MSG_LBUTTONDOWN, s_onMouseEvent);
    _c(content)->appendEventHandler(content, MSG_LBUTTONUP, s_onMouseEvent);
    _c(content)->appendEventHandler(content, MSG_MOUSEMOVE, s_onMouseEvent);
    _c(view)->addContent(view, (mHotPiece*)content, 0, 0);

    rc.bottom = SCR_COUNT * RECTH(rc);
    _c(content)->setRect(content, &rc);

#if 1
    back = (mHotPiece*)NEWPIECE(mRectPiece);
    _c(back)->setRect (back, &rc);
    _c(back)->setProperty (back, NCSP_RECTPIECE_FILLCOLOR, MakeRGBA(0xff, 0x00, 0x00, 0x00));
    _c(back)->setProperty (back, NCSP_RECTPIECE_YRADIUS, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_BORDERSIZE, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_XRADIUS, 5);
    _c(content)->addContent(content, back, 0, 0);
#endif

    GetClientRect(ctnr->hwnd, &rc);
    SetRect(&txtRc, 0, 0, WND_W, WND_H / SCR_ITEM);
    for(i=0; i<SCR_COUNT; i++){
#if 0
        item[i] = NEWPIECE(mShapeTransRoundPiece);
        _c(item[i])->setRect(item[i], &rc);
        _c(item[i])->setProperty(item[i], NCSP_TRANROUND_BKCOLOR, 0xff000000 + (0xff << (i % 3 * 8)));
        _c(content)->addContent(content, (mHotPiece*)item[i], 0, RECTH(rc) * i);
#endif

        for(j=0; j<SCR_ITEM; j++){
            mTextPiece *tmp;

            sprintf(txt[i][j], "%dth screen, %dth item", i, j);
            ct[i][j] = NEWPIECE(mTextPiece);
            tmp = ct[i][j];
            _c(tmp)->setRect(tmp, &txtRc);
            _c(tmp)->setProperty(tmp, NCSP_TEXTPIECE_LOGFONT, (DWORD)lf);
            _c(tmp)->setProperty(tmp, NCSP_LABELPIECE_LABEL, (DWORD)txt[i][j]);
            _c(tmp)->setProperty(tmp, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff000000);
            _c(content)->addContent(content, (mHotPiece*)tmp,
                    (RECTW(rc) - RECTW(txtRc))/2,
                    j * RECTH(txtRc) + i * RECTH(rc));
        }
    }


    timer = (mTimer*)ncsGetChildObj(self->hwnd, CH_IDC_TIMER);
    ncsAddEventListener((mObject *)timer, (mObject *)view, (NCS_CB_ONPIECEEVENT)timerhandle_view, MSG_TIMER);
    _c(timer)->start(timer);
    
    return TRUE;
}
static BOOL mymain_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static BOOL mymain_onClose(mWidget* self, int message)
{   
    DestroyMainWindow(self->hwnd);
    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {MSG_CLOSE, mymain_onClose },
    {MSG_ERASEBKGND, mymain_onEraseBk},
    {MSG_LBUTTONDOWN, SpeedMeterMessageHandler },
    {MSG_LBUTTONUP, SpeedMeterMessageHandler },
    {MSG_MOUSEMOVE, SpeedMeterMessageHandler },
    {0, NULL}
};

static NCS_EVENT_HANDLER ctrl_handles[] = {
    { MSG_KEYDOWN, onkeydown },
    { 0, NULL }
};
static NCS_PROP_ENTRY timer_props[] = {
    {NCSP_TIMER_INTERVAL, 1},//unit:10ms
    {0, 0},
};
static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        CH_IDC_CTNR,
        0, 0, WND_W, WND_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        ctrl_handles, NULL, 0, 0,
    },
    {
        NCSCTRL_TIMER,
        CH_IDC_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timer_props, NULL, NULL, NULL, 0, 0, 
    },
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        WND_X, WND_Y, WND_W, WND_H,
        WS_NONE,
        WS_EX_AUTOSECONDARYDC,
        "scrollview",
        NULL,
        NULL,
        mymain_handlers,
        _ctrl_tmpl,
        TABLESIZE(_ctrl_tmpl),
        0,
        0, 0,
};

int MiniGUIMain(int argc, const char *argv[]) {
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    ncsInitialize();
    ncs4TouchInitialize();
    REGISTER_NCS();

    mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect
                (&mymain_templ, HWND_DESKTOP);
    mGEffInit();
    _c(mydlg)->doModal(mydlg, TRUE);
    MainWindowThreadCleanup(mydlg->hwnd);

    MGNCS_UNREG_COMPONENT(mContainerCtrl);
    ncs4TouchUninitialize();
    ncsUninitialize ();

    return 0;
}

