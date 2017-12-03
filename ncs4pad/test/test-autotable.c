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
#include <mgncs4pad/mpad.h>
#include <mgplus/mgplus.h>
#include "register.h"

#define CH_IDC_CTNR     200
#define CH_IDC_TIMER    201

#define WND_W   ACTIVITY_W
#define WND_H   ACTIVITY_H

#define BTN_W 80
#define BTN_H 30
#define SECTION_W   300
#define SECTION_H   30
#define ITEM_W 160
#define ITEM_H 20
#define TABLEVIEW_ITEMNUM 10

#include "msimpletableviewpiece.h"
#include "mspeedmeter.h"

static mHotPiece* create_item_userpiece(const char* text, const char* picture);

static mSimpleTableView* table;


typedef struct _sectionInfo{
    char* title;
    char* index;
    char* item_info;
}SectionInfo;

SectionInfo sectioninfo[] = {
    {"A", "a", "AAAAAAAAA"},
    {"B", "b", "BBBBBBBBB"},
    {"C", "c", "CCCCCCCCC"},
    {"D", "d", "DDDDDDDDD"},
    {"E", "e", "EEEEEEEEE"},
    {"F", "f", "FFFFFFFFF"},
    {"G", "g", "GGGGGGGGG"},
    {"H", "h", "HHHHHHHHH"},
    {"I", "i", "IIIIIIIII"},
    {"J", "j", "JJJJJJJJJ"},
    {"K", "k", "KKKKKKKKK"},
    {"L", "l", "LLLLLLLLL"},
    {"M", "m", "MMMMMMMMM"},
    {"N", "n", "NNNNNNNNN"},
    {"O", "o", "OOOOOOOOO"},
    {"P", "p", "PPPPPPPPP"},
    {"Q", "q", "QQQQQQQQQ"},
    {"R", "r", "RRRRRRRRR"},
    {"S", "s", "SSSSSSSSS"},
    {"T", "t", "TTTTTTTTT"},
    {"U", "u", "UUUUUUUUU"},
    {"V", "v", "VVVVVVVVV"},
    {"W", "w", "WWWWWWWWW"},
    {"X", "x", "XXXXXXXXX"},
    {"Y", "y", "YYYYYYYYY"},
    {"Z", "z", "ZZZZZZZZZ"},
};

/* start of mSimpleTableViewPiece.*/

static void mSimpleTableView_construct(mSimpleTableView *self, DWORD add_data)
{
	Class(mTableViewPiece).construct((mTableViewPiece*)self, add_data);

    self->data = sectioninfo;
}

static void mSimpleTableView_destroy(mSimpleTableView *self)
{
    Class(mTableViewPiece).destroy((mTableViewPiece*)self);
}

static mTableViewItemPiece* mSimpleTableView_createItemForRow(mSimpleTableView* self, const mIndexPath* indexpath)
{
    RECT rect;
    RECT view_rc;
    mHotPiece* piece = create_item_userpiece(sectioninfo[indexpath->section].item_info, NULL);
    mTableViewItemPiece* item = NEWPIECEEX(mTableViewItemPiece, 0);

    _c(self)->getRect(self, &view_rc);
    _c(piece)->getRect(piece, &rect);
    _c(item)->setUserPiece(item, piece);
    rect.right = RECTW(view_rc);
    _c(item)->setRect(item, &rect);
    return item;
}

static int mSimpleTableView_numberOfSections(mSimpleTableView* self)
{
    return TABLESIZE(sectioninfo);
}

static int mSimpleTableView_numberOfRowsInSection(mSimpleTableView* self, int section)
{
    return 24;
}


static const char* mSimpleTableView_titleForSection(mSimpleTableView* self, int section)
{
    return sectioninfo[section].title;
}

static const char* mSimpleTableView_indexForSection(mSimpleTableView* self, int section)
{
    return sectioninfo[section].index;
}

static void mSimpleTableView_onCommitDeleteRowAtIndexPath(mTableViewPiece* self, const mIndexPath* indexpath)
{
    _c(self)->deleteRowAtIndexPath(self, indexpath, NCS_TABLEVIEW_ANIMATIONZOOM);
}


BEGIN_MINI_CLASS(mSimpleTableView, mTableViewPiece)
	CLASS_METHOD_MAP(mSimpleTableView, construct)
	CLASS_METHOD_MAP(mSimpleTableView, createItemForRow)
	CLASS_METHOD_MAP(mSimpleTableView, numberOfSections)
	CLASS_METHOD_MAP(mSimpleTableView, numberOfRowsInSection)
	CLASS_METHOD_MAP(mSimpleTableView, titleForSection)
	CLASS_METHOD_MAP(mSimpleTableView, indexForSection)
	CLASS_METHOD_MAP(mSimpleTableView, onCommitDeleteRowAtIndexPath)
	CLASS_METHOD_MAP(mSimpleTableView, destroy)
END_MINI_CLASS
/* end of mSimpleTableViewPiece.*/


static mHotPiece* create_item_userpiece(const char* text, const char* picture)
{
    RECT rc = {0, 0, ITEM_W, ITEM_H};
    /* label piece */
    mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
    _c(txt_piece)->setRect(txt_piece, &rc);
    _c(txt_piece)->setProperty(txt_piece,
            NCSP_LABELPIECE_LABEL, (DWORD)text);
    mLabelPiece_setWordBreak((mLabelPiece*)txt_piece, TRUE);

    return (mHotPiece*)txt_piece;
}

static BOOL timerhandle_view(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    mScrollViewPiece *view;
    int offset_y = 20;
    RECT rc;
    static int isDown = 1;

    view = (mScrollViewPiece*)listener;
    _c(view)->getViewport(view, &rc);

    if(isDown == 0){
        if(rc.top > 0)
            offset_y = -10;
        else{
            isDown = 1;
            offset_y = 10;
        }
    }
    else if(isDown){
        if(rc.bottom < TABLESIZE(sectioninfo) * 25 * ITEM_H)
            offset_y = 10;
        else{
            isDown = 0;
            offset_y = -10;
        }
    }

    _c(view)->moveViewport(view, rc.left, rc.top + offset_y);

    return TRUE;
}


static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData)
{
    mTimer *timer;
    RECT rc = {0, 0, WND_W, WND_H};
    mContainerCtrl* ctnr;
    mPanelPiece* panel;

    ctnr = (mContainerCtrl*)ncsGetChildObj(self->hwnd, CH_IDC_CTNR);

    panel = NEWPIECE(mPanelPiece);
    _c(panel)->setRect(panel, &rc);
    _c(ctnr)->setBody(ctnr, (mHotPiece*)panel);

    table = (mSimpleTableView*)NEWPIECEEX(mSimpleTableView, NCS_TABLEVIEW_INDEXLOCATE_STYLE);
    _c(table)->setSeparatorColor(table, 0xFFFF00FF);
    _c(table)->setSeparatorStyle(table, NCS_TABLEVIEW_SEPARATORSTYLE_NONE);
    _c(table)->setRect(table, &rc);
    _c(table)->reloadData(table);
    _c(panel)->addContent(panel, (mHotPiece*)table, 0, 0);

    timer = (mTimer*)ncsGetChildObj(self->hwnd, CH_IDC_TIMER);
    ncsAddEventListener((mObject *)timer, (mObject *)table, (NCS_CB_ONPIECEEVENT)timerhandle_view, MSG_TIMER);
    _c(timer)->start(timer);

    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {MSG_LBUTTONDOWN, SpeedMeterMessageHandler },
    {MSG_LBUTTONUP, SpeedMeterMessageHandler },
    {MSG_MOUSEMOVE, SpeedMeterMessageHandler },
    {0, NULL}
};

static NCS_PROP_ENTRY timer_props[] = {
    {NCSP_TIMER_INTERVAL, 5},//unit:10ms
    {0, 0},
};

static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        CH_IDC_CTNR,
        0, 0, WND_W, WND_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL, NULL, 0, 0,
    },
    {
        NCSCTRL_TIMER,
        CH_IDC_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timer_props, NULL, NULL, NULL, 0, 0, 
    },
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        0, 0, WND_W, WND_H,
        WS_NONE,
        WS_EX_AUTOSECONDARYDC,
        "scrollview",
        NULL,
        NULL,
        mymain_handlers,
        _ctrl_tmpl,
        TABLESIZE(_ctrl_tmpl),
        0,
        0, 0,
};

int MiniGUIMain(int argc, const char *argv[]) {
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    ShowCursor(FALSE);

    ncsInitialize();
    ncs4PadInitialize();
    REGISTER_NCS();
    MGNCS_INIT_CLASS(mSimpleTableView);
    MGNCS_INIT_CLASS(mIndexLocatePiece);

    mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect
                (&mymain_templ, HWND_DESKTOP);
    mGEffInit();
    _c(mydlg)->doModal(mydlg, TRUE);
    MainWindowThreadCleanup(mydlg->hwnd);

    MGNCS_UNREG_COMPONENT(mContainerCtrl);
    ncs4PadUninitialize();
    ncsUninitialize ();

    return 0;
}
