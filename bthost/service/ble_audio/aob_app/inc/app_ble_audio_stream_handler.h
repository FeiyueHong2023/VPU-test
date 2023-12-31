/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef __APP_BLE_AUDIO_STREAMING_HANDLER_H__
#define __APP_BLE_AUDIO_STREAMING_HANDLER_H__
#include "ble_aob_common.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_ble_audio_sink_streaming_init();

void app_ble_audio_policy_config();

BLE_AUDIO_POLICY_CONFIG_T* app_ble_audio_get_policy_config();

void app_ble_audio_sink_streaming_handle_event(uint8_t con_lid, uint8_t data, app_gaf_direction_t direction, app_ble_audio_event_t event);

#ifdef __cplusplus
    }
#endif


#endif /* __APP_BLE_AUDIO_STREAMING_HANDLER_H__ */

