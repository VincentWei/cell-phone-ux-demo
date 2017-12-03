#ifndef ERASER_ACTIVITY_HH
#define ERASER_ACTIVITY_HH

#include "NCSActivity.hh"

#include "eraser.hh"
using eraser::Controller;

class EraserActivity : public NCSActivity {
public:
    EraserActivity();
    ~EraserActivity();
    void setup();
    void onPaint(HDC hdc, const CLIPRGN* inv);
private:
    Controller *m_controller;
};

#endif
