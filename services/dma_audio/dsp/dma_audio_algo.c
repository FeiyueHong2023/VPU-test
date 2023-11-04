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
#include "string.h"
#include "cmsis.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_sysfreq.h"
#include "hal_cache.h"
#include "dma_audio_stream.h"
#include "dma_audio_stream_conf.h"
#include "dma_audio_algo.h"
#include "dma_audio.h"
#ifdef PSAP_SW_APP
#include "psap_sw_dsp.h"
#endif
#ifdef AUDIO_DOWN_MIXER
#include "audio_down_mixer_dsp.h"
#endif

static DAUD_ALGO_INIT_CALLBACK_T daud_algo_init_cb = NULL;

static DAUD_ALGO_CFG_CALLBACK_T daud_algo_cfg_cb = NULL;

static bool initialed = false;

static int algo_cfg_update_notify(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream,
    const struct DAUD_ALGO_CONFIG_T *cfg, bool set)
{
    TRACE(0, "[%s]: id=%d, stream=%d, cfg=%x, set=%d", __func__, id, stream, (int)cfg, set);
    if (daud_algo_cfg_cb) {
        daud_algo_cfg_cb(id, stream, cfg, set);
    }
    return 0;
}

void dma_audio_algo_setup_init_callback(DAUD_ALGO_INIT_CALLBACK_T callback)
{
    daud_algo_init_cb = callback;
}

void dma_audio_algo_setup_config_callback(DAUD_ALGO_CFG_CALLBACK_T callback)
{
    daud_algo_cfg_cb = callback;
}

void dma_audio_algo_init(struct DAUD_STREAM_CONFIG_T *cap_cfg, struct DAUD_STREAM_CONFIG_T *play_cfg)
{
    TRACE(0, "[%s] initialed ?= %d", __func__,initialed);

    if(!initialed){

        daud_set_algo_config_notify(algo_cfg_update_notify);

#ifdef PSAP_SW_APP
        psap_sw_dsp_init(cap_cfg, play_cfg);
#endif

#ifdef AUDIO_DOWN_MIXER
        audio_down_mixer_dsp_init();
#endif

        if (daud_algo_init_cb) {
            daud_algo_init_cb(true);
        }

        initialed = true;
    }
}

void dma_audio_algo_deinit(void)
{
    TRACE(0, "[%s] initialed ?= %d", __func__,initialed);

    if(initialed){
        if (daud_algo_init_cb) {
            daud_algo_init_cb(false);
        }
#ifdef PSAP_SW_APP
        psap_sw_dsp_deinit();
#endif
#ifdef AUDIO_DOWN_MIXER
        audio_down_mixer_dsp_deinit();
#endif
        initialed = false;
    }
}

int dma_audio_algo_process(uint8_t *dst, uint32_t dst_len, uint8_t *src, uint32_t src_len)
{
    int ret = 0;

#ifdef PSAP_SW_APP
    ret = psap_sw_dsp_process(dst, dst_len, src, src_len);
#endif

#ifdef AUDIO_DOWN_MIXER
    ret = audio_down_mixer_dsp_process(dst, dst_len, src, src_len);
#endif

    return ret;
}
