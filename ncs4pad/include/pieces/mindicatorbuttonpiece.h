#ifndef _MGNCS_INDICATORBUTTONPIECE_H
#define _MGNCS_INDICATORBUTTONPIECE_H

#include <mgplus/mgplus.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum mIndicatorButtonPieceState {
    NCS_IBP_CLICKED = 0,
    NCS_IBP_PUSHED,
};

typedef struct _mIndicatorButtonPieceImages {
    BITMAP *bmp[2];
} mIndicatorButtonPieceImages;

enum mIndicatorButtonPieceProp {
    NCSP_INDICATORBUTTON_IMAGE = USER_PIECE_PROP_BEGIN + 1,
};

typedef struct _mIndicatorButtonPiece mIndicatorButtonPiece;
typedef struct _mIndicatorButtonPieceClass mIndicatorButtonPieceClass;

#define mIndicatorButtonPieceHeader(clss) \
	mPanelPieceHeader(clss) \
    enum mIndicatorButtonPieceState state; \
    mImagePiece *imagePiece; \
    mIndicatorButtonPieceImages *images; \
    ARGB colors[4][4]; \
    float colorsPos[4]; \
    HGRAPHICS hgs; \
    HBRUSH brushSolid; \
    HBRUSH brushGradient; \
    int hgs_width; \
    int hgs_height;

struct _mIndicatorButtonPiece
{
	mIndicatorButtonPieceHeader(mIndicatorButtonPiece)
};

#define mIndicatorButtonPieceClassHeader(clss, superCls) \
	mPanelPieceClassHeader(clss, superCls)

struct _mIndicatorButtonPieceClass
{
	mIndicatorButtonPieceClassHeader(mIndicatorButtonPiece, mPanelPiece)
};

MGNCS_EXPORT extern mIndicatorButtonPieceClass g_stmIndicatorButtonPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_INDICATORBUTTONPIECE_H */
