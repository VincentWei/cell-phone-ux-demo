///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>
#include <mgncs4touch/mgncs4touch.h>

#include "TimeService.hh"
#include "ActivityStack.hh"
#include "ActivityDesktop.hh"
#include "DesktopToolbar.hh"
#include "IPhoneDesktopController.hh"
#include "IPhoneDesktopItem.hh"
#include "DesktopCommonProc.hh"

#define BKGND_DC_COLORKEY RGBA2Pixel(g_background_dc, 0xff, 0, 0, 0)

REGISTER_ACTIVITY(ActivityDesktop);

HWND ActivityDesktop::s_desktop = HWND_NULL;
static HDC g_background_dc;

HWND ActivityDesktop::getDesktopHwnd()
{
    return s_desktop;
}

ActivityDesktop::ActivityDesktop() :
Activity(), m_toolbar(NULL), m_animation(NULL), m_controller(NULL)
{
    m_hasBeenPoped = FALSE;
}

ActivityDesktop::~ActivityDesktop() {
    if (g_background_dc != HDC_INVALID) {
        DeleteMemDC(g_background_dc);
        g_background_dc = HDC_INVALID;
    }

#ifdef DEBUG
    fprintf(stderr, "=================%s:%d.\n", __FUNCTION__, __LINE__);
#endif
    delete m_toolbar;
}

LRESULT ActivityDesktop::DesktopProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            {
                HDC memDC;
                s_desktop = g_desktop_hwnd = hWnd;
                //                PostMessage(HWND_DESKTOP, MSG_REGISTERKEYHOOK, 0, (LPARAM)common_key_hook);
#ifdef _MGRM_PROCESSES
                RegisterKeyHookWindow (hWnd, HOOK_GOON);
#else
                RegisterKeyMsgHook(NULL, common_key_hook);
#endif

                memDC = CreateCompatibleDCEx(Get32MemDC(),SCREEN_W, SCREEN_H - ACTIVITY_Y);
                assert(memDC);
                SetSecondaryDC(hWnd,memDC , ON_UPDSECDC_DEFAULT);

                RECT rc;
                HWND body;
                IPhoneDesktopLayout* layout = NULL;
                GetClientRect(hWnd, &rc);
                body = CreateWindow(CTRL_STATIC, "body", 
                        WS_CHILD | WS_VISIBLE, IDC_DESKTOP_BODY, 
                        rc.left, rc.top, RECTW(rc), RECTH(rc), hWnd, 0);
                IPhoneDesktopController::create(body);

                ActivityDesktop* _this = 
                    dynamic_cast<ActivityDesktop*>(getActivityFromHWND(hWnd));
                assert(NULL != _this);

                _this->m_controller =
                    IPhoneDesktopController::getControllerFromHwnd(GetDlgItem(hWnd, IDC_DESKTOP_BODY)); 
                assert(NULL != _this->m_controller);
                const RECT& crc = _this->m_controller->getView().getRect();

                _this->createToolbar(hWnd);
                rc.bottom -= 49; // exclude the toolbar's height.
                _this->m_controller->getView().setAnimationArea(rc);

                //g_background_dc = CreateCompatibleDCEx(memDC, RECTW(crc), RECTH(crc));
                g_background_dc = CreateMyCompatibleDC(memDC, RECTW(crc), RECTH(crc));
                if (GetGDCapability(g_background_dc, GDCAP_AMASK) == 0) {
                    int w = GetGDCapability(g_background_dc, GDCAP_MAXX) + 1;
                    int h = GetGDCapability(g_background_dc, GDCAP_MAXY) + 1;
                    SetMemDCColorKey (g_background_dc, MEMDC_FLAG_SRCCOLORKEY, 
                            BKGND_DC_COLORKEY);
                    SetBrushColor(g_background_dc, BKGND_DC_COLORKEY);
                    FillBox(g_background_dc, 0, 0, w, h);
                }
                layout = &_this->m_controller->getLayout();
                if (layout->getHeight() > DESKTOP_TOOLBAR_HEIGHT) {
                    layout->setHeight(layout->getHeight() - DESKTOP_TOOLBAR_HEIGHT);
                    layout->reLayout();
                }
            }
            break;

        case MSG_KEYUP:
        case MSG_KEYDOWN:
            {
                PostMessage(hWnd, (MSG_KEYUP == message)?MSG_USER_GIVENKEY_UP:MSG_USER_GIVENKEY_DOWN,
                    wParam, lParam);
            }
            break;

        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
        case MSG_MOUSEMOVE:
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                SpeedMeterMessageHandler(0, message, x, y, 0);
                break;
            }
        case MSG_DESTROY:
            {
                ActivityDesktop* _this = 
                    dynamic_cast<ActivityDesktop*>(getActivityFromHWND(hWnd));
                assert(NULL != _this);
                delete _this;
                break;
            }
        case MSG_CLOSE:
#ifdef _MGRM_PROCESSES
            RegisterKeyHookWindow(HWND_NULL, 0);
#else
            RegisterKeyMsgHook(NULL, NULL);
#endif
            DestroyMainWindow(hWnd);
            break;

        case MSG_USER_IDLE:
            return 1;               // enable idle
    }

    return desktop_common_proc(hWnd, message, wParam, lParam);
}

HWND ActivityDesktop::createHWND() {
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_NONE;
//    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "ActivityDesktop";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DesktopProc;
    CreateInfo.lx = ACTIVITY_X;
    CreateInfo.ty = ACTIVITY_Y;
    CreateInfo.rx = CreateInfo.lx + ACTIVITY_W;
    CreateInfo.by = CreateInfo.ty + ACTIVITY_H;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = (DWORD)this;
    CreateInfo.hHosting = HWND_DESKTOP;

    return CreateMainWindow (&CreateInfo);
}

void ActivityDesktop::createToolbar(HWND parent)
{
    m_toolbar = new DesktopToolbar(parent);
//    ExcludeWindowExStyle(m_toolbar->hwnd(), WS_EX_TRANSPARENT);
    m_toolbar->addBtnEventListener(
            DesktopToolbar::TB_MENU, NCSN_ABP_CLICKED, evt_toolbar_menu);
    m_toolbar->addBtnEventListener(
            DesktopToolbar::TB_MSG, NCSN_ABP_CLICKED, evt_toolbar_sms);
    m_toolbar->addBtnEventListener(
            DesktopToolbar::TB_CALL, NCSN_ABP_CLICKED, evt_toolbar_call);
}

BOOL ActivityDesktop::evt_toolbar_menu(mObject* self, mObject *piece,
        int event_id, DWORD param)
{
    ActivityDesktop* _this = (ActivityDesktop*)Activity::getActivityFromHWND(
            GetMainWindowHandle(((mWidget*)self)->hwnd));

    if (event_id == NCSN_ABP_CLICKED) {
        if (! _this->m_hasBeenPoped) {
            TryStopShaking(self);
            ActivityStack::singleton()->pop();
            _this->m_hasBeenPoped = TRUE;
        }
        return TRUE;
    }

    return FALSE;
}
void ActivityDesktop::TryStopShaking(mObject* _self)
{
    mWidget* self = SAFE_CAST(mWidget, _self);
    assert(NULL != self);
    Activity* base = Activity::getActivityFromHWND(GetMainWindowHandle(self->hwnd));
    assert(NULL != base);
    ActivityDesktop* desktop = dynamic_cast<ActivityDesktop*>(base);
    assert(NULL != desktop);
    assert(NULL != desktop->m_controller);
    SignalEvent *event = 
        new SignalEvent(reinterpret_cast<const void*>(GetMainWindowHandle(self->hwnd)), SIGNAL_STOP_SHAKING);
    if (NULL != event) {
    #if 0
        desktop->m_controller->stateMachine().postEvent(event);
    #else
        desktop->m_controller->stateMachine().processEvent(event);
    #endif
    } else {
        assert(0);
    }
}

BOOL ActivityDesktop::evt_toolbar_call(mObject* _self, mObject *piece,
        int event_id, DWORD param)
{
    if (event_id == NCSN_ABP_CLICKED) {
        TryStopShaking(_self);
        ActivityStack::singleton()->switchTo("CallHistoryActivity",(Intent*)1);
        return TRUE;
    }

    return FALSE;
}

BOOL ActivityDesktop::evt_toolbar_sms(mObject* self, mObject *piece,
        int event_id, DWORD param)
{
    if (event_id == NCSN_ABP_CLICKED) {
        TryStopShaking(self);
        ActivityStack::singleton()->switchTo("SMSActivity");
        return TRUE;
    }

    return FALSE;
}

static void erase_view_background(HDC dst_dc, const RECT &rc, void * ctxt)
{
    BITMAP *pbmp = (BITMAP *)GetResource (Str2Key(DESKTOP_IMG_PATH));
    if (g_background_dc) {
        int w = GetGDCapability(dst_dc, GDCAP_MAXX) + 1;
        int h = GetGDCapability(dst_dc, GDCAP_MAXY) + 1;

        FillBoxWithBitmap (dst_dc, 0, 0, w, h, pbmp);
        // blit the app zoom animation layer.
        BitBlt(g_background_dc, 0, 0, -1, -1, dst_dc, 0, 0, 0);
    };
}

void ActivityDesktop::animationFinishedCb(MGEFF_ANIMATION animation)
{
    std::pair<HWND, HDC>*context = reinterpret_cast<std::pair<HWND, HDC>* >(mGEffAnimationGetContext(animation));
    if (NULL != context) {
        ShowWindow(context->first, SW_SHOWNORMAL);
        if (context->second) {
            DeleteMemDC (context->second);
        }
        delete context;
    }
}

void ActivityDesktop::onStateItemAround(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue)
{
    IPhoneDesktopItem *item = (IPhoneDesktopItem *)target;
    item->moveTo(*(POINT *)pValue);
}

void ActivityDesktop::onStateItemCenter(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue)
{
    IPhoneDesktopItem *item = (IPhoneDesktopItem *)target;
    item->setTransparency (*(int*)pValue);
}

void ActivityDesktop::onMoveToolBar(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue)
{
    HWND hwnd = (HWND)target;
    RECT rc;
    POINT* pt = (POINT*)pValue;
    GetWindowRect(hwnd, &rc);
    MoveWindow(hwnd, pt->x, pt->y, RECTW(rc), RECTH(rc), TRUE);
}

void ActivityDesktop::onStateApp(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue)
{
    static RECT old_rc;
    HDC hsdc = (HDC)id;
    RECT *prc = (RECT *)pValue;
    int w = GetGDCapability(g_background_dc, GDCAP_MAXX) + 1;
    int h = GetGDCapability(g_background_dc, GDCAP_MAXY) + 1;

    // TODO: don't use alpha.
    SetBrushColor(g_background_dc, BKGND_DC_COLORKEY);
    FillBox(g_background_dc, 0, 0, w, h);

    STRETCHBLT(hsdc, 0, 0, -1, -1, g_background_dc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), 0);
    old_rc = *prc;
}

void ActivityDesktop::onStatePageBar(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue)
{
    IPhoneDesktopLayout* layout = (IPhoneDesktopLayout*)id;
    layout->setPageBarCircleDistance(*(int*)pValue);
}

MGEFF_ANIMATION ActivityDesktop::pageBarAnimationCreate(void *target, int duration, BOOL zoomout)
{
    MGEFF_ANIMATION anim = NULL;
#ifdef HAVE_PAGESCROLL
    /* draw the pagebar information */
    int value_s, value_e;
    IPhoneDesktopLayout* layout = &m_controller->getLayout();
    anim = mGEffAnimationCreate(target, onStatePageBar, (intptr_t)layout, MGEFF_INT);
    if (zoomout) {
        value_s = PAGEBAR_DISTANCE;
        value_e = PAGEBAR_DISTANCE * 2;
    } else {
        value_s = PAGEBAR_DISTANCE * 2;
        value_e = PAGEBAR_DISTANCE;
    }
    mGEffAnimationSetStartValue(anim, &value_s);
    mGEffAnimationSetEndValue(anim, &value_e);
    mGEffAnimationSetDuration(anim, duration);
    mGEffAnimationSetCurve(anim, Linear);
#endif /* HAVE_PAGESCROLL */
    return anim;
}

void ActivityDesktop::setAnimation(Activity* activity, HWND dst_hwnd, int zoomout)
{
    IPhoneDesktopLayout* layout = NULL;
    HDC dst_hdc = activity->snapshot();
    assert(HDC_INVALID != dst_hdc);
    assert(HWND_INVALID != dst_hwnd);

    layout = &m_controller->getLayout();
    int page = layout->currentPage();

    //static int index[9] = {1, 2, 5, 8, 7, 6, 3, 0, 4};
    static int index[16] = {0, 1, 4, 5, 2, 3, 6, 7, 10, 11, 14, 15, 8, 9, 12, 13};
    static const double PI_DEVIDE_4 = 3.141592 / 4;
    int duration = 450;
    MGEFF_ANIMATION animation;
    m_animation = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    /* add animation for item around */
    int i, j, k = 0;
    double radians[] = {-PI_DEVIDE_4, PI_DEVIDE_4, PI_DEVIDE_4 * 3, PI_DEVIDE_4 * 5};
    POINT pt_1, pt_2;
    POINT *ppt_s, *ppt_e;
    RECT rc_layout = m_controller->getView().getRect();
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            k = i * 4 + j;
            if (zoomout) {
                // take all item reference of current page before them fly off because it can't take
                // when leave own position.
                m_items[k] = dynamic_cast<IPhoneDesktopItem *>(layout->index2Item(page, index[k]));
                ppt_s = &pt_1;
                ppt_e = &pt_2;
            } else {
                ppt_s = &pt_2;
                ppt_e = &pt_1;
            }

            /* continue if there is no item in this index*/
            if (!m_items[k])
                continue;

            layout->index2Point (pt_1, page, index[k]);
            pt_2.x = pt_1.x + RECTW(rc_layout) * sin(radians[i]);
            pt_2.y = pt_1.y - RECTH(rc_layout) * cos(radians[i]);

            animation = mGEffAnimationCreate(m_items[k], onStateItemAround, -1, MGEFF_POINT);

            mGEffAnimationSetStartValue(animation, ppt_s);
            mGEffAnimationSetEndValue(animation, ppt_e);
            mGEffAnimationSetDuration(animation, duration);
            mGEffAnimationSetCurve(animation, InCirc);
            mGEffAnimationAddToGroup(m_animation, animation);
        }
    }
#if 0
    /* add animation for item center */
    IPhoneDesktopItem *item = dynamic_cast<IPhoneDesktopItem *>(layout->index2Item(page, index[15]));
    if (item) {
        int alpha_1 = 255, alpha_2 = 0;
        int *palpha_s, *palpha_e;
        if (zoomout) {
            palpha_s = &alpha_1;
            palpha_e = &alpha_2;
        } else {
            palpha_s = &alpha_2;
            palpha_e = &alpha_1;
        }

        animation = mGEffAnimationCreate(item, onStateItemCenter, -1, MGEFF_INT);

        mGEffAnimationSetStartValue(animation, palpha_s);
        mGEffAnimationSetEndValue(animation, palpha_e);
        mGEffAnimationSetDuration(animation, duration);
        mGEffAnimationSetCurve(animation, InCirc);
        mGEffAnimationAddToGroup(m_animation, animation);
    }
#endif
    /* add animation for app */
    RECT rc_1;
    const RECT *prc_s, *prc_e;
    const RECT rc_2 = {0, 0, 
        RECTW(m_controller->getView().getRect()), 
        RECTH(m_controller->getView().getRect())};
    rc_1.left = rc_1.right = RECTW(rc_2) / 2;
    rc_1.top = rc_1.bottom = RECTH(rc_2) / 2;
    if (zoomout) {
        prc_s = &rc_1;
        prc_e = &rc_2;
    } else {
        prc_s = &rc_2;
        prc_e = &rc_1;
    }

    IPhoneDesktopView& view = m_controller->getView();

    /* load desktop bk image */

   // view.setEraseBackgroundCallback(erase_view_background, reinterpret_cast<void*>(&view));
    view.setEraseBackgroundCallback(erase_view_background, reinterpret_cast<void*>(&layout));

    animation = mGEffAnimationCreate((void *)layout->getHwnd(), onStateApp, (intptr_t)dst_hdc, MGEFF_RECT);
    mGEffAnimationSetStartValue(animation, prc_s);
    mGEffAnimationSetEndValue(animation, prc_e);
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationSetCurve(animation, InCirc);
    std::pair<HWND, HDC>* ctxt = new std::pair<HWND, HDC>(dst_hwnd, dst_hdc);
    mGEffAnimationSetContext(animation, reinterpret_cast<void*>(ctxt));
    mGEffAnimationSetFinishedCb(animation, animationFinishedCb);
    mGEffAnimationAddToGroup(m_animation, animation);

    POINT pt_s = {0, (zoomout ? ACTIVITY_H - DESKTOP_TOOLBAR_HEIGHT : ACTIVITY_H)};
    POINT pt_e = {0, (zoomout ? ACTIVITY_H : ACTIVITY_H - DESKTOP_TOOLBAR_HEIGHT)};
    animation = mGEffAnimationCreate((void *)m_toolbar->hwnd(), onMoveToolBar, 0, MGEFF_POINT);
    mGEffAnimationSetStartValue(animation, &pt_s);
    mGEffAnimationSetEndValue(animation, &pt_e);
    mGEffAnimationSetDuration(animation, (zoomout ? (duration >> 1) : (duration + 100)));
    mGEffAnimationSetCurve(animation, InQuad);
    mGEffAnimationAddToGroup(m_animation, animation);

//    MGEFF_ANIMATION anim1 = m_toolbar->getMoveAnimation(0, (zoomout ? DESKTOP_TOOLBAR_HEIGHT : 0), 
//            (zoomout ? (duration >> 1) : (duration + 200)), InQuad);
//    assert(anim1);
//    mGEffAnimationAddToGroup(m_animation, anim1);

    animation = mGEffAnimationCreate(&m_controller->getView(), IPhoneDesktopView::callRedraw, 0, MGEFF_INT);
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationAddToGroup(m_animation, animation);

    if ((animation = pageBarAnimationCreate((void*)g_background_dc, duration, zoomout)) != NULL)
        mGEffAnimationAddToGroup(m_animation, animation);

#if 0
    HWND hwnd = layout->getHwnd();
    mGEffAnimationAsyncRun(m_animation);
    mGEffAnimationSetProperty(m_animation, MGEFF_PROP_KEEPALIVE, 0);
    mGEffAnimationWait(&hwnd, m_animation);
#else
    UpdateWindow(TIMESERVICE->getHost(), FALSE);
    mGEffAnimationSyncRun(m_animation);
    mGEffAnimationDelete(m_animation);
#endif
    //view.setEraseBackgroundCallback(NULL, NULL);
}

int ActivityDesktop::onPause()
{
    Activity* next = ACTIVITYSTACK->top();
    ActivityDesktop::setAnimation(next, hwnd(), 1);

    SignalEvent *event = new SignalEvent((const void*)hwnd(), SIGNAL_APP_START);
    assert(NULL != m_controller);
    m_controller->stateMachine().postEvent(event);
#ifdef DEBUG
    printf("ActivityDesktop::onPause\n");
    fprintf(stderr, "%s:%s:%d, delete secondaryDC\n", __FILE__, __FUNCTION__, __LINE__);
#endif
    deleteHWNDSecondaryDC();
    return 0;
}

int ActivityDesktop::onResume()
{
#ifdef DEBUG
    printf("ActivityDesktop::onResume\n");
    fprintf(stderr, "%s:%s:%d, set secondaryDC\n", __FILE__, __FUNCTION__, __LINE__);
#endif
    setHWNDSecondaryDC(this->hwnd());

    SignalEvent *event = new SignalEvent((const void*)hwnd(), SIGNAL_APP_QUIT);
    assert(NULL != m_controller);
    m_controller->stateMachine().postEvent(event);

    Activity* prev = ACTIVITYSTACK->top();
    ShowWindow(hwnd(), SW_SHOW);
    ShowWindow(prev->hwnd(), SW_HIDE);
    ActivityDesktop::setAnimation(prev, hwnd(), 0);

    return 0;
}

int ActivityDesktop::onStart()
{
    m_needSwitchEffect = true;
    IPhoneDesktopView& view = m_controller->getView();

    Load32Resource (DESKTOP_IMG_PATH, RES_TYPE_IMAGE, 0);
    view.setEraseBackgroundCallback(erase_view_background, NULL);
//    Activity* prev = ACTIVITYSTACK->prev();
//    ShowWindow(hwnd(), SW_SHOW);
//    ActivityDesktop::setAnimation(prev, prev->hwnd(), 0);
    return 0;
}

int ActivityDesktop::onStop()
{
    m_needSwitchEffect = true;
    ReleaseRes(Str2Key(DESKTOP_IMG_PATH));
//    Activity* prev = ACTIVITYSTACK->prev();
//    ShowWindow(hwnd(), SW_SHOW);
//    ActivityDesktop::setAnimation(prev, hwnd(), 1);
    return 0;
}

void ActivityDesktop::show()
{
    Activity::show();
    m_needSwitchEffect = false;
}

