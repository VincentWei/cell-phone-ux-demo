#include <assert.h>

#include "physics-animation.h"

#define PRECISION_MS 50

typedef struct {
    MGEFF_ANIMATION m_animation;
    cpSpace *m_space;
    int m_lastTime;
    void *m_context;
    int (*cb_onCalc)(MGEFF_ANIMATION anim, cpSpace *space, void *context);
    void (*cb_onDraw)(MGEFF_ANIMATION anim, cpSpace *space, void *context);
} phyanim_t;

#ifdef PHYANIM_DEBUG
static HWND s_debughwnd;
#define MSG_USER_CHIPMUNK (MSG_USER+1)

/* Logic to Device */
static void LtoD(int lx, int ly, int w, int h, int *dx, int *dy) {
    *dx = 100 + lx;
    *dy = h + 50 - ly;
}

static void drawSegmentShape(cpShape *shape, void *data) {
    HDC hdc = (HDC)data;
    cpVect a, b;
    int W = GetGDCapability(hdc, GDCAP_MAXX) + 1;
    int H = GetGDCapability(hdc, GDCAP_MAXY) + 1;
    int x1, y1, x2, y2;

    a = cpSegmentShapeGetA(shape);
    b = cpSegmentShapeGetB(shape);

    SetPenColor(hdc, RGB2Pixel(hdc, 0x30, 0x30, 0x30));
    SetPenWidth(hdc, 2 * cpSegmentShapeGetRadius(shape));
    LtoD(shape->body->p.x + a.x, shape->body->p.y + a.y,
            W, H, &x1, &y1);
    LtoD(shape->body->p.x + b.x, shape->body->p.y + b.y,
            W, H, &x2, &y2);
    LineEx(hdc, x1, y1, x2, y2);
}

static void drawPolyShape(cpShape *shape, void *data) {
    HDC hdc = (HDC)data;
    int W = GetGDCapability(hdc, GDCAP_MAXX) + 1;
    int H = GetGDCapability(hdc, GDCAP_MAXY) + 1;
    int i;
    int count = cpPolyShapeGetNumVerts(shape);
    cpBody *body = shape->body;
    int x1, y1, x2, y2;

    SetPenColor(hdc, RGB2Pixel(hdc, 0x00, 0xff, 0x00));
    SetPenWidth(hdc, 2);
    for (i=0; i<count; ++i) {
        cpVect v1 = cpPolyShapeGetVert(shape, i);
        cpVect v2 = cpPolyShapeGetVert(shape, (i+1)%count);
        LtoD(body->p.x + v1.x, body->p.y + v1.y,
                W, H, &x1, &y1);
        LtoD(body->p.x + v2.x, body->p.y + v2.y,
                W, H, &x2, &y2);
        LineEx(hdc, x1, y1, x2, y2);
    }
}

static void drawBallShape(cpShape *shape, void *data) {
    HDC hdc = (HDC)data;
    int W = GetGDCapability(hdc, GDCAP_MAXX) + 1;
    int H = GetGDCapability(hdc, GDCAP_MAXY) + 1;
    int x, y;

    SetBrushColor(hdc, RGB2Pixel(hdc, 0xff, 0, 0));
    LtoD(shape->body->p.x, shape->body->p.y,
            W, H, &x, &y);
    FillCircle(hdc, x, y, cpCircleShapeGetRadius(shape));
}

static void drawShapes(void *obj, void *data) {
    cpShape *shape = (cpShape *)obj;

    switch(shape->klass->type){
        case CP_CIRCLE_SHAPE:
            drawBallShape(shape, data);
            break;
        case CP_SEGMENT_SHAPE:
            drawSegmentShape(shape, data);
            break;
        case CP_POLY_SHAPE:
            drawPolyShape(shape, data);
            break;
        default:
            assert(0);
            break;
    }
}

static void updateConstraint(cpConstraint *constraint, void *data) {
    HDC hdc = (HDC)data;
    const cpConstraintClass *klass = constraint->klass;
    int W = GetGDCapability(hdc, GDCAP_MAXX) + 1;
    int H = GetGDCapability(hdc, GDCAP_MAXY) + 1;
    int x1, y1, x2, y2;

    cpBody *body_a = constraint->a;
    cpBody *body_b = constraint->b;

    if (klass == cpDampedSpringGetClass()) {
        cpDampedSpring *spring = (cpDampedSpring *)constraint;
        cpVect a = cpvadd(body_a->p, cpvrotate(spring->anchr1, body_a->rot));
        cpVect b = cpvadd(body_b->p, cpvrotate(spring->anchr2, body_b->rot));

        SetPenColor(hdc, RGB2Pixel(hdc, 0xa0, 0xa0, 0xa0));
        SetPenWidth(hdc, 5);
        LtoD(a.x, a.y, W, H, &x1, &y1);
        LtoD(b.x, b.y, W, H, &x2, &y2);
        LineEx(hdc, x1, y1, x2, y2);
    }
}

static int ChipmunkWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_USER_CHIPMUNK:
            {
                cpSpace *space = (cpSpace *)wParam;
                cpArray *constraints;
                int i, count;
                RECT rc;
                HDC hdc;
                HDC clientdc = GetClientDC(hWnd);
                HDC memdc = CreateCompatibleDC(clientdc);

                hdc = memdc;

                GetClientRect(hWnd, &rc);

                SetBrushColor(hdc, RGBA2Pixel(hdc, 0x8f, 0x8f, 0x8f, 0xff));
                FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));

                cpSpaceHashEach(space->staticShapes, &drawShapes, (void *)hdc);
                cpSpaceHashEach(space->activeShapes, &drawShapes, (void *)hdc);

                constraints = space->constraints;
                for(i=0, count=constraints->num; i<count; i++){
                    updateConstraint((cpConstraint *)constraints->arr[i], (void *)hdc);
                }

                BitBlt(memdc, 0, 0, 0, 0, clientdc, 0, 0, -1);
                DeleteMemDC(memdc);
                ReleaseDC(clientdc);
            }
            return 0;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static HWND create_debug_hwnd(void) {
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_TOPMOST; // WS_EX_AUTOSECONDARYDC;
    CreateInfo.spCaption = "Physics Animation Debug";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = ChipmunkWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = g_rcScr.bottom - 150;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    return CreateMainWindow (&CreateInfo);
}

static void _phyanim_debug(cpSpace *space) {
    if (! s_debughwnd) {
        s_debughwnd = create_debug_hwnd();
    }
    SendMessage(s_debughwnd, MSG_USER_CHIPMUNK, (WPARAM)space, 0);
}

#else
#   define _phyanim_debug(x) /* NULL */
#endif /* PHYANIM_DEBUG */

static void s_step(MGEFF_ANIMATION handle, void* target, int id_notused, void* value) {
    phyanim_t *phy = (phyanim_t *)target;
    int now = *((int *)value);
    int i;
    int stop = 0;

    for (i=phy->m_lastTime; i<now; ++i) {
        cpSpaceStep(phy->m_space, PRECISION_MS / 1000.0f);
        if (phy->cb_onCalc) {
            if (phy->cb_onCalc(handle, phy->m_space, phy->m_context) != 0) {
                stop = 1;
                break;
            }
        }
    }
    if (now > phy->m_lastTime) {
        _phyanim_debug(phy->m_space);
        if (phy->cb_onDraw) {
            phy->cb_onDraw(handle, phy->m_space, phy->m_context);
        }
        phy->m_lastTime = now;
    }
    if (stop) {
        mGEffAnimationStop(handle);
    }
}

MGEFF_ANIMATION phyanim_create(cpSpace *space, void *context,
        int (*onCalc)(MGEFF_ANIMATION anim, cpSpace *space, void *context),
        void (*onDraw)(MGEFF_ANIMATION anim, cpSpace *space, void *context)) {
    MGEFF_ANIMATION anim;
    phyanim_t *phy;
    int startValue = 0;
    int endValue = 1000 * 10 / PRECISION_MS; /* 10 sec. */

    phy = (phyanim_t *)calloc(1, sizeof(*phy));
    phy->m_space = space;
    phy->m_context = context;
    phy->cb_onCalc = onCalc;
    phy->cb_onDraw = onDraw;
    phy->m_lastTime = -1;

    anim = mGEffAnimationCreate(phy, s_step, 0, MGEFF_INT);
    mGEffAnimationSetStartValue(anim, &startValue);
    mGEffAnimationSetEndValue(anim, &endValue);
    mGEffAnimationSetDuration(anim, 1000 * 10);
    mGEffAnimationSetCurve(anim, Linear);
    /* TODO: set Finish callback to destroy phy */

    return anim;
}

void phyanim_destroy(MGEFF_ANIMATION animation) {
    phyanim_t *phy = (phyanim_t *)mGEffAnimationGetTarget(animation);
    free(phy);
}

cpSpace *phyanim_getspace(MGEFF_ANIMATION animation) {
    phyanim_t *phy = (phyanim_t *)mGEffAnimationGetTarget(animation);
    return phy->m_space;
}
