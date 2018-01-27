#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgplus/mgplus.h>
#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mabstractitemmanager.h"
#include "mlistitemmanager.h"
#include "mitemiterator.h"
#include "mlistitemiterator.h"
#include "mpanelpiece.h"

#define HAVE_H_FILLLADDER
#define ROTATE_90 90
#define HOVER

typedef struct _TransAffine_3D
{
    double m[12]; //World TransForm;
}TransAffine3D;

typedef struct _Vector3D
{
    int x;
    int y;
    int z;
}Vector3D;

typedef struct _Vector2D
{
    int x;
    int y;
}Vector2D;

#ifdef HAVE_H_FILLLADDER
extern void effcommon_v_fillladder(HDC src_dc, int src_left, int src_top, int src_right, int src_bottom,
        HDC dst_dc, int dst_left, int dst_top, int dst_right, int dst_bottom,
        int dst_x0, int dst_y0,
        int dst_x1, int dst_y1,
        int dst_x2, int dst_y2,
        int dst_x3, int dst_y3);
extern void effcommon_h_fillladder(HDC src_dc, int src_left, int src_top, int src_right, int src_bottom,
        HDC dst_dc, int dst_left, int dst_top, int dst_right, int dst_bottom,
        int dst_x0, int dst_y0,
        int dst_x1, int dst_y1,
        int dst_x2, int dst_y2,
        int dst_x3, int dst_y3);
#else
extern void effcommon_fillladder(HDC src_dc, int src_left, int src_top, int src_right, int src_bottom,
        HDC dst_dc, int dst_left, int dst_top, int dst_right, int dst_bottom,
        int dst_x0, int dst_y0,
        int dst_x1, int dst_y1,
        int dst_x2, int dst_y2,
        int dst_x3, int dst_y3);
#endif
extern TransAffine3D* Init3DSenceTransAffine (void);
extern int MatrixMultiply (double* A, double* B, double* C);
extern int Set3DPointOfView (TransAffine3D* pTrans, double dXMove, double dYMove, double dZMove);
extern int Scale3DSence (TransAffine3D* pTrans, double x, double y, double z);
extern int Move3DSence (TransAffine3D* pTrans, double x, double y, double z);
extern int Roll3DSence (TransAffine3D* pTrans, double dZAngle);
extern int RotateHoriz3DSence (TransAffine3D* pTrans, double dYAngle);
extern int RotateVertical3DSence (TransAffine3D* pTrans, double dXAngle);
extern int Generate3DPointBy3DSence (TransAffine3D* pTrans, Vector3D* pVector);
extern void Terminate3DSenceTransAffine (TransAffine3D* pTransAffine);
extern int Perspective3DTo2D (Vector3D* pVector3D, Vector2D* pVector2D, int nPerspectiveZ, int nXCenter, int nYCenter);
extern int Reset3DSenceTransAffine (TransAffine3D* pTransAffine);


#ifdef ENABLE_ANIM_FPS_TEST
    extern void anim_fps_test_start (int new_status);
#endif


int s_totalObjRef = 0;
int s_totalPieceRef = 0;

static void getRectFromDC(HDC hdc, RECT *prc)
{
    prc->left = 0;
    prc->top  = 0;
    prc->right  = GetGDCapability(hdc, GDCAP_MAXX) + 1;
    prc->bottom = GetGDCapability(hdc, GDCAP_MAXY) + 1;
}

static void rotate(HDC dst_dc, HDC src_dc, mNormalVector *mv)
{
    int w, h, nZprep;
    int i;
    TransAffine3D *pTransAffine = NULL;
    Vector3D Points3D[2];
    Vector2D Points2D[2];
    POINTF   Points[4];
    double dAngle;
    RECT rc;

    if (dst_dc != HDC_INVALID && src_dc != HDC_INVALID) {
        assert(mv->angle >= 0.0 && mv->angle <= 180.0);
        dAngle = ROTATE_90 - mv->angle;
        if (dAngle > 0)
            dAngle -= ROTATE_90;
        else
            dAngle += ROTATE_90;
//        printf ("dAngnle = %lf\n", dAngle);

        getRectFromDC(src_dc, &rc);
        w = RECTW(rc);
        h = RECTH(rc);
        nZprep = 3 * w;
        pTransAffine = Init3DSenceTransAffine();

        if (!pTransAffine) {
            printf("[3D Sence]Init: can not malloc enough memory!");
            return;
        }

        if (mv->x) {
#ifdef HAVE_H_FILLLADDER
            Points3D[0].x = w;
            Points3D[0].y = - h / 2;
            Points3D[0].z = 0;

            Points3D[1].x = w;
            Points3D[1].y = (h + 1) / 2;
            Points3D[1].z = 0;

            Set3DPointOfView(pTransAffine, 0, 0, 0);
            RotateVertical3DSence(pTransAffine, dAngle);
            for (i = 0; i < 2; ++i) {
                Generate3DPointBy3DSence(pTransAffine, &Points3D[i]);
                Perspective3DTo2D(&Points3D[i], &Points2D[i], nZprep, 0, 0);
            }
            Terminate3DSenceTransAffine (pTransAffine);

            Points[0].xf = Points2D[0].x;
            Points[0].yf = Points2D[0].y + h / 2;

            Points[1].xf = Points2D[1].x;
            Points[1].yf = Points2D[1].y + h / 2;

            Points[3].xf = w - Points2D[0].x;
            Points[3].yf = Points2D[0].y + h / 2;

            Points[2].xf = w - Points2D[1].x;
            Points[2].yf = Points2D[1].y + h / 2;

            if (Points[0].xf != w && Points[1].xf != w) {
                double scale;
                if (Points[0].xf > w) {
                    scale = (float)w / Points[0].xf;
                    Points[0].xf = w;
                    Points[3].xf = 0;
                    Points[2].xf = w / 2 - scale * (w / 2 - Points[2].xf);
                    Points[1].xf = w - Points[2].xf;

                } else {
                    scale = (float)w / Points[1].xf;
                    Points[3].xf = w / 2 - scale * (w / 2 - Points[3].xf);
                    Points[0].xf = w - Points[3].xf;
                    Points[2].xf = 0;
                    Points[1].xf = w;
                }

                Points[2].yf = h / 2 - scale * (h / 2 - Points[2].yf);
                Points[3].yf = h - Points[2].yf;

                Points[0].yf = Points[3].yf;
                Points[1].yf = Points[2].yf;
            }

            if (Points[2].yf > Points[3].yf) {
                effcommon_h_fillladder(src_dc, rc.left, rc.top, rc.right, rc.bottom,
                        dst_dc, rc.left, rc.top, rc.right, rc.bottom,
                        Points[0].xf, Points[0].yf,
                        Points[1].xf, Points[1].yf,
                        Points[2].xf, Points[2].yf,
                        Points[3].xf, Points[3].yf);
            } else {
            //    printf("out of line\n");
            }
#endif
        } else if (mv->y) {
            Points3D[0].x = - w / 2;
            Points3D[0].y = h;
            Points3D[0].z = 0;

            Points3D[1].x = (w + 1) / 2;
            Points3D[1].y = h;
            Points3D[1].z = 0;

            Set3DPointOfView(pTransAffine, 0, 0, 0);
            RotateHoriz3DSence(pTransAffine, dAngle);
            for (i = 0; i < 2; ++i) {
                Generate3DPointBy3DSence(pTransAffine, &Points3D[i]);
                Perspective3DTo2D(&Points3D[i], &Points2D[i], nZprep, 0, 0);
            }
            Terminate3DSenceTransAffine (pTransAffine);

            Points[3].xf = Points2D[0].x + w / 2;
            Points[3].yf = Points2D[0].y;

            Points[0].xf = Points[3].xf;
            Points[0].yf = h - Points2D[0].y;

            Points[2].xf = Points2D[1].x + w / 2;
            Points[2].yf = Points2D[1].y;

            Points[1].xf = Points[2].xf;
            Points[1].yf = h - Points2D[1].y;

            if (Points[3].yf != h && Points[2].yf != h) {
                double scale;
                if (Points[3].yf > h) {
                    scale = (float)h / Points[3].yf;
                    Points[3].yf = h;
                    Points[0].yf = 0;
                    Points[2].yf = h / 2 - scale * (h / 2 - Points[2].yf);
                    Points[1].yf = h - Points[2].yf;

                } else {
                    scale = (float)h / Points[2].yf;
                    Points[3].yf = h / 2 - scale * (h / 2 - Points[3].yf);
                    Points[0].yf = h - Points[3].yf;
                    Points[2].yf = h;
                    Points[1].yf = 0;
                }

                Points[3].xf = w / 2 - scale * (w / 2 - Points[3].xf);
                Points[2].xf = w - Points[3].xf;

                Points[0].xf = Points[3].xf;
                Points[1].xf = Points[2].xf;
            }

#if 0
            printf ("%f %f\n", Points[1].xf, Points[1].yf);
            printf ("%f %f\n", Points[2].xf, Points[2].yf);
            printf ("%f %f\n", Points[3].xf, Points[3].yf);
            printf ("%f %f\n", Points[0].xf, Points[0].yf);
            printf ("\n");
#endif

            if (Points[1].xf > Points[0].xf) {
                effcommon_v_fillladder(src_dc, rc.left, rc.top, rc.right, rc.bottom,
                        dst_dc, rc.left, rc.top, rc.right, rc.bottom,
                        Points[1].xf, Points[1].yf,
                        Points[2].xf, Points[2].yf,
                        Points[3].xf, Points[3].yf,
                        Points[0].xf, Points[0].yf);
            } else {
                //            printf("out of line\n");
            }
#if 0
        else if (mv->z)
            Roll3DSence(pTransAffine, dAngle);
#endif
        } else {
            assert(0);
        }
    }
}

static void set_transroundpiece_paintmode(mPieceItem *item, TRANROUND_PAINTMODE mode)
{
    mHotPiece *piece = item->piece;

    if (mode == _c(piece)->getProperty(piece, NCSP_TRANROUND_PAINTMODE))
        return;


    if(INSTANCEOF(piece, mPanelPiece)) {

        //printf("/t%s mPanelPiece %p\n", __FUNCTION__, piece);
        mPanelPiece *panel = (mPanelPiece*)piece;
        mPieceItem *itemChild = NULL;
        mItemIterator *iter = _c(panel->itemManager)->createItemIterator(panel->itemManager);

        if (_c(panel)->getBkgndPiece(panel)) {
            mHotPiece* bk = (mHotPiece*)_c(panel)->getBkgndPiece(panel);
            _c(bk)->setProperty(bk, NCSP_TRANROUND_PAINTMODE, mode);
        }
        while ((itemChild = _c(iter)->next(iter)))
            set_transroundpiece_paintmode(itemChild, mode);
        DELETE(iter);
    } else if (INSTANCEOF(piece, mShapeTransRoundPiece)) {

        //printf("/t%s mShapeTransRoundPiece %p\n", __FUNCTION__, piece);
        _c(piece)->setProperty(piece, NCSP_TRANROUND_PAINTMODE, mode);
    }

    return;
}

static void paintmode_should_be_reset(mHotPiece* piece)
{
    mPanelPiece *topPanel = PanelPiece_getTopPanel(piece);
    assert(topPanel);
    topPanel->shouldResetPaintMode = TRUE;
}

static void test_rotate_and_paint(mPieceItem *item, HDC hdc, mObject *owner, DWORD add_data)
{
    if (item->normalVector.angle != 0.0 && item->normalVector.angle != 180.0) {
        /* need rotate */
        //mPanelPiece* panel = (mPanelPiece*)item->piece;
        RECT rc;
        HDC rotate_dc;
        HDC alpha_dc;

        if (item->isEnableCache) {
            getRectFromDC(hdc, &rc);

            if (item->cacheDC == HDC_INVALID) {
                //item->cacheDC = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
                if ( 0 == GetGDCapability(hdc, GDCAP_AMASK) ) {
                    item->cacheDC = CreateMemDC(RECTW(rc), RECTH(rc), 32, MEMDC_FLAG_HWSURFACE, 
                            0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                }
                else {
                    item->cacheDC = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
                }

                set_transroundpiece_paintmode(item, TRANROUND_PAINTMODE_GRAPHICSAVE);
                paintmode_should_be_reset(item->piece);
                _c(item->piece)->paint(item->piece, item->cacheDC, owner, add_data); 
            }

            if ( 0 == GetGDCapability(hdc, GDCAP_AMASK) ) {
                alpha_dc = CreateCompatibleDC(item->cacheDC);
                BitBlt(hdc, 0, 0, 0, 0, alpha_dc, 0, 0, 0);
                rotate(alpha_dc, item->cacheDC, &item->normalVector);
                BitBlt(alpha_dc, 0, 0, 0, 0, hdc, 0, 0, 0);
                DeleteMemDC(alpha_dc);
            }
            else{
                rotate(hdc, item->cacheDC, &item->normalVector);
            }

            //rotate(hdc, item->cacheDC, &item->normalVector);
        }
        else {
            getRectFromDC(hdc, &rc);
            rotate_dc = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
            
            set_transroundpiece_paintmode(item, TRANROUND_PAINTMODE_GRAPHICSAVE);
            
            paintmode_should_be_reset(item->piece);
            
            _c(item->piece)->paint(item->piece, rotate_dc, owner, add_data);
            
            rotate(hdc, rotate_dc, &item->normalVector);
            
            DeleteMemDC(rotate_dc);
        }
    } else {
        /* no need rotate */
        _c(item->piece)->paint(item->piece, hdc, owner, add_data);
    }
}

static mAbstractItemManager *mPanelPiece_createItemManager(mPanelPiece *self)
{
    return (mAbstractItemManager*)NEW(mListItemManager);
}

static void mPanelPiece_initItemNode(mPanelPiece *self, mPieceItem *item, mHotPiece *piece)
{
    item->piece = piece;
    piece->parent = (mHotPiece*)self;
}

static mPieceItem *mPanelPiece_createItemNode(mPanelPiece *self, mHotPiece *piece, int x, int y)
{
    mPieceItem *item = NULL;

    if ((item = NEW(mPieceItem)) == NULL)
        assert(0);

    if (INSTANCEOF(piece, mPanelPiece)) {
        ((mPanelPiece*)piece)->isTopPanel = FALSE;
//        printf("addContent %p at %d,%d (mPanelPiece)\n", piece, x, y);
    } else {
//        printf("addContent %p at %d,%d\n", piece, x, y);
    }

    item->x = x;
    item->y = y;
    item->wscalefactor = 1.0;
    item->hscalefactor = 1.0;
    item->alpha = 255;
    item->normalVector.angle = 0.0;
    item->normalVector.x = 0;
    item->normalVector.y = 0;
    item->normalVector.z = 0;
    item->underLayout = FALSE;

    _c(self)->initItemNode(self, item, piece);

    _c(self->itemManager)->addItem(self->itemManager, item);

    return item;
}

static mPieceItem* mPanelPiece_addContent(mPanelPiece* self, mHotPiece *piece, int x, int y)
{
    ADDREFPIECE(piece);
    return _c(self)->createItemNode(self, piece, x, y);
}

static mPieceItem* mPanelPiece_addContentToLayout(mPanelPiece *self, mHotPiece *piece)
{
    mPieceItem *item = _c(self)->addContent(self, piece, 0, 0);

    item->underLayout = TRUE;

    if (self->layout) {
        _c(self)->reLayout(self);
    }
    return item;
}

static void mPanelPiece_addContentFinished(mPanelPiece *self)
{}

static BOOL mPanelPiece_delContent(mPanelPiece* self, mHotPiece* piece)
{
    mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
    mPieceItem *item;
    while ((item = _c(iter)->next(iter))) {
        if (item->piece == piece) {
            if (self->hovering_piece == piece) {
                mWidget_releaseHoveringFocus();
                self->hovering_piece = NULL;
            }
            piece->parent = (mHotPiece*)-1;
            UNREFPIECE(item->piece);
            _c(self->itemManager)->removeItem(self->itemManager, item);
            DELETE(item);
            DELETE(iter);
            return TRUE;
        }
    }
    DELETE(iter);

    /* houhh add delContent reLayout. */
    if (self->layout) {
        _c(self)->reLayout(self);
    }
    return FALSE;
}

static void mPanelPiece_clearContents(mPanelPiece* self)
{
    mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
    mPieceItem *item;

    mWidget_releaseHoveringFocus();
    self->hovering_piece = NULL;
#if 0
    while ((item = _c(iter)->next(iter))) {
        DELPIECE(item->piece);
        _c(self->itemManager)->removeItem(self->itemManager, item);
        DELETE(item);
    }
#else
    item = _c(iter)->next(iter);
    while (item) {
        mPieceItem* prev = item;
        UNREFPIECE(item->piece);
        _c(self->itemManager)->removeItem(self->itemManager, item);
        item = _c(iter)->next(iter);
        DELETE(prev);
    }
#endif
    DELETE(iter);
}

static void mPanelPiece_reLayout(mPanelPiece *self)
{
    if (self->layout) {
        RECT rc;
        mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
        _c(self)->getRect(self, &rc);
        _c(self->layout)->reLayout(self->layout, iter, RECTW(rc), RECTH(rc));
        DELETE(iter);
    }
}

static void mPanelPiece_setLayoutManager(mPanelPiece *self, mLayoutManager *layout)
{
    if (self->layout) {
        DELETE(self->layout);
        self->layout = NULL;
    }

    if (layout) {
        self->layout = layout;
        _c(self)->reLayout(self);
    }
}

static BOOL mPanelPiece_setRect(mPanelPiece *self, const RECT *prc)
{
    BOOL ret = Class(mStaticPiece).setRect((mStaticPiece*)self, prc);

    if (ret)
        _c(self)->reLayout(self);
     
    return ret;
}

static void mPanelPiece_enableChildCache(mPanelPiece *self, mHotPiece* child, BOOL enable)
{
    mPieceItem *item = NULL;
    item = _c(self)->searchItem(self, child);

    if (item == NULL || item->isEnableCache == enable) 
        return;

    fprintf(stderr, "%s:%d enable panel child:%p, cache: %d.\n", 
            __FUNCTION__, __LINE__, child, enable);
    item->isEnableCache = enable;
}

static BOOL mPanelPiece_updateChildCache(mPanelPiece *self, mHotPiece* child)
{
    RECT clipRect;
    mPieceItem *item = NULL;
    mPanelPiece* itempiece = NULL;
    item = _c(self)->searchItem(self, child);

    if (item == NULL || item->isEnableCache == FALSE || item->cacheDC == HDC_INVALID) 
        return FALSE;
    else
        itempiece = (mPanelPiece*)item->piece;

    fprintf(stderr, "%s:%d update panel child:%p, cache: %p.\n", 
            __FUNCTION__, __LINE__, child, item->cacheDC);

    set_transroundpiece_paintmode(item, TRANROUND_PAINTMODE_GRAPHICSAVE);
    paintmode_should_be_reset(item->piece);

    /* set the panelpiece cliprect to whole piece rect for it can be update to cacheDC */
    clipRect = itempiece->clipRect;
    _c(itempiece)->getRect(itempiece, &itempiece->clipRect);
    _c(item->piece)->paint(item->piece, item->cacheDC, (mObject*)self->owner, 0); 
    itempiece->clipRect = clipRect;

    return TRUE;
}

static BOOL mPanelPiece_setProperty(mPanelPiece* self, intptr_t id, DWORD value)
{
    switch (id) {
        case NCSP_PANEL_CLIPRECT:
            self->clipRect = *((RECT*)value);
            break;
        default:
            return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
    }
    return TRUE;
}

static void mPanelPiece_setClipRect(mPanelPiece* self, const mPieceItem* item)
{
    RECT rc = self->clipRect;
    mHotPiece* piece = item->piece;
    OffsetRect(&rc, -item->x, -item->y);
    _c(piece)->setProperty(piece, NCSP_PANEL_CLIPRECT, (DWORD)&rc);
}

static void mPanelPiece_paint(mPanelPiece* self, HDC hdc, mObject* owner, DWORD add_data)
{
    RECT containerRc, clipRect;
    HDC tmpdc = HDC_INVALID;
    mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
    mPieceItem *item = NULL;

    _c(self)->getRect(self, &containerRc);
    clipRect = containerRc;
    if (RECTW(self->invalidRect) 
            && RECTH(self->invalidRect)) {
        IntersectRect(&clipRect, &clipRect, &self->invalidRect);
        /*
        fprintf(stderr, "piece is %p, clipRect is (%d,%d,%d,%d).\n",
                self, clipRect.left, clipRect.top, clipRect.right, clipRect.bottom);
                */
    }
    /* set clip rect of hdc */
#if 1
    ClipRectIntersect(hdc, &clipRect);
    //SelectClipRect(hdc, &containerRc);
#endif

    /* draw background.*/
    if (self->bkgndPiece){
        _c(self->bkgndPiece)->paint(self->bkgndPiece, hdc, owner, add_data);
    }
    if (self->isTopPanel) {
        //self->clipRect = containerRc;
        self->clipRect = clipRect;
    }

    while ((item = _c(iter)->next(iter))) {
        RECT rc;
        RECT rc2;
        int centerx, centery, left, top, w, h;
        _c(item->piece)->getRect(item->piece, &rc);

        assert(rc.left == 0 && rc.top == 0);

        if (item->wscalefactor <= 0.0 || item->hscalefactor <= 0.0)
            continue;

        w = RECTW(rc) * item->wscalefactor;
        h = RECTH(rc) * item->hscalefactor;
        centerx = item->x + RECTW(rc) / 2;
        centery = item->y + RECTH(rc) / 2;
        left = centerx - w / 2;
        top  = centery - h / 2;

        if (left >= containerRc.right || top >= containerRc.bottom)
            continue;

        rc2.left = item->x;
        rc2.top  = item->y;
        rc2.right  = rc2.left + RECTW(rc);
        rc2.bottom = rc2.top  + RECTH(rc);
        _c(self)->setClipRect(self, item);
        if (!DoesIntersect(&self->clipRect, &rc2)){
            continue;
        }

        if (self->isTopPanel || item->alpha != 255) {
            /* create memdc */
            tmpdc = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
        } else {
            /* get sub memdc */
            tmpdc = GetSubDC(hdc, left, top, RECTW(rc), RECTH(rc));
            if (tmpdc == HDC_INVALID) {
                int w = GetGDCapability(hdc, GDCAP_MAXX) + 1;
                int h = GetGDCapability(hdc, GDCAP_MAXY) + 1;
                if (RECTW(rc) > 0 && RECTH(rc) > 0 
                        && !(top >= h || left >= w)) {
                    fprintf(stderr, "Error: subDC INVALID :%d %d %d %d\n",
                            left, top, RECTW(rc), RECTH(rc));
                }
                continue;
            }
        }

        if (item->wscalefactor != 1.0 || item->hscalefactor != 1.0) {
            /* need scale */
            int xt = (left >= 0) ? 0 : -left;
            int yt = (top >= 0) ? 0 : -top;
            if ((w - xt > 0) && (h - yt > 0)) {
                StretchBlt(hdc, left + xt, top + yt, w - xt, h - yt,
                        tmpdc, xt, yt, 0, 0, 0);
                test_rotate_and_paint(item, tmpdc, owner,
                        (DWORD)1);
                StretchBlt(tmpdc, xt, yt, 0, 0,
                        hdc, left + xt, top + yt, w - xt, h - yt, 0);
            }
        } else {
            /* no need scale */
            if (self->isTopPanel || item->alpha != 255) {
                // should *RESET* all paintmode of shapeTransRoundPiece to 
                // TRANROUND_PAINTMODE_BITBLT
                // if self is the top panel piece
                if (self->shouldResetPaintMode) {
                    set_transroundpiece_paintmode(item, TRANROUND_PAINTMODE_BITBLT);
                    self->shouldResetPaintMode = FALSE;
                }
                /* set alpha */
                BitBlt(hdc, left, top, w, h, tmpdc, 0, 0, 0);
                if (item->alpha != 255) {
                    set_transroundpiece_paintmode(item, TRANROUND_PAINTMODE_GRAPHICSAVE);
                    SetMemDCAlpha(tmpdc, MEMDC_FLAG_SRCALPHA, item->alpha);
                }
                test_rotate_and_paint(item, tmpdc, owner, 0);
                BitBlt(tmpdc, 0, 0, 0, 0, hdc, left, top, 0);
            } else {
                test_rotate_and_paint(item, tmpdc, owner, add_data);
            }
        }

        //if (self->isTopPanel) {
        if (self->isTopPanel || item->alpha != 255) {
            DeleteMemDC(tmpdc);
        } else {
            ReleaseDC(tmpdc);
        }
    }

    DELETE(iter);
}

static void mPanelPiece_construct(mPanelPiece* self, DWORD add_data)
{
    Class(mStaticPiece).construct((mStaticPiece*)self, add_data);

    self->isTopPanel = TRUE;
    self->shouldResetPaintMode = FALSE;
    self->hovering_piece = NULL;
    self->layout = NULL;

    self->itemManager = _c(self)->createItemManager(self);

    INIT_LIST_HEAD(&self->eventHandlers);

    self->owner = NULL;
    self->update_flag = FALSE;
    self->parent = NULL;

    self->addData = add_data;
    memset(&self->invalidRect, 0, sizeof(RECT));

    /* add */
}

static void mPanelPiece_destroy(mPanelPiece* self)
{
    /* TODO: event handlers */

    // stop update animation if exists
    if (self->update_anim) {
        mGEffAnimationStop(self->update_anim);
        self->update_anim = NULL;
    }

    if (self->layout) DELETE(self->layout);
    
    _c(self)->clearContents(self);
    _c(self)->clearEventHandler(self);
    DELETE(self->itemManager);

    if (self->bkgndPiece)
        UNREFPIECE(self->bkgndPiece);

    Class(mStaticPiece).destroy((mStaticPiece*)self);
}

static mHotPiece *mPanelPiece_hitTest(mPanelPiece* self, int x, int y)
{
    return (mHotPiece *)self;
}

static mPieceItem *mPanelPiece_searchItem(mPanelPiece *self, mHotPiece *child)
{
    mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
    mPieceItem *item = NULL;
    while ((item = _c(iter)->next(iter))) {
        if (item->piece && item->piece == child)
            break;
    }
    DELETE(iter);
    return item;
}

enum {
    ANIM_MOVE = 1,
    ANIM_SCALE,
    ANIM_ROTATE,
    ANIM_ALPHA,
};

static void _piece_anim_cb(MGEFF_ANIMATION handle, void *target, intptr_t id, void *value)
{
    mHotPiece* child = (mHotPiece*)target;
    mPanelPiece* self = (mPanelPiece*) child->parent;
    assert(self);
    assert(child);

    switch (id) {
        case ANIM_MOVE:
            {
                POINT* pt = (POINT*)value;

                _c(self)->movePiece(self, child, pt->x, pt->y);
            }
            break;
        case ANIM_SCALE:
            {
                POINTF *ptf = (POINTF*)value;
                float wscalefactor = (ptf->xf < 0.0) ? 0.0 : ptf->xf;
                float hscalefactor = (ptf->yf < 0.0) ? 0.0 : ptf->yf;
                _c(self)->scalePiece(self, child, wscalefactor, hscalefactor);
            }
            break;
        case ANIM_ROTATE:
            {
                float angle = (*(float*)value >= ROTATE_90 * 2) ? 0.0 : *(float*)value;
                mPieceItem *item =  (item = _c(self)->searchItem(self, child));
                assert(item);
                _c(self)->rotatePiece(self, child, angle, 
                        item->normalVector.x, item->normalVector.y, item->normalVector.z);
            }
            break;
        case ANIM_ALPHA:
            {
                int alpha = *(int*)value;
                _c(self)->setPieceAlpha(self, child, alpha);
            }
            break;
        default:
            assert(0);
            break;
    }

}

/****************************MOVE PIECE START**********************************/
static void mPanelPiece_movePiece(mPanelPiece *self, mHotPiece *child, int x, int y)
{
    mPieceItem *item = NULL;

    if ((item = _c(self)->searchItem(self, child))) {
        /* update old position */
        _c(self)->invalidatePiece(self, child, NULL, FALSE);

        item->x = x;
        item->y = y;

        /* update new position */
        _c(self)->invalidatePiece(self, child, NULL, FALSE);
    }else{
        assert(0);
    }
}

static MGEFF_ANIMATION mPanelPiece_movePieceWithAnimation(mPanelPiece *self, mHotPiece *child,
        int x, int y, int duration, enum EffMotionType curve)
{
    MGEFF_ANIMATION anim = NULL;
    mPieceItem *item = NULL;
    if ((item = _c(self)->searchItem(self, child))) {
        POINT pt_s = { item->x, item->y };
        POINT pt_e = { x, y };
        anim = mGEffAnimationCreate(child, _piece_anim_cb, ANIM_MOVE, MGEFF_POINT);
        assert(anim);

        //printf("panelpiece ==> s:(%d,%d), e:(%d,%d)\n", pt_s.x, pt_s.y, pt_e.x, pt_e.y);
        mGEffAnimationSetStartValue(anim, &pt_s);
        mGEffAnimationSetEndValue(anim, &pt_e);
        mGEffAnimationSetDuration(anim, duration);
        mGEffAnimationSetCurve(anim, curve);
        return anim;
    }
    return NULL;
}
/****************************MOVE PIECE END************************************/

/****************************SCALE PIECE START*********************************/
static void mPanelPiece_scalePiece(mPanelPiece *self, mHotPiece *child, float wscalefactor, float hscalefactor)
{
    mPieceItem *item = NULL;

    // update old position
    PanelPiece_invalidatePiece((mHotPiece*)child, NULL);

    if ((item = _c(self)->searchItem(self, child))) {
        item->wscalefactor = (wscalefactor < 0.0) ? 0.0 : wscalefactor;
        item->hscalefactor = (hscalefactor < 0.0) ? 0.0 : hscalefactor;
    }

    // update new position
    PanelPiece_invalidatePiece((mHotPiece*)child, NULL);
}

static MGEFF_ANIMATION mPanelPiece_scalePieceWithAnimation(mPanelPiece *self, mHotPiece *child,
        float wscalefactor, float hscalefactor, int duration, enum EffMotionType curve)
{
    MGEFF_ANIMATION anim = NULL;
    mPieceItem *item = NULL;
    if ((item = _c(self)->searchItem(self, child))) {
        POINTF ptf_s;
        POINTF ptf_e;
        anim = mGEffAnimationCreate(child, _piece_anim_cb, ANIM_SCALE, MGEFF_POINTF);
        assert(anim);

        ptf_s.xf = item->wscalefactor;
        ptf_s.yf = item->hscalefactor;
        ptf_e.xf = (wscalefactor < 0.0) ? 0.0 : wscalefactor;
        ptf_e.yf = (hscalefactor < 0.0) ? 0.0 : hscalefactor;

        mGEffAnimationSetStartValue(anim, &ptf_s);
        mGEffAnimationSetEndValue(anim, &ptf_e);
        mGEffAnimationSetDuration(anim, duration);
        mGEffAnimationSetCurve(anim, curve);
        return anim;
    }
    return NULL;

}
/****************************SCALE PIECE END***********************************/

/****************************ROTATE PIECE START********************************/
static void mPanelPiece_rotatePiece(mPanelPiece *self, mHotPiece *child, float angle, int x, int y, int z)
{
    mPieceItem *item = NULL;

    // update old position
    PanelPiece_invalidatePiece((mHotPiece*)child, NULL);

    if ((item = _c(self)->searchItem(self, child))) {
        item->normalVector.angle = (angle >= ROTATE_90 * 2) ? 0.0 : angle;
        item->normalVector.x = x;
        item->normalVector.y = y;
        item->normalVector.z = z;
    }

    // update new position
    PanelPiece_invalidatePiece((mHotPiece*)child, NULL);
}

static MGEFF_ANIMATION mPanelPiece_rotatePieceWithAnimation(mPanelPiece *self, mHotPiece *child,
        float angle, int x, int y, int z, int duration, enum EffMotionType curve)
{
    MGEFF_ANIMATION anim = NULL;
    mPieceItem *item = NULL;
    float s, e;
    if ((item = _c(self)->searchItem(self, child))) {
        anim = mGEffAnimationCreate(child, _piece_anim_cb, ANIM_ROTATE, MGEFF_FLOAT);
        assert(anim);

        s = item->normalVector.angle;
	e = angle;
        item->normalVector.x = x;
        item->normalVector.y = y;
        item->normalVector.z = z;

        mGEffAnimationSetStartValue(anim, &s);
        mGEffAnimationSetEndValue(anim, &e);
        mGEffAnimationSetDuration(anim, duration);
        mGEffAnimationSetCurve(anim, curve);
        return anim;
    }
    return NULL;
}
/****************************ROTATE PIECE END**********************************/

/****************************ALPHA PIECE START*********************************/
static void mPanelPiece_setPieceAlpha(mPanelPiece *self, mHotPiece *child, int alpha)
{
    mPieceItem *item = NULL;

    // update old position
    PanelPiece_invalidatePiece((mHotPiece*)child, NULL);

    if ((item = _c(self)->searchItem(self, child))) {
        item->alpha = alpha;
    }

    // update new position
    PanelPiece_invalidatePiece((mHotPiece*)child, NULL);
}

static MGEFF_ANIMATION mPanelPiece_setPieceAlphaWithAnimation(mPanelPiece *self, mHotPiece *child,
        int alpha, int duration, enum EffMotionType curve)
{
    MGEFF_ANIMATION anim = NULL;
    mPieceItem *item = NULL;
    if ((item = _c(self)->searchItem(self, child))) {
        anim = mGEffAnimationCreate(child, _piece_anim_cb, ANIM_ALPHA, MGEFF_INT);
        assert(anim);

        mGEffAnimationSetStartValue(anim, &item->alpha);
        mGEffAnimationSetEndValue(anim, &alpha);
        mGEffAnimationSetDuration(anim, duration);
        mGEffAnimationSetCurve(anim, curve);
        return anim;
    }
    return NULL;
}
/****************************ALPHA PIECE END***********************************/

typedef struct {
    list_t list;
    int message;
    NCS_PIECE_EVENT_HANDLER handler;
} event_handler_t;

static mPieceItem *itemAt(mPanelPiece* self, int x, int y)
{
    mPieceItem *item = NULL;

    if(x >= self->left && x < self->right
            && y >= self->top && y < self->bottom)
    {
        RECT rc;
        //TODO: we should use reverse_iterator here
        mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
        mHotPiece *piece;
        while ((item = _c(iter)->prev(iter))) {
            piece = item->piece;;
            assert(piece);

            if (_c(piece)->getRect(piece, &rc) >= 0) {
                OffsetRect(&rc, item->x, item->y);
                if (PtInRect(&rc, x, y)) {
                    break;
                }
            }
        }
        DELETE(iter);
    }
    return item;
}

static mHotPiece * mPanelPiece_childAt(mPanelPiece* self, int x, int y) {
    mPieceItem *item = itemAt(self, x, y);
    return item ? item->piece : NULL;
}

static void mPanelPiece_appendEventHandler(mPanelPiece *self,
        int message, NCS_PIECE_EVENT_HANDLER handler) {
    event_handler_t *entry;
    entry = (event_handler_t *)calloc(1, sizeof(*entry));
    entry->message = message;
    entry->handler = handler;
    list_add(&entry->list, self->eventHandlers.prev);
}

static void mPanelPiece_delEventHandler(mPanelPiece *self, int message)
{
    list_t *i;
    list_for_each(i, &self->eventHandlers) {
        event_handler_t *entry = (event_handler_t*)i;
        if (entry->message == message) {
            free(entry);
            list_del(i);
        }
    }
}

static void mPanelPiece_clearEventHandler(mPanelPiece *self)
{
    list_t *i, *n;
    list_for_each_safe(i, n, &self->eventHandlers) {
        list_del(i);
        free(i);
    }
}

static NCS_PIECE_EVENT_HANDLER find_event_handle(mPanelPiece *self, int message) {
    list_t *list;
    list_for_each(list, &self->eventHandlers) {
        event_handler_t *entry = (event_handler_t *)list;
        if (entry->message == message) {
            return entry->handler;
        }
    }
    return NULL;
}

static int mPanelPiece_processMessage(mPanelPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    NCS_PIECE_EVENT_HANDLER handler = NULL;
#ifdef HOVER
    mHotPiece *hotfocus;
#endif
    mWidget *widget = (mWidget *)owner;

    switch (message) {
        case MSG_MOUSEMOVEIN:
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
        case MSG_MOUSEMOVE:
            handler = find_event_handle(self, message);
            break;
        default:
            if (message >= MSG_USER)
                handler = find_event_handle(self, message);
            break;
    }

    if (handler
            && (message != MSG_MOUSEMOVEIN || (mObject*)self != mWidget_getHoveringFocus((mWidget*)owner))
            && handler((mHotPiece *)self, message, wParam, lParam, owner) >= 0) {
        return 0;
    }

    /* See also mwidget.c */

    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK){
        int x, y;
        mPieceItem *item = NULL;
#ifdef HOVER
        hotfocus = (mHotPiece*)self->hovering_piece;
#endif

        x = LOSWORD(lParam);
        y = HISWORD(lParam);
        item = itemAt(self, x, y);

        if(!item)
        {
#ifdef HOVER
            if(hotfocus)
                _c(hotfocus)->processMessage(hotfocus, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)widget);
#endif
            self->hovering_piece = NULL;
            mWidget_releaseHoveringFocus();
        }
        else
        {
            mHotPiece *piece = item->piece;
#ifdef HOVER
            if(piece != hotfocus)
            {
                if(hotfocus)
                    _c(hotfocus)->processMessage(hotfocus, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)widget);
                _c(piece)->processMessage(piece, MSG_MOUSEMOVEIN, TRUE, 0, (mObject*)widget);
                self->hovering_piece = piece;
                mWidget_setHoveringFocus(widget, (mObject*)piece);
            }
#endif
            return _c(piece)->processMessage(piece, message, wParam, MAKELONG(x - item->x, y - item->y), (mObject*)widget);
        }
    }
    else if (message == MSG_MOUSEMOVEIN)
    {
        if(!wParam) //mouse move out
        {
#ifdef HOVER
            hotfocus = (mHotPiece*)self->hovering_piece;
            if(hotfocus){
                _c(hotfocus)->processMessage(hotfocus, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)widget);
            }
#endif
            self->hovering_piece = NULL;
            mWidget_releaseHoveringFocus();
        }
    }
    
    return 0;
}

static void _update_callback(MGEFF_ANIMATION handle, void *target, intptr_t id, void *value)
{
    mPanelPiece *self = (mPanelPiece*)target;
    assert (self && self->owner);

    //printf("_update_callback: %x\n", handle);

    if (self->update_flag) {
        self->update_flag = FALSE;
        PanelPiece_update((mHotPiece*)self, TRUE);
    }
}

static void _update_finished_cb(MGEFF_ANIMATION handle)
{
    mPanelPiece* self = (mPanelPiece*) mGEffAnimationGetContext(handle);
    self->update_anim = NULL;
}

#ifndef MGEFF_INFINITE
#define MGEFF_INFINITE  99999999
#endif

static void mPanelPiece_setOwner(mPanelPiece* self, mWidget* owner)
{
    assert (self->owner == NULL && self->parent == NULL);

    // TODO: mGEffDeinit() ??
    //mGEffInit();

    // set owner
    self->owner = owner;
    mWidget_setHoveringFocus(owner, (mObject *)self);
}

static mWidget *mPanelPiece_getOwner(mPanelPiece *self) {
    mPanelPiece *parent;

    for (parent=self; parent != (mPanelPiece*)-1 && parent->parent; 
            parent=(mPanelPiece *)parent->parent) {
    }

    if (parent == (mPanelPiece*)-1) {
    #ifdef DBG_REF/*DBG_REF defined in mpanelpiece.h*/
        fprintf(stderr, "%p piece is delContent from parent!!!\n", self);
    #endif
        return NULL;
    }
    if (parent)
        return parent->owner;
    else return NULL;
}

static void mPanelPiece_animationAsyncRun(mPanelPiece *self, MGEFF_ANIMATION anim, int keepalive)
{
    /* assert (PanelPiece_isTopPanel(self)); */
    self = PanelPiece_getTopPanel((mHotPiece*)self);
    
    // stop current update animation if exists
    if (self->update_anim)
        mGEffAnimationStop(self->update_anim);
    // create a new update animation
    self->update_anim = mGEffAnimationCreate(self, _update_callback, (intptr_t)(self+1), MGEFF_INT);
    mGEffAnimationSetProperty(self->update_anim, MGEFF_PROP_LOOPCOUNT, MGEFF_INFINITE);
    mGEffAnimationSetFinishedCb(self->update_anim, _update_finished_cb);
    mGEffAnimationSetContext(self->update_anim, self);

    
#ifdef ENABLE_ANIM_FPS_TEST
    anim_fps_test_start (1);
#endif

    // run user's animation
    mGEffAnimationAsyncRun(anim);
    mGEffAnimationSetProperty(anim, MGEFF_PROP_KEEPALIVE, keepalive);

    // run update_anim after user's anim, so it will be the last one in the schedular queue
    mGEffAnimationAsyncRun(self->update_anim);
    mGEffAnimationSetProperty(self->update_anim, MGEFF_PROP_KEEPALIVE, 0);
}

static void mPanelPiece_animationSyncRunAndDelete(mPanelPiece *self, MGEFF_ANIMATION anim)
{
    // get loopcount & duration from user's animation
    //int loopcount;
    int duration;

    /* assert(PanelPiece_isTopPanel(self)); */
    self = PanelPiece_getTopPanel((mHotPiece*)self);
    
    if (anim == NULL)
        return;

    //loopcount = mGEffAnimationGetProperty(anim, MGEFF_PROP_LOOPCOUNT);
    duration = mGEffAnimationGetDuration(anim);

    // stop current update animation if exists
    if (self->update_anim)
        mGEffAnimationStop(self->update_anim);
    // create a new update animation
    self->update_anim = mGEffAnimationCreate(self, _update_callback, (intptr_t)(self+1), MGEFF_INT);
#if 1
    mGEffAnimationSetProperty(self->update_anim, MGEFF_PROP_LOOPCOUNT, MGEFF_INFINITE);
#else
    mGEffAnimationSetProperty(self->update_anim, MGEFF_PROP_LOOPCOUNT, loopcount);
#endif
    mGEffAnimationSetDuration(self->update_anim, duration);
    mGEffAnimationSetFinishedCb(self->update_anim, _update_finished_cb);
    mGEffAnimationSetContext(self->update_anim, self);

#if 1

#ifdef ENABLE_ANIM_FPS_TEST
    anim_fps_test_start (1);
#endif

    mGEffAnimationAsyncRun(self->update_anim);
    mGEffAnimationSetProperty(self->update_anim, MGEFF_PROP_KEEPALIVE, 0);
    mGEffAnimationSyncRun(anim);
    mGEffAnimationDelete(anim);
#else
    // create a new animgrp
    self->animgrp = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    // add user's animation
    mGEffAnimationAddToGroup(self->animgrp, anim);
    // add update_anim after user's anim, so it will be the last one in the schedular queue
    mGEffAnimationAddToGroup(self->animgrp, self->update_anim);

    // run animation
    mGEffAnimationSyncRun(self->animgrp);
    mGEffAnimationDelete(self->animgrp);
#endif
}

mShapeTransRoundPiece* mPanelPiece_getBkgndPiece(mPanelPiece *self)
{
    return self->bkgndPiece;
}

void mPanelPiece_setBkgndPiece(mPanelPiece *self, mShapeTransRoundPiece* piece)
{
    if (self->bkgndPiece && self->bkgndPiece != piece) {
        UNREFPIECE(self->bkgndPiece);
    }
    else if (self->bkgndPiece && self->bkgndPiece == piece) {
        return;
    }
    self->bkgndPiece = piece;
    ADDREFPIECE(self->bkgndPiece);
}

static void mPanelPiece_invalidatePiece(mPanelPiece *self, mHotPiece *piece, const RECT *rc, BOOL reserveCache)
{
    RECT dirtyRect, parentRC;
    mPieceItem* item;
    mPanelPiece *parent;

    assert(piece);

    if (rc == NULL) {
        _c(piece)->getRect(piece, &dirtyRect);
    } else {
        dirtyRect = *rc;
    }

    item = _c(self)->searchItem(self, piece);
    assert(item);

    /* set dirtyRect if piece has been scaled */
    if (item->wscalefactor != 1 || item->hscalefactor != 1) {
        _c (item->piece)->getRect (item->piece, &dirtyRect);
        if (item->wscalefactor > 1) {
            int new_w = RECTW (dirtyRect) * item->wscalefactor;
            int offset_w = (new_w - RECTW (dirtyRect)) / 2;

            dirtyRect.left -= offset_w;
            dirtyRect.right += offset_w;
        }

        if (item->hscalefactor > 1) {
            int new_h = RECTH (dirtyRect) * item->hscalefactor;
            int offset_h = (new_h - RECTH (dirtyRect)) / 2;

            dirtyRect.top -= offset_h;
            dirtyRect.bottom += offset_h;
        }
    }

    // update coordinate
    OffsetRect(&dirtyRect, item->x, item->y);

    // intersect with parent
    _c(self)->getRect(self, &parentRC);
    IntersectRect(&dirtyRect, &parentRC, &dirtyRect);

    parent = (mPanelPiece *)self->parent;
    if (parent == (mPanelPiece *)-1) {
        fprintf(stderr, "%p piece is delContent from parent!!!\n", self);
        // assert(0);
    }else if (parent) {
        _c(parent)->invalidatePiece(parent, (mHotPiece *)self, &dirtyRect, reserveCache);
    }else{
        mWidget* owner;

        owner = (mWidget*) ((mPanelPiece*)self)->owner;
        if ( NULL != owner ) {
            OffsetRect(&dirtyRect, parentRC.left, parentRC.top);
            InvalidateRect(owner->hwnd, &dirtyRect, FALSE);
            ((mPanelPiece*)self)->update_flag = TRUE;
        }
    }
}

BEGIN_MINI_CLASS(mPanelPiece, mStaticPiece)
    CLASS_METHOD_MAP(mPanelPiece, construct )
    CLASS_METHOD_MAP(mPanelPiece, destroy   )
    CLASS_METHOD_MAP(mPanelPiece, setClipRect)
    CLASS_METHOD_MAP(mPanelPiece, setProperty)
    CLASS_METHOD_MAP(mPanelPiece, reLayout  )
    CLASS_METHOD_MAP(mPanelPiece, setLayoutManager)
    CLASS_METHOD_MAP(mPanelPiece, setRect   )
    CLASS_METHOD_MAP(mPanelPiece, paint     )
    CLASS_METHOD_MAP(mPanelPiece, hitTest   )
    CLASS_METHOD_MAP(mPanelPiece, addContent)
    CLASS_METHOD_MAP(mPanelPiece, addContentToLayout)
    CLASS_METHOD_MAP(mPanelPiece, addContentFinished)
    CLASS_METHOD_MAP(mPanelPiece, delContent)
    CLASS_METHOD_MAP(mPanelPiece, clearContents)
    CLASS_METHOD_MAP(mPanelPiece, searchItem)
    CLASS_METHOD_MAP(mPanelPiece, createItemManager)
    CLASS_METHOD_MAP(mPanelPiece, createItemNode)
    CLASS_METHOD_MAP(mPanelPiece, initItemNode )
    CLASS_METHOD_MAP(mPanelPiece, appendEventHandler)
    CLASS_METHOD_MAP(mPanelPiece, delEventHandler)
    CLASS_METHOD_MAP(mPanelPiece, clearEventHandler)
    CLASS_METHOD_MAP(mPanelPiece, processMessage)
    CLASS_METHOD_MAP(mPanelPiece, childAt)
    CLASS_METHOD_MAP(mPanelPiece, movePiece    )
    CLASS_METHOD_MAP(mPanelPiece, scalePiece   )
    CLASS_METHOD_MAP(mPanelPiece, rotatePiece  )
    CLASS_METHOD_MAP(mPanelPiece, setPieceAlpha)
    CLASS_METHOD_MAP(mPanelPiece, movePieceWithAnimation    )
    CLASS_METHOD_MAP(mPanelPiece, scalePieceWithAnimation   )
    CLASS_METHOD_MAP(mPanelPiece, rotatePieceWithAnimation  )
    CLASS_METHOD_MAP(mPanelPiece, setPieceAlphaWithAnimation)
    CLASS_METHOD_MAP(mPanelPiece, setOwner)
    CLASS_METHOD_MAP(mPanelPiece, getOwner)
    CLASS_METHOD_MAP(mPanelPiece, animationSyncRunAndDelete)
    CLASS_METHOD_MAP(mPanelPiece, animationAsyncRun)
    CLASS_METHOD_MAP(mPanelPiece, getBkgndPiece)
    CLASS_METHOD_MAP(mPanelPiece, setBkgndPiece)
    CLASS_METHOD_MAP(mPanelPiece, enableChildCache)
    CLASS_METHOD_MAP(mPanelPiece, updateChildCache)
    CLASS_METHOD_MAP(mPanelPiece, invalidatePiece)
END_MINI_CLASS


// global functions
void PanelPiece_invalidatePiece(mHotPiece *piece, const RECT *rc) {
    mPanelPiece *parent;

    parent = (mPanelPiece *)piece->parent;
    if (parent == (mPanelPiece *)-1) {
        fprintf(stderr, "%p piece is delContent from parent!!!\n", piece);
    #if 0
        assert(0);
    #endif
    }else if (parent) {
        _c(parent)->invalidatePiece(parent, piece, rc, FALSE);
    }else{
        mWidget* owner;

        owner = (mWidget*) ((mPanelPiece*)piece)->owner;
        if ( NULL != owner ) {
            InvalidateRect(owner->hwnd, rc, FALSE);
            ((mPanelPiece*)piece)->update_flag = TRUE;
        }
    }
}

void PanelPiece_update(mHotPiece *piece, BOOL fErase)
{
    mPanelPiece* topPanel = PanelPiece_getTopPanel(piece);
    if (topPanel) {
        // reach top level container
        mWidget* owner = (mWidget*)topPanel->owner;

        extern void GUIAPI UpdateInvalidRect (HWND hWnd, BOOL bErase);
        UpdateInvalidRect(owner->hwnd, fErase);
    }
}

mPanelPiece *PanelPiece_getTopPanel(mHotPiece *piece)
{
    mHotPiece *p = piece;
    mHotPiece *parent;
    assert(piece);

    parent = piece->parent;
    while (parent) {
        piece = parent;
        if (parent == (mHotPiece*)-1) {
            fprintf(stderr, "%p piece is delContent from parent!!!\n", p);
            return NULL;
        }
        parent = piece->parent;
    }

    //assert(INSTANCEOF(piece, mPanelPiece) && ((mPanelPiece*)piece)->owner);
    assert(INSTANCEOF(piece, mPanelPiece));
    if (((mPanelPiece*)piece)->owner)
        return (mPanelPiece*)piece;
    else 
        return NULL;
}

