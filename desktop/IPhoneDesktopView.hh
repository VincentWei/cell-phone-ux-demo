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
 * @file IPhoneDesktopView.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPVIEW_H
#define IPHONEDESKTOPVIEW_H

#include "GraphicsView.hh"
#include <mgeff/mgeff.h>
using namespace GraphicsView;

class IPhoneDesktopView : public View {
public:
    IPhoneDesktopView(Scene *scene, HWND hwnd);
    ~IPhoneDesktopView();

    static void callRedraw(MGEFF_ANIMATION animation, void *target, intptr_t id, void *pValue);

    void onDraw(HDC dc) { View::onDraw(dc); }
    void invalidateRect(const RECT *rc=NULL);

    const RECT& getAnimationArea() const {return m_animationArea;};
    void    setAnimationArea(const RECT& rc) {m_animationArea = rc;}
protected:
    void onDraw(HDC dc, const RECT &rc);
    void onDrawBackground(HDC dc, const RECT &rc);
private:
    int calcPageBarX(int current, int total) const;

    bool m_lButtonDown;
    POINT m_lastPoint;
    // Area for some view level animation performance.
    // In current, this area exclude bottom tool bar of desktop.
    RECT m_animationArea; 
};

#endif /* IPHONEDESKTOPVIEW_H */
