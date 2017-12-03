/*! ============================================================================
 * @file mlistitemmanager.c 
 * @Synopsis mListItemManager 
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
#include "mlistitemmanager.h"

static void mListItemManager_construct(mListItemManager *self, DWORD add_data)
{
	Class(mAbstractItemManager).construct((mAbstractItemManager*)self, add_data);
    INIT_LIST_HEAD(&self->queue);
}

static void mListItemManager_destroy(mListItemManager *self)
{
    _c(self)->clear(self);
    Class(mAbstractItemManager).destroy((mAbstractItemManager*)self);
}


int  mListItemManager_addItem(mListItemManager* self, mPieceItem* item)
{
    if (NULL != item) {
        list_add_tail(&(item->list), &self->queue);
        return 0;
    }
    return -1;
}

void mListItemManager_removeItem(mListItemManager* self, mPieceItem* item)
{
    if (NULL != item) {
        list_t *pos = NULL, *n = NULL;
        list_for_each_safe(pos, n, &self->queue) {
            if (pos == &item->list) {
                list_del(pos);
            }
        }
    }
}

void mListItemManager_clear(mListItemManager* self)
{
    list_t *pos = NULL, *n = NULL;
    list_for_each_safe(pos, n, &self->queue) {
        list_del(pos);
    }
}

mItemIterator* mListItemManager_createItemIterator(mListItemManager* self)
{
    mListItemIterator* iter = NEW(mListItemIterator);
    if (NULL != iter) {
        iter->current = iter->head = &self->queue;
        iter->manager = (mAbstractItemManager*)self;
        return (mItemIterator*)iter;
    }
    return NULL;
}

BEGIN_MINI_CLASS(mListItemManager, mAbstractItemManager)
	CLASS_METHOD_MAP(mListItemManager, construct)
	CLASS_METHOD_MAP(mListItemManager, destroy)
	CLASS_METHOD_MAP(mListItemManager, addItem)
	CLASS_METHOD_MAP(mListItemManager, removeItem)
	CLASS_METHOD_MAP(mListItemManager, clear)
	CLASS_METHOD_MAP(mListItemManager, createItemIterator)
END_MINI_CLASS

