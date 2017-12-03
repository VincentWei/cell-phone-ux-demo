#ifndef _MGNCS_MNAVIGATION_ITEM_H
#define _MGNCS_MNAVIGATION_ITEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mpanelpiece.h"
#include "mtransroundpiece.h"
#include "mtextpiece.h"
#include "mbuttonpanelpiece.h"
#include "m3dbuttonpiece.h"


typedef struct _mNavigationItemClass mNavigationItemClass;
typedef struct _mNavigationItem mNavigationItem;


/* NavigationItem set and get properties */
#define NAVIGATIONITEM_PROP_BEGIN    USER_PIECE_PROP_BEGIN + 1
enum {
    NCSP_NAVIGATIONITEM_CONTENT = NAVIGATIONITEM_PROP_BEGIN,
    NCSP_NAVIGATIONITEM_BACKGROUND,
    NCSP_NAVIGATIONITEM_BAR_BKG,
    NCSP_NAVIGATIONITEM_TITLE,
    NCSP_NAVIGATIONITEM_STYLE,
    NCSP_NAVIGATIONITEM_LEFT_BUTTON,
    NCSP_NAVIGATIONITEM_TITLE_BUTTON,
    NCSP_NAVIGATIONITEM_RIGHT_BUTTON,
    NCSP_NAVIGATIONITEM_DEFAULT_BUTTON_FONT,
};

/* navigation bar style */
typedef enum {
    NAVIGATION_STYLE_NORMAL = 0x00,      /* normal: the bar not overlapped the view, not transparent */
    NAVIGATION_STYLE_FULLSCREEN = 0x01,  /* full screen: the bar overlapped the view, transparent */
    NAVIGATION_STYLE_HIDE_BAR = 0x02     /* full screen: the bar not show */
} navigation_bar_style;


/* NaviagtionItem methods */
#define mNavigationItemClassHeader(clss, superCls) \
    mObjectClassHeader(clss, superCls) \
    m3DButtonPiece* (*createDefaultButton)(clss*, const char*, DWORD); \
	BOOL (*setProperty)(clss*, int, DWORD); \
	DWORD(*getProperty)(clss*, int); 
    
/* static interface */
MGNCS_EXPORT extern mNavigationItem* CreateNavigationItem (mHotPiece *content, const char *title, unsigned int style);
MGNCS_EXPORT extern void DestroyNavigationItem (mNavigationItem* item);
MGNCS_EXPORT extern void AdjustNavigationItemRectWithTitle (mHotPiece *content, const char *title, PLOGFONT font);

struct _mNavigationItemClass
{   
    mNavigationItemClassHeader(mNavigationItem, mObject)
};

MGNCS_EXPORT extern mNavigationItemClass g_stmNavigationItemCls;


/* NaviagtioItem data members */
#define mNavigationItemHeader(clss) \
    mObjectHeader(clss) \
    mShapeTransRoundPiece* background; \
    mHotPiece* content; \
    mButtonPanelPiece* bar_left_button; \
    mTextPiece* bar_title_button; \
    mButtonPanelPiece* bar_right_button; \
    PLOGFONT default_button_font; \
    unsigned int style; \
    char* title;

struct _mNavigationItem
{   
    mNavigationItemHeader(mNavigationItem)
};


#ifdef __cplusplus
}
#endif

#endif /*_MGNCS_MNAVIGATION_ITEM_H*/

