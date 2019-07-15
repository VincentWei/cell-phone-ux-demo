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
 * @file IPhoneDesktopStatePageTurning.hh
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATEPAGETURNING_HH
#define IPHONEDESKTOPSTATEPAGETURNING_HH

#include "IPhoneDesktopState.hh"
#include "TransitionAnimationFinished.hh"
#include <mgeff/mgeff.h>

class IPhoneDesktopStatePageTurning : public IPhoneDesktopState {
    public:
        IPhoneDesktopStatePageTurning(IPhoneDesktopController *controller, State *parentState, const char *name);
        ~IPhoneDesktopStatePageTurning() {}
    private:
        static void animationCallback (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value);
        static void noAnimationCallback (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value);
    private:
        //static void onTransposition(MGEFF_ANIMATION animation, void* target, intptr_t id, void *pValue);
        void onEnter(const Event* event, const Transition *_transition);
        void onLeave(const Event* event, const Transition *_transition);

    private:
        MGEFF_ANIMATION m_animation;
        TransitionAnimationFinished *m_transAnimationTransposition;
};

#endif /* IPHONEDESKTOPSTATEPAGETURNING_HH */

