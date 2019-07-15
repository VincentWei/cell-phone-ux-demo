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
#include "IPhoneDesktopState.hh"

IPhoneDesktopState::IPhoneDesktopState(IPhoneDesktopController *controller, StateMachine *machine, const char *name)
    : State(machine, name), m_controller(controller)
{
}

IPhoneDesktopState::IPhoneDesktopState(IPhoneDesktopController *controller, State *parentState, const char *name)
    : State(parentState, name), m_controller(controller)
{
}

IPhoneDesktopState::~IPhoneDesktopState() {
}
