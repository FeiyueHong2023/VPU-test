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
#ifndef __REG_SEC_BEST1603_H__
#define __REG_SEC_BEST1603_H__

#include "plat_types.h"

struct SEC_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
    __IO uint32_t REG_038;
};

// reg_00
#define SEC_CFG_NONSEC_APB0(n)                              (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_NONSEC_APB0_MASK                            (0xFFFFFFFF << 0)
#define SEC_CFG_NONSEC_APB0_SHIFT                           (0)

// reg_04
#define SEC_CFG_NONSEC_BYPASS_APB0(n)                       (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_NONSEC_BYPASS_APB0_MASK                     (0xFFFFFFFF << 0)
#define SEC_CFG_NONSEC_BYPASS_APB0_SHIFT                    (0)

// reg_08
#define SEC_CFG_SEC2NSEC_APB0(n)                            (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_SEC2NSEC_APB0_MASK                          (0xFFFFFFFF << 0)
#define SEC_CFG_SEC2NSEC_APB0_SHIFT                         (0)

// reg_0c
#define SEC_CFG_NONSEC_AHB1(n)                              (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_NONSEC_AHB1_MASK                            (0xFFFFFFFF << 0)
#define SEC_CFG_NONSEC_AHB1_SHIFT                           (0)

// reg_10
#define SEC_CFG_NONSEC_BYPASS_AHB1(n)                       (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_NONSEC_BYPASS_AHB1_MASK                     (0xFFFFFFFF << 0)
#define SEC_CFG_NONSEC_BYPASS_AHB1_SHIFT                    (0)

// reg_14
#define SEC_CFG_SEC2NSEC_AHB1(n)                            (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_SEC2NSEC_AHB1_MASK                          (0xFFFFFFFF << 0)
#define SEC_CFG_SEC2NSEC_AHB1_SHIFT                         (0)

// reg_18
#define SEC_CFG_NONSEC_AON(n)                               (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_NONSEC_AON_MASK                             (0xFFFFFFFF << 0)
#define SEC_CFG_NONSEC_AON_SHIFT                            (0)

// reg_1c
#define SEC_CFG_NONSEC_BYPASS_AON(n)                        (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_NONSEC_BYPASS_AON_MASK                      (0xFFFFFFFF << 0)
#define SEC_CFG_NONSEC_BYPASS_AON_SHIFT                     (0)

// reg_20
#define SEC_CFG_SEC2NSEC_AON(n)                             (((n) & 0xFFFFFFFF) << 0)
#define SEC_CFG_SEC2NSEC_AON_MASK                           (0xFFFFFFFF << 0)
#define SEC_CFG_SEC2NSEC_AON_SHIFT                          (0)

// reg_24
#define SEC_CFG_NONSEC_BCM                                  (1 << 0)
#define SEC_CFG_NONSEC_USB                                  (1 << 1)
#define SEC_CFG_NONSEC_EMMC                                 (1 << 2)
#define SEC_CFG_NONSEC_BTH_FLS                              (1 << 3)

// reg_28
#define SEC_HNONSEC_BTH_H2X                                 (1 << 0)
#define SEC_HNONSEC_BTC_H2X                                 (1 << 1)
#define SEC_ANONSEC_DSP_M                                   (1 << 2)
#define SEC_ANONSEC_DISPLAY_M                               (1 << 3)
#define SEC_ANONSEC_DMA2D_M                                 (1 << 4)
#define SEC_ANONSEC_GPU_M                                   (1 << 5)
#define SEC_HNONSEC_HDMA0_M                                 (1 << 6)
#define SEC_HNONSEC_HDMA1_M                                 (1 << 7)
#define SEC_HNONSEC_SES_H2X                                 (1 << 8)
#define SEC_HNONSEC_SPI_SLV                                 (1 << 9)
#define SEC_CFG_WR_SEC_DMA(n)                               (((n) & 0x3) << 10)
#define SEC_CFG_WR_SEC_DMA_MASK                             (0x3 << 10)
#define SEC_CFG_WR_SEC_DMA_SHIFT                            (10)
#define SEC_CFG_RD_SEC_DMA(n)                               (((n) & 0x3) << 12)
#define SEC_CFG_RD_SEC_DMA_MASK                             (0x3 << 12)
#define SEC_CFG_RD_SEC_DMA_SHIFT                            (12)

// reg_2c
#define SEC_PPC_IRQ_BTH_X2H                                 (1 << 0)
#define SEC_PPC_IRQ_BTC_X2H                                 (1 << 1)
#define SEC_PPC_IRQ_SES_X2H                                 (1 << 2)
#define SEC_PPC_IRQ_M55_S                                   (1 << 3)
#define SEC_PPC_IRQ_CODEC                                   (1 << 4)
#define SEC_PPC_IRQ_DMA_S                                   (1 << 5)
#define SEC_PPC_IRQ_DSP_S                                   (1 << 6)
#define SEC_MPC_IRQ_FLASH                                   (1 << 7)
#define SEC_MPC_IRQ_PSRAM                                   (1 << 8)
#define SEC_MPC_IRQ_SRAM0                                   (1 << 9)
#define SEC_MPC_IRQ_SRAM1                                   (1 << 10)
#define SEC_MPC_IRQ_SRAM2                                   (1 << 11)
#define SEC_MPC_IRQ_SRAM3                                   (1 << 12)

// reg_30
#define SEC_SPY_IRQ_BTH_X2H                                 (1 << 0)
#define SEC_SPY_IRQ_BTC_X2H                                 (1 << 1)
#define SEC_SPY_IRQ_SES_X2H                                 (1 << 2)
#define SEC_SPY_IRQ_M55_S                                   (1 << 3)
#define SEC_SPY_IRQ_CODEC                                   (1 << 4)
#define SEC_SPY_IRQ_DMA_S                                   (1 << 5)
#define SEC_SPY_IRQ_DSP_S                                   (1 << 6)
#define SEC_SPY_IRQ_FLASH                                   (1 << 7)
#define SEC_SPY_IRQ_PSRAM                                   (1 << 8)
#define SEC_SPY_IRQ_SRAM0                                   (1 << 9)
#define SEC_SPY_IRQ_SRAM1                                   (1 << 10)
#define SEC_SPY_IRQ_SRAM2                                   (1 << 11)
#define SEC_SPY_IRQ_SRAM3                                   (1 << 12)

// reg_34
#define SEC_IDAU_CALL_ADDR(n)                               (((n) & 0xFFFFFFFF) << 0)
#define SEC_IDAU_CALL_ADDR_MASK                             (0xFFFFFFFF << 0)
#define SEC_IDAU_CALL_ADDR_SHIFT                            (0)

// reg_38
#define SEC_IDAU_EN                                         (1 << 0)
#define SEC_XDMA_GLOBAL_SECURE_ENABLE                       (1 << 1)
#define SEC_XDMA_CHL_SECURE(n)                              (((n) & 0xFFFF) << 2)
#define SEC_XDMA_CHL_SECURE_MASK                            (0xFFFF << 2)
#define SEC_XDMA_CHL_SECURE_SHIFT                           (2)

#endif

