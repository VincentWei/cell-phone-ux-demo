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
// END_OF_INCS
#include "mlistitemmanager.h"
#include "mcontainerctrl.h"
#include "mpanelpiece.h"
#include "mwrappaintpiece.h"

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;
    mPanelPiece *content;
    RECT rc;

    GetClientRect(self->hwnd, &rc);
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            rc.left+50, rc.top+50, rc.right-50, rc.bottom-50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    content = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, 400, 400);
    _c(content)->setRect(content, &rc);


    mHotPiece *ct1 = (mHotPiece*)NEWPIECE(mLabelPiece);
    mHotPiece *ct2 = (mHotPiece*)NEWPIECE(mLabelPiece);
    mHotPiece *back = (mHotPiece*)NEWPIECE(mRectPiece);
    mHotPiece *ct4 = (mHotPiece*)NEWPIECE(mImagePiece);
    mHotPiece *ct5 = (mHotPiece*)NEWPIECE(mImagePiece);
    mHotPiece *ct6 = (mHotPiece*)NEWPIECE(mImagePiece);
    
    rc.right = 100;
    rc.bottom = 20;
    _c(ct1)->setRect (ct1, &rc);
    rc.right = 100;
    rc.bottom = 20;
    _c(ct2)->setRect (ct2, &rc);

    /* add rect piece as background.*/
    rc.right  = 600-5;
    rc.bottom = 400-5;
    _c(back)->setRect (back, &rc);
    _c(back)->setProperty (back, NCSP_RECTPIECE_FILLCOLOR, MakeRGBA(0x00, 0xff, 0x00, 0xff));
    _c(back)->setProperty (back, NCSP_RECTPIECE_YRADIUS, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_BORDERSIZE, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_XRADIUS, 5);

    /* add image.*/
    //LoadResource(HDC_SCREEN, RES_TYPE_IMAGE, "new2.jpg");
    static BITMAP bmp[3];
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


    {
        mWrapPaintPiece* wrap = (mWrapPaintPiece*)NEWPIECEEX(mWrapPaintPiece, content);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)wrap);
    }

    return TRUE;
}

static void mymain_onlbuttonup(mMainWnd* self, int message, int lParam, int wParam, DWORD addData)
{
    puts("lbuttonup\n");
    UpdateWindow(self->hwnd, TRUE);
}

static BOOL mymain_onClose(mWidget* self, int message)
{   
    DestroyMainWindow(self->hwnd);
    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {MSG_CLOSE, mymain_onClose },
    {MSG_LBUTTONUP, mymain_onlbuttonup},
    {0, NULL}
};

#include "register.h"
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
            WS_EX_NONE,
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
