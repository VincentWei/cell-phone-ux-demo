#ifndef CHIPMUNK_UTILS_H
#define CHIPMUNK_UTILS_H

#include <chipmunk/chipmunk.h>

typedef struct _cp_baffle_board{
    cpConstraint *spring;
    cpBody *bodies[2];
    cpShape *shapes[2];
} cp_baffle_board_t;

cp_baffle_board_t *create_baffle_board(cpSpace *space, int y_baseline, int y_height, int restLength, int x1, int x2, float stiffness, float damping);
cpShape *create_floor(cpSpace *space, int y_baseline, int x1, int x2);
cpShape *create_ball(cpSpace *space, int x, int y, int r, int m);
cpShape *create_block(cpSpace *space, int x1, int y1, int x2, int y2, int m);

void destroy_baffle_board(cpSpace *space, cp_baffle_board_t *board);
void destroy_static_shape(cpSpace *space, cpShape *shape);
void destroy_shape(cpSpace *space, cpShape *shape);

#endif /* CHIPMUNK_UTILS_H */
