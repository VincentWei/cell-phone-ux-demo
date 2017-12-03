/** 
 * @file IPhoneDesktopState.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATE_HH
#define IPHONEDESKTOPSTATE_HH

#include "StateMachine.hh"
#include "IPhoneDesktopController.hh"
using namespace mStateMachine;

class IPhoneDesktopState : public State {
public:
    IPhoneDesktopState(IPhoneDesktopController *controller, StateMachine *machine, const char *name=NULL);
    IPhoneDesktopState(IPhoneDesktopController *controller, State *parentState, const char *name=NULL);
    virtual ~IPhoneDesktopState();
protected:
    IPhoneDesktopController *m_controller;
};

#endif /* IPHONEDESKTOPSTATE_HH */
