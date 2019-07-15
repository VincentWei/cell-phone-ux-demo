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
#include <gtest/gtest.h>

#define private public

#include "ActivityStack.hh"

#define APP_NAME    "MyActivity"

class MyActivity : public Activity {
public:
    MyActivity();
    ~MyActivity();
    
    HWND createHWND();
protected:
    static int proc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam);
};

REGISTER_ACTIVITY(MyActivity);

MyActivity::MyActivity() {
    //m_style = STYLE_NAVBAR;
}

MyActivity::~MyActivity() {
}

static int randomInRange(int low, int high)
{
    return low + random() % (high-low+1);
}

int MyActivity::proc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam) {
    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_CLOSE:
            delete Activity::getActivityFromHWND(hWnd);
            DestroyMainWindow(hWnd);
            return 0;
        case MSG_PAINT:
            {
                HDC hdc = BeginPaint(hWnd);

                SetBrushColor(hdc, RGBA2Pixel(hdc, 
                            randomInRange(0, 255), 
                            randomInRange(0, 255), 
                            randomInRange(0, 255), 0xff));
                FillBox(hdc, 0, 0, ACTIVITY_W, ACTIVITY_H);
                
                EndPaint(hWnd, hdc);
            }
            return 0;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

HWND MyActivity::createHWND() {
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "sample-activity";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MyActivity::proc;
    CreateInfo.lx =  ACTIVITY_X;
    CreateInfo.ty = ACTIVITY_Y;
    CreateInfo.rx = ACTIVITY_X + ACTIVITY_W;
    CreateInfo.by = ACTIVITY_Y + ACTIVITY_H;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    return CreateMainWindow(&CreateInfo);
}

TEST(Intent, Intent_newIntent)
{
    const int ACTION = 0;
    const std::string URI = "test://uri";
    const int TEST_NUM = 10;
    int i;

    for (i = 0; i < TEST_NUM; ++i) {
        Intent* p1 = Intent::newIntent();
        EXPECT_TRUE(p1->action() == ACTION_MAIN);
        EXPECT_TRUE(p1->uri() == "");
        Intent::deleteIntent(p1);

        Intent* p2 = Intent::newIntent(ACTION+i);
        EXPECT_TRUE(p2->action() == ACTION+i);
        EXPECT_TRUE(p2->uri() == "");
        Intent::deleteIntent(p2);

        static char tmp[10];
        sprintf(tmp, "%d", i);
        Intent* p3 = Intent::newIntent(ACTION+i, URI+tmp);
        EXPECT_TRUE(p3->action() == ACTION+i);
        EXPECT_TRUE(p3->uri() == URI+tmp);
        Intent::deleteIntent(p3);
    }
}

TEST(Intent, Intent_putExtra)
{
    const int TEST_NUM = 10;
    int i;

    for (i = 0; i < TEST_NUM; ++i) {
        Intent* p1 = Intent::newIntent();
        static char key[10];
        static char value[20];
        sprintf(key, "key %d", i);
        sprintf(value, "value %d", i);

        std::string v(value);
        //p1->putExtra(key, &(value[0]));
        p1->putExtra(key, std::string(value));

        boost::any r = p1->getExtra(key);
        EXPECT_TRUE(!r.empty());
        //EXPECT_TRUE(r.type() == typeid(std::string));
        try {
            //const std::string s = boost::any_cast<std::string>(r);
            //EXPECT_TRUE(strcmp(s.c_str(), value) == 0);
            //EXPECT_TRUE(strcmp(boost::any_cast<char*>(r), value) == 0);
            EXPECT_TRUE(strcmp(std::string(boost::any_cast<std::string>(r)).c_str(), value) == 0);
        } catch(boost::bad_any_cast&) {
            std::cout << "Oops!\n";
        }
        Intent::deleteIntent(p1);
    }
}

TEST(ActivityStack, ActivityStack_singleton)
{
        ActivityStack* s = ActivityStack::singleton();
        ActivityStack* si = NULL;
        int i;
        const static int NUM = 10;
        
        EXPECT_TRUE(s != NULL);
        
        for (i = 0; i < NUM; ++i) {
                si = ActivityStack::singleton();
                EXPECT_EQ(si, s);
        }
}

TEST(ActivityStack, ActivityStack_underTop)
{
        ActivityStack* s = ActivityStack::singleton();
        int i;
        const static int NUM = 10;
        
        EXPECT_TRUE(s != NULL);
        
        for (i = 0; i < NUM; ++i) {
                s->push(APP_NAME);
        }
        
        EXPECT_EQ(s->depth(), NUM);
        
        Activity* underTop = NULL;
        for (i = 0; i < NUM; ++i) {
            underTop = s->_underTop();
            s->pop();
            EXPECT_EQ(underTop, s->top());
        }
}

TEST(ActivityStack, ActivityStack_push_pop_depth)
{
        ActivityStack* s = ActivityStack::singleton();
        int i;
        const static int NUM = 10;
        
        EXPECT_TRUE(s != NULL);
        
        for (i = 0; i < NUM; ++i) {
                s->push(APP_NAME);
        }
        
        EXPECT_EQ(s->depth(), NUM);
        
        for (i = 0; i < NUM; ++i) {
                s->pop();
        }
}

TEST(ActivityStack, ActivityStack_push_lparam)
{
        ActivityStack* s = ActivityStack::singleton();
        int i;
        const static int NUM = 10;
        
        EXPECT_TRUE(s != NULL);
        
        for (i = 0; i < NUM; ++i) {
                s->push(APP_NAME);
        }
        
        for (i = 0; i < NUM; ++i) {
                s->pop();
        }
}

TEST(ActivityStack, ActivityStack_push_lparam_rparam)
{
        ActivityStack* s = ActivityStack::singleton();
        int i;
        const static int NUM = 10;
        
        EXPECT_TRUE(s != NULL);
        
        for (i = 0; i < NUM; ++i) {
                s->push(APP_NAME);
        }
        
        for (i = 0; i < NUM; ++i) {
                s->pop();
        }
}

TEST(ActivityStack, ActivityStack_popTo) 
{
        ActivityStack* s = ActivityStack::singleton();
        int i;
        const static int NUM = 10;
        Activity* activities[NUM];
        
        EXPECT_TRUE(s != NULL);
        
        for (i = 0; i < NUM; ++i) {
                s->push(APP_NAME);
                activities[i] = s->top();
        }
        
        for (i = NUM-1; i >= 0; --i) {
                s->popTo(activities[i]);
                EXPECT_EQ(s->top(), activities[i]);
        }
        
        
        for (i = 0; i < NUM; ++i) {
                s->push(APP_NAME);
                activities[i] = s->top();
        }
        
        s->popTo(activities[0]);
        EXPECT_EQ(s->top(), activities[0]);
}

class MyEnvironment : public testing::Environment
{
    public:
        virtual void SetUp()
        {
            std::cout << "Testing Environment Setup ..." << std::endl;
            mGEffInit();
        }
        virtual void TearDown()
        {
            std::cout << "Testing Environment Tear Down ..." << std::endl;
            mGEffDeinit();
        }
};

static int mainWindProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_CLOSE:
            break;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static HWND createWindow()
{
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    ShowCursor(TRUE);
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "test-ActivityStack" , 0 , 0);
#endif

    memset(&CreateInfo, 0, sizeof(CreateInfo));
    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Desktop";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = mainWindProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = SCREEN_W;
    CreateInfo.by = ACTIVITY_Y;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    if (hMainWnd == HWND_INVALID) {
        assert(0);
        return -1;
    }

    return hMainWnd;
}


int MiniGUIMain(int argc, const char* argv[]) 
{
    int ret = 0;
    HWND hwnd = createWindow();

    ShowWindow(hwnd, SW_SHOW);

    testing::AddGlobalTestEnvironment(new MyEnvironment);
    ::testing::InitGoogleTest(&argc, (char **)argv);

    ret =RUN_ALL_TESTS();

#if 0
    while (GetMessage(&Msg, hwnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
#endif
    MainWindowThreadCleanup(hwnd);

    return ret;
}
