/*! ============================================================================
 * @file SmsProvider.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <sstream>
#include "global.h"
#include "SmsProvider.hh"

const char* SmsProvider::CONTENT_URI = "content://system/SMS";
const char* SmsProvider::DB_NAME = SYSTEM_DB_NAME;
const char* SmsProvider::TABLE_NAME = "SMS" ;
const char* SmsProvider::COL_ID = "_ID";
const char* SmsProvider::COL_SMS_NUM = "sms_num";
const char* SmsProvider::COL_SMS_BODY = "sms_body";
const char* SmsProvider::COL_SMS_TIME = "sms_time";
const char* SmsProvider::COL_SMS_TYPE = "sms_type";

SmsProvider::SmsProvider() :
    SQLiteProvider(DB_NAME)
{
    std::stringstream sql_cmd;
    sql_cmd << "CREATE TABLE " << TABLE_NAME << "(" << 
        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
        COL_SMS_NUM << " TEXT NOT NULL DEFAULT(0)," <<
        COL_SMS_BODY << " TEXT," <<
        COL_SMS_TIME << " INTEGER NOT NULL DEFAULT(0)," <<
        COL_SMS_TYPE << " INTEGER NOT NULL DEFAULT(0)" << ");";
    
#ifdef DEBUG
    printf ("SmsProvider................\n");
#endif

    m_db.execSQL(sql_cmd.str());

//    sql_cmd.str("");
//    sql_cmd << "CREATE TABLE " << TABLE_NAME << "(" << 
//        COL_ID << " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," <<
//        COL_SMS_NUM << " TEXT NOT NULL DEFAULT(0)," <<
//        COL_SMS_BODY << " TEXT," <<
//        COL_SMS_TIME << " INTEGER NOT NULL DEFAULT(0)" << ");";
//    
//    m_db.execSQL(sql_cmd.str());
}

SmsProvider::~SmsProvider()
{
#ifdef DEBUG
    printf ("~SmsProvider................\n");
#endif
}
