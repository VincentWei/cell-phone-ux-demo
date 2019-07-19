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
/**
 * \author Vincent Wei
 * \date 2018/04/01
 * 
 * \brief This file defines the external methods for MiniGUI COMMON NEWGAL engine.
 *
 \verbatim

    This file defines the external methods for MiniGUI COMMONLCD NEWGAL engine  
    and the COMM IAL engine. These two engines implement the communicate with 
    the Web Display Server in the following manner:

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
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

/* The pixel format */
#define COMMLCD_TRUE_RGB565      3
#define COMMLCD_TRUE_RGB8888     4

struct commlcd_info {
    short height;
    short width;        // Size of the screen
    short bpp;          // Depth (bits-per-pixel)
    short type;         // Pixel type
    short rlen;         // Length of one scan line in bytes
    void  *fb;          // Frame buffer
    short async_update;
};

#define USS_PATH    "/var/tmp/web-display-server"
#define USC_PATH    "/var/tmp/web-display-client-P%d"

#define USC_PERM    S_IRWXU            /* rwx for user only */

#define FT_VFBINFO      10
#define FT_PING         11
#define FT_PONG         12
#define FT_EVENT        13
#define FT_DIRTYPIXELS  14

struct _frame_header {
    int type;
    size_t payload_len;
    unsigned char payload[0];
};

static int _fd_socket;
static struct commlcd_info _lcd_info;

/* all methods for COMMLCD engine should return zero for success */
int __commlcd_drv_init (void)
{
    int len, retval;
    struct sockaddr_un unix_addr;

    /* create a Unix domain stream socket */
    if ((_fd_socket = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) {
        retval = 1;
        goto error;
    }

    /* fill socket address structure w/our address */
    memset (&unix_addr, 0, sizeof (unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf (unix_addr.sun_path, USC_PATH, getpid());
    len = sizeof (unix_addr.sun_family) + strlen (unix_addr.sun_path);

    unlink (unix_addr.sun_path);        /* in case it already exists */
    if (bind (_fd_socket, (struct sockaddr *) &unix_addr, len) < 0) {
        retval = 2;
        goto error;
    }

    if (chmod (unix_addr.sun_path, USC_PERM) < 0) {
        retval = 3;
        goto error;
    }

    /* fill socket address structure w/server's addr */
    memset (&unix_addr, 0, sizeof (unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy (unix_addr.sun_path, USS_PATH);
    len = sizeof (unix_addr.sun_family) + strlen (unix_addr.sun_path);

    if (connect (_fd_socket, (struct sockaddr *) &unix_addr, len) < 0) {
        retval = 4;
        goto error;
    }

    return 0;

error:
    _ERR_PRINTF ("__commlcd_drv_init: failed (%d)\n", retval);
    if (_fd_socket) {
        close (_fd_socket);
    }

    return retval;
}

#define SHADOW_SHM_KEY 0x4D475344 

/* return zero for success */
int __commlcd_drv_getinfo (struct commlcd_info *li, int width, int height, int bpp)
{
    ssize_t n;
    int retval = 0;
    struct _frame_header header;

    if (width > 1024 || height > 768 || bpp != 16 || bpp != 32) {
        retval = 1;
        goto error;
    }

    if (bpp == 16) {
        _lcd_info.type = COMMLCD_TRUE_RGB565;
        _lcd_info.rlen = (_lcd_info.width * 2 + 3)/4*4;
        _lcd_info.bpp = 16;
    }
    else {
        _lcd_info.type = COMMLCD_TRUE_RGB8888;
        _lcd_info.rlen = (_lcd_info.width * 4 + 3)/4*4;
        _lcd_info.bpp = 32;
    }

    _lcd_info.fb = calloc (height * _lcd_info.rlen, sizeof (char));
    if (_lcd_info.fb == NULL) {
        retval = 1;
        goto error;
    }

    header.type = FT_VFBINFO;
    header.payload_len = sizeof (struct commlcd_info);
    n = write (_fd_socket, &header, sizeof (struct _frame_header));
    n += write (_fd_socket, &_lcd_info, sizeof (struct commlcd_info));
    if (n != sizeof (struct _frame_header) + header.payload_len) {
        retval = 2;
        goto error;
    }

    _lcd_info.async_update = 0;
    memcpy (li, &_lcd_info, sizeof (struct commlcd_info));
    return retval;

error:
    _ERR_PRINTF ("__commlcd_drv_getinfo: failed (%d)\n", retval);
    if (_fd_socket) {
        close (_fd_socket);
    }

    return retval;
}

/* this method will be called synchronously */
int __commlcd_drv_update (const RECT* rc_dirty)
{
    int i, Bpp, dirty_w, dirty_h;
    unsigned char* pixels;
    ssize_t n;
    size_t bytes;
    struct _frame_header header;

    dirty_w = RECTWP (rc_dirty);
    dirty_h = RECTHP (rc_dirty);

    if (dirty_w < 0 || rc_dirty->left < 0 || (rc_dirty->left + dirty_w) > _lcd_info.width
            || dirty_h < 0 || rc_dirty->top < 0 || (rc_dirty->top + dirty_h) > _lcd_info.height) {
        return 1;
    }

    Bpp = 2;
    if (_lcd_info.bpp == 32)
        Bpp = 4;

    bytes = dirty_w * dirty_h * Bpp;

    header.type = FT_DIRTYPIXELS;
    header.payload_len = sizeof (RECT) + bytes;

    n = write (_fd_socket, &header, sizeof (struct _frame_header));
    n += write (_fd_socket, rc_dirty, sizeof (RECT));

    pixels = _lcd_info.fb + _lcd_info.rlen * rc_dirty->top + rc_dirty->left * Bpp;
    for (i = 0; i < dirty_h; i++) {
        n += write (_fd_socket, pixels, dirty_w * Bpp);

        pixels += _lcd_info.rlen;
    }

    if (n != header.payload_len) {
        _ERR_PRINTF ("__commlcd_drv_update: error on writting pixels: %zd (%zu)\n", n, header.payload_len);
    }

    return 0;
}

int __commlcd_drv_release (void)
{
    free (_lcd_info.fb);
    close (_fd_socket);

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
};

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
        *key = (short)_last_event.value1;
        *status = 1;
        break;

    case EVENT_KEYUP:
        *key = (short)_last_event.value1;
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

    retval = select (_fd_socket + 1, &rfds, NULL, NULL, timeout);

    if (retval > 0 && FD_ISSET (_fd_socket, &rfds)) {
        ssize_t n;
        struct _frame_header header;

        n = read (_fd_socket, &header, sizeof (struct _frame_header));
        if (n != sizeof (struct _frame_header)) {
            _ERR_PRINTF ("__comminput_wait_for_input: error on reading event: %zd\n", n);
        }
        switch (header.type) {
        case FT_PING:
            header.type = FT_PONG;
            header.payload_len = 0;
            n = write (_fd_socket, &header, sizeof (struct _frame_header));
            break;

        case FT_EVENT: {
            if (header.payload_len != sizeof (struct _remote_event)) {
                _ERR_PRINTF ("__comminput_wait_for_input: payload length does not matched the data type: %zu.\n", header.payload_len);
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

    return event_flag;
}

void __comminput_deinit (void)
{
}

