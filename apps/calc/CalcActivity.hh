#ifndef _CALC_ACTIVITY_HH_
#define _CALC_ACTIVITY_HH_
#include "NCSActivity.hh"
#include "global.h"


#define CALC_IDC_TITLECTRL   100
#define CALC_IDC_KEYBORADCTRL   101

#define CALC_TITLE_W SCREEN_W
#define CALC_TITLE_H 73

#define CALC_ROW_NUM 6
#define CALC_VER_NUM 4

#define CALC_SPACE_X 12
#define CALC_SPACE_Y 15
#define CALC_SPACE_V 5
#define CALC_GAP_W   13
#define CALC_GAP_H   7

#define CALC_BORDER_SPACE 3

#define CALC_PANEL_X ACTIVITY_X
#define CALC_PANEL_Y CALC_TITLE_H
#define CALC_PANEL_W CALC_TITLE_W
#define CALC_PANEL_H (SCREEN_H-CALC_TITLE_H)

#define CALC_NUM_SYM_BORDER_W ((CALC_PANEL_W-(CALC_SPACE_X<<1) \
            -(CALC_GAP_W*(CALC_VER_NUM-1)))/CALC_VER_NUM)
#define CALC_NUM_SYM_BORDER_H ((CALC_PANEL_H-CALC_SPACE_V-(CALC_SPACE_Y<<1) \
            -(CALC_GAP_H*(CALC_ROW_NUM-1)))/CALC_ROW_NUM)

#define CALC_C_EQUAL_BORDER_W ((CALC_PANEL_W-(CALC_SPACE_X<<1)-CALC_GAP_W)>>1)
#define CALC_C_EQUAL_BORDER_H CALC_NUM_SYM_H

#define CALC_NUM_SYM_W (CALC_NUM_SYM_BORDER_W-(CALC_BORDER_SPACE<<1))
#define CALC_NUM_SYM_H (CALC_NUM_SYM_BORDER_H-(CALC_BORDER_SPACE<<1))
#define CALC_C_EQUAL_W (CALC_C_EQUAL_BORDER_W-(CALC_BORDER_SPACE<<1))
#define CALC_C_EQUAL_H (CALC_C_EQUAL_BORDER_H-(CALC_BORDER_SPACE<<1))

#define CALC_RADIUS_RANGE 8

#define CALC_TITLE_FONT_H 42
#define CALC_M_KEY_FONT_H 23
#define CALC_ADD_KEY_FONT_H 35
#define CALC_SUB_KEY_FONT_H 48
#define CALC_MUL_DIV_KEY_FONT_H 30
#define CALC_EQUAL_KEY_FONT_H 45
#define CALC_KEY_FONT_H 33
#define CALC_C_KEY_FONT_H 30

#define OPERAND_MAX_NUM 16
#define CALC_BTN_NUM 22

#define CALC_ANIMATION_DURATION 400

#define CALC_TITLE_COLOR 0xc02b3d01 
#define CALC_M_OP_FONT_COLOR 0xffffffff 
#define CALC_TITLE_SHADOW_COLOR 0x7fffffff

#define CALC_KEYFONT_COLOR 0xc0192952
#define CALC_KEYFONT_SHADOWCOLOR 0x7fffffff
#define CALC_E_KEYFONT_SHADOWCOLOR 0x7f000000


typedef enum{
    OPERAND_LEFT,
    OPERAND_RIGHT,
    OPERAND_STORE,
    OPERAND_MAX,
}OPERAND_TYPE;

class CalcActivity : public NCSActivity {
public:
    CalcActivity();
    virtual mContainerCtrl* titleCreate(HWND hwnd,RECT rect);
    virtual mContainerCtrl* keyBoardCreate(HWND hwnd,RECT rect);
    virtual BOOL startTextFlipAnimation(mContainerCtrl *ctnr,
        char *str1,char *str2,int duration,enum EffMotionType type);
    virtual void initResource(void);
    virtual void releaseResource(void);
    int getButtonPieceIndex(mButtonPanelPiece* piece,int idx);
    mAnimationEditPiece *getAnimationEditPiece(void)
    {
        return m_editPiece;
    }
    mPanelPiece* getTitlePanelPiece(void)
    {
        return m_containerpiece;
    }
    char getOpSymbol(void)
    {
        return m_op;
    }
    void setOpSymbol(char a)
    {
        m_op = a;
    }
    char *getOperand(int idx)
    {
        if (idx >= 0 && idx < OPERAND_MAX)
            return m_operand[idx];
        return NULL;
    }
    ~CalcActivity();
public:
    int m_lastIndex;
    mHotPiece *m_titleBkPiece;
    MGEFF_ANIMATION m_animation;
private:
    PLOGFONT m_titleFont;
    PLOGFONT m_numKeyFont;
    PLOGFONT m_mKeyFont;
    PLOGFONT m_addKeyFont;
    PLOGFONT m_subKeyFont;
    PLOGFONT m_mulDivKeyFont;
    PLOGFONT m_equalKeyFont;
    PLOGFONT m_cKeyFont;

    mPanelPiece *m_containerpiece;
    mAnimationEditPiece *m_editPiece;
    mButtonPanelPiece* m_btnPiece[CALC_BTN_NUM];
    char m_operand[OPERAND_MAX][OPERAND_MAX_NUM];
    char m_op;
};
#endif/*end*/
