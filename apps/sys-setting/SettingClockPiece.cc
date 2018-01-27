#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>
#include <mgncs4touch/mgncs4touch.h>

#include "mcontainerctrl.h"
#include "mpanelpiece.h"
#include "mtextpiece.h"
#include "register.h"

#include "mtransroundpiece.h"

#include "SettingActivity.hh"
#include "TimeService.hh"

#define S_ID_DATEPICKER   101
#define S_ID_TIMEPICKER   102

#define S_PICKER_SPACE_Y    48
#define S_PICKER_X    30
#define S_PICKER_H    200
#define S_PICKER_Y    (SETTING_TABLE_H - S_PICKER_H - S_PICKER_SPACE_Y)
#define S_PICKER_W    (ACTIVITY_W-(S_PICKER_X << 1))

#define S_SET_TITLE_H 32
#define S_SET_TITLE_Y (S_PICKER_Y - S_SET_TITLE_H)

#define S_PICKER_HIDE_X    S_PICKER_X
#define S_PICKER_HIDE_Y    (SETTING_TABLE_H+S_SET_TITLE_H) 

#define S_BUTTON_W    ACTIVITY_W
#define S_BUTTON_H    48

#define S_BUTTON_DATE_X    0
#define S_BUTTON_DATE_Y    10
#define S_BUTTON_TIME_X    0
#define S_BUTTON_TIME_Y    (S_BUTTON_H + S_BUTTON_DATE_Y)

#define S_BUTTON_TITLE_H    S_BUTTON_H
#define S_BUTTON_TITLE_W    90

#define S_BUTTON_DETAIL_H    S_BUTTON_H    
#define S_BUTTON_DETAIL_W    120

#define S_SEPARATOR_H  2
#define S_SEPARATOR_Y(i)  (S_BUTTON_DATE_Y + S_BUTTON_H * (i + 1))

#define S_BUTTON_TITLE_Y     0 
#define S_BUTTON_DETAIL_X    (S_BUTTON_W - S_BUTTON_DETAIL_W)
#define S_BUTTON_DETAIL_Y    (S_BUTTON_TITLE_Y + S_SEPARATOR_H)


#define S_SET_DATE_TITLE_Y_X S_PICKER_X
#define S_SET_DATE_TITLE_Y_W (S_PICKER_W >> 1)

#define S_SET_DATE_TITLE_M_X (S_SET_DATE_TITLE_Y_X + S_SET_DATE_TITLE_Y_W)
#define S_SET_DATE_TITLE_M_W ((S_PICKER_W - S_SET_DATE_TITLE_Y_W)>>1)
#define S_SET_DATE_TITLE_D_X (S_SET_DATE_TITLE_M_X + S_SET_DATE_TITLE_M_W)
#define S_SET_DATE_TITLE_D_W S_SET_DATE_TITLE_M_W

#define S_SET_TIME_TITLE_H_X S_PICKER_X
#define S_SET_TIME_TITLE_H_W (S_PICKER_W >> 1)
#define S_SET_TIME_TITLE_M_X (S_SET_TIME_TITLE_H_X + S_SET_TIME_TITLE_H_W)
#define S_SET_TIME_TITLE_M_W (S_PICKER_W - S_SET_TIME_TITLE_H_W)

#define S_BTN_COLOR_NORMAL    0x00000000
#define S_BTN_COLOR_DOWN      0x80808080

#define S_MOVE_ANI_DURATION  200

static mPanelPiece *g_dateTitlePiece = NULL;
static mPanelPiece *g_timeTitlePiece = NULL;
static mWidgetHostPiece *g_datePiece = NULL;
static mWidgetHostPiece *g_timePiece = NULL;
static mWidgetHostPiece *g_curPiece = NULL;

#if WIDGETHOST_FROM_DESKTOP
static mWidget *g_dateWidget = NULL;
static mWidget *g_timeWidget = NULL;
#endif
enum{
    S_TYPE_DATE,
    S_TYPE_TIME,
    S_TYPE_MAX
};

static const char *s_text[S_TYPE_MAX] = {
    "Date",
    "Time",
};
static mTextPiece *sys_set[S_TYPE_MAX] = {NULL};
#if 0
static ARGB gradient_color[] = {
    0xffd0d0d0,0xfff0f0f0
};
#else
static ARGB gradient_color[] = {
    0xffffffff,0xffffffff
};
#endif
static float gradient_pos[] = {
    0.0,1.0
};


enum{
    DATEPICKER,
    TIMEPICKER,
};

static void datechanged_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    int y, m, d;
    _M((mDatePicker *)self, getDate, &y, &m, &d);

    TIMESERVICE->set(TimeService::YEAR, y);
    TIMESERVICE->set(TimeService::MDAY, d);
    TIMESERVICE->set(TimeService::MONTH, m);
}

static void timechanged_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    int h, m;
    _M((mTimePicker *)self, getTime, &h, &m);

    TIMESERVICE->set(TimeService::HOUR, h);
    TIMESERVICE->set(TimeService::MINUTE, m);
}

static NCS_EVENT_HANDLER datepicker_handler [] = {
    NCS_MAP_NOTIFY(NCSN_DPCK_DATECHANGED, datechanged_notify),
	{0, NULL}
};

static NCS_EVENT_HANDLER timepicker_handler [] = {
    NCS_MAP_NOTIFY(NCSN_TPCK_TIMECHANGED, timechanged_notify),
	{0, NULL}
};

static NCS_WND_TEMPLATE time_tmpl[] =
{
    {
        NCSCTRL_DATEPICKER,
        S_ID_DATEPICKER,
        0,0, S_PICKER_W, S_PICKER_H,
        WS_VISIBLE | NCSS_DPCK_YYYYMMDD | NCSS_NOTIFY,
        WS_EX_NONE,
        "datepicker",
        NULL, 
        NULL, 
        datepicker_handler,
        NULL,
        0,
        0 
    },
    {
        NCSCTRL_TIMEPICKER,
        S_ID_TIMEPICKER,
        0,0, S_PICKER_W, S_PICKER_H,
        WS_VISIBLE | NCSS_TPCK_24H | NCSS_NOTIFY,
        WS_EX_NONE,
        "timepicker",
        NULL,
        NULL,
        timepicker_handler,
        NULL,
        0,
        0 
    },
};
static void moveWindowAnimCb(MGEFF_ANIMATION anim, void* target, intptr_t id, void* value)
{
    mPanelPiece *piece = (mPanelPiece *) target;
    int pos = *(int*)value;

    if (pos < S_PICKER_Y)
        pos = S_PICKER_Y;
    if (id == (intptr_t)g_datePiece)
    {
        _c(piece)->movePiece(piece,(mHotPiece *)g_datePiece,S_PICKER_X,pos);
        _c(piece)->movePiece(piece,(mHotPiece *)g_dateTitlePiece,S_PICKER_X,pos - S_SET_TITLE_H);
    }
    else if (id == (intptr_t)g_timePiece)
    {
        _c(piece)->movePiece(piece,(mHotPiece *)g_timePiece,S_PICKER_X,pos);
        _c(piece)->movePiece(piece,(mHotPiece *)g_timeTitlePiece,S_PICKER_X,pos - S_SET_TITLE_H);
    }
}

static void clockSwitchWidget(mPanelPiece *piece,mWidget* from, mWidget* to)
{
    intptr_t id = (intptr_t)from;
    int int_s = S_PICKER_Y;
    int int_e = S_PICKER_HIDE_Y;
    int duration = S_MOVE_ANI_DURATION;
    enum EffMotionType curve = OutQuad;

    MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);

    // out animation
    MGEFF_ANIMATION anim = mGEffAnimationCreate((void *)piece, moveWindowAnimCb, id, MGEFF_INT);
    mGEffAnimationSetStartValue(anim, &int_s);
    mGEffAnimationSetEndValue(anim, &int_e);
    mGEffAnimationSetDuration(anim, duration);
    mGEffAnimationSetCurve(anim, curve);
    mGEffAnimationAddToGroup(group, anim);

    id = (intptr_t)to;
    int_s = S_PICKER_HIDE_Y;
    int_e = S_PICKER_Y,
    
    // in animation
    anim = mGEffAnimationCreate((void *)piece, moveWindowAnimCb, id, MGEFF_INT);
    mGEffAnimationSetStartValue(anim, &int_s);
    mGEffAnimationSetEndValue(anim, &int_e);
    mGEffAnimationSetDuration(anim, duration);
    mGEffAnimationSetCurve(anim, curve);
    mGEffAnimationAddToGroup(group, anim);

    mGEffAnimationSyncRun(group);
    mGEffAnimationDelete(group);
}

static BOOL evt_btnbar_down(mObject* self, mObject* piece,int event_id, DWORD param)
{
    void *picker = NULL;
    mPanelPiece *panelpiece = (mPanelPiece *)self;
    if ((int)param == S_TYPE_DATE && g_curPiece != g_datePiece)
    {
        picker = (mDatePicker*)_c(g_datePiece)->getHosted(g_datePiece);
        _c((mDatePicker*)picker)->setDate((mDatePicker*)picker, TIMESERVICE->get(TimeService::YEAR), 
            TIMESERVICE->get(TimeService::MONTH),
            TIMESERVICE->get(TimeService::MDAY));
        clockSwitchWidget(panelpiece,(mWidget*)g_timePiece,(mWidget*)g_datePiece);
        g_curPiece = g_datePiece;
    }
    else if ((int)param == S_TYPE_TIME && g_curPiece != g_timePiece)
    {
        picker = (mTimePicker*)_c(g_timePiece)->getHosted(g_timePiece);
        _c((mTimePicker*)picker)->setTime((mTimePicker*)picker, TIMESERVICE->get(TimeService::HOUR), 
            TIMESERVICE->get(TimeService::MINUTE));
        clockSwitchWidget(panelpiece,(mWidget*)g_datePiece,(mWidget*)g_timePiece);
        g_curPiece = g_timePiece;
    }
    return TRUE;
}

static mHotPiece *createSurroundBtn(mPanelPiece *panelPiece,int type,PLOGFONT font)
{
    mTextPiece* title;
    mTextPiece* detail;
    mHotPiece* separator;
    RECT rc = {0,0,S_BUTTON_W,S_BUTTON_H};
    mButtonPanelPiece* btn;

    static char time[16], date[16];
    assert(type >= S_TYPE_DATE && type < S_TYPE_MAX);

    btn = (mButtonPanelPiece*)NEWPIECEEX(mButtonPanelPiece,(DWORD)type);
    _c(btn)->setRect(btn, &rc);
    _c(btn)->setStateTable(btn, (mHotPiece*)_c(btn)->getBkgndPiece(btn),
            NCSP_TRANROUND_BKCOLOR, S_BTN_COLOR_NORMAL, S_BTN_COLOR_DOWN);
    _c(btn)->setStates(btn);
    ncsAddEventListener((mObject*)btn, (mObject*)panelPiece,
            (NCS_CB_ONPIECEEVENT)evt_btnbar_down, NCSN_ABP_CLICKED);

    title = (mTextPiece*)NEWPIECE(mTextPiece);
    SetRect(&rc,0,0,S_BUTTON_TITLE_W,S_BUTTON_TITLE_H);
    _c(title)->setRect(title, &rc);
    _c(title)->setProperty(title,NCSP_LABELPIECE_LABEL, (DWORD)s_text[type]);
    _c(title)->setProperty(title,NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
    _c(title)->setProperty(title,NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff0000ff);
    _c(btn)->addContent(btn,(mHotPiece *)title, S_BUTTON_DATE_X,S_BUTTON_TITLE_Y);

    detail = sys_set[type] = (mTextPiece*)NEWPIECE(mTextPiece);
    SetRect(&rc,0,0,S_BUTTON_DETAIL_W,S_BUTTON_DETAIL_H);
    _c(detail)->setRect(detail, &rc);
    if (type == S_TYPE_DATE)
    {
        memset(date, 0x00, sizeof(date));
        TIMESERVICE->format(date, sizeof(date), "%Y-%m-%d");
        _c(detail)->setProperty(detail,NCSP_LABELPIECE_LABEL, (DWORD)date);
    }
    else
    {
        memset(time, 0x00, sizeof(time));
        TIMESERVICE->format(time, sizeof(time), "%H:%M");
        _c(detail)->setProperty(detail,NCSP_LABELPIECE_LABEL, (DWORD)time);
    }
    _c(detail)->setProperty(detail,NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
    _c(detail)->setProperty(detail,NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff000000);
    _c(btn)->addContent(btn,(mHotPiece *)detail, S_BUTTON_DETAIL_X,S_BUTTON_DETAIL_Y);

    SetRect(&rc,0,0,S_BUTTON_W,S_SEPARATOR_H);
    separator = (mHotPiece*)NEWPIECE(mExSeparatorPiece);
    _c(separator)->setRect (separator, &rc);
    _c(panelPiece)->addContent(panelPiece,separator,0,S_SEPARATOR_Y(type));
    return (mHotPiece *)btn;
}

static mHotPiece *createPickerTitle(mPanelPiece *panelPiece,int type,PLOGFONT font)
{
    RECT rc = {0,0,S_PICKER_W,S_SET_TITLE_H};
    mTextPiece *title;
    mPanelPiece *panel;

    panel = NEWPIECE(mPanelPiece);
    _c(panel)->setRect(panel,&rc);

    if (type == S_TYPE_DATE)
    {
        SetRect(&rc,0,0,S_SET_DATE_TITLE_Y_W,S_SET_TITLE_H);
        title = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(title)->setRect(title,&rc);
        _c(title)->setProperty(title,NCSP_LABELPIECE_LABEL, (DWORD)"Y");
        _c(title)->setProperty(title,NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
        _c(title)->setProperty(title,NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff000000);
        _c(panel)->addContent(panel,(mHotPiece *)title,0,0);

        SetRect(&rc,0,0,S_SET_DATE_TITLE_M_W,S_SET_TITLE_H);
        title = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(title)->setRect(title,&rc);
        _c(title)->setProperty(title,NCSP_LABELPIECE_LABEL, (DWORD)"M");
        _c(title)->setProperty(title,NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
        _c(title)->setProperty(title,NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff000000);
        _c(panel)->addContent(panel,(mHotPiece *)title,
                S_SET_DATE_TITLE_M_X - S_SET_DATE_TITLE_Y_X,0);

        SetRect(&rc,0,0,S_SET_DATE_TITLE_D_W,S_SET_TITLE_H);
        title = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(title)->setRect(title,&rc);
        _c(title)->setProperty(title,NCSP_LABELPIECE_LABEL, (DWORD)"D");
        _c(title)->setProperty(title,NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
        _c(title)->setProperty(title,NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff000000);
        _c(panel)->addContent(panel,(mHotPiece *)title,
                S_SET_DATE_TITLE_D_X - S_SET_DATE_TITLE_Y_X,0);
        g_dateTitlePiece = panel;
    }
    else if (type == S_TYPE_TIME)
    {
        SetRect(&rc,0,0,S_SET_TIME_TITLE_H_W,S_SET_TITLE_H);
        title = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(title)->setRect(title,&rc);
        _c(title)->setProperty(title,NCSP_LABELPIECE_LABEL, (DWORD)"H");
        _c(title)->setProperty(title,NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
        _c(title)->setProperty(title,NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff000000);
        _c(panel)->addContent(panel,(mHotPiece *)title,0,0);

        SetRect(&rc,0,0,S_SET_TIME_TITLE_M_W,S_SET_TITLE_H);
        title = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(title)->setRect(title,&rc);
        _c(title)->setProperty(title,NCSP_LABELPIECE_LABEL, (DWORD)"M");
        _c(title)->setProperty(title,NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
        _c(title)->setProperty(title,NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff000000);
        _c(panel)->addContent(panel,(mHotPiece *)title,
                S_SET_TIME_TITLE_M_X - S_SET_TIME_TITLE_H_X,0);
        g_timeTitlePiece = panel;
    }
    return (mHotPiece *)panel;
}

static void updateTimeAndDate(HWND hwnd)
{
    static char time[16], date[16];

    TIMESERVICE->format(time, sizeof(time), "%H:%M");
    TIMESERVICE->format(date, sizeof(date), "%Y-%m-%d");

    _c(sys_set[S_TYPE_DATE])->setProperty(
            sys_set[S_TYPE_DATE], NCSP_LABELPIECE_LABEL, (DWORD)date);

    _c(sys_set[S_TYPE_TIME])->setProperty(
            sys_set[S_TYPE_TIME], NCSP_LABELPIECE_LABEL, (DWORD)time);

    PanelPiece_invalidatePiece((mHotPiece *)sys_set[S_TYPE_DATE],NULL);
    PanelPiece_invalidatePiece((mHotPiece *)sys_set[S_TYPE_TIME],NULL);
}

static BOOL onMinute(void* target, TimeService *ts, int eventId, DWORD param)
{
    updateTimeAndDate((HWND)target);

    return TRUE;
}

mHotPiece *SettingActivity::createClockView(HWND hwnd)
{
    mPanelPiece *panel;
    HBRUSH brush;
    mHotPiece *back_gradient;
    mWidgetHostPiece *date;
    mWidgetHostPiece *time;
    mContainerCtrl *ctnr = (mContainerCtrl *)ncsGetChildObj(hwnd, SETTING_IDC_CTNR);

    NCS_WND_TEMPLATE tmpl = time_tmpl[DATEPICKER];
    RECT rc = {0, 0, ACTIVITY_W, SETTING_TABLE_H};
#if WIDGETHOST_FROM_DESKTOP
    if (g_dateWidget && g_timeWidget)
    {
        SendMessage(g_dateWidget->hwnd,MSG_CLOSE,0,0);
        SendMessage(g_timeWidget->hwnd,MSG_CLOSE,0,0);
        _c(g_dateWidget)->release(g_dateWidget);
        _c(g_timeWidget)->release(g_timeWidget);
    }
#endif

    assert(ctnr);
    panel  = (mPanelPiece*)NEWPIECE(mPanelPiece);
    assert(panel);
    _c(panel)->setRect (panel, &rc);

    back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(back_gradient)->setRect(back_gradient, &rc);
    _c(back_gradient)->setProperty (back_gradient, 
                    NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_RADIUS, 0);
    brush = _c((mShapeTransRoundPiece *)back_gradient)->getBrush(
            (mShapeTransRoundPiece *)back_gradient);
    MGPlusSetLinearGradientBrushColorsEx(
            brush,gradient_color,TABLESIZE(gradient_color),gradient_pos);
    _c(panel)->addContent(panel, back_gradient, 0,0);

    date = NEWPIECEEX(mWidgetHostPiece, (DWORD)hwnd);
    SetRect(&rc, 0, 0, tmpl.w, tmpl.h);
    _c(date)->setRect(date, &rc);
    _c(date)->setHosted(date, &time_tmpl[DATEPICKER]);
    _c(panel)->addContent(panel, (mHotPiece*)date, S_PICKER_X,S_PICKER_Y);

    tmpl = time_tmpl[TIMEPICKER];
    time = NEWPIECEEX(mWidgetHostPiece, (DWORD)hwnd);
    SetRect(&rc, 0, 0, tmpl.w, tmpl.h);
    _c(time)->setRect(time, &rc);
    _c(time)->setHosted(time, &time_tmpl[TIMEPICKER]);
    _c(panel)->addContent(panel, (mHotPiece*)time, S_PICKER_HIDE_X, S_PICKER_HIDE_Y);
    printf(" %s  panel = %p \n",__FUNCTION__,panel);

    _c(panel)->addContent(panel, createSurroundBtn(panel,S_TYPE_DATE,m_btnFont),
            S_BUTTON_DATE_X,S_BUTTON_DATE_Y);

    _c(panel)->addContent(panel, createSurroundBtn(panel,S_TYPE_TIME,m_btnFont), 
            S_BUTTON_TIME_X,S_BUTTON_TIME_Y);

    _c(panel)->addContent(panel, createPickerTitle(panel,S_TYPE_DATE,m_btnFont),
            S_SET_DATE_TITLE_Y_X,S_SET_TITLE_Y);
    _c(panel)->addContent(panel, createPickerTitle(panel,S_TYPE_TIME,m_btnFont),
            S_PICKER_HIDE_X,S_PICKER_HIDE_Y);

    g_datePiece = date;
    g_timePiece = time;
#if WIDGETHOST_FROM_DESKTOP
    g_dateWidget = date->hosted;
    g_timeWidget = time->hosted;
    _c(g_dateWidget)->addRef(g_dateWidget);
    _c(g_timeWidget)->addRef(g_timeWidget);
#endif
    return (mHotPiece *)panel; 
}

void SettingActivity::initClockWin(HWND hwnd)
{
    mDatePicker* picker;
    TIMESERVICE->addEventListener((void*)hwnd, TimeService::MINUTE, onMinute);
    TIMESERVICE->addEventListener((void*)hwnd, TimeService::HOUR, onMinute);
    TIMESERVICE->addEventListener((void*)hwnd, TimeService::MDAY, onMinute);
    TIMESERVICE->addEventListener((void*)hwnd, TimeService::MONTH, onMinute);
    TIMESERVICE->addEventListener((void*)hwnd, TimeService::YEAR, onMinute);
    assert(g_datePiece);
    picker = (mDatePicker*)_c(g_datePiece)->getHosted(g_datePiece);
    _c(picker)->setDate(picker, TIMESERVICE->get(TimeService::YEAR), 
            TIMESERVICE->get(TimeService::MONTH),
            TIMESERVICE->get(TimeService::MDAY));
    g_curPiece = g_datePiece;
}

void SettingActivity::releaseClockView(HWND hwnd)
{
    TIMESERVICE->removeEventListener((void*)hwnd);
    TIMESERVICE->removeEventListener((void*)hwnd);
    TIMESERVICE->removeEventListener((void*)hwnd);
    TIMESERVICE->removeEventListener((void*)hwnd);
    TIMESERVICE->removeEventListener((void*)hwnd);
}
