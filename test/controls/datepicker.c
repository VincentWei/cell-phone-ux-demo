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
/*********************************************************************
 * Copyright (C) 2002 ~ 2010, Feynman Software Technology Co., Ltd.
 * Room 508B-C, Floor 5, Citic Guoan Shumagang, No.32, Haidian South
 * Road, Haidian District, Beijing, P. R. CHINA 100080.
 * All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * Feynman Software Technology Co. Ltd. ("Confidential Information").
 * You shall not disclose such Confidential Information and shall use
 * it only in accordance you entered into with Feynman Software.
 *          http://www.minigui.com
 *
 *    FileName : datepicker.c
 *      Author : <wangxin@minigui.org>
 * Create Date : Thursday September 09, 2010
 *     Version : 0.0.0.1
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include <mgncs4pad/mpad.h>

#define NDEBUG	1
#include <mgncs4pad/mpaddebug.h>


#define ID_DPICKER1    101
#define ID_DPICKER2    102
#define ID_DPICKER3    103
#define ID_DPICKER4    104


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	mDatePicker* pick = NULL;
/*
    if (NULL != (pick = (mDatePicker*)ncsGetChildObj(self->hwnd, ID_DPICKER1))) {
	    _M(pick, setDate, 2010, 3, 18);
	} else {
	    LOGE("---- Get mDatePicker Error. \n");
	}
*/
    if (NULL != (pick = (mDatePicker*)ncsGetChildObj(self->hwnd, ID_DPICKER2))) {
	    _M(pick, setProperty, NCSP_DPCK_YEAR,  2018);
	    _M(pick, setProperty, NCSP_DPCK_MONTH, 3);
	    _M(pick, setProperty, NCSP_DPCK_DAY,   28);
	} else {
	    LOGE("---- Get mDatePicker Error. \n");
	}

    if (NULL != (pick = (mDatePicker*)ncsGetChildObj(self->hwnd, ID_DPICKER3))) {
	    _M(pick, setDate, 2010, 3, 18);
	} else {
	    LOGE("---- Get mDatePicker Error. \n");
	}

    if (NULL != (pick = (mDatePicker*)ncsGetChildObj(self->hwnd, ID_DPICKER4))) {
	    _M(pick, setDate, 2010, 3, 18);
	} else {
	    LOGE("---- Get mDatePicker Error. \n");
	}

	return TRUE;
}


static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

static void datechanged_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    int y, m, d;
    _M((mDatePicker *)self, getDate, &y, &m, &d);
	LOGE("NCSN_DPCK_DATECHANGED: %d, %d, %d\n", y, m, d);
}

static NCS_EVENT_HANDLER multi_handlers [] = {
    NCS_MAP_NOTIFY(NCSN_DPCK_DATECHANGED, datechanged_notify),
	{0, NULL}
};

static NCS_PROP_ENTRY _props [] = {
	{NCSP_CMBPCK_HMARGIN, 10},
	{NCSP_CMBPCK_VMARGIN, 10},
	{0, 0}
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_DATEPICKER,
		ID_DPICKER1,
		5, 10, 280, 200,
		WS_VISIBLE | NCSS_DPCK_YYYYMMDD | NCSS_NOTIFY,
		WS_EX_NONE,
		"datepicker",
		_props,
		NULL, //rdr_info
		multi_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_DATEPICKER,
		ID_DPICKER2,
		320, 10, 280, 200,
		WS_VISIBLE | NCSS_DPCK_MMDDYYYY | NCSS_NOTIFY,
		WS_EX_NONE,
		"datepicker",
		_props,
		NULL, //rdr_info
		multi_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_DATEPICKER,
		ID_DPICKER3,
		5, 260, 280, 200,
		WS_VISIBLE | NCSS_DPCK_YYMMDD | NCSS_NOTIFY,
		WS_EX_NONE,
		"datepicker",
		_props,
		NULL, //rdr_info
		multi_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_DATEPICKER,
		ID_DPICKER4,
		320, 260, 280, 200,
		WS_VISIBLE | NCSS_DPCK_MMDDYY | NCSS_NOTIFY,
		WS_EX_NONE,
		"datepicker",
		_props,
		NULL, //rdr_info
		multi_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate},
	{MSG_CLOSE, mymain_onClose},
	{0, NULL}
};


//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_MAINWND,
	1,
	0, 0, 625, 500,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"mDatePicker Test ...",
	NULL,
	NULL,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ) / sizeof(NCS_WND_TEMPLATE),
	0,
	0,
	0,
};


int MiniGUIMain(int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
	JoinLayer (NAME_DEF_LAYER, NULL, 0, 0);
#endif

	ncsInitialize();
	ncs4PadInitialize();

	mDialogBox* mydlg = (mDialogBox*)ncsCreateMainWindowIndirect
									(&mymain_templ, HWND_DESKTOP);
	_c(mydlg)->doModal(mydlg, TRUE);

	ncs4PadUninitialize();
	ncsUninitialize();

	MainWindowThreadCleanup(mydlg->hwnd);
	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

