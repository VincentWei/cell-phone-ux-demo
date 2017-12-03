/** 
 * @file IPhoneDesktopStateShaking.hh
 * @synopsis  
 * @author ZouQiZhi
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATESHAKING_HH
#define IPHONEDESKTOPSTATESHAKING_HH

#include "IPhoneDesktopState.hh"
#include <mgeff/mgeff.h>

class IPhoneDesktopStateShaking : public IPhoneDesktopState {
public:
    IPhoneDesktopStateShaking(IPhoneDesktopController *, StateMachine *, const char *name=NULL);
    ~IPhoneDesktopStateShaking();
    void createChildStates();
private:
    void onEnter(const Event *event, const Transition *transition);
    void onLeave(const Event *event, const Transition *transition);
    static void callSetItemStatus(MGEFF_ANIMATION, void *, int, void *);
public:
    enum {
        S51_Pickedup,
        /*
        S511_PickedNormal,
        S512_Transposition,
        S513_PageTurning,
        */
        S52_Putdown,
        S53_ShakingAndScrolling,
        S_NR
    };
    IPhoneDesktopState *states[S_NR];
private:
    MGEFF_ANIMATION m_animation;
};

#endif /* IPHONEDESKTOPSTATESHAKING_HH */
