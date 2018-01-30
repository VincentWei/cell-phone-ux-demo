
#ifndef  ACTIVITYMP_INC
#define  ACTIVITYMUSICPALYER_INC

#include "NCSActivity.hh"
#include "MusicPlayerToolbar.hh"

//dialogbox
#define MP_X ACTIVITY_X
#define MP_Y ACTIVITY_Y
#define MP_W ACTIVITY_W
#define MP_H ACTIVITY_H

#define MP_CONT_X  95
#define MP_CONT_Y  95
#define MP_CONT_W  170
#define MP_CONT_H  MP_CONT_W

#define MP_INFO_X 0
#define MP_INFO_Y 363
#define MP_INFO_W MP_W
#define MP_INFO_H 22

//title
#define MP_TITLE_X 0
#define MP_TITLE_Y 0
#define MP_TITLE_W MP_W
#define MP_TITLE_H 39

#define MP_IMAGE_X MP_X
#define MP_IMAGE_Y (MP_TITLE_Y+MP_TITLE_H)
#define MP_IMAGE_W MP_W
#define MP_IMAGE_H 371

//statusbar button
#define MP_BUTTON_W  120
#define MP_BUTTON_H  57
#define MP_BUTTON1_X 0
#define MP_BUTTON1_Y 423
#define MP_BUTTON2_X 120
#define MP_BUTTON2_Y MP_BUTTON1_Y
#define MP_BUTTON3_X 240
#define MP_BUTTON3_Y MP_BUTTON1_Y

//control
#define MP_CTRL_RECT_X 0
#define MP_CTRL_RECT_Y 385
#define MP_CTRL_RECT_W MP_W
#define MP_CTRL_RECT_H 38

#define MP_CTRL_SEPA_X 0
#define MP_CTRL_SEPA_Y MP_CTRL_RECT_Y
#define MP_CTRL_SEPA_W MP_W
#define MP_CTRL_SEPA_H 1

#define MP_CTRL_TB_X 41
#define MP_CTRL_TB_Y 395
#define MP_CTRL_TB_W 280
#define MP_CTRL_TB_H 17 

#define MP_CTRL_CURTIME_X 0
#define MP_CTRL_CURTIME_Y 398
#define MP_CTRL_CURTIME_W 40
#define MP_CTRL_CURTIME_H 12

#define MP_CTRL_TOTALTIME_X 320
#define MP_CTRL_TOTALTIME_Y MP_CTRL_CURTIME_Y
#define MP_CTRL_TOTALTIME_W MP_CTRL_CURTIME_W
#define MP_CTRL_TOTALTIME_H MP_CTRL_CURTIME_H

#define getMinute(time, timer_interval)     ((time)*(timer_interval)/(100*60))
#define getSecond(time, timer_interval)     ((time)*(timer_interval)/100%60)
#define getTime(time, timer_interval)       ((time)*100/(timer_interval))

class ActivityMusicPlayer : public NCSActivity 
{
public:
    enum
    {
        MP_NUM = 15,
        BUTTON_COUNT = 4,
        VALUE_LEN = 256,
    };
    enum mode{
        MP_M_SINGLE,
        MP_M_SINGLE_CYCLE,
        MP_M_ALBUM,
        MP_M_ALBUM_CYCLE,
    };

    ActivityMusicPlayer();
    ~ActivityMusicPlayer();

    int  getCurTime();
    void setCurTime(int);

    int  curTimeInc();
    int  curTimeDec();

    int  getTotalTime();
    void setTotalTime(int);

    int  getIndex();
    void setIndex(int);
    int  indexInc();
    int  indexDec();

    int getAlbum();
    void setAlbum(int);
    int albumInc();
    int albumDec();

    int getMode();

    int onPrev();
    int onPause();
    int onNext();
    int onMusicStart();

    int onReachMax();

    int onCommand(const char *cmd)
    {
        if (std::string(cmd) == "prev") {
            return onPrev();
        }
        else if(std::string(cmd) == "pause") {
            return onPause();
        }
        else if(std::string(cmd) == "next"){
            return onNext();
        }
        else if(std::string(cmd) == "start"){
            return onMusicStart();
        }
        else {
            fprintf(stderr, "Unkonw command.\n");
            return -1;
        }
    }

    virtual int onTitleCommand(const char *cmd)
    {
        printf("enter the activityMusicPlayer onTitleCommand\n");
        return 0;
    }

    int loadData(mWidget *);


    //---------
    void initNavbar(mWidget* self);

    void loadMyResource();
    void unloadMyResource();

    MusicPlayerToolbar* m_toolbar;
    mHotPiece* m_navbarTitlePiece;

    // font
    PLOGFONT m_navbarBtnFont;
    PLOGFONT m_navbarTitleFont;
    PLOGFONT m_trkbarTitleFont;
    PLOGFONT m_font12;
    PBITMAP m_bmpCD;

private:
    int m_curtime;      //unit: 1 timer
    int m_totaltime;    //unit: 1 timer
    int m_album;
    int m_index;
    int m_mode;
};

#endif
