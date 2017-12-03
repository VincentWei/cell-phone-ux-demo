
#ifndef _MGUI_NCSCTRL_WRAPPAINT_H
#define _MGUI_NCSCTRL_WRAPPAINT_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    NCSP_WRAPPAINT_EFFECT = USER_PIECE_PROP_BEGIN + 1,
};

#define NCS_WRAPPAINT_ZOOM        1
#define NCS_WRAPPAINT_FOLDERPLANE 2

typedef struct _mWrapPaintPieceClass mWrapPaintPieceClass;
typedef struct _mWrapPaintPiece mWrapPaintPiece;

#define mWrapPaintPieceHeader(clss) \
    mStaticPieceHeader(clss) \
    mHotPiece* wrappiece;  \
    int effect;

#define mWrapPaintPieceClassHeader(clss, superCls) \
    mStaticPieceClassHeader(clss, superCls)

struct _mWrapPaintPiece
{   
    mWrapPaintPieceHeader(mWrapPaintPiece)
};

struct _mWrapPaintPieceClass
{   
    mWrapPaintPieceClassHeader(mWrapPaintPiece, mStaticPiece)
};

MGNCS_EXPORT extern mWrapPaintPieceClass g_stmWrapPaintPieceCls;

#ifdef __cplusplus
}
#endif
#endif /*_MGUI_NCSCTRL_WRAPPAINT_H*/
