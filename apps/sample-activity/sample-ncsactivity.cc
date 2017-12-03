#include "sample-ncsactivity.hh"

REGISTER_ACTIVITY(SampleNCSActivity);

static void mymain_onPaint (mWidget *_this, HDC hdc, const CLIPRGN* inv)
{
        RECT rt;
        GetClientRect (_this->hwnd, &rt);
        DrawText (hdc, "Hello, NCSActivity!", -1, &rt, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
}

static BOOL mymain_onClose (mWidget* _this, int message)
{
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


SampleNCSActivity::SampleNCSActivity() :
    NCSActivity( &mymain_templ)
{ 
    m_style = STYLE_PUSH;
}

SampleNCSActivity::~SampleNCSActivity() {
}

