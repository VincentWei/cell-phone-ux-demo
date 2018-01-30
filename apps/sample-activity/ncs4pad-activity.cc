#include "global.h"

#include "ncs4pad-activity.hh"

REGISTER_ACTIVITY(Ncs4padActivity);


static BOOL onLeftButtonClick(mContainerCtrl *self,
        mHotPiece *sender, int event_id, DWORD param)
{
    if(event_id ==  NCSN_ABP_CLICKED)
    {
        printf("%s:%d.\n", __FUNCTION__, __LINE__);
#if 0
        ncsNotifyParentEx((mWidget*)self,
                NCSN_TC_BUTTONCLICK, (DWORD)NCS_TC_LEFT_CLICK);
#endif
        return TRUE;
    }
    return FALSE;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr;
    ctnr = (mContainerCtrl*)ncsCreateWindow(NCSCTRL_CONTAINERCTRL,
            "ContainerCtrl",
            WS_VISIBLE, 0, 0,
            10, 10, 320, 400,
            self->hwnd,
            NULL, NULL, NULL, 0);

    mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);

    RECT rc = {0, 0, 100, 20};
    rc.right = 320;
    rc.bottom = 400;
    _c(body)->setRect (body, &rc);
    _c(ctnr)->setBody(ctnr, (mHotPiece*)body);
    printf("%s:%d.\n", __FUNCTION__, __LINE__);

    mHotPiece *ct1 = (mHotPiece*)NEWPIECE(mLabelPiece);
    mHotPiece *ct2 = (mHotPiece*)NEWPIECE(mLabelPiece);
    mHotPiece *back = (mHotPiece*)NEWPIECE(mRectPiece);
    mHotPiece *ct4 = (mHotPiece*)NEWPIECE(mImagePiece);
    mHotPiece *ct5 = (mHotPiece*)NEWPIECE(mImagePiece);
    mHotPiece *ct6 = (mHotPiece*)NEWPIECE(mImagePiece);
    mHotPiece *ct7 = (mHotPiece*)NEWPIECE(mPushButtonPiece);
    mHotPiece *ct7_ct = (mHotPiece*)NEWPIECE(mLabelPiece);
    rc.right = 50;
    rc.bottom = 50;
    _c(ct7_ct)->setProperty(ct7_ct, NCSP_LABELPIECE_LABEL, (DWORD)"good");
    _c(ct7_ct)->setRect(ct7_ct, &rc);
    _c(ct7)->setRect (ct7, &rc);
    ((mPushButtonPiece*)ct7)->content = ct7_ct;
    
    _c(ct1)->setRect (ct1, &rc);
    rc.right = 100;
    rc.bottom = 20;
    _c(ct2)->setRect (ct2, &rc);

    /* add rect piece as background.*/
    rc.right  = 320-5;
    rc.bottom = 400-5;
    _c(back)->setRect (back, &rc);
    _c(back)->setProperty (back, NCSP_RECTPIECE_FILLCOLOR, MakeRGBA(0x00, 0xff, 0x00, 0x80));
    _c(back)->setProperty (back, NCSP_RECTPIECE_YRADIUS, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_BORDERSIZE, 5);
    _c(back)->setProperty (back, NCSP_RECTPIECE_XRADIUS, 5);

    /* add image.*/
    //LoadResource(HDC_SCREEN, RES_TYPE_IMAGE, "new2.png");
    static BITMAP bmp[3];
    LoadBitmap(HDC_SCREEN, &bmp[0], "res/sample-activity/news.png");
    LoadBitmap(HDC_SCREEN, &bmp[1], "res/sample-activity/file.png");
    LoadBitmap(HDC_SCREEN, &bmp[2], "res/sample-activity/internet.png");
    rc.right  = bmp[0].bmWidth;
    rc.bottom = bmp[0].bmHeight;
    _c(ct4)->setRect (ct4, &rc);
    _c(ct4)->setProperty (ct4, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[0]);
    rc.right  = bmp[1].bmWidth;
    rc.bottom = bmp[1].bmHeight;
    _c(ct5)->setRect (ct5, &rc);
    _c(ct5)->setProperty (ct5, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[1]);
    rc.right  = bmp[2].bmWidth;
    rc.bottom = bmp[2].bmHeight;
    _c(ct6)->setRect (ct6, &rc);
    _c(ct6)->setProperty (ct6, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp[2]);
    {
        int event_ids[]=
        {
            NCSN_ABP_CLICKED,
            0
        };
        ncsAddEventListeners((mObject*)ct6, (mObject*)ctnr,
                (NCS_CB_ONPIECEEVENT)onLeftButtonClick, event_ids);
    }

    _c(ct1)->setProperty (ct1, NCSP_LABELPIECE_LABEL, (DWORD)"hello");
    _c(ct2)->setProperty (ct2, NCSP_LABELPIECE_LABEL, (DWORD)"world");
    
    _c(body)->addContent(body, back, 5, 5);
    _c(body)->addContent(body, ct1, 10, 20);
    _c(body)->addContent(body, ct2, 120, 20);
    _c(body)->addContent(body, ct4, 105, 105);
    _c(body)->addContent(body, ct5, 165, 105);
    _c(body)->addContent(body, ct6, 235, 105);
    _c(body)->addContent(body, ct7, 255, 295);

    return TRUE;
}

static BOOL mymain_onClose (mWidget* _this, int message)
{
        return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
        {MSG_CREATE, reinterpret_cast<void*>(mymain_onCreate)},
        {MSG_CLOSE, reinterpret_cast<void*>(mymain_onClose)},
        {0, NULL}
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_NONE,
        "Window Template",
        NULL,
        NULL,
        mymain_handlers,
        NULL,
        0,
        0,
        0, 0,
};


Ncs4padActivity::Ncs4padActivity() :
    NCSActivity(&mymain_templ)
{ 
    m_style = STYLE_PUSH;
}

Ncs4padActivity::~Ncs4padActivity() {
}

