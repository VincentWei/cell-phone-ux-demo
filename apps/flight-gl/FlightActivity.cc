#include "FlightActivity.hh"
#include <minigui/control.h>

REGISTER_ACTIVITY(FlightActivity);

#ifdef __cplusplus
extern "C" {
#endif
extern void letmefly_init(void);
extern void letmefly_deinit(void);
extern void letmefly_draw(void);
extern void letmefly_run(void);
extern void letmefly_reshape(int w, int h);
extern void letmefly_rolling(float d_angle);
extern void letmefly_sideslip(float d_angle);
#ifdef __cplusplus
}
#endif

FlightActivity::FlightActivity() {
    m_style = STYLE_PUSH;
}

FlightActivity::~FlightActivity() {
}

int FlightActivity::proc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam) {
    FlightActivity *act =
        (FlightActivity*)Activity::getActivityFromHWND(GetMainWindowHandle(hWnd));

    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_PAINT:
            letmefly_draw();
            mgEGLSwapBuffers( act->display(), act->surface());
            break;
        case MSG_ERASEBKGND:
                return 0;
        case MSG_TIMER:
            InvalidateRect(hWnd, NULL, TRUE);
            ResetTimer(hWnd, 1, 1);
            return 0;
        case MSG_LBUTTONDOWN:
            if (LOSWORD(lParam) > ACTIVITY_W / 2) {
                if (LOSWORD(lParam) < ACTIVITY_W / 4 * 3)
                    letmefly_sideslip(-0.5);
                else
                    letmefly_rolling(2.0);
            } else {
                if (LOSWORD(lParam) > ACTIVITY_W / 4)
                    letmefly_sideslip(0.5);
                else
                    letmefly_rolling(-2.0);
            }
            return 0;
        case MSG_LBUTTONUP:
            letmefly_rolling(0.0);
            letmefly_sideslip(0.0);
            return 0;
        case MSG_MOUSEMOVE:
            return 0;
        case MSG_CLOSE:
            letmefly_deinit();
            act->EGLRelease();
            DestroyMainWindow(hWnd);
            return 0;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

HWND FlightActivity::createHWND() {
    HWND hWnd;
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "flight-gl-activity";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = FlightActivity::proc;
    CreateInfo.lx =  ACTIVITY_X;
    CreateInfo.ty = ACTIVITY_Y;
    CreateInfo.rx = ACTIVITY_X + ACTIVITY_W;
    CreateInfo.by = ACTIVITY_Y + ACTIVITY_H;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hWnd = CreateMainWindow(&CreateInfo);

    EGLInit(hWnd);
    letmefly_init();
    letmefly_reshape(ACTIVITY_W, ACTIVITY_H);
    letmefly_run();
    SetTimer(hWnd, 1, 1);

    return hWnd;
}
