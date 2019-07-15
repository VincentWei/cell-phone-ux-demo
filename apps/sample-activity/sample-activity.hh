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
#include "Activity.hh"

class SampleActivity : public Activity {
public:
    SampleActivity();
    ~SampleActivity();
    
    HWND createHWND();
protected:
    static LRESULT proc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);
};
