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
#include <string.h>

#include <chipmunk/chipmunk.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#if 1
#define RED MakeRGBA(0xff, 0, 0, 0xff)
#define BLUE MakeRGBA(0, 0xff, 0, 0xff)
#define GREEN MakeRGBA(0, 0, 0xff, 0xff)
HWND g_hwnd;

cpSpace *space;

static void updateFloor(void *obj, void *data) {
    cpShape *shape = (cpShape*)obj;
    HDC hdc = (HDC)data;
    DWORD color = (DWORD)shape->data;

    cpVect a, b;
    a = cpSegmentShapeGetA(shape);
    b = cpSegmentShapeGetB(shape);

    // printf("hdc1 = %x\n", hdc);
    SetPenColor(hdc, RGBA2Pixel(hdc, GetRValue(color), GetGValue(color), GetBValue(color), GetAValue(color)));
    SetPenWidth(hdc, 2 * cpSegmentShapeGetRadius(shape));
    LineEx(hdc,
            shape->body->p.x + a.x, 480 - (shape->body->p.y + a.y),
            shape->body->p.x + b.x, 480 - (shape->body->p.y + b.y));
            /*
            shape->body->p.x + a.x,
            480 - (shape->body->p.y + a.y + cpSegmentShapeGetRadius(shape)));
            */
}

static void updateBall(void *obj, void *data) {
    cpShape *shape = (cpShape*)obj;
    HDC hdc = (HDC)data;
    DWORD color = (DWORD)shape->data;

    // printf("hdc2 = %x\n", hdc);
    int size = cpCircleShapeGetRadius(shape);
    /*
    MoveWindow(control,
            shape->body->p.x - cpCircleShapeGetRadius(shape),
            480 - (shape->body->p.y + cpCircleShapeGetRadius(shape)),
            size*2, size*2, FALSE);
            */

    SetBrushColor(hdc, RGBA2Pixel(hdc, GetRValue(color), GetGValue(color), GetBValue(color), GetAValue(color)));
    FillCircle(hdc, shape->body->p.x, 480 - shape->body->p.y, cpCircleShapeGetRadius(shape));
}

static void updateConstraint(cpConstraint *constraint, void *data) {
    HDC hdc = (HDC)data;
    const cpConstraintClass *klass = constraint->klass;

    cpBody *body_a = constraint->a;
    cpBody *body_b = constraint->b;

    if (klass == cpDampedSpringGetClass()) {
        cpDampedSpring *spring = (cpDampedSpring *)constraint;
        cpVect a = cpvadd(body_a->p, cpvrotate(spring->anchr1, body_a->rot));
        cpVect b = cpvadd(body_b->p, cpvrotate(spring->anchr2, body_b->rot));

        SetPenColor(hdc, RGB2Pixel(hdc, 0xa0, 0xa0, 0xa0));
        SetPenWidth(hdc, 5);
        LineEx(hdc, a.x, 480-a.y,
                b.x, 480-b.y);
    }
}

static void onTimer(void){
    cpSpaceStep(space, 1.0f/5.0f);  
    
    RECT rc;
    HDC hdc;
    HDC clientdc = GetClientDC(g_hwnd);
    HDC memdc = CreateCompatibleDC(clientdc);

    hdc = memdc;

    GetClientRect(g_hwnd, &rc);

    SetBrushColor(hdc, RGBA2Pixel(hdc, 0x8f, 0x8f, 0x8f, 0xff));
    FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));

    /*
    SetPenColor(hdc, RGBA2Pixel(hdc, 0xff, 0, 0, 0xff));
    SetPenWidth(hdc, 10);
    LineEx(hdc, 0, 10, 10, 10);
    */
    
    cpSpaceHashEach(space->staticShapes, &updateFloor, hdc);
    cpSpaceHashEach(space->activeShapes, &updateBall, hdc);

    cpArray *constraints = space->constraints;
    int i, count;
    for(i=0, count=constraints->num; i<count; i++){
        updateConstraint((cpConstraint *)constraints->arr[i], hdc);
    }

    BitBlt(memdc, 0, 0, 0, 0, clientdc, 0, 0, 0);
    DeleteMemDC(memdc);
    ReleaseDC(clientdc);
}

static cpFloat springForce(cpConstraint *spring, cpFloat dist)
{   
    cpFloat clamp = 20.0f;
    return cpfclamp(cpDampedSpringGetRestLength(spring) - dist, -clamp, clamp)*cpDampedSpringGetStiffness(spring);
}

static void setupChipmunk(void) {
    cpBody *floorBody;
    cpBody *ballBody;

    cpInitChipmunk();
    space = cpSpaceNew();

    space->gravity = cpv(0, -100);

    // floor
    {
        floorBody = cpBodyNew(INFINITY, INFINITY);
        floorBody->p = cpv(160, 430);

        cpShape *floorShape = cpSegmentShapeNew(floorBody, cpv(-100, 10), cpv(100, -10), 10.0f);
        floorShape->e = 0.8; floorShape->u = 0.1; floorShape->collision_type = 1;
        floorShape->data = GREEN;
        cpSpaceAddStaticShape(space, floorShape);
    }

    // ball 1
    if (1){
        ballBody = cpBodyNew(100, INFINITY);  
        ballBody->p = cpv(160, 400);  
        cpSpaceAddBody(space, ballBody);  

        cpShape *ballShape = cpCircleShapeNew(ballBody, 30, cpvzero);  
        ballShape->e = 0.9; // Elasticity  
        ballShape->u = 0.8; // Friction  
        ballShape->collision_type = 1; // Collisions are grouped by types  
        ballShape->data = RED;
        cpSpaceAddShape(space, ballShape);  
    }

    // cpConstraint
    if (1){
        cpConstraint *spring = cpDampedSpringNew(floorBody, ballBody, cpvzero, cpvzero, 0.0, 100.0, 20.5);
        // cpDampedSpringSetSpringForceFunc(spring, springForce);
        cpSpaceAddConstraint(space, spring);
    }

    // ball 2
    if (1){
        cpBody *ballBody = cpBodyNew(200, INFINITY);  
        ballBody->p = cpv(180, 300);  
        cpSpaceAddBody(space, ballBody);  

        cpShape *ballShape = cpCircleShapeNew(ballBody, 30, cpvzero);  
        ballShape->e = 0.5; // Elasticity  
        ballShape->u = 0.1; // Friction  
        ballShape->collision_type = 1; // Collisions are grouped by types  
        ballShape->data = BLUE;
        cpSpaceAddShape(space, ballShape);  
    }

    // floor
    {
        cpBody *floorBody = cpBodyNew(INFINITY, INFINITY);
        floorBody->p = cpv(160, 50);

        cpShape *floorShape = cpSegmentShapeNew(floorBody, cpv(-100, 30), cpv(100, 0), 10.0f);
        floorShape->e = 1.8; floorShape->u = 5.1; floorShape->collision_type = 1;
        floorShape->data = GREEN;
        cpSpaceAddStaticShape(space, floorShape);
    }
}
#endif

static int ChipmunkWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            g_hwnd = hWnd;
            SetTimer(hWnd, 1, 100);

            setupChipmunk();
            break;
        case MSG_TIMER:
            if (wParam == 1) {
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 2, 20);
            }else{
                onTimer();
                if (0){
                    HDC hdc = HDC_SCREEN;
                    SetBrushColor(hdc, RGBA2Pixel(hdc, 0x0f, 0x8f, 0x8f, 0xff));
                    printf("...\n");
                    // SetBrushColor(hdc, RGBA2Pixel(hdc, 0xff, 0, 0, 0xff));
                    FillBox(hdc, 0, 0, 1000, 1000);
                }
            }
            break;
            /*
        case MSG_PAINT:
            return 0;
            */
        case MSG_ERASEBKGND:
            return 0;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , argv[0], 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = argv[0];
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = ChipmunkWinProc;
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
