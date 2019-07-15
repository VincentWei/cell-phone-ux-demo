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
 * @file ContactsProvider.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  CONTACTSPROVIDER_INC
#define  CONTACTSPROVIDER_INC

#include "SQLiteProvider.hh"

class ContactsProvider : public SQLiteProvider {
public:
    static const char* CONTENT_URI;
    static const char* DB_NAME;
    static const char* TABLE_NAME;
    static const char* COL_ID;
    static const char* COL_NAME;
    static const char* COL_ICON;
    static const char* COL_DIAL;
    static const char* COL_SMS;
    static const int   COL_TOTAL;

    ContactsProvider ();
    virtual ~ContactsProvider ();

    virtual const char* contentURI() const { return ContactsProvider::CONTENT_URI; }

private:
};

#endif   /* ----- #ifndef CONTACTSPROVIDER_INC  ----- */

