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
#include <gtest/gtest.h>

#include "global.h"
#include "SettingService.hh"
#include "ContentResolver.hh"
#include "SettingsProvider.hh"

TEST(SettingService, getInstance)
{
    ASSERT_TRUE(NULL != GET_SETTINGSERVICE());
    SettingsProvider* sp = new SettingsProvider;
    GET_CONTENT_RESOLVER()->registerContentProvider(sp);
}

TEST(SettingService, getAllItemNames)
{
    StringArray strs = GET_SETTINGSERVICE()->getAllItemNames();
    EXPECT_EQ(strs.size(), 6);
    if (6 != strs.size()) {
        puts("Wrong item names count, Please execute test_ContentResolver first.");
    }
}

TEST(SettingService, getOptions)
{
    StringArray strs = GET_SETTINGSERVICE()->getAllItemNames();
    EXPECT_EQ(0, GET_SETTINGSERVICE()->getCandidacy(strs[0]).size());
    EXPECT_EQ(4, GET_SETTINGSERVICE()->getCandidacy(strs[3]).size());
    EXPECT_EQ(2, GET_SETTINGSERVICE()->getCandidacy(strs[4]).size());
    EXPECT_EQ(3, GET_SETTINGSERVICE()->getCandidacy(strs[5]).size());
    StringArray candidacy = GET_SETTINGSERVICE()->getCandidacy(strs[4]);
    EXPECT_EQ(candidacy[0], "Line1");
    EXPECT_EQ(candidacy[1], "Line2");
    EXPECT_EQ("on", GET_SETTINGSERVICE()->getCurrent(strs[2]));
    EXPECT_EQ("on", GET_SETTINGSERVICE()->getCurrent(strs[3]));
}

TEST(SettingService, getOptionType)
{
    StringArray strs = GET_SETTINGSERVICE()->getAllItemNames();
    EXPECT_EQ(1, GET_SETTINGSERVICE()->getItemType(strs[0]));
    EXPECT_EQ(2, GET_SETTINGSERVICE()->getItemType(strs[1]));
    EXPECT_EQ(3, GET_SETTINGSERVICE()->getItemType(strs[4]));
}

TEST(SettingService, setOptions)
{
    StringArray strs = GET_SETTINGSERVICE()->getAllItemNames();
    GET_SETTINGSERVICE()->setCurrent(strs[0], "off");
    GET_SETTINGSERVICE()->setCurrent(strs[5], "English");
    EXPECT_EQ("off", GET_SETTINGSERVICE()->getCurrent(strs[0]));
    EXPECT_EQ("English", GET_SETTINGSERVICE()->getCurrent(strs[5]));
}

int main(int argc, const char* argv[]) 
{
    int ret = 0;

    ::testing::InitGoogleTest(&argc, (char **)argv);

    ret = RUN_ALL_TESTS();

    return ret;
}
