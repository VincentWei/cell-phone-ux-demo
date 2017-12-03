/*!============================================================================
 * @file mlistitemmanager.h 
 * @Synopsis mListItemManager 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  MLISTITEMMANAGER_INC
#define  MLISTITEMMANAGER_INC

#ifdef __cplusplus
extern "C" {
#endif

#include "mabstractitemmanager.h"

typedef struct _mListItemManagerClass mListItemManagerClass;
typedef struct _mListItemManager mListItemManager;

#define mListItemManagerHeader(clss) \
    mAbstractItemManagerHeader(clss)    \
    list_t queue;

#define mListItemManagerClassHeader(clss, superCls) \
    mAbstractItemManagerClassHeader(clss, superCls)

struct _mListItemManager
{   
    mListItemManagerHeader(mListItemManager)
};

struct _mListItemManagerClass
{   
    mListItemManagerClassHeader(mListItemManager, mAbstractItemManager)
};

MGNCS_EXPORT extern mListItemManagerClass g_stmListItemManagerCls;

#ifdef __cplusplus
}
#endif


#endif   /* ----- #ifndef MLISTITEMMANAGER_INC  ----- */

