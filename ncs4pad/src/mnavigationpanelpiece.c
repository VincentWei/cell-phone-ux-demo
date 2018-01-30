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
#include "mnavigationbarpiece.h"
#include "mnavigationpanelpiece.h"

#define NAVIGATIONPANELPIECE_OPAQUE                      255
#define NAVIGATIONPANELPIECE_TRANSPARENT                 128
#define NAVIGATIONPANELPIECE_NOVISIBLE                   0 
#define NAVIGATIONPANELPIECE_ANIMATION_DURATION          500


/* get the current navigation item from the stack(list) */
static mNavigationItem* get_current_item (mNavigationPanelPiece* self)
{
    item_list_t* current = NULL;
    if ( list_empty(&self->item_head) )
        return NULL;
    
    current = (item_list_t*)self->item_head.next;
    if ( NULL == current)
        return NULL;
    
    return current->item;
}


/* get next navigation item after current from the stack(list)  */
static mNavigationItem* get_next_item (mNavigationPanelPiece* self)
{
    item_list_t* current = NULL;
    item_list_t* next = NULL;
    
    if ( list_empty(&self->item_head) )
        return NULL;
    
    current = (item_list_t*)self->item_head.next;
    if ( NULL == current)
        return NULL;
    
    next = (item_list_t*)current->list.next;
    if ( NULL == next || next == (item_list_t*)&self->item_head)
        return NULL;
    
    return next->item;
}


/* check whether the item is root item */
static BOOL is_root_item (mNavigationPanelPiece* self, mNavigationItem* item)
{
    item_list_t* root = NULL;
    if ( list_empty(&self->item_head) )
        return FALSE;
    
    root = (item_list_t*)self->item_head.prev;
    if ( NULL == root)
        return FALSE;
    
    return (BOOL)(root->item == item);
}


/* create change navigation bar animation */
static MGEFF_ANIMATION create_bar_animation (mNavigationPanelPiece* self, 
        mNavigationItem* src_item, mNavigationItem* target_item, BOOL push)
{
    int i;
    int start, end;
    RECT rect;
    MGEFF_ANIMATION src_alpha_anim[3];
    MGEFF_ANIMATION src_move_anim[3];
    MGEFF_ANIMATION target_alpha_anim[3];
    MGEFF_ANIMATION target_move_anim[3];
    MGEFF_ANIMATION bar_anim;
    MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

    mHotPiece* src_item_piece[3];
    mHotPiece* target_item_piece[3];
    mPieceItem* piece_item = NULL;
    mNavigationBarPiece* bar = self->bar;


    /* animation of button on the bar */
    src_item_piece[0] = (mHotPiece*)src_item->bar_left_button;
    src_item_piece[1] = (mHotPiece*)src_item->bar_title_button;
    src_item_piece[2] = (mHotPiece*)src_item->bar_right_button;

    target_item_piece[0] = (mHotPiece*)target_item->bar_left_button;
    target_item_piece[1] = (mHotPiece*)target_item->bar_title_button;
    target_item_piece[2] = (mHotPiece*)target_item->bar_right_button;

    for ( i = 0; i < 3; i++ ) {
        /* src item animation */
        piece_item = NULL;
        if ( NULL != src_item_piece[i] ) {
            /* src item fade out animation */
            _c(bar)->setPieceAlpha(bar, src_item_piece[i], NAVIGATIONPANELPIECE_OPAQUE);
            src_alpha_anim[i] = _c(bar)->setPieceAlphaWithAnimation(bar, src_item_piece[i], 
                    NAVIGATIONPANELPIECE_NOVISIBLE, NAVIGATIONPANELPIECE_ANIMATION_DURATION/2, OutCubic);
            mGEffAnimationAddToGroup(group, src_alpha_anim[i]);

            /* src item move out animation */
            _c(src_item_piece[i])->getRect(src_item_piece[i], &rect);
            piece_item = _c(bar)->searchItem(bar, src_item_piece[i]);

            if ( NULL != piece_item ) {
                if (push)
                    end = piece_item->x - RECTW(rect)/2;
                else 
                    end = piece_item->x + RECTW(rect)/2;

                src_move_anim[i] = _c(bar)->movePieceWithAnimation(bar, src_item_piece[i], 
                        end, piece_item->y, 
                        NAVIGATIONPANELPIECE_ANIMATION_DURATION/2, OutCubic);
                mGEffAnimationAddToGroup(group, src_move_anim[i]);
            }
        }
        
        /* target item animation */
        piece_item = NULL;
        if ( NULL != target_item_piece[i] ) {
            /* target item fade in animation */
            _c(bar)->setPieceAlpha(bar, target_item_piece[i], NAVIGATIONPANELPIECE_NOVISIBLE);
            target_alpha_anim[i] = _c(bar)->setPieceAlphaWithAnimation(bar, target_item_piece[i], 
                    NAVIGATIONPANELPIECE_OPAQUE, NAVIGATIONPANELPIECE_ANIMATION_DURATION/2, OutCubic);
            mGEffAnimationAddToGroup(group, target_alpha_anim[i]);

            /* target item move in animation */
            _c(target_item_piece[i])->getRect(target_item_piece[i], &rect);
            piece_item = _c(bar)->searchItem(bar, target_item_piece[i]);

            if ( NULL != piece_item ) {
                if (push) {
                    start = piece_item->x + RECTW(rect)/2;
                    end = piece_item->x;
                }
                else {
                    start = piece_item->x - RECTW(rect)/2;
                    end = piece_item->x;
                }

                _c(bar)->movePiece(bar, target_item_piece[i], start, piece_item->y);
                target_move_anim[i] = _c(bar)->movePieceWithAnimation(bar, target_item_piece[i], 
                        end, piece_item->y, 
                        NAVIGATIONPANELPIECE_ANIMATION_DURATION/2, OutCubic);
                mGEffAnimationAddToGroup(group, target_move_anim[i]);
            }
        }
    } /* end for */


    /* bar fade-in-out animation, if necessary */
    if ( src_item->style != target_item->style ) {
        /* first restore the bar according to the src style */
        _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0);
        switch ( src_item->style ) {
            case NAVIGATION_STYLE_FULLSCREEN:
                _c(self)->setPieceAlpha(self, (mHotPiece*)bar, NAVIGATIONPANELPIECE_TRANSPARENT);
                break;

            case NAVIGATION_STYLE_HIDE_BAR:
                _c(self)->setPieceAlpha(self, (mHotPiece*)bar, NAVIGATIONPANELPIECE_NOVISIBLE);
                break;
        
            case NAVIGATION_STYLE_NORMAL:
            default:
                _c(self)->setPieceAlpha(self, (mHotPiece*)bar, NAVIGATIONPANELPIECE_OPAQUE);
                break;
        }

        /* then create the change animation according to target style */
        switch ( target_item->style ) {
            case NAVIGATION_STYLE_FULLSCREEN:
                bar_anim = _c(self)->setPieceAlphaWithAnimation(self, (mHotPiece*)bar, 
                        NAVIGATIONPANELPIECE_TRANSPARENT, NAVIGATIONPANELPIECE_ANIMATION_DURATION, InQuad);
                break;

            case NAVIGATION_STYLE_HIDE_BAR:
                bar_anim = _c(self)->setPieceAlphaWithAnimation(self, (mHotPiece*)bar, 
                        NAVIGATIONPANELPIECE_NOVISIBLE, NAVIGATIONPANELPIECE_ANIMATION_DURATION, InQuad);
                break;
        
            case NAVIGATION_STYLE_NORMAL:
            default:
                bar_anim = _c(self)->setPieceAlphaWithAnimation(self, (mHotPiece*)bar, 
                        NAVIGATIONPANELPIECE_OPAQUE, NAVIGATIONPANELPIECE_ANIMATION_DURATION, InQuad);
                break;
        }

        mGEffAnimationAddToGroup(group, bar_anim);
    }
    
    return group;
}


/* create change navigation view animation */
static MGEFF_ANIMATION create_view_animation (mNavigationPanelPiece* self, 
        mNavigationItem* src_item, mNavigationItem* target_item, BOOL push)
{
    int /*src_start, */src_end;
    int target_start, target_end;
    RECT panel_rc;
    RECT src_content_rc;
    MGEFF_ANIMATION src_animation;
    MGEFF_ANIMATION target_animation;
    MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    mPieceItem* src_piece_item = NULL;
    mPieceItem* target_piece_item = NULL;

    /* get rect info */
    _c(self)->getRect(self, &panel_rc);
    _c(src_item->content)->getRect(src_item->content, &src_content_rc);
    src_piece_item = _c(self)->searchItem(self, (mHotPiece*)src_item->content);
    target_piece_item = _c(self)->searchItem(self, (mHotPiece*)target_item->content);

    /* animation direct */
    if (push) {
        //src_start = 0;
        src_end = 0 - RECTW(panel_rc);
        target_start = 0 + RECTW(panel_rc);
        target_end = 0;
    }
    else {
        //src_start = 0;
        src_end = 0 + RECTW(panel_rc);
        target_start = 0 - RECTW(panel_rc);
        target_end = 0;
    }

    if ( NULL != src_piece_item ) {
        src_animation = _c(self)->movePieceWithAnimation(self, (mHotPiece*)src_item->content, 
                src_end, 
                src_piece_item->y, 
                NAVIGATIONPANELPIECE_ANIMATION_DURATION, OutCubic);
        mGEffAnimationAddToGroup(group, src_animation);
    }

    if ( NULL != target_piece_item ) {
        if (push) {
            HDC memdc;

            memdc = CreateCompatibleDCEx(HDC_SCREEN, 1, 1);
            _c(target_item->content)->paint(target_item->content, memdc,
                    (mObject *)_c((mPanelPiece *)target_item->content)->getOwner((mPanelPiece *)target_item->content),
                    0);
            DeleteMemDC(memdc);
        }
        _c(self)->movePiece(self, (mHotPiece*)target_item->content, 
                target_start, 
                target_piece_item->y);
        target_animation = _c(self)->movePieceWithAnimation(self, (mHotPiece*)target_item->content, 
                target_end, 
                target_piece_item->y, 
                NAVIGATIONPANELPIECE_ANIMATION_DURATION, OutCubic);
        /* mGEffAnimationSetProperty(target_animation, MGEFF_PROP_DEBUG, 1); */
        mGEffAnimationAddToGroup(group, target_animation);
    }
    
    return group;
}


static BOOL bar_left_button_clicked_handler (mHotPiece *self,
         mHotPiece *sender, int event_id, DWORD param)
{
    mNavigationPanelPiece* panel = (mNavigationPanelPiece*)self;
    
    printf("navigation ==> pop item\n");

    _c(panel)->pop(panel);

    return TRUE;
}


static mButtonPanelPiece* create_default_left_button (mNavigationPanelPiece* self, 
        mNavigationItem* item, const char* title)
{
    mShapeTransRoundPiece* bk_piece;
    mButtonPanelPiece* button_piece = (mButtonPanelPiece*)_c(item)->createDefaultButton(item, title, 0);

    if ( NULL == button_piece )
        return NULL;

    /* set the left button style */
    AdjustNavigationItemRectWithTitle((mHotPiece*)button_piece, title, item->default_button_font);
    bk_piece = _c(button_piece)->getBkgndPiece(button_piece);
    _c(bk_piece)->setProperty(bk_piece, NCSP_TRANROUND_SHARPFLAG, TRANROUND_SHARPFLAG_LEFT);

    /* set callback functions */
    ncsAddEventListener((mObject*)button_piece, (mObject*)self,
        (NCS_CB_ONPIECEEVENT)bar_left_button_clicked_handler, NCSN_ABP_CLICKED);

    return button_piece;
}


static mTextPiece* create_default_title_button (mNavigationPanelPiece* self, const char* title)
{
    RECT rect;
    mTextPiece* text_piece;
    
    /* create text_piece */
    text_piece = (mTextPiece*)NEWPIECE(mTextPiece);

    /* set property */
    SetRect(&rect, 0, 0, 10, NAVIGATIONBAR_DEFAULT_BUTTON_H);
    _c(text_piece)->setRect(text_piece, &rect);
    AdjustNavigationItemRectWithTitle((mHotPiece*)text_piece, title, self->default_title_font);
    _c(text_piece)->setProperty(text_piece, NCSP_TEXTPIECE_LOGFONT, (DWORD)self->default_title_font);
    _c(text_piece)->setProperty(text_piece, NCSP_LABELPIECE_LABEL, (DWORD)title);
    _c(text_piece)->setProperty(text_piece, NCSP_TEXTPIECE_TEXTCOLOR,(DWORD)NAVIGATIONBAR_DEFAULT_FONT_COLOR);
    _c(text_piece)->setProperty(text_piece, NCSP_TEXTPIECE_TEXTSHADOWCOLOR,(DWORD)NAVIGATIONBAR_DEFAULT_FONT_SHADOW_COLOR);

    return text_piece;
}


/* just remove the piece from PanelPiece, not real delete it */
#if 0
static BOOL cancel_content (mPanelPiece* self, mHotPiece* piece)
{
    mItemIterator *iter = _c(self->itemManager)->createItemIterator(self->itemManager);
    mPieceItem *item;
    while ((item = _c(iter)->next(iter))) {
        if (item->piece == piece) {
            if (self->hovering_piece == piece) {
                self->hovering_piece = NULL;
            }
            _c(self->itemManager)->removeItem(self->itemManager, item);
            return TRUE;
        }
    }
    DELETE(iter);

    if (self->layout) {
        _c(self)->reLayout(self);
    }
    return FALSE;
}
#endif


/* make the navigation bar topmost */
static void move_bar_topmost (mNavigationPanelPiece* self)
{
    /* remove the bar from the list */
    //cancel_content((mPanelPiece*)self, (mHotPiece*)self->bar);
    _c(self)->delContent(self, (mHotPiece*)self->bar);

    /* re-add the bar to the list tail */
    _c(self)->addContent(self, (mHotPiece*)self->bar, 0, 0);
}


/* do update navigation view and bar */
static void enter_update_navigation (mNavigationPanelPiece* self, 
        mNavigationItem* current_item, mNavigationItem* prev_item)
{
    int alpha = 255;
    int content_y = 0;
    char* title = NULL;
    char* prev_title = NULL;
    RECT  rect;
    mNavigationBarPiece* bar = self->bar;
    mButtonPanelPiece* left_button = NULL;
    mTextPiece* title_button = NULL;
    mButtonPanelPiece* right_button = NULL;
    mTextPiece* text_piece = NULL;
    mPieceItem* piece_item = NULL;


    /* configure the bar background */
    if ( NULL != current_item->background ) {
        _c(bar)->setBkgndPiece(bar, current_item->background);
        bar->background = current_item->background;
    }


    /* configure the title button */
    title_button = (mTextPiece*)_c(current_item)->getProperty(current_item, NCSP_NAVIGATIONITEM_TITLE_BUTTON);
    title = (char*)_c(current_item)->getProperty(current_item, NCSP_NAVIGATIONITEM_TITLE);
    if ( NULL == title_button ) {
        title_button = create_default_title_button(self, title);
        _c(current_item)->setProperty(current_item, NCSP_NAVIGATIONITEM_TITLE_BUTTON, (DWORD)title_button);
    }
    else {
        _c(title_button)->setProperty(title_button, NCSP_LABELPIECE_LABEL, (DWORD)title);
        AdjustNavigationItemRectWithTitle((mHotPiece*)title_button, title, self->default_title_font);
    }

    _c(title_button)->getRect(title_button, &rect); 
    piece_item = _c(bar)->searchItem(bar, (mHotPiece*)title_button); 
    if ( NULL == piece_item )
        _c(bar)->addContent(bar, (mHotPiece*)title_button, 
                NAVIGATIONBAR_TITLE_X(RECTW(rect)), NAVIGATIONBAR_DEFAULT_BUTTON_Y);
    else
        _c(bar)->movePiece(bar, (mHotPiece*)title_button, 
                NAVIGATIONBAR_TITLE_X(RECTW(rect)), NAVIGATIONBAR_DEFAULT_BUTTON_Y);
    bar->title_button = title_button;


    /* configure the left button */
    left_button = (mButtonPanelPiece*)_c(current_item)->getProperty(current_item, NCSP_NAVIGATIONITEM_LEFT_BUTTON);
    if ( NULL == left_button && FALSE == is_root_item(self, current_item) ) {
        prev_title = (char*)_c(prev_item)->getProperty(prev_item, NCSP_NAVIGATIONITEM_TITLE);
        left_button = (mButtonPanelPiece*)create_default_left_button(self, current_item, (const char*)prev_title);
        _c(current_item)->setProperty(current_item, NCSP_NAVIGATIONITEM_LEFT_BUTTON, (DWORD)left_button);
    }
    else if ( FALSE == is_root_item(self, current_item) ) {
        prev_title = (char*)_c(prev_item)->getProperty(prev_item, NCSP_NAVIGATIONITEM_TITLE);
        text_piece = ((m3DButtonPiece*)left_button)->txtPiece;
        _c(text_piece)->setProperty(text_piece, NCSP_LABELPIECE_LABEL, (DWORD)prev_title);
        AdjustNavigationItemRectWithTitle((mHotPiece*)left_button, prev_title, current_item->default_button_font);
    }

    if ( NULL != left_button ) {
        piece_item = _c(bar)->searchItem(bar, (mHotPiece*)left_button); 
        if ( NULL == piece_item )
            _c(bar)->addContent(bar, (mHotPiece*)left_button, 
                    NAVIGATIONBAR_HORIZONAL_W, NAVIGATIONBAR_DEFAULT_BUTTON_Y);
        else
            _c(bar)->movePiece(bar, (mHotPiece*)left_button, 
                    NAVIGATIONBAR_HORIZONAL_W, NAVIGATIONBAR_DEFAULT_BUTTON_Y);
    }
    bar->left_button = left_button;
    
    
    /* configure the right button */
    right_button = (mButtonPanelPiece*)_c(current_item)->getProperty(current_item, NCSP_NAVIGATIONITEM_RIGHT_BUTTON);
    if ( NULL != right_button ) {
        _c(right_button)->getRect(right_button, &rect); 
        piece_item = _c(bar)->searchItem(bar, (mHotPiece*)right_button); 
        if ( NULL == piece_item )
            _c(bar)->addContent(bar, (mHotPiece*)right_button, 
                    NAVIGATIONBAR_RIGHT_X(RECTW(rect)), NAVIGATIONBAR_DEFAULT_BUTTON_Y);
        else
            _c(bar)->movePiece(bar, (mHotPiece*)right_button, 
                    NAVIGATIONBAR_RIGHT_X(RECTW(rect)), NAVIGATIONBAR_DEFAULT_BUTTON_Y);
    }
    bar->right_button = right_button;


    /* configure the bar */
    switch ( current_item->style ) {
        case NAVIGATION_STYLE_FULLSCREEN:
            _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0);
            alpha = NAVIGATIONPANELPIECE_TRANSPARENT;
            content_y = 0;
            break;

        case NAVIGATION_STYLE_HIDE_BAR:
            _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0 - NAVIGATIONBAR_H);
            alpha = NAVIGATIONPANELPIECE_TRANSPARENT;
            content_y = 0;
            break;

        case NAVIGATION_STYLE_NORMAL:
        default:
            _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0);
            alpha = NAVIGATIONPANELPIECE_OPAQUE;
            content_y = NAVIGATIONBAR_H;
            break;
    }


    /* configure the view(content) */
    piece_item = _c(self)->searchItem(self, (mHotPiece*)current_item->content); 
    if ( NULL == piece_item ) {
        _c(self)->addContent(self, current_item->content, 0, content_y);
        move_bar_topmost(self);
    }
    else {
        _c(self)->movePiece(self, (mHotPiece*)current_item->content, 0, content_y);
    }

    _c(self)->setPieceAlpha(self, (mHotPiece*)bar, alpha);
}


/* cleanup update navigation view and bar */
static void leave_update_navigation (mNavigationPanelPiece* self, mNavigationItem* old_item)
{
    mNavigationBarPiece* bar = self->bar;

    /* remove the old item from the bar */
    if ( NULL != old_item->bar_title_button) {
        //cancel_content((mPanelPiece*)bar, (mHotPiece*)old_item->bar_title_button);
        _c(bar)->delContent(bar, (mHotPiece*)old_item->bar_title_button);
    }

    if ( NULL != old_item->bar_left_button) {
        //cancel_content((mPanelPiece*)bar, (mHotPiece*)old_item->bar_left_button);
        _c(bar)->delContent(bar, (mHotPiece*)old_item->bar_left_button);
    }

    if ( NULL != old_item->bar_right_button) {
        //cancel_content((mPanelPiece*)bar, (mHotPiece*)old_item->bar_right_button);
        _c(bar)->delContent(bar, (mHotPiece*)old_item->bar_right_button);
    }

    /* don't remove the content from panel peice, the tableview piece need it, 
     * for content parent. */
    if ( NULL != old_item->content ) {
        //cancel_content((mPanelPiece*)self, (mHotPiece*)old_item->content);
        _c(self)->delContent(self, (mHotPiece*)old_item->content);
    }

    /* let panel piece update */
    if (_c(self)->getOwner(self)) {
        PanelPiece_invalidatePiece((mHotPiece*)self, NULL);
    }
}


/* create a navigation with root item */
mNavigationPanelPiece* CreateNavigationPanelPieceWithRootView (mNavigationItem *rootItem)
{
    item_list_t* item = NULL;
    mNavigationPanelPiece* navigation_panel_piece = NULL;

    if ( NULL == rootItem )
        return NULL;
    
    /* create NavigationPanelPiece */
    navigation_panel_piece = (mNavigationPanelPiece*)NEWPIECE(mNavigationPanelPiece);
    
    /* push the rootItem to the list(stack) */
    item = (item_list_t*)calloc(1, sizeof(item_list_t));
    item->item = rootItem;
    list_add(&item->list, &navigation_panel_piece->item_head);

    /* add root item referen */
    ADDREF(rootItem);
    
    /* update the UI */
    enter_update_navigation(navigation_panel_piece, rootItem, NULL);
    
    return navigation_panel_piece;    
}


static void mNavigationPanelPiece_construct (mNavigationPanelPiece *self, DWORD param)
{
    RECT rect;

	Class(mPanelPiece).construct((mPanelPiece*)self, param);

    self->default_title_font = CreateLogFont ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK, 
            FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            NAVIGATIONBAR_DEFAULT_TITLE_FONT_SIZE, 0);
   
    /* init list(stack) */
    INIT_LIST_HEAD(&self->item_head);

    /* create navigation bar piece */
    self->bar = (mNavigationBarPiece*)NEWPIECE(mNavigationBarPiece);
    SetRect(&rect, 0, 0, NAVIGATIONBAR_W, NAVIGATIONBAR_H);
    _c(self->bar)->setRect(self->bar, &rect);
    ADDREFPIECE(self->bar);
    _c(self)->addContent(self, (mHotPiece*)self->bar, 0, 0);
}


static void mNavigationPanelPiece_destroy (mNavigationPanelPiece* self, DWORD param)
{
    item_list_t* item_list = NULL;
    mNavigationItem* item = get_current_item(self);
    
    /* delete the last item from NavigationPanelPiece */
    if ( NULL != item ) {
        leave_update_navigation(self, item);
    }

    /* delete the bar */
    _c(self)->delContent(self, (mHotPiece*)self->bar);
    UNREFPIECE(self->bar);
    DestroyLogFont(self->default_title_font);

    /* release the list(stack) */
    while ( !list_empty(&self->item_head) ) {
        item_list = (item_list_t*)self->item_head.next;
        item = item_list->item;
        UNREF(item);
        list_del(self->item_head.next);
        free(item_list);
    }
    
    Class(mPanelPiece).destroy((mPanelPiece*)self);
}


static BOOL mNavigationPanelPiece_setProperty (mNavigationPanelPiece* self, int id, DWORD value)
{
    if ( NULL == self )
        return FALSE;

    switch (id) {
        case NCSP_NAVIGATIONPANELPIECE_BAR:
            return TRUE;

        case NCSP_NAVIGATIONPANELPIECE_BAR_BKG:
            if ( NULL == self->bar )
                return FALSE;
            _c(self->bar)->setBkgndPiece(self->bar, (mShapeTransRoundPiece*)value);
            break;

        default:
            return Class(mPanelPiece).setProperty((mPanelPiece*)self, id, value);
    }

    return TRUE;
}


static DWORD mNavigationPanelPiece_getProperty (mNavigationPanelPiece* self, int id)
{
    if ( NULL == self )
        return (DWORD)NULL;

	switch (id) {
        case NCSP_NAVIGATIONPANELPIECE_BAR:
            return (DWORD)self->bar;

        case NCSP_NAVIGATIONPANELPIECE_BAR_BKG:
            if ( NULL == self->bar )
                return (DWORD)NULL;
            return (DWORD)_c(self->bar)->getBkgndPiece(self->bar);

        default:
            break;
	}

	return Class(mPanelPiece).getProperty((mPanelPiece*)self, id);
}


static void mNavigationPanelPiece_push (mNavigationPanelPiece* self, mNavigationItem* item)
{
    item_list_t* new = NULL;
    MGEFF_ANIMATION group;
    MGEFF_ANIMATION bar_animation;
    MGEFF_ANIMATION view_animation;
    mNavigationItem* prev_item = NULL;

    if ( NULL == self || NULL == item)
        return;
    
    ADDREF(item);
    /* save the old item */
    prev_item = get_current_item(self);
    
    /* push the new item */
    new = (item_list_t*)calloc(1, sizeof(item_list_t));
    new->item = item;
    list_add(&new->list, &self->item_head);
    
    /* update the UI */
    enter_update_navigation(self, item, prev_item);
    
    /* create change animation */
    group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    bar_animation = create_bar_animation(self, prev_item, item, TRUE);
    view_animation = create_view_animation(self, prev_item, item, TRUE);
    
    /* play the change animation(sync) */
    mGEffAnimationAddToGroup(group, bar_animation);
    mGEffAnimationAddToGroup(group, view_animation);
    _c(self)->animationSyncRunAndDelete(self, group);

    /* adjust the bar to the right position */
    if ( NAVIGATION_STYLE_HIDE_BAR == item->style )
        _c(self)->movePiece(self, (mHotPiece*)self->bar, 0, 0 - NAVIGATIONBAR_H);
    
    /* cleanup the old item */
    leave_update_navigation(self, prev_item);
}


static void mNavigationPanelPiece_pop (mNavigationPanelPiece* self)
{
    item_list_t* item = NULL;
    MGEFF_ANIMATION group;
    MGEFF_ANIMATION bar_animation;
    MGEFF_ANIMATION view_animation;
    mNavigationItem* old_item = NULL;
    mNavigationItem* current_item = NULL;
    mNavigationItem* prev_item = NULL;

    if ( NULL == self )
        return;
    
    /* save the old item */
    old_item = get_current_item(self);
    
    /* pop the old item */
    item = (item_list_t*)self->item_head.next;
    list_del(self->item_head.next);
    free(item);
    
    /* save the new */
    current_item = get_current_item(self);
    prev_item = get_next_item(self);
    
    /* update the UI */
    enter_update_navigation(self, current_item, prev_item);
    
    /* create change animation */
    group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
    bar_animation = create_bar_animation(self, old_item, current_item, FALSE);
    view_animation = create_view_animation(self, old_item, current_item, FALSE);
     
    /* play the animation(sync) */
    mGEffAnimationAddToGroup(group, bar_animation);
    mGEffAnimationAddToGroup(group, view_animation);
    _c(self)->animationSyncRunAndDelete(self, group);

    /* adjust the bar to the right position */
    if ( NAVIGATION_STYLE_HIDE_BAR == current_item->style )
        _c(self)->movePiece(self, (mHotPiece*)self->bar, 0, 0 - NAVIGATIONBAR_H);
    
    /* cleanup the old item */
    leave_update_navigation(self, old_item);
    UNREF(old_item);
}


static void mNavigationPanelPiece_showNavigationBar (mNavigationPanelPiece* self, BOOL show, BOOL hasAnimation)
{
    MGEFF_ANIMATION animation;
    mNavigationItem* current_item = NULL;
    mNavigationBarPiece* bar = self->bar;

    current_item = get_current_item(self);

    /* only the fullscreen mode can change the bar show mode. */
    if ( NULL == current_item || NAVIGATION_STYLE_NORMAL == current_item->style )
        return;

    /* hide the bar */
    if ( FALSE == show && NAVIGATION_STYLE_FULLSCREEN == current_item->style ) {
        if ( TRUE == hasAnimation) {
            _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0);
            _c(self)->setPieceAlpha(self, (mHotPiece*)bar, NAVIGATIONPANELPIECE_TRANSPARENT); 
    
            animation = _c(self)->setPieceAlphaWithAnimation(self, (mHotPiece*)bar, 
                    NAVIGATIONPANELPIECE_NOVISIBLE, NAVIGATIONPANELPIECE_ANIMATION_DURATION, InQuad);
            _c(self)->animationSyncRunAndDelete(self, animation);
        }
    
        _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0 - NAVIGATIONBAR_H);
        _c(self)->setPieceAlpha(self, (mHotPiece*)bar, NAVIGATIONPANELPIECE_NOVISIBLE); 
        current_item->style = NAVIGATION_STYLE_HIDE_BAR; 
    }
    /* show the bar */
    else if ( TRUE == show && NAVIGATION_STYLE_HIDE_BAR == current_item->style ) {
        if ( TRUE == hasAnimation) {
            _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0);
            _c(self)->setPieceAlpha(self, (mHotPiece*)bar, NAVIGATIONPANELPIECE_NOVISIBLE); 
    
            animation = _c(self)->setPieceAlphaWithAnimation(self, (mHotPiece*)bar, 
                    NAVIGATIONPANELPIECE_TRANSPARENT, NAVIGATIONPANELPIECE_ANIMATION_DURATION, InQuad);
            _c(self)->animationSyncRunAndDelete(self, animation);
        }

        _c(self)->movePiece(self, (mHotPiece*)bar, 0, 0);
        _c(self)->setPieceAlpha(self, (mHotPiece*)bar, NAVIGATIONPANELPIECE_TRANSPARENT); 
        current_item->style = NAVIGATION_STYLE_FULLSCREEN; 
    }
}


static BOOL mNavigationPanelPiece_currentIsRoot (mNavigationPanelPiece* self)
{
    return is_root_item(self, get_current_item(self));
}


BEGIN_MINI_CLASS(mNavigationPanelPiece, mPanelPiece)
	CLASS_METHOD_MAP(mNavigationPanelPiece, construct)
	CLASS_METHOD_MAP(mNavigationPanelPiece, destroy)
	CLASS_METHOD_MAP(mNavigationPanelPiece, setProperty)
	CLASS_METHOD_MAP(mNavigationPanelPiece, getProperty)
	CLASS_METHOD_MAP(mNavigationPanelPiece, push)
	CLASS_METHOD_MAP(mNavigationPanelPiece, pop)
	CLASS_METHOD_MAP(mNavigationPanelPiece, showNavigationBar)
	CLASS_METHOD_MAP(mNavigationPanelPiece, currentIsRoot)
END_MINI_CLASS

