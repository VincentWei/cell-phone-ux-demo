#ifndef MHIGHLIGHTPIECE_H
#define MHIGHLIGHTPIECE_H

#include "mtextpiece.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mHighlightPieceClass mHighlightPieceClass;
typedef struct _mHighlightPiece mHighlightPiece;

#define mHighlightPieceClassHeader(clss, superCls) \
	mTextPieceClassHeader(clss, superCls) \

struct _mHighlightPieceClass
{
	mHighlightPieceClassHeader(mHighlightPiece, mTextPiece)
};

MGNCS_EXPORT extern mHighlightPieceClass g_stmHighlightPieceCls;

#define mHighlightPieceHeader(clss) \
	mTextPieceHeader(clss) \
    int highlight_x;    \
    HDC dc_white_font;

struct _mHighlightPiece
{
	mHighlightPieceHeader(mHighlightPiece)
};

enum mHighlightPieceProps {
	NCSP_HIGHLIGHTPIECE_HIGHTX     = USER_PIECE_PROP_BEGIN + 10,
};


#ifdef __cplusplus
}
#endif

#endif

