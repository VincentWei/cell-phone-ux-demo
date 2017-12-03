

#ifndef _MACTIVITYTOOLBARPIECE_H
#define _MACTIVITYTOOLBARPIECE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mpanelpiece.h"
#include "mtextpiece.h"

enum _ButtonPosition{
    ACTIVITYTOOLBAR_LEFT,
    ACTIVITYTOOLBAR_CENTER,
    ACTIVITYTOOLBAR_RIGHT,
    ACTIVITYTOOLBAR_MAX,
};

enum _ButtonFlag {
    TOOLBAR_BUTTON_LEFT = 1,
    TOOLBAR_BUTTON_CENTER = 2,
    TOOLBAR_BUTTON_RIGHT = 4,

    TOOLBAR_BUTTON_LEFT_HI = 8,
    TOOLBAR_BUTTON_CENTER_HI = 16,
    TOOLBAR_BUTTON_RIGHT_HI = 32,

    TOOLBAR_SELECT_COLOR = 64,
};

enum _DisplayButtonsTrategy {
    TOOLBAR_DISPLAY_ALL,
    TOOLBAR_DISPLAY_NAME,
    TOOLBAR_DISPLAY_PICTURE,
};

#define NCSN_ACTIVITY_TOOLBAR_BUTTON_STRATEGY (USER_PIECE_PROP_BEGIN+1)

typedef struct _mActivityToolbarPieceClass mActivityToolbarPieceClass;
typedef struct _mActivityToolbarPiece mActivityToolbarPiece;

#define mActivityToolbarPieceHeader(clss) \
    mPanelPieceHeader(clss) \
    mShapeTransRoundPiece *background; \
    mButtonPanelPiece *buttons[ACTIVITYTOOLBAR_MAX]; \
    mImagePiece *button_image [ACTIVITYTOOLBAR_MAX]; \
    mTextPiece *button_name [ACTIVITYTOOLBAR_MAX]; \
    PLOGFONT font;

#define mActivityToolbarPieceClassHeader(clss, superCls) \
    mPanelPieceClassHeader(clss, superCls) \
    void (*setButtonCallback)(mActivityToolbarPiece *self, int which_btn, NCS_CB_ONPIECEEVENT cb); \
    void (*setButtonImage)(mActivityToolbarPiece *self, int which_btn, BITMAP *bmp); \
    void (*changeButtonImage)(mActivityToolbarPiece *self, int which_btn, BITMAP *bmp); \
    void (*setButtonName)(mActivityToolbarPiece *self, int which_btn, const char *name);

struct _mActivityToolbarPiece
{   
    mActivityToolbarPieceHeader(mActivityToolbarPiece)
};

struct _mActivityToolbarPieceClass
{
    mActivityToolbarPieceClassHeader(mActivityToolbarPiece, mPanelPiece)
};

MGNCS_EXPORT extern mActivityToolbarPieceClass g_stmActivityToolbarPieceCls;


#ifdef __cplusplus
}
#endif
#endif /* _MACTIVITYTOOLBARPIECE_H */
