#include "IPhoneDesktopTransitionInterface.hh"

IPhoneDesktopItem *IPhoneDesktopTransitionInterface::point2Item(const POINT &point) {
    Item *item = m_controller->getLayout().point2Item(point);

    if (item) {
        RECT rc;
        POINT pt;
        item->getRect(rc);
        m_controller->getView().ViewToScene(point, pt);
        if (PtInRect(&rc, pt.x, pt.y)) {
            return (IPhoneDesktopItem *)item;
        }
    }
    return NULL;
}
