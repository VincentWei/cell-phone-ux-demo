#include "Activity.hh"

class SampleActivity : public Activity {
public:
    SampleActivity();
    ~SampleActivity();
    
    HWND createHWND();
protected:
    static LRESULT proc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);
};
