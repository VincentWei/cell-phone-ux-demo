
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
