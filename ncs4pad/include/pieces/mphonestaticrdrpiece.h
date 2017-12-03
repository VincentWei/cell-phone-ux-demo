#ifndef _MGNCS_PHONESTATICRDRPIECE_H
#define _MGNCS_PHONESTATICRDRPIECE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <mgplus/mgplus.h>
#include <minigui/common.h>

typedef struct _mPhoneStaticRDRPieceClass mPhoneStaticRDRPieceClass;
typedef struct _mPhoneStaticRDRPiece mPhoneStaticRDRPiece;

#define mPhoneStaticRDRPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)\

struct _mPhoneStaticRDRPieceClass
{
	mPhoneStaticRDRPieceClassHeader(mPhoneStaticRDRPiece, mStaticPiece)
};

MGNCS_EXPORT extern mPhoneStaticRDRPieceClass g_stmPhoneStaticRDRPieceCls;

#define mPhoneStaticRDRPieceHeader(clss) \
	mStaticPieceHeader(clss) \
    PLOGFONT main_font;     \
    PLOGFONT sub_font;      \
    RECT main_rc;           \
    RECT sub_rc;            \
    char* main_text_str;     \
    char* sub_text_str;      \
    ARGB main_font_color;\
    ARGB sub_font_color;

struct _mPhoneStaticRDRPiece
{
	mPhoneStaticRDRPieceHeader(mPhoneStaticRDRPiece)
};

#define PHONESTATICRDRPIECE_PROP_BEGIN    USER_PIECE_PROP_BEGIN + 400

enum mPhoneStaticRDRPieceProps {   
    NCSP_PHONESTATICRDRPIECE_MAIN_TEXT = PHONESTATICRDRPIECE_PROP_BEGIN,
    NCSP_PHONESTATICRDRPIECE_SUB_TEXT,
    NCSP_PHONESTATICRDRPIECE_MAIN_FONT,
    NCSP_PHONESTATICRDRPIECE_SUB_FONT,
    NCSP_PHONESTATICRDRPIECE_MAIN_COLOR,
    NCSP_PHONESTATICRDRPIECE_SUB_COLOR
};


#ifdef __cplusplus
}
#endif

#endif

