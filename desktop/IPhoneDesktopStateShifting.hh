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
 * @file IPhoneDesktopStateShifting.hh
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATESHIFTING_HH
#define IPHONEDESKTOPSTATESHIFTING_HH

#include "IPhoneDesktopState.hh"
#include "ScrollPageTrigger.hh"
#include "TransitionFilterMouseMove.hh"


class IPhoneDesktopStateShifting : public IPhoneDesktopState 
{
public:
    IPhoneDesktopStateShifting(IPhoneDesktopController* controller, StateMachine* stateMachine, const char* name=NULL); 
    IPhoneDesktopStateShifting(IPhoneDesktopController* controller, State* parentState, const char* name=NULL);
    ~IPhoneDesktopStateShifting();

    void createChildStates(State* returnState, State* appState);

protected:
    void onEnter(const Event *event, const Transition *transition);
    //void onLeave(const Event *event, const Transition *transition);

private:
    enum {
        S41_Draging,
        S42_Inertia,
        S_NR
    };

    ScrollParam dragingScrollParam;
    ScrollParam inertiaScrollParam;
    TransitionFilterMouseMove* transFilter;

};

#endif /* IPHONEDESKTOPSTATESHIFTING_HH */


