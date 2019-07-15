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
 * @file TransitionFilterMouseMove.hh
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TRANSITIONFILTERMOUSEMOVE_HH
#define TRANSITIONFILTERMOUSEMOVE_HH

#include "StateMachine.hh"
#include "IPhoneDesktopController.hh"
#include "IPhoneDesktopTransitionInterface.hh"


class TransitionFilterMouseMove : public EventTransition, public IPhoneDesktopTransitionInterface
{
public:
    TransitionFilterMouseMove(State* source, State* target, IPhoneDesktopController *controller, const char* name=NULL);
    ~TransitionFilterMouseMove();

    void setStartTick(void) { m_startTick = GetTickCount(); }
    void setStartPos(void) { GetCursorPos(&m_startPos); }

protected:
    bool testMouse(const MouseEvent *);

private:
    unsigned int m_startTick;
    POINT m_startPos;
};


#endif /* TRANSITIONFILTERMOUSEMOVE_HH */


