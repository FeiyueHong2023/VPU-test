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
#ifndef __APP_VENDOR_EVT__
#define __APP_VENDOR_EVT__

#include "app_tws_ibrt.h"

#define GET_HCI_HANDLE(ptr)    ((unsigned short)(((unsigned short) *((ptr)+1) << 8) | ((unsigned short) *(ptr))))

typedef enum {
    LL_CON_UPD_IND  = 0x00,
    LL_CHNL_MAP_IND = 0x01,
} LL_CONTROL_OPCODE_E;

typedef enum
{
    LE_TRC_LLCP_TX  = 0x14,
    LE_TRC_LLCP_RX  = 0x15,
    LE_TRC_LLCP_ACK = 0x16,
    LE_TRC_LMP      = 0x17,
    LE_TRC_VS_INFO   = 0x3C,
} APP_DBG_TRC_OPCODE_E;

typedef enum {
    TRC_VS_LE_AGC = 0x0000,
} TRC_LE_VS_OPCODE_E;

typedef struct app_dbg_trc_head {
    uint32_t time_stamp;
    uint16_t seq_number;   //sequence number
    uint8_t  opcode;
} __attribute__ ((__packed__)) app_dbg_trc_head_t;

typedef struct app_dbg_vs_opcode_head {
    uint16_t vs_opcode;
    uint8_t  link_id;
} __attribute__ ((__packed__)) app_dbg_vs_opcode_head_t;

typedef struct
{
    uint16_t    conn_handle;
    uint8_t     status;
    bt_bdaddr_t mobile_addr;
    uint8_t     role;
} snoop_connect_evt_t;

typedef struct
{
    uint16_t    conn_handle;
    uint8_t     reason;
    bt_bdaddr_t mobile_addr;
    uint8_t     role;
} snoop_disconnect_evt_t;

typedef struct
{
    uint8_t    role;
    uint8_t     status;
    bt_bdaddr_t mobile_addr;
} set_env_evt_t;

typedef struct
{
    uint16_t    conn_handle;
    uint8_t    role;
    bt_bdaddr_t mobile_addr;
} tws_switch_evt_t;

struct sco_switch_background_evt_t
{
    uint8_t status;
    uint16_t fg_handle;
    uint16_t bg_handle;
}__attribute__ ((packed));;
//Sub code of Vendor event
#define HCI_DBG_TRACE_WARNING_EVT_CODE      0x01
#define HCI_SCO_SNIFFER_STATUS_EVT_CODE     0x02
#define HCI_ACL_SNIFFER_STATUS_EVT_CODE     0x03
#define HCI_TWS_EXCHANGE_CMP_EVT_CODE       0x04
#define HCI_NOTIFY_CURRENT_ADDR_EVT_CODE    0x05
#define HCI_START_SWITCH_EVT_CODE           0x09
#define HCI_LL_MONITOR_EVT_CODE             0x0A
#define HCI_DBG_LMP_RESERVED_EVT_SUBCODE    0x0B
#define HCI_GET_TWS_SLAVE_MOBILE_RSSI_CODE            0x0C
#define HCI_DBG_IBRT_SWITCH_COMPLETE_EVT_SUBCODE      0x0D
#define HCI_DBG_IBRT_CONNECTED_EVT_SUBCODE            0x0E
#define HCI_DBG_IBRT_DISCONNECTED_EVT_SUBCODE         0x0f
#define HCI_DBG_SNIFFER_INIT_CMP_EVT_SUBCODE          0x10
#define HCI_DBG_RX_SEQ_ERROR_EVT_SUBCODE              0x11
#define HCI_DBG_FA_TO_EVT_SUBCODE                     0x14
#define HCI_DBG_ECC_DATA_TEST_EVT_SUBCODE             0x15
#define HCI_DBG_MULTI_IBRT_ACL_SCH_CHG_EVT_SUBCODE    0x16
#define HCI_DBG_TX_PWR_CHG_EVT_SUBCODE                0x24

#if BLE_AUDIO_ENABLED
#define HCI_DBG_ISO_QUALITY_REPORT_EVT_SUBCODE        0x26
#endif

#ifdef IS_TWS_IBRT_DEBUG_SYSTEM_ENABLED
#define HCI_DBG_LINK_LOSS_INFO_EVT_SUBCODE            0x27
#define HCI_DBG_LINK_LOSS_CLOCK_INFO_EVT_SUBCODE      0x28
#define HCI_DBG_A2DP_SINK_INFO_EVT_SUBCODE            0x29
#endif

#define HCI_DBG_SCO_SWITCH_RESULT_EVT_SUBCODE         0x2a



//ext_opcode for reserved vendor event
//HCI_DBG_LMP_RESERVED_EVT_SUBCODE
#define HCI_BLOCK_CHANNEL_REPORT_EXT_OPCODE    0x1

void app_hci_vendor_event_callback_init(void);
void app_hci_vendor_event_handler_v2(uint8_t* pbuf, uint32_t length);
void btapp_sniffer_sco_start(uint16_t conhdl,uint8_t sco_status,uint8_t airmode,uint32_t bandwidth);
void btapp_sniffer_ibrt_op(uint16_t conhdl, uint8_t op, uint8_t status, uint8_t reason);
void app_ibrt_ui_rx_seq_error_handling_timer_cb(void const *current_evt);
void app_hci_vender_ibrt_disconnected_handle(uint8_t *pbuf);
ibrt_role_e app_hci_interpreter_controller_role(uint8_t controller_role);
void app_hci_debug_trace_callback_init(void);
#endif
