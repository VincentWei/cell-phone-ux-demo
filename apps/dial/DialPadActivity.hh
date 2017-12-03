#ifndef _DIALPAD_ACTIVITY_HH_
#define _DIALPAD_ACTIVITY_HH_
#include "NCSActivity.hh"
#include "global.h"

#define IDC_DIAL_TITLE 100
#define IDC_DIAL_BODY 101
#define IDC_DIAL_TOOLBAR 102

#define DIAL_TITLE_W ACTIVITY_W
#define DIAL_TITLE_H 73

#define DIAL_TOOLBAR_NUM 3
#define DIAL_TOOLBAR_H 49
#define DIAL_TOOLBAR_SPACE_W 16
#define DIAL_TOOLBAR_SPACE_H 4
#define DIAL_TOOLBAR_W ((ACTIVITY_W - (DIAL_TOOLBAR_SPACE_W<<1) - \
            (DIAL_TOOLBAR_NUM - 1) * DIAL_TOOLBAR_SPACE_W)/DIAL_TOOLBAR_NUM)
#define DIAL_TOOLBAR_X(x) (DIAL_TOOLBAR_W*(x) + DIAL_TOOLBAR_SPACE_W * (x+1))
#define DIAL_TOOLBAR_Y (ACTIVITY_H - DIAL_TOOLBAR_H)

#define DIAL_PAD_X 10
#define DIAL_PAD_Y 10

#define DIAL_SPACE_W 16
#define DIAL_SPACE_H 6

#define DIAL_BTN_X_NUM 3
#define DIAL_BTN_Y_NUM 5

#define DIAL_BTN_W ((ACTIVITY_W - (DIAL_PAD_X<<1) - \
        (DIAL_BTN_X_NUM - 1)*DIAL_SPACE_W)/DIAL_BTN_X_NUM)
#define DIAL_BTN_H ((ACTIVITY_H - DIAL_TITLE_H - DIAL_TOOLBAR_H - (DIAL_PAD_Y<<1) \
        - (DIAL_BTN_Y_NUM - 1)*DIAL_SPACE_H)/DIAL_BTN_Y_NUM) 

#define DIAL_TITLE_FONT_H 42
#define DIAL_PADMAIN_FONT_H 36
#define DIAL_PADSUB_FONT_H 15
#define DIAL_PADSTAR_FONT_H 72

#define DIAL_TITLE_TEXT_COLOR 0xff2b3d01  
#define DIAL_TITLE_TEXT_SHADOW_COLOR 0xc0ffffff

#define DIAL_MAINFONT_COLOR 0xff000000
#define DIAL_MAINFONT_SHADOWCOLOR 0xc0ffffff
#define DIAL_SUBFONT_COLOR 0xff646464 

#define DIAL_BUTTON_RADIUS 4

#define DIAL_FIRST_LEN_MAX 15
#define DIAL_SECOND_LEN_MAX 24
#define DIAL_NUMBER_LEN_MAX 28
#define DIALPAD_BTN_NUM (DIAL_BTN_X_NUM * DIAL_BTN_Y_NUM)
class DialPadActivity : public NCSActivity {
public:
    DialPadActivity();
    ~DialPadActivity();

    virtual mContainerCtrl* titleCreate(HWND main_hwnd,RECT rect);
    virtual mContainerCtrl* dialPadCreate(HWND main_hwnd,RECT rect);
    virtual mContainerCtrl* toolBarCreate(HWND main_hwnd,RECT rect);

    void titleNumDisplay(HWND hwnd,const char *instr,char *outstr);
    void numberBtnRotate(mPanelPiece body,mButtonPanelPiece* piece,int duration, EffMotionType type);

    virtual void initResource(void);
    virtual void releaseResource(void);
    virtual int onResume();
    virtual int onPause();

    int getDialBtnIndexByPiece(mButtonPanelPiece* piece);

public:
    Intent *m_intent;

private:
    PLOGFONT m_titleBigFont;
    PLOGFONT m_titleSmallFont;
    PLOGFONT m_padMainFont;
    PLOGFONT m_padSubFont;
    PLOGFONT m_padStarFont;

    mAnimationEditPiece* m_editPiece;
    mButtonPanelPiece* m_dialBtnPiece[DIALPAD_BTN_NUM];
};
#endif/*end*/
