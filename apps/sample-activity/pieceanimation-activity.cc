#include "pieceanimation-activity.hh"
#include "mcontainerctrl.h"
#include "mlayout_manager.h"
#include "mpanelpiece.h"
#include "mtransroundpiece.h"

REGISTER_ACTIVITY(PieceAnimationActivity);

#define IDC_CTRL 100
#define ANIMATION_RUNTIME 600

static mHotPiece *ct1;
static mHotPiece *ct2;
static mHotPiece *ct3;
static mPanelPiece *ct4;
static mHotPiece *ct4_ct1;
static mPanelPiece *ct5;
static mHotPiece* ct5_back;
static mHotPiece *ct5_ct1;
static int x = 85, y = 120;

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0,
            IDC_CTRL,
            10, 10, 320, 400,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    mHotPiece *ct_back = (mHotPiece*)NEWPIECE(mRectPiece);
    ct1 = (mHotPiece*)NEWPIECE(mLabelPiece);
    ct2 = (mHotPiece*)NEWPIECE(mImagePiece);
    ct3 = (mHotPiece*)NEWPIECE(mImagePiece);
    ct4 = (mPanelPiece*)NEWPIECE(mPanelPiece);
    ct4_ct1 = (mHotPiece*)NEWPIECE(mImagePiece);
    ct5 = (mPanelPiece*)NEWPIECE(mPanelPiece);
    ct5_ct1 = (mHotPiece*)NEWPIECE(mImagePiece);
    ct5_back = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);

    RECT rc = {0, 0, 320, 400};
    _c(body)->setRect (body, &rc);
    _c(ctnr)->setBody(ctnr, (mHotPiece*)body);

    rc.right = 120;
    rc.bottom = 40; 
    _c(ct1)->setRect (ct1, &rc);
    _c(ct1)->setProperty (ct1, NCSP_LABELPIECE_LABEL, (DWORD)"#Magic#");

    /* add rect piece as background.*/
    rc.right  = 320-5;
    rc.bottom = 400-5;
    _c(ct_back)->setRect (ct_back, &rc);
    _c(ct_back)->setProperty (ct_back, NCSP_RECTPIECE_FILLCOLOR, MakeRGBA(0xff, 0x00, 0x00, 0x80));
    _c(ct_back)->setProperty (ct_back, NCSP_RECTPIECE_YRADIUS, 5);
    _c(ct_back)->setProperty (ct_back, NCSP_RECTPIECE_BORDERSIZE, 5);
    _c(ct_back)->setProperty (ct_back, NCSP_RECTPIECE_XRADIUS, 5);

    /* add image.*/
    //LoadResource(HDC_SCREEN, RES_TYPE_IMAGE, "new2.png");
    static BITMAP bmp[4];
    LoadBitmap(HDC_SCREEN, &bmp[0], "res/sample-activity/news.png");
    LoadBitmap(HDC_SCREEN, &bmp[1], "res/sample-activity/file.png");
    LoadBitmap(HDC_SCREEN, &bmp[2], "res/sample-activity/internet.png");
    LoadBitmap(HDC_SCREEN, &bmp[3], "res/lockscreen/background.png");

    rc.right  = bmp[0].bmWidth;
    rc.bottom = bmp[0].bmHeight;

    _c(ct2)->setRect (ct2, &rc);
    _c(ct2)->setProperty (ct2, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[0]);

    rc.right = bmp[1].bmWidth;
    rc.bottom = bmp[1].bmHeight;
    _c(ct3)->setRect (ct3, &rc);
    _c(ct3)->setProperty (ct3, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[1]);

    rc.right = bmp[2].bmWidth;
    rc.bottom = bmp[2].bmHeight;
    _c(ct4_ct1)->setRect (ct4_ct1, &rc);
    _c(ct4_ct1)->setProperty (ct4_ct1, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[2]);

    _c(ct5_ct1)->setRect(ct5_ct1, &rc);
    _c(ct5_ct1)->setProperty(ct5_ct1, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[2]);

    rc.right = bmp[3].bmWidth / 2;
    rc.bottom = bmp[3].bmHeight / 2;
    _c(ct5)->setRect(ct5, &rc);
    _c(ct5_back)->setRect(ct5_back, &rc);
#if 0
    _c(ct5_back)->setProperty(ct5_back, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[3]);
#else
//    _c(ct5_back)->setProperty(ct5_back, NCSP_TRANROUND_BKCOLOR, 0x3fff0000);
    _c(ct5_back)->setProperty(ct5_back, NCSP_TRANROUND_BORDERSIZE, 4);
    _c(ct5_back)->setProperty(ct5_back, NCSP_TRANROUND_BORDERCOLOR, 0xffffffff);
#if 1
    _c(ct5_back)->setProperty(ct5_back, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    ARGB colors[]={0x3fff0000, 0x7f00ff00, 0xaf003fff, 0xff0000ff};
    _c((mShapeTransRoundPiece*)ct5_back)->setGradientFillColors((mShapeTransRoundPiece*)ct5_back, colors, 4);
#endif
#endif

    rc.right = 100;
    rc.bottom = 100;
    _c(ct4)->setRect (ct4, &rc);

    _c(ct5)->addContent(ct5, ct5_back, 0, 0);
#if 1
    _c(ct5)->addContent(ct5, ct5_ct1,
            (bmp[3].bmWidth / 2 - bmp[2].bmWidth) / 2,
            (bmp[3].bmHeight / 2 - bmp[2].bmHeight) / 2);
#endif
//    _c(ct5)->setPieceAlpha(ct5, ct5_back, 128);
    //_c(ct4)->addContent(ct4, ct4_ct1, 5, 5);

    _c(body)->addContent(body, ct_back, 5, 5);
    _c(body)->addContent(body, ct1, x, y);
    _c(body)->addContent(body, ct2, 200, 40);
    _c(body)->addContent(body, ct3, 40, 250);
    _c(body)->addContent(body, ct4_ct1, 40, 40);
    _c(body)->addContent(body, (mHotPiece*)ct5, 60, 60);
//    _c(body)->setPieceAlpha(body, ct3, 128);
//    _c(body)->setPieceAlpha(body, (mHotPiece*)ct5, 128);
//    _c(body)->rotatePiece(body, (mHotPiece*)ct5, 135, 1, 0, 0);
    return TRUE;
}

static BOOL mymain_onClose (mWidget* _this, int message)
{
    return TRUE;
}

static int mymain_onKeyUp(mWidget* self, int scancode, int status)
{
    mContainerCtrl *ctnr = (mContainerCtrl*)ncsGetChildObj(self->hwnd, IDC_CTRL);
    mPanelPiece *body = (mPanelPiece*)ctnr->body;

    MGEFF_ANIMATION anim1_1 = _c(body)->movePieceWithAnimation(body, ct1, x, y - 15, ANIMATION_RUNTIME, Linear);
    MGEFF_ANIMATION anim1_2 = _c(body)->movePieceWithAnimation(body, ct1, x, y, ANIMATION_RUNTIME, Linear);
    POINT anim1_2_startValue = {x, y - 15};
    mGEffAnimationSetStartValue(anim1_2, &anim1_2_startValue);
    MGEFF_ANIMATION anim1 = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);
    mGEffAnimationAddToGroup(anim1, anim1_1);
    mGEffAnimationAddToGroup(anim1, anim1_2);

    MGEFF_ANIMATION anim2_1 = _c(body)->rotatePieceWithAnimation(body, ct2, 90.0, 0, 1, 0, ANIMATION_RUNTIME, Linear);
    MGEFF_ANIMATION anim2_2 = _c(body)->rotatePieceWithAnimation(body, ct2, 180.0, 0, 1, 0, ANIMATION_RUNTIME, Linear);
    float anim2_2_startValue = 90.0;
    mGEffAnimationSetStartValue(anim2_2, &anim2_2_startValue);
    MGEFF_ANIMATION anim2 = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);
    mGEffAnimationAddToGroup(anim2, anim2_1);
    mGEffAnimationAddToGroup(anim2, anim2_2);

    MGEFF_ANIMATION anim3_1 = _c(body)->scalePieceWithAnimation(body, ct3, 2.0, 2.0, ANIMATION_RUNTIME, Linear);
    MGEFF_ANIMATION anim3_2 = _c(body)->scalePieceWithAnimation(body, ct3, 1.0, 1.0, ANIMATION_RUNTIME, Linear);
    POINT anim3_2_startValue = {2, 2};
    mGEffAnimationSetStartValue(anim3_2, &anim3_2_startValue);
    MGEFF_ANIMATION anim3 = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);
    mGEffAnimationAddToGroup(anim3, anim3_1);
    mGEffAnimationAddToGroup(anim3, anim3_2);

    MGEFF_ANIMATION anim4_1 = _c(body)->setPieceAlphaWithAnimation(body, ct4_ct1, 0, ANIMATION_RUNTIME, Linear);
    MGEFF_ANIMATION anim4_2 = _c(body)->setPieceAlphaWithAnimation(body, ct4_ct1, 255, ANIMATION_RUNTIME, Linear);
    int anim4_2_startValue = 0;
    mGEffAnimationSetStartValue(anim4_2, &anim4_2_startValue);
    MGEFF_ANIMATION anim4 = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);
    mGEffAnimationAddToGroup(anim4, anim4_1);
    mGEffAnimationAddToGroup(anim4, anim4_2);

    MGEFF_ANIMATION anim5_1 = _c(body)->rotatePieceWithAnimation(body, (mHotPiece*)ct5, 90.0, 0, 1, 0, ANIMATION_RUNTIME, Linear);
    MGEFF_ANIMATION anim5_2 = _c(body)->rotatePieceWithAnimation(body, (mHotPiece*)ct5, 180.0, 0, 1, 0, ANIMATION_RUNTIME, Linear);
    float anim5_2_startValue = 90.0;
    mGEffAnimationSetStartValue(anim5_2, &anim5_2_startValue);
    MGEFF_ANIMATION anim5 = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);
    mGEffAnimationAddToGroup(anim5, anim5_1);
    mGEffAnimationAddToGroup(anim5, anim5_2);

    MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    mGEffAnimationAddToGroup(group, anim1);
    mGEffAnimationAddToGroup(group, anim2);
    mGEffAnimationAddToGroup(group, anim3);
    mGEffAnimationAddToGroup(group, anim4);
    mGEffAnimationAddToGroup(group, anim5);

    _c(body)->animationSyncRunAndDelete(body, group);
    return 1;
}

#if 0
static BOOL mymain_onEraseBkgnd(mWidget* self, HDC hdc, const PRECT clip)
{
    return TRUE;
}
#endif
static NCS_EVENT_HANDLER mymain_handlers [] = {
    {MSG_CREATE, reinterpret_cast<void*>(mymain_onCreate)},
    {MSG_CLOSE, reinterpret_cast<void*>(mymain_onClose)},
    {MSG_KEYUP, reinterpret_cast<int*>(mymain_onKeyUp)},
//    {MSG_ERASEBKGND, reinterpret_cast<BOOL*>(mymain_onEraseBkgnd)},
    {0, NULL}
};

static NCS_MNWND_TEMPLATE mymain_templ = {
    NCSCTRL_DIALOGBOX, 
    1,
    ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
    WS_NONE,
    WS_EX_AUTOSECONDARYDC,
    "Window Template",
    NULL,
    NULL,
    mymain_handlers,
    NULL,
    0,
    0,
    0, 0,
};


PieceAnimationActivity::PieceAnimationActivity() :
    NCSActivity( &mymain_templ)
{ 
    m_style = STYLE_PUSH;
}

PieceAnimationActivity::~PieceAnimationActivity() {
}
