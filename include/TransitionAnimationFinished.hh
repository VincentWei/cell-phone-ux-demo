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
/** 
 * @file TransitionAnimationFinished.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TRANSITIONANIMATIONFINISHED_HH
#define TRANSITIONANIMATIONFINISHED_HH

#include "StateMachine.hh"
#include <mgeff/mgeff.h>
using namespace mStateMachine;

/** 
 * @synopsis  TransitionAnimationFinished
 */
class TransitionAnimationFinished : public SignalTransition {
public:
    TransitionAnimationFinished(MGEFF_ANIMATION animation, State *source, State *target, const char *name=NULL);
    ~TransitionAnimationFinished();
    void setAnimation(MGEFF_ANIMATION animation);
private:
    static void onAnimationFinished(MGEFF_ANIMATION animation);
};

#endif /* TRANSITIONANIMATIONFINISHED_HH */
