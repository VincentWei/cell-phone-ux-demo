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
 * @file effector-shoot.h
 * @Synopsis  
 * @author ZhaolinHu
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef _EFF_EFFECTORSHOOT_H
#define _EFF_EFFECTORSHOOT_H

#include <mgeff/mgeff.h>

#define MGEFF_MINOR_shoot     "shoot"
#define MGEFF_EFFECTOR_SHOOT  mGEffStr2Key(MGEFF_MINOR_shoot)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EffME2Ctxt {
    int prev_reach;
} EffME2Ctxt;

MGEFF_EFFECTOR effshooteffector_init (MGEFF_EFFECTOR _effector);
void effshooteffector_finalize (MGEFF_EFFECTOR _effector);
void effshooteffector_ondraw(MGEFF_ANIMATION animation, MGEFF_EFFECTOR _effector, 
        HDC sink_dc, intptr_t id, void* value);
void effshooteffector_begindraw(MGEFF_ANIMATION animation, MGEFF_EFFECTOR _effector);

extern MGEFF_EFFECTOROPS shoot;

#ifdef __cplusplus
}
#endif
#endif
