#ifndef _MGNCS_MNAVIGATION_PANEL_PIECE_H
#define _MGNCS_MNAVIGATION_PANEL_PIECE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mnavigationitem.h"
#include "mnavigationbarpiece.h"
#include "m3dbuttonpiece.h"


typedef struct _mNavigationPanelPieceClass mNavigationPanelPieceClass;
typedef struct _mNavigationPanelPiece mNavigationPanelPiece;


/* NavigationItem list */
typedef struct _item_list_t {
    list_t list;
    mNavigationItem* item;
} item_list_t;


/* NavigationPanelPiece set and get properties */
#define NAVIGATIONPANELPIECE_PROP_BEGIN    USER_PIECE_PROP_BEGIN + 100
enum {
    NCSP_NAVIGATIONPANELPIECE_BAR = NAVIGATIONBARPIECE_PROP_BEGIN,
    NCSP_NAVIGATIONPANELPIECE_BAR_BKG,
};


/* NaviagtionPanelPiece methods */
#define mNavigationPanelPieceClassHeader(clss, superCls) \
    mPanelPieceClassHeader(clss, superCls) \
    void (*push)(clss*, mNavigationItem*); \
    void (*pop)(clss*); \
    void (*showNavigationBar)(clss*, BOOL, BOOL); \
    BOOL (*currentIsRoot)(clss*);

/* static interface*/
MGNCS_EXPORT extern mNavigationPanelPiece* CreateNavigationPanelPieceWithRootView (mNavigationItem *rootItem);

struct _mNavigationPanelPieceClass
{   
    mNavigationPanelPieceClassHeader(mNavigationPanelPiece, mPanelPiece)
};

MGNCS_EXPORT extern mNavigationPanelPieceClass g_stmNavigationPanelPieceCls;


/* NaviagtioPanelPiece data members */
#define mNavigationPanelPieceHeader(clss) \
    mPanelPieceHeader(clss) \
    PLOGFONT default_title_font; \
    list_t item_head; \
    mNavigationBarPiece* bar;

struct _mNavigationPanelPiece
{   
    mNavigationPanelPieceHeader(mNavigationPanelPiece)
};


#ifdef __cplusplus
}
#endif

#endif /*_MGNCS_MNAVIGATION_PANEL_PIECE_H*/

