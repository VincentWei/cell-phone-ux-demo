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

#include "global.h"
#include "register.h"

#include "trans-activity.hh"

REGISTER_ACTIVITY(TransActivity);

#define IDC_NAV1    200
#define IDC_NEWTRKBAR   201

#define CTRL_TRKBAR_W 220
#define CTRL_TRKBAR_H 17 

#if 0
static NCS_PROP_ENTRY trk_props [] = {
    {NCSP_TRKBAR_MINPOS, 0},
    {NCSP_TRKBAR_MAXPOS, CTRL_TRKBAR_W},
    {NCSP_TRKBAR_CURPOS, 0},
    {NCSP_TRKBAR_LINESTEP, 1},
    {NCSP_TRKBAR_PAGESTEP, CTRL_TRKBAR_W},
    {0, 0},
};

static NCS_WND_TEMPLATE hosted_tmpl[] = {
    {
        NCSCTRL_NEWTRACKBAR,
        IDC_NEWTRKBAR,
        0, 0, CTRL_TRKBAR_W, CTRL_TRKBAR_H,
        WS_VISIBLE | NCSS_NOTIFY | NCSS_NTRKBAR_CIRCLE,
        /*WS_EX_TRANSPARENT | */WS_EX_TROUNDCNS | WS_EX_BROUNDCNS,
        "",
        trk_props, /*track_rdr_info*/NULL, NULL, NULL, 
        0, 0,
    },
};
#endif

static BOOL main_onCreate(mWidget* self, DWORD dwAddData)
{
    mContainerCtrl *ct;
    //mWidgetHostPiece *trkbar;
    //mTrackBar *bar;
    mPanelPiece  *body;
    //RECT trkbar_rc = {0, 0, CTRL_TRKBAR_W, 17};
    RECT rc;
    mShapeTransRoundPiece* bk;
    mShapeTransRoundPiece* btn;
    ARGB bkcolor[] = {0xffd0d571, 0xffb5c363};

    ct = (mContainerCtrl*)ncsGetChildObj(self->hwnd, IDC_NAV1);

    SetRect(&rc, 0, 0, ACTIVITY_W, ACTIVITY_H);
    body = NEWPIECE(mPanelPiece);
    _c(body)->setRect(body, &rc);
    _c(ct)->setBody(ct, (mHotPiece*)body);

    bk = NEWPIECE(mShapeTransRoundPiece);
    _c(bk)->setRect(bk, &rc);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_BKCOLOR, 0xffffff00);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, 9);
    _c(body)->addContent(body, (mHotPiece*)bk, 0, 0);

    SetRect(&rc, 0, 0, 90, 27);
    btn = NEWPIECE(mShapeTransRoundPiece);
    _c(btn)->setRect(btn, &rc);
    _c(btn)->setProperty(btn, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    _c(btn)->setGradientFillColors(btn, bkcolor, TABLESIZE(bkcolor));
    //_c(btn)->setProperty(btn, NCSP_TRANROUND_BKCOLOR, 0xff00ffff);
    _c(btn)->setProperty(btn, NCSP_TRANROUND_BORDERSIZE, 2);
    //_c(btn)->setProperty(btn, NCSP_TRANROUND_BORDERCOLOR, 0xff00ffff);
    //_c(bk)->setProperty(bk, NCSP_TRANROUND_CORNERFLAG, 0x9);
    _c(btn)->setProperty(btn, NCSP_TRANROUND_RADIUS, 4);
    _c(btn)->setProperty(btn, NCSP_TRANROUND_SHARPFLAG, TRANROUND_SHARPFLAG_LEFT);
    //_c(btn)->setProperty(btn, NCSP_TRANROUND_SHARPFLAG, TRANROUND_SHARPFLAG_LEFT|TRANROUND_SHARPFLAG_RIGHT);
    _c(btn)->setProperty(btn, NCSP_TRANROUND_SHARPWIDTH, 10);
    _c(body)->addContent(body, (mHotPiece*)btn, 50, 50);

#if 0
    trkbar = NEWPIECEEX(mWidgetHostPiece,self->hwnd);
    _c(trkbar)->setRect(trkbar, &trkbar_rc);
    _c(trkbar)->setHosted(trkbar, hosted_tmpl);
    _c(body)->addContent(body, (mHotPiece*)trkbar, 50,50);
    bar = (mTrackBar*)_c(trkbar)->getHosted(trkbar);
    _c(bar)->setProperty(bar, NCSP_TRKBAR_THUMB_WIDTH, 16);
    _c(bar)->setProperty(bar, NCSP_TRKBAR_THUMB_HEIGHT, 16);
#endif

    return TRUE;
}

static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE, reinterpret_cast<void*>(main_onCreate)},
    {0, NULL}
};

static NCS_WND_TEMPLATE _ctrl_templ[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        IDC_NAV1,
        0, 0, ACTIVITY_W, ACTIVITY_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL, NULL,
        NULL,
        NULL, 0, 0
    },
};
static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_AUTOSECONDARYDC,
        "trans-activity",
        NULL,
        NULL,
        main_handlers,
        _ctrl_templ,
        TABLESIZE(_ctrl_templ),
        0,
        0, 0,
};

TransActivity::TransActivity()
    :NCSActivity(&mymain_templ){
}

TransActivity::~TransActivity() {
}





