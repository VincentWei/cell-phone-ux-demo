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
 * @file IPhoneDesktopController.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPCONTROLLER_H
#define IPHONEDESKTOPCONTROLLER_H

#include "IPhoneDesktopView.hh"
#include "IPhoneDesktopLayout.hh"
#include "StateMachine.hh"
using namespace mStateMachine;

/** 
 * @synopsis IPhoneDesktopController 
 */
class IPhoneDesktopController {
    public:
        static int create(HWND hwnd);
        ~IPhoneDesktopController();

        static IPhoneDesktopController *getControllerFromHwnd(HWND hwnd);
        static LRESULT wndProcWrapper(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

        Scene &getScene();
        IPhoneDesktopLayout &getLayout();
        IPhoneDesktopView &getView();
        HWND getHwnd()          { return m_hwnd;}
        StateMachine& stateMachine() {return m_stateMachine;}

    public:
        enum {
            S1_Normal,
            S2_App,
            S3_Standby,
            S4_Shifting,
            S5_Shaking,
            /*
               S41__Draging,
               S42__Inertia,
               */
            /*
            */
            S_NR
        };
        State *states[S_NR];
    private:
        IPhoneDesktopController(HWND hwnd);
        int wndProc(HWND hwnd, int message, WPARAM wparam, LPARAM lparam);
        void createItems();
        void createStates();
    private:
        Scene m_scene;
        IPhoneDesktopView m_view;
        IPhoneDesktopLayout m_layout;
        StateMachine m_stateMachine;
        POINT m_mousePosition;
        bool m_buttonPressed;
        HWND m_hwnd;
};

#endif /* IPHONEDESKTOPCONTROLLER_H */
