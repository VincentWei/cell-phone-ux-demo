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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#ifdef _MGINCORE_RES

#include "_pmp_inner_res.c"
#include "mgetc.h"

int CellPhoneInitInnerResource (void)
{
    return AddInnerRes(__mgpmp_pmp_inner_res, TABLESIZE(__mgpmp_pmp_inner_res), FALSE);
}

#endif /*_MGINCORE_RES*/
