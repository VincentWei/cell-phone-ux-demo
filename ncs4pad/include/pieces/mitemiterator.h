/*!============================================================================
 * @file mitemiterator.h 
 * @Synopsis mItemIterator 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  MITEMITERATOR_INC
#define  MITEMITERATOR_INC

#include "mpieceitem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* prefix declare. */
struct _mAbstractItemManager;
typedef struct _mAbstractItemManager mAbstractItemManager;

struct _mAbstractItemManagerClass;
typedef struct _mAbstractItemManagerClass mAbstractItemManagerClass;

typedef struct _mItemIteratorClass mItemIteratorClass;
typedef struct _mItemIterator mItemIterator;

#define mItemIteratorHeader(clss) \
    mObjectHeader(clss) \
    mAbstractItemManager* manager;

#define mItemIteratorClassHeader(clss, superCls) \
    mObjectClassHeader(clss, superCls)  \
    mPieceItem* (*next)(clss*); \
    mPieceItem* (*prev)(clss*); \
    mItemIterator* (*duplicate)(clss*);

struct _mItemIterator
{   
    mItemIteratorHeader(mItemIterator)
};

struct _mItemIteratorClass
{   
    mItemIteratorClassHeader(mItemIterator, mObject)
};

MGNCS_EXPORT extern mItemIteratorClass g_stmItemIteratorCls;

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef MITEMITERATOR_INC  ----- */

