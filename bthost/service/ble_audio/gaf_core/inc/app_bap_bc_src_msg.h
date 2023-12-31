/**
 ****************************************************************************************
 *
 * @file app_bap_bc_src_msg.h
 *
 * @brief BLE Audio Broadcast Source
 *
 * Copyright 2015-2019 BES.
 *
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
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_BAP_BC_SRC_MSG_H_
#define APP_BAP_BC_SRC_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"
#include "bap_bc.h"

//Broadcast source default value
#define APP_BAP_DFT_BC_SRC_MAX_BIG_NUM            1
#define APP_BAP_DFT_BC_SRC_MAX_SUBGRP_NUM         2
#define APP_BAP_DFT_BC_SRC_MAX_STREAM_NUM         IAP_NB_STREAMS
#define APP_BAP_DFT_BC_SRC_NB_STREAMS             1
#define APP_BAP_DFT_BC_SRC_NB_SUBGRPS             1
#define APP_BAP_DFT_BC_SRC_SDU_INTV_US            10000
#define APP_BAP_DFT_BC_SRC_MAX_SDU_SIZE           240
#define APP_BAP_DFT_BC_SRC_MAX_TRANS_LATENCY_MS   40
#define APP_BAP_DFT_BC_SRC_PACKING_TYPE           APP_ISO_PACKING_SEQUENTIAL
#define APP_BAP_DFT_BC_SRC_FRAMING_TYPE           APP_ISO_UNFRAMED_MODE
#define APP_BAP_DFT_BC_SRC_PHY_BF                 APP_PHY_2MBPS_BIT
#define APP_BAP_DFT_BC_SRC_RTN                    2
#define APP_BAP_DFT_BC_SRC_ADV_INTERVAL           256 //100ms  *0.625ms
#define APP_BAP_DFT_BC_SRC_PERIODIC_INTERVAL      80  //100ms  *1.25ms

#define APP_BAP_DFT_BC_SRC_PRES_DELAY_US          10000
#define APP_BAP_DFT_BC_SRC_CONTEXT_BF             APP_BAP_CONTEXT_TYPE_MEDIA_BIT
#define APP_BAP_DFT_BC_SRC_METADATA_LEN           5
#define APP_BAP_DFT_BC_SRC_CODEC_ID               APP_GAF_CODEC_TYPE_LC3
#define APP_BAP_DFT_BC_SRC_ADD_CODEC_LEN          5
#define APP_BAP_DFT_BC_SRC_LOCATION_BF            (APP_GAF_LOC_SIDE_LEFT_BIT | APP_GAF_LOC_SIDE_RIGHT_BIT)
#define APP_BAP_DFT_BC_SRC_FRAME_OCTET            120
#define APP_BAP_DFT_BC_SRC_SAMPLING_FREQ          APP_BAP_SAMPLING_FREQ_48000HZ
#define APP_BAP_DFT_BC_SRC_FRAME_DURATION         APP_BAP_FRAME_DUR_10MS
#define APP_BAP_DFT_BC_SRC_NB_LC3_STREAM          0
#define APP_BAP_DFT_BC_SRC_IS_ENCRYPTED           1
#define APP_BAP_DFT_BC_SRC_DP_ID                  0
#define APP_BAP_DFT_BC_SRC_CTL_DELAY_US           0x0102

#define APP_BAP_DFT_BC_SRC_ADV_DATA               "bis_adv_test"
#define APP_BAP_DFT_BC_SRC_PA_ADV_DATA            "bis_pa_adv_test"
#define APP_BAP_DFT_BC_SRC_MAX_ADV_DATA_LEN       252


/// Broadcast source group state
enum app_bap_bc_src_state
{
    /// Broadcast group in idle state
    APP_BAP_BC_SRC_STATE_IDLE,
    /// Broadcast group in configured state
    /// - Periodic ADV started
    APP_BAP_BC_SRC_STATE_CONFIGURED,
    /// Broadcast group in stream state
    /// - broadcaster started
    APP_BAP_BC_SRC_STATE_STREAMING,
};

/// Stream information
typedef struct app_bap_bc_src_stream
{
    /// Stream local identifier
    uint8_t                  stream_lid;
    /// Subgroup local identifier
    uint8_t                  sgrp_lid;
    /// IAP configuration
    app_bap_dp_cfg_t          dp_cfg;
    /// Pointer to level 3 Codec configuration value
    uint8_t cfg_len;
    /// Pointer to Codec Configuration structure
    app_bap_cfg_t* p_cfg;
    /// BIS Handle
    uint16_t        bis_hdl;
} app_bap_bc_src_stream_t;

/// Subgroup information structure
typedef struct app_bap_bc_src_sgrp
{
    /// Subgroup local identifier
    uint8_t                  sgrp_lid;
    /// Stream local index bit field indicating Streams belonging to the Subgroup
    uint32_t                 stream_lid_bf;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Length of complete Codec Configuration
    uint8_t cfg_len;
    /// Pointer to Codec Configuration structure
    app_bap_cfg_t* p_cfg;
    /// Length of complete Metadata
    uint8_t metadata_len;
    /// Pointer to Metadata structure
    app_bap_cfg_metadata_t* p_metadata;
} app_bap_bc_src_sgrp_t;
/// Broadcast Source BIG Information Structure
typedef struct app_bap_bc_src_grp
{
    /// Group local index
    uint8_t        grp_lid;
    /// BIG State
    uint8_t big_state;
    /// Broadcast ID
    bap_bcast_id_t         bcast_id;
    /// Number of Streams in the Broadcast Group. Cannot be 0
    uint8_t                nb_streams;
    /// Number of Subgroups in the Broadcast Group. Cannot be 0
    uint8_t                nb_subgroups;
    /// Broadcast Group parameters
    app_bap_bc_grp_param_t     grp_param;
    /// Advertising parameters
    app_bap_bc_adv_param_t     adv_param;
    /// Periodic Advertising parameters
    app_bap_bc_per_adv_param_t per_adv_param;
    // Audio output presentation delay in microseconds
    uint32_t pres_delay_us;
    // Indicate if streams are encrypted (!= 0) or not
    uint8_t encrypted;
    // Broadcast code. Meaningful only if encrypted parameter indicates that streams are encrypted
    app_gaf_bc_code_t bcast_code;
    /// Length of additional advertising data
    uint8_t        adv_data_len;
    /// Additional advertising data
    uint8_t        adv_data[APP_EXT_ADV_DATA_MAX_LEN];
    /// Length of additional periodic advertising data
    uint8_t        per_adv_data_len;
    /// Additional periodic advertising data
    uint8_t        per_adv_data[APP_PER_ADV_DATA_MAX_LEN];
    /// Pointer to array of Subgroup information structure
    app_bap_bc_src_sgrp_t* p_sgrp_infos;
    /// Array of Stream information structures
    app_bap_bc_src_stream_t *stream_info;
}app_bap_bc_src_grp_t;

/// Content of Broadcast source environment
typedef struct app_bap_bc_src_env
{
    /// BIG Information
    app_bap_bc_src_grp_t p_grp[APP_BAP_DFT_BC_SRC_MAX_BIG_NUM];
}app_bap_bc_src_env_t;


typedef struct app_bap_bc_src_cfg_info
{
    /// Broadcast ID
    bap_bcast_id_t         bcast_id;
    /// Number of Streams in the Broadcast Group. Cannot be 0
    uint8_t                nb_streams;
    /// Number of Subgroups in the Broadcast Group. Cannot be 0
    uint8_t                nb_subgroups;
    /// SDU interval in microseconds
    uint32_t sdu_intv_us;
    /// Maximum size of an SDU
    uint16_t max_sdu;
    /// Maximum time (in milliseconds) between the first transmission of an SDU to the end of the last transmission
    /// of the same SDU
    uint16_t max_tlatency;
    /// Minimum advertising interval in multiple of 0.625ms. Must be higher than 20ms.
    uint32_t adv_intv_min_slot;
    /// Maximum advertising interval in multiple of 0.625ms. Must be higher than 20ms.
    uint32_t adv_intv_max_slot;
    /// Minimum periodic advertising interval in multiple of 1.25ms. Must be higher than 7.5ms
    uint32_t adv_intv_min_frame;
    /// Maximum periodic advertising interval in multiple of 1.25ms. Must be higher than 7.5ms
    uint32_t adv_intv_max_frame;
    // Indicate if streams are encrypted (!= 0) or not
    uint8_t encrypted;
    // Broadcast code. Meaningful only if encrypted parameter indicates that streams are encrypted
    app_gaf_bc_code_t bcast_code;
}app_bap_bc_src_cfg_info_t;

typedef bool (*app_bap_src_cfg_info_cb_func)(app_bap_bc_src_cfg_info_t *info);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AOB_MOBILE_ENABLED
void app_bap_bc_src_enable_pa(app_bap_bc_src_grp_t *p_grp);
void app_bap_bc_src_enable(app_bap_bc_src_grp_t *p_grp);
void app_bap_bc_src_disable_pa(uint8_t grp_lid);
void app_bap_bc_src_disable(uint8_t grp_lid);

uint8_t app_bap_bc_src_find_big_idx(uint8_t grp_lid);
uint8_t app_bap_bc_src_find_stream_lid(uint8_t bis_hdl);
app_bap_bc_src_grp_t *app_bap_bc_src_get_big_info_by_big_idx(uint8_t big_idx);
app_bap_bc_src_grp_t *app_bap_bc_src_get_big_info_by_bis_hdl(uint16_t bis_hdl);

uint32_t app_bap_bc_src_cmp_evt_handler(void const *param);
uint32_t app_bap_bc_src_rsp_handler(void const *param);
uint8_t app_bap_bc_src_iso_send_data_to_all_channel(uint8_t **payload, uint16_t payload_len, uint32_t ref_time);

void app_bap_bc_src_register_cfg_info_cb(app_bap_src_cfg_info_cb_func cb);
void app_bap_bc_src_init(void);
void app_bap_bc_src_info_init(void);
#endif
#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_BC_SRC_MSG_H_

/// @} APP_BAP
