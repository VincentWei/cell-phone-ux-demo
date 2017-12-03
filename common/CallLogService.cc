/*! ============================================================================
 * @file CallLogService.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include <sstream>
#include "ContentResolver.hh"
#include "CallLogProvider.hh"
#include "CallLogService.hh"

CallLogService* CallLogService::s_service;

CallLogService* CallLogService::singleton()
{
    if (NULL == s_service) {
        s_service = new CallLogService;
    }
    return s_service;
}

CallLogService::CallLog* CallLogService::getCallLogByFactor(const std::string& key, const std::string& val)
{
    ContentCursor* cur = NULL;
    std::string orderBy(CallLogProvider::COL_TIME);
    CallLog* result = NULL;
    orderBy += " DESC";
    if ((!key.empty()) && (!val.empty())) {
        std::stringstream where;
        where << key << "=" << val;
        std::string whereDup(where.str());
        cur = GET_CONTENT_RESOLVER()->query(CallLogProvider::CONTENT_URI,
                NULL, &whereDup, NULL, &orderBy);
    }
    else {
        cur = GET_CONTENT_RESOLVER()->query(CallLogProvider::CONTENT_URI,
                NULL, NULL, NULL, &orderBy);
    }
    if (NULL != cur) {
        result  = new CallLog;
        if (NULL != result) {
            for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
                result->push_back(CallLogItem());
                result->back().m_dial = cur->getString(CallLogProvider::COL_DIAL);
                result->back().m_time = cur->getInt(CallLogProvider::COL_TIME);
                result->back().m_duration = cur->getInt(CallLogProvider::COL_DURATION);
                result->back().m_type = static_cast<CallType>(cur->getInt(CallLogProvider::COL_TYPE));
            }
        }
        else {
            assert(0);
        }

        delete cur;
    }
    else {
        assert(0);
    }

    return result;
}

CallLogService::CallLog* CallLogService::getAllCallLog()
{
    return getCallLogByFactor("", "");
}

CallLogService::CallLog* CallLogService::getLostCall()
{
    std::string value(1, '0' + CALL_LOST);
    return getCallLogByFactor(CallLogProvider::COL_TYPE, value);
}

CallLogService::CallLogService()
{
}

