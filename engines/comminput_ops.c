/**
 * \author Vincent Wei
 * \date 2018/07/07
 *
 * \brief This file defines the external methods for MiniGUI CommInput IAL engine.
 *
 \verbatim

    This file defines the external methods for MiniGUI CommLCD NEWGAL engine and CommInput IAL engine.

    Copyright (C) 2018, Beijing FMSoft Technologies Co., Ltd (www.fmsoft.cn).

 \endverbatim
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#define COMM_MOUSEINPUT    0x01
#define COMM_KBINPUT       0x02

#define COMM_MOUSELBUTTON  0x01
#define COMM_MOUSERBUTTON  0x04

#define KB_EVENT_DEV "/dev/input/event1"
#define TP_EVENT_DEV "/dev/input/event5"

static int sg_kb_event_fd;
static int sg_tp_event_fd;

int __comminput_init (void)
{
    sg_kb_event_fd = open (KB_EVENT_DEV, O_RDWR);
    if (sg_kb_event_fd < 0) {
        perror ("WARNING > __comminput_init: Failed when opening key-button event device file");
        return -1;
    }

    sg_tp_event_fd = open (TP_EVENT_DEV, O_RDWR /*| O_NONBLOCK */);
    if (sg_tp_event_fd < 0) {
        perror ("WARNING > __comminput_init: Failed when opening touch-panel event device file");
        close (sg_kb_event_fd);
        return -1;
    }

    return 0;
}

/* return 0 when there is really a touch event */
int __comminput_ts_getdata (short *x, short *y, short *button)
{
    static struct input_event data;

    while (read (sg_tp_event_fd, &data, sizeof (data)) == sizeof (data)) {
        if (data.type == EV_SYN && data.code == SYN_REPORT) {
            break;
        }

        if (data.type == EV_KEY) {
            switch (data.code) {
            case BTN_TOUCH:
                if (data.value > 0)
                    *button = COMM_MOUSELBUTTON;
                else
                    *button = 0;
                break;

            default:
                fprintf (stderr, "WARNING > __comminput_ts_getdata: unknow event code for EV_KEY event: %x, %x\n", data.code, data.value);
                return -1;
            }
        }
        else if (data.type == EV_ABS) {
            switch (data.code) {
            case ABS_MT_POSITION_X:
                *x = data.value;
                break;

            case ABS_MT_POSITION_Y:
                *y = data.value;
                break;

            case ABS_MT_TOUCH_MAJOR:
                if (data.value > 0)
                    *button = COMM_MOUSELBUTTON;
                else
                    *button = 0;
                break;

            case ABS_MT_WIDTH_MAJOR:
                break;

			case ABS_MT_TRACKING_ID:
				break;

            case ABS_X:
                *x = data.value;
                break;

            case ABS_Y:
                *y = data.value;
                break;

            case ABS_PRESSURE:
                if (data.value > 0)
                    *button = COMM_MOUSELBUTTON;
                else
                    *button = 0;
                break;

            default:
                fprintf (stderr, "WARNING > __comminput_ts_getdata: unknow event code for EV_ABS event: %x, %x.\n", data.code, data.value);
                return -1;
            }
        }
//        else {
//            fprintf (stderr, "WARNING > __comminput_ts_getdata: unknow event type for touch panel. %x, %x %x\n", data.type, data.code, data.value);
//        }
    }

    return 0;
}

/* return 0 when there is really a key event */
int __comminput_kb_getdata (short *key, short *status)
{
    size_t size_read;
    static struct input_event data;

    /* reset to null status */
    *key = 0; *status = 0;

    size_read = read (sg_kb_event_fd, &data, sizeof (data));
    if (size_read == sizeof (data) && data.type == EV_KEY) {
        if (data.code > 128) {
            _DBG_PRINTF ("WARNING > __comminput_kb_getdata: key code > 128 returned.\n");
        }
		else if (data.code == 115) {
			*key = SCANCODE_ESCAPE;
            *status = data.value;
            _DBG_PRINTF ("INFO > __comminput_kb_getdata: SCANCODE_ESCAPE %d.\n", data.value);
            return 0;
		}
    }
    else {
        _DBG_PRINTF ("WARNING > __comminput_kb_getdata: unknow event type for key button.\n");
    }

    return -1;
}

int __comminput_wait_for_input (struct timeval *timeout)
{
    fd_set rfds;
    int retval;
    int event_flag = 0;

    FD_ZERO (&rfds);
    FD_SET (sg_kb_event_fd, &rfds);
    FD_SET (sg_tp_event_fd, &rfds);

    retval = select (MAX (sg_kb_event_fd, sg_tp_event_fd) + 1, &rfds, NULL, NULL, timeout);

    if (retval > 0) {
        if (FD_ISSET (sg_kb_event_fd, &rfds)) {
            event_flag |= COMM_KBINPUT;
        }

        if (FD_ISSET (sg_tp_event_fd, &rfds)) {
            event_flag |= COMM_MOUSEINPUT;
        }
    }
    else if (retval < 0) {
        event_flag = -1;
    }

    return event_flag;
}

void __comminput_deinit (void)
{
    close (sg_tp_event_fd);
    close (sg_kb_event_fd);
}

