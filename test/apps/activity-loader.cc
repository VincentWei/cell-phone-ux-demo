#ifndef WIN32
#   include <sys/time.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#endif
#include <time.h>

#define protected public

#include "global.h"
#include "register.h"
#include "apps/screen-lock/mshutterpiece.h"

#include "Activity.hh"
#include "ContentResolver.hh"
#include "SimpleProviderFactory.hh"
#include "apps/screen-lock/ScreenLockActivity.hh"

#define BACK_GROUND_IMAGE   "res/common/toptitle.png"
#define IDC_STATIC_TIME         100
#define IDC_TIMER               150
#define TITLE_TEXT_POS_W    50 
#define TITLE_TEXT_POS_H    12
#define TITLE_TEXT_POS_Y    2
#define TITLE_TEXT_POS_X    ((SCREEN_W-TITLE_TEXT_POS_W)>>1)
static BITMAP backGroundBmp;

static CTRLDATA timeAndDate[] =
{
    {
        "static",
        WS_VISIBLE,
        TITLE_TEXT_POS_X, TITLE_TEXT_POS_Y,
        TITLE_TEXT_POS_W, TITLE_TEXT_POS_H,
        IDC_STATIC_TIME,
        "",
        0,
        WS_EX_TRANSPARENT, 
    },

};

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
    char time[10];
    memset(time, 0, 8);
    if(tm->tm_hour >= 12) {
        sprintf(time, "%02d:%02d PM", tm->tm_hour, tm->tm_min);
    } else {
        sprintf(time, "%02d:%02d AM", tm->tm_hour, tm->tm_min);
    }
#endif
    
    SetWindowText(GetDlgItem(hWnd, IDC_STATIC_TIME), time);

}

Activity *activity;
static LRESULT InfoBarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            {
                mGEffInit();

                if(!SetTimer(hWnd, IDC_TIMER, 100)) {
                    printf("create timer failed\n");
                    assert(0);
                    return -1;
                }

                if(0 != LoadBitmapFromFile(HDC_SCREEN, &backGroundBmp, BACK_GROUND_IMAGE)) {
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


                SetTopBarInfo(hWnd); 
            }
            break;
        case MSG_TIMER:
            SetTopBarInfo(hWnd);
            break;
        case MSG_PAINT:
            {
                HDC hdc = BeginPaint(hWnd);
                FillBoxWithBitmap(hdc, 0, 0, SCREEN_W, ACTIVITY_Y, &backGroundBmp); 
                
                //FillBoxWithBitmapPart(hdc, 181, 0, 0, 0, 0, 0,  &battery, signal*BATTERY_ICON_WIDTH); 
                EndPaint(hWnd, hdc);
            }
            return 0; 

        case MSG_CLOSE:
            fprintf(stderr, "%s:%d.\n", __FUNCTION__, __LINE__);
            UnloadBitmap(&backGroundBmp);

            SendNotifyMessage(activity->hwnd(), MSG_CLOSE, 0, 0);
            DestroyMainWindow(hWnd);
            mGEffDeinit ();
            break;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static int MyMiniGUIMain(const char *activity_name) {
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    if (! getenv("SHOW_CURSOR")) {
        ShowCursor(FALSE);
    }

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "CellPhone" , 0 , 0);
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
    
    Init32MemDC();
    ncsInitialize();
    ncs4TouchInitialize();
    REGISTER_NCS();

    hMainWnd = CreateMainWindow (&CreateInfo);
    if (hMainWnd == HWND_INVALID) {
        assert(0);
        return -1;
    }

    // register system content provider
    GET_CONTENT_RESOLVER()->registerContentProvider(
            ProviderFactory::CreateSystemProvider(SETTINGS_PROVIDER));
    GET_CONTENT_RESOLVER()->registerContentProvider(
            ProviderFactory::CreateSystemProvider(CONTACTS_PROVIDER));
    GET_CONTENT_RESOLVER()->registerContentProvider(
            ProviderFactory::CreateSystemProvider(MEDIASTORE_PROVIDER));
    GET_CONTENT_RESOLVER()->registerContentProvider(
            ProviderFactory::CreateSystemProvider(SMS_PROVIDER));
    GET_CONTENT_RESOLVER()->registerContentProvider(
            ProviderFactory::CreateSystemProvider(CALLLOG_PROVIDER));
    GET_CONTENT_RESOLVER()->registerContentProvider(
            ProviderFactory::CreateSystemProvider(APPSINFO_PROVIDER));

    ActivityFactory* tmp = ActivityFactory::singleton();

    activity = tmp->create(activity_name);
    if (! activity) {
        fprintf(stderr, "Failed to create %s\n", activity_name);
        tmp->list ();
        assert (0);
        exit(1);
    }
    activity->onStart();

    ShowWindow(activity->hwnd(), SW_SHOW);
    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    //activity->onStop();

    GET_CONTENT_RESOLVER()->clearContentProvider();
    delete GET_CONTENT_RESOLVER();

    MainWindowThreadCleanup (hMainWnd);

    MGNCS_UNREG_COMPONENT(mContainerCtrl);
    MGNCS_UNREG_COMPONENT(mIconFlow);
    Release32MemDC();
    ncs4TouchInitialize();
    ncsUninitialize();

    //delete activity;

    return 0;
}

int main(int argc, const char *argv[]) {
    int ret;
    std::vector<std::string> names;
    ActivityFactory::singleton()->queryNames(names);
    std::string activity_name;
    std::vector<std::string>::const_iterator i;

    if (argc == 1) {
        for (i=names.begin(); i!=names.end(); ++i) {
            if (strstr(argv[0], i->c_str())) {
                activity_name = i->c_str();
                break;
            }
        }
        if (i==names.end()) {
            fprintf(stderr, "Usage: %s mklinks|rmlinks|<ClassName>\n", argv[0]);
            ActivityFactory::singleton()->list();
            exit(1);
        }
    }else if (argc == 2) {
        if (strcmp(argv[1], "mklinks")==0) {
            for (i=names.begin(); i!=names.end(); ++i) {
                printf("mklink %s\n", i->c_str());
#ifndef WIN32
                if (symlink(argv[0], i->c_str()) < 0) {
                    printf("error on mklink %s\n", i->c_str());
                }
#endif
            }
            exit(0);
        }else if (strcmp(argv[1], "rmlinks") == 0) {
            for (i=names.begin(); i!=names.end(); ++i) {
                unlink(i->c_str());
            }
            exit(0);
        }else{
            activity_name = argv[1];
        }
    }

    InitGUI(argc, argv);

    DO_REGISTER_ACTIVITY (ScreenLockActivity);

    ret = MyMiniGUIMain(activity_name.c_str());

    TerminateGUI(0);

    return ret;
}

