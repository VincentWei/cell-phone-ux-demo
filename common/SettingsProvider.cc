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
 * @file SettingsProvider.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <sstream>
#include "global.h"
#include "SettingsProvider.hh"

const char* SettingsProvider::CONTENT_URI = SYSTEM_CONTENT_HEAD "settings";
const char* SettingsProvider::DB_NAME = SYSTEM_DB_NAME;
const char* SettingsProvider::TABLE_NAME = "settings";
const char* SettingsProvider::COL_ID = "_ID";
const char* SettingsProvider::COL_NAME = "name";
const char* SettingsProvider::COL_TYPE = "type";
const char* SettingsProvider::COL_CURRENT = "current";
const char* SettingsProvider::COL_CANDIDACY = "candidacy";
const int   SettingsProvider::COL_TOTAL = 5;

SettingsProvider::SettingsProvider() :
    SQLiteProvider(DB_NAME, true)
{
    std::stringstream sql_cmd(std::stringstream::in | std::stringstream::out);
    sql_cmd << "CREATE TABLE " << TABLE_NAME << "(" << 
        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
        COL_NAME << " TEXT NOT NULL," <<
        COL_TYPE << " INTEGER NOT NULL DEFAULT (3)," <<
        COL_CURRENT << " TEXT," <<
        COL_CANDIDACY << " TEXT" << ");";
    
#ifdef DEBUG
    printf ("SettingsProvider................\n");
#endif

    m_db.execSQL(sql_cmd.str());
}

SettingsProvider::~SettingsProvider()
{
#ifdef DEBUG
    printf ("~SettingsProvider................\n");
#endif
}

