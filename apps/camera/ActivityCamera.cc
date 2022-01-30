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
#include "global.h"
#include "effector-shoot.h"

#include "ActivityCamera.hh"

REGISTER_ACTIVITY(ActivityCamera);

#define IDC_VIEW        200
#define IDC_TOOLBAR     201
#define IDC_TIMER       202

#define GIF_FILE "res/camera/animation.gif"
#define GIF_JPEG_PATH "animation.png"
/* images */
enum _Image_Index{
    INDEX_PLAY_PRESSED,
    INDEX_PLAY_NORMAL,
    INDEX_SHOOT_PRESSED,
    INDEX_SHOOT_NORMAL,
    INDEX_VIEW,
    INDEX_IMAGE_MAX,
};
const static char *PICTURES[] ={
    "res/camera/play_pressed.png", 
    "res/camera/play_normal.png", 
    "res/camera/shoot_pressed.png",
    "res/camera/shoot_normal.png",
    "res/camera/animation.png",
    NULL,
};

#define SHOOTING_ASYN

#ifdef SHOOTING_ASYN
typedef struct{
    MGEFF_EFFECTOR effector;
    HWND hwnd;
    HDC sink_dc;
    HDC src1_dc;
    HDC src2_dc;
}effector_params_t;
#endif
/* position */
const static POINT TOOLBAR_LEFTTOP = {0, 431};
const static POINT SHOOTER_LEFTTOP = {140, 11};
const static POINT PLAYER_LEFTTOP = {13, 11};

#define TOOLBAR_W   ACTIVITY_W
#define TOOLBAR_H   49
#define SHOOTING_VAL 1
#if 0
static void shooting_sleep()
{
#ifdef WIN32
    clock_t endwait;
    int seconds = SHOOTING_VAL;
    endwait = clock () + (seconds * CLOCKS_PER_SEC)/2;
    while (clock() < endwait) {}
#else
    struct timeval timeout = {1, SHOOTING_VAL*1000/2};
    select(0, NULL, NULL, NULL, &timeout);
#endif /* WIN32 */
}
#endif

#ifdef SHOOTING_ASYN
static void my_finish_cb(MGEFF_ANIMATION handle)
{
    mTimer* timer;
    effector_params_t *params = (effector_params_t *)mGEffAnimationGetContext(handle);
    //ActivityCamera *act = (ActivityCamera*)Activity::getActivityFromHWND(params->hwnd);
    mGEffEffectorDelete (params->effector);
    DeleteMemDC (params->src1_dc);
    DeleteMemDC (params->src2_dc);
    ReleaseDC (params->sink_dc);

    timer = (mTimer*)ncsGetChildObj(params->hwnd, IDC_TIMER);
    _c(timer)->start(timer);

    return; 
}
#endif
static int shooting (mWidget *self, HDC src1_dc, HDC src2_dc, HDC dst_dc, int index, int duration)
{
    MGEFF_EFFECTOR effector = mGEffEffectorCreate (MGEFF_EFFECTOR_SHOOT);
    MGEFF_SOURCE src1 = mGEffCreateSource (src1_dc);
    MGEFF_SOURCE src2 = mGEffCreateSource (src2_dc);
    MGEFF_SINK   sink = mGEffCreateHDCSink (dst_dc);
    MGEFF_ANIMATION anim;

#ifdef SHOOTING_ASYN
    mWidget *parent = ncsGetParentObj(self->hwnd);
    static effector_params_t effector_params; 
    memset(&effector_params,0x00,sizeof(effector_params_t));
#endif
    mGEffEffectorAppendSource (effector, src1);
    mGEffEffectorAppendSource (effector, src2);
    mGEffSetBufferSink (sink, src1_dc);
    mGEffEffectorSetSink (effector, sink);
    mGEffEffectorSetProperty (effector, MGEFF_PROPERTY_ZOOM, MGEFF_ZOOMOUT);

    anim = mGEffAnimationCreateWithEffector (effector);
    mGEffAnimationSetCurve (anim, OutCubic);
    mGEffAnimationSetDuration (anim, duration);
    //mGEffAnimationSetDuration (anim, 2000);
#ifdef SHOOTING_ASYN
    mGEffAnimationSetFinishedCb(anim, my_finish_cb);

    effector_params.effector = effector;
    effector_params.hwnd = parent->hwnd;
    effector_params.sink_dc = dst_dc;
    effector_params.src1_dc = src1_dc;
    effector_params.src2_dc = src2_dc;
    mGEffAnimationSetContext(anim,(void *)&effector_params);

    mGEffAnimationAsyncRun (anim);
    mGEffAnimationSetProperty (anim, MGEFF_PROP_KEEPALIVE, 0);
#else
    mGEffAnimationSyncRun (anim);
    mGEffAnimationDelete (anim);
    mGEffEffectorDelete (effector);
#endif

//    shooting_sleep();

    return 0;
}

static int anim_onShoot (mWidget *self)
{
    mWidget *parent = ncsGetParentObj(self->hwnd);
    ActivityCamera *act = (ActivityCamera*)Activity::getActivityFromHWND(parent->hwnd);
    HDC sink_dc;
    HDC src1_dc;
    HDC src2_dc;
    MGEFF_ANIMATION anim;
#ifndef SHOOTING_ASYN
    mTimer *timer;
#else
    int prop;
#endif
    anim = act->getAnim();
    prop = mGEffAnimationGetProperty(anim, MGEFF_PROP_STATE);

    /*fix bug:371*/
    /*you can shoot before the anim is running.otherwise mGEffAnimatonPause will return with doing nothing.*/
    if(prop != MGEFF_STATE_RUNNING) 
        return 0;

    act->m_isShooting = TRUE;
    mGEffAnimationPause(anim);

    sink_dc = GetDC(self->hwnd);
    src1_dc = CreateCompatibleDC(sink_dc);
    src2_dc = CreateCompatibleDC(sink_dc);
    BitBlt (sink_dc, 0, 0, 0, 0, src1_dc, 0, 0, 0);
    SetBrushColor (src2_dc, RGB2Pixel(src2_dc, 0x3f, 0x3f, 0x3f));
    FillBox (src2_dc, 0, 0, 
            GetGDCapability(src2_dc, GDCAP_MAXX) + 1,
            GetGDCapability(src2_dc, GDCAP_MAXY) + 1);

    shooting (self, src1_dc, src2_dc, sink_dc, 0, 500);
#ifndef SHOOTING_ASYN
    DeleteMemDC (src1_dc);
    DeleteMemDC (src2_dc);
    ReleaseDC (sink_dc);

    timer = (mTimer*)ncsGetChildObj(parent->hwnd, IDC_TIMER);
    _c(timer)->start(timer);
#endif
    return 0;
}

static BOOL s_onClickEvent(mWidget *self, mHotPiece *piece, int event_id,  DWORD param)
{
    HWND hwnd = GetMainWindowHandle(self->hwnd);
    ActivityCamera *activity = (ActivityCamera*)Activity::getActivityFromHWND (hwnd);
    static BOOL haveShoot = FALSE;

    if (event_id == NCSN_ABP_CLICKED||event_id == NCSN_ABP_PUSHED) {
        if (!activity) {
            printf ("camera activity invalid!\n");
            return FALSE;
        }
        if (event_id == NCSN_ABP_PUSHED && piece == (mHotPiece*)activity->shooterPiece()) {
#ifdef SHOOTING_ASYN
            if (activity->m_isShooting)
                return FALSE;
#endif
            UpdateWindow(self->hwnd,FALSE);
            mWidget *parentObj = ncsGetParentObj (self->hwnd);
            mWidget *animateObj =ncsGetChildObj (parentObj->hwnd, IDC_VIEW);
            haveShoot = TRUE;
            return anim_onShoot(animateObj);
        }else if (event_id == NCSN_ABP_CLICKED && piece == (mHotPiece*)activity->playPiece()) {
            //TODO: to play
            MGEFF_ANIMATION anim;
            anim = activity->getAnim();
            mGEffAnimationPause(anim);
            if (haveShoot)
            {
                activity->m_intent->putExtra("index",(const char *)GIF_JPEG_PATH);
            }
            else
            {
                activity->m_intent->putExtra("index",(const char *)"0");
            }
            ACTIVITYSTACK->switchTo("ActivityPicturePlayer",activity->m_intent);
            haveShoot = FALSE;
        } else {
            printf ("not my care piece! p: %p\n", piece);
        }
        return TRUE;
    }
    return FALSE;
}
static BOOL on_timer(mObject *listener, mObject *sender, int id, DWORD total_count)
{
    MGEFF_ANIMATION anim;
    mWidget *self = (mWidget*)listener;
    ActivityCamera *act = (ActivityCamera*)Activity::getActivityFromHWND(self->hwnd);
    mTimer *timer = (mTimer*)sender;
    int prop;

    anim = act->getAnim();
    prop = mGEffAnimationGetProperty(anim, MGEFF_PROP_STATE);
    if(prop == MGEFF_STATE_PAUSED){
        act->m_isShooting = FALSE;
        mGEffAnimationResume(anim);
    }
    else if(prop == MGEFF_STATE_STOPPED)
        mGEffAnimationAsyncRun(anim);

    _c(timer)->stop(timer);

    return TRUE;
}

mPanelPiece *ActivityCamera::createView(mWidget *self)
{
    mPanelPiece *view;
    mImagePiece *image;
    RECT rc;
    BITMAP *bmp;

    view = NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, SCREEN_W, TOOLBAR_LEFTTOP.y);
    _c(view)->setRect(view, &rc);

    bmp = (PBITMAP)GetResource(Str2Key(PICTURES[INDEX_VIEW]));
    image = NEWPIECE(mImagePiece);
    //SetRect(&rc, 0, 0, bmp->bmWidth, bmp->bmHeight);
    _c(image)->setRect(image, &rc);
    _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)bmp);
    _c(view)->addContent(view, (mHotPiece*)image, 0, 0);

    if (m_image == HDC_INVALID) {
        m_image = CreateMemDCFromBitmap(Get32MemDC(), bmp);
    }
    
    return view;
}

mPanelPiece* ActivityCamera::createToolbar(mWidget *self)
{
    RECT rc;
    mPanelPiece *body;
    BITMAP *pbmp_pressed, *pbmp_normal;

    HWND hwnd = GetMainWindowHandle(self->hwnd);
    ActivityCamera *activity = (ActivityCamera*)Activity::getActivityFromHWND (hwnd);

    body = NEWPIECE(mPanelPiece);
    SetRect(&rc, 0, 0, TOOLBAR_W, TOOLBAR_H);
    _c(body)->setRect(body, &rc);

    {
        ARGB  colors[] = {0xff0f1700, 0xff434343, 0xff2d2d2d, 0xff151515
            ,0xff010101,0xff000000};
        float positions[] = {0.0,0.01, 0.01, 0.49, 0.51,1.0};
        SetRect(&rc, 0, 0, TOOLBAR_W, TOOLBAR_H);
        mShapeTransRoundPiece *background = NEWPIECE(mShapeTransRoundPiece);
        _c(background)->setRect(background, &rc);
        _c(background)->setProperty(background, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
        _c(background)->setProperty(background, NCSP_TRANROUND_CORNERFLAG, 0);
        HBRUSH brush = _c(background)->getBrush(background);
        MGPlusSetLinearGradientBrushColorsEx (brush, colors, TABLESIZE(positions), positions);
        _c(body)->addContent(body, (mHotPiece*)background, 0, 0);
    }
    {   /* play pannel */
        pbmp_pressed = (PBITMAP)GetResource(Str2Key(PICTURES[INDEX_PLAY_PRESSED]));
        pbmp_normal = (PBITMAP)GetResource(Str2Key(PICTURES[INDEX_PLAY_NORMAL]));

        /* image piece */
        mImagePiece* imgPiece = NEWPIECE(mImagePiece);
        SetRect(&rc, 0, 0, pbmp_normal->bmWidth, pbmp_normal->bmHeight);
        _c(imgPiece)->setRect(imgPiece, &rc);
        _c(imgPiece)->setProperty(imgPiece, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbmp_normal);

        /* pannel piece */
        mButtonPanelPiece *playPiece = NEWPIECE(mButtonPanelPiece);
        SetRect(&rc, 0, 0, pbmp_normal->bmWidth, TOOLBAR_H);
        _c(playPiece)->setRect(playPiece, &rc);
        _c(playPiece)->setStateTable(playPiece, 
                (mHotPiece*)imgPiece, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbmp_normal, (DWORD)pbmp_pressed);
        _c(playPiece)->setStates(playPiece);

        /* add imgPiece to player piece */
        _c(playPiece)->addContentToLayout(playPiece, (mHotPiece*)imgPiece);
        /* add to body */
        _c(body)->addContent(body, (mHotPiece*)playPiece, PLAYER_LEFTTOP.x, 0);

        ncsAddEventListener((mObject*)playPiece, (mObject*)self, (NCS_CB_ONPIECEEVENT)s_onClickEvent, NCSN_ABP_CLICKED);
        /* save shooter piece */
        activity->setPlayPiece (playPiece);
    }
    {   /* shooter piece */
        pbmp_pressed = (PBITMAP)GetResource(Str2Key(PICTURES[INDEX_SHOOT_PRESSED]));
        pbmp_normal = (PBITMAP)GetResource(Str2Key(PICTURES[INDEX_SHOOT_NORMAL]));

        /* image piece */
        mImagePiece* imgPiece = NEWPIECE(mImagePiece);
        SetRect(&rc, 0, 0, pbmp_normal->bmWidth, pbmp_normal->bmHeight);
        _c(imgPiece)->setRect(imgPiece, &rc);
        _c(imgPiece)->setProperty(imgPiece, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbmp_normal);

        /* pannel piece */
        mButtonPanelPiece *shooterPiece = NEWPIECE(mButtonPanelPiece);
        SetRect(&rc, 0, 0, pbmp_normal->bmWidth, TOOLBAR_H);
        _c(shooterPiece)->setRect(shooterPiece, &rc);
        _c(shooterPiece)->setStateTable(shooterPiece, 
                (mHotPiece*)imgPiece, NCSP_IMAGEPIECE_IMAGE, (DWORD)pbmp_normal, (DWORD)pbmp_pressed);
        _c(shooterPiece)->setStates(shooterPiece);

        /* add imgPiece to shooter piece */
        _c(shooterPiece)->addContentToLayout(shooterPiece, (mHotPiece*)imgPiece);
        /* add to body */
        _c(body)->addContent(body, (mHotPiece*)shooterPiece, SHOOTER_LEFTTOP.x, 0);

        ncsAddEventListener((mObject*)shooterPiece, (mObject*)self, (NCS_CB_ONPIECEEVENT)s_onClickEvent, NCSN_ABP_PUSHED);
        /* save shooter piece */
        activity->setShooterPiece (shooterPiece);
    }

    return body;
}

void ActivityCamera::createBody(mWidget *self)
{
    mContainerCtrl *ctView;
    mContainerCtrl *ctTool;
    mPanelPiece *toolbar;
    mPanelPiece *view;

    ctView = (mContainerCtrl*)ncsGetChildObj(self->hwnd, IDC_VIEW);
    ctTool = (mContainerCtrl*)ncsGetChildObj(self->hwnd, IDC_TOOLBAR);

    view = createView((mWidget*)ctView);
    _c(ctView)->setBody(ctView, (mHotPiece*)view);

    toolbar = createToolbar((mWidget*)ctTool);
    _c(ctTool)->setBody(ctTool, (mHotPiece*)toolbar);
}

static void on_view(MGEFF_ANIMATION handle, void *target, intptr_t id, void *value)
{
    mWidget *self = (mWidget*)target;
    ActivityCamera *act = (ActivityCamera*)Activity::getActivityFromHWND(self->hwnd);
    HDC hdc;
    HDC image;
    POINT v;

    hdc = GetDC(self->hwnd);
    image = act->getImage();
    v = *(POINT*)value;
    BitBlt(image, v.x, v.y, SCREEN_W, TOOLBAR_LEFTTOP.y, hdc, 0, 0, 0);
#if 0
    switch(id){
        case 0:
        case 1:
        case 2:
            BitBlt(image, v, 129, SCREEN_W, TOOLBAR_LEFTTOP.y, hdc, 0, 0, 0);
            break;
        case 3:
        case 4:
        case 5:
            BitBlt(image, 50, v, SCREEN_W, TOOLBAR_LEFTTOP.y, hdc, 0, 0, 0);
            break;
        default:
            break;
    }
#endif
    ReleaseDC(hdc);
}

#define PHOTOANIMATION_NUM  16
#define PHOTOANIMATION_LOOPCOUNT    10000
#define PHOTOANIMATION_DURATION 1000
static MGEFF_ANIMATION createPhotoAnimation(mWidget *self)
{
    int i;
    MGEFF_ANIMATION anim[PHOTOANIMATION_NUM];
    MGEFF_ANIMATION groupanim;
    mTimer *timer;
    POINT sv[PHOTOANIMATION_NUM] = {
        {50, 129}, {0, 79}, {50, 129}, {0, 179}, {50, 129}, {100, 129}, {50, 129}, {50, 79},
        {50, 129}, {100, 179}, {50, 129}, {50, 179}, {50, 129}, {100, 79}, {50, 129}, {0, 129},}; 
    POINT ev[PHOTOANIMATION_NUM] = {
        {0, 79}, {50, 129}, {0, 179},  {50, 129}, {100, 129}, {50, 129}, {50, 79}, {50, 129}, 
        {100, 179}, {50, 129}, {50, 179}, {50, 129}, {100, 79}, {50, 129}, {0, 129}, {50, 129}, };

    groupanim = mGEffAnimationCreateGroup(MGEFF_SEQUENTIAL);

    for(i=0; i<PHOTOANIMATION_NUM; i++){
        anim[i] = mGEffAnimationCreate(self, on_view, i, /*MGEFF_INT*/MGEFF_POINT);
        mGEffAnimationSetDuration(anim[i], PHOTOANIMATION_DURATION);
        mGEffAnimationSetStartValue(anim[i], &sv[i]);
        mGEffAnimationSetEndValue(anim[i], &ev[i]);
        
        mGEffAnimationAddToGroup(groupanim, anim[i]);
    }

    mGEffAnimationSetProperty(groupanim, MGEFF_PROP_LOOPCOUNT, PHOTOANIMATION_LOOPCOUNT);
    //mGEffAnimationAsyncRun(groupanim);

    timer = (mTimer*)ncsGetChildObj(self->hwnd, IDC_TIMER);
    if(timer){
        ncsAddEventListener((mObject*)timer, (mObject*)self, (NCS_CB_ONPIECEEVENT)on_timer, MSG_TIMER);
        _c(timer)->start(timer);
    }

    return groupanim;
}

/* main window */
static BOOL mainwnd_onCreate (mWidget* self, DWORD add_data)
{
    ActivityCamera* activity = (ActivityCamera*)Activity::getActivityFromHWND(self->hwnd);

    activity->createBody(self);

    activity->setAnim(createPhotoAnimation(self));

    return TRUE;
}

static void mainwnd_onClose (mWidget* self, int message)
{
    ActivityCamera* act = (ActivityCamera*)Activity::getActivityFromHWND(self->hwnd);
    MGEFF_ANIMATION anim = act->getAnim();
    mGEffAnimationStop(anim);
    mGEffAnimationDelete(anim);
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

static NCS_PROP_ENTRY timer_props[] = {
    {NCSP_TIMER_INTERVAL, 10},//unit:10ms
    {0, 0},
};
static NCS_WND_TEMPLATE _ctrl_tmpl[] = {
    {
        NCSCTRL_CONTAINERCTRL,
        IDC_TOOLBAR,
        0, TOOLBAR_LEFTTOP.y, SCREEN_W, TOOLBAR_H,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL,
        NULL,
        0, 0,
    },
    {
        NCSCTRL_CONTAINERCTRL,
        IDC_VIEW,
        0, 0, SCREEN_W, TOOLBAR_LEFTTOP.y,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        NULL,
        NULL,
        NULL,
        NULL,
        0, 0,
    },
    {
        NCSCTRL_TIMER,
        IDC_TIMER,
        0,0,0,0,
        WS_VISIBLE,
        WS_EX_NONE,
        "",
        timer_props, NULL, NULL, NULL, 0, 0, 
    },
};
static NCS_MNWND_TEMPLATE dialog_tmpl = {
    NCSCTRL_DIALOGBOX, 
    1,
    0, 0, SCREEN_W, SCREEN_H, /* full screen */
    WS_NONE,
    WS_EX_NONE,
    "ActivityCamera",
    NULL,
    NULL,
    mainwnd_handlers,
	_ctrl_tmpl,
	TABLESIZE(_ctrl_tmpl),//sizeof(_ctrl_tmpl)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};


void ActivityCamera::loadResource()
{
    int i;
    for (i=0; i<INDEX_IMAGE_MAX; ++i) {
        Load32Resource (PICTURES[i], RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
    }

    mGEffEffectorRegister (&shoot);
}

void ActivityCamera::unloadResource()
{
    int i;
    for (i=0; i<INDEX_IMAGE_MAX; ++i) {
        ReleaseRes(Str2Key(PICTURES[i]));
    }

    mGEffEffectorUnRegister (&shoot);
}

int ActivityCamera::onResume()
{
    mGEffAnimationResume(m_anim);
    return 0;
}

ActivityCamera::ActivityCamera():
    NCSActivity( &dialog_tmpl)
{
    loadResource ();
    m_intent = Intent::newIntent();
    m_count = 0;
    m_image = HDC_INVALID;
    m_isShooting = FALSE;
}

ActivityCamera::~ActivityCamera()
{
    if (m_image != HDC_INVALID) {
        DeleteMemDC (m_image);
        m_image = HDC_INVALID;
    }

    Intent::deleteIntent(m_intent);

    unloadResource ();
}

