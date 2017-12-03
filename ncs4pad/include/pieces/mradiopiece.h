#ifndef _MGNCS_RADIOPIECE_H
#define _MGNCS_RADIOPIECE_H

#include <mgplus/mgplus.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum mRadioPieceState {
    NCSN_RADIOPIECE_OFF = (USER_PIECE_EVENT_BEGIN + 0x200),
    NCSN_RADIOPIECE_ON,
};

enum mRadioPieceProps {
    NCSP_RADIO_COLOR = USER_PIECE_PROP_BEGIN + 0x810,
};

typedef struct _mRadioPiece mRadioPiece;
typedef struct _mRadioPieceClass mRadioPieceClass;

#define mRadioPieceHeader(clss) \
	mStaticPieceHeader(clss) \
    enum mRadioPieceState state; \
    ARGB color; \
    HGRAPHICS hgs; \
    HBRUSH brush; \
    int hgs_width; \
    int hgs_height;

struct _mRadioPiece
{
	mRadioPieceHeader(mRadioPiece)
};

#define mRadioPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls) \
    int (*getState)(clss*); \
    void (*setState)(clss*, enum mRadioPieceState state); \
    void (*changeState)(clss*); \
    void (*resetState)(clss*);

struct _mRadioPieceClass
{
	mRadioPieceClassHeader(mRadioPiece, mStaticPiece)
};

MGNCS_EXPORT extern mRadioPieceClass g_stmRadioPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_RADIOPIECE_H */
