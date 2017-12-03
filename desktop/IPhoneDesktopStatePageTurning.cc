/*!============================================================================
 * @file IPhoneDestktopStatePageTurning.cc
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <assert.h>
#include <string.h>
#include "global.h"
#include "IPhoneDesktopStatePageTurning.hh"
#include "IPhoneDesktopItem.hh"
#include "TransitionAnimationFinished.hh"
#include "IPhoneDesktopTransitionInterface.hh"
#include "IPhoneDesktopStateShaking.hh"
#include "IPhoneDesktopStatePickup.hh"
#include "TransitionCollideItem.hh"
#include "TransitionHover.hh"
#include "IPhoneDesktopLayout.hh"
#include "IPhoneDesktopView.hh"

IPhoneDesktopStatePageTurning::IPhoneDesktopStatePageTurning(IPhoneDesktopController *controller, State *parent, const char *name)
    : IPhoneDesktopState(controller, parent, name), m_animation(NULL)
{
    IPhoneDesktopStatePickup *statePickedup = dynamic_cast<IPhoneDesktopStatePickup *>(parentState());
    assert(statePickedup);
    m_transAnimationTransposition = new TransitionAnimationFinished(NULL,
            this, statePickedup->states[IPhoneDesktopStatePickup::S511_PickedNormal],
            "Transposition->PickedNormal");
}

void IPhoneDesktopStatePageTurning::animationCallback (MGEFF_ANIMATION handle, void* target, int id, void* value)
{
    RECT view_rc, move_rc;
    IPhoneDesktopView* view;

    int* cur_pos = (int*)value;
    IPhoneDesktopController* controller = (IPhoneDesktopController*)target;
    Item* curItem;
    POINT pt;

    if (NULL == controller) 
        return;

    view = &(controller->getView());
    view_rc = view->getRect ();
    SetRect (&move_rc, *cur_pos, 0, (*cur_pos)+RECTW(view_rc), RECTH(view_rc));

//    printf ("scroll animation:(%d, %d, %d, %d)\n", move_rc.left, move_rc.top, move_rc.right, move_rc.bottom);

    view->moveTo (move_rc);

    /* move focus item with view.*/
    curItem = controller->getScene().getFocusItem();
    assert(curItem != NULL);
    curItem->getCenterPoint(pt);
    pt.x += *cur_pos - view_rc.left;
    curItem->moveTo(pt);
//    printf("%s: pos(%d,%d)\n", __FUNCTION__, pt.x, pt.y);

    view->invalidateRect(&view->getAnimationArea());
}

void IPhoneDesktopStatePageTurning::noAnimationCallback (MGEFF_ANIMATION handle, void* target, int id, void* value)
{
    //do nothing
}

void IPhoneDesktopStatePageTurning::onEnter(const Event *event, const Transition *_transition)
{
    int view_w, half_view;
    int index_noItemIn;
    int cur_page, expect_page, total_page;
    int startvalue, endvalue;
    int duration;
    RECT cur_rc;

    IPhoneDesktopController* controller = m_controller;
    IPhoneDesktopView* view = &(controller->getView());
    IPhoneDesktopLayout* layout = &(controller->getLayout());
    TransitionHover* trans = (TransitionHover*)_transition;

    cur_rc = view->getRect ();
    view_w = RECTW(cur_rc);
    half_view = view_w / 2;
    cur_page = layout->currentPage ();

    /* scroll to prev page  */
    if (trans->getRect().left <  RECTW(cur_rc)/2) {
        expect_page = layout->prevPage ();
    }
    else {
        expect_page = layout->nextPage ();
        /* add new page when the picked item wanna to */
        if (cur_page == expect_page && layout->index2Item(cur_page, 0)) {
            layout->newPage();
            expect_page = layout->nextPage();
        }
    }

    total_page = layout->totalPage ();
    if (cur_page != expect_page) {
        /* maybe a hole will be left when the picked item is moved to an other page.
         * so, we push items, which was behind the piecked item, to their prev hole one by one */
        for (index_noItemIn = 0;
                index_noItemIn < layout->getItemTotal()
                && layout->index2Item(cur_page, index_noItemIn);
                ++index_noItemIn
            );
        if (index_noItemIn != layout->getItemTotal()) {
            IPhoneDesktopItem *item;
            POINT point;
            while ((item = dynamic_cast<IPhoneDesktopItem *>(layout->index2Item(cur_page, index_noItemIn + 1)))) {
                layout->index2Point(point, cur_page, index_noItemIn);
                item->moveTo(point);
                ++index_noItemIn;
            }
        }

        startvalue = cur_rc.left;
        endvalue = expect_page * view_w;
        duration = 200;

        printf ("s:%d, e:%d, duration:%d, expect_page:%d, total_page:%d\n", 
                startvalue, endvalue, duration, expect_page, total_page);

        /* just for test */
        /*switch (layout->currentPage()) {
          case 0:
          view->setBgColor (PIXEL_black);
          break;

          case 1:
          view->setBgColor (PIXEL_cyan);
          break;

          case 2:
          view->setBgColor (PIXEL_green);
          break;

          case 3:
          view->setBgColor (PIXEL_yellow);
          break;

          default:
          view->setBgColor (PIXEL_black);
          break;
          }*/

        /* run animation */
        m_animation = mGEffAnimationCreate(m_controller, IPhoneDesktopStatePageTurning::animationCallback, 0, MGEFF_INT);
        mGEffAnimationSetStartValue (m_animation, &startvalue);
        mGEffAnimationSetEndValue (m_animation, &endvalue);
        //mGEffAnimationSetCurve (animation,InQuad);

        mGEffAnimationSetDuration(m_animation, duration);
        /*TODO: rember to delete this animation */
        m_transAnimationTransposition->setAnimation(m_animation);
        mGEffAnimationAsyncRun(m_animation);
    } else {
        m_animation = mGEffAnimationCreate(m_controller, IPhoneDesktopStatePageTurning::noAnimationCallback, 0, MGEFF_INT);
        mGEffAnimationSetDuration(m_animation, 10);
        m_transAnimationTransposition->setAnimation(m_animation);
        mGEffAnimationAsyncRun(m_animation);
    }
}

void IPhoneDesktopStatePageTurning::onLeave(const Event *event, const Transition *transition)
{
    HWND hwnd = m_controller->getHwnd();
    IPhoneDesktopLayout *layout = &m_controller->getLayout();

    assert(m_animation);

    {
        /* delete scroll page animation */
        mGEffAnimationSetFinishedCb(m_animation, NULL);
        mGEffAnimationWait(&hwnd, m_animation);
        mGEffAnimationDelete(m_animation);
        m_animation = NULL;
    }

    {
        /* delete the last empty page */
        int last_page = layout->totalPage() - 1;
        if (layout->currentPage() != last_page
                && layout->index2Item(last_page, 0) == NULL)
            layout->delPage();
    }

    {
        /* send message to invoke a collide test */
        POINT pt;
        GetCursorPos (&pt);
        ScreenToClient(hwnd, &pt.x, &pt.y);
        SendMessage(hwnd, MSG_MOUSEMOVE, 0, MAKELONG(pt.x, pt.y));
    }
}
