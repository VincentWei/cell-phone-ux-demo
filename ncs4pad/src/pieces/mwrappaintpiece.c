#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mwrappaintpiece.h"

static void mWrapPaintPiece_construct(mWrapPaintPiece *self, DWORD add_data)
{
	Class(mLabelPiece).construct((mLabelPiece*)self, add_data);
    self->effect = NCS_WRAPPAINT_ZOOM;
    self->wrappiece = (mHotPiece*)add_data;
    self->parent = self->wrappiece->parent;
}

void wrappaint_effect(mWrapPaintPiece *self, HDC hdc, HDC effect_dc)
{
    RECT rc;
    _c(self)->getRect(self, &rc);
    if (RECTH(rc) != 0) {
        StretchBlt(effect_dc, 0, 0, 0, 0,
                hdc, 0, 0, RECTW(rc), RECTH(rc), 0);
    }
}

static void mWrapPaintPiece_paint(mWrapPaintPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    /* need rotate */
    HDC effect_dc;
    RECT rc;
    _c(self->wrappiece)->getRect(self->wrappiece, &rc);
    effect_dc = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
    _c(self->wrappiece)->paint(self->wrappiece, effect_dc, (mObject*)owner, add_data);

    wrappaint_effect(self, hdc, effect_dc);

    DeleteMemDC(effect_dc);
}

static BOOL mWrapPaintPiece_setProperty(mWrapPaintPiece *self, int id, DWORD value)
{
    if (id == NCSP_WRAPPAINT_EFFECT) {
        self->effect = value;
    } else {
        return Class(mLabelPiece).setProperty((mLabelPiece*)self, id, value);
    }
    return TRUE;
}

static DWORD mWrapPaintPiece_getProperty(mWrapPaintPiece* self, int id)
{
    if (id == NCSP_WRAPPAINT_EFFECT) {
        return (DWORD)self->effect;
    } else {
        return Class(mLabelPiece).getProperty((mLabelPiece*)self, id);
    }
}


BEGIN_MINI_CLASS(mWrapPaintPiece, mStaticPiece)
	CLASS_METHOD_MAP(mWrapPaintPiece, construct)
	CLASS_METHOD_MAP(mWrapPaintPiece, paint)
	CLASS_METHOD_MAP(mWrapPaintPiece, setProperty)
	CLASS_METHOD_MAP(mWrapPaintPiece, getProperty)
END_MINI_CLASS

