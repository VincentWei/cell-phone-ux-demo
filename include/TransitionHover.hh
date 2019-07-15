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
 * @file TransitionHover.hh
 * @synopsis  
 * @author ???
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TRANSITIONHOVER_HH
#define TRANSITIONHOVER_HH

#include "StateMachine.hh"
using namespace mStateMachine;

/** 
 * @synopsis  TransitionHover
 */
class TransitionHover : public EventTransition {
public:
    TransitionHover (State *source, State *target, RECT rc, int timeoutMs, const char *name);
    virtual ~TransitionHover();
    RECT getRect() { return m_rc;}
private:
    virtual void reset();
    bool testMove(const MoveEvent *);
    bool testTimer(const TimerEvent *);
private:
    RECT m_rc;
    int m_timeout;
    int m_status;
    int m_timeOnEnter;
};

#endif /* TRANSITIONHOVER_HH */
