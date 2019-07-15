///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include "mabstractitemmanager.h"
#include "mpieceitem.h"
#include "mitemiterator.h"
#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "mpanelpiece.h"

static void mCenterHBox_reLayout(mCenterHBox *self, mItemIterator* iter, int w, int h)
{
    mItemIterator *iter_duplicate = _c(iter)->duplicate(iter);
    mPieceItem *item = NULL;
    int offset = 0;
    int w_total = 0;
    RECT rc;
    while ((item = _c(iter)->next(iter))) {
        if (item->underLayout) {
            _c(item->piece)->getRect(item->piece, &rc);
            w_total += RECTW(rc);
        }
    }

    offset = (w - w_total) / 2;

    while ((item = _c(iter_duplicate)->next(iter_duplicate))) {
        if (item->underLayout) {
            _c(item->piece)->getRect(item->piece, &rc);
            item->x = offset;
            item->y = (h - RECTH(rc)) / 2;
            offset += RECTW(rc);
        }
    }

    DELETE(iter_duplicate);

    return;
}

BEGIN_MINI_CLASS(mCenterHBox, mLayoutManager)
    CLASS_METHOD_MAP(mCenterHBox, reLayout)
END_MINI_CLASS
