#include <stdlib.h>

#include "chipmunk-utils.h"

cp_baffle_board_t *create_baffle_board(cpSpace *space, int y_baseline, int y_height, int restLength, int x1, int x2, float stiffness, float damping) {
    cp_baffle_board_t *board;
    int r;
    int x_ball;
    int y_ball;
    int w_seg = 5;

    board = (cp_baffle_board_t *)calloc(1, sizeof(*board));

    r = y_height / 2;
    x_ball = x2 > x1 ? x1 + r : x1 - r;
    y_ball = y_baseline + r;

    /* Ball */
#if 1
    board->bodies[0] = cpBodyNew(1, INFINITY);
    board->bodies[0]->p = cpv(x_ball, y_ball);
    cpSpaceAddBody(space, board->bodies[0]);  

    board->shapes[0] = cpCircleShapeNew(board->bodies[0], r, cpvzero);  
    board->shapes[0]->e = 0; board->shapes[0]->u = 0; board->shapes[0]->collision_type = 1;
    cpSpaceAddShape(space, board->shapes[0]);
#else
    board->shapes[0] = create_block(space, x_ball - r, y_baseline, x_ball + r, y_baseline + 2*r, 1);
    board->bodies[0] = board->shapes[0]->body;
#endif

    /* Seg */
    board->bodies[1] = cpBodyNew(INFINITY, INFINITY);
    board->bodies[1]->p = cpv(x2, y_ball);

    board->shapes[1] = cpSegmentShapeNew(board->bodies[1],
            cpv(x1 < x2 ? -w_seg : w_seg, -r), cpv(x1 < x2 ? -w_seg : w_seg, r), w_seg);
    board->shapes[1]->e = 0; board->shapes[1]->u = 0; board->shapes[1]->collision_type = 1;
    cpSpaceAddStaticShape(space, board->shapes[1]);

    /* Spring */
    board->spring = cpDampedSpringNew(board->bodies[0], board->bodies[1],
            x1 < x2 ? cpv(-r, 0) : cpv(r, 0), cpvzero,
            restLength, stiffness, damping);
    cpSpaceAddConstraint(space, board->spring);

    return board;
}

void destroy_baffle_board(cpSpace *space, cp_baffle_board_t *board) {
    cpSpaceRemoveConstraint(space, board->spring);

    destroy_shape(space, board->shapes[0]);
    destroy_static_shape(space, board->shapes[1]);

    cpConstraintFree(board->spring);

    free(board);
}

cpShape *create_floor(cpSpace *space, int y_baseline, int x1, int x2) {
    cpBody *floorBody;
    cpShape *floorShape;

    floorBody = cpBodyNew(INFINITY, INFINITY);
    floorBody->p = cpv(x1, y_baseline);

    floorShape = cpSegmentShapeNew(floorBody, cpv(0, -10.0f), cpv(x2-x1, -10.0f), 10.0f);
    cpSpaceAddStaticShape(space, floorShape);

    return floorShape;
}

void destroy_static_shape(cpSpace *space, cpShape *shape) {
    cpBody *body = shape->body;
    if (cpSpaceContainsBody (space, body)) {
        cpSpaceRemoveBody(space, body);
    }
    else {
        printf ("destroy_static_shape: Space does not contain the specified body.\n");
    }
    cpSpaceRemoveStaticShape(space, shape);

    cpShapeFree(shape);
    cpBodyFree(body);
}

cpShape *create_ball(cpSpace *space, int x, int y, int r, int m) {
    cpBody *ballBody;
    cpShape *ballShape;
    
    ballBody = cpBodyNew(m, INFINITY);  
    ballBody->p = cpv(x, y);  
    cpSpaceAddBody(space, ballBody);  

    ballShape = cpCircleShapeNew(ballBody, r, cpvzero);  
    cpSpaceAddShape(space, ballShape);  

    return ballShape;
}

void destroy_shape(cpSpace *space, cpShape *shape) {
    cpBody *body = shape->body;
    if (cpSpaceContainsBody (space, body)) {
        cpSpaceRemoveBody(space, body);
    }
    else {
        printf ("destroy_static_shape: Space does not contain the specified body.\n");
    }
    cpSpaceRemoveShape(space, shape);

    cpShapeFree(shape);
    cpBodyFree(body);
}

cpShape *create_block(cpSpace *space, int x1, int y1, int x2, int y2, int m) {
    cpBody *blockBody;
    cpShape *blockShape;
    cpVect verts[4];
    cpVect center;

    center = cpv(x1, y1);
    verts[0] = cpvsub(cpv(x1, y1), center);
    verts[1] = cpvsub(cpv(x1, y2), center);
    verts[2] = cpvsub(cpv(x2, y2), center);
    verts[3] = cpvsub(cpv(x2, y1), center);

    blockBody = cpBodyNew(m, INFINITY);  
    blockBody->p = center;  
    cpSpaceAddBody(space, blockBody);  

    blockShape = cpPolyShapeNew(blockBody, sizeof(verts)/sizeof(verts[0]), verts, cpvzero);
    cpSpaceAddShape(space, blockShape);  

    return blockShape;
}
