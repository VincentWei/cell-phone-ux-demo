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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mcontainerctrl.h"
#include "mtransroundpiece.h"
#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "mpanelpiece.h"
#include "mlistitemmanager.h"
#include "mbuttonpanelpiece.h"

#define ANIMATION_RUNTIME 500
#define ANIM_BTN_NUM 20

static mButtonPanelPiece *btn_with_layout;
static mButtonPanelPiece *btn_with_gradient_bgcolor;
static mButtonPanelPiece *btn_with_anim_run;
static mHotPiece *g_txt_piece;
static mButtonPanelPiece *btn_layout_or_not;

static BOOL g_buttonRunIsOn = FALSE;

static void drawOneFrame(MGEFF_ANIMATION handler, void *target, intptr_t id, void *value)
{
    mWidget *self = (mWidget*)target;
    UpdateWindow(self->hwnd, TRUE);
}

static void buttonAnimCb(MGEFF_ANIMATION handle, void *target, intptr_t id, void *value)
{
    static char str[5];
    mPanelPiece *self = (mPanelPiece*)target;
    RECT *prc = (RECT*)value;
    _c(self)->setRect(self, prc);
    snprintf(str, 5, "%d%%", RECTWP(prc) / 4);
    _c(g_txt_piece)->setProperty(g_txt_piece, NCSP_LABELPIECE_LABEL, (DWORD)str);
}

static MGEFF_ANIMATION buttonAnimationCreate(mHotPiece *self, RECT *startValue, RECT *endValue)
{
    MGEFF_ANIMATION anim = mGEffAnimationCreate(self, buttonAnimCb, 0, MGEFF_RECT);
    mGEffAnimationSetStartValue(anim, startValue);
    mGEffAnimationSetEndValue(anim, endValue);
    mGEffAnimationSetDuration(anim, ANIMATION_RUNTIME);
    mGEffAnimationSetCurve(anim, Linear);
    return anim;
}

static BOOL onButtonClicked(mButtonPanelPiece *self, mHotPiece *piece,
        int event_id, DWORD param)
{
    if (event_id == NCSN_ABP_CLICKED) {
        MGEFF_ANIMATION anim, draw_anim, group;
        RECT rc_s, rc_e;

        _c(self)->getRect(self, &rc_s);
        if (!g_buttonRunIsOn) {
            SetRect(&rc_e, 0, 0, 400, 50);
            g_buttonRunIsOn = TRUE;
        } else {
            SetRect(&rc_e, 0, 0, 200, 50);
            g_buttonRunIsOn = FALSE;
        }

        anim = buttonAnimationCreate((mHotPiece*)self, &rc_s, &rc_e);
        draw_anim = mGEffAnimationCreate((void *)param, drawOneFrame, 1, MGEFF_INT);
        mGEffAnimationSetDuration(draw_anim, ANIMATION_RUNTIME);

        group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

        mGEffAnimationAddToGroup(group, anim);
        mGEffAnimationAddToGroup(group, draw_anim);
        mGEffAnimationSyncRun(group);
        mGEffAnimationDelete(group);
        return TRUE;
    }
    return FALSE;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;

    /* button with image and text in layout */
    {
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, 20, 200, 50,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, 200, 50};
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(body)->setRect(body, &rc);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        mHotPiece *img_piece = (mHotPiece*)NEWPIECE(mImagePiece);
        static BITMAP bmp_left;
        LoadBitmap(HDC_SCREEN, &bmp_left, "./news.png");
        rc.right = bmp_left.bmWidth;
        rc.bottom = bmp_left.bmHeight;
        _c(img_piece)->setRect(img_piece, &rc);
        _c(img_piece)->setProperty(img_piece, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp_left);

        mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
        SetRect(&rc, 0, 0, 50, 20);
        _c(txt_piece)->setRect(txt_piece, &rc);
        _c(txt_piece)->setProperty(txt_piece,
                NCSP_LABELPIECE_LABEL, (DWORD)"mButtonPanel");

        btn_with_layout = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);

        mHotPiece *background = (mHotPiece*)_c(btn_with_layout)->getBkgndPiece(btn_with_layout);
        _c(btn_with_layout)->setStateTable(btn_with_layout,
                (mHotPiece*)background, NCSP_TRANROUND_BKCOLOR, (DWORD)0xFF0000FF, (DWORD)0xFF00FF00);

        _c(btn_with_layout)->addContentToLayout(btn_with_layout, img_piece);
        _c(btn_with_layout)->addContentToLayout(btn_with_layout, txt_piece);

        SetRect(&rc, 0, 0, 200, 50);
        _c(btn_with_layout)->setRect(btn_with_layout, &rc);

        _c(btn_with_layout)->setStates(btn_with_layout);

        _c(body)->addContent(body, (mHotPiece*)btn_with_layout, 0, 0);
    }

    /* button with colorful background and state change */
    {
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, 110, 200, 50,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, 200, 50};
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(body)->setRect(body, &rc);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
        SetRect(&rc, 0, 0, 200, 20);
        _c(txt_piece)->setRect(txt_piece, &rc);

        btn_with_gradient_bgcolor = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);

        _c(btn_with_gradient_bgcolor)->setStateTable(btn_with_gradient_bgcolor,
                txt_piece, NCSP_LABELPIECE_LABEL, (DWORD)"normal", (DWORD)"pushed");

        _c(btn_with_gradient_bgcolor)->addContentToLayout(btn_with_gradient_bgcolor, txt_piece);

        SetRect(&rc, 0, 0, 200, 50);
        _c(btn_with_gradient_bgcolor)->setRect(btn_with_gradient_bgcolor, &rc);

        ARGB colors0[] = {0xfffdc972, 0xfffcb848, 0xffba3305, 0xffd25e39};
        float positions0[] = {0.0, 0.03, 0.98, 1.0};
        ARGB colors1[] = {0x3fff0000, 0x7f00ff00, 0xaf003fff, 0xff0000ff};
        _c(btn_with_gradient_bgcolor)->setGradientBackgroundColor(btn_with_gradient_bgcolor,
                colors0, positions0, 4,
                colors1, NULL, 4);
        mHotPiece *bkgnd = (mHotPiece*)_c(btn_with_gradient_bgcolor)->getBkgndPiece(btn_with_gradient_bgcolor);
        _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_SHARPFLAG,
                TRANROUND_CORNERFLAG_LEFTTOP | TRANROUND_CORNERFLAG_LEFTBOTTOM);

        _c(btn_with_gradient_bgcolor)->setStates(btn_with_gradient_bgcolor);

        _c(body)->addContent(body, (mHotPiece*)btn_with_gradient_bgcolor, 0, 0);
    }

    /* buttons with animation */
    {
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, 200, 400, 50,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, 400, 50};
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(body)->setRect(body, &rc);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);
 
        btn_with_anim_run  = (mButtonPanelPiece*)NEWPIECEEX(mButtonPanelPiece, (DWORD)ctnr);

        g_txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
        SetRect(&rc, 0, 0, 40, 20);
        _c(g_txt_piece)->setRect(g_txt_piece, &rc);
        _c(g_txt_piece)->setProperty(g_txt_piece, NCSP_LABELPIECE_LABEL, (DWORD)"50%");
        _c(btn_with_anim_run)->addContentToLayout(btn_with_anim_run, g_txt_piece);

        SetRect(&rc, 0, 0, 200, 50);
        _c(btn_with_anim_run)->setRect(btn_with_anim_run, &rc);
        ncsAddEventListener((mObject*)btn_with_anim_run, (mObject*)btn_with_anim_run,
                (NCS_CB_ONPIECEEVENT)onButtonClicked, NCSN_ABP_CLICKED);

        ARGB colors0[] = {0xfffdc972, 0xfffcb848, 0xffba3305, 0xffd25e39};
        float positions0[] = {0.0, 0.03, 0.98, 1.0};
        ARGB colors1[] = {0x3fff0000, 0x7f00ff00, 0xaf003fff, 0xff0000ff};
        _c(btn_with_anim_run)->setGradientBackgroundColor(btn_with_anim_run,
                colors0, positions0, 4,
                colors1, NULL, 4);

        _c(btn_with_anim_run)->setStates(btn_with_anim_run);

        _c(body)->addContent(body, (mHotPiece*)btn_with_anim_run, 0, 0);
    }

    /* piece in layout and piece out of layout */
    {
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, 290, 400, 50,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, 400, 50};
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(body)->setRect(body, &rc);

        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        int i;
        static char str[10][4];
        mHotPiece *txt_piece;
        mHotPiece *bkgnd;
        btn_layout_or_not = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
        bkgnd = (mHotPiece*)_c(btn_layout_or_not)->getBkgndPiece(btn_layout_or_not);
        _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_BKCOLOR, 0xFFFF0000);

        for (i = 0; i < 10; ++i) {
            snprintf(str[i], 4, "(%d)", i);
            txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
            SetRect(&rc, 0, 0, 40, 20);
            _c(txt_piece)->setRect(txt_piece, &rc);
            _c(txt_piece)->setProperty(txt_piece, NCSP_LABELPIECE_LABEL, (DWORD)str[i]);
            if (i == 0)
                _c(btn_layout_or_not)->addContent(btn_layout_or_not, txt_piece, 0, 0);
            else
                _c(btn_layout_or_not)->addContentToLayout(btn_layout_or_not, txt_piece);
        }

        SetRect(&rc, 0, 0, 400, 50);
        _c(btn_layout_or_not)->setRect(btn_layout_or_not, &rc);
        _c(body)->addContent(body, (mHotPiece*)btn_layout_or_not, 0, 0);

    }

    return TRUE;
}

static void mymain_onPaint(mWidget *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    SetBkMode(hdc, BM_TRANSPARENT);
    TextOut(hdc, 0, 0, "Button panel with image piece and label piece");
    TextOut(hdc, 0, 90, "Button panel with colorful background");
    TextOut(hdc, 0, 180, "Button panel with piece animation");
    TextOut(hdc, 0, 270, "Button panel with in-layout pieces and out-of-layout piece");
}

#if 0
static BOOL mymain_onlbuttonup(mMainWnd* self, int message, int lParam, int wParam, DWORD addData)
{
    return 0;
}
#endif

static BOOL mymain_onClose(mWidget* self, int message)
{   
    DestroyMainWindow(self->hwnd);
    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {MSG_PAINT, mymain_onPaint   },
    {MSG_CLOSE, mymain_onClose   },
//    {MSG_LBUTTONUP, mymain_onlbuttonup},
    {0, NULL}
};

#include "register.h"

int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mButtonPanelPiece" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();


    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "mButtonPanelPiece test",
            WS_CAPTION | WS_BORDER | WS_VISIBLE,
            WS_EX_AUTOSECONDARYDC,
            1,
            0, 0, g_rcScr.right,g_rcScr.bottom,
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

    MainWindowThreadCleanup(mymain->hwnd);

    mGEffDeinit();
    ncsUninitialize();

    return 0;
}
