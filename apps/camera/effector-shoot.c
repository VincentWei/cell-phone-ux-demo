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
/*!============================================================================
 * @file effector-shoot.c 
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <mgeff/mgeff.h>
#include <mgeff/mgeff-effector.h>
#include "effector-shoot.h"

/*
#define BITBLT(shdc, sx, sy, sw, sh, dhdc, dx, dy, rop) \
    do { \
        if (0 < sw && 0 < sh) BitBlt(shdc, sx, sy, sw, sh, dhdc, dx, dy, rop); \
    } while(0)
    */

#define ME_DIRECTION_LEFT       0
#define ME_DIRECTION_TOP        1
#define ME_DIRECTION_RIGHT      2
#define ME_DIRECTION_BOTTOM     3

#define ME_COVER_NO     0
#define ME_COVER_YES    1

void
eff_get_rect(HDC hdc, RECT* rect)
{
    rect->left = 0;
    rect->top  = 0;
    rect->right  = GetGDCapability(hdc, GDCAP_MAXX) + 1;
    rect->bottom = GetGDCapability(hdc, GDCAP_MAXY) + 1;
}

void
eff_bitblt (HDC src1_dc, HDC src2_dc, HDC sink_dc, PRECT prc, int reach, int direction, int cover)
{
    switch (direction) {
        case ME_DIRECTION_LEFT:
            BITBLT (src2_dc, prc->left + reach, prc->top, RECTWP(prc) - reach, RECTHP(prc),
                    sink_dc, prc->left, prc->top, 0);
            if (!cover)
                BITBLT (src1_dc, RECTWP(prc) - reach, prc->top, reach, RECTHP(prc),
                        sink_dc, RECTWP(prc) - reach, prc->top, 0);
            break;
        case ME_DIRECTION_TOP:
            BITBLT (src2_dc, prc->left, prc->top + reach, RECTWP(prc), RECTHP(prc) - reach,
                    sink_dc, prc->left, prc->top, 0);
            if (!cover)
                BITBLT (src1_dc, prc->left, RECTHP(prc) - reach, RECTWP(prc), reach,
                        sink_dc, prc->left, RECTHP(prc) - reach, 0);
            break;
        case ME_DIRECTION_RIGHT:
            BITBLT (src2_dc, prc->left, prc->top, RECTWP(prc) - reach, RECTHP(prc),
                    sink_dc, prc->left + reach, prc->top, 0);
            if (!cover)
                BITBLT (src1_dc, prc->left, prc->top, reach, RECTHP(prc),
                        sink_dc, prc->left, prc->top, 0);
            break;
        case ME_DIRECTION_BOTTOM:
            BITBLT (src2_dc, prc->left, prc->top, RECTWP(prc), RECTHP(prc) - reach,
                    sink_dc, prc->left, prc->top + reach, 0);
            if (!cover)
                BITBLT (src1_dc, prc->left, prc->top, RECTWP(prc), reach,
                        sink_dc, prc->left, prc->top, 0);
            break;
    }
}

MGEFF_EFFECTOR
effshooteffector_init (MGEFF_EFFECTOR effector)
{
    EffME2Ctxt *context = (EffME2Ctxt *)calloc (1, sizeof (EffME2Ctxt));
    context->prev_reach = 0;
    mGEffEffectorSetContext (effector, context);
    return effector;
}

void
effshooteffector_finalize (MGEFF_EFFECTOR effector)
{
    EffME2Ctxt *context = (EffME2Ctxt *)mGEffEffectorGetContext (effector);
    free (context);
}

void
effshooteffector_ondraw (MGEFF_ANIMATION anim, MGEFF_EFFECTOR effector,
        HDC sink_dc, intptr_t id, void *value)
{
    EffME2Ctxt *context = (EffME2Ctxt *)mGEffEffectorGetContext (effector);
    MGEFF_SOURCE src1 = mGEffEffectorGetSource (effector, 0);
    MGEFF_SOURCE src2 = mGEffEffectorGetSource (effector, 1);
    HDC src1_dc = mGEffGetSourceDC (src1);
    HDC src2_dc = mGEffGetSourceDC (src2);
    RECT rc1, rc2, rc_sink;
    float reach;
    int half_w;
    int half_h;
    int reach_w;
    int reach_h;
    int cover;
    RECT rc = {0, 0, 0, 0};

    eff_get_rect (src1_dc, &rc1);
    eff_get_rect (src2_dc, &rc2);
    eff_get_rect (sink_dc, &rc_sink);

    reach = (*(float *)value);
    half_w = RECTW(rc_sink) / 2;
    half_h = RECTH(rc_sink) / 2;
    reach_w = half_w * reach;
    reach_h = half_h * reach;

    if (context->prev_reach < reach_w)
        cover = ME_COVER_NO;
    else
        cover = ME_COVER_YES;

    rc.right = half_w;
    rc.bottom = half_h;
    eff_bitblt (src1_dc, src2_dc, sink_dc, &rc, reach_w, ME_DIRECTION_LEFT, cover);

    rc.left = half_w;
    rc.right = RECTW(rc_sink);
    eff_bitblt (src1_dc, src2_dc, sink_dc, &rc, reach_h, ME_DIRECTION_TOP, cover);

    rc.top = RECTH(rc_sink) - half_h;
    rc.bottom = RECTH(rc_sink);
    eff_bitblt (src1_dc, src2_dc, sink_dc, &rc, reach_w, ME_DIRECTION_RIGHT, cover);

    rc.left = 0;
    rc.right = RECTW(rc_sink) - half_w;
    eff_bitblt (src1_dc, src2_dc, sink_dc, &rc, reach_h, ME_DIRECTION_BOTTOM, cover);

    context->prev_reach = reach_w;
}

void
effshooteffector_begindraw (MGEFF_ANIMATION anim, MGEFF_EFFECTOR effector)
{
    float s = 1.0;
    float e = 0.0;
    int zoom;
    EffME2Ctxt *context = (EffME2Ctxt *)mGEffEffectorGetContext (effector);

    mGEffEffectorGetProperty (effector, MGEFF_PROPERTY_ZOOM, &zoom);

    if (zoom == MGEFF_ZOOMIN) {
        s = 1.0;
        e = 0.0;
    } else {
        s = 0.0;
        e = 1.0;
    }
    mGEffAnimationSetStartValue (anim, &s);
    mGEffAnimationSetEndValue (anim, &e);

    context = (EffME2Ctxt *)mGEffEffectorGetContext (effector);
    context->prev_reach = e;
}

MGEFF_EFFECTOROPS shoot = {
    MGEFF_MINOR_shoot,
    MGEFF_FLOAT,
    effshooteffector_init,
    effshooteffector_finalize,
    effshooteffector_ondraw,
    effshooteffector_begindraw,
    NULL,
    NULL,
    NULL,
};

