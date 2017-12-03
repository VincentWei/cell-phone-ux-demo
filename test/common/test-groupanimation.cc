#include <gtest/gtest.h>
#include <mgeff/mgeff.h>
#include <sys/time.h>
#include <time.h>
int sequential_group1_temp = 0,sequential_group2_temp = 0;
int startflag = 0;
HWND hwnd;
MGEFF_ANIMATION parallel_group, sequential_group1,sequential_group2;
static void act_onDraw (MGEFF_ANIMATION handle, void* target, int id, void* value)
{
    int val = *(int *)value;
    startflag = 1;
    if (id == 0)
    {
        sequential_group1_temp = val;
        //printf("the first sequent group value = %d\n",val);
    }
    else
    {
        sequential_group2_temp = val;
        //printf("the second sequent group value = %d\n",val);
    }
}
/* Test animation group create */
void Create() {
    MGEFF_ANIMATION animation1,animation2;
    int pts = 1;
    int pte = 200;
    sequential_group1 = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);
    
    sequential_group2 = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);

    animation1 = mGEffAnimationCreate (sequential_group1, act_onDraw, 
            0,MGEFF_INT);
    mGEffAnimationSetStartValue (animation1, &pts);
    mGEffAnimationSetEndValue (animation1, &pte);
    mGEffAnimationSetCurve (animation1,Linear);
    mGEffAnimationSetDuration (animation1, 60000);
    mGEffAnimationAddToGroup(sequential_group1,animation1);

    pts = 201;
    pte = 400;
    animation2 = mGEffAnimationCreate (sequential_group2, act_onDraw, 
            1,MGEFF_INT);
    mGEffAnimationSetStartValue (animation2, &pts);
    mGEffAnimationSetEndValue (animation2, &pte);
    mGEffAnimationSetDuration (animation2, 60000);
    mGEffAnimationSetCurve (animation2,Linear);
    mGEffAnimationAddToGroup(sequential_group2,animation2);

    parallel_group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    mGEffAnimationAddToGroup(parallel_group,sequential_group1);
    mGEffAnimationAddToGroup(parallel_group,sequential_group2);
}
#if 0
TEST(AnimationgroupTest, group_start) {
    EXPECT_EQ(startflag,1);
}

TEST(AnimationgroupTest, sequential_group1_Pause) {
    int temp;
    mGEffAnimationPause(sequential_group1);
    temp = sequential_group1_temp;
    sleep(1);
    EXPECT_EQ(temp,sequential_group1_temp);
    EXPECT_NE(temp,0);
}
TEST(AnimationgroupTest, parallel_group_Pause) {
    int temp;
    mGEffAnimationPause(parallel_group);
    temp = sequential_group2_temp;
    sleep(1);
    EXPECT_EQ(temp,sequential_group2_temp);
}
TEST(AnimationgroupTest, parallel_group_Resume) {
    int temp;
    startflag = 0;
    mGEffAnimationResume(parallel_group);
    temp = sequential_group1_temp;
    sleep(1);
    ASSERT_TRUE(startflag == 1);
    EXPECT_EQ(temp,sequential_group1_temp);
}
TEST(AnimationgroupTest, sequential_group2_Pause) {
    int temp;
    mGEffAnimationPause(sequential_group2);
    temp = sequential_group2_temp;
    sleep(1);
    EXPECT_EQ(temp,sequential_group2_temp);
    EXPECT_NE(temp,0);

}
TEST(AnimationgroupTest, sequential_group1_Resume) {
    int temp;
    mGEffAnimationResume(sequential_group1);
    temp = sequential_group1_temp;
    sleep(1);
    ASSERT_TRUE(temp != sequential_group1_temp);
}
TEST(AnimationgroupTest, sequential_group2_Resume) {
    int temp;
    mGEffAnimationResume(sequential_group1);
    temp = sequential_group1_temp;
    sleep(1);
    ASSERT_TRUE(temp != sequential_group1_temp);
}
#endif
TEST(AnimationgroupTest,Create){
    Create();
}
int MainWinProc(HWND hwnd, int msg, WPARAM wparam, LPARAM lparam)
{
    static int loop = -1;
    static int temp;
    int ret;
    switch(msg)
    {
        case MSG_CREATE:
            mGEffInit();
            return 0;
        case MSG_CLOSE:
            mGEffDeinit();
            break;

        case MSG_TIMER:
            if (loop == -1)
            {
                ret = RUN_ALL_TESTS();
                mGEffAnimationAsyncRun (parallel_group);
            }
            else if (loop == 0)
            {
                EXPECT_EQ(startflag,1);
            }
            else if (loop == 1)
            {
                mGEffAnimationPause(sequential_group1);
                temp = sequential_group1_temp;
            }
            else if (loop == 2)
            {
                EXPECT_EQ(temp,sequential_group1_temp);
            }
            else if (loop == 3)
            {
                mGEffAnimationPause(parallel_group);
                temp = sequential_group2_temp;
            }
            else if (loop == 4)
            {
                EXPECT_EQ(temp,sequential_group2_temp);
            }
            else if (loop == 5)
            {
                mGEffAnimationResume(parallel_group);
                temp = sequential_group1_temp;
            }
            else if (loop == 6)
            {
                EXPECT_EQ(temp,sequential_group1_temp);
            }
            else if (loop == 7)
            {
                mGEffAnimationPause(sequential_group2);
                temp = sequential_group2_temp;
            }
            else if (loop == 8)
            {
                EXPECT_EQ(temp,sequential_group2_temp);
            }
            else if (loop == 9)
            {
                mGEffAnimationResume(sequential_group1);
                temp = sequential_group1_temp;
            }
            else if (loop == 10)
            {
                EXPECT_NE(temp,sequential_group1_temp);
            }
            else if (loop == 11)
            {
                mGEffAnimationResume(sequential_group2);
                temp = sequential_group2_temp;
            }
            else if (loop == 12)
            {
                EXPECT_NE(temp,sequential_group2_temp);
            }
            else
            {
                printf("test end==============\n");
                KillTimer(hwnd,100);
            }
            loop++;
            break;
    }
    return DefaultMainWinProc(hwnd, msg,wparam,lparam);
}
int MiniGUIMain (int argc,const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    int ret;
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mgeff test" , 0 , 0);
#endif


    ::testing::InitGoogleTest(&argc, (char **)argv);
    CreateInfo.dwStyle = WS_VISIBLE;// | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mgeff test";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MainWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 320;
    CreateInfo.by = 240;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    hwnd = hMainWnd;
    SetTimer(hwnd,100,50);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }


    MainWindowThreadCleanup (hMainWnd);

    return ret;
}
