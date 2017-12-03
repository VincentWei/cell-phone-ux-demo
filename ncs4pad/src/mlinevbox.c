#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include "mpieceitem.h"
#include "mitemiterator.h"
#include "mlayout_manager.h"
#include "mlinevbox.h"
#include "mpanelpiece.h"

static void mLineVBox_reLayout(mLineVBox *self, mItemIterator* iter, int w, int h)
{
    RECT rc;
    int y = 0;
    mPieceItem* item = NULL;
    while((item = _c(iter)->next(iter))) {
        mHotPiece* piece = _c(item)->getPiece(item);
        _c(item)->setY(item, y);
        _c(piece)->getRect(piece, &rc);
        //fprintf(stderr, "item is %p, y is %d.\n", item, y);
        y += (RECTH(rc) + self->gap);
    }

    return;
}

static void mLineVBox_setGap(mLineVBox *self, int gap)
{
    self->gap = gap;
}

static void mLineVBox_getLayoutRect(mLineVBox *self, mItemIterator* iter, RECT* rect)
{
    RECT rc;
    int h = 0;
    int w = 0;
    mPieceItem* item = NULL;
    while((item = _c(iter)->next(iter))) {
        mHotPiece* piece = _c(item)->getPiece(item);
        _c(piece)->getRect(piece, &rc);
        h += RECTH(rc);
        if (RECTW(rc) > w) {
            w = RECTW(rc);
        }
    }
    rect->top = rect->left = 0;
    rect->right = w;
    rect->bottom = h;
    return;
}

BEGIN_MINI_CLASS(mLineVBox, mLayoutManager)
    CLASS_METHOD_MAP(mLineVBox, reLayout)
    CLASS_METHOD_MAP(mLineVBox, setGap)
    CLASS_METHOD_MAP(mLineVBox, getLayoutRect)
END_MINI_CLASS
