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
 * @file TransitionSpeedMeasure.cc
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include "TransitionSpeedMeasure.hh"
#include "global.h"

#define SPEEDMEASURE_TIMERID  2000

TransitionSpeedMeasure::TransitionSpeedMeasure(State* source, State* target, IPhoneDesktopController* controller, const char* name)
#ifndef SPEEDTEST_NEW
    : EventTransition(Event::MouseEvent | Event::TimerEvent | Event::MoveEvent, source, target, name)
#else
    : EventTransition(Event::MouseEvent | Event::MoveEvent, source, target, name)
#endif
{
    setButtonType(MouseEvent::ButtonPressed | MouseEvent::ButtonReleased);
    setButtons(MouseEvent::LeftButton);

    m_curSpeed = 0.0; 
    m_speedDownLimit = 0.0;
    m_speedUpLimit = 9999.0;
#ifndef SPEEDTEST_NEW
    m_speedhandle = speedmeter_create(16);
    printf("hwnd=%d.\n", controller->getHwnd());
    SetTimer(controller->getHwnd(), SPEEDMEASURE_TIMERID, 1);
#endif
}


TransitionSpeedMeasure::~TransitionSpeedMeasure() 
{
#ifndef SPEEDTEST_NEW
    speedmeter_destroy(m_speedhandle);
#endif
}


void TransitionSpeedMeasure::setSpeedRect(const float down, const float up)
{
    m_speedDownLimit = down;
    m_speedUpLimit = up;

    if (m_speedDownLimit < 0.0)
        m_speedDownLimit = 0.0 - m_speedDownLimit; 

    if (m_speedUpLimit < 0.0)
        m_speedUpLimit = 0.0 - m_speedUpLimit;

    if (m_speedUpLimit <= m_speedDownLimit)
        m_speedUpLimit = m_speedDownLimit + 0.1;
}


void TransitionSpeedMeasure::getSpeedRect(float& down, float& up) const
{
    down = m_speedDownLimit;
    up = m_speedUpLimit;
}


bool TransitionSpeedMeasure::testMouse(const MouseEvent* event) 
{
    bool ret = false;
    if (! EventTransition::testMouse(event)) {
        return false;
    }

    if (MouseEvent::ButtonReleased == event->type()) {
        float abs_speed;
#ifndef SPEEDTEST_NEW
        m_curSpeed = speedmeter_calc (m_speedhandle);
        abs_speed = m_curSpeed;
        if (abs_speed < 0.0)
            abs_speed = 0.0 - abs_speed;

        if (abs_speed >= m_speedDownLimit && abs_speed <= m_speedUpLimit)
            ret = true;

        printf("speed_move: %.4f, %.4f, (%.2f, %.2f), %d\n", abs_speed, m_curSpeed,
                m_speedDownLimit, m_speedUpLimit, ret);
#else
         float v_x, v_y;
         QueryMouseMoveVelocity(&v_x, &v_y);
         if (v_y > 10000.0f) {
             printf("v_y=%.2f, set to 10000 forcely\n", v_y);
             v_y = 9999.0f;
         }else if (v_y < -10000.0f) {
             printf("v_y=%.2f, set to -10000 forcely\n", v_y);
             v_y = -9999.0f;
         }
         m_curSpeed = v_x ;
         abs_speed = m_curSpeed;
         if (abs_speed < 0.0)
            abs_speed = 0.0 - abs_speed;

        if (abs_speed >= m_speedDownLimit && abs_speed <= m_speedUpLimit)
            ret = true;
         printf("ret =%d, v_x=%.2f, v_y=%.2f, abs_speed = %0.2f\n", 
                 ret, v_x, v_y, abs_speed);
#endif
    }

    return ret;
}


bool TransitionSpeedMeasure::testTimer(const TimerEvent *event)
{
    TimerEvent* e = (TimerEvent*)event;
    if (e->id() == SPEEDMEASURE_TIMERID) {
        POINT pt;
        GetCursorPos(&pt);
#ifndef SPEEDTEST_NEW
        speedmeter_add(m_speedhandle, pt.x, (unsigned int)e->tickCount());
#endif
    }
    return false;
}


