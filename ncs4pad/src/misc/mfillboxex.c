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
#include <stdint.h>
#include <assert.h>
#include "mfillboxex.h"

#define TOHIGH(a) ((a) << 8)
#define TOLOW(a) ((a) >> 8)
#define GET_INTERPOLATION(a1, a2, r) TOLOW(TOHIGH(a1) + (r) * ((a2) - (a1)))
#define GETPOS(x1, x2, x) TOHIGH((x) - (x1)) / ((x2) - (x1));

void fillspan_gradient(HDC hdc, void *context, const RECT *rc) {
    int y;
    struct gradient_context *gra = (struct gradient_context *)context;
    unsigned int pos;
    unsigned char r, g, b, a;

    for (y=rc->top; y<rc->bottom; ++y) {
        pos = GETPOS(gra->top, gra->bottom, y);
        if (pos > TOHIGH(1)) {
            pos = TOHIGH(1);
        }
        r = GET_INTERPOLATION(GetRValue(gra->rgba[0]), GetRValue(gra->rgba[1]), pos);
        g = GET_INTERPOLATION(GetGValue(gra->rgba[0]), GetGValue(gra->rgba[1]), pos);
        b = GET_INTERPOLATION(GetBValue(gra->rgba[0]), GetBValue(gra->rgba[1]), pos);
        a = GET_INTERPOLATION(GetAValue(gra->rgba[0]), GetAValue(gra->rgba[1]), pos);
        SetBrushColor(hdc, RGBA2Pixel(hdc, r, g, b, a));
        FillBox(hdc, rc->left, y, RECTWP(rc), 1);
    }
}

void fillspan_multigradient(HDC hdc, void *context, const RECT *rc) {
    int y;
    struct multigradient_context *gra = (struct multigradient_context *)context;
    unsigned int pos, pos_inner;
    unsigned char r, g, b, a;
    int i;

    assert(gra->colortable[0][0] == 0 && gra->colortable[gra->n-1][0] == 100);
    if (gra->_private_current_index < 0 || gra->_private_current_index >= gra->n) {
        fprintf(stderr, "FATAL: Please memset(&context, 0, sizeof(context))\n");
    }
    assert(gra->_private_current_index >= 0 && gra->_private_current_index < gra->n);

    for (y=rc->top; y<rc->bottom; ++y) {
        pos = GETPOS(gra->top, gra->bottom, y);
        if (pos > TOHIGH(1)) {
            pos = TOHIGH(1);
        }
        pos = TOLOW(pos * 100);
        assert(pos >= gra->colortable[gra->_private_current_index][0]);
        for (i=gra->_private_current_index; i<gra->n-1 && pos >= gra->colortable[i+1][0]; ++i)
            ;
        if (i >= gra->n-1) {
            r = GetRValue(gra->colortable[i][1]);
            g = GetGValue(gra->colortable[i][1]);
            b = GetBValue(gra->colortable[i][1]);
            a = GetAValue(gra->colortable[i][1]);
        }else{
            pos_inner = GETPOS(gra->colortable[i][0], gra->colortable[i+1][0], pos);
            r = GET_INTERPOLATION(GetRValue(gra->colortable[i][1]), GetRValue(gra->colortable[i+1][1]), pos_inner);
            g = GET_INTERPOLATION(GetGValue(gra->colortable[i][1]), GetGValue(gra->colortable[i+1][1]), pos_inner);
            b = GET_INTERPOLATION(GetBValue(gra->colortable[i][1]), GetBValue(gra->colortable[i+1][1]), pos_inner);
            a = GET_INTERPOLATION(GetAValue(gra->colortable[i][1]), GetAValue(gra->colortable[i+1][1]), pos_inner);
        }
        gra->_private_current_index = i;
        SetBrushColor(hdc, RGBA2Pixel(hdc, r, g, b, a));
        FillBox(hdc, rc->left, y, RECTWP(rc), 1);
    }
}

void fillspan_simple(HDC hdc, void *context, const RECT *rc) {
    unsigned int rgba = (unsigned int)(intptr_t)context;

    SetBrushColor(hdc, RGBA2Pixel(hdc, GetRValue(rgba), GetGValue(rgba), GetBValue(rgba), GetAValue(rgba)));
    FillBox(hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc));
}

void mFillRegion(HDC hdc, PCLIPRGN prgn, cb_fill_span_t cb, void *context) {
    PCLIPRECT cell;

    /*
    printf("Region:\n");
    printf("  type: %d\n", prgn->type);
    printf("  bound: (%d,%d,%d,%d)\n", prgn->rcBound.left, prgn->rcBound.top, prgn->rcBound.right, prgn->rcBound.bottom);
    */

    for (cell=prgn->head; cell; cell = cell->next) {
        /*
        printf("  cell: (%d,%d,%d,%d)\n", cell->rc.left, cell->rc.top, cell->rc.right, cell->rc.bottom);
        */
        cb(hdc, context, &cell->rc);
    }
}
