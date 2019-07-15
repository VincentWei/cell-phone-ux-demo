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

dmState* dmState::processEvent(intptr_t eventId)
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
            dmTransition* tmp = transTable->back();
            transTable->pop_back();
            delete tmp;
        }

        delete state;
        m_stateTable.pop_back();
    }
}

dmState* dmStateMachine::addState(const char* name, intptr_t id)
{
    assert(name != NULL);
    dmState* newState = new dmState(name, id);
    m_stateTable.push_back(newState);

    return newState;
}

dmState* dmStateMachine::findState(intptr_t stateId)
{
    dmState* found = NULL;
    dmStateTable::iterator it;
    for (it = m_stateTable.begin(); it != m_stateTable.end(); ++it) {
        if ((*it)->id() == stateId)
            found = (*it);
    }
    return found;
}

void dmStateMachine::addStateTable(const intptr_t* stateTable)
{
    const intptr_t* p = stateTable;
    intptr_t id;
    char* name;

    while (1) {
        id = (intptr_t)*p;
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

dmTransition* dmStateMachine::addTransition(dmState* source, intptr_t eventId, dmState* target, 
        const char* eventDesc)
{
    dmTransition* transition = new dmTransition(source, eventId, target, eventDesc);
    source->addTransition(transition);
    return transition;
}

dmTransition* dmStateMachine::addTransition(intptr_t sourceId, intptr_t eventId, intptr_t targetId, 
        const char* eventDesc)
{
    dmState* source = findState(sourceId);
    dmState* target = findState(targetId);
    if (source == NULL || target == NULL) 
        return NULL;
    return addTransition(source, eventId, target, eventDesc);
}

void dmStateMachine::addTransitionTable(const intptr_t* transitionTable)
{
    const intptr_t* p = transitionTable;
    intptr_t sourceId, eventId, targetId;
    char* eventDesc;

    while (1) {
        sourceId = *p;
        ++p;

        eventId = *p;
        ++p;

        targetId = *p;
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

bool dmStateMachine::moveOn(intptr_t eventId)
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

