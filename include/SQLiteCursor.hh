/*!============================================================================
 * @file SQLiteCursor.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  SQLITECURSOR_INC
#define  SQLITECURSOR_INC

#include "ContentCursor.hh"
#include "SQLiteDB.hh"

class SQLiteCursor : public ContentCursor {
public:
    virtual ~SQLiteCursor () {close();}
    virtual void close();
    virtual void deactivate();
    virtual int getColumnCount() const;
    virtual int getColumnIndex(const std::string& name) const;
    virtual std::string getString(const std::string& columnName) const;
    virtual int getInt(const std::string& columnName, int defVal) const;
    virtual double getDouble(const std::string& columnName, double defVal) const;
    virtual size_t  getCount() const;
    virtual ContentValues::Strings getColumnNames() const;
    virtual int getPosition() const;

    virtual bool isFirst() const { return m_queryResult.begin() == m_rowIterator;}
    virtual bool isLast() const { return m_queryResult.end() == m_rowIterator; }
    virtual bool isClosed() const;
    virtual bool moveToFirst(){ m_rowIterator = m_queryResult.begin(); return true; }
    virtual bool moveToLast() { m_rowIterator = m_queryResult.end(); return true; }
    virtual bool moveToNext() { 
        if (m_queryResult.end() != m_rowIterator) {
            return ++m_rowIterator != m_queryResult.end();
        }
        return false;
    }
    virtual bool moveToPosition(int pos);
    virtual bool moveToPrevious() {
        if (m_queryResult.begin() != m_rowIterator) {
            return --m_rowIterator != m_queryResult.begin();
        }
            return false;
    };

    virtual bool requery();

private:
    SQLiteCursor(SQLiteDB& db) :
        m_db(db), m_queryResult(db.lastQueryResult()) {
            refreshKeySet();
            m_rowIterator = m_queryResult.begin();
            m_queryCmd = m_db.lastSQLCmd();
        }

    // copy & assign forbidden.
    SQLiteCursor(const SQLiteCursor&);
    SQLiteCursor& operator=(const SQLiteCursor&);

    void destroyResult();
    const ContentValues* getRow(size_t num) const;
    bool refreshKeySet();

    SQLiteDB&   m_db;
    SQLiteDB::SQLCache    m_queryResult;
    ContentValues::Strings m_keySet;
    SQLiteDB::SQLCache::iterator m_rowIterator;
    std::string m_queryCmd;

    friend class SQLiteDB;
};
#endif   /* ----- #ifndef SQLITECURSOR_INC  ----- */

