#ifndef _MGNCS_ITEMPIECE_H
#define _MGNCS_ITEMPIECE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "global.h"
#include "register.h"

#define ITEMPIECE_WIDTH       SCREEN_W
#define ITEMPIECE_HEIGHT      50

typedef struct _mItemPiece mItemPiece;
typedef struct _mItemPieceClass mItemPieceClass;

enum mItemPieceState {
    ITEMPIECE_STATE_NORMAL,
    ITEMPIECE_STATE_PUSHED,
};

enum mItemPieceEvent {
    NCSN_ITEMPIECE_CONTENT_CLICKED = USER_PIECE_EVENT_BEGIN + 0x700,
    NCSN_ITEMPIECE_BUTTON_CLICKED,
    NCSN_ITEMPIECE_CHECKBOX_ON,
    NCSN_ITEMPIECE_CHECKBOX_OFF,
};

enum mItemPieceAccessoryControlStyle {
    ITEMPIECE_AC_INDICATORBUTTON,
    ITEMPIECE_AC_SWITCH,
    ITEMPIECE_AC_CHECKMARK,
    ITEMPIECE_AC_RADIO,
    ITEMPIECE_AC_INDICATOR,
};

#define mItemPieceHeader(clss) \
	mPanelPieceHeader(clss) \
    HDC privateDC; \
    int state; \
    unsigned margin; \
    unsigned border; \
    unsigned clearance; \
    int offsetLeft; \
    int offsetRight; \
    int align; \
    int valign; \
    BOOL isTitlePiece; \
    ARGB titleColor; \
    ARGB subTitleColor; \
    ARGB detailColor; \
    ARGB bkColor; \
    PLOGFONT titleFont; \
    PLOGFONT subTitleFont; \
    PLOGFONT detailFont; \
    PLOGFONT defaultTitleFont; \
    PLOGFONT defaultSubTitleFont; \
    PLOGFONT defaultDetailFont; \
    mImagePiece *imagePiece; \
    mTextPiece *titlePiece; \
    mTextPiece *subTitlePiece; \
    mTextPiece *detailPiece; \
    mShapeTransRoundPiece *bkPiece; \
    mHotPiece *accessoryControlPiece; \
    int acStyle; \
    int childAlign[6]; \
    int childrenTotal;

struct _mItemPiece
{
	mItemPieceHeader(mItemPiece)
};

#define mItemPieceClassHeader(clss, superCls) \
	mPanelPieceClassHeader(clss, superCls) \
    HDC myDC; \
    void (*setLayout)(clss*, int); \
    void (*activeLayout)(clss*); \
    void (*setImage)(clss*, PBITMAP, int, int); \
    void (*setTitle)(clss*, const char*, PLOGFONT, ARGB*); \
    void (*setSubTitle)(clss*, const char*, PLOGFONT, ARGB*); \
    void (*setDetail)(clss*, const char*, PLOGFONT, ARGB*); \
    void (*setSubStance)(clss*, const char*, PLOGFONT, ARGB*); \
    void (*setAccessoryControl)(clss*, const char*, BOOL, int, DWORD); \
    void (*setCheckBoxState)(clss*, BOOL); \
    int (*getCheckBoxState)(clss*); \
    BOOL (*setItemRect)(clss*, const RECT*);

struct _mItemPieceClass
{
	mItemPieceClassHeader(mItemPiece, mPanelPiece)
};

enum mItemPieceProperty {
    NCSP_ITEMPIECE_MARGIN = USER_PIECE_PROP_BEGIN + 1,
    NCSP_ITEMPIECE_BORDER,
    NCSP_ITEMPIECE_CLEARANCE,
    NCSP_ITEMPIECE_VALIGN,
    NCSP_ITEMPIECE_BKCOLOR,
    NCSP_ITEMPIECE_GRADIENT_BKCOLOR,
    NCSP_ITEMPIECE_MAX
};

MGNCS_EXPORT extern mItemPieceClass g_stmItemPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_ITEMPIECE_H */
