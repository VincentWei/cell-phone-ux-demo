/*!============================================================================
 * @file ScreenLockActivity.hh 
 * @Synopsis  
 * @author DengMin
 * @version 1.0
 * @date 11/10/2010 09:54:30 
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef SCREENLOCKACTIVITY_HH
#define SCREENLOCKACTIVITY_HH

#include "NCSActivity.hh"

#include "mgncs/mgncs.h"
#include "mgeff/mgeff.h"

#include "mcontainerctrl.h"
#include "mpanelpiece.h"

#include "mshutterpiece.h"
#include "mhighlightpiece.h"
#include "mtextpiece.h"

#include "DesktopToolbar.hh"

#include "dmStateMachine.hh"


// bubble number
#define BUBBLE_NUM      2


class ScreenLockActivity : public NCSActivity {

public:
    // constructor & destructor
    ScreenLockActivity();
    ~ScreenLockActivity();

    //private:
    // private method----------------------------------------------------

    // events
    // EVT_BUBBLE1_DOWN
    static int evt_bubble1_down(mHotPiece* self, int message, 
                                WPARAM wparam, LPARAM lparam, mObject* owner);
    // EVT_BUBBLE2_DOWN
    static int evt_bubble2_down(mHotPiece* self, int message, 
                                WPARAM wparam, LPARAM lparam, mObject* owner);
    // EVT_BUBBLE1_SWITCH_OK
    static void evt_bubble1_switch_ok(MGEFF_ANIMATION anim);
    // EVT_BUBBLE2_SWITCH_OK
    static void evt_bubble2_switch_ok(MGEFF_ANIMATION anim);
    // EVT_BUBBLE_IDLE
    static BOOL evt_bubble_idle(HWND hwnd, LINT id, DWORD count);
    // EVT_PHONE_IDLE
    static BOOL evt_phone_idle(HWND hwnd, LINT id, DWORD count);
    // EVT_ESC_PRESSED
    static int evt_esc_pressed(mWidget* self, int scancode, int status);
    // EVT_PHONE_SWITCH_OK
    static void evt_phone_switch_ok(MGEFF_ANIMATION anim);
    // EVT_MENU_PRESSED
    static void evt_menu_pressed(mHotPiece* self, mObject* owner);

    // actions
    void press_bubble1 ();
    void press_bubble2 ();
    static void bubble_unlock_anim(ScreenLockActivity* _this, int whichBubble);
    static void bubble_restore_anim(ScreenLockActivity* _this, int whichBubble);
    static void bubble_err_unlock_anim(ScreenLockActivity* _this, int whichBubble);
    static void unlock_phone(ScreenLockActivity* _this);
    static void lock_phone(ScreenLockActivity* _this);
    static void shutter_up1(ScreenLockActivity* _this);
    static void shutter_up2(ScreenLockActivity* _this);
    static void shutter_down1(ScreenLockActivity* _this);
    static void shutter_down2(ScreenLockActivity* _this);
    static void enter_desktop(ScreenLockActivity* _this);
    static void phone_idle(ScreenLockActivity* _this);
    static void bubble_idle(ScreenLockActivity* _this);

    virtual int onResume();
    virtual int onPause();

    // initialize helper functions
    void initStateMachine();
    void loadMyResource();
    void unloadMyResource();
    void initInfoBox(mWidget* self);
    void initBubbles(mWidget* self);
    void initHintBar(mWidget* self);
    void initToolBar(mWidget* self);

    void stopHighlight();

    //private:
    // private data----------------------------------------------------

    // resource
    // bitmap
    PBITMAP m_bgBmp;
    PBITMAP m_bubbleBmp[BUBBLE_NUM];
    // font
    PLOGFONT m_timeFont;
    PLOGFONT m_dateFont;
    PLOGFONT m_infoFont;
    PLOGFONT m_adFont;
    PLOGFONT m_hintbarFont;


    // time, date & user information
    mContainerCtrl* m_infoCtrl;
    mPanelPiece* m_infoContainer;
    mHotPiece* m_infoBoxBgPiece;
    mHotPiece* m_timePiece;
    mHotPiece* m_datePiece;
//    mHotPiece* m_separatorPiece;
    mShutterPiece* m_shutterPiece;

    // bubbles
    mContainerCtrl* m_bubbleCtrl[BUBBLE_NUM];
    mPanelPiece* m_bubbleContainer[BUBBLE_NUM];
    mHotPiece* m_bubblePiece[BUBBLE_NUM];

    // hint bar
    mContainerCtrl* m_hintbarCtrl;
    mPanelPiece* m_hintbarContainerShell;
    mPanelPiece* m_hintbarContainer;
    mHighlightPiece* m_highlightPiece;
    MGEFF_ANIMATION m_highlightAnim;

    // tool bar
    DesktopToolbar* m_toolbar;

    mWidget* self;

    // state machine
    dmStateMachine m_stateMachine;

    int m_intentFlag;
};


#endif
