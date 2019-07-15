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
/*!============================================================================
 * @file common.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  COMMON_INC
#define  COMMON_INC

#undef PACKAGE
#undef VERSION
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
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

