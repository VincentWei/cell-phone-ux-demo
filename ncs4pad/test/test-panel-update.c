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

#define ACTIVITY_W  360
#define ACTIVITY_H  480

#define WINDOW_W    ACTIVITY_W
#define WINDOW_H    ACTIVITY_H

#define BUBBLE_W   56
#define BUBBLE_H   56
#define BUBBLE_MAX_W   66
#define BUBBLE_MAX_H    66

#define BUBBLE_NORMAL2MAX_DURATION     200
#define BUBBLE_MAX2NORMAL_DURATION     200

#define bubble_img_path "res/lockscreen/bubble_btn1.png"

enum {
    P_LABEL1 = 0,
    P_BUBBLE1,

    P_NUM
};

static PBITMAP s_bubbleBmp;
static mPanelPiece* s_panel;
static mHotPiece* s_piece[P_NUM];
static int s_piece_xy[][2] =
{
    {0, 10},
    {80, 80},
};

static BOOL s_roll_flag = TRUE;

static void rollLabel(void);
static void scaleObj(int id, BOOL isInflate);

static void loadMyResource(void)
{
    s_bubbleBmp = (PBITMAP) LoadResource(bubble_img_path, RES_TYPE_IMAGE, 0);
    mGEffInit();
}

static void unloadMyResource(void)
{
    ReleaseRes(Str2Key(bubble_img_path));
    mGEffDeinit();
}

static void rollLabel_finished_cb(MGEFF_ANIMATION anim)
{
    if (s_roll_flag) rollLabel();
}

static void scale_finished_cb(MGEFF_ANIMATION anim)
{
    scaleObj(P_BUBBLE1, FALSE);
}

static void rollLabel()
{
    static const int dest = WINDOW_W-150;
    mHotPiece* piece = s_piece[P_LABEL1];
    MGEFF_ANIMATION anim;

    // reset position
    _c(s_panel)->movePiece(s_panel, piece, s_piece_xy[P_LABEL1][0], s_piece_xy[P_LABEL1][1]);
    PanelPiece_update(piece, TRUE);

    // run animation
    if (s_piece_xy[P_LABEL1][0] >= dest)
        s_piece_xy[P_LABEL1][0] = 0;
    else
        s_piece_xy[P_LABEL1][0] = dest;
    anim = _c(s_panel)->movePieceWithAnimation(
            s_panel, piece,
            s_piece_xy[P_LABEL1][0], s_piece_xy[P_LABEL1][1], 800, Linear);
    mGEffAnimationSetFinishedCb(anim, rollLabel_finished_cb);

    _c(s_panel)->animationAsyncRun(s_panel, anim, 0);
}

static void moveObj(int id, int offset_x, int offset_y)
{
    mHotPiece* piece = s_piece[id];
    MGEFF_ANIMATION anim;

    // get new xy
    s_piece_xy[id][0] += offset_x;
    s_piece_xy[id][1] += offset_y;

    // run animation
    anim = _c(s_panel)->movePieceWithAnimation(
            s_panel, piece,
            s_piece_xy[id][0], s_piece_xy[id][1], 300, Linear);

    _c(s_panel)->animationAsyncRun(s_panel, anim, 0);
}

static void scaleObj(int id, BOOL isInflate)
{
    mHotPiece* piece = s_piece[id];
    float scalew, scaleh;
    MGEFF_ANIMATION anim;

    // get scale factor
    if (isInflate) {
        scalew = (float)BUBBLE_MAX_W/BUBBLE_W;
        scaleh = (float)BUBBLE_MAX_H/BUBBLE_H;
    }
    else {
        scalew = (float)BUBBLE_W/BUBBLE_MAX_W;
        scaleh = (float)BUBBLE_H/BUBBLE_MAX_H;
    }

    // run animation
    anim = _c(s_panel)->scalePieceWithAnimation(
            s_panel, piece,
            scalew, scaleh,
            BUBBLE_NORMAL2MAX_DURATION, InQuad);

    if (isInflate)
        mGEffAnimationSetFinishedCb(anim, scale_finished_cb);

    _c(s_panel)->animationAsyncRun(s_panel, anim, 0);
}

static BOOL onkeydown(mWidget *widget, int message, int code, DWORD keyStatus) 
{
    const int offset_x = 20;
    const int offset_y = 20;

    switch (code) {
        case SCANCODE_H:
            printf("h\n");
            moveObj(P_BUBBLE1, -offset_x, 0);
            break;
        case SCANCODE_L:
            printf("l\n");
            moveObj(P_BUBBLE1, offset_x, 0);
            break;
        case SCANCODE_J:
            printf("j\n");
            moveObj(P_BUBBLE1, 0, offset_y);
            break;
        case SCANCODE_K:
            printf("k\n");
            moveObj(P_BUBBLE1, 0, -offset_y);
            break;
        case SCANCODE_SPACE:
            printf("space\n");
            scaleObj(P_BUBBLE1, TRUE);
            break;
        case SCANCODE_ENTER:
            printf("Enter\n");
            s_roll_flag = ! s_roll_flag;
            if (s_roll_flag) rollLabel();
            break;
        default:
            return FALSE;
    }
    InvalidateRect(widget->hwnd, NULL, TRUE);
    return TRUE;
}

static NCS_EVENT_HANDLER g_handles[] = {
    { MSG_KEYDOWN, onkeydown },
    { 0, NULL }
};

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctrl;
    mPanelPiece *panel;
    RECT rc;
    mHotPiece *label1;
    mHotPiece *img1;
    int i;

    loadMyResource();

    GetClientRect(self->hwnd, &rc);
    ctrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            0, 0, WINDOW_W, WINDOW_H,
            self->hwnd,
            NULL, NULL, NULL, 0);

    SetRect(&rc, 0, 0, WINDOW_W, WINDOW_H);

    panel = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(panel)->setRect(panel, &rc);
    _c(ctrl)->setBody(ctrl, (mHotPiece*)panel);

    label1 = (mHotPiece*)NEWPIECE(mLabelPiece);
    img1 = (mHotPiece*)NEWPIECE(mImagePiece);
    
    rc.right = 150;
    rc.bottom = 50;
    _c(label1)->setRect (label1, &rc);
    rc.right = s_bubbleBmp->bmWidth;
    rc.bottom = s_bubbleBmp->bmHeight;
    _c(img1)->setRect(img1, &rc);

    _c(label1)->setProperty(label1, NCSP_LABELPIECE_LABEL, (DWORD)"panel piece update testing");
    _c(img1)->setProperty(img1, NCSP_IMAGEPIECE_IMAGE, (DWORD)s_bubbleBmp);

    s_panel = panel;
    s_piece[P_LABEL1] = label1;
    s_piece[P_BUBBLE1] = img1;
    
    for (i = 0; i < P_NUM; ++i)
        _c(s_panel)->addContent(s_panel, s_piece[i], s_piece_xy[i][0], s_piece_xy[i][1]);


    ncsSetComponentHandlers((mComponent *)ctrl, g_handles, -1);

    rollLabel();

    return TRUE;
}

static void mymain_onlbuttonup(mMainWnd* self, int message, int lParam, int wParam, DWORD addData)
{
    puts("lbuttonup\n");
    UpdateWindow(self->hwnd, TRUE);
}

static BOOL mymain_onClose(mWidget* self, int message)
{   
    unloadMyResource();
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

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "panel piece update testing",
            WS_VISIBLE,
            WS_EX_NONE,//WS_EX_AUTOSECONDARYDC,
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
