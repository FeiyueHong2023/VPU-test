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
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_aoncmu)
#include CHIP_SPECIFIC_HDR(reg_aoniomux)
#include "cmsis.h"
#include "hal_chipid.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_memsc.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_uart.h"
#include "pmu.h"
#include "tgt_hardware.h"
#ifdef I2C_RECOVERY
#include "hal_i2c.h"
#include "string.h"
#endif

#define MEMSC_ID_IOMUX                      HAL_MEMSC_AON_ID_0

#define UART0_VOLTAGE_SEL                   HAL_IOMUX_PIN_VOLTAGE_MEM
#define UART2_VOLTAGE_SEL                   HAL_IOMUX_PIN_VOLTAGE_MEM

#ifdef I2S0_VOLTAGE_VMEM
#define I2S0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2S0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2S1_VOLTAGE_VMEM
#define I2S1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2S1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPDIF0_VOLTAGE_VMEM
#define SPDIF0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPDIF0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef DIGMIC_VOLTAGE_VMEM
#define DIGMIC_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define DIGMIC_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPI_VOLTAGE_VMEM
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPILCD_VOLTAGE_VMEM
#define SPILCD_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPILCD_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2C0_VOLTAGE_VMEM
#define I2C0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2C0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2C1_VOLTAGE_VMEM
#define I2C1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2C1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#define I2C2_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM

#define I2C3_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM

#ifdef CLKOUT_VOLTAGE_VMEM
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SDMMC0_VOLTAGE_VMEM
#define SDMMC0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SDMMC0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SDMMC0_PULLUP_ENABLE
#define SDMMC0_PULLUP_SEL                   HAL_IOMUX_PIN_PULLUP_ENABLE
#else
#define SDMMC0_PULLUP_SEL                   HAL_IOMUX_PIN_NOPULL
#endif

#ifndef SDMMC0_IOMUX_INDEX
#define SDMMC0_IOMUX_INDEX                  0
#endif

#ifndef I2S0_IOMUX_INDEX
#define I2S0_IOMUX_INDEX                    0
#endif

#ifndef I2S1_IOMUX_INDEX
#define I2S1_IOMUX_INDEX                    0
#endif

#ifndef I2S_MCLK_IOMUX_INDEX
#define I2S_MCLK_IOMUX_INDEX                0
#endif

#ifndef SPDIF0_IOMUX_INDEX
#define SPDIF0_IOMUX_INDEX                  0
#endif

#ifndef DIG_MIC2_CK_IOMUX_INDEX
#define DIG_MIC2_CK_IOMUX_INDEX             0
#endif

#ifndef DIG_MIC3_CK_IOMUX_INDEX
#define DIG_MIC3_CK_IOMUX_INDEX             0
#endif

#ifndef DIG_MIC_CK_IOMUX_PIN
#define DIG_MIC_CK_IOMUX_PIN                HAL_IOMUX_PIN_P5_2
#endif

#ifndef DIG_MIC_CK1_IOMUX_PIN
#define DIG_MIC_CK1_IOMUX_PIN               0
#endif

#ifndef DIG_MIC_CK2_IOMUX_PIN
#define DIG_MIC_CK2_IOMUX_PIN               0
#endif

#ifndef DIG_MIC_D0_IOMUX_PIN
#define DIG_MIC_D0_IOMUX_PIN                HAL_IOMUX_PIN_P5_3
#endif

#ifndef DIG_MIC_D1_IOMUX_PIN
#define DIG_MIC_D1_IOMUX_PIN                HAL_IOMUX_PIN_P5_5
#endif

#ifndef DIG_MIC_D2_IOMUX_PIN
#define DIG_MIC_D2_IOMUX_PIN                HAL_IOMUX_PIN_P5_7
#endif

#ifndef I2C0_IOMUX_INDEX
#define I2C0_IOMUX_INDEX                    0
#endif

#ifndef I2C1_IOMUX_INDEX
#define I2C1_IOMUX_INDEX                    0
#endif

#ifndef CLKOUT_IOMUX_INDEX
#define CLKOUT_IOMUX_INDEX                  0
#endif

#ifndef JTAG_IOMUX_INDEX
#define JTAG_IOMUX_INDEX                    0
#endif

#ifndef UART0_IOMUX_INDEX
#define UART0_IOMUX_INDEX                   0
#endif

#ifndef UART1_IOMUX_INDEX
#define UART1_IOMUX_INDEX                   0
#endif

#ifndef UART2_IOMUX_INDEX
#define UART2_IOMUX_INDEX                   0
#endif

#ifndef RXON_IOMUX_INDEX
#define RXON_IOMUX_INDEX                    0
#endif

#ifndef TXON_IOMUX_INDEX
#define TXON_IOMUX_INDEX                    0
#endif

#define IOMUX_FUNC_VAL_GPIO                 0

#define IOMUX_ALT_FUNC_NUM                  6

// Other func values: 0 -> gpio, 5 -> bt, 6 -> sdmmc, 7 -> ana_test
static const uint8_t index_to_func_val[IOMUX_ALT_FUNC_NUM] = { 1, 2, 3, 4, 5, 6};

static const enum HAL_IOMUX_FUNCTION_T pin_func_map[HAL_IOMUX_PIN_NUM][IOMUX_ALT_FUNC_NUM] = {
    // P0_0
    { HAL_IOMUX_FUNC_SENS_UART1_RX, HAL_IOMUX_FUNC_SYS_I2S0_SCK, HAL_IOMUX_FUNC_SYS_I2C_M0_SCL, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM0_CK, HAL_IOMUX_FUNC_SENS_I2S0_SCK, },
    // P0_1
    { HAL_IOMUX_FUNC_SENS_UART1_TX, HAL_IOMUX_FUNC_SYS_I2S0_WS, HAL_IOMUX_FUNC_SYS_I2C_M0_SDA, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_SENS_I2S0_WS, },
    // P0_2
    { HAL_IOMUX_FUNC_SENS_UART1_CTS, HAL_IOMUX_FUNC_SYS_I2S0_SDI0, HAL_IOMUX_FUNC_SYS_I2C_M1_SCL, HAL_IOMUX_FUNC_BTH_UART1_RX, HAL_IOMUX_FUNC_PDM1_CK, HAL_IOMUX_FUNC_SENS_I2S0_SDI0, },
    // P0_3
    { HAL_IOMUX_FUNC_SENS_UART1_RTS, HAL_IOMUX_FUNC_SYS_I2S0_SDO, HAL_IOMUX_FUNC_SYS_I2C_M1_SDA, HAL_IOMUX_FUNC_BTH_UART1_TX, HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_SENS_I2S0_SDO, },
    // P0_4
    { HAL_IOMUX_FUNC_SENS_I2C_M0_SCL, HAL_IOMUX_FUNC_SPDIF0_DI, HAL_IOMUX_FUNC_BTH_I2C_M0_SCL, HAL_IOMUX_FUNC_BTH_I2C_M1_SCL, HAL_IOMUX_FUNC_BT_UART_CTS, HAL_IOMUX_FUNC_SENS_I2S_MCLK_IN, },
    // P0_5
    { HAL_IOMUX_FUNC_SENS_I2C_M0_SDA, HAL_IOMUX_FUNC_SPDIF0_DO, HAL_IOMUX_FUNC_BTH_I2C_M0_SDA, HAL_IOMUX_FUNC_BTH_I2C_M1_SDA, HAL_IOMUX_FUNC_BT_UART_RTS, HAL_IOMUX_FUNC_NONE, },
    // P0_6
    { HAL_IOMUX_FUNC_SENS_I2C_M1_SCL, HAL_IOMUX_FUNC_CLK_REQ_OUT, HAL_IOMUX_FUNC_SYS_I2S1_SCK, HAL_IOMUX_FUNC_SENS_UART0_RX, HAL_IOMUX_FUNC_BT_UART_RX, HAL_IOMUX_FUNC_NONE, },
    // P0_7
    { HAL_IOMUX_FUNC_SENS_I2C_M1_SDA, HAL_IOMUX_FUNC_CLK_REQ_IN, HAL_IOMUX_FUNC_SYS_I2S1_WS, HAL_IOMUX_FUNC_SENS_UART0_TX, HAL_IOMUX_FUNC_BT_UART_TX, HAL_IOMUX_FUNC_NONE, },
    // P1_0
    { HAL_IOMUX_FUNC_SENS_SPI_CLK, HAL_IOMUX_FUNC_SYS_I2S1_SCK, HAL_IOMUX_FUNC_SENS_UART1_RX, HAL_IOMUX_FUNC_SYS_I2C_M1_SCL, HAL_IOMUX_FUNC_BTH_UART0_RX, HAL_IOMUX_FUNC_BT_UART_RX, },
    // P1_1
    { HAL_IOMUX_FUNC_SENS_SPI_CS0, HAL_IOMUX_FUNC_SYS_I2S1_WS, HAL_IOMUX_FUNC_SENS_UART1_TX, HAL_IOMUX_FUNC_SYS_I2C_M1_SDA, HAL_IOMUX_FUNC_BTH_UART0_TX, HAL_IOMUX_FUNC_BT_UART_TX, },
    // P1_2
    { HAL_IOMUX_FUNC_SENS_SPI_DI0, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, HAL_IOMUX_FUNC_SENS_UART1_CTS, HAL_IOMUX_FUNC_SENS_I2S_MCLK, HAL_IOMUX_FUNC_SENS_SPILCD_DI1, HAL_IOMUX_FUNC_SDMMC0_CLK, },
    // P1_3
    { HAL_IOMUX_FUNC_SENS_SPI_DIO, HAL_IOMUX_FUNC_SYS_I2S1_SDO, HAL_IOMUX_FUNC_SENS_UART1_RTS, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_SENS_SPILCD_CS1, HAL_IOMUX_FUNC_SDMMC0_CMD, },
    // P1_4
    { HAL_IOMUX_FUNC_SENS_SPI_CLK, HAL_IOMUX_FUNC_SYS_SPI_CLK, HAL_IOMUX_FUNC_PDM0_CK, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM0_CK, HAL_IOMUX_FUNC_SDMMC0_DATA0, },
    // P1_5
    { HAL_IOMUX_FUNC_SENS_SPI_CS0, HAL_IOMUX_FUNC_SYS_SPI_CS0, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_SDMMC0_DATA1, },
    // P1_6
    { HAL_IOMUX_FUNC_SENS_SPI_DI0, HAL_IOMUX_FUNC_SYS_SPI_DI0, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM1_CK, HAL_IOMUX_FUNC_SDMMC0_DATA2, },
    // P1_7
    { HAL_IOMUX_FUNC_SENS_SPI_DIO, HAL_IOMUX_FUNC_SYS_SPI_DIO, HAL_IOMUX_FUNC_SYS_I2S1_SDO, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_SDMMC0_DATA3, },
    // P2_0
    { HAL_IOMUX_FUNC_SENS_I2C_M2_SCL, HAL_IOMUX_FUNC_SYS_UART1_RX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_I2C_M4_SCL, HAL_IOMUX_FUNC_BT_UART_CTS, HAL_IOMUX_FUNC_NONE, },
    // P2_1
    { HAL_IOMUX_FUNC_SENS_I2C_M2_SDA, HAL_IOMUX_FUNC_SYS_UART1_TX, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_SENS_I2C_M4_SDA, HAL_IOMUX_FUNC_BT_UART_RTS, HAL_IOMUX_FUNC_NONE, },
    // P2_2
    { HAL_IOMUX_FUNC_SENS_UART0_RX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SYS_UART0_RX, HAL_IOMUX_FUNC_BTH_UART0_RX, HAL_IOMUX_FUNC_BT_UART_RX, HAL_IOMUX_FUNC_NONE, },
    // P2_3
    { HAL_IOMUX_FUNC_SENS_UART0_TX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SYS_UART0_TX, HAL_IOMUX_FUNC_BTH_UART0_TX, HAL_IOMUX_FUNC_BT_UART_TX, HAL_IOMUX_FUNC_NONE, },
    // P2_4
    { HAL_IOMUX_FUNC_SENS_SPILCD_CLK, HAL_IOMUX_FUNC_SENS_I2C_M2_SCL, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BT_FREQ, HAL_IOMUX_FUNC_BTH_SPI_CLK, HAL_IOMUX_FUNC_NONE, },
    // P2_5
    { HAL_IOMUX_FUNC_SENS_SPILCD_CS0, HAL_IOMUX_FUNC_SENS_I2C_M2_SDA, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPI_CS0, HAL_IOMUX_FUNC_NONE, },
    // P2_6
    { HAL_IOMUX_FUNC_SENS_SPILCD_DI0, HAL_IOMUX_FUNC_SENS_I2C_M3_SCL, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPI_DI0, HAL_IOMUX_FUNC_NONE, },
    // P2_7
    { HAL_IOMUX_FUNC_SENS_SPILCD_DIO, HAL_IOMUX_FUNC_SENS_I2C_M3_SDA, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BT_FREQ, HAL_IOMUX_FUNC_BTH_SPI_DIO, HAL_IOMUX_FUNC_NONE, },
    // P3_0
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPI_CLK, HAL_IOMUX_FUNC_DNTC_IN, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CLK, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_UART2_RX, },
    // P3_1
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPI_CS0, HAL_IOMUX_FUNC_DNTC_OUT, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CS, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_UART2_TX, },
    // P3_2
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPI_DI0, HAL_IOMUX_FUNC_SYS_UART0_RX, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_UART2_CTS, },
    // P3_3
    { HAL_IOMUX_FUNC_BT_FREQ, HAL_IOMUX_FUNC_BTH_SPI_DIO, HAL_IOMUX_FUNC_SYS_UART0_TX, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D1, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_UART2_RTS, },
    // P3_4
    { HAL_IOMUX_FUNC_SYS_I2C_M0_SCL, HAL_IOMUX_FUNC_SENS_I2C_M2_SCL, HAL_IOMUX_FUNC_BTH_I2C_M0_SCL, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D2, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM2_CK, },
    // P3_5
    { HAL_IOMUX_FUNC_SYS_I2C_M0_SDA, HAL_IOMUX_FUNC_SENS_I2C_M2_SDA, HAL_IOMUX_FUNC_BTH_I2C_M0_SDA, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D3, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM2_D, },
    // P3_6
    { HAL_IOMUX_FUNC_SYS_I2S_MCLK_IN, HAL_IOMUX_FUNC_SENS_I2C_M3_SCL, HAL_IOMUX_FUNC_BTH_I2C_M1_SCL, HAL_IOMUX_FUNC_SYS_I2S_MCLK, HAL_IOMUX_FUNC_BTH_SPI_CS1, HAL_IOMUX_FUNC_BTH_SPILCD_CS1, },
    // P3_7
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_I2C_M3_SDA, HAL_IOMUX_FUNC_BTH_I2C_M1_SDA, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_BTH_SPI_DI1, HAL_IOMUX_FUNC_BTH_SPILCD_DI1, },
    // P4_0
    { HAL_IOMUX_FUNC_SYS_I2S0_SCK, HAL_IOMUX_FUNC_SYS_I2S1_SCK, HAL_IOMUX_FUNC_SENS_UART2_CTS, HAL_IOMUX_FUNC_BTH_UART1_CTS, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CLK, },
    // P4_1
    { HAL_IOMUX_FUNC_SYS_I2S0_WS, HAL_IOMUX_FUNC_SYS_I2S1_WS, HAL_IOMUX_FUNC_SENS_UART2_RTS, HAL_IOMUX_FUNC_BTH_UART1_RTS, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CS, },
    // P4_2
    { HAL_IOMUX_FUNC_SYS_I2S0_SDI0, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, HAL_IOMUX_FUNC_SENS_UART2_RX, HAL_IOMUX_FUNC_BTH_UART1_RX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, },
    // P4_3
    { HAL_IOMUX_FUNC_SYS_I2S0_SDO, HAL_IOMUX_FUNC_SYS_I2S1_SDO, HAL_IOMUX_FUNC_SENS_UART2_TX, HAL_IOMUX_FUNC_BTH_UART1_TX, HAL_IOMUX_FUNC_BT_FREQ, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D1, },
    // P4_4
    { HAL_IOMUX_FUNC_SYS_I2S0_SCK, HAL_IOMUX_FUNC_SYS_I2S1_SCK, HAL_IOMUX_FUNC_BTH_I2C_M0_SCL, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPILCD_CLK, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D2, },
    // P4_5
    { HAL_IOMUX_FUNC_SYS_I2S0_WS, HAL_IOMUX_FUNC_SYS_I2S1_WS, HAL_IOMUX_FUNC_BTH_I2C_M0_SDA, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPILCD_CS0, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D3, },
    // P4_6
    { HAL_IOMUX_FUNC_SYS_I2S0_SDI0, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, HAL_IOMUX_FUNC_BTH_I2C_M1_SCL, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPILCD_DI0, HAL_IOMUX_FUNC_SPDIF0_DI, },
    // P4_7
    { HAL_IOMUX_FUNC_SYS_I2S0_SDO, HAL_IOMUX_FUNC_SYS_I2S1_SDO, HAL_IOMUX_FUNC_BTH_I2C_M1_SDA, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_BTH_SPILCD_DIO, HAL_IOMUX_FUNC_SPDIF0_DO, },
    // P5_0
    { HAL_IOMUX_FUNC_SYS_I2S1_SCK, HAL_IOMUX_FUNC_SENS_UART1_RX, HAL_IOMUX_FUNC_SYS_UART1_RX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_I2S0_SCK, HAL_IOMUX_FUNC_NONE, },
    // P5_1
    { HAL_IOMUX_FUNC_SYS_I2S1_WS, HAL_IOMUX_FUNC_SENS_UART1_TX, HAL_IOMUX_FUNC_SYS_UART1_TX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_I2S0_WS, HAL_IOMUX_FUNC_NONE, },
    // P5_2
    { HAL_IOMUX_FUNC_SYS_I2S1_SDI0, HAL_IOMUX_FUNC_SENS_UART1_CTS, HAL_IOMUX_FUNC_SYS_UART1_CTS, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_I2S0_SDI0, HAL_IOMUX_FUNC_PDM0_CK, },
    // P5_3
    { HAL_IOMUX_FUNC_SYS_I2S1_SDO, HAL_IOMUX_FUNC_SENS_UART1_RTS, HAL_IOMUX_FUNC_SYS_UART1_RTS, HAL_IOMUX_FUNC_BT_FREQ, HAL_IOMUX_FUNC_SENS_I2S0_SDO, HAL_IOMUX_FUNC_PDM0_D, },
    // P5_4
    { HAL_IOMUX_FUNC_SYS_UART1_RX, HAL_IOMUX_FUNC_SYS_I2C_M1_SCL, HAL_IOMUX_FUNC_SENS_I2C_M4_SCL, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_UART2_RX, HAL_IOMUX_FUNC_PDM1_CK, },
    // P5_5
    { HAL_IOMUX_FUNC_SYS_UART1_TX, HAL_IOMUX_FUNC_SYS_I2C_M1_SDA, HAL_IOMUX_FUNC_SENS_I2C_M4_SDA, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_UART2_TX, HAL_IOMUX_FUNC_PDM1_D, },
    // P5_6
    { HAL_IOMUX_FUNC_SYS_UART1_CTS, HAL_IOMUX_FUNC_SYS_I2C_M0_SCL, HAL_IOMUX_FUNC_SYS_I2S_MCLK, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SENS_UART2_CTS, HAL_IOMUX_FUNC_PDM2_CK, },
    // P5_7
    { HAL_IOMUX_FUNC_SYS_UART1_RTS, HAL_IOMUX_FUNC_SYS_I2C_M0_SDA, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_SYS_I2S_MCLK_IN, HAL_IOMUX_FUNC_SENS_UART2_RTS, HAL_IOMUX_FUNC_PDM2_D, },
};

static struct AONIOMUX_T * const aoniomux = (struct AONIOMUX_T *)AON_IOMUX_BASE;
static struct AONCMU_T * const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck00_pin  = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck01_pin = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck02_pin = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck10_pin  = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK1_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck11_pin = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK1_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck12_pin = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK1_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck20_pin  = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK2_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck21_pin = (enum HAL_IOMUX_PIN_T)DIG_MIC_CK2_IOMUX_PIN;

static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_d0_pin = DIG_MIC_D0_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_d1_pin = DIG_MIC_D1_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_d2_pin = DIG_MIC_D2_IOMUX_PIN;

// If there is CP, got_memsc should be an array (one entry for one CPU)
#if !defined(ROM_BUILD) && !defined(PROGRAMMER) || defined(IOMUX_LOCK_ENABLE)
BOOT_BSS_LOC
static bool got_memsc;
#endif

BOOT_TEXT_SRAM_LOC
uint32_t iomux_lock(void)
{
#if (defined(ROM_BUILD) || defined(PROGRAMMER)) && !defined(IOMUX_LOCK_ENABLE)
    return 0;
#else
    uint32_t lock;

    lock = int_lock();
    if (!got_memsc) {
        while (hal_memsc_aon_lock(MEMSC_ID_IOMUX) == 0);
        got_memsc = true;
        // lock is bit[0] when INT_LOCK_EXCEPTION=0 (for PRIMASK),
        // and bit[7:0] when INT_LOCK_EXCEPTION=1 (for BASEPRI)
        lock |= (uint32_t)(1 << 31);
    }

    return lock;
#endif
}

BOOT_TEXT_SRAM_LOC
void iomux_unlock(uint32_t lock)
{
#if defined(ROM_BUILD) || defined(PROGRAMMER) && !defined(IOMUX_LOCK_ENABLE)
    return;
#else
    if (lock & (uint32_t)(1 << 31)) {
        lock &= ~(1 << 31);
        got_memsc = false;
        hal_memsc_aon_unlock(MEMSC_ID_IOMUX);
    }
    int_unlock(lock);
#endif
}

void BOOT_TEXT_FLASH_LOC hal_iomux_set_default_config(void)
{
    uint32_t i;
    uint32_t idx;
    volatile uint32_t *reg_sel;
    volatile uint32_t *reg_pu;
    volatile uint32_t *reg_pd;
    uint32_t sel_idx;
    uint32_t pu_idx;
    uint32_t pd_idx;

#if !defined(MCU_SPI_SLAVE) && !defined(CODEC_APP)
    // Clear spi slave pin mux
    aoniomux->REG_008 = ~0UL;
    // Disable spi slave irq
    aoniomux->REG_070 = 0;
#endif

    // Set all unused GPIOs to pull-down by default
    for (i = 0; i < HAL_IOMUX_PIN_NUM; i++) {
        if (i < HAL_IOMUX_PIN_P3_0) {
            idx = i;
            reg_sel = &aoniomux->REG_004 + idx / 8;
            reg_pu = &aoniomux->REG_02C;
            reg_pd = &aoniomux->REG_030;
            sel_idx = (idx % 8) * 4;
            pu_idx = idx;
            pd_idx = idx;
        } else {
            idx = i - HAL_IOMUX_PIN_P3_0;
            reg_sel = &aoniomux->REG_010 + idx / 8;
            reg_pu = &aoniomux->REG_024;
            reg_pd = &aoniomux->REG_028;
            sel_idx = (idx % 8) * 4;
            pu_idx = idx;
            pd_idx = idx;
        }
        if (((*reg_sel & (0xF << sel_idx)) >> sel_idx) == 0xF) {
            *reg_pu &= ~(1 << pu_idx);
            *reg_pd |= (1 << pd_idx);
            if (i >= HAL_IOMUX_PIN_P3_0) {
                if (i < HAL_IOMUX_PIN_P4_0) {
                    pu_idx = i - HAL_IOMUX_PIN_P3_0;
                    pd_idx = i - HAL_IOMUX_PIN_P3_0 + 8;
                    reg_pu = &aoncmu->RAM_POFF0;
                } else if (i < HAL_IOMUX_PIN_P5_0) {
                    pu_idx = i - HAL_IOMUX_PIN_P4_0 + 16;
                    pd_idx = i - HAL_IOMUX_PIN_P4_0 + 24;
                    reg_pu = &aoncmu->RAM_POFF0;
                } else {
                    pu_idx = i - HAL_IOMUX_PIN_P5_0;
                    pd_idx = i - HAL_IOMUX_PIN_P5_0 + 8;
                    reg_pu = &aoncmu->RAM2_POFF0;
                }
                *reg_pu = (*reg_pu & ~(1 << pu_idx)) | (1 << pd_idx);
            }
        }
    }

    for (i = HAL_IOMUX_PIN_P0_0; i < HAL_IOMUX_PIN_NUM; i++) {
        hal_iomux_set_io_drv(i, 6);
    }

    hal_iomux_ispi_access_init();
    __DSB();
}

uint32_t hal_iomux_check(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; ++i) {
    }
    return 0;
}

uint32_t hal_iomux_init(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i;
    uint32_t ret;
    uint32_t lock;

    ret = 0;

    lock = iomux_lock();

    for (i = 0; i < count; ++i) {
        ret = hal_iomux_set_function(map[i].pin, map[i].function, HAL_IOMUX_OP_CLEAN_OTHER_FUNC_BIT);
        if (ret) {
            ret = (i << 8) + 1;
            goto _exit;
        }
        ret = hal_iomux_set_io_voltage_domains(map[i].pin, map[i].volt);
        if (ret) {
            ret = (i << 8) + 2;
            goto _exit;
        }
        ret = hal_iomux_set_io_pull_select(map[i].pin, map[i].pull_sel);
        if (ret) {
            ret = (i << 8) + 3;
            goto _exit;
        }
    }

_exit:
    iomux_unlock(lock);

    return ret;
}

#ifdef ANC_PROD_TEST
void hal_iomux_set_dig_mic_clock_pin(enum HAL_IOMUX_PIN_T pin)
{
    if (pin == HAL_IOMUX_PIN_P0_0) {
        digmic_ck00_pin = pin;
    } else if (pin == HAL_IOMUX_PIN_P1_4) {
        digmic_ck01_pin = pin;
    } else if (pin == HAL_IOMUX_PIN_P5_2) {
        digmic_ck02_pin = pin;
    } else if (pin == HAL_IOMUX_PIN_P0_2) {
        digmic_ck10_pin = pin;
    } else if (pin == HAL_IOMUX_PIN_P1_6) {
        digmic_ck11_pin = pin;
    } else if (pin == HAL_IOMUX_PIN_P5_4) {
        digmic_ck12_pin = pin;
    } else if (pin == HAL_IOMUX_PIN_P3_4) {
        digmic_ck20_pin = pin;
    } else if (pin == HAL_IOMUX_PIN_P5_6) {
        digmic_ck21_pin = pin;
    } 	else if (pin == 0xff) {
        digmic_ck00_pin  = HAL_IOMUX_PIN_P0_0;
        digmic_ck01_pin = HAL_IOMUX_PIN_P1_4;
        digmic_ck02_pin = HAL_IOMUX_PIN_P5_2;
        digmic_ck10_pin  = HAL_IOMUX_PIN_P0_2;
        digmic_ck11_pin = HAL_IOMUX_PIN_P1_6;
        digmic_ck12_pin = HAL_IOMUX_PIN_P5_4;
        digmic_ck20_pin  = HAL_IOMUX_PIN_P3_4;
        digmic_ck21_pin = HAL_IOMUX_PIN_P5_6;
    }
}

void hal_iomux_set_dig_mic_data0_pin(enum HAL_IOMUX_PIN_T pin)
{
    digmic_d0_pin = pin;
}

void hal_iomux_set_dig_mic_data1_pin(enum HAL_IOMUX_PIN_T pin)
{
    digmic_d1_pin = pin;
}

void hal_iomux_set_dig_mic_data2_pin(enum HAL_IOMUX_PIN_T pin)
{
    digmic_d2_pin = pin;
}
#endif

uint32_t hal_iomux_set_function(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_FUNCTION_T func, enum HAL_IOMUX_OP_TYPE_T type)
{
    int i;
    uint8_t val;
    __IO uint32_t *reg;
    uint32_t shift;

    if (pin >= HAL_IOMUX_PIN_LED_NUM) {
        return 1;
    }
    if (func >= HAL_IOMUX_FUNC_END) {
        return 2;
    }

    if (pin == HAL_IOMUX_PIN_P2_2 || pin == HAL_IOMUX_PIN_P2_3) {
        if (func ==  HAL_IOMUX_FUNC_I2C_SCL || func == HAL_IOMUX_FUNC_I2C_SDA) {
            // Enable analog I2C slave
            aoniomux->REG_048 &= ~IOMUX_AON_GPIO_I2C_MODE;
            // Set mcu GPIO func
            aoniomux->REG_00C = (aoniomux->REG_00C & ~(IOMUX_AON_GPIO_P22_SEL_MASK | IOMUX_AON_GPIO_P23_SEL_MASK)) |
                IOMUX_AON_GPIO_P22_SEL(IOMUX_FUNC_VAL_GPIO) | IOMUX_AON_GPIO_P23_SEL(IOMUX_FUNC_VAL_GPIO);
            return 0;
        } else {
            aoniomux->REG_048 |= IOMUX_AON_GPIO_I2C_MODE;
            // Continue to set the alt func
        }
    } else if ((HAL_IOMUX_PIN_LED1 <= pin) && (pin < HAL_IOMUX_PIN_LED_NUM)) {
        ASSERT(func == HAL_IOMUX_FUNC_GPIO, "Bad func=%d for IOMUX pin=%d", func, pin);
        return 0;
    } else if (pin == HAL_IOMUX_PIN_P2_6) {
        if (func == HAL_IOMUX_FUNC_BTH_UART1_RX) {
            // Enable bth uart1 rx
            aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P26_SEL, 7);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_P2_7) {
        if (func == HAL_IOMUX_FUNC_BTH_UART1_TX) {
            // Enable bth uart1 tx
            aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P27_SEL, 7);
            return 0;
        }
    }

    if (pin >= ARRAY_SIZE(pin_func_map)) {
        return 0;
    }

    if (func == HAL_IOMUX_FUNC_GPIO) {
        val = IOMUX_FUNC_VAL_GPIO;
    } else {
        for (i = 0; i < IOMUX_ALT_FUNC_NUM; i++) {
            if (pin_func_map[pin][i] == func) {
                break;
            }
        }

        if (i == IOMUX_ALT_FUNC_NUM) {
            return 3;
        }
        val = index_to_func_val[i];
    }

    reg = &aoniomux->REG_004 + pin / 8;
    shift = (pin % 8) * 4;

    *reg = (*reg & ~(0xF << shift)) | (val << shift);

    return 0;
}

enum HAL_IOMUX_FUNCTION_T hal_iomux_get_function(enum HAL_IOMUX_PIN_T pin)
{
    return HAL_IOMUX_FUNC_NONE;
}

uint32_t hal_iomux_set_io_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    if (pin >= HAL_IOMUX_PIN_LED_NUM) {
        return 1;
    }

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
    if ((HAL_IOMUX_PIN_LED1 <= pin) && (pin < HAL_IOMUX_PIN_LED_NUM)) {
        pmu_led_set_voltage_domains(pin, volt);
    }
#endif

    return 0;
}

uint32_t hal_iomux_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    volatile uint32_t *reg_pu;
    volatile uint32_t *reg_pd;
    uint32_t pu_idx;
    uint32_t pd_idx;

    if (pin >= HAL_IOMUX_PIN_LED_NUM) {
        return 1;
    }

    if (pin < HAL_IOMUX_PIN_LED1) {
        if (pin >= HAL_IOMUX_PIN_P3_0) {
            uint32_t mask = 0;

            if (pin < HAL_IOMUX_PIN_P4_0) {
                pu_idx = pin - HAL_IOMUX_PIN_P3_0;
                pd_idx = pin - HAL_IOMUX_PIN_P3_0 + 8;
                reg_pu = &aoncmu->RAM_POFF0;
            } else if (pin < HAL_IOMUX_PIN_P5_0) {
                pu_idx = pin - HAL_IOMUX_PIN_P4_0 + 16;
                pd_idx = pin - HAL_IOMUX_PIN_P4_0 + 24;
                reg_pu = &aoncmu->RAM_POFF0;
            } else {
                pu_idx = pin - HAL_IOMUX_PIN_P5_0;
                pd_idx = pin - HAL_IOMUX_PIN_P5_0 + 8;
                reg_pu = &aoncmu->RAM2_POFF0;
            }
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                mask |= (1 << pu_idx);
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                mask |= (1 << pd_idx);
            }
            *reg_pu = (*reg_pu & ~((1 << pu_idx) | (1 << pd_idx))) | mask;
        } else {
            reg_pu = &aoniomux->REG_02C;
            reg_pd = &aoniomux->REG_030;
            pu_idx = pin;
            pd_idx = pin;
            *reg_pu &= ~(1 << pu_idx);
            *reg_pd &= ~(1 << pd_idx);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                *reg_pu |= (1 << pu_idx);
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                *reg_pd |= (1 << pd_idx);
            }
        }
#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
    } else if ((HAL_IOMUX_PIN_LED1 <= pin) && (pin < HAL_IOMUX_PIN_LED_NUM)) {
        pmu_led_set_pull_select(pin, pull_sel);
#endif
    }

    return 0;
}

uint32_t hal_iomux_set_io_drv(enum HAL_IOMUX_PIN_T pin, uint32_t val)
{
    volatile uint32_t *reg_drv;
    uint32_t shift;

    if (pin >= HAL_IOMUX_PIN_PMU_NUM || (pin >= HAL_IOMUX_PIN_NUM && pin <= HAL_IOMUX_PIN_LED_NUM)) {
        return 1;
    }
    if (val > 0xf) {
        return 2;
    }

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
    if (pin >= HAL_IOMUX_PIN_PMU_P0_0 && pin < HAL_IOMUX_PIN_PMU_NUM) {
        return pmu_gpio_set_io_drv(pin, val);
    }
#endif

    if (pin < HAL_IOMUX_PIN_P5_0) {
        reg_drv = &aoniomux->REG_050 + pin / 8;
    } else {
        reg_drv = &aoniomux->REG_07C;
    }
    shift = (pin % 8) * 4;

    *reg_drv = (*reg_drv & ~(0xF << shift)) | (val << shift);
    return 0;
}

void hal_iomux_set_uart0_voltage(enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
}

void hal_iomux_set_uart1_voltage(enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
}

bool hal_iomux_uart0_connected(void)
{
    uint32_t reg_048, reg_00c, reg_02c, reg_030;
    uint32_t mask;
    int val;

    // Save current iomux settings
    reg_048 = aoniomux->REG_048;
    reg_00c = aoniomux->REG_00C;
    reg_02c = aoniomux->REG_02C;
    reg_030 = aoniomux->REG_030;

    // Disable analog I2C slave & master
    aoniomux->REG_048 |= IOMUX_AON_GPIO_I2C_MODE | IOMUX_AON_I2C0_M_SEL_GPIO;
    // Set uart0-rx as gpio
    aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P22_SEL, IOMUX_FUNC_VAL_GPIO);

    mask = (1 << HAL_IOMUX_PIN_P2_2);
    // Clear pullup
    aoniomux->REG_02C &= ~mask;
    // Setup pulldown
    aoniomux->REG_030 |= mask;

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_IN, 0);

    hal_sys_timer_delay(MS_TO_TICKS(2));

    val = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P2_2);

    // Restore iomux settings
    aoniomux->REG_030 = reg_030;
    aoniomux->REG_02C = reg_02c;
    aoniomux->REG_00C = reg_00c;
    aoniomux->REG_048 = reg_048;

    hal_sys_timer_delay(MS_TO_TICKS(2));

    return !!val;
}

bool hal_iomux_uart1_sys_connected(void)
{
    uint32_t reg_018, reg_024;
    int val;
    uint32_t rx_idx;

    // Save current iomux settings
    reg_018 = aoniomux->REG_018;
    reg_024 = aoncmu->RAM2_POFF0;

    // Set uart1-rx as gpio
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S24_SEL, IOMUX_FUNC_VAL_GPIO);

    rx_idx = HAL_IOMUX_PIN_P5_4 - HAL_IOMUX_PIN_P5_0;
    aoncmu->RAM2_POFF0 = (aoncmu->RAM2_POFF0 & ~(1 << rx_idx)) | (1 << (rx_idx + 8));

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P5_4, HAL_GPIO_DIR_IN, 0);

    hal_sys_timer_delay(MS_TO_TICKS(2));

    val = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P5_4);

    // Restore iomux settings
    aoncmu->RAM2_POFF0 = reg_024;
    aoniomux->REG_018 = reg_018;

    hal_sys_timer_delay(MS_TO_TICKS(2));

    return !!val;
}

bool hal_iomux_uart1_sens_connected(void)
{
    uint32_t reg_004, reg_02c, reg_030;
    uint32_t mask;
    int val;

    // Save current iomux settings
    reg_004 = aoniomux->REG_004;
    reg_02c = aoniomux->REG_02C;
    reg_030 = aoniomux->REG_030;

    // Set uart1-rx as gpio
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P00_SEL, IOMUX_FUNC_VAL_GPIO);

    mask = (1 << HAL_IOMUX_PIN_P0_0);
    // Clear pullup
    aoniomux->REG_02C &= ~mask;
    // Setup pulldown
    aoniomux->REG_030 |= mask;

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P0_0, HAL_GPIO_DIR_IN, 0);

    hal_sys_timer_delay(MS_TO_TICKS(2));

    val = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P0_0);

    // Restore iomux settings
    aoniomux->REG_030 = reg_030;
    aoniomux->REG_02C = reg_02c;
    aoniomux->REG_004 = reg_004;

    hal_sys_timer_delay(MS_TO_TICKS(2));

    return !!val;
}

bool hal_iomux_uart1_bth_connected(void)
{
    uint32_t reg_02c, reg_030;
    uint32_t mask;
    int val;
    enum HAL_IOMUX_PIN_T pin_rx;

    // Save current iomux settings
#if (UART1_IOMUX_INDEX == 26)
    pin_rx = HAL_IOMUX_PIN_P2_6;

    uint32_t reg_00c;
    reg_00c = aoniomux->REG_00C;
#else
    pin_rx = HAL_IOMUX_PIN_P0_2;

    uint32_t reg_004;
    reg_004 = aoniomux->REG_004;
#endif
    reg_02c = aoniomux->REG_02C;
    reg_030 = aoniomux->REG_030;

    // Set uart1-rx as gpio
#if (UART1_IOMUX_INDEX == 26)
    aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P26_SEL, IOMUX_FUNC_VAL_GPIO);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P02_SEL, IOMUX_FUNC_VAL_GPIO);
#endif

    mask = (1 << pin_rx);
    // Clear pullup
    aoniomux->REG_02C &= ~mask;
    // Setup pulldown
    aoniomux->REG_030 |= mask;

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pin_rx, HAL_GPIO_DIR_IN, 0);

    hal_sys_timer_delay(MS_TO_TICKS(2));

    val = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)pin_rx);

    // Restore iomux settings
    aoniomux->REG_030 = reg_030;
    aoniomux->REG_02C = reg_02c;
#if (UART1_IOMUX_INDEX == 26)
    aoniomux->REG_00C = reg_00c;
#else
    aoniomux->REG_004 = reg_004;
#endif

    hal_sys_timer_delay(MS_TO_TICKS(2));

    return !!val;
}

bool hal_iomux_uart1_connected(void)
{
#ifdef CHIP_SUBSYS_SENS
    return hal_iomux_uart1_sens_connected();
#elif defined(CHIP_SUBSYS_BTH)
    return hal_iomux_uart1_bth_connected();
#else
    return hal_iomux_uart1_sys_connected();
#endif
}

static void hal_iomux_set_uart0_common(uint8_t func)
{
    uint32_t mask;
    uint32_t lock;

    lock = iomux_lock();

    // Disable analog I2C slave & master
    aoniomux->REG_048 |= IOMUX_AON_GPIO_I2C_MODE | IOMUX_AON_I2C0_M_SEL_GPIO;
    // Set uart0 func
    aoniomux->REG_00C = (aoniomux->REG_00C & ~(IOMUX_AON_GPIO_P22_SEL_MASK | IOMUX_AON_GPIO_P23_SEL_MASK)) |
        IOMUX_AON_GPIO_P22_SEL(func) | IOMUX_AON_GPIO_P23_SEL(func);

    mask = (1 << HAL_IOMUX_PIN_P2_2) | (1 << HAL_IOMUX_PIN_P2_3);
    // Setup pullup
    aoniomux->REG_02C |= (1 << HAL_IOMUX_PIN_P2_2);
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P2_3);
    // Clear pulldown
    aoniomux->REG_030 &= ~mask;

    iomux_unlock(lock);
}

void hal_iomux_set_sys_uart0(void)
{
#if ((UART0_IOMUX_INDEX == 32) && !defined(CHIP_SUBSYS_BTH)) || \
    ((UART2_IOMUX_INDEX == 32) && defined(CHIP_SUBSYS_BTH) && defined(BTH_USE_SYS_PERIPH))
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_SYS_UART0_RX, UART0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_SYS_UART0_TX, UART0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
#else
    hal_iomux_set_uart0_common(3);
#endif
}

void hal_iomux_set_sens_uart0(void)
{
    hal_iomux_set_uart0_common(1);
}

void hal_iomux_set_bth_uart0(void)
{
    hal_iomux_set_uart0_common(4);
}

void hal_iomux_set_btc_uart0(void)
{
    hal_iomux_set_uart0_common(5);
}

void hal_iomux_set_uart0(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_uart0();
#elif defined(CHIP_SUBSYS_BTH)
    hal_iomux_set_bth_uart0();
#else
    hal_iomux_set_sys_uart0();
#endif
}

void hal_iomux_set_sys_uart1(void)
{
    uint32_t lock;
    uint32_t rx_idx;
    uint32_t tx_idx;

    lock = iomux_lock();

    // Set uart1 func
#if (UART1_IOMUX_INDEX == 20)
    uint32_t mask;
    aoniomux->REG_00C= (aoniomux->REG_00C & ~(IOMUX_AON_GPIO_P20_SEL_MASK | IOMUX_AON_GPIO_P21_SEL_MASK)) |
        IOMUX_AON_GPIO_P20_SEL(2) | IOMUX_AON_GPIO_P21_SEL(2);

    mask = (1 << HAL_IOMUX_PIN_P2_0) | (1 << HAL_IOMUX_PIN_P2_1);
    // Setup pullup
    aoniomux->REG_02C |= (1 << HAL_IOMUX_PIN_P2_0);
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P2_1);
    // Clear pulldown
    aoniomux->REG_030 &= ~mask;
#else
    aoniomux->REG_018 = (aoniomux->REG_018 & ~(IOMUX_AON_GPIO_S24_SEL_MASK | IOMUX_AON_GPIO_S25_SEL_MASK)) |
        IOMUX_AON_GPIO_S24_SEL(1) | IOMUX_AON_GPIO_S25_SEL(1);

    rx_idx = HAL_IOMUX_PIN_P5_4 - HAL_IOMUX_PIN_P5_0;
    tx_idx = HAL_IOMUX_PIN_P5_5 - HAL_IOMUX_PIN_P5_0;
    aoncmu->RAM2_POFF0 = (aoncmu->RAM2_POFF0 &
            ~((1 << tx_idx) | (1 << (rx_idx + 8)) | (1 << (tx_idx + 8)))) |
            (1 << rx_idx);
#endif

    iomux_unlock(lock);
}

void hal_iomux_set_bth_uart1(void)
{
    uint32_t mask;
    uint32_t lock;

    lock = iomux_lock();

    // Set uart1 func
#if (UART1_IOMUX_INDEX == 26)
    aoniomux->REG_00C = (aoniomux->REG_00C & ~(IOMUX_AON_GPIO_P26_SEL_MASK | IOMUX_AON_GPIO_P27_SEL_MASK)) |
        IOMUX_AON_GPIO_P26_SEL(7) | IOMUX_AON_GPIO_P27_SEL(7);

    mask = (1 << HAL_IOMUX_PIN_P2_6) | (1 << HAL_IOMUX_PIN_P2_7);
    // Setup pullup
    aoniomux->REG_02C |= (1 << HAL_IOMUX_PIN_P2_6);
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P2_7);
#else
    aoniomux->REG_004 = (aoniomux->REG_004 & ~(IOMUX_AON_GPIO_P02_SEL_MASK | IOMUX_AON_GPIO_P03_SEL_MASK)) |
        IOMUX_AON_GPIO_P02_SEL(4) | IOMUX_AON_GPIO_P03_SEL(4);

    mask = (1 << HAL_IOMUX_PIN_P0_2) | (1 << HAL_IOMUX_PIN_P0_3);
    // Setup pullup
    aoniomux->REG_02C |= (1 << HAL_IOMUX_PIN_P0_2);
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_3);
#endif
    // Clear pulldown
    aoniomux->REG_030 &= ~mask;

    iomux_unlock(lock);
}

void hal_iomux_set_sens_uart1(void)
{
    uint32_t mask;
    uint32_t lock;

    lock = iomux_lock();

    // Set uart1 func
#if (UART1_IOMUX_INDEX == 10)
    aoniomux->REG_008 = (aoniomux->REG_008 & ~(IOMUX_AON_GPIO_P10_SEL_MASK | IOMUX_AON_GPIO_P11_SEL_MASK)) |
        IOMUX_AON_GPIO_P10_SEL(3) | IOMUX_AON_GPIO_P11_SEL(3);

    mask = (1 << HAL_IOMUX_PIN_P1_0) | (1 << HAL_IOMUX_PIN_P1_1);
    // Setup pullup
    aoniomux->REG_02C |= (1 << HAL_IOMUX_PIN_P1_0);
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P1_1);
#else
    aoniomux->REG_004 = (aoniomux->REG_004 & ~(IOMUX_AON_GPIO_P00_SEL_MASK | IOMUX_AON_GPIO_P01_SEL_MASK)) |
        IOMUX_AON_GPIO_P00_SEL(1) | IOMUX_AON_GPIO_P01_SEL(1);

    mask = (1 << HAL_IOMUX_PIN_P0_0) | (1 << HAL_IOMUX_PIN_P0_1);
    // Setup pullup
    aoniomux->REG_02C |= (1 << HAL_IOMUX_PIN_P0_0);
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_1);
#endif
    // Clear pulldown
    aoniomux->REG_030 &= ~mask;

    iomux_unlock(lock);
}

void hal_iomux_set_uart1(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_uart1();
#elif defined(CHIP_SUBSYS_BTH)
    hal_iomux_set_bth_uart1();
#else
    hal_iomux_set_sys_uart1();
#endif
}

void hal_iomux_set_sens_uart2(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
#if (UART2_IOMUX_INDEX == 42)
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_SENS_UART2_RX, UART2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P4_3, HAL_IOMUX_FUNC_SENS_UART2_TX, UART2_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P5_4, HAL_IOMUX_FUNC_SENS_UART2_RX, UART2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P5_5, HAL_IOMUX_FUNC_SENS_UART2_TX, UART2_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}

void hal_iomux_set_uart2(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_uart2();
#elif defined(CHIP_SUBSYS_BTH) && defined(BTH_USE_SYS_PERIPH)
    hal_iomux_set_sys_uart0();
#endif
}

void hal_iomux_set_analog_i2c(void)
{
#if !(defined(JTAG_ENABLE) && (JTAG_IOMUX_INDEX == 22))
    uint32_t mask;
    uint32_t lock;

    lock = iomux_lock();
#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 0))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = HAL_IOMUX_PIN_P2_2;
    cfg.sda = HAL_IOMUX_PIN_P2_3;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    // Disable analog I2C master
    aoniomux->REG_048 |= IOMUX_AON_I2C0_M_SEL_GPIO;
    // Set mcu GPIO func
    aoniomux->REG_00C = (aoniomux->REG_00C & ~(IOMUX_AON_GPIO_P22_SEL_MASK | IOMUX_AON_GPIO_P23_SEL_MASK)) |
        IOMUX_AON_GPIO_P22_SEL(IOMUX_FUNC_VAL_GPIO) | IOMUX_AON_GPIO_P23_SEL(IOMUX_FUNC_VAL_GPIO);
    // Enable analog I2C slave
    aoniomux->REG_048 &= ~IOMUX_AON_GPIO_I2C_MODE;

    mask = (1 << HAL_IOMUX_PIN_P2_2) | (1 << HAL_IOMUX_PIN_P2_3);
    // Setup pullup
    aoniomux->REG_02C |= mask;
    // Clear pulldown
    aoniomux->REG_030 &= ~mask;

    iomux_unlock(lock);
#endif
}

void hal_iomux_set_jtag(void)
{
    uint32_t mask;
    uint32_t lock;

    // CAUTION: Only DSP can use P20/P27 as JTAG

#if (JTAG_IOMUX_INDEX == 22) || !defined(JTAG_DSP)
    hal_iomux_set_uart0_common(2);

    lock = iomux_lock();

    mask = (1 << HAL_IOMUX_PIN_P2_2) | (1 << HAL_IOMUX_PIN_P2_3);
    // Clear pullup
    aoniomux->REG_02C &= ~mask;
    // Clear pulldown
    aoniomux->REG_030 &= ~mask;

    iomux_unlock(lock);
#else
    uint32_t val;
    uint8_t sel;
    uint8_t tms;

    sel = 3;
    tms = 2;

    lock = iomux_lock();

    // SWCLK/TCK, SWDIO/TMS
    mask = IOMUX_AON_GPIO_P20_SEL_MASK | IOMUX_AON_GPIO_P27_SEL_MASK;
    val = IOMUX_AON_GPIO_P20_SEL(tms) | IOMUX_AON_GPIO_P27_SEL(sel);
    aoniomux->REG_00C = (aoniomux->REG_00C & ~mask) | val;

    // TDI, TDO
#ifdef JTAG_TDI_TDO_PIN
    mask = IOMUX_AON_GPIO_P24_SEL_MASK | IOMUX_AON_GPIO_P25_SEL_MASK;
    val = IOMUX_AON_GPIO_P24_SEL(sel) | IOMUX_AON_GPIO_P25_SEL(sel);
    aoniomux->REG_00C = (aoniomux->REG_00C & ~mask) | val;
#endif

    // RESET
#ifdef JTAG_RESET_PIN
    aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P26_SEL, sel);
#endif

    mask = (1 << HAL_IOMUX_PIN_P2_0) | (1 << HAL_IOMUX_PIN_P2_7);
#ifdef JTAG_TDI_TDO_PIN
    mask |= (1 << HAL_IOMUX_PIN_P2_4) | (1 << HAL_IOMUX_PIN_P2_5);
#endif
#ifdef JTAG_RESET_PIN
    mask |= (1 << HAL_IOMUX_PIN_P2_6);
#endif
    // Clear pullup
    aoniomux->REG_02C &= ~mask;
    // Clear pulldown
    aoniomux->REG_030 &= ~mask;

    iomux_unlock(lock);
#endif
}

void hal_iomux_ispi_access_enable(enum HAL_IOMUX_ISPI_MOD_T mod, enum HAL_IOMUX_ISPI_OWNER_T owner)
{
    aoniomux->REG_044 = (aoniomux->REG_044 & ~(3 << (mod * 2))) | ((owner & 3) << (mod * 2));
}

void BOOT_TEXT_FLASH_LOC hal_iomux_ispi_access_init(void)
{
    enum HAL_IOMUX_ISPI_OWNER_T owner;

#ifdef BTH_AS_MAIN_MCU
    owner = HAL_IOMUX_ISPI_OWNER_BTH;
#else
    owner = HAL_IOMUX_ISPI_OWNER_SYS;
#endif

    aoniomux->REG_044 = IOMUX_AON_SPI_PMU_SEL(owner) |
        IOMUX_AON_SPI_ANA_SEL(owner) |
        IOMUX_AON_SPI_RF_SEL(owner) |
        IOMUX_AON_SPI_XTAL_SEL(owner) |
        IOMUX_AON_SPI_RC_SEL(owner) |
        IOMUX_AON_SPI_EMMC_SEL(owner) |
        IOMUX_AON_SPI_PCIE_SEL(owner) |
        IOMUX_AON_SPI_USB_SEL(owner) |
        IOMUX_AON_SPI_PMU1_SEL(owner);
}

void hal_iomux_set_sys_i2s0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
#if (I2S0_IOMUX_INDEX == 45)
        {HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_SYS_I2S0_WS,   I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_4, HAL_IOMUX_FUNC_SYS_I2S0_SCK,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_6, HAL_IOMUX_FUNC_SYS_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_SYS_I2S0_SDO,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_IOMUX_INDEX == 41)
        {HAL_IOMUX_PIN_P4_1, HAL_IOMUX_FUNC_SYS_I2S0_WS,   I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_SYS_I2S0_SCK,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_SYS_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_3, HAL_IOMUX_FUNC_SYS_I2S0_SDO,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SYS_I2S0_WS,   I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SYS_I2S0_SCK,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SYS_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SYS_I2S0_SDO,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
}

void hal_iomux_set_sens_i2s0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
#if (I2S0_IOMUX_INDEX == 51)
        {HAL_IOMUX_PIN_P5_1, HAL_IOMUX_FUNC_SENS_I2S0_WS,   I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P5_0, HAL_IOMUX_FUNC_SENS_I2S0_SCK,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P5_2, HAL_IOMUX_FUNC_SENS_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P5_3, HAL_IOMUX_FUNC_SENS_I2S0_SDO,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SENS_I2S0_WS,   I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SENS_I2S0_SCK,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SENS_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SENS_I2S0_SDO,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
}

void hal_iomux_set_i2s0(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_i2s0();
#else
    hal_iomux_set_sys_i2s0();
#endif
}

void hal_iomux_set_sys_i2s1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
#if (I2S0_IOMUX_INDEX == 11)
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SYS_I2S1_WS,   I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SYS_I2S1_SCK,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SYS_I2S1_SDO,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_IOMUX_INDEX == 41)
        {HAL_IOMUX_PIN_P4_1, HAL_IOMUX_FUNC_SYS_I2S1_WS,   I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_SYS_I2S1_SCK,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_3, HAL_IOMUX_FUNC_SYS_I2S1_SDO,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_IOMUX_INDEX == 45)
        {HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_SYS_I2S1_WS,   I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_4, HAL_IOMUX_FUNC_SYS_I2S1_SCK,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_6, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_SYS_I2S1_SDO,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_IOMUX_INDEX == 51)
        {HAL_IOMUX_PIN_P5_1, HAL_IOMUX_FUNC_SYS_I2S1_WS,   I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P5_0, HAL_IOMUX_FUNC_SYS_I2S1_SCK,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P5_2, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P5_3, HAL_IOMUX_FUNC_SYS_I2S1_SDO,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SYS_I2S1_WS,   I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_SYS_I2S1_SCK,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_SYS_I2S1_SDI0, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_SYS_I2S1_SDO,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
}

void hal_iomux_set_i2s1(void)
{
#if !(defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH))
    hal_iomux_set_sys_i2s1();
#endif
}

void hal_iomux_set_sens_i2s_mclk(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SENS_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}

void hal_iomux_set_sys_i2s_mclk(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
#if (I2S_MCLK_IOMUX_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SYS_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P5_6, HAL_IOMUX_FUNC_SYS_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}

void hal_iomux_set_i2s_mclk(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_i2s_mclk();
#else
    hal_iomux_set_sys_i2s_mclk();
#endif
}

void hal_iomux_set_sens_i2s_mclk_in(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SENS_I2S_MCLK_IN, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}


void hal_iomux_set_sys_i2s_mclk_in(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
#if (I2S_MCLK_IOMUX_INDEX == 57)
        {HAL_IOMUX_PIN_P5_7, HAL_IOMUX_FUNC_SYS_I2S_MCLK_IN, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SYS_I2S_MCLK_IN, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}

void hal_iomux_set_i2s0_mclk_in(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_i2s_mclk_in();
#else
    hal_iomux_set_sys_i2s_mclk_in();
#endif
}

void hal_iomux_set_sys_spdif0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spdif[] = {
#if (SPDIF0_IOMUX_INDEX == 46)
        {HAL_IOMUX_PIN_P4_6, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_spdif, ARRAY_SIZE(pinmux_spdif));
}

void hal_iomux_set_spdif0(void)
{
#if !(defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH))
    hal_iomux_set_sys_spdif0();
#endif
}

void hal_iomux_set_spdif1(void)
{
}


void hal_iomux_set_dig_mic(uint32_t map)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic_clk[] = {
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_PDM0_CK, DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic_clk1[] = {
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_PDM1_CK, DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic_clk2[] = {
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_PDM2_CK, DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic0[] = {
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_PDM0_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic1[] = {
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_PDM1_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic2[] = {
       {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_PDM2_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    uint32_t clk_map = 0;
    if ((map & 0xF) == 0) {
        pinmux_digitalmic_clk[0].function  = HAL_IOMUX_FUNC_GPIO;
        pinmux_digitalmic_clk1[0].function = HAL_IOMUX_FUNC_GPIO;
        pinmux_digitalmic_clk2[0].function = HAL_IOMUX_FUNC_GPIO;
    }

    if (digmic_ck00_pin == HAL_IOMUX_PIN_P0_0) {
        pinmux_digitalmic_clk[0].pin  = HAL_IOMUX_PIN_P0_0;
        hal_iomux_init(pinmux_digitalmic_clk, ARRAY_SIZE(pinmux_digitalmic_clk));
        clk_map |= (1 << 0);
    }
    if (digmic_ck01_pin == HAL_IOMUX_PIN_P1_4) {
        pinmux_digitalmic_clk[0].pin  = HAL_IOMUX_PIN_P1_4;
        hal_iomux_init(pinmux_digitalmic_clk, ARRAY_SIZE(pinmux_digitalmic_clk));
        clk_map |= (1 << 0);
    }
    if (digmic_ck02_pin == HAL_IOMUX_PIN_P5_2) {
        pinmux_digitalmic_clk[0].pin  = HAL_IOMUX_PIN_P5_2;
        hal_iomux_init(pinmux_digitalmic_clk, ARRAY_SIZE(pinmux_digitalmic_clk));
        clk_map |= (1 << 0);
    }
    if (digmic_ck10_pin == HAL_IOMUX_PIN_P0_2) {
        pinmux_digitalmic_clk1[0].pin  = HAL_IOMUX_PIN_P0_2;
        hal_iomux_init(pinmux_digitalmic_clk1, ARRAY_SIZE(pinmux_digitalmic_clk1));
        clk_map |= (1 << 1);
    }
    if (digmic_ck11_pin == HAL_IOMUX_PIN_P1_6) {
        pinmux_digitalmic_clk1[0].pin  = HAL_IOMUX_PIN_P1_6;
        hal_iomux_init(pinmux_digitalmic_clk1, ARRAY_SIZE(pinmux_digitalmic_clk1));
        clk_map |= (1 << 1);
    }
    if (digmic_ck12_pin == HAL_IOMUX_PIN_P5_4) {
        pinmux_digitalmic_clk1[0].pin  = HAL_IOMUX_PIN_P5_4;
        hal_iomux_init(pinmux_digitalmic_clk1, ARRAY_SIZE(pinmux_digitalmic_clk1));
        clk_map |= (1 << 1);
    }
    if (digmic_ck20_pin == HAL_IOMUX_PIN_P3_4) {
        pinmux_digitalmic_clk2[0].pin  = HAL_IOMUX_PIN_P3_4;
        hal_iomux_init(pinmux_digitalmic_clk2, ARRAY_SIZE(pinmux_digitalmic_clk2));
        clk_map |= (1 << 2);
    }
    if (digmic_ck21_pin == HAL_IOMUX_PIN_P5_6) {
        pinmux_digitalmic_clk2[0].pin  = HAL_IOMUX_PIN_P5_6;
        hal_iomux_init(pinmux_digitalmic_clk2, ARRAY_SIZE(pinmux_digitalmic_clk2));
        clk_map |= (1 << 2);
    }

    if (digmic_d0_pin == HAL_IOMUX_PIN_P0_1 || digmic_d0_pin == HAL_IOMUX_PIN_P1_5 || digmic_d0_pin == HAL_IOMUX_PIN_P5_3) {
        pinmux_digitalmic0[0].pin = digmic_d0_pin;
    }

    if (digmic_d1_pin == HAL_IOMUX_PIN_P0_3 || digmic_d1_pin == HAL_IOMUX_PIN_P1_7 || digmic_d1_pin == HAL_IOMUX_PIN_P5_5) {
        pinmux_digitalmic1[0].pin = digmic_d1_pin;
    }

    if (digmic_d2_pin == HAL_IOMUX_PIN_P3_5 || digmic_d2_pin == HAL_IOMUX_PIN_P5_7) {
        pinmux_digitalmic2[0].pin = digmic_d2_pin;
    }

    if (map & (1 << 0)) {
        hal_iomux_init(pinmux_digitalmic0, ARRAY_SIZE(pinmux_digitalmic0));
    }

    if (map & (1 << 1)) {
        hal_iomux_init(pinmux_digitalmic1, ARRAY_SIZE(pinmux_digitalmic1));
    }

    if (map & (1 << 2)) {
        hal_iomux_init(pinmux_digitalmic2, ARRAY_SIZE(pinmux_digitalmic2));
    }

   hal_cmu_mcu_pdm_clock_out(clk_map);
   hal_cmu_mcu_pdm_data_in(map);
}

void hal_iomux_set_sys_spi(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_3wire[] = {
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SYS_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SYS_SPI_CS0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_SYS_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#ifdef SPI_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_4wire[] = {
#ifdef SPI_IOMUX_DI0_INDEX
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_SYS_SPI_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#endif

    hal_iomux_init(pinmux_spi_3wire, ARRAY_SIZE(pinmux_spi_3wire));
#ifdef SPI_IOMUX_4WIRE
    hal_iomux_init(pinmux_spi_4wire, ARRAY_SIZE(pinmux_spi_4wire));
    aoniomux->REG_064 &= ~IOMUX_AON_SPI2_WM3;
#else
    aoniomux->REG_064 |= IOMUX_AON_SPI2_WM3;
#endif
}

void hal_iomux_set_bth_spi_capsensor(void)
{
    aoniomux->REG_008 = (aoniomux->REG_008 & ~(IOMUX_AON_GPIO_P14_SEL_MASK | IOMUX_AON_GPIO_P15_SEL_MASK | IOMUX_AON_GPIO_P16_SEL_MASK | IOMUX_AON_GPIO_P17_SEL_MASK)) |
        IOMUX_AON_GPIO_P14_SEL(7) | IOMUX_AON_GPIO_P15_SEL(7) | IOMUX_AON_GPIO_P16_SEL(7) | IOMUX_AON_GPIO_P17_SEL(7);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P1_4, HAL_IOMUX_PIN_NOPULL);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P1_5, HAL_IOMUX_PIN_NOPULL);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P1_6, HAL_IOMUX_PIN_NOPULL);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P1_7, HAL_IOMUX_PIN_NOPULL);
}

void hal_iomux_set_bth_spi(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_3wire[] = {
#if (SPI_IOMUX_CLK_INDEX == 30)
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_BTH_SPI_CLK, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_BTH_SPI_CS0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_BTH_SPI_DIO, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_BTH_SPI_CLK, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_BTH_SPI_CS0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_BTH_SPI_DIO, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#ifdef SPI_IOMUX_CS1_INDEX
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_BTH_SPI_CS1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#ifdef SPI_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_4wire[] = {
#ifdef SPI_IOMUX_DI0_INDEX
#if (SPI_IOMUX_DI0_INDEX == 32)
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_BTH_SPI_DI0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_BTH_SPI_DI0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#endif
#ifdef SPI_IOMUX_DI1_INDEX
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_BTH_SPI_DI1, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#endif

    hal_iomux_init(pinmux_spi_3wire, ARRAY_SIZE(pinmux_spi_3wire));
#ifdef SPI_IOMUX_4WIRE
    hal_iomux_init(pinmux_spi_4wire, ARRAY_SIZE(pinmux_spi_4wire));
    aoniomux->REG_064 &= ~IOMUX_AON_SPI3_WM3;
#else
    aoniomux->REG_064 &= ~IOMUX_AON_SPI3_WM3;
#endif
}

void hal_iomux_set_sens_spi(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_3wire[] = {
#if (SPI_IOMUX_CLK_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SENS_SPI_CLK, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SENS_SPI_CS0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SENS_SPI_DIO, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SENS_SPI_CLK, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SENS_SPI_CS0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_SENS_SPI_DIO, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#ifdef SPI_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_4wire[] = {
#ifdef SPI_IOMUX_DI0_INDEX
#if (SPI_IOMUX_DI0_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SENS_SPI_DI0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_SENS_SPI_DI0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#endif
    };
#endif

    hal_iomux_init(pinmux_spi_3wire, ARRAY_SIZE(pinmux_spi_3wire));
#ifdef SPI_IOMUX_4WIRE
    hal_iomux_init(pinmux_spi_4wire, ARRAY_SIZE(pinmux_spi_4wire));
    aoniomux->REG_064 &= ~IOMUX_AON_SPI0_WM3;
#else
    aoniomux->REG_064 |= IOMUX_AON_SPI0_WM3;
#endif
}

void hal_iomux_set_spi(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_spi();
#elif defined(CHIP_SUBSYS_BTH)
   hal_iomux_set_bth_spi();
#else
    hal_iomux_set_sys_spi();
#endif
}

void hal_iomux_set_bth_spilcd(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_3wire[] = {
        {HAL_IOMUX_PIN_P4_4, HAL_IOMUX_FUNC_BTH_SPILCD_CLK, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_BTH_SPILCD_CS0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_BTH_SPILCD_DIO, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#ifdef SPILCD_IOMUX_CS1_INDEX
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_BTH_SPILCD_CS1, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#ifdef SPILCD_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_4wire[] = {
#ifdef SPILCD_IOMUX_DI0_INDEX
        {HAL_IOMUX_PIN_P4_6, HAL_IOMUX_FUNC_BTH_SPILCD_DI0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#ifdef SPILCD_IOMUX_DI1_INDEX
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_BTH_SPILCD_DI1, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#endif

    hal_iomux_init(pinmux_spilcd_3wire, ARRAY_SIZE(pinmux_spilcd_3wire));
#ifdef SPILCD_IOMUX_4WIRE
    hal_iomux_init(pinmux_spilcd_4wire, ARRAY_SIZE(pinmux_spilcd_4wire));
    aoniomux->REG_064 &= ~IOMUX_AON_SPI4_WM3;
#else
    aoniomux->REG_064 &= ~IOMUX_AON_SPI4_WM3;
#endif
}

void hal_iomux_set_sens_spilcd(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_3wire[] = {
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_SENS_SPILCD_CLK, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_SENS_SPILCD_CS0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_SENS_SPILCD_DIO, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#ifdef SPILCD_IOMUX_CS1_INDEX
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SENS_SPILCD_CS1, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#ifdef SPILCD_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_4wire[] = {
#ifdef SPILCD_IOMUX_DI0_INDEX
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_SENS_SPILCD_DI0, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#ifdef SPILCD_IOMUX_DI1_INDEX
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SENS_SPILCD_DI1, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#endif

    hal_iomux_init(pinmux_spilcd_3wire, ARRAY_SIZE(pinmux_spilcd_3wire));
#ifdef SPILCD_IOMUX_4WIRE
    hal_iomux_init(pinmux_spilcd_4wire, ARRAY_SIZE(pinmux_spilcd_4wire));
    aoniomux->REG_064 &= ~IOMUX_AON_SPI1_WM3;
#else
    aoniomux->REG_064 |= IOMUX_AON_SPI1_WM3;
#endif
}

void hal_iomux_set_spilcd(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_spilcd();
#elif defined(CHIP_SUBSYS_BTH)
    hal_iomux_set_bth_spilcd();;
#endif
}

void hal_iomux_set_sys_i2c0(void)
{
#if (I2C0_IOMUX_INDEX == 22)
    hal_iomux_set_analog_i2c();
    // IOMUX_AON_GPIO_I2C_MODE should be kept in disabled state
    aoniomux->REG_048 &= ~IOMUX_AON_I2C0_M_SEL_GPIO;
#else
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C0_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SYS_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SYS_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C0_IOMUX_INDEX == 56)
        {HAL_IOMUX_PIN_P5_6, HAL_IOMUX_FUNC_SYS_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P5_7, HAL_IOMUX_FUNC_SYS_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SYS_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SYS_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 0))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
#endif
}

void hal_iomux_set_bth_i2c0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C0_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_BTH_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_BTH_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C0_IOMUX_INDEX == 44)
        {HAL_IOMUX_PIN_P4_4, HAL_IOMUX_FUNC_BTH_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_BTH_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_BTH_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_BTH_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 0))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
}

void hal_iomux_set_sens_i2c0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SENS_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_SENS_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 0))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
}

void hal_iomux_set_i2c0(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_i2c0();
#elif defined(CHIP_SUBSYS_BTH)
    hal_iomux_set_bth_i2c0();
#else
    hal_iomux_set_sys_i2c0();
#endif
}

void hal_iomux_set_sys_i2c1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C1_IOMUX_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SYS_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SYS_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C1_IOMUX_INDEX == 54)
        {HAL_IOMUX_PIN_P5_4, HAL_IOMUX_FUNC_SYS_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P5_5, HAL_IOMUX_FUNC_SYS_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SYS_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SYS_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 1))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
    aoniomux->REG_048 |= IOMUX_AON_I2C1_M_SEL_GPIO;
}

void hal_iomux_set_bth_i2c1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C1_IOMUX_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_BTH_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_BTH_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C1_IOMUX_INDEX == 46)
        {HAL_IOMUX_PIN_P4_6, HAL_IOMUX_FUNC_BTH_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_BTH_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_BTH_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_BTH_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 0))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
}

void hal_iomux_set_sens_i2c1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_SENS_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SENS_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 1))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
}

void hal_iomux_set_i2c1(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_i2c1();
#elif defined(CHIP_SUBSYS_BTH)
    hal_iomux_set_bth_i2c1();
#else
    hal_iomux_set_sys_i2c1();
#endif
}

void hal_iomux_set_sens_i2c2(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C2_IOMUX_INDEX == 24)
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_SENS_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_SENS_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C2_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SENS_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SENS_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SENS_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SENS_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 2))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
}

void hal_iomux_set_i2c2(void)
{
#if defined(CHIP_SUBSYS_SENS) || defined(BTH_USE_SYS_SENS_PERIPH)
    hal_iomux_set_sens_i2c2();
#endif
}

void hal_iomux_set_sens_i2c3(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C3_IOMUX_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SENS_I2C_M3_SCL, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SENS_I2C_M3_SDA, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_SENS_I2C_M3_SCL, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_SENS_I2C_M3_SDA, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 3))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
}

void hal_iomux_set_i2c3(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_i2c3();
#endif
}

void hal_iomux_set_sens_i2c4(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C4_IOMUX_INDEX == 54)
        {HAL_IOMUX_PIN_P5_4, HAL_IOMUX_FUNC_SENS_I2C_M4_SCL, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P5_5, HAL_IOMUX_FUNC_SENS_I2C_M4_SDA, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SENS_I2C_M4_SCL, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SENS_I2C_M4_SDA, I2C3_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
    };

#if defined(I2C_RECOVERY) && (I2C_RECOVERY & (1 << 4))
    struct HAL_GPIO_I2C_CONFIG_T cfg;

    memset(&cfg, 0, sizeof(struct HAL_GPIO_I2C_CONFIG_T));
    cfg.scl = pinmux_i2c[0].pin;
    cfg.sda = pinmux_i2c[1].pin;
    hal_gpio_i2c_recovery(&cfg, 0);
#endif
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
}

void hal_iomux_set_i2c4(void)
{
#if defined(CHIP_SUBSYS_SENS) || defined(BTH_USE_SYS_SENS_PERIPH)
    hal_iomux_set_sens_i2c4();
#endif
}

void hal_iomux_set_pwm0(void)
{
#if (PWM0_IOMUX_INDEX == 30)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S00_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_0);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_0);
#elif (PWM0_IOMUX_INDEX == 40)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S10_SEL(n), 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_0);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_0);
#elif (PWM0_IOMUX_INDEX == 50)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S20_SEL(n), 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_0);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_0);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P00_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_0);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_0);
#endif
}

void hal_iomux_set_pwm1(void)
{
#if (PWM1_IOMUX_INDEX == 31)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S01_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_1);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_1);
#elif (PWM1_IOMUX_INDEX == 41)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S11_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_1);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_1);
#elif (PWM1_IOMUX_INDEX == 51)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S21_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_1);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_1);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P01_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_1);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_1);
#endif
}

void hal_iomux_set_pwm2(void)
{
#if (PWM2_IOMUX_INDEX == 32)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S02_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_2);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_2);
#elif (PWM2_IOMUX_INDEX == 42)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S12_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_2);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_2);
#elif (PWM2_IOMUX_INDEX == 52)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S22_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_2);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_2);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P02_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_2);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_2);
#endif
}

void hal_iomux_set_pwm3(void)
{
#if (PWM3_IOMUX_INDEX == 33)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S03_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_3);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_3);
#elif (PWM3_IOMUX_INDEX == 43)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S13_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_3);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_3);
#elif (PWM3_IOMUX_INDEX == 53)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S23_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_3);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_3);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P03_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_3);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_3);
#endif
}

void hal_iomux_set_pwm4(void)
{
#if (PWM4_IOMUX_INDEX == 34)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S04_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_4);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_4);
#elif (PWM4_IOMUX_INDEX == 44)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S14_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_4);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_4);
#elif (PWM4_IOMUX_INDEX == 54)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S24_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_4);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_4);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P04_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_4);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_4);
#endif
}

void hal_iomux_set_pwm5(void)
{
#if (PWM5_IOMUX_INDEX == 35)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S05_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_5);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_5);
#elif (PWM5_IOMUX_INDEX == 45)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S15_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_5);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_5);
#elif (PWM5_IOMUX_INDEX == 55)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S25_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_5);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_5);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P05_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_5);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_5);
#endif
}

void hal_iomux_set_pwm6(void)
{
#if (PWM6_IOMUX_INDEX == 36)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S06_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_6);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_6);
#elif (PWM6_IOMUX_INDEX == 46)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S16_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_6);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_6);
#elif (PWM6_IOMUX_INDEX == 56)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S26_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_6);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_6);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P06_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_6);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_6);
#endif
}

void hal_iomux_set_pwm7(void)
{
#if (PWM7_IOMUX_INDEX == 37)
    aoniomux->REG_010 = SET_BITFIELD(aoniomux->REG_010, IOMUX_AON_GPIO_S07_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P3_7);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P3_7);
#elif (PWM7_IOMUX_INDEX == 47)
    aoniomux->REG_014 = SET_BITFIELD(aoniomux->REG_014, IOMUX_AON_GPIO_S17_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P4_7);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P4_7);
#elif (PWM7_IOMUX_INDEX == 57)
    aoniomux->REG_018 = SET_BITFIELD(aoniomux->REG_018, IOMUX_AON_GPIO_S27_SEL, 8);
    // Clear pullup
    aoniomux->REG_024 &= ~(1 << HAL_IOMUX_PIN_P5_7);
    // Clear pulldown
    aoniomux->REG_028 &= ~(1 << HAL_IOMUX_PIN_P5_7);
#else
    aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P07_SEL, 8);
    // Clear pullup
    aoniomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P0_7);
    // Clear pulldown
    aoniomux->REG_030 &= ~(1 << HAL_IOMUX_PIN_P0_7);
#endif
}

void hal_iomux_set_clock_out(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clkout[] = {
#if (CLKOUT_IOMUX_INDEX == 17)
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 21)
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 57)
        {HAL_IOMUX_PIN_P5_7, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_clkout, ARRAY_SIZE(pinmux_clkout));
}

void hal_iomux_set_sys_clock_out(void)
{
}

void hal_iomux_set_bt_clock_out(void)
{
}

void hal_iomux_set_bt_tport(void)
{
    uint32_t lock;
    uint32_t val;

    val = 14;

    lock = iomux_lock();

    aoniomux->REG_010 = (aoniomux->REG_010 & ~(IOMUX_AON_GPIO_S00_SEL_MASK | IOMUX_AON_GPIO_S01_SEL_MASK | IOMUX_AON_GPIO_S02_SEL_MASK | IOMUX_AON_GPIO_S03_SEL_MASK |
    IOMUX_AON_GPIO_S04_SEL_MASK | IOMUX_AON_GPIO_S05_SEL_MASK | IOMUX_AON_GPIO_S06_SEL_MASK | IOMUX_AON_GPIO_S07_SEL_MASK)) |
    IOMUX_AON_GPIO_S00_SEL(val) | IOMUX_AON_GPIO_S01_SEL(val) | IOMUX_AON_GPIO_S02_SEL(val) | IOMUX_AON_GPIO_S03_SEL(val) |
    IOMUX_AON_GPIO_S04_SEL(val) | IOMUX_AON_GPIO_S05_SEL(val) | IOMUX_AON_GPIO_S06_SEL(val) | IOMUX_AON_GPIO_S07_SEL(val);
    // ANA TEST DIR
    aoniomux->REG_080 = 0xFFFF;
    // ANA TEST SEL
    aoniomux->REG_044 = (aoniomux->REG_044 & ~(IOMUX_AON_ANA_TEST_SEL_MASK)) | IOMUX_AON_ANA_TEST_SEL(5);

    iomux_unlock(lock);
}

void hal_iomux_clear_bt_tport(void)
{
}

void hal_iomux_set_sleep_tport(void)
{
    uint32_t lock;
    uint32_t val;

    val = 14;

    lock = iomux_lock();

    aoniomux->REG_010 = (aoniomux->REG_010 & ~(IOMUX_AON_GPIO_S00_SEL_MASK | IOMUX_AON_GPIO_S01_SEL_MASK | IOMUX_AON_GPIO_S02_SEL_MASK | IOMUX_AON_GPIO_S03_SEL_MASK |
    IOMUX_AON_GPIO_S04_SEL_MASK | IOMUX_AON_GPIO_S05_SEL_MASK | IOMUX_AON_GPIO_S06_SEL_MASK | IOMUX_AON_GPIO_S07_SEL_MASK)) |
    IOMUX_AON_GPIO_S00_SEL(val) | IOMUX_AON_GPIO_S01_SEL(val) | IOMUX_AON_GPIO_S02_SEL(val) | IOMUX_AON_GPIO_S03_SEL(val) |
    IOMUX_AON_GPIO_S04_SEL(val) | IOMUX_AON_GPIO_S05_SEL(val) | IOMUX_AON_GPIO_S06_SEL(val) | IOMUX_AON_GPIO_S07_SEL(val);
    // ANA TEST DIR
    aoniomux->REG_080 = 0xFFFF;
    // ANA TEST SEL
    aoniomux->REG_044 = (aoniomux->REG_044 & ~(IOMUX_AON_ANA_TEST_SEL_MASK)) | IOMUX_AON_ANA_TEST_SEL(1);

    iomux_unlock(lock);
}

void hal_iomux_set_antenna_sw(void)
{
    uint32_t lock;

    lock = iomux_lock();
    //bt wifi sw
    aoniomux->REG_00C = (aoniomux->REG_00C & ~(IOMUX_AON_GPIO_P21_SEL_MASK)) | IOMUX_AON_GPIO_P21_SEL(7);
    //antenna sw
    aoniomux->REG_008 = (aoniomux->REG_008 & ~(IOMUX_AON_GPIO_P10_SEL_MASK)) | IOMUX_AON_GPIO_P10_SEL(8);

    iomux_unlock(lock);
}

void hal_iomux_set_bt_active_out(void)
{
    uint32_t lock;

    lock = iomux_lock();

    aoniomux->REG_004 = (aoniomux->REG_004 & ~(IOMUX_AON_GPIO_P05_SEL_MASK)) | IOMUX_AON_GPIO_P05_SEL(5);

    iomux_unlock(lock);
}

void hal_iomux_set_bt_rf_sw(int rx_on, int tx_on)
{
    uint32_t lock;

    lock = iomux_lock();

    if (rx_on) {
#if (RXON_IOMUX_INDEX == 11)
        aoniomux->REG_008 = SET_BITFIELD(aoniomux->REG_008, IOMUX_AON_GPIO_P10_SEL, 8);
#elif (RXON_IOMUX_INDEX == 14)
        aoniomux->REG_008 = SET_BITFIELD(aoniomux->REG_008, IOMUX_AON_GPIO_P14_SEL, 4);
#elif (RXON_IOMUX_INDEX == 16)
        aoniomux->REG_008 = SET_BITFIELD(aoniomux->REG_008, IOMUX_AON_GPIO_P16_SEL, 4);
#elif (RXON_IOMUX_INDEX == 20)
        aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P20_SEL, 8);
#elif (RXON_IOMUX_INDEX == 25)
        aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P25_SEL, 7);
#else
        aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P06_SEL, 6);
#endif
    }

    if (tx_on) {
#if (TXON_IOMUX_INDEX == 15)
        aoniomux->REG_008 = SET_BITFIELD(aoniomux->REG_008, IOMUX_AON_GPIO_P15_SEL, 4);
#elif (TXON_IOMUX_INDEX == 24)
        aoniomux->REG_00C = SET_BITFIELD(aoniomux->REG_00C, IOMUX_AON_GPIO_P24_SEL, 7);
#else
        aoniomux->REG_004 = SET_BITFIELD(aoniomux->REG_004, IOMUX_AON_GPIO_P07_SEL, 6);
#endif
    }

    iomux_unlock(lock);
}

void hal_iomux_bt_3wire_tports_init(void)
{
    //set gpio driver max
    aoniomux->REG_058 = (aoniomux->REG_058& ~(IOMUX_AON_GPIO_P2_DRV1_SEL_MASK | IOMUX_AON_GPIO_P2_DRV2_SEL_MASK | IOMUX_AON_GPIO_P2_DRV2_SEL_MASK))|
        IOMUX_AON_GPIO_P2_DRV1_SEL(0xF) | IOMUX_AON_GPIO_P2_DRV2_SEL(0xF) | IOMUX_AON_GPIO_P2_DRV3_SEL(0xF);

    //select 3wire tports function
    aoniomux->REG_00C = (aoniomux->REG_00C& ~(IOMUX_AON_GPIO_P21_SEL_MASK | IOMUX_AON_GPIO_P22_SEL_MASK | IOMUX_AON_GPIO_P23_SEL_MASK))|
        IOMUX_AON_GPIO_P21_SEL(6) | IOMUX_AON_GPIO_P22_SEL(6) | IOMUX_AON_GPIO_P23_SEL(6);

}

static const struct HAL_IOMUX_PIN_FUNCTION_MAP iomux_tport[] = {
/*    {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},*/
    {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
};

int hal_iomux_tportopen(void)
{
    int i;

    for (i=0;i<sizeof(iomux_tport)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP);i++){
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&iomux_tport[i], 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)iomux_tport[i].pin, HAL_GPIO_DIR_OUT, 0);
    }
    return 0;
}

int hal_iomux_tportset(int port)
{
    hal_gpio_pin_set((enum HAL_GPIO_PIN_T)iomux_tport[port].pin);
    return 0;
}

int hal_iomux_tportclr(int port)
{
    hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)iomux_tport[port].pin);
    return 0;
}

static void hal_iomux_config_sys_spi_slave(uint32_t val)
{
    uint32_t lock;
    uint32_t reg_val;

    lock = iomux_lock();

#if (SPI_SLV_IOMUX_INDEX == 24)
    aoniomux->REG_00C= (aoniomux->REG_00C& ~(IOMUX_AON_GPIO_P24_SEL_MASK | IOMUX_AON_GPIO_P25_SEL_MASK |
        IOMUX_AON_GPIO_P26_SEL_MASK | IOMUX_AON_GPIO_P27_SEL_MASK)) | IOMUX_AON_GPIO_P24_SEL(val) | IOMUX_AON_GPIO_P25_SEL(val) |
        IOMUX_AON_GPIO_P26_SEL(val) | IOMUX_AON_GPIO_P27_SEL(val);
#else
    aoniomux->REG_008 = (aoniomux->REG_008 & ~(IOMUX_AON_GPIO_P14_SEL_MASK | IOMUX_AON_GPIO_P15_SEL_MASK |
        IOMUX_AON_GPIO_P16_SEL_MASK | IOMUX_AON_GPIO_P17_SEL_MASK)) | IOMUX_AON_GPIO_P14_SEL(val) | IOMUX_AON_GPIO_P15_SEL(val) |
        IOMUX_AON_GPIO_P16_SEL(val) | IOMUX_AON_GPIO_P17_SEL(val);
#endif

    if (val == IOMUX_FUNC_VAL_GPIO) {
        reg_val = 0;
    } else {
        reg_val = IOMUX_AON_SPI_INTR_RESETN | IOMUX_AON_SPI_INTR_MASK;
    }

    aoniomux->REG_070 = reg_val;

    iomux_unlock(lock);
}

void hal_iomux_set_spi_slave(void)
{
    hal_iomux_config_sys_spi_slave(2);

}

void hal_iomux_clear_sens_spi_slave(void)
{
    hal_iomux_config_sys_spi_slave(IOMUX_FUNC_VAL_GPIO);
}

int WEAK hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type)
{
    return 0;
}

bool WEAK hal_pwrkey_pressed(void)
{
    return 0;
}

bool hal_pwrkey_startup_pressed(void)
{
    return hal_pwrkey_pressed();
}

enum HAL_PWRKEY_IRQ_T WEAK hal_pwrkey_get_irq_state(void)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;

    return state;
}

void hal_iomux_set_codec_gpio_trigger(enum HAL_IOMUX_PIN_T pin, bool polarity)
{
    int trig_sel = 0;

    if (pin == HAL_IOMUX_PIN_P2_2) {
        trig_sel = 0x1;
    } else if (pin == HAL_IOMUX_PIN_P2_5) {
        trig_sel = 0x0;
    } else {
        ASSERT(false, "%s: bad codec trigger pin: %d", __func__, pin);
    }
    aoniomux->REG_04C = SET_BITFIELD(aoniomux->REG_04C, IOMUX_AON_CFG_CODEC_TRIG_SEL, trig_sel);
    // polarity: the trigger edge, 1 - rising edge, 0 - falling edge
    if (polarity) {
        aoniomux->REG_04C &= ~IOMUX_AON_CFG_CODEC_TRIG_POL;
    } else {
        aoniomux->REG_04C |= IOMUX_AON_CFG_CODEC_TRIG_POL;
    }
}

void hal_iomux_set_sdmmc0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP iomux[] = {
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SDMMC0_CLK,   SDMMC0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},//No pull-up required
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SDMMC0_CMD,   SDMMC0_VOLTAGE_SEL, SDMMC0_PULLUP_SEL},
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SDMMC0_DATA0, SDMMC0_VOLTAGE_SEL, SDMMC0_PULLUP_SEL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SDMMC0_DATA1, SDMMC0_VOLTAGE_SEL, SDMMC0_PULLUP_SEL},
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_SDMMC0_DATA2, SDMMC0_VOLTAGE_SEL, SDMMC0_PULLUP_SEL},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_SDMMC0_DATA3, SDMMC0_VOLTAGE_SEL, SDMMC0_PULLUP_SEL},
    };

#ifdef SDMMC0_IO_DRV
    for (uint8_t i = 0; i < sizeof(iomux) / sizeof(iomux[0]); i++) {
        hal_iomux_set_io_drv(iomux[i].pin, SDMMC0_IO_DRV);
    }
#endif

    hal_iomux_init(iomux, ARRAY_SIZE(iomux));
}

void hal_iomux_set_sdmmc0_dt_n_out_group(int enable)
{
}

extern void pmu_uart_enable(void); 

void hal_iomux_single_wire_bth_uart_rx(uint32_t uart)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_uart[] = {
#if (UART1_IOMUX_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_BTH_UART1_RX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#ifndef UART_HALF_DUPLEX
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_GPIO,         HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
#else
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_BTH_UART1_RX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#ifdef SINGLE_WIRE_UART_PMU
        {HAL_IOMUX_PIN_P4_3, HAL_IOMUX_FUNC_BTH_UART1_TX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
#endif
    };

#if !defined(SINGLE_WIRE_UART_PMU) && defined(UART_HALF_DUPLEX)
    aoniomux->REG_064 &= ~IOMUX_AON_UART1_HALFN_BTH;
#endif

    hal_iomux_init(pinmux_uart, ARRAY_SIZE(pinmux_uart));

#ifdef SINGLE_WIRE_UART_PMU
    pmu_uart_enable();
#endif
}

void hal_iomux_single_wire_bth_uart_tx(uint32_t uart)
{
#if !defined(SINGLE_WIRE_UART_PMU) && !defined(UART_HALF_DUPLEX)
#if (UART1_IOMUX_INDEX == 26)
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_uart[] = {
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_GPIO,         HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_BTH_UART1_TX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_uart[0].pin, HAL_GPIO_DIR_IN, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_uart[1].pin, HAL_GPIO_DIR_IN, 1);

    hal_iomux_init(pinmux_uart, ARRAY_SIZE(pinmux_uart));
#endif
#endif
}

void hal_iomux_single_wire_sens_uart_rx(uint32_t uart)
{
}

void hal_iomux_single_wire_sens_uart_tx(uint32_t uart)
{
}

void hal_iomux_single_wire_sys_uart_rx(uint32_t uart)
{
}

void hal_iomux_single_wire_sys_uart_tx(uint32_t uart)
{
}

void hal_iomux_set_qspinand(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] =
    {
#if (NAND_IOMUX_INDEX == 40)
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_1, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CS, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_3, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_4, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_SYS_QSPI_NAND_CS, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SYS_QSPI_NAND_D0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
#endif
    };
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}

void hal_iomux_single_wire_uart_rx(uint32_t uart)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_single_wire_sens_uart_rx(uart);
#elif defined(CHIP_SUBSYS_BTH)
    hal_iomux_single_wire_bth_uart_rx(uart);
#else
    hal_iomux_single_wire_sys_uart_rx(uart);
#endif
}

void hal_iomux_single_wire_uart_tx(uint32_t uart)
{
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_single_wire_sens_uart_tx(uart);
#elif defined(CHIP_SUBSYS_BTH)
    hal_iomux_single_wire_bth_uart_tx(uart);
#else
    hal_iomux_single_wire_sys_uart_tx(uart);
#endif
}

