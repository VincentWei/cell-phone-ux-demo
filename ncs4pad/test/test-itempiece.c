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

#include "register.h"

#define ITEM_H 100
#define ITEM_W 500
#define CLEARANCE 20

static char *title_text = "Hi, I'm the title.";
static char *subTitle_text = "I'm the BLUE sub title";
static char *detail_text = "Detail";
static BITMAP bmp;
static mIndicatorButtonPieceImages indicator_bmps;

static BOOL onClicked(mContainerCtrl *self, mHotPiece *piece,
        int event_id, DWORD param)
{
    if (event_id == NCSN_ITEMPIECE_CONTENT_CLICKED) {
        printf("test-itempiece %s --- %p NCSN_ITEMPIECE_CONTENT_CLICKED\n",  __FUNCTION__, piece);
        return TRUE;
    } else if (event_id == NCSN_ITEMPIECE_BUTTON_CLICKED) {
        printf("test-itempiece %s --- %p NCSN_ITEMPIECE_BUTTON_CLICKED\n", __FUNCTION__, piece);
        return TRUE;
    } else if (event_id == NCSN_ITEMPIECE_CHECKBOX_ON) {
        printf("test-itempiece %s --- %p NCSN_ITEMPIECE_CHECKBOX_ON\n",  __FUNCTION__, piece);
        return TRUE;
    } else if (event_id == NCSN_ITEMPIECE_CHECKBOX_OFF) {
        printf("test-itempiece %s --- %p NCSN_ITEMPIECE_CHECKBOX_OFF\n",  __FUNCTION__, piece);
        return TRUE;
    }
    return FALSE;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    int curItemY = CLEARANCE;
    LoadBitmap(HDC_SCREEN, &bmp, "./file.png");
#if 0
    LoadBitmap(HDC_SCREEN, &indicator_bmps.bmp[0], "./indicator_green_normal.png");
    LoadBitmap(HDC_SCREEN, &indicator_bmps.bmp[1], "./indicator_green_click.png");
#else
    indicator_bmps.bmp[0] =
        (BITMAP*)LoadResource("./indicator_green_normal.png",RES_TYPE_IMAGE,(DWORD)HDC_SCREEN);
    indicator_bmps.bmp[1] =
        (BITMAP*)LoadResource("./indicator_green_click.png",RES_TYPE_IMAGE,(DWORD)HDC_SCREEN);
#endif

    /* 1 */
    {
        mContainerCtrl *ctnr;
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, curItemY, ITEM_W, ITEM_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, ITEM_W, ITEM_H};
        mItemPiece *body = NEWPIECE(mItemPiece);

        _c(body)->setLayout(body, NCS_ALIGN_LEFT);
        _c(body)->setImage(body, &bmp, -1, -1);
        _c(body)->setTitle(body, title_text, NULL, NULL);
        _c(body)->setSubTitle(body, subTitle_text, NULL, NULL);
        _c(body)->setLayout(body, NCS_ALIGN_RIGHT);
        _c(body)->setAccessoryControl(body, NULL, TRUE, ITEMPIECE_AC_SWITCH, 0);
        _c(body)->setDetail(body, detail_text, NULL, NULL);
        _c(body)->setRect(body, &rc);

        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        int event_ids[] = {NCSN_ITEMPIECE_CONTENT_CLICKED, NCSN_ITEMPIECE_CHECKBOX_ON, NCSN_ITEMPIECE_CHECKBOX_OFF, 0};
        ncsAddEventListeners((mObject*)body, (mObject*)ctnr,
                (NCS_CB_ONPIECEEVENT)onClicked, event_ids);
    }

    curItemY += ITEM_H + CLEARANCE;

    /* 2 */
    {
        mContainerCtrl *ctnr;
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, curItemY, ITEM_W, ITEM_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, ITEM_W, ITEM_H};
        mItemPiece *body = NEWPIECE(mItemPiece);
        _c(body)->setRect(body, &rc);
        _c(body)->setProperty(body, NCSP_ITEMPIECE_VALIGN, NCS_VALIGN_TOP);
        _c(body)->setLayout(body, NCS_ALIGN_LEFT);
        _c(body)->setTitle(body, title_text, NULL, NULL);
        _c(body)->setLayout(body, NCS_ALIGN_RIGHT);
        _c(body)->setImage(body, &bmp, -1, -1);
        _c(body)->activeLayout(body);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        int event_ids[] = {NCSN_ITEMPIECE_CONTENT_CLICKED, NCSN_ITEMPIECE_CHECKBOX_ON, NCSN_ITEMPIECE_CHECKBOX_OFF, 0};
        ncsAddEventListeners((mObject*)body, (mObject*)ctnr,
                (NCS_CB_ONPIECEEVENT)onClicked, event_ids);
    }

    curItemY += ITEM_H + CLEARANCE;

    /* 3 */
    {
        mContainerCtrl *ctnr;
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, curItemY, ITEM_W, ITEM_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, ITEM_W, ITEM_H};
        mItemPiece *body = NEWPIECE(mItemPiece);
        _c(body)->setRect(body, &rc);
        _c(body)->setLayout(body, NCS_ALIGN_LEFT);
        _c(body)->setAccessoryControl(body, NULL, TRUE, ITEMPIECE_AC_CHECKMARK, 0);
        _c(body)->setImage(body, &bmp, 80, -1);
        _c(body)->setTitle(body, title_text, NULL, NULL);
        _c(body)->setSubTitle(body, subTitle_text, NULL, NULL);
        _c(body)->setLayout(body, NCS_ALIGN_RIGHT);
        _c(body)->setDetail(body, detail_text, NULL, NULL);
        _c(body)->activeLayout(body);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        int event_ids[] = {NCSN_ITEMPIECE_CONTENT_CLICKED, NCSN_ITEMPIECE_CHECKBOX_ON, NCSN_ITEMPIECE_CHECKBOX_OFF, 0};
        ncsAddEventListeners((mObject*)body, (mObject*)ctnr,
                (NCS_CB_ONPIECEEVENT)onClicked, event_ids);
    }

    curItemY += ITEM_H + CLEARANCE;

    /* 4 */
    {
        mContainerCtrl *ctnr;
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, curItemY, ITEM_W, ITEM_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, ITEM_W, ITEM_H};
        mItemPiece *body = NEWPIECE(mItemPiece);
        _c(body)->setRect(body, &rc);
        _c(body)->setProperty(body, NCSP_ITEMPIECE_MARGIN, 5);
        _c(body)->setProperty(body, NCSP_ITEMPIECE_CLEARANCE, 5);
        _c(body)->setLayout(body, NCS_ALIGN_LEFT);
        _c(body)->setImage(body, &bmp, -1, -1);
        _c(body)->setTitle(body, title_text, NULL, NULL);
        _c(body)->setSubTitle(body, subTitle_text, NULL, NULL);
        _c(body)->setLayout(body, NCS_ALIGN_RIGHT);
        _c(body)->setAccessoryControl(body, NULL, TRUE, ITEMPIECE_AC_INDICATOR, 0);
        _c(body)->setDetail(body, detail_text, NULL, NULL);
        _c(body)->activeLayout(body);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        int event_ids[] = {NCSN_ITEMPIECE_CONTENT_CLICKED, NCSN_ITEMPIECE_CHECKBOX_ON, NCSN_ITEMPIECE_CHECKBOX_OFF, 0};
        ncsAddEventListeners((mObject*)body, (mObject*)ctnr,
                (NCS_CB_ONPIECEEVENT)onClicked, event_ids);
    }

    curItemY += ITEM_H + CLEARANCE;

    /* 5 */
    {
        mContainerCtrl *ctnr;
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, curItemY, ITEM_W, ITEM_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, ITEM_W, ITEM_H};
        mItemPiece *body = NEWPIECE(mItemPiece);
        _c(body)->setRect(body, &rc);
        _c(body)->setProperty(body, NCSP_ITEMPIECE_MARGIN, 5);
        _c(body)->setProperty(body, NCSP_ITEMPIECE_CLEARANCE, 5);
        _c(body)->setLayout(body, NCS_ALIGN_LEFT);
        _c(body)->setImage(body, &bmp, -1, -1);
        _c(body)->setTitle(body, title_text, NULL, NULL);
        _c(body)->setSubTitle(body, subTitle_text, NULL, NULL);
        _c(body)->setLayout(body, NCS_ALIGN_RIGHT);
        _c(body)->setAccessoryControl(body, NULL, TRUE, ITEMPIECE_AC_INDICATORBUTTON, (DWORD)&indicator_bmps);
        _c(body)->setDetail(body, detail_text, NULL, NULL);
        _c(body)->activeLayout(body);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        ncsAddEventListener((mObject*)body, (mObject*)ctnr,
                (NCS_CB_ONPIECEEVENT)onClicked, NCSN_ITEMPIECE_BUTTON_CLICKED);
    }

    return TRUE;
}

static void mymain_onPaint(mWidget *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    int curTextY = 0;
    SetBkMode(hdc, BM_TRANSPARENT);
    TextOut(hdc, 0, curTextY, "=w= itempiece with slideswitch");
    curTextY += CLEARANCE + ITEM_H;
    TextOut(hdc, 0, curTextY, "=w= itempiece with top text and top right image");
    curTextY += CLEARANCE + ITEM_H;
    TextOut(hdc, 0, curTextY, "=w= itempiece with scaled image (80 x bmHeight) and checkmark");
    curTextY += CLEARANCE + ITEM_H;
    TextOut(hdc, 0, curTextY, "=w= itempiece with margin (5) , clearance (5) and indicator");
    curTextY += CLEARANCE + ITEM_H;
    TextOut(hdc, 0, curTextY, "=w= itempiece with margin (5) , clearance (5) and indicatorButton");
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
    JoinLayer(NAME_DEF_LAYER , "mItemPiece" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "mItemPiece test",
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

    UNREGISTER_NCS();
    mGEffDeinit();
    ncsUninitialize();

    return 0;
}
