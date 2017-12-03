#include "TransitionStandStillOnItem.hh"

TransitionStandStillOnItem::TransitionStandStillOnItem(State *source, State *target, IPhoneDesktopController *controller, int timeoutMs, const char *name)
    : TransitionStandStill(source, target, timeoutMs, name), IPhoneDesktopTransitionInterface(controller)
{
}

TransitionStandStillOnItem::~TransitionStandStillOnItem() {
}

bool TransitionStandStillOnItem::eventTest(const Event *event) {
    if (! TransitionStandStill::eventTest(event)) {
        return false;
    }
    
    IPhoneDesktopItem *item = point2Item(m_point);
    if (item) {
        item->setHilite(false);
        setItem(item);
        return true;
    }else{
        return false;
    }
}
