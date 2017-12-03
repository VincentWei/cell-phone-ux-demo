#ifndef  MGROUPITEMMANAGER_INC
#define  MGROUPITEMMANAGER_INC

#ifdef __cplusplus
extern "C" {
#endif

#include "mabstractitemmanager.h"
#include "mgrouppieceitem.h"
#include "mitemiterator.h"


typedef int (*ITEM_CMP)(const char* s1, const char* s2);
typedef struct _mGroupItemManagerClass mGroupItemManagerClass;
typedef struct _mGroupItemManager mGroupItemManager;

#define mGroupItemManagerHeader(clss) \
    mAbstractItemManagerHeader(clss) \
    list_t itemList; \
    ITEM_CMP itemCmp;

#define mGroupItemManagerClassHeader(clss, superCls) \
    mAbstractItemManagerClassHeader(clss, superCls) \
    BOOL (*addGroupItem)(clss*,  mGroupPieceItem*, BOOL); \
    BOOL (*removeGroupItem)(clss*, const char*, BOOL); \
    mGroupPieceItem* (*searchGroupItem)(clss*, const char*, BOOL); \
    mGroupPieceItem* (*searchGreaterGroupItem)(clss*, const char*, BOOL); \
    mItemIterator* (*createGroupItemIterator)(clss*, const char*);

struct _mGroupItemManager
{   
    mGroupItemManagerHeader(mGroupItemManager)
};

struct _mGroupItemManagerClass
{   
    mGroupItemManagerClassHeader(mGroupItemManager, mAbstractItemManager)
};

MGNCS_EXPORT extern mGroupItemManagerClass g_stmGroupItemManagerCls;

#ifdef __cplusplus
}
#endif


#endif   /* ----- #ifndef MGROUPITEMMANAGER_INC  ----- */

