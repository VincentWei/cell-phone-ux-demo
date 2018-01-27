#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncsconfig.h>
#include <mgncs/mcommon.h>
#include <mgncs/mobject.h>
#include <mgncs/mcomponent.h>
#include <mgncs/mwidget.h>
#include <mgncs/mrdr.h>
#include <mgncs/piece.h>

#include <mgplus/mgplus.h>

#include "mphonestaticrdrpiece.h"

#define LABEL_TEXT_LEN 16

static void mPhoneStaticRDRPiece_construct(mPhoneStaticRDRPiece *self, DWORD add_data)
{
	Class(mStaticPiece).construct((mStaticPiece*)self, add_data);

    self->main_font = NULL;
    self->sub_font = NULL;

    self->main_font_color = 0;
    self->sub_font_color = 0;

    self->main_text_str = (char *)calloc(LABEL_TEXT_LEN,sizeof(char));
    self->sub_text_str = (char *)calloc(LABEL_TEXT_LEN,sizeof(char));

    return;
}

static void mPhoneStaticRDRPiece_destroy(mPhoneStaticRDRPiece *self)
{
	free(self->main_text_str);
	free(self->sub_text_str);

    Class(mStaticPiece).destroy((mStaticPiece*)self);
    return;
}

static void mPhoneStaticRDRPiece_paint(mPhoneStaticRDRPiece *self, 
                            HDC hdc, mWidget *owner, DWORD add_data)
{
    DWORD uFormat = DT_CENTER|DT_SINGLELINE|DT_VCENTER;
    DWORD uFormat1 = DT_CENTER|DT_SINGLELINE|DT_TOP;
    ARGB color1 = self->main_font_color;
    ARGB color2 = self->sub_font_color;

    if(owner == NULL)
        return ;
	
    if (self->main_font != NULL && self->main_text_str != NULL)
    {
        SelectFont(hdc, self->main_font);

        if (self->main_font_color != 0)
        {
    	    SetTextColor(hdc, RGBA2Pixel(hdc, 
                        color1>>16&0xff,color1>>8&0xff, color1&0xff, color1>>24&0xff));
        }
        SetBkMode(hdc, BM_TRANSPARENT);
        if (self->sub_font == NULL)
        {
            _c(self)->getRect(self,&self->main_rc);
        }
        if (self->sub_font != NULL && self->sub_text_str != NULL)
            uFormat = DT_CENTER|DT_SINGLELINE|DT_BOTTOM;

    	DrawText(hdc, self->main_text_str, -1, &self->main_rc, uFormat);
    }
    if (self->sub_font != NULL&&self->sub_text_str != NULL)
    {
        SelectFont(hdc, self->sub_font);
    
        if (self->sub_font_color != 0)
        {
    	    SetTextColor(hdc, RGBA2Pixel(hdc, 
                        color2>>16&0xff,color2>>8&0xff, color2&0xff, color2>>24&0xff));
        }
        SetBkMode(hdc, BM_TRANSPARENT);
    	DrawText(hdc, self->sub_text_str, -1, &self->sub_rc, uFormat1);
    }
}

static BOOL mPhoneStaticRDRPiece_setProperty(mPhoneStaticRDRPiece *self, int id, DWORD value)
{   
    RECT rc;
	switch(id)
	{
	    case NCSP_PHONESTATICRDRPIECE_MAIN_TEXT:
            assert((char*)value != NULL);
			strncpy(self->main_text_str,(char *)value,LABEL_TEXT_LEN-1);
	    	break;
	    case NCSP_PHONESTATICRDRPIECE_SUB_TEXT:
            assert((char*)value != NULL);
			strncpy(self->sub_text_str,(char *)value,LABEL_TEXT_LEN-1);
	    	break;
	    case NCSP_PHONESTATICRDRPIECE_MAIN_FONT:
            _c(self)->getRect(self,&rc);
            rc.bottom = (rc.bottom<<1)/3;
    		self->main_rc = rc;
            self->main_font = (PLOGFONT)value; 
	    	break;
	    case NCSP_PHONESTATICRDRPIECE_SUB_FONT:
            _c(self)->getRect(self,&rc);
            self->sub_font = (PLOGFONT)value; 
            if(self->sub_font == NULL)
            {
    		    self->main_rc = rc;
            }
            else
            {
                rc.top = (rc.bottom<<1)/3;
                self->sub_rc = rc;
            }
            break;
			
	    case NCSP_PHONESTATICRDRPIECE_MAIN_COLOR:
            self->main_font_color = (ARGB)value; 
	    	break;
	    case NCSP_PHONESTATICRDRPIECE_SUB_COLOR:
            self->sub_font_color = (ARGB)value; 
	    	break;
			
	    default:
		    return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
	}
	return TRUE;
}

static DWORD mPhoneStaticRDRPiece_getProperty(mPhoneStaticRDRPiece* self, int id)
{
	switch(id)
	{
	    case NCSP_PHONESTATICRDRPIECE_MAIN_TEXT:
	    	break;
	    case NCSP_PHONESTATICRDRPIECE_SUB_TEXT:
	    	break;
	    case NCSP_PHONESTATICRDRPIECE_MAIN_FONT:
                return (DWORD)self->main_font; 
	    	break;
	    case NCSP_PHONESTATICRDRPIECE_SUB_FONT:
                return (DWORD)self->sub_font; 
	    	break;
	}
	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}

BEGIN_MINI_CLASS(mPhoneStaticRDRPiece, mStaticPiece)
	CLASS_METHOD_MAP(mPhoneStaticRDRPiece, construct)
	CLASS_METHOD_MAP(mPhoneStaticRDRPiece, destroy)
	CLASS_METHOD_MAP(mPhoneStaticRDRPiece, paint)
	CLASS_METHOD_MAP(mPhoneStaticRDRPiece, setProperty)
	CLASS_METHOD_MAP(mPhoneStaticRDRPiece, getProperty)
END_MINI_CLASS

