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
#ifndef _ERASER_H
#define _ERASER_H

#include <chipmunk/chipmunk.h>
#include <vector>
using std::vector;

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgeff/mgeff.h>

namespace eraser {

class CpObject {
public:
    ~CpObject();
    const cpVect& pos() const { return m_body.p; }
    const cpVect& v() const { return m_body.v; }
protected:
    CpObject();
protected:
    cpBody m_body;
};

class Point : public CpObject {
public:
    Point(int origX, int origY, unsigned int pixel);
    ~Point();
    unsigned int pixel() const { return m_pixel; }
    const cpCircleShape& shape() const { return m_shape; }
private:
    int m_origX;
    int m_origY;
    unsigned int m_pixel;
    cpCircleShape m_shape;
};

class Controller {
public:
    Controller(HWND hwnd, int w, int h);
    ~Controller();

    enum {
        FOREGROUND = 0,
        BACKGROUND,
        NR_DC
    };
    HDC getDC(int index) const;
    void setup();
    void paint(HDC hdc);
private:
    static int s_onCalc(MGEFF_ANIMATION anim, cpSpace *space, void *_self);
    static void s_onDraw(MGEFF_ANIMATION anim, cpSpace *space, void *_self);
    static void s_finishCB(MGEFF_ANIMATION handle);

    int onCalc();
    void onDraw();
    void finishCB();

    void generatePointsList();
private:
    HWND m_hwnd;
    cpSpace m_space;
    vector<Point> m_points;
    MGEFF_ANIMATION m_animation;
    int m_w;
    int m_h;
    HDC m_dc[NR_DC];
};

};

#endif /* _ERASER_H */
