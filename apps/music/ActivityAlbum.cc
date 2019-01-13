#include <string.h>

#include "global.h"

#include "Activity.hh"
#include "ActivityAlbum.hh"
#include "MediaStoreService.hh"
#include "AlbumTableViewPiece.hh"
#include "SongTableViewPiece.hh"
#include "../common/mactivitytoolbarpiece.h"

REGISTER_ACTIVITY(PhoneAlbumActivity);

#define TIMETEXT_COLOR  0xff998b78
#define TIMETEXT_SIZE   12
//ctrl id

static ARGB title_gradient_color[] = {0xffd0d571, 0xffb5c363,
        0xff9eb23a,0xff7f9e2c,0xff597311,0xff597311};
static float title_gradient_pos[] = {0.0, 0.49,0.50,0.96,0.97,1.0};

static ARGB trkbar_gradient_color[] = {0x99000000, 0x994d4d4d,0x99000000,0x99000000};
static float trkbar_gradient_pos[] = {0.0, 0.02,0.03,1.0};

#if WIDGETHOST_FROM_DESKTOP
static mWidget *g_trackbarWidget = NULL;
#endif

static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData );
static BOOL main_onClose(mWidget* self, int message);
static BOOL main_onDestroy(mWidget* self, int message);
static BOOL playall_btn_album_event_handler(mObject *self, mObject *piece,int event_id, DWORD param);
static BOOL playall_btn_song_event_handler(mObject *self, mObject *piece,int event_id, DWORD param);
inline int time2string (int totaltime,int second, char *strbuf, int bufsize);

static mImagePiece *g_albumImage = NULL;
static int album_pre_index = -1;
static void resetString(char *src,char *dst,int len);
#define MUSIC_IMG_PATH "res/musicplayer/player/CD.png"
const static char *PICTURES[] = {
    "res/musicplayer/player/prev.png",
    "res/musicplayer/player/pause.png",
    "res/musicplayer/player/next.png",
    "res/musicplayer/player/play.png",
};
enum{
    MUSIC_PREV_PICTURE = 0,
    MUSIC_PAUSE_PICTURE,
    MUSIC_NEXT_PICTURE,
    MUSIC_PLAY_PICTURE,
};

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
static NCS_PROP_ENTRY timer_props[] = {
    {NCSP_TIMER_INTERVAL, 20},//unit:10ms
    {0, 0},
};
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
    },
    {
        NCSCTRL_TIMER,
        IDC_ALBUM_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timer_props, NULL, NULL, NULL, 0, 0, 
    },
    {
        NCSCTRL_TIMER,
        IDC_UPDATE_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timer_props, NULL, NULL, NULL, 0, 0, 
    },
};

static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static BOOL locateAlbumSong(PhoneAlbumActivity* act,char *albumAndSong)
{
    int i,j;
    char albumname[64] = {0};
    char songname[64] = {0};
    PMUSICPLAYERDATAIN playlist = act->getPlayerDatain();

    int index;
    int len = strlen(albumAndSong);
    for (index=0;index<len;index++)
    {
        if (albumAndSong[index] == '&')
        {
            strncpy(albumname,albumAndSong,(index < 64 - 1)?index:(64 - 1));
            if (++index < len)
            {
                strncpy(songname,(char *)&albumAndSong[index],
                        ((len - index) < 64 - 1)?(len - index):(64 - 1));
            }
            break;
        }
    }
    for (i=0;i<playlist->n;i++)
    {
        if (strcmp(playlist->pAlbum[i].name,albumname) == 0)
        {
            playlist->album = i;
            break;
        }
    }
    if (i < playlist->n)
    {
        for (j=0;j<playlist->pAlbum[i].n;j++)
        {
            if (strcmp(playlist->pAlbum[i].pSong[j].name,songname) == 0)
            {
                playlist->index = j;
                break;
            }
        }
    }
    playlist->mode = act->MP_M_ALBUM_CYCLE;
    return TRUE;
}
static BOOL main_userApp(mMainWnd* self, int msg, DWORD wparam, DWORD lparam)
{
    mWidget *ctrl_widget;
    PhoneAlbumActivity* activity = 
        (PhoneAlbumActivity*)Activity::getActivityFromHWND (self->hwnd);

    Intent *intent;

    ctrl_widget =ncsGetChildObj (self->hwnd, IDC_NAV);
    activity->setCtrl(ctrl_widget);

    intent = (Intent *)wparam;
    if (intent != NULL)
    {
        if (intent->action() == ACTION_VIEW)
        {
            char album_song[128] = {0};
            std::string info = boost::any_cast<std::string>(intent->getString("music info"));
            strncpy(album_song,info.c_str(),128 - 1);
            activity->initAlbumActivity (ctrl_widget->hwnd,TRUE);
            locateAlbumSong(activity,album_song);
            activity->resetDisplay();
        }
        else
        {
            activity->initAlbumActivity (ctrl_widget->hwnd,FALSE);
        }
    }
    else
    {
        activity->initAlbumActivity (ctrl_widget->hwnd,FALSE);
    }

    return TRUE;
}

static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE,      reinterpret_cast<void*>(main_onCreate)},
    {MSG_CLOSE,       reinterpret_cast<void*>(main_onClose)},
    {MSG_DESTROY,       reinterpret_cast<void*>(main_onDestroy)},
    {MSG_LBUTTONDOWN, reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_LBUTTONUP,   reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_MOUSEMOVE,   reinterpret_cast<void*>(SpeedMeterMessageHandler)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {MSG_USER_APP_DATA, reinterpret_cast<void*>(main_userApp)},
    {0, NULL}
};

static NCS_MNWND_TEMPLATE dialog_tmpl = {
    NCSCTRL_DIALOGBOX,
    1,
    0, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H, /* full screen */
    WS_NONE,
    WS_EX_NONE,
    "PhoneContactsActivity",
    NULL,
    NULL,
    main_handlers,
    _ctrl_tmpl,
    sizeof(_ctrl_tmpl)/sizeof(NCS_WND_TEMPLATE),
    0,
    0, 0,
};

PhoneAlbumActivity::PhoneAlbumActivity() :
    NCSActivity( &dialog_tmpl), m_nav(NULL), m_buttonFont(NULL)
{
    MGNCS_INIT_CLASS(mAlbumTableViewPiece);
    MGNCS_INIT_CLASS(mSongTableViewPiece);
    MGNCS_INIT_CLASS(mActivityToolbarPiece);
    memset (&m_playerDatain, 0, sizeof(m_playerDatain));
    memset (&m_focusAlbum, 0, sizeof(m_focusAlbum));
    memset (m_navViews, 0, sizeof(m_navViews));
    memset (m_navItems, 0, sizeof(m_navItems));
    album_pre_index = -1;
}

PhoneAlbumActivity::~PhoneAlbumActivity()
{
    unloadResource ();
    unloadAblumInfo ();
}

void PhoneAlbumActivity::loadResource(void)
{
    int i;

    m_buttonFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            PLAYALL_BTN_TEXT_SIZE, 0);

    m_trkbarTitleFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            TRKBAR_TITLE_TEXT_SIZE, 0);
    m_timeFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            TIMETEXT_SIZE, 0);

    Load32Resource (MUSIC_IMG_PATH, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    for (i=0; i<m_playerDatain.n; ++i) {
        Load32Resource (m_playerDatain.pAlbum[i].photo, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    }
}

void PhoneAlbumActivity::unloadResource(void)
{
    int i;

    DestroyLogFont (m_buttonFont);
    DestroyLogFont (m_trkbarTitleFont);
    DestroyLogFont (m_timeFont);

    assert (m_playerDatain.pAlbum);

    for (i=0; i<m_playerDatain.n; ++i) {
        ReleaseRes (Str2Key (m_playerDatain.pAlbum[i].photo));
    }
    for (i=0;i<(signed)TABLESIZE(PICTURES);i++)
    {
        UnregisterRes(PICTURES[i]);
    }
    UnregisterRes(MUSIC_IMG_PATH);
}

int PhoneAlbumActivity::createNavigation (HWND ctrl_hwnd, int root_item)
{
    mContainerCtrl *ctrl;
    RECT rc;
    HBRUSH brush;

    assert (root_item >= NAVITEM_ALBUMLIST && root_item < NAVITEM_MAX);

    /* create nav item */
    m_navViews[NAVITEM_ALBUMLIST] = createAlbumListView ();
    m_navViews[NAVITEM_SONGLIST] = createSongListView ();
    ADDREFPIECE(m_navViews[NAVITEM_ALBUMLIST]);
    ADDREFPIECE(m_navViews[NAVITEM_SONGLIST]);

    printf ("######m_navViewsAlbum: %p, SongList: %p\n", m_navViews[NAVITEM_ALBUMLIST], m_navViews[NAVITEM_SONGLIST]);

    m_navItems[NAVITEM_ALBUMLIST] = ncsCreateNavigationItem(m_navViews[NAVITEM_ALBUMLIST], "Album", 0);
    m_navItems[NAVITEM_SONGLIST] = ncsCreateNavigationItem(m_navViews[NAVITEM_SONGLIST], "Song", 0);

    mButtonPanelPiece *playall_btn_album = createNavRightButton (
            m_navItems[NAVITEM_ALBUMLIST],"Play All", playall_btn_album_event_handler);
    _c(m_navItems[NAVITEM_ALBUMLIST])->setProperty(m_navItems[NAVITEM_ALBUMLIST],
                            NCSP_NAVIGATIONITEM_RIGHT_BUTTON, (DWORD)playall_btn_album);

    mButtonPanelPiece *playall_btn_song = createNavRightButton (
            m_navItems[NAVITEM_SONGLIST],"Play All", playall_btn_song_event_handler);
    _c(m_navItems[NAVITEM_SONGLIST])->setProperty(m_navItems[NAVITEM_SONGLIST],
                            NCSP_NAVIGATIONITEM_RIGHT_BUTTON, (DWORD)playall_btn_song);

    m_nav = ncsCreateNavigationPanelPieceWithRootView (m_navItems[root_item]);
    SetRect(&rc, 0, 0, ACTIVITY_W, ACTIVITY_H);
    _c(m_nav)->setRect(m_nav, &rc);

    /* navigation back ground (mShapeTransRoundPiece)*/
    mShapeTransRoundPiece *navbar_bk = NEWPIECE(mShapeTransRoundPiece);
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

mHotPiece *PhoneAlbumActivity::createAlbumListView()
{
    RECT rc = {0, 0, ACTIVITY_W, MUSIC_VIEW_H};
    mAlbumTableViewPiece *album_piece;

    album_piece = NEWPIECEEX (mAlbumTableViewPiece, this);
    assert (album_piece);
    _c(album_piece)->setRect (album_piece, &rc);
    _c(album_piece)->reloadData (album_piece);

    return (mHotPiece *)album_piece;
}

mHotPiece *PhoneAlbumActivity::createSongListView()
{
    RECT rc = {0, 0, ACTIVITY_W, MUSIC_VIEW_H};
    mSongTableViewPiece *song_piece;

    song_piece = NEWPIECEEX (mSongTableViewPiece, this);
    assert (song_piece);
    _c(song_piece)->setRect (song_piece, &rc);
    _c(song_piece)->reloadData (song_piece);

    return (mHotPiece*)song_piece;
}

int PhoneAlbumActivity::onBack()
{
    if (_c(m_nav)->currentIsRoot(m_nav))
        return 0;

    _c(m_nav)->pop(m_nav);
    return 1;
}

static void trkbar_title_change(PhoneAlbumActivity *act)
{
    static char temp[128] = {0};
    MUSICPLAYERDATAIN *player_datain;
    char *str_song,*str_name; 

    memset(temp,0x00,sizeof(temp));

    player_datain = act->getPlayerDatain();
    str_song = player_datain->pAlbum[player_datain->album].pSong[player_datain->index].name;
    str_name = player_datain->pAlbum[player_datain->album].pSong[player_datain->index].author;
    if (strlen(str_song) >= sizeof(temp) - 4)
    {
        strncpy(temp,str_song,sizeof(temp) - 4);
    }
    else
    {
        strcpy(temp,str_song);
    }
    strcat(temp," - ");
    if (strlen(str_name) < sizeof(temp) - strlen(temp))
        strcat(temp,str_name);

    _c(act->m_titlePiece)->setProperty(
            act->m_titlePiece, NCSP_LABELPIECE_LABEL, (DWORD)temp);

    if (album_pre_index != player_datain->album)
    {
        mNavigationPanelPiece *nav = act->getNavigationController();
        mNavigationBarPiece* bar;
        mTextPiece *text;
        static char name[MP_TITLE_NAME];
        char *str_name = player_datain->pAlbum[player_datain->album].name;
        RECT rect = {0,0,230,NAVIGATIONBAR_DEFAULT_BUTTON_H};

        const BITMAP *bitmap = RetrieveRes(player_datain->pAlbum[player_datain->album].photo);

        bar = (mNavigationBarPiece*)_c(nav)->getProperty(nav, NCSP_NAVIGATIONPANELPIECE_BAR);
        text = (mTextPiece*)_c(bar)->getProperty(bar, NCSP_NAVIGATIONBARPIECE_TITLE_BUTTON);
        _c(text)->setRect(text, &rect);

        memset(name,0x00,sizeof(name));
        resetString(str_name, name,MP_TITLE_NAME - 1);
        _c(bar)->movePiece(bar, (mHotPiece*)text,
                NAVIGATIONBAR_TITLE_X(RECTW(rect)),NAVIGATIONBAR_DEFAULT_BUTTON_Y);
        _c(text)->setProperty(text, NCSP_LABELPIECE_LABEL, (DWORD)name);

        if (bitmap == NULL)
        {
            bitmap = RetrieveRes(MUSIC_IMG_PATH);
        }
        _c(g_albumImage)->setProperty (g_albumImage,NCSP_IMAGEPIECE_IMAGE,(DWORD)bitmap); 
    }
    album_pre_index = player_datain->album;
}

static BOOL music_play_pre (mObject *self, mObject *piece, int event_id,  DWORD param)
{
    mContainerCtrl *ctnr;
    mWidget *mainwnd;
    PhoneAlbumActivity *act;

    ctnr = (mContainerCtrl*)_c((mPanelPiece*)self)->getOwner((mPanelPiece*)self);
    mainwnd = ncsObjFromHandle(GetParent(ctnr->hwnd));
    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);

    act->onMusicPrev();

    return TRUE;
}

static BOOL music_play_pause (mObject *self, mObject *piece, int event_id,  DWORD param)
{
    mContainerCtrl *ctnr;
    mWidget *mainwnd;
    PhoneAlbumActivity *act;

    ctnr = (mContainerCtrl*)_c((mPanelPiece*)self)->getOwner((mPanelPiece*)self);
    mainwnd = ncsObjFromHandle(GetParent(ctnr->hwnd));
    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);

    if(act->isPlay())
        act->onMusicPause();
    else
        act->onMusicPlay();
    return TRUE;
}

static BOOL music_play_next (mObject *self, mObject *piece, int event_id,  DWORD param)
{
    mContainerCtrl *ctnr;
    mWidget *mainwnd;
    PhoneAlbumActivity *act;

    ctnr = (mContainerCtrl*)_c((mPanelPiece*)self)->getOwner((mPanelPiece*)self);
    mainwnd = ncsObjFromHandle(GetParent(ctnr->hwnd));
    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);

    act->onMusicNext();

    return TRUE;
}

static mActivityToolbarPiece *create_toolbar ()
{
    RECT rc = {0, 0, ACTIVITY_W, MUSIC_TOOLBAR_H};
    BITMAP *bmp;
    mActivityToolbarPiece *toolbar;

    toolbar = NEWPIECEEX (mActivityToolbarPiece, 0x7|TOOLBAR_SELECT_COLOR);

    _c(toolbar)->setRect (toolbar, &rc);

    bmp = (BITMAP*)Load32Resource (PICTURES[MUSIC_PREV_PICTURE], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_LEFT, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_LEFT, music_play_pre);

    bmp = (BITMAP*)Load32Resource (PICTURES[MUSIC_PAUSE_PICTURE], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_CENTER, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_CENTER, music_play_pause);

    bmp = (BITMAP*)Load32Resource (PICTURES[MUSIC_NEXT_PICTURE], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_RIGHT, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_RIGHT, music_play_next);

    bmp = (BITMAP*)Load32Resource (PICTURES[MUSIC_PLAY_PICTURE], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    return toolbar;
}
static BOOL on_timerhandler_updatetrkbar(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mTrackBar *trkbar;
    mContainerCtrl *ctnr;
    mWidget *mainwnd;
    PhoneAlbumActivity *act;
    MUSICPLAYERDATAIN *data;
    mTimer *timer;
    int timeVal;
    int maxpos;
    int curpos;
    static int oldpos = 0;

    if (_c((mPanelPiece *)listener)->getOwner((mPanelPiece *)listener) == NULL)
    {
        return FALSE;
    }
    ctnr = (mContainerCtrl *)_c((mWidgetHostPiece*)listener)->getOwner((mWidgetHostPiece*)listener);
    trkbar = (mTrackBar*)_c((mWidgetHostPiece*)listener)->getHosted((mWidgetHostPiece*)listener);
    mainwnd = ncsObjFromHandle(GetParent(ctnr->hwnd));
    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);
    data = act->getPlayerDatain();

    timer = (mTimer*)sender;
    timeVal = _c(timer)->getProperty(timer, NCSP_TIMER_INTERVAL);

    maxpos = _c(trkbar)->getProperty(trkbar, NCSP_TRKBAR_MAXPOS);
    curpos = data->curtime * maxpos / getTime(data->pAlbum[data->album].pSong[data->index].totaltime, timeVal);
    if(curpos == oldpos && curpos != maxpos)
        return TRUE;
    if(curpos != maxpos)
        ExcludeWindowStyle(trkbar->hwnd, NCSS_NOTIFY);
    else    //fix bug 359
        act->m_switch = TRUE;
    _c(trkbar)->setProperty (trkbar, NCSP_TRKBAR_CURPOS, (int)curpos);
    oldpos = curpos;
    if((int)curpos != maxpos)
        IncludeWindowStyle(trkbar->hwnd, NCSS_NOTIFY);

    return TRUE;
}
static BOOL on_timerhandler_updatecurtime(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mWidget *mainwnd;
    PhoneAlbumActivity *act;
    MUSICPLAYERDATAIN *data;
    mTimer *timer;
    int timeVal;
    static char oldstr[TOTALTIME_STRLEN] = {0};

    //get datain
    mainwnd = (mWidget*)listener;
    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);
    data = act->getPlayerDatain();
    data->curtime++;
    timer = (mTimer*)sender;
    timeVal = _c(timer)->getProperty(timer, NCSP_TIMER_INTERVAL);
    time2string(data->pAlbum[data->album].pSong[data->index].totaltime,
            count2Sec(data->curtime, timeVal), data->curtime_str, sizeof(data->curtime_str));

    if(strcmp(data->curtime_str, oldstr) != 0){
        mTextPiece *curtime;

        curtime = (mTextPiece*)act->getObjOfCurTime();
        _c(curtime)->setProperty(curtime, NCSP_LABELPIECE_LABEL, (DWORD)data->curtime_str);
        strncpy(oldstr, data->curtime_str, sizeof(data->curtime_str));
        act->m_isUpdate = TRUE;
    }

    return TRUE;
}
static BOOL on_timerhandler_update(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mWidget *mainwnd;
    PhoneAlbumActivity *act;

    mainwnd = (mWidget*)listener;
    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);
    if(!act->m_isUpdate)
        return TRUE;

#if 0
    {
        mWidget *ctrl_widget;
        RECT rc = {TRKBAR_CUR_TIME_X,ACTIVITY_H - MUSIC_TOOLBAR_H - CTRL_TRKBAR_H - 4,
            TRKBAR_CUR_TIME_W,ACTIVITY_H - MUSIC_TOOLBAR_H};
        ctrl_widget =ncsGetChildObj (mainwnd->hwnd, IDC_NAV);
        InvalidateRect(ctrl_widget->hwnd,&rc,FALSE);
    }
#else
    PanelPiece_invalidatePiece(act->getObjOfCurTime(),NULL);
#endif
    act->m_isUpdate = FALSE;

    return TRUE;
}

static BOOL trkbar_changed_notify(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mTrackBar *trkbar;
    mContainerCtrl *ctnr;
    mWidget *mainwnd;
    PhoneAlbumActivity *act;
    MUSICPLAYERDATAIN *data;
    mTimer *timer;
    int timeVal;
    int maxpos;
    int curpos;
    int totaltime;

    if (_c((mPanelPiece *)sender)->getOwner((mPanelPiece *)sender) == NULL)
    {
        return FALSE;
    }
    //get mainwnd widget
    ctnr = (mContainerCtrl *)_c((mWidgetHostPiece*)sender)->getOwner((mWidgetHostPiece*)sender);
    mainwnd = ncsObjFromHandle(GetParent(ctnr->hwnd));

    //get timer val
    timer = (mTimer*)ncsGetChildObj(mainwnd->hwnd, IDC_ALBUM_TIMER);
    timeVal = _c(timer)->getProperty(timer, NCSP_TIMER_INTERVAL);

    //get trkbar ctrl which hosted
    trkbar = (mTrackBar*)_c((mWidgetHostPiece*)sender)->getHosted((mWidgetHostPiece*)sender);
    maxpos = _c(trkbar)->getProperty(trkbar, NCSP_TRKBAR_MAXPOS);
    curpos = _c(trkbar)->getProperty(trkbar, NCSP_TRKBAR_CURPOS);

    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);
    data = act->getPlayerDatain();
    totaltime = data->pAlbum[data->album].pSong[data->index].totaltime;
    data->curtime = curpos * getTime(totaltime, timeVal) / maxpos;
    time2string(totaltime,count2Sec(data->curtime, timeVal), 
            data->curtime_str, sizeof(data->curtime_str));

    act->m_isUpdate = TRUE;

    return TRUE;
}
static BOOL trkbar_reachmax_notify(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mContainerCtrl *ctnr;
    mWidget *mainwnd;
    PhoneAlbumActivity *act;

    if (_c((mPanelPiece *)sender)->getOwner((mPanelPiece *)sender) == NULL)
        return FALSE;

    ctnr = (mContainerCtrl *)_c((mWidgetHostPiece*)sender)->getOwner((mWidgetHostPiece*)sender);
    mainwnd = ncsObjFromHandle(GetParent(ctnr->hwnd));
    act = (PhoneAlbumActivity*)Activity::getActivityFromHWND(mainwnd->hwnd);

    //fix bug 359
    if(act->m_switch){
        act->onReachMax();
        act->m_switch = FALSE;
    }
    else{
        if(act->isPlay())
            act->onMusicPause();
    }

    return TRUE;
}
static BOOL trkbar_reachmin_notify(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    if (_c((mPanelPiece *)sender)->getOwner((mPanelPiece *)sender) == NULL)
    {
        return FALSE;
    }
    dbg();
    return TRUE;
}
mHotPiece *PhoneAlbumActivity::createSongPlayView(mWidget *ctnr, int isPlay)
{
    mWidget *mainwnd = ncsObjFromHandle(GetParent(ctnr->hwnd));
    mPanelPiece *panel;
    mImagePiece *image;
    mWidgetHostPiece *trkbar;
    mTextPiece *curtime;
    mTextPiece *totaltime;
    HBRUSH brush;
    mShapeTransRoundPiece *trkbar_bk;
    mActivityToolbarPiece *toolbar;
    mTimer *timer;
    mTimer *update;
    RECT rc = {0, 0, ACTIVITY_W, MUSIC_VIEW_H};
    RECT trkbar_rc = {0, 0, CTRL_TRKBAR_W, CTRL_TRKBAR_H};
    RECT image_rc = {0, 0, ACTIVITY_W, MUSIC_VIEW_H - MUSIC_TOOLBAR_H};

#if WIDGETHOST_FROM_DESKTOP
    if (g_trackbarWidget)
    {
        SendMessage(g_trackbarWidget->hwnd,MSG_CLOSE,0,0);
        _c(g_trackbarWidget)->release(g_trackbarWidget);
    }
#endif
    panel = (mPanelPiece*)NEWPIECE(mPanelPiece);
    assert(panel);
    _c(panel)->setRect (panel, &rc);

    toolbar = create_toolbar();
    assert(toolbar);
    _c(panel)->addContent(panel, (mHotPiece *)toolbar, 0, 
            ACTIVITY_H - MUSIC_TOOLBAR_H - NAVIGATIONBAR_H);
    m_toolBar = (mHotPiece *)toolbar;

    image = (mImagePiece*)NEWPIECE(mImagePiece);
    _c(image)->setRect (image, &image_rc);
    _c(image)->setProperty (image,
            NCSP_IMAGEPIECE_DRAWMODE, (DWORD)NCS_DM_SCALED); 
    _c(panel)->addContent(panel, (mHotPiece *)image, 0, 0);
    g_albumImage = image; 

    trkbar_bk = NEWPIECE(mShapeTransRoundPiece);
    SetRect(&rc, 0, 0, MUSIC_BAR_W, MUSIC_BAR_H);
    _c(trkbar_bk)->setRect(trkbar_bk, &rc);
    _c(trkbar_bk)->setProperty (trkbar_bk, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    _c(trkbar_bk)->setProperty (trkbar_bk, NCSP_TRANROUND_CORNERFLAG,0);
    brush = _c(trkbar_bk)->getBrush(trkbar_bk);
    MGPlusSetLinearGradientBrushColorsEx (brush, 
            trkbar_gradient_color, TABLESIZE(trkbar_gradient_color), trkbar_gradient_pos);
    _c(panel)->addContent(panel, (mHotPiece *)trkbar_bk, MUSIC_BAR_X, MUSIC_BAR_Y);

    trkbar = NEWPIECEEX(mWidgetHostPiece,this->hwnd());
    _c(trkbar)->setRect(trkbar, &trkbar_rc);
    _c(trkbar)->setHosted(trkbar, hosted_tmpl);
    _c(panel)->addContent(panel, (mHotPiece*)trkbar, CTRL_TRKBAR_X,CTRL_TRKBAR_Y);
    m_trkbar = _c(trkbar)->getHosted(trkbar);
    _c(m_trkbar)->setProperty(m_trkbar, NCSP_TRKBAR_THUMB_WIDTH, TRKBAR_THUMB_SIZE_W);
    _c(m_trkbar)->setProperty(m_trkbar, NCSP_TRKBAR_THUMB_HEIGHT, TRKBAR_THUMB_SIZE_H);

    SetRect(&rc, 0, 0, TRKBAR_CUR_TIME_W, TRKBAR_CUR_TIME_H);
    curtime = NEWPIECE(mTextPiece);
    _c(curtime)->setRect(curtime, &rc);
    _c(curtime)->setProperty(curtime, NCSP_LABELPIECE_LABEL, (DWORD)"00:00");
    _c(curtime)->setProperty(curtime, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_timeFont);
    _c(curtime)->setProperty(curtime, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)TIMETEXT_COLOR);
    _c(panel)->addContent(panel, (mHotPiece*)curtime, TRKBAR_CUR_TIME_X, TRKBAR_CUR_TIME_Y);
    m_curtime = (mHotPiece*)curtime;
    
    totaltime = NEWPIECE(mTextPiece);
    _c(totaltime)->setRect(totaltime, &rc);
    _c(totaltime)->setProperty(totaltime, NCSP_LABELPIECE_LABEL, (DWORD)"00:00");
    _c(totaltime)->setProperty(totaltime, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_timeFont);
    _c(totaltime)->setProperty(totaltime, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)TIMETEXT_COLOR);
    _c(panel)->addContent(panel, (mHotPiece*)totaltime, TRKBAR_TOTAL_TIME_X, TRKBAR_TOTAL_TIME_Y);
    m_totaltime = (mHotPiece*)totaltime;

    ncsAddEventListener((mObject *)trkbar, (mObject *)curtime, (NCS_CB_ONPIECEEVENT)trkbar_changed_notify, NCSN_TRKBAR_CHANGED);
    ncsAddEventListener((mObject *)trkbar, (mObject *)curtime, (NCS_CB_ONPIECEEVENT)trkbar_reachmax_notify, NCSN_TRKBAR_REACHMAX);
    ncsAddEventListener((mObject *)trkbar, (mObject *)curtime, (NCS_CB_ONPIECEEVENT)trkbar_reachmin_notify, NCSN_TRKBAR_REACHMIN);

    timer = (mTimer*)ncsGetChildObj(mainwnd->hwnd, IDC_ALBUM_TIMER);
    if(timer){
        ncsAddEventListener((mObject *)timer, (mObject *)trkbar, (NCS_CB_ONPIECEEVENT)on_timerhandler_updatetrkbar, MSG_TIMER);
        ncsAddEventListener((mObject *)timer, (mObject *)mainwnd, (NCS_CB_ONPIECEEVENT)on_timerhandler_updatecurtime, MSG_TIMER);
        m_isPlay = isPlay;
        if(isPlay)
            _c(timer)->start(timer);
    }

    update = (mTimer*)ncsGetChildObj(mainwnd->hwnd, IDC_UPDATE_TIMER);
    if(update){
        ncsAddEventListener((mObject*)update, (mObject*)mainwnd, (NCS_CB_ONPIECEEVENT)on_timerhandler_update, MSG_TIMER);
        m_isUpdate = FALSE;
        _c(update)->start(update);
    }

    m_titlePiece = (mHotPiece *)NEWPIECE(mTextPiece);
    SetRect(&rc, 0, 0, TRKBAR_TITLE_W, TRKBAR_TITLE_H);
    _c(m_titlePiece)->setRect(m_titlePiece, &rc);
    _c(m_titlePiece)->setProperty(m_titlePiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_trkbarTitleFont);
    _c(m_titlePiece)->setProperty(m_titlePiece, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)TRKBAR_TITLE_TEXT_COLOR);
    _c(panel)->addContent(panel, (mHotPiece*)m_titlePiece, TRKBAR_TITLE_X,TRKBAR_TITLE_Y);
      
#if WIDGETHOST_FROM_DESKTOP
    g_trackbarWidget = trkbar->hosted;
    _c(g_trackbarWidget)->addRef(g_trackbarWidget);
#endif
    return (mHotPiece *)panel; 
}

void PhoneAlbumActivity::createSongPlayItem(const char *name)
{
    mHotPiece *play_piece;

    mNavigationItem *playitem = getSongPlayNavItem();


    if (playitem != NULL)
    {
        _c(playitem)->setProperty(playitem,NCSP_NAVIGATIONITEM_TITLE, (DWORD)name);
        return;
    }
    play_piece = createSongPlayView(m_ctnr, 1);
    assert(play_piece);


    m_navItems[NAVITEM_SONGPLAY] = ncsCreateNavigationItem(
            (mHotPiece *)play_piece, name, NAVIGATION_STYLE_NORMAL);

    assert(m_navItems[NAVITEM_SONGPLAY]);
    ADDREF(m_navItems[NAVITEM_SONGPLAY]);
}

static void resetString(char *src,char *dst,int len)
{
    int src_len;
    assert(src);
    assert(dst);
    src_len = strlen(src);
    if (src_len <= len)
    {
        strcpy(dst,src);
    }
    else
    {
        int i;
        for (i=len - 3;i>=0;i--)
        {
            if (src[i] == ' ')
            {
                break;
            }
        } 
        if (i > len - 3 || i <= 0)
        {
            strncpy(dst,src,len - 3);
        }
        else
        {
            strncpy(dst,src,i);
        }
        if (strlen(src) > strlen(dst))
            strcat(dst,"...");
    }
    return;
}

void PhoneAlbumActivity::listEnterPlayWin(ALBUMINFO *album,SONGINFO *song)
{
    static char name[MP_TITLE_NAME];

    memset(name,0x00,sizeof(name));

    if (album == NULL)
    {
        if (song == NULL)
        {
            strcpy(name,"Play");
        }
        else
        {
            resetString(song->name,name,MP_TITLE_NAME - 1);
        }
    }
    else
    {
        resetString(album->name,name,MP_TITLE_NAME - 1);
    }
    createSongPlayItem(name);
    assert(NAVITEM_SONGPLAY);

    onMusicPlay();
    _c(m_trkbar)->setProperty (m_trkbar, NCSP_TRKBAR_CURPOS, (int)0);
    resetDisplay();
    {
        ALBUMINFO *currentalbum;
        currentalbum = getFocusAlbum();
        const BITMAP *bitmap = RetrieveRes(currentalbum->photo);
        if (bitmap == NULL)
        {
            bitmap = RetrieveRes(MUSIC_IMG_PATH);
        }
        _c(g_albumImage)->setProperty (g_albumImage,NCSP_IMAGEPIECE_IMAGE, (DWORD)bitmap); 
    }
    _c(m_nav)->push(m_nav,m_navItems[NAVITEM_SONGPLAY]);
}

void PhoneAlbumActivity::resetDisplay(void){
    int totaltime;
    char *tt_str;

    m_playerDatain.curtime = 0;
    totaltime = m_playerDatain.pAlbum[m_playerDatain.album].pSong[m_playerDatain.index].totaltime;
    time2string(totaltime,m_playerDatain.curtime, 
            m_playerDatain.curtime_str, sizeof(m_playerDatain.curtime_str));
    tt_str = m_playerDatain.pAlbum[m_playerDatain.album].pSong[m_playerDatain.index].totaltime_str;

    _c((mTextPiece*)m_curtime)->setProperty((mTextPiece*)m_curtime, NCSP_LABELPIECE_LABEL, (DWORD)m_playerDatain.curtime_str);
    _c((mTextPiece*)m_totaltime)->setProperty((mTextPiece*)m_totaltime, NCSP_LABELPIECE_LABEL, (DWORD)tt_str);
    trkbar_title_change(this);
    UpdateWindow(this->hwnd(),FALSE);
}
inline int time2string (int totaltime,int second, char *strbuf, int bufsize)
{
    int hour, min, sec;
    memset (strbuf, 0, bufsize);

    if (second > totaltime)
        second = totaltime;
    hour = second/3600; /* hour */

    min = second%3600;
    sec = min;
    min = min/60; /* min */

    sec = sec%60; /* second */

    assert (min < 60 && sec < 60);

    if (hour) {
        if (bufsize < (int)sizeof("00:00:00")) {
            printf ("Buf is too small to save time string !\n");
            return -1;
        }
        sprintf (strbuf, "%02d:%02d:%02d", hour, min, sec);
    } else {
        if (bufsize < (int)sizeof("00:00")) {
            printf ("Buf is too small to save time string !\n");
            return -1;
        }
        sprintf (strbuf, "%02d:%02d", min, sec);
    }
    return 0;
} 

int PhoneAlbumActivity::loadAblumInfoFromLib ()
{
    int i,j;
    MediaStoreService *music_server;
    ContentValues::Strings album_names;
    ContentValues::Strings songs_at_album;
    ALBUMINFO *albums;

    music_server = GET_MEDIASTORE_SERVICE();
    album_names = music_server->getAllAlbumName (); 
    albums = (ALBUMINFO*)calloc (album_names.size(), sizeof(ALBUMINFO));
    memset (albums, 0, album_names.size()*sizeof(ALBUMINFO));
    m_playerDatain.n = album_names.size();
    m_playerDatain.pAlbum = albums;
    m_playerDatain.index = 0;
    m_playerDatain.album = 0;
    m_playerDatain.curtime = 0;
    time2string(0,m_playerDatain.curtime, 
            m_playerDatain.curtime_str, sizeof(m_playerDatain.curtime_str));

    for (i=0; i<(int)album_names.size(); ++i) { /* get album info */
        ALBUMINFO *cur_album;
        std::string author;
        std::string logoname;

        cur_album = &albums[i];

        strncpy (cur_album->name, album_names[i].c_str(), sizeof(cur_album->name)-1); /* album name */
        logoname = music_server->getAlbumLogo (album_names[i]);
        strncpy (cur_album->photo, logoname.c_str(), sizeof(cur_album->photo)-1); /* logo name */
        author = music_server->getAlbumAuthor (album_names[i]);
        strncpy (cur_album->author, author.c_str(), sizeof(cur_album->author)-1); /* author */

        songs_at_album = music_server->getSongsByAlbum (album_names[i]); /* get songs */
        cur_album->pSong = (SONGINFO*)calloc (songs_at_album.size(), sizeof(SONGINFO));
        memset (cur_album->pSong, 0, songs_at_album.size()*sizeof(SONGINFO));
        cur_album->n = songs_at_album.size();

        for (j=0; j<(int)songs_at_album.size(); ++j) { /* get song info */
            SONGINFO *cursong;

            cursong = &cur_album->pSong[j];

            cursong->totaltime = music_server->getSongDuration(songs_at_album[j]); /* total time */
            time2string (cursong->totaltime,
                    cursong->totaltime, cursong->totaltime_str, sizeof(cursong->totaltime_str)); /* total time string */
            strncpy (cursong->name, songs_at_album[j].c_str(), sizeof (cursong->name)-1); /* song name */
            strncpy (cursong->author, cur_album->author, sizeof (cursong->author)-1); /* author */
        }
    }
    if (m_playerDatain.n > 0)
    {
        m_focusAlbum = m_playerDatain.pAlbum[0];
    }

    return 0;
}

int PhoneAlbumActivity::unloadAblumInfo()
{
    int i;

    for (i=0; i<m_playerDatain.n; ++i) {
        free (m_playerDatain.pAlbum[i].pSong);
    }

    free (m_playerDatain.pAlbum);
    m_playerDatain.pAlbum = NULL;
    
    return 0;
}

int PhoneAlbumActivity::deinitAlbumActivity()
{
    UNREFPIECE(m_navViews[NAVITEM_ALBUMLIST]);
    UNREFPIECE(m_navViews[NAVITEM_SONGLIST]);
    if (m_navItems[NAVITEM_SONGPLAY] != NULL)
    {
        UNREF(m_navItems[NAVITEM_SONGPLAY]);
    }
    ncsDestroyNavigationItem(m_navItems[NAVITEM_ALBUMLIST]);
    ncsDestroyNavigationItem(m_navItems[NAVITEM_SONGLIST]);
    ncsDestroyNavigationItem(m_navItems[NAVITEM_SONGPLAY]);

    return 0;
}

int PhoneAlbumActivity::initAlbumActivity(HWND ctrl_hwnd,BOOL startplay)
{
    loadAblumInfoFromLib ();
    loadResource ();
    createSongPlayItem("");
    if (startplay)
    {
        createNavigation (ctrl_hwnd,NAVITEM_SONGPLAY);
    }
    else
    {
        createNavigation (ctrl_hwnd);
    }

    return 0;
}

int PhoneAlbumActivity::updateSongListViewData ()
{
    mSongTableViewPiece *song_list_view = (mSongTableViewPiece*) m_navViews[NAVITEM_SONGLIST];

    _c(song_list_view)->moveViewport (song_list_view, 0, 0);
    _c(song_list_view)->reloadData (song_list_view);

    return 0;
}

void PhoneAlbumActivity::setCtrl(mWidget *ctrl){
    m_ctnr = ctrl;
}

mButtonPanelPiece *PhoneAlbumActivity::createNavRightButton (
        mNavigationItem *self,const char *name, NCS_CB_ONPIECEEVENT cb)
{
    mButtonPanelPiece *playall_btn;
    RECT rc = {0, 0, PLAYALL_BTN_W, NAVIGATIONBAR_DEFAULT_BUTTON_H};

    playall_btn = (mButtonPanelPiece*)_c(self)->createDefaultButton(self, name,(DWORD)this);
    _c(playall_btn)->setRect (playall_btn, &rc);

    ncsAddEventListener((mObject*)playall_btn, (mObject*)playall_btn, cb, NCSN_ABP_CLICKED);
    return playall_btn;
}

int PhoneAlbumActivity::onReachMax()
{
    mWidget *dialogbox;
    mTimer *timer;
    int index;

    dialogbox = ncsObjFromHandle(m_hwnd);
    timer = (mTimer *)ncsGetChildObj(dialogbox->hwnd, IDC_ALBUM_TIMER);
    _c(timer)->stop(timer);
    switch(m_playerDatain.mode)
    {
        case MP_M_SINGLE:
            onMusicPause();
            /*
            _c(m_trkbar)->setProperty (m_trkbar, NCSP_TRKBAR_CURPOS, (int)0);
            resetDisplay();
            */
            return 0;

        case MP_M_ALBUM:
        #if 1
            index = m_playerDatain.index;
            if(index + 1 >= m_playerDatain.pAlbum[m_playerDatain.album].n){
                index = 0;
                m_playerDatain.index = 0;
                //onMusicPause();
                //return 0;
            } else
                m_playerDatain.index++;
            break;
        #else
            return 0;
        #endif

        case MP_M_ALBUM_CYCLE:
            index = m_playerDatain.index;
            if(index + 1 >= m_playerDatain.pAlbum[m_playerDatain.album].n){
                m_playerDatain.index = 0;
                if (m_playerDatain.album >= m_playerDatain.n - 1)
                {
                    m_playerDatain.album = 0;
                }
                else
                {
                    m_playerDatain.album ++;
                }
                setFocusAlbum(m_playerDatain.pAlbum[m_playerDatain.album]);
            } else
                m_playerDatain.index++;
            break;

        case MP_M_SINGLE_CYCLE:
        default:
                break;
    }
    resetDisplay();
    if(!isPlay())
    {
        _c(m_trkbar)->setProperty (m_trkbar, NCSP_TRKBAR_CURPOS, (int)0);
        return 0;
    }
    _c(timer)->start(timer);
    return 0;
}

void PhoneAlbumActivity::onMusicPause(void)
{
    mTimer *timer;
    BITMAP* bmp = (PBITMAP)RetrieveRes(PICTURES[MUSIC_PLAY_PICTURE]);

    m_isPlay = FALSE;
    timer = (mTimer *)ncsGetChildObj(m_hwnd, IDC_ALBUM_TIMER);
    _c(timer)->stop(timer);

    _c((mActivityToolbarPiece *)m_toolBar)->changeButtonImage (
            (mActivityToolbarPiece *)m_toolBar, ACTIVITYTOOLBAR_CENTER, bmp);
}

void PhoneAlbumActivity::onMusicPlay(void)
{
    mTimer *timer;
    BITMAP* bmp = (PBITMAP)RetrieveRes(PICTURES[MUSIC_PAUSE_PICTURE]);
    int curpos, maxpos;

    m_isPlay = TRUE;

    //fix bug 359
    curpos = _c(m_trkbar)->getProperty (m_trkbar, NCSP_TRKBAR_CURPOS);
    maxpos = _c(m_trkbar)->getProperty (m_trkbar, NCSP_TRKBAR_MAXPOS);
    if(curpos == maxpos)
        onMusicNext();

    timer = (mTimer *)ncsGetChildObj(m_hwnd, IDC_ALBUM_TIMER);
    _c(timer)->start(timer);
    _c((mActivityToolbarPiece *)m_toolBar)->changeButtonImage (
            (mActivityToolbarPiece *)m_toolBar, ACTIVITYTOOLBAR_CENTER, bmp);
}

void PhoneAlbumActivity::onMusicPrev(void)
{
    mTimer *timer;
    int index;
    int album;

    timer = (mTimer *)ncsGetChildObj(m_hwnd, IDC_ALBUM_TIMER);
    if(isPlay())
        _c(timer)->stop(timer);

    index = m_playerDatain.index;
    album = m_playerDatain.album;

    if(index <= 0){
        if(album > 0){
            album--;
            index = m_playerDatain.pAlbum[album].n - 1;
        }
        else{
            album = m_playerDatain.n - 1;
            index = m_playerDatain.pAlbum[album].n -1;
        }
    }
    else
        index--;

    m_playerDatain.index = index;
    m_playerDatain.album = album;

    resetDisplay();
    _c(m_trkbar)->setProperty (m_trkbar, NCSP_TRKBAR_CURPOS, (int)0);

    if(isPlay())
        _c(timer)->start(timer);
}

void PhoneAlbumActivity::onMusicNext(void)
{
    mTimer *timer;
    int index;
    int album;

    timer = (mTimer *)ncsGetChildObj(m_hwnd, IDC_ALBUM_TIMER);
    if(isPlay())
        _c(timer)->stop(timer);

    index = m_playerDatain.index;
    album = m_playerDatain.album;

    if(index + 1 >= m_playerDatain.pAlbum[album].n){
        album = (album + 1 >= m_playerDatain.n) ? (0) : (album + 1);
        index = 0;
    }
    else
        index++;

    m_playerDatain.index = index;
    m_playerDatain.album = album;

    resetDisplay();
    _c(m_trkbar)->setProperty (m_trkbar, NCSP_TRKBAR_CURPOS, (int)0);

    if(isPlay())
        _c(timer)->start(timer);
}

static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData)
{
/*    mWidget *ctrl_widget;
    PhoneAlbumActivity* activity = 
        (PhoneAlbumActivity*)Activity::getActivityFromHWND (self->hwnd);

    ctrl_widget =ncsGetChildObj (self->hwnd, IDC_NAV);
    activity->setCtrl(ctrl_widget);
    activity->initAlbumActivity (ctrl_widget->hwnd);
*/
    return TRUE;
}

static BOOL playall_btn_song_event_handler(mObject *self, mObject *piece,int event_id, DWORD param)
{
    PhoneAlbumActivity* act = (PhoneAlbumActivity*)param;
    ALBUMINFO *currentalbum;
    MUSICPLAYERDATAIN *player_datain;
    assert(act);
    currentalbum = act->getFocusAlbum();
    player_datain = act->getPlayerDatain();
    player_datain->index = 0;
    player_datain->mode = PhoneAlbumActivity::MP_M_ALBUM;
    act->listEnterPlayWin(currentalbum,NULL);
    return TRUE;
}
static BOOL playall_btn_album_event_handler(mObject *self, mObject *piece,int event_id, DWORD param)
{
    PhoneAlbumActivity* act = (PhoneAlbumActivity*)param;
    ALBUMINFO *currentalbum;
    MUSICPLAYERDATAIN *player_datain;
    assert(act);
    player_datain = act->getPlayerDatain();
    if (player_datain->n <= 0)
        return TRUE;
    player_datain->album = 0;
    player_datain->index = 0;
    player_datain->mode = PhoneAlbumActivity::MP_M_ALBUM_CYCLE;
    act->setFocusAlbum(player_datain->pAlbum[player_datain->album]);
    currentalbum = act->getFocusAlbum();
    act->listEnterPlayWin(currentalbum,NULL);
    return TRUE;
}

static BOOL main_onClose(mWidget* self, int message)
{
    fprintf(stderr, "%s:%d.\n", __FUNCTION__, __LINE__);
    return TRUE;
}
static BOOL main_onDestroy(mWidget* self, int message)
{
    PhoneAlbumActivity* activity = 
        (PhoneAlbumActivity*)Activity::getActivityFromHWND (self->hwnd);

    fprintf(stderr, "%s:%d.\n", __FUNCTION__, __LINE__);
    activity->deinitAlbumActivity();

    return TRUE;
}

