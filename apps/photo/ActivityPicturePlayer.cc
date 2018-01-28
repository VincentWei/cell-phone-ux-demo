#include <cassert>
#include <sstream>
#include "ActivityPicturePlayer.hh"
#include "mlayout_manager.h"
#include "mcenterhbox.h"
#include "mpanelpiece.h"
#include "mbuttonpanelpiece.h"
#include "mtextpiece.h"

REGISTER_ACTIVITY(ActivityPicturePlayer);

#define IDC_START   100
#define IDC_CC      (IDC_START+0)
#define IDC_TIMER   (IDC_START+1)
#define IDC_ANIMATIONEDIT   (IDC_START+2)

#define AT_FULL_AREA(x, y)  ((y) >= PP_TITLE_H && (y) <= PP_CTRL_Y)
#define thisActivity(hwnd)  ((ActivityPicturePlayer*)Activity::getActivityFromHWND(hwnd))

static const char *gPic[] ={
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

static DWORD title_gradient_color[] = {
    0xccd0d571,0xccb5c363,0xcc9eb23a,0xcc7f9e2c,0xcc597311,0xcc597311
};
static float title_gradient_scale[] = {
    0.0,0.49,0.50,0.96,0.97,1.0
};

static DWORD control_gradient_color[] = {
    0xcc0f1700,0xcc434343,0xcc2d2d2d,0xcc151515,0xcc010101,0xcc000000
};
static float control_gradient_scale[] = {
    0.0,0.01,0.04,0.49,0.51,1.0
};
static void photo_set_gradient_color(mShapeTransRoundPiece *backpiece,DWORD *colors,float *scales,int num)
{
    HBRUSH brush;
    brush = _c(backpiece)->getBrush(backpiece);
    MGPlusSetLinearGradientBrushColorsEx(brush,colors,num,scales);
}

static int enterFull(mWidget *self){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    //mWidget *cc = ncsGetChildObj(self->hwnd, IDC_CC);
    //mPanelPiece *body  = (mPanelPiece*)cc->body;
    mPanelPiece* body = activity->mBody;
    mPanelPiece *title = (mPanelPiece*)activity->getTitle();
    mPanelPiece *ctrl  = (mPanelPiece*)activity->getCtrl();
    MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    MGEFF_ANIMATION anima1, anima2;


    if(activity->isFull())
    {
        return 0;
    }

    activity->setFull(TRUE);
    activity->m_isExiting = FALSE;

    /*_c(body)->movePiece(body, (mHotPiece*)ctrl, PP_CTRL_X, PP_H);
    PanelPiece_update((mHotPiece*)ctrl, TRUE);

    _c(body)->movePiece(body, 
            (mHotPiece*)activity->getImage(),PP_CTRL_X, 0);
    PanelPiece_update((mHotPiece*)activity->getImage(), TRUE);

    MoveWindow(self->hwnd,0,0,SCREEN_W,SCREEN_H,TRUE);
    MoveWindow(cc->hwnd,0,0,SCREEN_W,SCREEN_H,TRUE);

    anima1 = _c(body)->movePieceWithAnimation(body, (mHotPiece*)title, PP_TITLE_X, -PP_TITLE_H, 400, OutCubic);
    anima2 = _c(body)->movePieceWithAnimation(body, (mHotPiece*)ctrl,  PP_CTRL_X,  PP_CTRL_H + PP_H, 400, OutCubic);
    */

    /*_c(body)->movePiece(body, (mHotPiece*)ctrl, PP_CTRL_X, SCREEN_H-PP_CTRL_H);
    //_c(body)->movePiece(body, (mHotPiece*)activity->getImage(), PP_CTRL_X, 0);

    //MoveWindow(self->hwnd, 0, 0, SCREEN_W,SCREEN_H, TRUE);
    //MoveWindow(cc->hwnd, 0, 0, SCREEN_W, SCREEN_H, TRUE);

    PanelPiece_update((mHotPiece*)body, TRUE);*/
    
    anima1 = _c(body)->movePieceWithAnimation(body, (mHotPiece*)title, PP_TITLE_X, -PP_TITLE_H, 400, OutCubic);
    anima2 = _c(body)->movePieceWithAnimation(body, (mHotPiece*)ctrl,  PP_CTRL_X,  SCREEN_H, 400, OutCubic);

    mGEffAnimationAddToGroup(group, anima1);
    mGEffAnimationAddToGroup(group, anima2);
    _c(body)->animationSyncRunAndDelete(body, group);

    return 0;
}
static void my_finish_cb(MGEFF_ANIMATION handle)
{
    ActivityPicturePlayer *act = 
        (ActivityPicturePlayer *)mGEffAnimationGetContext(handle);

    act->setFull(FALSE);
    act->m_isExiting = FALSE;
    return; 
}
static int exitFull(mWidget *self){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    //mWidget *cc = ncsGetChildObj(self->hwnd, IDC_CC);
    //mPanelPiece *body  = (mPanelPiece*)cc->body;
    mPanelPiece *body  = activity->mBody;
    mPanelPiece *title = (mPanelPiece*)activity->getTitle();
    mPanelPiece *ctrl  = (mPanelPiece*)activity->getCtrl();
    MGEFF_ANIMATION group;
    MGEFF_ANIMATION anima1, anima2;

    if(!activity->isFull() || activity->m_isExiting)
    {
        return 0;
    }
    activity->m_isExiting = TRUE;

    /*_c(body)->movePiece(body, 
            (mHotPiece*)activity->getImage(),PP_CTRL_X, -ACTIVITY_Y);
    PanelPiece_update((mHotPiece*)activity->getImage(), TRUE);
    
    MoveWindow(cc->hwnd,0,0,SCREEN_W,SCREEN_H-ACTIVITY_Y,TRUE);
    MoveWindow(self->hwnd,0,ACTIVITY_Y,SCREEN_W,SCREEN_H-ACTIVITY_Y,TRUE);*/

    group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    anima1 = _c(body)->movePieceWithAnimation(body, (mHotPiece*)title, PP_TITLE_X, PP_TITLE_Y, 300, OutCubic);
    anima2 = _c(body)->movePieceWithAnimation(body, (mHotPiece*)ctrl,  PP_CTRL_X,  PP_CTRL_Y,  300, OutCubic);
    mGEffAnimationAddToGroup(group, anima1);
    mGEffAnimationAddToGroup(group, anima2);
    mGEffAnimationSetFinishedCb(group, my_finish_cb);
    mGEffAnimationSetContext(group,(void *)activity);
    _c(body)->animationAsyncRun(body, group,0);

    return 0;
}

//btn & timer event handlers
static BOOL onNext(mObject *self, mObject *piece,
        int event_id, DWORD param)
{
    mWidget *dialog = ncsGetParentObj(((mWidget*)self)->hwnd);
    ActivityPicturePlayer *activity = thisActivity(dialog->hwnd);
    mImagePiece *image = (mImagePiece*)activity->getImage();
    int index = activity->getIndex();
    if (event_id == NCSN_ABP_CLICKED) {
            ReleaseRes(Str2Key(gPic[index]));
            index = (index + 1) % TABLESIZE(gPic);
            activity->setIndex(index);
            activity->setPic((BITMAP*)Load32Resource (gPic[index], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN));
            _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)activity->getPic());
            UpdateWindow(dialog->hwnd, FALSE);
            return TRUE;
    }
    return FALSE;
}
static BOOL onPrev(mObject*self, mObject *piece,
        int event_id, DWORD param)
{
    mWidget *dialog = ncsGetParentObj(((mWidget*)self)->hwnd);
    ActivityPicturePlayer *activity = thisActivity(dialog->hwnd);
    mImagePiece *image = (mImagePiece*)activity->getImage();
    int index = activity->getIndex();
    if (event_id == NCSN_ABP_CLICKED) {
            ReleaseRes(Str2Key(gPic[index]));
            index = (index) ? (index - 1) : (TABLESIZE(gPic) - 1);  //set tail index when index is equal to 0
            activity->setIndex(index);
            activity->setPic((BITMAP*)Load32Resource (gPic[index], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN));
            _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)activity->getPic());
            UpdateWindow(dialog->hwnd, FALSE);
            return TRUE;
    }
    return FALSE;
}

static BOOL onPlay(mObject *self, mObject *piece,
        int event_id, DWORD param)
{
    mWidget *dialog = ncsGetParentObj(((mWidget*)self)->hwnd);
    mTimer *timer = (mTimer *)ncsGetChildObj(dialog->hwnd, IDC_TIMER);
    if (event_id == NCSN_ABP_CLICKED) {
        enterFull(dialog);
        _c(timer)->start(timer);
        return TRUE;
    }

    return FALSE;
}
#if 0
static int onDel(mHotPiece *self, int message, WPARAM wparam, LPARAM lparam, mObject *owner){
    mWidget *dialog = ncsGetParentObj(((mWidget*)owner)->hwnd);
    ActivityPicturePlayer *activity = thisActivity(dialog->hwnd);
    mShapeTransRoundPiece *bk = (mShapeTransRoundPiece*)activity->getDelBk();
    ARGB highLight[] = {0xa0e2edf9, 0xa0a1abb7, 0xa0e2edf9};
    ARGB normal[] = {0xffffffff, 0xffd4d4d4};

    switch(message){
        case MSG_LBUTTONDOWN:
            _c(bk)->setGradientFillColors(bk, highLight, TABLESIZE(highLight));
            UpdateWindow(dialog->hwnd, FALSE);
            break;
        case MSG_MOUSEMOVE:
            break;
        case MSG_LBUTTONUP:
            _c(bk)->setGradientFillColors(bk, normal, TABLESIZE(normal));
            UpdateWindow(dialog->hwnd, FALSE);
            break;
    }

    return 0;
}
#endif
static BOOL onExit(mObject *self, mObject *piece,
        int event_id, DWORD param)
{
    mWidget *dialog = ncsGetParentObj(((mWidget*)self)->hwnd);
    if (event_id == NCSN_ABP_CLICKED) {
        ActivityStack::singleton()->pop();
        SendNotifyMessage (dialog->hwnd, MSG_CLOSE, 0, 0);
        return TRUE;
    }
    return FALSE;
}
static HDC createDCByFile(HDC hdc, BITMAP *bmp, int w, int h){
    HDC dc;

    if(w == 0)
        w = bmp->bmWidth;
    if(h == 0)
        h = bmp->bmHeight;
    h += ACTIVITY_Y;
#if 1
    dc = CreateCompatibleDCEx(hdc, w, h);
    FillBoxWithBitmap(dc, 0, 0, w, h, bmp);
#else
    dc = CreateMemDCFromBitmap(Get32MemDC(), bmp);
#endif
    return dc;
}
static int fillAnimation(HDC s1, HDC s2, HDC d, const char *name, int duration){
    unsigned int key = mGEffStr2Key(name);
    MGEFF_EFFECTOR eff = mGEffEffectorCreate(key);
    MGEFF_SOURCE ms1 = mGEffCreateSource(s1);
    MGEFF_SOURCE ms2 = mGEffCreateSource(s2);
    MGEFF_SINK sink = mGEffCreateHDCSink(d);
    MGEFF_ANIMATION handle;

    mGEffEffectorAppendSource(eff, ms1);
    mGEffEffectorAppendSource(eff, ms2);
    mGEffSetBufferSink(sink, s1);
    mGEffEffectorSetSink(eff, sink);

    handle = mGEffAnimationCreateWithEffector(eff);
    mGEffAnimationSetDuration(handle, duration);
    mGEffAnimationSyncRun(handle);
    mGEffAnimationDelete(handle);

    mGEffEffectorDelete(eff);

    return 0;
}
static int onNextWithEffector(mWidget *self){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    mImagePiece *image = (mImagePiece*)activity->getImage();
    int index = activity->getIndex();
    HDC hdc = GetDC(self->hwnd);
    HDC s1,s2;

    s1 = createDCByFile(hdc, activity->getPic(), PP_W, PP_H);
    ReleaseRes(Str2Key(gPic[index]));

    index = (index + 1) % TABLESIZE(gPic);
    activity->setIndex(index);
    activity->setPic((BITMAP*)Load32Resource (gPic[index], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN));
    _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)activity->getPic());
    s2 = createDCByFile(hdc, activity->getPic(), PP_W, PP_H);
    fillAnimation(s1, s2, hdc, MGEFF_MINOR_scroll, 1000);
    DeleteMemDC(s1);
    DeleteMemDC(s2);

    ReleaseDC(hdc);
    return 0;
}
static int onPrevWithEffector(mWidget *self){
    ActivityPicturePlayer *activity = (ActivityPicturePlayer*)Activity::getActivityFromHWND(self->hwnd);
    mImagePiece *image = (mImagePiece*)activity->getImage();
    int index = activity->getIndex();
    HDC hdc = GetDC(self->hwnd);
    HDC s1,s2;

    s1 = createDCByFile(hdc, activity->getPic(), PP_W, PP_H);
    ReleaseRes(Str2Key(gPic[index]));

    index = (index) ? (index - 1) : (TABLESIZE(gPic) - 1);
    activity->setIndex(index);
    activity->setPic((BITMAP*)Load32Resource (gPic[index], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN));
    _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)activity->getPic());
    s2 = createDCByFile(hdc, activity->getPic(), PP_W, PP_H);
    fillAnimation(s1, s2, hdc, MGEFF_MINOR_scroll, 500);
    DeleteMemDC(s1);
    DeleteMemDC(s2);

    ReleaseDC(hdc);
    return 0;
}
static BOOL timerHandler(mWidget *listener, mWidget *sender, int id, DWORD total_count){
    onNextWithEffector(listener);
    return 0;
}
static int mainwndTimerListener(mWidget *self){
    mTimer *timer;

    timer = (mTimer *)ncsGetChildObj(self->hwnd, IDC_TIMER);
    if(timer)
        ncsAddEventListener((mObject *)timer,
                (mObject *)self,
                (NCS_CB_ONPIECEEVENT)timerHandler,
                MSG_TIMER);
    else{
        fprintf(stderr, "timer obj is invalidate\n");
        return -1;
    }

    return 0;
}

//mainwnd event handlers
static mHotPiece* createBody(mWidget* self, mWidget *ctnr){
    RECT rc;
    mPanelPiece *toppiece;
    mHotPiece *body;
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);

    GetWindowRect(ctnr->hwnd, &rc);
    //rc.bottom += ACTIVITY_Y;
    toppiece = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(toppiece)->setRect(toppiece, &rc);

    body = (mHotPiece*)NEWPIECE(mPanelPiece);
    _c(body)->setRect(body, &rc);

    //_c((mContainerCtrl*)self)->setBody((mContainerCtrl*)self, body);
    _c((mContainerCtrl*)ctnr)->setBody((mContainerCtrl*)ctnr, (mHotPiece*)toppiece);
    _c(toppiece)->addContent(toppiece, body, 0, 0);

    activity->mBody = (mPanelPiece*)body; 
    return body;
    //return (mHotPiece*)toppiece;
}
static mHotPiece* createBkImage(mHotPiece *body, BITMAP *pic){
    RECT rc;
    mHotPiece *image;

    _c(body)->getRect(body, &rc);
    //rc.bottom += ACTIVITY_Y;
    image = (mHotPiece*)NEWPIECE(mImagePiece);
    _c(image)->setRect(image, &rc);
    _c((mImagePiece*)image)->setProperty((mImagePiece*)image, NCSP_IMAGEPIECE_IMAGE, (DWORD)pic);
    _c((mImagePiece*)image)->setProperty((mImagePiece*)image, 
            NCSP_IMAGEPIECE_DRAWMODE, (DWORD)NCS_DM_SCALED);

    //_c((mPanelPiece*)body)->addContent((mPanelPiece*)body, image, 0, -ACTIVITY_Y);
    _c((mPanelPiece*)body)->addContent((mPanelPiece*)body, image, 0, 0);

    return image;
}
#define BTN_IMAGE       0x01
#define BTN_LABEL       0x02
#define BTN_TRANROUND   0x04
#if 0
typedef struct PICPLAYERBTN{
    RECT *prc;
    DWORD *pargb;
    float *scales;
    int  ncolor;
    int radius;
    BITMAP *bmp;
    const char *str;
    NCS_PIECE_EVENT_HANDLER handler; 
    mHotPiece *btn;
    mHotPiece *bk;
    mHotPiece *image;
    mHotPiece *label;
}PICPLAYERBTN, *PPICPLAYERBTN;
static int createBtn(mHotPiece *ctrl, int x, int y, PICPLAYERBTN *data, int style){
    mHotPiece *btn;

    //add btn container
    btn = (mHotPiece*)NEWPIECE(mPanelPiece);
    _c(btn)->setRect(btn, data->prc);
    _c((mPanelPiece*)ctrl)->addContent((mPanelPiece*)ctrl,
        btn, x, y);
    _c((mPanelPiece*)btn)->appendEventHandler((mPanelPiece*)btn, MSG_LBUTTONUP, data->handler);
    _c((mPanelPiece*)btn)->appendEventHandler((mPanelPiece*)btn, MSG_MOUSEMOVE, data->handler);
    _c((mPanelPiece*)btn)->appendEventHandler((mPanelPiece*)btn, MSG_LBUTTONDOWN, data->handler);
    data->btn = btn;

    //add btn background
    if(style & BTN_TRANROUND){
        mShapeTransRoundPiece *bk;
        bk = NEWPIECE(mShapeTransRoundPiece);
        _c(bk)->setRect(bk, data->prc);
        _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, data->radius);
        _c(bk)->setProperty(bk, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        photo_set_gradient_color(bk,data->pargb,data->scales,data->ncolor);
        _c((mPanelPiece*)btn)->addContent((mPanelPiece*)btn, (mHotPiece*)bk, 0, 0);
        data->bk = (mHotPiece*)bk;
    }

    //add btn image
    if(style & BTN_IMAGE){
        mImagePiece *image;
        image = NEWPIECE(mImagePiece);
        _c(image)->setRect(image, data->prc);
        _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)data->bmp); //fix
        _c((mPanelPiece*)btn)->addContent((mPanelPiece*)btn, (mHotPiece*)image, 0, 0);
        data->image = (mHotPiece*)image;
    }

    //add label
    if(style & BTN_LABEL){
        mLabelPiece *label;
        label = NEWPIECE(mLabelPiece);
        _c(label)->setRect(label, data->prc);
        _c(label)->setProperty(label, NCSP_LABELPIECE_LABEL, (DWORD)data->str);
        _c((mPanelPiece*)btn)->addContent((mPanelPiece*)btn, (mHotPiece*)label, 0, 0);
        data->label = (mHotPiece *)label;
    }

    return 0;
}
#endif

static mButtonPanelPiece* createBtn(mContainerCtrl *ctnr, mPanelPiece *panel, RECT *prc,
        ARGB *normalColors, float *normalPos, int normalNum,
        ARGB *pushedColors, float *pushedPos, int pushedNum,
        BITMAP *pbmp, char *str, PLOGFONT font,unsigned short radius,
        int event_id, NCS_CB_ONPIECEEVENT handler)
{
    RECT rc;
    mButtonPanelPiece *btn = NEWPIECE(mButtonPanelPiece);
    mShapeTransRoundPiece *bkgnd = _c(btn)->getBkgndPiece(btn);

    SetRect(&rc, 0, 0, RECTWP(prc), RECTHP(prc));

    if (pbmp != NULL) {
        mImagePiece *imgPiece = NEWPIECE(mImagePiece);
        _c(imgPiece)->setRect(imgPiece, &rc);
        _c(imgPiece)->setProperty(imgPiece, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbmp);
        _c(btn)->addContent(btn, (mHotPiece*)imgPiece, 0, 0);
    }

    if (str != NULL) {
        mHotPiece *label = (mHotPiece*)NEWPIECE(mTextPiece);
        _c(label)->setRect(label, &rc);
        _c(label)->setProperty(label, NCSP_LABELPIECE_LABEL,(DWORD)str);
        _c(label)->setProperty(label, NCSP_TEXTPIECE_LOGFONT,(DWORD)font);
        _c(label)->setProperty(label, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)PP_TITLE_TEXT_COLOR);
        _c(btn)->addContent(btn, (mHotPiece*)label, 5, 0);
    }

    _c(btn)->setRect(btn, &rc);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_RADIUS, radius);

    _c(btn)->setGradientBackgroundColor(btn,
            normalColors, normalPos, normalNum,
            pushedColors, pushedPos, pushedNum);
    _c(btn)->setStates(btn);

    ncsAddEventListener((mObject*)btn, (mObject*)ctnr, handler, event_id);

    if (prc->left > 0 || prc->top > 0)
        _c(panel)->addContent(panel, (mHotPiece*)btn, prc->left, prc->top);
    else
        _c(panel)->addContentToLayout(panel, (mHotPiece*)btn);

    return btn;
}

static mHotPiece* createTitleBar(mWidget *self,mContainerCtrl*ctnr,mHotPiece *body){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    mPanelPiece *containerpiece = (mPanelPiece*)body;
    mPanelPiece *title;

    mHotPiece *backpiece;
    mHotPiece *mtextpiece;

    RECT contentrc;

    ARGB normalColors[] = {0xffffffff, 0xffd1d1d1};
    ARGB pushedColors[] = {0xa0e2edf9, 0xa0a1abb7, 0xa0e2edf9};

    SetRect(&contentrc,0,0,PP_TITLE_W, PP_TITLE_H);
    title = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(title)->setRect(title, &contentrc);

    _c(containerpiece)->addContent(containerpiece, (mHotPiece*)title, PP_TITLE_X, PP_TITLE_Y);
    _c(containerpiece)->setPieceAlpha(containerpiece, (mHotPiece*)title, 0xe0);

    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &contentrc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_CORNERFLAG,0);

    _c(backpiece)->setProperty (backpiece,
            NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);

    photo_set_gradient_color((mShapeTransRoundPiece *)backpiece,
            title_gradient_color,title_gradient_scale,TABLESIZE(title_gradient_color));
    _c(title)->addContent(title, backpiece, 0, 0);

    mtextpiece = (mHotPiece*)NEWPIECE(mTextPiece);
    _c(mtextpiece)->setRect (mtextpiece, &contentrc);
    _c(mtextpiece)->setProperty(mtextpiece, NCSP_LABELPIECE_LABEL,(DWORD)"Picture");
    _c(mtextpiece)->setProperty(mtextpiece, NCSP_TEXTPIECE_LOGFONT,(DWORD)activity->getTitleFont());
    _c(mtextpiece)->setProperty(mtextpiece, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)PP_TITLE_TEXT_COLOR);
    _c(mtextpiece)->setProperty(mtextpiece, NCSP_TEXTPIECE_TEXTSHADOWCOLOR,
            (DWORD)PP_TITLE_TEXT_SHADOW_COLOR);
    _c(title)->addContent(title, (mHotPiece*)mtextpiece,0,0);

    SetRect(&contentrc, PP_EXIT_X, PP_EXIT_Y, PP_EXIT_X + PP_DEL_W, PP_EXIT_Y + PP_DEL_H);

    mButtonPanelPiece* btn = createBtn(ctnr, title, &contentrc,
            normalColors, NULL, TABLESIZE(normalColors),
            pushedColors, NULL, TABLESIZE(pushedColors),
            NULL, (char *)"Return",activity->getPlayFont(), 4, NCSN_ABP_CLICKED, onExit);
    mShapeTransRoundPiece *bkgnd = _c(btn)->getBkgndPiece(btn);
    ARGB border_colors[] = {0xffaaaaaa, 0xff888888};
    float border_pos[] = {0.0, 1.0};
    _c(btn)->setGradientBorderColor(btn, border_colors, border_pos, TABLESIZE(border_pos));
    _c(btn)->setStates(btn);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_SHARPFLAG, TRANROUND_SHARPFLAG_LEFT);
    _c(bkgnd)->setProperty(bkgnd, NCSP_TRANROUND_BORDERSIZE, 1);

    return (mHotPiece*)title;
}

static mHotPiece* createCtrlBar(mWidget *self, mContainerCtrl *ctnr, mHotPiece *body){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    mPanelPiece *panel;
    RECT rc;
    mCenterHBox *box = NEW(mCenterHBox);
    ARGB highLight[] = {0xa0d0e4fb, 0xa0167cef, 0xa0d0e4fb};

    //add ctrl container
    panel = NEWPIECE(mPanelPiece);
    _c(panel)->setLayoutManager(panel, (mLayoutManager*)box);

    SetRect(&rc, 0, 0, PP_BTN_W, PP_BTN_H);

    //add three btn
    createBtn(ctnr, panel, &rc,
            control_gradient_color, control_gradient_scale, TABLESIZE(control_gradient_color),
            highLight,NULL,TABLESIZE(highLight),
            activity->getPrevPic(), NULL,NULL, 0, NCSN_ABP_CLICKED, onPrev);

    createBtn(ctnr, panel, &rc,
            control_gradient_color, control_gradient_scale, TABLESIZE(control_gradient_color),
            highLight, NULL, TABLESIZE(highLight),
            activity->getPlayPic(), NULL,NULL, 0, NCSN_ABP_CLICKED, onPlay);

    createBtn(ctnr, panel, &rc,
            control_gradient_color, control_gradient_scale, TABLESIZE(control_gradient_color),
            highLight, NULL, TABLESIZE(highLight),
            activity->getNextPic(), NULL,NULL, 0, NCSN_ABP_CLICKED, onNext);

     SetRect(&rc, 0, 0, PP_CTRL_W, PP_CTRL_H);
    _c(panel)->setRect(panel, &rc);
    _c((mPanelPiece*)body)->addContent((mPanelPiece*)body,
        (mHotPiece*)panel, PP_CTRL_X, PP_CTRL_Y);
    _c((mPanelPiece*)body)->setPieceAlpha((mPanelPiece*)body, (mHotPiece*)panel, 0xe0);
 
    return (mHotPiece*)panel;
}

static BOOL mainwndOnCreate (mWidget* self, DWORD add_data) {
#if 0
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    mWidget   *cc   = ncsGetChildObj(self->hwnd, IDC_CC);
    mHotPiece *body = createBody(cc);
    //add containerctrl bkimage
    activity->setImage(createBkImage(body, activity->getPic()));

    //add titlebar
    activity->setTitle(createTitleBar(self,(mContainerCtrl*)cc,body));

    //add controlbar
    activity->setCtrl(createCtrlBar(self, body));

#endif
    //add timer listener
    mainwndTimerListener(self);

    return TRUE;
}
static BOOL mainwndOnClose (mWidget* self, int message) {
    return TRUE;
}
static BOOL mainwndOnPaint(mWidget *self, HDC hdc, const RECT *pinv) {
    return TRUE;
}
static int mainwndOnKeydown(mWidget *self, int scancode, DWORD keyFlags){
    return 0;
} 
static int mainwndOnLbuttondown(mWidget *self, int message, int x, int y, DWORD keyFlags){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);

    if(AT_FULL_AREA(x, y)){
        activity->setIsDown(TRUE);
        activity->setPoint(x, y);
        activity->setDirection(ActivityPicturePlayer::MOVE_NO);
    }
    else{
    }
    return 0;
}
static int mainwndOnLbuttonup(mWidget *self, int message, int x, int y, DWORD keyFlags){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    mTimer *timer = (mTimer *)ncsGetChildObj(self->hwnd, IDC_TIMER);

    if(activity->isFull()){
        if(activity->getIsDown() && activity->getDirection() != ActivityPicturePlayer::MOVE_NO){
            if(activity->getDirection() == ActivityPicturePlayer::MOVE_LEFT)
                onPrevWithEffector(self);
            else if(activity->getDirection() == ActivityPicturePlayer::MOVE_RIGHT)
                onNextWithEffector(self);
            activity->setDirection(ActivityPicturePlayer::MOVE_NO);
        }
        else
            exitFull(self);
        _c(timer)->stop(timer);
    }
    else if(AT_FULL_AREA(x, y)){
        enterFull(self);
    }
    activity->setIsDown(FALSE);

    return 0;
}
static int mainwndOnMousemove(mWidget *self, int message, int x, int y, DWORD keyFlags){
    ActivityPicturePlayer *activity = thisActivity(self->hwnd);
    POINT *point;

    if(activity->getIsDown()){
        point = activity->getPoint();
        if(abs(point->x - x) > 10){
            activity->setDirection(
                    ((x > point->x) ? (ActivityPicturePlayer::MOVE_RIGHT) : (ActivityPicturePlayer::MOVE_LEFT)));
        }
    }

    return 0;
}

static void mainwndOnUserApp (mWidget* _this,int message, WPARAM wparam, LPARAM lparam)
{
#if 1
    ActivityPicturePlayer *activity = thisActivity(_this->hwnd);
    mContainerCtrl *cc   = (mContainerCtrl *)ncsGetChildObj(_this->hwnd, IDC_CC);
    mHotPiece *body = createBody(_this, (mWidget *)cc);
    BITMAP *pic = NULL;

    Intent *intent = (Intent *)wparam;
    if (intent != NULL)
    {
        unsigned int i;
        int index = 0; 
        const char* path = intent->getCString("index");
        for (i=0;i<TABLESIZE(gPic);i++)
        {
            if (strcmp("0",path) == 0)
            {
                index = 0;
                break;
            }
            else if (strcmp("animation.png",path) == 0)
            {
                index = TABLESIZE(gPic) - 1; 
                break;
            }
            else if (strcmp(gPic[i],path) == 0)
            {
                index = i;
                break;
            }
        }
        activity->setIndex(index);
        pic = (BITMAP *)Load32Resource (gPic[index], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
        activity->setPic(pic);
    }
    activity->setImage(createBkImage(body, pic));

    //add titlebar
    activity->setTitle(createTitleBar(_this,cc,body));
    //add controlbar
    activity->setCtrl(createCtrlBar(_this, cc, body));
#endif
    return;
}

static BOOL main_onEraseBk(mWidget *self, HDC hdc, const PRECT clip){
    return TRUE;
}

static NCS_EVENT_HANDLER mainwndHandlers[] = {
    {MSG_CREATE,      reinterpret_cast<void*>(mainwndOnCreate)},
    {MSG_CLOSE,       reinterpret_cast<void*>(mainwndOnClose)},
    {MSG_PAINT,       reinterpret_cast<void*>(mainwndOnPaint)},
    {MSG_KEYDOWN,     reinterpret_cast<void*>(mainwndOnKeydown)},
    {MSG_LBUTTONDOWN, reinterpret_cast<void*>(mainwndOnLbuttondown)},
    {MSG_LBUTTONUP,   reinterpret_cast<void*>(mainwndOnLbuttonup)},
    {MSG_MOUSEMOVE,   reinterpret_cast<void*>(mainwndOnMousemove)},
    {MSG_USER_APP_DATA, reinterpret_cast<void*>(mainwndOnUserApp)},
    {MSG_ERASEBKGND, reinterpret_cast<void*>(main_onEraseBk)},
	{0, NULL }
};

static NCS_PROP_ENTRY timerProps[] = {
    {NCSP_TIMER_INTERVAL, 300},//unit:10ms
    {0, 0},
};
static NCS_WND_TEMPLATE ctrlTmpl[] = {
    {
        NCSCTRL_TIMER,
        IDC_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timerProps, NULL, NULL, NULL, 0, 0, 
    },
    {
        NCSCTRL_CONTAINERCTRL,
        IDC_CC,
        PP_CC_X, PP_CC_Y, PP_CC_W,PP_CC_H,
        WS_VISIBLE,
        //WS_EX_TRANSPARENT,
        WS_EX_NONE,
        "cc",
        NULL, NULL, NULL, 0
    },
};
static NCS_MNWND_TEMPLATE dialog_tmpl = {
    NCSCTRL_DIALOGBOX,
    1,
    PP_X, PP_Y, PP_W, PP_H,
    WS_NONE,
    WS_EX_NONE,
    "ActivityPicturePlayer",
    NULL, NULL,
    mainwndHandlers,
    ctrlTmpl, TABLESIZE(ctrlTmpl), 0, 0,
};

BOOL ActivityPicturePlayer::isFull(){
    return mIsFull;
}
void ActivityPicturePlayer::setFull(BOOL isFull){
    mIsFull = isFull;
}
int ActivityPicturePlayer::indexInc(){
    return ++mIndex;
}
int ActivityPicturePlayer::indexDec(){
    if(mIndex == 0)
        mIndex = TABLESIZE(gPic) - 1;
    else
        --mIndex;
    return mIndex;
}
int ActivityPicturePlayer::getIndex(){
    return mIndex;
}
void ActivityPicturePlayer::setIndex(int index){
    mIndex = index;
}
void ActivityPicturePlayer::setTitle(mHotPiece *title){
    mTitle = title;
}
mHotPiece *ActivityPicturePlayer::getTitle(void){
    return mTitle;
}
void ActivityPicturePlayer::setCtrl(mHotPiece *ctrl){
    mCtrl = ctrl;
}
mHotPiece *ActivityPicturePlayer::getCtrl(void){
    return mCtrl;
}
void ActivityPicturePlayer::setImage(mHotPiece *image){
     mImage = image;
}
mHotPiece *ActivityPicturePlayer::getImage(void){
    return mImage;
}
void ActivityPicturePlayer::setPic(BITMAP *pic){
    mPic = pic;
}
mHotPiece *ActivityPicturePlayer::getDelBk(){
    return mDelBk;
}
void ActivityPicturePlayer::setDelBk(mHotPiece* bk){
    mDelBk = bk;
}
BITMAP *ActivityPicturePlayer::getPic(){
    return mPic;
}
BITMAP *ActivityPicturePlayer::getNextPic(){
    return mNextPic;
}
BITMAP *ActivityPicturePlayer::getPlayPic(){
    return mPlayPic;
}
BITMAP *ActivityPicturePlayer::getPrevPic(){
    return mPrevPic;
}
void ActivityPicturePlayer::setIsDown(BOOL isDown){
    mIsDown = isDown;
}
BOOL ActivityPicturePlayer::getIsDown(){
    return mIsDown;
}
void ActivityPicturePlayer::setPoint(int x, int y){
    mPoint.x = x;
    mPoint.y = y;
}
POINT *ActivityPicturePlayer::getPoint(){
    return &mPoint;
}
void ActivityPicturePlayer::setDirection(MOVEDIRECTION dir){
    mDirection = dir;
}
ActivityPicturePlayer::MOVEDIRECTION ActivityPicturePlayer::getDirection(){
    return mDirection;
}
void ActivityPicturePlayer::initRes(){
    //SetResPath("res/pictureviewer/");

    m_titleFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, 
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            PP_TITLE_TEXT_FONT_H, 0);
    m_playFont = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, 
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            PP_PLAY_TEXT_FONT_H, 0);
}
int ActivityPicturePlayer::loadRes(void){
    mPic = (BITMAP *)Load32Resource (gPic[mIndex], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);

    mNextPic = (BITMAP *)Load32Resource("res/pictureviewer/pic_next.png",
            RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    mPlayPic = (BITMAP *)Load32Resource("res/pictureviewer/pic_pause_play.png",
            RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    mPrevPic = (BITMAP *)Load32Resource("res/pictureviewer/pic_prev.png",
            RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    return 0;
}
int ActivityPicturePlayer::releaseRes(void){
    ReleaseRes(Str2Key(gPic[mIndex]));
    ReleaseRes(Str2Key("res/pictureviewer/pic_next.png"));
    ReleaseRes(Str2Key("res/pictureviewer/pic_pause_play.png"));
    ReleaseRes(Str2Key("res/pictureviewer/pic_prev.png"));

    DestroyLogFont(m_titleFont);
    DestroyLogFont(m_playFont);
    return 0;
}

int ActivityPicturePlayer::onHome()
{
    //mWidget *cc = ncsGetChildObj(this->hwnd(), IDC_CC);
    //mPanelPiece *body  = (mPanelPiece*)cc->body;
    mPanelPiece *body  = mBody;
    _c(body)->movePiece(body, 
            (mHotPiece*)getImage(),PP_CTRL_X,0);
    return 0;
}

int ActivityPicturePlayer::onBack()
{
    if (this->isFull())
    {
    #if 0
        mWidget *cc = ncsGetChildObj(this->hwnd(), IDC_CC);
        MoveWindow(cc->hwnd,0,0,SCREEN_W,SCREEN_H-ACTIVITY_Y,TRUE);
        MoveWindow(this->hwnd(),0,ACTIVITY_Y,SCREEN_W,SCREEN_H-ACTIVITY_Y,TRUE);
        return 0;
    #else
        SendMessage(this->hwnd(),MSG_LBUTTONDOWN,100,200);
        SendMessage(this->hwnd(),MSG_LBUTTONUP,100,200);
    #endif
        return -1;
    }
    return 0;
}
ActivityPicturePlayer::ActivityPicturePlayer()
    :NCSActivity( &dialog_tmpl){
    MGNCS_INIT_CLASS(mShapeTransRoundPiece);
    mIndex = 0;
    mIsDown = FALSE;
    mDirection = ActivityPicturePlayer::MOVE_NO;
    mIsFull = FALSE;
    m_isExiting = FALSE;
    mPic = NULL;
    initRes();
    loadRes();
    m_style = STYLE_ZOOM;
}
ActivityPicturePlayer::~ActivityPicturePlayer(){
    releaseRes();
}

