/**
 ****************************************************************************************
 *
 * @file app_batt.h
 *
 * @brief Battery Application Module entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef APP_BATT_H_
#define APP_BATT_H_

#ifdef __cplusplus
extern "C"{
#endif

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Battery Application Module entry point
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_BATT)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"         // Kernel Task Definition

/*
 * STRUCTURES DEFINITION
 ****************************************************************************************
 */

/// Battery Application Module Environment Structure
struct app_batt_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Current Battery Level
    uint8_t batt_lvl;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Battery Application environment
extern struct app_batt_env_tag app_batt_env;

/// Table of message handlers
extern const struct app_subtask_handlers app_batt_handlers;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * Health Thermometer Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Battery Application Module
 ****************************************************************************************
 */
void app_batt_init(void);

/**
 ****************************************************************************************
 * @brief Add a Battery Service instance in the DB
 ****************************************************************************************
 */
void app_batt_add_bas(void);

/**
 ****************************************************************************************
 * @brief Enable the Battery Service
 ****************************************************************************************
 */
void app_batt_enable_prf(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send a Battery level value
 ****************************************************************************************
 */
void app_batt_send_lvl(uint8_t instance, uint8_t batt_lvl);

/**
 ****************************************************************************************
 * @brief Send a Battery restore bond data
 ****************************************************************************************
 */
void app_batt_srv_restore_bond_data_req(uint8_t con_lid, uint8_t bass_ntf_cfg_bf);

#endif //(BLE_APP_BATT)

/// @} APP
#ifdef __cplusplus
}
#endif

#endif // APP_BATT_H_