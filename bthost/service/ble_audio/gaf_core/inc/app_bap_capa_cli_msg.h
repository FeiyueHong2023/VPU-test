/**
 ****************************************************************************************
 *
 * @file app_bap_capa_cli_msg.h
 *
 * @brief BLE Audio Published Audio Capabilities Client
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

#ifndef APP_BAP_CAPA_CLI_MSG_H_
#define APP_BAP_CAPA_CLI_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"
#include "co_list.h"

#define APP_BAP_DFT_CAPA_C_SINK_LOCATION_BF       (APP_GAF_LOC_SIDE_RIGHT_BIT | APP_GAF_LOC_SIDE_LEFT_BIT)
#define APP_BAP_DFT_CAPA_C_SRC_LOCATION_BF        (APP_GAF_LOC_SIDE_RIGHT_BIT | APP_GAF_LOC_SIDE_LEFT_BIT)
#define APP_BAP_DFT_CAPA_C_SINK_CONTEXT_BF        (APP_BAP_CONTEXT_TYPE_MEDIA_BIT | APP_BAP_CONTEXT_TYPE_RINGTONE_BIT | APP_BAP_CONTEXT_TYPE_CONVERSATIONAL_BIT | APP_BAP_CONTEXT_TYPE_GAME_BIT)
#define APP_BAP_DFT_CAPA_C_SRC_CONTEXT_BF         (APP_BAP_CONTEXT_TYPE_MEDIA_BIT | APP_BAP_CONTEXT_TYPE_RINGTONE_BIT | APP_BAP_CONTEXT_TYPE_CONVERSATIONAL_BIT | APP_BAP_CONTEXT_TYPE_GAME_BIT)
#define APP_BAP_DFT_CAPA_C_CODEC_ID               "\x06\x00\x00\x00\x00\x00"
#define APP_BAP_DFT_CAPA_C_SAMPLING_FREQ_BF       (APP_BAP_SAMPLING_FREQ_48000HZ_BIT | APP_BAP_SAMPLING_FREQ_24000HZ_BIT | APP_BAP_SAMPLING_FREQ_32000HZ_BIT | APP_BAP_SAMPLING_FREQ_16000HZ_BIT)
#ifdef BLE_AUDIO_FRAME_DUR_7_5MS
#define APP_BAP_DFT_CAPA_C_FRAME_DURATION_BF      (APP_BAP_FRAME_DUR_7_5MS_BIT | APP_BAP_FRAME_DUR_10MS_BIT|APP_BAP_FRAME_DUR_7_5MS_PREF_BIT)
#else
#define APP_BAP_DFT_CAPA_C_FRAME_DURATION_BF      (APP_BAP_FRAME_DUR_7_5MS_BIT | APP_BAP_FRAME_DUR_10MS_BIT)
#endif
#define APP_BAP_DFT_CAPA_C_CHAN_CNT_BF            2
#define APP_BAP_DFT_CAPA_C_FRAME_OCT_MIN          20
#define APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX          400
#define APP_BAP_DFT_CAPA_C_MAX_FRAMES_SDU         2

#ifdef NO_DEFINE_SOURCE_PAC_NON_AUDIO_CHAR
#define APP_BAP_DFT_CAPA_C_ADD_CAPA_LEN           0
#define APP_BAP_DFT_CAPA_C_METADATA_CAPA_LEN      13
#define APP_BAP_DFT_CAPA_C_PREFERRED_CONTEXT_BF   0
#else
#define APP_BAP_DFT_CAPA_C_ADD_CAPA_LEN           5
#define APP_BAP_DFT_CAPA_C_PREFERRED_CONTEXT_BF   APP_BAP_CONTEXT_TYPE_MEDIA_BIT
#define APP_BAP_DFT_CAPA_C_METADATA_CAPA_LEN      5
#endif /// NO_DEFINE_SOURCE_PAC_NON_AUDIO_CHAR

#ifdef LC3PLUS_SUPPORT
#define APP_BAP_DFT_CAPA_C_LC3PLUS_CODEC_ID               "\xFF\x08\xA9\x00\x02"
#define APP_BAP_DFT_CAPA_C_LC3PLUS_SAMPLING_FREQ_BF       (APP_BAP_SAMPLING_FREQ_48000HZ_BIT | APP_BAP_SAMPLING_FREQ_96000HZ_BIT)
#define APP_BAP_DFT_CAPA_C_LC3PLUS_FRAME_DURATION_BF      (APP_BAP_FRAME_DUR_10MS_BIT | APP_BAP_FRAME_DUR_5MS_BIT | APP_BAP_FRAME_DUR_2_5MS_BIT)
#define APP_BAP_DFT_CAPA_C_LC3PLUS_CHAN_CNT_BF            2
#define APP_BAP_DFT_CAPA_C_LC3PLUS_FRAME_OCT_MIN          160
#define APP_BAP_DFT_CAPA_C_LC3PLUS_FRAME_OCT_MAX          190
#define APP_BAP_DFT_CAPA_C_LC3PLUS_MAX_FRAMES_SDU         1
#endif

#ifdef NO_DEFINE_SOURCE_PAC_NON_AUDIO_CHAR
#define APP_BAP_DET_CAPA_C_NON_AUDIO_CODEC_ID             "\xFF\x02\x00\x00\x00"
#define APP_BAP_DFT_CAPA_C_NON_AUDIO_SAMPLING_FREQ_BF       (0x00)
#define APP_BAP_DFT_CAPA_C_NON_AUDIO_FRAME_DURATION_BF      (APP_BAP_FRAME_DUR_10MS_BIT)
#define APP_BAP_DFT_CAPA_C_NON_AUDIO_CHAN_CNT_BF            (1)
#define APP_BAP_DFT_CAPA_C_NON_AUDIO_FRAME_OCT_MIN          (14)
#define APP_BAP_DFT_CAPA_C_NON_AUDIO_FRAME_OCT_MAX          (14)
#define APP_BAP_DFT_CAPA_C_NON_AUDIO_MAX_FRAMES_SDU         (1)
#endif
#ifdef HID_ULL_ENABLE
#define APP_BAP_DFT_CAPA_C_ULL_CODEC_ID                "\x08\x00\x00\x00\x00\x00"
#define APP_BAP_DFT_CAPA_C_ULL_SAMPLING_FREQ_BF       (APP_BAP_SAMPLING_FREQ_48000HZ_BIT | APP_BAP_SAMPLING_FREQ_96000HZ_BIT)
#define APP_BAP_DFT_CAPA_C_ULL_FRAME_DURATION_BF      (APP_BAP_SAMPLING_FREQ_48000HZ_BIT | APP_BAP_SAMPLING_FREQ_24000HZ_BIT | APP_BAP_SAMPLING_FREQ_32000HZ_BIT | APP_BAP_SAMPLING_FREQ_16000HZ_BIT)
#define APP_BAP_DFT_CAPA_C_ULL_CHAN_CNT_BF            2
#define APP_BAP_DFT_CAPA_C_ULL_FRAME_OCT_MIN          160
#define APP_BAP_DFT_CAPA_C_ULL_FRAME_OCT_MAX          190
#define APP_BAP_DFT_CAPA_C_ULL_MAX_FRAMES_SDU         1
#endif

/// PAC Record information structure
typedef struct app_bap_capa_cli_supp
{
    /// Supported Audio Locations for the direction
    uint32_t location_bf;
    /// Supported Audio Contexts for the direction
    uint16_t context_bf_supp;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Pointer to Codec Capabilities structure
    app_bap_capa_t* p_capa;
    /// Pointer to Codec Capabilities Metadata structure
    app_bap_capa_metadata_t* p_metadata;
} app_bap_capa_cli_supp_t;

/// Published Audio Capabilities Client environment structure
typedef struct app_bap_capa_cli_env
{
    /// Preferred Mtu
    uint8_t preferred_mtu;
    /// Pointer to Direction information structure
    app_bap_capa_cli_supp_t capa_info[APP_GAF_DIRECTION_MAX];
}app_bap_capa_cli_env_t;

/// PAC Record information structure
typedef struct app_bap_capa_record_cfg
{
    /// List header
    co_list_hdr_t hdr;
    /// Configuration identifier
    uint8_t cfg_id;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Codec Capabilities Length
    uint8_t cfg_len;
    /// Pointer to Codec Capabilities structure
    app_bap_cfg_t* p_cfg;
    /// Metadata Length
    uint8_t metadata_len;
    /// Pointer to Codec Capabilities Metadata structure
    app_bap_cfg_metadata_t* p_metadata;
}app_bap_capa_record_cfg_t;
/// PAC information structure
typedef struct app_bap_capa_pac_cfg
{
    /// List of records (@see app_bap_capa_record_cfg_t for elements)
    co_list_t list_cfg;
    /// Number of cfg
    uint8_t nb_cfg;
} app_bap_capa_pac_cfg_t;

/// PACC Direction Configuration structure
typedef struct app_bap_capa_dir_cfg
{
    /// Audio Locations for the direction
    uint32_t location_bf;
    /// Audio Contexts for the direction
    uint16_t context_bf;
    /// Number of PACs for the direction
    uint8_t nb_pacs;
} app_bap_capa_dir_cfg_t;

typedef struct app_bap_capa_cfg
{
    /// Connection local index
    uint8_t con_lid;
    /// Total number of PAC Groups
    uint8_t nb_pacs;
    /// Total number of PAC Groups
    uint8_t nb_cfg;
    /// Pointer to Direction information structure
    app_bap_capa_dir_cfg_t dir_cfg[APP_GAF_DIRECTION_MAX];
    /// PAC informations (nb_pacs elements)
    app_bap_capa_pac_cfg_t* p_pac_cfg;
}app_bap_capa_cfg_t;
/// Published Audio Capabilities Client Configuration structure
typedef struct app_bap_capa_cfg_env
{
    app_bap_capa_cfg_t capa_cfg[BLE_AUDIO_CONNECTION_CNT];
}app_bap_capa_cfg_env_t;

#ifdef AOB_MOBILE_ENABLED
uint32_t app_bap_capa_cli_cmp_evt_handler(void const *param);
uint32_t app_bap_capa_cli_ind_handler(void const *param);

void app_bap_capa_cli_init(void);
void app_bap_capa_cli_info_init(void);
void app_bap_capa_cli_start(uint8_t con_idx);
uint8_t app_bap_capa_cli_get_con_idx(uint8_t con_lid);
app_bap_capa_record_cfg_t *app_bap_capa_cli_get_pac_record(uint8_t con_lid, uint8_t direction, const void* codec_id, 
                                                            uint8_t sampleRate);
uint32_t app_bap_capa_get_peer_audio_location_bf(uint8_t con_lid, uint8_t direction);
bool app_bap_capa_cli_is_peer_support_stereo_channel(uint8_t con_lid, uint8_t direction);
bool app_bap_capa_cli_is_codec_capa_support_stereo_channel(uint8_t con_lid, uint8_t direction, const void* codec_id, 
                                                            uint8_t sampleRate);
void app_bap_capa_cli_clean_up_resource(uint8_t con_idx);

#endif

#endif
#endif // APP_BAP_CAPA_CLI_MSG_H_

/// @} APP_BAP
