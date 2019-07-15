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
#ifndef CHIPMUNK_UTILS_H
#define CHIPMUNK_UTILS_H

#include <chipmunk/chipmunk.h>

void create_baffle_board(cpSpace *space, int y_baseline, int y_height, int x1, int x2, float stiffness, float damping);
cpShape *create_floor(cpSpace *space, int y_baseline, int x1, int x2);
cpShape *create_ball(cpSpace *space, int x, int y, int r, int m);

#endif /* CHIPMUNK_UTILS_H */
