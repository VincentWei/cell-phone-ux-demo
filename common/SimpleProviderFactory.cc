/*! ============================================================================
 * @file SimpleProviderFactory.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include "global.h"
#include "ContentProvider.hh"
#include "SettingsProvider.hh"
#include "SmsProvider.hh"
#include "MediaStoreProvider.hh"
#include "ContactsProvider.hh"
#include "CallLogProvider.hh"
#include "AppsInfoProvider.hh"
#include "SimpleProviderFactory.hh"

namespace ProviderFactory {

ContentProvider* CreateSystemProvider(const std::string& name)
{
    if (name == SETTINGS_PROVIDER) {
        return new SettingsProvider;
    } else if (name == CONTACTS_PROVIDER) {
        return new ContactsProvider;
    } else if (SMS_PROVIDER == name) {
        return new SmsProvider;
    } else if (MEDIASTORE_PROVIDER == name) {
        return new MediaStoreProvider;
    } else if (CALLLOG_PROVIDER == name) {
        return new CallLogProvider;
    } else if (APPSINFO_PROVIDER == name) {
        return new AppsInfoProvider;
    }

    assert(0);
    return NULL;
}

}

