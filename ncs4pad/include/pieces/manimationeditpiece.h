
#ifndef _MGNCS_ANIMATINEDITPIECE_H
#define _MGNCS_ANIMATINEDITPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT,
}text_align;

typedef struct _mAnimationEditPieceClass mAnimationEditPieceClass;
typedef struct _mAnimationEditPiece mAnimationEditPiece;

#define mAnimationEditPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)\
    void (*setContent)(HWND hwnd,mAnimationEditPiece *self, const char* str,text_align align); \
    int (*getContent)(mAnimationEditPiece *self, char *strbuff, int buf_len); \
    void (*append)(HWND hwnd,mAnimationEditPiece *self, const char* str); \
    void (*tailDelete)(HWND hwnd,mAnimationEditPiece *self, int num); 

struct _mAnimationEditPieceClass
{
	mAnimationEditPieceClassHeader(mAnimationEditPiece, mStaticPiece)
};

MGNCS_EXPORT extern mAnimationEditPieceClass g_stmAnimationEditPieceCls;

#define mAnimationEditPieceHeader(clss) \
	mStaticPieceHeader(clss) \
    char *text; \
    int content_length; \
    int text_offset; \
    PLOGFONT *pFont; \
    ARGB text_color;\
    ARGB text_shadow_color;
    

struct _mAnimationEditPiece
{
	mAnimationEditPieceHeader(mAnimationEditPiece)
};

#define ANIMATINEDITPIECE_PROP_BEGIN    USER_PIECE_PROP_BEGIN + 450
enum mAnimationEditPieceProps {   
    NCSP_ANIMATIONEDITPIECE_FONT = ANIMATINEDITPIECE_PROP_BEGIN,
    NCSP_ANIMATIONEDITPIECE_TEXTCOLOR,
    NCSP_ANIMATIONEDITPIECE_TEXTSHADOWCOLOR,
};

#ifdef __cplusplus
}
#endif
#endif /*_MGNCS_ANIMATINEDITPIECE_H*/
