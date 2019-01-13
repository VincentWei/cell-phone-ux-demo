
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>

#include "animationedit.h"

#define USE_MGPLUS

#define ANINATIOM_DURATION 200 /*200ms*/
#define TEXT_BUFFER_LENGTH 15
const int FONT_SIZE_TABLE[] ={35, 25, 15};

static BOOL mAnimationEdit_onEraseBkgnd(mAnimationEdit *self, HDC hdc, const RECT *pinv);
static void mAnimationEdit_onPaint (mAnimationEdit *self, HDC hdc, const PCLIPRGN pinv_clip);

static void animation_set_content (mAnimationEdit *self, const char *effectorname, int duration)
{
    unsigned long key;
    MGEFF_EFFECTOR effector;
    MGEFF_SOURCE source1;
    MGEFF_SOURCE source2;
    MGEFF_SINK sink;
    MGEFF_ANIMATION handle;
    HDC ctrl_dc = GetClientDC (self->hwnd);
    //HDC ctrl_dc = GetDC (self->hwnd);

    HDC src1_dc = CreateCompatibleDC (ctrl_dc);
    HDC src2_dc = CreateCompatibleDC (ctrl_dc);

    /*init source 1*/
    BitBlt (ctrl_dc, 0, 0, 0, 0, src1_dc, 0, 0, 0);

    /*init source 2*/
    mAnimationEdit_onEraseBkgnd(self, src2_dc, NULL);
    mAnimationEdit_onPaint (self, src2_dc, NULL);

    key = mGEffStr2Key(effectorname);
    effector = mGEffEffectorCreate (key);
    source1 = mGEffCreateSource (src1_dc);
    source2 = mGEffCreateSource (src2_dc);
    sink = mGEffCreateHDCSink (ctrl_dc);

    mGEffEffectorAppendSource (effector, source1);
    mGEffEffectorAppendSource (effector, source2);
    mGEffSetBufferSink(sink, ctrl_dc);
    mGEffEffectorSetSink (effector, sink);

    handle = mGEffAnimationCreateWithEffector (effector);

    mGEffAnimationSetDuration (handle, duration);

    mGEffAnimationSyncRun (handle);
    mGEffAnimationDelete (handle);
    mGEffEffectorDelete (effector);

    ReleaseDC (ctrl_dc);
    DeleteMemDC (src1_dc);
    DeleteMemDC (src2_dc);
}

static void cb_display_string (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value)
{
    mAnimationEdit *self= (mAnimationEdit *)target;

    self->text_offest = *(int *)value;
    UpdateWindow (self->hwnd, TRUE);
}

static int calculate_string_width (mAnimationEdit *self, const char *str)
{
    /* calculate move distance */
    SIZE size;
    HDC hdc = GetDC (self->hwnd);
    GetTextExtent (hdc, str, strlen(str), &size);
    ReleaseDC (hdc);

    return size.cx;
}

static void set_font (mAnimationEdit *self, const char *str)
{
    HWND hwnd = self->hwnd;
    RECT rc;
    int width;
    int width_win;
    int i;

    GetWindowRect (hwnd, &rc);
    width_win = RECTW (rc);

    for (i=0; i<TABLESIZE(FONT_SIZE_TABLE); ++i) {
        SetWindowFont (hwnd, self->pFont[i]);
        width = calculate_string_width (self, str);

        if (width <= width_win) {
            return;
        }
    }
}

static void animation_modify_string (mAnimationEdit *self, const char *key_str, BOOL append)
{
    MGEFF_ANIMATION animation_handle = NULL;
    int move_distance;
    RECT rc;
    int width_win;
    int width_text;
    char *tmpstr;
    int endval;

    /* set string font size */
    tmpstr = (char *)calloc (1, self->content_length);
    memset (tmpstr, 0, self->content_length);
    if (append == TRUE) {
        sprintf (tmpstr, "%s%s", self->text, key_str);
    } else {
        sprintf (tmpstr, "%s", self->text);
        tmpstr[strlen (tmpstr) - strlen (key_str)] = 0;
    }
    set_font (self, tmpstr);
    move_distance = calculate_string_width (self, key_str);

    /* create animation */
    animation_handle = mGEffAnimationCreate(self, cb_display_string, -1, MGEFF_INT);

    /* start val */
    GetWindowRect (self->hwnd, &rc);
    width_win = RECTW (rc);
    width_text = calculate_string_width (self, self->text);
    self->text_offest = width_win - width_text;
    mGEffAnimationSetStartValue (animation_handle, &self->text_offest);

    /* end val */
    endval = self->text_offest + ((append == FALSE)?move_distance:(0-move_distance));
    mGEffAnimationSetEndValue (animation_handle, &endval);

    /* misc */
    mGEffAnimationSetProperty (animation_handle, MGEFF_PROP_DURATION, ANINATIOM_DURATION);
    mGEffAnimationSetCurve (animation_handle, InQuad);

    if (append == TRUE) {
        strcat (self->text, key_str);
        mGEffAnimationSyncRun (animation_handle);
        mGEffAnimationDelete (animation_handle);
    } else {
        int string_length;
        int key_str_len;
        mGEffAnimationSyncRun (animation_handle);
        mGEffAnimationDelete (animation_handle);
        string_length = strlen(self->text);
        key_str_len = strlen (key_str);
        memset (&self->text[string_length - key_str_len], 0,key_str_len);
    } 
   free (tmpstr);
}

static void mAnimationEdit_construct(mAnimationEdit *self, DWORD param)
{
    int i;
    RECT rc;

    Class(mWidget).construct((mWidget*)self, param);

    self->content_length = TEXT_BUFFER_LENGTH+1;
    self->text = (char *)calloc (1, self->content_length);
    memset (self->text, 0, self->content_length);

    GetWindowRect (self->hwnd, &rc);
    self->text_offest = RECTW (rc);
    self->text_color = 0xffffffff;

    self->pFont = (PLOGFONT *)calloc (TABLESIZE (FONT_SIZE_TABLE), sizeof(PLOGFONT));

    /*create font*/
    for (i=0; i<TABLESIZE(FONT_SIZE_TABLE); ++i) {
        self->pFont[i] = CreateLogFontEx ("ttf", "helvetica", "GB2312",
                FONT_WEIGHT_REGULAR,
                FONT_SLANT_ROMAN,
                FONT_FLIP_NONE,
                FONT_OTHER_NONE,
                FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
                FONT_SIZE_TABLE[i], 0);
    }
}

static void mAnimationEdit_destroy (mAnimationEdit *self, DWORD param)
{
    int i;
    free (self->text);
    self->text = NULL;
    self->content_length = 0;
    
    for (i=0; i<3; ++i) {
        DestroyLogFont (self->pFont[i]);
    }

    free (self->pFont);
    self->pFont = NULL;
}

static void mAnimationEdit_setContent(mAnimationEdit *self, const char* str, const char *effectorname)
{
    int string_width;
    RECT rc;
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
    GetWindowRect (self->hwnd, &rc);
    set_font (self, self->text);
    string_width = calculate_string_width (self, self->text);
    self->text_offest = RECTW (rc) - string_width;

    if (effectorname == NULL) {
        /* no animation */
        UpdateWindow (self->hwnd, TRUE);
    } else {
        /* have animation */
        animation_set_content(self, effectorname, ANINATIOM_DURATION);
    }
}

static int mAnimationEdit_getContent(mAnimationEdit *self, char *strbuff, int buf_len)
{
    if (buf_len <= strlen (self->text)) {
        return -1;
    }

    strcpy (strbuff, self->text);
    return 0;
}

static void mAnimationEdit_append(mAnimationEdit *self, const char* str)
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
    
    animation_modify_string (self, str, TRUE);
}

static void mAnimationEdit_tailDelete(mAnimationEdit *self, int num) 
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

    animation_modify_string (self, &self->text[strlen(self->text) - num], FALSE);
}

static BOOL mAnimationEdit_onEraseBkgnd(mAnimationEdit *self, HDC hdc, const RECT *pinv)
{
    if(Body)
        _c(Body)->paint(Body, hdc, NULL, 0);
    return TRUE;
}

static void mAnimationEdit_onPaint (mAnimationEdit *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    int h;
    int y;
    DWORD color;
    SIZE size;
    SetBkMode(hdc,BM_TRANSPARENT);

    /* get font y  coordinate */
    GetTextExtent (hdc, "A", 1, &size);
    h = GetGDCapability (hdc, GDCAP_MAXY) + 1;
    y = (h - size.cy)/2;

    color = (DWORD)self->text_color;
    SetTextColor (hdc, RGBA2Pixel(hdc, color>>16&0xff, color>>8&0xff, color&0xff, color>>24&0xff));
    TextOut (hdc, self->text_offest+1, y+1, self->text);

    color = ~color;
    SetTextColor (hdc, RGBA2Pixel(hdc, color>>16&0xff,color>>8&0xff, color&0xff, color>>24&0xff));
    TextOut (hdc, self->text_offest, y, self->text);
}

static mObject* mAnimationEdit_createBody(mAnimationEdit *self)
{
    int w;
    int h;
    RECT rc;
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    mShapeTransRoundPiece *body = NEWPIECE(mShapeTransRoundPiece);

    GetWindowRect (self->hwnd, &rc);
    w = RECTW(rc);
    h = RECTH(rc);
    SetRect (&rc, 0, 0, w, h);
    _c(body)->setRect (body, &rc);

    if (dwStyle & NCSS_ANIMATIONEDIT_BKGRADIENT) {
        _c(body)->setProperty (body, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    }
    _c(body)->setProperty (body, NCSP_TRANROUND_CORNERFLAG, 0);

    return (mObject*)body;
}

static BOOL mAnimationEdit_setGradientFillColors (mAnimationEdit *self, ARGB *colors, int num)
{
    mShapeTransRoundPiece *body = (mShapeTransRoundPiece*)self->body;
    if(body) {
        return _c(body)->setGradientFillColors (body, colors, num);
    }
    return FALSE;
}

static BOOL mAnimationEdit_setProperty(mAnimationEdit *self, int id, DWORD value)
{
    mShapeTransRoundPiece *body = (mShapeTransRoundPiece*)self->body;

    if (body) {
        switch (id) {
            case NCSP_ANIMATIONEDIT_RADIUS:
                return _c(body)->setProperty(body, NCSP_TRANROUND_RADIUS, value);
            case NCSP_ANIMATIONEDIT_BKCOLOR:
                return _c(body)->setProperty(body, NCSP_TRANROUND_BKCOLOR, value);
            case NCSP_ANIMATIONEDIT_BORDERSIZE:
                return _c(body)->setProperty(body, NCSP_TRANROUND_BORDERSIZE, value);
            case NCSP_ANIMATIONEDIT_BORDERCOLOR:
                return _c(body)->setProperty(body, NCSP_TRANROUND_BORDERCOLOR, value);
            case NCSP_ANIMATIONEDIT_FILLMODE:
                return _c(body)->setProperty(body, NCSP_TRANROUND_FILLMODE, value);
            case NCSP_ANIMATIONEDIT_CORNERFLAG:
                return _c(body)->setProperty(body, NCSP_TRANROUND_CORNERFLAG, value);
            default:
                break;
        }
    }

    if (id == NCSP_ANIMATIONEDIT_TEXTCOLOR) {
        self->text_color = (ARGB)value;
    }

    return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mAnimationEdit_getProperty(mAnimationEdit *self, int id)
{
    mShapeTransRoundPiece *body = (mShapeTransRoundPiece*)self->body;
    if (body) {
        switch (id) {
            case NCSP_ANIMATIONEDIT_RADIUS:
                return _c(body)->getProperty(body, NCSP_TRANROUND_RADIUS);
            case NCSP_ANIMATIONEDIT_BKCOLOR:
                return _c(body)->getProperty(body, NCSP_TRANROUND_BKCOLOR);
            case NCSP_ANIMATIONEDIT_BORDERSIZE:
                return _c(body)->getProperty(body, NCSP_TRANROUND_BORDERSIZE);
            case NCSP_ANIMATIONEDIT_BORDERCOLOR:
                return _c(body)->getProperty(body, NCSP_TRANROUND_BORDERCOLOR);
            case NCSP_ANIMATIONEDIT_FILLMODE:
                return _c(body)->getProperty(body, NCSP_TRANROUND_FILLMODE);
            case NCSP_ANIMATIONEDIT_CORNERFLAG:
                return _c(body)->getProperty(body, NCSP_TRANROUND_CORNERFLAG);
            default:
                break;
        }
    }
    if (id == NCSP_ANIMATIONEDIT_TEXTCOLOR) {
        return (DWORD) self->text_color;
    }

    return Class(mWidget).getProperty((mWidget*)self, id);
}

static HBRUSH mAnimationEdit_getBKBrush (mAnimationEdit *self)
{
    mShapeTransRoundPiece *body = (mShapeTransRoundPiece*)self->body;

    if(body) {
        return _c(body)->getBrush (body);
    }

    return MP_INV_HANDLE;
}


BEGIN_CMPT_CLASS(mAnimationEdit, mWidget)
CLASS_METHOD_MAP(mAnimationEdit, construct)
CLASS_METHOD_MAP(mAnimationEdit, destroy)
CLASS_METHOD_MAP(mAnimationEdit, onPaint)
CLASS_METHOD_MAP(mAnimationEdit, setContent)
CLASS_METHOD_MAP(mAnimationEdit, getContent)
CLASS_METHOD_MAP(mAnimationEdit, append)
CLASS_METHOD_MAP(mAnimationEdit, tailDelete)
CLASS_METHOD_MAP(mAnimationEdit, onEraseBkgnd)
CLASS_METHOD_MAP(mAnimationEdit, createBody)
CLASS_METHOD_MAP(mAnimationEdit, getBKBrush)
CLASS_METHOD_MAP(mAnimationEdit, getProperty)
CLASS_METHOD_MAP(mAnimationEdit, setProperty)
CLASS_METHOD_MAP(mAnimationEdit, setGradientFillColors)
END_CMPT_CLASS

