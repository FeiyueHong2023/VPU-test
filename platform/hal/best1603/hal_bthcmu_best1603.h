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
#ifndef __HAL_BTHCMU_BEST1603_H__
#define __HAL_BTHCMU_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_CORE0,        // 0
    HAL_CMU_MOD_H_CACHE0,       // 1
    HAL_CMU_MOD_H_FLASH_SYS,    // 2
    HAL_CMU_MOD_H_FLASH_BTH,    // 3
    HAL_CMU_MOD_H_GDMA,         // 4
    HAL_CMU_MOD_H_AHB0,         // 5
    HAL_CMU_MOD_H_AHB1,         // 6
    HAL_CMU_MOD_H_AH2H_BT,      // 7
    HAL_CMU_MOD_H_AH2H_SENS,    // 8
    HAL_CMU_MOD_H_BT_TPORT,     // 9
    HAL_CMU_MOD_H_BT_DUMP,      // 10
    HAL_CMU_MOD_H_CODEC,        // 11
    HAL_CMU_MOD_H_RAM0,         // 12
    HAL_CMU_MOD_H_RAM1,         // 13
    HAL_CMU_MOD_H_RAM2,         // 14
    HAL_CMU_MOD_H_RAM3,         // 15
    HAL_CMU_MOD_H_RAM4,         // 16
    HAL_CMU_MOD_H_RAM5,         // 17
    HAL_CMU_MOD_H_RAM6,         // 18
    HAL_CMU_MOD_H_AH2H_SYS,     // 19
    HAL_CMU_MOD_H_CP,           // 20
    HAL_CMU_MOD_H_BECO0,        // 21
    HAL_CMU_MOD_H_BECO1,        // 22
    HAL_CMU_MOD_H_DUMPCPU,      // 23
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 24
    HAL_CMU_MOD_P_WDT,          // 25
    HAL_CMU_MOD_P_BTH_TIMER0,   // 26
    HAL_CMU_MOD_P_BTH_TIMER1,   // 27
    HAL_CMU_MOD_P_SPI_ITN,      // 28
    HAL_CMU_MOD_P_UART0,        // 29
    HAL_CMU_MOD_P_UART1,        // 30
    HAL_CMU_MOD_P_PCM,          // 31
    HAL_CMU_MOD_P_I2C0,         // 32
    HAL_CMU_MOD_P_I2S0_FPGA,    // 33
    HAL_CMU_MOD_P_I2C1,         // 34
    HAL_CMU_MOD_P_SPI,         // 35
    HAL_CMU_MOD_P_SLCD,         // 36
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 37
    HAL_CMU_MOD_O_FLASH_BTH,    // 38
    HAL_CMU_MOD_O_WDT,          // 39
    HAL_CMU_MOD_O_BTH_TIMER0,   // 40
    HAL_CMU_MOD_O_BTH_TIMER1,   // 41
    HAL_CMU_MOD_O_SPI_ITN,      // 42
    HAL_CMU_MOD_O_UART0,        // 43
    HAL_CMU_MOD_O_UART1,        // 44
    HAL_CMU_MOD_O_PCM,          // 45
    HAL_CMU_MOD_O_I2C0,         // 46
    HAL_CMU_MOD_O_I2S0_FPGA,    // 47
    HAL_CMU_MOD_O_SPI,         // 48
    HAL_CMU_MOD_O_SLCD,         // 49
    HAL_CMU_MOD_O_I2C1,         // 50

    // AON ACLK/ARST
    HAL_CMU_AON_A_CMU,          // 51
    HAL_CMU_AON_A_GPIO0,        // 52
    HAL_CMU_AON_A_GPIO0_INT,    // 53
    HAL_CMU_AON_A_WDT,          // 54
    HAL_CMU_AON_A_TIMER0,       // 55
    HAL_CMU_AON_A_PSC,          // 56
    HAL_CMU_AON_A_IOMUX,        // 57
    HAL_CMU_AON_A_APBC,         // 58
    HAL_CMU_AON_A_H2H_MCU,      // 59
    HAL_CMU_AON_A_I2C_SLV,      // 60
    HAL_CMU_AON_A_TZC,          // 61
    HAL_CMU_AON_A_TIMER1,       // 62
    HAL_CMU_AON_A_H2H_BTC,      // 63
    HAL_CMU_AON_A_SYS_GPIO0,    // 64
    HAL_CMU_AON_A_SYS_GPIO0_INT,// 65
    HAL_CMU_AON_A_SYS_GPIO1,    // 66
    HAL_CMU_AON_A_SYS_GPIO1_INT,// 67
    HAL_CMU_AON_A_SYS_IOMUX,    // 68
    HAL_CMU_AON_A_EFUSE,        // 69
    // AON OCLK/ORST
    HAL_CMU_AON_O_WDT,          // 70
    HAL_CMU_AON_O_TIMER0,       // 71
    HAL_CMU_AON_O_GPIO0,        // 72
    HAL_CMU_AON_O_IOMUX,        // 73
    HAL_CMU_AON_O_BTAON,        // 74
    HAL_CMU_AON_O_PSC,          // 75
    HAL_CMU_AON_O_TIMER1,       // 76
    HAL_CMU_AON_O_SYS_GPIO0,    // 77
    HAL_CMU_AON_O_SYS_GPIO1,    // 78
    HAL_CMU_AON_O_SYS_IOMUX,    // 79
    HAL_CMU_AON_O_EFUSE,        // 80
    HAL_CMU_AON_O_PMU,          // 81
    HAL_CMU_AON_O_USB,          // 82
    // AON SUBSYS
    HAL_CMU_AON_MCU,            // 83
    HAL_CMU_AON_CODEC,          // 84
    HAL_CMU_AON_SENS,           // 85
    HAL_CMU_AON_BT,             // 86
    HAL_CMU_AON_MCUCPU,         // 87
    HAL_CMU_AON_SENSCPU,        // 88
    HAL_CMU_AON_BTCPU,          // 89
    HAL_CMU_AON_BTH,            // 90
    HAL_CMU_AON_BTHCPU,         // 91
    HAL_CMU_AON_MCUDSP,         // 92
    HAL_CMU_AON_GLOBAL,         // 93

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    // SYS HCLK/HRST
    HAL_CMU_SYS_H_AHB0,         // 94
    HAL_CMU_SYS_H_AHB1,         // 95
    HAL_CMU_SYS_H_AHB2,         // 96
    HAL_CMU_SYS_H_DMA0,         // 97
    HAL_CMU_SYS_H_DMA1,         // 98
    HAL_CMU_SYS_H_AH2H_AON,     // 99
    HAL_CMU_SYS_H_FLASH,        // 100
    HAL_CMU_SYS_H_SDMMC,        // 101
    HAL_CMU_SYS_H_USBC,         // 102
    HAL_CMU_SYS_H_USBH,         // 103
    HAL_CMU_SYS_H_CODEC_NA,     // 104
    HAL_CMU_SYS_H_SEC_ENG,      // 105
    HAL_CMU_SYS_H_GPU,          // 106
    HAL_CMU_SYS_H_LCDC,         // 107
    HAL_CMU_SYS_H_DISPPRE,      // 108
    HAL_CMU_SYS_H_DISP,         // 109
    HAL_CMU_SYS_H_DMA2D,        // 110
    HAL_CMU_SYS_H_AHB_S,        // 111
    HAL_CMU_SYS_H_PSRAM,        // 112
    HAL_CMU_SYS_H_SPI_AHB,      // 113
    HAL_CMU_SYS_H_NANDFLASH,    // 114
    HAL_CMU_SYS_H_DUMP_PC,      // 115
    // SYS PCLK/PRST
    HAL_CMU_SYS_P_CMU,          // 116
    HAL_CMU_SYS_P_WDT,          // 117
    HAL_CMU_SYS_P_SYS_TIMER0,   // 118
    HAL_CMU_SYS_P_SYS_TIMER1,   // 119
    HAL_CMU_SYS_P_SYS_TIMER2,   // 120
    HAL_CMU_SYS_P_SYS_TIMER3,   // 121
    HAL_CMU_SYS_P_I2C0,         // 122
    HAL_CMU_SYS_P_I2C1,         // 123
    HAL_CMU_SYS_P_SPI0,         // 124
    HAL_CMU_SYS_P_SPI1,         // 125
    HAL_CMU_SYS_P_SPI_ITN,      // 126
    HAL_CMU_SYS_P_UART0,        // 127
    HAL_CMU_SYS_P_UART1,        // 128
    HAL_CMU_SYS_P_TRNG,         // 129
    HAL_CMU_SYS_P_I2S0,         // 130
    HAL_CMU_SYS_P_I2S1,         // 131
    HAL_CMU_SYS_P_SPDIF0,       // 132
    HAL_CMU_SYS_P_GPIO0,        // 133
    HAL_CMU_SYS_P_GPIO0_INT,    // 134
    HAL_CMU_SYS_P_GPIO1,        // 135
    HAL_CMU_SYS_P_GPIO1_INT,    // 136
    HAL_CMU_SYS_P_IOMUX,        // 137
    HAL_CMU_SYS_P_SEC_ENG,      // 138
    HAL_CMU_SYS_P_TZC,          // 139
    HAL_CMU_SYS_P_PCM,          // 140
    HAL_CMU_SYS_P_IRQCTL,       // 141
    HAL_CMU_SYS_P_PWM,          // 142
    HAL_CMU_SYS_P_TCNT,         // 143
    HAL_CMU_SYS_P_PWM1,         // 144
    // SYS OCLK/ORST
    HAL_CMU_SYS_O_SLEEP,        // 145
    HAL_CMU_SYS_O_FLASH,        // 146
    HAL_CMU_SYS_O_USB,          // 147
    HAL_CMU_SYS_O_SDMMC,        // 148
    HAL_CMU_SYS_O_WDT,          // 149
    HAL_CMU_SYS_O_SYS_TIMER0,   // 150
    HAL_CMU_SYS_O_SYS_TIMER1,   // 151
    HAL_CMU_SYS_O_SYS_TIMER2,   // 152
    HAL_CMU_SYS_O_SYS_TIMER3,   // 153
    HAL_CMU_SYS_O_I2C0,         // 154
    HAL_CMU_SYS_O_I2C1,         // 155
    HAL_CMU_SYS_O_SPI0,         // 156
    HAL_CMU_SYS_O_SPI1,         // 157
    HAL_CMU_SYS_O_SPI_ITN,      // 158
    HAL_CMU_SYS_O_UART0,        // 159
    HAL_CMU_SYS_O_UART1,        // 160
    HAL_CMU_SYS_O_EMMC,         // 161
    HAL_CMU_SYS_O_I2S0,         // 162
    HAL_CMU_SYS_O_I2S1,         // 163
    HAL_CMU_SYS_O_SPDIF0,       // 164
    HAL_CMU_SYS_O_USB32K,       // 165
    HAL_CMU_SYS_O_PSRAM,        // 166
    HAL_CMU_SYS_O_CORE0,        // 167
    HAL_CMU_SYS_O_CORETCM,      // 168
    HAL_CMU_SYS_O_COREROM,      // 169
    HAL_CMU_SYS_O_COREDBG,      // 170
    HAL_CMU_SYS_O_DSP,          // 171
    HAL_CMU_SYS_O_DSPTCM,       // 172
    HAL_CMU_SYS_O_GPUX1,        // 173
    HAL_CMU_SYS_O_GPIO0,        // 174
    HAL_CMU_SYS_O_GPIO1,        // 175
    HAL_CMU_SYS_O_IOMUX,        // 176
    // SYS QCLK/QRST
    HAL_CMU_SYS_Q_BTH2SYS,      // 177
    HAL_CMU_SYS_Q_BTH2FLS,      // 178
    HAL_CMU_SYS_Q_SES2SYS,      // 179
    HAL_CMU_SYS_Q_DISPN2D,      // 180
    HAL_CMU_SYS_Q_DISTV2D,      // 181
    HAL_CMU_SYS_Q_LCDC_P,       // 182
    HAL_CMU_SYS_Q_DIS_PN,       // 183
    HAL_CMU_SYS_Q_DIS_TV,       // 184
    HAL_CMU_SYS_Q_DIS_PIX,      // 185
    HAL_CMU_SYS_Q_DIS_DSI,      // 186
    HAL_CMU_SYS_Q_DIS_QSPI,     // 187
    HAL_CMU_SYS_Q_PWM0,         // 188
    HAL_CMU_SYS_Q_PWM1,         // 189
    HAL_CMU_SYS_Q_PWM2,         // 190
    HAL_CMU_SYS_Q_PWM3,         // 191
    HAL_CMU_SYS_Q_PCM,          // 192
    HAL_CMU_SYS_Q_BTC2SYS,      // 193
    HAL_CMU_MOD_Q_PWM4,         // 194
    HAL_CMU_MOD_Q_PWM5,         // 195
    HAL_CMU_MOD_Q_PWM6,         // 196
    HAL_CMU_MOD_Q_PWM7,         // 197
    HAL_CMU_MOD_Q_NANDFLASH,    // 198
    // SYS XCLK/XRST
    HAL_CMU_SYS_X_CORE0,        // 199
    HAL_CMU_SYS_X_DSP,          // 200
    HAL_CMU_SYS_X_GPU,          // 201
    HAL_CMU_SYS_X_DMAX,         // 202
    HAL_CMU_SYS_X_NIC,          // 203
    HAL_CMU_SYS_X_DMA2D,        // 204
    HAL_CMU_SYS_X_DISP,         // 205
    HAL_CMU_SYS_X_DISPB,        // 206
    HAL_CMU_SYS_X_EMMC,         // 207
    HAL_CMU_SYS_X_FLASH_NA,     // 208
    HAL_CMU_SYS_X_CODEC,        // 209
    HAL_CMU_SYS_X_RAM0,         // 210
    HAL_CMU_SYS_X_RAM1,         // 211
    HAL_CMU_SYS_X_RAM2,         // 212
    HAL_CMU_SYS_X_RAM3,         // 213
    HAL_CMU_SYS_X_RAM4,         // 214
    HAL_CMU_SYS_X_RAM5,         // 215
    HAL_CMU_SYS_X_PSRAM,        // 216
#endif

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_AON_GLOBAL,
    HAL_CMU_MOD_BT = HAL_CMU_AON_BT,
    HAL_CMU_MOD_BTCPU = HAL_CMU_AON_BTCPU,

    HAL_CMU_MOD_H_FLASH = HAL_CMU_MOD_H_FLASH_BTH,
    HAL_CMU_MOD_O_FLASH = HAL_CMU_MOD_O_FLASH_BTH,
    HAL_CMU_MOD_H_FLASH1 = HAL_CMU_MOD_H_FLASH_SYS,
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    HAL_CMU_MOD_O_FLASH1 = HAL_CMU_SYS_O_FLASH,
#else
    HAL_CMU_MOD_O_FLASH1 = HAL_CMU_MOD_QTY,
#endif

    HAL_CMU_MOD_P_TIMER2 = HAL_CMU_MOD_P_BTH_TIMER1,
    HAL_CMU_MOD_O_TIMER2 = HAL_CMU_MOD_O_BTH_TIMER1,

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    HAL_CMU_MOD_O_EMMC = HAL_CMU_SYS_O_EMMC,
    HAL_CMU_MOD_X_EMMC = HAL_CMU_SYS_X_EMMC,
    HAL_CMU_MOD_H_USBC = HAL_CMU_SYS_H_USBC,
    HAL_CMU_MOD_H_USBH = HAL_CMU_SYS_H_USBH,
    HAL_CMU_MOD_O_USB = HAL_CMU_SYS_O_USB,
    HAL_CMU_MOD_O_USB32K = HAL_CMU_SYS_O_USB32K,
    HAL_CMU_MOD_H_SDMMC = HAL_CMU_SYS_H_SDMMC,
    HAL_CMU_MOD_O_SDMMC = HAL_CMU_SYS_O_SDMMC,
    HAL_CMU_MOD_H_SEC_ENG = HAL_CMU_SYS_H_SEC_ENG,
    HAL_CMU_MOD_P_SEC_ENG = HAL_CMU_SYS_P_SEC_ENG,
    HAL_CMU_MOD_P_TRNG = HAL_CMU_SYS_P_TRNG,
    HAL_CMU_MOD_P_I2S0 = HAL_CMU_SYS_P_I2S0,
    HAL_CMU_MOD_O_I2S0 = HAL_CMU_SYS_O_I2S0,
    HAL_CMU_MOD_P_I2S1 = HAL_CMU_SYS_P_I2S1,
    HAL_CMU_MOD_O_I2S1 = HAL_CMU_SYS_O_I2S1,
    HAL_CMU_MOD_P_SPDIF0 = HAL_CMU_SYS_P_SPDIF0,
    HAL_CMU_MOD_O_SPDIF0 = HAL_CMU_SYS_O_SPDIF0,
    HAL_CMU_MOD_P_PWM = HAL_CMU_SYS_P_PWM,
    HAL_CMU_MOD_O_PWM0 = HAL_CMU_SYS_Q_PWM0,
    HAL_CMU_MOD_P_UART2 = HAL_CMU_SYS_P_UART0,
    HAL_CMU_MOD_O_UART2 = HAL_CMU_SYS_O_UART0,
#ifdef BTH_USE_SYS_SENS_PERIPH
    HAL_CMU_MOD_P_I2C2 = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_O_I2C2 = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_P_I2C3 = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_O_I2C3 = HAL_CMU_MOD_QTY,
#endif
#else
    HAL_CMU_MOD_O_EMMC = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_X_EMMC = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_H_SDMMC = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_O_SDMMC = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_H_SEC_ENG = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_P_SEC_ENG = HAL_CMU_MOD_QTY,
#endif

#ifdef FPGA
    HAL_CMU_MOD_P_I2C0 = HAL_CMU_MOD_P_I2C0_FPGA,
    HAL_CMU_MOD_O_I2C0 = HAL_CMU_MOD_O_I2C0_FPGA,
    HAL_CMU_MOD_P_I2S0 = HAL_CMU_MOD_P_I2S0_FPGA,
    HAL_CMU_MOD_O_I2S0 = HAL_CMU_MOD_O_I2S0_FPGA,
#endif
};

enum HAL_CMU_FREQ_T {
    HAL_CMU_FREQ_32K,
    HAL_CMU_FREQ_6M,
    HAL_CMU_FREQ_6P5M = HAL_CMU_FREQ_6M,
    HAL_CMU_FREQ_12M,
    HAL_CMU_FREQ_13M = HAL_CMU_FREQ_12M,
    HAL_CMU_FREQ_15M,
    HAL_CMU_FREQ_18M,
    HAL_CMU_FREQ_21M,
    HAL_CMU_FREQ_24M,
    HAL_CMU_FREQ_26M = HAL_CMU_FREQ_24M,
    HAL_CMU_FREQ_30M,
    HAL_CMU_FREQ_36M,
    HAL_CMU_FREQ_42M,
    HAL_CMU_FREQ_48M,
    HAL_CMU_FREQ_52M = HAL_CMU_FREQ_48M,
    HAL_CMU_FREQ_60M,
    HAL_CMU_FREQ_72M,
    HAL_CMU_FREQ_78M = HAL_CMU_FREQ_72M,
    HAL_CMU_FREQ_84M,
    HAL_CMU_FREQ_96M,
    HAL_CMU_FREQ_104M = HAL_CMU_FREQ_96M,
    HAL_CMU_FREQ_120M,
    HAL_CMU_FREQ_144M,
    HAL_CMU_FREQ_168M,
    HAL_CMU_FREQ_192M,
    HAL_CMU_FREQ_208M = HAL_CMU_FREQ_192M,
#ifdef AUD_PLL_DOUBLE
    HAL_CMU_FREQ_240M,
    HAL_CMU_FREQ_290M,
    HAL_CMU_FREQ_340M,
    HAL_CMU_FREQ_390M,
#endif

    HAL_CMU_FREQ_QTY
};
#define HAL_CMU_FREQ_T                      HAL_CMU_FREQ_T
#define HAL_CMU_FREQ_15M                    HAL_CMU_FREQ_15M

#define HAL_CMU_CLOCK_OUT_MCU_32K           HAL_CMU_CLOCK_OUT_BTH_32K
#define HAL_CMU_CLOCK_OUT_MCU_SYS           HAL_CMU_CLOCK_OUT_BTH_SYS
#define HAL_CMU_CLOCK_OUT_MCU_FLASH0        HAL_CMU_CLOCK_OUT_BTH_FLASH0
#define HAL_CMU_CLOCK_OUT_MCU_PCLK          HAL_CMU_CLOCK_OUT_BTH_PCLK
#define HAL_CMU_CLOCK_OUT_MCU_PCM           HAL_CMU_CLOCK_OUT_BTH_PCM
#define HAL_CMU_CLOCK_OUT_MCU_ISPI          HAL_CMU_CLOCK_OUT_BTH_ISPI
#define HAL_CMU_CLOCK_OUT_MCU_I2S0          HAL_CMU_CLOCK_OUT_SYS_I2S0
#define HAL_CMU_CLOCK_OUT_MCU_SPDIF0        HAL_CMU_CLOCK_OUT_SYS_SPDIF0

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(hal_maincmu)

#ifdef __cplusplus
}
#endif

#endif

