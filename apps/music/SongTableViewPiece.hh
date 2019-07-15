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


#ifndef _SONGTABLEVIEW_H
#define _SONGTABLEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSongTableViewPieceClass mSongTableViewPieceClass;
typedef struct _mSongTableViewPiece mSongTableViewPiece;


#define mSongTableViewPieceHeader(clss) \
    mTableViewPieceHeader(clss) \
    PhoneAlbumActivity *activity;

#define mSongTableViewPieceClassHeader(clss, superCls) \
    mTableViewPieceClassHeader(clss, superCls)

struct _mSongTableViewPiece
{   
    mSongTableViewPieceHeader(mSongTableViewPiece)
};

struct _mSongTableViewPieceClass
{
    mSongTableViewPieceClassHeader(mSongTableViewPiece, mTableViewPiece)
};

MGNCS_EXPORT extern mSongTableViewPieceClass g_stmSongTableViewPieceCls;


#ifdef __cplusplus
}
#endif
#endif /* _SONGTABLEVIEW_H */
