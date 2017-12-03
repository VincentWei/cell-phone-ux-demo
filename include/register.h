#include "mpieceitem.h"
#include "mitemiterator.h"
#include "mlistitemiterator.h"
#include "mabstractitemmanager.h"
#include "mlistitemmanager.h"
#include "mgroupitemmanager.h"
#include "mtransroundpiece.h"
#include "mrotateswitchpiece.h"
#include "mpanelpiece.h"
#include "mwrappaintpiece.h"
#include "mitempiece.h"
#include "mslideswitchpiece.h"
#include "mcheckmarkpiece.h"
#include "mradiopiece.h"
#include "mindicatorpiece.h"
#include "mindicatorbuttonpiece.h"
#include "mtableviewitempiece.h"
#include "mindexlocatepiece.h"
#include "mtableviewpiece.h"
#include "mbuttonpanelpiece.h"
#include "manimationeditpiece.h"
#include "mcontainerctrl.h"
#include "mlinevbox.h"
#include "mcenterhbox.h"
#include "mtextpiece.h"
#include "mexseparatorpiece.h"
#include "miconflow.h"
#include "mnavigationitem.h"
#include "mnavigationbarpiece.h"
#include "mnavigationpanelpiece.h"
#include "m3dbuttonpiece.h"
#include "mwidgethostpiece.h"

#ifdef ENABLE_SCROLLVIEW
#   include "mscrollviewpiece.h"
#   define SCROLLVIEW MGNCS_INIT_CLASS(mScrollViewPiece);
#else
#   define SCROLLVIEW /* NULL */
#endif

#define REGISTER_NCS() \
    MGNCS_INIT_CLASS(mPieceItem); \
    MGNCS_INIT_CLASS(mAbstractItemManager); \
    MGNCS_INIT_CLASS(mItemIterator); \
    MGNCS_INIT_CLASS(mListItemManager); \
    MGNCS_INIT_CLASS(mListItemIterator); \
    MGNCS_INIT_CLASS(mRotateSwitchPiece); \
    MGNCS_INIT_CLASS(mPanelPiece); \
    MGNCS_INIT_CLASS(mItemPiece); \
    MGNCS_INIT_CLASS(mCheckMarkPiece); \
    MGNCS_INIT_CLASS(mRadioPiece); \
    MGNCS_INIT_CLASS(mIndicatorPiece); \
    MGNCS_INIT_CLASS(mIndicatorButtonPiece); \
    MGNCS_INIT_CLASS(mSlideSwitchPiece); \
    MGNCS_INIT_CLASS(mTextPiece); \
    MGNCS_INIT_CLASS(mWrapPaintPiece); \
    MGNCS_INIT_CLASS(mShapeTransRoundPiece); \
    MGNCS_INIT_CLASS(mAnimationEditPiece); \
    MGNCS_INIT_CLASS(mButtonPanelPiece);\
    MGNCS_INIT_CLASS(mLayoutManager);\
    MGNCS_INIT_CLASS(mCenterHBox); \
    MGNCS_INIT_CLASS(mExSeparatorPiece);\
    MGNCS_INIT_CLASS(mLineVBox);\
    MGNCS_REGISTER_COMPONENT(mContainerCtrl); \
    MGNCS_REGISTER_COMPONENT_EX(mIconFlow, WS_VSCROLL, \
        WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);\
    SCROLLVIEW \
    MGNCS_INIT_CLASS(mNavigationItem); \
    MGNCS_INIT_CLASS(mTableViewItemPiece); \
    MGNCS_INIT_CLASS(mTableViewPiece); \
    MGNCS_INIT_CLASS(mIndexLocatePiece); \
    MGNCS_INIT_CLASS(mNavigationBarPiece); \
    MGNCS_INIT_CLASS(mNavigationPanelPiece); \
    MGNCS_INIT_CLASS(m3DButtonPiece);   \
    MGNCS_INIT_CLASS(mWidgetHostPiece);

#define UNREGISTER_NCS() \
    MGNCS_UNREG_COMPONENT(mIconFlow); \
    MGNCS_UNREG_COMPONENT(mContainerCtrl);
