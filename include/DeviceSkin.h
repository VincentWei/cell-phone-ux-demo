/*!============================================================================
 * @file DeviceSkin.h 
 * @Synopsis  
 * @author czz
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */


#ifndef __DEVICESKIN__
#define __DEVICESKIN__

#ifdef __cplusplus
extern "C"
{
#endif

#define SKIN_DECLARE_HANDLE(name) struct _skin_##name##___{int unused;}; \
    typedef struct _skin_##name##___ * name

/* 
 * define HSKIN handle.
 * */
SKIN_DECLARE_HANDLE(HSKIN);

/** 
 * @synopsis  load a skin config，return hanle to the created skin configure object on 
 * success，or return NULL on failure。
 * @param file
 * 
 * @returns   
 */
HSKIN SkinLoad(const char* file);

/** 
 * @synopsis  unload a skin configure object specified by hskin. release resouces used 
 * by this object.
 * 
 * @param hskin
 */
void  SkinUnload(HSKIN hskin);

/** 
 * 
 * @synopsis  Get key's corresponding value in configure file. value should be allocated 
 * by user and should be big enough to hold the value. the value is a 
 * NULL-terminated C-string specified by value and the string's len is returned
 * by len.  
 * If key not found or buffer for value is too small return -1;
 * On success return 0;
* 
 * @param HSKIN
 * @param key
 * @param value
 * @param len
 * 
 * @returns   
 */
int SkinGetItem(HSKIN, const char* key, char* value, int len);

/** 
 * 
 * @synopsis  Get the scancode by point. return scancode on success or -1 on failure
 * 
 * @param hskin
 * @param pt
 * 
 * @returns   
 */
int   SkinPointToScancode(HSKIN hskin, const POINT* pt);

/** 
 * 
 * @synopsis  Get region of the key specified by scancode, return pointer to region on 
 * success or -1 on failure 
 * 
 * @param hskin
 * @param scancode
 * 
 * @returns   
 */
const RECT* SkinScancodeToRect(HSKIN hskin, int scancode);

/** 
 * @synopsis  Get region of the key which contains poit specified by pt.
 * 
 * @param hskin
 * @param pt
 * 
 * @returns   
 */
const RECT* SkinPointToRect(HSKIN hskin, const POINT* pt);

/** 
 * @synopsis  Get name of the key of specified by scancode.
 * 
 * @param hskin
 * @param scancode
 * 
 * @returns   
 */
const char* SkinScancodeToName(HSKIN hskin, int scancode);

/** 
 * @synopsis Get name of the key of specified by point.
 * 
 * @param hskin
 * @param pt
 * 
 * @returns   
 */
const char* SkinPointToName(HSKIN hskin, const POINT* pt);

#ifdef __cplusplus
}
#endif

#endif
