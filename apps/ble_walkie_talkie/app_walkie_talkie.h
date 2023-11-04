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

#ifndef __APP_WALKIE_TALKIE__H__
#define __APP_WALKIE_TALKIE__H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*wt_ready_to_send_data)();
    void (*wt_find_device)(uint8_t device_id, uint8_t* device_addrss);
    void (*wt_device_loss)(uint8_t device_id,uint8_t* device_addrss);
    void (*wt_device_is_stalking)(uint8_t device_id,uint8_t* device_addrss);
    void (*wt_device_end_stalking)(uint8_t device_id,uint8_t* device_addrss);
}app_walkie_talkie_full_dup_event_cb;

void app_walkie_talkie_init();

void app_walkie_talkie_deinit();

void app_walkie_talkie_handle_event(APP_W_T_KEY_EVENT_E event);

void app_walkie_talkie_reg_full_dup_state_changed_callback(const app_walkie_talkie_full_dup_event_cb *cb);

void app_walkie_talkie_dereg_full_dup_state_changed_callback(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_WALKIE_TALKIE__H__ */
