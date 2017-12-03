#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mcontainerctrl.h"
#include "mpanelpiece.h"
#include "mscrollviewpiece.h"
#include "mtransroundpiece.h"
#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "manimationeditpiece.h"
#include "mbuttonpanelpiece.h"
#include "mtextpiece.h"
#include "register.h"

#include "SettingService.hh"

#include "museritem.h"
#include "SettingActivity.hh"
#include "SettingMainWinPiece.hh"
#include "SettingSubWinPiece.hh"
#include "mnavigationpanelpiece.h"
#include "mspeedmeter.h"

mHotPiece *SettingActivity::m_settingMainWinPiece = NULL;
mHotPiece *SettingActivity::m_settingSubWinPiece = NULL;
StringArray SettingActivity::m_settingData;
mNavigationPanelPiece *SettingActivity::m_nav = NULL;
mNavigationItem *SettingActivity::m_navItems[TYPE_WIN_MAX_NUM] = {NULL};

REGISTER_ACTIVITY(SettingActivity);

static ARGB title_gradient_color[] = {0xffd0d571, 0xffb5c363, 
    0xff9eb23a,0xff7f9e2c,0xff597311,0xff597311};
static float title_gradient_pos[] = {0.0, 0.49,0.50,0.96,0.97,1.0};

static void setting_gradient_color(mShapeTransRoundPiece *piece,
        ARGB *colors, float *pos, int num)
{
    HBRUSH brush;
    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    brush = _c(piece)->getBrush(piece);
    MGPlusSetLinearGradientBrushColorsEx(brush,colors,num,pos);
    return;
}

static BOOL settingInitData(StringArray *settingdata)
{
    StringArray strs = GET_SETTINGSERVICE()->getAllItemNames();
    if (settingdata != NULL)
        *settingdata = strs;
    return TRUE;
}

mHotPiece *SettingActivity::createMainWinView()
{
    return (mHotPiece *)NEWPIECEEX(mSettingMainWinPiece,this);
}

mHotPiece *SettingActivity::createSubWinView(const char* name)
{
    return (mHotPiece *)NEWPIECEEX(mSettingSubWinPiece,(DWORD)name);
}

int SettingActivity::onBack()
{
    if (_c(m_nav)->currentIsRoot(m_nav))
        return 0;

    _c(m_nav)->pop(m_nav);
    return 1;
}

static BOOL btnOk_event_handler(mWidget *self, mHotPiece *piece,int event_id, DWORD param)
{
    assert(NCSN_ABP_CLICKED);

    ActivityStack::singleton()->pop();
    return TRUE;
}

void SettingActivity::createNavigatorBtns(mNavigationItem* self)
{
    m3DButtonPiece *mbutton;
    RECT rc = {0,0,SETTING_OK_BTN_W,SETTING_OK_BTN_H};
    mbutton = (m3DButtonPiece*)_c(self)->createDefaultButton(self, "OK",0);
    assert(mbutton);

    _c(mbutton)->setRect (mbutton, &rc);
    m_okBtn = (mHotPiece *)mbutton;
    ncsAddEventListener((mObject*)m_okBtn, (mObject*)m_okBtn,
            (NCS_CB_ONPIECEEVENT)btnOk_event_handler, NCSN_ABP_CLICKED);

    _c(self)->setProperty(self,NCSP_NAVIGATIONITEM_RIGHT_BUTTON, (DWORD)m_okBtn);
    return;
}

void SettingActivity::createCtnrBody(mContainerCtrl* ctnr,mNavigationItem *rootItem)
{
    RECT rc;
    assert(ctnr);
    assert(rootItem);
    m_barBack = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    SetRect(&rc, 0, 0, NAVIGATIONBAR_W, NAVIGATIONBAR_H);
    _c(m_barBack)->setRect(m_barBack, &rc);
    _c(m_barBack)->setProperty (m_barBack, NCSP_TRANROUND_CORNERFLAG,0);
    setting_gradient_color((mShapeTransRoundPiece *)m_barBack,
            title_gradient_color,title_gradient_pos,TABLESIZE(title_gradient_color));
    m_nav = CreateNavigationPanelPieceWithRootView(rootItem);
    SetRect(&rc, 0, 0, ACTIVITY_W, ACTIVITY_H);
    _c(m_nav)->setRect(m_nav, (const RECT*)&rc);
    _c(m_nav)->setProperty(m_nav, NCSP_NAVIGATIONPANELPIECE_BAR_BKG, (DWORD)m_barBack);
    _c(ctnr)->setBody(ctnr, (mHotPiece*)m_nav);
}
void SettingActivity::createUIMainWin(HWND hwnd)
{
    RECT rc = {0,0,SETTING_ITEM_W,SETTING_TABLE_H};

    mSettingMainWinPiece *mainWinPiece = (mSettingMainWinPiece *)createMainWinView();
    assert(mainWinPiece);
    _c(mainWinPiece)->setRect(mainWinPiece, &rc);


    _c(mainWinPiece)->myreloadData(mainWinPiece,TYPE_ALL);

    m_navItems[TYPE_MAIN_WIN_UI] = CreateNavigationItem(
            (mHotPiece *)mainWinPiece, "Settings", NAVIGATION_STYLE_NORMAL);

    assert(m_navItems[TYPE_MAIN_WIN_UI]);
    ADDREF(m_navItems[TYPE_MAIN_WIN_UI]);


    m_settingMainWinPiece = (mHotPiece *)mainWinPiece;
}

void SettingActivity::createUIClockWin(HWND hwnd)
{
    RECT rc = {0,0,SETTING_ITEM_W,SETTING_TABLE_H};

    mPanelPiece *clockPiece = NULL;

    if (m_navItems[TYPE_CLOCK_WIN_UI] != NULL)
    {
        _c(m_navItems[TYPE_CLOCK_WIN_UI])->setProperty(m_navItems[TYPE_CLOCK_WIN_UI], 
            NCSP_NAVIGATIONITEM_TITLE, (DWORD)"Date and Time");
        return;
    }
    clockPiece = (mPanelPiece *)createClockView(hwnd);
    assert(clockPiece);
    _c(clockPiece)->setRect(clockPiece, &rc);

    m_navItems[TYPE_CLOCK_WIN_UI] = CreateNavigationItem(
            (mHotPiece *)clockPiece, "Clock", NAVIGATION_STYLE_NORMAL);

    assert(m_navItems[TYPE_CLOCK_WIN_UI]);
    ADDREF(m_navItems[TYPE_CLOCK_WIN_UI]);
}

void SettingActivity::createUISubWin(const char* name)
{
    RECT rc = {0, 0, SETTING_ITEM_W,SETTING_TABLE_H};
    mSettingSubWinPiece *subWinPiece = NULL;

    if (m_navItems[TYPE_SUB_WIN_UI] != NULL)
    {
        _c(m_navItems[TYPE_SUB_WIN_UI])->setProperty(m_navItems[TYPE_SUB_WIN_UI], 
            NCSP_NAVIGATIONITEM_TITLE, (DWORD)name);
        return;
    }
    subWinPiece = (mSettingSubWinPiece *)createSubWinView(name);
    assert(subWinPiece);
    _c(subWinPiece)->setRect(subWinPiece, &rc);

    
    m_navItems[TYPE_SUB_WIN_UI] = CreateNavigationItem(
            (mHotPiece *)subWinPiece, name, NAVIGATION_STYLE_NORMAL);

    assert(m_navItems[TYPE_SUB_WIN_UI]);
    ADDREF(m_navItems[TYPE_SUB_WIN_UI]);
#if 0
    _c(m_navItems[TYPE_SUB_WIN_UI])->setProperty(m_navItems[TYPE_SUB_WIN_UI], 
            NCSP_NAVIGATIONITEM_BACKGROUND, (DWORD)m_barBack);
#endif
    m_settingSubWinPiece = (mHotPiece *)subWinPiece;
}

void SettingActivity::initResource()
{
    m_btnFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, 
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            SETTING_OK_BTN_TEXT_H, 0);

    m_itemTextFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, 
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            SETTING_ITEM_TEXT_H, 0);
    return;
}

void SettingActivity::releaseResource()
{
    DestroyLogFont(m_btnFont);
    DestroyLogFont(m_itemTextFont);
    return;
}

static BOOL on_timer(
        mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mMainWnd* self = (mMainWnd *)listener;
    mTimer *timer = (mTimer *)sender;
    SettingActivity* act = 
        (SettingActivity*)Activity::getActivityFromHWND (self->hwnd);
    act->createUIClockWin(self->hwnd);
    act->initClockWin(self->hwnd);
    _c(timer)->stop(timer);
    return TRUE;
}

static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mTimer *timer;
    mContainerCtrl *ctnr = (mContainerCtrl *)ncsGetChildObj(self->hwnd, SETTING_IDC_CTNR);
    SettingActivity* act = 
        (SettingActivity*)Activity::getActivityFromHWND (self->hwnd);

    settingInitData(&SettingActivity::m_settingData);

    act->createUIMainWin(self->hwnd);
    act->createNavigatorBtns(act->getNavItem(TYPE_MAIN_WIN_UI));
    act->createCtnrBody(ctnr,act->getNavItem(TYPE_MAIN_WIN_UI));
    timer = (mTimer*)ncsGetChildObj(self->hwnd, SETTING_TIMER);
    if(timer){
        ncsAddEventListener((mObject *)timer, (mObject *)self, 
                (NCS_CB_ONPIECEEVENT)on_timer, MSG_TIMER);
    }
#if 0
    _c(timer)->start(timer);
#else
    act->createUIClockWin(self->hwnd);
    act->initClockWin(self->hwnd);
#endif
    return TRUE;
}


static void main_onClose(mWidget* self, int message)
{
    return;
}


static void main_onPaint (mWidget* _this, HDC hdc, const PCLIPRGN clipRgn)
{
    return;
}

static void main_onDestroy(mWidget* self, int message)
{
    SettingActivity* activity= 
        (SettingActivity*)Activity::getActivityFromHWND (self->hwnd);
    assert (activity);
    if (activity->getNavItem(TYPE_MAIN_WIN_UI) != NULL)
    {
        UNREF(activity->getNavItem(TYPE_MAIN_WIN_UI));
    }
    if (activity->getNavItem(TYPE_SUB_WIN_UI) != NULL)
    {
        UNREF(activity->getNavItem(TYPE_SUB_WIN_UI));
    }
    if (activity->getNavItem(TYPE_CLOCK_WIN_UI) != NULL)
    {
        UNREF(activity->getNavItem(TYPE_CLOCK_WIN_UI));
    }
    DestroyNavigationItem(activity->getNavItem(TYPE_MAIN_WIN_UI));
    DestroyNavigationItem(activity->getNavItem(TYPE_SUB_WIN_UI));
    DestroyNavigationItem(activity->getNavItem(TYPE_CLOCK_WIN_UI));
    return ;
}
static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}
static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE, reinterpret_cast<void*>(main_onCreate)},
    {MSG_CLOSE, reinterpret_cast<void*>(main_onClose)},
    {MSG_DESTROY,       reinterpret_cast<void*>(main_onDestroy)},
    {MSG_PAINT, reinterpret_cast<void*>(main_onPaint)},
    {MSG_LBUTTONDOWN, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_LBUTTONUP, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_MOUSEMOVE, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {0, NULL}
};

static NCS_PROP_ENTRY timer_props[] = {
    {NCSP_TIMER_INTERVAL, 50},//unit:10ms
    {0, 0},
};
static NCS_WND_TEMPLATE _ctrl_templ[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        SETTING_IDC_CTNR,
        0, 0, ACTIVITY_W, ACTIVITY_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL, NULL, 0, 0
    },
    {
        NCSCTRL_TIMER,
        SETTING_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timer_props, NULL, NULL, NULL, 0, 0, 
    },
};
static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_NONE,
        "sys-setting",
        NULL,
        NULL,
        main_handlers,
        _ctrl_templ,
        TABLESIZE(_ctrl_templ),
        0,
        0, 0,
};

SettingActivity::SettingActivity() :
    NCSActivity( &mymain_templ)
{ 
    int i;
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_INIT_CLASS(mLayoutManager);
    MGNCS_INIT_CLASS(mCenterHBox);
    MGNCS_INIT_CLASS(mButtonPanelPiece);
    MGNCS_INIT_CLASS(mTextPiece);
    MGNCS_INIT_CLASS(mSettingMainWinPiece);
    MGNCS_INIT_CLASS(mSettingSubWinPiece);
    MGNCS_INIT_CLASS(mNavigationPanelPiece);
    initResource();

    m_settingMainWinPiece = NULL;
    m_settingSubWinPiece = NULL;
    m_nav = NULL;
    for (i=TYPE_MAIN_WIN_UI;i<TYPE_WIN_MAX_NUM;i++)
    {
        m_navItems[i] = NULL;
    }
}

SettingActivity::~SettingActivity(void) 
{
    releaseResource();
}
int SettingActivity::onStop()
{
    releaseClockView(this->hwnd());
    return 0;
}
