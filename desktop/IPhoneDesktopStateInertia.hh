/*!============================================================================
 * @file IPhoneDesktopStateInertia.hh
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATEINERTIA_HH
#define IPHONEDESKTOPSTATEINERTIA_HH

#include "IPhoneDesktopState.hh"


class IPhoneDesktopStateInertia : public IPhoneDesktopState
{
public:
    IPhoneDesktopStateInertia(IPhoneDesktopController* controller, State* parentState, const char* name=NULL);
    ~IPhoneDesktopStateInertia();

    static void onStopInertiaAnimationTrigger(void* object, Transition* trans, const Event* event, void* param);

};

#endif /* IPHONEDESKTOPSTATEINERTIA_HH */


