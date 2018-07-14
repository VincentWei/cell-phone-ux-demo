/**
 * \author Vincent Wei
 * \date 2018/04/01
 * 
 * \brief This file defines the external methods for MiniGUI COMMON NEWGAL engine.
 *
 \verbatim

    This file defines the external methods for MiniGUI COMMONLCD NEWGAL engine  
    and the COMM IAL engine. These two engines implement the communicate with 
    the MiniGUI remote server in the following manner:

    1. A websocket client connects to the server, tells the server the demo it want to run.
       It will also tell the server the resolution (width and height in pixels) to show the demo.

    2. The server forks and execute the MiniGUI program specified by the websocket client
       in the child. Before this, the server should set the following environment variables
       for the MiniGUI client.

        * MG_GAL_ENGINE: should be commonlcd
        * MG_DEFAULTMODE: should be a pattern like 320x240-16
        * MG_IAL_ENGINE: shold be common

    3. The server creates a UNIX domain socket at a well-known path (/var/tmp/mg-remote-sever)
       and wait for the connection from the MiniGUI client.

    4. The MiniGUI client connects to the UNIX socket, creates a shared memory as a
       virtual frame buffer, and sends the identifier of the shared memory to the server
       via the UNIX socket.

    5. Note that the resolution of the virtual frame buffer should be sent by the server as
       the first message. 

    6. The server accepts the connection request from the MiniGUI client, gets the
       identifier of the shared memory sent by the client via the UNIX socket,
       and attaches to the shared memory.

    7. The MiniGUI client sends the dirty rectangle information via the
       UNIX socket, and the server then sends the information and the pixel data
       to the websocket client.

    8. The server gets the input events from the websocket client and sends them
       to the MiniGUI client via the UNIX socket.

    9. Note that the MiniGUI client should run in MiniGUI-Processes or MiniGUI-Standalone
       runmode.

    Copyright (C) 2018, Beijing FMSoft Technologies Co., Ltd (www.fmsoft.cn).

 \endverbatim
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <semaphore.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "lcd_spi.h"

/* The pixel format */
#define COMMLCD_TRUE_RGB565      3
#define COMMLCD_TRUE_RGB8888     4

struct commlcd_info {
    short height, width;  // Size of the screen
    short bpp;            // Depth (bits-per-pixel)
    short type;           // Pixel type
    short rlen;           // Length of one scan line in bytes
    void  *fb;            // Frame buffer
};

/* all methods for COMMLCD engine should return zero for success */

#define SRV_PATH    "/var/tmp/mg-remote-server"
#define CLI_PATH    "/var/tmp/mg-remote-client-%d"
#define CLI_PERM    S_IRWXU            /* rwx for user only */

#define FT_MODE         1
    #define MAX_MODE        15
#define FT_IPCID        2

#define FT_PING         3
#define FT_PONG         4

#define FT_EVENT        5

#define FT_DIRTY        6
#define FT_ACK          7

struct _frame_header {
    int type;
    size_t payload_len;
    unsigned char payload[0];
}

static int _fd_socket;
static pthread_mutex_t _lock_socket;
static int _shmid_vfb;
static int _semid_vfb;
static struct commlcd_info _lcd_info;

/* Down/up a semaphore uninterruptablly. */
static void _sysvipc_sem_op (int semid, int sem_num, int value)
{
    struct sembuf sb;

again:
    sb.sem_num = sem_num;
    sb.sem_op = value;
    sb.sem_flg = 0;

    if (semop (semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

extern BOOL GAL_ParseVideoMode (const char* mode, int* w, int* h, int* depth);

int __commlcd_drv_init (void)
{
    int len, retval;
    struct sockaddr_un unix_addr;
    struct _frame_header header;
    ssize_t n;
    char mode [MAX_MODE + 1];
    union semun sunion;

    /* create a Unix domain stream socket */
    if ((_fd_socket = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) {
        retval = 1;
        goto error;
    }

    /* fill socket address structure w/our address */
    memset (&unix_addr, 0, sizeof (unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf (unix_addr.sun_path, CLI_PATH, getpid());
    len = sizeof (unix_addr.sun_family) + strlen (unix_addr.sun_path);

    unlink (unix_addr.sun_path);        /* in case it already exists */
    if (bind (_fd_socket, (struct sockaddr *) &unix_addr, len) < 0) {
        retval = 2;
        goto error;
    }

    if (chmod (unix_addr.sun_path, CLI_PERM) < 0) {
        retval = 3;
        goto error;
    }

    /* fill socket address structure w/server's addr */
    memset (&unix_addr, 0, sizeof (unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy (unix_addr.sun_path, SRV_PATH);
    len = sizeof (unix_addr.sun_family) + strlen (unix_addr.sun_path);

    if (connect (_fd_socket, (struct sockaddr *) &unix_addr, len) < 0) {
        retval = 4;
        goto error;
    }

    n = read (_fd_socket, &header, sizeof (struct _frame_header));
    if (n < sizeof (struct _frame_header) || header.type != FT_MODE) {
        retval = 5;
        goto error;
    }

    if (header.payload_len > MAX_LEN_MODE) {
        retval = 6;
        goto error;
    }

    memset (mode, 0, sizeof (mode));
    n = read (_fd_socket, mode, header.payload_len);
    if (n < header.payload_len) {
        retval = 7;
        goto error;
    }

    if (!GAL_ParseVideoMode (mode, &_lcd_info.width, &_lcd_info.height, &_lcd_info.bpp)) {
        retval = 8;
        goto error;
    }

    if (_lcd_info.width > 640 || _lcd_info.height > 640 || _lcd_info.bpp != 16 || _lcd_info.bpp != 32) {
        retval = 9;
        goto error;
    }

    if (_lcd_info.bpp == 16) {
        _lcd_info.type = COMMLCD_TRUE_RGB565;
        _lcd_info.rlen = (_lcd_info.width * 2 + 3)/4*4;
    }
    else {
        _lcd_info.type = COMMLCD_TRUE_RGB8888;
        _lcd_info.rlen = (_lcd_info.width * 4 + 3)/4*4;
    }

    /* create a shared memory as the virtual frame buffer */
    _shmid_vfb = shmget (IPC_PRIVATE, _lcd_info.rlen * _lcd_info.height, IPC_CREAT | 0666 | IPC_EXCL);
    if (_shmid_vfb == -1){
        retval = 10;
        goto error;
    }

    _lcd_info.fb = shmat (_shmid_vfb, 0, 0); 

    header.type = FT_IPCID;
    header.payload_len = sizeof (_shmid_vfb);
    n = write (_fd_socket, &header, sizeof (struct _frame_header));
    n = write (_fd_socket, &_shmid_vfb, sizeof (int));

    _semid_vfb = semget (IPC_PRIVATE, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if (_semdi_vfb < 0) {
        retval = 11;
        goto error;
    }
    
    sunion.val = 1;
    semctl (_semid_vfb, 0, SETVAL, sunion);

    header.type = FT_IPCID;
    header.payload_len = sizeof (_semid_vfb);
    n = write (_fd_socket, &header, sizeof (struct _frame_header));
    n = write (_fd_socket, &_semid_vfb, sizeof (int));

    pthread_mutex_init (&_lock_socket, NULL);
    return 0;

error:
    _ERR_PRINTF ("__commlcd_drv_init: failed (%d)\n", retval);
    if (_lcd_info.fb != NULL) {
        shmdt (_lcd_info.fb);
    }

    if (_fd_socket) {
        close (_fd_socket);
    }

    return retval;
}

#define SHADOW_SHM_KEY 0x4D475344 

/* return zero for success */
int __commlcd_drv_getinfo (struct commlcd_info *li)
{
    memcpy (li, &_lcd_info, sizeof (struct commlcd_info));
    return 0;
}

/* this method will be called async */
int __commlcd_drv_update (const RECT* rc_dirty)
{
    ssize_t n;
    struct _frame_header header;

    pthread_mutex_lock (&_lock_socket);
    //_sysvipc_sem_op (_semid_vfb, 0, -1);

    header.type = FT_DIRTY;
    header.payload_len = sizeof (RECT);
    n = write (_fd_socket, &header, sizeof (struct _frame_header));
    n = write (_fd_socket, rc_dirty, sizeof (RECT));

    n = read (_fd_socket, &header, sizeof (struct _frame_header));
    if (header.type != FT_ACK) {
        _ERR_PRINTF ("__commlcd_drv_update: ACK expected, but got type %d.\n", header.type);
    }

    //_sysvipc_sem_op (_semid_vfb, 0, 1);
    pthread_mutex_unlock (&_lock_socket);
    return 0;
}

int __commlcd_drv_release (void)
{
    union semun ignored;

    pthread_mutex_destroy (&_lock_socket);
    shmdt (_lcd_info.fb);
    close (_fd_socket);
    
    shmctl (_shmid_vfb, IPC_RMID, NULL);
    semctl (_semid_vfb, 0, IPC_RMID, ignored);
    return 0;
}

int (*__commlcd_drv_setclut) (int firstcolor, int ncolors, GAL_Color *colors) = NULL;

#define COMM_MOUSEINPUT    0x01
#define COMM_KBINPUT       0x02

#define COMM_MOUSELBUTTON  0x01
#define COMM_MOUSERBUTTON  0x04

int __comminput_init (void)
{
    return 0;
}

#define EVENT_NULL          0
#define EVENT_MOUSEMOVE     1
#define EVENT_LBUTTONDOWN   2
#define EVENT_LBUTTONUP     3

#define EVENT_KEYDOWN       11
#define EVENT_KEYUP         12

struct _remote_event {
    int type;
    int value1;
    int value2;
}

static struct _remote_event _last_event;

/* return 0 when there is really a touch event */
int __comminput_ts_getdata (short *x, short *y, short *button)
{
    switch (_last_event.type) {
    case EVENT_MOUSEMOVE:
        *x = _last_event.value1;
        *y = _last_event.value2;
        break;

    case EVENT_LBUTTONDOWN:
        *button = COMM_MOUSELBUTTON;
        break;

    case EVENT_LBUTTONUP:
        *button = 0;
        break;

    default:
        return -1;
    }

    memset (&_last_event, 0, sizeof (struct _remote_event));
    return 0;
}

/* return 0 when there is really a key event */
int __comminput_kb_getdata (short *key, short *status)
{
    switch (_last_event.type) {
    case EVENT_KEYDOWN:
        *key = (short)value1;
        *status = 1;
        break;

    case EVENT_KEYUP:
        *key = (short)value1;
        *status = 0;
        break;

    default:
        return -1;
    }

    memset (&_last_event, 0, sizeof (struct _remote_event));
    return 0;
}

int __comminput_wait_for_input (struct timeval *timeout)
{
    fd_set rfds;
    int retval;
    int event_flag = 0;

    FD_ZERO (&rfds);
    FD_SET (_fd_socket, &rfds);

    pthread_mutex_lock (&_lock_socket);
    retval = select (_fd_socket + 1, &rfds, NULL, NULL, timeout);

    if (retval > 0 && FD_ISSET (_fd_socket, &rfds)) {
        ssize_t n;
        struct _frame_header header;

        n = read (_fd_socket, &header, sizeof (struct _frame_header));
        switch (header.type) {
        case FT_PING:
            header.type = FT_PONG;
            header.payload_len = 0;
            n = write (_fd_socket, &header, sizeof (struct _frame_header));
            break;

        case FT_EVENT: {
            if (header.payload_len != sizeof (struct _remote_event)) {
                _ERR_PRINTF ("__comminput_wait_for_input: payload length does not matched the frame type: %d.\n", header.payload_len);
                break;
            }

            n = read (_fd_socket, &_last_event, sizeof (struct _remote_event));
            switch (_last_event.type) {
            case EVENT_MOUSEMOVE:
            case EVENT_LBUTTONDOWN:
            case EVENT_LBUTTONUP:
                event_flag |= COMM_MOUSEINPUT;
                break;
            case EVENT_KEYDOWN:
            case EVENT_KEYUP:
                event_flag |= COMM_KBINPUT;
                break;
            }
            break;
        }
        default:
            _ERR_PRINTF ("__comminput_wait_for_input: FT_PING or FT_EVENT expected, but got type %d.\n", header.type);
            break;
        }
    }
    else if (retval < 0) {
        event_flag = -1;
    }

    pthread_mutex_unklock (&_lock_socket);
    return event_flag;
}

void __comminput_deinit (void)
{
}

