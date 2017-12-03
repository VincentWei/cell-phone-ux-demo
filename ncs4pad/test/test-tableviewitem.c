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

#define EDIT_W 400
#define EDIT_H 150
#define ITEM_W EDIT_W
#define ITEM_H 50
static mTableViewItemPiece *itemPiece1;
static BOOL onPieceClicked(mHotPiece *self, mHotPiece *sender, int event_id, DWORD param)
{
    if (event_id == NCSN_TABLEVIEWITEMPIECE_DELBTNCLICKED) {
        printf("self %p -- NCSN_TABLEVIEWITEMPIECE_DELETECLICKED\n", self);
        return TRUE;
    } else if (event_id == NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED) {
        printf("self %p -- NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED\n", self);
        return TRUE;
    } else if (event_id == NCSN_SLIDESWITCHPIECE_ON) {
        _c(itemPiece1)->changeMode(itemPiece1);
        printf("self %p -- NCSN_SLIDESWITCHPIECE_ON\n", self);
        return TRUE;
    } else if (event_id == NCSN_SLIDESWITCHPIECE_OFF) {
        _c(itemPiece1)->changeMode(itemPiece1);
        printf("self %p -- NCSN_SLIDESWITCHPIECE_OFF\n", self);
        return TRUE;
    }
    return FALSE;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    /* create control button */
    {
        mContainerCtrl *ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "TitleBar",
                WS_VISIBLE, 0, 0,
                0, 0, ITEM_W, ITEM_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc;

        mPanelPiece *body       = NEWPIECE(mPanelPiece);
        mShapeTransRoundPiece * body_bk  = NEWPIECE(mShapeTransRoundPiece);
        mHotPiece *body_txt    = (mHotPiece*)NEWPIECE(mLabelPiece);

        /* body_bk */
        SetRect(&rc, 0, 0, ITEM_W, ITEM_H);
        _c(body_bk)->setRect(body_bk, &rc);
        _c(body_bk)->setProperty(body_bk, NCSP_TRANROUND_RADIUS, 0);
        _c(body_bk)->setProperty(body_bk, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        ARGB colors[] = {0xffb1bdcb, 0xff889ab2, 0xff8293ad, 0xff76859a};
        float pos[] = {0.0, 0.49, 0.50, 1.0};
        HBRUSH brush = _c(body_bk)->getBrush(body_bk);
        MGPlusSetLinearGradientBrushColorsEx(brush, colors, TABLESIZE(colors), pos);
        _c(body)->addContent(body, (mHotPiece*)body_bk, 0, 0);

        /* body_txt */
        SetRect(&rc, 0, 0, 100, 40);
        _c(body_txt)->setRect(body_txt, &rc);
        _c(body_txt)->setProperty(body_txt, NCSP_LABELPIECE_LABEL, (DWORD)"Core Team");
        _c(body)->addContentToLayout(body, body_txt);

        mSlideSwitchPiece *ss = NEWPIECE(mSlideSwitchPiece);
        SetRect(&rc, 0, 0, 80, 40);
        _c(ss)->setRect(ss, &rc);
        _c(body)->addContent(body, (mHotPiece*)ss, 10, 5);

        int event_ids[] = {
            NCSN_SLIDESWITCHPIECE_OFF,
            NCSN_SLIDESWITCHPIECE_ON,
            0};
        ncsAddEventListeners((mObject*)ss, (mObject*)ss,
                (NCS_CB_ONPIECEEVENT)onPieceClicked, event_ids);

        /* body */
        SetRect(&rc, 0, 0, ITEM_W, ITEM_H);
        _c(body)->setRect(body, &rc);
        _c(body)->setLayoutManager(body, (mLayoutManager*)NEW(mCenterHBox));
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);
    }

    /* create the first item */
    {
        mContainerCtrl *ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, ITEM_H, ITEM_W, ITEM_H * 3 + 2,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, ITEM_W, ITEM_H * 3 + 2};
        /* body */
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);
        _c(body)->setRect(body, &rc);
        _c(body)->setLayoutManager(body, (mLayoutManager*)NEW(mLineVBox));

        SetRect(&rc, 0, 0, ITEM_W, ITEM_H);

        /* item */
        itemPiece1 = NEWPIECE(mTableViewItemPiece);
        _c(itemPiece1)->setRect(itemPiece1, &rc);
        mHotPiece *bk = (mHotPiece*)_c(itemPiece1)->getBkgndPiece(itemPiece1);
        _c(bk)->setProperty(bk, NCSP_TRANROUND_BKCOLOR, 0xFFFFFFFF);

        {
            /* image piece */
            mHotPiece *img_piece = (mHotPiece*)NEWPIECE(mImagePiece);
            static BITMAP bmp_left;
            LoadBitmap(HDC_SCREEN, &bmp_left, "./news.png");
            rc.right = bmp_left.bmWidth;
            rc.bottom = bmp_left.bmHeight;
            _c(img_piece)->setRect(img_piece, &rc);
            _c(img_piece)->setProperty(img_piece, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp_left);

            /* label piece */
            mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
            SetRect(&rc, 0, 0, 60, 20);
            _c(txt_piece)->setRect(txt_piece, &rc);
            _c(txt_piece)->setProperty(txt_piece,
                    NCSP_LABELPIECE_LABEL, (DWORD)"(3__3) hey boy!");

            /* userPiece */
            mButtonPanelPiece *userPiece = NEWPIECE(mButtonPanelPiece);
            mHotPiece *bk = (mHotPiece*)_c(userPiece)->getBkgndPiece(userPiece);
            _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, 0);
            _c(userPiece)->setStateTable(userPiece, bk, NCSP_TRANROUND_BKCOLOR, 0x00000000, 0xff7b68ee);
            _c(userPiece)->setStates(userPiece);
            SetRect(&rc, 0, 0, ITEM_W, ITEM_H);
            _c(userPiece)->addContentToLayout(userPiece, img_piece);
            _c(userPiece)->addContentToLayout(userPiece, txt_piece);
            _c(userPiece)->setRect(userPiece, &rc);
            _c(itemPiece1)->setUserPiece(itemPiece1, (mHotPiece*)userPiece);

        }

        _c(body)->addContentToLayout(body, (mHotPiece*)itemPiece1);

        int event_ids[] = {
            NCSN_TABLEVIEWITEMPIECE_DELBTNCLICKED,
            NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED,
            0};
        ncsAddEventListeners((mObject*)itemPiece1, (mObject*)itemPiece1,
                (NCS_CB_ONPIECEEVENT)onPieceClicked, event_ids);

        _c(body)->reLayout(body);
    }

    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {0, NULL}
};

int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mTableViewItemPiece" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "mTableViewItemPiece test",
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
