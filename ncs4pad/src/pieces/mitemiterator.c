/*! ============================================================================
 * @file mitemiterator.c 
 * @Synopsis mItemIterator 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mitemiterator.h"

static void mItemIterator_construct(mItemIterator *self, DWORD add_data)
{
	Class(mObject).construct((mObject*)self, add_data);
    self->manager = NULL;
}

static void mItemIterator_destroy(mItemIterator *self)
{
    Class(mObject).destroy((mObject*)self);
}

static mPieceItem*  mItemIterator_next(mItemIterator* self)
{
    return NULL;
}

static mPieceItem*  mItemIterator_prev(mItemIterator* self)
{
    return NULL;
}

static mItemIterator* mItemIterator_duplicate(mItemIterator* self)
{
    mItemIterator* iter = NEW(mItemIterator);
    if (NULL != iter) {
        iter->manager = self->manager;
        return iter;
    }
    return NULL;
}

BEGIN_MINI_CLASS(mItemIterator, mObject)
	CLASS_METHOD_MAP(mItemIterator, construct)
	CLASS_METHOD_MAP(mItemIterator, destroy)
	CLASS_METHOD_MAP(mItemIterator, next)
	CLASS_METHOD_MAP(mItemIterator, prev)
	CLASS_METHOD_MAP(mItemIterator, duplicate)
END_MINI_CLASS

