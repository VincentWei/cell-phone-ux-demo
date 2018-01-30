#include "global.h"
#include <minigui/control.h>

#include "sample-navigation.hh"

REGISTER_ACTIVITY(SampleNavigation);


SampleNavigation::SampleNavigation() {
}


SampleNavigation::~SampleNavigation() {
}


#define SAMPLE_FONT_COLOR 0xff000000 
//#define SAMPLE_BAR_COLOR  0xffd8d81c 
#define SAMPLE_BAR_COLOR1 0xffffff00 
#define SAMPLE_BAR_COLOR2 0xffff0000 
#define SAMPLE_ITEM_COLOR 0xff969696
#define SAMPLE_BUTTON_COLOR 0xffe6e6e6
#define SAMPLE_ITEM_W  ACTIVITY_W
#define SAMPLE_ITEM_H  ACTIVITY_H-NAVIGATIONBAR_H


static PLOGFONT font;
static mContainerCtrl *ctnr;
static mNavigationPanelPiece* body;
static mNavigationItem* item1;
static mNavigationItem* item2;
static mPanelPiece* item_piece1;
static mPanelPiece* item_piece2;


static BOOL button1_clicked_handler (mObject *self,
         mObject *sender, int event_id, DWORD param)
{
    printf("sample-navigation ==> push item\n");
    
    _c(body)->push(body, item2);

    printf("current %s root item\n", _c(body)->currentIsRoot(body) ? "is" : "isn't");

    return TRUE;
}

static BOOL show = FALSE;
static int count = 0;
static char buff[64];
static BOOL button2_clicked_handler (mObject *self,
         mObject *sender, int event_id, DWORD param)
{
    RECT rect;
    mNavigationBarPiece* bar;
    if ( show ) {
        mTextPiece* text_piece1;
        printf("sample-navigation ==> show navigation bar\n");
        count += 1;
        sprintf(buff, "count is:%d", count);
        bar = (mNavigationBarPiece*)_c(body)->getProperty(body, NCSP_NAVIGATIONPANELPIECE_BAR);
        SetRect(&rect, 0, 0, 150, NAVIGATIONBAR_DEFAULT_BUTTON_H);
        text_piece1 = (mTextPiece*)_c(bar)->getProperty(bar, NCSP_NAVIGATIONBARPIECE_TITLE_BUTTON);
        _c(text_piece1)->setRect(text_piece1, &rect);
        _c(bar)->movePiece(bar, (mHotPiece*)text_piece1, 
                NAVIGATIONBAR_TITLE_X(RECTW(rect)),
                NAVIGATIONBAR_DEFAULT_BUTTON_Y);
        _c(text_piece1)->setProperty(text_piece1, NCSP_LABELPIECE_LABEL, (DWORD)buff);
    }
    else
        printf("sample-navigation ==> hide navigation bar\n");

    _c(body)->showNavigationBar(body, show, TRUE);
    show = 1 - show;
    
    return TRUE;
}

LRESULT SampleNavigation::proc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam) {
    switch (message) {
        case MSG_CREATE: 
            {
            mTextPiece* text_piece1;
            mTextPiece* text_piece2;
            mTextPiece* text_piece3;
            mButtonPanelPiece* button_piece1;
            mButtonPanelPiece* button_piece2;
            mShapeTransRoundPiece* bar_bk_piece1;
            mShapeTransRoundPiece* bar_bk_piece2;
            mShapeTransRoundPiece* item_bk_piece;
            RECT rect;
            mShapeTransRoundPiece* bk_piece;

            font = CreateLogFont ("ttf", "helvetica", "GB2312",
                FONT_WEIGHT_BOOK, 
                FONT_SLANT_ROMAN,
                FONT_SETWIDTH_NORMAL,
                FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                14, 0);

            text_piece1 = (mTextPiece*)NEWPIECE(mTextPiece);
            SetRect(&rect, 0, 0, SAMPLE_ITEM_W, SAMPLE_ITEM_H);
            _c(text_piece1)->setRect(text_piece1, &rect);
            _c(text_piece1)->setProperty(text_piece1, NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
            _c(text_piece1)->setProperty(text_piece1, NCSP_LABELPIECE_LABEL, (DWORD)"Hello I am view1");
            _c(text_piece1)->setProperty(text_piece1, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)SAMPLE_FONT_COLOR);

            text_piece2 = (mTextPiece*)NEWPIECE(mTextPiece);
            SetRect(&rect, 0, 0, SAMPLE_ITEM_W, ACTIVITY_H);
            _c(text_piece2)->setRect(text_piece2, &rect);
            _c(text_piece2)->setProperty(text_piece2, NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
            _c(text_piece2)->setProperty(text_piece2, NCSP_LABELPIECE_LABEL, (DWORD)"Yes I am view2");
            _c(text_piece2)->setProperty(text_piece2, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)SAMPLE_FONT_COLOR);

            text_piece3 = (mTextPiece*)NEWPIECE(mTextPiece);
            SetRect(&rect, 0, 0, SAMPLE_ITEM_W, NAVIGATIONBAR_H);
            _c(text_piece3)->setRect(text_piece3, &rect);
            _c(text_piece3)->setProperty(text_piece3, NCSP_TEXTPIECE_LOGFONT, (DWORD)font);
            _c(text_piece3)->setProperty(text_piece3, NCSP_LABELPIECE_LABEL, (DWORD)"click me to show/hide navigation bar");
            _c(text_piece3)->setProperty(text_piece3, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)SAMPLE_FONT_COLOR);


            item_bk_piece = (mShapeTransRoundPiece*)NEWPIECE(mShapeTransRoundPiece);
            SetRect(&rect, 0, 0, SAMPLE_ITEM_W, ACTIVITY_H);
            _c(item_bk_piece)->setRect(item_bk_piece, &rect);
            _c(item_bk_piece)->setProperty(item_bk_piece, NCSP_TRANROUND_BKCOLOR, (DWORD)SAMPLE_ITEM_COLOR);


            button_piece2 = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
            SetRect(&rect, 0, 0, SAMPLE_ITEM_W, NAVIGATIONBAR_H);
            _c(button_piece2)->setRect(button_piece2, &rect);
            _c(button_piece2)->addContent(button_piece2, (mHotPiece*)text_piece3, 0, 0);
            bk_piece = _c(button_piece2)->getBkgndPiece(button_piece2);
            _c(bk_piece)->setProperty(bk_piece, NCSP_TRANROUND_SHARPFLAG, 0);
            _c(bk_piece)->setProperty(bk_piece, NCSP_TRANROUND_BKCOLOR, (DWORD)0xff0000ff);

            ncsAddEventListener((mObject*)button_piece2, (mObject*)button_piece2,
                (NCS_CB_ONPIECEEVENT)button2_clicked_handler, NCSN_ABP_CLICKED);


            item_piece1 = (mPanelPiece*)NEWPIECE(mPanelPiece);
            SetRect(&rect, 0, 0, SAMPLE_ITEM_W, SAMPLE_ITEM_H);
            _c(item_piece1)->setRect(item_piece1, &rect);
            _c(item_piece1)->setBkgndPiece(item_piece1, item_bk_piece);
            _c(item_piece1)->addContent(item_piece1, (mHotPiece*)text_piece1, 0, 0);
            ADDREFPIECE(item_piece1);

            item_piece2 = (mPanelPiece*)NEWPIECE(mPanelPiece);
            SetRect(&rect, 0, 0, SAMPLE_ITEM_W, ACTIVITY_H);
            _c(item_piece2)->setRect(item_piece2, &rect);
            _c(item_piece2)->setBkgndPiece(item_piece2, item_bk_piece);
            _c(item_piece2)->addContent(item_piece2, (mHotPiece*)text_piece2, 0, 0);
            _c(item_piece2)->addContent(item_piece2, (mHotPiece*)button_piece2, 0, SAMPLE_ITEM_H);
            //_c(item_piece2)->addContent(item_piece2, (mHotPiece*)button_piece2, 0, SAMPLE_ITEM_H-NAVIGATIONBAR_H);
            ADDREFPIECE(item_piece2);


            bar_bk_piece1 = (mShapeTransRoundPiece*)NEWPIECE(mShapeTransRoundPiece);
            SetRect(&rect, 0, 0, NAVIGATIONBAR_W, NAVIGATIONBAR_H);
            _c(bar_bk_piece1)->setRect(bar_bk_piece1, &rect);
            _c(bar_bk_piece1)->setProperty(bar_bk_piece1, NCSP_TRANROUND_BKCOLOR, (DWORD)SAMPLE_BAR_COLOR1);
            _c(bar_bk_piece1)->setProperty(bar_bk_piece1, NCSP_TRANROUND_CORNERFLAG, (DWORD)0);

            bar_bk_piece2 = (mShapeTransRoundPiece*)NEWPIECE(mShapeTransRoundPiece);
            SetRect(&rect, 0, 0, NAVIGATIONBAR_W, NAVIGATIONBAR_H);
            _c(bar_bk_piece2)->setRect(bar_bk_piece2, &rect);
            _c(bar_bk_piece2)->setProperty(bar_bk_piece2, NCSP_TRANROUND_BKCOLOR, (DWORD)SAMPLE_BAR_COLOR2);
            _c(bar_bk_piece2)->setProperty(bar_bk_piece2, NCSP_TRANROUND_CORNERFLAG, (DWORD)0);

            item1 = CreateNavigationItem ((mHotPiece*)item_piece1, 
                    (const char*)"view1",  NAVIGATION_STYLE_NORMAL);

            button_piece1 = (mButtonPanelPiece*)_c(item1)->createDefaultButton(item1, "goto 2", 0);
            AdjustNavigationItemRectWithTitle((mHotPiece*)button_piece1, "goto 2", item1->default_button_font);
            bk_piece = _c(button_piece1)->getBkgndPiece(button_piece1);
            _c(bk_piece)->setProperty(bk_piece, NCSP_TRANROUND_SHARPFLAG, TRANROUND_SHARPFLAG_RIGHT);

            ncsAddEventListener((mObject*)button_piece1, (mObject*)button_piece1,
                (NCS_CB_ONPIECEEVENT)button1_clicked_handler, NCSN_ABP_CLICKED);
            _c(item1)->setProperty(item1, NCSP_NAVIGATIONITEM_RIGHT_BUTTON, (DWORD)button_piece1);
            _c(item1)->setProperty(item1, NCSP_NAVIGATIONITEM_BACKGROUND, (DWORD)bar_bk_piece1);

            item2 = CreateNavigationItem ((mHotPiece*)item_piece2, 
            //        (const char*)"view2", NAVIGATION_STYLE_NORMAL);
                    (const char*)"view2", NAVIGATION_STYLE_FULLSCREEN);
            _c(item2)->setProperty(item2, NCSP_NAVIGATIONITEM_BACKGROUND, (DWORD)bar_bk_piece2);

            body = CreateNavigationPanelPieceWithRootView (item1);
            SetRect(&rect, 0, 0, ACTIVITY_W, ACTIVITY_H);
            _c(body)->setRect(body, (const RECT*)&rect);


            ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
                    "ContainerCtrl",
                    WS_VISIBLE, 0, 0,
                    0, 0, ACTIVITY_W, ACTIVITY_H,
                    hWnd,
                    NULL, NULL, NULL, 0);
            _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

            printf("current %s root item\n", _c(body)->currentIsRoot(body) ? "is" : "isn't");

            break;
        }

        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            return 0;

        case MSG_DESTROY:
            UNREFPIECE(item_piece1);
            UNREFPIECE(item_piece2);
            DestroyNavigationItem (item1);
            DestroyNavigationItem (item2);

            DestroyLogFont(font);
            return 0;

        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

HWND SampleNavigation::createHWND() {
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
    CreateInfo.spCaption = "sample-navigation";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = SampleNavigation::proc;
    CreateInfo.lx =  ACTIVITY_X;
    CreateInfo.ty = ACTIVITY_Y;
    CreateInfo.rx = ACTIVITY_X + ACTIVITY_W;
    CreateInfo.by = ACTIVITY_Y + ACTIVITY_H;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    return CreateMainWindow(&CreateInfo);
}
