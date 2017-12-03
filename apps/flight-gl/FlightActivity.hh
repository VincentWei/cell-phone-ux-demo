#include "GLActivity.hh"

class FlightActivity : public GLActivity {
public:
    FlightActivity();
    ~FlightActivity();
    
    HWND createHWND();

protected:
    static int proc(HWND hWnd, int message, WPARAM wParam,LPARAM lParam);
};
