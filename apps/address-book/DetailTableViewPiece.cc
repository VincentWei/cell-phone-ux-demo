#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgncs/mgncs.h>
#include "museritem.h"
#include "ContactsLoadHelper.hh"
#include "ActivityContacts.hh"
#include "DetailTableViewPiece.hh"
#include "ContactService.hh"

static mItemPiece *detailCreateItemPiece(ContactsInfo *info, int row);

static void mDetailTableViewPiece_construct(mDetailTableViewPiece *self, DWORD add_data) {
    Class(mTableViewPiece).construct((mTableViewPiece *)self, NCS_TABLEVIEW_INDEX_STYLE);
    _c(self)->showScrollBar(self, FALSE);
    self->data = NULL;
    self->act = (DWORD)add_data;
}

void mDetailTableViewPiece_setContactsData (mDetailTableViewPiece *self, ContactsInfo *data)
{
    self->data = data;
}

static mTableViewItemPiece *mDetailTableViewPiece_createItemForRow(mDetailTableViewPiece *self, const mIndexPath *indexpath)
{
    int row = indexpath->row;
    RECT rc;
    mItemPiece *item_piece = NULL;

    mTableViewItemPiece *tableview_item = NEWPIECE(mTableViewItemPiece);

    assert (self->data);

    SetRect(&rc,0,0,ACTIVITY_W,CONSTACT_DETAILTABLE_ITEM_H);
    _c(tableview_item)->setRect(tableview_item, &rc);

    item_piece = detailCreateItemPiece (self->data, row);
    _c(item_piece)->setRect(item_piece, &rc);


    _c(tableview_item)->setUserPiece(tableview_item, (mHotPiece *)item_piece);

    return tableview_item;
}

static int mDetailTableViewPiece_numberOfSections(mDetailTableViewPiece *self)
{
    if (self->data) {
        return 1; /* only one group */
    }

    return 0;
}

static int mDetailTableViewPiece_numberOfRowsInSection(mDetailTableViewPiece *self, int section){
    return 3;
}

static void mDetailTableViewPiece_rowDidSelectAtIndexPath(mDetailTableViewPiece *self, const mIndexPath *indexpath){
    int row = indexpath->row;
    Intent *intent;
    PhoneContactsActivity *act = (PhoneContactsActivity *)self->act;
    ContactsInfo *info = self->data;

    intent= act->getIntent ();

    switch(row){
        case 0:
            {
            #if 0
                std::string str_temp;
                str_temp = GET_CONTACTSERVICE()->getNameByDialNum(info->dial_num);
                if(!str_temp.empty()) {
                    intent->putExtra("number", str_temp);
                } else {
                    intent->putExtra("number", info->dial_num);
                }
            #else
                if (!info->name.empty())
                {
                    intent->putExtra("number", info->name);
                }
                else
                {
                    intent->putExtra("number", info->dial_num);
                }
            #endif
                ACTIVITYSTACK->switchTo("PhoneCallActivity", intent);
            }
            break;
        case 1:
            printf("push sms activity\n");
            break;
        case 2:
            printf("push ring default activity\n");
            break;
        default:
            break;
    }
}

static void mDetailTableViewPiece_destroy(mDetailTableViewPiece *self) {
    Class(mTableViewPiece).destroy((mTableViewPiece*)self);
}

static mItemPiece *detailCreateItemPiece(ContactsInfo *info, int row){
    mItemPiece *item = NULL;

    switch(row){
        case 0:
            printf ("num: %s\n", info->dial_num.c_str());
            item = CreateBriefInfoItem("Mobile Phone", info->dial_num.c_str());
            break;
        case 1:
            printf ("sms: %s\n", info->sms_num.c_str());
            item = CreateBriefInfoItem("              SMS", info->sms_num.c_str());
            break;
        case 2:
            //item = CreateIndicatorItem("Ring Default", NULL, NULL);
            item = CreateIndicatorItem("Ring Default", "", "");
            break;
        default:
            break;
    }

    return item;
}

BEGIN_MINI_CLASS(mDetailTableViewPiece, mTableViewPiece)
    CLASS_METHOD_MAP(mDetailTableViewPiece, construct)
    CLASS_METHOD_MAP(mDetailTableViewPiece, rowDidSelectAtIndexPath)
    CLASS_METHOD_MAP(mDetailTableViewPiece, createItemForRow)
    CLASS_METHOD_MAP(mDetailTableViewPiece, numberOfSections)
    CLASS_METHOD_MAP(mDetailTableViewPiece, numberOfRowsInSection)
    CLASS_METHOD_MAP(mDetailTableViewPiece, destroy)
    CLASS_METHOD_MAP(mDetailTableViewPiece, setContactsData)
END_MINI_CLASS
