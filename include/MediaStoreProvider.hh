/*!============================================================================
 * @file MediaStoreProvider.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  MEDIASTOREPROVIDER_INC
#define  MEDIASTOREPROVIDER_INC

#include "SQLiteProvider.hh"

class MediaStoreProvider : public SQLiteProvider {
    public:
    static const char* CONTENT_URI;
    static const char* DB_NAME;
    static const char* TABLE_ALBUM;
    static const char* TABLE_SONGS;
    static const char* COL_ID;
    static const char* COL_ALBUM;
    static const char* COL_SONG;
    static const char* COL_AUTHOR;
    static const char* COL_LOGO;
    static const char* COL_DURATION;

    MediaStoreProvider();
    virtual ~MediaStoreProvider();

    virtual const char* contentURI() const { return MediaStoreProvider::CONTENT_URI; }

    private:
};

#endif   /* ----- #ifndef MEDIASTOREPROVIDER_INC  ----- */

