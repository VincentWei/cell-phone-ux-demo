/*!============================================================================
 * @file common.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  COMMON_INC
#define  COMMON_INC

#include "config.h"
#include <string>
std::string stringReplace(const std::string& str, const std::string& oldVal, const std::string& newVal);

struct DeleteObject {
    template <typename T>
        void operator()(const T* ptr) const {
            delete ptr;
        }
};
#endif   /* ----- #ifndef COMMON_INC  ----- */

