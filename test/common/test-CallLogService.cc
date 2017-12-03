/*! ============================================================================
 * @file test-CallLogService.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <gtest/gtest.h>

#include "global.h"
#include "CallLogService.hh"
#include "ContentResolver.hh"
#include "SimpleProviderFactory.hh"
#include "CallLogProvider.hh"

TEST(CallLogService, getSingleton)
{
    ASSERT_TRUE(NULL != GET_CALLLOG_SERVICE());
}

TEST(CallLogService, setup)
{
    CallLogProvider* cp = new CallLogProvider;
    GET_CONTENT_RESOLVER()->registerContentProvider(cp);
    ASSERT_TRUE(GET_CALLLOG_SERVICE());
}

TEST(CallLogService, getData)
{
    CallLogService::CallLog* callLog = 
        GET_CALLLOG_SERVICE()->getLostCall();
    EXPECT_NE(callLog, reinterpret_cast<CallLogService::CallLog*>(NULL));
    EXPECT_EQ(callLog->size(), 2);
    delete callLog;

    callLog = 
        GET_CALLLOG_SERVICE()->getAllCallLog();
    EXPECT_NE(callLog, reinterpret_cast<CallLogService::CallLog*>(NULL));
    EXPECT_EQ(callLog->size(), 12);
    delete callLog;
}

int main(int argc, const char* argv[]) 
{
    int ret = 0;

    ::testing::InitGoogleTest(&argc, (char **)argv);

    ret = RUN_ALL_TESTS();

    return ret;
}
