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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>

#include "mcontainerctrl.h"
#include "mpanelpiece.h"

static BITMAP g_bmp;

static int container_event_handler(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner) {
    mPanelPiece *self = (mPanelPiece *)_self;
    int x, y;
    x = LOSWORD(lParam);
    y = HISWORD(lParam);

    {
        mHotPiece *child;
        child = _c(self)->childAt(self, x, y);
        if (child) {
            printf("self=%p, child=%p\n", self, child);
            return -1;
        }else{
            return 0;
        }
    }
}

static mPanelPiece *create_subContainer(int w, int h) {
    mPanelPiece *container;
    RECT rc;

    container = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, w, h);
    _c(container)->setRect(container, &rc);

    mHotPiece *ct6 = (mHotPiece*)NEWPIECE(mImagePiece);
    mHotPiece *back = (mHotPiece*)NEWPIECE(mRectPiece);

    SetRect(&rc, 0, 0, w, h);
    _c(back)->setRect (back, &rc);
    _c(back)->setProperty (back, NCSP_RECTPIECE_FILLCOLOR, MakeRGBA(0x00, 0xff, 0x00, 0xff));
    _c(back)->setProperty (back, NCSP_RECTPIECE_YRADIUS, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_BORDERSIZE, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_XRADIUS, 5);

    SetRect(&rc, 0, 0, g_bmp.bmWidth, g_bmp.bmHeight);
    _c(ct6)->setRect (ct6, &rc);
    _c(ct6)->setProperty (ct6, NCSP_IMAGEPIECE_IMAGE, (DWORD)&g_bmp);

    _c(container)->addContent(container, back, 0, 0);
    _c(container)->addContent(container, ct6, 30, 30);

    _c(container)->appendEventHandler(container, MSG_LBUTTONDOWN, container_event_handler);

    return container;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctrl;
    mPanelPiece *container1, *container2, *container3;
    RECT rc;

    LoadBitmap(HDC_SCREEN, &g_bmp, "news.png");

    GetClientRect(self->hwnd, &rc);
    ctrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            rc.left+50, rc.top+50, rc.right-50, rc.bottom-50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    container1 = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, 400, 400);
    _c(container1)->setRect(container1, &rc);

    container2 = create_subContainer(100, 100);
    _c(container1)->addContent(container1, (mHotPiece *)container2, 10, 10);

    container3 = create_subContainer(200, 200);
    _c(container1)->addContent(container1, (mHotPiece *)container3, 120, 10);

    _c(ctrl)->setBody(ctrl, (mHotPiece*)container1);

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
    {0, NULL}
};

#include "register.h"
int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "test" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "container-nesting",
            WS_CAPTION | WS_BORDER | WS_VISIBLE,
            WS_EX_NONE, // WS_AUTO_SECONDARYDC,
            1,
            0, 0, g_rcScr.right,g_rcScr.bottom,
            HWND_DESKTOP,
            0, 0,
            NULL,
            NULL,
            mymain_handlers,
            0);

    while(GetMessage(&Msg, mymain->hwnd))
    {   
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(mymain->hwnd);

    ncsUninitialize();

    return 0;
}
