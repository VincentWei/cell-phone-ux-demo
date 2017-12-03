/**
 * $Id: speedmeter.c 1209 2010-05-27 05:40:31Z rlzheng $
 *
 * The implementation of speedmeter.
 *
 * Copyright (C) 2010 Feynman Software.
 *
 * All rights reserved by Feynman Software.
 *
 * Author: rlzheng
 * Data  : 2010.05.20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "speedmeter.h"

speedmeter_handle speedmeter_create(int sampling_num)
{
    speedmeter* handle = (speedmeter*)calloc(1, sizeof(speedmeter));
    
    handle->cur_index = -1;
    handle->cur_size = 0;
    handle->sampling_num = sampling_num;
    handle->sampling_data = (node*)calloc(1, sizeof(node) * sampling_num);
    if (handle->sampling_data == NULL) {
        fprintf(stderr, "Speedmeter create error!\n");
        return NULL;
    }
    return (speedmeter_handle)handle;
}

void speedmeter_destroy(speedmeter_handle handle)
{
    if (handle) {
        if (handle->sampling_data) {
            free(handle->sampling_data);
        }
        free(handle);
    }
}

void speedmeter_add(speedmeter_handle handle, float x, unsigned int t)
{
    if (handle->cur_size < handle->sampling_num) {
        handle->cur_size++;
        handle->cur_index++;
    }
    else {
        handle->cur_index = (handle->cur_index +1) % handle->sampling_num;
    }
    handle->sampling_data[handle->cur_index].x = x;
    handle->sampling_data[handle->cur_index].t = t;
}

float speedmeter_calc(speedmeter_handle handle)
{
    float total_v = 0, v = 0;
    int i, j, k;
    int s1, s2, count;
    unsigned t;

    if (handle->cur_size < 2) {
        return 0;
    }

    if (handle->cur_size == handle->sampling_num) {
        s1 = (handle->cur_index +1) % handle->sampling_num;
        s2 = (handle->cur_index +1 + handle->sampling_num/2)
                % handle->sampling_num;

        count = (s1 > s2) ? (handle->sampling_num - s1)
            : (handle->sampling_num - s2);

        for (k = 0, i = s2, j = s1; k < count; i++, j++, k++) {
            t = handle->sampling_data[i].t - handle->sampling_data[j].t;
            if (t <= 0) {
                fprintf(stderr, "Error, divisor is 0\n");
                return 0;
            }
            v = (handle->sampling_data[i].x -handle->sampling_data[j].x) /t;
            total_v += v;
        }

        if (s1 > s2) {
            count = s2;
            j = 0;
        }
        else {
            count = s1;
            i = 0;
        }

        for (k = 0; k < count; i++, j++, k++) {
            t = handle->sampling_data[i].t - handle->sampling_data[j].t;
            if (t <= 0) {
                fprintf(stderr, "Error, divisor is 0\n");
                return 0;
            }
            v = (handle->sampling_data[i].x -handle->sampling_data[j].x) /t;
            total_v += v;
        }
    }
    else {
        count = handle->cur_size / 2;
        for (i = count, j = 0; j <count; i++, j++ ) {
            t = handle->sampling_data[i].t - handle->sampling_data[j].t;
            if (t <= 0) {
                fprintf(stderr, "Error, divisor is 0\n");
                return 0;
            }
            v = (handle->sampling_data[i].x - handle->sampling_data[j].x) /t;
            total_v += v;
        }
    }
    v = total_v / (handle->cur_size /2);

    return v;
}

void speedmeter_reset(speedmeter_handle handle)
{
    handle->cur_index = -1;
    handle->cur_size = 0;
    memset(handle->sampling_data, 0,
            sizeof(node) * handle->sampling_num);
}

