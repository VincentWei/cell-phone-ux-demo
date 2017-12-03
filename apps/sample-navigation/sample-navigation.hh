#include "Activity.hh"

class SampleNavigation : public Activity {
public:
    SampleNavigation();
    ~SampleNavigation();
    
    HWND createHWND();
protected:
    static int proc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam);
};
