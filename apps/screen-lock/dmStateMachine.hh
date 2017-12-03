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

#include <vector>
#include <string>
#include <cassert>

// convenient for state table declaration
#define BEGIN_STATE_DECLARE(tableName) \
    static const int tableName[] = {

#define DECLARE_STATE(s) \
        s, (int)#s, 

#define END_STATE_DELCARE \
        0, 0, \
    };

// convenient for transition table declaration
#define BEGIN_TRANSITION_DECLARE(tableName) \
    static const int tableName[] = {

#define DECLARE_TRANSITION(s1, evt, s2) \
        s1, evt, s2, (int)#evt, 

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
        dmEvent(const char* name, int id) : dmNamedObj(name), m_id(id) {}
        virtual ~dmEvent() {}

        int id() const {return m_id;}

    protected:
        int m_id;
};

class dmState : public dmNamedObj
{
    public:
        dmState(const char* name, int id) : dmNamedObj(name), m_id(id) {}
        virtual ~dmState();

        void addTransition(dmTransition* transition);

        dmTransitionTable* getTransitionTable() {return &m_transitionTable;}

        dmState* processEvent(int eventId);

        int id() const {return m_id;}

    protected:
        int m_id;
        dmTransitionTable m_transitionTable;
};

class dmTransition
{
    public:
        dmTransition(const dmState* source, int eventId, dmState* target, const char* eventDesc="")
            : m_source(source), m_eventId(eventId), m_target(target), m_eventDesc(eventDesc) {}
        ~dmTransition() {}

        virtual bool transit() {return true;}

        const dmState* source() const {return m_source;}

        const dmState* target() const {return m_target;}
        dmState* target() {return m_target;};

        int eventId() const {return m_eventId;}

        virtual char* getTransitionDesc(char* descOut, int maxlen);
    protected:
        const dmState* m_source;
        int m_eventId;
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
        int currentStateId() const 
        {
            assert(currentState() != NULL);
            return currentState()->id();
        }

        bool isStateChanged() {return m_stateChangedFlag;}

        dmState* findState(int stateId);

        dmState* addState(const char* name, int id);
        void addStateTable(const int* stateTable);

        dmTransition* addTransition(dmState* source, int eventId, dmState* target,
                const char* eventDesc="");
        dmTransition* addTransition(int sourceId, int eventId, int targetId, 
                const char* eventDesc="");
        void addTransitionTable(const int* transitionTable);

        void setInitState(dmState* state)
        {
            assert(state != NULL);
            m_initState = state;
            m_currentState = m_initState;
            m_stateChangedFlag = true;
        }
        void setInitState(int id)
        {
            setInitState(findState(id));
        }

        bool moveOn(int eventId);

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
