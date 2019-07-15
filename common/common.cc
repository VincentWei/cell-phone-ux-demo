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
 * @file common.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include "common.hh"

std::string stringReplace(const std::string& str, const std::string& oldVal, const std::string& newVal)
{
    std::string strSrc(str);
    std::string::size_type pos=0;       
    std::string::size_type srclen = oldVal.length();        
    std::string::size_type dstlen = newVal.length();        
    while( (pos=strSrc.find(oldVal, pos)) != std::string::npos)
    {                
        strSrc.replace(pos, srclen, newVal);                
        pos += dstlen;        
    }
    return strSrc;
}
