/*!============================================================================
 * @file IPhoneDesktopStateTransposition.hh
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATETRANSPOSITION_HH
#define IPHONEDESKTOPSTATETRANSPOSITION_HH

#include "IPhoneDesktopState.hh"
#include "TransitionAnimationFinished.hh"
#include <mgeff/mgeff.h>

class IPhoneDesktopStateTransposition : public IPhoneDesktopState {
    public:
        IPhoneDesktopStateTransposition(IPhoneDesktopController *controller, State *parentState, const char *name);
        ~IPhoneDesktopStateTransposition() {}

    private:
        static void onTransposition(MGEFF_ANIMATION animation, void* target, int id, void *pValue);
        void onEnter(const Event* event, const Transition *_transition);
        void onLeave(const Event* event, const Transition *_transition);

    private:
        MGEFF_ANIMATION m_animation;
        TransitionAnimationFinished *m_transAnimationTransposition;
};

#endif /* IPHONEDESKTOPSTATETRANSPOSITION_HH */

