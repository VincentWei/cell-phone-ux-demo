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
// END_OF_INCS
#include "mcontainerctrl.h"
#include "mtransroundpiece.h"
#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "mpanelpiece.h"
#include "mbuttonpanelpiece.h"
#include "mrotateswitchpiece.h"
#include "mscrollviewpiece.h"
#include "mspeedmeter.h"

#include "register.h"

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

static NCS_EVENT_HANDLER g_handles[] = {
    { MSG_KEYDOWN, onkeydown },
    { 0, NULL }
};

static int s_onMouseEvent(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    printf("%s(message=%d)\n", __FUNCTION__, message);
    return -1;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;
    mPanelPiece *content;
    mScrollViewPiece *view;
    RECT rc;
    mHotPiece *ct1;
    mHotPiece *ct2;
    mHotPiece *back;
    mHotPiece *ct4;
    mHotPiece *ct5;
    mHotPiece *ct6;
    static BITMAP bmp[3];

    GetClientRect(self->hwnd, &rc);
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            (RECTW(rc) - 300)/2, rc.top+10, 300, RECTH(rc)-10*2,
            self->hwnd,
            NULL, NULL, NULL, 0);

    content = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(content)->appendEventHandler(content, MSG_LBUTTONDOWN, s_onMouseEvent);
    _c(content)->appendEventHandler(content, MSG_LBUTTONUP, s_onMouseEvent);
    _c(content)->appendEventHandler(content, MSG_MOUSEMOVE, s_onMouseEvent);

    view = NEWPIECE(mScrollViewPiece);
    GetClientRect(ctnr->hwnd, &rc);
    _c(view)->setRect(view, &rc);
    _c(view)->addContent(view, (mHotPiece*)content, 0, 0);

    _c(ctnr)->setBody(ctnr, (mHotPiece*)view);

    ct1 = (mHotPiece*)NEWPIECE(mLabelPiece);
    ct2 = (mHotPiece*)NEWPIECE(mLabelPiece);
    back = (mHotPiece*)NEWPIECE(mRectPiece);
    ct4 = (mHotPiece*)NEWPIECE(mImagePiece);
    ct5 = (mHotPiece*)NEWPIECE(mImagePiece);
    ct6 = (mHotPiece*)NEWPIECE(mImagePiece);
    
    rc.right = 100;
    rc.bottom = 20;
    _c(ct1)->setRect (ct1, &rc);
    rc.right = 100;
    rc.bottom = 20;
    _c(ct2)->setRect (ct2, &rc);

    /* add rect piece as background.*/
    GetClientRect(ctnr->hwnd, &rc);
    rc.bottom += 10 * RECTH(rc);
    _c(content)->setRect(content, &rc);
    _c(back)->setRect (back, &rc);
    _c(back)->setProperty (back, NCSP_RECTPIECE_FILLCOLOR, MakeRGBA(0x00, 0xff, 0x00, 0xff));
    _c(back)->setProperty (back, NCSP_RECTPIECE_YRADIUS, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_BORDERSIZE, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_XRADIUS, 5);

    /* add image.*/
    //LoadResource(HDC_SCREEN, RES_TYPE_IMAGE, "new2.jpg");
    LoadBitmap(HDC_SCREEN, &bmp[0], "news.png");
    LoadBitmap(HDC_SCREEN, &bmp[1], "file.png");
    LoadBitmap(HDC_SCREEN, &bmp[2], "internet.png");
    rc.right  = bmp[0].bmWidth;
    rc.bottom = bmp[0].bmHeight;
    _c(ct4)->setRect (ct4, &rc);
    _c(ct4)->setProperty (ct4, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[0]);
    rc.right  = bmp[1].bmWidth;
    rc.bottom = bmp[1].bmHeight;
    _c(ct5)->setRect (ct5, &rc);
    _c(ct5)->setProperty (ct5, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[1]);
    rc.right  = bmp[2].bmWidth;
    rc.bottom = bmp[2].bmHeight;
    _c(ct6)->setRect (ct6, &rc);
    _c(ct6)->setProperty (ct6, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[2]);

    _c(ct1)->setProperty (ct1, NCSP_LABELPIECE_LABEL, (DWORD)"hello");
    _c(ct2)->setProperty (ct2, NCSP_LABELPIECE_LABEL, (DWORD)"world");
    
    _c(content)->addContent(content, back, 5, 5);
    _c(content)->addContent(content, ct1, 10, 20);
    _c(content)->addContent(content, ct2, 120, 20);
    _c(content)->addContent(content, ct4, 105, 105);
    _c(content)->addContent(content, ct5, 165, 105);
    _c(content)->addContent(content, ct6, 235, 105);

    ncsSetComponentHandlers((mComponent *)ctnr, g_handles, -1);

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
    {MSG_LBUTTONDOWN, SpeedMeterMessageHandler },
    {MSG_LBUTTONUP, SpeedMeterMessageHandler },
    {MSG_MOUSEMOVE, SpeedMeterMessageHandler },
    {0, NULL}
};

int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "indepCoorContainerPiece test",
            WS_CAPTION | WS_BORDER | WS_VISIBLE,
            WS_EX_AUTOSECONDARYDC,
            1,
            0, 0, g_rcScr.right/2,g_rcScr.bottom,
            HWND_DESKTOP,
            0, 0,
            NULL,
            NULL,
            mymain_handlers,
            0);

    mGEffInit();

    while(GetMessage(&Msg, mymain->hwnd))
    {   
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    mGEffDeinit();

    MainWindowThreadCleanup(mymain->hwnd);

    ncsUninitialize();

    return 0;
}
