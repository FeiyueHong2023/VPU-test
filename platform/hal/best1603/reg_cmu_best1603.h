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
#ifndef __REG_CMU_BEST1603_H__
#define __REG_CMU_BEST1603_H__

#include "plat_types.h"

struct CMU_T {
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
    __IO uint32_t ADMA_CH15_REQ;    // 0x68
    __IO uint32_t REG_RAM_CFG1;     // 0x6C
    __IO uint32_t UART_CLK;         // 0x70
    __IO uint32_t I2C_CLK;          // 0x74
    __IO uint32_t SYS2SENS_MASK0;   // 0x78
    __IO uint32_t SYS2SENS_MASK1;   // 0x7C
    __IO uint32_t WRITE_UNLOCK;     // 0x80
    __IO uint32_t WAKEUP_MASK0;     // 0x84
    __IO uint32_t WAKEUP_MASK1;     // 0x88
    __IO uint32_t WAKEUP_CLK_CFG;   // 0x8C
    __IO uint32_t RESERVED_090;     // 0x90
    __IO uint32_t RESERVED_094;     // 0x94
    __IO uint32_t DSP2M55_IRQ_SET;  // 0x98
    __IO uint32_t DSP2M55_IRQ_CLR;  // 0x9C
    __IO uint32_t SYS2BTH_IRQ_SET;  // 0xA0
    __IO uint32_t SYS2BTH_IRQ_CLR;  // 0xA4
    __IO uint32_t SYS_DIV;          // 0xA8
    __IO uint32_t RESERVED_0AC;     // 0xAC
    __IO uint32_t SYS2BTH_INTMASK0; // 0xB0
    __IO uint32_t SYS2BTH_INTMASK1; // 0xB4
    __IO uint32_t M552DSP_IRQ_SET;  // 0xB8
    __IO uint32_t M552DSP_IRQ_CLR;  // 0xBC
    __IO uint32_t MEMSC[4];         // 0xC0
    __I  uint32_t MEMSC_STATUS;     // 0xD0
    __IO uint32_t ADMA_CH0_4_REQ;   // 0xD4
    __IO uint32_t ADMA_CH5_9_REQ;   // 0xD8
    __IO uint32_t ADMA_CH10_14_REQ; // 0xDC
    __IO uint32_t GDMA_CH0_4_REQ;   // 0xE0
    __IO uint32_t GDMA_CH5_9_REQ;   // 0xE4
    __IO uint32_t GDMA_CH10_14_REQ; // 0xE8
    __IO uint32_t GDMA_CH15_REQ;    // 0xEC
    __IO uint32_t MISC;             // 0xF0
    __IO uint32_t SIMU_RES;         // 0xF4
    __IO uint32_t MISC_0F8;         // 0xF8
    __IO uint32_t SYS2OTH_MASK2;    // 0xFC
    __IO uint32_t DSI_CLK_ENABLE;   // 0x100
    __IO uint32_t DSI_CLK_DISABLE;  // 0x104
    __IO uint32_t RESERVED_108;     // 0x108
    __IO uint32_t I2S0_CLK;         // 0x10C
    __IO uint32_t I2S1_CLK;         // 0x110
    __IO uint32_t REG_RAM_CFG2;     // 0x114
    __IO uint32_t M55_TCM_CFG;      // 0x118
    __IO uint32_t RESERVED_11C;     // 0x11C
    __IO uint32_t PWM01_CLK;        // 0x120
    __IO uint32_t PWM23_CLK;        // 0x124
    __IO uint32_t SYS2SENS_IRQ_SET; // 0x128
    __IO uint32_t SYS2SENS_IRQ_CLR; // 0x12C
    __IO uint32_t RESERVED_130;     // 0x130
    __IO uint32_t M55_CODEC_OBS0;   // 0x134
    __IO uint32_t M55_GPIO_OBS0;    // 0x138
    __IO uint32_t LCDC_CLK;         // 0x13C
    __IO uint32_t ISIRQ_SET;        // 0x140
    __IO uint32_t ISIRQ_CLR;        // 0x144
    __IO uint32_t SYS2BTC_INTMASK0; // 0x148
    __IO uint32_t SYS2BTC_INTMASK1; // 0x14C
    __IO uint32_t AXI_MANUAL_CLK;   // 0x150
    __IO uint32_t DSP_WAKEUP_MASK0; // 0x154
    __IO uint32_t DSP_WAKEUP_MASK1; // 0x158
    __IO uint32_t DSP_WAKEUP_MASK2; // 0x15C
    __IO uint32_t QCLK_ENABLE;      // 0x160
    __IO uint32_t QCLK_DISABLE;     // 0x164
    __IO uint32_t QCLK_MODE;        // 0x168
    __IO uint32_t QRESET_PULSE;     // 0x16C
    __IO uint32_t QRESET_SET;       // 0x170
    __IO uint32_t QRESET_CLR;       // 0x174
    __IO uint32_t XCLK_ENABLE;      // 0x178
    __IO uint32_t XCLK_DISABLE;     // 0x17C
    __IO uint32_t XCLK_MODE;        // 0x180
    __IO uint32_t XRESET_PULSE;     // 0x184
    __IO uint32_t XRESET_SET;       // 0x188
    __IO uint32_t XRESET_CLR;       // 0x18C
    __IO uint32_t RF2P_CFG;         // 0x190
    __IO uint32_t CPU_IDLE_MSK;     // 0x194
    __IO uint32_t M55_COREPACCEPT;  // 0x198
    __IO uint32_t CFG_LP_ENABLE;    // 0x19C
    __IO uint32_t CFG_FORCE_LP;     // 0x1A0
    __IO uint32_t CFG_BYPASS_CLKEN; // 0x1A4
    __IO uint32_t CFG_LP_NUMBER;    // 0x1A8
    __IO uint32_t FORCE_ACK;        // 0x1AC
    __IO uint32_t RD_TIMER_TRIG;    // 0x1B0
    __IO uint32_t RD_TIMER_TRIG_I2S;// 0x1B4
    __IO uint32_t PWM45_CLK;        // 0x1B8
    __IO uint32_t PWM67_CLK;        // 0x1BC
    __IO uint32_t RD_TIMER_TRIG_GPIO0;// 0x1C0
    __IO uint32_t RD_TIMER_TRIG_GPIO1;// 0x1C4
    __IO uint32_t CFG_CG_DSP;       // 0x1C8
    __IO uint32_t PVT_CFG;          // 0x1CC
    __IO uint32_t PVT_COUNT;        // 0x1D0
};

struct SAVED_CMU_REGS_T {
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
    uint32_t ADMA_CH15_REQ;
    uint32_t REG_RAM_CFG1;
    uint32_t UART_CLK;
    uint32_t I2C_CLK;
    uint32_t SYS2SENS_MASK0;
    uint32_t SYS2SENS_MASK1;
    uint32_t WAKEUP_CLK_CFG;
    uint32_t SYS_DIV;
    uint32_t SYS2BTH_INTMASK0;
    uint32_t SYS2BTH_INTMASK1;
    uint32_t ADMA_CH0_4_REQ;
    uint32_t ADMA_CH5_9_REQ;
    uint32_t ADMA_CH10_14_REQ;
    uint32_t GDMA_CH0_4_REQ;
    uint32_t GDMA_CH5_9_REQ;
    uint32_t GDMA_CH10_14_REQ;
    uint32_t GDMA_CH15_REQ;
    uint32_t MISC;
    uint32_t SIMU_RES;
    uint32_t MISC_0F8;
    uint32_t SYS2OTH_MASK2;
    uint32_t DSI_CLK_ENABLE;
    uint32_t I2S0_CLK;
    uint32_t I2S1_CLK;
    uint32_t REG_RAM_CFG2;
    uint32_t M55_TCM_CFG;
    uint32_t PWM01_CLK;
    uint32_t PWM23_CLK;
    uint32_t M55_CODEC_OBS0;
    uint32_t M55_GPIO_OBS0;
    uint32_t LCDC_CLK;
    uint32_t SYS2BTC_INTMASK0;
    uint32_t SYS2BTC_INTMASK1;
    uint32_t AXI_MANUAL_CLK;
    uint32_t DSP_WAKEUP_MASK0;
    uint32_t DSP_WAKEUP_MASK1;
    uint32_t DSP_WAKEUP_MASK2;
    uint32_t QCLK_ENABLE;
    uint32_t QCLK_MODE;
    uint32_t QRESET_CLR;
    uint32_t XCLK_ENABLE;
    uint32_t XCLK_MODE;
    uint32_t XRESET_CLR;
    uint32_t RF2P_CFG;
    uint32_t CPU_IDLE_MSK;
    uint32_t M55_COREPACCEPT;
    uint32_t CFG_LP_ENABLE;
    uint32_t CFG_FORCE_LP;
    uint32_t CFG_BYPASS_CLKEN;
    uint32_t CFG_LP_NUMBER;
    uint32_t FORCE_ACK;
    uint32_t RD_TIMER_TRIG;
    uint32_t RD_TIMER_TRIG_I2S;
    uint32_t PWM45_CLK;
    uint32_t PWM67_CLK;
    uint32_t RD_TIMER_TRIG_GPIO0;
    uint32_t RD_TIMER_TRIG_GPIO1;
    uint32_t CFG_CG_DSP;
    uint32_t PVT_CFG;
    uint32_t PVT_COUNT;
};

#if !defined(CMU_HDR_CHECK_SUBSYS) || !defined(CHIP_SUBSYS_BTH)
#define MCU2SENS_IRQ_SET                                    SYS2SENS_IRQ_SET
#define MCU2SENS_IRQ_CLR                                    SYS2SENS_IRQ_CLR

#define CMU_SENS2MCU_DATA_DONE_SET                          CMU_SES2SYS_DATA_DONE_SET
#define CMU_SENS2MCU_DATA1_DONE_SET                         CMU_SES2SYS_DATA1_DONE_SET
#define CMU_SENS2MCU_DATA2_DONE_SET                         CMU_SES2SYS_DATA2_DONE_SET
#define CMU_SENS2MCU_DATA3_DONE_SET                         CMU_SES2SYS_DATA3_DONE_SET
#define CMU_MCU2SENS_DATA_IND_SET                           CMU_SYS2SES_DATA_IND_SET
#define CMU_MCU2SENS_DATA1_IND_SET                          CMU_SYS2SES_DATA1_IND_SET
#define CMU_MCU2SENS_DATA2_IND_SET                          CMU_SYS2SES_DATA2_IND_SET
#define CMU_MCU2SENS_DATA3_IND_SET                          CMU_SYS2SES_DATA3_IND_SET
#define CMU_SENS2MCU_DATA_MSK_SET                           CMU_SES2SYS_DATA_MSK_SET
#define CMU_SENS2MCU_DATA1_MSK_SET                          CMU_SES2SYS_DATA1_MSK_SET
#define CMU_SENS2MCU_DATA2_MSK_SET                          CMU_SES2SYS_DATA2_MSK_SET
#define CMU_SENS2MCU_DATA3_MSK_SET                          CMU_SES2SYS_DATA3_MSK_SET
#define CMU_MCU2SENS_DATA_MSK_SET                           CMU_SYS2SES_DATA_MSK_SET
#define CMU_MCU2SENS_DATA1_MSK_SET                          CMU_SYS2SES_DATA1_MSK_SET
#define CMU_MCU2SENS_DATA2_MSK_SET                          CMU_SYS2SES_DATA2_MSK_SET
#define CMU_MCU2SENS_DATA3_MSK_SET                          CMU_SYS2SES_DATA3_MSK_SET
#define CMU_SENS2MCU_DATA_INTR                              CMU_SES2SYS_DATA_INTR
#define CMU_SENS2MCU_DATA1_INTR                             CMU_SES2SYS_DATA1_INTR
#define CMU_SENS2MCU_DATA2_INTR                             CMU_SES2SYS_DATA2_INTR
#define CMU_SENS2MCU_DATA3_INTR                             CMU_SES2SYS_DATA3_INTR
#define CMU_MCU2SENS_DATA_INTR                              CMU_SYS2SES_DATA_INTR
#define CMU_MCU2SENS_DATA1_INTR                             CMU_SYS2SES_DATA1_INTR
#define CMU_MCU2SENS_DATA2_INTR                             CMU_SYS2SES_DATA2_INTR
#define CMU_MCU2SENS_DATA3_INTR                             CMU_SYS2SES_DATA3_INTR
#define CMU_SENS2MCU_DATA_INTR_MSK                          CMU_SES2SYS_DATA_INTR_MSK
#define CMU_SENS2MCU_DATA1_INTR_MSK                         CMU_SES2SYS_DATA1_INTR_MSK
#define CMU_SENS2MCU_DATA2_INTR_MSK                         CMU_SES2SYS_DATA2_INTR_MSK
#define CMU_SENS2MCU_DATA3_INTR_MSK                         CMU_SES2SYS_DATA3_INTR_MSK
#define CMU_MCU2SENS_DATA_INTR_MSK                          CMU_SYS2SES_DATA_INTR_MSK
#define CMU_MCU2SENS_DATA1_INTR_MSK                         CMU_SYS2SES_DATA1_INTR_MSK
#define CMU_MCU2SENS_DATA2_INTR_MSK                         CMU_SYS2SES_DATA2_INTR_MSK
#define CMU_MCU2SENS_DATA3_INTR_MSK                         CMU_SYS2SES_DATA3_INTR_MSK

#define CMU_SENS2MCU_DATA_DONE_CLR                          CMU_SES2SYS_DATA_DONE_CLR
#define CMU_SENS2MCU_DATA1_DONE_CLR                         CMU_SES2SYS_DATA1_DONE_CLR
#define CMU_SENS2MCU_DATA2_DONE_CLR                         CMU_SES2SYS_DATA2_DONE_CLR
#define CMU_SENS2MCU_DATA3_DONE_CLR                         CMU_SES2SYS_DATA3_DONE_CLR
#define CMU_MCU2SENS_DATA_IND_CLR                           CMU_SYS2SES_DATA_IND_CLR
#define CMU_MCU2SENS_DATA1_IND_CLR                          CMU_SYS2SES_DATA1_IND_CLR
#define CMU_MCU2SENS_DATA2_IND_CLR                          CMU_SYS2SES_DATA2_IND_CLR
#define CMU_MCU2SENS_DATA3_IND_CLR                          CMU_SYS2SES_DATA3_IND_CLR
#define CMU_SENS2MCU_DATA_MSK_CLR                           CMU_SES2SYS_DATA_MSK_CLR
#define CMU_SENS2MCU_DATA1_MSK_CLR                          CMU_SES2SYS_DATA1_MSK_CLR
#define CMU_SENS2MCU_DATA2_MSK_CLR                          CMU_SES2SYS_DATA2_MSK_CLR
#define CMU_SENS2MCU_DATA3_MSK_CLR                          CMU_SES2SYS_DATA3_MSK_CLR
#define CMU_MCU2SENS_DATA_MSK_CLR                           CMU_SYS2SES_DATA_MSK_CLR
#define CMU_MCU2SENS_DATA1_MSK_CLR                          CMU_SYS2SES_DATA1_MSK_CLR
#define CMU_MCU2SENS_DATA2_MSK_CLR                          CMU_SYS2SES_DATA2_MSK_CLR
#define CMU_MCU2SENS_DATA3_MSK_CLR                          CMU_SYS2SES_DATA3_MSK_CLR
#endif

// reg_000
#define CMU_MANUAL_HCLK_ENABLE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_ENABLE_MASK                         (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_ENABLE_SHIFT                        (0)

// reg_004
#define CMU_MANUAL_HCLK_DISABLE(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_DISABLE_MASK                        (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_DISABLE_SHIFT                       (0)

// reg_008
#define CMU_MANUAL_PCLK_ENABLE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_ENABLE_MASK                         (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_ENABLE_SHIFT                        (0)

// reg_00c
#define CMU_MANUAL_PCLK_DISABLE(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_DISABLE_MASK                        (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_DISABLE_SHIFT                       (0)

// reg_010
#define CMU_MANUAL_OCLK_ENABLE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_ENABLE_MASK                         (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_ENABLE_SHIFT                        (0)

// reg_014
#define CMU_MANUAL_OCLK_DISABLE(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_DISABLE_MASK                        (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_DISABLE_SHIFT                       (0)

// reg_018
#define CMU_MODE_HCLK(n)                                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_HCLK_MASK                                  (0xFFFFFFFF << 0)
#define CMU_MODE_HCLK_SHIFT                                 (0)

// reg_01c
#define CMU_MODE_PCLK(n)                                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_PCLK_MASK                                  (0xFFFFFFFF << 0)
#define CMU_MODE_PCLK_SHIFT                                 (0)

// reg_020
#define CMU_MODE_OCLK(n)                                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_OCLK_MASK                                  (0xFFFFFFFF << 0)
#define CMU_MODE_OCLK_SHIFT                                 (0)

// reg_024
#define CMU_RF_SLP0(n)                                      (((n) & 0x1F) << 0)
#define CMU_RF_SLP0_MASK                                    (0x1F << 0)
#define CMU_RF_SLP0_SHIFT                                   (0)
#define CMU_RF_DSLP0(n)                                     (((n) & 0x1F) << 5)
#define CMU_RF_DSLP0_MASK                                   (0x1F << 5)
#define CMU_RF_DSLP0_SHIFT                                  (5)
#define CMU_RF_SD0(n)                                       (((n) & 0x1F) << 10)
#define CMU_RF_SD0_MASK                                     (0x1F << 10)
#define CMU_RF_SD0_SHIFT                                    (10)
#define CMU_RF_SLP1(n)                                      (((n) & 0x1F) << 15)
#define CMU_RF_SLP1_MASK                                    (0x1F << 15)
#define CMU_RF_SLP1_SHIFT                                   (15)
#define CMU_RF_DSLP1(n)                                     (((n) & 0x1F) << 20)
#define CMU_RF_DSLP1_MASK                                   (0x1F << 20)
#define CMU_RF_DSLP1_SHIFT                                  (20)
#define CMU_RF_SD1(n)                                       (((n) & 0x1F) << 25)
#define CMU_RF_SD1_MASK                                     (0x1F << 25)
#define CMU_RF_SD1_SHIFT                                    (25)

// reg_028
#define CMU_HRESETN_PULSE(n)                                (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_PULSE_MASK                              (0xFFFFFFFF << 0)
#define CMU_HRESETN_PULSE_SHIFT                             (0)

#define SYS_PRST_NUM                                        29

// reg_02c
#define CMU_PRESETN_PULSE(n)                                (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_PULSE_MASK                              (0xFFFFFFFF << 0)
#define CMU_PRESETN_PULSE_SHIFT                             (0)
#define CMU_GLOBAL_RESETN_PULSE                             (1 << (SYS_PRST_NUM+1-1))

// reg_030
#define CMU_ORESETN_PULSE(n)                                (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_PULSE_MASK                              (0xFFFFFFFF << 0)
#define CMU_ORESETN_PULSE_SHIFT                             (0)

// reg_034
#define CMU_HRESETN_SET(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_SET_MASK                                (0xFFFFFFFF << 0)
#define CMU_HRESETN_SET_SHIFT                               (0)

// reg_038
#define CMU_HRESETN_CLR(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_CLR_MASK                                (0xFFFFFFFF << 0)
#define CMU_HRESETN_CLR_SHIFT                               (0)

// reg_03c
#define CMU_PRESETN_SET(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_SET_MASK                                (0xFFFFFFFF << 0)
#define CMU_PRESETN_SET_SHIFT                               (0)
#define CMU_GLOBAL_RESETN_SET                               (1 << (SYS_PRST_NUM+1-1))

// reg_040
#define CMU_PRESETN_CLR(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_CLR_MASK                                (0xFFFFFFFF << 0)
#define CMU_PRESETN_CLR_SHIFT                               (0)
#define CMU_GLOBAL_RESETN_CLR                               (1 << (SYS_PRST_NUM+1-1))

// reg_044
#define CMU_ORESETN_SET(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_SET_MASK                                (0xFFFFFFFF << 0)
#define CMU_ORESETN_SET_SHIFT                               (0)

// reg_048
#define CMU_ORESETN_CLR(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_CLR_MASK                                (0xFFFFFFFF << 0)
#define CMU_ORESETN_CLR_SHIFT                               (0)

// reg_050
#define CMU_WATCHDOG_RESET                                  (1 << 0)
#define CMU_SOFT_GLOBLE_RESET                               (1 << 1)
#define CMU_RTC_INTR_H                                      (1 << 2)
#define CMU_CHG_INTR_H                                      (1 << 3)
#define CMU_SOFT_BOOT_MODE(n)                               (((n) & 0xFFFFFFF) << 4)
#define CMU_SOFT_BOOT_MODE_MASK                             (0xFFFFFFF << 4)
#define CMU_SOFT_BOOT_MODE_SHIFT                            (4)

// reg_054
#define CMU_CFG_HCLK_MCU_OFF_TIMER(n)                       (((n) & 0xFF) << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_MASK                     (0xFF << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_SHIFT                    (0)
#define CMU_HCLK_MCU_ENABLE                                 (1 << 8)
#define CMU_RAM_RETN_UP_EARLY                               (1 << 9)
#define CMU_FLS_SEC_MSK_EN                                  (1 << 10)
#define CMU_DEBUG_REG_SEL(n)                                (((n) & 0x7) << 11)
#define CMU_DEBUG_REG_SEL_MASK                              (0x7 << 11)
#define CMU_DEBUG_REG_SEL_SHIFT                             (11)

// reg_058
#define CMU_SLEEP_TIMER(n)                                  (((n) & 0xFFFFFF) << 0)
#define CMU_SLEEP_TIMER_MASK                                (0xFFFFFF << 0)
#define CMU_SLEEP_TIMER_SHIFT                               (0)
#define CMU_SLEEP_TIMER_EN                                  (1 << 24)
#define CMU_DEEPSLEEP_EN                                    (1 << 25)
#define CMU_DEEPSLEEP_ROMRAM_EN                             (1 << 26)
#define CMU_MANUAL_RAM_RETN                                 (1 << 27)
#define CMU_DEEPSLEEP_START                                 (1 << 28)
#define CMU_DEEPSLEEP_MODE                                  (1 << 29)
#define CMU_PU_OSC                                          (1 << 30)
#define CMU_WAKEUP_DEEPSLEEP_L                              (1 << 31)

// reg_05c
#define CMU_CFG_DIV_SDMMC(n)                                (((n) & 0x3) << 0)
#define CMU_CFG_DIV_SDMMC_MASK                              (0x3 << 0)
#define CMU_CFG_DIV_SDMMC_SHIFT                             (0)
#define CMU_SEL_OSCX2_SDMMC                                 (1 << 2)
#define CMU_SEL_PLL_SDMMC                                   (1 << 3)
#define CMU_EN_PLL_SDMMC                                    (1 << 4)
#define CMU_SEL_32K_TIMER(n)                                (((n) & 0x7) << 5)
#define CMU_SEL_32K_TIMER_MASK                              (0x7 << 5)
#define CMU_SEL_32K_TIMER_SHIFT                             (5)
#define CMU_SEL_32K_WDT                                     (1 << 8)
#define CMU_SEL_TIMER_FAST(n)                               (((n) & 0x7) << 9)
#define CMU_SEL_TIMER_FAST_MASK                             (0x7 << 9)
#define CMU_SEL_TIMER_FAST_SHIFT                            (9)
#define CMU_CFG_CLK_OUT(n)                                  (((n) & 0x1F) << 12)
#define CMU_CFG_CLK_OUT_MASK                                (0x1F << 12)
#define CMU_CFG_CLK_OUT_SHIFT                               (12)
#define CMU_JTAG_SEL_CP                                     (1 << 17)
#define CMU_XDMA_ID_TYPE                                    (1 << 18)
#define CMU_XDMA_SRAM_CLK_ON                                (1 << 19)
#define CMU_SRC_PLL_EMMC(n)                                 (((n) & 0x7) << 20)
#define CMU_SRC_PLL_EMMC_MASK                               (0x7 << 20)
#define CMU_SRC_PLL_EMMC_SHIFT                              (20)
#define CMU_SEL_OSCX2_EMMC                                  (1 << 23)
#define CMU_SEL_PLL_EMMC                                    (1 << 24)
#define CMU_EN_PLL_EMMC                                     (1 << 25)
#define CMU_BT_PLAYTIME_STAMP_MASK                          (1 << 26)
#define CMU_BT_PLAYTIME_STAMP1_MASK                         (1 << 27)
#define CMU_BT_PLAYTIME_STAMP2_MASK                         (1 << 28)
#define CMU_BT_PLAYTIME_STAMP3_MASK                         (1 << 29)
#define CMU_CFG_TIMER_FAST(n)                               (((n) & 0x3) << 30)
#define CMU_CFG_TIMER_FAST_MASK                             (0x3 << 30)
#define CMU_CFG_TIMER_FAST_SHIFT                            (30)

// reg_060
#define CMU_RSTN_DIV_FLS0_ENABLE                            (1 << 0)
#define CMU_SEL_OSCX4_FLS0_ENABLE                           (1 << 1)
#define CMU_SEL_OSCX2_FLS0_ENABLE                           (1 << 2)
#define CMU_SEL_PLL_FLS0_ENABLE                             (1 << 3)
#define CMU_BYPASS_DIV_FLS0_ENABLE                          (1 << 4)
#define CMU_RSTN_DIV_SYS_ENABLE                             (1 << 5)
#define CMU_SEL_OSC_2_SYS_ENABLE                            (1 << 6)
#define CMU_SEL_OSC_4_SYS_ENABLE                            (1 << 7)
#define CMU_SEL_SLOW_SYS_ENABLE                             (1 << 8)
#define CMU_SEL_OSCX2_SYS_ENABLE                            (1 << 9)
#define CMU_SEL_FAST_SYS_ENABLE                             (1 << 10)
#define CMU_SEL_PLL_SYS_ENABLE                              (1 << 11)
#define CMU_BYPASS_DIV_SYS_ENABLE                           (1 << 12)
#define CMU_EN_OSCX2_ENABLE                                 (1 << 13)
#define CMU_PU_OSCX2_ENABLE                                 (1 << 14)
#define CMU_EN_OSCX4_ENABLE                                 (1 << 15)
#define CMU_PU_OSCX4_ENABLE                                 (1 << 16)
#define CMU_EN_PLL_ENABLE                                   (1 << 17)
#define CMU_PU_PLL_ENABLE                                   (1 << 18)
#define CMU_RSTN_DIV_PSRAM_ENABLE                           (1 << 19)
#define CMU_SEL_OSCX4_PSRAM_ENABLE                          (1 << 20)
#define CMU_SEL_OSCX2_PSRAM_ENABLE                          (1 << 21)
#define CMU_SEL_PLL_PSRAM_ENABLE                            (1 << 22)
#define CMU_BYPASS_DIV_PSRAM_ENABLE                         (1 << 23)
#define CMU_SEL_OSC_8_SYS_ENABLE                            (1 << 24)
#define CMU_SEL_PLL_FLS0_FAST_ENABLE                        (1 << 25)
#define CMU_SEL_PLL_FLS1_FAST_ENABLE                        (1 << 26)

// reg_064
#define CMU_RSTN_DIV_FLS0_DISABLE                           (1 << 0)
#define CMU_SEL_OSCX4_FLS0_DISABLE                          (1 << 1)
#define CMU_SEL_OSCX2_FLS0_DISABLE                          (1 << 2)
#define CMU_SEL_PLL_FLS0_DISABLE                            (1 << 3)
#define CMU_BYPASS_DIV_FLS0_DISABLE                         (1 << 4)
#define CMU_RSTN_DIV_SYS_DISABLE                            (1 << 5)
#define CMU_SEL_OSC_2_SYS_DISABLE                           (1 << 6)
#define CMU_SEL_OSC_4_SYS_DISABLE                           (1 << 7)
#define CMU_SEL_SLOW_SYS_DISABLE                            (1 << 8)
#define CMU_SEL_OSCX2_SYS_DISABLE                           (1 << 9)
#define CMU_SEL_FAST_SYS_DISABLE                            (1 << 10)
#define CMU_SEL_PLL_SYS_DISABLE                             (1 << 11)
#define CMU_BYPASS_DIV_SYS_DISABLE                          (1 << 12)
#define CMU_EN_OSCX2_DISABLE                                (1 << 13)
#define CMU_PU_OSCX2_DISABLE                                (1 << 14)
#define CMU_EN_OSCX4_DISABLE                                (1 << 15)
#define CMU_PU_OSCX4_DISABLE                                (1 << 16)
#define CMU_EN_PLL_DISABLE                                  (1 << 17)
#define CMU_PU_PLL_DISABLE                                  (1 << 18)
#define CMU_RSTN_DIV_PSRAM_DISABLE                          (1 << 19)
#define CMU_SEL_OSCX4_PSRAM_DISABLE                         (1 << 20)
#define CMU_SEL_OSCX2_PSRAM_DISABLE                         (1 << 21)
#define CMU_SEL_PLL_PSRAM_DISABLE                           (1 << 22)
#define CMU_BYPASS_DIV_PSRAM_DISABLE                        (1 << 23)
#define CMU_SEL_OSC_8_SYS_DISABLE                           (1 << 24)
#define CMU_SEL_PLL_FLS0_FAST_DISABLE                       (1 << 25)
#define CMU_SEL_PLL_FLS1_FAST_DISABLE                       (1 << 26)

// reg_068
#define CMU_DMA0_CH15_REQ_IDX(n)                            (((n) & 0x3F) << 0)
#define CMU_DMA0_CH15_REQ_IDX_MASK                          (0x3F << 0)
#define CMU_DMA0_CH15_REQ_IDX_SHIFT                         (0)
#define CMU_MASK_OBS1_CODEC_M55(n)                          (((n) & 0x7FF) << 6)
#define CMU_MASK_OBS1_CODEC_M55_MASK                        (0x7FF << 6)
#define CMU_MASK_OBS1_CODEC_M55_SHIFT                       (6)
#define CMU_MASK_OBS_CODEC_DSP                              (1 << 17)
#define CMU_MASK_OBS1_GPIO_M55(n)                           (((n) & 0x7FF) << 18)
#define CMU_MASK_OBS1_GPIO_M55_MASK                         (0x7FF << 18)
#define CMU_MASK_OBS1_GPIO_M55_SHIFT                        (18)
#define CMU_MASK_OBS_GPIO_DSP                               (1 << 29)

// reg_06c
#define CMU_ROM_RTSEL(n)                                    (((n) & 0x3) << 0)
#define CMU_ROM_RTSEL_MASK                                  (0x3 << 0)
#define CMU_ROM_RTSEL_SHIFT                                 (0)
#define CMU_ROM_PTSEL(n)                                    (((n) & 0x3) << 2)
#define CMU_ROM_PTSEL_MASK                                  (0x3 << 2)
#define CMU_ROM_PTSEL_SHIFT                                 (2)
#define CMU_ROM_TRB(n)                                      (((n) & 0x3) << 4)
#define CMU_ROM_TRB_MASK                                    (0x3 << 4)
#define CMU_ROM_TRB_SHIFT                                   (4)
#define CMU_ROM_PGEN                                        (1 << 6)
#define CMU_ROM_TM                                          (1 << 7)
#define CMU_RF_RTSEL(n)                                     (((n) & 0x3) << 8)
#define CMU_RF_RTSEL_MASK                                   (0x3 << 8)
#define CMU_RF_RTSEL_SHIFT                                  (8)
#define CMU_RF_WTSEL(n)                                     (((n) & 0x3) << 10)
#define CMU_RF_WTSEL_MASK                                   (0x3 << 10)
#define CMU_RF_WTSEL_SHIFT                                  (10)
#define CMU_RF_RCT(n)                                       (((n) & 0x3) << 12)
#define CMU_RF_RCT_MASK                                     (0x3 << 12)
#define CMU_RF_RCT_SHIFT                                    (12)
#define CMU_RF_WCT(n)                                       (((n) & 0x3) << 14)
#define CMU_RF_WCT_MASK                                     (0x3 << 14)
#define CMU_RF_WCT_SHIFT                                    (14)
#define CMU_RF_KP(n)                                        (((n) & 0x7) << 16)
#define CMU_RF_KP_MASK                                      (0x7 << 16)
#define CMU_RF_KP_SHIFT                                     (16)
#define CMU_RF2_SLP0(n)                                     (((n) & 0x3) << 19)
#define CMU_RF2_SLP0_MASK                                   (0x3 << 19)
#define CMU_RF2_SLP0_SHIFT                                  (19)
#define CMU_RF2_DSLP0(n)                                    (((n) & 0x3) << 21)
#define CMU_RF2_DSLP0_MASK                                  (0x3 << 21)
#define CMU_RF2_DSLP0_SHIFT                                 (21)
#define CMU_RF2_SD0(n)                                      (((n) & 0x3) << 23)
#define CMU_RF2_SD0_MASK                                    (0x3 << 23)
#define CMU_RF2_SD0_SHIFT                                   (23)
#define CMU_RF2_SLP1(n)                                     (((n) & 0x3) << 25)
#define CMU_RF2_SLP1_MASK                                   (0x3 << 25)
#define CMU_RF2_SLP1_SHIFT                                  (25)
#define CMU_RF2_DSLP1(n)                                    (((n) & 0x3) << 27)
#define CMU_RF2_DSLP1_MASK                                  (0x3 << 27)
#define CMU_RF2_DSLP1_SHIFT                                 (27)
#define CMU_RF2_SD1(n)                                      (((n) & 0x3) << 29)
#define CMU_RF2_SD1_MASK                                    (0x3 << 29)
#define CMU_RF2_SD1_SHIFT                                   (29)

// reg_070
#define CMU_CFG_DIV_UART0(n)                                (((n) & 0x3) << 0)
#define CMU_CFG_DIV_UART0_MASK                              (0x3 << 0)
#define CMU_CFG_DIV_UART0_SHIFT                             (0)
#define CMU_SEL_OSCX2_UART0                                 (1 << 2)
#define CMU_SEL_PLL_UART0                                   (1 << 3)
#define CMU_EN_PLL_UART0                                    (1 << 4)
#define CMU_CFG_DIV_UART1(n)                                (((n) & 0x3) << 5)
#define CMU_CFG_DIV_UART1_MASK                              (0x3 << 5)
#define CMU_CFG_DIV_UART1_SHIFT                             (5)
#define CMU_SEL_OSCX2_UART1                                 (1 << 7)
#define CMU_SEL_PLL_UART1                                   (1 << 8)
#define CMU_EN_PLL_UART1                                    (1 << 9)
#define CMU_CFG_DIV_DSI(n)                                  (((n) & 0x3) << 10)
#define CMU_CFG_DIV_DSI_MASK                                (0x3 << 10)
#define CMU_CFG_DIV_DSI_SHIFT                               (10)
#define CMU_SEL_PLL_SOURCE(n)                               (((n) & 0x3) << 12)
#define CMU_SEL_PLL_SOURCE_MASK                             (0x3 << 12)
#define CMU_SEL_PLL_SOURCE_SHIFT                            (12)
#define CMU_RSTN_DIV_GPU                                    (1 << 14)
#define CMU_SEL_OSC_2_GPU                                   (1 << 15)
#define CMU_SEL_OSC_4_GPU                                   (1 << 16)
#define CMU_SEL_OSC_8_GPU                                   (1 << 17)
#define CMU_SEL_SLOW_GPU                                    (1 << 18)
#define CMU_SEL_OSCX2_GPU                                   (1 << 19)
#define CMU_SEL_FAST_GPU                                    (1 << 20)
#define CMU_SEL_PLL_GPU                                     (1 << 21)
#define CMU_BYPASS_DIV_GPU                                  (1 << 22)
#define CMU_CFG_DIV_GPU(n)                                  (((n) & 0x3) << 23)
#define CMU_CFG_DIV_GPU_MASK                                (0x3 << 23)
#define CMU_CFG_DIV_GPU_SHIFT                               (23)
#define CMU_SEL_SYS_GT(n)                                   (((n) & 0x3) << 25)
#define CMU_SEL_SYS_GT_MASK                                 (0x3 << 25)
#define CMU_SEL_SYS_GT_SHIFT                                (25)

// reg_074
#define CMU_CFG_DIV_I2C(n)                                  (((n) & 0x3) << 0)
#define CMU_CFG_DIV_I2C_MASK                                (0x3 << 0)
#define CMU_CFG_DIV_I2C_SHIFT                               (0)
#define CMU_SEL_OSC_I2C                                     (1 << 2)
#define CMU_SEL_OSCX2_I2C                                   (1 << 3)
#define CMU_SEL_PLL_I2C                                     (1 << 4)
#define CMU_EN_PLL_I2C                                      (1 << 5)
#define CMU_POL_CLK_I2S0_IN                                 (1 << 6)
#define CMU_SEL_I2S0_CLKIN                                  (1 << 7)
#define CMU_EN_CLK_I2S0_OUT                                 (1 << 8)
#define CMU_POL_CLK_I2S0_OUT                                (1 << 9)
#define CMU_FORCE_PU_OFF                                    (1 << 10)
#define CMU_LOCK_CPU_EN                                     (1 << 11)
#define CMU_SEL_ROM_FAST                                    (1 << 12)
#define CMU_POL_CLK_I2S1_IN                                 (1 << 13)
#define CMU_SEL_I2S1_CLKIN                                  (1 << 14)
#define CMU_EN_CLK_I2S1_OUT                                 (1 << 15)
#define CMU_POL_CLK_I2S1_OUT                                (1 << 16)
#define CMU_EN_I2S_MCLK                                     (1 << 17)
#define CMU_SEL_I2S_MCLK(n)                                 (((n) & 0x7) << 18)
#define CMU_SEL_I2S_MCLK_MASK                               (0x7 << 18)
#define CMU_SEL_I2S_MCLK_SHIFT                              (18)
#define CMU_POL_CLK_DSI_IN                                  (1 << 21)
#define CMU_POL_CLK_PCM_IN                                  (1 << 22)
#define CMU_SEL_I2S0_OSC                                    (1 << 23)
#define CMU_SEL_I2S0_IN                                     (1 << 24)
#define CMU_SEL_I2S1_OSC                                    (1 << 25)
#define CMU_SEL_I2S1_IN                                     (1 << 26)

// reg_078
#define CMU_SYS2SES_INTISR_MASK0(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_SYS2SES_INTISR_MASK0_MASK                       (0xFFFFFFFF << 0)
#define CMU_SYS2SES_INTISR_MASK0_SHIFT                      (0)

// reg_07c
#define CMU_SYS2SES_INTISR_MASK1(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_SYS2SES_INTISR_MASK1_MASK                       (0xFFFFFFFF << 0)
#define CMU_SYS2SES_INTISR_MASK1_SHIFT                      (0)

// reg_080
#define CMU_WRITE_UNLOCK_H                                  (1 << 0)
#define CMU_WRITE_UNLOCK_STATUS                             (1 << 1)

// reg_084
#define CMU_WAKEUP_IRQ_MASK0(n)                             (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK0_MASK                           (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK0_SHIFT                          (0)

// reg_088
#define CMU_WAKEUP_IRQ_MASK1(n)                             (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK1_MASK                           (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK1_SHIFT                          (0)

// reg_08c
#define CMU_TIMER_WT26(n)                                   (((n) & 0xFF) << 0)
#define CMU_TIMER_WT26_MASK                                 (0xFF << 0)
#define CMU_TIMER_WT26_SHIFT                                (0)
#define CMU_TIMER_WTPLL(n)                                  (((n) & 0xF) << 8)
#define CMU_TIMER_WTPLL_MASK                                (0xF << 8)
#define CMU_TIMER_WTPLL_SHIFT                               (8)
#define CMU_LPU_AUTO_SWITCH26                               (1 << 12)
#define CMU_LPU_AUTO_SWITCHPLL                              (1 << 13)
#define CMU_LPU_STATUS_26M                                  (1 << 14)
#define CMU_LPU_STATUS_PLL                                  (1 << 15)
#define CMU_LPU_AUTO_MID                                    (1 << 16)

// reg_098
#define CMU_M552DSP_DATA_DONE_SET                           (1 << 0)
#define CMU_M552DSP_DATA1_DONE_SET                          (1 << 1)
#define CMU_M552DSP_DATA2_DONE_SET                          (1 << 2)
#define CMU_M552DSP_DATA3_DONE_SET                          (1 << 3)
#define CMU_DSP2M55_DATA_IND_SET                            (1 << 4)
#define CMU_DSP2M55_DATA1_IND_SET                           (1 << 5)
#define CMU_DSP2M55_DATA2_IND_SET                           (1 << 6)
#define CMU_DSP2M55_DATA3_IND_SET                           (1 << 7)

// reg_09c
#define CMU_M552DSP_DATA_DONE_CLR                           (1 << 0)
#define CMU_M552DSP_DATA1_DONE_CLR                          (1 << 1)
#define CMU_M552DSP_DATA2_DONE_CLR                          (1 << 2)
#define CMU_M552DSP_DATA3_DONE_CLR                          (1 << 3)
#define CMU_DSP2M55_DATA_IND_CLR                            (1 << 4)
#define CMU_DSP2M55_DATA1_IND_CLR                           (1 << 5)
#define CMU_DSP2M55_DATA2_IND_CLR                           (1 << 6)
#define CMU_DSP2M55_DATA3_IND_CLR                           (1 << 7)

// reg_0a0
#define CMU_BTH2SYS_DATA_DONE_SET                           (1 << 0)
#define CMU_BTH2SYS_DATA1_DONE_SET                          (1 << 1)
#define CMU_SYS2BTH_DATA_IND_SET                            (1 << 2)
#define CMU_SYS2BTH_DATA1_IND_SET                           (1 << 3)
#define CMU_BTH_ALLIRQ_MASK_SET                             (1 << 4)
#define CMU_BT_PLAYTIME_STAMP_INTR                          (1 << 5)
#define CMU_BT_PLAYTIME_STAMP1_INTR                         (1 << 6)
#define CMU_BT_PLAYTIME_STAMP2_INTR                         (1 << 7)
#define CMU_BT_PLAYTIME_STAMP3_INTR                         (1 << 8)
#define CMU_BT_PLAYTIME_STAMP_INTR_MSK                      (1 << 9)
#define CMU_BT_PLAYTIME_STAMP1_INTR_MSK                     (1 << 10)
#define CMU_BT_PLAYTIME_STAMP2_INTR_MSK                     (1 << 11)
#define CMU_BT_PLAYTIME_STAMP3_INTR_MSK                     (1 << 12)
#define CMU_BTH2SYS_DATA_MSK_SET                            (1 << 13)
#define CMU_BTH2SYS_DATA1_MSK_SET                           (1 << 14)
#define CMU_SYS2BTH_DATA_MSK_SET                            (1 << 15)
#define CMU_SYS2BTH_DATA1_MSK_SET                           (1 << 16)
#define CMU_BTH2SYS_DATA_INTR                               (1 << 17)
#define CMU_BTH2SYS_DATA1_INTR                              (1 << 18)
#define CMU_SYS2BTH_DATA_INTR                               (1 << 19)
#define CMU_SYS2BTH_DATA1_INTR                              (1 << 20)
#define CMU_BTH2SYS_DATA_INTR_MSK                           (1 << 21)
#define CMU_BTH2SYS_DATA1_INTR_MSK                          (1 << 22)
#define CMU_SYS2BTH_DATA_INTR_MSK                           (1 << 23)
#define CMU_SYS2BTH_DATA1_INTR_MSK                          (1 << 24)
#define CMU_SENS_ALLIRQ_MASK_SET                            (1 << 25)

// reg_0a4
#define CMU_BTH2SYS_DATA_DONE_CLR                           (1 << 0)
#define CMU_BTH2SYS_DATA1_DONE_CLR                          (1 << 1)
#define CMU_SYS2BTH_DATA_IND_CLR                            (1 << 2)
#define CMU_SYS2BTH_DATA1_IND_CLR                           (1 << 3)
#define CMU_BTH_ALLIRQ_MASK_CLR                             (1 << 4)
#define CMU_BT_PLAYTIME_STAMP_INTR_CLR                      (1 << 5)
#define CMU_BT_PLAYTIME_STAMP1_INTR_CLR                     (1 << 6)
#define CMU_BT_PLAYTIME_STAMP2_INTR_CLR                     (1 << 7)
#define CMU_BT_PLAYTIME_STAMP3_INTR_CLR                     (1 << 8)
#define CMU_BTH2SYS_DATA_MSK_CLR                            (1 << 13)
#define CMU_BTH2SYS_DATA1_MSK_CLR                           (1 << 14)
#define CMU_SYS2BTH_DATA_MSK_CLR                            (1 << 15)
#define CMU_SYS2BTH_DATA1_MSK_CLR                           (1 << 16)
#define CMU_SENS_ALLIRQ_MASK_CLR                            (1 << 25)

// reg_0a8
#define CMU_CFG_DIV_SYS(n)                                  (((n) & 0x3) << 0)
#define CMU_CFG_DIV_SYS_MASK                                (0x3 << 0)
#define CMU_CFG_DIV_SYS_SHIFT                               (0)
#define CMU_SEL_SMP_MCU(n)                                  (((n) & 0x7) << 2)
#define CMU_SEL_SMP_MCU_MASK                                (0x7 << 2)
#define CMU_SEL_SMP_MCU_SHIFT                               (2)
#define CMU_CFG_DIV_FLS0(n)                                 (((n) & 0x3) << 5)
#define CMU_CFG_DIV_FLS0_MASK                               (0x3 << 5)
#define CMU_CFG_DIV_FLS0_SHIFT                              (5)
#define CMU_CFG_DIV_XCLK(n)                                 (((n) & 0x3) << 7)
#define CMU_CFG_DIV_XCLK_MASK                               (0x3 << 7)
#define CMU_CFG_DIV_XCLK_SHIFT                              (7)
#define CMU_SEL_USB_6M                                      (1 << 9)
#define CMU_SEL_USB_SRC                                     (1 << 10)
#define CMU_POL_CLK_USB                                     (1 << 11)
#define CMU_USB_ID                                          (1 << 12)
#define CMU_CFG_DIV_PCLK(n)                                 (((n) & 0x3) << 13)
#define CMU_CFG_DIV_PCLK_MASK                               (0x3 << 13)
#define CMU_CFG_DIV_PCLK_SHIFT                              (13)
#define CMU_CFG_DIV_SPI0(n)                                 (((n) & 0x3) << 15)
#define CMU_CFG_DIV_SPI0_MASK                               (0x3 << 15)
#define CMU_CFG_DIV_SPI0_SHIFT                              (15)
#define CMU_SEL_OSCX2_SPI0                                  (1 << 17)
#define CMU_SEL_PLL_SPI0                                    (1 << 18)
#define CMU_EN_PLL_SPI0                                     (1 << 19)
#define CMU_CFG_DIV_SPI1(n)                                 (((n) & 0x3) << 20)
#define CMU_CFG_DIV_SPI1_MASK                               (0x3 << 20)
#define CMU_CFG_DIV_SPI1_SHIFT                              (20)
#define CMU_SEL_OSCX2_SPI1                                  (1 << 22)
#define CMU_SEL_PLL_SPI1                                    (1 << 23)
#define CMU_EN_PLL_SPI1                                     (1 << 24)
#define CMU_SEL_OSCX2_SPI2                                  (1 << 25)
#define CMU_SEL_DIV_XCLK                                    (1 << 26)
#define CMU_CFG_DIV_PSRAM(n)                                (((n) & 0x3) << 27)
#define CMU_CFG_DIV_PSRAM_MASK                              (0x3 << 27)
#define CMU_CFG_DIV_PSRAM_SHIFT                             (27)
#define CMU_SEL_PLLBB_PSRAM                                 (1 << 29)
#define CMU_JTAG_SEL_DUAL                                   (1 << 30)

// reg_0b0
#define CMU_SYS2BTH_INTISR_MASK0(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_SYS2BTH_INTISR_MASK0_MASK                       (0xFFFFFFFF << 0)
#define CMU_SYS2BTH_INTISR_MASK0_SHIFT                      (0)

// reg_0b4
#define CMU_SYS2BTH_INTISR_MASK1(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_SYS2BTH_INTISR_MASK1_MASK                       (0xFFFFFFFF << 0)
#define CMU_SYS2BTH_INTISR_MASK1_SHIFT                      (0)

// reg_0b8
#define CMU_DSP2M55_DATA_DONE_SET                           (1 << 0)
#define CMU_DSP2M55_DATA1_DONE_SET                          (1 << 1)
#define CMU_DSP2M55_DATA2_DONE_SET                          (1 << 2)
#define CMU_DSP2M55_DATA3_DONE_SET                          (1 << 3)
#define CMU_M552DSP_DATA_IND_SET                            (1 << 4)
#define CMU_M552DSP_DATA1_IND_SET                           (1 << 5)
#define CMU_M552DSP_DATA2_IND_SET                           (1 << 6)
#define CMU_M552DSP_DATA3_IND_SET                           (1 << 7)
#define CMU_BT_TOG_TRIG0_INTR                               (1 << 8)
#define CMU_BT_TOG_TRIG1_INTR                               (1 << 9)
#define CMU_BT_TOG_TRIG2_INTR                               (1 << 10)
#define CMU_BT_TOG_TRIG3_INTR                               (1 << 11)
#define CMU_BT_TOG_TRIG0_INTR_MSK                           (1 << 12)
#define CMU_BT_TOG_TRIG1_INTR_MSK                           (1 << 13)
#define CMU_BT_TOG_TRIG2_INTR_MSK                           (1 << 14)
#define CMU_BT_TOG_TRIG3_INTR_MSK                           (1 << 15)

// reg_0bc
#define CMU_DSP2M55_DATA_DONE_CLR                           (1 << 0)
#define CMU_DSP2M55_DATA1_DONE_CLR                          (1 << 1)
#define CMU_DSP2M55_DATA2_DONE_CLR                          (1 << 2)
#define CMU_DSP2M55_DATA3_DONE_CLR                          (1 << 3)
#define CMU_M552DSP_DATA_IND_CLR                            (1 << 4)
#define CMU_M552DSP_DATA1_IND_CLR                           (1 << 5)
#define CMU_M552DSP_DATA2_IND_CLR                           (1 << 6)
#define CMU_M552DSP_DATA3_IND_CLR                           (1 << 7)
#define CMU_BT_TOG_TRIG0_INTR_CLR                           (1 << 8)
#define CMU_BT_TOG_TRIG1_INTR_CLR                           (1 << 9)
#define CMU_BT_TOG_TRIG2_INTR_CLR                           (1 << 10)
#define CMU_BT_TOG_TRIG3_INTR_CLR                           (1 << 11)

// reg_0c0
#define CMU_MEMSC0                                          (1 << 0)

// reg_0c4
#define CMU_MEMSC1                                          (1 << 0)

// reg_0c8
#define CMU_MEMSC2                                          (1 << 0)

// reg_0cc
#define CMU_MEMSC3                                          (1 << 0)

// reg_0d0
#define CMU_MEMSC_STATUS0                                   (1 << 0)
#define CMU_MEMSC_STATUS1                                   (1 << 1)
#define CMU_MEMSC_STATUS2                                   (1 << 2)
#define CMU_MEMSC_STATUS3                                   (1 << 3)

// reg_0d4
#define CMU_DMA0_CH0_REQ_IDX(n)                             (((n) & 0x3F) << 0)
#define CMU_DMA0_CH0_REQ_IDX_MASK                           (0x3F << 0)
#define CMU_DMA0_CH0_REQ_IDX_SHIFT                          (0)
#define CMU_DMA0_CH1_REQ_IDX(n)                             (((n) & 0x3F) << 6)
#define CMU_DMA0_CH1_REQ_IDX_MASK                           (0x3F << 6)
#define CMU_DMA0_CH1_REQ_IDX_SHIFT                          (6)
#define CMU_DMA0_CH2_REQ_IDX(n)                             (((n) & 0x3F) << 12)
#define CMU_DMA0_CH2_REQ_IDX_MASK                           (0x3F << 12)
#define CMU_DMA0_CH2_REQ_IDX_SHIFT                          (12)
#define CMU_DMA0_CH3_REQ_IDX(n)                             (((n) & 0x3F) << 18)
#define CMU_DMA0_CH3_REQ_IDX_MASK                           (0x3F << 18)
#define CMU_DMA0_CH3_REQ_IDX_SHIFT                          (18)
#define CMU_DMA0_CH4_REQ_IDX(n)                             (((n) & 0x3F) << 24)
#define CMU_DMA0_CH4_REQ_IDX_MASK                           (0x3F << 24)
#define CMU_DMA0_CH4_REQ_IDX_SHIFT                          (24)

// reg_0d8
#define CMU_DMA0_CH5_REQ_IDX(n)                             (((n) & 0x3F) << 0)
#define CMU_DMA0_CH5_REQ_IDX_MASK                           (0x3F << 0)
#define CMU_DMA0_CH5_REQ_IDX_SHIFT                          (0)
#define CMU_DMA0_CH6_REQ_IDX(n)                             (((n) & 0x3F) << 6)
#define CMU_DMA0_CH6_REQ_IDX_MASK                           (0x3F << 6)
#define CMU_DMA0_CH6_REQ_IDX_SHIFT                          (6)
#define CMU_DMA0_CH7_REQ_IDX(n)                             (((n) & 0x3F) << 12)
#define CMU_DMA0_CH7_REQ_IDX_MASK                           (0x3F << 12)
#define CMU_DMA0_CH7_REQ_IDX_SHIFT                          (12)
#define CMU_DMA0_CH8_REQ_IDX(n)                             (((n) & 0x3F) << 18)
#define CMU_DMA0_CH8_REQ_IDX_MASK                           (0x3F << 18)
#define CMU_DMA0_CH8_REQ_IDX_SHIFT                          (18)
#define CMU_DMA0_CH9_REQ_IDX(n)                             (((n) & 0x3F) << 24)
#define CMU_DMA0_CH9_REQ_IDX_MASK                           (0x3F << 24)
#define CMU_DMA0_CH9_REQ_IDX_SHIFT                          (24)

// reg_0dc
#define CMU_DMA0_CH10_REQ_IDX(n)                            (((n) & 0x3F) << 0)
#define CMU_DMA0_CH10_REQ_IDX_MASK                          (0x3F << 0)
#define CMU_DMA0_CH10_REQ_IDX_SHIFT                         (0)
#define CMU_DMA0_CH11_REQ_IDX(n)                            (((n) & 0x3F) << 6)
#define CMU_DMA0_CH11_REQ_IDX_MASK                          (0x3F << 6)
#define CMU_DMA0_CH11_REQ_IDX_SHIFT                         (6)
#define CMU_DMA0_CH12_REQ_IDX(n)                            (((n) & 0x3F) << 12)
#define CMU_DMA0_CH12_REQ_IDX_MASK                          (0x3F << 12)
#define CMU_DMA0_CH12_REQ_IDX_SHIFT                         (12)
#define CMU_DMA0_CH13_REQ_IDX(n)                            (((n) & 0x3F) << 18)
#define CMU_DMA0_CH13_REQ_IDX_MASK                          (0x3F << 18)
#define CMU_DMA0_CH13_REQ_IDX_SHIFT                         (18)
#define CMU_DMA0_CH14_REQ_IDX(n)                            (((n) & 0x3F) << 24)
#define CMU_DMA0_CH14_REQ_IDX_MASK                          (0x3F << 24)
#define CMU_DMA0_CH14_REQ_IDX_SHIFT                         (24)

// reg_0e0
#define CMU_DMA1_CH0_REQ_IDX(n)                             (((n) & 0x3F) << 0)
#define CMU_DMA1_CH0_REQ_IDX_MASK                           (0x3F << 0)
#define CMU_DMA1_CH0_REQ_IDX_SHIFT                          (0)
#define CMU_DMA1_CH1_REQ_IDX(n)                             (((n) & 0x3F) << 6)
#define CMU_DMA1_CH1_REQ_IDX_MASK                           (0x3F << 6)
#define CMU_DMA1_CH1_REQ_IDX_SHIFT                          (6)
#define CMU_DMA1_CH2_REQ_IDX(n)                             (((n) & 0x3F) << 12)
#define CMU_DMA1_CH2_REQ_IDX_MASK                           (0x3F << 12)
#define CMU_DMA1_CH2_REQ_IDX_SHIFT                          (12)
#define CMU_DMA1_CH3_REQ_IDX(n)                             (((n) & 0x3F) << 18)
#define CMU_DMA1_CH3_REQ_IDX_MASK                           (0x3F << 18)
#define CMU_DMA1_CH3_REQ_IDX_SHIFT                          (18)
#define CMU_DMA1_CH4_REQ_IDX(n)                             (((n) & 0x3F) << 24)
#define CMU_DMA1_CH4_REQ_IDX_MASK                           (0x3F << 24)
#define CMU_DMA1_CH4_REQ_IDX_SHIFT                          (24)

// reg_0e4
#define CMU_DMA1_CH5_REQ_IDX(n)                             (((n) & 0x3F) << 0)
#define CMU_DMA1_CH5_REQ_IDX_MASK                           (0x3F << 0)
#define CMU_DMA1_CH5_REQ_IDX_SHIFT                          (0)
#define CMU_DMA1_CH6_REQ_IDX(n)                             (((n) & 0x3F) << 6)
#define CMU_DMA1_CH6_REQ_IDX_MASK                           (0x3F << 6)
#define CMU_DMA1_CH6_REQ_IDX_SHIFT                          (6)
#define CMU_DMA1_CH7_REQ_IDX(n)                             (((n) & 0x3F) << 12)
#define CMU_DMA1_CH7_REQ_IDX_MASK                           (0x3F << 12)
#define CMU_DMA1_CH7_REQ_IDX_SHIFT                          (12)
#define CMU_DMA1_CH8_REQ_IDX(n)                             (((n) & 0x3F) << 18)
#define CMU_DMA1_CH8_REQ_IDX_MASK                           (0x3F << 18)
#define CMU_DMA1_CH8_REQ_IDX_SHIFT                          (18)
#define CMU_DMA1_CH9_REQ_IDX(n)                             (((n) & 0x3F) << 24)
#define CMU_DMA1_CH9_REQ_IDX_MASK                           (0x3F << 24)
#define CMU_DMA1_CH9_REQ_IDX_SHIFT                          (24)

// reg_0e8
#define CMU_DMA1_CH10_REQ_IDX(n)                            (((n) & 0x3F) << 0)
#define CMU_DMA1_CH10_REQ_IDX_MASK                          (0x3F << 0)
#define CMU_DMA1_CH10_REQ_IDX_SHIFT                         (0)
#define CMU_DMA1_CH11_REQ_IDX(n)                            (((n) & 0x3F) << 6)
#define CMU_DMA1_CH11_REQ_IDX_MASK                          (0x3F << 6)
#define CMU_DMA1_CH11_REQ_IDX_SHIFT                         (6)
#define CMU_DMA1_CH12_REQ_IDX(n)                            (((n) & 0x3F) << 12)
#define CMU_DMA1_CH12_REQ_IDX_MASK                          (0x3F << 12)
#define CMU_DMA1_CH12_REQ_IDX_SHIFT                         (12)
#define CMU_DMA1_CH13_REQ_IDX(n)                            (((n) & 0x3F) << 18)
#define CMU_DMA1_CH13_REQ_IDX_MASK                          (0x3F << 18)
#define CMU_DMA1_CH13_REQ_IDX_SHIFT                         (18)
#define CMU_DMA1_CH14_REQ_IDX(n)                            (((n) & 0x3F) << 24)
#define CMU_DMA1_CH14_REQ_IDX_MASK                          (0x3F << 24)
#define CMU_DMA1_CH14_REQ_IDX_SHIFT                         (24)

// reg_0ec
#define CMU_DMA1_CH15_REQ_IDX(n)                            (((n) & 0x3F) << 0)
#define CMU_DMA1_CH15_REQ_IDX_MASK                          (0x3F << 0)
#define CMU_DMA1_CH15_REQ_IDX_SHIFT                         (0)

// reg_0f0
#define CMU_RESERVED(n)                                     (((n) & 0xFFFFFFFF) << 0)
#define CMU_RESERVED_MASK                                   (0xFFFFFFFF << 0)
#define CMU_RESERVED_SHIFT                                  (0)

// reg_0f4
#define CMU_DEBUG(n)                                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_DEBUG_MASK                                      (0xFFFFFFFF << 0)
#define CMU_DEBUG_SHIFT                                     (0)

// reg_0f8
#define CMU_RESERVED_3(n)                                   (((n) & 0xFFFFFFFF) << 0)
#define CMU_RESERVED_3_MASK                                 (0xFFFFFFFF << 0)
#define CMU_RESERVED_3_SHIFT                                (0)

// reg_0fc
#define CMU_SYS2SES_INTISR_MASK2(n)                         (((n) & 0xFF) << 0)
#define CMU_SYS2SES_INTISR_MASK2_MASK                       (0xFF << 0)
#define CMU_SYS2SES_INTISR_MASK2_SHIFT                      (0)
#define CMU_SYS2BTH_INTISR_MASK2(n)                         (((n) & 0xFF) << 8)
#define CMU_SYS2BTH_INTISR_MASK2_MASK                       (0xFF << 8)
#define CMU_SYS2BTH_INTISR_MASK2_SHIFT                      (8)
#define CMU_SYS2BTC_INTISR_MASK2(n)                         (((n) & 0xFF) << 16)
#define CMU_SYS2BTC_INTISR_MASK2_MASK                       (0xFF << 16)
#define CMU_SYS2BTC_INTISR_MASK2_SHIFT                      (16)
#define CMU_WAKEUP_IRQ_MASK2(n)                             (((n) & 0xFF) << 24)
#define CMU_WAKEUP_IRQ_MASK2_MASK                           (0xFF << 24)
#define CMU_WAKEUP_IRQ_MASK2_SHIFT                          (24)

// reg_100
#define CMU_RSTN_DIV_DSI_ENABLE                             (1 << 0)
#define CMU_SEL_OSCX4_DSI_ENABLE                            (1 << 1)
#define CMU_SEL_OSCX2_DSI_ENABLE                            (1 << 2)
#define CMU_SEL_PLL_DSI_ENABLE                              (1 << 3)
#define CMU_BYPASS_DIV_DSI_ENABLE                           (1 << 4)
#define CMU_RSTN_DIV_PER_ENABLE                             (1 << 5)
#define CMU_BYPASS_DIV_PER_ENABLE                           (1 << 6)
#define CMU_SEL_CLKIN_DSI_ENABLE                            (1 << 7)
#define CMU_RSTN_DIV_PIX_ENABLE                             (1 << 8)
#define CMU_SEL_OSCX4_PIX_ENABLE                            (1 << 9)
#define CMU_SEL_OSCX2_PIX_ENABLE                            (1 << 10)
#define CMU_SEL_PLL_PIX_ENABLE                              (1 << 11)
#define CMU_BYPASS_DIV_PIX_ENABLE                           (1 << 12)
#define CMU_RSTN_DIV_QSPI_ENABLE                            (1 << 13)
#define CMU_SEL_OSCX4_QSPI_ENABLE                           (1 << 14)
#define CMU_SEL_OSCX2_QSPI_ENABLE                           (1 << 15)
#define CMU_SEL_PLL_QSPI_ENABLE                             (1 << 16)
#define CMU_BYPASS_DIV_QSPI_ENABLE                          (1 << 17)

// reg_104
#define CMU_RSTN_DIV_DSI_DISABLE                            (1 << 0)
#define CMU_SEL_OSCX4_DSI_DISABLE                           (1 << 1)
#define CMU_SEL_OSCX2_DSI_DISABLE                           (1 << 2)
#define CMU_SEL_PLL_DSI_DISABLE                             (1 << 3)
#define CMU_BYPASS_DIV_DSI_DISABLE                          (1 << 4)
#define CMU_RSTN_DIV_PER_DISABLE                            (1 << 5)
#define CMU_BYPASS_DIV_PER_DISABLE                          (1 << 6)
#define CMU_SEL_CLKIN_DSI_DISABLE                           (1 << 7)
#define CMU_RSTN_DIV_PIX_DISABLE                            (1 << 8)
#define CMU_SEL_OSCX4_PIX_DISABLE                           (1 << 9)
#define CMU_SEL_OSCX2_PIX_DISABLE                           (1 << 10)
#define CMU_SEL_PLL_PIX_DISABLE                             (1 << 11)
#define CMU_BYPASS_DIV_PIX_DISABLE                          (1 << 12)
#define CMU_RSTN_DIV_QSPI_DISABLE                           (1 << 13)
#define CMU_SEL_OSCX4_QSPI_DISABLE                          (1 << 14)
#define CMU_SEL_OSCX2_QSPI_DISABLE                          (1 << 15)
#define CMU_SEL_PLL_QSPI_DISABLE                            (1 << 16)
#define CMU_BYPASS_DIV_QSPI_DISABLE                         (1 << 17)

// reg_10c
#define CMU_CFG_DIV_I2S0(n)                                 (((n) & 0x1FFF) << 0)
#define CMU_CFG_DIV_I2S0_MASK                               (0x1FFF << 0)
#define CMU_CFG_DIV_I2S0_SHIFT                              (0)
#define CMU_EN_CLK_PLL_I2S0                                 (1 << 13)
#define CMU_CFG_DIV_PER(n)                                  (((n) & 0x3) << 14)
#define CMU_CFG_DIV_PER_MASK                                (0x3 << 14)
#define CMU_CFG_DIV_PER_SHIFT                               (14)
#define CMU_SEL_PLL_AUD(n)                                  (((n) & 0x3) << 16)
#define CMU_SEL_PLL_AUD_MASK                                (0x3 << 16)
#define CMU_SEL_PLL_AUD_SHIFT                               (16)
#define CMU_CFG_DIV_QSPI(n)                                 (((n) & 0xF) << 18)
#define CMU_CFG_DIV_QSPI_MASK                               (0xF << 18)
#define CMU_CFG_DIV_QSPI_SHIFT                              (18)

// reg_110
#define CMU_CFG_DIV_SPDIF0(n)                               (((n) & 0x1FFF) << 0)
#define CMU_CFG_DIV_SPDIF0_MASK                             (0x1FFF << 0)
#define CMU_CFG_DIV_SPDIF0_SHIFT                            (0)
#define CMU_CFG_DIV_I2S1(n)                                 (((n) & 0x1FFF) << 13)
#define CMU_CFG_DIV_I2S1_MASK                               (0x1FFF << 13)
#define CMU_CFG_DIV_I2S1_SHIFT                              (13)
#define CMU_EN_CLK_PLL_SPDIF0                               (1 << 26)
#define CMU_EN_CLK_PLL_I2S1                                 (1 << 27)
#define CMU_CFG_DIV_PIX(n)                                  (((n) & 0xF) << 28)
#define CMU_CFG_DIV_PIX_MASK                                (0xF << 28)
#define CMU_CFG_DIV_PIX_SHIFT                               (28)

// reg_114
#define CMU_RF3_SLP0(n)                                     (((n) & 0x1F) << 0)
#define CMU_RF3_SLP0_MASK                                   (0x1F << 0)
#define CMU_RF3_SLP0_SHIFT                                  (0)
#define CMU_RF3_DSLP0(n)                                    (((n) & 0x1F) << 5)
#define CMU_RF3_DSLP0_MASK                                  (0x1F << 5)
#define CMU_RF3_DSLP0_SHIFT                                 (5)
#define CMU_RF3_SD0(n)                                      (((n) & 0x1F) << 10)
#define CMU_RF3_SD0_MASK                                    (0x1F << 10)
#define CMU_RF3_SD0_SHIFT                                   (10)
#define CMU_RF3_SLP1(n)                                     (((n) & 0x1F) << 15)
#define CMU_RF3_SLP1_MASK                                   (0x1F << 15)
#define CMU_RF3_SLP1_SHIFT                                  (15)
#define CMU_RF3_DSLP1(n)                                    (((n) & 0x1F) << 20)
#define CMU_RF3_DSLP1_MASK                                  (0x1F << 20)
#define CMU_RF3_DSLP1_SHIFT                                 (20)
#define CMU_RF3_SD1(n)                                      (((n) & 0x1F) << 25)
#define CMU_RF3_SD1_MASK                                    (0x1F << 25)
#define CMU_RF3_SD1_SHIFT                                   (25)

// reg_118
#define CMU_M55_ITCMSZ(n)                                   (((n) & 0xF) << 0)
#define CMU_M55_ITCMSZ_MASK                                 (0xF << 0)
#define CMU_M55_ITCMSZ_SHIFT                                (0)
#define CMU_M55_DTCMSZ(n)                                   (((n) & 0xF) << 4)
#define CMU_M55_DTCMSZ_MASK                                 (0xF << 4)
#define CMU_M55_DTCMSZ_SHIFT                                (4)
#define CMU_M55_PAHBSZ(n)                                   (((n) & 0x7) << 8)
#define CMU_M55_PAHBSZ_MASK                                 (0x7 << 8)
#define CMU_M55_PAHBSZ_SHIFT                                (8)
#define CMU_M55_MEMALIAS(n)                                 (((n) & 0x1F) << 11)
#define CMU_M55_MEMALIAS_MASK                               (0x1F << 11)
#define CMU_M55_MEMALIAS_SHIFT                              (11)
#define CMU_M55_FPU                                         (1 << 16)
#define CMU_M55_MVE(n)                                      (((n) & 0x3) << 17)
#define CMU_M55_MVE_MASK                                    (0x3 << 17)
#define CMU_M55_MVE_SHIFT                                   (17)
#define CMU_M55_MPUNSDISABLE                                (1 << 19)
#define CMU_M55_MPUSDISABLE                                 (1 << 20)
#define CMU_M55_SAUDISABLE                                  (1 << 21)
#define CMU_M55_INITTCMEN(n)                                (((n) & 0x3) << 22)
#define CMU_M55_INITTCMEN_MASK                              (0x3 << 22)
#define CMU_M55_INITTCMEN_SHIFT                             (22)
#define CMU_M55_INITPAHBEN                                  (1 << 24)
#define CMU_M55_INITL1RSTDIS                                (1 << 25)
#define CMU_M55_DBGEN                                       (1 << 26)
#define CMU_M55_SPIDEN                                      (1 << 27)
#define CMU_M55_NIDEN                                       (1 << 28)
#define CMU_M55_SPNIDEN                                     (1 << 29)
#define CMU_M55_DAPEN                                       (1 << 30)
#define CMU_M55_IDR_TYPE_SEL                                (1 << 31)

// reg_120
#define CMU_CFG_DIV_PWM0(n)                                 (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM0_MASK                               (0xFFF << 0)
#define CMU_CFG_DIV_PWM0_SHIFT                              (0)
#define CMU_SEL_OSC_PWM0                                    (1 << 12)
#define CMU_EN_OSC_PWM0                                     (1 << 13)
#define CMU_CFG_DIV_PWM1(n)                                 (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM1_MASK                               (0xFFF << 16)
#define CMU_CFG_DIV_PWM1_SHIFT                              (16)
#define CMU_SEL_OSC_PWM1                                    (1 << 28)
#define CMU_EN_OSC_PWM1                                     (1 << 29)

// reg_124
#define CMU_CFG_DIV_PWM2(n)                                 (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM2_MASK                               (0xFFF << 0)
#define CMU_CFG_DIV_PWM2_SHIFT                              (0)
#define CMU_SEL_OSC_PWM2                                    (1 << 12)
#define CMU_EN_OSC_PWM2                                     (1 << 13)
#define CMU_CFG_DIV_PWM3(n)                                 (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM3_MASK                               (0xFFF << 16)
#define CMU_CFG_DIV_PWM3_SHIFT                              (16)
#define CMU_SEL_OSC_PWM3                                    (1 << 28)
#define CMU_EN_OSC_PWM3                                     (1 << 29)

// reg_128
#define CMU_SES2SYS_DATA_DONE_SET                           (1 << 0)
#define CMU_SES2SYS_DATA1_DONE_SET                          (1 << 1)
#define CMU_SES2SYS_DATA2_DONE_SET                          (1 << 2)
#define CMU_SES2SYS_DATA3_DONE_SET                          (1 << 3)
#define CMU_SYS2SES_DATA_IND_SET                            (1 << 4)
#define CMU_SYS2SES_DATA1_IND_SET                           (1 << 5)
#define CMU_SYS2SES_DATA2_IND_SET                           (1 << 6)
#define CMU_SYS2SES_DATA3_IND_SET                           (1 << 7)
#define CMU_SES2SYS_DATA_MSK_SET                            (1 << 8)
#define CMU_SES2SYS_DATA1_MSK_SET                           (1 << 9)
#define CMU_SES2SYS_DATA2_MSK_SET                           (1 << 10)
#define CMU_SES2SYS_DATA3_MSK_SET                           (1 << 11)
#define CMU_SYS2SES_DATA_MSK_SET                            (1 << 12)
#define CMU_SYS2SES_DATA1_MSK_SET                           (1 << 13)
#define CMU_SYS2SES_DATA2_MSK_SET                           (1 << 14)
#define CMU_SYS2SES_DATA3_MSK_SET                           (1 << 15)
#define CMU_SES2SYS_DATA_INTR                               (1 << 16)
#define CMU_SES2SYS_DATA1_INTR                              (1 << 17)
#define CMU_SES2SYS_DATA2_INTR                              (1 << 18)
#define CMU_SES2SYS_DATA3_INTR                              (1 << 19)
#define CMU_SYS2SES_DATA_INTR                               (1 << 20)
#define CMU_SYS2SES_DATA1_INTR                              (1 << 21)
#define CMU_SYS2SES_DATA2_INTR                              (1 << 22)
#define CMU_SYS2SES_DATA3_INTR                              (1 << 23)
#define CMU_SES2SYS_DATA_INTR_MSK                           (1 << 24)
#define CMU_SES2SYS_DATA1_INTR_MSK                          (1 << 25)
#define CMU_SES2SYS_DATA2_INTR_MSK                          (1 << 26)
#define CMU_SES2SYS_DATA3_INTR_MSK                          (1 << 27)
#define CMU_SYS2SES_DATA_INTR_MSK                           (1 << 28)
#define CMU_SYS2SES_DATA1_INTR_MSK                          (1 << 29)
#define CMU_SYS2SES_DATA2_INTR_MSK                          (1 << 30)
#define CMU_SYS2SES_DATA3_INTR_MSK                          (1 << 31)

// reg_12c
#define CMU_SES2SYS_DATA_DONE_CLR                           (1 << 0)
#define CMU_SES2SYS_DATA1_DONE_CLR                          (1 << 1)
#define CMU_SES2SYS_DATA2_DONE_CLR                          (1 << 2)
#define CMU_SES2SYS_DATA3_DONE_CLR                          (1 << 3)
#define CMU_SYS2SES_DATA_IND_CLR                            (1 << 4)
#define CMU_SYS2SES_DATA1_IND_CLR                           (1 << 5)
#define CMU_SYS2SES_DATA2_IND_CLR                           (1 << 6)
#define CMU_SYS2SES_DATA3_IND_CLR                           (1 << 7)
#define CMU_SES2SYS_DATA_MSK_CLR                            (1 << 8)
#define CMU_SES2SYS_DATA1_MSK_CLR                           (1 << 9)
#define CMU_SES2SYS_DATA2_MSK_CLR                           (1 << 10)
#define CMU_SES2SYS_DATA3_MSK_CLR                           (1 << 11)
#define CMU_SYS2SES_DATA_MSK_CLR                            (1 << 12)
#define CMU_SYS2SES_DATA1_MSK_CLR                           (1 << 13)
#define CMU_SYS2SES_DATA2_MSK_CLR                           (1 << 14)
#define CMU_SYS2SES_DATA3_MSK_CLR                           (1 << 15)

// reg_134
#define CMU_MASK_OBS0_CODEC_M55(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_MASK_OBS0_CODEC_M55_MASK                        (0xFFFFFFFF << 0)
#define CMU_MASK_OBS0_CODEC_M55_SHIFT                       (0)

// reg_138
#define CMU_MASK_OBS0_GPIO_M55(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_MASK_OBS0_GPIO_M55_MASK                         (0xFFFFFFFF << 0)
#define CMU_MASK_OBS0_GPIO_M55_SHIFT                        (0)

// reg_13c
#define CMU_RSTN_DIV_LCDCP                                  (1 << 0)
#define CMU_SEL_OSCX4_LCDCP                                 (1 << 1)
#define CMU_SEL_OSCX2_LCDCP                                 (1 << 2)
#define CMU_SEL_PLL_LCDCP                                   (1 << 3)
#define CMU_BYPASS_DIV_LCDCP                                (1 << 4)
#define CMU_CFG_DIV_LCDCP(n)                                (((n) & 0xF) << 5)
#define CMU_CFG_DIV_LCDCP_MASK                              (0xF << 5)
#define CMU_CFG_DIV_LCDCP_SHIFT                             (5)

// reg_140
#define CMU_BTC2SYS_DATA_DONE_SET                           (1 << 0)
#define CMU_BTC2SYS_DATA1_DONE_SET                          (1 << 1)
#define CMU_SYS2BTC_DATA_IND_SET                            (1 << 2)
#define CMU_SYS2BTC_DATA1_IND_SET                           (1 << 3)
#define CMU_BTC2SYS_DATA_MSK_SET                            (1 << 4)
#define CMU_BTC2SYS_DATA1_MSK_SET                           (1 << 5)
#define CMU_SYS2BTC_DATA_MSK_SET                            (1 << 6)
#define CMU_SYS2BTC_DATA1_MSK_SET                           (1 << 7)
#define CMU_BTC2SYS_DATA_INTR                               (1 << 8)
#define CMU_BTC2SYS_DATA1_INTR                              (1 << 9)
#define CMU_SYS2BTC_DATA_INTR                               (1 << 10)
#define CMU_SYS2BTC_DATA1_INTR                              (1 << 11)
#define CMU_BTC2SYS_DATA_INTR_MSK                           (1 << 12)
#define CMU_BTC2SYS_DATA1_INTR_MSK                          (1 << 13)
#define CMU_SYS2BTC_DATA_INTR_MSK                           (1 << 14)
#define CMU_SYS2BTC_DATA1_INTR_MSK                          (1 << 15)
#define CMU_BTC_ALLIRQ_MASK_SET                             (1 << 16)

#ifndef CMU_BT2MCU_DATA_DONE_SET
//#define BTCMU_INTERSYS_BIT_OFFSET                           16

#define CMU_BT2MCU_DATA_DONE_SET                            CMU_BTC2SYS_DATA_DONE_SET
#define CMU_BT2MCU_DATA1_DONE_SET                           CMU_BTC2SYS_DATA1_DONE_SET
#define CMU_MCU2BT_DATA_IND_SET                             CMU_SYS2BTC_DATA_IND_SET
#define CMU_MCU2BT_DATA1_IND_SET                            CMU_SYS2BTC_DATA1_IND_SET
#define CMU_BT2MCU_DATA_MSK_SET                             CMU_BTC2SYS_DATA_MSK_SET
#define CMU_BT2MCU_DATA1_MSK_SET                            CMU_BTC2SYS_DATA1_MSK_SET
#define CMU_MCU2BT_DATA_MSK_SET                             CMU_SYS2BTC_DATA_MSK_SET
#define CMU_MCU2BT_DATA1_MSK_SET                            CMU_SYS2BTC_DATA1_MSK_SET
#define CMU_BT2MCU_DATA_INTR_MSK                            CMU_BTC2SYS_DATA_INTR_MSK
#define CMU_BT2MCU_DATA1_INTR_MSK                           CMU_BTC2SYS_DATA1_INTR_MSK
#define CMU_MCU2BT_DATA_INTR_MSK                            CMU_SYS2BTC_DATA_INTR_MSK
#define CMU_MCU2BT_DATA1_INTR_MSK                           CMU_SYS2BTC_DATA1_INTR_MSK
#endif

// reg_144
#define CMU_BTC2SYS_DATA_DONE_CLR                           (1 << 0)
#define CMU_BTC2SYS_DATA1_DONE_CLR                          (1 << 1)
#define CMU_SYS2BTC_DATA_IND_CLR                            (1 << 2)
#define CMU_SYS2BTC_DATA1_IND_CLR                           (1 << 3)
#define CMU_BTC2SYS_DATA_MSK_CLR                            (1 << 4)
#define CMU_BTC2SYS_DATA1_MSK_CLR                           (1 << 5)
#define CMU_SYS2BTC_DATA_MSK_CLR                            (1 << 6)
#define CMU_SYS2BTC_DATA1_MSK_CLR                           (1 << 7)
#define CMU_BTC_ALLIRQ_MASK_CLR                             (1 << 16)

#ifndef CMU_BT2MCU_DATA_DONE_CLR
#define CMU_BT2MCU_DATA_DONE_CLR                            CMU_BTC2SYS_DATA_DONE_CLR
#define CMU_BT2MCU_DATA1_DONE_CLR                           CMU_BTC2SYS_DATA1_DONE_CLR
#define CMU_MCU2BT_DATA_IND_CLR                             CMU_SYS2BTC_DATA_IND_CLR
#define CMU_MCU2BT_DATA1_IND_CLR                            CMU_SYS2BTC_DATA1_IND_CLR
#define CMU_BT2MCU_DATA_MSK_CLR                             CMU_BTC2SYS_DATA_MSK_CLR
#define CMU_BT2MCU_DATA1_MSK_CLR                            CMU_BTC2SYS_DATA1_MSK_CLR
#define CMU_MCU2BT_DATA_MSK_CLR                             CMU_SYS2BTC_DATA_MSK_CLR
#define CMU_MCU2BT_DATA1_MSK_CLR                            CMU_SYS2BTC_DATA1_MSK_CLR
#endif

// reg_148
#define CMU_SYS2BTC_INTISR_MASK0(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_SYS2BTC_INTISR_MASK0_MASK                       (0xFFFFFFFF << 0)
#define CMU_SYS2BTC_INTISR_MASK0_SHIFT                      (0)

// reg_14c
#define CMU_SYS2BTC_INTISR_MASK1(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_SYS2BTC_INTISR_MASK1_MASK                       (0xFFFFFFFF << 0)
#define CMU_SYS2BTC_INTISR_MASK1_SHIFT                      (0)

// reg_150
#define CMU_MANUAL_EN_AHB                                   (1 << 0)
#define CMU_MANUAL_EN_BTH                                   (1 << 1)
#define CMU_MANUAL_EN_SES                                   (1 << 2)
#define CMU_MANUAL_EN_DISPLAY                               (1 << 3)
#define CMU_MANUAL_EN_FLASH                                 (1 << 4)
#define CMU_MANUAL_EN_GPU                                   (1 << 5)
#define CMU_MANUAL_EN_GPV                                   (1 << 6)
#define CMU_MANUAL_EN_DSP                                   (1 << 7)
#define CMU_MANUAL_EN_M55                                   (1 << 8)
#define CMU_MANUAL_EN_MAIN                                  (1 << 9)
#define CMU_MANUAL_EN_PSRAM                                 (1 << 10)
#define CMU_MANUAL_EN_STORE                                 (1 << 11)
#define CMU_MANUAL_EN_ROM                                   (1 << 12)
#define CMU_MANUAL_EN_SRAM0                                 (1 << 13)
#define CMU_MANUAL_EN_SRAM1                                 (1 << 14)
#define CMU_MANUAL_EN_SRAM2                                 (1 << 15)
#define CMU_MANUAL_EN_SRAM3                                 (1 << 16)
#define CMU_MANUAL_EN_BTC                                   (1 << 17)
#define CMU_MANUAL_EN_M55_S                                 (1 << 18)
#define CMU_MANUAL_EN_STORE_NIC                             (1 << 19)
#define CMU_BYPASS_CG_ROM                                   (1 << 20)
#define CMU_BYPASS_CG_RAM                                   (1 << 21)
#define CMU_MANUAL_EN_AON                                   (1 << 22)
#define CMU_MANUAL_EN_PSRAM_MC                              (1 << 23)

// reg_154
#define CMU_WAKEUP_IRQ_MASK_DSP0(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK_DSP0_MASK                       (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK_DSP0_SHIFT                      (0)

// reg_158
#define CMU_WAKEUP_IRQ_MASK_DSP1(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK_DSP1_MASK                       (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK_DSP1_SHIFT                      (0)

// reg_15c
#define CMU_WAKEUP_IRQ_MASK_DSP2(n)                         (((n) & 0xFF) << 0)
#define CMU_WAKEUP_IRQ_MASK_DSP2_MASK                       (0xFF << 0)
#define CMU_WAKEUP_IRQ_MASK_DSP2_SHIFT                      (0)

// reg_160
#define CMU_MANUAL_QCLK_ENABLE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_QCLK_ENABLE_MASK                         (0xFFFFFFFF << 0)
#define CMU_MANUAL_QCLK_ENABLE_SHIFT                        (0)

// reg_164
#define CMU_MANUAL_QCLK_DISABLE(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_QCLK_DISABLE_MASK                        (0xFFFFFFFF << 0)
#define CMU_MANUAL_QCLK_DISABLE_SHIFT                       (0)

// reg_168
#define CMU_MODE_QCLK(n)                                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_QCLK_MASK                                  (0xFFFFFFFF << 0)
#define CMU_MODE_QCLK_SHIFT                                 (0)

// reg_16c
#define CMU_QRESETN_PULSE(n)                                (((n) & 0xFFFFFFFF) << 0)
#define CMU_QRESETN_PULSE_MASK                              (0xFFFFFFFF << 0)
#define CMU_QRESETN_PULSE_SHIFT                             (0)

// reg_170
#define CMU_QRESETN_SET(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_QRESETN_SET_MASK                                (0xFFFFFFFF << 0)
#define CMU_QRESETN_SET_SHIFT                               (0)

// reg_174
#define CMU_QRESETN_CLR(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_QRESETN_CLR_MASK                                (0xFFFFFFFF << 0)
#define CMU_QRESETN_CLR_SHIFT                               (0)

// reg_178
#define CMU_MANUAL_XCLK_ENABLE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_XCLK_ENABLE_MASK                         (0xFFFFFFFF << 0)
#define CMU_MANUAL_XCLK_ENABLE_SHIFT                        (0)

// reg_17c
#define CMU_MANUAL_XCLK_DISABLE(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_XCLK_DISABLE_MASK                        (0xFFFFFFFF << 0)
#define CMU_MANUAL_XCLK_DISABLE_SHIFT                       (0)

// reg_180
#define CMU_MODE_XCLK(n)                                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_XCLK_MASK                                  (0xFFFFFFFF << 0)
#define CMU_MODE_XCLK_SHIFT                                 (0)

// reg_184
#define CMU_XRESETN_PULSE(n)                                (((n) & 0xFFFFFFFF) << 0)
#define CMU_XRESETN_PULSE_MASK                              (0xFFFFFFFF << 0)
#define CMU_XRESETN_PULSE_SHIFT                             (0)

// reg_188
#define CMU_XRESETN_SET(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_XRESETN_SET_MASK                                (0xFFFFFFFF << 0)
#define CMU_XRESETN_SET_SHIFT                               (0)

// reg_18c
#define CMU_XRESETN_CLR(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define CMU_XRESETN_CLR_MASK                                (0xFFFFFFFF << 0)
#define CMU_XRESETN_CLR_SHIFT                               (0)

// reg_190
#define CMU_RF2P_TEST1A                                     (1 << 0)
#define CMU_RF2P_TEST_RNMA                                  (1 << 1)
#define CMU_RF2P_RMEA                                       (1 << 2)
#define CMU_RF2P_RMA(n)                                     (((n) & 0xF) << 3)
#define CMU_RF2P_RMA_MASK                                   (0xF << 3)
#define CMU_RF2P_RMA_SHIFT                                  (3)
#define CMU_RF2P_RA(n)                                      (((n) & 0x3) << 7)
#define CMU_RF2P_RA_MASK                                    (0x3 << 7)
#define CMU_RF2P_RA_SHIFT                                   (7)
#define CMU_RF2P_WA(n)                                      (((n) & 0x7) << 9)
#define CMU_RF2P_WA_MASK                                    (0x7 << 9)
#define CMU_RF2P_WA_SHIFT                                   (9)
#define CMU_RF2P_WPULSE(n)                                  (((n) & 0x7) << 12)
#define CMU_RF2P_WPULSE_MASK                                (0x7 << 12)
#define CMU_RF2P_WPULSE_SHIFT                               (12)
#define CMU_RF2P_RMEB                                       (1 << 15)
#define CMU_RF2P_RMB(n)                                     (((n) & 0xF) << 16)
#define CMU_RF2P_RMB_MASK                                   (0xF << 16)
#define CMU_RF2P_RMB_SHIFT                                  (16)
#define CMU_RF2P_TEST1B                                     (1 << 20)

// reg_194
#define CMU_CPU_IDLE_MASK                                   (1 << 0)
#define CMU_DSP_IDLE_MASK                                   (1 << 1)
#define CMU_WDT_NMI_MASK(n)                                 (((n) & 0x3) << 2)
#define CMU_WDT_NMI_MASK_MASK                               (0x3 << 2)
#define CMU_WDT_NMI_MASK_SHIFT                              (2)
#define CMU_BUS_DEEPSLEEP_MASK                              (1 << 4)
#define CMU_BT_TOG_TRIG0_MASK                               (1 << 5)
#define CMU_BT_TOG_TRIG1_MASK                               (1 << 6)
#define CMU_BT_TOG_TRIG2_MASK                               (1 << 7)
#define CMU_BT_TOG_TRIG3_MASK                               (1 << 8)
#define CMU_FORCE_ACK_PSRAM_MC_MASK                         (1 << 9)
#define CMU_DSP_NMI_INT                                     (1 << 10)
#define CMU_DSP_NMI_MASK                                    (1 << 11)

// reg_198
#define CMU_M55_COREPACCEPT                                 (1 << 0)
#define CMU_M55_COREPDENY                                   (1 << 1)
#define CMU_M55_COREPACTIVE(n)                              (((n) & 0x1FFFFF) << 2)
#define CMU_M55_COREPACTIVE_MASK                            (0x1FFFFF << 2)
#define CMU_M55_COREPACTIVE_SHIFT                           (2)

// reg_19c
#define CMU_CFG_LP_ENABLE_AHB                               (1 << 0)
#define CMU_CFG_LP_ENABLE_BTH                               (1 << 1)
#define CMU_CFG_LP_ENABLE_SES                               (1 << 2)
#define CMU_CFG_LP_ENABLE_DISPLAY                           (1 << 3)
#define CMU_CFG_LP_ENABLE_FLASH                             (1 << 4)
#define CMU_CFG_LP_ENABLE_GPU                               (1 << 5)
#define CMU_CFG_LP_ENABLE_GPV                               (1 << 6)
#define CMU_CFG_LP_ENABLE_DSP                               (1 << 7)
#define CMU_CFG_LP_ENABLE_M55                               (1 << 8)
#define CMU_CFG_LP_ENABLE_MAIN                              (1 << 9)
#define CMU_CFG_LP_ENABLE_PSRAM                             (1 << 10)
#define CMU_CFG_LP_ENABLE_STORE                             (1 << 11)
#define CMU_CFG_LP_ENABLE_ROM                               (1 << 12)
#define CMU_CFG_LP_ENABLE_SRAM0                             (1 << 13)
#define CMU_CFG_LP_ENABLE_SRAM1                             (1 << 14)
#define CMU_CFG_LP_ENABLE_SRAM2                             (1 << 15)
#define CMU_CFG_LP_ENABLE_SRAM3                             (1 << 16)
#define CMU_CFG_LP_ENABLE_BTC                               (1 << 17)
#define CMU_CFG_LP_ENABLE_M55_S                             (1 << 18)
#define CMU_CFG_LP_ENABLE_STORE_NIC                         (1 << 19)
#define CMU_CFG_LP_ENABLE_PSRAM_MC                          (1 << 20)

// reg_1a0
#define CMU_CFG_FORCE_LP_AHB                                (1 << 0)
#define CMU_CFG_FORCE_LP_BTH                                (1 << 1)
#define CMU_CFG_FORCE_LP_SES                                (1 << 2)
#define CMU_CFG_FORCE_LP_DISPLAY                            (1 << 3)
#define CMU_CFG_FORCE_LP_FLASH                              (1 << 4)
#define CMU_CFG_FORCE_LP_GPU                                (1 << 5)
#define CMU_CFG_FORCE_LP_GPV                                (1 << 6)
#define CMU_CFG_FORCE_LP_DSP                                (1 << 7)
#define CMU_CFG_FORCE_LP_M55                                (1 << 8)
#define CMU_CFG_FORCE_LP_MAIN                               (1 << 9)
#define CMU_CFG_FORCE_LP_PSRAM                              (1 << 10)
#define CMU_CFG_FORCE_LP_STORE                              (1 << 11)
#define CMU_CFG_FORCE_LP_ROM                                (1 << 12)
#define CMU_CFG_FORCE_LP_SRAM0                              (1 << 13)
#define CMU_CFG_FORCE_LP_SRAM1                              (1 << 14)
#define CMU_CFG_FORCE_LP_SRAM2                              (1 << 15)
#define CMU_CFG_FORCE_LP_SRAM3                              (1 << 16)
#define CMU_CFG_FORCE_LP_BTC                                (1 << 17)
#define CMU_CFG_FORCE_LP_M55_S                              (1 << 18)
#define CMU_CFG_FORCE_LP_STORE_NIC                          (1 << 19)
#define CMU_CFG_FORCE_LP_PSRAM_MC                           (1 << 20)

// reg_1a4
#define CMU_CFG_BYPASS_CLKEN_AHB                            (1 << 0)
#define CMU_CFG_BYPASS_CLKEN_BTH                            (1 << 1)
#define CMU_CFG_BYPASS_CLKEN_SES                            (1 << 2)
#define CMU_CFG_BYPASS_CLKEN_DISPLAY                        (1 << 3)
#define CMU_CFG_BYPASS_CLKEN_FLASH                          (1 << 4)
#define CMU_CFG_BYPASS_CLKEN_GPU                            (1 << 5)
#define CMU_CFG_BYPASS_CLKEN_GPV                            (1 << 6)
#define CMU_CFG_BYPASS_CLKEN_DSP                            (1 << 7)
#define CMU_CFG_BYPASS_CLKEN_M55                            (1 << 8)
#define CMU_CFG_BYPASS_CLKEN_MAIN                           (1 << 9)
#define CMU_CFG_BYPASS_CLKEN_PSRAM                          (1 << 10)
#define CMU_CFG_BYPASS_CLKEN_STORE                          (1 << 11)
#define CMU_CFG_BYPASS_CLKEN_ROM                            (1 << 12)
#define CMU_CFG_BYPASS_CLKEN_SRAM0                          (1 << 13)
#define CMU_CFG_BYPASS_CLKEN_SRAM1                          (1 << 14)
#define CMU_CFG_BYPASS_CLKEN_SRAM2                          (1 << 15)
#define CMU_CFG_BYPASS_CLKEN_SRAM3                          (1 << 16)
#define CMU_CFG_BYPASS_CLKEN_BTC                            (1 << 17)
#define CMU_CFG_BYPASS_CLKEN_M55_S                          (1 << 18)
#define CMU_CFG_BYPASS_CLKEN_STORE_NIC                      (1 << 19)
#define CMU_CFG_BYPASS_CLKEN_PSRAM_MC                       (1 << 20)

// reg_1a8
#define CMU_CFG_LP_NUMBER(n)                                (((n) & 0xFFFF) << 0)
#define CMU_CFG_LP_NUMBER_MASK                              (0xFFFF << 0)
#define CMU_CFG_LP_NUMBER_SHIFT                             (0)

// reg_1ac
#define CMU_FORCE_ACK_AHB                                   (1 << 0)
#define CMU_FORCE_ACK_BTH                                   (1 << 1)
#define CMU_FORCE_ACK_SES                                   (1 << 2)
#define CMU_FORCE_ACK_DISPLAY                               (1 << 3)
#define CMU_FORCE_ACK_FLASH                                 (1 << 4)
#define CMU_FORCE_ACK_GPU                                   (1 << 5)
#define CMU_FORCE_ACK_GPV                                   (1 << 6)
#define CMU_FORCE_ACK_DSP                                   (1 << 7)
#define CMU_FORCE_ACK_M55                                   (1 << 8)
#define CMU_FORCE_ACK_MAIN                                  (1 << 9)
#define CMU_FORCE_ACK_PSRAM                                 (1 << 10)
#define CMU_FORCE_ACK_STORE                                 (1 << 11)
#define CMU_FORCE_ACK_ROM                                   (1 << 12)
#define CMU_FORCE_ACK_SRAM0                                 (1 << 13)
#define CMU_FORCE_ACK_SRAM1                                 (1 << 14)
#define CMU_FORCE_ACK_SRAM2                                 (1 << 15)
#define CMU_FORCE_ACK_SRAM3                                 (1 << 16)
#define CMU_FORCE_ACK_BTC                                   (1 << 17)
#define CMU_FORCE_ACK_M55_S                                 (1 << 18)
#define CMU_FORCE_ACK_STORE_NIC                             (1 << 19)
#define CMU_FORCE_ACK_BUS                                   (1 << 20)
#define CMU_FORCE_ACK_PSRAM_MC                              (1 << 21)

// reg_1b0
#define CMU_RD_TIMER_TRIG(n)                                (((n) & 0xFFFFFFFF) << 0)
#define CMU_RD_TIMER_TRIG_MASK                              (0xFFFFFFFF << 0)
#define CMU_RD_TIMER_TRIG_SHIFT                             (0)

// reg_1b4
#define CMU_RD_TIMER_TRIG_I2S(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CMU_RD_TIMER_TRIG_I2S_MASK                          (0xFFFFFFFF << 0)
#define CMU_RD_TIMER_TRIG_I2S_SHIFT                         (0)

// reg_1b8
#define CMU_CFG_DIV_PWM4(n)                                 (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM4_MASK                               (0xFFF << 0)
#define CMU_CFG_DIV_PWM4_SHIFT                              (0)
#define CMU_SEL_OSC_PWM4                                    (1 << 12)
#define CMU_EN_OSC_PWM4                                     (1 << 13)
#define CMU_CFG_DIV_PWM5(n)                                 (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM5_MASK                               (0xFFF << 16)
#define CMU_CFG_DIV_PWM5_SHIFT                              (16)
#define CMU_SEL_OSC_PWM5                                    (1 << 28)
#define CMU_EN_OSC_PWM5                                     (1 << 29)

// reg_1bc
#define CMU_CFG_DIV_PWM6(n)                                 (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM6_MASK                               (0xFFF << 0)
#define CMU_CFG_DIV_PWM6_SHIFT                              (0)
#define CMU_SEL_OSC_PWM6                                    (1 << 12)
#define CMU_EN_OSC_PWM6                                     (1 << 13)
#define CMU_CFG_DIV_PWM7(n)                                 (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM7_MASK                               (0xFFF << 16)
#define CMU_CFG_DIV_PWM7_SHIFT                              (16)
#define CMU_SEL_OSC_PWM7                                    (1 << 28)
#define CMU_EN_OSC_PWM7                                     (1 << 29)

// reg_1c0
#define CMU_RD_TIMER_TRIG_GPIO0(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_RD_TIMER_TRIG_GPIO0_MASK                        (0xFFFFFFFF << 0)
#define CMU_RD_TIMER_TRIG_GPIO0_SHIFT                       (0)

// reg_1c4
#define CMU_RD_TIMER_TRIG_GPIO1(n)                          (((n) & 0xFFFFFFFF) << 0)
#define CMU_RD_TIMER_TRIG_GPIO1_MASK                        (0xFFFFFFFF << 0)
#define CMU_RD_TIMER_TRIG_GPIO1_SHIFT                       (0)

// reg_1c8
#define CMU_CFG_BYPASS_AUTO_CG_DSP                          (1 << 0)
#define CMU_CFG_CG_DSP_ICACHE(n)                            (((n) & 0xF) << 1)
#define CMU_CFG_CG_DSP_ICACHE_MASK                          (0xF << 1)
#define CMU_CFG_CG_DSP_ICACHE_SHIFT                         (1)
#define CMU_CFG_CG_DSP_DCACHE(n)                            (((n) & 0xF) << 5)
#define CMU_CFG_CG_DSP_DCACHE_MASK                          (0xF << 5)
#define CMU_CFG_CG_DSP_DCACHE_SHIFT                         (5)
#define CMU_CFG_CG_DSP_PREFETCH                             (1 << 9)
#define CMU_DUMP_PC_SEL                                     (1 << 10)

// reg_1cc
#define CMU_PVT_RO_EN                                       (1 << 0)
#define CMU_PVT_COUNT_START_EN                              (1 << 1)
#define CMU_PVT_OSC_CLK_EN                                  (1 << 2)
#define CMU_PVT_RSTN                                        (1 << 3)
#define CMU_PVT_REG_TIMESET(n)                              (((n) & 0x3FF) << 4)
#define CMU_PVT_REG_TIMESET_MASK                            (0x3FF << 4)
#define CMU_PVT_REG_TIMESET_SHIFT                           (4)

// reg_1d0
#define CMU_PVT_COUNT_VALUE(n)                              (((n) & 0x3FFFF) << 0)
#define CMU_PVT_COUNT_VALUE_MASK                            (0x3FFFF << 0)
#define CMU_PVT_COUNT_VALUE_SHIFT                           (0)
#define CMU_PVT_COUNT_DONE                                  (1 << 18)

// MCU System AHB Clocks:
#define SYS_HCLK_AHB0                                       (1 << 0)
#define SYS_HRST_AHB0                                       (1 << 0)
#define SYS_HCLK_AHB1                                       (1 << 1)
#define SYS_HRST_AHB1                                       (1 << 1)
#define SYS_HCLK_AHB2                                       (1 << 2)
#define SYS_HRST_AHB2                                       (1 << 2)
#define SYS_HCLK_DMA0                                       (1 << 3)
#define SYS_HRST_DMA0                                       (1 << 3)
#define SYS_HCLK_DMA1                                       (1 << 4)
#define SYS_HRST_DMA1                                       (1 << 4)
#define SYS_HCLK_AH2H_AON                                   (1 << 5)
#define SYS_HRST_AH2H_AON                                   (1 << 5)
#define SYS_HCLK_FLASH                                      (1 << 6)
#define SYS_HRST_FLASH                                      (1 << 6)
#define SYS_HCLK_SDMMC                                      (1 << 7)
#define SYS_HRST_SDMMC                                      (1 << 7)
#define SYS_HCLK_USBC                                       (1 << 8)
#define SYS_HRST_USBC                                       (1 << 8)
#define SYS_HCLK_USBH                                       (1 << 9)
#define SYS_HRST_USBH                                       (1 << 9)
#define SYS_HCLK_CODEC                                      (1 << 10)
#define SYS_HRST_CODEC                                      (1 << 10)
#define SYS_HCLK_BCM                                        (1 << 11)
#define SYS_HRST_BCM                                        (1 << 11)
#define SYS_HCLK_GPU                                        (1 << 12)
#define SYS_HRST_GPU                                        (1 << 12)
#define SYS_HCLK_LCDC                                       (1 << 13)
#define SYS_HRST_LCDC                                       (1 << 13)
#define SYS_HCLK_DISPPRE                                    (1 << 14)
#define SYS_HRST_DISPPRE                                    (1 << 14)
#define SYS_HCLK_DISP                                       (1 << 15)
#define SYS_HRST_DISP                                       (1 << 15)
#define SYS_HCLK_DMA2D                                      (1 << 16)
#define SYS_HRST_DMA2D                                      (1 << 16)
#define SYS_HCLK_AHB_S                                      (1 << 17)
#define SYS_HRST_AHB_S                                      (1 << 17)
#define SYS_HCLK_PSRAM                                      (1 << 18)
#define SYS_HRST_PSRAM                                      (1 << 18)
#define SYS_HCLK_SPI_AHB                                    (1 << 19)
#define SYS_HRST_SPI_AHB                                    (1 << 19)
#define SYS_HCLK_FLASH1                                     (1 << 20)
#define SYS_HRST_FLASH1                                     (1 << 20)
#define SYS_HCLK_DUMPCPU                                    (1 << 21)
#define SYS_HRST_DUMPCPU                                    (1 << 21)

// MCU System APB Clocks:
#define SYS_PCLK_CMU                                        (1 << 0)
#define SYS_PRST_CMU                                        (1 << 0)
#define SYS_PCLK_WDT                                        (1 << 1)
#define SYS_PRST_WDT                                        (1 << 1)
#define SYS_PCLK_TIMER0                                     (1 << 2)
#define SYS_PRST_TIMER0                                     (1 << 2)
#define SYS_PCLK_TIMER1                                     (1 << 3)
#define SYS_PRST_TIMER1                                     (1 << 3)
#define SYS_PCLK_TIMER2                                     (1 << 4)
#define SYS_PRST_TIMER2                                     (1 << 4)
#define SYS_PCLK_TIMER3                                     (1 << 5)
#define SYS_PRST_TIMER3                                     (1 << 5)
#define SYS_PCLK_I2C0                                       (1 << 6)
#define SYS_PRST_I2C0                                       (1 << 6)
#define SYS_PCLK_I2C1                                       (1 << 7)
#define SYS_PRST_I2C1                                       (1 << 7)
#define SYS_PCLK_SPI0                                       (1 << 8)
#define SYS_PRST_SPI0                                       (1 << 8)
#define SYS_PCLK_SPI1                                       (1 << 9)
#define SYS_PRST_SPI1                                       (1 << 9)
#define SYS_PCLK_SPI_ITN                                    (1 << 10)
#define SYS_PRST_SPI_ITN                                    (1 << 10)
#define SYS_PCLK_UART0                                      (1 << 11)
#define SYS_PRST_UART0                                      (1 << 11)
#define SYS_PCLK_UART1                                      (1 << 12)
#define SYS_PRST_UART1                                      (1 << 12)
#define SYS_PCLK_TRNG                                       (1 << 13)
#define SYS_PRST_TRNG                                       (1 << 13)
#define SYS_PCLK_I2S0                                       (1 << 14)
#define SYS_PRST_I2S0                                       (1 << 14)
#define SYS_PCLK_I2S1                                       (1 << 15)
#define SYS_PRST_I2S1                                       (1 << 15)
#define SYS_PCLK_SPDIF0                                     (1 << 16)
#define SYS_PRST_SPDIF0                                     (1 << 16)
#define SYS_PCLK_GPIO0                                      (1 << 17)
#define SYS_PRST_GPIO0                                      (1 << 17)
#define SYS_PCLK_GPIO0_INT                                  (1 << 18)
#define SYS_PRST_GPIO0_INT                                  (1 << 18)
#define SYS_PCLK_GPIO1                                      (1 << 19)
#define SYS_PRST_GPIO1                                      (1 << 19)
#define SYS_PCLK_GPIO1_INT                                  (1 << 20)
#define SYS_PRST_GPIO1_INT                                  (1 << 20)
#define SYS_PCLK_IOMUX                                      (1 << 21)
#define SYS_PRST_IOMUX                                      (1 << 21)
#define SYS_PCLK_BCM                                        (1 << 22)
#define SYS_PRST_BCM                                        (1 << 22)
#define SYS_PCLK_TZC                                        (1 << 23)
#define SYS_PRST_TZC                                        (1 << 23)
#define SYS_PCLK_PCM                                        (1 << 24)
#define SYS_PRST_PCM                                        (1 << 24)
#define SYS_PCLK_IRQCTL                                     (1 << 25)
#define SYS_PRST_IRQCTL                                     (1 << 25)
#define SYS_PCLK_PWM                                        (1 << 26)
#define SYS_PRST_PWM                                        (1 << 26)
#define SYS_PCLK_TCNT                                       (1 << 27)
#define SYS_PRST_TCNT                                       (1 << 27)
#define SYS_PCLK_PWM1                                       (1 << 28)
#define SYS_PRST_PWM1                                       (1 << 28)

// MCU System Other Clocks:
#define SYS_OCLK_SLEEP                                      (1 << 0)
#define SYS_ORST_SLEEP                                      (1 << 0)
#define SYS_OCLK_FLASH                                      (1 << 1)
#define SYS_ORST_FLASH                                      (1 << 1)
#define SYS_OCLK_USB                                        (1 << 2)
#define SYS_ORST_USB                                        (1 << 2)
#define SYS_OCLK_SDMMC                                      (1 << 3)
#define SYS_ORST_SDMMC                                      (1 << 3)
#define SYS_OCLK_WDT                                        (1 << 4)
#define SYS_ORST_WDT                                        (1 << 4)
#define SYS_OCLK_TIMER0                                     (1 << 5)
#define SYS_ORST_TIMER0                                     (1 << 5)
#define SYS_OCLK_TIMER1                                     (1 << 6)
#define SYS_ORST_TIMER1                                     (1 << 6)
#define SYS_OCLK_TIMER2                                     (1 << 7)
#define SYS_ORST_TIMER2                                     (1 << 7)
#define SYS_OCLK_TIMER3                                     (1 << 8)
#define SYS_ORST_TIMER3                                     (1 << 8)
#define SYS_OCLK_I2C0                                       (1 << 9)
#define SYS_ORST_I2C0                                       (1 << 9)
#define SYS_OCLK_I2C1                                       (1 << 10)
#define SYS_ORST_I2C1                                       (1 << 10)
#define SYS_OCLK_SPI0                                       (1 << 11)
#define SYS_ORST_SPI0                                       (1 << 11)
#define SYS_OCLK_SPI1                                       (1 << 12)
#define SYS_ORST_SPI1                                       (1 << 12)
#define SYS_OCLK_SPI_ITN                                    (1 << 13)
#define SYS_ORST_SPI_ITN                                    (1 << 13)
#define SYS_OCLK_UART0                                      (1 << 14)
#define SYS_ORST_UART0                                      (1 << 14)
#define SYS_OCLK_UART1                                      (1 << 15)
#define SYS_ORST_UART1                                      (1 << 15)
#define SYS_OCLK_EMMC                                       (1 << 16)
#define SYS_ORST_EMMC                                       (1 << 16)
#define SYS_OCLK_I2S0                                       (1 << 17)
#define SYS_ORST_I2S0                                       (1 << 17)
#define SYS_OCLK_I2S1                                       (1 << 18)
#define SYS_ORST_I2S1                                       (1 << 18)
#define SYS_OCLK_SPDIF0                                     (1 << 19)
#define SYS_ORST_SPDIF0                                     (1 << 19)
#define SYS_OCLK_USB32K                                     (1 << 20)
#define SYS_ORST_USB32K                                     (1 << 20)
#define SYS_OCLK_PSRAM                                      (1 << 21)
#define SYS_ORST_PSRAM                                      (1 << 21)
#define SYS_OCLK_CORE0                                      (1 << 22)
#define SYS_ORST_CORE0                                      (1 << 22)
#define SYS_OCLK_CORETCM                                    (1 << 23)
#define SYS_ORST_CORETCM                                    (1 << 23)
#define SYS_OCLK_COREROM                                    (1 << 24)
#define SYS_ORST_COREROM                                    (1 << 24)
#define SYS_OCLK_COREDBG                                    (1 << 25)
#define SYS_ORST_COREDBG                                    (1 << 25)
#define SYS_OCLK_DSP                                        (1 << 26)
#define SYS_ORST_DSP                                        (1 << 26)
#define SYS_OCLK_DSPTCM                                     (1 << 27)
#define SYS_ORST_DSPTCM                                     (1 << 27)
#define SYS_OCLK_GPUX1                                      (1 << 28)
#define SYS_ORST_GPUX1                                      (1 << 28)
#define SYS_OCLK_GPIO0                                      (1 << 29)
#define SYS_ORST_GPIO0                                      (1 << 29)
#define SYS_OCLK_GPIO1                                      (1 << 30)
#define SYS_ORST_GPIO1                                      (1 << 30)
#define SYS_OCLK_IOMUX                                      (1 << 31)
#define SYS_ORST_IOMUX                                      (1 << 31)

// MCU System Other Clocks:
#define SYS_QCLK_BTH2SYS                                    (1 << 0)
#define SYS_QRST_BTH2SYS                                    (1 << 0)
#define SYS_QCLK_BTH2FLS                                    (1 << 1)
#define SYS_QRST_BTH2FLS                                    (1 << 1)
#define SYS_QCLK_SES2SYS                                    (1 << 2)
#define SYS_QRST_SES2SYS                                    (1 << 2)
#define SYS_QCLK_DISPN2D                                    (1 << 3)
#define SYS_QRST_DISPN2D                                    (1 << 3)
#define SYS_QCLK_DISTV2D                                    (1 << 4)
#define SYS_QRST_DISTV2D                                    (1 << 4)
#define SYS_QCLK_LCDC_P                                     (1 << 5)
#define SYS_QRST_LCDC_P                                     (1 << 5)
#define SYS_QCLK_DIS_PN                                     (1 << 6)
#define SYS_QRST_DIS_PN                                     (1 << 6)
#define SYS_QCLK_DIS_TV                                     (1 << 7)
#define SYS_QRST_DIS_TV                                     (1 << 7)
#define SYS_QCLK_DIS_PIX                                    (1 << 8)
#define SYS_QRST_DIS_PIX                                    (1 << 8)
#define SYS_QCLK_DIS_DSI                                    (1 << 9)
#define SYS_QRST_DIS_DSI                                    (1 << 9)
#define SYS_QCLK_DIS_QSPI                                   (1 << 10)
#define SYS_QRST_DIS_QSPI                                   (1 << 10)
#define SYS_QCLK_PWM0                                       (1 << 11)
#define SYS_QRST_PWM0                                       (1 << 11)
#define SYS_QCLK_PWM1                                       (1 << 12)
#define SYS_QRST_PWM1                                       (1 << 12)
#define SYS_QCLK_PWM2                                       (1 << 13)
#define SYS_QRST_PWM2                                       (1 << 13)
#define SYS_QCLK_PWM3                                       (1 << 14)
#define SYS_QRST_PWM3                                       (1 << 14)
#define SYS_QCLK_PCM                                        (1 << 15)
#define SYS_QRST_PCM                                        (1 << 15)
#define SYS_QCLK_BTC2SYS                                    (1 << 16)
#define SYS_QRST_BTC2SYS                                    (1 << 16)
#define SYS_QCLK_PWM4                                       (1 << 17)
#define SYS_QRST_PWM4                                       (1 << 17)
#define SYS_QCLK_PWM5                                       (1 << 18)
#define SYS_QRST_PWM5                                       (1 << 18)
#define SYS_QCLK_PWM6                                       (1 << 19)
#define SYS_QRST_PWM6                                       (1 << 19)
#define SYS_QCLK_PWM7                                       (1 << 20)
#define SYS_QRST_PWM7                                       (1 << 20)
#define SYS_QCLK_FLASH1                                     (1 << 21)
#define SYS_QRST_FLASH1                                     (1 << 21)

// MCU System AXI Clocks:
#define SYS_XCLK_CORE0                                      (1 << 0)
#define SYS_XRST_CORE0                                      (1 << 0)
#define SYS_XCLK_DSP                                        (1 << 1)
#define SYS_XRST_DSP                                        (1 << 1)
#define SYS_XCLK_GPU                                        (1 << 2)
#define SYS_XRST_GPU                                        (1 << 2)
#define SYS_XCLK_DMAX                                       (1 << 3)
#define SYS_XRST_DMAX                                       (1 << 3)
#define SYS_XCLK_NIC                                        (1 << 4)
#define SYS_XRST_NIC                                        (1 << 4)
#define SYS_XCLK_DMA2D                                      (1 << 5)
#define SYS_XRST_DMA2D                                      (1 << 5)
#define SYS_XCLK_DISP                                       (1 << 6)
#define SYS_XRST_DISP                                       (1 << 6)
#define SYS_XCLK_DISPB                                      (1 << 7)
#define SYS_XRST_DISPB                                      (1 << 7)
#define SYS_XCLK_EMMC                                       (1 << 8)
#define SYS_XRST_EMMC                                       (1 << 8)
#define SYS_XCLK_FLASH                                      (1 << 9)
#define SYS_XRST_FLASH                                      (1 << 9)
#define SYS_XCLK_CODEC                                      (1 << 10)
#define SYS_XRST_CODEC                                      (1 << 10)
#define SYS_XCLK_RAM0                                       (1 << 11)
#define SYS_XRST_RAM0                                       (1 << 11)
#define SYS_XCLK_RAM1                                       (1 << 12)
#define SYS_XRST_RAM1                                       (1 << 12)
#define SYS_XCLK_RAM2                                       (1 << 13)
#define SYS_XRST_RAM2                                       (1 << 13)
#define SYS_XCLK_RAM3                                       (1 << 14)
#define SYS_XRST_RAM3                                       (1 << 14)
#define SYS_XCLK_RAM4                                       (1 << 15)
#define SYS_XRST_RAM4                                       (1 << 15)
#define SYS_XCLK_RAM5                                       (1 << 16)
#define SYS_XRST_RAM5                                       (1 << 16)
#define SYS_XCLK_PSRAM                                      (1 << 17)
#define SYS_XRST_PSRAM                                      (1 << 17)

#endif
