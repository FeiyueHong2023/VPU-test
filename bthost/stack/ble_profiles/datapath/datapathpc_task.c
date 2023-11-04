/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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

/**
 ****************************************************************************************
 * @addtogroup DATAPATHPCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#include "gapc_msg.h"
#if (BLE_DATAPATH_CLIENT)
#include <string.h>
#include "gap.h"
#include "datapathpc.h"
#include "datapathpc_task.h"

#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
static int gapc_disconnect_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    return KE_MSG_CONSUMED;
}

static void send_write_request_or_write_command(uint8_t conidx, const uint8_t* ptrData, uint32_t length, bool isWithRsp)
{
    co_buf_t* p_buf = NULL;
    prf_buf_alloc(&p_buf, length);
    uint8_t* p_data = co_buf_data(p_buf);
    memcpy(p_data, ptrData, length);
    datapathpc_env_t *datapathpc_env = PRF_ENV_GET(DATAPATHPC, datapathpc);
    enum gatt_write_type writeType;

    if (isWithRsp)
    {
        writeType = GATT_WRITE;
    }
    else
    {
        writeType = GATT_WRITE_NO_RESP;
    }

    // Dummy parameter provided to GATT
    uint16_t dummy = 0;
    uint16_t ret = gatt_cli_write(conidx, datapathpc_env->user_lid, dummy, writeType,
                                 datapathpc_env->p_env[conidx]->datapath.svc.shdl + DATAPATHPC_IDX_RX_VAL, 0, p_buf);

    TRACE(0, "%s conidx 0x%x ret %d", __func__, conidx, ret);
    // Release the buffer
    co_buf_release(p_buf);

}

__STATIC int send_data_via_write_command_handler(ke_msg_id_t const msgid,
                                      struct ble_datapath_send_data_req_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    send_write_request_or_write_command(param->connecionIndex, param->value, param->length, false);
    return (KE_MSG_CONSUMED);
}

__STATIC int send_data_via_write_request_handler(ke_msg_id_t const msgid,
                                      struct ble_datapath_send_data_req_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    send_write_request_or_write_command(param->connecionIndex, param->value, param->length, true);
    return (KE_MSG_CONSUMED);
}

static void send_notificatons_enable(uint8_t conidx, const uint8_t* ptrData, uint32_t length)
{

  co_buf_t* p_buf = NULL;
  prf_buf_alloc(&p_buf, length);
  uint8_t* p_data = co_buf_data(p_buf);
  memcpy(p_data, ptrData, length);
  TRACE(0, "%s conidx 0x%x length %d", __func__, conidx, length);
  datapathpc_env_t *datapathpc_env = PRF_ENV_GET(DATAPATHPC, datapathpc);
  enum gatt_write_type writeType = GATT_WRITE;
  // Dummy parameter provided to GATT
  uint16_t dummy = 0;
  uint16_t ret = gatt_cli_write(conidx, datapathpc_env->user_lid, dummy, writeType,
                                datapathpc_env->p_env[conidx]->datapath.svc.shdl + DATAPATHPC_IDX_TX_NTF_CFG, 0, p_buf);
  TRACE(0, "%s conidx 0x%x ret %d", __func__, conidx, ret);
  // Release the buffer
  co_buf_release(p_buf);

}

__STATIC void datapath_user_register(uint8_t conidx)
{
    datapathpc_env_t *datapathpc_env = PRF_ENV_GET(DATAPATHPC, datapathpc); 
    TRACE(0,"%s  start shdl %d user_lid %d",__func__,datapathpc_env->p_env[conidx]->datapath.svc.shdl,datapathpc_env->user_lid);
    // Register for reception of events
    gatt_cli_event_register(conidx, datapathpc_env->user_lid, datapathpc_env->p_env[conidx]->datapath.svc.shdl,
        datapathpc_env->p_env[conidx]->datapath.svc.shdl + DATAPATHPC_IDX_RX_DESC);
}

static struct ble_datapath_control_notification_t ble_datapath_control_notification;
__STATIC int control_notification_handler(ke_msg_id_t const msgid,
                                    struct ble_datapath_control_notification_t const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    ble_datapath_control_notification.connecionIndex=param->connecionIndex;
    ble_datapath_control_notification.length=param->length;
    memcpy(ble_datapath_control_notification.value, param->value, param->length);
    return (KE_MSG_CONSUMED);
}

__STATIC int discover_handler(ke_msg_id_t const msgid,
                                    struct ble_datapath_discover_t const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{

    uint8_t conidx= param->connecionIndex;
    datapathpc_env_t *datapathpc_env = PRF_ENV_GET(DATAPATHPC, datapathpc);
    const datapathpc_content_t* p_datapath= &(datapathpc_env->p_env[conidx]->datapath);
    datapathpc_discover(param->connecionIndex, PRF_CON_DISCOVERY, p_datapath);

    return (KE_MSG_CONSUMED);
}

uint16_t datapathpc_discover(uint8_t conidx, uint8_t con_type, const datapathpc_content_t* p_datapath)
{
    uint16_t status = PRF_ERR_REQ_DISALLOWED;
    // Client environment
    datapathpc_env_t *datapathpc_env = PRF_ENV_GET(DATAPATHPC, datapathpc);
    if (datapathpc_env != NULL)
    {
       if ((conidx < BLE_CONNECTION_MAX)&&(datapathpc_env->p_env[conidx] != NULL))
        {
            datapathpc_env->p_env[conidx] = (struct datapathpc_cnx_env *) ke_malloc(sizeof(struct datapathpc_cnx_env), KE_MEM_PROFILE);
            if(datapathpc_env->p_env[conidx]!= NULL)
            {
                memset(datapathpc_env->p_env[conidx], 0, sizeof(struct datapathpc_cnx_env));
                // Config connection, start discovering
            if (con_type == PRF_CON_DISCOVERY)
                {
                // Service UUID
                TRACE(0,"datapathpc_discover");
                // Start discovery of Volume Control Service in peer device database
                status = gatt_cli_discover_svc(conidx, datapathpc_env->user_lid, 0,
                                                GATT_DISCOVER_SVC_PRIMARY_BY_UUID,
                                                true,
                                                GATT_MIN_HDL,
                                                GATT_MAX_HDL,
                                                GATT_UUID_128,
                                                DATAPATHPS_UUID_128);
                // Go to DISCOVERING state
                datapathpc_env->p_env[conidx]->discover  = true;
                }
                // normal connection, get saved att details
                else
                {
                    memcpy(&(datapathpc_env->p_env[conidx]->datapath), p_datapath, sizeof(datapathpc_content_t));
                    status = GAP_ERR_NO_ERROR;
                }
            }
            else
            {
                status = GAP_ERR_INSUFF_RESOURCES;
            }
        }
    }
    return (status);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(datapathpc)
{
    {GAPC_DISCONNECT_IND,                       (ke_msg_func_t) gapc_disconnect_ind_handler},
    {DATAPATHPC_SEND_DATA_VIA_WRITE_COMMAND,    (ke_msg_func_t) send_data_via_write_command_handler},
    {DATAPATHPC_SEND_DATA_VIA_WRITE_REQUEST,    (ke_msg_func_t) send_data_via_write_request_handler},
    {DATAPATHPC_CONTROL_NOTIFICATION,           (ke_msg_func_t) control_notification_handler},
    {DATAPATHPC_DISCOVER,                       (ke_msg_func_t) discover_handler},

};

void datapathpc_task_init(struct ke_task_desc *task_desc, void *p_env)
{
    // Get the address of the environment
    datapathpc_env_t *datapathpc_env = (datapathpc_env_t *)p_env;

    task_desc->msg_handler_tab = datapathpc_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(datapathpc_msg_handler_tab);
    task_desc->state           = &(datapathpc_env->state);
    task_desc->idx_max         = BLE_CONNECTION_MAX;
}
__STATIC void cb_write_cmp(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status)
{

}
__STATIC void cb_att_val_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t dummy,
                            uint16_t hdl, uint16_t offset, uint16_t max_length)
{

}

__STATIC void _cb_att_val_evt(uint8_t conidx, uint8_t user_lid, uint16_t token, uint8_t evt_type, bool complete,
                            uint16_t hdl, co_buf_t* p_data)
{
    TRACE(0,"%s evt_type:%d data_len %d", __func__, evt_type,p_data->data_len);
    ASSERT(complete, "%s %d, event not complete", __func__, __LINE__);
    DUMP8("%02x ",co_buf_data(p_data),p_data->data_len);
    /// inform APP layer that client received an notification
    struct ble_datapath_rx_data_ind_t *cmd =
        KE_MSG_ALLOC_DYN(DATAPATHPC_NOTIFICATION_RECEIVED,
                         KE_BUILD_ID(TASK_APP, conidx),
                         KE_BUILD_ID(prf_get_task_from_id(TASK_ID_DATAPATHPC), conidx),
                         ble_datapath_rx_data_ind_t,
                         p_data->data_len);

    cmd->length = p_data->data_len;
    memcpy(cmd->data, co_buf_data(p_data), p_data->data_len);
    ke_msg_send(cmd);
}
/**
 ****************************************************************************************
 * @brief Callback function called when a discovery procedure has been completed
 *
 * @param[in] con_lid       Connection local index
 * @param[in] user_lid      User local index
 * @param[in] dummy         Dummy value
 * @param[in] status        Procedure status
 ****************************************************************************************
 */
__STATIC void cb_datapathpc_discover_cmp(uint8_t con_lid, uint8_t user_lid, uint16_t dummy,
                                           uint16_t status)
{
    datapathpc_env_t *datapathpc_env = PRF_ENV_GET(DATAPATHPC, datapathpc);
    TRACE(0,"cb_datapathpc_discover_cmp  con_lid==%d",con_lid);
    // Pointer to Connection information structure
    datapathpc_cnx_env_t* p_env = datapathpc_env->p_env[con_lid];
    TRACE(0,"datapath_env...shld==%d",datapathpc_env->p_env[0]->datapath.svc.shdl);
    // Check that reception of complete event is expected
    if (p_env!=NULL)
    {
        
        if (status == GAP_ERR_NO_ERROR)
        {   
            const uint8_t* ptrData =(ble_datapath_control_notification.value);
            // Register for reception of events
            datapath_user_register(con_lid);
            send_notificatons_enable( ble_datapath_control_notification.connecionIndex, 
                                      ptrData,
                                      ble_datapath_control_notification.length);
        }
        else
        {
            // Free the allocated connection environment
            ke_free(p_env);
        }

        // Inform upper layer about command handling completion
    }
}
/**
 ****************************************************************************************
 * @brief Callback function called when content of a full service has been discovered in a peer device database
 *
 * @param[in] con_lid       Connection local index
 * @param[in] user_lid      User local index
 * @param[in] dummy         Dummy value
 * @param[in] hdl           First handle value of provided list of attributes
 * @param[in] disc_info     Discovery information
 * @param[in] nb_att        Number of attributes
 * @param[in] p_atts        Pointer to attribute information
 ****************************************************************************************
 */
__STATIC void cb_datapathpc_gatt_svc(uint8_t con_lid, uint8_t user_lid, uint16_t dummy, uint16_t hdl, uint8_t disc_info,
                                  uint8_t nb_att, const gatt_svc_att_t *p_atts)
{
    datapathpc_env_t *datapathpc_env = PRF_ENV_GET(DATAPATHPC, datapathpc);
    // Check that environment still exists for this connection
    datapathpc_cnx_env_t* p_env = datapathpc_env->p_env[con_lid];

    // Also check that reception of service information was expected
    if (p_env)
    {
        // Pointer to set of callback functions for communication with upper layer
        //const prf_hdr_t* p_cb = datapathpc_env->hdr;

    TRACE(0,"datapathpc_env...shld==%d",p_env->datapath.svc.shdl);
        if ((disc_info == GATT_SVC_CMPLT)
                        || (disc_info == GATT_SVC_START))
        {
            // Keep start handle
            p_env->datapath.svc.shdl = hdl;
        }
    TRACE(0,"datapathpc_env...shld==%d",p_env->datapath.svc.shdl);
   // TRACE(0,"p_vcc_env...shld==%d",p_vcc_env->p_con_info[0]->vcs_info.svc_info.shdl); 

        if ((disc_info == GATT_SVC_CMPLT)
                || (disc_info == GATT_SVC_END))
        {
            // Pointer to Service content description structure
            // Keep end handle
            p_env->datapath.svc.ehdl = hdl + nb_att - 1;
        }
    }
}


/// Set of callbacks functions for communication with GATT as a GATT User Client
__STATIC const gatt_cli_cb_t datapath_gatt_cb = {
    .cb_discover_cmp = cb_datapathpc_discover_cmp,
    .cb_write_cmp = cb_write_cmp,
    .cb_att_val_get = cb_att_val_get,
    .cb_att_val_evt = _cb_att_val_evt,
    .cb_svc = cb_datapathpc_gatt_svc,
    .cb_svc_info = NULL,
};

const gatt_cli_cb_t* datapathpc_task_get_cli_cbs(void)
{
    return (&datapath_gatt_cb);
}
#endif /* #if (BLE_DATAPATH_CLIENT) */

/// @} DATAPATHPCTASK
