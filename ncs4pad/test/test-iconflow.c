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
/**
 * $Id: iconview.c 601 2009-10-13 01:03:31Z xwyan $
 *
 * Listing P2C14.3
 *
 * iconview.c: Sample program for mGNCS Programming Guide
 *      The demo application for IconView.
 *
 * Copyright (C) 2009 Feynman Software.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// START_OF_INCS
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>
#include "miconflow.h"
#include "mtransroundpiece.h"
// END_OF_INCS

#define IDC_ICONFLOW    100
#define IDC_ADD         600
#define IDC_DELETE      601

static BITMAP icon_demos [8];

static const char* iconfiles[8] =
{
    "./res/1.jpg",
    "./res/2.jpg",
    "./res/3.jpg",
    "./res/4.jpg",
    "./res/5.jpg",
    "./res/6.jpg",
    "./res/7.jpg",
    "./res/8.jpg",
};

#if 0
static BITMAP bkicon_demo;
static const char* bkiconfile = "./res/icon_bk.bmp";
#endif
static const char *iconlabels[8] =
{
    "music",
    "ebook",
    "game",
    "picture",
    "video",
    "calendar",
    "setting",
    "watch",
};

static BOOL iconv_init(mDialogBox* self)
{
    NCS_ICONFLOW_ITEMINFO info;
    static int i = 0, j = 0, pos = 0;
    mIconFlow *iconvObj;
    HWND iconvWnd;
    RECT rc;
    mShapeTransRoundPiece *bk = NEWPIECE(mShapeTransRoundPiece);
    ARGB COLORS_ICONFLOW_BK[] = {0xff646873, 0xff2e2f32};

    for(i = 0; i < TABLESIZE(icon_demos); i++)
    {
        LoadBitmap (HDC_SCREEN, &icon_demos[i], iconfiles[i]);
    }

    iconvWnd = GetDlgItem (self->hwnd, IDC_ICONFLOW);
    iconvObj = (mIconFlow*)ncsObjFromHandle(iconvWnd);

    if (!iconvObj)
        return FALSE;

// START_OF_ADDITEMS

    for(i = 0; i < TABLESIZE(icon_demos); i++)
    {
        pos = 0; 
        memset (&info, 0, sizeof(NCS_ICONFLOW_ITEMINFO)); 
        info.bmp = &icon_demos[i]; 
        info.index = TABLESIZE(icon_demos) * j + i; 
        info.label = iconlabels[i]; 
        info.addData = (DWORD)iconlabels[i]; 
        _c(iconvObj)->addItem(iconvObj, &info, &pos); 
    } 
    _c(iconvObj)->setCurSel(iconvObj, 0); 
// END_OF_ADDITEMS

    _c(iconvObj)->setIconSize(iconvObj,200, 120);

    _c(iconvObj)->setProperty (iconvObj, NCSP_ICONFLOW_SPAN, (DWORD)(120));

#if 0
    LoadBitmap (HDC_SCREEN, &bkicon_demo, bkiconfile);
    _c(iconvObj)->setProperty (iconvObj, NCSP_ICONFLOW_ICONFRAME, (DWORD)&bkicon_demo);
#else
    _c(iconvObj)->setProperty (iconvObj, NCSP_ICONFLOW_ICONBORDER, 2);
#endif

    GetWindowRect(iconvWnd, &rc);
    _c(bk)->setRect(bk, &rc);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, 0);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_FILLMODE, (DWORD)NCSP_TRANROUND_GRADIENT_FILL);
    _c(bk)->setGradientFillColors(bk, COLORS_ICONFLOW_BK, TABLESIZE(COLORS_ICONFLOW_BK));

    _c(iconvObj)->setProperty (iconvObj, NCSP_ICONFLOW_BKGNDPIECE, (DWORD)bk);

    return TRUE;
}

// START_OF_WNDHANDLERS
static BOOL mainwnd_onKeyDown(mWidget* self, 
        int message, int code, DWORD key_status)
{
    if (message == MSG_KEYDOWN) {
        if (code == SCANCODE_REMOVE) {
            mIconFlow *iconView;
            int curSel, count;
            HITEM delItem;

            iconView = 
                (mIconFlow*)ncsObjFromHandle(GetDlgItem (self->hwnd, IDC_ICONFLOW));
            count = _c(iconView)->getItemCount(iconView);

            if (iconView) {
                curSel = _c(iconView)->getCurSel(iconView);

                if (curSel >= 0) {
                    delItem = _c(iconView)->getItem(iconView, curSel);
                    _c(iconView)->removeItem(iconView, delItem);
                    if (curSel == count -1)
                        curSel--;
                    _c(iconView)->setCurSel(iconView, curSel);
                }
            }
        }
    }
    return FALSE;
}

static NCS_EVENT_HANDLER mainwnd_handlers[] = {
	{MSG_KEYDOWN, mainwnd_onKeyDown},
	{0, NULL }
};
// END_OF_WNDHANDLERS

// START_OF_ICONVHANDLERS
static void iconv_notify (mWidget *self, int id, int nc, DWORD add_data)
{
    if (nc == NCSN_ICONFLOW_CLICKED)
    {
        if (self) {
            int idx;
            const char  *text;
            mIconFlow   *iconvObj = (mIconFlow*)self;

            idx = _c(iconvObj)->indexOf(iconvObj, (HITEM)add_data);
            text = _c(iconvObj)->getText(iconvObj, (HITEM)add_data);
            fprintf (stderr, "click icon[%d], text is %s \n", idx, text);
        }
    }
}

static NCS_EVENT_HANDLER iconv_handlers[] = {
    NCS_MAP_NOTIFY(NCSN_ICONFLOW_CLICKED, iconv_notify),
    NCS_MAP_NOTIFY(NCSN_ICONFLOW_SELCHANGED, iconv_notify),
	{0, NULL }
};
// END_OF_ICONVHANDLERS

static NCS_RDR_INFO iconv_rdr_info = {
	"classic", "classic", NULL
};

static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
	{
		NCSCTRL_ICONFLOW, 
		IDC_ICONFLOW,
        0, 0, 240, 320,
        WS_BORDER | WS_CHILD | WS_VISIBLE | NCSS_NOTIFY | NCSS_ICONFLOW_LOOP | NCSS_ICONFLOW_VERTICAL,
		WS_EX_NONE,
		"",
		NULL,
		&iconv_rdr_info,
		iconv_handlers,
		NULL,
		0,
		0
	}
		};

static NCS_MNWND_TEMPLATE mainwnd_tmpl = {
	NCSCTRL_DIALOGBOX, 
	1,
	0, 0, 240, 320,
    WS_VISIBLE,
	WS_EX_NONE,
    "IconView Demo",
	NULL,
	NULL,
	mainwnd_handlers,
	_ctrl_tmpl,
	sizeof(_ctrl_tmpl)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	ncsInitialize();
    MGNCS_REGISTER_COMPONENT_EX(mIconFlow, WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect 
                                (&mainwnd_tmpl, HWND_DESKTOP);

    iconv_init(mydlg);

    mGEffInit ();
	_c(mydlg)->doModal(mydlg, TRUE);

	MainWindowThreadCleanup(mydlg->hwnd);

    mGEffDeinit ();

    MGNCS_UNREG_COMPONENT(mIconFlow);

	ncsUninitialize();
	return 0;
}
