/*! ============================================================================
 * @file SQLiteDB.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <stdio.h>
#include <cassert>
#include <sstream>
#include "common.hh"
#include "SQLiteDB.hh"
#include "SQLiteCursor.hh"

SQLiteDB::SQLiteDB (const std::string& db_file, bool useCache) : m_useCache(useCache)
{
    int ret = sqlite3_open(db_file.c_str(), &m_dbConnection);
    assert(SQLITE_OK == ret);
}

SQLiteDB::~SQLiteDB ()
{
    sqlite3_close(m_dbConnection);
    if (m_useCache) {
        while (m_resultCache.size() > 0) {
            for_each(m_resultCache.begin()->second.begin(), 
                    m_resultCache.begin()->second.end(), DeleteObject());
            m_resultCache.begin()->second.clear();
            m_resultCache.erase(m_resultCache.begin());
        }
    }
    else {
        for_each(m_sqlResult.begin(), m_sqlResult.end(), DeleteObject());
    }

    // nodes of m_sqlResult not need to be delete because m_resultCache holding a copy of it.
    m_sqlResult.clear();
}

int SQLiteDB::onQuery(void *self, int argc, char **argv, char **azColName)
{
    ContentValues* content = new ContentValues;
    if (NULL != content) {
        for(int i = 0; i < argc; i++) {
            content->putString(azColName[i], argv[i] ? argv[i] : "");
        }
    }
    else {
        assert(0);
        return -1;
    }

    if (NULL != self) {
        SQLiteDB* db = static_cast<SQLiteDB*>(self);
        db->m_sqlResult.push_back(content);
    }
    else {
        delete content;
        assert(0);
        return -1;
    }

    return 0;
}

ContentCursor* SQLiteDB::query(const std::string& table, 
        const ContentValues::Strings* projection, 
        const std::string* selection, 
        const ContentValues::Strings* selectionArgs, 
        const std::string* sortOrder)
{
    std::stringstream sql_cmd;
    sql_cmd << "SELECT ";

    if (NULL != projection) {
        ContentValues::Strings::const_iterator iter = projection->begin();
        for (; projection->end() != iter; ++iter) {
            sql_cmd << *(iter) << ",";
        }
        sql_cmd.seekp(-1, std::ios_base::end);
    }
    else {
        sql_cmd << "*";
    }

    sql_cmd << " FROM " << table;

    if (NULL != selection) {
        sql_cmd << " WHERE ";
        if (NULL != selectionArgs && (!selectionArgs->empty())) {
            sql_cmd << replaceSelectionArgs(*selection, *selectionArgs);
        }
        else {
            sql_cmd << *selection;
        }
    }

    if (NULL != sortOrder) {
        sql_cmd << " ORDER BY " << *sortOrder;
    }

    sql_cmd << ";";

    return query(sql_cmd.str());
}

bool SQLiteDB::execSQL(const std::string& sql_cmd, bool is_query)
{
    char* err_msg = NULL;
//    fprintf(stdout, "execSQL=> %s\n", sql_cmd.c_str());
    // clear last query result, but not destroy item inside, let the SQLiteCursor do that.
    if (!m_useCache) {
        for_each(m_sqlResult.begin(), m_sqlResult.end(), DeleteObject());
    }

    m_sqlResult.clear();
    int ret = sqlite3_exec(m_dbConnection, sql_cmd.c_str(), 
            (is_query ? SQLiteDB::onQuery : NULL), this, &err_msg);

    if (SQLITE_OK != ret) {
        //fprintf(stderr, "execSQL-> SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return false;
    } 

    m_lastSQLCmd = sql_cmd;

    if (m_useCache) {
        cacheQueryResult(sql_cmd, m_sqlResult);
    }
    return true;
}

bool SQLiteDB::queryFromCache(const std::string& sql_cmd)
{
    SQLCacheItem::iterator ret = m_resultCache.find(sql_cmd);
    if (m_resultCache.end() != ret) {
        //fprintf(stderr, "[cache matched]%s\n", sql_cmd.c_str());
        m_sqlResult = ret->second;
        return true;
    }
    return false;
}

void SQLiteDB::cacheQueryResult(const std::string& cmd, const SQLCache& result)
{
    if (m_resultCache.size() >= s_cacheItemTotal) {
        for_each(m_resultCache.begin()->second.begin(), 
                m_resultCache.begin()->second.end(), DeleteObject());
        m_resultCache.begin()->second.clear();
        m_resultCache.erase(m_resultCache.begin());
    }

    SQLCacheItem::iterator ret = m_resultCache.find(cmd);

    if (ret != m_resultCache.end()) {
        for_each(ret->second.begin(), 
                ret->second.end(), DeleteObject());
        ret->second.clear();
        m_resultCache.erase(ret);
    }

    m_resultCache.insert(m_resultCache.end(), SQLCacheItem::value_type(cmd, result));
    //fprintf(stderr, "[cache]%s\n", cmd.c_str());
}

ContentCursor* SQLiteDB::query(const std::string& sql_cmd)
{
    if ((m_useCache && queryFromCache(sql_cmd)) || execSQL(sql_cmd, true)) {
        return new SQLiteCursor(*this);
    }

    return NULL;
}

bool SQLiteDB::insert(const std::string& table, const ContentValues& values)
{
    std::stringstream sql_cmd(std::stringstream::in | std::stringstream::out);
    ContentValues::Strings key_set = values.keySet();
    if (!key_set.empty()) {
        ContentValues::Strings::iterator iter = key_set.begin();
        sql_cmd << "INSERT INTO " << table << " (";
        for (; key_set.end() != iter; ++iter) {
            sql_cmd << (*iter) << ",";
        }
        sql_cmd.seekp(-1, std::ios_base::end);

        sql_cmd << ") VALUES (";
        for (iter = key_set.begin(); key_set.end() != iter; ++iter) {
            if (ContentValues::isInteger(values.get(*iter))) {
                sql_cmd << values.getAsInteger(*iter, 0) << ",";
            } else if (ContentValues::isReal(values.get(*iter))) {
                sql_cmd << values.getAsReal(*iter, 0) << ",";
            } else if (ContentValues::isString(values.get(*iter))){
                sql_cmd << "'" << values.getAsString(*iter) << "'" << ",";
            } else if (ContentValues::isNull(values.get(*iter))) {
                sql_cmd << "NULL" << ",";
            } else {
                assert(0);
            }
        }
        sql_cmd.seekp(-1, std::ios_base::end);
        sql_cmd << ");";
        execSQL(sql_cmd.str());
        refreshTableCache(table);
    }

    return false;
}

bool SQLiteDB::remove(const std::string& table, const std::string* selection, 
        const ContentValues::Strings* selectionArgs)
{
    std::stringstream sql_cmd;
    sql_cmd << "DELETE FROM " << table;
    if (NULL != selection) {
        sql_cmd << " WHERE ";
        if (NULL != selectionArgs && (!selectionArgs->empty())) {
            sql_cmd << replaceSelectionArgs(*selection, *selectionArgs);
        } else {
            sql_cmd << *selection;
        }
    }
    refreshTableCache(table);
    return execSQL(sql_cmd.str());
}

bool SQLiteDB::update(const std::string& table, const ContentValues& values, 
        const std::string* selection, 
        const ContentValues::Strings* selectionArgs)
{
    std::stringstream sql_cmd;
    sql_cmd << "UPDATE " << table;

    ContentValues::Strings keys = values.keySet();
    if (!keys.empty()) {
        sql_cmd << " SET ";
        for (size_t i = 0; i < keys.size(); ++i) {
            sql_cmd << keys[i] << "=";
            if (ContentValues::isInteger(values.get(keys[i]))) {
                sql_cmd << values.getAsInteger(keys[i], 0) << ",";
            } else if (ContentValues::isReal(values.get(keys[i]))) {
                sql_cmd << values.getAsReal(keys[i], 0) << ",";
            } else if (ContentValues::isString(values.get(keys[i]))){
                sql_cmd << "'" << values.getAsString(keys[i]) << "'" << ",";
            } else if (ContentValues::isNull(values.get(keys[i]))) {
                sql_cmd << "NULL" << ",";
            } else {
                assert(0);
            }
        }
        sql_cmd.seekp(-1, std::ios_base::end);

        if (NULL != selection) {
            sql_cmd << " WHERE ";
            if (NULL != selectionArgs && (!selectionArgs->empty())) {
                sql_cmd << replaceSelectionArgs(*selection, *selectionArgs);
            } else {
                sql_cmd << *selection;
            }
        }
        sql_cmd << ";";
    }
    refreshTableCache(table);
    return execSQL(sql_cmd.str());
}

std::string SQLiteDB::replaceSelectionArgs(const std::string& selection,
        const ContentValues::Strings& selectionArgs)
{
    size_t pos = 0, i = 0;
    std::string result(selection);

    if (!selectionArgs.empty()) {
        while ((pos = result.find_first_of('?')) != std::string::npos) {
            if (i < selectionArgs.size()) {
                result.replace(pos, 1, selectionArgs[i]);
                ++i;
            } else {
                break;
            }
        }
    }
    return result;
}

void SQLiteDB::refreshTableCache(const std::string& tableName) {
    if (m_useCache) {
        SQLCacheItem::iterator i = m_resultCache.begin();
        for (; i != m_resultCache.end(); ++i) {
            // TODO: This method is more extensive for refresh the cache
            if (i->first.find(tableName) != std::string::npos) {
                //fprintf(stderr, "[refresh cache]%s\n", i->first.c_str());
                for_each(i->second.begin(), i->second.end(), DeleteObject());
                i->second.clear();
                m_resultCache.erase(i);
            }
        }
    }
}

