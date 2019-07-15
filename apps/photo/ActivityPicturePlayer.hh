///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _ACTIVITYPICTUREPLAYER_H_
#define _ACTIVITYPICTUREPLAYER_H_

#include "NCSActivity.hh"

//dialogbox
#define PP_X 0
//#define PP_Y ACTIVITY_Y 
#define PP_Y 0 
#define PP_W SCREEN_W
//#define PP_H (SCREEN_H-ACTIVIT_Y)
#define PP_H SCREEN_H

//cc
#define PP_CC_X PP_X
#define PP_CC_Y 0
#define PP_CC_W PP_W
#define PP_CC_H PP_H

//title
#define PP_TITLE_X PP_CC_X
#define PP_TITLE_Y PP_CC_Y
#define PP_TITLE_W PP_CC_W
#define PP_TITLE_H 38

#define PP_EXIT_X (PP_TITLE_X+10)
#define PP_EXIT_W 70
#define PP_EXIT_H 27
#define PP_EXIT_Y ((PP_TITLE_H-PP_EXIT_H)>>1)

#define PP_DEL_X   (PP_TITLE_W-PP_EXIT_X-PP_EXIT_W)
#define PP_DEL_Y   PP_EXIT_Y
#define PP_DEL_W   PP_EXIT_W
#define PP_DEL_H   PP_EXIT_H

//ctrl
#define PP_BTN_NUM 3
#define PP_CTRL_X PP_CC_X
#define PP_CTRL_H 49
#define PP_CTRL_Y (PP_H-PP_CTRL_H)
#define PP_CTRL_W PP_CC_W

#define PP_BTN_W (PP_W/PP_BTN_NUM + (PP_BTN_NUM - (PP_W/PP_BTN_NUM)%PP_BTN_NUM))
#define PP_BTN_H PP_CTRL_H

//image
#define PP_IMAGE_X 0
#define PP_IMAGE_Y 0
#define PP_IMAGE_W PP_W
#define PP_IMAGE_H PP_H

#define PP_TITLE_TEXT_COLOR 0xff192952 
#define PP_TITLE_TEXT_SHADOW_COLOR 0x66ffffff 
#define PP_TITLE_TEXT_FONT_H 20 
#define PP_PLAY_TEXT_FONT_H 16
class ActivityPicturePlayer: public NCSActivity{
public:
    typedef enum{
        MOVE_NO,
        MOVE_LEFT,
        MOVE_RIGHT,
    }MOVEDIRECTION;
    ActivityPicturePlayer();
    ~ActivityPicturePlayer();

    int indexInc();
    int indexDec();
    int getIndex();
    void setIndex(int);

    BOOL isFull();
    void setFull(BOOL);

    mHotPiece *getTitle();
    void setTitle(mHotPiece*);
    
    mHotPiece *getCtrl();
    void setCtrl(mHotPiece*);

    mHotPiece *getImage();
    void setImage(mHotPiece *);

    mHotPiece *getExitBk();
    void setExitBk(mHotPiece*);
    mHotPiece *getDelBk();
    void setDelBk(mHotPiece*);
    mHotPiece *getNextBk();
    void setNextBk(mHotPiece*);
    mHotPiece *getPlayBk();
    void setPlayBk(mHotPiece*);
    mHotPiece *getPrevBk();
    void setPrevBk(mHotPiece*);

    BITMAP *getPic();
    void setPic(BITMAP *);

    void setIsDown(BOOL);
    BOOL getIsDown();

    void setPoint(int, int);
    POINT *getPoint();

    void setDirection(MOVEDIRECTION);
    MOVEDIRECTION getDirection();

    BITMAP *getNextPic();
    BITMAP *getPlayPic();
    BITMAP *getPrevPic();
    PLOGFONT getTitleFont()
    {
        return m_titleFont;
    }
    PLOGFONT getPlayFont()
    {
        return m_playFont;
    }
    virtual int onBack();
    virtual int onHome();
public:
    BOOL m_isExiting;
    mPanelPiece* mBody;

private:
    void initRes();
    int loadRes();
    int releaseRes();

    BOOL mIsFull;
    BOOL mIsDown;    //mouse is donw
    int mIndex;
    POINT mPoint;
    MOVEDIRECTION mDirection;     //mouse move direction

    mHotPiece *mTitle;
    mHotPiece *mCtrl;
    mHotPiece *mImage;
    mHotPiece *mDelBk;

    BITMAP *mPic;
    BITMAP *mNextPic;
    BITMAP *mPlayPic;
    BITMAP *mPrevPic;
    HDC mShdc1;
    HDC mShdc2;

    PLOGFONT m_titleFont;
    PLOGFONT m_playFont;
};

#endif
