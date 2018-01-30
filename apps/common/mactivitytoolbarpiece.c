#include <string.h>
#include <assert.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "global.h"
#include "mactivitytoolbarpiece.h"

#define TOOLBAR_SPACE_W 16
#define TOOLBAR_SPACE_H 4
#define TOOLBARFONT_SIZE  25

#if 0
static ARGB  toolbar_color_normal [] = {0xff333333, 0xff343434, 0xff444444, 0xff575757};
static float toolbar_color_positions_normal[] = {0.0, 0.50, 0.49, 1.0};
#else
static ARGB  toolbar_color_normal[] = {0xff0f1700, 0xff434343, 0xff2d2d2d, 0xff151515
,0xff010101,0xff000000};
static float toolbar_color_positions_normal[] = {0.0,0.01, 0.01, 0.49, 0.51,1.0};
#endif
static ARGB  toolbar_color_normal1[] = {0xff0f1700, 0xff434343, 0xff2d2d2d, 0xff151515
,0xff010101,0xff000000};
static float toolbar_color_positions_normal1[] = {0.0,0.01, 0.01, 0.49, 0.51,1.0};
static ARGB toolbar_color_pressed [] = { 0x7f7f7f7f,0x7f7f7f7f };
static float toolbar_color_positions_pressed [] = { 0.0,1.0 };

static void setting_gradient_color(mShapeTransRoundPiece *piece, ARGB *colors, float *pos, int num)
{
    HBRUSH brush;
    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    brush = _c(piece)->getBrush(piece);
    MGPlusSetLinearGradientBrushColorsEx(brush,colors,num,pos);
}

static void create_pressed_state (mButtonPanelPiece *btn_piece,BOOL value,DWORD add_data)
{
    mShapeTransRoundPiece* bkgnd = _c(btn_piece)->getBkgndPiece(btn_piece);
    _c(bkgnd)->setProperty(bkgnd,NCSP_TRANROUND_RADIUS,2);
    if (!value)
    {
        if ((add_data & TOOLBAR_SELECT_COLOR) == TOOLBAR_SELECT_COLOR)
        {
            _c(btn_piece)->setGradientBackgroundColor(btn_piece,
                    toolbar_color_normal1, toolbar_color_positions_normal1, TABLESIZE(toolbar_color_normal1),
                    toolbar_color_pressed, toolbar_color_positions_pressed, TABLESIZE(toolbar_color_pressed));
        }
        else
        {
            _c(btn_piece)->setGradientBackgroundColor(btn_piece,
                    toolbar_color_normal, toolbar_color_positions_normal, TABLESIZE(toolbar_color_normal),
                    toolbar_color_pressed, toolbar_color_positions_pressed, TABLESIZE(toolbar_color_pressed));
        }
    }
    else
    {
        _c(btn_piece)->setGradientBackgroundColor(btn_piece,
                toolbar_color_pressed, toolbar_color_positions_pressed, TABLESIZE(toolbar_color_pressed),
                toolbar_color_pressed, toolbar_color_positions_pressed, TABLESIZE(toolbar_color_pressed));
    }
}

static void mActivityToolbarPiece_construct(mActivityToolbarPiece* self, DWORD add_data)
{
    mButtonPanelPiece *button;
    Class(mPanelPiece).construct((mPanelPiece*)self, 0);

    self->font = NULL;
    memset (self->button_name, 0, sizeof(self->button_name));

    /* add back ground */
    self->background = NEWPIECE (mShapeTransRoundPiece); 
    _c(self->background)->setProperty (self->background, NCSP_TRANROUND_CORNERFLAG, 0);
    if ((add_data & TOOLBAR_SELECT_COLOR) == TOOLBAR_SELECT_COLOR)
    {
        setting_gradient_color (self->background, toolbar_color_normal1, 
                toolbar_color_positions_normal1, TABLESIZE(toolbar_color_normal1));
    }
    else
    {
        setting_gradient_color (self->background, toolbar_color_normal, 
                toolbar_color_positions_normal, TABLESIZE(toolbar_color_normal));
    }
    _c(self)->addContent(self, (mHotPiece*)self->background, 0, 0);

    /* add buttons */
    memset (self->buttons, 0, sizeof(self->buttons));
    if (add_data & TOOLBAR_BUTTON_LEFT ) {
        button = NEWPIECE (mButtonPanelPiece);
        _c(self)->addContent(self, (mHotPiece*)button, 0, 0);
        create_pressed_state (button,
                (add_data&TOOLBAR_BUTTON_LEFT_HI) == TOOLBAR_BUTTON_LEFT_HI,add_data);
        _c(button)->setStates(button);
        self->buttons [ACTIVITYTOOLBAR_LEFT] = button;
    }

    if (add_data & TOOLBAR_BUTTON_CENTER ) {
        button = NEWPIECE (mButtonPanelPiece);
        _c(self)->addContent(self, (mHotPiece*)button, ACTIVITY_W/3 + (3 - ACTIVITY_W/3%3), 0);
        create_pressed_state (button,
                (add_data&TOOLBAR_BUTTON_CENTER_HI) == TOOLBAR_BUTTON_CENTER_HI,add_data);
        _c(button)->setStates(button);
        self->buttons [ACTIVITYTOOLBAR_CENTER] = button;
    }

    if (add_data & TOOLBAR_BUTTON_RIGHT) {
        button = NEWPIECE (mButtonPanelPiece);
        if (add_data & TOOLBAR_BUTTON_CENTER) {
            _c(self)->addContent(self, (mHotPiece*)button, (ACTIVITY_W/3 + (3 - ACTIVITY_W/3%3))*2, 0);
        } else {
            _c(self)->addContent(self, (mHotPiece*)button, ACTIVITY_W/2, 0);
        }
        create_pressed_state (button,
                (add_data&TOOLBAR_BUTTON_RIGHT_HI) == TOOLBAR_BUTTON_RIGHT_HI,add_data);
        _c(button)->setStates(button);
        self->buttons [ACTIVITYTOOLBAR_RIGHT] = button;
    }
}

void mActivityToolbarPiece_setButtonCallback(mActivityToolbarPiece *self, int which_btn, NCS_CB_ONPIECEEVENT cb)
{
    mButtonPanelPiece *button;
    if (which_btn > ACTIVITYTOOLBAR_MAX || which_btn < 0) {
        printf ("ActivityToolbar button id error! id: %d\n", which_btn);
        assert (0);
    }
    button = self->buttons[which_btn];

    ncsAddEventListener ((mObject*)button, (mObject*)button, cb, NCSN_ABP_CLICKED);
}


void mActivityToolbarPiece_changeButtonImage(mActivityToolbarPiece *self, int which_btn, BITMAP *bmp)
{
    mImagePiece* img_piece;
    //mButtonPanelPiece *button;
    RECT rc;

    //button = self->buttons[which_btn];
    img_piece = self->button_image[which_btn];
    _c(img_piece)->setProperty(img_piece, NCSP_IMAGEPIECE_IMAGE, (DWORD)bmp);
    _c(img_piece)->getRect(img_piece,&rc);
    PanelPiece_invalidatePiece((mHotPiece*)img_piece,&rc);
}

void mActivityToolbarPiece_setButtonImage(mActivityToolbarPiece *self, int which_btn, BITMAP *bmp)
{
    RECT rc;
    mButtonPanelPiece *button;
    mImagePiece* img_piece;

    if (which_btn > ACTIVITYTOOLBAR_MAX || which_btn < 0) {
        printf ("ActivityToolbar button id error! id: %d\n", which_btn);
        assert (0);
    }
    button = self->buttons[which_btn];

    if (!button) {
        return;
    }

    img_piece = NEWPIECE(mImagePiece);
    SetRect(&rc, 0, 0, bmp->bmWidth+8, bmp->bmHeight+2);
    _c(img_piece)->setRect(img_piece, &rc);
    _c(img_piece)->setProperty(img_piece, NCSP_IMAGEPIECE_IMAGE, (DWORD)bmp);

    _c(button)->addContentToLayout(button, (mHotPiece*)img_piece);
    //_c(button)->addContent(button, (mHotPiece*)img_piece, 0, 0);

    self->button_image[which_btn] = img_piece;
}

static BOOL mActivityToolbarPiece_setRect(mActivityToolbarPiece *self, const RECT *prc)
{
    int i;
    RECT rc = {0, 0, RECTWP(prc)/3, RECTHP (prc)};

    Class(mPanelPiece).setRect((mPanelPiece*)self, prc);
    _c(self->background)->setRect (self->background, prc);

    if (!self->buttons[ACTIVITYTOOLBAR_CENTER]) {
        SetRect (&rc, 0, 0, RECTWP(prc)/2, RECTHP (prc));
    }

    for (i=0; i<ACTIVITYTOOLBAR_MAX; ++i) {
        if (self->buttons[i]) {
            _c(self->buttons[i])->setRect (self->buttons[i], &rc);
        }

        if (self->button_name [i]) {
            _c(self->button_name[i])->setRect (self->button_name[i], &rc);
        }
    }

    return TRUE;
}

static void reset_rect_for_text_piece (mActivityToolbarPiece *self)
{
    RECT rc;

    _c(self)->getRect (self, &rc);

    if (RECTW (rc) && RECTH (rc)) {
        _c(self)->setRect (self, &rc);
    }
}

void mActivityToolbarPiece_setButtonName(mActivityToolbarPiece *self, int which_btn, const char *name)
{
    mTextPiece *button_name;
    mButtonPanelPiece *button = self->buttons[which_btn];

    if (which_btn > ACTIVITYTOOLBAR_MAX || which_btn < 0) {
        printf ("ActivityToolbar button id error! id: %d\n", which_btn);
        assert (0);
    }
    if (!self->font) {
        self->font = CreateLogFont ("ttf", "helvetica", "GB2312",
                FONT_WEIGHT_BOOK,
                FONT_SLANT_ROMAN,
                FONT_SETWIDTH_NORMAL,
                FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                TOOLBARFONT_SIZE, 0);
    }


    button_name = NEWPIECE (mTextPiece);
    
    _c(button_name)->setProperty (button_name, NCSP_TEXTPIECE_LOGFONT, (DWORD)self->font);
    _c(button_name)->setProperty (button_name, NCSP_LABELPIECE_LABEL, (DWORD)name);
    _c(button_name)->setProperty (button_name, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_CENTER);
    _c(button_name)->setProperty (button_name, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xffffffff);

    _c(button)->addContent(button, (mHotPiece*)button_name, 0, 0);

    reset_rect_for_text_piece (self);
     
    /* save button name piece */
    self->button_name[which_btn] = button_name;
}

static void mActivityToolbarPiece_destroy (mActivityToolbarPiece *self)
{
    if (self->font) {
        DestroyLogFont (self->font);
        self->font = NULL;
    }

    Class(mPanelPiece).destroy((mPanelPiece*)self);
}

static BOOL mActivityToolbarPiece_setProperty (mActivityToolbarPiece *self, int id, DWORD value)
{
    int i;
    if (id == NCSN_ACTIVITY_TOOLBAR_BUTTON_STRATEGY){

        if (value == TOOLBAR_DISPLAY_NAME) {
            for (i=0; i<ACTIVITYTOOLBAR_MAX; ++i) {
                if (self->button_image[i]) {
                    _c(self->buttons[i])->delContent (self->buttons[i], (mHotPiece*)self->button_image[i]);
                }
            }
            _c(self)->reLayout(self);
        } else if (value == TOOLBAR_DISPLAY_PICTURE ) {
            for (i=0; i<ACTIVITYTOOLBAR_MAX; ++i) {
                if (self->button_name[i]) {
                    _c(self->buttons[i])->delContent (self->buttons[i], (mHotPiece*)self->button_name[i]);
                }
            }
            _c(self)->reLayout(self);
        } else {
            printf ("Not support now!\n");
        }
        return TRUE;
    }

    return Class(mPanelPiece).setProperty ((mPanelPiece*)self, id, value);
}

BEGIN_MINI_CLASS(mActivityToolbarPiece, mPanelPiece)
    CLASS_METHOD_MAP(mActivityToolbarPiece, construct)
    CLASS_METHOD_MAP(mActivityToolbarPiece, destroy)
    CLASS_METHOD_MAP(mActivityToolbarPiece, setRect)
    CLASS_METHOD_MAP(mActivityToolbarPiece, setButtonCallback)
    CLASS_METHOD_MAP(mActivityToolbarPiece, setButtonImage)
    CLASS_METHOD_MAP(mActivityToolbarPiece, changeButtonImage)
    CLASS_METHOD_MAP(mActivityToolbarPiece, setProperty)
    CLASS_METHOD_MAP(mActivityToolbarPiece, setButtonName)
END_MINI_CLASS

