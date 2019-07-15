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
#include <stdio.h>
#include <string.h>

#include <chipmunk/chipmunk.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define RED MakeRGBA(0xff, 0, 0, 0xff)
#define BLUE MakeRGBA(0, 0xff, 0, 0xff)
#define GREEN MakeRGBA(0, 0, 0xff, 0xff)

#define A 100

cpBody *ballBody;

static cpFloat springForce(cpConstraint *spring, cpFloat dist)
{   
    cpFloat clamp = 20.0f;
    return cpfclamp(cpDampedSpringGetRestLength(spring) - dist, -clamp, clamp)*cpDampedSpringGetStiffness(spring);
}

int onCollisionBegin(cpArbiter *arb, struct cpSpace *space, void *data){
    return 0;
}

void setupChipmunk(cpSpace *space) {
    space->gravity = cpv(-200, 0);

    // floor
    {
        cpBody *floorBody;
        floorBody = cpBodyNew(INFINITY, INFINITY);
        floorBody->p = cpv(A, 150);

        cpShape *floorShape = cpSegmentShapeNew(floorBody, cpv(-10, -300), cpv(-10, 300), 10.0f);
        floorShape->e = 0; floorShape->u = 1.0; floorShape->collision_type = 1;
        floorShape->data = GREEN;
        cpSpaceAddStaticShape(space, floorShape);
    }

    // ball 1
    if (1){
        ballBody = cpBodyNew(100, INFINITY);  
        ballBody->p = cpv(A+30, 0);  
        cpSpaceAddBody(space, ballBody);  

        // cpShape *ballShape = cpSegmentShapeNew(ballBody, cpv(-20, 0), cpv(20, 0), 10.0f);
        cpShape *ballShape = cpCircleShapeNew(ballBody, 30, cpvzero);  
        ballShape->e = 0; // Elasticity  
        ballShape->u = 1.0; // Friction  
        ballShape->collision_type = 1; // Collisions are grouped by types  
        ballShape->data = RED;
        cpSpaceAddShape(space, ballShape);  
    }

    {
        cpBody *blockBody1, *blockBody2;
        cpShape *blockShape;

        blockBody1 = cpBodyNew(1, INFINITY);
        blockBody1->p = cpv(A+30, 150);
        cpSpaceAddBody(space, blockBody1);  

        // blockShape = cpSegmentShapeNew(blockBody1, cpv(-30, 0), cpv(30, 0), 10.0f);
        blockShape = cpCircleShapeNew(blockBody1, 30, cpvzero);  
        blockShape->e = 0; blockShape->u = 0; blockShape->collision_type = 1;
        blockShape->data = 0xffffffff;
        cpSpaceAddShape(space, blockShape);

        blockBody2 = cpBodyNew(INFINITY, INFINITY);
        blockBody2->p = cpv(A+30, 350);

        blockShape = cpSegmentShapeNew(blockBody2, cpv(-30, 0), cpv(30, 0), 10.0f);
        blockShape->e = 0; blockShape->u = 0; blockShape->collision_type = 1;
        blockShape->data = 0xffffffff;
        cpSpaceAddStaticShape(space, blockShape);

        cpConstraint *spring = cpDampedSpringNew(blockBody1, blockBody2, cpvzero, cpvzero, 200.0, 50000.0, 10);
        // cpDampedSpringSetSpringForceFunc(spring, springForce);
        cpSpaceAddConstraint(space, spring);

        blockBody2 = cpBodyNew(INFINITY, INFINITY);
        blockBody2->p = cpv(A-60, 120);

        blockShape = cpSegmentShapeNew(blockBody2, cpv(-30, 5), cpv(30, 5), 10.0f);
        blockShape->e = 0; blockShape->u = 100; blockShape->collision_type = 1;
        blockShape->data = RED;
        cpSpaceAddStaticShape(space, blockShape);
    }
}

int proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_LBUTTONDOWN:
            cpBodySetVel(ballBody, cpv(0, 1200));
            break;
        default:
            return -1;
    }
    return -1;
}

void timer_callback(cpSpace *space) {
    float v = cpBodyGetVel(ballBody).y;
    float p = cpBodyGetPos(ballBody).y;
    if (v < 0 && p <= 90) {
        // printf("v=%f, x=%f\n", v, p);
        cpBodySleep(ballBody);
        ballBody->p.y = 90;
    }
}
