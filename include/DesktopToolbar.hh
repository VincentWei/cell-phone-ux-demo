/*!============================================================================
 * @file DesktopToolbar.hh 
 * @Synopsis  
 * @author DengMin
 * @version 1.0
 * @date 10/11/2010 13:57:45 
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef DESKTOPTOOLBAR_HH
#define DESKTOPTOOLBAR_HH

#include <cstring>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgplus/mgplus.h>
#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mcontainerctrl.h"
#include "mpanelpiece.h"
#include "mtransroundpiece.h"
#include "mtextpiece.h"

#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "mbuttonpanelpiece.h"

#define DESKTOP_TOOLBAR_HEIGHT 49
#define DESKTOP_TOOLBAR_WIDTH  ACTIVITY_W

class DesktopToolbar {
    public:
        DesktopToolbar(HWND parent, bool show=TRUE);
        virtual ~DesktopToolbar();

        // add event listener
        void addBtnEventListener(int btnIdx, int message, NCS_CB_ONPIECEEVENT handler);
        void addBtnStateChangedListener(int btnIdx, NCS_CB_ONPIECEEVENT handler);

        // get move animation
        MGEFF_ANIMATION getMoveAnimation(int destx, int desty, int duration, 
                enum EffMotionType curve);

        mContainerCtrl* ctrl() { return m_toolbarCtrl; }
        mPanelPiece* shell() { return m_toolbarContainerShell; }
        HWND hwnd() { return m_toolbarCtrl->hwnd; }

        enum TB_INDEX {
            TB_MSG = 0,
            TB_CALL,
            TB_MENU,

            TB_MAX
        };

    private:
        void loadRes();
        void unloadRes();

        void initComponent(bool show);

    private:
        // parent window
        HWND m_parent;

        // component
        mContainerCtrl* m_toolbarCtrl;
        mPanelPiece* m_toolbarContainerShell;
        mPanelPiece* m_toolbarContainer;
        mButtonPanelPiece* m_toolbarBtn[TB_MAX];
        mHotPiece* m_toolbarNumPiece[TB_MAX];

        // resource
        // bitmap
        PBITMAP m_toolbarBmp[TB_MAX];
        PBITMAP m_toolbarRingBmp;
        // font
        PLOGFONT m_toolbarNumFont;
};

#endif // DESKTOPTOOLBAR_HH
