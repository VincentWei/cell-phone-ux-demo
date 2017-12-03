#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "register.h"
#include "mslideswitchpiece.h"

#define ANIM_RUNTIME 300
#define OFFSET_X 2
#define OFFSET_Y 2

static void setBkgndPiece(mHotPiece *piece, int border, int cornerFlag,
        mColorTable *colorTable, mColorTable *borderColorTable)
{
    HBRUSH brush;
    mShapeTransRoundPiece *bk = (mShapeTransRoundPiece*)piece;

    if (cornerFlag >= 0)
        _c(bk)->setProperty(bk, NCSP_TRANROUND_CORNERFLAG, cornerFlag);

    if (border >= 0) {
        _c(bk)->setProperty(bk, NCSP_TRANROUND_BORDERSIZE, border);
        _c(bk)->setProperty(bk, NCSP_TRANROUND_GRADIENTBORDER, TRUE);
    }

    if (colorTable) {
        _c(bk)->setProperty(bk, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        brush = _c(bk)->getBrush(bk);
        ShapeTransRoundPiece_setBrushGradientColors(brush, colorTable);
    }

    if (borderColorTable) {
        brush = _c(bk)->getBorderBrush(bk);
        ShapeTransRoundPiece_setBrushGradientColors(brush, borderColorTable);
    }
}

static void setTextPiece(mHotPiece *txt, const char *str, PLOGFONT font, ARGB color)
{
    if (str) _c(txt)->setProperty (txt, NCSP_LABELPIECE_LABEL, (DWORD)str);
    if (font) _c(txt)->setProperty (txt, NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
    _c(txt)->setProperty (txt, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)color);
}

static void setChildPieces(mSlideSwitchPiece *self)
{
    ARGB color_l[] = {0xff4d6d86, 0xff4b6f87, 0xff3880b0, 0xff57a2cc, 0xff7da9c4, 0xff7da9c4};
    ARGB color_r[] = {0xffcc2222, 0xffdd2222, 0xffff2222, 0xffff2222, 0xffdd2222, 0xffcc2222};
    float pos[] = {0.0, 0.05, 0.18, 0.95, 0.96, 1.0};
    ARGB color_b[] = {0xffffffff, 0xffffffff, 0x88ffffff, 0xffffffff};
    float pos_b[] = {0.00, 0.05, 0.9, 1.0};

    mColorTable colorTable_l, colorTable_r, colorTable_b;

    ShapeTransRoundPiece_setColorTable(&colorTable_l, color_l, pos, TABLESIZE(color_l));
    ShapeTransRoundPiece_setColorTable(&colorTable_r, color_r, pos, TABLESIZE(color_r));
    ShapeTransRoundPiece_setColorTable(&colorTable_b, color_b, pos_b, TABLESIZE(color_b));

    /* left */
    setBkgndPiece(self->leftBackgroundPiece, 0,
            TRANROUND_CORNERFLAG_LEFTTOP | TRANROUND_CORNERFLAG_LEFTBOTTOM,
            &colorTable_l, NULL);

    setTextPiece (self->leftTextPiece, "ON", NULL, 0xFFFFFFFF);

    /* right */
    setBkgndPiece(self->rightBackgroundPiece, 0,
            TRANROUND_CORNERFLAG_RIGHTTOP | TRANROUND_CORNERFLAG_RIGHTBOTTOM,
            &colorTable_r, NULL);

    setTextPiece (self->rightTextPiece, "OFF", NULL, 0xFFFFFFFF);

    /* button */
    setBkgndPiece(self->buttonPiece, 0, -1, &colorTable_b, NULL);
    _c(self->buttonPiece)->setProperty(self->buttonPiece, NCSP_TRANROUND_USESHADOW, TRUE);
}

static void mSlideSwitchPiece_construct(mSlideSwitchPiece *self, DWORD add_data)
{

    Class(mPanelPiece).construct ((mPanelPiece *)self, add_data);

    self->state = NCSN_SLIDESWITCHPIECE_OFF;
    self->radius = 0;
 
    /* left */
    self->leftBackgroundPiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    self->leftTextPiece = (mHotPiece*)NEWPIECE(mTextPiece);

    /* right */
    self->rightBackgroundPiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    self->rightTextPiece = (mHotPiece*)NEWPIECE(mTextPiece);

    /* button */
    self->buttonPiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);

    /* set piece property */
    setChildPieces(self);

    _c(self)->addContent(self, self->leftBackgroundPiece, 0, 0);
    _c(self)->addContent(self, self->leftTextPiece, 0, 0);
    _c(self)->addContent(self, self->rightBackgroundPiece, 0, 0);
    _c(self)->addContent(self, self->rightTextPiece, 0, 0);
    _c(self)->addContent(self, self->buttonPiece, 0, 0);
}

static int mSlideSwitchPiece_getState(mSlideSwitchPiece *self)
{
    return self->state;
}

static void pushingAnimation(mSlideSwitchPiece *self, POINT *pt_e)
{
    mPanelPiece *topPanel = PanelPiece_getTopPanel((mHotPiece *)self);

//    MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
//    mGEffAnimationAddToGroup(group, 
//    _c(self)->movePieceWithAnimation(self, self->leftBackgroundPiece, endValue->x, endValue->y));
    MGEFF_ANIMATION anim = _c(self)->movePieceWithAnimation(self,
            self->buttonPiece, pt_e->x, pt_e->y, ANIM_RUNTIME, Linear);

    _c(topPanel)->animationAsyncRun(topPanel, anim, 0);
}

static void setStateWithAniamtion(mSlideSwitchPiece *self, enum mSlideSwitchPieceState state)
{
    if (self->state == state) return;

    self->state = state;

    if (state == NCSN_SLIDESWITCHPIECE_ON) {
        RECT rc;
        POINT pt_e = {0, 0};
        _c(self)->getRect(self, &rc);
        pt_e.x = RECTW(rc) / 2 - OFFSET_X;
        pushingAnimation(self, &pt_e);
    } else if (state == NCSN_SLIDESWITCHPIECE_OFF) {
        POINT pt_e = {0, 0};
        pushingAnimation(self, &pt_e);
    }
}

static void mSlideSwitchPiece_setState(mSlideSwitchPiece *self, enum mSlideSwitchPieceState state)
{
    mPieceItem *item;
    if (self->state == state) return;

    self->state = state;

    item = _c(self)->searchItem(self, (mHotPiece *)self->buttonPiece);
    assert(item);
    if (state == NCSN_SLIDESWITCHPIECE_ON) {
        RECT rc;
        _c(self)->getRect(self, &rc);
        item->x = RECTW(rc) / 2 - OFFSET_X;
        item->y = 0;
    } else if (state == NCSN_SLIDESWITCHPIECE_OFF) {
        item->x = item->y = 0;
    }
}

static void mSlideSwitchPiece_changeState(mSlideSwitchPiece *self)
{
    enum mSlideSwitchPieceState state = 
        (self->state == NCSN_SLIDESWITCHPIECE_OFF ? NCSN_SLIDESWITCHPIECE_ON : NCSN_SLIDESWITCHPIECE_OFF);

    setStateWithAniamtion(self, state);
}

static void mSlideSwitchPiece_resetState(mSlideSwitchPiece *self)
{
    setStateWithAniamtion(self, NCSN_SLIDESWITCHPIECE_OFF);
}

static int mSlideSwitchPiece_processMessage(mSlideSwitchPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK) {
//        mHotPiece *hotPiece = _c(self)->childAt(self, LOSWORD(lParam), HISWORD(lParam));
        if (message == MSG_LBUTTONUP) {
            //if (hotPiece == self->leftTextPiece) {
            if (self->state == NCSN_SLIDESWITCHPIECE_ON) {
                setStateWithAniamtion(self, NCSN_SLIDESWITCHPIECE_OFF);
                ncsRaiseEvent((mObject*)self, self->state, 0);
            //} else if (hotPiece == self->rightTextPiece) {
            } else if (self->state == NCSN_SLIDESWITCHPIECE_OFF) {
                setStateWithAniamtion(self, NCSN_SLIDESWITCHPIECE_ON);
                ncsRaiseEvent((mObject*)self, self->state, 0);
            }
        }
    }

    return 0;
}

static BOOL mSlideSwitchPiece_setProperty(mSlideSwitchPiece * self, int id, DWORD value)
{
    switch (id) {
        case NCSP_SLIDESWITCH_RADIUS:
            self->radius = (int)value;
            _c(self->leftBackgroundPiece)->setProperty(self->leftBackgroundPiece, NCSP_TRANROUND_RADIUS, self->radius);
            _c(self->rightBackgroundPiece)->setProperty(self->rightBackgroundPiece, NCSP_TRANROUND_RADIUS, self->radius);
            _c(self->buttonPiece)->setProperty(self->buttonPiece, NCSP_TRANROUND_RADIUS, self->radius);
            break;
        default:
            return Class(mPanelPiece).setProperty((mPanelPiece *)self, id, value);
    }
    return TRUE;
}

static DWORD mSlideSwitchPiece_getProperty(mSlideSwitchPiece *self, int id)
{
    switch (id) {
        case NCSP_SLIDESWITCH_RADIUS:
            return (DWORD)self->radius;
        default:
            return Class(mPanelPiece).getProperty((mPanelPiece *)self, id);
    }
}

static BOOL mSlideSwitchPiece_setRect(mSlideSwitchPiece *self, const RECT *prc)
{
    RECT rc;
    mPieceItem *item;
    BOOL ret = Class(mPanelPiece).setRect((mPanelPiece *)self, prc);

    if (ret) {
        _c(self)->setProperty(self, NCSP_SLIDESWITCH_RADIUS, RECTHP(prc) / 4);

        SetRect(&rc, 0, 0, RECTWP(prc) / 2 - OFFSET_X, RECTHP(prc) - OFFSET_Y * 2);
        _c(self->leftBackgroundPiece)->setRect(self->leftBackgroundPiece, &rc);
        if ((item = _c(self)->searchItem(self, self->leftBackgroundPiece))) {
            item->x = OFFSET_X, item->y = OFFSET_Y;
        }
        _c(self->leftTextPiece)->setRect(self->leftTextPiece, &rc);
        if ((item = _c(self)->searchItem(self, self->leftTextPiece))) {
            item->x = OFFSET_X, item->y = OFFSET_Y;
        }

        _c(self->rightBackgroundPiece)->setRect(self->rightBackgroundPiece, &rc);
        if ((item = _c(self)->searchItem(self, self->rightBackgroundPiece))) {
            item->x = RECTWP(prc) / 2, item->y = OFFSET_Y;
        }
        _c(self->rightTextPiece)->setRect(self->rightTextPiece, &rc);
        if ((item = _c(self)->searchItem(self, self->rightTextPiece))) {
            item->x = RECTWP(prc) / 2, item->y = OFFSET_Y;
        }

        SetRect(&rc, 0, 0, RECTWP(prc) / 2 + OFFSET_X, RECTHP(prc));
        _c(self->buttonPiece)->setRect(self->buttonPiece, &rc);
        if ((item = _c(self)->searchItem(self, self->buttonPiece))) {
            item->x = 0, item->y = 0;
        }
    }

    return ret;
}

static void mSlideSwitchPiece_setLeftBkgnd(mSlideSwitchPiece *self, int border,
        int cornerFlag, mColorTable *colorTable, mColorTable *borderColorTable)
{
    setBkgndPiece(self->leftBackgroundPiece, border, cornerFlag,
            colorTable, borderColorTable);
}

static void mSlideSwitchPiece_setRightBkgnd(mSlideSwitchPiece *self, int border,
        int cornerFlag, mColorTable *colorTable, mColorTable *borderColorTable)
{
    setBkgndPiece(self->rightBackgroundPiece, border, cornerFlag,
            colorTable, borderColorTable);
}

static void mSlideSwitchPiece_setButton(mSlideSwitchPiece *self, int border,
        int cornerFlag, mColorTable *colorTable, mColorTable *borderColorTable)
{
    setBkgndPiece(self->buttonPiece, border, cornerFlag,
            colorTable, borderColorTable);
}

static void mSlideSwitchPiece_setLeftText(mSlideSwitchPiece *self,
        const char *str, PLOGFONT font, ARGB color)
{
    setTextPiece(self->leftTextPiece, str, font, color);
}

static void mSlideSwitchPiece_setRightText(mSlideSwitchPiece *self,
        const char *str, PLOGFONT font, ARGB color)
{
    setTextPiece(self->rightTextPiece, str, font, color);
}

BEGIN_MINI_CLASS(mSlideSwitchPiece, mPanelPiece)
    CLASS_METHOD_MAP(mSlideSwitchPiece, construct)
    CLASS_METHOD_MAP(mSlideSwitchPiece, getState)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setState)
    CLASS_METHOD_MAP(mSlideSwitchPiece, changeState)
    CLASS_METHOD_MAP(mSlideSwitchPiece, resetState)
    CLASS_METHOD_MAP(mSlideSwitchPiece, processMessage)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setProperty)
    CLASS_METHOD_MAP(mSlideSwitchPiece, getProperty)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setRect)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setLeftBkgnd)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setRightBkgnd)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setButton)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setLeftText)
    CLASS_METHOD_MAP(mSlideSwitchPiece, setRightText)
END_MINI_CLASS
