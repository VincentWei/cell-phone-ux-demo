
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
#include "mtextpiece.h"

#define PAD_WIDTH (g_bitmap.bmWidth-60)
#define PAD_HEIGHT (g_bitmap.bmHeight-100)

BITMAP g_bitmap;
PLOGFONT g_font[9];

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;
    mPanelPiece *content;
    RECT rc;

    memset (&g_bitmap, 0, sizeof(g_bitmap));
    LoadBitmapFromFile (HDC_SCREEN, &g_bitmap, "../../res/common/desktop.png");
    if (g_bitmap.bmWidth == 0 || g_bitmap.bmHeight == 0) {
        printf ("Load g_bitmap: desktop.png error!\n");
        return FALSE;
    }

    GetClientRect(self->hwnd, &rc);
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            (g_bitmap.bmWidth-PAD_WIDTH)/2, (g_bitmap.bmHeight-PAD_HEIGHT)/2, PAD_WIDTH, PAD_HEIGHT,
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

    {
        mHotPiece *textpiece = (mHotPiece*)NEWPIECE(mTextPiece);
        SetRect(&rc, 0, 0, PAD_WIDTH, PAD_HEIGHT);
        _c(textpiece)->setRect (textpiece, &rc);

        const int DEFAULT_PIECE_SIZE=40;
        int i,j=0;
        ARGB colors[] = {0xfff10000, 0xfff51000, 0xffffe100, 0xfffff300, 0xfffffee0, 0xffffffff};
        for (i=0; i<TABLESIZE(g_font); ++i) {
            int g_fontsize = 10+i*5;

            //TODO:
            if (g_fontsize == 40) {
                g_fontsize++;
            }

            g_font[i] = CreateLogFont ("ttf", "helvetica", "GB2312",
                    FONT_WEIGHT_BOOK,
                    FONT_SLANT_ROMAN,
                    FONT_SETWIDTH_NORMAL,
                    FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    g_fontsize, 0);

            mHotPiece *textpiece = (mHotPiece*)NEWPIECE(mTextPiece);
            SetRect(&rc, 0, 0, PAD_WIDTH, DEFAULT_PIECE_SIZE);
            _c(textpiece)->setRect (textpiece, &rc);
            _c((mTextPiece*)textpiece)->setProperty ((mTextPiece*)textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)g_font[i]);
            _c((mTextPiece*)textpiece)->setProperty ((mTextPiece*)textpiece, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);
            _c((mTextPiece*)textpiece)->setProperty ((mTextPiece*)textpiece, NCSP_TEXTPIECE_TEXTCOLOR, (WORD)colors[++j]);
            _c((mTextPiece*)textpiece)->setProperty ((mTextPiece*)textpiece, NCSP_LABELPIECE_LABEL, (DWORD)"This is a test!");
            _c(content)->addContent(content, textpiece, 5, 5+DEFAULT_PIECE_SIZE*i );
        }
    }

    return TRUE;
}

static BOOL mymain_onClose(mWidget* self, int message)
{   
    int i;
    UnloadBitmap (&g_bitmap);
    DestroyMainWindow(self->hwnd);
    for (i=0; i<TABLESIZE(g_font); ++i) {
        DestroyLogFont (g_font[i]);
    }
    return TRUE;
}


static void mymain_onPaint (mWidget *_this, HDC hdc, const PCLIPRGN pinv_clip)
{
    FillBoxWithBitmap (hdc, 0, 0, 0, 0, &g_bitmap);
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

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "mTextPiece test",
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
