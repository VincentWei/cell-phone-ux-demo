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
 * @file ContactsLoadHelper.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <sstream>
#include "common.hh"
#include "ContentResolver.hh"
#include "ContactsProvider.hh"
#include "ContactsLoadHelper.hh"

int LoadGroupContacts(ContactsGroup& group, const std::string& groupIndex,
        OperateAccord groupAccord, OperateAccord orderAccord,
        bool is_ascending)
{
    static const char* s_selectionAccord[OPERATE_ACCORD_TOTAL] = {
        ContactsProvider::COL_ID,
        ContactsProvider::COL_NAME,
        ContactsProvider::COL_DIAL,
        ContactsProvider::COL_SMS,
    };
    int ret = -1;
    ContentResolver* content = ContentResolver::singleton();
    std::stringstream selectionBuilder;
    selectionBuilder << s_selectionAccord[groupAccord] << 
//        "=" << "'" << groupIndex << "'" << " OR " << s_selectionAccord[groupAccord] << 
        " LIKE " << "'" << groupIndex << "%" << "'";

    std::string orderBy(s_selectionAccord[orderAccord]);
    orderBy += is_ascending ? " ASC" : " DESC";

    std::string selection(selectionBuilder.str());
    ContentCursor *cur = content->query(ContactsProvider::CONTENT_URI, 
                NULL,
                &selection,
                NULL,
                &orderBy
                );

    if (NULL != cur && cur->getCount() > 0) {
        do {
            ContactsInfo* item = new ContactsInfo;
            item->icon_path = cur->getString(ContactsProvider::COL_ICON);
            item->photo_path = stringReplace(item->icon_path, ".png", "big.png");
            item->name = cur->getString(ContactsProvider::COL_NAME);
            item->dial_num = cur->getString(ContactsProvider::COL_DIAL);
            item->sms_num = cur->getString(ContactsProvider::COL_SMS);
            group.push_back(item);
        } while (cur->moveToNext());
        ret = 0;
    }

    delete cur;
    return ret;
}
