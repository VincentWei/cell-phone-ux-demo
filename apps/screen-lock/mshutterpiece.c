#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global.h"

#include "mshutterpiece.h"

static void mShutterPiece_construct(mShutterPiece *self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);

	self->str = (const char*)add_data;
	self->align = NCS_ALIGN_CENTER;
	self->valign = NCS_VALIGN_CENTER;
	mLabelPiece_setAutoWrap(self, 0);

    if (add_data) {
        self->shutter_width = ((RECT*)add_data)->right;
        self->shutter_height = ((RECT*)add_data)->bottom;
    }
    else {
        self->shutter_width = self->right;
        self->shutter_height = self->bottom;
    }
    self->offset_x = 0;
    self->offset_y = 0;
}

static BOOL mShutterPiece_setRect(mShutterPiece *self, RECT *prc)
{
    if (! Class(mStaticPiece).setRect((mStaticPiece*)self, prc))
        return FALSE;

    self->offset_x = prc->right - self->shutter_width;
    self->offset_y = prc->bottom - self->shutter_height;

    //printf("mShutterPiece_setRect offset: %d, %d\n", self->offset_x, self->offset_y);

	return TRUE;
}

static void mShutterPiece_paint(mShutterPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    RECT rcClient;
    DWORD uFormat = DT_WORDBREAK;
    gal_pixel old_color;
    const char* str = self->str;

    const char* line_begin = NULL;
    const char* line_end = NULL;
    int line_num = 0;
    int line_height = 0;
    int line_len = 0;
    RECT rc;

    if(owner == NULL || str == NULL)
        return ;

    if (self->font == NULL) {
        self->font = GetWindowFont(owner->hwnd);
    }

    SelectFont(hdc, self->font);
    //_c(self)->getRect(self, &rcClient);

    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = self->shutter_width;
    rcClient.bottom = self->shutter_height;
    OffsetRect(&rcClient, self->offset_x, self->offset_y);

    //printf("mShutterPiece_paint rcClient: %d, %d, %d, %d\n", rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

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

    // get line_num
    for (line_begin = str; *line_begin != '\0'; ++line_begin) {
        if (*line_begin == '\n') ++line_num;
    }
    ++line_num;

    // get line_height
    line_height = (RECTH(rcClient)) / line_num;

    // init rc
    memcpy(&rc, &rcClient, sizeof(rc));

    // draw lines
    for (line_begin = str; *line_begin != '\0'; line_begin = line_end + 1) {
        // get line length
        line_end = strchr(line_begin, '\n');
        line_len = -1;
        if (line_end != NULL)
            line_len = line_end - line_begin;

        rc.top += 1;
        rc.bottom = rc.top + line_height;

        SetTextColor(hdc, ncsColor2Pixel(hdc, self->shadow_color));
        DrawText(hdc, line_begin, line_len, &rc, uFormat);

        --rc.top, --rc.bottom;
        SetTextColor(hdc, ncsColor2Pixel(hdc, self->color));
        DrawText(hdc, line_begin, line_len, &rc, uFormat);

        // there is no more lines
        if (line_end == NULL)
            break;

        // set next top
        rc.top = rc.top + line_height;
    }

    SetTextColor(hdc, ncsColor2Pixel(hdc, old_color));
}

static BOOL mShutterPiece_setProperty(mShutterPiece *self, int id, DWORD value)
{
    switch(id)
    {
        case NCSP_SHUTTERPIECE_WIDTH:
            self->shutter_width = value;
            return TRUE;
        case NCSP_SHUTTERPIECE_HEIGHT:
            self->shutter_height = value;
            return TRUE;
    }
    return Class(mTextPiece).setProperty((mTextPiece*)self, id, value);
}

static DWORD mShutterPiece_getProperty(mShutterPiece* self, int id)
{
    switch(id)
    {
        case NCSP_SHUTTERPIECE_WIDTH:
            return self->shutter_width;
        case NCSP_SHUTTERPIECE_HEIGHT:
            return self->shutter_height;
    }

    return Class(mTextPiece).getProperty((mTextPiece*)self, id);
}

BEGIN_MINI_CLASS(mShutterPiece, mTextPiece)
	CLASS_METHOD_MAP(mShutterPiece, construct)
	CLASS_METHOD_MAP(mShutterPiece, setRect)
	CLASS_METHOD_MAP(mShutterPiece, paint)
	CLASS_METHOD_MAP(mShutterPiece, setProperty)
	CLASS_METHOD_MAP(mShutterPiece, getProperty)
END_MINI_CLASS

