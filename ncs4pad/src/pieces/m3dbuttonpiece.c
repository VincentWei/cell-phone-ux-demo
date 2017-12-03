#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "mtextpiece.h"
#include "mpanelpiece.h"
#include "mbuttonpanelpiece.h"
#include "m3dbuttonpiece.h"

#define TXTFONT_SIZE 12
#define TXTFONT_COLOR 0xFF000000

static PLOGFONT createLogFont (unsigned size)
{
    return CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL, FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            size, 0);
}

static void m3DButtonPiece_construct(m3DButtonPiece *self, DWORD addData)
{
    mShapeTransRoundPiece *bk;
    ARGB colors0[] = {0xffcd8088, 0xffe1505f, 0xffaf1d28, 0xffa71e26};
    float pos0[] = {0.0, 0.49, 0.50, 1.0};
    ARGB colors1[] = {0xffcd8088, 0xffe1505f, 0xffaf1d28, 0xffa71e26};
    float pos1[] = {0.0, 0.49, 0.50, 1.0};

    Class(mButtonPanelPiece).construct((mButtonPanelPiece*)self, addData);

    self->txtFont = createLogFont(TXTFONT_SIZE);
    self->txtPiece = NEWPIECE(mTextPiece);
    _c(self->txtPiece)->setProperty(self->txtPiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)self->txtFont);
    _c(self->txtPiece)->setProperty(self->txtPiece, NCSP_LABELPIECE_LABEL, (DWORD)"button");
    _c(self)->addContentToLayout(self, (mHotPiece*)self->txtPiece);

    bk = _c(self)->getBkgndPiece(self);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_BORDERSIZE, 1);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_GRADIENTBORDER, TRUE);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, 4);
    _c(self)->setGradientBackgroundColor(self,
            colors0, pos0, TABLESIZE(colors0),
            colors1, pos1, TABLESIZE(colors1));
    _c(self)->setStates(self);
}

static void m3DButtonPiece_destroy(m3DButtonPiece *self)
{
    DestroyLogFont(self->txtFont);
    Class(mButtonPanelPiece).destroy((mButtonPanelPiece*)self);
}

static BOOL m3DButtonPiece_setRect(m3DButtonPiece *self, const RECT *prc)
{
    _c(self->txtPiece)->setRect(self->txtPiece, prc);

    return Class(mButtonPanelPiece).setRect((mButtonPanelPiece*)self, prc);
}

static BOOL m3DButtonPiece_setProperty(m3DButtonPiece *self, int id, DWORD value)
{
    switch (id) {
        case NCSP_3DBUTTONPIECE_TEXT:
            if ((char *)value != NULL)
                _c(self->txtPiece)->setProperty(self->txtPiece, NCSP_LABELPIECE_LABEL, value);
            break;
        case NCSP_3DBUTTONPIECE_TEXTLOGFONT:
            if ((PLOGFONT)value != NULL)
                _c(self->txtPiece)->setProperty(self->txtPiece, NCSP_TEXTPIECE_LOGFONT, value);
            break;
        case NCSP_3DBUTTONPIECE_TEXTCOLOR:
            _c(self->txtPiece)->setProperty(self->txtPiece, NCSP_TEXTPIECE_TEXTCOLOR, value);
            break;
        default:
            return Class(mButtonPanelPiece).setProperty((mButtonPanelPiece*)self, id, value);
    }
    return TRUE;
}

static DWORD m3DButtonPiece_getProperty(m3DButtonPiece *self, int id)
{
    switch (id) {
        case NCSP_3DBUTTONPIECE_TEXT:
            return _c(self->txtPiece)->getProperty(self->txtPiece, NCSP_LABELPIECE_LABEL);
        case NCSP_3DBUTTONPIECE_TEXTLOGFONT:
            return _c(self->txtPiece)->getProperty(self->txtPiece, NCSP_TEXTPIECE_LOGFONT);
        case NCSP_3DBUTTONPIECE_TEXTCOLOR:
            return _c(self->txtPiece)->getProperty(self->txtPiece, NCSP_TEXTPIECE_TEXTCOLOR);
        default:
            return Class(mButtonPanelPiece).getProperty((mButtonPanelPiece*)self, id);
    }
}

BEGIN_MINI_CLASS(m3DButtonPiece, mButtonPanelPiece)
    CLASS_METHOD_MAP(m3DButtonPiece, construct   )
    CLASS_METHOD_MAP(m3DButtonPiece, destroy     )
    CLASS_METHOD_MAP(m3DButtonPiece, setRect     )
    CLASS_METHOD_MAP(m3DButtonPiece, setProperty )
    CLASS_METHOD_MAP(m3DButtonPiece, getProperty )
END_MINI_CLASS
