/** 
 * @file IPhoneDesktopStateNormal.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATENORMAL_HH
#define IPHONEDESKTOPSTATENORMAL_HH

#include "IPhoneDesktopState.hh"

class IPhoneDesktopStateNormal : public IPhoneDesktopState {
public:
    IPhoneDesktopStateNormal(IPhoneDesktopController *controller, StateMachine *stateMachine, const char *name=NULL);
    ~IPhoneDesktopStateNormal();
private:
    static void s_onClickOnItem(void *object, Transition *transition, const Event* event, void *param);
};

#endif /* IPHONEDESKTOPSTATENORMAL_HH */
