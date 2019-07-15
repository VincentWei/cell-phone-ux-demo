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
/*! ============================================================================
 * @file test-MediaStoreService.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <gtest/gtest.h>

#include "global.h"
#include "MediaStoreService.hh"
#include "ContentResolver.hh"
#include "SimpleProviderFactory.hh"
#include "MediaStoreProvider.hh"

TEST(MediaStoreService, getSingleton)
{
    ASSERT_TRUE(NULL != GET_MEDIASTORE_SERVICE());
}

TEST(MediaStoreService, setup)
{
    MediaStoreProvider* mp = new MediaStoreProvider;
    GET_CONTENT_RESOLVER()->registerContentProvider(mp);
    ASSERT_TRUE(GET_MEDIASTORE_SERVICE());
}

TEST(MediaStoreService, getSongs)
{
    ContentValues::Strings albums = GET_MEDIASTORE_SERVICE()->getAllAlbumName();
    for (ContentValues::Strings::iterator iter = albums.begin();
            albums.end() != iter; ++iter) {
        ContentValues::Strings songs = GET_MEDIASTORE_SERVICE()->getSongsByAlbum(*iter);
        for (ContentValues::Strings::iterator itr = songs.begin();
                songs.end() != itr; ++itr) {
            printf("%s IN %s BY %s DURATION:%d.{%s}\n", 
                    itr->c_str(), 
                    iter->c_str(), 
                    GET_MEDIASTORE_SERVICE()->getAlbumAuthor(*iter).c_str(),
                    GET_MEDIASTORE_SERVICE()->getSongDuration(*itr),
                    GET_MEDIASTORE_SERVICE()->getAlbumLogo(*iter).c_str());
        }
    }
}

int main(int argc, const char* argv[]) 
{
    int ret = 0;

    ::testing::InitGoogleTest(&argc, (char **)argv);

    ret = RUN_ALL_TESTS();

    return ret;
}
