#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>
#include <mgplus/mgplus.h>

#include "manimationeditpiece.h"

#define ANINATIOM_DURATION 200 /*200ms*/
#define TEXT_BUFFER_LENGTH 15
#define TEXT_ZOOM_NO 3

static void cb_display_string (MGEFF_ANIMATION handle, void* target, int id, void* value)
{
    mAnimationEditPiece *self= (mAnimationEditPiece *)target;
    HWND hwnd = (HWND)id;
    RECT rc;

    _c(self)->getRect(self,&rc);
    self->text_offset = *(int *)value;
    UpdateWindow (hwnd, TRUE);
}

static int calculate_string_width (HWND hwnd,mAnimationEditPiece *self, const char *str)
{
    /* calculate move distance */
    SIZE size;
    HDC hdc = GetDC (hwnd);
    GetTextExtent (hdc, str, strlen(str), &size);
    ReleaseDC (hdc);
    return size.cx;
}

static void set_font (HWND hwnd,mAnimationEditPiece *self, const char *str)
{
    int i;
    RECT rc;
    int width;
    int width_win;
    LOGFONT logfont;

    _c(self)->getRect(self,&rc);
    width_win = RECTW (rc);

    if (self->pFont == NULL||self->pFont[0] == NULL)
        return;

    logfont = *(self->pFont[0]);
    if (self->pFont[1] == NULL&&self->pFont[2] == NULL)
    {
        for (i=1;i<TEXT_ZOOM_NO;i++)/*font[0] come from user*/
        {
            self->pFont[i] = CreateLogFont (logfont.type, logfont.family,logfont.charset,
                FONT_WEIGHT_BOOK,
                FONT_SLANT_ROMAN,
                FONT_SETWIDTH_NORMAL,
                FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                logfont.size*1/(i+1), 0);
        }
    }
    for (i=0; i<TEXT_ZOOM_NO; ++i) {
        SetWindowFont (hwnd, self->pFont[i]);
        width = calculate_string_width (hwnd,self, str);

        if (width <= width_win) {
            return;
        }
    }
}

static void animation_modify_string (HWND hwnd,mAnimationEditPiece *self, 
        const char *key_str, BOOL append)
{
    MGEFF_ANIMATION animation_handle = NULL;
    int move_distance;
    RECT rc;

    int width_win;
    int width_text;
    int endval;
    /* set string font size */
    char *tmpstr = (char *)calloc (1, self->content_length);
    memset (tmpstr, 0, self->content_length);
    if (append == TRUE) {
        sprintf (tmpstr, "%s%s", self->text, key_str);
    } else {
        sprintf (tmpstr, "%s", self->text);
        tmpstr[strlen (tmpstr) - strlen (key_str)] = 0;
    }
    set_font (hwnd,self, tmpstr);
    move_distance = calculate_string_width (hwnd,self, key_str);

    /* create animation */
    animation_handle = mGEffAnimationCreate(self, cb_display_string,hwnd, MGEFF_INT);

    /* start val */
    _c(self)->getRect(self,&rc);
    width_win = RECTW (rc);
    width_text = calculate_string_width (hwnd,self, self->text);
    self->text_offset = width_win - width_text;
    mGEffAnimationSetStartValue (animation_handle, &self->text_offset);

    /* end val */
    endval = self->text_offset + ((append == FALSE)?move_distance:(0-move_distance));
    mGEffAnimationSetEndValue (animation_handle, &endval);

    /* misc */
    mGEffAnimationSetProperty (animation_handle, MGEFF_PROP_DURATION, ANINATIOM_DURATION);
    mGEffAnimationSetCurve (animation_handle, InQuad);

    if (append == TRUE) {
        strcat (self->text, key_str);
        mGEffAnimationSyncRun (animation_handle);
        mGEffAnimationDelete (animation_handle);
    } else {
        int string_length = strlen(self->text);
        int key_str_len = strlen (key_str);
        mGEffAnimationSyncRun (animation_handle);
        mGEffAnimationDelete (animation_handle);
        memset (&self->text[string_length - key_str_len], 0,key_str_len);
    } 
   free (tmpstr);
}

static void mAnimationEditPiece_construct(mAnimationEditPiece *self, DWORD param)
{
    int i;

	Class(mStaticPiece).construct((mStaticPiece*)self, param);

    self->content_length = TEXT_BUFFER_LENGTH+1;
    self->text = (char *)calloc (1, self->content_length);
    if (self->text == NULL)
    {
        printf("calloc text mem failure!!\n");
        return;
    }
    memset (self->text, 0, self->content_length);

    self->text_offset = 0;
    self->text_color = 0xffffffff;
    self->text_shadow_color = 0xffffffff;

    self->pFont = (PLOGFONT *)calloc (TEXT_ZOOM_NO, sizeof(PLOGFONT));

    if (self->pFont == NULL)
    {
        printf("calloc font mem failure!!\n");
        return;
    }
    for (i=0;i<TEXT_ZOOM_NO;i++)
    {
        self->pFont[i] = NULL;
    }
}

static void mAnimationEditPiece_destroy (mAnimationEditPiece *self, DWORD param)
{
    int i;
    free (self->text);
    self->text = NULL;
    self->content_length = 0;
    
    for (i=1; i<TEXT_ZOOM_NO; ++i) {/*font[0] come from user,will be released by user*/
        DestroyLogFont (self->pFont[i]);
    }

    free (self->pFont);
    self->pFont = NULL;
}

static void mAnimationEditPiece_setContent(HWND hwnd,
        mAnimationEditPiece *self, const char* str,text_align align)
{
    RECT rc;
    int string_width;
    /* check content for new string */
    if (strlen (str) >= self->content_length) {
        char *p = realloc (self->text, strlen(str)+1);
        if (!p) {
            printf ("realloc error!\n");
            return;
        }
        self->text = p;
        self->content_length = strlen(str)+1;
    }

    memset (self->text, 0, self->content_length);
    strcpy (self->text, str);
    
    /* calculate new string's offest */
    _c(self)->getRect(self,&rc);
    set_font (hwnd,self, self->text);
    string_width = calculate_string_width (hwnd,self, self->text);
    self->text_offset =(RECTW (rc) - string_width)>>1;
    if (align == TEXT_ALIGN_RIGHT)
    {
        self->text_offset = RECTW (rc) - string_width;
    }
    if (align == TEXT_ALIGN_LEFT)
    {
        self->text_offset = 0;
    }
}

static int mAnimationEditPiece_getContent(mAnimationEditPiece *self, char *strbuff, int buf_len)
{
    if (buf_len <= strlen (self->text)) {
        return -1;
    }
    strcpy (strbuff, self->text);
    return 0;
}

static void mAnimationEditPiece_append(HWND hwnd,mAnimationEditPiece *self, const char* str)
{
    /* check content for append */
    if ((strlen (str) + strlen (self->text)) >= self->content_length) {
        char *p =  realloc (self->text, self->content_length*2);
        if (!p) {
            printf ("realloc error!\n");
            return;
        }

        self->text = p;
        self->content_length = self->content_length * 2;
    }
    
    animation_modify_string (hwnd,self, str, TRUE);
}

static void mAnimationEditPiece_tailDelete(HWND hwnd,mAnimationEditPiece *self, int num) 
{
    assert (num>=0);

    /* had deleted all */
    if (self->text == NULL) {
        return;
    }

    /* delete num > strlen */
    if (num > strlen (self->text)) {
        num = strlen (self->text);
    }

    animation_modify_string (hwnd,self, &self->text[strlen(self->text) - num], FALSE);
}

static void mAnimationEditPiece_paint (mAnimationEditPiece *self, HDC hdc, 
        mWidget *owner, DWORD add_data)
{
    SIZE size;
    int h;
    int y;
    DWORD color;

    SetBkMode(hdc,BM_TRANSPARENT);

    /* get font y  coordinate */
    GetTextExtent (hdc, "A", 1, &size);
    h = GetGDCapability (hdc, GDCAP_MAXY) + 1;
    y = (h - size.cy)/2;

    color = (DWORD)self->text_shadow_color;
    SetTextColor (hdc, RGBA2Pixel(hdc, color>>16&0xff,color>>8&0xff, color&0xff, color>>24&0xff));
    TextOut (hdc, self->text_offset+1, y+1, self->text);

    color = (DWORD)self->text_color;
    SetTextColor (hdc, RGBA2Pixel(hdc, color>>16&0xff, color>>8&0xff, color&0xff, color>>24&0xff));
    TextOut (hdc, self->text_offset, y, self->text);

}

static BOOL mAnimationEditPiece_setProperty(mAnimationEditPiece *self, int id, DWORD value)
{
    switch (id) {
        case NCSP_ANIMATIONEDITPIECE_FONT:
            if (self->pFont != NULL)
            {
                self->pFont[0] = (PLOGFONT)value;
            }
            break;
        case NCSP_ANIMATIONEDITPIECE_TEXTCOLOR:
            self->text_color = (ARGB)value;
            break;
        case NCSP_ANIMATIONEDITPIECE_TEXTSHADOWCOLOR:
            self->text_shadow_color = (ARGB)value;
            break;
        default:
            return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
    }
    return TRUE;
}

static DWORD mAnimationEditPiece_getProperty(mAnimationEditPiece *self, int id)
{
	switch(id)
	{
        case NCSP_ANIMATIONEDITPIECE_FONT:
            if (self->pFont != NULL)
            {
                return (DWORD)self->pFont[0];
            }
            else
            {
                return (DWORD)NULL;
            }
        case NCSP_ANIMATIONEDITPIECE_TEXTCOLOR:
            return (DWORD)self->text_color;
        case NCSP_ANIMATIONEDITPIECE_TEXTSHADOWCOLOR:
            return (DWORD)self->text_shadow_color;
	}
	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}


BEGIN_MINI_CLASS(mAnimationEditPiece, mStaticPiece)
	CLASS_METHOD_MAP(mAnimationEditPiece, construct)
	CLASS_METHOD_MAP(mAnimationEditPiece, destroy)
	CLASS_METHOD_MAP(mAnimationEditPiece, paint)
	CLASS_METHOD_MAP(mAnimationEditPiece, setProperty)
	CLASS_METHOD_MAP(mAnimationEditPiece, getProperty)
    CLASS_METHOD_MAP(mAnimationEditPiece, setContent)
    CLASS_METHOD_MAP(mAnimationEditPiece, getContent)
    CLASS_METHOD_MAP(mAnimationEditPiece, append)
    CLASS_METHOD_MAP(mAnimationEditPiece, tailDelete)
END_MINI_CLASS

