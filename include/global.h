/*!============================================================================
 * @file global.h 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  GLOBAL_INC
#define  GLOBAL_INC
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "ResourceManager.hh"
#define DESKTOP_IMG_PATH    "res/lockscreen/background.png"
#define DEBUG

#ifdef DEBUG
#   define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)
#else
#   define dbg() /* NULL */
#endif

#include <stdio.h>
#ifndef WIN32 /* UNIX */
#   include <sys/times.h>
#endif

#define HAVE_PAGESCROLL

static inline unsigned int MyGetTickCount() {
#ifdef WIN32
    unsigned int GetTickCount(void);
    return GetTickCount();
#else
    return (unsigned int) times(NULL);
#endif
}

#define MAX_PAGE_NUMBER 10
#define COLUMNS_PER_PAGE 4
#define ROWS_PER_PAGE 4
#define ITEMS_PER_PAGE 16

#define SCREEN_W       360
#define SCREEN_H       480
#define TITLE_X        0
#define TITLE_Y        0
#define TITLE_H        22
#define TITLE_W        SCREEN_W
#define ACTIVITY_X     0
#define ACTIVITY_Y     22
#define ACTIVITY_W     SCREEN_W
#define ACTIVITY_H     (SCREEN_H - TITLE_H)

#define SCREEN_IDLE_TIMER	(30*100)

#define PAGEBAR_Y      (ACTIVITY_H - DESKTOP_TOOLBAR_HEIGHT - 10)
#define PAGEBAR_R      2
#define PAGEBAR_DISTANCE 10

/* useless */
#define BUTTON_W       80
#define BUTTON_H       36
#define BUTTON_X(i)    (BUTTON_W * (i))
#define BUTTONBAR_Y    (ACTIVITY_H - BUTTONBAR_H)
#define BUTTONBAR_H    36

/* this is control id please repalce it. */
#define IDC_STATIC_TITLE   100
#define IDC_STATIC_BAR     101
#define IDC_FIRST_BUTTON   102
#define IDC_BUTTON_LEFT    IDC_FIRST_BUTTON
#define IDC_BUTTON_MID     (IDC_BUTTON_LEFT + 1)
#define IDC_BUTTON_RIGHT   (IDC_BUTTON_LEFT + 2)
#define IDC_DESKTOP_BODY   1

#define APP_NAME_SHOOT "Shoot"
#define APP_NAME_PICTUREPREVIEWER "PicturePreviewer"
#define APP_NAME_DIAL  "Dial"
#define APP_NAME_PHONEBOOK "PhoneBook"
#define APP_NAME_MUSICALBUM "MusicAlbum"
#define APP_NAME_MUSICLIST "MusicList"
#define APP_NAME_PICTUREVIEWER      "PictureViewer"
#define APP_NAME_DESKTOP "ActivityDesktop"

// action
#define ACTION_MAIN     0       // main action (generally means starting from the desktop)
#define ACTION_VIEW     1       // view action (view from other APP)
#define ACTION_EDIT     2       // edit action (edit from other APP)

#define MSG_USER_APP_START (MSG_USER + 10)
#define MSG_USER_APP_STOP  (MSG_USER + 11)
#define MSG_USER_RETURN_TO_DESKTOP  (MSG_USER + 12)
#define MSG_USER_RETURN_KEY_PRESSED (MSG_USER + 13)
#define MSG_USER_ITEM_CLICKED (MSG_USER + 14)
#define MSG_USER_APP_DATA (MSG_USER + 15)
#define MSG_USER_IDLE	(MSG_USER + 16)
#define MSG_USER_CLICK	(MSG_USER + 17)
#define MSG_USER_BACK	(MSG_USER + 18)
#define MSG_USER_RETURN_TO_STANDBY  (MSG_USER + 19)

#define MSG_USER_STATEMACHINE_EVENT   (MSG_USER + 111)
#ifndef BITBLT
#define BITBLT(shdc, sx, sy, sw, sh, dhdc, dx, dy, rop) \
    do { \
        if (0 != sw && 0 != sh) BitBlt(shdc, sx, sy, sw, sh, dhdc, dx, dy, rop); \
    } while(0)
#endif

#ifndef STRETCHBLT
#define STRETCHBLT(shdc, sx, sy, sw, sh, dhdc, dx, dy, dw, dh, rop) \
    do { \
        if (0 != sw && 0 != sh && 0 != dw && 0 != dh)   \
        StretchBlt(shdc, sx, sy, sw, sh, dhdc, dx, dy, dw, dh, rop); \
    } while(0)
#endif

#define HAVE_APP_INOUT_ANIMATION 0

#define SYSTEM_DB_NAME "etc/system.db"
#define SYSTEM_CONTENT_HEAD "content://system/"
#define SETTINGS_PROVIDER   "settings"
#define CONTACTS_PROVIDER   "contacts"
#define SMS_PROVIDER   "sms"
#define MEDIASTORE_PROVIDER "mediastore"
#define CALLLOG_PROVIDER "callLog"
#define APPSINFO_PROVIDER "AppsInfo"

#endif   /* ----- #ifndef GLOBAL_INC  ----- */

