#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "speedmeter.h"

typedef struct {
    int m_x;
    int m_y;
    unsigned int m_t;
} record_t;

typedef struct{
    int m_precision_ms;
    int m_size;
    int m_index;
    int m_count;
    record_t *m_records;
} speedmeter_t;

SPEEDMETER speedmeter_create(int duration_ms, int precision_ms){
    speedmeter_t *handle;

    handle = (speedmeter_t *)calloc(1, sizeof(*handle));

    handle->m_precision_ms = precision_ms;
    assert(handle->m_precision_ms > 0);

    handle->m_size = duration_ms / precision_ms;
    assert(handle->m_size > 0);

    handle->m_records = (record_t *)calloc(handle->m_size, sizeof(handle->m_records[0]));

    return (SPEEDMETER) handle;
}

void speedmeter_destroy(SPEEDMETER _handle){
    speedmeter_t *handle = (speedmeter_t *)_handle;
    assert(handle);

    if (handle->m_records) {
        free(handle->m_records);
    }
    free(handle);
}

void speedmeter_append(SPEEDMETER _handle, int x, int y, unsigned int t){
    speedmeter_t *handle = (speedmeter_t *)_handle;
    assert(handle);

    // printf("append (%d,%d,%u) at %d\n", x, y, t, handle->m_index);
    handle->m_records[handle->m_index].m_x = x;
    handle->m_records[handle->m_index].m_y = y;
    handle->m_records[handle->m_index].m_t = t;

    if (++handle->m_index >= handle->m_size) {
        handle->m_index = 0;
    }
    if (handle->m_count < handle->m_size) {
        assert(handle->m_index < handle->m_size);
        ++ handle->m_count;
    }
}

void speedmeter_stop(SPEEDMETER _handle){
    speedmeter_t *handle = (speedmeter_t *)_handle;
    assert(handle);
}

void speedmeter_reset(SPEEDMETER _handle){
    speedmeter_t *handle = (speedmeter_t *)_handle;
    assert(handle);
    handle->m_index = 0;
    handle->m_count = 0;
}

void speedmeter_velocity(SPEEDMETER _handle, float *v_x, float *v_y){
    speedmeter_t *handle = (speedmeter_t *)_handle;
    int i1, i2;
    unsigned int dT;
    assert(handle);

    if (handle->m_count < handle->m_size) {
        if (handle->m_count == 0) {
            *v_x = 0;
            *v_y = 0;
            return;
        }

        i1 = 0;
        i2 = handle->m_index - 1;
    }else{
        i2 = handle->m_index - 1;
        i1 = i2 - 1;
    }
    i1 %= handle->m_size;
    i2 %= handle->m_size;

    dT = handle->m_records[i2].m_t - handle->m_records[i1].m_t; 
    // printf("i1=%d i2=%d dT=%u\n", i1, i2, dT);
    *v_x = 1.0f * (handle->m_records[i2].m_x - handle->m_records[i1].m_x) / dT;
    *v_y = 1.0f * (handle->m_records[i2].m_y - handle->m_records[i1].m_y) / dT;
    // printf("v_x=%.4f v_y=%.4f\n", *v_x, *v_y);
}

/*
int speedmeter_getpath(SPEEDMETER _handle, POINT *points, unsigned int *times, int count){
    speedmeter_t *handle = (speedmeter_t *)_handle;
    assert(handle);

    assert(0);
    return -1;
}
*/
