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
/** 
 * @file StateMachine.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <string>
#include <vector>
#include "Event.hh"

namespace mStateMachine {

class Transition;
class StateMachine;

/** 
 * @synopsis  State
 */
class State {
public:
    State(StateMachine *stateMachine, const char *name="");
    State(State *parentState, const char *name="");
    virtual ~State();

    const char *name() const { return m_name.c_str(); }
    State *parentState() { return m_parentState; }
    StateMachine *stateMachine();
    int depth() const { return m_depth; }
    void setInitialState(State *state);
    void addTransition(Transition *transition);
protected:
    virtual void onEnter(const Event *event, const Transition *transition);
    virtual void onLeave(const Event *event, const Transition *transition);
private:
    State(); /* XXX: for root state only */
    void addState(State *childState);
    Transition *eventTest(const Event *);
    State *enter(const Event *event, const Transition *transition, bool autojump);
    void leave(const Event *event, const Transition *transition);
    void resetTransitions();
    friend class StateMachine;
private:
    std::string m_name;
    std::vector<Transition *> m_transitions;
    std::vector<State *> m_childStates;
    StateMachine *m_stateMachine;
    State *m_parentState;
    State *m_initalState;
    int m_depth;
    bool m_entered;
};

/** 
 * @synopsis  Transition
 */
class Transition {
public:
    virtual ~Transition();

    State *source() { return m_source; }
    State *target() { return m_target; }
    const char *name() const { return m_name.c_str(); }
    void subscribe(void *object, void (*onTrigger)(void *object, Transition *transition, const Event *, void *param), void *param);
    virtual void reset() {}
protected:
    Transition(State *source, State *target, const char *name = "");
    friend class State;
    virtual bool eventTest(const Event *) { return true; }
    /* XXX: no use, so comment it
    virtual void onTransition();
    */
private:
    friend class StateMachine;
    void action(const Event * );
protected:
    std::string m_name;
private:
    State *m_source;
    State *m_target;
    struct action {
        void *object;
        void *param;
        void (*onTrigger)(void *, Transition *, const Event*, void *);
    };
    std::vector<struct action> m_actions;
};

/** 
 * @synopsis EventTransition 
 */
class EventTransition : public Transition {
public:
    EventTransition(int eventMask, State *source, State *target, const char *name="") : Transition(source, target, name), m_eventMask(eventMask) {}
    void setButtonType(int type);
    void setButtons(int buttons);
    void setKeyType(int type);
    void setKey(int scancode);
protected:
    virtual bool eventTest(const Event *);
    virtual bool testKey(const KeyEvent *);
    virtual bool testMouse(const MouseEvent *);
    virtual bool testMove(const MoveEvent *) { return true; }
    virtual bool testTimer(const TimerEvent *) { return false; }
protected:
    int m_buttonType;
    int m_buttons;
    int m_keyType;
    std::vector<int> m_keys;
private:
    int m_eventMask;
};

/** 
 * @synopsis  SignalTransition
 */
class SignalTransition : public Transition {
public:
    SignalTransition(State *source, State *target, const char *name);
    ~SignalTransition();
    virtual void setSignal(const void *sender, const char *signal);
protected:
    SignalTransition(const void *sender, const char *signal, State *source, State *target, const char *name="");
    virtual bool eventTest(const Event *);
private:
    const void *m_sender;
    unsigned long m_signal;
};

/** 
 * @synopsis  StateMachine
 */
class StateMachine {
public:
    StateMachine(HWND hwnd=NULL);
    ~StateMachine();

    void setInitialState(State *state);
    void start();
    State *currentState();
    void addState(State *state);
    void processEvent(const Event *event);
    void postEvent(Event *event);
    void onEvent();
private:
    State m_rootState;
    State *m_currentState;
    enum {
        STATEMACHINE_IDLE,
        STATEMACHINE_RUNNING,
        STATEMACHINE_PROCESSING,
        STATEMACHINE_FINISHED
    };
    int m_status;
    HWND m_hwnd;
    std::vector<Event *> m_events;
};

};

#endif /* STATEMACHINE_H */
