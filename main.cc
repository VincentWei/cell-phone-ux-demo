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
#ifndef WIN32
#    include <sys/time.h>
#endif
#include <time.h>

#include "global.h"
#include "register.h"

#include "Activity.hh"
#include "ActivityStack.hh"

#include "TimeService.hh"
#include "ContentResolver.hh"
#include "SimpleProviderFactory.hh"
#include "SQLiteDB.hh"

#include "ResourceManager.hh"

#ifdef _MGINCORE_RES
# include "inner_res_init.h"
#endif

//#define USE_DRAW_WND
#define USE_ASS_WND

#define SCREEN_IDLE_TIMER_ID	1

extern "C" {
    extern int tscal_win_entry (void);
}

//#define STEPLOADRES
//#undef STEPLOADRES

#ifdef STEPLOADRES
#define LOAD_RESOURCE_TIMER_ID	2
#define LOAD_RESOURCE_TIMER     (2*100)

static BOOL delayLoadRes (HWND hwnd, int timer_id, DWORD tick_count);
#endif
#if 0
static BOOL unloadRes();
#endif

#define BACK_GROUND_IMAGE   "res/common/toptitle.png"

#define IDC_STATIC_TIME         100
#define TITLE_TEXT_POS_W    40 
#define TITLE_TEXT_POS_H    16
#define TITLE_TEXT_POS_Y    2
#define TITLE_TEXT_POS_X    ((SCREEN_W-TITLE_TEXT_POS_W)>>1)

#define SYSTEM_TIME_TEXT_SIZE  15
PLOGFONT g_sysTimeFont = NULL;

#if defined(WIN32) && defined(NDEBUG)
    #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") 
#endif

static CTRLDATA timeAndDate[] =
{
    {
        "static",
        WS_VISIBLE ,
        TITLE_TEXT_POS_X, TITLE_TEXT_POS_Y,
        TITLE_TEXT_POS_W, TITLE_TEXT_POS_H,
        IDC_STATIC_TIME,
        "",
        0,
        WS_EX_TRANSPARENT, 
    },

};

HWND hwndHome;
HWND hwndBack;
HWND hwndEnter;

#define ASSISTANT_DRAW  0xffffaaaa
#define ASSISTANT_WND_W  250//ACTIVITY_W
#define ASSISTANT_WND_H  136//ACTIVITY_H
#define ASSISTANT_WND_LEFT 5 

#if defined(USE_ASS_WND) && defined(USE_DRAW_WND)
static int drawWndProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    DWORD color;
    static BOOL isDown = FALSE;
    static int point = 0;

    if(wParam != ASSISTANT_DRAW 
            && message >= MSG_FIRSTMOUSEMSG && message <= MSG_LASTMOUSEMSG)
        return 0;

    switch (message) {
    case MSG_PAINT:
        {
            HDC hdc;

            hdc = BeginPaint(hWnd);
            TextOut(hdc, 0, 0, "press ENTER key to clear screen");
            EndPaint(hWnd, hdc);
        }
        break;

    case MSG_LBUTTONDOWN:
    case MSG_MOUSEMOVE:
    case MSG_LBUTTONUP:
        {
            int x, y;
            HDC hdc;

            if(message == MSG_MOUSEMOVE && !isDown)
                return 0;
            if(message == MSG_LBUTTONDOWN && isDown)
                return 0;
            if(message == MSG_LBUTTONUP && !isDown)
                return 0;

            x = LOWORD(lParam);
            y = HIWORD(lParam);

            hdc = GetClientDC(hWnd);

            if(message == MSG_LBUTTONDOWN){
                isDown = TRUE;
                point = 1;
                color = 0xffff0000;
            }
            else if(message == MSG_MOUSEMOVE){
                point++;
                color = 0xff888888;
            }
            else if(message == MSG_LBUTTONUP){
                char ch[20] = {0};
                isDown = FALSE;
                point++;
                color = 0xff0000ff;
                sprintf(ch, "point count:%d", point);
                FillBox(hdc, 0, 16, ACTIVITY_W, 16);
                TextOut(hdc, 0, 16, ch);
                point = 0;
            }

            SetBrushColor(hdc, color);
            FillCircle(hdc, x, y, 2);

            ReleaseDC(hdc);
        }
        break;

    case MSG_KEYUP:
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case MSG_CLOSE:
        DestroyMainWindow(hWnd);
        break;
    default:
        break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static HWND createDrawWnd (HWND hostWnd)
{
    MAINWINCREATE CreateInfo;
    HWND hwnd;

    
    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW;
    CreateInfo.spCaption = "draw Window";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = drawWndProc;
    CreateInfo.lx =  ACTIVITY_X + ACTIVITY_W + ASSISTANT_WND_LEFT;
    CreateInfo.ty = ASSISTANT_WND_H - 20;
    CreateInfo.rx = ACTIVITY_X + ACTIVITY_W + ASSISTANT_WND_LEFT + ACTIVITY_W;
    CreateInfo.by = ASSISTANT_WND_H + SCREEN_H - 20;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = hostWnd;

    hwnd = CreateMainWindow(&CreateInfo);
    //ShowWindow(hwnd, SW_SHOWNORMAL);
    //
    return hwnd;
}
#endif
#if 0
void SetTopBarInfo(HWND hWnd)
{
#ifdef WIN32
    char time[]="12:00:00";
    char year_month[]="2010-08";
    char day[]="20";
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *tm = localtime(&tv.tv_sec);
    char time[8];
    memset(time, 0, 8);
    if(tm->tm_hour >= 12) {
        sprintf(time, "%02d:%02d PM", tm->tm_hour, tm->tm_min);
    } else {
        sprintf(time, "%02d:%02d AM", tm->tm_hour, tm->tm_min);
    }
#endif
    
    SetWindowText(GetDlgItem(hWnd, IDC_STATIC_TIME), time);

}
#endif

BOOL updateInfoBar(void* target, TimeService *ts, int eventId, DWORD param)
{
    char time[]="12:00:00";

    HWND hwnd = (HWND)target;
#if 0
    int hour = ts->get(TimeService::HOUR);
    int min = ts->get(TimeService::MINUTE);
    if(hour >= 12) {
        sprintf(time, "%02d:%02d PM", hour, min);
    } else {
        sprintf(time, "%02d:%02d AM", hour, min);
    }
#else
    //TIMESERVICE->format(time, sizeof(time), "%I:%M %p");
    TIMESERVICE->format(time, sizeof(time), "%H:%M");

#endif
    
    assert (g_sysTimeFont);
    SetWindowFont (GetDlgItem(hwnd, IDC_STATIC_TIME), g_sysTimeFont);
    SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TIME), time);

    return TRUE;
}

#if defined(USE_ASS_WND)
typedef enum{
    ESC_BUTTON,
    BACKSPACE_BUTTON,
    BUTTON_MAX,
}button_e;
static PBITMAP kb;
static PBITMAP button_image[BUTTON_MAX*2];
#define IDC_BTN_HOME 100
#define IDC_BTN_BACK 101
#define ASS_BUTTON_W 122//131
#define ASS_BUTTON_H 38//45
#define ASS_BUTTON_X 0
#define KB_IMAGE   "res/desktop/kb.jpg"
#define ESC_IMAGE   "res/desktop/esc.png"
#define BACKSPACE_IMAGE   "res/desktop/backspace.png"
#define ESC_HILIGHT_IMAGE   "res/desktop/esc_hilight.png"
#define BACKSPACE_HILIGHT_IMAGE   "res/desktop/backspace_hilight.png"
static void assistantBtnNotifyProc(mWidget *self, int id, int nc, DWORD addData)
{
    mButton *button = (mButton *)self;
    if (ESC_BUTTON == id) {
        _c(button)->setProperty (button, NCSP_BUTTON_IMAGE, (DWORD)button_image[ESC_BUTTON + BUTTON_MAX]);
        UpdateWindow(self->hwnd,TRUE);
        SendNotifyMessage(HWND_DESKTOP, MSG_KEYUP, SCANCODE_ESCAPE, 0);
        usleep(200000);
        _c(button)->setProperty (button, NCSP_BUTTON_IMAGE, (DWORD)button_image[ESC_BUTTON]);
    }
    else if (BACKSPACE_BUTTON == id) {
        _c(button)->setProperty (button, NCSP_BUTTON_IMAGE, (DWORD)button_image[BACKSPACE_BUTTON + BUTTON_MAX]);
        UpdateWindow(self->hwnd,TRUE);
        SendNotifyMessage(HWND_DESKTOP, MSG_KEYUP, SCANCODE_BACKSPACE, 0);
        usleep(200000);
        _c(button)->setProperty (button, NCSP_BUTTON_IMAGE, (DWORD)button_image[BACKSPACE_BUTTON]);
    }
}

static BOOL ass_onEraseback(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static NCS_EVENT_HANDLER command_handlers[] = {
    NCS_MAP_NOTIFY(NCSN_WIDGET_CLICKED, assistantBtnNotifyProc),
    {MSG_ERASEBKGND, reinterpret_cast<void*>(ass_onEraseback)},
    {0, NULL},
};

static LRESULT assistantWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int i;
    static mButton *button[BUTTON_MAX];
    switch (message) {
    case MSG_CREATE:
        kb = (PBITMAP)LoadResource(KB_IMAGE, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
        button_image[ESC_BUTTON] = (PBITMAP)Load32Resource(ESC_IMAGE, 
            RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
        button_image[BACKSPACE_BUTTON] = (PBITMAP)Load32Resource(BACKSPACE_IMAGE, 
            RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
        button_image[ESC_BUTTON + BUTTON_MAX] = (PBITMAP)Load32Resource(ESC_HILIGHT_IMAGE, 
            RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
        button_image[BACKSPACE_BUTTON + BUTTON_MAX] = (PBITMAP)Load32Resource(BACKSPACE_HILIGHT_IMAGE, 
            RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
        for (i=0;i<BUTTON_MAX;i++)
        {
            button[i] = (mButton *)ncsCreateWindow(NCSCTRL_BUTTON,
                    0,
                    WS_VISIBLE | NCSS_BUTTON_IMAGE,
                    WS_EX_NONE,
                    i,
                    ASS_BUTTON_X,80*i,ASS_BUTTON_W,ASS_BUTTON_H,
                    hWnd,
                    NULL, NULL, command_handlers, 0);
            if (NULL != button[i]) {
                _c(button[i])->setProperty (button[i], NCSP_BUTTON_IMAGE, (DWORD)button_image[i]);
            }
            else {
                fprintf(stderr, "create button[%d] failed!\n", i);
                return -1;
            }
        }
        break;
    case MSG_PAINT:
        {
            HDC hdc;

            hdc = BeginPaint(hWnd);
            FillBoxWithBitmap(HDC_SCREEN, ACTIVITY_W, 0,800 ,480, kb); 
            EndPaint(hWnd, hdc);
        }
        break;
    case MSG_LBUTTONDOWN:
    case MSG_LBUTTONUP:
        break;
    case MSG_ERASEBKGND: 
        {
            HDC hdc = (HDC)wParam;
            FillBoxWithBitmapPart(hdc, 0, 0, ASS_BUTTON_W, 3*ASS_BUTTON_H,
                    0, 0, kb, 13, 150);
        }
        return 0;
    case MSG_CLOSE:
        ReleaseRes(Str2Key(KB_IMAGE));
        ReleaseRes(Str2Key(ESC_IMAGE));
        ReleaseRes(Str2Key(BACKSPACE_IMAGE));
        ReleaseRes(Str2Key(ESC_HILIGHT_IMAGE));
        ReleaseRes(Str2Key(BACKSPACE_HILIGHT_IMAGE));
        DestroyMainWindow(hWnd);
        break;
    default:
        break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}


static HWND createAssistantWnd (HWND hostWnd)
{
    HWND hwnd;
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC|WS_EX_TOOLWINDOW;
    CreateInfo.spCaption = "Assistant Window";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = assistantWndProc;
#ifdef USE_DRAW_WND
    CreateInfo.lx = ACTIVITY_X + ACTIVITY_W + ASSISTANT_WND_LEFT;
    CreateInfo.ty = 0;
    CreateInfo.rx = ACTIVITY_X + ACTIVITY_W + ASSISTANT_WND_LEFT + ASSISTANT_WND_W;
    CreateInfo.by = ASSISTANT_WND_H;
#else
    CreateInfo.lx = ACTIVITY_W+13;
    CreateInfo.ty = 150;
    CreateInfo.rx = CreateInfo.lx + ASS_BUTTON_W;
    CreateInfo.by = CreateInfo.ty + ASS_BUTTON_H*2+80-ASS_BUTTON_H;
#endif
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = hostWnd;

    hwnd = CreateMainWindow(&CreateInfo);
    //ShowWindow(hwnd, SW_SHOWNORMAL);
    //
    return hwnd;
}
#endif


static LRESULT InfoBarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef USE_32MEMDC
    static HDC bar_hdc;
#endif
    switch (message) {
        case MSG_CREATE:
            {
                // load resource
#ifdef STEPLOADRES
                SetTimerEx (hWnd, LOAD_RESOURCE_TIMER_ID, LOAD_RESOURCE_TIMER, (TIMERPROC)delayLoadRes);
#else
                int cnt = 0;

                cnt = LoadAllRes(res_list);

                printf ("LoadAllRes: %d\n", cnt);
#endif

#ifdef USE_32MEMDC
                bar_hdc = CreateCompatibleDCEx(Get32MemDC(),SCREEN_W, ACTIVITY_Y);
                assert(bar_hdc);
                SetSecondaryDC(hWnd,bar_hdc , ON_UPDSECDC_DEFAULT);
#endif
                g_sysTimeFont = CreateLogFontEx ("ttf", "helvetica", "UTF-8",
                        FONT_WEIGHT_REGULAR,
                        FONT_SLANT_ROMAN,
                        FONT_FLIP_NONE,
                        FONT_OTHER_NONE,
                        FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
                        SYSTEM_TIME_TEXT_SIZE, 0);

                mGEffInit();

                if(Load32Resource (BACK_GROUND_IMAGE, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN) == NULL) {
                    printf("load bitmap %s failed\n", BACK_GROUND_IMAGE);
                    assert(0);
                    return -1;
                }

                /*
                if(0 != LoadBitmapFromFile(HDC_SCREEN, &battery, BATTERY_IMAGE)) {
                    printf("load bitmap %s failed\n", BATTERY_IMAGE);
                    return -1;
                }*/ 

                size_t i = 0;
                for(i = 0; i < sizeof(timeAndDate)/sizeof(CTRLDATA); i++) { 
                    HWND hstatic= CreateWindowEx(timeAndDate[i].class_name,
                            timeAndDate[i].caption, timeAndDate[i].dwStyle, timeAndDate[i].dwExStyle, 
                            timeAndDate[i].id,
                            timeAndDate[i].x, timeAndDate[i].y, timeAndDate[i].w, timeAndDate[i].h,
                            hWnd, 
                            timeAndDate[i].dwAddData);

                    if(HWND_INVALID == hstatic) {
                        printf("create static for displaying date and time failed\n");
                        assert(0);
                        return -1;
                    }
                    SetWindowElementAttr (hstatic, WE_FGC_WINDOW, 0xFFFFFFFF);
                    //SetWindowElementAttr (GetDlgItem(hWnd, IDC_STATIC_DAY), WE_ATTR_TYPE_FONT, 1);
                } 

                //ACTIVITYSTACK->push(APP_NAME_DESKTOP);
                ACTIVITYSTACK->push("ScreenLockActivity");
            }
            break;
        case MSG_PAINT:
            {
                BITMAP *backGroundBmp = (BITMAP*)GetResource (Str2Key (BACK_GROUND_IMAGE));
                if (!backGroundBmp) {
                    printf ("Back ground image hadn't loaded success\n");
                    break;
                }
                HDC hdc = BeginPaint(hWnd);
                FillBoxWithBitmap(hdc, 0, 0, SCREEN_W, ACTIVITY_Y, backGroundBmp); 
                
                //FillBoxWithBitmapPart(hdc, 181, 0, 0, 0, 0, 0,  &battery, signal*BATTERY_ICON_WIDTH); 
                EndPaint(hWnd, hdc);
            }
            return 0; 

        case MSG_CLOSE:
            {
                for (;;) {
                    if (ACTIVITYSTACK->depth() == 0) {
                        break;
                    }

                    ACTIVITYSTACK->pop();
                }
            }
            printf ("ACTIVITYSTACK ....%d................\n",
            ACTIVITYSTACK->depth());

            DestroyMainWindow(hWnd);

            DestroyLogFont (g_sysTimeFont);
            g_sysTimeFont = NULL;

            ReleaseRes(Str2Key (BACK_GROUND_IMAGE));

            UnLoadAllRes (res_list);
           // unloadRes();
#ifdef USE_32MEMDC
            DeleteMemDC(bar_hdc);
#endif

            break;
        default:
            break;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}


/* 
 * this is initialize and finalize function, please call it when desktop
 * strat and end.
 */
BOOL initializeEnvironment (void)
{
    Init32MemDC();
    ncsInitialize ();
    ncs4TouchInitialize();

    REGISTER_NCS();

    return TRUE;
}

void finalizeEnvironment ()
{
    MGNCS_UNREG_COMPONENT(mIconFlow);
    MGNCS_UNREG_COMPONENT(mContainerCtrl);

    ncs4TouchUninitialize();
	ncsUninitialize ();
    Release32MemDC();
}

static BOOL idleHandler(HWND hwnd, LINT timer_id, DWORD tick_count)
{
    Activity* currApp = ACTIVITYSTACK->top();

    if (currApp) {
        assert (IsMainWindow (currApp->hwnd ()));
#if 0
        if (strcmp(ACTIVITYSTACK->currentAppName(), "ScreenLockActivity") != 0
            &&SendMessage(currApp->hwnd (), MSG_USER_IDLE, 0, 0) == 0) {
            ACTIVITYSTACK->push("ScreenLockActivity", (Intent*) 1);
        }
#else
        if (SendMessage(currApp->hwnd (), MSG_USER_IDLE, 0, 0) != 0) {
            ACTIVITYSTACK->switchTo("ScreenLockActivity", (Intent*) 2);
        }
#endif

    }
    
    return TRUE;
}

#ifdef STEPLOADRES
static BOOL delayLoadRes (HWND hwnd, int timer_id, DWORD tick_count)
{
    int cnt = 0;
    static int step = 1;
    int tick;

    if (step > MAX_STEP) {
        KillTimer (hwnd, timer_id);
        return TRUE;
    }

    printf ("delayLoadRes\n");

    tick = GetTickCount();

    cnt = StepLoadRes(res_list, step);

    printf ("loadRes: %d\n", GetTickCount() - tick);

    step++;

    printf ("StepLoadRes: %d\n", cnt);

    return TRUE;
}
#endif
#if 0
static BOOL unloadRes()
{
    UnLoadAllRes (res_list);

    return TRUE;
}
#endif

HWND g_info_bar;

int main (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    ContentProvider* provider[6];

#ifdef USE_ASS_WND
    //HWND ass;
#endif

#if defined(USE_ASS_WND) && defined(USE_DRAW_WND)
    HWND draw;
#endif

    if (InitGUI (argc, argv) != 0) {
        return 1;
    }

    DO_REGISTER_ACTIVITY (ActivityDesktop);
    DO_REGISTER_ACTIVITY (PhoneContactsActivity);
    DO_REGISTER_ACTIVITY (CalcActivity);
    DO_REGISTER_ACTIVITY (CallHistoryActivity);
    DO_REGISTER_ACTIVITY (ActivityCamera);
#ifdef GPU_GLES1
    DO_REGISTER_ACTIVITY (CoverFlowActivity);
#endif
    DO_REGISTER_ACTIVITY (DialPadActivity);
    DO_REGISTER_ACTIVITY (EraserActivity);
#ifdef GPU_GLES1
    DO_REGISTER_ACTIVITY (FlightActivity);
#endif
    DO_REGISTER_ACTIVITY (PhoneAlbumActivity);
    DO_REGISTER_ACTIVITY (ActivityMusicPlayer);
    DO_REGISTER_ACTIVITY (PhoneCallActivity);
    DO_REGISTER_ACTIVITY (ActivityPicturePlayer);
    DO_REGISTER_ACTIVITY (ActivityPicturePreviewer);
    DO_REGISTER_ACTIVITY (TransActivity);
    DO_REGISTER_ACTIVITY (Ncs4padActivity);
    DO_REGISTER_ACTIVITY (PieceAnimationActivity);
    DO_REGISTER_ACTIVITY (SampleActivity);
    DO_REGISTER_ACTIVITY (SampleNCSActivity);
    DO_REGISTER_ACTIVITY (SampleNavigation);
    DO_REGISTER_ACTIVITY (ScreenLockActivity);
    DO_REGISTER_ACTIVITY (SMSActivity);
    DO_REGISTER_ACTIVITY (SettingActivity);
    DO_REGISTER_ACTIVITY (TimeSettingActivity);

#ifdef _MGINCORE_RES
    CellPhoneInitInnerResource ();
#endif

    initializeEnvironment ();

    if (! getenv("SHOW_CURSOR")) {
        ShowCursor(FALSE);
    }
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "CellPhone" , 0 , 0);
#endif

#ifdef SCREEN_CALIBRATION
    tscal_win_entry ();
#endif

    memset(&CreateInfo, 0, sizeof(CreateInfo));
    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Desktop";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = InfoBarProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = SCREEN_W;
    CreateInfo.by = ACTIVITY_Y;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    /*char* cacheBuf = (char*)malloc(SQLiteDB::CACHE_PAGE_SIZE * SQLiteDB::MAX_PAGE_NUM);
    if (NULL != cacheBuf) {
        sqlite3_config(SQLITE_CONFIG_PAGECACHE, cacheBuf, SQLiteDB::CACHE_PAGE_SIZE, SQLiteDB::MAX_PAGE_NUM);
    }*/
    
    // register system content provider
    provider[0] = 
        ProviderFactory::CreateSystemProvider(SETTINGS_PROVIDER);
    provider[1] = 
        ProviderFactory::CreateSystemProvider(CONTACTS_PROVIDER);
    provider[2] = 
        ProviderFactory::CreateSystemProvider(MEDIASTORE_PROVIDER);
    provider[3] = 
        ProviderFactory::CreateSystemProvider(SMS_PROVIDER);
    provider[4] = 
        ProviderFactory::CreateSystemProvider(CALLLOG_PROVIDER);
    provider[5] = 
        ProviderFactory::CreateSystemProvider(APPSINFO_PROVIDER);

    for (int k = 0; k < 6; ++k) {
        GET_CONTENT_RESOLVER()->registerContentProvider(provider[k]);
    }

    hMainWnd = CreateMainWindow (&CreateInfo);
    if (hMainWnd == HWND_INVALID) {
        assert(0);
        return -1;
    }

    TIMESERVICE->setHost(hMainWnd);
    TIMESERVICE->addEventListener((void*)hMainWnd, TimeService::HOUR, updateInfoBar);
    TIMESERVICE->addEventListener((void*)hMainWnd, TimeService::MINUTE, updateInfoBar);
    updateInfoBar((void*)hMainWnd, TIMESERVICE, TimeService::MINUTE, 0);

    ShowWindow(hMainWnd, SW_SHOW);

#ifdef USE_ASS_WND
    /*ass = */createAssistantWnd(HWND_DESKTOP);
#endif

#if defined(USE_ASS_WND) && defined(USE_DRAW_WND)
    draw = createDrawWnd(HWND_DESKTOP);
#endif

    SetTimerEx(hMainWnd, SCREEN_IDLE_TIMER_ID, SCREEN_IDLE_TIMER, (TIMERPROC)idleHandler);
 
    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        
        if (Msg.message >= MSG_FIRSTMOUSEMSG && Msg.message <= MSG_LASTMOUSEMSG) {

            //printf("wParam: %d lParam: %d\n", Msg.wParam, Msg.lParam);
            
            // KillTimer(hMainWnd, SCREEN_IDLE_TIMER_ID);
            // SetTimerEx(hMainWnd, SCREEN_IDLE_TIMER_ID, SCREEN_IDLE_TIMER, (TIMERPROC)idleHandler);
            ResetTimerEx(hMainWnd, SCREEN_IDLE_TIMER_ID, SCREEN_IDLE_TIMER, (TIMERPROC)idleHandler);

#if defined(USE_ASS_WND) && defined(USE_DRAW_WND)
            //send mousemsg to draw wnd
            if(IsControl(Msg.hwnd) && Msg.hwnd != hwndHome && Msg.hwnd != hwndBack)
                {
                    LPARAM lParam;

                    lParam = Msg.lParam;
                    if(Msg.hwnd != hMainWnd){
                        int x,y;

                        x = LOWORD(lParam);
                        y = HIWORD(lParam);
                        ClientToScreen(Msg.hwnd, &x, &y);
                        lParam = MAKELONG(x, y);
                    }
                    SendMessage(draw, Msg.message, ASSISTANT_DRAW, lParam);
                }
#endif
        }
#if defined(USE_ASS_WND) && defined(USE_DRAW_WND)
        else if (Msg.message == MSG_KEYUP && Msg.wParam == SCANCODE_ENTER)
            SendMessage(draw, Msg.message, Msg.wParam, Msg.lParam);
#endif
        
        DispatchMessage(&Msg);

        if (Msg.message == MSG_LBUTTONDOWN && ACTIVITYSTACK->top()
            // && (strcmp(ACTIVITYSTACK->currentAppName(), "ScreenLockActivity") == 0)
            ) {
            PostMessage ((HWND) ACTIVITYSTACK->top()->hwnd(), MSG_USER_CLICK, Msg.wParam, Msg.lParam);
        }

    }

    KillTimer(hMainWnd, SCREEN_IDLE_TIMER_ID);

    TIMESERVICE->removeEventListener((void*)hMainWnd);

    //free(cacheBuf);

    MainWindowThreadCleanup (hMainWnd);
    GET_CONTENT_RESOLVER()->clearContentProvider();
    delete GET_CONTENT_RESOLVER();

    finalizeEnvironment (); 

    mGEffDeinit ();
    TerminateGUI (0);

    fprintf(stderr, "%s:quit.\n", __FUNCTION__);
	return 0;
}
