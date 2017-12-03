
#ifndef _MGUI_NCSCTRL_TEXTPIECE_H
#define _MGUI_NCSCTRL_TEXTPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    NCSP_TEXTPIECE_LOGFONT= USER_PIECE_PROP_BEGIN + 1,
    NCSP_TEXTPIECE_TEXTCOLOR,
    NCSP_TEXTPIECE_TEXTSHADOWCOLOR,
    NCSP_TEXTPIECE_MAXLEN,
};

typedef struct _mTextPieceClass mTextPieceClass;
typedef struct _mTextPiece mTextPiece;

#define mTextPieceHeader(clss) \
    mLabelPieceHeader(clss) \
    DWORD color; /* ARGB */ \
    DWORD shadow_color; /* ARGB */ \
    BOOL isShadow;  \
    int maxLen;  \
    PLOGFONT font;

#define mTextPieceClassHeader(clss, superCls) \
    mLabelPieceClassHeader(clss, superCls)

struct _mTextPiece
{   
    mTextPieceHeader(mTextPiece)
};

struct _mTextPieceClass
{   
    mTextPieceClassHeader(mTextPiece, mLabelPiece)
};

MGNCS_EXPORT extern mTextPieceClass g_stmTextPieceCls;

#ifdef __cplusplus
}
#endif
#endif /*_MGUI_NCSCTRL_TEXTPIECE_H*/
