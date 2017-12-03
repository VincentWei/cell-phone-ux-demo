/*! ============================================================================
 * @file TransitionCollideItem.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include "global.h"
#include "TransitionCollideItem.hh"
#include "IPhoneDesktopStateShaking.hh"
#include "IPhoneDesktopView.hh"

TransitionCollideItem::TransitionCollideItem(State *source, State *target, 
        IPhoneDesktopController *controller, const char *name)
    : EventTransition(Event::MoveEvent | Event::TimerEvent | Event::MouseEvent, source, target, name),
    IPhoneDesktopTransitionInterface(controller),
      m_controller(controller), m_status(TransitionCollideItem::INITIAL),
      m_expectPage(0), m_expectIndex(0), m_timeOut(1), m_timeOnEnter(time(NULL)), m_isCollide(false)
{

    setButtonType(MouseEvent::ButtonReleased);
    setButtons(MouseEvent::LeftButton);

    IPhoneDesktopStateShaking* p = dynamic_cast<IPhoneDesktopStateShaking*>(m_controller->states[IPhoneDesktopController::S5_Shaking]);
    assert(NULL != p);
    m_sourceState = dynamic_cast<IPhoneDesktopStatePickup*>(p->states[IPhoneDesktopStateShaking::S51_Pickedup]);
    assert(NULL != m_sourceState);

    m_expectPostion.x = m_expectPostion.y = 0;
    m_itemPos.x = m_itemPos.y = 0;
    m_tick = 0;
}

TransitionCollideItem::~TransitionCollideItem() {
}

bool TransitionCollideItem::testTimer(const TimerEvent *event) {
    if (m_isCollide) {
        // TODO:Not use the time function because it's precision is too low.
        // if ( time(NULL) - m_timeOnEnter >= m_timeOut ) {
        if ( 1||(++m_tick > 3) ) {
            m_expectPostion = m_itemPos;
            const RECT& view_rc = m_controller->getView().getRect();
            if (m_expectPostion.x < view_rc.left) m_expectPostion.x = view_rc.left;
            if (m_expectPostion.y < view_rc.top) m_expectPostion.y = view_rc.top;
            if (m_expectPostion.x >= RECTW(view_rc)) m_expectPostion.x = RECTW(view_rc) - 1;
            if (m_expectPostion.y >= RECTH(view_rc)) m_expectPostion.y = RECTH(view_rc) - 1;

            m_controller->getView().ViewToScene(m_itemPos, m_expectPostion);
            m_sourceState->setOriginIndex(m_expectIndex);
            m_isCollide = false;
            return true;
        }
    }
    return false;
}

bool TransitionCollideItem::testMove(const MoveEvent* event)
{
    if (NULL != m_controller) {
        if (TransitionCollideItem::INITIAL == m_status) {
            m_status = TransitionCollideItem::MOVING;
            resetTimer();
        }
        else if(!m_isCollide && TransitionCollideItem::MOVING == m_status) {
            int curPage = m_expectPage;
            m_itemPos.x = event->pos().x - m_sourceState->offsetPosition().x;
            m_itemPos.y = event->pos().y - m_sourceState->offsetPosition().y;
            m_controller->getLayout().point2Index(m_itemPos, curPage, m_expectIndex);

            if ((m_sourceState->originIndex() != -1 && m_sourceState->originIndex() != m_expectIndex)
                    || curPage != m_expectPage) {
                resetTimer();
                m_expectPage = curPage;
                m_isCollide = true;
            }
        } else {
        }
    }
    return false;
}

bool TransitionCollideItem::testMouse(const MouseEvent *event) {

    if (! EventTransition::testMouse(event)) {
        return false;
    }

    if (event->type() == MouseEvent::ButtonReleased) {
        m_isCollide = false;
    }

    return false;
}

