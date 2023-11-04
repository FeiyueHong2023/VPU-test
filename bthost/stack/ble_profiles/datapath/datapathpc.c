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
 * @addtogroup DPPS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_DATAPATH_CLIENT)
#include <string.h>
#include "gap.h"
#include "datapathpc.h"
#include "datapathpc_task.h"
#include "prf_utils.h"
#include "gatt_msg.h"
#include "ke_mem.h"
#include "co_utils.h"
#include "gatt.h"
#include "prf.h"

extern uint16_t datapathps_gatt_get_start_handle(void);


/**
 ****************************************************************************************
 * @brief Initialization of the DATAPATHPC module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Collector or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @see enum attm_value_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint16_t _init_cb(prf_data_t* env, uint16_t* start_hdl,
    uint8_t sec_lvl, uint8_t user_prio, const void* params, const void* p_cb)
{
    uint8_t idx;
    uint16_t status = GAP_ERR_NO_ERROR;
    // Allocate DATAPATHPC required environment variable
    datapathpc_env_t* datapathpc_env =
            (datapathpc_env_t* ) ke_malloc(sizeof(datapathpc_env_t), KE_MEM_PROFILE);
    ASSERT(datapathpc_env, "[%s] datapathpc_env is NULL", __func__);
    memset((uint8_t *)datapathpc_env, 0, sizeof(datapathpc_env_t));
    for(idx = 0; idx < BLE_CONNECTION_MAX ; idx++)
    {
        datapathpc_env->p_env[idx] =
            (struct datapathpc_cnx_env *) ke_malloc(sizeof(struct datapathpc_cnx_env), KE_MEM_PROFILE);
        ASSERT(datapathpc_env->p_env[idx], "[%s] datapathpc_env->p_env[%d] is NULL", __func__,idx);
        memset(datapathpc_env->p_env[idx], 0, sizeof(struct datapathpc_cnx_env));
    }
    // Initialize DATAPATHPC environment
    env->p_env = (prf_hdr_t*) datapathpc_env;

    // Register as GATT User Client
    status = gatt_user_cli_register(PREFERRED_BLE_MTU, 0, datapathpc_task_get_cli_cbs(),
                                    &(datapathpc_env->user_lid));

    if (GAP_ERR_NO_ERROR == status)
    {
        // initialize environment variable
        env->api_id                     = TASK_ID_DATAPATHPC;
        datapathpc_task_init(&(env->desc), (void *)datapathpc_env);
    }

    TRACE(0, "DATAPATHPC uid %d shdl %d", datapathpc_env->user_lid, datapathpc_env->p_env[0]->datapath.svc.shdl);
    return (status);
}

/**
 ****************************************************************************************
 * @brief Destruction of the DATAPATHPC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static uint16_t _destroy_cb(prf_data_t* p_env, uint8_t reason)
{
    datapathpc_env_t* datapathpc_env = (datapathpc_env_t*) p_env->p_env;

    // free profile environment variables
    p_env->p_env = NULL;
    ke_free(datapathpc_env);

    return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void _con_create_cb(prf_data_t* p_env, uint8_t conidx, bool is_le_con)
{
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void _con_cleanup_cb(prf_data_t* env, uint8_t conidx, uint16_t reason)
{
    /* Nothing to do */
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// DATAPATHPC Task interface required by profile manager
const struct prf_task_cbs datapathpc_itf =
{
    _init_cb,
    _destroy_cb,
    _con_create_cb,
    _con_cleanup_cb,
};

/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* datapathpc_prf_itf_get(void)
{
   return &datapathpc_itf;
}


#endif /* BLE_DATAPATH_CLIENT */

/// @} DATAPATHPC
