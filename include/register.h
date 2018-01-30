#include "mnavigationitem.h"
#include "mnavigationbarpiece.h"
#include "mnavigationpanelpiece.h"

#define REGISTER_NCS() \
    MGNCS_INIT_CLASS(mNavigationItem); \
    MGNCS_INIT_CLASS(mNavigationBarPiece); \
    MGNCS_INIT_CLASS(mNavigationPanelPiece); \
    MGNCS_INIT_CLASS(mWidgetHostPiece);

#define UNREGISTER_NCS()
