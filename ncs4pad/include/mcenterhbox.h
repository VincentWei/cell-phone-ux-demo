#ifndef _MGNCS_MCENTERHBOX_H
#define _MGNCS_MCENTERHBOX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _mCenterHBox mCenterHBox;
typedef struct _mCenterHBoxClass mCenterHBoxClass;

#define mCenterHBoxHeader(clss) \
    mLayoutManagerHeader(clss)

struct _mCenterHBox
{
    mCenterHBoxHeader(mCenterHBox)
};

#define mCenterHBoxClassHeader(clss, superCls) \
    mLayoutManagerClassHeader(clss, superCls)

struct _mCenterHBoxClass
{
    mCenterHBoxClassHeader(mCenterHBox, mLayoutManager)
};

MGNCS_EXPORT extern mCenterHBoxClass g_stmCenterHBoxCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_MCENTERHBOX_H */
