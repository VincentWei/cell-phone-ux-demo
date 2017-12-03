#include "global.h"
#include "IPhoneDesktopStateShaking.hh"
#include "IPhoneDesktopStatePickup.hh"
#include "IPhoneDesktopItem.hh"
#include "TransitionAnimationFinished.hh"
#include "TransitionPressOnItem.hh"
#include "IPhoneDesktopStateShifting.hh"

IPhoneDesktopStateShaking::IPhoneDesktopStateShaking(IPhoneDesktopController *controller, StateMachine *stateMachine, const char *name)
    : IPhoneDesktopState(controller, stateMachine, name), m_animation(NULL)
{
    int i = 0; 
    for (i = 0; i < S_NR; i++) {
        states[i] = NULL;
    }
}

IPhoneDesktopStateShaking::~IPhoneDesktopStateShaking() {
    if (m_animation) {
        mGEffAnimationStop(m_animation);
        m_animation = NULL;
    }
}

void IPhoneDesktopStateShaking::createChildStates() {
    if (states[S51_Pickedup] == NULL) {
        EventTransition* trans;
        states[S51_Pickedup] =  new IPhoneDesktopStatePickup (m_controller, this, "Pickedup");
        states[S52_Putdown] = new IPhoneDesktopState (m_controller, this, "Putdown");
#ifdef HAVE_PAGESCROLL
        states[S53_ShakingAndScrolling] = new IPhoneDesktopStateShifting(m_controller, this, "ShakingAndScrolling");
#endif /* HAVE_PAGESCROLL */
        ((IPhoneDesktopStatePickup*)states[S51_Pickedup])->createChildStates();
#ifdef HAVE_PAGESCROLL
        ((IPhoneDesktopStateShifting*)states[S53_ShakingAndScrolling])->createChildStates(states[S52_Putdown], NULL);
#endif /* HAVE_PAGESCROLL */
        setInitialState(states[S51_Pickedup]);
        //setInitialState(states[S52_Putdown]);

        trans = new TransitionPressOnItem(states[S52_Putdown], states[S51_Pickedup], m_controller, "Putdown->Pickedup");

        trans = new EventTransition(Event::MouseEvent, 
                states[S51_Pickedup], states[S52_Putdown],
                "PickedUp->PutDown" );
        trans->setButtonType(MouseEvent::ButtonReleased);
        trans->setButtons(MouseEvent::LeftButton);

#ifdef HAVE_PAGESCROLL
        trans = new EventTransition(Event::MoveEvent, 
                states[S52_Putdown], states[S53_ShakingAndScrolling], "EventTransition::S5_Shaking->S53_ShakingAndScrolling");
        trans = new EventTransition(Event::MouseEvent, 
                states[S53_ShakingAndScrolling], states[S52_Putdown], "EventTransition::S53_ShakingAndScrolling->S52_Putdown");
        trans->setButtonType(MouseEvent::ButtonReleased);
        trans->setButtons(MouseEvent::LeftButton);
#endif /* HAVE_PAGESCROLL */
    }
}

void IPhoneDesktopStateShaking::callSetItemStatus(MGEFF_ANIMATION animation, void *target, int id, void *pValue) {
    static const int table[] = {
        1,
        0,
        2,
        0
    };
    IPhoneDesktopItem *item = (IPhoneDesktopItem *)target;
    int index = *(int *)pValue;
    index = (index >> 8) % (sizeof(table)/sizeof(table[0]));
    item->setStatus(table[index]);
}

void IPhoneDesktopStateShaking::onEnter(const Event *event, const Transition *transition) {
    Scene::ItemList &items = m_controller->getScene().items();
    Scene::ItemList::const_iterator i;
    MGEFF_ANIMATION animation;
    int duration = 400;
    int value;

    //createChildStates();

    m_animation = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    for (i=items.begin(); i!=items.end(); ++i) {
        animation = mGEffAnimationCreate(*i, callSetItemStatus, -1, MGEFF_INT);
        value = (rand() % 4) << 8;
        mGEffAnimationSetStartValue(animation, &value);
        value += (4 << 8) - 1;
        mGEffAnimationSetEndValue(animation, &value);
        mGEffAnimationSetDuration(animation, duration + rand() % 30);
        mGEffAnimationSetProperty(animation, MGEFF_PROP_LOOPCOUNT, MGEFF_INFINITE);
        mGEffAnimationAddToGroup(m_animation, animation);
    }

    animation = mGEffAnimationCreate(&m_controller->getView(), IPhoneDesktopView::callRedraw, 1, MGEFF_INT);

    mGEffAnimationSetDuration(animation, duration);
    mGEffAnimationSetProperty(animation, MGEFF_PROP_LOOPCOUNT, MGEFF_INFINITE);
    mGEffAnimationAddToGroup(m_animation, animation);

    mGEffAnimationSetProperty(m_animation, MGEFF_PROP_LOOPCOUNT, MGEFF_INFINITE);

    mGEffAnimationAsyncRun(m_animation);
    mGEffAnimationSetProperty(m_animation, MGEFF_PROP_KEEPALIVE, 0);
}

#if 0
extern "C" {
    extern void EffGCCheckPoint();
};
#endif

void IPhoneDesktopStateShaking::onLeave(const Event *event, const Transition *transition) {
    Scene::ItemList &items = m_controller->getScene().items();
    Scene::ItemList::const_iterator i;
    IPhoneDesktopItem *item;

    if (m_animation) {
        mGEffAnimationStop(m_animation);
        m_animation = NULL;
    }

    for (i=items.begin(); i!=items.end(); ++i) {
        item = (IPhoneDesktopItem *) (*i);
        item->setStatus(0);
    }
    m_controller->getView().invalidateRect(&m_controller->getView().getAnimationArea());

#if 0
    EffGCCheckPoint();
#endif
}
