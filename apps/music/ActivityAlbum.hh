
#ifndef _ACTIVITYALBUM_HH
#define _ACTIVITYALBUM_HH

#include "mpanelpiece.h"
#include "mnavigationitem.h"
#include "mnavigationpanelpiece.h"
#include "mnavigationbarpiece.h"
#include "NCSActivity.hh"

#define DEFAULT_ALBUM_PHOTO  "res/musicplayer/Album/default.png"
#define MP_STR_MAX  256
#define MP_TITLE_NAME  (ACTIVITY_W >=360?28:20)

#define MUSIC_TOOLBAR_H 49
#define TOTALTIME_STRLEN    12

#define IDC_NAV 200
#define IDC_NEWTRKBAR   201
#define IDC_ALBUM_TIMER 202
#define IDC_UPDATE_TIMER 203

#define MUSIC_VIEW_H  (ACTIVITY_H-NAVIGATIONBAR_H)
#define PLAYALL_BTN_COLOR 0xff192952 

#define PLAYALL_BTN_W 60
#define PLAYALL_BTN_TEXT_SIZE 16

#define MUSIC_BAR_X   0
#define MUSIC_BAR_H   60 
#define MUSIC_BAR_Y   (MUSIC_VIEW_H - MUSIC_BAR_H - MUSIC_TOOLBAR_H)
#define MUSIC_BAR_W   ACTIVITY_W

#define TRKBAR_TITLE_X  0
#define TRKBAR_TITLE_Y  MUSIC_BAR_Y
#define TRKBAR_TITLE_W  MUSIC_BAR_W  
#define TRKBAR_TITLE_H  36

#define CTRL_TRKBAR_X   48
#define CTRL_TRKBAR_Y   (MUSIC_BAR_Y + TRKBAR_TITLE_H) 
#define CTRL_TRKBAR_W   (MUSIC_BAR_W - (CTRL_TRKBAR_X << 1)) 
#define CTRL_TRKBAR_H   17
#define TRKBAR_THUMB_SIZE_W 16
#define TRKBAR_THUMB_SIZE_H TRKBAR_THUMB_SIZE_W 

#define TRKBAR_CUR_TIME_X  0
#define TRKBAR_CUR_TIME_Y  (CTRL_TRKBAR_Y - 2)
#define TRKBAR_CUR_TIME_W  CTRL_TRKBAR_X
#define TRKBAR_CUR_TIME_H  20

#define TRKBAR_TOTAL_TIME_X  (TRKBAR_CUR_TIME_W + CTRL_TRKBAR_W)
#define TRKBAR_TOTAL_TIME_Y  TRKBAR_CUR_TIME_Y
#define TRKBAR_TOTAL_TIME_W  TRKBAR_CUR_TIME_W
#define TRKBAR_TOTAL_TIME_H  TRKBAR_CUR_TIME_H

#define TRKBAR_TITLE_TEXT_SIZE 12
#define TRKBAR_TITLE_TEXT_COLOR 0xffe3c790

#define getTime(time, timer_interval)       ((time)*100/(timer_interval))
#define count2Sec(time, timer_interval)     ((time) * (timer_interval) / 100)
#define sec2count(sec, timer_interval)      ((sec) * 100 / (timer_interval))

typedef struct _tagsonginfo{
    int curtime;
    int totaltime;
    char totaltime_str[TOTALTIME_STRLEN];
    char name[MP_STR_MAX];
    char author[MP_STR_MAX];
}SONGINFO, *PSONGINFO;

typedef struct _tagalbuminfo{
    PSONGINFO pSong;
    char name[MP_STR_MAX];
    char author[MP_STR_MAX];
    char photo[MP_STR_MAX];
    int n;
}ALBUMINFO, *PALBUMINFO;

typedef struct _tagdatain{
    PALBUMINFO pAlbum;
    int album;
    int index;
    int mode;
    int n;
    int curtime;
    char curtime_str[TOTALTIME_STRLEN];
}MUSICPLAYERDATAIN, *PMUSICPLAYERDATAIN;

class PhoneAlbumActivity : public NCSActivity {

    public:
        enum mode{
            MP_M_SINGLE,
            MP_M_SINGLE_CYCLE,
            MP_M_ALBUM,
            MP_M_ALBUM_CYCLE,
        };
        PhoneAlbumActivity();
        ~PhoneAlbumActivity();

        int initAlbumActivity(HWND,BOOL);
        int deinitAlbumActivity();
        int updateSongListViewData ();

        mNavigationPanelPiece *getNavigationController(){return m_nav;}
        mNavigationItem *getAlbumListNavItem () { return m_navItems[NAVITEM_ALBUMLIST]; }
        mNavigationItem *getSongListNavItem () { return m_navItems[NAVITEM_SONGLIST]; }
        mNavigationItem *getSongPlayNavItem () { return m_navItems[NAVITEM_SONGPLAY]; }

        MUSICPLAYERDATAIN *getPlayerDatain () { return &m_playerDatain; }
        void listEnterPlayWin(ALBUMINFO *album,SONGINFO *song);

        void setFocusAlbum (ALBUMINFO album) { m_focusAlbum = album; }
        ALBUMINFO *getFocusAlbum (void) { return &m_focusAlbum; }
        mHotPiece *getObjOfCurTime(void){ return m_curtime;}
        mHotPiece *getObjOfTotalTime(void){return m_totaltime;}

        void setCtrl(mWidget *);

        int onReachMax();

        BOOL isPlay(void){ return m_isPlay; }
        void onMusicPause();
        void onMusicPlay();
        void onMusicNext();
        void onMusicPrev();

    protected:
        int onBack();

    private:
        void loadResource(void);
        void unloadResource(void);
        /* create view */
        int createNavigation (HWND ctrl_hwnd, int root_item = NAVITEM_ALBUMLIST);
        mHotPiece *createAlbumListView();
        mHotPiece *createSongListView();
        void createSongPlayItem(const char *name);
        mHotPiece *createSongPlayView(mWidget *, int);
        int loadAblumInfoFromLib ();
        int unloadAblumInfo ();
        mButtonPanelPiece *createNavRightButton (
                mNavigationItem *self,const char *name, NCS_CB_ONPIECEEVENT cb);
    public:
        mHotPiece *m_titlePiece;
        BOOL m_isUpdate;
        BOOL m_switch;

        void resetDisplay();

    private:
        enum {
            NAVITEM_ALBUMLIST,
            NAVITEM_SONGLIST,
            NAVITEM_SONGPLAY,
            NAVITEM_MAX,
        }NAVITEM;

        mNavigationPanelPiece *m_nav;
        mNavigationItem *m_navItems [NAVITEM_MAX];
        mHotPiece *m_navViews[NAVITEM_MAX];
        mWidget *m_ctnr;
        mWidget *m_trkbar;

        MUSICPLAYERDATAIN m_playerDatain;
        ALBUMINFO m_focusAlbum;
        PLOGFONT m_buttonFont;
        PLOGFONT m_trkbarTitleFont;
        PLOGFONT m_timeFont;

        mHotPiece *m_curtime;
        mHotPiece *m_totaltime;
        mHotPiece *m_toolBar;
        
        BOOL m_isPlay;
#if 0
        int m_curtime;      //unit: 1 timer
        int m_totaltime;    //unit: 1 timer
        int m_album;
        int m_index;
        int m_mode;
#endif
};

#endif /* _ACTIVITYALBUM_HH */
