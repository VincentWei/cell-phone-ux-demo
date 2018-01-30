#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "register.h"

#include "SMSActivity.hh"

#include "SMSListViewPiece.hh"
#include "TimeService.hh"

static void mSMSListPiece_construct(mSMSListPiece *self, DWORD add_data)
{
    Class(mTableViewPiece).construct((mTableViewPiece*)self, NCS_TABLEVIEW_INDEX_STYLE);
    self->act = (SMSActivity *)add_data;
    self->editMode = FALSE;
    self->list_head = NULL;
    self->list_end = NULL;
}

static int mSMSListPiece_numberOfSections(mSMSListPiece* self)
{
    return 1;
}
static int mSMSListPiece_numberOfRowsInSection(mSMSListPiece* self, int section)
{
    return self->act->smsGetListNumberNo();
}

static mItemPiece *smsCreateUserPiece(mSMSListPiece* self,const mIndexPath* indexpath)
{
    mItemPiece *smsitem = NULL;
    SMSListData *data = (SMSListData *)calloc(1,sizeof(SMSListData));

    assert(data);
    data->next = NULL;
    if (self->list_head == NULL)
    {
        self->list_head = self->list_end = data; 
    }
    else
    {
        self->list_end->next = data; 
        self->list_end = self->list_end->next;
    }
    self->act->smsGetListData(data->name,data->smsbody,data->time,indexpath->row);
    
    smsitem = CreateSMSItem(data->name,data->smsbody,data->time);
    return smsitem;
}

static mTableViewItemPiece* mSMSListPiece_createItemForRow(
                mSMSListPiece* self, const mIndexPath* indexpath)
{
    mTableViewItemPiece *item = NEWPIECEEX(mTableViewItemPiece,0);

    RECT rc = {0,0,SMS_ITEM_W,SMS_ITEM_H};
    _c(item)->setRect(item,&rc);

    mItemPiece *smsitem  = smsCreateUserPiece(self,indexpath);
    _c(smsitem)->setRect(smsitem,&rc);

    _c(item)->setUserPiece(item,(mHotPiece*)smsitem);
    return item;
}

static void mSMSListPiece_rowDidSelectAtIndexPath(
        mSMSListPiece* self, const mIndexPath* indexpath)
{
    self->act->listEnterDetailWin(indexpath);
}

static void mSMSListPiece_destroy(mSMSListPiece *self)
{
    SMSListData *data = self->list_head;
    for (;data;)
    {
        self->list_head = self->list_head->next;
        free(data);
        data = self->list_head;
    }
    Class(mTableViewPiece).destroy((mTableViewPiece*)self);
}

static void mSMSListPiece_onCommitDeleteRowAtIndexPath(
        mSMSListPiece* self, const mIndexPath* indexpath)
{
        /* edit datasource. */
    self->act->smsDeleteRow(indexpath->row);

    _c(self)->deleteRowAtIndexPath(self, indexpath, NCS_TABLEVIEW_ANIMATIONZOOM);
}

static BOOL mSMSListPiece_willSelectRowAtIndexPath(mSMSListPiece* self, const mIndexPath* indexpath)
{
    if (self->editMode) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

static int mSMSListPiece_processMessage(mSMSListPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK) {
        if (message == MSG_LBUTTONUP) {
            float v_x, v_y;
            mHotPiece * hotPiece = self->act->m_editBtn;
            QueryMouseMoveVelocity(&v_x, &v_y);
            printf("%s:v_x=%.2f, v_y=%.2f, set to 10000 forcely\n", __FUNCTION__, v_x, v_y);
            if (hotPiece && v_x > 200.0 && abs(v_y) < 100.0) {
                _c(hotPiece)->processMessage(hotPiece, MSG_LBUTTONDOWN, 0, 0, owner);
                _c(hotPiece)->processMessage(hotPiece, MSG_LBUTTONUP, 0, 0, owner);
                //return 0;
            }
        }
    }

    return Class(mTableViewPiece).processMessage((mTableViewPiece*)self, message, wParam, lParam, owner);
}

BEGIN_MINI_CLASS(mSMSListPiece, mTableViewPiece)
	CLASS_METHOD_MAP(mSMSListPiece, construct)
	CLASS_METHOD_MAP(mSMSListPiece, processMessage)
	CLASS_METHOD_MAP(mSMSListPiece, numberOfSections)
	CLASS_METHOD_MAP(mSMSListPiece, numberOfRowsInSection)
	CLASS_METHOD_MAP(mSMSListPiece, createItemForRow)
	CLASS_METHOD_MAP(mSMSListPiece, rowDidSelectAtIndexPath)
	CLASS_METHOD_MAP(mSMSListPiece, willSelectRowAtIndexPath)
	CLASS_METHOD_MAP(mSMSListPiece, onCommitDeleteRowAtIndexPath)
	CLASS_METHOD_MAP(mSMSListPiece, destroy)
END_MINI_CLASS

