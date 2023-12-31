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

/**
 ****************************************************************************************
 * @addtogroup AMSCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_AMS_CLIENT)
#include "ams_common.h"
#include "amsc.h"
#include "amsc_task.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "prf_dbg.h"

#ifdef BISTO_ENABLED
#include "amsp.h"
#include "amsp_task.h"
#endif

#define PRF_TAG         "[AMSC]"
#define HAVE_DEFERED_OP 1
#define NO_DEFERED_OP   0
#define SUBSCRIBE_AMS_SVC_CCCD_VAL      PRF_CLI_START_NTF
#define SUBSCRIBE_AMS_SVC_CCCD_LEN      2

/*
 * STRUCTURES
 ****************************************************************************************
 */
/// AMSC UUID
const uint8_t ams_svc_uuid[GATT_UUID_128_LEN] = {
    0xdc, 0xf8, 0x55, 0xad, 0x02, 0xc5, 0xf4, 0x8e,
    0x3a, 0x43, 0x36, 0x0f, 0x2b, 0x50, 0xd3, 0x89,
};

/// State machine used to retrieve AMS characteristics information
const prf_char128_def_t amsc_ams_char[AMSC_CHAR_MAX] = {
    /// Remote Command
    [AMSC_REMOTE_COMMAND_CHAR] = {
        {0xc2, 0x51, 0xca, 0xf7, 0x56, 0x0e, 0xdf, 0xb8,
         0x8a, 0x4a, 0xb1, 0x57, 0xd8, 0x81, 0x3c, 0x9b},
        PRF_ATT_REQ_PRES_MAND,
        PROP(WR) | PROP(N),
    },

    /// Entity Update
    [AMSC_ENTITY_UPDATE_CHAR] = {
        {0x02, 0xc1, 0x96, 0xba, 0x92, 0xbb, 0x0c, 0x9a,
         0x1f, 0x41, 0x8d, 0x80, 0xce, 0xab, 0x7c, 0x2f},
        PRF_ATT_REQ_PRES_OPT,
        PROP(WR) | PROP(N),
    },

    /// Entity Attribute
    [AMSC_ENTITY_ATTRIBUTE_CHAR] = {
        {0xd7, 0xd5, 0xbb, 0x70, 0xa8, 0xa3, 0xab, 0xa6,
         0xd8, 0x46, 0xab, 0x23, 0x8c, 0xf3, 0xb2, 0xc6},
        PRF_ATT_REQ_PRES_OPT,
        PROP(WR) | PROP(RD),
    },
};

/// State machine used to retrieve AMS characteristic descriptor information
const prf_desc_def_t amsc_ams_desc[AMSC_DESC_MAX] = {
    /// Remote Command Char. - Client Characteristic Configuration
    [AMSC_DESC_REMOTE_CMD_CL_CFG] = {
        GATT_DESC_CLIENT_CHAR_CFG,
        PRF_ATT_REQ_PRES_MAND,
        AMSC_REMOTE_COMMAND_CHAR,
    },
    /// Entity Update Char. - Client Characteristic Configuration
    [AMSC_DESC_ENTITY_UPDATE_CL_CFG] = {
        GATT_DESC_CLIENT_CHAR_CFG,
        PRF_ATT_REQ_PRES_OPT,
        AMSC_ENTITY_UPDATE_CHAR,
    },
};

/*
 * LOCAL FUNCTIONs DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Handles reception of the @ref AMSC_ENABLE_REQ message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int _enable_req_handler(ke_msg_id_t const msgid,
                               struct amsc_enable_req *param,
                               ke_task_id_t const dest_id,
                               ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref AMSC_READ_CMD message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int _read_cmd_handler(ke_msg_id_t const msgid,
                             ams_cli_rd_cmd_t *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref AMSC_WRITE_CMD message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int _write_cmd_handler(ke_msg_id_t const msgid,
                              gatt_cli_write_cmd_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id);

/**
 * @brief Handles reception of the @ref GATT_CMP_EVT message.
 * 
 * @param msgid         msgid Id of the message received.
 * @param param         param Pointer to the parameters of the message.
 * @param dest_id       dest_id ID of the receiving task instance.
 * @param src_id        src_id ID of the sending task instance.
 * @return int          If the message was consumed or not.
 */
static int _gatt_cmp_evt_handler(ke_msg_id_t const msgid,
                                 gatt_proc_cmp_evt_t *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id);

/**
 * @brief This function is called when GATT client user discovery procedure is over.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param status        Status of the procedure (@see enum hl_err)
 */
static void _cb_discover_cmp(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status);

/**
 * @brief This function is called when GATT client user read procedure is over.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param status        Status of the procedure (@see enum hl_err)
 */
static void _cb_read_cmp(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status);

/**
 * @brief This function is called when GATT client user write procedure is over.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param status        Status of the procedure (@see enum hl_err)
 */
static void _cb_write_cmp(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status);

/**
 * @brief This function is called when GATT client user has initiated a write procedure.
 *
 *        @see gatt_cli_att_val_get_cfm shall be called to provide attribute value.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param token         Procedure token that must be returned in confirmation function
 * @param dummy         Dummy parameter provided by upper layer for command execution - 0x0000 else.
 * @param hdl           Attribute handle
 * @param offset        Value offset
 * @param max_length    Maximum value length to return
 */
static void _cb_att_val_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t dummy,
                            uint16_t hdl, uint16_t offset, uint16_t max_length);

/**
 * @brief This function is called when a full service has been found during a discovery procedure.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param hdl           First handle value of following list
 * @param disc_info     Discovery information (@see enum gatt_svc_disc_info)
 * @param nb_att        Number of attributes
 * @param p_atts        Pointer to attribute information present in a service
 */
static void _cb_svc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t hdl, uint8_t disc_info,
                    uint8_t nb_att, const gatt_svc_att_t* p_atts);

/**
 * @brief This function is called when a service has been found during a discovery procedure.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param start_hdl     Service start handle
 * @param end_hdl       Service end handle
 * @param uuid_type     UUID Type (@see enum gatt_uuid_type)
 * @param p_uuid        Pointer to service UUID (LSB first)
 */
static void _cb_svc_info(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t start_hdl, uint16_t end_hdl,
                        uint8_t uuid_type, const uint8_t* p_uuid);

/**
 * @brief This function is called when an include service has been found during a discovery procedure.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param inc_svc_hdl   Include service attribute handle
 * @param start_hdl     Service start handle
 * @param end_hdl       Service end handle
 * @param uuid_type     UUID Type (@see enum gatt_uuid_type)
 * @param p_uuid        Pointer to service UUID (LSB first)
 */
static void _cb_inc_svc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t inc_svc_hdl,
                        uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_type, const uint8_t* p_uuid);

/**
 * @brief This function is called when a characteristic has been found during a discovery procedure.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param char_hdl      Characteristic attribute handle
 * @param val_hdl       Value handle
 * @param prop          Characteristic properties (@see enum gatt_att_info_bf - bits [0-7])
 * @param uuid_type     UUID Type (@see enum gatt_uuid_type)
 * @param p_uuid        Pointer to characteristic value UUID (LSB first)
 */
static void _cb_char(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t char_hdl, uint16_t val_hdl,
                    uint8_t prop, uint8_t uuid_type, const uint8_t* p_uuid);

/**
 * @brief This function is called when a descriptor has been found during a discovery procedure.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param desc_hdl      Characteristic descriptor attribute handle
 * @param uuid_type     UUID Type (@see enum gatt_uuid_type)
 * @param p_uuid        Pointer to attribute UUID (LSB first)
 */
static void _cb_desc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t desc_hdl,
                    uint8_t uuid_type, const uint8_t* p_uuid);

/**
 * @brief This function is called during a read procedure when attribute value is retrieved
 *        form peer device.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param dummy         Dummy parameter provided by upper layer for command execution
 * @param hdl           Attribute handle
 * @param offset        Value offset
 * @param p_data        Pointer to buffer that contains attribute value starting from offset
 */
static void _cb_att_val(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t hdl, uint16_t offset,
                        co_buf_t* p_data);

/**
 * @brief This function is called when a notification or an indication is received onto
 *        register handle range (@see gatt_cli_event_register).
 * 
 *        @see gatt_cli_val_event_cfm must be called to confirm event reception.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param token         Procedure token that must be returned in confirmation function
 * @param evt_type      Event type triggered (@see enum gatt_evt_type)
 * @param complete      True if event value if complete value has been received
 *                      False if data received is equals to maximum attribute protocol value.
 *                      In such case GATT Client User should perform a read procedure.
 * @param hdl           Attribute handle
 * @param p_data        Pointer to buffer that contains attribute value
 */
static void _cb_att_val_evt(uint8_t conidx, uint8_t user_lid, uint16_t token, uint8_t evt_type, bool complete, 
                            uint16_t hdl, co_buf_t* p_data);

/**
 * @brief Event triggered when a service change has been received or if an attribute
 *        transaction triggers an out of sync error.
 * 
 * @param conidx        Connection index
 * @param user_lid      GATT user local identifier
 * @param out_of_sync   True if an out of sync error has been received
 * @param start_hdl     Service start handle
 * @param end_hdl       Service end handle
 */
static void _cb_svc_changed(uint8_t conidx, uint8_t user_lid, bool out_of_sync, uint16_t start_hdl, uint16_t end_hdl);

/*
 * PRIVATE VARIABLE DECLARATIONS
 ****************************************************************************************
 */
static const gatt_cli_cb_t amsc_gatt_cli_cbs = {
    .cb_discover_cmp    = _cb_discover_cmp,
    .cb_read_cmp        = _cb_read_cmp,
    .cb_write_cmp       = _cb_write_cmp,
    .cb_att_val_get     = _cb_att_val_get,
    .cb_svc             = _cb_svc,
    .cb_svc_info        = _cb_svc_info,
    .cb_inc_svc         = _cb_inc_svc,
    .cb_char            = _cb_char,
    .cb_desc            = _cb_desc,
    .cb_att_val         = _cb_att_val,
    .cb_att_val_evt     = _cb_att_val_evt,
    .cb_svc_changed     = _cb_svc_changed,
};

/// Specifies the default message handlers
KE_MSG_HANDLER_TAB(amsc){ /// AKA amsc_msg_handler_tab
    /// handlers for command from upper layer
    {AMSC_ENABLE_REQ,       (ke_msg_func_t)_enable_req_handler},
    {AMSC_READ_CMD,         (ke_msg_func_t)_read_cmd_handler},
    {AMSC_WRITE_CMD,        (ke_msg_func_t)_write_cmd_handler},

    /// handlers for command from GATT layer
    {GATT_CMP_EVT,          (ke_msg_func_t)_gatt_cmp_evt_handler},
};

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
static int _enable_req_handler(ke_msg_id_t const msgid,
                               struct amsc_enable_req *param,
                               ke_task_id_t const dest_id,
                               ke_task_id_t const src_id)
{
    // Status
    uint8_t status = GAP_ERR_NO_ERROR;
    PRF_ENV_T(amsc) *amsc_env = PRF_ENV_GET(AMSC, amsc);
    // Get connection index
    uint8_t conidx = param->conidx;
    LOG_I("%s Entry.conidx=%d", __func__, conidx);

    if (amsc_env->env[conidx] == NULL)
    {
        LOG_I("%s passed state check", __func__);
        // allocate environment variable for task instance
        amsc_env->env[conidx] = (struct amsc_cnx_env *)ke_malloc(sizeof(struct amsc_cnx_env), KE_MEM_PROFILE);
        ASSERT(amsc_env->env[conidx], "%s alloc error", __func__);
        memset(amsc_env->env[conidx], 0, sizeof(struct amsc_cnx_env));

        amsc_env->env[conidx]->last_char_code = AMSC_ENABLE_OP_CODE;

        /// Start discovering
        gatt_cli_discover_svc_cmd_t *cmd = KE_MSG_ALLOC(GATT_CMD,
                                                        TASK_GATT,
                                                        dest_id,
                                                        gatt_cli_discover_svc_cmd);

        cmd->cmd_code = GATT_CLI_DISCOVER_SVC;
        cmd->dummy = 0;
        cmd->user_lid = amsc_env->user_lid;
        cmd->conidx = conidx;
        cmd->disc_type = GATT_DISCOVER_SVC_PRIMARY_BY_UUID;
        cmd->full = true;
        cmd->start_hdl = GATT_MIN_HDL;
        cmd->end_hdl = GATT_MAX_HDL;
        cmd->uuid_type = GATT_UUID_128;
        memcpy(cmd->uuid, ams_svc_uuid, GATT_UUID_128_LEN);

        /// send msg to GATT layer
        ke_msg_send(cmd);

        /// Go to DISCOVERING state
        // ke_state_set(dest_id, AMSC_DISCOVERING);

        // Configure the environment for a discovery procedure
        amsc_env->env[conidx]->last_req = GATT_DECL_PRIMARY_SERVICE;
    }
    else
    {
        LOG_I("amsc_env->env[conidx] != NULL, pls check");
        status = PRF_ERR_REQ_DISALLOWED;
    }
    // send an error if request fails
    if (status != GAP_ERR_NO_ERROR)
    {
        amsc_enable_rsp_send(amsc_env, conidx, status);
    }

    return (KE_MSG_CONSUMED);
}

static int _read_cmd_handler(ke_msg_id_t const msgid,
                             ams_cli_rd_cmd_t *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id)
{
    LOG_I("%s Entry. hdl=0x%4.4x", __func__, param->hdl);

    // Get the address of the environment
    PRF_ENV_T(amsc) *amsc_env = PRF_ENV_GET(AMSC, amsc);

    if (amsc_env != NULL)
    { /// ask GATT layer to read server ATT value according to incoming handle
        gatt_cli_read_cmd_t *cmd = KE_MSG_ALLOC(GATT_CMD,
                                                TASK_GATT,
                                                dest_id,
                                                gatt_cli_read_cmd);

        cmd->cmd_code = GATT_CLI_READ;
        cmd->dummy = HAVE_DEFERED_OP;
        cmd->user_lid = amsc_env->user_lid;
        cmd->conidx = param->conidx;
        cmd->hdl = param->hdl;
        cmd->offset = 0;
        cmd->length = 0;

        /// store the defered read confirm info
        amsc_env->rdInfo.conidx = param->conidx;
        amsc_env->rdInfo.user_lid = param->user_lid;
        amsc_env->rdInfo.token = param->token;
        amsc_env->rdInfo.status = GAP_ERR_NO_ERROR;
        ke_msg_send(cmd);
    }
    else
    {
        // amsc_send_no_conn_cmp_evt(dest_id, src_id, param->handle,
        //                           AMSC_WRITE_CL_CFG_OP_CODE);
        ASSERT(0, "%s implement me", __func__);
    }

    return KE_MSG_CONSUMED;
}

static int _write_cmd_handler(ke_msg_id_t const msgid,
                              gatt_cli_write_cmd_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    LOG_I("%s hdl=0x%4.4x, conidx=%d, len=%d",
          __func__, param->hdl, param->conidx, param->value_length);

    uint8_t conidx = KE_IDX_GET(dest_id);

    // Get the address of the environment
    PRF_ENV_T(amsc) *amsc_env = PRF_ENV_GET(AMSC, amsc);

    if (amsc_env != NULL)
    { /// ask GATT layer to read server ATT value
        gatt_cli_write_cmd_t *cmd = KE_MSG_ALLOC_DYN(GATT_CMD,
                                                TASK_GATT,
                                                dest_id,
                                                gatt_cli_write_cmd,
                                                param->value_length);

        cmd->cmd_code = GATT_CLI_WRITE;
        cmd->dummy = HAVE_DEFERED_OP;
        cmd->user_lid = amsc_env->user_lid;
        cmd->conidx = param->conidx;
        cmd->write_type = param->write_type;
        cmd->hdl = param->hdl;
        cmd->offset = param->offset;
        cmd->value_length = param->value_length;
        memcpy((uint8_t*)cmd->value, (uint8_t*)param->value, param->value_length);

        amsc_env->wrInfo[conidx].conidx = param->conidx;
        amsc_env->wrInfo[conidx].user_lid = param->user_lid;
        amsc_env->wrInfo[conidx].token = param->dummy;
        amsc_env->wrInfo[conidx].status = GAP_ERR_NO_ERROR;

        // Send the message
        ke_msg_send(cmd);
    }
    else
    {
        // amsc_send_no_conn_cmp_evt(dest_id, src_id, param->handle,
        //                           AMSC_WRITE_CL_CFG_OP_CODE);
        ASSERT(0, "%s implement me", __func__);
    }

    return KE_MSG_CONSUMED;
}

static int _gatt_cmp_evt_handler(ke_msg_id_t const msgid,
                                 gatt_proc_cmp_evt_t *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    LOG_I("%s", __func__);

    return KE_MSG_CONSUMED;
}

static void _cb_discover_cmp(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status)
{

    /// Get the address of the environment
    PRF_ENV_T(amsc) *amsc_env = PRF_ENV_GET(AMSC, amsc);
    ASSERT(amsc_env->env[conidx], "%s %d", __func__, __LINE__);
    LOG_I("%s status=%d, conidx=%d,  nb_svc=%d", __func__, status, conidx,amsc_env->env[conidx]->nb_svc);

    uint8_t ret = status;

    if ((ATT_ERR_ATTRIBUTE_NOT_FOUND == status) ||
        (GAP_ERR_NO_ERROR == status))
    {
        // Discovery
        // check characteristic validity
        if (amsc_env->env[conidx]->nb_svc == 1)
        {
            ret = prf_check_svc128_validity(AMSC_CHAR_MAX, 
                                            amsc_env->env[conidx]->ams.chars, amsc_ams_char,
                                            AMSC_DESC_MAX,
                                            amsc_env->env[conidx]->ams.descs, amsc_ams_desc);
        }
        // too much services
        else if (amsc_env->env[conidx]->nb_svc > 1)
        {
            ret = PRF_ERR_MULTIPLE_SVC;
        }
        // no services found
        else
        {
            ret = PRF_ERR_STOP_DISC_CHAR_MISSING;
        }
    }

    if (GAP_ERR_NO_ERROR == ret)
    {
        /// inform APP layer that enable rsp status
        struct amsc_enable_rsp *rsp = KE_MSG_ALLOC(AMSC_ENABLE_RSP,
                                                KE_BUILD_ID(TASK_APP, conidx),
                                                KE_BUILD_ID(prf_get_task_from_id(TASK_ID_AMSC), conidx),
                                                amsc_enable_rsp);

    
        /// registor AMSC event handle for GATT layer
        gatt_cli_event_register(conidx,
                                amsc_env->user_lid,
                                amsc_env->env[conidx]->ams.svc.shdl,
                                amsc_env->env[conidx]->ams.svc.ehdl);
    
        /// Subscribe Entity Update to MS
        co_buf_t* pBuf = NULL;
        prf_buf_alloc(&pBuf, SUBSCRIBE_AMS_SVC_CCCD_LEN);
    
        /// Dummy parameter provided to GATT
        uint16_t dummy = amsc_env->env[conidx]->ams.descs[AMSC_DESC_ENTITY_UPDATE_CL_CFG].desc_hdl;
        /// CCCD value for subscribe notification source
        uint16_t cccdVal = SUBSCRIBE_AMS_SVC_CCCD_VAL;
    
        uint8_t* pData = co_buf_data(pBuf);
        memcpy(pData, (uint8_t *)&cccdVal, SUBSCRIBE_AMS_SVC_CCCD_LEN);
    
        LOG_I("Subscribe Entity Update, hdl:%d, value: 0x%02x 0x%02x",
              amsc_env->env[conidx]->ams.descs[AMSC_DESC_ENTITY_UPDATE_CL_CFG].desc_hdl,
              pData[0], pData[1]);
        gatt_cli_write(conidx,
                       amsc_env->user_lid,
                       dummy,
                       GATT_WRITE,
                       amsc_env->env[conidx]->ams.descs[AMSC_DESC_ENTITY_UPDATE_CL_CFG].desc_hdl,
                       0,
                       pBuf);
    
        /// Release the buffer
        co_buf_release(pBuf);
        rsp->status = status;
        rsp->ams.chars[AMSC_REMOTE_COMMAND_CHAR].val_hdl = amsc_env->env[conidx]->ams.chars[AMSC_REMOTE_COMMAND_CHAR].val_hdl;
        rsp->ams.chars[AMSC_ENTITY_UPDATE_CHAR].val_hdl = amsc_env->env[conidx]->ams.chars[AMSC_ENTITY_UPDATE_CHAR].val_hdl;
        rsp->ams.chars[AMSC_ENTITY_ATTRIBUTE_CHAR].val_hdl = amsc_env->env[conidx]->ams.chars[AMSC_ENTITY_ATTRIBUTE_CHAR].val_hdl;

        ke_msg_send(rsp);
    }

#ifdef AMS_ENABLED
    LOG_I("%s rmtVal=0x%4.4x, rmtCfg=0x%4.4x",
          __func__,
          amsc_env->env[conidx]->ams.chars[AMSC_REMOTE_COMMAND_CHAR].val_hdl,
          amsc_env->env[conidx]->ams.descs[AMSC_DESC_REMOTE_CMD_CL_CFG].desc_hdl);
    LOG_I("%s EnUpVal=0x%4.4x, EnUpCfg=0x%4.4x",
          __func__,
          amsc_env->env[conidx]->ams.chars[AMSC_ENTITY_UPDATE_CHAR].val_hdl,
          amsc_env->env[conidx]->ams.descs[AMSC_DESC_ENTITY_UPDATE_CL_CFG].desc_hdl);
    LOG_I("%s EnAtrVal=0x%4.4x", __func__,
          amsc_env->env[conidx]->ams.chars[AMSC_ENTITY_ATTRIBUTE_CHAR].val_hdl);

    amsp_set_ready_flag(conidx,
                        0,
                        amsc_env->env[conidx]->ams.chars[AMSC_REMOTE_COMMAND_CHAR].val_hdl,
                        amsc_env->env[conidx]->ams.descs[AMSC_DESC_REMOTE_CMD_CL_CFG].desc_hdl,
                        0,
                        amsc_env->env[conidx]->ams.chars[AMSC_ENTITY_UPDATE_CHAR].val_hdl,
                        amsc_env->env[conidx]->ams.descs[AMSC_DESC_ENTITY_UPDATE_CL_CFG].desc_hdl,
                        0,
                        amsc_env->env[conidx]->ams.chars[AMSC_ENTITY_ATTRIBUTE_CHAR].val_hdl);
#endif
}

static void _cb_read_cmp(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status)
{
    LOG_I("%s read complete status=%d", __func__, status);

    /// unique handling for BISTO
#ifdef BISTO_ENABLED
    /// Get the address of the environment
    PRF_ENV_T(amsc) *amsc_env = PRF_ENV_GET(AMSC, amsc);
    ASSERT(amsc_env->env[conidx], "%s %d", __func__, __LINE__);

    if ((GAP_ERR_NO_ERROR != status) &&
        (HAVE_DEFERED_OP == dummy))
    { /// inform proxy to execute defered read confirm operation
        ams_defer_cfm_info_t *cfm = KE_MSG_ALLOC(AMSP_READ_CFM,
                                             KE_BUILD_ID(prf_get_task_from_id(TASK_ID_AMSP), conidx),
                                             KE_BUILD_ID(prf_get_task_from_id(TASK_ID_AMSC), conidx),
                                             ams_defer_cfm_info_t);

        /// restore defered confirm op
        memcpy((uint8_t *)cfm, (uint8_t *)&amsc_env->rdInfo, sizeof(ams_defer_cfm_info_t));
        ke_msg_send(cfm);
    }
    else
    {
        LOG_E("status:%d, dummy:%d", status, dummy);
    }
#endif
}

static void _cb_write_cmp(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t status)
{
    LOG_I("%s %d", __func__, dummy);

    /// Get the address of the environment
    PRF_ENV_T(amsc) *amsc_env = PRF_ENV_GET(AMSC, amsc);
    ASSERT(amsc_env->env[conidx], "%s %d", __func__, __LINE__);

    if (dummy == amsc_env->env[conidx]->ams.descs[AMSC_DESC_ENTITY_UPDATE_CL_CFG].desc_hdl)
    {
        /// Subscribe Remote Command to MS
        co_buf_t* pBuf = NULL;
        prf_buf_alloc(&pBuf, SUBSCRIBE_AMS_SVC_CCCD_LEN);

        /// Dummy parameter provided to GATT
        uint16_t dummy = amsc_env->env[conidx]->ams.descs[AMSC_DESC_REMOTE_CMD_CL_CFG].desc_hdl;
        /// CCCD value for subscribe Remote Command
        uint16_t cccdVal = SUBSCRIBE_AMS_SVC_CCCD_VAL;

        uint8_t* pData = co_buf_data(pBuf);
        memcpy(pData, (uint8_t *)&cccdVal, SUBSCRIBE_AMS_SVC_CCCD_LEN);

        LOG_I("Subscribe Remote Command, hdl:%d, value: 0x%02x 0x%02x",
              amsc_env->env[conidx]->ams.descs[AMSC_DESC_REMOTE_CMD_CL_CFG].desc_hdl,
              pData[0], pData[1]);
        gatt_cli_write(conidx,
                       amsc_env->user_lid,
                       dummy,
                       GATT_WRITE,
                       amsc_env->env[conidx]->ams.descs[AMSC_DESC_REMOTE_CMD_CL_CFG].desc_hdl,
                       0,
                       pBuf);

        /// Release the buffer
        co_buf_release(pBuf);
    }
    else
    {
        /// inform APP layer that client write process is complished
        ams_cli_wr_cmp_t *cmp = KE_MSG_ALLOC(AMSC_WRITE_CMP,
                                             KE_BUILD_ID(TASK_APP, conidx),
                                             KE_BUILD_ID(prf_get_task_from_id(TASK_ID_AMSC), conidx),
                                             ams_cli_wr_cmp);

        cmp->conidx = conidx;
        cmp->user_lid = user_lid;
        cmp->hdl = dummy;
        cmp->status = status;

        ke_msg_send(cmp);
    }

}

static void _cb_att_val_get(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t dummy,
                            uint16_t hdl, uint16_t offset, uint16_t max_length)
{
    LOG_I("%s", __func__);
}

static void _cb_svc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t hdl, uint8_t disc_info,
                    uint8_t nb_att, const gatt_svc_att_t* p_atts)
{
    LOG_I("%s incoming hdl=0x%4.4x, nb_att=%d",
          __func__, hdl, nb_att);
    
    LOG_I("charac[0] info: prop=%d, handle=0x%4.4x, att_type=%d",
          p_atts[0].info.charac.prop, p_atts[0].info.charac.val_hdl, p_atts[0].att_type);
    LOG_I("svc[0] info: att_type=%d, svc.end_hdl=0x%4.4x, svc.start_hdl=0x%4.4x",
          p_atts[0].att_type, p_atts[0].info.svc.end_hdl, p_atts[0].info.svc.start_hdl);

    PRF_ENV_T(amsc) *amsc_env = PRF_ENV_GET(AMSC, amsc);
    ASSERT_INFO(amsc_env, 0, 0);
    ASSERT_INFO(amsc_env->env[conidx], 0, 0);

    if (amsc_env->env[conidx]->nb_svc == 0)
    {
        LOG_I("retrieving characteristics and descriptors.");
        for (uint8_t i = 0; i < nb_att; i++)
        {
            LOG_I("att_type:%d, uuid_type:%d, uuid:", p_atts[i].att_type, p_atts[i].uuid_type);
            DUMP8("0x%02x ", p_atts[i].uuid, 16);
            if ((GATT_ATT_PRIMARY_SVC == p_atts[i].att_type) || //!< 1
                (GATT_ATT_SECONDARY_SVC == p_atts[i].att_type) || //!< 2
                (GATT_ATT_INCL_SVC == p_atts[i].att_type)) //!< 3
            {
                LOG_I("start_hdl:%d, end_hdl:%d", p_atts[i].info.svc.start_hdl, p_atts[i].info.svc.end_hdl);
            }
            else
            {
                LOG_I("val_hdl:%d, prop:%d", p_atts[i].info.charac.val_hdl, p_atts[i].info.charac.prop);
            }
        }

        /// Retrieve AMS characteristics and descriptors
        prf_extract_svc128_info(hdl, nb_att, p_atts,
                                AMSC_CHAR_MAX, amsc_ams_char, amsc_env->env[conidx]->ams.chars,
                                AMSC_DESC_MAX, amsc_ams_desc, amsc_env->env[conidx]->ams.descs);

        for (uint8_t j = 0; j < AMSC_CHAR_MAX; j++)
        {
            LOG_I("[characteristic] val_hdl:%d, prop:%d", amsc_env->env[conidx]->ams.chars[j].val_hdl, amsc_env->env[conidx]->ams.chars[j].prop);
            if(amsc_env->env[conidx]->ams.chars[j].val_hdl == 0) {
                return;
            }
        }

        for (uint8_t k = 0; k < AMSC_DESC_MAX; k++)
        {
            LOG_I("[descriptor] val_hdl:%d", amsc_env->env[conidx]->ams.descs[k].desc_hdl);
            if(amsc_env->env[conidx]->ams.descs[k].desc_hdl == 0) {
                return;
            }
        }

        /// Even if we get multiple responses we only store 1 range
        amsc_env->env[conidx]->ams.svc.shdl = p_atts[0].info.svc.start_hdl;
        amsc_env->env[conidx]->ams.svc.ehdl = p_atts[0].info.svc.end_hdl;
        amsc_env->shdl = hdl;
        amsc_env->env[conidx]->nb_svc++;
    }

}

static void _cb_svc_info(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t start_hdl, uint16_t end_hdl,
                        uint8_t uuid_type, const uint8_t* p_uuid)
{
    LOG_I("%s", __func__);
}

static void _cb_inc_svc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t inc_svc_hdl,
                        uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_type, const uint8_t* p_uuid)
{
    LOG_I("%s", __func__);
}

static void _cb_char(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t char_hdl, uint16_t val_hdl,
                    uint8_t prop, uint8_t uuid_type, const uint8_t* p_uuid)
{
    LOG_I("%s", __func__);
}

static void _cb_desc(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t desc_hdl,
                    uint8_t uuid_type, const uint8_t* p_uuid)
{
    LOG_I("%s", __func__);
}

static void _cb_att_val(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint16_t hdl, uint16_t offset,
                        co_buf_t* p_data)
{
    uint8_t *p_value = co_buf_data(p_data);
    LOG_I("%s hdl:%d, received dataLen:%d, data:", __func__, hdl, p_data->data_len);
    DUMP8("0x%02x ", p_value, p_data->data_len);
    /// report received data to APP layer
    ams_cli_att_val_t *val = KE_MSG_ALLOC_DYN(AMSC_ATT_VAL,
                                              KE_BUILD_ID(TASK_APP, conidx),
                                              KE_BUILD_ID(prf_get_task_from_id(TASK_ID_AMSC), conidx),
                                              ams_cli_att_val,
                                              p_data->data_len);
    val->conidx = conidx;
    val->user_lid = user_lid;
    val->dummy = dummy;
    val->hdl = hdl;
    val->offset = offset;
    val->value_length = p_data->data_len;
    memcpy(val->value, p_value, p_data->data_len);

    ke_msg_send(val);
}

static void _cb_att_val_evt(uint8_t conidx, uint8_t user_lid, uint16_t token, uint8_t evt_type, bool complete, 
                            uint16_t hdl, co_buf_t* p_data)
{
    LOG_I("%s evt_type:%d", __func__, evt_type);
    ASSERT(complete, "%s %d, event not complete", __func__, __LINE__);

    /// inform APP layer that client received an indication
    ams_cli_ind_evt_t *cmd =
        KE_MSG_ALLOC_DYN(AMSC_IND_EVT,
                         KE_BUILD_ID(TASK_APP, conidx),
                         KE_BUILD_ID(prf_get_task_from_id(TASK_ID_AMSC), conidx),
                         ams_cli_ind_evt,
                         p_data->data_len);

    cmd->conidx = conidx;
    cmd->hdl = hdl;
    cmd->value_length = p_data->data_len;
    memcpy(cmd->value, co_buf_data(p_data), p_data->data_len);

    ke_msg_send(cmd);

    // confirm event reception
    gatt_cli_att_event_cfm(conidx, user_lid, token);

}

static void _cb_svc_changed(uint8_t conidx, uint8_t user_lid, bool out_of_sync, uint16_t start_hdl, uint16_t end_hdl)
{
    LOG_I("%s", __func__);
}

void amsc_task_init(struct ke_task_desc *task_desc, void* p_env)
{
    LOG_I("%s Entry.", __func__);
    // Get the address of the environment
    PRF_ENV_T(amsc) *amsc_env = (PRF_ENV_T(amsc) *)p_env;

    task_desc->msg_handler_tab = amsc_msg_handler_tab;
    task_desc->msg_cnt = ARRAY_LEN(amsc_msg_handler_tab);
    task_desc->state = amsc_env->state;
    task_desc->idx_max = AMSC_IDX_MAX;
}

const gatt_cli_cb_t *amsc_task_get_cli_cbs(void)
{
    return &amsc_gatt_cli_cbs;
}

#endif //(BLE_AMS_CLIENT)

/// @} AMSCTASK
