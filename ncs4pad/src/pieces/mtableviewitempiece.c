#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "register.h"

#include "mspeedmeter.h"
#include "physics-animation.h"
#include "mtableviewitempiece.h"

#define ANIM_RUNTIME 300

#define SW_W 34
#define SW_H 45
#define DEL_W 60
#define DEL_H 30
#define DEL_BTN_W (DEL_W - 10)
#define DEL_BTN_H DEL_H

static void userPieceZoomAnimationCb(MGEFF_ANIMATION handle, void *target, int id, void *value)
{
    mButtonPanelPiece *userPiece = (mButtonPanelPiece*)target;
    _c(userPiece)->setRect(userPiece, (RECT*)value);
    PanelPiece_invalidatePiece((mHotPiece*)userPiece, NULL);
}

static MGEFF_ANIMATION userPieceZoomAnimation(mTableViewItemPiece *self, mHotPiece *child, BOOL left, BOOL right)
{
    MGEFF_ANIMATION anim = mGEffAnimationCreate(child, userPieceZoomAnimationCb, 0, MGEFF_RECT);

    RECT rcParent;
    RECT rc_s, rc_e;
    unsigned width;

    _c(self)->getRect(self, &rcParent);
    width = RECTW(rcParent);
    if (left) width -= SW_W;
    if (right) width -= DEL_W;

    if (width < RECTW(rcParent))
        self->userPieceIsLocked = TRUE;
    else
        self->userPieceIsLocked = FALSE;


    _c(child)->getRect(child, &rc_s);
    SetRect(&rc_e, 0, 0, width, RECTH(rc_s));

    mGEffAnimationSetStartValue(anim, &rc_s);
    mGEffAnimationSetEndValue(anim, &rc_e);
    mGEffAnimationSetDuration(anim, ANIM_RUNTIME);
    mGEffAnimationSetCurve(anim, Linear);

    return anim;
}

static MGEFF_ANIMATION item_swtichPiece_appear(mTableViewItemPiece *self)
{
    MGEFF_ANIMATION anim;
#if 1
    anim = _c(self)->movePieceWithAnimation(self, self->switchPiece,
            self->ptSwitchPiece[1].x, self->ptSwitchPiece[1].y,
            ANIM_RUNTIME, Linear);
#else
    anim = _c(self)->setPieceAlphaWithAnimation(self, self->switchPiece,
            255, ANIM_RUNTIME, Linear);
#endif
    return anim;
}

static MGEFF_ANIMATION item_swtichPiece_disappear(mTableViewItemPiece *self)
{
    MGEFF_ANIMATION anim;
#if 1
    anim = _c(self)->movePieceWithAnimation(self, self->switchPiece,
            self->ptSwitchPiece[0].x, self->ptSwitchPiece[0].y,
            ANIM_RUNTIME, Linear);
#else
    anim = _c(self)->setPieceAlphaWithAnimation(self, self->switchPiece,
            128, ANIM_RUNTIME, Linear);
#endif
    _c((mRotateSwitchPiece*)self->switchPiece)->resetState((mRotateSwitchPiece*)self->switchPiece);
    
    return anim;
}

static MGEFF_ANIMATION item_userPiece_zoomout(mTableViewItemPiece *self, BOOL left, BOOL right)
{
    if (self->userPiece) {
        MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

        if (!left) {
            MGEFF_ANIMATION anim_move = _c(self)->movePieceWithAnimation(
                    self, self->userPiece,
                    self->ptUserPiece[0].x, self->ptUserPiece[0].y,
                    ANIM_RUNTIME, Linear);
            mGEffAnimationAddToGroup(group, anim_move);
        }

        {
            MGEFF_ANIMATION anim_zoom;
            anim_zoom = userPieceZoomAnimation(self, self->userPiece, left, right);
            mGEffAnimationAddToGroup(group, anim_zoom);
        }

        return group;
    }

    return NULL;
}

static MGEFF_ANIMATION item_userPiece_zoomin(mTableViewItemPiece *self, BOOL left, BOOL right)
{
    if (self->userPiece) {
        MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);

        if (left) {
            MGEFF_ANIMATION anim_move = _c(self)->movePieceWithAnimation(
                    self, self->userPiece,
                    self->ptUserPiece[1].x, self->ptUserPiece[1].y,
                    ANIM_RUNTIME, Linear);
            mGEffAnimationAddToGroup(group, anim_move);
        }

        {
            MGEFF_ANIMATION anim_zoom;
            anim_zoom = userPieceZoomAnimation(self, self->userPiece, left, right);
            mGEffAnimationAddToGroup(group, anim_zoom);
        }

        return group;
    }

    return NULL;
}

static MGEFF_ANIMATION item_deletePiece_appear(mTableViewItemPiece *self)
{
    self->deletePieceIsAppear = TRUE;

    return _c(self)->movePieceWithAnimation(self, self->deletePiece,
            self->ptDeletePiece[1].x, self->ptDeletePiece[1].y,
            ANIM_RUNTIME, Linear);
}

static MGEFF_ANIMATION item_deletePiece_disappear(mTableViewItemPiece *self)
{
    self->deletePieceIsAppear = FALSE;

    return  _c(self)->movePieceWithAnimation(self, self->deletePiece,
            self->ptDeletePiece[0].x, self->ptDeletePiece[0].y,
            ANIM_RUNTIME, Linear);
}


static void anim_finished_cb(MGEFF_ANIMATION handle)
{
    mTableViewItemPiece *self = (mTableViewItemPiece *)mGEffAnimationGetContext(handle);
    self->switchStateChangeAnimation = NULL;
}
static BOOL onSwitchPieceStateChanged(mTableViewItemPiece *self, mHotPiece *piece, int event_id, DWORD param)
{
    if (event_id == NCSN_ROTATESWITCHPIECE_OFF ||
            event_id == NCSN_ROTATESWITCHPIECE_ON)
    {
        if (self->switchStateChangeAnimation) {
            mGEffAnimationStop(self->switchStateChangeAnimation);
            self->switchStateChangeAnimation = NULL;
        }
    }
    if (event_id == NCSN_ROTATESWITCHPIECE_OFF) {
            mPanelPiece *topPanel = PanelPiece_getTopPanel((mHotPiece*)self);
            MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
            puts("NCSN_ROTATESWITCHPIECE_OFF");
            //animation
            mGEffAnimationAddToGroup(group, item_deletePiece_disappear(self));
            mGEffAnimationAddToGroup(group, item_userPiece_zoomout(self, TRUE, FALSE));
            mGEffAnimationSetContext(group, self);
            mGEffAnimationSetFinishedCb(group, anim_finished_cb);
            _c(topPanel)->animationAsyncRun(topPanel, group, 0);
            self->switchStateChangeAnimation = group;
            return TRUE;
    } else if (event_id == NCSN_ROTATESWITCHPIECE_ON) {
            mPanelPiece *topPanel = PanelPiece_getTopPanel((mHotPiece*)self);
            MGEFF_ANIMATION group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
            puts("NCSN_ROTATESWITCHPIECE_ON");
            //animation
            mGEffAnimationAddToGroup(group, item_deletePiece_appear(self));
            mGEffAnimationAddToGroup(group, item_userPiece_zoomout(self, TRUE, TRUE));
            mGEffAnimationSetContext(group, self);
            mGEffAnimationSetFinishedCb(group, anim_finished_cb);
            _c(topPanel)->animationAsyncRun(topPanel, group, 0);
            self->switchStateChangeAnimation = group;
            return TRUE;
    }
    return FALSE;
}

static mShapeTransRoundPiece *createBackgroundPiece(mTableViewItemPiece *self)
{
    mShapeTransRoundPiece *bk = NEWPIECE(mShapeTransRoundPiece);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_BKCOLOR, NCS_TABLEVIEWITEM_BKCOLOR_VALUE);
    _c(bk)->setProperty(bk, NCSP_TRANROUND_RADIUS, 0);

    return bk;
}

static mRotateSwitchPiece *createSwitchPiece(mTableViewItemPiece *self)
{
    mRotateSwitchPiece *sw = NEWPIECE(mRotateSwitchPiece);
    int event_ids[] = {NCSN_ROTATESWITCHPIECE_OFF, NCSN_ROTATESWITCHPIECE_ON, 0};

    _c(sw)->setProperty(sw, NCSP_ROTATESWITCH_RADIUS, 4);

    ncsAddEventListeners((mObject*)sw, (mObject*)self,
            (NCS_CB_ONPIECEEVENT)onSwitchPieceStateChanged, event_ids);

    return sw;
}

#if 0
static mButtonPanelPiece *createUserPiece(mTableViewItemPiece *self)
{
    mButtonPanelPiece *content = NEWPIECE(mButtonPanelPiece);
    _c(content->background)->setProperty(content->background, NCSP_TRANROUND_RADIUS, 0);
    _c(content)->setStateTable(content, (mHotPiece*)content->background, NCSP_TRANROUND_BKCOLOR, 0x00000000, 0xFF0000FF);
    _c(content)->setStates(content);

    return content;
}
#endif

static m3DButtonPiece *createDeletePiece(mTableViewItemPiece *self)
{
    m3DButtonPiece *btn = NEWPIECE(m3DButtonPiece);
    _c(btn)->setProperty(btn, NCSP_3DBUTTONPIECE_TEXT, (DWORD)"Delete");

    return btn;
}

static void setRectOfUserPiece(mTableViewItemPiece *self, mHotPiece *piece)
{
    RECT rc, rcChild;
    int w, h;
    POINT *pt;

    _c(self)->getRect(self, &rc);
    _c(piece)->getRect(piece, &rcChild);

    w = RECTW(rc);
    if (RECTH(rc) < RECTH(rcChild)) {
        h = RECTH(rcChild);
        SetRect(&rc, 0, 0, RECTW(rc), RECTH(rcChild));
        _c(self)->setRect(self, &rc);
    } else {
        h = RECTH(rc);
        SetRect(&rcChild, 0, 0, w, h);
        _c(piece)->setRect(piece, &rcChild);
    }
    pt = self->ptUserPiece;
    pt[0].x = 0;
    pt[1].x = SW_W;
    pt[0].y = pt[1].y = RECTH(rc) / 2 - h / 2;
}

static void reLayout(mTableViewItemPiece* self, const RECT *prc)
{
    RECT rc;
    int h, w;
    POINT *pt;
    mPieceItem *item;
    mShapeTransRoundPiece* bk = _c(self)->getBkgndPiece(self);

    _c(bk)->setRect(bk, prc);

    if (self->switchPiece) {
        w = h = SW_H / 2;
        pt = self->ptSwitchPiece;
        pt[0].x = -(SW_W / 2 + w / 2);
        pt[1].x = pt[0].x + SW_W;
        pt[0].y = pt[1].y = RECTHP(prc) / 2 - h / 2;
        SetRect(&rc, 0, 0, w, h);
        _c(self->switchPiece)->setRect(self->switchPiece, &rc);
        if ((item = _c(self)->searchItem(self, self->switchPiece))) {
            item->x = pt[0].x;
            item->y = pt[0].y;
        }
    }

    if (self->deletePiece) {
        w = DEL_BTN_W;
        h = DEL_BTN_H;
        pt = self->ptDeletePiece;
        pt[0].x = RECTWP(prc) + DEL_W / 2 - w / 2;
        pt[1].x = pt[0].x - DEL_W;
        pt[0].y = pt[1].y = RECTHP(prc) / 2 - h / 2;
        SetRect(&rc, 0, 0, w, h);
        _c(self->deletePiece)->setRect(self->deletePiece, &rc);
        if ((item = _c(self)->searchItem(self, self->deletePiece))) {
            item->x = pt[0].x;
            item->y = pt[0].y;
        }
    }

    if (self->userPiece)
        setRectOfUserPiece(self, self->userPiece);
}

static void editModeEnter(mTableViewItemPiece *self)
{
    RECT rc;

    if (self->switchPiece || self->deletePiece) {
        return;
    }
    self->switchPiece = (mHotPiece*)createSwitchPiece(self);
    self->deletePiece = (mHotPiece*)createDeletePiece(self);

    _c(self)->addContent(self, self->switchPiece, 0, 0);
    _c(self)->addContent(self, self->deletePiece, 0, 0);

    _c(self)->getRect(self, &rc);
    reLayout(self, &rc);
}

static void editModeLeave(mTableViewItemPiece *self)
{
    //_c(self)->delContent(self, self->switchPiece);
    //_c(self)->delContent(self, self->deletePiece);

    //self->switchPiece = NULL;
    //self->deletePiece = NULL;
}

static void mTableViewItemPiece_construct(mTableViewItemPiece *self, DWORD add_data)
{
    Class(mPanelPiece).construct((mPanelPiece*)self, add_data);

    self->mode = NCS_TABLEVIEWITEMPIECE_NORMAL;

    self->switchPiece = NULL;
    self->userPiece   = NULL;
    self->deletePiece = NULL;

    _c(self)->setBkgndPiece(self, createBackgroundPiece(self));

    self->userPieceIsLocked = FALSE;
    self->deletePieceIsAppear = FALSE;
    self->highlight = TRUE;
    self->pushDown = FALSE;

    self->add_data = add_data;

    self->modeChangeAnimation = NULL;
    self->switchStateChangeAnimation = NULL;
}

static BOOL mTableViewItemPiece_setRect(mTableViewItemPiece *self, const RECT *prc)
{
    BOOL ret = Class(mPanelPiece).setRect((mPanelPiece*)self, prc);

    if (ret) {
        reLayout(self, prc);
    }

    return ret;
}

static void mTableViewItemPiece_setUserPiece(mTableViewItemPiece *self, mHotPiece* piece)
{
    /* delete the current userPiece */
    if (self->userPiece) {
        _c(self)->delContent(self, piece);
    }

    if (piece) {
        setRectOfUserPiece(self, piece);

        _c(self)->addContent(self, piece, self->ptUserPiece[0].x, self->ptUserPiece[0].y);

        self->userPiece = piece;
    } else {
        self->userPiece = NULL;
    }
}
static void mTableViewItemPiece_destroy(mTableViewItemPiece *self)
{
    if (self->switchStateChangeAnimation) {
        mGEffAnimationStop(self->switchStateChangeAnimation);
        self->switchStateChangeAnimation = NULL;
    }
    if (self->modeChangeAnimation) {
        mGEffAnimationStop(self->modeChangeAnimation);
        mGEffAnimationDelete(self->modeChangeAnimation);
        self->modeChangeAnimation = NULL;
    }

    Class(mPanelPiece).destroy((mPanelPiece*)self);
}

static void _finished_cb(MGEFF_ANIMATION handle)
{
    mTableViewItemPiece *self = mGEffAnimationGetContext(handle);
    editModeLeave(self);
}

static void mTableViewItemPiece_changeMode(mTableViewItemPiece *self)
{
    MGEFF_ANIMATION group = NULL;

    self->mode = (self->mode == NCS_TABLEVIEWITEMPIECE_NORMAL ?
            NCS_TABLEVIEWITEMPIECE_EDIT : NCS_TABLEVIEWITEMPIECE_NORMAL);

    if (!self->switchPiece)
        editModeEnter(self);

    if (self->modeChangeAnimation) {
        mGEffAnimationStop(self->modeChangeAnimation);
        mGEffAnimationDelete(self->modeChangeAnimation);
        self->modeChangeAnimation = NULL;
    }

    if (self->mode == NCS_TABLEVIEWITEMPIECE_NORMAL) {
        mPanelPiece *topPanel = PanelPiece_getTopPanel((mHotPiece*)self);
        group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
        mGEffAnimationAddToGroup(group, item_swtichPiece_disappear(self));
        mGEffAnimationAddToGroup(group, item_userPiece_zoomout(self, FALSE, FALSE));
        mGEffAnimationAddToGroup(group, item_deletePiece_disappear(self));
        mGEffAnimationSetContext(group, self);
        mGEffAnimationSetFinishedCb(group, _finished_cb);
        //animation
        _c(topPanel)->animationAsyncRun(topPanel, group, 1);
    } else if (self->mode == NCS_TABLEVIEWITEMPIECE_EDIT) {
        mPanelPiece *topPanel = PanelPiece_getTopPanel((mHotPiece*)self);
        group = mGEffAnimationCreateGroup(MGEFF_PARALLEL);
        mGEffAnimationAddToGroup(group, item_swtichPiece_appear(self));
        mGEffAnimationAddToGroup(group, item_userPiece_zoomin(self, TRUE, FALSE));
        mGEffAnimationAddToGroup(group, item_deletePiece_disappear(self));
        //animation
        _c(topPanel)->animationAsyncRun(topPanel, group, 1);
    }

    self->modeChangeAnimation = group;
}

static void mTableViewItemPiece_resetEditMode(mTableViewItemPiece *self)
{
    mObject *owner = (mObject*)self->owner;
    if (self->deletePieceIsAppear) {
        mRotateSwitchPiece *switchPiece = (mRotateSwitchPiece*)self->switchPiece;
        if (!switchPiece) {
            editModeEnter(self);
        }

        _c(switchPiece)->processMessage(switchPiece, MSG_LBUTTONDOWN, 0, 0, owner);
        _c(switchPiece)->processMessage(switchPiece, MSG_LBUTTONUP, 0, 0, owner);
    }
}


static ARGB  func_gradient_color[] = {0xff0b69ab, 0xff27ade0};
static float func_gradient_pos[] = {0.0, 1.0};

static void s_setGradientColor(mTableViewItemPiece* self, mShapeTransRoundPiece *piece,
        ARGB *colors, float *pos, int num)
{
    HBRUSH brush;
    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_GRADIENT_FILL);
    brush = _c(piece)->getBrush(piece);
    MGPlusSetLinearGradientBrushColorsEx(brush, colors, num, pos);
    PanelPiece_invalidatePiece((mHotPiece*)self, NULL);
}

static void s_setSolidFillColor(mTableViewItemPiece* self, mShapeTransRoundPiece *piece)
{
    _c(piece)->setProperty (piece, NCSP_TRANROUND_FILLMODE, NCSP_TRANROUND_SINGLE_FILL);
    _c(piece)->setProperty(piece, NCSP_TRANROUND_BKCOLOR, NCS_TABLEVIEWITEM_BKCOLOR_VALUE);
    PanelPiece_invalidatePiece((mHotPiece*)self, NULL);
}

static void mTableViewItemPiece_setHighlight(mTableViewItemPiece* self, BOOL highlight)
{
    mShapeTransRoundPiece *bk = _c(self)->getBkgndPiece(self);
    if (highlight) {
        s_setGradientColor(self, bk, 
                func_gradient_color, func_gradient_pos, TABLESIZE(func_gradient_color));
    }
    else {
        s_setSolidFillColor(self, bk);
    }
}

static int mTableViewItemPiece_processMessage(mTableViewItemPiece *self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    mHotPiece *hotPiece;
    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK) {
        hotPiece = _c(self)->childAt(self, LOSWORD(lParam),HISWORD(lParam));
        if (message == MSG_LBUTTONUP) {
            if (hotPiece == self->switchPiece) {
//                ncsRaiseEvent((mObject*)self, NCSN_TABLEVIEWITEMPIECE_SWITCHCLICKED, (DWORD)0);
            } else if (hotPiece == self->deletePiece) {
                ncsRaiseEvent((mObject*)self, NCSN_TABLEVIEWITEMPIECE_DELBTNCLICKED, (DWORD)self->add_data);
            } else {
                if (!self->userPieceIsLocked) {
                    if (hotPiece == self->userPiece&&self->pushDown) {
                        ncsRaiseEvent((mObject*)self, NCSN_TABLEVIEWITEMPIECE_CONTENTCLICKED, self->add_data);
                    }
                } else {
                    if (!self->deletePieceIsAppear) {
#if 0
                        float v_x, v_y;
                        mRotateSwitchPiece *switchPiece = (mRotateSwitchPiece*)self->switchPiece;
                        QueryMouseMoveVelocity(&v_x, &v_y);
                        if (switchPiece && abs(v_x) > 200.0 && abs(v_y) < 100.0) {
                            _c(switchPiece)->processMessage(switchPiece, MSG_LBUTTONDOWN, 0, 0, owner);
                            _c(switchPiece)->processMessage(switchPiece, MSG_LBUTTONUP, 0, 0, owner);
                            fprintf(stderr, "valie =%d, click rotatepiece, vx=%f\n", 
                                    self->deletePieceIsAppear, v_x);
                        }
#endif
                    }
                    else {
                        _c(self)->resetEditMode(self);
                    }
                }
            }
            _c(self)->setHighlight(self, FALSE);
            self->pushDown = FALSE;
            //fprintf(stderr, "%p:setHighlight FALSE.\n", self);
        }
        else if (message == MSG_LBUTTONDOWN && self->highlight){
            _c(self)->setHighlight(self, TRUE);
            self->pushDown = TRUE;
            //fprintf(stderr, "%p:setHighlight TRUE.\n", self);
        }
        else if (message == MSG_MOUSEMOVE) {
            _c(self)->setHighlight(self, FALSE);
            self->pushDown = FALSE;
            //fprintf(stderr, "%p:setHighlight FALSE.\n", self);
        }

        if (hotPiece == self->userPiece && self->userPieceIsLocked)
            return 0;
    } 

    return Class(mPanelPiece).processMessage((mPanelPiece*)self, message, wParam, lParam, owner);
}

static mHotPiece* mTableViewItemPiece_getSwitchPiece(mTableViewItemPiece *self)
{
    return self->switchPiece;
}

static mHotPiece* mTableViewItemPiece_getUserPiece(mTableViewItemPiece *self)
{
    return self->userPiece;
}

static mHotPiece* mTableViewItemPiece_getDeletePiece(mTableViewItemPiece *self)
{
    return self->deletePiece;
}


BEGIN_MINI_CLASS(mTableViewItemPiece, mPanelPiece)
    CLASS_METHOD_MAP(mTableViewItemPiece, construct)
    CLASS_METHOD_MAP(mTableViewItemPiece, destroy)
    CLASS_METHOD_MAP(mTableViewItemPiece, changeMode )
    CLASS_METHOD_MAP(mTableViewItemPiece, resetEditMode)
    CLASS_METHOD_MAP(mTableViewItemPiece, processMessage)
    CLASS_METHOD_MAP(mTableViewItemPiece, setRect )
    CLASS_METHOD_MAP(mTableViewItemPiece, setHighlight )
    CLASS_METHOD_MAP(mTableViewItemPiece, getSwitchPiece)
    CLASS_METHOD_MAP(mTableViewItemPiece, getUserPiece)
    CLASS_METHOD_MAP(mTableViewItemPiece, setUserPiece)
    CLASS_METHOD_MAP(mTableViewItemPiece, getDeletePiece)
END_MINI_CLASS
