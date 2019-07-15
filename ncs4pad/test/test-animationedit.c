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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>

#include "global.h"
#include "mpieceitem.h"
#include "mitemiterator.h"
#include "mabstractitemmanager.h"
#include "mlistitemmanager.h"
#include "mpanelpiece.h"
#include "mtransroundpiece.h"
#include "manimationeditpiece.h"
#include "mcontainerctrl.h"

#define IDC_ANIMATIONEDIT 100
static mAnimationEditPiece *mypiece;
static BOOL mainwnd_onMouseEvent (mWidget* _this, int message, int x, int y, DWORD keyStatus);
/* test tailDelete, append, getContent */
static  BOOL mainwnd_onChar (mWidget* _this, int message, int ch, DWORD key_flags)
{
    char tmpstr[128] = {0};

    mContainerCtrl *self = (mContainerCtrl*)ncsGetChildObj(_this->hwnd, IDC_ANIMATIONEDIT);
    memset (tmpstr, 0, sizeof(tmpstr));

    tmpstr[0] = ch;
    if (ch == 127) {
        //BS
        _c(mypiece)->tailDelete (self->hwnd,mypiece, 1); 
    } else {
        _c(mypiece)->append (self->hwnd,mypiece, tmpstr);
    }

    memset (tmpstr, 0, sizeof(tmpstr));
    _c(mypiece)->getContent(mypiece, tmpstr, sizeof(tmpstr));
    return TRUE;
}
static void willdrawit(MGEFF_ANIMATION handle, void *target, intptr_t id, void *value)
{
    mContainerCtrl *ctnr = (mContainerCtrl *)target;
    UpdateWindow(ctnr->hwnd, TRUE);
}
static BOOL mainwnd_onMouseEvent (mWidget* _this, int message, int x, int y, DWORD keyStatus)
{
    mContainerCtrl *ctrl = (mContainerCtrl*)ncsGetChildObj(_this->hwnd, IDC_ANIMATIONEDIT);
    mPanelPiece *self = (mPanelPiece *)ctrl->body;
    MGEFF_ANIMATION willdo;
    MGEFF_ANIMATION group;
    group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    willdo = mGEffAnimationCreate(ctrl, willdrawit, 0, MGEFF_INT);
    mGEffAnimationSetDuration(willdo, 1000+100);

    MGEFF_ANIMATION animation = _c(self)->rotatePieceWithAnimation(self,(mHotPiece *)mypiece,
            180,1,0,0,1000,Linear);

    mGEffAnimationAddToGroup(group, willdo);
    mGEffAnimationAddToGroup(group, animation);
    
    mGEffAnimationSyncRun (group);
    mGEffAnimationDelete (group);
    return TRUE;
}
static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mContainerCtrl *containerctrl;
    mPanelPiece *containerpiece;

    mHotPiece *backpiece;
    mHotPiece *manimationeditpiece;

    RECT contentrc;

    DWORD gradient[2] = {0xffcfd571,0xff7c9731};
    PLOGFONT font = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            42, 0);

    containerctrl = (mContainerCtrl*)ncsGetChildObj(self->hwnd, IDC_ANIMATIONEDIT);
    if (containerctrl == NULL)
    {
        printf("container create failure!!!!\n");
        return FALSE;
    }
    containerpiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    SetRect(&contentrc,0,0,ACTIVITY_W,74);
    _c(containerpiece)->setRect (containerpiece, &contentrc);
    _c(containerctrl)->setBody(containerctrl, (mHotPiece*)containerpiece);

    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &contentrc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_CORNERFLAG,0);

    _c(backpiece)->setProperty (backpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    _c((mShapeTransRoundPiece *)backpiece)->setGradientFillColors(
            (mShapeTransRoundPiece *)backpiece,gradient,2);

    _c(containerpiece)->addContent(containerpiece, backpiece, 0, 0);

    manimationeditpiece = (mHotPiece*)NEWPIECE(mAnimationEditPiece);
    _c(manimationeditpiece)->setRect (manimationeditpiece, &contentrc);
    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_FONT,(DWORD)font);
    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_TEXTCOLOR,0x7f2b3d01);
    _c(manimationeditpiece)->setProperty (
            manimationeditpiece, NCSP_ANIMATIONEDITPIECE_TEXTSHADOWCOLOR,0x7fffffff);
    _c(containerpiece)->addContent(containerpiece, manimationeditpiece, 0, 0);

    _c((mAnimationEditPiece *)manimationeditpiece)->setContent(
            containerctrl->hwnd,(mAnimationEditPiece *)manimationeditpiece,"123-4567-7890",TEXT_ALIGN_CENTER);

    mypiece = (mAnimationEditPiece *)manimationeditpiece;
    return TRUE;
}

static NCS_EVENT_HANDLER mainwnd_handlers[] = {
    {MSG_CREATE, mymain_onCreate},
    {MSG_CHAR, mainwnd_onChar},
    {MSG_LBUTTONDOWN, mainwnd_onMouseEvent},
    {0, NULL }
};

/*定义控件*/
static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
   {
       NCSCTRL_CONTAINERCTRL,
       IDC_ANIMATIONEDIT,
       0, 20, ACTIVITY_W, 74,
       WS_VISIBLE,
       WS_EX_NONE,
       "hahah",
       NULL,
       NULL,
       NULL, NULL, 0, 0
   },
};

/*定义窗口*/
static NCS_MNWND_TEMPLATE mainwnd_tmpl = {
    NCSCTRL_DIALOGBOX,
    1,
    0, 0, ACTIVITY_W, ACTIVITY_H,
    WS_VISIBLE,
    WS_EX_NONE,
    "AnimationEdit Demo",
    NULL,
    NULL,
    mainwnd_handlers,
    _ctrl_tmpl,
    sizeof(_ctrl_tmpl)/sizeof(NCS_WND_TEMPLATE),
    0,
    0, 0,
};


#include "register.h"
int MiniGUIMain(int argc, const char* argv[])
{
    ncsInitialize();

    REGISTER_NCS();

    mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect
                (&mainwnd_tmpl, HWND_DESKTOP);
    mGEffInit();
    _c(mydlg)->doModal(mydlg, TRUE);
    MainWindowThreadCleanup(mydlg->hwnd);

    MGNCS_UNREG_COMPONENT(mContainerCtrl);
    ncsUninitialize ();

    return 0;
}
