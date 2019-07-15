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

#include "global.h"

#include "ContactsLoadHelper.hh"
#include "ActivityContacts.hh"
#include "ContactsTableViewPiece.hh"

static void mContactsTableViewPiece_construct(mContactsTableViewPiece* panel, DWORD add_data);
static mTableViewItemPiece* mContactsTableViewPiece_createItemForRow(mContactsTableViewPiece* self, const mIndexPath* index);
static int mContactsTableViewPiece_numberOfSections(mContactsTableViewPiece* self);
static int mContactsTableViewPiece_numberOfRowsInSection(mContactsTableViewPiece* self, int section);
static void mContactsTableViewPiece_rowDidSelectAtIndexPath(mContactsTableViewPiece* self, const mIndexPath* indexpath);

static void mContactsTableViewPiece_construct(mContactsTableViewPiece* self, DWORD add_data)
{
    if (!add_data) {
        printf ("The piece need to know *activity from addData!\n");
        assert (0);
    }
    self->activity = (PhoneContactsActivity *)add_data;
    Class(mTableViewPiece).construct((mTableViewPiece*)self, NCS_TABLEVIEW_INDEXLOCATE_STYLE);
}

static mTableViewItemPiece* mContactsTableViewPiece_createItemForRow(mContactsTableViewPiece* self, const mIndexPath* index)
{
    RECT rc= {0, 0, 0, 0};
    ContactsInfo *contact;
    ContactsData *sections;
    BITMAP *bmp;
    mItemPiece *item;

    mTableViewItemPiece *tableview_item = NEWPIECE (mTableViewItemPiece);

    sections = self->activity->getContactsData();
    assert((index->section < GROUPSIZE) && (index->row < (int)sections->size()));

    contact = sections->at(index->section)->group[index->row];
    assert (contact);

    bmp = (BITMAP*)GetResource (Str2Key(contact->icon_path.c_str()));
    if (!bmp) {
        bmp = (BITMAP *)GetResource (Str2Key (CONTACT_DEFAULT_PHOTO));
    }
    assert (bmp);
          
    item = CreateContactItem (bmp, contact->name.c_str(), contact->dial_num.c_str());
    _c(item)->getRect (item, &rc);
    assert (RECTW (rc) && RECTH(rc));

    _c(tableview_item)->setRect (tableview_item, &rc);
    _c(tableview_item)->setUserPiece (tableview_item, (mHotPiece*)item);

    return tableview_item;
}

static int mContactsTableViewPiece_numberOfSections(mContactsTableViewPiece* self)
{
    ContactsData *contact_data;
    contact_data = self->activity->getContactsData();

    return (contact_data->size() == 0) ? 0 : contact_data->size();
}

static int mContactsTableViewPiece_numberOfRowsInSection(mContactsTableViewPiece* self, int section)
{
    ContactsData *contact_data;
    int group_size;

    contact_data = self->activity->getContactsData();
    group_size = contact_data->at(section)->group.size();

    assert (section < (int)contact_data->size());

    return group_size;
}

static void mContactsTableViewPiece_rowDidSelectAtIndexPath(mContactsTableViewPiece* self, const mIndexPath* indexpath)
{
    mNavigationPanelPiece *nav;
    mNavigationItem *nextItem;
    ContactsInfo *info;
    ContactsData *contactData;
    PhoneContactsActivity *activity;

    activity = self->activity;
    contactData = activity->getContactsData();
    info = contactData->at(indexpath->section)->group[indexpath->row];

    activity->updateDetailNavItem (info);

    nav = activity->getNavigationController();
    nextItem = activity->getDedailNavItem();
    assert(nextItem);

    _c(nav)->push(nav, nextItem);
}

static const char* mContactsTableViewPiece_titleForSection(mContactsTableViewPiece* self, int section)
{
    ContactsData *contact_data = self->activity->getContactsData();

    return contact_data->at(section)->titlename.c_str();
}

static const char* mContactsTableViewPiece_indexForSection(mContactsTableViewPiece* self, int section)
{
    return _c(self)->titleForSection (self, section);
}

BEGIN_MINI_CLASS(mContactsTableViewPiece, mTableViewPiece)
        CLASS_METHOD_MAP(mContactsTableViewPiece, construct)
        CLASS_METHOD_MAP(mContactsTableViewPiece, rowDidSelectAtIndexPath)
        CLASS_METHOD_MAP(mContactsTableViewPiece, createItemForRow)
        CLASS_METHOD_MAP(mContactsTableViewPiece, numberOfSections)
        CLASS_METHOD_MAP(mContactsTableViewPiece, numberOfRowsInSection)
        CLASS_METHOD_MAP(mContactsTableViewPiece, titleForSection)
        CLASS_METHOD_MAP(mContactsTableViewPiece, indexForSection)
END_MINI_CLASS
