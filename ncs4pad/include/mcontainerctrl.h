#ifndef _MGUI_NCSCTRL_CTNR_H
#define _MGUI_NCSCTRL_CTNR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NCSCTRL_CONTAINERCTRL NCSCLASSNAME("containerctrl")

typedef struct _mContainerCtrl mContainerCtrl;
typedef struct _mContainerCtrlClass mContainerCtrlClass;
typedef struct _mContainerCtrlRenderer mContainerCtrlRenderer;

#define mContainerCtrlHeader(clsName)  \
    mWidgetHeader(clsName)

struct _mContainerCtrl
{
    mContainerCtrlHeader(mContainerCtrl)
};

#define mContainerCtrlClassHeader(clsName, parentClass) \
    mWidgetClassHeader(clsName, parentClass)            \
    void (*setBody)(clsName *, mHotPiece*);

struct _mContainerCtrlClass
{
    mContainerCtrlClassHeader(mContainerCtrl, mWidget)
};

#define mContainerCtrlRendererHeader(clsName, parentClass) \
    mWidgetRendererHeader(clsName, parentClass)

struct _mContainerCtrlRenderer
{
    mContainerCtrlRendererHeader(mContainerCtrl, mWidget)
};

MGNCS_EXPORT extern mContainerCtrlClass g_stmContainerCtrlCls;

enum mContainerCtrlProp
{
    NCSP_CTNRCTRL_MAX = NCSP_WIDGET_MAX + 1
};

#define NCSS_CTNRCTRL_SHIFT NCSS_WIDGET_SHIFT

enum mContainerCtrlNotify
{
    NCSN_CTNRCTRL_MAX = NCSN_WIDGET_MAX + 1
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGUI_NCSCTRL_CTNR_H */
