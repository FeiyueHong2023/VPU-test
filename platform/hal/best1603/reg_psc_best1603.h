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
#ifndef __REG_PSC_BEST1603_H__
#define __REG_PSC_BEST1603_H__

#include "plat_types.h"

struct AONPSC_T {
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
    __IO uint32_t REG_084;
    __IO uint32_t REG_088;
    __IO uint32_t REG_08C;
    __IO uint32_t REG_090;
    __IO uint32_t REG_094;
    __IO uint32_t REG_098;
    __IO uint32_t REG_09C;
    __IO uint32_t REG_0A0;
    __IO uint32_t REG_0A4;
    __IO uint32_t REG_0A8;
    __IO uint32_t REG_0AC;
    __IO uint32_t REG_0B0;
    __IO uint32_t REG_0B4;
    __IO uint32_t REG_0B8;
    __IO uint32_t REG_0BC;
    __IO uint32_t REG_0C0;
    __IO uint32_t REG_0C4;
    __IO uint32_t REG_0C8;
    __IO uint32_t REG_0CC;
    __IO uint32_t REG_0D0;
    __IO uint32_t REG_0D4;
    __IO uint32_t REG_0D8;
    __IO uint32_t REG_0DC;
    __IO uint32_t REG_0E0;
    __IO uint32_t REG_0E4;
    __IO uint32_t REG_0E8;
    __IO uint32_t REG_0EC;
    __IO uint32_t REG_0F0;
    __IO uint32_t REG_0F4;
    __IO uint32_t REG_0F8;
    __IO uint32_t REG_0FC;
    __IO uint32_t REG_100;
    __IO uint32_t REG_104;
    __IO uint32_t REG_108;
    __IO uint32_t REG_10C;
    __IO uint32_t REG_110;
    __IO uint32_t REG_114;
    __IO uint32_t REG_118;
    __IO uint32_t REG_11C;
    __IO uint32_t REG_120;
    __IO uint32_t REG_124;
    __IO uint32_t REG_128;
    __IO uint32_t REG_12C;
    __IO uint32_t REG_130;
    __IO uint32_t REG_134;
    __IO uint32_t REG_138;
    __IO uint32_t REG_13C;
    __IO uint32_t REG_140;
    __IO uint32_t REG_144;
    __IO uint32_t REG_148;
    __IO uint32_t REG_14C;
    __IO uint32_t REG_150;
    __IO uint32_t REG_154;
    __IO uint32_t REG_158;
    __IO uint32_t REG_15C;
    __IO uint32_t REG_160;
    __IO uint32_t REG_164;
    __IO uint32_t REG_168;
    __IO uint32_t REG_16C;
    __IO uint32_t REG_170;
    __IO uint32_t REG_174;
    __IO uint32_t REG_178;
    __IO uint32_t REG_17C;
    __IO uint32_t REG_180;
    __IO uint32_t REG_184;
    __IO uint32_t REG_188;
    __IO uint32_t REG_18C;
    __IO uint32_t REG_190;
    __IO uint32_t REG_194;
    __IO uint32_t REG_198;
    __IO uint32_t REG_19C;
    __IO uint32_t REG_1A0;
    __IO uint32_t REG_1A4;
    __IO uint32_t REG_1A8;

};

// reg_00
#define PSC_AON_SYS_PG_AUTO_EN                              (1 << 0)
#define PSC_AON_SYS_PG_HW_EN                                (1 << 1)
#define PSC_AON_SYS_BYPASS_PWR_MEM                          (1 << 2)
#define PSC_AON_SYS_BUS_SLP_ENABLE                          (1 << 3)

// reg_04
#define PSC_AON_SYS_PSW_ACK_VALID                           (1 << 0)
#define PSC_AON_SYS_RESERVED(n)                             (((n) & 0x7F) << 1)
#define PSC_AON_SYS_RESERVED_MASK                           (0x7F << 1)
#define PSC_AON_SYS_RESERVED_SHIFT                          (1)
#define PSC_AON_SYS_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_SYS_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_SYS_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_SYS_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_SYS_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_SYS_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_SYS_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_SYS_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_SYS_POWERUP_STATE_SHIFT                     (13)

// reg_08
#define PSC_AON_SYS_POWERDN_TIMER1(n)                       (((n) & 0x3F) << 0)
#define PSC_AON_SYS_POWERDN_TIMER1_MASK                     (0x3F << 0)
#define PSC_AON_SYS_POWERDN_TIMER1_SHIFT                    (0)
#define PSC_AON_SYS_POWERDN_TIMER2(n)                       (((n) & 0x3F) << 6)
#define PSC_AON_SYS_POWERDN_TIMER2_MASK                     (0x3F << 6)
#define PSC_AON_SYS_POWERDN_TIMER2_SHIFT                    (6)
#define PSC_AON_SYS_POWERDN_TIMER3(n)                       (((n) & 0x3F) << 12)
#define PSC_AON_SYS_POWERDN_TIMER3_MASK                     (0x3F << 12)
#define PSC_AON_SYS_POWERDN_TIMER3_SHIFT                    (12)
#define PSC_AON_SYS_POWERDN_TIMER4(n)                       (((n) & 0x3F) << 18)
#define PSC_AON_SYS_POWERDN_TIMER4_MASK                     (0x3F << 18)
#define PSC_AON_SYS_POWERDN_TIMER4_SHIFT                    (18)
#define PSC_AON_SYS_POWERDN_TIMER5(n)                       (((n) & 0xFF) << 24)
#define PSC_AON_SYS_POWERDN_TIMER5_MASK                     (0xFF << 24)
#define PSC_AON_SYS_POWERDN_TIMER5_SHIFT                    (24)

// reg_0c
#define PSC_AON_SYS_POWERUP_TIMER1(n)                       (((n) & 0x3F) << 0)
#define PSC_AON_SYS_POWERUP_TIMER1_MASK                     (0x3F << 0)
#define PSC_AON_SYS_POWERUP_TIMER1_SHIFT                    (0)
#define PSC_AON_SYS_POWERUP_TIMER2(n)                       (((n) & 0xFF) << 6)
#define PSC_AON_SYS_POWERUP_TIMER2_MASK                     (0xFF << 6)
#define PSC_AON_SYS_POWERUP_TIMER2_SHIFT                    (6)
#define PSC_AON_SYS_POWERUP_TIMER3(n)                       (((n) & 0x3F) << 14)
#define PSC_AON_SYS_POWERUP_TIMER3_MASK                     (0x3F << 14)
#define PSC_AON_SYS_POWERUP_TIMER3_SHIFT                    (14)
#define PSC_AON_SYS_POWERUP_TIMER4(n)                       (((n) & 0x3F) << 20)
#define PSC_AON_SYS_POWERUP_TIMER4_MASK                     (0x3F << 20)
#define PSC_AON_SYS_POWERUP_TIMER4_SHIFT                    (20)
#define PSC_AON_SYS_POWERUP_TIMER5(n)                       (((n) & 0x3F) << 26)
#define PSC_AON_SYS_POWERUP_TIMER5_MASK                     (0x3F << 26)
#define PSC_AON_SYS_POWERUP_TIMER5_SHIFT                    (26)

// reg_10
#define PSC_AON_SYS_POWERDN_START                           (1 << 0)

// reg_14
#define PSC_AON_SYS_POWERUP_START                           (1 << 0)

// reg_18
#define PSC_AON_SYS_CLK_STOP_REG                            (1 << 0)
#define PSC_AON_SYS_ISO_EN_REG                              (1 << 1)
#define PSC_AON_SYS_RESETN_ASSERT_REG                       (1 << 2)
#define PSC_AON_SYS_PSW_EN_REG                              (1 << 3)
#define PSC_AON_SYS_CLK_STOP_DR                             (1 << 4)
#define PSC_AON_SYS_ISO_EN_DR                               (1 << 5)
#define PSC_AON_SYS_RESETN_ASSERT_DR                        (1 << 6)
#define PSC_AON_SYS_PSW_EN_DR                               (1 << 7)
#define PSC_AON_SYS_MEM_PSW_EN_REG                          (1 << 8)
#define PSC_AON_SYS_MEM_PSW_EN_DR                           (1 << 9)
#define PSC_AON_SYS_MEM_SD_REG                              (1 << 10)
#define PSC_AON_SYS_MEM_SD_DR                               (1 << 11)
#define PSC_AON_SYS_BUS_SLP_REQ_REG                         (1 << 12)
#define PSC_AON_SYS_BUS_SLP_REQ_DR                          (1 << 13)
#define PSC_AON_SYS_BUS_SLP_ACK_SYNC                        (1 << 14)

#if 0
// reg_1c
#define PSC_AON_SYS_MAIN_STATE(n)                           (((n) & 0x3) << 0)
#define PSC_AON_SYS_MAIN_STATE_MASK                         (0x3 << 0)
#define PSC_AON_SYS_MAIN_STATE_SHIFT                        (0)
#define PSC_AON_SYS_POWERDN_STATE(n)                        (((n) & 0x7) << 2)
#define PSC_AON_SYS_POWERDN_STATE_MASK                      (0x7 << 2)
#define PSC_AON_SYS_POWERDN_STATE_SHIFT                     (2)
#define PSC_AON_SYS_POWERUP_STATE(n)                        (((n) & 0x7) << 5)
#define PSC_AON_SYS_POWERUP_STATE_MASK                      (0x7 << 5)
#define PSC_AON_SYS_POWERUP_STATE_SHIFT                     (5)
#define PSC_AON_BTC_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_BTC_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_BTC_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_BTC_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_BTC_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_BTC_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_BTC_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_BTC_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_BTC_POWERUP_STATE_SHIFT                     (13)
#define PSC_AON_SES_MAIN_STATE(n)                           (((n) & 0x3) << 16)
#define PSC_AON_SES_MAIN_STATE_MASK                         (0x3 << 16)
#define PSC_AON_SES_MAIN_STATE_SHIFT                        (16)
#define PSC_AON_SES_POWERDN_STATE(n)                        (((n) & 0x7) << 18)
#define PSC_AON_SES_POWERDN_STATE_MASK                      (0x7 << 18)
#define PSC_AON_SES_POWERDN_STATE_SHIFT                     (18)
#define PSC_AON_SES_POWERUP_STATE(n)                        (((n) & 0x7) << 21)
#define PSC_AON_SES_POWERUP_STATE_MASK                      (0x7 << 21)
#define PSC_AON_SES_POWERUP_STATE_SHIFT                     (21)
#define PSC_AON_CODEC_MAIN_STATE(n)                         (((n) & 0x3) << 24)
#define PSC_AON_CODEC_MAIN_STATE_MASK                       (0x3 << 24)
#define PSC_AON_CODEC_MAIN_STATE_SHIFT                      (24)
#define PSC_AON_CODEC_POWERDN_STATE(n)                      (((n) & 0x7) << 26)
#define PSC_AON_CODEC_POWERDN_STATE_MASK                    (0x7 << 26)
#define PSC_AON_CODEC_POWERDN_STATE_SHIFT                   (26)
#define PSC_AON_CODEC_POWERUP_STATE(n)                      (((n) & 0x7) << 29)
#define PSC_AON_CODEC_POWERUP_STATE_MASK                    (0x7 << 29)
#define PSC_AON_CODEC_POWERUP_STATE_SHIFT                   (29)
#endif

// reg_20
#define PSC_AON_BTC_PG_AUTO_EN                              (1 << 0)
#define PSC_AON_BTC_PG_HW_EN                                (1 << 1)
#define PSC_AON_BTC_BYPASS_PWR_MEM                          (1 << 2)

// reg_24
#define PSC_AON_BTC_PSW_ACK_VALID                           (1 << 0)
#define PSC_AON_BTC_RESERVED(n)                             (((n) & 0x7F) << 1)
#define PSC_AON_BTC_RESERVED_MASK                           (0x7F << 1)
#define PSC_AON_BTC_RESERVED_SHIFT                          (1)
#define PSC_AON_BTC_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_BTC_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_BTC_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_BTC_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_BTC_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_BTC_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_BTC_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_BTC_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_BTC_POWERUP_STATE_SHIFT                     (13)

#define PSC_AON_BTC_SLEEP_NO_WFI                            (1 << 2)

// reg_28
#define PSC_AON_BTC_POWERDN_START                           (1 << 0)

// reg_2c
#define PSC_AON_BTC_POWERUP_START                           (1 << 0)

// reg_30
#define PSC_AON_BTC_CLK_STOP_REG                            (1 << 0)
#define PSC_AON_BTC_ISO_EN_REG                              (1 << 1)
#define PSC_AON_BTC_RESETN_ASSERT_REG                       (1 << 2)
#define PSC_AON_BTC_PSW_EN_REG                              (1 << 3)
#define PSC_AON_BTC_CLK_STOP_DR                             (1 << 4)
#define PSC_AON_BTC_ISO_EN_DR                               (1 << 5)
#define PSC_AON_BTC_RESETN_ASSERT_DR                        (1 << 6)
#define PSC_AON_BTC_PSW_EN_DR                               (1 << 7)
#define PSC_AON_BTC_MEM_PSW_EN_REG                          (1 << 8)
#define PSC_AON_BTC_MEM_PSW_EN_DR                           (1 << 9)
#define PSC_AON_BTC_MEM_SD_REG                              (1 << 10)
#define PSC_AON_BTC_MEM_SD_DR                               (1 << 11)

// reg_34
#define PSC_AON_BTH_PG_AUTO_EN                              (1 << 0)
#define PSC_AON_BTH_PG_HW_EN                                (1 << 1)
#define PSC_AON_BTH_BYPASS_PWR_MEM                          (1 << 2)

// reg_38
#define PSC_AON_BTH_PSW_ACK_VALID                           (1 << 0)
#define PSC_AON_BTH_RESERVED(n)                             (((n) & 0x7F) << 1)
#define PSC_AON_BTH_RESERVED_MASK                           (0x7F << 1)
#define PSC_AON_BTH_RESERVED_SHIFT                          (1)
#define PSC_AON_BTH_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_BTH_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_BTH_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_BTH_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_BTH_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_BTH_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_BTH_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_BTH_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_BTH_POWERUP_STATE_SHIFT                     (13)

// reg_3c
#define PSC_AON_BTH_POWERDN_START                           (1 << 0)

// reg_40
#define PSC_AON_BTH_POWERUP_START                           (1 << 0)

// reg_44
#define PSC_AON_BTH_CLK_STOP_REG                            (1 << 0)
#define PSC_AON_BTH_ISO_EN_REG                              (1 << 1)
#define PSC_AON_BTH_RESETN_ASSERT_REG                       (1 << 2)
#define PSC_AON_BTH_PSW_EN_REG                              (1 << 3)
#define PSC_AON_BTH_CLK_STOP_DR                             (1 << 4)
#define PSC_AON_BTH_ISO_EN_DR                               (1 << 5)
#define PSC_AON_BTH_RESETN_ASSERT_DR                        (1 << 6)
#define PSC_AON_BTH_PSW_EN_DR                               (1 << 7)
#define PSC_AON_BTH_MEM_PSW_EN_REG                          (1 << 8)
#define PSC_AON_BTH_MEM_PSW_EN_DR                           (1 << 9)
#define PSC_AON_BTH_MEM_SD_REG                              (1 << 10)
#define PSC_AON_BTH_MEM_SD_DR                               (1 << 11)

// reg_48
#define PSC_AON_SES_PG_AUTO_EN                              (1 << 0)
#define PSC_AON_SES_PG_HW_EN                                (1 << 1)
#define PSC_AON_SES_BYPASS_PWR_MEM                          (1 << 2)

// reg_4c
#define PSC_AON_SES_PSW_ACK_VALID                           (1 << 0)
#define PSC_AON_SES_RESERVED(n)                             (((n) & 0x7F) << 1)
#define PSC_AON_SES_RESERVED_MASK                           (0x7F << 1)
#define PSC_AON_SES_RESERVED_SHIFT                          (1)
#define PSC_AON_SES_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_SES_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_SES_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_SES_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_SES_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_SES_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_SES_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_SES_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_SES_POWERUP_STATE_SHIFT                     (13)

// reg_50
#define PSC_AON_SES_POWERDN_START                           (1 << 0)

// reg_54
#define PSC_AON_SES_POWERUP_START                           (1 << 0)

// reg_58
#define PSC_AON_SES_CLK_STOP_REG                            (1 << 0)
#define PSC_AON_SES_ISO_EN_REG                              (1 << 1)
#define PSC_AON_SES_RESETN_ASSERT_REG                       (1 << 2)
#define PSC_AON_SES_PSW_EN_REG                              (1 << 3)
#define PSC_AON_SES_CLK_STOP_DR                             (1 << 4)
#define PSC_AON_SES_ISO_EN_DR                               (1 << 5)
#define PSC_AON_SES_RESETN_ASSERT_DR                        (1 << 6)
#define PSC_AON_SES_PSW_EN_DR                               (1 << 7)
#define PSC_AON_SES_MEM_PSW_EN_REG                          (1 << 8)
#define PSC_AON_SES_MEM_PSW_EN_DR                           (1 << 9)
#define PSC_AON_SES_MEM_SD_REG                              (1 << 10)
#define PSC_AON_SES_MEM_SD_DR                               (1 << 11)

// reg_5c
#define PSC_AON_CODEC_PG_AUTO_EN                            (1 << 0)
#define PSC_AON_CODEC_BYPASS_PWR_MEM                        (1 << 1)

// reg_60
#define PSC_AON_CODEC_PSW_ACK_VALID                         (1 << 0)
#define PSC_AON_CODEC_RESERVED(n)                           (((n) & 0x7F) << 1)
#define PSC_AON_CODEC_RESERVED_MASK                         (0x7F << 1)
#define PSC_AON_CODEC_RESERVED_SHIFT                        (1)
#define PSC_AON_CODEC_MAIN_STATE(n)                         (((n) & 0x3) << 8)
#define PSC_AON_CODEC_MAIN_STATE_MASK                       (0x3 << 8)
#define PSC_AON_CODEC_MAIN_STATE_SHIFT                      (8)
#define PSC_AON_CODEC_POWERDN_STATE(n)                      (((n) & 0x7) << 10)
#define PSC_AON_CODEC_POWERDN_STATE_MASK                    (0x7 << 10)
#define PSC_AON_CODEC_POWERDN_STATE_SHIFT                   (10)
#define PSC_AON_CODEC_POWERUP_STATE(n)                      (((n) & 0x7) << 13)
#define PSC_AON_CODEC_POWERUP_STATE_MASK                    (0x7 << 13)
#define PSC_AON_CODEC_POWERUP_STATE_SHIFT                   (13)

// reg_64
#define PSC_AON_CODEC_POWERDN_START                         (1 << 0)

// reg_68
#define PSC_AON_CODEC_POWERUP_START                         (1 << 0)

// reg_6c
#define PSC_AON_CODEC_CLK_STOP_REG                          (1 << 0)
#define PSC_AON_CODEC_ISO_EN_REG                            (1 << 1)
#define PSC_AON_CODEC_RESETN_ASSERT_REG                     (1 << 2)
#define PSC_AON_CODEC_PSW_EN_REG                            (1 << 3)
#define PSC_AON_CODEC_CLK_STOP_DR                           (1 << 4)
#define PSC_AON_CODEC_ISO_EN_DR                             (1 << 5)
#define PSC_AON_CODEC_RESETN_ASSERT_DR                      (1 << 6)
#define PSC_AON_CODEC_PSW_EN_DR                             (1 << 7)
#define PSC_AON_CODEC_MEM_PSW_EN_REG                        (1 << 8)
#define PSC_AON_CODEC_MEM_PSW_EN_DR                         (1 << 9)
#define PSC_AON_CODEC_MEM_SD_REG                            (1 << 10)
#define PSC_AON_CODEC_MEM_SD_DR                             (1 << 11)

// reg_70
#define PSC_AON_SESCPU_PG_AUTO_EN                           (1 << 0)
#define PSC_AON_SESCPU_PG_HW_EN                             (1 << 1)

// reg_74
#define PSC_AON_SESCPU_PSW_ACK_VALID                        (1 << 0)
#define PSC_AON_SESCPU_RESERVED(n)                          (((n) & 0x7F) << 1)
#define PSC_AON_SESCPU_RESERVED_MASK                        (0x7F << 1)
#define PSC_AON_SESCPU_RESERVED_SHIFT                       (1)
#define PSC_AON_SESCPU_MAIN_STATE(n)                        (((n) & 0x3) << 8)
#define PSC_AON_SESCPU_MAIN_STATE_MASK                      (0x3 << 8)
#define PSC_AON_SESCPU_MAIN_STATE_SHIFT                     (8)
#define PSC_AON_SESCPU_POWERDN_STATE(n)                     (((n) & 0x7) << 10)
#define PSC_AON_SESCPU_POWERDN_STATE_MASK                   (0x7 << 10)
#define PSC_AON_SESCPU_POWERDN_STATE_SHIFT                  (10)
#define PSC_AON_SESCPU_POWERUP_STATE(n)                     (((n) & 0x7) << 13)
#define PSC_AON_SESCPU_POWERUP_STATE_MASK                   (0x7 << 13)
#define PSC_AON_SESCPU_POWERUP_STATE_SHIFT                  (13)
#define PSC_AON_SESCPU_INT_MASK                             (1 << 16)

// reg_78
#define PSC_AON_SESCPU_POWERDN_START                        (1 << 0)

// reg_7c
#define PSC_AON_SESCPU_POWERUP_START                        (1 << 0)

// reg_80
#define PSC_AON_SESCPU_CLK_STOP_REG                         (1 << 0)
#define PSC_AON_SESCPU_ISO_EN_REG                           (1 << 1)
#define PSC_AON_SESCPU_RESETN_ASSERT_REG                    (1 << 2)
#define PSC_AON_SESCPU_PSW_EN_REG                           (1 << 3)
#define PSC_AON_SESCPU_CLK_STOP_DR                          (1 << 4)
#define PSC_AON_SESCPU_ISO_EN_DR                            (1 << 5)
#define PSC_AON_SESCPU_RESETN_ASSERT_DR                     (1 << 6)
#define PSC_AON_SESCPU_PSW_EN_DR                            (1 << 7)

// reg_84
#define PSC_AON_SYSCPU0_PG_AUTO_EN                          (1 << 0)
#define PSC_AON_SYSCPU0_PG_HW_EN                            (1 << 1)
#define PSC_AON_SYSCPU0_BYPASS_PWR_MEM                      (1 << 2)
#define PSC_AON_SYSCPU0_BUS_SLP_ENABLE                      (1 << 3)

// reg_88
#define PSC_AON_SYSCPU0_PSW_ACK_VALID                       (1 << 0)
#define PSC_AON_SYSCPU0_RESERVED(n)                         (((n) & 0x7F) << 1)
#define PSC_AON_SYSCPU0_RESERVED_MASK                       (0x7F << 1)
#define PSC_AON_SYSCPU0_RESERVED_SHIFT                      (1)
#define PSC_AON_SYSCPU0_MAIN_STATE(n)                       (((n) & 0x3) << 8)
#define PSC_AON_SYSCPU0_MAIN_STATE_MASK                     (0x3 << 8)
#define PSC_AON_SYSCPU0_MAIN_STATE_SHIFT                    (8)
#define PSC_AON_SYSCPU0_POWERDN_STATE(n)                    (((n) & 0x7) << 10)
#define PSC_AON_SYSCPU0_POWERDN_STATE_MASK                  (0x7 << 10)
#define PSC_AON_SYSCPU0_POWERDN_STATE_SHIFT                 (10)
#define PSC_AON_SYSCPU0_POWERUP_STATE(n)                    (((n) & 0x7) << 13)
#define PSC_AON_SYSCPU0_POWERUP_STATE_MASK                  (0x7 << 13)
#define PSC_AON_SYSCPU0_POWERUP_STATE_SHIFT                 (13)
#define PSC_AON_SYSCPU0_INT_MASK                            (1 << 16)

// reg_8c
#define PSC_AON_SYSCPU0_POWERDN_START                       (1 << 0)

// reg_90
#define PSC_AON_SYSCPU0_POWERUP_START                       (1 << 0)

// reg_94
#define PSC_AON_SYSCPU0_CLK_STOP_REG                        (1 << 0)
#define PSC_AON_SYSCPU0_ISO_EN_REG                          (1 << 1)
#define PSC_AON_SYSCPU0_RESETN_ASSERT_REG                   (1 << 2)
#define PSC_AON_SYSCPU0_PSW_EN_REG                          (1 << 3)
#define PSC_AON_SYSCPU0_CLK_STOP_DR                         (1 << 4)
#define PSC_AON_SYSCPU0_ISO_EN_DR                           (1 << 5)
#define PSC_AON_SYSCPU0_RESETN_ASSERT_DR                    (1 << 6)
#define PSC_AON_SYSCPU0_PSW_EN_DR                           (1 << 7)
#define PSC_AON_SYSCPU0_MEM_PSW_EN_REG                      (1 << 8)
#define PSC_AON_SYSCPU0_MEM_PSW_EN_DR                       (1 << 9)
#define PSC_AON_SYSCPU0_MEM_SD_REG                          (1 << 10)
#define PSC_AON_SYSCPU0_MEM_SD_DR                           (1 << 11)
#define PSC_AON_SYSCPU0_BUS_SLP_REQ_REG                     (1 << 12)
#define PSC_AON_SYSCPU0_BUS_SLP_REQ_DR                      (1 << 13)
#define PSC_AON_SYSCPU0_BUS_SLP_ACK_SYNC                    (1 << 14)

// reg_98
#define PSC_AON_SYSCPU1_PG_AUTO_EN                          (1 << 0)
#define PSC_AON_SYSCPU1_PG_HW_EN                            (1 << 1)
#define PSC_AON_SYSCPU1_BYPASS_PWR_MEM                      (1 << 2)
#define PSC_AON_SYSCPU1_BUS_SLP_ENABLE                      (1 << 3)

// reg_9c
#define PSC_AON_SYSCPU1_PSW_ACK_VALID                       (1 << 0)
#define PSC_AON_SYSCPU1_RESERVED(n)                         (((n) & 0x7F) << 1)
#define PSC_AON_SYSCPU1_RESERVED_MASK                       (0x7F << 1)
#define PSC_AON_SYSCPU1_RESERVED_SHIFT                      (1)
#define PSC_AON_SYSCPU1_MAIN_STATE(n)                       (((n) & 0x3) << 8)
#define PSC_AON_SYSCPU1_MAIN_STATE_MASK                     (0x3 << 8)
#define PSC_AON_SYSCPU1_MAIN_STATE_SHIFT                    (8)
#define PSC_AON_SYSCPU1_POWERDN_STATE(n)                    (((n) & 0x7) << 10)
#define PSC_AON_SYSCPU1_POWERDN_STATE_MASK                  (0x7 << 10)
#define PSC_AON_SYSCPU1_POWERDN_STATE_SHIFT                 (10)
#define PSC_AON_SYSCPU1_POWERUP_STATE(n)                    (((n) & 0x7) << 13)
#define PSC_AON_SYSCPU1_POWERUP_STATE_MASK                  (0x7 << 13)
#define PSC_AON_SYSCPU1_POWERUP_STATE_SHIFT                 (13)
#define PSC_AON_SYSCPU1_INT_MASK                            (1 << 16)

// reg_a0
#define PSC_AON_SYSCPU1_POWERDN_START                       (1 << 0)

// reg_a4
#define PSC_AON_SYSCPU1_POWERUP_START                       (1 << 0)

// reg_a8
#define PSC_AON_SYSCPU1_CLK_STOP_REG                        (1 << 0)
#define PSC_AON_SYSCPU1_ISO_EN_REG                          (1 << 1)
#define PSC_AON_SYSCPU1_RESETN_ASSERT_REG                   (1 << 2)
#define PSC_AON_SYSCPU1_PSW_EN_REG                          (1 << 3)
#define PSC_AON_SYSCPU1_CLK_STOP_DR                         (1 << 4)
#define PSC_AON_SYSCPU1_ISO_EN_DR                           (1 << 5)
#define PSC_AON_SYSCPU1_RESETN_ASSERT_DR                    (1 << 6)
#define PSC_AON_SYSCPU1_PSW_EN_DR                           (1 << 7)
#define PSC_AON_SYSCPU1_MEM_PSW_EN_REG                      (1 << 8)
#define PSC_AON_SYSCPU1_MEM_PSW_EN_DR                       (1 << 9)
#define PSC_AON_SYSCPU1_MEM_SD_REG                          (1 << 10)
#define PSC_AON_SYSCPU1_MEM_SD_DR                           (1 << 11)
#define PSC_AON_SYSCPU1_BUS_SLP_REQ_REG                     (1 << 12)
#define PSC_AON_SYSCPU1_BUS_SLP_REQ_DR                      (1 << 13)
#define PSC_AON_SYSCPU1_BUS_SLP_ACK_SYNC                    (1 << 14)

// reg_ac
#define PSC_AON_DIS_PG_AUTO_EN                              (1 << 0)
#define PSC_AON_DIS_PG_HW_EN                                (1 << 1)
#define PSC_AON_DIS_BYPASS_PWR_MEM                          (1 << 2)

// reg_b0
#define PSC_AON_DIS_PSW_ACK_VALID                           (1 << 0)
#define PSC_AON_DIS_RESERVED(n)                             (((n) & 0x7F) << 1)
#define PSC_AON_DIS_RESERVED_MASK                           (0x7F << 1)
#define PSC_AON_DIS_RESERVED_SHIFT                          (1)
#define PSC_AON_DIS_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_DIS_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_DIS_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_DIS_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_DIS_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_DIS_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_DIS_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_DIS_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_DIS_POWERUP_STATE_SHIFT                     (13)

// reg_b4
#define PSC_AON_DIS_POWERDN_START                           (1 << 0)

// reg_b8
#define PSC_AON_DIS_POWERUP_START                           (1 << 0)

// reg_bc
#define PSC_AON_DIS_CLK_STOP_REG                            (1 << 0)
#define PSC_AON_DIS_ISO_EN_REG                              (1 << 1)
#define PSC_AON_DIS_RESETN_ASSERT_REG                       (1 << 2)
#define PSC_AON_DIS_PSW_EN_REG                              (1 << 3)
#define PSC_AON_DIS_CLK_STOP_DR                             (1 << 4)
#define PSC_AON_DIS_ISO_EN_DR                               (1 << 5)
#define PSC_AON_DIS_RESETN_ASSERT_DR                        (1 << 6)
#define PSC_AON_DIS_PSW_EN_DR                               (1 << 7)
#define PSC_AON_DIS_MEM_PSW_EN_REG                          (1 << 8)
#define PSC_AON_DIS_MEM_PSW_EN_DR                           (1 << 9)
#define PSC_AON_DIS_MEM_SD_REG                              (1 << 10)
#define PSC_AON_DIS_MEM_SD_DR                               (1 << 11)

// reg_c0
#define PSC_AON_PSRAM_PG_AUTO_EN                            (1 << 0)
#define PSC_AON_PSRAM_PG_HW_EN                              (1 << 1)

// reg_c4
#define PSC_AON_PSRAM_PSW_ACK_VALID                         (1 << 0)
#define PSC_AON_PSRAM_RESERVED(n)                           (((n) & 0x7F) << 1)
#define PSC_AON_PSRAM_RESERVED_MASK                         (0x7F << 1)
#define PSC_AON_PSRAM_RESERVED_SHIFT                        (1)
#define PSC_AON_PSRAM_MAIN_STATE(n)                         (((n) & 0x3) << 8)
#define PSC_AON_PSRAM_MAIN_STATE_MASK                       (0x3 << 8)
#define PSC_AON_PSRAM_MAIN_STATE_SHIFT                      (8)
#define PSC_AON_PSRAM_POWERDN_STATE(n)                      (((n) & 0x7) << 10)
#define PSC_AON_PSRAM_POWERDN_STATE_MASK                    (0x7 << 10)
#define PSC_AON_PSRAM_POWERDN_STATE_SHIFT                   (10)
#define PSC_AON_PSRAM_POWERUP_STATE(n)                      (((n) & 0x7) << 13)
#define PSC_AON_PSRAM_POWERUP_STATE_MASK                    (0x7 << 13)
#define PSC_AON_PSRAM_POWERUP_STATE_SHIFT                   (13)

// reg_c8
#define PSC_AON_PSRAM_POWERDN_START                         (1 << 0)

// reg_cc
#define PSC_AON_PSRAM_POWERUP_START                         (1 << 0)

// reg_d0
#define PSC_AON_PSRAM_CLK_STOP_REG                          (1 << 0)
#define PSC_AON_PSRAM_ISO_EN_REG                            (1 << 1)
#define PSC_AON_PSRAM_RESETN_ASSERT_REG                     (1 << 2)
#define PSC_AON_PSRAM_PSW_EN_REG                            (1 << 3)
#define PSC_AON_PSRAM_CLK_STOP_DR                           (1 << 4)
#define PSC_AON_PSRAM_ISO_EN_DR                             (1 << 5)
#define PSC_AON_PSRAM_RESETN_ASSERT_DR                      (1 << 6)
#define PSC_AON_PSRAM_PSW_EN_DR                             (1 << 7)

// reg_d4
#define PSC_AON_STORE_PG_AUTO_EN                            (1 << 0)
#define PSC_AON_STORE_PG_HW_EN                              (1 << 1)
#define PSC_AON_STORE_BYPASS_PWR_MEM                        (1 << 2)

// reg_d8
#define PSC_AON_STORE_PSW_ACK_VALID                         (1 << 0)
#define PSC_AON_STORE_RESERVED(n)                           (((n) & 0x7F) << 1)
#define PSC_AON_STORE_RESERVED_MASK                         (0x7F << 1)
#define PSC_AON_STORE_RESERVED_SHIFT                        (1)
#define PSC_AON_STORE_MAIN_STATE(n)                         (((n) & 0x3) << 8)
#define PSC_AON_STORE_MAIN_STATE_MASK                       (0x3 << 8)
#define PSC_AON_STORE_MAIN_STATE_SHIFT                      (8)
#define PSC_AON_STORE_POWERDN_STATE(n)                      (((n) & 0x7) << 10)
#define PSC_AON_STORE_POWERDN_STATE_MASK                    (0x7 << 10)
#define PSC_AON_STORE_POWERDN_STATE_SHIFT                   (10)
#define PSC_AON_STORE_POWERUP_STATE(n)                      (((n) & 0x7) << 13)
#define PSC_AON_STORE_POWERUP_STATE_MASK                    (0x7 << 13)
#define PSC_AON_STORE_POWERUP_STATE_SHIFT                   (13)

// reg_dc
#define PSC_AON_STORE_POWERDN_START                         (1 << 0)

// reg_e0
#define PSC_AON_STORE_POWERUP_START                         (1 << 0)

// reg_e4
#define PSC_AON_STORE_CLK_STOP_REG                          (1 << 0)
#define PSC_AON_STORE_ISO_EN_REG                            (1 << 1)
#define PSC_AON_STORE_RESETN_ASSERT_REG                     (1 << 2)
#define PSC_AON_STORE_PSW_EN_REG                            (1 << 3)
#define PSC_AON_STORE_CLK_STOP_DR                           (1 << 4)
#define PSC_AON_STORE_ISO_EN_DR                             (1 << 5)
#define PSC_AON_STORE_RESETN_ASSERT_DR                      (1 << 6)
#define PSC_AON_STORE_PSW_EN_DR                             (1 << 7)
#define PSC_AON_STORE_MEM_PSW_EN_REG                        (1 << 8)
#define PSC_AON_STORE_MEM_PSW_EN_DR                         (1 << 9)
#define PSC_AON_STORE_MEM_SD_REG                            (1 << 10)
#define PSC_AON_STORE_MEM_SD_DR                             (1 << 11)

// reg_e8
#define PSC_AON_BLK_PG_AUTO_EN                              (1 << 0)
#define PSC_AON_BLK_POWERDN_HW_EN                           (1 << 1)
#define PSC_AON_BLK_POWERUP_HW_EN                           (1 << 2)

// reg_ec
#define PSC_AON_BLK_PSW_ACK_VALID                           (1 << 0)
#define PSC_AON_BLK_RESERVED(n)                             (((n) & 0x7F) << 1)
#define PSC_AON_BLK_RESERVED_MASK                           (0x7F << 1)
#define PSC_AON_BLK_RESERVED_SHIFT                          (1)
#define PSC_AON_BLK_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_BLK_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_BLK_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_BLK_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_BLK_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_BLK_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_BLK_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_BLK_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_BLK_POWERUP_STATE_SHIFT                     (13)

// reg_f0
#define PSC_AON_BLK_POWERDN_START                           (1 << 0)

// reg_f4
#define PSC_AON_BLK_POWERUP_START                           (1 << 0)

// reg_f8
#define PSC_AON_BLK_CLK_STOP_REG                            (1 << 0)
#define PSC_AON_BLK_ISO_EN_REG                              (1 << 1)
#define PSC_AON_BLK_RESETN_ASSERT_REG                       (1 << 2)
#define PSC_AON_BLK_PSW_EN_REG                              (1 << 3)
#define PSC_AON_BLK_CLK_STOP_DR                             (1 << 4)
#define PSC_AON_BLK_ISO_EN_DR                               (1 << 5)
#define PSC_AON_BLK_RESETN_ASSERT_DR                        (1 << 6)
#define PSC_AON_BLK_PSW_EN_DR                               (1 << 7)

// reg_fc
#define PSC_AON_SYS_GPIO_INTR_MASK(n)                       (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK_MASK                     (0xFFFFFFFF << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK_SHIFT                    (0)

// reg_100
#define PSC_AON_SYS_WAKEUP_INTR_MASK(n)                     (((n) & 0xFFFF) << 0)
#define PSC_AON_SYS_WAKEUP_INTR_MASK_MASK                   (0xFFFF << 0)
#define PSC_AON_SYS_WAKEUP_INTR_MASK_SHIFT                  (0)

// reg_108
#define PSC_AON_SYS_GPIO_INTR_MASK_STATUS(n)                (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK_STATUS_MASK              (0xFFFFFFFF << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK_STATUS_SHIFT             (0)

// reg_10c
#define PSC_AON_SYS_WAKEUP_INTR_MASK_STATUS(n)              (((n) & 0xFFFF) << 0)
#define PSC_AON_SYS_WAKEUP_INTR_MASK_STATUS_MASK            (0xFFFF << 0)
#define PSC_AON_SYS_WAKEUP_INTR_MASK_STATUS_SHIFT           (0)

// reg_114
#define PSC_AON_BTC_GPIO_INTR_MASK(n)                       (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK_MASK                     (0xFFFFFFFF << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK_SHIFT                    (0)

// reg_118
#define PSC_AON_BTC_WAKEUP_INTR_MASK(n)                     (((n) & 0xFFFF) << 0)
#define PSC_AON_BTC_WAKEUP_INTR_MASK_MASK                   (0xFFFF << 0)
#define PSC_AON_BTC_WAKEUP_INTR_MASK_SHIFT                  (0)

// reg_120
#define PSC_AON_BTC_GPIO_INTR_MASK_STATUS(n)                (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK_STATUS_MASK              (0xFFFFFFFF << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK_STATUS_SHIFT             (0)

// reg_124
#define PSC_AON_BTC_WAKEUP_INTR_MASK_STATUS(n)              (((n) & 0xFFFF) << 0)
#define PSC_AON_BTC_WAKEUP_INTR_MASK_STATUS_MASK            (0xFFFF << 0)
#define PSC_AON_BTC_WAKEUP_INTR_MASK_STATUS_SHIFT           (0)

// reg_12c
#define PSC_AON_BTH_GPIO_INTR_MASK(n)                       (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK_MASK                     (0xFFFFFFFF << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK_SHIFT                    (0)

// reg_130
#define PSC_AON_BTH_WAKEUP_INTR_MASK(n)                     (((n) & 0xFFFF) << 0)
#define PSC_AON_BTH_WAKEUP_INTR_MASK_MASK                   (0xFFFF << 0)
#define PSC_AON_BTH_WAKEUP_INTR_MASK_SHIFT                  (0)

// reg_138
#define PSC_AON_BTH_GPIO_INTR_MASK_STATUS(n)                (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK_STATUS_MASK              (0xFFFFFFFF << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK_STATUS_SHIFT             (0)

// reg_13c
#define PSC_AON_BTH_WAKEUP_INTR_MASK_STATUS(n)              (((n) & 0xFFFF) << 0)
#define PSC_AON_BTH_WAKEUP_INTR_MASK_STATUS_MASK            (0xFFFF << 0)
#define PSC_AON_BTH_WAKEUP_INTR_MASK_STATUS_SHIFT           (0)

// reg_144
#define PSC_AON_SES_GPIO_INTR_MASK(n)                       (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_SES_GPIO_INTR_MASK_MASK                     (0xFFFFFFFF << 0)
#define PSC_AON_SES_GPIO_INTR_MASK_SHIFT                    (0)

// reg_148
#define PSC_AON_SES_WAKEUP_INTR_MASK(n)                     (((n) & 0xFFFF) << 0)
#define PSC_AON_SES_WAKEUP_INTR_MASK_MASK                   (0xFFFF << 0)
#define PSC_AON_SES_WAKEUP_INTR_MASK_SHIFT                  (0)

// reg_150
#define PSC_AON_SES_GPIO_INTR_MASK_STATUS(n)                (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_SES_GPIO_INTR_MASK_STATUS_MASK              (0xFFFFFFFF << 0)
#define PSC_AON_SES_GPIO_INTR_MASK_STATUS_SHIFT             (0)

// reg_154
#define PSC_AON_SES_WAKEUP_INTR_MASK_STATUS(n)              (((n) & 0xFFFF) << 0)
#define PSC_AON_SES_WAKEUP_INTR_MASK_STATUS_MASK            (0xFFFF << 0)
#define PSC_AON_SES_WAKEUP_INTR_MASK_STATUS_SHIFT           (0)

// reg_15c
#define PSC_AON_GPIO_INTR_RAW_STATUS(n)                     (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_GPIO_INTR_RAW_STATUS_MASK                   (0xFFFFFFFF << 0)
#define PSC_AON_GPIO_INTR_RAW_STATUS_SHIFT                  (0)

// reg_160
#define PSC_AON_WAKEUP_INTR_RAW_STATUS(n)                   (((n) & 0xFFFF) << 0)
#define PSC_AON_WAKEUP_INTR_RAW_STATUS_MASK                 (0xFFFF << 0)
#define PSC_AON_WAKEUP_INTR_RAW_STATUS_SHIFT                (0)

// reg_168
#define PSC_AON_SRAM0_CE_PSW_EN_REG                         (1 << 0)
#define PSC_AON_SRAM1_CE_PSW_EN_REG                         (1 << 1)
#define PSC_AON_SRAM2_CE_PSW_EN_REG                         (1 << 2)
#define PSC_AON_SRAM3_CE_PSW_EN_REG                         (1 << 3)
#define PSC_AON_SRAM4_CE_PSW_EN_REG                         (1 << 4)
#define PSC_AON_SRAM5_CE_PSW_EN_REG                         (1 << 5)
#define PSC_AON_SRAM6_CE_PSW_EN_REG                         (1 << 6)
#define PSC_AON_SRAM7_CE_PSW_EN_REG                         (1 << 7)
#define PSC_AON_SRAM8_CE_PSW_EN_REG                         (1 << 8)
#define PSC_AON_SRAM9_CE_PSW_EN_REG                         (1 << 9)
#define PSC_AON_SRAM10_CE_PSW_EN_REG                        (1 << 10)
#define PSC_AON_SRAM0_CE_PSW_EN_DR                          (1 << 11)
#define PSC_AON_SRAM1_CE_PSW_EN_DR                          (1 << 12)
#define PSC_AON_SRAM2_CE_PSW_EN_DR                          (1 << 13)
#define PSC_AON_SRAM3_CE_PSW_EN_DR                          (1 << 14)
#define PSC_AON_SRAM4_CE_PSW_EN_DR                          (1 << 15)
#define PSC_AON_SRAM5_CE_PSW_EN_DR                          (1 << 16)
#define PSC_AON_SRAM6_CE_PSW_EN_DR                          (1 << 17)
#define PSC_AON_SRAM7_CE_PSW_EN_DR                          (1 << 18)
#define PSC_AON_SRAM8_CE_PSW_EN_DR                          (1 << 19)
#define PSC_AON_SRAM9_CE_PSW_EN_DR                          (1 << 20)
#define PSC_AON_SRAM10_CE_PSW_EN_DR                         (1 << 21)
#define PSC_AON_SRAM11_CE_PSW_EN_REG                        (1 << 22)
#define PSC_AON_SRAM12_CE_PSW_EN_REG                        (1 << 23)
#define PSC_AON_SRAM11_CE_PSW_EN_DR                         (1 << 24)
#define PSC_AON_SRAM12_CE_PSW_EN_DR                         (1 << 25)

// reg_16c
#define PSC_AON_SRAM0_PE_PSW_EN_REG                         (1 << 0)
#define PSC_AON_SRAM1_PE_PSW_EN_REG                         (1 << 1)
#define PSC_AON_SRAM2_PE_PSW_EN_REG                         (1 << 2)
#define PSC_AON_SRAM3_PE_PSW_EN_REG                         (1 << 3)
#define PSC_AON_SRAM4_PE_PSW_EN_REG                         (1 << 4)
#define PSC_AON_SRAM5_PE_PSW_EN_REG                         (1 << 5)
#define PSC_AON_SRAM6_PE_PSW_EN_REG                         (1 << 6)
#define PSC_AON_SRAM7_PE_PSW_EN_REG                         (1 << 7)
#define PSC_AON_SRAM8_PE_PSW_EN_REG                         (1 << 8)
#define PSC_AON_SRAM9_PE_PSW_EN_REG                         (1 << 9)
#define PSC_AON_SRAM10_PE_PSW_EN_REG                        (1 << 10)
#define PSC_AON_SRAM0_PE_PSW_EN_DR                          (1 << 11)
#define PSC_AON_SRAM1_PE_PSW_EN_DR                          (1 << 12)
#define PSC_AON_SRAM2_PE_PSW_EN_DR                          (1 << 13)
#define PSC_AON_SRAM3_PE_PSW_EN_DR                          (1 << 14)
#define PSC_AON_SRAM4_PE_PSW_EN_DR                          (1 << 15)
#define PSC_AON_SRAM5_PE_PSW_EN_DR                          (1 << 16)
#define PSC_AON_SRAM6_PE_PSW_EN_DR                          (1 << 17)
#define PSC_AON_SRAM7_PE_PSW_EN_DR                          (1 << 18)
#define PSC_AON_SRAM8_PE_PSW_EN_DR                          (1 << 19)
#define PSC_AON_SRAM9_PE_PSW_EN_DR                          (1 << 20)
#define PSC_AON_SRAM10_PE_PSW_EN_DR                         (1 << 21)

// reg_170
#define PSC_AON_SRAM0_ISO_EN_REG                            (1 << 0)
#define PSC_AON_SRAM1_ISO_EN_REG                            (1 << 1)
#define PSC_AON_SRAM2_ISO_EN_REG                            (1 << 2)
#define PSC_AON_SRAM3_ISO_EN_REG                            (1 << 3)
#define PSC_AON_SRAM4_ISO_EN_REG                            (1 << 4)
#define PSC_AON_SRAM5_ISO_EN_REG                            (1 << 5)
#define PSC_AON_SRAM6_ISO_EN_REG                            (1 << 6)
#define PSC_AON_SRAM7_ISO_EN_REG                            (1 << 7)
#define PSC_AON_SRAM8_ISO_EN_REG                            (1 << 8)
#define PSC_AON_SRAM9_ISO_EN_REG                            (1 << 9)
#define PSC_AON_SRAM10_ISO_EN_REG                           (1 << 10)
#define PSC_AON_SRAM0_ISO_EN_DR                             (1 << 11)
#define PSC_AON_SRAM1_ISO_EN_DR                             (1 << 12)
#define PSC_AON_SRAM2_ISO_EN_DR                             (1 << 13)
#define PSC_AON_SRAM3_ISO_EN_DR                             (1 << 14)
#define PSC_AON_SRAM4_ISO_EN_DR                             (1 << 15)
#define PSC_AON_SRAM5_ISO_EN_DR                             (1 << 16)
#define PSC_AON_SRAM6_ISO_EN_DR                             (1 << 17)
#define PSC_AON_SRAM7_ISO_EN_DR                             (1 << 18)
#define PSC_AON_SRAM8_ISO_EN_DR                             (1 << 19)
#define PSC_AON_SRAM9_ISO_EN_DR                             (1 << 20)
#define PSC_AON_SRAM10_ISO_EN_DR                            (1 << 21)

// reg_174
#define PSC_AON_SYS_RETMEM0_PDEN                            (1 << 0)
#define PSC_AON_SYS_RETMEM1_PDEN                            (1 << 1)
#define PSC_AON_BTC_RETMEM_PDEN                             (1 << 2)
#define PSC_AON_BTH_RETMEM_PDEN                             (1 << 3)
#define PSC_AON_SES_RETMEM_PDEN                             (1 << 4)

// reg_178
#define PSC_AON_XTAL_IN_ISO_EN_REG                          (1 << 0)
#define PSC_AON_RC_IN_ISO_EN_REG                            (1 << 1)
#define PSC_AON_PCIE_IN_ISO_EN_REG                          (1 << 2)
#define PSC_AON_CODEC_IN_ISO_EN_REG                         (1 << 3)
#define PSC_AON_RF_IN_ISO_EN_REG                            (1 << 4)
#define PSC_AON_USBPHY_IN_ISO_EN_REG                        (1 << 5)
#define PSC_AON_SPIFLASH0_IN_ISO_EN_REG                     (1 << 6)
#define PSC_AON_SPIFLASH1_IN_ISO_EN_REG                     (1 << 7)
#define PSC_AON_PSRAM_IN_ISO_EN_REG                         (1 << 8)
#define PSC_AON_DSI_IN_ISO_EN_REG                           (1 << 9)
#define PSC_AON_EMMC_IN_ISO_EN_REG                          (1 << 10)
#define PSC_AON_GPIOS_IN_ISO_EN_REG                         (1 << 11)
#define PSC_AON_GPIOP_IN_ISO_EN_REG                         (1 << 12)

// reg_17c
#define PSC_AON_ANA_INTF_PSW_EN_REG                         (1 << 0)
#define PSC_AON_RF_INTF_PSW_EN_REG                          (1 << 1)
#define PSC_AON_USB_INTF_PSW_EN_REG                         (1 << 2)
#define PSC_AON_EMMC_INTF_PSW_EN_REG                        (1 << 3)
#define PSC_AON_PCIE_INTF_PSW_EN_REG                        (1 << 4)
#define PSC_AON_ANA_INTF_ISO_EN_REG                         (1 << 5)
#define PSC_AON_RF_INTF_ISO_EN_REG                          (1 << 6)
#define PSC_AON_USB_INTF_ISO_EN_REG                         (1 << 7)
#define PSC_AON_EMMC_INTF_ISO_EN_REG                        (1 << 8)
#define PSC_AON_PCIE_INTF_ISO_EN_REG                        (1 << 9)
#define PSC_AON_RF_INTF_RESTORE_REG                         (1 << 10)

// reg_180
#define PSC_AON_SYSINT2SYSCPU0_MASK                         (1 << 0)
#define PSC_AON_SYSINT2SYSCPU1_MASK                         (1 << 1)
#define PSC_AON_SYSCPU0_INTR_MASK                           (1 << 2)
#define PSC_AON_SYSCPU1_INTR_MASK                           (1 << 3)
#define PSC_AON_SYS_PWRON_PRE_SEL                           (1 << 4)
#define PSC_AON_SYS_PWRDN_PRE_SEL                           (1 << 5)
#define PSC_AON_SES_PWRON_PRE_SEL                           (1 << 6)
#define PSC_AON_SES_PWRDN_PRE_SEL                           (1 << 7)
#define PSC_AON_SYS_POWERDN_TIMER6(n)                       (((n) & 0xFF) << 8)
#define PSC_AON_SYS_POWERDN_TIMER6_MASK                     (0xFF << 8)
#define PSC_AON_SYS_POWERDN_TIMER6_SHIFT                    (8)

// reg_184
#define PSC_AON_SYS_GPIO_INTR_MASK2(n)                      (((n) & 0xFFFF) << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK2_MASK                    (0xFFFF << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK2_SHIFT                   (0)

// reg_188
#define PSC_AON_BTC_GPIO_INTR_MASK2(n)                      (((n) & 0xFFFF) << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK2_MASK                    (0xFFFF << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK2_SHIFT                   (0)

// reg_18c
#define PSC_AON_BTH_GPIO_INTR_MASK2(n)                      (((n) & 0xFFFF) << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK2_MASK                    (0xFFFF << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK2_SHIFT                   (0)

// reg_190
#define PSC_AON_SES_GPIO_INTR_MASK2(n)                      (((n) & 0xFFFF) << 0)
#define PSC_AON_SES_GPIO_INTR_MASK2_MASK                    (0xFFFF << 0)
#define PSC_AON_SES_GPIO_INTR_MASK2_SHIFT                   (0)

// reg_194
#define PSC_AON_BTH_PSC_DSLEEP_MSK                          (1 << 0)
#define PSC_AON_SES_PSC_DSLEEP_MSK                          (1 << 1)

// reg_198
#define PSC_AON_SYS_GPIO_INTR_MASK_STATUS2(n)               (((n) & 0xFFFF) << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK_STATUS2_MASK             (0xFFFF << 0)
#define PSC_AON_SYS_GPIO_INTR_MASK_STATUS2_SHIFT            (0)

// reg_19c
#define PSC_AON_BTC_GPIO_INTR_MASK_STATUS2(n)               (((n) & 0xFFFF) << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK_STATUS2_MASK             (0xFFFF << 0)
#define PSC_AON_BTC_GPIO_INTR_MASK_STATUS2_SHIFT            (0)

// reg_1a0
#define PSC_AON_BTH_GPIO_INTR_MASK_STATUS2(n)               (((n) & 0xFFFF) << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK_STATUS2_MASK             (0xFFFF << 0)
#define PSC_AON_BTH_GPIO_INTR_MASK_STATUS2_SHIFT            (0)

// reg_1a4
#define PSC_AON_SES_GPIO_INTR_MASK_STATUS2(n)               (((n) & 0xFFFF) << 0)
#define PSC_AON_SES_GPIO_INTR_MASK_STATUS2_MASK             (0xFFFF << 0)
#define PSC_AON_SES_GPIO_INTR_MASK_STATUS2_SHIFT            (0)

// reg_1a8
#define PSC_AON_GPIO_INTR_RAW_STATUS2(n)                    (((n) & 0xFFFF) << 0)
#define PSC_AON_GPIO_INTR_RAW_STATUS2_MASK                  (0xFFFF << 0)
#define PSC_AON_GPIO_INTR_RAW_STATUS2_SHIFT                 (0)
#endif
