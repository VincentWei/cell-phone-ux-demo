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
 * @file Utility.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  UTILITY_INC
#define  UTILITY_INC

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#ifdef __cplusplus
extern "C" {
#endif

HDC GetWindowForeground(HWND hwnd, BOOL set_foreground);

/** 
 * @synopsis ReadXmlConfig 
 * 
 * @param file
 * @param path
 * @param attr
 * @param value
 * @param size
 * 
 * @returns   
 */
int ReadXmlConfig(const char* file, const char* path, const char* attr, char* value, size_t size);

#ifdef __cplusplus
}
#endif
#endif   /* ----- #ifndef UTILITY_INC  ----- */

