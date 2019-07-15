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
 * @file test-ContentResolver.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <algorithm>
#include <gtest/gtest.h>
#include <unistd.h>

#define private public
#define protected public
#include "global.h"

#include "ContentResolver.hh"
#include "SettingsProvider.hh"

class ObserverSample : public ContentObserver {
    public:
        static int s_count;
    virtual void onContentChanged() {
        ++s_count;
    }
};

int ObserverSample::s_count = 0;

TEST(ContentResolver, getSingleton)
{
    ASSERT_TRUE(NULL != GET_CONTENT_RESOLVER());
}

TEST(ContentResolver, realSingleton)
{
    ContentResolver* cr = GET_CONTENT_RESOLVER();
    EXPECT_EQ(cr, GET_CONTENT_RESOLVER());
}

TEST(ContentResolver, registerProviderValidity)
{
    GET_CONTENT_RESOLVER()->registerContentProvider(GET_CONTENT_RESOLVER());
    ASSERT_TRUE(GET_CONTENT_RESOLVER()->m_providers.size() == 1);
    GET_CONTENT_RESOLVER()->registerContentProvider(GET_CONTENT_RESOLVER());
    EXPECT_EQ(GET_CONTENT_RESOLVER()->m_providers.size(), 1);
    GET_CONTENT_RESOLVER()->unregisterContentProvider(GET_CONTENT_RESOLVER());
    EXPECT_EQ(GET_CONTENT_RESOLVER()->m_providers.size(), 0);
}

TEST(ContentResolver, matchURIValidity)
{
    GET_CONTENT_RESOLVER()->registerContentProvider(GET_CONTENT_RESOLVER());
    ContentProvider* cp = GET_CONTENT_RESOLVER()->matchProviderByURI(ContentResolver::CONTENT_URI);
    ASSERT_TRUE(GET_CONTENT_RESOLVER() == cp);
}

TEST(ContentResolver, registerObserverValidity)
{
    ObserverSample* os = new ObserverSample();
    GET_CONTENT_RESOLVER()->registerContentObserver("NotExistProvider", os);
    ContentProvider* cp = GET_CONTENT_RESOLVER()->matchProviderByURI("NotExistProvider");
    EXPECT_EQ(NULL, cp);


    GET_CONTENT_RESOLVER()->registerContentObserver(ContentResolver::CONTENT_URI, os);
    cp = GET_CONTENT_RESOLVER()->matchProviderByURI(ContentResolver::CONTENT_URI);
    ASSERT_TRUE(NULL != cp);
    EXPECT_EQ(1, cp->m_observers.size());
    GET_CONTENT_RESOLVER()->registerContentObserver(ContentResolver::CONTENT_URI, os);
    EXPECT_EQ(1, cp->m_observers.size());

    GET_CONTENT_RESOLVER()->unregisterContentObserver("NotExistProvider", os);
    EXPECT_EQ(1, cp->m_observers.size());
    GET_CONTENT_RESOLVER()->unregisterContentObserver(ContentResolver::CONTENT_URI, os);
    EXPECT_EQ(0, cp->m_observers.size());

    GET_CONTENT_RESOLVER()->registerContentObserver(ContentResolver::CONTENT_URI, os);
    EXPECT_EQ(1, cp->m_observers.size());
    GET_CONTENT_RESOLVER()->unregisterContentObserver(os);
    EXPECT_EQ(0, cp->m_observers.size());

    delete os;
}

TEST(ContentResolver, observerValidity)
{
    ObserverSample* os = new ObserverSample();
    ASSERT_TRUE(0 == ObserverSample::s_count);
    GET_CONTENT_RESOLVER()->registerContentObserver(ContentResolver::CONTENT_URI, os);
    GET_CONTENT_RESOLVER()->notifyChange(ContentResolver::CONTENT_URI);
    EXPECT_EQ(1, ObserverSample::s_count);
    GET_CONTENT_RESOLVER()->notifyChange(ContentResolver::CONTENT_URI);
    EXPECT_EQ(2, ObserverSample::s_count);
    GET_CONTENT_RESOLVER()->unregisterContentObserver(ContentResolver::CONTENT_URI, os);

    GET_CONTENT_RESOLVER()->notifyChange(ContentResolver::CONTENT_URI);
    EXPECT_EQ(2, ObserverSample::s_count);
    delete os;
}

#define TYPE_CHECKBOX  1
#define TYPE_INDICATOR 2
#define TYPE_TEXT   3

TEST(ContentResolver, insertValidity)
{
    const char* data[8][3] = {
        {"Clock", "", NULL},
        {"Anykey answer", "on", NULL},
        {"Automatic redial", "off", NULL},
        {"Shock", "on", NULL},
        {"Network selection", "China Mobile", "Automatic#China Mobile#China Unicom#China Telecom"},
        {"Phone line in use", "Line1", "Line1#Line2"},
        {"Language", "English", "Simplified Chinese#English#Spanish"},
        {"Restore factory settings", "", NULL},
    };

    SettingsProvider* sp = new SettingsProvider;
    ContentValues cv;
    GET_CONTENT_RESOLVER()->registerContentProvider(sp);
    for (int i = 0; i < 8; ++i) {
        cv.putString(SettingsProvider::COL_NAME, data[i][0]);
        cv.putString(SettingsProvider::COL_CURRENT, data[i][1]);
        if (NULL != data[i][2]) {
            cv.putString(SettingsProvider::COL_CANDIDACY, data[i][2]);
        } else {
            cv.putInteger(SettingsProvider::COL_TYPE, TYPE_INDICATOR);
        }
        GET_CONTENT_RESOLVER()->insert(SettingsProvider::CONTENT_URI, cv);
        cv.clear();
    }

    ContentCursor* cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, NULL, NULL, NULL, NULL); 
    ASSERT_TRUE(NULL != cur);

    int rowCount = cur->getCount();
    cv.clear();
    cv.putString(SettingsProvider::COL_NAME, "Restore factory settings");
    cv.putInteger(SettingsProvider::COL_TYPE, TYPE_TEXT);
    GET_CONTENT_RESOLVER()->insert(SettingsProvider::CONTENT_URI, cv);
    cur->requery();
    cur->moveToFirst();
    ASSERT_TRUE(NULL != cur);
    EXPECT_EQ(rowCount + 1, cur->getCount());

    EXPECT_EQ(SettingsProvider::COL_TOTAL, cur->getColumnCount());

    EXPECT_EQ("Anykey answer", cur->getString(SettingsProvider::COL_NAME));
    EXPECT_EQ("on", cur->getString(SettingsProvider::COL_CURRENT));
    EXPECT_EQ(1, cur->getInt(SettingsProvider::COL_ID, -1));

    cur->moveToLast();
    cur->moveToPrevious();
    EXPECT_EQ("Restore factory settings", cur->getString(SettingsProvider::COL_NAME));
    EXPECT_EQ("", cur->getString(SettingsProvider::COL_CURRENT));
    EXPECT_EQ(7, cur->getInt(SettingsProvider::COL_ID, -1));
}

TEST(ContentResolver, ColumNameValidity)
{
    ContentCursor* cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
            NULL, NULL, NULL, NULL); 
    ASSERT_TRUE(NULL != cur);
    EXPECT_EQ(SettingsProvider::COL_TOTAL, cur->getColumnCount());
    ContentValues::Strings col_set = cur->getColumnNames();
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_ID);
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_NAME);
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_CURRENT);
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_CANDIDACY);
}

TEST(ContentResolver, buildQueryValidity)
{
    ContentValues::Strings projection;
    projection.push_back(SettingsProvider::COL_ID);

    ContentCursor* cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
            &projection, NULL, NULL, NULL); 
    ASSERT_TRUE(NULL != cur);

    EXPECT_EQ(1, cur->getColumnCount());
    ContentValues::Strings col_set = cur->getColumnNames();
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_ID);

    projection.push_back(SettingsProvider::COL_NAME);
    cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
            &projection, NULL, NULL, NULL); 
    ASSERT_TRUE(NULL != cur);
    EXPECT_EQ(2, cur->getColumnCount());
    col_set = cur->getColumnNames();
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_ID);
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_NAME);

    std::string asc("_ID DESC"); // 降序
    std::string selection("_ID=1");
    cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
            &projection, &selection, NULL, &asc); 
    ASSERT_TRUE(NULL != cur);
    EXPECT_EQ(1, cur->getCount());

    ContentValues::Strings selectionArgs;
    selectionArgs.push_back("2");
    selectionArgs.push_back("1");
    selection = ("_ID=? OR _ID=?");
    cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
            &projection, &selection, &selectionArgs, &asc); 
    ASSERT_TRUE(NULL != cur);
    EXPECT_EQ(2, cur->getCount());
}

TEST(ContentResolver, RequeryValidity)
{
    ContentValues::Strings projection;
    projection.push_back(SettingsProvider::COL_ID);

    projection.push_back(SettingsProvider::COL_NAME);
    ContentCursor* cur1 = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
            &projection, NULL, NULL, NULL); 
    ASSERT_TRUE(NULL != cur1);
    EXPECT_EQ(2, cur1->getColumnCount());

    std::string selection("_ID=? OR _ID=?");
    ContentValues::Strings selectionArgs;
    selectionArgs.push_back("2");
    selectionArgs.push_back("1");
    ContentCursor* cur2 = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
            &projection, &selection, &selectionArgs, NULL); 
    ASSERT_TRUE(NULL != cur2);
    EXPECT_EQ(2, cur2->getCount());

    cur1->requery();
    EXPECT_EQ(2, cur1->getColumnCount());
    ContentValues::Strings col_set = cur1->getColumnNames();
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_ID);
    std::find(col_set.begin(), col_set.end(), SettingsProvider::COL_NAME);
}

TEST(ContentResolver, updateValidity)
{
    std::string where = SettingsProvider::COL_NAME;
    where += "=?";
    ContentValues::Strings args;
    args.push_back("'Anykey answer'");
    ContentValues cv;
    cv.putString(SettingsProvider::COL_CURRENT, "off");
    cv.putInteger(SettingsProvider::COL_TYPE, TYPE_INDICATOR);
    GET_CONTENT_RESOLVER()->update(SettingsProvider::CONTENT_URI, cv, &where, &args);

    ContentCursor* cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, NULL, NULL, NULL, NULL); 
    ASSERT_TRUE(NULL != cur);
    EXPECT_EQ("off", cur->getString(SettingsProvider::COL_CURRENT));
    EXPECT_EQ(TYPE_INDICATOR, cur->getInt(SettingsProvider::COL_TYPE));

    cv.clear();
    cv.putInteger(SettingsProvider::COL_TYPE, TYPE_CHECKBOX);
    GET_CONTENT_RESOLVER()->update(SettingsProvider::CONTENT_URI, cv, &where, &args);

    cv.clear();
    cv.putString(SettingsProvider::COL_CURRENT, "on");
    GET_CONTENT_RESOLVER()->update(SettingsProvider::CONTENT_URI, cv, NULL, NULL);
    cur->requery();
    cur->moveToFirst();
    EXPECT_EQ("on", cur->getString(SettingsProvider::COL_CURRENT));
    cur->moveToNext();
    EXPECT_EQ("on", cur->getString(SettingsProvider::COL_CURRENT));
}


TEST(ContentResolver, deleteValidity)
{
    std::string where = "_ID=?";
    ContentValues::Strings args;
    args.push_back("7");
    GET_CONTENT_RESOLVER()->remove(SettingsProvider::CONTENT_URI, &where, &args);
    ContentCursor* cur = GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, NULL, NULL, NULL, NULL); 
    ASSERT_TRUE(NULL != cur);

    EXPECT_EQ(6, cur->getCount());

//    GET_CONTENT_RESOLVER()->remove(SettingsProvider::CONTENT_URI, NULL, NULL);
//    cur->requery();
//    EXPECT_EQ(0, cur->getCount());
}

int main(int argc, char *argv[])
{
    unlink(SYSTEM_DB_NAME);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return 0;
}
