/*!============================================================================
 * @file DataSource.hh 
 * @Synopsis DataSource interface. 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  DATASOURCE_INC
#define  DATASOURCE_INC

#include <string>

class ContentCursor;
class ContentValues;

class DataSource {
    public:
        virtual ContentCursor* query(const std::string& table, 
                 const ContentValues::Strings* projection, 
                 const std::string* selection, 
                 const ContentValues::Strings* selectionArgs, 
                 const std::string* sortOrder) = 0;

        virtual bool insert(const std::string& table, const ContentValues& values) = 0;
        virtual bool remove(const std::string& table, const std::string* selection, 
                 const ContentValues::Strings* selectionArgs) = 0;
        virtual bool update(const std::string& table, const ContentValues& values, 
                const std::string* selection, 
                const ContentValues::Strings* selectionArgs) = 0;

};

#endif   /* ----- #ifndef DATASOURCE_INC  ----- */
