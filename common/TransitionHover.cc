#include <assert.h>
#include "TransitionHover.hh"
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "global.h"

TransitionHover::TransitionHover(State *source, State *target, RECT rc, int timeoutMs, const char *name)
    : EventTransition(Event::MoveEvent | Event::TimerEvent, source, target, name), 
    m_rc(rc), 
    m_timeout(timeoutMs), 
    m_status(0), 
    m_timeOnEnter(0)
{
    assert(m_timeout > 0);
}

TransitionHover::~TransitionHover() {
}

void TransitionHover::reset() {
    m_status = 0;
}

bool TransitionHover::testMove(const MoveEvent *event) {
    POINT pt = event->pos();
    if (m_status == 0 && PtInRect(&m_rc, pt.x, pt.y)) {
        m_timeOnEnter = MyGetTickCount();
        m_status = 1;
    }
    return false;
}

bool TransitionHover::testTimer(const TimerEvent *event) {
    if (m_status == 1 && (int)(MyGetTickCount() - m_timeOnEnter) > m_timeout/10) {
        m_status = 2;
        return true;
    }else{
        return false;
    }
}
