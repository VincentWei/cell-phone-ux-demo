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
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <tslib.h>

static void ts_test(void) {
    const char *device;
    struct tsdev *ts;
    int fd;
    int ret;

    device = getenv("TSLIB_TSDEVICE");
    assert(device);

    ts = ts_open(device, 0);
    assert(ts);

    ret = ts_config(ts);
    assert(ret == 0);

    for (;;) {
        struct ts_sample samp;
        if (rand() % 10) {
            usleep(1000 * 10);
        }
        ret = ts_read(ts, &samp, 1);
        if (ret != 1) {
            printf("ts_read()=%d\n", ret);
        }else{
            printf("x,y=%d,%d pressure=%d\n", samp.x, samp.y, samp.pressure);
        }
    }

    ts_close(ts);
}

static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {
        case MSG_CREATE:
            SetTimer(hWnd, 100, 100);
            break;
        case MSG_TIMER:
            KillTimer(hWnd, 100);
            ts_test();
            break;
        case MSG_ERASEBKGND:
            return 0;
        case MSG_PAINT:
            {
                HDC hdc;
                hdc = BeginPaint (hWnd);
                SetBrushColor(hdc, RGB2Pixel(hdc, 0xff, 0xff, 0xff));
                FillBox(hdc, 0, 0, 1000, 1000);
                EndPaint (hWnd, hdc);
            }
            return 0;
        case MSG_CLOSE:
            KillTimer (hWnd, 100);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    srand(time(NULL));

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , argv[0] , 0 , 0);
#endif

    CreateInfo.dwStyle = 
        WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
    CreateInfo.spCaption = argv[0];
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = HelloWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
