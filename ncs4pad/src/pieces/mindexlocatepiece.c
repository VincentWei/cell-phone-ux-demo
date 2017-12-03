#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mlayout_manager.h"
#include "mlinevbox.h"
#include "mpanelpiece.h"
#include "mtransroundpiece.h"
#include "mtextpiece.h"
#include "mtableviewpiece.h"
#include "mindexlocatepiece.h"

#define IL_WIDTH	20
#define IL_ITEM_WIDTH	20
#define IL_ITEM_HEIGHT	15

#define IL_BG_COLOR_NORMAL	0x00000000
#define IL_BG_COLOR_TOUCHED	0x80808080

#define MARGIN_TOP	8
#define MARGIN_BOTTOM	5
#define MARGIN_TOTAL	(MARGIN_TOP + MARGIN_BOTTOM)
#define OFFSET_Y	-1

static int rulerLength(mIndexLocatePiece* self)
{
    RECT rc;
    _c(self)->getRect(self, &rc);
    
    return RECTH(rc) - MARGIN_TOP - MARGIN_BOTTOM;
}

static void resetBackgroundRect(mIndexLocatePiece* self)
{
    RECT rc;
    
    _c(self)->getRect(self, &rc);
    rc.right = IL_WIDTH;

    _c(self->backgroundPiece)->setRect(self->backgroundPiece, &rc);
}

static mHotPiece* createBackgroundPiece(mIndexLocatePiece* self)
{
    mHotPiece* bg;

    _c(self)->setBkgndPiece(self, NEWPIECE(mShapeTransRoundPiece));
    self->backgroundPiece = (mHotPiece*)_c(self)->getBkgndPiece(self);
    bg = self->backgroundPiece;
    resetBackgroundRect(self);
    _c(bg)->setProperty(bg, NCSP_TRANROUND_BKCOLOR, IL_BG_COLOR_NORMAL);
    _c(bg)->setProperty(bg, NCSP_TRANROUND_RADIUS, 10);

    return bg;
}

static void mIndexLocatePiece_construct(mIndexLocatePiece* self, DWORD add_data)
{
    Class(mPanelPiece).construct((mPanelPiece*)self, add_data);

    {
	mLineVBox* box = (mLineVBox*)NEW(mLineVBox);
	_c(self)->setLayoutManager(self, (mLayoutManager*)box);
    }

    self->parentPiece = (mHotPiece*)add_data;
    self->backgroundPiece  = (mHotPiece*)createBackgroundPiece(self);

    self->parent = self->parentPiece;
    self->touchedFlag = 0;
}

static int positionToIndex(mIndexLocatePiece* self, int indexNum, int y)
{
    int L = rulerLength(self);
    if (y < MARGIN_TOP) {
        return 0;
    }else if (y >= MARGIN_TOP + L) {
        return indexNum-1;
    }else{
        return (y - MARGIN_TOP) * indexNum / rulerLength(self);
    }
}

static void mIndexLocatePiece_reloadData(mIndexLocatePiece* self)
{
    mTableViewPiece* parent = (mTableViewPiece*) self->parentPiece;
    int num = _c(parent)->numberOfSections(parent);
    int i;
    const char* indexVal = NULL;
    RECT rc;
    mHotPiece *child;
    int L = rulerLength(self);

    /* reset background rect */
    resetBackgroundRect(self);

    if (num <= 0) return;

    rc.left = rc.top = 0;
    rc.right = IL_ITEM_WIDTH;
    rc.bottom = L / num;

    for (i = 0; i < num; ++i) {
        indexVal = _c(parent)->indexForSection(parent, i);

        child = (mHotPiece*)NEWPIECE(mTextPiece);
        _c(child)->setRect(child, &rc);
        _c(child)->setProperty(child, NCSP_LABELPIECE_LABEL, (DWORD)indexVal);
        _c(child)->setProperty(child, NCSP_LABELPIECE_ALIGN, (DWORD)NCS_ALIGN_CENTER);
        _c(child)->setProperty(child, NCSP_LABELPIECE_VALIGN, (DWORD)NCS_VALIGN_CENTER);
        _c(child)->setProperty(child, NCSP_TEXTPIECE_TEXTCOLOR, 0xFF000000);
        _c(self)->addContent(self, child,
                0, MARGIN_TOP + i*L/num + OFFSET_Y);
    }
}

/* static void mIndexLocatePiece_paint(mIndexLocatePiece* self, */
/* 				    HDC hdc, mObject * owner, DWORD add_data) */
/* { */
/*     Class(mTableViewPiece).paint((mTableViewPiece*)self, hdc, owner, add_data); */
/* } */

static int mIndexLocatePiece_processMessage(mIndexLocatePiece* self, int message,
					    WPARAM wParam, LPARAM lParam, mObject* owner)
{
    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK) {
        /* int x = LOSWORD(lParam); */
        int y = HISWORD(lParam);
        int totalHeight = rulerLength(self);
        int num = _c((mTableViewPiece*)self->parentPiece)->numberOfSections(
                (mTableViewPiece*)self->parentPiece);
        int percent = 0;

        if (totalHeight)
            percent = positionToIndex(self, num, y);

        if (message == MSG_LBUTTONDOWN) {
            /* show background */
            _c(self->backgroundPiece)->setProperty(
                    self->backgroundPiece, NCSP_TRANROUND_BKCOLOR, IL_BG_COLOR_TOUCHED);

            PanelPiece_invalidatePiece(self->parentPiece, NULL);

            self->touchedFlag = 1;

            ncsRaiseEvent((mObject*)self,
                    NCSN_INDEXLOCATEPIECE_TOUCHED, (DWORD)percent);
        }
        else if (message == MSG_LBUTTONUP) {
            /* hide background */
            _c(self->backgroundPiece)->setProperty(
                    self->backgroundPiece, NCSP_TRANROUND_BKCOLOR, IL_BG_COLOR_NORMAL);

            PanelPiece_invalidatePiece(self->parentPiece, NULL);

            self->touchedFlag = 0;
        }
        else if (message == MSG_MOUSEMOVE) {
            if (self->touchedFlag)
                ncsRaiseEvent((mObject*)self,
                        NCSN_INDEXLOCATEPIECE_TOUCHED, (DWORD)percent);
        }
    } 
    else if (message == MSG_MOUSEMOVEIN) {
        if (wParam) {	/* move in */
            /* show background */
            _c(self->backgroundPiece)->setProperty(
                    self->backgroundPiece, NCSP_TRANROUND_BKCOLOR, IL_BG_COLOR_TOUCHED);

            PanelPiece_invalidatePiece(self->parentPiece, NULL);
        }
        else {		/* move out */
            /* hide background */
            _c(self->backgroundPiece)->setProperty(
                    self->backgroundPiece, NCSP_TRANROUND_BKCOLOR, IL_BG_COLOR_NORMAL);

            PanelPiece_invalidatePiece(self->parentPiece, NULL);

            self->touchedFlag = 0;
        }
    }

    return Class(mPanelPiece).processMessage((mPanelPiece*)self,
            message, wParam, lParam, owner);
}

static void mIndexLocatePiece_destroy(mIndexLocatePiece * self)
{

    Class(mPanelPiece).destroy((mPanelPiece*)self);
}

BEGIN_MINI_CLASS(mIndexLocatePiece, mPanelPiece)
    CLASS_METHOD_MAP(mIndexLocatePiece, construct)
    CLASS_METHOD_MAP(mIndexLocatePiece, processMessage)
    CLASS_METHOD_MAP(mIndexLocatePiece, destroy)
    CLASS_METHOD_MAP(mIndexLocatePiece, reloadData)
END_MINI_CLASS
