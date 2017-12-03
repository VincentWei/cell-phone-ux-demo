/*!============================================================================
 * @file TransitionSpeedMeasure.hh
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TRANSITIONSPEEDMEASURE_HH
#define TRANSITIONSPEEDMEASURE_HH

#include "StateMachine.hh"
#include "IPhoneDesktopController.hh"
#include "speedmeter.h"

#define DIRECTION_NONE  0
#define DIRECTION_LEFT  1
#define DIRECTION_RIGHT 2

#define SPEEDTEST_NEW 1

#define LOWSPEED_DOWN      0.0
#ifdef SPEEDTEST_NEW
#define LOWSPEED_UP        300.0
#define HIGHSPEED_DOWN     300.0
#else
#define LOWSPEED_UP        0.25
#define HIGHSPEED_DOWN     0.25
#endif
#define HIGHSPEED_UP       9999.0


class TransitionSpeedMeasure : public EventTransition
{
public:
    TransitionSpeedMeasure(State* source, State* target, IPhoneDesktopController* controller, const char* name=NULL);
    ~TransitionSpeedMeasure();

    float getCurSpeed (void) const { return m_curSpeed; }
    void setSpeedRect (const float down, const float up);
    void getSpeedRect (float& down, float& up) const;

protected:
    bool testMouse(const MouseEvent *);
    bool testMove(const MoveEvent *) { return false; } 
    bool testTimer(const TimerEvent *event);

private:
    float m_curSpeed; 
    float m_speedDownLimit;
    float m_speedUpLimit;
    struct _speedmeter* m_speedhandle;
};


#endif /* TRANSITIONSPEEDMEASURE_HH */


