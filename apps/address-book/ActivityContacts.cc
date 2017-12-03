#include <string.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include "mcontainerctrl.h"
#include "mpanelpiece.h"
#include "mtableviewpiece.h"
#include "mtableviewitempiece.h"
#include "mbuttonpanelpiece.h"

#include "museritem.h"
#include "ContactsLoadHelper.hh"

#include "mspeedmeter.h"

#include "Activity.hh"
#include "ActivityContacts.hh"
#include "ContactsTableViewPiece.hh"
#include "DetailTableViewPiece.hh"

#include "../common/mactivitytoolbarpiece.h"

REGISTER_ACTIVITY(PhoneContactsActivity);


enum _ACTIVE_INDEX{
    TOOLBAR_IMAGE_CALLLOG,
    TOOLBAR_IMAGE_DIAL,
    TOOLBAR_IMAGE_CONTACTS,
    TOOLBAR_IMAGE_MAX,
};

const static char *PICTURES[TOOLBAR_IMAGE_MAX] = {
    "res/address-book/address_book_calllog.png",
    "res/address-book/address_book_dial.png",
    "res/address-book/address_book_contacts.png",
};

static ARGB title_gradient_color[] = {0xffd0d571, 0xffb5c363, 
        0xff9eb23a,0xff7f9e2c,0xff597311,0xff597311};
static float title_gradient_pos[] = {0.0, 0.49,0.50,0.96,0.97,1.0};

static ARGB  detail_colors [] = {0xfffffff, 0xffe0e0e0, 0xfff7f7f7, 0xfff7f7f7};
static float detail_colors_positions [] = {0.00f, 0.01f, 0.39f, 1.00f};

static void setting_gradient_color(mShapeTransRoundPiece *piece, ARGB *colors, float *pos, int num);

static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData );
static BOOL main_onClose(mWidget* self, int message);
static BOOL main_onDestroy(mWidget* self, int message);

static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        IDC_NAV,
        0, 0, ACTIVITY_W, ACTIVITY_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL,
        NULL,
        0, 0,
    }
};

static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}
static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE,      reinterpret_cast<void*>(main_onCreate)},
    {MSG_CLOSE,       reinterpret_cast<void*>(main_onClose)},
    {MSG_DESTROY,     reinterpret_cast<void*>(main_onDestroy)},
    {MSG_LBUTTONDOWN, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_LBUTTONUP,   reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_MOUSEMOVE,   reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {0, NULL}
};

static NCS_MNWND_TEMPLATE dialog_tmpl = {
    NCSCTRL_DIALOGBOX,
    1,
    0, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H, /* full screen */
    WS_NONE,
    WS_EX_NONE, "PhoneContactsActivity",
    NULL,
    NULL,
    main_handlers,
    _ctrl_tmpl,
    sizeof(_ctrl_tmpl)/sizeof(NCS_WND_TEMPLATE),
    0,
    0, 0,
};

const char *GROUPNAMES[GROUPSIZE+1] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
};

static BOOL to_call_log (mObject *self, mObject *piece, int event_id,  DWORD param);
static BOOL to_dail (mObject *self, mObject *piece, int event_id,  DWORD param);

PhoneContactsActivity::PhoneContactsActivity() :
    NCSActivity( &dialog_tmpl)
{
    MGNCS_INIT_CLASS(mContactsTableViewPiece);
    MGNCS_INIT_CLASS(mDetailTableViewPiece);
    MGNCS_INIT_CLASS(mActivityToolbarPiece);

    m_intent = NULL;
    m_nav = NULL;
    memset(m_viewPiece, 0, sizeof(m_viewPiece));
    memset(m_navItems, 0, sizeof(m_navItems));
    m_detailNameItem = NULL;
    m_detailTableViewPiece = NULL;

    m_font = NULL;
}

PhoneContactsActivity::~PhoneContactsActivity() 
{
    int i;
    unloadResource ();
    unloadContactsPhoto();
    unloadContactsGroup();

    Intent::deleteIntent(m_intent);
    DestroyLogFont (m_font);
    m_font = NULL;

    for (i=0; i<NAVITEM_MAX; ++i) {
        UNREFPIECE(m_viewPiece[i]);
        UNREF(m_navItems[i]);
        DestroyNavigationItem(m_navItems[i]);
    }
}

int PhoneContactsActivity::initContacts ()
{
    loadResource();
    loadContactGroupFromLib ();
    loadContactsPhoto();

    m_intent = Intent::newIntent();

    m_font = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            DETAIL_FONT_SIZE, 0);

    return 0;
}

int PhoneContactsActivity::uninitContacts ()
{
    return 0;
}

int PhoneContactsActivity::destroyNavigation (HWND ctrl_hwnd)
{
    return 0;
}

void PhoneContactsActivity::updateDetailNavItem (ContactsInfo *info)
{
    const char *icon_path;
    const char *name;
    BITMAP *bmp;
    ARGB font_color = 0xff000000;

    assert (info);

    icon_path = info->photo_path.c_str();
    name = info->name.c_str();

    bmp = (BITMAP *)GetResource (Str2Key (icon_path));
    if (!bmp) {
        bmp = (BITMAP*)GetResource (Str2Key(CONTACT_DEFAULT_PHOTO));
    }

    assert (bmp && m_detailNameItem && m_detailTableViewPiece);

    /* create icon add name */
    _c(m_detailNameItem)->setProperty (m_detailNameItem, NCSP_ITEMPIECE_MARGIN, 5);
    _c(m_detailNameItem)->setProperty (m_detailNameItem, NCSP_ITEMPIECE_CLEARANCE, 12);
    _c(m_detailNameItem)->setLayout (m_detailNameItem, NCS_ALIGN_LEFT);
    _c(m_detailNameItem)->setImage (m_detailNameItem, bmp, DETAIL_PHOTO_W, DETAIL_PHOTO_H);
    /*text*/
    if (m_detailNameItem->titlePiece == NULL)
    {
        m_detailNameItem->titlePiece = NEWPIECE (mTextPiece);
        _c(m_detailNameItem->titlePiece)->setProperty (
                m_detailNameItem->titlePiece, NCSP_TEXTPIECE_MAXLEN, ACTIVITY_W*2/3);

        _c(m_detailNameItem)->addContent (m_detailNameItem, 
                (mHotPiece *)m_detailNameItem->titlePiece, 0, 0);
        m_detailNameItem->childAlign[m_detailNameItem->childrenTotal++] = m_detailNameItem->align;
    }
    _c(m_detailNameItem)->setTitle (m_detailNameItem, (char*)name, m_font, &font_color);
    _c(m_detailNameItem)->activeLayout (m_detailNameItem);

    _c(m_detailTableViewPiece)->setContactsData (m_detailTableViewPiece, info);
    _c(m_detailTableViewPiece)->reloadData (m_detailTableViewPiece);
}

void PhoneContactsActivity::loadContactGroupFromLib ()
{
    int i, ret = 0;
    ContactsSection *section = NULL;

    for (i = 0; i < GROUPSIZE; ++i) {
        if (NULL == section) {
            section = new ContactsSection;
        }
        assert(NULL != section);
        section->titlename = GROUPNAMES[i];
        ret = LoadGroupContacts (section->group, section->titlename);

        if (ret == 0) {
            m_contactsData.push_back(section);
            section = NULL;
        } 
    }

    // forbidden no such item at last.
    if (section){
        delete section;
    }
}

void PhoneContactsActivity::unloadContactsGroup()
{
    ContactsSection *section = NULL;

    while (!m_contactsData.empty()) {
        section = m_contactsData.back();

        while (!section->group.empty()) {
            delete section->group.back();
            section->group.pop_back();
        }

        delete section;
        m_contactsData.pop_back();
    }
}

void PhoneContactsActivity::loadContactsPhoto ()
{
    unsigned int i,j;
    ContactsSection *section = NULL;
    ContactsInfo *info = NULL;

    Load32Resource (CONTACT_DEFAULT_PHOTO, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);

    for (i=0; i<m_contactsData.size(); ++i) {
        section = m_contactsData[i];
        assert(NULL != section);

        for (j=0; j<section->group.size(); ++j) {
            info = section->group[j];
            assert(NULL != info);

            Load32Resource (info->icon_path.c_str(), RES_TYPE_IMAGE, (DWORD) HDC_SCREEN);
            Load32Resource (info->photo_path.c_str(), RES_TYPE_IMAGE, (DWORD) HDC_SCREEN);
        }
    }
}

void PhoneContactsActivity::unloadContactsPhoto()
{
    unsigned int i,j;
    ContactsSection *section;
    ContactsInfo *info;

    ReleaseRes (Str2Key(CONTACT_DEFAULT_PHOTO));

    for (i=0; i<m_contactsData.size(); ++i) {
        section = m_contactsData[i];

        for (j=0; j<section->group.size(); ++j) {
            info = section->group[j];

            ReleaseRes (Str2Key (info->icon_path.c_str()));
            ReleaseRes (Str2Key (info->photo_path.c_str()));
        }
    }
}

int PhoneContactsActivity::createNavigation (HWND ctrl_hwnd)
{
    RECT rc;
    mContainerCtrl* ctrl = NULL;
    mShapeTransRoundPiece *navbar_bk = NULL;
    HBRUSH brush;

    /* create nav item */

    m_viewPiece[NAVITEM_CONTACT] = createContactView();
    m_viewPiece[NAVITEM_DETAIL] = createDetailView();
    ADDREFPIECE(m_viewPiece[NAVITEM_CONTACT]);
    ADDREFPIECE(m_viewPiece[NAVITEM_DETAIL]);

    m_navItems[NAVITEM_CONTACT] = CreateNavigationItem (
            (mHotPiece*)m_viewPiece[NAVITEM_CONTACT], "All Contacts", 0);
    m_navItems[NAVITEM_DETAIL] = CreateNavigationItem(
            (mHotPiece*)m_viewPiece[NAVITEM_DETAIL], "Brief Intro", 0);

    ADDREF(m_navItems[NAVITEM_CONTACT]);
    ADDREF(m_navItems[NAVITEM_DETAIL]);

    m_nav = CreateNavigationPanelPieceWithRootView (m_navItems[NAVITEM_CONTACT]);
    SetRect(&rc, 0, 0, ACTIVITY_W, ACTIVITY_H);
    _c(m_nav)->setRect(m_nav, &rc);

    /* navigation back ground (mShapeTransRoundPiece)*/
    navbar_bk = NEWPIECE(mShapeTransRoundPiece);
    SetRect(&rc, 0, 0, NAVIGATIONBAR_W, NAVIGATIONBAR_H);
    _c(navbar_bk)->setRect(navbar_bk, &rc);
    _c(navbar_bk)->setProperty (navbar_bk, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    _c(navbar_bk)->setProperty (navbar_bk, NCSP_TRANROUND_CORNERFLAG,0);

    brush = _c(navbar_bk)->getBrush(navbar_bk);
    MGPlusSetLinearGradientBrushColorsEx (brush, 
            title_gradient_color, TABLESIZE(title_gradient_color), title_gradient_pos);
    _c(m_nav)->setProperty(m_nav, NCSP_NAVIGATIONPANELPIECE_BAR_BKG, (DWORD)navbar_bk);

    ctrl = (mContainerCtrl*)ncsObjFromHandle(ctrl_hwnd);
    _c(ctrl)->setBody(ctrl, (mHotPiece*)m_nav);

    return 0;
}

int PhoneContactsActivity::onBack()
{
    if (_c(m_nav)->currentIsRoot(m_nav))
        return 0;

    _c(m_nav)->pop(m_nav);
    return 1;
}

mActivityToolbarPiece *PhoneContactsActivity::createToolbar ()
{
    RECT rc = {0, 0, ACTIVITY_W, TOOLBAR_H};
    BITMAP *bmp;
    mActivityToolbarPiece *toolbar;

    toolbar = NEWPIECEEX (mActivityToolbarPiece, 0x7|TOOLBAR_BUTTON_RIGHT_HI);

    bmp = (BITMAP*)GetResource (Str2Key (PICTURES[TOOLBAR_IMAGE_CALLLOG]));
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_LEFT, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_LEFT, to_call_log);

    bmp = (BITMAP*)GetResource (Str2Key (PICTURES[TOOLBAR_IMAGE_DIAL]));
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_CENTER, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_CENTER, to_dail);

    bmp = (BITMAP*)GetResource (Str2Key (PICTURES[TOOLBAR_IMAGE_CONTACTS]));
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_RIGHT, bmp);

    _c(toolbar)->setRect (toolbar, &rc);

    return toolbar;
}
#if 0
BOOL onTimer(HWND _view, int id, DWORD nouse) {
    for (;;){
        RECT rc;
        mContactsTableViewPiece *view = (mContactsTableViewPiece *)_view;
        _c(view)->getViewport(view, &rc);
        if (rc.top >= 960 + 50 || rc.top < 50) {
            rc.top = 50;
        }else{
            rc.top += 5;
        }
        _c(view)->moveViewport(view, rc.left, rc.top);
#if 0
        PanelPiece_update((mHotPiece *)view, FALSE);
#else
        break;
#endif
    }
    return TRUE;
}
#endif

mPanelPiece *PhoneContactsActivity::createContactView()
{
    RECT rc = {0, 0, ACTIVITY_W, VIEW_H};
    mActivityToolbarPiece *toolbar = NULL;
    mContactsTableViewPiece *contact_tableview = NULL;

    mPanelPiece *panel = NEWPIECE (mPanelPiece);
    _c(panel)->setRect (panel, &rc);

    contact_tableview = NEWPIECEEX (mContactsTableViewPiece, (DWORD)this);
    SetRect (&rc, 0, 0, ACTIVITY_W, VIEW_H-TOOLBAR_H);
    _c(contact_tableview)->setRect (contact_tableview, &rc);

    _c(contact_tableview)->reloadData (contact_tableview);

    toolbar = createToolbar ();

    _c(panel)->addContent (panel, (mHotPiece*)contact_tableview, 0, 0);
    _c(panel)->addContent (panel, (mHotPiece*)toolbar, 0, VIEW_H-TOOLBAR_H);

    // SetTimerEx((HWND)contact_tableview, 1234, 1, onTimer);

    return panel;
}

mPanelPiece *PhoneContactsActivity::createDetailView()
{
    RECT rc;
    mShapeTransRoundPiece *bk = NULL;
    mActivityToolbarPiece *toolbar = NULL;

    mPanelPiece *panel = NEWPIECE (mPanelPiece);

    SetRect (&rc, 0, 0, ACTIVITY_W, VIEW_H);
    _c(panel)->setRect (panel, &rc);

    bk = NEWPIECE (mShapeTransRoundPiece);
    _c(bk)->setRect (bk, &rc);
    _c(bk)->setProperty (bk, NCSP_TRANROUND_CORNERFLAG, 0);
    setting_gradient_color (bk, detail_colors, detail_colors_positions, TABLESIZE (detail_colors)); 

    m_detailTableViewPiece = NEWPIECEEX (mDetailTableViewPiece,(DWORD)this);

    SetRect (&rc, 0, 0, ACTIVITY_W, VIEW_H-TOOLBAR_H);
    _c(m_detailTableViewPiece)->setRect (m_detailTableViewPiece, &rc);
    _c(m_detailTableViewPiece)->setContactsData (m_detailTableViewPiece, NULL);
    _c(m_detailTableViewPiece)->reloadData (m_detailTableViewPiece);

    toolbar = createToolbar ();
    /*bug:s3c6410*/
    _c(panel)->addContent (panel, (mHotPiece*)bk, 0, -6);
    _c(panel)->addContent (panel, (mHotPiece*)m_detailTableViewPiece, 0, DETAIL_PHONENUM_Y);
    _c(panel)->addContent (panel, (mHotPiece*)toolbar, 0, VIEW_H-TOOLBAR_H);

    /* create item piece */
    m_detailNameItem = NEWPIECE (mItemPiece);
    SetRect (&rc, 0, 0, ACTIVITY_W, DETAIL_PHOTO_H);
    _c(m_detailNameItem)->setRect (m_detailNameItem, &rc);

    _c(panel)->addContent (panel, (mHotPiece*)m_detailNameItem, 0, DETAIL_PHOTO_Y);

    return panel; 
}

void PhoneContactsActivity::loadResource()
{
    int i;
    for (i=0; i<TOOLBAR_IMAGE_MAX; ++i) {
        Load32Resource (PICTURES[i], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    }
}

void PhoneContactsActivity::unloadResource()
{
    int i;

    for (i=0; i<TOOLBAR_IMAGE_MAX; ++i) {
        ReleaseRes (Str2Key(PICTURES[i]));
    }
}


static BOOL to_call_log (mObject *self, mObject *piece, int event_id,  DWORD param)
{
    ACTIVITYSTACK->switchTo("CallHistoryActivity", NULL);
    return TRUE;
}

static BOOL to_dail (mObject *self, mObject *piece, int event_id,  DWORD param)
{
    ACTIVITYSTACK->switchTo("DialPadActivity", NULL);
    return TRUE;
}

static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mWidget *ctrl_widget;
    PhoneContactsActivity* activity = (PhoneContactsActivity*)Activity::getActivityFromHWND (self->hwnd);

    activity->initContacts ();

    ctrl_widget =ncsGetChildObj (self->hwnd, IDC_NAV);
    activity->createNavigation (ctrl_widget->hwnd);

    return TRUE;
}

static BOOL main_onClose(mWidget* self, int message)
{
    return TRUE;
}

static BOOL main_onDestroy(mWidget* self, int message)
{
    return TRUE;
}

static void setting_gradient_color(mShapeTransRoundPiece *piece, ARGB *colors, float *pos, int num)
{
    HBRUSH brush;
    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    brush = _c(piece)->getBrush(piece);
    MGPlusSetLinearGradientBrushColorsEx(brush,colors,num,pos);
}

