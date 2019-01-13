#include <string.h>
#include <cassert>
#include <sstream>

#include "global.h"

#include "ActivityMusicPlayer.hh"
#include "ActivityAlbum.hh"

REGISTER_ACTIVITY(ActivityMusicPlayer);

#define IDC_START           200
#define IDC_IMAGE           (IDC_START)
#define IDC_TIMER           (IDC_START+1)
#define IDC_CTRL_BK         (IDC_START+2)
#define IDC_CTRL_TRACKBAR   (IDC_START+3)
#define IDC_CTRL_CURTIME    (IDC_START+4)
#define IDC_CTRL_TOTALTIME  (IDC_START+5)
#define IDC_CTRL_INFO       (IDC_START+6)
#define IDC_CTRL_CONT       (IDC_START+7)
#define IDC_CTRL_FIRSTBTN   (IDC_START+8)
#define IDC_CTRL_LASTBTN    (IDC_START+11)
#define IDC_CTRL_SEPARATOR  (IDC_START+12)
#if 0
#define IDC_BTN_NEXT        (IDC_START+13)
#define IDC_BTN_PREV        (IDC_START+14)
#define IDC_BTN_START       (IDC_START+15)
#define IDC_BTN_PAUSE       (IDC_START+16)
#endif

#define APP_NAME_MUSICPLAYER "MusicPlayer"

#define MUSIC_IMG_PATH "res/musicplayer/player/CD.png"

#define NAVBAR_X    0
#define NAVBAR_Y    0
#define NAVBAR_W    ACTIVITY_W
//#define NAVBAR_H    (60-NAVBAR_Y)
#define NAVBAR_H    38

#define NAVBAR_BTN_W    70
#define NAVBAR_BTN_H    30
#define NAVBAR_BTN_X    10
#define NAVBAR_BTN_Y    ((NAVBAR_H - NAVBAR_BTN_H)/2)

#define NAVBAR_TITLE_X    0
#define NAVBAR_TITLE_Y    0
#define NAVBAR_TITLE_W    NAVBAR_W
#define NAVBAR_TITLE_H    NAVBAR_H

typedef struct _GradientData {
    float pos;
    ARGB color;
} GradientData;

static const GradientData mp_navbar_gradient[] = 
{ 
	{ 0.00f, 0xffd0d571, },
	{ 0.49f, 0xffb5c363, },
	{ 0.50f, 0xff9eb23a, },
	{ 0.96f, 0xff7f9e2c, },
	{ 0.97f, 0xff597311, },
	{ 1.00f, 0xff597311, },
};

static SONGINFO songinfo[] = {
    {0, 10,  "", "dangerous", },
    {0, 30,  "", "take me to your heart"},
    {0, 180, "", "bean", "michael jackon"},
};

static ALBUMINFO albuminfo[] = {
    {songinfo, "jayalbum", "mj", "res/musicplayer/Album/default.png", TABLESIZE(songinfo)},
    {songinfo, "linalbum", "michael", "res/musicplayer/Album/big01.png", TABLESIZE(songinfo)},
};

static MUSICPLAYERDATAIN datain = {
    albuminfo, 0, 1, ActivityMusicPlayer::MP_M_ALBUM_CYCLE, TABLESIZE(albuminfo),
};

static int setProperty(mWidget *self, ActivityMusicPlayer *activity){
    mWidget *tmp;

    tmp = ncsGetChildObj(self->hwnd, IDC_IMAGE);
    ncsSetProperty(tmp->hwnd, NCSP_IMAGE_IMAGE, (DWORD)activity->m_bmpCD);

    return 0;
}

static int setFontColor(mWidget *self){
    mWidget *widget;

    widget = (mWidget *)ncsGetChildObj(self->hwnd, IDC_CTRL_INFO);
    ncsSetElementEx(widget, NULL, NCS_FGC_3DBODY, 0xff90c8e1);

    widget = (mWidget *)ncsGetChildObj(self->hwnd, IDC_CTRL_CURTIME);
    ncsSetElementEx(widget, NULL, NCS_FGC_3DBODY, 0xff998b78);

    widget = (mWidget *)ncsGetChildObj(self->hwnd, IDC_CTRL_TOTALTIME);
    ncsSetElementEx(widget, NULL, NCS_FGC_3DBODY, 0xff998b78);

    return 0;
}
static BOOL update_trkbar(mTrackBar *listener, mTimer *sender, int id, DWORD total_count)
{
    ActivityMusicPlayer *activity;
    int total, cur;
    int maxpos;
    int curpos;
    static int old_curpos = 0;

    activity = (ActivityMusicPlayer *)ActivityMusicPlayer::getActivityFromHWND(GetParent(listener->hwnd));
    cur = activity->getCurTime();
    total = activity->getTotalTime();
    maxpos = _c(listener)->getProperty(listener, NCSP_TRKBAR_MAXPOS);
    curpos = cur * maxpos / total;
    if(curpos == old_curpos)
        return TRUE;
    if((int)curpos != maxpos){
        ExcludeWindowStyle(listener->hwnd, NCSS_NOTIFY);
    }
    _c(listener)->setProperty (listener, NCSP_TRKBAR_CURPOS, (int)curpos);
    old_curpos = curpos;
    if((int)curpos != maxpos){
        IncludeWindowStyle(listener->hwnd, NCSS_NOTIFY);
    }

    return TRUE;
}
static BOOL update_curtime(mStatic *listener, mTimer *sender, int id, DWORD total_count)
{
    ActivityMusicPlayer *activity;
    int timerVal;
    int cur = 0;
    char time[10];
    static char oldtime[10] = {0};

    activity = (ActivityMusicPlayer *)ActivityMusicPlayer::getActivityFromHWND(GetParent(listener->hwnd));
    cur = activity->curTimeInc();
    timerVal = _c(sender)->getProperty(sender, NCSP_TIMER_INTERVAL);
    sprintf(time, "%02d:%02d", getMinute(cur, timerVal), getSecond(cur, timerVal));
    if(strcmp(time, oldtime) != 0)
    {
        _c(listener)->setProperty(listener, NCSP_WIDGET_TEXT, (DWORD)time);
        strcpy(oldtime, time);
    }

    return TRUE;
}

// START_OF_WNDHANDLERS
static BOOL mainwnd_timerListener(mWidget* self)
{
    mTimer *timer;

    timer = SAFE_CAST(mTimer, _c(self)->getChild(self, IDC_TIMER));
    if(timer){
        ncsAddEventListener((mObject *)timer,
                (mObject *)ncsGetChildObj(self->hwnd, IDC_CTRL_TRACKBAR),
                (NCS_CB_ONPIECEEVENT)update_trkbar,
                MSG_TIMER);
        ncsAddEventListener((mObject *)timer, 
                (mObject *)ncsGetChildObj(self->hwnd, IDC_CTRL_CURTIME),
                (NCS_CB_ONPIECEEVENT)update_curtime,
                MSG_TIMER);
        //_c(timer)->start(timer);
    }
    return TRUE;
}

static BOOL update_curtime_trkbar(mStatic *listener, mTrackBar *sender, int id, DWORD total_count)
{
    ActivityMusicPlayer* activity;
    mDialogBox *dialogbox;
    mTimer *timer;
    char time[10];
    int curpos, curtime;
    int maxpos;
    int timeVal;

    dialogbox = (mDialogBox *)ncsGetParentObj(sender->hwnd);
    activity = (ActivityMusicPlayer *)ActivityMusicPlayer::getActivityFromHWND(GetParent(sender->hwnd));
    timer = (mTimer *)ncsGetChildObj(dialogbox->hwnd, IDC_TIMER);
    timeVal = _c(timer)->getProperty(timer, NCSP_TIMER_INTERVAL);
    curpos = _c(sender)->getProperty(sender, NCSP_TRKBAR_CURPOS);
    maxpos = _c(sender)->getProperty(sender, NCSP_TRKBAR_MAXPOS);
    curtime = curpos * activity->getTotalTime()/maxpos;
    activity->setCurTime(curtime);
    sprintf(time, "%02d:%02d", getMinute(curtime, timeVal), getSecond(curtime, timeVal));

    _c(listener)->setProperty(listener, NCSP_WIDGET_TEXT, (DWORD)time);

    printf("enter the update_curtime_trkbar, cur is %d, pos is %d\n", curtime, curpos);

    return TRUE;
}

static int mainwnd_trkbarListener(mWidget *self)
{
    mTrackBar *trackbar;

    trackbar = SAFE_CAST(mTrackBar, _c(self)->getChild(self, IDC_CTRL_TRACKBAR));
    if(trackbar){
        ncsAddEventListener((mObject *)trackbar,
                (mObject *)ncsGetChildObj(self->hwnd, IDC_CTRL_CURTIME),
                (NCS_CB_ONPIECEEVENT)update_curtime_trkbar,
                NCSN_TRKBAR_CHANGED);
    }

    return 0;
}

static BOOL onPrev(mObject* self, mObject* piece,
        int event_id, DWORD param)
{
    ActivityMusicPlayer* _this = (ActivityMusicPlayer*)Activity::getActivityFromHWND(
            GetMainWindowHandle(((mWidget*)self)->hwnd));

    if(event_id != NCSN_ABP_CLICKED)
        return FALSE;
    printf("onPrev\n");

    _this->onPrev();

    return TRUE;
}
static BOOL onPlay(mObject* self, mObject* piece,
        int event_id, DWORD param)
{
    ActivityMusicPlayer* _this = (ActivityMusicPlayer*)Activity::getActivityFromHWND(
            GetMainWindowHandle(((mWidget*)self)->hwnd));

    if(event_id != NCSN_ABP_CLICKED)
        return FALSE;
    printf("onPlay\n");

    _this->m_toolbar->switchPlayState();

    if (_this->m_toolbar->isPlay())
        _this->onMusicStart();
    else
        _this->onPause();

    return TRUE;
}
static BOOL onNext(mObject* self, mObject* piece,
        int event_id, DWORD param)
{
    ActivityMusicPlayer* _this = (ActivityMusicPlayer*)Activity::getActivityFromHWND(
            GetMainWindowHandle(((mWidget*)self)->hwnd));

    if (event_id != NCSN_ABP_CLICKED)
        return FALSE;
    printf("onNext\n");

    _this->onNext();

    return TRUE;
}


static BOOL mainwnd_onCreate (mWidget* self, DWORD add_data){
    ActivityMusicPlayer *tmp = (ActivityMusicPlayer *)Activity::getActivityFromHWND(self->hwnd);

    mWidget* trkbar = ncsGetChildObj(self->hwnd, IDC_CTRL_TRACKBAR);
    assert(trkbar);

    tmp->initNavbar(self);

    tmp->m_toolbar = new MusicPlayerToolbar(self->hwnd);
    tmp->m_toolbar->addBtnEventListener(MusicPlayerToolbar::TB_PREV,
            NCSN_ABP_CLICKED, onPrev);
    tmp->m_toolbar->addBtnEventListener(MusicPlayerToolbar::TB_PLAY,
            NCSN_ABP_CLICKED, onPlay);
    tmp->m_toolbar->addBtnEventListener(MusicPlayerToolbar::TB_NEXT,
            NCSN_ABP_CLICKED, onNext);

    //set property
    setProperty(self, tmp);

    //set font color
    setFontColor(self);

    //load the init data
    tmp->loadData(self);

    //add timer listener
    mainwnd_timerListener(self);

    //add trkbar listener
    mainwnd_trkbarListener(self);

    return TRUE;
}
static void mainwnd_onClose (mWidget* self, int message){
    return;
}

//rush color format:argb
static BOOL mainwnd_onPaint(mWidget *self, HDC hdc, const RECT *pinv)
{
    RECT rc;
    HBRUSH brush;
    HPATH  path;
    ARGB   color[2];
    HGRAPHICS hgs = MGPlusGraphicCreateFromDC(hdc);

    GetClientRect(self->hwnd, &rc);

    brush = MGPlusBrushCreate(MP_BRUSH_TYPE_LINEARGRADIENT);
    path  = MGPlusPathCreate(MP_PATH_FILL_MODE_ALTERNATE);

    color[0] = 0xff616570;//DWORDTOARGB(0xff706561);
    color[1] = 0xff393a3f;//DWORDTOARGB(0xff3f3a39);

    MGPlusPathAddRectangle(path, rc.left, rc.top, RECTW(rc), RECTH(rc));

    MGPlusSetLinearGradientBrushMode(brush, MP_LINEAR_GRADIENT_MODE_VERTICAL);
    MGPlusSetLinearGradientBrushRect(brush, &rc);
    MGPlusSetLinearGradientBrushColors(brush, color, 2);

    MGPlusFillPath(hgs, brush, path);

    MGPlusGraphicSave(hgs, hdc, 0, 0, 0, 0, 0, 0);

    MGPlusPathDelete(path);
    MGPlusBrushDelete(brush);
    MGPlusGraphicDelete(hgs);
    
    return TRUE;
}

static NCS_EVENT_HANDLER mainwnd_handlers[] = {
    {MSG_CREATE,    reinterpret_cast<void*>(mainwnd_onCreate)},
    {MSG_CLOSE,     reinterpret_cast<void*>(mainwnd_onClose)},
    {MSG_PAINT,     reinterpret_cast<void*>(mainwnd_onPaint)},
	{0, NULL }
};

static NCS_PROP_ENTRY timer_props[] = {
    {NCSP_TIMER_INTERVAL, 1},//unit:10ms
    {0, 0},
};
#if 0
static void titlecmd_notify(mWidget *self, int id, int nc, DWORD add_data){
    printf("enter the titlecmd_notify\n");
}

static NCS_EVENT_HANDLER titlecmd_handlers[] = {
    NCS_MAP_NOTIFY(NCSN_BNB_BUTTONCLICK, titlecmd_notify),
    {0, NULL},
};
#endif
static NCS_PROP_ENTRY rect_props [] = {
    {NCSP_RECTANGLE_BORDERSIZE, 0},
    {NCSP_RECTANGLE_FILLCOLOR, 0xFF333333},
    {0, 0}
};
static NCS_PROP_ENTRY trk_props [] = {
    {NCSP_TRKBAR_MINPOS, 0},
    {NCSP_TRKBAR_MAXPOS, 272},
    {NCSP_TRKBAR_CURPOS, 0},
    {NCSP_TRKBAR_LINESTEP, 1},
    {NCSP_TRKBAR_PAGESTEP, 272},
    {0, 0},
};
static void trkbar_reachmax_notify(mTrackBar* self, int id, int code, DWORD add_data){
    ActivityMusicPlayer *activity;
    mDialogBox *dialogbox;

    dialogbox = (mDialogBox *)ncsObjFromHandle(GetParent(self->hwnd));
    activity = (ActivityMusicPlayer *)ActivityMusicPlayer::getActivityFromHWND(dialogbox->hwnd);
    activity->onReachMax();
}
static void trkbar_reachmin_notify(mTrackBar* self, int id, int code, DWORD add_data){
    printf("reach minpos of the trkbar\n");
}

static NCS_EVENT_HANDLER track_handlers[] = {
    NCS_MAP_NOTIFY(NCSN_TRKBAR_REACHMAX, trkbar_reachmax_notify),
    NCS_MAP_NOTIFY(NCSN_TRKBAR_REACHMIN, trkbar_reachmin_notify),
    {0, NULL}
};
#if 0
static void cmd_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    ActivityMusicPlayer* activity = (ActivityMusicPlayer *)Activity::getActivityFromHWND(GetParent(self->hwnd));
    if (NULL != activity) {
        const char* name = GetWindowCaption(self->hwnd);
        if (NULL != name) {
            puts(name);
            activity->onCommand(name);
        }
    }
}
#endif
static NCS_WND_TEMPLATE _ctrl_tmpl[]= {
#if 0
    {
        NCSCTRL_BTNNAVBAR,
        IDC_STATIC_TITLE,
        0, 0, MP_TITLE_W, MP_TITLE_H,
        WS_VISIBLE|NCSS_BNB_LOPT|NCSS_NOTIFY,
        WS_EX_NONE,
        "",
        NULL, NULL, NULL, NULL,
        0, 0,
    },
#endif
#if 0
    {
        NCSCTRL_RECTANGLE,
        IDC_CTRL_CONT,
        MP_CONT_X, MP_CONT_Y, MP_CONT_W, MP_CONT_H, 
        WS_VISIBLE,
        WS_EX_TRANSPARENT,//WS_EX_NONE,
        "",
        cont_props, cont_rdr_info, NULL, NULL,
        0, 0
    },
#endif
    {
        NCSCTRL_IMAGE,
        IDC_IMAGE,
        MP_IMAGE_X, MP_IMAGE_Y, MP_IMAGE_W, MP_IMAGE_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL, NULL, NULL, NULL,
        0, 0,
    },
    {
        NCSCTRL_STATIC,
        IDC_CTRL_INFO,
        MP_INFO_X, MP_INFO_Y, MP_INFO_W, MP_INFO_H,
        WS_VISIBLE,
        WS_EX_TRANSPARENT,
        "",
        NULL, NULL, NULL, NULL,
        0, 0,
    },
    {
        NCSCTRL_TIMER,
        IDC_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timer_props, NULL, NULL, NULL, 0, 0, 
    },
    {
        NCSCTRL_RECTANGLE,
        IDC_CTRL_BK,
        MP_CTRL_RECT_X, MP_CTRL_RECT_Y, MP_CTRL_RECT_W, MP_CTRL_RECT_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        rect_props, NULL, NULL, NULL,
        0, 0,
    },
    {
        NCSCTRL_SEPARATOR,
        IDC_CTRL_SEPARATOR,
        MP_CTRL_SEPA_X, MP_CTRL_SEPA_Y, MP_CTRL_SEPA_W, MP_CTRL_SEPA_H,
        WS_VISIBLE,
        WS_EX_TRANSPARENT,
        "",
        NULL, NULL, NULL, NULL,
        0, 0,
    },
    {
        NCSCTRL_NEWTRACKBAR,
        IDC_CTRL_TRACKBAR,
        MP_CTRL_TB_X, MP_CTRL_TB_Y, MP_CTRL_TB_W, MP_CTRL_TB_H,
        WS_VISIBLE | NCSS_NOTIFY | NCSS_NTRKBAR_CIRCLE,
        WS_EX_TRANSPARENT | WS_EX_TROUNDCNS | WS_EX_BROUNDCNS,
        "",
        trk_props, /*track_rdr_info*/NULL, track_handlers, NULL, 
        0, 0,
    },
    {
        NCSCTRL_STATIC,
        IDC_CTRL_CURTIME,
        MP_CTRL_CURTIME_X,MP_CTRL_CURTIME_Y,MP_CTRL_CURTIME_W,MP_CTRL_CURTIME_H,
        WS_VISIBLE,
        WS_EX_TRANSPARENT,
        "",
        NULL, NULL, NULL, NULL,
        0, 0,
    },
    {
        NCSCTRL_STATIC,
        IDC_CTRL_TOTALTIME,
        MP_CTRL_TOTALTIME_X,MP_CTRL_TOTALTIME_Y,MP_CTRL_TOTALTIME_W,MP_CTRL_TOTALTIME_H,
        WS_VISIBLE,
        WS_EX_TRANSPARENT,
        "",
        NULL, NULL, NULL, NULL,
        0, 0,

    },

#if 0
    {
        NCSCTRL_BUTTON,
        IDC_BTN_PREV,
        MP_BUTTON1_X, MP_BUTTON1_Y, MP_BUTTON_W, MP_BUTTON_H,
        WS_VISIBLE|NCSS_BUTTON_IMAGE,
        WS_EX_NONE,
        "prev",
        NULL, NULL, command_handlers, NULL, 0, 0,
    },
    {
        NCSCTRL_BUTTON,
        IDC_BTN_PAUSE,
        MP_BUTTON2_X, MP_BUTTON2_Y, MP_BUTTON_W, MP_BUTTON_H,
        WS_VISIBLE|NCSS_BUTTON_IMAGE,
        WS_EX_NONE,
        "pause",
        NULL, NULL, command_handlers, NULL, 0, 0,
    },
    {
        NCSCTRL_BUTTON,
        IDC_BTN_NEXT,
        MP_BUTTON3_X, MP_BUTTON3_Y, MP_BUTTON_W, MP_BUTTON_H,
        WS_VISIBLE|NCSS_BUTTON_IMAGE,
        WS_EX_NONE,
        "next",
        NULL, NULL, command_handlers, NULL, 0, 0,
    },
    {
        NCSCTRL_BUTTON,
        IDC_BTN_START,
        MP_BUTTON2_X, MP_BUTTON2_Y, MP_BUTTON_W, MP_BUTTON_H,
        NCSS_BUTTON_IMAGE,
        WS_EX_NONE,
        "start",
        NULL, NULL, command_handlers, NULL, 0, 0,
    },
#endif
};

static NCS_RDR_INFO dialog_rdr_info[] =
{
    //{"flat", "flat", NULL},
    //{"skin", "skin", NULL},
    //{"classic", "classic", NULL},
    {"fashion","fashion",NULL},
    //{"pad", "pad", NULL},
};

static NCS_MNWND_TEMPLATE dialog_tmpl = {
    NCSCTRL_DIALOGBOX,
    1,
    MP_X, MP_Y, MP_W, MP_H,
    WS_NONE,
    WS_EX_AUTOSECONDARYDC,
    APP_NAME_MUSICPLAYER,
    NULL, dialog_rdr_info,
    mainwnd_handlers,
    _ctrl_tmpl,
    TABLESIZE(_ctrl_tmpl),
    0,
    0, 0,
};

int ActivityMusicPlayer::loadData(mWidget *self){
    mStatic *st;
    mTimer *timer;
    int tmp;
    int timerVal;
    char time[10];
    char info[256];

    printf("enter the loaddata\n");
    timer = (mTimer *)ncsGetChildObj(self->hwnd, IDC_TIMER);
    timerVal = _c(timer)->getProperty(timer, NCSP_TIMER_INTERVAL);

    tmp = getTime(albuminfo[m_album].pSong[m_index].curtime, timerVal);
    setCurTime(tmp);
    sprintf(time, "%02d:%02d", getMinute(tmp, timerVal), getSecond(tmp, timerVal));
    st = (mStatic *)ncsGetChildObj(self->hwnd, IDC_CTRL_CURTIME);
    _c(st)->setProperty(st, NCSP_WIDGET_TEXT, (DWORD)time);

    tmp = getTime(albuminfo[m_album].pSong[m_index].totaltime, timerVal);
    setTotalTime(tmp);
    sprintf(time, "%02d:%02d", getMinute(tmp, timerVal), getSecond(tmp, timerVal));
    st = (mStatic *)ncsGetChildObj(self->hwnd, IDC_CTRL_TOTALTIME);
    _c(st)->setProperty(st, NCSP_WIDGET_TEXT, (DWORD)time);

    st = (mStatic *)ncsGetChildObj(self->hwnd, IDC_CTRL_INFO);
    sprintf(info, "%s-%s", albuminfo[m_album].pSong[m_index].name, albuminfo[m_album].pSong[m_index].author);
    ncsSetProperty(st->hwnd, NCSP_WIDGET_TEXT, (DWORD)info);

#if 0
    btnnavbar = (mBtnNavBar *)ncsGetChildObj(self->hwnd, IDC_STATIC_TITLE);
    _c(btnnavbar)->setProperty(btnnavbar, NCSP_WIDGET_TEXT, (DWORD)albuminfo[m_album].name);
    _c(btnnavbar)->setProperty(btnnavbar, NCSP_BNB_MARGIN, 1);
    _c(btnnavbar)->setProperty(btnnavbar, NCSP_BNB_LSTRING, (DWORD)"Album");
#endif

    printf("enter the loaddata\n");
    return 0;
}

static BOOL evt_navbarBtnDown(mWidget* self, mHotPiece *piece,
        int event_id, DWORD param)
{
    if (event_id == NCSN_ABP_CLICKED) {
        ActivityStack::singleton()->pop();
        return TRUE;
    }

    return FALSE;
}

void ActivityMusicPlayer::initNavbar(mWidget* self)
{
    RECT rc;

    rc.left = 0;
    rc.top = 0;

    mContainerCtrl* ctrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "navbar",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            NAVBAR_X, NAVBAR_Y, NAVBAR_W, NAVBAR_H,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mPanelPiece* body = (mPanelPiece*)NEWPIECE(mPanelPiece);

    rc.right = NAVBAR_W;
    rc.bottom = NAVBAR_H;
    _c(body)->setRect (body, &rc);
    _c(ctrl)->setBody(ctrl, (mHotPiece*)body);

    {
        unsigned i;
        mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_gradient)->setRect (back_gradient, &rc);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 0);
#if 0
        ARGB colors[3] = {0x90F0F0F0, 0x90808080};
        _c((mShapeTransRoundPiece*)back_gradient)->setGradientFillColors(
                (mShapeTransRoundPiece*)back_gradient, colors, TABLESIZE(colors));
#else
        HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
                (mShapeTransRoundPiece*)back_gradient);

        /* 0.0f, 1.0f are needed */
        for (i=0; i<TABLESIZE(mp_navbar_gradient); ++i)
            MGPlusLinearGradientBrushAddColor(brush, mp_navbar_gradient[i].color, mp_navbar_gradient[i].pos);
#endif
        _c(body)->addContent(body, back_gradient, 0, 0);
    }

    // title
    rc.right = NAVBAR_TITLE_W;
    rc.bottom = NAVBAR_TITLE_H;
    m_navbarTitlePiece = (mHotPiece*)NEWPIECE(mTextPiece);
    assert(m_navbarTitlePiece);
    _c(m_navbarTitlePiece)->setRect (m_navbarTitlePiece, &rc);
    _c(m_navbarTitlePiece)->setProperty(m_navbarTitlePiece, NCSP_LABELPIECE_LABEL,
            (DWORD)"Lost Highway");
    _c(m_navbarTitlePiece)->setProperty(m_navbarTitlePiece, NCSP_TEXTPIECE_LOGFONT,
            (DWORD)m_navbarTitleFont);
    _c(m_navbarTitlePiece)->setProperty(m_navbarTitlePiece, NCSP_TEXTPIECE_TEXTCOLOR,
            (DWORD)0xff192952);
    _c(m_navbarTitlePiece)->setProperty(m_navbarTitlePiece, NCSP_TEXTPIECE_TEXTSHADOWCOLOR,
            (DWORD)0xffffffff);
    _c(body)->addContent(body, (mHotPiece*)m_navbarTitlePiece,
            NAVBAR_TITLE_X, NAVBAR_TITLE_Y);

    // container for button
    rc.right = NAVBAR_BTN_W;
    rc.bottom = NAVBAR_BTN_H;
    mButtonPanelPiece *btnPanel = NEWPIECE(mButtonPanelPiece);
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
    mShapeTransRoundPiece *bkgnd = (mShapeTransRoundPiece*)_c(btnPanel)->getBkgndPiece(btnPanel);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_RADIUS, 4);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_SHARPFLAG, TRANROUND_SHARPFLAG_LEFT);

    mHotPiece* btn = (mHotPiece*)NEWPIECE(mTextPiece);
    _c(btn)->setRect (btn, &rc);
    _c(btn)->setProperty(btn, NCSP_LABELPIECE_LABEL, (DWORD)"Album");
    _c(btn)->setProperty(btn, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_navbarBtnFont);
    _c(btn)->setProperty(btn, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xff192952);
    _c(btnPanel)->addContent(btnPanel, (mHotPiece*)btn, 5, 0);

    _c(body)->addContent(body, (mHotPiece*)btnPanel, NAVBAR_BTN_X, NAVBAR_BTN_Y);

    // add event listener for button
    ncsAddEventListener((mObject*)btnPanel, (mObject*)ctrl, 
            (NCS_CB_ONPIECEEVENT)evt_navbarBtnDown, NCSN_ABP_CLICKED);
}

int ActivityMusicPlayer::onReachMax()
{
    mWidget *dialogbox;
    mTimer *timer;
    int index;

    dialogbox = ncsObjFromHandle(m_hwnd);
    timer = (mTimer *)ncsGetChildObj(dialogbox->hwnd, IDC_TIMER);
    _c(timer)->stop(timer);
    switch(getMode())
    {
        case MP_M_SINGLE:
            onPause();
            return 0;

        case MP_M_ALBUM:
            index = getIndex();
            if(index + 1 >= albuminfo[m_album].n){
                index = 0;
                onPause();
                return 0;
            } else
                index++;
            break;

        case MP_M_ALBUM_CYCLE:
            index = getIndex();
            index = (index + 1 >= albuminfo[m_album].n) ? 0 : (index + 1);
            setIndex(index);
            break;

        case MP_M_SINGLE_CYCLE:
        default:
                break;
    }
    loadData(dialogbox);
    _c(timer)->start(timer);
    return 0;
}

int ActivityMusicPlayer::onPrev()
{
    mWidget *dialogbox;
    mTimer *timer;
    int index;
    int album;

    printf("enter the noPrev\n");
    dialogbox = ncsObjFromHandle(m_hwnd);
    timer = (mTimer *)ncsGetChildObj(dialogbox->hwnd, IDC_TIMER);
    _c(timer)->stop(timer);

    index = getIndex();
    album = getAlbum();

    if(index <= 0){
        index = albuminfo[album].n - 1;
        album = (album <= 0) ? (datain.n - 1) : (album - 1);
    } else{
        index--;
    }

    printf("count=%d, index=%d, album=%d\n", albuminfo[album].n, index, album);
    setIndex(index);
    setAlbum(album);

    loadData(dialogbox);

    if (m_toolbar->isPlay())
        onMusicStart();

    return 0;
}

int ActivityMusicPlayer::onPause()
{
    mTimer *timer;

    printf("enter the onPause\n");
    timer = (mTimer *)ncsGetChildObj(m_hwnd, IDC_TIMER);
    _c(timer)->stop(timer);

#if 0
    btn = (mButton *)ncsGetChildObj(m_hwnd, IDC_BTN_PAUSE);
    ExcludeWindowStyle(btn->hwnd, WS_VISIBLE);

    btn = (mButton *)ncsGetChildObj(m_hwnd, IDC_BTN_START);
    IncludeWindowStyle(btn->hwnd, WS_VISIBLE);
    InvalidateRect(btn->hwnd, NULL, FALSE);
#endif

    return 0;
}

int ActivityMusicPlayer::onMusicStart()
{
    mTimer *timer;

    printf("enter the onMusicStart\n");
    timer = (mTimer *)ncsGetChildObj(m_hwnd, IDC_TIMER);

#if 0
    btn = (mButton *)ncsGetChildObj(m_hwnd, IDC_BTN_START);
    ExcludeWindowStyle(btn->hwnd, WS_VISIBLE);
    
    btn = (mButton *)ncsGetChildObj(m_hwnd, IDC_BTN_PAUSE);
    IncludeWindowStyle(btn->hwnd, WS_VISIBLE);
    InvalidateRect(btn->hwnd, NULL, FALSE);
#endif

    _c(timer)->start(timer);
    return 0;
}

int ActivityMusicPlayer::onNext()
{
    mWidget *dialogbox;
    mTimer *timer;
    int index;
    int album;

    printf("enter the noNext\n");
    dialogbox = ncsObjFromHandle(m_hwnd);
    timer = (mTimer *)ncsGetChildObj(dialogbox->hwnd, IDC_TIMER);
    _c(timer)->stop(timer);

    index = getIndex();
    album = getAlbum();

    if(index + 1 >= albuminfo[album].n){
        index = 0;
        album = (album + 1 >= datain.n) ? (0) : (album + 1);
    } else{
        index++;
    }

    printf("count=%d, index=%d, album=%d\n", albuminfo[album].n, index, album);
    setIndex(index);
    setAlbum(album);

    loadData(dialogbox);

    if (m_toolbar->isPlay())
        onMusicStart();

    return 0;
}

int ActivityMusicPlayer::getCurTime()
{
    return m_curtime;
}

int ActivityMusicPlayer::getTotalTime()
{
    return m_totaltime;
}

int ActivityMusicPlayer::curTimeInc(){
    return ++m_curtime;
}

int ActivityMusicPlayer::curTimeDec(){
    return --m_curtime;
}

void ActivityMusicPlayer::setCurTime(int cur)
{
    m_curtime = cur;
}

void ActivityMusicPlayer::setTotalTime(int total)
{
    m_totaltime = total;
}

void ActivityMusicPlayer::setIndex(int index){
    m_index = index;
}

int ActivityMusicPlayer::getIndex(){
    return m_index;
}

int ActivityMusicPlayer::indexInc(){
    return ++m_index;
}

int ActivityMusicPlayer::indexDec(){
    return --m_index;
}

void ActivityMusicPlayer::setAlbum(int album){
    m_album = album;
}

int ActivityMusicPlayer::getAlbum(){
    return m_album;
}

int ActivityMusicPlayer::albumInc(){
    return ++m_album;
}

int ActivityMusicPlayer::albumDec(){
    return --m_album;
}

int ActivityMusicPlayer::getMode(){
    return m_mode;
}


void ActivityMusicPlayer::loadMyResource()
{
    m_bmpCD = (PBITMAP) Load32Resource(MUSIC_IMG_PATH, RES_TYPE_IMAGE, 0);

    m_navbarTitleFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            20, 0);
    m_navbarBtnFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            16, 0);
    m_font12 = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            12, 0);
}

void ActivityMusicPlayer::unloadMyResource()
{
    DestroyLogFont(m_navbarTitleFont);
    DestroyLogFont(m_navbarBtnFont);
    DestroyLogFont(m_font12);

    ReleaseRes(Str2Key(MUSIC_IMG_PATH));
}

ActivityMusicPlayer::ActivityMusicPlayer():
    NCSActivity( &dialog_tmpl){
       // loadConfig();
        m_curtime = 0;
        m_totaltime = 0;
        m_album = datain.album;
        m_index = datain.index;
        m_mode = datain.mode;

        MGNCS_INIT_CLASS(mLayoutManager);
        MGNCS_INIT_CLASS(mCenterHBox);
        MGNCS_INIT_CLASS(mButtonPanelPiece);
        loadMyResource();
}

ActivityMusicPlayer::~ActivityMusicPlayer(){
    delete m_toolbar;

    unloadMyResource();
}



