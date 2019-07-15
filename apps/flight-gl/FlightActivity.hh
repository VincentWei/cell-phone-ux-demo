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
#include "GLActivity.hh"

class FlightActivity : public GLActivity {
public:
    FlightActivity();
    ~FlightActivity();
    
    HWND createHWND();

protected:
    static int proc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam);
};
