#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h> 

#include "mexseparatorpiece.h"

static inline void line(HDC hdc, int x0,int y0, int x1, int y1, gal_pixel color)
{
	gal_pixel old = SetPenColor(hdc, color);
	MoveTo(hdc, x0, y0);
	LineTo(hdc, x1, y1);
	SetPenColor(hdc, old);
}

static void mExSeparatorPiece_construct(mExSeparatorPiece *self, DWORD add_data)
{
    Class(mStaticPiece).construct((mStaticPiece*)self, add_data);
    self->direction = EXSEPARATOR_DIRECTION_HORIZONTAL;
    self->color = 0xffced3d6;
    self->size = 2;
}

static void mExSeparatorPiece_paint(mExSeparatorPiece *self, HDC hdc, mWidget *onwer, DWORD add_data)
{
    int i;
    DWORD color = self->color;
    RECT rc;
    int dark_width;
    int light_width;
    gal_pixel pixel_color;
    DWORD color_data;
    
    _c(self)->getRect(self, &rc);

    /* single line */
    if (self->size == 0) {
        if(self->direction == EXSEPARATOR_DIRECTION_VERTICAL) {
            line(hdc, rc.left, rc.top, rc.left, rc.bottom, ncsColor2Pixel(hdc, self->color));
        } else if (self->direction== EXSEPARATOR_DIRECTION_HORIZONTAL) {
            line(hdc, rc.left, rc.top, rc.right, rc.top, ncsColor2Pixel(hdc, self->color));
        }
        return;
    }

    dark_width = self->size/2;
    light_width = self->size - dark_width;

    for (i=0; i<dark_width; ++i) {
        color_data = ncsCommRDRCalc3dboxColor (color, (NCSR_COLOR_DARKER/dark_width)*(i+1));
        pixel_color = ncsColor2Pixel(hdc, color_data);
        if (self->direction == EXSEPARATOR_DIRECTION_VERTICAL) {

            line(hdc, rc.left+i, rc.top, rc.left+i, rc.bottom, pixel_color);
        } else {
            line(hdc, rc.left, rc.top+i, rc.right, rc.top+i, pixel_color);
        }
    }

    for (i=0; i<light_width; ++i) {
        color_data = ncsCommRDRCalc3dboxColor (color, (NCSR_COLOR_LIGHTER/light_width)*(i+1));
		pixel_color = ncsColor2Pixel(hdc, color_data);
        if (self->direction == EXSEPARATOR_DIRECTION_VERTICAL) {
            line(hdc, rc.left+dark_width+i, rc.top, 
                      rc.left+dark_width+i, rc.bottom, pixel_color);
        } else {
            line(hdc, rc.left, rc.top+dark_width+i, 
                      rc.right, rc.top+dark_width+i, pixel_color);
        }
    }
}

static DWORD mExSeparatorPiece_getProperty(mExSeparatorPiece *self, int id)
{
    if (id == NCSP_EXSEPARATOR_DIRECTION) {
        return (DWORD)self->direction;
    } else if (id == NCSP_EXSEPARATOR_COLOR) {
        return (DWORD)self->color;
    } else if (id == NCSP_EXSEPARATOR_SIZE) {
        return (DWORD)self->size;
    } else {
        return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
    }
}

static BOOL mExSeparatorPiece_setProperty(mExSeparatorPiece *self, int id, DWORD value)
{
    if (id == NCSP_EXSEPARATOR_DIRECTION) {
        self->direction = (int)value;
    } else if (id == NCSP_EXSEPARATOR_COLOR) {
        self->color = value;
    } else if (id == NCSP_EXSEPARATOR_SIZE) {
        self->size = (int)value;
    } else {
        return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
    }
    return TRUE;
}

BEGIN_MINI_CLASS(mExSeparatorPiece, mStaticPiece)
	CLASS_METHOD_MAP(mExSeparatorPiece, paint)
	CLASS_METHOD_MAP(mExSeparatorPiece, setProperty)
	CLASS_METHOD_MAP(mExSeparatorPiece, getProperty)
	CLASS_METHOD_MAP(mExSeparatorPiece, construct)
END_MINI_CLASS

