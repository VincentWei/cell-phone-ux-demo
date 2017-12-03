/*! ============================================================================
 * @file CallLogProvider.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <sstream>
#include "global.h"
#include "CallLogProvider.hh"

const char* CallLogProvider::CONTENT_URI = "content://system/callLog";
const char* CallLogProvider::DB_NAME = SYSTEM_DB_NAME;
const char* CallLogProvider::TABLE_NAME = "callLog";
const char* CallLogProvider::COL_ID = "_ID";
const char* CallLogProvider::COL_DIAL = "dial_num";
const char* CallLogProvider::COL_TIME = "dial_time";
const char* CallLogProvider::COL_DURATION = "dial_duration";
const char* CallLogProvider::COL_TYPE = "dial_type";

CallLogProvider::CallLogProvider () :
    SQLiteProvider(DB_NAME) 
{
    std::stringstream sql_cmd;
    sql_cmd << "CREATE TABLE " << TABLE_NAME << "(" << 
        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
        COL_DIAL << " TEXT," <<
        COL_TYPE << " INTEGER NOT NULL DEFAULT(0)," <<
        COL_DURATION << " INTEGER NOT NULL DEFAULT(0)," <<
        COL_TIME << " INTEGER NOT NULL DEFAULT(0)" << ");";
    
#ifdef DEBUG
    printf ("CallLogProvider.....................\n");
#endif

    m_db.execSQL(sql_cmd.str());
}

CallLogProvider::~CallLogProvider()
{
#ifdef DEBUG
    printf ("~CallLogProvider.....................\n");
#endif
}

