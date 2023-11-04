/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#include "algo_process.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "string.h"
#include "hal_timer.h"
#include "speech_cfg.h"

static algo_process_cfg_t g_capture_cfg = {0};
static algo_process_cfg_t g_playback_cfg = {0};

SpeechNs4State *ns4_st = NULL;
Ec2FloatState *ec2float_st = NULL;

#define TEST_MIPS
#ifdef TEST_MIPS
    static uint32_t tx_start_ticks = 0;
    static uint32_t tx_end_ticks = 0;
#endif

#define SPEECH_TX_NS
#define SPEECH_TX_AEC

/***************************capture***************************/

int32_t algo_process_call_capture_open(algo_process_cfg_t *cfg)
{
    g_capture_cfg = *cfg;
    TRACE(4, "[%s] sample_rate=%d, frame_len=%d, mic_num=%d, mode=%d", __func__, \
                                        g_capture_cfg.sample_rate, \
                                        g_capture_cfg.frame_len, \
                                        g_capture_cfg.mic_num, \
                                        g_capture_cfg.mode);

#ifdef SPEECH_TX_NS
    const SpeechNs4Config ns4_cfg = {
        /*.bypass =*/ 0,
        /*.mode =*/ -18,
    };
#endif

#ifdef SPEECH_TX_AEC
    const Ec2FloatConfig config = {
        /*.bypass =*/ 0,
        /*.hpf_enabled =*/ 0,
        /*.af_enabled =*/ 1,
        /*.adprop_enabled =*/ 0,
        /*.varistep_enabled =*/ 0,
        /*.nlp_enabled =*/ 1,
        /*.clip_enabled =*/ 0,
        /*.stsupp_enabled =*/ 0,
        /*.hfsupp_enabled =*/ 0,
        /*.constrain_enabled =*/ 0,
        /*.ns_enabled =*/ 0,
        /*.cng_enabled =*/ 0,
        /*.blocks =*/ 6,
        /*.delay =*/ 0,
        /*.gamma =*/ 0.9,
        /*.echo_band_start =*/ 300,
        /*.echo_band_end =*/ 1800,
        /*.min_ovrd =*/ 2,
        /*.target_supp =*/ -40,
        /*.highfre_band_start =*/ 4000,
        /*.highfre_supp   =   */ 8.f,
        /*.noise_supp =*/ -15,
        /*.cng_type =*/ 1,
        /*.cng_level =*/ -60,
        /*.clip_threshold =*/ -20.f,
        /*.banks =*/ 64,
    };
#endif

    POSSIBLY_UNUSED int fs = g_capture_cfg.sample_rate;
    POSSIBLY_UNUSED int frame_size = g_capture_cfg.frame_len;

    TRACE(1,"Gary test!!");
#ifdef SPEECH_TX_NS
    ns4_st = speech_ns4_create(fs, frame_size, &ns4_cfg);
#endif
#ifdef SPEECH_TX_AEC
    ec2float_st = ec2float_create(fs, frame_size, 0, &config);
#endif

    return 0;
}

int32_t algo_process_call_capture_close(void)
{
    TRACE(1, "[%s] ...", __func__);

    // Add capture algo close func

    return 0;
}

int32_t algo_process_call_capture_set_cfg(uint8_t *cfg, uint32_t len)
{
    ASSERT(cfg != NULL, "[%s] cfg is NULL", __func__);

    // Set your algo config or tuning algo config

    return 0;
}

int32_t algo_process_call_capture_ctl(uint32_t ctl, uint8_t *ptr, uint32_t ptr_len)
{
    ASSERT(ptr != NULL, "[%s] ptr is NULL", __func__);

    // Ctrl your algo params or return algo params

    return 0;
}

int32_t algo_process_call_capture_process(uint8_t *in[PCM_CHANNEL_INDEX_QTY], uint8_t *out, uint32_t frame_len)
{
    if (g_capture_cfg.sample_bytes == sizeof(int16_t)){
        int16_t **pcm_in = (int16_t **)in;
        int16_t *pcm_out = (int16_t *)out;
#if 1
        int16_t *pcm_buf = pcm_in[PCM_CHANNEL_INDEX_MIC1];
        int16_t *ref_buf = pcm_in[PCM_CHANNEL_INDEX_REF];

#ifdef TEST_MIPS
        uint32_t old_tick = tx_start_ticks;
        tx_start_ticks = hal_fast_sys_timer_get();
#endif

#ifdef SPEECH_TX_AEC
        ec2float_process(ec2float_st, pcm_buf, ref_buf, frame_len, pcm_buf);
#endif
#ifdef SPEECH_TX_NS
        speech_ns4_process(ns4_st, pcm_buf, frame_len);
#endif

        for(uint32_t i = 0; i < frame_len; i++) {
            pcm_out[i] = pcm_buf[i];
        }

#ifdef TEST_MIPS
        tx_end_ticks = hal_fast_sys_timer_get();
        TRACE(2,"[%s] takes %d us in  %d us", __FUNCTION__,
            FAST_TICKS_TO_US(tx_end_ticks - tx_start_ticks),FAST_TICKS_TO_US(tx_start_ticks - old_tick));
#endif

#else
        for (uint32_t i = 0; i < frame_len; i++) {
            pcm_out[i] = pcm_in[0][i];
        }
#endif
    } else if (g_capture_cfg.sample_bytes == sizeof(int32_t)){
        int32_t **pcm_in = (int32_t **)in;
        int32_t *pcm_out = (int32_t *)out;
#if 0
        // Add your 24bit algo process
#else
        for (uint32_t i = 0; i < frame_len; i++) {
            pcm_out[i] = pcm_in[0][i] >> 1;
        }
#endif
    }

    return 0;
}

const algo_process_stream_t algo_process_call_capture = {
    .open    = algo_process_call_capture_open,
    .close   = algo_process_call_capture_close,
    .set_cfg = algo_process_call_capture_set_cfg,
    .ctl     = algo_process_call_capture_ctl,
    .process = algo_process_call_capture_process,
};

/***************************playback***************************/

int32_t algo_process_call_playback_open(algo_process_cfg_t *cfg)
{
    g_playback_cfg = *cfg;
    TRACE(4, "[%s] sample_rate=%d, frame_len=%d, mic_num=%d, mode=%d", __func__, \
                                        g_playback_cfg.sample_rate, \
                                        g_playback_cfg.frame_len, \
                                        g_playback_cfg.mic_num, \
                                        g_playback_cfg.mode);

    // Add playback algo open func

    return 0;
}

int32_t algo_process_call_playback_close(void)
{
    TRACE(1, "[%s] ...", __func__);

    // Add playback algo close func

    return 0;
}

int32_t algo_process_call_playback_set_cfg(uint8_t *cfg, uint32_t len)
{
    ASSERT(cfg != NULL, "[%s] cfg is NULL", __func__);

    // Set your algo config or tuning algo config

    return 0;
}

int32_t algo_process_call_playback_ctl(uint32_t ctl, uint8_t *ptr, uint32_t ptr_len)
{
    ASSERT(ptr != NULL, "[%s] cfg is NULL", __func__);

    // Ctrl your algo params or return algo params

    return 0;
}

static int32_t _algo_process_call_playback_process(uint8_t *in, uint8_t *out, uint32_t frame_len)
{
    if (g_playback_cfg.sample_bytes == sizeof(int16_t)){
        int16_t *pcm_in = (int16_t *)in;
        int16_t *pcm_out = (int16_t *)out;
#if 0
        // Add your 16bit algo process
#else
        for (uint32_t i = 0; i < frame_len; i++) {
            pcm_out[i] = pcm_in[i];
        }
#endif
    } else if (g_playback_cfg.sample_bytes == sizeof(int32_t)){
        int32_t *pcm_in = (int32_t *)in;
        int32_t *pcm_out = (int32_t *)out;
#if 0
        // Add your 24bit algo process
#else
        for (uint32_t i = 0; i < frame_len; i++) {
            pcm_out[i] = pcm_in[i];
        }
#endif
    }

    return 0;
}


int32_t algo_process_call_playback_process(uint8_t *in[PCM_CHANNEL_INDEX_QTY], uint8_t *out, uint32_t frame_len)
{
    return _algo_process_call_playback_process(in[0], out, frame_len);
}

const algo_process_stream_t algo_process_call_playback = {
    .open    = algo_process_call_playback_open,
    .close   = algo_process_call_playback_close,
    .set_cfg = algo_process_call_playback_set_cfg,
    .ctl     = algo_process_call_playback_ctl,
    .process = algo_process_call_playback_process,
};