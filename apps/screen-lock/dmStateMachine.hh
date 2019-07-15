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
 * @file dmStateMachine.hh 
 * @Synopsis  
 * @author DengMin
 * @version 1.0
 * @date 26/10/2010 10:45:54 
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef DMSTATEMACHINE_HH
#define DMSTATEMACHINE_HH

#include <stdint.h>
#include <vector>
#include <string>
#include <cassert>

// convenient for state table declaration
#define BEGIN_STATE_DECLARE(tableName) \
    static const intptr_t tableName[] = {

#define DECLARE_STATE(s) \
        s, (intptr_t)#s, 

#define END_STATE_DELCARE \
        0, 0, \
    };

// convenient for transition table declaration
#define BEGIN_TRANSITION_DECLARE(tableName) \
    static const intptr_t tableName[] = {

#define DECLARE_TRANSITION(s1, evt, s2) \
        s1, evt, s2, (intptr_t)#evt, 

#define END_TRANSITION_DELCARE \
        0, 0, 0, 0, \
    };


class dmTransition;
class dmState;

typedef std::vector<dmTransition*> dmTransitionTable;
typedef std::vector<dmState*> dmStateTable;

class dmNamedObj
{
    public:
        dmNamedObj(const char* name) : m_name(name) {}
        virtual ~dmNamedObj() {}

        virtual const char* name() const {return m_name;}
        virtual void name(const char* name) {m_name = name;}
    protected:
        const char* m_name;
};

class dmEvent : public dmNamedObj
{
    public:
        dmEvent(const char* name, intptr_t id) : dmNamedObj(name), m_id(id) {}
        virtual ~dmEvent() {}

        intptr_t id() const {return m_id;}

    protected:
        intptr_t m_id;
};

class dmState : public dmNamedObj
{
    public:
        dmState(const char* name, intptr_t id) : dmNamedObj(name), m_id(id) {}
        virtual ~dmState();

        void addTransition(dmTransition* transition);

        dmTransitionTable* getTransitionTable() {return &m_transitionTable;}

        dmState* processEvent(intptr_t eventId);

        intptr_t id() const {return m_id;}

    protected:
        intptr_t m_id;
        dmTransitionTable m_transitionTable;
};

class dmTransition
{
    public:
        dmTransition(const dmState* source, intptr_t eventId, dmState* target, const char* eventDesc="")
            : m_source(source), m_eventId(eventId), m_target(target), m_eventDesc(eventDesc) {}
        virtual ~dmTransition() {}

        virtual bool transit() {return true;}

        const dmState* source() const {return m_source;}

        const dmState* target() const {return m_target;}
        dmState* target() {return m_target;};

        intptr_t eventId() const {return m_eventId;}

        virtual char* getTransitionDesc(char* descOut, int maxlen);
    protected:
        const dmState* m_source;
        intptr_t m_eventId;
        dmState* m_target;
        const char* m_eventDesc;
};

class dmStateMachine : public dmNamedObj
{
    public:
        dmStateMachine(const char* name="") : dmNamedObj(name), 
            m_currentState(NULL), m_initState(NULL), m_stateChangedFlag(false) {}
        virtual ~dmStateMachine();

        bool isInitialized() {return currentState() != NULL;}

        dmState* currentState() const 
        {
            return m_currentState;
        }
        intptr_t currentStateId() const 
        {
            assert(currentState() != NULL);
            return currentState()->id();
        }

        bool isStateChanged() {return m_stateChangedFlag;}

        dmState* findState(intptr_t stateId);

        dmState* addState(const char* name, intptr_t id);
        void addStateTable(const intptr_t* stateTable);

        dmTransition* addTransition(dmState* source, intptr_t eventId, dmState* target,
                const char* eventDesc="");
        dmTransition* addTransition(intptr_t sourceId, intptr_t eventId, intptr_t targetId, 
                const char* eventDesc="");
        void addTransitionTable(const intptr_t* transitionTable);

        void setInitState(dmState* state)
        {
            assert(state != NULL);
            m_initState = state;
            m_currentState = m_initState;
            m_stateChangedFlag = true;
        }
        void setInitState(intptr_t id)
        {
            setInitState(findState(id));
        }

        bool moveOn(intptr_t eventId);

        void reset()
        {
            setInitState(m_initState);
        }

    protected:
        dmStateTable m_stateTable;
        dmState* m_currentState;
        dmState* m_initState;
        bool m_stateChangedFlag;
};


#endif
