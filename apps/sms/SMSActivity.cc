#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "register.h"

#include "SMSActivity.hh"
#include "SMSListViewPiece.hh"
#include "msmsdetailview.h"
#include "TimeService.hh"
#include "ContactService.hh"

#include "../common/mactivitytoolbarpiece.h"

#include "SmsService.hh"

#define SMS_DETAIL_TOOLBAR_H	50
#define SMS_DETAIL_VIEW_H	(ACTIVITY_H - NAVIGATIONBAR_H)
#define SMS_DETAIL_TABLE_H	(ACTIVITY_H - NAVIGATIONBAR_H - SMS_DETAIL_TOOLBAR_H)

//#define TOOLBAR 
mNavigationPanelPiece *SMSActivity::m_nav = NULL;

REGISTER_ACTIVITY(SMSActivity);

static ARGB title_gradient_color[] = {0xffd0d571, 0xffb5c363, 
        0xff9eb23a,0xff7f9e2c,0xff597311,0xff597311};
static float title_gradient_pos[] = {0.0, 0.49,0.50,0.96,0.97,1.0};

static void sms_gradient_color(mShapeTransRoundPiece *piece,
                               ARGB *colors, float *pos, int num)
{
    HBRUSH brush;
    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    brush = _c(piece)->getBrush(piece);
    MGPlusSetLinearGradientBrushColorsEx(brush,colors,num,pos);
    return;
}
#if 0
static void SMSDeinitData(SmsService::SmsSession &session)
{
#if 0
    while (session.size() > 0) {
        for_each(session.begin()->second.begin(), 
                session.begin()->second.end(), DeleteObject());
        session.begin()->second.clear();
        session.erase(session.begin());
    }

    session.clear();
#endif

//    delete GET_SMSSERVICE();
}
#endif
static void SMSInitData(SmsService::SmsSession& session,StringArray& keys)
{
    SmsService::SmsSession::iterator iter;
    std::vector<int> numbers;
    std::vector<int>::iterator int_iter;

    session = GET_SMSSERVICE()->getAllSession();
    for (iter = session.begin();iter != session.end();iter ++)
    {
        numbers.push_back(iter->second[0]->m_smsTime.get(TimeService::SEC_FROM_EPOCH));
    }
    if (numbers.size() <= 0)
    {
        return;
    }
    sort(numbers.begin(),numbers.end());
    for (int_iter=numbers.end() - 1;;int_iter--)
    {
        for (iter = session.begin();iter != session.end();iter ++)
        {
            if (*int_iter == iter->second[0]->m_smsTime.get(TimeService::SEC_FROM_EPOCH))
            {
                keys.push_back(iter->first);
                break;
            }
        }
        if (int_iter == numbers.begin())
            break;
    }
}

mHotPiece *SMSActivity::createSMSListView()
{
    return (mHotPiece *)NEWPIECEEX(mSMSListPiece,(DWORD)this);
}

int SMSActivity::onBack()
{
    if (_c(m_nav)->currentIsRoot(m_nav))
        return 0;

    _c(m_nav)->pop(m_nav);
    return 1;
}

#ifdef TOOLBAR
BOOL onSmsDetailCall(mWidget* self, mHotPiece *piece,
        int event_id, DWORD param)
{
    mPanelPiece* parent = PanelPiece_getTopPanel(piece->parent);
    assert(parent);
    SMSActivity* _this = (SMSActivity*) Activity::getActivityFromHWND(
        GetMainWindowHandle(_c(parent)->getOwner(parent)->hwnd));
    assert(_this);
    
    if (event_id == NCSN_ABP_CLICKED) {
        printf("\tCall clicked\n");
        _this->m_phoneCallIntent->putExtra("number", std::string(_this->m_contactName));
        ACTIVITYSTACK->switchTo("PhoneCallActivity", _this->m_phoneCallIntent);
    }

    return TRUE;
}
#endif


#ifdef TOOLBAR
static mActivityToolbarPiece *create_toolbar ()
{
    RECT rc = {0, 0, ACTIVITY_W, SMS_DETAIL_TOOLBAR_H};
    mActivityToolbarPiece *toolbar;

    toolbar = NEWPIECEEX (mActivityToolbarPiece, 
            TOOLBAR_BUTTON_LEFT | TOOLBAR_BUTTON_RIGHT);

    _c(toolbar)->setRect (toolbar, &rc);
    _c (toolbar)->setButtonName (toolbar, ACTIVITYTOOLBAR_LEFT, "Call");
    _c (toolbar)->setButtonName (toolbar, ACTIVITYTOOLBAR_RIGHT, "Reply");

    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_LEFT, (NCS_CB_ONPIECEEVENT)onSmsDetailCall);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_RIGHT, NULL);

    return toolbar;
}
#endif


mHotPiece *SMSActivity::createSMSDetailView()
{
    // return (mHotPiece *)NEWPIECEEX(mSmsDetailView,(DWORD)this);
    
    mPanelPiece* view = (mPanelPiece*)NEWPIECEEX(mPanelPiece, (DWORD)0);
    mTableViewPiece* table = (mTableViewPiece*)NEWPIECEEX(mSmsDetailView, (DWORD)this);
#ifdef TOOLBAR
    mPanelPiece* toolbar = (mPanelPiece*)create_toolbar();
#endif

    RECT rc;

    rc.left = rc.top = 0;
    rc.right = ACTIVITY_W;

    rc.bottom = SMS_DETAIL_VIEW_H;
    _c(view)->setRect(view, &rc);

#ifdef TOOLBAR
    rc.bottom = SMS_DETAIL_TABLE_H;
#else
    rc.bottom = SMS_DETAIL_VIEW_H;
#endif
    _c(table)->setRect(table, &rc);

#ifdef TOOLBAR
    rc.bottom = SMS_DETAIL_TOOLBAR_H;
    _c(toolbar)->setRect(toolbar, &rc);
#endif
    _c(view)->addContent(view, (mHotPiece*)table, 0, 0);
#ifdef TOOLBAR
    _c(view)->addContent(view, (mHotPiece*)toolbar, 0, SMS_DETAIL_TABLE_H);
#endif
    
    _c(table)->setSeparatorStyle(table, NCS_TABLEVIEW_SEPARATORSTYLE_NONE);
    _c(table)->reloadData(table);

    m_smsDetailPiece = (mHotPiece*)table;

    return (mHotPiece*)view;
}

static BOOL btnOk_event_handler(mWidget *self, mHotPiece *piece,int event_id, DWORD param)
{
    char *btnText;
    m3DButtonPiece *button = (m3DButtonPiece *)self;
    mSMSListPiece *SMSListPiece;

    assert(button);

    SMSListPiece = (mSMSListPiece *)param;

    assert(SMSListPiece);
    assert(NCSN_ABP_CLICKED);

    btnText = (char *)_c(button)->getProperty(button, NCSP_3DBUTTONPIECE_TEXT);
    if (strcmp(btnText,"Edit") == 0) {
        _c(button)->setProperty(button, NCSP_3DBUTTONPIECE_TEXT, (DWORD)"Finish");
        SMSListPiece->editMode = TRUE;
    }
    else{
        _c(button)->setProperty(button, NCSP_3DBUTTONPIECE_TEXT, (DWORD)"Edit");
        SMSListPiece->editMode = FALSE;
    }

    //_c(SMSListPiece)->reloadData(SMSListPiece);
    _c(SMSListPiece)->changeMode(SMSListPiece);
    return TRUE;
}

void SMSActivity::createNavigatorBtns(mNavigationItem *self)
{
    m3DButtonPiece *mbutton;
    RECT rc = {0,0,SMS_BTN_W,SMS_BTN_H};
    mbutton = (m3DButtonPiece*)_c(self)->createDefaultButton(self, "Edit",(DWORD)m_smsListPiece);
    assert(mbutton);

    _c(mbutton)->setRect (mbutton, &rc);
    m_editBtn = (mHotPiece *)mbutton;
    ncsAddEventListener((mObject*)m_editBtn, (mObject*)m_editBtn,
            (NCS_CB_ONPIECEEVENT)btnOk_event_handler, NCSN_ABP_CLICKED);

    _c(self)->setProperty(self,NCSP_NAVIGATIONITEM_RIGHT_BUTTON, (DWORD)m_editBtn);

#if 1
    m_barBack = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    SetRect(&rc, 0, 0, NAVIGATIONBAR_W, NAVIGATIONBAR_H);
    _c(m_barBack)->setRect(m_barBack, &rc);
    _c(m_barBack)->setProperty (m_barBack, NCSP_TRANROUND_CORNERFLAG,0);
    sms_gradient_color((mShapeTransRoundPiece *)m_barBack,
            title_gradient_color,title_gradient_pos,TABLESIZE(title_gradient_color));
#endif
    return;
}

void SMSActivity::createCtnrBody(mContainerCtrl* ctnr,mNavigationItem *rootItem)
{
    RECT rc;
    assert(ctnr);
    assert(rootItem);
#if 0
    _c(m_navItems[SMS_LIST_ITEM])->setProperty(m_navItems[SMS_LIST_ITEM], 
            NCSP_NAVIGATIONITEM_BACKGROUND, (DWORD)m_barBack);
#endif
    m_nav = ncsCreateNavigationPanelPieceWithRootView(rootItem);
    SetRect(&rc, 0, 0, ACTIVITY_W, ACTIVITY_H);
    _c(m_nav)->setRect(m_nav, (const RECT*)&rc);
    _c(m_nav)->setProperty(m_nav, NCSP_NAVIGATIONPANELPIECE_BAR_BKG, (DWORD)m_barBack);
    _c(ctnr)->setBody(ctnr, (mHotPiece*)m_nav);
}

void SMSActivity::createSMSListWin()
{
    RECT rc = {0,0,SMS_ITEM_W,SMS_TABLE_H};

    mSMSListPiece *SMSListPiece = (mSMSListPiece *)createSMSListView();
    _c(SMSListPiece)->setRect(SMSListPiece, &rc);


    _c(SMSListPiece)->reloadData(SMSListPiece);

    m_navItems[SMS_LIST_ITEM] = ncsCreateNavigationItem(
            (mHotPiece *)SMSListPiece, "SMS", NAVIGATION_STYLE_NORMAL);

    assert(m_navItems[SMS_LIST_ITEM]);
    ADDREF(m_navItems[SMS_LIST_ITEM]);

    m_smsListPiece = (mHotPiece *)SMSListPiece;
}

void SMSActivity::createSMSDetailWin(char *name)
{
    RECT rc = {0, 0, SMS_ITEM_W,SMS_TABLE_H};
    mSmsDetailView *subWinPiece = NULL;

    assert(name);
    if (m_navItems[SMS_DETAIL_ITEM] != NULL)
    {
        _c(m_navItems[SMS_DETAIL_ITEM])->setProperty(m_navItems[SMS_DETAIL_ITEM],
                NCSP_NAVIGATIONITEM_TITLE, (DWORD)name);
        return;
    }

    subWinPiece = (mSmsDetailView *)createSMSDetailView();
    _c(subWinPiece)->setRect(subWinPiece, &rc);
    
    m_navItems[SMS_DETAIL_ITEM] = ncsCreateNavigationItem(
            (mHotPiece *)subWinPiece,name, NAVIGATION_STYLE_NORMAL);

    ADDREF(m_navItems[SMS_DETAIL_ITEM]);
#if 0
    _c(m_navItems[SMS_DETAIL_ITEM])->setProperty(m_navItems[SMS_DETAIL_ITEM], 
            NCSP_NAVIGATIONITEM_BACKGROUND, (DWORD)m_barBack);
#endif
}

void SMSActivity::listEnterDetailWin(const mIndexPath* indexpath)
{
    int row;
    const char *temp;
    std::string str_temp;
    static char contact_name[SMS_NAME_MAX_LEN] = {0};

    mSmsDetailView *detailViewPiece = NULL;

    assert(indexpath);
    row = indexpath->row;

    assert(row < (signed)m_keys.size());
    memset(m_contactName,0x00,SMS_NAME_MAX_LEN);
    temp = m_keys.at(row).c_str();
    
    strncpy(m_contactName,temp,SMS_NAME_MAX_LEN - 1);
    memset(contact_name,0x00,SMS_NAME_MAX_LEN);

    str_temp = GET_CONTACTSERVICE()->getNameByDialNum(m_contactName);
    if(str_temp.empty())
    {
        strncpy(contact_name,m_contactName,SMS_NAME_MAX_LEN - 1);
    }
    else
    {
        strncpy(contact_name,str_temp.c_str(),SMS_NAME_MAX_LEN - 1);
    }

    createSMSDetailWin(contact_name);
    assert(m_smsDetailPiece);

    detailViewPiece = (mSmsDetailView *)m_smsDetailPiece;

    _c(detailViewPiece)->moveViewport(detailViewPiece, 0, 0);
    _c(detailViewPiece)->reloadData(detailViewPiece);
    _c(m_nav)->push(m_nav,m_navItems[SMS_DETAIL_ITEM]);
}

enum timeDisplayType{
    CURRENT_TIME,
    BACK_WEEK_TIME,
    FAR_AWAY_TIME,
};
static enum timeDisplayType getTimeType(time_t t) 
{
    time_t temp;
    int cur_h,cur_m,cur_s;
    cur_h = TIMESERVICE->get(TimeService::HOUR);
    cur_m = TIMESERVICE->get(TimeService::MINUTE);
    cur_s = TIMESERVICE->get(TimeService::SECOND);

    temp = TIMESERVICE->get(TimeService::SEC_FROM_EPOCH) - \
           (cur_h * 60 * 60 + cur_m * 60 + cur_s); 

    if (t >= temp )
        return CURRENT_TIME;
    else if (t < temp && t > temp - (6*24*60*60))
        return BACK_WEEK_TIME;

    return FAR_AWAY_TIME;
}

static void smsFormatTime(char *time,int len,time_t t)
{
    TimeSnapshot timesnap(t);
    enum timeDisplayType type = getTimeType(t);

    if (type == CURRENT_TIME)
    {
        timesnap.format(time,SMS_NAME_MAX_LEN,"%H:%M %p"); 
    }
    else if (type == BACK_WEEK_TIME)
    {
        timesnap.format(time,SMS_NAME_MAX_LEN,"%A"); 
    }
    else
    {
        timesnap.format(time,SMS_NAME_MAX_LEN,"%Y-%m-%d"); 
    }
}

void SMSActivity::smsGetListData(char *name,char *detail,char *time,int row)
{
    time_t temp_time = 0;
    struct SmsContent *content;
    std::string str_temp;

    assert(row < (signed)m_keys.size());
    assert(name);
    assert(detail);
    assert(time);

    {
        int index = 0,index_temp = 0;
        std::vector<SmsContent*>::iterator i;
        for (i=m_session.find(m_keys.at(row))->second.begin(); 
                i!=m_session.find(m_keys.at(row))->second.end(); ++i,index++) 
        {
            if ((*i)->m_smsTime.get(TimeService::SEC_FROM_EPOCH) > temp_time)
            {
                temp_time = (*i)->m_smsTime.get(TimeService::SEC_FROM_EPOCH);
                index_temp = index;
            }
        }
        content = m_session.find(m_keys.at(row))->second[index_temp];
    }
    str_temp = GET_CONTACTSERVICE()->getNameByDialNum(m_keys.at(row));
    if(str_temp.empty())
    {
        strncpy(name,m_keys.at(row).c_str(),SMS_NAME_MAX_LEN - 1);
    }
    else
    {
        strncpy(name,str_temp.c_str(),SMS_NAME_MAX_LEN - 1);
    }
    strncpy(detail,content->m_smsBody.c_str(),SMS_SIMPLE_MESSAGE_MAX_LEN - 8);
    if (strlen(content->m_smsBody.c_str()) > (SMS_SIMPLE_MESSAGE_MAX_LEN - 8))
    {
        strcat(detail," . . .");
    }

    smsFormatTime(time,SMS_NAME_MAX_LEN,temp_time);
}

void SMSActivity::initResource()
{
    m_btnFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            SMS_BTN_TEXT_H, 0);
    return;
}

void SMSActivity::releaseResource()
{
    DestroyLogFont(m_btnFont);
    return;
}

static BOOL main_onCreate(mWidget* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr = (mContainerCtrl *)ncsGetChildObj(self->hwnd, SMS_IDC_CTNR);
    SMSActivity* act = 
        (SMSActivity*)Activity::getActivityFromHWND (self->hwnd);

    act->self = self;

    act->createSMSListWin();
    act->createNavigatorBtns(act->getNavItem(SMS_LIST_ITEM));
    act->createCtnrBody(ctnr,act->getNavItem(SMS_LIST_ITEM));
    return TRUE;
}


static BOOL main_onClose(mWidget* self, int message)
{
    return TRUE;
}


static void main_onPaint (mWidget* _this, HDC hdc, const PCLIPRGN clipRgn)
{
    return;
}

static BOOL main_onDestroy(mWidget* self, int message)
{
    SMSActivity* activity= 
        (SMSActivity*)Activity::getActivityFromHWND (self->hwnd);

    assert (activity);

    if (activity->getNavItem(SMS_LIST_ITEM) != NULL)
    {
        UNREF(activity->getNavItem(SMS_LIST_ITEM));
    }

    if (activity->getNavItem(SMS_DETAIL_ITEM) != NULL)
    {
        UNREF(activity->getNavItem(SMS_DETAIL_ITEM));
    }
    
    ncsDestroyNavigationItem(activity->getNavItem(SMS_LIST_ITEM));
    ncsDestroyNavigationItem(activity->getNavItem(SMS_DETAIL_ITEM));
    return TRUE;
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

static NCS_WND_TEMPLATE _ctrl_templ[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        SMS_IDC_CTNR,
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
    "SMS",
    NULL,
    NULL,
    main_handlers,
    _ctrl_templ,
    TABLESIZE(_ctrl_templ),
    0,
    0, 0,
};

SMSActivity::SMSActivity() :
    NCSActivity( &mymain_templ)
{ 
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_INIT_CLASS(mLayoutManager);
    MGNCS_INIT_CLASS(mCenterHBox);
    MGNCS_INIT_CLASS(mButtonPanelPiece);
    MGNCS_INIT_CLASS(mTextPiece);
    MGNCS_INIT_CLASS(mNavigationPanelPiece);
    MGNCS_INIT_CLASS(mSMSListPiece);
    MGNCS_INIT_CLASS(mSmsDetailView);
    MGNCS_INIT_CLASS(mActivityToolbarPiece);

    m_smsListPiece = NULL;
    m_navItems[SMS_DETAIL_ITEM] = NULL;
    m_navItems[SMS_LIST_ITEM] = NULL;
    m_nav = NULL;
    memset(m_contactName,0x00,SMS_NAME_MAX_LEN);
    SMSInitData(m_session,m_keys);
    initResource();

    m_phoneCallIntent = Intent::newIntent();
}

SMSActivity::~SMSActivity(void) 
{
    releaseResource();
    Intent::deleteIntent(m_phoneCallIntent);

//    SMSDeinitData(m_session);
}

