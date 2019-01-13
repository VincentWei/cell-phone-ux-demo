#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

#include "../common/mactivitytoolbarpiece.h"

#include "DialPadActivity.hh"

#define BACKSPACE_STR "backspace"
#define TOOLBAR_H 49 

REGISTER_ACTIVITY(DialPadActivity);

const char* dialtoolbarbmps[] = {
    "res/dial/callrecord.png",
    "res/dial/dialpad.png",
    "res/dial/contact.png",
};

const char* dialpadbmps[] = {
    "res/dial/dialplus.png",
    "res/dial/dialcall.png",
    "res/dial/dialbks.png",
};
const char *dialmainstr[] = {
    "1","2","3",
    "4","5","6",
    "7","8","9",
    "*","0","#",
};
#define STAR_CHAR_INDEX 9
const char *dialsubstr[] = {
    "","ABC","DEF",
    "GHI","JKL","MNO",
    "PQRS","TUV","WXYZ",
    "","+","",
};
static ARGB title_gradient_color[] = {
        0xffd0d571,0xff7e9d2c
};
static float title_gradient_pos[] = {
        0.0,1.0
};

static ARGB dial_bk_gradient_color[] = {
        0xff646873,0xff7a7d83,0xff646872,0xff2e2f32
};
static float dial_bk_gradient_pos[] = {
        0.0,0.005,0.01,1.0
};

static ARGB dial_btn_gradient_color[] = {
        0xffffffff,0xfff2f2f2,0xffb4b9bf 
};
static float dial_btn_gradient_pos[] = {
        0.0,0.02,1.0
};

static ARGB dial_btnhi_gradient_color[] = {
        0xff989898,0xffc2c2c2,0xff898989
};
static float dial_btnhi_gradient_pos[] = {
        0.0,0.48,1.0
};

static ARGB dial_call_gradient_color[] = {
        0xffc2e4c0,0xff77c472,0xff22a11b
};
static float dial_call_gradient_pos[] = {
        0.0,0.02,1.0
};

enum _ACTIVE_INDEX{
    TOOLBAR_IMAGE_CALLLOG,
    TOOLBAR_IMAGE_DIAL,
    TOOLBAR_IMAGE_CONTACTS,
    TOOLBAR_MAX,
};

const static char *PICTURES[] = {
    "res/address-book/address_book_calllog.png",
    "res/address-book/address_book_dial.png",
    "res/address-book/address_book_contacts.png",
};

static BOOL to_call_log (mObject *self, mObject *piece, int event_id,  DWORD param);
static BOOL to_contacts (mObject *self, mObject *piece, int event_id,  DWORD param);

static void dial_set_gradient_color(mShapeTransRoundPiece *backpiece,
                ARGB *data,float *pos,int num)
{
    HBRUSH brush;
    brush = _c(backpiece)->getBrush(backpiece);
    MGPlusSetLinearGradientBrushColorsEx(brush,data,num,pos);
    return;
}

static BOOL dialkey_event_handler(mWidget *self, mHotPiece *piece,
        int event_id, DWORD param)
{
    mContainerCtrl *ctnr = (mContainerCtrl *)self;
    HWND main_hwnd;
    DialPadActivity* act;
    mContainerCtrl *editctrl;
    static char outstr[DIAL_SECOND_LEN_MAX +1] = {0};

    int index;

    if (ctnr == NULL||event_id != NCSN_ABP_CLICKED)
        return FALSE;

    main_hwnd = GetMainWindowHandle(ctnr->hwnd);
    act = (DialPadActivity*)
        Activity::getActivityFromHWND (main_hwnd);
    if (act == NULL)
        return FALSE;

    editctrl = (mContainerCtrl*)ncsGetChildObj(main_hwnd, IDC_DIAL_TITLE);
    assert(editctrl);

    index = act->getDialBtnIndexByPiece((mButtonPanelPiece*)piece);
    if (index == -1)
        return FALSE;

    if (index < DIALPAD_BTN_NUM - DIAL_BTN_X_NUM)
    {
        memset(outstr,0x00,sizeof(outstr));
        act->titleNumDisplay(editctrl->hwnd,dialmainstr[index],outstr);
    }
    else if (index == DIALPAD_BTN_NUM - 2)
    {
        act->titleNumDisplay(editctrl->hwnd,"",outstr);
        if (strlen(outstr) > 0)
        {
            act->m_intent->putExtra("number",std::string(outstr));
            ACTIVITYSTACK->push("PhoneCallActivity",act->m_intent);
        }
    }
    else if (index == DIALPAD_BTN_NUM - 1)
    {
        memset(outstr,0x00,sizeof(outstr));
        act->titleNumDisplay(editctrl->hwnd,BACKSPACE_STR,outstr);
    }
    return TRUE;
}
/*
static BOOL toolbar_event_handler(mWidget *self,
         mHotPiece *sender, int event_id, DWORD param)
{
    return TRUE;
}
*/
static mButtonPanelPiece* dialBtnCreate(mContainerCtrl *ctnr, int index,RECT rc,PLOGFONT mainfont,PLOGFONT subfont)
{
    mButtonPanelPiece *dialbtnpiece;
    mShapeTransRoundPiece *backpiece;
    mTextPiece *maintextpiece;
    mTextPiece *subtextpiece;
    mHotPiece *imagepiece;

    RECT rect;
    SetRect(&rect,0,0,RECTW(rc),RECTH(rc));
    dialbtnpiece = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
    _c(dialbtnpiece)->setRect (dialbtnpiece, &rect);

    backpiece = _c(dialbtnpiece)->getBkgndPiece(dialbtnpiece);
    _c(backpiece)->setProperty(backpiece,NCSP_TRANROUND_RADIUS,DIAL_BUTTON_RADIUS);
    if (index == DIALPAD_BTN_NUM - 2)
    {
        _c(dialbtnpiece)->setGradientBackgroundColor(dialbtnpiece,
                dial_call_gradient_color,dial_call_gradient_pos,TABLESIZE(dial_call_gradient_color),
                dial_btnhi_gradient_color,dial_btnhi_gradient_pos,TABLESIZE(dial_btnhi_gradient_color)); 
    }
    else
    {
        _c(dialbtnpiece)->setGradientBackgroundColor(dialbtnpiece,
                dial_btn_gradient_color,dial_btn_gradient_pos,TABLESIZE(dial_btn_gradient_color),
                dial_btnhi_gradient_color,dial_btnhi_gradient_pos,TABLESIZE(dial_btnhi_gradient_color)); 
    }
    if (index < DIALPAD_BTN_NUM - DIAL_BTN_X_NUM)
    {
        int y = 0;
        if (index == STAR_CHAR_INDEX)
        {
            SetRect(&rect,0,0,RECTW(rc),RECTH(rc));
            y = 10;
        }
        else
        {
            SetRect(&rect,0,0,RECTW(rc),RECTH(rc)*2/3);
        }
        maintextpiece = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(maintextpiece)->setRect (maintextpiece, &rect);  
        _c(maintextpiece)->setProperty (maintextpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)mainfont);
        _c(maintextpiece)->setProperty (maintextpiece, NCSP_TEXTPIECE_TEXTCOLOR, DIAL_MAINFONT_COLOR);
        _c(maintextpiece)->setProperty (maintextpiece, NCSP_TEXTPIECE_TEXTSHADOWCOLOR, DIAL_MAINFONT_SHADOWCOLOR);
        _c((mTextPiece*)maintextpiece)->setProperty (maintextpiece,
                NCSP_LABELPIECE_LABEL, (DWORD)dialmainstr[index]);
        _c(dialbtnpiece)->addContent(dialbtnpiece, (mHotPiece*)maintextpiece,0,y);

        SetRect(&rect,0,0,RECTW(rc),RECTH(rc)*1/3);
        subtextpiece = (mTextPiece*)NEWPIECE(mTextPiece);
        _c(subtextpiece)->setRect (subtextpiece, &rect);  
        _c(subtextpiece)->setProperty (subtextpiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)subfont);
        _c(subtextpiece)->setProperty (subtextpiece, NCSP_TEXTPIECE_TEXTCOLOR, DIAL_SUBFONT_COLOR);
        _c((mTextPiece*)subtextpiece)->setProperty (subtextpiece,
                NCSP_LABELPIECE_LABEL, (DWORD)dialsubstr[index]);
        _c(dialbtnpiece)->addContent(dialbtnpiece, (mHotPiece*)subtextpiece,0,RECTH(rc)*2/3-4);
    }
    else if (index >= DIALPAD_BTN_NUM - DIAL_BTN_X_NUM && index < DIALPAD_BTN_NUM)
    {
        SetRect(&rect,0,0,RECTW(rc),RECTH(rc));
        imagepiece = (mHotPiece*)NEWPIECE(mImagePiece);
        _c(imagepiece)->setRect (imagepiece, &rect);
        _c(imagepiece)->setProperty (imagepiece,NCSP_IMAGEPIECE_IMAGE,
                (DWORD)RetrieveRes(dialpadbmps[index - (DIALPAD_BTN_NUM - DIAL_BTN_X_NUM)]));
        _c(dialbtnpiece)->addContentToLayout(dialbtnpiece, imagepiece);
    }
    _c(dialbtnpiece)->setStates(dialbtnpiece);
    return dialbtnpiece;
}

mContainerCtrl* DialPadActivity::dialPadCreate(HWND main_hwnd,RECT rect)
{
    int i;
    mContainerCtrl *containerctrl;
    mPanelPiece *containerpiece;
    mPanelPiece *body;

    mButtonPanelPiece *dialbtnpiece;
    mHotPiece *backpiece;

    RECT rc;
    SetRect(&rc,0,0,RECTW(rect), RECTH(rect));

    containerctrl = (mContainerCtrl*)ncsCreateWindow(
            NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_NONE/*WS_EX_TRANSPARENT*/, IDC_DIAL_BODY,
            rect.left, rect.top,
            RECTW(rect), RECTH(rect),
            main_hwnd,
            NULL, NULL, NULL, 0);
    if (containerctrl == NULL)
        return NULL;
    body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(body)->setRect (body, &rc);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)body);

    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(containerpiece)->setRect (containerpiece, &rc);
    _c(body)->addContent(body, (mHotPiece*)containerpiece, 0, 0);

    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &rc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_CORNERFLAG,0);

    _c(backpiece)->setProperty (backpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);

    dial_set_gradient_color((mShapeTransRoundPiece *)backpiece,
            dial_bk_gradient_color,dial_bk_gradient_pos,TABLESIZE(dial_bk_gradient_color));

    _c(containerpiece)->addContent(containerpiece, backpiece, 0, 0);


    for (i=0;i<DIALPAD_BTN_NUM;i++)
    {
        int x,y;
        PLOGFONT mainfont = m_padMainFont;
        x = (i%DIAL_BTN_X_NUM) * DIAL_BTN_W + DIAL_SPACE_W * ((i%DIAL_BTN_X_NUM)) + DIAL_PAD_X;     
        y = (i/DIAL_BTN_X_NUM) * DIAL_BTN_H + DIAL_SPACE_H * ((i/DIAL_BTN_X_NUM)) + DIAL_PAD_Y;     
        SetRect(&rc,x,y,x + DIAL_BTN_W,y + DIAL_BTN_H);
        if (i == STAR_CHAR_INDEX)
        {
            mainfont = m_padStarFont;
        }
        dialbtnpiece = dialBtnCreate(containerctrl,i,rc,mainfont,m_padSubFont);
        _c(containerpiece)->addContent(containerpiece,(mHotPiece *)dialbtnpiece,
                x, y);
        ncsAddEventListener((mObject*)dialbtnpiece, (mObject*)containerctrl,
                (NCS_CB_ONPIECEEVENT)dialkey_event_handler, NCSN_ABP_CLICKED);
        m_dialBtnPiece[i] = dialbtnpiece;
    }
    return containerctrl;
}

mContainerCtrl* DialPadActivity::titleCreate(HWND main_hwnd,RECT rect)
{
    mContainerCtrl *containerctrl;
    mPanelPiece *toppiece;
    mPanelPiece *containerpiece;

    mHotPiece *backpiece;
    mHotPiece *textpiece;

    RECT rc;
    SetRect(&rc,0,0,RECTW(rect), RECTH(rect));

    containerctrl = (mContainerCtrl*)ncsCreateWindow(
            NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_NONE/*WS_EX_TRANSPARENT*/, IDC_DIAL_TITLE,
            rect.left, rect.top,
            RECTW(rect), RECTH(rect),
            main_hwnd,
            NULL, NULL, NULL, 0);
    if (containerctrl == NULL)
        return NULL;

    toppiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(toppiece)->setRect (toppiece, &rc);

    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(containerpiece)->setRect (containerpiece, &rc);

    //_c(containerctrl)->setBody(containerctrl, (mHotPiece*)containerpiece);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)toppiece);
    _c(toppiece)->addContent(toppiece, (mHotPiece*)containerpiece, 0, 0);

    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &rc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_CORNERFLAG,0);

    _c(backpiece)->setProperty (backpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);

    dial_set_gradient_color((mShapeTransRoundPiece *)backpiece,
            title_gradient_color,title_gradient_pos,TABLESIZE(title_gradient_color));

    _c(containerpiece)->addContent(containerpiece, backpiece, 0, 0);

    textpiece = (mHotPiece*)NEWPIECE(mAnimationEditPiece);
    _c(textpiece)->setRect (textpiece, &rc);
    _c(textpiece)->setProperty (
            textpiece, NCSP_ANIMATIONEDITPIECE_FONT,(DWORD)m_titleBigFont);

    _c(textpiece)->setProperty (
            textpiece, NCSP_ANIMATIONEDITPIECE_TEXTCOLOR,DIAL_TITLE_TEXT_COLOR);
    _c(textpiece)->setProperty (
            textpiece, NCSP_ANIMATIONEDITPIECE_TEXTSHADOWCOLOR,DIAL_TITLE_TEXT_SHADOW_COLOR);
    _c(containerpiece)->addContent(containerpiece, textpiece, 0, 0);

    m_editPiece = (mAnimationEditPiece *)textpiece;

    return containerctrl;
}

static mActivityToolbarPiece *create_toolbar ()
{
    RECT rc = {0, 0, ACTIVITY_W, TOOLBAR_H};
    BITMAP *bmp;
    mActivityToolbarPiece *toolbar;

    toolbar = NEWPIECEEX (mActivityToolbarPiece, 0x7|TOOLBAR_BUTTON_CENTER_HI);

    _c(toolbar)->setRect (toolbar, &rc);

    bmp = (BITMAP*)Load32Resource (PICTURES[TOOLBAR_IMAGE_CALLLOG], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_LEFT, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_LEFT, to_call_log);

    bmp = (BITMAP*)Load32Resource (PICTURES[TOOLBAR_IMAGE_DIAL], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_CENTER, bmp);

    bmp = (BITMAP*)Load32Resource (PICTURES[TOOLBAR_IMAGE_CONTACTS], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    _c(toolbar)->setButtonImage (toolbar, ACTIVITYTOOLBAR_RIGHT, bmp);
    _c(toolbar)->setButtonCallback (toolbar, ACTIVITYTOOLBAR_RIGHT, to_contacts);

    return toolbar;
}

mContainerCtrl* DialPadActivity::toolBarCreate(HWND main_hwnd,RECT rect)
{
    mContainerCtrl *containerctrl;
    mActivityToolbarPiece *toolbar;
    mPanelPiece* body;

    RECT rc;
    SetRect(&rc,0,0,RECTW(rect), RECTH(rect));

    containerctrl = (mContainerCtrl*)ncsCreateWindow(
            NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, WS_EX_NONE/*WS_EX_TRANSPARENT*/, IDC_DIAL_TOOLBAR,
            rect.left, rect.top,
            RECTW(rect), RECTH(rect),
            main_hwnd,
            NULL, NULL, NULL, 0);
    if (containerctrl == NULL)
        return NULL;

    body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    toolbar = create_toolbar ();
    _c(toolbar)->getRect(toolbar, &rc);
    _c(body)->setRect(body, &rc);
    
    _c(body)->addContent(body, (mHotPiece*)toolbar, 0, 0);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)body);

#if 0
    mPanelPiece *containerpiece;
    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &rc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_RADIUS, 0);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    dial_set_gradient_color((mShapeTransRoundPiece *)backpiece,
            toolbar_gradient_color,toolbar_gradient_pos,TABLESIZE(toolbar_gradient_color));
    _c(containerpiece)->addContent(containerpiece,backpiece,0, 0);

    for (i=0;i<DIAL_TOOLBAR_NUM;i++)
    {
        SetRect(&rc,0,0,DIAL_TOOLBAR_W,RECTH(rect) - (DIAL_TOOLBAR_SPACE_H<<1));
        toolbarpiece = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
        _c(toolbarpiece)->setRect (toolbarpiece, &rc);

        toolbarbk = _c(toolbarpiece)->getBkgndPiece(toolbarpiece);
        _c(toolbarbk)->setProperty(toolbarbk,NCSP_TRANROUND_RADIUS,2);
        if (i == 1)
        {
            _c(toolbarpiece)->setGradientBackgroundColor(toolbarpiece,
                toolbarhi_gradient_color,toolbarhi_gradient_pos,TABLESIZE(toolbarhi_gradient_color),
                NULL,NULL,0);
        }
        else
        {
            _c(toolbarpiece)->setGradientBackgroundColor(toolbarpiece,
                toolbar_gradient_color,toolbar_gradient_pos,TABLESIZE(toolbar_gradient_color),
                toolbardn_gradient_color,toolbardn_gradient_pos,TABLESIZE(toolbardn_gradient_color));
        }
        _c(toolbarpiece)->setStates(toolbarpiece);
        _c(containerpiece)->addContent(containerpiece,(mHotPiece *)toolbarpiece,
                DIAL_TOOLBAR_X(i), DIAL_TOOLBAR_SPACE_H);

        imagepiece = (mHotPiece*)NEWPIECE(mImagePiece);
        _c(imagepiece)->setRect (imagepiece, &rc);
        _c(imagepiece)->setProperty (imagepiece,NCSP_IMAGEPIECE_IMAGE,
                (DWORD)RetrieveRes(dialtoolbarbmps[i]));
        _c(toolbarpiece)->addContentToLayout(toolbarpiece, imagepiece);

        ncsAddEventListener((mObject*)toolbarpiece,(mObject*)containerctrl,
                (NCS_CB_ONPIECEEVENT)toolbar_event_handler, NCSN_ABP_CLICKED);
    }
#endif
    return NULL;
}

int DialPadActivity::getDialBtnIndexByPiece(mButtonPanelPiece* piece)
{
    int i;
    for (i=0;i<DIALPAD_BTN_NUM;i++)
    {
        if (m_dialBtnPiece[i] == piece)
            return i;
    }
    return -1;
}
void DialPadActivity::titleNumDisplay(HWND hwnd,const char *instr,char *outstr)
{
    char tmpstr[DIAL_SECOND_LEN_MAX+1] = {0};
    if (m_editPiece == NULL || instr == NULL)
       return; 

    _c(m_editPiece)->getContent(m_editPiece, tmpstr, sizeof(tmpstr));
    if (strlen(tmpstr) >= DIAL_SECOND_LEN_MAX 
            && strcmp(instr,BACKSPACE_STR) != 0
            && strlen(instr) > 0)
    {
        _c(m_editPiece)->setContent(hwnd,m_editPiece,"",TEXT_ALIGN_RIGHT);
    }
    if (strcmp(instr,BACKSPACE_STR) == 0)
    {
        _c(m_editPiece)->tailDelete (hwnd,m_editPiece, 1); 
    }
    else if (strcmp(instr,"") == 0)
    {
    
    }
    else
    {
        _c(m_editPiece)->append (hwnd,m_editPiece, instr);
    }
    if (outstr != NULL)
    {
        memset(tmpstr,0,sizeof(tmpstr));
        _c(m_editPiece)->getContent(m_editPiece, tmpstr, sizeof(tmpstr));
        strcpy(outstr,tmpstr);
    }
}

int DialPadActivity::onPause()
{
#ifdef DEBUG
    fprintf(stderr, "%s:%s:%d, onPause\n", __FILE__, __FUNCTION__, __LINE__);
#endif
    mContainerCtrl *editctrl = (mContainerCtrl*)ncsGetChildObj(this->hwnd(), IDC_DIAL_TITLE);
    _c(m_editPiece)->setContent(editctrl->hwnd,m_editPiece,"",TEXT_ALIGN_RIGHT);
    return 0;
}

int DialPadActivity::onResume()
{
#ifdef DEBUG
    fprintf(stderr, "%s:%s:%d, onResume\n", __FILE__, __FUNCTION__, __LINE__);
#endif
    return 0;
}

void DialPadActivity::initResource()
{
    unsigned int i;
    m_titleBigFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            DIAL_TITLE_FONT_H, 0);
    m_padMainFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            DIAL_PADMAIN_FONT_H, 0);
    m_padSubFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            DIAL_PADSUB_FONT_H, 0);
    m_padStarFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            DIAL_PADSTAR_FONT_H, 0);
    for (i=0;i<TABLESIZE(dialtoolbarbmps);i++)
    {
        if (Load32Resource(dialtoolbarbmps[i],RES_TYPE_IMAGE,0) == NULL)
        {
            printf("bmp load failure!%s\n",dialtoolbarbmps[i]);
        }
    }
    for (i=0;i<TABLESIZE(dialpadbmps);i++)
    {
        if (Load32Resource(dialpadbmps[i],RES_TYPE_IMAGE,0) == NULL)
        {
            printf("bmp load failure!%s\n",dialpadbmps[i]);
        }
    }
    return;
}

void DialPadActivity::releaseResource()
{
    unsigned int i;
    for (i=0;i<TABLESIZE(dialtoolbarbmps);i++)
    {
        UnregisterRes(dialtoolbarbmps[i]);
    }
    for (i=0;i<TABLESIZE(dialpadbmps);i++)
    {
        UnregisterRes(dialpadbmps[i]);
    }
    DestroyLogFont(m_titleBigFont);
    DestroyLogFont(m_padMainFont);
    DestroyLogFont(m_padSubFont);
    DestroyLogFont(m_padStarFont);
    return;
}

static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData )
{

    RECT rect_title = {0,0,DIAL_TITLE_W,DIAL_TITLE_H};
    RECT rect_toolbar = {0,DIAL_TOOLBAR_Y,ACTIVITY_W,ACTIVITY_H};
    RECT rect_dialpad = {0,DIAL_TITLE_H,ACTIVITY_W,DIAL_TOOLBAR_Y};

    DialPadActivity* act = 
        (DialPadActivity*)Activity::getActivityFromHWND (self->hwnd);
    assert(act);

    act->titleCreate(self->hwnd,rect_title);
    act->toolBarCreate(self->hwnd,rect_toolbar);
    act->dialPadCreate(self->hwnd,rect_dialpad);
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

static void main_onUserApp (mWidget* _this,int message, WPARAM wparam, LPARAM lparam)
{
    return;
}

static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}
static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE, reinterpret_cast<void*>(main_onCreate)},
    {MSG_USER_APP_DATA, reinterpret_cast<void*>(main_onUserApp)},
    {MSG_CLOSE, reinterpret_cast<void*>(main_onClose)},
    {MSG_PAINT, reinterpret_cast<void*>(main_onPaint)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {0, NULL}
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_NONE,
        "Phone Contacts",
        NULL,
        NULL,
        main_handlers,
        NULL,
        0, 
        0,
        0, 0,
};
DialPadActivity::DialPadActivity() :
    NCSActivity( &mymain_templ)
{ 
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_INIT_CLASS(mAnimationEditPiece);
    MGNCS_INIT_CLASS(mLayoutManager);
    MGNCS_INIT_CLASS(mCenterHBox);
    MGNCS_INIT_CLASS(mButtonPanelPiece);
    MGNCS_INIT_CLASS(mTextPiece);

    MGNCS_INIT_CLASS(mActivityToolbarPiece);

    //m_style = STYLE_PUSH;
    m_pushIndex = 2;

    m_editPiece = NULL;
    m_intent = Intent::newIntent();

    initResource();
}

DialPadActivity::~DialPadActivity(void) 
{
    Intent::deleteIntent(m_intent);
    releaseResource();
}

static BOOL to_call_log (mObject *self, mObject *piece, int event_id,  DWORD param)
{
    // ACTIVITYSTACK->push("CallHistoryActivity", NULL);
    ACTIVITYSTACK->switchTo("CallHistoryActivity", NULL);
    return TRUE;
}

static BOOL to_contacts (mObject *self, mObject *piece, int event_id,  DWORD param)
{
    // ACTIVITYSTACK->push("PhoneContactsActivity", NULL);
    ACTIVITYSTACK->switchTo("PhoneContactsActivity", NULL);
    return TRUE;
}

