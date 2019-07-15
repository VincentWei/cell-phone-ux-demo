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
 * @file IPhoneDesktopTransitionInterface.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPTRANSITIONINTERFACE_HH
#define IPHONEDESKTOPTRANSITIONINTERFACE_HH

#include "IPhoneDesktopController.hh"
#include "IPhoneDesktopItem.hh"

class IPhoneDesktopTransitionInterface {
public:
    IPhoneDesktopTransitionInterface(IPhoneDesktopController *controller) : m_controller(controller), m_item(NULL) {}
    IPhoneDesktopItem *point2Item(const POINT &point);
    IPhoneDesktopItem *item() const { return m_item; }
protected:
    void setItem(IPhoneDesktopItem *item) { m_item = item; }
protected:
    IPhoneDesktopController *m_controller;
private:
    IPhoneDesktopItem *m_item;
};

#endif /* IPHONEDESKTOPTRANSITIONINTERFACE_HH */
