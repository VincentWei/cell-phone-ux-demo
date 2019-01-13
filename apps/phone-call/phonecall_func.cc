#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

#include "ActivityStack.hh"
#include "PhoneCallActivity.hh"

#define SEPARATOR_DARKER_COLOR 0xffb3b7a5
#define SEPARATOR_LIGHTER_COLOR 0xffffffff
#define PHONE_PNG_INDEX 3
const char* bmpfiles[] = {
    "res/phone-call/mute.png",
    "res/phone-call/keyboard.png",
    "res/phone-call/handsfree.png",
    "res/phone-call/phone.png",
    "res/phone-call/mute_32.png",
    "res/phone-call/keyboard_32.png",
    "res/phone-call/handsfree_32.png",
};

const char *main_text[] = {
    "1", "2", "3",
    "4", "5", "6",
    "7", "8", "9",
    "*", "0", "#"
};

const char *sub_text[] = {
    "",     "ABC", "DEF",
    "GHI",  "JKL", "MNO",
    "PQRS", "TUV", "WXYZ",
    "",     "+",   ""
};

static ARGB  func_gradient_color[] = {0xFFFFFFFF, 0xffc1af4c};
static float func_gradient_pos[] = {0.0, 1.0};

static ARGB  bk_gradient_color[] = {BACK_ARGB_BKCOLOR, BACK_ARGB_BKCOLOR};
static float bk_gradient_pos[] = {0.0, 1.0};

static ARGB func_bk_gradient_color[] = {0xccfcfcfc, 0xcce5e5e5, 0xccb2b2b2};
static float func_bk_gradient_pos[] = {0.0, 0.98, 1.0};

static ARGB  endcall_gradient_color[] = {0xFFfdc972, 0xFFfcb848, 0xffba3305, 0xffd25e39};
static float endcall_gradient_pos[] = {0.0, 0.03, 0.8, 1.0};

static ARGB hidekeypad_gradient_color[] = {0xFF868686, 0xFF616161, 0xff1c1c1c, 0xff393939, 0xff393939};
static float hidekeypad_gradient_pos[] = {0.0, 0.02, 0.94, 0.99, 1.0};

static ARGB title_gradient_color[] = {0xffcfd571, 0xff7c9731, 0xff354605};
static float title_gradient_pos[] = {0.0, 0.98, 1.0};

static ARGB btndown_gradient_color[] = {0xa0d0e4fb, 0xa0167cef, 0xa0d0e4fb};
static float btndown_gradient_pos[] = {0.0, 0.5, 1.0};
#if 0
static gradient_data func_gradient_color[] = {
    {0.0,0xFFFFFFFF},
    {1.0,0xffc1af4c}
};
static gradient_data bk_gradient_color[] = {
    {0.0,BACK_ARGB_BKCOLOR},
    {1.0,BACK_ARGB_BKCOLOR}
};
static gradient_data func_bk_gradient_color[] = {
    {0.0,0xccfcfcfc},
    {0.98,0xcce5e5e5},
    {1.0,0xccb2b2b2}
};
static gradient_data endcall_gradient_color[] = {
    {0.0,0xFFfdc972},
    {0.03,0xFFfcb848},
    {0.98,0xffba3305},
    {1.0,0xffd25e39}
};
static gradient_data hidekeypad_gradient_color[] = {
    {0.0,0xFF868686},
    {0.02,0xFF616161},
    {0.94,0xff1c1c1c},
    {0.99,0xff393939},
    {1.0,0xff393939}
};
static gradient_data title_gradient_color[] = {
    {0.0,0xffcfd571},
    {0.98,0xff7c9731},
    {1.0,0xff354605}
};
static gradient_data btndown_gradient_color[] = {
    {0.0,0xa0d0e4fb},
    {0.5,0xa0167cef},
    {1.0,0xa0d0e4fb}
};
#endif
static BOOL first_click_status = FALSE;

static void set_gradient_color(mShapeTransRoundPiece *piece,
        ARGB *colors, float *pos, int num)
{
    HBRUSH brush;
    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    brush = _c(piece)->getBrush(piece);
    MGPlusSetLinearGradientBrushColorsEx(brush,colors,num,pos);
    return;
}

static const char* get_key_str_from_piece(mHotPiece *self, PhoneCallActivity *act)
{
    int i, j;
    for (i = 0; i < N_FUNC_BTN_Y_NUM; ++i) {
        for (j = 0; j < N_FUNC_BTN_X_NUM; ++j) {
            if (self == (mHotPiece*)act->getKeyPiece(j, i)) {
                return main_text[i * N_FUNC_BTN_X_NUM + j];
            }
        }
    }
    return NULL;
}

static BOOL onClickKeypad(mWidget *self, mHotPiece *piece,
        int event_id, DWORD param)
{
    if(event_id == NCSN_ABP_PUSHED)
    {
        mAnimationEditPiece *editpiece;
        HWND main_hwnd = GetMainWindowHandle(self->hwnd);
        PhoneCallActivity* act = (PhoneCallActivity*)
            Activity::getActivityFromHWND (main_hwnd);
        mContainerCtrl *edit = (mContainerCtrl *)ncsGetChildObj(
            main_hwnd, ID_ANIMATION_EDIT);

        if (act == NULL||edit == NULL)
            return FALSE;

        const char *str = get_key_str_from_piece(piece, act);

        editpiece = act->getTitleEditPiece();
        if (editpiece == NULL)
            return FALSE;
        if (strlen(act->getPhoneNumber())>=(PHONE_NUMBER_LEN-1)||
                first_click_status == TRUE)
        {
            memset(act->getPhoneNumber(),0x00,PHONE_NUMBER_LEN);
            _c(editpiece)->setContent (edit->hwnd,editpiece,"",TEXT_ALIGN_CENTER);
        }


        strcat(act->getPhoneNumber(),str);
        _c(editpiece)->append(edit->hwnd,editpiece, str);
        first_click_status = FALSE;
        return TRUE;
    }
    return FALSE;
}

void PhoneCallActivity::initResource(void)
{
    int i;
    for (i=0;i<(signed)(sizeof(bmpfiles)/sizeof(char *));i++)
    {
        if (Load32Resource(bmpfiles[i],RES_TYPE_IMAGE,0) == NULL)
        {
            printf("bmp load failure!%s\n",bmpfiles[i]);
        }
    }
    m_mainFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            KEYPAD_TEXT_FONT_H, 0);

    m_starFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            KEYPAD_STARTEXT_FONT_H, 0);

    m_subFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            KEYPAD_SUBTEXT_FONT_H, 0);

    m_infoFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALL_INFO_FONT_H, 0);

    m_funcFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            FUNC_TEXT_FONT_H, 0);
    m_endcallFont = m_infoFont;

    m_hideFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            CALL_HIDEKEYPAD_FONT_H, 0);

    m_titleFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            TITLE_TEXT_FONT_H, 0);
}

void PhoneCallActivity::releaseResource(void)
{
    int i;
    for (i=0;i<(signed)(sizeof(bmpfiles)/sizeof(char *));i++)
    {
        UnregisterRes(bmpfiles[i]);
    }
    DestroyLogFont(m_mainFont);
    DestroyLogFont(m_subFont);
    DestroyLogFont(m_infoFont);
    DestroyLogFont(m_funcFont);
    DestroyLogFont(m_titleFont);
    DestroyLogFont(m_starFont);
    DestroyLogFont(m_hideFont);
}

static void imageAnimationPauseResume(PhoneCallActivity* act,BOOL isPause)
{
    int i;
    func_contain_piece *contain_piece_temp = NULL;
    ANIMATION_INDEX record;
    static BOOL contain_status[ANIMATION_MAX]; 

    assert(act);

    for (i=0;i<ANIMATION_MAX;i++)
    {
        record = (ANIMATION_INDEX)i;
        contain_piece_temp = act->getFuncContainPiece(NULL,&record);

        assert(contain_piece_temp);
        if (isPause)
        {
            contain_status[record] = FALSE;
            if (contain_piece_temp->animation_handle != NULL&&
                    mGEffAnimationGetProperty(contain_piece_temp->animation_handle,
                        MGEFF_PROP_STATE) == MGEFF_STATE_RUNNING)
            {
                act->stopImageMoveAnimation(NULL,(ANIMATION_INDEX)i);
                contain_status[record] = TRUE;
            }
        }
        else
        { 
            if (contain_status[record] && contain_piece_temp->animation_handle != NULL&&
                    mGEffAnimationGetProperty(contain_piece_temp->animation_handle,
                        MGEFF_PROP_STATE) == MGEFF_STATE_PAUSED)
            {
                RECT rc = {0, 0, FUNC_BTN_W, FUNC_BTN_H};

                PanelPiece_invalidatePiece((mHotPiece*)contain_piece_temp->func_subgradient_piece, &rc);
                set_gradient_color(contain_piece_temp->func_subgradient_piece,
                        func_gradient_color,func_gradient_pos,TABLESIZE(func_gradient_color));
                act->startImageMoveAnimation(NULL,(ANIMATION_INDEX)i,
                        JUMP_ANIMATION_DURATION,InQuad,ANIMATION_OFFSET);
            }
        }
    }
}

static int func_event_handler(mHotPiece *_self,
                int message, WPARAM wParam, LPARAM lParam, mObject *owner) 
{
    assert(owner != NULL);
    mContainerCtrl *ctnr = (mContainerCtrl *)owner;
    HWND main_hwnd = GetMainWindowHandle(ctnr->hwnd);

    PhoneCallActivity* act = (PhoneCallActivity*)
                Activity::getActivityFromHWND (main_hwnd);

    mPanelPiece *self = (mPanelPiece *)_self;

    func_contain_piece *contain_piece = NULL;
    ANIMATION_INDEX idx;

    contain_piece = act->getFuncContainPiece(self,&idx);
    if (contain_piece == NULL)
        return -1;

    if (idx == ANIMATION_KEYPAD)
    {
        if (message == MSG_LBUTTONUP)
        {
            set_gradient_color(contain_piece->func_subgradient_piece,
                    func_bk_gradient_color,func_bk_gradient_pos,TABLESIZE(func_bk_gradient_color));
            act->startFlipPushAnimation(ctnr,FLIP_ANIMATION_DURATION,OutQuad,0);
            imageAnimationPauseResume(act,TRUE);
            first_click_status = TRUE;
        }
        else if (message == MSG_LBUTTONDOWN)
        {
            set_gradient_color(contain_piece->func_subgradient_piece,
                    func_gradient_color,func_gradient_pos,TABLESIZE(func_gradient_color));
            PanelPiece_invalidatePiece((mHotPiece*)contain_piece->func_subgradient_piece,NULL);
        }
        return 0;
    }
    if (message == MSG_LBUTTONUP)
        return 0;
    if (contain_piece->animation_handle != NULL&&
        mGEffAnimationGetProperty(contain_piece->animation_handle,
                MGEFF_PROP_STATE) == MGEFF_STATE_RUNNING)
    {
        act->stopImageMoveAnimation(ctnr,(ANIMATION_INDEX)idx);
    }
    else
    { 
        RECT rc = {0, 0, FUNC_BTN_W, FUNC_BTN_H};

        PanelPiece_invalidatePiece((mHotPiece*)contain_piece->func_subgradient_piece, &rc);
        set_gradient_color(contain_piece->func_subgradient_piece,
            func_gradient_color,func_gradient_pos,TABLESIZE(func_gradient_color));

        act->startImageMoveAnimation(ctnr,(ANIMATION_INDEX)idx,
                JUMP_ANIMATION_DURATION,InQuad,ANIMATION_OFFSET);
    }
    return 0;
}

func_contain_piece *PhoneCallActivity::getFuncContainPiece(
        mPanelPiece *self,ANIMATION_INDEX *idx)
{
    int i;

    if ((self == NULL) && (idx != NULL)) {
        ANIMATION_INDEX index = *idx;

        if ((index >= ANIMATION_MUTE) && (index < ANIMATION_MAX)) {
            return &m_funcContainPiece[index];
        }

        return NULL;
    }
    else if (self != NULL) {
        for (i = 0; i < ANIMATION_MAX; i++) {
            if (m_funcContainPiece[i].func_subcontain_piece == self) {
                if (idx != NULL) {
                    *idx = (ANIMATION_INDEX)i;
                }

                return &m_funcContainPiece[i];
            }
        }
    }

    return NULL;
}
static BOOL endcall_event_handler(mWidget *self, mHotPiece *piece,
        int event_id, DWORD param) 
{
    mContainerCtrl *ctnr = (mContainerCtrl *)self;
    HWND main_hwnd;
    PhoneCallActivity* act;

    mButtonPanelPiece *hidepadpiece;

    if (ctnr == NULL||event_id != NCSN_ABP_CLICKED)
        return FALSE;

    main_hwnd = GetMainWindowHandle(ctnr->hwnd);
    act = (PhoneCallActivity*)
                Activity::getActivityFromHWND (main_hwnd);
    if (act == NULL)
        return FALSE;

    hidepadpiece = act->getBtnPiece(BTN_HIDEPAD);

    if (piece == (mHotPiece*)hidepadpiece) {
        imageAnimationPauseResume(act,FALSE);
        act->startFlipPushAnimation(ctnr,FLIP_ANIMATION_DURATION,OutQuad,0);
    }
    else {
        ACTIVITYSTACK->pop();
        //ActivityStack::singleton()->pop();
    }

    return TRUE;
}

mPanelPiece* PhoneCallActivity::titleCreate(mContainerCtrl *ctnr)
{
    mContainerCtrl *containerctrl = ctnr;
    mPanelPiece *toppiece;
    mPanelPiece *containerpiece;

    mHotPiece *backpiece;
    mHotPiece *manimationeditpiece;

    RECT contentrc;

    SetRect(&contentrc,0,0,CALL_TITLE_W, CALL_TITLE_H);
    if (ctnr == NULL)
        return NULL;

    toppiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(toppiece)->setRect (toppiece, &contentrc);

    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(containerpiece)->setRect (containerpiece, &contentrc);

    //_c(containerctrl)->setBody(containerctrl, (mHotPiece*)containerpiece);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)toppiece);
    _c(toppiece)->addContent(toppiece, (mHotPiece*)containerpiece, 0, 0);

    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &contentrc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_CORNERFLAG,0);

    set_gradient_color((mShapeTransRoundPiece *)backpiece,
            title_gradient_color,title_gradient_pos,TABLESIZE(title_gradient_color));

    _c(containerpiece)->addContent(containerpiece, backpiece, 0, 0);

    manimationeditpiece = (mHotPiece*)NEWPIECE(mAnimationEditPiece);
    _c(manimationeditpiece)->setRect (manimationeditpiece, &contentrc);
    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_FONT,(DWORD)m_titleFont);
    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_TEXTCOLOR,CALL_TITLE_COLOR);
    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_TEXTSHADOWCOLOR,CALL_TITLE_SHADOW_COLOR);
    _c(containerpiece)->addContent(containerpiece, manimationeditpiece, 0, 0);

    m_animationEditPiece = (mAnimationEditPiece *)manimationeditpiece;

#if 0
    _c((mAnimationEditPiece *)manimationeditpiece)->setContent(
        containerctrl->hwnd,(mAnimationEditPiece *)manimationeditpiece,m_phoneNumber,TEXT_ALIGN_CENTER);
#endif
    return containerpiece;
}

static BOOL add_func_content(mContainerCtrl *ctnr,
        mPanelPiece *body,PLOGFONT text_font)
{
    int idx;
    int contain_x,contain_y;
    HWND main_hwnd;
    PhoneCallActivity* act;
    mPanelPiece *content[N_FUNC_BTN_NUM];
	mHotPiece *image[N_FUNC_BTN_NUM];
    mHotPiece *backpiece[N_FUNC_BTN_NUM];
    mHotPiece *label;

    PBITMAP pbitmap;
    func_contain_piece *contain_piece = NULL;
    ANIMATION_INDEX index;

    int conerflag[N_FUNC_BTN_NUM] = {
        TRANROUND_CORNERFLAG_LEFTBOTTOM,
        0,
        TRANROUND_CORNERFLAG_RIGHTBOTTOM
    };
    const char *text_str[N_FUNC_BTN_NUM] = {
        "Mute",
        "Keypad",
        "Handsfree"
    };

    RECT rc = { 0, 0, FUNC_BTN_W, FUNC_BTN_H };
    RECT image_rc;
    RECT inforc;

    if (ctnr == NULL||body == NULL)
        return FALSE;

    main_hwnd = GetMainWindowHandle(ctnr->hwnd);
    act = (PhoneCallActivity*)Activity::getActivityFromHWND (main_hwnd);
    if (act == NULL)
        return FALSE;

    for (idx = 0; idx < N_FUNC_BTN_NUM; idx++)
    {
        contain_x = (FUNC_BTN_W + HSEP_W) * idx;
        contain_y = VSEP_H;

        content[idx] = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(content[idx])->setRect (content[idx], &rc);

        backpiece[idx] = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(backpiece[idx])->setRect (backpiece[idx], &rc);
        _c(backpiece[idx])->setProperty (backpiece[idx], NCSP_TRANROUND_RADIUS, 2);
        _c(backpiece[idx])->setProperty (backpiece[idx], NCSP_TRANROUND_CORNERFLAG, 
               conerflag[idx]);

        set_gradient_color((mShapeTransRoundPiece *)backpiece[idx],
                func_bk_gradient_color,func_bk_gradient_pos,TABLESIZE(func_bk_gradient_color));

        pbitmap = (PBITMAP)RetrieveRes(bmpfiles[idx]);
        SetRect(&image_rc,0,0,pbitmap->bmWidth,pbitmap->bmHeight);
        image[idx] = (mHotPiece*)NEWPIECE(mImagePiece);
        _c(image[idx])->setRect (image[idx], &image_rc);
        _c(image[idx])->setProperty (image[idx],NCSP_IMAGEPIECE_IMAGE, (DWORD)pbitmap);

        _c(content[idx])->addContent(content[idx], backpiece[idx], 0, 0);	
        _c(content[idx])->addContent(content[idx], image[idx], 
                (FUNC_BTN_W-pbitmap->bmWidth)>>1, (FUNC_BTN_H-pbitmap->bmHeight)>>1);	

        SetRect(&inforc,0,0,FUNC_BTN_W,FUNC_TEXT_FONT_H+4);
        label = (mHotPiece*)NEWPIECE(mTextPiece);
        _c(label)->setRect (label, &inforc);
        if (text_font != NULL)
        {
	        _c(label)->setProperty (label,NCSP_TEXTPIECE_LOGFONT, (DWORD)text_font);
        }
        _c(label)->setProperty(label, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)FUNC_TEXT_COLOR);
	    _c(label)->setProperty (label,NCSP_LABELPIECE_LABEL, (DWORD)text_str[idx]);
        _c(content[idx])->addContent(content[idx], label, 0,FUNC_TEXT_Y);	

        _c(body)->addContent(body, (mHotPiece*)content[idx], contain_x,contain_y);	
        _c(content[idx])->appendEventHandler(content[idx], MSG_LBUTTONDOWN, func_event_handler);
        _c(content[idx])->appendEventHandler(content[idx], MSG_LBUTTONUP, func_event_handler);

        index = (ANIMATION_INDEX)idx; 
        contain_piece = act->getFuncContainPiece(NULL,&index);
        contain_piece->func_subcontain_piece = content[idx];
        contain_piece->func_subimage_piece = (mImagePiece *)image[idx];
        contain_piece->func_subgradient_piece = (mShapeTransRoundPiece *)backpiece[idx];
    }

    return TRUE;
}

mContainerCtrl* PhoneCallActivity::functionCreate(HWND hwnd,RECT rect)
{
    int j;
    mContainerCtrl *containerctrl;
    mPanelPiece *containerpiece;

    mPanelPiece *content;
    mPanelPiece *labelcontent;
	mHotPiece *labelbackpiece;
    mHotPiece *separatorx[N_FUNC_BTN_X_NUM - 1];
    mHotPiece *separatory;
    mHotPiece *label;

    RECT inforc = {0, 0, CALL_INFO_W, CALL_INFO_H};

    RECT vseprc = {0, 0, VSEP_W, VSEP_H};
    RECT hseprc = {0, 0, HSEP_W, HSEP_H};

    RECT containrc;
    RECT contentrc;

    containerctrl = (mContainerCtrl*)ncsCreateWindow(
            NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, ID_CTRN_FUNC,
            rect.left, rect.top,
            RECTW(rect), RECTH(rect),
            hwnd,
            NULL, NULL, NULL, 0);

    /* start set contain */
    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&containrc,0,0,RECTW(rect),RECTH(rect));
    _c(containerpiece)->setRect (containerpiece, &containrc);

    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)containerpiece);
    /* end set contain */


    /* start set label content piece */
    labelcontent = (mPanelPiece*)NEWPIECE(mPanelPiece);

    _c(labelcontent)->setRect (labelcontent, &inforc);
    _c(containerpiece)->addContent(containerpiece, (mHotPiece*)labelcontent, 0, 0);

    labelbackpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(labelbackpiece)->setRect (labelbackpiece, &inforc);
    _c(labelbackpiece)->setProperty (labelbackpiece, NCSP_TRANROUND_BKCOLOR, BACK_ARGB_BKCOLOR);
    _c(labelbackpiece)->setProperty (labelbackpiece, NCSP_TRANROUND_RADIUS, 2);
    _c(labelbackpiece)->setProperty (labelbackpiece, NCSP_TRANROUND_CORNERFLAG, 
            TRANROUND_CORNERFLAG_RIGHTTOP|TRANROUND_CORNERFLAG_LEFTTOP);
    _c(labelcontent)->addContent(labelcontent, labelbackpiece, 0, 0);	

    label = (mHotPiece*)NEWPIECE(mTextPiece);
    _c(label)->setRect (label, &inforc);
    _c(label)->setProperty(label, NCSP_LABELPIECE_LABEL,(DWORD)"Calling...");
    _c(label)->setProperty(label, NCSP_TEXTPIECE_LOGFONT,(DWORD)m_infoFont);
    _c(label)->setProperty(label, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)CALL_INFO_COLOR);
    _c(labelcontent)->addContent(labelcontent, label, 0, 0);	
    /* end set label content piece */

    /* start set content piece */
    content = (mPanelPiece*)NEWPIECE(mPanelPiece);

    SetRect(&contentrc,0,0,RECTW(rect),RECTH(rect)- CALL_INFO_H);
    _c(content)->setRect (content, &contentrc);
    _c(containerpiece)->addContent(containerpiece, (mHotPiece*)content, 0, CALL_INFO_H);
    /* end set content piece */

    /* start set separator piece */
    for (j = 0; j < (N_FUNC_BTN_X_NUM - 1); j++)
    {
        separatorx[j] = (mHotPiece*)NEWPIECE(mPhoneSeparatorPiece);
        _c(separatorx[j])->setRect (separatorx[j], &hseprc);

        _c(separatorx[j])->setProperty (separatorx[j],
			NCSP_PHONESEPARATORPIECE_DARKER_COLOR, (DWORD)SEPARATOR_DARKER_COLOR);
        _c(separatorx[j])->setProperty (separatorx[j], 
			NCSP_PHONESEPARATORPIECE_LIGHTER_COLOR, (DWORD)SEPARATOR_LIGHTER_COLOR);
        _c(separatorx[j])->setProperty (separatorx[j], 
            NCSP_PHONESEPARATORPIECE_DIRECT_MODE, (DWORD)PHONESEPARATORPIECE_VERT);

        _c(content)->addContent(content, separatorx[j],
                KEYPAD_BTN_W + (KEYPAD_BTN_W  + HSEP_W) * j, 0);
    }

    separatory = (mHotPiece*)NEWPIECE(mPhoneSeparatorPiece);
	_c(separatory)->setRect (separatory, &vseprc);

	_c(separatory)->setProperty (separatory,
            NCSP_PHONESEPARATORPIECE_DARKER_COLOR, (DWORD)SEPARATOR_DARKER_COLOR);
    _c(separatory)->setProperty (separatory,
            NCSP_PHONESEPARATORPIECE_LIGHTER_COLOR, (DWORD)SEPARATOR_LIGHTER_COLOR);
    _c(separatory)->setProperty (separatory,
            NCSP_PHONESEPARATORPIECE_DIRECT_MODE, (DWORD)PHONESEPARATORPIECE_HERT);

    _c(content)->addContent(content, separatory,0, 0);
    /* end set separator piece */

    /* start set control piece */
    add_func_content(containerctrl,content,m_funcFont);
    /* end set control piece */

    m_ctrnPiece[CNTR_FUNC] = content;
     _c(containerpiece)->enableChildCache(containerpiece, (mHotPiece*)m_ctrnPiece[CNTR_FUNC], TRUE);
    return containerctrl;
}

mPanelPiece* PhoneCallActivity::keypadCreate(mContainerCtrl *ctnr, RECT rect)
{
    int i,j;
    mPanelPiece *containerpiece = (mPanelPiece *)ctnr->body;
    mPanelPiece *content;

    mHotPiece *keyboard;
    mButtonPanelPiece *key;
    mShapeTransRoundPiece *keyBackground;
    mHotPiece *separatorx[N_FUNC_BTN_X_NUM - 1];
    mHotPiece *separatory[N_FUNC_BTN_Y_NUM];

    RECT vseprc = {0, 0, VSEP_W, VSEP_H};
    RECT hseprc = {0, 0, HSEP_W, HSEP_H - VSEP_H};

    RECT btnrc = {0, 0, KEYPAD_BTN_W, KEYPAD_BTN_H};

    RECT contentrc;

    /* start set content piece */
    content = (mPanelPiece*)NEWPIECE(mPanelPiece);

    SetRect(&contentrc,0,0,RECTW(rect),RECTH(rect)-CALL_INFO_H);
    _c(content)->setRect (content, &contentrc);
    _c(containerpiece)->addContent(containerpiece, (mHotPiece*)content, 400, CALL_INFO_H);
    /* end set content piece */

    /* start set separator piece */
    for (j = 0; j < (N_FUNC_BTN_X_NUM - 1); j++)
    {
        separatorx[j] = (mHotPiece*)NEWPIECE(mPhoneSeparatorPiece);
        _c(separatorx[j])->setRect (separatorx[j], &hseprc);

        _c(separatorx[j])->setProperty (separatorx[j],
			NCSP_PHONESEPARATORPIECE_DARKER_COLOR, (DWORD)SEPARATOR_DARKER_COLOR);
        _c(separatorx[j])->setProperty (separatorx[j], 
			NCSP_PHONESEPARATORPIECE_LIGHTER_COLOR, (DWORD)SEPARATOR_LIGHTER_COLOR);
        _c(separatorx[j])->setProperty (separatorx[j], 
            NCSP_PHONESEPARATORPIECE_DIRECT_MODE, (DWORD)PHONESEPARATORPIECE_VERT);

        _c(content)->addContent(content, separatorx[j],
                KEYPAD_BTN_W + (KEYPAD_BTN_W  + HSEP_W) * j, 0);
    }

    for (i=0;i<N_FUNC_BTN_Y_NUM;i++)
    {
	    separatory[i] = (mHotPiece*)NEWPIECE(mPhoneSeparatorPiece);
		_c(separatory[i])->setRect (separatory[i], &vseprc);

		_c(separatory[i])->setProperty (separatory[i],
				NCSP_PHONESEPARATORPIECE_DARKER_COLOR, (DWORD)SEPARATOR_DARKER_COLOR);
		_c(separatory[i])->setProperty (separatory[i], 
				NCSP_PHONESEPARATORPIECE_LIGHTER_COLOR, (DWORD)SEPARATOR_LIGHTER_COLOR);
        _c(separatory[i])->setProperty (separatory[i], 
            NCSP_PHONESEPARATORPIECE_DIRECT_MODE, (DWORD)PHONESEPARATORPIECE_HERT);

	    _c(content)->addContent(content, separatory[i], 
			    0, (KEYPAD_BTN_H + VSEP_H) * i);
	}
    /* end set separator piece */

    /* start set keypad piece */
    for (j=0;j<N_FUNC_BTN_Y_NUM;j++)
    {
        for (i=0;i<N_FUNC_BTN_X_NUM;i++)
        {
            key = NEWPIECE(mButtonPanelPiece);
            keyBackground = _c(key)->getBkgndPiece(key);
            _c(key)->setRect (key, &btnrc);
            if (j == N_FUNC_BTN_Y_NUM - 1&&i == 0)
            {
                _c(keyBackground)->setProperty (keyBackground,
                        NCSP_TRANROUND_CORNERFLAG, TRANROUND_CORNERFLAG_LEFTBOTTOM);
            }
            else if (j == N_FUNC_BTN_Y_NUM - 1&&i == N_FUNC_BTN_X_NUM - 1)
            {
                _c(keyBackground)->setProperty (keyBackground,
                        NCSP_TRANROUND_CORNERFLAG, TRANROUND_CORNERFLAG_RIGHTBOTTOM);
            }
            else
            {
                _c(keyBackground)->setProperty (keyBackground, NCSP_TRANROUND_CORNERFLAG, 0);
            }
            _c(keyBackground)->setProperty (keyBackground, NCSP_TRANROUND_RADIUS, 2);

            _c(key)->setGradientBackgroundColor(key,
                    bk_gradient_color,bk_gradient_pos,TABLESIZE(bk_gradient_color),
                    btndown_gradient_color,btndown_gradient_pos,TABLESIZE(btndown_gradient_color));
            _c(key)->setStates(key);
            ncsAddEventListener((mObject*)key, (mObject*)ctnr, 
                    (NCS_CB_ONPIECEEVENT)onClickKeypad, NCSN_ABP_PUSHED);

	        _c(content)->addContent(content, (mHotPiece*)key,
                    (KEYPAD_BTN_W+VSEP_H) * i, (KEYPAD_BTN_H + HSEP_W) * j+HSEP_W);

            keyboard = (mHotPiece*)NEWPIECE(mPhoneStaticRDRPiece);
            _c(keyboard)->setRect (keyboard, &btnrc);

            _c(keyboard)->setProperty(keyboard,
                NCSP_PHONESTATICRDRPIECE_MAIN_TEXT, (DWORD)main_text[j*N_FUNC_BTN_X_NUM + i]);
            if (strcmp(main_text[j*N_FUNC_BTN_X_NUM + i],"*") == 0)
            {
                _c(keyboard)->setProperty(keyboard,
                        NCSP_PHONESTATICRDRPIECE_MAIN_FONT, (DWORD)m_starFont);
            }
            else
            {
                _c(keyboard)->setProperty(keyboard,
                        NCSP_PHONESTATICRDRPIECE_MAIN_FONT, (DWORD)m_mainFont);
                _c(keyboard)->setProperty(keyboard,
                        NCSP_PHONESTATICRDRPIECE_SUB_TEXT, (DWORD)sub_text[j*N_FUNC_BTN_X_NUM + i]);
                _c(keyboard)->setProperty(keyboard,
                    NCSP_PHONESTATICRDRPIECE_SUB_FONT, (DWORD)m_subFont);
            }
            _c(keyboard)->setProperty(keyboard,
                NCSP_PHONESTATICRDRPIECE_MAIN_COLOR, (DWORD)CALL_KEYPAD_MAINCOLOR);
            _c(keyboard)->setProperty(keyboard,
                NCSP_PHONESTATICRDRPIECE_SUB_COLOR, (DWORD)CALL_KEYPAD_SUBCOLOR);

	        _c(key)->addContentToLayout(key, keyboard);

            m_keyPiece[i][j] = (mButtonPanelPiece *)key;
        }
    }
    /* end set keypad piece */
    m_ctrnPiece[CNTR_KEYPAD] = (mPanelPiece *)content;
    _c(containerpiece)->enableChildCache(containerpiece, (mHotPiece*)m_ctrnPiece[CNTR_KEYPAD], TRUE);
    return content;
}

mContainerCtrl* PhoneCallActivity::endCallButtonCreate(HWND hwnd,RECT rect)
{
    mContainerCtrl *containerctrl;
    mPanelPiece *containerpiece;
    mButtonPanelPiece *endcallcontainpiece;
    mButtonPanelPiece *hidepadcontainpiece;
    mShapeTransRoundPiece *background;
	mHotPiece *backpiece;
    RECT containrc;
    RECT backrc;
    RECT labelrc;
    RECT image_rc;

    mHotPiece *endcallLabel;
	mHotPiece *endcallimage;
    mHotPiece *hidepadLabel;
    PBITMAP pbitmap;
    
    containerctrl = (mContainerCtrl*)ncsCreateWindow(
            NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, ID_BTN_ENDCALL,
            rect.left, rect.top,
            RECTW(rect), RECTH(rect),
            hwnd,
            NULL, NULL, NULL, 0);
    /* start set main contain */
    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&containrc,0,0,RECTW(rect),RECTH(rect));
    _c(containerpiece)->setRect (containerpiece, &containrc);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)containerpiece);
    /*end set main contain*/

    /* start set main backpiece */
    backrc = containrc;
    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &backrc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_BKCOLOR, ENDCALL_BK_COLOR);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_CORNERFLAG, 0);
    _c(containerpiece)->addContent(containerpiece, backpiece, 0, 0);	
    /* end set main backpiece */

    /* start set endcall contain */
    endcallcontainpiece = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
    containrc.right -= ENDCALL_X<<1;
    containrc.bottom -= ENDCALL_Y<<1;
    _c(containerpiece)->addContent(containerpiece, 
            (mHotPiece*)endcallcontainpiece, ENDCALL_X, ENDCALL_Y);	
    /*end set endcall contain*/

    /*start set endcall image piece*/
    pbitmap = (PBITMAP)RetrieveRes(bmpfiles[PHONE_PNG_INDEX]);
    if (pbitmap != NULL)
    {
        SetRect(&image_rc,0,0,pbitmap->bmWidth,containrc.bottom);
        endcallimage = (mHotPiece*)NEWPIECE(mImagePiece);
        _c(endcallimage)->setRect (endcallimage, &image_rc);
        _c(endcallimage)->setProperty (endcallimage,NCSP_IMAGEPIECE_IMAGE, (DWORD)pbitmap);
        _c(endcallcontainpiece)->addContentToLayout(endcallcontainpiece, endcallimage);	
    }
    /*end set endcall image piece*/

    /* start set endcall label piece */
    SetRect(&labelrc,0,0,ENDCALL_TEXT_W,containrc.bottom);
    endcallLabel = (mHotPiece*)NEWPIECE(mTextPiece);
    _c(endcallLabel)->setRect (endcallLabel, &labelrc);
    _c(endcallLabel)->setProperty(endcallLabel, NCSP_LABELPIECE_LABEL,(DWORD)"End call");
    _c(endcallLabel)->setProperty(endcallLabel, NCSP_TEXTPIECE_LOGFONT,(DWORD)m_endcallFont);
    _c(endcallLabel)->setProperty(endcallLabel, NCSP_TEXTPIECE_TEXTCOLOR,ENDCALL_TEXT_COLOR);
    _c(endcallLabel)->setProperty(endcallLabel, NCSP_TEXTPIECE_TEXTSHADOWCOLOR,ENDCALL_TEXT_SHADOWCOLOR);
    _c(endcallcontainpiece)->addContentToLayout(endcallcontainpiece,endcallLabel);	
    ncsAddEventListener((mObject*)endcallcontainpiece, (mObject*)containerctrl,
            (NCS_CB_ONPIECEEVENT)endcall_event_handler, NCSN_ABP_CLICKED);

    background = _c(endcallcontainpiece)->getBkgndPiece(endcallcontainpiece);
    _c(background)->setProperty (background, NCSP_TRANROUND_RADIUS,4);

    set_gradient_color(background,
            endcall_gradient_color,endcall_gradient_pos,TABLESIZE(endcall_gradient_color));
    _c(background)->setProperty(background,NCSP_TRANROUND_FILLENGINE,TRANROUND_FILLENGINE_NORMAL);

    _c(endcallcontainpiece)->setRect (endcallcontainpiece, &containrc);
    
    m_btnPiece[BTN_ENDCALL] = endcallcontainpiece;
    /* end set endcall label piece */

    /* start set hidepad contain */
    containrc.right = HIDEPAD_W;
    hidepadcontainpiece = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
    _c(hidepadcontainpiece)->setRect (hidepadcontainpiece, &containrc);
    _c(containerpiece)->addContent(containerpiece, 
            (mHotPiece*)hidepadcontainpiece, HIDEPAD_X, ENDCALL_Y);	
    _c(containerpiece)->movePiece(containerpiece,
            (mHotPiece *)hidepadcontainpiece,200,300);
    /*end set hidepad contain*/

    /* start set hidepad label piece */
    SetRect(&labelrc,0,0,containrc.right,containrc.bottom);
    hidepadLabel = (mHotPiece*)NEWPIECE(mTextPiece);
    _c(hidepadLabel)->setRect (hidepadLabel, &labelrc);
    _c(hidepadLabel)->setProperty(hidepadLabel, NCSP_LABELPIECE_LABEL,(DWORD)"Hide Keypad");
    _c(hidepadLabel)->setProperty(hidepadLabel, NCSP_TEXTPIECE_LOGFONT,(DWORD)m_hideFont);
    _c(hidepadLabel)->setProperty(hidepadLabel, NCSP_TEXTPIECE_TEXTCOLOR,ENDCALL_TEXT_COLOR);
    _c(hidepadLabel)->setProperty(hidepadLabel, NCSP_TEXTPIECE_TEXTSHADOWCOLOR,ENDCALL_TEXT_SHADOWCOLOR);
    _c(hidepadcontainpiece)->addContentToLayout(hidepadcontainpiece, hidepadLabel);

    ncsAddEventListener((mObject*)hidepadcontainpiece, (mObject*)containerctrl,
            (NCS_CB_ONPIECEEVENT)endcall_event_handler, NCSN_ABP_CLICKED);

    background = _c(hidepadcontainpiece)->getBkgndPiece(hidepadcontainpiece);
    _c(background)->setProperty(background,NCSP_TRANROUND_RADIUS,4);
    _c(hidepadcontainpiece)->setGradientBackgroundColor(hidepadcontainpiece,
            hidekeypad_gradient_color,hidekeypad_gradient_pos,TABLESIZE(hidekeypad_gradient_color),
            btndown_gradient_color,btndown_gradient_pos,TABLESIZE(btndown_gradient_color));
    _c(background)->setProperty(background,NCSP_TRANROUND_FILLENGINE,TRANROUND_FILLENGINE_NORMAL);
    _c(hidepadcontainpiece)->setStates(hidepadcontainpiece);

    m_btnPiece[BTN_HIDEPAD] = hidepadcontainpiece;
    /* end set hidepad label piece */
    return containerctrl;
}

static void act_onDraw (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value)
{
    ANIMATION_INDEX index;
    mPanelPiece *self = (mPanelPiece *)target;
    PhoneCallActivity* act;
    HWND main_hwnd = GetMainWindowHandle(_c(self)->getOwner(self)->hwnd);

    act = (PhoneCallActivity*)
                Activity::getActivityFromHWND (main_hwnd);
    if (act == NULL)
        return;

    func_contain_piece *contain_piece = NULL;
    contain_piece = act->getFuncContainPiece(self,&index);
    if (contain_piece == NULL)
        return;

    if (id == 1) {
        POINT *pt = (POINT*)value;
        _c(self)->movePiece(self,
                (mHotPiece *)contain_piece->func_subimage_piece,pt->x,pt->y);
    }else{
        int alpha = *((int *)value);
        _c(self)->setPieceAlpha(self,
                (mHotPiece *)contain_piece->func_subimage_piece,alpha);
    }
	return;
}
static void my_finish_cb(MGEFF_ANIMATION handle)
{
    return; 
}
MGEFF_ANIMATION PhoneCallActivity::startImageMoveAnimation(mContainerCtrl *ctnr,
                        ANIMATION_INDEX idx,int duration,enum EffMotionType type,
                        int offset)
{
    POINT pts = {0, 0};
    POINT pte = {0, 0};
    MGEFF_ANIMATION animation;
    MGEFF_ANIMATION animation1;
    MGEFF_ANIMATION animation2;

    //HWND main_hwnd;
    mPanelPiece *self;

    func_contain_piece *contain_piece = NULL;
    ANIMATION_INDEX index = idx;
    
    int id;

    if (offset == 0||idx<ANIMATION_MUTE||idx >= ANIMATION_MAX)
    {
        return NULL;
    }

    if (index == ANIMATION_MUTE)
        id = 0;
    else if (index == ANIMATION_HANDSFREE)
        id = 2;
    else
        return NULL;
    PBITMAP pbitmap = (PBITMAP)RetrieveRes(bmpfiles[id]);
    pts.x = (FUNC_BTN_W-pbitmap->bmWidth)>>1;
    pts.y = ((FUNC_BTN_H-pbitmap->bmHeight)>>1)-offset;
    pte.x = (FUNC_BTN_W-pbitmap->bmWidth)>>1;
    pte.y = ((FUNC_BTN_H-pbitmap->bmHeight)>>1)+offset;

    contain_piece = getFuncContainPiece(NULL,&index);
    if (contain_piece == NULL)
        return NULL;
    
    self = contain_piece->func_subcontain_piece;
    if (self == NULL)
        return NULL;

    if (contain_piece->animation_handle != NULL)
    {
        mGEffAnimationResume(contain_piece->animation_handle);
        return contain_piece->animation_handle;
    }
    assert(ctnr);
    //main_hwnd = GetMainWindowHandle(ctnr->hwnd);

    animation = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);
	if (animation == NULL)
		return NULL;

    if (idx) {
        animation1 = mGEffAnimationCreate (self, act_onDraw, 1, MGEFF_POINT);
        mGEffAnimationSetStartValue (animation1, &pts);
        mGEffAnimationSetEndValue (animation1, &pte);
        mGEffAnimationSetCurve (animation1,InQuad);
        mGEffAnimationSetDuration (animation1, duration);

        animation2 = mGEffAnimationCreate (self, act_onDraw, 
                1, MGEFF_POINT);
        mGEffAnimationSetStartValue (animation2, &pte);
        mGEffAnimationSetEndValue (animation2, &pts);
        mGEffAnimationSetCurve (animation2,OutQuad);
        mGEffAnimationSetDuration (animation2, duration);
    }else{
        int alphaStart = 0xff, alphaEnd = 0x00;
        duration = 800;
        EffMotionType curve = InQuad;

        animation1 = mGEffAnimationCreate (self, act_onDraw, 
                2, MGEFF_INT);
        mGEffAnimationSetStartValue (animation1, &alphaStart);
        mGEffAnimationSetEndValue (animation1, &alphaEnd);
        mGEffAnimationSetCurve (animation1,curve);
        mGEffAnimationSetDuration (animation1, duration);

        animation2 = mGEffAnimationCreate (self, act_onDraw, 
                2, MGEFF_INT);
        mGEffAnimationSetStartValue (animation2, &alphaEnd);
        mGEffAnimationSetEndValue (animation2, &alphaStart);
        mGEffAnimationSetCurve (animation2,curve);
        mGEffAnimationSetDuration (animation2, duration);
    }

    mGEffAnimationAddToGroup(animation,animation1);
    mGEffAnimationAddToGroup(animation,animation2);
	mGEffAnimationSetProperty(animation, MGEFF_PROP_LOOPCOUNT,LOOP_COUNT);
    mGEffAnimationSetFinishedCb(animation, my_finish_cb);

    _c((mPanelPiece*)ctnr->body)->animationAsyncRun ((mPanelPiece*)ctnr->body, animation, 0);
	contain_piece->animation_handle = animation;

    return animation;
}

void PhoneCallActivity::stopImageMoveAnimation(mContainerCtrl *ctnr,ANIMATION_INDEX idx)
{
    mPanelPiece *self;

    func_contain_piece *contain_piece = NULL;
    ANIMATION_INDEX index = idx;
    int id;
    if (index == ANIMATION_MUTE)
        id = 0;
    else if (index == ANIMATION_HANDSFREE)
        id = 2;
    else
        return;

    PBITMAP pbitmap = (PBITMAP)RetrieveRes(bmpfiles[id]);
    if (idx<ANIMATION_MUTE||idx >= ANIMATION_MAX)
    {
        return ;
    }

    contain_piece = getFuncContainPiece(NULL,&index);
    if (contain_piece == NULL)
        return ;
    
    self = contain_piece->func_subcontain_piece;
    if (self == NULL)
        return ;

    set_gradient_color(contain_piece->func_subgradient_piece,
            func_bk_gradient_color,func_bk_gradient_pos,TABLESIZE(func_bk_gradient_color));

    self = contain_piece->func_subcontain_piece;
    if (id == 2)
    {
        _c(self)->movePiece(self,
                (mHotPiece *)contain_piece->func_subimage_piece,
                (FUNC_BTN_W-pbitmap->bmWidth)>>1, (FUNC_BTN_H-pbitmap->bmHeight)>>1);
    }
    else if (id == 0)
    {
        _c(self)->setPieceAlpha(self,
                (mHotPiece *)contain_piece->func_subimage_piece,255);
    }
    mGEffAnimationPause(contain_piece->animation_handle);
    PanelPiece_invalidatePiece((mHotPiece*)self, NULL);

    return; 
}

static void act_push(mContainerCtrl *ctnr,PhoneCallActivity* act,int id,int pt)
{
    int hidepadx,hidepady;
    float temp;
    RECT rc;

    mButtonPanelPiece *endcallcontainpiece;
    mButtonPanelPiece *hidepadcontainpiece;

    mPanelPiece *self = (mPanelPiece *)ctnr->body;

    endcallcontainpiece = act->getBtnPiece(BTN_ENDCALL);
    hidepadcontainpiece = act->getBtnPiece(BTN_HIDEPAD);
    if (id == 1)
    {
        hidepadx = HIDEPAD_X+(HIDEPAD_W - pt);
        hidepady = ENDCALL_Y;

        temp = (float)HIDEPAD_W*(2.0-(float)pt/HIDEPAD_W);

        SetRect(&rc, 0, 0, hidepadx, CALL_END_CALL_H - (ENDCALL_Y<<1));
        PanelPiece_invalidatePiece((mHotPiece*)endcallcontainpiece, &rc);

        SetRect(&rc,0,0,(int)temp,CALL_END_CALL_H-(ENDCALL_Y<<1));
    }
    else
    {
        hidepadx = HIDEPAD_X+pt;
        hidepady = ENDCALL_Y;
        if (pt == HIDEPAD_W)
            hidepadx = 400;

        temp = (float)HIDEPAD_W*(1.0+(float)pt/HIDEPAD_W);
        if (pt == HIDEPAD_W)
            temp += ENDCALL_HIDEPAD_SPACE; 
        SetRect(&rc,0,0,(int)temp,CALL_END_CALL_H-(ENDCALL_Y<<1));
        PanelPiece_invalidatePiece((mHotPiece*)endcallcontainpiece, &rc);
    }
    _c(self)->movePiece(self,
            (mHotPiece *)hidepadcontainpiece,hidepadx,hidepady);
    _c(endcallcontainpiece)->setRect(endcallcontainpiece,&rc);
    return;
}

static void act_flip(mContainerCtrl *ctnr,PhoneCallActivity* act,int id,int pt)
{
    float angle;
    mPanelPiece *func_self;
    func_self = (mPanelPiece *)ctnr->body;
    if (id == 1)
    {
        angle = 180.0*((float)pt/HIDEPAD_W);

        /* this just for 32bit pixel format bitmap show */
        if (angle == 0)
            angle = 1;

        if (angle <=90)
        {
            _c(func_self)->rotatePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_FUNC),angle,0,1,0); 
        }
        else
        {
            _c(func_self)->movePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_FUNC),400,CALL_INFO_H);
            _c(func_self)->movePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_KEYPAD),0,CALL_INFO_H);
            _c(func_self)->rotatePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_KEYPAD),angle,0,1,0); 
        }        
    }
    else
    {
        angle = 180.0-180.0*((float)pt/HIDEPAD_W);
        if (angle > 90)
        {
            _c(func_self)->rotatePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_KEYPAD),angle,0,1,0); 
        }
        else
        {
            _c(func_self)->movePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_KEYPAD),400,CALL_INFO_H);
            _c(func_self)->movePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_FUNC),0,CALL_INFO_H);
            _c(func_self)->rotatePiece(func_self,
                (mHotPiece *)act->getCntrPiece(CNTR_FUNC),angle,0,1,0); 
        }
    }
}
static void act_pushflipCB (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value)
{
    HWND main_hwnd;
    mContainerCtrl *ctnr = (mContainerCtrl *)target;
    PhoneCallActivity* act;
    mContainerCtrl *function_ctrl;

    int pt = *(int*)value;

    if (ctnr == NULL)
        return;

    main_hwnd = GetMainWindowHandle(ctnr->hwnd);
    act = (PhoneCallActivity*)Activity::getActivityFromHWND(main_hwnd);
    if (act == NULL)
        return;

    function_ctrl = (mContainerCtrl *)ncsGetChildObj(main_hwnd, ID_CTRN_FUNC);
    if (function_ctrl == NULL)
        return;

    act_push(ctnr,act,id,pt);
    act_flip(function_ctrl,act,id,pt);
	return;
}


MGEFF_ANIMATION PhoneCallActivity::startFlipPushAnimation(mContainerCtrl *ctnr,
                        int duration,enum EffMotionType type,DWORD add_data)
{
    int i;
    int pte = 0;
    int pts = HIDEPAD_W; 
    MGEFF_ANIMATION animation;

    HWND main_hwnd = GetMainWindowHandle(ctnr->hwnd);
    mContainerCtrl *btn = (mContainerCtrl *)ncsGetChildObj(main_hwnd, ID_BTN_ENDCALL);
    mContainerCtrl *pad_ctrl = (mContainerCtrl *)ncsGetChildObj(main_hwnd, ID_CTRN_FUNC);
    mPanelPiece* cntr = (mPanelPiece*)pad_ctrl->body;

    PBITMAP pbitmap;
    mImagePiece* image;
    func_contain_piece *contain_piece = NULL;
    PhoneCallActivity* act = (PhoneCallActivity*)Activity::getActivityFromHWND (main_hwnd);

    if (btn == ctnr)
    {
        animation = mGEffAnimationCreate (ctnr, act_pushflipCB, 0, MGEFF_INT);
    }
    else
    {
        /* if is func container must change bitmap pixel format to match the cache dc */
        for ( i = 0; i <  ANIMATION_MAX; i++ ) {
            pbitmap = (PBITMAP)RetrieveRes(bmpfiles[i+4]);
            contain_piece = act->getFuncContainPiece(NULL, (ANIMATION_INDEX*)&i);
            image = contain_piece->func_subimage_piece;
            _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbitmap);
        }

        /* update the cache */
        _c(cntr)->updateChildCache(cntr, (mHotPiece*)m_ctrnPiece[CNTR_FUNC]);

        animation = mGEffAnimationCreate (btn, act_pushflipCB, 1, MGEFF_INT);
    }

    mGEffAnimationSetStartValue (animation, &pte);
    mGEffAnimationSetEndValue (animation, &pts);
    mGEffAnimationSetCurve (animation,type);
    mGEffAnimationSetDuration (animation, duration);

    _c((mPanelPiece*)ctnr->body)->animationSyncRunAndDelete ((mPanelPiece*)ctnr->body, animation);

    /* if is func container must restore bitmap pixel format to match the real dc */
    if (btn != ctnr) {
        for ( i = 0; i <  ANIMATION_MAX; i++ ) {
            pbitmap = (PBITMAP)RetrieveRes(bmpfiles[i]);
            contain_piece = act->getFuncContainPiece(NULL, (ANIMATION_INDEX*)&i);
            image = contain_piece->func_subimage_piece;
            _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbitmap);
        }
        InvalidateRect(pad_ctrl->hwnd, NULL, TRUE);
    }

    return NULL;
}

int PhoneCallActivity::onStop()
{
    imageAnimationPauseResume(this,TRUE);
    return 0;
}
