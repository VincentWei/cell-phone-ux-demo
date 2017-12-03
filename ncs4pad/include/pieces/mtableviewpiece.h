#ifndef _MGNCS_TABLEVIEWPIECE_H
#define _MGNCS_TABLEVIEWPIECE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mpieceitem.h"
#include "mpanelpiece.h"
#include "mtableviewitempiece.h"
#include "mscrollviewpiece.h"
#include "mindexlocatepiece.h"

typedef struct _mTableViewPiece mTableViewPiece;
typedef struct _mTableViewPieceClass mTableViewPieceClass;

typedef struct _mIndexPath{
    int section;
    int row;
}mIndexPath;


typedef enum _TableViewAnimType {
    NCS_TABLEVIEW_ANIMATIONNONE = 0,
    NCS_TABLEVIEW_ANIMATIONFADE,
    NCS_TABLEVIEW_ANIMATIONZOOM,
    NCS_TABLEVIEW_ANIMATIONLEFT,
}TableViewAnimType;

enum mTableViewSeparatorStyle{
    NCS_TABLEVIEW_SEPARATORSTYLE_NONE = 0,
    NCS_TABLEVIEW_SEPARATORSTYLE_SINGLINE,
    NCS_TABLEVIEW_SEPARATORSTYLE_MAX,
};

enum mTableViewPieceState{
    NCS_TABLEVIEW_NORMAL = 0,
    NCS_TABLEVIEW_EDIT,
};

enum mTableViewPieceEvent {
    NCSN_TABLEVIEW_CONTENTCLICKED= USER_PIECE_EVENT_BEGIN +  0x400,
};

enum {
    NCS_TABLEVIEW_GROUP_STYLE = 0, 
    NCS_TABLEVIEW_INDEX_STYLE,
    NCS_TABLEVIEW_INDEXLOCATE_STYLE, /* 带索引定位条风格*/
};

enum mTableViewItemPieceType {
    NCS_TABLEVIEW_NORMALROWTYPE = 0,
    NCS_TABLEVIEW_SEPARATORTYPE,
    NCS_TABLEVIEW_SECTIONTYPE,
    NCS_TABLEVIEW_TITLETYPE,
};

#define MSG_TABLEVIEWPIECE_STATE_CHANGED   (MSG_USER + 100)
#define MSG_TABLEVIEWPIECE_DELITEM         (MSG_USER + 101)

#define NCS_TABLEVIEW_GROUPGAP      30
#define NCS_TABLEVIEW_INDEXLOCATE_W 30
#define NCS_TABLEVIEW_INDEXLOCATE_Y 0

#define NCS_TABLEVIEW_SEPARATOR_DEFAULTCOLOR 0xffced3d6

#define mTableViewPieceHeader(clss) \
	mScrollViewPieceHeader(clss) \
    mTableViewItemPiece* focusPiece; \
    int mode;\
    int style; \
    POINT indexPos;\
    mIndexLocatePiece* indexLocate; \
    mPanelPiece* tablePanel; \
    DWORD separatorColor;\
    int rowHeight;\
    int separatorStyle; 

struct _mTableViewPiece
{
	mTableViewPieceHeader(mTableViewPiece)
};

#define mTableViewPieceClassHeader(clss, superCls) \
	mScrollViewPieceClassHeader(clss, superCls) \
    void (*changeMode)(clss*); \
    void (*getDefaultRowRect)(clss* self, RECT* rect); \
    mPanelPiece* (*createSeparatorLine)(clss* self); \
    mPanelPiece* (*createDefaultRow)(clss* self, mTableViewItemPiece* item_piece);\
    void (*reloadData)(clss* self);\
    mPanelPiece* (*createIndexSectionHead)(clss* self, int section); \
    mPanelPiece* (*createIndexSectionContent)(clss* self, int section);\
    mPanelPiece* (*createGroupSectionContent)(clss* self, int section);\
    mTableViewItemPiece* (*indexPathToItem)(clss* self, const mIndexPath* indexpath);\
    void (*itemToIndexPath)(clss* self, const mTableViewItemPiece* item, mIndexPath* indexpath);\
    void (*insertRowAtIndexPath)(clss* self, const mIndexPath* indexpath, TableViewAnimType animtype);\
    void (*deleteRowAtIndexPath)(clss* self, const mIndexPath* indexpath, TableViewAnimType animtype);\
    void (*rectForHeaderInSection)(clss* self, int section, RECT* rect); \
    void (*rectForRowAtIndexPath)(clss* self, const mIndexPath* indexpath, RECT* rect);\
    void (*rectForSection)(clss* self, int section, RECT* rect);\
    void (*setSeparatorStyle)(clss* self, enum mTableViewSeparatorStyle);\
    void (*setSeparatorColor)(clss* self, DWORD color);\
    /* public interface. */ \
    BOOL (*willSelectRowAtIndexPath)(clss* self, const mIndexPath* indexpath);\
    void (*onCommitInsertRowAtIndexPath)(clss* self, const mIndexPath* indexpath); \
    void (*onCommitDeleteRowAtIndexPath)(clss* self, const mIndexPath* indexpath); \
    mTableViewItemPiece* (*createItemForRow)(clss* self, const mIndexPath* indexpath);\
    int (*numberOfSections)(clss* self); \
    int (*numberOfRowsInSection)(clss* self, int section);\
    const char* (*titleForSection)(clss* self, int section); \
    const char* (*indexForSection)(clss* self, int section);\
    void (*rowDidSelectAtIndexPath)(clss* self, const mIndexPath* indexpath);
    

struct _mTableViewPieceClass
{
	mTableViewPieceClassHeader(mTableViewPiece, mScrollViewPiece)
};

MGNCS_EXPORT extern mTableViewPieceClass g_stmTableViewPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_TABLEVIEWPIECE_H */
