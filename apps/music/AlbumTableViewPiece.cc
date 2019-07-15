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

#include "ActivityAlbum.hh"
#include "AlbumTableViewPiece.hh"

static void mAlbumTableViewPiece_construct(mAlbumTableViewPiece* panel, DWORD add_data);
static mTableViewItemPiece* mAlbumTableViewPiece_createItemForRow(mAlbumTableViewPiece* self, const mIndexPath* index);
static int mAlbumTableViewPiece_numberOfSections(mAlbumTableViewPiece* self);
static int mAlbumTableViewPiece_numberOfRowsInSection(mAlbumTableViewPiece* self, int section);
static void mAlbumTableViewPiece_rowDidSelectAtIndexPath(mAlbumTableViewPiece* self, const mIndexPath* indexpath);

static void mAlbumTableViewPiece_construct(mAlbumTableViewPiece* self, DWORD add_data)
{
    if (!add_data) {
        printf ("mAlbumTableViewPiece need ActivityAlbum* as addData!\n");
        assert (0);
    }

    Class(mTableViewPiece).construct((mTableViewPiece*)self, NCS_TABLEVIEW_INDEX_STYLE);

    self->activity = (PhoneAlbumActivity*)add_data;
//    _c(self)->showScrollBar(self, FALSE);
}

static mTableViewItemPiece* mAlbumTableViewPiece_createItemForRow(mAlbumTableViewPiece* self, const mIndexPath* index)
{
    RECT rc;
    mItemPiece *item;
    ALBUMINFO *album;
    BITMAP *icon;
    mTableViewItemPiece *tableview_item;

    MUSICPLAYERDATAIN *play_datain = self->activity->getPlayerDatain();

    assert (index->row < play_datain->n);
    album = &play_datain->pAlbum[index->row];

    icon = (BITMAP*)GetResource (Str2Key (album->photo));
    if (!icon) {
        icon = (BITMAP*)GetResource (Str2Key (DEFAULT_ALBUM_PHOTO));
    }

    assert (icon);
    item = CreateAlbumItem(icon, album->name, album->author);
    _c(item)->getRect (item, &rc);

    /* create mTableViewItem */
    tableview_item = NEWPIECE (mTableViewItemPiece);
    _c(tableview_item)->setRect (tableview_item, &rc);
    _c(tableview_item)->setUserPiece (tableview_item, (mHotPiece*)item);

    return tableview_item;
}

static int mAlbumTableViewPiece_numberOfSections(mAlbumTableViewPiece* self)
{
    /* only have 1 group */
    return 1;
}

static int mAlbumTableViewPiece_numberOfRowsInSection(mAlbumTableViewPiece* self, int section)
{
    MUSICPLAYERDATAIN *player_datain = self->activity->getPlayerDatain ();
    assert (player_datain);

    return player_datain->n;
}

static void mAlbumTableViewPiece_rowDidSelectAtIndexPath(mAlbumTableViewPiece* self, const mIndexPath* indexpath)
{
    mNavigationPanelPiece *nav;
    mNavigationItem *next_nav_item;
    PhoneAlbumActivity *activity;
    MUSICPLAYERDATAIN *player_datain;

    activity = self->activity;
    player_datain = activity->getPlayerDatain();

    assert (indexpath->row < player_datain->n);
    activity->setFocusAlbum (player_datain->pAlbum[indexpath->row]);
    player_datain->album = indexpath->row;
    activity->updateSongListViewData ();

    nav = activity->getNavigationController ();
    next_nav_item = activity->getSongListNavItem();
    assert(next_nav_item);

    _c(nav)->push(nav, next_nav_item);
}

BEGIN_MINI_CLASS(mAlbumTableViewPiece, mTableViewPiece)
        CLASS_METHOD_MAP(mAlbumTableViewPiece, construct)
        CLASS_METHOD_MAP(mAlbumTableViewPiece, rowDidSelectAtIndexPath)
        CLASS_METHOD_MAP(mAlbumTableViewPiece, createItemForRow)
        CLASS_METHOD_MAP(mAlbumTableViewPiece, numberOfSections)
        CLASS_METHOD_MAP(mAlbumTableViewPiece, numberOfRowsInSection)
END_MINI_CLASS
