/***************************************************************************
 *
 * Copyright 2020-2022 BES.
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
#ifndef __PLAT_ADDR_MAP_SENS_BEST1603_H__
#define __PLAT_ADDR_MAP_SENS_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RAM0_BASE                               SENS_RAM_BASE
#define RAM0_SIZE                               SENS_RAM_SIZE

#define RAM_TOTAL_SIZE                          (RAM0_BASE + RAM0_SIZE - RAM0_BASE)

#define RAM_BASE                                SENS_RAM_BASE
#define RAM_SIZE                                (SENS_MAILBOX_BASE - RAM_BASE)

#define RAMX_BASE                               (SENS_RAM_BASE - SENS_RAM0_BASE + SENS_RAMX0_BASE)

#define FLASH_BASE                              BTH_FLASH_BASE
#define FLASH_NC_BASE                           BTH_FLASH_NC_BASE
#define FLASHX_BASE                             BTH_FLASHX_BASE
#define FLASHX_NC_BASE                          BTH_FLASHX_NC_BASE

#define WDT_BASE                                SENS_WDT_BASE
#define TIMER0_BASE                             AON_TIMER1_BASE
#define TIMER1_BASE                             SENS_TIMER0_BASE
#define TIMER2_BASE                             SENS_TIMER1_BASE
#define I2C0_BASE                               SENS_I2C0_BASE
#define I2C1_BASE                               SENS_I2C1_BASE
#define I2C2_BASE                               SENS_I2C2_BASE
#define I2C3_BASE                               SENS_I2C3_BASE
#define I2C4_BASE                               SENS_I2C4_BASE
#define SPI_BASE                                SENS_SPI_BASE
#define SPILCD_BASE                             SENS_SPILCD_BASE
#define ISPI_BASE                               SENS_ISPI_BASE
#define UART0_BASE                              SENS_UART0_BASE
#define UART1_BASE                              SENS_UART1_BASE
#define UART2_BASE                              SENS_UART2_BASE
#define TCNT_BASE                               SENS_TCNT_BASE
#define I2S0_BASE                               SENS_I2S0_BASE
#define CPUDUMP_BASE                            SENS_CPUDUMP_BASE

#define GPIO_BASE                               AON_GPIO_BASE
#define GPIO1_BASE                              AON_GPIO1_BASE

#define FLASH_CTRL_BASE                         BTH_FLASH_CTRL_BASE
#define AUDMA_BASE                              SENS_SDMA_BASE

#define SENSOR_ENG0_BASE                        SENS_SENSOR_ENG0_BASE
#define SENSOR_ENG1_BASE                        SENS_SENSOR_ENG1_BASE
#define SENSOR_ENG2_BASE                        SENS_SENSOR_ENG2_BASE
#define SENSOR_ENG3_BASE                        SENS_SENSOR_ENG3_BASE

#define NO_BT_INTERSYS

#ifdef __cplusplus
}
#endif

#endif
