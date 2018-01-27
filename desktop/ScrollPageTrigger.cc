/*!============================================================================
 * @file ScrollPageTirgger.c
 * @Synopsis  
 * @author humingming
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include "ScrollPageTrigger.hh"
#include "TransitionSpeedMeasure.hh"

#define ANIMATIONTIME_SLOW    200
#define ANIMATIONTIME_FAST    100


ScrollPageTrigger::ScrollPageTrigger ()
{
}


ScrollPageTrigger::~ScrollPageTrigger ()
{
}


void ScrollPageTrigger::onScrollPageTrigger(void* object, Transition* trans, const Event* event, void* param)
{
    ScrollParam* scroll_param = (ScrollParam*)param;
    if (NULL == scroll_param)
        return;

    int view_w, boundary_w;
    int cur_page, total_page;
    int page_left, page_right;
    int startvalue, endvalue;
    int duration;
    float speed, abs_speed;
    RECT cur_rc;

    IPhoneDesktopController* controller = scroll_param->controller;
    IPhoneDesktopView* view = &(controller->getView());
    IPhoneDesktopLayout* layout = &(controller->getLayout());
    TransitionSpeedMeasure* trans_speed = (TransitionSpeedMeasure*)trans;


    speed = trans_speed->getCurSpeed ();
    abs_speed = speed < 0.0 ? -speed:speed;

    cur_rc = view->getRect ();
    view_w = RECTW(cur_rc);
    boundary_w = view_w / 2;
    cur_page = layout->currentPage ();
    total_page = layout->totalPage ();


    startvalue = cur_rc.left;
    
    /* inertia scroll */
    if (abs_speed >=HIGHSPEED_DOWN && abs_speed <=HIGHSPEED_UP) {
        /* scroll to prev page  */
        if (speed > 0.0) {
            cur_page = layout->prevPage ();
        }
        /* scroll to next page */
        else {
            cur_page = layout->nextPage (); 
        }
    }
    /* not inertia scroll */
    else {
        page_left = cur_page * view_w;
        page_right = page_left + view_w;

        if (cur_rc.left <= page_left-boundary_w) {
            cur_page = layout->prevPage ();
            printf ("move prevPage\n");
        }
        else if (cur_rc.right >= page_right+boundary_w) {
            cur_page = layout->nextPage ();
            printf ("move nextPage\n");
        }
    }

    endvalue = cur_page * view_w;
 

    /* calucate animation duration */
    if (abs_speed >= LOWSPEED_DOWN && abs_speed <=LOWSPEED_UP)
        duration = ANIMATIONTIME_SLOW;
    else if (abs_speed >=HIGHSPEED_DOWN && abs_speed <=HIGHSPEED_UP)
        duration = ANIMATIONTIME_SLOW - (int)(abs_speed*18.0);
    else 
        duration = ANIMATIONTIME_FAST;

    if (duration < ANIMATIONTIME_FAST)
        duration = ANIMATIONTIME_FAST;

    printf ("state:%p, s:%d, e:%d, duration:%d, cur_page:%d, total_page:%d\n", 
            object, startvalue, endvalue, duration, layout->currentPage(), total_page);


    /* run animation */
    if (scroll_param->animation) {
        mGEffAnimationSetStartValue (scroll_param->animation, &startvalue);
        mGEffAnimationSetEndValue (scroll_param->animation, &endvalue);
        //mGEffAnimationSetCurve (animation,InQuad);

        mGEffAnimationSetDuration (scroll_param->animation, duration);
        /* remember to delete this animation manual to avoid memory leak */
        mGEffAnimationAsyncRun (scroll_param->animation);
    }
}


void ScrollPageTrigger::animationCallback (MGEFF_ANIMATION handle, void* target, intptr_t id, void* value)
{
    RECT view_rc, move_rc;
    IPhoneDesktopView* view;

    int* cur_pos = (int*)value;
    IPhoneDesktopController* controller = (IPhoneDesktopController*)target;

    if (NULL == controller) 
        return;

    view = &(controller->getView());
    view_rc = view->getRect ();
    SetRect (&move_rc, *cur_pos, 0, (*cur_pos)+RECTW(view_rc), RECTH(view_rc));
    //printf ("scroll animation:(%d, %d, %d, %d)\n", move_rc.left, move_rc.top, move_rc.right, move_rc.bottom);

    view->moveTo (move_rc);
    view->invalidateRect(&view->getAnimationArea());
    //UpdateWindow (controller->getHwnd(), TRUE);
}


