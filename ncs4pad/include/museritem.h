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
#ifndef __MUSERITEM__H__
#define __MUSERITEM__H__

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "register.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * CreateBriefInfoItem : create brief info item piece
 *
 * Params              : title
 *                     : subtitle
 *
 * Return              : mItemPiece
 */
mItemPiece *CreateBriefInfoItemEx (const char *title,
        const char *subtitle, DWORD data);
#define CreateBriefInfoItem(title,edit_text) \
        CreateBriefInfoItemEx(title,edit_text,0)

/*
 * CreateEditItem : create edit item piece
 *
 * Params         : title
 *                : edit_text
 *
 * Return         : mItemPiece
 */
mItemPiece *CreateEditItemEx (const char *title,
        const char *edit_text, DWORD data);
#define CreateEditItem(title,edit_text) \
        CreateEditItemEx(title,edit_text,0)

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
        DWORD data);
#define CreateContactItem(icon,title,subtitle) \
        CreateContactItemEx(icon,title,subtitle,0)

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
        DWORD data);
#define CreateSMSItem(title,subtitle,detail) \
        CreateSMSItemEx(title,subtitle,detail,0)

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
        const char *content, const char *detail,
        enum enumNCSAlign iconAlign, DWORD data);
#define CreateSMSContentItem(icon,content,detail,iconAlign) \
        CreateSMSContentItemEx(icon,content,detail,iconAlign,0)

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
        DWORD data);
#define CreateAlbumItem(icon,title,detail) \
        CreateAlbumItemEx(icon,title,detail,0)

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
        DWORD data);
#define CreateSongItem(title,subtitle,detail) \
        CreateSongItemEx(title,subtitle,detail,0)

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
        DWORD data);
#define CreateDetailBtnItem(title,btn_text,default_value) \
        CreateDetailBtnItemEx(title,btn_text,default_value,0)

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
        enum enumNCSAlign btnAlign, DWORD data);
#define CreateCheckBoxItem(title,btn_text,default_value,btnAlign) \
        CreateCheckBoxItemEx(title,btn_text,default_value,btnAlign,0)

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
        DWORD data);
#define CreateRadioItem(title,btn_text,default_value,btnStyle,btnAlign) \
        CreateRadioItemEx(title,btn_text,default_value,btnStyle,btnAlign,0)

/*
 * CreateSwitchCtrlItem : create switch control item piece
 *
 * Params               : title
 *                      : btn_text
 *                      : default_value
 *
 * Return               : mItemPiece
 */
mItemPiece *CreateSwitchCtrlItemEx (const char *title,
        const char *btn_text, BOOL default_value,
        DWORD data);
#define CreateSwitchCtrlItem(title,btn_text,default_value) \
        CreateSwitchCtrlItemEx(title,btn_text,default_value,0)

/*
 * CreateIndicatorItem : create disclosure indicator item piece
 *
 * Params              : title
 *
 * Return              : mItemPiece
 */
mItemPiece *CreateIndicatorItemEx (const char *title,
        const char *detail, const char *btn_text,
        DWORD data);
#define CreateIndicatorItem(title,detail,btn_text) \
        CreateIndicatorItemEx(title,detail,btn_text,0)

/*
 * CreateButtonItem : create button item piece
 *
 * Params           : title
 *
 * Return           : mItemPiece
 */
mItemPiece *CreateButtonItemEx (const char *title,
        const char *btn_text, DWORD data);
#define CreateButtonItem(title,btn_text) \
        CreateButtonItemEx(title,btn_text,0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MUSERITEM__H__ */

