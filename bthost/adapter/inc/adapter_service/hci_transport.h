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
#ifndef __HCI_TRANSPORT_H__
#define __HCI_TRANSPORT_H__
#include "bt_common_define.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CFG_SCO_OVER_HCI 0

#define HCI_INVALID_CONN_HANDLE 0xffff
#define HCI_CONN_HANDLE_MASK 0x0fff
#define HCI_CONN_FLAG_SHIFTS (12)

#define HCI_DATA_TYPE_LEN           1
#define HCI_EVT_HEADER_LEN          2
#define HCI_CMD_HEADER_LEN          3
#define HCI_SCO_HEADER_LEN          3
#define HCI_ACL_HEADER_LEN          4
#define HCI_ISO_HEADER_LEN          4
#define HCI_DEBUG_HEADER_LEN        3

#define HCI_L2CAP_HEADER_LEN        4
#define HCI_ISO_SDU_HEADER_LEN      4 // without TIme_Stamp
#define HCI_ISO_SDU_MAX_HEADER_LEN  8 // include Time_Stamp

typedef enum {
    HCI_DATA_TYPE_CMD = 0x01,
    HCI_DATA_TYPE_ACL = 0x02,
    HCI_DATA_TYPE_SCO = 0x03,
    HCI_DATA_TYPE_EVT = 0x04,
    HCI_DATA_TYPE_ISO = 0x05,
    HCI_DATA_TYPE_DEBUG = 0x06,
} hci_data_type_t;

#define HCI_ACL_RX_DATA_SIZE 1021
#if defined(BT_BNEP_SUPPORT)
#undef HCI_ACL_RX_DATA_SIZE
#define HCI_ACL_RX_DATA_SIZE 1700
#endif
#define HCI_SCO_RX_DATA_SIZE 255

#ifdef BLE_HOST_SUPPORT
#define HCI_NUM_ACL_RX_HOST_BUFFERS  (6+16)
#else
#define HCI_NUM_ACL_RX_HOST_BUFFERS  6
#endif
#define HCI_NUM_SCO_RX_HOST_BUFFERS  6
#define HCI_NUM_EVT_RX_HOST_BUFFERS 24
#define HCI_NUM_ISO_RX_HOST_BUFFERS 24
#define HCI_NUM_ISO_TX_HOST_BUFFERS 24

#define HCI_BT_ACL_TX_DATA_SIZE 1021 // initial value, shall overwrite by read buffer size comand
#define HCI_BT_SCO_TX_DATA_SIZE 255
#define HCI_LE_ACL_TX_DATA_SIZE 251
#define HCI_LE_ISO_TX_DATA_SIZE 400

#define HCI_NUM_CMD_TX_CONTROLLER_BUFFERS     6 // initial value, shall overwrite by read buffer size comand
#define HCI_NUM_BT_ACL_TX_CONTROLLER_BUFFERS  6
#define HCI_NUM_BT_SCO_TX_CONTROLLER_BUFFERS  6
#define HCI_NUM_LE_ACL_TX_CONTROLLER_BUFFERS 16
#define HCI_NUM_LE_ISO_TX_CONTROLLER_BUFFERS 20

#define HCI_NUM_CMD_TX_HOST_BUFFERS    40
#define HCI_NUM_BT_ACL_TX_HOST_BUFFERS 80
#define HCI_NUM_BT_SCO_TX_HOST_BUFFERS 12
#define HCI_NUM_LE_ACL_TX_HOST_BUFFERS 80

#define HCI_L2CAP_LENGTH_TOO_LONG (1024*5)

#define HCI_ACL_PB_FLAG_MASK 0x3000
#define HCI_ACL_BC_FLAG_MASK 0xc000

typedef enum {
    HCI_ACL_TX_START_NONE_AUTOFLUSH = 0x0000,
    HCI_ACL_TX_CONTINUE_FRAGMENT    = 0x1000,
    HCI_ACL_TX_START_AUTO_FLUSHABLE = 0x2000,
    HCI_ACL_RX_START_NONE_AUTOFLUSH = 0x0000, // only allowed for loopback mode
    HCI_ACL_RX_CONTINUE_FRAGMENT    = 0x1000,
    HCI_ACL_RX_START_FRAGMENT       = 0x2000,

    HCI_LEU_TX_START_NONE_FLUSHABLE = 0x0000, // le acl data are all non-automatic-flushable
    HCI_LEU_TX_CONTINUE_FRAGMENT    = 0x1000,
    HCI_LEU_RX_CONTINUE_FRAGMENT    = 0x1000,
    HCI_LEU_RX_START_FRAGMENT       = 0x2000,

    HCI_APB_TX_CONTINUE_FRAGMENT    = 0x1000,
    HCI_APB_TX_START_AUTO_FLUSHABLE = 0x2000,
    HCI_APB_RX_CONTINUE_FRAGMENT    = 0x1000,
    HCI_APB_RX_START_FRAGMENT       = 0x2000,
} hci_acl_pb_flag_t;

typedef enum {
    HCI_ACL_BC_POINT_TO_POINT  = 0x0000, // acl-u or le-u
    HCI_ACL_BC_BREDR_BROADCAST = 0x4000, // apb-u
} hci_acl_bc_flag_t;

struct hci_acl_header_t {
    uint8_t acl_hci_type;
    uint16_t acl_conn_handle;
    uint16_t acl_data_length;
} __attribute__ ((packed));

struct l2cap_header_t {
    uint16_t l2cap_pdu_len;
    uint16_t channel_id;
} __attribute__ ((packed));

#define HCI_SCO_PS_FLAG_MASK 0x3000

typedef enum {
    HCI_SCO_PS_GOOD_DATA = 0x0000,
    HCI_SCO_PS_MAY_ERROR = 0x1000,
    HCI_SCO_PS_LOST_DATA = 0x2000, // payload data length shall be 0
    HCI_SCO_PS_SOME_LOST = 0x3000, // the missing packet payload data length shall be 0
} hci_sco_ps_flag_t;

struct hci_sco_header_t {
    uint8_t sco_hci_type;
    uint16_t sco_conn_handle;
    uint8_t sco_data_length;
} __attribute__ ((packed));

struct hci_cmd_header_t {
    uint8_t cmd_hci_type;
    uint16_t cmd_opcode;
    uint8_t cmd_data_len;
} __attribute__ ((packed));

struct hci_evt_header_t {
    uint8_t evt_hci_type;
    uint8_t evt_opcode;
    uint8_t evt_data_len;
} __attribute__ ((packed));

struct hci_debug_trace_header_t {
    uint8_t debug_hci_type;
    uint8_t debug_opcode;
    uint8_t debug_data_len;
} __attribute__ ((packed));

#define HCI_ISO_PB_FLAG_MASK 0x3000
#define HCI_ISO_TS_FLAG_MASK 0x4000
#define HCI_ISO_PYL_LEN_MASK 0x3fff
#define HCI_ISO_SDU_LEN_MASK 0x0fff
#define HCI_ISO_PS_FLAG_MASK 0xc000

typedef enum {
    HCI_ISO_PB_START_FRAGMENT    = 0x0000,
    HCI_ISO_PB_CONTINUE_FRAGMENT = 0x1000,
    HCI_ISO_PB_COMPLETE_SDU      = 0x2000,
    HCI_ISO_PB_LAST_FRAGMENT     = 0x3000,
} hci_iso_pb_flag_t;

typedef enum {
    HCI_ISO_TS_FLAG_CLR = 0x0000,
    HCI_ISO_TS_FLAG_SET = 0x4000,   // TS_FLAG shall only be set if the PB_FLAG is HCI_ISO_PB_START_FRAGMENT or HCI_ISO_PB_COMPLETE_SDU
} hci_iso_ts_flag_t;

typedef enum {
    HCI_ISO_PS_GOOD_DATA = 0x0000,
    HCI_ISO_PS_MAY_ERROR = 0x4000,
    HCI_ISO_PS_LOST_DATA = 0x8000,  // PB_FLAG shall be HCI_ISO_PB_COMPLETE_SDU in this case
} hci_iso_ps_flag_t;

struct hci_iso_header_t {
    uint8_t iso_hci_type;
    uint16_t iso_conn_handle;       // conhdl 0~11 pb_flag 12~13 ts_flag 14 rfu 15
    uint16_t iso_data_load_length;  // length 0~13 rfu 14~15
    uint16_t iso_sdu_sequence_num;
    uint16_t iso_sdu_length;        // sdu_length 0~11 rfu 12~13 ps_flag 14~15
} __attribute__ ((packed));

struct hci_iso_timestamp_header_t {
    uint8_t iso_hci_type;
    uint16_t iso_conn_handle;       // conhdl 0~11 pb_flag 12~13 ts_flag 14 rfu 15
    uint16_t iso_data_load_length;  // length 0~13 rfu 14~15
    uint32_t iso_sdu_timestamp;
    uint16_t iso_sdu_sequence_num;
    uint16_t iso_sdu_length;        // sdu_length 0~11 rfu 12~13 ps_flag 14~15
} __attribute__ ((packed));

#define DUMP_PUSH_POP_TIME 0

struct hci_rx_desc_flag_t {
    uint16_t hci_type: 3;
    uint16_t sdu_rx_complete: 1;
    uint16_t acl_pb_flag: 4;
    uint16_t acl_bc_flag: 4;
    uint16_t sco_ps_flag: 4;
} __attribute__ ((packed));

struct hci_iso_flag_t {
    uint16_t iso_pb_flag: 4;
    uint16_t iso_ts_flag: 4;
    uint16_t iso_ps_flag: 4;
} __attribute__ ((packed));

struct hci_rx_desc_t {
    struct single_link_node_t node;
    struct hci_rx_desc_flag_t rx_flag;
    uint8_t num_sdu_fragments;
    uint8_t sdu_header_len;
    uint16_t hci_packet_len;
    uint16_t sdu_data_len;
    uint16_t buffer_offset;
    uint16_t connhdl; // connhdl or event opcode
#if DUMP_PUSH_POP_TIME==1
    uint32_t push_time;
#endif
};

#ifdef BLE_ISO_ENABLED
struct hci_rx_iso_desc_t {
    struct hci_rx_desc_t head;
    struct hci_iso_flag_t iso_flag;
    uint16_t iso_sdu_seqn;
    uint32_t iso_time_stamp;
};

struct hci_tx_iso_desc_t {
    struct single_link_node_t node;
    uint16_t connhdl;
    uint16_t iso_packet_len;
    uint16_t iso_sdu_seqn;
    void (*tx_packet_free)(struct hci_tx_iso_desc_t *);
};
#endif

struct hci_transport_callback_t {
    void *(*hci_rx_alloc)(hci_data_type_t type, uint16_t size);
    void (*hci_rx_free)(hci_data_type_t type, uint8_t *buffer);
    void (*hci_rx_setup)(hci_data_type_t type, uint8_t *buffer, uint16_t packet_len);
#ifdef BLE_ISO_ENABLED
    bool (*hci_tx_iso_ind)(struct hci_tx_iso_desc_t *tx_desc);
#endif
    void (*hci_rx_data_ind)(struct hci_rx_desc_t *rx_desc);
};

void hci_transport_init(struct hci_transport_callback_t *callback);
void hci_tx_data_packet(const uint8_t *data, uint16_t len);
struct hci_rx_desc_t *hci_alloc_rx_desc(hci_data_type_t data_type, uint16_t packet_len);
void hci_rx_desc_free(struct hci_rx_desc_t *rx_desc);
uint8_t *hci_rx_packet_buffer(struct hci_rx_desc_t *rx_desc);
int hci_signal_rx_complete(uint16_t connhdl, uint8_t num_complete);
int hci_rx_queue_available_space(void);
#ifdef BLE_ISO_ENABLED
int hci_signal_tx_iso_data(uint32_t data);
int hci_signal_iso_continue_tx(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* __HCI_TRANSPORT_H__ */
