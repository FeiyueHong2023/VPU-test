/**
 * @file aob_bis_api.cpp
 * @author BES AI team
 * @version 0.1
 * @date 2020-08-31
 *
 * @copyright Copyright (c) 2015-2021 BES Technic.
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
 */

/*****************************header include********************************/
#include "app_bap_bc_scan_msg.h"
#include "app_gaf_define.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "aob_bis_api.h"
#include "hal_trace.h"
#include "ble_app_dbg.h"
#include "hal_aud.h"
#include "plat_types.h"
#include "heap_api.h"

#include "app_bap.h"
#include "app_gaf_custom_api.h"
#include "app_bap_bc_src_msg.h"
#include "app_tws_ctrl_thread.h"
#include "ble_audio_tws_cmd_handler.h"
#include "ble_audio_earphone_info.h"
#include "ble_audio_test.h"
#include "bap_bc_sink.h"
#include "bap_bc_assist_msg.h"

#include "gaf_bis_media_stream.h"
#include "aob_media_api.h"
#include "aob_bis_api.h"

#include "ble_audio_core.h"

/****************************for server(earbud)*****************************/

/*********************external function declaration*************************/

/************************private macro defination***************************/
#define AOB_BIS_TWS_SYNC_BCAST_ID_LEN   3
#define AOB_BIS_TWS_SYNC_BCAST_CODE_LEN 16

/************************private type defination****************************/

/**********************private function declaration*************************/

/************************private variable defination************************/
#ifdef CFG_BAP_BC
typedef struct{
    uint8_t  grp_lid;
    uint32_t bis_stream_bf;
    uint32_t ble_bis_select_bf;
    void (*bis_stream_lost_callback)();
    void (*bis_stream_private_data_callback)(uint8_t *buf, uint8_t buf_len);
} aob_bis_sink_env_t;

static aob_bis_sink_env_t aob_bis_sink_env = {0};
#endif

static BLE_AUD_CORE_EVT_CB_T *p_bis_sink_cb  = NULL;
static BLE_AUD_CORE_EVT_CB_T *p_bis_deleg_cb = NULL;

/*BIS TWS EVENT*/
void aob_bis_tws_sync_state_req_handler(uint8_t *buf)
{
    app_bap_bc_scan_env_t *aob_bap_scan_env = app_bap_bc_scan_get_scan_env();
    uint8_t ARRAY_EMPTY[GAP_KEY_LEN] = {0};

    if ((memcmp(&buf[0], &ARRAY_EMPTY[0], BAP_BC_BROADCAST_ID_LEN) == 0) &&
         aob_bap_scan_env->scan_state == APP_BAP_BC_SCAN_STATE_STREAMING)
    {
        aob_bis_sink_disable(ble_audio_earphone_info_get_bis_grp_lid());
    }
    else
    {
        ble_audio_earphone_info_set_bis_bcast_id(&buf[0]);
        /*if (memcmp(&bcast_id[AOB_BIS_TWS_SYNC_BCAST_ID_LEN], &ARRAY_EMPTY[0], GAP_KEY_LEN) != 0)
        {
            ble_audio_earphone_info_set_bis_bcast_code(&buf[AOB_BIS_TWS_SYNC_BCAST_ID_LEN]);
        }*/

        uint8_t *state = &app_bap_bc_scan_get_scan_env()->scan_state;
        if (*state > APP_BAP_BC_SCAN_STATE_SYNCHRONIZED)
        {
            *state = APP_BAP_BC_SCAN_STATE_SYNCHRONIZED;
        }
        else
        {
            app_bap_bc_scan_start(APP_BAP_BC_SINK_SCAN);
        }
    }
}

void aob_bis_tws_sync_state_req(void)
{
#ifdef AOB_BIS_TWS_SYNC_ENABLED
    uint8_t sync_data[AOB_BIS_TWS_SYNC_BCAST_ID_LEN+AOB_BIS_TWS_SYNC_BCAST_CODE_LEN] = {0};
    uint8_t sync_data_len = 0;
    uint8_t *bcast_id   = ble_audio_earphone_info_get_bis_bcast_id();
    //uint8_t *bcast_code = ble_audio_earphone_info_get_bis_bcast_code();
    app_bap_bc_scan_env_t *aob_bap_scan_env = app_bap_bc_scan_get_scan_env();

    if(aob_bap_scan_env)
    {
        LOG_I("%s, scan state = %d", __func__, aob_bap_scan_env->scan_state);

        if (GETB(app_bap_get_role_bit_filed(), BAP_ROLE_SUPP_BC_SINK))
        {
            if (aob_bap_scan_env->scan_state >= APP_BAP_BC_SCAN_STATE_SYNCHRONIZED)
            {
                memcpy(&sync_data[0], bcast_id, AOB_BIS_TWS_SYNC_BCAST_ID_LEN);
                sync_data_len += AOB_BIS_TWS_SYNC_BCAST_ID_LEN;
                //memcpy(&sync_data[AOB_BIS_TWS_SYNC_BCAST_ID_LEN], bcast_id, AOB_BIS_TWS_SYNC_BCAST_CODE_LEN);
                //sync_data_len += AOB_BIS_TWS_SYNC_BCAST_CODE_LEN;
                tws_ctrl_send_cmd(IBRT_AOB_CMD_EXCH_BLE_AUDIO_INFO, bcast_id, sync_data_len);
            }
        }
    }
#endif
}

static bool isBisCodeConfiguredViaExternal = false;
static uint8_t configuredBisCode[APP_GAP_KEY_LEN];
#ifdef AOB_MOBILE_ENABLED
/*BAP Broadcast Source APIs*/
void aob_bis_src_set_encrypt(uint8_t big_idx, uint8_t *bcast_code)
{
    if (NULL != bcast_code)
    {
        isBisCodeConfiguredViaExternal = true;        
        memcpy(configuredBisCode, bcast_code, sizeof(app_gaf_bc_code_t));
        if (ble_audio_is_ux_bis_src())
        {
            app_bap_bc_src_set_encrypt(big_idx, APP_BAP_DFT_BC_SRC_IS_ENCRYPTED, (app_gaf_bc_code_t *)bcast_code);
        }
    }
}

void aob_bis_src_disable_encrypt(uint8_t big_idx)
{
    if (ble_audio_is_ux_bis_src())
    {
        app_bap_bc_src_set_encrypt(big_idx, false, NULL);
    }
}

void aob_bis_src_set_stream_codec_cfg(uint8_t big_idx, uint8_t stream_lid, uint16_t frame_octet, uint8_t sampling_freq)
{
    app_bap_bc_src_set_stream_codec_cfg(big_idx, stream_lid, frame_octet, sampling_freq);
}

void aob_bis_src_enable_pa(uint8_t big_idx)
{
    app_bap_bc_src_grp_t *p_grp = app_bap_bc_src_get_big_info_by_big_idx(big_idx);
    app_bap_bc_src_enable_pa(p_grp);
}

void aob_bis_src_disable_pa(uint8_t grp_lid)
{
    app_bap_bc_src_disable_pa(grp_lid);
}

void aob_bis_src_enable(uint8_t big_idx)
{
    app_bap_bc_src_grp_t *p_grp = app_bap_bc_src_get_big_info_by_big_idx(big_idx);
    app_bap_bc_src_enable(p_grp);
}

void aob_bis_src_disable(uint8_t grp_lid)
{
    app_bap_bc_src_disable(grp_lid);
}

void aob_bis_src_add_group_req(uint8_t big_idx)
{
    app_bap_bc_src_start(big_idx);
}

void aob_bis_src_start_streaming(uint8_t big_idx, uint32_t stream_lid_bf)
{
    app_bap_bc_src_start_streaming(big_idx, stream_lid_bf);
}

void aob_bis_src_update_metadata(uint8_t grp_lid, uint8_t sgrp_lid, app_bap_cfg_metadata_t* metadata)
{
    app_bap_bc_src_update_metadata_req(grp_lid, sgrp_lid, metadata);
}

void aob_bis_src_stop_streaming(uint8_t big_idx, uint32_t stream_lid_bf)
{
    app_bap_bc_src_stop_streaming(big_idx, stream_lid_bf);
}

static void aob_bis_src_enabled_cb(app_bap_bc_src_grp_t *p_grp)
{
    //big state changed APP_BAP_BC_SRC_STATE_CONFIGURED -> APP_BAP_BC_SRC_STATE_STREAMING
    LOG_D("%s ", __func__);
    uint32_t stream_lid_bf = 0;
    for (uint8_t stream_lid = 0; stream_lid < p_grp->nb_streams; stream_lid++)
    {
        stream_lid_bf |= (1 << stream_lid);
    }//the input should be stream_lid_bf not stream_lid
    app_bap_bc_src_start_streaming(app_bap_bc_src_find_big_idx(p_grp->grp_lid),stream_lid_bf);
}

static void aob_bis_src_disabled_cb(uint8_t grp_lid)
{
    //big state changed APP_BAP_BC_SRC_STATE_STREAMING -> APP_BAP_BC_SRC_STATE_CONFIGURED
    LOG_D("%s ", __func__);
}

static void aob_bis_src_pa_enabled_cb(app_bap_bc_src_grp_t *p_grp)
{
    //big state changed APP_BAP_BC_SRC_STATE_IDLE -> APP_BAP_BC_SRC_STATE_CONFIGURED
    LOG_D("%s ", __func__);
    app_bap_bc_src_enable(p_grp);
}

static void aob_bis_src_pa_disabled_cb(uint8_t grp_lid)
{
    //big state changed APP_BAP_BC_SRC_STATE_CONFIGURED -> APP_BAP_BC_SRC_STATE_IDLE
    LOG_D("%s ", __func__);
    app_bap_bc_src_remove_group_cmd(grp_lid);
}

static void aob_bis_src_stream_started_cb(uint16_t bis_hdl)
{
    app_bap_bc_src_grp_t *app_p_grp = app_bap_bc_src_get_big_info_by_bis_hdl(bis_hdl);
    uint8_t status = CO_ERROR_NO_ERROR;

    if (NULL == app_p_grp)
    {
        LOG_D("Bis src can not start audio stream, find grp info err");
        return;
    }

    uint8_t stream_lid = app_bap_bc_src_find_stream_lid(bis_hdl);
    app_p_grp->stream_info->bis_hdl = bis_hdl;
    aob_bis_src_set_stream_codec_cfg(app_p_grp->grp_lid, stream_lid, 120, BAP_SAMPLING_FREQ_48000HZ);
    status = app_bap_bc_src_tx_stream_start(bis_hdl);

    if (status != CO_ERROR_NO_ERROR)
    {
        LOG_W("aob_bap bc src start tx stream failed status 0x%02x",status);
    }
    else
    {
        gaf_bis_src_audio_stream_start_handler(app_p_grp->grp_lid);
        LOG_D("Bis src stream was started");
    }
}

static void aob_bis_src_stream_stoped_cb(uint16_t bis_hdl)
{
    app_bap_bc_src_grp_t *app_p_grp = app_bap_bc_src_get_big_info_by_bis_hdl(bis_hdl);

    if (NULL == app_p_grp)
    {
        LOG_D("Bis src can not stop audio stream, find grp info err");
        return;
    }
    app_bap_bc_src_tx_stream_stop(bis_hdl);
    gaf_bis_src_audio_stream_stop_handler(app_p_grp->grp_lid);
    app_p_grp->stream_info->bis_hdl = GATT_INVALID_HDL;
    LOG_D("Bis src stream was stoped");

    aob_bis_src_disable(app_p_grp->grp_lid);
}

static src_event_handler_t src_event_cb = {
    .bis_src_enabled_ind       = aob_bis_src_enabled_cb,
    .bis_src_disabled_ind      = aob_bis_src_disabled_cb,
    .bis_src_pa_enabled_ind    = aob_bis_src_pa_enabled_cb,
    .bis_src_pa_disabled_ind   = aob_bis_src_pa_disabled_cb,
    .bis_src_stream_started_cb = aob_bis_src_stream_started_cb,
    .bis_src_stream_stoped_cb  = aob_bis_src_stream_stoped_cb,
};

static bool aob_bis_src_cfg_info_cb_func(app_bap_bc_src_cfg_info_t *info)
{
    TRACE(1, "aob src cfg init info");
    info->nb_streams = APP_BAP_DFT_BC_SRC_NB_STREAMS;
    info->nb_subgroups = APP_BAP_DFT_BC_SRC_NB_SUBGRPS;
    info->sdu_intv_us = APP_BAP_DFT_BC_SRC_SDU_INTV_US;
    info->max_sdu = APP_BAP_DFT_BC_SRC_MAX_SDU_SIZE;
    info->max_tlatency = APP_BAP_DFT_BC_SRC_MAX_TRANS_LATENCY_MS;
    info->adv_intv_min_slot = APP_BAP_DFT_BC_SRC_ADV_INTERVAL;
    info->adv_intv_max_slot = APP_BAP_DFT_BC_SRC_ADV_INTERVAL;
    info->adv_intv_min_frame = APP_BAP_DFT_BC_SRC_PERIODIC_INTERVAL;
    info->adv_intv_max_frame = APP_BAP_DFT_BC_SRC_PERIODIC_INTERVAL;
    info->encrypted = APP_BAP_DFT_BC_SRC_IS_ENCRYPTED;
    app_bap_add_data_set(info->bcast_id.id, BAP_BC_BROADCAST_ID_LEN);//Data that can be used by scanning devices to help find broadcast Audio Streams.

    if (info->encrypted)
    {
        app_bap_add_data_set(info->bcast_code.bcast_code, APP_GAP_KEY_LEN);//BIS source bcast_code set,bcast_code will be filled by customer,set same value to test encrypted BIS
    }

    return true;
}
void aob_bis_src_mobile_api_init(void)
{
    aob_mgr_gaf_mobile_src_evt_handler_register(&src_event_cb);
    app_bap_bc_src_register_cfg_info_cb(aob_bis_src_cfg_info_cb_func);
}
#endif

/*BAP Broadcast Sink APIs*/
void aob_bis_sink_enable(uint8_t pa_lid, uint8_t mse, uint8_t stream_pos_bf,
                              uint16_t timeout_10ms, uint8_t encrypted, uint8_t *bcast_code,
                              uint8_t *bcast_id)
{
    app_bap_bc_sink_enable_t p_sink_enable;
    p_sink_enable.pa_lid = pa_lid;
    p_sink_enable.mse = mse;
    p_sink_enable.stream_pos_bf = stream_pos_bf;
    p_sink_enable.timeout_10ms = timeout_10ms;
    p_sink_enable.encrypted = encrypted;
    memcpy(&p_sink_enable.bcast_id.id[0], &bcast_id[0], BAP_BC_BROADCAST_ID_LEN);
    if (p_sink_enable.encrypted)
    {
        if (bcast_code)
        {
            memcpy(&p_sink_enable.bcast_code, bcast_code, sizeof(app_gaf_bc_code_t));
        }
    }

    app_bap_bc_sink_enable(&p_sink_enable);
}

void aob_bis_sink_disable(uint8_t grp_lid)
{
    app_bap_bc_sink_disable(grp_lid);
}

void aob_bis_sink_start_scan(void)
{
    app_bap_bc_scan_start(APP_BAP_BC_SINK_SCAN);
}

void aob_bis_sink_stop_scan(void)
{
    app_bap_bc_scan_stop();
}

void aob_bis_sink_scan_pa_sync(uint8_t *addr, uint8_t addr_type, uint8_t adv_sid)
{
    app_gaf_per_adv_bdaddr_t addr_t;
    memcpy(&addr_t.addr, addr, APP_GAP_BD_ADDR_LEN);
    addr_t.addr_type = addr_type;
    addr_t.adv_sid = adv_sid;
    app_bap_bc_scan_pa_sync(&addr_t);
}

void aob_bis_sink_scan_pa_terminate(uint8_t pa_lid)
{
    app_bap_bc_scan_pa_terminate(pa_lid);
}

void aob_bis_sink_start_streaming(uint8_t grp_lid, uint32_t stream_pos_bf,
                                         uint8_t codec_type, AOB_BIS_MEDIA_INFO_T *media_info)
{
    AOB_BIS_GROUP_INFO_T *aob_bis_group_info = ble_audio_earphone_info_get_bis_group_info();
    app_bap_bc_scan_stream_info_t stream_info = {0};

    stream_info.sgrp_id = 0;
    stream_info.stream_pos = stream_pos_bf;
    stream_info.codec_id.codec_id[0] = APP_GAF_CODEC_TYPE_LC3;
    stream_info.param.location_bf = (GAF_BAP_AUDIO_LOCATION_SIDE_LEFT | GAF_BAP_AUDIO_LOCATION_SIDE_RIGHT);
    stream_info.param.frame_octet = media_info->frame_octet;
    stream_info.param.sampling_freq = media_info->sampling_freq;
    stream_info.param.frame_dur     = GAF_BAP_FRAME_DURATION_10MS;
    stream_info.param.frames_sdu    = 1;
    stream_info.ltv_len         = 0;

    aob_bis_group_info->play_stream_num = 1;
    aob_bis_group_info->play_ch_num     = 1;
    aob_bis_group_info->play_stream_info[0].stream_lid = \
        ble_audio_earphone_info_bis_stream_pos_2_stream_lid(stream_pos_bf);
    aob_bis_group_info->play_stream_info[0].select_ch_bf = 0x01;
    app_bap_bc_sink_streaming_config(grp_lid, stream_pos_bf, codec_type, NULL, &stream_info, \
                                     (app_bap_bc_sink_audio_streaming_t*)&aob_bis_group_info->play_stream_info[0].stream_info);

    app_bap_bc_sink_start_streaming((app_bap_bc_sink_audio_streaming_t*)&aob_bis_group_info->play_stream_info[0].stream_info);
}

void aob_bis_sink_stop_streaming(uint8_t grp_lid, uint8_t stream_pos)
{
    app_bap_bc_sink_stop_streaming(grp_lid, stream_pos);
}

void aob_bis_sink_set_player_channel(uint32_t channel_bf)
{
    aob_bis_sink_env.ble_bis_select_bf = channel_bf;
}

void aob_bis_sink_private_data_callback(uint8_t *buf, uint8_t buf_len)
{
    if (aob_bis_sink_env.bis_stream_private_data_callback)
    {
        aob_bis_sink_env.bis_stream_private_data_callback(buf, buf_len);
    }
}

static uint8_t aob_bis_get_bf_num(uint32_t data_bf)
{
    uint8_t cnt = 0;
    uint32_t stream_bf = data_bf;

    while (stream_bf)
    {
        cnt++;
        stream_bf &= (stream_bf - 1);
    }

    return cnt;
}

static void aob_bis_sink_select_play_channel(uint8_t stream_id, uint32_t select_channel_bf,
                                                app_bap_bc_scan_stream_info_t *stream_info,
                                                AOB_BIS_GROUP_INFO_T *aob_bis_group_info)
{
    uint8_t ch_num = 0;
    uint8_t loc_bf_count = 0;
    uint32_t stream_location_bf = stream_info->param.location_bf;

    if (aob_bis_group_info->play_ch_num >= 2)
    {
        return;
    }

    if (select_channel_bf == 0)
    {
        // When no channel is selected, the channel of stream0 will be played by default,
        // with a maximum of two channels
        ch_num = aob_bis_get_bf_num(stream_location_bf);
        if (ch_num > 1)
        {
            aob_bis_group_info->play_ch_num = 2;
            aob_bis_group_info->play_stream_info[0].select_ch_bf = 3;
        }
        else
        {
            aob_bis_group_info->play_ch_num = 1;
            aob_bis_group_info->play_stream_info[0].select_ch_bf = 1;
        }
    }
    else
    {
        // Map the selected channel to the bit bit of the data stream
        for (int i=0; i<32; ++i)
        {
            if (stream_location_bf & (1UL<<i))
            {
                if (select_channel_bf & (1UL<<i))
                {
                    aob_bis_group_info->play_ch_num++;
                    aob_bis_group_info->play_stream_info[stream_id].select_ch_bf |= \
                        (1UL << loc_bf_count);
                    select_channel_bf &= ~(1UL << i);
                }
                loc_bf_count++;
            }
            stream_location_bf &= ~(1UL << i);

            if ((!select_channel_bf) || (!stream_location_bf) || (aob_bis_group_info->play_ch_num >=2))
            {
                break;
            }
        }
    }
}

static void aob_bis_sink_select_play_stream(uint8_t grp_lid, uint32_t stream_pos_bf, AOB_BIS_GROUP_INFO_T *aob_bis_group_info)
{
    /// Get streaming info from PA's BASE
    uint8_t stream_lid;
    uint8_t ch_num;
    uint32_t select_ch_bf = aob_bis_sink_env.ble_bis_select_bf;
    app_bap_bc_scan_pa_report_info_t *pa_info = app_bap_bc_scan_pa_idx_get(aob_bis_group_info->pa_lid);
    app_bap_bc_scan_stream_info_t *stream_info_p[2] = {NULL};
    app_bap_bc_scan_subgrp_info_t *subgroup_info_p[2] = {NULL};

    LOG_I("select_ch_bf = %u", select_ch_bf);

    /// select player stream, max two stream
    if (select_ch_bf)
    {
        for (int num = 0; num < pa_info->group_info.nb_streams; ++num)
        {
            if (pa_info->stream_info[num].param.location_bf & select_ch_bf)
            {
                if(!stream_info_p[0])
                {
                    LOG_I("0-finded pa_info->stream_info[num].param.location_bf = %u", pa_info->stream_info[num].param.location_bf);
                    LOG_I("0-finded stream pos = %d", pa_info->stream_info[num].stream_pos);
                    stream_info_p[0] = &pa_info->stream_info[num];
                }
                else if(!stream_info_p[1])
                {
                    LOG_I("1-finded pa_info->stream_info[num].param.location_bf = %u", pa_info->stream_info[num].param.location_bf);
                    LOG_I("1-finded stream pos = %d", pa_info->stream_info[num].stream_pos);
                    stream_info_p[1] = &pa_info->stream_info[num];
                    break;
                }
            }
        }
    }

    /// if not find stream, used stream_0
    if (!stream_info_p[0])
    {
        stream_info_p[0] = &pa_info->stream_info[0];
    }

    /// find stream in subgrep
    for (int num = 0; num < pa_info->group_info.nb_subgroups; ++num)
    {
        if (pa_info->subgroup_info[num].sgrp_id == stream_info_p[0]->sgrp_id)
        {
            LOG_I("0-finded pa_info->subgroup_info[num].sgrp_id = %d", pa_info->subgroup_info[num].sgrp_id);
            subgroup_info_p[0] = &pa_info->subgroup_info[num];
        }
        if ((stream_info_p[1]) && pa_info->subgroup_info[num].sgrp_id == stream_info_p[1]->sgrp_id)
        {
            LOG_I("1-finded pa_info->subgroup_info[num].sgrp_id = %d", pa_info->subgroup_info[num].sgrp_id);
            subgroup_info_p[1] = &pa_info->subgroup_info[num];
        }
    }

    /// set sink stream cfg
    if (stream_info_p[0])
    {
        app_bap_bc_sink_streaming_config(grp_lid, stream_info_p[0]->stream_pos, APP_GAF_CODEC_TYPE_LC3, \
                                     subgroup_info_p[0], stream_info_p[0],
                                     (app_bap_bc_sink_audio_streaming_t*)&aob_bis_group_info->play_stream_info[0].stream_info);

        if (stream_info_p[0]->param.location_bf & select_ch_bf)
        {
            aob_bis_sink_select_play_channel(0, aob_bis_sink_env.ble_bis_select_bf, stream_info_p[0], \
                                                                                aob_bis_group_info);
        }
        else
        {
            aob_bis_sink_select_play_channel(0, 0, stream_info_p[0], aob_bis_group_info);
        }
        aob_bis_group_info->play_stream_num = 1;
        stream_lid = ble_audio_earphone_info_bis_stream_pos_2_stream_lid(stream_info_p[0]->stream_pos);
        aob_bis_group_info->play_stream_info[0].stream_lid = stream_lid;
        aob_bis_group_info->play_stream_info[0].bis_hdl    = aob_bis_group_info->bis_hdl[stream_lid];
        ch_num = aob_bis_get_bf_num(stream_info_p[0]->param.location_bf);
        aob_bis_group_info->play_stream_info[0].blocks_size = stream_info_p[0]->param.frame_octet*ch_num;
    }

    if (stream_info_p[1])
    {
       app_bap_bc_sink_streaming_config(grp_lid, stream_info_p[1]->stream_pos, APP_GAF_CODEC_TYPE_LC3, \
                                    subgroup_info_p[1], stream_info_p[1],
                                    (app_bap_bc_sink_audio_streaming_t*)&aob_bis_group_info->play_stream_info[1].stream_info);
       aob_bis_sink_select_play_channel(1, aob_bis_sink_env.ble_bis_select_bf, stream_info_p[1], \
                                                                           aob_bis_group_info);
       aob_bis_group_info->play_stream_num++;
       stream_lid = ble_audio_earphone_info_bis_stream_pos_2_stream_lid(stream_info_p[1]->stream_pos);
       aob_bis_group_info->play_stream_info[1].stream_lid = stream_lid;
       aob_bis_group_info->play_stream_info[1].bis_hdl    = aob_bis_group_info->bis_hdl[stream_lid];
       ch_num = aob_bis_get_bf_num(stream_info_p[1]->param.location_bf);
       aob_bis_group_info->play_stream_info[1].blocks_size = stream_info_p[1]->param.frame_octet*ch_num;
    }
}

static void aob_bis_sink_state_cb(uint8_t grp_lid, uint8_t state, uint32_t stream_pos_bf)
{
    LOG_D("Bis sink state is %d", state);
    AOB_BIS_GROUP_INFO_T *aob_bis_group_info = ble_audio_earphone_info_get_bis_group_info();

    if (NULL == aob_bis_group_info)
    {
        LOG_I("Bis earphone info get error");
        return;
    }

    switch (state)
    {
        case BAP_BC_SINK_FAILED:
        case BAP_BC_SINK_CANCELLED:
        case BAP_BC_SINK_MIC_FAILURE:
        {
            app_bap_bc_scan_env_t *aob_bap_scan_env = app_bap_bc_scan_get_scan_env();
            aob_bap_scan_env->scan_state = APP_BAP_BC_SCAN_STATE_SYNC_LOST;
            if (APP_BAP_BC_DELEG_TRIGGER != aob_bap_scan_env->scan_trigger_method)
            {
                aob_bis_sink_scan_pa_terminate(aob_bis_group_info->pa_lid);
            }

#ifdef AOB_BIS_TWS_SYNC_ENABLED
            uint8_t ARRAY_EMPTY[BAP_BC_BROADCAST_ID_LEN] = {0};
            ble_audio_earphone_info_set_bis_bcast_id(ARRAY_EMPTY);
            aob_bis_tws_sync_state_req();
#endif
        }
            break;
        case BAP_BC_SINK_ESTABLISHED:
        {
            aob_bis_sink_select_play_stream(grp_lid, stream_pos_bf, aob_bis_group_info);
            LOG_I("Play_stream_num = %d, stream_pos_bf = %u",aob_bis_group_info->play_stream_num, stream_pos_bf);
            if (aob_bis_group_info->play_stream_num == 1)
            {
                app_bap_bc_sink_start_streaming((app_bap_bc_sink_audio_streaming_t *)&aob_bis_group_info->play_stream_info[0].stream_info);
            }
            if (aob_bis_group_info->play_stream_num >= 2)
            {
                app_bap_bc_sink_start_streaming((app_bap_bc_sink_audio_streaming_t *)&aob_bis_group_info->play_stream_info[0].stream_info);
                app_bap_bc_sink_start_streaming((app_bap_bc_sink_audio_streaming_t *)&aob_bis_group_info->play_stream_info[1].stream_info);
            }
        }
            break;
        case BAP_BC_SINK_LOST:
        case BAP_BC_SINK_PEER_TERMINATE:
        case BAP_BC_SINK_UPPER_TERMINATE:
        {
            app_bap_bc_scan_env_t *aob_bap_scan_env = app_bap_bc_scan_get_scan_env();
            aob_bap_scan_env->scan_state = APP_BAP_BC_SCAN_STATE_SYNC_LOST;

            app_bap_bc_sink_rx_stream_stop(aob_bis_group_info->play_stream_info[0].bis_hdl, aob_bis_group_info->grp_lid);
            gaf_bis_audio_stream_stop_handler(aob_bis_group_info->play_stream_info[0].stream_lid);
            if (aob_bis_group_info->play_stream_num > 1)
            {
                app_bap_bc_sink_rx_stream_stop(aob_bis_group_info->play_stream_info[1].bis_hdl, aob_bis_group_info->grp_lid);
                gaf_bis_audio_stream_stop_handler(aob_bis_group_info->play_stream_info[1].stream_lid);
            }

            if (APP_BAP_BC_DELEG_TRIGGER != aob_bap_scan_env->scan_trigger_method)
            {
                aob_bis_sink_scan_pa_terminate(aob_bis_group_info->pa_lid);
            }

#ifdef AOB_BIS_TWS_SYNC_ENABLED
            uint8_t ARRAY_EMPTY[BAP_BC_BROADCAST_ID_LEN] = {0};
            ble_audio_earphone_info_set_bis_bcast_id(ARRAY_EMPTY);
            aob_bis_tws_sync_state_req();
#endif
            ble_audio_earphone_info_clear_bis_group_info();
            if (aob_bis_sink_env.bis_stream_lost_callback)
            {
                aob_bis_sink_env.bis_stream_lost_callback();
            }
        }
            break;
        default:
            LOG_D("unkonw sink status %d", state);
            break;
    }

    if ((NULL != p_bis_sink_cb) && (NULL != p_bis_sink_cb->ble_bis_sink_status_cb)){
        p_bis_sink_cb->ble_bis_sink_status_cb(grp_lid, state, stream_pos_bf);
    }
}

static void aob_bis_sink_enabled_cb(uint8_t grp_lid)
{
    LOG_I("Bis sink %d was enabled", grp_lid);
    ble_audio_earphone_info_set_bis_grp_lid(grp_lid);

    uint8_t nb_ase = 0;
    uint8_t ase_lid_list[APP_BAP_DFT_ASCS_NB_ASE_CHAR] = {0};

    aob_bis_sink_env.grp_lid = grp_lid;
    for (int con_lid = 0; con_lid < MOBILE_CONNECTION_MAX; con_lid++)
    {
        nb_ase = aob_media_get_curr_streaming_ase_lid_list(con_lid, ase_lid_list);
        for (int idx = 0; idx < nb_ase; idx++)
        {
            TRACE(2, "stop cis before bis start, ase_lid = %d", ase_lid_list[idx]);
            aob_media_disable_stream(ase_lid_list[idx]);
        }
    }

    if ((NULL != p_bis_sink_cb) && (NULL != p_bis_sink_cb->ble_bis_sink_enabled_cb)) {
        p_bis_sink_cb->ble_bis_sink_enabled_cb(grp_lid);
    }
}

static void aob_bis_sink_disabled_cb(uint8_t grp_lid)
{
    LOG_I("Bis sink %d was disabled", grp_lid);
    ble_audio_earphone_info_set_bis_grp_lid(0);

    if ((NULL != p_bis_sink_cb) && (NULL != p_bis_sink_cb->ble_bis_sink_disabled_cb)){
        p_bis_sink_cb->ble_bis_sink_disabled_cb(grp_lid);
    }
}

static void aob_bis_sink_stream_started_cb(uint8_t stream_pos)
{
    AOB_BIS_GROUP_INFO_T *aob_bis_group_info = ble_audio_earphone_info_get_bis_group_info();
    uint8_t stream_lid = ble_audio_earphone_info_bis_stream_pos_2_stream_lid(stream_pos);

#ifdef AOB_BIS_TWS_SYNC_ENABLED
    ble_audio_earphone_info_set_bis_bcast_id(aob_bis_group_info->bcast_id);
    aob_bis_tws_sync_state_req();
#endif

    uint8_t status = app_bap_bc_sink_rx_stream_start(aob_bis_group_info->bis_hdl[stream_lid],\
                                    aob_bis_group_info->group_info.max_sdu,\
                                    aob_bis_group_info->grp_lid);
    if (!status)
    {
        gaf_bis_audio_stream_start_handler(stream_lid);
        LOG_I("Bis sink pos %d stream was started,", stream_pos);
    }

    aob_bis_sink_env.bis_stream_bf |= 1UL<<stream_pos;
    
    if ((NULL != p_bis_sink_cb) && (NULL != p_bis_sink_cb->ble_bis_sink_stream_start_cb)){
		p_bis_sink_cb->ble_bis_sink_stream_start_cb(stream_lid);
    } 
 
}

static void aob_bis_sink_stream_stoped_cb(uint8_t stream_pos)
{
    AOB_BIS_GROUP_INFO_T *aob_bis_group_info = ble_audio_earphone_info_get_bis_group_info();
    uint8_t stream_lid = ble_audio_earphone_info_bis_stream_pos_2_stream_lid(stream_pos);

    app_bap_bc_sink_rx_stream_stop(aob_bis_group_info->bis_hdl[stream_lid], 0);
    gaf_bis_audio_stream_stop_handler(stream_lid);

        aob_bis_sink_env.bis_stream_bf &= ~(1<<stream_pos);
    if (aob_bis_sink_env.bis_stream_bf)
    {
        for (int i= 0; i<32; ++i)
        {
            if (aob_bis_sink_env.bis_stream_bf & (1UL<<i))
            {
                app_bap_bc_sink_stop_streaming(aob_bis_group_info->grp_lid, i);
                break;
            }
        }
    }
    else
    {
        app_bap_bc_sink_disable(aob_bis_group_info->grp_lid);
    }

    LOG_I("Bis sink pos %d stream was stoped", stream_pos);
    if ((NULL != p_bis_sink_cb) && (NULL != p_bis_sink_cb->ble_bis_sink_stream_stoped_cb)){
        p_bis_sink_cb->ble_bis_sink_stream_stoped_cb(stream_lid);
    }
}

static sink_event_handler_t sink_event_cb ={
    .bis_sink_state_cb          = aob_bis_sink_state_cb,
    .bis_sink_enabled_cb        = aob_bis_sink_enabled_cb,
    .bis_sink_disabled_cb       = aob_bis_sink_disabled_cb,
    .bis_sink_stream_started_cb = aob_bis_sink_stream_started_cb,
    .bis_sink_stream_stoped_cb  = aob_bis_sink_stream_stoped_cb,
};

void aob_bis_sink_api_init(void)
{
    p_bis_sink_cb = ble_audio_get_evt_cb();
    aob_mgr_gaf_sink_evt_handler_register(&sink_event_cb);
}

/*BAP Broadcast Scan APIs*/
void aob_bis_scan_set_scan_param(uint16_t scan_timeout_s, uint16_t intv_1m_slot,
        uint16_t intv_coded_slot, uint16_t wd_1m_slot, uint16_t wd_coded_slot)
{
    app_bap_bc_scan_param_t scan_param;
    scan_param.intv_1m_slot = intv_1m_slot;
    scan_param.intv_coded_slot = intv_coded_slot;
    scan_param.wd_1m_slot = wd_1m_slot;
    scan_param.wd_coded_slot = wd_coded_slot;
    app_bap_bc_scan_set_scan_param(scan_timeout_s, &scan_param);
}

static void aob_bis_scan_state_idle_cb(void)
{
    app_bap_bc_scan_env_t *aob_bap_scan_env = app_bap_bc_scan_get_scan_env();
    aob_bap_scan_env->scan_state = APP_BAP_BC_SCAN_STATE_IDLE;
    LOG_D("%s scan_state_idle", __func__);
}

static void aob_bis_scan_state_scanning_cb(void)
{
    LOG_D("%s scan_state_scanning", __func__);
}

static void aob_bis_scan_state_synchronizing_cb(void)
{
    LOG_D("%s scan_state_synchronizing", __func__);
}

static void aob_bis_scan_state_synchronized_cb(void)
{
    LOG_D("%s scan_state_synchronized", __func__);
}

static void aob_bis_scan_state_streaming_cb(app_gaf_bc_scan_state_stream_t *p_scan_state)
{
    AOB_BIS_GROUP_INFO_T *aob_bis_group_info = ble_audio_earphone_info_get_bis_group_info();

    if (APP_BAP_BC_SINK_TRIGGER == p_scan_state->scan_trigger_method)
    {
        app_bap_bc_sink_enable_t p_sink_enable;
        p_sink_enable.pa_lid = aob_bis_group_info->pa_lid;
        p_sink_enable.mse = 0;
        ///TODO:Maybe only need to sink with one bis, not all in stream_pos_bf
        p_sink_enable.stream_pos_bf = aob_bis_group_info->stream_pos_bf;
        p_sink_enable.timeout_10ms = 100;
        app_bap_bc_scan_env_t* aob_app_bc_scan = (app_bap_bc_scan_env_t *)app_bap_bc_scan_get_scan_env;
        memcpy(&p_sink_enable.bcast_id.id,&aob_app_bc_scan->bcast_id.id,BAP_BC_BROADCAST_ID_LEN);//get bcast_id

        p_sink_enable.encrypted = p_scan_state->encrypted;//BIS should get encrypted from BIGInfo_Advertising_Report
        if (p_sink_enable.encrypted){
            if (isBisCodeConfiguredViaExternal)
            {
                memcpy(p_sink_enable.bcast_code.bcast_code, configuredBisCode, APP_GAP_KEY_LEN);
            }
            else
            {
                app_bap_add_data_set(p_sink_enable.bcast_code.bcast_code, APP_GAP_KEY_LEN);//BIS slave&master bcast_code set,bcast_code will be filled by customer,set same value to test encrypted BIS
            }
            LOG_D("%s slave&master generate bcast_code %d", __func__, p_sink_enable.stream_pos_bf);
        }
        app_bap_bc_sink_enable(&p_sink_enable);
    }

    if (APP_BAP_BC_DELEG_TRIGGER != p_scan_state->scan_trigger_method)
    {
        app_bap_bc_scan_stop();
    }
}

static void aob_bis_scan_pa_sync_req_cb(uint8_t pa_lid)
{
    LOG_D("%s pa_lid %d", __func__, pa_lid);
    aob_bis_deleg_pa_sync_ri(pa_lid, true);
}

static void aob_bis_scan_pa_terminate_req_cb(uint8_t pa_lid)
{
    LOG_D("%s pa_lid %d", __func__, pa_lid);
    aob_bis_deleg_pa_terminate_ri(pa_lid, true);
}

static void aob_bis_scan_pa_established_cb(uint8_t pa_lid, uint8_t *addr, uint8_t addr_type,
                                                                        uint8_t adv_sid, uint16_t serv_data)
{
    LOG_D("aob_bap bc scan pa established pa_lid = %d, serv_data = 0x%04x, addr:", pa_lid, serv_data);
    DUMP8("%02x ", addr, GAP_BD_ADDR_LEN);
    ble_audio_earphone_info_set_bis_pa_lid(pa_lid);
}

static void aob_bis_scan_pa_terminated_cb(uint8_t pa_lid, uint16_t reason)
{
    LOG_D("aob_bap bc scan pa terminated pa_lid %d reason 0x%04x", pa_lid, reason);
    ble_audio_earphone_info_set_bis_pa_lid(0);
}

static void aob_bis_scan_stream_info_report_cb(app_gaf_bc_scan_stream_report_ind_t* stream_info)
{
    LOG_D("aob_bap bc scan stream pos %d", stream_info->stream_pos);

    ble_audio_earphone_info_set_bis_stream_pos_bf(1 << (stream_info->stream_pos - 1));
}

static void aob_bis_scan_big_info_report_cb(app_gaf_bc_scan_big_info_report_ind_t* big_info)
{
    uint32_t sink_sync_ref_offset = big_info->report.pto*(big_info->report.nse/big_info->report.bn \
                                - big_info->report.irc)*big_info->report.iso_interval*1250;

    ble_audio_earphone_info_update_sink_sync_ref_offset(sink_sync_ref_offset);
    AOB_BIS_GROUP_INFO_T *aob_bis_group_info = ble_audio_earphone_info_get_bis_group_info();
    memcpy(&aob_bis_group_info->group_info, &big_info->report, sizeof(app_gaf_big_info_t));
}

static scan_event_handler_t scan_event_cb ={
    .scan_state_idle_cb          = aob_bis_scan_state_idle_cb,
    .scan_state_scanning_cb      = aob_bis_scan_state_scanning_cb,
    .scan_state_synchronizing_cb = aob_bis_scan_state_synchronizing_cb,
    .scan_state_synchronized_cb  = aob_bis_scan_state_synchronized_cb,
    .scan_state_streaming_cb     = aob_bis_scan_state_streaming_cb,
    .scan_pa_sync_req_cb         = aob_bis_scan_pa_sync_req_cb,
    .scan_pa_terminate_req_cb    = aob_bis_scan_pa_terminate_req_cb,
    .scan_pa_established_cb      = aob_bis_scan_pa_established_cb,
    .scan_pa_terminated_cb       = aob_bis_scan_pa_terminated_cb,
    .scan_stream_report_cb       = aob_bis_scan_stream_info_report_cb,
    .scan_big_info_report_cb     = aob_bis_scan_big_info_report_cb,
};

void aob_bis_scan_api_init(void)
{
    aob_mgr_gaf_scan_evt_handler_register(&scan_event_cb);
}

/*BAP Broadcast Deleg APIs*/
void aob_bis_deleg_start_solicite(uint16_t timeout_s, uint32_t context_bf)
{
    app_bap_bc_deleg_start_solicite(timeout_s, context_bf);
}

void aob_bis_deleg_stop_solicite(void)
{
    app_bap_bc_deleg_stop_solicite();
}

void aob_bis_deleg_pa_sync_ri(uint8_t pa_lid, bool accept)
{
    app_bap_bc_scan_dele_pa_sync_ri(pa_lid, accept);
}

void aob_bis_deleg_pa_terminate_ri(uint8_t pa_lid, bool accept)
{
    app_bap_bc_scan_dele_pa_terminate_ri(pa_lid, accept);
}

static void aob_bis_deleg_solicite_started_cb(void)
{
    LOG_D("aob_bap bc_deleg start solicite cmp");

    if ((NULL != p_bis_deleg_cb) && (NULL != p_bis_deleg_cb->ble_bis_deleg_scolite_start_cb)){
        p_bis_deleg_cb->ble_bis_deleg_scolite_start_cb();
    }
}

static void aob_bis_deleg_solicite_stoped_cb(void)
{
    LOG_D("aob_bap bc_deleg stop solicite cmp");

    if ((NULL != p_bis_deleg_cb) && (NULL != p_bis_deleg_cb->ble_bis_deleg_scolite_stoped_cb)){
        p_bis_deleg_cb->ble_bis_deleg_scolite_stoped_cb();
    }
}

static void aob_bis_deleg_source_add_ri_cb(uint8_t src_lid, uint8_t *bcast_id, uint8_t con_lid)
{
    LOG_D("aob_bap bc_deleg source add ri");
    ble_audio_earphone_info_set_bis_src_lid(src_lid);
    /// Store bcast id for later sync with PA according to EA's bcast id if no PAST
    ble_audio_earphone_info_set_bis_bcast_id(&bcast_id[0]);

    if ((NULL != p_bis_deleg_cb) && (NULL != p_bis_deleg_cb->ble_bis_deleg_source_add_ri_cb)){
        p_bis_deleg_cb->ble_bis_deleg_source_add_ri_cb(src_lid, con_lid);
    }
}

static void aob_bis_deleg_source_remove_ri_cb(uint8_t src_lid, uint8_t con_lid)
{
    LOG_D("aob_bap bc_deleg source remove ri");
    ble_audio_earphone_info_set_bis_src_lid(0);
    if ((NULL != p_bis_deleg_cb) && (NULL != p_bis_deleg_cb->ble_bis_deleg_source_rm_ri_cb)){
        p_bis_deleg_cb->ble_bis_deleg_source_rm_ri_cb(src_lid, con_lid);
    }
}

static deleg_event_handler_t deleg_event_cb = {
    .deleg_solicite_started_cb = aob_bis_deleg_solicite_started_cb,
    .deleg_solicite_stoped_cb  = aob_bis_deleg_solicite_stoped_cb,
    .deleg_remote_scan_started = NULL,
    .deleg_remote_scan_stoped  = NULL,
    .deleg_source_add_ri_cb    = aob_bis_deleg_source_add_ri_cb,
    .deleg_source_remove_ri_cb = aob_bis_deleg_source_remove_ri_cb,
    .deleg_source_update_ri_cb = NULL,
};

void aob_bis_deleg_api_init(void)
{
    p_bis_deleg_cb = ble_audio_get_evt_cb();
    aob_mgr_gaf_deleg_evt_handler_register(&deleg_event_cb);
}

#ifdef AOB_MOBILE_ENABLED
/*BAP Broadcast Assist APIs*/
void aob_bis_assist_scan_bc_src(uint8_t con_lid)
{
    if (false == app_bap_bc_assist_is_deleg_scan())
    {
        app_bap_bc_scan_start(APP_BAP_BC_ASSIST_SCAN);
    }
    app_bap_bc_set_assist_is_deleg_scan(con_lid, true);

}

void aob_bis_assist_start_scan(uint16_t timeout_s)
{
    app_bap_bc_assist_start_scan(timeout_s);
}

void aob_bis_assist_stop_scan(void)
{
    app_bap_bc_assist_stop_scan();
}

void aob_bis_assist_discovery(uint8_t con_lid)
{
    app_bap_bc_assist_start(con_lid);
}

static void aob_bis_assist_solicitation_cb(uint8_t addr_type, uint8_t *addr, uint16_t length, uint8_t *adv_data)
{
    LOG_D("aob_bap bc_assist_solicitation_ind addr_type = %d, len = %d", addr_type, length);
    DUMP8("%02x ", addr, GAP_BD_ADDR_LEN);
    DUMP8("%02x ", adv_data, length);
    BLE_ADV_DATA_T *particle;

    for (uint8_t offset = 0; offset < length;)
    {
        particle = (BLE_ADV_DATA_T *)(adv_data + offset);
        ble_bdaddr_t connect_addr;
        memcpy(connect_addr.addr, addr, APP_GAP_BD_ADDR_LEN);
        connect_addr.addr_type = addr_type;

        if ((GAP_AD_TYPE_COMPLETE_NAME == particle->type) && \
                ble_audio_dflt_check_device_is_master(addr))
        {
            LOG_I("aob_bap bc_assist start connecting!");
            app_bap_bc_assist_stop_scan();
            app_ble_start_connect(&connect_addr, APP_GAPM_STATIC_ADDR);
            break;
        }
        else
        {
            offset += (ADV_PARTICLE_HEADER_LEN + particle->length);
        }
    }
}

static void aob_bis_assist_source_state_cb(uint16_t cmd_code, uint16_t status, uint8_t con_lid, uint8_t src_lid)
{
    switch (cmd_code)
    {
        case (BAP_BC_ASSIST_START_SCAN):
        {
            LOG_D("aob_bap bc_assist start scan cmp");
        } break;

        case (BAP_BC_ASSIST_STOP_SCAN):
        {
            LOG_D("aob_bap bc_assist stop scan cmp");
        } break;

        case (BAP_BC_ASSIST_DISCOVER):
        {
            LOG_D("aob_bap bc_assist discovery cmp, con_lid = %d", con_lid);
            app_bap_bc_assist_set_cfg_cmd(con_lid, 0, 1);
        } break;

        case (BAP_BC_ASSIST_GET_STATE):
        {
            LOG_D("aob_bap bc_assist get state cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
        } break;

        case (BAP_BC_ASSIST_GET_CFG):
        {
            LOG_D("aob_bap bc_assist get cfg cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
            if (false == app_bap_bc_assist_is_deleg_scan())
            {
                app_bap_bc_scan_start(APP_BAP_BC_ASSIST_SCAN);
            }
            app_bap_bc_set_assist_is_deleg_scan(con_lid, true);
        } break;

        case (BAP_BC_ASSIST_SET_CFG):
        {
            LOG_D("aob_bap bc_assist set cfg cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
            app_bap_bc_assist_get_cfg_cmd(con_lid, src_lid);
        } break;

        case (BAP_BC_ASSIST_UPDATE_SCAN):
        {
            LOG_D("aob_bap bc_assist update scan cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
        } break;

        case (BAP_BC_ASSIST_ADD_SOURCE):
        {
            LOG_D("aob_bap bc_assist source add cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
            app_bap_bc_scan_stop();
        } break;

        case (BAP_BC_ASSIST_ADD_SOURCE_LOCAL):
        {
            LOG_D("aob_bap bc_assist source add local cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
        } break;

        case (BAP_BC_ASSIST_REMOVE_SOURCE):
        {
            LOG_D("aob_bap bc_assist source remove cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
        } break;

        case (BAP_BC_ASSIST_MODIFY_SOURCE):
        {
            LOG_D("aob_bap bc_assist source modify cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
        } break;

        case (BAP_BC_ASSIST_MODIFY_SOURCE_LOCAL):
        {
            LOG_D("aob_bap bc_assist source modify local cmp, con_lid = %d, src_lid = %d", con_lid, src_lid);
        } break;

        default:
            break;
    }
}

static void aob_bis_assist_bond_data_cb(uint8_t con_lid, uint8_t nb_rx_state, uint16_t shdl, uint16_t ehdl)
{
    LOG_D("aob_bap bass found con_lid = %d, nb_rx_state = %d, shdl = %04x, ehdl = %04x", con_lid, nb_rx_state, shdl, ehdl);
}

static void aob_bis_assist_bcast_cdoe_ri_cb(uint8_t con_lid, uint8_t src_lid)
{
    LOG_D("aob_bap bcast code ri con_lid = %d, src_lid = %d", con_lid, src_lid);
    uint8_t bcast_code[APP_GAP_KEY_LEN] = {0};

    if (isBisCodeConfiguredViaExternal)
    {
        memcpy(bcast_code, configuredBisCode, APP_GAP_KEY_LEN);
    }
    else
    {
        app_bap_add_data_set(bcast_code, APP_GAP_KEY_LEN);
    }

    app_bap_bc_assist_send_bcast_code(con_lid, src_lid, bcast_code);
}

static assist_event_handler_t assist_event_cb = {
    .assist_solicitation_cb    = aob_bis_assist_solicitation_cb,
    .assist_source_state_cb    = aob_bis_assist_source_state_cb,
    .assist_bond_data_cb       = aob_bis_assist_bond_data_cb,
    .assist_bcast_code_ri_cb   = aob_bis_assist_bcast_cdoe_ri_cb,
};

void aob_bis_assist_mobile_api_init(void)
{
    aob_mgr_gaf_mobile_assist_evt_handler_register(&assist_event_cb);
}
#endif
