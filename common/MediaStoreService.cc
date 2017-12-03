/*! ============================================================================
 * @file MediaStoreService.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include <sstream>
#include "common.hh"
#include "ContentResolver.hh"
#include "MediaStoreProvider.hh"
#include "MediaStoreService.hh"

MediaStoreService* MediaStoreService::s_service;

MediaStoreService* MediaStoreService::singleton()
{
    if (NULL == s_service) {
        s_service = new MediaStoreService;
    }
    return s_service;
}

ContentCursor* MediaStoreService::getContentByFactor(const std::string& key, 
        const std::string& val, bool isQueryAlbum)
{
    ContentCursor* cur = NULL;
    if ((!key.empty()) && (!val.empty())) {
        std::stringstream where;
        where << key << "=" << val;
        std::string whereDup(where.str());
        cur = GET_CONTENT_RESOLVER()->query(std::string(MediaStoreProvider::CONTENT_URI) 
                + (isQueryAlbum ? MediaStoreProvider::TABLE_ALBUM : MediaStoreProvider::TABLE_SONGS),
                NULL, &whereDup, NULL, NULL);
    }
    else {
        cur = GET_CONTENT_RESOLVER()->query(std::string(MediaStoreProvider::CONTENT_URI) 
                + (isQueryAlbum ? MediaStoreProvider::TABLE_ALBUM : MediaStoreProvider::TABLE_SONGS),
                NULL, NULL, NULL, NULL);
    }

    return cur;
}

ContentValues::Strings MediaStoreService::getAllAlbumName()
{
    ContentValues::Strings result;
    ContentCursor* cur = getContentByFactor("", "");
    if (NULL != cur) {
        for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
            result.push_back(cur->getString(MediaStoreProvider::COL_ALBUM));
        }
        delete cur;
    }
    else {
        assert(0);
    }
    return result;
}


std::string MediaStoreService::getAlbumAuthor(const std::string& name)
{
    ContentCursor* cur = getContentByFactor(MediaStoreProvider::COL_ALBUM,
            "'" + stringReplace(name, "'", "''") + "'");
    std::string ret;

    if (NULL != cur) {
        ret = cur->getString(MediaStoreProvider::COL_AUTHOR);
        delete cur;
    }
    else {
        assert(0);
    }

    return ret;
}

std::string MediaStoreService::getAlbumLogo(const std::string& name)
{
    ContentCursor* cur = getContentByFactor(MediaStoreProvider::COL_ALBUM,
            "'" + stringReplace(name, "'", "''") + "'");
    std::string ret;
    if (NULL != cur) {
        ret = cur->getString(MediaStoreProvider::COL_LOGO);
        delete cur;
    }
    else {
        assert(0);
    }
    return ret;
}


ContentValues::Strings MediaStoreService::getSongsByAlbum(const std::string& name)
{
    ContentValues::Strings result;
    ContentCursor* cur = getContentByFactor(MediaStoreProvider::COL_ALBUM,
            "'" + stringReplace(name, "'", "''") + "'", false);
    if (NULL != cur) {
        for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
            result.push_back(cur->getString(MediaStoreProvider::COL_SONG));
        }
        delete cur;
    }
    else {
        assert(0);
    }
    return result;
}

std::string MediaStoreService::getAlbumBySong(const std::string& name)
{
    ContentCursor* cur = getContentByFactor(MediaStoreProvider::COL_SONG, 
            "'" + stringReplace(name, "'", "''") + "'", false);
    std::string ret;
    if (NULL != cur) {
        std::string ret = cur->getString(MediaStoreProvider::COL_ALBUM);
        delete cur;
    }
    else {
        assert(0);
    }
    return ret; 
}

int MediaStoreService::getSongDuration(const std::string& name)
{
    ContentCursor* cur = getContentByFactor(MediaStoreProvider::COL_SONG, 
            "'" + stringReplace(name, "'", "''") + "'", false);
    int ret = 0;
    if (NULL != cur) {
        ret = cur->getInt(MediaStoreProvider::COL_DURATION);
        delete cur;
    }
    else {
        assert(0);
    }
    return ret;
}

MediaStoreService::MediaStoreService ()
{
}

