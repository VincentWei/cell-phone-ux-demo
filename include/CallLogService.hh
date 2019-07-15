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
 * @file CallLogService.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  CALLLOGSERVICE_INC
#define  CALLLOGSERVICE_INC

#include <vector>
#include <string>
class ContentCursor;

enum CallType {
    DIAL_OUT,
    CALL_IN,
    CALL_LOST // Call in but lost.
};

struct CallLogItem {
    std::string m_dial; // dial number
    CallType m_type; // call type
    time_t m_time; // call start time
    size_t m_duration; // call duration time
};

class CallLogService
{
public:
    typedef std::vector<CallLogItem> CallLog;

    static CallLogService* singleton();

    // memory free by user.
    CallLog* getAllCallLog();

    // memory free by user.
    CallLog* getLostCall();
private:
    CallLog* getCallLogByFactor(const std::string& key, const std::string& val);
    CallLogService();

    static CallLogService* s_service;
};

#define GET_CALLLOG_SERVICE() CallLogService::singleton()
#endif   /* ----- #ifndef CALLLOGSERVICE_INC  ----- */

