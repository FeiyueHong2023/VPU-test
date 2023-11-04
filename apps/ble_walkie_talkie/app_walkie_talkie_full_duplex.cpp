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
#include "hsm.h"
#include <string.h>
#include "walkie_talkie_dbg.h"
#include "app_walkie_talkie_key_handler.h"
#include "bluetooth_ble_api.h"
#include "walkie_talkie.h"
#include "app_walkie_talkie_full_duplex.h"

#ifdef BLE_WALKIE_TALKIE_FULL_DUPLEX

#define SEARCH_DEVICE_TIME_OUT   (3500)

typedef enum
{
    WT_SM_BTN_DOWN,
    WT_SM_BTN_UP,
    WT_SM_PA_SYNC_ESTB,
    WT_SM_PA_SYNC_LOSS,
    WT_SM_GAP_INIT_DONE,
    WT_LOOK_FOR_DEVS_TO,
    WT_RECV_AUDIO_START,
    WT_RECV_AUDIO_STOP,
}WALKIE_TALKIE_SM_EVT_T;

typedef struct
{
    Hsm walkie_talkie_dup_sm;
    State look_for_network;
    State full_dup_work;
    bool init_flag;
    osTimerId           look_for_devices_timer_id;
    osTimerDefEx_t      look_for_devices_timer_def;

    const walkie_full_dup_event_cb* state_change_cb;

} walkie_talkie_full_dup_sm_t;

static walkie_talkie_full_dup_sm_t walkie_talkie_dup_sm;

Msg const* app_walkie_talkie_full_dup_super_state(walkie_talkie_full_dup_sm_t *me, Msg *msg)
{
    LOG_I("W-T-FD-SM::Top state on event %d",(msg->evt));
    switch (msg->evt)
    {
        case START_EVT:
            return 0;
        case ENTRY_EVT:
            return 0;
         case WT_SM_GAP_INIT_DONE:
            STATE_START(me, &me->look_for_network);
            return 0;
        case EXIT_EVT:
            return 0;
    }

    return msg;
}

Msg const* app_walkie_talkie_look_for_network_state(walkie_talkie_full_dup_sm_t *me, Msg *msg)
{
    LOG_I("W-T-FD-SM::Look for network state on event %d",(msg->evt));
    switch (msg->evt)
    {
        case START_EVT:
            return 0;
        case ENTRY_EVT:
            /* First look for the surrounding devices.*/
            bes_ble_walkie_scan_start(BES_IF_WALKIE_SCAN_PASSIVE);
            osTimerStart(me->look_for_devices_timer_id,SEARCH_DEVICE_TIME_OUT);
            return 0;
        case WT_LOOK_FOR_DEVS_TO:
            STATE_TRAN(me, &me->full_dup_work);
            return 0;
        case WT_SM_PA_SYNC_ESTB:
            walkie_talkie_dup_sm.state_change_cb->wt_find_device(msg->param0,(uint8_t*)msg->param1);
            osTimerStop(me->look_for_devices_timer_id);
            osTimerStart(me->look_for_devices_timer_id,SEARCH_DEVICE_TIME_OUT);
            return 0;
        case EXIT_EVT:
            return 0;
    }

    return msg;
}

Msg const* app_walkie_talkie_full_dup_state(walkie_talkie_full_dup_sm_t *me, Msg *msg)
{
    LOG_I("W-T-FD-SM:full duplex state on event %d",msg->evt);

    switch (msg->evt)
    {
        case START_EVT:
            return 0;
        case ENTRY_EVT:
            bes_ble_walkie_adv_start(NULL, 0);
            walkie_talkie_dup_sm.state_change_cb->wt_ready_to_send_data();
            return 0;
        case WT_TALK_BTN_DOWN:
            if (walkie_talkie_get_neighbor_count() > 0)
            {
                walkie_talkie_start_capture();
            }
            return 0;
        case WT_TALK_BTN_UP:
            walkie_talkie_stop_capture();
            return 0;
         case WT_SM_PA_SYNC_ESTB:
            walkie_talkie_dup_sm.state_change_cb->wt_find_device(msg->param0,(uint8_t*)msg->param1);
            return 0;
        case WT_SM_PA_SYNC_LOSS:
            //STATE_TRAN(me, &me->look_for_network);
            walkie_talkie_dup_sm.state_change_cb->wt_device_loss(msg->param0,(uint8_t*)msg->param1);
            return 0;
        case WT_RECV_AUDIO_START:
            walkie_talkie_dup_sm.state_change_cb->wt_device_is_stalking(msg->param0,(uint8_t*)msg->param1);
            return 0;
        case WT_RECV_AUDIO_STOP:
            walkie_talkie_dup_sm.state_change_cb->wt_device_end_stalking(msg->param0,(uint8_t*)msg->param1);
            return 0;
        case EXIT_EVT:
            return 0;
    }

    return msg;
}

static void app_walkie_talkie_full_dup_sm_handle_event(
                WALKIE_TALKIE_SM_EVT_T event,uint8_t device_id = 0,uint8_t *addr = NULL)
{
    Msg message;

    if(!walkie_talkie_dup_sm.init_flag)
    {
        return;
    }
    message.evt = event;
    message.param0 = device_id;
    message.param1 = (uint32_t)addr;
    message.param2 = 0;
    HsmOnEvent((Hsm *)&walkie_talkie_dup_sm, &message);
}

void app_walkie_talkie_full_dup_handle_key_event(APP_W_T_KEY_EVENT_E event)
{
    if (WT_TALK_BTN_DOWN == event)
    {
        app_walkie_talkie_full_dup_sm_handle_event(WT_SM_BTN_DOWN);
    }
    else if (WT_TALK_BTN_UP == event)
    {
        app_walkie_talkie_full_dup_sm_handle_event(WT_SM_BTN_UP);
    }
}

static void app_walkie_talkie_full_dup_event_callback(
                    WALKIE_TAKIE_SRV_EVENT_T event,uint8_t device_id,uint8_t *addr)
{
    switch (event)
    {
        case AUDIO_STREAMING_START:
            app_walkie_talkie_full_dup_sm_handle_event(WT_RECV_AUDIO_START,device_id,addr);
            break;
        case AUDIO_STREAMING_STOP:
            app_walkie_talkie_full_dup_sm_handle_event(WT_RECV_AUDIO_STOP,device_id,addr);
            break;
        case PA_SYNC_ESTB:
            app_walkie_talkie_full_dup_sm_handle_event(WT_SM_PA_SYNC_ESTB,device_id,addr);
            break;
        case PA_SYNC_LOSS:
            app_walkie_talkie_full_dup_sm_handle_event(WT_SM_PA_SYNC_LOSS,device_id,addr);
            break;
        case BLE_GAP_INIT_DONE:
            app_walkie_talkie_full_dup_sm_handle_event(WT_SM_GAP_INIT_DONE);
            break;
    }
}

POSSIBLY_UNUSED static void app_walkie_talkie_create_full_dup_sm(walkie_talkie_full_dup_sm_t *me)
{
    HsmCtor((Hsm *)me, "super", (EvtHndlr)app_walkie_talkie_full_dup_super_state);
      AddState(&me->look_for_network, "look_for_network", &((Hsm *)me)->top,(EvtHndlr)app_walkie_talkie_look_for_network_state);
      AddState(&me->full_dup_work, "full_dup", &((Hsm *)me)->top, (EvtHndlr)app_walkie_talkie_full_dup_state);
}

void app_walkie_talkie_search_devices_timeout_cb(void const *ctx)
{
    LOG_I("W-T-FD-SM:Look for device timeout");
    app_walkie_talkie_full_dup_sm_handle_event(WT_LOOK_FOR_DEVS_TO);
}

void app_walkie_talkie_full_dup_init(const ble_if_walkie_bdaddr* filter_addr_list,uint8_t list_size,uint8_t work_mode)
{
    wakie_talke_register_event_call_back(app_walkie_talkie_full_dup_event_callback);

    osTimerInit(walkie_talkie_dup_sm.look_for_devices_timer_def,app_walkie_talkie_search_devices_timeout_cb);
    walkie_talkie_dup_sm.look_for_devices_timer_id = \
        osTimerCreate(&walkie_talkie_dup_sm.look_for_devices_timer_def.os_timer_def,osTimerOnce, &walkie_talkie_dup_sm);

    app_walkie_talkie_create_full_dup_sm(&walkie_talkie_dup_sm);
    HsmOnStart((Hsm *)&walkie_talkie_dup_sm);
    walkie_talkie_dup_sm.init_flag = true;
    walkie_talkie_srv_init(filter_addr_list,list_size,work_mode);
}

void app_wt_full_dup_reg_state_changed_callback(const walkie_full_dup_event_cb* cbs)
{
    walkie_talkie_dup_sm.state_change_cb = cbs;
}

void app_walkie_talkie_full_dup_deinit(void)
{
    osTimerStop(walkie_talkie_dup_sm.look_for_devices_timer_id);
    osTimerDelete(walkie_talkie_dup_sm.look_for_devices_timer_id);
    walkie_talkie_srv_deinit();
    memset((uint8_t*)&walkie_talkie_dup_sm, 0, sizeof(walkie_talkie_full_dup_sm_t));
}

#endif /* BLE_WALKIE_TALKIE_HALF_DUPLEX */
