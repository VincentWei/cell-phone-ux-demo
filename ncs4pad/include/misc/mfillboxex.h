#ifndef _MFILLBOXEX_H
#define _MFILLBOXEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <minigui/common.h>
#include <minigui/gdi.h>

typedef void (*cb_fill_span_t)(HDC hdc, void *context, const RECT *rc);
void mFillRegion(HDC hdc, PCLIPRGN prgn, cb_fill_span_t cb, void *context);

void fillspan_simple(HDC hdc, void *ctx_rgba, const RECT *rc);

struct gradient_context{
    int top;
    int bottom;
    unsigned int rgba[2];
};
void fillspan_gradient(HDC hdc, void *ctx_gradient, const RECT *rc);

typedef unsigned int gradient_t[2];
struct multigradient_context{
    int top;
    int bottom;
    int n;
    gradient_t *colortable;
    int _private_current_index;
};
void fillspan_multigradient(HDC hdc, void *ctx_multigradient, const RECT *rc);

#ifdef __cplusplus
}
#endif

#endif /* _MFILLBOXEX_H */
