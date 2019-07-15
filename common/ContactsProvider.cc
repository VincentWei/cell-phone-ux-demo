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
 * @file ContactsProvider.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <sstream>
#include "global.h"
#include "ContactsProvider.hh"

const char* ContactsProvider::CONTENT_URI = SYSTEM_CONTENT_HEAD "contacts";
const char* ContactsProvider::DB_NAME = SYSTEM_DB_NAME;
const char* ContactsProvider::TABLE_NAME = "contacts";
const char* ContactsProvider::COL_ID = "_ID";
const char* ContactsProvider::COL_NAME = "name";
const char* ContactsProvider::COL_ICON = "icon_path";
const char* ContactsProvider::COL_DIAL = "dial_num";
const char* ContactsProvider::COL_SMS  = "sms_num";
const int   ContactsProvider::COL_TOTAL = 5;

ContactsProvider::ContactsProvider() :
    SQLiteProvider(DB_NAME)
{
    std::stringstream sql_cmd;
    sql_cmd << "CREATE TABLE " << TABLE_NAME << "(" << 
        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
        COL_NAME << " TEXT NOT NULL," <<
        COL_ICON << " TEXT," <<
        COL_DIAL << " TEXT," <<
        COL_SMS << " TEXT" << ");";
    
#ifdef DEBUG
    printf ("ContactsProvider.....................\n");
#endif

    m_db.execSQL(sql_cmd.str());
}

ContactsProvider::~ContactsProvider()
{
#ifdef DEBUG
    printf ("~ContactsProvider.....................\n");
#endif
}

