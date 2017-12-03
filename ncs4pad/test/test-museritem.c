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

#include "museritem.h"

//#include "register.h"

static BITMAP bmp;
static int curx = 25;
static int cury = 25;
static mItemPiece *smsitem;
static mItemPiece *smscontent;
static int item_w = ACTIVITY_W + 1;
static int flag = 0;
static char sms[2][255]=
{
"this is a text    ",
"this is a text       "
"aaaaaaaaaaaaaa      c      eeeeeee"
"eeeeeeeeeeeeeeee aaaaaaaaaaaa xxxxxxxx"
"    xxxxxx  ccccccc"
};

BOOL itemsetrect ( HWND hwnd, int id, DWORD data)
{
    RECT rect;

    if (flag == 0) {
        if (item_w > ACTIVITY_W) {
            flag = 1;
        }

        item_w++;
        _c(smscontent)->setSubStance (smscontent,
                sms[0],NULL, NULL);
    }
    else {
        if (item_w < 260) {
            flag = 0;
        }

        item_w--;
        _c(smscontent)->setSubStance (smscontent,
                sms[1],NULL, NULL);
    }

    SetRect (&rect, 0, 0, item_w, 50); 
//    printf ("item_w: %d\n", item_w);
#if 0
//    UpdateWindow (hwnd, FALSE);
    HDC hdc = GetClientDC (hwnd);
    SetBrushColor (hdc, 0xFF00FF00);
    FillBox (hdc, 0, 0, 1024, 768);
    //SetRect (&rect, 0, 0, ACTIVITY_W, 50); 

    //printf ("item_w: %d\n", rect.w);
    ReleaseDC(hdc);
    UpdateWindow (hwnd, TRUE);
#endif
//    _c(smsitem)->setItemRect (smsitem, &rect);
    _c(smsitem)->setRect (smsitem, &rect);
    UpdateWindow (hwnd, TRUE);
    return TRUE;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    LoadBitmap(HDC_SCREEN, &bmp, "./file.png");

    curx = 0;
    cury = 0;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateEditItem ("Clock", "12:12");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSwitchCtrlItem ("switch on", NULL, TRUE);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSwitchCtrlItem ("switch off this is long title of test", NULL, FALSE);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateIndicatorItem ("CreateIndicatorItem", "12:12", "btn_text");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow (NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateCheckBoxItem ("CreateCheckBoxItem Left", NULL, TRUE, NCS_ALIGN_LEFT);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateCheckBoxItem ("CreateCheckBoxItem Right", NULL, FALSE, NCS_ALIGN_RIGHT);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow (NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateContactItem (&bmp, "This is a Contact", "phone number");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSongItem ("name", "song name", "12:12");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateAlbumItem (&bmp, "album", "name");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSMSItem ("CreateSMSItem", "sms", "12:12");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    curx = 380;
    cury = 0;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateEditItem ("CreateEditItem", "edit");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 80,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSMSContentItem (&bmp,
            "SMSContent Left. hshahahahha e cde this is a test of smscontent align is left",
            "12:12 pm", NCS_ALIGN_LEFT);

    SetWindowBkColor (ctnr->hwnd, PIXEL_cyan); 

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 85;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 90,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSMSContentItem (&bmp,
            "SMSContent Right.tiiiiiiiiiiiiiiiiiiiiiihis is a tsmscontent a iiiiiiii000000  a  ge iiiiiiiiiiig e ge g iiiiiiiiiiie fe ge ge ge g eg e ge g eg  geg eg e   lign is right",
            "12:12 AM", NCS_ALIGN_RIGHT);

    smscontent = item;

    SetWindowBkColor (ctnr->hwnd, PIXEL_cyan); 

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 95;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateDetailBtnItem ("DetailBtn", "btn_text", TRUE);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateDetailBtnItem ("DetailBtn", "btn_text", FALSE);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateButtonItem ("Button", "btn_text");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateRadioItem ("CreateRadioItem Left", NULL, TRUE, 0, NCS_ALIGN_LEFT);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateRadioItem ("CreateRadioItem Right", NULL, FALSE, 0, NCS_ALIGN_RIGHT);

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;

    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSMSItem ("CreateSMSItem", "sms", "12:12");

    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    cury += 55;
    {
    mContainerCtrl *ctnr;

    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            curx, cury, ACTIVITY_W, 50,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mItemPiece *item = CreateSMSItem ("CreateSMSItem", "sms", "12:12");

    smsitem = item;
    _c(ctnr)->setBody(ctnr, (mHotPiece *)item);
    }

    SetTimerEx (self->hwnd, 100, 5, itemsetrect);

    return TRUE;
}

static void mymain_onPaint(mWidget *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    SetBkMode(hdc, BM_TRANSPARENT);
    TextOut(hdc, 0, 0, "=w=");
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
