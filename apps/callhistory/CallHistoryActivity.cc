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

#include "register.h"

#include "museritem.h"
#include "mspeedmeter.h"
#include "CallLogService.hh"
#include "CallHistoryActivity.hh"
#include "mcallhistorytableviewpiece.hh"
#include "../common/mactivitytoolbarpiece.h"


/*register activity*/
REGISTER_ACTIVITY(CallHistoryActivity);

/*static class member*/
mHotPiece *CallHistoryActivity::m_tableview;
Intent *CallHistoryActivity::m_intent;

/*create title button*/
static BOOL allcalls_btn_handler(mObject *listener, mObject *sender,int event_id, DWORD param)
{
    CallHistoryActivity *act;
    mCallHistoryTableViewPiece *tv = (mCallHistoryTableViewPiece *)CallHistoryActivity::m_tableview;

    assert(NCSN_ABP_CLICKED);

    if(!tv->ismissed)
        return TRUE;

    tv->ismissed = 0;
    tv->isSelect = TRUE;

    if (tv->data) {
        delete tv->data;
    }

    tv->data = GET_CALLLOG_SERVICE()->getAllCallLog();
    _c(tv)->reloadData(tv);

    act = (CallHistoryActivity*)Activity::getActivityFromHWND(((mWidget*)listener)->hwnd);

    _c(act->m_btn[BTN_CLEAR])->setProperty(act->m_btn[BTN_CLEAR], NCSP_3DBUTTONPIECE_TEXT, (DWORD)"Edit");

    PanelPiece_invalidatePiece(act->m_btn[BTN_CLEAR],NULL);

    return TRUE;
}

static BOOL missedcalls_btn_handler(mObject *listener, mObject *sender,int event_id, DWORD param)
{
    CallHistoryActivity *act;
    mCallHistoryTableViewPiece *tv = (mCallHistoryTableViewPiece *)CallHistoryActivity::m_tableview;

    assert(NCSN_ABP_CLICKED);

    if(tv->ismissed)
        return TRUE;

    tv->ismissed = 1;
    tv->isSelect = TRUE;
    if (tv->data)
    {
        delete tv->data;
    }
    tv->data = GET_CALLLOG_SERVICE()->getLostCall();
    _c(tv)->moveViewport(tv, 0, 0);
    _c(tv)->reloadData(tv);

    act = (CallHistoryActivity*)Activity::getActivityFromHWND(((mWidget*)listener)->hwnd);
    _c(act->m_btn[BTN_CLEAR])->setProperty(act->m_btn[BTN_CLEAR], NCSP_3DBUTTONPIECE_TEXT, (DWORD)"Edit");
    PanelPiece_invalidatePiece(act->m_btn[BTN_CLEAR],NULL);

    return TRUE;
}

static BOOL edit_btn_handler(mObject *listener, mObject *sender,int event_id, DWORD param)
{
    m3DButtonPiece *btn;
    char *btnText;
    mCallHistoryTableViewPiece *tv = (mCallHistoryTableViewPiece *)CallHistoryActivity::m_tableview;

    assert(NCSN_ABP_CLICKED);

    btn = (m3DButtonPiece*)sender;
    btnText = (char *)_c(btn)->getProperty(btn, NCSP_3DBUTTONPIECE_TEXT);
    assert(btnText);

    //warning:the opt can not update rect
    if(strcmp(btnText, "Edit") == 0){
        tv->isSelect = FALSE;
        btnText = (char*)"Finish";
    }
    else{
        tv->isSelect = TRUE;
        btnText = (char*)"Edit";
    }
    _c(btn)->setProperty(btn, NCSP_3DBUTTONPIECE_TEXT, (DWORD)btnText);

    _c(tv)->changeMode(tv);

    return TRUE;
}

void CallHistoryActivity::createNavBtns(mWidget *self){
    int i;
    m3DButtonPiece *btn;
    mShapeTransRoundPiece *bkgnd;
    ARGB normalColors[] = {0xffffffff, 0xffd1d1d1};
    ARGB borderColors[] = {0xffaaaaaa, 0xff888888};
    ARGB pushedColors[] = {0xa0e2edf9, 0xa0a1abb7, 0xa0e2edf9};
    RECT rc[BTN_NUM] = {{0,0,CH_BTN_ALLCALLS_W,CH_BTN_H}, {0,0,CH_BTN_MISSEDCALLS_W,CH_BTN_H}, {0,0,CH_BTN_CLEAR_W,CH_BTN_H},};
//    RECT rcSep;
    const char *title[BTN_NUM] = {"All calls", "Missed calls", "Edit"};
    NCS_CB_ONPIECEEVENT cb[BTN_NUM] = {allcalls_btn_handler, missedcalls_btn_handler, edit_btn_handler};

    //new a button
    for(i=BTN_ALLCALLS; i<BTN_NUM; i++){
        btn = NEWPIECE(m3DButtonPiece);
        _c(btn)->setRect (btn, rc+i);
        _c(btn)->setProperty(btn, NCSP_3DBUTTONPIECE_TEXTLOGFONT, (DWORD)m_btnFont);
        _c(btn)->setProperty(btn, NCSP_3DBUTTONPIECE_TEXT, (DWORD)title[i]);
        _c(btn)->setProperty(btn, NCSP_3DBUTTONPIECE_TEXTCOLOR, CH_BTN_COLOR);
        
        //btn background and color
        bkgnd = (mShapeTransRoundPiece*)_c(btn)->getBkgndPiece(btn);
        _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_RADIUS, 4);
        //_c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_BORDERSIZE, 2);
        _c(btn)->setGradientBackgroundColor(btn, normalColors, NULL, TABLESIZE(normalColors),
                pushedColors, NULL, TABLESIZE(pushedColors));
        _c(btn)->setGradientBorderColor(btn, borderColors, NULL, TABLESIZE(borderColors));
        _c(btn)->setStates(btn);

        //btn event handler
        ncsAddEventListener((mObject*)btn, (mObject*)self, (NCS_CB_ONPIECEEVENT)cb[i], NCSN_ABP_CLICKED);
        m_btn[i] = (mHotPiece*)btn;
    }
    m_editButton = m_btn[2];/* for edit button.*/

    bkgnd = (mShapeTransRoundPiece*)_c(btn)->getBkgndPiece((m3DButtonPiece*)m_btn[0]);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_CORNERFLAG, 
            TRANROUND_CORNERFLAG_LEFTTOP | TRANROUND_CORNERFLAG_LEFTBOTTOM);
    bkgnd = (mShapeTransRoundPiece*)_c(btn)->getBkgndPiece((m3DButtonPiece*)m_btn[1]);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_CORNERFLAG, 
            TRANROUND_CORNERFLAG_RIGHTTOP | TRANROUND_CORNERFLAG_RIGHTBOTTOM);
#if 0
    m_separator = (mHotPiece*)NEWPIECE(mExSeparatorPiece);
    SetRect(&rcSep, 0, 0, CH_BTN_SEPARATOR_W, CH_BTN_SEPARATOR_H);
    _c(m_separator)->setRect(m_separator, &rcSep);
    _c(m_separator)->setProperty(m_separator, NCSP_EXSEPARATOR_DIRECTION, EXSEPARATOR_DIRECTION_VERTICAL);
#endif
}

/*create title*/
static void callhistory_gradient_color(mShapeTransRoundPiece *piece, ARGB *colors, float *pos, int num) {
    HBRUSH brush;

    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    brush = _c(piece)->getBrush(piece);
    MGPlusSetLinearGradientBrushColorsEx(brush,colors,num,pos);
}
mHotPiece *CallHistoryActivity::createTitle(mWidget *self){
    RECT rc;
    mPanelPiece *title;
    mShapeTransRoundPiece *titlebg;
    ARGB title_gradient_color[] = {0xffd0d571, 0xffb5c363, 
        0xff9eb23a,0xff7f9e2c,0xff597311,0xff597311};
    float title_gradient_pos[] = {0.0, 0.49,0.50,0.96,0.97,1.0};
    //create title
    title = NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, CH_TITLE_W, CH_TITLE_H);
    _c(title)->setRect(title, &rc);

    //create title background
    titlebg = NEWPIECE(mShapeTransRoundPiece);
    _c(titlebg)->setRect(titlebg, &rc);
    _c(titlebg)->setProperty (titlebg, NCSP_TRANROUND_CORNERFLAG,0);
    callhistory_gradient_color(titlebg, title_gradient_color, title_gradient_pos, TABLESIZE(title_gradient_color));

    //create title button
    createNavBtns(self);

    //add background && button to title
    _c(title)->addContent(title, (mHotPiece*)titlebg,  0, 0);
    _c(title)->addContent(title, m_btn[0], CH_BTN_ALLCALLS_X, CH_BTN_Y);
    _c(title)->addContent(title, m_btn[1], CH_BTN_MISSEDCALLS_X, CH_BTN_Y);
    _c(title)->addContent(title, m_btn[2], CH_BTN_CLEAR_X, CH_BTN_Y);
//    _c(title)->addContent(title, m_separator, CH_BTN_SEPARATOR_X, CH_BTN_SEPARATOR_Y);
    
    return (mHotPiece*)title;
}

/*create table*/
mHotPiece *CallHistoryActivity::createTable(mWidget *self){
    mCallHistoryTableViewPiece *chview;
    RECT rc;

    chview = NEWPIECEEX(mCallHistoryTableViewPiece, 0);
    assert(NULL != chview);
    m_tableview = (mHotPiece *)chview;
    SetRect(&rc, 0, 0, CH_TABLE_W, CH_TABLE_H);
    _c(chview)->setRect(chview, &rc);
    //_c(chview)->reloadData(chview);
    chview->activity = this;

    return (mHotPiece*)chview;
}

/*create toolbar*/
static BOOL callcontacts_event_handler(mObject *self, mObject *piece, int event_id,  DWORD param)
{
    // ACTIVITYSTACK->push("PhoneContactsActivity", NULL);
    ACTIVITYSTACK->switchTo("PhoneContactsActivity", NULL);
    return TRUE;
}

static BOOL calldial_event_handler(mObject *self, mObject *piece, int event_id,  DWORD param)
{
    // ACTIVITYSTACK->push("DialPadActivity", NULL);
    ACTIVITYSTACK->switchTo("DialPadActivity", NULL);
    return TRUE;
}

const static char *g_pic[] = {
    "res/address-book/address_book_calllog.png",
    "res/address-book/address_book_dial.png",
    "res/address-book/address_book_contacts.png",
};

enum _ACTIVE_INDEX{
    TOOLBAR_IMAGE_CALLLOG,
    TOOLBAR_IMAGE_DIAL,
    TOOLBAR_IMAGE_CONTACTS,
    TOOLBAR_MAX,
};
mHotPiece *CallHistoryActivity::createToolbar(mWidget *self) {
    BITMAP *bmp;
    mActivityToolbarPiece *toolbar;
    RECT rc = {0, 0, CH_TOOLBAR_W, CH_TOOLBAR_H};

    toolbar = NEWPIECEEX (mActivityToolbarPiece, 
            TOOLBAR_BUTTON_LEFT|TOOLBAR_BUTTON_CENTER|TOOLBAR_BUTTON_RIGHT|TOOLBAR_BUTTON_LEFT_HI);

    _c(toolbar)->setRect (toolbar, &rc);

    bmp = (BITMAP*)Load32Resource (g_pic[TOOLBAR_IMAGE_CALLLOG], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_LEFT, bmp);

    bmp = (BITMAP*)Load32Resource (g_pic[TOOLBAR_IMAGE_CONTACTS], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_RIGHT, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_RIGHT, callcontacts_event_handler);

    bmp = (BITMAP*)Load32Resource (g_pic[TOOLBAR_IMAGE_DIAL], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_CENTER, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_CENTER, calldial_event_handler);

    return (mHotPiece*)toolbar;
}

/*create body*/
void CallHistoryActivity::createBody(mWidget *self, mContainerCtrl *ct){
    RECT rc;
    mPanelPiece *body;

    body = NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, ACTIVITY_W, ACTIVITY_H);
    _c(body)->setRect(body, &rc);
    _c(ct)->setBody(ct, (mHotPiece*)body);

    _c(body)->addContent(body, createTitle(self), 0, 0);
    _c(body)->addContent(body, createTable(self), 0, CH_TITLE_H);
    _c(body)->addContent(body, createToolbar(self), 0, CH_TITLE_H + CH_TABLE_H);
}

void CallHistoryActivity::initResource(void){
    m_btnFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, 
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            CH_BTN_TEXT_H, 0);
}
void CallHistoryActivity::releaseResource(void) {
    DestroyLogFont(m_btnFont);
}

static BOOL main_onCreate(mWidget* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr = (mContainerCtrl*)ncsGetChildObj(self->hwnd, CH_IDC_CTNR);
    CallHistoryActivity* activity = (CallHistoryActivity*)Activity::getActivityFromHWND(self->hwnd);

    activity->createBody(self, ctnr);

    return TRUE;
}

static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static void main_onClose(mWidget* self, int message)
{
    return;
}

static void main_onPaint(mWidget* _this, HDC hdc, const PCLIPRGN clipRgn)
{
    return;
}

static BOOL main_userApp(mMainWnd* self, int msg, DWORD wparam, DWORD lparam)
{
    mCallHistoryTableViewPiece *tv = (mCallHistoryTableViewPiece *)CallHistoryActivity::m_tableview;
    int select = 0;
    select = (int)wparam;

    if (tv->data)
    {
        delete tv->data;
    }
    if (select == 1)
    {
        tv->ismissed = 1;
        tv->data = GET_CALLLOG_SERVICE()->getLostCall();
    }
    else
    {
        tv->ismissed = 0;
        tv->data = GET_CALLLOG_SERVICE()->getAllCallLog();
    }
    _c(tv)->reloadData(tv);
    return TRUE;
}

static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE, reinterpret_cast<void*>(main_onCreate)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {MSG_CLOSE, reinterpret_cast<void*>(main_onClose)},
    {MSG_USER_APP_DATA, reinterpret_cast<void*>(main_userApp)},
    {MSG_PAINT, reinterpret_cast<void*>(main_onPaint)},
    {MSG_LBUTTONDOWN, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_LBUTTONUP, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_MOUSEMOVE, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {0, NULL}
};

static NCS_WND_TEMPLATE _ctrl_templ[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        CH_IDC_CTNR,
        0, 0, ACTIVITY_W, ACTIVITY_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL, NULL, 0, 0
    },
};
static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_NONE,
        "call-history",
        NULL,
        NULL,
        main_handlers,
        _ctrl_templ,
        TABLESIZE(_ctrl_templ),
        0,
        0, 0,
};
CallHistoryActivity::CallHistoryActivity() :
    NCSActivity(&mymain_templ)
{ 
    MGNCS_INIT_CLASS(mCallHistoryTableViewPiece);
    MGNCS_INIT_CLASS(mActivityToolbarPiece);

    //m_style = STYLE_PUSH;
    m_pushIndex = 1;

    m_intent = Intent::newIntent();

    initResource();
}

CallHistoryActivity::~CallHistoryActivity(void) 
{
    releaseResource();
    Intent::deleteIntent(m_intent);
}

