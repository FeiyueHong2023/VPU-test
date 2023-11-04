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
#ifndef __REG_AONCMU_BEST1603_H__
#define __REG_AONCMU_BEST1603_H__

#include "plat_types.h"

struct AONCMU_T {
    __I  uint32_t CHIP_ID;          // 0x00
    __IO uint32_t TOP_CLK_ENABLE;   // 0x04
    __IO uint32_t TOP_CLK_DISABLE;  // 0x08
    __IO uint32_t RESET_PULSE;      // 0x0C
    __IO uint32_t RESET_SET;        // 0x10
    __IO uint32_t RESET_CLR;        // 0x14
    __IO uint32_t CLK_SELECT;       // 0x18
    __IO uint32_t CLK_OUT;          // 0x1C
    __IO uint32_t WRITE_UNLOCK;     // 0x20
    __IO uint32_t RAM5_SLP0;        // 0x24
    __IO uint32_t RAM5_SLP1;        // 0x28
    __IO uint32_t RAM6_SLP0;        // 0x2C
    __IO uint32_t RAM6_SLP1;        // 0x30
    __IO uint32_t RAM_POFF0;        // 0x34
    __IO uint32_t BOOTMODE;         // 0x38
    __IO uint32_t RESERVED_03C;     // 0x3C
    __IO uint32_t MOD_CLK_ENABLE;   // 0x40
    __IO uint32_t MOD_CLK_DISABLE;  // 0x44
    __IO uint32_t MOD_CLK_MODE;     // 0x48
    __IO uint32_t AON_CLK;          // 0x4C
    __IO uint32_t FREE_TIMER;       // 0x50
    __IO uint32_t RAM_POFF1;        // 0x54
    __IO uint32_t RAM2_POFF0;       // 0x58
    __IO uint32_t RAM_CFG;          // 0x5C
    __IO uint32_t RAM2_POFF1;       // 0x60
    __IO uint32_t MCU_PWR_MODE;     // 0x64
    __IO uint32_t RAM_LP_TIMER;     // 0x68
    __IO uint32_t TIMER_OSC_BT;     // 0x6C
    __IO uint32_t RAM3_POFF0;       // 0x70
    __IO uint32_t RAM3_POFF1;       // 0x74
    __IO uint32_t RAM_AUTO_EN_MODES; // 0x78
    __IO uint32_t SE_WLOCK;         // 0x7C
    __IO uint32_t SE_RLOCK;         // 0x80
    __IO uint32_t PD_STAB_TIMER;    // 0x84
    __IO uint32_t RAM_SLP0;         // 0x88
    __IO uint32_t RAM_SLP1;         // 0x8C
    __IO uint32_t RAM2_SLP0;        // 0x90
    __IO uint32_t RAM2_SLP1;        // 0x94
    __IO uint32_t RAM_CFG_SEL;      // 0x98
    __IO uint32_t RAM2_AUTO_EN_MODES; // 0x9C
    __IO uint32_t TOP_CLK1_ENABLE;  // 0xA0
    __IO uint32_t TOP_CLK1_DISABLE; // 0xA4
    __IO uint32_t RAM_BC;           // 0xA8
    __IO uint32_t SENS_VTOR;        // 0xAC
    __IO uint32_t RAM3_SLP0;        // 0xB0
    __IO uint32_t BT_RAM_CFG;       // 0xB4
    __IO uint32_t SENS_RAM_CFG;     // 0xB8
    __IO uint32_t CODEC_RAM_CFG;    // 0xBC
    __IO uint32_t RAM_RTSEL;        // 0xC0
    __IO uint32_t GBL_RESET_PULSE;  // 0xC4
    __IO uint32_t GBL_RESET_SET;    // 0xC8
    __IO uint32_t GBL_RESET_CLR;    // 0xCC
    __IO uint32_t FLASH_IODRV;      // 0xD0
    __IO uint32_t BTH_RAM_CFG;      // 0xD4
    __IO uint32_t BTH_VTOR;         // 0xD8
    __IO uint32_t SYS_RAM_SEL0;     // 0xDC
    __IO uint32_t SYS_RAM_SEL1;     // 0xE0
    __IO uint32_t RAM3_SLP1;        // 0xE4
    __IO uint32_t RAM4_SLP0;        // 0xE8
    __IO uint32_t RAM4_SLP1;        // 0xEC
    __IO uint32_t DEBUG_RESERVED;   // 0xF0
    __IO uint32_t SW_RESERVED_0F4;  // 0xF4
    __IO uint32_t SW_INFO;          // 0xF8
    __IO uint32_t CHIP_FEATURE;     // 0xFC
    __IO uint32_t MEMSC[16];        // 0x100
    __I  uint32_t MEMSC_STATUS;     // 0x140
    __IO uint32_t RAM4_POFF0;       // 0x144
    __IO uint32_t RAM4_POFF1;       // 0x148
    __IO uint32_t SW_RESERVED_14C;  // 0x14C
    __IO uint32_t PSRAM_PHY_CTRL;   // 0x150
    __IO uint32_t FLASH1_CFG;       // 0x154
    __IO uint32_t CEN_MSK;          // 0x158
    __IO uint32_t M55_COREPREQ;     // 0x15C
    __IO uint32_t DSP_RUNSTALL;     // 0x160
    __IO uint32_t DSP_VTOR;         // 0x164
    __IO uint32_t BTC_VTOR;         // 0x168
    __IO uint32_t RAM7_SLP0;        // 0x16C
    __IO uint32_t RAM7_SLP1;        // 0x170
    __IO uint32_t LAST_PC_INFO;     // 0x174
    __IO uint32_t EFUSE_CFG;        // 0x178
    __IO uint32_t RAM8_SLP0;        // 0x17C
    __IO uint32_t RAM8_SLP1;        // 0x180
    __IO uint32_t PVT_CFG;          // 0x184
    __IO uint32_t PVT_COUNT;        // 0x188
    __IO uint32_t SYS_LAST_PC_INFO; // 0x18C
};

// reg_000
#define AON_CMU_CHIP_ID(n)                                  (((n) & 0xFFFF) << 0)
#define AON_CMU_CHIP_ID_MASK                                (0xFFFF << 0)
#define AON_CMU_CHIP_ID_SHIFT                               (0)
#define AON_CMU_REVISION_ID(n)                              (((n) & 0xFFFF) << 16)
#define AON_CMU_REVISION_ID_MASK                            (0xFFFF << 16)
#define AON_CMU_REVISION_ID_SHIFT                           (16)

// reg_004
#define AON_CMU_EN_CLK_TOP_PLLBB_ENABLE                     (1 << 0)
#define AON_CMU_EN_CLK_TOP_OSCX2_ENABLE                     (1 << 1)
#define AON_CMU_EN_CLK_TOP_OSC_ENABLE                       (1 << 2)
#define AON_CMU_EN_CLK_TOP_JTAG_ENABLE                      (1 << 3)
#define AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE                    (1 << 4)
#define AON_CMU_EN_CLK_32K_CODEC_ENABLE                     (1 << 5)
#define AON_CMU_EN_CLK_32K_MCU_ENABLE                       (1 << 6)
#define AON_CMU_EN_CLK_32K_BTH_ENABLE                       (1 << 7)
#define AON_CMU_EN_CLK_32K_BT_ENABLE                        (1 << 8)
#define AON_CMU_EN_CLK_DCDC0_ENABLE                         (1 << 9)
#define AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE                    (1 << 10)
#define AON_CMU_PU_PLLDSI_ENABLE                            (1 << 11)
#define AON_CMU_EN_X2_DIG_ENABLE                            (1 << 12)
#define AON_CMU_EN_X4_DIG_ENABLE                            (1 << 13)
#define AON_CMU_PU_PLLBB_ENABLE                             (1 << 14)
#define AON_CMU_PU_PLLUSB_ENABLE                            (1 << 15)
#define AON_CMU_PU_RC_ENABLE                                (1 << 16)
#define AON_CMU_PU_OSC_ENABLE                               (1 << 17)
#define AON_CMU_EN_CLK_TOP_OSCX4_ENABLE                     (1 << 18)
#define AON_CMU_EN_BT_CLK_SYS_ENABLE                        (1 << 19)
#define AON_CMU_SEL_CLK_OSC_MCU_ENABLE                      (1 << 20)
#define AON_CMU_SEL_CLK_OSC_SEN_ENABLE                      (1 << 21)
#define AON_CMU_SEL_CLK_OSC_CDC_ENABLE                      (1 << 22)
#define AON_CMU_SEL_CLK_OSC_BTH_ENABLE                      (1 << 23)
#define AON_CMU_SEL_CLK_OSC_BTC_ENABLE                      (1 << 24)
#define AON_CMU_SEL_CLK_OSC_PRE_ENABLE                      (1 << 25)

// reg_008
#define AON_CMU_EN_CLK_TOP_PLLBB_DISABLE                    (1 << 0)
#define AON_CMU_EN_CLK_TOP_OSCX2_DISABLE                    (1 << 1)
#define AON_CMU_EN_CLK_TOP_OSC_DISABLE                      (1 << 2)
#define AON_CMU_EN_CLK_TOP_JTAG_DISABLE                     (1 << 3)
#define AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE                   (1 << 4)
#define AON_CMU_EN_CLK_32K_CODEC_DISABLE                    (1 << 5)
#define AON_CMU_EN_CLK_32K_MCU_DISABLE                      (1 << 6)
#define AON_CMU_EN_CLK_32K_BTH_DISABLE                      (1 << 7)
#define AON_CMU_EN_CLK_32K_BT_DISABLE                       (1 << 8)
#define AON_CMU_EN_CLK_DCDC0_DISABLE                        (1 << 9)
#define AON_CMU_EN_CLK_TOP_PLLDSI_DISABLE                   (1 << 10)
#define AON_CMU_PU_PLLDSI_DISABLE                           (1 << 11)
#define AON_CMU_EN_X2_DIG_DISABLE                           (1 << 12)
#define AON_CMU_EN_X4_DIG_DISABLE                           (1 << 13)
#define AON_CMU_PU_PLLBB_DISABLE                            (1 << 14)
#define AON_CMU_PU_PLLUSB_DISABLE                           (1 << 15)
#define AON_CMU_PU_RC_DISABLE                               (1 << 16)
#define AON_CMU_PU_OSC_DISABLE                              (1 << 17)
#define AON_CMU_EN_CLK_TOP_OSCX4_DISABLE                    (1 << 18)
#define AON_CMU_EN_BT_CLK_SYS_DISABLE                       (1 << 19)
#define AON_CMU_SEL_CLK_OSC_MCU_DISABLE                     (1 << 20)
#define AON_CMU_SEL_CLK_OSC_SEN_DISABLE                     (1 << 21)
#define AON_CMU_SEL_CLK_OSC_CDC_DISABLE                     (1 << 22)
#define AON_CMU_SEL_CLK_OSC_BTH_DISABLE                     (1 << 23)
#define AON_CMU_SEL_CLK_OSC_BTC_DISABLE                     (1 << 24)
#define AON_CMU_SEL_CLK_OSC_PRE_DISABLE                     (1 << 25)

#define AON_ARST_NUM                                        19
#define AON_ORST_NUM                                        13
#define AON_ACLK_NUM                                        AON_ARST_NUM
#define AON_OCLK_NUM                                        AON_ORST_NUM

// reg_00c
#define AON_CMU_ARESETN_PULSE(n)                            (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_PULSE_MASK                          (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_PULSE_SHIFT                         (0)
#define AON_CMU_ORESETN_PULSE(n)                            (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_PULSE_MASK                          (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_PULSE_SHIFT                         (AON_ARST_NUM)

// reg_010
#define AON_CMU_ARESETN_SET(n)                              (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_SET_MASK                            (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_SET_SHIFT                           (0)
#define AON_CMU_ORESETN_SET(n)                              (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_SET_MASK                            (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_SET_SHIFT                           (AON_ARST_NUM)

// reg_014
#define AON_CMU_ARESETN_CLR(n)                              (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_CLR_MASK                            (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_CLR_SHIFT                           (0)
#define AON_CMU_ORESETN_CLR(n)                              (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_CLR_MASK                            (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_CLR_SHIFT                           (AON_ARST_NUM)

// reg_018
#define AON_CMU_SEL_OSCX2_DIG                               (1 << 0)
#define AON_CMU_SEL_X2_PHASE(n)                             (((n) & 0x1F) << 1)
#define AON_CMU_SEL_X2_PHASE_MASK                           (0x1F << 1)
#define AON_CMU_SEL_X2_PHASE_SHIFT                          (1)
#define AON_CMU_SEL_X4_PHASE(n)                             (((n) & 0x1F) << 6)
#define AON_CMU_SEL_X4_PHASE_MASK                           (0x1F << 6)
#define AON_CMU_SEL_X4_PHASE_SHIFT                          (6)
#define AON_CMU_SEL_X4_DIG                                  (1 << 11)
#define AON_CMU_LPU_AUTO_SWITCH26                           (1 << 12)
#define AON_CMU_EN_MCU_WDG_RESET                            (1 << 13)
#define AON_CMU_TIMER_WT24(n)                               (((n) & 0xFF) << 14)
#define AON_CMU_TIMER_WT24_MASK                             (0xFF << 14)
#define AON_CMU_TIMER_WT24_SHIFT                            (14)
#define AON_CMU_TIMER_WT24_EN                               (1 << 22)
#define AON_CMU_OSC_READY_MODE                              (1 << 23)
#define AON_CMU_SEL_PU_OSC_READY_ANA                        (1 << 24)
#define AON_CMU_EN_SENS_WDG_RESET                           (1 << 25)
#define AON_CMU_OSC_READY_BYPASS_SYNC                       (1 << 26)
#define AON_CMU_SEL_SLOW_SYS_BYPASS                         (1 << 27)
#define AON_CMU_EN_BTH_WDG_RESET                            (1 << 28)
#define AON_CMU_EN_BTC_WDG_RESET                            (1 << 29)
#define AON_CMU_EN_OSC_READY_MODE                           (1 << 30)
#define AON_CMU_SEL_X4_RC                                   (1 << 31)

// reg_01c
#define AON_CMU_EN_CLK_OUT                                  (1 << 0)
#define AON_CMU_SEL_CLK_OUT(n)                              (((n) & 0x7) << 1)
#define AON_CMU_SEL_CLK_OUT_MASK                            (0x7 << 1)
#define AON_CMU_SEL_CLK_OUT_SHIFT                           (1)
#define AON_CMU_CFG_CLK_OUT(n)                              (((n) & 0xF) << 4)
#define AON_CMU_CFG_CLK_OUT_MASK                            (0xF << 4)
#define AON_CMU_CFG_CLK_OUT_SHIFT                           (4)
#define AON_CMU_CFG_DIV_DCDC(n)                             (((n) & 0xF) << 8)
#define AON_CMU_CFG_DIV_DCDC_MASK                           (0xF << 8)
#define AON_CMU_CFG_DIV_DCDC_SHIFT                          (8)
#define AON_CMU_BYPASS_DIV_DCDC                             (1 << 12)
#define AON_CMU_CLK_DCDC_DRV(n)                             (((n) & 0x3) << 13)
#define AON_CMU_CLK_DCDC_DRV_MASK                           (0x3 << 13)
#define AON_CMU_CLK_DCDC_DRV_SHIFT                          (13)
#define AON_CMU_SEL_32K_TIMER(n)                            (((n) & 0x3) << 15)
#define AON_CMU_SEL_32K_TIMER_MASK                          (0x3 << 15)
#define AON_CMU_SEL_32K_TIMER_SHIFT                         (15)
#define AON_CMU_SEL_32K_WDT                                 (1 << 17)
#define AON_CMU_SEL_TIMER_FAST(n)                           (((n) & 0x3) << 18)
#define AON_CMU_SEL_TIMER_FAST_MASK                         (0x3 << 18)
#define AON_CMU_SEL_TIMER_FAST_SHIFT                        (18)
#define AON_CMU_CFG_TIMER_FAST(n)                           (((n) & 0x3) << 20)
#define AON_CMU_CFG_TIMER_FAST_MASK                         (0x3 << 20)
#define AON_CMU_CFG_TIMER_FAST_SHIFT                        (20)
#define AON_CMU_SEL_HCLK_OSC_AON                            (1 << 22)
#define AON_CMU_SEL_CODEC_HCLK_SENS_AON                     (1 << 23)
#define AON_CMU_CLK_DCDC_DRV_HI(n)                          (((n) & 0x3) << 24)
#define AON_CMU_CLK_DCDC_DRV_HI_MASK                        (0x3 << 24)
#define AON_CMU_CLK_DCDC_DRV_HI_SHIFT                       (24)

// reg_020
#define AON_CMU_WRITE_UNLOCK_H                              (1 << 0)
#define AON_CMU_WRITE_UNLOCK_STATUS                         (1 << 1)

// reg_024
#define AON_CMU_RAM5_SLP0(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM5_SLP0_MASK                              (0x3FF << 0)
#define AON_CMU_RAM5_SLP0_SHIFT                             (0)
#define AON_CMU_RAM5_DSLP0(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM5_DSLP0_MASK                             (0x3FF << 10)
#define AON_CMU_RAM5_DSLP0_SHIFT                            (10)
#define AON_CMU_RAM5_SD0(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM5_SD0_MASK                               (0x3FF << 20)
#define AON_CMU_RAM5_SD0_SHIFT                              (20)

// reg_028
#define AON_CMU_RAM5_SLP1(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM5_SLP1_MASK                              (0x3FF << 0)
#define AON_CMU_RAM5_SLP1_SHIFT                             (0)
#define AON_CMU_RAM5_DSLP1(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM5_DSLP1_MASK                             (0x3FF << 10)
#define AON_CMU_RAM5_DSLP1_SHIFT                            (10)
#define AON_CMU_RAM5_SD1(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM5_SD1_MASK                               (0x3FF << 20)
#define AON_CMU_RAM5_SD1_SHIFT                              (20)

// reg_02c
#define AON_CMU_RAM6_SLP0(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM6_SLP0_MASK                              (0x3FF << 0)
#define AON_CMU_RAM6_SLP0_SHIFT                             (0)
#define AON_CMU_RAM6_DSLP0(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM6_DSLP0_MASK                             (0x3FF << 10)
#define AON_CMU_RAM6_DSLP0_SHIFT                            (10)
#define AON_CMU_RAM6_SD0(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM6_SD0_MASK                               (0x3FF << 20)
#define AON_CMU_RAM6_SD0_SHIFT                              (20)

// reg_030
#define AON_CMU_RAM6_SLP1(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM6_SLP1_MASK                              (0x3FF << 0)
#define AON_CMU_RAM6_SLP1_SHIFT                             (0)
#define AON_CMU_RAM6_DSLP1(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM6_DSLP1_MASK                             (0x3FF << 10)
#define AON_CMU_RAM6_DSLP1_SHIFT                            (10)
#define AON_CMU_RAM6_SD1(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM6_SD1_MASK                               (0x3FF << 20)
#define AON_CMU_RAM6_SD1_SHIFT                              (20)

// reg_034
#define AON_CMU_RAM_POFF0(n)                                (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM_POFF0_MASK                              (0xFFFFFFFF << 0)
#define AON_CMU_RAM_POFF0_SHIFT                             (0)

// reg_038
#define AON_CMU_WATCHDOG_RESET                              (1 << 0)
#define AON_CMU_SOFT_GLOBLE_RESET                           (1 << 1)
#define AON_CMU_RTC_INTR_H                                  (1 << 2)
#define AON_CMU_CHG_INTR_H                                  (1 << 3)
#define AON_CMU_SOFT_BOOT_MODE(n)                           (((n) & 0xFFFFFFF) << 4)
#define AON_CMU_SOFT_BOOT_MODE_MASK                         (0xFFFFFFF << 4)
#define AON_CMU_SOFT_BOOT_MODE_SHIFT                        (4)

// reg_03c
#define AON_CMU_RESERVED(n)                                 (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RESERVED_MASK                               (0xFFFFFFFF << 0)
#define AON_CMU_RESERVED_SHIFT                              (0)
#define AON_CMU_RESERVED_CODEC_DMA_SEL(n)                   (((n) & 0x1F) << 16)
#define AON_CMU_RESERVED_CODEC_DMA_SEL_MASK                 (0x1F << 16)
#define AON_CMU_RESERVED_CODEC_DMA_SEL_SHIFT                (16)
#define AON_CMU_RESERVED_PSRAM_DLL_CK_RDY_AUTO_SW           (1 << 21)
#define AON_CMU_RESERVED_PSRAM_PU_DVDD_AUTO_SW              (1 << 22)
#define AON_CMU_RESERVED_PSRAM_PU2_AUTO_SW                  (1 << 23)
#define AON_CMU_RESERVED_PSRAM_PU_AUTO_SW                   (1 << 24)
#define AON_CMU_RESERVED_PSRAM_LDO_PU_AUTO_SW               (1 << 25)
#define AON_CMU_RESERVED_PSRAM_LDO_PRECHARGE_AUTO_SW        (1 << 26)

// reg_040
#define AON_CMU_MANUAL_ACLK_ENABLE(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MANUAL_ACLK_ENABLE_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_MANUAL_ACLK_ENABLE_SHIFT                    (0)
#define AON_CMU_MANUAL_OCLK_ENABLE(n)                       (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_ENABLE_MASK                     (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_ENABLE_SHIFT                    (AON_ACLK_NUM)

// reg_044
#define AON_CMU_MANUAL_ACLK_DISABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MANUAL_ACLK_DISABLE_MASK                    (0xFFFFFFFF << 0)
#define AON_CMU_MANUAL_ACLK_DISABLE_SHIFT                   (0)
#define AON_CMU_MANUAL_OCLK_DISABLE(n)                      (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_DISABLE_MASK                    (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_DISABLE_SHIFT                   (AON_ACLK_NUM)

// reg_048
#define AON_CMU_MODE_ACLK(n)                                (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MODE_ACLK_MASK                              (0xFFFFFFFF << 0)
#define AON_CMU_MODE_ACLK_SHIFT                             (0)
#define AON_CMU_MODE_OCLK(n)                                (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MODE_OCLK_MASK                              (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MODE_OCLK_SHIFT                             (AON_ACLK_NUM)

// reg_04c
#define AON_CMU_SEL_CLK_OSC                                 (1 << 0)
#define AON_CMU_BT_OSC_AUTOSW                               (1 << 1)
#define AON_CMU_POL_SPI_CS(n)                               (((n) & 0x7) << 2)
#define AON_CMU_POL_SPI_CS_MASK                             (0x7 << 2)
#define AON_CMU_POL_SPI_CS_SHIFT                            (2)
#define AON_CMU_CFG_SPI_ARB(n)                              (((n) & 0x7) << 5)
#define AON_CMU_CFG_SPI_ARB_MASK                            (0x7 << 5)
#define AON_CMU_CFG_SPI_ARB_SHIFT                           (5)
#define AON_CMU_PU_FLASH_IO                                 (1 << 8)
#define AON_CMU_SEL_CODEC_DMA_SENS                          (1 << 9)
#define AON_CMU_SEL_BT_PCM_SENS                             (1 << 10)
#define AON_CMU_PU_FLASH1_IO                                (1 << 11)
#define AON_CMU_SEL_I2S_MCLK(n)                             (((n) & 0x3) << 12)
#define AON_CMU_SEL_I2S_MCLK_MASK                           (0x3 << 12)
#define AON_CMU_SEL_I2S_MCLK_SHIFT                          (12)
#define AON_CMU_EN_I2S_MCLK                                 (1 << 14)
#define AON_CMU_SEL_PDM_CLKOUT1_SENS                        (1 << 15)
#define AON_CMU_SEL_PDM_CLKOUT0_SENS                        (1 << 16)
#define AON_CMU_SEL_PDM_DATAIN0_SYS                         (1 << 17)
#define AON_CMU_SEL_PDM_DATAIN1_SYS                         (1 << 18)
#define AON_CMU_SEL_PDM_DATAIN2_SYS                         (1 << 19)
#define AON_CMU_SEL_CLK_OSC_2                               (1 << 20)
#define AON_CMU_SEL_CLK_OSC_4                               (1 << 21)
#define AON_CMU_EN_CLK_SPI_ARB                              (1 << 22)
#define AON_CMU_RF_INTF_AUTO_MODE                           (1 << 23)
#define AON_CMU_PU_DSI_AUTO_MODE                            (1 << 24)
#define AON_CMU_OSC_READY_AON_SYNC                          (1 << 25)

// reg_050
#define AON_CMU_FREE_TIMER(n)                               (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_FREE_TIMER_MASK                             (0xFFFFFFFF << 0)
#define AON_CMU_FREE_TIMER_SHIFT                            (0)

// reg_054
#define AON_CMU_RAM_POFF1(n)                                (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM_POFF1_MASK                              (0xFFFFFFFF << 0)
#define AON_CMU_RAM_POFF1_SHIFT                             (0)

// reg_058
#define AON_CMU_RAM2_POFF0(n)                               (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM2_POFF0_MASK                             (0xFFFFFFFF << 0)
#define AON_CMU_RAM2_POFF0_SHIFT                            (0)

// reg_05c
#define AON_CMU_RAM_TEST1                                   (1 << 0)
#define AON_CMU_RAM_TEST_RNM                                (1 << 1)
#define AON_CMU_RAM_RME                                     (1 << 2)
#define AON_CMU_RAM_RM(n)                                   (((n) & 0xF) << 3)
#define AON_CMU_RAM_RM_MASK                                 (0xF << 3)
#define AON_CMU_RAM_RM_SHIFT                                (3)
#define AON_CMU_RAM_RA(n)                                   (((n) & 0x3) << 7)
#define AON_CMU_RAM_RA_MASK                                 (0x3 << 7)
#define AON_CMU_RAM_RA_SHIFT                                (7)
#define AON_CMU_RAM_WA(n)                                   (((n) & 0x7) << 9)
#define AON_CMU_RAM_WA_MASK                                 (0x7 << 9)
#define AON_CMU_RAM_WA_SHIFT                                (9)
#define AON_CMU_RAM_WPULSE(n)                               (((n) & 0x7) << 12)
#define AON_CMU_RAM_WPULSE_MASK                             (0x7 << 12)
#define AON_CMU_RAM_WPULSE_SHIFT                            (12)
#define AON_CMU_RF_TEST1                                    (1 << 15)
#define AON_CMU_RF_TEST_RNM                                 (1 << 16)
#define AON_CMU_RF_RME                                      (1 << 17)
#define AON_CMU_RF_RM(n)                                    (((n) & 0xF) << 18)
#define AON_CMU_RF_RM_MASK                                  (0xF << 18)
#define AON_CMU_RF_RM_SHIFT                                 (18)
#define AON_CMU_RF_RA(n)                                    (((n) & 0x3) << 22)
#define AON_CMU_RF_RA_MASK                                  (0x3 << 22)
#define AON_CMU_RF_RA_SHIFT                                 (22)
#define AON_CMU_RF_WA(n)                                    (((n) & 0x7) << 24)
#define AON_CMU_RF_WA_MASK                                  (0x7 << 24)
#define AON_CMU_RF_WA_SHIFT                                 (24)
#define AON_CMU_RF_WPULSE(n)                                (((n) & 0x7) << 27)
#define AON_CMU_RF_WPULSE_MASK                              (0x7 << 27)
#define AON_CMU_RF_WPULSE_SHIFT                             (27)
#define AON_CMU_CODEC_RAM_RTSEL(n)                          (((n) & 0x3) << 30)
#define AON_CMU_CODEC_RAM_RTSEL_MASK                        (0x3 << 30)
#define AON_CMU_CODEC_RAM_RTSEL_SHIFT                       (30)

// reg_060
#define AON_CMU_RAM2_POFF1(n)                               (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM2_POFF1_MASK                             (0xFFFFFFFF << 0)
#define AON_CMU_RAM2_POFF1_SHIFT                            (0)

// reg_064
#define AON_CMU_POWER_MODE_MCU(n)                           (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_POWER_MODE_MCU_MASK                         (0xFFFFFFFF << 0)
#define AON_CMU_POWER_MODE_MCU_SHIFT                        (0)

// reg_068
#define AON_CMU_TIMER1_MCU_REG(n)                           (((n) & 0xF) << 0)
#define AON_CMU_TIMER1_MCU_REG_MASK                         (0xF << 0)
#define AON_CMU_TIMER1_MCU_REG_SHIFT                        (0)
#define AON_CMU_TIMER2_MCU_REG(n)                           (((n) & 0xF) << 4)
#define AON_CMU_TIMER2_MCU_REG_MASK                         (0xF << 4)
#define AON_CMU_TIMER2_MCU_REG_SHIFT                        (4)
#define AON_CMU_TIMER3_MCU_REG(n)                           (((n) & 0xF) << 8)
#define AON_CMU_TIMER3_MCU_REG_MASK                         (0xF << 8)
#define AON_CMU_TIMER3_MCU_REG_SHIFT                        (8)
#define AON_CMU_PG_AUTO_EN_MCU_REG                          (1 << 12)
#define AON_CMU_POWER_MODE_BT_DR                            (1 << 13)
#define AON_CMU_USB_EN_RFS                                  (1 << 14)
#define AON_CMU_USB_EN_R15K                                 (1 << 15)
#define AON_CMU_USB_FS_LS_SEL                               (1 << 16)
#define AON_CMU_USB_POL_RX_DP                               (1 << 17)
#define AON_CMU_USB_POL_RX_DM                               (1 << 18)
#define AON_CMU_USB_DEBOUNCE_EN                             (1 << 19)
#define AON_CMU_USB_INSERT_DET_EN                           (1 << 20)
#define AON_CMU_USB_INSERT_INTR_EN                          (1 << 21)
#define AON_CMU_USB_INTR_CLR                                (1 << 22)
#define AON_CMU_USB_NOLS_MODE                               (1 << 23)
#define AON_CMU_CFG_FS_EN_RFS_DR                            (1 << 24)
#define AON_CMU_USB_STAT_RX_DP                              (1 << 25)
#define AON_CMU_USB_STAT_RX_DM                              (1 << 26)

// reg_06c
#define AON_CMU_TIMER_PU_OSC_BT(n)                          (((n) & 0xFF) << 0)
#define AON_CMU_TIMER_PU_OSC_BT_MASK                        (0xFF << 0)
#define AON_CMU_TIMER_PU_OSC_BT_SHIFT                       (0)
#define AON_CMU_TIMER_EN_OSC_BT(n)                          (((n) & 0xFF) << 8)
#define AON_CMU_TIMER_EN_OSC_BT_MASK                        (0xFF << 8)
#define AON_CMU_TIMER_EN_OSC_BT_SHIFT                       (8)
#define AON_CMU_USB_LS_MODE                                 (1 << 16)
#define AON_CMU_USB_EN_RLS                                  (1 << 17)
#define AON_CMU_USB_RESETB_PHY                              (1 << 18)
#define AON_CMU_USB_LOOP_BACK                               (1 << 19)
#define AON_CMU_USB_RX_PD                                   (1 << 20)
#define AON_CMU_USB_DRV_CTRL(n)                             (((n) & 0x1F) << 21)
#define AON_CMU_USB_DRV_CTRL_MASK                           (0x1F << 21)
#define AON_CMU_USB_DRV_CTRL_SHIFT                          (21)

// reg_070
#define AON_CMU_RAM3_POFF0(n)                               (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM3_POFF0_MASK                             (0xFFFFFFFF << 0)
#define AON_CMU_RAM3_POFF0_SHIFT                            (0)

// reg_074
#define AON_CMU_RAM3_POFF1(n)                               (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM3_POFF1_MASK                             (0xFFFFFFFF << 0)
#define AON_CMU_RAM3_POFF1_SHIFT                            (0)

// reg_078
#define AON_CMU_SRAM_AUTO_EN_MODES(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_SRAM_AUTO_EN_MODES_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_SRAM_AUTO_EN_MODES_SHIFT                    (0)

// reg_07c
#define AON_CMU_OTP_WR_LOCK(n)                              (((n) & 0xFFFF) << 0)
#define AON_CMU_OTP_WR_LOCK_MASK                            (0xFFFF << 0)
#define AON_CMU_OTP_WR_LOCK_SHIFT                           (0)
#define AON_CMU_OTP_WR_UNLOCK                               (1 << 31)

// reg_080
#define AON_CMU_OTP_RD_LOCK(n)                              (((n) & 0xFFFF) << 0)
#define AON_CMU_OTP_RD_LOCK_MASK                            (0xFFFF << 0)
#define AON_CMU_OTP_RD_LOCK_SHIFT                           (0)
#define AON_CMU_OTP_RD_UNLOCK                               (1 << 31)

// reg_084
#define AON_CMU_CFG_PD_STAB_TIMER(n)                        (((n) & 0xF) << 0)
#define AON_CMU_CFG_PD_STAB_TIMER_MASK                      (0xF << 0)
#define AON_CMU_CFG_PD_STAB_TIMER_SHIFT                     (0)
#define AON_CMU_SEL_PSC_FAST                                (1 << 4)
#define AON_CMU_SEL_32K_PSC                                 (1 << 5)
#define AON_CMU_CFG_DIV_PSC(n)                              (((n) & 0xFF) << 6)
#define AON_CMU_CFG_DIV_PSC_MASK                            (0xFF << 6)
#define AON_CMU_CFG_DIV_PSC_SHIFT                           (6)
#define AON_CMU_OSC_MCU_ON                                  (1 << 14)
#define AON_CMU_OSC_SENS_ON                                 (1 << 15)
#define AON_CMU_OSC_BTH_ON                                  (1 << 16)
#define AON_CMU_OSC_BTC_ON                                  (1 << 17)
#define AON_CMU_OSCX4_BTC_ON                                (1 << 18)
#define AON_CMU_OSC_PSC_ON                                  (1 << 19)
#define AON_CMU_OSCX2_PSC_ON                                (1 << 20)
#define AON_CMU_OSCX4_PSC_ON                                (1 << 21)
#define AON_CMU_OSC_OTHR_ON                                 (1 << 22)
#define AON_CMU_OSCX2_OTHR_ON                               (1 << 23)
#define AON_CMU_OSCX4_OTHR_ON                               (1 << 24)

// reg_088
#define AON_CMU_RAM_SLP0(n)                                 (((n) & 0x3FF) << 0)
#define AON_CMU_RAM_SLP0_MASK                               (0x3FF << 0)
#define AON_CMU_RAM_SLP0_SHIFT                              (0)
#define AON_CMU_RAM_DSLP0(n)                                (((n) & 0x3FF) << 10)
#define AON_CMU_RAM_DSLP0_MASK                              (0x3FF << 10)
#define AON_CMU_RAM_DSLP0_SHIFT                             (10)
#define AON_CMU_RAM_SD0(n)                                  (((n) & 0x3FF) << 20)
#define AON_CMU_RAM_SD0_MASK                                (0x3FF << 20)
#define AON_CMU_RAM_SD0_SHIFT                               (20)

// reg_08c
#define AON_CMU_RAM_SLP1(n)                                 (((n) & 0x3FF) << 0)
#define AON_CMU_RAM_SLP1_MASK                               (0x3FF << 0)
#define AON_CMU_RAM_SLP1_SHIFT                              (0)
#define AON_CMU_RAM_DSLP1(n)                                (((n) & 0x3FF) << 10)
#define AON_CMU_RAM_DSLP1_MASK                              (0x3FF << 10)
#define AON_CMU_RAM_DSLP1_SHIFT                             (10)
#define AON_CMU_RAM_SD1(n)                                  (((n) & 0x3FF) << 20)
#define AON_CMU_RAM_SD1_MASK                                (0x3FF << 20)
#define AON_CMU_RAM_SD1_SHIFT                               (20)

// reg_090
#define AON_CMU_RAM2_SLP0(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM2_SLP0_MASK                              (0x3FF << 0)
#define AON_CMU_RAM2_SLP0_SHIFT                             (0)
#define AON_CMU_RAM2_DSLP0(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM2_DSLP0_MASK                             (0x3FF << 10)
#define AON_CMU_RAM2_DSLP0_SHIFT                            (10)
#define AON_CMU_RAM2_SD0(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM2_SD0_MASK                               (0x3FF << 20)
#define AON_CMU_RAM2_SD0_SHIFT                              (20)

// reg_094
#define AON_CMU_RAM2_SLP1(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM2_SLP1_MASK                              (0x3FF << 0)
#define AON_CMU_RAM2_SLP1_SHIFT                             (0)
#define AON_CMU_RAM2_DSLP1(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM2_DSLP1_MASK                             (0x3FF << 10)
#define AON_CMU_RAM2_DSLP1_SHIFT                            (10)
#define AON_CMU_RAM2_SD1(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM2_SD1_MASK                               (0x3FF << 20)
#define AON_CMU_RAM2_SD1_SHIFT                              (20)

// reg_098
#define AON_CMU_SRAM0_CFG_SEL(n)                            (((n) & 0x3) << 0)
#define AON_CMU_SRAM0_CFG_SEL_MASK                          (0x3 << 0)
#define AON_CMU_SRAM0_CFG_SEL_SHIFT                         (0)
#define AON_CMU_SRAM1_CFG_SEL(n)                            (((n) & 0x3) << 2)
#define AON_CMU_SRAM1_CFG_SEL_MASK                          (0x3 << 2)
#define AON_CMU_SRAM1_CFG_SEL_SHIFT                         (2)
#define AON_CMU_SRAM2_CFG_SEL(n)                            (((n) & 0x3) << 4)
#define AON_CMU_SRAM2_CFG_SEL_MASK                          (0x3 << 4)
#define AON_CMU_SRAM2_CFG_SEL_SHIFT                         (4)
#define AON_CMU_SRAM3_CFG_SEL(n)                            (((n) & 0x3) << 6)
#define AON_CMU_SRAM3_CFG_SEL_MASK                          (0x3 << 6)
#define AON_CMU_SRAM3_CFG_SEL_SHIFT                         (6)
#define AON_CMU_SRAM4_CFG_SEL(n)                            (((n) & 0x3) << 8)
#define AON_CMU_SRAM4_CFG_SEL_MASK                          (0x3 << 8)
#define AON_CMU_SRAM4_CFG_SEL_SHIFT                         (8)
#define AON_CMU_SRAM5_CFG_SEL(n)                            (((n) & 0x3) << 10)
#define AON_CMU_SRAM5_CFG_SEL_MASK                          (0x3 << 10)
#define AON_CMU_SRAM5_CFG_SEL_SHIFT                         (10)
#define AON_CMU_SRAM6_CFG_SEL(n)                            (((n) & 0x3) << 12)
#define AON_CMU_SRAM6_CFG_SEL_MASK                          (0x3 << 12)
#define AON_CMU_SRAM6_CFG_SEL_SHIFT                         (12)
#define AON_CMU_SRAM7_CFG_SEL(n)                            (((n) & 0x3) << 14)
#define AON_CMU_SRAM7_CFG_SEL_MASK                          (0x3 << 14)
#define AON_CMU_SRAM7_CFG_SEL_SHIFT                         (14)
#define AON_CMU_SRAM8_CFG_SEL(n)                            (((n) & 0x3) << 16)
#define AON_CMU_SRAM8_CFG_SEL_MASK                          (0x3 << 16)
#define AON_CMU_SRAM8_CFG_SEL_SHIFT                         (16)
#define AON_CMU_SRAM9_CFG_SEL(n)                            (((n) & 0x3) << 18)
#define AON_CMU_SRAM9_CFG_SEL_MASK                          (0x3 << 18)
#define AON_CMU_SRAM9_CFG_SEL_SHIFT                         (18)
#define AON_CMU_SRAM10_CFG_SEL(n)                           (((n) & 0x3) << 20)
#define AON_CMU_SRAM10_CFG_SEL_MASK                         (0x3 << 20)
#define AON_CMU_SRAM10_CFG_SEL_SHIFT                        (20)
#define AON_CMU_SENS_SRAM4_CFG_SEL                          (1 << 22)
#define AON_CMU_SENS_SRAM5_CFG_SEL                          (1 << 23)

// reg_09c
#define AON_CMU_SRAM2_AUTO_EN_MODES(n)                      (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_SRAM2_AUTO_EN_MODES_MASK                    (0xFFFFFFFF << 0)
#define AON_CMU_SRAM2_AUTO_EN_MODES_SHIFT                   (0)

// reg_0a0
#define AON_CMU_EN_CLK_PLLBB_MCU_ENABLE                     (1 << 0)
#define AON_CMU_EN_CLK_PLLDSI_MCU_ENABLE                    (1 << 1)
#define AON_CMU_EN_CLK_PLLUSB_MCU_ENABLE                    (1 << 2)
#define AON_CMU_EN_CLK_OSCX4_BTH_ENABLE                     (1 << 3)
#define AON_CMU_EN_CLK_OSCX4_BT_ENABLE                      (1 << 4)
#define AON_CMU_EN_CLK_PLLBB_BT_ENABLE                      (1 << 5)
#define AON_CMU_EN_CLK_OSC_CODEC_ENABLE                     (1 << 6)
#define AON_CMU_EN_CLK_OSCX2_CODEC_ENABLE                   (1 << 7)
#define AON_CMU_EN_CLK_OSCX4_CODEC_ENABLE                   (1 << 8)
#define AON_CMU_EN_CLK_PLLBB_CODEC_ENABLE                   (1 << 9)
#define AON_CMU_EN_CLK_PLLUSB_CODEC_ENABLE                  (1 << 10)
#define AON_CMU_EN_CLK_PLLBB_BTH_ENABLE                     (1 << 11)
#define AON_CMU_EN_CLK_32K_SENS_ENABLE                      (1 << 12)
#define AON_CMU_EN_CLK_PLLBB_SENS_ENABLE                    (1 << 13)

// reg_0a4
#define AON_CMU_EN_CLK_PLLBB_MCU_DISABLE                    (1 << 0)
#define AON_CMU_EN_CLK_PLLDSI_MCU_DISABLE                   (1 << 1)
#define AON_CMU_EN_CLK_PLLUSB_MCU_DISABLE                   (1 << 2)
#define AON_CMU_EN_CLK_OSCX4_BTH_DISABLE                    (1 << 3)
#define AON_CMU_EN_CLK_OSCX4_BT_DISABLE                     (1 << 4)
#define AON_CMU_EN_CLK_PLLBB_BT_DISABLE                     (1 << 5)
#define AON_CMU_EN_CLK_OSC_CODEC_DISABLE                    (1 << 6)
#define AON_CMU_EN_CLK_OSCX2_CODEC_DISABLE                  (1 << 7)
#define AON_CMU_EN_CLK_OSCX4_CODEC_DISABLE                  (1 << 8)
#define AON_CMU_EN_CLK_PLLBB_CODEC_DISABLE                  (1 << 9)
#define AON_CMU_EN_CLK_PLLUSB_CODEC_DISABLE                 (1 << 10)
#define AON_CMU_EN_CLK_PLLBB_BTH_DISABLE                    (1 << 11)
#define AON_CMU_EN_CLK_32K_SENS_DISABLE                     (1 << 12)
#define AON_CMU_EN_CLK_PLLBB_SENS_DISABLE                   (1 << 13)

// reg_0a8
#define AON_CMU_RAM_BC1                                     (1 << 0)
#define AON_CMU_RAM_BC2                                     (1 << 1)
#define AON_CMU_RF_BC1                                      (1 << 2)
#define AON_CMU_RF_BC2                                      (1 << 3)
#define AON_CMU_BT_RAM_BC1                                  (1 << 4)
#define AON_CMU_BT_RAM_BC2                                  (1 << 5)
#define AON_CMU_BT_RF_BC1                                   (1 << 6)
#define AON_CMU_BT_RF_BC2                                   (1 << 7)
#define AON_CMU_BTH_RAM_BC1                                 (1 << 8)
#define AON_CMU_BTH_RAM_BC2                                 (1 << 9)
#define AON_CMU_BTH_RF_BC1                                  (1 << 10)
#define AON_CMU_BTH_RF_BC2                                  (1 << 11)
#define AON_CMU_SENS_RAM_BC1                                (1 << 12)
#define AON_CMU_SENS_RAM_BC2                                (1 << 13)
#define AON_CMU_SENS_RF_BC1                                 (1 << 14)
#define AON_CMU_SENS_RF_BC2                                 (1 << 15)
#define AON_CMU_CODEC_RAM_BC1                               (1 << 16)
#define AON_CMU_CODEC_RAM_BC2                               (1 << 17)
#define AON_CMU_CODEC_RF_BC1                                (1 << 18)
#define AON_CMU_CODEC_RF_BC2                                (1 << 19)

// reg_0ac
#define AON_CMU_VTOR_CORE_SENS(n)                           (((n) & 0x1FFFFFF) << 7)
#define AON_CMU_VTOR_CORE_SENS_MASK                         (0x1FFFFFF << 7)
#define AON_CMU_VTOR_CORE_SENS_SHIFT                        (7)

// reg_0b0
#define AON_CMU_RAM3_SLP0(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM3_SLP0_MASK                              (0x3FF << 0)
#define AON_CMU_RAM3_SLP0_SHIFT                             (0)
#define AON_CMU_RAM3_DSLP0(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM3_DSLP0_MASK                             (0x3FF << 10)
#define AON_CMU_RAM3_DSLP0_SHIFT                            (10)
#define AON_CMU_RAM3_SD0(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM3_SD0_MASK                               (0x3FF << 20)
#define AON_CMU_RAM3_SD0_SHIFT                              (20)

// reg_0b4
#define AON_CMU_BT_RAM_TEST1                                (1 << 0)
#define AON_CMU_BT_RAM_TEST_RNM                             (1 << 1)
#define AON_CMU_BT_RAM_RME                                  (1 << 2)
#define AON_CMU_BT_RAM_RM(n)                                (((n) & 0xF) << 3)
#define AON_CMU_BT_RAM_RM_MASK                              (0xF << 3)
#define AON_CMU_BT_RAM_RM_SHIFT                             (3)
#define AON_CMU_BT_RAM_RA(n)                                (((n) & 0x3) << 7)
#define AON_CMU_BT_RAM_RA_MASK                              (0x3 << 7)
#define AON_CMU_BT_RAM_RA_SHIFT                             (7)
#define AON_CMU_BT_RAM_WA(n)                                (((n) & 0x7) << 9)
#define AON_CMU_BT_RAM_WA_MASK                              (0x7 << 9)
#define AON_CMU_BT_RAM_WA_SHIFT                             (9)
#define AON_CMU_BT_RAM_WPULSE(n)                            (((n) & 0x7) << 12)
#define AON_CMU_BT_RAM_WPULSE_MASK                          (0x7 << 12)
#define AON_CMU_BT_RAM_WPULSE_SHIFT                         (12)
#define AON_CMU_BT_RF_TEST1                                 (1 << 15)
#define AON_CMU_BT_RF_TEST_RNM                              (1 << 16)
#define AON_CMU_BT_RF_RME                                   (1 << 17)
#define AON_CMU_BT_RF_RM(n)                                 (((n) & 0xF) << 18)
#define AON_CMU_BT_RF_RM_MASK                               (0xF << 18)
#define AON_CMU_BT_RF_RM_SHIFT                              (18)
#define AON_CMU_BT_RF_RA(n)                                 (((n) & 0x3) << 22)
#define AON_CMU_BT_RF_RA_MASK                               (0x3 << 22)
#define AON_CMU_BT_RF_RA_SHIFT                              (22)
#define AON_CMU_BT_RF_WA(n)                                 (((n) & 0x7) << 24)
#define AON_CMU_BT_RF_WA_MASK                               (0x7 << 24)
#define AON_CMU_BT_RF_WA_SHIFT                              (24)
#define AON_CMU_BT_RF_WPULSE(n)                             (((n) & 0x7) << 27)
#define AON_CMU_BT_RF_WPULSE_MASK                           (0x7 << 27)
#define AON_CMU_BT_RF_WPULSE_SHIFT                          (27)
#define AON_CMU_CODEC_RAM_WTSEL(n)                          (((n) & 0x3) << 30)
#define AON_CMU_CODEC_RAM_WTSEL_MASK                        (0x3 << 30)
#define AON_CMU_CODEC_RAM_WTSEL_SHIFT                       (30)

// reg_0b8
#define AON_CMU_SENS_RAM_TEST1                              (1 << 0)
#define AON_CMU_SENS_RAM_TEST_RNM                           (1 << 1)
#define AON_CMU_SENS_RAM_RME                                (1 << 2)
#define AON_CMU_SENS_RAM_RM(n)                              (((n) & 0xF) << 3)
#define AON_CMU_SENS_RAM_RM_MASK                            (0xF << 3)
#define AON_CMU_SENS_RAM_RM_SHIFT                           (3)
#define AON_CMU_SENS_RAM_RA(n)                              (((n) & 0x3) << 7)
#define AON_CMU_SENS_RAM_RA_MASK                            (0x3 << 7)
#define AON_CMU_SENS_RAM_RA_SHIFT                           (7)
#define AON_CMU_SENS_RAM_WA(n)                              (((n) & 0x7) << 9)
#define AON_CMU_SENS_RAM_WA_MASK                            (0x7 << 9)
#define AON_CMU_SENS_RAM_WA_SHIFT                           (9)
#define AON_CMU_SENS_RAM_WPULSE(n)                          (((n) & 0x7) << 12)
#define AON_CMU_SENS_RAM_WPULSE_MASK                        (0x7 << 12)
#define AON_CMU_SENS_RAM_WPULSE_SHIFT                       (12)
#define AON_CMU_SENS_RF_TEST1                               (1 << 15)
#define AON_CMU_SENS_RF_TEST_RNM                            (1 << 16)
#define AON_CMU_SENS_RF_RME                                 (1 << 17)
#define AON_CMU_SENS_RF_RM(n)                               (((n) & 0xF) << 18)
#define AON_CMU_SENS_RF_RM_MASK                             (0xF << 18)
#define AON_CMU_SENS_RF_RM_SHIFT                            (18)
#define AON_CMU_SENS_RF_RA(n)                               (((n) & 0x3) << 22)
#define AON_CMU_SENS_RF_RA_MASK                             (0x3 << 22)
#define AON_CMU_SENS_RF_RA_SHIFT                            (22)
#define AON_CMU_SENS_RF_WA(n)                               (((n) & 0x7) << 24)
#define AON_CMU_SENS_RF_WA_MASK                             (0x7 << 24)
#define AON_CMU_SENS_RF_WA_SHIFT                            (24)
#define AON_CMU_SENS_RF_WPULSE(n)                           (((n) & 0x7) << 27)
#define AON_CMU_SENS_RF_WPULSE_MASK                         (0x7 << 27)
#define AON_CMU_SENS_RF_WPULSE_SHIFT                        (27)
#define AON_CMU_CODEC_RF_RTSEL(n)                           (((n) & 0x3) << 30)
#define AON_CMU_CODEC_RF_RTSEL_MASK                         (0x3 << 30)
#define AON_CMU_CODEC_RF_RTSEL_SHIFT                        (30)

// reg_0bc
#define AON_CMU_CODEC_RAM_TEST1                             (1 << 0)
#define AON_CMU_CODEC_RAM_TEST_RNM                          (1 << 1)
#define AON_CMU_CODEC_RAM_RME                               (1 << 2)
#define AON_CMU_CODEC_RAM_RM(n)                             (((n) & 0xF) << 3)
#define AON_CMU_CODEC_RAM_RM_MASK                           (0xF << 3)
#define AON_CMU_CODEC_RAM_RM_SHIFT                          (3)
#define AON_CMU_CODEC_RAM_RA(n)                             (((n) & 0x3) << 7)
#define AON_CMU_CODEC_RAM_RA_MASK                           (0x3 << 7)
#define AON_CMU_CODEC_RAM_RA_SHIFT                          (7)
#define AON_CMU_CODEC_RAM_WA(n)                             (((n) & 0x7) << 9)
#define AON_CMU_CODEC_RAM_WA_MASK                           (0x7 << 9)
#define AON_CMU_CODEC_RAM_WA_SHIFT                          (9)
#define AON_CMU_CODEC_RAM_WPULSE(n)                         (((n) & 0x7) << 12)
#define AON_CMU_CODEC_RAM_WPULSE_MASK                       (0x7 << 12)
#define AON_CMU_CODEC_RAM_WPULSE_SHIFT                      (12)
#define AON_CMU_CODEC_RF_TEST1                              (1 << 15)
#define AON_CMU_CODEC_RF_TEST_RNM                           (1 << 16)
#define AON_CMU_CODEC_RF_RME                                (1 << 17)
#define AON_CMU_CODEC_RF_RM(n)                              (((n) & 0xF) << 18)
#define AON_CMU_CODEC_RF_RM_MASK                            (0xF << 18)
#define AON_CMU_CODEC_RF_RM_SHIFT                           (18)
#define AON_CMU_CODEC_RF_RA(n)                              (((n) & 0x3) << 22)
#define AON_CMU_CODEC_RF_RA_MASK                            (0x3 << 22)
#define AON_CMU_CODEC_RF_RA_SHIFT                           (22)
#define AON_CMU_CODEC_RF_WA(n)                              (((n) & 0x7) << 24)
#define AON_CMU_CODEC_RF_WA_MASK                            (0x7 << 24)
#define AON_CMU_CODEC_RF_WA_SHIFT                           (24)
#define AON_CMU_CODEC_RF_WPULSE(n)                          (((n) & 0x7) << 27)
#define AON_CMU_CODEC_RF_WPULSE_MASK                        (0x7 << 27)
#define AON_CMU_CODEC_RF_WPULSE_SHIFT                       (27)
#define AON_CMU_CODEC_RF_WTSEL(n)                           (((n) & 0x3) << 30)
#define AON_CMU_CODEC_RF_WTSEL_MASK                         (0x3 << 30)
#define AON_CMU_CODEC_RF_WTSEL_SHIFT                        (30)

// reg_0c0
#define AON_CMU_RAM_RTSEL(n)                                (((n) & 0x3) << 0)
#define AON_CMU_RAM_RTSEL_MASK                              (0x3 << 0)
#define AON_CMU_RAM_RTSEL_SHIFT                             (0)
#define AON_CMU_RAM_WTSEL(n)                                (((n) & 0x3) << 2)
#define AON_CMU_RAM_WTSEL_MASK                              (0x3 << 2)
#define AON_CMU_RAM_WTSEL_SHIFT                             (2)
#define AON_CMU_RF_RTSEL(n)                                 (((n) & 0x3) << 4)
#define AON_CMU_RF_RTSEL_MASK                               (0x3 << 4)
#define AON_CMU_RF_RTSEL_SHIFT                              (4)
#define AON_CMU_RF_WTSEL(n)                                 (((n) & 0x3) << 6)
#define AON_CMU_RF_WTSEL_MASK                               (0x3 << 6)
#define AON_CMU_RF_WTSEL_SHIFT                              (6)
#define AON_CMU_BT_RAM_RTSEL(n)                             (((n) & 0x3) << 8)
#define AON_CMU_BT_RAM_RTSEL_MASK                           (0x3 << 8)
#define AON_CMU_BT_RAM_RTSEL_SHIFT                          (8)
#define AON_CMU_BT_RAM_WTSEL(n)                             (((n) & 0x3) << 10)
#define AON_CMU_BT_RAM_WTSEL_MASK                           (0x3 << 10)
#define AON_CMU_BT_RAM_WTSEL_SHIFT                          (10)
#define AON_CMU_BT_RF_RTSEL(n)                              (((n) & 0x3) << 12)
#define AON_CMU_BT_RF_RTSEL_MASK                            (0x3 << 12)
#define AON_CMU_BT_RF_RTSEL_SHIFT                           (12)
#define AON_CMU_BT_RF_WTSEL(n)                              (((n) & 0x3) << 14)
#define AON_CMU_BT_RF_WTSEL_MASK                            (0x3 << 14)
#define AON_CMU_BT_RF_WTSEL_SHIFT                           (14)
#define AON_CMU_SENS_RAM_RTSEL(n)                           (((n) & 0x3) << 16)
#define AON_CMU_SENS_RAM_RTSEL_MASK                         (0x3 << 16)
#define AON_CMU_SENS_RAM_RTSEL_SHIFT                        (16)
#define AON_CMU_SENS_RAM_WTSEL(n)                           (((n) & 0x3) << 18)
#define AON_CMU_SENS_RAM_WTSEL_MASK                         (0x3 << 18)
#define AON_CMU_SENS_RAM_WTSEL_SHIFT                        (18)
#define AON_CMU_SENS_RF_RTSEL(n)                            (((n) & 0x3) << 20)
#define AON_CMU_SENS_RF_RTSEL_MASK                          (0x3 << 20)
#define AON_CMU_SENS_RF_RTSEL_SHIFT                         (20)
#define AON_CMU_SENS_RF_WTSEL(n)                            (((n) & 0x3) << 22)
#define AON_CMU_SENS_RF_WTSEL_MASK                          (0x3 << 22)
#define AON_CMU_SENS_RF_WTSEL_SHIFT                         (22)
#define AON_CMU_BTH_RAM_RTSEL(n)                            (((n) & 0x3) << 24)
#define AON_CMU_BTH_RAM_RTSEL_MASK                          (0x3 << 24)
#define AON_CMU_BTH_RAM_RTSEL_SHIFT                         (24)
#define AON_CMU_BTH_RAM_WTSEL(n)                            (((n) & 0x3) << 26)
#define AON_CMU_BTH_RAM_WTSEL_MASK                          (0x3 << 26)
#define AON_CMU_BTH_RAM_WTSEL_SHIFT                         (26)
#define AON_CMU_BTH_RF_RTSEL(n)                             (((n) & 0x3) << 28)
#define AON_CMU_BTH_RF_RTSEL_MASK                           (0x3 << 28)
#define AON_CMU_BTH_RF_RTSEL_SHIFT                          (28)
#define AON_CMU_BTH_RF_WTSEL(n)                             (((n) & 0x3) << 30)
#define AON_CMU_BTH_RF_WTSEL_MASK                           (0x3 << 30)
#define AON_CMU_BTH_RF_WTSEL_SHIFT                          (30)

// reg_0c4
#define AON_CMU_SOFT_RSTN_MCU_PULSE                         (1 << 0)
#define AON_CMU_SOFT_RSTN_CODEC_PULSE                       (1 << 1)
#define AON_CMU_SOFT_RSTN_SENS_PULSE                        (1 << 2)
#define AON_CMU_SOFT_RSTN_BT_PULSE                          (1 << 3)
#define AON_CMU_SOFT_RSTN_MCUCPU_PULSE                      (1 << 4)
#define AON_CMU_SOFT_RSTN_SENSCPU_PULSE                     (1 << 5)
#define AON_CMU_SOFT_RSTN_BTCPU_PULSE                       (1 << 6)
#define AON_CMU_SOFT_RSTN_BTH_PULSE                         (1 << 7)
#define AON_CMU_SOFT_RSTN_BTHCPU_PULSE                      (1 << 8)
#define AON_CMU_SOFT_RSTN_MCUDSP_PULSE                      (1 << 9)
#define AON_CMU_GLOBAL_RESETN_PULSE                         (1 << 10)

// reg_0c8
#define AON_CMU_SOFT_RSTN_MCU_SET                           (1 << 0)
#define AON_CMU_SOFT_RSTN_CODEC_SET                         (1 << 1)
#define AON_CMU_SOFT_RSTN_SENS_SET                          (1 << 2)
#define AON_CMU_SOFT_RSTN_BT_SET                            (1 << 3)
#define AON_CMU_SOFT_RSTN_MCUCPU_SET                        (1 << 4)
#define AON_CMU_SOFT_RSTN_SENSCPU_SET                       (1 << 5)
#define AON_CMU_SOFT_RSTN_BTCPU_SET                         (1 << 6)
#define AON_CMU_SOFT_RSTN_BTH_SET                           (1 << 7)
#define AON_CMU_SOFT_RSTN_BTHCPU_SET                        (1 << 8)
#define AON_CMU_SOFT_RSTN_MCUDSP_SET                        (1 << 9)
#define AON_CMU_GLOBAL_RESETN_SET                           (1 << 10)

// reg_0cc
#define AON_CMU_SOFT_RSTN_MCU_CLR                           (1 << 0)
#define AON_CMU_SOFT_RSTN_CODEC_CLR                         (1 << 1)
#define AON_CMU_SOFT_RSTN_SENS_CLR                          (1 << 2)
#define AON_CMU_SOFT_RSTN_BT_CLR                            (1 << 3)
#define AON_CMU_SOFT_RSTN_MCUCPU_CLR                        (1 << 4)
#define AON_CMU_SOFT_RSTN_SENSCPU_CLR                       (1 << 5)
#define AON_CMU_SOFT_RSTN_BTCPU_CLR                         (1 << 6)
#define AON_CMU_SOFT_RSTN_BTH_CLR                           (1 << 7)
#define AON_CMU_SOFT_RSTN_BTHCPU_CLR                        (1 << 8)
#define AON_CMU_SOFT_RSTN_MCUDSP_CLR                        (1 << 9)
#define AON_CMU_GLOBAL_RESETN_CLR                           (1 << 10)

// reg_0d0
#define AON_CMU_FLASH0_IODRV(n)                             (((n) & 0x7) << 0)
#define AON_CMU_FLASH0_IODRV_MASK                           (0x7 << 0)
#define AON_CMU_FLASH0_IODRV_SHIFT                          (0)
#define AON_CMU_FLASH0_IOEN_DR                              (1 << 7)
#define AON_CMU_FLASH0_IOEN(n)                              (((n) & 0xFF) << 8)
#define AON_CMU_FLASH0_IOEN_MASK                            (0xFF << 8)
#define AON_CMU_FLASH0_IOEN_SHIFT                           (8)
#define AON_CMU_FLASH1_IODRV(n)                             (((n) & 0x7) << 16)
#define AON_CMU_FLASH1_IODRV_MASK                           (0x7 << 16)
#define AON_CMU_FLASH1_IODRV_SHIFT                          (16)
#define AON_CMU_FLASH1_IOEN_DR                              (1 << 23)
#define AON_CMU_FLASH1_IOEN(n)                              (((n) & 0xFF) << 24)
#define AON_CMU_FLASH1_IOEN_MASK                            (0xFF << 24)
#define AON_CMU_FLASH1_IOEN_SHIFT                           (24)

// reg_0d4
#define AON_CMU_BTH_RAM_TEST1                               (1 << 0)
#define AON_CMU_BTH_RAM_TEST_RNM                            (1 << 1)
#define AON_CMU_BTH_RAM_RME                                 (1 << 2)
#define AON_CMU_BTH_RAM_RM(n)                               (((n) & 0xF) << 3)
#define AON_CMU_BTH_RAM_RM_MASK                             (0xF << 3)
#define AON_CMU_BTH_RAM_RM_SHIFT                            (3)
#define AON_CMU_BTH_RAM_RA(n)                               (((n) & 0x3) << 7)
#define AON_CMU_BTH_RAM_RA_MASK                             (0x3 << 7)
#define AON_CMU_BTH_RAM_RA_SHIFT                            (7)
#define AON_CMU_BTH_RAM_WA(n)                               (((n) & 0x7) << 9)
#define AON_CMU_BTH_RAM_WA_MASK                             (0x7 << 9)
#define AON_CMU_BTH_RAM_WA_SHIFT                            (9)
#define AON_CMU_BTH_RAM_WPULSE(n)                           (((n) & 0x7) << 12)
#define AON_CMU_BTH_RAM_WPULSE_MASK                         (0x7 << 12)
#define AON_CMU_BTH_RAM_WPULSE_SHIFT                        (12)
#define AON_CMU_BTH_RF_TEST1                                (1 << 15)
#define AON_CMU_BTH_RF_TEST_RNM                             (1 << 16)
#define AON_CMU_BTH_RF_RME                                  (1 << 17)
#define AON_CMU_BTH_RF_RM(n)                                (((n) & 0xF) << 18)
#define AON_CMU_BTH_RF_RM_MASK                              (0xF << 18)
#define AON_CMU_BTH_RF_RM_SHIFT                             (18)
#define AON_CMU_BTH_RF_RA(n)                                (((n) & 0x3) << 22)
#define AON_CMU_BTH_RF_RA_MASK                              (0x3 << 22)
#define AON_CMU_BTH_RF_RA_SHIFT                             (22)
#define AON_CMU_BTH_RF_WA(n)                                (((n) & 0x7) << 24)
#define AON_CMU_BTH_RF_WA_MASK                              (0x7 << 24)
#define AON_CMU_BTH_RF_WA_SHIFT                             (24)
#define AON_CMU_BTH_RF_WPULSE(n)                            (((n) & 0x7) << 27)
#define AON_CMU_BTH_RF_WPULSE_MASK                          (0x7 << 27)
#define AON_CMU_BTH_RF_WPULSE_SHIFT                         (27)

// reg_0d8
#define AON_CMU_VTOR_CORE_BTH(n)                            (((n) & 0x1FFFFFF) << 7)
#define AON_CMU_VTOR_CORE_BTH_MASK                          (0x1FFFFFF << 7)
#define AON_CMU_VTOR_CORE_BTH_SHIFT                         (7)

// reg_0dc
#define AON_CMU_SYS_RAM_BLK0_SEL(n)                         (((n) & 0x7) << 0)
#define AON_CMU_SYS_RAM_BLK0_SEL_MASK                       (0x7 << 0)
#define AON_CMU_SYS_RAM_BLK0_SEL_SHIFT                      (0)
#define AON_CMU_SYS_RAM_BLK1_SEL(n)                         (((n) & 0x7) << 3)
#define AON_CMU_SYS_RAM_BLK1_SEL_MASK                       (0x7 << 3)
#define AON_CMU_SYS_RAM_BLK1_SEL_SHIFT                      (3)
#define AON_CMU_SYS_RAM_BLK2_SEL(n)                         (((n) & 0x7) << 6)
#define AON_CMU_SYS_RAM_BLK2_SEL_MASK                       (0x7 << 6)
#define AON_CMU_SYS_RAM_BLK2_SEL_SHIFT                      (6)
#define AON_CMU_SYS_RAM_BLK3_SEL(n)                         (((n) & 0x7) << 9)
#define AON_CMU_SYS_RAM_BLK3_SEL_MASK                       (0x7 << 9)
#define AON_CMU_SYS_RAM_BLK3_SEL_SHIFT                      (9)
#define AON_CMU_SYS_RAM_BLK4_SEL(n)                         (((n) & 0x7) << 12)
#define AON_CMU_SYS_RAM_BLK4_SEL_MASK                       (0x7 << 12)
#define AON_CMU_SYS_RAM_BLK4_SEL_SHIFT                      (12)
#define AON_CMU_SYS_RAM_BLK5_SEL(n)                         (((n) & 0x7) << 15)
#define AON_CMU_SYS_RAM_BLK5_SEL_MASK                       (0x7 << 15)
#define AON_CMU_SYS_RAM_BLK5_SEL_SHIFT                      (15)
#define AON_CMU_SYS_RAM_BLK6_SEL(n)                         (((n) & 0x7) << 18)
#define AON_CMU_SYS_RAM_BLK6_SEL_MASK                       (0x7 << 18)
#define AON_CMU_SYS_RAM_BLK6_SEL_SHIFT                      (18)
#define AON_CMU_SYS_RAM_BLK7_SEL(n)                         (((n) & 0x7) << 21)
#define AON_CMU_SYS_RAM_BLK7_SEL_MASK                       (0x7 << 21)
#define AON_CMU_SYS_RAM_BLK7_SEL_SHIFT                      (21)
#define AON_CMU_SYS_RAM_BLK8_SEL(n)                         (((n) & 0x7) << 24)
#define AON_CMU_SYS_RAM_BLK8_SEL_MASK                       (0x7 << 24)
#define AON_CMU_SYS_RAM_BLK8_SEL_SHIFT                      (24)
#define AON_CMU_SYS_RAM_BLK9_SEL(n)                         (((n) & 0x7) << 27)
#define AON_CMU_SYS_RAM_BLK9_SEL_MASK                       (0x7 << 27)
#define AON_CMU_SYS_RAM_BLK9_SEL_SHIFT                      (27)

// reg_0e0
#define AON_CMU_SYS_RAM_BLK10_SEL(n)                        (((n) & 0x7) << 0)
#define AON_CMU_SYS_RAM_BLK10_SEL_MASK                      (0x7 << 0)
#define AON_CMU_SYS_RAM_BLK10_SEL_SHIFT                     (0)
#define AON_CMU_SYS_RAM_BLK11_SEL(n)                        (((n) & 0x7) << 3)
#define AON_CMU_SYS_RAM_BLK11_SEL_MASK                      (0x7 << 3)
#define AON_CMU_SYS_RAM_BLK11_SEL_SHIFT                     (3)
#define AON_CMU_SYS_RAM_BLK12_SEL(n)                        (((n) & 0x7) << 6)
#define AON_CMU_SYS_RAM_BLK12_SEL_MASK                      (0x7 << 6)
#define AON_CMU_SYS_RAM_BLK12_SEL_SHIFT                     (6)
#define AON_CMU_SYS_RAM_BLK13_SEL(n)                        (((n) & 0x7) << 9)
#define AON_CMU_SYS_RAM_BLK13_SEL_MASK                      (0x7 << 9)
#define AON_CMU_SYS_RAM_BLK13_SEL_SHIFT                     (9)
#define AON_CMU_SYS_RAM_BLK14_SEL(n)                        (((n) & 0x7) << 12)
#define AON_CMU_SYS_RAM_BLK14_SEL_MASK                      (0x7 << 12)
#define AON_CMU_SYS_RAM_BLK14_SEL_SHIFT                     (12)
#define AON_CMU_SYS_RAM_BLK15_SEL(n)                        (((n) & 0x7) << 15)
#define AON_CMU_SYS_RAM_BLK15_SEL_MASK                      (0x7 << 15)
#define AON_CMU_SYS_RAM_BLK15_SEL_SHIFT                     (15)

// reg_0e4
#define AON_CMU_RAM3_SLP1(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM3_SLP1_MASK                              (0x3FF << 0)
#define AON_CMU_RAM3_SLP1_SHIFT                             (0)
#define AON_CMU_RAM3_DSLP1(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM3_DSLP1_MASK                             (0x3FF << 10)
#define AON_CMU_RAM3_DSLP1_SHIFT                            (10)
#define AON_CMU_RAM3_SD1(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM3_SD1_MASK                               (0x3FF << 20)
#define AON_CMU_RAM3_SD1_SHIFT                              (20)

// reg_0e8
#define AON_CMU_RAM4_SLP0(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM4_SLP0_MASK                              (0x3FF << 0)
#define AON_CMU_RAM4_SLP0_SHIFT                             (0)
#define AON_CMU_RAM4_DSLP0(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM4_DSLP0_MASK                             (0x3FF << 10)
#define AON_CMU_RAM4_DSLP0_SHIFT                            (10)
#define AON_CMU_RAM4_SD0(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM4_SD0_MASK                               (0x3FF << 20)
#define AON_CMU_RAM4_SD0_SHIFT                              (20)

// reg_0ec
#define AON_CMU_RAM4_SLP1(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM4_SLP1_MASK                              (0x3FF << 0)
#define AON_CMU_RAM4_SLP1_SHIFT                             (0)
#define AON_CMU_RAM4_DSLP1(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM4_DSLP1_MASK                             (0x3FF << 10)
#define AON_CMU_RAM4_DSLP1_SHIFT                            (10)
#define AON_CMU_RAM4_SD1(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM4_SD1_MASK                               (0x3FF << 20)
#define AON_CMU_RAM4_SD1_SHIFT                              (20)

// reg_0f0
#define AON_CMU_DEBUG0(n)                                   (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG0_MASK                                 (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG0_SHIFT                                (0)

// reg_0f4
#define AON_CMU_DEBUG1(n)                                   (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG1_MASK                                 (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG1_SHIFT                                (0)

// reg_0f8
#define AON_CMU_DEBUG2(n)                                   (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG2_MASK                                 (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG2_SHIFT                                (0)

// reg_0fc
#define AON_CMU_EFUSE(n)                                    (((n) & 0xFFFF) << 0)
#define AON_CMU_EFUSE_MASK                                  (0xFFFF << 0)
#define AON_CMU_EFUSE_SHIFT                                 (0)
#define AON_CMU_EFUSE_LOCK                                  (1 << 31)

// reg_100
#define AON_CMU_MEMSC0                                      (1 << 0)

// reg_104
#define AON_CMU_MEMSC1                                      (1 << 0)

// reg_108
#define AON_CMU_MEMSC2                                      (1 << 0)

// reg_10c
#define AON_CMU_MEMSC3                                      (1 << 0)

// reg_110
#define AON_CMU_MEMSC4                                      (1 << 0)

// reg_114
#define AON_CMU_MEMSC5                                      (1 << 0)

// reg_118
#define AON_CMU_MEMSC6                                      (1 << 0)

// reg_11c
#define AON_CMU_MEMSC7                                      (1 << 0)

// reg_120
#define AON_CMU_MEMSC8                                      (1 << 0)

// reg_124
#define AON_CMU_MEMSC9                                      (1 << 0)

// reg_128
#define AON_CMU_MEMSC10                                     (1 << 0)

// reg_12c
#define AON_CMU_MEMSC11                                     (1 << 0)

// reg_130
#define AON_CMU_MEMSC12                                     (1 << 0)

// reg_134
#define AON_CMU_MEMSC13                                     (1 << 0)

// reg_138
#define AON_CMU_MEMSC14                                     (1 << 0)

// reg_13c
#define AON_CMU_MEMSC15                                     (1 << 0)

// reg_140
#define AON_CMU_MEMSC_STATUS0                               (1 << 0)
#define AON_CMU_MEMSC_STATUS1                               (1 << 1)
#define AON_CMU_MEMSC_STATUS2                               (1 << 2)
#define AON_CMU_MEMSC_STATUS3                               (1 << 3)
#define AON_CMU_MEMSC_STATUS4                               (1 << 4)
#define AON_CMU_MEMSC_STATUS5                               (1 << 5)
#define AON_CMU_MEMSC_STATUS6                               (1 << 6)
#define AON_CMU_MEMSC_STATUS7                               (1 << 7)
#define AON_CMU_MEMSC_STATUS8                               (1 << 8)
#define AON_CMU_MEMSC_STATUS9                               (1 << 9)
#define AON_CMU_MEMSC_STATUS10                              (1 << 10)
#define AON_CMU_MEMSC_STATUS11                              (1 << 11)
#define AON_CMU_MEMSC_STATUS12                              (1 << 12)
#define AON_CMU_MEMSC_STATUS13                              (1 << 13)
#define AON_CMU_MEMSC_STATUS14                              (1 << 14)
#define AON_CMU_MEMSC_STATUS15                              (1 << 15)

// reg_144
#define AON_CMU_RAM4_POFF0(n)                               (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM4_POFF0_MASK                             (0xFFFFFFFF << 0)
#define AON_CMU_RAM4_POFF0_SHIFT                            (0)

// reg_148
#define AON_CMU_RAM4_POFF1(n)                               (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RAM4_POFF1_MASK                             (0xFFFFFFFF << 0)
#define AON_CMU_RAM4_POFF1_SHIFT                            (0)

// reg_14c
#define AON_CMU_DEBUG3(n)                                   (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG3_MASK                                 (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG3_SHIFT                                (0)

// reg_150
#define AON_CMU_REG_PSRAM_LDO_PU                            (1 << 0)
#define AON_CMU_REG_PSRAM_LDO_PRECHARGE                     (1 << 1)
#define AON_CMU_REG_PSRAM_PU_DVDD                           (1 << 2)
#define AON_CMU_REG_PSRAM_PU                                (1 << 3)
#define AON_CMU_REG_PSRAM_PU2                               (1 << 4)
#define AON_CMU_REG_PSRAM_LDO_RES(n)                        (((n) & 0xF) << 5)
#define AON_CMU_REG_PSRAM_LDO_RES_MASK                      (0xF << 5)
#define AON_CMU_REG_PSRAM_LDO_RES_SHIFT                     (5)
#define AON_CMU_REG_PSRAM_SWRC(n)                           (((n) & 0x3) << 9)
#define AON_CMU_REG_PSRAM_SWRC_MASK                         (0x3 << 9)
#define AON_CMU_REG_PSRAM_SWRC_SHIFT                        (9)
#define AON_CMU_REG_PSRAM_PHY_RSTN_DR                       (1 << 11)
#define AON_CMU_REG_PSRAM_PHY_RSTN                          (1 << 12)
#define AON_CMU_REG_PSRAM_PHY_RSTN2_DR                      (1 << 13)
#define AON_CMU_REG_PSRAM_PHY_RSTN2                         (1 << 14)
#define AON_CMU_REG_PSRAM_ITUNE(n)                          (((n) & 0xF) << 15)
#define AON_CMU_REG_PSRAM_ITUNE_MASK                        (0xF << 15)
#define AON_CMU_REG_PSRAM_ITUNE_SHIFT                       (15)
#define AON_CMU_REG_PSRAM_DLL_CK_RDY                        (1 << 19)
#define AON_CMU_REG_PSRAM_DLL_RESETB                        (1 << 20)
#define AON_CMU_REG_PSRAM_RX_BIAS(n)                        (((n) & 0x3) << 21)
#define AON_CMU_REG_PSRAM_RX_BIAS_MASK                      (0x3 << 21)
#define AON_CMU_REG_PSRAM_RX_BIAS_SHIFT                     (21)
#define AON_CMU_REG_PSRAM_RX_BIAS2(n)                       (((n) & 0x3) << 23)
#define AON_CMU_REG_PSRAM_RX_BIAS2_MASK                     (0x3 << 23)
#define AON_CMU_REG_PSRAM_RX_BIAS2_SHIFT                    (23)
#define AON_CMU_REG_PSRAM_DIE_PU                            (1 << 25)

// reg_154
#define AON_CMU_REG_SPIFLASH1_SCLK_EDGE                     (1 << 0)
#define AON_CMU_SPIFLASH1_ECS                               (1 << 1)

// reg_158
#define AON_CMU_SYS_CEN_MSK_EN                              (1 << 0)
#define AON_CMU_BTH_CEN_MSK_EN                              (1 << 1)
#define AON_CMU_BTC_CEN_MSK_EN                              (1 << 2)
#define AON_CMU_SES_CEN_MSK_EN                              (1 << 3)
#define AON_CMU_DSLP_FORCE_ON_BT                            (1 << 4)

// reg_15c
#define AON_CMU_M55_COREPREQ                                (1 << 0)
#define AON_CMU_M55_COREPSTATE(n)                           (((n) & 0x1F) << 1)
#define AON_CMU_M55_COREPSTATE_MASK                         (0x1F << 1)
#define AON_CMU_M55_COREPSTATE_SHIFT                        (1)
#define AON_CMU_M55_WICCONTROL(n)                           (((n) & 0x3) << 6)
#define AON_CMU_M55_WICCONTROL_MASK                         (0x3 << 6)
#define AON_CMU_M55_WICCONTROL_SHIFT                        (6)

// reg_160
#define AON_CMU_DSP_RUNSTALL                                (1 << 0)
#define AON_CMU_DSP_STATVECTORSEL                           (1 << 1)
#define AON_CMU_SEL_X4_RC_96M                               (1 << 2)

// reg_164
#define AON_CMU_DSP_VTOR(n)                                 (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DSP_VTOR_MASK                               (0xFFFFFFFF << 0)
#define AON_CMU_DSP_VTOR_SHIFT                              (0)

// reg_168
#define AON_CMU_VTOR_CORE_BTC(n)                            (((n) & 0x1FFFFFF) << 7)
#define AON_CMU_VTOR_CORE_BTC_MASK                          (0x1FFFFFF << 7)
#define AON_CMU_VTOR_CORE_BTC_SHIFT                         (7)

// reg_16c
#define AON_CMU_RAM7_SLP0(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM7_SLP0_MASK                              (0x3FF << 0)
#define AON_CMU_RAM7_SLP0_SHIFT                             (0)
#define AON_CMU_RAM7_DSLP0(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM7_DSLP0_MASK                             (0x3FF << 10)
#define AON_CMU_RAM7_DSLP0_SHIFT                            (10)
#define AON_CMU_RAM7_SD0(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM7_SD0_MASK                               (0x3FF << 20)
#define AON_CMU_RAM7_SD0_SHIFT                              (20)

// reg_170
#define AON_CMU_RAM7_SLP1(n)                                (((n) & 0x3FF) << 0)
#define AON_CMU_RAM7_SLP1_MASK                              (0x3FF << 0)
#define AON_CMU_RAM7_SLP1_SHIFT                             (0)
#define AON_CMU_RAM7_DSLP1(n)                               (((n) & 0x3FF) << 10)
#define AON_CMU_RAM7_DSLP1_MASK                             (0x3FF << 10)
#define AON_CMU_RAM7_DSLP1_SHIFT                            (10)
#define AON_CMU_RAM7_SD1(n)                                 (((n) & 0x3FF) << 20)
#define AON_CMU_RAM7_SD1_MASK                               (0x3FF << 20)
#define AON_CMU_RAM7_SD1_SHIFT                              (20)

// reg_174
#define AON_CMU_BTC_LAST_PC_ADDR(n)                         (((n) & 0xFF) << 0)
#define AON_CMU_BTC_LAST_PC_ADDR_MASK                       (0xFF << 0)
#define AON_CMU_BTC_LAST_PC_ADDR_SHIFT                      (0)
#define AON_CMU_BTC_LAST_PC_EN                              (1 << 8)
#define AON_CMU_BTH_LAST_PC_ADDR(n)                         (((n) & 0xFF) << 9)
#define AON_CMU_BTH_LAST_PC_ADDR_MASK                       (0xFF << 9)
#define AON_CMU_BTH_LAST_PC_ADDR_SHIFT                      (9)
#define AON_CMU_BTH_LAST_PC_EN                              (1 << 17)
#define AON_CMU_SENS_LAST_PC_ADDR(n)                        (((n) & 0xFF) << 18)
#define AON_CMU_SENS_LAST_PC_ADDR_MASK                      (0xFF << 18)
#define AON_CMU_SENS_LAST_PC_ADDR_SHIFT                     (18)
#define AON_CMU_SENS_LAST_PC_EN                             (1 << 26)

// reg_178
#define AON_CMU_EN_CLK_EFUSE                                (1 << 0)
#define AON_CMU_SEL_OSC_EFUSE                               (1 << 1)
#define AON_CMU_CFG_DIV_EFUSE(n)                            (((n) & 0xFFF) << 2)
#define AON_CMU_CFG_DIV_EFUSE_MASK                          (0xFFF << 2)
#define AON_CMU_CFG_DIV_EFUSE_SHIFT                         (2)
#define AON_CMU_BYPASS_DIV_EFUSE                            (1 << 14)
#define AON_CMU_SEL_BES_GPIO                                (1 << 15)

// reg_17c
#define AON_CMU_RAM8_SLP0(n)                                (((n) & 0x1F) << 0)
#define AON_CMU_RAM8_SLP0_MASK                              (0x3FF << 0)
#define AON_CMU_RAM8_SLP0_SHIFT                             (0)
#define AON_CMU_RAM8_DSLP0(n)                               (((n) & 0x1F) << 10)
#define AON_CMU_RAM8_DSLP0_MASK                             (0x3FF << 10)
#define AON_CMU_RAM8_DSLP0_SHIFT                            (10)
#define AON_CMU_RAM8_SD0(n)                                 (((n) & 0x1F) << 20)
#define AON_CMU_RAM8_SD0_MASK                               (0x3FF << 20)
#define AON_CMU_RAM8_SD0_SHIFT                              (20)

// reg_180
#define AON_CMU_RAM8_SLP1(n)                                (((n) & 0x1F) << 0)
#define AON_CMU_RAM8_SLP1_MASK                              (0x3FF << 0)
#define AON_CMU_RAM8_SLP1_SHIFT                             (0)
#define AON_CMU_RAM8_DSLP1(n)                               (((n) & 0x1F) << 10)
#define AON_CMU_RAM8_DSLP1_MASK                             (0x3FF << 10)
#define AON_CMU_RAM8_DSLP1_SHIFT                            (10)
#define AON_CMU_RAM8_SD1(n)                                 (((n) & 0x1F) << 20)
#define AON_CMU_RAM8_SD1_MASK                               (0x3FF << 20)
#define AON_CMU_RAM8_SD1_SHIFT                              (20)

// reg_184
#define AON_CMU_PVT_RO_EN                                   (1 << 0)
#define AON_CMU_PVT_COUNT_START_EN                          (1 << 1)
#define AON_CMU_PVT_OSC_CLK_EN                              (1 << 2)
#define AON_CMU_PVT_RSTN                                    (1 << 3)
#define AON_CMU_PVT_REG_TIMESET(n)                          (((n) & 0x3FF) << 4)
#define AON_CMU_PVT_REG_TIMESET_MASK                        (0x3FF << 4)
#define AON_CMU_PVT_REG_TIMESET_SHIFT                       (4)

// reg_188
#define AON_CMU_PVT_COUNT_VALUE(n)                          (((n) & 0x3FFFF) << 0)
#define AON_CMU_PVT_COUNT_VALUE_MASK                        (0x3FFFF << 0)
#define AON_CMU_PVT_COUNT_VALUE_SHIFT                       (0)
#define AON_CMU_PVT_COUNT_DONE                              (1 << 18)

// reg_18c
#define AON_CMU_SYS_LAST_PC_ADDR(n)                         (((n) & 0xFF) << 0)
#define AON_CMU_SYS_LAST_PC_ADDR_MASK                       (0xFF << 0)
#define AON_CMU_SYS_LAST_PC_ADDR_SHIFT                      (0)
#define AON_CMU_SYS_LAST_PC_EN                              (1 << 8)

// APB and AHB Clocks:
#define AON_ACLK_CMU                                        (1 << 0)
#define AON_ARST_CMU                                        (1 << 0)
#define AON_ACLK_GPIO0                                      (1 << 1)
#define AON_ARST_GPIO0                                      (1 << 1)
#define AON_ACLK_GPIO0_INT                                  (1 << 2)
#define AON_ARST_GPIO0_INT                                  (1 << 2)
#define AON_ACLK_WDT                                        (1 << 3)
#define AON_ARST_WDT                                        (1 << 3)
#define AON_ACLK_TIMER0                                     (1 << 4)
#define AON_ARST_TIMER0                                     (1 << 4)
#define AON_ACLK_PSC                                        (1 << 5)
#define AON_ARST_PSC                                        (1 << 5)
#define AON_ACLK_IOMUX                                      (1 << 6)
#define AON_ARST_IOMUX                                      (1 << 6)
#define AON_ACLK_APBC                                       (1 << 7)
#define AON_ARST_APBC                                       (1 << 7)
#define AON_ACLK_H2H_MCU                                    (1 << 8)
#define AON_ARST_H2H_MCU                                    (1 << 8)
#define AON_ACLK_I2C_SLV                                    (1 << 9)
#define AON_ARST_I2C_SLV                                    (1 << 9)
#define AON_ACLK_TZC                                        (1 << 10)
#define AON_ARST_TZC                                        (1 << 10)
#define AON_ACLK_TIMER1                                     (1 << 11)
#define AON_ARST_TIMER1                                     (1 << 11)
#define AON_ACLK_H2H_BTC                                    (1 << 12)
#define AON_ARST_H2H_BTC                                    (1 << 12)
#define AON_ACLK_SYS_GPIO0                                  (1 << 13)
#define AON_ARST_SYS_GPIO0                                  (1 << 13)
#define AON_ACLK_SYS_GPIO0_INT                              (1 << 14)
#define AON_ARST_SYS_GPIO0_INT                              (1 << 14)
#define AON_ACLK_SYS_GPIO1                                  (1 << 15)
#define AON_ARST_SYS_GPIO1                                  (1 << 15)
#define AON_ACLK_SYS_GPIO1_INT                              (1 << 16)
#define AON_ARST_SYS_GPIO1_INT                              (1 << 16)
#define AON_ACLK_SYS_IOMUX                                  (1 << 17)
#define AON_ARST_SYS_IOMUX                                  (1 << 17)
#define AON_ACLK_EFUSE                                      (1 << 18)
#define AON_ARST_EFUSE                                      (1 << 18)

// AON other Clocks:
#define AON_OCLK_WDT                                        (1 << 0)
#define AON_ORST_WDT                                        (1 << 0)
#define AON_OCLK_TIMER0                                     (1 << 1)
#define AON_ORST_TIMER0                                     (1 << 1)
#define AON_OCLK_GPIO0                                      (1 << 2)
#define AON_ORST_GPIO0                                      (1 << 2)
#define AON_OCLK_IOMUX                                      (1 << 3)
#define AON_ORST_IOMUX                                      (1 << 3)
#define AON_OCLK_BTAON                                      (1 << 4)
#define AON_ORST_BTAON                                      (1 << 4)
#define AON_OCLK_PSC                                        (1 << 5)
#define AON_ORST_PSC                                        (1 << 5)
#define AON_OCLK_TIMER1                                     (1 << 6)
#define AON_ORST_TIMER1                                     (1 << 6)
#define AON_OCLK_SYS_GPIO0                                  (1 << 7)
#define AON_ORST_SYS_GPIO0                                  (1 << 7)
#define AON_OCLK_SYS_GPIO1                                  (1 << 8)
#define AON_ORST_SYS_GPIO1                                  (1 << 8)
#define AON_OCLK_SYS_IOMUX                                  (1 << 9)
#define AON_ORST_SYS_IOMUX                                  (1 << 9)
#define AON_OCLK_EFUSE                                      (1 << 10)
#define AON_ORST_EFUSE                                      (1 << 10)
#define AON_OCLK_PMU                                        (1 << 11)
#define AON_ORST_PMU                                        (1 << 11)
#define AON_OCLK_USB                                        (1 << 12)
#define AON_ORST_USB                                        (1 << 12)
#endif

