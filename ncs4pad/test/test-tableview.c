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

#include "register.h"

#define BTN_W 80
#define BTN_H 30
#define ITEM_W 300
#define ITEM_H 50

#include "msimpletableviewpiece.h"
#include "mspeedmeter.h"

static mHotPiece* create_item_userpiece(const char* text, const char* picture);

static mSimpleTableView* table;
static mSimpleTableView* table_index;

typedef struct _itemInfo{
    char* key;
    char* text;
    char* picture;
}ItemInfo;

typedef struct _sectionInfo{
    char* title;
    char* index;
    int num;
    ItemInfo* rows;
}SectionInfo;

ItemInfo listinfo[] = {
    {"houhuihua", " :can you tell?", "./news.png"},
    {"huzhaolin", " :are you man?", "./news.png"},
    {"wanzheng", " :wanzheng?", "./news.png"},
#if 0
    {"dongkai", "dongkai?", "./news.png"},
    {"maoxinghui", "maoxinghui?", "./news.png"},
    {"zouqizhi", "zouqizhi?", "./news.png"},
    {"yuanzuochao", "yuanzuochao?", "./news.png"},
    {"wangxubing", "wangxubing?", "./news.png"},
    {"dengming", "dengming?", "./news.png"},
    //{"humingming", "humingming?", "./news.png"},
#endif
};

ItemInfo listinfo_e[] = {
    {"houhuihua", "e:can you tell me your name?", "./news.png"},
    {"huzhaolin", "e:are you man?", "./news.png"},
    {"wanzheng", "e:wanzheng?", "./news.png"},
};

SectionInfo sectioninfo[] = {
    {"A", "a", TABLESIZE(listinfo), listinfo},
    {"B", "b", 0, listinfo},
    {"C", "c", TABLESIZE(listinfo), listinfo},
    {"D", "d", TABLESIZE(listinfo), listinfo},
    {"E", "e", TABLESIZE(listinfo_e), listinfo_e},
    {"F", "f", TABLESIZE(listinfo), listinfo},
    {"G", "g", TABLESIZE(listinfo), listinfo},
    {"H", "h", TABLESIZE(listinfo), listinfo},
    {"I", "i", TABLESIZE(listinfo), listinfo},
    {"J", "j", TABLESIZE(listinfo), listinfo},
    {"K", "k", TABLESIZE(listinfo), listinfo},
    {"L", "l", TABLESIZE(listinfo), listinfo},
    {"M", "m", 0, listinfo},
    {"N", "n", 0, listinfo},
    {"O", "o", TABLESIZE(listinfo), listinfo},
    {"P", "p", TABLESIZE(listinfo), listinfo},
    {"Q", "q", TABLESIZE(listinfo), listinfo},
    {"R", "r", TABLESIZE(listinfo), listinfo},
    {"S", "s", TABLESIZE(listinfo), listinfo},
    {"T", "t", TABLESIZE(listinfo), listinfo},
    {"U", "u", TABLESIZE(listinfo), listinfo},
    {"V", "v", TABLESIZE(listinfo), listinfo},
    {"W", "w", TABLESIZE(listinfo), listinfo},
    {"X", "x", TABLESIZE(listinfo), listinfo},
    {"Y", "y", TABLESIZE(listinfo), listinfo},
    {"Z", "z", TABLESIZE(listinfo), listinfo},
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
    ItemInfo* rowinfo = sectioninfo[indexpath->section].rows + indexpath->row;
    mHotPiece* piece = create_item_userpiece(rowinfo->text, rowinfo->picture);
    mTableViewItemPiece* item = NEWPIECEEX(mTableViewItemPiece, 0);

    _c(self)->getRect(self, &view_rc);
    _c(piece)->getRect(piece, &rect);
    _c(item)->setUserPiece(item, piece);
    rect.right = RECTW(view_rc);
    _c(item)->setRect(item, &rect);

    //fprintf(stderr, "%s:rect (%d,%d,%d,%d).\n", __FUNCTION__, rect.left, 
    //        rect.top, rect.right, rect.bottom);
    return item;
}

static int mSimpleTableView_numberOfSections(mSimpleTableView* self)
{
    return TABLESIZE(sectioninfo);
}

static int mSimpleTableView_numberOfRowsInSection(mSimpleTableView* self, int section)
{
    return sectioninfo[section].num;
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
    /* edit datasource. */
    /* ... */

    /* delete animation. */
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

static int onEditStateChange(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message == MSG_BTNPANELPIECE_STATE_CHANGED) {
        enum mButtonPanelPieceState state = (enum mButtonPanelPieceState)wParam;
        if (state == NCSN_ABP_CLICKED) {
	    _c(table)->changeMode(table);
        }
    }
    return 0;
}

static int onDeleteStateChange(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message == MSG_BTNPANELPIECE_STATE_CHANGED) {

        enum mButtonPanelPieceState state = (enum mButtonPanelPieceState)wParam;
        if (state == NCS_BPP_PUSHED) {

        } else if (state == NCS_BPP_NORMAL){
            //_c(table)->removeItem(table, "key", TRUE);
        }
    }
    return 0;
}

#if 0
static int onPieceClick(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    return 0;
}
#endif

static BOOL create_edit_button(mMainWnd* self)
{
    mContainerCtrl *ctnr;
    {
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, 0, BTN_W, BTN_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, BTN_W, BTN_H};
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(body)->setRect(body, &rc);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
        SetRect(&rc, 0, 0, 50, 20);
        _c(txt_piece)->setRect(txt_piece, &rc);

        mButtonPanelPiece *btn = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);

        _c(btn)->setStateTable(btn,
                txt_piece, NCSP_LABELPIECE_LABEL, (DWORD)"group", (DWORD)"pushed");

        _c(btn)->addContentToLayout(btn, txt_piece);

        SetRect(&rc, 0, 0, BTN_W, BTN_H);
        _c(btn)->setRect(btn, &rc);

        ARGB colors0[] = {0xfffdc972, 0xfffcb848, 0xffba3305, 0xffd25e39};
        float positions0[] = {0.0, 0.03, 0.98, 1.0};
        ARGB colors1[] = {0x3fff0000, 0x7f00ff00, 0xaf003fff, 0xff0000ff};
        _c(btn)->setGradientBackgroundColor(btn,
                colors0, positions0, 4,
                colors1, NULL, 4);

        _c(btn)->setStates(btn);

        _c(btn)->appendEventHandler(btn, MSG_BTNPANELPIECE_STATE_CHANGED, onEditStateChange);

        _c(body)->addContent(body, (mHotPiece*)btn, 0, 0);
    }
    return TRUE;
}

static BOOL create_delete_button(mMainWnd* self)
{
    mContainerCtrl *ctnr;
    {
        ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                BTN_W*2, 0, BTN_W, BTN_H,
                self->hwnd,
                NULL, NULL, NULL, 0);

        RECT rc = {0, 0, BTN_W, BTN_H};
        mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
        _c(body)->setRect(body, &rc);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

        mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
        SetRect(&rc, 0, 0, 50, 20);
        _c(txt_piece)->setRect(txt_piece, &rc);

        mButtonPanelPiece *btn = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);

        _c(btn)->setStateTable(btn,
                txt_piece, NCSP_LABELPIECE_LABEL, (DWORD)"index", (DWORD)"pushed");

        _c(btn)->addContentToLayout(btn, txt_piece);

        SetRect(&rc, 0, 0, BTN_W, BTN_H);
        _c(btn)->setRect(btn, &rc);

        ARGB colors0[] = {0xfffdc972, 0xfffcb848, 0xffba3305, 0xffd25e39};
        float positions0[] = {0.0, 0.03, 0.98, 1.0};
        ARGB colors1[] = {0x3fff0000, 0x7f00ff00, 0xaf003fff, 0xff0000ff};
        _c(btn)->setGradientBackgroundColor(btn,
                colors0, positions0, 4,
                colors1, NULL, 4);

        _c(btn)->setStates(btn);

        _c(btn)->appendEventHandler(btn, MSG_BTNPANELPIECE_STATE_CHANGED, onDeleteStateChange);

        _c(body)->addContent(body, (mHotPiece*)btn, 0, 0);
    }
    return TRUE;
}

#if 0
static mButtonPanelPiece *createUserPiece(void)
{
    mButtonPanelPiece *content = NEWPIECE(mButtonPanelPiece);
    mShapeTransRoundPiece* bk = _c(content)->getBkgndPiece(content);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, 0);
    _c(content)->setStateTable(content, (mHotPiece*)bk, NCSP_TRANROUND_BKCOLOR, 0xFFFFFFFF, 0xFF0000FF);
    _c(content)->setStates(content);

    //_c(content)->appendEventHandler(content, MSG_BTNPANELPIECE_STATE_CHANGED, onPieceStateChange);
    return content;
}
#endif

static mHotPiece* create_item_userpiece(const char* text, const char* picture)
{
    /* image piece */
    static BITMAP bmp_left;
    mHotPiece *img_piece = (mHotPiece*)NEWPIECE(mImagePiece);

    /* userPiece */
    //mButtonPanelPiece* panel = createUserPiece();
    mPanelPiece* panel = NEWPIECE(mPanelPiece);
    RECT rc = {0, 0, ITEM_W, ITEM_H};
    _c(panel)->setRect(panel, &rc);

    LoadBitmap(HDC_SCREEN, &bmp_left, picture);
    rc.right = bmp_left.bmWidth;
    rc.bottom = bmp_left.bmHeight;
    _c(img_piece)->setRect(img_piece, &rc);
    _c(img_piece)->setProperty(img_piece, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp_left);

    /* label piece */
    mHotPiece *txt_piece = (mHotPiece*)NEWPIECE(mLabelPiece);
    SetRect(&rc, 0, 0, 160, 20);
    _c(txt_piece)->setRect(txt_piece, &rc);
    _c(txt_piece)->setProperty(txt_piece,
            NCSP_LABELPIECE_LABEL, (DWORD)text);
    mLabelPiece_setWordBreak((mLabelPiece*)txt_piece, TRUE);

    _c(panel)->addContent(panel, img_piece, 10, -2);
    _c(panel)->addContent(panel, txt_piece, 80, 20);

    return (mHotPiece*)panel;
}

#if 0
static BOOL _onItemContentPieceClick(mMainWnd* self,
        mHotPiece *sender, int event_id, DWORD param)
{
    assert(event_id == NCSN_TABLEPANEL_CONTENTCLICKED);
    fprintf(stderr, "%s:sender is %p, self is %p.\n", __FUNCTION__, sender, self);
    return TRUE;
}
#endif

#define TABLEVIEW_ITEMNUM 10

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
#if 0
    {
        RECT rc = {0, 0, ITEM_W, ITEM_H * TABLEVIEW_ITEMNUM};
        /* table */
        mContainerCtrl* ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                0, BTN_H + 10, ITEM_W, TABLEVIEW_ITEMNUM * ITEM_H + 2,
                self->hwnd,
                NULL, NULL, NULL, 0);

        table = (mSimpleTableView*)NEWPIECEEX(mSimpleTableView, NCS_TABLEVIEW_GROUP_STYLE);
        _c(ctnr)->setBody(ctnr, (mHotPiece*)table);

        _c(table)->setRect(table, &rc);
        _c(table)->reloadData(table);
    }
#endif

    {
        RECT rc = {0, 0, ITEM_W, ITEM_H * TABLEVIEW_ITEMNUM};
        /* table */
        mContainerCtrl* ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                "ContainerCtrl",
                WS_VISIBLE, 0, 0,
                ITEM_W + 50, BTN_H + 10, ITEM_W, TABLEVIEW_ITEMNUM * ITEM_H + 2,
                self->hwnd,
                NULL, NULL, NULL, 0);

        table_index = table = (mSimpleTableView*)NEWPIECEEX(mSimpleTableView, NCS_TABLEVIEW_INDEXLOCATE_STYLE);
        _c(table)->setSeparatorColor(table, 0xFFFF00FF);
        _c(table)->setSeparatorStyle(table, NCS_TABLEVIEW_SEPARATORSTYLE_NONE);
        _c(table)->setRect(table, &rc);
        _c(table)->reloadData(table);
#if 0
        _c(ctnr)->setBody(ctnr, (mHotPiece*)table);
#else
        {
            mPanelPiece* panel = NEWPIECE(mPanelPiece);
            _c(panel)->addContent(panel, (mHotPiece*)table, 0, 0);
            _c(panel)->setRect(panel, &rc);
            _c(ctnr)->setBody(ctnr, (mHotPiece*)panel);
        }
#endif
    }

    create_delete_button(self);
    create_edit_button(self);
    return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, mymain_onCreate },
    {MSG_LBUTTONDOWN, SpeedMeterMessageHandler },
    {MSG_LBUTTONUP, SpeedMeterMessageHandler },
    {MSG_MOUSEMOVE, SpeedMeterMessageHandler },
    {0, NULL}
};

int MiniGUIMain(int argc, const char *argv[]) {
    MSG Msg;
    mWidget *mymain;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mSimpleTableViewPiece" , 0 , 0);
#endif

    ncsInitialize();
    REGISTER_NCS();
    MGNCS_INIT_CLASS(mSimpleTableView);
    MGNCS_INIT_CLASS(mIndexLocatePiece);

    mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "mSimpleTableViewPiece test",
            WS_CAPTION | WS_BORDER | WS_VISIBLE,
            WS_EX_AUTOSECONDARYDC,
            1,
            //0, 0, g_rcScr.right,g_rcScr.bottom,
            0, 0, 320 * 2 + 100, 580,
            HWND_DESKTOP,
            0, 0,
            NULL,
            NULL,
            mymain_handlers,
            0);

    mGEffInit();

    while(GetMessage(&Msg, mymain->hwnd))
    {   
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(mymain->hwnd);

    mGEffDeinit();
    ncsUninitialize();

    return 0;
}
