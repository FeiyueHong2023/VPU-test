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
#ifndef __APP_ANC_ASSIST_H__
#define __APP_ANC_ASSIST_H__

#include "plat_types.h"
#include "hal_aud.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANC_ASSIST_MODE_NONE = 0,
    ANC_ASSIST_MODE_STANDALONE,
    ANC_ASSIST_MODE_PHONE_CALL,
    ANC_ASSIST_MODE_RECORD,
    ANC_ASSIST_MODE_MUSIC,
    ANC_ASSIST_MODE_MUSIC_AAC,
    ANC_ASSIST_MODE_MUSIC_SBC,
    ANC_ASSIST_MODE_LE_CALL,
    ANC_ASSIST_MODE_QTY
} anc_assist_mode_t;

typedef enum {
    ANC_ASSIST_USER_ANC = 0,
    ANC_ASSIST_USER_PSAP,
    ANC_ASSIST_USER_PSAP_SW,
    ANC_ASSIST_USER_CUSTOM_LEAK_DETECT,
    ANC_ASSIST_USER_PROMPT_LEAK_DETECT,
    ANC_ASSIST_USER_PILOT_ANC,
    ANC_ASSIST_USER_FIR_LMS,
    // upper are the assist_algo_m55 user
    ANC_ASSIST_USER_ALGO_DSP,

    ANC_ASSIST_USER_KWS,
    ANC_ASSIST_USER_WD,
    ANC_ASSIST_USER_ONESHOT_ADAPT_ANC,
    ANC_ASSIST_USER_NOISE_ADAPT_ANC,
	ANC_ASSIST_USER_ULTRASOUND,
    ANC_ASSIST_USER_ADAPTIVE_EQ,
    ANC_ASSIST_USER_AUTO_EQ_SELECTION,
    ANC_ASSIST_USER_QTY
} anc_assist_user_t;

typedef int32_t (*anc_assist_user_callback_t)(void *buf, uint32_t len, void *other);
typedef int32_t (*anc_assist_user_result_callback_t)(void *buf, uint32_t len, void *other, uint32_t sub_cmd);

// #define ANC_ASSIST_CALL_SUSPEND_USERS       ((0x1 << ANC_ASSIST_USER_AUTO_ANC) | (0x1 << ANC_ASSIST_USER_SOUND_DETECT) | (0x1 << ANC_ASSIST_USER_VOICE_DETECT))
// #define ANC_ASSIST_RECORDING_SUSPEND_USERS  ((0x1 << ANC_ASSIST_USER_AUTO_ANC) | (0x1 << ANC_ASSIST_USER_SOUND_DETECT) | (0x1 << ANC_ASSIST_USER_VOICE_DETECT) | (0x1 << ANC_ASSIST_USER_ALGO_DSP))

int32_t app_anc_assist_init(void);
int32_t app_anc_assist_deinit(void);
int32_t app_anc_assist_register(anc_assist_user_t user, anc_assist_user_callback_t func);
int32_t app_anc_assist_result_register(anc_assist_user_t user, anc_assist_user_result_callback_t func);
anc_assist_user_result_callback_t app_anc_assist_get_result_register(anc_assist_user_t user);
int32_t app_anc_assist_is_runing(void);
uint32_t app_anc_assist_get_mic_ch_num(enum AUD_IO_PATH_T path);
uint32_t app_anc_assist_get_mic_ch_map(enum AUD_IO_PATH_T path);
int32_t app_anc_assist_set_mode(anc_assist_mode_t mode);
int32_t app_anc_assist_set_playback_info(int32_t sample_rate);
int32_t app_anc_assist_open(anc_assist_user_t user);
int32_t app_anc_assist_close(anc_assist_user_t user);
int32_t app_anc_assist_reset(anc_assist_user_t user);
int32_t app_anc_assist_ctrl(anc_assist_user_t user, uint32_t ctrl, uint8_t *buf, uint32_t len);
int32_t app_anc_assist_process(void *pcm_buf, uint32_t pcm_len);
int32_t app_anc_assist_process_interval(void *buf, uint32_t len);
int32_t app_anc_assist_parser_app_mic_buf(void *buf, uint32_t *len);
int32_t app_anc_assist_get_prompt_anc_index(int32_t *anc_index, int32_t channel_idx, float *band1, float *band2, float *band3);

uint32_t codec_play_merge_pilot_data(uint8_t *buf, uint32_t len, void *cfg);
int32_t app_anc_assist_users_suspend(bool en, uint32_t users);
int32_t app_anc_assist_set_capture_info(uint32_t frame_len);
int32_t app_anc_assist_algo_reset(void);

#ifdef __cplusplus
}
#endif

#endif