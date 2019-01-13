#include "DesktopToolbar.hh"
#include "global.h"

#include "cassert"

// position & size
#define TOOLBAR_W       ACTIVITY_W
#define TOOLBAR_H       DESKTOP_TOOLBAR_HEIGHT
#define TOOLBAR_X       0
#define TOOLBAR_Y       (ACTIVITY_H - TOOLBAR_H)
#define TOOLBAR_X_HIDE  (TOOLBAR_X+ACTIVITY_W)

#define RING_W      18
#define RING_H      18
#define RING_X_OFF  0
#define RING_Y_OFF  -2

#define BTN_MARGIN_H   5
#define BTN_MARGIN_V   5
static const int toolbar_subcontainer_size[][2] =
{
    { TOOLBAR_W/3-2*BTN_MARGIN_H, TOOLBAR_H-2*BTN_MARGIN_V },
    { TOOLBAR_W/3-2*BTN_MARGIN_H, TOOLBAR_H-2*BTN_MARGIN_V }, 
    { TOOLBAR_W/3-2*BTN_MARGIN_H, TOOLBAR_H-2*BTN_MARGIN_V }
};
static const int toolbar_subcontainer_xy[][2] =
{
    { 1*BTN_MARGIN_H,                                                                 BTN_MARGIN_V },
    { 2*BTN_MARGIN_H+toolbar_subcontainer_size[0][0],                                 BTN_MARGIN_V },
    { 3*BTN_MARGIN_H+toolbar_subcontainer_size[0][0]+toolbar_subcontainer_size[1][0], BTN_MARGIN_V }
};
static const int toolbar_btn_size[][2] =
{
    {34, 29},
    {30, 29},
    {29, 24}
};
static const int toolbar_btn_xy[][2] =
{
    {(toolbar_subcontainer_size[0][0]-toolbar_btn_size[0][0])/2, (toolbar_subcontainer_size[0][1]-toolbar_btn_size[0][1])/2},
    {(toolbar_subcontainer_size[1][0]-toolbar_btn_size[1][0])/2, (toolbar_subcontainer_size[1][1]-toolbar_btn_size[1][1])/2},
    {(toolbar_subcontainer_size[2][0]-toolbar_btn_size[2][0])/2, (toolbar_subcontainer_size[2][1]-toolbar_btn_size[2][1])/2}
};
static const int toolbar_ring_xy[][2] =
{
    {35, 3},
    {35, 3},
    {0,  0}
};

// color
#define BTN_COLOR_NORMAL    MakeRGBA(0x00, 0x00, 0x00, 0x00)
#define BTN_COLOR_DOWN      MakeRGBA(0x80, 0x80, 0x80, 0x80)

// image
static const char* toolbar_img_path[] =
{
    "res/lockscreen/sms_icon.png",
    "res/lockscreen/phone.png",
    "res/lockscreen/main_scr.png",
};
#define RING_IMG_PATH    "res/lockscreen/red_area.png"

// string
static const char* toolbar_ringnum[] =
{
    "3", "2", 
};

// gradient color
typedef struct _GradientData {
    float pos;
    ARGB color;
} GradientData;
static const GradientData toolbar_gradient[] = { 
	{ 0.00f, 0x7f0f1700, },
	{ 0.01f, 0x7f434343, },
	{ 0.04f, 0x7f2d2d2d, },
	{ 0.49f, 0x7f151515, },
	{ 0.51f, 0x7f010101, },
	{ 1.00f, 0x7f000000, },
};

static BOOL register4NCS();

DesktopToolbar::DesktopToolbar(HWND parent, bool show) :
    m_parent(parent)
{
    register4NCS();

    loadRes();
    initComponent(show);
}

DesktopToolbar::~DesktopToolbar()
{
    unloadRes();
}

void DesktopToolbar::addBtnEventListener(int whichBtn, int message, 
        NCS_CB_ONPIECEEVENT handler)
{
    assert(whichBtn == TB_MSG || whichBtn == TB_CALL || whichBtn == TB_MENU);

    mButtonPanelPiece* btn = m_toolbarBtn[whichBtn];
    ncsAddEventListener((mObject*)btn, (mObject*)m_toolbarCtrl, handler, message);
}

MGEFF_ANIMATION DesktopToolbar::getMoveAnimation(int destx, int desty, int duration, 
        enum EffMotionType curve)
{
    return _c(this->m_toolbarContainerShell)->movePieceWithAnimation(
            this->m_toolbarContainerShell, (mHotPiece*)this->m_toolbarContainer, 
            destx, desty, duration, curve);
}

void DesktopToolbar::loadRes()
{
    for (int i = 0; i < TB_MAX; ++i)
        m_toolbarBmp[i] = (PBITMAP) Load32Resource(toolbar_img_path[i], RES_TYPE_IMAGE, 0);

    m_toolbarRingBmp = (PBITMAP) Load32Resource(RING_IMG_PATH, RES_TYPE_IMAGE, 0);

    m_toolbarNumFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            8, 0);
}

void DesktopToolbar::unloadRes()
{
    ReleaseRes(Str2Key(RING_IMG_PATH));

    for (int i = 0; i < TB_MAX; ++i)
        ReleaseRes(Str2Key(toolbar_img_path[i]));

    DestroyLogFont(m_toolbarNumFont);
}

void DesktopToolbar::initComponent(bool show)
{
    RECT rc;
    int x = show ? 0 : TOOLBAR_X_HIDE;

    memset(&rc, 0, sizeof(rc));

    m_toolbarCtrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "m_toolbarCtrl",
            WS_VISIBLE, 
            WS_EX_TRANSPARENT,
            0,
            TOOLBAR_X, TOOLBAR_Y, TOOLBAR_W, TOOLBAR_H,
            m_parent,
            NULL, NULL, NULL, 0);
    assert(m_toolbarCtrl);
    mContainerCtrl* ctrl = m_toolbarCtrl;

    m_toolbarContainerShell = (mPanelPiece*)NEWPIECE(mPanelPiece);
    m_toolbarContainer = (mPanelPiece*)NEWPIECE(mPanelPiece);

    mPanelPiece *body = m_toolbarContainerShell;

    rc.right = TOOLBAR_W;
    rc.bottom = TOOLBAR_H;

    _c(body)->setRect (body, &rc);
    _c(ctrl)->setBody(ctrl, (mHotPiece*)body);

    body = m_toolbarContainer;
    _c(body)->setRect (body, &rc);
    _c(m_toolbarContainerShell)->addContent(m_toolbarContainerShell, 
            (mHotPiece*)body, x, 0);

    // backgroud
    {
        unsigned i;
        mHotPiece *back_gradient = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
        _c(back_gradient)->setRect (back_gradient, &rc);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(back_gradient)->setProperty (back_gradient, NCSP_TRANROUND_CORNERFLAG, 0);

        HBRUSH brush = _c((mShapeTransRoundPiece*)back_gradient)->getBrush(
                (mShapeTransRoundPiece*)back_gradient);

        /* 0.0f, 1.0f are needed */
        for (i=0; i<TABLESIZE(toolbar_gradient); ++i)
            MGPlusLinearGradientBrushAddColor(brush, toolbar_gradient[i].color, toolbar_gradient[i].pos);

        _c(body)->addContent(body, back_gradient, 0, 0);
    }

    // create buttons
    mButtonPanelPiece* btn;
    for (int i=0; i<TB_MAX; ++i) {
        m_toolbarBtn[i] = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
        btn = m_toolbarBtn[i];

        rc.right = toolbar_subcontainer_size[i][0];
        rc.bottom = toolbar_subcontainer_size[i][1];
        _c(btn)->setRect(btn, &rc);
        _c(btn)->setStateTable(btn, (mHotPiece*)_c(btn)->getBkgndPiece(btn), 
                NCSP_TRANROUND_BKCOLOR, BTN_COLOR_NORMAL, BTN_COLOR_DOWN);
        _c(btn)->setStateTable(btn, (mHotPiece*)_c(btn)->getBkgndPiece(btn),
                NCSP_TRANROUND_RADIUS, 0, 4);
        _c(btn)->setLayoutManager(btn, NULL);
        _c(btn)->setStates(btn);
        _c(body)->addContent(body, (mHotPiece*)btn,
                toolbar_subcontainer_xy[i][0], toolbar_subcontainer_xy[i][1]);

        mHotPiece* imagePiece = (mHotPiece*)NEWPIECE(mImagePiece);
        rc.right = toolbar_btn_size[i][0];
        rc.bottom = toolbar_btn_size[i][1];
        _c(imagePiece)->setRect(imagePiece, &rc);
        _c(imagePiece)->setProperty(imagePiece,
                NCSP_IMAGEPIECE_IMAGE, (DWORD)m_toolbarBmp[i]);
        _c(btn)->addContent(btn, (mHotPiece*)imagePiece,
                toolbar_btn_xy[i][0], toolbar_btn_xy[i][1]);

        mHotPiece* ringPiece;
        // Don't create ring for the last button
        if (i < 2) {
            // create ring piece
            ringPiece = (mHotPiece*)NEWPIECE(mImagePiece);
            rc.right = RING_W;
            rc.bottom = RING_H;
            _c(ringPiece)->setRect(ringPiece, &rc);
            _c(ringPiece)->setProperty(ringPiece,
                    NCSP_IMAGEPIECE_IMAGE, (DWORD)m_toolbarRingBmp);
            _c(btn)->addContent(btn, (mHotPiece*)ringPiece, 
                    toolbar_ring_xy[i][0], toolbar_ring_xy[i][1]);

            // create number piece
            m_toolbarNumPiece[i] = (mHotPiece*)NEWPIECE(mTextPiece);
            rc.right = RING_W;
            rc.bottom = RING_H;
            _c(m_toolbarNumPiece[i])->setRect(m_toolbarNumPiece[i], &rc);
            _c(m_toolbarNumPiece[i])->setProperty(m_toolbarNumPiece[i], 
                    NCSP_LABELPIECE_LABEL, (DWORD)toolbar_ringnum[i]);
            _c(m_toolbarNumPiece[i])->setProperty(m_toolbarNumPiece[i], 
                    NCSP_TEXTPIECE_LOGFONT, (DWORD)m_toolbarNumFont);
            _c(btn)->addContent(btn, (mHotPiece*)m_toolbarNumPiece[i], 
                    toolbar_ring_xy[i][0]+RING_X_OFF, toolbar_ring_xy[i][1]+RING_Y_OFF);
        }
    }
}

static BOOL register4NCS()
{
    MGNCS_INIT_CLASS(mPanelPiece);
    MGNCS_INIT_CLASS(mButtonPanelPiece);
    MGNCS_INIT_CLASS(mLayoutManager);
    MGNCS_INIT_CLASS(mCenterHBox);
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_REGISTER_COMPONENT(mContainerCtrl);

    return TRUE;
}

