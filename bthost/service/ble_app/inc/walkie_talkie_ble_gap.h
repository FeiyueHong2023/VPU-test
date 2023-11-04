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
#ifndef __WALKIE_TALKIE_BLE_GAP_H__
#define __WALKIE_TALKIE_BLE_GAP_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WALKIE_GAP_EXT_ADV,
    WALKIE_GAP_PER_ADV,
    WALKIE_GAP_BOARDCAST,
} WALKIE_ADV_TYPE_E;

typedef enum {
    WALKIE_GAP_DATA_INVALIE,
    WALKIE_GAP_DATA_DEV_INFO,
    WALKIE_GAP_DATA_START,
    WALKIE_GAP_DATA_AUDIO,
    WALKIE_GAP_DATA_END,
} WALKIE_DATA_TYPE_E;

typedef enum {
    /// gap init complete, para = NULL
    WALKIE_GAP_INIT_DONE,
    /// find walkie talkie adv, para = &walkie_gap_data_t
    WALKIE_GAP_FIND_DEV,
   /// periodic active create, para = &actv_idx
    WALKIE_GAP_PER_ACTV_CREATE,
    /// periodic sync established, para = &actv_idx
    WALKIE_GAP_PER_SYNC_EST,
    /// periodic sync LOST, para = &actv_idx
    WALKIE_GAP_PER_SYNC_LOST,
    /// periodic sync STOP, para = &actv_idx
    WALKIE_GAP_PER_SYNC_STOPED,
    /// receive start data packet, para = &actv_idx
    WALKIE_GAP_START_DATA,
    /// receive audio data packet, para = &walkie_gap_data_t
    WALKIE_GAP_AUDIO_DATA,
    /// active scan rsponse
    WALKIE_GAP_ACTIVE_SCAN_RSP,
    ///  receive end data packet, para = &actv_idx
    WALKIE_GAP_END_DATA,
    /// big info report
    WALKIE_GAP_BIG_INFO_REPORT,
    ///
    WALKIE_BG_SYNC_STATUS_ESTABLISHED ,
    /// Synchronization has failed
    WALKIE_BG_SYNC_STATUS_FAILED,
    /// Synchronization establishment has been cancelled
    WALKIE_BG_SYNC_STATUS_CANCELLED,
    /// Synchronization has been lost
    WALKIE_BG_SYNC_STATUS_LOST,
    /// RX ISO datapth setup complete
    WALKIE_BG_RX_DP_SETUP,
} WALKIE_BLE_GAP_EVENT_E;

typedef enum
{
    WALKIE_SCAN_PASSIVE,
    WALKIE_SCAN_ACTIVE,
}WALKIE_BLE_GAP_SCAN_TYP_E;

typedef void (*walkie_ble_gap_event_cb)(WALKIE_BLE_GAP_EVENT_E event, void *para);

typedef struct
{
    /// device id
    uint8_t actv_idx;
    /// only WALKIE_GAP_DATA_DEV_INFO data type used
    uint8_t *addr;
    // packet number
    uint8_t pacet_num;
    /// audio data_length
    uint8_t data_len;
    /// audio data
    uint8_t *data;
} walkie_gap_data_t;

typedef struct walkie_bdaddr
{
    /// BD Address of device
    uint8_t addr[6];
    /// Address type of the device 0=public/1=private random
    uint8_t addr_type;
} walkie_bdaddr_t;

typedef struct
{
    /// active_idx
    uint8_t actv_idx;
     /// BIG_Handle is used to identify the BIS Group. (Range 0x00-0xEF)
    uint8_t  big_hdl;
    /// Used to identify the periodic advertising set (Range 0x0000-0x0EFF)
    uint16_t sync_hdl;

    /// Value of the SDU interval in microseconds (Range 0x0000FF-0x0FFFFF)
    uint32_t  sdu_interval;
    /// Value of the ISO Interval (1.25 ms unit)
    uint16_t  iso_interval;
    /// Value of the maximum PDU size (Range 0x0000-0x00FB)
    uint16_t  max_pdu;
    /// VValue of the maximum SDU size (Range 0x0000-0x0FFF)
    uint16_t  max_sdu;
    /// Number of BIS present in the group (Range 0x01-0x1F)
    uint8_t   num_bis;
    /// Number of sub-events (Range 0x01-0x1F)
    uint8_t   nse;
    /// Burst number (Range 0x01-0x07)
    uint8_t   bn;
    /// Pre-transmit offset (Range 0x00-0x0F)
    uint8_t   pto;
    /// Initial retransmission count (Range 0x01-0x0F)
    uint8_t   irc;
    /// PHY used for transmission (0x01: 1M, 0x02: 2M, 0x03: Coded, All other values: RFU)
    uint8_t   phy;
    /// Framing mode (0x00: Unframed, 0x01: Framed, All other values: RFU)
    uint8_t   framing;
    /// True if broadcast isochronous group is encrypted, False otherwise
    bool      encrypted;
}walkie_gap_big_info_t;

void walkie_ble_gap_init(WALKIE_ADV_TYPE_E adv_type, walkie_ble_gap_event_cb event_cb);
void walkie_ble_gap_deinit(void);

int  walkie_add_device_to_list(const walkie_bdaddr_t *bdaddr, uint8_t size);
void walkie_clear_device_list();

//full duplex mode need set defult device info data
void walkie_adv_start(uint8_t *def_dev_info_data, uint8_t data_len);
void walkie_adv_stop(void);
void walkie_send_data(WALKIE_DATA_TYPE_E data_type, uint8_t *p_data, uint8_t data_len);

void walkie_scan_start(WALKIE_BLE_GAP_SCAN_TYP_E scan_type);
void walkie_scan_stop(WALKIE_BLE_GAP_SCAN_TYP_E scan_type);

bool walkie_per_sync_start(const uint8_t *addr);
void walkie_per_sync_stop(uint8_t actv_idx);

void walkie_set_audio_streaming_adv_type(WALKIE_ADV_TYPE_E adv_type);
WALKIE_ADV_TYPE_E walkie_get_audio_streaming_adv_type();

uint8_t *walkie_get_trans_device_addr(uint8_t actv_idx);

bool walkie_send_big_create_sync_hci_cmd(const walkie_gap_big_info_t* p_big_info);
void walkie_talkie_print_big_info(const walkie_gap_big_info_t* p_big_info);
void walkie_talkie_big_src_create();
bool walkie_bc_src_start_streaming();
void walkie_bc_src_stop_streaming();
void walkie_bc_src_send_data(uint8_t *payload, uint16_t payload_len);

uint8_t walkie_bis_rx_stream_start_by_con_hdl(uint16_t con_hdl);
uint8_t walkie_bis_rx_stream_start(uint8_t actv_idx);
void walkie_bis_rx_stream_stop(uint8_t actv_idx);
void walkie_bis_register_recv_iso_data_callback();
void walkie_bis_unregister_recv_iso_data_callback();


#ifdef __cplusplus
}
#endif

#endif /*_WALKIE_TALKIE_BLE_GAP_H__*/
