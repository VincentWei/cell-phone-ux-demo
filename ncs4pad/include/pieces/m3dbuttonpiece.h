#ifndef _MGNCS_3DBUTTONPIECE_H
#define _MGNCS_3DBUTTONPIECE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _m3DButtonPiece m3DButtonPiece;
typedef struct _m3DButtonPieceClass m3DButtonPieceClass;

enum m3DButtonPieceProp
{
    NCSP_3DBUTTONPIECE_TEXT = USER_PIECE_PROP_BEGIN + 1,
    NCSP_3DBUTTONPIECE_TEXTLOGFONT,
    NCSP_3DBUTTONPIECE_TEXTCOLOR,
};

#define m3DButtonPieceHeader(clss) \
	mButtonPanelPieceHeader(clss) \
    mTextPiece *txtPiece; \
    PLOGFONT txtFont;

struct _m3DButtonPiece
{
	m3DButtonPieceHeader(m3DButtonPiece)
};

#define m3DButtonPieceClassHeader(clss, superCls) \
	mButtonPanelPieceClassHeader(clss, superCls)

struct _m3DButtonPieceClass
{
	m3DButtonPieceClassHeader(m3DButtonPiece, mButtonPanelPiece)
};

MGNCS_EXPORT extern m3DButtonPieceClass g_stm3DButtonPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_3DBUTTONPIECE_H */
