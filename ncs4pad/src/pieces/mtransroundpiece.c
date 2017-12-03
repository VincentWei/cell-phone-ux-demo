#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mtransroundpiece.h"
#include "mfillboxex.h"

static void mShapeTransRoundPiece_construct(mShapeTransRoundPiece *self, DWORD add_data)
{
    RECT rc;
    //ARGB colors[] = {0xff222222, 0xff333333, 0xff444444, 0xff666666};
    ARGB colors[] = {0xff1a1a1a,0xff505050,0xff555555,0xff868890};
    float pos[] = {0.0, 0.2, 0.9, 1.0};

	Class(mStaticPiece).construct((mStaticPiece*)self, add_data);
    _c(self)->getRect(self, &rc);

    self->hgs = MP_INV_HANDLE;
    self->hgs_width = RECTW(rc);
    self->hgs_height = RECTH(rc);
    self->brush_solid = MGPlusBrushCreate (MP_BRUSH_TYPE_SOLIDCOLOR);
    self->brush_gradient = MGPlusBrushCreate (MP_BRUSH_TYPE_LINEARGRADIENT);
    MGPlusSetLinearGradientBrushMode (self->brush_gradient, MP_LINEAR_GRADIENT_MODE_VERTICAL);
    self->brush_gradient_border = MGPlusBrushCreate (MP_BRUSH_TYPE_LINEARGRADIENT);
    MGPlusSetLinearGradientBrushMode (self->brush_gradient_border, MP_LINEAR_GRADIENT_MODE_VERTICAL);
    MGPlusSetLinearGradientBrushColorsEx(self->brush_gradient_border, colors, TABLESIZE(colors), pos);

#define DEFAULT_CORNER_RADIUS 0
#define DEFALUT_BORDER_CORLOR 0xff000000
    self->paint_mode = TRANROUND_PAINTMODE_BITBLT;
    self->fill_mode = NCSP_TRANROUND_SINGLE_FILL;
    self->corner_radius = DEFAULT_CORNER_RADIUS;
    self->corner_flag = 0xf;
    self->border_size = 0;
    self->border_color = DEFALUT_BORDER_CORLOR;
    self->use_gradient_border = FALSE;
    self->bk_color = MakeRGBA(0, 0, 0xff, 0xff);
    self->sharp_flag = 0;
    self->sharp_width = 15;
    self->use_shadow = FALSE;
#if 0
    self->fill_engine = TRANROUND_FILLENGINE_PLUS;
#else
    self->fill_engine = TRANROUND_FILLENGINE_NORMAL;
#endif
}

HBRUSH mShapeTransRoundPiece_getBrush (mShapeTransRoundPiece *self)
{
    if (self->fill_mode == NCSP_TRANROUND_SINGLE_FILL) {
        return self->brush_solid;
    } else if (self->fill_mode == NCSP_TRANROUND_GRADIENT_FILL) {
        return self->brush_gradient;
    }
    return MP_INV_HANDLE;
}

HBRUSH mShapeTransRoundPiece_getBorderBrush(mShapeTransRoundPiece *self)
{
    assert(self->brush_gradient_border);
    return self->brush_gradient_border;
}

static BOOL mShapeTransRoundPiece_setGradientFillColors (mShapeTransRoundPiece* self, ARGB *colors, int num)
{
    if (self->fill_mode == NCSP_TRANROUND_SINGLE_FILL) {
        return FALSE;
    }
    MGPlusSetLinearGradientBrushColors (self->brush_gradient, colors, num);
    return TRUE;
}

static BOOL mShapeTransRoundPiece_setProperty(mShapeTransRoundPiece* self, int id, DWORD value)
{
    switch (id) {
        case NCSP_TRANROUND_BORDERCOLOR:
            self->border_color = (ARGB)value;
            break;
        case NCSP_TRANROUND_BKCOLOR:
            self->bk_color = (ARGB)value;
            break;
        case NCSP_TRANROUND_RADIUS:
            self->corner_radius = (int)value;    
            break;
        case NCSP_TRANROUND_BORDERSIZE:
            self->border_size = (int)value;
            break;
        case NCSP_TRANROUND_GRADIENTBORDER:
            self->use_gradient_border = ((int)value == 0 ? FALSE : TRUE);
        case NCSP_TRANROUND_FILLMODE:
            self->fill_mode = (TRANROUND_FILLMODE)value;
            break;
        case NCSP_TRANROUND_CORNERFLAG:
            self->corner_flag = (int)value;
            break;
        case NCSP_TRANROUND_PAINTMODE:
            self->paint_mode = (TRANROUND_PAINTMODE)value;
            break;
        case NCSP_TRANROUND_SHARPFLAG:
            self->sharp_flag = (int)value;
            break;
        case NCSP_TRANROUND_SHARPWIDTH:
            self->sharp_width = (int)value;
            break;
        case NCSP_TRANROUND_USESHADOW:
            self->use_shadow = ((int)value == 0 ? FALSE : TRUE);
            break;
        case NCSP_TRANROUND_FILLENGINE:
            self->fill_engine = value;
            break;
        default:
            return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
    }
    return TRUE;
}

static DWORD mShapeTransRoundPiece_getProperty(mShapeTransRoundPiece* self, int id)
{
    switch (id) {
        case NCSP_TRANROUND_BORDERCOLOR:
            return (DWORD)self->border_color;
        case NCSP_TRANROUND_BKCOLOR:
            return (DWORD)self->bk_color;
        case NCSP_TRANROUND_RADIUS:
            return (DWORD)self->corner_radius;
        case NCSP_TRANROUND_BORDERSIZE:
            return (DWORD)self->border_size;
        case NCSP_TRANROUND_GRADIENTBORDER:
            return (DWORD)self->use_gradient_border;
        case NCSP_TRANROUND_FILLMODE:
            return (DWORD)self->fill_mode;
        case NCSP_TRANROUND_CORNERFLAG:
            return (DWORD)self->corner_flag;
        case NCSP_TRANROUND_PAINTMODE:
            return (DWORD)self->paint_mode;
        case NCSP_TRANROUND_SHARPFLAG:
            return (DWORD)self->sharp_flag;
        case NCSP_TRANROUND_SHARPWIDTH:
            return (DWORD)self->sharp_width;
        case NCSP_TRANROUND_USESHADOW:
            return (DWORD)self->use_shadow;
        default:
            return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
    }
}


static void setPath (mShapeTransRoundPiece *self, 
        HPATH path, int tx, int ty, int tw, int th)
{
    int sharp_width = self->sharp_width;
    int radius = self->corner_radius;
    int x, y, w, h;
    x = tx, y = ty, w = tw, h = th;
    sharp_width -= x;

    MGPlusPathReset(path);
    if (radius) {
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_LEFT) != 0) {
            x += sharp_width;
            w -= sharp_width;
            // hide left corner
            self->corner_flag &= ~TRANROUND_CORNERFLAG_LEFTTOP;
            self->corner_flag &= ~TRANROUND_CORNERFLAG_LEFTBOTTOM;
        }
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_RIGHT) != 0) {
            w -= sharp_width;
            // hide right corner
            self->corner_flag &= ~TRANROUND_CORNERFLAG_RIGHTTOP;
            self->corner_flag &= ~TRANROUND_CORNERFLAG_RIGHTBOTTOM;
        }

        MGPlusPathAddRoundRect(path, x, y, w, h, radius);
        /* hide left top */
        if ((self->corner_flag & TRANROUND_CORNERFLAG_LEFTTOP) == 0) {
            MGPlusPathAddRectangle (path, x, y, radius, radius);
        }
        /* hide right top */
        if ((self->corner_flag & TRANROUND_CORNERFLAG_RIGHTTOP) == 0) {
            MGPlusPathAddRectangle (path, (x+w)-radius, y, radius, radius);
        }
        /* hide right bottom */
        if ((self->corner_flag & TRANROUND_CORNERFLAG_RIGHTBOTTOM) == 0) {
            MGPlusPathAddRectangle (path, 
                    (x+w)-radius, (y+h)-radius, radius, radius);
        }
        /* hide left bottom */
        if ((self->corner_flag & TRANROUND_CORNERFLAG_LEFTBOTTOM) == 0) {
            MGPlusPathAddRectangle (path, x, (y+h)-radius, radius, radius);
        }

        // add left sharp
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_LEFT) != 0) {
            MPPOINT point_lines[] = {{x, y}, {tx, (h+1)>>1}, {x, h}};
            MGPlusPathAddLines(path, point_lines, 3);
        }
        // add right sharp
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_RIGHT) != 0) {
            MPPOINT point_lines[] = {{x+w+sharp_width, h>>1}, {x+w, y}, {x+w, h}};
            MGPlusPathAddLines(path, point_lines, 3);
        }
    } else {
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_LEFT) != 0) {
            x += sharp_width;
            w -= sharp_width;
        }
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_RIGHT) != 0) {
            w -= sharp_width;
        }

        //MGPlusPathAddRectangle (path, x, y, w, h);

        // add left sharp
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_LEFT) != 0) {
            MPPOINT point_lines[] = {{x, y}, {x-w, (h+1)>>1}, {x, h}, {x+w, h}};
            MGPlusPathAddLines(path, point_lines, 4);
        }
        else {
            MPPOINT point_lines[] = {{x, y}, {x, y+h}, {x+w, y+h}};
            MGPlusPathAddLines(path, point_lines, 3);
        }

        // add right sharp
        if ((self->sharp_flag & TRANROUND_SHARPFLAG_RIGHT) != 0) {
            MPPOINT point_lines[] = {{x+w+sharp_width, h>>1}, {x+w, y}, {x, y}};
            MGPlusPathAddLines(path, point_lines, 3);
        }
        else {
            MPPOINT point_lines[] = {{x+w, y}, {x, y}};
            MGPlusPathAddLines(path, point_lines, 2);
        }
    }
}

static void mShapeTransRoundPiece_FillWithPlus(mShapeTransRoundPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
#if 0
    RECT rc;
    int r,g,b;
    gal_pixel pixel;
    
    _c(self)->getRect(self, &rc);
    if (self->fill_mode == NCSP_TRANROUND_SINGLE_FILL
            && self->border_size == 0
            && (self->corner_flag == 0 || self->corner_radius == 0)) {
        pixel = RGB2Pixel(hdc, 0xff, 0, 0);
    }else{
        r = rand() & 0xff;
        g = rand() & 0xff;
        b = rand() & 0xff;
        pixel = RGB2Pixel(hdc, r, g, b);
    }

    SetBrushColor(hdc, pixel);
    FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
    return;
#else
    HBRUSH brush;
    RECT rc, brushRC;
    HPATH path;
    HPEN pen;
    int pen_size_compensate;
    int x = 0, y = 0, w, h;
    int border;

    _c(self)->getRect(self, &rc);
    w = RECTW(rc);
    h = RECTH(rc);
    border = self->border_size;

    if (self->fill_mode == NCSP_TRANROUND_SINGLE_FILL
            && self->border_size == 0
            && (self->corner_flag == 0 || self->corner_radius == 0)) {
        if (GetAValue(self->bk_color)) {
            HDC tmpdc = CreateCompatibleDCEx(hdc, w, h);
            SetBrushColor (tmpdc, ncsColor2Pixel(tmpdc, self->bk_color));
            FillBox(tmpdc, 0, 0, w, h);
            BitBlt(tmpdc, 0, 0, 0, 0, hdc, rc.left, rc.top, 0);
            DeleteMemDC(tmpdc);
        }
        return;
    }

    if (self->hgs == MP_INV_HANDLE)  {
        /* first create hgs */
        self->hgs = MGPlusGraphicCreate (w, h);
    }
    else if (self->hgs_width != w || self->hgs_height != h) {
        /* re-create hgs */
        MGPlusGraphicDelete (self->hgs);
        self->hgs = MGPlusGraphicCreate (w, h);
    }
    else {
        ;
    }

    MGPlusGraphicClear (self->hgs, 0);

    self->hgs_width = w;
    self->hgs_height = h;

    path = MGPlusPathCreate (MP_PATH_FILL_MODE_WINDING);
    pen = MGPlusPenCreate (1, 0xFFFFFFFF);


    /* draw border */
    if (border) {
        if (self->use_gradient_border) {
            setPath(self, path, x, y, w, h);
            brush = self->brush_gradient_border;
            SetRect (&brushRC, x, y, w, h);
            MGPlusSetLinearGradientBrushRect(brush, &brushRC);
            MGPlusFillPath (self->hgs, brush, path);

            x = y = border;
            w -= border * 2;
            h -= border * 2;
            if (self->use_shadow)
                --h;
        }
        else {
            pen_size_compensate = border / 2;
            MGPlusPenSetWidth(pen, pen_size_compensate);
            MGPlusPenSetColor(pen, self->border_color);

            MGPlusDrawRoundRect (self->hgs, pen, 
                    pen_size_compensate, pen_size_compensate, 
                    w-pen_size_compensate*2, h-pen_size_compensate*2, 
                    self->corner_radius-pen_size_compensate);
        }
    }

    setPath (self, path, x, y, w, h); 

    /* fill round rect */
    if (self->fill_mode == NCSP_TRANROUND_SINGLE_FILL) {
        brush = self->brush_solid;
        MGPlusSetSolidBrushColor (brush, self->bk_color);
    }
    else if (self->fill_mode == NCSP_TRANROUND_GRADIENT_FILL) {
        brush = self->brush_gradient;
        SetRect (&brushRC, x, y, w, h);
        MGPlusSetLinearGradientBrushRect (brush, &brushRC);
    }
    else {
        assert (0);
    }

    MGPlusFillPath (self->hgs, brush, path);

    MGPlusPenDelete (pen);
    MGPlusPathDelete (path);

    if (self->paint_mode == TRANROUND_PAINTMODE_BITBLT) {
        /* bitblt to target dc */
        HDC hgs_dc = MGPlusGetGraphicDC (self->hgs);

        if (hgs_dc == MP_GENERIC_ERROR) {
            printf ("get self->hgs dc error!\n");
            return;
        }

        BitBlt (hgs_dc, 0, 0, 0, 0, hdc, rc.left, rc.top, 0);
    }
    else {
        MGPlusGraphicSave(self->hgs, hdc, 0, 0, 0, 0, rc.left, rc.top);
    }
#endif
}

static void mInitRoundRectClipRgn(mShapeTransRoundPiece *self, PCLIPRGN region, RECT *prc, int rx, int ry) 
{
    PCLIPRGN cliprgn1;
    PCLIPRGN cliprgn2;
    RECT rc;
    int r;

    assert(region != 0);

    cliprgn1 = CreateClipRgn();
    cliprgn2 = CreateClipRgn();

    if(rx == ry)
    {
        r = rx;

        //lefttop
        if (self->corner_flag & TRANROUND_CORNERFLAG_LEFTTOP){
            SetRect(&rc, prc->left, prc->top, prc->left+r, prc->top+r);
            InitCircleRegion(cliprgn1, prc->left+r, prc->top+r, r);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }

        //righttop
        if (self->corner_flag & TRANROUND_CORNERFLAG_RIGHTTOP){
            SetRect(&rc, prc->right-r, prc->top, prc->right, prc->top+r);
            InitCircleRegion(cliprgn1, prc->right-r-1, prc->top+r, r);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }

        //rightbottom
        if (self->corner_flag & TRANROUND_CORNERFLAG_RIGHTBOTTOM){
            SetRect(&rc, prc->right-r, prc->bottom-r, prc->right, prc->bottom);
            InitCircleRegion(cliprgn1, prc->right-r-1, prc->bottom-r-1, r);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }

        //leftbottom
        if (self->corner_flag & TRANROUND_CORNERFLAG_LEFTBOTTOM){
            SetRect(&rc, prc->left, prc->bottom-r, prc->left+r, prc->bottom);
            InitCircleRegion(cliprgn1, prc->left+r, prc->bottom-r-1, r);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }
    }
    else
    {
        //lefttop
        if ((self->corner_flag & TRANROUND_CORNERFLAG_LEFTTOP)){
            SetRect(&rc, prc->left, prc->top, prc->left+rx, prc->top+ry);
            InitEllipseRegion(cliprgn1, prc->left+rx, prc->top+ry, rx, ry);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }

        //righttop
        if ((self->corner_flag & TRANROUND_CORNERFLAG_RIGHTTOP)){
            SetRect(&rc, prc->right-rx, prc->top, prc->right, prc->top+ry);
            InitEllipseRegion(cliprgn1, prc->right-rx-1, prc->top+ry, rx, ry);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }

        //rightbottom
        if ((self->corner_flag & TRANROUND_CORNERFLAG_RIGHTBOTTOM)){
            SetRect(&rc, prc->right-rx, prc->bottom-ry, prc->right, prc->bottom);
            InitEllipseRegion(cliprgn1, prc->right-rx-1, prc->bottom-ry-1, rx, ry);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }

        //leftbottom
        if ((self->corner_flag & TRANROUND_CORNERFLAG_LEFTBOTTOM)){
            SetRect(&rc, prc->left, prc->bottom-ry, prc->left+rx, prc->bottom);
            InitEllipseRegion(cliprgn1, prc->left+rx, prc->bottom-ry-1, rx, ry);
            SetClipRgn(cliprgn2, &rc);
            SubtractRegion(cliprgn2, cliprgn2, cliprgn1);
            SubtractRegion(region, region, cliprgn2);
        }
    }

    DestroyClipRgn(cliprgn1);
    DestroyClipRgn(cliprgn2);
}

static void mInitSharpClipRgn(mShapeTransRoundPiece *self, PCLIPRGN region, RECT *prc)
{
    int i;
    POINT p[6];

    assert(region != 0);

    if(self->sharp_width > (RECTWP(prc) >> 1))
        self->sharp_width = RECTWP(prc) >> 1;

    i = 0;
    if(self->sharp_flag & TRANROUND_SHARPFLAG_LEFT){
        self->corner_flag &= ~TRANROUND_CORNERFLAG_LEFTTOP;
        self->corner_flag &= ~TRANROUND_CORNERFLAG_LEFTBOTTOM;

        p[i].x = prc->left + self->sharp_width;
        p[i].y = prc->top;
        i++;
        p[i].x = prc->left;
        p[i].y = prc->top + (RECTHP(prc) >> 1) - 1;
        i++;
        p[i].x = prc->left + self->sharp_width;
        p[i].y = prc->bottom;
        i++;
    }
    else{
        p[i].x = prc->left;
        p[i].y = prc->top;
        i++;
        p[i].x = prc->left;
        p[i].y = prc->bottom;
    }

    if(self->sharp_flag & TRANROUND_SHARPFLAG_RIGHT){
        self->corner_flag &= ~TRANROUND_CORNERFLAG_RIGHTTOP;
        self->corner_flag &= ~TRANROUND_CORNERFLAG_RIGHTBOTTOM;

        p[i].x = prc->right - self->sharp_width;
        p[i].y = prc->bottom;
        i++;
        p[i].x = prc->right - 1;
        p[i].y = prc->top + (RECTHP(prc) >> 1) - 1;
        i++;
        p[i].x = prc->right - self->sharp_width;
        p[i].y = prc->top;
        i++;
    }
    else{
        p[i].x = prc->right - 1;
        p[i].y = prc->bottom;
        i++;
        p[i].x = prc->right - 1;
        p[i].y = prc->top;
        i++;
    }

    InitPolygonRegion(region, p, i);
}

static PCLIPRGN mCreateFillRegionClipRgn(mShapeTransRoundPiece *self, RECT *prc)
{
    PCLIPRGN prgn;

    prgn = CreateClipRgn();

    if(self->sharp_flag) {
        mInitSharpClipRgn(self, prgn, prc);
    } else {
        SetClipRgn(prgn, prc);
    }

    mInitRoundRectClipRgn(self, prgn, prc, self->corner_radius, self->corner_radius);

    return prgn;
}

static void mShapeTransRoundPiece_mFillRegion(mShapeTransRoundPiece *self, HDC hdc, PCLIPRGN prgn, RECT *prc, BOOL fillborder)
{
#if 1
#   define MPGetBValue(argb)      ((BYTE)(argb))
#   define MPGetGValue(argb)      ((BYTE)(((DWORD)(argb)) >> 8))
#   define MPGetRValue(argb)      ((BYTE)((DWORD)(argb) >> 16))
#   define MPGetAValue(argb)      ((BYTE)((DWORD)(argb) >> 24))
    if (self->fill_mode == NCSP_TRANROUND_SINGLE_FILL){
        int color;
        color = (fillborder) ? (self->border_color) : (self->bk_color);
        if (MPGetAValue(color) != 0){
            mFillRegion(hdc, prgn, fillspan_simple,
                    (void*)MakeRGBA(
                        MPGetRValue(color),
                        MPGetGValue(color),
                        MPGetBValue(color),
                        MPGetAValue(color)));
        }
    }else{
        struct multigradient_context context;
        int i;
        ARGB *argb;
        float *position;
        HBRUSH brush;

        brush = (fillborder) ? (self->brush_gradient_border) : (self->brush_gradient);

        memset(&context, 0, sizeof(context));
        context.top = prc->top;
        context.bottom = prc->bottom; 
        context.n = MGPlusLinearGradientBrushGetColorNumber(brush);
        assert(context.n >= 0);

        context.colortable = (gradient_t *)malloc(sizeof(context.colortable[0]) * context.n);
        argb = (ARGB *)malloc(sizeof(argb[0]) * context.n);
        position = (float *)malloc(sizeof(position[0]) * context.n);
        MGPlusLinearGradientBrushGetColors(brush, argb, position);

        for (i=0; i<context.n; ++i) {
            context.colortable[i][0] = (unsigned int)(position[i] * 100);
            context.colortable[i][1] = MakeRGBA(
                    MPGetRValue(argb[i]),
                    MPGetGValue(argb[i]),
                    MPGetBValue(argb[i]),
                    MPGetAValue(argb[i]));
        }

        mFillRegion(hdc, prgn, fillspan_multigradient, &context);

        free(argb);
        free(position);
        free(context.colortable);
    }
#else
    SetBrushColor(hdc, RGB2Pixel(hdc, rand()&0xff, rand()&0xff, rand()&0xff));
    FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
#endif
}

static void mShapeTransRoundPiece_Fill(mShapeTransRoundPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    RECT rc;
    RECT memrc;
    PCLIPRGN prgn;
    HDC tmpdc;

    _c(self)->getRect(self, &rc);

    tmpdc = hdc;
    if (self->paint_mode == TRANROUND_PAINTMODE_BITBLT){
        if (GetGDCapability(hdc, GDCAP_AMASK) == 0)
            tmpdc = CreateMemDC(RECTW(rc), RECTH(rc), 32, MEMDC_FLAG_HWSURFACE, 
                    0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        else{
            tmpdc = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
        }
    }

    memrc = rc;
    OffsetRect(&memrc, -memrc.left, -memrc.top);

    if(self->border_size){
        prgn = mCreateFillRegionClipRgn(self, &memrc);
        mShapeTransRoundPiece_mFillRegion(self, tmpdc, prgn, &memrc, TRUE);
        DestroyClipRgn(prgn);

        memrc.left += self->border_size;
        memrc.top  += self->border_size;
        memrc.right -= self->border_size;
        memrc.bottom -= self->border_size;
    }

    prgn = mCreateFillRegionClipRgn(self, &memrc);
    mShapeTransRoundPiece_mFillRegion(self, tmpdc, prgn, &memrc, FALSE);
    DestroyClipRgn(prgn);

    if (self->paint_mode == TRANROUND_PAINTMODE_BITBLT){
        BitBlt(tmpdc, 0, 0, 0, 0, hdc, rc.left, rc.top, 0);
        DeleteMemDC(tmpdc);
    }
}

static void mShapeTransRoundPiece_destroy (mShapeTransRoundPiece *self)
{
    MGPlusBrushDelete (self->brush_solid);    
    MGPlusBrushDelete (self->brush_gradient);    
    MGPlusBrushDelete (self->brush_gradient_border);    
    MGPlusGraphicDelete (self->hgs);

    Class(mStaticPiece).destroy((mStaticPiece*)self);

}
static void mShapeTransRoundPiece_paint(mShapeTransRoundPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
#if 0 /* XXX: for debug */
    RECT rc;

    _c(self)->getRect(self, &rc);
    SetBrushColor(hdc, RGB2Pixel(hdc, rand() & 0xff, rand() & 0xff, rand() & 0xff));
    FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
#else
    if (self->fill_engine == TRANROUND_FILLENGINE_NORMAL){
        mShapeTransRoundPiece_Fill(self, hdc, owner, add_data);
    }else{
        mShapeTransRoundPiece_FillWithPlus(self, hdc, owner, add_data);
    }
#endif
}

BEGIN_MINI_CLASS(mShapeTransRoundPiece, mStaticPiece)
        CLASS_METHOD_MAP(mShapeTransRoundPiece, construct)
        CLASS_METHOD_MAP(mShapeTransRoundPiece, destroy)
        CLASS_METHOD_MAP(mShapeTransRoundPiece, paint)
        CLASS_METHOD_MAP(mShapeTransRoundPiece, setProperty)
        CLASS_METHOD_MAP(mShapeTransRoundPiece, getProperty)
        CLASS_METHOD_MAP(mShapeTransRoundPiece, setGradientFillColors);
        CLASS_METHOD_MAP(mShapeTransRoundPiece, getBrush);
        CLASS_METHOD_MAP(mShapeTransRoundPiece, getBorderBrush);
END_MINI_CLASS

// global functions
void ShapeTransRoundPiece_setColorTable(mColorTable *colorTable, ARGB *colors, float *pos, int num)
{
    colorTable->colors = colors;
    colorTable->positions = pos;
    colorTable->tableSize = num;
}

void ShapeTransRoundPiece_setBrushGradientColors (HBRUSH brush, mColorTable *colorTable)
{
    if (brush && colorTable && colorTable->colors && colorTable->tableSize > 0) {
        if (colorTable->positions) {
            MGPlusSetLinearGradientBrushColorsEx (brush,
                    colorTable->colors, colorTable->tableSize, colorTable->positions);
        } else {
            MGPlusSetLinearGradientBrushColors (brush,
                    colorTable->colors, colorTable->tableSize);
        }
    }
}
