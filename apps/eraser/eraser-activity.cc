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
#include "eraser-activity.hh"

REGISTER_ACTIVITY(EraserActivity);

static void mymain_onPaint (mWidget *_this, HDC hdc, const CLIPRGN* inv)
{
    EraserActivity *activity = (EraserActivity *)Activity::getActivityFromHWND(_this->hwnd);
    activity->onPaint(hdc, inv);
}

static BOOL mymain_onClose (mWidget* _this, int message)
{
    DestroyMainWindow (_this->hwnd);
    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_PAINT, reinterpret_cast<void*>(mymain_onPaint)},
    {MSG_CLOSE, reinterpret_cast<void*>(mymain_onClose)},
    {0, NULL}
};

static NCS_MNWND_TEMPLATE mymain_templ = {
    NCSCTRL_DIALOGBOX, 
    1,
    ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
    WS_NONE,
    WS_EX_NONE,
    "Window Template",
    NULL,
    NULL,
    mymain_handlers,
    NULL,
    0,
    0,
    0, 0,
};

EraserActivity::EraserActivity() :
    NCSActivity( &mymain_templ)
{ 
    m_controller = new Controller(hwnd(), ACTIVITY_W, ACTIVITY_H);
    setup();
}

EraserActivity::~EraserActivity() {
    delete m_controller;
}

void EraserActivity::setup() {
#if 0
    HDC dc;

    dc = m_controller->getDC(Controller::FOREGROUND);
    // TODO: paint on dc

    dc = m_controller->getDC(Controller::BACKGROUND);
    // TODO: paint on dc
#endif

    m_controller->setup();
}

void EraserActivity::onPaint(HDC hdc, const CLIPRGN* inv){
    m_controller->paint(hdc);
}
