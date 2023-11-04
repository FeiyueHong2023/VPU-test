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

#include "ke_task.h"
#include "ke_msg.h"
#include "gapm_le_msg.h"
#include "gapc_msg.h"
#include "bt_drv_reg_op.h"
#include "walkie_talkie_ble_gap.h"
#include "co_hci.h"
#include "hl_hci.h"
#include "data_path.h"
#include "iap.h"
#include "walkie_talkie_iso_data_path.h"
#include "isoohci_int.h"

#define WALKIE_LOG_T "W-T-G"

#define ADDR_IS_EMPTY(arrr) (!(arrr[0] || arrr[1] || arrr[2] || arrr[3] || arrr[4] || arrr[5]))
#define WALKIE_ADV_DATA_MAX     0xE5
#define WALKIE_EXT_T            0x20
#define WALKIE_PER_EXT_T        0x30
#define WALKIE_PER_T            0x18

/* WALKIE_DEVICE_ID_BASE      (2)    // ADV Active = 0, SCAN Active = 1, PA sync start from 2 ..*/
enum WAKIE_BIG_OPERATION{
    WALKIE_BIG_CREAT_CMP            = 0,
    WALKIE_BIG_RX_DATA_SETUP_CMP    = 1,
    WALKIE_BIG_TX_DATA_SETUP_CMP    = 2,
    WALKIE_BIG_RMV_TX_DP            = 3,
    WALKIE_BIG_RMV_RX_DP            = 4,
    WALKIE_BIG_SYNC_CMP             = 5,
};

#define BIG_HDL_TO_ACTV_IDX(BIG_HDL)      (BIG_HDL + 1)

typedef struct send_adv_data_t{
    uint8_t data[WALKIE_ADV_DATA_MAX];
    uint8_t len;
}send_adv_data_t;

//ADV Process Information
typedef struct walkie_adv_para {
    // walkie adv active handle
    uint8_t actv_idx;
    // walkie adv active is enable
    bool    actv_enable;
    // send max ADV number
    uint8_t max_adv_evt;
    // send ADV duration
    uint16_t duration;
    // advertisement data
    send_adv_data_t adv_data;
    // creat ADV para
    struct gapm_activity_create_adv_cmd para;
}walkie_adv_para_t;

//ADV Process Information
typedef struct walkie_periodic_sync_para {
    struct{
        // walkie adv active handle
        uint8_t actv_idx;
        // walkie adv active is enable
        bool    actv_enable;
        // walkie adv addr
        uint8_t addr[GAP_BD_ADDR_LEN];
        // previous packet number
        uint8_t prev_pack_num;
        // packet start flg
        bool    pk_start_flg;
    } device_info[WALKIE_DEVICE_MAX];
    // creat ADV para
    gapm_per_sync_param_t para;
}walkie_periodic_sync_para_t;

//SCAN Process Information
typedef struct walkie_scan_para {
    // walkie adv active handle
    uint8_t actv_idx;
    // walkie scan active is enable
    uint8_t actv_enable;
    // previous packet number
    uint8_t prev_pack_num;
    // packet start flg
    bool    pk_start_flg;
    /// the address of the device being listened to
    uint8_t rev_device[GAP_BD_ADDR_LEN];
    // creat scan para
    struct gapm_scan_param para;
}walkie_scan_para_t;

typedef struct walkie_addr_list {
    uint8_t addr_num;
    gap_bdaddr_t addr_list[WALKIE_DEVICE_MAX];
}walkie_addr_list_t;

typedef struct walkie_data_header {
    uint8_t  data_len;
    uint8_t  data_type;
    uint8_t  pacet_num;
    uint8_t  pacet_type;
}walkie_data_header_t;

//BIS Process Information
typedef struct wt_bis_para{
    uint16_t conhdl;
    struct gapm_activity_create_adv_cmd *adv_para;
    struct hci_le_create_big_cmd big_para;
    struct hci_le_create_big_cmp_evt big_creat_cmp;

    /// actv_idx start from 2,big_hdl start from 1
    struct {
        uint8_t  actv_idx;
        uint8_t  big_hdl;
        uint16_t bis_con_hdl;
    } bis_receiver[WALKIE_DEVICE_MAX - 1];
}walkie_bis_para_t;

static walkie_addr_list_t walkie_addr = {0};
static walkie_ble_gap_event_cb walkie_event_cb = NULL;
static WALKIE_ADV_TYPE_E audio_adv_type = WALKIE_GAP_PER_ADV;;

uint16_t iso_data_seq = 0;

#ifdef BLE_ISO_ENABLED
static void walkie_talkie_big_hci_cmd_cmp_handler(uint16_t opcode, uint16_t event, void const *p_evt);
#endif

void walkie_set_audio_streaming_adv_type(WALKIE_ADV_TYPE_E adv_type)
{
    audio_adv_type = adv_type;
}

WALKIE_ADV_TYPE_E walkie_get_audio_streaming_adv_type()
{
    return audio_adv_type;
}

static void walkie_stop_activity(uint8_t actv_idx)
{
    struct gapm_activity_stop_cmd *cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                                  TASK_GAPM, TASK_BLE_WALKIE,
                                                  gapm_activity_stop_cmd);
    if (!cmd)
    {
        return;
    }

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = actv_idx;

    // Send the message
    ke_msg_send(cmd);
}

static void walkie_delete_activity(uint8_t actv_idx)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_delete_cmd *cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
                                                      TASK_GAPM, TASK_BLE_WALKIE,
                                                      gapm_activity_delete_cmd);
    if (!cmd)
    {
        return;
    }

    // Fill the allocated kernel message
    cmd->operation = GAPM_DELETE_ACTIVITY;
    cmd->actv_idx = actv_idx;

    // Send the message
    ke_msg_send(cmd);
}

//ble host walkie adv function
static walkie_adv_para_t walkie_adv_info = {
    .actv_idx     = 0xFF,
    .actv_enable  = false,
    .max_adv_evt  = 0,
    .duration     = 0,
    .adv_data     = {{0}, 0},
    .para = {
        .own_addr_type = GAPM_STATIC_ADDR,
        .type = GAPM_ADV_TYPE_EXTENDED,
        .adv_param.prop = GAPM_ADV_PROP_NON_CONN_NON_SCAN_MASK,
        .adv_param.disc_mode = GAPM_ADV_MODE_GEN_DISC,
        .adv_param.with_flags = false,
        .adv_param.max_tx_pwr = 12,
        .adv_param.filter_pol = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY,
        .adv_param.peer_addr.addr_type = GAPM_STATIC_ADDR,
        .adv_param.peer_addr.addr = {0},
        .adv_param.local_addr.addr = {0},
        .adv_param.prim_cfg.adv_intv_min = WALKIE_EXT_T,
        .adv_param.prim_cfg.adv_intv_max = WALKIE_EXT_T,
        .adv_param.prim_cfg.chnl_map = 0x07,
        .adv_param.prim_cfg.phy = GAP_PHY_LE_1MBPS,
        .second_cfg.max_skip = 0,
        .second_cfg.phy = GAP_PHY_LE_1MBPS,
        .second_cfg.adv_sid = 0,
        .period_cfg.interval_min = WALKIE_PER_T,
        .period_cfg.interval_max = WALKIE_PER_T,
        .switching_pattern_len = 0,
        .randomAddrRenewIntervalInSecond = 0,
        .cte_cfg = {0},
    },
};

// BIS
#ifdef BLE_ISO_ENABLED
// sdu_interval= 30000   iso_interval=24(1.25 unit ms), max_sdu=100,   num_bis=1
// NSE = 3                BN =1               PTO = 1        IRC = 1
static walkie_bis_para_t walkie_big_info = {
    .adv_para = &walkie_adv_info.para,
    .big_para = {
        .big_hdl = 0,
        .adv_hdl = 0,
        .num_bis = 1,
        .sdu_interval = WALKIE_BC_SDU_INTERVAL_US,
        .max_sdu = WALKIE_BC_MAX_SDU_SIZE,
        .trans_latency = WALKIE_BC_MAX_TRANS_LATENCY_MS,
        .rtn = 2,
        .phy = 1,
        .packing = 0,
        .framing = 0,
        .encryption = 0,
        .broadcast_code = {0},
        },

        .bis_receiver =
          {{2,1,0xFFF},{3,2,0xFFF},{4,3,0xFFF},{5,4,0xFFF},{6,5,0xFFF},
          {7,6,0xFFF},{8,7,0xFFF},{9,8,0xFFF},{10,9,0xFFF},},
};
#endif

static void walkie_adv_creat(struct gapm_activity_create_adv_cmd *adv_para)
{
    struct gapm_activity_create_adv_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
                                                    TASK_GAPM, TASK_BLE_WALKIE,
                                                    gapm_activity_create_adv_cmd);

    if (!p_cmd)
    {
       return;
    }

    memcpy(p_cmd, adv_para, sizeof(struct gapm_activity_create_adv_cmd));
    // Set operation code
    p_cmd->operation = GAPM_CREATE_ADV_ACTIVITY;

    // Send the message
    ke_msg_send(p_cmd);
}

static void walkie_adv_enable(uint16_t duration, uint8_t max_adv_evt)
{
    // Prepare the GAPM_ACTIVITY_START_CMD message
    struct gapm_activity_start_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_ACTIVITY_START_CMD,
                                                    TASK_GAPM, TASK_BLE_WALKIE,
                                                    gapm_activity_start_cmd,
                                                    sizeof(gapm_adv_param_t));
    if (!p_cmd)
    {
       return;
    }

    gapm_adv_param_t *adv_add_param = (gapm_adv_param_t *)(p_cmd->u_param);
    p_cmd->operation = GAPM_START_ACTIVITY;
    p_cmd->actv_idx = walkie_adv_info.actv_idx;
    adv_add_param->duration = duration;
    adv_add_param->max_adv_evt = max_adv_evt;

    // Send the message
    ke_msg_send(p_cmd);
}

static void walkie_adv_disenable()
{
    walkie_stop_activity(walkie_adv_info.actv_idx);
}

static void walkie_adv_delete()
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    walkie_delete_activity(walkie_adv_info.actv_idx);

    //clear adv_info
    walkie_adv_info.actv_idx = 0xFF;
}

static void walkie_set_adv_data(uint8_t operation, const WALKIE_DATA_TYPE_E packet_type,
                                        const uint8_t *adv_data, uint8_t data_len)
{
    static uint8_t ea_pack_num = 1;
    static uint8_t pa_pack_num = 1;
    walkie_data_header_t data_header;

    if(WALKIE_ADV_DATA_MAX < data_len)
    {
        TRACE(0, "%s: %s: data len error! max=%d, len=%d",
            WALKIE_LOG_T,__FUNCTION__, WALKIE_ADV_DATA_MAX, data_len);
        return;
    }

    data_header.data_len   = sizeof(walkie_data_header_t) - 1 + data_len;
    data_header.data_type  = GAP_AD_TYPE_WALKIE;
    data_header.pacet_type = packet_type;
    if(packet_type != WALKIE_GAP_DATA_INVALIE)
    {
        if(operation == GAPM_SET_ADV_DATA)
        {
            if(packet_type == WALKIE_GAP_DATA_START)
            {
                ea_pack_num = 1;
            }
            data_header.pacet_num  = ea_pack_num;
            ea_pack_num++;
        }
        else if(operation == GAPM_SET_PERIOD_ADV_DATA)
        {
            if(packet_type == WALKIE_GAP_DATA_START)
            {
                pa_pack_num = 1;
            }
            data_header.pacet_num  = pa_pack_num;
            pa_pack_num++;
        }
    }
    else
    {
        data_header.pacet_num = 1;
    }

    // Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                TASK_GAPM, TASK_BLE_WALKIE,
                                                gapm_set_adv_data_cmd,
                                                data_header.data_len +1);
    if (!p_cmd)
    {
        return;
    }
    // Fill the allocated kernel message
    p_cmd->operation       = operation;
    p_cmd->actv_idx        = walkie_adv_info.actv_idx;
    p_cmd->length          = data_header.data_len + 1;

    memcpy(p_cmd->data, &data_header, sizeof(walkie_data_header_t));
    memcpy(p_cmd->data +  sizeof(walkie_data_header_t), adv_data, data_len);

    // Send the message
    ke_msg_send(p_cmd);
}

static walkie_periodic_sync_para_t walkie_per_info = {
    .para = {
        .skip = 0,
        .sync_to = 100,
        .type = GAPM_PER_SYNC_TYPE_GENERAL,
        .conidx = 0,
        .adv_addr.addr = {0},
        .adv_addr.addr_type = 0,
        .adv_addr.adv_sid = 0,
#ifdef BLE_ISO_ENABLED
        .report_en_bf = GAPM_REPORT_ADV_EN_BIT|GAPM_REPORT_BIGINFO_EN_BIT,
#else
        .report_en_bf = GAPM_REPORT_ADV_EN_BIT,
#endif
        .cte_type = 0,
    },
};

static bool walkie_periodic_sync_create(uint8_t own_addr_type)
{
    struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
                                                         TASK_GAPM, TASK_BLE_WALKIE,
                                                         gapm_activity_create_cmd);
    if (!p_cmd)
    {
        return false;
    }

    // Set operation code
    p_cmd->operation = GAPM_CREATE_PERIOD_SYNC_ACTIVITY;
    p_cmd->own_addr_type = own_addr_type;

    // Send the message
    ke_msg_send(p_cmd);

    return true;
}

static void walkie_periodic_sync_enable(uint8_t actv_idx, const gapm_per_sync_param_t *per_para)
{
    // Prepare the GAPM_ACTIVITY_START_CMD message
    struct gapm_activity_start_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_ACTIVITY_START_CMD,
                                                         TASK_GAPM, TASK_BLE_WALKIE,
                                                         gapm_activity_start_cmd,
                                                         sizeof(gapm_per_sync_param_t));
    if (!p_cmd)
    {
        return;
    }

    gapm_per_sync_param_t *per_sync_param = (gapm_per_sync_param_t *)(p_cmd->u_param);

    p_cmd->operation = GAPM_START_ACTIVITY;
    p_cmd->actv_idx = actv_idx;
    memcpy(per_sync_param, per_para, sizeof(gapm_per_sync_param_t));

    // Send the message
    ke_msg_send(p_cmd);
}

static void walkie_periodic_sync_disenable(uint8_t actv_idx)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    walkie_stop_activity(actv_idx);
}

static void walkie_periodic_sync_delete(uint8_t actv_idx)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    walkie_delete_activity(actv_idx);
}

//ble host walkie scan function
static walkie_scan_para_t walkie_scan_info = {
    .actv_idx = 0xFF,
    .actv_enable = false,
    .prev_pack_num = 0,
    .pk_start_flg = 0,
    .rev_device = {0},
    .para = {
        .type = GAPM_SCAN_TYPE_SEL_OBSERVER,
        .prop = GAPM_SCAN_PROP_PHY_1M_BIT,
        .dup_filt_pol = GAPM_DUP_FILT_DIS,
        .rsvd         = 0,
        .scan_param_1m.scan_intv = 50,
        .scan_param_1m.scan_wd =50,
        .scan_param_coded.scan_intv = 0,
        .scan_param_coded.scan_wd = 0,
        .duration = 0,
        .period = 0,
    },
};

static void walkie_scan_creat(uint8_t own_addr_type)
{
    struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
       TASK_GAPM, TASK_BLE_WALKIE, gapm_activity_create_cmd);
    if (!p_cmd)
    {
        return;
    }

    // Set operation code
    p_cmd->operation = GAPM_CREATE_SCAN_ACTIVITY;
    p_cmd->own_addr_type = own_addr_type;

    // Send the message
    ke_msg_send(p_cmd);
}

static void walkie_scan_enable(uint8_t activity_idx,gapm_scan_param_t *scan_para)
{
    // Prepare the GAPM_ACTIVITY_START_CMD message
    struct gapm_activity_start_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_ACTIVITY_START_CMD,
                                                         TASK_GAPM, TASK_BLE_WALKIE,
                                                         gapm_activity_start_cmd,
                                                         sizeof(gapm_scan_param_t));
    if (!p_cmd)
    {
        return;
    }

    gapm_scan_param_t *scan_param = (gapm_scan_param_t *)(p_cmd->u_param);

    p_cmd->operation = GAPM_START_ACTIVITY;
    p_cmd->actv_idx = activity_idx;
    memcpy(scan_param, scan_para, sizeof(gapm_scan_param_t));

    // Send the message
    ke_msg_send(p_cmd);
}

static void walkie_scan_disenable(uint8_t active_idx)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    walkie_stop_activity(active_idx);
}

static void walkie_scan_delete()
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    walkie_delete_activity(walkie_scan_info.actv_idx);
    //clear scan info
    walkie_scan_info.actv_idx = 0xFF;
}
static void walkie_set_device_list(uint8_t list_type, gap_bdaddr_t *bdaddr, uint8_t size)
{
    // Prepare the GAPM_ACTIVITY_START_CMD message
    uint8_t para_len = ((list_type == GAPM_SET_WL)? sizeof(gap_bdaddr_t):sizeof(gap_per_adv_bdaddr_t)) * size;
    struct gapm_list_set_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_LIST_SET_CMD,
                                                         TASK_GAPM, TASK_BLE_WALKIE,
                                                         gapm_list_set_cmd,
                                                         para_len);
    if (!p_cmd)
    {
        return;
    }

    p_cmd->operation = list_type;
    p_cmd->size = size;
    if(bdaddr)
    {
        if (list_type == GAPM_SET_WL){
            struct gapm_list_set_wl_cmd *para_data = (struct gapm_list_set_wl_cmd *)p_cmd;
            memcpy(&para_data->wl_info[0], bdaddr, sizeof(gap_bdaddr_t)*size);
        }
        else
        {
            struct gapm_list_set_pal_cmd *para_data = (struct gapm_list_set_pal_cmd *)p_cmd;
            for(int i=0; i<size; i++)
            {
                memcpy(&para_data->pal_info[i], &bdaddr[i], sizeof(gap_bdaddr_t));
                para_data->pal_info[i].adv_sid = 0;
            }
        }
    }
    // Send the message
    ke_msg_send(p_cmd);
}

static int *walkie_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                            struct gapm_cmp_evt *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    TRACE(0, "%s: %s status=%x, oper=%x, actv_idx=%x", WALKIE_LOG_T,__FUNCTION__,
                                param->status, param->operation, param->actv_idx);
    if(param->status != GAP_ERR_NO_ERROR){
        return KE_MSG_CONSUMED;
    }

    switch(param->operation)
    {
        case GAPM_CREATE_ADV_ACTIVITY:
            walkie_adv_info.actv_idx = param->actv_idx;
            break;
        case GAPM_CREATE_SCAN_ACTIVITY:
            walkie_scan_info.actv_idx = param->actv_idx;
            break;
        case GAPM_CREATE_PERIOD_SYNC_ACTIVITY:
        {
            walkie_gap_data_t gap_data = {0,};

            for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
            {
                if(walkie_per_info.device_info[i].actv_idx == 0)
                {
                    walkie_per_info.device_info[i].actv_idx = param->actv_idx;
                    memcpy(walkie_per_info.para.adv_addr.addr, walkie_per_info.device_info[i].addr, GAP_BD_ADDR_LEN);
                    break;
                }
            }
            gap_data.addr = walkie_per_info.para.adv_addr.addr;
            gap_data.actv_idx = param->actv_idx;

            walkie_event_cb(WALKIE_GAP_PER_ACTV_CREATE, &gap_data);
            walkie_periodic_sync_enable(param->actv_idx,&walkie_per_info.para);
        }
            break;
        case GAPM_START_ACTIVITY:
            if(param->status == GAP_ERR_NO_ERROR)
            {
                if((param->actv_idx == walkie_adv_info.actv_idx) && (!walkie_adv_info.duration))
                {
                    if (audio_adv_type == WALKIE_GAP_BOARDCAST)
                    {
#ifdef BLE_ISO_ENABLED
                        walkie_talkie_big_src_create();
#endif
                    }
                    walkie_adv_info.actv_enable = true;
                }
                else if(param->actv_idx == walkie_scan_info.actv_idx)
                {
                    walkie_scan_info.actv_enable = true;
                }
                else
                {
                    for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
                    {
                        if(walkie_per_info.device_info[i].actv_idx == param->actv_idx)
                        {
                            walkie_per_info.device_info[i].actv_enable = true;
                            break;
                        }
                    }
                }
            }
            break;
        case GAPM_STOP_ACTIVITY:
            if(param->status == GAP_ERR_NO_ERROR)
            {
                if(param->actv_idx == walkie_adv_info.actv_idx)
                {
                    walkie_adv_info.actv_enable = false;
                    if(walkie_adv_info.duration)
                    {
                        walkie_set_adv_data(GAPM_SET_ADV_DATA, WALKIE_GAP_DATA_END, NULL, 0);
                    }
                }
                else if(param->actv_idx == walkie_scan_info.actv_idx){
                    walkie_scan_info.actv_enable = false;
                }
                else{
                    for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
                    {
                        if(walkie_per_info.device_info[i].actv_idx == param->actv_idx)
                        {
                            walkie_per_info.device_info[i].actv_enable = false;
                            break;
                        }
                    }
                    walkie_periodic_sync_delete(param->actv_idx);
                }
            }
            break;
        case GAPM_DELETE_ACTIVITY:
            if(param->status == GAP_ERR_NO_ERROR)
            {
                if((param->actv_idx != walkie_adv_info.actv_idx)
                    && (param->actv_idx != walkie_scan_info.actv_idx))
                {
                    for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
                    {
                        if(walkie_per_info.device_info[i].actv_idx == param->actv_idx)
                        {
                            walkie_event_cb(WALKIE_GAP_PER_SYNC_STOPED, &walkie_per_info.device_info[i].actv_idx);
                            memset(&walkie_per_info.device_info[i], 0, sizeof(walkie_per_info.device_info[i]));
                            break;
                        }
                    }
                }
            }
            break;
        case GAPM_SET_ADV_DATA:
            if(walkie_adv_info.para.type == GAPM_ADV_TYPE_PERIODIC)
            {
                walkie_set_adv_data(GAPM_SET_PERIOD_ADV_DATA, WALKIE_GAP_DATA_INVALIE, NULL, 0);
                break;
            }
            // FALLTHROUGH
        case GAPM_SET_PERIOD_ADV_DATA:
            if(!walkie_adv_info.actv_enable)
            {
                walkie_adv_enable(walkie_adv_info.duration, walkie_adv_info.max_adv_evt);
            }
            break;
        case GAPM_SET_WL:
            if(walkie_adv_info.para.type == GAPM_ADV_TYPE_PERIODIC)
            {
                walkie_set_device_list(GAPM_SET_PAL, &walkie_addr.addr_list[0], walkie_addr.addr_num);
            }
            if (audio_adv_type == WALKIE_GAP_EXT_ADV)
            {    
                walkie_event_cb(WALKIE_GAP_INIT_DONE, NULL);
            }
            break;
        case GAPM_SET_PAL:
            if ((audio_adv_type == WALKIE_GAP_PER_ADV) || (audio_adv_type == WALKIE_GAP_BOARDCAST))
            {
                walkie_event_cb(WALKIE_GAP_INIT_DONE, NULL);
            }
            break;
        default:
            break;
    }

    return KE_MSG_CONSUMED;
}

static int walkie_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                      struct gapm_activity_created_ind const *p_param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    TRACE(0, "%s:actv index %d created, type=%d, tx_pwr=%d", WALKIE_LOG_T, p_param->actv_idx,
                            p_param->actv_type, p_param->tx_pwr);

    return (KE_MSG_CONSUMED);
}

static int walkie_gapm_activity_stopped_ind_handler(ke_msg_id_t const msgid,
                                      struct gapm_activity_stopped_ind const *p_param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    TRACE(0, "%s:activity_stopped:actv index %d, type=%d, %d,%d",
        WALKIE_LOG_T, p_param->actv_idx,p_param->actv_type, p_param->reason, p_param->per_adv_stop);
    if((p_param->actv_idx != walkie_adv_info.actv_idx)
          && (p_param->actv_idx != walkie_scan_info.actv_idx))
    {
        if(p_param->reason)
        {
            for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
            {
                if(walkie_per_info.device_info[i].actv_idx == p_param->actv_idx)
                {
                    walkie_per_info.device_info[i].actv_enable = false;
                    walkie_event_cb(WALKIE_GAP_PER_SYNC_LOST, &walkie_per_info.device_info[i].actv_idx);
                    walkie_periodic_sync_delete(p_param->actv_idx);
                    break;
                }
            }
        }
    }

    return (KE_MSG_CONSUMED);
}

static bool walkie_gapm_adv_report_filter(WALKIE_ADV_TYPE_E repot_type, uint8_t actv_idx, 
                                                            uint8_t *addr, walkie_data_header_t *header)
{
    bool *start_flg = NULL;
    uint8_t *prev_num = NULL;
    uint8_t *rev_addr = NULL;

    // Get filter condition data based on report type
    if(repot_type == WALKIE_GAP_EXT_ADV)
    {
        if (actv_idx == walkie_scan_info.actv_idx)
        {
            start_flg = &walkie_scan_info.pk_start_flg;
            prev_num = &walkie_scan_info.prev_pack_num;
            rev_addr = walkie_scan_info.rev_device;
        }
    }
    else
    {
        for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
        {
            if(walkie_per_info.device_info[i].actv_idx == actv_idx)
            {
                start_flg = &walkie_per_info.device_info[i].pk_start_flg;
                prev_num = &walkie_per_info.device_info[i].prev_pack_num;
                break;
            }
        }
    }

    // Receive packets must start with start packets
    if((*start_flg) || ((!*start_flg) && ((header->pacet_type == WALKIE_GAP_DATA_START)||(header->pacet_type == WALKIE_GAP_DATA_AUDIO))))
    {
        // Half-duplex mode only accepts data from one device,
        // preventing data from other devices from being inserted
        if(repot_type == WALKIE_GAP_EXT_ADV)
        {
            if(!ADDR_IS_EMPTY(rev_addr) && memcmp(addr, rev_addr, GAP_BD_ADDR_LEN))
            {
                return false;
            }
        }

        // filter duplicate packets
        if(*prev_num != header->pacet_num)
        {
            *prev_num = header->pacet_num;
            if(header->pacet_type == WALKIE_GAP_DATA_START)
            {
                // Receive start packet to save parameters
                *start_flg = 1;
                if(repot_type == WALKIE_GAP_EXT_ADV)
                {
                    memcpy(rev_addr, addr, GAP_BD_ADDR_LEN);
                }
            }
            else if(header->pacet_type == WALKIE_GAP_DATA_END)
            {
                // Receive the end packet to clear the parameters
                *start_flg = 0;
                *prev_num = 0;
                if(repot_type == WALKIE_GAP_EXT_ADV)
                {
                    memset(rev_addr, 0, GAP_BD_ADDR_LEN);
                }
            }
            return true;
       }
    }

    return false;
}


static int walkie_gapm_adv_report_evt_handler(ke_msg_id_t const msgid,
                                     struct gapm_ext_adv_report_ind *param,
                                     ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    bool report_data_flag = false;
    walkie_data_header_t *data_header = (walkie_data_header_t *)param->data;

    if(!walkie_event_cb)
    {
        return KE_MSG_CONSUMED;
    }

    if((data_header->data_type != GAP_AD_TYPE_WALKIE)
        || (data_header->pacet_type == WALKIE_GAP_DATA_INVALIE))
    {
        return KE_MSG_CONSUMED;
    }

    if(param->actv_idx == walkie_scan_info.actv_idx)
    {
        if (data_header->pacet_type == WALKIE_GAP_DATA_DEV_INFO){
            report_data_flag = true;
        }
        else
        {
            report_data_flag = walkie_gapm_adv_report_filter(WALKIE_GAP_EXT_ADV, param->actv_idx,
                                                                param->trans_addr.addr, data_header);
        }
    }
    else
    {
        report_data_flag = walkie_gapm_adv_report_filter(WALKIE_GAP_PER_ADV, param->actv_idx,
                                                            param->trans_addr.addr, data_header);
    }

    if(report_data_flag)
    {
        walkie_gap_data_t audio_data = {0};

        audio_data.actv_idx = param->actv_idx;
        audio_data.pacet_num = data_header->pacet_num;
        audio_data.data_len  = param->length - sizeof(walkie_data_header_t);
        audio_data.data      = param->data + sizeof(walkie_data_header_t);

        if (data_header->pacet_type == WALKIE_GAP_DATA_DEV_INFO)
        {
            audio_data.addr = param->trans_addr.addr;
            walkie_event_cb(WALKIE_GAP_FIND_DEV, &audio_data);
        }
        else if(data_header->pacet_type == WALKIE_GAP_DATA_START)
        {
            walkie_event_cb(WALKIE_GAP_START_DATA, &audio_data.actv_idx);
        }
        else if(data_header->pacet_type == WALKIE_GAP_DATA_END)
        {
            walkie_event_cb(WALKIE_GAP_END_DATA, &audio_data.actv_idx);
        }
        else
        {
            walkie_event_cb(WALKIE_GAP_AUDIO_DATA, &audio_data);
        }
    }
    return KE_MSG_CONSUMED;
}

static void walkie_gapm_sync_established_evt_handler(ke_msg_id_t const msgid,
                                  struct gapm_sync_established_ind *param,
                                  ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    TRACE(0, "[%s][%d]%s: sync device, actv_idx=%d", __FUNCTION__, __LINE__, WALKIE_LOG_T, param->actv_idx);
    DUMP8("%02x ", &param->addr.addr, GAP_BD_ADDR_LEN);
    walkie_event_cb(WALKIE_GAP_PER_SYNC_EST, &param->actv_idx);
}

#ifdef BLE_ISO_ENABLED
static int walkie_gapm_big_info_adv_evt_report_handler(ke_msg_id_t const msg_id,
                                                struct gapm_big_info_adv_report_ind *p_evt,
                                                ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    if(!walkie_event_cb)
    {
        return KE_MSG_CONSUMED;
    }

    walkie_gap_big_info_t big_info;

    //
    big_info.big_hdl = walkie_big_info.bis_receiver[p_evt->actv_idx - WALKIE_DEVICE_ID_BASE].big_hdl;
    big_info.actv_idx = p_evt->actv_idx;
    big_info.sync_hdl = gapm_per_sync_hdl_get(p_evt->actv_idx);
    big_info.sdu_interval = p_evt->report.sdu_interval;
    big_info.iso_interval = p_evt->report.iso_interval;
    big_info.max_pdu = p_evt->report.max_pdu;
    big_info.max_sdu = p_evt->report.max_sdu;
    big_info.num_bis = p_evt->report.num_bis;
    big_info.nse = p_evt->report.nse;
    big_info.bn = p_evt->report.bn;
    big_info.pto = p_evt->report.pto;
    big_info.irc = p_evt->report.irc;
    big_info.phy = p_evt->report.phy;
    big_info.framing = p_evt->report.framing;
    big_info.encrypted = p_evt->report.encrypted;

    walkie_event_cb(WALKIE_GAP_BIG_INFO_REPORT,(void*)&big_info);

    return (KE_MSG_CONSUMED);
}
#endif

static int walkie_appm_msg_handler(ke_msg_id_t const msgid,
                          void *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

KE_MSG_HANDLER_TAB(ble_walkie)
{
    /// please add in order of gapm_msg size
    // GAPM messages
    {GAPM_CMP_EVT,                 (ke_msg_func_t)walkie_gapm_cmp_evt_handler},
    {GAPM_ACTIVITY_CREATED_IND,    (ke_msg_func_t)walkie_gapm_activity_created_ind_handler},
    {GAPM_ACTIVITY_STOPPED_IND,    (ke_msg_func_t)walkie_gapm_activity_stopped_ind_handler},
    {GAPM_EXT_ADV_REPORT_IND,      (ke_msg_func_t)walkie_gapm_adv_report_evt_handler},
    {GAPM_SYNC_ESTABLISHED_IND,    (ke_msg_func_t)walkie_gapm_sync_established_evt_handler},
    #ifdef BLE_ISO_ENABLED
    {GAPM_BIG_INFO_ADV_REPORT_IND, (ke_msg_func_t)walkie_gapm_big_info_adv_evt_report_handler},
    #endif
    {KE_MSG_DEFAULT_HANDLER,       (ke_msg_func_t)walkie_appm_msg_handler},
};

// Application task descriptor
static ke_state_t ble_walkie_state;
const struct ke_task_desc TASK_BLE_WALKIE_APP = {ble_walkie_msg_handler_tab, &ble_walkie_state, 1, ARRAY_LEN(ble_walkie_msg_handler_tab)};

void walkie_ble_gap_init(WALKIE_ADV_TYPE_E adv_type, walkie_ble_gap_event_cb event_cb)
{
    TRACE(3, "%s:%s:adv type = %d, start", WALKIE_LOG_T,__func__,adv_type);
    walkie_event_cb = event_cb;

    if (ke_task_check(TASK_BLE_WALKIE) == TASK_NONE) {//task type exist
        // Create APP task
        ke_task_create(TASK_BLE_WALKIE, &TASK_BLE_WALKIE_APP);
        // Initialize Task state
        ke_state_set(TASK_BLE_WALKIE, 0);
    }

    walkie_scan_creat(GAPM_STATIC_ADDR);

    if(adv_type == WALKIE_GAP_PER_ADV)
    {
        walkie_adv_info.para.type = GAPM_ADV_TYPE_PERIODIC;
        walkie_adv_info.para.adv_param.prim_cfg.adv_intv_min = WALKIE_PER_EXT_T;
        walkie_adv_info.para.adv_param.prim_cfg.adv_intv_max = WALKIE_PER_EXT_T;
    }
    else
    {
        walkie_adv_info.para.type = GAPM_ADV_TYPE_EXTENDED;
        walkie_adv_info.para.adv_param.prim_cfg.adv_intv_min = WALKIE_EXT_T;
        walkie_adv_info.para.adv_param.prim_cfg.adv_intv_max = WALKIE_EXT_T;
    }
    walkie_adv_creat(&walkie_adv_info.para);
    TRACE(0, "%s:%s,para.type = %d, end!", WALKIE_LOG_T, __FUNCTION__, walkie_adv_info.para.type);
}

void walkie_ble_gap_deinit(void)
{
    TRACE(0, "[%s]%s: start", WALKIE_LOG_T, __FUNCTION__);

    walkie_adv_disenable();
    walkie_adv_delete();
    osDelay(10);
    walkie_scan_disenable(walkie_scan_info.actv_idx);
    walkie_scan_delete();

    walkie_scan_info.actv_enable = false;
    walkie_adv_info.actv_enable = false;
    TRACE(0, "%s:%s: end!", WALKIE_LOG_T,__FUNCTION__ );
}

void walkie_adv_start(uint8_t *def_dev_info_data, uint8_t data_len)
{
    if (walkie_adv_info.actv_idx == 0xFF)
    {
        TRACE(0, "%s:%s: adv actv_idx invalid!!!", WALKIE_LOG_T,__FUNCTION__);
        return;
    }

    walkie_adv_info.duration = 0;
    if (walkie_adv_info.para.type == GAPM_ADV_TYPE_PERIODIC)
    {
        walkie_set_adv_data(GAPM_SET_ADV_DATA, WALKIE_GAP_DATA_DEV_INFO, def_dev_info_data, data_len);
    }
    else
    {
        walkie_set_adv_data(GAPM_SET_ADV_DATA, WALKIE_GAP_DATA_START, NULL, 0);
    }
}

void walkie_adv_stop(void)
{
    if (walkie_adv_info.actv_idx == 0xFF)
    {
        TRACE(0, "%s:%s: adv actv_idx invalid!!!", WALKIE_LOG_T, __FUNCTION__);
        return;
    }

    TRACE(0, "%s:%s",WALKIE_LOG_T, __FUNCTION__ );
    if (walkie_adv_info.actv_enable == true)
    {
        if (walkie_adv_info.para.type != GAPM_ADV_TYPE_PERIODIC)
        {
            walkie_adv_info.duration = 50;
        }
        walkie_adv_disenable();
    }
}

void walkie_send_data(WALKIE_DATA_TYPE_E packet_type, uint8_t *p_data, uint8_t data_len)
{

    uint8_t adv_type = GAPM_SET_ADV_DATA;

    if(walkie_adv_info.actv_idx == 0xFF)
    {
        TRACE(0, "%s:%s,adv actv_idx invalid!!!", WALKIE_LOG_T,__FUNCTION__);
        return;
    }

    if (walkie_adv_info.actv_enable == false)
    {
        TRACE(0, "%s:%s,adv is disenable, not send data!!!",WALKIE_LOG_T,__FUNCTION__);
        return;
    }
#ifdef BLE_ISO_ENABLED
    if (WALKIE_GAP_BOARDCAST ==  walkie_get_audio_streaming_adv_type()
        && (packet_type == WALKIE_GAP_DATA_AUDIO))
    {
        walkie_bc_src_send_data(p_data, data_len);
        return;
    }
#endif
    if ((walkie_adv_info.para.type == GAPM_ADV_TYPE_PERIODIC) && (packet_type != WALKIE_GAP_DATA_DEV_INFO))
    {
        adv_type = GAPM_SET_PERIOD_ADV_DATA;
    }
    walkie_set_adv_data(adv_type, packet_type, p_data, data_len);

    TRACE(0, "%s: %s,send data type %d ok", WALKIE_LOG_T,__FUNCTION__, packet_type);
}

void walkie_scan_start(WALKIE_BLE_GAP_SCAN_TYP_E scan_type)
{
    walkie_scan_para_t* p_wt_scan_info = NULL;

    if (WALKIE_SCAN_PASSIVE == scan_type)
    {
        p_wt_scan_info = &walkie_scan_info;
    }

    if(p_wt_scan_info->actv_idx == 0xFF)
    {
        TRACE(0, "%s:%s, scan actv_idx invalid!!!", WALKIE_LOG_T,__FUNCTION__);
        return;
    }

    TRACE(0, "%s:%s,scan type = %d", WALKIE_LOG_T,__FUNCTION__, scan_type);
    if(p_wt_scan_info->actv_enable == false)
    {
        walkie_scan_enable(p_wt_scan_info->actv_idx,&p_wt_scan_info->para);
    }
}

void walkie_scan_stop(WALKIE_BLE_GAP_SCAN_TYP_E scan_type)
{
    walkie_scan_para_t* p_wt_scan_param = NULL;

    if (scan_type == WALKIE_SCAN_PASSIVE)
    {
        p_wt_scan_param = &walkie_scan_info;
    }

    if (p_wt_scan_param->actv_idx == 0xFF)
    {
        TRACE(0, "[%s]%s: scan actv_idx invalid!!!", WALKIE_LOG_T,__FUNCTION__);
        return;
    }

    TRACE(0, "%s:%s,scan type = %d", WALKIE_LOG_T,__FUNCTION__, scan_type);
    if(p_wt_scan_param->actv_enable == true)
    {
        walkie_scan_disenable(p_wt_scan_param->actv_idx);
    }
}

bool walkie_per_sync_start(const uint8_t *addr)
{
    TRACE(0, "%s:per_sync_start", WALKIE_LOG_T);
    for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
    {
        if(walkie_per_info.device_info[i].actv_idx == 0)
        {
            memcpy(walkie_per_info.device_info[i].addr, addr, GAP_BD_ADDR_LEN);
            break;
        }
    }

    return walkie_periodic_sync_create(GAPM_STATIC_ADDR);
}

void walkie_per_sync_stop(uint8_t actv_idx)
{
    walkie_periodic_sync_disenable(actv_idx);
}

uint8_t *walkie_get_trans_device_addr(uint8_t actv_idx)
{
    if(walkie_adv_info.para.type == GAPM_ADV_TYPE_PERIODIC)
    {
        for(int i = 0; i < WALKIE_DEVICE_MAX; i++)
        {
            if(walkie_per_info.device_info[i].actv_idx == actv_idx)
            {
                return walkie_per_info.device_info[i].addr;
            }
        }
    }
    else
    {
        return walkie_scan_info.rev_device;
    }

    return  NULL;
}

int walkie_add_device_to_list(const walkie_bdaddr_t *bdaddr, uint8_t size)
{
    if(!bdaddr || !size)
    {
         TRACE(0, "%s: %s,bdaddr=%p, size=%d!",WALKIE_LOG_T, __FUNCTION__, bdaddr, size);
        return -1;
    }

    if((walkie_addr.addr_num + size) > WALKIE_DEVICE_MAX)
    {
        TRACE(0, "[%s][ERROR]%s: list is full, max_num=%d, addr_num=%d, size=%d",
                            __FUNCTION__, WALKIE_LOG_T, WALKIE_DEVICE_MAX, walkie_addr.addr_num, size);
        return -1;
    }

    if(walkie_scan_info.actv_enable || walkie_adv_info.actv_enable)
    {
        TRACE(0, "[%s][ERROR]%s: act is enable not set addr list, scan_en=%d, adv_en=%d", __FUNCTION__, WALKIE_LOG_T,
                      walkie_adv_info.actv_enable, walkie_scan_info.actv_enable);
        return -1;
    }

    memcpy(&walkie_addr.addr_list[walkie_addr.addr_num], bdaddr, sizeof(gap_bdaddr_t)*size);
    walkie_addr.addr_num += size;

    //set white list
    walkie_set_device_list(GAPM_SET_WL, &walkie_addr.addr_list[0], walkie_addr.addr_num);

    TRACE(0, "%s: %s num=%d!", WALKIE_LOG_T,__FUNCTION__, walkie_addr.addr_num);
    return 0;
}

void walkie_clear_device_list(void)
{
    memset(&walkie_addr, 0, sizeof(walkie_addr));
    walkie_set_device_list(GAPM_SET_WL, &walkie_addr.addr_list[0], walkie_addr.addr_num);
    TRACE(0, "[%s][%d]%s: num=%d!", __FUNCTION__, __LINE__, WALKIE_LOG_T, walkie_addr.addr_num);
}

// BIS

#ifdef BLE_ISO_ENABLED
static inline void walkie_update_bis_receiver_con_hdl(uint8_t actv_idx,uint16_t con_hdl)
{
    walkie_big_info.bis_receiver[actv_idx - 2].bis_con_hdl = con_hdl;
}

static inline uint16_t walkie_get_bis_receiver_con_hdl(uint8_t actv_idx)
{
    return walkie_big_info.bis_receiver[actv_idx - 2].bis_con_hdl;
}

static uint8_t walkie_get_actv_idx_by_con_hdl(uint16_t conhdl)
{
    for (uint8_t idx = 0; idx < WALKIE_DEVICE_MAX - 1; idx++)
    {
        if (walkie_big_info.bis_receiver[idx].bis_con_hdl == conhdl)
        {
            return walkie_big_info.bis_receiver[idx].actv_idx;
        }
    }

    return 0xFF;
}

static void walkie_bis_recv_iso_data(uint16_t conhdl, uint8_t pkt_status)
{
    gaf_media_data_t *p_decoder_frame = NULL;
    POSSIBLY_UNUSED walkie_gap_data_t audio_data = {0};

    while ((p_decoder_frame = (gaf_media_data_t *)walkie_iso_dp_itf_get_rx_data(conhdl, NULL)))
    {
        audio_data.actv_idx = walkie_get_actv_idx_by_con_hdl(conhdl);
        ASSERT(audio_data.actv_idx != 0xFF,"actv idx error.")
        audio_data.pacet_num = p_decoder_frame->pkt_seq_nb;
        audio_data.data_len = p_decoder_frame->data_len;
        audio_data.data      = p_decoder_frame->sdu_data;
        // TRACE(4,"W-T-GAP:iso data con_hdl=0x%x,seq = %u,len = %u,status = %d",
        //        conhdl,p_sdu_buf.pkt_seq_nb, p_sdu_buf.sdu_length,p_sdu_buf.status);
        if (0 == p_decoder_frame->pkt_status)
        {
            walkie_event_cb(WALKIE_GAP_AUDIO_DATA, &audio_data);
        }

        gaf_stream_data_free(p_decoder_frame);
    }
}

void walkie_bis_register_recv_iso_data_callback()
{
    isoohci_data_come_callback_register((void*)walkie_bis_recv_iso_data);
}

void walkie_bis_unregister_recv_iso_data_callback()
{
    isoohci_data_come_callback_register(NULL);
}

uint8_t walkie_bis_rx_stream_start(uint8_t actv_idx)
{
    uint16_t con_hdl = walkie_get_bis_receiver_con_hdl(actv_idx);

    return walkie_iso_sink_rx_stream_start(con_hdl, WAKIE_FRAME_OCTET);
}

uint8_t walkie_bis_rx_stream_start_by_con_hdl(uint16_t con_hdl)
{
    return walkie_iso_sink_rx_stream_start(con_hdl, WAKIE_FRAME_OCTET);
}

void walkie_bis_rx_stream_stop(uint8_t actv_idx)
{
    uint16_t con_hdl = walkie_get_bis_receiver_con_hdl(actv_idx);
    walkie_iso_sink_rx_stream_stop(con_hdl);
}

void walkie_send_iso_dp_remove_cmd(uint8_t conhdl,uint8_t direction_bf)
{
    // Allocate HCI command message
    struct hci_le_remove_iso_data_path_cmd* p_cmd =
            HL_HCI_CMD_ALLOC(HCI_LE_REMOVE_ISO_DATA_PATH_CMD_OPCODE, hci_le_remove_iso_data_path_cmd);

    // Fill command parameters
    p_cmd->data_path_direction = direction_bf;
    p_cmd->conhdl = conhdl;

    // Configure environment for reception of command complete event
    HL_HCI_CMD_SEND_TO_CTRL(p_cmd, direction_bf ?
        WALKIE_BIG_RMV_RX_DP : WALKIE_BIG_RMV_TX_DP, walkie_talkie_big_hci_cmd_cmp_handler);
}

static void walkie_big_hci_cmd_setup_iso_data(struct hci_le_setup_iso_data_path_cmd *para)
{
    struct hci_le_setup_iso_data_path_cmd* p_cmd =
        HL_HCI_CMD_ALLOC(HCI_LE_SETUP_ISO_DATA_PATH_CMD_OPCODE,hci_le_setup_iso_data_path_cmd);

    if (para == NULL)
    {
        TRACE(0, "%s: setup_iso_data is NULL!", __func__);
        return;
    }
    memcpy(p_cmd, para, sizeof(struct hci_le_setup_iso_data_path_cmd));
    // Send the HCI command
    HL_HCI_CMD_SEND_TO_CTRL(p_cmd, p_cmd->data_path_direction ? 
        WALKIE_BIG_RX_DATA_SETUP_CMP : WALKIE_BIG_TX_DATA_SETUP_CMP, walkie_talkie_big_hci_cmd_cmp_handler);
}

static void walkie_bc_sink_start_setup_data_path(uint16_t conhdl)
{
    struct hci_le_setup_iso_data_path_cmd big_data;

    big_data.conhdl = conhdl;
    big_data.data_path_direction = 0x01;
    big_data.data_path_id = 0;
    big_data.ctrl_delay[0] = 6;
    big_data.codec_cfg_len = 0;

    walkie_big_hci_cmd_setup_iso_data(&big_data);
}

static void walkie_talkie_big_hci_cmd_cmp_handler(uint16_t opcode, uint16_t event, void const *p_evt)
{
    switch(event)
    {
        case WALKIE_BIG_CREAT_CMP:
        {
            struct hci_basic_cmd_cmp_evt * bc_hci_cmp_evt = (struct hci_basic_cmd_cmp_evt *)p_evt;

            if(bc_hci_cmp_evt->status == CO_ERROR_NO_ERROR)
            {
                TRACE(0, "%s:create big cmd OK!", WALKIE_LOG_T);
            }
            else
            {
                TRACE(0, "%s:create big cmd fail,error code = %d!", WALKIE_LOG_T,bc_hci_cmp_evt->status);
            }
        }
            break;
        case WALKIE_BIG_RX_DATA_SETUP_CMP:
        {
            struct hci_le_setup_iso_data_path_cmd_cmp_evt *iso_dp_cmp_evt =
                            (struct hci_le_setup_iso_data_path_cmd_cmp_evt*)p_evt;
            TRACE(2, "%s:set rx iso data path status = %d,bis_hdl = 0x%x!",
                WALKIE_LOG_T,iso_dp_cmp_evt->status,iso_dp_cmp_evt->conhdl);
            if (iso_dp_cmp_evt->status == CO_ERROR_NO_ERROR)
            {
                 walkie_event_cb(WALKIE_BG_RX_DP_SETUP, (void*)&iso_dp_cmp_evt->conhdl);
            }
         }
            break;
        case WALKIE_BIG_TX_DATA_SETUP_CMP:
        {
            struct hci_le_setup_iso_data_path_cmd_cmp_evt *iso_dp_cmp_evt
                    = (struct hci_le_setup_iso_data_path_cmd_cmp_evt*)p_evt;
            TRACE(2, "%s:set tx iso data path status = %d,bis_hdl = 0x%x!",
                WALKIE_LOG_T,iso_dp_cmp_evt->status,iso_dp_cmp_evt->conhdl);
            if (iso_dp_cmp_evt->status == CO_ERROR_NO_ERROR)
            {
                 walkie_iso_src_start_streaming(iso_dp_cmp_evt->conhdl,
                    WALKIE_BC_SDU_INTERVAL_US, WALKIE_BC_MAX_TRANS_LATENCY_MS, WALKIE_BC_MAX_SDU_SIZE);
            }
         }
            break;
        case WALKIE_BIG_SYNC_CMP:
            break;
        case WALKIE_BIG_RMV_TX_DP:
            TRACE(1, "%s:remove tx iso data path", WALKIE_LOG_T);
            break;
        case WALKIE_BIG_RMV_RX_DP:
            TRACE(1, "%s:remove rx iso data path", WALKIE_LOG_T);
            break;
        default:
            break;
    }
}

void walkie_big_sync_estab_evt_handler(uint8_t evt_code, struct hci_le_big_sync_est_evt const* p_evt)
{
    uint8_t actv_idx;
    TRACE(3,"%s:%s big_hdl = 0x%x,con_hdl=0x%x",WALKIE_LOG_T,__func__,p_evt->big_hdl,p_evt->conhdl[0]);

    if (p_evt->status == CO_ERROR_NO_ERROR)
    {
        actv_idx = BIG_HDL_TO_ACTV_IDX(p_evt->big_hdl);
        TRACE(5,"actv_idx = %d,bn = %d,irc = %d,max_pdu =%d,nse=%d",
                actv_idx,p_evt->bn,p_evt->irc,p_evt->max_pdu,p_evt->nse);
        TRACE(3,"pto = %d,trans_latency = %d,iso_itv= %d",
                p_evt->pto,p_evt->big_trans_latency,p_evt->iso_interval);

        walkie_update_bis_receiver_con_hdl(actv_idx,p_evt->conhdl[0]);
        walkie_event_cb(WALKIE_BG_SYNC_STATUS_ESTABLISHED, (void*)&actv_idx);
        walkie_bc_sink_start_setup_data_path(p_evt->conhdl[0]);
    }
    else
    {
        if (p_evt->status == CO_ERROR_OPERATION_CANCELED_BY_HOST)
        {
            walkie_event_cb(WALKIE_BG_SYNC_STATUS_CANCELLED, (void*)&p_evt->big_hdl);
        }
        else
        {
            walkie_event_cb(WALKIE_BG_SYNC_STATUS_FAILED, (void*)&p_evt->big_hdl);
        }
    }
}

void walkie_big_sync_lost_evt_handler(uint8_t evt_code, struct hci_le_big_sync_lost_evt const* p_evt)
{
    uint8_t actv_idx = BIG_HDL_TO_ACTV_IDX(p_evt->big_hdl);
    TRACE(3,"%s:big_sync_lost actv_idx = %d,big_hdl = 0x%x",WALKIE_LOG_T,actv_idx,p_evt->big_hdl);

     walkie_event_cb(WALKIE_BG_SYNC_STATUS_LOST, (void*)&actv_idx);
}

void walkie_talkie_print_big_info(const walkie_gap_big_info_t* p_big_info)
{
    TRACE(0,"W-T-GAP:Big Info:");
    TRACE(2,"big_hdl=0x%x,sync_hdl=0x%x",p_big_info->big_hdl,p_big_info->sync_hdl);
    TRACE(4,"sdu_interval= %u,iso_interval=%u,max_sdu=%u,num_bis=%u",
        p_big_info->sdu_interval,p_big_info->iso_interval,p_big_info->max_sdu,p_big_info->num_bis);
    TRACE(4,"nse= %u,bn=%u,pto=%u,irc=%u",p_big_info->nse,p_big_info->bn,p_big_info->pto,p_big_info->irc);
    TRACE(4,"phy= %u,framing=%u,encrypted=%u",p_big_info->phy,p_big_info->framing,p_big_info->encrypted);
}

void walkie_big_hci_create_cmp_evt_handler(uint8_t evt_code, struct hci_le_create_big_cmp_evt const* p_evt)
{
    if(p_evt->status == CO_ERROR_NO_ERROR)
    {
        memcpy(&walkie_big_info.big_creat_cmp, p_evt, sizeof(struct hci_le_create_big_cmp_evt const));
        TRACE(1,"%s:big_hci_create_cmp bis con_hdl = 0x%x",WALKIE_LOG_T,p_evt->conhdl[0]);
        struct hci_le_setup_iso_data_path_cmd big_data;
        big_data.conhdl = p_evt->conhdl[0];
        big_data.data_path_direction = 0;
        big_data.data_path_id = 0;
        big_data.ctrl_delay[0] = 6;
        big_data.codec_cfg_len = 0;
        walkie_big_hci_cmd_setup_iso_data(&big_data);
    }
    else
    {
        TRACE(0, "[%s][%d][ERROR]: BIG creat fail!", __FUNCTION__, __LINE__);
    }
}

bool walkie_send_big_create_sync_hci_cmd(const walkie_gap_big_info_t* p_big_info)
{
    TRACE(0, "%s:create big sync_start", WALKIE_LOG_T);

    uint8_t p_broadcast_code[IAP_BROADCAST_CODE_LEN] = {0x11,0x22,};

    // Allocate HCI command message
    struct hci_le_big_create_sync_cmd* p_cmd =
            HL_HCI_CMD_ALLOC(HCI_LE_BIG_CREATE_SYNC_CMD_OPCODE,hci_le_big_create_sync_cmd);

    // Fill command parameters
    p_cmd->big_hdl = p_big_info->big_hdl;
    p_cmd->big_sync_timeout = 100;
    p_cmd->mse = p_big_info->nse;
    p_cmd->sync_hdl = p_big_info->sync_hdl;
    p_cmd->num_bis = p_big_info->num_bis;
    p_cmd->bis_id[0] = 0x01;
    p_cmd->encryption = p_big_info->encrypted;

    if (p_cmd->encryption)
    {
        memcpy(&p_cmd->broadcast_code[0], p_broadcast_code, IAP_BROADCAST_CODE_LEN);
    }

    // Send the HCI command
    HL_HCI_CMD_SEND_TO_CTRL(p_cmd, WALKIE_BIG_SYNC_CMP, walkie_talkie_big_hci_cmd_cmp_handler);

    return true;
}

// src
static void walkie_send_create_big_hci_cmd(struct hci_le_create_big_cmd *para)
{
    struct hci_le_create_big_cmd* p_big_cmd = HL_HCI_CMD_ALLOC(HCI_LE_CREATE_BIG_CMD_OPCODE, hci_le_create_big_cmd);

    if (para)
    {
        memcpy(p_big_cmd, para, sizeof(struct hci_le_create_big_cmd));
    }
    else
    {
        TRACE(0, "[%s][%d]: create big parmater is null!", WALKIE_LOG_T, __LINE__);
        return;
    }

    // Send the HCI command
    HL_HCI_CMD_SEND_TO_CTRL(p_big_cmd, WALKIE_BIG_CREAT_CMP, walkie_talkie_big_hci_cmd_cmp_handler);
}

void walkie_talkie_big_src_create()
{
    walkie_big_info.big_para.adv_hdl = walkie_adv_info.actv_idx;
    walkie_big_info.big_para.big_hdl = 0;
    walkie_send_create_big_hci_cmd(&walkie_big_info.big_para);
}

void walkie_talkie_bis_sync_stop(uint8_t actv_idx)
{
    TRACE(0, "%s:bis_sync_stop",WALKIE_LOG_T);
}

bool walkie_bc_src_start_streaming()
{
    return walkie_iso_src_start_streaming(walkie_big_info.big_creat_cmp.conhdl[0],
            WALKIE_BC_SDU_INTERVAL_US, WALKIE_BC_MAX_TRANS_LATENCY_MS, WALKIE_BC_MAX_SDU_SIZE);
}

void walkie_bc_src_stop_streaming()
{
    iso_data_seq = 0;
    walkie_iso_src_stop_streaming(walkie_big_info.big_creat_cmp.conhdl[0]);
}

void walkie_bc_src_send_data(uint8_t *payload, uint16_t payload_len)
{
    walkie_iso_src_send_data(walkie_big_info.big_creat_cmp.conhdl[0],iso_data_seq,payload,payload_len,0);
    iso_data_seq++;
}

#endif  //BLE_ISO_ENABLED


