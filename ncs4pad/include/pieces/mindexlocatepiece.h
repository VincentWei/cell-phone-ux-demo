#ifndef _MGNCS_INDEXLOCATEPIECE_H
#define _MGNCS_INDEXLOCATEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mpanelpiece.h"

    typedef struct _mIndexLocatePiece mIndexLocatePiece;
    typedef struct _mIndexLocatePieceClass mIndexLocatePieceClass;

    enum mIndexLocatePieceEvent {
	NCSN_INDEXLOCATEPIECE_TOUCHED = USER_PIECE_EVENT_BEGIN +  0x200,
    };

#define mIndexLocatePieceHeader(clss)		\
    mPanelPieceHeader(clss)			\
    mHotPiece* parentPiece;			\
    mHotPiece* backgroundPiece;			\
    int touchedFlag;

    struct _mIndexLocatePiece
    {
	mIndexLocatePieceHeader(mIndexLocatePiece)
    };

#define mIndexLocatePieceClassHeader(clss, superCls)		\
    mPanelPieceClassHeader(clss, superCls)			\
    void (*reloadData)(clss* self);

    struct _mIndexLocatePieceClass
    {
	mIndexLocatePieceClassHeader(mIndexLocatePiece, mPanelPiece)
    };

    MGNCS_EXPORT extern mIndexLocatePieceClass g_stmIndexLocatePieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_INDEXLOCATEPIECE_H */
