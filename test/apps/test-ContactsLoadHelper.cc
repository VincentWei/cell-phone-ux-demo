
#include <gtest/gtest.h>

#include <iostream>
#include <unistd.h>
#define private public
#define protected public
#include "global.h"
#include "ContactsLoadHelper.hh"
#include "ContentResolver.hh"
#include "ContactsProvider.hh"

TEST(ContactsLoadHelper, basicLoad)
{
    const char* data[14][3] = {
        {"Andy", "18666291666", "13345678901"},
        {"A", "010-110", "13511110000"},
        {"Andrew", "13455667788", "13511115555"},
        {"Boost", "818109", "18666291665"},
        {"penny", "294837", "18662282665"},
        {"Amen", "123456", "18522458907"},
        {"Shark", "393837", "18633982665"},
        {"Bee", "13311982665", "13311982665"},
        {"Puppy", "020-33948532", ""},
        {"Pin", "010-31118532", ""},
        {"bunny", "020-03418532", "13643249887"},
        {"Beyond", "23818532", "13553849897"},
        {"betch", "0000", ""},
        {"a", "0000", ""},
    };
    ContentValues cv;
    ContactsProvider* sp = new ContactsProvider;
    GET_CONTENT_RESOLVER()->registerContentProvider(sp);
    for (int i = 0; i < 14; ++i) {
        cv.putString(ContactsProvider::COL_NAME, data[i][0]);
        cv.putString(ContactsProvider::COL_DIAL, data[i][1]);
        cv.putString(ContactsProvider::COL_SMS, data[i][2]);
        GET_CONTENT_RESOLVER()->insert(ContactsProvider::CONTENT_URI, cv);
        cv.clear();
    }

    ContactsGroup group;
    ASSERT_TRUE(0 == LoadGroupContacts(group, "S"));
    EXPECT_EQ(1, group.size());

    group.clear();
    ASSERT_TRUE(0 == LoadGroupContacts(group, "P"));
    EXPECT_EQ(3, group.size());
}

TEST(ContactsLoadHelper, order)
{
    ContactsGroup group;
    size_t j = 0;

    ASSERT_TRUE(0 == LoadGroupContacts(group, "a"));
    EXPECT_EQ(5, group.size());

    group.clear();
    ASSERT_TRUE(0 == LoadGroupContacts(group, "a",
        OPERATE_BY_NAME, OPERATE_BY_NAME, false));
    for (j = 1; j < group.size(); ++j) {
        ASSERT_TRUE(group[j - 1]->name > group[j]->name);
    }

    group.clear();
    ASSERT_TRUE(0 == LoadGroupContacts(group, "a",
        OPERATE_BY_NAME, OPERATE_BY_NAME, true));
    EXPECT_EQ(group[0]->name, "A");
    for (j = 1; j < group.size(); ++j) {
        ASSERT_TRUE(group[j - 1]->name < group[j]->name);
    }
}

int main(int argc, char *argv[])
{
    unlink(SYSTEM_DB_NAME);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return 0;
}

