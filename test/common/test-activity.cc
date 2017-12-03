#include <stdarg.h>
#include <gtest/gtest.h>
#include <sys/times.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <sys/times.h>

#define private public
#define protected public

#include "Activity.hh"

#define myprintf(x,y...) printf("\033[1m\033[0;34m"x"\033[0m\n",##y)
#define TEST_TIMERID 0x1343
#define TEST_TIMER_INTERVAL 10
class ActivityTest:public Activity
{
public:
    virtual HWND createHWND();
    static int Myproc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam);
    static ActivityTest *CreateActivity(const ActivityProperty& prop) { 
        return new ActivityTest(prop); 
    }
    bool ActivityTestGetonOkFlag()
    {
        return onOkFlag;
    }
    bool ActivityTestGetonCancelFlag()
    {
        return onCancelFlag;
    }
protected:
    ActivityTest(const ActivityProperty&);
    virtual int onOk()
    {
        onOkFlag = true;
        return -1;
    }
    virtual int onCancel()
    {
        onCancelFlag = true;
        return -1;
    }
    
    virtual void onDestroy()
    {
        SendNotifyMessage (m_hwnd, MSG_CLOSE, 0, 0);
    }
    ~ActivityTest();
private:
    bool onOkFlag;
    bool onCancelFlag;
};

const char* ActivityTestName = "ActivityTest";
ActivityTest *g_p_activity;

HWND init_activity_test (void);

ActivityTest::ActivityTest(const ActivityProperty& prop) :
    Activity(prop)
{
    onOkFlag = false;
    onCancelFlag = false;
    ShowWindow(m_hwnd,SW_SHOW);
}

ActivityTest::~ActivityTest()
{
}

HWND ActivityTest::createHWND()
{
    HWND g_hwnd;
    g_hwnd = init_activity_test ();
    return g_hwnd;
}

int ActivityTest::Myproc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            //SetTimer(hWnd,TEST_TIMERID,TEST_TIMER_INTERVAL);
            break;

        case MSG_TIMER:
            if ((int)wParam == TEST_TIMERID)
            {
                KillTimer(hWnd,TEST_TIMERID);
                ShowWindow(hWnd,SW_HIDE);
                g_p_activity->onDestroy();
            }
            break;

        case MSG_CLOSE:
            myprintf("ok   Receive the MSG_CLOSE message");
            DestroyMainWindow(g_p_activity->hwnd());
            PostQuitMessage(g_p_activity->hwnd());
            return 0;

        case MSG_COMMAND:
            switch (wParam) {
                case IDOK:
                    g_p_activity->onOk();
                    break;

                case IDCANCEL:
                    g_p_activity->onCancel();
                    break;
            }
            break;

        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}
HWND init_activity_test (void)
{
    HWND g_hwnd;
    MAINWINCREATE CreateInfo;

#ifdef  _MGRM_PROCESSES
    JoinLayer (NAME_DEF_LAYER, "activity", 0, 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE|WS_CAPTION | WS_BORDER;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "ActivityTest";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = ActivityTest::Myproc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 320;
    CreateInfo.by = 240;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    g_hwnd = CreateMainWindow(&CreateInfo);

    return g_hwnd;
}

TEST(Activity, GetActivityFromHWND) {
    ActivityTest *p_activity;
    p_activity = (ActivityTest *)Activity::getActivityFromHWND(g_p_activity->hwnd());
    ASSERT_EQ(p_activity->hwnd(), g_p_activity->hwnd());
    myprintf("ok   the Activity HWND:0x%x",g_p_activity->hwnd());
}

TEST(Activity, snapshot) {
    HDC hdc_temp;
    hdc_temp = g_p_activity->snapshot();
    ASSERT_NE(hdc_temp,NULL);
    myprintf("ok   the snapshot hdc is not NULL");
}

TEST(Activity,name) {
    const char* get_name = g_p_activity->name().c_str();
    ASSERT_STREQ(ActivityTestName,get_name);
    myprintf("ok   the Activity have the correct name:%s",ActivityTestName);
}

TEST(Activity,effectorInName) {
    const char* effectorin_name = g_p_activity->effectorInName().c_str();
    if (strlen(effectorin_name)>0)
        myprintf("0k   effectorInName:%s",effectorin_name);
}

TEST(Activity,effectorOutName) {
    const char* effectorout_name = g_p_activity->effectorOutName().c_str();
    if (strlen(effectorout_name)>0)
        myprintf("0k   effectoroutName:%s",effectorout_name);
}

TEST(Activity,hasInEffect) {
    bool hasInEffect = g_p_activity->hasInEffect();
    bool get_form_inname = strlen(g_p_activity->effectorInName().c_str())>0?true:false;
    ASSERT_EQ(get_form_inname,hasInEffect);
}

TEST(Activity,hasOutEffect) {
    bool hasOutEffect = g_p_activity->hasOutEffect();
    bool get_form_outname = strlen(g_p_activity->effectorOutName().c_str())>0?true:false;
    ASSERT_EQ(get_form_outname,hasOutEffect);
}

TEST(Activity,onOk) {
    SendMessage (g_p_activity->hwnd(), MSG_COMMAND, IDOK, 0);
    ASSERT_TRUE(g_p_activity->ActivityTestGetonOkFlag());
}

TEST(Activity,onCancel) {
    SendMessage (g_p_activity->hwnd(), MSG_COMMAND, IDCANCEL, 0);
    ASSERT_TRUE(g_p_activity->ActivityTestGetonCancelFlag());
}

TEST(Activity,get_property) {
    ActivityProperty property = g_p_activity->property();
    EXPECT_STREQ(g_p_activity->effectorInName().c_str(),property.m_effectorInName.c_str());
    EXPECT_STREQ(g_p_activity->effectorOutName().c_str(),property.m_effectorOutName.c_str());
}

TEST(Activity,set_property) {
    ActivityProperty property;
    const char* cfg_temp = "/home/chaotictimes";
    const char* Inname_temp = "my test In Name";
    const char* Outname_temp = "my test Out Name";
    property.m_configPath = cfg_temp;
    property.m_effectorInName = Inname_temp;
    property.m_effectorOutName = Outname_temp;
    g_p_activity->property(property);
    property.m_configPath = "";
    property.m_effectorInName = "";
    property.m_effectorOutName = "";
    property = g_p_activity->property();
    EXPECT_STREQ(cfg_temp,property.m_configPath.c_str());
    EXPECT_STREQ(Inname_temp,property.m_effectorInName.c_str());
    EXPECT_STREQ(Outname_temp,property.m_effectorOutName.c_str());
}


int MiniGUIMain(int argc, const char* argv[]) 
{
    int ret = 0;
    MSG Msg;

    ::testing::InitGoogleTest(&argc, (char **)argv);

    ActivityProperty activity_property;
    activity_property.m_name = (std::string)ActivityTestName;
    g_p_activity = ActivityTest::CreateActivity(activity_property); 
    g_p_activity->create();

    ret = RUN_ALL_TESTS();

    SetTimer(g_p_activity->hwnd(),TEST_TIMERID,TEST_TIMER_INTERVAL);

    while (GetMessage(&Msg, g_p_activity->hwnd())) {
        TranslateMessage(&Msg);        
        DispatchMessage(&Msg);
    }
    delete g_p_activity;

    MainWindowThreadCleanup (g_p_activity->hwnd());
    SendNotifyMessage(HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
    return ret;
}
