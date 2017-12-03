/*!============================================================================
 * @file ResourceManager.cc 
 * @Synopsis Resource Manager source. 
 * @author WangXuBin
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */
#include "ResourceManager.hh"

static HDC s_hw_memRefDC = HDC_INVALID;
static HDC s_sw_memRefDC = HDC_INVALID;

static HDC s_hw_mem32RefDC = HDC_INVALID;
//static HDC s_sw_mem32RefDC = HDC_INVALID;

void Init32MemDC (void)
{
#ifdef USE_32HW_MEMDC
    s_hw_memRefDC = CreateMemDC(1, 1, 32, MEMDC_FLAG_HWSURFACE,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
#else
    s_hw_memRefDC = HDC_SCREEN;
#endif
#ifdef USE_32SW_MEMDC
    s_sw_memRefDC = CreateMemDC(1, 1, 32, MEMDC_FLAG_SWSURFACE,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
#else
    s_sw_memRefDC = HDC_SCREEN;
#endif

    s_hw_mem32RefDC = CreateMemDC(1, 1, 32, MEMDC_FLAG_HWSURFACE,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
}

void Release32MemDC (void)
{
#ifdef USE_32HW_MEMDC
    if (s_hw_memRefDC != HDC_INVALID)
        DeleteMemDC (s_hw_memRefDC);
    if (s_sw_memRefDC != HDC_INVALID)
        DeleteMemDC (s_hw_memRefDC);
#endif
    if (s_hw_mem32RefDC != HDC_INVALID)
        DeleteMemDC (s_hw_mem32RefDC);
}

#ifndef COMP_HW_MEMD
HDC CreateMyCompatibleDC(HDC hdc, int w, int h)
{
    HDC ret = HDC_INVALID;
    int depth = GetGDCapability(hdc, GDCAP_BITSPP);
    int Amask = GetGDCapability(hdc, GDCAP_AMASK);
    int Rmask = GetGDCapability(hdc, GDCAP_RMASK);
    int Gmask = GetGDCapability(hdc, GDCAP_GMASK);
    int Bmask = GetGDCapability(hdc, GDCAP_BMASK);

    ret = CreateMemDC(w, h, depth, MEMDC_FLAG_SWSURFACE,
            Rmask, Gmask, Bmask, Amask);
    return ret;
}
#endif

HDC Get32MemDC (void)
{
    return s_hw_memRefDC;
}

HDC Get32SwMemDC (void)
{
    return s_sw_memRefDC;
}


void* Load32Resource (const char* res_name, int type, DWORD usr_param)
{
    HDC hdc = s_hw_memRefDC;

    if (hdc == HDC_INVALID) {
        hdc = HDC_SCREEN;
    }

    return LoadResource (res_name, type, (DWORD) hdc);
}

int StepLoadRes (RES_NODE *node_list, int step)
{
    int cnt;

    RES_NODE *res_node = node_list;

    cnt = 0;

#ifdef DEBUG
    printf ("start load resource.\n");
#endif

    for (;;) {
        if (res_node->res_name == NULL) {
            break;
        }

        if (res_node->flag == 1) {
            if ((step == 0) || (res_node->step == step)) {
                if (res_node->bits == 32) {
                    LoadResource (res_node->res_name, res_node->type, (DWORD) s_hw_mem32RefDC);
                }
                else {
                    Load32Resource (res_node->res_name, res_node->type, (DWORD) HDC_SCREEN);
                }

                cnt++;
#ifdef DEBUG
                printf ("\tload %s\n", res_node->res_name);
#endif
            }
        } /* if */

        res_node++;
    } /* for */

#ifdef DEBUG
    printf ("end load resource.(%d)\n", cnt);
#endif
    return cnt;
}

int UnLoadAllRes (RES_NODE *node_list)
{
    int cnt;

    RES_NODE *res_node = node_list;

    cnt = 0;

#ifdef DEBUG
    printf ("start unload resource.\n");
#endif

    for (;;) {
        if (res_node->res_name == NULL) {
            break;
        }

        if (res_node->flag == 1) {
            ReleaseRes (Str2Key(res_node->res_name));
            cnt++;
#ifdef DEBUG
            printf ("\tunload %s\n", res_node->res_name);
#endif
        } /* if */

        res_node++;
    } /* for */

#ifdef DEBUG
    printf ("end unload resource.(%d)\n", cnt);
#endif
    return cnt;
}

RES_NODE res_list[] =
{
    /* res */
    /* res/address-book */
    { "res/address-book/address_book_add.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/address-book/address_book_calllog.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/address-book/address_book_contacts.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/address-book/address_book_dial.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/address-book/address_book_photo.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/address-book/address_book_unknow.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/address-book/photo01big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo01.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo02big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo02.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo03big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo03.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo04big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo04.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo05big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo05.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo06big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo06.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo07big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo07.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo08big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo08.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo09big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo09.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo10big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo10.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo11big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo11.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo12big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo12.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo13big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo13.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo14big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo14.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/address-book/photo15big.png", RES_TYPE_IMAGE, 16, 0, 3 },
    { "res/address-book/photo15.png", RES_TYPE_IMAGE, 16, 1, 5 },
    /* res/camera */
//    { "res/camera/animation.gif", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/camera/play_normal.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/camera/play_pressed.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/camera/shoot_normal.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/camera/shoot_pressed.png", RES_TYPE_IMAGE, 16, 1, 6 },
    /* res/common */
    { "res/common/indicator_green_click.png", RES_TYPE_IMAGE, 16, 1, 8 },
    { "res/common/indicator_green_normal.png", RES_TYPE_IMAGE, 16, 1, 8 },
    { "res/common/toptitle.png", RES_TYPE_IMAGE, 16, 1, 8 },
    /* res/desktop */
    { "res/desktop/calc_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/calc.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/calc_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/camera_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/camera.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/camera_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/contacts_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/contacts.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/contacts_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/date_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/date.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/date_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/dial_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/dial.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/dial_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/games_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/games.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/games_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/internet_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/internet.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/internet_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/music_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/music.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/music_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/picture_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/picture.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/picture_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/settings_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/settings.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/settings_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/sms_left.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/sms.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/desktop/sms_right.png", RES_TYPE_IMAGE, 16, 1, 3 },
    /* res/dial */
    { "res/dial/callrecord.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/dial/contact.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/dial/dialbks.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/dial/dialcall.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/dial/dialpad.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/dial/dialplus.png", RES_TYPE_IMAGE, 16, 1, 4 },
    /* res/font */
//    { "res/font/Helvetica.ttf", RES_TYPE_IMAGE, 16, 1, 3 },
    /* res/gvfb */
//    { "res/gvfb/phone_frame.png", RES_TYPE_IMAGE, 16, 1, 3 },
//    { "res/gvfb/phone_frame_pressed.png", RES_TYPE_IMAGE, 16, 1, 3 },
//    { "res/gvfb/phone_frame_up.png", RES_TYPE_IMAGE, 16, 1, 3 },
    /* res/lockscreen */
    { "res/lockscreen/background.png", RES_TYPE_IMAGE, 16, 0, 1 },
    { "res/lockscreen/bubble_btn1.png", RES_TYPE_IMAGE, 16, 0, 1 },
    { "res/lockscreen/bubble_btn2.png", RES_TYPE_IMAGE, 16, 0, 1 },
    { "res/lockscreen/main_scr.png", RES_TYPE_IMAGE, 16, 0, 1 },
    { "res/lockscreen/phone.png", RES_TYPE_IMAGE, 16, 0, 1 },
    { "res/lockscreen/red_area.png", RES_TYPE_IMAGE, 16, 0, 1 },
    { "res/lockscreen/sms_icon.png", RES_TYPE_IMAGE, 16, 0, 1 },
    /* res/musicplayer */
    /* res/musicplayer/Album */
    { "res/musicplayer/Album/big01.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/musicplayer/Album/big02.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/musicplayer/Album/big03.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/musicplayer/Album/big04.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/musicplayer/Album/big05.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/musicplayer/Album/big06.png", RES_TYPE_IMAGE, 16, 1, 5 },
    { "res/musicplayer/Album/big07.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/musicplayer/Album/big08.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/musicplayer/Album/big09.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/musicplayer/Album/big10.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/musicplayer/Album/big11.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/musicplayer/Album/big12.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/musicplayer/Album/default.png", RES_TYPE_IMAGE, 16, 1, 4 },
    /* res/musicplayer/player */
    { "res/musicplayer/player/CD.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/musicplayer/player/image.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/musicplayer/player/next.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/musicplayer/player/pause.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/musicplayer/player/play.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/musicplayer/player/prev.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/musicplayer/player/titlebutton.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/musicplayer/player/title.png", RES_TYPE_IMAGE, 16, 1, 4 },
    /* res/phone-call */
    { "res/phone-call/handsfree.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/phone-call/keyboard.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/phone-call/mute.png", RES_TYPE_IMAGE, 16, 1, 6 },
    { "res/phone-call/handsfree_32.png", RES_TYPE_IMAGE, 32, 1, 6 },
    { "res/phone-call/keyboard_32.png", RES_TYPE_IMAGE, 32, 1, 6 },
    { "res/phone-call/mute_32.png", RES_TYPE_IMAGE, 32, 1, 6 },
    { "res/phone-call/phone.png", RES_TYPE_IMAGE, 16, 1, 6 },
    /* res/pictureviewer */
    /* res/pictureviewer/picplayer */
    { "res/pictureviewer/picplayer/1.png", RES_TYPE_IMAGE, 16, 1, 1 },
    { "res/pictureviewer/picplayer/2.png", RES_TYPE_IMAGE, 16, 1, 1 },
    { "res/pictureviewer/picplayer/3.png", RES_TYPE_IMAGE, 16, 1, 1 },
    { "res/pictureviewer/picplayer/4.png", RES_TYPE_IMAGE, 16, 1, 2 },
    { "res/pictureviewer/picplayer/5.png", RES_TYPE_IMAGE, 16, 1, 2 },
    { "res/pictureviewer/picplayer/6.png", RES_TYPE_IMAGE, 16, 1, 2 },
    { "res/pictureviewer/picplayer/7.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/pictureviewer/picplayer/8.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/pictureviewer/picplayer/9.png", RES_TYPE_IMAGE, 16, 1, 3 },
    { "res/pictureviewer/picplayer/pic_next.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/pictureviewer/picplayer/pic_pause_play.png", RES_TYPE_IMAGE, 16, 1, 4 },
    { "res/pictureviewer/picplayer/pic_prev.png", RES_TYPE_IMAGE, 16, 1, 4 },
    /* res/sample-activity */
    { "res/sample-activity/file.png", RES_TYPE_IMAGE, 16, 0, 0 },
    { "res/sample-activity/internet.png", RES_TYPE_IMAGE, 16, 0, 0 },
    { "res/sample-activity/news.png", RES_TYPE_IMAGE, 16, 0, 0 },
    { NULL, 0, 0 }
};

