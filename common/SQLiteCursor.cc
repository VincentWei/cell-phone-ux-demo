/*! ============================================================================
 * @file SQLiteCursor.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include "common.hh"
#include "SQLiteCursor.hh"

void SQLiteCursor::close()
{
    destroyResult();
}

void SQLiteCursor::deactivate()
{
}

int SQLiteCursor::getColumnCount() const
{
    const ContentValues* row = getRow(0);
    if (NULL != row) {
        return row->count();
    }
    return 0;
}

int SQLiteCursor::getColumnIndex(const std::string& name) const
{
    ContentValues::Strings::const_iterator iter = m_keySet.begin();
    for (int i = 0; m_keySet.end() != iter; ++iter, ++i) {
        if (name == *iter) {
            return i;
        }
    }
    return -1;
}

std::string SQLiteCursor::getString(const std::string& columnName) const
{
    if (!isLast()) {
        assert(NULL != *m_rowIterator);
        return (*m_rowIterator)->getAsString(columnName);
    }
    return "";
}

int SQLiteCursor::getInt(const std::string& columnName, int defVal) const
{
    if (!isLast()) {
        assert(NULL != *m_rowIterator);
        return (*m_rowIterator)->getAsInteger(columnName, defVal);
    }
    return defVal;
}

double SQLiteCursor::getDouble(const std::string& columnName, double defVal) const
{
    if (!isLast()) {
        return (*m_rowIterator)->getAsReal(columnName, defVal);
    }
    return defVal;
}

size_t  SQLiteCursor::getCount() const
{
    return m_queryResult.size();
}

ContentValues::Strings SQLiteCursor::getColumnNames() const
{
    return m_keySet;
}

int SQLiteCursor::getPosition() const
{
    return -1;
}

bool SQLiteCursor::isClosed() const
{
    return false;
}

bool SQLiteCursor::moveToPosition(int pos)
{
    return false;
}

bool SQLiteCursor::requery()
{
    destroyResult(); // destroy old result.
    bool ret = m_db.query(m_queryCmd);
    m_queryResult = m_db.lastQueryResult();
    refreshKeySet();
    return ret;
}

void SQLiteCursor::destroyResult()
{
    // let SQLiteDB free memory for m_queryResult, so there don't do that.
    //for_each(m_queryResult.begin(), m_queryResult.end(), DeleteObject());
    m_queryResult.clear();
}

const ContentValues* SQLiteCursor::getRow(size_t num) const
{
    if (getCount() > num) {
        return m_queryResult[num];
    }
    return NULL;
}

bool SQLiteCursor::refreshKeySet()
{
    const ContentValues* row = getRow(0);
    m_keySet.clear();
    if (NULL != row) {
        m_keySet = row->keySet();
        return true;
    }
    return false;
}

