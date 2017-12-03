#ifndef MSPEEDMETER_H
#define MSPEEDMETER_H

#define EXPORT /* TODO */

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Easy APIs
 */
EXPORT int QueryMouseMoveVelocity(float *v_x, float *v_y);

/* Proc for MiniGUI */
EXPORT int SpeedMeterProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

#ifdef NCSCTRL_WIDGET
/* Handler for mGNCS */
EXPORT BOOL SpeedMeterMessageHandler(mWidget*, int message, int x, int y, DWORD keyStatus);
#endif

#if 0
EXPORT int QueryMouseMovePath(POINT *points, unsigned int *times, int count);
#endif

/* 
 * Low level APIs
 */
/* DECLARE_HANDLE(SPEEDMETER) */
typedef struct _notused_mSpeedMeter_handle{ int not_used; } *SPEEDMETER;

EXPORT SPEEDMETER mSpeedMeter_create(int duration_ms, int precision_ms);
EXPORT void mSpeedMeter_destroy(SPEEDMETER handle);
EXPORT void mSpeedMeter_append(SPEEDMETER handle, int x, int y, unsigned int t);
EXPORT void mSpeedMeter_stop(SPEEDMETER handle);
EXPORT void mSpeedMeter_reset(SPEEDMETER handle);
EXPORT int mSpeedMeter_velocity(SPEEDMETER handle, float *v_x, float *v_y);
#if 0
EXPORT int  mSpeedMeter_getpath(SPEEDMETER handle, POINT *points, unsigned int *times, int count);
#endif

#ifdef __cplusplus
}
#endif

#endif /* MSPEEDMETER_H */
