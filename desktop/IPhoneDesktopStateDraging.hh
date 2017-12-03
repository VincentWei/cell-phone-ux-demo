/*!============================================================================
 * @file IPhoneDesktopStateDraging.hh
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPSTATEDRAGING_HH
#define IPHONEDESKTOPSTATEDRAGING_HH

#include "IPhoneDesktopState.hh"


class IPhoneDesktopStateDraging : public IPhoneDesktopState
{
public:
    IPhoneDesktopStateDraging(IPhoneDesktopController* controller, State* parentState, const char* name=NULL);
    ~IPhoneDesktopStateDraging();

    static void onDragingTrigger (void* object, Transition* trans, const Event* evnet, void* param);

};

#endif /* IPHONEDESKTOPSTATEDRAGING_HH */


