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
 * @file ScrollPageTrigger.hh
 * @Synopsis  
 * @author humingming
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef SCROLLPAGETRIGGER_HH
#define SCROLLPAGETRIGGER_HH

#include "TransitionSpeedMeasure.hh"
#include <mgeff/mgeff.h>

#define SCROLLPAGE_ID    100


typedef struct _ScrollParam {
    IPhoneDesktopController* controller;
    MGEFF_ANIMATION animation;
}ScrollParam;


class ScrollPageTrigger 
{
public:
    ScrollPageTrigger ();
    ~ScrollPageTrigger ();

    static void onScrollPageTrigger (void* object, Transition* trans, const Event* event, void* param);
    static void animationCallback (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value);

};

#endif /* end if SCROLLPAGETRIGGER_HH */ 


