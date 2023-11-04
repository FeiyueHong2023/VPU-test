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
#ifndef __REG_BTHCMU_BEST1603_H__
#define __REG_BTHCMU_BEST1603_H__

#include "plat_types.h"

struct BTHCMU_T {
    __IO uint32_t HCLK_ENABLE;      // 0x00
    __IO uint32_t HCLK_DISABLE;     // 0x04
    __IO uint32_t PCLK_ENABLE;      // 0x08
    __IO uint32_t PCLK_DISABLE;     // 0x0C
    __IO uint32_t OCLK_ENABLE;      // 0x10
    __IO uint32_t OCLK_DISABLE;     // 0x14
    __IO uint32_t HCLK_MODE;        // 0x18
    __IO uint32_t PCLK_MODE;        // 0x1C
    __IO uint32_t OCLK_MODE;        // 0x20
    __IO uint32_t REG_RAM_CFG0;     // 0x24
    __IO uint32_t HRESET_PULSE;     // 0x28
    __IO uint32_t PRESET_PULSE;     // 0x2C
    __IO uint32_t ORESET_PULSE;     // 0x30
    __IO uint32_t HRESET_SET;       // 0x34
    __IO uint32_t HRESET_CLR;       // 0x38
    __IO uint32_t PRESET_SET;       // 0x3C
    __IO uint32_t PRESET_CLR;       // 0x40
    __IO uint32_t ORESET_SET;       // 0x44
    __IO uint32_t ORESET_CLR;       // 0x48
    __IO uint32_t RESERVED_04C;     // 0x4C
    __IO uint32_t BOOTMODE;         // 0x50
    __IO uint32_t MCU_TIMER;        // 0x54
    __IO uint32_t SLEEP;            // 0x58
    __IO uint32_t PERIPH_CLK;       // 0x5C
    __IO uint32_t SYS_CLK_ENABLE;   // 0x60
    __IO uint32_t SYS_CLK_DISABLE;  // 0x64
    __IO uint32_t SYS_DIV;          // 0x68
    __IO uint32_t REG_RAM_CFG1;     // 0x6C
    __IO uint32_t UART_CLK;         // 0x70
    __IO uint32_t CPU_CFG;          // 0x74
    __IO uint32_t MCU2SENS_MASK0;   // 0x78
    __IO uint32_t MCU2SENS_MASK1;   // 0x7C
    __IO uint32_t WRITE_UNLOCK;     // 0x80
    __IO uint32_t WAKEUP_MASK0;     // 0x84
    __IO uint32_t WAKEUP_MASK1;     // 0x88
    __IO uint32_t WAKEUP_CLK_CFG;   // 0x8C
    __IO uint32_t RESERVED_090[4];  // 0x90
    __IO uint32_t ISIRQ_SET;        // 0xA0
    __IO uint32_t ISIRQ_CLR;        // 0xA4
    __IO uint32_t RESERVED_0A8;     // 0xA8
    __IO uint32_t RESERVED_0AC;     // 0xAC
    __IO uint32_t MCU2BT_INTMASK0;  // 0xB0
    __IO uint32_t MCU2BT_INTMASK1;  // 0xB4
    __IO uint32_t RESERVED_0B8;     // 0xB8
    __IO uint32_t RESERVED_0BC;     // 0xBC
    __IO uint32_t MEMSC[4];         // 0xC0
    __I  uint32_t MEMSC_STATUS;     // 0xD0
    __IO uint32_t RESERVED_0D4[3];  // 0xD4
    __IO uint32_t ADMA_CH0_4_REQ;   // 0xE0
    __IO uint32_t ADMA_CH5_9_REQ;   // 0xE4
    __IO uint32_t ADMA_CH10_14_REQ; // 0xE8
    __IO uint32_t ADMA_CH15_REQ;    // 0xEC
    __IO uint32_t MISC;             // 0xF0
    __IO uint32_t SIMU_RES;         // 0xF4
    __IO uint32_t MISC_0F8;         // 0xF8
    __IO uint32_t RESERVED_0FC[6];  // 0xFC
    __IO uint32_t REG_RAM_CFG2;     // 0x114
    __IO uint32_t TPORT_IRQ_LEN;    // 0x118
    __IO uint32_t TPORT_CUR_ADDR;   // 0x11C
    __IO uint32_t TPORT_START;      // 0x120
    __IO uint32_t TPORT_END;        // 0x124
    __IO uint32_t MCU2SENS_IRQ_SET; // 0x128
    __IO uint32_t MCU2SENS_IRQ_CLR; // 0x12C
    __IO uint32_t TPORT_CTRL;       // 0x130
    __IO uint32_t BTH2SYS_IRQ_SET;  // 0x134
    __IO uint32_t BTH2SYS_IRQ_CLR;  // 0x138
    __IO uint32_t BTH2SYS_MASK0;    // 0x13C
    __IO uint32_t BTH2SYS_MASK1;    // 0x140
    __IO uint32_t BT_TOG_IRQ;       // 0x144
    __IO uint32_t BT_TOG_IRQ_CLR;   // 0x148
    __IO uint32_t CP_VTOR;          // 0x14C
    __IO uint32_t ROM_CFG;          // 0x150
    __IO uint32_t WAKEUP_MCU2BT_MASK2;// 0x154
    __IO uint32_t MCU2SENS_BTH2SYS_MASK2;// 0x158
    __IO uint32_t RESERVED_15C;     // 0x15C
    __IO uint32_t CP2MCU_IRQ_SET;   // 0x160
    __IO uint32_t CP2MCU_IRQ_CLR;   // 0x164
    __IO uint32_t MCU2CP_IRQ_SET;   // 0x168
    __IO uint32_t MCU2CP_IRQ_CLR;   // 0x16C
    __IO uint32_t SPI_CLK;          // 0x170
    __IO uint32_t I2C_CLK;          // 0x174
    __IO uint32_t PVT_CFG;          // 0x178
    __IO uint32_t PVT_COUNT;        // 0x17C
};

struct SAVED_BTHCMU_REGS_T {
    uint32_t HCLK_ENABLE;
    uint32_t PCLK_ENABLE;
    uint32_t OCLK_ENABLE;
    uint32_t HCLK_MODE;
    uint32_t PCLK_MODE;
    uint32_t OCLK_MODE;
    uint32_t REG_RAM_CFG0;
    uint32_t HRESET_CLR;
    uint32_t PRESET_CLR;
    uint32_t ORESET_CLR;
    uint32_t MCU_TIMER;
    uint32_t SLEEP;
    uint32_t PERIPH_CLK;
    uint32_t SYS_CLK_ENABLE;
    uint32_t SYS_DIV;
    uint32_t REG_RAM_CFG1;
    uint32_t UART_CLK;
    uint32_t CPU_CFG;
    uint32_t MCU2SENS_MASK0;
    uint32_t MCU2SENS_MASK1;
    uint32_t WAKEUP_CLK_CFG;
    uint32_t MCU2BT_INTMASK0;
    uint32_t MCU2BT_INTMASK1;
    uint32_t ADMA_CH0_4_REQ;
    uint32_t ADMA_CH5_9_REQ;
    uint32_t ADMA_CH10_14_REQ;
    uint32_t ADMA_CH15_REQ;
    uint32_t MISC;
    uint32_t SIMU_RES;
    uint32_t MISC_0F8;
    uint32_t REG_RAM_CFG2;
    uint32_t TPORT_IRQ_LEN;
    uint32_t TPORT_CUR_ADDR;
    uint32_t TPORT_START;
    uint32_t TPORT_END;
    uint32_t TPORT_CTRL;
    uint32_t BTH2SYS_MASK0;
    uint32_t BTH2SYS_MASK1;
    uint32_t BT_TOG_IRQ;
    uint32_t BT_TOG_IRQ_CLR;
    uint32_t CP_VTOR;
    uint32_t ROM_CFG;
    uint32_t WAKEUP_MCU2BT_MASK2;
    uint32_t MCU2SENS_BTH2SYS_MASK2;
    uint32_t RESERVED_15C;
    uint32_t CP2MCU_IRQ_SET;
    uint32_t CP2MCU_IRQ_CLR;
    uint32_t MCU2CP_IRQ_SET;
    uint32_t MCU2CP_IRQ_CLR;
    uint32_t SPI_CLK;
    uint32_t I2C_CLK;
    uint32_t PVT_CFG;
    uint32_t PVT_COUNT;
};

// reg_000
#define BTH_CMU_MANUAL_HCLK_ENABLE(n)                       (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MANUAL_HCLK_ENABLE_MASK                     (0xFFFFFFFF << 0)
#define BTH_CMU_MANUAL_HCLK_ENABLE_SHIFT                    (0)

// reg_004
#define BTH_CMU_MANUAL_HCLK_DISABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MANUAL_HCLK_DISABLE_MASK                    (0xFFFFFFFF << 0)
#define BTH_CMU_MANUAL_HCLK_DISABLE_SHIFT                   (0)

// reg_008
#define BTH_CMU_MANUAL_PCLK_ENABLE(n)                       (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MANUAL_PCLK_ENABLE_MASK                     (0xFFFFFFFF << 0)
#define BTH_CMU_MANUAL_PCLK_ENABLE_SHIFT                    (0)

// reg_00c
#define BTH_CMU_MANUAL_PCLK_DISABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MANUAL_PCLK_DISABLE_MASK                    (0xFFFFFFFF << 0)
#define BTH_CMU_MANUAL_PCLK_DISABLE_SHIFT                   (0)

// reg_010
#define BTH_CMU_MANUAL_OCLK_ENABLE(n)                       (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MANUAL_OCLK_ENABLE_MASK                     (0xFFFFFFFF << 0)
#define BTH_CMU_MANUAL_OCLK_ENABLE_SHIFT                    (0)

// reg_014
#define BTH_CMU_MANUAL_OCLK_DISABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MANUAL_OCLK_DISABLE_MASK                    (0xFFFFFFFF << 0)
#define BTH_CMU_MANUAL_OCLK_DISABLE_SHIFT                   (0)

// reg_018
#define BTH_CMU_MODE_HCLK(n)                                (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MODE_HCLK_MASK                              (0xFFFFFFFF << 0)
#define BTH_CMU_MODE_HCLK_SHIFT                             (0)

// reg_01c
#define BTH_CMU_MODE_PCLK(n)                                (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MODE_PCLK_MASK                              (0xFFFFFFFF << 0)
#define BTH_CMU_MODE_PCLK_SHIFT                             (0)

// reg_020
#define BTH_CMU_MODE_OCLK(n)                                (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MODE_OCLK_MASK                              (0xFFFFFFFF << 0)
#define BTH_CMU_MODE_OCLK_SHIFT                             (0)

// reg_024
#define BTH_CMU_RF_RET1N0(n)                                (((n) & 0xF) << 0)
#define BTH_CMU_RF_RET1N0_MASK                              (0xF << 0)
#define BTH_CMU_RF_RET1N0_SHIFT                             (0)
#define BTH_CMU_RF_RET2N0(n)                                (((n) & 0xF) << 4)
#define BTH_CMU_RF_RET2N0_MASK                              (0xF << 4)
#define BTH_CMU_RF_RET2N0_SHIFT                             (4)
#define BTH_CMU_RF_PGEN0(n)                                 (((n) & 0xF) << 8)
#define BTH_CMU_RF_PGEN0_MASK                               (0xF << 8)
#define BTH_CMU_RF_PGEN0_SHIFT                              (8)
#define BTH_CMU_RF_RET1N1(n)                                (((n) & 0xF) << 12)
#define BTH_CMU_RF_RET1N1_MASK                              (0xF << 12)
#define BTH_CMU_RF_RET1N1_SHIFT                             (12)
#define BTH_CMU_RF_RET2N1(n)                                (((n) & 0xF) << 16)
#define BTH_CMU_RF_RET2N1_MASK                              (0xF << 16)
#define BTH_CMU_RF_RET2N1_SHIFT                             (16)
#define BTH_CMU_RF_PGEN1(n)                                 (((n) & 0xF) << 20)
#define BTH_CMU_RF_PGEN1_MASK                               (0xF << 20)
#define BTH_CMU_RF_PGEN1_SHIFT                              (20)
#define BTH_CMU_RF_EMAA(n)                                  (((n) & 0x7) << 24)
#define BTH_CMU_RF_EMAA_MASK                                (0x7 << 24)
#define BTH_CMU_RF_EMAA_SHIFT                               (24)
#define BTH_CMU_RF_EMAB(n)                                  (((n) & 0x7) << 27)
#define BTH_CMU_RF_EMAB_MASK                                (0x7 << 27)
#define BTH_CMU_RF_EMAB_SHIFT                               (27)
#define BTH_CMU_RF_EMASA                                    (1 << 30)

// reg_028
#define BTH_CMU_HRESETN_PULSE(n)                            (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_HRESETN_PULSE_MASK                          (0xFFFFFFFF << 0)
#define BTH_CMU_HRESETN_PULSE_SHIFT                         (0)

#define BTH_PRST_NUM                                        13

// reg_02c
#define BTH_CMU_PRESETN_PULSE(n)                            (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_PRESETN_PULSE_MASK                          (0xFFFFFFFF << 0)
#define BTH_CMU_PRESETN_PULSE_SHIFT                         (0)
#define BTH_CMU_GLOBAL_RESETN_PULSE                         (1 << (BTH_PRST_NUM+1-1))

// reg_030
#define BTH_CMU_ORESETN_PULSE(n)                            (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_ORESETN_PULSE_MASK                          (0xFFFFFFFF << 0)
#define BTH_CMU_ORESETN_PULSE_SHIFT                         (0)

// reg_034
#define BTH_CMU_HRESETN_SET(n)                              (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_HRESETN_SET_MASK                            (0xFFFFFFFF << 0)
#define BTH_CMU_HRESETN_SET_SHIFT                           (0)

// reg_038
#define BTH_CMU_HRESETN_CLR(n)                              (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_HRESETN_CLR_MASK                            (0xFFFFFFFF << 0)
#define BTH_CMU_HRESETN_CLR_SHIFT                           (0)

// reg_03c
#define BTH_CMU_PRESETN_SET(n)                              (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_PRESETN_SET_MASK                            (0xFFFFFFFF << 0)
#define BTH_CMU_PRESETN_SET_SHIFT                           (0)
#define BTH_CMU_GLOBAL_RESETN_SET                           (1 << (BTH_PRST_NUM+1-1))

// reg_040
#define BTH_CMU_PRESETN_CLR(n)                              (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_PRESETN_CLR_MASK                            (0xFFFFFFFF << 0)
#define BTH_CMU_PRESETN_CLR_SHIFT                           (0)
#define BTH_CMU_GLOBAL_RESETN_CLR                           (1 << (BTH_PRST_NUM+1-1))

// reg_044
#define BTH_CMU_ORESETN_SET(n)                              (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_ORESETN_SET_MASK                            (0xFFFFFFFF << 0)
#define BTH_CMU_ORESETN_SET_SHIFT                           (0)

// reg_048
#define BTH_CMU_ORESETN_CLR(n)                              (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_ORESETN_CLR_MASK                            (0xFFFFFFFF << 0)
#define BTH_CMU_ORESETN_CLR_SHIFT                           (0)

// reg_050
#define BTH_CMU_WATCHDOG_RESET                              (1 << 0)
#define BTH_CMU_SOFT_GLOBLE_RESET                           (1 << 1)
#define BTH_CMU_RTC_INTR_H                                  (1 << 2)
#define BTH_CMU_CHG_INTR_H                                  (1 << 3)
#define BTH_CMU_SOFT_BOOT_MODE(n)                           (((n) & 0xFFFFFFF) << 4)
#define BTH_CMU_SOFT_BOOT_MODE_MASK                         (0xFFFFFFF << 4)
#define BTH_CMU_SOFT_BOOT_MODE_SHIFT                        (4)

// reg_054
#define BTH_CMU_CFG_HCLK_MCU_OFF_TIMER(n)                   (((n) & 0xFF) << 0)
#define BTH_CMU_CFG_HCLK_MCU_OFF_TIMER_MASK                 (0xFF << 0)
#define BTH_CMU_CFG_HCLK_MCU_OFF_TIMER_SHIFT                (0)
#define BTH_CMU_HCLK_MCU_ENABLE                             (1 << 8)
#define BTH_CMU_RAM_RETN_UP_EARLY                           (1 << 9)
#define BTH_CMU_FLS_SEC_MSK_EN                              (1 << 10)
#define BTH_CMU_DEBUG_REG_SEL(n)                            (((n) & 0xF) << 11)
#define BTH_CMU_DEBUG_REG_SEL_MASK                          (0xF << 11)
#define BTH_CMU_DEBUG_REG_SEL_SHIFT                         (11)

// reg_058
#define BTH_CMU_SLEEP_TIMER(n)                              (((n) & 0xFFFFFF) << 0)
#define BTH_CMU_SLEEP_TIMER_MASK                            (0xFFFFFF << 0)
#define BTH_CMU_SLEEP_TIMER_SHIFT                           (0)
#define BTH_CMU_SLEEP_TIMER_EN                              (1 << 24)
#define BTH_CMU_DEEPSLEEP_EN                                (1 << 25)
#define BTH_CMU_DEEPSLEEP_ROMRAM_EN                         (1 << 26)
#define BTH_CMU_MANUAL_RAM_RETN                             (1 << 27)
#define BTH_CMU_DEEPSLEEP_START                             (1 << 28)
#define BTH_CMU_DEEPSLEEP_MODE                              (1 << 29)
#define BTH_CMU_PU_OSC                                      (1 << 30)
#define BTH_CMU_WAKEUP_DEEPSLEEP_L                          (1 << 31)

// reg_05c
#define BTH_CMU_CFG_DIV_PER(n)                              (((n) & 0x3) << 0)
#define BTH_CMU_CFG_DIV_PER_MASK                            (0x3 << 0)
#define BTH_CMU_CFG_DIV_PER_SHIFT                           (0)
#define BTH_CMU_POL_CLK_PCM_IN                              (1 << 2)
#define BTH_CMU_FORCE_PU_OFF                                (1 << 3)
#define BTH_CMU_LOCK_CPU_EN                                 (1 << 4)
#define BTH_CMU_SEL_32K_TIMER(n)                            (((n) & 0x7) << 5)
#define BTH_CMU_SEL_32K_TIMER_MASK                          (0x7 << 5)
#define BTH_CMU_SEL_32K_TIMER_SHIFT                         (5)
#define BTH_CMU_SEL_32K_WDT                                 (1 << 8)
#define BTH_CMU_SEL_TIMER_FAST(n)                           (((n) & 0x7) << 9)
#define BTH_CMU_SEL_TIMER_FAST_MASK                         (0x7 << 9)
#define BTH_CMU_SEL_TIMER_FAST_SHIFT                        (9)
#define BTH_CMU_CFG_CLK_OUT(n)                              (((n) & 0x1F) << 12)
#define BTH_CMU_CFG_CLK_OUT_MASK                            (0x1F << 12)
#define BTH_CMU_CFG_CLK_OUT_SHIFT                           (12)
#define BTH_CMU_MASK_OBS(n)                                 (((n) & 0x3F) << 17)
#define BTH_CMU_MASK_OBS_MASK                               (0x3F << 17)
#define BTH_CMU_MASK_OBS_SHIFT                              (17)
#define BTH_CMU_SEL_OSCX2_SPI2                              (1 << 23)
#define BTH_CMU_BT_PLAYTIME_STAMP_MASK                      (1 << 24)
#define BTH_CMU_BT_PLAYTIME_STAMP1_MASK                     (1 << 25)
#define BTH_CMU_BT_PLAYTIME_STAMP2_MASK                     (1 << 26)
#define BTH_CMU_BT_PLAYTIME_STAMP3_MASK                     (1 << 27)
#define BTH_CMU_BT_SYNC_MASK                                (1 << 28)
#define BTH_CMU_CFG_TIMER_FAST(n)                           (((n) & 0x3) << 29)
#define BTH_CMU_CFG_TIMER_FAST_MASK                         (0x3 << 29)
#define BTH_CMU_CFG_TIMER_FAST_SHIFT                        (29)

// reg_060
#define BTH_CMU_RSTN_DIV_FLS0_ENABLE                        (1 << 0)
#define BTH_CMU_SEL_OSCX4_FLS0_ENABLE                       (1 << 1)
#define BTH_CMU_SEL_OSCX2_FLS0_ENABLE                       (1 << 2)
#define BTH_CMU_SEL_PLL_FLS0_ENABLE                         (1 << 3)
#define BTH_CMU_BYPASS_DIV_FLS0_ENABLE                      (1 << 4)
#define BTH_CMU_RSTN_DIV_BTH_ENABLE                         (1 << 5)
#define BTH_CMU_SEL_OSC_2_BTH_ENABLE                        (1 << 6)
#define BTH_CMU_SEL_OSC_4_BTH_ENABLE                        (1 << 7)
#define BTH_CMU_SEL_SLOW_BTH_ENABLE                         (1 << 8)
#define BTH_CMU_SEL_OSCX2_BTH_ENABLE                        (1 << 9)
#define BTH_CMU_SEL_FAST_BTH_ENABLE                         (1 << 10)
#define BTH_CMU_SEL_PLL_BTH_ENABLE                          (1 << 11)
#define BTH_CMU_BYPASS_DIV_BTH_ENABLE                       (1 << 12)
#define BTH_CMU_EN_OSCX2_ENABLE                             (1 << 13)
#define BTH_CMU_PU_OSCX2_ENABLE                             (1 << 14)
#define BTH_CMU_EN_OSCX4_ENABLE                             (1 << 15)
#define BTH_CMU_PU_OSCX4_ENABLE                             (1 << 16)
#define BTH_CMU_EN_PLL_ENABLE                               (1 << 17)
#define BTH_CMU_PU_PLL_ENABLE                               (1 << 18)
#define BTH_CMU_SEL_PLL_FLS0_FAST_ENABLE                    (1 << 19)
#define BTH_CMU_RSTN_DIV_PER_ENABLE                         (1 << 20)
#define BTH_CMU_BYPASS_DIV_PER_ENABLE                       (1 << 21)

// reg_064
#define BTH_CMU_RSTN_DIV_FLS0_DISABLE                       (1 << 0)
#define BTH_CMU_SEL_OSCX4_FLS0_DISABLE                      (1 << 1)
#define BTH_CMU_SEL_OSCX2_FLS0_DISABLE                      (1 << 2)
#define BTH_CMU_SEL_PLL_FLS0_DISABLE                        (1 << 3)
#define BTH_CMU_BYPASS_DIV_FLS0_DISABLE                     (1 << 4)
#define BTH_CMU_RSTN_DIV_BTH_DISABLE                        (1 << 5)
#define BTH_CMU_SEL_OSC_2_BTH_DISABLE                       (1 << 6)
#define BTH_CMU_SEL_OSC_4_BTH_DISABLE                       (1 << 7)
#define BTH_CMU_SEL_SLOW_BTH_DISABLE                        (1 << 8)
#define BTH_CMU_SEL_OSCX2_BTH_DISABLE                       (1 << 9)
#define BTH_CMU_SEL_FAST_BTH_DISABLE                        (1 << 10)
#define BTH_CMU_SEL_PLL_BTH_DISABLE                         (1 << 11)
#define BTH_CMU_BYPASS_DIV_BTH_DISABLE                      (1 << 12)
#define BTH_CMU_EN_OSCX2_DISABLE                            (1 << 13)
#define BTH_CMU_PU_OSCX2_DISABLE                            (1 << 14)
#define BTH_CMU_EN_OSCX4_DISABLE                            (1 << 15)
#define BTH_CMU_PU_OSCX4_DISABLE                            (1 << 16)
#define BTH_CMU_EN_PLL_DISABLE                              (1 << 17)
#define BTH_CMU_PU_PLL_DISABLE                              (1 << 18)
#define BTH_CMU_SEL_PLL_FLS0_FAST_DISABLE                   (1 << 19)
#define BTH_CMU_RSTN_DIV_PER_DISABLE                        (1 << 20)
#define BTH_CMU_BYPASS_DIV_PER_DISABLE                      (1 << 21)

// reg_068
#define BTH_CMU_CFG_DIV_BTH(n)                              (((n) & 0x3) << 0)
#define BTH_CMU_CFG_DIV_BTH_MASK                            (0x3 << 0)
#define BTH_CMU_CFG_DIV_BTH_SHIFT                           (0)
#define BTH_CMU_CFG_DIV_FLS0(n)                             (((n) & 0x3) << 2)
#define BTH_CMU_CFG_DIV_FLS0_MASK                           (0x3 << 2)
#define BTH_CMU_CFG_DIV_FLS0_SHIFT                          (2)
#define BTH_CMU_CFG_DIV_PCLK(n)                             (((n) & 0x3) << 4)
#define BTH_CMU_CFG_DIV_PCLK_MASK                           (0x3 << 4)
#define BTH_CMU_CFG_DIV_PCLK_SHIFT                          (4)
#define BTH_CMU_MASK_OBS_GPIO_CORE0(n)                      (((n) & 0x3F) << 6)
#define BTH_CMU_MASK_OBS_GPIO_CORE0_MASK                    (0x3F << 6)
#define BTH_CMU_MASK_OBS_GPIO_CORE0_SHIFT                   (6)
#define BTH_CMU_WDT_NMI_MASK(n)                             (((n) & 0x3) << 12)
#define BTH_CMU_WDT_NMI_MASK_MASK                           (0x3 << 12)
#define BTH_CMU_WDT_NMI_MASK_SHIFT                          (12)
#define BTH_CMU_SEL_SYS_GT(n)                               (((n) & 0x3) << 14)
#define BTH_CMU_SEL_SYS_GT_MASK                             (0x3 << 14)
#define BTH_CMU_SEL_SYS_GT_SHIFT                            (14)
#define BTH_CMU_BT_TOG_TRIG0_MASK                           (1 << 16)
#define BTH_CMU_BT_TOG_TRIG1_MASK                           (1 << 17)
#define BTH_CMU_BT_TOG_TRIG2_MASK                           (1 << 18)
#define BTH_CMU_BT_TOG_TRIG3_MASK                           (1 << 19)

// reg_06c
#define BTH_CMU_RF2_RET1N0(n)                               (((n) & 0x3) << 0)
#define BTH_CMU_RF2_RET1N0_MASK                             (0x3 << 0)
#define BTH_CMU_RF2_RET1N0_SHIFT                            (0)
#define BTH_CMU_RF2_RET2N0(n)                               (((n) & 0x3) << 2)
#define BTH_CMU_RF2_RET2N0_MASK                             (0x3 << 2)
#define BTH_CMU_RF2_RET2N0_SHIFT                            (2)
#define BTH_CMU_RF2_PGEN0(n)                                (((n) & 0x3) << 4)
#define BTH_CMU_RF2_PGEN0_MASK                              (0x3 << 4)
#define BTH_CMU_RF2_PGEN0_SHIFT                             (4)
#define BTH_CMU_RF2_RET1N1(n)                               (((n) & 0x3) << 6)
#define BTH_CMU_RF2_RET1N1_MASK                             (0x3 << 6)
#define BTH_CMU_RF2_RET1N1_SHIFT                            (6)
#define BTH_CMU_RF2_RET2N1(n)                               (((n) & 0x3) << 8)
#define BTH_CMU_RF2_RET2N1_MASK                             (0x3 << 8)
#define BTH_CMU_RF2_RET2N1_SHIFT                            (8)
#define BTH_CMU_RF2_PGEN1(n)                                (((n) & 0x3) << 10)
#define BTH_CMU_RF2_PGEN1_MASK                              (0x3 << 10)
#define BTH_CMU_RF2_PGEN1_SHIFT                             (10)
#define BTH_CMU_RF_EMA(n)                                   (((n) & 0x7) << 12)
#define BTH_CMU_RF_EMA_MASK                                 (0x7 << 12)
#define BTH_CMU_RF_EMA_SHIFT                                (12)
#define BTH_CMU_RF_EMAW(n)                                  (((n) & 0x3) << 15)
#define BTH_CMU_RF_EMAW_MASK                                (0x3 << 15)
#define BTH_CMU_RF_EMAW_SHIFT                               (15)
#define BTH_CMU_RF_WABL                                     (1 << 17)
#define BTH_CMU_RF_WABLM(n)                                 (((n) & 0x3) << 18)
#define BTH_CMU_RF_WABLM_MASK                               (0x3 << 18)
#define BTH_CMU_RF_WABLM_SHIFT                              (18)
#define BTH_CMU_RF_EMAS                                     (1 << 20)
#define BTH_CMU_RF_RAWL                                     (1 << 21)
#define BTH_CMU_RF_RAWLM(n)                                 (((n) & 0x3) << 22)
#define BTH_CMU_RF_RAWLM_MASK                               (0x3 << 22)
#define BTH_CMU_RF_RAWLM_SHIFT                              (22)

// reg_070
#define BTH_CMU_CFG_DIV_UART0(n)                            (((n) & 0x3) << 0)
#define BTH_CMU_CFG_DIV_UART0_MASK                          (0x3 << 0)
#define BTH_CMU_CFG_DIV_UART0_SHIFT                         (0)
#define BTH_CMU_SEL_OSCX2_UART0                             (1 << 2)
#define BTH_CMU_SEL_PLL_UART0                               (1 << 3)
#define BTH_CMU_EN_PLL_UART0                                (1 << 4)
#define BTH_CMU_CFG_DIV_UART1(n)                            (((n) & 0x3) << 5)
#define BTH_CMU_CFG_DIV_UART1_MASK                          (0x3 << 5)
#define BTH_CMU_CFG_DIV_UART1_SHIFT                         (5)
#define BTH_CMU_SEL_OSCX2_UART1                             (1 << 7)
#define BTH_CMU_SEL_PLL_UART1                               (1 << 8)
#define BTH_CMU_EN_PLL_UART1                                (1 << 9)

// reg_074
#define BTH_CMU_CFG_FPU                                     (1 << 0)
#define BTH_CMU_CFG_DSP                                     (1 << 1)
#define BTH_CMU_CFG_DBGEN                                   (1 << 2)
#define BTH_CMU_CFG_IDEN                                    (1 << 3)
#define BTH_CMU_CFG_SEC                                     (1 << 4)
#define BTH_CMU_CFG_FPU_CORE1                               (1 << 5)
#define BTH_CMU_CFG_DSP_CORE1                               (1 << 6)
#define BTH_CMU_CFG_DBGEN_CORE1                             (1 << 7)
#define BTH_CMU_CFG_IDEN_CORE1                              (1 << 8)
#define BTH_CMU_JTAG_SEL_BTH_CP                             (1 << 9)
#define BTH_CMU_BECO_CLK_AUTO_EN_CORE0                      (1 << 10)
#define BTH_CMU_BECO_CLK_AUTO_EN_CORE1                      (1 << 11)
#define BTH_CMU_BYPASS_CG_RAM                               (1 << 12)

// reg_078
#define BTH_CMU_MCU2SENS_INTISR_MASK0(n)                    (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MCU2SENS_INTISR_MASK0_MASK                  (0xFFFFFFFF << 0)
#define BTH_CMU_MCU2SENS_INTISR_MASK0_SHIFT                 (0)

// reg_07c
#define BTH_CMU_MCU2SENS_INTISR_MASK1(n)                    (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MCU2SENS_INTISR_MASK1_MASK                  (0xFFFFFFFF << 0)
#define BTH_CMU_MCU2SENS_INTISR_MASK1_SHIFT                 (0)

// reg_080
#define BTH_CMU_WRITE_UNLOCK_H                              (1 << 0)
#define BTH_CMU_WRITE_UNLOCK_STATUS                         (1 << 1)

// reg_084
#define BTH_CMU_WAKEUP_IRQ_MASK0(n)                         (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_WAKEUP_IRQ_MASK0_MASK                       (0xFFFFFFFF << 0)
#define BTH_CMU_WAKEUP_IRQ_MASK0_SHIFT                      (0)

// reg_088
#define BTH_CMU_WAKEUP_IRQ_MASK1(n)                         (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_WAKEUP_IRQ_MASK1_MASK                       (0xFFFFFFFF << 0)
#define BTH_CMU_WAKEUP_IRQ_MASK1_SHIFT                      (0)

// reg_08c
#define BTH_CMU_TIMER_WT26(n)                               (((n) & 0xFF) << 0)
#define BTH_CMU_TIMER_WT26_MASK                             (0xFF << 0)
#define BTH_CMU_TIMER_WT26_SHIFT                            (0)
#define BTH_CMU_TIMER_WTPLL(n)                              (((n) & 0xF) << 8)
#define BTH_CMU_TIMER_WTPLL_MASK                            (0xF << 8)
#define BTH_CMU_TIMER_WTPLL_SHIFT                           (8)
#define BTH_CMU_LPU_AUTO_SWITCH26                           (1 << 12)
#define BTH_CMU_LPU_AUTO_SWITCHPLL                          (1 << 13)
#define BTH_CMU_LPU_STATUS_26M                              (1 << 14)
#define BTH_CMU_LPU_STATUS_PLL                              (1 << 15)
#define BTH_CMU_LPU_AUTO_MID                                (1 << 16)

// reg_0a0
#define BTH_CMU_BT2MCU_DATA_DONE_SET                        (1 << 0)
#define BTH_CMU_BT2MCU_DATA1_DONE_SET                       (1 << 1)
#define BTH_CMU_MCU2BT_DATA_IND_SET                         (1 << 2)
#define BTH_CMU_MCU2BT_DATA1_IND_SET                        (1 << 3)
#define BTH_CMU_BT_ALLIRQ_MASK_SET                          (1 << 4)
#define BTH_CMU_BT_PLAYTIME_STAMP_INTR                      (1 << 5)
#define BTH_CMU_BT_PLAYTIME_STAMP1_INTR                     (1 << 6)
#define BTH_CMU_BT_PLAYTIME_STAMP2_INTR                     (1 << 7)
#define BTH_CMU_BT_PLAYTIME_STAMP3_INTR                     (1 << 8)
#define BTH_CMU_BT_PLAYTIME_STAMP_INTR_MSK                  (1 << 9)
#define BTH_CMU_BT_PLAYTIME_STAMP1_INTR_MSK                 (1 << 10)
#define BTH_CMU_BT_PLAYTIME_STAMP2_INTR_MSK                 (1 << 11)
#define BTH_CMU_BT_PLAYTIME_STAMP3_INTR_MSK                 (1 << 12)
#define BTH_CMU_BT2MCU_DATA_MSK_SET                         (1 << 13)
#define BTH_CMU_BT2MCU_DATA1_MSK_SET                        (1 << 14)
#define BTH_CMU_MCU2BT_DATA_MSK_SET                         (1 << 15)
#define BTH_CMU_MCU2BT_DATA1_MSK_SET                        (1 << 16)
#define BTH_CMU_BT2MCU_DATA_INTR                            (1 << 17)
#define BTH_CMU_BT2MCU_DATA1_INTR                           (1 << 18)
#define BTH_CMU_MCU2BT_DATA_INTR                            (1 << 19)
#define BTH_CMU_MCU2BT_DATA1_INTR                           (1 << 20)
#define BTH_CMU_BT2MCU_DATA_INTR_MSK                        (1 << 21)
#define BTH_CMU_BT2MCU_DATA1_INTR_MSK                       (1 << 22)
#define BTH_CMU_MCU2BT_DATA_INTR_MSK                        (1 << 23)
#define BTH_CMU_MCU2BT_DATA1_INTR_MSK                       (1 << 24)
#define BTH_CMU_SENS_ALLIRQ_MASK_SET                        (1 << 25)
#define BTH_CMU_BT_SYNC_INTR                                (1 << 26)
#define BTH_CMU_BT_SYNC_INTR_MSK                            (1 << 27)

#ifndef CMU_BT2MCU_DATA_DONE_SET
#define CMU_BT2MCU_DATA_DONE_SET                            BTH_CMU_BT2MCU_DATA_DONE_SET
#define CMU_BT2MCU_DATA1_DONE_SET                           BTH_CMU_BT2MCU_DATA1_DONE_SET
#define CMU_MCU2BT_DATA_IND_SET                             BTH_CMU_MCU2BT_DATA_IND_SET
#define CMU_MCU2BT_DATA1_IND_SET                            BTH_CMU_MCU2BT_DATA1_IND_SET
#define CMU_BT2MCU_DATA_MSK_SET                             BTH_CMU_BT2MCU_DATA_MSK_SET
#define CMU_BT2MCU_DATA1_MSK_SET                            BTH_CMU_BT2MCU_DATA1_MSK_SET
#define CMU_MCU2BT_DATA_MSK_SET                             BTH_CMU_MCU2BT_DATA_MSK_SET
#define CMU_MCU2BT_DATA1_MSK_SET                            BTH_CMU_MCU2BT_DATA1_MSK_SET
#define CMU_BT2MCU_DATA_INTR_MSK                            BTH_CMU_BT2MCU_DATA_INTR_MSK
#define CMU_BT2MCU_DATA1_INTR_MSK                           BTH_CMU_BT2MCU_DATA1_INTR_MSK
#define CMU_MCU2BT_DATA_INTR_MSK                            BTH_CMU_MCU2BT_DATA_INTR_MSK
#define CMU_MCU2BT_DATA1_INTR_MSK                           BTH_CMU_MCU2BT_DATA1_INTR_MSK
#endif

// reg_0a4
#define BTH_CMU_BT2MCU_DATA_DONE_CLR                        (1 << 0)
#define BTH_CMU_BT2MCU_DATA1_DONE_CLR                       (1 << 1)
#define BTH_CMU_MCU2BT_DATA_IND_CLR                         (1 << 2)
#define BTH_CMU_MCU2BT_DATA1_IND_CLR                        (1 << 3)
#define BTH_CMU_BT_ALLIRQ_MASK_CLR                          (1 << 4)
#define BTH_CMU_BT_PLAYTIME_STAMP_INTR_CLR                  (1 << 5)
#define BTH_CMU_BT_PLAYTIME_STAMP1_INTR_CLR                 (1 << 6)
#define BTH_CMU_BT_PLAYTIME_STAMP2_INTR_CLR                 (1 << 7)
#define BTH_CMU_BT_PLAYTIME_STAMP3_INTR_CLR                 (1 << 8)
#define BTH_CMU_BT2MCU_DATA_MSK_CLR                         (1 << 13)
#define BTH_CMU_BT2MCU_DATA1_MSK_CLR                        (1 << 14)
#define BTH_CMU_MCU2BT_DATA_MSK_CLR                         (1 << 15)
#define BTH_CMU_MCU2BT_DATA1_MSK_CLR                        (1 << 16)
#define BTH_CMU_SENS_ALLIRQ_MASK_CLR                        (1 << 25)
#define BTH_CMU_BT_SYNC_INTR_CLR                            (1 << 26)

#ifndef CMU_BT2MCU_DATA_DONE_CLR
#define CMU_BT2MCU_DATA_DONE_CLR                            BTH_CMU_BT2MCU_DATA_DONE_CLR
#define CMU_BT2MCU_DATA1_DONE_CLR                           BTH_CMU_BT2MCU_DATA1_DONE_CLR
#define CMU_MCU2BT_DATA_IND_CLR                             BTH_CMU_MCU2BT_DATA_IND_CLR
#define CMU_MCU2BT_DATA1_IND_CLR                            BTH_CMU_MCU2BT_DATA1_IND_CLR
#define CMU_BT2MCU_DATA_MSK_CLR                             BTH_CMU_BT2MCU_DATA_MSK_CLR
#define CMU_BT2MCU_DATA1_MSK_CLR                            BTH_CMU_BT2MCU_DATA1_MSK_CLR
#define CMU_MCU2BT_DATA_MSK_CLR                             BTH_CMU_MCU2BT_DATA_MSK_CLR
#define CMU_MCU2BT_DATA1_MSK_CLR                            BTH_CMU_MCU2BT_DATA1_MSK_CLR
#endif

// reg_0b0
#define BTH_CMU_MCU2BT_INTISR_MASK0(n)                      (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MCU2BT_INTISR_MASK0_MASK                    (0xFFFFFFFF << 0)
#define BTH_CMU_MCU2BT_INTISR_MASK0_SHIFT                   (0)

// reg_0b4
#define BTH_CMU_MCU2BT_INTISR_MASK1(n)                      (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_MCU2BT_INTISR_MASK1_MASK                    (0xFFFFFFFF << 0)
#define BTH_CMU_MCU2BT_INTISR_MASK1_SHIFT                   (0)

// reg_0c0
#define BTH_CMU_MEMSC0                                      (1 << 0)

// reg_0c4
#define BTH_CMU_MEMSC1                                      (1 << 0)

// reg_0c8
#define BTH_CMU_MEMSC2                                      (1 << 0)

// reg_0cc
#define BTH_CMU_MEMSC3                                      (1 << 0)

// reg_0d0
#define BTH_CMU_MEMSC_STATUS0                               (1 << 0)
#define BTH_CMU_MEMSC_STATUS1                               (1 << 1)
#define BTH_CMU_MEMSC_STATUS2                               (1 << 2)
#define BTH_CMU_MEMSC_STATUS3                               (1 << 3)

// reg_0e0
#define BTH_CMU_GDMA_CH0_REQ_IDX(n)                         (((n) & 0x3F) << 0)
#define BTH_CMU_GDMA_CH0_REQ_IDX_MASK                       (0x3F << 0)
#define BTH_CMU_GDMA_CH0_REQ_IDX_SHIFT                      (0)
#define BTH_CMU_GDMA_CH1_REQ_IDX(n)                         (((n) & 0x3F) << 6)
#define BTH_CMU_GDMA_CH1_REQ_IDX_MASK                       (0x3F << 6)
#define BTH_CMU_GDMA_CH1_REQ_IDX_SHIFT                      (6)
#define BTH_CMU_GDMA_CH2_REQ_IDX(n)                         (((n) & 0x3F) << 12)
#define BTH_CMU_GDMA_CH2_REQ_IDX_MASK                       (0x3F << 12)
#define BTH_CMU_GDMA_CH2_REQ_IDX_SHIFT                      (12)
#define BTH_CMU_GDMA_CH3_REQ_IDX(n)                         (((n) & 0x3F) << 18)
#define BTH_CMU_GDMA_CH3_REQ_IDX_MASK                       (0x3F << 18)
#define BTH_CMU_GDMA_CH3_REQ_IDX_SHIFT                      (18)
#define BTH_CMU_GDMA_CH4_REQ_IDX(n)                         (((n) & 0x3F) << 24)
#define BTH_CMU_GDMA_CH4_REQ_IDX_MASK                       (0x3F << 24)
#define BTH_CMU_GDMA_CH4_REQ_IDX_SHIFT                      (24)

// reg_0e4
#define BTH_CMU_GDMA_CH5_REQ_IDX(n)                         (((n) & 0x3F) << 0)
#define BTH_CMU_GDMA_CH5_REQ_IDX_MASK                       (0x3F << 0)
#define BTH_CMU_GDMA_CH5_REQ_IDX_SHIFT                      (0)
#define BTH_CMU_GDMA_CH6_REQ_IDX(n)                         (((n) & 0x3F) << 6)
#define BTH_CMU_GDMA_CH6_REQ_IDX_MASK                       (0x3F << 6)
#define BTH_CMU_GDMA_CH6_REQ_IDX_SHIFT                      (6)
#define BTH_CMU_GDMA_CH7_REQ_IDX(n)                         (((n) & 0x3F) << 12)
#define BTH_CMU_GDMA_CH7_REQ_IDX_MASK                       (0x3F << 12)
#define BTH_CMU_GDMA_CH7_REQ_IDX_SHIFT                      (12)
#define BTH_CMU_GDMA_CH8_REQ_IDX(n)                         (((n) & 0x3F) << 18)
#define BTH_CMU_GDMA_CH8_REQ_IDX_MASK                       (0x3F << 18)
#define BTH_CMU_GDMA_CH8_REQ_IDX_SHIFT                      (18)
#define BTH_CMU_GDMA_CH9_REQ_IDX(n)                         (((n) & 0x3F) << 24)
#define BTH_CMU_GDMA_CH9_REQ_IDX_MASK                       (0x3F << 24)
#define BTH_CMU_GDMA_CH9_REQ_IDX_SHIFT                      (24)

// reg_0e8
#define BTH_CMU_GDMA_CH10_REQ_IDX(n)                        (((n) & 0x3F) << 0)
#define BTH_CMU_GDMA_CH10_REQ_IDX_MASK                      (0x3F << 0)
#define BTH_CMU_GDMA_CH10_REQ_IDX_SHIFT                     (0)
#define BTH_CMU_GDMA_CH11_REQ_IDX(n)                        (((n) & 0x3F) << 6)
#define BTH_CMU_GDMA_CH11_REQ_IDX_MASK                      (0x3F << 6)
#define BTH_CMU_GDMA_CH11_REQ_IDX_SHIFT                     (6)
#define BTH_CMU_GDMA_CH12_REQ_IDX(n)                        (((n) & 0x3F) << 12)
#define BTH_CMU_GDMA_CH12_REQ_IDX_MASK                      (0x3F << 12)
#define BTH_CMU_GDMA_CH12_REQ_IDX_SHIFT                     (12)
#define BTH_CMU_GDMA_CH13_REQ_IDX(n)                        (((n) & 0x3F) << 18)
#define BTH_CMU_GDMA_CH13_REQ_IDX_MASK                      (0x3F << 18)
#define BTH_CMU_GDMA_CH13_REQ_IDX_SHIFT                     (18)
#define BTH_CMU_GDMA_CH14_REQ_IDX(n)                        (((n) & 0x3F) << 24)
#define BTH_CMU_GDMA_CH14_REQ_IDX_MASK                      (0x3F << 24)
#define BTH_CMU_GDMA_CH14_REQ_IDX_SHIFT                     (24)

// reg_0ec
#define BTH_CMU_GDMA_CH15_REQ_IDX(n)                        (((n) & 0x3F) << 0)
#define BTH_CMU_GDMA_CH15_REQ_IDX_MASK                      (0x3F << 0)
#define BTH_CMU_GDMA_CH15_REQ_IDX_SHIFT                     (0)

// reg_0f0
#define BTH_CMU_RESERVED(n)                                 (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_RESERVED_MASK                               (0xFFFFFFFF << 0)
#define BTH_CMU_RESERVED_SHIFT                              (0)

// reg_0f4
#define BTH_CMU_DEBUG(n)                                    (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_DEBUG_MASK                                  (0xFFFFFFFF << 0)
#define BTH_CMU_DEBUG_SHIFT                                 (0)

// reg_0f8
#define BTH_CMU_RESERVED_3(n)                               (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_RESERVED_3_MASK                             (0xFFFFFFFF << 0)
#define BTH_CMU_RESERVED_3_SHIFT                            (0)

// reg_114
#define BTH_CMU_RF3_RET1N0                                  (1 << 0)
#define BTH_CMU_RF3_RET2N0                                  (1 << 1)
#define BTH_CMU_RF3_PGEN0                                   (1 << 2)
#define BTH_CMU_RF3_RET1N1                                  (1 << 3)
#define BTH_CMU_RF3_RET2N1                                  (1 << 4)
#define BTH_CMU_RF3_PGEN1                                   (1 << 5)

// reg_118
#define BTH_CMU_TPORT_INTR_LEN(n)                           (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_TPORT_INTR_LEN_MASK                         (0xFFFFFFFF << 0)
#define BTH_CMU_TPORT_INTR_LEN_SHIFT                        (0)

// reg_11c
#define BTH_CMU_TPORT_CURR_ADDR(n)                          (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_TPORT_CURR_ADDR_MASK                        (0xFFFFFFFF << 0)
#define BTH_CMU_TPORT_CURR_ADDR_SHIFT                       (0)

// reg_120
#define BTH_CMU_TPORT_START_ADDR(n)                         (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_TPORT_START_ADDR_MASK                       (0xFFFFFFFF << 0)
#define BTH_CMU_TPORT_START_ADDR_SHIFT                      (0)

// reg_124
#define BTH_CMU_TPORT_END_ADDR(n)                           (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_TPORT_END_ADDR_MASK                         (0xFFFFFFFF << 0)
#define BTH_CMU_TPORT_END_ADDR_SHIFT                        (0)

// reg_128
#define BTH_CMU_SENS2MCU_DATA_DONE_SET                      (1 << 0)
#define BTH_CMU_SENS2MCU_DATA1_DONE_SET                     (1 << 1)
#define BTH_CMU_SENS2MCU_DATA2_DONE_SET                     (1 << 2)
#define BTH_CMU_SENS2MCU_DATA3_DONE_SET                     (1 << 3)
#define BTH_CMU_MCU2SENS_DATA_IND_SET                       (1 << 4)
#define BTH_CMU_MCU2SENS_DATA1_IND_SET                      (1 << 5)
#define BTH_CMU_MCU2SENS_DATA2_IND_SET                      (1 << 6)
#define BTH_CMU_MCU2SENS_DATA3_IND_SET                      (1 << 7)
#define BTH_CMU_SENS2MCU_DATA_MSK_SET                       (1 << 8)
#define BTH_CMU_SENS2MCU_DATA1_MSK_SET                      (1 << 9)
#define BTH_CMU_SENS2MCU_DATA2_MSK_SET                      (1 << 10)
#define BTH_CMU_SENS2MCU_DATA3_MSK_SET                      (1 << 11)
#define BTH_CMU_MCU2SENS_DATA_MSK_SET                       (1 << 12)
#define BTH_CMU_MCU2SENS_DATA1_MSK_SET                      (1 << 13)
#define BTH_CMU_MCU2SENS_DATA2_MSK_SET                      (1 << 14)
#define BTH_CMU_MCU2SENS_DATA3_MSK_SET                      (1 << 15)
#define BTH_CMU_SENS2MCU_DATA_INTR                          (1 << 16)
#define BTH_CMU_SENS2MCU_DATA1_INTR                         (1 << 17)
#define BTH_CMU_SENS2MCU_DATA2_INTR                         (1 << 18)
#define BTH_CMU_SENS2MCU_DATA3_INTR                         (1 << 19)
#define BTH_CMU_MCU2SENS_DATA_INTR                          (1 << 20)
#define BTH_CMU_MCU2SENS_DATA1_INTR                         (1 << 21)
#define BTH_CMU_MCU2SENS_DATA2_INTR                         (1 << 22)
#define BTH_CMU_MCU2SENS_DATA3_INTR                         (1 << 23)
#define BTH_CMU_SENS2MCU_DATA_INTR_MSK                      (1 << 24)
#define BTH_CMU_SENS2MCU_DATA1_INTR_MSK                     (1 << 25)
#define BTH_CMU_SENS2MCU_DATA2_INTR_MSK                     (1 << 26)
#define BTH_CMU_SENS2MCU_DATA3_INTR_MSK                     (1 << 27)
#define BTH_CMU_MCU2SENS_DATA_INTR_MSK                      (1 << 28)
#define BTH_CMU_MCU2SENS_DATA1_INTR_MSK                     (1 << 29)
#define BTH_CMU_MCU2SENS_DATA2_INTR_MSK                     (1 << 30)
#define BTH_CMU_MCU2SENS_DATA3_INTR_MSK                     (1 << 31)

// reg_12c
#define BTH_CMU_SENS2MCU_DATA_DONE_CLR                      (1 << 0)
#define BTH_CMU_SENS2MCU_DATA1_DONE_CLR                     (1 << 1)
#define BTH_CMU_SENS2MCU_DATA2_DONE_CLR                     (1 << 2)
#define BTH_CMU_SENS2MCU_DATA3_DONE_CLR                     (1 << 3)
#define BTH_CMU_MCU2SENS_DATA_IND_CLR                       (1 << 4)
#define BTH_CMU_MCU2SENS_DATA1_IND_CLR                      (1 << 5)
#define BTH_CMU_MCU2SENS_DATA2_IND_CLR                      (1 << 6)
#define BTH_CMU_MCU2SENS_DATA3_IND_CLR                      (1 << 7)
#define BTH_CMU_SENS2MCU_DATA_MSK_CLR                       (1 << 8)
#define BTH_CMU_SENS2MCU_DATA1_MSK_CLR                      (1 << 9)
#define BTH_CMU_SENS2MCU_DATA2_MSK_CLR                      (1 << 10)
#define BTH_CMU_SENS2MCU_DATA3_MSK_CLR                      (1 << 11)
#define BTH_CMU_MCU2SENS_DATA_MSK_CLR                       (1 << 12)
#define BTH_CMU_MCU2SENS_DATA1_MSK_CLR                      (1 << 13)
#define BTH_CMU_MCU2SENS_DATA2_MSK_CLR                      (1 << 14)
#define BTH_CMU_MCU2SENS_DATA3_MSK_CLR                      (1 << 15)

#if !defined(CMU_HDR_CHECK_SUBSYS) || defined(CHIP_SUBSYS_BTH)
#define CMU_SENS2MCU_DATA_DONE_SET                          BTH_CMU_SENS2MCU_DATA_DONE_SET
#define CMU_SENS2MCU_DATA1_DONE_SET                         BTH_CMU_SENS2MCU_DATA1_DONE_SET
#define CMU_SENS2MCU_DATA2_DONE_SET                         BTH_CMU_SENS2MCU_DATA2_DONE_SET
#define CMU_SENS2MCU_DATA3_DONE_SET                         BTH_CMU_SENS2MCU_DATA3_DONE_SET
#define CMU_MCU2SENS_DATA_IND_SET                           BTH_CMU_MCU2SENS_DATA_IND_SET
#define CMU_MCU2SENS_DATA1_IND_SET                          BTH_CMU_MCU2SENS_DATA1_IND_SET
#define CMU_MCU2SENS_DATA2_IND_SET                          BTH_CMU_MCU2SENS_DATA2_IND_SET
#define CMU_MCU2SENS_DATA3_IND_SET                          BTH_CMU_MCU2SENS_DATA3_IND_SET
#define CMU_SENS2MCU_DATA_MSK_SET                           BTH_CMU_SENS2MCU_DATA_MSK_SET
#define CMU_SENS2MCU_DATA1_MSK_SET                          BTH_CMU_SENS2MCU_DATA1_MSK_SET
#define CMU_SENS2MCU_DATA2_MSK_SET                          BTH_CMU_SENS2MCU_DATA2_MSK_SET
#define CMU_SENS2MCU_DATA3_MSK_SET                          BTH_CMU_SENS2MCU_DATA3_MSK_SET
#define CMU_MCU2SENS_DATA_MSK_SET                           BTH_CMU_MCU2SENS_DATA_MSK_SET
#define CMU_MCU2SENS_DATA1_MSK_SET                          BTH_CMU_MCU2SENS_DATA1_MSK_SET
#define CMU_MCU2SENS_DATA2_MSK_SET                          BTH_CMU_MCU2SENS_DATA2_MSK_SET
#define CMU_MCU2SENS_DATA3_MSK_SET                          BTH_CMU_MCU2SENS_DATA3_MSK_SET
#define CMU_SENS2MCU_DATA_INTR                              BTH_CMU_SENS2MCU_DATA_INTR
#define CMU_SENS2MCU_DATA1_INTR                             BTH_CMU_SENS2MCU_DATA1_INTR
#define CMU_SENS2MCU_DATA2_INTR                             BTH_CMU_SENS2MCU_DATA2_INTR
#define CMU_SENS2MCU_DATA3_INTR                             BTH_CMU_SENS2MCU_DATA3_INTR
#define CMU_MCU2SENS_DATA_INTR                              BTH_CMU_MCU2SENS_DATA_INTR
#define CMU_MCU2SENS_DATA1_INTR                             BTH_CMU_MCU2SENS_DATA1_INTR
#define CMU_MCU2SENS_DATA2_INTR                             BTH_CMU_MCU2SENS_DATA2_INTR
#define CMU_MCU2SENS_DATA3_INTR                             BTH_CMU_MCU2SENS_DATA3_INTR
#define CMU_SENS2MCU_DATA_INTR_MSK                          BTH_CMU_SENS2MCU_DATA_INTR_MSK
#define CMU_SENS2MCU_DATA1_INTR_MSK                         BTH_CMU_SENS2MCU_DATA1_INTR_MSK
#define CMU_SENS2MCU_DATA2_INTR_MSK                         BTH_CMU_SENS2MCU_DATA2_INTR_MSK
#define CMU_SENS2MCU_DATA3_INTR_MSK                         BTH_CMU_SENS2MCU_DATA3_INTR_MSK
#define CMU_MCU2SENS_DATA_INTR_MSK                          BTH_CMU_MCU2SENS_DATA_INTR_MSK
#define CMU_MCU2SENS_DATA1_INTR_MSK                         BTH_CMU_MCU2SENS_DATA1_INTR_MSK
#define CMU_MCU2SENS_DATA2_INTR_MSK                         BTH_CMU_MCU2SENS_DATA2_INTR_MSK
#define CMU_MCU2SENS_DATA3_INTR_MSK                         BTH_CMU_MCU2SENS_DATA3_INTR_MSK

#define CMU_SENS2MCU_DATA_DONE_CLR                          BTH_CMU_SENS2MCU_DATA_DONE_CLR
#define CMU_SENS2MCU_DATA1_DONE_CLR                         BTH_CMU_SENS2MCU_DATA1_DONE_CLR
#define CMU_SENS2MCU_DATA2_DONE_CLR                         BTH_CMU_SENS2MCU_DATA2_DONE_CLR
#define CMU_SENS2MCU_DATA3_DONE_CLR                         BTH_CMU_SENS2MCU_DATA3_DONE_CLR
#define CMU_MCU2SENS_DATA_IND_CLR                           BTH_CMU_MCU2SENS_DATA_IND_CLR
#define CMU_MCU2SENS_DATA1_IND_CLR                          BTH_CMU_MCU2SENS_DATA1_IND_CLR
#define CMU_MCU2SENS_DATA2_IND_CLR                          BTH_CMU_MCU2SENS_DATA2_IND_CLR
#define CMU_MCU2SENS_DATA3_IND_CLR                          BTH_CMU_MCU2SENS_DATA3_IND_CLR
#define CMU_SENS2MCU_DATA_MSK_CLR                           BTH_CMU_SENS2MCU_DATA_MSK_CLR
#define CMU_SENS2MCU_DATA1_MSK_CLR                          BTH_CMU_SENS2MCU_DATA1_MSK_CLR
#define CMU_SENS2MCU_DATA2_MSK_CLR                          BTH_CMU_SENS2MCU_DATA2_MSK_CLR
#define CMU_SENS2MCU_DATA3_MSK_CLR                          BTH_CMU_SENS2MCU_DATA3_MSK_CLR
#define CMU_MCU2SENS_DATA_MSK_CLR                           BTH_CMU_MCU2SENS_DATA_MSK_CLR
#define CMU_MCU2SENS_DATA1_MSK_CLR                          BTH_CMU_MCU2SENS_DATA1_MSK_CLR
#define CMU_MCU2SENS_DATA2_MSK_CLR                          BTH_CMU_MCU2SENS_DATA2_MSK_CLR
#define CMU_MCU2SENS_DATA3_MSK_CLR                          BTH_CMU_MCU2SENS_DATA3_MSK_CLR
#endif

// reg_130
#define BTH_CMU_TPORT_INTR_MSK                              (1 << 0)
#define BTH_CMU_TPORT_INTR_EN                               (1 << 1)
#define BTH_CMU_TPORT_INTR_CLR                              (1 << 2)
#define BTH_CMU_TPORT_ADDR_BYPASS                           (1 << 3)
#define BTH_CMU_TPORT_FILTER_SEL                            (1 << 4)
#define BTH_CMU_TPORT_EN                                    (1 << 5)
#define BTH_CMU_TPORT_INTR_STATUS                           (1 << 6)

// reg_134
#define BTH_CMU_SYS2BTH_DATA_DONE_SET                       (1 << 0)
#define BTH_CMU_SYS2BTH_DATA1_DONE_SET                      (1 << 1)
#define BTH_CMU_BTH2SYS_DATA_IND_SET                        (1 << 2)
#define BTH_CMU_BTH2SYS_DATA1_IND_SET                       (1 << 3)
#define BTH_CMU_SYS2BTH_DATA_MSK_SET                        (1 << 4)
#define BTH_CMU_SYS2BTH_DATA1_MSK_SET                       (1 << 5)
#define BTH_CMU_BTH2SYS_DATA_MSK_SET                        (1 << 6)
#define BTH_CMU_BTH2SYS_DATA1_MSK_SET                       (1 << 7)
#define BTH_CMU_SYS_ALLIRQ_MASK_SET                         (1 << 8)
#define BTH_CMU_SYS2BTH_DATA_INTR                           (1 << 9)
#define BTH_CMU_SYS2BTH_DATA1_INTR                          (1 << 10)
#define BTH_CMU_BTH2SYS_DATA_INTR                           (1 << 11)
#define BTH_CMU_BTH2SYS_DATA1_INTR                          (1 << 12)
#define BTH_CMU_SYS2BTH_DATA_INTR_MSK                       (1 << 13)
#define BTH_CMU_SYS2BTH_DATA1_INTR_MSK                      (1 << 14)
#define BTH_CMU_BTH2SYS_DATA_INTR_MSK                       (1 << 15)
#define BTH_CMU_BTH2SYS_DATA1_INTR_MSK                      (1 << 16)

// reg_138
#define BTH_CMU_SYS2BTH_DATA_DONE_CLR                       (1 << 0)
#define BTH_CMU_SYS2BTH_DATA1_DONE_CLR                      (1 << 1)
#define BTH_CMU_BTH2SYS_DATA_IND_CLR                        (1 << 2)
#define BTH_CMU_BTH2SYS_DATA1_IND_CLR                       (1 << 3)
#define BTH_CMU_SYS2BTH_DATA_MSK_CLR                        (1 << 4)
#define BTH_CMU_SYS2BTH_DATA1_MSK_CLR                       (1 << 5)
#define BTH_CMU_BTH2SYS_DATA_MSK_CLR                        (1 << 6)
#define BTH_CMU_BTH2SYS_DATA1_MSK_CLR                       (1 << 7)
#define BTH_CMU_SYS_ALLIRQ_MASK_CLR                         (1 << 8)

// reg_13c
#define BTH_CMU_BTH2SYS_INTISR_MASK0(n)                     (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_BTH2SYS_INTISR_MASK0_MASK                   (0xFFFFFFFF << 0)
#define BTH_CMU_BTH2SYS_INTISR_MASK0_SHIFT                  (0)

// reg_140
#define BTH_CMU_BTH2SYS_INTISR_MASK1(n)                     (((n) & 0xFFFFFFFF) << 0)
#define BTH_CMU_BTH2SYS_INTISR_MASK1_MASK                   (0xFFFFFFFF << 0)
#define BTH_CMU_BTH2SYS_INTISR_MASK1_SHIFT                  (0)

// reg_144
#define BTH_CMU_BT_TOG_TRIG0_INTR                           (1 << 0)
#define BTH_CMU_BT_TOG_TRIG1_INTR                           (1 << 1)
#define BTH_CMU_BT_TOG_TRIG2_INTR                           (1 << 2)
#define BTH_CMU_BT_TOG_TRIG3_INTR                           (1 << 3)
#define BTH_CMU_BT_TOG_TRIG0_INTR_MSK                       (1 << 4)
#define BTH_CMU_BT_TOG_TRIG1_INTR_MSK                       (1 << 5)
#define BTH_CMU_BT_TOG_TRIG2_INTR_MSK                       (1 << 6)
#define BTH_CMU_BT_TOG_TRIG3_INTR_MSK                       (1 << 7)

// reg_148
#define BTH_CMU_BT_TOG_TRIG0_INTR_CLR                       (1 << 0)
#define BTH_CMU_BT_TOG_TRIG1_INTR_CLR                       (1 << 1)
#define BTH_CMU_BT_TOG_TRIG2_INTR_CLR                       (1 << 2)
#define BTH_CMU_BT_TOG_TRIG3_INTR_CLR                       (1 << 3)

// reg_14c
#define BTH_CMU_VTOR_CORE_BTH_CORE1(n)                      (((n) & 0x1FFFFFF) << 7)
#define BTH_CMU_VTOR_CORE_BTH_CORE1_MASK                    (0x1FFFFFF << 7)
#define BTH_CMU_VTOR_CORE_BTH_CORE1_SHIFT                   (7)

#ifndef CMU_VTOR_CORE1_MASK
#define CMU_VTOR_CORE1_MASK                                 BTH_CMU_VTOR_CORE_BTH_CORE1_MASK
#define CMU_VTOR_CORE1_SHIFT                                BTH_CMU_VTOR_CORE_BTH_CORE1_SHIFT
#endif

// reg_150
#define BTH_CMU_ROM_RTSEL(n)                                (((n) & 0x3) << 0)
#define BTH_CMU_ROM_RTSEL_MASK                              (0x3 << 0)
#define BTH_CMU_ROM_RTSEL_SHIFT                             (0)
#define BTH_CMU_ROM_PTSEL(n)                                (((n) & 0x3) << 2)
#define BTH_CMU_ROM_PTSEL_MASK                              (0x3 << 2)
#define BTH_CMU_ROM_PTSEL_SHIFT                             (2)
#define BTH_CMU_ROM_TRB(n)                                  (((n) & 0x3) << 4)
#define BTH_CMU_ROM_TRB_MASK                                (0x3 << 4)
#define BTH_CMU_ROM_TRB_SHIFT                               (4)
#define BTH_CMU_ROM_PGEN                                    (1 << 6)
#define BTH_CMU_ROM_TM                                      (1 << 7)
#define BTH_CMU_ROM_BYPASS_CG                               (1 << 8)
#define BTH_CMU_CACHEC_REMAP                                (1 << 9)

// reg_154
#define BTH_CMU_WAKEUP_IRQ_MASK2(n)                         (((n) & 0xFFFF) << 0)
#define BTH_CMU_WAKEUP_IRQ_MASK2_MASK                       (0xFFFF << 0)
#define BTH_CMU_WAKEUP_IRQ_MASK2_SHIFT                      (0)
#define BTH_CMU_MCU2BT_INTISR_MASK2(n)                      (((n) & 0xFFFF) << 16)
#define BTH_CMU_MCU2BT_INTISR_MASK2_MASK                    (0xFFFF << 16)
#define BTH_CMU_MCU2BT_INTISR_MASK2_SHIFT                   (16)

// reg_158
#define BTH_CMU_MCU2SENS_INTISR_MASK2(n)                    (((n) & 0xFFFF) << 0)
#define BTH_CMU_MCU2SENS_INTISR_MASK2_MASK                  (0xFFFF << 0)
#define BTH_CMU_MCU2SENS_INTISR_MASK2_SHIFT                 (0)
#define BTH_CMU_BTH2SYS_INTISR_MASK2(n)                     (((n) & 0xFFFF) << 16)
#define BTH_CMU_BTH2SYS_INTISR_MASK2_MASK                   (0xFFFF << 16)
#define BTH_CMU_BTH2SYS_INTISR_MASK2_SHIFT                  (16)

// reg_160
#define BTH_CMU_MCU2CP_DATA_DONE_SET                        (1 << 0)
#define BTH_CMU_MCU2CP_DATA1_DONE_SET                       (1 << 1)
#define BTH_CMU_MCU2CP_DATA2_DONE_SET                       (1 << 2)
#define BTH_CMU_MCU2CP_DATA3_DONE_SET                       (1 << 3)
#define BTH_CMU_CP2MCU_DATA_IND_SET                         (1 << 4)
#define BTH_CMU_CP2MCU_DATA1_IND_SET                        (1 << 5)
#define BTH_CMU_CP2MCU_DATA2_IND_SET                        (1 << 6)
#define BTH_CMU_CP2MCU_DATA3_IND_SET                        (1 << 7)

// reg_164
#define BTH_CMU_MCU2CP_DATA_DONE_CLR                        (1 << 0)
#define BTH_CMU_MCU2CP_DATA1_DONE_CLR                       (1 << 1)
#define BTH_CMU_MCU2CP_DATA2_DONE_CLR                       (1 << 2)
#define BTH_CMU_MCU2CP_DATA3_DONE_CLR                       (1 << 3)
#define BTH_CMU_CP2MCU_DATA_IND_CLR                         (1 << 4)
#define BTH_CMU_CP2MCU_DATA1_IND_CLR                        (1 << 5)
#define BTH_CMU_CP2MCU_DATA2_IND_CLR                        (1 << 6)
#define BTH_CMU_CP2MCU_DATA3_IND_CLR                        (1 << 7)

// reg_168
#define BTH_CMU_CP2MCU_DATA_DONE_SET                        (1 << 0)
#define BTH_CMU_CP2MCU_DATA1_DONE_SET                       (1 << 1)
#define BTH_CMU_CP2MCU_DATA2_DONE_SET                       (1 << 2)
#define BTH_CMU_CP2MCU_DATA3_DONE_SET                       (1 << 3)
#define BTH_CMU_MCU2CP_DATA_IND_SET                         (1 << 4)
#define BTH_CMU_MCU2CP_DATA1_IND_SET                        (1 << 5)
#define BTH_CMU_MCU2CP_DATA2_IND_SET                        (1 << 6)
#define BTH_CMU_MCU2CP_DATA3_IND_SET                        (1 << 7)

// reg_16c
#define BTH_CMU_CP2MCU_DATA_DONE_CLR                        (1 << 0)
#define BTH_CMU_CP2MCU_DATA1_DONE_CLR                       (1 << 1)
#define BTH_CMU_CP2MCU_DATA2_DONE_CLR                       (1 << 2)
#define BTH_CMU_CP2MCU_DATA3_DONE_CLR                       (1 << 3)
#define BTH_CMU_MCU2CP_DATA_IND_CLR                         (1 << 4)
#define BTH_CMU_MCU2CP_DATA1_IND_CLR                        (1 << 5)
#define BTH_CMU_MCU2CP_DATA2_IND_CLR                        (1 << 6)
#define BTH_CMU_MCU2CP_DATA3_IND_CLR                        (1 << 7)

#ifndef CMU_MCU2CP_DATA_DONE_SET
#define CMU_MCU2CP_DATA_DONE_SET                            BTH_CMU_MCU2CP_DATA_DONE_SET
#define CMU_MCU2CP_DATA1_DONE_SET                           BTH_CMU_MCU2CP_DATA1_DONE_SET
#define CMU_MCU2CP_DATA2_DONE_SET                           BTH_CMU_MCU2CP_DATA2_DONE_SET
#define CMU_MCU2CP_DATA3_DONE_SET                           BTH_CMU_MCU2CP_DATA3_DONE_SET
#define CMU_CP2MCU_DATA_IND_SET                             BTH_CMU_CP2MCU_DATA_IND_SET
#define CMU_CP2MCU_DATA1_IND_SET                            BTH_CMU_CP2MCU_DATA1_IND_SET
#define CMU_CP2MCU_DATA2_IND_SET                            BTH_CMU_CP2MCU_DATA2_IND_SET
#define CMU_CP2MCU_DATA3_IND_SET                            BTH_CMU_CP2MCU_DATA3_IND_SET
#define CMU_MCU2CP_DATA_DONE_CLR                            BTH_CMU_MCU2CP_DATA_DONE_CLR
#define CMU_MCU2CP_DATA1_DONE_CLR                           BTH_CMU_MCU2CP_DATA1_DONE_CLR
#define CMU_MCU2CP_DATA2_DONE_CLR                           BTH_CMU_MCU2CP_DATA2_DONE_CLR
#define CMU_MCU2CP_DATA3_DONE_CLR                           BTH_CMU_MCU2CP_DATA3_DONE_CLR
#define CMU_CP2MCU_DATA_IND_CLR                             BTH_CMU_CP2MCU_DATA_IND_CLR
#define CMU_CP2MCU_DATA1_IND_CLR                            BTH_CMU_CP2MCU_DATA1_IND_CLR
#define CMU_CP2MCU_DATA2_IND_CLR                            BTH_CMU_CP2MCU_DATA2_IND_CLR
#define CMU_CP2MCU_DATA3_IND_CLR                            BTH_CMU_CP2MCU_DATA2_IND_CLR
#define CMU_CP2MCU_DATA_DONE_SET                            BTH_CMU_CP2MCU_DATA_DONE_SET
#define CMU_CP2MCU_DATA1_DONE_SET                           BTH_CMU_CP2MCU_DATA1_DONE_SET
#define CMU_CP2MCU_DATA2_DONE_SET                           BTH_CMU_CP2MCU_DATA2_DONE_SET
#define CMU_CP2MCU_DATA3_DONE_SET                           BTH_CMU_CP2MCU_DATA3_DONE_SET
#define CMU_MCU2CP_DATA_IND_SET                             BTH_CMU_MCU2CP_DATA_IND_SET
#define CMU_MCU2CP_DATA1_IND_SET                            BTH_CMU_MCU2CP_DATA1_IND_SET
#define CMU_MCU2CP_DATA2_IND_SET                            BTH_CMU_MCU2CP_DATA2_IND_SET
#define CMU_MCU2CP_DATA3_IND_SET                            BTH_CMU_MCU2CP_DATA3_IND_SET
#define CMU_CP2MCU_DATA_DONE_CLR                            BTH_CMU_CP2MCU_DATA_DONE_CLR
#define CMU_CP2MCU_DATA1_DONE_CLR                           BTH_CMU_CP2MCU_DATA1_DONE_CLR
#define CMU_CP2MCU_DATA2_DONE_CLR                           BTH_CMU_CP2MCU_DATA2_DONE_CLR
#define CMU_CP2MCU_DATA3_DONE_CLR                           BTH_CMU_CP2MCU_DATA3_DONE_CLR
#define CMU_MCU2CP_DATA_IND_CLR                             BTH_CMU_MCU2CP_DATA_IND_CLR
#define CMU_MCU2CP_DATA1_IND_CLR                            BTH_CMU_MCU2CP_DATA1_IND_CLR
#define CMU_MCU2CP_DATA2_IND_CLR                            BTH_CMU_MCU2CP_DATA2_IND_CLR
#define CMU_MCU2CP_DATA3_IND_CLR                            BTH_CMU_MCU2CP_DATA3_IND_CLR
#endif

// reg_170
#define BTH_CMU_CFG_DIV_SPI0(n)                             (((n) & 0x3) << 0)
#define BTH_CMU_CFG_DIV_SPI0_MASK                           (0x3 << 0)
#define BTH_CMU_CFG_DIV_SPI0_SHIFT                          (0)
#define BTH_CMU_SEL_OSCX2_SPI0                              (1 << 2)
#define BTH_CMU_SEL_PLL_SPI0                                (1 << 3)
#define BTH_CMU_EN_PLL_SPI0                                 (1 << 4)
#define BTH_CMU_CFG_DIV_SPI1(n)                             (((n) & 0x3) << 5)
#define BTH_CMU_CFG_DIV_SPI1_MASK                           (0x3 << 5)
#define BTH_CMU_CFG_DIV_SPI1_SHIFT                          (5)
#define BTH_CMU_SEL_OSCX2_SPI1                              (1 << 7)
#define BTH_CMU_SEL_PLL_SPI1                                (1 << 8)
#define BTH_CMU_EN_PLL_SPI1                                 (1 << 9)

// reg_174
#define BTH_CMU_CFG_DIV_I2C0(n)                             (((n) & 0x3) << 0)
#define BTH_CMU_CFG_DIV_I2C0_MASK                           (0x3 << 0)
#define BTH_CMU_CFG_DIV_I2C0_SHIFT                          (0)
#define BTH_CMU_SEL_OSCX2_I2C0                              (1 << 2)
#define BTH_CMU_SEL_PLL_I2C0                                (1 << 3)
#define BTH_CMU_EN_PLL_I2C0                                 (1 << 4)
#define BTH_CMU_CFG_DIV_I2C1(n)                             (((n) & 0x3) << 5)
#define BTH_CMU_CFG_DIV_I2C1_MASK                           (0x3 << 5)
#define BTH_CMU_CFG_DIV_I2C1_SHIFT                          (5)
#define BTH_CMU_SEL_OSCX2_I2C1                              (1 << 7)
#define BTH_CMU_SEL_PLL_I2C1                                (1 << 8)
#define BTH_CMU_EN_PLL_I2C1                                 (1 << 9)
#define BTH_CMU_SEL_OSC_I2C0                                (1 << 10)
#define BTH_CMU_SEL_OSC_I2C1                                (1 << 11)

// reg_178
#define BTH_CMU_PVT_RO_EN                                   (1 << 0)
#define BTH_CMU_PVT_COUNT_START_EN                          (1 << 1)
#define BTH_CMU_PVT_OSC_CLK_EN                              (1 << 2)
#define BTH_CMU_PVT_RSTN                                    (1 << 3)
#define BTH_CMU_PVT_REG_TIMESET(n)                          (((n) & 0x3FF) << 4)
#define BTH_CMU_PVT_REG_TIMESET_MASK                        (0x3FF << 4)
#define BTH_CMU_PVT_REG_TIMESET_SHIFT                       (4)

// reg_17c
#define BTH_CMU_PVT_COUNT_VALUE(n)                          (((n) & 0x3FFFF) << 0)
#define BTH_CMU_PVT_COUNT_VALUE_MASK                        (0x3FFFF << 0)
#define BTH_CMU_PVT_COUNT_VALUE_SHIFT                       (0)
#define BTH_CMU_PVT_COUNT_DONE                              (1 << 18)

// MCU System AHB Clocks:
#define BTH_HCLK_CORE0                                      (1 << 0)
#define BTH_HRST_CORE0                                      (1 << 0)
#define BTH_HCLK_CACHE0                                     (1 << 1)
#define BTH_HRST_CACHE0                                     (1 << 1)
#define BTH_HCLK_FLASH0                                     (1 << 2)
#define BTH_HRST_FLASH0                                     (1 << 2)
#define BTH_HCLK_FLASH1                                     (1 << 3)
#define BTH_HRST_FLASH1                                     (1 << 3)
#define BTH_HCLK_GDMA                                       (1 << 4)
#define BTH_HRST_GDMA                                       (1 << 4)
#define BTH_HCLK_AHB0                                       (1 << 5)
#define BTH_HRST_AHB0                                       (1 << 5)
#define BTH_HCLK_AHB1                                       (1 << 6)
#define BTH_HRST_AHB1                                       (1 << 6)
#define BTH_HCLK_AH2H_BT                                    (1 << 7)
#define BTH_HRST_AH2H_BT                                    (1 << 7)
#define BTH_HCLK_AH2H_SENS                                  (1 << 8)
#define BTH_HRST_AH2H_SENS                                  (1 << 8)
#define BTH_HCLK_BT_TPORT                                   (1 << 9)
#define BTH_HRST_BT_TPORT                                   (1 << 9)
#define BTH_HCLK_BT_DUMP                                    (1 << 10)
#define BTH_HRST_BT_DUMP                                    (1 << 10)
#define BTH_HCLK_CODEC                                      (1 << 11)
#define BTH_HRST_CODEC                                      (1 << 11)
#define BTH_HCLK_RAM0                                       (1 << 12)
#define BTH_HRST_RAM0                                       (1 << 12)
#define BTH_HCLK_RAM1                                       (1 << 13)
#define BTH_HRST_RAM1                                       (1 << 13)
#define BTH_HCLK_RAM2                                       (1 << 14)
#define BTH_HRST_RAM2                                       (1 << 14)
#define BTH_HCLK_RAM3                                       (1 << 15)
#define BTH_HRST_RAM3                                       (1 << 15)
#define BTH_HCLK_RAM4                                       (1 << 16)
#define BTH_HRST_RAM4                                       (1 << 16)
#define BTH_HCLK_RAM5                                       (1 << 17)
#define BTH_HRST_RAM5                                       (1 << 17)
#define BTH_HCLK_RAM6                                       (1 << 18)
#define BTH_HRST_RAM6                                       (1 << 18)
#define BTH_HCLK_AH2H_SYS                                   (1 << 19)
#define BTH_HRST_AH2H_SYS                                   (1 << 19)
#define BTH_HCLK_CORE1                                      (1 << 20)
#define BTH_HRST_CORE1                                      (1 << 20)
#define BTH_HCLK_CP0                                        (1 << 21)
#define BTH_HRST_CP0                                        (1 << 21)
#define BTH_HCLK_CP1                                        (1 << 22)
#define BTH_HRST_CP1                                        (1 << 22)
#define BTH_HCLK_DUMPCPU                                    (1 << 23)
#define BTH_HRST_DUMPCPU                                    (1 << 23)

// MCU System APB Clocks:
#define BTH_PCLK_CMU                                        (1 << 0)
#define BTH_PRST_CMU                                        (1 << 0)
#define BTH_PCLK_WDT                                        (1 << 1)
#define BTH_PRST_WDT                                        (1 << 1)
#define BTH_PCLK_TIMER0                                     (1 << 2)
#define BTH_PRST_TIMER0                                     (1 << 2)
#define BTH_PCLK_TIMER1                                     (1 << 3)
#define BTH_PRST_TIMER1                                     (1 << 3)
#define BTH_PCLK_SPI_ITN                                    (1 << 4)
#define BTH_PRST_SPI_ITN                                    (1 << 4)
#define BTH_PCLK_UART0                                      (1 << 5)
#define BTH_PRST_UART0                                      (1 << 5)
#define BTH_PCLK_UART1                                      (1 << 6)
#define BTH_PRST_UART1                                      (1 << 6)
#define BTH_PCLK_PCM                                        (1 << 7)
#define BTH_PRST_PCM                                        (1 << 7)
#define BTH_PCLK_I2C0                                       (1 << 8)
#define BTH_PRST_I2C0                                       (1 << 8)
#define BTH_PCLK_I2S0                                       (1 << 9)
#define BTH_PRST_I2S0                                       (1 << 9)
#define BTH_PCLK_I2C1                                       (1 << 10)
#define BTH_PRST_I2C1                                       (1 << 10)
#define BTH_PCLK_SPI0                                       (1 << 11)
#define BTH_PRST_SPI0                                       (1 << 11)
#define BTH_PCLK_SPI1                                       (1 << 12)
#define BTH_PRST_SPI1                                       (1 << 12)

// MCU System Other Clocks:
#define BTH_OCLK_SLEEP                                      (1 << 0)
#define BTH_ORST_SLEEP                                      (1 << 0)
#define BTH_OCLK_FLASH1                                     (1 << 1)
#define BTH_ORST_FLASH1                                     (1 << 1)
#define BTH_OCLK_WDT                                        (1 << 2)
#define BTH_ORST_WDT                                        (1 << 2)
#define BTH_OCLK_TIMER0                                     (1 << 3)
#define BTH_ORST_TIMER0                                     (1 << 3)
#define BTH_OCLK_TIMER1                                     (1 << 4)
#define BTH_ORST_TIMER1                                     (1 << 4)
#define BTH_OCLK_SPI_ITN                                    (1 << 5)
#define BTH_ORST_SPI_ITN                                    (1 << 5)
#define BTH_OCLK_UART0                                      (1 << 6)
#define BTH_ORST_UART0                                      (1 << 6)
#define BTH_OCLK_UART1                                      (1 << 7)
#define BTH_ORST_UART1                                      (1 << 7)
#define BTH_OCLK_PCM                                        (1 << 8)
#define BTH_ORST_PCM                                        (1 << 8)
#define BTH_OCLK_I2C0                                       (1 << 9)
#define BTH_ORST_I2C0                                       (1 << 9)
#define BTH_OCLK_I2S0                                       (1 << 10)
#define BTH_ORST_I2S0                                       (1 << 10)
#define BTH_OCLK_SPI0                                       (1 << 11)
#define BTH_ORST_SPI0                                       (1 << 11)
#define BTH_OCLK_SPI1                                       (1 << 12)
#define BTH_ORST_SPI1                                       (1 << 12)
#define BTH_OCLK_I2C1                                       (1 << 13)
#define BTH_ORST_I2C1                                       (1 << 13)

#endif
