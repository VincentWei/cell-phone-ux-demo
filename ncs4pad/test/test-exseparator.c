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
#include "mcontainerctrl.h"
#include "mpanelpiece.h"
#include "mexseparatorpiece.h"
#include "mtransroundpiece.h"

#define PAD_WIDTH (bitmap.bmWidth-60)
#define PAD_HEIGHT (bitmap.bmHeight-100)

BITMAP bitmap;
static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;
    mPanelPiece *content;
    RECT rc;

    memset (&bitmap, 0, sizeof(bitmap));
    LoadBitmapFromFile (HDC_SCREEN, &bitmap, "../../res/common/desktop.png");
    if (bitmap.bmWidth == 0 || bitmap.bmHeight == 0) {
        printf ("Load bitmap: desktop.png error!\n");
        return FALSE;
    }

    GetClientRect(self->hwnd, &rc);
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            (bitmap.bmWidth-PAD_WIDTH)/2, (bitmap.bmHeight-PAD_HEIGHT)/2, PAD_WIDTH, PAD_HEIGHT,
            self->hwnd,
            NULL, NULL, NULL, 0);

    content = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, PAD_WIDTH, PAD_HEIGHT);
    _c(content)->setRect(content, &rc);

    _c(ctnr)->setBody(ctnr, (mHotPiece*)content);

    {
        SetRect(&rc, 0, 0, PAD_WIDTH, PAD_HEIGHT);
        mHotPiece *back_solid = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_solid)->setRect (back_solid, &rc);
        ARGB bkcolor = 0x7fced3d6;
        _c(back_solid)->setProperty (back_solid, NCSP_TRANROUND_BKCOLOR, bkcolor);
        _c(content)->addContent(content, back_solid, 0,0);
    }

    mHotPiece *separator_piece[8];
    int i;
    //int separator_size = 8;
    int separator_size = 2;

    {
        for (i=0; i<TABLESIZE(separator_piece); ++i) {
            separator_piece[i] = (mHotPiece*)NEWPIECE(mExSeparatorPiece);
            _c(separator_piece[i])->setProperty(separator_piece[i], NCSP_EXSEPARATOR_SIZE, separator_size);
            _c(separator_piece[i])->setProperty(separator_piece[i], NCSP_EXSEPARATOR_COLOR, 0x7fced3d6);

            if (i < 4) {
                SetRect(&rc, 0, 0, PAD_WIDTH, separator_size);
                _c(separator_piece[i])->setRect (separator_piece[i], &rc);
                _c(content)->addContent(content, separator_piece[i], 0, (PAD_HEIGHT/5)*(i+1));
            } else {
                SetRect(&rc, 0, 0, separator_size, PAD_HEIGHT);
                _c(separator_piece[i])->setRect (separator_piece[i], &rc);
                _c(separator_piece[i])->setProperty(separator_piece[i], \
                        NCSP_EXSEPARATOR_DIRECTION, (DWORD)EXSEPARATOR_DIRECTION_VERTICAL);
                _c(content)->addContent(content, separator_piece[i], (PAD_WIDTH/5)*(i-3), 0);
            }
        }
    }

    return TRUE;
}

static BOOL mymain_onClose(mWidget* self, int message)
{   
    UnloadBitmap (&bitmap);
    DestroyMainWindow(self->hwnd);
    return TRUE;
}


static void mymain_onPaint (mWidget *_this, HDC hdc, const PCLIPRGN pinv_clip)
{
    FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bitmap);
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate},
    {MSG_PAINT, mymain_onPaint},
    {MSG_CLOSE, mymain_onClose},
    {0, NULL}
};

#include "register.h"
int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "exSeparator" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "mExSeparatorPiece test",
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
