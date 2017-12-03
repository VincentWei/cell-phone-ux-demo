/*!============================================================================
 * @file ContactService.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  CONTACTSERVICE_INC
#define  CONTACTSERVICE_INC

#include <string>
class ContentCursor;

class ContactSerivce
{
public:
    static ContactSerivce* singleton();

    bool hasContact(const std::string& name);
    std::string getDialNumByName(const std::string& name);
    std::string getSmsNumByName(const std::string& name);
    std::string getNameByDialNum(const std::string& name);
    std::string getNameBySmsNum(const std::string& name);
    std::string getPhotoBySmsNum(const std::string& sms);
    std::string getPhotoByDialNum(const std::string& num);
    std::string getPhotoByName(const std::string& name);

private:
    ContactSerivce();
    ContentCursor* getContentByFactor(const std::string& key,
            const std::string& value);

    static ContactSerivce* s_service;
};

#define GET_CONTACTSERVICE() ContactSerivce::singleton()
#endif   /* ----- #ifndef CONTACTSERVICE_INC  ----- */

