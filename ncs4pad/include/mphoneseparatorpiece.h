#ifndef _MGNCS_PHONESEPARATORPIECE_H
#define _MGNCS_PHONESEPARATORPIECE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <mgplus/mgplus.h>

typedef enum{
    PHONESEPARATORPIECE_VERT = NCS_PIECE_PAINT_VERT,
    PHONESEPARATORPIECE_HERT
}PHONESEPARATORPIECE_DIRECT_MODE_E;

typedef struct _mPhoneSeparatorPieceClass mPhoneSeparatorPieceClass;
typedef struct _mPhoneSeparatorPiece mPhoneSeparatorPiece;

#define mPhoneSeparatorPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mPhoneSeparatorPieceClass
{
	mPhoneSeparatorPieceClassHeader(mPhoneSeparatorPiece, mStaticPiece)
};

MGNCS_EXPORT extern mPhoneSeparatorPieceClass g_stmPhoneSeparatorPieceCls;

#define mPhoneSeparatorPieceHeader(clss) \
	mStaticPieceHeader(clss) \
    DWORD darkercolor;           \
    DWORD lightercolor;\
    PHONESEPARATORPIECE_DIRECT_MODE_E direct_mode;

struct _mPhoneSeparatorPiece
{
	mPhoneSeparatorPieceHeader(mPhoneSeparatorPiece)
};


#define PHONESEPARATORPIECE_PROP_BEGIN    USER_PIECE_PROP_BEGIN + 300

enum mPhoneSeparatorPieceProps {   
    NCSP_PHONESEPARATORPIECE_DARKER_COLOR = PHONESEPARATORPIECE_PROP_BEGIN,
    NCSP_PHONESEPARATORPIECE_LIGHTER_COLOR,
    NCSP_PHONESEPARATORPIECE_DIRECT_MODE,
};

#ifdef __cplusplus
}
#endif

#endif

