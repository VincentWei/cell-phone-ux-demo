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
 * @file ContentResolver.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  CONTENTRESOLVER_INC
#define  CONTENTRESOLVER_INC

#include <set>
#include "ContentProvider.hh"

class ContentCursor;
class ContentObserver;

class ContentResolver : public ContentProvider {
public:
    typedef std::set<ContentProvider*> ContentProviderCollection;
    static const char* CONTENT_URI;

    static ContentResolver* singleton();

    virtual ~ContentResolver ();

    std::string insert(const std::string& uri, const ContentValues& values);
    ContentCursor* query(const std::string& uri, const ContentValues::Strings* projection, const std::string* 
                    selection, const ContentValues::Strings* selectionArgs, const std::string* sortOrder);
    void notifyChange(const std::string& uri);
    int remove(const std::string& uri, const std::string* where, const ContentValues::Strings* selectionArgs);
    void registerContentObserver(const std::string& uri, ContentObserver* observer);
    void unregisterContentObserver(const std::string& uri, ContentObserver* observer);
    void unregisterContentObserver(ContentObserver* observer);
    int update(const std::string& uri, const ContentValues& values, 
            const std::string* selection, const ContentValues::Strings* selectionArgs);
    void registerContentProvider(ContentProvider* provider);
    void unregisterContentProvider(ContentProvider* provider);
    void clearContentProvider();

protected:
    virtual const char* contentURI() const { return ContentResolver::CONTENT_URI; }
    ContentResolver ();
    ContentProvider* matchProviderByURI(const std::string& uri);

private:
    static ContentResolver* s_resolver;
    ContentProviderCollection m_providers; 
};

#define GET_CONTENT_RESOLVER() ContentResolver::singleton()

#endif   /* ----- #ifndef CONTENTRESOLVER_INC  ----- */

