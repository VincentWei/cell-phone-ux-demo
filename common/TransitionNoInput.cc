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
/*! ============================================================================
 * @file TransitionNoInput.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include "TransitionNoInput.hh"

TransitionNoInput::TransitionNoInput(State *source, State *target, 
        unsigned int timeoutSec, const char *name)
    : EventTransition(Event::MouseEvent | Event::MoveEvent | Event::TimerEvent, source, target, name),
    m_timeOut(timeoutSec)
{
    assert(timeoutSec > 0);
    m_timeOnEnter = time(NULL);
    setButtonType(MouseEvent::ButtonPressed | MouseEvent::ButtonReleased);
    setButtons(MouseEvent::LeftButton | MouseEvent::RightButton);
}

TransitionNoInput::~TransitionNoInput() {
}

void TransitionNoInput::reset() {
    m_timeOnEnter = time(NULL);
}

bool TransitionNoInput::testMouse(const MouseEvent *event) {
    m_timeOnEnter = time(NULL);
    return false;
}

bool TransitionNoInput::testMove(const MoveEvent *event) {
    m_timeOnEnter = time(NULL);
    return false;
}

bool TransitionNoInput::testTimer(const TimerEvent *event) {
    if ( time(NULL) - m_timeOnEnter >= m_timeOut ) {
        return true;
    }
    return false;
}

