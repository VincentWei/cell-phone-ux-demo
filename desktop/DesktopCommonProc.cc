/*! ============================================================================
 * @file desktop_common_proc.c 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include <mgeff/mgeff.h>
#include "global.h"
#include "DesktopCommonProc.hh"
#include "ActivityStack.hh"
#include "IPhoneDesktopController.hh"

HWND g_desktop_hwnd = HWND_INVALID;

int common_key_hook(void* context, HWND dst_wnd, 
                int msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case MSG_KEYUP:
        case MSG_KEYDOWN:
            {
                PostMessage(g_desktop_hwnd, 
                        (MSG_KEYUP == msg ? MSG_USER_GIVENKEY_UP : 
                        MSG_USER_GIVENKEY_DOWN), wparam, lparam);
            }
            break;
    }
    return HOOK_GOON;
}

void transition_clicked(void *object, mStateMachine::Transition* _transition, const Event* event, void *param) 
{
    const Event::EventParameters& params = event->parameters();
    if (!params.empty() && params.back().type() == typeid(std::string)) {
        ActivityStack::singleton()->push(boost::any_cast<std::string>(params.back()).c_str());
    }
}

int desktop_common_proc(HWND hwnd, int msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case MSG_USER_GIVENKEY_UP:
        case MSG_USER_GIVENKEY_DOWN:
            {
                int scancode = (int)wparam;
                if (SCANCODE_ESCAPE == scancode && 
                        MSG_USER_GIVENKEY_UP == msg) {
                    ACTIVITYSTACK->home();
                } else if (SCANCODE_BACKSPACE == scancode && 
                        MSG_USER_GIVENKEY_UP == msg) {
                    if (NULL != ACTIVITYSTACK->top()
                        // forbidden back to standby screen via press backspace.
                        && (strcmp(ACTIVITYSTACK->currentAppName(), "ActivityDesktop") != 0)) {
                        ACTIVITYSTACK->back();
                    }
                }
            }
            break;
    }

    return DefaultMainWinProc(hwnd, msg, wparam, lparam);
}

