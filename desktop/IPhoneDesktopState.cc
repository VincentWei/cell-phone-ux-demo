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
