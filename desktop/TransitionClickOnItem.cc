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

#include "global.h"
#include "TransitionClickOnItem.hh"

TransitionClickOnItem::TransitionClickOnItem(State *source, State *target, IPhoneDesktopController *controller, const char *name)
    : EventTransition(Event::MouseEvent | Event::MoveEvent, source, target, name), IPhoneDesktopTransitionInterface(controller)
{
    setButtonType(MouseEvent::ButtonPressed | MouseEvent::ButtonReleased);
    setButtons(MouseEvent::LeftButton);
}

TransitionClickOnItem::~TransitionClickOnItem() {
}

bool TransitionClickOnItem::testMove(const MoveEvent *event) { 
    POINT pt;
    RECT rc, viewRc;
    pt = event->pos();
    IPhoneDesktopItem* oldItem = item();
    if (oldItem) {
        oldItem->setHilite(false);
        oldItem->getRect(rc);
        m_controller->getView().SceneToView(rc, viewRc);
        InvalidateRect(m_controller->getHwnd(), &viewRc, FALSE);
    }
    return true;
}

bool TransitionClickOnItem::testMouse(const MouseEvent *event) {
    POINT point;
    RECT rc;
    RECT viewRc;

    if (! EventTransition::testMouse(event)) {
        return false;
    }

    event->point(point);
    if (event->type() == MouseEvent::ButtonPressed) {
        IPhoneDesktopItem* item = point2Item(point);
        if (item) {
            item->setHilite(true);
            item->getRect(rc);
            m_controller->getView().SceneToView(rc, viewRc);
            InvalidateRect(m_controller->getHwnd(), &viewRc, FALSE);
        }
        setItem(item);
        return false;
    }else{
        IPhoneDesktopItem *oldItem, *newItem;
        oldItem = item();
        newItem = point2Item(point);

        if (NULL != oldItem) {
            oldItem->setHilite(false);
            oldItem->getRect(rc);
            m_controller->getView().SceneToView(rc, viewRc);
            InvalidateRect(m_controller->getHwnd(), &viewRc, FALSE);
        }

        if (newItem && oldItem == newItem) {
            setItem(newItem);
            MouseEvent* mouseEvent = const_cast<MouseEvent*>(event);
            mouseEvent->appendParameter(std::string(newItem->name()));
            return true;
        }else{
            return false;
        }
    }
}
