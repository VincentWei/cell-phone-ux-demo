#include "MusicPlayerToolbar.hh"
#include "global.h"

#include "cassert"

// position & size
#define TOOLBAR_W  ACTIVITY_W
#define TOOLBAR_H  (480-423-11)
#define TOOLBAR_X  ACTIVITY_X
#define TOOLBAR_Y  (ACTIVITY_H-TOOLBAR_H)

static const int toolbar_subcontainer_size[][2] =
{
    { TOOLBAR_W/3, TOOLBAR_H, },
    { TOOLBAR_W/3, TOOLBAR_H, },
    { TOOLBAR_W/3, TOOLBAR_H, },
};
static const int toolbar_subcontainer_xy[][2] =
{
    { 0,                                                              5, },
    { 0+toolbar_subcontainer_size[0][0],                              5, },
    { 0+toolbar_subcontainer_size[0][0]+toolbar_subcontainer_size[1][0],    5, },
};
static const int toolbar_btn_size[][2] =
{
    { 23, 28, },
    { 23, 28, },
    { 23, 28, },
};
static const int toolbar_btn_xy[][2] =
{
    { (toolbar_subcontainer_size[0][0]-toolbar_btn_size[0][0])/2, (toolbar_subcontainer_size[0][1]-toolbar_btn_size[0][1])/2, },
    { (toolbar_subcontainer_size[1][0]-toolbar_btn_size[1][0])/2, (toolbar_subcontainer_size[1][1]-toolbar_btn_size[1][1])/2, },
    { (toolbar_subcontainer_size[2][0]-toolbar_btn_size[2][0])/2, (toolbar_subcontainer_size[2][1]-toolbar_btn_size[2][1])/2, },
};

// image
static const char* toolbar_img_path[] =
{
    "res/musicplayer/player/prev.png",
    "res/musicplayer/player/play.png",
    "res/musicplayer/player/next.png",
    "res/musicplayer/player/pause.png",
};

// gradient color
static ARGB gradient_color[] = {
    0x7f0f1700,0x7f434343,0x7f2d2d2d,0x7f151515,0x7f010101,0x7f000000
};

static float gradient_pos[] = {
    0.00f,0.01f,0.04f,0.49f,0.51f,1.00f
};
static ARGB down_gradient_color[] = {
    0xa0e2edf9, 0xa0a1abb7, 0xa0e2edf 
};

static float down_gradient_pos[] = {
    0.00f,0.5f,1.00f
};

MusicPlayerToolbar::MusicPlayerToolbar(HWND parent) :
    m_parent(parent)
{
    m_playState = PAUSE;

    loadRes();
    initComponent();
}

MusicPlayerToolbar::~MusicPlayerToolbar()
{
    unloadRes();
}

void MusicPlayerToolbar::addBtnEventListener(int btnIdx, int message, 
        NCS_CB_ONPIECEEVENT handler)
{
    assert(btnIdx == TB_PREV || btnIdx == TB_PLAY || btnIdx == TB_NEXT);

    mButtonPanelPiece* btnPanel = m_toolbarSubContainer[btnIdx];
    ncsAddEventListener((mObject*)btnPanel, (mObject*)m_toolbarCtrl, handler, message);
}

void MusicPlayerToolbar::loadRes()
{
    for (int i = 0; i < TB_MAX; ++i)
        m_toolbarBmp[i] = (PBITMAP) Load32Resource(toolbar_img_path[i], RES_TYPE_IMAGE, 0);

    m_pauseBmp = (PBITMAP) Load32Resource(toolbar_img_path[TB_MAX], RES_TYPE_IMAGE, 0);
}

void MusicPlayerToolbar::unloadRes()
{
    for (int i = 0; i < TB_MAX; ++i)
        ReleaseRes(Str2Key(toolbar_img_path[i]));

    ReleaseRes(Str2Key(toolbar_img_path[TB_MAX]));
}

void MusicPlayerToolbar::initComponent()
{
    RECT rc;

    memset(&rc, 0, sizeof(rc));

    m_toolbarCtrl = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "m_toolbarCtrl",
            WS_VISIBLE, WS_EX_TRANSPARENT, 0,
            TOOLBAR_X, TOOLBAR_Y, TOOLBAR_W, TOOLBAR_H,
            m_parent,
            NULL, NULL, NULL, 0);
    assert(m_toolbarCtrl);
    mContainerCtrl* ctrl = m_toolbarCtrl;

    m_toolbarContainerShell = (mPanelPiece*)NEWPIECE(mPanelPiece);

    mPanelPiece *body = m_toolbarContainerShell;

    rc.right = TOOLBAR_W;
    rc.bottom = TOOLBAR_H;

    _c(body)->setRect (body, &rc);
    _c(ctrl)->setBody(ctrl, (mHotPiece*)body);
    for (int i=0; i<TB_MAX; ++i) {
        m_toolbarSubContainer[i] = (mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
        rc.right = toolbar_subcontainer_size[i][0];
        rc.bottom = toolbar_subcontainer_size[i][1];
        _c(m_toolbarSubContainer[i])->setRect(m_toolbarSubContainer[i], &rc);

        mButtonPanelPiece* sub = m_toolbarSubContainer[i];
        mShapeTransRoundPiece* bkgnd = _c(sub)->getBkgndPiece(sub);

        _c(body)->addContent(body, (mHotPiece*)sub,
                toolbar_subcontainer_xy[i][0], toolbar_subcontainer_xy[i][1]);
        _c(bkgnd)->setProperty (bkgnd, NCSP_TRANROUND_CORNERFLAG,0);
        _c(sub)->setGradientBackgroundColor(sub,
                gradient_color,gradient_pos,TABLESIZE(gradient_color),
                down_gradient_color,down_gradient_pos,TABLESIZE(down_gradient_color));
        _c(sub)->setStates(sub);

        m_toolbarBtnPiece[i] = (mImagePiece*)NEWPIECE(mImagePiece);
        rc.right = toolbar_btn_size[i][0];
        rc.bottom = toolbar_btn_size[i][1];
        _c(m_toolbarBtnPiece[i])->setRect(m_toolbarBtnPiece[i], &rc);
        _c(m_toolbarBtnPiece[i])->setProperty(m_toolbarBtnPiece[i],
                NCSP_IMAGEPIECE_IMAGE, (DWORD)m_toolbarBmp[i]);
        _c(sub)->addContent(sub, (mHotPiece*)m_toolbarBtnPiece[i], 
                toolbar_btn_xy[i][0], toolbar_btn_xy[i][1]);
    }
}

