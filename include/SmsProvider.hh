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
/*!============================================================================
 * @file SmsProvider.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  SMSPROVIDER_INC
#define  SMSPROVIDER_INC

#include "SQLiteProvider.hh"

class SmsProvider : public SQLiteProvider {
public:
    static const char* CONTENT_URI;
    static const char* DB_NAME;
    static const char* TABLE_NAME;
    static const char* COL_ID;
    static const char* COL_SMS_NUM;
    static const char* COL_SMS_BODY;
    static const char* COL_SMS_TIME;
    static const char* COL_SMS_TYPE;

    SmsProvider();
    virtual ~SmsProvider();

    virtual const char* contentURI() const { return SmsProvider::CONTENT_URI; }

private:
};

#endif   /* ----- #ifndef SMSPROVIDER_INC  ----- */

