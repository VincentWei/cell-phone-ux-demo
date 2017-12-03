/*! ============================================================================
 * @file mlistitemmanager.c 
 * @Synopsis mGroupItemManager 
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

#include "mgroupitemmanager.h"
#include "mgroupitemiterator.h"
#include "mgrouppieceitem.h"

#define ITEM_HEAD(ptr, list) list_entry(ptr, mGroupPieceItem, list)

static void mGroupItemManager_construct(mGroupItemManager *self, DWORD add_data)
{
	Class(mAbstractItemManager).construct((mAbstractItemManager*)self, add_data);

    INIT_LIST_HEAD(&self->itemList);
    self->itemCmp = strcmp;
}

static void mGroupItemManager_destroy(mGroupItemManager *self)
{
    Class(mAbstractItemManager).destroy((mAbstractItemManager*)self);
}


BOOL mGroupItemManager_addGroupItem(mGroupItemManager* self, mGroupPieceItem* item, BOOL group)
{
    BOOL ret = FALSE;
    mGroupPieceItem* s = NULL;
    if (group) {
        if (list_empty(&self->itemList)) {
            list_add(&(item->list), &self->itemList);
            INIT_LIST_HEAD(&item->child);
            ret = TRUE;
        }
        else {
            /* only intert one group item. */
            s = _c(self)->searchGroupItem(self, _c(item)->getKey(item), group);
            if (!s) {
                s = _c(self)->searchGreaterGroupItem(self, _c(item)->getKey(item), group);
                if (!s) {
                    /* add to tail.*/
                    list_add_tail(&(item->list), &self->itemList);
                }
                else {
                    /* add to before of search group.*/
                    list_add_tail(&(item->list), &s->list);
                }
                INIT_LIST_HEAD(&item->child);
                ret = TRUE;
            }
        }
    }
    else  {
        s = _c(self)->searchGreaterGroupItem(self, _c(item)->getKey(item), FALSE);
        if (s) {
            /* add to before of get grout item(s->list). */
            list_add_tail(&(item->list), &s->list);
        }
        else {
            char* key = _c(item)->getKey(item);
            char group[2];
            group[0] = key[0]; 
            group[1] = '\0';
            s = _c(self)->searchGroupItem(self, group, TRUE);
            list_add_tail(&(item->list), &s->child);
        }
        ret = TRUE;
    }
    return ret;
}

mGroupPieceItem* mGroupItemManager_searchGreaterGroupItem(mGroupItemManager* self, const char* key, BOOL group)
{
    list_t* i;
    mGroupPieceItem* ret = NULL;
    if (group) {   
        list_for_each(i, &self->itemList) {
            mGroupPieceItem* c = (mGroupPieceItem*)ITEM_HEAD(i, list);
            if (self->itemCmp(_c(c)->getKey(c), key) >= 0) {
                ret  = c ;
                break;
            }
        }
    }
    else {
        /* first key char is group key. */
        char group[2];
        mGroupPieceItem* g_item;
        group[0] = key[0]; 
        group[1] = '\0';
        g_item = _c(self)->searchGroupItem(self, group, TRUE);
        if (g_item) {
            list_for_each(i, &g_item->child) {
                mGroupPieceItem* c = (mGroupPieceItem*)ITEM_HEAD(i, list);
                if (self->itemCmp(_c(c)->getKey(c), key) >= 0) {
                    ret  = c ;
                    break;
                }
            }
        }
    }
    return ret;
}


mGroupPieceItem* mGroupItemManager_searchGroupItem(mGroupItemManager* self, const char* key, BOOL group)
{
    list_t* i;
    mGroupPieceItem* ret = NULL;
    if (group) {   
        list_for_each(i, &self->itemList) {
            mGroupPieceItem* c = (mGroupPieceItem*)ITEM_HEAD(i, list);
            if (self->itemCmp(key, _c(c)->getKey(c)) == 0) {
                ret  = c ;
                break;
            }
        }
    }
    else {
        /* first key char is group key. */
        char group[2];
        group[0] = key[0]; 
        group[1] = '\0';
        ret = _c(self)->searchGroupItem(self, group, TRUE);
        if (ret) {
            list_for_each(i, &ret->list) {
                mGroupPieceItem* c = (mGroupPieceItem*)ITEM_HEAD(i, list);
                if (self->itemCmp(key, _c(c)->getKey(c)) == 0) {
                    ret  = c ;
                    break;
                }
            }
        }
    }
    return ret;
}

BOOL mGroupItemManager_removeGroupItem(mGroupItemManager* self, const char* key, BOOL group)
{
    return TRUE;
}

void mGroupItemManager_clear(mGroupItemManager* self)
{
    return;
}

mItemIterator* mGroupItemManager_createItemIterator(mGroupItemManager* self)
{
    return _c(self)->createGroupItemIterator(self, NULL);
}

mItemIterator* mGroupItemManager_createGroupItemIterator(mGroupItemManager* self, const char* filter_key)
{
    mGroupItemIterator* iter = (mGroupItemIterator*)NEWEX(mGroupItemIterator, (DWORD)filter_key);
    if (NULL != iter) {
#if 0
        iter->current = iter->head = &self->itemList;
        iter->manager = (mAbstractItemManager*)self;
#else
        iter->parentIter = (mListItemIterator*)NEW(mListItemIterator);
        iter->parentIter->current = iter->parentIter->head = &self->itemList;
        iter->manager = (mAbstractItemManager*)self;

#endif
        return (mItemIterator*)iter;
    }
    return NULL;
}

BEGIN_MINI_CLASS(mGroupItemManager, mAbstractItemManager)
	CLASS_METHOD_MAP(mGroupItemManager, construct)
	CLASS_METHOD_MAP(mGroupItemManager, destroy)
	CLASS_METHOD_MAP(mGroupItemManager, searchGroupItem)
	CLASS_METHOD_MAP(mGroupItemManager, searchGreaterGroupItem)
	CLASS_METHOD_MAP(mGroupItemManager, addGroupItem)
	CLASS_METHOD_MAP(mGroupItemManager, removeGroupItem)
	CLASS_METHOD_MAP(mGroupItemManager, clear)
	CLASS_METHOD_MAP(mGroupItemManager, createItemIterator)
	CLASS_METHOD_MAP(mGroupItemManager, createGroupItemIterator)
END_MINI_CLASS

