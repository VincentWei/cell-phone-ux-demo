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
 * @file MediaStoreProvider.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <sstream>
#include "global.h"
#include "MediaStoreProvider.hh"

const char* MediaStoreProvider::CONTENT_URI = "content://system/MediaStore/";
const char* MediaStoreProvider::DB_NAME = SYSTEM_DB_NAME;
const char* MediaStoreProvider::TABLE_ALBUM = "albums";
const char* MediaStoreProvider::TABLE_SONGS = "songs";
const char* MediaStoreProvider::COL_ID = "_ID";
const char* MediaStoreProvider::COL_ALBUM = "album";
const char* MediaStoreProvider::COL_SONG = "song";
const char* MediaStoreProvider::COL_AUTHOR = "author";
const char* MediaStoreProvider::COL_LOGO = "logo";
const char* MediaStoreProvider::COL_DURATION = "duration";

MediaStoreProvider::MediaStoreProvider() : 
    SQLiteProvider(DB_NAME) 
{
    std::stringstream sql_cmd;

#ifdef DEBUG
    printf ("MediaStoreProvider................\n");
#endif

    sql_cmd << "CREATE TABLE " << TABLE_ALBUM << "(" << 
        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
        COL_ALBUM << " TEXT," <<
        COL_AUTHOR << " TEXT," <<
        COL_LOGO << " TEXT" << ");";
    
    m_db.execSQL(sql_cmd.str());

    sql_cmd.str("");
    sql_cmd << "CREATE TABLE " << TABLE_SONGS << "(" << 
        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
        COL_ALBUM << " TEXT," <<
        COL_SONG << " TEXT," <<
        COL_DURATION << " INTEGER NOT NULL DEFAULT(320)" << ");";

    m_db.execSQL(sql_cmd.str());
}

MediaStoreProvider::~MediaStoreProvider()
{
#ifdef DEBUG
    printf ("~MediaStoreProvider................\n");
#endif
}

