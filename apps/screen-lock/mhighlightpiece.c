#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global.h"

#include "mhighlightpiece.h"

static void highlight(mHighlightPiece* self, HDC hdc, const char* str, RECT* prc, 
                      UINT uFormat)
{
    const int highlight_width = 60; //40;
    const int w = RECTWP(prc);
    const int h = RECTHP(prc);

#if 0
    HDC memdc2 = CreateCompatibleDC(hdc);
    HDC memdc3 = CreateCompatibleDC(hdc);

	SetTextColor(memdc2, RGBA2Pixel(memdc2, 0x00, 0x00, 0x00, 0xff));
    SetBkMode(memdc2, BM_TRANSPARENT);
    DrawText(memdc2, str, -1, prc, uFormat);

    SetBrushColor(memdc3, RGBA2Pixel(memdc3, 0x80, 0x80, 0x80, 0xfe));
    FillBox(memdc3, 0, 0, highlight_x, h);
    SetBrushColor(memdc3, RGBA2Pixel(memdc3, 0xff, 0xff, 0xFF, 0xfe));
    FillBox(memdc3, highlight_x, 0, highlight_width, h);
    SetBrushColor(memdc3, RGBA2Pixel(memdc3, 0x80, 0x80, 0x80, 0xfe));
    FillBox(memdc3, highlight_x+highlight_width, 0, w-highlight_x-highlight_width, h);

    BitBlt(memdc3, 0, 0, 0, 0, memdc2, 0, 0, 0);
    //BitBlt(memdc2, 0, 0, 0, 0, memdc1, 0, 0, 0);
    BitBlt(memdc2, 0, 0, 0, 0, hdc, 0, 0, 0);

    //DeleteMemDC(memdc1);
    DeleteMemDC(memdc2);
    DeleteMemDC(memdc3);
#else
    if (self->dc_white_font == HDC_INVALID) {
        /* create dc */
        self->dc_white_font = CreateCompatibleDC(hdc);
        
        /* draw dc with white font */
        SetBrushColor(self->dc_white_font, RGBA2Pixel(self->dc_white_font, 0x0, 0x0, 0x0, 0x00));
        FillBox(self->dc_white_font, 0, 0, w, h);
        SetTextColor(self->dc_white_font, RGBA2Pixel(self->dc_white_font, 0xff, 0xff, 0xff, 0xff));
        SetBkMode(self->dc_white_font, BM_TRANSPARENT);
        DrawText(self->dc_white_font, str, -1, prc, uFormat);
    }
    /* black */
    SetTextColor(hdc, RGBA2Pixel(hdc, 0x80, 0x80, 0x80, 0xff));
    DrawText(hdc, str, -1, prc, uFormat);

    if (GetGDCapability(self->dc_white_font, GDCAP_AMASK) == 0)
        SetMemDCColorKey (self->dc_white_font, MEMDC_FLAG_SRCCOLORKEY, 
                          RGBA2Pixel(self->dc_white_font, 0x0, 0x0, 0x0, 0x00));
    BitBlt(self->dc_white_font, self->highlight_x, 0, highlight_width, h, hdc, self->highlight_x, 0, 0);
#endif
}

static void mHighlightPiece_construct(mHighlightPiece *self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);

	self->str = (const char*)add_data;
	self->align = NCS_ALIGN_CENTER;
	self->valign = NCS_VALIGN_CENTER;
	mLabelPiece_setAutoWrap(self, 0);

    self->highlight_x = 0;
    self->dc_white_font = HDC_INVALID;
}

static void mHighlightPiece_destroy(mHighlightPiece* self)
{
    Class(mTextPiece).destroy((mTextPiece*)self);
    if (self->dc_white_font != HDC_INVALID) {
        DeleteMemDC(self->dc_white_font);
    }
}

static void mHighlightPiece_paint(mHighlightPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    RECT rcClient;
    DWORD uFormat = DT_WORDBREAK;
    gal_pixel old_color;
    const char* str = self->str;

    if(owner == NULL || str == NULL)
        return ;

    if (self->font == NULL) {
        self->font = GetWindowFont(owner->hwnd);
    }

    SelectFont(hdc, self->font);
    _c(self)->getRect(self, &rcClient);

    old_color = SetTextColor(hdc, ncsColor2Pixel(hdc, self->color));

    if(self->align == NCS_ALIGN_CENTER)
        uFormat |= DT_CENTER ;
    else if(self->align == NCS_ALIGN_RIGHT)
        uFormat |= DT_RIGHT ;
    else
        uFormat |= DT_LEFT;

    if(self->valign == NCS_VALIGN_CENTER)
        uFormat |= DT_VCENTER ;
    else if(self->valign == NCS_VALIGN_BOTTOM)
        uFormat |= DT_BOTTOM ;
    else
        uFormat |= DT_TOP;

    if(!mLabelPiece_isAutoWrap(self))
        uFormat |= DT_SINGLELINE;

    if(!mLabelPiece_isPrefix(self))
        uFormat |= DT_NOPREFIX;

    if(mLabelPiece_isWordBreak(self))
        uFormat |= DT_WORDBREAK;

    SetBkMode(hdc, BM_TRANSPARENT);

    //DrawText(hdc, str, -1, &rcClient, uFormat);
    highlight(self, hdc, str, &rcClient, uFormat);

    SetTextColor(hdc, ncsColor2Pixel(hdc, old_color));
}

static BOOL mHighlightPiece_setProperty(mHighlightPiece *self, int id, DWORD value)
{
    switch(id)
    {
        case NCSP_HIGHLIGHTPIECE_HIGHTX:
            self->highlight_x = value;
            break;
        default:
            return Class(mTextPiece).setProperty((mTextPiece*)self, id, value);
    }
    return TRUE;
}

static DWORD mHighlightPiece_getProperty(mHighlightPiece* self, int id)
{
	switch(id)
	{
	case NCSP_HIGHLIGHTPIECE_HIGHTX:
		return self->highlight_x;
	}

	return Class(mTextPiece).getProperty((mTextPiece*)self, id);
}

BEGIN_MINI_CLASS(mHighlightPiece, mTextPiece)
	CLASS_METHOD_MAP(mHighlightPiece, construct)
	CLASS_METHOD_MAP(mHighlightPiece, destroy)
	CLASS_METHOD_MAP(mHighlightPiece, paint)
	CLASS_METHOD_MAP(mHighlightPiece, setProperty)
	CLASS_METHOD_MAP(mHighlightPiece, getProperty)
END_MINI_CLASS

