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
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#ifndef WIN32
#    include <sys/time.h>
#endif

#define WINDOW_W  800
#define WINDOW_H  480

#define IDC_BTN_BITBLT
#define IDC_BTN_FILLBOX


#define TEST_CASE_BITBLT 0x1
#define TEST_CASE_FILLBOX 0x2

#define TEST_CASE_NUM 3

static int g_testcase = 0;
static int g_testcount = 100;
static unsigned int g_surface = MEMDC_FLAG_SWSURFACE;
static int g_width = ACTIVITY_W;
static int g_height = ACTIVITY_H;

static const int masks [][4] = {
    {0xf800, 0x07e0, 0x001f,      0}, /* 565  RGBA */
    {0xf000, 0x0f00, 0x00f0, 0x000f}, /* 4444 RGBA */
    {0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff}, /* 8888 RGBA */
};

enum {
    MASK_565 = 0,
    MASK_4444,
    MASK_8888,
};



static const char* testcase_names[] = {
    "bitblt",
    "fillbox",
    "all",
};


static void TellSpeed (HDC hdc, unsigned int start_tick, 
                       unsigned int end_tick, const char* drawing, unsigned int count)
{
	char msg [1024];
    unsigned int ticks = end_tick - start_tick;
	double seconds = ticks / 100.0;
    int x = 380;
    int y = 0;

	sprintf (msg,
             "\n%s: do %u times                   \n"
             "==============================================\n"
             "Totol: %0.2f seconds.               \n"
             /* "Per-drawing: %0.2f seconds.\n" */
             "Speed: %0.2f times per seconds.       ",
             drawing, count, seconds,
             /* ticks / (count * 100.0), */
             count / seconds);

    printf("%s\n", msg);
    
    SetBrushColor (hdc, RGB2Pixel(hdc, 0, 0, 0));
    TabbedTextOut (hdc, x, y, msg);
}

static const int colortable[][3] = {
    { 0x00, 0xff, 0xff, },
    { 0xff, 0x00, 0xff, },
    { 0xff, 0xff, 0x00, },
    { 0xff, 0xff, 0xff, },
};


static gal_pixel getcolor(HDC mem_dc, int coloridx, BOOL alpha)
{
    gal_pixel pixel;
    
    if (alpha)
        pixel = RGBA2Pixel (
            mem_dc,
            colortable[coloridx % 4][0],
            colortable[coloridx % 4][1],
            colortable[coloridx % 4][2], 0x80);
    else
        pixel = RGB2Pixel (
            mem_dc,
            colortable[coloridx % 4][0],
            colortable[coloridx % 4][1],
            colortable[coloridx % 4][2]);

    return pixel;
}

static void fill_with_color(HDC mem_dc, int x, int y, int width, int height, BOOL alpha)
{
    int half_w = width / 2;
    int half_h = height / 2;

    static int coloridx = 0;
    gal_pixel color;

    color = getcolor(mem_dc, coloridx, alpha);
    SetBrushColor (mem_dc, color);
    FillBox (mem_dc, x, y, half_w, half_h);
    
    color = getcolor(mem_dc, coloridx+1, alpha);
    SetBrushColor (mem_dc, color);
    FillBox (mem_dc, x+half_w, y, half_w, half_h);

    color = getcolor(mem_dc, coloridx+2, alpha);
    SetBrushColor (mem_dc, color);
    FillBox (mem_dc, x, y+half_h, half_w, half_h);

    color = getcolor(mem_dc, coloridx+3, alpha);
    SetBrushColor (mem_dc, color);
    FillBox (mem_dc, x+half_w, y+half_h, half_w, half_h);

    if (coloridx++ >= 4) coloridx = 0;
}

static void clear_window(HDC hdc)
{
    SetBrushColor (hdc, RGB2Pixel(hdc, 0, 0, 0));
    FillBox (hdc, 0, 0, WINDOW_W, WINDOW_H);
}


static void bitblt_test (HWND hwnd, HDC hdc, int test_count)
{
    HDC mem_dc;
    int count;
    unsigned int start_tick, end_tick;
    int width = g_width;
    int height = g_height;
    int dx = 0;
    int dy = 0;

    int surface_flag = g_surface;

    /* Compatible DC */
    mem_dc = CreateCompatibleDC (hdc);
    fill_with_color(mem_dc, 0, 0, width, height, FALSE);
    count = test_count;
    start_tick = GetTickCount ();
    while (count--) {
        BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
        /* getchar(); */
    }
    end_tick = GetTickCount ();
    clear_window(hdc);
    TellSpeed (hdc, start_tick, end_tick, "Compatible DC Blit", test_count);
    DeleteCompatibleDC (mem_dc);

    
    /* Compatible DC with colorkey */
    mem_dc = CreateCompatibleDC (hdc);
    fill_with_color(mem_dc, 0, 0, width, height, FALSE);
    SetMemDCColorKey (mem_dc, MEMDC_FLAG_SRCCOLORKEY, RGB2Pixel(mem_dc, 0xff, 0xff, 0xff));
    count = test_count;
    start_tick = GetTickCount ();
    while (count--) {
        BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
        /* getchar(); */
    }
    end_tick = GetTickCount ();
    clear_window(hdc);
    TellSpeed (hdc, start_tick, end_tick, "Compatible DC with colorkey Blit", test_count);
    DeleteCompatibleDC (mem_dc);


    /* Compatible DC with alpha */
    mem_dc = CreateCompatibleDC (hdc);
    fill_with_color(mem_dc, 0, 0, width, height, FALSE);
    SetMemDCAlpha (mem_dc, MEMDC_FLAG_SRCALPHA, 0x80);
    count = test_count;
    start_tick = GetTickCount ();
    while (count--) {
        BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
        /* getchar(); */
    }
    end_tick = GetTickCount ();
    clear_window(hdc);
    TellSpeed (hdc, start_tick, end_tick, "Compatible DC with source alpha Blit", test_count);
    DeleteCompatibleDC (mem_dc);


    /* 16 bit Memory DC */
    mem_dc = CreateMemDC (
        width, height, 16, 
        surface_flag,
        masks[MASK_565][0], masks[MASK_565][1], masks[MASK_565][2], masks[MASK_565][3]);
    fill_with_color(mem_dc, 0, 0, width, height, FALSE);
    start_tick = GetTickCount ();
    count = test_count;
    while (count--) {
        BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
        /* getchar(); */
    }
    end_tick = GetTickCount ();
    clear_window(hdc);
    TellSpeed (hdc, start_tick, end_tick, "16 bit MemDC Blit", test_count);
    DeleteMemDC (mem_dc);

    
    /* 16 bit Memory DC with colorKey */
    mem_dc = CreateMemDC (
        width, height, 16, 
        surface_flag | MEMDC_FLAG_SRCCOLORKEY,
        masks[MASK_565][0], masks[MASK_565][1], masks[MASK_565][2], masks[MASK_565][3]);
    fill_with_color(mem_dc, 0, 0, width, height, FALSE);
    SetMemDCColorKey (mem_dc, MEMDC_FLAG_SRCCOLORKEY, RGB2Pixel(mem_dc, 0xff, 0xff, 0xff));
    start_tick = GetTickCount ();
    count = test_count;
    while (count--) {
        BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
        /* getchar(); */
    }
    end_tick = GetTickCount ();
    clear_window(hdc);
    TellSpeed (hdc, start_tick, end_tick, "16 bit MemDC with colorkey Blit", test_count);
    DeleteMemDC (mem_dc);


    /* 16 bit Memory DC with alpha */
    if (MEMDC_FLAG_SWSURFACE == surface_flag) {
        mem_dc = CreateMemDC (
            width, height, 16,
            surface_flag,
            masks[MASK_4444][0], masks[MASK_4444][1], masks[MASK_4444][2], masks[MASK_4444][3]);
        fill_with_color(mem_dc, 0, 0, width, height, TRUE);
        start_tick = times (NULL);
        count = test_count;
        while (count--) {
            BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
            /* getchar(); */
        }
        end_tick = times (NULL);
        clear_window(hdc);
        TellSpeed (hdc, start_tick, end_tick, "16 bit MemDC with alpha Blit", test_count);
        DeleteMemDC (mem_dc);
    }

    /* 32 bit Memory DC */
    mem_dc = CreateMemDC (
        width, height, 32, 
        surface_flag,
        masks[MASK_8888][0], masks[MASK_8888][1], masks[MASK_8888][2], masks[MASK_8888][3]);
    fill_with_color(mem_dc, 0, 0, width, height, FALSE);

    start_tick = GetTickCount ();
    count = test_count;
    while (count--) {
        BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
        /* getchar(); */
    }
    end_tick = GetTickCount ();
    clear_window(hdc);
    TellSpeed (hdc, start_tick, end_tick, "32 bit MemDC Blit", test_count);

    DeleteMemDC (mem_dc);


    /* 32 bit Memory DC with alpha*/
    mem_dc = CreateMemDC (
        width, height, 32, 
        surface_flag | MEMDC_FLAG_SRCALPHA,
        masks[MASK_8888][0], masks[MASK_8888][1], masks[MASK_8888][2], masks[MASK_8888][3]);
    fill_with_color(mem_dc, 0, 0, width, height, TRUE);
    start_tick = GetTickCount ();
    count = test_count;
    while (count--) {
        BitBlt (mem_dc, 0, 0, width, height, hdc, dx, dy, 0);
        /* getchar(); */
    }
    end_tick = GetTickCount ();
    clear_window(hdc);
    TellSpeed (hdc, start_tick, end_tick, "32 bit MemDC with Alpha Blit", test_count);
    DeleteMemDC (mem_dc);
}



static void fillbox_test (HWND hwnd, HDC hdc, int test_count)
{
    HDC mem_dc;
    int count;
    unsigned int start_tick, end_tick;
    int width = g_width;
    int height = g_height;
    int dx = 0;
    int dy = 0;

    count = test_count;
    start_tick = GetTickCount ();
    while (count--) {
        fill_with_color(hdc, dx, dy, width, height, FALSE);
    }
    end_tick = GetTickCount ();
    TellSpeed (hdc, start_tick, end_tick, "Screen DC FillBox", test_count);


    mem_dc = CreateMemDC (
        width, height, 32, 
        g_surface,
        masks[MASK_8888][0], masks[MASK_8888][1], masks[MASK_8888][2], masks[MASK_8888][3]);
    count = test_count;
    start_tick = GetTickCount ();
    while (count--) {
        fill_with_color(mem_dc, dx, dy, width, height, TRUE);
    }
    end_tick = GetTickCount ();
    TellSpeed (hdc, start_tick, end_tick, "32 bit MemDC FillBox", test_count);
    DeleteMemDC(mem_dc);
}



/* static void bitblt_test(HDC hdc) */
/* { */
/*     struct timeval tv_start, tv_end; */
/*     HDC dc_32_sw = CreateMemDC (100, 100, 32, MEMDC_FLAG_SWSURFACE); */
/*     HDC dc_32_sw_alpha = CreateMemDC (100, 100, 32, MEMDC_FLAG_HWSURFACE) */
    
/*     gettimeofday(&tv_start, NULL); */

    
    
/*     gettimeofday(&tv_end, NULL); */
/* } */

static int HelloWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_CREATE:
    {
        /* HWND bitbltBtn; */
        /* HWND fillboxBtn; */
        
        /* bitbltBtn = CreateWindow(CTRL_BUTTON, */
        /*                         "Bitblt Test", */
        /*                         WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, */
        /*                         IDC_BTN_BITBLT, */
        /*                         10, 10, 60, 50, hWnd, 0); */
        /* fillboxBtn = CreateWindow(CTRL_BUTTON, */
        /*                         "Back", */
        /*                         WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, */
        /*                         IDC_BTN_FILLBOX, */
        /*                         100, 10, 60, 50, hWnd, 0); */

        /* SetNotificationCallback(hwndHome, assistantBtnNotifyProc); */
        /* SetNotificationCallback(hwndBack, assistantBtnNotifyProc); */
    }
    break;
    
    case MSG_PAINT:
    {
        /* HDC hdc = BeginPaint (hWnd); */
        /* TextOut (hdc, 0, 0, "Performance Test"); */
        /* EndPaint (hWnd, hdc); */
    }
    break;

    case MSG_CLOSE:
        DestroyMainWindow (hWnd);
        PostQuitMessage (hWnd);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int getTestcaseIndexFromName(const char* name)
{
    int i;
    for (i = 0; i < TEST_CASE_NUM; ++i) {
        if (strcmp(testcase_names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

void display_help(const char* program_name)
{
    static const char* help_msg =
        "Usage: %s [option]... [testcase]...\n"
        "\n"
        "Options: \n"
        "  -n <number>  			test number\n"
        "  -W <width>   			width of test window\n"
        "  -H <height>  			height of test window\n"
        "  -S <surface> 			h: MEMDC_FLAG_HWSURFACE w: MEMDC_FLAG_SWSURFACE\n"
        "  -h           			display this help message\n"
        "\n"
        "Testcase list:\n"
        "  bitblt\n"
        "  fillbox\n"
        "  all\n\n";

    printf(help_msg, program_name);
}


int parse_args(int argc, const char** argv)
{
#if 1

    int c;
    int i;

    opterr = 0;

    while ((c = getopt (argc, (char* const*)argv, "hn:S:W:H:")) != -1) 
        switch (c) {
        case 'h':
            display_help(argv[0]);
            break;
        case 'n':
            g_testcount = atoi(optarg);
            break;
        case 'S':
            g_surface = (optarg[0] == 'h') ? MEMDC_FLAG_HWSURFACE : MEMDC_FLAG_SWSURFACE;
            break;
        case 'W':
            g_width = atoi(optarg);
            break;
        case 'H':
            g_height = atoi(optarg);
            break;
        case '?':
            if (optopt == 'n' || optopt == 'S' || optopt == 'W' || optopt == 'H')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
        }

    for (i = optind; i < argc; ++i) {
        int idx = getTestcaseIndexFromName(argv[i]);
        if (idx >= 0) {
            if (idx == TEST_CASE_NUM-1)
                g_testcase = 0xffffffff;
            else
                g_testcase |= 1 << idx;
        }

    }

    return 0;

#else

    int i;
    int c;
    
    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') { /* options */
            c = argv[i][1];
            if ('\0' == c) {
                continue;
            }
            else if ('h' == c) {
                display_help();
            }
            else if ('n' == c) {
                if (++i < argc)
                    g_testcount = atoi(argv[i]);
            }
            else if ('S' == c) {
                if (++i < argc)
                    g_surface = (argv[i][0] == 'h') ? MEMDC_FLAG_HWSURFACE : MEMDC_FLAG_SWSURFACE;
            }
            else if ('W' == c) {
                if (++i < argc)
                    g_width = atoi(argv[i]);
            }
            else if ('H' == c) {
                if (++i < argc)
                    g_height = atoi(argv[i]);
            }
        }
        else {                  /* testcase */
            int idx = getTestcaseIndexFromName(argv[i]);
            if (idx >= 0) {
                if (idx == TEST_CASE_NUM-1)
                    g_testcase = 0xffffffff;
                else
                    g_testcase |= 1 << idx;
            }
        }
    }
#endif
}


static void InitCreateInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_VISIBLE;
    pCreateInfo->dwExStyle = 0;
    pCreateInfo->spCaption = "Performance Test" ;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor (0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = HelloWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0; 
    pCreateInfo->rx = WINDOW_W;
    pCreateInfo->by = WINDOW_H;
    pCreateInfo->iBkColor = PIXEL_lightwhite; 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}


int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    HDC hdc;

    if (parse_args(argc, argv) != 0) {
        exit(1);
    }
        
    printf("Current option values: \n");
    printf("  surface: %s\n", (g_surface == MEMDC_FLAG_HWSURFACE)
           ? "MEMDC_FLAG_HWSURFACE" : "MEMDC_FLAG_SWSURFACE");
    printf("  width: %d\n", g_width);
    printf("  height: %d\n", g_height);
    printf("  test number: %d\n", g_testcount);

    /* printf("%x\n", g_testcase); */

#ifdef _MGRM_PROCESSES
    JoinLayer (NAME_DEF_LAYER, argv[0], 0, 0);
#endif

    InitCreateInfo (&CreateInfo);

    hMainWnd = CreateMainWindow (&CreateInfo);
    if (hMainWnd == HWND_INVALID)
        return -1;

    hdc = GetClientDC(hMainWnd);

    clear_window(hdc);
    
    if ( (g_testcase & TEST_CASE_BITBLT) != 0)
        bitblt_test(hMainWnd, hdc, g_testcount);
    if ( (g_testcase & TEST_CASE_FILLBOX) != 0)
        fillbox_test(hMainWnd, hdc, g_testcount);
    
    ReleaseDC(hdc);

    /* printf("\nDONE!\n"); */

    PostMessage(hMainWnd, MSG_CLOSE, 0, 0);

    while (GetMessage (&Msg, hMainWnd)) {
        DispatchMessage (&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    exit(0);
}


