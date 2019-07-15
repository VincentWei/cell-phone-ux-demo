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
 * @file IPhoneDestktopStateShifting.cc
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include "IPhoneDesktopStateShifting.hh"
#include "IPhoneDesktopStateDraging.hh"
#include "IPhoneDesktopStateInertia.hh"
#include "TransitionSpeedMeasure.hh"
#include "TransitionAnimationFinished.hh"
#include "DesktopCommonProc.hh"
#include "global.h"


IPhoneDesktopStateShifting::IPhoneDesktopStateShifting(IPhoneDesktopController* controller, StateMachine* stateMachine, const char* name)
    : IPhoneDesktopState(controller, stateMachine, name)
{
    dragingScrollParam.controller = m_controller;
    dragingScrollParam.animation = mGEffAnimationCreate (m_controller, 
            ScrollPageTrigger::animationCallback, SCROLLPAGE_ID, MGEFF_INT);

    inertiaScrollParam.controller = m_controller;
    inertiaScrollParam.animation = mGEffAnimationCreate (m_controller, 
            ScrollPageTrigger::animationCallback, SCROLLPAGE_ID, MGEFF_INT);

    transFilter = NULL;
}


IPhoneDesktopStateShifting::IPhoneDesktopStateShifting(IPhoneDesktopController* controller, State* parentState, const char* name)
    : IPhoneDesktopState(controller, parentState, name)
{
    dragingScrollParam.controller = m_controller;
    dragingScrollParam.animation = mGEffAnimationCreate (m_controller, 
            ScrollPageTrigger::animationCallback, SCROLLPAGE_ID, MGEFF_INT);

    inertiaScrollParam.controller = m_controller;
    inertiaScrollParam.animation = mGEffAnimationCreate (m_controller, 
            ScrollPageTrigger::animationCallback, SCROLLPAGE_ID, MGEFF_INT);

    transFilter = NULL;
}


IPhoneDesktopStateShifting::~IPhoneDesktopStateShifting() 
{
    HWND hwnd = m_controller->getHwnd();

    mGEffAnimationSetFinishedCb(dragingScrollParam.animation, NULL);
    if (mGEffAnimationGetProperty(dragingScrollParam.animation, 
                MGEFF_PROP_STATE) == MGEFF_STATE_RUNNING) {
        mGEffAnimationWait (&hwnd, dragingScrollParam.animation);
    }
    mGEffAnimationDelete (dragingScrollParam.animation);

    mGEffAnimationSetFinishedCb(inertiaScrollParam.animation, NULL);
    if (mGEffAnimationGetProperty(inertiaScrollParam.animation,
                MGEFF_PROP_STATE) == MGEFF_STATE_RUNNING) {
        mGEffAnimationWait (&hwnd, inertiaScrollParam.animation);
    }
    mGEffAnimationDelete (inertiaScrollParam.animation);
}


void IPhoneDesktopStateShifting::onEnter(const Event *event, const Transition *transition)
{
    if (NULL != transFilter) {
        transFilter->setStartTick();
        transFilter->setStartPos();
    }
}


/*void IPhoneDesktopStateShifting::onLeave(const Event *event, const Transition *transition)
{
}*/


void IPhoneDesktopStateShifting::createChildStates(State* returnState, State* appState) 
{
    if (NULL == returnState)
        return;

    EventTransition *trans;
    TransitionSpeedMeasure* transSpeed;
    //TransitionAnimationFinished* transAnimation;
    IPhoneDesktopStateDraging* stateDraging;
    IPhoneDesktopStateInertia* stateInertia;

    stateDraging = new IPhoneDesktopStateDraging (m_controller, this, "State::Shifting Draging");
    stateInertia = new IPhoneDesktopStateInertia (m_controller, this, "State::Shifting Inertia");


    trans = new EventTransition(Event::MoveEvent, stateDraging, NULL, "S41_Draging->Targetless");
    trans->subscribe (stateInertia, IPhoneDesktopStateDraging::onDragingTrigger, m_controller);

    if (NULL != appState) {
        transFilter = new TransitionFilterMouseMove(stateDraging, appState, m_controller, "S41_Draging->S2_APP");
        transFilter->subscribe(stateDraging, transition_clicked, NULL);
    }

    transSpeed = new TransitionSpeedMeasure(stateDraging, stateInertia, m_controller, 
            "TransitionSpeedMeasur::S41_Draging->S42_Inertia.");
    transSpeed->setSpeedRect (HIGHSPEED_DOWN, HIGHSPEED_UP);
    transSpeed->subscribe (stateDraging, ScrollPageTrigger::onScrollPageTrigger, &inertiaScrollParam);


    std::string name  = std::string("TransitionSpeedMeasure::S41_Draging->") + returnState->name(); 
    transSpeed = new TransitionSpeedMeasure(stateDraging, returnState, m_controller, name.c_str());
    transSpeed->setSpeedRect (LOWSPEED_DOWN, LOWSPEED_UP);
    transSpeed->subscribe (stateDraging, ScrollPageTrigger::onScrollPageTrigger, &dragingScrollParam);


    name  = std::string("TransitionAnimationFinished::S42_Inertia->") + returnState->name(); 
    /*transAnimation = */new TransitionAnimationFinished(inertiaScrollParam.animation, 
            stateInertia, returnState, name.c_str());


    trans = new EventTransition(Event::MouseEvent, stateInertia, stateDraging, 
            "EventTransition::S42_Intertia->S41_Draging");
    trans->setButtonType(MouseEvent::ButtonPressed);
    trans->setButtons(MouseEvent::LeftButton);
    trans->subscribe (stateInertia, IPhoneDesktopStateInertia::onStopInertiaAnimationTrigger, 
            inertiaScrollParam.animation);


    setInitialState (stateDraging);
}


