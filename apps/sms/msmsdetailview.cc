#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "register.h"

#include "SMSActivity.hh"
#include <SmsService.hh>
#include <ContactService.hh>
#include <SettingService.hh>

#include <vector>
#include <set>

#define SMS_DETAIL_DEBUG	0

#define TABLE_VIEW_W	ACTIVITY_W
#define TABLE_VIEW_H	400

#define TOOLBAR_X	0
#define TOOLBAR_Y	430

#define ICON_PATH_LEN       256
#define DEFAULT_ICON_PATH	"res/address-book/address_book_unknow.png"

typedef struct _RowInfo {
    const char* text;
    const char* picture;
    int isme;
    const char* time;
} RowInfo;

typedef struct _SectionInfo {
    char* title;
    int num;
    RowInfo* rows;
} SectionInfo;


static std::vector<SmsService::SmsContentArray> sms_table;
static std::vector<std::string> title_list;
static std::vector< std::vector<std::string> > sms_time_table;

static RowInfo getRowInfo(int section, int row);
static int numberOfSections();
static int numberOfRowsInSection(int section);
static const char* titleForSection(int section);


static void mSmsDetailView_construct(mSmsDetailView *self, DWORD add_data)
{
    Class(mTableViewPiece).construct((mTableViewPiece*)self, add_data);

    self->act = (void *)add_data;
    assert(self->act);
    self->name = ((SMSActivity *)(self->act))->m_contactName; 

    self->my_icon = NULL;
    self->sender_icon = NULL;
    self->my_icon_path = (char *)malloc(ICON_PATH_LEN);
    assert(self->my_icon_path);
    self->sender_icon_path = (char *)malloc(ICON_PATH_LEN);
    assert(self->sender_icon_path);
}

static void loadIcons(mSmsDetailView* self)
{
    std::string icon_path = GET_SETTINGSERVICE()->getCurrent(SettingService::PHOTO);
    if (icon_path.empty()) {
        icon_path = DEFAULT_ICON_PATH;
    }

    self->my_icon = (PBITMAP)Load32Resource(icon_path.c_str(), RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);

    memset(self->my_icon_path,0x00,ICON_PATH_LEN);
    strncpy(self->my_icon_path,icon_path.c_str(),ICON_PATH_LEN - 1);

    icon_path = GET_CONTACTSERVICE()->getPhotoBySmsNum(self->name);

    if (icon_path.empty()) {
        icon_path = DEFAULT_ICON_PATH;
    }

    self->sender_icon = (PBITMAP)Load32Resource(icon_path.c_str(), RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);

    memset(self->sender_icon_path,0x00,ICON_PATH_LEN);
    strncpy(self->sender_icon_path,icon_path.c_str(),ICON_PATH_LEN - 1);

    assert(self->my_icon && self->sender_icon);
}

static void releaseIcons(mSmsDetailView* self)
{
    if (self->my_icon) {
        RES_KEY key = Str2Key(self->my_icon_path);
        ReleaseRes(key);
        
        self->my_icon = NULL;
    }

    if (self->sender_icon) {
        RES_KEY key = Str2Key(self->sender_icon_path);
        ReleaseRes(key);
        
        self->sender_icon = NULL;
    }
}

static void mSmsDetailView_destroy(mSmsDetailView *self)
{
    releaseIcons(self);
    
    if (self->my_icon_path != NULL)
        free(self->my_icon_path);
    if (self->sender_icon_path != NULL)
        free(self->sender_icon_path);
    Class(mTableViewPiece).destroy((mTableViewPiece*)self);
}

static mTableViewItemPiece* mSmsDetailView_createItemForRow(mSmsDetailView* self, const mIndexPath* indexpath)
{
    RECT rect;
    RECT view_rc;
    RowInfo rowinfo = getRowInfo(indexpath->section, indexpath->row);
    PBITMAP icon = NULL;

    if (rowinfo.isme)
        icon = self->my_icon;
    else
        icon = self->sender_icon;

    enumNCSAlign icon_align = rowinfo.isme ? NCS_ALIGN_RIGHT : NCS_ALIGN_LEFT;
    mHotPiece* piece = (mHotPiece*)CreateSMSContentItem(icon, rowinfo.text, rowinfo.time, icon_align);
    mTableViewItemPiece* item = NEWPIECEEX(mTableViewItemPiece, 0);

    //mShapeTransRoundPiece* bkg = _c(item)->getBkgndPiece(item);
    //_c(bkg)->setProperty (bkg, NCSP_TRANROUND_BKCOLOR, 0);

    _c(self)->getRect(self, &view_rc);
    _c(piece)->getRect(piece, &rect);
    _c(item)->setUserPiece(item, piece);
    rect.right = RECTW(view_rc);
    _c(item)->setRect(item, &rect);

    return item;
}

static int mSmsDetailView_numberOfSections(mSmsDetailView* self)
{
    return numberOfSections();
}

static int mSmsDetailView_numberOfRowsInSection(mSmsDetailView* self, int section)
{
    return numberOfRowsInSection(section);
}


static const char* mSmsDetailView_titleForSection(mSmsDetailView* self, int section)
{
    return titleForSection(section);
}

/* static const char* mSmsDetailView_indexForSection(mSmsDetailView* self, int section) */
/* { */
/*     return section_list[section].index; */
/* } */

static void mSmsDetailView_onCommitDeleteRowAtIndexPath(mSmsDetailView* self, const mIndexPath* indexpath)
{
    /* edit datasource. */
    /* ... */

    /* delete animation. */
    _c(self)->deleteRowAtIndexPath(self, indexpath, NCS_TABLEVIEW_ANIMATIONZOOM);
}

static mPanelPiece* mSmsDetailView_createIndexSectionHead(mSmsDetailView* self, int section)
{
    RECT rc;
    const char* string = _c(self)->titleForSection(self, section);
    if (strlen(string)) {
        mTextPiece* text = NEWPIECEEX(mTextPiece, (DWORD)string);
        mSeparatorPiece* separator = NEWPIECE(mSeparatorPiece);
        mPanelPiece* panel = NEWPIECE(mPanelPiece);
        mRectPiece* rect = NEWPIECE(mRectPiece);

        _c(self)->getDefaultRowRect(self, &rc);
        rc.bottom += 8;
        
        _c(panel)->setRect(panel, &rc);
        _c(rect)->setRect(rect, &rc);
        rc.bottom -= 5;
        _c(text)->setRect(text, &rc);
        rc.bottom = 5;
        rc.right -= 10;
        _c(separator)->setRect(separator, &rc);
        
        _c(text)->setProperty(text, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_CENTER);
        _c(text)->setProperty(text, NCSP_LABELPIECE_VALIGN, NCS_VALIGN_CENTER);
        _c(text)->setProperty(text, NCSP_TEXTPIECE_TEXTCOLOR, 0xFF808080);
        _c(rect)->setProperty(rect , NCSP_RECTPIECE_FILLCOLOR, 0xFFFFFFFF);
        _c(rect)->setProperty(rect , NCSP_RECTPIECE_BORDERSIZE, 0x0);

        _c(self)->getDefaultRowRect(self, &rc);
        _c(panel)->addContent(panel, (mHotPiece*)rect, 0, 0);
        _c(panel)->addContent(panel, (mHotPiece*)separator, 3, rc.bottom - 2);
        _c(panel)->addContent(panel, (mHotPiece*)text, 5, 0);

        _c(panel)->reLayout(panel);

        return panel;
    }
    else {
        return NULL;
    }
}

static BOOL mSmsDetailView_willSelectRowAtIndexPath(mSmsDetailView* self, const mIndexPath* indexpath)
{
    return FALSE;
}

class TimeInDay: public TimeSnapshot
{
public:
    TimeInDay(time_t t) : TimeSnapshot(t) { }
    
    bool operator == (const TimeInDay& time) const {
        return inSameDay(this, &time);
    }
    bool operator <  (const TimeInDay& time) const {
        return get(TimeService::YEAR)  > time.get(TimeService::YEAR)
            || get(TimeService::MONTH) > time.get(TimeService::MONTH)
            || get(TimeService::MDAY)  > time.get(TimeService::MDAY);
    }
};

static void mSmsDetailView_reloadData(mSmsDetailView* self)
{
    // update date for current row
    const SmsService::SmsSession& allSession = ((SMSActivity *)(self->act))->m_session;//GET_SMSSERVICE()->getAllSession();
    SmsService::SmsSession::const_iterator found = allSession.find(self->name);
    assert (found != allSession.end());
    const SmsService::SmsContentArray& sms_list = found->second;
    SmsService::SmsContentArray::const_iterator it;
    std::set<TimeInDay> day_set;
    std::set<TimeInDay>::const_iterator day_it;
    SmsService::SmsContentArray tmp_sms_list;
    std::vector<std::string> tmp_time_list;
    char title[64];
    char time[64];

    printf("=============================mSmsDetailView_reloadData\n");

    for (it = sms_list.begin(); it != sms_list.end(); ++it) {
        day_set.insert(TimeInDay((*it)->m_smsTime.get(TimeService::SEC_FROM_EPOCH)));
    }

    sms_table.clear();
    title_list.clear();
    sms_time_table.clear();
    for (day_it = day_set.begin(); day_it != day_set.end(); ++day_it) {
        tmp_sms_list.clear();
        tmp_time_list.clear();

        for (it = sms_list.begin(); it != sms_list.end(); ++it) {
            if (TimeSnapshot::inSameDay(&(*it)->m_smsTime, &(*day_it))) {
                tmp_sms_list.push_back(*it);
            }

            (*it)->m_smsTime.format(time, 64, "%I:%M %p");
            tmp_time_list.push_back(time);
        }

        sms_table.push_back(tmp_sms_list);
        sms_time_table.push_back(tmp_time_list);

        day_it->format(title, 64, "%Y-%m-%d");
        title_list.push_back(title);
        
        printf("======================== tmp_sms_list size: %lu %s\n", (unsigned long)tmp_sms_list.size(), title);
    }

    releaseIcons(self);
    loadIcons(self);

    Class(mTableViewPiece).reloadData((mTableViewPiece*)self);
}


BEGIN_MINI_CLASS(mSmsDetailView, mTableViewPiece)
	CLASS_METHOD_MAP(mSmsDetailView, construct)
	CLASS_METHOD_MAP(mSmsDetailView, createItemForRow)
	CLASS_METHOD_MAP(mSmsDetailView, numberOfSections)
	CLASS_METHOD_MAP(mSmsDetailView, numberOfRowsInSection)
	CLASS_METHOD_MAP(mSmsDetailView, titleForSection)
	/* CLASS_METHOD_MAP(mSmsDetailView, indexForSection) */
	CLASS_METHOD_MAP(mSmsDetailView, onCommitDeleteRowAtIndexPath)
	CLASS_METHOD_MAP(mSmsDetailView, destroy)
	CLASS_METHOD_MAP(mSmsDetailView, willSelectRowAtIndexPath)
	CLASS_METHOD_MAP(mSmsDetailView, createIndexSectionHead)
	CLASS_METHOD_MAP(mSmsDetailView, reloadData)
END_MINI_CLASS


#if 0

static int onEditStateChange(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message == MSG_BTNPANELPIECE_STATE_CHANGED) {
        enum mButtonPanelPieceState state = (enum mButtonPanelPieceState)wParam;
        if (state == NCSN_ABP_CLICKED) {
	    _c(table)->changeMode(table);
        }
    }
    return 0;
}

static int onDeleteStateChange(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message == MSG_BTNPANELPIECE_STATE_CHANGED) {

        enum mButtonPanelPieceState state = (enum mButtonPanelPieceState)wParam;
        if (state == NCS_BPP_PUSHED) {

        } else if (state == NCS_BPP_NORMAL){
            //_c(table)->removeItem(table, "key", TRUE);
        }
    }
    return 0;
}

static int onPieceClick(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    return 0;
}
#endif

#if 0
static BOOL create_delete_button(mMainWnd* self)
{
    mContainerCtrl *ctnr;
    {
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                BTN_W*2, 0, BTN_W, BTN_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, BTN_W, BTN_H};
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(body)->setRect(body, &rc);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
        SetRect(&rc, 0, 0, 50, 20);
        _c(txt_piece)->setRect(txt_piece, &rc);

        mButtonPanelPiece *btn = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);

        _c(btn)->setStateTable(btn,
                txt_piece, NCSP_LABELPIECE_LABEL, (DWORD)"index", (DWORD)"pushed");

        _c(btn)->addContentToLayout(btn, txt_piece);

        SetRect(&rc, 0, 0, BTN_W, BTN_H);
        _c(btn)->setRect(btn, &rc);

        ARGB colors0[] = {0xfffdc972, 0xfffcb848, 0xffba3305, 0xffd25e39};
        float positions0[] = {0.0, 0.03, 0.98, 1.0};
        ARGB colors1[] = {0x3fff0000, 0x7f00ff00, 0xaf003fff, 0xff0000ff};
        _c(btn)->setGradientBackgroundColor(btn,
                colors0, positions0, 4,
                colors1, NULL, 4);

        _c(btn)->setStates(btn);

        _c(btn)->appendEventHandler(btn, MSG_BTNPANELPIECE_STATE_CHANGED, onDeleteStateChange);

        _c(body)->addContent(body, (mHotPiece*)btn, 0, 0);
    }
    return TRUE;
}
#endif

/* ==================== test ========== */

#if SMS_DETAIL_DEBUG

/* static mSmsDetailView* sms_detail_table; */

static const RowInfo row_list1[] = {
    {"MiniGUI takes modern graphic user interface (GUI) technology into \
embedded devices, and makes all kinds of embedded devices which use \
popular embedded operating systems,b especially the consumer \
electronics products, with easy access to the best graphics user \
interface and user experience.", "res/address-book/address_book_photo.png", 0},
    {"Excellent! What about mDolphin?", "res/address-book/address_book_unknow.png", 1},
    {"mDolphin is a high end embedded browser that was developed based on \
the open-source browser core WebKit, and uses MiniGUI as its \
underlying graphics support system.  In addition to support for \
popular W3C standards, mDolphin also provides numerous extensions in \
order to adapt all kinds of embedded devices for the application \
requirements.", "res/address-book/address_book_photo.png", 0},
};

static const RowInfo row_list2[] = {
    {"Hello", "res/address-book/address_book_photo.png", 0},
    {"Hi ...", "res/address-book/address_book_unknow.png", 1},
};

SectionInfo section_list[] = {
    {"2010-09-13", TABLESIZE(row_list1), (RowInfo*)row_list1},
    {"2010-10-01", TABLESIZE(row_list2), (RowInfo*)row_list2},
    {"2010-10-02", TABLESIZE(row_list2), (RowInfo*)row_list2},
    {"2010-10-03", TABLESIZE(row_list2), (RowInfo*)row_list2},
    {"2010-10-04", TABLESIZE(row_list2), (RowInfo*)row_list2},
    {"2010-10-05", TABLESIZE(row_list2), (RowInfo*)row_list2},
    {"2010-10-06", TABLESIZE(row_list2), (RowInfo*)row_list2},
    {"2010-10-07", TABLESIZE(row_list2), (RowInfo*)row_list2},
    {"2010-10-08", TABLESIZE(row_list2), (RowInfo*)row_list2},
};

static RowInfo getRowInfo(int section, int row)
{
    return *(section_list[section].rows + row);
}

static int numberOfSections()
{
    return TABLESIZE(section_list);
}

static int numberOfRowsInSection(int section)
{
    return section_list[section].num;
}

static const char* titleForSection(int section)
{
    return section_list[section].title;
}

#else

static RowInfo getRowInfo(int section, int row)
{
    RowInfo rowInfo;
    const SmsContent* sms = sms_table[section][row];
    
    rowInfo.text = sms->m_smsBody.c_str();
    rowInfo.isme = sms->m_smsType == SMS_SENT;
    rowInfo.time = sms_time_table[section][row].c_str();
    
    if (rowInfo.isme)
        rowInfo.picture = "res/address-book/address_book_unknow.png";
    else
        rowInfo.picture = "res/address-book/address_book_photo.png";

    return rowInfo;
}

static int numberOfSections()
{
    return sms_table.size();
}

static int numberOfRowsInSection(int section)
{
    return sms_table[section].size();
}

static const char* titleForSection(int section)
{
    return title_list[section].c_str();
}
#endif  /* SMS_DETAIL_DEBUG */

#if 0

static void create_toolbar(mWidget* self)
{
}

static BOOL mymain_onCreate(mWidget* self, DWORD dwAddData )
{

    RECT rc = {0, 0, TABLE_VIEW_W, TABLE_VIEW_H};
    /* table */
    mContainerCtrl* ctnr = (mContainerCtrl*)ncsCreateWindow(
        NCSCTRL_CONTAINERCTRL,
        "ContainerCtrl",
        WS_VISIBLE, 0, 0,
        0, 0, TABLE_VIEW_W, TABLE_VIEW_H, 
        self->hwnd,
        NULL, NULL, NULL, 0);

    sms_detail_table = (mSmsDetailView*)NEWPIECEEX(mSmsDetailView, NCS_TABLEVIEW_INDEXLOCATE_STYLE);
    mSmsDetailView* table = sms_detail_table;
    _c(table)->setRect(table, &rc);
    _c(table)->reloadData(table);
#if 1
    _c(ctnr)->setBody(ctnr, (mHotPiece*)table);
#else
    {
        mPanelPiece* panel = NEWPIECE(mPanelPiece);
        _c(panel)->addContent(panel, (mHotPiece*)table, 0, 0);
        _c(panel)->setRect(panel, &rc);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)panel);
    }
#endif

    create_toolbar(self);
    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {MSG_LBUTTONDOWN, SpeedMeterMessageHandler },
    {MSG_LBUTTONUP, SpeedMeterMessageHandler },
    {MSG_MOUSEMOVE, SpeedMeterMessageHandler },
    {0, NULL}
};

int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mSmsDetailViewPiece" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();
    MGNCS_INIT_CLASS(mSmsDetailView);
    MGNCS_INIT_CLASS(mIndexLocatePiece);

    mymain = ncsCreateMainWindow(
        NCSCTRL_MAINWND, "mSmsDetailViewPiece test",
        WS_CAPTION | WS_BORDER | WS_VISIBLE,
        WS_EX_AUTOSECONDARYDC,
        1,
        0, 0, 360, 480,
        HWND_DESKTOP,
        0, 0,
        NULL,
        NULL,
        mymain_handlers,
        0);

    mGEffInit();

    while(GetMessage(&Msg, mymain->hwnd)) {   
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(mymain->hwnd);

    mGEffDeinit();
    ncsUninitialize();

    return 0;
}

#endif
