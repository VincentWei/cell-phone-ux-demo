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
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>

#include "mphoneseparatorpiece.h"
    
static void mPhoneSeparatorPiece_construct(mPhoneSeparatorPiece *self, DWORD add_data)
{
    Class(mStaticPiece).construct((mStaticPiece*)self, add_data);

    self->darkercolor  = 0x0;
    self->lightercolor = 0x0;

    self->direct_mode = PHONESEPARATORPIECE_HERT; 

    return;
}

static void mPhoneSeparatorPiece_destroy(mPhoneSeparatorPiece *self)
{
    Class(mStaticPiece).destroy((mStaticPiece*)self);

    return;
}

static inline void line(HDC hdc, int x0,int y0, int x1, int y1, gal_pixel color)
{
    gal_pixel old = SetPenColor(hdc, color);

    MoveTo(hdc, x0, y0);

    LineTo(hdc, x1, y1);

    SetPenColor(hdc, old);

    return;
}

static void mPhoneSeparatorPiece_paint(mPhoneSeparatorPiece *self, 
                            HDC hdc, mWidget *owner, DWORD add_data)
{
    RECT rect;
    int middle;

    if ((self->darkercolor == self->lightercolor)
            && (self->darkercolor == 0))
        return;

    _c(self)->getRect(self, &rect);

    if(self->direct_mode == PHONESEPARATORPIECE_HERT){
        int x;

        middle = (rect.top + rect.bottom) / 2;

        for (x = rect.top; x < middle; x++){
            line (hdc, rect.left, x, rect.right, x, ncsColor2Pixel(hdc, self->darkercolor));
        }

        for (x = middle; x < rect.bottom; x++){
            line (hdc, rect.left, x, rect.right, x, ncsColor2Pixel(hdc, self->lightercolor));
        }
    }
    else {
        int y;

        middle = (rect.left + rect.right) / 2;

        for (y = rect.left; y < middle; y++){
            line (hdc, y, rect.top, y, rect.bottom, ncsColor2Pixel(hdc, self->darkercolor));
        }

        for (y = middle; y < rect.right; y++) {
            line (hdc, y, rect.top, y, rect.bottom, ncsColor2Pixel(hdc, self->lightercolor));
        }
    }
}

static BOOL mPhoneSeparatorPiece_setProperty(mPhoneSeparatorPiece *self, int id, DWORD value)
{
    switch(id)
    {
        case NCSP_PHONESEPARATORPIECE_DARKER_COLOR:
            self->darkercolor = value; 
            break;
        case NCSP_PHONESEPARATORPIECE_LIGHTER_COLOR:
            self->lightercolor = value; 
            break;
        case NCSP_PHONESEPARATORPIECE_DIRECT_MODE:
            self->direct_mode = (PHONESEPARATORPIECE_DIRECT_MODE_E)value; 
            break;
        default:
            return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
    }

    return TRUE;
}

static DWORD mPhoneSeparatorPiece_getProperty(mPhoneSeparatorPiece* self, int id)
{
    switch(id)
    {
        case NCSP_PHONESEPARATORPIECE_DARKER_COLOR:
            return (DWORD)self->darkercolor;
        case NCSP_PHONESEPARATORPIECE_LIGHTER_COLOR:
            return (DWORD)self->lightercolor;
        case NCSP_PHONESEPARATORPIECE_DIRECT_MODE:
            return (PHONESEPARATORPIECE_DIRECT_MODE_E)self->direct_mode; 
    }

    return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}

BEGIN_MINI_CLASS(mPhoneSeparatorPiece, mStaticPiece)
    CLASS_METHOD_MAP( mPhoneSeparatorPiece, construct     )
    CLASS_METHOD_MAP( mPhoneSeparatorPiece, destroy       )
    CLASS_METHOD_MAP( mPhoneSeparatorPiece, paint         )
    CLASS_METHOD_MAP( mPhoneSeparatorPiece, setProperty   )
    CLASS_METHOD_MAP( mPhoneSeparatorPiece, getProperty   )
END_MINI_CLASS

