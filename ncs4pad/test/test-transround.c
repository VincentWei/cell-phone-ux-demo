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
#include "mtransroundpiece.h"
#include "register.h"

BITMAP bitmap;

HDC tmpdc;

#define PIECE_WIDTH 300
#define PIECE_HEIGHT 180
static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;
    mPanelPiece *content;
    RECT rc;

    GetClientRect(self->hwnd, &rc);
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            rc.left+50, rc.top+50, rc.right-50, rc.bottom-50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    content = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, RECTW(rc)-60, RECTH(rc)-40);
    _c(content)->setRect(content, &rc);
    _c(ctnr)->setBody(ctnr, (mHotPiece*)content);

    rc.left = 0;
    rc.right = 0;
    rc.right  = rc.left + PIECE_WIDTH;
    rc.bottom = rc.top + PIECE_HEIGHT;

    /* fill solid back ground */
    {
        mHotPiece *back_solid = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_solid)->setRect (back_solid, &rc);
        ARGB BKCOLOR = 0x8f0000ff;
        _c(back_solid)->setProperty (back_solid, NCSP_TRANROUND_BKCOLOR, BKCOLOR);
        _c(back_solid)->setProperty (back_solid, NCSP_TRANROUND_BORDERSIZE, 8);
        ARGB BORDER_COLOR = 0x8f00ff00;
        _c(back_solid)->setProperty (back_solid, NCSP_TRANROUND_BORDERCOLOR, BORDER_COLOR);
        _c(content)->addContent(content, back_solid, 5, 5);
    }

    /* fill gradient back ground */
    {
        mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_gradient)->setRect (back_gradient, &rc);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 
                TRANROUND_CORNERFLAG_LEFTTOP | TRANROUND_CORNERFLAG_RIGHTTOP);
        ARGB colors[]={0x3fff0000, 0x7f00ff00, 0xaf003fff, 0xff0000ff};
        _c((mShapeTransRoundPiece*)back_gradient)->setGradientFillColors((mShapeTransRoundPiece*)back_gradient, colors, 4);
        _c(content)->addContent(content, back_gradient, 5, 15+PIECE_HEIGHT);
    }

    /* fill with custom's brush */
    {
        mHotPiece *back_custom_brush = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_custom_brush)->setRect (back_custom_brush, &rc);
        _c(back_custom_brush)->setProperty (back_custom_brush, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        HBRUSH brush = _c((mShapeTransRoundPiece*)back_custom_brush)->getBrush((mShapeTransRoundPiece*)back_custom_brush);

        /* 0.0f, 1.0f are needed */
        MGPlusLinearGradientBrushAddColor (brush, 0x3fff0000, 0.0f);
        MGPlusLinearGradientBrushAddColor (brush, 0x7f00ff00, 0.2f);
        MGPlusLinearGradientBrushAddColor (brush, 0xaf003fff, 0.4f);
        MGPlusLinearGradientBrushAddColor (brush, 0xff0000ff, 1.0f);
        _c(content)->addContent(content, back_custom_brush, 15+PIECE_WIDTH, 5);
    }

    /* vertical gradient fill */
    {
        mHotPiece *back_vertical_fill = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_vertical_fill)->setRect (back_vertical_fill, &rc);
        _c(back_vertical_fill)->setProperty (back_vertical_fill, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        HBRUSH brush = _c((mShapeTransRoundPiece*)back_vertical_fill)->getBrush((mShapeTransRoundPiece*)back_vertical_fill);

        MGPlusSetLinearGradientBrushMode (brush, MP_LINEAR_GRADIENT_MODE_HORIZONTAL);

        /* 0.0f, 1.0f are needed */
        MGPlusLinearGradientBrushAddColor (brush, 0x3fff0000, 0.0f);
        MGPlusLinearGradientBrushAddColor (brush, 0x7f00ff00, 0.2f);
        MGPlusLinearGradientBrushAddColor (brush, 0xaf003fff, 0.4f);
        MGPlusLinearGradientBrushAddColor (brush, 0xff0000ff, 1.0f);
#if 0
        _c(content)->addContent(content, back_vertical_fill, 15+PIECE_WIDTH, 15+PIECE_HEIGHT);
#else
        tmpdc = CreateMemDC(PIECE_WIDTH, PIECE_HEIGHT, 32, MEMDC_FLAG_SWSURFACE, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        _c((mShapeTransRoundPiece*)back_vertical_fill)->paint((mShapeTransRoundPiece*)back_vertical_fill, tmpdc, NULL, 0);
#endif
    }

    {
        mHotPiece *label1 = (mHotPiece*)NEWPIECE(mLabelPiece);
        rc.bottom = 10;
        _c(label1)->setRect (label1, &rc);
        _c(label1)->setProperty (label1, NCSP_LABELPIECE_LABEL, (DWORD)"This message on the container ctrl!");
        _c(content)->addContent(content, label1, 10, 100);
    }

    {
        mHotPiece *bitmap_piece = (mHotPiece*)NEWPIECE(mImagePiece);
        LoadBitmap(HDC_SCREEN, &bitmap, "news.png");
        rc.right  = bitmap.bmWidth;
        rc.bottom = bitmap.bmHeight;
        _c(bitmap_piece)->setRect (bitmap_piece, &rc);
        _c((mImagePiece*)bitmap_piece)->setProperty ((mImagePiece*)bitmap_piece, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bitmap);
        _c(content)->addContent(content, bitmap_piece, 110, 250);
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
    TextOut (hdc, 10, 100, "This message display at main win!");
    TextOut (hdc, 10, 350, "This message display at main win!");
    TextOut (hdc, 10+PIECE_WIDTH, 100, "This message display at main win!");
    TextOut (hdc, 10+PIECE_WIDTH, 350, "This message display at main win!");

    BitBlt (tmpdc, 0, 0, 0, 0, hdc, 65+PIECE_WIDTH, 65+PIECE_HEIGHT, 0);
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {MSG_PAINT, mymain_onPaint},
    {MSG_CLOSE, mymain_onClose },
    {0, NULL}
};

int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "transround" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "mShapeTransRoundPiece test",
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
