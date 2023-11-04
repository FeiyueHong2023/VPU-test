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
#ifndef __HAL_SENSCMU_BEST1603_H__
#define __HAL_SENSCMU_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_CMU_DEFAULT_CRYSTAL_FREQ        24000000

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_MCU,          // 0
    HAL_CMU_MOD_H_RAM0,         // 1
    HAL_CMU_MOD_H_RAM1,         // 2
    HAL_CMU_MOD_H_RAM2,         // 3
    HAL_CMU_MOD_H_RAM3,         // 4
    HAL_CMU_MOD_H_RAM4,         // 5
    HAL_CMU_MOD_H_AHB0,         // 6
    HAL_CMU_MOD_H_AHB1,         // 7
    HAL_CMU_MOD_H_AH2H_BT,      // 8
    HAL_CMU_MOD_H_ADMA,         // 9
    HAL_CMU_MOD_H_CODEC,        // 10
    HAL_CMU_MOD_H_SENSOR_ENG0,  // 11
    HAL_CMU_MOD_H_SENSOR_ENG1,  // 12
    HAL_CMU_MOD_H_SENSOR_ENG2,  // 13
    HAL_CMU_MOD_H_SENSOR_ENG3,  // 14
    HAL_CMU_MOD_H_TEP,          // 15
    HAL_CMU_MOD_H_AVS,          // 16
    HAL_CMU_MOD_H_A2A,          // 17
    HAL_CMU_MOD_H_VAD,          // 18
    HAL_CMU_MOD_H_RAM5,         // 19
    HAL_CMU_MOD_H_SPI_AHB,      // 20
    HAL_CMU_MOD_H_DUMPCPU,      // 21
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 22
    HAL_CMU_MOD_P_WDT,          // 23
    HAL_CMU_MOD_P_SENS_TIMER0,  // 24
    HAL_CMU_MOD_P_SENS_TIMER1,  // 25
    HAL_CMU_MOD_P_I2C0,         // 26
    HAL_CMU_MOD_P_I2C1,         // 27
    HAL_CMU_MOD_P_I2C2,         // 28
    HAL_CMU_MOD_P_I2C3,         // 29
    HAL_CMU_MOD_P_I2C4,         // 30
    HAL_CMU_MOD_P_SPI,          // 31
    HAL_CMU_MOD_P_SLCD,         // 32
    HAL_CMU_MOD_P_SPI_ITN,      // 33
    HAL_CMU_MOD_P_UART0,        // 34
    HAL_CMU_MOD_P_UART1,        // 35
    HAL_CMU_MOD_P_UART2,        // 36
    HAL_CMU_MOD_P_TCNT,         // 37
    HAL_CMU_MOD_P_I2S0,         // 38
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 39
    HAL_CMU_MOD_O_WDT,          // 40
    HAL_CMU_MOD_O_SENS_TIMER0,  // 41
    HAL_CMU_MOD_O_SENS_TIMER1,  // 42
    HAL_CMU_MOD_O_SENS_TIMER2,  // 43
    HAL_CMU_MOD_O_I2C0,         // 44
    HAL_CMU_MOD_O_I2C1,         // 45
    HAL_CMU_MOD_O_I2C2,         // 46
    HAL_CMU_MOD_O_I2C3,         // 47
    HAL_CMU_MOD_O_I2C4,         // 48
    HAL_CMU_MOD_O_SPI,          // 49
    HAL_CMU_MOD_O_SLCD,         // 50
    HAL_CMU_MOD_O_SPI_ITN,      // 51
    HAL_CMU_MOD_O_UART0,        // 52
    HAL_CMU_MOD_O_UART1,        // 53
    HAL_CMU_MOD_O_UART2,        // 54
    HAL_CMU_MOD_O_I2S0,         // 55
    HAL_CMU_MOD_O_ADC_FREE,     // 56
    HAL_CMU_MOD_O_ADC_CH0,      // 57
    HAL_CMU_MOD_O_ADC_CH1,      // 58
    HAL_CMU_MOD_O_ADC_ANA,      // 59
    HAL_CMU_MOD_O_SADC_ANA,     // 60
    HAL_CMU_MOD_O_CAL32K,       // 61
    HAL_CMU_MOD_O_CAL26M,       // 62

    // AON ACLK/ARST
    HAL_CMU_AON_A_CMU,          // 63
    HAL_CMU_AON_A_GPIO0,        // 64
    HAL_CMU_AON_A_GPIO0_INT,    // 65
    HAL_CMU_AON_A_WDT,          // 66
    HAL_CMU_AON_A_TIMER0,       // 67
    HAL_CMU_AON_A_PSC,          // 68
    HAL_CMU_AON_A_IOMUX,        // 69
    HAL_CMU_AON_A_APBC,         // 70
    HAL_CMU_AON_A_H2H_MCU,      // 71
    HAL_CMU_AON_A_I2C_SLV,      // 72
    HAL_CMU_AON_A_TZC,          // 73
    HAL_CMU_AON_A_TIMER1,       // 74
    HAL_CMU_AON_A_H2H_BTC,      // 75
    HAL_CMU_AON_A_SYS_GPIO0,    // 76
    HAL_CMU_AON_A_SYS_GPIO0_INT,// 77
    HAL_CMU_AON_A_SYS_GPIO1,    // 78
    HAL_CMU_AON_A_SYS_GPIO1_INT,// 79
    HAL_CMU_AON_A_SYS_IOMUX,    // 80
    HAL_CMU_AON_A_EFUSE,        // 81
    // AON OCLK/ORST
    HAL_CMU_AON_O_WDT,          // 82
    HAL_CMU_AON_O_TIMER0,       // 83
    HAL_CMU_AON_O_GPIO0,        // 84
    HAL_CMU_AON_O_IOMUX,        // 85
    HAL_CMU_AON_O_BTAON,        // 86
    HAL_CMU_AON_O_PSC,          // 87
    HAL_CMU_AON_O_TIMER1,       // 88
    HAL_CMU_AON_O_SYS_GPIO0,    // 89
    HAL_CMU_AON_O_SYS_GPIO1,    // 90
    HAL_CMU_AON_O_SYS_IOMUX,    // 91
    HAL_CMU_AON_O_EFUSE,        // 92
    HAL_CMU_AON_O_PMU,          // 93
    HAL_CMU_AON_O_USB,          // 94
    // AON SUBSYS
    HAL_CMU_AON_MCU,            // 95
    HAL_CMU_AON_CODEC,          // 96
    HAL_CMU_AON_SENS,           // 97
    HAL_CMU_AON_BT,             // 98
    HAL_CMU_AON_MCUCPU,         // 99
    HAL_CMU_AON_SENSCPU,        // 100
    HAL_CMU_AON_BTCPU,          // 101
    HAL_CMU_AON_BTH,            // 102
    HAL_CMU_AON_BTHCPU,         // 103
    HAL_CMU_AON_MCUDSP,         // 104
    HAL_CMU_AON_GLOBAL,         // 105

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_AON_GLOBAL,

    HAL_CMU_MOD_P_TIMER2 = HAL_CMU_MOD_P_SENS_TIMER1,
    HAL_CMU_MOD_O_TIMER2 = HAL_CMU_MOD_O_SENS_TIMER1,
};

enum HAL_CMU_I2S_MCLK_ID_T {
    HAL_CMU_I2S_MCLK_OSCD2              = 0x00,
    HAL_CMU_I2S_MCLK_OSC                = 0x01,
    HAL_CMU_I2S_MCLK_PLLI2S0            = 0x02,
    HAL_CMU_I2S_MCLK_PLLMCLK            = 0x03,
};
#define HAL_CMU_I2S_MCLK_ID_T               HAL_CMU_I2S_MCLK_ID_T

enum HAL_I2S_ID_T {
    HAL_I2S_ID_0 = 0,
    HAL_I2S_ID_1,

    HAL_I2S_ID_QTY,
};
#define HAL_I2S_ID_T                        HAL_I2S_ID_T

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

enum HAL_CMU_PLL_T {
    HAL_CMU_PLL_BB = 0,
    HAL_CMU_PLL_AUD = HAL_CMU_PLL_BB,

    HAL_CMU_PLL_QTY
};
#define HAL_CMU_PLL_T                       HAL_CMU_PLL_T

enum HAL_CMU_PLL_USER_T {
    HAL_CMU_PLL_USER_SYS,
    HAL_CMU_PLL_USER_AUD,

    HAL_CMU_PLL_USER_QTY,
    HAL_CMU_PLL_USER_ALL = HAL_CMU_PLL_USER_QTY,
};
#define HAL_CMU_PLL_USER_T                  HAL_CMU_PLL_USER_T

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(hal_maincmu)

int hal_cmu_fast_timer_init(void);

enum HAL_CMU_PLL_T hal_cmu_get_audio_pll(void);

void hal_cmu_codec_high_speed_enable(void);

void hal_cmu_codec_high_speed_disable(void);

void hal_cmu_mcu_pdm_clock_out(uint32_t clk_map);

void hal_cmu_sens_pdm_clock_out(uint32_t clk_map);

void hal_cmu_mcu_pdm_data_in(uint32_t data_map);

void hal_cmu_sens_pdm_data_in(uint32_t data_map);

uint32_t hal_cmu_get_aon_chip_id(void);

uint32_t hal_cmu_get_aon_revision_id(void);

#ifdef __cplusplus
}
#endif

#endif

