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
 * @file AppsInfoProvider.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <sstream>
#include "global.h"
#include "AppsInfoProvider.hh"

const char* AppsInfoProvider::CONTENT_URI = SYSTEM_CONTENT_HEAD "apps";
const char* AppsInfoProvider::DB_NAME = SYSTEM_DB_NAME;
const char* AppsInfoProvider::TABLE_NAME = "apps";
const char* AppsInfoProvider::COL_ID = "_ID";
const char* AppsInfoProvider::COL_APP_ACTIVITY = "activity";
const char* AppsInfoProvider::COL_APP_NAME = "name";
const char* AppsInfoProvider::COL_APP_ICON = "icon";

AppsInfoProvider::AppsInfoProvider() :
    SQLiteProvider(DB_NAME)
{
    std::stringstream sql_cmd;
    sql_cmd << "CREATE TABLE " << TABLE_NAME << "(" << 
        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
        COL_APP_ACTIVITY << " TEXT," <<
        COL_APP_NAME << " TEXT," <<
        COL_APP_ICON << " TEXT" <<
        ");";
    
#ifdef DEBUG
    printf ("AppsInfoProvider................\n");
#endif

    m_db.execSQL(sql_cmd.str());
}

AppsInfoProvider::~AppsInfoProvider()
{
#ifdef DEBUG
    printf ("~AppsInfoProvider................\n");
#endif
}
