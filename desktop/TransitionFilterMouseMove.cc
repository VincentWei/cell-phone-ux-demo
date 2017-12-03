/*!============================================================================
 * @file TransitionFilterMouseMove.cc
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <stdlib.h>
#include "TransitionFilterMouseMove.hh"

#define FILTERRANGE   20
#define FILTERTIME    20


TransitionFilterMouseMove::TransitionFilterMouseMove(State* source, State* target, IPhoneDesktopController *controller, const char* name)
    : EventTransition(Event::MouseEvent, source, target, name), IPhoneDesktopTransitionInterface(controller)
{
    setButtonType(MouseEvent::ButtonReleased);
    setButtons(MouseEvent::LeftButton);

    m_startTick = 0; 
    m_startPos.x = 0;
    m_startPos.y = 0;

    setStartTick();
    setStartPos();
}

TransitionFilterMouseMove::~TransitionFilterMouseMove() 
{
}

bool TransitionFilterMouseMove::testMouse(const MouseEvent* event) 
{
    bool ret = false;
    POINT pt;
    int abs_dx = 0;
    unsigned int tick; 
    IPhoneDesktopItem* item;
    MouseEvent* mouseEvent;

    if ( !EventTransition::testMouse(event) ) {
        return false;
    }

    GetCursorPos(&pt);
    tick = GetTickCount();

    if (MouseEvent::ButtonReleased == event->type()) {
        printf("filterMouseMove: tick:(%d -- %d), pt:(%d,%d) -- (%d,%d)\n", 
                m_startTick, tick, 
                m_startPos.x, m_startPos.y, pt.x, pt.y); 

        abs_dx = abs(pt.x - m_startPos.x) + abs(pt.y - m_startPos.y);
        if ( (tick - m_startTick <= FILTERTIME) && (abs_dx <= FILTERRANGE) ) {
            item = point2Item(pt);
            if (NULL != item) {
                mouseEvent = const_cast<MouseEvent*>(event);
                mouseEvent->appendParameter(std::string(item->name()));
                ret = true;
            }
        }
    }

    return ret;
}

