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
 * @file ContentProvider.hh 
 * @Synopsis Content Provider interface. 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  CONTENTPROVIDER_INC
#define  CONTENTPROVIDER_INC

#include <string>
#include <set>
#include "ContentCursor.hh"
#include "ContentValues.hh"

#define CONTENT_URI_SCHEME "content://"

class ContentObserver {
public:
    virtual void onContentChanged() = 0;
};

class ContentProvider {
public:
    typedef std::set<ContentObserver*>ContentObserverCollection;

    static int getRowNumFromURI(const std::string& uri);
    static std::string getDomainFromURI(const std::string& uri);

    ContentProvider();
    virtual ~ContentProvider();

    virtual ContentCursor* query(const std::string& uri, const ContentValues::Strings* projection, 
            const std::string* selection, const ContentValues::Strings* selectionArgs, 
            const std::string* sortOrder) = 0;
    virtual int remove(const std::string& uri, const std::string* selection, 
            const ContentValues::Strings* selectionArgs) = 0;
    virtual int update(const std::string& uri, const ContentValues& values, 
            const std::string* selection, const ContentValues::Strings* selectionArgs) = 0;
    virtual std::string insert(const std::string& uri, const ContentValues& values) = 0;

    void registerContentObserver(ContentObserver* observer) {
        if (NULL != observer) {
            if (m_observers.end() == m_observers.find(observer)) {
                m_observers.insert(observer);
            }
        }
    }

    void unregisterContentObserver(ContentObserver* observer) {
        m_observers.erase(observer);
    }

    void notifyChange() {
        ContentObserverCollection::iterator iter;
        for (iter = m_observers.begin(); m_observers.end() != iter; ++iter) {
            (*iter)->onContentChanged();
        }
    }
    
    virtual const char* contentURI() const = 0;
private:
    static std::string getLastSectionOfURI(const std::string& uri);
    ContentObserverCollection m_observers;
    const char* m_contentURI;
};
#endif   /* ----- #ifndef CONTENTPROVIDER_INC  ----- */
