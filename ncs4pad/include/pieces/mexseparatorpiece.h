
#ifndef _MGUI_NCSCTRL_EX_SEPARATORPIECE_H
#define _MGUI_NCSCTRL_EX_SEPARATORPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    NCSP_EXSEPARATOR_DIRECTION = USER_PIECE_PROP_BEGIN + 1,
    NCSP_EXSEPARATOR_COLOR,
    NCSP_EXSEPARATOR_SIZE,
};

enum {
    EXSEPARATOR_DIRECTION_VERTICAL,
    EXSEPARATOR_DIRECTION_HORIZONTAL,
};


typedef struct _mExSeparatorPieceClass mExSeparatorPieceClass;
typedef struct _mExSeparatorPiece mExSeparatorPiece;

#define mExSeparatorPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mExSeparatorPieceClass
{
	mExSeparatorPieceClassHeader(mExSeparatorPiece, mStaticPiece)
};

MGNCS_EXPORT extern mExSeparatorPieceClass g_stmExSeparatorPieceCls;

#define mExSeparatorPieceHeader(clss) \
	mStaticPieceHeader(clss) \
    int size; \
    DWORD color; \
    int direction;

struct _mExSeparatorPiece
{
	mExSeparatorPieceHeader(mExSeparatorPiece)
};


#ifdef __cplusplus
}
#endif

#endif

