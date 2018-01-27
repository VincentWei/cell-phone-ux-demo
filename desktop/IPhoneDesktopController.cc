#include <stdio.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <algorithm>
#include <sstream>
#include <cassert>

#include "IPhoneDesktopController.hh"
#include "IPhoneDesktopItem.hh"
#include "IPhoneDesktopStateNormal.hh"
#include "IPhoneDesktopStateApp.hh"
#include "IPhoneDesktopStateShifting.hh"
#include "IPhoneDesktopStateShaking.hh"
#include "TransitionClickOnItem.hh"
#include "TransitionStandStillOnItem.hh"
#include "TransitionNoInput.hh"
#include "TransitionSpeedMeasure.hh"
#include "utility.h"
#include "DesktopCommonProc.hh"
#include "global.h"
#include "ContentResolver.hh"
#include "AppsInfoProvider.hh"

#define XPATH_ICON_PREFIX "/cellphone/desktop/application["
#define XPATH_ICON_NAME_POSTFIX "]/name"
#define XPATH_ICON_SIGN_POSTFIX "]/signature"
#define XPATH_ICON_PATH_POSTFIX "]/icon"
#define VALUE_LEN 128
#define CONFIG_FILE "etc/cellphone.xml"

static const int MSG_USER_CREATE = MSG_USER + 1;
static const int TIMER_ID = 101;

IPhoneDesktopController::IPhoneDesktopController(HWND hwnd)
    : m_view(&m_scene, hwnd), m_layout(&m_scene, hwnd), m_stateMachine(hwnd), m_buttonPressed(false), m_hwnd(hwnd)
{
    m_mousePosition.x = m_mousePosition.y = 0;

    SetWindowAdditionalData2(hwnd, (DWORD) this);
    SetWindowCallbackProc(hwnd, wndProcWrapper);
    SendMessage(hwnd, MSG_USER_CREATE, 0, 0);

    createStates();
}

IPhoneDesktopController::~IPhoneDesktopController()
{
    printf ("IPhoneDesktopController::~IPhoneDesktopController\n");
}

int IPhoneDesktopController::create(HWND hwnd) {
    IPhoneDesktopController *controller;
    controller = new IPhoneDesktopController(hwnd);
    if (! controller) {
        return -1;
    }
    return 0;
}

void IPhoneDesktopController::createStates() {
    EventTransition *trans;
    SignalTransition* sigTrans = NULL;
    states[S1_Normal]   = new IPhoneDesktopStateNormal(this, &m_stateMachine, "Normal");
    states[S2_App]      = new IPhoneDesktopStateApp(this, &m_stateMachine, "App");
#ifdef HAVE_PAGESCROLL
    states[S4_Shifting] = new IPhoneDesktopStateShifting(this, &m_stateMachine, "Shifting");
#endif /* HAVE_PAGESCROLL */
    states[S5_Shaking]  = new IPhoneDesktopStateShaking(this, &m_stateMachine, "Shaking");

    ((IPhoneDesktopStateShaking*)states[S5_Shaking])->createChildStates();

//    trans = new TransitionClickOnItem(states[S1_Normal], states[S2_App], this, "1-2");
    trans = new TransitionClickOnItem(states[S1_Normal], NULL, this, "Click targetless");
    trans->subscribe(states[S1_Normal], transition_clicked, NULL);

#if 0
    trans = new EventTransition(Event::KeyEvent, states[S2_App], NULL, "homekey targetless" );
    trans->subscribe(states[S2_App], transition_home_key, NULL);
    trans->setKeyType(KeyEvent::KeyReleased);
    trans->setKey(SCANCODE_ESCAPE);

    trans = new EventTransition(Event::KeyEvent, states[S2_App], NULL, "backkey targetless" );
    trans->subscribe(states[S2_App], transition_back_key, NULL);
    trans->setKeyType(KeyEvent::KeyReleased);
    trans->setKey(SCANCODE_BACKSPACE);
#endif
   
    sigTrans = new SignalTransition(states[S2_App], states[S1_Normal], "2-1");
    sigTrans->setSignal(reinterpret_cast<void*>(g_desktop_hwnd), SIGNAL_APP_QUIT);

    sigTrans = new SignalTransition(states[S1_Normal], states[S2_App], "1-2");
    sigTrans->setSignal(reinterpret_cast<void*>(g_desktop_hwnd), SIGNAL_APP_START);

    trans = new TransitionStandStillOnItem(states[S1_Normal], states[S5_Shaking], this, 1000, "1-5");

    sigTrans = new SignalTransition(states[S5_Shaking], states[S1_Normal], "5-1");
    sigTrans->setSignal(reinterpret_cast<void*>(g_desktop_hwnd), SIGNAL_STOP_SHAKING);

    trans = new EventTransition(Event::KeyEvent, states[S5_Shaking], states[S1_Normal], "5-1");
    trans->setKeyType(KeyEvent::KeyReleased);
    trans->setKey(SCANCODE_ESCAPE);

    /* for test state Shifting */
#ifdef HAVE_PAGESCROLL
    ((IPhoneDesktopStateShifting*)states[S4_Shifting])->createChildStates(states[S1_Normal], states[S2_App]);

    trans = new EventTransition(Event::MoveEvent, 
            states[S1_Normal], states[S4_Shifting], "EventTransition::S1_Normal->S4_Shifting");
#endif /* HAVE_PAGESCROLL */
    //trans = new EventTransition(Event::MouseEvent, states[S4_Shifting], states[S1_Normal], "EventTransition::S4_Shifting->S1_Normal");
    //trans->setButtonType(MouseEvent::ButtonReleased);
    //trans->setButtons(MouseEvent::LeftButton);

    m_stateMachine.setInitialState(states[S1_Normal]);
    m_stateMachine.start();
}

IPhoneDesktopController *IPhoneDesktopController::getControllerFromHwnd(HWND hwnd) {
    return (IPhoneDesktopController *)GetWindowAdditionalData2(hwnd);
}

LRESULT IPhoneDesktopController::wndProcWrapper(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    IPhoneDesktopController *controller = getControllerFromHwnd(hwnd);
    return controller->wndProc(hwnd, message, wParam, lParam);
}

Scene &IPhoneDesktopController::getScene() {
    return m_scene;
}

IPhoneDesktopLayout &IPhoneDesktopController::getLayout() {
    return m_layout;
}

IPhoneDesktopView &IPhoneDesktopController::getView() {
    return m_view;
}

void IPhoneDesktopController::createItems() {
    std::ostringstream xpath;
    std::string iconPath, sign, name;
    IPhoneDesktopItem *item = NULL;

    ContentCursor* cur = GET_CONTENT_RESOLVER()->query(AppsInfoProvider::CONTENT_URI,
            NULL, NULL, NULL, NULL);
    if (NULL != cur) {
        ContentValues::Strings names;
        ActivityFactory::singleton()->queryNames(names);

        for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
            name = cur->getString(AppsInfoProvider::COL_APP_ACTIVITY);
            if (name.length() > 0 && std::find(names.begin(), names.end(), name) != names.end()) {
                sign = cur->getString(AppsInfoProvider::COL_APP_NAME);
                iconPath = cur->getString(AppsInfoProvider::COL_APP_ICON);
                item = new IPhoneDesktopItem(name, sign.empty() ? "NoName" : sign, iconPath);
                m_scene.addItem(item);
            }
        }
        delete cur;
    }
    m_layout.reLayout();
}

int IPhoneDesktopController::wndProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_USER_CREATE:
            SetTimer(hwnd, TIMER_ID, 5);
            createItems();
            return 0;
        case MSG_ERASEBKGND:
            /* XXX: do nothing */
            return 0;
        case MSG_PAINT:
            {
                HDC dc = BeginPaint(hwnd);
                /* TODO: rc */
                m_view.onDraw(dc);
                EndPaint(hwnd, dc);
                {
                    Event *event = 0;
                    event = new TimerEvent((int)TIMER_ID, (unsigned int)lParam);
                    if (event) {
                        m_stateMachine.postEvent(event);
                    }
                }
            }
            return 0;
        case MSG_USER_STATEMACHINE_EVENT:
            m_stateMachine.onEvent();
            return 0;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
        case MSG_MOUSEMOVE:
        case MSG_MOUSEMOVEIN:
        case MSG_KEYDOWN:
        case MSG_KEYUP:
        case MSG_TIMER:
            {
                Event *event = 0;
                switch (message) {
                    case MSG_TIMER:
                        //printf("MSG_TIMER:%d\n", wParam);
                        event = new TimerEvent((int)wParam, (unsigned int)lParam);
                        break;
                    case MSG_LBUTTONDOWN:
                    case MSG_LBUTTONUP:
                        m_buttonPressed = (message == MSG_LBUTTONDOWN);
                        if (m_buttonPressed) {
                            m_mousePosition.x = LOSWORD(lParam);
                            m_mousePosition.y = HISWORD(lParam);
                        }
                        event = new MouseEvent(message == MSG_LBUTTONDOWN ? MouseEvent::ButtonPressed : MouseEvent::ButtonReleased, LOSWORD(lParam), HISWORD(lParam), MouseEvent::LeftButton);
                        break;
                    case MSG_MOUSEMOVE:
                        if (m_buttonPressed) {
                            POINT pos;
                            pos.x = LOSWORD(lParam);
                            pos.y = HISWORD(lParam);
            //               if (m_mousePosition.x != pos.x || m_mousePosition.y != pos.y) {
                                event = new MoveEvent(m_mousePosition, pos);
                                m_mousePosition = pos;
            //               }
                        }
                        break;
                    case MSG_MOUSEMOVEIN:
                        if (!wParam && m_buttonPressed) {
                            //puts("MSG_MOUSEMOVEOUT");
                            SendMessage(hwnd, MSG_LBUTTONUP, wParam, lParam);
                        }
                        break;
                    case MSG_KEYDOWN:
                    case MSG_KEYUP:
                        event = new KeyEvent(message == MSG_KEYDOWN ? KeyEvent::KeyPressed : KeyEvent::KeyReleased, (int)wParam);
                        break;
                    default:
                        assert(0);
                        break;
                }
                if (event) {
                    m_stateMachine.postEvent(event);
                }
            }
            break;
        case MSG_DESTROY:
            {
                delete this;
                break;
            }
        default:
            break;
    }

    return DefaultWindowProc(hwnd, message, wParam, lParam);
}
