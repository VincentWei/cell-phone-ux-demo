#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mcontainerctrl.h"
#include "mpanelpiece.h"
#include "mscrollviewpiece.h"
#include "mphonestaticrdrpiece.h"
#include "mphoneseparatorpiece.h"
#include "mtransroundpiece.h"

#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "manimationeditpiece.h"
#include "mbuttonpanelpiece.h" 
#include "PhoneCallActivity.hh"

static PBITMAP desktopbmp;
REGISTER_ACTIVITY(PhoneCallActivity);


static BOOL main_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *ctnr_func;
    mContainerCtrl *ctnr_title;

    PhoneCallActivity* act = (PhoneCallActivity*)
                Activity::getActivityFromHWND (self->hwnd);

    RECT rc = {N_FUNC_AREA_X, N_FUNC_AREA_Y,
        N_FUNC_AREA_X + CALL_AREA_W, N_FUNC_AREA_Y + CALL_AREA_H};

#ifdef ACTIVITY_NO_SECONDARYDC 
    act->setHWNDSecondaryDC(self->hwnd);
#endif
    ctnr_title = (mContainerCtrl*)ncsGetChildObj(self->hwnd, ID_ANIMATION_EDIT);
    act->titleCreate(ctnr_title);

    ctnr_func = (mContainerCtrl*)act->functionCreate(self->hwnd,rc);
    act->keypadCreate(ctnr_func, rc);

    rc.left = 0;
    rc.right = SCREEN_W;
    rc.top = ACTIVITY_H - CALL_END_CALL_H;  
    rc.bottom = ACTIVITY_H;  
    act->endCallButtonCreate(self->hwnd,rc);
    return TRUE;
}


static void main_onClose(mWidget* self, int message)
{
    return;
}

static void main_onDestroy(mWidget* self, int message)
{
    PhoneCallActivity* act = (PhoneCallActivity*)
                Activity::getActivityFromHWND (self->hwnd);
#ifdef DEBUG
    fprintf(stderr, "%s:destroy.\n", __FUNCTION__);
#endif
#ifdef ACTIVITY_NO_SECONDARYDC 
    act->deleteHWNDSecondaryDC();
#endif
    return;
}

static void main_onPaint (mWidget* _this, HDC hdc, const PCLIPRGN clipRgn)
{
    FillBoxWithBitmap(hdc, 0, 0, ACTIVITY_W, ACTIVITY_H, desktopbmp);
}

static BOOL main_userApp(mMainWnd* self, int msg, DWORD wparam, DWORD lparam)
{
    mContainerCtrl* ctnr_title = (mContainerCtrl*)ncsGetChildObj(self->hwnd, ID_ANIMATION_EDIT);
    PhoneCallActivity* act = (PhoneCallActivity*)
                Activity::getActivityFromHWND (self->hwnd);

    mAnimationEditPiece *editpiece = act->getTitleEditPiece();

    Intent *intent = (Intent *)wparam;
    if (intent != NULL)
    {
        std::string number = boost::any_cast<std::string>(intent->getString("number"));

        memset(act->getPhoneNumber(),0x00,PHONE_NUMBER_LEN);
        strcpy(act->getPhoneNumber(),number.c_str());
    }
    _c(editpiece)->setContent (ctnr_title->hwnd,editpiece,act->getPhoneNumber(),TEXT_ALIGN_CENTER);
    return TRUE;
}

static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}
static NCS_EVENT_HANDLER main_handlers [] = {
    {MSG_CREATE, reinterpret_cast<void*>(main_onCreate)},
    {MSG_USER_APP_DATA, reinterpret_cast<void*>(main_userApp)},
    {MSG_CLOSE, reinterpret_cast<void*>(main_onClose)},
    {MSG_DESTROY, reinterpret_cast<void*>(main_onDestroy)},
    {MSG_PAINT, reinterpret_cast<void*>(main_onPaint)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
    {0, NULL}
};

static NCS_WND_TEMPLATE _ctrl_templ[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        ID_ANIMATION_EDIT,
        0, 0, CALL_TITLE_W, CALL_TITLE_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL, NULL, 0, 0
    },
};

static NCS_MNWND_TEMPLATE mymain_templ = {
        NCSCTRL_DIALOGBOX, 
        1,
        ACTIVITY_X, ACTIVITY_Y, ACTIVITY_W, ACTIVITY_H,
        WS_NONE,
        WS_EX_NONE,
        "Phone Call",
        NULL,
        NULL,
        main_handlers,
        _ctrl_templ,
        sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
        0,
        0, 0,
};

PhoneCallActivity::PhoneCallActivity() :
    NCSActivity( &mymain_templ)
{ 
    int i;
    MGNCS_INIT_CLASS(mPhoneSeparatorPiece);
    MGNCS_INIT_CLASS(mPhoneStaticRDRPiece);
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_INIT_CLASS(mAnimationEditPiece);
    MGNCS_INIT_CLASS(mLayoutManager);
    MGNCS_INIT_CLASS(mCenterHBox);
    MGNCS_INIT_CLASS(mButtonPanelPiece);

    initResource();
    
    memset(m_phoneNumber,0x00,PHONE_NUMBER_LEN);
    strcpy(m_phoneNumber,"138-9999-8888");

    for (i = 0; i < ANIMATION_MAX; i++) {
        m_funcContainPiece[i].func_subcontain_piece = NULL;
        m_funcContainPiece[i].func_subimage_piece = NULL;
        m_funcContainPiece[i].func_subgradient_piece = NULL;
        m_funcContainPiece[i].animation_handle = NULL;
    }

    for (i = 0; i < CNTR_MAX; i++) {
        m_ctrnPiece[i] = NULL;
    }

    for (i = 0; i < BTN_MAX; i++) {
        m_btnPiece[i] = NULL;
    }

    m_animationEditPiece = NULL;
    desktopbmp = (PBITMAP) Load32Resource(DESKTOP_IMG_PATH, RES_TYPE_IMAGE, 0);
    //m_style = STYLE_NAVBAR;
}

PhoneCallActivity::~PhoneCallActivity(void) 
{
    int i;

    releaseResource();

    for (i = 0; i < ANIMATION_MAX; i++) {
        if (m_funcContainPiece[i].animation_handle != NULL) {
            mGEffAnimationStop(m_funcContainPiece[i].animation_handle);
            m_funcContainPiece[i].animation_handle = NULL;
        }
    }

    UnregisterRes(DESKTOP_IMG_PATH);
}


int PhoneCallActivity::onPause()
{
    fprintf(stderr, "%s:%s:%d, delete secondaryDC\n", __FILE__, __FUNCTION__, __LINE__);

    deleteHWNDSecondaryDC();

    return 0;
}

int PhoneCallActivity::onResume()
{
    fprintf(stderr, "%s:%s:%d, set secondaryDC\n", __FILE__, __FUNCTION__, __LINE__);

    setHWNDSecondaryDC(this->hwnd());

    return 0;
}


