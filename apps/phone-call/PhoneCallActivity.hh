#ifndef _PHONECALL_ACTIVITY_HH_
#define _PHONECALL_ACTIVITY_HH_
#include "NCSActivity.hh"
#include "global.h"

// new start
#define ID_ANIMATION_EDIT   100
#define ID_BTN_ENDCALL      101
#define ID_BTN_HIDEKEYPAD   102
#define ID_CTRN_FUNC        103
#define ID_CTRN_KEYPAD      104
#define ID_CTRN_ANIMATION   105
#define IDC_KEYPAD          133

#define SPACE_W 10
#define SPACE_H 10

#define CALL_TITLE_W SCREEN_W
#define CALL_TITLE_H 74

#define CALL_END_CALL_X 10
#define CALL_END_CALL_H 48
#define CALL_END_CALL_W (SCREEN_W - (CALL_END_CALL_X<<1))

#define CALL_END_CALL_Y (ACTIVITY_H - CALL_END_CALL_H - 3)

#define CALL_HIDEPAD_CALL_H CALL_END_CALL_H
#define CALL_HIDEPAD_CALL_W (SCREEN_W / 2 - 20)

#define CALL_HIDEPAD_CALL_X (ACTIVITY_W + 10)
#define CALL_HIDEPAD_CALL_Y (ACTIVITY_H - CALL_HIDEPAD_CALL_H - 3)

// keyfunc
#define N_FUNC_BTN_NUM 3
#define CALL_AREA_W (SCREEN_W-(SPACE_W<<1) + \
        (N_FUNC_BTN_NUM - ((SCREEN_W-(SPACE_W<<1))%N_FUNC_BTN_NUM)))
#define CALL_INFO_H 26
#define CALL_AREA_H (ACTIVITY_H-CALL_TITLE_H-(SPACE_H<<1)-CALL_END_CALL_H)

#define N_FUNC_AREA_X  SPACE_W
#define N_FUNC_AREA_Y  SPACE_H+CALL_TITLE_H

// keypad
#define N_FUNC_BTN_X_NUM 3
#define N_FUNC_BTN_Y_NUM 4

#define KEYPAD_BTN_H  (((CALL_AREA_H - CALL_INFO_H) - VSEP_H * N_FUNC_BTN_Y_NUM) / N_FUNC_BTN_Y_NUM)
#define KEYPAD_BTN_W  ((CALL_AREA_W - HSEP_W * (N_FUNC_BTN_X_NUM - 1)) / N_FUNC_BTN_X_NUM)

#define FUNC_BTN_H ((CALL_AREA_H - CALL_INFO_H)- VSEP_H)
#define FUNC_BTN_W ((CALL_AREA_W - HSEP_W * (N_FUNC_BTN_NUM - 1)) / N_FUNC_BTN_NUM) 
#define CALL_INFO_W ((FUNC_BTN_W * N_FUNC_BTN_NUM) + HSEP_W * (N_FUNC_BTN_X_NUM - 1))

// sep
#define VSEP_W       CALL_INFO_W
#define VSEP_H       2

#define HSEP_W       2
#define HSEP_H       (CALL_AREA_H - CALL_INFO_H)
// call info
#define ENDCALL_X 10
#define ENDCALL_HIDEPAD_SPACE 20
#define ENDCALL_Y 8
#define ENDCALL_W ((ACTIVITY_W-(ENDCALL_X<<1)-ENDCALL_HIDEPAD_SPACE)>>1)
#define HIDEPAD_X ENDCALL_X+ENDCALL_W+ENDCALL_HIDEPAD_SPACE
#define HIDEPAD_W ENDCALL_W

// new end
/*text*/
#define FUNC_TEXT_Y 298-ACTIVITY_Y-CALL_TITLE_H-SPACE_H
#define FUNC_TEXT_FONT_H 14
#define TITLE_TEXT_FONT_H 42
#define KEYPAD_TEXT_FONT_H 36
#define KEYPAD_STARTEXT_FONT_H 60 
#define KEYPAD_SUBTEXT_FONT_H 12
#define CALL_INFO_FONT_H 18
#define CALL_HIDEKEYPAD_FONT_H 16
#define ENDCALL_IMAGE_X 131-ENDCALL_X   
#define ENDCALL_TEXT_IMAGE_SPACE 3
#define ENDCALL_TEXT_X(pbmp) ENDCALL_IMAGE_X+pbmp->bmWidth+ENDCALL_TEXT_IMAGE_SPACE
#define ENDCALL_TEXT_W 68 
#define CALL_INFO_COLOR 0xff2b3d01 
#define CALL_TITLE_COLOR 0x7f2b3d01 
#define CALL_KEYPAD_MAINCOLOR 0xff000000 
#define CALL_KEYPAD_SUBCOLOR 0x7f646464 
#define CALL_TITLE_SHADOW_COLOR 0x7fffffff 
#define FUNC_TEXT_COLOR 0xff373737 
#define ENDCALL_TEXT_COLOR 0xc0ffffff 
#define ENDCALL_TEXT_SHADOWCOLOR 0xc0000000 
#define ENDCALL_BK_COLOR 0xc0000000 
/*end text*/

#define PHONE_NUMBER_LEN 64

#define LOOP_COUNT 1000000
#define JUMP_ANIMATION_DURATION 400
#define FLIP_ANIMATION_DURATION 600
#define ANIMATION_OFFSET 15
#define BACK_ARGB_BKCOLOR    0xccfcfcfc
typedef enum{
    CNTR_FUNC,
    CNTR_KEYPAD,
    CNTR_MAX,
}CNTR_PIECE_INDEX;

typedef enum{
    BTN_ENDCALL,
    BTN_HIDEPAD,
    BTN_MAX,
}BTN_PIECE_INDEX;
typedef enum{
    ANIMATION_MUTE,
    ANIMATION_KEYPAD,
    ANIMATION_HANDSFREE,
    ANIMATION_MAX,
}ANIMATION_INDEX;

typedef struct func_contain_piece_t{
    mPanelPiece *func_subcontain_piece;
    mImagePiece *func_subimage_piece;
    mShapeTransRoundPiece *func_subgradient_piece;
    MGEFF_ANIMATION animation_handle;
}func_contain_piece;

class PhoneCallActivity : public NCSActivity {
public:
    PhoneCallActivity();
    ~PhoneCallActivity();

    virtual mPanelPiece* titleCreate(mContainerCtrl *ctnr);
    virtual mContainerCtrl* functionCreate(HWND hwnd,RECT rect);
    virtual mPanelPiece* keypadCreate(mContainerCtrl *ctnr,RECT rect);
    virtual mContainerCtrl* endCallButtonCreate(HWND hwnd,RECT rect);

    virtual void initResource(void);
    virtual void releaseResource(void);

    virtual MGEFF_ANIMATION startImageMoveAnimation(mContainerCtrl *ctnr,
        ANIMATION_INDEX idx,int duration,enum EffMotionType type,int offset);
    virtual void stopImageMoveAnimation(mContainerCtrl *ctnr,ANIMATION_INDEX idx);

    virtual MGEFF_ANIMATION startFlipPushAnimation(mContainerCtrl *ctnr,
            int duration,enum EffMotionType type,DWORD add_data);

    virtual int onStop();

    func_contain_piece *getFuncContainPiece(
            mPanelPiece *self,ANIMATION_INDEX *idx);

    char *getPhoneNumber(void)
    {
        return m_phoneNumber;
    }

    mPanelPiece *getCntrPiece(CNTR_PIECE_INDEX index)
    {
        if (index < CNTR_MAX)
        {
            return m_ctrnPiece[index];
        }
        return NULL;
    }
    mButtonPanelPiece *getBtnPiece(BTN_PIECE_INDEX index)
    {
        if (index < BTN_MAX)
        {
            return m_btnPiece[index];
        }
        return NULL;
    }
    mAnimationEditPiece *getTitleEditPiece(void)
    {
        return m_animationEditPiece;
    }
    mButtonPanelPiece *getKeyPiece(int x,int y)
    {
        if (x < N_FUNC_BTN_X_NUM && y < N_FUNC_BTN_Y_NUM)
            return m_keyPiece[x][y];
        return NULL;
    }

    virtual int onResume();
    virtual int onPause();

private:
    char m_phoneNumber[PHONE_NUMBER_LEN];
    func_contain_piece m_funcContainPiece[ANIMATION_MAX];
    mPanelPiece* m_ctrnPiece[CNTR_MAX];
    mButtonPanelPiece* m_btnPiece[BTN_MAX];
    mAnimationEditPiece *m_animationEditPiece;
    mButtonPanelPiece *m_keyPiece[N_FUNC_BTN_X_NUM][N_FUNC_BTN_Y_NUM];
    PLOGFONT m_mainFont;
    PLOGFONT m_subFont;
    PLOGFONT m_starFont;
    PLOGFONT m_infoFont;
    PLOGFONT m_funcFont;
    PLOGFONT m_endcallFont;
    PLOGFONT m_hideFont;
    PLOGFONT m_titleFont;
};
#endif/*end*/
