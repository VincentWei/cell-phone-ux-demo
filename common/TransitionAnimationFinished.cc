#include "TransitionAnimationFinished.hh"
#include "global.h"

static const char SIGNAL_FINISHED[] = "finished";

TransitionAnimationFinished::TransitionAnimationFinished(MGEFF_ANIMATION animation, State *source, State *target, const char *name)
    : SignalTransition(animation, SIGNAL_FINISHED, source, target, name)
{
    setAnimation(animation);
}

TransitionAnimationFinished::~TransitionAnimationFinished() {
}

void TransitionAnimationFinished::setAnimation(MGEFF_ANIMATION animation) {
    setSignal(animation, SIGNAL_FINISHED);
    if (animation) {
        mGEffAnimationSetContext(animation, this); /* XXX: occupy the context of the animation */
        mGEffAnimationSetFinishedCb(animation, onAnimationFinished);
    }
    else
        printf ("animation not set\n");
}

void TransitionAnimationFinished::onAnimationFinished(MGEFF_ANIMATION animation) {
    SignalEvent *event = new SignalEvent(animation, SIGNAL_FINISHED);
    Transition *transition = (Transition *) mGEffAnimationGetContext(animation);

    printf("%s:%d.\n", __FUNCTION__, __LINE__);
    transition->source()->stateMachine()->postEvent(event);
}
