/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __APP_ANC_ASSIST_CORE_H__
#define __APP_ANC_ASSIST_CORE_H__

#include "app_anc_assist.h"
#include "../../../utils/kfifo/kfifo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MIC_CHANNEL_NUMBER 10

typedef enum {
    APP_ANC_ASSIST_CORE_BTH_M55,
    APP_ANC_ASSIST_CORE_BTH_DSP,
    APP_ANC_ASSIST_CORE_BTH_SESHUB,
    APP_ANC_ASSIST_CORE_BTH_BTC,
    APP_ANC_ASSIST_CORE_NUM,
} app_anc_assist_core_t;

typedef enum {
    APP_ANC_ASSIST_CORE_CLOSE = 0,
    APP_ANC_ASSIST_CORE_OPEN,
    APP_ANC_ASSIST_CORE_PROCESS,
    APP_ANC_ASSIST_CORE_RESET,
    APP_ANC_ASSIST_CORE_CTRL,
    APP_ANC_ASSIST_CORE_SET_MODE,
    APP_ANC_ASSIST_CORE_QTY
} app_anc_assist_core_cmd_t;

typedef struct {
    struct kfifo ff_mic_fifo;
    uint8_t *ff_mic_fifo_mem;
    struct kfifo ref_fifo;
    uint8_t *ref_fifo_mem;
    struct kfifo talk_fifo;
    uint8_t *talk_fifo_mem;
    struct kfifo vpu_fifo;
    uint8_t *vpu_fifo_mem;
    struct kfifo fb_mic_fifo;
    uint8_t *fb_mic_fifo_mem;
} anc_assist_fifo_t;

typedef struct {
    anc_assist_user_t user;
    app_anc_assist_core_cmd_t cmd;
    uint32_t ctrl;
    uint32_t ctrl_buf_len;
    uint8_t ctrl_buf[128];
} app_anc_assist_core_to_m55_data_t;

typedef struct {
    anc_assist_user_t user;
    app_anc_assist_core_cmd_t cmd;
    anc_assist_fifo_t *fifo_ptr;
} app_anc_assist_core_to_bth_rsp_t;

typedef struct {
    anc_assist_user_t user;
    uint32_t len;
    uint32_t sub_cmd;
    uint8_t buf[256];
} app_anc_assist_core_to_bth_data_t;


#define APP_AI_KWS_RPC_HEADER_LEN            (8)
typedef enum {
    APP_AI_KWS_FRAME_DATA_PROCESS = 0,
    APP_AI_KWS_FRAME_DETECT_RESULT,
    APP_AI_KWS_MODEL_FILE_UPDATE,
    APP_AI_KWS_MODEL_FILE_UPDATE_RESULT,
    APP_AI_KWS_SHARE_MEM_BUFF_ADDRESS,
    APP_AI_KWS_SUBCMD_QTY,
}__attribute__((aligned(1))) app_ai_kws_rpc_subcmd_t;

typedef struct {
    uint32_t    frame_len;
    uint8_t     frame_buf[320];
} __attribute__((aligned(1))) app_ai_kws_frame_data_info_t;

typedef struct {
    uint8_t     hotword_detected;
    int32_t     score;
    uint32_t    detect_offset;
    uint8_t     kws_user;
} __attribute__((aligned(1))) app_ai_kws_detect_ret_info_t;

typedef struct {
    uint8_t     modelLanguage;
    // start logic address of the model
    uint32_t    startAddr;
    // length of model file
    uint32_t    len;
} __attribute__((aligned(1))) app_ai_model_file_entry_t;

typedef struct {
    uint8_t                     kws_user;
    uint8_t                     pending_update;
    uint8_t                     current_hotword;
    uint8_t                     current_lang;
    app_ai_model_file_entry_t   entry;
} __attribute__((aligned(1))) app_ai_kws_model_file_info_t;

typedef union {
    app_ai_kws_frame_data_info_t    frame_data_info;
    app_ai_kws_detect_ret_info_t    detect_ret_info;
    app_ai_kws_model_file_info_t    model_file_info;
} __attribute__((aligned(1))) app_ai_kws_rpc_payload_t;

typedef struct {
    uint32_t                                sub_cmd;
    uint32_t                                payload_len;
    app_ai_kws_rpc_payload_t                payload;
} __attribute__((aligned(1))) app_ai_kws_rpc_comm_data_t;


typedef struct {
    uint8_t     kws_user;
    uint32_t    pcm_cache_buf_addr;
    uint32_t    pcm_cache_buf_len;
    uint32_t    model_file_buf_addr;
    uint32_t    model_file_buf_len;
} __attribute__((aligned(1))) app_ai_kws_memory_share_info_t;

void app_anc_assist_core_init(void);
void app_anc_assist_core_deinit(void);
void app_anc_assist_core_open(app_anc_assist_core_t core, anc_assist_user_t user);
void app_anc_assist_core_close(app_anc_assist_core_t core, anc_assist_user_t user);
void app_anc_assist_core_reset(app_anc_assist_core_t core, anc_assist_user_t user);
void app_anc_assist_core_ctrl(app_anc_assist_core_t core, anc_assist_user_t user, uint32_t ctrl, uint8_t *buf, uint32_t len);
void app_anc_assist_core_process(app_anc_assist_core_t core, float *in_buf[],  uint32_t frame_len);

uint32_t app_anc_assist_core_get_status(void);
void app_anc_assist_core_suspend_users(bool en, uint32_t users);
bool app_anc_assist_core_user_is_suspend(anc_assist_user_t user);

#ifdef __cplusplus
}
#endif

#endif