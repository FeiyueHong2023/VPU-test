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
#ifndef __HAL_CMU_BEST1603_H__
#define __HAL_CMU_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_AHB0,         // 0
    HAL_CMU_MOD_H_AHB1,         // 1
    HAL_CMU_MOD_H_AHB2,         // 2
    HAL_CMU_MOD_H_DMA0,         // 3
    HAL_CMU_MOD_H_DMA1,         // 4
    HAL_CMU_MOD_H_AH2H_AON,     // 5
    HAL_CMU_MOD_H_SYS_FLASH,    // 6
    HAL_CMU_MOD_H_SDMMC,        // 7
    HAL_CMU_MOD_H_USBC,         // 8
    HAL_CMU_MOD_H_USBH,         // 9
    HAL_CMU_MOD_H_CODEC_NA,     // 10
    HAL_CMU_MOD_H_SEC_ENG,      // 11
    HAL_CMU_MOD_H_GPU,          // 12
    HAL_CMU_MOD_H_LCDC,         // 13
    HAL_CMU_MOD_H_DISPPRE,      // 14
    HAL_CMU_MOD_H_DISP,         // 15
    HAL_CMU_MOD_H_DMA2D,        // 16
    HAL_CMU_MOD_H_AHB_S,        // 17
    HAL_CMU_MOD_H_PSRAM,        // 18
    HAL_CMU_MOD_H_SPI_AHB,      // 19
    HAL_CMU_MOD_H_NANDFLASH,    // 20
    HAL_CMU_MOD_H_DUMPCPU,      // 21
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 22
    HAL_CMU_MOD_P_WDT,          // 23
    HAL_CMU_MOD_P_SYS_TIMER0,   // 24
    HAL_CMU_MOD_P_SYS_TIMER1,   // 25
    HAL_CMU_MOD_P_SYS_TIMER2,   // 26
    HAL_CMU_MOD_P_SYS_TIMER3,   // 27
    HAL_CMU_MOD_P_I2C0,         // 28
    HAL_CMU_MOD_P_I2C1,         // 29
    HAL_CMU_MOD_P_SPI0,         // 30
    HAL_CMU_MOD_P_SPI1,         // 31
    HAL_CMU_MOD_P_SPI_ITN,      // 32
    HAL_CMU_MOD_P_UART0,        // 33
    HAL_CMU_MOD_P_UART1,        // 34
    HAL_CMU_MOD_P_TRNG,         // 35
    HAL_CMU_MOD_P_I2S0,         // 36
    HAL_CMU_MOD_P_I2S1,         // 37
    HAL_CMU_MOD_P_SPDIF0,       // 38
    HAL_CMU_MOD_P_GPIO0,        // 39
    HAL_CMU_MOD_P_GPIO0_INT,    // 40
    HAL_CMU_MOD_P_GPIO1,        // 41
    HAL_CMU_MOD_P_GPIO1_INT,    // 42
    HAL_CMU_MOD_P_IOMUX,        // 43
    HAL_CMU_MOD_P_SEC_ENG,      // 44
    HAL_CMU_MOD_P_TZC,          // 45
    HAL_CMU_MOD_P_PCM,          // 46
    HAL_CMU_MOD_P_IRQCTL,       // 47
    HAL_CMU_MOD_P_PWM,          // 48
    HAL_CMU_MOD_P_TCNT,         // 49
    HAL_CMU_MOD_P_PWM1,         // 50
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 51
    HAL_CMU_MOD_O_SYS_FLASH,    // 52
    HAL_CMU_MOD_O_USB,          // 53
    HAL_CMU_MOD_O_SDMMC,        // 54
    HAL_CMU_MOD_O_WDT,          // 55
    HAL_CMU_MOD_O_SYS_TIMER0,   // 56
    HAL_CMU_MOD_O_SYS_TIMER1,   // 57
    HAL_CMU_MOD_O_SYS_TIMER2,   // 58
    HAL_CMU_MOD_O_SYS_TIMER3,   // 59
    HAL_CMU_MOD_O_I2C0,         // 60
    HAL_CMU_MOD_O_I2C1,         // 61
    HAL_CMU_MOD_O_SPI0,         // 62
    HAL_CMU_MOD_O_SPI1,         // 63
    HAL_CMU_MOD_O_SPI_ITN,      // 64
    HAL_CMU_MOD_O_UART0,        // 65
    HAL_CMU_MOD_O_UART1,        // 66
    HAL_CMU_MOD_O_EMMC,         // 67
    HAL_CMU_MOD_O_I2S0,         // 68
    HAL_CMU_MOD_O_I2S1,         // 69
    HAL_CMU_MOD_O_SPDIF0,       // 70
    HAL_CMU_MOD_O_USB32K,       // 71
    HAL_CMU_MOD_O_PSRAM,        // 72
    HAL_CMU_MOD_O_CORE0,        // 73
    HAL_CMU_MOD_O_CORETCM,      // 74
    HAL_CMU_MOD_O_COREROM,      // 75
    HAL_CMU_MOD_O_COREDBG,      // 76
    HAL_CMU_MOD_O_DSP,          // 77
    HAL_CMU_MOD_O_DSPTCM,       // 78
    HAL_CMU_MOD_O_GPUX1,        // 79
    HAL_CMU_MOD_O_GPIO0,        // 80
    HAL_CMU_MOD_O_GPIO1,        // 81
    HAL_CMU_MOD_O_IOMUX,        // 82
    // QCLK/QRST
    HAL_CMU_MOD_Q_BTH2SYS,      // 83
    HAL_CMU_MOD_Q_BTH2FLS,      // 84
    HAL_CMU_MOD_Q_SES2SYS,      // 85
    HAL_CMU_MOD_Q_DISPN2D,      // 86
    HAL_CMU_MOD_Q_DISTV2D,      // 87
    HAL_CMU_MOD_Q_LCDC_P,       // 88
    HAL_CMU_MOD_Q_DIS_PN,       // 89
    HAL_CMU_MOD_Q_DIS_TV,       // 90
    HAL_CMU_MOD_Q_DIS_PIX,      // 91
    HAL_CMU_MOD_Q_DIS_DSI,      // 92
    HAL_CMU_MOD_Q_DIS_QSPI,     // 93
    HAL_CMU_MOD_Q_PWM0,         // 94
    HAL_CMU_MOD_Q_PWM1,         // 95
    HAL_CMU_MOD_Q_PWM2,         // 96
    HAL_CMU_MOD_Q_PWM3,         // 97
    HAL_CMU_MOD_Q_PCM,          // 98
    HAL_CMU_MOD_Q_BTC2SYS,      // 99
    HAL_CMU_MOD_Q_PWM4,         // 100
    HAL_CMU_MOD_Q_PWM5,         // 101
    HAL_CMU_MOD_Q_PWM6,         // 102
    HAL_CMU_MOD_Q_PWM7,         // 103
    HAL_CMU_MOD_Q_NANDFLASH,    // 104
    // XCLK/XRST
    HAL_CMU_MOD_X_CORE0,        // 105
    HAL_CMU_MOD_X_DSP,          // 106
    HAL_CMU_MOD_X_GPU,          // 107
    HAL_CMU_MOD_X_DMAX,         // 108
    HAL_CMU_MOD_X_NIC,          // 109
    HAL_CMU_MOD_X_DMA2D,        // 110
    HAL_CMU_MOD_X_DISP,         // 111
    HAL_CMU_MOD_X_DISPB,        // 112
    HAL_CMU_MOD_X_EMMC,         // 113
    HAL_CMU_MOD_X_FLASH_NA,     // 114
    HAL_CMU_MOD_X_CODEC,        // 115
    HAL_CMU_MOD_X_RAM0,         // 116
    HAL_CMU_MOD_X_RAM1,         // 117
    HAL_CMU_MOD_X_RAM2,         // 118
    HAL_CMU_MOD_X_RAM3,         // 119
    HAL_CMU_MOD_X_RAM4,         // 120
    HAL_CMU_MOD_X_RAM5,         // 121
    HAL_CMU_MOD_X_PSRAM,        // 122

    // AON ACLK/ARST
    HAL_CMU_AON_A_CMU,          // 123
    HAL_CMU_AON_A_GPIO0,        // 124
    HAL_CMU_AON_A_GPIO0_INT,    // 125
    HAL_CMU_AON_A_WDT,          // 126
    HAL_CMU_AON_A_TIMER0,       // 127
    HAL_CMU_AON_A_PSC,          // 128
    HAL_CMU_AON_A_IOMUX,        // 129
    HAL_CMU_AON_A_APBC,         // 130
    HAL_CMU_AON_A_H2H_MCU,      // 131
    HAL_CMU_AON_A_I2C_SLV,      // 132
    HAL_CMU_AON_A_TZC,          // 133
    HAL_CMU_AON_A_TIMER1,       // 134
    HAL_CMU_AON_A_H2H_BTC,      // 135
    HAL_CMU_AON_A_SYS_GPIO0,    // 136
    HAL_CMU_AON_A_SYS_GPIO0_INT,// 137
    HAL_CMU_AON_A_SYS_GPIO1,    // 138
    HAL_CMU_AON_A_SYS_GPIO1_INT,// 139
    HAL_CMU_AON_A_SYS_IOMUX,    // 140
    HAL_CMU_AON_A_EFUSE,        // 141
    // AON OCLK/ORST
    HAL_CMU_AON_O_WDT,          // 142
    HAL_CMU_AON_O_TIMER0,       // 143
    HAL_CMU_AON_O_GPIO0,        // 144
    HAL_CMU_AON_O_IOMUX,        // 145
    HAL_CMU_AON_O_BTAON,        // 146
    HAL_CMU_AON_O_PSC,          // 147
    HAL_CMU_AON_O_TIMER1,       // 148
    HAL_CMU_AON_O_SYS_GPIO0,    // 149
    HAL_CMU_AON_O_SYS_GPIO1,    // 150
    HAL_CMU_AON_O_SYS_IOMUX,    // 151
    HAL_CMU_AON_O_EFUSE,        // 152
    HAL_CMU_AON_O_PMU,          // 153
    HAL_CMU_AON_O_USB,          // 154
    // AON SUBSYS
    HAL_CMU_AON_MCU,            // 155
    HAL_CMU_AON_CODEC,          // 156
    HAL_CMU_AON_SENS,           // 157
    HAL_CMU_AON_BT,             // 158
    HAL_CMU_AON_MCUCPU,         // 159
    HAL_CMU_AON_SENSCPU,        // 160
    HAL_CMU_AON_BTCPU,          // 161
    HAL_CMU_AON_BTH,            // 162
    HAL_CMU_AON_BTHCPU,         // 163
    HAL_CMU_AON_MCUDSP,         // 164
    HAL_CMU_AON_GLOBAL,         // 165

    // BTH
    HAL_CMU_BTH_H_FLASH1,       // 166
    HAL_CMU_BTH_O_FLASH1,       // 167

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_AON_GLOBAL,
    HAL_CMU_MOD_BT = HAL_CMU_AON_BT,
    HAL_CMU_MOD_BTCPU = HAL_CMU_AON_BTCPU,

    HAL_CMU_MOD_O_PWM0 = HAL_CMU_MOD_Q_PWM0,
    HAL_CMU_MOD_O_PWM1 = HAL_CMU_MOD_Q_PWM1,
    HAL_CMU_MOD_O_PWM2 = HAL_CMU_MOD_Q_PWM2,
    HAL_CMU_MOD_O_PWM3 = HAL_CMU_MOD_Q_PWM3,

    HAL_CMU_MOD_O_PCM = HAL_CMU_MOD_Q_PCM,

    HAL_CMU_MOD_P_SPI = HAL_CMU_MOD_P_SPI0,
    HAL_CMU_MOD_O_SPI = HAL_CMU_MOD_O_SPI0,
    HAL_CMU_MOD_P_SLCD = HAL_CMU_MOD_P_SPI1,
    HAL_CMU_MOD_O_SLCD = HAL_CMU_MOD_O_SPI1,

    HAL_CMU_MOD_H_CODEC = HAL_CMU_MOD_X_CODEC,

    HAL_CMU_MOD_H_MCU = HAL_CMU_MOD_O_CORE0,

#ifdef ALT_BOOT_FLASH
    HAL_CMU_MOD_H_FLASH = HAL_CMU_BTH_H_FLASH1,
    HAL_CMU_MOD_O_FLASH = HAL_CMU_BTH_O_FLASH1,
    HAL_CMU_MOD_H_FLASH1 = HAL_CMU_MOD_H_SYS_FLASH,
    HAL_CMU_MOD_O_FLASH1 = HAL_CMU_MOD_O_SYS_FLASH,
#else
    HAL_CMU_MOD_H_FLASH = HAL_CMU_MOD_H_SYS_FLASH,
    HAL_CMU_MOD_O_FLASH = HAL_CMU_MOD_O_SYS_FLASH,
    HAL_CMU_MOD_H_FLASH1 = HAL_CMU_BTH_H_FLASH1,
    HAL_CMU_MOD_O_FLASH1 = HAL_CMU_BTH_O_FLASH1,
#endif

    HAL_CMU_MOD_P_TIMER2 = HAL_CMU_MOD_P_SYS_TIMER1,
    HAL_CMU_MOD_O_TIMER2 = HAL_CMU_MOD_O_SYS_TIMER1,
};

#define HAL_CMU_FREQ_32K                    HAL_CMU_AXI_FREQ_32K
#define HAL_CMU_FREQ_3M                     HAL_CMU_AXI_FREQ_3M
#define HAL_CMU_FREQ_3P25M                  HAL_CMU_AXI_FREQ_3P25M
#define HAL_CMU_FREQ_6M                     HAL_CMU_AXI_FREQ_6M
#define HAL_CMU_FREQ_6P5M                   HAL_CMU_AXI_FREQ_6P5M
#define HAL_CMU_FREQ_12M                    HAL_CMU_AXI_FREQ_12M
#define HAL_CMU_FREQ_13M                    HAL_CMU_AXI_FREQ_13M
#define HAL_CMU_FREQ_15M                    HAL_CMU_AXI_FREQ_15M
#define HAL_CMU_FREQ_18M                    HAL_CMU_AXI_FREQ_18M
#define HAL_CMU_FREQ_21M                    HAL_CMU_AXI_FREQ_21M
#define HAL_CMU_FREQ_24M                    HAL_CMU_AXI_FREQ_24M
#define HAL_CMU_FREQ_26M                    HAL_CMU_AXI_FREQ_26M
#define HAL_CMU_FREQ_30M                    HAL_CMU_AXI_FREQ_30M
#define HAL_CMU_FREQ_36M                    HAL_CMU_AXI_FREQ_36M
#define HAL_CMU_FREQ_42M                    HAL_CMU_AXI_FREQ_42M
#define HAL_CMU_FREQ_48M                    HAL_CMU_AXI_FREQ_48M
#define HAL_CMU_FREQ_52M                    HAL_CMU_AXI_FREQ_52M
#define HAL_CMU_FREQ_60M                    HAL_CMU_AXI_FREQ_60M
#define HAL_CMU_FREQ_72M                    HAL_CMU_AXI_FREQ_72M
#define HAL_CMU_FREQ_78M                    HAL_CMU_AXI_FREQ_78M
#define HAL_CMU_FREQ_84M                    HAL_CMU_AXI_FREQ_84M
#define HAL_CMU_FREQ_96M                    HAL_CMU_AXI_FREQ_96M
#define HAL_CMU_FREQ_104M                   HAL_CMU_AXI_FREQ_104M
#define HAL_CMU_FREQ_120M                   HAL_CMU_AXI_FREQ_120M
#define HAL_CMU_FREQ_144M                   HAL_CMU_AXI_FREQ_144M
#define HAL_CMU_FREQ_168M                   HAL_CMU_AXI_FREQ_168M
#define HAL_CMU_FREQ_192M                   HAL_CMU_AXI_FREQ_192M
#define HAL_CMU_FREQ_208M                   HAL_CMU_AXI_FREQ_208M
#ifdef AUD_PLL_DOUBLE
#define HAL_CMU_FREQ_240M                   HAL_CMU_AXI_FREQ_240M
#define HAL_CMU_FREQ_290M                   HAL_CMU_AXI_FREQ_290M
#define HAL_CMU_FREQ_340M                   HAL_CMU_AXI_FREQ_340M
#define HAL_CMU_FREQ_390M                   HAL_CMU_AXI_FREQ_390M
#endif
#define HAL_CMU_FREQ_QTY                    HAL_CMU_AXI_FREQ_QTY
#define HAL_CMU_FREQ_T                      HAL_CMU_AXI_FREQ_T

#define HAL_CMU_CLOCK_OUT_MCU_32K           HAL_CMU_CLOCK_OUT_SYS_32K
#define HAL_CMU_CLOCK_OUT_MCU_SYS           HAL_CMU_CLOCK_OUT_SYS_SYS
#define HAL_CMU_CLOCK_OUT_MCU_FLASH0        HAL_CMU_CLOCK_OUT_SYS_FLASH0
#define HAL_CMU_CLOCK_OUT_MCU_USB           HAL_CMU_CLOCK_OUT_SYS_USB
#define HAL_CMU_CLOCK_OUT_MCU_PCLK          HAL_CMU_CLOCK_OUT_SYS_PCLK
#define HAL_CMU_CLOCK_OUT_MCU_I2S0          HAL_CMU_CLOCK_OUT_SYS_I2S0
#define HAL_CMU_CLOCK_OUT_MCU_CORE          HAL_CMU_CLOCK_OUT_SYS_CORE
#define HAL_CMU_CLOCK_OUT_MCU_SPDIF0        HAL_CMU_CLOCK_OUT_SYS_SPDIF0
#define HAL_CMU_CLOCK_OUT_MCU_SDMMC         HAL_CMU_CLOCK_OUT_SYS_SDMMC
#define HAL_CMU_CLOCK_OUT_MCU_ISPI          HAL_CMU_CLOCK_OUT_SYS_ISPI
#define HAL_CMU_CLOCK_OUT_MCU_SPI1          HAL_CMU_CLOCK_OUT_SYS_SPI1
#define HAL_CMU_CLOCK_OUT_MCU_SPI0          HAL_CMU_CLOCK_OUT_SYS_SPI0
#define HAL_CMU_CLOCK_OUT_MCU_PSRAM         HAL_CMU_CLOCK_OUT_SYS_PSRAM
#define HAL_CMU_CLOCK_OUT_MCU_PIX           HAL_CMU_CLOCK_OUT_SYS_PIX
#define HAL_CMU_CLOCK_OUT_MCU_DSI           HAL_CMU_CLOCK_OUT_SYS_DSI
#define HAL_CMU_CLOCK_OUT_MCU_EMMC          HAL_CMU_CLOCK_OUT_SYS_EMMC
#define HAL_CMU_CLOCK_OUT_MCU_GPU           HAL_CMU_CLOCK_OUT_SYS_GPU
#define HAL_CMU_CLOCK_OUT_MCU_LCDCP         HAL_CMU_CLOCK_OUT_SYS_LCDCP
#define HAL_CMU_CLOCK_OUT_MCU_QSPI          HAL_CMU_CLOCK_OUT_SYS_QSPI
#define HAL_CMU_CLOCK_OUT_MCU_PCM           HAL_CMU_CLOCK_OUT_SYS_PCM
#define HAL_CMU_CLOCK_OUT_MCU_I2S1          HAL_CMU_CLOCK_OUT_SYS_I2S1

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(hal_maincmu)

void hal_cmu_hifi4_sleep_signal_set(void);

void hal_cmu_hifi4_sleep_signal_clear(void);

int hal_cmu_hifi4_sleep_signal_get(void);

void hal_cmu_hifi4_wakeup_allow_signal_set(void);

void hal_cmu_hifi4_wakeup_allow_signal_clear(void);

int hal_cmu_hifi4_wakeup_allow_signal_get(void);

void hal_cmu_hifi4_wakeup_mask_get(uint32_t mask[3]);

uint32_t hal_cmu_sys_get_gpio0_irq_tick();

uint32_t hal_cmu_sys_get_gpio1_irq_tick();

#ifdef __cplusplus
}
#endif

#endif

