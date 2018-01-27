#include "Activity.hh"

class SampleNavigation : public Activity {
public:
    SampleNavigation();
    ~SampleNavigation();
    
    HWND createHWND();
protected:
    static LRESULT proc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);
};
