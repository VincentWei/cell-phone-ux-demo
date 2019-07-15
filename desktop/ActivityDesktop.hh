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
 * @file ActivityDesktop.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef ACTIVITYDESKTOP_H
#define ACTIVITYDESKTOP_H

#include "global.h"
#include "Activity.hh"
#include "DesktopToolbar.hh"

class IPhoneDesktopController;
class IPhoneDesktopItem;
class IPhoneDesktopController;
/** 
 * @synopsis  ActivityDesktop
 */
class ActivityDesktop : public Activity
{
public:
    static LRESULT DesktopProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static HWND getDesktopHwnd();

    static void TryStopShaking(mObject*);
    static BOOL evt_toolbar_menu(mObject* self, mObject *piece,
        int event_id, DWORD param);
    static BOOL evt_toolbar_call(mObject* self, mObject *piece,
        int event_id, DWORD param);
    static BOOL evt_toolbar_sms(mObject* self, mObject *piece,
        int event_id, DWORD param);

    static void animationFinishedCb(MGEFF_ANIMATION animation);
    static void onStateItemAround(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
    static void onStateItemCenter(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
    static void onStateApp(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
    static void onMoveToolBar(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
    static void onStatePageBar(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);
    MGEFF_ANIMATION pageBarAnimationCreate(void *target, int duration, int zoomout);

    ActivityDesktop();
    ~ActivityDesktop();
    void show();

    void createToolbar(HWND parent);

    BOOL m_hasBeenPoped;

protected:
    virtual int onStart();
    virtual int onStop();
    virtual int onPause();
    virtual int onResume();

    void setAnimation(Activity* activity, HWND, int zoomout);

protected:
    static HWND s_desktop;
    HWND createHWND();

    DesktopToolbar* m_toolbar;

    MGEFF_ANIMATION m_animation;
    IPhoneDesktopItem* m_items[ITEMS_PER_PAGE];
    IPhoneDesktopController* m_controller;
};

#endif
