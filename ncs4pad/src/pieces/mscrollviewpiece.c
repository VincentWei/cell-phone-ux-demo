//#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)
#define dbg() /* NULL */
//#define PRINT_RECT(s, rc) printf("[%s]: %d %d %d %d\n", s, (rc)->left, (rc)->top, (rc)->right, (rc)->bottom)
#define PRINT_RECT(s, rc) /* NULL */
#if 0
#include <sys/times.h>
#   define LOG_TIME(prefix) printf("%s%s(%d) %06u\n", prefix, __FUNCTION__, __LINE__, ((unsigned int)times(NULL)) % 1000000u)
#else
#   define LOG_TIME(prefix) /* NULL */
#endif

#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mpanelpiece.h"
#include "mtransroundpiece.h"
#include "mscrollviewpiece.h"
#include "mspeedmeter.h"
#include "physics-animation.h"
#include "chipmunk-utils.h"

#define ENABLE_CACHE_BY_DEFAULT 1
#define SCROLLBAR_TIMEOUT_MS (500)
#define PRESS_TIMEOUT (10)
#define R (80)
#define MIN_SPEED (0.01f)
#define MAX_CROSS_BORDER (100)
#define CLICK_TIMEOUT (8)
#define CLICK_MICRO_MOVEMENT (8)

struct scroll_phy_ctx {
    cpShape *floor;
    cpShape *movingBlock;
    cp_baffle_board_t *board1;
    cp_baffle_board_t *board2;
};

static void phy_ctx_destroy(cpSpace *space, struct scroll_phy_ctx *ctx) {
    destroy_static_shape(space, ctx->floor);
    destroy_shape(space, ctx->movingBlock);
    destroy_baffle_board(space, ctx->board1);
    destroy_baffle_board(space, ctx->board2);

    free(ctx);
}

static mPieceItem* s_getContent(mScrollViewPiece* self) 
{
    if (self->m_content) {
        return self->m_content;
    }else{
        mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
        mPieceItem *item;
       
        while ((item = _c(iter)->next(iter))) {
            if (item != self->m_scrollbar) {
                self->m_content = item;
                break;
            }
        }
        DELETE(iter);
        return self->m_content;
    }
}

static int s_canScroll(mScrollViewPiece *self) {
    RECT rc;
    int w, W;
    mPieceItem* child = s_getContent(self);
    mHotPiece* piece = _c(child)->getPiece(child);

    _c(piece)->getRect(piece, &rc);
    W = RECTH(rc);
    _c(self)->getViewport(self, &rc);
    w = RECTH(rc);

    return (w < W);
}

static int s_onCalc(MGEFF_ANIMATION anim, cpSpace *space, void *_self) {
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    mPieceItem* child = s_getContent(self);
    mHotPiece* piece = _c(child)->getPiece(child);
    RECT contentRc;
    RECT viewPort;
    struct scroll_phy_ctx *ctx = (struct scroll_phy_ctx *)self->m_phy_ctx;
    cpShape *block = ctx->movingBlock;
    float v = block->body->v.x;
    float p1, p2;
    int L;
    int needStop = 0;

    assert(cpPolyShapeGetNumVerts(block) == 4);
    p1 = cpvadd(cpPolyShapeGetVert(block, 0), block->body->p).x;
    p2 = cpvadd(cpPolyShapeGetVert(block, 2), block->body->p).x;

    _c(piece)->getRect(piece, &contentRc);
    L = RECTH(contentRc);
    _c(self)->getViewport(self, &viewPort);

    switch (self->m_movingStatus) {
        case 0:
            if (v > MIN_SPEED && p2 > L) {
                self->m_movingStatus = 1;
            }else if (v < -MIN_SPEED && p1 < 0) {
                self->m_movingStatus = -1;
            }
            break;
        case -1:
            if (v > 0 && p1 >= 0) {
                cpBodySleep(block->body);
                cpBodySetPos(block->body, cpv(0, block->body->p.y));
                self->m_movingStatus = 0;
                needStop = 1;
            }
            break;
        case 1:
            if (v < 0 && p2 <= L) {
                cpBodySleep(block->body);
                cpBodySetPos(block->body, cpv(L-(p2-p1), block->body->p.y));
                self->m_movingStatus = 0;
                needStop = 1;
            }
            break;
        default:
            assert(0);
            break;
    }

    /* XXX: Recover from a sad situation */
    if (self->m_movingStatus != 0) {
        cp_baffle_board_t *board;
        cpConstraint *spring;

        board = (self->m_movingStatus < 0 ? ctx->board1 : ctx->board2);
        spring = board->spring;
        if (spring->a->p.y != spring->b->p.y) {
            cpBodySleep(block->body);
            if (self->m_movingStatus < 0) {
                cpBodySetPos(block->body, cpv(0, block->body->p.y));
            }else{
                cpBodySetPos(block->body, cpv(L-(p2-p1), block->body->p.y));
            }
            self->m_movingStatus = 0;
            needStop = 1;
            // printf("recover\n");
        }
    }

    /* TODO: An ugly workaround, fix me */
    if (v <= MIN_SPEED && v >= -MIN_SPEED) {
        if (self->m_movingStatus == 0) {
            needStop = 1;
        }else{
            if (p2 > L && p2 < L+10) {
                cpBodySleep(block->body);
                cpBodySetPos(block->body, cpv(L-(p2-p1), block->body->p.y));
                self->m_movingStatus = 0;
                needStop = 1;
            }else if (p1 < 0 && p1 > 0-10) {
                cpBodySleep(block->body);
                cpBodySetPos(block->body, cpv(0, block->body->p.y));
                self->m_movingStatus = 0;
                needStop = 1;
            }
        }
    }

    return (needStop ? -1 : 0);
}

static void s_onDraw(MGEFF_ANIMATION anim, cpSpace *space, void *_self) {
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    mPieceItem* child = s_getContent(self);
    cpVect p;
    struct scroll_phy_ctx *ctx = (struct scroll_phy_ctx *)self->m_phy_ctx;
    cpShape *block = ctx->movingBlock;

    //assert(block->klass->type == CP_POLY_SHAPE);
    assert(cpPolyShapeGetNumVerts(block) == 4);
    p = cpvadd(cpPolyShapeGetVert(block, 0), block->body->p);

    _c(self)->moveViewport(self, p.y-R, p.x);
    PanelPiece_update(_c(child)->getPiece(child), FALSE);
}

static cpSpace *s_setupSpace(mScrollViewPiece *self, float v_x, float v_y) {
    cpSpace *space;
    cpShape *shape;
    mPieceItem* child = s_getContent(self);
    mHotPiece* piece = _c(child)->getPiece(child);
    RECT rc;
    int w, W, x;
    int y_baseline;
    struct scroll_phy_ctx *ctx;

    //assert(self->m_phy_ctx == NULL);
    ctx = (struct scroll_phy_ctx *)calloc(1, sizeof(*ctx));

    _c(piece)->getRect(piece, &rc);
    W = RECTH(rc);
    _c(self)->getViewport(self, &rc);
    w = RECTH(rc);
    assert(w < W);

    y_baseline = rc.left + R;
    x = rc.top;
    assert(x>-1000 && x+w<W+1000);

    space = cpSpaceNew();
    space->gravity = cpv(0, -200);

    shape = create_floor(space, y_baseline, -1000, W+1000);
    shape->u = 1.0f;
    ctx->floor = shape;

    shape = create_block(space, x, y_baseline, x+w, y_baseline+R, 10);
    shape->u = 3.0f;
    ctx->movingBlock = shape;
    cpBodySetVel(shape->body, cpv(-v_y, 0));

    ctx->board1 = create_baffle_board(space, y_baseline, R, 2*MAX_CROSS_BORDER, MIN(0, shape->body->p.x), -2*MAX_CROSS_BORDER, 2000, 5);
    ctx->board2 = create_baffle_board(space, y_baseline, R, 2*MAX_CROSS_BORDER, MAX(W, rc.bottom), W+2*MAX_CROSS_BORDER, 2000, 5);
    if (rc.top < 0) {
        self->m_movingStatus = -1;
    }else if (rc.bottom > W) {
        self->m_movingStatus = 1;
    }else{
        self->m_movingStatus = 0;
    }
    self->m_phy_ctx = ctx;

    return space;
}

static void s_autoHideScrollbar(mScrollViewPiece *self, int hide) {
    hide = (hide ? 1 : 0);
    if (self->m_bScrollbarAutoHided != hide) {
        RECT rc;

        self->m_bScrollbarAutoHided = hide;

        _c(self->m_scrollbar->piece)->getRect(self->m_scrollbar->piece, &rc);
        PanelPiece_invalidatePiece((mHotPiece *)self, &rc);
    }
}

static BOOL s_hideScrollBar(HWND _self, int id, DWORD tickCount) {
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    s_autoHideScrollbar(self, TRUE);
    KillTimer((HWND)self, ((int)self) + 1);
    return TRUE;
}

static void s_finish_cb(MGEFF_ANIMATION handle) {
    mScrollViewPiece *self = (mScrollViewPiece *) mGEffAnimationGetContext(handle);

    {
        cpSpace *space = phyanim_getspace(handle);
        phy_ctx_destroy(space, (struct scroll_phy_ctx *)self->m_phy_ctx);
        cpSpaceFree(space);
    }

    phyanim_destroy(handle);
    self->m_animation = NULL;
    self->m_phy_ctx = NULL;
    SetTimerEx((HWND)self, ((int)self) + 1, SCROLLBAR_TIMEOUT_MS / 10, s_hideScrollBar);
}

static int s_viewOnMouseRelease(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    float v_x, v_y;
    cpSpace *space;

    if (s_canScroll(self)) {
        QueryMouseMoveVelocity(&v_x, &v_y);
        if (v_y > 10000.0f) {
            printf("v_y=%.2f, set to 10000 forcely\n", v_y);
            v_y = 10000.0f;
        }else if (v_y < -10000.0f) {
            printf("v_y=%.2f, set to -10000 forcely\n", v_y);
            v_y = -10000.0f;
        }
        // printf("v_y=%.2f\n", v_y);

        space = s_setupSpace(self, v_x, v_y);
        assert(self->m_animation == NULL);
        self->m_animation = phyanim_create(space, self, s_onCalc, s_onDraw); 
        mGEffAnimationSetContext(self->m_animation, self);
        mGEffAnimationSetFinishedCb(self->m_animation, s_finish_cb);
        mGEffAnimationAsyncRun(self->m_animation);
        mGEffAnimationSetProperty(self->m_animation, MGEFF_PROP_KEEPALIVE, 0);
    }
    return 0;
}

static int s_viewOnMouseMove(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    RECT rc;
    int x, y;

    x = LOSWORD(lParam);
    y = HISWORD(lParam);
    if (s_canScroll(self)) {
        _c(self)->getViewport(self, &rc);
        _c(self)->moveViewport(self, 
                rc.left + (self->m_oldMousePos.x - x) * self->m_ratioX,
                rc.top + (self->m_oldMousePos.y - y) * self->m_ratioY);
        s_autoHideScrollbar(self, FALSE);
    }
    self->m_oldMousePos.x = x;
    self->m_oldMousePos.y = y;
    return 0;
}

static void s_checkTimeout(mScrollViewPiece *self, mObject *owner) {
    if (! self->m_bTimedout) {
        self->m_bTimedout = (GetTickCount() - self->m_timePressed >= PRESS_TIMEOUT);
        if (self->m_bTimedout) {
            mPieceItem* child = s_getContent(self);
            mHotPiece* piece = _c(child)->getPiece(child);
            RECT viewPort;
            LPARAM lParam;

            // printf("timeout\n");
            _c(self)->getViewport(self, &viewPort);
            lParam = MAKELONG(self->m_pressMousePos.x + viewPort.left, self->m_pressMousePos.y + viewPort.top);
            _c(piece)->processMessage(piece, MSG_LBUTTONDOWN, 0, lParam, owner);
        }
    }
    KillTimer((HWND)self, (int)self);
}

static BOOL s_onTimer(HWND _self, int id, DWORD tickCount) {
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    // printf("Timer=%u\n", tickCount);
    s_checkTimeout(self, (mObject *)_c(self)->getOwner(self));
    return TRUE;
}

static int s_onMousePress(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    self->m_bPressed = TRUE;
    self->m_mouseFlag = 0;
    self->m_bTimedout = FALSE;
    self->m_timePressed = GetTickCount();
    self->m_oldMousePos.x = LOSWORD(lParam);
    self->m_oldMousePos.y = HISWORD(lParam);
    self->m_pressMousePos = self->m_oldMousePos;

    if (self->m_animation) {
        // printf("Cancel animation\n");
        mGEffAnimationStop(self->m_animation);
        self->m_animation = NULL;
        self->m_mouseFlag |= 0x02;
    }else{
        KillTimer((HWND)self, (int)self);
        // printf("Pressed=%u\n", self->m_timePressed);
        SetTimerEx((HWND)self, (int)self, PRESS_TIMEOUT, s_onTimer);
    }
    return 0;
}

static BOOL s_onMouseMoveLeft(mScrollViewPiece *self, mObject *owner) {
    float v_x, v_y;
    mPieceItem *child;
    mHotPiece *piece;
    RECT viewPort;
    LPARAM lParam;
    self->m_bMouseMoved = FALSE;
    QueryMouseMoveVelocity(&v_x, &v_y);
    if ((v_x < 0) && (-v_x > 10*abs(v_y))) {
        child = s_getContent(self);
        piece = _c(child)->getPiece(child);

        _c(self)->getViewport(self, &viewPort);
        lParam = MAKELONG(self->m_pressMousePos.x + viewPort.left, self->m_pressMousePos.y + viewPort.top);
        _c(piece)->processMessage(piece, MSG_LBUTTONDOWN, 0, lParam, owner);

        return TRUE;
    }
    return FALSE;
}

static int s_onMouseRelease(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    if (! self->m_bPressed) {
        return 0;
    }

    self->m_bPressed = FALSE;
    if ((self->m_mouseFlag & 0x02) == 0) {
        int flag;
        if ((self->m_mouseFlag & 0x01) == 0) {
            flag = 0;
        }else{
            if (GetTickCount() < self->m_timePressed + CLICK_TIMEOUT
                    && (ABS(LOSWORD(lParam) - self->m_pressMousePos.x) + ABS(HISWORD(lParam) - self->m_pressMousePos.y) < CLICK_MICRO_MOVEMENT)) {
                flag = 0;
            }else{
                flag = 1;
            }
            printf("***** release-press=%d, movement=%d\n",
                    GetTickCount() - self->m_timePressed, 
                    ABS(LOSWORD(lParam) - self->m_pressMousePos.x) + ABS(HISWORD(lParam) - self->m_pressMousePos.y));
        }
        if (flag == 0) {
            self->m_timePressed -= PRESS_TIMEOUT;
            s_checkTimeout(self, owner);
        }
    }

    if (self->m_bTimedout) {
        return -1;
    }
    else if (self->m_bMouseMoved) {
        if (s_canScroll(self))
            s_viewOnMouseRelease(_self, message, wParam, lParam, owner);
        if (s_onMouseMoveLeft(self, owner))
            return -1; /* Pass to the child */
    }
    return 0;
}

static int s_onMouseMove(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    mScrollViewPiece *self = (mScrollViewPiece *)_self;
    if (! self->m_bPressed) {
        return 0; /* Drop it */
    }

    self->m_bMouseMoved = TRUE;

    if (self->m_mouseFlag == 0) {
        self->m_mouseFlag |= 0x01;
        // printf("Moved=%u\n", GetTickCount());
        s_checkTimeout(self, owner);
    }

    if (self->m_bTimedout) {
        return -1; /* Pass to the child */
    }else{
        return s_viewOnMouseMove(_self, MSG_MOUSEMOVE, wParam, lParam, owner);
    }
}

static int s_onMouseMoveIn(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner){
    if (!((BOOL)wParam)) {
        mScrollViewPiece *self = (mScrollViewPiece *)_self;
        s_onMouseRelease(_self, MSG_LBUTTONUP, 0, 0, owner);
        //self->m_bMoved = FALSE;
        KillTimer((HWND)self, (int)self);
    }
    return 0;
}

static void s_removeCache(mScrollViewPiece *self) {
    if (self->m_cache != HDC_INVALID) {
        DeleteMemDC(self->m_cache);
        self->m_cache = HDC_INVALID;
    }
}

static void mScrollViewPiece_construct(mScrollViewPiece *self, DWORD addData) {
    Class(mPanelPiece).construct((mPanelPiece *)self, addData);

    cpInitChipmunk(); /* TODO */

    self->m_ratioX = 0.0f;
    self->m_ratioY = 1.0f;
    self->m_bNeedScrollBar = TRUE;
    self->m_bScrollbarAutoHided = TRUE;
    self->m_bPressed = FALSE;
    self->m_bMouseMoved = FALSE;
    self->m_mouseFlag = 0;
    self->m_bTimedout = FALSE;
    self->m_movingStatus = 0;
    self->m_phy_ctx = NULL;
    {
        mHotPiece *scrollbar;
        mItemIterator *iter;

        scrollbar = (mHotPiece *)NEWPIECE(mShapeTransRoundPiece);
        _c(scrollbar)->setProperty(scrollbar, NCSP_TRANROUND_BKCOLOR, MakeRGBA(0, 0, 0, 0x50));
        _c(scrollbar)->setProperty(scrollbar, NCSP_TRANROUND_RADIUS, 0);

        _c(self)->addContent(self, scrollbar, 0, 0);

        iter = _c(self->itemManager)->createItemIterator(self->itemManager);
        self->m_scrollbar = _c(iter)->next(iter);
        DELETE(iter);
    }

    /* Cache */
#if ENABLE_CACHE_BY_DEFAULT
    self->m_cachable = TRUE;
#else
    self->m_cachable = FALSE;
#endif
    self->m_cache = HDC_INVALID;
    memset(&self->m_contentDirtyRect, 0, sizeof(self->m_contentDirtyRect));

    _c(self)->appendEventHandler(self, MSG_LBUTTONDOWN, s_onMousePress);
    _c(self)->appendEventHandler(self, MSG_LBUTTONUP, s_onMouseRelease);
    _c(self)->appendEventHandler(self, MSG_MOUSEMOVE, s_onMouseMove);
    _c(self)->appendEventHandler(self, MSG_MOUSEMOVEIN, s_onMouseMoveIn);
}

static void mScrollViewPiece_destroy(mScrollViewPiece *self) {
    if (self->m_animation) {
        mGEffAnimationStop(self->m_animation);
    }
    assert(self->m_phy_ctx == NULL);

    KillTimer((HWND)self, (int)self);
    KillTimer((HWND)self, ((int)self)+1);

    s_removeCache(self);

    Class(mPanelPiece).destroy((mPanelPiece*)self);
}

static void mScrollViewPiece_enableCache(mScrollViewPiece *self, BOOL cachable) {
    if (self->m_cachable != cachable) {
        if (! cachable) {
            s_removeCache(self);
        }
        self->m_cachable = cachable;
    }
}

static void mScrollViewPiece_moveViewport(mScrollViewPiece *self, int x, int y) {
    mPieceItem* child = s_getContent(self);

    /* XXX: support vertical scrolling only */
    if (x != 0) {
        // assert(0);
        x = 0;
    }

    if (y < -MAX_CROSS_BORDER) {
        y = -MAX_CROSS_BORDER;
    }else{
        mHotPiece* piece = _c(child)->getPiece(child);
        RECT viewRc, bodyRc;
        int max;

        _c(piece)->getRect(piece, &bodyRc);
        _c(self)->getRect(self, &viewRc);
        max = MAX(0, RECTH(bodyRc) + MAX_CROSS_BORDER - RECTH(viewRc));
        if (y > max) {
            y = max;
        }
    }

    _c(self)->movePiece(self, _c(child)->getPiece(child), -x, -y);
}

static void mScrollViewPiece_getViewport(mScrollViewPiece *self, RECT *rc) {
    int w, h;
    mPieceItem* child = s_getContent(self);

    _c(self)->getRect(self, rc);
    w = RECTWP(rc);
    h = RECTHP(rc);
    SetRect(rc, -_c(child)->getX(child), -_c(child)->getY(child), -_c(child)->getX(child) + w, 
            -_c(child)->getY(child) + h);
}

static void s_drawScrollBar(mScrollViewPiece *self, HDC hdc, mObject *owner, DWORD add_data) {
    RECT sbRc;
    int l_content, l_view;
    float ratio;
    int w, h, space, total, offset;
    RECT viewRc, viewPort;
    RECT bodyRc;
    mPieceItem* child = s_getContent(self);
    mHotPiece* piece = _c(child)->getPiece(child);
    float magic;

    _c(self)->getRect(self, &viewRc);
    _c(self)->getViewport(self, &viewPort);
    _c(piece)->getRect(piece, &bodyRc);

    if (RECTH(bodyRc) <= 0) {
        // assert(0);
        return;
    }

    if (viewPort.top <= 0 && viewPort.bottom >= RECTH(bodyRc)) {
        return;
    }

    l_view = RECTH(viewRc);
    l_content = RECTH(bodyRc);
    offset = viewPort.top;
    magic = 1.0f * l_content / MAX_CROSS_BORDER * 2.0f;
    if (offset < 0) {
        l_content += -offset * magic;
        offset = 0;
    }else if (l_view + offset > l_content) {
        l_content += (l_view + offset - l_content) * magic;
        offset = l_content - l_view;
    }

    ratio = l_view * 1.0f / l_content;
    if (ratio >= 1.0f) {
        return;
    }

    space = MIN(l_view * 0.1f, 10);
    total = l_view - 2 * space;
    h = total * ratio;
    if (h <= 0) {
        assert(0);
        return;
    }
    w = 3;

    sbRc.left = viewRc.right - w - 4;
    sbRc.right = sbRc.left + w;
    sbRc.top = viewRc.top + space + total*offset/l_content;
    sbRc.bottom = sbRc.top + h;

    /*
    printf("offset=%d, l_view=%d, l_content=%d\n", offset, l_view, l_content);
    printf("sbRc=(%d,%d,%d,%d)\n", sbRc.left, sbRc.top, RECTW(sbRc), RECTH(sbRc));
    */

#if 1
    _c(self->m_scrollbar->piece)->setRect(self->m_scrollbar->piece, &sbRc);
    _c(self->m_scrollbar->piece)->paint(self->m_scrollbar->piece, hdc, owner, add_data);
#else
    SetBrushColor(hdc, RGBA2Pixel(hdc, 0xff, 0, 0, 0xff));
    FillBox(hdc, sbRc.left, sbRc.top, RECTW(sbRc), RECTH(sbRc));
#endif
}

static void mScrollViewPiece_showScrollBar(mScrollViewPiece *self, BOOL show) {
    self->m_bNeedScrollBar = show;
}

static void s_setChildClipRect(mPieceItem *item, const RECT *rc) {
    // printf("child clip rect = (%d,%d,%d,%d)\n", rc->left, rc->top, RECTWP(rc), RECTHP(rc));
    _c(item->piece)->setProperty(item->piece, NCSP_PANEL_CLIPRECT, (DWORD)rc);
}

static void s_updateCache(mScrollViewPiece *self, mPieceItem *child, const RECT *c_whole, const RECT *c_viewport, const RECT *c_visible, mObject * owner, DWORD add_data) {
    RECT old_c_visible;
    RECT both_c_visible; /* intersection of c_visible and old_c_visible */
    RECT c_invalid[4];
    int n_invalid;
    RECT rc_m_src, rc_m_dst;

    dbg();
    /* Moved */
    if (memcmp(c_viewport, &self->m_cachedViewport, sizeof(*c_viewport))) {
        IntersectRect(&old_c_visible, c_whole, &self->m_cachedViewport);
        IntersectRect(&both_c_visible, &old_c_visible, c_visible);

        if (! IsRectEmpty(&both_c_visible)) {
            CopyRect(&rc_m_src, &both_c_visible);
            OffsetRect(&rc_m_src, -self->m_cachedViewport.left, -self->m_cachedViewport.top);

            CopyRect(&rc_m_dst, &both_c_visible);
            OffsetRect(&rc_m_dst, -c_viewport->left, -c_viewport->top);

            LOG_TIME("    ");
            BitBlt(self->m_cache, rc_m_src.left, rc_m_src.top, RECTW(rc_m_src), RECTH(rc_m_src),
                    self->m_cache, rc_m_dst.left, rc_m_dst.top, -1);
            LOG_TIME("    ");

            n_invalid = SubtractRect(c_invalid, c_visible, &both_c_visible);
            assert(n_invalid <= 1);
            if (n_invalid > 0){
                PRINT_RECT("old dirty rect      ", &self->m_contentDirtyRect);
                GetBoundRect(&self->m_contentDirtyRect, &self->m_contentDirtyRect, &c_invalid[0]);
                PRINT_RECT("invalid rect of move", &c_invalid[0]);
                PRINT_RECT("new dirty rect      ", &self->m_contentDirtyRect);
            }
        }
    }

    /* Dirty ? */
    if (! IsRectEmpty(&self->m_contentDirtyRect)) {
        IntersectRect(&self->m_contentDirtyRect, &self->m_contentDirtyRect, c_visible);
        PRINT_RECT("dirty rect", &self->m_contentDirtyRect);
        if (! IsRectEmpty(&self->m_contentDirtyRect)) {
            HDC childDC;

            s_setChildClipRect(child, &self->m_contentDirtyRect);
            LOG_TIME("    ");
            childDC = GetSubDC(self->m_cache, child->x, child->y, RECTWP(c_whole), RECTHP(c_whole));
            ClipRectIntersect(childDC, &self->m_contentDirtyRect);

            _c(child->piece)->paint(child->piece, childDC, owner, add_data);
            memset(&self->m_contentDirtyRect, 0, sizeof(self->m_contentDirtyRect));
            LOG_TIME("    ");

            ReleaseDC(childDC);

            memset(&self->m_contentDirtyRect, 0, sizeof(self->m_contentDirtyRect));
        }
    }
    dbg();

    CopyRect(&self->m_cachedViewport, c_viewport);
}

static void s_drawContentWithCache(mScrollViewPiece *self, HDC hdc, mObject * owner, DWORD add_data) {
    mPieceItem* child = s_getContent(self);
    RECT c_whole; /* whole rect of content */
    RECT c_viewport; /* view port of content */
    RECT c_visible; /* the visible part of content */
    RECT v_visible; /* the visible part in view */

    LOG_TIME("    ");
    if (self->isTopPanel) {
        _c(self)->getRect(self, &self->clipRect);
    }
    _c(child->piece)->getRect(child->piece, &c_whole);
    _c(self)->getViewport(self, &c_viewport);
    IntersectRect(&c_visible, &c_whole, &c_viewport);

    if (! IsRectEmpty(&c_visible)) {
        CopyRect(&v_visible, &c_visible);
        OffsetRect(&v_visible, child->x, child->y);

        if (self->m_cache == HDC_INVALID) {
            self->m_cache = CreateCompatibleDCEx(hdc, RECTW(c_viewport), RECTH(c_viewport));
            _c(child->piece)->getRect(child->piece, &self->m_contentDirtyRect);
        }

        if (! IsRectEmpty(&self->m_contentDirtyRect) || memcmp(&self->m_cachedViewport, &c_viewport, sizeof(c_viewport))) {
            LOG_TIME("    ");
            s_updateCache(self, child, &c_whole, &c_viewport, &c_visible, owner, add_data);
            LOG_TIME("    ");
        }

        LOG_TIME("    ");
        BitBlt(self->m_cache, v_visible.left, v_visible.top, RECTW(v_visible), RECTH(v_visible),
                hdc, v_visible.left, v_visible.top, -1);
        LOG_TIME("    ");
    }else{
        // printf("c_visible = NULL\n");
    }
}

static void mScrollViewPiece_paint(mScrollViewPiece *self, HDC hdc, mObject * owner, DWORD add_data) {
    /* 
     * Background
     */
    LOG_TIME("  ");
    {
        RECT viewRc, contentRc;
        RECT visible[4];
        int n;

        _c(self)->getRect(self, &viewRc);
        if (self->m_content) {
            _c(self->m_content->piece)->getRect(self->m_content->piece, &contentRc);
            if (RECTH(viewRc) >= RECTH(contentRc) && self->m_content->y != 0) {
                _c(self)->movePiece(self, self->m_content->piece, self->m_content->x, 0);
            }
            OffsetRect(&contentRc, self->m_content->x, self->m_content->y);
            n = SubtractRect(visible, &viewRc, &contentRc);
        }else{
            visible[0] = viewRc;
            n = 1;
        }

        if (n > 0) {
            int i;
            int S1=4;
            int S2=1;
            gal_pixel colors[2];
            RECT *prc;

            assert(n == 1);

            colors[0] = RGB2Pixel(hdc, 0xd0, 0xd0, 0xd0);
            colors[1] = RGB2Pixel(hdc, 0xe0, 0xe0, 0xe0);
            while (n-- > 0) {
                prc = &visible[n];

                /* TODO: optimization */
                for (i=0; (S1+S2)*i<RECTWP(prc); ++i) {
                    SetBrushColor(hdc, colors[0]);
                    FillBox(hdc, (S1+S2)*i + prc->left, prc->top, S1, RECTHP(prc));
                    SetBrushColor(hdc, colors[1]);
                    FillBox(hdc, (S1+S2)*i+S1 + prc->left, prc->top, S2, RECTHP(prc));
                }
            }
        }

    }

    /*
     * Content
     */
    LOG_TIME("  ");
    if (self->m_cachable) {
        s_drawContentWithCache(self, hdc, owner, add_data);
    }else{
        Class(mPanelPiece).paint((mPanelPiece *)self, hdc, owner, add_data);
    }

    /* 
     * Decorator
     */
    LOG_TIME("  ");
    {
        /* ScrollBar */
        if (self->m_bNeedScrollBar && !self->m_bScrollbarAutoHided) {
            s_drawScrollBar(self, hdc, owner, add_data);
        }
    }
    LOG_TIME("  ");
}

static BOOL mScrollViewPiece_setRect(mScrollViewPiece *self, const RECT *prc) {
    s_removeCache(self);
    return Class(mPanelPiece).setRect((mPanelPiece*)self, prc);
}

static void mScrollViewPiece_invalidatePiece(mScrollViewPiece *self, mHotPiece *piece, const RECT *rc, BOOL reserveCache) {
    Class(mPanelPiece).invalidatePiece((mPanelPiece*)self, piece, rc, reserveCache);
    if (! reserveCache) {
        mPieceItem* child = s_getContent(self); 

        if (child->piece == piece) {
            if (rc == NULL) {
                RECT dirtyRect;
                _c(piece)->getRect(piece, &dirtyRect);
                GetBoundRect(&self->m_contentDirtyRect, &self->m_contentDirtyRect, &dirtyRect);
            }else{
                GetBoundRect(&self->m_contentDirtyRect, &self->m_contentDirtyRect, rc);
            }
            PRINT_RECT("InvalidatePiece", &self->m_contentDirtyRect);
        }
    }
}

static void mScrollViewPiece_movePiece(mScrollViewPiece *self, mHotPiece *child, int x, int y)
{
    mPieceItem *item = NULL;
    BOOL reserveCache = (self->m_content && child == self->m_content->piece);

    if ((item = _c(self)->searchItem(self, child))) {
        /* update old position */
        _c(self)->invalidatePiece(self, child, NULL, reserveCache);

        item->x = x;
        item->y = y;

        /* update new position */
        _c(self)->invalidatePiece(self, child, NULL, reserveCache);
    }else{
        assert(0);
    }
}

BEGIN_MINI_CLASS(mScrollViewPiece, mPanelPiece)
        CLASS_METHOD_MAP(mScrollViewPiece, construct    )
        CLASS_METHOD_MAP(mScrollViewPiece, destroy      )
        CLASS_METHOD_MAP(mScrollViewPiece, moveViewport )
        CLASS_METHOD_MAP(mScrollViewPiece, getViewport  )
        CLASS_METHOD_MAP(mScrollViewPiece, paint        )
        CLASS_METHOD_MAP(mScrollViewPiece, showScrollBar)
        CLASS_METHOD_MAP(mScrollViewPiece, enableCache)
        CLASS_METHOD_MAP(mScrollViewPiece, setRect)
        CLASS_METHOD_MAP(mScrollViewPiece, invalidatePiece)
        CLASS_METHOD_MAP(mScrollViewPiece, movePiece)
END_MINI_CLASS
