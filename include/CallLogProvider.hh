/*!============================================================================
 * @file CallLogProvider.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  CALLLOGPROVIDER_INC
#define  CALLLOGPROVIDER_INC

#include "SQLiteProvider.hh"

class CallLogProvider : public SQLiteProvider {
public:
    static const char* CONTENT_URI;
    static const char* DB_NAME;
    static const char* TABLE_NAME;
    static const char* COL_ID;
    static const char* COL_DIAL;
    static const char* COL_TIME;
    static const char* COL_TYPE;
    static const char* COL_DURATION;
    static const int   COL_TOTAL = 5;

    CallLogProvider ();
    virtual ~CallLogProvider ();

    virtual const char* contentURI() const { return CallLogProvider::CONTENT_URI; }

private:
};
#endif   /* ----- #ifndef CALLLOGPROVIDER_INC  ----- */

