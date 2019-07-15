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
/*
** $Id: mgb_tswin.c 136 2007-10-29 06:30:47Z zhounuohua $
**
** mgb_tswin.c: The window for touch screen calibration. 
**
** Copyright (C) 2007, Feynman Software.
**
** All rights reserved by Feynman Software.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define ORG_PTS_NUM 15
static POINT src_pts [5];
static POINT dst_pts [5];
static int _authz_mouse = 0;
static int  move_flag = 0;
static int  count = 0;
static POINT org_pts[ORG_PTS_NUM];

static char* note[5] = 
{"left-top", "right-top", "right-bottom", "left-bottom", "center"};
static char welcome_text [512];

static RECT authz_rc, msg_rc;

static DLGTEMPLATE DlgAuthz =
{
    WS_VISIBLE,
    WS_EX_NONE,
    0, 0, 0, 0,
    "Authz",
    0, 0,
    0, NULL,
    0
};

static void set_dst_pts (void)
{
    int dst_off_x = g_rcScr.right/8;
    int dst_off_y = g_rcScr.bottom/8;

    dst_pts [0].x = dst_off_x;
    dst_pts [0].y = dst_off_y;

    dst_pts [1].x = g_rcScr.right - 1 - dst_off_x;
    dst_pts [1].y = dst_off_y;

    dst_pts [2].x = g_rcScr.right - 1 - dst_off_x;
    dst_pts [2].y = g_rcScr.bottom - 1 - dst_off_y;

    dst_pts [3].x = dst_off_x;
    dst_pts [3].y = g_rcScr.bottom - 1 - dst_off_y;

    dst_pts [4].x = g_rcScr.right/2;
    dst_pts [4].y = g_rcScr.bottom/2;

}
typedef void (*DO_MOUSE_CALIBRATE_PROC) (int* x, int* y);
extern DO_MOUSE_CALIBRATE_PROC __mg_do_mouse_calibrate;

static BOOL check_touch_points (void)
{
    int i;

    for (i = 0; i < 5; i++) {
        __mg_do_mouse_calibrate (&src_pts [i].x, &src_pts [i].y);

        if (ABS (src_pts [i].x - dst_pts [i].x) > 10 || 
                ABS (src_pts [i].y - dst_pts [i].y) > 10) {
            __mg_do_mouse_calibrate = NULL;
            return FALSE;
        }
    }

    return TRUE;
}

static void reset_org_pts (void)
{
    int i = 0;
    for (i = 0; i < ORG_PTS_NUM; i ++) {
        org_pts [i].x = 0;
        org_pts [i].y = 0;
    }
}

static BOOL cal_src_pts (int idx)
{
    int i = 0;
    int sumx = 0, sumy = 0;

    if (count <= 0)
        return FALSE;

    if (count > ORG_PTS_NUM)
    	count = ORG_PTS_NUM;

    for (i = 0; i < count; i ++) {
        sumx += org_pts [i].x;
        sumy += org_pts [i].y;
    }

    src_pts[idx].x = sumx/count;
    src_pts[idx].y = sumy/count;

    return TRUE;
}

static int AuthzDlgProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_INITDIALOG:
            SetCapture (hDlg);
            set_dst_pts ();
            break;

        case MSG_COMMAND:
            switch (wParam) {
            case IDOK:
            case IDCANCEL:
                EndDialog (hDlg, wParam);
                break;
            }
            break;

        case MSG_MOUSEMOVE:
        {
            if (_authz_mouse < 5 && move_flag && count < ORG_PTS_NUM) {
                GetOriginalMousePosition (&(org_pts [count].x), 
                        &(org_pts [count].y));
                count ++;
                return 0;
            }
            break;
        }

        case MSG_LBUTTONUP:
        {
            move_flag = 0;

            if (_authz_mouse < 5) {

                /*Get original points failure*/
                if (!cal_src_pts (_authz_mouse)) {
                    _authz_mouse = 0;
                    InvalidateRect (hDlg, NULL, TRUE);
                    return 0;
                }

                count = 0;
                reset_org_pts ();

                if (_authz_mouse == 4) {
                    if (SetMouseCalibrationParameters (src_pts, dst_pts)) {
                        if (check_touch_points ()) {
                            ReleaseCapture ();
                            EndDialog (hDlg, 0);
                            return 0;
                        }
                    }

                    _authz_mouse = 0;
                    InvalidateRect (hDlg, NULL, TRUE);
                    return 0;
                }

                _authz_mouse ++;
                InvalidateRect (hDlg, NULL, TRUE);
            }
            else
                count = 0;

            break;
        }
        case MSG_LBUTTONDOWN:
        {
            move_flag = 1;
            reset_org_pts ();
            if (_authz_mouse < 5) {
                GetOriginalMousePosition (&(org_pts [0].x), &(org_pts [0].y));
                count = 1;
            }
            break;
        }

        case MSG_PAINT:
        {
            int x = 0, y = 0;
            hdc = BeginPaint (hDlg);
            SetBkMode(hdc, BM_TRANSPARENT);

            DrawText (hdc, 
                    "Welcome to touch screen calibration.",
                    -1, &authz_rc, DT_CENTER | DT_WORDBREAK);

            if (_authz_mouse < 5) {
                sprintf (welcome_text, 
                        "Please calibrate: enter %s point on screen.", 
                        note[_authz_mouse]);
                DrawText (hdc, 
                        welcome_text,
                        -1, &msg_rc, DT_CENTER | DT_WORDBREAK);
                x = dst_pts[_authz_mouse].x; 
                y = dst_pts[_authz_mouse].y;

                SetPenWidth (hdc, 2);
                LineEx (hdc, x - 10, y, x + 10, y);
                LineEx (hdc, x, y - 10, x, y + 10);
            }
            EndPaint (hDlg, hdc);
            return 0;
        }
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int tscal_win_entry (void)
{
    DlgAuthz.x = 0;
    DlgAuthz.y = 0;
    DlgAuthz.w = g_rcScr.right;
    DlgAuthz.h = g_rcScr.bottom;

    authz_rc.left = msg_rc.left = 10;
    authz_rc.right = msg_rc.right = g_rcScr.right - 20;

    if (g_rcScr.bottom > 400)
        authz_rc.top = g_rcScr.bottom/4;
    else
        authz_rc.top = g_rcScr.bottom/6;

    authz_rc.bottom = g_rcScr.bottom / 2 - 10;

    msg_rc.top = authz_rc.bottom + 20;
    msg_rc.bottom = g_rcScr.bottom - 20;

    DialogBoxIndirectParam (&DlgAuthz, HWND_DESKTOP, AuthzDlgProc, 0L);

    return 0;
}
