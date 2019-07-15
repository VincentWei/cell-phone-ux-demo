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
#include "TransitionPressOnItem.hh"
#include "IPhoneDesktopView.hh"
#include "global.h"

TransitionPressOnItem::TransitionPressOnItem(State *source, State *target, IPhoneDesktopController *controller, const char *name)
    : EventTransition(Event::MouseEvent, source, target, name), IPhoneDesktopTransitionInterface(controller)
{
    setButtonType(MouseEvent::ButtonPressed);
    setButtons(MouseEvent::LeftButton);
}

TransitionPressOnItem::~TransitionPressOnItem() {
}

bool TransitionPressOnItem::testMouse(const MouseEvent *event) {
    POINT point;

    if (! EventTransition::testMouse(event)) {
        return false;
    }

    event->point(point);
    if (event->type() == MouseEvent::ButtonPressed) {
        Item* item;
        printf("%s:pos(%d,%d).\n", __FUNCTION__, point.x, point.y);
        if ((item = point2Item(point))) {
            RECT rc;
            item->getRect(rc);
            m_controller->getView().ViewToScene(point, point);
            if (PtInRect(&rc, point.x, point.y)) {
                printf("%s:press item is %p, pos(%d,%d).\n", __FUNCTION__, item, point.x, point.y);
                return true;
            }
        }
    }
    return false;
}
