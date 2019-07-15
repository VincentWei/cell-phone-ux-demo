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
#include "sample-activity.hh"
#include <minigui/control.h>

REGISTER_ACTIVITY(SampleActivity);

SampleActivity::SampleActivity() {
    m_style = STYLE_PUSH;
}

SampleActivity::~SampleActivity() {
}

LRESULT SampleActivity::proc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam) {
    switch (message) {
        case MSG_CREATE:
            {
                CreateWindow(CTRL_STATIC, "This is title", WS_VISIBLE | SS_CENTER, 100,
                        0, 0, ACTIVITY_W, 20, hWnd, 0);
                CreateWindow(CTRL_STATIC, "This is body", WS_VISIBLE | SS_CENTER | SS_BLACKRECT, 101,
                        0, 20, ACTIVITY_W, ACTIVITY_H - 20 - 20, hWnd, 0);
                CreateWindow(CTRL_STATIC, "This is button", WS_VISIBLE | SS_CENTER, 102,
                        0, ACTIVITY_H - 20, ACTIVITY_W, 20, hWnd, 0);
            }
            break;
        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            return 0;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

HWND SampleActivity::createHWND() {
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "sample-activity";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = SampleActivity::proc;
    CreateInfo.lx =  ACTIVITY_X;
    CreateInfo.ty = ACTIVITY_Y;
    CreateInfo.rx = ACTIVITY_X + ACTIVITY_W;
    CreateInfo.by = ACTIVITY_Y + ACTIVITY_H;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    return CreateMainWindow(&CreateInfo);
}
