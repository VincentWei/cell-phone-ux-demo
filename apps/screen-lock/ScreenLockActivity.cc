#include "ScreenLockActivity.hh"
#include <mgplus/mgplus.h>

#include "mtransroundpiece.h"

#include "global.h"
#include "TimeService.hh"
#include "PanelManager.hh"
//#include "DesktopCommonProc.hh"

REGISTER_ACTIVITY(ScreenLockActivity);

// position & size---------------------------------------------
#define INFOBOX_W   240
#define INFOBOX_H   130
#define INFOBOX_X   ((ACTIVITY_W - INFOBOX_W)>>1)
#define INFOBOX_Y   37
#define HILIGHT_TEXT_W (ACTIVITY_W>=360?28:24)

#define INFOBOX_H1   (INFOBOX_H*(100-40)/100)
#define INFOBOX_H2   (INFOBOX_H-INFOBOX_H1)
#define INFOBOX_Y1   (0)
#define INFOBOX_Y2   (INFOBOX_Y1+INFOBOX_H1)

#define TIME_X      0//(124-INFOBOX_X)
#define TIME_Y      (40-INFOBOX_Y)
#define TIME_W      INFOBOX_W//(239-INFOBOX_X-TIME_X)
#define TIME_H      (83-INFOBOX_Y-TIME_Y)

#define DATE_X      0//(95-INFOBOX_X)
#define DATE_Y      (92-INFOBOX_Y)
#define DATE_W      INFOBOX_W//(266-INFOBOX_X-DATE_X)
#define DATE_H      (109-INFOBOX_Y-DATE_Y)

#define SEPARATOR_X   0
#define SEPARATOR_Y   (DATE_Y+DATE_H)
#define SEPARATOR_W   INFOBOX_W
#define SEPARATOR_H   10

#define SHUTTER_X   0
#define SHUTTER_Y   (SEPARATOR_Y + SEPARATOR_H)
#define SHUTTER_W   INFOBOX_W
#define SHUTTER_H   (INFOBOX_H - SHUTTER_Y)

#define SHUTTER_H1   (SHUTTER_H - 4)
#define SHUTTER_H2   (SHUTTER_H/2 + 4)
#define SHUTTER_UP_END   SHUTTER_H2

#define BUBBLE_MAX_W   66
#define BUBBLE_MAX_H    66

#define BUBBLE_W   56
#define BUBBLE_H   56

// hintbar
#define HINTBAR_H  (480-431)
#define HINTBAR_W  ACTIVITY_W
#define HINTBAR_X  0
#define HINTBAR_Y  (ACTIVITY_H - HINTBAR_H)

#define HIGHLIGHT_START_X	-50
#define INFO_BOX_RADIUS 4
// gradient color
typedef struct _GradientData {
    float pos;
    ARGB color;
} GradientData;

static const GradientData hintbar_gradient[] = { 
	{ 0.00f, 0x7f0f1700, },
	{ 0.01f, 0x7f434343, },
	{ 0.04f, 0x7f2d2d2d, },
	{ 0.49f, 0x7f151515, },
	{ 0.51f, 0x7f010101, },
	{ 1.00f, 0x7f000000, },
};

#if 0
static const GradientData infobox1_gradient[] = { 
	{ 0.00f, 0xcce0e496, },
	{ 0.02f, 0xccd1d671, },
	{ 0.62f, 0xccbbc866, },
	{ 0.62f, 0xcca9b93f, },
	{ 0.97f, 0xcc94ac35, },
	{ 0.98f, 0xcc76892a, },
	{ 0.98f, 0xccafc169, },
	{ 1.00f, 0xcc92aa34, },
};
#else
static const GradientData infobox1_gradient[] = { 
	{ 0.00f, 0xcce0e496, },
	{ 0.02f, 0xccd1d671, },
	{ 0.60f, 0xccbbc866, },
	{ 0.61f, 0xcca9b93f, },
	{ 0.97f, 0xcc94ac35, },
	{ 1.00f, 0xcc404040, },
};

#endif

static const GradientData infobox2_gradient[] = { 
	{ 0.00f, 0xccafc169, },
	{ 0.02f, 0xcc92aa34, },
	{ 1.00f, 0xcc7b9b28, },
};


// control ID------------------------------------------
#define ID_HINTBAR  100

// resource--------------------------------------------
static const char* btn_img_path[] =
{
    "res/lockscreen/bubble_btn1.png",
    "res/lockscreen/bubble_btn2.png",
};

// timer id------------------------------------------------
#define START_HIGHLIGHT_TIMER   10
#define BUBBLE_IDLE_TIMER       11
#define INFOBOX_UPDATE_TIMER    12
#define PHONE_IDLE_TIMER_ID		13
// timer interval---------------------------------------------
#define START_HIGHLIGHT_DURATION   20
#define BUBBLE_IDLE_INTERVAL       (5000/10)
#define INFOBOX_UPDATE_INTERVAL    (100/10)
#define HIGHLIGHT_DURATION			2000

// animation duaration
#define BUBBLE_NORMAL2MAX_DURATION     200
#define BUBBLE_MIN2NORMAL_DURATION     200
#define BUBBLE_MAX2MIN_DURATION        400
#define BUBBLE_MAX2NORMAL_DURATION     200
#define DRAW_ADD_DURATION              400

#define HINTBAR_MOVE_DURATION       200
#define TOOLBAR_MOVE_DURATION       500

#define SHUTTER_UP1_DURATION       200
#define SHUTTER_DOWN1_DURATION     100
#define SHUTTER_UP2_DURATION       100
#define SHUTTER_DOWN2_DURATION     200

#define HINTBAR_TEXT    "Click two bubbles to unlock"
#define AD_TEXT         "Fill with magic"
#define USER_INFO       "2 missed calls\n3 unread messages"

// get my activity
#define thisActivity(hwnd)    ((ScreenLockActivity*)(Activity::getActivityFromHWND(hwnd)))

// time format
enum TimeFormat {
    TIME_FORMAT_12 = 0,
    TIME_FORMAT_24 = 1,
};

// state & transition-----------------------------------
enum ScreenLockStateId
{
    STA_PHONE_LOCKED      = 0,
    STA_UNLOCK_BUBBLE1            ,
    STA_BUBBLE1_UNLOCKED          ,
    STA_UNLOCK_BUBBLE2            ,
    STA_BUBBLE2_UNLOCKED          ,
    STA_PHONE_UNLOCKED       ,
    STA_ERR_UNLOCK_B2    ,
    STA_PAUSED       ,

    STA_NUM,
};

enum ScreenLockEventId
{
    EVT_BUBBLE1_DOWN	     = 0,
    EVT_BUBBLE2_DOWN          ,
    EVT_BUBBLE1_SWITCH_OK    ,
    EVT_BUBBLE2_SWITCH_OK    ,
    EVT_BUBBLE_IDLE         ,
    EVT_PHONE_IDLE	        ,
    EVT_ESC_PRESSED         ,
    EVT_PHONE_SWITCH_OK     ,
    EVT_PAUSE     ,
    EVT_RESUME     ,

    EVT_NUM,
};

BEGIN_STATE_DECLARE(stateTable)
    DECLARE_STATE(STA_PHONE_LOCKED)
    DECLARE_STATE(STA_UNLOCK_BUBBLE1)
    DECLARE_STATE(STA_BUBBLE1_UNLOCKED)
    DECLARE_STATE(STA_UNLOCK_BUBBLE2)
    DECLARE_STATE(STA_BUBBLE2_UNLOCKED)
    DECLARE_STATE(STA_PHONE_UNLOCKED)
    DECLARE_STATE(STA_ERR_UNLOCK_B2)
    DECLARE_STATE(STA_PAUSED)
END_STATE_DELCARE

BEGIN_TRANSITION_DECLARE(transitionTable)
    DECLARE_TRANSITION(STA_PHONE_LOCKED, EVT_BUBBLE1_DOWN, STA_UNLOCK_BUBBLE1)
    DECLARE_TRANSITION(STA_PHONE_LOCKED, EVT_BUBBLE2_DOWN, STA_ERR_UNLOCK_B2)

    DECLARE_TRANSITION(STA_UNLOCK_BUBBLE1, EVT_BUBBLE1_SWITCH_OK, STA_BUBBLE1_UNLOCKED)
    DECLARE_TRANSITION(STA_UNLOCK_BUBBLE1, EVT_BUBBLE2_DOWN, STA_UNLOCK_BUBBLE2)

    DECLARE_TRANSITION(STA_BUBBLE1_UNLOCKED, EVT_BUBBLE2_DOWN, STA_UNLOCK_BUBBLE2)
    DECLARE_TRANSITION(STA_BUBBLE1_UNLOCKED, EVT_BUBBLE_IDLE, STA_PHONE_LOCKED)

    DECLARE_TRANSITION(STA_UNLOCK_BUBBLE2, EVT_BUBBLE2_SWITCH_OK, STA_BUBBLE2_UNLOCKED)

    DECLARE_TRANSITION(STA_BUBBLE2_UNLOCKED, EVT_PHONE_SWITCH_OK, STA_PHONE_UNLOCKED)

    DECLARE_TRANSITION(STA_PHONE_UNLOCKED, EVT_PHONE_IDLE, STA_PHONE_LOCKED)
    DECLARE_TRANSITION(STA_PHONE_UNLOCKED, EVT_ESC_PRESSED, STA_PHONE_LOCKED)
    DECLARE_TRANSITION(STA_PHONE_UNLOCKED, EVT_PAUSE, STA_PAUSED)

    DECLARE_TRANSITION(STA_PAUSED, EVT_RESUME, STA_PHONE_UNLOCKED)

    DECLARE_TRANSITION(STA_ERR_UNLOCK_B2, EVT_BUBBLE2_SWITCH_OK, STA_PHONE_LOCKED)
END_TRANSITION_DELCARE


// declare animation callback function
static void shutterCallback(MGEFF_ANIMATION anim, void* target, int id, void* value);
static void shutterUp1EndCallback(MGEFF_ANIMATION anim);
static void shutterUp2EndCallback(MGEFF_ANIMATION anim);
static void highlightAnimCallback(MGEFF_ANIMATION anim, void* target, int id, void* value);
static void hintbarAnimFinishCb(MGEFF_ANIMATION anim);
static void toolbarAnimFinishCb(MGEFF_ANIMATION anim);

// declare local helper function
static int registerControlsAndPieces();
static void startHighlight(HWND hwnd);
static void updateTimeAndDate(HWND hwnd);
static BOOL hintbar_onTimer(HWND hwnd, int id, DWORD count);
static BOOL updateInfoBox(void* target, TimeService *ts, int eventId, DWORD param);

static int randomInRange(int low, int high);
static void randomInRect(const RECT* prc, int xy[]);
static void randomBubblePos(int x[], int y[]);

#if 0
static char* currentTime(char* str, int formatType);
static char* currentDate(char* str);
static const char* currentDayOfWeek();
#endif

static BOOL evt_toolbar_menu(mWidget* self, mHotPiece *piece,
        int event_id, DWORD param);
static BOOL evt_toolbar_call(mWidget* self, mHotPiece *piece,
        int event_id, DWORD param);
static BOOL evt_toolbar_sms(mWidget* self, mHotPiece *piece,
        int event_id, DWORD param);

static int app_key_hook(void* context, HWND dst_wnd, 
                        int msg, WPARAM wparam, LPARAM lparam);

// bubble's event handlers
static const MGEFF_FINISHED_CB evt_bubble_switch_ok[] =
{
    ScreenLockActivity::evt_bubble1_switch_ok,
    ScreenLockActivity::evt_bubble2_switch_ok,
};


// events------------------------------------------------------------------
// EVT_BUBBLE1_DOWN
int ScreenLockActivity::evt_bubble1_down(mHotPiece* self, int message, 
        WPARAM wparam, LPARAM lparam, mObject* owner)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(((mWidget*)owner)->hwnd));
    _this->press_bubble1 ();
    return 0;
}

// EVT_BUBBLE2_DOWN
int ScreenLockActivity::evt_bubble2_down(mHotPiece* self, int message, 
        WPARAM wparam, LPARAM lparam, mObject* owner)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(((mWidget*)owner)->hwnd));
    _this->press_bubble2 ();
    return 0;
}

// EVT_BUBBLE1_SWITCH_OK
void ScreenLockActivity::evt_bubble1_switch_ok(MGEFF_ANIMATION anim)
{
    ScreenLockActivity* _this = (ScreenLockActivity*) mGEffAnimationGetContext(anim);
    assert(_this);

    if (_this->m_stateMachine.moveOn(EVT_BUBBLE1_SWITCH_OK)) {
        const int state = _this->m_stateMachine.currentStateId();
        if (state == STA_BUBBLE1_UNLOCKED) {
            // setup bubble idle timer
            SetTimerEx(_this->hwnd(), 
                    BUBBLE_IDLE_TIMER, BUBBLE_IDLE_INTERVAL, evt_bubble_idle);
        }
    }
}

// EVT_BUBBLE2_SWITCH_OK
void ScreenLockActivity::evt_bubble2_switch_ok(MGEFF_ANIMATION anim)
{
    ScreenLockActivity* _this = (ScreenLockActivity*) mGEffAnimationGetContext(anim);
    assert(_this);

    if (_this->m_stateMachine.moveOn(EVT_BUBBLE2_SWITCH_OK)) {
        const int state = _this->m_stateMachine.currentStateId();
        if (state == STA_BUBBLE2_UNLOCKED) {
            if (1 == _this->m_intentFlag) {
                _this->m_intentFlag = 0;
                
                _this->stopHighlight();
                
                TIMESERVICE->removeEventListener((void*)_this->m_infoCtrl->hwnd);
                ACTIVITYSTACK->pop();
            }
            else {
                // unlock phone
                unlock_phone(_this);
            }
        }
    }
}

// EVT_BUBBLE_IDLE
BOOL ScreenLockActivity::evt_bubble_idle(HWND hwnd, int id, DWORD count)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(hwnd));
    ScreenLockActivity::bubble_idle(_this);
    return FALSE;
}

// EVT_PHONE_IDLE
BOOL ScreenLockActivity::evt_phone_idle(HWND hwnd, int id, DWORD count)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(hwnd));
    if (_this->m_stateMachine.moveOn(EVT_PHONE_IDLE)) {
        const int state = _this->m_stateMachine.currentStateId();
        if (state == STA_PHONE_LOCKED) {
            lock_phone(_this);
        }
    }
    return FALSE;
}

// EVT_ESC_PRESSED
int ScreenLockActivity::evt_esc_pressed(mWidget* self, int scancode, int status)
{
    ScreenLockActivity* _this = thisActivity(self->hwnd);

    printf("scancode, status: %d %d\n", scancode, status);

    switch (status) {
    case SCANCODE_ESCAPE:
        printf("Home pressed!\n");

        if (_this->m_stateMachine.moveOn(EVT_ESC_PRESSED)) {
            KillTimer(self->hwnd, PHONE_IDLE_TIMER_ID);
            ScreenLockActivity::lock_phone(_this);
        }

        ScreenLockActivity::bubble_idle(_this);
        break;
    case SCANCODE_BACKSPACE:
        printf("Back pressed!\n");
        
        ScreenLockActivity::bubble_idle(_this);
        break;
    case SCANCODE_1:
        printf("1 pressed!\n");
        
        _this->press_bubble1 ();
        break;
    case SCANCODE_2:
        printf("2 pressed!\n");
        
        _this->press_bubble2 ();
        break;
    }
    return 1;
}

// EVT_PHONE_SWITCH_OK
void ScreenLockActivity::evt_phone_switch_ok(MGEFF_ANIMATION anim)
{
    ScreenLockActivity* _this = (ScreenLockActivity*) mGEffAnimationGetContext(anim);
    assert(_this);

    if (_this->m_stateMachine.moveOn(EVT_PHONE_SWITCH_OK)) {
        const int state = _this->m_stateMachine.currentStateId();
        if (state == STA_PHONE_UNLOCKED) {
            // setup phone idle timer
            // SetTimerEx(_this->hwnd(), PHONE_IDLE_TIMER_ID, SCREEN_IDLE_TIMER, evt_phone_idle);
        }
    }
}

// EVT_MENU_PRESSED
void ScreenLockActivity::evt_menu_pressed(mHotPiece* self, mObject *owner)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(((mWidget*)owner)->hwnd));

    enter_desktop(_this);
}

// actions---------------------------------------------------------------------------
void ScreenLockActivity::press_bubble1 ()
{
    int whichBubble = 0;

    printf("evt_bubble1_down\n");

    if (this->m_stateMachine.moveOn(EVT_BUBBLE1_DOWN)) {
        assert(this->m_stateMachine.currentStateId() == STA_UNLOCK_BUBBLE1);

        // play bubble1's unlock animation
        bubble_unlock_anim(this, whichBubble);
    }
}

void ScreenLockActivity::press_bubble2 ()
{
    int whichBubble = 1;

    printf("evt_bubble2_down\n");

    if (this->m_stateMachine.moveOn(EVT_BUBBLE2_DOWN)) {
        if (this->m_stateMachine.currentStateId() == STA_UNLOCK_BUBBLE2) {
            // play bubble2's unlock animation
            bubble_unlock_anim(this, whichBubble);
        }
        else {
            assert(this->m_stateMachine.currentStateId() == STA_ERR_UNLOCK_B2);
            // play bubble2's error unlock animation
            bubble_err_unlock_anim(this, whichBubble);
        }
    }
}

void ScreenLockActivity::bubble_unlock_anim(ScreenLockActivity* _this, int whichBubble)
{
    mPanelPiece* body = _this->m_bubbleContainer[whichBubble];
    mHotPiece* piece = _this->m_bubblePiece[whichBubble];

    MGEFF_ANIMATION animInflate = _c(body)->scalePieceWithAnimation(body, piece, 
            //BUBBLE_MAX_W, BUBBLE_MAX_H,
            (float)BUBBLE_MAX_W/BUBBLE_W, (float)BUBBLE_MAX_H/BUBBLE_H,
            BUBBLE_NORMAL2MAX_DURATION, InQuad);
    MGEFF_ANIMATION animShrink = _c(body)->scalePieceWithAnimation(body, piece, 
            0, 0,
            BUBBLE_MAX2MIN_DURATION, InQuad);

    assert(animInflate);
    assert(animShrink);

    MGEFF_ANIMATION groupSerial = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);

    mGEffAnimationAddToGroup(groupSerial, animInflate);
    mGEffAnimationAddToGroup(groupSerial, animShrink);

    mGEffAnimationSetContext(groupSerial, _this);
    mGEffAnimationSetFinishedCb(groupSerial, evt_bubble_switch_ok[whichBubble]);

    _c(body)->animationAsyncRun(body, groupSerial, 0);
}

void ScreenLockActivity::bubble_restore_anim(ScreenLockActivity* _this, int whichBubble)
{
    mPanelPiece* body = _this->m_bubbleContainer[whichBubble];
    mHotPiece* piece = _this->m_bubblePiece[whichBubble];

    MGEFF_ANIMATION animInflate = _c(body)->scalePieceWithAnimation(body, piece, 
            (float)1, (float)1,
            BUBBLE_MIN2NORMAL_DURATION, InQuad);

    assert(animInflate);

    MGEFF_ANIMATION groupSerial = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);

    mGEffAnimationAddToGroup(groupSerial, animInflate);

    mGEffAnimationSetContext(groupSerial, _this);
    mGEffAnimationSetFinishedCb(groupSerial, evt_bubble_switch_ok[whichBubble]);

    _c(body)->animationAsyncRun(body, groupSerial, 0);
}

void ScreenLockActivity::bubble_err_unlock_anim(ScreenLockActivity* _this, int whichBubble)
{
    mPanelPiece* body = _this->m_bubbleContainer[whichBubble];
    mHotPiece* piece = _this->m_bubblePiece[whichBubble];

    MGEFF_ANIMATION animInflate = _c(body)->scalePieceWithAnimation(body, piece, 
            //BUBBLE_MAX_W, BUBBLE_MAX_H,
            (float)BUBBLE_MAX_W/BUBBLE_W, (float)BUBBLE_MAX_H/BUBBLE_H,
            BUBBLE_NORMAL2MAX_DURATION, InQuad);
    MGEFF_ANIMATION animShrink = _c(body)->scalePieceWithAnimation(body, piece, 
            (float)1, (float)1,
            BUBBLE_MAX2NORMAL_DURATION, InQuad);

    assert(animInflate);
    assert(animShrink);

    MGEFF_ANIMATION groupSerial = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);

    mGEffAnimationAddToGroup(groupSerial, animInflate);
    mGEffAnimationAddToGroup(groupSerial, animShrink);

    mGEffAnimationSetContext(groupSerial, _this);
    mGEffAnimationSetFinishedCb(groupSerial, evt_bubble_switch_ok[whichBubble]);

    _c(body)->animationAsyncRun(body, groupSerial, 0);
}

void ScreenLockActivity::unlock_phone(ScreenLockActivity* _this)
{
    // stop hintbar highlight animation
    //mGEffAnimationStop(_this->m_highlightAnim);
    _this->stopHighlight();
    _this->m_highlightAnim = NULL;

    MGEFF_ANIMATION anim1 = _c(_this->m_hintbarContainerShell)->movePieceWithAnimation(
            _this->m_hintbarContainerShell, (mHotPiece*)_this->m_hintbarContainer, 
            HINTBAR_X, HINTBAR_H, HINTBAR_MOVE_DURATION, InQuad);
    assert(anim1);

    mGEffAnimationSetContext(anim1, _this);
    mGEffAnimationSetFinishedCb(anim1, hintbarAnimFinishCb);

    // hintbar fly-out
    mPanelPiece* panel = _this->m_hintbarContainerShell;
    _c(panel)->animationAsyncRun(panel, anim1, 0);

    shutter_up1(_this);
}

void ScreenLockActivity::lock_phone(ScreenLockActivity* _this)
{
    MGEFF_ANIMATION anim1 = _this->m_toolbar->getMoveAnimation(
        ACTIVITY_W, 0, TOOLBAR_MOVE_DURATION, InQuad);
    mPanelPiece* panel = _this->m_toolbar->shell();
    MGEFF_ANIMATION bubbleInflate[BUBBLE_NUM];
    int x[BUBBLE_NUM], y[BUBBLE_NUM];
    int i;

    randomBubblePos(x, y);

    // toolbar fly-out
    assert(anim1);
    _c(panel)->animationAsyncRun(panel, anim1, 0);

    mGEffAnimationSetContext(anim1, _this);
    mGEffAnimationSetFinishedCb(anim1, toolbarAnimFinishCb);

    // update bubble's position
    for (i = 0; i < BUBBLE_NUM; ++i) {
        MoveWindow(_this->m_bubbleCtrl[i]->hwnd, x[i], y[i], BUBBLE_MAX_W, BUBBLE_MAX_H, FALSE);
    }

    for (i = 0; i < BUBBLE_NUM; ++i) {
        bubbleInflate[i] = _c(_this->m_bubbleContainer[i])->scalePieceWithAnimation(
            _this->m_bubbleContainer[i], _this->m_bubblePiece[i], 
            1, 1, BUBBLE_MIN2NORMAL_DURATION, InQuad);

        assert(bubbleInflate[i]);

        _c(_this->m_bubbleContainer[i])->animationAsyncRun(
            _this->m_bubbleContainer[i], bubbleInflate[i], 0);
    }

    //_c(_this->m_shutterPiece)->rollText(_this->m_shutterPiece, (mWidget*)_this->m_infoCtrl, USER_INFO);
    shutter_up2(_this);

    // restart hintbar highlight animation
    //mGEffAnimationReStart(_this->m_highlightAnim);
    startHighlight(_this->hwnd());
}

void ScreenLockActivity::shutter_up1(ScreenLockActivity* _this)
{
    RECT start, end;

    _c(_this->m_shutterPiece)->getRect(_this->m_shutterPiece, &start);
    memcpy(&end, &start, sizeof(start));
    end.bottom = SHUTTER_UP_END;

    MGEFF_ANIMATION anim = mGEffAnimationCreate(_this, shutterCallback, 1, MGEFF_RECT);
    mGEffAnimationSetStartValue (anim, &start);
    mGEffAnimationSetEndValue (anim, &end);
    mGEffAnimationSetProperty (anim, MGEFF_PROP_DURATION, SHUTTER_UP1_DURATION);
    mGEffAnimationSetFinishedCb (anim, shutterUp1EndCallback);
    _c(_this->m_infoContainer)->animationAsyncRun(_this->m_infoContainer, anim, 0);
}

void ScreenLockActivity::shutter_up2(ScreenLockActivity* _this)
{
    RECT start, end;

    _c(_this->m_shutterPiece)->getRect(_this->m_shutterPiece, &start);
    memcpy(&end, &start, sizeof(start));
    end.bottom = SHUTTER_UP_END;

    MGEFF_ANIMATION anim = mGEffAnimationCreate(_this, shutterCallback, 1, MGEFF_RECT);
    mGEffAnimationSetStartValue (anim, &start);
    mGEffAnimationSetEndValue (anim, &end);
    mGEffAnimationSetProperty (anim, MGEFF_PROP_DURATION, SHUTTER_UP2_DURATION);
    mGEffAnimationSetFinishedCb (anim, shutterUp2EndCallback);
    _c(_this->m_infoContainer)->animationAsyncRun(_this->m_infoContainer, anim, 0);
}

void ScreenLockActivity::shutter_down1(ScreenLockActivity* _this)
{
    RECT start;
    RECT end = {
        0, 0, SHUTTER_W, SHUTTER_H2
    };

    memcpy(&start, &end, sizeof(start));
    start.bottom = SHUTTER_UP_END;

    MGEFF_ANIMATION anim = mGEffAnimationCreate(_this, shutterCallback, 1, MGEFF_RECT);
    mGEffAnimationSetStartValue (anim, &start);
    mGEffAnimationSetEndValue (anim, &end);
    mGEffAnimationSetProperty (anim, MGEFF_PROP_DURATION, SHUTTER_DOWN1_DURATION);
    _c(_this->m_infoContainer)->animationAsyncRun(_this->m_infoContainer, anim, 0);
}

void ScreenLockActivity::shutter_down2(ScreenLockActivity* _this)
{
    RECT start;
    RECT end = {
        0, 0, SHUTTER_W, SHUTTER_H1
    };

    memcpy(&start, &end, sizeof(start));
    start.bottom = SHUTTER_UP_END;

    MGEFF_ANIMATION anim = mGEffAnimationCreate(_this, shutterCallback, 1, MGEFF_RECT);
    mGEffAnimationSetStartValue (anim, &start);
    mGEffAnimationSetEndValue (anim, &end);
    mGEffAnimationSetProperty (anim, MGEFF_PROP_DURATION, SHUTTER_DOWN2_DURATION);
    //mGEffAnimationAsyncRun(anim, 0);
    _c(_this->m_infoContainer)->animationAsyncRun(_this->m_infoContainer, anim, 0);
}

void ScreenLockActivity::enter_desktop(ScreenLockActivity* _this)
{
    ACTIVITYSTACK->push(APP_NAME_DESKTOP);
}

void ScreenLockActivity::phone_idle(ScreenLockActivity* _this)
{
    if (_this->m_stateMachine.moveOn(EVT_PHONE_IDLE)) {
        const int state = _this->m_stateMachine.currentStateId();
        if (state == STA_PHONE_LOCKED) {
            lock_phone(_this);
        }
    }
}

void ScreenLockActivity::bubble_idle(ScreenLockActivity* _this)
{
    if (_this->m_stateMachine.moveOn(EVT_BUBBLE_IDLE)) {
        // play bubble1 restore animation
        bubble_restore_anim(_this, 0);
    }
}


int ScreenLockActivity::onPause()
{
    // kill phone idle timer
    KillTimer(hwnd(), PHONE_IDLE_TIMER_ID);

    // remove event listener
    //TIMESERVICE->removeEventListener((void*)m_infoCtrl->hwnd);

    fprintf(stderr, "%s:%s:%d, delete secondaryDC\n", __FILE__, __FUNCTION__, __LINE__);
    deleteHWNDSecondaryDC();
    return 0;
}

int ScreenLockActivity::onResume()
{
    fprintf(stderr, "%s:%s:%d, set secondaryDC\n", __FILE__, __FUNCTION__, __LINE__);
    setHWNDSecondaryDC(this->hwnd());

    if (m_stateMachine.moveOn(EVT_RESUME)) {
        // setup phone idle timer
        // SetTimerEx(hwnd(), PHONE_IDLE_TIMER_ID, SCREEN_IDLE_TIMER, evt_phone_idle);

        // add event listener
        //TIMESERVICE->addEventListener((void*)m_infoCtrl->hwnd, TimeService::MINUTE, updateInfoBox);

        RegisterKeyMsgHook((void*)GetMainWindowHandle(self->hwnd), NULL);

        if (2 == m_intentFlag) {
            m_intentFlag = 0;
            //evt_esc_pressed(self, 0, 1);
            phone_idle(this);
        }
    }

    return 0;
}

// animation callback---------------------------------------------------------
static void shutterCallback(MGEFF_ANIMATION anim, void* target, int id, void* value)
{
    ScreenLockActivity* _this = (ScreenLockActivity*) mGEffAnimationGetTarget(anim);
    RECT* prc = (RECT*) value;

    //printf("shutterCallback: %d\n", prc->bottom);

    {
        RECT myRc, bgRc;
        int offset = 0;
        _c(_this->m_shutterPiece)->getRect(_this->m_shutterPiece, &myRc);
        offset = prc->bottom - myRc.bottom;
        _c(_this->m_infoBoxBgPiece)->getRect(_this->m_infoBoxBgPiece, &bgRc);
        bgRc.bottom += offset;
        PanelPiece_invalidatePiece((mHotPiece*)_this->m_infoBoxBgPiece, NULL);
        _c(_this->m_infoBoxBgPiece)->setRect(_this->m_infoBoxBgPiece, &bgRc);
        PanelPiece_invalidatePiece((mHotPiece*)_this->m_infoBoxBgPiece, NULL);
    }
    _c(_this->m_shutterPiece)->setRect(_this->m_shutterPiece, prc);
    PanelPiece_invalidatePiece((mHotPiece*)_this->m_shutterPiece, NULL);
    PanelPiece_invalidatePiece((mHotPiece*)_this->m_shutterPiece, NULL);
    //InvalidateRect(_this->m_infoCtrl->hwnd, NULL, FALSE);
}
static void shutterUp1EndCallback(MGEFF_ANIMATION anim)
{
    ScreenLockActivity* _this = (ScreenLockActivity*) mGEffAnimationGetTarget(anim);

    _c(_this->m_shutterPiece)->setProperty(_this->m_shutterPiece, 
            NCSP_LABELPIECE_LABEL, (DWORD)AD_TEXT);
    _c(_this->m_shutterPiece)->setProperty(_this->m_shutterPiece, 
            NCSP_SHUTTERPIECE_HEIGHT, SHUTTER_H2);

    ScreenLockActivity::shutter_down1(_this);
}
static void shutterUp2EndCallback(MGEFF_ANIMATION anim)
{
    ScreenLockActivity* _this = (ScreenLockActivity*) mGEffAnimationGetTarget(anim);

    _c(_this->m_shutterPiece)->setProperty(_this->m_shutterPiece, 
            NCSP_LABELPIECE_LABEL, (DWORD)USER_INFO);
    _c(_this->m_shutterPiece)->setProperty(_this->m_shutterPiece, 
            NCSP_SHUTTERPIECE_HEIGHT, SHUTTER_H1);

    ScreenLockActivity::shutter_down2(_this);
}

static void highlightAnimCallback(MGEFF_ANIMATION anim, void* target, int id, void* value)
{
    ScreenLockActivity* _this = (ScreenLockActivity*) target;
    int highlight_x = *(int*)value;

    _c(_this->m_highlightPiece)->setProperty(_this->m_highlightPiece, 
            NCSP_HIGHLIGHTPIECE_HIGHTX, highlight_x);
    //UpdateWindow(_this->m_hintbarCtrl->hwnd, FALSE);
    PanelPiece_invalidatePiece((mHotPiece*)_this->m_highlightPiece, NULL);
}
static void hintbarAnimFinishCb(MGEFF_ANIMATION anim)
{
    printf("hintbarAnimFinishCb\n");
    ScreenLockActivity* _this = (ScreenLockActivity*)mGEffAnimationGetContext(anim);
    assert(_this);

    // hintbar move-out
    MoveWindow(_this->m_hintbarCtrl->hwnd, 0, HINTBAR_Y+HINTBAR_H, HINTBAR_W, HINTBAR_H, TRUE);
    //ShowWindow(_this->m_hintbarCtrl->hwnd, SW_HIDE);

    // toolbar fly-in
    MGEFF_ANIMATION anim2 = _this->m_toolbar->getMoveAnimation(0, 0, 
            TOOLBAR_MOVE_DURATION, InQuad);
    assert(anim2);
    mGEffAnimationSetFinishedCb(anim2, ScreenLockActivity::evt_phone_switch_ok);
    mGEffAnimationSetContext(anim2, _this);

    mPanelPiece* panel = _this->m_toolbar->shell();
    _c(panel)->animationAsyncRun(panel, anim2, 0);
}

static void toolbarAnimFinishCb(MGEFF_ANIMATION anim)
{
    printf("toolbarAnimFinishCb\n");
    ScreenLockActivity* _this = (ScreenLockActivity*)mGEffAnimationGetContext(anim);
    assert(_this);

    // hintbar move-in
    MoveWindow(_this->m_hintbarCtrl->hwnd, 0, HINTBAR_Y, HINTBAR_W, HINTBAR_H, TRUE);

    // hintbar fly-in
    MGEFF_ANIMATION anim2 = _c(_this->m_hintbarContainerShell)->movePieceWithAnimation(
            _this->m_hintbarContainerShell, (mHotPiece*)_this->m_hintbarContainer, 
            0, 0, HINTBAR_MOVE_DURATION, OutQuad);
    assert(anim2);

    mPanelPiece* panel = _this->m_hintbarContainerShell;
    _c(panel)->animationAsyncRun(panel, anim2, 0);
}

// initialize helper functions----------------------------------------------
void ScreenLockActivity::initStateMachine()
{
    m_stateMachine.name("screen-lock state machine");

    // add states
    m_stateMachine.addStateTable(stateTable);

    // set init state
    m_stateMachine.setInitState(STA_PHONE_LOCKED);

    // add transition table
    m_stateMachine.addTransitionTable(transitionTable);
}

void ScreenLockActivity::loadMyResource()
{
    m_bgBmp = (PBITMAP) Load32Resource(DESKTOP_IMG_PATH, RES_TYPE_IMAGE, HDC_SCREEN);

    for (int i = 0; i < BUBBLE_NUM; ++i)
        m_bubbleBmp[i] = (PBITMAP) Load32Resource(btn_img_path[i], RES_TYPE_IMAGE, HDC_SCREEN);

    m_timeFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            48, 0);
    m_dateFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            16, 0);
    m_infoFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            20, 0);
    m_hintbarFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            HILIGHT_TEXT_W, 0);
}

void ScreenLockActivity::unloadMyResource()
{
    DestroyLogFont(m_timeFont);
    DestroyLogFont(m_dateFont);
    DestroyLogFont(m_infoFont);
    DestroyLogFont(m_hintbarFont);

    for (int i = 0; i < BUBBLE_NUM; ++i)
        ReleaseRes(Str2Key(btn_img_path[i]));

    ReleaseRes(Str2Key(DESKTOP_IMG_PATH));
}

void ScreenLockActivity::initInfoBox(mWidget* self)
{
    this->m_infoCtrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "m_infoCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            INFOBOX_X, INFOBOX_Y-TITLE_H, INFOBOX_W, INFOBOX_H,
            self->hwnd,
            NULL, NULL, NULL, 0);
    assert(m_infoCtrl);

#if 0
    //const char* xml = "<infobox type='mPanelPiece' width='239'><x><y></y></x><ab><abc></abc></ab></a>";
    //const char* xmlFile = "apps/screen-lock/panel.xml";
    //PanelManager panelMgr;
    //panelMgr.parseXmlFile(xmlFile);

    char xml[1024];
    sprintf(xml,
            "<infobox type='mPanelPiece' width='%d' height='%d'>"
            "    <background0 type='mShapeTransRoundPiece' width='%d' height='%d' left='%d' top='%d'></background0>"
            "    <background1 type='mShapeTransRoundPiece' width='%d' height='%d' left='%d' top='%d'></background1>"
            "    <time        type='mTextPiece' width='%d' height='%d' left='%d' top='%d'></time>"
            "    <date        type='mTextPiece' width='%d' height='%d' left='%d' top='%d'></date>"
            "    <shutter     type='mShutterPiece' width='%d' height='%d' left='%d' top='%d' additonaldata='shutter_add'></shutter>"
            "</infobox>", 
            INFOBOX_W, INFOBOX_H,
            INFOBOX_W, INFOBOX_H1, 0, INFOBOX_Y1,
            INFOBOX_W, INFOBOX_H2, 0, INFOBOX_Y2,
            TIME_W, TIME_H, TIME_X, TIME_Y,
            DATE_W, DATE_H, DATE_X, DATE_Y,
            SHUTTER_W, SHUTTER_H, SHUTTER_X, SHUTTER_Y
            );

    PanelManager panelMgr;
    RECT* prc;

    prc = (RECT*)malloc(sizeof(RECT));
    memset(prc, 0, sizeof(RECT));
    prc->right = SHUTTER_W;
    prc->bottom = SHUTTER_H;
    panelMgr.putData("shutter_add", prc);

    panelMgr.parseXml(xml);

    free(prc);
    prc = NULL;

    m_infoContainer = (mPanelPiece*)panelMgr.get("infobox");
    assert(this->m_infoContainer && INSTANCEOF(this->m_infoContainer, mPanelPiece));
    _c(this->m_infoCtrl)->setBody(this->m_infoCtrl, (mHotPiece*)m_infoContainer);

    m_timePiece = panelMgr.get("time");
    assert(this->m_timePiece && INSTANCEOF(this->m_timePiece, mTextPiece));
    _c(this->m_timePiece)->setProperty(this->m_timePiece, 
            NCSP_TEXTPIECE_LOGFONT, (DWORD)this->m_timeFont);
    _c(this->m_timePiece)->setProperty(this->m_timePiece, 
            NCSP_LABELPIECE_LABEL, (DWORD)"15:19");

    m_datePiece = panelMgr.get("date");
    assert(this->m_datePiece && INSTANCEOF(this->m_datePiece, mTextPiece));
    _c(this->m_datePiece)->setProperty(this->m_datePiece, 
            NCSP_TEXTPIECE_LOGFONT, (DWORD)this->m_dateFont);
    _c(this->m_datePiece)->setProperty(this->m_datePiece, 
            NCSP_LABELPIECE_LABEL, (DWORD)"2010/10/26 Wed");

    m_shutterPiece = (mShutterPiece*)panelMgr.get("shutter");
    assert(m_shutterPiece && INSTANCEOF(m_shutterPiece, mShutterPiece));
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_TEXTPIECE_TEXTCOLOR, 0xFFFFFFFF);
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_TEXTPIECE_LOGFONT, (DWORD)m_infoFont);
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_LABELPIECE_LABEL, (DWORD)USER_INFO);

    {
    mShapeTransRoundPiece* back_gradient = (mShapeTransRoundPiece*)panelMgr.get("background0");
    assert(back_gradient && INSTANCEOF(back_gradient, mShapeTransRoundPiece));
    _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 
            TRANROUND_CORNERFLAG_LEFTTOP | TRANROUND_CORNERFLAG_RIGHTTOP);
    HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
            (mShapeTransRoundPiece*)back_gradient);
    /* 0.0f, 1.0f are needed */
    for (unsigned int i=0; i<TABLESIZE(infobox1_gradient); ++i)
        MGPlusLinearGradientBrushAddColor(brush, infobox1_gradient[i].color, infobox1_gradient[i].pos);
    }

    {
    mShapeTransRoundPiece* back_gradient = (mShapeTransRoundPiece*)panelMgr.get("background1");
    m_infoBoxBgPiece = (mHotPiece*)back_gradient;
    assert(back_gradient && INSTANCEOF(back_gradient, mShapeTransRoundPiece));
    _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 
            TRANROUND_CORNERFLAG_LEFTBOTTOM | TRANROUND_CORNERFLAG_RIGHTBOTTOM);

    HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
            (mShapeTransRoundPiece*)back_gradient);

    /* 0.0f, 1.0f are needed */
    for (unsigned int i=0; i<TABLESIZE(infobox2_gradient); ++i)
        MGPlusLinearGradientBrushAddColor(brush, infobox2_gradient[i].color, infobox2_gradient[i].pos);
    }

#else

    mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    m_infoContainer = body;

    RECT rc;
    rc.left = 0;
    rc.top = 0;

    rc.right = INFOBOX_W;
    rc.bottom = INFOBOX_H;
    _c(body)->setRect (body, &rc);
    _c(this->m_infoCtrl)->setBody(this->m_infoCtrl, (mHotPiece*)body);

    this->m_timePiece = (mHotPiece*)NEWPIECE(mTextPiece);
    this->m_datePiece = (mHotPiece*)NEWPIECE(mTextPiece);
    //this->m_separatorPiece = (mHotPiece*)NEWPIECE(mSeparatorPiece);
    this->m_shutterPiece = (mShutterPiece*)NEWPIECE(mShutterPiece);

    rc.right = TIME_W;
    rc.bottom = TIME_H;
    _c(this->m_timePiece)->setRect(this->m_timePiece, &rc);
    _c(this->m_timePiece)->setProperty(
        this->m_timePiece, 
        NCSP_TEXTPIECE_LOGFONT, (DWORD)this->m_timeFont);
    _c(this->m_timePiece)->setProperty(
        this->m_timePiece, 
        NCSP_LABELPIECE_LABEL, (DWORD)"15:19");
    _c(this->m_timePiece)->setProperty(
        this->m_timePiece, 
        NCSP_LABELPIECE_ALIGN, (DWORD)NCS_ALIGN_CENTER);
    _c(this->m_timePiece)->setProperty(
            this->m_timePiece,
            NCSP_TEXTPIECE_TEXTSHADOWCOLOR, (DWORD)0x88404040);

    rc.right = DATE_W;
    rc.bottom = DATE_H;
    _c(this->m_datePiece)->setRect(this->m_datePiece, &rc);
    _c(this->m_datePiece)->setProperty(this->m_datePiece, 
            NCSP_TEXTPIECE_LOGFONT, (DWORD)this->m_dateFont);
    _c(this->m_datePiece)->setProperty(this->m_datePiece, 
            NCSP_LABELPIECE_LABEL, (DWORD)"2010/10/26 Wed");
    _c(this->m_datePiece)->setProperty(this->m_datePiece, 
            NCSP_TEXTPIECE_TEXTSHADOWCOLOR, (DWORD)0x88404040);

    //rc.right = SEPARATOR_W;
    //rc.bottom = SEPARATOR_H;
    //_c(this->m_separatorPiece)->setRect(this->m_separatorPiece, &rc);

    rc.right = SHUTTER_W;
    rc.bottom = SHUTTER_H1;
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_SHUTTERPIECE_WIDTH, SHUTTER_W);
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_SHUTTERPIECE_HEIGHT, SHUTTER_H1);
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_TEXTPIECE_TEXTCOLOR, 0xFFFFFFFF);
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_TEXTPIECE_LOGFONT, (DWORD)m_infoFont);
    _c(this->m_shutterPiece)->setRect(this->m_shutterPiece, &rc);
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_LABELPIECE_LABEL, (DWORD)USER_INFO);
    _c(this->m_shutterPiece)->setProperty(this->m_shutterPiece, 
            NCSP_TEXTPIECE_TEXTSHADOWCOLOR, (DWORD)0x88404040);
    
#if 0
    /* background.*/
    mHotPiece *back = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    rc.right = INFOBOX_W;
    rc.bottom = INFOBOX_H;
    _c(back)->setRect (back, &rc);
    _c(back)->setProperty (back, NCSP_TRANROUND_BKCOLOR, 0xD0B0D86D);
    //_c(back)->setProperty (back, NCSP_RECTPIECE_YRADIUS, 5);
    //_c(back)->setProperty (back, NCSP_RECTPIECE_BORDERSIZE, 0);
    //_c(back)->setProperty (back, NCSP_RECTPIECE_XRADIUS, 5);
    _c(body)->addContent(body, back, 0, 0);
#else
    {
        mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        rc.right = INFOBOX_W;
        rc.bottom = INFOBOX_H1;
        _c(back_gradient)->setRect(back_gradient, &rc);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 
                TRANROUND_CORNERFLAG_LEFTTOP | TRANROUND_CORNERFLAG_RIGHTTOP);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_RADIUS, INFO_BOX_RADIUS);

        HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
                (mShapeTransRoundPiece*)back_gradient);

        /* 0.0f, 1.0f are needed */
        for (unsigned int i=0; i<TABLESIZE(infobox1_gradient); ++i)
            MGPlusLinearGradientBrushAddColor(brush, infobox1_gradient[i].color, infobox1_gradient[i].pos);

        _c(body)->addContent(body, back_gradient, 0, INFOBOX_Y1);
    }
    {
        mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        rc.right = INFOBOX_W;
        rc.bottom = INFOBOX_H2;
        _c(back_gradient)->setRect(back_gradient, &rc);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 
                TRANROUND_CORNERFLAG_LEFTBOTTOM | TRANROUND_CORNERFLAG_RIGHTBOTTOM);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_RADIUS, INFO_BOX_RADIUS);

        HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
                (mShapeTransRoundPiece*)back_gradient);

        /* 0.0f, 1.0f are needed */
        for (unsigned int i=0; i<TABLESIZE(infobox2_gradient); ++i)
            MGPlusLinearGradientBrushAddColor(brush, infobox2_gradient[i].color, infobox2_gradient[i].pos);

        _c(body)->addContent(body, back_gradient, 0, INFOBOX_Y2);

        m_infoBoxBgPiece = back_gradient;
    }
#endif

    _c(body)->addContent(body, this->m_timePiece, TIME_X, TIME_Y);
    _c(body)->addContent(body, this->m_datePiece, DATE_X, DATE_Y);
    //_c(body)->addContent(body, this->m_separatorPiece, SEPARATOR_X, SEPARATOR_Y);
    _c(body)->addContent(body, (mHotPiece*)this->m_shutterPiece, SHUTTER_X, SHUTTER_Y);
#endif
}

void ScreenLockActivity::initBubbles(mWidget* self)
{
    int x[2], y[2];
    RECT rc;

    randomBubblePos(x, y);

    rc.left = 0;
    rc.top = 0;
    
    for (int i = 0; i < BUBBLE_NUM; ++i) {

        this->m_bubbleCtrl[i] = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "m_infoCtrl",
                WS_VISIBLE, WS_EX_TRANSPARENT, 0,
                x[i], y[i], BUBBLE_MAX_W, BUBBLE_MAX_H,
                self->hwnd,
                NULL, NULL, NULL, 0);
        mContainerCtrl* ctrl = this->m_bubbleCtrl[i];

        this->m_bubbleContainer[i] = (mPanelPiece*)NEWPIECE(mPanelPiece);
        mPanelPiece *body = this->m_bubbleContainer[i];

        rc.right = BUBBLE_MAX_W;
        rc.bottom = BUBBLE_MAX_H;
        _c(body)->setRect (body, &rc);
        _c(ctrl)->setBody(ctrl, (mHotPiece*)body);

        this->m_bubblePiece[i] = (mHotPiece*)NEWPIECE(mImagePiece);
        mHotPiece* piece = this->m_bubblePiece[i];
#if 1
        rc.right = BUBBLE_W;
        rc.bottom = BUBBLE_H;
#else
        rc.right = BUBBLE_MAX_W;
        rc.bottom = BUBBLE_MAX_H;
#endif
        _c(piece)->setRect(piece, &rc);
        _c(piece)->setProperty(piece, NCSP_IMAGEPIECE_IMAGE, (DWORD)this->m_bubbleBmp[i]);
#if 1
        _c(body)->addContent(body, piece, 
                (BUBBLE_MAX_W-BUBBLE_W)/2, (BUBBLE_MAX_H-BUBBLE_H)/2);
#else
        _c(body)->addContent(body, piece, 0,0);
#endif

    }

    _c(this->m_bubbleContainer[0])->appendEventHandler(this->m_bubbleContainer[0], MSG_LBUTTONDOWN, evt_bubble1_down);
    _c(this->m_bubbleContainer[1])->appendEventHandler(this->m_bubbleContainer[1], MSG_LBUTTONDOWN, evt_bubble2_down);
}

void ScreenLockActivity::initHintBar(mWidget* self)
{
    RECT rc;

    this->m_hintbarCtrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "m_hintbarCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            HINTBAR_X, HINTBAR_Y, HINTBAR_W, HINTBAR_H,
            self->hwnd,
            NULL, NULL, NULL, 0);
    mContainerCtrl* ctrl = this->m_hintbarCtrl;

    this->m_hintbarContainerShell = (mPanelPiece*)NEWPIECE(mPanelPiece);
    this->m_hintbarContainer = (mPanelPiece*)NEWPIECE(mPanelPiece);

    mPanelPiece *body = this->m_hintbarContainerShell;

    rc.left = 0;
    rc.top = 0;
    rc.right = HINTBAR_W;
    rc.bottom = HINTBAR_H;

    _c(body)->setRect (body, &rc);
    _c(ctrl)->setBody(ctrl, (mHotPiece*)body);

    body = this->m_hintbarContainer;

    _c(body)->setRect (body, &rc);

    {
        mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_gradient)->setRect (back_gradient, &rc);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 0);

#if 0
        ARGB colors[3] = {0x90F0F0F0, 0x90808080};
        _c((mShapeTransRoundPiece*)back_gradient)->setGradientFillColors(
                (mShapeTransRoundPiece*)back_gradient, colors, TABLESIZE(colors));
#else
        HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
                (mShapeTransRoundPiece*)back_gradient);

        /* 0.0f, 1.0f are needed */
        for (unsigned int i=0; i<TABLESIZE(hintbar_gradient); ++i)
            MGPlusLinearGradientBrushAddColor(brush, hintbar_gradient[i].color, hintbar_gradient[i].pos);
#endif
        _c(body)->addContent(body, back_gradient, 0, 0);
    }

    this->m_highlightPiece = (mHighlightPiece*)NEWPIECE(mHighlightPiece);
    rc.right = HINTBAR_W;
    rc.bottom = HINTBAR_H;
    _c(this->m_highlightPiece)->setRect(this->m_highlightPiece, &rc);
    _c(m_highlightPiece)->setProperty(m_highlightPiece, NCSP_LABELPIECE_LABEL, (DWORD)HINTBAR_TEXT);
    _c(m_highlightPiece)->setProperty(m_highlightPiece, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xFFFFFFFF);
    _c(m_highlightPiece)->setProperty(m_highlightPiece, NCSP_TEXTPIECE_LOGFONT, (DWORD)m_hintbarFont);
    _c(m_highlightPiece)->setProperty(m_highlightPiece, NCSP_HIGHLIGHTPIECE_HIGHTX, (DWORD)HIGHLIGHT_START_X);

    _c(body)->addContent(body, (mHotPiece*)this->m_highlightPiece, 0, 0);
    _c(m_hintbarContainerShell)->addContent(m_hintbarContainerShell, (mHotPiece*)this->m_hintbarContainer, 0, 0);
}

void ScreenLockActivity::initToolBar(mWidget* self)
{
    m_toolbar = new DesktopToolbar(self->hwnd, false);
    m_toolbar->addBtnEventListener(
            DesktopToolbar::TB_MENU, NCSN_ABP_CLICKED, (NCS_CB_ONPIECEEVENT)evt_toolbar_menu);
   m_toolbar->addBtnEventListener(
           DesktopToolbar::TB_MSG, NCSN_ABP_CLICKED, (NCS_CB_ONPIECEEVENT)evt_toolbar_sms);
   m_toolbar->addBtnEventListener(
           DesktopToolbar::TB_CALL, NCSN_ABP_CLICKED, (NCS_CB_ONPIECEEVENT)evt_toolbar_call);
}

// other helper functions------------------------------------------------
static int registerControlsAndPieces()
{
    MGNCS_INIT_CLASS(mShutterPiece);
    MGNCS_INIT_CLASS(mHighlightPiece);
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_INIT_CLASS(mTextPiece);
    return 0;
}

static void startHighlight(HWND hwnd)
{
    int start = HIGHLIGHT_START_X;  //27;
    int end = 450;  //334;
    int duration = HIGHLIGHT_DURATION;
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(hwnd));

    MGEFF_ANIMATION anim = mGEffAnimationCreate(_this, highlightAnimCallback, 1, MGEFF_INT);
    mGEffAnimationSetStartValue (anim, &start);
    mGEffAnimationSetEndValue(anim, &end);
    mGEffAnimationSetProperty(anim, MGEFF_PROP_DURATION, duration);
    mGEffAnimationSetProperty(anim, MGEFF_PROP_LOOPCOUNT, MGEFF_INFINITE);

    _this->m_highlightAnim = anim;

    //mGEffAnimationAsyncRun(anim, 1);
    _c(_this->m_hintbarContainerShell)->animationAsyncRun(_this->m_hintbarContainerShell, anim, 0);
}


void ScreenLockActivity::stopHighlight()
{
    if (this->m_highlightAnim) {
        //HWND hwnd = this->hwnd();
        mGEffAnimationStop(this->m_highlightAnim);
        //mGEffAnimationWait(&hwnd, this->m_highlightAnim);
        this->m_highlightAnim = NULL;
    }
}   

static void updateTimeAndDate(HWND hwnd)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(hwnd));

    // TODO:
    if (_this == NULL)
        return;

#if 0
    static char time[16], date[16], dateAndWDay[32];

    currentTime(time, TIME_FORMAT_24);
    currentDate(date);
    sprintf(dateAndWDay, "%s %s", date, currentDayOfWeek());

    //printf("time:%s\n", time);
    //printf("date:%s\n", dateAndWDay);
#else
    static char time[32], dateAndWDay[128];

    if (TIMESERVICE->format(time, sizeof(time), "%H:%M") == 0) {
        fprintf(stderr, "TimeService::format returned 0\n");
        exit(1);
    }
    if (TIMESERVICE->format(dateAndWDay, sizeof(dateAndWDay), "%Y/%m/%d %A") == 0) {
        fprintf(stderr, "TimeService::format returned 0\n");
        exit(1);
    }
#endif

    _c(_this->m_timePiece)->setProperty(_this->m_timePiece, NCSP_LABELPIECE_LABEL, (DWORD)time);
    _c(_this->m_datePiece)->setProperty(_this->m_datePiece, NCSP_LABELPIECE_LABEL, (DWORD)dateAndWDay);

    PanelPiece_invalidatePiece((mHotPiece *)(_this->m_timePiece),NULL);
    PanelPiece_invalidatePiece((mHotPiece *)(_this->m_datePiece),NULL);
}

static BOOL updateInfoBox(void* target, TimeService *ts, int eventId, DWORD param)
{
    updateTimeAndDate((HWND)target);

    return TRUE;
}

static int randomInRange(int low, int high)
{
    return low + rand() % (high-low+1);
}

static void randomInRect(const RECT* prc, int xy[])
{
    xy[0] = randomInRange(prc->left, prc->right);
    xy[1] = randomInRange(prc->top, prc->bottom);
}

static void randomBubblePos(int x[], int y[])
{
#if 0
    const int x_min = ACTIVITY_X;
    const int x_max = ACTIVITY_X+ACTIVITY_W-BUBBLE_MAX_W;
    const int y_min = INFOBOX_Y+INFOBOX_H;
    const int y_max = HINTBAR_Y-BUBBLE_MAX_H;
    const int xgap_min = BUBBLE_MAX_W+5;
    const int ygap_min = BUBBLE_MAX_H+5;
    x[0] = randomInRange(x_min, x_max);
    y[0] = randomInRange(y_min, y_max);
    while (1) {
        x[1] = randomInRange(x_min, x_max);
        y[1] = randomInRange(y_min, y_max);
        if ( abs(x[1]-x[0]) >= xgap_min || abs(y[1]-y[0]) >= ygap_min )
                break;
    }
#else

    const int x_min = ACTIVITY_X;
    const int x_max = ACTIVITY_X+ACTIVITY_W;
    const int y_min = INFOBOX_Y+INFOBOX_H;
    const int y_max = HINTBAR_Y;

    //printf("border: %d, %d, %d, %d\n", x_min, y_min, x_max, y_max);

    const int x_mid = x_min + (x_max-x_min)/2;
    const int y_mid = y_min + (y_max-y_min)/2;

    RECT rc_lt = { x_min, y_min, x_mid, y_mid };
    RECT rc_rt = { x_mid, y_min, x_max, y_mid };
    RECT rc_rb = { x_mid, y_mid, x_max, y_max };
    RECT rc_lb = { x_min, y_mid, x_mid, y_max };

    rc_lt.right -= BUBBLE_MAX_W;
    rc_lt.bottom -= BUBBLE_MAX_H;

    rc_rt.right -= BUBBLE_MAX_W;
    rc_rt.bottom -= BUBBLE_MAX_H;

    rc_rb.right -= BUBBLE_MAX_W;
    rc_rb.bottom -= BUBBLE_MAX_H;

    rc_lb.right -= BUBBLE_MAX_W;
    rc_lb.bottom -= BUBBLE_MAX_H;

    // get 4 areas
    RECT* area[4];
    area[0] = &rc_lt;
    area[1] = &rc_rt;
    area[2] = &rc_rb;
    area[3] = &rc_lb;

#if 0
    for (int i=0; i<4; ++i) {
        printf("area %d: %d %d %d %d\n", i,
                area[i]->left, area[i]->top, area[i]->right, area[i]->bottom);
    }
#endif

    // pick areas for two bubbles
    int b1 = randomInRange(0, 3);
    int b2 = (b1 + 2) % 4;

    //printf("bubble area: %d, %d\n", b1, b2);

    int xy[2];

    // get bubble1's position
    randomInRect(area[b1], xy);
    x[0] = xy[0];
    y[0] = xy[1];

    // get bubble2's position
    randomInRect(area[b2], xy);
    x[1] = xy[0];
    y[1] = xy[1];

#if 0
    for (int i=0; i<2; ++i) {
        printf("bubble %d: %d %d\n", i, x[i], y[i]);
    }
#endif

#endif
}

#if 0
static char* currentTime(char* str, int formatType)
{
    int hour = 0;
    int min = 0;

#if 0
    struct timeval tv;
    struct tm* tm;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    hour = tm->tm_hour;
    min = tm->tm_min;
#else
    TimeService* ts = TimeService::singleton();
    hour = ts->get(TimeService::HOUR);
    min = ts->get(TimeService::MINUTE);
#endif

    if (formatType == TIME_FORMAT_12) {
        if (hour == 0) hour = 12;
        else if (hour > 12) hour -= 12;
    }
    sprintf(str, "%02d:%02d%s", hour, min, 
            formatType==TIME_FORMAT_24?"":(hour>=12?" PM":" AM"));

    return str;
}
static char* currentDate(char* str)
{
    int year = 2010;
    int moth = 10;
    int mday = 10;

#if 0
    struct timeval tv;
    struct tm* tm;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    year = tm->tm_year+1900;
    moth = tm->tm_mon+1;
    mday = tm->tm_mday;
#else
    TimeService* ts = TimeService::singleton();
    year = ts->get(TimeService::YEAR);
    moth = ts->get(TimeService::MONTH);
    mday = ts->get(TimeService::MDAY);
#endif

    sprintf(str, "%04d/%02d/%02d", year, moth, mday);

    return str;
}
static const char* currentDayOfWeek()
{
    int wday = 0;
    static const char* day_of_week_str[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

#if 0
    struct timeval tv;
    struct tm* tm;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    wday = tm->tm_wday;
#else
    TimeService* ts = TimeService::singleton();
    wday = ts->get(TimeService::WDAY);
#endif

    return day_of_week_str[wday];
}
#endif

BOOL evt_toolbar_menu(mWidget* self, mHotPiece *piece,
        int event_id, DWORD param)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(self->hwnd));

    if (event_id == NCSN_ABP_CLICKED) {
        printf("\ttoolbar menu clicked\n");

        if (_this->m_stateMachine.moveOn(EVT_PAUSE)) {
            _this->evt_menu_pressed((mHotPiece*)piece, (mObject*)self);
        }
        return TRUE;
    }

    return FALSE;
}

static int app_key_hook(void* context, HWND dst_wnd, 
                        int msg, WPARAM wparam, LPARAM lparam)
{
    int scancode = (int)wparam;
    
    switch (msg) {
    case MSG_KEYUP:
        if (SCANCODE_BACKSPACE == scancode) {
            SendNotifyMessage((HWND)context, MSG_USER_BACK,
                              0, 0);
        }
        else if (SCANCODE_ESCAPE == scancode) {
            SendNotifyMessage((HWND)context, MSG_USER_RETURN_TO_STANDBY,
                              0, 0);
        }

        break;
    }
    return HOOK_STOP;
}

BOOL evt_toolbar_call(mWidget* self, mHotPiece *piece,
                      int event_id, DWORD param)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(self->hwnd));
        
    if (event_id == NCSN_ABP_CLICKED) {
        if (_this->m_stateMachine.moveOn(EVT_PAUSE)) {
            ActivityStack::singleton()->push("CallHistoryActivity",(Intent*)1);
            RegisterKeyMsgHook((void*)GetMainWindowHandle(self->hwnd), app_key_hook);
        }
        return TRUE;
    }

    return FALSE;
}

BOOL evt_toolbar_sms(mWidget* self, mHotPiece *piece,
                     int event_id, DWORD param)
{
    ScreenLockActivity* _this = thisActivity(GetMainWindowHandle(self->hwnd));
    
    if (event_id == NCSN_ABP_CLICKED) {
        if (_this->m_stateMachine.moveOn(EVT_PAUSE)) {
            ActivityStack::singleton()->switchTo("SMSActivity");
            RegisterKeyMsgHook((void*)GetMainWindowHandle(self->hwnd), app_key_hook);
        }
        return TRUE;
    }

    return FALSE;
}


// main window's message handler---------------------------------------------
static BOOL mainWnd_onCreate(mWidget* self, DWORD add_data)
{
    ScreenLockActivity* _this = thisActivity(self->hwnd);
    assert(_this);
//    PostMessage(self->hwnd, MSG_REGISTERKEYHOOK, 0, (LPARAM)common_key_hook);

    _this->self = self;

#ifdef ACTIVITY_NO_SECONDARYDC 
    _this->setHWNDSecondaryDC(self->hwnd);
#endif

    thisActivity(self->hwnd)->initInfoBox(self);
    thisActivity(self->hwnd)->initBubbles(self);
    thisActivity(self->hwnd)->initHintBar(self);
    thisActivity(self->hwnd)->initToolBar(self);

    SetTimerEx(_this->m_hintbarCtrl->hwnd, START_HIGHLIGHT_TIMER, START_HIGHLIGHT_DURATION, hintbar_onTimer);

    // update time & date
    updateTimeAndDate(_this->m_infoCtrl->hwnd);
    TIMESERVICE->addEventListener((void*)_this->m_infoCtrl->hwnd, TimeService::MINUTE, updateInfoBox);
    TIMESERVICE->addEventListener((void*)_this->m_infoCtrl->hwnd, TimeService::HOUR, updateInfoBox);
    return TRUE;
}
static void mainWnd_onPaint(mWidget *self, HDC hdc, const CLIPRGN* inv)
{
    FillBoxWithBitmap(hdc, 0, 0, ACTIVITY_W, ACTIVITY_H, thisActivity(self->hwnd)->m_bgBmp);
//    FillBitmapPartInBox(hdc,0, 0, 0, 0, thisActivity(self->hwnd)->m_bgBmp, 0, 0, 0, 0);
}

static void mainWnd_onClose(mWidget* self, int message)
{
    printf("stopHighlight onClose\n");
    
    return;
}

static void mainWnd_onDestroy(mWidget* self, int message)
{
    ScreenLockActivity* _this = thisActivity(self->hwnd);
    fprintf(stderr, "%s:destroy.\n", __FUNCTION__);
    _this->stopHighlight();
#ifdef ACTIVITY_NO_SECONDARYDC 
    _this->deleteHWNDSecondaryDC();
#endif
    return;
}

static int mainWnd_onKeyUp(mWidget* self, int scancode, int status)
{
    return ScreenLockActivity::evt_esc_pressed(self, scancode, status);
}

static int mainWnd_onAppData(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
    ScreenLockActivity* _this = thisActivity(self->hwnd);
    
    _this->m_intentFlag = wParam;
    
    return 0;
}

static int mainWnd_onIdle(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
    ScreenLockActivity* _this = thisActivity(self->hwnd);

    ScreenLockActivity::phone_idle(_this);
    
    return 0;
}

static int mainWnd_onUserClick(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
    ScreenLockActivity* _this = thisActivity(self->hwnd);

    ScreenLockActivity::bubble_idle(_this);
    
    return 0;
}

static int mainWnd_onUserBack(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
    //ScreenLockActivity* _this = thisActivity(self->hwnd);
    
    if (ACTIVITYSTACK->top() && strcmp(ACTIVITYSTACK->currentAppName(), "ScreenLockActivity") != 0) {
        ACTIVITYSTACK->back();
    }
        
    return 0;
}


static int mainWnd_onUserReturnToStandby(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
    //ScreenLockActivity* _this = thisActivity(self->hwnd);
    
    if (ACTIVITYSTACK->top() && strcmp(ACTIVITYSTACK->currentAppName(), "ScreenLockActivity") != 0) {
        ACTIVITYSTACK->switchTo("ScreenLockActivity");//, (Intent*) 2);
    }
        
    return 0;
}



// control's message handler-------------------------------------------------
static BOOL hintbar_onTimer(HWND hwnd, int id, DWORD count)
{
    printf("hintbar_onTimer\n");
    startHighlight(hwnd);
    return FALSE;
}


// ncs template
static NCS_EVENT_HANDLER mymain_handlers [] = {
        {MSG_CREATE, reinterpret_cast<void*>(mainWnd_onCreate)},
        {MSG_PAINT,  reinterpret_cast<void*>(mainWnd_onPaint)},
        {MSG_CLOSE,  reinterpret_cast<void*>(mainWnd_onClose)},
        {MSG_DESTROY,  reinterpret_cast<void*>(mainWnd_onDestroy)},
        {MSG_KEYUP,    reinterpret_cast<void*>(mainWnd_onKeyUp)},
        {MSG_USER_APP_DATA,  reinterpret_cast<void*>(mainWnd_onAppData)},
        {MSG_USER_IDLE,  reinterpret_cast<void*>(mainWnd_onIdle)},
        {MSG_USER_CLICK,  reinterpret_cast<void*>(mainWnd_onUserClick)},
        {MSG_USER_BACK,  reinterpret_cast<void*>(mainWnd_onUserBack)},
        {MSG_USER_RETURN_TO_STANDBY,  reinterpret_cast<void*>(mainWnd_onUserReturnToStandby)},
        {0, NULL}
};

static NCS_WND_TEMPLATE _ctrl_templ[] = {
    {0, NULL}
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_NONE,
        "ScreenLockActivity",
        NULL,
        NULL,
        mymain_handlers,
        _ctrl_templ,
        sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
        0,
        0, 0,
};


// constructor & destructor
ScreenLockActivity::ScreenLockActivity() :
    NCSActivity(&mymain_templ), m_highlightAnim(NULL), m_intentFlag(0)
{ 
    registerControlsAndPieces();

    initStateMachine();

    loadMyResource();
}

ScreenLockActivity::~ScreenLockActivity()
{
    unloadMyResource();
    delete m_toolbar;
}

