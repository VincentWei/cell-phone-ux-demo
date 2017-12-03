
/*!============================================================================
 * @file IPhoneDestktopStateApp.cc
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <assert.h>
#include <global.h>
#include <math.h>
#include "boost/any.hpp"
#include "IPhoneDesktopStateApp.hh"
#include "IPhoneDesktopItem.hh"
#include "DesktopToolbar.hh"
#include "TransitionAnimationFinished.hh"
#include "IPhoneDesktopTransitionInterface.hh"
#include "ResourceManager.hh"

static HDC g_bg_hdc;
#if 0
static void erase_view_bg(HDC dst_dc, const RECT &rc, void * ctxt)
{
    IPhoneDesktopView* view = reinterpret_cast<IPhoneDesktopView*>(ctxt);
    if (g_bg_hdc && NULL != view) {
        BitBlt(g_bg_hdc, 0, 0, -1, -1, dst_dc, rc.left, rc.top, -1);
    }
}
#endif

IPhoneDesktopStateApp::IPhoneDesktopStateApp(IPhoneDesktopController *controller, StateMachine *stateMachine, const char *name)
:   IPhoneDesktopState(controller, stateMachine, name), m_animation(NULL)
{
    const RECT& rc = m_controller->getView().getRect();
    g_bg_hdc = CreateCompatibleDCEx(Get32MemDC(), RECTW(rc), RECTH(rc));
}

IPhoneDesktopStateApp::~IPhoneDesktopStateApp()
{
    DeleteMemDC(g_bg_hdc);
}

void IPhoneDesktopStateApp::animationFinishedCb(MGEFF_ANIMATION animation)
{
    std::pair<HWND, HDC>*context = reinterpret_cast<std::pair<HWND, HDC>* >(mGEffAnimationGetContext(animation));
    if (NULL != context) {
        ShowWindow(context->first, SW_SHOWNORMAL);
        DeleteMemDC (context->second);
        delete context;
    }
}

void IPhoneDesktopStateApp::onStateItemAround(MGEFF_ANIMATION animation, void *target, int id, void *pValue)
{
    IPhoneDesktopItem *item = (IPhoneDesktopItem *)target;
    item->moveTo(*(POINT *)pValue);
}

void IPhoneDesktopStateApp::onStateItemCenter(MGEFF_ANIMATION animation, void *target, int id, void *pValue)
{
    IPhoneDesktopItem *item = (IPhoneDesktopItem *)target;
    // item->setScale (1.00f);
    item->setTransparency (*(int*)pValue);
}

void IPhoneDesktopStateApp::onStateApp(MGEFF_ANIMATION animation, void *target, int id, void *pValue)
{
    static RECT old_rc;
    HDC hsdc = (HDC)id;
    RECT *prc = (RECT *)pValue;
    if (RECTW(old_rc) != 0 && RECTH(old_rc) != 0) {
        FillBox(g_bg_hdc, old_rc.left, old_rc.top, RECTW(old_rc), RECTH(old_rc));
    }
    STRETCHBLT(hsdc, 0, 0, -1, -1, g_bg_hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), -1);
    old_rc = *prc;
}

void IPhoneDesktopStateApp::setAnimation(const Event *event, const Transition *_transition, int zoomout)
{
    HDC dst_hdc = HDC_INVALID;
    HWND dst_hwnd = HWND_INVALID;
    const Event::EventParameters& params = event->parameters();
    if (params.size() == 2
            && params.back().type() == typeid(HWND)
            && params.front().type() == typeid(HDC)) {
        dst_hdc = boost::any_cast<HDC>(params.front());
        dst_hwnd = boost::any_cast<HWND>(params.back());
    }
    assert(HDC_INVALID != dst_hdc);
    assert(HWND_INVALID != dst_hwnd);

    #if HAVE_APP_INOUT_ANIMATION
    IPhoneDesktopLayout *layout = &m_controller->getLayout();
    int page = layout->currentPage();
/*
 * 0 | 1 | 2
 *---+---+---
 * 3 | 4 | 5
 *---+---+---
 * 6 | 7 | 8
 *
 * */
    //static int index[9] = {1, 2, 5, 8, 7, 6, 3, 0, 4};
/*
 *  0  1 |  2  3
 *  4  5 |  6  7
 * ------+------
 *  8  9 | 10 11
 * 12 13 | 14 15
 *
 * */
    static int index[ITEMS_PER_PAGE] = {0, 1, 4, 5, 2, 3, 6, 7, 10, 11, 14, 15, 8, 9, 12, 13};
    static const double PI_DEVIDE_4 = 3.141592 / 4;
    int duration = 800;
    MGEFF_ANIMATION animation;
    m_animation = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    /* add animation for item around */
    int i, j, k = 0;
    float radians[] = {-PI_DEVIDE_4, PI_DEVIDE_4, PI_DEVIDE_4 * 3, PI_DEVIDE_4 * 5};
    POINT pt_1, pt_2;
    POINT *ppt_s, *ppt_e;
    RECT rc_layout = m_controller->getView().getRect();
    for (i = 0; i < ROWS_PER_PAGE; ++i) {
        for (j = 0; j < COLUMNS_PER_PAGE; ++j, ++k) {
            if (zoomout) {
                ppt_s = &pt_1;
                ppt_e = &pt_2;
                m_items[k] = dynamic_cast<IPhoneDesktopItem *>(layout->index2Item(page, index[k]));
            } else {
                ppt_s = &pt_2;
                ppt_e = &pt_1;
            }

            /* continue if there is no item in this index*/
            if (!m_items[k])
                continue;

            layout->index2Point (pt_1, page, index[k]);
            pt_2.x = pt_1.x + RECTW(rc_layout) * sin(radians[i]);
            pt_2.y = pt_1.y - RECTH(rc_layout) * cos(radians[i]);
            printf("[start.x = %d start.y = %d] [end.x = %d end.y = %d]\n", ppt_s->x, ppt_s->y, ppt_e->x, ppt_e->y);

            animation = mGEffAnimationCreate(m_items[k], onStateItemAround, -1, MGEFF_POINT);

            mGEffAnimationSetStartValue(animation, ppt_s);
            mGEffAnimationSetEndValue(animation, ppt_e);
            mGEffAnimationSetDuration(animation, duration);
            mGEffAnimationSetCurve(animation, InCirc);
            mGEffAnimationAddToGroup(m_animation, animation);
        }
    }
#if 0
    /* add animation for center item */
    IPhoneDesktopItem *item = dynamic_cast<IPhoneDesktopItem *>(layout->index2Item(page, index[15]));
    if (item) {
        int alpha_1 = 255, alpha_2 = 0;
        int *palpha_s, *palpha_e;
        if (zoomout) {
            palpha_s = &alpha_1;
            palpha_e = &alpha_2;
        } else {
            palpha_s = &alpha_2;
            palpha_e = &alpha_1;
        }

        animation = mGEffAnimationCreate(item, onStateItemCenter, -1, MGEFF_INT);

        mGEffAnimationSetStartValue(animation, palpha_s);
        mGEffAnimationSetEndValue(animation, palpha_e);
        mGEffAnimationSetDuration(animation, duration);
        mGEffAnimationSetCurve(animation, InCirc);
        mGEffAnimationAddToGroup(m_animation, animation);
    }
#endif
    /* add animation for app */
    RECT rc_1;
    const RECT *prc_s, *prc_e;
    const RECT rc_2 = {0, 0, 
        RECTW(m_controller->getView().getRect()), 
        RECTH(m_controller->getView().getRect())};
    rc_1.left = rc_1.right = RECTW(rc_2) / 2;
    rc_1.top = rc_1.bottom = RECTH(rc_2) / 2;
    if (zoomout) {
        prc_s = &rc_1;
        prc_e = &rc_2;
    } else {
        prc_s = &rc_2;
        prc_e = &rc_1;
    }

    IPhoneDesktopView& view = m_controller->getView();

    SetBrushColor(g_bg_hdc, zoomout ? view.getBgColor() : (view.getBgColor() & 0x00FFFFFF));
    FillBox(g_bg_hdc, 0, 0, ACTIVITY_W, ACTIVITY_H);
    if (!zoomout) {
        SetBrushColor(g_bg_hdc, view.getBgColor());   
    }
    view.setEraseBackgroundCallback(erase_view_bg, reinterpret_cast<void*>(&view));
    animation = mGEffAnimationCreate((void *)layout->getHwnd(), onStateApp, (int)dst_hdc, MGEFF_RECT);
    mGEffAnimationSetStartValue(animation, prc_s);
    mGEffAnimationSetEndValue(animation, prc_e);
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationSetCurve(animation, InCirc);
    std::pair<HWND, HDC>* ctxt = new std::pair<HWND, HDC>(dst_hwnd, dst_hdc);
    mGEffAnimationSetContext(animation, reinterpret_cast<void*>(ctxt));
    mGEffAnimationSetFinishedCb(animation, animationFinishedCb);
    mGEffAnimationAddToGroup(m_animation, animation);

    animation = mGEffAnimationCreate(&m_controller->getView(), IPhoneDesktopView::callRedraw, 1, MGEFF_INT);
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationAddToGroup(m_animation, animation);

    HWND hwnd = layout->getHwnd();
    mGEffAnimationAsyncRun(m_animation);
    mGEffAnimationSetProperty(m_animation, MGEFF_PROP_KEEPALIVE, 0);
    mGEffAnimationWait(&hwnd, m_animation);
    view.setEraseBackgroundCallback(NULL, NULL);
    #else
    //std::pair<HWND, HDC>* ctxt = new std::pair<HWND, HDC>(dst_hwnd, dst_hdc);
    //ShowWindow(ctxt->first, SW_SHOWNORMAL);
    //DeleteMemDC (ctxt->second);
    //delete ctxt;
    #endif /* HAVE_APP_INOUT_ANIMATION */
}
