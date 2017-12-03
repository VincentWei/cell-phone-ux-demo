#ifndef _MGNCS_LAYOUTMANAGER_H
#define _MGNCS_LAYOUTMANAGER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mitemiterator.h"

typedef struct _mLayoutManager mLayoutManager;
typedef struct _mLayoutManagerClass mLayoutManagerClass;

#define mLayoutManagerHeader(clss) \
    mObjectHeader(clss)

struct _mLayoutManager
{
    mLayoutManagerHeader(mLayoutManager)
};

#define mLayoutManagerClassHeader(clss, superCls) \
    mObjectClassHeader(clss, superCls) \
    void (*reLayout)(clss*, mItemIterator*, int, int);

struct _mLayoutManagerClass
{
    mLayoutManagerClassHeader(mLayoutManager, mObject)
};

MGNCS_EXPORT extern mLayoutManagerClass g_stmLayoutManagerCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_LAYOUTMANAGER_H */
