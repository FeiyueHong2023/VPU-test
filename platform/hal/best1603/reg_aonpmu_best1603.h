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
#ifndef __REG_AONPMU_BEST1603_H__
#define __REG_AONPMU_BEST1603_H__

#include "plat_types.h"

struct AONPMU_T {
    __I  uint32_t EFUSE_DATA;           // 0x000
    __IO uint32_t EFUSE_CTRL;           // 0x004
    __IO uint32_t EFUSE_CFG0;           // 0x008
    __IO uint32_t EFUSE_CFG1;           // 0x00C
    __IO uint32_t EFUSE_CFG2;           // 0x010
    __IO uint32_t EFUSE_DATA_OUT;       // 0x014
    __IO uint32_t EFUSE_CFG3;           // 0x018
    __IO uint32_t EFUSE_REDUN_DATA;     // 0x01C
    __IO uint32_t RESET_CFG;            // 0x020
    __IO uint32_t VCORE_ON_DELAY;       // 0x024
    __IO uint32_t SLEEP_CFG;            // 0x028
};

// reg_000
#define AON_PMU_EFUSE_DATA_OUT(n)                           (((n) & 0xFFFFFFFF) << 0)
#define AON_PMU_EFUSE_DATA_OUT_MASK                         (0xFFFFFFFF << 0)
#define AON_PMU_EFUSE_DATA_OUT_SHIFT                        (0)

// reg_004
#define AON_PMU_REG_EFUSE_ADDRESS(n)                        (((n) & 0x3FF) << 0)
#define AON_PMU_REG_EFUSE_ADDRESS_MASK                      (0x3FF << 0)
#define AON_PMU_REG_EFUSE_ADDRESS_SHIFT                     (0)
#define AON_PMU_REG_EFUSE_STROBE_TRIGGER                    (1 << 10)
#define AON_PMU_REG_EFUSE_TURN_ON                           (1 << 11)
#define AON_PMU_REG_EFUSE_CLK_EN                            (1 << 12)
#define AON_PMU_REG_EFUSE_READ_MODE                         (1 << 13)
#define AON_PMU_REG_EFUSE_PGM_MODE                          (1 << 14)
#define AON_PMU_REG_EFUSE_PGM_READ_SEL                      (1 << 15)

// reg_008
#define AON_PMU_REG_EFUSE_TIME_CSB_ADDR_VALUE(n)            (((n) & 0xF) << 0)
#define AON_PMU_REG_EFUSE_TIME_CSB_ADDR_VALUE_MASK          (0xF << 0)
#define AON_PMU_REG_EFUSE_TIME_CSB_ADDR_VALUE_SHIFT         (0)
#define AON_PMU_REG_EFUSE_TIME_PS_CSB_VALUE(n)              (((n) & 0xF) << 4)
#define AON_PMU_REG_EFUSE_TIME_PS_CSB_VALUE_MASK            (0xF << 4)
#define AON_PMU_REG_EFUSE_TIME_PS_CSB_VALUE_SHIFT           (4)
#define AON_PMU_REG_EFUSE_TIME_PD_PS_VALUE(n)               (((n) & 0x3F) << 8)
#define AON_PMU_REG_EFUSE_TIME_PD_PS_VALUE_MASK             (0x3F << 8)
#define AON_PMU_REG_EFUSE_TIME_PD_PS_VALUE_SHIFT            (8)
#define AON_PMU_REG_EFUSE_TIME_PGM_STROBING_VALUE(n)        (((n) & 0x1FF) << 14)
#define AON_PMU_REG_EFUSE_TIME_PGM_STROBING_VALUE_MASK      (0x1FF << 14)
#define AON_PMU_REG_EFUSE_TIME_PGM_STROBING_VALUE_SHIFT     (14)
#define AON_PMU_REG_EFUSE_TIME_ADDR_STROBE_VALUE(n)         (((n) & 0xF) << 23)
#define AON_PMU_REG_EFUSE_TIME_ADDR_STROBE_VALUE_MASK       (0xF << 23)
#define AON_PMU_REG_EFUSE_TIME_ADDR_STROBE_VALUE_SHIFT      (23)

// reg_00c
#define AON_PMU_REG_EFUSE_TIME_READ_STROBING_VALUE(n)       (((n) & 0x1FF) << 0)
#define AON_PMU_REG_EFUSE_TIME_READ_STROBING_VALUE_MASK     (0x1FF << 0)
#define AON_PMU_REG_EFUSE_TIME_READ_STROBING_VALUE_SHIFT    (0)
#define AON_PMU_REG_EFUSE_TIME_PS_PD_VALUE(n)               (((n) & 0x3F) << 9)
#define AON_PMU_REG_EFUSE_TIME_PS_PD_VALUE_MASK             (0x3F << 9)
#define AON_PMU_REG_EFUSE_TIME_PS_PD_VALUE_SHIFT            (9)
#define AON_PMU_REG_EFUSE_TIME_CSB_PS_VALUE(n)              (((n) & 0xF) << 15)
#define AON_PMU_REG_EFUSE_TIME_CSB_PS_VALUE_MASK            (0xF << 15)
#define AON_PMU_REG_EFUSE_TIME_CSB_PS_VALUE_SHIFT           (15)
#define AON_PMU_REG_EFUSE_TIME_STROBE_CSB_VALUE(n)          (((n) & 0x3F) << 19)
#define AON_PMU_REG_EFUSE_TIME_STROBE_CSB_VALUE_MASK        (0x3F << 19)
#define AON_PMU_REG_EFUSE_TIME_STROBE_CSB_VALUE_SHIFT       (19)
#define AON_PMU_REG_EFUSE_TIME_PD_OFF_VALUE(n)              (((n) & 0x3F) << 25)
#define AON_PMU_REG_EFUSE_TIME_PD_OFF_VALUE_MASK            (0x3F << 25)
#define AON_PMU_REG_EFUSE_TIME_PD_OFF_VALUE_SHIFT           (25)

// reg_010
#define AON_PMU_REG_EFUSE_TESTR_SEL(n)                      (((n) & 0x3) << 0)
#define AON_PMU_REG_EFUSE_TESTR_SEL_MASK                    (0x3 << 0)
#define AON_PMU_REG_EFUSE_TESTR_SEL_SHIFT                   (0)
#define AON_PMU_REG_EFUSE_TEST_MODE                         (1 << 2)
#define AON_PMU_REG_EFUSE_REDUNDANCY_DATA_OUT(n)            (((n) & 0xF) << 3)
#define AON_PMU_REG_EFUSE_REDUNDANCY_DATA_OUT_MASK          (0xF << 3)
#define AON_PMU_REG_EFUSE_REDUNDANCY_DATA_OUT_SHIFT         (3)
#define AON_PMU_REG_EFUSE_REDUNDANCY_DATA_OUT_DR            (1 << 7)
#define AON_PMU_REG_EFUSE_DATA_OUT_DR                       (1 << 8)

// reg_014
#define AON_PMU_REG_EFUSE_DATA_OUT_HI(n)                    (((n) & 0xFFFF) << 0)
#define AON_PMU_REG_EFUSE_DATA_OUT_HI_MASK                  (0xFFFF << 0)
#define AON_PMU_REG_EFUSE_DATA_OUT_HI_SHIFT                 (0)
#define AON_PMU_REG_EFUSE_DATA_OUT_LO(n)                    (((n) & 0xFFFF) << 16)
#define AON_PMU_REG_EFUSE_DATA_OUT_LO_MASK                  (0xFFFF << 16)
#define AON_PMU_REG_EFUSE_DATA_OUT_LO_SHIFT                 (16)

// reg_018
#define AON_PMU_EFUSE_CHIP_SEL_ENB_DR                       (1 << 0)
#define AON_PMU_EFUSE_CHIP_SEL_ENB_REG                      (1 << 1)
#define AON_PMU_EFUSE_STROBE_DR                             (1 << 2)
#define AON_PMU_EFUSE_STROBE_REG                            (1 << 3)
#define AON_PMU_EFUSE_LOAD_DR                               (1 << 4)
#define AON_PMU_EFUSE_LOAD_REG                              (1 << 5)
#define AON_PMU_EFUSE_PGM_ENB_DR                            (1 << 6)
#define AON_PMU_EFUSE_PGM_ENB_REG                           (1 << 7)
#define AON_PMU_EFUSE_PGM_SEL_DR                            (1 << 8)
#define AON_PMU_EFUSE_PGM_SEL_REG                           (1 << 9)
#define AON_PMU_EFUSE_READ_MODE_DR                          (1 << 10)
#define AON_PMU_EFUSE_READ_MODE_REG                         (1 << 11)
#define AON_PMU_EFUSE_PWR_DN_ENB_DR                         (1 << 12)
#define AON_PMU_EFUSE_PWR_DN_ENB_REG                        (1 << 13)
#define AON_PMU_EFUSE_REDUNDANCY_EN_DR                      (1 << 14)
#define AON_PMU_EFUSE_REDUNDANCY_EN_REG                     (1 << 15)
#define AON_PMU_EFUSE_REDUNDANCY_INFO_ROW_SEL_DR            (1 << 16)
#define AON_PMU_EFUSE_REDUNDANCY_INFO_ROW_SEL_REG           (1 << 17)
#define AON_PMU_EFUSE_TRCS_DR                               (1 << 18)
#define AON_PMU_EFUSE_TRCS_REG                              (1 << 19)

// reg_01c
#define AON_PMU_EFUSE_REDUNDANCY_DATA_OUT(n)                (((n) & 0xF) << 0)
#define AON_PMU_EFUSE_REDUNDANCY_DATA_OUT_MASK              (0xF << 0)
#define AON_PMU_EFUSE_REDUNDANCY_DATA_OUT_SHIFT             (0)

// reg_020
#define AON_PMU_PWR_ON_D1_REG(n)                            (((n) & 0xFF) << 0)
#define AON_PMU_PWR_ON_D1_REG_MASK                          (0xFF << 0)
#define AON_PMU_PWR_ON_D1_REG_SHIFT                         (0)
#define AON_PMU_PWR_ON_D2_REG(n)                            (((n) & 0xFF) << 8)
#define AON_PMU_PWR_ON_D2_REG_MASK                          (0xFF << 8)
#define AON_PMU_PWR_ON_D2_REG_SHIFT                         (8)
#define AON_PMU_PWR_ON_D3_REG(n)                            (((n) & 0xFF) << 16)
#define AON_PMU_PWR_ON_D3_REG_MASK                          (0xFF << 16)
#define AON_PMU_PWR_ON_D3_REG_SHIFT                         (16)
#define AON_PMU_RESETN_ANA_DR                               (1 << 24)
#define AON_PMU_RESETN_ANA_REG                              (1 << 25)
#define AON_PMU_RESETN_DIG_DR                               (1 << 26)
#define AON_PMU_RESETN_DIG_REG                              (1 << 27)
#define AON_PMU_RESETN_RC_DR                                (1 << 28)
#define AON_PMU_RESETN_RC_REG                               (1 << 29)
#define AON_PMU_RESETN_RF_DR                                (1 << 30)
#define AON_PMU_RESETN_RF_REG                               (1 << 31)

// reg_024
#define AON_PMU_VCORE_ON_DELAY_DR                           (1 << 0)
#define AON_PMU_VCORE_ON_DELAY_REG                          (1 << 1)
#define AON_PMU_SEL_RESETN_D2                               (1 << 2)

// reg_028
#define AON_PMU_REG_ANA_VPA_1P1_EN                          (1 << 0)
#define AON_PMU_REG_RF_BG_LOCKB                             (1 << 1)
#define AON_PMU_REG_RF_TST_BUF_LOCKB                        (1 << 2)
#define AON_PMU_ANA_VPA_1P1_EN_DR                           (1 << 3)
#define AON_PMU_RF_BG_LOCKB_DR                              (1 << 4)
#define AON_PMU_RF_TST_BUF_LOCKB_DR                         (1 << 5)
#define AON_PMU_SLEEP_ALLOW                                 (1 << 6)

#endif
