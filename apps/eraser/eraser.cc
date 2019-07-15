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
#include <assert.h>

#include "global.h"

#include "eraser.hh"

#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)

using namespace eraser;

CpObject::CpObject() {
    cpBodyInit(&m_body, 1, 0);
}

CpObject::~CpObject() {
    cpBodyDestroy(&m_body);
}

Point::Point(int origX, int origY, unsigned int pixel)
    : m_origX(origX), m_origY(origY), m_pixel(pixel)
{
    cpCircleShapeInit(&m_shape, &m_body, 0.5, cpvzero);
}

Point::~Point() {
    cpShapeDestroy((cpShape *)&m_shape);
}

Controller::Controller(HWND hwnd, int w, int h)
    : m_hwnd(hwnd), m_w(w), m_h(h)
{
    cpSpaceInit(&m_space);

    {
        int i;
        for (i=0; i<NR_DC; ++i) {
            m_dc[i] = CreateCompatibleDCEx(HDC_SCREEN, m_w, m_h);
        }
    }
}

Controller::~Controller() {
    cpSpaceDestroy(&m_space);
}

HDC Controller::getDC(int index) const {
    return m_dc[index];
}

void Controller::setup() {
    assert(m_points.empty());
    generatePointsList();

    {
        vector<Point>::const_iterator i;
        for (i=m_points.begin(); i!=m_points.end(); ++i) {
            cpSpaceAddShape(&m_space, (cpShape *)&i->shape());
        }
    }

    assert(m_animation == NULL);
    m_animation = phyanim_create(&m_space, this, s_onCalc, s_onDraw);
    mGEffAnimationSetDuration(m_animation, 3600000);
    mGEffAnimationSetContext(m_animation, this);
    mGEffAnimationSetFinishedCb(m_animation, s_finishCB);
    mGEffAnimationAsyncRun(m_animation);
    mGEffAnimationSetProperty(m_animation, MGEFF_PROP_KEEPALIVE, 0);
}

void Controller::paint(HDC hdc) {
    BitBlt(m_dc[BACKGROUND], 0, 0, 0, 0, hdc, 0, 0, -1);

    {
        vector<Point>::const_iterator i;
        for (i=m_points.begin(); i!=m_points.end(); ++i) {
            const cpVect pos = i->pos();
            SetPixel(hdc, pos.x, pos.y, i->pixel());
        }
    }
}

void Controller::generatePointsList() {
    int x, y;
    int w, h;
    unsigned char *buf1, *buf2;
    unsigned int *pixel1, *pixel2;
    int pitch1, pitch2;
    int tmpw, tmph;
    int bpp;
    RECT rc;

    w = GetGDCapability(m_dc[FOREGROUND], GDCAP_MAXX);
    h = GetGDCapability(m_dc[FOREGROUND], GDCAP_MAXY);
    bpp = GetGDCapability(m_dc[FOREGROUND], GDCAP_BPP);
    assert(bpp == 4);
    SetRect(&rc, 0, 0, w, h);

    buf1 = (unsigned char *) LockDC(m_dc[FOREGROUND], &rc, &tmpw, &tmph, &pitch1);
    UnlockDC(m_dc[FOREGROUND]);
    assert(buf1);
    assert(tmpw == w && tmph == h);

    buf2 = (unsigned char *) LockDC(m_dc[BACKGROUND], &rc, &tmpw, &tmph, &pitch2);
    UnlockDC(m_dc[BACKGROUND]);
    assert(buf2);
    assert(tmpw == w && tmph == h);

    for (y=0; y<h; ++y) {
        for (x=0; x<w; ++x) {
            pixel1 = (unsigned int *) (buf1 + y * pitch1 + x * bpp);
            pixel2 = (unsigned int *) (buf2 + y * pitch2 + x * bpp);
            if (*pixel1 != *pixel2) {
                m_points.push_back( Point(x, y, *pixel1) );
            }
        }
    }

    m_w = w;
    m_h = h;
}

int Controller::s_onCalc(MGEFF_ANIMATION anim, cpSpace *space, void *_self){
    Controller *_this = (Controller *)_self;
    return _this->onCalc();
}

void Controller::s_onDraw(MGEFF_ANIMATION anim, cpSpace *space, void *_self){
    Controller *_this = (Controller *)_self;
    _this->onDraw();
}

void Controller::s_finishCB(MGEFF_ANIMATION handle){
    Controller *_this = (Controller *) mGEffAnimationGetContext(handle);
    _this->finishCB();
}


int Controller::onCalc(){
    return 0;
}

void Controller::onDraw(){
}

void Controller::finishCB(){
    assert(0);
}
