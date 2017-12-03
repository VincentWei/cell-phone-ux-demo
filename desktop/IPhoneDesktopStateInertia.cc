/*!============================================================================
 * @file IPhoneDestktopStateInertia.cc
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include "IPhoneDesktopStateInertia.hh"
#include "global.h"


IPhoneDesktopStateInertia::IPhoneDesktopStateInertia(IPhoneDesktopController* controller, State* parentState, const char* name)
    : IPhoneDesktopState(controller, parentState, name)
{
}


IPhoneDesktopStateInertia::~IPhoneDesktopStateInertia() 
{
}


void IPhoneDesktopStateInertia::onStopInertiaAnimationTrigger(void* object, Transition* trans, const Event* event, void* param)
{
    IPhoneDesktopStateInertia* state = reinterpret_cast<IPhoneDesktopStateInertia*>(object);
    HWND hwnd = state->m_controller->getHwnd();
    MGEFF_ANIMATION animation = (MGEFF_ANIMATION)param;

    if (animation){
        mGEffAnimationSetFinishedCb(animation, NULL);
        mGEffAnimationWait (&hwnd, animation);
        mGEffAnimationStop (animation);
        printf ("stop animation handle:%p\n", animation);
    }
}


