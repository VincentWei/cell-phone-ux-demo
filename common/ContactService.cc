/*! ============================================================================
 * @file ContactService.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include <sstream>
#include "ContentResolver.hh"
#include "ContactsProvider.hh"
#include "ContactService.hh"

ContactSerivce* ContactSerivce::s_service;

ContactSerivce* ContactSerivce::singleton()
{
    if (NULL == s_service) {
        s_service = new ContactSerivce;
    }
    return s_service;
}


bool ContactSerivce::hasContact(const std::string& name)
{
    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_NAME, name);
    if (NULL != cur) {
        if (cur->getCount() > 0) {
            delete cur;
            return true;
        }

        delete cur;
    }

    return false;
}

std::string ContactSerivce::getDialNumByName(const std::string& name)
{
    std::string dial;

    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_NAME, name);

    if (NULL != cur) {
        dial = cur->getString(ContactsProvider::COL_DIAL);
        delete cur;
    }

    return dial;
}

std::string ContactSerivce::getSmsNumByName(const std::string& name)
{
    std::string sms;

    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_NAME, name);

    if (NULL != cur) {
        sms = cur->getString(ContactsProvider::COL_SMS);
        delete cur;
    }

    return sms;
}

std::string ContactSerivce::getNameByDialNum(const std::string& dial)
{
    std::string name;

    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_DIAL, dial);

    if (NULL != cur) {
        name = cur->getString(ContactsProvider::COL_NAME);
        delete cur;
    }
    else {
        assert(0);
    }

    return name;
}

std::string ContactSerivce::getNameBySmsNum(const std::string& sms)
{
    std::string name;

    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_SMS, sms);

    if (NULL != cur) {
        name = cur->getString(ContactsProvider::COL_NAME);
        delete cur;
    }

    return name;
}

ContactSerivce::ContactSerivce()
{
}

ContentCursor* ContactSerivce::getContentByFactor(const std::string& key,
        const std::string& value)
{
    std::stringstream sql_cmd;
    sql_cmd << key << "='" << value << "'";
    std::string where(sql_cmd.str());
    ContentCursor* cur = 
        GET_CONTENT_RESOLVER()->query(ContactsProvider::CONTENT_URI, 
                NULL, &where, NULL, NULL);
    return cur;
}

std::string ContactSerivce::getPhotoBySmsNum(const std::string& sms)
{
    std::string photo;

    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_SMS, sms);

    if (NULL != cur) {
        photo = cur->getString(ContactsProvider::COL_ICON);
        delete cur;
    }

    return photo;
}

std::string ContactSerivce::getPhotoByDialNum(const std::string& num)
{
    std::string photo;

    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_DIAL, num);

    if (NULL != cur) {
        photo = cur->getString(ContactsProvider::COL_ICON);
        delete cur;
    }

    return photo;
}

std::string ContactSerivce::getPhotoByName(const std::string& name)
{
    std::string photo;

    ContentCursor* cur =
        getContentByFactor(ContactsProvider::COL_NAME, name);

    if (NULL != cur) {
        photo = cur->getString(ContactsProvider::COL_ICON);
        delete cur;
    }

    return photo;
}
