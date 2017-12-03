/** 
 * @file IPhoneDesktopStatePickup.hh
 * @synopsis  
 * @author ZouQiZhi
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATEPICKUP_HH
#define IPHONEDESKTOPSTATEPICKUP_HH

#include "IPhoneDesktopState.hh"
#include <mgeff/mgeff.h>

class IPhoneDesktopStatePickup : public IPhoneDesktopState {
public:
    IPhoneDesktopStatePickup (IPhoneDesktopController *, State *, const char *name=NULL);
    ~IPhoneDesktopStatePickup ();
    const POINT& offsetPosition() const
    {
        return m_offsetPos;
    }
    int originIndex() const
    {
        return m_originIndex;
    }
    void setOriginIndex(int index) 
    {
        m_originIndex = index;
    }
    void createChildStates();

private:
    void onEnter(const Event *event, const Transition *transition);
    void onLeave(const Event *event, const Transition *transition);

    static void s_callSetItemAlpha (MGEFF_ANIMATION animation, void *target, int id, void *pValue);
    static void s_callSetItemScale (MGEFF_ANIMATION animation, void *target, int id, void *pValue);
    static void s_callSetItemPos (MGEFF_ANIMATION animation, void *target, int id, void *pValue);
    static void s_transitionPickupMove(void *object, Transition *_transition, const Event* event, void *param);

private:
    POINT m_offsetPos;
    int  m_originIndex;
public:
    enum {
        /*
        S51_Pickedup,
        */
        S511_PickedNormal,
        S512_Transposition,
        S513_PageTurning,
        S_NR
    };
    IPhoneDesktopState *states[S_NR];
};

#endif /* IPHONEDESKTOPSTATEPICKUP_HH */
