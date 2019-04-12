/* 
 ** $Id: miconflow.c 620 2009-12-21 03:00:13Z ZhaolinHu $
 **
 ** The implementation of mIconFlow class.
 **
 ** Copyright (C) 2009 Feynman Software.
 **
 ** All rights reserved by Feynman Software.
 */

#define dbg()  fprintf(stderr, "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "miconflow.h"

#include <mgncs/mrdr.h>


#define DEF_ICON_WIDTH  100
#define DEF_ICON_HEIGHT 100
#define DEF_VISITEMCOUNT  7

static void mIconFlow_drawItem (mIconFlow *self, HITEM hItem, HDC hdc, RECT *rcDraw)
{
    PBITMAP bmp;
    int x, y;
    int W, H, w, h;
    float z;
    HDC hdc_bmp, hdc_icon;

    bmp = (PBITMAP)(_c(self)->getImage(self, hItem));
    assert(bmp);

    x = rcDraw->left + ( RECTWP(rcDraw) - self->defItemWidth ) / 2;
    if (x < rcDraw->left) {
        x = rcDraw->left;
    }
    y = rcDraw->top + ( RECTHP(rcDraw) - self->defItemHeight ) / 2;
    if (y < rcDraw->top) {
        y = rcDraw->top;
    }

    W = self->defItemWidth;
    H = self->defItemHeight;
    w = RECTWP(rcDraw);
    h = RECTHP(rcDraw);
    z = (float)w / (float)self->defItemWidth;

    hdc_bmp = CreateCompatibleDCEx (hdc, W , H);
    hdc_icon = CreateCompatibleDCEx (hdc, W, H);

    if (self->iconUseZoom)
        FillBoxWithBitmap (hdc_bmp, 0, 0, W, H, bmp);
    else
        FillBoxWithBitmapPart (hdc_bmp, 0, 0, W, H, 0, 0,
                bmp, bmp->bmWidth / 2 - W / 2, bmp->bmHeight / 2 - H / 2);

    if (self->iconFrameDC != HDC_INVALID) {
        SetBrushColor (hdc_bmp, GetPixel (self->iconFrameDC, 0, 0));
        FillBox (hdc_bmp, 0, 0, 5, H);
        FillBox (hdc_bmp, 0, 0, W, 5);
        FillBox (hdc_bmp, 0, H - 4, W, H);
        FillBox (hdc_bmp, W - 4, 0, W, H);

        SetMemDCColorKey (self->iconFrameDC, MEMDC_FLAG_SRCCOLORKEY, GetPixel (self->iconFrameDC, 0, 0));
        //        SetMemDCAlpha (self->iconFrameDC, MEMDC_FLAG_SRCALPHA, 255);
        BitBlt (self->iconFrameDC, 0, 0, 0, 0, hdc_bmp, 0, 0, 0);
        //    StretchBlt (self->iconFrameDC, 0, 0, 0, 0, hdc_bmp, 0, 0, W, H, 0);

        SetMemDCColorKey (hdc_icon, MEMDC_FLAG_SRCCOLORKEY, GetPixel (self->iconFrameDC, 0, 0));
    }

    StretchBlt (hdc_bmp, 0, 0, 0, 0, hdc_icon, 0, 0, w, h, 0);
    
    if (self->iconBorder) {
        unsigned b = self->iconBorder / 2;
        int lh = h - b - 1;
        int lw = w - b - 1;
        SetPenWidth (hdc_icon, self->iconBorder);
        //SetPenColor (hdc_icon, PIXEL_lightwhite);
        SetPenColor (hdc_icon, 0xffffffff);
        LineEx (hdc_icon, b, b, b, lh);
        LineEx (hdc_icon, b, b, lw, b);
        LineEx (hdc_icon, b, lh, lw + 1, lh);
        LineEx (hdc_icon, lw, b, lw, lh + 1);
    }

    SetMemDCAlpha (hdc_icon, MEMDC_FLAG_SRCALPHA, z * z * 255);
    BitBlt (hdc_icon, 0, 0, w, h, hdc, x, y, 0);
    DeleteCompatibleDC (hdc_icon);
    DeleteCompatibleDC (hdc_bmp);
}

static void mIconFlow_destroy (mIconFlow *self)
{
    if (self->iconFrameDC != HDC_INVALID && self->iconFrameDC != HDC_SCREEN)
        DeleteCompatibleDC (self->iconFrameDC);

    if (self->bkgndPiece)
        DELPIECE(self->bkgndPiece);

    Class(mItemView).destroy ((mItemView *)self);
}

static int mIconFlow_inItem (mIconFlow *self, int mouseX, int mouseY, 
                     HITEM *pRet, POINT *pt)
{
    int i, index = -1;
    HITEM hItem = 0;
    int visItemCountP1 = self->visItemCount + 1;

    if (mouseX < 0 || mouseX > self->visWidth)
        return -1;
    if (mouseY < 0 || mouseY > self->visHeight)
        return -1;

    for (i = 0; i < visItemCountP1; ++i) {
        if (       mouseX > (self->point3d[i].x - self->defItemWidth * self->point3d[i].z / 2)
                && mouseX < (self->point3d[i].x + self->defItemWidth * self->point3d[i].z / 2))
            if (       mouseY > (self->point3d[i].y - self->defItemHeight * self->point3d[i].z / 2)
                    && mouseY < (self->point3d[i].y + self->defItemHeight * self->point3d[i].z / 2)) {

                int count = _c(self)->getItemCount (self);
                index = _c(self)->indexOf (self, _c(self)->getHilight (self));
                if ((i + 1) % 2 == 0) {
                    index = index + (i + 1) / 2;
                    if (index >= count)
                        index -= count;
                } else {
                    index = index - (i + 1) / 2;
                    if (index < 0)
                        index += count;
                }
                hItem = _c(self)->getItem (self, index);
                printf ("index = %d\n", index);

                break;
            }
    }

    if (pRet)
        *pRet = hItem;

    if (pt);

    return index;
}

static HITEM mIconFlow_addItem(mIconFlow *self, 
        NCS_ICONFLOW_ITEMINFO *info, int *pos)
{
    HITEM   hItem = 0;
    int     idx;

    if (!info)
        return 0;

    hItem = _c(self)->createItem(self, 0, 0, info->index, 
            self->defItemHeight, info->label, info->addData, &idx, FALSE);
    
    if (!hItem) {
        return 0;
    }

    if (pos)
        *pos = idx;

    //set image
    if (info && info->bmp)
        _c(self)->setImage(self, hItem, ((DWORD)info->bmp));
   
    _c(self)->refreshItem(self, hItem, NULL);

    return hItem;
}

static int
mIconFlow_onKeyDown (mIconFlow* self, int scancode, int state)
{
    if (self->direction != ICONFLOW_DIR_NONE)
        return 0;

    switch (scancode) {
        case SCANCODE_CURSORBLOCKLEFT:
        case SCANCODE_CURSORBLOCKUP:
//            if (self->prevIndex < _c(self)->getItemCount (self) - 1)
                _c(self)->beginAnimation (self, self->prevIndex, self->prevIndex + 1.0f);
            break;
        case SCANCODE_CURSORBLOCKRIGHT:
        case SCANCODE_CURSORBLOCKDOWN:
//            if (self->prevIndex > 0)
                _c(self)->beginAnimation (self, self->prevIndex, self->prevIndex - 1.0f);
            break;
        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
            ncsNotifyParentEx ((mWidget*)self, NCSN_ITEMV_ENTER, (DWORD)_c(self)->getHilight (self));
            return 0;
    }

    return 1;
}

static int
mIconFlow_onSizeChanged (mIconFlow* self, RECT *rcClient)
{
    Class(mScrollWidget).onSizeChanged((mScrollWidget*)self, rcClient);
    return 0;
}

static void
mIconFlow_setVisItemCenter (mIconFlow *self, int count, int width, int height)
{
    int i, pos, m;
    ICONFLOW_POINT3D pt3d = {self->visWidth >> 1, self->visHeight >> 1, 1.0f};

    int visItemCountP2 = count + 2;

    for (i = 0; i < visItemCountP2; ++i) {
        pos = visItemCountP2 - 1 - i;

        memcpy (&self->point3d[pos], &pt3d, sizeof(ICONFLOW_POINT3D));

        if (((i + 1) % 2)) {
            m = (i + 1) / 2;
            for (; m < visItemCountP2 / 2; ++m) {
                self->point3d[pos].z = self->point3d[pos].z * 2.0 / 3.0;
                if (self->isVertical)
                    self->point3d[pos].y -= self->span * self->point3d[pos].z * self->point3d[pos].z;
                else
                    self->point3d[pos].x -= self->span * self->point3d[pos].z * self->point3d[pos].z;
            }
        } else {
            m = visItemCountP2 - (i + 1) / 2;
            for (; m > visItemCountP2 / 2; --m) {
                self->point3d[pos].z = self->point3d[pos].z * 2.0 / 3.0;
                if (self->isVertical)
                    self->point3d[pos].y = self->visHeight - self->point3d[pos + 1].y;
                else
                    self->point3d[pos].x = self->visWidth - self->point3d[pos + 1].x;
            }
        }
    }
    /*
    for (i = 0; i < visItemCountP2; ++i)
        printf ("%d ", self->point3d[i].y);
    printf ("\n");
    */
}

static void
mIconFlow_setIconSize(mIconFlow *self, int width, int height)
{
    if (width <= 0)
        self->defItemWidth = DEF_ICON_WIDTH;
    else 
        self->defItemWidth = width;

    if (height <= 0)
        self->defItemHeight = DEF_ICON_HEIGHT;
    else 
        self->defItemHeight = height;

    _c(self)->setVisItemCenter (self,
            self->visItemCount, self->defItemWidth, self->defItemHeight);

    InvalidateRect(self->hwnd, NULL, TRUE);
}

static void
mIconFlow_setVisItemCount (mIconFlow *self, int count)
{
    if (count <= 0)
        self->visItemCount = DEF_VISITEMCOUNT;

    if (count % 2 == 0)
        ++self->visItemCount;

    _c(self)->setVisItemCenter (self,
            self->visItemCount, self->defItemWidth, self->defItemHeight);

    InvalidateRect(self->hwnd, NULL, TRUE);
}

static void
mIconFlow_setSpan (mIconFlow *self, int span)
{
    if (span <= 0)
        return;

    self->span = span;

    _c(self)->setVisItemCenter (self,
            self->visItemCount, self->defItemWidth, self->defItemHeight);

    InvalidateRect(self->hwnd, NULL, TRUE);
}

static void
mIconFlow_beginAnimation (mIconFlow *self, float startvalue, float endvalue)
{
    float distance = startvalue - endvalue;
    
    if (distance > -0.1f && distance < 0.1f) {
        return;
    }

    if (distance > 0) {
        self->direction = ICONFLOW_DIR_PREV;
    } else {
        self->direction = ICONFLOW_DIR_NEXT;
    }

    _c(self)->runAnimation (self, startvalue, endvalue);
}

static void
mIconFlow_endAnimation (mIconFlow *self)
{
    int count = _c(self)->getItemCount (self);
    while (self->key >= count)
        self->key -= count;
    while (self->key < 0)
        self->key += count;
    self->prevIndex = self->key;
    _c(self)->hilight (self, _c(self)->getItem (self , self->prevIndex));

    self->direction = ICONFLOW_DIR_NONE;
}

static int g_finalFrame = 0;

static BOOL
mIconFlow_onEraseBkgnd(mIconFlow *self, HDC hdc, const RECT *pinv)
{
    return TRUE;
}

static void
mIconFlow_onPaint(mIconFlow *self, HDC real_hdc, const PCLIPRGN pinv_clip)
{
    int visItemCountP1;
    int count = _c(self)->getItemCount (self);
    HITEM *hItem;
    RECT *rcDraw;
    int curCenterX, curCenterY;
    int *sortLine;
    float cursor;
    int index;
    float move;
    int i;
    HDC hdc;

    if (count == 0)
        return;

    visItemCountP1 = self->visItemCount + 1;
    hItem = (HITEM*)calloc (visItemCountP1, sizeof (HITEM));
    rcDraw = (RECT*)calloc (visItemCountP1, sizeof (RECT));
    sortLine = (int*)calloc (visItemCountP1, sizeof (int));
    cursor = self->key;
    while (cursor >= count)
        cursor -= count;
    while (cursor < 0)
        cursor += count;
    index = cursor;
    move = cursor - index;

    for (i = 0; i < visItemCountP1; ++i) {
        int idx_p3d; /* index of point3d */
        float curScale;
        int curWidth;
        int curHeight;
        int j, m;
        
        if (i % 2 == 0) {
            index -= i;
            idx_p3d = i + 2;
            if (index < 0)
                index += count;
        } else {
            index += i;
            idx_p3d = (i == 1 ? i - 1 : i - 2);
            if (index >= count)
                index -= count;
        }

        hItem[i] = _c(self)->getItem (self, index);

        if (self->isVertical) {
            curCenterX = self->point3d[i].x;
            curCenterY = self->point3d[i].y
                + move * (self->point3d[idx_p3d].y - self->point3d[i].y);
            if (curCenterY == self->point3d[visItemCountP1].y)
                hItem[i] = (HITEM)NULL;
        } else {
            curCenterY = self->point3d[i].y;
            curCenterX = self->point3d[i].x
                + move * (self->point3d[idx_p3d].x - self->point3d[i].x);
            if (curCenterX == self->point3d[visItemCountP1].x)
                hItem[i] = (HITEM)NULL;
        }
        
        curScale    = self->point3d[i].z
                + move * (self->point3d[idx_p3d].z - self->point3d[i].z);
        curWidth    = self->defItemWidth * curScale;
        curHeight   = self->defItemHeight * curScale;

#if 1
//iconflow       
        rcDraw[i].left    = curCenterX - curWidth / 2;
        rcDraw[i].top     = curCenterY - curHeight / 2;
        rcDraw[i].right   = rcDraw[i].left + curWidth;
        rcDraw[i].bottom  = rcDraw[i].top + curHeight;
#else
//iconslide
        rcDraw[i].left    = (i - move) * self->defItemWidth / 2;
        rcDraw[i].top     = 0;
        rcDraw[i].right   = rcDraw[i].left + self->defItemWidth / 2;
        rcDraw[i].bottom  = rcDraw[i].top + self->defItemHeight / 2;
#endif   
        m = i;
        for (j = i - 1; j >= 0 && RECTH(rcDraw[i]) > RECTH(rcDraw[sortLine[j]]); --j)
            --m;
        for (j = i; j >= m; --j)
            sortLine[j] = sortLine[j - 1];
        sortLine[m] = i;
    }

    hdc = CreateCompatibleDC (real_hdc);

    if (self->bkgndPiece) {
        _c(self->bkgndPiece)->paint(self->bkgndPiece, hdc, (mObject*)self, 0);
    } else {
        SetBrushColor (hdc, RGB2Pixel(hdc, 0, 0, 0));
        FillBox (hdc, 0, 0, GetGDCapability(hdc, GDCAP_MAXX) + 1, GetGDCapability(hdc, GDCAP_MAXY) + 1);
    }

    for (i = visItemCountP1 - 1; i >= 0; --i) {
        if (hItem[sortLine[i]])
            _c(self)->drawItem (self, hItem[sortLine[i]], hdc, &rcDraw[sortLine[i]]);
    }

    BitBlt (hdc, 0, 0, 0, 0, real_hdc, 0, 0, -1);
    DeleteMemDC (hdc);

    free (hItem);
    free (rcDraw);
    free (sortLine);

    if (g_finalFrame) {
        _c(self)->endAnimation (self);
        g_finalFrame = 0;
    }
}

static void
_finished_cb (MGEFF_ANIMATION animation)
{
    g_finalFrame = 1;
}

static void call_draw (MGEFF_ANIMATION anim, void *target, intptr_t id, void *value)
{
    UpdateWindow ((HWND)target, TRUE);
}

static void
mIconFlow_runAnimation (mIconFlow *self, float startvalue, float endvalue)
{
    float distance = startvalue - endvalue;
    int duration = 200;
    MGEFF_ANIMATION group;
    MGEFF_ANIMATION drawAnim;
    EffPropertyAnimationSetting settings[] = {
        {
            &self->key, MGEFF_FLOAT,
            (void *)&startvalue, (void *)&endvalue,
            InOutCubic, duration, _finished_cb
        },
        {NULL, 0, NULL, NULL, 0, 0, NULL}
    };

    if (distance < 0)
        distance =  -distance;
    if (distance >= 1)
        duration /= distance;
    else
        duration *= distance;

    group = mGEffCreatePropertyAnimation (settings);
    drawAnim = mGEffAnimationCreate ((void*)self->hwnd, call_draw, 0, MGEFF_INT);
    mGEffAnimationSetDuration (drawAnim, settings->duration);
    mGEffAnimationAddToGroup (group, drawAnim);

    mGEffAnimationAsyncRun (group);
    mGEffAnimationSetProperty(group, MGEFF_PROP_KEEPALIVE, 0);
}

static void
mIconFlow_construct (mIconFlow *self, DWORD add_data)
{
    Class(mItemView).construct((mItemView*)self, add_data);

    self->bkgndPiece = NULL;
    self->key = 0.0f;
    self->prevIndex = 0;
    self->visItemCount = DEF_VISITEMCOUNT;
    self->direction = ICONFLOW_DIR_NONE;
    self->iconFrameDC = HDC_INVALID;
    self->isVertical = FALSE;
    self->iconBorder = 0;
    self->span = DEF_ICON_WIDTH * 1.6;
    self->iconUseZoom = FALSE;
}

static BOOL
mIconFlow_onCreate (mIconFlow *self, LPARAM lParam)
{
//    if (_c(self)->getItemCount (self) > 0) {
        _c(self)->hilight (self, _c(self)->getItem (self, self->prevIndex));
//    }

    if (GetWindowStyle(self->hwnd) & NCSS_ICONFLOW_VERTICAL) {
        self->isVertical = TRUE;
        self->span = DEF_ICON_HEIGHT * 1.6;
    }

    _c(self)->setIconSize (self, self->visWidth / 4, self->visHeight / 4);

    Class (mScrollWidget).setProperty ((mScrollWidget *)self, NCSP_SWGT_DRAWMODE, NCSID_SWGT_NEVER);

    return TRUE;
}

//TODO: delete this if KS_LEFTBUTTON's bug is fixed
static int lbuttondown = 0;
static unsigned mouse_down_and_move = 0;

static int
mIconFlow_wndProc (mIconFlow *self, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_LBUTTONDOWN:
            {
                self->mouse.x = LOSWORD (lParam);
                self->mouse.y = HISWORD (lParam);
                lbuttondown = 1;
                mouse_down_and_move = 0;
            }
            return 0;
        case MSG_MOUSEMOVE:
            if (self->direction == ICONFLOW_DIR_NONE 
                    //TODO: delete this if KS_LEFTBUTTON's bug is fixed
                    && lbuttondown
//                    && (GetShiftKeyStatus() & KS_LEFTBUTTON) 
                    ) {
                if (self->isVertical) {
                    if (self->mouse.y - HISWORD (lParam) > 3)
                        self->key += 0.25f;
                    else if (HISWORD (lParam) - self->mouse.y > 3)
                        self->key -= 0.25f;
                    else
                        return 0;
                } else {
                    if (self->mouse.x - LOSWORD (lParam) > 3)
                        self->key += 0.25f;
                    else if (LOSWORD (lParam) - self->mouse.x > 3)
                        self->key -= 0.25f;
                    else
                        return 0;
                }

                self->mouse.x = LOSWORD (lParam);
                self->mouse.y = HISWORD (lParam);
                g_finalFrame = 1;
                InvalidateRect(self->hwnd, NULL, TRUE);

                if (!mouse_down_and_move)
                    mouse_down_and_move = 1;
            }
            return 0;
        case MSG_LBUTTONUP:
            {
                HITEM hItem = 0;
#if 0
                int index;
                index = _c(self)->inItem (self, self->mouse.x, self->mouse.y, &hItem, NULL);
#endif

                if (self->key - self->prevIndex < 0.5) {
                    _c(self)->beginAnimation (self, self->key, self->prevIndex);
                } else {
                    _c(self)->beginAnimation (self, self->key, self->prevIndex + 1.0f);
                }

                if (hItem == _c(self)->getHilight (self) && !mouse_down_and_move)
                    ncsNotifyParentEx ((mWidget*)self, NCSN_ICONFLOW_CLICKED, (DWORD)hItem);

                //TODO: delete this if KS_LEFTBUTTON's bug is fixed
                lbuttondown = 0;
                mouse_down_and_move = 0;
            }
            return 0;
        case MSG_MOUSEMOVEIN:
            if (!wParam && lbuttondown)
                SendMessage(self->hwnd, MSG_LBUTTONUP, wParam, lParam);
            return 0;
    }

    return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

static BOOL
mIconFlow_setProperty(mIconFlow* self, int id, DWORD value)
{
    if( id >= NCSP_ICONFLOW_MAX)
        return FALSE;

    switch(id) {
        case NCSP_ICONFLOW_DEFICONHEIGHT:
            _c(self)->setIconSize(self, self->defItemWidth, value);
            return TRUE;
        case NCSP_ICONFLOW_DEFICONWIDTH:
            _c(self)->setIconSize(self, value, self->defItemHeight);
            return TRUE;
        case NCSP_ICONFLOW_BKGNDPIECE:
            self->bkgndPiece = (mHotPiece*)value;
            return TRUE;
        case NCSP_ICONFLOW_VISITEMCOUNT:
            _c(self)->setVisItemCount (self, value);
            return TRUE;
        case NCSP_ICONFLOW_ICONFRAME:
#if 0
            {
                PBITMAP bmp = (PBITMAP)value;
                if (bmp == NULL) {
                    printf ("Err: background icon file not found\n");
                    assert (0);
                    return FALSE;
                }
                if (self->iconFrameDC != HDC_INVALID && self->iconFrameDC != HDC_SCREEN)
                    DeleteCompatibleDC(self->iconFrameDC);
                self->iconFrameDC = CreateCompatibleDCEx (HDC_SCREEN, self->defItemWidth, self->defItemHeight);
                FillBoxWithBitmap (self->iconFrameDC, 0, 0, self->defItemWidth, self->defItemHeight, bmp);
            }
#endif
            return TRUE;
        case NCSP_ICONFLOW_SPAN:
            _c(self)->setSpan (self, (int)value);
            return TRUE;
        case NCSP_ICONFLOW_ICONBORDER:
            self->iconBorder = (unsigned)value;
            return TRUE;
        case NCSP_ICONFLOW_ICONUSEZOOM:
            self->iconUseZoom = (int)value == 0 ? FALSE : TRUE;
            return TRUE;
    }

    return Class(mItemView).setProperty((mItemView*)self, id, value);
}

static DWORD
mIconFlow_getProperty(mIconFlow* self, int id)
{
    if( id >= NCSP_ICONFLOW_MAX)
        return -1;

    switch(id)
    {
        case NCSP_ICONFLOW_DEFICONWIDTH:
            return self->defItemWidth;
        case NCSP_ICONFLOW_DEFICONHEIGHT:
            return self->defItemHeight;
        case NCSP_ICONFLOW_BKGNDPIECE:
            return (DWORD)self->bkgndPiece;
        case NCSP_ICONFLOW_VISITEMCOUNT:
            return self->visItemCount;
        case NCSP_ICONFLOW_ICONFRAME:
            return (DWORD)self->iconFrameDC;
        case NCSP_ICONFLOW_SPAN:
            return (DWORD)self->span;
        case NCSP_ICONFLOW_ICONBORDER:
            return (DWORD)self->iconBorder;
        case NCSP_ICONFLOW_ICONUSEZOOM:
            return (DWORD)self->iconUseZoom;
    }

    return Class(mItemView).getProperty((mItemView*)self, id);
}

BEGIN_CMPT_CLASS(mIconFlow, mItemView)
    CLASS_METHOD_MAP(mIconFlow, drawItem);
    CLASS_METHOD_MAP(mIconFlow, construct);
    CLASS_METHOD_MAP(mIconFlow, destroy);
    CLASS_METHOD_MAP(mIconFlow, setProperty);
    CLASS_METHOD_MAP(mIconFlow, getProperty);
    CLASS_METHOD_MAP(mIconFlow, onSizeChanged);
    CLASS_METHOD_MAP(mIconFlow, beginAnimation);
    CLASS_METHOD_MAP(mIconFlow, endAnimation);
    CLASS_METHOD_MAP(mIconFlow, onCreate);
    CLASS_METHOD_MAP(mIconFlow, onPaint);
    CLASS_METHOD_MAP(mIconFlow, onEraseBkgnd);
    CLASS_METHOD_MAP(mIconFlow, onKeyDown);
    CLASS_METHOD_MAP(mIconFlow, runAnimation);
    CLASS_METHOD_MAP(mIconFlow, wndProc);
    CLASS_METHOD_MAP(mIconFlow, inItem);
    CLASS_METHOD_MAP(mIconFlow, addItem);
    CLASS_METHOD_MAP(mIconFlow, setVisItemCenter);
    CLASS_METHOD_MAP(mIconFlow, setIconSize);
    CLASS_METHOD_MAP(mIconFlow, setVisItemCount);
    CLASS_METHOD_MAP(mIconFlow, setSpan);
    SET_DLGCODE(DLGC_WANTARROWS);
END_CMPT_CLASS
