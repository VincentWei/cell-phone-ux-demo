#if 0
#   define LOG_TIME(prefix) printf("%s%s(%d) %06u\n", prefix, __FUNCTION__, __LINE__, ((unsigned int)times(NULL)) % 1000000u)
#else
#   define LOG_TIME(prefix) /* NULL */
#endif

#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mlayout_manager.h"
#include "mtextpiece.h"
#include "mlinevbox.h"
#include "mpanelpiece.h"
#include "mtransroundpiece.h"
#include "mscrollviewpiece.h"
#include "mwrappaintpiece.h"
#include "mexseparatorpiece.h"
#include "mitempiece.h"
#include "mtableviewpiece.h"

/* local function declaration */
static void _piece_anim_cb(MGEFF_ANIMATION handle, void *target, intptr_t id, void *value);
static BOOL onIndexLocatePieceTouched(mTableViewPiece *self,
				      mHotPiece *sender, int event_id, DWORD param);
static mPieceItem* nextItem(mTableViewPiece* self, mItemIterator* iter, mHotPiece* prev);

static int s_onContentMouseMove(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    mTableViewPiece *self = (mTableViewPiece*)(_self->parent);
    {
        mItemIterator* section_iter =
            _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);
        mPanelPiece* sectionPiece = NULL;
        mPieceItem* item = NULL;

        assert(NULL != section_iter);
        while (NULL != (item = _c(section_iter)->next(section_iter))) {
            mItemIterator* row_iter;
            sectionPiece = SAFE_CAST(mPanelPiece, _c(item)->getPiece(item));
            assert(NULL != sectionPiece);

            row_iter = 
                _c(sectionPiece->itemManager)->createItemIterator(sectionPiece->itemManager);
            while(NULL != (item = _c(row_iter)->next(row_iter))) {
                mTableViewItemPiece* piece = (mTableViewItemPiece*)_c(item)->getPiece(item);
                if (_c(item)->getType(item) == NCS_TABLEVIEW_NORMALROWTYPE){
                    _c(piece)->setHighlight(piece, FALSE);
                }
            }
            DELETE(row_iter);
        }

        DELETE(section_iter);
    }
    return -1;
}

static int s_onContentMousePress(mHotPiece *_self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    mIndexPath indexpath;
    mTableViewItemPiece* clickPiece = NULL;
    mTableViewPiece *self = (mTableViewPiece*)(_self->parent);
    mPieceItem* tableItem = _c(self)->searchItem(self, (mHotPiece*)self->tablePanel);
    if (tableItem) {
        int x = LOSWORD(lParam); 
        int y = HISWORD(lParam);
        mPanelPiece* hotGroup = (mPanelPiece*)_c(self->tablePanel)->childAt(self->tablePanel, 
                x, y);
        /*
        fprintf(stderr, "current(%d,%d), item x=%d,y=%d.\n", 
                LOSWORD(lParam), HISWORD(lParam),
                _c(tableItem)->getX(tableItem),_c(tableItem)->getY(tableItem));
        */

        if (hotGroup) {
            mPieceItem* groupItem = _c(self->tablePanel)->searchItem(self->tablePanel, (mHotPiece*)hotGroup);
            mTableViewItemPiece* hotPiece = (mTableViewItemPiece*)_c(hotGroup)->childAt(hotGroup, 
                    x - _c(groupItem)->getX(groupItem),
                    y - _c(groupItem)->getY(groupItem));
            mPieceItem* item = _c(hotGroup)->searchItem(hotGroup, (mHotPiece*)hotPiece);

            if (item && _c(item)->getType(item) == NCS_TABLEVIEW_NORMALROWTYPE) {
                mTableViewItemPiece* piece = (mTableViewItemPiece*)_c(item)->getPiece(item);
                _c(self)->itemToIndexPath(self, piece, &indexpath);
                piece->highlight =_c(self)->willSelectRowAtIndexPath(self, &indexpath);
                clickPiece = piece;
                fprintf(stderr, "click down At item indexpath(%d,%d), hightlight=%d, piece=%p, (x,y)=(%d,%d)\n", 
                        indexpath.section, indexpath.row,
                        piece->highlight, piece,
                        item->x, item->y);
            }
        }
    }
    /* resetEditMode.*/
    {
        mItemIterator* section_iter =
            _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);
        mPanelPiece* sectionPiece = NULL;
        mPieceItem* item = NULL;

        assert(NULL != section_iter);
        while (NULL != (item = _c(section_iter)->next(section_iter))) {
            mItemIterator* row_iter;
            sectionPiece = SAFE_CAST(mPanelPiece, _c(item)->getPiece(item));
            assert(NULL != sectionPiece);

            row_iter = 
                _c(sectionPiece->itemManager)->createItemIterator(sectionPiece->itemManager);
            while(NULL != (item = _c(row_iter)->next(row_iter))) {
                mTableViewItemPiece* piece = (mTableViewItemPiece*)_c(item)->getPiece(item);
                if (piece != clickPiece 
                        && _c(item)->getType(item) == NCS_TABLEVIEW_NORMALROWTYPE){
                    _c(piece)->resetEditMode(piece);
                }
            }
            DELETE(row_iter);
        }

        DELETE(section_iter);
    }
    return -1;
}

static int deleteItem(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
    assert(message == MSG_TABLEVIEWPIECE_DELITEM);
    {
        mPieceItem* item;
        mPieceItem* item2;
        mTableViewPiece* table = (mTableViewPiece*)wParam;
        mHotPiece* sender = (mHotPiece*)lParam;
        mItemIterator* iter = _c(table->tablePanel->itemManager)->createItemIterator(table->tablePanel->itemManager);

        while((item = _c(iter)->next(iter))) {
            mPanelPiece* group = (mPanelPiece*)_c(item)->getPiece(item);
            mItemIterator* iter_group;

            assert(INSTANCEOF(group, mPanelPiece));

            iter_group = _c(group->itemManager)->createItemIterator(group->itemManager);

            while((item2 = _c(iter_group)->next(iter_group))){
                mTableViewItemPiece* piece = (mTableViewItemPiece*)_c(item2)->getPiece(item2);
                if ((mHotPiece*)piece == sender) {
                    if (_c(item2)->getType(item2) == NCS_TABLEVIEW_NORMALROWTYPE) {
                        mIndexPath indexpath = {-1, -1};
                        if (table->focusPiece == piece) table->focusPiece = NULL;
                        //_c(table)->removeItem(table, (char*)item, TRUE);
                        _c(table)->itemToIndexPath(table, piece, &indexpath);
                        _c(table)->onCommitDeleteRowAtIndexPath(table, &indexpath);
                        fprintf(stderr, "%s:delete piece is %p, indexpath(%d,%d)\n", __FUNCTION__, 
                                piece, indexpath.section, indexpath.row);
                        printf("table %p -- NCSN_TABLEVIEWITEMPIECE_DELETECLICKED\n", table);
                        break;
                    }
                }
            }
            DELETE(iter_group);
        }
        DELETE(iter);
    }
    return 0;
}


static BOOL onDeletePieceClicked(mTableViewPiece *self, mHotPiece *sender, int event_id, DWORD param)
{
    mWidget* _w = _c(self)->getOwner(self);

    assert(event_id == NCSN_TABLEVIEWITEMPIECE_DELBTNCLICKED);

    ncsSetComponentHandler((mComponent*)_w, MSG_TABLEVIEWPIECE_DELITEM, deleteItem);
    PostMessage(_w->hwnd, MSG_TABLEVIEWPIECE_DELITEM, (WPARAM)self, (LPARAM)sender);

    return TRUE;
}

static BOOL onContentPieceClicked(mTableViewPiece *self, mHotPiece* sender, int event_id, DWORD param)
{
    mPieceItem* item, *item2;
    mItemIterator* iter = _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);

    assert(event_id == NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED);

    while((item = _c(iter)->next(iter))) {
        mPanelPiece* group = (mPanelPiece*)_c(item)->getPiece(item);
        mItemIterator* iter_group;
        assert(INSTANCEOF(group, mPanelPiece));

        iter_group = _c(group->itemManager)->createItemIterator(group->itemManager);
        while((item2 = _c(iter_group)->next(iter_group))) {
            mTableViewItemPiece* piece = (mTableViewItemPiece*)_c(item2)->getPiece(item2);
            if (_c(item2)->getType(item2) == NCS_TABLEVIEW_NORMALROWTYPE){
                _c(piece)->resetEditMode(piece);
                if (piece == (mTableViewItemPiece*)sender) {
                    mIndexPath indexpath = {-1, -1};
                    _c(self)->itemToIndexPath(self, piece, &indexpath);
                    _c(self)->rowDidSelectAtIndexPath(self, &indexpath);
                    PanelPiece_update((mHotPiece*)self, TRUE);
                    printf("self %p -- NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED\n", self);
                }
            }
        }
        DELETE(iter_group);
    }
    DELETE(iter);
    return TRUE;
}

static void mTableViewPiece_construct(mTableViewPiece *self, DWORD add_data)
{
    Class(mScrollViewPiece).construct((mScrollViewPiece*)self, add_data);
    
    //self->backgroundPiece  = (mHotPiece*)createBackgroundPiece(self);

    self->style = add_data;
    if (self->style == NCS_TABLEVIEW_INDEXLOCATE_STYLE) {
        self->indexLocate= (mIndexLocatePiece*)NEWPIECEEX(mIndexLocatePiece, (DWORD)self);
        _c(self)->showScrollBar(self, FALSE);

        ncsAddEventListener((mObject*)self->indexLocate, (mObject*)self,
                (NCS_CB_ONPIECEEVENT)onIndexLocatePieceTouched, NCSN_INDEXLOCATEPIECE_TOUCHED);
    }

    self->tablePanel = NEWPIECE(mPanelPiece);
    if (self->tablePanel) {
        mLineVBox* box = (mLineVBox*)NEW(mLineVBox);
        _c(self->tablePanel)->setLayoutManager(self->tablePanel, (mLayoutManager*)box);
        _c(self)->addContent(self, (mHotPiece*)self->tablePanel, 0, 0);
        if (self->style == NCS_TABLEVIEW_GROUP_STYLE) {   
            _c(box)->setGap(box, NCS_TABLEVIEW_GROUPGAP);
        }
        _c(self->tablePanel)->appendEventHandler(self->tablePanel, MSG_LBUTTONDOWN, s_onContentMousePress);
        //_c(self->tablePanel)->appendEventHandler(self->tablePanel, MSG_LBUTTONUP, s_onContentMouseUp);
        _c(self->tablePanel)->appendEventHandler(self->tablePanel, MSG_MOUSEMOVE, s_onContentMouseMove);
    }
    _c(self)->setSeparatorStyle(self, NCS_TABLEVIEW_SEPARATORSTYLE_SINGLINE);
    _c(self)->setSeparatorColor(self, NCS_TABLEVIEW_SEPARATOR_DEFAULTCOLOR);
    self->focusPiece = NULL;
}


static int mTableViewPiece_processMessage(mTableViewPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message == MSG_MOUSEMOVEIN 
            || (message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK)) {
        static BOOL indexFocus = FALSE;
        if (self->indexLocate) {
            RECT rc;
            int x = LOSWORD(lParam), y = HISWORD(lParam);
            _c(self->indexLocate)->getRect(self->indexLocate, &rc);
            OffsetRect(&rc, self->indexPos.x, self->indexPos.y);
            if (indexFocus || PtInRect(&rc, x, y)) {
                x -= self->indexPos.x;
                y -= self->indexPos.y;
                lParam = MAKELONG(x, y);
                //fprintf(stderr, "self->indexLocate get message:%d, x=%d,y=%d.\n", message, LOSWORD(lParam), HISWORD(lParam));
                if (message == MSG_LBUTTONDOWN) {
                    indexFocus = TRUE;
                } else if (message == MSG_MOUSEMOVEIN || message == MSG_LBUTTONUP){
                    indexFocus = FALSE;
                }
                return _c(self->indexLocate)->processMessage(self->indexLocate, message, wParam, lParam, owner);
            }
        }
    } 

    return Class(mScrollViewPiece).processMessage((mScrollViewPiece*)self, message, wParam, lParam, owner);
}


static void mTableViewPiece_changeMode(mTableViewPiece* self)
{
    /* set all tablepanel item to another mode.*/
    mItemIterator* section_iter =
        _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);
    mItemIterator* row_iter;
    mPanelPiece* sectionPiece = NULL;
    mPieceItem* item = NULL;

    while (NULL != (item = _c(section_iter)->next(section_iter))) {
        sectionPiece = SAFE_CAST(mPanelPiece, _c(item)->getPiece(item));
        assert(NULL != sectionPiece);

        row_iter = 
            _c(sectionPiece->itemManager)->createItemIterator(sectionPiece->itemManager);

        while(NULL != (item = _c(row_iter)->next(row_iter))) {
            mTableViewItemPiece* piece = (mTableViewItemPiece*)_c(item)->getPiece(item);
            if (_c(item)->getType(item) == NCS_TABLEVIEW_NORMALROWTYPE) {
                _c(piece)->changeMode(piece);
            }
        }

        DELETE(row_iter);
        row_iter = NULL;
    }
    _c(self)->invalidatePiece(self, (mHotPiece*)self->tablePanel, NULL, FALSE);

    DELETE(section_iter);
}

static mTableViewItemPiece* mTableViewPiece_indexPathToItem(mTableViewPiece* self, const mIndexPath* indexpath)
{
    mItemIterator* section_iter =
	_c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);
    mItemIterator* row_iter = NULL;
    int i = 0, j = 0;
    mPanelPiece* sectionPiece = NULL;
    mPieceItem* item = NULL;

    assert(NULL != indexpath);
    assert(NULL != section_iter);
    while (NULL != (item = _c(section_iter)->next(section_iter))) {
        sectionPiece = SAFE_CAST(mPanelPiece, _c(item)->getPiece(item));
        assert(NULL != sectionPiece);

        if (indexpath->section == i++) { // match section.
            mItemIterator* row_iter = 
                _c(sectionPiece->itemManager)->createItemIterator(sectionPiece->itemManager);
            while(NULL != (item = _c(row_iter)->next(row_iter))) {
                mTableViewItemPiece* piece = (mTableViewItemPiece*)_c(item)->getPiece(item);
                if (_c(item)->getType(item) == NCS_TABLEVIEW_NORMALROWTYPE){
                    if (indexpath->row == j++) { // match row.
                        DELETE(section_iter);
                        DELETE(row_iter);
                        return piece;
                    }
                }
            }
        }
    }

    DELETE(section_iter);
    DELETE(row_iter);
    return NULL;
}

static void mTableViewPiece_itemToIndexPath(mTableViewPiece* self, const mTableViewItemPiece* piece, mIndexPath* indexpath)
{
    int i = 0, j = 0;
    mPieceItem* item = NULL;
    mItemIterator* row_iter = NULL;
    mPanelPiece* sectionPiece = SAFE_CAST(mPanelPiece, piece->parent);
    mItemIterator* section_iter = _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);

    assert(NULL != sectionPiece);
    assert(NULL != indexpath);
    assert(NULL != piece);
    assert(NULL != section_iter);

    while (NULL != (item = _c(section_iter)->next(section_iter))) {
        if (sectionPiece == SAFE_CAST(mPanelPiece, _c(item)->getPiece(item))) {
            indexpath->section = i;
            break;
        }
        i++;
    }

    row_iter = _c(sectionPiece->itemManager)->createItemIterator(sectionPiece->itemManager);
    while(NULL != (item = _c(row_iter)->next(row_iter))) {
        mTableViewItemPiece* itemPiece = (mTableViewItemPiece*)_c(item)->getPiece(item);
        if (_c(item)->getType(item) == NCS_TABLEVIEW_NORMALROWTYPE){
            if (itemPiece == piece) { // match row.
                indexpath->row = j;
                break;
            }
            ++j;
        }
    }
    DELETE(row_iter);
    DELETE(section_iter);
}


static void mTableViewPiece_rectForHeaderInSection(mTableViewPiece* self, int section, RECT* rect)
{
    RECT rc;
    int i = 0;
    mPieceItem* item;
    mPieceItem* headeritem;
    mPieceItem* parentItem = NULL;
    mItemIterator *iter = _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);
    
    while((item = _c(iter)->next(iter))){
        if (i++ == section) {
            mPanelPiece* group = (mPanelPiece*)_c(item)->getPiece(item);
            mItemIterator *iter = _c(group->itemManager)->createItemIterator(group->itemManager);
            mHotPiece* piece;

            assert(INSTANCEOF(group, mPanelPiece));

            /* group first is headerpiece. */
            headeritem = _c(iter)->next(iter);
            piece = _c(headeritem)->getPiece(headeritem);
            _c(piece)->getRect(piece, &rc);

            parentItem = _c((mPanelPiece*)self->tablePanel)->searchItem(
                    (mPanelPiece*)self->tablePanel, (mHotPiece*)group);
            DELETE(iter);
            break;
        }
    }
    
    assert(parentItem);
    rect->left   = _c(parentItem)->getX(parentItem) /* + _c(item)->getX(item); */;
    rect->top    = _c(parentItem)->getY(parentItem) /* + _c(item)->getY(item); */;
    rect->right  = rect->left + RECTW(rc);
    rect->bottom = rect->top  + RECTH(rc);
    DELETE(iter);
}

static void mTableViewPiece_rectForRowAtIndexPath(mTableViewPiece* self, const mIndexPath* indexpath, RECT* rect)
{
    RECT rc;
    mHotPiece* piece;
    mPanelPiece* parent;
    mPieceItem* item;
    mPieceItem* parentItem;
    
    piece = (mHotPiece*) _c(self)->indexPathToItem(self, indexpath);

    printf("onIndexLocatePieceTouched piece: %p\n", piece);

    if (!piece)
        return;
    
    parent = (mPanelPiece*) piece->parent;
    parentItem = _c(self->tablePanel)->searchItem(self->tablePanel, (mHotPiece*)parent);
    item = _c(parent)->searchItem(parent, (mHotPiece*)piece);
    _c(piece)->getRect(piece, &rc);
    
    rect->left   = _c(parentItem)->getX(parentItem) + _c(item)->getX(item);
    rect->top    = _c(parentItem)->getY(parentItem) + _c(item)->getY(item);
    rect->right  = rect->left + RECTW(rc);
    rect->bottom = rect->top + RECTH(rc);
}

static void mTableViewPiece_rectForSection(mTableViewPiece* self, int section, RECT* rect)
{
    RECT rc;
    int i = 0;
    mPieceItem* item;
    mPieceItem* groupItem;
    mItemIterator *iter = _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);
    
    while((item = _c(iter)->next(iter))){
        if (i++ == section) {
            mPanelPiece* group = (mPanelPiece*)_c(item)->getPiece(item);
            groupItem = _c((mPanelPiece*)self->tablePanel)->searchItem(
                    (mPanelPiece*)self->tablePanel, (mHotPiece*)group);
            _c(group)->getRect(group, &rc);
            break;
        }
    }
    
    rect->left   = _c(groupItem)->getX(groupItem);
    rect->top    = _c(groupItem)->getY(groupItem);
    rect->right  = rect->left + RECTW(rc);
    rect->bottom = rect->top  + RECTH(rc);
    DELETE(iter);
}

#define NCS_TABLEVIEW_ROWHEIGHT 30
static void mTableViewPiece_getDefaultRowRect(mTableViewPiece* self, RECT* rect)
{
    _c(self)->getRect(self, rect);
    //fprintf(stderr, "%s:rect(%d,%d,%d,%d).\n", __FUNCTION__, rect->left, rect->top, 
    //        rect->right, rect->bottom);
    rect->left = rect->top = 0;
    rect->bottom = NCS_TABLEVIEW_ROWHEIGHT;
}

/* separator. */
static mPanelPiece* mTableViewPiece_createSeparatorLine(mTableViewPiece* self)
{
    if (self->separatorStyle == NCS_TABLEVIEW_SEPARATORSTYLE_SINGLINE) {
        RECT rc;
        mPanelPiece* panel= NEWPIECE(mPanelPiece);
        mExSeparatorPiece* piece = NEWPIECE(mExSeparatorPiece);
        int size = _c(piece)->getProperty(piece, NCSP_EXSEPARATOR_SIZE);

        _c(self)->getDefaultRowRect(self, &rc);
        rc.bottom = size;
        _c(piece)->setProperty(piece, NCSP_EXSEPARATOR_COLOR, self->separatorColor);
        _c(panel)->setRect(panel, &rc);
        _c(piece)->setRect(piece, &rc);  
        _c(panel)->addContent(panel, (mHotPiece*)piece, 0, 0);
        return panel;
    }
    else 
        return NULL;
}

static mPanelPiece* mTableViewPiece_createDefaultRow(mTableViewPiece* self, mTableViewItemPiece* item_piece)
{
    mPanelPiece* panel = NEWPIECE(mPanelPiece);
    static BITMAP bmp;
    mTextPiece* text;
    if (LoadBitmapFromFile(HDC_SCREEN, &bmp, item_piece->picture)) {
        mImagePiece* image = NEWPIECE(mImagePiece);
        _c(image)->setProperty(image, NCSP_IMAGEPIECE_IMAGE, (DWORD)&bmp);
        _c(panel)->addContentToLayout(panel, (mHotPiece*)image);
    }
    text = NEWPIECEEX(mTextPiece, (DWORD)item_piece->text);
    /* text logfont support.*/
    /*
    if (item_piece->textfont) {
        _c(text)->setProperty(text, NCSP_TEXTPIECE_LOGFONT);
    }
    */
    _c(panel)->addContentToLayout(panel, (mHotPiece*)text); 
    _c(panel)->addContentToLayout(panel, (mHotPiece*)item_piece->accessory);
    //_c(panel)->setLayout(panel, ...);

    return panel;
}

static void mTableViewPiece_reloadData(mTableViewPiece* self)
{
    int i = 0;
    mPanelPiece* section_piece = NULL;
    int section_num = _c(self)->numberOfSections(self);

    self->focusPiece = NULL;
    _c(self->tablePanel)->clearContents(self->tablePanel);
    for (i = 0; i < section_num ; i ++) {
        mPieceItem* item;
        if (self->style == NCS_TABLEVIEW_GROUP_STYLE) {
            section_piece = _c(self)->createGroupSectionContent(self, i);
        }
        else {    
            section_piece = _c(self)->createIndexSectionContent(self, i);        
        }
        item = _c(self->tablePanel)->addContentToLayout(self->tablePanel, (mHotPiece*)section_piece);
        _c(item)->setType(item, NCS_TABLEVIEW_SECTIONTYPE);
    }
    if (section_num > 0) {
        RECT rc;
        mLineVBox* vbox = (mLineVBox*)self->tablePanel->layout;
        mItemIterator *iter = _c(self->tablePanel->itemManager)->createItemIterator(self->tablePanel->itemManager);
        _c(self)->getDefaultRowRect(self, &rc);

        _c(vbox)->getLayoutRect(vbox, iter, &rc);
        _c(self->tablePanel)->setRect(self->tablePanel, &rc);
        DELETE(iter);
    }

    if (self->indexLocate) {
        RECT rc;
        _c(self)->getRect(self, &rc);
        self->indexPos.x = RECTW(rc) - NCS_TABLEVIEW_INDEXLOCATE_W;
        self->indexPos.y = NCS_TABLEVIEW_INDEXLOCATE_Y;
        rc.left  = 0;
        rc.right = NCS_TABLEVIEW_INDEXLOCATE_W;
        _c(self->indexLocate)->setRect(self->indexLocate, &rc);
        _c(self->indexLocate)->reloadData(self->indexLocate);
    }
    _c(self)->invalidatePiece(self, (mHotPiece*)self->tablePanel, NULL, FALSE);

    if (_c(self)->getOwner(self)) {
        PanelPiece_invalidatePiece((mHotPiece*)self, NULL);
    }
}

static mPanelPiece* mTableViewPiece_createIndexSectionHead(mTableViewPiece* self, int section)
{
    RECT rc;
    ARGB color;
    const char* string = _c(self)->titleForSection(self, section);
    if (strlen(string)) {
        mItemPiece *item = NEWPIECEEX(mItemPiece, (DWORD)0);
        _c(self)->getDefaultRowRect(self, &rc);
        _c(item)->setRect(item, &rc);
        color = 0xFF000000;
        _c(item)->setTitle(item, string, NULL, &color);
        _c(item)->setProperty(item, NCSP_ITEMPIECE_BKCOLOR, 0xFFD5D5F5);
        _c(item)->activeLayout(item);
#if 0
        mTextPiece* text = NEWPIECEEX(mTextPiece, (DWORD)string);
        mRectPiece* rect = NEWPIECE(mRectPiece);

        _c(self)->getDefaultRowRect(self, &rc);
        _c(text)->setRect(text, &rc);
        _c(rect)->setRect(rect, &rc);
        
        _c(text)->setProperty(text, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);
        _c(text)->setProperty(text, NCSP_LABELPIECE_VALIGN, NCS_VALIGN_CENTER);
        _c(text)->setProperty(text, NCSP_TEXTPIECE_TEXTCOLOR, 0xFF000000);
        _c(rect)->setProperty(rect , NCSP_RECTPIECE_FILLCOLOR, 0xFFF5D5D5);
        _c(rect)->setProperty(rect , NCSP_RECTPIECE_BORDERSIZE, 0x0);

        _c(panel)->addContent(panel, (mHotPiece*)rect, 0,  0);
        _c(panel)->addContent(panel, (mHotPiece*)text, 20, 0);
#endif

        return (mPanelPiece*)item;
    }
    else {
        return NULL;
    }
}

#if 0
static void s_outputRect(mTableViewPiece* self, RECT* rc, int section)
{
    fprintf(stderr, "section is %s, rows=%d, rect is (%d,%d,%d,%d).\n", 
            _c(self)->indexForSection(self, section),
            _c(self)->numberOfRowsInSection(self, section),
            rc->left, rc->top, rc->right, rc->bottom);
}
#endif

static mPanelPiece* mTableViewPiece_createIndexSectionContent(mTableViewPiece* self, int section)
{
    int i= 0;
    int row_num = _c(self)->numberOfRowsInSection(self, section);
    mPanelPiece* group = NEWPIECE(mPanelPiece);
    RECT rc;

    mPanelPiece* index = _c(self)->createIndexSectionHead(self, section);  
    mLineVBox* box = (mLineVBox*)NEW(mLineVBox);

    _c(group)->setLayoutManager(group, (mLayoutManager*)box);
    if (index) {
        mPieceItem* item = _c(group )->addContentToLayout(group, (mHotPiece*)index);
        _c(item)->setType(item, NCS_TABLEVIEW_TITLETYPE);
    }

    for (i = 0;  i < row_num; i++) {
        mIndexPath index = {section, i};
        mTableViewItemPiece* item_piece = _c(self)->createItemForRow(self, &index);
        assert(INSTANCEOF(item_piece, mTableViewItemPiece));
        {
            mPieceItem* item;
            int event_ids[] = {NCSN_TABLEVIEWITEMPIECE_DELBTNCLICKED, 0};
            if (!(_c(item_piece)->getUserPiece(item_piece))) {
                mPanelPiece* panel = _c(self)->createDefaultRow(self, item_piece);
                _c(item_piece)->setUserPiece(item_piece, (mHotPiece*)panel);
            }
            item = _c(group)->addContentToLayout(group, (mHotPiece*)item_piece);
            _c(item)->setType(item, NCS_TABLEVIEW_NORMALROWTYPE);

            /* add event. */
            ncsAddEventListeners((mObject*)item_piece, (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)onDeletePieceClicked, event_ids);
            event_ids[0] = NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED;
            ncsAddEventListeners((mObject*)item_piece, (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)onContentPieceClicked, event_ids);

            item_piece->highlight = _c(self)->willSelectRowAtIndexPath(self, &index);

            /* create separator. */
            if (i < row_num -1) {
                mPanelPiece* piece = _c(self)->createSeparatorLine(self);
                if (piece) {
                    item = _c(group)->addContentToLayout(group, (mHotPiece*)piece); 
                    _c(item)->setType(item, NCS_TABLEVIEW_SEPARATORTYPE);
                }
            }
        }
    }

    if (row_num > 0) {
        mLineVBox* vbox = (mLineVBox*)group->layout;
        mItemIterator *iter = _c(group->itemManager)->createItemIterator(group->itemManager);
        _c(self)->getDefaultRowRect(self, &rc);

        _c(vbox)->getLayoutRect(vbox, iter, &rc);
        _c(group)->setRect(group, &rc);
        DELETE(iter);
        //s_outputRect(self, &rc, section);
    }
    return group;
}

static mPanelPiece* mTableViewPiece_createGroupSectionContent(mTableViewPiece* self, int section)
{
    RECT rc;
    int i= 0;
    int row_num = _c(self)->numberOfRowsInSection(self, section);
    mPanelPiece* group = NEWPIECE(mPanelPiece);
    mLineVBox* box = (mLineVBox*)NEW(mLineVBox);
    _c(group)->setLayoutManager(group, (mLayoutManager*)box);

    for (i= 0;  i < row_num; i++) {
        mIndexPath index = {section, i};
        mTableViewItemPiece* item_piece = _c(self)->createItemForRow(self, &index);
        assert(INSTANCEOF(item_piece, mTableViewItemPiece));
        {
            mPieceItem* item;
            int event_ids[] = {NCSN_TABLEVIEWITEMPIECE_DELBTNCLICKED, 0};

            if (!(_c(item_piece)->getUserPiece(item_piece))) {
                mPanelPiece* panel = _c(self)->createDefaultRow(self, item_piece);
                _c(item_piece)->setUserPiece(item_piece, (mHotPiece*)panel);
            }
            item = _c(group)->addContentToLayout(group, (mHotPiece*)item_piece);
            _c(item)->setType(item, NCS_TABLEVIEW_NORMALROWTYPE);

            /* add event. */
            ncsAddEventListeners((mObject*)item_piece, (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)onDeletePieceClicked, event_ids);
            event_ids[0] = NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED;
            ncsAddEventListeners((mObject*)item_piece, (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)onContentPieceClicked, event_ids);

            item_piece->highlight = _c(self)->willSelectRowAtIndexPath(self, &index);

            /* create separator. */
            if (i < row_num -1) {
                mPanelPiece* piece = _c(self)->createSeparatorLine(self);
                if (piece) {
                    item = _c(group)->addContentToLayout(group, (mHotPiece*)piece); 
                    _c(item)->setType(item, NCS_TABLEVIEW_SEPARATORTYPE);
                }
            }
        }
    }
    if (row_num > 0) {
        mLineVBox* vbox = (mLineVBox*)group->layout;
        mItemIterator *iter = _c(group->itemManager)->createItemIterator(group->itemManager);
        mShapeTransRoundPiece* shape = NEWPIECE(mShapeTransRoundPiece);
        _c(group)->setBkgndPiece(group, shape);
        _c(shape)->setProperty(shape, NCSP_TRANROUND_BKCOLOR, 0xFFFF0000);

        _c(vbox)->getLayoutRect(vbox, iter, &rc);
        _c(group)->setRect(group, &rc);
        _c(shape)->setRect(shape, &rc);
        DELETE(iter);
    }

    return group;
}

static void mTableViewPiece_destroy(mTableViewPiece *self)
{
    if (self->indexLocate) {
        DELPIECE(self->indexLocate);
    }

    Class(mScrollViewPiece).destroy((mScrollViewPiece*)self);
}

static void mTableViewPiece_insertRowAtIndexPath(
    mTableViewPiece* self, const mIndexPath* indexpath, TableViewAnimType animtype)
{

}

static void mTableViewPiece_deleteRowAtIndexPath(
    mTableViewPiece* self, const mIndexPath* indexpath, TableViewAnimType animtype)
{
    int v_s = 0;
    int v_e = 0;
    RECT rc;
    mHotPiece* piece = (mHotPiece*) _c(self)->indexPathToItem(self, indexpath);
    mPanelPiece* group = (mPanelPiece*)piece->parent;
    mPieceItem* item = _c(group)->searchItem((mPanelPiece*)group, (mHotPiece*)piece);
    mWrapPaintPiece* wrap = NEWPIECEEX(mWrapPaintPiece, (DWORD)piece); /* use wrap piece to do deleting effect */
    MGEFF_ANIMATION  anim = mGEffAnimationCreate(wrap, _piece_anim_cb, 0, MGEFF_POINT);
    assert(anim);

    printf("mTableViewPiece_deleteRowAtIndexPath\n");
    _c(item)->setPiece(item, (mHotPiece*)wrap);
    _c(piece)->getRect(piece, &rc);
    _c(wrap)->setRect(wrap, &rc);
    v_s = RECTH(rc) ;
    v_e = 0;

    mGEffAnimationSetStartValue(anim, &v_s);
    mGEffAnimationSetEndValue(anim, &v_e);
    mGEffAnimationSetDuration(anim, 400);
    mGEffAnimationSetCurve(anim, InCubic);

    _c(self)->animationSyncRunAndDelete(self, anim);
    fprintf(stderr, "%s:after animation.\n", __FUNCTION__);
    _c(item)->setPiece(item, piece);

    /* delete separator */
    {
        mItemIterator* row_iter = 
            _c(group->itemManager)->createItemIterator(group->itemManager);
        mPieceItem* next = nextItem(self, row_iter, piece);
        if (next && _c(next)->getType(next) == NCS_TABLEVIEW_SEPARATORTYPE) {
            printf("delete separator\n");
            _c(group)->delContent(group, _c(next)->getPiece(next));
        }
        DELETE(row_iter);
    }


    /* delete row */
    _c(group)->delContent(group, piece);

    /* relayout & update panel */
    _c(group)->reLayout(group);
    _c(self->tablePanel)->reLayout(self->tablePanel);
    _c(self)->invalidatePiece(self, (mHotPiece*)self->tablePanel, NULL, FALSE);
    PanelPiece_invalidatePiece((mHotPiece*)self, NULL);

    DELPIECE(wrap);
}

static void mTableViewPiece_paint(mTableViewPiece* self, HDC hdc, mObject * owner, DWORD add_data)
{
    RECT containerRc;

    LOG_TIME(">> TableView: ");

    _c(self)->getRect(self, &containerRc);
    SelectClipRect(hdc, &containerRc);
    self->clipRect = containerRc;
    //fprintf(stderr, "clip rc(%d,%d,%d,%d).\n", containerRc.left, containerRc.top, 
    //        containerRc.right, containerRc.bottom);

    Class(mScrollViewPiece).paint((mScrollViewPiece*)self, hdc, owner, add_data);

    if (self->indexLocate) {
        HDC subdc;
        RECT rc;
        _c(self->indexLocate)->getRect(self->indexLocate, &rc);
        subdc = GetSubDC(hdc, self->indexPos.x, self->indexPos.y, RECTW(rc), RECTH(rc));
        _c(self->indexLocate)->paint(self->indexLocate, subdc, owner, add_data);
        ReleaseDC(subdc);
    }
    LOG_TIME("<< TableView: ");
}

static void mTableViewPiece_setSeparatorStyle(mTableViewPiece* self, enum mTableViewSeparatorStyle style)
{
    assert((style >= NCS_TABLEVIEW_SEPARATORSTYLE_NONE && 
            style < NCS_TABLEVIEW_SEPARATORSTYLE_MAX));
    self->separatorStyle = style;
}

static void mTableViewPiece_setSeparatorColor(mTableViewPiece* self, DWORD color)
{
    self->separatorColor = color;
}

/* follow is public interface, user need implement these! */
static void mTableViewPiece_onCommitInsertRowAtIndexPath(mTableViewPiece* self, const mIndexPath* indexpath)
{
    /* edit datasource. */
    /* ... */

    /* insert animation. */
    //_c(self)->insertRowAtIndexPath(self, indexpath, NCS_TABLEVIEW_ANIMATIONZOOM);
}

static void mTableViewPiece_onCommitDeleteRowAtIndexPath(mTableViewPiece* self, const mIndexPath* indexpath)
{
    /* edit datasource. */
    /* ... */

    /* delete animation. */
    //_c(self)->deleteRowAtIndexPath(self, indexpath, NCS_TABLEVIEW_ANIMATIONZOOM);
}

static mTableViewItemPiece* mTableViewPiece_createItemForRow(mTableViewPiece* self, const mIndexPath* indexpath)
{
    fprintf(stderr, "you need implement me %s.\n", __FUNCTION__);
    return NULL;
}

static int mTableViewPiece_numberOfSections(mTableViewPiece* self)
{
    return 1;
}

static int mTableViewPiece_numberOfRowsInSection(mTableViewPiece* self, int section)
{
    fprintf(stderr, "you need implement me %s.\n", __FUNCTION__);
    return 0;
}

static const char* mTableViewPiece_titleForSection(mTableViewPiece* self, int section)
{
    return "";
}

static const char* mTableViewPiece_indexForSection(mTableViewPiece* self, int section)
{
    return "";
}

static void  mTableViewPiece_rowDidSelectAtIndexPath(mTableViewPiece* self, const mIndexPath* indexpath)
{
    fprintf(stderr, "you need implement me %s, indexpath (%d,%d).\n", __FUNCTION__,
            indexpath->section, indexpath->row);
}

static BOOL mTableViewPiece_willSelectRowAtIndexPath(mTableViewPiece* self, const mIndexPath* indexpath)
{
    return TRUE;
}

BEGIN_MINI_CLASS(mTableViewPiece, mScrollViewPiece)
CLASS_METHOD_MAP(mTableViewPiece, construct   )
CLASS_METHOD_MAP(mTableViewPiece, changeMode  )
CLASS_METHOD_MAP(mTableViewPiece, processMessage      )
CLASS_METHOD_MAP(mTableViewPiece, destroy     )
CLASS_METHOD_MAP(mTableViewPiece, paint)
CLASS_METHOD_MAP(mTableViewPiece, setSeparatorStyle)
CLASS_METHOD_MAP(mTableViewPiece, setSeparatorColor)
CLASS_METHOD_MAP(mTableViewPiece, rectForHeaderInSection)
CLASS_METHOD_MAP(mTableViewPiece, rectForRowAtIndexPath)
CLASS_METHOD_MAP(mTableViewPiece, rectForSection)
CLASS_METHOD_MAP(mTableViewPiece, getDefaultRowRect)
CLASS_METHOD_MAP(mTableViewPiece, createDefaultRow)
CLASS_METHOD_MAP(mTableViewPiece, createSeparatorLine)
CLASS_METHOD_MAP(mTableViewPiece, reloadData)
CLASS_METHOD_MAP(mTableViewPiece, createIndexSectionHead)
CLASS_METHOD_MAP(mTableViewPiece, createIndexSectionContent)
CLASS_METHOD_MAP(mTableViewPiece, createGroupSectionContent)
CLASS_METHOD_MAP(mTableViewPiece, insertRowAtIndexPath)
CLASS_METHOD_MAP(mTableViewPiece, deleteRowAtIndexPath)
CLASS_METHOD_MAP(mTableViewPiece, itemToIndexPath)
CLASS_METHOD_MAP(mTableViewPiece, indexPathToItem)
CLASS_METHOD_MAP(mTableViewPiece, willSelectRowAtIndexPath)
CLASS_METHOD_MAP(mTableViewPiece, onCommitInsertRowAtIndexPath)
CLASS_METHOD_MAP(mTableViewPiece, onCommitDeleteRowAtIndexPath)
CLASS_METHOD_MAP(mTableViewPiece, createItemForRow)
CLASS_METHOD_MAP(mTableViewPiece, numberOfSections)
CLASS_METHOD_MAP(mTableViewPiece, numberOfRowsInSection)
CLASS_METHOD_MAP(mTableViewPiece, titleForSection)
CLASS_METHOD_MAP(mTableViewPiece, indexForSection)
CLASS_METHOD_MAP(mTableViewPiece, rowDidSelectAtIndexPath)
END_MINI_CLASS


/* local functions */

static void autoAdjustTableViewPosition(mTableViewPiece* self)
{
    RECT rc;
    int x, y;
    int view_bottom, max_y;

    _c(self->tablePanel)->getRect(self->tablePanel, &rc);
    view_bottom = rc.bottom;
    _c(self)->getRect(self, &rc);    
    max_y = view_bottom - rc.bottom;

    /* printf("=====================table_bottom: %d, view_bottom: %d\n", rc.bottom, view_bottom); */

    if (max_y <= 0) {
        /* do nothing */
    }
    else {
        _c(self)->getViewport(self, &rc);
        x = rc.left;
        y = rc.top;
    
        if (y > max_y) y = max_y;

        _c(self)->moveViewport(self, x, y);
    }
}

static BOOL onIndexLocatePieceTouched(mTableViewPiece *self,
				      mHotPiece *sender, int event_id, DWORD param)
{
    RECT rc;
    int x, y;
    int view_bottom, max_y;

    _c(self->tablePanel)->getRect(self->tablePanel, &rc);
    view_bottom = rc.bottom;
    _c(self)->getRect(self, &rc);    
    max_y = view_bottom - rc.bottom;

    _c(self)->rectForHeaderInSection(self, param, &rc);
    x = rc.left;
    y = rc.top;

    if (y > max_y) y = max_y;
    _c(self)->moveViewport(self, x, y);
    
    return TRUE;
}

static void _piece_anim_cb(MGEFF_ANIMATION handle, void *target, intptr_t id, void *value)
{
    mHotPiece* child = (mHotPiece*)target;
    mPanelPiece* group = (mPanelPiece*) child->parent;
    mPanelPiece* viewContent = (mPanelPiece*)group->parent;
    mTableViewPiece* view = (mTableViewPiece*)viewContent->parent;
    RECT rc;

    _c(child)->getRect(child, &rc);
    rc.bottom = rc.top + *(int*)value;
    _c(child)->setRect(child,  &rc);

    assert(INSTANCEOF(group, mPanelPiece));
    _c(group)->reLayout(group);
    _c(viewContent)->reLayout(viewContent);
    assert(INSTANCEOF(viewContent, mPanelPiece));
    {
        mLineVBox* vbox = (mLineVBox*)group->layout;
        mItemIterator *iter = _c(group->itemManager)->createItemIterator(group->itemManager);
        _c(vbox)->getLayoutRect(vbox, iter, &rc);
        _c(group)->setRect(group, &rc);
        DELETE(iter);

        vbox = (mLineVBox*)viewContent->layout;
        iter = _c(viewContent->itemManager)->createItemIterator(viewContent->itemManager);
        _c(vbox)->getLayoutRect(vbox, iter, &rc);
        _c(viewContent)->setRect(viewContent, &rc);
        DELETE(iter);
    }

    autoAdjustTableViewPosition(view);

    PanelPiece_invalidatePiece((mHotPiece*)viewContent, NULL);
}

static mPieceItem* nextItem(mTableViewPiece* self, mItemIterator* iter, mHotPiece* prev)
{
    mPieceItem* item = NULL;
    mHotPiece* piece = NULL;
    mPieceItem* result = NULL;

    assert(NULL != iter);
    while ( (item = _c(iter)->next(iter)) ) {
        piece = (mHotPiece*)_c(item)->getPiece(item);

        if (piece == prev) {
            if ( (item = _c(iter)->next(iter)) ) {
                result = item;
            }
            break;
        }
    }

    return result;
}
