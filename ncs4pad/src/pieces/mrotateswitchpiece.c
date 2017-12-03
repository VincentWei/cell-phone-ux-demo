#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>

#include "mpanelpiece.h"
#include "mrotateswitchpiece.h"

#define ANIM_RUNTIME 300

static void mRotateSwitchPiece_construct(mRotateSwitchPiece *self, DWORD add_data)
{
    Class(mStaticPiece).construct((mStaticPiece*)self, add_data);

    self->state = NCSN_ROTATESWITCHPIECE_OFF;
    self->angle = 0;
    self->radius = 0;
    self->rotate_animation = NULL;

    self->colors[0] = 0xffed5961;
    self->colors[1] = 0xffce5961;
    self->colors[2] = 0xffa8191b;
    self->colors[3] = 0xffb9191b;

    self->colorsPos[0] = 0.0;
    self->colorsPos[1] = 0.49;
    self->colorsPos[2] = 0.51;
    self->colorsPos[3] = 1.0;

    self->hgs = MP_INV_HANDLE;
    self->brushSolid = MGPlusBrushCreate(MP_BRUSH_TYPE_SOLIDCOLOR);
    self->brushGradient = MGPlusBrushCreate(MP_BRUSH_TYPE_LINEARGRADIENT);
    MGPlusSetLinearGradientBrushMode(self->brushGradient, MP_LINEAR_GRADIENT_MODE_VERTICAL);
}

static void mRotateSwitchPiece_destroy(mRotateSwitchPiece *self)
{
    if (self->rotate_animation != NULL)
    {
        mGEffAnimationStop(self->rotate_animation);
        self->rotate_animation = NULL;
    }
    MGPlusBrushDelete(self->brushSolid);
    MGPlusBrushDelete(self->brushGradient);
    MGPlusGraphicDelete(self->hgs);

    Class(mStaticPiece).destroy((mStaticPiece*)self);
}

static void rollingAnimationCb(MGEFF_ANIMATION handle, void *target, int id, void *value)
{
    RECT rc;
    mRotateSwitchPiece *self = (mRotateSwitchPiece*)target;
    _c(self)->getRect(self, &rc);

    ((mRotateSwitchPiece*)target)->angle = *(int*)value;

    /* houhh20110220, setRect to delete self->hgs, let rotate repaint.*/
    _c(self)->setRect(self, &rc);
    PanelPiece_invalidatePiece((mHotPiece*)self, &rc);
}
static void anim_finished_cb(MGEFF_ANIMATION handle)
{
    mRotateSwitchPiece *self = (mRotateSwitchPiece *)mGEffAnimationGetContext(handle);
    self->rotate_animation = NULL;
}

static void rollingAnimation(mRotateSwitchPiece *self, int startValue, int endValue)
{
    mPanelPiece *topPanel = PanelPiece_getTopPanel((mHotPiece*)self);
    MGEFF_ANIMATION anim;
    if (self->rotate_animation != NULL)
    {
        mGEffAnimationStop(self->rotate_animation);
        self->rotate_animation = NULL;
    }
    anim = mGEffAnimationCreate(self, rollingAnimationCb, (int)topPanel, MGEFF_INT);
    
    mGEffAnimationSetStartValue(anim, &startValue);
    mGEffAnimationSetEndValue(anim, &endValue);
    mGEffAnimationSetDuration(anim, ANIM_RUNTIME);
    mGEffAnimationSetCurve(anim, Linear);
    mGEffAnimationSetContext(anim, self);
    mGEffAnimationSetFinishedCb(anim, anim_finished_cb);

    self->rotate_animation = anim;
    _c(topPanel)->animationAsyncRun(topPanel, anim, 0);
}

static void setState(mRotateSwitchPiece *self, enum mRotateSwitchPieceState state)
{
    if (self->state == state) return;

    self->state = state;

    if (state == NCSN_ROTATESWITCHPIECE_ON) {

        rollingAnimation(self, 0, 90);
    } else if (state == NCSN_ROTATESWITCHPIECE_OFF){
    
        rollingAnimation(self, 90, 0);
    }
}

static void mRotateSwitchPiece_resetState(mRotateSwitchPiece *self)
{
    setState(self, NCSN_ROTATESWITCHPIECE_OFF);
}

static void mRotateSwitchPiece_changeState(mRotateSwitchPiece* self)
{
    enum mRotateSwitchPieceState state =
        (self->state == NCSN_ROTATESWITCHPIECE_OFF ? NCSN_ROTATESWITCHPIECE_ON : NCSN_ROTATESWITCHPIECE_OFF);

    setState(self, state);
}

static int mRotateSwitchPiece_processMessage(mRotateSwitchPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message == MSG_LBUTTONDOWN) {
    } else if (message == MSG_LBUTTONUP) {
        _c(self)->changeState(self);
        ncsRaiseEvent((mObject*)self, self->state, 0);
    }
    return 0;
}

static void mRotateSwitchPiece_paint(mRotateSwitchPiece *self, HDC hdc, mObject *owner, DWORD add_data)
{
    HBRUSH brushSolid = self->brushSolid;
    HBRUSH brushGradient = self->brushGradient;
    int w = self->hgs_width;
    int h = self->hgs_height;
    RECT rc;
    HPEN pen;

    //hgs = MGPlusGraphicCreateFromDC (hdc);
    if (self->hgs == MP_INV_HANDLE) {
        HGRAPHICS hgs = MGPlusGraphicCreate (self->hgs_width, self->hgs_height);
        MGPlusGraphicClear(hgs, 0xffffffff);

        pen = MGPlusPenCreate(2, 0x77777777);
        MGPlusDrawArc(hgs, pen, w / 2, h / 2, w / 2 - 2, h / 2 - 2, 0, 360);

        MGPlusSetSolidBrushColor(brushSolid, 0xFFFFFFFF);
        MGPlusFillEllipse(hgs, brushSolid, w / 2, h / 2,
                w / 2 - 2, h / 2 - 2);

        _c(self)->getRect(self, &rc);
        MGPlusSetLinearGradientBrushColorsEx(brushGradient, self->colors,
                TABLESIZE(self->colors), self->colorsPos);
        MGPlusSetLinearGradientBrushRect(brushGradient, &rc);
        MGPlusFillEllipse(hgs, brushGradient, w /2 , h / 2, w / 3, h / 3);

        MGPlusResetWorldTransform(hgs);
        MGPlusTranslateWorldTransform(hgs, - w / 2, - h / 2);
        MGPlusRotateWorldTransform(hgs, self->angle);
        MGPlusTranslateWorldTransform(hgs, w / 2, h / 2);

        MGPlusPenSetWidth(pen, 2);
        MGPlusPenSetColor(pen, 0xFFFFFFFF);
        MGPlusDrawLine(hgs, pen, (w + 1) / 4 + 1, h / 2, w * 3 / 4, h / 2);
        MGPlusPenDelete (pen);
        self->hgs = hgs;
    }

    BitBlt(MGPlusGetGraphicDC(self->hgs), 0, 0, 0, 0, hdc, 0, 0, 0);

}

static BOOL mRotateSwitchPiece_setProperty(mRotateSwitchPiece *self, int id, DWORD value)
{
    switch(id) {
        case NCSP_ROTATESWITCH_RADIUS:
            self->radius = (int)value;
            break;
        default:
            return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
    }
    return TRUE;
}

static DWORD mRotateSwitchPiece_getProperty(mRotateSwitchPiece *self, int id)
{
    switch(id) {
        case NCSP_ROTATESWITCH_RADIUS:
            return (DWORD)self->radius;
        default:
            return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
    }
}

static BOOL mRotateSwitchPiece_setRect(mRotateSwitchPiece *self, const RECT *prc)
{
    BOOL ret = Class(mStaticPiece).setRect((mStaticPiece*)self, prc);

    if (ret) {
        self->hgs_width = RECTWP(prc);
        self->hgs_height = RECTHP(prc);
#if 0
        if (self->hgs != MP_INV_HANDLE)
            MGPlusGraphicDelete(self->hgs);
        self->hgs = MGPlusGraphicCreate(self->hgs_width, self->hgs_height);
#else
        if (self->hgs != MP_INV_HANDLE) {
            MGPlusGraphicDelete(self->hgs);
            self->hgs = MP_INV_HANDLE;
        }
#endif
    }

    return ret;
}

BEGIN_MINI_CLASS(mRotateSwitchPiece, mStaticPiece)
    CLASS_METHOD_MAP(mRotateSwitchPiece, construct   )
    CLASS_METHOD_MAP(mRotateSwitchPiece, destroy     )
    CLASS_METHOD_MAP(mRotateSwitchPiece, changeState )
    CLASS_METHOD_MAP(mRotateSwitchPiece, resetState  )
    CLASS_METHOD_MAP(mRotateSwitchPiece, processMessage)
    CLASS_METHOD_MAP(mRotateSwitchPiece, paint       )
    CLASS_METHOD_MAP(mRotateSwitchPiece, setProperty )
    CLASS_METHOD_MAP(mRotateSwitchPiece, getProperty )
    CLASS_METHOD_MAP(mRotateSwitchPiece, setRect     )
END_MINI_CLASS
