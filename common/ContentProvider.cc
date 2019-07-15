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
 * @file ContentProvider.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "ContentProvider.hh"

std::string ContentProvider::getLastSectionOfURI(const std::string& uri)
{
    static std::string def;

    if (0 == uri.find(CONTENT_URI_SCHEME)) {
        size_t pos = uri.find_last_of('/');

        if (std::string::npos != pos) {
            return uri.substr(pos + 1);
        }
    }

    return def;
}

int ContentProvider::getRowNumFromURI(const std::string& uri)
{
    int row = -1;

    std::string last = getLastSectionOfURI(uri);

    if (!last.empty()) {
        std::istringstream ss(last);
        ss >> row;
    }

    return row;
}

std::string ContentProvider::getDomainFromURI(const std::string& uri)
{
    if (getRowNumFromURI(uri) > 0) {
        std::string last = getLastSectionOfURI(uri);
        std::string domain = getLastSectionOfURI(uri.substr(0, uri.length() - last.length()));
        assert(domain.find_first_of('/') == std::string::npos);
        return domain;
    }
    else {
        return getLastSectionOfURI(uri);
    }
}

ContentProvider::ContentProvider()
{
#ifdef DEBUG
    printf ("ContentProvider.....................\n");
#endif
}

ContentProvider::~ContentProvider()
{
#ifdef DEBUG
    printf ("~ContentProvider.....................\n");
#endif
}

