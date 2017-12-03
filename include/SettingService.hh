
/*!============================================================================
 * @file SettingService.hh 
 * @Synopsis  
 * @author DengMin
 * @version 1.0
 * @date 08/12/2010 08:54:09 
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef _SETTINGSERVICE_HH
#define _SETTINGSERVICE_HH

#include "ContentValues.hh"

enum ExtraType {
    TYPE_EDIT,
    TYPE_CHECKBOX,
    TYPE_INDICATOR,
    TYPE_TEXT,
    TYPE_NULL
};

class ContentCursor;

class SettingService
{
public:
    enum Option {
        PHOTO,
        WLAN,
        CLOCK,
        SHOCK,
        NETWORK,
        LANGUAGE,
        AIRMODE,
        BLUETOOTH,
        SILENTMODE,
        MAX_OPTION
    };

    static SettingService* singleton();

    ContentValues::Strings getAllItemNames();
    ContentValues::Strings getCandidacy(const std::string& name);
    std::string getCurrent(const std::string& name);
    std::string getCurrent(Option opt);
    ExtraType getItemType(Option opt);
    ExtraType getItemType(const std::string& name);
    bool getRawData(const ContentValues*& value);

    void setCurrent(const std::string& name, const std::string& val);

private:
    static const char* s_optionTable[MAX_OPTION];
    SettingService();
    ContentCursor* getContentByName(const std::string& name);

    static SettingService* s_service;
};

#define GET_SETTINGSERVICE() SettingService::singleton()
#define CANDIDACY_SEPARTOR  "#"

#endif
