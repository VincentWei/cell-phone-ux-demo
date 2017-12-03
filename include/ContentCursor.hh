/*!============================================================================
 * @file ContentCursor.hh 
 * @Synopsis Interface of content cursor.  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */


#ifndef  CONTENTCURSOR_INC
#define  CONTENTCURSOR_INC

#include "ContentValues.hh"

class ContentCursor {
public:
    virtual ~ContentCursor() {}
    virtual void close() = 0;
    virtual void deactivate() = 0;
    virtual int getColumnCount() const = 0;
    virtual int getColumnIndex(const std::string& name) const = 0;
    virtual std::string getString(const std::string& columnName) const = 0;
    virtual int getInt(const std::string& columnName, int defaultVal = 0) const = 0;
    virtual double getDouble(const std::string& columnName, double defaultVal = 0) const = 0;
    virtual size_t  getCount() const = 0;
    virtual ContentValues::Strings getColumnNames() const = 0;
    virtual int getPosition() const = 0;

    virtual bool isFirst() const = 0;
    virtual bool isLast() const = 0;
    virtual bool isClosed() const = 0;
    virtual bool moveToFirst() = 0;
    virtual bool moveToLast() = 0;
    virtual bool moveToNext() = 0;
    virtual bool moveToPosition(int pos) = 0;
    virtual bool moveToPrevious() = 0;

    virtual bool requery() = 0;
private:
    /* data */
};
#endif   /* ----- #ifndef CONTENTCURSOR_INC  ----- */
