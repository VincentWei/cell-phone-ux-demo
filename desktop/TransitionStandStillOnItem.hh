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
/** 
 * @file TransitionStandStillOnItem.hh
 * @synopsis  
 * @author ???
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TRANSITIONSTANDSTILLONITEM_HH
#define TRANSITIONSTANDSTILLONITEM_HH

#include "TransitionStandStill.hh"
#include "IPhoneDesktopController.hh"
#include "IPhoneDesktopTransitionInterface.hh"

class TransitionStandStillOnItem : public TransitionStandStill, public IPhoneDesktopTransitionInterface {
public:
    TransitionStandStillOnItem(State *source, State *target, IPhoneDesktopController *controller, int timeoutMs, const char *name=NULL);
    ~TransitionStandStillOnItem();
private:
    bool eventTest(const Event *);
};

#endif /* TRANSITIONSTANDSTILLONITEM_HH */
