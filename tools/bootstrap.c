#define dbg() printf("F:%s L:%d\n", __FUNCTION__, __LINE__)

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

/*
 * [Are you ready?]
 *    * --(TimeOut)--> [Debug mode]
 *       * --(Input  )--> [Calibrate] --> [Are you ready?]}
 *       * --(TimeOut)--> {AutoIAL}
 *    * --(Input  )--> {Mouse or Tslib}
 */
enum {
    STATE_INIT = 0,
    STATE_DEBUGMODE,
    STATE_CALIBRATE,
    STATE_RUN_MIN,
    STATE_RUN_AUTOIAL = STATE_RUN_MIN,
    STATE_RUN_TSLIB,
    STATE_RUN_MOUSE,
    STATE_RUN_MAX = STATE_RUN_MOUSE,
    STATE_NR
};

static int g_state = STATE_INIT; 
static int g_fd_ts = -1, g_fd_mouse = -1;
static HWND g_hwnd;

static char g_message[1024] = "hello";

static void set_message(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vsprintf(g_message, format, ap);
    va_end(ap);

    UpdateWindow(g_hwnd, FALSE);
}

static const char *get_message(void) {
    return g_message;
}

static void switch_to(int state) {
    // set_message("switch_to(%d)", state);
    g_state = state;
}

static int open_fd(void) {
    FILE *fp = fopen("/proc/bus/input/devices", "r");
    char buf[1024];
    int stat = 0; /* 0: init, 1: is ts, 2: is mouse, 3: is not ts/mouse */

    while (fgets(buf, sizeof(buf), fp)) {
        buf[sizeof(buf)/sizeof(buf[0])-1] = 0;
        if (buf[0] == '\n') {
            stat = 0;
        }else if (buf[0] == 'N') {
            if (strstr(buf, "s3c-keypad")) {
                stat = 3;
            }else if (strstr(buf, "S3C TouchScreen")) {
                stat = 1;
            }else{
                /* XXX: Others are mouse */
                stat = 2;
            }
        }else if (buf[0] == 'H') {
            switch (stat) {
                case 1:
                case 2:
                    {
                        char *p;
                        p = strtok(buf, " =\n");
                        while (p) {
                            if (strncmp(p, "event", 5) == 0) {
                                char path[PATH_MAX];
                                int *pfd = (stat == 1 ? &g_fd_ts : &g_fd_mouse);
                                sprintf(path, "/dev/input/%s", p);
                                *pfd = open(path, O_RDONLY);
                                if (*pfd < 0){
                                    set_message("Failed to open %s: %s", path, strerror(errno));
                                }else{
                                }
                                break;
                            }
                            p = strtok(NULL, " =\n");
                        }
                        if (! p){
                            set_message("Can find device node for %s", stat == 1 ? "TouchScreen" : "Mouse");
                        }
                    }
                    break;
                case 3:
                    break;
                case 0:
                default:
                    set_message("Unexpect line: %s", buf);
                    stat = 3;
                    break;
            }
            continue;
        }
    }
    fclose(fp);
    return (g_fd_ts >= 0 || g_fd_mouse >= 0) ? 0 : -1;
}

static void close_fd(void) {
    if (g_fd_ts >= 0) {
        close(g_fd_ts);
        g_fd_ts = -1;
    }else if (g_fd_mouse >= 0) {
        close(g_fd_mouse);
        g_fd_mouse = -1;
    }
}

static int do_select(int *pfd) {
    fd_set rset;
    struct timeval tv;
    int maxfd = -1;
    int ret;

    FD_ZERO(&rset);

    if (g_fd_ts >= 0) {
        FD_SET(g_fd_ts, &rset);
        if (g_fd_ts > maxfd) {
            maxfd = g_fd_ts;
        }
    }
    if (g_fd_mouse >= 0) {
        FD_SET(g_fd_mouse, &rset);
        if (g_fd_mouse > maxfd) {
            maxfd = g_fd_mouse;
        }
    }

    if (maxfd < 0) {
        return -1;
    }else{
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(maxfd+1, &rset, NULL, NULL, &tv);
        if (ret > 0) {
            if (g_fd_mouse >= 0 && FD_ISSET(g_fd_mouse, &rset)) {
                if (pfd) {
                    *pfd = g_fd_mouse;
                }
            }else if (g_fd_ts >= 0 && FD_ISSET(g_fd_ts, &rset)) {
                if (pfd) {
                    *pfd = g_fd_ts;
                }
            }else{
                ret = 0;
            }
        }
        return ret;
    }
}

static void process(HWND hWnd) {
    int count;
    int ret;

    for (;;) {
        switch (g_state) {
            case STATE_INIT:
                {
                    if (open_fd() < 0) {
                        switch_to(STATE_RUN_AUTOIAL);
                    }

                    for (count=5; count>0; --count) {
                        int fd;
                        ret = do_select(&fd);
                        if (ret < 0) {
                            set_message("select(): %s", strerror(errno));
                            switch_to(STATE_RUN_AUTOIAL);
                            break;
                        }else if (ret == 0) {
                            set_message("Are you ready? [%d]", count);
                            continue;
                        }else{
                            if (fd == g_fd_ts) {
                                switch_to(STATE_RUN_TSLIB);
                            }else{
                                switch_to(STATE_RUN_MOUSE);
                            }
                            break;
                        }
                    }

                    if (count <= 0) {
                        switch_to(STATE_DEBUGMODE);
                    }
                    break;
                }
            case STATE_DEBUGMODE:
                {
                    for (count=5; count>0; --count) {
                        ret = do_select(NULL);
                        if (ret < 0) {
                            set_message("select(): %s", strerror(errno));
                            switch_to(STATE_RUN_AUTOIAL);
                            break;
                        }else if (ret == 0) {
                            set_message("Debug mode [%d]", count);
                            continue;
                        }else{
                            switch_to(STATE_CALIBRATE);
                            break;
                        }
                    }

                    if (count <= 0) {
                        switch_to(STATE_RUN_AUTOIAL);
                    }
                }
                break;
            case STATE_CALIBRATE:
                {
                    pid_t pid;

                    close_fd();

                    pid = vfork();
                    if (pid < 0) {
                        assert(0);
                    }else if (pid == 0){
                        execlp("ts_calibrate", "ts_calibrate", NULL);
                        perror("execl()");
                        exit(-1);
                    }else{
                        for (;;) {
                            int status;
                            if (waitpid(pid, &status, 0) < 0) {
                                if (errno == EINTR) {
                                    continue;
                                }else{
                                    perror("waitpid()");
                                    assert(0);
                                    break;
                                }
                            }else{
                                switch_to(STATE_INIT);
                                break;
                            }
                        }
                    }
                }
                break;
            case STATE_RUN_AUTOIAL:
            case STATE_RUN_TSLIB:
            case STATE_RUN_MOUSE:
                {
                    close_fd();
                    return;
                }
                break;
            default:
                assert(0);
                break;
        }
    }
}

static int myproc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_CREATE:
            g_hwnd = hWnd;
            SetTimer(hWnd, 0x100, 1);
            break;
        case MSG_TIMER:
            {
                KillTimer(hWnd, 0x100);
                process(hWnd);
                SendNotifyMessage(hWnd, MSG_CLOSE, 0, 0);
            }
            break;
        case MSG_PAINT:
            {
                HDC hdc;
                RECT rc;
                hdc = BeginPaint (hWnd);

                GetClientRect(hWnd, &rc);
                SetBrushColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
                FillBox(hdc, 0, 0, RECTW(rc), RECTH(rc));

                SetTextColor(hdc, RGB2Pixel(hdc, 0x80, 0x80, 0x80));
                SetBkColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
                TextOut(hdc, 10, 10, get_message());

                EndPaint (hWnd, hdc);
            }
            return 0;
        case MSG_ERASEBKGND:
            return 0;
        case MSG_CLOSE:
            KillTimer (hWnd, 100);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int run_minigui (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    ShowCursor(FALSE);

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , argv[0] , 0 , 0);
#endif

    CreateInfo.dwStyle = 
        WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
    CreateInfo.spCaption = argv[0];
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = myproc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    ExitGUISafely(0);
    return 0;
}

static void run(int state) {
    char cmd[1024];

    assert(state >= STATE_RUN_MIN && state <= STATE_RUN_MAX);

    sprintf(cmd, "run-%d.sh", state - STATE_RUN_MIN);

    for (;;) {
        pid_t pid;
        pid = vfork();
        if (pid < 0) {
            assert(0);
        }else if (pid == 0){
            execlp(cmd, "cmd", NULL);
            perror("exec()");
            exit(-1);
        }else{
            for (;;) {
                int status;
                if (waitpid(pid, &status, 0) < 0) {
                    if (errno == EINTR) {
                        continue;
                    }else{
                        perror("waitpid()");
                        assert(0);
                        break;
                    }
                }else{
                    break;
                }
            }
        }
    }
}

int main(int argc, const char *argv[]) {
    InitGUI(argc, argv);
    run_minigui(argc, argv);
    TerminateGUI(0);

    if (g_state >= STATE_RUN_MIN && g_state <= STATE_RUN_MAX) {
        run(g_state);
        return 0;
    }else{
        return -1;
    }
}
