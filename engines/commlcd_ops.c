/**
 * \author Vincent Wei
 * \date 2018/04/01
 * 
 * \brief This file defines the external methods for MiniGUI COMMON NEWGAL engine.
 *
 \verbatim

    This file defines the external methods for MiniGUI COMMONLCD NEWGAL engine.  
    This engine and the COMM IAL engine communicate with the MiniGUI remote server
    in the following manner:

    1. A websocket client connects to the server, tells the server the demo it want to run.
       It will tell the server the resolution (width and height in pixels) to show the demo.

    2. The server forks and execute the MiniGUI program specified by the websocket client
       in the child. Before this, the server will set the following environment variables
       for the resolution requested by the websocket client.

        * MG_GAL_ENGINE: should be commonlcd
        * MG_DEFAULTMODE: should be a pattern like 320x240-16
        * MG_IAL_ENGINE: shold be common

    3. The server creates a UNIX domain socket at a well-known path (/var/run/mg-remote-sever)
       and wait for the connection from the MiniGUI client.

    4. The MiniGUI client connects to the UNIX socket, creates a shared memory as a
       virtual frame buffer, and sends the identifier of the shared memory to the server
       via the UNIX socket.

    5. Note that the resolution of the virtual frame buffer was set by the server.
       The MiniGUI client gets the information via environment variables (MG_DEFAULTMODE).

    6. The server accepts the connection request from the MiniGUI client, gets the
       identifier of the shared memory sent by the client via the UNIX socket,
       and attaches to the shared memory.

    7. The MiniGUI client sends the dirty rectangle information via the
       UNIX socket, and the server then send the information and the pixel data
       to the websocket client.

    8. The server gets the input events from the websocket client and sends them
       to the MiniGUI client via the UNIX socket.

    9. Note that the MiniGUI client should run in MiniGUI-Processes or MiniGUI-Standalone
       run mode.

    Copyright (C) 2018, Beijing FMSoft Technologies Co., Ltd (www.fmsoft.cn).

 \endverbatim
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/types.h>
#include <linux/ioctl.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "lcd_spi.h"

/* The pixel format defined by depth */
#define COMMLCD_PSEUDO_RGB332    1
#define COMMLCD_TRUE_RGB555      2

#define COMMLCD_TRUE_RGB565      3
#define COMMLCD_TRUE_RGB888      4
#define COMMLCD_TRUE_RGB0888     5

#define COMMLCD_TRUE_BGR565      6
#define COMMLCD_TRUE_BGR888      7

struct commlcd_info {
    short height, width;  // Size of the screen
    short bpp;            // Depth (bits-per-pixel)
    short type;           // Pixel type
    short rlen;           // Length of one scan line in bytes
    void  *fb;            // Frame buffer
};

static int _fd_lcd;
static void* _mem_vfb;
static struct _lcd_spi_info _lcd_info;

/* all methods for COMMLCD engine should return zero for success */

int __commlcd_drv_init (void)
{
    _fd_lcd = open ("/dev/" LCDSPIDEV_NAME, O_RDWR);
    if (_fd_lcd < 0) {
        perror ("WARNING > CommLCD: failed to open SPI-LCD device /dev/" LCDSPIDEV_NAME);
        return 1;
    }

    return 0;
}

/* return zero for success */
int __commlcd_drv_getinfo (struct commlcd_info *li)
{
    if (ioctl (_fd_lcd, LCD_SPI_GET_INFO, &_lcd_info) == 0) {
        fprintf (stderr, "DEBUG > CommLCD > LCD Display ID:          %02X, %02X, %02X.\n",
                (unsigned)_lcd_info.display_id[1], (unsigned)_lcd_info.display_id[2], (unsigned)_lcd_info.display_id[3]);
        fprintf (stderr, "DEBUG > CommLCD > LCD module name:         %s.\n", _lcd_info.lcd_module);
        fprintf (stderr, "DEBUG > CommLCD > LCD contoller name:      %s.\n", _lcd_info.lcd_ctrller);
        fprintf (stderr, "DEBUG > CommLCD > LCD resolution:          %u X %u.\n", (unsigned)_lcd_info.res_x, (unsigned)_lcd_info.res_y);
        fprintf (stderr, "DEBUG > CommLCD > LCD pixel format:        %u.\n", (unsigned)_lcd_info.pixel_format);
        fprintf (stderr, "DEBUG > CommLCD > LCD line pitch:          %u.\n", _lcd_info.line_pitch);
        fprintf (stderr, "DEBUG > CommLCD > LCD size of virtual FB:  %u.\n", _lcd_info.size_vfb);
        if (_lcd_info.pixel_format != LCDSPIDEV_PIXEL_RGB565) {
            fprintf (stderr, "WARING > CommLCD: only supports the pixel format LCDSPIDEV_PIXEL_RGB565 (%u).\n", LCDSPIDEV_PIXEL_RGB565);
            return 2;
        }
    }
    else {
        perror ("WARING > CommLCD: failed to get info of SPI-LCD");
        return 3;
    }

    _mem_vfb = mmap (0, _lcd_info.size_vfb, PROT_READ | PROT_WRITE, MAP_SHARED, _fd_lcd, 0);
    if (_mem_vfb == (void*)-1) {
        perror ("WARING > CommLCD: failed to mmap the virtual frame buffer of SPI-LCD");
        close (_fd_lcd);
        return 4;
    }

    li->type = COMMLCD_TRUE_RGB565;
    li->height = _lcd_info.res_y;
    li->width = _lcd_info.res_x;
    li->bpp = 16;
    li->rlen = _lcd_info.line_pitch;
    li->fb = _mem_vfb;
    return 0;
}

/* this method will be called async */
int __commlcd_drv_update (const RECT* rc_dirty)
{
    struct _lcd_spi_sync_window window;
    window.xs = rc_dirty->left;
    window.ys = rc_dirty->top;
    window.xe = rc_dirty->right - 1;
    window.ye = rc_dirty->bottom - 1;

    if (ioctl (_fd_lcd, LCD_SPI_SYNC_WINDOW, &window)) {
        perror ("WARING > CommLCD: failed to sync window");
    }

    return 0;
}

int __commlcd_drv_release (void)
{
    munmap (_mem_vfb, _lcd_info.size_vfb);
    close (_fd_lcd);
    return 0;
}

int (*__commlcd_drv_setclut) (int firstcolor, int ncolors, GAL_Color *colors) = NULL;

