/**
 *
 * @copyright Copyright (c) 2015-2022 BES Technic.
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

#include "cmsis_os.h"
#include "hal_trace.h"
#include "factory_section.h"

#include "walkie_talkie_dbg.h"
#include "app_walkie_talkie_key_handler.h"
#include "app_walkie_talkie.h"
#include "bluetooth_ble_api.h"
#include "walkie_talkie.h"
#include "app_walkie_talkie_half_duplex.h"
#include "app_walkie_talkie_full_duplex.h"
#include "walkie_talkie_network_addr.h"

#ifdef WT_GAP_UNIT_TEST
#include "walkie_talkie_unit_test.h"
#endif

/*
 *  MVP framework,App_walkie_talkie as the Presenter
 *                Watch UI as the View layer
 *                walkie talkie service as the Mode
*/
static const app_walkie_talkie_full_dup_event_cb* app_full_dup_cb = NULL;

static void app_walkie_talkie_ready_to_send_data_cb()
{
    LOG_D("W-T:ready_to_send_data_cb");
    if(app_full_dup_cb && app_full_dup_cb->wt_ready_to_send_data)
    {
        app_full_dup_cb->wt_ready_to_send_data();
    }
}

static void app_walkie_talkie_find_device_cb(uint8_t device_id, uint8_t* device_addrss)
{
    LOG_D("W-T:find_device(%d),address:",device_id);
    DUMP8("%02x ",device_addrss, 6);
    if(app_full_dup_cb && app_full_dup_cb->wt_find_device)
    {
        app_full_dup_cb->wt_find_device(device_id, device_addrss);
    }
}

static void app_walkie_talkie_device_loss_cb(uint8_t device_id,uint8_t* device_addrss)
{
    LOG_D("W-T:device(%d),address loss",device_id);
    DUMP8("%02x ",device_addrss, 6);
    if(app_full_dup_cb && app_full_dup_cb->wt_device_loss)
    {
        app_full_dup_cb->wt_device_loss(device_id, device_addrss);
    }
}

static void app_walkie_talkie_device_is_stalking(uint8_t device_id,uint8_t* device_addrss)
{
    LOG_D("W-T:device(%d) is talking",device_id);
    DUMP8("%02x ",device_addrss, 6);
    if(app_full_dup_cb && app_full_dup_cb->wt_device_is_stalking)
    {
        app_full_dup_cb->wt_device_is_stalking(device_id, device_addrss);
    }
}

static void app_wakie_talkie_device_end_stalking(uint8_t device_id,uint8_t* device_addrss)
{
    LOG_D("W-T:device(%d) talking end",device_id);
    DUMP8("%02x ",device_addrss, 6);
    if(app_full_dup_cb && app_full_dup_cb->wt_device_end_stalking)
    {
        app_full_dup_cb->wt_device_end_stalking(device_id, device_addrss);
    }
}

static const walkie_full_dup_event_cb walkie_talkie_full_dup_event_cbs = {
    .wt_ready_to_send_data = app_walkie_talkie_ready_to_send_data_cb,
    .wt_find_device = app_walkie_talkie_find_device_cb,
    .wt_device_loss = app_walkie_talkie_device_loss_cb,
    .wt_device_is_stalking = app_walkie_talkie_device_is_stalking,
    .wt_device_end_stalking = app_wakie_talkie_device_end_stalking,
};

void app_walkie_talkie_reg_full_dup_state_changed_callback(const app_walkie_talkie_full_dup_event_cb *cb)
{
    app_full_dup_cb = cb;
}

void app_walkie_talkie_dereg_full_dup_state_changed_callback(void)
{
    app_full_dup_cb = NULL;
}

void app_walkie_talkie_init()
{
    uint8_t* local_ble_addr = factory_section_get_ble_address();
    LOG_D("W-T:Local Address:");
    ASSERT(local_ble_addr,"Local Address Err!");
    DUMP8("%02x ",local_ble_addr, 6);

#ifndef WT_GAP_UNIT_TEST
#ifdef BLE_WALKIE_TALKIE_HALF_DUPLEX
    app_walkie_talkie_half_dup_init(w_t_white_list_addr,ARRAY_SIZE(w_t_white_list_addr),WALKIE_TALKIE_HALF_DUPLEX_MODE);
#else
#ifdef BLE_WALKIE_TALKIE_FULL_DUPLEX
    app_walkie_talkie_full_dup_init(w_t_white_list_addr,ARRAY_SIZE(w_t_white_list_addr),WALKIE_TALKIE_FULL_DUPLEX_MODE);
    app_wt_full_dup_reg_state_changed_callback(&walkie_talkie_full_dup_event_cbs);
#endif
#endif
#endif
}

void app_walkie_talkie_deinit()
{

#ifndef WT_GAP_UNIT_TEST
#ifdef BLE_WALKIE_TALKIE_HALF_DUPLEX
    app_walkie_talkie_half_dup_deinit();
#else
    app_walkie_talkie_full_dup_deinit();
#endif

#endif
}

void app_walkie_talkie_handle_event(APP_W_T_KEY_EVENT_E event)
{
#ifdef BLE_WALKIE_TALKIE_HALF_DUPLEX
    app_walkie_talkie_half_dup_handle_key_event(event);
#else
#ifdef BLE_WALKIE_TALKIE_FULL_DUPLEX
    app_walkie_talkie_full_dup_handle_key_event(event);
#endif
#endif
}

