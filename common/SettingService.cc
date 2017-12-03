
#include <cstring>
#include <cstdarg>
#include <cassert>

#include "global.h"
#include "SettingService.hh"
#include "ContentResolver.hh"
#include "SettingsProvider.hh"

SettingService* SettingService::s_service = NULL;
const char* SettingService::s_optionTable[MAX_OPTION] = {
    "Photo", "WLAN", "Clock", "Shock", "Network selection", "Language", "Airplane mode", "Bluetooth", "Silent mode"
};

SettingService::SettingService()
{
}

SettingService* SettingService::singleton()
{
    if (NULL == s_service) {
        static SettingService sSettingService;
        s_service = &sSettingService;
        return s_service;
        //return s_service = new SettingService;
    }
    return s_service;
}


ContentValues::Strings SettingService::getAllItemNames()
{
//    StringArray selection;
    StringArray item_names;
//    selection.push_back(SettingsProvider::COL_NAME);
    ContentCursor* cur = 
        GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, NULL, NULL, NULL, NULL); 

    if (NULL != cur) {
        for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
            item_names.push_back(cur->getString(SettingsProvider::COL_NAME));
        }
        delete cur;
    }

    return item_names;
}

ContentValues::Strings SettingService::getCandidacy(const std::string& name)
{
    ContentValues::Strings result;
    ContentCursor* cur = getContentByName(name);
    if (NULL != cur) {
        std::string candidacy(cur->getString(SettingsProvider::COL_CANDIDACY));
        size_t pos;
        if (candidacy.length() > 0) {
            while (std::string::npos != 
                    (pos = candidacy.find_first_of(CANDIDACY_SEPARTOR))) {
                result.push_back(candidacy.substr(0, pos));
                candidacy = candidacy.substr(pos + 1);
            }
            if (!candidacy.empty()) {
                result.push_back(candidacy);
            }
        }
        delete cur;
    }

    return result;
}

std::string SettingService::getCurrent(const std::string& name)
{
    std::string result;
    ContentCursor* cur = getContentByName(name);
    if (NULL != cur) {
        for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
            result = (cur->getString(SettingsProvider::COL_CURRENT));
        }
        delete cur;
    }

    return result;
}

ExtraType SettingService::getItemType(const std::string& name)
{
    ContentCursor* cur = getContentByName(name);
    ExtraType ret;
    if (NULL != cur) {
        for (cur->moveToFirst(); !cur->isLast(); cur->moveToNext()) {
            ret = static_cast<ExtraType>(cur->getInt(SettingsProvider::COL_TYPE));
            delete cur;
            return ret;
        }
        delete cur;
    }

    return TYPE_NULL;
}

bool SettingService::getRawData(const ContentValues*& value)
{
    return false;
}


void SettingService::setCurrent(const std::string& name, const std::string& val)
{
    std::string where(SettingsProvider::COL_NAME);
    where += "='";
    where += name;
    where += "'";
    ContentValues cv;
    cv.putString(SettingsProvider::COL_CURRENT, val);
    GET_CONTENT_RESOLVER()->update(SettingsProvider::CONTENT_URI, cv, &where, NULL); 
}

ContentCursor* SettingService::getContentByName(const std::string& name)
{
    std::string where(SettingsProvider::COL_NAME);
    where += "='";
    where += name;
    where += "'";
    ContentCursor* cur = 
        GET_CONTENT_RESOLVER()->query(SettingsProvider::CONTENT_URI, 
                NULL, &where, NULL, NULL);
    return cur;
}

std::string SettingService::getCurrent(Option opt)
{
    return getCurrent(s_optionTable[opt]);
}

ExtraType SettingService::getItemType(Option opt)
{
    return getItemType(s_optionTable[opt]);
}
