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
#ifndef __HAL_IOMUX_BEST1603_H__
#define __HAL_IOMUX_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#if !defined(ROM_BUILD) && !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
#define PMU_HAS_LED_PIN
#define PMU_HAS_GPIO_PIN
#define PMU_HAS_GPIO_IRQ
#endif

#define HAL_GPIO_BANK0_PIN_NUM              32
#define HAL_GPIO_BANK1_PIN_NUM              16

enum HAL_IOMUX_PIN_T {
    HAL_IOMUX_PIN_P0_0 = 0,
    HAL_IOMUX_PIN_P0_1,
    HAL_IOMUX_PIN_P0_2,
    HAL_IOMUX_PIN_P0_3,
    HAL_IOMUX_PIN_P0_4,
    HAL_IOMUX_PIN_P0_5,
    HAL_IOMUX_PIN_P0_6,
    HAL_IOMUX_PIN_P0_7,

    HAL_IOMUX_PIN_P1_0,
    HAL_IOMUX_PIN_P1_1,
    HAL_IOMUX_PIN_P1_2,
    HAL_IOMUX_PIN_P1_3,
    HAL_IOMUX_PIN_P1_4,
    HAL_IOMUX_PIN_P1_5,
    HAL_IOMUX_PIN_P1_6,
    HAL_IOMUX_PIN_P1_7,

    HAL_IOMUX_PIN_P2_0,
    HAL_IOMUX_PIN_P2_1,
    HAL_IOMUX_PIN_P2_2,
    HAL_IOMUX_PIN_P2_3,
    HAL_IOMUX_PIN_P2_4,
    HAL_IOMUX_PIN_P2_5,
    HAL_IOMUX_PIN_P2_6,
    HAL_IOMUX_PIN_P2_7,

    HAL_IOMUX_PIN_P3_0,
    HAL_IOMUX_PIN_P3_1,
    HAL_IOMUX_PIN_P3_2,
    HAL_IOMUX_PIN_P3_3,
    HAL_IOMUX_PIN_P3_4,
    HAL_IOMUX_PIN_P3_5,
    HAL_IOMUX_PIN_P3_6,
    HAL_IOMUX_PIN_P3_7,

    HAL_IOMUX_PIN_P4_0,
    HAL_IOMUX_PIN_P4_1,
    HAL_IOMUX_PIN_P4_2,
    HAL_IOMUX_PIN_P4_3,
    HAL_IOMUX_PIN_P4_4,
    HAL_IOMUX_PIN_P4_5,
    HAL_IOMUX_PIN_P4_6,
    HAL_IOMUX_PIN_P4_7,

    HAL_IOMUX_PIN_P5_0,
    HAL_IOMUX_PIN_P5_1,
    HAL_IOMUX_PIN_P5_2,
    HAL_IOMUX_PIN_P5_3,
    HAL_IOMUX_PIN_P5_4,
    HAL_IOMUX_PIN_P5_5,
    HAL_IOMUX_PIN_P5_6,
    HAL_IOMUX_PIN_P5_7,

    HAL_IOMUX_PIN_NUM,

    HAL_IOMUX_PIN_LED1 = HAL_IOMUX_PIN_NUM,
    HAL_IOMUX_PIN_LED2,
    HAL_IOMUX_PIN_LED3,

    HAL_IOMUX_PIN_LED_NUM,

    HAL_IOMUX_PIN_PMU_P0_0 = HAL_IOMUX_PIN_LED_NUM,
    HAL_IOMUX_PIN_PMU_P0_1,
    HAL_IOMUX_PIN_PMU_P0_2,
    HAL_IOMUX_PIN_PMU_P0_3,
    HAL_IOMUX_PIN_PMU_P0_4,
    HAL_IOMUX_PIN_PMU_P0_5,
    HAL_IOMUX_PIN_PMU_P0_6,
    HAL_IOMUX_PIN_PMU_P0_7,

    HAL_IOMUX_PIN_PMU_P1_0,
    HAL_IOMUX_PIN_PMU_P1_1,
    HAL_IOMUX_PIN_PMU_P1_2,
    HAL_IOMUX_PIN_PMU_P1_3,
    HAL_IOMUX_PIN_PMU_P1_4,
    HAL_IOMUX_PIN_PMU_P1_5,
    HAL_IOMUX_PIN_PMU_P1_6,
    HAL_IOMUX_PIN_PMU_P1_7,

    HAL_IOMUX_PIN_PMU_P2_0,
    HAL_IOMUX_PIN_PMU_P2_1,
    HAL_IOMUX_PIN_PMU_P2_2,
    HAL_IOMUX_PIN_PMU_P2_3,
    HAL_IOMUX_PIN_PMU_P2_4,
    HAL_IOMUX_PIN_PMU_P2_5,
    HAL_IOMUX_PIN_PMU_P2_6,
    HAL_IOMUX_PIN_PMU_P2_7,

    HAL_IOMUX_PIN_PMU_P3_0,
    HAL_IOMUX_PIN_PMU_P3_1,
    HAL_IOMUX_PIN_PMU_P3_2,
    HAL_IOMUX_PIN_PMU_P3_3,
    HAL_IOMUX_PIN_PMU_P3_4,
    HAL_IOMUX_PIN_PMU_P3_5,
    HAL_IOMUX_PIN_PMU_P3_6,
    HAL_IOMUX_PIN_PMU_P3_7,

    HAL_IOMUX_PIN_PMU_P4_0,
    HAL_IOMUX_PIN_PMU_P4_1,
    HAL_IOMUX_PIN_PMU_P4_2,
    HAL_IOMUX_PIN_PMU_P4_3,
    HAL_IOMUX_PIN_PMU_P4_4,
    HAL_IOMUX_PIN_PMU_P4_5,
    HAL_IOMUX_PIN_PMU_P4_6,
    HAL_IOMUX_PIN_PMU_P4_7,

    HAL_IOMUX_PIN_PMU_P5_0,
    HAL_IOMUX_PIN_PMU_P5_1,
    HAL_IOMUX_PIN_PMU_P5_2,
    HAL_IOMUX_PIN_PMU_P5_3,
    HAL_IOMUX_PIN_PMU_P5_4,
    HAL_IOMUX_PIN_PMU_P5_5,
    HAL_IOMUX_PIN_PMU_P5_6,
    HAL_IOMUX_PIN_PMU_P5_7,

    HAL_IOMUX_PIN_PMU_P6_0,
    HAL_IOMUX_PIN_PMU_P6_1,
    HAL_IOMUX_PIN_PMU_P6_2,
    HAL_IOMUX_PIN_PMU_P6_3,
    HAL_IOMUX_PIN_PMU_P6_4,
    HAL_IOMUX_PIN_PMU_P6_5,
    HAL_IOMUX_PIN_PMU_P6_6,
    HAL_IOMUX_PIN_PMU_P6_7,

    HAL_IOMUX_PIN_PMU_P7_0,
    HAL_IOMUX_PIN_PMU_P7_1,
    HAL_IOMUX_PIN_PMU_P7_2,
    HAL_IOMUX_PIN_PMU_P7_3,

    HAL_IOMUX_PIN_PMU_NUM
};

enum HAL_GPIO_PIN_T {
    HAL_GPIO_PIN_P0_0 = HAL_IOMUX_PIN_P0_0,
    HAL_GPIO_PIN_P0_1 = HAL_IOMUX_PIN_P0_1,
    HAL_GPIO_PIN_P0_2 = HAL_IOMUX_PIN_P0_2,
    HAL_GPIO_PIN_P0_3 = HAL_IOMUX_PIN_P0_3,
    HAL_GPIO_PIN_P0_4 = HAL_IOMUX_PIN_P0_4,
    HAL_GPIO_PIN_P0_5 = HAL_IOMUX_PIN_P0_5,
    HAL_GPIO_PIN_P0_6 = HAL_IOMUX_PIN_P0_6,
    HAL_GPIO_PIN_P0_7 = HAL_IOMUX_PIN_P0_7,

    HAL_GPIO_PIN_P1_0 = HAL_IOMUX_PIN_P1_0,
    HAL_GPIO_PIN_P1_1 = HAL_IOMUX_PIN_P1_1,
    HAL_GPIO_PIN_P1_2 = HAL_IOMUX_PIN_P1_2,
    HAL_GPIO_PIN_P1_3 = HAL_IOMUX_PIN_P1_3,
    HAL_GPIO_PIN_P1_4 = HAL_IOMUX_PIN_P1_4,
    HAL_GPIO_PIN_P1_5 = HAL_IOMUX_PIN_P1_5,
    HAL_GPIO_PIN_P1_6 = HAL_IOMUX_PIN_P1_6,
    HAL_GPIO_PIN_P1_7 = HAL_IOMUX_PIN_P1_7,

    HAL_GPIO_PIN_P2_0 = HAL_IOMUX_PIN_P2_0,
    HAL_GPIO_PIN_P2_1 = HAL_IOMUX_PIN_P2_1,
    HAL_GPIO_PIN_P2_2 = HAL_IOMUX_PIN_P2_2,
    HAL_GPIO_PIN_P2_3 = HAL_IOMUX_PIN_P2_3,
    HAL_GPIO_PIN_P2_4 = HAL_IOMUX_PIN_P2_4,
    HAL_GPIO_PIN_P2_5 = HAL_IOMUX_PIN_P2_5,
    HAL_GPIO_PIN_P2_6 = HAL_IOMUX_PIN_P2_6,
    HAL_GPIO_PIN_P2_7 = HAL_IOMUX_PIN_P2_7,

    HAL_GPIO_PIN_P3_0 = HAL_IOMUX_PIN_P3_0,
    HAL_GPIO_PIN_P3_1 = HAL_IOMUX_PIN_P3_1,
    HAL_GPIO_PIN_P3_2 = HAL_IOMUX_PIN_P3_2,
    HAL_GPIO_PIN_P3_3 = HAL_IOMUX_PIN_P3_3,
    HAL_GPIO_PIN_P3_4 = HAL_IOMUX_PIN_P3_4,
    HAL_GPIO_PIN_P3_5 = HAL_IOMUX_PIN_P3_5,
    HAL_GPIO_PIN_P3_6 = HAL_IOMUX_PIN_P3_6,
    HAL_GPIO_PIN_P3_7 = HAL_IOMUX_PIN_P3_7,

    HAL_GPIO_PIN_P4_0 = HAL_IOMUX_PIN_P4_0,
    HAL_GPIO_PIN_P4_1 = HAL_IOMUX_PIN_P4_1,
    HAL_GPIO_PIN_P4_2 = HAL_IOMUX_PIN_P4_2,
    HAL_GPIO_PIN_P4_3 = HAL_IOMUX_PIN_P4_3,
    HAL_GPIO_PIN_P4_4 = HAL_IOMUX_PIN_P4_4,
    HAL_GPIO_PIN_P4_5 = HAL_IOMUX_PIN_P4_5,
    HAL_GPIO_PIN_P4_6 = HAL_IOMUX_PIN_P4_6,
    HAL_GPIO_PIN_P4_7 = HAL_IOMUX_PIN_P4_7,

    HAL_GPIO_PIN_P5_0 = HAL_IOMUX_PIN_P5_0,
    HAL_GPIO_PIN_P5_1 = HAL_IOMUX_PIN_P5_1,
    HAL_GPIO_PIN_P5_2 = HAL_IOMUX_PIN_P5_2,
    HAL_GPIO_PIN_P5_3 = HAL_IOMUX_PIN_P5_3,
    HAL_GPIO_PIN_P5_4 = HAL_IOMUX_PIN_P5_4,
    HAL_GPIO_PIN_P5_5 = HAL_IOMUX_PIN_P5_5,
    HAL_GPIO_PIN_P5_6 = HAL_IOMUX_PIN_P5_6,
    HAL_GPIO_PIN_P5_7 = HAL_IOMUX_PIN_P5_7,

    HAL_GPIO_PIN_NUM = HAL_IOMUX_PIN_NUM,

    HAL_GPIO_PIN_LED1 = HAL_IOMUX_PIN_LED1,
    HAL_GPIO_PIN_LED2 = HAL_IOMUX_PIN_LED2,
    HAL_GPIO_PIN_LED3 = HAL_IOMUX_PIN_LED3,

    HAL_GPIO_PIN_LED_NUM = HAL_IOMUX_PIN_LED_NUM,

    HAL_GPIO_PIN_PMU_P0_0 = HAL_IOMUX_PIN_PMU_P0_0,
    HAL_GPIO_PIN_PMU_P0_1 = HAL_IOMUX_PIN_PMU_P0_1,
    HAL_GPIO_PIN_PMU_P0_2 = HAL_IOMUX_PIN_PMU_P0_2,
    HAL_GPIO_PIN_PMU_P0_3 = HAL_IOMUX_PIN_PMU_P0_3,
    HAL_GPIO_PIN_PMU_P0_4 = HAL_IOMUX_PIN_PMU_P0_4,
    HAL_GPIO_PIN_PMU_P0_5 = HAL_IOMUX_PIN_PMU_P0_5,
    HAL_GPIO_PIN_PMU_P0_6 = HAL_IOMUX_PIN_PMU_P0_6,
    HAL_GPIO_PIN_PMU_P0_7 = HAL_IOMUX_PIN_PMU_P0_7,

    HAL_GPIO_PIN_PMU_P1_0 = HAL_IOMUX_PIN_PMU_P1_0,
    HAL_GPIO_PIN_PMU_P1_1 = HAL_IOMUX_PIN_PMU_P1_1,
    HAL_GPIO_PIN_PMU_P1_2 = HAL_IOMUX_PIN_PMU_P1_2,
    HAL_GPIO_PIN_PMU_P1_3 = HAL_IOMUX_PIN_PMU_P1_3,
    HAL_GPIO_PIN_PMU_P1_4 = HAL_IOMUX_PIN_PMU_P1_4,
    HAL_GPIO_PIN_PMU_P1_5 = HAL_IOMUX_PIN_PMU_P1_5,
    HAL_GPIO_PIN_PMU_P1_6 = HAL_IOMUX_PIN_PMU_P1_6,
    HAL_GPIO_PIN_PMU_P1_7 = HAL_IOMUX_PIN_PMU_P1_7,

    HAL_GPIO_PIN_PMU_P2_0 = HAL_IOMUX_PIN_PMU_P2_0,
    HAL_GPIO_PIN_PMU_P2_1 = HAL_IOMUX_PIN_PMU_P2_1,
    HAL_GPIO_PIN_PMU_P2_2 = HAL_IOMUX_PIN_PMU_P2_2,
    HAL_GPIO_PIN_PMU_P2_3 = HAL_IOMUX_PIN_PMU_P2_3,
    HAL_GPIO_PIN_PMU_P2_4 = HAL_IOMUX_PIN_PMU_P2_4,
    HAL_GPIO_PIN_PMU_P2_5 = HAL_IOMUX_PIN_PMU_P2_5,
    HAL_GPIO_PIN_PMU_P2_6 = HAL_IOMUX_PIN_PMU_P2_6,
    HAL_GPIO_PIN_PMU_P2_7 = HAL_IOMUX_PIN_PMU_P2_7,

    HAL_GPIO_PIN_PMU_P3_0 = HAL_IOMUX_PIN_PMU_P3_0,
    HAL_GPIO_PIN_PMU_P3_1 = HAL_IOMUX_PIN_PMU_P3_1,
    HAL_GPIO_PIN_PMU_P3_2 = HAL_IOMUX_PIN_PMU_P3_2,
    HAL_GPIO_PIN_PMU_P3_3 = HAL_IOMUX_PIN_PMU_P3_3,
    HAL_GPIO_PIN_PMU_P3_4 = HAL_IOMUX_PIN_PMU_P3_4,
    HAL_GPIO_PIN_PMU_P3_5 = HAL_IOMUX_PIN_PMU_P3_5,
    HAL_GPIO_PIN_PMU_P3_6 = HAL_IOMUX_PIN_PMU_P3_6,
    HAL_GPIO_PIN_PMU_P3_7 = HAL_IOMUX_PIN_PMU_P3_7,

    HAL_GPIO_PIN_PMU_P4_0 = HAL_IOMUX_PIN_PMU_P4_0,
    HAL_GPIO_PIN_PMU_P4_1 = HAL_IOMUX_PIN_PMU_P4_1,
    HAL_GPIO_PIN_PMU_P4_2 = HAL_IOMUX_PIN_PMU_P4_2,
    HAL_GPIO_PIN_PMU_P4_3 = HAL_IOMUX_PIN_PMU_P4_3,
    HAL_GPIO_PIN_PMU_P4_4 = HAL_IOMUX_PIN_PMU_P4_4,
    HAL_GPIO_PIN_PMU_P4_5 = HAL_IOMUX_PIN_PMU_P4_5,
    HAL_GPIO_PIN_PMU_P4_6 = HAL_IOMUX_PIN_PMU_P4_6,
    HAL_GPIO_PIN_PMU_P4_7 = HAL_IOMUX_PIN_PMU_P4_7,

    HAL_GPIO_PIN_PMU_P5_0 = HAL_IOMUX_PIN_PMU_P5_0,
    HAL_GPIO_PIN_PMU_P5_1 = HAL_IOMUX_PIN_PMU_P5_1,
    HAL_GPIO_PIN_PMU_P5_2 = HAL_IOMUX_PIN_PMU_P5_2,
    HAL_GPIO_PIN_PMU_P5_3 = HAL_IOMUX_PIN_PMU_P5_3,
    HAL_GPIO_PIN_PMU_P5_4 = HAL_IOMUX_PIN_PMU_P5_4,
    HAL_GPIO_PIN_PMU_P5_5 = HAL_IOMUX_PIN_PMU_P5_5,
    HAL_GPIO_PIN_PMU_P5_6 = HAL_IOMUX_PIN_PMU_P5_6,
    HAL_GPIO_PIN_PMU_P5_7 = HAL_IOMUX_PIN_PMU_P5_7,

    HAL_GPIO_PIN_PMU_P6_0 = HAL_IOMUX_PIN_PMU_P6_0,
    HAL_GPIO_PIN_PMU_P6_1 = HAL_IOMUX_PIN_PMU_P6_1,
    HAL_GPIO_PIN_PMU_P6_2 = HAL_IOMUX_PIN_PMU_P6_2,
    HAL_GPIO_PIN_PMU_P6_3 = HAL_IOMUX_PIN_PMU_P6_3,
    HAL_GPIO_PIN_PMU_P6_4 = HAL_IOMUX_PIN_PMU_P6_4,
    HAL_GPIO_PIN_PMU_P6_5 = HAL_IOMUX_PIN_PMU_P6_5,
    HAL_GPIO_PIN_PMU_P6_6 = HAL_IOMUX_PIN_PMU_P6_6,
    HAL_GPIO_PIN_PMU_P6_7 = HAL_IOMUX_PIN_PMU_P6_7,

    HAL_GPIO_PIN_PMU_P7_0 = HAL_IOMUX_PIN_PMU_P7_0,
    HAL_GPIO_PIN_PMU_P7_1 = HAL_IOMUX_PIN_PMU_P7_1,
    HAL_GPIO_PIN_PMU_P7_2 = HAL_IOMUX_PIN_PMU_P7_2,
    HAL_GPIO_PIN_PMU_P7_3 = HAL_IOMUX_PIN_PMU_P7_3,

    HAL_GPIO_PIN_PMU_NUM = HAL_IOMUX_PIN_PMU_NUM
};

enum HAL_IOMUX_FUNCTION_T {
    HAL_IOMUX_FUNC_NONE = 0,
    HAL_IOMUX_FUNC_GPIO,
    HAL_IOMUX_FUNC_AS_GPIO = HAL_IOMUX_FUNC_GPIO,
    HAL_IOMUX_FUNC_BT_UART_RX,
    HAL_IOMUX_FUNC_BT_UART_TX,
    HAL_IOMUX_FUNC_BT_UART_CTS,
    HAL_IOMUX_FUNC_BT_UART_RTS,
    HAL_IOMUX_FUNC_BT_UART_WCI_RX,
    HAL_IOMUX_FUNC_BT_UART_WCI_TX,
    HAL_IOMUX_FUNC_BT_FREQ,
    HAL_IOMUX_FUNC_BTH_UART0_RX,
    HAL_IOMUX_FUNC_BTH_UART0_TX,
    HAL_IOMUX_FUNC_BTH_UART1_RX,
    HAL_IOMUX_FUNC_BTH_UART1_TX,
    HAL_IOMUX_FUNC_BTH_UART1_CTS,
    HAL_IOMUX_FUNC_BTH_UART1_RTS,
    HAL_IOMUX_FUNC_BTH_I2C_M0_SCL,
    HAL_IOMUX_FUNC_BTH_I2C_M0_SDA,
    HAL_IOMUX_FUNC_BTH_I2C_M1_SCL,
    HAL_IOMUX_FUNC_BTH_I2C_M1_SDA,
    HAL_IOMUX_FUNC_BTH_SPI_CLK,
    HAL_IOMUX_FUNC_BTH_SPI_CS0,
    HAL_IOMUX_FUNC_BTH_SPI_CS1,
    HAL_IOMUX_FUNC_BTH_SPI_DI0,
    HAL_IOMUX_FUNC_BTH_SPI_DI1,
    HAL_IOMUX_FUNC_BTH_SPI_DIO,
    HAL_IOMUX_FUNC_BTH_SPILCD_DCN,
    HAL_IOMUX_FUNC_BTH_SPILCD_CLK,
    HAL_IOMUX_FUNC_BTH_SPILCD_CS0,
    HAL_IOMUX_FUNC_BTH_SPILCD_CS1,
    HAL_IOMUX_FUNC_BTH_SPILCD_DI0,
    HAL_IOMUX_FUNC_BTH_SPILCD_DI1,
    HAL_IOMUX_FUNC_BTH_SPILCD_DIO,
    HAL_IOMUX_FUNC_CLK_OUT,
    HAL_IOMUX_FUNC_DNTC_IN,
    HAL_IOMUX_FUNC_DNTC_OUT,
    HAL_IOMUX_FUNC_CLK_REQ_OUT,
    HAL_IOMUX_FUNC_CLK_REQ_IN,
    HAL_IOMUX_FUNC_I2C_SCL,
    HAL_IOMUX_FUNC_I2C_SDA,
    HAL_IOMUX_FUNC_SYS_I2C_M0_SCL,
    HAL_IOMUX_FUNC_SYS_I2C_M0_SDA,
    HAL_IOMUX_FUNC_SYS_I2C_M1_SCL,
    HAL_IOMUX_FUNC_SYS_I2C_M1_SDA,
    HAL_IOMUX_FUNC_SYS_I2S0_SCK,
    HAL_IOMUX_FUNC_SYS_I2S0_SDI0,
    HAL_IOMUX_FUNC_SYS_I2S0_SDO,
    HAL_IOMUX_FUNC_SYS_I2S0_WS,
    HAL_IOMUX_FUNC_SYS_I2S1_SCK,
    HAL_IOMUX_FUNC_SYS_I2S1_SDI0,
    HAL_IOMUX_FUNC_SYS_I2S1_SDO,
    HAL_IOMUX_FUNC_SYS_I2S1_WS,
    HAL_IOMUX_FUNC_SYS_I2S_MCLK,
    HAL_IOMUX_FUNC_SYS_I2S_MCLK_IN,
    HAL_IOMUX_FUNC_SYS_SPI_DCN,
    HAL_IOMUX_FUNC_SYS_SPI_CLK,
    HAL_IOMUX_FUNC_SYS_SPI_CS0,
    HAL_IOMUX_FUNC_SYS_SPI_CS1,
    HAL_IOMUX_FUNC_SYS_SPI_CS2,
    HAL_IOMUX_FUNC_SYS_SPI_DI0,
    HAL_IOMUX_FUNC_SYS_SPI_DI1,
    HAL_IOMUX_FUNC_SYS_SPI_DI2,
    HAL_IOMUX_FUNC_SYS_SPI_DIO,
    HAL_IOMUX_FUNC_SYS_UART0_RX,
    HAL_IOMUX_FUNC_SYS_UART0_TX,
    HAL_IOMUX_FUNC_SYS_UART1_CTS,
    HAL_IOMUX_FUNC_SYS_UART1_RTS,
    HAL_IOMUX_FUNC_SYS_UART1_RX,
    HAL_IOMUX_FUNC_SYS_UART1_TX,
    HAL_IOMUX_FUNC_SYS_QSPI_NAND_CLK,
    HAL_IOMUX_FUNC_SYS_QSPI_NAND_CS,
    HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0,
    HAL_IOMUX_FUNC_SYS_QSPI_NAND_D1,
    HAL_IOMUX_FUNC_SYS_QSPI_NAND_D2,
    HAL_IOMUX_FUNC_SYS_QSPI_NAND_D3,
    HAL_IOMUX_FUNC_PDM0_CK,
    HAL_IOMUX_FUNC_PDM0_D,
    HAL_IOMUX_FUNC_PDM1_CK,
    HAL_IOMUX_FUNC_PDM1_D,
    HAL_IOMUX_FUNC_PDM2_CK,
    HAL_IOMUX_FUNC_PDM2_D,
    HAL_IOMUX_FUNC_PWM0,
    HAL_IOMUX_FUNC_PWM1,
    HAL_IOMUX_FUNC_PWM2,
    HAL_IOMUX_FUNC_PWM3,
    HAL_IOMUX_FUNC_PWM4,
    HAL_IOMUX_FUNC_PWM5,
    HAL_IOMUX_FUNC_PWM6,
    HAL_IOMUX_FUNC_PWM7,
    HAL_IOMUX_FUNC_SDMMC0_CLK,
    HAL_IOMUX_FUNC_SDMMC0_CMD,
    HAL_IOMUX_FUNC_SDMMC0_DATA0,
    HAL_IOMUX_FUNC_SDMMC0_DATA1,
    HAL_IOMUX_FUNC_SDMMC0_DATA2,
    HAL_IOMUX_FUNC_SDMMC0_DATA3,
    HAL_IOMUX_FUNC_SENS_I2C_M0_SCL,
    HAL_IOMUX_FUNC_SENS_I2C_M0_SDA,
    HAL_IOMUX_FUNC_SENS_I2C_M1_SCL,
    HAL_IOMUX_FUNC_SENS_I2C_M1_SDA,
    HAL_IOMUX_FUNC_SENS_I2C_M2_SCL,
    HAL_IOMUX_FUNC_SENS_I2C_M2_SDA,
    HAL_IOMUX_FUNC_SENS_I2C_M3_SCL,
    HAL_IOMUX_FUNC_SENS_I2C_M3_SDA,
    HAL_IOMUX_FUNC_SENS_I2C_M4_SCL,
    HAL_IOMUX_FUNC_SENS_I2C_M4_SDA,
    HAL_IOMUX_FUNC_SENS_I2S0_SCK,
    HAL_IOMUX_FUNC_SENS_I2S0_SDI0,
    HAL_IOMUX_FUNC_SENS_I2S0_SDO,
    HAL_IOMUX_FUNC_SENS_I2S0_WS,
    HAL_IOMUX_FUNC_SENS_I2S_MCLK,
    HAL_IOMUX_FUNC_SENS_I2S_MCLK_IN,
    HAL_IOMUX_FUNC_SENS_SPI_CLK,
    HAL_IOMUX_FUNC_SENS_SPI_CS0,
    HAL_IOMUX_FUNC_SENS_SPI_CS1,
    HAL_IOMUX_FUNC_SENS_SPI_DI0,
    HAL_IOMUX_FUNC_SENS_SPI_DI1,
    HAL_IOMUX_FUNC_SENS_SPI_DIO,
    HAL_IOMUX_FUNC_SENS_SPILCD_CLK,
    HAL_IOMUX_FUNC_SENS_SPILCD_CS0,
    HAL_IOMUX_FUNC_SENS_SPILCD_CS1,
    HAL_IOMUX_FUNC_SENS_SPILCD_DI0,
    HAL_IOMUX_FUNC_SENS_SPILCD_DI1,
    HAL_IOMUX_FUNC_SENS_SPILCD_DIO,
    HAL_IOMUX_FUNC_SENS_UART0_RX,
    HAL_IOMUX_FUNC_SENS_UART0_TX,
    HAL_IOMUX_FUNC_SENS_UART1_CTS,
    HAL_IOMUX_FUNC_SENS_UART1_RTS,
    HAL_IOMUX_FUNC_SENS_UART1_RX,
    HAL_IOMUX_FUNC_SENS_UART1_TX,
    HAL_IOMUX_FUNC_SENS_UART2_RX,
    HAL_IOMUX_FUNC_SENS_UART2_TX,
    HAL_IOMUX_FUNC_SENS_UART2_CTS,
    HAL_IOMUX_FUNC_SENS_UART2_RTS,
    HAL_IOMUX_FUNC_SPDIF0_DI,
    HAL_IOMUX_FUNC_SPDIF0_DO,

    HAL_IOMUX_FUNC_END
};

enum HAL_IOMUX_ISPI_MOD_T {
    HAL_IOMUX_ISPI_MOD_PMU = 0,
    HAL_IOMUX_ISPI_MOD_ANA,
    HAL_IOMUX_ISPI_MOD_RF,
    HAL_IOMUX_ISPI_MOD_XTAL,
    HAL_IOMUX_ISPI_MOD_RC,
    HAL_IOMUX_ISPI_MOD_EMMC,
    HAL_IOMUX_ISPI_MOD_PCIE,
    HAL_IOMUX_ISPI_MOD_USB,
    HAL_IOMUX_ISPI_MOD_ITNPMU,
};

enum HAL_IOMUX_ISPI_OWNER_T {
    HAL_IOMUX_ISPI_OWNER_SYS = 0,
    HAL_IOMUX_ISPI_OWNER_SENS,
    HAL_IOMUX_ISPI_OWNER_BTH,
};

#define HAL_IOMUX_ISPI_ACCESS_T

uint32_t iomux_lock(void);

void iomux_unlock(uint32_t lock);

void hal_iomux_ispi_access_enable(enum HAL_IOMUX_ISPI_MOD_T mod, enum HAL_IOMUX_ISPI_OWNER_T owner);

void hal_iomux_set_i2s_mclk(void);

void hal_iomux_set_i2s1(void);

void hal_iomux_set_mcu_clock_out(void);

void hal_iomux_set_bt_clock_out(void);

int hal_iomux_tportopen(void);

int hal_iomux_tportclr(int port);

int hal_iomux_tportset(int port);

void hal_iomux_set_spi_slave(void);

void hal_iomux_clear_spi_slave(void);

void hal_iomux_set_sens_uart0(void);

void hal_iomux_set_sens_uart1(void);

void hal_iomux_set_sens_uart2(void);

void hal_iomux_set_sens_i2s0(void);

void hal_iomux_set_btc_uart0(void);

void hal_iomux_set_sys_uart0(void);

void hal_iomux_bt_3wire_tports_init(void);

void hal_iomux_set_sens_i2c2(void);

void hal_iomux_set_sens_i2c3(void);

void hal_iomux_set_sens_i2c4(void);

uint32_t hal_iomux_set_io_drv(enum HAL_IOMUX_PIN_T pin, uint32_t val);

void hal_iomux_set_sleep_tport(void);

void hal_iomux_set_antenna_sw(void);

void hal_iomux_set_bth_spi_capsensor(void);

void hal_iomux_set_sens_spi(void);

#ifdef __cplusplus
}
#endif

#endif
