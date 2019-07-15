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
 * @file IPhoneDestktopStateDraging.cc
 * @Synopsis  
 * @author humingming
 * @version 1.0
 * Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include "IPhoneDesktopStateDraging.hh"
#include "global.h"


IPhoneDesktopStateDraging::IPhoneDesktopStateDraging(IPhoneDesktopController* controller, State* parentState, const char* name)
    : IPhoneDesktopState(controller, parentState, name)
{
}


IPhoneDesktopStateDraging::~IPhoneDesktopStateDraging() 
{
}


void IPhoneDesktopStateDraging::onDragingTrigger(void* object, Transition* trans, const Event* event, void* param)
{
    int /*cur_page, */total_page;
    int scene_w, view_w, boundary_w;
    int distance;
    POINT old_pos, pos;
    RECT view_rc, move_rc;

    MoveEvent* move_event = (MoveEvent*)event;
    IPhoneDesktopController* controller = (IPhoneDesktopController*)param;
    IPhoneDesktopLayout* layout = &(controller->getLayout());
    IPhoneDesktopView* view = &(controller->getView());

    old_pos = move_event->oldPos ();
    pos = move_event->pos ();
    distance = pos.x - old_pos.x;

    view_rc = view->getRect ();
    view_w = RECTW(view_rc);
    //cur_page = layout->currentPage ();
    total_page = layout->totalPage ();
    scene_w = view_w * total_page;
    boundary_w = view_w / 2;

    //printf ("draging: %d-%d=dx:%d, rc(%d,%d,%d,%d), cur:%d, total:%d, scene:%d, half:%d\n", 
    //        pos.x, old_pos.x, distance, 
    //        view_rc.left, view_rc.top, view_rc.right, view_rc.bottom, 
    //        cur_page, total_page, scene_w, half_view);

    /* distance positive: move to left 
     * distance negative: move to right 
     */
    SetRect (&move_rc, view_rc.left-distance, view_rc.top, view_rc.right-distance, view_rc.bottom);
    if (move_rc.left <= 0-boundary_w || move_rc.right >= scene_w+boundary_w)
        return;

    view->moveTo (move_rc);
    view->invalidateRect(&view->getAnimationArea());
}


