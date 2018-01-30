#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>

#include "global.h"

#include "mnavigationitem.h"
#include "mnavigationpanelpiece.h"
#include "mnavigationbarpiece.h"

static void mNavigationBarPiece_construct (mNavigationBarPiece *self, DWORD param)
{
	Class(mPanelPiece).construct((mPanelPiece*)self, param);

    self->left_button = NULL;
    self->title_button = NULL;
    self->right_button = NULL;
}


static void mNavigationBarPiece_destroy (mNavigationBarPiece* self, DWORD param)
{
    Class(mPanelPiece).destroy((mPanelPiece*)self);
}


static BOOL mNavigationBarPiece_setProperty (mNavigationBarPiece* self, int id, DWORD value)
{
    if ( NULL == self )
        return FALSE;

    switch (id) {
        case NCSP_NAVIGATIONBARPIECE_BACKGROUND:
            if ( NULL == (mShapeTransRoundPiece*)value ) 
                return FALSE;
            self->background = (mShapeTransRoundPiece*)value;
            break;

        case NCSP_NAVIGATIONBARPIECE_BKG:
            break;

        case NCSP_NAVIGATIONBARPIECE_LEFT_BUTTON:
            self->left_button = (mButtonPanelPiece*)value;
            break;

        case NCSP_NAVIGATIONBARPIECE_TITLE_BUTTON:
            self->title_button = (mTextPiece*)value;
            break;

        case NCSP_NAVIGATIONBARPIECE_RIGHT_BUTTON:
            self->right_button = (mButtonPanelPiece*)value;
            break;

        default:
            return Class(mPanelPiece).setProperty((mPanelPiece*)self, id, value);
    }

    return TRUE;
}


static DWORD mNavigationBarPiece_getProperty (mNavigationBarPiece* self, int id)
{
    if ( NULL == self )
        return (DWORD)NULL;

	switch (id) {
        case NCSP_NAVIGATIONBARPIECE_BACKGROUND:
            return (DWORD)self->background;

        case NCSP_NAVIGATIONBARPIECE_BKG:
            return (DWORD)NULL;

        case NCSP_NAVIGATIONBARPIECE_LEFT_BUTTON:
            return (DWORD)self->left_button;

        case NCSP_NAVIGATIONBARPIECE_TITLE_BUTTON:
            return (DWORD)self->title_button;

        case NCSP_NAVIGATIONBARPIECE_RIGHT_BUTTON:
            return (DWORD)self->right_button;

        default:
            break;
	}

	return Class(mPanelPiece).getProperty((mPanelPiece*)self, id);
}


BEGIN_MINI_CLASS(mNavigationBarPiece, mPanelPiece)
	CLASS_METHOD_MAP(mNavigationBarPiece, construct)
	CLASS_METHOD_MAP(mNavigationBarPiece, destroy)
	CLASS_METHOD_MAP(mNavigationBarPiece, setProperty)
	CLASS_METHOD_MAP(mNavigationBarPiece, getProperty)
END_MINI_CLASS

