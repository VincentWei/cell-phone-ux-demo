#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>

#include "mpanelpiece.h"
#include "mcheckmarkpiece.h"

static void mCheckMarkPiece_construct (mCheckMarkPiece *self, DWORD add_data)
{
    Class(mStaticPiece).construct ((mStaticPiece*)self, add_data);

    self->state = NCSN_CHECKMARKPIECE_OFF;
    self->color = 0xFF000000;

    self->hgs   = MP_INV_HANDLE;
}

static void mCheckMarkPiece_destroy (mCheckMarkPiece *self)
{
    MGPlusGraphicDelete (self->hgs);

    Class(mStaticPiece).destroy ((mStaticPiece*)self);
}

static int mCheckMarkPiece_getState (mCheckMarkPiece *self)
{
    return self->state;
}

static void setStateWithUpdatePiece (mCheckMarkPiece *self, enum mCheckMarkPieceState state)
{
    _c(self)->setState(self, state);

    //TODO: why PanelPiece_invalidatePiece not work?
    PanelPiece_update ((mHotPiece*)self, TRUE);
}

static void mCheckMarkPiece_setState(mCheckMarkPiece *self, enum mCheckMarkPieceState state)
{
    if (self->state == state) return;

    self->state = state;
}

static void mCheckMarkPiece_resetState(mCheckMarkPiece *self)
{
    setStateWithUpdatePiece (self, NCSN_CHECKMARKPIECE_OFF);
}

static void mCheckMarkPiece_changeState(mCheckMarkPiece* self)
{
    enum mCheckMarkPieceState state =
        (self->state == NCSN_CHECKMARKPIECE_OFF) ? NCSN_CHECKMARKPIECE_ON : NCSN_CHECKMARKPIECE_OFF;

    setStateWithUpdatePiece (self, state);
}

#if 0
static int mCheckMarkPiece_processMessage (mCheckMarkPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message == MSG_LBUTTONDOWN) {
    } else if (message == MSG_LBUTTONUP) {
        _c(self)->changeState(self);
        ncsRaiseEvent((mObject*)self, self->state, 0);
    }
    return 0;
}
#endif

static void mCheckMarkPiece_paint (mCheckMarkPiece *self, HDC hdc, mObject *owner, DWORD add_data)
{
    HGRAPHICS hgs;
    HPEN      pen;

    int w;
    int h;

    if (self->state == NCSN_CHECKMARKPIECE_ON) {
        hgs = self->hgs;
        w   = self->hgs_width;
        h   = self->hgs_height;

        MGPlusGraphicClear (hgs, 0);

        pen = MGPlusPenCreate (2, self->color);

        MGPlusDrawLine (hgs, pen, w / 4, h / 2, w / 2, h * 3 / 4);
        MGPlusDrawLine (hgs, pen, w / 2, h * 3 / 4, w * 6 / 7, h / 4);

        MGPlusPenDelete (pen);

        BitBlt (MGPlusGetGraphicDC(hgs), 0, 0, 0, 0, hdc, 0, 0, 0);
    }
}

static BOOL mCheckMarkPiece_setProperty (mCheckMarkPiece *self, int id, DWORD value)
{
    switch (id) {
        case NCSP_CHECKMARK_COLOR:
            self->color = (ARGB)value;
            break;
        default:
            return Class(mStaticPiece).setProperty ((mStaticPiece*)self, id, value);
    }

    return TRUE;
}

static DWORD mCheckMarkPiece_getProperty(mCheckMarkPiece *self, int id)
{
    switch (id) {
        case NCSP_CHECKMARK_COLOR:
            return (DWORD)self->color;
        default:
            return Class(mStaticPiece).getProperty ((mStaticPiece*)self, id);
    }
}

static BOOL mCheckMarkPiece_setRect(mCheckMarkPiece *self, const RECT *prc)
{
    BOOL ret = Class(mStaticPiece).setRect ((mStaticPiece*)self, prc);

    if (ret) {
        self->hgs_width = RECTWP(prc);
        self->hgs_height = RECTHP(prc);

        if (self->hgs != MP_INV_HANDLE) {
            MGPlusGraphicDelete (self->hgs);
        }

        self->hgs = MGPlusGraphicCreate (self->hgs_width, self->hgs_height);
    }

    return ret;
}

BEGIN_MINI_CLASS(mCheckMarkPiece, mStaticPiece)
    CLASS_METHOD_MAP(mCheckMarkPiece, construct   )
    CLASS_METHOD_MAP(mCheckMarkPiece, destroy     )
    CLASS_METHOD_MAP(mCheckMarkPiece, getState    )
    CLASS_METHOD_MAP(mCheckMarkPiece, setState    )
    CLASS_METHOD_MAP(mCheckMarkPiece, changeState )
    CLASS_METHOD_MAP(mCheckMarkPiece, resetState  )
    CLASS_METHOD_MAP(mCheckMarkPiece, paint       )
    CLASS_METHOD_MAP(mCheckMarkPiece, setProperty )
    CLASS_METHOD_MAP(mCheckMarkPiece, getProperty )
    CLASS_METHOD_MAP(mCheckMarkPiece, setRect     )
END_MINI_CLASS
