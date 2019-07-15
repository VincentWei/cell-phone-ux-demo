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
/*!============================================================================
 * @file IPhoneDestktopStateTransposition.cc
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <assert.h>
#include <string.h>
#include "global.h"
#include "IPhoneDesktopStateTransposition.hh"
#include "IPhoneDesktopItem.hh"
#include "TransitionAnimationFinished.hh"
#include "IPhoneDesktopTransitionInterface.hh"
#include "IPhoneDesktopStateShaking.hh"
#include "IPhoneDesktopStatePickup.hh"
#include "TransitionCollideItem.hh"

#define CURVE InQuad
#define STEP 50
#ifdef __ARCH_X86__
    #define TOTAL_TIME 150
#else
    #define TOTAL_TIME 300
#endif
//#define ITEM_NUM    16

IPhoneDesktopStateTransposition::IPhoneDesktopStateTransposition(IPhoneDesktopController *controller, State *parent, const char *name)
    : IPhoneDesktopState(controller, parent, name), m_animation(NULL)
{
    IPhoneDesktopStatePickup *statePickedup = dynamic_cast<IPhoneDesktopStatePickup *>(parentState());
    assert(statePickedup);
    m_transAnimationTransposition = new TransitionAnimationFinished(NULL,
            this, statePickedup->states[IPhoneDesktopStatePickup::S511_PickedNormal],
            "Transposition->PickedNormal");
}

void IPhoneDesktopStateTransposition::onTransposition(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue)
{
    IPhoneDesktopItem *item = (IPhoneDesktopItem *)target;
    item->moveTo(*(POINT *)pValue);
}

void IPhoneDesktopStateTransposition::onEnter(const Event *event, const Transition *_transition)
{
    const TransitionCollideItem *transition;
    transition = dynamic_cast<const TransitionCollideItem *>(_transition);
    assert(NULL != transition);

    IPhoneDesktopLayout *layout = &m_controller->getLayout();

    POINT point_s, point_e, point_current;

    /* find current page and index */
    int index_e = 0, page_e = 0;
    point_current = transition->expectPosition();

    if (layout->point2Index(point_current, page_e, index_e) == -1)
        return;

    /* find how many pages is full begin from current page,
     * and restore the final fullpage's number to page_s */
    int index_s;
    int page_s = page_e;

    while (1) {
        for (index_s = 0; index_s < layout->getItemTotal() && layout->index2Item(page_s, index_s); ++index_s);
        if (index_s == layout->getItemTotal())
            ++page_s;
        else
            break;
    }

    /* move the items not in current page to next point */
    if (page_s > page_e) {
        int page;
        int index = layout->getItemTotal() - 1;
        POINT point;
        IPhoneDesktopItem *cur_item, *next_item;
        cur_item = dynamic_cast<IPhoneDesktopItem *>(layout->index2Item(page_e, layout->getItemTotal() - 1));
        assert(NULL != cur_item);

        for (page = page_e + 1; page <= page_s; ++page) {
            for (index = 0; cur_item && index < layout->getItemTotal(); ++index) {
                next_item = dynamic_cast<IPhoneDesktopItem *>(layout->index2Item(page, index));
                layout->index2Point(point, page, index);
                cur_item->moveTo(point);
                cur_item = next_item;
            }
        }

        /* this should set to (layout->getItemTotal() - 1) when mutil pages checking is in */
        index_s = layout->getItemTotal() - 1;
    }

    /* forward or backward */
    int next;
    if ((index_s + layout->getItemTotal() * (page_s - page_e)) > index_e)
        next = -1;
    else
        next = 1;

    /* create animation for items in current page */
    int i;
    int duration = TOTAL_TIME;
    IPhoneDesktopItem *item;
    MGEFF_ANIMATION animation;
    m_animation = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    layout->index2Point (point_e, page_e, index_s);
    for (i = index_s; i != index_e; i += next) {
        layout->index2Point (point_s, page_e, i + next);
        item = dynamic_cast<IPhoneDesktopItem *>(layout->point2Item(point_s));
        if (!item)
            continue;

        animation = mGEffAnimationCreate(item, onTransposition, -1, MGEFF_POINT);
        mGEffAnimationSetCurve(animation, CURVE);
        mGEffAnimationSetStartValue(animation, &point_s);
        mGEffAnimationSetEndValue(animation, &point_e);
#if 0
//        assert(duration + next * i * STEP >= 0);
        if (next > 0) {
            printf("duration = %d\n", duration + (abs(index_s - index_e) - i) / 3 * STEP);
            mGEffAnimationSetDuration(animation, duration + (abs(index_s - index_e) - i) / 3 * STEP);
        } else {
            printf("duration = %d\n", duration + (i - abs(index_s - index_e)) / 3 * STEP);
            mGEffAnimationSetDuration(animation, duration + (i - abs(index_s - index_e)) / 3 * STEP);
        }
#endif
        mGEffAnimationSetDuration(animation, duration);
        mGEffAnimationAddToGroup(m_animation, animation);

        memcpy(&point_e, &point_s, sizeof(POINT));
    }

    animation = mGEffAnimationCreate(&m_controller->getView(), IPhoneDesktopView::callRedraw, 1, MGEFF_INT);
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationAddToGroup(m_animation, animation);

    m_transAnimationTransposition->setAnimation(m_animation);

    mGEffAnimationAsyncRun(m_animation);
}

void IPhoneDesktopStateTransposition::onLeave(const Event *event, const Transition *transition)
{
    HWND hwnd = m_controller->getHwnd();
    IPhoneDesktopLayout *layout = &m_controller->getLayout();

    if (m_animation) {
        /* delete transposition animation */
        mGEffAnimationSetFinishedCb(m_animation, NULL);
        mGEffAnimationWait(&hwnd, m_animation);
        mGEffAnimationDelete(m_animation);
        m_animation = NULL;
    }

    {
        /* increase new page total if some items have gone to a new page  */
        if (layout->index2Item(layout->totalPage(), 0))
            layout->newPage();
    }

    {
        /* send message to invoke a scroll page test */
        POINT pt;
        GetCursorPos (&pt);
        ScreenToClient(hwnd, &pt.x, &pt.y);
        SendMessage(hwnd, MSG_MOUSEMOVE, 0, MAKELONG(pt.x, pt.y));
    }

}
