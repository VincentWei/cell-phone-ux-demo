#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>

#include "global.h"
#include "register.h"
#include "mitempiece.h"

#define TITLEFONT_SIZE        18
#define TITLEFONT_COLOR       0xFF192952
#define TITLE_WIDTH           300
#define TITLE_HEIGHT          24
#define TITLE_MAXLEN          /*180*/(ACTIVITY_W>>1)

#define SUBTITLEFONT_SIZE     12
#define SUBTITLEFONT_COLOR    0xFF0000FF
#define SUBTITLE_WIDTH        300
#define SUBTITLE_HEIGHT       5
#define SUBTITLE_MAXLEN       200

#define DETAILFONT_SIZE       12
#define DETAILFONT_COLOR      0xFF1882bd
#define DETAIL_WIDTH          300
#define DETAIL_HEIGHT         24

#define SUBSTANCE_WIDTH       /*280*/(ACTIVITY_W - 80)

#define INDICATOR_WIDTH    24
#define INDICATOR_HEIGHT   24

#define CHECKMARK_WIDTH    24
#define CHECKMARK_HEIGHT   24

#define RADIO_WIDTH        24
#define RADIO_HEIGHT       24

#define SWITCH_WIDTH       69
#define SWITCH_HEIGHT      24

/* locale function */
static PLOGFONT createLogFont (unsigned size);
static void setChildOffset (mItemPiece *self, mHotPiece *piece, int *px, int *py);
static void reviseTitleOffset (mItemPiece *self, int subTitleOffset, int subTitleAlign);
static void setImage (mImagePiece *img, PBITMAP pbmp, int w, int h);
static void resetRectOfText (mItemPiece *self, mTextPiece *txt, PLOGFONT font);
static void setText (mItemPiece *self, mTextPiece *txt, const char *text, PLOGFONT font, ARGB *pColor);

/* function */
static void mItemPiece_construct (mItemPiece *self, DWORD add_data)
{
    Class(mPanelPiece).construct ((mPanelPiece *)self, add_data);

    //self->privateDC      = CreateCompatibleDCEx (HDC_SCREEN, 1, 1);
    //self->privateDC      = Get32MemDC();

    self->state          = ITEMPIECE_STATE_NORMAL;

    self->childrenTotal  = 0;
    /* default align */
    self->align          = NCS_ALIGN_LEFT;
    self->valign         = NCS_VALIGN_CENTER;

    self->margin         = 6;
    self->border         = 10;
    self->clearance      = 10;

    self->offsetLeft     = self->border + self->margin;
    self->offsetRight    = ITEMPIECE_WIDTH - (self->border + self->margin);

    self->imagePiece     = NULL;

    /* for title */
    /* for content */
    self->isTitlePiece   = TRUE;

    self->titlePiece     = NULL;
    self->defaultTitleFont      = createLogFont (TITLEFONT_SIZE);
    self->titleFont      = self->defaultTitleFont;
    self->titleColor     = TITLEFONT_COLOR;

    /* for subtitle */
    self->subTitlePiece  = NULL;
    self->defaultSubTitleFont   = createLogFont (SUBTITLEFONT_SIZE);
    self->subTitleFont   = self->defaultSubTitleFont;
    self->subTitleColor  = SUBTITLEFONT_COLOR;

    /* for detail */
    self->detailPiece    = NULL;
    self->defaultDetailFont     = createLogFont (DETAILFONT_SIZE);
    self->detailFont     = self->defaultDetailFont;
    self->detailColor    = DETAILFONT_COLOR;

    /* accessory control */
    self->accessoryControlPiece = NULL;

    self->acStyle        = -1;

    memset (&self->childAlign, NCS_ALIGN_LEFT, sizeof(self->childAlign));

    /* add bkPiece */
    self->bkPiece        = NEWPIECE (mShapeTransRoundPiece);
    self->bkColor        = 0x00000000;

    _c(self->bkPiece)->setProperty (self->bkPiece, NCSP_TRANROUND_BKCOLOR, (ARGB)self->bkColor);

    _c(self)->addContent (self, (mHotPiece *)(self->bkPiece), 0, 0);

    self->childAlign[self->childrenTotal++] = self->align;
}

static void mItemPiece_destroy (mItemPiece *self)
{
    DestroyLogFont (self->defaultTitleFont);
    DestroyLogFont (self->defaultSubTitleFont);
    DestroyLogFont (self->defaultDetailFont);

    //DeleteMemDC (self->privateDC);

    Class(mPanelPiece).destroy ((mPanelPiece *)self);
}

static void mItemPiece_setLayout (mItemPiece *self, int alignStyle)
{
    self->align = alignStyle;
}

static void mItemPiece_setImage (mItemPiece *self, PBITMAP pbmp, int w, int h)
{
    mImagePiece *img = NULL;

    if (self->imagePiece != NULL) {
        setImage (self->imagePiece, pbmp, w, h);
        return;
    }

    img = NEWPIECE (mImagePiece);

    self->imagePiece = img;

    _c(self)->addContent (self, (mHotPiece *)img, 0, 0);

    self->childAlign[self->childrenTotal++] = self->align;

    setImage (img, pbmp, w, h);
}

static void mItemPiece_setTitle (mItemPiece *self, const char *text, PLOGFONT font, ARGB *pColor)
{
    mTextPiece *txt = NULL;

    self->titleFont = font ? font : self->defaultTitleFont;
    self->titleColor = pColor ? *pColor : self->titleColor;

    if (self->titlePiece != NULL) {
        setText (self, self->titlePiece, text, self->titleFont, &(self->titleColor));
        return;
    }

    /* if titlePiece is null */
    txt = NEWPIECE (mTextPiece);

    self->titlePiece = txt;

    if (self->isTitlePiece) {
        if (self->accessoryControlPiece == NULL)
        {
            _c(txt)->setProperty (txt, NCSP_TEXTPIECE_MAXLEN, TITLE_MAXLEN);
        }
        else
        {
            _c(txt)->setProperty (txt, NCSP_TEXTPIECE_MAXLEN, TITLE_MAXLEN+80);
        }

        _c(txt)->setProperty (txt, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);
    }
    else {
        _c(txt)->setProperty (txt, NCSP_LABELPIECE_WORDBREAK, 1);

        _c(txt)->setProperty (txt, NCSP_LABELPIECE_AUTOWRAP, 1);

        _c(txt)->setProperty (txt, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);
    }

    _c(self)->addContent (self, (mHotPiece *)txt, 0, 0);

    self->childAlign[self->childrenTotal++] = self->align;

    setText (self, txt, text, self->titleFont, &(self->titleColor));
}

static void mItemPiece_setSubTitle (mItemPiece *self, const char *text, PLOGFONT font, ARGB *pColor)
{
    mTextPiece *txt = NULL;

    self->subTitleFont = font ? font : self->defaultSubTitleFont;
    self->subTitleColor = pColor ? *pColor : self->subTitleColor;

    if (self->subTitlePiece != NULL) {
        setText (self, self->subTitlePiece, text, self->subTitleFont, &(self->subTitleColor));
        return;
    }

    /* if subTitlePiece is null */
    txt = NEWPIECE (mTextPiece);

    self->subTitlePiece = txt;

    _c(txt)->setProperty (txt, NCSP_TEXTPIECE_MAXLEN, SUBTITLE_MAXLEN);

    _c(txt)->setProperty (txt, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);

    _c(self)->addContent (self, (mHotPiece *)txt, 0, 0);

    self->childAlign[self->childrenTotal++] = self->align;

    setText (self, txt, text, self->subTitleFont, &(self->subTitleColor));
}

static void mItemPiece_setDetail (mItemPiece *self, const char *text, PLOGFONT font, ARGB *pColor)
{
    mTextPiece *txt = NULL;

    self->detailFont = font ? font : self->defaultDetailFont;
    self->detailColor = pColor ? *pColor : self->detailColor;

    if (self->detailPiece != NULL) {
        setText (self, self->detailPiece, text, self->detailFont, &(self->detailColor));
        return;
    }

    /* if detailPiece is null */
    txt = NEWPIECE (mTextPiece);

    self->detailPiece = txt;

    _c(txt)->setProperty (txt, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_RIGHT);

    _c(self)->addContent (self, (mHotPiece *)txt, 0, 0);

    self->childAlign[self->childrenTotal++] = self->align;

    setText (self, txt, text, self->detailFont, &(self->detailColor));
}

static void mItemPiece_setSubStance (mItemPiece *self, char *text, PLOGFONT font, ARGB *pColor)
{
    self->isTitlePiece = FALSE;

    _c(self)->setTitle (self, text, font, pColor);
}

static BOOL onSwitchStateChanged(mItemPiece *self, mSlideSwitchPiece *ac, int event_id, DWORD param)
{
    if (event_id == NCSN_SLIDESWITCHPIECE_OFF) {
        ncsRaiseEvent ((mObject*)self, NCSN_ITEMPIECE_CHECKBOX_OFF, ((mPanelPiece *)self)->addData);
        return TRUE;
    }
    else if (event_id == NCSN_SLIDESWITCHPIECE_ON) {
        ncsRaiseEvent ((mObject*)self, NCSN_ITEMPIECE_CHECKBOX_ON, ((mPanelPiece *)self)->addData);
        return TRUE;
    }

    return FALSE;
}

static BOOL onIndicatorButtonClicked (mItemPiece *self, mIndicatorButtonPiece *ac, int event_id, DWORD param)
{
    if (event_id == NCSN_ABP_CLICKED) {
        ncsRaiseEvent ((mObject*)self, NCSN_ITEMPIECE_BUTTON_CLICKED, ((mPanelPiece *)self)->addData);
        return TRUE;
    }

    return FALSE;
}

static void setSlideSwitchPiece4AccessryControl(mHotPiece *piece)
{
    ARGB color_b[]   = {0xffffffff, 0xffdedede, 0xffc4c7c9, 0xffd0d3d4, 0xffd0d3d4};
    float pos_b[]    = {0.0, 0.68, 0.85, 0.93, 1.0};

    ARGB color_bb[]  = {0xffb4b4b4, 0xffadadad, 0xff808080, 0xff707070, 0xffa9a9a9, 0xffb9b9b9};
    float pos_bb[]   = {0.0, 0.68, 0.85, 0.93, 0.95, 1.0};

    mColorTable colorTable_b, colorTable_bb;
    mSlideSwitchPiece *ac = (mSlideSwitchPiece*)piece;

    ShapeTransRoundPiece_setColorTable (&colorTable_b, color_b, pos_b, TABLESIZE(color_b));
    ShapeTransRoundPiece_setColorTable (&colorTable_bb, color_bb, pos_bb, TABLESIZE(color_bb));

    _c(ac)->setButton (ac, 1, -1, &colorTable_b, &colorTable_bb);
}

static void mItemPiece_setAccessoryControl (mItemPiece *self,
        char *text, BOOL turnOn, int acStyle, DWORD add_data)
{
    RECT rc;
    mHotPiece *ac = NULL;

    int event_ids[] = { NCSN_SLIDESWITCHPIECE_OFF, NCSN_SLIDESWITCHPIECE_ON, 0 };

    if (self->accessoryControlPiece != NULL) {
        puts("\tErr: accessory control piece can only be set once");
        return;
    }

    _c(self)->getRect(self, &rc);

    switch (acStyle) {
        case ITEMPIECE_AC_SWITCH:
            ac = (mHotPiece *)NEWPIECE (mSlideSwitchPiece);
            setSlideSwitchPiece4AccessryControl(ac);

            SetRect (&rc, 0, 0, SWITCH_WIDTH, SWITCH_HEIGHT);

            _c(ac)->setRect (ac, &rc);

            _c(ac)->setProperty(ac, NCSP_SLIDESWITCH_RADIUS, 7);

            _c((mSlideSwitchPiece *)ac)->setState ((mSlideSwitchPiece *)ac,
                    (turnOn ? NCSN_SLIDESWITCHPIECE_ON : NCSN_SLIDESWITCHPIECE_OFF));

            ncsAddEventListeners ((mObject*)ac, (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)onSwitchStateChanged, event_ids);
            break;
        case ITEMPIECE_AC_INDICATORBUTTON:
            ac = (mHotPiece *)NEWPIECE (mIndicatorButtonPiece);

            SetRect (&rc, 0, 0, INDICATOR_WIDTH, INDICATOR_HEIGHT);

            _c(ac)->setRect (ac, &rc);

            _c(ac)->setProperty (ac, NCSP_INDICATORBUTTON_IMAGE, add_data);

            ncsAddEventListener ((mObject*)ac, (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)onIndicatorButtonClicked, NCSN_ABP_CLICKED);
            break;
        case ITEMPIECE_AC_CHECKMARK:
            ac = (mHotPiece *)NEWPIECE (mCheckMarkPiece);

            SetRect (&rc, 0, 0, CHECKMARK_WIDTH, CHECKMARK_HEIGHT);

            _c(ac)->setRect (ac, &rc);

            _c((mCheckMarkPiece *)ac)->setState ((mCheckMarkPiece *)ac,
                    (turnOn ? NCSN_CHECKMARKPIECE_ON : NCSN_CHECKMARKPIECE_OFF));
            break;
        case ITEMPIECE_AC_RADIO:
            ac = (mHotPiece *)NEWPIECE (mRadioPiece);

            SetRect (&rc, 0, 0, RADIO_WIDTH, RADIO_HEIGHT);

            _c(ac)->setRect (ac, &rc);

            _c((mRadioPiece *)ac)->setState ((mRadioPiece *)ac,
                    (turnOn ? NCSN_RADIOPIECE_ON : NCSN_RADIOPIECE_OFF));
            break;
        case ITEMPIECE_AC_INDICATOR:
            ac = (mHotPiece *)NEWPIECE (mIndicatorPiece);

            SetRect (&rc, 0, 0, INDICATOR_WIDTH, INDICATOR_HEIGHT);

            _c(ac)->setRect (ac, &rc);
            break;
        default:
            assert (0);
    }

    self->acStyle               = acStyle;

    self->accessoryControlPiece = ac;

    _c(self)->addContent (self, ac, 0, 0);

    self->childAlign[self->childrenTotal++] = self->align;
}

static void mItemPiece_setCheckBoxState(mItemPiece * self, BOOL turnOn)
{
    mHotPiece *ac = self->accessoryControlPiece;

    switch (self->acStyle) {
        case ITEMPIECE_AC_SWITCH:
            _c((mSlideSwitchPiece *)ac)->setState ((mSlideSwitchPiece *)ac,
                    (turnOn ? NCSN_SLIDESWITCHPIECE_ON : NCSN_SLIDESWITCHPIECE_OFF));
            break;
        case ITEMPIECE_AC_CHECKMARK:
            _c((mCheckMarkPiece *)ac)->setState ((mCheckMarkPiece *)ac,
                    (turnOn ? NCSN_CHECKMARKPIECE_ON : NCSN_CHECKMARKPIECE_OFF));
            break;
        case ITEMPIECE_AC_RADIO:
            _c((mRadioPiece *)ac)->setState ((mRadioPiece *)ac,
                    (turnOn ? NCSN_RADIOPIECE_ON : NCSN_RADIOPIECE_OFF));
            break;
    }
}

static int mItemPiece_getCheckBoxState(mItemPiece *self)
{
    mHotPiece *ac = self->accessoryControlPiece;

    switch (self->acStyle) {
        case ITEMPIECE_AC_SWITCH:
            return _c((mSlideSwitchPiece *)ac)->getState ((mSlideSwitchPiece *)ac);
            break;
        case ITEMPIECE_AC_CHECKMARK:
            return _c((mCheckMarkPiece *)ac)->getState ((mCheckMarkPiece *)ac);
            break;
    }

    return -1;
}

static void mItemPiece_activeLayout (mItemPiece *self)
{
    mItemIterator *iter;
    mPieceItem    *item;

    RECT rc;

    int i = 0;
    int subTitleAlign = NCS_ALIGN_LEFT;
    int subTitleOffsetLeft = -1;
    int subTitleOffsetRight = -1;

    _c(self)->getRect (self, &rc);

    self->offsetLeft  = self->margin + self->border;
    self->offsetRight = RECTW(rc) - (self->margin + self->border);

    iter = _c(self->itemManager)->createItemIterator (self->itemManager);

    i = 0;

    while ((item = _c(iter)->next(iter))) {
        assert (i < self->childrenTotal);
        assert (item->piece);

        if (item->piece == (mHotPiece *)self->bkPiece) {
            i++;
            continue;
        }

        if (item->piece != (mHotPiece *)self->subTitlePiece) {
            self->align = self->childAlign[i];
            if (item->piece == (mHotPiece*)self->titlePiece) {
                if (self->align == NCS_ALIGN_LEFT)
                    subTitleOffsetLeft  = self->offsetLeft;
                else if (self->align == NCS_ALIGN_RIGHT)
                    subTitleOffsetRight = self->offsetRight;
            }
            setChildOffset (self, item->piece, &item->x, &item->y);
        } else {
            subTitleAlign = self->childAlign[i];
            if (subTitleOffsetRight < 0)
                subTitleOffsetRight = self->offsetRight;
            if (subTitleOffsetLeft  < 0)
                subTitleOffsetLeft  = self->offsetLeft;
        }

        i++;
    }

    DELETE (iter);

    if (subTitleAlign == NCS_ALIGN_LEFT)
        reviseTitleOffset (self, subTitleOffsetLeft, NCS_ALIGN_LEFT);
    else
        reviseTitleOffset (self, subTitleOffsetRight, NCS_ALIGN_RIGHT);
}

static BOOL mItemPiece_setRect (mItemPiece *self, const RECT *prc)
{
    BOOL ret;
    RECT bkrc;
    RECT rc;

    _c(self)->getRect (self, &rc);

    if (rc.left == prc->left && rc.right == prc->right
            && rc.top == prc->top && rc.bottom == prc->bottom) {
        return TRUE;
    }

    /* set back piece */
    SetRect (&bkrc, 0, 0, RECTW(*prc) - 2 * self->border, RECTH(*prc));

    _c(self->bkPiece)->setRect (self->bkPiece, &bkrc);

    _c(self)->movePiece (self, (mHotPiece *)(self->bkPiece), self->border, 0);

    _c(self)->setItemRect (self, prc);

    ret = Class(mPanelPiece).setRect ((mPanelPiece *)self, prc);

    /* fix me */
    if (self->isTitlePiece) {
        _c(self)->activeLayout (self);
    }

    return ret;
}

static BOOL mItemPiece_setItemRect(mItemPiece *self, const RECT *prc)
{
     RECT rc;

    _c(self)->getRect (self, &rc);

    if (rc.left == prc->left && rc.right == prc->right
            && rc.top == prc->top && rc.bottom == prc->bottom) {
        return TRUE;
    }

    if (RECTW(*prc) < 320) {
        if (self->detailPiece) {
            if ((RECTW(*prc) - 280) >= 0) {
                _c(self)->setPieceAlpha (self, (mHotPiece *)(self->detailPiece), (RECTW(*prc) - 270) * 0xFF / 50);
            }
            else {
                _c(self)->setPieceAlpha (self, (mHotPiece *)(self->detailPiece), 0x00000000);
            }
        }
        if (self->accessoryControlPiece) {
            if ((RECTW(*prc) - 270) >= 0) {
                _c(self)->setPieceAlpha (self, (mHotPiece *)(self->accessoryControlPiece), (RECTW(*prc) - 270) * 0xFF / 50);
            }
            else {
                _c(self)->setPieceAlpha (self, (mHotPiece *)(self->accessoryControlPiece), 0x00);
            }
        }
    }
    else {
        if (self->detailPiece) {
            _c(self)->setPieceAlpha (self, (mHotPiece *)(self->detailPiece), 0xFF);
        }
    }

    return TRUE;
//    return _c(self)->setRect (self, prc);
}

static BOOL mItemPiece_setProperty (mItemPiece *self, int id, DWORD value)
{
    RECT rect;
    RECT bkrc;
    HBRUSH brush;

    switch (id) {
        case NCSP_ITEMPIECE_MARGIN:
            if ((int)value >= 0) {
                self->margin = (unsigned)value;
            }
            return TRUE;
        case NCSP_ITEMPIECE_BORDER:
            if ((int)value >= 0) {
                self->border = (unsigned)value;
            }
            return TRUE;
        case NCSP_ITEMPIECE_CLEARANCE:
            if ((int)value >= 0) {
                self->clearance = (unsigned)value;
            }
            return TRUE;
        case NCSP_ITEMPIECE_VALIGN:
            if ((int)value >= 0 && (int)value < 3) {
                self->valign = (int)value;
            }
            return TRUE;
        case NCSP_ITEMPIECE_BKCOLOR:
            _c(self)->getRect (self, &rect);
            _c(self->bkPiece)->setRect (self->bkPiece, &rect);
            _c(self->bkPiece)->setProperty (self->bkPiece, NCSP_TRANROUND_BKCOLOR, value);
            _c(self)->movePiece (self, (mHotPiece *)(self->bkPiece), 0, 0);
            return TRUE;
        case NCSP_ITEMPIECE_GRADIENT_BKCOLOR:
            /* if not has bkPiece then create it */
            assert (self->bkPiece != NULL);

            _c(self->bkPiece)->setProperty (self->bkPiece, NCSP_TRANROUND_BKCOLOR, (ARGB)value);

            _c(self->bkPiece)->setProperty (self->bkPiece, NCSP_TRANROUND_RADIUS, 6);

//            _c(self->bkPiece)->setProperty (self->bkPiece, NCSP_TRANROUND_BORDERCOLOR, 0xff6c6c6c);
//            _c(self->bkPiece)->setProperty (self->bkPiece, NCSP_TRANROUND_BORDERSIZE, 1);

            _c(self)->getRect (self, &rect);

            SetRect (&bkrc, 0, 0, RECTW(rect) - 2 * self->border, RECTH(rect));

            _c(self->bkPiece)->setRect (self->bkPiece, &bkrc);

            _c(self)->movePiece (self, (mHotPiece *)(self->bkPiece), self->border, 0);

            _c(self->bkPiece)->setProperty (self->bkPiece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);

            brush = _c((mShapeTransRoundPiece*)self->bkPiece)->getBrush((mShapeTransRoundPiece*)self->bkPiece);

            /* 0.0f, 1.0f are needed */
            MGPlusLinearGradientBrushAddColor (brush, 0xe0e8ebab, 0.0f);
            MGPlusLinearGradientBrushAddColor (brush, 0xe0d0d571, 0.07f);
            MGPlusLinearGradientBrushAddColor (brush, 0xe0d0d571, 0.91f);
            MGPlusLinearGradientBrushAddColor (brush, 0xe09a9e43, 1.0f);

            return TRUE;
        default:
            return Class(mPanelPiece).setProperty ((mPanelPiece *)self, id, value);
    }
}

static DWORD mItemPiece_getProperty (mItemPiece *self, int id)
{
    switch (id) {
        case NCSP_ITEMPIECE_MARGIN:
            return (DWORD)self->margin;
        case NCSP_ITEMPIECE_BORDER:
            return (DWORD)self->border;
        case NCSP_ITEMPIECE_CLEARANCE:
            return (DWORD)self->clearance;
        default:
            return Class(mPanelPiece).getProperty ((mPanelPiece *)self, id);
    }
}

static int mItemPiece_processMessage(mItemPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    mHotPiece *ac = self->accessoryControlPiece;

    int cur_state;

    if ((message >= MSG_FIRSTMOUSEMSG) && (message <= MSG_RBUTTONDBLCLK)) {
        if (message == MSG_LBUTTONUP) {
            if (self->state == ITEMPIECE_STATE_PUSHED) {
                self->state = ITEMPIECE_STATE_NORMAL;
                switch (self->acStyle) {
                    case ITEMPIECE_AC_SWITCH:
                        break;
                    case ITEMPIECE_AC_INDICATORBUTTON:
                        break;
                    case ITEMPIECE_AC_CHECKMARK:
                        _c((mCheckMarkPiece *)ac)->changeState ((mCheckMarkPiece *)ac);

                        ncsRaiseEvent ((mObject*)self, NCSN_ITEMPIECE_CONTENT_CLICKED, ((mPanelPiece *)self)->addData);
                        return 0;
                    case ITEMPIECE_AC_RADIO:
                        cur_state = _c((mRadioPiece *)ac)->getState ((mRadioPiece *)ac);

                        if (cur_state != NCSN_RADIOPIECE_ON) {
                            _c((mRadioPiece *)ac)->changeState ((mRadioPiece *)ac);
                        }

                        ncsRaiseEvent ((mObject*)self, NCSN_ITEMPIECE_CONTENT_CLICKED, ((mPanelPiece *)self)->addData);
                        return 0;
                    case ITEMPIECE_AC_INDICATOR:
                    default:
                        ncsRaiseEvent ((mObject*)self, NCSN_ITEMPIECE_CONTENT_CLICKED, ((mPanelPiece *)self)->addData);
                        return 0;
                }
            }
            else {
                return 0;
            }
        } else if (message == MSG_LBUTTONDOWN) {
            if (self->state == ITEMPIECE_STATE_NORMAL) {
                self->state = ITEMPIECE_STATE_PUSHED;
            }
            else {
                return 0;
            }
        }
    }

    return Class(mPanelPiece).processMessage ((mPanelPiece *)self, message, wParam, lParam, owner);
}

static PLOGFONT createLogFont (unsigned size)
{
    return CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL, FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            size, 0);
}

static void setChildOffset(mItemPiece *self, mHotPiece *piece, int *px, int *py)
{
    RECT rc, rcChild;

    assert (piece != NULL);

    _c(self)->getRect (self, &rc);
    _c(piece)->getRect (piece, &rcChild);

    switch (self->align) {
        case NCS_ALIGN_LEFT:
            *px = self->offsetLeft;
            self->offsetLeft += RECTW(rcChild) + self->clearance;
            break;
        case NCS_ALIGN_RIGHT:
            *px = self->offsetRight - RECTW(rcChild);
            self->offsetRight -= RECTW(rcChild) + self->clearance;
            break;
        default:
            assert(0);
    }

    if ((INSTANCEOF(piece, mTextPiece)) || (INSTANCEOF(piece, mImagePiece))) {
        switch (self->valign) {
            case NCS_VALIGN_TOP:
                *py = self->margin;
                break;
            case NCS_VALIGN_CENTER:
                *py = RECTH(rc) / 2 - RECTH(rcChild) / 2;
                break;
            case NCS_VALIGN_BOTTOM:
                *py = RECTH(rc) - RECTH(rcChild) - self->margin;
                break;
            default:
                assert(0);
        }
    }
    else {
        *py = RECTH(rc) / 2 - RECTH(rcChild) / 2;
    }
}

static void reviseTitleOffset (mItemPiece *self, int subTitleOffset, int subTitleAlign)
{
    mPieceItem *titleItem;
    mPieceItem *subTitleItem;

    RECT rcTitle;
    RECT rcSubTitle;

    if (self->titlePiece && self->subTitlePiece) {
        titleItem    = _c(self)->searchItem (self, (mHotPiece *)self->titlePiece);
        subTitleItem = _c(self)->searchItem (self, (mHotPiece *)self->subTitlePiece);

        assert (titleItem && subTitleItem);

        _c(titleItem->piece)->getRect(titleItem->piece, &rcTitle);
        _c(subTitleItem->piece)->getRect(subTitleItem->piece, &rcSubTitle);

        if (subTitleOffset >= 0) {
            if (subTitleAlign == NCS_ALIGN_LEFT)
                subTitleItem->x = subTitleOffset;
            else if (subTitleAlign == NCS_ALIGN_RIGHT)
                subTitleItem->x = subTitleOffset - RECTW(rcSubTitle);
        }

        switch (self->valign) {
            case NCS_VALIGN_TOP:
                subTitleItem->y = titleItem->y + RECTH(rcTitle);
                break;
            case NCS_VALIGN_CENTER:
            case NCS_VALIGN_BOTTOM:
                titleItem->y -= RECTH(rcSubTitle) / 2;
                subTitleItem->y = titleItem->y + RECTH(rcTitle);
                break;
            default:
                assert (0);
        }
    }
}

static void setImage (mImagePiece *img, PBITMAP pbmp, int w, int h)
{
    RECT rc;

    assert (img != NULL);
    assert (pbmp != NULL);

    if (w <= 0) w = pbmp->bmWidth;
    if (h <= 0) h = pbmp->bmHeight;

    SetRect (&rc, 0, 0, w, h);

    _c(img)->setRect (img, &rc);

    _c(img)->setProperty (img, NCSP_IMAGEPIECE_DRAWMODE, (DWORD)NCS_DM_SCALED);

    _c(img)->setProperty (img, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbmp);
}

static void resetRectOfText(mItemPiece *self, mTextPiece *txt, PLOGFONT font)
{
    char *str;
    RECT rc;
    SIZE size;
    int  width;
    PLOGFONT oldFont;
    int lineY;
    HDC hdc = HDC_SCREEN;

    assert (txt != NULL);

    oldFont = SelectFont (hdc, font);

    if ((str = (char*)_c(txt)->getProperty(txt, NCSP_LABELPIECE_LABEL)) != NULL) {
        GetTextExtent (hdc, str, strlen(str), &size);

        /* substance piece line */
        if (txt == self->titlePiece) {
            if (!(self->isTitlePiece)) {
                lineY = size.cy;

                width = 0;
                size.cy = 0;
                for (; width < size.cx; width += SUBSTANCE_WIDTH) {
                    size.cy += lineY;
                }

                if (size.cx - width > 0) {
                    size.cy += lineY;
                }

                size.cx = SUBSTANCE_WIDTH;

                /* fix item rect */
                _c(self)->getRect (self, &rc);

                if (size.cy + 3 * self->margin + self->border < ITEMPIECE_HEIGHT) {
                    rc.bottom = ITEMPIECE_HEIGHT;
                }
                else {
                    rc.bottom = size.cy + 3 * self->margin + self->border;
                }

                _c(self)->setRect (self, &rc);

                //printf ("rc.bottom : %d size.cy : %d\n", rc.bottom, size.cy);
            }
            if (!self->subTitlePiece)
                size.cy += self->margin;
        } else if (txt == self->subTitlePiece){
            size.cy += self->margin;
        }

        //printf ("size %d %d\n", size.cx, size.cy);

        SetRect (&rc, 0, 0, size.cx, size.cy);

        _c(txt)->setRect (txt, &rc);
    }
    else {
        puts("\t!!! NULL NCSP_LABELPIECE_LABEL !!! please check your program carefully");
        assert(0);
    }

    SelectFont (hdc, oldFont);
}

static void setText (mItemPiece *self, mTextPiece *txt, const char *text, PLOGFONT font, ARGB *pColor)
{
    assert(txt && text);

    /* set label */
    if (text) _c(txt)->setProperty (txt, NCSP_LABELPIECE_LABEL, (DWORD)text);

    /* set text color */
    if (pColor) _c(txt)->setProperty (txt, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)*pColor);

    /* set logfont */
    if (font) _c(txt)->setProperty (txt, NCSP_TEXTPIECE_LOGFONT, (DWORD)font);

    resetRectOfText (self, txt, font);
}

/* class */
BEGIN_MINI_CLASS(mItemPiece, mPanelPiece)
    CLASS_METHOD_MAP(mItemPiece, construct            )
    CLASS_METHOD_MAP(mItemPiece, destroy              )
    CLASS_METHOD_MAP(mItemPiece, setLayout            )
    CLASS_METHOD_MAP(mItemPiece, setImage             )
    CLASS_METHOD_MAP(mItemPiece, setTitle             )
    CLASS_METHOD_MAP(mItemPiece, setSubTitle          )
    CLASS_METHOD_MAP(mItemPiece, setDetail            )
    CLASS_METHOD_MAP(mItemPiece, setSubStance         )
    CLASS_METHOD_MAP(mItemPiece, setAccessoryControl  )
    CLASS_METHOD_MAP(mItemPiece, activeLayout         )
    CLASS_METHOD_MAP(mItemPiece, setCheckBoxState     )
    CLASS_METHOD_MAP(mItemPiece, getCheckBoxState     )
    CLASS_METHOD_MAP(mItemPiece, setRect              )
    CLASS_METHOD_MAP(mItemPiece, setItemRect          )
    CLASS_METHOD_MAP(mItemPiece, processMessage       )
    CLASS_METHOD_MAP(mItemPiece, setProperty          )
    CLASS_METHOD_MAP(mItemPiece, getProperty          )
END_MINI_CLASS

