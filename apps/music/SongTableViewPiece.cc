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
#include "SongTableViewPiece.hh"

static void mSongTableViewPiece_construct(mSongTableViewPiece* panel, DWORD add_data);
static mTableViewItemPiece* mSongTableViewPiece_createItemForRow(mSongTableViewPiece* self, const mIndexPath* index);
static int mSongTableViewPiece_numberOfSections(mSongTableViewPiece* self);
static int mSongTableViewPiece_numberOfRowsInSection(mSongTableViewPiece* self, int section);
static void mSongTableViewPiece_rowDidSelectAtIndexPath(mSongTableViewPiece* self, const mIndexPath* indexpath);

static void mSongTableViewPiece_construct(mSongTableViewPiece* self, DWORD add_data)
{
    if (!add_data) {
        printf ("mSongTableViewPiece need ActivitySong* as addData!\n");
        assert (0);
    }

    Class(mTableViewPiece).construct((mTableViewPiece*)self, NCS_TABLEVIEW_INDEX_STYLE);

    self->activity = (PhoneAlbumActivity*)add_data;
    //_c(self)->showScrollBar(self, FALSE);
}

static mTableViewItemPiece* mSongTableViewPiece_createItemForRow(mSongTableViewPiece* self, const mIndexPath* index)
{
    RECT rc;
    mItemPiece *item;
    ALBUMINFO *album;

    mTableViewItemPiece *tableview_item;

    album = self->activity->getFocusAlbum ();

    assert (album);
    assert (album->pSong);

    item = CreateSongItem(
            album->pSong[index->row].name, 
            album->pSong[index->row].author, 
            album->pSong[index->row].totaltime_str);

    /* create mTableViewItem */
    tableview_item = NEWPIECE (mTableViewItemPiece);
    _c(item)->getRect (item, &rc);
    _c(tableview_item)->setRect (tableview_item, &rc);
    _c(tableview_item)->setUserPiece (tableview_item, (mHotPiece*)item);

    return tableview_item;
}

static int mSongTableViewPiece_numberOfSections(mSongTableViewPiece* self)
{
    /* only have 1 group */
    return 1;
}

static int mSongTableViewPiece_numberOfRowsInSection(mSongTableViewPiece* self, int section)
{
    ALBUMINFO *album = self->activity->getFocusAlbum ();
    assert (album);

    return album->n;
}

static void mSongTableViewPiece_rowDidSelectAtIndexPath(mSongTableViewPiece* self, const mIndexPath* indexpath)
{
    PhoneAlbumActivity *activity;
    ALBUMINFO *album;
    MUSICPLAYERDATAIN *player_datain;

    album = self->activity->getFocusAlbum ();
    assert (album);
    assert (album->pSong);

    assert (indexpath->row < album->n);
    activity = self->activity;

    player_datain = activity->getPlayerDatain();
    player_datain->index = indexpath->row;
    player_datain->mode = PhoneAlbumActivity::MP_M_SINGLE;
    //activity->listEnterPlayWin(NULL,&album->pSong[indexpath->row]);
    activity->listEnterPlayWin(album,&album->pSong[indexpath->row]);
}

BEGIN_MINI_CLASS(mSongTableViewPiece, mTableViewPiece)
        CLASS_METHOD_MAP(mSongTableViewPiece, construct)
        CLASS_METHOD_MAP(mSongTableViewPiece, rowDidSelectAtIndexPath)
        CLASS_METHOD_MAP(mSongTableViewPiece, createItemForRow)
        CLASS_METHOD_MAP(mSongTableViewPiece, numberOfSections)
        CLASS_METHOD_MAP(mSongTableViewPiece, numberOfRowsInSection)
END_MINI_CLASS
