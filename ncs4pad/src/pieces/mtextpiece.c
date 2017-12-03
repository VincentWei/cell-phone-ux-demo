#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mtextpiece.h"

static void mTextPiece_construct(mTextPiece *self, DWORD add_data)
{
	Class(mLabelPiece).construct ((mLabelPiece*)self, add_data);

	self->font          = NULL;

    self->color         = 0xFFFFFFFF;
    self->shadow_color  = 0xFFFFFFFF;

    self->isShadow      = FALSE;

    self->maxLen        = -1;
}

static void mTextPiece_paint(mTextPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    RECT rcClient;
    DWORD uFormat = 0;
    gal_pixel old_color;
    DWORD shadow_color;
    const char* str = self->str;

    if ((owner == NULL) || (str == NULL)) {
        return;
    }

    if (self->font == NULL) {
        self->font = GetWindowFont (owner->hwnd);
    }

    SelectFont (hdc, self->font);
    _c(self)->getRect (self, &rcClient);

    if (self->align == NCS_ALIGN_CENTER) {
        uFormat |= DT_CENTER;
    }
    else if (self->align == NCS_ALIGN_RIGHT) {
        uFormat |= DT_RIGHT;
    }
    else {
        uFormat |= DT_LEFT;
    }

    if (self->valign == NCS_VALIGN_CENTER) {
        uFormat |= DT_VCENTER;
    }
    else if (self->valign == NCS_VALIGN_BOTTOM) {
        uFormat |= DT_BOTTOM;
    }
    else {
        uFormat |= DT_TOP;
    }

    if (!mLabelPiece_isAutoWrap(self)) {
        uFormat |= DT_SINGLELINE;
    }

    if (!mLabelPiece_isPrefix(self)) {
        uFormat |= DT_NOPREFIX;
    }

    if (mLabelPiece_isWordBreak(self)) {
        uFormat |= DT_WORDBREAK;
    }

    SetBkMode (hdc, BM_TRANSPARENT);

    shadow_color = self->shadow_color;

    if (self->isShadow && ((uFormat & DT_CENTER) == DT_CENTER)) {
        RECT rc;

        old_color = SetTextColor (hdc,
                RGBA2Pixel(hdc,
                    shadow_color >> 16 & 0xFF,
                    shadow_color >> 8 & 0xFF,
                    shadow_color & 0xFF,
                    shadow_color >> 24 & 0xFF)
                );

        SetRect(&rc,rcClient.left,rcClient.top+1,rcClient.right,rcClient.bottom+1);

        if (self->maxLen > 0) {
            TextOutOmitted (hdc, rc.left, rc.top, str, strlen(str), self->maxLen);
        }
        else {
            DrawText (hdc, str, -1, &rc, uFormat);
        }

        shadow_color = self->color;

        SetTextColor (hdc,
                RGBA2Pixel (hdc,
                    shadow_color >> 16 & 0xFF,
                    shadow_color >> 8 & 0xFF,
                    shadow_color & 0xFF,
                    shadow_color >> 24 & 0xFF)
                );

        if (self->maxLen > 0) {
            TextOutOmitted (hdc, rcClient.left, rcClient.top, str, strlen(str), self->maxLen);
        }
        else {
            DrawText (hdc, str, -1, &rcClient, uFormat);
        }
    }
    else {
        shadow_color = self->color;

        old_color = SetTextColor(hdc,
                RGBA2Pixel (hdc,
                    shadow_color >> 16 & 0xFF,
                    shadow_color >> 8 & 0xFF,
                    shadow_color & 0xFF,
                    shadow_color >> 24 & 0xFF));

        if (self->maxLen > 0) {
            TextOutOmitted (hdc, rcClient.left, rcClient.top, str, strlen(str), self->maxLen);
        }
        else {
            DrawText (hdc, str, -1, &rcClient, uFormat);
        }
    }

    SetTextColor (hdc, ncsColor2Pixel(hdc, old_color));
}

static BOOL mTextPiece_setProperty (mTextPiece *self, int id, DWORD value)
{
    if (id == NCSP_TEXTPIECE_LOGFONT) {
        self->font = (PLOGFONT)value;
    }
    else if (id == NCSP_TEXTPIECE_TEXTCOLOR) {
        self->color = (DWORD)value;
    }
    else if (id == NCSP_TEXTPIECE_TEXTSHADOWCOLOR) {
        self->shadow_color = (DWORD)value;
        self->isShadow     = TRUE;
    }
    else if (id == NCSP_TEXTPIECE_MAXLEN) {
        self->maxLen = (int)value;
    }
    else {
        return Class(mLabelPiece).setProperty ((mLabelPiece*)self, id, value);
    }

    return TRUE;
}

static DWORD mTextPiece_getProperty(mTextPiece* self, int id)
{
    if (id == NCSP_TEXTPIECE_LOGFONT) {
        return (DWORD)self->font;
    }
    else if (id == NCSP_TEXTPIECE_TEXTCOLOR) {
        return (DWORD)self->color;
    }
    else if (id == NCSP_TEXTPIECE_TEXTSHADOWCOLOR) {
        return (DWORD)self->shadow_color;
    }
    else {
        return Class(mLabelPiece).getProperty ((mLabelPiece*)self, id);
    }
}

static BOOL mTextPiece_autoSize (mTextPiece* self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
    SIZE size;
    HDC hdc;
    if (!owner || !self->str)
        return FALSE;

    hdc = GetClientDC (owner->hwnd);

    if (self->font == NULL) {
        self->font = GetWindowFont (owner->hwnd);
    }

    SelectFont (hdc, self->font);

    GetTextExtent (hdc, self->str, strlen(self->str), &size);

    ReleaseDC (hdc);

    if (pszMin) {   
        if (size.cx < pszMin->cx) {
            size.cx = pszMin->cx;
        }

        if (size.cy < pszMin->cy) {
            size.cy = pszMin->cy;
        }
    }

    if (pszMax) {   
        if (size.cx > pszMax->cx) {
            size.cx = pszMax->cx;
        }

        if (size.cy > pszMax->cy) {
            size.cy = pszMax->cy;
        }
    }

    self->right  = self->left + size.cx;
    self->bottom = self->top + size.cy;

    return TRUE;
}

BEGIN_MINI_CLASS(mTextPiece, mLabelPiece)
	CLASS_METHOD_MAP(mTextPiece, construct    )
	CLASS_METHOD_MAP(mTextPiece, paint        )
	CLASS_METHOD_MAP(mTextPiece, setProperty  )
	CLASS_METHOD_MAP(mTextPiece, getProperty  )
	CLASS_METHOD_MAP(mTextPiece, autoSize     )
END_MINI_CLASS

