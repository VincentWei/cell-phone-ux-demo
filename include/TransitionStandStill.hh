/** 
 * @file TransitionStandStill.hh
 * @synopsis  
 * @author ???
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TRANSITIONSTANDSTILL_HH
#define TRANSITIONSTANDSTILL_HH

#include "StateMachine.hh"
using namespace mStateMachine;

/** 
 * @synopsis  TransitionStandStill
 */
class TransitionStandStill : public EventTransition {
public:
    TransitionStandStill(State *source, State *target, int timeoutMs, const char *name);
    virtual ~TransitionStandStill();
private:
    virtual void reset();
    bool testMouse(const MouseEvent *);
    bool testMove(const MoveEvent *);
    bool testTimer(const TimerEvent *);
protected:
    POINT m_point;
private:
    int m_timeout;
    int m_status;
    int m_timeOnEnter;
};

#endif /* TRANSITIONSTANDSTILL_HH */
