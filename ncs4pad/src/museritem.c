#include <string.h>

#include "global.h"
#include "ResourceManager.hh"

#define TITLEFONT_COLOR       0xFF192952
#define SUBTITLEFONT_COLOR    0xFF868686 
#define DETAILFONT_COLOR      0xFF1882bd 
#define CONTENTFONT_COLOR     0xFF192952
#define BACKGRAND_COLOR       0x7F5CCC5C

static int ICON_WIDTH  = 45;
static int ICON_HEIGHT = 45;

static int ITEMPIECE_MARGIN    = 3;
static int ITEMPIECE_CLEARANCE = 5;

static int init_flag = 0;
static mIndicatorButtonPieceImages indicator_bmps;

/*
 * InitUserItem : init user item
 *
 * Params       : (none)
 *
 * Return       : (none)
 */
void InitUserItem (void)
{
    if (init_flag == 0) {
        init_flag = 1;

        indicator_bmps.bmp[0] =
            (BITMAP *)Load32Resource ("res/common/indicator_green_normal.png",
                RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);

        indicator_bmps.bmp[1] =
            (BITMAP *)Load32Resource ("res/common/indicator_green_click.png",
                RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);

#if 0
        LoadBitmap (HDC_SCREEN,
                &indicator_bmps.bmp[0],
                "res/common/indicator_green_normal.png");

        LoadBitmap (HDC_SCREEN,
                &indicator_bmps.bmp[1],
                "res/common/indicator_green_click.png");
#endif
    }
}

/*
 * CreateBriefInfoItem : create BriefInfo item piece
 *
 * Params              : title
 *                     : subtitle
 *
 * Return              : mItemPiece
 */
mItemPiece *CreateBriefInfoItemEx (const char *title,
        const char *subtitle, DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;
   
    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE+5);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = 0xff192952;
    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    argb = 0xff373737;
    _c(item)->setDetail (item, (char *)subtitle, item->defaultTitleFont, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateEditItem : create edit item piece
 *
 * Params         : title
 *                : detail
 *
 * Return         : mItemPiece
 */
mItemPiece *CreateEditItemEx (const char *title,
        const char *detail, DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;
   
    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    argb = DETAILFONT_COLOR;

    _c(item)->setDetail (item, (char *)detail, NULL, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateContactItem : create contact item piece
 *
 * Params            : icon
 *                   : title
 *                   : subtitle
 *
 * Return            : mItemPiece
 */
mItemPiece *CreateContactItemEx (PBITMAP icon,
        const char *title, const char *subtitle,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    _c(item)->setImage (item, icon, ICON_WIDTH, ICON_HEIGHT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    argb = SUBTITLEFONT_COLOR;

    _c(item)->setSubTitle (item, (char *)subtitle, NULL, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateSMSItem : create sms item piece
 *
 * Params        : title
 *               : subtitle
 *               : detail
 *
 * Return        : mItemPiece
 */
mItemPiece *CreateSMSItemEx (const char *title,
        const char *subtitle, const char *detail,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    argb = SUBTITLEFONT_COLOR;

    _c(item)->setSubTitle (item, (char *)subtitle, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    argb = DETAILFONT_COLOR;

    _c(item)->setDetail (item, (char *)detail, NULL, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateSMSContentItem : create sms content item piece
 *
 * Params               : icon
 *                      : content
 *                      : detail
 *                      : iconAlign
 *
 * Return               : mItemPiece
 */
mItemPiece *CreateSMSContentItemEx (PBITMAP icon,
        const char *content, const char *subTitle,
        enum enumNCSAlign iconAlign, DWORD data)
{
    RECT rect;
    ARGB argb;
    ARGB bkargb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_VALIGN, NCS_VALIGN_TOP);

    if (iconAlign == NCS_ALIGN_LEFT) {
        _c(item)->setLayout (item, NCS_ALIGN_LEFT);

        _c(item)->setImage (item, icon, ICON_WIDTH, ICON_HEIGHT);

        argb = CONTENTFONT_COLOR;

        _c(item)->setSubStance (item, (char *)content, NULL, &argb);

        argb = SUBTITLEFONT_COLOR;

        _c(item)->setSubTitle (item, (char *)subTitle, NULL, &argb);
    }
    else {
        bkargb = BACKGRAND_COLOR;

        _c(item)->setProperty (item, NCSP_ITEMPIECE_GRADIENT_BKCOLOR, bkargb);

        _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

        _c(item)->setImage (item, icon, ICON_WIDTH, ICON_HEIGHT);

        argb = SUBTITLEFONT_COLOR;

        _c(item)->setSubTitle (item, (char *)subTitle, NULL, &argb);

        _c(item)->setLayout (item, NCS_ALIGN_LEFT);

        argb = CONTENTFONT_COLOR;

        _c(item)->setSubStance (item, (char *)content, NULL, &argb);
    }

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateAlbumItem : create Album item piece
 *
 * Params          : icon
 *                 : title
 *                 : detail
 *
 * Return          : mItemPiece
 */
mItemPiece *CreateAlbumItemEx (PBITMAP icon,
        const char *title, const char *detail,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    _c(item)->setImage (item, icon, ICON_WIDTH, ICON_HEIGHT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    argb = DETAILFONT_COLOR;

    _c(item)->setDetail (item, (char *)detail, NULL, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateSongItem : create Song item piece
 *
 * Params         : title
 *                : subtitle
 *                : detail
 *
 * Return         : mItemPiece
 */
mItemPiece *CreateSongItemEx (const char *title,
        const char *subtitle, const char *detail,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    argb = SUBTITLEFONT_COLOR;

    _c(item)->setSubTitle (item, (char *)subtitle, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    argb = DETAILFONT_COLOR;

    _c(item)->setDetail (item, (char *)detail, NULL, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateDetailBtnItem : create detail button item piece
 *
 * Params              : title
 *                     : btn_text
 *                     : default_value
 *
 * Return              : mItemPiece
 */
mItemPiece *CreateDetailBtnItemEx (const char *title,
        const char *btn_text, BOOL default_value,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    InitUserItem();

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    _c(item)->setAccessoryControl (item,
            (char *)btn_text, default_value,
            ITEMPIECE_AC_INDICATORBUTTON, (DWORD)&indicator_bmps);

    argb = DETAILFONT_COLOR;

    _c(item)->setDetail (item, (char *)btn_text, NULL, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateCheckBoxItem : create check box item piece
 *
 * Params             : title
 *                    : btn_text
 *                    : default_value
 *                    : btnAlign
 *
 * Return             : mItemPiece
 */
mItemPiece *CreateCheckBoxItemEx (const char *title,
        const char *btn_text, BOOL default_value,
        enum enumNCSAlign btnAlign, DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    if (btnAlign == NCS_ALIGN_RIGHT) {
        _c(item)->setLayout (item, NCS_ALIGN_LEFT);

        argb = TITLEFONT_COLOR;

        _c(item)->setTitle (item, (char *)title, NULL, &argb);

        _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

        _c(item)->setAccessoryControl (item,
                (char *)btn_text, default_value,
                ITEMPIECE_AC_CHECKMARK, 0);
    }
    else {
        _c(item)->setLayout (item, NCS_ALIGN_LEFT);

        _c(item)->setAccessoryControl (item,
                (char *)btn_text, default_value,
                ITEMPIECE_AC_CHECKMARK, 0);

        argb = TITLEFONT_COLOR;

        _c(item)->setTitle (item, (char *)title, NULL, &argb);
    }

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateRadioItem : create radio item piece
 *
 * Params          : title
 *                 : btn_text
 *                 : default_value
 *                 : btnStyle
 *                 : btnAlign
 *
 * Return          : mItemPiece
 */
mItemPiece *CreateRadioItemEx (const char *title,
        const char *btn_text, BOOL default_value,
        int btnStyle, enum enumNCSAlign btnAlign,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    if (btnAlign == NCS_ALIGN_RIGHT) {
        _c(item)->setLayout (item, NCS_ALIGN_LEFT);

        argb = TITLEFONT_COLOR;

        _c(item)->setTitle (item, (char *)title, NULL, &argb);

        _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

        _c(item)->setAccessoryControl (item,
                (char *)btn_text, default_value,
                ITEMPIECE_AC_RADIO, 0);
    }
    else {
        _c(item)->setLayout (item, NCS_ALIGN_LEFT);

        _c(item)->setAccessoryControl (item,
                (char *)btn_text, default_value,
                ITEMPIECE_AC_RADIO, 0);

        argb = TITLEFONT_COLOR;

        _c(item)->setTitle (item, (char *)title, NULL, &argb);
    }

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateSwitchCtrlItem : create switch control item piece
 *
 * Params               : title
 *                      : btn_text
 *                      : default_val
 *
 * Return               : mItemPiece
 */
mItemPiece *CreateSwitchCtrlItemEx (const char *title,
        const char *btn_text, BOOL default_value,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    _c(item)->setAccessoryControl (item,
            (char *)btn_text, default_value,
            ITEMPIECE_AC_SWITCH, 0);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateIndicatorItem : create disclosure indicator item piece
 *
 * Params              : title
 *                     : detail
 *                     : btn_text
 *
 * Return              : mItemPiece
 */
mItemPiece *CreateIndicatorItemEx (const char *title,
        const char *detail, const char *btn_text,
        DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    _c(item)->setAccessoryControl (item,
            (char *)btn_text, TRUE,
            ITEMPIECE_AC_INDICATOR, 0);

    argb = DETAILFONT_COLOR;

    _c(item)->setDetail (item, (char *)detail, NULL, &argb);

    _c(item)->activeLayout (item);

    return item;
}

/*
 * CreateButtonItem : create button item piece
 *
 * Params           : title
 *
 * Return           : mItemPiece
 */
mItemPiece *CreateButtonItemEx (const char *title,
        const char *btn_text, DWORD data)
{
    RECT rect;
    ARGB argb;

    mItemPiece *item;

    InitUserItem();

    item = NEWPIECEEX (mItemPiece, data);

    SetRect (&rect, 0, 0, ITEMPIECE_WIDTH, ITEMPIECE_HEIGHT);

    _c(item)->setRect (item, &rect);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_MARGIN, ITEMPIECE_MARGIN);

    _c(item)->setProperty (item, NCSP_ITEMPIECE_CLEARANCE, ITEMPIECE_CLEARANCE);

    _c(item)->setLayout (item, NCS_ALIGN_LEFT);

    argb = TITLEFONT_COLOR;

    _c(item)->setTitle (item, (char *)title, NULL, &argb);

    _c(item)->setLayout (item, NCS_ALIGN_RIGHT);

    _c(item)->setAccessoryControl (item,
            (char *)btn_text, TRUE,
            ITEMPIECE_AC_INDICATORBUTTON, (DWORD)&indicator_bmps);

    _c(item)->activeLayout (item);

    return item;
}


