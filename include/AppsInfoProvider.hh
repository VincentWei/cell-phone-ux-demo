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
 * @file AppsInfoProvider.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  APPSINFOPROVIDER_INC
#define  APPSINFOPROVIDER_INC

#include "SQLiteProvider.hh"

class AppsInfoProvider : public SQLiteProvider {
public:
    static const char* CONTENT_URI;
    static const char* DB_NAME;
    static const char* TABLE_NAME;
    static const char* COL_ID;
    static const char* COL_APP_ACTIVITY;
    static const char* COL_APP_NAME;
    static const char* COL_APP_ICON;

    AppsInfoProvider();
    virtual ~AppsInfoProvider();

    virtual const char* contentURI() const { return AppsInfoProvider::CONTENT_URI; }

private:
};

#endif   /* ----- #ifndef APPSINFOPROVIDER_INC  ----- */

