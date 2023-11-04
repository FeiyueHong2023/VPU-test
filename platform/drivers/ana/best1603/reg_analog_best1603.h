/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __REG_ANALOG_BEST1603_H__
#define __REG_ANALOG_BEST1603_H__

#include "plat_types.h"

enum ANA_REG_T {
    ANA_REG_01 = 0x01,
    ANA_REG_02,
    ANA_REG_03,
    ANA_REG_04,
    ANA_REG_05,
    ANA_REG_06,
    ANA_REG_07,
    ANA_REG_08,
    ANA_REG_09,
    ANA_REG_0A,
    ANA_REG_0B,
    ANA_REG_0C,
    ANA_REG_0D,
    ANA_REG_0E,
    ANA_REG_0F,
    ANA_REG_10,
    ANA_REG_11,
    ANA_REG_12,
    ANA_REG_13,
    ANA_REG_14,
    ANA_REG_15,
    ANA_REG_16,
    ANA_REG_17,
    ANA_REG_18,
    ANA_REG_19,
    ANA_REG_1A,
    ANA_REG_1B,
    ANA_REG_1C,
    ANA_REG_1D,
    ANA_REG_1E,
    ANA_REG_1F,
    ANA_REG_20,
    ANA_REG_21,
    ANA_REG_22,
    ANA_REG_23,
    ANA_REG_24,
    ANA_REG_25,
    ANA_REG_26,
    ANA_REG_27,
    ANA_REG_28,
    ANA_REG_29,
    ANA_REG_2A,
    ANA_REG_2B,
    ANA_REG_2C,
    ANA_REG_2D,
    ANA_REG_2E,
    ANA_REG_2F,
    ANA_REG_30,
    ANA_REG_31,
    ANA_REG_32,
    ANA_REG_33,
    ANA_REG_34,
    ANA_REG_35,
    ANA_REG_36,
    ANA_REG_37,
    ANA_REG_38,
    ANA_REG_39,
    ANA_REG_3A,
    ANA_REG_3B,
    ANA_REG_3C,
    ANA_REG_3D,
    ANA_REG_3E,
    ANA_REG_3F,
    ANA_REG_40,
    ANA_REG_41,
    ANA_REG_42,
    ANA_REG_43,
    ANA_REG_44,
    ANA_REG_45,
    ANA_REG_46,
    ANA_REG_47,
    ANA_REG_48,
    ANA_REG_49,
    ANA_REG_4A,
    ANA_REG_4B,
    ANA_REG_4C,
    ANA_REG_4D,
    ANA_REG_4E,
    ANA_REG_4F,
    ANA_REG_50,
    ANA_REG_51,
    ANA_REG_52,
    ANA_REG_53,
    ANA_REG_54,
    ANA_REG_55,
    ANA_REG_56,
    ANA_REG_57,
    ANA_REG_58,
    ANA_REG_59,
    ANA_REG_5A,
    ANA_REG_5B,
    ANA_REG_5C,
    ANA_REG_5D,
    ANA_REG_5E,
    ANA_REG_5F,
    ANA_REG_60,
    ANA_REG_61,
    ANA_REG_62,
    ANA_REG_63,
    ANA_REG_64,
    ANA_REG_65,
    ANA_REG_66,
    ANA_REG_67,
    ANA_REG_68,
    ANA_REG_69,
    ANA_REG_6A,
    ANA_REG_6B,
    ANA_REG_6C,
    ANA_REG_6D,
    ANA_REG_6E,
    ANA_REG_6F,
    ANA_REG_70,
    ANA_REG_71,
    ANA_REG_72,
    ANA_REG_73,
    ANA_REG_74,
    ANA_REG_75,
    ANA_REG_76,
    ANA_REG_77,
    ANA_REG_78,
    ANA_REG_79,
    ANA_REG_7A,
    ANA_REG_7B,
    ANA_REG_7C,
    ANA_REG_7D,
    ANA_REG_7E,
    ANA_REG_7F,
    ANA_REG_80,
    ANA_REG_81,
    ANA_REG_82,
    ANA_REG_83,
    ANA_REG_84,
    ANA_REG_85,
    ANA_REG_86,
    ANA_REG_87,
    ANA_REG_88,
    ANA_REG_89,
    ANA_REG_8A,
    ANA_REG_8B,
    ANA_REG_8C,
    ANA_REG_8D,
    ANA_REG_8E,
    ANA_REG_8F,
    ANA_REG_90,
    ANA_REG_91,
    ANA_REG_92,
    ANA_REG_93,
    ANA_REG_94,
    ANA_REG_95,
    ANA_REG_96,
    ANA_REG_97,
    ANA_REG_98,
    ANA_REG_99,
    ANA_REG_9A,
    ANA_REG_9B,
    ANA_REG_9C,
    ANA_REG_9D,
    ANA_REG_9E,
    ANA_REG_9F,
    ANA_REG_A0,
    ANA_REG_A1,
    ANA_REG_A2,
    ANA_REG_A3,
    ANA_REG_A4,
    ANA_REG_A5,
    ANA_REG_A6,
    ANA_REG_A7,
    ANA_REG_A8,
    ANA_REG_A9,
    ANA_REG_AA,
    ANA_REG_AB,
    ANA_REG_AC,
    ANA_REG_AD,
    ANA_REG_AE,
    ANA_REG_AF,
    ANA_REG_B0,
    ANA_REG_B1,
    ANA_REG_B2,
    ANA_REG_B3,
    ANA_REG_B4,
    ANA_REG_B5,
    ANA_REG_B6,
    ANA_REG_B7,
    ANA_REG_B8,
    ANA_REG_B9,
    ANA_REG_BA,
    ANA_REG_BB,
    ANA_REG_BC,
    ANA_REG_BD,
    ANA_REG_BE,
    ANA_REG_BF,
    ANA_REG_C0,
    ANA_REG_C1,
    ANA_REG_C2,
    ANA_REG_C3,
    ANA_REG_C4,
    ANA_REG_C5,
    ANA_REG_C6,
    ANA_REG_C7,
    ANA_REG_C8,
    ANA_REG_C9,
    ANA_REG_CA,
    ANA_REG_CB,
    ANA_REG_CC,
    ANA_REG_CD,
    ANA_REG_CE,
    ANA_REG_CF,
    ANA_REG_D0,
    ANA_REG_D1,
    ANA_REG_D2,
    ANA_REG_D3,
    ANA_REG_D4,
    ANA_REG_D5,
    ANA_REG_D6,
    ANA_REG_D7,
    ANA_REG_D8,
    ANA_REG_D9,
    ANA_REG_DA,
    ANA_REG_DB,
    ANA_REG_DC,
    ANA_REG_DD,
    ANA_REG_DE,
    ANA_REG_DF,
    ANA_REG_F0 = 0xF0,
    ANA_REG_F1,
    ANA_REG_F2,
    ANA_REG_F3,
    ANA_REG_F4,
    ANA_REG_F5,
    ANA_REG_F6,
    ANA_REG_F7,
    ANA_REG_F8,
    ANA_REG_F9,

};


// REG_00
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_0F
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)

// REG_4E
#define RESERVED_DIG_15_0_SHIFT             0
#define RESERVED_DIG_15_0_MASK              (0xFFFF << RESERVED_DIG_15_0_SHIFT)
#define RESERVED_DIG_15_0(n)                BITFIELD_VAL(RESERVED_DIG_15_0, n)

// REG_4F
#define RESERVED_DIG_31_16_SHIFT            0
#define RESERVED_DIG_31_16_MASK             (0xFFFF << RESERVED_DIG_31_16_SHIFT)
#define RESERVED_DIG_31_16(n)               BITFIELD_VAL(RESERVED_DIG_31_16, n)

// REG_50
#define RESERVED_DIG_32                     (1 << 0)

// REG_53
#define REG_BYPASS_TX_REGULATOR             (1 << 14)

// REG_54
#define REG_VOICE_PU_LDO_DR                 (1 << 12)
#define REG_VOICE_PU_LDO                    (1 << 13)
#define REG_VOICE_PU_OSC_DR                 (1 << 14)
#define REG_VOICE_PU_OSC                    (1 << 15)

// REG_5B
#define CFG_BT_TRIG_PU_OSC                  (1 << 15)

// REG_6B
#define CFG_TX_PEAK_OFF_ADC_EN              (1 << 0)
#define CFG_TX_PEAK_OFF_DAC_EN              (1 << 1)
#define CFG_PEAK_DET_DR                     (1 << 2)
#define CFG_TX_PEAK_OFF_ADC                 (1 << 3)
#define CFG_TX_PEAK_OFF_DAC                 (1 << 4)
#define CFG_PEAK_DET_DB_DELAY_SHIFT         5
#define CFG_PEAK_DET_DB_DELAY_MASK          (0x7 << CFG_PEAK_DET_DB_DELAY_SHIFT)
#define CFG_PEAK_DET_DB_DELAY(n)            BITFIELD_VAL(CFG_PEAK_DET_DB_DELAY, n)

// REG_6C
#define REG_CODEC_TX_PEAK_NL_EN             (1 << 0)
#define REG_CODEC_TX_PEAK_NR_EN             (1 << 1)
#define REG_CODEC_TX_PEAK_PL_EN             (1 << 2)
#define REG_CODEC_TX_PEAK_PR_EN             (1 << 3)
#define REG_CODEC_TX_PEAK_DET_BIT_TMP       (1 << 4)
#define DIG_PU_CLKMUX_LDO0P9                (1 << 5)
#define REG_PU_OSC                          (1 << 6)
#define REG_CLKMUX_DVDD_SEL                 (1 << 7)
#define REG_CRYSTAL_SEL                     (1 << 8)
#define REG_IDETLEAR_EN                     (1 << 9)
#define REG_EXTPLL_SEL                      (1 << 11)
#define REG_TX_REGULATOR_BIT_SHIFT          12
#define REG_TX_REGULATOR_BIT_MASK           (0xF << REG_TX_REGULATOR_BIT_SHIFT)
#define REG_TX_REGULATOR_BIT(n)             BITFIELD_VAL(REG_TX_REGULATOR_BIT, n)

// REG_6D
#define REG_CODEC_TX_EAR_DR_EN              (1 << 0)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE  (1 << 1)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT   3
#define REG_CODEC_TX_EAR_DRE_GAIN_L_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L, n)
#define REG_CODEC_TX_EAR_ENBIAS             (1 << 13)
#define REG_CODEC_TX_EAR_LPBIAS             (1 << 14)
#define REG_CODEC_TX_EAR_OCEN               (1 << 15)

// REG_6E
#define REG_CODEC_TEST_SEL_SHIFT            0
#define REG_CODEC_TEST_SEL_MASK             (0xF << REG_CODEC_TEST_SEL_SHIFT)
#define REG_CODEC_TEST_SEL(n)               BITFIELD_VAL(REG_CODEC_TEST_SEL, n)
#define REG_CODEC_TX_DAC_VREF_L_SHIFT       8
#define REG_CODEC_TX_DAC_VREF_L_MASK        (0xF << REG_CODEC_TX_DAC_VREF_L_SHIFT)
#define REG_CODEC_TX_DAC_VREF_L(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_L, n)

// REG_6F
#define REG_CODEC_TX_EAR_COMP1_L_SHIFT      0
#define REG_CODEC_TX_EAR_COMP1_L_MASK       (0x3F << REG_CODEC_TX_EAR_COMP1_L_SHIFT)
#define REG_CODEC_TX_EAR_COMP1_L(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_COMP1_L, n)
#define REG_CODEC_TX_EAR_COMP2_L_SHIFT      8
#define REG_CODEC_TX_EAR_COMP2_L_MASK       (0xFF << REG_CODEC_TX_EAR_COMP2_L_SHIFT)
#define REG_CODEC_TX_EAR_COMP2_L(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_COMP2_L, n)

// REG_70
#define REG_CODEC_TX_EAR_LOWGAINL_SHIFT     0
#define REG_CODEC_TX_EAR_LOWGAINL_MASK      (0xF << REG_CODEC_TX_EAR_LOWGAINL_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINL, n)
#define REG_CODEC_TX_EAR_LCAL               (1 << 14)

// REG_71
#define REG_CODEC_TX_EAR_OFF_BITL_TMP_SHIFT 0
#define REG_CODEC_TX_EAR_OFF_BITL_TMP_MASK  (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_TMP_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL_TMP(n)    BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL_TMP, n)
#define REG_CODEC_TX_EAR_OFFEN              (1 << 14)
#define REG_CODEC_TX_EN_DACLDO              (1 << 15)

// REG_72
#define REG_CODEC_CLKMUX_VREFGEN_VOSEL      (1 << 0)
#define REG_PU_LPO48MADC                    (1 << 1)
#define REG_CODEC_TX_ST1_CASN_L_SHIFT       2
#define REG_CODEC_TX_ST1_CASN_L_MASK        (0x7 << REG_CODEC_TX_ST1_CASN_L_SHIFT)
#define REG_CODEC_TX_ST1_CASN_L(n)          BITFIELD_VAL(REG_CODEC_TX_ST1_CASN_L, n)
#define REG_CODEC_TX_ST1_CASP_L_SHIFT       5
#define REG_CODEC_TX_ST1_CASP_L_MASK        (0xF << REG_CODEC_TX_ST1_CASP_L_SHIFT)
#define REG_CODEC_TX_ST1_CASP_L(n)          BITFIELD_VAL(REG_CODEC_TX_ST1_CASP_L, n)

// REG_73
#define REG_CODEC_TX_EAR_SOFTSTART_SHIFT    0
#define REG_CODEC_TX_EAR_SOFTSTART_MASK     (0x3F << REG_CODEC_TX_EAR_SOFTSTART_SHIFT)
#define REG_CODEC_TX_EAR_SOFTSTART(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_SOFTSTART, n)
#define REG_CODEC_TX_EAR_OUTPUTSEL_L_SHIFT  6
#define REG_CODEC_TX_EAR_OUTPUTSEL_L_MASK   (0x7 << REG_CODEC_TX_EAR_OUTPUTSEL_L_SHIFT)
#define REG_CODEC_TX_EAR_OUTPUTSEL_L(n)     BITFIELD_VAL(REG_CODEC_TX_EAR_OUTPUTSEL_L, n)
#define REG_CODEC_TX_EN_LDAC                (1 << 9)
#define REG_CODEC_TX_EAR_DR_ST_L_SHIFT      11
#define REG_CODEC_TX_EAR_DR_ST_L_MASK       (0x3 << REG_CODEC_TX_EAR_DR_ST_L_SHIFT)
#define REG_CODEC_TX_EAR_DR_ST_L(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_DR_ST_L, n)
#define REG_CODEC_TX_OUTPUT_L_LP_TMP        (1 << 14)

// REG_74
#define REG_CODEC_TX_EAR_DIS_SHIFT          0
#define REG_CODEC_TX_EAR_DIS_MASK           (0x3 << REG_CODEC_TX_EAR_DIS_SHIFT)
#define REG_CODEC_TX_EAR_DIS(n)             BITFIELD_VAL(REG_CODEC_TX_EAR_DIS, n)
#define REG_CODEC_TX_EN_LCLK                (1 << 3)
#define REG_CODEC_TX_EN_EARPA_L             (1 << 4)
#define REG_CODEC_TX_EN_S1PA                (1 << 6)
#define REG_CODEC_TX_EN_S2PA                (1 << 7)
#define REG_CODEC_TX_EN_S3PA                (1 << 8)
#define REG_CODEC_TX_EN_S4PA                (1 << 9)
#define REG_CODEC_TX_EN_S5PA                (1 << 10)
#define REG_PU_TX_REGULATOR                 (1 << 11)
#define REG_CODEC_DAC_CLK_EDGE_SEL          (1 << 13)
#define REG_CODEC_TX_EAR_GAIN_TMP_SHIFT     14
#define REG_CODEC_TX_EAR_GAIN_TMP_MASK      (0x3 << REG_CODEC_TX_EAR_GAIN_TMP_SHIFT)
#define REG_CODEC_TX_EAR_GAIN_TMP(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_GAIN_TMP, n)

// REG_75
#define REG_CODEC_TX_EN_LPPA                (1 << 0)
#define REG_CODEC_TX_SW_MODE                (1 << 1)
#define DRE_GAIN_SEL_L                      (1 << 2)
#define REG_CODEC_TX_EN_LDAC_ANA            (1 << 3)
#define CFG_TX_TREE_EN                      (1 << 4)
#define CFG_TX_CH0_MUTE                     (1 << 5)
#define CFG_TX_CLK_INV                      (1 << 7)
#define CFG_CODEC_DIN_L_RST                 (1 << 8)
#define REG_CODEC_TX_EAR_VCM_L_SHIFT        9
#define REG_CODEC_TX_EAR_VCM_L_MASK         (0x7 << REG_CODEC_TX_EAR_VCM_L_SHIFT)
#define REG_CODEC_TX_EAR_VCM_L(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_VCM_L, n)
#define REG_CODEC_TX_OUTPUT_L_LP_DR         (1 << 14)

// REG_76
#define CODEC_DIN_L_HI_RSTVAL_SHIFT         0
#define CODEC_DIN_L_HI_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_HI_RSTVAL_SHIFT)
#define CODEC_DIN_L_HI_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_HI_RSTVAL, n)

// REG_77
#define CODEC_DIN_L_LO_RSTVAL_SHIFT         0
#define CODEC_DIN_L_LO_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_LO_RSTVAL_SHIFT)
#define CODEC_DIN_L_LO_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_LO_RSTVAL, n)

// REG_7A
#define REG_CODEC_TX_VREF_RZ                (1 << 0)
#define REG_CODEC_TX_OC_PATH                (1 << 1)
#define REG_CODEC_TX_OFF_RANGE_X2           (1 << 2)
#define REG_CLKMUX_LOWF_IN                  (1 << 8)
#define REG_ZERO_DETECT_CHANGE              (1 << 9)
#define REG_PU_ZERO_DET_L                   (1 << 11)
#define REG_CODEC_TX_OUTPUT_LP_POL          (1 << 14)

// REG_7B
#define TX_PEAK_DET_STATUS                  (1 << 0)
#define TX_PEAK_DET_NL_STATUS               (1 << 1)
#define TX_PEAK_DET_PL_STATUS               (1 << 2)

// REG_81
#define RESERVED_ANA_15_0_SHIFT             0
#define RESERVED_ANA_15_0_MASK              (0xFFFF << RESERVED_ANA_15_0_SHIFT)
#define RESERVED_ANA_15_0(n)                BITFIELD_VAL(RESERVED_ANA_15_0, n)

// REG_82
#define RESERVED_ANA_31_16_SHIFT            0
#define RESERVED_ANA_31_16_MASK             (0xFFFF << RESERVED_ANA_31_16_SHIFT)
#define RESERVED_ANA_31_16(n)               BITFIELD_VAL(RESERVED_ANA_31_16, n)

// REG_83
#define RESERVED_ANA_47_32_SHIFT            0
#define RESERVED_ANA_47_32_MASK             (0xFFFF << RESERVED_ANA_47_32_SHIFT)
#define RESERVED_ANA_47_32(n)               BITFIELD_VAL(RESERVED_ANA_47_32, n)

// REG_84
#define RESERVED_ANA_55_48_SHIFT            0
#define RESERVED_ANA_55_48_MASK             (0xFF << RESERVED_ANA_55_48_SHIFT)
#define RESERVED_ANA_55_48(n)               BITFIELD_VAL(RESERVED_ANA_55_48, n)

// REG_85
#define REG_CODEC_TX_EAR_GAIN_CHANGE        (1 << 10)

// REG_D0
#define REG_CODEC_TX_EAR_OFF_BITL_1_SHIFT   0
#define REG_CODEC_TX_EAR_OFF_BITL_1_MASK    (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_1_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL_1(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL_1, n)

// REG_D1
#define REG_CODEC_TX_EAR_OFF_BITL_2_SHIFT   0
#define REG_CODEC_TX_EAR_OFF_BITL_2_MASK    (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_2_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL_2(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL_2, n)

// REG_D2
#define REG_CODEC_TX_EAR_DRE_GAIN_L_1_SHIFT 0
#define REG_CODEC_TX_EAR_DRE_GAIN_L_1_MASK  (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_1_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_1(n)    BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L_1, n

// REG_D3
#define REG_CODEC_TX_EAR_DRE_GAIN_L_2_SHIFT 0
#define REG_CODEC_TX_EAR_DRE_GAIN_L_2_MASK  (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_2_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_2(n)    BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L_2, n

// REG_D4
#define ANADRE_GAIN_SEL                     (1 << 0)
#define ANADRE_DC_SEL                       (1 << 1)

// REG_D7
#define REG_CODEC_TX_VREFBUF_CSEL_L_SHIFT   3
#define REG_CODEC_TX_VREFBUF_CSEL_L_MASK    (0x7 << REG_CODEC_TX_VREFBUF_CSEL_L_SHIFT)
#define REG_CODEC_TX_VREFBUF_CSEL_L(n)      BITFIELD_VAL(REG_CODEC_TX_VREFBUF_CSEL_L, n)
#define REG_CODEC_DAC_LOWGAIN_SHIFT         9
#define REG_CODEC_DAC_LOWGAIN_MASK          (0x7 << REG_CODEC_DAC_LOWGAIN_SHIFT)
#define REG_CODEC_DAC_LOWGAIN(n)            BITFIELD_VAL(REG_CODEC_DAC_LOWGAIN, n)

// REG_D8
#define REG_CODEC_TX_OCP_SEL_L_SHIFT        0
#define REG_CODEC_TX_OCP_SEL_L_MASK         (0x3 << REG_CODEC_TX_OCP_SEL_L_SHIFT)
#define REG_CODEC_TX_OCP_SEL_L(n)           BITFIELD_VAL(REG_CODEC_TX_OCP_SEL_L, n)
#define REG_CODEC_TX_REG_LOWGAIN            (1 << 10)
#define REG_CODEC_TX_REG_LP_SHIFT           11
#define REG_CODEC_TX_REG_LP_MASK            (0x7 << REG_CODEC_TX_REG_LP_SHIFT)
#define REG_CODEC_TX_REG_LP(n)              BITFIELD_VAL(REG_CODEC_TX_REG_LP, n)
#define REG_CODEC_CLKMUX_VREFGEN_EN         (1 << 15)

// REG_D9
#define REG_CODEC_CLKMUX_VREFGEN_VSEL_SHIFT 0
#define REG_CODEC_CLKMUX_VREFGEN_VSEL_MASK  (0xF << REG_CODEC_CLKMUX_VREFGEN_VSEL_SHIFT)
#define REG_CODEC_CLKMUX_VREFGEN_VSEL(n)    BITFIELD_VAL(REG_CODEC_CLKMUX_VREFGEN_VSEL, n
#define REG_PU_OSC_PLL24MADC                (1 << 4)
#define REG_PU_OSC_PLL24MDAC                (1 << 5)
#define DAC_FIFO_FLUSH                      (1 << 6)
#define DAC_FIFO_DELAY_SHIFT                7
#define DAC_FIFO_DELAY_MASK                 (0x1F << DAC_FIFO_DELAY_SHIFT)
#define DAC_FIFO_DELAY(n)                   BITFIELD_VAL(DAC_FIFO_DELAY, n)
#define DAC_FIFO_OVERFLOW_STATUS            (1 << 12)
#define DAC_FIFO_UNDERFLOW_STATUS           (1 << 13)

// REG_DA
#define RESERVED_ANA_LV_SHIFT               0
#define RESERVED_ANA_LV_MASK                (0x3FF << RESERVED_ANA_LV_SHIFT)
#define RESERVED_ANA_LV(n)                  BITFIELD_VAL(RESERVED_ANA_LV, n)

// REG_DB
#define REG_CODEC_TX_CASN_L_SHIFT           0
#define REG_CODEC_TX_CASN_L_MASK            (0x7 << REG_CODEC_TX_CASN_L_SHIFT)
#define REG_CODEC_TX_CASN_L(n)              BITFIELD_VAL(REG_CODEC_TX_CASN_L, n)
#define REG_CODEC_TX_CASP_L_SHIFT           3
#define REG_CODEC_TX_CASP_L_MASK            (0x7 << REG_CODEC_TX_CASP_L_SHIFT)
#define REG_CODEC_TX_CASP_L(n)              BITFIELD_VAL(REG_CODEC_TX_CASP_L, n)
#define REG_CODEC_TX_ST2_LP_L_SHIFT         12
#define REG_CODEC_TX_ST2_LP_L_MASK          (0xF << REG_CODEC_TX_ST2_LP_L_SHIFT)
#define REG_CODEC_TX_ST2_LP_L(n)            BITFIELD_VAL(REG_CODEC_TX_ST2_LP_L, n)

// REG_DD
#define REG_CODEC_TX_EAR_ST1_LP_L_SHIFT     8
#define REG_CODEC_TX_EAR_ST1_LP_L_MASK      (0xF << REG_CODEC_TX_EAR_ST1_LP_L_SHIFT)
#define REG_CODEC_TX_EAR_ST1_LP_L(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_ST1_LP_L, n)
#define REG_CODEC_TX_VREFBUF_LP_SHIFT       12
#define REG_CODEC_TX_VREFBUF_LP_MASK        (0x7 << REG_CODEC_TX_VREFBUF_LP_SHIFT)
#define REG_CODEC_TX_VREFBUF_LP(n)          BITFIELD_VAL(REG_CODEC_TX_VREFBUF_LP, n)

// REG_DE
#define DBG_RCOSC48M_TUN_CODE_FINE_SHIFT    0
#define DBG_RCOSC48M_TUN_CODE_FINE_MASK     (0x1F << DBG_RCOSC48M_TUN_CODE_FINE_SHIFT)
#define DBG_RCOSC48M_TUN_CODE_FINE(n)       BITFIELD_VAL(DBG_RCOSC48M_TUN_CODE_FINE, n)
#define DBG_RCOSC48M_TUN_CODE_SHIFT         5
#define DBG_RCOSC48M_TUN_CODE_MASK          (0x3F << DBG_RCOSC48M_TUN_CODE_SHIFT)
#define DBG_RCOSC48M_TUN_CODE(n)            BITFIELD_VAL(DBG_RCOSC48M_TUN_CODE, n)
#define REG_CODEC_RC_OSC_EN_CLK_DLATCH      (1 << 11)
#define REG_CODEC_RC_OSC_EN_CLK_ADC_DIG     (1 << 12)

// REG_DF
#define REG_RCOSC48M_CALIB_EN               (1 << 0)
#define REG_RCOSC48M_LLC_SFT_RSTN           (1 << 1)
#define REG_RCOSC48M_POLAR_SEL              (1 << 2)
#define REG_REF_24M_DIV_SHIFT               3
#define REG_REF_24M_DIV_MASK                (0x3F << REG_REF_24M_DIV_SHIFT)
#define REG_REF_24M_DIV(n)                  BITFIELD_VAL(REG_REF_24M_DIV, n)
#define REG_RCOSC48M_TUN_CODE_DR            (1 << 9)
#define REG_RCOSC48M_TUN_CODE_SHIFT         10
#define REG_RCOSC48M_TUN_CODE_MASK          (0x3F << REG_RCOSC48M_TUN_CODE_SHIFT)
#define REG_RCOSC48M_TUN_CODE(n)            BITFIELD_VAL(REG_RCOSC48M_TUN_CODE, n)

// REG_F0
#define REG_RCOSC48M_GOAL_CNT_MARK_15_0_SHIFT 0
#define REG_RCOSC48M_GOAL_CNT_MARK_15_0_MASK (0xFFFF << REG_RCOSC48M_GOAL_CNT_MARK_15_0_S
#define REG_RCOSC48M_GOAL_CNT_MARK_15_0(n)  BITFIELD_VAL(REG_RCOSC48M_GOAL_CNT_MARK_15_0,

// REG_F1
#define REG_RCOSC48M_REF_DIV_NUM_SHIFT      0
#define REG_RCOSC48M_REF_DIV_NUM_MASK       (0x3FF << REG_RCOSC48M_REF_DIV_NUM_SHIFT)
#define REG_RCOSC48M_REF_DIV_NUM(n)         BITFIELD_VAL(REG_RCOSC48M_REF_DIV_NUM, n)
#define REG_RCOSC48M_GOAL_CNT_MARK_16       (1 << 10)
#define DBG_RCOSC48M_GOAL_CNT_16_13_SHIFT   11
#define DBG_RCOSC48M_GOAL_CNT_16_13_MASK    (0xF << DBG_RCOSC48M_GOAL_CNT_16_13_SHIFT)
#define DBG_RCOSC48M_GOAL_CNT_16_13(n)      BITFIELD_VAL(DBG_RCOSC48M_GOAL_CNT_16_13, n)

// REG_F2
#define DBG_RCOSC48M_STATE_SHIFT            0
#define DBG_RCOSC48M_STATE_MASK             (0x3 << DBG_RCOSC48M_STATE_SHIFT)
#define DBG_RCOSC48M_STATE(n)               BITFIELD_VAL(DBG_RCOSC48M_STATE, n)
#define DBG_RCOSC48M_TUN_DONE               (1 << 2)
#define DBG_RCOSC48M_GOAL_CNT_12_0_SHIFT    3
#define DBG_RCOSC48M_GOAL_CNT_12_0_MASK     (0x1FFF << DBG_RCOSC48M_GOAL_CNT_12_0_SHIFT)
#define DBG_RCOSC48M_GOAL_CNT_12_0(n)       BITFIELD_VAL(DBG_RCOSC48M_GOAL_CNT_12_0, n)

// REG_F3
#define DBG_RCOSC48M_REF_CNT_SHIFT          0
#define DBG_RCOSC48M_REF_CNT_MASK           (0x3FF << DBG_RCOSC48M_REF_CNT_SHIFT)
#define DBG_RCOSC48M_REF_CNT(n)             BITFIELD_VAL(DBG_RCOSC48M_REF_CNT, n)
#define REG_TUN_CODE_AG_SHIFT               10
#define REG_TUN_CODE_AG_MASK                (0x1F << REG_TUN_CODE_AG_SHIFT)
#define REG_TUN_CODE_AG(n)                  BITFIELD_VAL(REG_TUN_CODE_AG, n)
#define REG_TUN_CODE_AG_DR                  (1 << 15)

// REG_F4
#define REG_CODEC_COMP_IBSEL_SHIFT          0
#define REG_CODEC_COMP_IBSEL_MASK           (0xF << REG_CODEC_COMP_IBSEL_SHIFT)
#define REG_CODEC_COMP_IBSEL(n)             BITFIELD_VAL(REG_CODEC_COMP_IBSEL, n)
#define REG_CODEC_COMP_INV                  (1 << 4)
#define REG_CODEC_COMP_EN                   (1 << 5)
#define REG_CODEC_COMP_GEN_PULSE            (1 << 6)
#define CODEC_COMP_VALID                    (1 << 7)
#define CODEC_COMP_VOP                      (1 << 8)
#define REG_CODEC_COMP_VOP                  (1 << 9)
#define REG_DC_48M_DIV_SHIFT                10
#define REG_DC_48M_DIV_MASK                 (0xF << REG_DC_48M_DIV_SHIFT)
#define REG_DC_48M_DIV(n)                   BITFIELD_VAL(REG_DC_48M_DIV, n)
#define REG_DC_CLK_SEL_OSC                  (1 << 14)

// REG_F5
#define REG_CODEC_COMP_CNT_SHIFT            0
#define REG_CODEC_COMP_CNT_MASK             (0x3FF << REG_CODEC_COMP_CNT_SHIFT)
#define REG_CODEC_COMP_CNT(n)               BITFIELD_VAL(REG_CODEC_COMP_CNT, n)

// REG_F6
#define REG_SCAN_MODE                       (1 << 0)
#define REG_BIST_MODE                       (1 << 1)
#define REG_JTAG_MODE                       (1 << 2)
#define REG_CODEC_EN_BIAS                   (1 << 3)
#define REG_CODEC_EN_BIAS_LP_TMP            (1 << 4)
#define REG_CODEC_EN_BIAS_LP_DR             (1 << 5)
#define REG_CODEC_LP_VCM                    (1 << 7)
#define REG_CODEC_EN_VCM                    (1 << 8)
#define REG_CODEC_VCM_EN_LPF                (1 << 9)
#define REG_CODEC_BIAS_LOWV                 (1 << 10)
#define REG_CODEC_BIAS_LOWV_LP              (1 << 11)
#define REG_CODEC_TX_DRV2X_L                (1 << 12)
#define REG_CK26M_SEL                       (1 << 13)
#define REG_CODEC_BUF_LOWPOWER              (1 << 14)
#define REG_CODEC_BUF_LOWPOWER2             (1 << 15)

// REG_F7
#define REG_CODEC_RC_OSC_EN_AGPIO_TEST      (1 << 0)
#define REG_CODEC_RC_OSC_FR_SHIFT           1
#define REG_CODEC_RC_OSC_FR_MASK            (0x7 << REG_CODEC_RC_OSC_FR_SHIFT)
#define REG_CODEC_RC_OSC_FR(n)              BITFIELD_VAL(REG_CODEC_RC_OSC_FR, n)
#define REG_CODEC_RC_OSC_PWR_BIT_SHIFT      4
#define REG_CODEC_RC_OSC_PWR_BIT_MASK       (0x3 << REG_CODEC_RC_OSC_PWR_BIT_SHIFT)
#define REG_CODEC_RC_OSC_PWR_BIT(n)         BITFIELD_VAL(REG_CODEC_RC_OSC_PWR_BIT, n)
#define REG_CODEC_RC_OSC_VOUT_SEL_SHIFT     6
#define REG_CODEC_RC_OSC_VOUT_SEL_MASK      (0x7 << REG_CODEC_RC_OSC_VOUT_SEL_SHIFT)
#define REG_CODEC_RC_OSC_VOUT_SEL(n)        BITFIELD_VAL(REG_CODEC_RC_OSC_VOUT_SEL, n)
#define REG_RC_OSC_PWR_VBG_SHIFT            9
#define REG_RC_OSC_PWR_VBG_MASK             (0x3 << REG_RC_OSC_PWR_VBG_SHIFT)
#define REG_RC_OSC_PWR_VBG(n)               BITFIELD_VAL(REG_RC_OSC_PWR_VBG, n)
#define REG_RC_OSC_MUX_CLK_BT1603_DIG       (1 << 11)
#define REG_RC_OSC_REF_VBG                  (1 << 12)
#define REG_CODEC_RC_OSC_EN_CLK_BT1603_DIG  (1 << 13)
#define REG_CODEC_RC_OSC_EN_CLK_TO_MUX      (1 << 14)
#define REG_CODEC_EN_VCMBUFFER              (1 << 15)

// REG_F8
#define REG_CODEC_BUF_LOWVCM_SHIFT          0
#define REG_CODEC_BUF_LOWVCM_MASK           (0x7 << REG_CODEC_BUF_LOWVCM_SHIFT)
#define REG_CODEC_BUF_LOWVCM(n)             BITFIELD_VAL(REG_CODEC_BUF_LOWVCM, n)
#define REG_CODEC_BIAS_IBSEL_SHIFT          4
#define REG_CODEC_BIAS_IBSEL_MASK           (0xF << REG_CODEC_BIAS_IBSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL(n)             BITFIELD_VAL(REG_CODEC_BIAS_IBSEL, n)
#define REG_CODEC_BIAS_IBSEL_VOICE_SHIFT    8
#define REG_CODEC_BIAS_IBSEL_VOICE_MASK     (0xF << REG_CODEC_BIAS_IBSEL_VOICE_SHIFT)
#define REG_CODEC_BIAS_IBSEL_VOICE(n)       BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_VOICE, n)
#define REG_CODEC_BIAS_IBSEL_TX_SHIFT       12
#define REG_CODEC_BIAS_IBSEL_TX_MASK        (0xF << REG_CODEC_BIAS_IBSEL_TX_SHIFT)
#define REG_CODEC_BIAS_IBSEL_TX(n)          BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_TX, n)

// REG_F9
#define REG_CODEC_VCM_LOW_VCM_SHIFT         0
#define REG_CODEC_VCM_LOW_VCM_MASK          (0xF << REG_CODEC_VCM_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_LOW_VCM(n)            BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM, n)
#define REG_CODEC_VCM_LOW_VCM_LPF_SHIFT     4
#define REG_CODEC_VCM_LOW_VCM_LPF_MASK      (0xF << REG_CODEC_VCM_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LPF(n)        BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LPF, n)
#define REG_CODEC_VCM_LOW_VCM_LP_SHIFT      8
#define REG_CODEC_VCM_LOW_VCM_LP_MASK       (0xF << REG_CODEC_VCM_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LP(n)         BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LP, n)

#endif


