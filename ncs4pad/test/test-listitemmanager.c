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

// START_OF_INCS
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>
// END_OF_INCS
#include "mlistitemmanager.h"
#include "register.h"


HWND createMainWin(void)
{
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_TOPMOST ;
    CreateInfo.spCaption = "Hello, world!";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DefaultMainWinProc;
    CreateInfo.lx = 50;
    CreateInfo.ty = 50;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);

    return hMainWnd;
}

int MiniGUIMain(int argc, const char *argv[]) 
{
    HWND hMainWnd;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "exSeparator" , 0 , 0);
#endif

    hMainWnd = createMainWin();
    ncsInitialize();

#if 0
    MGNCS_INIT_CLASS(mPieceItem);
    MGNCS_INIT_CLASS(mAbstractItemManager);
    MGNCS_INIT_CLASS(mItemIterator);
    MGNCS_INIT_CLASS(mListItemManager);
#else
    REGISTER_NCS();
#endif

    mListItemManager* manager = NEW(mListItemManager);
    mPieceItem* item;
    mPieceItem* item1 = NEW(mPieceItem);
    mPieceItem* item2 = NEW(mPieceItem);
    mPieceItem* item3 = NEW(mPieceItem);
    mPieceItem* item4 = NEW(mPieceItem);

    _c(manager)->addItem(manager, item1);

    fprintf(stderr, "test only one item:");
    fprintf(stderr, "item1 is %p.\n", item1);

    mItemIterator* iter = _c(manager)->createItemIterator(manager);
    mItemIterator* prev_iter = _c(iter)->duplicate(iter);
    fprintf(stderr, "start iter->next():\n");
    while((item = _c(iter)->next(iter))) {
        fprintf(stderr, "next:item is %p.\n", item);
    }

    fprintf(stderr, "duplicate new iter = %p\n", prev_iter);
    fprintf(stderr, "start iter->prev():\n");
    while((item = _c(prev_iter)->prev(prev_iter))) {
        fprintf(stderr, "prev:item is %p.\n", item);
    }

    mItemIterator* iter_2 = _c(manager)->createItemIterator(manager);
    fprintf(stderr, "***********\ntest many items:\n");
    fprintf(stderr, "item1 is %p.\n", item1);
    fprintf(stderr, "item2 is %p.\n", item2);
    fprintf(stderr, "item2 is %p.\n", item3);
    fprintf(stderr, "item3 is %p.\n", item4);
    _c(manager)->addItem(manager, item2);
    _c(manager)->addItem(manager, item3);
    _c(manager)->addItem(manager, item4);

    mItemIterator* prev_iter_2 = _c(iter_2)->duplicate(iter_2);
    while((item = _c(iter_2)->next(iter_2))) {
        fprintf(stderr, "next:item is %p.\n", item);
    }

    fprintf(stderr, "start iter->prev():\n");
    while((item = _c(prev_iter_2)->prev(prev_iter_2))) {
        fprintf(stderr, "prev:item is %p.\n", item);
        DELETE(item);
    }

    DELETE(manager);
    DELETE(iter);
    DELETE(prev_iter);
    DELETE(iter_2);
    DELETE(prev_iter_2);

    ncsUninitialize();
    DestroyMainWindow(hMainWnd);

    MainWindowThreadCleanup(hMainWnd);

    return 0;
}
