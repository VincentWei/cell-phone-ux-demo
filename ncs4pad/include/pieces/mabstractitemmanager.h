/*!============================================================================
 * @file mabstractitemmanager.h 
 * @Synopsis mAbstractItemManager 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  MABSTRACTITEMMANAGER_INC
#define  MABSTRACTITEMMANAGER_INC

#include "mitemiterator.h"
#include "mpieceitem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* typedef struct _mAbstractItemManagerClass mAbstractItemManagerClass;
 * typedef struct _mAbstractItemManager mAbstractItemManager;
 */

#define mAbstractItemManagerHeader(clss) \
    mObjectHeader(clss)

#define mAbstractItemManagerClassHeader(clss, superCls) \
    mObjectClassHeader(clss, superCls)  \
    int  (*addItem)(clss*, mPieceItem*);  \
    void (*removeItem)(clss*, mPieceItem*);  \
    void (*clear)(clss*);   \
    mItemIterator* (*createItemIterator)(clss*);

struct _mAbstractItemManager
{   
    mAbstractItemManagerHeader(mAbstractItemManager)
};

struct _mAbstractItemManagerClass
{   
    mAbstractItemManagerClassHeader(mAbstractItemManager, mObject)
};

MGNCS_EXPORT extern mAbstractItemManagerClass g_stmAbstractItemManagerCls;

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef MABSTRACTITEMMANAGER_INC  ----- */
