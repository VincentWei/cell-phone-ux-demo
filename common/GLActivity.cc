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

#include "common.hh"

#ifdef GPU_GLES1

#include "GLActivity.hh"

#ifdef __ARCH_X86__
    #define GLACTIVITY_RED_SIZE     8
    #define GLACTIVITY_GREEN_SIZE   8
    #define GLACTIVITY_BLUE_SIZE    8
    #define GLACTIVITY_DEPTH_SIZE   24
#endif
#ifdef __ARCH_S3C6410__
    #define GLACTIVITY_RED_SIZE     5
    #define GLACTIVITY_GREEN_SIZE   6
    #define GLACTIVITY_BLUE_SIZE    5
    #define GLACTIVITY_DEPTH_SIZE   16
#endif

GLActivity::GLActivity() :
        m_display(EGL_NO_DISPLAY), m_context(EGL_NO_CONTEXT), m_surface(EGL_NO_SURFACE)
{}

GLActivity::~GLActivity()
{}

int GLActivity::EGLInit (HWND hWnd)
{
    EGLint config_list[] = { 
        EGL_RED_SIZE,   GLACTIVITY_RED_SIZE,  
        EGL_GREEN_SIZE, GLACTIVITY_GREEN_SIZE,
        EGL_BLUE_SIZE,  GLACTIVITY_BLUE_SIZE,  
        EGL_DEPTH_SIZE, GLACTIVITY_DEPTH_SIZE, 
        EGL_NONE
    };  

    EGLint num_config;
    EGLConfig configs[10], config;
    EGLint context_attrs[] = { 
        EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE
    };  
    int i;
    EGLint value;

    m_display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
    if (eglInitialize(m_display, NULL, NULL)
            != EGL_TRUE || eglGetError() != EGL_SUCCESS) {
        printf("eglInitialize failed\n");
        return 0;
    }   
    if (eglChooseConfig (m_display, config_list, configs, 10, &num_config)
            != EGL_TRUE) {
        printf("eglChooseConfig failed\n");
        return 0;
    }

    for (i = 0; i < 10; ++i) {
        eglGetConfigAttrib (m_display, configs[i], EGL_GREEN_SIZE, &value);
        if (value == GLACTIVITY_GREEN_SIZE) {
            config = configs[i];
            break;
        }
    }

    eglBindAPI (EGL_OPENGL_ES_API);

    m_context = eglCreateContext (m_display, config, NULL, context_attrs);
    if (m_context == EGL_NO_CONTEXT || eglGetError () != EGL_SUCCESS) {
        printf ("eglCreateContext failed\n");
        return 0;
    }

    m_surface = mgEGLCreateWindowSurface (m_display, config, hWnd, NULL);
    if (mgEGLMakeCurrent (m_display, m_surface, m_surface, m_context)
            != EGL_TRUE) {
        printf ("eglMakeCurrent failed\n");
        return 0;
    }

    return 1;
}

void GLActivity::EGLRelease (void)
{
    if (eglMakeCurrent (m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE) {
        printf ("eglMakeCurrent failed\n");
        return;
    }

    if (m_context != EGL_NO_CONTEXT)
        eglDestroyContext (m_display, m_context);
    if (m_surface != EGL_NO_SURFACE)
        mgEGLDestroySurface (m_display, m_surface);

    eglTerminate (m_display);
    return;
}

#endif  /* GPU_GLES1 */
