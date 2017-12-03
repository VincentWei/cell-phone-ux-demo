/*! ============================================================================
 * @file mabstractitemmanager.c 
 * @Synopsis mAbstractItemManager 
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

#include "mabstractitemmanager.h"

static void mAbstractItemManager_construct(mAbstractItemManager *self, DWORD add_data)
{
	Class(mObject).construct((mObject*)self, add_data);
}

static void mAbstractItemManager_destroy(mAbstractItemManager *self)
{
    Class(mObject).destroy((mObject*)self);
}

static int  mAbstractItemManager_addItem(mAbstractItemManager* self, mPieceItem* item)
{
    return -1;
}

static void mAbstractItemManager_removeItem(mAbstractItemManager* self, mPieceItem* item)
{

}

static mItemIterator* mAbstractItemManager_createItemIterator(mAbstractItemManager* self)
{
    return NULL;
}

BEGIN_MINI_CLASS(mAbstractItemManager, mObject)
	CLASS_METHOD_MAP(mAbstractItemManager, construct)
	CLASS_METHOD_MAP(mAbstractItemManager, destroy)
	CLASS_METHOD_MAP(mAbstractItemManager, addItem)
	CLASS_METHOD_MAP(mAbstractItemManager, removeItem)
	CLASS_METHOD_MAP(mAbstractItemManager, createItemIterator)
END_MINI_CLASS

