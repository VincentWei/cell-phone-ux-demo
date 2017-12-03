#ifndef SPEEDMETER_H
#define SPEEDMETER_H

#define EXPORT /* TODO */

/* DECLARE_HANDLE(SPEEDMETER) */
typedef struct _notused_speedmeter_handle{ int not_used; } *SPEEDMETER;

EXPORT SPEEDMETER speedmeter_create(int duration_ms, int precision_ms);
EXPORT void speedmeter_destroy(SPEEDMETER handle);
EXPORT void speedmeter_append(SPEEDMETER handle, int x, int y, unsigned int t);
EXPORT void speedmeter_stop(SPEEDMETER handle);
EXPORT void speedmeter_reset(SPEEDMETER handle);
EXPORT void speedmeter_velocity(SPEEDMETER handle, float *v_x, float *v_y);
/*
EXPORT int  speedmeter_getpath(SPEEDMETER handle, POINT *points, unsigned int *times, int count);
*/

#endif /* SPEEDMETER_H */
