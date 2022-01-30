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
#include <sys/times.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

int g_w = 360, g_h = 480;
int g_hw = 1;
int g_time = 5000;
int g_format = 0;
int g_to_screen = 1;

unsigned int format_table[][5] = {
    /* RGB565 */
    {16, 0xF800, 0x07E0, 0x001F, 0x00000000},
    /* ARGB8888 */
    {32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000},
    /* XRGB8888 */
    {32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000},
};

const char *format_name[] = {
    "RGB565", "ARGB8888", "XRGB8888"
};

static void do_test(HWND hWnd){
    HDC memdc;
    int n;
    clock_t start, end;
    clock_t t1, t2;

    printf(
            "* %d-bit %s %s, %s, %dx%d, %.1fs *\n",
            format_table[g_format][0],
            format_name[g_format],
            g_hw ? "Hard" : "Soft",
            g_to_screen ? "to Screen" : "Overlapped Blit",
            g_w, g_h,
            g_time / 1000.0f);

    g_time /= 10;

    memdc = CreateMemDC(g_w, g_h, format_table[g_format][0], g_hw ? MEMDC_FLAG_HWSURFACE : MEMDC_FLAG_SWSURFACE,
            format_table[g_format][1],
            format_table[g_format][2],
            format_table[g_format][3],
            format_table[g_format][4]);

    SetBrushColor(memdc, RGB2Pixel(memdc, 0xff, 0, 0));
    FillBox(memdc, 0, 0, 1000, 1000);

    for (n=0, start=times(NULL);;++n) {
        t1 = times(NULL);
        if (g_to_screen) {
            BitBlt(memdc, 0, 0, 0, 0, HDC_SCREEN, 50, 0, 0);
        }else{
            BitBlt(memdc, 0, 0, 0, 0, memdc, 0, 5, 0);
        }
        t2 = times(NULL);
        if (t2-t1 >= 10) {
            printf("[%d] %ld\n", n, t2-t1);
        }
        if ((n & 0xf) == 0) {
            end = times(NULL);
            if (end >= start + g_time) {
                break;
            }
        }
    }
    printf("n=%d, time=%ld, speed=%ldfps\n", n, (end-start), n * 100 / (end-start));
    exit(0);
}

static LRESULT HelloWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
        case MSG_CREATE:
            SetTimer(hWnd, 100, 10);
            break;
        case MSG_TIMER:
            {
                KillTimer(hWnd, 100);

                do_test(hWnd);
            }
            return 0;
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

static struct option long_options[] = {
    {"format", 1, 0, 'f'},
    {"width", 1, 0, 'w'},
    {"height", 1, 0, 'h'},
    {"time", 1, 0, 't'},
    {"swsurface", 0, 0, 's'},
    {"overlapped", 0, 0, 'o'},
    {0, 0, 0, 0}
};

static int parse_opt(int argc, const char *argv[]) {
    int c;

    for (;;) {
        c = getopt_long(argc, (char * const *)argv, "f:w:h:t:so", long_options, NULL);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'f':
                g_format = atoi(optarg);
                break;
            case 'w':
                g_w = atoi(optarg);
                break;
            case 'h':
                g_h = atoi(optarg);
                break;
            case 't':
                g_time = atoi(optarg);
                break;
            case 's':
                g_hw = 0;
                break;
            case 'o':
                g_to_screen = 0;
                break;
            default:
                return -1;
        }
    }
    return 0;
}

static void usage(const char *s) {
    printf("usage: %s [--width|-w <width>] "
            "[--height|-h <height>] "
            "[--format|-f <format: 0)RGB565 1)ARGB8888 2)XRGB8888 >] "
            "[--time|-t <time>] "
            "[--swsurface|-s] "
            "[--overlapped|-o] "
            "\n", s);
}

int MiniGUIMain (int argc, const char *argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    if (parse_opt(argc, argv) < 0) {
        usage(argv[0]);
        return 1;
    }

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
