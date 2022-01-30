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

#include "mfillboxex.h"

static LRESULT HelloWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_CREATE:
            break;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            {
                HDC memdc = CreateCompatibleDCEx(hdc, 300, 300);

                {
                    CLIPRGN my_cliprgn1;
                    CLIPRGN my_cliprgn2;
                    BLOCKHEAP my_cliprc_heap;

                    InitFreeClipRectList (&my_cliprc_heap, 100);

                    InitClipRgn (&my_cliprgn1, &my_cliprc_heap);
                    InitClipRgn (&my_cliprgn2, &my_cliprc_heap);

                    InitCircleRegion (&my_cliprgn1, 100, 100, 60);
                    InitEllipseRegion (&my_cliprgn2, 100, 100, 50, 70);

                    SetBrushColor (memdc, PIXEL_blue);
                    FillBox (memdc, 0, 0, 300, 300);

                    SubtractRegion (&my_cliprgn1, &my_cliprgn1, &my_cliprgn2);
                    SelectClipRegion (memdc, &my_cliprgn1);

                    mFillRegion(memdc, &my_cliprgn1, fillspan_simple, (void *)MakeRGBA(0, 0xff, 0, 0xff));

                    {
                        struct gradient_context context;
                        context.top = 100;
                        context.bottom = 120; 
                        context.rgba[0] = MakeRGBA(0, 0xff, 0, 0xff);
                        context.rgba[1] = MakeRGBA(0, 0, 0xff, 0xff);
                        mFillRegion(memdc, &my_cliprgn1, fillspan_gradient, &context);
                    }


                    {
                        struct multigradient_context context;
                        context.top = 120;
                        context.bottom = 150; 
                        context.n = 3;
                        context.colortable = (gradient_t *)malloc(sizeof(context.colortable[0]) * context.n);
                        context.colortable[0][0] = 0;
                        context.colortable[0][1] = MakeRGBA(0, 0xff, 0, 0xff);
                        context.colortable[1][0] = 30;
                        context.colortable[1][1] = MakeRGBA(0, 0, 0xff, 0xff);
                        context.colortable[2][0] = 100;
                        context.colortable[2][1] = MakeRGBA(0xff, 0, 0, 0xff);
                        mFillRegion(memdc, &my_cliprgn1, fillspan_multigradient, &context);
                        free(context.colortable);
                    }

                    EmptyClipRgn (&my_cliprgn1);
                    EmptyClipRgn (&my_cliprgn2);

                    DestroyFreeClipRectList (&my_cliprc_heap);
                }

                BitBlt(memdc, 0, 0, 0, 0, hdc, 50, 50, 0);
                DeleteMemDC(memdc);
            }
            EndPaint (hWnd, hdc);
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

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , argv[0] , 0 , 0);
#endif

    CreateInfo.dwStyle = 
        WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
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
