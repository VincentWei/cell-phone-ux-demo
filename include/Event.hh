/** 
 * @file Event.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef EVENT_HH
#define EVENT_HH

#include <minigui/common.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <vector>
#include "boost/any.hpp"

/** 
 * @synopsis  Event for StateMachine
 */
class Event {
public:
    enum Type{
        KeyEvent = 0x01,
        MouseEvent = 0x02,
        MoveEvent = 0x04,
        TimerEvent = 0x08,
        SignalEvent = 0x10,
    };
    typedef std::vector<boost::any> EventParameters;
    virtual ~Event() {}

    int type() const { return m_type; }
    void appendParameter(const boost::any& param) {
        m_params.push_back(param);
    }
    const EventParameters& parameters() const {
        return m_params;
    }
private:
    EventParameters m_params;
protected:
    Event(int type) : m_type(type) {}
private:
    int m_type;
};

/** 
 * @synopsis  KeyEvent
 */
class KeyEvent : public Event {
public:
    enum Type {
        KeyPressed = 0x01,
        KeyReleased = 0x02
    };
    KeyEvent(Type type, int key) : Event(Event::KeyEvent), m_type(type), m_key(key) {}
    ~KeyEvent(){}
    Type type() const { return m_type; }
    int key() const { return m_key; }

private:
    Type m_type;
    int m_key;
};

/** 
 * @synopsis  MouseEvent
 */
class MouseEvent : public Event {
public:
    enum Type {
        ButtonPressed = 0x01,
        ButtonReleased = 0x02
    };
    enum {
        LeftButton = 0x01,
        RightButton = 0x02,
    };
    MouseEvent(Type type, int x, int y, int buttons) : Event(Event::MouseEvent), m_type(type), m_x(x), m_y(y), m_buttons(buttons) {}
    ~MouseEvent() {}
    Type type() const { return m_type; }
    void point(POINT &pt) const { pt.x = m_x; pt.y = m_y; }
    int buttons() const { return m_buttons; }

private:
    Type m_type;
    int m_x;
    int m_y;
    int m_buttons;
};

/** 
 * @synopsis  MoveEvent
 */
class MoveEvent : public Event {
public:
    MoveEvent(const POINT &oldPos, const POINT &pos) : Event(Event::MoveEvent), m_oldPos(oldPos), m_pos(pos) {}
    ~MoveEvent() {}

    const POINT & oldPos() const { return m_oldPos; }
    const POINT & pos() const { return m_pos; }
private:
    POINT m_oldPos;
    POINT m_pos;
};

/** 
 * @synopsis TimerEvent 
 */
class TimerEvent : public Event {
public:
    TimerEvent(int id, unsigned int tickCount) : Event(Event::TimerEvent), m_id(id), m_tickCount(tickCount) {} 
    ~TimerEvent() {}

    int id() { return m_id; }
    unsigned int tickCount() { return m_tickCount; }
private:
    int m_id;
    unsigned int m_tickCount;
};

/** 
 * @synopsis  SignalEvent
 */
class SignalEvent : public Event {
public:
    SignalEvent(const void *sender, const char *signal) : Event(Event::SignalEvent), m_sender(sender), m_signal(Str2Key(signal)) {}
    const void *sender() const { return m_sender; }
    unsigned long signal() const { return m_signal; }
private:
    const void *m_sender;
    unsigned long m_signal;
};

#endif /* EVENT_HH */
