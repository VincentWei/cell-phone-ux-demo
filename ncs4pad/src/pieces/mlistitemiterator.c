/*! ============================================================================
 * @file mlistitemiterator.c 
 * @Synopsis mListItemIterator 
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

#include "mlistitemiterator.h"

#define ITEM_HEAD(ptr) list_entry(ptr, mPieceItem, list)

static void mListItemIterator_construct(mListItemIterator *self, DWORD add_data)
{
	Class(mItemIterator).construct((mItemIterator*)self, add_data);
    self->current = self->head = NULL;
    self->manager = NULL;
}

static void mListItemIterator_destroy(mListItemIterator *self)
{
    Class(mItemIterator).destroy((mItemIterator*)self);
}

static mPieceItem*  mListItemIterator_next(mListItemIterator* self)
{
    if (self->current->next != self->head) {
        self->current = self->current->next;
        return (mPieceItem*)ITEM_HEAD(self->current);
    }
    return NULL;
}

static mPieceItem*  mListItemIterator_prev(mListItemIterator* self)
{
    if (self->current->prev != self->head) {
        self->current = self->current->prev;
        return (mPieceItem*)ITEM_HEAD(self->current);
    }
    return NULL;
}

static mListItemIterator* mListItemIterator_duplicate(mListItemIterator* self)
{
    mListItemIterator* iter = NEW(mListItemIterator);
    if (NULL != iter) {
        iter->head = self->head;
        iter->current = self->current;
        iter->manager = self->manager;
        return iter;
    }
    return NULL;
}

BEGIN_MINI_CLASS(mListItemIterator, mItemIterator)
	CLASS_METHOD_MAP(mListItemIterator, construct)
	CLASS_METHOD_MAP(mListItemIterator, destroy)
	CLASS_METHOD_MAP(mListItemIterator, next)
	CLASS_METHOD_MAP(mListItemIterator, prev)
	CLASS_METHOD_MAP(mListItemIterator, duplicate)
END_MINI_CLASS
