#ifndef _MGNCS_INDICATORPIECE_H
#define _MGNCS_INDICATORPIECE_H

#include <mgplus/mgplus.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum mIndicatorPieceProps {
    NCSP_INDICATOR_COLOR = USER_PIECE_PROP_BEGIN + 0x810,
};

typedef struct _mIndicatorPiece mIndicatorPiece;
typedef struct _mIndicatorPieceClass mIndicatorPieceClass;

#define mIndicatorPieceHeader(clss) \
	mStaticPieceHeader(clss) \
    ARGB color; \
    HGRAPHICS hgs; \
    HBRUSH brush; \
    int hgs_width; \
    int hgs_height;

struct _mIndicatorPiece
{
	mIndicatorPieceHeader(mIndicatorPiece)
};

#define mIndicatorPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mIndicatorPieceClass
{
	mIndicatorPieceClassHeader(mIndicatorPiece, mStaticPiece)
};

MGNCS_EXPORT extern mIndicatorPieceClass g_stmIndicatorPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_INDICATORPIECE_H */
