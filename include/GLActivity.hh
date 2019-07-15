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
/*!============================================================================
 * @file GLActivity.hh 
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef GLACTIVITY_INC
#define GLACTIVITY_INC

#include "Activity.hh"
#include "ActivityStack.hh"
#include <EGL/egl.h>
#include <mgegl/mgegl.h>

class GLActivity : public Activity
{
    public:
        virtual ~GLActivity();
    protected:
        GLActivity();
        int EGLInit(HWND hWnd);
        void EGLRelease();
        EGLDisplay display(void) { return m_display; }
        mgEGLSurface surface(void) { return m_surface; }
    private:
        EGLDisplay          m_display;
        EGLContext          m_context;
        mgEGLSurface        m_surface;
};

#endif /* GLACTIVITY_INC */
