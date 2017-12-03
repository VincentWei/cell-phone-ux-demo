/*!============================================================================
 * @file DesktopCommonProc.hh
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  DESKTOP_COMMON_PROC_INC
#define  DESKTOP_COMMON_PROC_INC

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "Activity.hh"
#include "StateMachine.hh"

#define SIGNAL_APP_QUIT "ApplicationQuit"
#define SIGNAL_APP_START "ApplicationStart"
#define SIGNAL_STOP_SHAKING "StopShaking"

extern HWND g_desktop_hwnd;

int desktop_common_proc(HWND hwnd, int msg, WPARAM wparam, LPARAM lparam);
void transition_home_key(void *object, mStateMachine::Transition *_transition, const Event* event, void *param) ;
void transition_back_key(void *object, mStateMachine::Transition *_transition, const Event* event, void *param) ;
void transition_clicked(void *object, mStateMachine::Transition* _transition, const Event* event, void *param);
int common_key_hook(void* context, HWND dst_wnd, 
                int msg, WPARAM wparam, LPARAM lparam);

#endif   /* ----- #ifndef DESKTOP_COMMON_PROC_INC  ----- */

