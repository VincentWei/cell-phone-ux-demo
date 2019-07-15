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
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mfillboxex.h"

int gauss_blur(HDC hdc, unsigned char *data, int width, int height, int pitch, double sigma, int radius) {   
    double *gaussMatrix, gaussSum = 0.0, _2sigma2 = 2 * sigma * sigma;   
    int x, y, xx, yy, xxx, yyy;   
    double *pdbl, a, r, g, b, d;   
    unsigned int *pout, *poutb, bbb;

    pout = poutb = (unsigned int *)malloc(width * height * sizeof(pout[0]));   
    if (!pout){
        return -1;   
    }

    gaussMatrix = pdbl = (double *)malloc((radius * 2 + 1) * (radius * 2 + 1) * sizeof(pdbl[0]));   
    if (!gaussMatrix) {   
        free(pout);
        return -1;   
    }   

    for (y = -radius; y <= radius; y++) {   
        for (x = -radius; x <= radius; x++) {   
            a = exp(-(double)(x * x + y * y) / _2sigma2);   
            *pdbl++ = a;   
            gaussSum += a;   
        }   
    }   

    pdbl = gaussMatrix;   
    for (y = -radius; y <= radius; y++) {   
        for (x = -radius; x <= radius; x++) {   
            *pdbl++ /= gaussSum;   
        }   
    }   
    for (y = 0; y < height; y++) {   
        for (x = 0; x < width; x++) {   
            a = r = g = b = 0.0;   
            pdbl = gaussMatrix;   
            for (yy = -radius; yy <= radius; yy++) {   
                yyy = y + yy;   
                if (yyy >= 0 && yyy < height) {   
                    for (xx = -radius; xx <= radius; xx++) {   
                        xxx = x + xx;   
                        if (xxx >= 0 && xxx < width) {   
                            bbb = *((unsigned int *)&data[xxx*4 + yyy * pitch]);
                            // printf("x,y=%d,%d pitch=%d 0x%08x\n", xxx, yyy, pitch, bbb);
                            d = *pdbl;   
                            a += d * GetAValue(bbb);   
                            r += d * GetRValue(bbb);   
                            g += d * GetGValue(bbb);   
                            b += d * GetBValue(bbb);   
                        }   
                        pdbl++;   
                    }   
                } else {   
                    pdbl += (radius * 2 + 1);   
                }   
            }   
            *pout++ = MakeRGBA((int)r, (int)g, (int)b, (int)a);
            // printf("r,g,b,a=%f,%f,%f,%f ", r, g, b, a);
            if (x==0 && y==0) {
                printf("r,g,b,a=%d,%d,%d,%d ", (int)r, (int)g, (int)b, (int)a);
            }
        }   
    }   
    // memcpy(data, pout, height * pitch);
    for (y=0; y<height; ++y) {
        for (x=0; x<width; ++x) {
            unsigned int *pdst = (unsigned int *)&data[x*4 + y*pitch];
            int r, g, b, a;
            r = GetRValue(poutb[x + y * width]);
            g = GetGValue(poutb[x + y * width]);
            b = GetBValue(poutb[x + y * width]);
            a = GetAValue(poutb[x + y * width]);
            *pdst = RGBA2Pixel(hdc, r, g, b, a);
            // printf("dst=0x%08x ", *pdst);
        }
    }
    free(gaussMatrix);   
    free(poutb);
    return 0;   
}  

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

                        BitBlt(memdc, 0, 0, 0, 0, hdc, 0, 0, -1);
                    }

                    {
                        RECT rc = {0, 0, 300, 300};
                        int width;
                        int height;
                        int pitch;
                        Uint8 *data;

                        SelectClipRect(memdc, &rc);

                        data = LockDC(memdc, &rc, &width, &height, &pitch);
                        gauss_blur(hdc, data, width, height, pitch, 1.0, 1.0);
                        UnlockDC(memdc);

                        BitBlt(memdc, 0, 0, 0, 0, hdc, 300, 0, -1);

                        SetBrushColor(memdc, RGBA2Pixel(memdc, 0xff, 0, 0, 0xff));
                        FillBox(memdc, 0, 0, 300, 300);
                        BitBlt(memdc, 0, 0, 0, 0, hdc, 0, 300, -1);
                    }

                    EmptyClipRgn (&my_cliprgn1);
                    EmptyClipRgn (&my_cliprgn2);

                    DestroyFreeClipRectList (&my_cliprc_heap);
                }

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
