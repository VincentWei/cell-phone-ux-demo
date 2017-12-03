#ifndef  MGROUPPIECEITEM_INC
#define  MGROUPPIECEITEM_INC

#ifdef __cplusplus
extern "C" {
#endif

//#include "mpieceitem.h"

typedef struct _mGroupPieceItemClass mGroupPieceItemClass;
typedef struct _mGroupPieceItem mGroupPieceItem;

#define mGroupPieceItemHeader(clss) \
    mPieceItemHeader(clss) \
    list_t child; \
    char* key;

#define mGroupPieceItemClassHeader(clss, superCls) \
    mPieceItemClassHeader(clss, superCls)  \
    void (*setKey)(clss*, const char*); \
    char* (*getKey)(clss*);

struct _mGroupPieceItem
{   
    mGroupPieceItemHeader(mGroupPieceItem)
};

struct _mGroupPieceItemClass
{   
    mGroupPieceItemClassHeader(mGroupPieceItem, mPieceItem)
};

MGNCS_EXPORT extern mGroupPieceItemClass g_stmGroupPieceItemCls;

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef MGROUPPIECEITEM_INC  ----- */

