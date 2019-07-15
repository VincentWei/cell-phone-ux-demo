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

// test-GraphicsView.cc

#include <stdio.h>

#include <gtest/gtest.h>
#include <iostream>
#include <climits>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "GraphicsView.hh"

#define RECT_EQ(rc1, rc2)   \
    (rc1.left == rc2.left &&    \
     rc1.top == rc2.top &&    \
     rc1.right == rc2.right &&    \
     rc1.bottom == rc2.bottom)
#define VALID_RECT(rc)   \
    (rc.right >= rc.left &&    \
     rc.bottom >= rc.top)
#define POINT_EQ(p1, p2)   \
    (p1.x == p2.x &&    \
     p1.y == p2.y)

HWND g_hwnd;

using namespace std;
using namespace GraphicsView;

class MyItem : public Item {
    public:
        virtual void onDraw(HDC dc) {}
};

static RECT zeroRect;
static POINT zeroPoint;

void setRect(RECT& rc, int l, int t, int r, int b)
{
    rc.left = l;
    rc.top = t;
    rc.right = r;
    rc.bottom = b;
}

void setRect(RECT& rc, const RECT& rcIn)
{
    memcpy(&rc, &rcIn, sizeof(rc));
}

void setPoint(POINT& p, int x, int y)
{
    p.x = x;
    p.y = y;
}

void setPoint(POINT& p, const POINT& pIn)
{
    memcpy(&p, &pIn, sizeof(p));
}

TEST (GraphicsView_Item_Test, initValue_Test) {
    MyItem item;
    RECT rc;

    item.getRect(rc);
    cout << "Init Item.getRect(): " << rc.left << " " << rc.top << " " << rc.right << " " << rc.bottom << endl;
    //EXPECT_TRUE(RECT_EQ(rc, zeroRect));

    
    EXPECT_EQ(1.0f, item.scale());
    EXPECT_EQ(1.0f, item.transparency());
}

TEST (GraphicsView_Item_Test, moveToRect_Test) {
    MyItem item;
    RECT rcIn;
    RECT rcOut;

    cout << "=== Move to zero rect" << endl;
    setRect(rcIn, zeroRect);
    item.moveTo(rcIn);
    item.getRect(rcOut);
    EXPECT_TRUE(RECT_EQ(rcOut, rcIn));

    cout << "=== Move to normal rect" << endl;
    setRect(rcIn, 1234, 2345, 3456, 4567);
    item.moveTo(rcIn);
    item.getRect(rcOut);
    EXPECT_TRUE(RECT_EQ(rcOut, rcIn));

    cout << "=== Move to max rect" << endl;
    setRect(rcIn, INT_MIN, INT_MIN, INT_MAX, INT_MAX);
    item.moveTo(rcIn);
    item.getRect(rcOut);
    EXPECT_TRUE(RECT_EQ(rcOut, rcIn));

    cout << "=== Move to invalid rect 1" << endl;
    setRect(rcIn, 3456, 1234, 2345, 4567);
    item.moveTo(rcIn);
    item.getRect(rcOut);
    EXPECT_TRUE(VALID_RECT(rcOut));

    cout << "=== Move to invalid rect 2" << endl;
    setRect(rcIn, 1234, 4567, 3456, 2345);
    item.moveTo(rcIn);
    item.getRect(rcOut);
    EXPECT_TRUE(VALID_RECT(rcOut));

    cout << "=== Move to invalid rect 3" << endl;
    setRect(rcIn, 3456, 4567, 1234, 2345);
    item.moveTo(rcIn);
    item.getRect(rcOut);
    EXPECT_TRUE(VALID_RECT(rcOut));
}

TEST (GraphicsView_Item_Test, moveToPoint_Test) {
    MyItem item;
    POINT pIn;
    POINT pOut;
    RECT  rcOut;

    cout << "=== Move to zero point" << endl;
    setPoint(pIn, zeroPoint);
    item.moveTo(pIn);
    item.getCenterPoint(pOut);
    EXPECT_TRUE(POINT_EQ(pOut, pIn));

    cout << "=== Move to normal point" << endl;
    setPoint(pIn, 1234, 2345);
    item.moveTo(pIn);
    item.getCenterPoint(pOut);
    EXPECT_TRUE(POINT_EQ(pOut, pIn));

    cout << "=== Move to min point" << endl;
    setPoint(pIn, INT_MIN, INT_MIN);
    item.moveTo(pIn);
    item.getRect(rcOut);
    EXPECT_TRUE(VALID_RECT(rcOut));

    cout << "=== Move to max point" << endl;
    setPoint(pIn, INT_MAX, INT_MAX);
    item.moveTo(pIn);
    item.getRect(rcOut);
    EXPECT_TRUE(VALID_RECT(rcOut));
}

TEST (GraphicsView_Item_Test, isIn_Test) {
    MyItem item;
    RECT rcIn;
    RECT rcOut;
    
    cout << "=== Is in bigger rect" << endl;
    setRect(rcIn, 0, 0, 10, 10);
    item.moveTo(rcIn);
    setRect(rcOut, 0, 0, 20, 20);
    EXPECT_TRUE(item.isIn(rcOut));

    cout << "=== Is in same rect" << endl;
    setRect(rcIn, 0, 0, 10, 10);
    item.moveTo(rcIn);
    setRect(rcOut, 0, 0, 10, 10);
    EXPECT_TRUE(item.isIn(rcOut));

    cout << "=== Is in smaller rect" << endl;
    setRect(rcIn, 0, 0, 10, 10);
    item.moveTo(rcIn);
    setRect(rcOut, 0, 0, 9, 9);
    EXPECT_TRUE(item.isIn(rcOut));

    cout << "=== Is in corossed rect" << endl;
    setRect(rcIn, 0, 0, 10, 10);
    item.moveTo(rcIn);
    setRect(rcOut, -1, -1, 9, 9);
    EXPECT_TRUE(item.isIn(rcOut));
}

TEST (GraphicsView_Item_Test, setScale_Test) {
    MyItem item;

    item.setScale(123.456f);
    EXPECT_EQ(123.456f, item.scale());

    item.setScale(0.0f);
    EXPECT_EQ(0.0f, item.scale());
}

TEST (GraphicsView_Item_Test, setTransparency_Test) {
    MyItem item;

    item.setTransparency(123.456f);
    EXPECT_EQ(123.456f, item.transparency());

    item.setTransparency(0.0f);
    EXPECT_EQ(0.0f, item.transparency());
}

TEST (GraphicsView_Scene_Test, initValue_Test) {
    Scene scene;

    EXPECT_TRUE(scene.items().empty());
    EXPECT_EQ(NULL, scene.getFocusItem());
}

TEST (GraphicsView_Scene_Test, addItem_Test) {
    Scene scene;
    Item *pItem = new MyItem();

    scene.addItem(pItem);
    EXPECT_EQ(pItem, scene.items().back());

    scene.addItem(NULL);
    EXPECT_EQ(NULL, scene.items().back());
}

TEST (GraphicsView_Scene_Test, removeItem_Test) {
    Scene scene;
    Item *pItem = new MyItem();

    scene.addItem(pItem);
    scene.removeItem(pItem);
    EXPECT_TRUE(scene.items().empty());

    scene.addItem(NULL);
    scene.removeItem(NULL);
    EXPECT_TRUE(scene.items().empty());
}

TEST (GraphicsView_Scene_Test, itemAt_Test) {
    Scene scene;
    Item *pItem1 = new MyItem();
    Item *pItem2 = new MyItem();
    Item *pItem3 = new MyItem();
    Item *pItem4 = new MyItem();
    RECT rc;
    POINT p1, p2, p3, p4;

    setRect(rc, 10, 10, 20, 20);
    pItem1->moveTo(rc);
    pItem2->moveTo(rc);
    pItem3->moveTo(rc);

    setPoint(p1, -10, -10);
    pItem1->moveTo(p1);
    scene.addItem(pItem1);

    setPoint(p2, -10, 10);
    pItem2->moveTo(p2);
    scene.addItem(pItem2);

    setPoint(p3, 10, -10);
    pItem3->moveTo(p3);
    scene.addItem(pItem3);

    setPoint(p4, 10, 10);
    pItem4->moveTo(p4);
    scene.addItem(pItem4);

    EXPECT_EQ(pItem1, scene.itemAt(p1));
    EXPECT_EQ(pItem2, scene.itemAt(p2));
    EXPECT_EQ(pItem3, scene.itemAt(p3));
    EXPECT_EQ(pItem4, scene.itemAt(p4));
}

TEST (GraphicsView_Scene_Test, focusItem_Test) {
    Scene scene;
    Item *pItem = new MyItem();

    scene.setFocusItem(NULL);
    EXPECT_EQ(NULL, scene.getFocusItem());

    scene.setFocusItem(pItem);
    EXPECT_EQ(pItem, scene.getFocusItem());
}

TEST (GraphicsView_Scene_Test, items_Test) {
    Scene scene;

    for (int i = 0; i < 10; ++i) {
        scene.addItem(new MyItem());
    }
    EXPECT_EQ(10, scene.items().size());
}

static int MyProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            break;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static HWND createHWND()
{
    HWND hwnd;

    MAINWINCREATE CreateInfo;

#ifdef  _MGRM_PROCESSES
    JoinLayer (NAME_DEF_LAYER, "activity", 0, 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Test Graphics View";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MyProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 320;
    CreateInfo.by = 240;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hwnd = CreateMainWindow (&CreateInfo);
    return hwnd;
}

class MyView : public View {
    public:
        static MyView* createMyView(Scene* pScene, HWND hwnd, bool doubleBuffer=false) {
            return new MyView(pScene, hwnd);
        }
        static void destoryMyView(MyView* pView) {
            delete pView;
        }
        const RECT& getRect() const {
            return m_rc;
        }
    protected:

        MyView(Scene* pScene, HWND hwnd, bool doubleBuffer=false) : View(pScene, hwnd, doubleBuffer) {}
};

static Scene* pScene = NULL;
static MyView* pView = NULL;

TEST (GraphicsView_View_Test, moveTo_getRect_Test) {
    RECT rcIn;
    RECT rcOut;

    setRect(rcIn, zeroRect);
    pView->moveTo(rcIn);
    rcOut = pView->getRect();
    EXPECT_TRUE(RECT_EQ(rcOut, rcIn));

    setRect(rcIn, 123, 234, 345, 456);
    pView->moveTo(rcIn);
    rcOut = pView->getRect();
    EXPECT_TRUE(RECT_EQ(rcOut, rcIn));
}

TEST (GraphicsView_View_Test, SceneToView_POINT_Test) {
    POINT pIn;
    POINT pOut;
    POINT pExpect;
    RECT rc = pView->getRect();

    setPoint(pIn, zeroPoint);
    setPoint(pExpect, pIn.x - rc.left, pIn.y - rc.top);
    pView->SceneToView(pIn, pOut);
    EXPECT_TRUE(POINT_EQ(pOut, pExpect));

    setPoint(pIn, 123, 234);
    setPoint(pExpect, pIn.x - rc.left, pIn.y - rc.top);
    pView->SceneToView(pIn, pOut);
    EXPECT_TRUE(POINT_EQ(pOut, pExpect));
}

TEST (GraphicsView_View_Test, ViewToScene_POINT_Test) {
    POINT pIn;
    POINT pOut;
    POINT pExpect;
    RECT rc = pView->getRect();

    setPoint(pIn, zeroPoint);
    setPoint(pExpect, pIn.x + rc.left, pIn.y + rc.top);
    pView->ViewToScene(pIn, pOut);
    EXPECT_TRUE(POINT_EQ(pOut, pExpect));

    setPoint(pIn, 123, 234);
    setPoint(pExpect, pIn.x + rc.left, pIn.y + rc.top);
    pView->ViewToScene(pIn, pOut);
    EXPECT_TRUE(POINT_EQ(pOut, pExpect));
}

TEST (GraphicsView_View_Test, SceneToView_RECT_Test) {
    RECT rcIn;
    RECT rcOut;
    RECT rcExpect;
    RECT mRc = pView->getRect();

    setRect(rcIn, zeroRect);
    setRect(rcExpect, 
            rcIn.left - mRc.left, rcIn.top - mRc.top, 
            rcIn.right - mRc.left, rcIn.bottom - mRc.top);
    pView->SceneToView(rcIn, rcOut);
    EXPECT_TRUE(RECT_EQ(rcOut, rcExpect));

    setRect(rcIn, 123, 234, 345, 456);
    setRect(rcExpect, 
            rcIn.left - mRc.left, rcIn.top - mRc.top, 
            rcIn.right - mRc.left, rcIn.bottom - mRc.top);
    pView->SceneToView(rcIn, rcOut);
    EXPECT_TRUE(RECT_EQ(rcOut, rcExpect));
}

TEST (GraphicsView_View_Test, ViewToScene_RECT_Test) {
    RECT rcIn;
    RECT rcOut;
    RECT rcExpect;
    RECT mRc = pView->getRect();

    setRect(rcIn, zeroRect);
    setRect(rcExpect, 
            rcIn.left + mRc.left, rcIn.top + mRc.top, 
            rcIn.right + mRc.left, rcIn.bottom + mRc.top);
    pView->ViewToScene(rcIn, rcOut);
    EXPECT_TRUE(RECT_EQ(rcOut, rcExpect));

    setRect(rcIn, 123, 234, 345, 456);
    setRect(rcExpect, 
            rcIn.left + mRc.left, rcIn.top + mRc.top, 
            rcIn.right + mRc.left, rcIn.bottom + mRc.top);
    pView->ViewToScene(rcIn, rcOut);
    EXPECT_TRUE(RECT_EQ(rcOut, rcExpect));
}

TEST (GraphicsView_View_Test, bgColor_Test) {

    gal_pixel red = RGBA2Pixel(HDC_SCREEN, 0xff, 0x00, 0x00, 0xff);
    gal_pixel green = RGBA2Pixel(HDC_SCREEN, 0x00, 0xff, 0x00, 0xff);
    gal_pixel blue = RGBA2Pixel(HDC_SCREEN, 0x00, 0x00, 0xff, 0xff);

    pView->setBgColor(red);
    EXPECT_EQ(red, pView->getBgColor());

    pView->setBgColor(green);
    EXPECT_EQ(green, pView->getBgColor());

    pView->setBgColor(blue);
    EXPECT_EQ(blue, pView->getBgColor());
}

static void eraseBgCallback(HDC hdc, const RECT& rect, void* param) {
}

TEST (GraphicsView_View_Test, setEraseBackgroundCallback_Test) {
    pView->setEraseBackgroundCallback(eraseBgCallback, NULL);
}

class MyEnvironment : public testing::Environment
{
    public:
        virtual void SetUp()
        {
            std::cout << "Testing Environment Setup ..." << std::endl;

            bzero(&zeroRect, sizeof(zeroRect));
            bzero(&zeroPoint, sizeof(zeroPoint));

            pScene = new Scene();
            pView = MyView::createMyView(pScene, g_hwnd);

            std::cout << "Testing Environment Setup OK." << std::endl;
        }
        virtual void TearDown()
        {
            std::cout << "Testing Environment Tear Down ..." << std::endl;
            delete pScene;
            MyView::destoryMyView(pView);
            std::cout << "Testing Environment Tear Down OK." << std::endl;
        }
};

int MiniGUIMain(int argc, const char* argv[]) {

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif
    g_hwnd = createHWND();

    testing::AddGlobalTestEnvironment(new MyEnvironment);
    ::testing::InitGoogleTest(&argc, (char **)argv);
    if (RUN_ALL_TESTS() < 0) {
        return -1;
    }

    SendMessage(g_hwnd, MSG_CLOSE, 0, 0);
    MainWindowThreadCleanup (g_hwnd);
    SendNotifyMessage(HWND_DESKTOP, MSG_ENDSESSION, 0, 0);

    return 0;
}
