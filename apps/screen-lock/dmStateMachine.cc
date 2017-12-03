
#include "dmStateMachine.hh"
#include <cstdio>

#define DEBUG_ON    0

dmState::~dmState()
{
}

void dmState::addTransition(dmTransition* transition)
{
    m_transitionTable.push_back(transition);
}

dmState* dmState::processEvent(int eventId)
{
    dmState* nextState = NULL;
    dmTransitionTable::iterator it;
    for (it = m_transitionTable.begin(); it != m_transitionTable.end(); ++it) {
        dmTransition* t = *it;
        if (t->eventId() == eventId) {
            if (t->transit()) {
                nextState = t->target();
#if DEBUG_ON
                char desc[128];
                printf("+++ MOVE: %s\n", t->getTransitionDesc(desc, 128));
#endif
            }
        }
    }
    return nextState;
}

char* dmTransition::getTransitionDesc(char* descOut, int maxlen)
{
    if (maxlen > 0) {
#ifndef WIN32
        snprintf(descOut, maxlen, "[%s -> %s on %s]", 
                m_source->name(), m_target->name(), m_eventDesc);
#else
        _snprintf(descOut, maxlen, "[%s -> %s on %s]", 
                m_source->name(), m_target->name(), m_eventDesc);
#endif
    }

    return descOut;
}

dmStateMachine::~dmStateMachine()
{
    while (m_stateTable.size() != 0) {
        dmState* state = m_stateTable.back();
        dmTransitionTable* transTable = state->getTransitionTable();

        while (transTable->size() != 0) {
            delete transTable->back();
            transTable->pop_back();
        }

        delete state;
        m_stateTable.pop_back();
    }
}

dmState* dmStateMachine::addState(const char* name, int id)
{
    assert(name != NULL);
    dmState* newState = new dmState(name, id);
    m_stateTable.push_back(newState);

    return newState;
}

dmState* dmStateMachine::findState(int stateId)
{
    dmState* found = NULL;
    dmStateTable::iterator it;
    for (it = m_stateTable.begin(); it != m_stateTable.end(); ++it) {
        if ((*it)->id() == stateId)
            found = (*it);
    }
    return found;
}

void dmStateMachine::addStateTable(const int* stateTable)
{
    const int* p = stateTable;
    int id;
    char* name;

    while (1) {
        id = (int)*p;
        ++p;

        name = (char*)*p;
        ++p;

        if (id == 0 && name == 0)
            break;

        dmState* pNewState = addState(name, id);
        assert(pNewState);

#if DEBUG_ON
        printf("add state: %s\n", pNewState->name());
#endif
    }
}

dmTransition* dmStateMachine::addTransition(dmState* source, int eventId, dmState* target, 
        const char* eventDesc)
{
    dmTransition* transition = new dmTransition(source, eventId, target, eventDesc);
    source->addTransition(transition);
    return transition;
}

dmTransition* dmStateMachine::addTransition(int sourceId, int eventId, int targetId, 
        const char* eventDesc)
{
    dmState* source = findState(sourceId);
    dmState* target = findState(targetId);
    if (source == NULL || target == NULL) 
        return NULL;
    return addTransition(source, eventId, target, eventDesc);
}

void dmStateMachine::addTransitionTable(const int* transitionTable)
{
    const int* p = transitionTable;
    int sourceId, eventId, targetId;
    char* eventDesc;

    while (1) {
        sourceId = (int)*p;
        ++p;

        eventId = (int)*p;
        ++p;

        targetId = (int)*p;
        ++p;

        eventDesc = (char*)*p;
        ++p;

        if (sourceId == 0 && eventId == 0 && targetId == 0 && eventDesc == NULL)
            break;

        //printf("add transition: %d->%d %d %s\n", sourceId, targetId, eventId, eventDesc);

        dmTransition* pNewTrans = addTransition(sourceId, eventId, targetId, eventDesc);
        assert(pNewTrans);

#if DEBUG_ON
        char desc[128];
        printf("add transition: %s\n", pNewTrans->getTransitionDesc(desc, 128));
#endif
    }
}

bool dmStateMachine::moveOn(int eventId)
{
    m_stateChangedFlag = false;

    assert(isInitialized());
    dmState* newState = m_currentState->processEvent(eventId);
    if (newState != NULL) {
        m_currentState = newState;
        m_stateChangedFlag = true;
    }
#if DEBUG_ON
    else {
        printf("--- STAY: [%s on evt:%d]\n", currentState()->name(), eventId);
    }
#endif
    return m_stateChangedFlag;
}

