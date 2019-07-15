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
/*! ============================================================================
 * @file SmsService.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include "common.hh"
#include "ContentResolver.hh"
#include "SmsProvider.hh"
#include "SmsService.hh"

SmsService* SmsService::s_service = 0;

SmsService* SmsService::singleton()
{
    if (NULL == s_service) {
        static SmsService sSmsService;
        s_service = &sSmsService;
        //s_service = new SmsService;
    }
    return s_service;
}

void SmsService::clearSession()
{
    while (m_session.size() > 0) {
        for_each(m_session.begin()->second.begin(),
                m_session.begin()->second.end(), DeleteObject());
        m_session.begin()->second.clear();
        m_session.erase(m_session.begin());
    }

    m_session.clear();
}

const SmsService::SmsSession &SmsService::getAllSession()
{
    std::string orderBy(SmsProvider::COL_SMS_TIME);
    orderBy += " DESC";
    ContentCursor* cur = GET_CONTENT_RESOLVER()->query(SmsProvider::CONTENT_URI,
            NULL, NULL, NULL, &orderBy);

    if (NULL != cur) {
        clearSession();

        for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
            SmsContent *content = new SmsContent;
            if (NULL != content) {
                SmsSession::iterator iter;
                std::string smsNum;

                content->m_smsTime = cur->getInt(SmsProvider::COL_SMS_TIME);
                content->m_smsBody = cur->getString(SmsProvider::COL_SMS_BODY);
                content->m_smsType = static_cast<SmsType>(cur->getInt(SmsProvider::COL_SMS_TYPE));

                smsNum = cur->getString(SmsProvider::COL_SMS_NUM);

                iter = m_session.find(smsNum);

                if (m_session.end() == iter) {
                    SmsContentArray contentArray;
                    contentArray.push_back(content);
                    m_session.insert(m_session.end(), SmsSession::value_type(smsNum.c_str(), contentArray));
                }
                else {
                    //SmsContentArray &contentArray = iter->second;
                    iter->second.push_back(content);
                }
            }
            else {
                assert(0);
            }
        }

        delete cur;
    }
    return m_session;
}

void SmsService::delSession(const std::string& smsNum)
{
    puts("Not Implement.");
}

SmsService::~SmsService()
{
#ifdef DEBUG
    printf ("call ~SmsService\n");
#endif

    clearSession();
}

SmsService::SmsService()
{
#ifdef DEBUG
    printf ("call SmsService\n");
#endif
}

//ContentCursor* SmsService::getContentByNum(const std::string num)
//{
//    return NULL;
//}

