/**
 * \file miconflow.h
 * \author ZhaolinHu
 * \date 2010/04/16
 *
 * This file includes the definition of IconView.
 *
 \verbatim

    Copyright (C) 2010 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgeff, which is control 
    set of MiniGUI.

  	\endverbatim
 */

/**
 * $Id: miconflow.h 236 2010-04-16 01:57:37Z ZhaolinHu $
 *
 *      Copyright (C) 2010 Feynman Software.
 */

#ifndef _MGUI_CTRL_ICONFLOW_H
#define _MGUI_CTRL_ICONFLOW_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Widget_IconView mIconFlow
 * @{
 */

/** 
 * Structure of the iconflow item info, contains information about an item.
 * This structure is used for creating or retrieving an item. 
 */
typedef struct _NCS_ICONFLOW_ITEMINFO
{
    /**
     * The index of the item
     */
    int index;

    /**
     * The bitmap icon of the item
     */
    PBITMAP bmp;

    /**
     * The text label of the item
     */
    const char *label;

    /** Attached additional data of this item */
    DWORD addData;
}NCS_ICONFLOW_ITEMINFO;

typedef struct _mIconFlow mIconFlow;
typedef struct _mIconFlowClass mIconFlowClass;
typedef struct _mIconFlowRenderer mIconFlowRenderer;

/**
 * \def NCSCTRL_ICONFLOWIEW
 * \brief The control class name of mIconFlow.
 */
#define NCSCTRL_ICONFLOW NCSCLASSNAME("iconflow")

/**
 * \def NCSS_ICONFLOW_SHIFT
 * \brief The bits used by mIconFlow in style.
 */
#define NCSS_ICONFLOW_SHIFT     (NCSS_ITEMV_SHIFT+1)

/**
 * \def NCSS_ICONFLOW_SORT
 * \sa NCSS_ITEMV_AUTOSORT
 */
#define NCSS_ICONFLOW_SORT      (NCSS_ITEMV_AUTOSORT)

#define NCSS_ICONFLOW_VERTICAL  (0x0001<<NCSS_ITEMV_SHIFT)

enum ICONFLOW_DIRECTION
{
    ICONFLOW_DIR_NONE,
    ICONFLOW_DIR_PREV,
    ICONFLOW_DIR_NEXT
};

typedef struct _ICONFLOW_POINT3D {
    int x;
    int y;
    float z;
} ICONFLOW_POINT3D;

/**
 * \def NCSS_ICONFLOW_LOOP
 * \sa NCSS_ITEMV_LOOP
 */
#define NCSS_ICONFLOW_LOOP      NCSS_ITEMV_LOOP

#define mIconFlowHeader(className)  \
    mItemViewHeader(className)     \
    mHotPiece *bkgndPiece;          \
    HDC iconFrameDC;                \
    float key;                      \
    int visItemCount;               \
    int prevIndex;                  \
    ICONFLOW_POINT3D point3d[20];   \
    int defItemWidth;               \
    unsigned span;                  \
    POINT mouse;                    \
    BOOL isVertical;                \
    unsigned iconBorder;            \
    BOOL iconUseZoom;               \
    enum ICONFLOW_DIRECTION direction;

/**
 * \struct mIconFlow
 * \brief The structure of mIconFlow control, which derived from
 *        mItemView. It is the abstract superclass of all classes
 *        which represent controls that have items.
 *
 *  - defItemWidth\n
 *    The default icon width.
 *
 *  - nrCol\n
 *    The number of column.
 *
 *  - flags\n
 *    The iconflow flags.
 */
struct _mIconFlow
{
	mIconFlowHeader(mIconFlow)
};

#define mIconFlowClassHeader(clsName, parentClass)                      \
    mItemViewClassHeader(clsName, parentClass)                         \
    void (*beginAnimation)(clsName*, float startvalue, float endvalue);      \
    void (*endAnimation)(clsName*);                                          \
    void (*drawItem)(clsName*, HITEM hItem, HDC hdc, RECT *rcDraw);     \
    void (*runAnimation)(clsName*, float startvalue, float endvalue);    \
    void (*setVisItemCenter)(clsName*, int count ,int width, int height);   \
    void (*setIconSize)(clsName*, int width, int height);               \
    void (*setVisItemCount)(clsName*, int count);                       \
    void (*setSpan)(clsName*, int span);                                \
    HITEM (*addItem)(clsName*, NCS_ICONFLOW_ITEMINFO *info, int *pos);

struct _mIconFlowClass
{
	mIconFlowClassHeader(mIconFlow, mItemView)
};

/**
 * \struct mIconFlowClass
 * \brief The virtual function table of mIconFlow, which derived from
 *        mItemViewClass.
 *
 * - void (*\b setIconSize)(mIconFlow *self, int width, int height);\n
 *   Sets the icon size.
 *
 * - HITEM (*\b addItem)(mIconFlow *self, \ref NCS_ICONFLOW_ITEMINFO *info, int *pos);\n
 *   Add a new item according to \a info to iconflow.
 *      \param info     The item information.
 *      \param pos      The position in which item has been inserted. It can be NULL.
 *      \return         The handle to new item.
 */
#define mIconFlowRendererHeader(clsName, parentClass) \
	mItemViewRendererHeader(clsName, parentClass)

/**
 * \struct mIconFlowRenderer
 * \brief The structure of mIconFlow effector, which derived
 *        from mItemViewRenderer.
 */
struct  _mIconFlowRenderer {
	mIconFlowRendererHeader(mIconFlow, mItemView)
};

/**
 * \enum mIconFlowProp
 * \brief The properties id of mIconFlow.
 */
enum mIconFlowProp
{
    /**
     * The default icon height.
     */
	NCSP_ICONFLOW_DEFICONHEIGHT = NCSP_ITEMV_DEFITEMHEIGHT,

    /**
     * The default icon width.
     */
	NCSP_ICONFLOW_DEFICONWIDTH = NCSP_ITEMV_MAX + 1,
    NCSP_ICONFLOW_BKGNDPIECE,
    NCSP_ICONFLOW_VISITEMCOUNT,
    NCSP_ICONFLOW_ICONFRAME,
    NCSP_ICONFLOW_SPAN,
    NCSP_ICONFLOW_ICONBORDER,
    NCSP_ICONFLOW_ICONUSEZOOM,
    /**
     * The maximum value of mIconFlow properties id.
     */
    NCSP_ICONFLOW_MAX,
};

/**
 * \enum mIconFlowNotify
 * \brief The notification code id of mIconFlow.
 */
enum mIconFlowNotify
{
    NCSN_ICONFLOW_ENTER = NCSN_ITEMV_ENTER,
    NCSN_ICONFLOW_CLICKED = NCSN_ITEMV_CLICKED,
    NCSN_ICONFLOW_SELCHANGED = NCSN_ITEMV_SELCHANGED,
    /**
     * The maximum value of mIconFlow notification code id.
     */
    NCSN_ICONFLOW_MAX = NCSN_ITEMV_MAX + 1,
};

/**
 * \var g_stmIconFlowCls
 * \brief The instance of mIconFlowClass.
 *
 * \sa mIconFlowClass
 */
MGNCS_EXPORT extern mIconFlowClass g_stmIconFlowCls;
    /** @} end of Widget_IconView */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_ICONFLOW_H */
