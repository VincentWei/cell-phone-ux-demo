/**
 * $Id: speedmeter.h 1209 2010-05-27 05:40:31Z rlzheng $
 *
 * This file create for mphoneview, but it is a independentpart module.
 *
 * Copyright (C) 2010 Feynman Software.
 *
 * All rights reserved by Feynman Software.
 *
 * Author: rlzheng
 * Data  : 2010.05.20
 */

#ifndef _SPEEDMETER_H
#define _SPEEDMETER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _node {
    float x;
    unsigned int t;
}node;

struct _speedmeter{
    int cur_index;
    int cur_size;
    int sampling_num;
    node *sampling_data;
};

typedef struct _speedmeter speedmeter;
typedef struct _speedmeter* speedmeter_handle;

speedmeter_handle speedmeter_create(int sampling_num);                 
void speedmeter_destroy(speedmeter_handle handle);                     
void speedmeter_add(speedmeter_handle handle, float x, unsigned int t);
float speedmeter_calc(speedmeter_handle handle);                       
void speedmeter_reset(speedmeter_handle handle);

#ifdef __cplusplus
}
#endif

#endif /* end of _SPEEDMETER_H */
