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
 * @file TransitionNoInput.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  TRANSITIONNOINPUT_INC
#define  TRANSITIONNOINPUT_INC

#include <ctime>
#include "StateMachine.hh"
using namespace mStateMachine;

/** 
 * @synopsis  TransitionNoInput
 */
class TransitionNoInput : public EventTransition
{
public:
    TransitionNoInput(State *source, State *target, 
            unsigned int timeoutSec, const char *name="");
    ~TransitionNoInput();

private:
    virtual void reset();
    bool testMouse(const MouseEvent *);
    bool testMove(const MoveEvent *);
    bool testTimer(const TimerEvent *);

private:
    time_t  m_timeOut;
    time_t  m_timeOnEnter;
};


#endif   /* ----- #ifndef TRANSITIONNOINPUT_INC  ----- */
