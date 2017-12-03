#ifndef _MGNCS_TABLEVIEWITEMPIECEPIECE_H
#define _MGNCS_TABLEVIEWITEMPIECEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _mTableViewItemPiece mTableViewItemPiece;
typedef struct _mTableViewItemPieceClass mTableViewItemPieceClass;

enum mTableViewItemPieceMode {
    NCS_TABLEVIEWITEMPIECE_NORMAL = 0,
    NCS_TABLEVIEWITEMPIECE_EDIT, 
};

enum mTableViewItemPieceEvent {
    NCSN_TABLEVIEWITEMPIECE_SWITCHCLICKED = USER_PIECE_EVENT_BEGIN + 0X200,
    NCSN_TABLEVIEWITEMPIECE_DELBTNCLICKED,
    NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED,
};
#define NCS_TABLEVIEWITEM_BKCOLOR_VALUE     0xFFFFFFFF
#define NCS_TABLEVIEWITEM_HIGHLIGHT_VALUE   0xFF0000FF

#define mTableViewItemPieceHeader(clss) \
	mPanelPieceHeader(clss) \
    enum mTableViewItemPieceMode mode; \
    mHotPiece *switchPiece; \
    mHotPiece *userPiece;   \
    mHotPiece *deletePiece; \
    POINT ptSwitchPiece[2]; \
    POINT ptUserPiece[2];   \
    POINT ptDeletePiece[2]; \
    BOOL userPieceIsLocked; \
    BOOL deletePieceIsAppear; \
    DWORD add_data; \
    const char* picture;\
    const char* text; \
    mHotPiece* accessory; \
    BOOL highlight;\
    BOOL pushDown;\
    MGEFF_ANIMATION modeChangeAnimation;\
    MGEFF_ANIMATION switchStateChangeAnimation;

struct _mTableViewItemPiece
{
	mTableViewItemPieceHeader(mTableViewItemPiece)
};

#define mTableViewItemPieceClassHeader(clss, superCls) \
	mPanelPieceClassHeader(clss, superCls) \
    void (*changeMode)(clss*); \
    void (*resetEditMode)(clss*); \
    void (*setHighlight)(clss*, BOOL); \
    void (*setUserPiece)(clss*, mHotPiece*); \
    mHotPiece* (*getSwitchPiece)(clss*);  \
    mHotPiece* (*getUserPiece)(clss*); \
    mHotPiece* (*getDeletePiece)(clss*);

struct _mTableViewItemPieceClass
{
	mTableViewItemPieceClassHeader(mTableViewItemPiece, mPanelPiece)
};

MGNCS_EXPORT extern mTableViewItemPieceClass g_stmTableViewItemPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_TABLEVIEWITEMPIECEPIECE_H */
