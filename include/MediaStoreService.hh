/*!============================================================================
 * @file MediaStoreService.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  MEDIASTORESERVICE_INC
#define  MEDIASTORESERVICE_INC

#include <string>
#include "ContentValues.hh"

class ContentCursor;

class MediaStoreService {
public:
    static MediaStoreService* singleton();

    ContentValues::Strings getAllAlbumName();

    std::string getAlbumAuthor(const std::string& name);
    std::string getSongAuthor(const std::string& name);
    std::string getAlbumLogo(const std::string& name);

    ContentValues::Strings getSongsByAlbum(const std::string& name);
    std::string getAlbumBySong(const std::string& name);
    int getSongDuration(const std::string& name);

private:
    MediaStoreService ();
    ContentCursor* getContentByFactor(const std::string& key, const std::string& val, bool isQueryAlbum = true);
    static MediaStoreService* s_service;
};

#define GET_MEDIASTORE_SERVICE() MediaStoreService::singleton()
#endif   /* ----- #ifndef MEDIASTORESERVICE_INC  ----- */

