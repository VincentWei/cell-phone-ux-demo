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
 * @file SettingsProvider.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  SETTINGS_INC
#define  SETTINGS_INC

#include "SQLiteProvider.hh"

class SettingsProvider : public SQLiteProvider {
public:
    static const char* CONTENT_URI;
    static const char* DB_NAME;
    static const char* TABLE_NAME;
    static const char* COL_ID;
    static const char* COL_NAME;
    static const char* COL_TYPE;
    static const char* COL_CURRENT;
    static const char* COL_CANDIDACY;
    static const int   COL_TOTAL;

    SettingsProvider();
    virtual ~SettingsProvider();

    virtual const char* contentURI() const { return SettingsProvider::CONTENT_URI; }
};

#endif   /* ----- #ifndef SETTINGS_INC  ----- */

