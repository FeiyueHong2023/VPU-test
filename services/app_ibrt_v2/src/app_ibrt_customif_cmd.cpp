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
/*****************************header include********************************/
#include "string.h"
#include "hal_trace.h"
#include "app_tws_ctrl_thread.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_ibrt_customif_cmd.h"
#include "app_ibrt_if.h"
#include "bluetooth_bt_api.h"
#include "app_dip.h"
#include "a2dp_decoder.h"
#ifdef __GMA_VOICE__
#include "gma_crypto.h"
#endif

#ifdef BISTO_ENABLED
#include "gsound_custom.h"
#endif

#ifdef DUAL_MIC_RECORDING
#include "app_ai_if.h"
#include "ai_control.h"
#include "app_ai_if.h"
#include "ai_thread.h"
#include "app_recording_handle.h"
#endif

#ifdef __BIXBY_VOICE__
#include "app_bixby_thirdparty_if.h"
#endif

#if !defined(IBRT_UI) && defined(__REPORT_EVENT_TO_CUSTOMIZED_UX__)
#include "app_custom_adapter.h"
#endif
#if defined(SPA_AUDIO_ENABLE)
#include "spa_ext_tws_handler.h"
#endif

/*********************external function declaration*************************/
#if defined(ANC_APP)
extern "C" int32_t app_anc_tws_sync_change(uint8_t *buf, uint32_t len);
#endif

#if defined(ANC_ASSIST_ENABLED)
extern "C" int32_t anc_assist_tws_sync_status_change(uint8_t *buf, uint32_t len);
#endif
extern "C" int32_t audio_process_tws_sync_change(uint8_t *buf, uint32_t len);

#if defined(IBRT)

/****************************function defination****************************/
#ifdef __GMA_VOICE__
void app_ibrt_gma_secret_key_sync(uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_GMA_SECRET_KEY, p_buff, length);
}
void app_ibrt_gma_secret_key_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
    gma_set_secret_key((char *)p_buff);
}

#endif

#ifdef BISTO_ENABLED
static void app_ibrt_bisto_dip_sync(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_BISTO_DIP_SYNC, p_buff, length);
}

static void app_ibrt_bisto_dip_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    gsound_mobile_type_get_callback(*(MOBILE_CONN_TYPE_E *)p_buff);
}
#endif

#ifdef DUAL_MIC_RECORDING
static void app_ibrt_customif_audio_send(uint8_t* p_buff, uint16_t length)
{
    TRACE(1, "%s", __func__);
    app_recording_send_data_to_master();
}

static void app_ibrt_customif_audio_send_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length)
{
    ai_function_handle(CALLBACK_STORE_SLAVE_DATA, (void*)p_buff, length, AI_SPEC_RECORDING, app_ai_get_device_id_from_index(AI_SPEC_RECORDING));
    //TRACE(1, "%s", __func__);
}

static void app_ibrt_customif_audio_send_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t* ptrParam,
        uint16_t paramLen)
{
    TRACE(1, "%s", __func__);
    app_recording_audio_send_done();
}

static void app_ibrt_customif_update_bitrate(uint8_t* p_buff, uint16_t length)
{
    TRACE(1, "%s", __func__);
    app_ibrt_send_cmd_without_rsp(APP_IBRT_CUSTOM_CMD_UPDATE_BITRATE, p_buff, length);
}

static void app_ibrt_customif_update_bitrate_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length)
{
#ifdef VOC_ENCODE_SCALABLE
    SCALABLE_CONFIG_BITRATE_T *info = (SCALABLE_CONFIG_BITRATE_T *)p_buff;
    TRACE(3, "%s bitrate:%d, cursor:%d", __func__, info->bitrate, info->cursor);

    app_recording_update_scalable_bitrate(info->bitrate, info->cursor);
#endif
}

static void app_ibrt_customif_update_bitrate_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t* ptrParam,
        uint16_t paramLen)
{
    TRACE(1, "%s", __func__);
}

static void app_ibrt_customif_report_buf_lvl(uint8_t* p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_IBRT_CUSTOM_CMD_REPORT_BUF_LVL, p_buff, length);
}

static void app_ibrt_customif_report_buf_lvl_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length)
{
#ifdef VOC_ENCODE_SCALABLE
    app_recording_on_peer_buf_lvl_received(*p_buff);
#endif
}

static void app_ibrt_customif_report_buf_lvl_done(uint16_t cmdcode, uint16_t rsp_seq,
                                                  uint8_t *ptrParam, uint16_t paramLen)
{
    TRACE(1, "%s", __func__);
}
#endif

#if defined(ANC_APP)
static void app_ibrt_sync_anc_status(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_SYNC_ANC_STATUS, p_buff, length);
}

static void app_ibrt_sync_anc_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_anc_tws_sync_change(p_buff, length);
}
#endif

#if defined(PSAP_APP)
static void app_ibrt_sync_psap_status(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_SYNC_PSAP_STATUS, p_buff, length);
}

static void app_ibrt_sync_psap_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(0, "[%s] TODO ...", __func__);
}
#endif

#if defined(ANC_ASSIST_ENABLED)
static void app_ibrt_sync_anc_assist_status(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_SYNC_ANC_ASSIST_STATUS, p_buff, length);
}

static void app_ibrt_sync_anc_assist_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    anc_assist_tws_sync_status_change(p_buff, length);
}
#endif

#if defined(__BIXBY_VOICE__)
static void app_ibrt_sync_bixby_status(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_with_rsp(APP_TWS_CMD_SYNC_BIXBY_STATE, p_buff, length);
}

static void app_ibrt_sync_bixby_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ibrt_bixby_status_send_handle(p_buff, length);
    tws_ctrl_send_rsp(APP_TWS_CMD_SYNC_BIXBY_STATE, rsp_seq, p_buff, length);
}

static void app_ibrt_sync_bixby_status_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ibrt_bixby_status_send_rsp_timeout_handle(p_buff, length);
}

static void app_ibrt_sync_bixby_status_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ibrt_bixby_status_send_rsp_handle(p_buff, length);
}
#endif

#if !defined(IBRT_UI) && defined(__REPORT_EVENT_TO_CUSTOMIZED_UX__)
static void app_ibrt_customif_share_info_send(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_with_rsp(APP_TWS_CMD_SHARE_LINK_INFO, p_buff, length);
}

static void app_ibrt_customif_share_info_cmd_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ctm_adpt_tws_share_info_cmd_cb(rsp_seq, p_buff, length);
}

static void app_ibrt_customif_share_info_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(0, "%s", __func__);
}

static void app_ibrt_customif_share_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ctm_adpt_tws_share_info_rsp_cb(rsp_seq, p_buff, length);
}
#endif

#ifdef ENABLE_ROLE_SWITCH_MONITOR
static void app_ibrt_customif_role_switch_monitor_send(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_ROLE_SWITCH_MONITOR, p_buff, length);
}

static void app_ibrt_customif_role_switch_monitor_cmd_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
}
#endif

static void app_ibrt_sync_audio_process(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_SYNC_AUDIO_PROCESS, p_buff, length);
}

static void app_ibrt_sync_audio_process_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    audio_process_tws_sync_change(p_buff, length);
}

static void app_ibrt_customif_control_sbm_send(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_with_rsp(APP_TWS_CMD_CONTROL_SBM, p_buff, length);
}

static void app_ibrt_customif_control_sbm_cmd_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    SBM_CONTROL_REQ_T* req = (SBM_CONTROL_REQ_T *)p_buff;
    sbm_control_local_scalable_sbm_feature_handler(req->device_id, req->isEnable);

    tws_ctrl_send_rsp(APP_TWS_CMD_CONTROL_SBM, rsp_seq, p_buff, length);
}

static void app_ibrt_customif_control_sbm_wait_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    SBM_CONTROL_REQ_T* req = (SBM_CONTROL_REQ_T *)p_buff;
    sbm_local_scalable_sbm_feature_updated_callback(req->device_id, req->isEnable, false);
}

static void app_ibrt_customif_control_sbm_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    SBM_CONTROL_REQ_T* req = (SBM_CONTROL_REQ_T *)p_buff;
    sbm_local_scalable_sbm_feature_updated_callback(req->device_id, req->isEnable, true);
}

static const app_tws_cmd_instance_t g_ibrt_custom_cmd_handler_table[]=
{
#ifdef GFPS_ENABLED
    {
        APP_TWS_CMD_SHARE_FASTPAIR_INFO,                "SHARE_FASTPAIR_INFO",
        app_ibrt_share_fastpair_info,
        app_ibrt_shared_fastpair_info_received_handler,  0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,          app_ibrt_custom_cmd_rsp_handler_null
    },
#endif

#ifdef __GMA_VOICE__
    {
        APP_TWS_CMD_GMA_SECRET_KEY,                     "GMA_SECRET_KEY_SYNC",
        app_ibrt_gma_secret_key_sync,
        app_ibrt_gma_secret_key_sync_handler,           0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null
    },
#endif

#ifdef BISTO_ENABLED
    {
        APP_TWS_CMD_BISTO_DIP_SYNC,                     "BISTO_DIP_SYNC",
        app_ibrt_bisto_dip_sync,
        app_ibrt_bisto_dip_sync_handler,                0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null
    },
#endif

#ifdef DUAL_MIC_RECORDING
    {
        APP_IBRT_CUSTOM_CMD_DMA_AUDIO,                          "TWS_CMD_DMA_AUDIO",
        app_ibrt_customif_audio_send,
        app_ibrt_customif_audio_send_handler,               0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null,
        app_ibrt_customif_audio_send_done,
    },
    {
        APP_IBRT_CUSTOM_CMD_UPDATE_BITRATE,                     "TWS_CMD_UPDATE_BITRATE",
        app_ibrt_customif_update_bitrate,
        app_ibrt_customif_update_bitrate_handler,               0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null,
        app_ibrt_customif_update_bitrate_done,
    },
    {
        APP_IBRT_CUSTOM_CMD_REPORT_BUF_LVL,                     "TWS_CMD_REPORT_BUF_LVL",
        app_ibrt_customif_report_buf_lvl,
        app_ibrt_customif_report_buf_lvl_handler,               0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null,
        app_ibrt_customif_report_buf_lvl_done,
    },
#endif

#if defined(ANC_APP)
    {
        APP_TWS_CMD_SYNC_ANC_STATUS,                            "SYNC_ANC_STATUS",
        app_ibrt_sync_anc_status,
        app_ibrt_sync_anc_status_handler,                       0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null
    },
#endif

#if defined(PSAP_APP)
    {
        APP_TWS_CMD_SYNC_PSAP_STATUS,                           "SYNC_PSAP_STATUS",
        app_ibrt_sync_psap_status,
        app_ibrt_sync_psap_status_handler,                      0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null
    },
#endif

#if defined(ANC_ASSIST_ENABLED)
    {
        APP_TWS_CMD_SYNC_ANC_ASSIST_STATUS,                     "SYNC_ANC_ASSIST_STATUS",
        app_ibrt_sync_anc_assist_status,
        app_ibrt_sync_anc_assist_status_handler,                0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null
    },
#endif

    {
        APP_TWS_CMD_SYNC_AUDIO_PROCESS,                         "SYNC_AUDIO_PROCESS",
        app_ibrt_sync_audio_process,
        app_ibrt_sync_audio_process_handler,                    0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null
    },

#if defined(__BIXBY_VOICE__)
    {
        APP_TWS_CMD_SYNC_BIXBY_STATE,                           "SYNC_BIXBY_STATE",
        app_ibrt_sync_bixby_status,
        app_ibrt_sync_bixby_status_handler,                     RSP_TIMEOUT_FAST_ACK,
        app_ibrt_sync_bixby_status_rsp_timeout_handler,         app_ibrt_sync_bixby_status_rsp_handler
    },
#endif

    {
        APP_TWS_CMD_CONTROL_SBM,                                "TWS_CONTROL_SBM",
        app_ibrt_customif_control_sbm_send,
        app_ibrt_customif_control_sbm_cmd_handler,              RSP_TIMEOUT_DEFAULT,
        app_ibrt_customif_control_sbm_wait_rsp_timeout_handler, app_ibrt_customif_control_sbm_rsp_handler
    },

#if !defined(IBRT_UI) && defined(__REPORT_EVENT_TO_CUSTOMIZED_UX__)
    {
        APP_TWS_CMD_SHARE_LINK_INFO,                            "TWS_SHARE_LINK_INFO",
        app_ibrt_customif_share_info_send,
        app_ibrt_customif_share_info_cmd_handler,               RSP_TIMEOUT_DEFAULT,
        app_ibrt_customif_share_info_rsp_timeout_handler,       app_ibrt_customif_share_info_rsp_handler
    },
#endif

#ifdef ENABLE_ROLE_SWITCH_MONITOR
    {
        APP_TWS_CMD_ROLE_SWITCH_MONITOR,                        "TWS_ROLE_SWITCH_MONITOR",
        app_ibrt_customif_role_switch_monitor_send,
        app_ibrt_customif_role_switch_monitor_cmd_handler,      0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null
    },
#endif
#if defined(SPA_AUDIO_ENABLE)
    {
        APP_TWS_CMD_SPA_SENS_DATA_SYNC,                    "APP_TWS_CMD_SPA_SENS_DATA_SYNC",
        app_spa_tws_sens_data_sync_snd_handler,
        app_spa_tws_sens_data_sync_rcv_handler,                   0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null
    },
#endif
};

int app_ibrt_customif_cmd_table_get(void **cmd_tbl, uint16_t *cmd_size)
{
    *cmd_tbl = (void *)&g_ibrt_custom_cmd_handler_table;
    *cmd_size = ARRAY_SIZE(g_ibrt_custom_cmd_handler_table);
    return 0;
}

#endif
