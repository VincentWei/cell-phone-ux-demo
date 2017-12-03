/*!============================================================================
 * @file mlistitemiterator.h 
 * @Synopsis mListItemIterator 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  MLISTITEMITERATOR_INC
#define  MLISTITEMITERATOR_INC

#include "mitemiterator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mListItemIteratorClass mListItemIteratorClass;
typedef struct _mListItemIterator mListItemIterator;

#define mListItemIteratorHeader(clss) \
    mItemIteratorHeader(clss) \
    list_t* head; \
    list_t* current;

#define mListItemIteratorClassHeader(clss, superCls) \
    mItemIteratorClassHeader(clss, superCls)

struct _mListItemIterator
{   
    mListItemIteratorHeader(mListItemIterator)
};

struct _mListItemIteratorClass
{   
    mListItemIteratorClassHeader(mListItemIterator, mItemIterator)
};

MGNCS_EXPORT extern mListItemIteratorClass g_stmListItemIteratorCls;

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef MLISTITEMITERATOR_INC  ----- */

