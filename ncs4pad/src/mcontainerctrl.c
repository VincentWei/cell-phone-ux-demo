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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mcontainerctrl.h"
#include "mlayout_manager.h"
#include "mpanelpiece.h"

static void mContainerCtrl_setBody(mContainerCtrl* self, mHotPiece* body)
{
    self->body = (mObject*)body;

    assert(INSTANCEOF(body, mPanelPiece));

    // set owner for body
    _c((mPanelPiece*)body)->setOwner((mPanelPiece*)body, (mWidget*)self);

    return;
}

#ifdef ENABLE_ANIM_FPS_TEST
#include <sys/times.h>
#include <unistd.h>

#define TCK_PER_SEC sysconf(_SC_CLK_TCK)
#define ANIM_TICKS_MAX    (2 * TCK_PER_SEC)

volatile int anim_fps_test_status = 0; /* 0:ready, 1:running */
static int anim_frames = 0;
static clock_t anim_start_time = 0;

static void anim_fps_test_tellspeed (int ticks)
{
    unsigned int fps = (int) ( (double) anim_frames / (ticks / TCK_PER_SEC) );

    /* print result */
    fprintf (stderr, "**** anim fps: n=%d, time=%d, speed=%ufps\n", anim_frames, (int) ticks, fps);
}

void anim_fps_test_start (int new_status)
{
    if (1 == new_status) {
        printf ("**** anim test start ...\n");
    }
    else {
        printf ("**** anim test end.\n");
    }
        
    if (anim_fps_test_status == 1) { /* print result of previous test */
        int ticks = times (NULL) - anim_start_time;
        anim_fps_test_tellspeed (ticks);
    }

    /* set status */
    anim_fps_test_status = new_status;

    anim_frames = 0;
    anim_start_time = times (NULL);
}
#endif

static void mContainerCtrl_onPaint(mContainerCtrl *self, HDC hdc, const PCLIPRGN pclip) {
    if (self->body) {
        _c((mHotPiece*)(self->body))->paint(
                (mHotPiece*)self->body, hdc, (mObject*)self, (DWORD)NULL);

#ifdef ENABLE_ANIM_FPS_TEST

        if (anim_fps_test_status == 1) { /* running */

            /* printf ("+++++++++ %d\n", random ()); */

            if ( (times (NULL) - anim_start_time) >= ANIM_TICKS_MAX ) { /* end test */
                printf ("**** anim test timeout.\n");
                anim_fps_test_start (0);
            }
            else {
                ++ anim_frames;
            }
        }
#endif
    }
}

static BOOL mContainerCtrl_onEraseBkgnd(mContainerCtrl *self, HDC hdc, const RECT *pinv)
{
    return TRUE;
}

static int mContainerCtrl_wndProc(mWidget* self,  int message, WPARAM wParam, LPARAM lParam)
{
    if (message == MSG_PAINT) {
        mPanelPiece* topPanel = (mPanelPiece*)self->body;
        if ( NULL != topPanel) {
            RECT* invalidRect = &topPanel->invalidRect;
            GetUpdateRect(self->hwnd, invalidRect);
            /*
            fprintf(stderr, "piece is %p, invalidRect is (%d,%d,%d,%d).\n", topPanel, invalidRect->left, 
                    invalidRect->top, invalidRect->right, invalidRect->bottom);
                    */
        }
    }

    return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}


BEGIN_CMPT_CLASS(mContainerCtrl, mWidget)
    CLASS_METHOD_MAP(mContainerCtrl, setBody)
    CLASS_METHOD_MAP(mContainerCtrl, wndProc)
    CLASS_METHOD_MAP(mContainerCtrl, onPaint)
    CLASS_METHOD_MAP(mContainerCtrl, onEraseBkgnd)
END_CMPT_CLASS
