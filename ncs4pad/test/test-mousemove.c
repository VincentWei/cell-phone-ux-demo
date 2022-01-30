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
/* 
** $Id: helloworld.c 793 2010-07-28 03:36:29Z dongjunjie $
**
** Listing 1.1
**
** helloworld.c: Sample program for MiniGUI Programming Guide
**      The first MiniGUI application.
**
** Copyright (C) 2004 ~ 2009 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mspeedmeter.h"

HDC g_memdc = HDC_INVALID;
int x_last = -1;
int y_last = -1;
int g_pressed = 0;

static int deal_with_noise(HWND hwnd, int *x, int *y) {
    int x1, y1;
    float vx, vy;

    QueryMouseMoveVelocity(&vx, &vy);

    if (ABS(vx) > ABS(vy)) {
        x1 = *x;
        y1 = (*x - x_last) / vx * vy + y_last;
    }else{
        y1 = *y;
        x1 = (*y - y_last) / vy * vx + x_last;
    }

    if (ABS(*x - x1) + ABS(*y - y1) >= 20) {
        SetBrushColor(g_memdc, RGB2Pixel(g_memdc, 0, 0xff, 0));
        FillCircle(g_memdc, x1, y1, 3);

        SetBrushColor(g_memdc, RGB2Pixel(g_memdc, 0xff, 0, 0));
        FillCircle(g_memdc, *x, *y, 3);

        InvalidateRect(hwnd, NULL, FALSE);
        return -1;
    }else{
        return 0;
    }
}

static LRESULT HelloWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_RBUTTONDOWN:
            {
                SetBrushColor(g_memdc, 0xFFFFFFFF);
                FillBox(g_memdc, 0, 0, 1000, 1000);
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        case MSG_LBUTTONDOWN:
            g_pressed = 1;
            SpeedMeterProc(hWnd, message, wParam, lParam);
            x_last = LOSWORD (lParam);
            y_last = HISWORD (lParam);
            MoveTo(g_memdc, x_last, y_last);
            break;
        case MSG_LBUTTONUP:
            g_pressed = 0;
            SpeedMeterProc(hWnd, message, wParam, lParam);
            break;
        case MSG_MOUSEMOVE:
            if (g_pressed) {
                int x_pos = LOSWORD (lParam);
                int y_pos = HISWORD (lParam);

                /* Make noise */
                {
                    int r = rand();
                    if ((r % 40) == 0) {
                        r = rand();
                        int L = 100 - (r % 200);
                        int x_ratio = (r % 100);
                        x_pos += L * x_ratio / 100;
                        y_pos += L * (100 - x_ratio) / 100;
                    }
                }

                /* deal with noise */
                if (deal_with_noise(hWnd, &x_pos, &y_pos) == 0) {
                    SetBrushColor(g_memdc, RGB2Pixel(g_memdc, 0, 0, 0xff));
                    FillCircle(g_memdc, x_pos, y_pos, 2);

                    LineTo(g_memdc, x_pos, y_pos);
                    InvalidateRect(hWnd, NULL, FALSE);

                    x_last = x_pos;
                    y_last = y_pos;

                    lParam = MAKELONG(x_pos, y_pos);
                    SpeedMeterProc(hWnd, message, wParam, lParam);
                }
            }
            break;
        case MSG_ERASEBKGND:
            return 0;
        case MSG_PAINT:
            {
                HDC hdc;
                hdc = BeginPaint (hWnd);
                if (g_memdc == HDC_INVALID) {
                    g_memdc = CreateCompatibleDC(hdc);
                    SetBrushColor(g_memdc, RGB2Pixel(g_memdc, 0xff, 0xff, 0xff));
                    FillBox(g_memdc, 0, 0, 1000, 1000);
                    SetPenColor(g_memdc, RGB2Pixel(g_memdc, 0, 0, 0));
                }
                BitBlt(g_memdc, 0, 0, 0, 0, hdc, 0, 0, 0);
                if (g_pressed) {
                    float vx, vy;
                    char s[128];
                    if (QueryMouseMoveVelocity(&vx, &vy) == 0) {
                        sprintf(s, "x: %-6.0f", vx);
                        TextOut(hdc, 50, 50, s);
                        sprintf(s, "y: %-6.0f", vy);
                        TextOut(hdc, 50, 80, s);
                    }
                }
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
