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
 * @file IPhoneDesktopStateApp.hh
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATEAPP_HH
#define IPHONEDESKTOPSTATEAPP_HH

#include "global.h"
#include "IPhoneDesktopState.hh"
#include "IPhoneDesktopItem.hh"
#include <mgeff/mgeff.h>

class IPhoneDesktopStateApp : public IPhoneDesktopState {
    public:
        IPhoneDesktopStateApp(IPhoneDesktopController *controller, StateMachine *stateMachine, const char *name);
        ~IPhoneDesktopStateApp();

    private:
        static void animationFinishedCb(MGEFF_ANIMATION animation);
        static void onStateItemAround(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
        static void onStateItemCenter(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
        static void onStateApp(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
        void setAnimation(const Event *event, const Transition *_transition, int zoomout);
        inline void onEnter(const Event* event, const Transition *_transition) 
        {
//            setAnimation(event, _transition, 1);
        };
        inline void onLeave(const Event* event, const Transition *_transition)
        {
//            setAnimation(event, _transition, 0);
        };

    private:
        MGEFF_ANIMATION m_animation;
        IPhoneDesktopItem *m_items[ITEMS_PER_PAGE];
};

#endif /* IPHONEDESKTOPSTATEAPP_HH */
