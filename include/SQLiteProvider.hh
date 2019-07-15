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
 * @file SQLiteProvider.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  SQLITEPROVIDER_INC
#define  SQLITEPROVIDER_INC
#include <stdio.h>
#include <stdlib.h>

#include "ContentProvider.hh"
#include "SQLiteDB.hh"

class SQLiteProvider : public ContentProvider {
public:
    SQLiteProvider (const std::string& db_name, bool useCache = true) : 
        m_db(db_name, useCache) 
    {}
    virtual ~SQLiteProvider ()
    {
#ifdef DEBUG
        printf ("~SQLiteProvider.......................\n");
#endif
    }

    ContentCursor* query(const std::string& uri, const ContentValues::Strings* projection, const std::string* 
                    selection, const ContentValues::Strings* selectionArgs, const std::string* sortOrder) {
        return m_db.query(ContentProvider::getDomainFromURI(uri), projection, selection,
                selectionArgs, sortOrder);
    }

    int remove(const std::string& uri, const std::string* selection, const ContentValues::Strings* selectionArgs) {
        m_db.remove(ContentProvider::getDomainFromURI(uri), selection, selectionArgs);
        return -1;
    }

    int update(const std::string& uri, const ContentValues& values, 
            const std::string* selection, const ContentValues::Strings* selectionArgs) {
        m_db.update(ContentProvider::getDomainFromURI(uri), values, selection, selectionArgs);
        return -1;
    }

    std::string insert(const std::string& uri, const ContentValues& values) {
        m_db.insert(ContentProvider::getDomainFromURI(uri), values);
        return "";
    }

protected:
    SQLiteDB m_db;
    
private:
    SQLiteProvider();
};

#endif   /* ----- #ifndef SQLITEPROVIDER_INC  ----- */

