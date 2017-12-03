/** 
 * @file TransitionClickOnItem.hh
 * @synopsis  synopsis
 * @author ???
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TRANSITIONCLICKONITEM_HH
#define TRANSITIONCLICKONITEM_HH

#include "StateMachine.hh"
#include "IPhoneDesktopTransitionInterface.hh"

class TransitionClickOnItem : public EventTransition, public IPhoneDesktopTransitionInterface {
public:
    TransitionClickOnItem(State *source, State *target, IPhoneDesktopController *controller, const char *name=NULL);
    ~TransitionClickOnItem();
private:
    bool testMove(const MoveEvent *MoveEvent);
    bool testMouse(const MouseEvent *event);
};

#endif /* TRANSITIONCLICKONITEM_HH */
