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
#ifndef __REG_AONIOMUX_BEST1603_H_
#define __REG_AONIOMUX_BEST1603_H_

#include "plat_types.h"

struct AONIOMUX_T {
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
    __IO uint32_t REG_03C;
    __IO uint32_t REG_040;
    __IO uint32_t REG_044;
    __IO uint32_t REG_048;
    __IO uint32_t REG_04C;
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
    __IO uint32_t REG_070;
    __IO uint32_t REG_074;
    __IO uint32_t REG_078;
    __IO uint32_t REG_07C;
    __IO uint32_t REG_080;
};

// reg_00
#define IOMUX_AON_32_H00001603(n)                           (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_AON_32_H00001603_MASK                         (0xFFFFFFFF << 0)
#define IOMUX_AON_32_H00001603_SHIFT                        (0)

// reg_04
#define IOMUX_AON_GPIO_P00_SEL(n)                           (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_P00_SEL_MASK                         (0xF << 0)
#define IOMUX_AON_GPIO_P00_SEL_SHIFT                        (0)
#define IOMUX_AON_GPIO_P01_SEL(n)                           (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_P01_SEL_MASK                         (0xF << 4)
#define IOMUX_AON_GPIO_P01_SEL_SHIFT                        (4)
#define IOMUX_AON_GPIO_P02_SEL(n)                           (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_P02_SEL_MASK                         (0xF << 8)
#define IOMUX_AON_GPIO_P02_SEL_SHIFT                        (8)
#define IOMUX_AON_GPIO_P03_SEL(n)                           (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_P03_SEL_MASK                         (0xF << 12)
#define IOMUX_AON_GPIO_P03_SEL_SHIFT                        (12)
#define IOMUX_AON_GPIO_P04_SEL(n)                           (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_P04_SEL_MASK                         (0xF << 16)
#define IOMUX_AON_GPIO_P04_SEL_SHIFT                        (16)
#define IOMUX_AON_GPIO_P05_SEL(n)                           (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_P05_SEL_MASK                         (0xF << 20)
#define IOMUX_AON_GPIO_P05_SEL_SHIFT                        (20)
#define IOMUX_AON_GPIO_P06_SEL(n)                           (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_P06_SEL_MASK                         (0xF << 24)
#define IOMUX_AON_GPIO_P06_SEL_SHIFT                        (24)
#define IOMUX_AON_GPIO_P07_SEL(n)                           (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_P07_SEL_MASK                         (0xF << 28)
#define IOMUX_AON_GPIO_P07_SEL_SHIFT                        (28)

// reg_08
#define IOMUX_AON_GPIO_P10_SEL(n)                           (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_P10_SEL_MASK                         (0xF << 0)
#define IOMUX_AON_GPIO_P10_SEL_SHIFT                        (0)
#define IOMUX_AON_GPIO_P11_SEL(n)                           (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_P11_SEL_MASK                         (0xF << 4)
#define IOMUX_AON_GPIO_P11_SEL_SHIFT                        (4)
#define IOMUX_AON_GPIO_P12_SEL(n)                           (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_P12_SEL_MASK                         (0xF << 8)
#define IOMUX_AON_GPIO_P12_SEL_SHIFT                        (8)
#define IOMUX_AON_GPIO_P13_SEL(n)                           (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_P13_SEL_MASK                         (0xF << 12)
#define IOMUX_AON_GPIO_P13_SEL_SHIFT                        (12)
#define IOMUX_AON_GPIO_P14_SEL(n)                           (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_P14_SEL_MASK                         (0xF << 16)
#define IOMUX_AON_GPIO_P14_SEL_SHIFT                        (16)
#define IOMUX_AON_GPIO_P15_SEL(n)                           (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_P15_SEL_MASK                         (0xF << 20)
#define IOMUX_AON_GPIO_P15_SEL_SHIFT                        (20)
#define IOMUX_AON_GPIO_P16_SEL(n)                           (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_P16_SEL_MASK                         (0xF << 24)
#define IOMUX_AON_GPIO_P16_SEL_SHIFT                        (24)
#define IOMUX_AON_GPIO_P17_SEL(n)                           (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_P17_SEL_MASK                         (0xF << 28)
#define IOMUX_AON_GPIO_P17_SEL_SHIFT                        (28)

// reg_0c
#define IOMUX_AON_GPIO_P20_SEL(n)                           (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_P20_SEL_MASK                         (0xF << 0)
#define IOMUX_AON_GPIO_P20_SEL_SHIFT                        (0)
#define IOMUX_AON_GPIO_P21_SEL(n)                           (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_P21_SEL_MASK                         (0xF << 4)
#define IOMUX_AON_GPIO_P21_SEL_SHIFT                        (4)
#define IOMUX_AON_GPIO_P22_SEL(n)                           (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_P22_SEL_MASK                         (0xF << 8)
#define IOMUX_AON_GPIO_P22_SEL_SHIFT                        (8)
#define IOMUX_AON_GPIO_P23_SEL(n)                           (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_P23_SEL_MASK                         (0xF << 12)
#define IOMUX_AON_GPIO_P23_SEL_SHIFT                        (12)
#define IOMUX_AON_GPIO_P24_SEL(n)                           (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_P24_SEL_MASK                         (0xF << 16)
#define IOMUX_AON_GPIO_P24_SEL_SHIFT                        (16)
#define IOMUX_AON_GPIO_P25_SEL(n)                           (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_P25_SEL_MASK                         (0xF << 20)
#define IOMUX_AON_GPIO_P25_SEL_SHIFT                        (20)
#define IOMUX_AON_GPIO_P26_SEL(n)                           (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_P26_SEL_MASK                         (0xF << 24)
#define IOMUX_AON_GPIO_P26_SEL_SHIFT                        (24)
#define IOMUX_AON_GPIO_P27_SEL(n)                           (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_P27_SEL_MASK                         (0xF << 28)
#define IOMUX_AON_GPIO_P27_SEL_SHIFT                        (28)

// reg_10
#define IOMUX_AON_GPIO_S00_SEL(n)                           (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_S00_SEL_MASK                         (0xF << 0)
#define IOMUX_AON_GPIO_S00_SEL_SHIFT                        (0)
#define IOMUX_AON_GPIO_S01_SEL(n)                           (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_S01_SEL_MASK                         (0xF << 4)
#define IOMUX_AON_GPIO_S01_SEL_SHIFT                        (4)
#define IOMUX_AON_GPIO_S02_SEL(n)                           (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_S02_SEL_MASK                         (0xF << 8)
#define IOMUX_AON_GPIO_S02_SEL_SHIFT                        (8)
#define IOMUX_AON_GPIO_S03_SEL(n)                           (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_S03_SEL_MASK                         (0xF << 12)
#define IOMUX_AON_GPIO_S03_SEL_SHIFT                        (12)
#define IOMUX_AON_GPIO_S04_SEL(n)                           (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_S04_SEL_MASK                         (0xF << 16)
#define IOMUX_AON_GPIO_S04_SEL_SHIFT                        (16)
#define IOMUX_AON_GPIO_S05_SEL(n)                           (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_S05_SEL_MASK                         (0xF << 20)
#define IOMUX_AON_GPIO_S05_SEL_SHIFT                        (20)
#define IOMUX_AON_GPIO_S06_SEL(n)                           (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_S06_SEL_MASK                         (0xF << 24)
#define IOMUX_AON_GPIO_S06_SEL_SHIFT                        (24)
#define IOMUX_AON_GPIO_S07_SEL(n)                           (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_S07_SEL_MASK                         (0xF << 28)
#define IOMUX_AON_GPIO_S07_SEL_SHIFT                        (28)

// reg_14
#define IOMUX_AON_GPIO_S10_SEL(n)                           (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_S10_SEL_MASK                         (0xF << 0)
#define IOMUX_AON_GPIO_S10_SEL_SHIFT                        (0)
#define IOMUX_AON_GPIO_S11_SEL(n)                           (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_S11_SEL_MASK                         (0xF << 4)
#define IOMUX_AON_GPIO_S11_SEL_SHIFT                        (4)
#define IOMUX_AON_GPIO_S12_SEL(n)                           (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_S12_SEL_MASK                         (0xF << 8)
#define IOMUX_AON_GPIO_S12_SEL_SHIFT                        (8)
#define IOMUX_AON_GPIO_S13_SEL(n)                           (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_S13_SEL_MASK                         (0xF << 12)
#define IOMUX_AON_GPIO_S13_SEL_SHIFT                        (12)
#define IOMUX_AON_GPIO_S14_SEL(n)                           (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_S14_SEL_MASK                         (0xF << 16)
#define IOMUX_AON_GPIO_S14_SEL_SHIFT                        (16)
#define IOMUX_AON_GPIO_S15_SEL(n)                           (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_S15_SEL_MASK                         (0xF << 20)
#define IOMUX_AON_GPIO_S15_SEL_SHIFT                        (20)
#define IOMUX_AON_GPIO_S16_SEL(n)                           (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_S16_SEL_MASK                         (0xF << 24)
#define IOMUX_AON_GPIO_S16_SEL_SHIFT                        (24)
#define IOMUX_AON_GPIO_S17_SEL(n)                           (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_S17_SEL_MASK                         (0xF << 28)
#define IOMUX_AON_GPIO_S17_SEL_SHIFT                        (28)

// reg_18
#define IOMUX_AON_GPIO_S20_SEL(n)                           (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_S20_SEL_MASK                         (0xF << 0)
#define IOMUX_AON_GPIO_S20_SEL_SHIFT                        (0)
#define IOMUX_AON_GPIO_S21_SEL(n)                           (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_S21_SEL_MASK                         (0xF << 4)
#define IOMUX_AON_GPIO_S21_SEL_SHIFT                        (4)
#define IOMUX_AON_GPIO_S22_SEL(n)                           (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_S22_SEL_MASK                         (0xF << 8)
#define IOMUX_AON_GPIO_S22_SEL_SHIFT                        (8)
#define IOMUX_AON_GPIO_S23_SEL(n)                           (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_S23_SEL_MASK                         (0xF << 12)
#define IOMUX_AON_GPIO_S23_SEL_SHIFT                        (12)
#define IOMUX_AON_GPIO_S24_SEL(n)                           (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_S24_SEL_MASK                         (0xF << 16)
#define IOMUX_AON_GPIO_S24_SEL_SHIFT                        (16)
#define IOMUX_AON_GPIO_S25_SEL(n)                           (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_S25_SEL_MASK                         (0xF << 20)
#define IOMUX_AON_GPIO_S25_SEL_SHIFT                        (20)
#define IOMUX_AON_GPIO_S26_SEL(n)                           (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_S26_SEL_MASK                         (0xF << 24)
#define IOMUX_AON_GPIO_S26_SEL_SHIFT                        (24)
#define IOMUX_AON_GPIO_S27_SEL(n)                           (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_S27_SEL_MASK                         (0xF << 28)
#define IOMUX_AON_GPIO_S27_SEL_SHIFT                        (28)

// reg_1c
#define IOMUX_AON_RESERVED1(n)                              (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_AON_RESERVED1_MASK                            (0xFFFFFFFF << 0)
#define IOMUX_AON_RESERVED1_SHIFT                           (0)

// reg_20
#define IOMUX_AON_RESERVED2(n)                              (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_AON_RESERVED2_MASK                            (0xFFFFFFFF << 0)
#define IOMUX_AON_RESERVED2_SHIFT                           (0)

// reg_24
#define IOMUX_AON_R_GPIO_S0_PU(n)                           (((n) & 0xFF) << 0)
#define IOMUX_AON_R_GPIO_S0_PU_MASK                         (0xFF << 0)
#define IOMUX_AON_R_GPIO_S0_PU_SHIFT                        (0)
#define IOMUX_AON_R_GPIO_S1_PU(n)                           (((n) & 0xFF) << 8)
#define IOMUX_AON_R_GPIO_S1_PU_MASK                         (0xFF << 8)
#define IOMUX_AON_R_GPIO_S1_PU_SHIFT                        (8)
#define IOMUX_AON_R_GPIO_S2_PU(n)                           (((n) & 0xFF) << 16)
#define IOMUX_AON_R_GPIO_S2_PU_MASK                         (0xFF << 16)
#define IOMUX_AON_R_GPIO_S2_PU_SHIFT                        (16)

// reg_28
#define IOMUX_AON_R_GPIO_S0_PD(n)                           (((n) & 0xFF) << 0)
#define IOMUX_AON_R_GPIO_S0_PD_MASK                         (0xFF << 0)
#define IOMUX_AON_R_GPIO_S0_PD_SHIFT                        (0)
#define IOMUX_AON_R_GPIO_S1_PD(n)                           (((n) & 0xFF) << 8)
#define IOMUX_AON_R_GPIO_S1_PD_MASK                         (0xFF << 8)
#define IOMUX_AON_R_GPIO_S1_PD_SHIFT                        (8)
#define IOMUX_AON_R_GPIO_S2_PD(n)                           (((n) & 0xFF) << 16)
#define IOMUX_AON_R_GPIO_S2_PD_MASK                         (0xFF << 16)
#define IOMUX_AON_R_GPIO_S2_PD_SHIFT                        (16)

// reg_2c
#define IOMUX_AON_R_GPIO_P0_PU(n)                           (((n) & 0xFF) << 0)
#define IOMUX_AON_R_GPIO_P0_PU_MASK                         (0xFF << 0)
#define IOMUX_AON_R_GPIO_P0_PU_SHIFT                        (0)
#define IOMUX_AON_R_GPIO_P1_PU(n)                           (((n) & 0xFF) << 8)
#define IOMUX_AON_R_GPIO_P1_PU_MASK                         (0xFF << 8)
#define IOMUX_AON_R_GPIO_P1_PU_SHIFT                        (8)
#define IOMUX_AON_R_GPIO_P2_PU(n)                           (((n) & 0xFF) << 16)
#define IOMUX_AON_R_GPIO_P2_PU_MASK                         (0xFF << 16)
#define IOMUX_AON_R_GPIO_P2_PU_SHIFT                        (16)

// reg_30
#define IOMUX_AON_R_GPIO_P0_PD(n)                           (((n) & 0xFF) << 0)
#define IOMUX_AON_R_GPIO_P0_PD_MASK                         (0xFF << 0)
#define IOMUX_AON_R_GPIO_P0_PD_SHIFT                        (0)
#define IOMUX_AON_R_GPIO_P1_PD(n)                           (((n) & 0xFF) << 8)
#define IOMUX_AON_R_GPIO_P1_PD_MASK                         (0xFF << 8)
#define IOMUX_AON_R_GPIO_P1_PD_SHIFT                        (8)
#define IOMUX_AON_R_GPIO_P2_PD(n)                           (((n) & 0xFF) << 16)
#define IOMUX_AON_R_GPIO_P2_PD_MASK                         (0xFF << 16)
#define IOMUX_AON_R_GPIO_P2_PD_SHIFT                        (16)

// reg_38
#define IOMUX_AON_MCUIO_DB_ACTIVE(n)                        (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_AON_MCUIO_DB_ACTIVE_MASK                      (0xFFFFFFFF << 0)
#define IOMUX_AON_MCUIO_DB_ACTIVE_SHIFT                     (0)

// reg_3c
#define IOMUX_AON_MCUIO_DB_VALUE(n)                         (((n) & 0xFF) << 0)
#define IOMUX_AON_MCUIO_DB_VALUE_MASK                       (0xFF << 0)
#define IOMUX_AON_MCUIO_DB_VALUE_SHIFT                      (0)
#define IOMUX_AON_PWK_DB_VALUE(n)                           (((n) & 0xFF) << 8)
#define IOMUX_AON_PWK_DB_VALUE_MASK                         (0xFF << 8)
#define IOMUX_AON_PWK_DB_VALUE_SHIFT                        (8)
#define IOMUX_AON_PWK_DB_ACTIVE                             (1 << 16)

// reg_40
#define IOMUX_AON_POWER_KEY_ON_INT_STATUS                   (1 << 0)
#define IOMUX_AON_POWER_KEY_OFF_INT_STATUS                  (1 << 1)
#define IOMUX_AON_R_POWER_KEY_INTR_U                        (1 << 2)
#define IOMUX_AON_R_POWER_KEY_INTR_D                        (1 << 3)
#define IOMUX_AON_POWER_KEY_ON_INT_EN                       (1 << 4)
#define IOMUX_AON_POWER_KEY_OFF_INT_EN                      (1 << 5)
#define IOMUX_AON_POWER_KEY_ON_INT_MSK                      (1 << 6)
#define IOMUX_AON_POWER_KEY_OFF_INT_MSK                     (1 << 7)
#define IOMUX_AON_POWER_KEY_DB                              (1 << 30)
#define IOMUX_AON_POWER_ON_FEEDOUT                          (1 << 31)

// reg_44
#define IOMUX_AON_SPI_PMU_SEL(n)                            (((n) & 0x3) << 0)
#define IOMUX_AON_SPI_PMU_SEL_MASK                          (0x3 << 0)
#define IOMUX_AON_SPI_PMU_SEL_SHIFT                         (0)
#define IOMUX_AON_SPI_ANA_SEL(n)                            (((n) & 0x3) << 2)
#define IOMUX_AON_SPI_ANA_SEL_MASK                          (0x3 << 2)
#define IOMUX_AON_SPI_ANA_SEL_SHIFT                         (2)
#define IOMUX_AON_SPI_RF_SEL(n)                             (((n) & 0x3) << 4)
#define IOMUX_AON_SPI_RF_SEL_MASK                           (0x3 << 4)
#define IOMUX_AON_SPI_RF_SEL_SHIFT                          (4)
#define IOMUX_AON_SPI_XTAL_SEL(n)                           (((n) & 0x3) << 6)
#define IOMUX_AON_SPI_XTAL_SEL_MASK                         (0x3 << 6)
#define IOMUX_AON_SPI_XTAL_SEL_SHIFT                        (6)
#define IOMUX_AON_SPI_RC_SEL(n)                             (((n) & 0x3) << 8)
#define IOMUX_AON_SPI_RC_SEL_MASK                           (0x3 << 8)
#define IOMUX_AON_SPI_RC_SEL_SHIFT                          (8)
#define IOMUX_AON_SPI_EMMC_SEL(n)                           (((n) & 0x3) << 10)
#define IOMUX_AON_SPI_EMMC_SEL_MASK                         (0x3 << 10)
#define IOMUX_AON_SPI_EMMC_SEL_SHIFT                        (10)
#define IOMUX_AON_SPI_PCIE_SEL(n)                           (((n) & 0x3) << 12)
#define IOMUX_AON_SPI_PCIE_SEL_MASK                         (0x3 << 12)
#define IOMUX_AON_SPI_PCIE_SEL_SHIFT                        (12)
#define IOMUX_AON_SPI_USB_SEL(n)                            (((n) & 0x3) << 14)
#define IOMUX_AON_SPI_USB_SEL_MASK                          (0x3 << 14)
#define IOMUX_AON_SPI_USB_SEL_SHIFT                         (14)
#define IOMUX_AON_SPI_PMU1_SEL(n)                           (((n) & 0x3) << 16)
#define IOMUX_AON_SPI_PMU1_SEL_MASK                         (0x3 << 16)
#define IOMUX_AON_SPI_PMU1_SEL_SHIFT                        (16)
#define IOMUX_AON_ANA_TEST_SEL(n)                           (((n) & 0xF) << 18)
#define IOMUX_AON_ANA_TEST_SEL_MASK                         (0xF << 18)
#define IOMUX_AON_ANA_TEST_SEL_SHIFT                        (18)

// reg_48
#define IOMUX_AON_GPIO_I2C_MODE                             (1 << 0)
#define IOMUX_AON_I2C0_M_SEL_GPIO                           (1 << 1)
#define IOMUX_AON_I2C1_M_SEL_GPIO                           (1 << 2)
#define IOMUX_AON_BT_RXTX_SW_EN                             (1 << 3)

// reg_4c
#define IOMUX_AON_CFG_CODEC_TRIG_SEL(n)                     (((n) & 0x7) << 0)
#define IOMUX_AON_CFG_CODEC_TRIG_SEL_MASK                   (0x7 << 0)
#define IOMUX_AON_CFG_CODEC_TRIG_SEL_SHIFT                  (0)
#define IOMUX_AON_CFG_CODEC_TRIG_POL                        (1 << 3)
#define IOMUX_AON_CFG_EN_CLK_REQOUT                         (1 << 4)
#define IOMUX_AON_CFG_EN_CLK_REQIN                          (1 << 5)
#define IOMUX_AON_CFG_POL_CLK_REQOUT                        (1 << 6)
#define IOMUX_AON_CFG_POL_CLK_REQIN                         (1 << 7)

// reg_50
#define IOMUX_AON_GPIO_P0_DRV0_SEL(n)                       (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_P0_DRV0_SEL_MASK                     (0xF << 0)
#define IOMUX_AON_GPIO_P0_DRV0_SEL_SHIFT                    (0)
#define IOMUX_AON_GPIO_P0_DRV1_SEL(n)                       (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_P0_DRV1_SEL_MASK                     (0xF << 4)
#define IOMUX_AON_GPIO_P0_DRV1_SEL_SHIFT                    (4)
#define IOMUX_AON_GPIO_P0_DRV2_SEL(n)                       (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_P0_DRV2_SEL_MASK                     (0xF << 8)
#define IOMUX_AON_GPIO_P0_DRV2_SEL_SHIFT                    (8)
#define IOMUX_AON_GPIO_P0_DRV3_SEL(n)                       (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_P0_DRV3_SEL_MASK                     (0xF << 12)
#define IOMUX_AON_GPIO_P0_DRV3_SEL_SHIFT                    (12)
#define IOMUX_AON_GPIO_P0_DRV4_SEL(n)                       (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_P0_DRV4_SEL_MASK                     (0xF << 16)
#define IOMUX_AON_GPIO_P0_DRV4_SEL_SHIFT                    (16)
#define IOMUX_AON_GPIO_P0_DRV5_SEL(n)                       (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_P0_DRV5_SEL_MASK                     (0xF << 20)
#define IOMUX_AON_GPIO_P0_DRV5_SEL_SHIFT                    (20)
#define IOMUX_AON_GPIO_P0_DRV6_SEL(n)                       (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_P0_DRV6_SEL_MASK                     (0xF << 24)
#define IOMUX_AON_GPIO_P0_DRV6_SEL_SHIFT                    (24)
#define IOMUX_AON_GPIO_P0_DRV7_SEL(n)                       (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_P0_DRV7_SEL_MASK                     (0xF << 28)
#define IOMUX_AON_GPIO_P0_DRV7_SEL_SHIFT                    (28)

// reg_54
#define IOMUX_AON_GPIO_P1_DRV0_SEL(n)                       (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_P1_DRV0_SEL_MASK                     (0xF << 0)
#define IOMUX_AON_GPIO_P1_DRV0_SEL_SHIFT                    (0)
#define IOMUX_AON_GPIO_P1_DRV1_SEL(n)                       (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_P1_DRV1_SEL_MASK                     (0xF << 4)
#define IOMUX_AON_GPIO_P1_DRV1_SEL_SHIFT                    (4)
#define IOMUX_AON_GPIO_P1_DRV2_SEL(n)                       (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_P1_DRV2_SEL_MASK                     (0xF << 8)
#define IOMUX_AON_GPIO_P1_DRV2_SEL_SHIFT                    (8)
#define IOMUX_AON_GPIO_P1_DRV3_SEL(n)                       (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_P1_DRV3_SEL_MASK                     (0xF << 12)
#define IOMUX_AON_GPIO_P1_DRV3_SEL_SHIFT                    (12)
#define IOMUX_AON_GPIO_P1_DRV4_SEL(n)                       (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_P1_DRV4_SEL_MASK                     (0xF << 16)
#define IOMUX_AON_GPIO_P1_DRV4_SEL_SHIFT                    (16)
#define IOMUX_AON_GPIO_P1_DRV5_SEL(n)                       (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_P1_DRV5_SEL_MASK                     (0xF << 20)
#define IOMUX_AON_GPIO_P1_DRV5_SEL_SHIFT                    (20)
#define IOMUX_AON_GPIO_P1_DRV6_SEL(n)                       (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_P1_DRV6_SEL_MASK                     (0xF << 24)
#define IOMUX_AON_GPIO_P1_DRV6_SEL_SHIFT                    (24)
#define IOMUX_AON_GPIO_P1_DRV7_SEL(n)                       (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_P1_DRV7_SEL_MASK                     (0xF << 28)
#define IOMUX_AON_GPIO_P1_DRV7_SEL_SHIFT                    (28)

// reg_58
#define IOMUX_AON_GPIO_P2_DRV0_SEL(n)                       (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_P2_DRV0_SEL_MASK                     (0xF << 0)
#define IOMUX_AON_GPIO_P2_DRV0_SEL_SHIFT                    (0)
#define IOMUX_AON_GPIO_P2_DRV1_SEL(n)                       (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_P2_DRV1_SEL_MASK                     (0xF << 4)
#define IOMUX_AON_GPIO_P2_DRV1_SEL_SHIFT                    (4)
#define IOMUX_AON_GPIO_P2_DRV2_SEL(n)                       (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_P2_DRV2_SEL_MASK                     (0xF << 8)
#define IOMUX_AON_GPIO_P2_DRV2_SEL_SHIFT                    (8)
#define IOMUX_AON_GPIO_P2_DRV3_SEL(n)                       (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_P2_DRV3_SEL_MASK                     (0xF << 12)
#define IOMUX_AON_GPIO_P2_DRV3_SEL_SHIFT                    (12)
#define IOMUX_AON_GPIO_P2_DRV4_SEL(n)                       (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_P2_DRV4_SEL_MASK                     (0xF << 16)
#define IOMUX_AON_GPIO_P2_DRV4_SEL_SHIFT                    (16)
#define IOMUX_AON_GPIO_P2_DRV5_SEL(n)                       (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_P2_DRV5_SEL_MASK                     (0xF << 20)
#define IOMUX_AON_GPIO_P2_DRV5_SEL_SHIFT                    (20)
#define IOMUX_AON_GPIO_P2_DRV6_SEL(n)                       (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_P2_DRV6_SEL_MASK                     (0xF << 24)
#define IOMUX_AON_GPIO_P2_DRV6_SEL_SHIFT                    (24)
#define IOMUX_AON_GPIO_P2_DRV7_SEL(n)                       (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_P2_DRV7_SEL_MASK                     (0xF << 28)
#define IOMUX_AON_GPIO_P2_DRV7_SEL_SHIFT                    (28)

// reg_5c
#define IOMUX_AON_GPIO_S0_DRV0_SEL(n)                       (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_S0_DRV0_SEL_MASK                     (0xF << 0)
#define IOMUX_AON_GPIO_S0_DRV0_SEL_SHIFT                    (0)
#define IOMUX_AON_GPIO_S0_DRV1_SEL(n)                       (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_S0_DRV1_SEL_MASK                     (0xF << 4)
#define IOMUX_AON_GPIO_S0_DRV1_SEL_SHIFT                    (4)
#define IOMUX_AON_GPIO_S0_DRV2_SEL(n)                       (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_S0_DRV2_SEL_MASK                     (0xF << 8)
#define IOMUX_AON_GPIO_S0_DRV2_SEL_SHIFT                    (8)
#define IOMUX_AON_GPIO_S0_DRV3_SEL(n)                       (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_S0_DRV3_SEL_MASK                     (0xF << 12)
#define IOMUX_AON_GPIO_S0_DRV3_SEL_SHIFT                    (12)
#define IOMUX_AON_GPIO_S0_DRV4_SEL(n)                       (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_S0_DRV4_SEL_MASK                     (0xF << 16)
#define IOMUX_AON_GPIO_S0_DRV4_SEL_SHIFT                    (16)
#define IOMUX_AON_GPIO_S0_DRV5_SEL(n)                       (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_S0_DRV5_SEL_MASK                     (0xF << 20)
#define IOMUX_AON_GPIO_S0_DRV5_SEL_SHIFT                    (20)
#define IOMUX_AON_GPIO_S0_DRV6_SEL(n)                       (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_S0_DRV6_SEL_MASK                     (0xF << 24)
#define IOMUX_AON_GPIO_S0_DRV6_SEL_SHIFT                    (24)
#define IOMUX_AON_GPIO_S0_DRV7_SEL(n)                       (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_S0_DRV7_SEL_MASK                     (0xF << 28)
#define IOMUX_AON_GPIO_S0_DRV7_SEL_SHIFT                    (28)

// reg_60
#define IOMUX_AON_GPIO_S1_DRV0_SEL(n)                       (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_S1_DRV0_SEL_MASK                     (0xF << 0)
#define IOMUX_AON_GPIO_S1_DRV0_SEL_SHIFT                    (0)
#define IOMUX_AON_GPIO_S1_DRV1_SEL(n)                       (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_S1_DRV1_SEL_MASK                     (0xF << 4)
#define IOMUX_AON_GPIO_S1_DRV1_SEL_SHIFT                    (4)
#define IOMUX_AON_GPIO_S1_DRV2_SEL(n)                       (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_S1_DRV2_SEL_MASK                     (0xF << 8)
#define IOMUX_AON_GPIO_S1_DRV2_SEL_SHIFT                    (8)
#define IOMUX_AON_GPIO_S1_DRV3_SEL(n)                       (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_S1_DRV3_SEL_MASK                     (0xF << 12)
#define IOMUX_AON_GPIO_S1_DRV3_SEL_SHIFT                    (12)
#define IOMUX_AON_GPIO_S1_DRV4_SEL(n)                       (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_S1_DRV4_SEL_MASK                     (0xF << 16)
#define IOMUX_AON_GPIO_S1_DRV4_SEL_SHIFT                    (16)
#define IOMUX_AON_GPIO_S1_DRV5_SEL(n)                       (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_S1_DRV5_SEL_MASK                     (0xF << 20)
#define IOMUX_AON_GPIO_S1_DRV5_SEL_SHIFT                    (20)
#define IOMUX_AON_GPIO_S1_DRV6_SEL(n)                       (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_S1_DRV6_SEL_MASK                     (0xF << 24)
#define IOMUX_AON_GPIO_S1_DRV6_SEL_SHIFT                    (24)
#define IOMUX_AON_GPIO_S1_DRV7_SEL(n)                       (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_S1_DRV7_SEL_MASK                     (0xF << 28)
#define IOMUX_AON_GPIO_S1_DRV7_SEL_SHIFT                    (28)

// reg_64
#define IOMUX_AON_SPI0_WM3                                  (1 << 0)
#define IOMUX_AON_SPI1_WM3                                  (1 << 1)
#define IOMUX_AON_UART0_HALFN_SYS                           (1 << 2)
#define IOMUX_AON_UART0_HALFN_BTC                           (1 << 3)
#define IOMUX_AON_UART0_HALFN_BTH                           (1 << 4)
#define IOMUX_AON_UART1_HALFN_BTH                           (1 << 5)
#define IOMUX_AON_UART0_HALFN                               (1 << 6)
#define IOMUX_AON_UART1_HALFN                               (1 << 7)
#define IOMUX_AON_UART2_HALFN                               (1 << 8)
#define IOMUX_AON_SEL_SPI0_SI1_MUX_SI0                      (1 << 9)
#define IOMUX_AON_SEL_SPI0_SI2_MUX_SI0                      (1 << 10)
#define IOMUX_AON_SEL_SPI1_SI1_MUX_SI0                      (1 << 11)
#define IOMUX_AON_SEL_SPI1_SI2_MUX_SI0                      (1 << 12)
#define IOMUX_AON_SEL_SPI0_SLAVE                            (1 << 13)
#define IOMUX_AON_SEL_SPI1_SLAVE                            (1 << 14)
#define IOMUX_AON_SEL_ACTIVE_WF_AON                         (1 << 15)
#define IOMUX_AON_SEL_LCDTE_AON                             (1 << 16)
#define IOMUX_AON_SPI2_WM3                                  (1 << 17)
#define IOMUX_AON_SPI3_WM3                                  (1 << 18)
#define IOMUX_AON_SPI4_WM3                                  (1 << 19)
#define IOMUX_AON_QSPI_NAND_WM3_0                           (1 << 20)
#define IOMUX_AON_QSPI_NAND_WM3_1                           (1 << 21)
#define IOMUX_AON_QSPI_NAND_WM3_2                           (1 << 22)
#define IOMUX_AON_QSPI_NAND_WM3_3                           (1 << 23)
#define IOMUX_AON_UART1_HALFN_SYS                           (1 << 24)
#define IOMUX_AON_SEL_SPI2_SLAVE                            (1 << 25)
#define IOMUX_AON_SEL_SPI3_SLAVE                            (1 << 26)
#define IOMUX_AON_SEL_SPI4_SLAVE                            (1 << 27)

// reg_68
#define IOMUX_AON_AGPIO_EN(n)                               (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_AON_AGPIO_EN_MASK                             (0xFFFFFFFF << 0)
#define IOMUX_AON_AGPIO_EN_SHIFT                            (0)

// reg_70
#define IOMUX_AON_SPI_INTR_RESETN                           (1 << 0)
#define IOMUX_AON_SPI_INTR_MASK                             (1 << 1)

// reg_74
#define IOMUX_AON_AGPIO_EN1(n)                              (((n) & 0xFFFF) << 0)
#define IOMUX_AON_AGPIO_EN1_MASK                            (0xFFFF << 0)
#define IOMUX_AON_AGPIO_EN1_SHIFT                           (0)

// reg_78
#define IOMUX_AON_MCUIO_DB_ACTIVE1(n)                       (((n) & 0xFFFF) << 0)
#define IOMUX_AON_MCUIO_DB_ACTIVE1_MASK                     (0xFFFF << 0)
#define IOMUX_AON_MCUIO_DB_ACTIVE1_SHIFT                    (0)

// reg_7c
#define IOMUX_AON_GPIO_S2_DRV0_SEL(n)                       (((n) & 0xF) << 0)
#define IOMUX_AON_GPIO_S2_DRV0_SEL_MASK                     (0xF << 0)
#define IOMUX_AON_GPIO_S2_DRV0_SEL_SHIFT                    (0)
#define IOMUX_AON_GPIO_S2_DRV1_SEL(n)                       (((n) & 0xF) << 4)
#define IOMUX_AON_GPIO_S2_DRV1_SEL_MASK                     (0xF << 4)
#define IOMUX_AON_GPIO_S2_DRV1_SEL_SHIFT                    (4)
#define IOMUX_AON_GPIO_S2_DRV2_SEL(n)                       (((n) & 0xF) << 8)
#define IOMUX_AON_GPIO_S2_DRV2_SEL_MASK                     (0xF << 8)
#define IOMUX_AON_GPIO_S2_DRV2_SEL_SHIFT                    (8)
#define IOMUX_AON_GPIO_S2_DRV3_SEL(n)                       (((n) & 0xF) << 12)
#define IOMUX_AON_GPIO_S2_DRV3_SEL_MASK                     (0xF << 12)
#define IOMUX_AON_GPIO_S2_DRV3_SEL_SHIFT                    (12)
#define IOMUX_AON_GPIO_S2_DRV4_SEL(n)                       (((n) & 0xF) << 16)
#define IOMUX_AON_GPIO_S2_DRV4_SEL_MASK                     (0xF << 16)
#define IOMUX_AON_GPIO_S2_DRV4_SEL_SHIFT                    (16)
#define IOMUX_AON_GPIO_S2_DRV5_SEL(n)                       (((n) & 0xF) << 20)
#define IOMUX_AON_GPIO_S2_DRV5_SEL_MASK                     (0xF << 20)
#define IOMUX_AON_GPIO_S2_DRV5_SEL_SHIFT                    (20)
#define IOMUX_AON_GPIO_S2_DRV6_SEL(n)                       (((n) & 0xF) << 24)
#define IOMUX_AON_GPIO_S2_DRV6_SEL_MASK                     (0xF << 24)
#define IOMUX_AON_GPIO_S2_DRV6_SEL_SHIFT                    (24)
#define IOMUX_AON_GPIO_S2_DRV7_SEL(n)                       (((n) & 0xF) << 28)
#define IOMUX_AON_GPIO_S2_DRV7_SEL_MASK                     (0xF << 28)
#define IOMUX_AON_GPIO_S2_DRV7_SEL_SHIFT                    (28)

// reg_80
#define IOMUX_AON_ANA_TEST_DIR(n)                           (((n) & 0xFFFF) << 0)
#define IOMUX_AON_ANA_TEST_DIR_MASK                         (0xFFFF << 0)
#define IOMUX_AON_ANA_TEST_DIR_SHIFT                        (0)

#endif
