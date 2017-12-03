
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

#include "animationedit.h"

#define IDC_ANIMATIONEDIT 100
#define IDC_ANIMATIONEDIT2 101
/* test tailDelete, append, getContent */
static  BOOL mainwnd_onChar (mWidget* _this, int message, int ch, DWORD key_flags)
{
    char tmpstr[128] = {0};

    mAnimationEdit *self = (mAnimationEdit*)ncsGetChildObj(_this->hwnd, IDC_ANIMATIONEDIT);
    mAnimationEdit *self2 = (mAnimationEdit*)ncsGetChildObj(_this->hwnd, IDC_ANIMATIONEDIT2);
    memset (tmpstr, 0, sizeof(tmpstr));

    tmpstr[0] = ch;
    if (ch == 127) {
        //BS
        _c(self)->tailDelete (self, 1); 
        _c(self2)->tailDelete (self2, 1); 
    } else {
        _c(self)->append (self, tmpstr);
        _c(self2)->append (self2, tmpstr);
    }

    memset (tmpstr, 0, sizeof(tmpstr));
    _c(self)->getContent(self, tmpstr, sizeof(tmpstr));
    printf ("Current text: %s\n", tmpstr);

    return TRUE;
}


/*测试setContent*/
const char *str[] = {
    "AAAAAAAAAA",
    "BBBBBBBBBB",
    "CCCCCCC",
    "DDDDDDDDDDD",
    NULL
};
int current_string = 0;
static BOOL mainwnd_onMouseEvent (mWidget* _this, int message, int x, int y, DWORD keyStatus)
{
    mAnimationEdit *self = (mAnimationEdit*)ncsGetChildObj(_this->hwnd, IDC_ANIMATIONEDIT);
    _c(self)->setContent(self, str[current_string], MGEFF_MINOR_flip);

    self = (mAnimationEdit*)ncsGetChildObj(_this->hwnd, IDC_ANIMATIONEDIT2);
    _c(self)->setContent(self, str[current_string], MGEFF_MINOR_flip);

    current_string ++;

    if (str[current_string] == NULL) {
        current_string = 0;
    }

    return TRUE;
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
    mAnimationEdit *animationEdit;
  
    {
        animationEdit = (mAnimationEdit*)ncsGetChildObj (self->hwnd, IDC_ANIMATIONEDIT);
        ARGB colors[]={0xffff0000, 0xff00ff00, 0xff003fff, 0xff0000ff};
        _c((mAnimationEdit*)animationEdit)->setGradientFillColors((mAnimationEdit*)animationEdit, colors, 2);
    }

    {
        animationEdit = (mAnimationEdit*)ncsGetChildObj (self->hwnd, IDC_ANIMATIONEDIT2);
        HBRUSH brush = _c(animationEdit)->getBKBrush (animationEdit);
        if (brush == MP_INV_HANDLE) {
            printf ("invalid brush !\n");
            return FALSE;
        }

        MGPlusLinearGradientBrushAddColor (brush, 0xffcfd571, 0.0f);
        MGPlusLinearGradientBrushAddColor (brush, 0xff7c9731, 0.8f);
        MGPlusLinearGradientBrushAddColor (brush, 0xff354605, 1.0f);
    }

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
       NCSCTRL_ANIMATIONEDIT,
       IDC_ANIMATIONEDIT,
       20, 20, 200, 50,
       WS_VISIBLE|NCSS_ANIMATIONEDIT_BKGRADIENT,
       WS_EX_NONE,
       "",
       NULL,
       NULL,
       NULL, NULL, 0, 0
   },
   {
       NCSCTRL_ANIMATIONEDIT,
       IDC_ANIMATIONEDIT2,
       20, 150, 200, 50,
       WS_VISIBLE|NCSS_ANIMATIONEDIT_BKGRADIENT,
       WS_EX_NONE,
       "",
       NULL,
       NULL,
       NULL, NULL, 0, 0
   }
};

/*定义窗口*/
static NCS_MNWND_TEMPLATE mainwnd_tmpl = {
    NCSCTRL_DIALOGBOX,
    1,
    0, 0, 240, 320,
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


int MiniGUIMain(int argc, const char* argv[])
{
    ncsInitialize();
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    MGNCS_REGISTER_COMPONENT(mAnimationEdit);

    mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect
                (&mainwnd_tmpl, HWND_DESKTOP);
    mGEffInit();

    _c(mydlg)->doModal(mydlg, TRUE);
    MainWindowThreadCleanup(mydlg->hwnd);
    MGNCS_UNREG_COMPONENT(mAnimationEdit);
    ncsUninitialize ();

    return 0;
}
