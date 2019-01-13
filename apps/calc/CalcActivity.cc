#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

#include "CalcActivity.hh"

#define MSG_CALC_USERAPP 13098
#define PRESS_TIMEOUT   10

REGISTER_ACTIVITY(CalcActivity);

const char *keystr[] = {
    "mc","m+","m-","mr",
    "7","8","9","+",
    "4","5","6","-",
    "1","2","3","x",
    "+/-","0",".","/",
    "C","="
};
static ARGB title_gradient_color[] = {
    0xffd0d571,0xff7e9d2c
};
static float title_gradient_pos[] = {
    0.0,1.0
};

static ARGB calc_bk_gradient_color[] = {
    0xff646873,0xff7a7d83,0xff646872,0xff2e2f32
};
static float calc_bk_gradient_pos[] = {
    0.0,0.005,0.01,1.0
};

static ARGB calc_titlebk_gradient_color[] = {
    0xff000000,0xff000000
};
static float calc_titlebk_gradient_pos[] = {
    0.0,1.0
};
static ARGB border_gradient_color[] = {
    0xff1a1a1a,0xff505050,0xff555555,0xff868890
};
static float border_gradient_pos[] = {
    0.0,0.1,0.98,1.0
};

static ARGB btn_num_gradient_color[] = {
    0xffffffff,0xffc2c2c2,0xfff4f4f4
};
static float btn_num_gradient_pos[] = {
    0.0,0.48,1.0
};

static ARGB btn_op_gradient_color[] = {
    0xffbfd2ed,0xff8aa9d7,0xff367fb4,0xff7de1fb
};
static float btn_op_gradient_pos[] = {
    0.0,0.02,0.48,1.0
};

static ARGB btn_op_high_gradient_color[] = {
    0xff7f94b3,0xff4d75af,0xff0562a4,0xff37b4d6
};
static float btn_op_high_gradient_pos[] = {
    0.0,0.01,0.48,1.0
};

static ARGB btndown_gradient_color[] = {
    0xffbdbdbd,0xff919191,0xffb6b6b6
};
static float btndown_gradient_pos[] = {
    0.0,0.48,1.0
};

static ARGB btn_edown_gradient_color[] = {
    0xff78995f,0xff49861c,0xff1d650b
};
static float btn_edown_gradient_pos[] = {
    0.0,0.02,1.0
};

static ARGB btn_e_gradient_color[] = {
    0xffaede8a,0xff6bc329,0xff299210
};
static float btn_e_gradient_pos[] = {
    0.0,0.48,1.0
};

#ifdef __cplusplus
extern "C" {
#endif
extern int phone_calc_compute(const char* left,
        char op,const char *right,char *result,size_t len);
#ifdef __cplusplus
}
#endif

static void titleAreaDash (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value);
static void calc_set_gradient_color(mShapeTransRoundPiece *backpiece,
        ARGB *data,float *pos,int num)
{
    HBRUSH brush;
    brush = _c(backpiece)->getBrush(backpiece);
    MGPlusSetLinearGradientBrushColorsEx(brush,data,num,pos);
    return;
}

static void calc_renew_op_color(CalcActivity* act)
{
    int i;
    int idx;
    mButtonPanelPiece *piece;
    for (i=0;i<4;i++)
    {
        idx = (i+2)*CALC_VER_NUM - 1;
        piece = (mButtonPanelPiece*)act->getButtonPieceIndex(NULL,idx);

        calc_set_gradient_color(_c(piece)->getBkgndPiece(piece),
            btn_op_gradient_color,btn_op_gradient_pos,TABLESIZE(btn_op_gradient_color));
    }
    return;
}

intptr_t CalcActivity::getButtonPieceIndex(mButtonPanelPiece* piece,int idx)
{
    int i;
    if (idx == -1)
    {
        for (i=0;i<CALC_BTN_NUM;i++)
        {
            if (piece == m_btnPiece[i])
                break;
        }
        if (i<CALC_BTN_NUM)
            return i;
    }
    if (idx >= 0 && idx < CALC_BTN_NUM)
    {
        return (intptr_t)m_btnPiece[idx]; 
    }
    return -1;
}

static BOOL judge_symbol_exist(char *str,char c)
{
    int i;
    int len = strlen(str);
    for (i=0;i<len;i++)
        if (str[i] == c)
            return TRUE;
    return FALSE;
}

static BOOL num_key_event(HWND hwnd, mButtonPanelPiece *piece)
{
    char *strtemp;
    CalcActivity* act = (CalcActivity*) Activity::getActivityFromHWND (hwnd);
    mAnimationEditPiece *editPiece = act->getAnimationEditPiece();
    mContainerCtrl *ctrl = (mContainerCtrl*)ncsGetChildObj(hwnd, CALC_IDC_TITLECTRL);
    int index = act->getButtonPieceIndex(piece,-1);

    if (editPiece == NULL || ctrl == NULL || (index < 0 && index > CALC_BTN_NUM))
        return FALSE;
    if (strcmp(keystr[index],"+/-") != 0&&
        (act->m_lastIndex > -1 && strcmp(keystr[act->m_lastIndex],"mr") == 0))
    {
        act->setOpSymbol('0');
        memset(act->getOperand(OPERAND_LEFT),0x00,OPERAND_MAX_NUM);
        memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
    }

    if (act->getOpSymbol() == '0' || act->getOpSymbol() == '=')
    {
        if (act->getOpSymbol() == '=')
        {
            if (strcmp(keystr[index],"+/-") != 0 && strcmp(keystr[index],".") != 0)
                memset(act->getOperand(OPERAND_LEFT),0x00,OPERAND_MAX_NUM);
            act->setOpSymbol('0');
        }
        strtemp = act->getOperand(OPERAND_LEFT);
    }
    else
    {
        strtemp = act->getOperand(OPERAND_RIGHT);
    }

    if (strcmp(keystr[index],".") == 0 && judge_symbol_exist(strtemp,'.'))
        return FALSE;
    if (strlen(strtemp) == 0 && strcmp(keystr[index],".") == 0)
    {
        strcpy(strtemp,"0");
        _c(editPiece)->setContent (ctrl->hwnd,editPiece,strtemp,TEXT_ALIGN_RIGHT);
    }

    if (strcmp(keystr[index],"+/-") == 0)
    {
        if (strlen(strtemp) >= OPERAND_MAX_NUM-1)
        {
            memset(strtemp,0x00,OPERAND_MAX_NUM);
            _c(editPiece)->setContent (ctrl->hwnd,editPiece,"",TEXT_ALIGN_RIGHT);
        }
        else if (strlen(strtemp) > 0 && 
                ((atof(strtemp) > 0.0) || strtemp[0] == '-'))
        {
            char temp[OPERAND_MAX_NUM] = {0};
            strcpy(temp,strtemp);
            memset(strtemp,0x00,OPERAND_MAX_NUM);
            if (temp[0] != '-')
            {
                strcpy(strtemp,"-");
                strcat(strtemp,temp);
            }
            else
            {
                strcpy(strtemp,&temp[1]);
            }
            _c(editPiece)->setContent (ctrl->hwnd,editPiece,strtemp,TEXT_ALIGN_RIGHT);
        }
    }
    if (strlen(strtemp) >= OPERAND_MAX_NUM-1 || 
            strlen(strtemp) == 0 || strtemp[0] == '.' || 
            (strtemp[0] == '0' && (strlen(strtemp) == 1 && 
            strcmp(keystr[index],".") != 0)))
    {
        memset(strtemp,0x00,OPERAND_MAX_NUM);
        _c(editPiece)->setContent (ctrl->hwnd,editPiece,"",TEXT_ALIGN_RIGHT);
        if (strcmp(keystr[index],"+/-") == 0)
        {
            _c(editPiece)->setContent (ctrl->hwnd,editPiece, "0",TEXT_ALIGN_RIGHT);
        }
    }

    if (strcmp(keystr[index],"+/-") != 0)
    {
        strcat(strtemp,keystr[index]);
        if (strlen(strtemp) == 1)
        {
            _c(editPiece)->setContent (ctrl->hwnd,editPiece, strtemp,TEXT_ALIGN_RIGHT);
        }
        else
        {
            _c(editPiece)->append (ctrl->hwnd,editPiece, keystr[index]);
        }
    }
    return TRUE;
}

static BOOL c_e_key_event(CalcActivity* act,mHotPiece *self,
    int message, int index,HWND main_hwnd)
{
    mAnimationEditPiece *editPiece = act->getAnimationEditPiece();
    mContainerCtrl *ctrl = (mContainerCtrl*)ncsGetChildObj(main_hwnd, CALC_IDC_TITLECTRL);

    if (message != NCSN_ABP_PUSHED || (index < 0 && index > CALC_BTN_NUM) 
            || ctrl == NULL || editPiece == NULL)
        return FALSE;

    if (strcmp(keystr[index],"C") == 0)
    {
        act->setOpSymbol('0');
        memset(act->getOperand(OPERAND_LEFT),0x00,OPERAND_MAX_NUM);
        memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
        //memset(act->getOperand(OPERAND_STORE),0x00,OPERAND_MAX_NUM);
        //act->getOperand(OPERAND_STORE)[0] = '0';
        _c(editPiece)->setContent (ctrl->hwnd,editPiece,"0",TEXT_ALIGN_RIGHT);
    }
    else if (strcmp(keystr[index],"=") == 0)
    {
        if (act->getOpSymbol() != '0' && act->getOpSymbol() != '=' 
                && strlen(act->getOperand(OPERAND_RIGHT)) > 0)
        {
            char result[OPERAND_MAX_NUM] = {0};
            phone_calc_compute((const char *)act->getOperand(OPERAND_LEFT),act->getOpSymbol(),
                (const char *)act->getOperand(OPERAND_RIGHT),result,OPERAND_MAX_NUM-1);
            act->startTextFlipAnimation(ctrl,act->getOperand(OPERAND_RIGHT),
                    result,CALC_ANIMATION_DURATION,Linear);
            act->setOpSymbol('=');
            strncpy(act->getOperand(OPERAND_LEFT),result,OPERAND_MAX_NUM-1);
            memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
        }
    }
    return TRUE;
}

static BOOL m_key_event(CalcActivity* act, mHotPiece *self,
    int message, int index,HWND main_hwnd)
{
    char *strbuffer;
    char strdis[OPERAND_MAX_NUM] = {0};
    char result[OPERAND_MAX_NUM] = {0};
    mAnimationEditPiece *editPiece = act->getAnimationEditPiece();
    mContainerCtrl *ctrl = (mContainerCtrl*)ncsGetChildObj(main_hwnd, CALC_IDC_TITLECTRL);

    if (message != NCSN_ABP_PUSHED || (index < 0 && index > CALC_BTN_NUM) 
            || ctrl == NULL || editPiece == NULL)
        return FALSE;

    strbuffer = act->getOperand(OPERAND_STORE);
    _c(editPiece)->getContent (editPiece,strdis,OPERAND_MAX_NUM-1);
    if (strcmp(keystr[index],"mc") == 0)
    {
        memset(strbuffer,0x00,OPERAND_MAX_NUM);
        strbuffer[0] = '0';
        result[0] = '0';
    }
    else if (strcmp(keystr[index],"m+") == 0)
    {
        phone_calc_compute((const char *)strbuffer,'+',(const char *)strdis,result,OPERAND_MAX_NUM-1);
        strcpy(strbuffer,result);
        act->setOpSymbol('0');
        memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
        memset(act->getOperand(OPERAND_LEFT),0x00,OPERAND_MAX_NUM);
    }
    else if (strcmp(keystr[index],"m-") == 0)
    {
        phone_calc_compute((const char *)strbuffer,'-',(const char *)strdis,result,OPERAND_MAX_NUM-1);
        strcpy(strbuffer,result);
        act->setOpSymbol('0');
        memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
        memset(act->getOperand(OPERAND_LEFT),0x00,OPERAND_MAX_NUM);
    }
    else if (strcmp(keystr[index],"mr") == 0)
    {
        if (act->getOpSymbol() != '0' && act->getOpSymbol() != '=')
        {
            /*
            phone_calc_compute((const char *)act->getOperand(OPERAND_LEFT),act->getOpSymbol(),
                    (const char *)strbuffer,result,OPERAND_MAX_NUM-1);
            strncpy(act->getOperand(OPERAND_LEFT),result,OPERAND_MAX_NUM-1);
            act->startTextFlipAnimation(ctrl,act->getOperand(OPERAND_LEFT),
                    result,CALC_ANIMATION_DURATION,Linear);
            */
            strncpy(act->getOperand(OPERAND_RIGHT),strbuffer,OPERAND_MAX_NUM-1);
            _c(editPiece)->setContent (ctrl->hwnd,editPiece,strbuffer,TEXT_ALIGN_RIGHT);
        }
        else
        {
            strncpy(act->getOperand(OPERAND_LEFT),strbuffer,OPERAND_MAX_NUM-1);
            memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
            act->startTextFlipAnimation(ctrl,act->getOperand(OPERAND_RIGHT),
                    strbuffer,CALC_ANIMATION_DURATION,Linear);
            memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
            act->setOpSymbol('0');
        }
    }
    return TRUE;
}

static BOOL op_key_event(CalcActivity* act, mHotPiece *self,
    int message, int index,HWND main_hwnd)
{
    mContainerCtrl *ctrl = (mContainerCtrl*)ncsGetChildObj(main_hwnd, CALC_IDC_TITLECTRL);
    if (index < 0 && index > CALC_BTN_NUM)
        return FALSE;

    if (message == NCSN_ABP_PUSHED)
    {
        char result[OPERAND_MAX_NUM] = {0};
        calc_set_gradient_color(_c((mButtonPanelPiece*)self)->getBkgndPiece((mButtonPanelPiece*)self),
            btn_op_high_gradient_color,btn_op_high_gradient_pos,TABLESIZE(btn_op_high_gradient_color));

        if (strlen(act->getOperand(OPERAND_RIGHT)) > 0 )
        {
            phone_calc_compute((const char *)act->getOperand(OPERAND_LEFT),act->getOpSymbol(),
                    (const char *)act->getOperand(OPERAND_RIGHT),result,OPERAND_MAX_NUM-1);
            strncpy(act->getOperand(OPERAND_LEFT),result,OPERAND_MAX_NUM-1);

            act->startTextFlipAnimation(ctrl,act->getOperand(OPERAND_RIGHT),
                    result,CALC_ANIMATION_DURATION,Linear);
            memset(act->getOperand(OPERAND_RIGHT),0x00,OPERAND_MAX_NUM);
        }
        if (strlen(act->getOperand(OPERAND_LEFT)) > 0)
        {
            act->setOpSymbol(keystr[index][0]);
        }
    }
    return TRUE;
}

static BOOL s_onTimer(HWND hwnd, LINT id, DWORD tickCount) {
    num_key_event(hwnd, (mButtonPanelPiece*)id);
    KillTimer(hwnd, id);
    return TRUE;
}

static BOOL key_event_handler(mWidget *self, mHotPiece *piece,
        int event_id, DWORD param)
{
    mContainerCtrl *ctnr = (mContainerCtrl *)self;
    HWND main_hwnd;
    CalcActivity* act;
    int index = -1;

    if (ctnr == NULL||event_id != NCSN_ABP_PUSHED)
        return FALSE;

    main_hwnd = GetMainWindowHandle(ctnr->hwnd);
    act = (CalcActivity*)
                Activity::getActivityFromHWND (main_hwnd);
    if (act == NULL)
        return FALSE;

    if (NULL != act->m_animation)
        return FALSE;
    calc_renew_op_color(act);

    index = act->getButtonPieceIndex((mButtonPanelPiece*)piece,-1);
    if ((index/CALC_VER_NUM) >= 1 && ((index+1)%CALC_VER_NUM) != 0)
    {
        if (index/CALC_VER_NUM != CALC_ROW_NUM - 1) //num key
        {
            if (event_id == NCSN_ABP_PUSHED) {
                SetTimerEx(main_hwnd, (LINT)piece, PRESS_TIMEOUT, s_onTimer);
            //num_key_event(self, piece);
            }
        }
        else    // c&e key
        {
            c_e_key_event(act,piece,event_id,index,main_hwnd);
        }
    }
    else if (index/CALC_VER_NUM == 0)   //m key
    {
        m_key_event(act,piece,event_id,index,main_hwnd);
    }
    else if (((index+1)%CALC_VER_NUM) == 0) //opt key
    {
        op_key_event(act,piece,event_id,index,main_hwnd);
    }

    if(act->m_lastIndex == -1)
        act->m_lastIndex = index;
    else if((act->m_lastIndex + 1) % CALC_VER_NUM == 0){
        mButtonPanelPiece *tmp;
        tmp = (mButtonPanelPiece*)act->getButtonPieceIndex(NULL, act->m_lastIndex);
        PanelPiece_invalidatePiece((mHotPiece*)tmp, NULL);
    }

    act->m_lastIndex = index;

    return TRUE;
}

BOOL CalcActivity::startTextFlipAnimation(mContainerCtrl *ctrl,
        char *str1,char *str2,int duration, EffMotionType type)
{
#if 0
    mPanelPiece *self = (mPanelPiece *)ctrl->body;

    _c(m_editPiece)->setContent (ctrl->hwnd,m_editPiece,str1,TEXT_ALIGN_RIGHT);
    MGEFF_ANIMATION animation = _c(self)->rotatePieceWithAnimation(self,(mHotPiece *)m_editPiece,
            90,1,0,0,duration,Linear);
    _c(self)->animationSyncRunAndDelete(self,animation);

    _c(m_editPiece)->setContent (ctrl->hwnd,m_editPiece,str2,TEXT_ALIGN_RIGHT);
    MGEFF_ANIMATION animation1 = _c(self)->rotatePieceWithAnimation(self,(mHotPiece *)m_editPiece,
            0,1,0,0,duration,Linear);
    _c(self)->animationSyncRunAndDelete(self,animation1);
#else
    MGEFF_EFFECTOR effector;
    MGEFF_SOURCE source1;
    MGEFF_SOURCE source2;
    MGEFF_SINK sink;
    MGEFF_ANIMATION handle;
    HDC src1_dc,src2_dc;
    HDC dst_dc;

    unsigned long key = mGEffStr2Key(MGEFF_MINOR_cubicrotate);

    _c(m_editPiece)->setContent (ctrl->hwnd,m_editPiece,str1,TEXT_ALIGN_RIGHT);
    dst_dc = GetDC(ctrl->hwnd);
    src1_dc = CreateCompatibleDC(dst_dc);
    BitBlt (dst_dc, 0, 0, 0, 0, src1_dc, 0, 0, 1);

    _c(m_editPiece)->setContent (ctrl->hwnd,m_editPiece,str2,TEXT_ALIGN_RIGHT);

    src2_dc = CreateCompatibleDC(dst_dc);

    _c(m_titleBkPiece)->paint(m_titleBkPiece,src2_dc,NULL,0);
    _c(m_editPiece)->paint(m_editPiece,src2_dc,NULL,0);

    effector = mGEffEffectorCreate (key);
    source1 = mGEffCreateSource (src1_dc);
    source2 = mGEffCreateSource (src2_dc);
    sink = mGEffCreateHDCSink (dst_dc);

    mGEffEffectorAppendSource (effector, source1);
    mGEffEffectorAppendSource (effector, source2);
    mGEffSetBufferSink(sink, src1_dc);
    mGEffEffectorSetSink (effector, sink);

    mGEffEffectorSetProperty (effector, MGEFF_PROPERTY_DIRECTION, MGEFF_DIRECTION_BOTTOM2TOP);
    handle = mGEffAnimationCreateWithEffector (effector);

    mGEffAnimationSetDuration (handle, duration);
    mGEffAnimationSyncRun (handle);
    mGEffAnimationDelete (handle);
    DeleteMemDC(src1_dc);
    DeleteMemDC(src2_dc);
    ReleaseDC(dst_dc);
    mGEffEffectorDelete(effector);
#endif
    return TRUE;
}

mContainerCtrl* CalcActivity::titleCreate(HWND hwnd,RECT rect)
{
    mContainerCtrl *containerctrl;
    mPanelPiece *toppiece;
    mPanelPiece *containerpiece;

    mHotPiece *backpiece1;
    mHotPiece *backpiece;
    mHotPiece *manimationeditpiece;

    RECT contentrc;
    SetRect(&contentrc,0,0,RECTW(rect), RECTH(rect));
    containerctrl = (mContainerCtrl*)ncsCreateWindow(
            NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_NONE/*WS_EX_TRANSPARENT*/, CALC_IDC_TITLECTRL,
            rect.left, rect.top,
            RECTW(rect), RECTH(rect),
            hwnd,
            NULL, NULL, NULL, 0);
    if (containerctrl == NULL)
        return NULL;

    //create memdc for title
    toppiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(toppiece)->setRect (toppiece, &contentrc);

    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(containerpiece)->setRect (containerpiece, &contentrc);
    m_containerpiece = containerpiece;

    //_c(containerctrl)->setBody(containerctrl, (mHotPiece*)containerpiece);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)toppiece);
    _c(toppiece)->addContent(toppiece, (mHotPiece*)containerpiece, 0, 0);

    backpiece1 = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece1)->setRect (backpiece1, &contentrc);
    _c(backpiece1)->setProperty (backpiece1, NCSP_TRANROUND_CORNERFLAG,0);

    _c(backpiece1)->setProperty (backpiece1,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    calc_set_gradient_color((mShapeTransRoundPiece *)backpiece1,
            calc_titlebk_gradient_color,calc_titlebk_gradient_pos,TABLESIZE(calc_titlebk_gradient_color));
    _c(containerpiece)->addContent(containerpiece, backpiece1, 0, 0);

    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &contentrc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_CORNERFLAG,0);

    _c(backpiece)->setProperty (backpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);

    calc_set_gradient_color((mShapeTransRoundPiece *)backpiece,
            title_gradient_color,title_gradient_pos,TABLESIZE(title_gradient_color));

    _c(containerpiece)->addContent(containerpiece, backpiece, -ACTIVITY_W, 0);
    m_titleBkPiece = backpiece;

    manimationeditpiece = (mHotPiece*)NEWPIECE(mAnimationEditPiece);
    contentrc.bottom >>= 1;
    contentrc.bottom <<= 1;
    contentrc.right -= 2;
    _c(manimationeditpiece)->setRect (manimationeditpiece, &contentrc);

    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_FONT,(DWORD)m_titleFont);

    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_TEXTCOLOR,CALC_TITLE_COLOR);
    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_TEXTSHADOWCOLOR,CALC_TITLE_SHADOW_COLOR);
    _c(containerpiece)->addContent(containerpiece, manimationeditpiece, -ACTIVITY_W, 0);
    m_editPiece = (mAnimationEditPiece *)manimationeditpiece;
    _c(m_editPiece)->setContent (containerctrl->hwnd,m_editPiece,"0",TEXT_ALIGN_RIGHT);

    return containerctrl;
}
mContainerCtrl* CalcActivity::keyBoardCreate(HWND hwnd,RECT rect)
{
    int i,j;
    mContainerCtrl *containerctrl;
    mPanelPiece *containerpiece;
    mPanelPiece *body;
    mHotPiece *containerbackpiece;

    mHotPiece *labelborderbackpiece;
    
    mButtonPanelPiece *buttonpiece;
    mTextPiece *textpiece;

    RECT labelborder_rc = {0, 0, CALC_NUM_SYM_BORDER_W,CALC_NUM_SYM_BORDER_H};
    RECT c_equal_border_rc = {0, 0, CALC_C_EQUAL_BORDER_W,CALC_C_EQUAL_BORDER_H};
    RECT label_rc = {0, 0, CALC_NUM_SYM_W,CALC_NUM_SYM_H};
    RECT c_equal_label_rc = {0, 0, CALC_C_EQUAL_W,CALC_C_EQUAL_H};
    RECT contain_rc = {0,0,RECTW(rect),RECTH(rect)};
    int border_x,border_y;
    int btn_x,btn_y;
    int event_ids[] = {NCSN_ABP_PUSHED, 0};

    containerctrl = (mContainerCtrl*)ncsCreateWindow(
            NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_NONE/*WS_EX_TRANSPARENT*/, CALC_IDC_KEYBORADCTRL,
            rect.left, rect.top,
            RECTW(rect), RECTH(rect),
            hwnd,
            NULL, NULL, NULL, 0);

    /* start set body contain */
    body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(body)->setRect (body, &contain_rc);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)body);
    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);

    _c(body)->addContent(body, (mHotPiece*)containerpiece, 0, 0);
    _c(containerpiece)->setRect (containerpiece, &contain_rc);

    containerbackpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(containerbackpiece)->setRect (containerbackpiece, &contain_rc);
    _c(containerbackpiece)->setProperty (containerbackpiece, NCSP_TRANROUND_CORNERFLAG,0);
    _c(containerbackpiece)->setProperty (containerbackpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    calc_set_gradient_color((mShapeTransRoundPiece *)containerbackpiece,
            calc_bk_gradient_color,calc_bk_gradient_pos,TABLESIZE(calc_bk_gradient_color));
    _c(containerpiece)->addContent(containerpiece, (mHotPiece*)containerbackpiece, 0, 0);
    /* end set body contain */

    for (i=0;i<CALC_ROW_NUM-1;i++)
        for (j=0;j<CALC_VER_NUM;j++)
    {
        border_x = CALC_SPACE_X+(CALC_NUM_SYM_BORDER_W+CALC_GAP_W)*j;
        border_y = CALC_SPACE_Y+(CALC_NUM_SYM_BORDER_H+CALC_GAP_H)*i;
        labelborderbackpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(labelborderbackpiece)->setRect (labelborderbackpiece, &labelborder_rc);
        _c(labelborderbackpiece)->setProperty (labelborderbackpiece, NCSP_TRANROUND_RADIUS, CALC_RADIUS_RANGE);
        _c(labelborderbackpiece)->setProperty (labelborderbackpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        calc_set_gradient_color((mShapeTransRoundPiece *)labelborderbackpiece,
            border_gradient_color,border_gradient_pos,TABLESIZE(border_gradient_color));
        _c(containerpiece)->addContent(containerpiece, labelborderbackpiece,border_x,border_y);	
        
        btn_x = border_x + CALC_BORDER_SPACE;
        btn_y = border_y + CALC_BORDER_SPACE; 
        
        buttonpiece = (mButtonPanelPiece *)NEWPIECE(mButtonPanelPiece);
        _c(buttonpiece)->setRect (buttonpiece, &label_rc);
        if (i == 0||j == CALC_VER_NUM - 1)
        {
            if (i == 0)
            {
                _c(buttonpiece)->setGradientBackgroundColor(buttonpiece,
                    btn_op_gradient_color,btn_op_gradient_pos,TABLESIZE(btn_op_gradient_color),
                    btn_op_high_gradient_color,btn_op_high_gradient_pos,TABLESIZE(btn_op_high_gradient_color));
            }
            else if (j == CALC_VER_NUM - 1 && i != 0)
            {
                mShapeTransRoundPiece* bkgnd = _c(buttonpiece)->getBkgndPiece(buttonpiece);
                _c(bkgnd)->setProperty (bkgnd,
                        NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
                 calc_set_gradient_color(bkgnd,
                    btn_op_gradient_color,btn_op_gradient_pos,TABLESIZE(btn_op_gradient_color));
            }
        }
        else
        {
            _c(buttonpiece)->setGradientBackgroundColor(buttonpiece,
                btn_num_gradient_color,btn_num_gradient_pos,TABLESIZE(btn_num_gradient_color),
                btndown_gradient_color,btndown_gradient_pos,TABLESIZE(btndown_gradient_color));
        }
        _c(buttonpiece)->setStateTable(buttonpiece,
            (mHotPiece*)_c(buttonpiece)->getBkgndPiece(buttonpiece), NCSP_TRANROUND_RADIUS,CALC_RADIUS_RANGE, CALC_RADIUS_RANGE);
        _c(buttonpiece)->setStates(buttonpiece);
        _c(containerpiece)->addContent(containerpiece, (mHotPiece*)buttonpiece,btn_x,btn_y);	

        ncsAddEventListeners((mObject*)buttonpiece, (mObject*)containerctrl,
            (NCS_CB_ONPIECEEVENT)key_event_handler, event_ids);
        m_btnPiece[i*CALC_VER_NUM+j] = buttonpiece;

        textpiece = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(textpiece)->setRect(textpiece, &label_rc);
        if (i == 0||j == CALC_VER_NUM - 1)
        {
            if (i == 0)
            {
                _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_mKeyFont);
            }
            else if (i == 1)
            {
                _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_addKeyFont);
            }
            else if (i == 2)
            {
                _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_subKeyFont);
            }
            else
            {
                _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_mulDivKeyFont);
            }
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_TEXTCOLOR, CALC_M_OP_FONT_COLOR);
        }
        else
        {
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_numKeyFont);
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_TEXTCOLOR, CALC_KEYFONT_COLOR);
            if (i == CALC_ROW_NUM - 2 && j==0)
            {
                _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_cKeyFont);
            }
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_TEXTSHADOWCOLOR, CALC_KEYFONT_SHADOWCOLOR);
        }
        _c((mTextPiece*)textpiece)->setProperty (textpiece, 
                NCSP_LABELPIECE_LABEL, (DWORD)keystr[i*CALC_VER_NUM+j]);
        _c(buttonpiece)->addContent(buttonpiece, (mHotPiece*)textpiece,0,0);	
    }
    for (j=0;j<2;j++)
    {
        border_x = CALC_SPACE_X+(CALC_C_EQUAL_BORDER_W+CALC_GAP_W)*j;
        border_y = CALC_SPACE_Y+(CALC_NUM_SYM_BORDER_H+CALC_GAP_H)*i;
        labelborderbackpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(labelborderbackpiece)->setRect (labelborderbackpiece, &c_equal_border_rc);
        _c(labelborderbackpiece)->setProperty (labelborderbackpiece, NCSP_TRANROUND_RADIUS, CALC_RADIUS_RANGE);
        _c(labelborderbackpiece)->setProperty (labelborderbackpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        calc_set_gradient_color((mShapeTransRoundPiece *)labelborderbackpiece,
            border_gradient_color,border_gradient_pos,TABLESIZE(border_gradient_color));
        _c(containerpiece)->addContent(containerpiece, labelborderbackpiece,border_x,border_y);	

        btn_x = border_x + CALC_BORDER_SPACE;
        btn_y = border_y + CALC_BORDER_SPACE; 
        buttonpiece = (mButtonPanelPiece *)NEWPIECE(mButtonPanelPiece);
        _c(buttonpiece)->setRect (buttonpiece, &c_equal_label_rc);
        if (j == 0)
        {
            _c(buttonpiece)->setGradientBackgroundColor(buttonpiece,
                btn_num_gradient_color,btn_num_gradient_pos,TABLESIZE(btn_num_gradient_color),
                btndown_gradient_color,btndown_gradient_pos,TABLESIZE(btndown_gradient_color));
        }
        else
        {
            _c(buttonpiece)->setGradientBackgroundColor(buttonpiece,
                btn_e_gradient_color,btn_e_gradient_pos,TABLESIZE(btn_e_gradient_color),
                btn_edown_gradient_color,btn_edown_gradient_pos,TABLESIZE(btn_edown_gradient_color));
        }
        _c(buttonpiece)->setStateTable(buttonpiece,
            (mHotPiece*)_c(buttonpiece)->getBkgndPiece(buttonpiece),NCSP_TRANROUND_RADIUS,CALC_RADIUS_RANGE,CALC_RADIUS_RANGE);
        _c(buttonpiece)->setStates(buttonpiece);
        _c(containerpiece)->addContent(containerpiece, (mHotPiece*)buttonpiece,btn_x,btn_y);	

        ncsAddEventListeners((mObject*)buttonpiece, (mObject*)containerctrl,
            (NCS_CB_ONPIECEEVENT)key_event_handler, event_ids);
        m_btnPiece[i*CALC_VER_NUM+j] = buttonpiece;

        textpiece = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(textpiece)->setRect (textpiece, &c_equal_label_rc);
        if (j == 1)
        {
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_equalKeyFont);
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_TEXTCOLOR, CALC_M_OP_FONT_COLOR);
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_TEXTSHADOWCOLOR, CALC_E_KEYFONT_SHADOWCOLOR);
        }
        else
        {
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_cKeyFont);
            _c(textpiece)->setProperty (textpiece, NCSP_TEXTPIECE_TEXTCOLOR, CALC_KEYFONT_COLOR);
        }
        _c((mTextPiece*)textpiece)->setProperty (textpiece, 
                NCSP_LABELPIECE_LABEL, (DWORD)keystr[i*CALC_VER_NUM+j]);
        _c(buttonpiece)->addContent(buttonpiece, (mHotPiece*)textpiece,0,0);	
    }
    return containerctrl;
}
void CalcActivity::initResource()
{
    m_titleFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALC_TITLE_FONT_H, 0);
    m_numKeyFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALC_KEY_FONT_H, 0);
    m_mKeyFont = m_numKeyFont;
    m_addKeyFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALC_ADD_KEY_FONT_H, 0);
    m_subKeyFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALC_SUB_KEY_FONT_H, 0);
    m_mulDivKeyFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALC_MUL_DIV_KEY_FONT_H, 0);
    m_equalKeyFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALC_EQUAL_KEY_FONT_H, 0);
    m_cKeyFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALC_C_KEY_FONT_H, 0);
    return;
}

void CalcActivity::releaseResource()
{
    DestroyLogFont(m_titleFont);
    DestroyLogFont(m_numKeyFont);
    DestroyLogFont(m_addKeyFont);
    DestroyLogFont(m_subKeyFont);
    DestroyLogFont(m_mulDivKeyFont);
    DestroyLogFont(m_equalKeyFont);
    DestroyLogFont(m_cKeyFont);
    return;
}

static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData )
{
    CalcActivity* act = (CalcActivity*)Activity::getActivityFromHWND (self->hwnd);
    RECT rectbody = {CALC_PANEL_X,CALC_PANEL_Y,
        CALC_PANEL_W+CALC_PANEL_X,CALC_PANEL_H+CALC_PANEL_Y};
    RECT title_rect = {0,0,CALC_TITLE_W,CALC_TITLE_H};

    act->titleCreate(self->hwnd,title_rect);
    act->keyBoardCreate(self->hwnd,rectbody);
    return TRUE;
}


static BOOL main_onClose(mWidget* self, int message)
{
    return TRUE;
}


static void main_onPaint (mWidget* _this, HDC hdc, const PCLIPRGN clipRgn)
{
    return;
}

static void calc_title_animation(mWidget* self,int pts,int pte,intptr_t id,int duration,EffMotionType curve)
{
    mContainerCtrl* ctnr = (mContainerCtrl*)ncsGetChildObj(self->hwnd, CALC_IDC_TITLECTRL);
    CalcActivity* act = (CalcActivity*)
                Activity::getActivityFromHWND (self->hwnd);
    mPanelPiece *body;

    MGEFF_ANIMATION animation;

    assert(ctnr);
    body = (mPanelPiece*)ctnr->body;
    assert(body);
    
    animation = mGEffAnimationCreate (self, titleAreaDash, id, MGEFF_INT);

    mGEffAnimationSetStartValue (animation, &pts);
    mGEffAnimationSetEndValue (animation, &pte);
    mGEffAnimationSetCurve (animation,curve);
    mGEffAnimationSetDuration (animation, duration);

    fprintf(stderr, "%s:animation is %p.\n", __FUNCTION__, animation);
    _c((mPanelPiece*)ctnr->body)->animationAsyncRun ((mPanelPiece*)ctnr->body, animation,0);
    act->m_animation = animation;
}

#define CALC_AM_START_POS1 -ACTIVITY_W
#define CALC_AM_START_POS2 0
#define CALC_AM_START_POS3 -60
#define CALC_AM_START_POS4 0
#define CALC_AM_START_POS5  -20

#define CALC_AM_END_POS1 CALC_AM_START_POS2
#define CALC_AM_END_POS2 CALC_AM_START_POS3
#define CALC_AM_END_POS3 CALC_AM_START_POS2
#define CALC_AM_END_POS4 CALC_AM_START_POS5
#define CALC_AM_END_POS5 CALC_AM_START_POS4
static void titleAreaDash (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value)
{
    mWidget* self = (mWidget*)target;
    CalcActivity* act = (CalcActivity*)
                Activity::getActivityFromHWND (self->hwnd);
    //mContainerCtrl* ctnr = (mContainerCtrl*)ncsGetChildObj(self->hwnd, CALC_IDC_TITLECTRL);
    //mPanelPiece *body = (mPanelPiece *)ctnr->body;
    mPanelPiece *body = act->getTitlePanelPiece();
    mAnimationEditPiece *editpiece = act->getAnimationEditPiece();

    int _v = *((int *)value);

    if (id <= 5)
    {
        _c(body)->movePiece(body,(mHotPiece *)act->m_titleBkPiece,_v,0);
        _c(body)->movePiece(body,(mHotPiece *)editpiece,_v,0);
    }
    if (id == 1 && _v == CALC_AM_END_POS1)
    {
        calc_title_animation(self,CALC_AM_START_POS2,CALC_AM_END_POS2,2,200,OutExpo);
    }
    if (id == 2 && _v == CALC_AM_END_POS2)
    {
        calc_title_animation(self,CALC_AM_START_POS3,CALC_AM_END_POS3,3,200,InExpo);
    }
    if (id == 3 && _v == CALC_AM_END_POS3)
    {
        calc_title_animation(self,CALC_AM_START_POS4,CALC_AM_END_POS4,4,100,OutExpo);
    }
    if (id == 4 && _v == CALC_AM_END_POS4)
    {
        calc_title_animation(self,CALC_AM_START_POS5,CALC_AM_END_POS5,5,100,InExpo);
    }
    if (id == 5 && _v == CALC_AM_END_POS5)
    {
        act->m_animation = NULL;
    }
}


static void onUserApp(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
    calc_title_animation(self,CALC_AM_START_POS1,CALC_AM_END_POS1,1,400,InQuart);
}

static BOOL main_userApp(mMainWnd* self, int msg, DWORD wparam, DWORD lparam)
{
    ncsSetComponentHandler((mComponent*)self, MSG_CALC_USERAPP, (void *)onUserApp);
    PostMessage(self->hwnd, MSG_CALC_USERAPP,0,0);
    return TRUE;
}
static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE, reinterpret_cast<void*>(main_onCreate)},
    {MSG_CLOSE, reinterpret_cast<void*>(main_onClose)},
    {MSG_PAINT, reinterpret_cast<void*>(main_onPaint)},
    {MSG_USER_APP_DATA, reinterpret_cast<void*>(main_userApp)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {0, NULL}
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_NONE,
        "Phone Call",
        NULL,
        NULL,
        main_handlers,
        NULL,
        0,
        0,
        0, 0,
};

CalcActivity::CalcActivity() :
    NCSActivity( &mymain_templ)
{ 
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_INIT_CLASS(mAnimationEditPiece);
    MGNCS_INIT_CLASS(mLayoutManager);
    MGNCS_INIT_CLASS(mCenterHBox);
    MGNCS_INIT_CLASS(mButtonPanelPiece);
    MGNCS_INIT_CLASS(mTextPiece);

    initResource();

    m_editPiece = NULL;
    m_op = '0';
    memset(m_operand,0x00,OPERAND_MAX*OPERAND_MAX_NUM);
    m_operand[OPERAND_STORE][0] = '0';
    m_lastIndex = -1;
    m_style = STYLE_PUSH;
}

CalcActivity::~CalcActivity(void) 
{
    if (m_animation&&mGEffAnimationGetProperty(m_animation,
                    MGEFF_PROP_STATE) == MGEFF_STATE_RUNNING)
    {
        mGEffAnimationStop(m_animation);
    }
    releaseResource();
}



