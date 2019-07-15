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
#include <assert.h>
#include "IPhoneDesktopStateNormal.hh"
#include "ActivityStack.hh"
#include "IPhoneDesktopItem.hh"
#include "TransitionClickOnItem.hh"
#include "IPhoneDesktopTransitionInterface.hh"

IPhoneDesktopStateNormal::IPhoneDesktopStateNormal(IPhoneDesktopController *controller, StateMachine *stateMachine, const char *name)
    : IPhoneDesktopState(controller, stateMachine, name)
{
#if 0
    Transition *trans;
    trans = new TransitionClickOnItem(this, NULL, controller, "1->null: click on item");
    trans->subscribe(this, s_onClickOnItem, NULL);
#endif
}

IPhoneDesktopStateNormal::~IPhoneDesktopStateNormal() {
}

void IPhoneDesktopStateNormal::s_onClickOnItem(void *object, Transition *_transition, const Event* event, void *param) {
    IPhoneDesktopTransitionInterface *transition;
    IPhoneDesktopItem *item;

    transition = dynamic_cast<IPhoneDesktopTransitionInterface *>(_transition);
    assert(NULL != transition);
    item = transition->item();

    {
        ActivityStack* act_stack = ActivityStack::singleton();
        act_stack->push(item->name());
    }
}
