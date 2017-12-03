/*!============================================================================
 * @file ContactsLoadHelper.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  CONTACTSLOADHELPER_INC
#define  CONTACTSLOADHELPER_INC

#include <string>
#include <vector>

typedef struct {
    std::string icon_path;
    std::string name;
    std::string dial_num;
    std::string sms_num;
    std::string photo_path;
} ContactsInfo;

typedef std::vector<ContactsInfo*> ContactsGroup;

enum OperateAccord{
    OPERATE_BY_ID,
    OPERATE_BY_NAME,
    OPERATE_BY_DIAL_NUM,
    OPERATE_BY_SMS_NUM,
    OPERATE_ACCORD_TOTAL
};

int LoadGroupContacts(ContactsGroup& group, const std::string& groupIndex,
        OperateAccord groupAccord = OPERATE_BY_NAME, OperateAccord orderAccord = OPERATE_BY_NAME,
        bool is_ascending = true);
#endif   /* ----- #ifndef CONTACTSLOADHELPER_INC  ----- */

