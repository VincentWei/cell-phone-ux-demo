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
#include <assert.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>
#include <mgncs4touch/mgncs4touch.h>
#include <mgplus/mgplus.h>
#include "register.h"

#define hostdbg
#ifdef hostdbg
#undef dbg
#define dbg() printf("[%s]:%d\n", __FUNCTION__, __LINE__)
#else 
#define dbg()
#endif

//ctrl id
#define IDC_CTRL_TRACKBAR	200
#define CH_IDC_CTNR			201
#define ID_DATEPICKER       202
#define ID_TIMEPICKER       203
#define IDC_BTN             204
#define IDC_LABEL           205
#define IDC_EDIT            206

//wnd
#define WND_X	0
#define WND_Y	0
#define WND_W	800
#define WND_H	600

//ctrl info
#define CTRL_TRKBAR_X	0
#define CTRL_TRKBAR_Y	0
#define CTRL_TRKBAR_W	280
#define CTRL_TRKBAR_H	17

#define DATE_X    0
#define DATE_Y    0
#define TIME_X    0
#define TIME_Y    0
#define PICKER_W    400
#define PICKER_H    110

#define BTN_W   50
#define BTN_H   30

#define EDIT_W  100
#define EDIT_H  30

//piece info
#define PIECE_TEXT_H    50

#define NEWTRKBAR_X 20
#define NEWTRKBAR_Y 50
#define PIECE_DATE_X      20
#define PIECE_DATE_Y      (NEWTRKBAR_Y+CTRL_TRKBAR_H+PIECE_TEXT_H)
#define PIECE_TIME_X      20
#define PIECE_TIME_Y      (PIECE_DATE_Y+PICKER_H+PIECE_TEXT_H)
#define PIECE_BTN_X     20
#define PIECE_BTN_Y     (PIECE_TIME_Y + PICKER_H + PIECE_TEXT_H)
#define PIECE_LABEL_X   (PIECE_BTN_X + BTN_W + PIECE_BTN_X)
#define PIECE_LABEL_Y   PIECE_BTN_Y
#define PIECE_EDIT_X    (PIECE_LABEL_X + BTN_W + PIECE_BTN_X)
#define PIECE_EDIT_Y    PIECE_BTN_Y

static NCS_PROP_ENTRY trk_props [] = {
    {NCSP_TRKBAR_MINPOS, 0},
    {NCSP_TRKBAR_MAXPOS, 272},
    {NCSP_TRKBAR_CURPOS, 0},
    {NCSP_TRKBAR_LINESTEP, 1},
    {NCSP_TRKBAR_PAGESTEP, 272},
    {0, 0},
};
static NCS_WND_TEMPLATE hosted_tmpl[] = 
{
    {
        NCSCTRL_NEWTRACKBAR,
        IDC_CTRL_TRACKBAR,
        CTRL_TRKBAR_X, CTRL_TRKBAR_Y, CTRL_TRKBAR_W, CTRL_TRKBAR_H,
        WS_VISIBLE | NCSS_NOTIFY | NCSS_NTRKBAR_CIRCLE,
        WS_EX_TROUNDCNS | WS_EX_BROUNDCNS,
        "",
        trk_props, NULL, NULL, NULL, 
        0, 0,
    },
	{
		NCSCTRL_DATEPICKER,
		ID_DATEPICKER,
		DATE_X, DATE_Y, PICKER_W, PICKER_H,
		WS_VISIBLE | NCSS_DPCK_YYYYMMDD | NCSS_NOTIFY,
		WS_EX_NONE,
		"datepicker",
		NULL, NULL, NULL, NULL,
		0, 0
	},
	{
		NCSCTRL_TIMEPICKER,
		ID_TIMEPICKER,
		TIME_X, TIME_Y, PICKER_W, PICKER_H,
		WS_VISIBLE | NCSS_TPCK_24H | NCSS_NOTIFY,
		WS_EX_NONE,
		"timepicker",
		NULL, NULL, NULL, NULL,
		0, 0
	},
    {
        NCSCTRL_BUTTON,
        IDC_BTN,
        0, 0, BTN_W, BTN_H,
        WS_BORDER | WS_VISIBLE | NCSS_NOTIFY,
        WS_EX_NONE,
        "button",
        NULL, NULL, NULL, NULL, 0, 0,
    },
    {
        NCSCTRL_STATIC,
        IDC_LABEL,
        0, 0, BTN_W, BTN_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "LABEL",
        NULL, NULL, NULL, NULL, 0, 0,
    },
    {
        NCSCTRL_SLEDIT,
        IDC_EDIT,
        0, 0, EDIT_W, EDIT_H,
        WS_BORDER | WS_VISIBLE | NCSS_EDIT_LEFT,
        WS_EX_NONE,
        "",
        NULL, NULL, NULL, NULL, 0, 0,
    },
};

static BOOL datechanged_notify(mObject *listener, mObject *sender, int id, DWORD add_data){
    dbg();
    return TRUE;
}

static BOOL timechanged_notify(mObject *listener, mObject *sender, int id, DWORD add_data){
    dbg();
    return TRUE;
}

static BOOL on_trkbar_changed(mObject *listener, mObject *sender, int id, DWORD add_data){
	dbg();
	return TRUE;
}
static BOOL on_trkbar_max(mObject *listener, mObject *sender, int id, DWORD add_data){
	dbg();
	return TRUE;
}
static BOOL on_trkbar_min(mObject *listener, mObject *sender, int id, DWORD add_data){
	dbg();
	return TRUE;
}
static BOOL on_btn_pushed(mObject *listener, mObject *sender, int id, DWORD add_data){
	dbg();
	return TRUE;
}

static mWidgetHostPiece *createHostPiece(mWidget *self, mPanelPiece *parent, NCS_WND_TEMPLATE *tmpl, int x, int y)
{
    mWidgetHostPiece *host;
    RECT rc;

	host = NEWPIECE(mWidgetHostPiece);
	SetRect(&rc, 0, 0, tmpl->w, tmpl->h);
	_c(host)->setRect(host, &rc);
    _c(host)->setHosted(host, tmpl);
	_c(parent)->addContent(parent, (mHotPiece*)host, x, y);

    return host;
}

static BOOL mymain_onCreate(mWidget *self, DWORD add_data)
{
	RECT rc;
	mPanelPiece *body;
    mPanelPiece *panel;
	mWidgetHostPiece *trkbar;
    mWidgetHostPiece *datehost;
    mWidgetHostPiece *timehost;
    mWidgetHostPiece *btn;
    //mWidgetHostPiece *label;
    //mWidgetHostPiece *edit;
    mShapeTransRoundPiece *bk;
#if 0
    mTextPiece *newtrkbar;
    mTextPiece *date;
    mTextPiece *time;
#endif
    mContainerCtrl *ctnr = (mContainerCtrl*)ncsGetChildObj(self->hwnd, CH_IDC_CTNR);

	body = NEWPIECE(mPanelPiece);
	SetRect(&rc, 0, 0, WND_W, WND_H);
	_c(body)->setRect(body, &rc);
	_c(ctnr)->setBody(ctnr, (mHotPiece*)body);

    panel = NEWPIECE(mPanelPiece);
	SetRect(&rc, 0, 0, WND_W, WND_H-20);
	_c(panel)->setRect(panel, &rc);
    _c(body)->addContent(body, (mHotPiece*)panel, 0, 50);

    bk = NEWPIECE(mShapeTransRoundPiece);
    _c(bk)->setRect(bk, &rc);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_BKCOLOR, 0xff497823);
    _c(panel)->addContent(panel, (mHotPiece*)bk, 0, 0);

    //trkbar
    trkbar = createHostPiece(self, panel, &hosted_tmpl[0], NEWTRKBAR_X, NEWTRKBAR_Y);
	ncsAddEventListener((mObject*)trkbar, (mObject*)self, on_trkbar_changed, NCSN_TRKBAR_CHANGED);
	ncsAddEventListener((mObject*)trkbar, (mObject*)self, on_trkbar_max, NCSN_TRKBAR_REACHMAX);
	ncsAddEventListener((mObject*)trkbar, (mObject*)self, on_trkbar_min, NCSN_TRKBAR_REACHMIN);

    //date
    datehost = createHostPiece(self, panel, &hosted_tmpl[1], PIECE_DATE_X, PIECE_DATE_Y);
	ncsAddEventListener((mObject*)datehost, (mObject*)self, datechanged_notify, NCSN_DPCK_DATECHANGED);

    //time
    timehost = createHostPiece(self, panel, &hosted_tmpl[2], PIECE_TIME_X, PIECE_TIME_Y);
	ncsAddEventListener((mObject*)timehost, (mObject*)self, timechanged_notify, NCSN_TPCK_TIMECHANGED);

    //btn
    btn = createHostPiece(self, panel, &hosted_tmpl[3], PIECE_BTN_X, PIECE_BTN_Y);
	ncsAddEventListener((mObject*)btn, (mObject*)self, on_btn_pushed, NCSN_BUTTON_PUSHED);

    //label
    /*label = */createHostPiece(self, panel, &hosted_tmpl[4], PIECE_LABEL_X, PIECE_LABEL_Y);

    //edit
    /*edit = */createHostPiece(self, panel, &hosted_tmpl[5], PIECE_EDIT_X, PIECE_EDIT_Y);
    
#if 0
    //text
	SetRect(&rc, 0, 0, WND_W, PIECE_TEXT_H);
    newtrkbar = NEWPIECE(mTextPiece);
    _c(newtrkbar)->setProperty(newtrkbar, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xffaaaaaa);
    _c(newtrkbar)->setProperty(newtrkbar, NCSP_LABELPIECE_LABEL, (DWORD)"NEWTRACKBAR CTRL");
	_c(body)->addContent(body, (mHotPiece*)newtrkbar, WND_X, NEWTRKBAR_Y - PIECE_TEXT_H);

	SetRect(&rc, 0, 0, WND_W, PIECE_TEXT_H);
    date = NEWPIECE(mTextPiece);
    _c(date)->setProperty(date, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xffaaaaaa);
    _c(date)->setProperty(date, NCSP_LABELPIECE_LABEL, (DWORD)"DATE CTRL");
	_c(body)->addContent(body, (mHotPiece*)date, WND_X, PIECE_DATE_Y - PIECE_TEXT_H);

	SetRect(&rc, 0, 0, WND_W, PIECE_TEXT_H);
    time = NEWPIECE(mTextPiece);
    _c(time)->setProperty(time, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xffaaaaaa);
    _c(time)->setProperty(time, NCSP_LABELPIECE_LABEL, (DWORD)"TIME CTRL");
	_c(body)->addContent(body, (mHotPiece*)time, WND_X, PIECE_TIME_Y - PIECE_TEXT_H);
#endif

	return TRUE;
}
static void mymain_onPaint (mWidget *_this, HDC hdc, const CLIPRGN* inv)
{
}

static BOOL mymain_onClose (mWidget* _this, int message)
{
	DestroyMainWindow (_this->hwnd);
	return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
	{MSG_CREATE, mymain_onCreate},
	{MSG_PAINT, mymain_onPaint},
	{MSG_CLOSE, mymain_onClose},
	{0, NULL}
};

static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        CH_IDC_CTNR,
        0, 0, WND_W, WND_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL, NULL, 0, 0,
    },
};
static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        WND_X, WND_Y, WND_W, WND_H,
        WS_NONE,
        WS_EX_NONE,
        "test-hostpiece",
        NULL,
        NULL,
        mymain_handlers,
        _ctrl_tmpl,
        TABLESIZE(_ctrl_tmpl),
        0,
        0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{
    ncsInitialize();
    ncs4TouchInitialize();
    REGISTER_NCS();

    mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect
                (&mymain_templ, HWND_DESKTOP);
    mGEffInit();
    _c(mydlg)->doModal(mydlg, TRUE);
    MainWindowThreadCleanup(mydlg->hwnd);

    MGNCS_UNREG_COMPONENT(mContainerCtrl);
    ncs4TouchUninitialize();
    ncsUninitialize ();

    return 0;
}
