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
#include <assert.h>
#include "StateMachine.hh"
#include "global.h"
using namespace std;
using namespace mStateMachine;

#define d(x) printf x

/*
 * State
 */
State::State() :
    m_name("root"),
    m_stateMachine(NULL),
    m_parentState(NULL),
    m_initalState(NULL),
    m_depth(0),
    m_entered(false)
{
}

State::State(StateMachine *stateMachine, const char *name) :
    m_name(name),
    m_stateMachine(stateMachine),
    m_parentState(NULL),
    m_initalState(NULL),
    m_depth(1),
    m_entered(false)
{
    stateMachine->addState(this);
}

State::State(State *parent, const char *name) :
    m_name(name),
    m_stateMachine(NULL),
    m_parentState(parent),
    m_initalState(NULL),
    m_depth(parent->depth() + 1),
    m_entered(false)
{
    parent->addState(this);
}

State::~State() {
    while (! m_childStates.empty()) {
        State *child = m_childStates.back();
        m_childStates.pop_back();
        delete child;
    }

    while (! m_transitions.empty()) {
        Transition *trans = m_transitions.back();
        m_transitions.pop_back();
        delete trans;
    }

    d(("State[%s] deleted\n", name()));
}

StateMachine *State::stateMachine() {
    if (m_stateMachine) {
        return m_stateMachine;
    }else{
        return parentState()->stateMachine();
    }
}

void State::setInitialState(State *state) {
    vector<State *>::const_iterator i;
    for (i=m_childStates.begin(); i!=m_childStates.end(); ++i) {
        if (state == (*i)) {
            m_initalState = state;
            return;
        }
    }
    assert(0);
}

void State::onEnter(const Event *event, const Transition *transition) {
}

void State::onLeave(const Event *event, const Transition *transition) {
}

void State::addState(State *state) {
    vector<State *>::const_iterator i;
    for (i=m_childStates.begin(); i!=m_childStates.end(); ++i) {
        if (state == (*i)) {
            assert(0);
            return;
        }
    }
    m_childStates.push_back(state);
}

void State::addTransition(Transition *transition) {
    vector<Transition *>::const_iterator i;

    for (i=m_transitions.begin(); i!=m_transitions.end(); ++i) {
        if ((*i) == transition) {
            assert(0);
            return;
        }
    }
    m_transitions.push_back(transition);
}

Transition *State::eventTest(const Event *event) {
    vector<Transition *>::iterator i;
    Transition *trans;

    for (i=m_transitions.begin(); i!=m_transitions.end(); ++i) {
        trans = (*i);
        if (trans->eventTest(event)) {
            if (trans->target()) {
                return trans;
            }else{
                /* XXX: Don't return if it is a targetless transition, and do not reset other transitions */
                trans->action(event);
            }
        }
    }
    return NULL;
}

State *State::enter(const Event *event, const Transition *transition, bool autojump) {
    if (m_entered) {
        printf("renter state:%s transition=%s\n", name(), transition->name());
    }
    assert(! m_entered);
    m_entered = true;

#ifdef DEBUG
    if (depth() > 0) {
        int i;
        printf("+++ Enter State:");
        for (i=depth(); i>0; --i) {
            printf(" ");
        }
        printf("%s\n", name());
    }
#endif

    onEnter(event, transition);

    resetTransitions();

    if (autojump) {
        if (m_initalState) {
            return m_initalState->enter(event, transition, true);
        }else{
            assert(m_childStates.empty());
            return this;
        }
    }else{
        return this;
    }
}

void State::leave(const Event *event, const Transition *transition) {
    if (! m_entered) {
        printf("releave state:%s transition=%s\n", name(), transition->name());
    }
    assert(m_entered);
    m_entered = false;

#ifdef DEBUG
    if (depth() > 0) {
        int i;
        printf("--- Leave State:");
        for (i=depth(); i>0; --i) {
            printf(" ");
        }
        printf("%s\n", name());
    }
#endif

    onLeave(event, transition);
}

void State::resetTransitions() {
    vector<Transition *>::iterator i;

    for (i=m_transitions.begin(); i!=m_transitions.end(); ++i) {
        (*i)->reset();
    }
}

/*
 * Transition
 */
Transition::Transition(State *source, State *target, const char *name)
    : m_name(name), m_source(source), m_target(target)
{
    source->addTransition(this);
}

Transition::~Transition() {
    d(("Transition[%s] deleted\n", name()));
}

void Transition::subscribe(void *object, void (*onTrigger)(void *object, Transition *transition, const Event *, void *param), void *param) {
    struct action o = {object, param, onTrigger};
    m_actions.push_back(o);
}

void Transition::action(const Event* event) {
    vector<struct action>::const_iterator i;
    for (i=m_actions.begin(); i!=m_actions.end(); ++i) {
        i->onTrigger(i->object, this, event, i->param);
    }
}

/* XXX: no use
void Transition::onTransition() {
    d(("Transition[%s] %s\n", name(), __FUNCTION__));
}
*/

/*
 * EventTransition
 */
bool EventTransition::eventTest(const Event *event) {
    assert(event);
    if (! (event->type() & m_eventMask)) {
        return false;
    }
    switch (event->type()) {
        case Event::KeyEvent:
            return testKey((const KeyEvent *)event);
        case Event::MouseEvent:
            if (((MouseEvent*)event)->type() == 2) {
                printf("%s:%d, type=%d\n", __FUNCTION__, event->type(), ((MouseEvent*)event)->type());
            }
            return testMouse((const MouseEvent *)event);
        case Event::MoveEvent:
            return testMove((const MoveEvent *)event);
        case Event::TimerEvent:
            return testTimer((const TimerEvent *)event);
        default:
            assert(0);
            return false;
    }
}

void EventTransition::setButtonType(int type) {
    m_buttonType = type;
}

void EventTransition::setButtons(int buttons) {
    m_buttons = buttons;
}

void EventTransition::setKeyType(int type) {
    m_keyType = type;
}

void EventTransition::setKey(int scancode) {
    m_keys.push_back(scancode);
}

bool EventTransition::testKey(const KeyEvent *event) {
    vector<int>::const_iterator i;
    for (i=m_keys.begin(); i!=m_keys.end(); ++i){
        if ((event->type() & m_keyType) && (event->key() == *i)) {
            return true;
        }
    }
    return false;
}

bool EventTransition::testMouse(const MouseEvent *event) {
    if ((event->type() & m_buttonType) && (event->buttons() & m_buttons)) {
        return true;
    }else{
        return false;
    }
}

SignalTransition::SignalTransition(State *source, State *target, const char *name)
    : Transition(source, target, name), m_sender(NULL), m_signal(0)
{
}

SignalTransition::SignalTransition(const void *sender, const char *signal, State *source, State *target, const char *name)
    : Transition(source, target, name)
{
    setSignal(sender, signal);
}

SignalTransition::~SignalTransition() {
}

void SignalTransition::setSignal(const void *sender, const char *signal) {
    m_sender = sender;
    m_signal = Str2Key(signal);
}

/*
void SignalTransition::emitSignal(SignalTransition *transition) {
    SignalEvent event(transition, NULL);
    State *hostState = transition->hostState();
    StateMachine *machine = hostState->stateMachine();
    machine->processEvent(&event);
}
*/

bool SignalTransition::eventTest(const Event *_event) {
    SignalEvent *event;
    if (_event->type() != Event::SignalEvent) {
        return false;
    }

    event = (SignalEvent *)_event;
    return (event->sender() == m_sender && event->signal() == m_signal);
}

/*
 * StateMachine
 */
StateMachine::StateMachine(HWND hwnd) : m_status(STATEMACHINE_IDLE), m_hwnd(hwnd) {
}

StateMachine::~StateMachine() {
    while (! m_events.empty()) {
        Event *event = m_events.front();
        m_events.erase(m_events.begin());
        delete event;
    }
}

void StateMachine::start() {
    assert(m_status == STATEMACHINE_IDLE);
    m_status = STATEMACHINE_RUNNING;
    m_currentState = m_rootState.enter(NULL, NULL, true);
}

void StateMachine::setInitialState(State *state) {
    assert(m_status == STATEMACHINE_IDLE);
    m_rootState.setInitialState(state);
}

State *StateMachine::currentState() {
    assert(m_status == STATEMACHINE_RUNNING || m_status == STATEMACHINE_PROCESSING);
    return m_currentState;
}

void StateMachine::addState(State *state) {
    m_rootState.addState(state);
    state->m_parentState = &m_rootState;
}

void StateMachine::postEvent(Event *event) {
    m_events.push_back(event);
    SendNotifyMessage(m_hwnd, MSG_USER_STATEMACHINE_EVENT, -1, -1);
}

void StateMachine::onEvent() {
    if (m_status == STATEMACHINE_PROCESSING) {
        return;
    }
    m_status = STATEMACHINE_PROCESSING;
    while (! m_events.empty()) {
        Event *event = m_events.front();
        m_events.erase(m_events.begin());
        processEvent(event);
        delete event;
    }
    m_status = STATEMACHINE_RUNNING;
}

void StateMachine::processEvent(const Event *event) {
    State *old_state, *new_state;
    Transition *trans = NULL;
    int i;
    vector<State *> current_2_transHost;
    vector<State *> target_2_ancestor;
    vector<State *>::iterator iterator;

    assert(m_currentState);
    for (old_state = m_currentState; /* NULL */; old_state = old_state->parentState()) {
        if (old_state->depth() <= 0) {
            return;
        }
        /* XXX: can't leave right now, since don't know wheather there is a proper transition with a not-null target */
        current_2_transHost.push_back(old_state);
        if ((trans = old_state->eventTest(event))) {
            break;
        }
    }

    printf("find transition %s\n", trans->name());
    new_state = trans->target();
    assert(new_state);
    
    for (iterator=current_2_transHost.begin(); iterator!=current_2_transHost.end(); ++iterator) {
        (*iterator)->leave(event, trans);
    }

    for (i=old_state->depth()-new_state->depth(); i>1; --i) {
        old_state = old_state->parentState();
        old_state->leave(event, trans);
    }

    for (i=new_state->depth() - old_state->depth(); i>=0; --i) {
        target_2_ancestor.insert(target_2_ancestor.begin(), new_state);
        new_state = new_state->parentState();
    }

    assert(old_state->depth() == new_state->depth() + 1);

    while (old_state->parentState() != new_state) {
        old_state = old_state->parentState();
        old_state->leave(event, trans);

        target_2_ancestor.insert(target_2_ancestor.begin(), new_state);
        new_state = new_state->parentState();
    }

    if(target_2_ancestor.size() == 0)
    {
        old_state->parentState()->leave(event, trans);
        target_2_ancestor.insert(target_2_ancestor.begin(), new_state);
    }

    trans->action(event);

    for (iterator=target_2_ancestor.begin(); iterator!=target_2_ancestor.end(); ++iterator) {
        if ((*iterator) == trans->target()) {
            m_currentState = (*iterator)->enter(event, trans, true);
        }else{
            (*iterator)->enter(event, trans, false);
        }
    }

    for (i=new_state->depth(); i>0; --i, new_state=new_state->parentState()) {
        new_state->resetTransitions();
    }
}
