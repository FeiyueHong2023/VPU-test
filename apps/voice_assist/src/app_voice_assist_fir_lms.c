/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "hal_trace.h"
#include "app_anc_assist.h"
#include "app_voice_assist_fir_lms.h"
#include "anc_ff_fir_lms.h"
#include "anc_assist.h"
#include "anc_process.h"
#include "audio_dump.h"
#include "app_utils.h"
#include "heap_api.h"

static ANCFFFirLmsSt * fir_st= NULL;
static ANC_FF_FIR_LMS_CFG_T cfg ={
    .max_cnt = 100,
    .period_cnt = 5,
};
static int stop_flag = 0;

#define HEAP_BUFF_SIZE (40*1024)

static int32_t _voice_assist_fir_lms_callback(void *buf, uint32_t len, void *other);

int32_t app_voice_assist_fir_lms_init(void)
{
    app_anc_assist_register(ANC_ASSIST_USER_FIR_LMS, _voice_assist_fir_lms_callback);
    return 0;
}

int32_t app_voice_assist_fir_lms_reset(void)
{
    anc_ff_fir_lms_reset(fir_st, 0);
    return 0;
}

void set_fixed_fir_filter(void)
{
    anc_ff_fir_lms_reset(fir_st, 1);
}


int32_t app_voice_assist_fir_lms_open(void)
{
    TRACE(0, "[%s] fir lms start stream", __func__);
    stop_flag = 0;
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_208M);

    syspool_init_specific_size(HEAP_BUFF_SIZE);
    fir_st = anc_ff_fir_lms_create(16000, 120, &cfg, syspool_malloc);
    app_anc_assist_open(ANC_ASSIST_USER_FIR_LMS);
    return 0;
}

int32_t app_voice_assist_fir_lms_close(void)
{
    TRACE(0, "[%s] fir lms close stream", __func__);
    anc_ff_fir_lms_destroy(fir_st);
    app_anc_assist_close(ANC_ASSIST_USER_FIR_LMS);
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
#ifdef FIR_LMS_DUMP
    audio_dump_deinit();
#endif
    return 0;
}



static int32_t _voice_assist_fir_lms_callback(void * buf, uint32_t len, void *other)
{

    if (stop_flag == 0) {
        float ** input_data = buf;
        float * ff_data = input_data[0];  // error
        float * fb_data = input_data[1];  // error

        for (int i = 0; i < len; i++) {
            ff_data[i] = ff_data[i] / 256;
            fb_data[i] = fb_data[i] / 256;
        }
        
        if (anc_ff_fir_lms_process(fir_st, ff_data, fb_data, 120)) {
            stop_flag = 1;
            // anc_ff_fir_lms_reset(fir_st, 1);
            app_voice_assist_fir_lms_close();
        }
    } else {
        return 0;
    }
    return 0;
    
} 