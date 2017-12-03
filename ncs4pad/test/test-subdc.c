#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

static void fill_subdc(HDC windc) {
    HDC memdc, subdc;
    memdc = CreateCompatibleDCEx(windc, 50, 50);

    //subdc = CreateSubMemDC(memdc, -50, -50, 150, 150, TRUE);
    subdc = GetSubDC(memdc, -50, -50, 150, 150);

    /* outer */
    SetBrushColor(subdc, RGB2Pixel(subdc, 0xff, 0, 0));
    FillBox(subdc, 0, 0, 150, 150);

    /* inner */
    SetBrushColor(subdc, RGB2Pixel(subdc, 0, 0xff, 0));
    FillBox(subdc, 60, 60, 10, 10);

    BitBlt(memdc, 0, 0, 0, 0, windc, 10, 10, -1);

    ReleaseDC(subdc);
    DeleteMemDC(memdc);
}

static void read_subdc(HDC windc) {
    HDC memdc, subdc;
    memdc = CreateCompatibleDCEx(windc, 50, 50);

    // subdc = CreateSubMemDC(memdc, -50, -50, 150, 150, TRUE);
    subdc = GetSubDC(memdc, -50, -50, 150, 150);

    /* outer */
    SetBrushColor(subdc, RGB2Pixel(subdc, 0xff, 0, 0));
    FillBox(subdc, 0, 0, 150, 150);

    /* inner */
    SetBrushColor(subdc, RGB2Pixel(subdc, 0, 0xff, 0));
    FillBox(subdc, 60, 60, 10, 10);

    BitBlt(subdc, 0, 0, 0, 0, windc, 10, 10, -1);

    ReleaseDC(subdc);
    DeleteMemDC(memdc);
}

static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_ERASEBKGND:
            return 0;
        case MSG_KEYDOWN:
            {
                int scancode = (int)wParam;
                HDC windc = GetClientDC(hWnd);

                switch (scancode) {
                    case SCANCODE_1:
                        fill_subdc(windc);
                        break;
                    case SCANCODE_2:
                        read_subdc(windc);
                        break;
                    case SCANCODE_0:
                        InvalidateRect(hWnd, NULL, FALSE);
                        break;
                    default:
                        break;
                }
                ReleaseDC(windc);
            }
            break;
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
