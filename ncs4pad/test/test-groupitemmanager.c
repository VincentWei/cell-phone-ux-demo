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
#include "mgroupitemmanager.h"
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
    JoinLayer(NAME_DEF_LAYER , "test-groupitemmanager" , 0 , 0);
#endif

    hMainWnd = createMainWin();
    ncsInitialize();
    REGISTER_NCS();

    mGroupItemManager* manager = NEW(mGroupItemManager);
    mGroupPieceItem* item;
    mGroupPieceItem* item1 = NEWEX(mGroupPieceItem, (DWORD)"abc");
    mGroupPieceItem* item1_1 = NEWEX(mGroupPieceItem, (DWORD)"abcd");
    mGroupPieceItem* item1_2 = NEWEX(mGroupPieceItem, (DWORD)"abcde");
    mGroupPieceItem* item1_3 = NEWEX(mGroupPieceItem, (DWORD)"abcdef");
    mGroupPieceItem* item1_4 = NEWEX(mGroupPieceItem, (DWORD)"abcdefg");
    mGroupPieceItem* item1_5 = NEWEX(mGroupPieceItem, (DWORD)"abcdefgh");
    mGroupPieceItem* item2 = NEWEX(mGroupPieceItem, (DWORD)"bcd");
    mGroupPieceItem* item2_1 = NEWEX(mGroupPieceItem, (DWORD)"bcde");
    mGroupPieceItem* item2_2 = NEWEX(mGroupPieceItem, (DWORD)"bcdef");
    mGroupPieceItem* item2_3 = NEWEX(mGroupPieceItem, (DWORD)"bcdefg");
    mGroupPieceItem* item3 = NEWEX(mGroupPieceItem, (DWORD)"cde");
    mGroupPieceItem* item4 = NEWEX(mGroupPieceItem, (DWORD)"def");

    mGroupPieceItem* g_item1 = NEWEX(mGroupPieceItem, (DWORD)"a");
    mGroupPieceItem* g_item2 = NEWEX(mGroupPieceItem, (DWORD)"b");
    mGroupPieceItem* g_item3 = NEWEX(mGroupPieceItem, (DWORD)"c");
    mGroupPieceItem* g_item4 = NEWEX(mGroupPieceItem, (DWORD)"d");
    mGroupPieceItem* g_item5 = NEWEX(mGroupPieceItem, (DWORD)"e");
    _c(manager)->addGroupItem(manager, g_item4, TRUE);
    _c(manager)->addGroupItem(manager, g_item1, TRUE);
    _c(manager)->addGroupItem(manager, g_item2, TRUE);
    _c(manager)->addGroupItem(manager, g_item3, TRUE);
    _c(manager)->addGroupItem(manager, g_item5, TRUE);

    _c(manager)->addGroupItem(manager, item1, FALSE);
    _c(manager)->addGroupItem(manager, item1_1, FALSE);
    _c(manager)->addGroupItem(manager, item1_2, FALSE);
    _c(manager)->addGroupItem(manager, item1_3, FALSE);
    _c(manager)->addGroupItem(manager, item1_4, FALSE);
    _c(manager)->addGroupItem(manager, item1_5, FALSE);
    _c(manager)->addGroupItem(manager, item2, FALSE);
    _c(manager)->addGroupItem(manager, item2_3, FALSE);
    _c(manager)->addGroupItem(manager, item2_1, FALSE);
    _c(manager)->addGroupItem(manager, item2_2, FALSE);
    _c(manager)->addGroupItem(manager, item3, FALSE);
    _c(manager)->addGroupItem(manager, item4, FALSE);

    fprintf(stderr, "test only one item:");
    fprintf(stderr, "item1 is %p.\n", item1);

    mItemIterator* iter = _c(manager)->createGroupItemIterator(manager, NULL);
    //mItemIterator* prev_iter = _c(iter)->duplicate(iter);
#if 0
    fprintf(stderr, "start iter->next():\n");
    while((item = (mGroupPieceItem*)_c(iter)->next(iter))) {
        fprintf(stderr, "next:item is %p, key is %s\n", item, _c(item)->getKey(item));
    }
#else
    fprintf(stderr, "start iter->prev():\n");
    while((item = (mGroupPieceItem*)_c(iter)->prev(iter))) {
        fprintf(stderr, "prev:item is %p, key is %s\n", item, _c(item)->getKey(item));
    }
#endif

#if 0
    fprintf(stderr, "duplicate new iter = %p\n", prev_iter);
    fprintf(stderr, "start iter->prev():\n");
    while((item = _c(prev_iter)->prev(prev_iter))) {
        fprintf(stderr, "prev:item is %p.\n", item);
    }
#endif

#if 0
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
#endif

    ncsUninitialize();
    DestroyMainWindow(hMainWnd);

    MainWindowThreadCleanup(hMainWnd);

    return 0;
}
