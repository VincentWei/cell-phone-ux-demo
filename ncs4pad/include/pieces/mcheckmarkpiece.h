#ifndef _MGNCS_CHECKMARKPIECE_H
#define _MGNCS_CHECKMARKPIECE_H

#include <mgplus/mgplus.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum mCheckMarkPieceState {
    NCSN_CHECKMARKPIECE_OFF = (USER_PIECE_EVENT_BEGIN + 0x200),
    NCSN_CHECKMARKPIECE_ON,
};

enum mCheckMarkPieceProps {
    NCSP_CHECKMARK_COLOR = USER_PIECE_PROP_BEGIN + 0x810,
};

typedef struct _mCheckMarkPiece mCheckMarkPiece;
typedef struct _mCheckMarkPieceClass mCheckMarkPieceClass;

#define mCheckMarkPieceHeader(clss) \
	mStaticPieceHeader(clss) \
    enum mCheckMarkPieceState state; \
    ARGB color; \
    HGRAPHICS hgs; \
    HBRUSH brush; \
    int hgs_width; \
    int hgs_height;

struct _mCheckMarkPiece
{
	mCheckMarkPieceHeader(mCheckMarkPiece)
};

#define mCheckMarkPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls) \
    int (*getState)(clss*); \
    void (*setState)(clss*, enum mCheckMarkPieceState state); \
    void (*changeState)(clss*); \
    void (*resetState)(clss*);

struct _mCheckMarkPieceClass
{
	mCheckMarkPieceClassHeader(mCheckMarkPiece, mStaticPiece)
};

MGNCS_EXPORT extern mCheckMarkPieceClass g_stmCheckMarkPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_CHECKMARKPIECE_H */
