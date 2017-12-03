#ifndef  MGROUPITEMITERATOR_INC
#define  MGROUPITEMITERATOR_INC

#include "mitemiterator.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "mlistitemiterator.h"

typedef struct _mGroupItemIteratorClass mGroupItemIteratorClass;
typedef struct _mGroupItemIterator mGroupItemIterator;

#define mGroupItemIteratorHeader(clss) \
    mItemIteratorHeader(clss) \
    list_t* head; \
    list_t* current;\
    list_t* parent;\
    mListItemIterator* parentIter; \
    mListItemIterator* childIter; \
    char* key;

#define mGroupItemIteratorClassHeader(clss, superCls) \
    mItemIteratorClassHeader(clss, superCls)

struct _mGroupItemIterator
{   
    mGroupItemIteratorHeader(mGroupItemIterator)
};

struct _mGroupItemIteratorClass
{   
    mGroupItemIteratorClassHeader(mGroupItemIterator, mItemIterator)
};

MGNCS_EXPORT extern mGroupItemIteratorClass g_stmGroupItemIteratorCls;

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef MGROUPITEMITERATOR_INC  ----- */

