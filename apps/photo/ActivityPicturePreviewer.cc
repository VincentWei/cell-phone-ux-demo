#include "ActivityPicturePreviewer.hh"

REGISTER_ACTIVITY(ActivityPicturePreviewer);

#define thisActivity(hwnd)  ((ActivityPicturePreviewer*)Activity::getActivityFromHWND(hwnd))

#define IDC_ICONFLOW    200
#define IDC_TB          201

#define PP_FONT_COLOR 0xff192952
#define PP_TITLE_FONT_SHADOW_COLOR 0x66ffffff

#define PHOTO_TITLE_H 38 
#define PHOTO_BTN_H 27

#define PP_TITLE_FONT_H 20
static ARGB COLORS_TITLE[] = {0xffd0d571,0xffb5c363,0xff9eb23a,0xff7f9e2c,0xff597311,0xff597311};
static float POS_TITLE[] = {0.0,0.49,0.50,0.96,0.97,1.0};

/*
static RECT PLAY_BTN_RECT = {10, (PHOTO_TITLE_H - PHOTO_BTN_H)>>1, 
    61, PHOTO_BTN_H + ((PHOTO_TITLE_H - PHOTO_BTN_H)>>1)};
    */
#define BTN_FONT_H 16
//static ARGB COLORS_BTN_NORMAL[] = {0xffffffff, 0xffd1d1d1};
//static ARGB COLORS_BTN_PUSHED[] = {0xa0e2edf9, 0xa0a1abb7, 0xa0e2edf9};

static ARGB COLORS_ICONFLOW_BK[] = {0xff646873, 0xff2e2f32};

static const char *PICTURES[] = {
    "res/pictureviewer/1.png",
    "res/pictureviewer/2.png",
    "res/pictureviewer/3.png",
    "res/pictureviewer/4.png",
    "res/pictureviewer/5.png",
    "res/pictureviewer/6.png",
    "res/pictureviewer/7.png",
    "res/pictureviewer/8.png",
    "res/pictureviewer/9.png",
};

static BOOL iconv_init(mDialogBox* self)
{
    NCS_ICONFLOW_ITEMINFO info;
    unsigned i = 0;
    int pos = 0;
    mIconFlow *iconvObj;
    HWND iconvWnd;
    RECT rc;
    mShapeTransRoundPiece *bk = NEWPIECE(mShapeTransRoundPiece);

    iconvWnd = GetDlgItem (self->hwnd, IDC_ICONFLOW);
    iconvObj = (mIconFlow*)ncsObjFromHandle(iconvWnd);

    if (!iconvObj) assert(0);

// START_OF_ADDITEMS
    for(i = 0; i < TABLESIZE(PICTURES); i++)
    {
        pos = 0; 
        memset (&info, 0, sizeof(NCS_ICONFLOW_ITEMINFO)); 
        info.bmp = (PBITMAP)GetResource(Str2Key(PICTURES[i]));
        info.index = i; 
        info.label = PICTURES[i]; 
        info.addData = (DWORD)PICTURES[i]; 
        _c(iconvObj)->addItem(iconvObj, &info, &pos); 
    } 
    _c(iconvObj)->setCurSel(iconvObj, 0); 
// END_OF_ADDITEMS

    _c(iconvObj)->setIconSize (iconvObj, ACTIVITY_W - 20, ACTIVITY_H / 2);
    _c(iconvObj)->setProperty (iconvObj, NCSP_ICONFLOW_SPAN, (DWORD)(ACTIVITY_H/2));
    _c(iconvObj)->setProperty (iconvObj, NCSP_ICONFLOW_ICONBORDER, 1);

    GetWindowRect(iconvWnd, &rc);
    SetRect(&rc, 0, 0, RECTW(rc), RECTH(rc));
    _c(bk)->setRect(bk, &rc);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, 0);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_FILLMODE, (DWORD)NCSP_TRANROUND_GRADIENT_FILL);
    _c(bk)->setGradientFillColors(bk, COLORS_ICONFLOW_BK, TABLESIZE(COLORS_ICONFLOW_BK));

    _c(iconvObj)->setProperty (iconvObj, NCSP_ICONFLOW_BKGNDPIECE, (DWORD)bk);

    return TRUE;
}

#if 0
static BOOL onPieceClicked(mHotPiece *self, mHotPiece *sender, int event_id, DWORD param)
{
    if (event_id == NCSN_ABP_CLICKED) {
        printf("self %p -- NCSN_ABP_CLICKED\n", self);
        ActivityStack::singleton()->pop();
    }
    return TRUE;
}
#endif

static BOOL toolbar_onCreate(mWidget *self, DWORD add_data)
{
    RECT rc;
    RECT rcPiece;
    ActivityPicturePreviewer *activity = thisActivity(GetParent(self->hwnd));

    mPanelPiece *body = NEWPIECE(mPanelPiece);

    GetWindowRect(self->hwnd, &rc);
    /* body_bk */
    {
        mShapeTransRoundPiece *body_bk = NEWPIECE(mShapeTransRoundPiece);
        SetRect(&rcPiece, 0, 0, RECTW(rc), RECTH(rc));
        _c(body_bk)->setRect(body_bk, &rcPiece);
        _c(body_bk)->setProperty(body_bk, NCSP_TRANROUND_RADIUS, 0);
        _c(body_bk)->setProperty(body_bk, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        HBRUSH brush = _c(body_bk)->getBrush(body_bk);
        MGPlusSetLinearGradientBrushColorsEx(brush, COLORS_TITLE, TABLESIZE(COLORS_TITLE), POS_TITLE);
        _c(body)->addContent(body, (mHotPiece*)body_bk, 0, 0);
    }

    /* body_txt */
    {
        mHotPiece *body_txt = (mHotPiece*)NEWPIECE(mTextPiece);
        SetRect(&rcPiece, 0, 0, RECTW(rc), RECTH(rc));
        _c(body_txt)->setRect(body_txt, &rcPiece);
        _c(body_txt)->setProperty(body_txt, NCSP_LABELPIECE_LABEL,(DWORD)"Picture");
        _c(body_txt)->setProperty(body_txt, NCSP_TEXTPIECE_LOGFONT,(DWORD)activity->getTitleFont());
        _c(body_txt)->setProperty(body_txt, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)PP_FONT_COLOR);
        _c(body_txt)->setProperty(body_txt, NCSP_TEXTPIECE_TEXTSHADOWCOLOR,
                (DWORD)PP_TITLE_FONT_SHADOW_COLOR);
        _c(body)->addContent(body, body_txt, 0, 0);
    }

#if 0
    /* body_btn */
    {
        mButtonPanelPiece *body_btn = NEWPIECE(mButtonPanelPiece);
        mHotPiece *btn_txt = (mHotPiece*)NEWPIECE(mTextPiece);
        mHotPiece *btn_bk = (mHotPiece*)_c(body_btn)->getBkgndPiece(body_btn);
        _c(btn_bk)->setProperty(btn_bk, NCSP_TRANROUND_RADIUS, 2);
        SetRect(&rcPiece, 0, 0, RECTW(PLAY_BTN_RECT), RECTH(PLAY_BTN_RECT));
        _c(btn_txt)->setRect(btn_txt, &rcPiece);
        _c(btn_txt)->setProperty(btn_txt, NCSP_LABELPIECE_LABEL, (DWORD)"Play");
        _c(btn_txt)->setProperty(btn_txt, NCSP_TEXTPIECE_LOGFONT, (DWORD)activity->getTitleBtnFont());
        _c(btn_txt)->setProperty(btn_txt, NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)PP_FONT_COLOR);
        _c(body_btn)->addContent(body_btn, btn_txt, 0, 0);
        _c(body_btn)->setRect(body_btn, &rcPiece);
        _c(body_btn)->setGradientBackgroundColor(body_btn,
                COLORS_BTN_NORMAL, NULL, TABLESIZE(COLORS_BTN_NORMAL),
                COLORS_BTN_PUSHED, NULL, TABLESIZE(COLORS_BTN_PUSHED)
                );
        _c(body_btn)->setStates(body_btn);
        _c(body)->addContent(body, (mHotPiece*)body_btn, PLAY_BTN_RECT.left, PLAY_BTN_RECT.top);

        int event_ids[] = {NCSN_ABP_CLICKED, 0};
        ncsAddEventListeners((mObject*)body_btn, (mObject*)body_btn,
                (NCS_CB_ONPIECEEVENT)onPieceClicked, event_ids);
    }
#endif

    /* body */
    _c(body)->setRect(body, &rc);
    _c((mContainerCtrl*)self)->setBody((mContainerCtrl*)self, (mHotPiece*)body);

    return TRUE;
}

static BOOL toolbar_onClose(mWidget *self, int message)
{
    return TRUE;
}
static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static NCS_EVENT_HANDLER toolbar_handlers[] = {
    {MSG_CREATE,    reinterpret_cast<void*>(toolbar_onCreate)},
    {MSG_CLOSE,     reinterpret_cast<void*>(toolbar_onClose)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {0, NULL}
};

// START_OF_ICONVHANDLERS
static void iconv_notify (mWidget *self, int id, int nc, DWORD add_data)
{
    if (nc == NCSN_ICONFLOW_CLICKED)
    {
        if (self) {
            int idx;
            const char  *text;
            mIconFlow   *iconvObj = (mIconFlow*)self;

            HWND main_hwnd = GetMainWindowHandle(self->hwnd);
            ActivityPicturePreviewer *act = 
                (ActivityPicturePreviewer*)Activity::getActivityFromHWND(main_hwnd);
            assert(act);

            idx = _c(iconvObj)->indexOf(iconvObj, (HITEM)add_data);
            text = _c(iconvObj)->getText(iconvObj, (HITEM)add_data);
            fprintf (stderr, "click icon[%d], text is %s \n", idx, text);

            act->m_intent->putExtra("index",text);
            ACTIVITYSTACK->switchTo("ActivityPicturePlayer",act->m_intent);
        }
    }
}

static NCS_EVENT_HANDLER iconv_handlers[] = {
    NCS_MAP_NOTIFY(NCSN_ICONFLOW_CLICKED, iconv_notify),
    NCS_MAP_NOTIFY(NCSN_ICONFLOW_SELCHANGED, iconv_notify),
	{0, NULL }
};
// END_OF_ICONVHANDLERS

static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        IDC_TB,
        0, 0, ACTIVITY_W, PHOTO_TITLE_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        toolbar_handlers,
        NULL,
        0, 0,
    },
	{
		NCSCTRL_ICONFLOW, 
		IDC_ICONFLOW,
        0, PHOTO_TITLE_H, ACTIVITY_W, ACTIVITY_H - PHOTO_TITLE_H,
        WS_CHILD | WS_VISIBLE | NCSS_NOTIFY | NCSS_ICONFLOW_LOOP | NCSS_ICONFLOW_VERTICAL,
		WS_EX_NONE,
		"",
		NULL,
		NULL,
		iconv_handlers,
		NULL,
		0, 0
	}
};

/* main window */
static BOOL mainwnd_onCreate (mWidget* self, DWORD add_data)
{
    iconv_init((mDialogBox*)self);
    return TRUE;
}

static void mainwnd_onClose (mWidget* self, int message)
{
    return;
}

static BOOL mainwnd_onEraseBkgnd(mWidget *self, HDC hdc, const RECT *pinv)
{
    return TRUE;
}

static NCS_EVENT_HANDLER mainwnd_handlers[] = {
    {MSG_CREATE,    reinterpret_cast<void*>(mainwnd_onCreate)},
    {MSG_CLOSE,     reinterpret_cast<void*>(mainwnd_onClose)},
    {MSG_ERASEBKGND,reinterpret_cast<void*>(mainwnd_onEraseBkgnd)},
	{0, NULL }
};

static NCS_MNWND_TEMPLATE dialog_tmpl = {
    NCSCTRL_DIALOGBOX, 
    1,
    ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H, /* full screen */
    WS_NONE,
    WS_EX_NONE,
    "ActivityPicturePreviewer",
    NULL,
    NULL,
    mainwnd_handlers,
	_ctrl_tmpl,
	sizeof(_ctrl_tmpl)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

void ActivityPicturePreviewer::loadResource()
{
    unsigned i;
    //SetResPath("res/pictureviewer/");
    for (i=0; i<TABLESIZE(PICTURES); ++i) {
        Load32Resource (PICTURES[i], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    }

    m_titleFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            PP_TITLE_FONT_H, 0);

    m_titleBtnFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            BTN_FONT_H, 0);
}

void ActivityPicturePreviewer::unloadResource()
{
    unsigned i;
    for (i=0; i<TABLESIZE(PICTURES); ++i) {
        ReleaseRes(Str2Key(PICTURES[i]));
    }

    DestroyLogFont(m_titleBtnFont);
    DestroyLogFont(m_titleFont);
}

ActivityPicturePreviewer::ActivityPicturePreviewer():
    NCSActivity( &dialog_tmpl)
{
    loadResource ();
    m_intent = Intent::newIntent();
    m_style = STYLE_ALPHA;
}

ActivityPicturePreviewer::~ActivityPicturePreviewer()
{
    Intent::deleteIntent(m_intent);
    unloadResource ();
}

