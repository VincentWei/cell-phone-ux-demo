#ifndef PHYSICS_ANIMATION_H
#define PHYSICS_ANIMATION_H

#ifndef EXPORT
#   define EXPORT /* TODO */
#endif

#include <chipmunk/chipmunk.h>
#include <mgeff/mgeff.h>

#ifdef __cplusplus
extern "C" {
#endif

EXPORT MGEFF_ANIMATION phyanim_create(cpSpace *space, void *context,
        int (*onCalc)(MGEFF_ANIMATION anim, cpSpace *space, void *context),
        void (*onDraw)(MGEFF_ANIMATION anim, cpSpace *space, void *context));
EXPORT void phyanim_destroy(MGEFF_ANIMATION animation);
EXPORT cpSpace *phyanim_getspace(MGEFF_ANIMATION animation);

#ifdef __cplusplus
}
#endif

#endif /* PHYSICS_ANIMATION_H */
