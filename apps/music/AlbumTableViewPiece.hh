

#ifndef _ALBUMTABLEVIEW_H
#define _ALBUMTABLEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mAlbumTableViewPieceClass mAlbumTableViewPieceClass;
typedef struct _mAlbumTableViewPiece mAlbumTableViewPiece;


#define mAlbumTableViewPieceHeader(clss) \
    mTableViewPieceHeader(clss) \
    PhoneAlbumActivity *activity;

#define mAlbumTableViewPieceClassHeader(clss, superCls) \
    mTableViewPieceClassHeader(clss, superCls)

struct _mAlbumTableViewPiece
{   
    mAlbumTableViewPieceHeader(mAlbumTableViewPiece)
};

struct _mAlbumTableViewPieceClass
{
    mAlbumTableViewPieceClassHeader(mAlbumTableViewPiece, mTableViewPiece)
};

MGNCS_EXPORT extern mAlbumTableViewPieceClass g_stmAlbumTableViewPieceCls;


#ifdef __cplusplus
}
#endif
#endif /* _ALBUMTABLEVIEW_H */
