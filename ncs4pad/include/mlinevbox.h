#ifndef _MGNCS_MLINEVBOX_H
#define _MGNCS_MLINEVBOX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _mLineVBox mLineVBox;
typedef struct _mLineVBoxClass mLineVBoxClass;

#define mLineVBoxHeader(clss)  \
    mLayoutManagerHeader(clss) \
    int gap;

struct _mLineVBox
{
    mLineVBoxHeader(mLineVBox)
};

#define mLineVBoxClassHeader(clss, superCls) \
    mLayoutManagerClassHeader(clss, superCls)\
    void (*setGap)(clss*, int); \
    void (*getLayoutRect)(clss*, mItemIterator* , RECT* );

struct _mLineVBoxClass
{
    mLineVBoxClassHeader(mLineVBox, mLayoutManager)
};

MGNCS_EXPORT extern mLineVBoxClass g_stmLineVBoxCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_MLINEVBOX_H */
