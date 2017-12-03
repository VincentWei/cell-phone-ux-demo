#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "mpanelpiece.h"
#include "mbuttonpanelpiece.h"

static void changeState(mButtonPanelPiece *self, enum mButtonPanelPieceState state, mObject* owner)
{
    RECT rc;
    self->state = state;

    _c(self)->getRect(self, &rc);
    _c(self)->setStates(self);

#if 1
    PanelPiece_invalidatePiece((mHotPiece*)self, NULL);
#else
    PanelPiece_update((mHotPiece*)self, TRUE);
#endif
}

static int s_onMouseEvent(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    enum mButtonPanelPieceState state = ((mButtonPanelPiece*)self)->state;
    if (message == MSG_LBUTTONDOWN) {
        if (state != NCS_BPP_PUSHED) {
            changeState((mButtonPanelPiece*)self, NCS_BPP_PUSHED, owner);
            _c(self)->processMessage(self, MSG_BTNPANELPIECE_STATE_CHANGED,
                    (WPARAM)NCSN_ABP_PUSHED, (LPARAM)0, owner);
            ncsRaiseEvent((mObject*)self, NCSN_ABP_PUSHED, ((mPanelPiece*)self)->addData);
        }
    }
    else if (message == MSG_LBUTTONUP) {
        if (state != NCS_BPP_NORMAL) {
            changeState((mButtonPanelPiece*)self, NCS_BPP_NORMAL, owner);
            _c(self)->processMessage(self, MSG_BTNPANELPIECE_STATE_CHANGED,
                    (WPARAM)NCSN_ABP_CLICKED, (LPARAM)0, owner);
            ncsRaiseEvent((mObject*)self, NCSN_ABP_CLICKED, ((mPanelPiece*)self)->addData);
        }
    }
    else if (message == MSG_MOUSEMOVEIN) {
        if (!(BOOL)wParam) {
//            fprintf(stderr, "MOVEOUT from piece %p\n", self);
            if (state == NCS_BPP_PUSHED)
                changeState((mButtonPanelPiece*)self, NCS_BPP_NORMAL, owner);
        }
        else {
//            fprintf(stderr, "MOVEIN to piece %p\n", self);
            ;
        }
    }
    else {
        return -1;
    }
    return 0;
}

static void mButtonPanelPiece_construct(mButtonPanelPiece *self, DWORD addData)
{
    Class(mPanelPiece).construct((mPanelPiece*)self, addData);

    INIT_LIST_HEAD(&self->stateTable);

    self->state = NCS_BPP_NORMAL;

    memset(&self->gradientBackgroundColorTable[0], 0, sizeof(mColorTable));
    memset(&self->gradientBackgroundColorTable[1], 0, sizeof(mColorTable));

    _c(self)->setBkgndPiece(self, NEWPIECE(mShapeTransRoundPiece));
    //_c(self)->addContent(self, (mHotPiece*)self->bkgndPiece, 0, 0);
    if (self->bkgndPiece) {
        _c(self->bkgndPiece)->setProperty(self->bkgndPiece, NCSP_TRANROUND_BKCOLOR, MakeRGBA(0, 0, 0, 0));
    }

    _c(self)->appendEventHandler(self, MSG_LBUTTONDOWN, s_onMouseEvent);
    _c(self)->appendEventHandler(self, MSG_LBUTTONUP, s_onMouseEvent);
    _c(self)->appendEventHandler(self, MSG_MOUSEMOVEIN, s_onMouseEvent);
    
    _c(self)->setLayoutManager(self, (mLayoutManager*)NEW(mCenterHBox));
}

static void mButtonPanelPiece_destroy(mButtonPanelPiece *self)
{
    list_t *i = NULL, *n = NULL;
    list_for_each_safe(i, n, &self->stateTable) {
        list_del(i);
        free(i);
    }

    /* no need delete bkgndPiece piece here
     * it will be freed in the mContent list */
    //    DELPIECE(self->bkgndPiece);

    _c(self)->clearGradientBackgroundColor(self, TRUE, TRUE);

    Class(mPanelPiece).destroy((mPanelPiece*)self);
}

static BOOL mButtonPanelPiece_setRect(mButtonPanelPiece *self, const RECT *prc)
{
    BOOL ret = Class(mPanelPiece).setRect((mPanelPiece*)self, prc);

    if (ret && self->bkgndPiece)
        _c(self->bkgndPiece)->setRect(self->bkgndPiece, prc);

    return ret;
}

static void mButtonPanelPiece_setStateTable(mButtonPanelPiece *self,
        mHotPiece *piece, int propertyId, DWORD normalValue, DWORD pushedValue)
{
    mStateTable *ms;

    if (!ncsIsValidObj((mObject*)piece)) {
        puts("invalid object");
        assert(0);
    }

    ms = (mStateTable*)calloc(1, sizeof(mStateTable));
    assert(ms);

    list_add(&ms->list, &self->stateTable);
    ms->piece = piece;
    ms->propertyId = propertyId;
    ms->propertyValue[NCS_BPP_NORMAL] = normalValue;
    ms->propertyValue[NCS_BPP_PUSHED] = pushedValue;

//    _c(piece)->setProperty(piece, propertyId, ms->propertyValue[self->state]);
}

static void mButtonPanelPiece_setStates(mButtonPanelPiece *self)
{
    HBRUSH brush;
    mColorTable *colorTable;
    list_t *i = NULL;
    list_for_each(i, &self->stateTable) {
        mStateTable *ms = (mStateTable*)i;
        _c(ms->piece)->setProperty(ms->piece, ms->propertyId, ms->propertyValue[self->state]);
    }

    colorTable = &self->gradientBackgroundColorTable[self->state];
    if (colorTable->tableSize > 0) {
        _c(self->bkgndPiece)->setProperty(self->bkgndPiece,
                NCSP_TRANROUND_FILLMODE, (DWORD)NCSP_TRANROUND_GRADIENT_FILL);

        brush = _c(self->bkgndPiece)->getBrush(self->bkgndPiece);
//        MGPlusSetLinearGradientBrushMode (brush, MP_LINEAR_GRADIENT_MODE_VERTICAL);

        MGPlusSetLinearGradientBrushColorsEx (brush, colorTable->colors, colorTable->tableSize, colorTable->positions);
    }
}

static void clearColorTable(mButtonPanelPiece *self, mColorTable *colorTable)
{
    colorTable->tableSize = 0;
    if (colorTable->colors != NULL)
        free(colorTable->colors);
    if (colorTable->positions != NULL)
        free(colorTable->positions);
}

static void resetColorTable(mButtonPanelPiece *self, mColorTable *colorTable,
        ARGB *values, float *positions, unsigned int tableSize)
{
    int i;
    assert(tableSize > 1 || values != NULL);

    clearColorTable(self, colorTable);

    colorTable->colors = (ARGB*)calloc(tableSize, sizeof(ARGB));
    colorTable->positions = (float*)calloc(tableSize, sizeof(float));
    colorTable->tableSize = tableSize;
    memcpy(colorTable->colors, values, tableSize * sizeof(ARGB));

    if (positions == NULL) {
        for (i = 0; i < tableSize; ++i)
            colorTable->positions[i] = (float)((double)i / (double)(tableSize - 1));
    } else {
        memcpy(colorTable->positions, positions, tableSize * sizeof(float));
    }
}

static void mButtonPanelPiece_setGradientBackgroundColor(mButtonPanelPiece *self,
        ARGB *normalValues, float *normalValuePositions, unsigned int normalTableSize,
        ARGB *pushedValues, float *pushedValuePositions, unsigned int pushedTableSize)
{
    mColorTable *colorTable = self->gradientBackgroundColorTable;

    if (normalTableSize > 0)
        resetColorTable(self, &colorTable[NCS_BPP_NORMAL], normalValues, normalValuePositions, normalTableSize);

    if (pushedTableSize > 0)
        resetColorTable(self, &colorTable[NCS_BPP_PUSHED], pushedValues, pushedValuePositions, pushedTableSize);
}

static void mButtonPanelPiece_clearGradientBackgroundColor(mButtonPanelPiece *self, BOOL clearNormalState, BOOL clearPushedState)
{
    mColorTable *colorTable = self->gradientBackgroundColorTable;

    if (clearNormalState)
        clearColorTable(self, &colorTable[NCS_BPP_NORMAL]);

    if (clearPushedState)
        clearColorTable(self, &colorTable[NCS_BPP_PUSHED]);
}

static void mButtonPanelPiece_setGradientBorderColor(mButtonPanelPiece *self,
        ARGB *values, float *positions, unsigned int tableSize)
{
    HBRUSH brush;
    brush = _c(self->bkgndPiece)->getBorderBrush(self->bkgndPiece);

    if (positions) {
        MGPlusSetLinearGradientBrushColorsEx (brush, values, tableSize, positions);
    } else {
        MGPlusSetLinearGradientBrushColors(brush, values, tableSize);
    }
}

BEGIN_MINI_CLASS(mButtonPanelPiece, mPanelPiece)
    CLASS_METHOD_MAP(mButtonPanelPiece, construct   )
    CLASS_METHOD_MAP(mButtonPanelPiece, destroy     )
    CLASS_METHOD_MAP(mButtonPanelPiece, setRect     )
    CLASS_METHOD_MAP(mButtonPanelPiece, setStateTable)
    CLASS_METHOD_MAP(mButtonPanelPiece, setGradientBackgroundColor  )
    CLASS_METHOD_MAP(mButtonPanelPiece, clearGradientBackgroundColor)
    CLASS_METHOD_MAP(mButtonPanelPiece, setStates    )
    CLASS_METHOD_MAP(mButtonPanelPiece, setGradientBorderColor)
END_MINI_CLASS
