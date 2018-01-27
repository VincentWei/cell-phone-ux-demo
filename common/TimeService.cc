
#include "TimeService.hh"

#define USE_MY_EVENT    1

#include <mgncs/mgncs.h>

#define TIME_UNIT       10  // millisecond
#define TIME_PRECISION  1  // 10 milliseconds
#define TICKETS_ONE_SECOND  (1000/(TIME_PRECISION*TIME_UNIT))

TimeService* TimeService::s_service = NULL;

typedef struct {
    list_t list;
    DWORD target;
    int event;
    TIMESERVICE_ONEVENT_CB handler;
} event_handler_t;

// class TimeService
// {
//    public:

// get the single instance of TimeService
TimeService* TimeService::singleton()
{
    if (NULL == TimeService::s_service) {
        TimeService::s_service = new TimeService();
    }
    return TimeService::s_service;
}

/*
const struct tm* TimeService::get() const
{
    return &m_time;
}
*/

const int TimeService::get(int type) const 
{
    return m_time.get(type);
}

void raiseEvent(list_t* pEventHandlers, int eventId, DWORD param)
{
    // broadcast event to every registered target
    list_t *list;
    list_for_each(list, pEventHandlers) {
        event_handler_t *entry = (event_handler_t *)list;
        if (entry->event == eventId) {
            entry->handler((void*)entry->target, TIMESERVICE, eventId, param);
        }
    }
}

bool TimeService::set(int type, int val)
{
    bool ret = false;
    time_t old_time = m_time.get(TimeService::SEC_FROM_EPOCH);

    ret = m_time.set(type, val);

    m_diff += m_time.get(TimeService::SEC_FROM_EPOCH) - old_time;

    if (ret) {
#if USE_MY_EVENT
        raiseEvent(&this->m_eventHandler, type, 0);
#else
        ncsRaiseEvent((mObject*)this, type, 0);
#endif
    }
    
    return ret;
}

void TimeService::reset()
{
    m_time.reset();

    m_ticket = 0;

    m_diff = 0;
}

size_t TimeService::format(char* s, size_t max, const char* fmt) const
{
    return m_time.format(s, max, fmt);
}

BOOL TimeService::addEventListener(void* target, int eventId, TIMESERVICE_ONEVENT_CB onEvent)
{
#if USE_MY_EVENT
    event_handler_t *entry;
    entry = (event_handler_t *)calloc(1, sizeof(*entry));
    entry->target = (DWORD) target;
    entry->event = eventId;
    entry->handler = onEvent;
    list_add(&entry->list, m_eventHandler.prev);

    return TRUE;
#else
    return ncsAddEventListener((mObject*)this, (mObject*)target, (NCS_CB_ONOBJEVENT)onEvent, eventId);
#endif
}

BOOL TimeService::removeEventListener(void* target)
{
#if USE_MY_EVENT
    BOOL ret = FALSE;
    event_handler_t *entry = NULL;
    list_t *i = NULL, *n = NULL;
    list_for_each_safe(i, n, &m_eventHandler) {
        entry = (event_handler_t *)i;
        if (entry->target == (DWORD)target) {
            list_del(i);
            free(i);
            ret = TRUE;
        }
    }
    return ret;
#else
    return ncsRemoveEventListener((mObject*)target);
#endif
}

// private:
TimeService::TimeService()
{
    reset();

    INIT_LIST_HEAD(&m_eventHandler);

    SetTimerEx((HWND)this, (LINT)this, TIME_PRECISION, timerProc);
}

BOOL TimeService::timerProc(HWND hwnd, LINT id, DWORD count)
{
    TimeService* _this = (TimeService*) id;

    // if (++_this->m_ticket >= TICKETS_ONE_SECOND) {
    //     struct tm prev_tm;
    //     struct tm current_tm;

    //     _this->m_time.localtime(&prev_tm);
    //     _this->m_time.inc();
    //     _this->m_time.localtime(&current_tm);

    //     _this->m_ticket = 0;
    
    {
        struct tm prev_tm;
        struct tm current_tm;

        _this->m_time.localtime(&prev_tm);
        _this->m_time.set(TimeService::SEC_FROM_EPOCH, time(NULL) + _this->m_diff);
        _this->m_time.localtime(&current_tm);

        if (current_tm.tm_sec != prev_tm.tm_sec) {
#if USE_MY_EVENT
            raiseEvent(&_this->m_eventHandler, SECOND, 0);
#else
            ncsRaiseEvent((mObject*)_this, SECOND, 0);
#endif
        }

        if (current_tm.tm_min != prev_tm.tm_min) {
#if USE_MY_EVENT
            raiseEvent(&_this->m_eventHandler, MINUTE, 0);
#else
            ncsRaiseEvent((mObject*)_this, MINUTE, 0);
#endif
        }

        if (current_tm.tm_hour != prev_tm.tm_hour) {
#if USE_MY_EVENT
            raiseEvent(&_this->m_eventHandler, HOUR, 0);
#else
            ncsRaiseEvent((mObject*)_this, HOUR, 0);
#endif
        }

        if (current_tm.tm_mday != prev_tm.tm_mday) {
#if USE_MY_EVENT
            raiseEvent(&_this->m_eventHandler, MDAY, 0);
#else
            ncsRaiseEvent((mObject*)_this, MDAY, 0);
#endif
        }

        if (current_tm.tm_mon != prev_tm.tm_mon) {
#if USE_MY_EVENT
            raiseEvent(&_this->m_eventHandler, MONTH, 0);
#else
            ncsRaiseEvent((mObject*)_this, MONTH, 0);
#endif
        }

        if (current_tm.tm_year != prev_tm.tm_year) {
#if USE_MY_EVENT
            raiseEvent(&_this->m_eventHandler, YEAR, 0);
#else
            ncsRaiseEvent((mObject*)_this, YEAR, 0);
#endif
        }
    }

    return TRUE;
}

TIMESERVICE_ONEVENT_CB TimeService::findEventHandler(DWORD target, int eventId)
{
    list_t *list;
    list_for_each(list, &m_eventHandler) {
        event_handler_t *entry = (event_handler_t *)list;
        if (entry->target == target && entry->event == eventId) {
            return entry->handler;
        }
    }
    return NULL;
}

// };

// class TimeSnapshot
// {
//    public:
const int TimeSnapshot::get(int type) const
{
    struct tm tm;
    localtime(&tm);

    switch (type) {
    case TimeService::SECOND:
        return tm.tm_sec;
        break;
    case TimeService::MINUTE:
        return tm.tm_min;
        break;
    case TimeService::HOUR:
        return tm.tm_hour;
        break;
    case TimeService::MDAY:
        return tm.tm_mday;
        break;
    case TimeService::MONTH:
        return tm.tm_mon + 1;
        break;
    case TimeService::YEAR:
        return tm.tm_year + 1900;
        break;
    case TimeService::SEC_FROM_EPOCH:
        return m_timeInSeconds;
        break;

    case TimeService::WDAY:
        return tm.tm_wday;
        break;
    default:
        assert(0);
        break;
    }
}

bool TimeSnapshot::set(int type, int val)
{
    struct tm tm;
    time_t new_timeInSeconds;

    localtime(&tm);

    switch (type) {
    case TimeService::SECOND:
        if (val != tm.tm_sec) {
            tm.tm_sec = val;
        }
        break;
    case TimeService::MINUTE:
        if (val != tm.tm_min) {
            tm.tm_min = val;
        }
        break;
    case TimeService::HOUR:
        if (val != tm.tm_hour) {
            tm.tm_hour = val;
        }
        break;
    case TimeService::MDAY:
        if (val != tm.tm_mday) {
            tm.tm_mday = val;
        }
        break;
    case TimeService::MONTH:
        if (val - 1 != tm.tm_mon) {
            tm.tm_mon = val - 1;
        }
        break;
    case TimeService::YEAR:
        if (val - 1900 != tm.tm_year) {
            tm.tm_year = val - 1900;
        }
        break;
            
    case TimeService::SEC_FROM_EPOCH:
        new_timeInSeconds = val;
        break;
        
    default:
        assert(0);
        break;
    }

    if (type != TimeService::SEC_FROM_EPOCH)
        new_timeInSeconds = mktime(&tm);

    if (m_timeInSeconds == new_timeInSeconds)
        return false;
    else {
        m_timeInSeconds = new_timeInSeconds;
        if (m_timeInSeconds <= 0)
            m_timeInSeconds = 0;
        return true;
    }
}

size_t TimeSnapshot::format(char* s, size_t max, const char* fmt) const
{
    struct tm tm;

    localtime(&tm);

    assert(strstr(fmt, "%F") == NULL);

    return strftime(s, max, fmt, &tm);
}

bool TimeSnapshot::inSameDay(const TimeSnapshot* first, const TimeSnapshot* second)
{
    return (first->get(TimeService::YEAR) == second->get(TimeService::YEAR)
            && first->get(TimeService::MONTH) == second->get(TimeService::MONTH)
            && first->get(TimeService::MDAY) == second->get(TimeService::MDAY));
}

void TimeSnapshot::inc()
{
    ++ m_timeInSeconds;
    if (m_timeInSeconds < 0)
        m_timeInSeconds = 0;
}

void TimeSnapshot::reset()
{
    m_timeInSeconds = time(NULL);
}

struct tm* TimeSnapshot::localtime(struct tm* tm) const
{
#ifndef WIN32
    return localtime_r(&m_timeInSeconds, tm);
#else
	struct tm *tmp = ::localtime(&m_timeInSeconds);
    memcpy(tm, tmp, sizeof(*tm));
    return tm;
#endif
}

//    protected:
TimeSnapshot::TimeSnapshot()
{
    TimeSnapshot(time(NULL));
}

TimeSnapshot::TimeSnapshot(time_t t)
{
    assert (t >= 0);
    m_timeInSeconds = t;
}
// };
