#include "ActivityStack.hh"
#include "CoverFlowActivity.hh"
#include "mspeedmeter.h"
#include "MediaStoreService.hh"
#include <sys/time.h>
#include <minigui/control.h>

REGISTER_ACTIVITY(CoverFlowActivity);

#ifdef __cplusplus
extern "C" {
#endif
extern void coverflow_init(void);
extern void coverflow_deinit(void);
extern unsigned int coverflow_elasped_time(struct timeval *tv);
extern void coverflow_set_texture(unsigned char *image, int w, int h, int bpp, int itemw, int itemh, int n);
extern void coverflow_set_sub_tex_index(unsigned *indexarray);
extern void coverflow_add(void);
extern void coverflow_draw(void);
extern void coverflow_backing_anim_start_end_value(float *s, float *e);
extern void coverflow_stepping_anim_start_end_value(float *s, float *e, int direction);
extern void coverflow_scrolling_anim_start_end_value(float *s, float *e, float vx);
extern void coverflow_reshape(int w, int h);
extern bool coverflow_set_eye(float eyex, float eyey, float eyez, bool depthtest);
extern float coverflow_eyex(void);
extern int coverflow_hilight(void);
#ifdef __cplusplus
}
#endif

CoverFlowActivity::CoverFlowActivity() :
    m_animation(NULL), m_depthTest(FALSE),
    m_mouseMoved(FALSE), m_mouseDir(COVERFLOW_MOUSEDIR_LEFT),
    m_lButtonDown(FALSE),
    m_stepping(FALSE), m_scrolling(FALSE),
    m_songNames(NULL), m_albumIndex(NULL)
{
    SetRect(&m_hilightRect,
            ACTIVITY_W / 2 - 75, ACTIVITY_W / 2 - 60,
            ACTIVITY_W / 2 + 75, ACTIVITY_W / 2 + 90);
    m_mouse.x = m_mouse.y = 0;
    gettimeofday(&m_tv, NULL);
    m_style = STYLE_PUSH;
    m_albumList.size        = 0;
    m_albumList.authorName  = NULL;
    m_albumList.albumName   = NULL;
    m_intent = Intent::newIntent(ACTION_VIEW);
}

CoverFlowActivity::~CoverFlowActivity()
{
    Intent::deleteIntent(m_intent);
    releaseSongData();
}

BOOL CoverFlowActivity::setMouseDir(int mouseMove)
{
    int oldDir = m_mouseDir;
    m_mouseDir = mouseMove > 0 ? COVERFLOW_MOUSEDIR_RIGHT : COVERFLOW_MOUSEDIR_LEFT;
    return m_mouseDir != oldDir;
}

void CoverFlowActivity::loadBitmapToGL(const char *path, int albumSize)
{
    PBITMAP bmp = NULL;

    if (path != NULL) {
        LoadResource(path, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
        bmp = (PBITMAP)GetResource(Str2Key(path));

        if (bmp != NULL) {
            coverflow_set_texture(bmp->bmBits,
                    bmp->bmWidth, bmp->bmHeight, bmp->bmBytesPerPixel,
                    128, 128, albumSize);
        }

        ReleaseRes(Str2Key(path));
    }
}

void CoverFlowActivity::createAnimation(void *target, float s, float e, unsigned duration, EffMotionType curve, bool depthTest)
{
    m_animation = mGEffAnimationCreate(target, call_draw, -1, MGEFF_FLOAT);
    mGEffAnimationSetDuration(m_animation, duration);
    mGEffAnimationSetCurve(m_animation, OutQuad);
    mGEffAnimationSetStartValue(m_animation, (void*)&s);
    mGEffAnimationSetEndValue(m_animation, (void*)&e);
    mGEffAnimationSetContext(m_animation, (void*)&m_depthTest);
    m_depthTest = depthTest;
}

void CoverFlowActivity::backingAnimation(void *target)
{
    float s, e;
    if (m_animation)
        return;
    coverflow_backing_anim_start_end_value(&s, &e);
    if (s != e) {
//        printf("%s %f %f\n", __FUNCTION__, s, e);
        createAnimation(target, s, e, 200, OutQuad, true);
        mGEffAnimationSyncRun(m_animation);
        deleteAnimation();
    }
}

void CoverFlowActivity::s_stepping_finished_cb(MGEFF_ANIMATION anim)
{
    HWND hWnd = (HWND)mGEffAnimationGetTarget(anim);
    CoverFlowActivity *act =
        (CoverFlowActivity*)Activity::getActivityFromHWND(GetMainWindowHandle(hWnd));
    act->setStepping(FALSE);
    act->deleteAnimation();
    coverflow_set_eye(coverflow_eyex(), 0.0, 0.0, false);
}

void CoverFlowActivity::steppingAnimation(void * target, int direction)
{
    float s, e;
    if (m_animation)
        return;
    coverflow_stepping_anim_start_end_value(&s, &e, direction);
    if (s != e) {
//        printf("%s %f %f\n", __FUNCTION__, s, e);
        createAnimation(target, s, e, 300, OutQuad, false);
        mGEffAnimationSetFinishedCb(m_animation, s_stepping_finished_cb);
        setStepping(TRUE);
        mGEffAnimationAsyncRun(m_animation);
    }
}

void CoverFlowActivity::s_scrolling_finished_cb(MGEFF_ANIMATION anim)
{
    HWND hWnd = (HWND)mGEffAnimationGetTarget(anim);
    HWND hWndParent = GetParent(hWnd);
    CoverFlowActivity *act =
        (CoverFlowActivity*)Activity::getActivityFromHWND(GetMainWindowHandle(hWnd));
    act->setScrolling(FALSE);
    act->deleteAnimation();
    coverflow_set_eye(coverflow_eyex(), 0.0, 0.0, false);
    SendMessage(hWndParent, MSG_LBUTTONUP, 0, 0);
}

void CoverFlowActivity::scrollingAnimation(void *target)
{
    float s, e, vx, vy;
    int duration;
    if (m_animation)
        return;
    QueryMouseMoveVelocity(&vx, &vy);
    coverflow_scrolling_anim_start_end_value(&s, &e, vx / 5.0);
    duration = abs(vx) / 2;
    if (s != e && duration > 0) {
//        printf("%s %f %f\n", __FUNCTION__, s, e);
        createAnimation(target, s, e, duration, InQuad, true);
        mGEffAnimationSetFinishedCb(m_animation, s_scrolling_finished_cb);
        setScrolling(TRUE);
        mGEffAnimationAsyncRun(m_animation);
    }
//    printf("vx = %f\n", vx);
}

void CoverFlowActivity::call_draw(MGEFF_ANIMATION anim, void *target, int id, void *value)
{
    bool *depthTest = (bool*)mGEffAnimationGetContext(anim);
    if(!coverflow_set_eye(*(float*)value, 0.0, 0.0, *depthTest))
        mGEffAnimationStop(anim);
    UpdateWindow((HWND)target, TRUE);
}

int CoverFlowActivity::proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    int mouse_move_x;
    float vx, vy;
    CoverFlowActivity *act =
        (CoverFlowActivity*)Activity::getActivityFromHWND(GetMainWindowHandle(hWnd));
    HWND hWndGL = GetDlgItem(hWnd, 10032);
    std::string* strbuff = NULL;

    switch (message) {
        case MSG_LBUTTONDOWN:
            SetCapture(hWnd);
            act->setLButtonDown(TRUE);
            if (act->scrolling()) {
                mGEffAnimationStop(act->animation());
            } else {
                act->setMouse(LOSWORD(lParam), HISWORD(lParam));
                SpeedMeterMessageHandler(NULL, message, LOSWORD(lParam), HISWORD(lParam), (DWORD)wParam);
            }
            return 0;
        case MSG_MOUSEMOVE:
            if (act->lButtonDown()) {
                mouse_move_x = LOSWORD(lParam) - act->mouseX();
                act->setMouseMoved(TRUE);
                if (abs(mouse_move_x) > 3) {
                    gettimeofday(act->tv(), NULL);
                    act->setMouse(LOSWORD(lParam), HISWORD(lParam));
                    SpeedMeterMessageHandler(NULL, message, LOSWORD(lParam), HISWORD(lParam), (DWORD)wParam);
                }
                if (!act->stepping()) {
                    if (act->setMouseDir(mouse_move_x)) {
#if 0
                        SpeedMeterMessageHandler(NULL, MSG_LBUTTONUP, LOSWORD(lParam), HISWORD(lParam), (DWORD)wParam);
                        SpeedMeterMessageHandler(NULL, MSG_LBUTTONDOWN, LOSWORD(lParam), HISWORD(lParam), (DWORD)wParam);
                        SpeedMeterMessageHandler(NULL, MSG_MOUSEMOVE, LOSWORD(lParam), HISWORD(lParam), (DWORD)wParam);
#endif
                    }
                    QueryMouseMoveVelocity(&vx, &vy);
                    if (abs(vx) > 80) {
//                        printf("mouse_move_x = %d\n", mouse_move_x);
                        if (abs(mouse_move_x) < 10)
                            coverflow_set_eye(coverflow_eyex() - mouse_move_x, 0.0, 0.0, false);
                        else
                            coverflow_set_eye(coverflow_eyex() - mouse_move_x, 0.0, 0.0, true);
                        InvalidateRect(hWndGL, NULL, TRUE);
                    } else if (vx != 0){
                        act->pourStepCan(mouse_move_x);
                        if (abs(act->stepCan()) > 10) {
                            act->clearStepCan();
                            act->steppingAnimation((void*)hWndGL, mouse_move_x);
                        }
                    }
                }
            }
            return 0;
        case MSG_LBUTTONUP:
            ReleaseCapture();
            if (act->lButtonDown()) {
                if (act->mouseMoved()) {
                    SpeedMeterMessageHandler(NULL, message, LOSWORD(lParam), HISWORD(lParam), (DWORD)wParam);
                    if (coverflow_elasped_time(act->tv()) > 10) {
                        if (!act->stepping() && !act->scrolling())
                            act->backingAnimation((void*)hWndGL);
                    } else {
                        act->scrollingAnimation((void*)hWndGL);
                    }
                    act->setMouseMoved(FALSE);
                    coverflow_set_eye(coverflow_eyex(), 0.0, 0.0, false);
                } else {
                    if (PtInRect(act->hilightRect(), LOSWORD(lParam), HISWORD(lParam))) {
                        printf("hilight = %d\n", coverflow_hilight());
                        strbuff = new std::string((act->albumName(coverflow_hilight())));
                        strbuff->append("&", 1);
                        strbuff->append(act->songName(coverflow_hilight()));
                        act->m_intent->putExtra("music info", *strbuff);
                        ACTIVITYSTACK->switchTo("PhoneAlbumActivity", act->m_intent);
                        delete(strbuff);
                    }
                }
                act->setLButtonDown(FALSE);
            } else {
                act->backingAnimation((void*)hWndGL);
            }
            return 0;
        case MSG_CLOSE:
            coverflow_deinit();
            act->EGLRelease();
            DestroyMainWindow(hWnd);
            return 0;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int CoverFlowActivity::glProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static RECT rc = {0, ACTIVITY_H / 8 * 5, ACTIVITY_W, ACTIVITY_H / 8 * 5 + 20};
    CoverFlowActivity *act =
        (CoverFlowActivity*)Activity::getActivityFromHWND(GetMainWindowHandle(hWnd));

    switch(message) {
        case MSG_PAINT:
            coverflow_draw();
            mgEGLSwapBuffers( act->display(), act->surface());
            {
                hdc = GetSecondaryDC(hWnd);
                SetBkMode (hdc, BM_TRANSPARENT);
                SetTextColor(hdc, 0xFFFFFFFF);
                DrawText(hdc, act->songName(coverflow_hilight()), -1, &rc, DT_CENTER | DT_VCENTER);
                rc.top += 20, rc.bottom += 20;
                DrawText(hdc, act->albumName(coverflow_hilight()), -1, &rc, DT_CENTER | DT_VCENTER);
                rc.top += 20, rc.bottom += 20;
                DrawText(hdc, act->authorName(coverflow_hilight()), -1, &rc, DT_CENTER | DT_VCENTER);
                rc.top -= 40, rc.bottom -= 40;
                ReleaseDC(hdc);
            }
            break;
        case MSG_ERASEBKGND:
            return 0;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

void CoverFlowActivity::loadSongData()
{
    unsigned i, j, k, size;
    char **songNames;
    unsigned *albumIndex;

    MediaStoreService *music_server;
    ContentValues::Strings album_names;
    std::string author_name;
    ContentValues::Strings songs_at_album;
    music_server = GET_MEDIASTORE_SERVICE();
    album_names  = music_server->getAllAlbumName();

    m_albumList.size  = album_names.size();
    m_albumList.authorName   = (char**)calloc(m_albumList.size, sizeof(char*));
    m_albumList.albumName   = (char**)calloc(m_albumList.size, sizeof(char*));

    size = 0;
    for (i = 0; i < album_names.size(); ++i) {
        author_name    = music_server->getAlbumAuthor(album_names[i]);
        songs_at_album = music_server->getSongsByAlbum(album_names[i]);
        size += songs_at_album.size();
        m_albumList.authorName[i] = (char*)calloc(author_name.length() + 1, sizeof(char));
        strncpy(m_albumList.authorName[i], author_name.c_str(), author_name.length());
        m_albumList.albumName[i] = (char*)calloc(album_names[i].length() + 1, sizeof(char));
        strncpy(m_albumList.albumName[i], album_names[i].c_str(), album_names[i].length());
//        printf("m_albumName[%d] %s\n", i, m_albumList.albumName[i]);
    }

    songNames     = (char **)calloc(size, sizeof(char*));
    albumIndex    = (unsigned *)calloc(size, sizeof(unsigned));
    m_songNames     = (char **)calloc(size, sizeof(char*));
    m_albumIndex    = (unsigned *)calloc(size, sizeof(unsigned));

    k = 0;
    for (i = 0; i < m_albumList.size; ++i) {
        songs_at_album = music_server->getSongsByAlbum(album_names[i]);
        for (j = 0; j < songs_at_album.size(); ++j, ++k) {
            albumIndex[k] = i;
            songNames[k]  = (char*)calloc(songs_at_album[j].length() + 1, sizeof(char));
            strncpy(songNames[k], songs_at_album[j].c_str(), songs_at_album[j].length());
            coverflow_add();
//            printf("songName[k] %s\n", k, songNames[k]);
        }
    }

    k = size - 1;
    for (i = 0; i < 16; ++i) {
        for (j = i; j < size; j += 16, --k) {
            m_albumIndex[k] = albumIndex[j];
            m_songNames[k]  = songNames[j];
//            printf("m_songNames[%d] %s\n", k, m_songNames[k]);
        }
    }

    free(songNames);
    free(albumIndex);

    coverflow_set_sub_tex_index(m_albumIndex);
}

void CoverFlowActivity::releaseSongData()
{
    unsigned i;
    for (i = 0; i < m_albumList.size; ++i) {
        free(m_albumList.albumName[i]);
        free(m_albumList.authorName[i]);
        free(m_songNames[i]);
    }
    free(m_albumList.albumName);
    free(m_albumList.authorName);
    free(m_songNames);
    free(m_albumIndex);
}

HWND CoverFlowActivity::createHWND() {
    int x, y, w, h;
    HWND hWnd, hWndGL;
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
    CreateInfo.spCaption = "coverflow-gl-activity";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = proc;
    CreateInfo.lx =  ACTIVITY_X;
    CreateInfo.ty = ACTIVITY_Y;
    CreateInfo.rx = ACTIVITY_X + ACTIVITY_W;
    CreateInfo.by = ACTIVITY_Y + ACTIVITY_H;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hWnd = CreateMainWindow(&CreateInfo);

    w = ACTIVITY_W;
    h = ACTIVITY_W;
//    h = ACTIVITY_H / 2;
    x = 0;
    y = 0;
//    y = (ACTIVITY_H - h) / 2;
    hWndGL = CreateWindow(CTRL_STATIC, "gl-control", WS_VISIBLE, 10032,
            x, y, w, h,
            hWnd, 0);
    SetWindowCallbackProc(hWndGL, glProc);

    EGLInit(hWndGL);
    coverflow_init();
    coverflow_reshape(w, h);

    loadSongData();
    loadBitmapToGL("res/musicplayer/Album/album_map.png", m_albumList.size);

    return hWnd;
}
