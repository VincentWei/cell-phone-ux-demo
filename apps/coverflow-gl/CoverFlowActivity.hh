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
#include <mgeff/mgeff.h>

enum {
    COVERFLOW_MOUSEDIR_LEFT,
    COVERFLOW_MOUSEDIR_RIGHT,
};

typedef struct _AlbumList
{
    unsigned size;
    char** authorName;
    char** albumName;
} AlbumList;

class CoverFlowActivity : public GLActivity {
public:
    CoverFlowActivity();
    ~CoverFlowActivity();
    
    HWND createHWND();

protected:
    static void loadBitmapToGL(const char *path, int albumSize);

    RECT *hilightRect(void) { return &m_hilightRect; }

    int stepCan(void) { return m_stepCan; }
    void pourStepCan(int increment) { m_stepCan += increment; }
    void clearStepCan(void) { m_stepCan = 0; }

    int mouseX(void) { return m_mouse.x; }
    int mouseY(void) { return m_mouse.y; }
    void setMouse(int x, int y) { m_mouse.x = x, m_mouse.y = y; }

    BOOL mouseMoved(void) { return m_mouseMoved; }
    void setMouseMoved(BOOL isMoved) { m_mouseMoved = isMoved; }

    BOOL setMouseDir(int mouseMove);

    BOOL lButtonDown(void) { return m_lButtonDown; }
    void setLButtonDown(BOOL isDown) { m_lButtonDown = isDown; }

    BOOL stepping(void) { return m_stepping; }
    void setStepping(BOOL isStepping) { m_stepping = isStepping; }

    BOOL scrolling(void) { return m_scrolling; }
    void setScrolling(BOOL isScrolling) { m_scrolling = isScrolling; }

    MGEFF_ANIMATION animation(void) { return m_animation; }
    void createAnimation(void *target, float s, float e, unsigned duration, EffMotionType curve, bool depthTest);
    void deleteAnimation(void) { m_animation = NULL; }

    void backingAnimation(void *taget);

    static void s_stepping_finished_cb(MGEFF_ANIMATION anim);
    void steppingAnimation(void *target, int direction);

    static void s_scrolling_finished_cb(MGEFF_ANIMATION anim);
    void scrollingAnimation(void *target);

    struct timeval *tv(void) { return &m_tv; }

    char* songName(int index) { return m_songNames[index]; }
    char* albumName(int index) { return m_albumList.albumName[m_albumIndex[index]]; }
    char* authorName(int index) { return m_albumList.authorName[m_albumIndex[index]]; }
    void loadSongData(void);
    void releaseSongData(void);

    static void call_draw(MGEFF_ANIMATION anim, void *target, int id, void *value);
    static int proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
    static int glProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

private:
    RECT                m_hilightRect;
    MGEFF_ANIMATION     m_animation;
    BOOL                m_depthTest;
    int                 m_stepCan;
    POINT               m_mouse;
    BOOL                m_mouseMoved;
    int                 m_mouseDir;
    BOOL                m_lButtonDown;
    BOOL                m_stepping;
    BOOL                m_scrolling;
    struct timeval      m_tv;
    AlbumList           m_albumList;
    char**              m_songNames;
    unsigned*           m_albumIndex;
    Intent*             m_intent;
};
