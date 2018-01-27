#include <string.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>
#include <mgncs4touch/mgncs4touch.h>

#include "TimeSettingActivity.hh"

#include "TimeService.hh"

REGISTER_ACTIVITY(TimeSettingActivity);

// position & size
#define NAVBAR_X    0
#define NAVBAR_Y    0
#define NAVBAR_W    ACTIVITY_W
#define NAVBAR_H    38

#define NAVBAR_BTN_H    25
#define NAVBAR_BTN_X    10
#define NAVBAR_BTN_Y    ( (NAVBAR_H - NAVBAR_BTN_H) / 2 )
#define NAVBAR_BTN_W    80

#define NAVBAR_TITLE_X    0
#define NAVBAR_TITLE_Y    0
#define NAVBAR_TITLE_W    NAVBAR_W
#define NAVBAR_TITLE_H    NAVBAR_H

#define SURROUND_X  (ACTIVITY_X+5)
#define SURROUND_Y  (NAVBAR_Y + NAVBAR_H + 5)
#define SURROUND_W  (ACTIVITY_W-5-5)
#define SURROUND_H  380

#define BTNBAR_X    0
#define BTNBAR_W    SURROUND_W
#define BTNBAR_H    40
#define BTNBAR_Y(i) ((i)*BTNBAR_H)

#define BTNLABEL_W      ((BTNBAR_W-20)/2)
#define BTNLABEL_H      BTNBAR_H
#define BTNLABEL_X(i)   (10+(i)*BTNLABEL_W)
#define BTNLABEL_Y      0

#define PICKER_X    20
#define PICKER_Y    200
#define PICKER_W    (ACTIVITY_W-40)
#define PICKER_H    200

#define PICKER_HIDE_X    20
#define PICKER_HIDE_Y    (200+(SCREEN_H-PICKER_H))

// color
#define BTN_COLOR_NORMAL    0x00000000
#define BTN_COLOR_DOWN      0x80808080

// string
static const char* btn_name_str[] =
{
    "Date", "Time",
};

// ID
#define ID_DATEPICKER   101
#define ID_TIMEPICKER   102

// animation
#define ID_ANIM_IN      1
#define ID_ANIM_OUT     2
#define ANIM_IN_DURATION    300
#define ANIM_OUT_DURATION    300

// timer
#define TIME_UPDATE_TIMER   1
#define TIME_UPDATE_INTERVAL    50

// time format
enum TimeFormat {
    TIME_FORMAT_12 = 0,
    TIME_FORMAT_24 = 1,
};

typedef struct _GradientData {
    float pos;
    ARGB color;
} GradientData;

static const GradientData surround_gradient[] = { 
	{ 0.00f, 0xffd0d0d0, },
	{ 1.00f, 0xfff0f0f0, },
};

static const GradientData navbar_gradient[] = 
{ 
	{ 0.00f, 0xffd0d571, },
	{ 0.49f, 0xffb5c363, },
	{ 0.50f, 0xff9eb23a, },
	{ 0.96f, 0xff7f9e2c, },
	{ 0.97f, 0xff597311, },
	{ 1.00f, 0xff597311, },
};

// get my activity
#define thisActivity(hwnd)    ((TimeSettingActivity*)(Activity::getActivityFromHWND(hwnd)))

// local function declaration
static BOOL evt_btnbar_date_down(mObject* self, mObject* piece,
        int event_id, DWORD param);

static BOOL evt_btnbar_time_down(mObject* self, mObject* piece,
        int event_id, DWORD param);

static void moveWindowAnimCb(MGEFF_ANIMATION anim, void* target, intptr_t id, void* value)
{
    mWidget* widget = (mWidget*) target;
    RECT* pos = (RECT*)value;

    MoveWindow(widget->hwnd, pos->left, pos->top, RECTWP(pos), RECTHP(pos), FALSE);
    UpdateWindow(GetMainWindowHandle(widget->hwnd), FALSE);
}

static int registerControlsAndPieces();

#if 0
static char* formatTime(const time_t* t, char* str, int formatType);
static char* formatDate(const time_t* t, char* str, char separator);
#endif

static BOOL evt_navbarBtnDown(mObject* self, mObject* piece,
        int event_id, DWORD param);

static mShapeTransRoundPiece* createGradientBackground(
    const RECT* prc, const GradientData* gradientTable, int gradientNum);

static NCS_CB_ONPIECEEVENT evt_btnbar_down[] = 
{
    evt_btnbar_date_down,
    evt_btnbar_time_down,
};

// initialize helper functions
void TimeSettingActivity::loadMyResource()
{
    m_navbarTitleFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            20, 0);
    m_navbarBtnFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            16, 0);
    m_btnNameFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            15, 0);
    m_btnContentFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            15, 0);
    m_datePickerTitleFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            15, 0);
    m_datePickerNumFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            30, 0);
    m_timePickerTitleFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            15, 0);
    m_timePickerNumFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            30, 0);
}

void TimeSettingActivity::unloadMyResource()
{
    DestroyLogFont(m_navbarTitleFont    );
    DestroyLogFont(m_navbarBtnFont      );
    DestroyLogFont(m_btnNameFont        );
    DestroyLogFont(m_btnContentFont     );
    DestroyLogFont(m_datePickerTitleFont);
    DestroyLogFont(m_datePickerNumFont  );
    DestroyLogFont(m_timePickerTitleFont);
    DestroyLogFont(m_timePickerNumFont  );
}

// actions
void TimeSettingActivity::switchToTimePicker()
{
    mWidget* from = (mWidget*)m_datePicker;
    mWidget* to = (mWidget*)m_timePicker;

    switchWidget(from, to);

    _c((mTimePicker*)to)->setTime((mTimePicker*)to, TIMESERVICE->get(TimeService::HOUR), 
            TIMESERVICE->get(TimeService::MINUTE));
}

void TimeSettingActivity::switchToDatePicker()
{
    mWidget* from = (mWidget*)m_timePicker;
    mWidget* to = (mWidget*)m_datePicker;

    switchWidget(from, to);

    _c((mDatePicker*)to)->setDate((mDatePicker*)to, TIMESERVICE->get(TimeService::YEAR), 
            TIMESERVICE->get(TimeService::MONTH),
            TIMESERVICE->get(TimeService::MDAY));
}

void TimeSettingActivity::switchWidget(mWidget* from, mWidget* to)
{
    mWidget* target = from;
    int id = ID_ANIM_OUT;
    RECT start = {
        PICKER_X, PICKER_Y,
        PICKER_X+PICKER_W, PICKER_Y+PICKER_H,
    };
    RECT end = {
        PICKER_HIDE_X, PICKER_HIDE_Y,
        PICKER_HIDE_X+PICKER_W, PICKER_HIDE_Y+PICKER_H,
    };
    int duration = ANIM_OUT_DURATION;
    enum EffMotionType curve = OutQuad;

    MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);

    // out animation
    MGEFF_ANIMATION anim = mGEffAnimationCreate(target, moveWindowAnimCb, id, MGEFF_RECT);
    mGEffAnimationSetStartValue(anim, &start);
    mGEffAnimationSetEndValue(anim, &end);
    mGEffAnimationSetDuration(anim, duration);
    mGEffAnimationSetCurve(anim, curve);
    mGEffAnimationAddToGroup(group, anim);

    target = to;
    id = ID_ANIM_IN;
    start.top = PICKER_HIDE_Y;
    start.bottom = PICKER_HIDE_Y+PICKER_H;
    end.top = PICKER_Y;
    end.bottom = PICKER_Y+PICKER_H;
    duration = ANIM_IN_DURATION;
    
    // in animation
    anim = mGEffAnimationCreate(target, moveWindowAnimCb, id, MGEFF_RECT);
    mGEffAnimationSetStartValue(anim, &start);
    mGEffAnimationSetEndValue(anim, &end);
    mGEffAnimationSetDuration(anim, duration);
    mGEffAnimationSetCurve(anim, curve);
    mGEffAnimationAddToGroup(group, anim);

    mGEffAnimationSyncRun(group);
}

void TimeSettingActivity::enterSettings()
{
}

// local function declaration
#if 0
static char* formatTime(const time_t* time, char* str, int formatType)
{
    struct tm* tm = localtime(time);
    int hour = 0;
    int min = 0;

    hour = tm->tm_hour;
    min = tm->tm_min;

    if (formatType == TIME_FORMAT_12) {
        if (hour == 0) hour = 12;
        else if (hour > 12) hour -= 12;
    }
    sprintf(str, "%02d:%02d%s", hour, min, 
            formatType==TIME_FORMAT_24?"":(hour>=12?" PM":" AM"));

    return str;
}
static char* formatDate(const time_t* time, char* str, char separator)
{
    struct tm* tm = localtime(time);
    int year = 2010;
    int moth = 10;
    int mday = 10;

    year = tm->tm_year+1900;
    moth = tm->tm_mon+1;
    mday = tm->tm_mday;

    sprintf(str, "%04d%c%02d%c%02d", year, separator, moth, separator, mday);

    return str;
}
#endif

static void updateTimeAndDate(HWND hwnd)
{
    TimeSettingActivity* _this = thisActivity(GetMainWindowHandle(hwnd));
    static char time[16], date[16];

#if 0
    formatTime(&_this->m_currentTime, time, TIME_FORMAT_24);
    formatDate(&_this->m_currentTime, date, '-');

    //printf("time:%s\n", time);
    //printf("date:%s\n", date);
#else
    TIMESERVICE->format(time, sizeof(time), "%H:%M");
    TIMESERVICE->format(date, sizeof(date), "%Y-%m-%d");
#endif

    mHotPiece* piece = (mHotPiece*)_this->m_btnLabel[TimeSettingActivity::BTNBAR_DATE][TimeSettingActivity::BTNLABEL_CONTENT];
    _c(piece)->setProperty(piece, NCSP_LABELPIECE_LABEL, (DWORD)date);

    piece = (mHotPiece*)_this->m_btnLabel[TimeSettingActivity::BTNBAR_TIME][TimeSettingActivity::BTNLABEL_CONTENT];
    _c(piece)->setProperty(piece, NCSP_LABELPIECE_LABEL, (DWORD)time);

    RECT rc;
    rc.left = SURROUND_X;
    rc.top = SURROUND_Y;
    rc.right = rc.left + SURROUND_W;
    rc.bottom = rc.top+2*BTNBAR_H;

    InvalidateRect(hwnd, &rc, FALSE);
}

#if 0
static BOOL updateTimeAndDateOnTimer(HWND hwnd, int id, DWORD count)
{
    TimeSettingActivity* _this = thisActivity(GetMainWindowHandle(hwnd));
    //printf("updateTimeAndDateOnTimer %d %d\n", _this->m_currentTime+1, time(NULL));
    _this->m_currentTime += 1;

    updateTimeAndDate(hwnd);
    return TRUE;
}
#endif

static BOOL evt_btnbar_date_down(mObject* self, mObject* piece,
        int event_id, DWORD param)
{
    TimeSettingActivity* _this = thisActivity(GetMainWindowHandle(((mWidget*)self)->hwnd));

    if (event_id == NCSN_ABP_CLICKED) {
        if (_this->m_state == TimeSettingActivity::IN_TIME_PICKER) {
            _this->m_state = TimeSettingActivity::IN_PICKER_SWITCH;
            _this->switchToDatePicker();
            _this->m_state = TimeSettingActivity::IN_DATE_PICKER;
        }

        //ShowWindow(_this->m_timePicker->hwnd, SW_HIDE);
        //ShowWindow(_this->m_datePicker->hwnd, SW_SHOW);
        //UpdateWindow(_this->m_datePicker->hwnd, FALSE);
        return TRUE;
    }

    return FALSE;
}

static BOOL evt_btnbar_time_down(mObject* self, mObject* piece,
        int event_id, DWORD param)
{
    TimeSettingActivity* _this = thisActivity(GetMainWindowHandle(((mWidget*)self)->hwnd));

    if (event_id == NCSN_ABP_CLICKED) {
            if (_this->m_state == TimeSettingActivity::IN_DATE_PICKER) {
                _this->m_state = TimeSettingActivity::IN_PICKER_SWITCH;
                _this->switchToTimePicker();
                _this->m_state = TimeSettingActivity::IN_TIME_PICKER;
            }

            //ShowWindow(_this->m_datePicker->hwnd, SW_HIDE);
            //ShowWindow(_this->m_timePicker->hwnd, SW_SHOW);
            return TRUE;
    }

    return FALSE;
}


static int registerControlsAndPieces()
{
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_INIT_CLASS(mTextPiece);
    return 0;
}

static void datechanged_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    int y, m, d;
    _M((mDatePicker *)self, getDate, &y, &m, &d);
	printf("NCSN_DPCK_DATECHANGED: %d, %d, %d\n", y, m, d);

#if 0
    TimeSettingActivity* _this = thisActivity(GetMainWindowHandle(self->hwnd));

    struct tm* tm = localtime(&(_this->m_currentTime));
    //printf("%d %d %d\n", tm->tm_year, tm->tm_mon, tm->tm_mday);
    tm->tm_year = y - 1900;
    tm->tm_mon = m - 1;
    tm->tm_mday = d;
    _this->m_currentTime = mktime(tm);
    updateTimeAndDate(_this->hwnd());
#else
    TIMESERVICE->set(TimeService::YEAR, y);
    TIMESERVICE->set(TimeService::MONTH, m);
    TIMESERVICE->set(TimeService::MDAY, d);
#endif
}

static void timechanged_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    int h, m;
    _M((mTimePicker *)self, getTime, &h, &m);
	printf("NCSN_TPCK_TIMECHANGED:%d %d\n", h, m);

#if 0
    TimeSettingActivity* _this = thisActivity(GetMainWindowHandle(self->hwnd));

    struct tm* tm = localtime(&(_this->m_currentTime));
    //printf("%d %d %d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    tm->tm_hour = h;
    tm->tm_min = m;
    _this->m_currentTime = mktime(tm);
    updateTimeAndDate(_this->hwnd());
#else
    TIMESERVICE->set(TimeService::HOUR, h);
    TIMESERVICE->set(TimeService::MINUTE, m);
#endif
}

static BOOL onMinute(void* target, TimeService *ts, int eventId, DWORD param)
{
    char time[128];
    TIMESERVICE->format(time, sizeof(time), "%Y-%m-%d %H:%M");
    printf("*********time: %s\n", time);

    updateTimeAndDate((HWND)target);

    return TRUE;
}


// main window's message handler---------------------------------------------
static BOOL mainWnd_onCreate(mWidget* self, DWORD add_data)
{
    TimeSettingActivity* _this = thisActivity(self->hwnd);
    assert(_this);

    _this->self = self;

    _this->initNavbar();
    _this->initSurround();
    _this->initDatePicker();
    _this->initTimePicker();

    // update time & date
    updateTimeAndDate(self->hwnd);

    // set timer for update time & date
#if 0
    SetTimerEx(self->hwnd, TIME_UPDATE_TIMER, TIME_UPDATE_INTERVAL, updateTimeAndDateOnTimer);
#else
    TIMESERVICE->addEventListener((void*)self->hwnd, TimeService::MINUTE, onMinute);
    TIMESERVICE->addEventListener((void*)self->hwnd, TimeService::HOUR, onMinute);
    TIMESERVICE->addEventListener((void*)self->hwnd, TimeService::MDAY, onMinute);
    TIMESERVICE->addEventListener((void*)self->hwnd, TimeService::MONTH, onMinute);
    TIMESERVICE->addEventListener((void*)self->hwnd, TimeService::YEAR, onMinute);
#endif

    return TRUE;
}
static void mainWnd_onClose(mWidget* self, int message)
{
    return;
}
static void mainWnd_onDestroy(mWidget* self, int message)
{
    TimeSettingActivity* _this = thisActivity(self->hwnd);
    assert(_this);

    TIMESERVICE->removeEventListener((void*)self->hwnd);
    TIMESERVICE->removeEventListener((void*)self->hwnd);
    TIMESERVICE->removeEventListener((void*)self->hwnd);
    TIMESERVICE->removeEventListener((void*)self->hwnd);
    TIMESERVICE->removeEventListener((void*)self->hwnd);

    return;
}
static void mainWnd_onPaint(mWidget *self, HDC hdc, const CLIPRGN* inv)
{
    SetBrushColor(hdc, RGBA2Pixel(hdc, 0x80, 0x80, 0x80, 0xff));
    FillBox(hdc, 0, 0, ACTIVITY_W, ACTIVITY_H);
}

static NCS_EVENT_HANDLER datepicker_handler [] = {
    NCS_MAP_NOTIFY(NCSN_DPCK_DATECHANGED, datechanged_notify),
	{0, NULL}
};

static NCS_EVENT_HANDLER timepicker_handler [] = {
    NCS_MAP_NOTIFY(NCSN_TPCK_TIMECHANGED, timechanged_notify),
	{0, NULL}
};

// ncs template
static NCS_EVENT_HANDLER mymain_handlers [] = {
        {MSG_CREATE, reinterpret_cast<void*>(mainWnd_onCreate)},
        {MSG_CLOSE,  reinterpret_cast<void*>(mainWnd_onClose)},
        {MSG_DESTROY,  reinterpret_cast<void*>(mainWnd_onDestroy)},
        {MSG_PAINT,  reinterpret_cast<void*>(mainWnd_onPaint)},
        {0, NULL}
};

static NCS_WND_TEMPLATE picker_templ[] = {
	{
		NCSCTRL_DATEPICKER,
		ID_DATEPICKER,
		PICKER_X, PICKER_Y, PICKER_W, PICKER_H,
		WS_VISIBLE | NCSS_DPCK_YYYYMMDD | NCSS_NOTIFY,
		WS_EX_NONE,
		"datepicker",
		NULL, //props,
		NULL, //rdr_info
		datepicker_handler, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_TIMEPICKER,
		ID_TIMEPICKER,
		PICKER_X, PICKER_Y, PICKER_W, PICKER_H,
		WS_VISIBLE | NCSS_TPCK_24H | NCSS_NOTIFY,
		WS_EX_NONE,
		"timepicker",
		NULL, //props,
		NULL, //rdr_info
		timepicker_handler, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_MAINWND, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_AUTOSECONDARYDC,
        "TimeSettingActivity",
        NULL,
        NULL,
        mymain_handlers,
        NULL,//_ctrl_templ,
        0,//sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
        0,
        0, 
        0,
};

// constructor & destructor
TimeSettingActivity::TimeSettingActivity() :
    NCSActivity( &mymain_templ)
{ 
    m_style = STYLE_PUSH;

    registerControlsAndPieces();

    loadMyResource();

    m_state = IN_DATE_PICKER;
#if 0
    time(&m_currentTime);
#endif

    MGNCS_INIT_CLASS(mButtonPanelPiece);
}

TimeSettingActivity::~TimeSettingActivity()
{
    unloadMyResource();
}

void TimeSettingActivity::initNavbar()
{
    this->m_navbarCtrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "m_navbarCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            NAVBAR_X, NAVBAR_Y, NAVBAR_W, NAVBAR_H,
            self->hwnd,
            NULL, NULL, NULL, 0);
    assert(m_navbarCtrl);

    mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    assert(body);

    RECT rc;
    rc.left = 0;
    rc.top = 0;

    rc.right = NAVBAR_W;
    rc.bottom = NAVBAR_H;
    _c(body)->setRect(body, &rc);
    _c(this->m_navbarCtrl)->setBody(this->m_navbarCtrl, (mHotPiece*)body);

    // use the same rc to create background
    mShapeTransRoundPiece* bg = createGradientBackground(&rc, navbar_gradient, TABLESIZE(navbar_gradient));
    _c (body)->addContent(body, (mHotPiece*)bg, 0, 0);
    _c(bg)->setProperty (bg, NCSP_TRANROUND_CORNERFLAG, 0);

    // title
    rc.right = NAVBAR_TITLE_W;
    rc.bottom = NAVBAR_TITLE_H;
    mHotPiece* titlePiece = (mHotPiece*)NEWPIECE(mTextPiece);
    assert(titlePiece);
    _c(titlePiece)->setRect (titlePiece, &rc);
    _c(titlePiece)->setProperty(titlePiece, NCSP_LABELPIECE_LABEL,
            (DWORD)"Date and Time");
    _c(titlePiece)->setProperty(titlePiece, NCSP_TEXTPIECE_LOGFONT,
            (DWORD)m_navbarTitleFont);
    _c(titlePiece)->setProperty(titlePiece, NCSP_TEXTPIECE_TEXTCOLOR,
            (DWORD)0xff192952);
    _c(titlePiece)->setProperty(titlePiece, NCSP_TEXTPIECE_TEXTSHADOWCOLOR,
            (DWORD)0xffffffff);
    _c(body)->addContent(body, (mHotPiece*)titlePiece,
            NAVBAR_TITLE_X, NAVBAR_TITLE_Y);

    // container for button
    rc.right = NAVBAR_BTN_W;
    rc.bottom = NAVBAR_BTN_H;
    //mPanelPiece* btnPanel = (mPanelPiece*)NEWPIECE(mPanelPiece);
    mButtonPanelPiece* btnPanel = NEWPIECE(mButtonPanelPiece);
    _c(btnPanel)->setRect (btnPanel, &rc);
    // background for button
    if (0) {
        mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_gradient)->setRect (back_gradient, &rc);
        //_c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 0);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(back_gradient)->setProperty(back_gradient, NCSP_TRANROUND_RADIUS, 4);

        HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
                (mShapeTransRoundPiece*)back_gradient);

        /* 0.0f, 1.0f are needed */
        MGPlusLinearGradientBrushAddColor(brush, 0xffffffff, 0.0f);
        MGPlusLinearGradientBrushAddColor(brush, 0xffd1d1d1, 1.0f);

        _c(btnPanel)->addContent(btnPanel, back_gradient, 0, 0);
    }

    ARGB normalColors[] = {0xffffffff, 0xffd1d1d1};
    ARGB pushedColors[] = {0xa0e2edf9, 0xa0a1abb7, 0xa0e2edf9};
    _c(btnPanel)->setGradientBackgroundColor(btnPanel,
            normalColors, NULL, TABLESIZE(normalColors),
            pushedColors, NULL, TABLESIZE(pushedColors));
    _c(btnPanel)->setStates(btnPanel);
    mShapeTransRoundPiece *bkgnd = _c(btnPanel)->getBkgndPiece(btnPanel);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_CORNERFLAG, TRANROUND_CORNERFLAG_RIGHTTOP|TRANROUND_CORNERFLAG_RIGHTBOTTOM);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_RADIUS, 4);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_SHARPFLAG, TRANROUND_SHARPFLAG_LEFT);
    //_c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_SHARPWIDTH, 10);

    mHotPiece* btn = (mHotPiece*)NEWPIECE(mTextPiece);
    _c(btn)->setRect (btn, &rc);
    _c(btn)->setProperty(btn, NCSP_LABELPIECE_LABEL, (DWORD)"Settings");
    _c(btn)->setProperty(btn, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_navbarBtnFont);
    _c(btn)->setProperty(btn, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff192952);
    _c(btnPanel)->addContent(btnPanel, (mHotPiece*)btn, 5, 0);

    _c(body)->addContent(body, (mHotPiece*)btnPanel, NAVBAR_BTN_X, NAVBAR_BTN_Y);

    // add event listener for button
    ncsAddEventListener((mObject*)btnPanel, (mObject*)this->m_navbarCtrl, 
            (NCS_CB_ONPIECEEVENT)evt_navbarBtnDown, NCSN_ABP_CLICKED);
}

void TimeSettingActivity::initSurround()
{
    this->m_surroundCtrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "m_surroundCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            SURROUND_X, SURROUND_Y, SURROUND_W, SURROUND_H,
            self->hwnd,
            NULL, NULL, NULL, 0);
    assert(m_surroundCtrl);

    mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    assert(body);

    RECT rc;
    rc.left = 0;
    rc.top = 0;

    rc.right = SURROUND_W;
    rc.bottom = SURROUND_H;
    _c(body)->setRect (body, &rc);
    _c(this->m_surroundCtrl)->setBody(this->m_surroundCtrl, (mHotPiece*)body);

    rc.right = SURROUND_W;
    rc.bottom = SURROUND_H;
    mShapeTransRoundPiece* bg = createGradientBackground(&rc, surround_gradient, TABLESIZE(surround_gradient));
    _c (body)->addContent (body, (mHotPiece*)bg, 0, 0);

    mContainerCtrl* btnBarCtrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "btnCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            SURROUND_X, SURROUND_Y, BTNBAR_W, 2*BTNBAR_H,
            self->hwnd,
            NULL, NULL, NULL, 0);
    body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    assert(body);
    rc.right = BTNBAR_W;
    rc.bottom = 2*BTNBAR_H;
    _c(body)->setRect(body, &rc);
    _c(btnBarCtrl)->setBody(btnBarCtrl, (mHotPiece*)body);

    PLOGFONT font = NULL;
    ARGB color;
    int align;
    for (int i=0; i<BTNBAR_NUM; ++i) {
        //m_btnBar[i] = (mPanelPiece*)NEWPIECE(mPanelPiece);
        m_btnBar[i] = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
        rc.right = BTNBAR_W;
        rc.bottom = BTNBAR_H;
        _c(m_btnBar[i])->setRect(m_btnBar[i], &rc);
        _c(m_btnBar[i])->setStateTable(m_btnBar[i], (mHotPiece*)_c(m_btnBar[i])->getBkgndPiece(m_btnBar[i]),
                NCSP_TRANROUND_BKCOLOR, BTN_COLOR_NORMAL, BTN_COLOR_DOWN);
        _c(m_btnBar[i])->setStates(m_btnBar[i]);

        // add into container
        _c(body)->addContent(body, (mHotPiece*)m_btnBar[i], BTNBAR_X, BTNBAR_Y(i));

        // add event handler
        ncsAddEventListener((mObject*)m_btnBar[i], (mObject*)btnBarCtrl,
                (NCS_CB_ONPIECEEVENT)evt_btnbar_down[i], NCSN_ABP_CLICKED);

        for (int j=0; j<BTNLABEL_NUM; ++j) {
            m_btnLabel[i][j] = (mTextPiece*)NEWPIECE(mTextPiece);
            rc.right = BTNLABEL_W;
            rc.bottom = BTNLABEL_H;
            _c(m_btnLabel[i][j])->setRect(m_btnLabel[i][j], &rc);

            // set font & label
            if (j == BTNLABEL_NAME) {
                font = m_btnNameFont;
                color = 0xff0000ff;
                align = NCS_ALIGN_LEFT;

                _c(m_btnLabel[i][j])->setProperty(m_btnLabel[i][j],
                        NCSP_LABELPIECE_LABEL, (DWORD)btn_name_str[i]);
            }
            else {
                font = m_btnContentFont;
                color = 0xff000000;
                align = NCS_ALIGN_RIGHT;
            }
            _c(m_btnLabel[i][j])->setProperty(m_btnLabel[i][j],
                    NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
            _c(m_btnLabel[i][j])->setProperty(m_btnLabel[i][j],
                    NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)color);
            _c(m_btnLabel[i][j])->setProperty(m_btnLabel[i][j],
                    NCSP_LABELPIECE_ALIGN, (DWORD)align);

            _c(m_btnLabel[i][j])->setProperty(
                m_btnLabel[i][j], NCSP_LABELPIECE_VALIGN, NCS_VALIGN_CENTER);


            // add into container
            _c(m_btnBar[i])->addContent(m_btnBar[i], 
                    (mHotPiece*)m_btnLabel[i][j], BTNLABEL_X(j), BTNLABEL_Y);
        }

    }
}

void TimeSettingActivity::initDatePicker()
{
    m_datePicker = (mDatePicker*)ncsCreateWindowIndirect(&picker_templ[0], self->hwnd);
    assert(m_datePicker);

    _c(m_datePicker)->setDate(m_datePicker, TIMESERVICE->get(TimeService::YEAR), 
            TIMESERVICE->get(TimeService::MONTH),
            TIMESERVICE->get(TimeService::MDAY));
}

void TimeSettingActivity::initTimePicker()
{
    m_timePicker = (mTimePicker*)ncsCreateWindowIndirect(&picker_templ[1], self->hwnd);
    assert(m_timePicker);

#if 0
    _c(m_timePicker)->setTime(m_timePicker, TIMESERVICE->get(TimeService::HOUR), 
            TIMESERVICE->get(TimeService::MINUTE));
#endif

    MoveWindow(m_timePicker->hwnd, PICKER_HIDE_X, PICKER_HIDE_Y, PICKER_W, PICKER_H, FALSE);
}

static BOOL evt_navbarBtnDown(mObject* self, mObject* piece,
        int event_id, DWORD param)
{
    printf("evt_navbarBtnDown\n");
    if (event_id == NCSN_ABP_CLICKED) {
        ActivityStack::singleton()->pop();
        return TRUE;
    }
    return FALSE;
}

static mShapeTransRoundPiece* createGradientBackground(
        const RECT* prc, const GradientData* gradientTable, int gradientNum)
{
    RECT rc;
    // int x = 0;
    // int y = 0;

    rc.left = 0;
    rc.top = 0;
    rc.right = RECTWP(prc);
    rc.bottom = RECTHP(prc);

    mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(back_gradient)->setRect(back_gradient, &rc);
    _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);

    HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
            (mShapeTransRoundPiece*)back_gradient);

    /* 0.0f, 1.0f are needed */
    for (int i=0; i<gradientNum; ++i)
        MGPlusLinearGradientBrushAddColor(brush, gradientTable[i].color, gradientTable[i].pos);

    //_c(parent)->addContent(parent, back_gradient, x, y);

    return (mShapeTransRoundPiece*)back_gradient;
}
