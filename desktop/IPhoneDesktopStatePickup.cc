#include <assert.h>
#include "global.h"
#include "IPhoneDesktopStatePickup.hh"
#include "IPhoneDesktopStateShaking.hh"
#include "IPhoneDesktopStateTransposition.hh"
#include "TransitionCollideItem.hh"
#include "IPhoneDesktopItem.hh"
#include "TransitionHover.hh"
#include "IPhoneDesktopStatePageTurning.hh"
#include "IPhoneDesktopView.hh"

#define PICKUP_CURVE Linear //InQuad
#define PICKUP_SCALE_STARTVAL 1.0f
#define PICKUP_SCALE_ENDVAL 1.3f
#define PICKUP_SCALE_DURATION 75 
#define PICKUP_ALPHA_DURATION  PICKUP_SCALE_DURATION 
#define PICKUP_ALPHA_STARTVAL 0xff
#define PICKUP_ALPHA_ENDVAL (0xff/2)

IPhoneDesktopStatePickup::IPhoneDesktopStatePickup(IPhoneDesktopController *controller, 
        State *stateMachine, const char *name)
    : IPhoneDesktopState(controller, stateMachine, name), m_originIndex(-1)
{
    m_offsetPos.x = m_offsetPos.y = 0;

    states[S511_PickedNormal] = NULL;
}

IPhoneDesktopStatePickup::~IPhoneDesktopStatePickup() {
}

void IPhoneDesktopStatePickup::s_transitionPickupMove(void *object, Transition *_transition, const Event* event, void *param) 
{
    assert (event->type() ==  Event::MoveEvent);
    IPhoneDesktopStatePickup* p = (IPhoneDesktopStatePickup*)(object);
    IPhoneDesktopItem *curItem =(IPhoneDesktopItem*)(p->m_controller->getScene().getFocusItem()); 
    POINT pt, scene_pt;
    MoveEvent* e = (MoveEvent*) event;

    pt = e->pos();
    pt.x -= p->m_offsetPos.x;
    pt.y -= p->m_offsetPos.y;

    /* must be add viewport offset.*/
    scene_pt = pt;
    p->m_controller->getView().ViewToScene(pt, scene_pt);
    curItem->moveTo(scene_pt);
    //printf("%s: item(%s) pos(%d,%d).\n", __FUNCTION__, curItem->name(), scene_pt.x, scene_pt.y);
}

void IPhoneDesktopStatePickup::createChildStates()
{
    if (! states[S511_PickedNormal]) {
        EventTransition* trans;

        states[S511_PickedNormal]  = new IPhoneDesktopState(m_controller, this, "PickedNormal");
        states[S512_Transposition] = new IPhoneDesktopStateTransposition(m_controller, this, "Transposition");
#ifdef HAVE_PAGESCROLL
        states[S513_PageTurning]   = new IPhoneDesktopStatePageTurning(m_controller, this, "PageTurning");
#endif
        setInitialState(states[S511_PickedNormal]);

        trans = new EventTransition(Event::MoveEvent, this, NULL, "Pickup targetless" );
        trans->subscribe(this, s_transitionPickupMove, NULL);

        trans = new TransitionCollideItem(states[S511_PickedNormal], states[S512_Transposition], 
                m_controller, "PickedNormal->Transposition" );
#ifdef HAVE_PAGESCROLL
        RECT rc;
        GetClientRect(m_controller->getHwnd(), &rc);
        rc.left = rc.right - rc.right / 6;
        trans = new TransitionHover(states[S511_PickedNormal], states[S513_PageTurning], 
                rc, 1000, "PickedNormal->PageTurning" );

        GetClientRect(m_controller->getHwnd(), &rc);
        rc.right = rc.right/6;
        trans = new TransitionHover(states[S511_PickedNormal], states[S513_PageTurning], 
                rc, 1000, "PickedNormal->PageTurning" );
#endif
    }
}

void IPhoneDesktopStatePickup::s_callSetItemScale (MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue) 
{
    IPhoneDesktopItem *curItem = (IPhoneDesktopItem *)target;
    curItem->setScale (*(float*)pValue);
}

void IPhoneDesktopStatePickup::s_callSetItemAlpha (MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue) 
{
    IPhoneDesktopItem *curItem = (IPhoneDesktopItem *)target;
    curItem->setTransparency (*(int*)pValue);
}

void IPhoneDesktopStatePickup::onEnter(const Event *event, const Transition *transition) 
{
    MGEFF_ANIMATION animation, group_animation;
    int alphaValue, duration;
    int curPage;
    float value;
    POINT curPos;
    IPhoneDesktopItem *curItem;
    POINT pt;

    //createChildStates();

    /*get current item*/
    GetCursorPos (&curPos); 
    ScreenToClient(m_controller->getHwnd(), &curPos.x, &curPos.y);
    m_controller->getView().ViewToScene(curPos, curPos);

    curPage = m_controller->getLayout().currentPage();
    m_controller->getLayout().point2Index(curPos, curPage, m_originIndex);
    curItem = (IPhoneDesktopItem*) m_controller->getLayout().index2Item(curPage, m_originIndex);
#if 0
    assert (NULL != curItem);
#else
    if (NULL == curItem)
    {
        return;
    }
#endif
    curItem->getCenterPoint(pt);
    m_offsetPos.x = curPos.x - pt.x;
    m_offsetPos.y = curPos.y - pt.y;

    m_controller->getScene().setFocusItem(curItem);

    /*create scale animation*/
    group_animation = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    animation = mGEffAnimationCreate(curItem, s_callSetItemScale, -1, MGEFF_FLOAT);
    value = PICKUP_SCALE_STARTVAL;
    mGEffAnimationSetStartValue(animation, &value);
    value = PICKUP_SCALE_ENDVAL;
    mGEffAnimationSetEndValue(animation, &value);
    duration = PICKUP_SCALE_DURATION;
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationAddToGroup(group_animation, animation);

    /*create alpha animation*/
    animation = mGEffAnimationCreate(curItem, s_callSetItemAlpha, -1, MGEFF_INT);
    alphaValue = PICKUP_ALPHA_STARTVAL;
    mGEffAnimationSetStartValue(animation, &alphaValue);
    alphaValue = PICKUP_ALPHA_ENDVAL;
    mGEffAnimationSetEndValue(animation, &alphaValue);
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationAddToGroup(group_animation, animation);

    mGEffAnimationAsyncRun(group_animation);
    mGEffAnimationSetProperty(group_animation, MGEFF_PROP_KEEPALIVE, 0);
}

void IPhoneDesktopStatePickup::s_callSetItemPos (MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue) 
{
    IPhoneDesktopItem *curItem = (IPhoneDesktopItem *)target;
    curItem->moveTo (*(POINT*)pValue);
}

void IPhoneDesktopStatePickup::onLeave (const Event *event, const Transition *transition)  
{
    MGEFF_ANIMATION animation, group_animation;
    IPhoneDesktopItem *curItem;
    float value;
    int alphaValue, duration;
    IPhoneDesktopLayout *layout = &m_controller->getLayout();
    int curPage = layout->currentPage();
    int vacancyIndex;
    POINT posValue;

    curItem = (IPhoneDesktopItem *)m_controller->getScene().getFocusItem();
    assert (curItem);

    group_animation = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    /*create scale animation*/
    animation = mGEffAnimationCreate(curItem, s_callSetItemScale, -1, MGEFF_FLOAT);
    value = PICKUP_SCALE_ENDVAL;
    mGEffAnimationSetCurve(animation, PICKUP_CURVE);
    mGEffAnimationSetStartValue(animation, &value);
    value = PICKUP_SCALE_STARTVAL;
    mGEffAnimationSetEndValue(animation, &value);
    duration = PICKUP_SCALE_DURATION;
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationAddToGroup(group_animation, animation);

    /*create alpha animation*/
    animation = mGEffAnimationCreate(curItem, s_callSetItemAlpha, -1, MGEFF_INT);
    alphaValue = PICKUP_ALPHA_ENDVAL;
    mGEffAnimationSetCurve(animation, PICKUP_CURVE);
    mGEffAnimationSetStartValue(animation, &alphaValue);
    alphaValue = PICKUP_ALPHA_STARTVAL;
    mGEffAnimationSetEndValue(animation, &alphaValue);
    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationAddToGroup(group_animation, animation);

    if (curItem != layout->index2Item(curPage, m_originIndex)) {
        /*create move animation*/
        animation = mGEffAnimationCreate(curItem, s_callSetItemPos, -1, MGEFF_POINT);

        /*get target position*/
        for (; (vacancyIndex = layout->getVacancy(curPage)) < 0; ++curPage); /* cusItem may be not in the curpage */
        /* increase new page total if some items have gone to a new page  */
        while (curPage >= layout->totalPage())
            layout->newPage();

        if (vacancyIndex >= 0) {
            curItem->getCenterPoint(posValue);
            mGEffAnimationSetCurve(animation, PICKUP_CURVE);
            mGEffAnimationSetStartValue (animation, &posValue);

            m_controller->getLayout().index2Point(posValue, curPage, vacancyIndex);
            mGEffAnimationSetEndValue(animation, &posValue);

            mGEffAnimationSetDuration(animation, duration);
            mGEffAnimationAddToGroup (group_animation, animation);
        } else {
            puts("vacancyIndex < 0");
            assert(0);
        }
    }

    mGEffAnimationAsyncRun(group_animation);
    mGEffAnimationSetProperty(group_animation, MGEFF_PROP_KEEPALIVE, 0);
}
