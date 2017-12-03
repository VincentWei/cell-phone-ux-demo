/*! ============================================================================
 * @file test-SmsService.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <gtest/gtest.h>

#include "global.h"
#include "SmsService.hh"
#include "ContentResolver.hh"
#include "SimpleProviderFactory.hh"
#include "SmsProvider.hh"

TEST(SmsService, getSingleton)
{
    ASSERT_TRUE(NULL != GET_SMSSERVICE());
}

TEST(SmsService, setup)
{
    SmsProvider* sp = new SmsProvider;
    GET_CONTENT_RESOLVER()->registerContentProvider(sp);
    ASSERT_TRUE(GET_SMSSERVICE());
}

TEST(SmsService, getData)
{
    const SmsService::SmsSession& session = 
        GET_SMSSERVICE()->getAllSession();
    SmsService::SmsSession::const_iterator iter = session.begin();
    for (; session.end() != iter; ++iter) {
        const SmsService::SmsContentArray& smsArray = iter->second;
        printf("========== session with %s =========\n", iter->first.c_str());
        for (SmsService::SmsContentArray::const_iterator it = smsArray.begin();
                smsArray.end() != it; ++it) {
            printf("[%u] %s : %s\n", (*it)->m_smsTime, 
                    (*it)->m_smsType == 2 ? "me" : "other side", 
                    (*it)->m_smsBody.c_str());
        }
    }
}

TEST(SmsService, delSession)
{
}

int main(int argc, const char* argv[]) 
{
    int ret = 0;

    ::testing::InitGoogleTest(&argc, (char **)argv);

    ret = RUN_ALL_TESTS();

    return ret;
}
