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
 * @file TransitionCollideItem.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  TRANSITIONCOLLIDEITEM_INC
#define  TRANSITIONCOLLIDEITEM_INC

#include <ctime>
#include <minigui/common.h>
#include "StateMachine.hh"
#include "IPhoneDesktopController.hh"
#include "IPhoneDesktopTransitionInterface.hh"
#include "IPhoneDesktopStatePickup.hh"

class TransitionCollideItem : public EventTransition, public IPhoneDesktopTransitionInterface
{
public:
    enum CollideStatus {
        INITIAL,
        MOVING,
        DISSOCIATION
    };
    TransitionCollideItem(State *source, State *target, IPhoneDesktopController *controller, const char *name="");
    ~TransitionCollideItem();
    const POINT& expectPosition() const
    {
        return m_expectPostion;
    }

private:
    bool testMove(const MoveEvent *);
    bool testTimer(const TimerEvent *event);
    bool testMouse(const MouseEvent *event);
    inline void resetTimer() {m_tick = 0; m_timeOnEnter = time(NULL);}

private:
    IPhoneDesktopController* m_controller;
    bool m_status;
    int  m_expectPage;
    int  m_expectIndex;
    time_t  m_timeOut;
    time_t  m_timeOnEnter;
    bool m_isCollide;
    IPhoneDesktopStatePickup* m_sourceState;
    POINT m_expectPostion;
    POINT m_itemPos;
    int m_tick;
};

#endif   /* ----- #ifndef TRANSITIONCOLLIDEITEM_INC  ----- */

