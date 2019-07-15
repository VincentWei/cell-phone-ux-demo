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
 * @file SmsService.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  SMSSERVICE_INC
#define  SMSSERVICE_INC

#include <map>
#include <vector>
#include <string>
#include "TimeService.hh"

class ContentCursor;

enum SmsType {
    SMS_UNREAD,
    SMS_RECEIVED,
    SMS_SENT
};

struct SmsContent {
    SmsType m_smsType;
    std::string m_smsBody;
    TimeSnapshot m_smsTime;
};

class SmsService
{
public:
    typedef std::vector<SmsContent *> SmsContentArray;
    typedef std::map<std::string, SmsContentArray> SmsSession;

    static SmsService* singleton();

    // The method is not thread safety!
    const SmsSession &getAllSession();

    const SmsContentArray& getSmsList(const std::string& talkToWhom);

    void  delSession(const std::string& smsNum);

    virtual ~SmsService();
private:
    SmsService();
    void  clearSession();

    static SmsService *s_service;
    SmsSession  m_session;
};

#define GET_SMSSERVICE() SmsService::singleton()
#endif   /* ----- #ifndef SMSSERVICE_INC  ----- */

