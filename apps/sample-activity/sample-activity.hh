#include "Activity.hh"

class SampleActivity : public Activity {
public:
    SampleActivity();
    ~SampleActivity();
    
    HWND createHWND();
protected:
    static int proc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam);
};
