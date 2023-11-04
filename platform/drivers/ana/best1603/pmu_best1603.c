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
#include "pmu.h"
#include "analog.h"
#include "cmsis.h"
#include "cmsis_nvic.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_aud.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "tgt_hardware.h"
#include "hal_psc.h"
#include "hal_bootmode.h"

#define PMU_LED_VIA_PWM
//#define PMU_RF_WORKAROUND
#define PMU_POWER_PROTECT_FORCE_HW_PMU_OFF
#define PMU_POWER_PROTECT_VOLT_DISABLE
#define PMU_BUCK_DIRECT_ULP_MODE

#if !defined(DIG_DCDC_MODE) && !defined(PMU_LDO_VCORE_CALIB)
#define PMU_LDO_VCORE_CALIB
#endif

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_USE_SYS_PERIPH) && \
        (!defined(CORE_SLEEP_POWER_DOWN) || defined(CPU_PD_SUBSYS_PU))
// BTH
#define SYS_GPIO_SLEEP_POWER_UP
#endif
#if !defined(CHIP_SUBSYS_BTH) && \
        (!defined(CORE_SLEEP_POWER_DOWN) || defined(CPU_PD_SUBSYS_PU))
// M55
#define SYS_GPIO_SLEEP_POWER_UP
#endif

// LDO soft start interval is about 1000 us
#define PMU_LDO_PU_STABLE_TIME_US       1800
#define PMU_LDO_PULLDOWN_STABLE_TIME_US 1000
#define PMU_DCDC_HW_RAMP_STABLE_TIME_US 200
#define PMU_DCDC_HW_RAMP_1_CLK_TIME_US  40
#define PMU_DCDC_PU_STABLE_TIME_US      100
#define PMU_VHPPA_STABLE_TIME_US        10
#define PMU_VANA_STABLE_TIME_US         10
#define PMU_VCORE_STABLE_TIME_US        10
#define PMU_BIG_BG_STABLE_TIME_US       200

// External 32.768KHz stable time(ms)
#define PMU_EXT_32K_STABLE_TIME         300
#define PMU_EXT_32K_CLK_OUT_STABLE_TIME 1
#define PMU_CLK_SWITCH_STABLE_TIME      1

#define HPPA_RAMP_UP_VOLT_MV            1800

#define PMU_DCDC_ANA_1_6V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x84 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0xC9 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x94 : 0x98)
#define PMU_DCDC_ANA_1_5V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x77 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0xB9 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x84 : 0x88)
#define PMU_DCDC_ANA_1_4V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x69 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0xA9 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x78 : 0x7C)
#define PMU_DCDC_ANA_1_3V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x5C : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x99 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x68 : 0x6C)
#define PMU_DCDC_ANA_1_2V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x4F : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x89 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x58 : 0x5C)
#define PMU_DCDC_ANA_1_15V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x48 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x81 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x50 : 0x54)
#define PMU_DCDC_ANA_1_125V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x44 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x7D : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x4C : 0x50)
#define PMU_DCDC_ANA_1_1V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x41 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x79 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x48 : 0x4C)
#define PMU_DCDC_ANA_1_05V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x3B : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x71 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x40 : 0x44)
#define PMU_DCDC_ANA_1_0V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x34 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x69 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x38 : 0x3C)

#define PMU_DCDC_ANA_SLEEP_1_3V         PMU_DCDC_ANA_1_3V
#define PMU_DCDC_ANA_SLEEP_1_2V         PMU_DCDC_ANA_1_2V
#define PMU_DCDC_ANA_SLEEP_1_1V         PMU_DCDC_ANA_1_1V
#define PMU_DCDC_ANA_SLEEP_1_0V         PMU_DCDC_ANA_1_0V

#define PMU_VBUCK2ANA_1_5V              0x30
#define PMU_VBUCK2ANA_1_4V              0x2C
#define PMU_VBUCK2ANA_1_3V              0x28
#define PMU_VBUCK2ANA_1_2V              0x24
#define PMU_VBUCK2ANA_1_1V              0x20
#define PMU_VBUCK2ANA_1_0V              0x1C

#define PMU_DCDC_DIG_1_1V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xD5 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x7A : 0xF8)
#define PMU_DCDC_DIG_1_05V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xC7 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x73 : 0xE8)
#define PMU_DCDC_DIG_1_0V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xBA : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x6B : 0xD8)
#define PMU_DCDC_DIG_0_95V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xAD : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x64 : 0xC8)
#define PMU_DCDC_DIG_0_925V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xA6 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x60 : 0xC0)
#define PMU_DCDC_DIG_0_9V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x9F : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x5C : 0xB8)
#define PMU_DCDC_DIG_0_87V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x97 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x56 : 0xB0)
#define PMU_DCDC_DIG_0_85V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x92 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x54 : 0xA8)
#define PMU_DCDC_DIG_0_8V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x84 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x4C : 0x98)
#define PMU_DCDC_DIG_0_78V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x7F : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x49 : 0x92)
#define PMU_DCDC_DIG_0_775V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x7E : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x48 : 0x90)
#define PMU_DCDC_DIG_0_75V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x77 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x44 : 0x88)
#define PMU_DCDC_DIG_0_74V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x74 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x43 : 0x85)
#define PMU_DCDC_DIG_0_73V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x71 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x41 : 0x82)
#define PMU_DCDC_DIG_0_725V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x70 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x40 : 0x80)
#define PMU_DCDC_DIG_0_72V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x6F : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x3F : 0x7E)
#define PMU_DCDC_DIG_0_71V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x6C : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x3E : 0x7B)
#define PMU_DCDC_DIG_0_7V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x6A : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x3D : 0x78)
#define PMU_DCDC_DIG_0_675V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x63 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x39 : 0x70)
#define PMU_DCDC_DIG_0_65V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x5C : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x35 : 0x68)
#define PMU_DCDC_DIG_0_640V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x59 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x34 : 0x64)
#define PMU_DCDC_DIG_0_635V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x58 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x33 : 0x63)
#define PMU_DCDC_DIG_0_630V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x56 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x32 : 0x61)
#define PMU_DCDC_DIG_0_625V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x55 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x31 : 0x5F)
#define PMU_DCDC_DIG_0_620V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x54 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x30 : 0x5E)
#define PMU_DCDC_DIG_0_610V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x51 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2F : 0x5B)
#define PMU_DCDC_DIG_0_6V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x4F : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2D : 0x58)
#define PMU_DCDC_DIG_0_58V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x49 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2D : 0x52)
#define PMU_DCDC_DIG_0_575V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x48 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2D : 0x50)
#define PMU_DCDC_DIG_0_55V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x41 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2D : 0x48)
#define PMU_DCDC_DIG_0_5V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x33 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2D : 0x38)
#define PMU_DCDC_DIG_0_45V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x26 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2D : 0x28)
#define PMU_DCDC_DIG_0_4V               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x18 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x2D : 0x18)
                                                                         /* 0.8V */                                /* 0.85V */
#define PMU_DCDC_DIG_DEFAULT            ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x84 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x4C : (pmu_type == PMU_CHIP_TYPE_1805) ? 0xA8 : \
                                                                         /* 0.8V   0.9V */ \
                                         (pmu_type == PMU_CHIP_TYPE_1808) ? 0xA0 : 0xC0)
#define PMU_DCDC_DIG_50MV               (PMU_DCDC_DIG_0_75V - PMU_DCDC_DIG_0_7V)

#define PMU_VDIG_1_2V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x29 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x24 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x24 : 0x24)
#define PMU_VDIG_1_1V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x23 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x20 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x22 : 0x20)
#define PMU_VDIG_1_05V                  ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x21 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x1E : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x20 : 0x1E)
#define PMU_VDIG_1_0V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1E : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x1C : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x1E : 0x1C)
#define PMU_VDIG_0_95V                  ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1C : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x1A : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x1C : 0x1A)
#define PMU_VDIG_0_9V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x19 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x18 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x1A : 0x18)
#define PMU_VDIG_0_85V                  ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x17 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x16 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x18 : 0x16)
#define PMU_VDIG_0_8V                   ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) ? 0x14 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x16 : 0x14)
#define PMU_VDIG_0_775V                 ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) ? 0x13 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x15 : 0x13)
#define PMU_VDIG_0_75V                  ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) ? 0x12 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x14 : 0x12)
#define PMU_VDIG_0_725V                 ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) ? 0x11 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x13 : 0x11)
#define PMU_VDIG_0_7V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x0F : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x10 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x12 : 0x10)
#define PMU_VDIG_0_675V                 ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x0E : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x0F : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x11 : 0x0F)
#define PMU_VDIG_0_65V                  ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x0D : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x0E : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x10 : 0x0E)
#define PMU_VDIG_0_625V                 ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x0C : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x0D : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x0F : 0x0D)
#define PMU_VDIG_0_6V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x0A : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x0C : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x0E : 0x0C)
#define PMU_VDIG_0_575V                 ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x09 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x0B : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x0D : 0x0B)
#define PMU_VDIG_0_55V                  ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x08 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x0A : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x0C : 0x0A)
#define PMU_VDIG_0_525V                 ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x07 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x09 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x0B : 0x09)
#define PMU_VDIG_0_5V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x05 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x08 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x0A : 0x08)
#define PMU_VDIG_0_475V                 ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x04 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x07 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x09 : 0x08)
#define PMU_VDIG_0_45V                  ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x03 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x06 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x08 : 0x06)
#define PMU_VDIG_0_4V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x01 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x04 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x06 : 0x04)
#define PMU_VDIG_MAX                    PMU_VDIG_1_2V
#define PMU_VDIG_DEFAULT                ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) ? 0x14 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x19 : \
                                         (pmu_type == PMU_CHIP_TYPE_1808) ? 0x14 : 0x18)

#define PMU1809_LDO_VIO_VBIT_MAX        0x1FF
// 1808 vbit_real = vio_vbit_normal/dsleep << 1 | reg_ldo_vio_res_sel_fine
#define PMU1808_LDO_VIO_VBIT_MAX        0x3F
#define PMU_IO_3_3V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1FF : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x2A : 0x15)
#define PMU_IO_3_2V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1FF : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x28 : 0x14)
#define PMU_IO_3_1V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x18A : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x26 : 0x13)
#define PMU_IO_3_0V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x174 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x24 : 0x12)
#define PMU_IO_2_9V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x15F : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x22 : 0x11)
#define PMU_IO_2_8V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x14A : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x20 : 0x10)
#define PMU_IO_2_7V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x135 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x1E : 0xF)
#define PMU_IO_2_6V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x120 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x1C : 0xE)
#define PMU_IO_2_5V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x10B : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x1A : 0xD)
#define PMU_IO_2_4V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xF6  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x18 : 0xC)
#define PMU_IO_2_3V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xE1  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x16 : 0xB)
#define PMU_IO_2_2V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xCC  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x14 : 0xA)
#define PMU_IO_2_1V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xB7  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x12 : 0x9)
#define PMU_IO_2_0V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xA2  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x10 : 0x8)
#define PMU_IO_1_9V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x8D  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0xE  : 0x7)
#define PMU_IO_1_8V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x78  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0xC  : 0x6)
#define PMU_IO_1_7V                     ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x63  : (pmu_type == PMU_CHIP_TYPE_1808) ? 0xA  : 0x5)

#define PMU_VMEM_2_8V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1F : 0x10)
#define PMU_VMEM_2_0V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x11 : 0x8)
#define PMU_VMEM_1_9V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xF : 0x7)
#define PMU_VMEM_1_8V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xC : 0x6)
#define PMU_VMEM_1_7V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xA : 0x5)
#define PMU_VMEM_1_6V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x8 : 0x4)

#define PMU_CODEC_2_7V                  0xF
#define PMU_CODEC_2_6V                  0xE
#define PMU_CODEC_2_5V                  0xD
#define PMU_CODEC_2_4V                  0xC
#define PMU_CODEC_2_3V                  0xB
#define PMU_CODEC_2_2V                  0xA
#define PMU_CODEC_2_1V                  0x9
#define PMU_CODEC_2_0V                  0x8
#define PMU_CODEC_1_9V                  0x7
#define PMU_CODEC_1_8V                  0x6
#define PMU_CODEC_1_7V                  0x5
#define PMU_CODEC_1_6V                  0x4
#define PMU_CODEC_1_5V                  0x3

#define PMU_HPPA2CODEC_2_3V             0x1C
#define PMU_HPPA2CODEC_2_2V             0x18
#define PMU_HPPA2CODEC_2_1V             0x14
#define PMU_HPPA2CODEC_2_0V             0x10
#define PMU_HPPA2CODEC_1_95V            0xE
#define PMU_HPPA2CODEC_1_9V             0xC
#define PMU_HPPA2CODEC_1_8V             0x8
#define PMU_HPPA2CODEC_1_7V             0x4
#define PMU_HPPA2CODEC_1_6V             0x0

#define PMU1809_LDO_VUSB_VBIT_MAX       0x1FF
// 1808 vbit_real = vusb_vbit_normal/dsleep << 2 | reg_ldo_vusb_res_sel_fine[14:13]
#define PMU1808_LDO_VUSB_VBIT_MAX       0x3F
#define PMU_USB_3_4V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1C7 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x2C : (pmu_type == PMU_CHIP_TYPE_1803) ? 0xA : 0x16)
#define PMU_USB_3_3V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1B2 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x2A : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x9 : 0x15)
#define PMU_USB_3_2V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x19D : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x28 : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x8 : 0x14)
#define PMU_USB_3_1V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x188 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x26 : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x7 : 0x13)
#define PMU_USB_3_0V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x173 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x24 : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x6 : 0x12)
#define PMU_USB_2_9V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x15E : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x22 : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x5 : 0x11)
#define PMU_USB_2_8V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x149 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x20 : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x4 : 0x10)
#define PMU_USB_2_7V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x134 : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x1E : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x3 : 0xF)
#define PMU_USB_2_6V                    ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x11F : (pmu_type == PMU_CHIP_TYPE_1808) ? 0x1C : (pmu_type == PMU_CHIP_TYPE_1803) ? 0x2 : 0xE)

#define PMU_VHPPA_2_0V                  0x8
#define PMU_VHPPA_1_9V                  0x7
#define PMU_VHPPA_1_8V                  0x6
#define PMU_VHPPA_1_7V                  0x5
#define PMU_VHPPA_1_6V                  0x4
#define PMU_VHPPA_1_5V                  0x3

#define PMU_DCDC_HPPA_2_1V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xC4 : 0xEC)
#define PMU_DCDC_HPPA_2_0V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xB7 : 0xDC)
#define PMU_DCDC_HPPA_1_95V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xB1 : 0xD4)
#define PMU_DCDC_HPPA_1_9V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0xAA : 0xCC)
#define PMU_DCDC_HPPA_1_8V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x9D : 0xBC)
#define PMU_DCDC_HPPA_1_75V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x97 : 0xB4)
#define PMU_DCDC_HPPA_1_725V            ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x94 : 0xB0)
#define PMU_DCDC_HPPA_1_7V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x90 : 0xAC)
#define PMU_DCDC_HPPA_1_65V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x8A : 0xA4)
#define PMU_DCDC_HPPA_1_6V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x83 : 0x9C)
#define PMU_DCDC_HPPA_1_55V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x7D : 0x94)
#define PMU_DCDC_HPPA_1_5V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x76 : 0x8C)
#define PMU_DCDC_HPPA_1_45V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x70 : 0x84)
#define PMU_DCDC_HPPA_1_4V              ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x69 : 0x7C)
#define PMU_DCDC_HPPA_1_35V             ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x63 : 0x74)

#define PMU_VMIC_3_3V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x3F : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x1B : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x17 : 0x1B)
#define PMU_VMIC_2_6V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x30 : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x14 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x10 : 0x14)
#define PMU_VMIC_2_0V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x1B : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x0A : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x09 : 0x0A)
#define PMU_VMIC_1_6V                   ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x0E : (pmu_type == PMU_CHIP_TYPE_1806) ? 0x07 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0x06 : 0x06)
#define PMU_VMIC_VBIT_MAX               ((pmu_type == PMU_CHIP_TYPE_1809) ? 0x3F : 0x1F)

#define PMU_VMIC_RES_3_3V               0xF
#define PMU_VMIC_RES_2_8V               0xA
#define PMU_VMIC_RES_2_2V               0x4

#define NTC_TEMPERATURE_RAW_DEFAULT     ((pmu_type == PMU_CHIP_TYPE_1806) ? 0xA75 : (pmu_type == PMU_CHIP_TYPE_1805) ? 0xA75 : 0x25D)

// 00   PMU_REG_METAL_ID
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// 02   PMU_REG_POWER_KEY_CFG
#define REG_PU_VBAT_DIV                     (1 << 15)
#define PU_LPO_DR                           (1 << 14)
#define PU_LPO_REG                          (1 << 13)
#define POWERKEY_WAKEUP_OSC_EN              (1 << 12)
#define RTC_POWER_ON_EN                     (1 << 11)
#define PU_ALL_REG                          (1 << 10)
#define CLK_32K_SEL_SHIFT                   8
#define CLK_32K_SEL_MASK                    (0x3 << CLK_32K_SEL_SHIFT)
#define CLK_32K_SEL(n)                      BITFIELD_VAL(CLK_32K_SEL, n)
#define DEEPSLEEP_MODE_DIGI_DR              (1 << 7)
#define DEEPSLEEP_MODE_DIGI_REG             (1 << 6)
#define REG_VCORE_SSTIME_MODE_SHIFT         4
#define REG_VCORE_SSTIME_MODE_MASK          (0x3 << REG_VCORE_SSTIME_MODE_SHIFT)
#define REG_VCORE_SSTIME_MODE(n)            BITFIELD_VAL(REG_VCORE_SSTIME_MODE, n)
#define REG_LDO_SS_BIAS_EN_DR               (1 << 3)
#define REG_LDO_SS_BIAS_EN                  (1 << 2)
#define REG_LP_EN_VHPPA_DSLEEP              (1 << 1)
#define REG_LP_EN_VHPPA_NORMAL              (1 << 0)

// 03   PMU_REG_BIAS_CFG
#define REG_VSENSE_SEL_VMEM                 (1 << 15)
#define PU_LP_BIAS_LDO_DSLEEP               (1 << 14)
#define PU_LP_BIAS_LDO_DR                   (1 << 13)
#define PU_LP_BIAS_LDO_REG                  (1 << 12)
#define PU_BIAS_LDO_DR                      (1 << 11)
#define PU_BIAS_LDO_REG                     (1 << 10)
#define BG_VBG_SEL_DR                       (1 << 9)
#define BG_VBG_SEL_REG                      (1 << 8)
#define BG_CONSTANT_GM_BIAS_DR              (1 << 7)
#define BG_CONSTANT_GM_BIAS_REG             (1 << 6)
#define BG_CORE_EN_DR                       (1 << 5)
#define BG_CORE_EN_REG                      (1 << 4)
#define REG_PU_LDO_VANA_DR                  (1 << 3)
#define REG_PU_LDO_VANA                     (1 << 2)
#define BG_NOTCH_EN_DR                      (1 << 1)
#define BG_NOTCH_EN_REG                     (1 << 0)

// 04   PMU_REG_SOFT_START_CFG
#define REG_SOFT_START2_VANA_LDO            (1 << 15)
#define EG_SOFT_START2_VCORE_LDO            (1 << 13)
#define REG_SOFT_START2_VIO                 (1 << 11)
#define REG_SOFT_START2_VMEM                (1 << 10)
#define REG_SOFT_START2_VUSB                (1 << 9)
#define REG_SOFT_START2_VBAT2VCORE          (1 << 7)
#define REG_CLK_RCCAL_DIV_SHIFT             0
#define REG_CLK_RCCAL_DIV_MASK              (0x7 << REG_CLK_RCCAL_DIV_SHIFT)
#define REG_CLK_RCCAL_DIV(n)                BITFIELD_VAL(REG_CLK_RCCAL_DIV, n)

#define REG_PU_LDO_VANA_REG                 REG_PU_LDO_VANA

// 05   PMU_REG_CHARGER_CFG
#define REG_PU_LDO_VRTC_RF_DSLEEP           (1 << 15)
#define REG_PU_LDO_VRTC_RF_DR               (1 << 14)
#define REG_PU_LDO_VRTC_RF_REG              (1 << 13)
#define REG_CHARGE_OUT_INTR_MSK             (1 << 12)
#define REG_CHARGE_IN_INTR_MSK              (1 << 11)
#define REG_AC_ON_OUT_EN                    (1 << 10)
#define REG_AC_ON_IN_EN                     (1 << 9)
#define REG_CHARGE_INTR_EN                  (1 << 8)
#define REG_AC_ON_DB_VALUE_SHIFT            0
#define REG_AC_ON_DB_VALUE_MASK             (0xFF << REG_AC_ON_DB_VALUE_SHIFT)
#define REG_AC_ON_DB_VALUE(n)               BITFIELD_VAL(REG_AC_ON_DB_VALUE, n)

// 07   PMU_REG_ANA_CFG
#define REG_LIGHT_LOAD_VANA_BUCK_LDO        (1 << 15)
#define REG_PULLDOWN_VANA_LDO               (1 << 14)
#define LP_EN_VANA_LDO_DSLEEP               (1 << 13)
#define LP_EN_VANA_LDO_DR                   (1 << 12)
#define LP_EN_VANA_LDO_REG                  (1 << 11)
#define REG_PU_LDO_VANA_DSLEEP              (1 << 10)
#define LDO_ANA_VBIT_DSLEEP_SHIFT           5
#define LDO_ANA_VBIT_DSLEEP_MASK            (0x1F << LDO_ANA_VBIT_DSLEEP_SHIFT)
#define LDO_ANA_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_ANA_VBIT_DSLEEP, n)
#define LDO_ANA_VBIT_NORMAL_SHIFT           0
#define LDO_ANA_VBIT_NORMAL_MASK            (0x1F << LDO_ANA_VBIT_NORMAL_SHIFT)
#define LDO_ANA_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_ANA_VBIT_NORMAL, n)

#define LDO_VANA_VBIT_DSLEEP_SHIFT          LDO_ANA_VBIT_DSLEEP_SHIFT
#define LDO_VANA_VBIT_DSLEEP_MASK           LDO_ANA_VBIT_DSLEEP_MASK
#define LDO_VANA_VBIT_NORMAL_SHIFT          LDO_ANA_VBIT_NORMAL_SHIFT
#define LDO_VANA_VBIT_NORMAL_MASK           LDO_ANA_VBIT_NORMAL_MASK

// 08   PMU_REG_DIG_CFG
#define REG_DCDC_CLK_SE_EN                  (1 << 15)
#define LP_EN_VCORE_LDO_DSLEEP              (1 << 14)
#define LP_EN_VCORE_LDO_DR                  (1 << 13)
#define LP_EN_VCORE_LDO_REG                 (1 << 12)
#define LDO_DIG_VBIT_DSLEEP_SHIFT           6
#define LDO_DIG_VBIT_DSLEEP_MASK            (0x3F << LDO_DIG_VBIT_DSLEEP_SHIFT)
#define LDO_DIG_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_DIG_VBIT_DSLEEP, n)
#define LDO_DIG_VBIT_NORMAL_SHIFT           0
#define LDO_DIG_VBIT_NORMAL_MASK            (0x3F << LDO_DIG_VBIT_NORMAL_SHIFT)
#define LDO_DIG_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_DIG_VBIT_NORMAL, n)
#define MAX_LDO_VCORE_VBIT                  (LDO_DIG_VBIT_NORMAL_MASK >> LDO_DIG_VBIT_NORMAL_SHIFT)

#define LDO_VCORE_VBIT_DSLEEP_SHIFT         LDO_DIG_VBIT_DSLEEP_SHIFT
#define LDO_VCORE_VBIT_DSLEEP_MASK          LDO_DIG_VBIT_DSLEEP_MASK
#define LDO_VCORE_VBIT_NORMAL_SHIFT         LDO_DIG_VBIT_NORMAL_SHIFT
#define LDO_VCORE_VBIT_NORMAL_MASK          LDO_DIG_VBIT_NORMAL_MASK

// Real bit is in REG_101 -- but we never change the default value
#define REG_PU_LDO_VCORE_DSLEEP             0

// 09   PMU_REG_IO_CFG
#define LP_EN_VIO_LDO_DSLEEP                (1 << 15)
#define LP_EN_VIO_LDO_DR                    (1 << 14)
#define LP_EN_VIO_LDO_REG                   (1 << 13)
#define REG_PU_LDO_VIO_DSLEEP               (1 << 12)
#define REG_PU_LDO_VIO_DR                   (1 << 11)
#define REG_PU_LDO_VIO                      (1 << 10)
#define REG_LDO_VIO_VBIT_NORMAL_SHIFT       5
#define REG_LDO_VIO_VBIT_NORMAL_MASK        (0x1F << REG_LDO_VIO_VBIT_NORMAL_SHIFT)
#define REG_LDO_VIO_VBIT_NORMAL(n)          BITFIELD_VAL(REG_LDO_VIO_VBIT_NORMAL, n)
#define REG_LDO_VIO_VBIT_DSLEEP_SHIFT       0
#define REG_LDO_VIO_VBIT_DSLEEP_MASK        (0x1F << REG_LDO_VIO_VBIT_DSLEEP_SHIFT)
#define REG_LDO_VIO_VBIT_DSLEEP(n)          BITFIELD_VAL(REG_LDO_VIO_VBIT_DSLEEP, n)

// REG_88
#define PMU1809_REG_LDO_VIO_VBIT_NORMAL_SHIFT       0
#define PMU1809_REG_LDO_VIO_VBIT_NORMAL_MASK        (0x1FF << PMU1809_REG_LDO_VIO_VBIT_NORMAL_SHIFT)
#define PMU1809_REG_LDO_VIO_VBIT_NORMAL(n)          BITFIELD_VAL(PMU1809_REG_LDO_VIO_VBIT_NORMAL, n)

// REG_89
#define PMU1809_REG_LDO_VIO_VBIT_DSLEEP_SHIFT       0
#define PMU1809_REG_LDO_VIO_VBIT_DSLEEP_MASK        (0x1FF << PMU1809_REG_LDO_VIO_VBIT_DSLEEP_SHIFT)
#define PMU1809_REG_LDO_VIO_VBIT_DSLEEP(n)          BITFIELD_VAL(PMU1809_REG_LDO_VIO_VBIT_DSLEEP, n)

#define REG_PU_LDO_VIO_REG                  REG_PU_LDO_VIO
#define LDO_VIO_VBIT_NORMAL_SHIFT           ((pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_REG_LDO_VIO_VBIT_NORMAL_SHIFT   : REG_LDO_VIO_VBIT_NORMAL_SHIFT)
#define LDO_VIO_VBIT_NORMAL_MASK            ((pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_REG_LDO_VIO_VBIT_NORMAL_MASK    : REG_LDO_VIO_VBIT_NORMAL_MASK)
#define LDO_VIO_VBIT_NORMAL(n)              ((pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_REG_LDO_VIO_VBIT_NORMAL(n)      : REG_LDO_VIO_VBIT_NORMAL(n))
#define LDO_VIO_VBIT_DSLEEP_SHIFT           ((pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_REG_LDO_VIO_VBIT_DSLEEP_SHIFT   : REG_LDO_VIO_VBIT_DSLEEP_SHIFT)
#define LDO_VIO_VBIT_DSLEEP_MASK            ((pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_REG_LDO_VIO_VBIT_DSLEEP_MASK    : REG_LDO_VIO_VBIT_DSLEEP_MASK)
#define LDO_VIO_VBIT_DSLEEP(n)              ((pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_REG_LDO_VIO_VBIT_DSLEEP(n)      : REG_LDO_VIO_VBIT_DSLEEP(n))

// 0A   PMU_REG_MEM_CFG
#define LP_EN_VMEM_LDO_DSLEEP               (1 << 15)
#define LP_EN_VMEM_LDO_DR                   (1 << 14)
#define LP_EN_VMEM_LDO_REG                  (1 << 13)
#define REG_PU_LDO_VMEM_DSLEEP              (1 << 12)
#define REG_PU_LDO_VMEM_DR                  (1 << 11)
#define REG_PU_LDO_VMEM_REG                 (1 << 10)
#define LDO_VMEM_VBIT_NORMAL_SHIFT          5
#define LDO_VMEM_VBIT_NORMAL_MASK           (0x1F << LDO_VMEM_VBIT_NORMAL_SHIFT)
#define LDO_VMEM_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_VMEM_VBIT_NORMAL, n)
#define LDO_VMEM_VBIT_DSLEEP_SHIFT          0
#define LDO_VMEM_VBIT_DSLEEP_MASK           (0x1F << LDO_VMEM_VBIT_DSLEEP_SHIFT)
#define LDO_VMEM_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_VMEM_VBIT_DSLEEP, n)

// 0B   PMU_REG_GP_CFG
#define LP_EN_VGP_LDO_DSLEEP                (1 << 15)
#define LP_EN_VGP_LDO_DR                    (1 << 14)
#define LP_EN_VGP_LDO_REG                   (1 << 13)
#define REG_PU_LDO_VGP_DSLEEP               (1 << 12)
#define REG_PU_LDO_VGP_DR                   (1 << 11)
#define REG_PU_LDO_VGP_REG                  (1 << 10)
#define LDO_VGP_VBIT_NORMAL_SHIFT           5
#define LDO_VGP_VBIT_NORMAL_MASK            (0x1F << LDO_VGP_VBIT_NORMAL_SHIFT)
#define LDO_VGP_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_VGP_VBIT_NORMAL, n)
#define LDO_VGP_VBIT_DSLEEP_SHIFT           0
#define LDO_VGP_VBIT_DSLEEP_MASK            (0x1F << LDO_VGP_VBIT_DSLEEP_SHIFT)
#define LDO_VGP_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_VGP_VBIT_DSLEEP, n)

// 0C   PMU_REG_USB_CFG
#define PMU1806_LP_EN_VUSB_LDO_DSLEEP       (1 << 15)
#define PMU1806_LP_EN_VUSB_LDO_DR           (1 << 14)
#define PMU1806_LP_EN_VUSB_LDO_REG          (1 << 13)
#define PMU1806_PU_LDO_VUSB_DSLEEP          (1 << 12)
#define PMU1806_PU_LDO_VUSB_DR              (1 << 11)
#define PMU1806_PU_LDO_VUSB_REG             (1 << 10)
#define PMU1806_LDO_VUSB_VBIT_NORMAL_SHIFT  5
#define PMU1806_LDO_VUSB_VBIT_NORMAL_MASK   (0x1F << LDO_VUSB_VBIT_NORMAL_SHIFT)
#define PMU1806_LDO_VUSB_VBIT_NORMAL(n)     BITFIELD_VAL(LDO_VUSB_VBIT_NORMAL, n)
#define PMU1806_LDO_VUSB_VBIT_DSLEEP_SHIFT  0
#define PMU1806_LDO_VUSB_VBIT_DSLEEP_MASK   (0x1F << LDO_VUSB_VBIT_DSLEEP_SHIFT)
#define PMU1806_LDO_VUSB_VBIT_DSLEEP(n)     BITFIELD_VAL(LDO_VUSB_VBIT_DSLEEP, n)

#define PMU1803_REG_LDO_VANA_LV_MODE        (1 << 15)
#define PMU1803_REG_PULLDOWN_VUSB           (1 << 14)
#define PMU1803_LP_EN_VUSB_LDO_DSLEEP       (1 << 13)
#define PMU1803_LP_EN_VUSB_LDO_DR           (1 << 12)
#define PMU1803_LP_EN_VUSB_LDO_REG          (1 << 11)
#define PMU1803_PU_LDO_VUSB_DSLEEP          (1 << 10)
#define PMU1803_PU_LDO_VUSB_DR              (1 << 9)
#define PMU1803_PU_LDO_VUSB_REG             (1 << 8)
#define PMU1803_LDO_VUSB_VBIT_NORMAL_SHIFT  4
#define PMU1803_LDO_VUSB_VBIT_NORMAL_MASK   (0xF << LDO_VUSB_VBIT_NORMAL_SHIFT)
#define PMU1803_LDO_VUSB_VBIT_NORMAL(n)     BITFIELD_VAL(LDO_VUSB_VBIT_NORMAL, n)
#define PMU1803_LDO_VUSB_VBIT_DSLEEP_SHIFT  0
#define PMU1803_LDO_VUSB_VBIT_DSLEEP_MASK   (0xF << LDO_VUSB_VBIT_DSLEEP_SHIFT)
#define PMU1803_LDO_VUSB_VBIT_DSLEEP(n)     BITFIELD_VAL(LDO_VUSB_VBIT_DSLEEP, n)

// REG_8A
#define PMU1809_LDO_VUSB_VBIT_NORMAL_SHIFT  0
#define PMU1809_LDO_VUSB_VBIT_NORMAL_MASK   (0x1FF << PMU1809_LDO_VUSB_VBIT_NORMAL_SHIFT)
#define PMU1809_LDO_VUSB_VBIT_NORMAL(n)     BITFIELD_VAL(PMU1809_LDO_VUSB_VBIT_NORMAL, n)

// REG_8B
#define PMU1809_LDO_VUSB_VBIT_DSLEEP_SHIFT  0
#define PMU1809_LDO_VUSB_VBIT_DSLEEP_MASK   (0x1FF << PMU1809_LDO_VUSB_VBIT_DSLEEP_SHIFT)
#define PMU1809_LDO_VUSB_VBIT_DSLEEP(n)     BITFIELD_VAL(PMU1809_LDO_VUSB_VBIT_DSLEEP, n)

#define LP_EN_VUSB_LDO_DSLEEP               ((pmu_type == PMU_CHIP_TYPE_1803) ? PMU1803_LP_EN_VUSB_LDO_DSLEEP       : PMU1806_LP_EN_VUSB_LDO_DSLEEP)
#define LP_EN_VUSB_LDO_DR                   ((pmu_type == PMU_CHIP_TYPE_1803) ? PMU1803_LP_EN_VUSB_LDO_DR           : PMU1806_LP_EN_VUSB_LDO_DR)
#define LP_EN_VUSB_LDO_REG                  ((pmu_type == PMU_CHIP_TYPE_1803) ? PMU1803_LP_EN_VUSB_LDO_REG          : PMU1806_LP_EN_VUSB_LDO_REG)
#define REG_PU_LDO_VUSB_DSLEEP              ((pmu_type == PMU_CHIP_TYPE_1803) ? PMU1803_PU_LDO_VUSB_DSLEEP          : PMU1806_PU_LDO_VUSB_DSLEEP)
#define REG_PU_LDO_VUSB_DR                  ((pmu_type == PMU_CHIP_TYPE_1803) ? PMU1803_PU_LDO_VUSB_DR              : PMU1806_PU_LDO_VUSB_DR)
#define REG_PU_LDO_VUSB_REG                 ((pmu_type == PMU_CHIP_TYPE_1803) ? PMU1803_PU_LDO_VUSB_REG             : PMU1806_PU_LDO_VUSB_REG)
#define LDO_VUSB_VBIT_NORMAL_SHIFT          ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_LDO_VUSB_VBIT_NORMAL_SHIFT  : \
                                             (pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_LDO_VUSB_VBIT_NORMAL_SHIFT  : PMU1806_LDO_VUSB_VBIT_NORMAL_SHIFT)
#define LDO_VUSB_VBIT_NORMAL_MASK           ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_LDO_VUSB_VBIT_NORMAL_MASK   : \
                                             (pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_LDO_VUSB_VBIT_NORMAL_MASK   : PMU1806_LDO_VUSB_VBIT_NORMAL_MASK)
#define LDO_VUSB_VBIT_NORMAL(n)             ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_LDO_VUSB_VBIT_NORMAL(n)     : \
                                             (pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_LDO_VUSB_VBIT_NORMAL(n)     : PMU1806_LDO_VUSB_VBIT_NORMAL(n))
#define LDO_VUSB_VBIT_DSLEEP_SHIFT          ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_LDO_VUSB_VBIT_DSLEEP_SHIFT  : \
                                             (pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_LDO_VUSB_VBIT_DSLEEP_SHIFT  : PMU1806_LDO_VUSB_VBIT_DSLEEP_SHIFT)
#define LDO_VUSB_VBIT_DSLEEP_MASK           ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_LDO_VUSB_VBIT_DSLEEP_MASK   : \
                                             (pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_LDO_VUSB_VBIT_DSLEEP_MASK   : PMU1806_LDO_VUSB_VBIT_DSLEEP_MASK)
#define LDO_VUSB_VBIT_DSLEEP(n)             ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_LDO_VUSB_VBIT_DSLEEP(n)     : \
                                             (pmu_type == PMU_CHIP_TYPE_1809) ? PMU1809_LDO_VUSB_VBIT_DSLEEP(n)     : PMU1806_LDO_VUSB_VBIT_DSLEEP(n))

// 0D   PMU_REG_BAT2DIG_CFG
#define LP_EN_VBAT2VCORE_LDO_DR             (1 << 15)
#define LP_EN_VBAT2VCORE_LDO                (1 << 14)
#define REG_PU_VBAT2VCORE_LDO_DR            (1 << 13)
#define REG_PU_VBAT2VCORE_LDO               (1 << 12)
#define LDO_VBAT2VCORE_VBIT_DSLEEP_SHIFT    6
#define LDO_VBAT2VCORE_VBIT_DSLEEP_MASK     (0x3F << LDO_VBAT2VCORE_VBIT_DSLEEP_SHIFT)
#define LDO_VBAT2VCORE_VBIT_DSLEEP(n)       BITFIELD_VAL(LDO_VBAT2VCORE_VBIT_DSLEEP, n)
#define LDO_VBAT2VCORE_VBIT_NORMAL_SHIFT    0
#define LDO_VBAT2VCORE_VBIT_NORMAL_MASK     (0x3F << LDO_VBAT2VCORE_VBIT_NORMAL_SHIFT)
#define LDO_VBAT2VCORE_VBIT_NORMAL(n)       BITFIELD_VAL(LDO_VBAT2VCORE_VBIT_NORMAL, n)

#define LP_EN_VBAT2VCORE_LDO_REG            LP_EN_VBAT2VCORE_LDO
#define REG_PU_LDO_VBAT2VCORE_DR            REG_PU_VBAT2VCORE_LDO_DR
#define REG_PU_LDO_VBAT2VCORE_REG           REG_PU_VBAT2VCORE_LDO
// Real bit is in REG_101 -- but we never change the default value
#define REG_PU_LDO_VBAT2VCORE_DSLEEP        0

// 0E   PMU_REG_HPPA_LDO_EN
#define REG_PULL_DOWN_VHPPA                 (1 << 15)
#define REG_PU_LDO_VHPPA_DSLEEP             (1 << 14)
#define REG_PU_LDO_VHPPA_EN                 (1 << 13)
#define REG_RES_SEL_VHPPA_SHIFT             8
#define REG_RES_SEL_VHPPA_MASK              (0x1F << REG_RES_SEL_VHPPA_SHIFT)
#define REG_RES_SEL_VHPPA(n)                BITFIELD_VAL(REG_RES_SEL_VHPPA, n)
#define REG_LP_BIAS_SEL_LDO_SHIFT           6
#define REG_LP_BIAS_SEL_LDO_MASK            (0x3 << REG_LP_BIAS_SEL_LDO_SHIFT)
#define REG_LP_BIAS_SEL_LDO(n)              BITFIELD_VAL(REG_LP_BIAS_SEL_LDO, n)
#define REG_BYPASS_VBUCK2ANA                (1 << 5)
#define REG_PULLDOWN_VBUCK2ANA              (1 << 4)
#define REG_PULLDOWN_VIO                    (1 << 3)
#define REG_PULLDOWN_VMEM                   (1 << 2)
#define REG_PULLDOWN_VCORE                  (1 << 1)
#define REG_PULLDOWN_VHPPA2VCODEC           (1 << 0)

#define PMU1808_REG_LP_BIAS_SEL_LDO_SHIFT   5
#define PMU1808_REG_LP_BIAS_SEL_LDO_MASK    (0x7 << PMU1808_REG_LP_BIAS_SEL_LDO_SHIFT)
#define PMU1808_REG_LP_BIAS_SEL_LDO(n)      BITFIELD_VAL(PMU1808_REG_LP_BIAS_SEL_LDO, n)

// 0F   PMU_REG_HPPA2CODEC_CFG
#define PU_LDO_VCODEC_DSLEEP                (1 << 15)
#define REG_BYPASS_VCORE                    (1 << 14)
#define REG_PU_LDO_VHPPA2VCODEC_DR          (1 << 13)
#define REG_PU_LDO_VHPPA2VCODEC             (1 << 12)
#define LP_EN_VHPPA2VCODEC_LDO_DR           (1 << 11)
#define LP_EN_VHPPA2VCODEC_LDO              (1 << 10)
#define LDO_VHPPA2VCODEC_VBIT_DSLEEP_SHIFT  5
#define LDO_VHPPA2VCODEC_VBIT_DSLEEP_MASK   (0x1F << LDO_VHPPA2VCODEC_VBIT_DSLEEP_SHIFT)
#define LDO_VHPPA2VCODEC_VBIT_DSLEEP(n)     BITFIELD_VAL(LDO_VHPPA2VCODEC_VBIT_DSLEEP, n)
#define LDO_VHPPA2VCODEC_VBIT_NORMAL_SHIFT  0
#define LDO_VHPPA2VCODEC_VBIT_NORMAL_MASK   (0x1F << LDO_VHPPA2VCODEC_VBIT_NORMAL_SHIFT)
#define LDO_VHPPA2VCODEC_VBIT_NORMAL(n)     BITFIELD_VAL(LDO_VHPPA2VCODEC_VBIT_NORMAL, n)

#define REG_PU_LDO_VHPPA2VCODEC_REG         REG_PU_LDO_VHPPA2VCODEC
#define LP_EN_VHPPA2VCODEC_LDO_REG          LP_EN_VHPPA2VCODEC_LDO
#define REG_PU_LDO_VCODEC_DSLEEP            PU_LDO_VCODEC_DSLEEP

// 10   PMU_REG_CODEC_CFG
#define PU_LDO_VCODEC_DR                    (1 << 15)
#define PU_LDO_VCODEC_REG                   (1 << 14)
#define LP_EN_VCODEC_LDO_DSLEEP             (1 << 13)
#define LP_EN_VCODEC_LDO_DR                 (1 << 12)
#define LP_EN_VCODEC_LDO_REG                (1 << 11)
#define REG_PULLDOWN_VCODEC                 (1 << 10)
#define LDO_VCODEC_VBIT_NORMAL_SHIFT        5
#define LDO_VCODEC_VBIT_NORMAL_MASK         (0x1F << LDO_VCODEC_VBIT_NORMAL_SHIFT)
#define LDO_VCODEC_VBIT_NORMAL(n)           BITFIELD_VAL(LDO_VCODEC_VBIT_NORMAL, n)
#define LDO_VCODEC_VBIT_DSLEEP_SHIFT        0
#define LDO_VCODEC_VBIT_DSLEEP_MASK         (0x1F << LDO_VCODEC_VBIT_DSLEEP_SHIFT)
#define LDO_VCODEC_VBIT_DSLEEP(n)           BITFIELD_VAL(LDO_VCODEC_VBIT_DSLEEP, n)

#define REG_PU_LDO_VCODEC_DR                PU_LDO_VCODEC_DR
#define REG_PU_LDO_VCODEC_REG               PU_LDO_VCODEC_REG

// 11   PMU_REG_RESETN_MODE
#define PMU1806_PWRKEY_LONGPRESS            (1 << 3)
#define PMU1806_HW_RESET                    (1 << 2)
#define PMU1806_RTC_LOADED                  (1 << 1)
#define PMU1806_CHIP_PWR_ON                 (1 << 0)

// 15   PMU_REG_DCDC_DIG_EN
#define REG_UVLO_SEL_SHIFT                  14
#define REG_UVLO_SEL_MASK                   (0x3 << REG_UVLO_SEL_SHIFT)
#define REG_UVLO_SEL(n)                     BITFIELD_VAL(REG_UVLO_SEL, n)
#define REG_PU_LDO_DIG_DR                   (1 << 13)
#define REG_PU_LDO_DIG_REG                  (1 << 12)
#define REG_DCDC1_PFM_SEL_DSLEEP            (1 << 11)
#define REG_DCDC1_PFM_SEL_NORMAL            (1 << 10)
#define REG_DCDC1_ULP_MODE_DSLEEP           (1 << 9)
#define REG_DCDC1_ULP_MODE_NORMAL           (1 << 8)
#define REG_DCDC1_BURST_MODE_SEL_DSLEEP     (1 << 7)
#define REG_DCDC1_BURST_MODE_SEL_NORMAL     (1 << 6)
#define REG_DCDC1_VREF_SEL_DSLEEP_SHIFT     4
#define REG_DCDC1_VREF_SEL_DSLEEP_MASK      (0x3 << REG_DCDC1_VREF_SEL_DSLEEP_SHIFT)
#define REG_DCDC1_VREF_SEL_DSLEEP(n)        BITFIELD_VAL(REG_DCDC1_VREF_SEL_DSLEEP, n)
#define REG_DCDC1_VREF_SEL_NORMAL_SHIFT     2
#define REG_DCDC1_VREF_SEL_NORMAL_MASK      (0x3 << REG_DCDC1_VREF_SEL_NORMAL_SHIFT)
#define REG_DCDC1_VREF_SEL_NORMAL(n)        BITFIELD_VAL(REG_DCDC1_VREF_SEL_NORMAL, n)
#define REG_PU_DCDC1_DR                     (1 << 1)
#define REG_PU_DCDC1                        (1 << 0)

#define REG_PU_LDO_VCORE_DR                 REG_PU_LDO_DIG_DR
#define REG_PU_LDO_VCORE_REG                REG_PU_LDO_DIG_REG

// 16   PMU_REG_DCDC_ANA_CFG_16
#define DCDC2_CAP_BIT_OSC_SHIFT             12
#define DCDC2_CAP_BIT_OSC_MASK              (0xF << DCDC2_CAP_BIT_OSC_SHIFT)
#define DCDC2_CAP_BIT_OSC(n)                BITFIELD_VAL(DCDC2_CAP_BIT_OSC, n)
#define DCDC2_CAP_BIT_SHIFT                 5
#define DCDC2_CAP_BIT_MASK                  (0x7F << DCDC2_CAP_BIT_SHIFT)
#define DCDC2_CAP_BIT(n)                    BITFIELD_VAL(DCDC2_CAP_BIT, n)
#define DCDC2_BURST_THRESHOLD_SHIFT         0
#define DCDC2_BURST_THRESHOLD_MASK          (0x1F << DCDC2_BURST_THRESHOLD_SHIFT)
#define DCDC2_BURST_THRESHOLD(n)            BITFIELD_VAL(DCDC2_BURST_THRESHOLD, n)

// 17   PMU_REG_DCDC_ANA_CFG_17
#define DCDC2_DELTAV_SHIFT                  8
#define DCDC2_DELTAV_MASK                   (0x3F << DCDC2_DELTAV_SHIFT)
#define DCDC2_DELTAV(n)                     BITFIELD_VAL(DCDC2_DELTAV, n)

#define PMU1803_DCDC2_DELTAV_SHIFT          8
#define PMU1803_DCDC2_DELTAV_MASK           (0xF << PMU1803_DCDC2_DELTAV_SHIFT)
#define PMU1803_DCDC2_DELTAV(n)             BITFIELD_VAL(PMU1803_DCDC2_DELTAV, n)

// 18   PMU_REG_DCDC_ANA_CFG_18
#define DCDC2_HYSTERESIS_BIT_SHIFT          13
#define DCDC2_HYSTERESIS_BIT_MASK           (0x7 << DCDC2_HYSTERESIS_BIT_SHIFT)
#define DCDC2_HYSTERESIS_BIT(n)             BITFIELD_VAL(DCDC2_HYSTERESIS_BIT, n)
#define DCDC2_IHALF_IRCOMP                  (1 << 12)
#define DCDC2_INTERNAL_FREQ_SHIFT           9
#define DCDC2_INTERNAL_FREQ_MASK            (0x7 << DCDC2_INTERNAL_FREQ_SHIFT)
#define DCDC2_INTERNAL_FREQ(n)              BITFIELD_VAL(DCDC2_INTERNAL_FREQ, n)
#define DCDC2_IS_GAIN_SHIFT                 4
#define DCDC2_IS_GAIN_MASK                  (0x1F << DCDC2_IS_GAIN_SHIFT)
#define DCDC2_IS_GAIN(n)                    BITFIELD_VAL(DCDC2_IS_GAIN, n)
#define DCDC2_IS_GAIN_PFM_SHIFT             0
#define DCDC2_IS_GAIN_PFM_MASK              (0xF << DCDC2_IS_GAIN_PFM_SHIFT)
#define DCDC2_IS_GAIN_PFM(n)                BITFIELD_VAL(DCDC2_IS_GAIN_PFM, n)

// 19   PMU_REG_DCDC_ANA_CFG_19
#define DCDC2_IX2_ERR                       (1 << 15)
#define DCDC2_IX2_IRCOMP                    (1 << 14)
#define DCDC2_LP_ERR                        (1 << 13)
#define DCDC2_LP_VCOMP                      (1 << 12)
#define DCDC2_PULLDOWN_EN                   (1 << 11)
#define DCDC2_SLOPE_DOUBLE                  (1 << 10)
#define DCDC2_SLOPE_EN_BURST                (1 << 9)
#define DCDC2_SOFT_START_EN                 (1 << 8)
#define DCDC2_SS_TIME_SEL                   (1 << 7)
#define DCDC2_SYNC_DISABLE                  (1 << 6)
#define DCDC2_TEST_MODE_EN                  (1 << 5)
#define DCDC2_ZCD_CAL_BIT_SHIFT             1
#define DCDC2_ZCD_CAL_BIT_MASK              (0xF << DCDC2_ZCD_CAL_BIT_SHIFT)
#define DCDC2_ZCD_CAL_BIT(n)                BITFIELD_VAL(DCDC2_ZCD_CAL_BIT, n)
#define DCDC2_REG_BYPASS                    (1 << 0)

// 1A   PMU_REG_DCDC_HPPA_CFG_1A
#define DCDC3_CAP_BIT_OSC_SHIFT             12
#define DCDC3_CAP_BIT_OSC_MASK              (0xF << DCDC3_CAP_BIT_OSC_SHIFT)
#define DCDC3_CAP_BIT_OSC(n)                BITFIELD_VAL(DCDC3_CAP_BIT_OSC, n)
#define DCDC3_CAP_BIT_SHIFT                 5
#define DCDC3_CAP_BIT_MASK                  (0x7F << DCDC3_CAP_BIT_SHIFT)
#define DCDC3_CAP_BIT(n)                    BITFIELD_VAL(DCDC3_CAP_BIT, n)
#define DCDC3_BURST_THRESHOLD_SHIFT         0
#define DCDC3_BURST_THRESHOLD_MASK          (0x1F << DCDC3_BURST_THRESHOLD_SHIFT)
#define DCDC3_BURST_THRESHOLD(n)            BITFIELD_VAL(DCDC3_BURST_THRESHOLD, n)

// 1B   PMU_REG_DCDC_HPPA_CFG_1B
#define DCDC3_DELTAV_SHIFT                  8
#define DCDC3_DELTAV_MASK                   (0x3F << DCDC3_DELTAV_SHIFT)
#define DCDC3_DELTAV(n)                     BITFIELD_VAL(DCDC3_DELTAV, n)

#define PMU1803_DCDC3_DELTAV_SHIFT          8
#define PMU1803_DCDC3_DELTAV_MASK           (0xF << PMU1803_DCDC3_DELTAV_SHIFT)
#define PMU1803_DCDC3_DELTAV(n)             BITFIELD_VAL(PMU1803_DCDC3_DELTAV, n)

// 1C   PMU_REG_DCDC_HPPA_CFG_1C
#define DCDC3_HYSTERESIS_BIT_SHIFT          13
#define DCDC3_HYSTERESIS_BIT_MASK           (0x7 << DCDC3_HYSTERESIS_BIT_SHIFT)
#define DCDC3_HYSTERESIS_BIT(n)             BITFIELD_VAL(DCDC3_HYSTERESIS_BIT, n)
#define DCDC3_IHALF_IRCOMP                  (1 << 12)
#define DCDC3_INTERNAL_FREQ_SHIFT           9
#define DCDC3_INTERNAL_FREQ_MASK            (0x7 << DCDC3_INTERNAL_FREQ_SHIFT)
#define DCDC3_INTERNAL_FREQ(n)              BITFIELD_VAL(DCDC3_INTERNAL_FREQ, n)
#define DCDC3_IS_GAIN_SHIFT                 4
#define DCDC3_IS_GAIN_MASK                  (0x1F << DCDC3_IS_GAIN_SHIFT)
#define DCDC3_IS_GAIN(n)                    BITFIELD_VAL(DCDC3_IS_GAIN, n)
#define DCDC3_IS_GAIN_PFM_SHIFT             0
#define DCDC3_IS_GAIN_PFM_MASK              (0xF << DCDC3_IS_GAIN_PFM_SHIFT)
#define DCDC3_IS_GAIN_PFM(n)                BITFIELD_VAL(DCDC3_IS_GAIN_PFM, n)

// 1D   PMU_REG_DCDC_HPPA_CFG_1D
#define DCDC3_IX2_ERR                       (1 << 15)
#define DCDC3_IX2_IRCOMP                    (1 << 14)
#define DCDC3_LP_ERR                        (1 << 13)
#define DCDC3_LP_VCOMP                      (1 << 12)
#define DCDC3_PULLDOWN_EN                   (1 << 11)
#define DCDC3_SLOPE_DOUBLE                  (1 << 10)
#define DCDC3_SLOPE_EN_BURST                (1 << 9)
#define DCDC3_SOFT_START_EN                 (1 << 8)
#define DCDC3_SS_TIME_SEL                   (1 << 7)
#define DCDC3_SYNC_DISABLE                  (1 << 6)
#define DCDC3_TEST_MODE_EN                  (1 << 5)
#define DCDC3_ZCD_CAL_BIT_SHIFT             1
#define DCDC3_ZCD_CAL_BIT_MASK              (0xF << DCDC3_ZCD_CAL_BIT_SHIFT)
#define DCDC3_ZCD_CAL_BIT(n)                BITFIELD_VAL(DCDC3_ZCD_CAL_BIT, n)
#define DCDC3_REG_BYPASS                    (1 << 0)

// 1E   PMU_REG_THERM
#define REG_SS_VCORE_EN                     (1 << 15)
#define REG_PMU_VSEL2_SHIFT                 12
#define REG_PMU_VSEL2_MASK                  (0x7 << REG_PMU_VSEL2_SHIFT)
#define REG_PMU_VSEL2(n)                    BITFIELD_VAL(REG_PMU_VSEL2, n)
#define REG_GPADC_RESETN_DR                 (1 << 11)
#define REG_GPADC_RESETN                    (1 << 10)
#define SAR_PWR_BIT_SHIFT                   8
#define SAR_PWR_BIT_MASK                    (0x3 << SAR_PWR_BIT_SHIFT)
#define SAR_PWR_BIT(n)                      BITFIELD_VAL(SAR_PWR_BIT, n)
#define SAR_OP_IBIT_SHIFT                   5
#define SAR_OP_IBIT_MASK                    (0x7 << SAR_OP_IBIT_SHIFT)
#define SAR_OP_IBIT(n)                      BITFIELD_VAL(SAR_OP_IBIT, n)
#define SAR_THERM_GAIN_SHIFT                3
#define SAR_THERM_GAIN_MASK                 (0x3 << SAR_THERM_GAIN_SHIFT)
#define SAR_THERM_GAIN(n)                   BITFIELD_VAL(SAR_THERM_GAIN, n)
#define PMU1806_REG_PU_SAR_VREF_SW          (1 << 2)
#define PMU1803_SAR_VREF_BIT_SHIFT          0
#define PMU1803_SAR_VREF_BIT_MASK           (0x7 << PMU1803_SAR_VREF_BIT_SHIFT)
#define PMU1803_SAR_VREF_BIT(n)             BITFIELD_VAL(PMU1803_SAR_VREF_BIT, n)

// 1F   PMU_REG_SAR_STABLE_SEL
#define SAR_MODE_SEL                        (1 << 15)
#define KEY_DB_DSB                          (1 << 14)
#define KEY_INTERVAL_MODE                   (1 << 13)
#define GPADC_INTERVAL_MODE                 (1 << 12)
#define STATE_RESET                         (1 << 11)
#define SAR_ADC_MODE                        (1 << 10)
#define DELAY_BEFORE_SAMP_SHIFT             8
#define DELAY_BEFORE_SAMP_MASK              (0x3 << DELAY_BEFORE_SAMP_SHIFT)
#define DELAY_BEFORE_SAMP(n)                BITFIELD_VAL(DELAY_BEFORE_SAMP, n)
#define CONV_CLK_INV                        (1 << 7)
#define SAMP_CLK_INV                        (1 << 6)
#define TIME_SAMP_NEG                       (1 << 5)
#define TIME_SAMP_POS                       (1 << 4)
#define SAR_OUT_POLARITY                    (1 << 3)
#define TIMER_SAR_STABLE_SEL_SHIFT          0
#define TIMER_SAR_STABLE_SEL_MASK           (0x7 << TIMER_SAR_STABLE_SEL_SHIFT)
#define TIMER_SAR_STABLE_SEL(n)             BITFIELD_VAL(TIMER_SAR_STABLE_SEL, n)

// 20   PMU_REG_GPADC_EN_DR
#define REG_CLK_GPADC_DIV_VALUE_SHIFT       11
#define REG_CLK_GPADC_DIV_VALUE_MASK        (0x1F << REG_CLK_GPADC_DIV_VALUE_SHIFT)
#define REG_CLK_GPADC_DIV_VALUE(n)          BITFIELD_VAL(REG_CLK_GPADC_DIV_VALUE, n)
#define REG_KEY_IN_DR                       (1 << 10)
#define REG_KEY_IN_REG                      (1 << 9)
#define REG_GPADC_EN_DR                     (1 << 8)
#define REG_GPADC_EN_REG_SHIFT              0
#define REG_GPADC_EN_REG_MASK               (0xFF << REG_GPADC_EN_REG_SHIFT)
#define REG_GPADC_EN_REG(n)                 BITFIELD_VAL(REG_GPADC_EN_REG, n)

// 21   PMU_REG_PWR_SEL
#define REG_PMU_VSEL1_SHIFT                 13
#define REG_PMU_VSEL1_MASK                  (0x7 << REG_PMU_VSEL1_SHIFT)
#define REG_PMU_VSEL1(n)                    BITFIELD_VAL(REG_PMU_VSEL1, n)
#define REG_POWER_SEL_CNT_SHIFT             8
#define REG_POWER_SEL_CNT_MASK              (0x1F << REG_POWER_SEL_CNT_SHIFT)
#define REG_POWER_SEL_CNT(n)                BITFIELD_VAL(REG_POWER_SEL_CNT, n)
#define REG_PWR_SEL_DR                      (1 << 7)
#define REG_PWR_SEL                         (1 << 6)
#define CLK_BG_EN                           (1 << 5)
#define CLK_BG_DIV_VALUE_SHIFT              0
#define CLK_BG_DIV_VALUE_MASK               (0x1F << CLK_BG_DIV_VALUE_SHIFT)
#define CLK_BG_DIV_VALUE(n)                 BITFIELD_VAL(CLK_BG_DIV_VALUE, n)

// 25   PMU_REG_SAR_VREF
#define REG_PULL_DOWN_VBAT2VCORE            (1 << 15)
#define REG_SAR_REF_MODE_SEL                (1 << 14)
#define REG_CLK_GPADC_EN                    (1 << 13)
#define REG_DR_PU_SAR                       (1 << 12)
#define REG_PU_SAR                          (1 << 11)
#define REG_DR_TSC_SAR_BIT                  (1 << 10)
#define SAR_VREF_BIT_SHIFT                  0
#define SAR_VREF_BIT_MASK                   (0xF << SAR_VREF_BIT_SHIFT)
#define SAR_VREF_BIT(n)                     BITFIELD_VAL(SAR_VREF_BIT, n)

// 26   PMU_REG_INT_MASK
#define RESET_EN_VCORE_LOW                  (1 << 15)
#define RTC_INT1_MSK                        (1 << 14)
#define RTC_INT0_MSK                        (1 << 13)
#define KEY_ERR1_INTR_MSK                   (1 << 12)
#define KEY_ERR0_INTR_MSK                   (1 << 11)
#define KEY_PRESS_INTR_MSK                  (1 << 10)
#define KEY_RELEASE_INTR_MSK                (1 << 9)
#define SAMPLE_DONE_INTR_MSK                (1 << 8)
#define CHAN_DATA_INTR_MSK_SHIFT            0
#define CHAN_DATA_INTR_MSK_MASK             (0xFF << CHAN_DATA_INTR_MSK_SHIFT)
#define CHAN_DATA_INTR_MSK(n)               BITFIELD_VAL(CHAN_DATA_INTR_MSK, n)

// 27   PMU_REG_INT_EN
#define REG_BG_SLEEP_MSK                    (1 << 15)
#define RTC_INT_EN_1                        (1 << 14)
#define RTC_INT_EN_0                        (1 << 13)
#define KEY_ERR1_INTR_EN                    (1 << 12)
#define KEY_ERR0_INTR_EN                    (1 << 11)
#define KEY_PRESS_INTR_EN                   (1 << 10)
#define KEY_RELEASE_INTR_EN                 (1 << 9)
#define SAMPLE_DONE_INTR_EN                 (1 << 8)
#define CHAN_DATA_INTR_EN_SHIFT             0
#define CHAN_DATA_INTR_EN_MASK              (0xFF << CHAN_DATA_INTR_EN_SHIFT)
#define CHAN_DATA_INTR_EN(n)                BITFIELD_VAL(CHAN_DATA_INTR_EN, n)

// 2F   RTC_MATCH_MINUTES_SECONDS
#define RTC_MATCH_TIME_MIN_TENS_SHIFT       12
#define RTC_MATCH_TIME_MIN_TENS_MASK        (0x7 << RTC_MATCH_TIME_MIN_TENS_SHIFT)
#define RTC_MATCH_TIME_MIN_TENS(n)          BITFIELD_VAL(RTC_MATCH_TIME_MIN_TENS, n)
#define RTC_MATCH_TIME_MIN_UNITS_SHIFT      8
#define RTC_MATCH_TIME_MIN_UNITS_MASK       (0xF << RTC_MATCH_TIME_MIN_UNITS_SHIFT)
#define RTC_MATCH_TIME_MIN_UNITS(n)         BITFIELD_VAL(RTC_MATCH_TIME_MIN_UNITS, n)
#define RTC_MATCH_TIME_SEC_TENS_SHIFT       4
#define RTC_MATCH_TIME_SEC_TENS_MASK        (0x7 << RTC_MATCH_TIME_SEC_TENS_SHIFT)
#define RTC_MATCH_TIME_SEC_TENS(n)          BITFIELD_VAL(RTC_MATCH_TIME_SEC_TENS, n)
#define RTC_MATCH_TIME_SEC_UNITS_SHIFT      0
#define RTC_MATCH_TIME_SEC_UNITS_MASK       (0xF << RTC_MATCH_TIME_SEC_UNITS_SHIFT)
#define RTC_MATCH_TIME_SEC_UNITS(n)         BITFIELD_VAL(RTC_MATCH_TIME_SEC_UNITS, n)

// 30   RTC_MATCH_HOURS
#define RTC_MATCH_HOURS_TENS_SHIFT          4
#define RTC_MATCH_HOURS_TENS_MASK           (0x3 << RTC_MATCH_HOURS_TENS_SHIFT)
#define RTC_MATCH_HOURS_TENS(n)             BITFIELD_VAL(RTC_MATCH_HOURS_TENS, n)
#define RTC_MATCH_HOURS_UNITS_SHIFT         0
#define RTC_MATCH_HOURS_UNITS_MASK          (0xF << RTC_MATCH_HOURS_UNITS_SHIFT)
#define RTC_MATCH_HOURS_UNITS(n)            BITFIELD_VAL(RTC_MATCH_HOURS_UNITS, n)

// 31   RTC_MATCH_DATAS
#define RTC_MATCH_DATAS_WEEK_SHIFT          13
#define RTC_MATCH_DATAS_WEEK_MASK           (0x7 << RTC_MATCH_DATAS_WEEK_SHIFT)
#define RTC_MATCH_DATAS_WEEK(n)             BITFIELD_VAL(RTC_MATCH_DATAS_WEEK, n)
#define RTC_MATCH_DATAS_MONTH_TENS          (1 << 12)
#define RTC_MATCH_DATAS_MONTH_UNITS_SHIFT   8
#define RTC_MATCH_DATAS_MONTH_UNITS_MASK    (0xF << RTC_MATCH_DATAS_MONTH_UNITS_SHIFT)
#define RTC_MATCH_DATAS_MONTH_UNITS(n)      BITFIELD_VAL(RTC_MATCH_DATAS_MONTH_UNITS, n)
#define RTC_MATCH_DATAS_DAY_TENS_SHIFT      4
#define RTC_MATCH_DATAS_DAY_TENS_MASK       (0x3 << RTC_MATCH_DATAS_DAY_TENS_SHIFT)
#define RTC_MATCH_DATAS_DAY_TENS(n)         BITFIELD_VAL(RTC_MATCH_DATAS_DAY_TENS, n)
#define RTC_MATCH_DATAS_DAY_UNITS_SHIFT     0
#define RTC_MATCH_DATAS_DAY_UNITS_MASK      (0xF << RTC_MATCH_DATAS_DAY_UNITS_SHIFT)
#define RTC_MATCH_DATAS_DAY_UNITS(n)        BITFIELD_VAL(RTC_MATCH_DATAS_DAY_UNITS, n)

// 32   RTC_MATCH_YEARS
#define RTC_MATCH_YEARS_TENS_SHIFT          4
#define RTC_MATCH_YEARS_TENS_MASK           (0xF << RTC_MATCH_YEARS_TENS_SHIFT)
#define RTC_MATCH_YEARS_TENS(n)             BITFIELD_VAL(RTC_MATCH_YEARS_TENS, n)
#define RTC_MATCH_YEARS_UNITS_SHIFT         0
#define RTC_MATCH_YEARS_UNITS_MASK          (0xF << RTC_MATCH_YEARS_UNITS_SHIFT)
#define RTC_MATCH_YEARS_UNITS(n)            BITFIELD_VAL(RTC_MATCH_YEARS_UNITS, n)

// 33   PMU_REG_DCDC_DIG_CFG_33
#define DCDC1_CAP_BIT_OSC_SHIFT             12
#define DCDC1_CAP_BIT_OSC_MASK              (0xF << DCDC1_CAP_BIT_OSC_SHIFT)
#define DCDC1_CAP_BIT_OSC(n)                BITFIELD_VAL(DCDC1_CAP_BIT_OSC, n)
#define DCDC1_CAP_BIT_SHIFT                 5
#define DCDC1_CAP_BIT_MASK                  (0x7F << DCDC1_CAP_BIT_SHIFT)
#define DCDC1_CAP_BIT(n)                    BITFIELD_VAL(DCDC1_CAP_BIT, n)
#define DCDC1_BURST_THRESHOLD_SHIFT         0
#define DCDC1_BURST_THRESHOLD_MASK          (0x1F << DCDC1_BURST_THRESHOLD_SHIFT)
#define DCDC1_BURST_THRESHOLD(n)            BITFIELD_VAL(DCDC1_BURST_THRESHOLD, n)

// 34   PMU_REG_DCDC_DIG_CFG_34
#define DCDC1_DELTAV_SHIFT                  8
#define DCDC1_DELTAV_MASK                   (0x3F << DCDC1_DELTAV_SHIFT)
#define DCDC1_DELTAV(n)                     BITFIELD_VAL(DCDC1_DELTAV, n)

#define PMU1803_DCDC1_DELTAV_SHIFT          8
#define PMU1803_DCDC1_DELTAV_MASK           (0xF << PMU1803_DCDC1_DELTAV_SHIFT)
#define PMU1803_DCDC1_DELTAV(n)             BITFIELD_VAL(PMU1803_DCDC1_DELTAV, n)

// 35   PMU_REG_DCDC_DIG_CFG_35
#define DCDC1_HYSTERESIS_BIT_SHIFT          13
#define DCDC1_HYSTERESIS_BIT_MASK           (0x7 << DCDC1_HYSTERESIS_BIT_SHIFT)
#define DCDC1_HYSTERESIS_BIT(n)             BITFIELD_VAL(DCDC1_HYSTERESIS_BIT, n)
#define DCDC1_IHALF_IRCOMP                  (1 << 12)
#define DCDC1_INTERNAL_FREQ_SHIFT           9
#define DCDC1_INTERNAL_FREQ_MASK            (0x7 << DCDC1_INTERNAL_FREQ_SHIFT)
#define DCDC1_INTERNAL_FREQ(n)              BITFIELD_VAL(DCDC1_INTERNAL_FREQ, n)
#define DCDC1_IS_GAIN_SHIFT                 4
#define DCDC1_IS_GAIN_MASK                  (0x1F << DCDC1_IS_GAIN_SHIFT)
#define DCDC1_IS_GAIN(n)                    BITFIELD_VAL(DCDC1_IS_GAIN, n)
#define DCDC1_IS_GAIN_PFM_SHIFT             0
#define DCDC1_IS_GAIN_PFM_MASK              (0xF << DCDC1_IS_GAIN_PFM_SHIFT)
#define DCDC1_IS_GAIN_PFM(n)                BITFIELD_VAL(DCDC1_IS_GAIN_PFM, n)

// 36   PMU_REG_DCDC_DIG_CFG_36
#define DCDC1_IX2_ERR                       (1 << 15)
#define DCDC1_IX2_IRCOMP                    (1 << 14)
#define DCDC1_LP_ERR                        (1 << 13)
#define DCDC1_LP_VCOMP                      (1 << 12)
#define DCDC1_PULLDOWN_EN                   (1 << 11)
#define DCDC1_SLOPE_DOUBLE                  (1 << 10)
#define DCDC1_SLOPE_EN_BURST                (1 << 9)
#define DCDC1_SOFT_START_EN                 (1 << 8)
#define DCDC1_SS_TIME_SEL                   (1 << 7)
#define DCDC1_SYNC_DISABLE                  (1 << 6)
#define DCDC1_TEST_MODE_EN                  (1 << 5)
#define DCDC1_ZCD_CAL_BIT_SHIFT             1
#define DCDC1_ZCD_CAL_BIT_MASK              (0xF << DCDC1_ZCD_CAL_BIT_SHIFT)
#define DCDC1_ZCD_CAL_BIT(n)                BITFIELD_VAL(DCDC1_ZCD_CAL_BIT, n)
#define DCDC1_REG_BYPASS                    (1 << 0)

// 37   PMU_REG_RTC_CLK_DIV
#define PMU1806_CFG_DIV_RTC_1HZ_SHIFT       0
#define PMU1806_CFG_DIV_RTC_1HZ_MASK        (0xFFFF << 1806_CFG_DIV_RTC_1HZ_SHIFT)
#define PMU1806_CFG_DIV_RTC_1HZ(n)          BITFIELD_VAL(1806_CFG_DIV_RTC_1HZ, n)

// 38   PMU_REG_MIC_BIAS_E
#define REG_MIC_BIASE_CHANSEL_SHIFT         14
#define REG_MIC_BIASE_CHANSEL_MASK          (0x3 << REG_MIC_BIASE_CHANSEL_SHIFT)
#define REG_MIC_BIASE_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASE_CHANSEL, n)
#define REG_MIC_BIASE_EN                    (1 << 13)
#define REG_MIC_BIASE_ENLPF                 (1 << 12)
#define REG_MIC_BIASE_LPFSEL_SHIFT          10
#define REG_MIC_BIASE_LPFSEL_MASK           (0x3 << REG_MIC_BIASE_LPFSEL_SHIFT)
#define REG_MIC_BIASE_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASE_LPFSEL, n)
#define REG_MIC_BIASE_VSEL_SHIFT            5
#define REG_MIC_BIASE_VSEL_MASK             (0x1F << REG_MIC_BIASE_VSEL_SHIFT)
#define REG_MIC_BIASE_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASE_VSEL, n)
#define REG_MIC_LDOE_RES_SHIFT              1
#define REG_MIC_LDOE_RES_MASK               (0xF << REG_MIC_LDOE_RES_SHIFT)
#define REG_MIC_LDOE_RES(n)                 BITFIELD_VAL(REG_MIC_LDOE_RES, n)
#define REG_MIC_LDOE_LOOPCTRL               (1 << 0)

// 39   PMU_REG_MIC_LDO_EN
#define PMU1806_REG_LED_IO1_AIO_EN          (1 << 15)
#define PMU1806_REG_LED_IO2_AIO_EN          (1 << 14)
#define PMU1806_REG_LED_IO1_RX_EN           (1 << 13)
#define PMU1806_REG_LED_IO2_RX_EN           (1 << 12)
#define PMU1806_REG_MIC_LDOA_EN             (1 << 11)
#define PMU1806_REG_MIC_LDOB_EN             (1 << 10)
#define PMU1806_REG_MIC_LDOC_EN             (1 << 9)
#define PMU1806_REG_MIC_LDOD_EN             (1 << 8)
#define PMU1806_REG_MIC_LDOE_EN             (1 << 7)
#define PMU1806_DCDC1_OFFSET_CURRENT_EN     (1 << 6)
#define PMU1806_DCDC2_OFFSET_CURRENT_EN     (1 << 5)
#define PMU1806_DCDC3_OFFSET_CURRENT_EN     (1 << 4)
#define PMU1806_DCDC4_OFFSET_CURRENT_EN     (1 << 3)

#define PMU1803_CLK_32K_COUNT_EN_TRIGGER    (1 << 12)
#define PMU1803_REG_LED_IO1_AIO_EN          (1 << 11)
#define PMU1803_REG_LED_IO2_AIO_EN          (1 << 10)
#define PMU1803_REG_LED_IO1_RX_EN           (1 << 9)
#define PMU1803_REG_LED_IO2_RX_EN           (1 << 8)
#define PMU1803_REG_MIC_LDOA_EN             (1 << 7)
#define PMU1803_REG_MIC_LDOB_EN             (1 << 6)
#define PMU1803_REG_MIC_LDOC_EN             (1 << 5)
#define PMU1803_REG_SAR_HIGH_AVDD18_EN      (1 << 4)
#define PMU1803_DCDC1_OFFSET_CURRENT_EN     (1 << 3)
#define PMU1803_DCDC2_OFFSET_CURRENT_EN     (1 << 2)
#define PMU1803_DCDC3_OFFSET_CURRENT_EN     (1 << 1)
#define PMU1803_DCDC4_OFFSET_CURRENT_EN     (1 << 0)

#define REG_LED_IO1_AIO_EN                  ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_LED_IO1_AIO_EN      : PMU1806_REG_LED_IO1_AIO_EN)
#define REG_LED_IO2_AIO_EN                  ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_LED_IO2_AIO_EN      : PMU1806_REG_LED_IO2_AIO_EN)
#define REG_LED_IO1_RX_EN                   ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_LED_IO1_RX_EN       : PMU1806_REG_LED_IO1_RX_EN)
#define REG_LED_IO2_RX_EN                   ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_LED_IO2_RX_EN       : PMU1806_REG_LED_IO2_RX_EN)
#define REG_MIC_LDOA_EN                     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_MIC_LDOA_EN         : PMU1806_REG_MIC_LDOA_EN)
#define REG_MIC_LDOB_EN                     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_MIC_LDOB_EN         : PMU1806_REG_MIC_LDOB_EN)
#define REG_MIC_LDOC_EN                     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_MIC_LDOC_EN         : PMU1806_REG_MIC_LDOC_EN)
#define REG_MIC_LDOD_EN                     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? 0                               : PMU1806_REG_MIC_LDOD_EN)
#define REG_MIC_LDOE_EN                     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? 0                               : PMU1806_REG_MIC_LDOE_EN)
#define DCDC1_OFFSET_CURRENT_EN             ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_DCDC1_OFFSET_CURRENT_EN : PMU1806_DCDC1_OFFSET_CURRENT_EN)
#define DCDC2_OFFSET_CURRENT_EN             ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_DCDC2_OFFSET_CURRENT_EN : PMU1806_DCDC2_OFFSET_CURRENT_EN)
#define DCDC3_OFFSET_CURRENT_EN             ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_DCDC3_OFFSET_CURRENT_EN : PMU1806_DCDC3_OFFSET_CURRENT_EN)
#define DCDC4_OFFSET_CURRENT_EN             ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_DCDC4_OFFSET_CURRENT_EN : PMU1806_DCDC4_OFFSET_CURRENT_EN)

// 3A   PMU_REG_LED_CFG
#define REG_LED_IO1_IBIT_SHIFT              14
#define REG_LED_IO1_IBIT_MASK               (0x3 << REG_LED_IO1_IBIT_SHIFT)
#define REG_LED_IO1_IBIT(n)                 BITFIELD_VAL(REG_LED_IO1_IBIT, n)
#define REG_LED_IO1_OENB                    (1 << 13)
#define REG_LED_IO1_PDEN                    (1 << 12)
#define REG_LED_IO1_PU                      (1 << 11)
#define REG_LED_IO1_PUEN                    (1 << 10)
#define REG_LED_IO1_SEL_SHIFT               8
#define REG_LED_IO1_SEL_MASK                (0x3 << REG_LED_IO1_SEL_SHIFT)
#define REG_LED_IO1_SEL(n)                  BITFIELD_VAL(REG_LED_IO1_SEL, n)
#define REG_LED_IO2_IBIT_SHIFT              6
#define REG_LED_IO2_IBIT_MASK               (0x3 << REG_LED_IO2_IBIT_SHIFT)
#define REG_LED_IO2_IBIT(n)                 BITFIELD_VAL(REG_LED_IO2_IBIT, n)
#define REG_LED_IO2_OENB                    (1 << 5)
#define REG_LED_IO2_PDEN                    (1 << 4)
#define REG_LED_IO2_PU                      (1 << 3)
#define REG_LED_IO2_PUEN                    (1 << 2)
#define REG_LED_IO2_SEL_SHIFT               0
#define REG_LED_IO2_SEL_MASK                (0x3 << REG_LED_IO2_SEL_SHIFT)
#define REG_LED_IO2_SEL(n)                  BITFIELD_VAL(REG_LED_IO2_SEL, n)

// 3B   PMU_REG_MIC_BIAS_A
#define REG_MIC_BIASA_CHANSEL_SHIFT         14
#define REG_MIC_BIASA_CHANSEL_MASK          (0x3 << REG_MIC_BIASA_CHANSEL_SHIFT)
#define REG_MIC_BIASA_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASA_CHANSEL, n)
#define REG_MIC_BIASA_EN                    (1 << 13)
#define REG_MIC_BIASA_ENLPF                 (1 << 12)
#define REG_MIC_BIASA_LPFSEL_SHIFT          10
#define REG_MIC_BIASA_LPFSEL_MASK           (0x3 << REG_MIC_BIASA_LPFSEL_SHIFT)
#define REG_MIC_BIASA_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASA_LPFSEL, n)
#define REG_MIC_BIASA_VSEL_SHIFT            5
#define REG_MIC_BIASA_VSEL_MASK             (0x1F << REG_MIC_BIASA_VSEL_SHIFT)
#define REG_MIC_BIASA_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASA_VSEL, n)
#define REG_MIC_LDOA_RES_SHIFT              1
#define REG_MIC_LDOA_RES_MASK               (0xF << REG_MIC_LDOA_RES_SHIFT)
#define REG_MIC_LDOA_RES(n)                 BITFIELD_VAL(REG_MIC_LDOA_RES, n)
#define REG_MIC_LDOA_LOOPCTRL               (1 << 0)

// 3C   PMU_REG_MIC_BIAS_B
#define REG_MIC_BIASB_CHANSEL_SHIFT         14
#define REG_MIC_BIASB_CHANSEL_MASK          (0x3 << REG_MIC_BIASB_CHANSEL_SHIFT)
#define REG_MIC_BIASB_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASB_CHANSEL, n)
#define REG_MIC_BIASB_EN                    (1 << 13)
#define REG_MIC_BIASB_ENLPF                 (1 << 12)
#define REG_MIC_BIASB_LPFSEL_SHIFT          10
#define REG_MIC_BIASB_LPFSEL_MASK           (0x3 << REG_MIC_BIASB_LPFSEL_SHIFT)
#define REG_MIC_BIASB_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASB_LPFSEL, n)
#define REG_MIC_BIASB_VSEL_SHIFT            5
#define REG_MIC_BIASB_VSEL_MASK             (0x1F << REG_MIC_BIASB_VSEL_SHIFT)
#define REG_MIC_BIASB_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASB_VSEL, n)
#define REG_MIC_LDOB_RES_SHIFT              1
#define REG_MIC_LDOB_RES_MASK               (0xF << REG_MIC_LDOB_RES_SHIFT)
#define REG_MIC_LDOB_RES(n)                 BITFIELD_VAL(REG_MIC_LDOB_RES, n)
#define REG_MIC_LDOB_LOOPCTRL               (1 << 0)

// 3D   PMU_REG_MIC_BIAS_C
#define REG_MIC_BIASC_CHANSEL_SHIFT         14
#define REG_MIC_BIASC_CHANSEL_MASK          (0x3 << REG_MIC_BIASC_CHANSEL_SHIFT)
#define REG_MIC_BIASC_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASC_CHANSEL, n)
#define REG_MIC_BIASC_EN                    (1 << 13)
#define REG_MIC_BIASC_ENLPF                 (1 << 12)
#define REG_MIC_BIASC_LPFSEL_SHIFT          10
#define REG_MIC_BIASC_LPFSEL_MASK           (0x3 << REG_MIC_BIASC_LPFSEL_SHIFT)
#define REG_MIC_BIASC_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASC_LPFSEL, n)
#define REG_MIC_BIASC_VSEL_SHIFT            5
#define REG_MIC_BIASC_VSEL_MASK             (0x1F << REG_MIC_BIASC_VSEL_SHIFT)
#define REG_MIC_BIASC_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASC_VSEL, n)
#define REG_MIC_LDOC_RES_SHIFT              1
#define REG_MIC_LDOC_RES_MASK               (0xF << REG_MIC_LDOC_RES_SHIFT)
#define REG_MIC_LDOC_RES(n)                 BITFIELD_VAL(REG_MIC_LDOC_RES, n)
#define REG_MIC_LDOC_LOOPCTRL               (1 << 0)

// 3F   PMU_REG_EFUSE_CTRL
#define TSMC_EFUSE_A_SHIFT                  0
#define TSMC_EFUSE_A_MASK                   (0x3F << TSMC_EFUSE_A_SHIFT)
#define TSMC_EFUSE_A(n)                     BITFIELD_VAL(TSMC_EFUSE_A, n)
#define TSMC_EFUSE_CLK_EN                   (1 << 8)
#define TSMC_EFUSE_NR                       (1 << 9)
#define TSMC_EFUSE_MR                       (1 << 10)
#define MANUAL_MODE                         (1 << 11)
#define TSMC_EFUSE_PGENB                    (1 << 12)
#define REG_MARGIN_READ                     (1 << 13)

// 40   PMU_REG_EFUSE_SEL
#define TSMC_EFUSE_STROBE_TRIG              (1 << 0)
#define TSMC_EFUSE_STROBE_WIDTH_SHIFT       1
#define TSMC_EFUSE_STROBE_WIDTH_MASK        (0xFF << TSMC_EFUSE_STROBE_WIDTH_SHIFT)
#define TSMC_EFUSE_STROBE_WIDTH(n)          BITFIELD_VAL(TSMC_EFUSE_STROBE_WIDTH, n)
#define REG_READ_STROBE_WIDTH_SHIFT         9
#define REG_READ_STROBE_WIDTH_MASK          (0xF << REG_READ_STROBE_WIDTH_SHIFT)
#define REG_READ_STROBE_WIDTH(n)            BITFIELD_VAL(REG_READ_STROBE_WIDTH, n)
#define EFUSE_SEL_SHIFT                     13
#define EFUSE_SEL_MASK                      (0x7 << EFUSE_SEL_SHIFT)
#define EFUSE_SEL(n)                        BITFIELD_VAL(EFUSE_SEL, n)

// 41   PMU_REG_DCDC1_OFFSET
#define DCDC1_OFFSET_BIT_SHIFT              8
#define DCDC1_OFFSET_BIT_MASK               (0xFF << DCDC1_OFFSET_BIT_SHIFT)
#define DCDC1_OFFSET_BIT(n)                 BITFIELD_VAL(DCDC1_OFFSET_BIT, n)
#define DCDC2_OFFSET_BIT_SHIFT              0
#define DCDC2_OFFSET_BIT_MASK               (0xFF << DCDC2_OFFSET_BIT_SHIFT)
#define DCDC2_OFFSET_BIT(n)                 BITFIELD_VAL(DCDC2_OFFSET_BIT, n)

// 42   PMU_REG_DCDC3_OFFSET
#define DCDC3_OFFSET_BIT_SHIFT              8
#define DCDC3_OFFSET_BIT_MASK               (0xFF << DCDC3_OFFSET_BIT_SHIFT)
#define DCDC3_OFFSET_BIT(n)                 BITFIELD_VAL(DCDC3_OFFSET_BIT, n)
#define DCDC4_OFFSET_BIT_SHIFT              0
#define DCDC4_OFFSET_BIT_MASK               (0xFF << DCDC4_OFFSET_BIT_SHIFT)
#define DCDC4_OFFSET_BIT(n)                 BITFIELD_VAL(DCDC4_OFFSET_BIT, n)

// 43   PMU_REG_BUCK2ANA_CFG
#define REG_PU_VBUCK2ANA_LDO_DR             (1 << 15)
#define REG_PU_VBUCK2ANA_LDO                (1 << 14)
#define LP_EN_VBUCK2ANA_LDO_DR              (1 << 13)
#define LP_EN_VBUCK2ANA_LDO                 (1 << 12)
#define LDO_VBUCK2ANA_VBIT_DSLEEP_SHIFT     6
#define LDO_VBUCK2ANA_VBIT_DSLEEP_MASK      (0x3F << LDO_VBUCK2ANA_VBIT_DSLEEP_SHIFT)
#define LDO_VBUCK2ANA_VBIT_DSLEEP(n)        BITFIELD_VAL(LDO_VBUCK2ANA_VBIT_DSLEEP, n)
#define LDO_VBUCK2ANA_VBIT_NORMAL_SHIFT     0
#define LDO_VBUCK2ANA_VBIT_NORMAL_MASK      (0x3F << LDO_VBUCK2ANA_VBIT_NORMAL_SHIFT)
#define LDO_VBUCK2ANA_VBIT_NORMAL(n)        BITFIELD_VAL(LDO_VBUCK2ANA_VBIT_NORMAL, n)

#define REG_PU_LDO_VBUCK2ANA_DR             REG_PU_VBUCK2ANA_LDO_DR
#define REG_PU_LDO_VBUCK2ANA_REG            REG_PU_VBUCK2ANA_LDO
#define LP_EN_VBUCK2ANA_LDO_REG             LP_EN_VBUCK2ANA_LDO
// No pu dsleep cfg
#define REG_PU_LDO_VBUCK2ANA_DSLEEP         0

//1805 43
#define PMU1805_DCDC1_IS_GAIN_SHIFT         0
#define PMU1805_DCDC1_IS_GAIN_MASK          (0x3F << PMU1805_DCDC1_IS_GAIN)
#define PMU1805_DCDC1_IS_GAIN(n)            BITFIELD_VAL(PMU1805_DCDC1_IS_GAIN, n)
#define PMU1805_SAR_VREF_BIT_SHIFT          6
#define PMU1805_SAR_VREF_BIT_MASK           (0x3F << PMU1805_SAR_VREF_BIT_SHIFT)
#define PMU1805_SAR_VREF_BIT(n)             BITFIELD_VAL(PMU1805_SAR_VREF_BIT, n)

// 44   PMU_REG_SLEEP_CFG
#define PMIC_TMODE_1300_SHIFT               0
#define PMIC_TMODE_1300_MASK                (0x7 << PMIC_TMODE_1300_SHIFT)
#define PMIC_TMODE_1300(n)                  BITFIELD_VAL(PMIC_TMODE_1300, n)
#define POWERON_PRESS_EN                    (1 << 3)
#define POWERON_RELEASE_EN                  (1 << 4)
#define SLEEP_ALLOW                         (1 << 5)
#define REG_VHPPA2VCODEC_SLP_ON             (1 << 6)
#define AC_OUT_LDO_ON_EN_BIT7               (1 << 7)
#define AC_OUT_LDO_ON_EN_BIT14              (1 << 14)

#define AC_OUT_LDO_ON_EN                    ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? AC_OUT_LDO_ON_EN_BIT14 : AC_OUT_LDO_ON_EN_BIT7)
#define REG_PU_LDO_VHPPA2VCODEC_DSLEEP      REG_VHPPA2VCODEC_SLP_ON

// 46   PMU_REG_DCDC_DIG_VOLT
#define REG_DCDC1_VBIT_DSLEEP_SHIFT         8
#define REG_DCDC1_VBIT_DSLEEP_MASK          (0xFF << REG_DCDC1_VBIT_DSLEEP_SHIFT)
#define REG_DCDC1_VBIT_DSLEEP(n)            BITFIELD_VAL(REG_DCDC1_VBIT_DSLEEP, n)
#define REG_DCDC1_VBIT_NORMAL_SHIFT         0
#define REG_DCDC1_VBIT_NORMAL_MASK          (0xFF << REG_DCDC1_VBIT_NORMAL_SHIFT)
#define REG_DCDC1_VBIT_NORMAL(n)            BITFIELD_VAL(REG_DCDC1_VBIT_NORMAL, n)
#define MAX_DCDC1_VBIT_VAL                  (REG_DCDC1_VBIT_NORMAL_MASK >> REG_DCDC1_VBIT_NORMAL_SHIFT)

// 47   PMU_REG_DCDC_ANA_VOLT
#define REG_DCDC2_VBIT_DSLEEP_SHIFT         8
#define REG_DCDC2_VBIT_DSLEEP_MASK          (0xFF << REG_DCDC2_VBIT_DSLEEP_SHIFT)
#define REG_DCDC2_VBIT_DSLEEP(n)            BITFIELD_VAL(REG_DCDC2_VBIT_DSLEEP, n)
#define REG_DCDC2_VBIT_NORMAL_SHIFT         0
#define REG_DCDC2_VBIT_NORMAL_MASK          (0xFF << REG_DCDC2_VBIT_NORMAL_SHIFT)
#define REG_DCDC2_VBIT_NORMAL(n)            BITFIELD_VAL(REG_DCDC2_VBIT_NORMAL, n)
#define MAX_DCDC2_VBIT_VAL                  (REG_DCDC2_VBIT_NORMAL_MASK >> REG_DCDC2_VBIT_NORMAL_SHIFT)

// 48   PMU_REG_DCDC_HPPA_VOLT
#define REG_DCDC3_VBIT_DSLEEP_SHIFT         8
#define REG_DCDC3_VBIT_DSLEEP_MASK          (0xFF << REG_DCDC3_VBIT_DSLEEP_SHIFT)
#define REG_DCDC3_VBIT_DSLEEP(n)            BITFIELD_VAL(REG_DCDC3_VBIT_DSLEEP, n)
#define REG_DCDC3_VBIT_NORMAL_SHIFT         0
#define REG_DCDC3_VBIT_NORMAL_MASK          (0xFF << REG_DCDC3_VBIT_NORMAL_SHIFT)
#define REG_DCDC3_VBIT_NORMAL(n)            BITFIELD_VAL(REG_DCDC3_VBIT_NORMAL, n)
#define MAX_DCDC3_VBIT_VAL                  (REG_DCDC3_VBIT_NORMAL_MASK >> REG_DCDC3_VBIT_NORMAL_SHIFT)

// 4A   PMU_REG_DCDC_ANA_EN
#define REG_DCDC2_PFM_SEL_DSLEEP            (1 << 11)
#define REG_DCDC2_PFM_SEL_NORMAL            (1 << 10)
#define REG_DCDC2_ULP_MODE_DSLEEP           (1 << 9)
#define REG_DCDC2_ULP_MODE_NORMAL           (1 << 8)
#define REG_DCDC2_BURST_MODE_SEL_DSLEEP     (1 << 7)
#define REG_DCDC2_BURST_MODE_SEL_NORMAL     (1 << 6)
#define REG_DCDC2_VREF_SEL_DSLEEP_SHIFT     4
#define REG_DCDC2_VREF_SEL_DSLEEP_MASK      (0x3 << REG_DCDC2_VREF_SEL_DSLEEP_SHIFT)
#define REG_DCDC2_VREF_SEL_DSLEEP(n)        BITFIELD_VAL(REG_DCDC2_VREF_SEL_DSLEEP, n)
#define REG_DCDC2_VREF_SEL_NORMAL_SHIFT     2
#define REG_DCDC2_VREF_SEL_NORMAL_MASK      (0x3 << REG_DCDC2_VREF_SEL_NORMAL_SHIFT)
#define REG_DCDC2_VREF_SEL_NORMAL(n)        BITFIELD_VAL(REG_DCDC2_VREF_SEL_NORMAL, n)
#define REG_PU_DCDC2_DR                     (1 << 1)
#define REG_PU_DCDC2                        (1 << 0)

// 4B   PMU_REG_DCDC_HPPA_EN
#define REG_DCDC3_PFM_SEL_DSLEEP            (1 << 11)
#define REG_DCDC3_PFM_SEL_NORMAL            (1 << 10)
#define REG_DCDC3_ULP_MODE_DSLEEP           (1 << 9)
#define REG_DCDC3_ULP_MODE_NORMAL           (1 << 8)
#define REG_DCDC3_BURST_MODE_SEL_DSLEEP     (1 << 7)
#define REG_DCDC3_BURST_MODE_SEL_NORMAL     (1 << 6)
#define REG_DCDC3_VREF_SEL_DSLEEP_SHIFT     4
#define REG_DCDC3_VREF_SEL_DSLEEP_MASK      (0x3 << REG_DCDC3_VREF_SEL_DSLEEP_SHIFT)
#define REG_DCDC3_VREF_SEL_DSLEEP(n)        BITFIELD_VAL(REG_DCDC3_VREF_SEL_DSLEEP, n)
#define REG_DCDC3_VREF_SEL_NORMAL_SHIFT     2
#define REG_DCDC3_VREF_SEL_NORMAL_MASK      (0x3 << REG_DCDC3_VREF_SEL_NORMAL_SHIFT)
#define REG_DCDC3_VREF_SEL_NORMAL(n)        BITFIELD_VAL(REG_DCDC3_VREF_SEL_NORMAL, n)
#define REG_PU_DCDC3_DR                     (1 << 1)
#define REG_PU_DCDC3                        (1 << 0)

// 4C   PMU_REG_PU_VSYS_DIV
#define REG_LDO_VANA_LV_MODE                (1 << 15)
#define REG_PULLDOWN_VUSB                   (1 << 14)
#define REG_PU_VSYS_DIV                     (1 << 13)
#define DCDC3_IS_GAIN_1805_SHIFT            6
#define DCDC3_IS_GAIN_1805_MASK             (0x3F << DCDC3_IS_GAIN_1805_SHIFT)
#define DCDC3_IS_GAIN_1805(n)               BITFIELD_VAL(DCDC3_IS_GAIN_1805, n)
#define DCDC2_IS_GAIN_1805_SHIFT            0
#define DCDC2_IS_GAIN_1805_MASK             (0x3F << DCDC2_IS_GAIN_1805_SHIFT)
#define DCDC2_IS_GAIN_1805(n)               BITFIELD_VAL(DCDC2_IS_GAIN_1805, n)

// 4D   PMU_REG_HW_DET_CFG
#define VCORE_DET_RES_SEL_SHIFT             10
#define VCORE_DET_RES_SEL_MASK              (0x3F << VCORE_DET_RES_SEL_SHIFT)
#define VCORE_DET_RES_SEL(N)                BITFIELD_VAL(VCORE_DET_RES_SEL, n)
#define VCORE_DET_ISEL_SHIFT                8
#define VCORE_DET_ISEL_MASK                 (0x3 << VCORE_DET_ISEL_SHIFT)
#define VCORE_DET_ISEL(N)                   BITFIELD_VAL(VCORE_DET_ISEL, n)
#define VCORE_DET_EN                        (1 << 7)
#define VBAT_DET_RES_SEL_SHIFT              2
#define VBAT_DET_RES_SEL_MASK               (0x1F << VBAT_DET_RES_SEL_SHIFT)
#define VBAT_DET_RES_SEL(N)                 BITFIELD_VAL(VBAT_DET_RES_SEL, n)
#define VBAT_DET_ISEL_SHIFT                 0
#define VBAT_DET_ISEL_MASK                  (0x3 << VBAT_DET_ISEL_SHIFT)
#define VBAT_DET_ISEL(N)                    BITFIELD_VAL(VBAT_DET_ISEL, n)

// 4F   PMU_REG_POWER_OFF
#define SOFT_POWER_OFF                      (1 << 0)
#define HARDWARE_POWER_OFF_EN               (1 << 1)
#define AC_ON_EN                            (1 << 2)
#define RC_CAL_READY                        (1 << 3)
#define KEY_START                           (1 << 4)
#define GPADC_START                         (1 << 5)
#define EFUSE_READ_DONE                     (1 << 6)
#define EFUSE_READ_BUSY                     (1 << 7)
#define VBAT_OVP                            (1 << 8)
#define VCORE_LOW                           (1 << 9)
#define REG_LED_IO1_DATA_IN                 (1 << 10)
#define REG_LED_IO2_DATA_IN                 (1 << 11)
#define RD_EFUSE_REG                        (1 << 12)
#define REG_WDT_LOAD                        (1 << 13)

#define EFUSE_READ_TRIG_SHIFT               (12)
#define EFUSE_READ_TRIG_MASK                (0xF << EFUSE_READ_TRIG_SHIFT)
#define EFUSE_READ_TRIG(n)                  BITFIELD_VAL(EFUSE_READ_TRIG, n)
#define EFUSE_READ_TRIG_WORD                (0xA)

#define PMU1806_SHIPMENT_POWER_OFF          (1 << 6)
#define PMU1806_GPIO3_DATA_IN               (1 << 12)

// 50   PMU1806_REG_VTOI_CFG
#define CHARGER_PU_CLK_26M                  (1 << 14)
#define BG_VTOI_EN_DR                       (1 << 13)
#define BG_VTOI_EN_REG                      (1 << 12)
#define REG_TSC_SAR_BIT_SHIFT               0
#define REG_TSC_SAR_BIT_MASK                (0xFFF << REG_TSC_SAR_BIT_SHIFT)
#define REG_TSC_SAR_BIT(n)                  BITFIELD_VAL(REG_TSC_SAR_BIT, n)

// 52   PMU_REG_INT_STATUS
#define RD_RES1                             (1 << 15)
#define RTC_INT_1                           (1 << 14)
#define RTC_INT_0                           (1 << 13)
#define KEY_ERR1_INTR                       (1 << 12)
#define KEY_ERR0_INTR                       (1 << 11)
#define KEY_PRESS_INTR                      (1 << 10)
#define KEY_RELEASE_INTR                    (1 << 9)
#define SAMPLE_PERIOD_DONE_INTR             (1 << 8)
#define CHAN_DATA_VALID_INTR_SHIFT          0
#define CHAN_DATA_VALID_INTR_MASK           (0xFF << CHAN_DATA_VALID_INTR_SHIFT)
#define CHAN_DATA_VALID_INTR(n)             BITFIELD_VAL(CHAN_DATA_VALID_INTR, n)

#define PMU1806_GPIO_INTR_MSKED             (1 << 15)

// 53   PMU_REG_INT_MSKED_STATUS
// 51   PMU_REG_INT_CLR
#define PMIC_CODEC_PWM                      (1 << 15)
#define RTC_INT1_MSKED                      (1 << 14)
#define RTC_INT0_MSKED                      (1 << 13)
#define KEY_ERR1_INTR_MSKED                 (1 << 12)
#define KEY_ERR0_INTR_MSKED                 (1 << 11)
#define KEY_PRESS_INTR_MSKED                (1 << 10)
#define KEY_RELEASE_INTR_MSKED              (1 << 9)
#define SAMPLE_DONE_INTR_MSKED              (1 << 8)
#define CHAN_DATA_INTR_MSKED_SHIFT          0
#define CHAN_DATA_INTR_MSKED_MASK           (0xFF << CHAN_DATA_INTR_MSKED_SHIFT)
#define CHAN_DATA_INTR_MSKED(n)             BITFIELD_VAL(CHAN_DATA_INTR_MSKED, n)

// 5E   PMU_REG_CHARGER_STATUS
#define DIG_PU_VHPPA                        (1 << 15)
#define DIG_PU_VRTC_RF                      (1 << 14)
#define DIG_PU_VCODEC                       (1 << 13)
#define DIG_PU_VUSB                         (1 << 12)
#define POWER_ON_RELEASE                    (1 << 11)
#define POWER_ON_PRESS                      (1 << 10)
#define POWER_ON                            (1 << 9)
#define DEEPSLEEP_MODE                      (1 << 8)
//#define PMU_LDO_ON                          (1 << 7)
#define PU_OSC_OUT                          (1 << 6)
#define UVLO_LV                             (1 << 5)
#define AC_ON_DET_OUT_MASKED                (1 << 4)
#define AC_ON_DET_IN_MASKED                 (1 << 3)
#define AC_ON                               (1 << 2)
#define AC_ON_DET_OUT                       (1 << 1)
#define AC_ON_DET_IN                        (1 << 0)

#define PMU1806_DIG_PU_VCORE_M              (1 << 15)
#define PMU1806_DIG_PU_VDPA_0P9             (1 << 13)

// 61   PMU1806_REG_XO_32K
#define REG_XO_32K_EXT2_EN                  (1 << 15)
#define REG_XO_32K_EXT1_EN                  (1 << 14)
#define REG_XO_32K_ISEL_VALUE_SHIFT         8
#define REG_XO_32K_ISEL_VALUE_MASK          (0x3F << REG_XO_32K_ISEL_VALUE_SHIFT)
#define REG_XO_32K_ISEL_VALUE(n)            BITFIELD_VAL(REG_XO_32K_ISEL_VALUE, n)
#define REG_XO_32K_ISEL_DR                  (1 << 7)
#define REG_XO_32K_PU_LDO_DR                (1 << 6)
#define REG_XO_32K_PU_LDO                   (1 << 5)
#define REG_XO_32K_LDO_PRECHARGE_VALUE_SHIFT 2
#define REG_XO_32K_LDO_PRECHARGE_VALUE_MASK (0x7 << REG_XO_32K_LDO_PRECHARGE_VALUE_SHIFT)
#define REG_XO_32K_LDO_PRECHARGE_VALUE(n)   BITFIELD_VAL(REG_XO_32K_LDO_PRECHARGE_VALUE, n)
#define REG_XO_32K_LDO_PRECHARGE_DR         (1 << 1)
#define REG_XO_32K_LDO_PRECHARGE            (1 << 0)

// 62   PMU1806_REG_XO_32K_ISEL_REMAP
#define REG_XO_32K_PWR_SEL                  (1 << 15)
#define REG_XO_32K_EXT3_EN                  (1 << 14)
#define REG_XO_32K_ISEL_RAMP_STA_DLY_SHIFT  5
#define REG_XO_32K_ISEL_RAMP_STA_DLY_MASK   (0x1FF << REG_XO_32K_ISEL_RAMP_STA_DLY_SHIFT)
#define REG_XO_32K_ISEL_RAMP_STA_DLY(n)     BITFIELD_VAL(REG_XO_32K_ISEL_RAMP_STA_DLY, n)
#define REG_XO_32K_ISEL_RAMP_T_STEP_SHIFT   0
#define REG_XO_32K_ISEL_RAMP_T_STEP_MASK    (0x1F << REG_XO_32K_ISEL_RAMP_T_STEP_SHIFT)
#define REG_XO_32K_ISEL_RAMP_T_STEP(n)      BITFIELD_VAL(REG_XO_32K_ISEL_RAMP_T_STEP, n)

// 63   PMU1806_REG_1P8_SWITCH
#define REG_EN_SW1_1P8                      (1 << 15)
#define REG_EN_SW1_1P8_DR                   (1 << 14)
#define REG_EN_SW2_1P8                      (1 << 13)
#define REG_EN_SW2_1P8_DR                   (1 << 12)
#define REG_EN_SW3_1P8                      (1 << 11)
#define REG_EN_SW3_1P8_DR                   (1 << 10)
#define REG_EN_SW4_1P8                      (1 << 9)
#define REG_EN_SW4_1P8_DR                   (1 << 8)
#define REG_EN_SW1_1P8_PULL_DOWN_SHIFT      6
#define REG_EN_SW1_1P8_PULL_DOWN_MASK       (0x3 << REG_EN_SW1_1P8_PULL_DOWN_SHIFT)
#define REG_EN_SW1_1P8_PULL_DOWN(n)         BITFIELD_VAL(REG_EN_SW1_1P8_PULL_DOWN, n)
#define REG_EN_SW2_1P8_PULL_DOWN_SHIFT      4
#define REG_EN_SW2_1P8_PULL_DOWN_MASK       (0x3 << REG_EN_SW2_1P8_PULL_DOWN_SHIFT)
#define REG_EN_SW2_1P8_PULL_DOWN(n)         BITFIELD_VAL(REG_EN_SW2_1P8_PULL_DOWN, n)
#define REG_EN_SW3_1P8_PULL_DOWN_SHIFT      2
#define REG_EN_SW3_1P8_PULL_DOWN_MASK       (0x3 << REG_EN_SW3_1P8_PULL_DOWN_SHIFT)
#define REG_EN_SW3_1P8_PULL_DOWN(n)         BITFIELD_VAL(REG_EN_SW3_1P8_PULL_DOWN, n)
#define REG_EN_SW4_1P8_PULL_DOWN_SHIFT      0
#define REG_EN_SW4_1P8_PULL_DOWN_MASK       (0x3 << REG_EN_SW4_1P8_PULL_DOWN_SHIFT)
#define REG_EN_SW4_1P8_PULL_DOWN(n)         BITFIELD_VAL(REG_EN_SW4_1P8_PULL_DOWN, n)

// 65   PMU1806_REG_ID_DET
#define REG_ID_DET1_EN                      (1 << 15)
#define REG_ID_DET1_ISEL_SHIFT              9
#define REG_ID_DET1_ISEL_MASK               (0x3F << REG_ID_DET1_ISEL_SHIFT)
#define REG_ID_DET1_ISEL(n)                 BITFIELD_VAL(REG_ID_DET1_ISEL, n)
#define REG_ID_DET2_EN                      (1 << 8)
#define REG_ID_DET2_ISEL_SHIFT              2
#define REG_ID_DET2_ISEL_MASK               (0x3F << REG_ID_DET2_ISEL_SHIFT)
#define REG_ID_DET2_ISEL(n)                 BITFIELD_VAL(REG_ID_DET2_ISEL, n)
#define REG_PU_VCHRG_DIV                    (1 << 0)

// 66   PMU1806_REG_LDO_VMEM2
#define PMU1806_REG_PU_LDO_VMEM2_DSLEEP     (1 << 15)
#define PMU1806_REG_PU_LDO_VMEM2_RC_SLP     (1 << 14)
#define PMU1806_REG_PU_LDO_VMEM2_DR         (1 << 13)
#define PMU1806_REG_PU_LDO_VMEM2_REG        (1 << 12)
#define PMU1806_LP_EN_VMEM2_LDO_DSLEEP      (1 << 11)
#define PMU1806_LP_EN_VMEM2_LDO_DR          (1 << 10)
#define PMU1806_LP_EN_VMEM2_LDO_REG         (1 << 9)
#define PMU1806_REG_LDO_VMEM2_PULLDOWN      (1 << 7)
#define PMU1806_REG_LDO_VMEM2_BYPASS        (1 << 6)
#define PMU1806_REG_LDO_VMEM2_SOFT_START2   (1 << 5)
#define PMU1806_LDO_VMEM2_VBIT_RC_SHIFT     0
#define PMU1806_LDO_VMEM2_VBIT_RC_MASK      (0x1F << LDO_VMEM2_VBIT_RC_SHIFT)
#define PMU1806_LDO_VMEM2_VBIT_RC(n)        BITFIELD_VAL(LDO_VMEM2_VBIT_RC, n)

// 6B   PMU1806_REG_VUSB_SWITCH
#define REG_EN_SW_VUSB33                    (1 << 15)
#define REG_EN_SW_VUSB33_DR                 (1 << 14)
#define RTC_LPO_IBIAS_SEL_SHIFT             10
#define RTC_LPO_IBIAS_SEL_MASK              (0x3 << RTC_LPO_IBIAS_SEL_SHIFT)
#define RTC_LPO_IBIAS_SEL(n)                BITFIELD_VAL(RTC_LPO_IBIAS_SEL, n)
#define REG_LDO_VDPA_0P9_ILIMIT_SEL_SHIFT   8
#define REG_LDO_VDPA_0P9_ILIMIT_SEL_MASK    (0x3 << REG_LDO_VDPA_0P9_ILIMIT_SEL_SHIFT)
#define REG_LDO_VDPA_0P9_ILIMIT_SEL(n)      BITFIELD_VAL(REG_LDO_VDPA_0P9_ILIMIT_SEL, n)
#define REG_EN_SW_VUSB33_ISEL_SHIFT         4
#define REG_EN_SW_VUSB33_ISEL_MASK          (0xF << REG_EN_SW_VUSB33_ISEL_SHIFT)
#define REG_EN_SW_VUSB33_ISEL(n)            BITFIELD_VAL(REG_EN_SW_VUSB33_ISEL, n)
#define REG_EN_SW_VUSB33_PULL_DOWN_SHIFT    2
#define REG_EN_SW_VUSB33_PULL_DOWN_MASK     (0x3 << REG_EN_SW_VUSB33_PULL_DOWN_SHIFT)
#define REG_EN_SW_VUSB33_PULL_DOWN(n)       BITFIELD_VAL(REG_EN_SW_VUSB33_PULL_DOWN, n)
#define REG_VBG_REFRESH_SEL_SHIFT           0
#define REG_VBG_REFRESH_SEL_MASK            (0x3 << REG_VBG_REFRESH_SEL_SHIFT)
#define REG_VBG_REFRESH_SEL(n)              BITFIELD_VAL(REG_VBG_REFRESH_SEL, n)

// 69   PMU1806_REG_LDO_DIG_L2_CFG
#define REG_LDO_VCORE_L2_BYPASS             (1 << 7)
#define REG_LDO_VCORE_L2_BYPASS_DR          (1 << 6)

// 6C   PMU1806_REG_CODEC_VCM
#define REG_VCM_EN                          (1 << 15)
#define REG_VCM_EN_LPF                      (1 << 14)
#define REG_VCM_LP_EN                       (1 << 13)
#define REG_VCM_LOW_VCM_SHIFT               8
#define REG_VCM_LOW_VCM_MASK                (0xF << REG_VCM_LOW_VCM_SHIFT)
#define REG_VCM_LOW_VCM(n)                  BITFIELD_VAL(REG_VCM_LOW_VCM, n)
#define REG_VCM_LOW_VCM_LP_SHIFT            4
#define REG_VCM_LOW_VCM_LP_MASK             (0xF << REG_VCM_LOW_VCM_LP_SHIFT)
#define REG_VCM_LOW_VCM_LP(n)               BITFIELD_VAL(REG_VCM_LOW_VCM_LP, n)
#define REG_VCM_LOW_VCM_LPF_SHIFT           0
#define REG_VCM_LOW_VCM_LPF_MASK            (0xF << REG_VCM_LOW_VCM_LPF_SHIFT)
#define REG_VCM_LOW_VCM_LPF(n)              BITFIELD_VAL(REG_VCM_LOW_VCM_LPF, n)

// 6F   PMU1803_REG_PMIC_UART_CFG
#define REG_PMIC_UART_OENB_SEL              (1 << 11)
#define REG_PMIC_UART_OENB2                 (1 << 10)
#define REG_PMIC_UART_TX2                   (1 << 9)
#define REG_PMIC_UART_DR2                   (1 << 8)
#define REG_LED_IO2_OENB_PRE                (1 << 7)
#define REG_LED_IO1_OENB_PRE                (1 << 6)
#define REG_PMIC_UART_OENB1                 (1 << 5)
#define REG_PMIC_UART_TX1                   (1 << 4)
#define REG_PMIC_UART_DR1                   (1 << 3)
#define REG_UART_LED2_SEL                   (1 << 2)
#define REG_UART_LED1_SEL                   (1 << 1)
#define REG_GPIO_I_SEL                      (1 << 0)

// 6F   PMU1806_REG_XO_32K_CAP
#define REG_BG_EN_REFRESH                   (1 << 10)
#define REG_BG_EN_REFRESH_DR                (1 << 9)
#define REG_XO_32K_CAPBIT_SHIFT             0
#define REG_XO_32K_CAPBIT_MASK              (0x1FF << REG_XO_32K_CAPBIT_SHIFT)
#define REG_XO_32K_CAPBIT(n)                BITFIELD_VAL(REG_XO_32K_CAPBIT, n)

// 70   PMU1806_REG_SLEEP_CFG_RC
#define REG_VCORE_L2_RAMP_EN                (1 << 8)
#define REG_VCORE_L1_RAMP_EN                (1 << 7)
#define REG_VCORE_M_RAMP_EN                 (1 << 6)
#define REG_RC_MASK1                        (1 << 5)
#define REG_OSC_MASK1                       (1 << 4)
#define RC_MODE2_EN                         (1 << 3)
#define RCSLEEP_MODE_DIGI_DR                (1 << 2)
#define RCSLEEP_MODE_DIGI_REG               (1 << 1)
#define RCSLEEP_ALLOW                       (1 << 0)

// 72   PMU1806_REG_DIG_L_VOLT_RC
#define LDO_VBAT2VCORE_VBIT_RC_SHIFT        6
#define LDO_VBAT2VCORE_VBIT_RC_MASK         (0x3F << LDO_VBAT2VCORE_VBIT_RC_SHIFT)
#define LDO_VBAT2VCORE_VBIT_RC(n)           BITFIELD_VAL(LDO_VBAT2VCORE_VBIT_RC, n)
#define LDO_DIG_L_VBIT_RC_SHIFT             0
#define LDO_DIG_L_VBIT_RC_MASK              (0x3F << LDO_DIG_L_VBIT_RC_SHIFT)
#define LDO_DIG_L_VBIT_RC(n)                BITFIELD_VAL(LDO_DIG_L_VBIT_RC, n)

// 73   PMU1806_REG_USB_VOLT_RC
#define LDO_VUSB_VBIT_RC_SHIFT              5
#define LDO_VUSB_VBIT_RC_MASK               (0x1F << LDO_VUSB_VBIT_RC_SHIFT)
#define LDO_VUSB_VBIT_RC(n)                 BITFIELD_VAL(LDO_VUSB_VBIT_RC, n)
#define LDO_VMEM_VBIT_RC_SHIFT              0
#define LDO_VMEM_VBIT_RC_MASK               (0x1F << LDO_VMEM_VBIT_RC_SHIFT)
#define LDO_VMEM_VBIT_RC(n)                 BITFIELD_VAL(LDO_VMEM_VBIT_RC, n)

// 74   PMU1806_REG_DIG_VOLT_RC
#define REG_DCDC1_BURST_MODE_SEL_RC         (1 << 12)
#define REG_DCDC1_VREF_SEL_RC_SHIFT         10
#define REG_DCDC1_VREF_SEL_RC_MASK          (0x3 << REG_DCDC1_VREF_SEL_RC_SHIFT)
#define REG_DCDC1_VREF_SEL_RC(n)            BITFIELD_VAL(REG_DCDC1_VREF_SEL_RC, n)
#define REG_DCDC1_ULP_MODE_RC               (1 << 9)
#define REG_DCDC1_PFM_SEL_RC                (1 << 8)
#define REG_DCDC1_VBIT_RC_SHIFT             0
#define REG_DCDC1_VBIT_RC_MASK              (0xFF << REG_DCDC1_VBIT_RC_SHIFT)
#define REG_DCDC1_VBIT_RC(n)                BITFIELD_VAL(REG_DCDC1_VBIT_RC, n)

// 75   PMU1806_REG_ANA_VOLT_RC
#define REG_DCDC2_BURST_MODE_SEL_RC         (1 << 12)
#define REG_DCDC2_VREF_SEL_RC_SHIFT         10
#define REG_DCDC2_VREF_SEL_RC_MASK          (0x3 << REG_DCDC2_VREF_SEL_RC_SHIFT)
#define REG_DCDC2_VREF_SEL_RC(n)            BITFIELD_VAL(REG_DCDC2_VREF_SEL_RC, n)
#define REG_DCDC2_ULP_MODE_RC               (1 << 9)
#define REG_DCDC2_PFM_SEL_RC                (1 << 8)
#define REG_DCDC2_VBIT_RC_SHIFT             0
#define REG_DCDC2_VBIT_RC_MASK              (0xFF << REG_DCDC2_VBIT_RC_SHIFT)
#define REG_DCDC2_VBIT_RC(n)                BITFIELD_VAL(REG_DCDC2_VBIT_RC, n)

// 76   PMU1806_REG_HPPA_VOLT_RC
#define REG_DCDC3_BURST_MODE_SEL_RC         (1 << 12)
#define REG_DCDC3_VREF_SEL_RC_SHIFT         10
#define REG_DCDC3_VREF_SEL_RC_MASK          (0x3 << REG_DCDC3_VREF_SEL_RC_SHIFT)
#define REG_DCDC3_VREF_SEL_RC(n)            BITFIELD_VAL(REG_DCDC3_VREF_SEL_RC, n)
#define REG_DCDC3_ULP_MODE_RC               (1 << 9)
#define REG_DCDC3_PFM_SEL_RC                (1 << 8)
#define REG_DCDC3_VBIT_RC_SHIFT             0
#define REG_DCDC3_VBIT_RC_MASK              (0xFF << REG_DCDC3_VBIT_RC_SHIFT)
#define REG_DCDC3_VBIT_RC(n)                BITFIELD_VAL(REG_DCDC3_VBIT_RC, n)

// REG_94
#define BG_VBG_REFRESH_EN_REG               (1 << 9)
#define BG_VBG_REFRESH_EN_DR                (1 << 8)
#define BG_VREF_GEN_EN_REG                  (1 << 7)
#define BG_VREF_GEN_EN_DR                   (1 << 6)
#define BG_VREF_SEL_REG                     (1 << 5)
#define BG_VREF_SEL_DR                      (1 << 4)
#define BG_VREF_REFRESH_EN_REG              (1 << 3)
#define BG_VREF_REFRESH_EN_DR               (1 << 2)
#define BG_OTP_EN_REG                       (1 << 1)
#define BG_OTP_EN_DR                        (1 << 0)

// 101  PMU_REG_DCDC_RAMP_EN
#define REG_MIC_BIASA_IX2                   (1 << 15)
#define REG_MIC_BIASB_IX2                   (1 << 14)
#define REG_MIC_BIASC_IX2                   (1 << 13)
#define REG_MIC_BIASD_IX2                   (1 << 12)
#define REG_MIC_BIASE_IX2                   (1 << 11)
#define REG_PULLDOWN_VGP                    (1 << 10)
#define REG_PU_LDO_DIG_DSLEEP               (1 << 9)
#define LDO_VHPPA2VCODEC_BYPASS             (1 << 8)
#define LDO_VGP_LV_MODE                     (1 << 7)
#define IPTAT_EN                            (1 << 6)
#define REG_PU_AVDD25_ANA                   (1 << 5)
#define REG_DCDC1_RAMP_EN                   (1 << 4)
#define REG_DCDC2_RAMP_EN                   (1 << 3)
#define REG_DCDC3_RAMP_EN                   (1 << 2)
#define REG_PU_VMEM_DELAY_DR                (1 << 1)
#define REG_PU_VMEM_DELAY                   (1 << 0)

// 102  PMU1806_REG_RESERVED_METAL_1
// only for 1806 metal_1
#define RTC_RESETN                          (1 << 8)
#define REG_GPIO_32K_SEL                    (1 << 4)

// 102  PMU_REG_RESERVED_PMU
// only for 1805_metal_id1
#define PMU_REG_RESERVED_BIT9               (1 << 9)
#define PMU_REG_RESERVED_ANA_HV3_SHIFT      5
#define PMU_REG_RESERVED_ANA_HV3_MASK       (0x3 << PMU_REG_RESERVED_ANA_HV3_SHIFT)
#define PMU_REG_RESERVED_ANA_HV3(n)         BITFIELD_VAL(PMU_REG_RESERVED_ANA_HV3, n)
#define PMU_REG_RESERVED_ANA_HV2_SHIFT      2
#define PMU_REG_RESERVED_ANA_HV2_MASK       (0x3 << PMU_REG_RESERVED_ANA_HV2_SHIFT)
#define PMU_REG_RESERVED_ANA_HV2(n)         BITFIELD_VAL(PMU_REG_RESERVED_ANA_HV2, n)
#define PMU_REG_RESERVED_ANA_HV1_SHIFT      0
#define PMU_REG_RESERVED_ANA_HV1_MASK       (0x3 << PMU_REG_RESERVED_ANA_HV1_SHIFT)
#define PMU_REG_RESERVED_ANA_HV1(n)         BITFIELD_VAL(PMU_REG_RESERVED_ANA_HV1, n)

// 102  PMU_REG_RESERVED_PMU
// only for 1805_metal_id2
#define DCDC1_ERR_BIAS                      (1 << 15)
#define DCDC2_ERR_BIAS                      (1 << 14)
#define DCDC3_ERR_BIAS                      (1 << 13)
#define DCDC2_IREF_BURST1                   (1 << 10)
#define DCDC2_IREF_BURST0                   (1 << 9)
#define DCDC1_IREF_BURST1                   (1 << 8)
#define DCDC3_IREF_BURST1                   (1 << 6)
#define DCDC3_IREF_BURST0                   (1 << 5)
#define DCDC1_IREF_BURST0                   (1 << 0)

// 107  PMU_REG_SUBCNT_DATA
#define SUBCNT_DATA2_SHIFT                  8
#define SUBCNT_DATA2_MASK                   (0xFF << SUBCNT_DATA2_SHIFT)
#define SUBCNT_DATA2(n)                     BITFIELD_VAL(SUBCNT_DATA2, n)
#define SUBCNT_DATA3_SHIFT                  0
#define SUBCNT_DATA3_MASK                   (0xFF << SUBCNT_DATA3_SHIFT)
#define SUBCNT_DATA3(n)                     BITFIELD_VAL(SUBCNT_DATA3, n)

// 108  PMU_REG_PWM_BR_EN
#define TG_SUBCNT_D2_ST_SHIFT               9
#define TG_SUBCNT_D2_ST_MASK                (0x7F << TG_SUBCNT_D2_ST_SHIFT)
#define TG_SUBCNT_D2_ST(n)                  BITFIELD_VAL(TG_SUBCNT_D2_ST, n)
#define REG_PWM2_BR_EN                      (1 << 8)
#define TG_SUBCNT_D3_ST_SHIFT               1
#define TG_SUBCNT_D3_ST_MASK                (0x7F << TG_SUBCNT_D3_ST_SHIFT)
#define TG_SUBCNT_D3_ST(n)                  BITFIELD_VAL(TG_SUBCNT_D3_ST, n)
#define REG_PWM3_BR_EN                      (1 << 0)

// 109  PMU_REG_PWM_EN
#define REG_LED0_OUT                        (1 << 15)
#define REG_LED1_OUT                        (1 << 14)
#define PWM_SELECT_EN_SHIFT                 12
#define PWM_SELECT_EN_MASK                  (0x3 << PWM_SELECT_EN_SHIFT)
#define PWM_SELECT_EN(n)                    BITFIELD_VAL(PWM_SELECT_EN, n)
#define PWM_SELECT_INV_SHIFT                10
#define PWM_SELECT_INV_MASK                 (0x3 << PWM_SELECT_INV_SHIFT)
#define PWM_SELECT_INV(n)                   BITFIELD_VAL(PWM_SELECT_INV, n)
#define REG_CLK_PWM_DIV_SHIFT               5
#define REG_CLK_PWM_DIV_MASK                (0x1F << REG_CLK_PWM_DIV_SHIFT)
#define REG_CLK_PWM_DIV(n)                  BITFIELD_VAL(REG_CLK_PWM_DIV, n)

// 1803
#define PMU1803_REG_PWM_CLK_EN_LED2         (1 << 1)
#define PMU1803_REG_PWM_CLK_EN_LED1         (1 << 0)

// 1805 1806 1809
#define PMU_REG_PWM_CLK_EN_LED2             (1 << 15)
#define PMU_REG_PWM_CLK_EN_LED1             (1 << 14)

// 10B  PMU_REG_WDT_TIMER
#define REG_WDT_TIMER_SHIFT                 0
#define REG_WDT_TIMER_MASK                  (0xFFFF << REG_WDT_TIMER_SHIFT)
#define REG_WDT_TIMER(n)                    BITFIELD_VAL(REG_WDT_TIMER, n)

// 10C  PMU_REG_WDT_CFG
#define REG_HW_RESET_TIME_SHIFT             10
#define REG_HW_RESET_TIME_MASK              (0x3F << REG_HW_RESET_TIME_SHIFT)
#define REG_HW_RESET_TIME(n)                BITFIELD_VAL(REG_HW_RESET_TIME, n)
#define REG_HW_RESET_EN                     (1 << 9)
#define REG_WDT_RESET_EN                    (1 << 8)
#define REG_WDT_EN                          (1 << 7)
#define BG_R_TEMP_SHIFT                     4
#define BG_R_TEMP_MASK                      (0x7 << BG_R_TEMP_SHIFT)
#define BG_R_TEMP(n)                        BITFIELD_VAL(BG_R_TEMP, n)
#define BG_TRIM_VBG_SHIFT                   1
#define BG_TRIM_VBG_MASK                    (0x7 << BG_TRIM_VBG_SHIFT)
#define BG_TRIM_VBG(n)                      BITFIELD_VAL(BG_TRIM_VBG, n)
#define SAR_PU_OVP                          (1 << 0)

// 10F  PMU_REG_USB_RET_LDO
#define REG_PU_VUSB_RET_LDO_DR              (1 << 11)
#define REG_PU_VUSB_RET_LDO                 (1 << 10)
#define REG_VBG_SEL_DLY_TIME_SHIFT          6
#define REG_VBG_SEL_DLY_TIME_MASK           (0xF << REG_VBG_SEL_DLY_TIME_SHIFT)
#define REG_VBG_SEL_DLY_TIME(n)             BITFIELD_VAL(REG_VBG_SEL_DLY_TIME, n)
#define STON_SW_DLY_CNT_ECO_SHIFT           2
#define STON_SW_DLY_CNT_ECO_MASK            (0xF << STON_SW_DLY_CNT_ECO_SHIFT)
#define STON_SW_DLY_CNT_ECO(n)              BITFIELD_VAL(STON_SW_DLY_CNT_ECO, n)
#define VBG_SEL_SLEEP_DLY_SEL               (1 << 1)
#define REG_COMMAND_RST_DIS                 (1 << 0)

// 114  PMU1806_REG_METAL_1
#define PMU_GPIO_NEDG_INTR_MASKED_P4_P7     (1 << 7)
#define PMU_GPIO_INTR_MASKED_P4_P7          (1 << 6)
#define PMU_GPIO_NEDG_INTR_MASKED           (1 << 5)
#define PMU_GPIO_INTR_MASKED                (1 << 4)
#define RTC_DATE_SEL                        (1 << 0)

// 115  REG_LDO_DIG_L_CFG
#define LP_EN_VCORE_L_LDO_DSLEEP            (1 << 14)
#define LP_EN_VCORE_L_LDO_DR                (1 << 13)
#define LP_EN_VCORE_L_LDO_REG               (1 << 12)
#define LDO_DIG_VBIT_L_DSLEEP_SHIFT         6
#define LDO_DIG_VBIT_L_DSLEEP_MASK          (0x3F << LDO_DIG_VBIT_L_DSLEEP_SHIFT)
#define LDO_DIG_VBIT_L_DSLEEP(n)            BITFIELD_VAL(LDO_DIG_VBIT_L_DSLEEP, n)
#define LDO_DIG_VBIT_L_NORMAL_SHIFT         0
#define LDO_DIG_VBIT_L_NORMAL_MASK          (0x3F << LDO_DIG_VBIT_L_NORMAL_SHIFT)
#define LDO_DIG_VBIT_L_NORMAL(n)            BITFIELD_VAL(LDO_DIG_VBIT_L_NORMAL, n)
#define MAX_LDO_VCORE_L_VBIT                (LDO_DIG_VBIT_L_NORMAL_MASK >> LDO_DIG_VBIT_L_NORMAL_SHIFT)

// 116  PMU_REG_PU_LDO
#define REG_PU_LDO_DIG_L_DSLEEP             (1 << 15)
#define REG_PU_LDO_DIG_M_DSLEEP             (1 << 14)
#define REG_PU_LDO_DIG_L_DR                 (1 << 13)
#define REG_PU_LDO_DIG_L_REG                (1 << 12)
#define REG_PU_LDO_DIG_M_DR                 (1 << 11)
#define REG_PU_LDO_DIG_M_REG                (1 << 10)
#define REG_PU_VMEM_RET_LDO                 (1 << 9)
#define REG_PU_VMEM_RET_LDO_DR              (1 << 8)
#define REG_PU_VIO_RET_LDO                  (1 << 7)
#define REG_PU_VIO_RET_LDO_DR               (1 << 6)
#define REG_PU_VDCDC_RET_LDO                (1 << 5)
#define REG_PU_VDCDC_RET_LDO_DR             (1 << 4)
#define REG_PU_VCORE_L1_RET_LDO             (1 << 3)
#define REG_PU_VCORE_L1_RET_LDO_DR          (1 << 2)
#define REG_PU_VBAT2VCORE_RET_LDO           (1 << 1)
#define REG_PU_VBAT2VCORE_RET_LDO_DR        (1 << 0)

// external pmu vcore_logic module
#define REG_PU_LDO_VCORE_L_DR               REG_PU_LDO_DIG_L_DR
#define REG_PU_LDO_VCORE_L_REG              REG_PU_LDO_DIG_L_REG
#define REG_PU_LDO_VCORE_L_DSLEEP           REG_PU_LDO_DIG_L_DSLEEP
#define LDO_VCORE_L_VBIT_DSLEEP_MASK        LDO_DIG_VBIT_L_DSLEEP_MASK
#define LDO_VCORE_L_VBIT_DSLEEP_SHIFT       LDO_DIG_VBIT_L_DSLEEP_SHIFT
#define LDO_VCORE_L_VBIT_NORMAL_MASK        LDO_DIG_VBIT_L_NORMAL_MASK
#define LDO_VCORE_L_VBIT_NORMAL_SHIFT       LDO_DIG_VBIT_L_NORMAL_SHIFT

// 11A  PMU_REG_LDO_VCORE_L
#define REG_VCORE_L_BYPASS_DLY_CNT_SHIFT    8
#define REG_VCORE_L_BYPASS_DLY_CNT_MASK     (0xFF << REG_VCORE_L_BYPASS_DLY_CNT_SHIFT)
#define REG_VCORE_L_BYPASS_DLY_CNT(n)       BITFIELD_VAL(REG_VCORE_L_BYPASS_DLY_CNT, n)
#define REG_LDO_VCORE_L_BYPASS              (1 << 2)
#define REG_LDO_VCORE_L_BYPASS_DR           (1 << 1)
//#define REG_PU_LDO_VBAT2VCORE_DSLEEP        (1 << 0)

// 11B  PMU1806_REG_MAIN_LPO
#define REG_PU_MAIN_LPO_DR                  (1 << 1)
#define REG_PU_MAIN_LPO                     (1 << 0)

// 120 PMU1806_REG_LP_BIAS_SEL_LDO
#define LDO_VCORE_L1_RET_BYPASS             (1 << 10)

// 122 PMU1806_REG_SAR_MIC_BIAS_LP
#define REG_MICBIASB_LP_ENABLE              (1 << 11)
#define REG_MICBIASA_LP_ENABLE              (1 << 10)

// 123 PMU_REG_PWM4_EN
#define REG_REG_LED2_OUT                    (1 << 3)
#define REG_PWM4_SELECT_EN                  (1 << 2)
#define REG_PWM4_SELECT_INV                 (1 << 1)
#define REG_PWM4_CLK_EN                     (1 << 0)

// 126 PMU1805_REG_SUBCNT_DATA
#define SUBCNT_DATA4_SHIFT                  8
#define SUBCNT_DATA4_MASK                   (0xFF << SUBCNT_DATA4_SHIFT)
#define SUBCNT_DATA4(n)                     BITFIELD_VAL(SUBCNT_DATA4, n)
#define TG_SUBCNT_D4_ST_SHIFT               0
#define TG_SUBCNT_D4_ST_MASK                (0x7F << TG_SUBCNT_D4_ST_SHIFT)
#define TG_SUBCNT_D4_ST(n)                  BITFIELD_VAL(TG_SUBCNT_D4_ST, n)
#define REG_PWM4_BR_EN                      (1 << 0)

// 127 PMU_REG_GPIO3_CONFIG
#define REG_GPIO3_PU                         (1 << 8)
#define REG_GPIO3_VSEL_SHIFT                 6
#define REG_GPIO3_VSEL_MASK                  (0x3 << REG_GPIO3_VSEL_SHIFT)
#define REG_GPIO3_VSEL(n)                    BITFIELD_VAL(REG_GPIO3_VSEL, n)
#define REG_GPIO3_RX_EN                      (1 << 5)
#define REG_GPIO3_RPU                        (1 << 4)
#define REG_GPIO3_RPD                        (1 << 3)
#define REG_GPIO3_OENB                       (1 << 2)
#define REG_GPIO3_IBIT_SHIFT                 0
#define REG_GPIO3_IBIT_MASK                  (0x3 << REG_GPIO3_IBIT_SHIFT)
#define REG_GPIO3_IBIT(n)                    BITFIELD_VAL(REG_GPIO3_IBIT, n)

// 128  PMU1805_REG_RTC_EN_FLAG
#define PMU1805_RTC_LOADED                  (1 << 0)

// 11C  PMU1806_REG_DCDC1_EDGE_CON
#define DCDC1_EDGE_CON_P_CHANGE_SHIFT       9
#define DCDC1_EDGE_CON_P_CHANGE_MASK        (0x3 << DCDC1_EDGE_CON_P_CHANGE_SHIFT)
#define DCDC1_EDGE_CON_P_CHANGE(n)          BITFIELD_VAL(DCDC1_EDGE_CON_P_CHANGE, n)
#define DCDC1_SLEEP_DELAY                   (1 << 8)
#define DCDC1_LOGIC_IR_CLK_EN               (1 << 7)
#define DCDC1_EDGE_CON_N_SHIFT              5
#define DCDC1_EDGE_CON_N_MASK               (0x3 << DCDC1_EDGE_CON_N_SHIFT)
#define DCDC1_EDGE_CON_N(n)                 BITFIELD_VAL(DCDC1_EDGE_CON_N, n)
#define DCDC1_EDGE_CON_P_SHIFT              3
#define DCDC1_EDGE_CON_P_MASK               (0x3 << DCDC1_EDGE_CON_P_SHIFT)
#define DCDC1_EDGE_CON_P(n)                 BITFIELD_VAL(DCDC1_EDGE_CON_P, n)
#define DCDC1_ULP_RES_SHIFT                 1
#define DCDC1_ULP_RES_MASK                  (0x3 << DCDC1_ULP_RES_SHIFT)
#define DCDC1_ULP_RES(n)                    BITFIELD_VAL(DCDC1_ULP_RES, n)
#define DCDC1_REG_RES_FB                    (1 << 0)

// 11D  PMU1806_REG_DCDC2_EDGE_CON
#define DCDC2_EDGE_CON_P_CHANGE_SHIFT       9
#define DCDC2_EDGE_CON_P_CHANGE_MASK        (0x3 << DCDC2_EDGE_CON_P_CHANGE_SHIFT)
#define DCDC2_EDGE_CON_P_CHANGE(n)          BITFIELD_VAL(DCDC2_EDGE_CON_P_CHANGE, n)
#define DCDC2_SLEEP_DELAY                   (1 << 8)
#define DCDC2_LOGIC_IR_CLK_EN               (1 << 7)
#define DCDC2_EDGE_CON_N_SHIFT              5
#define DCDC2_EDGE_CON_N_MASK               (0x3 << DCDC2_EDGE_CON_N_SHIFT)
#define DCDC2_EDGE_CON_N(n)                 BITFIELD_VAL(DCDC2_EDGE_CON_N, n)
#define DCDC2_EDGE_CON_P_SHIFT              3
#define DCDC2_EDGE_CON_P_MASK               (0x3 << DCDC2_EDGE_CON_P_SHIFT)
#define DCDC2_EDGE_CON_P(n)                 BITFIELD_VAL(DCDC2_EDGE_CON_P, n)
#define DCDC2_ULP_RES_SHIFT                 1
#define DCDC2_ULP_RES_MASK                  (0x3 << DCDC2_ULP_RES_SHIFT)
#define DCDC2_ULP_RES(n)                    BITFIELD_VAL(DCDC2_ULP_RES, n)
#define DCDC2_REG_RES_FB                    (1 << 0)

// 120  PMU1806_REG_LP_BIAS_SEL_LDO
#define LDO_VCORE_L1_RET_BYPASS             (1 << 10)
#define LDO_VSEL_VCORE_RET_SHIFT            7
#define LDO_VSEL_VCORE_RET_MASK             (0x7 << LDO_VSEL_VCORE_RET_SHIFT)
#define LDO_VSEL_VCORE_RET(n)               BITFIELD_VAL(LDO_VSEL_VCORE_RET, n)
#define REG_LP_BIAS_SEL_DCDC_SHIFT          5
#define REG_LP_BIAS_SEL_DCDC_MASK           (0x3 << REG_LP_BIAS_SEL_DCDC_SHIFT)
#define REG_LP_BIAS_SEL_DCDC(n)             BITFIELD_VAL(REG_LP_BIAS_SEL_DCDC, n)
#define REG_1806_LP_BIAS_SEL_LDO_SHIFT      2
#define REG_1806_LP_BIAS_SEL_LDO_MASK       (0x7 << REG_1806_LP_BIAS_SEL_LDO_SHIFT)
#define REG_1806_LP_BIAS_SEL_LDO(n)         BITFIELD_VAL(REG_1806_LP_BIAS_SEL_LDO, n)
#define REG_SOFT_START_VUSB                 (1 << 1)
#define LDO_VBUCK2ANA_LIGHT_LOAD            (1 << 0)

// 121  PMU1805_REG_LP_BIAS_SEL_LDO
#define REG_1805_LP_BIAS_SEL_LDO_SHIFT      10
#define REG_1805_LP_BIAS_SEL_LDO_MASK       (0xF << REG_1805_LP_BIAS_SEL_LDO_SHIFT)
#define REG_1805_LP_BIAS_SEL_LDO(n)         BITFIELD_VAL(REG_1805_LP_BIAS_SEL_LDO, n)
#define RZ_VMEM_RET_SHIFT                   8
#define RZ_VMEM_RET_MASK                    (0x3 << RZ_VMEM_RET_SHIFT)
#define RZ_VMEM_RET(n)                      BITFIELD_VAL(RZ_VMEM_RET, n)
#define RZ_VIO_RET_SHIFT                    6
#define RZ_VIO_RET_MASK                     (0x3 << RZ_VIO_RET_SHIFT)
#define RZ_VIO_RET(n)                       BITFIELD_VAL(RZ_VIO_RET, n)
#define RZ_VDCDC_RET_SHIFT                  4
#define RZ_VDCDC_RET_MASK                   (0x3 << RZ_VDCDC_RET_SHIFT)
#define RZ_VDCDC_RET(n)                     BITFIELD_VAL(RZ_VDCDC_RET, n)
#define RZ_VCORE_L1_RET_SHIFT               2
#define RZ_VCORE_L1_RET_MASK                (0x3 << RZ_VCORE_L1_RET_SHIFT)
#define RZ_VCORE_L1_RET(n)                  BITFIELD_VAL(RZ_VCORE_L1_RET, n)
#define RZ_VBAT2VCORE_RET_SHIFT             0
#define RZ_VBAT2VCORE_RET_MASK              (0x3 << RZ_VBAT2VCORE_RET_SHIFT)
#define RZ_VBAT2VCORE_RET(n)                BITFIELD_VAL(RZ_VBAT2VCORE_RET, n)

// 135  PMU1806_REG_GPIO_IRQ_CONFIG
#define REG_PMU_GPIO_INTR_CLK_EN            (1 << 5)
#define REG_PMU_GPIO_CLK_DIV_SHIFT          0
#define REG_PMU_GPIO_CLK_DIV_MASK           (0x1f << REG_PMU_GPIO_CLK_DIV_SHIFT)
#define REG_PMU_GPIO_CLK_DIV(n)             BITFIELD_VAL(REG_PMU_GPIO_CLK_DIV, n)

// 15D  PMU1806_REG_GPIO_DB_TARGET
#define REG_PMU_GPIO_L_DB_TARGET_SHIFT      0
#define REG_PMU_GPIO_L_DB_TARGET_MASK       (0xff << REG_PMU_GPIO_L_DB_TARGET_SHIFT)
#define REG_PMU_GPIO_L_DB_TARGET(n)         BITFIELD_VAL(REG_PMU_GPIO_L_DB_TARGET, n)
#define REG_PMU_GPIO_H_DB_TARGET_SHIFT      8
#define REG_PMU_GPIO_H_DB_TARGET_MASK       (0xff << REG_PMU_GPIO_H_DB_TARGET_SHIFT)
#define REG_PMU_GPIO_H_DB_TARGET(n)         BITFIELD_VAL(REG_PMU_GPIO_H_DB_TARGET, n)

// 179  RTC_LOAD_MINUTES_SECONDS
#define RTC_LOAD_TIME_MIN_TENS_SHIFT        12
#define RTC_LOAD_TIME_MIN_TENS_MASK         (0x7 << RTC_LOAD_TIME_MIN_TENS_SHIFT)
#define RTC_LOAD_TIME_MIN_TENS(n)           BITFIELD_VAL(RTC_LOAD_TIME_MIN_TENS, n)
#define RTC_LOAD_TIME_MIN_UNITS_SHIFT       8
#define RTC_LOAD_TIME_MIN_UNITS_MASK        (0xF << RTC_LOAD_TIME_MIN_UNITS_SHIFT)
#define RTC_LOAD_TIME_MIN_UNITS(n)          BITFIELD_VAL(RTC_LOAD_TIME_MIN_UNITS, n)
#define RTC_LOAD_TIME_SEC_TENS_SHIFT        4
#define RTC_LOAD_TIME_SEC_TENS_MASK         (0x7 << RTC_LOAD_TIME_SEC_TENS_SHIFT)
#define RTC_LOAD_TIME_SEC_TENS(n)           BITFIELD_VAL(RTC_LOAD_TIME_SEC_TENS, n)
#define RTC_LOAD_TIME_SEC_UNITS_SHIFT       0
#define RTC_LOAD_TIME_SEC_UNITS_MASK        (0xF << RTC_LOAD_TIME_SEC_UNITS_SHIFT)
#define RTC_LOAD_TIME_SEC_UNITS(n)          BITFIELD_VAL(RTC_LOAD_TIME_SEC_UNITS, n)

// 17A  RTC_LOAD_HOURS
#define RTC_LOAD_HOURS_TENS_SHIFT           4
#define RTC_LOAD_HOURS_TENS_MASK            (0x3 << RTC_LOAD_HOURS_TENS_SHIFT)
#define RTC_LOAD_HOURS_TENS(n)              BITFIELD_VAL(RTC_LOAD_HOURS_TENS, n)
#define RTC_LOAD_HOURS_UNITS_SHIFT          0
#define RTC_LOAD_HOURS_UNITS_MASK           (0xF << RTC_LOAD_HOURS_UNITS_SHIFT)
#define RTC_LOAD_HOURS_UNITS(n)             BITFIELD_VAL(RTC_LOAD_HOURS_UNITS, n)

// 17B  RTC_LOAD_DATAS
#define RTC_LOAD_DATAS_WEEK_SHIFT           13
#define RTC_LOAD_DATAS_WEEK_MASK            (0x7 << RTC_LOAD_DATAS_WEEK_SHIFT)
#define RTC_LOAD_DATAS_WEEK(n)              BITFIELD_VAL(RTC_LOAD_DATAS_WEEK, n)
#define RTC_LOAD_DATAS_MONTH_TENS           (1 << 12)
#define RTC_LOAD_DATAS_MONTH_UNITS_SHIFT    8
#define RTC_LOAD_DATAS_MONTH_UNITS_MASK     (0xF << RTC_LOAD_DATAS_MONTH_UNITS_SHIFT)
#define RTC_LOAD_DATAS_MONTH_UNITS(n)       BITFIELD_VAL(RTC_LOAD_DATAS_MONTH_UNITS, n)
#define RTC_LOAD_DATAS_DAY_TENS_SHIFT       4
#define RTC_LOAD_DATAS_DAY_TENS_MASK        (0x3 << RTC_LOAD_DATAS_DAY_TENS_SHIFT)
#define RTC_LOAD_DATAS_DAY_TENS(n)          BITFIELD_VAL(RTC_LOAD_DATAS_DAY_TENS, n)
#define RTC_LOAD_DATAS_DAY_UNITS_SHIFT      0
#define RTC_LOAD_DATAS_DAY_UNITS_MASK       (0xF << RTC_LOAD_DATAS_DAY_UNITS_SHIFT)
#define RTC_LOAD_DATAS_DAY_UNITS(n)         BITFIELD_VAL(RTC_LOAD_DATAS_DAY_UNITS, n)

// 17C  RTC_LOAD_YEARS
#define RTC_LOAD_YEARS_TENS_SHIFT           4
#define RTC_LOAD_YEARS_TENS_MASK            (0xF << RTC_LOAD_YEARS_TENS_SHIFT)
#define RTC_LOAD_YEARS_TENS(n)              BITFIELD_VAL(RTC_LOAD_YEARS_TENS, n)
#define RTC_LOAD_YEARS_UNITS_SHIFT          0
#define RTC_LOAD_YEARS_UNITS_MASK           (0xF << RTC_LOAD_YEARS_UNITS_SHIFT)
#define RTC_LOAD_YEARS_UNITS(n)             BITFIELD_VAL(RTC_LOAD_YEARS_UNITS, n)

// 17D  RTC_GET_MINUTES_SECONDS
#define RTC_GET_TIME_MIN_TENS_SHIFT         12
#define RTC_GET_TIME_MIN_TENS_MASK          (0x7 << RTC_GET_TIME_MIN_TENS_SHIFT)
#define RTC_GET_TIME_MIN_TENS(n)            BITFIELD_VAL(RTC_GET_TIME_MIN_TENS, n)
#define RTC_GET_TIME_MIN_UNITS_SHIFT        8
#define RTC_GET_TIME_MIN_UNITS_MASK         (0xF << RTC_GET_TIME_MIN_UNITS_SHIFT)
#define RTC_GET_TIME_MIN_UNITS(n)           BITFIELD_VAL(RTC_GET_TIME_MIN_UNITS, n)
#define RTC_GET_TIME_SEC_TENS_SHIFT         4
#define RTC_GET_TIME_SEC_TENS_MASK          (0x7 << RTC_GET_TIME_SEC_TENS_SHIFT)
#define RTC_GET_TIME_SEC_TENS(n)            BITFIELD_VAL(RTC_GET_TIME_SEC_TENS, n)
#define RTC_GET_TIME_SEC_UNITS_SHIFT        0
#define RTC_GET_TIME_SEC_UNITS_MASK         (0xF << RTC_GET_TIME_SEC_UNITS_SHIFT)
#define RTC_GET_TIME_SEC_UNITS(n)           BITFIELD_VAL(RTC_GET_TIME_SEC_UNITS, n)

// 17E  RTC_GET_HOURS
#define RTC_GET_HOURS_TENS_SHIFT            4
#define RTC_GET_HOURS_TENS_MASK             (0x3 << RTC_GET_HOURS_TENS_SHIFT)
#define RTC_GET_HOURS_TENS(n)               BITFIELD_VAL(RTC_GET_HOURS_TENS, n)
#define RTC_GET_HOURS_UNITS_SHIFT           0
#define RTC_GET_HOURS_UNITS_MASK            (0xF << RTC_GET_HOURS_UNITS_SHIFT)
#define RTC_GET_HOURS_UNITS(n)              BITFIELD_VAL(RTC_GET_HOURS_UNITS, n)

// 17F  RTC_GET_DATAS
#define RTC_GET_DATAS_WEEK_SHIFT            13
#define RTC_GET_DATAS_WEEK_MASK             (0x7 << RTC_GET_DATAS_WEEK_SHIFT)
#define RTC_GET_DATAS_WEEK(n)               BITFIELD_VAL(RTC_GET_DATAS_WEEK, n)
#define RTC_GET_DATAS_MONTH_TENS            (1 << 12)
#define RTC_GET_DATAS_MONTH_UNITS_SHIFT     8
#define RTC_GET_DATAS_MONTH_UNITS_MASK      (0xF << RTC_GET_DATAS_MONTH_UNITS_SHIFT)
#define RTC_GET_DATAS_MONTH_UNITS(n)        BITFIELD_VAL(RTC_GET_DATAS_MONTH_UNITS, n)
#define RTC_GET_DATAS_DAY_TENS_SHIFT        4
#define RTC_GET_DATAS_DAY_TENS_MASK         (0x3 << RTC_GET_DATAS_DAY_TENS_SHIFT)
#define RTC_GET_DATAS_DAY_TENS(n)           BITFIELD_VAL(RTC_GET_DATAS_DAY_TENS, n)
#define RTC_GET_DATAS_DAY_UNITS_SHIFT       0
#define RTC_GET_DATAS_DAY_UNITS_MASK        (0xF << RTC_GET_DATAS_DAY_UNITS_SHIFT)
#define RTC_GET_DATAS_DAY_UNITS(n)          BITFIELD_VAL(RTC_GET_DATAS_DAY_UNITS, n)

// 180  RTC_GET_YEARS
#define RTC_GET_YEARS_TENS_SHIFT            4
#define RTC_GET_YEARS_TENS_MASK             (0xF << RTC_GET_YEARS_TENS_SHIFT)
#define RTC_GET_YEARS_TENS(n)               BITFIELD_VAL(RTC_GET_YEARS_TENS, n)
#define RTC_GET_YEARS_UNITS_SHIFT           0
#define RTC_GET_YEARS_UNITS_MASK            (0xF << RTC_GET_YEARS_UNITS_SHIFT)
#define RTC_GET_YEARS_UNITS(n)              BITFIELD_VAL(RTC_GET_YEARS_UNITS, n)

// 50   PMU1808_REG_LDO_ON_SOURCE
#define PMU1808_LDO_ON_SOURCE_SHIFT         0
#define PMU1808_LDO_ON_SOURCE_MASK          (0x7 << PMU1808_LDO_ON_SOURCE_SHIFT)
#define PMU1808_LDO_ON_SOURCE(n)            BITFIELD_VAL(PMU1808_LDO_ON_SOURCE, n)

// 78   PMU1808_REG_ABORT_SOURCE
#define PMU1808_ABORT_SOURCE_SHIFT          0
#define PMU1808_ABORT_SOURCE_MASK           (0x7 << PMU1808_ABORT_SOURCE_SHIFT)
#define PMU1808_ABORT_SOURCE(n)             BITFIELD_VAL(PMU1808_ABORT_SOURCE, n)

// BB   PMU1808_REG_VIO_CFG_BB
#define REG_LDO_VIO_RES_SEL_FINE            (1 << 14)

// BC   PMU1808_REG_VUSB_RES_SEL_FINE
#define REG_LDO_VUSB_RES_SEL_FINE_SHIFT     13
#define REG_LDO_VUSB_RES_SEL_FINE_MASK      (0x3 << REG_LDO_VUSB_RES_SEL_FINE_SHIFT)
#define REG_LDO_VUSB_RES_SEL_FINE(n)        BITFIELD_VAL(REG_LDO_VUSB_RES_SEL_FINE, n)

// BE   PMU1808_REG_BE
#define PMU1808_PULLDOWN_VUSB               (1 << 7)

// BF   PMU_REG_RTC_CLK_CFG
#define REG_CLK_RTC_EN                      (1 << 5)

// 1809_REG
// REG_10
#define REG_LDO_VMEM2_VBIT_NORMAL_SHIFT     8
#define REG_LDO_VMEM2_VBIT_NORMAL_MASK      (0xFF << REG_LDO_VMEM2_VBIT_NORMAL_SHIFT)
#define REG_LDO_VMEM2_VBIT_NORMAL(n)        BITFIELD_VAL(REG_LDO_VMEM2_VBIT_NORMAL, n)
#define REG_LDO_VMEM2_VBIT_DSLEEP_SHIFT     0
#define REG_LDO_VMEM2_VBIT_DSLEEP_MASK      (0xFF << REG_LDO_VMEM2_VBIT_DSLEEP_SHIFT)
#define REG_LDO_VMEM2_VBIT_DSLEEP(n)        BITFIELD_VAL(REG_LDO_VMEM2_VBIT_DSLEEP, n)

#define LDO_VMEM2_VBIT_NORMAL_SHIFT         REG_LDO_VMEM2_VBIT_NORMAL_SHIFT
#define LDO_VMEM2_VBIT_NORMAL_MASK          REG_LDO_VMEM2_VBIT_NORMAL_MASK
#define LDO_VMEM2_VBIT_NORMAL(n)            REG_LDO_VMEM2_VBIT_NORMAL(n)
#define LDO_VMEM2_VBIT_DSLEEP_SHIFT         REG_LDO_VMEM2_VBIT_DSLEEP_SHIFT
#define LDO_VMEM2_VBIT_DSLEEP_MASK          REG_LDO_VMEM2_VBIT_DSLEEP_MASK
#define LDO_VMEM2_VBIT_DSLEEP(n)            REG_LDO_VMEM2_VBIT_DSLEEP(n)

// REG_15
#define REG_UVLO_SEL_SHIFT                  14
#define REG_UVLO_SEL_MASK                   (0x3 << REG_UVLO_SEL_SHIFT)
#define REG_UVLO_SEL(n)                     BITFIELD_VAL(REG_UVLO_SEL, n)
#define REG_PU_LDO_DIG_DR                   (1 << 13)
#define REG_PU_LDO_DIG_REG                  (1 << 12)
#define REG_BUCK_VCORE_BURST_MODE_SEL_RC    (1 << 8)
#define REG_BUCK_VCORE_BURST_MODE_SEL_DSLEEP (1 << 7)
#define REG_BUCK_VCORE_BURST_MODE_SEL_NORMAL (1 << 6)
#define REG_BUCK_VCORE_VREF_SEL_DSLEEP_SHIFT 4
#define REG_BUCK_VCORE_VREF_SEL_DSLEEP_MASK (0x3 << REG_BUCK_VCORE_VREF_SEL_DSLEEP_SHIFT)
#define REG_BUCK_VCORE_VREF_SEL_DSLEEP(n)   BITFIELD_VAL(REG_BUCK_VCORE_VREF_SEL_DSLEEP, n)
#define REG_BUCK_VCORE_VREF_SEL_NORMAL_SHIFT 2
#define REG_BUCK_VCORE_VREF_SEL_NORMAL_MASK (0x3 << REG_BUCK_VCORE_VREF_SEL_NORMAL_SHIFT)
#define REG_BUCK_VCORE_VREF_SEL_NORMAL(n)   BITFIELD_VAL(REG_BUCK_VCORE_VREF_SEL_NORMAL, n)
#define REG_PU_BUCK_VCORE_DR                (1 << 1)
#define REG_PU_BUCK_VCORE                   (1 << 0)

// REG_43
#define PMU1809_SAR_BYPASS_IREF             (1 << 10)
#define PMU1809_SAR_INPUT_C_SEL             (1 << 9)
#define PMU1809_SAR_VREF_BIT_SHIFT          3
#define PMU1809_SAR_VREF_BIT_MASK           (0x3F << PMU1809_SAR_VREF_BIT_SHIFT)
#define PMU1809_SAR_VREF_BIT(n)             BITFIELD_VAL(PMU1809_SAR_VREF_BIT, n)
#define PMU1809_SAR_INPUT_R_SEL_SHIFT       0
#define PMU1809_SAR_INPUT_R_SEL_MASK        (0x7 << PMU1809_SAR_INPUT_R_SEL_SHIFT)
#define PMU1809_SAR_INPUT_R_SEL(n)          BITFIELD_VAL(PMU1809_SAR_INPUT_R_SEL, n)

// REG_4A
#define REG_CLK_DCDC3_32K_EN                (1 << 14)
#define REG_CLK_DCDC2_32K_EN                (1 << 13)
#define REG_CLK_DCDC1_32K_EN                (1 << 12)
#define REG_BUCK_VANA_BURST_MODE_SEL_RC     (1 << 8)
#define REG_BUCK_VANA_BURST_MODE_SEL_DSLEEP (1 << 7)
#define REG_BUCK_VANA_BURST_MODE_SEL_NORMAL (1 << 6)
#define REG_BUCK_VANA_VREF_SEL_DSLEEP_SHIFT 4
#define REG_BUCK_VANA_VREF_SEL_DSLEEP_MASK  (0x3 << REG_BUCK_VANA_VREF_SEL_DSLEEP_SHIFT)
#define REG_BUCK_VANA_VREF_SEL_DSLEEP(n)    BITFIELD_VAL(REG_BUCK_VANA_VREF_SEL_DSLEEP, n)
#define REG_BUCK_VANA_VREF_SEL_NORMAL_SHIFT 2
#define REG_BUCK_VANA_VREF_SEL_NORMAL_MASK  (0x3 << REG_BUCK_VANA_VREF_SEL_NORMAL_SHIFT)
#define REG_BUCK_VANA_VREF_SEL_NORMAL(n)    BITFIELD_VAL(REG_BUCK_VANA_VREF_SEL_NORMAL, n)
#define REG_PU_BUCK_VANA_DR                 (1 << 1)
#define REG_PU_BUCK_VANA                    (1 << 0)

// REG_4B
#define REG_BUCK_VHPPA_BURST_MODE_SEL_RC    (1 << 8)
#define REG_BUCK_VHPPA_BURST_MODE_SEL_DSLEEP (1 << 7)
#define REG_BUCK_VHPPA_BURST_MODE_SEL_NORMAL (1 << 6)
#define REG_BUCK_VHPPA_VREF_SEL_DSLEEP_SHIFT 4
#define REG_BUCK_VHPPA_VREF_SEL_DSLEEP_MASK (0x3 << REG_BUCK_VHPPA_VREF_SEL_DSLEEP_SHIFT)
#define REG_BUCK_VHPPA_VREF_SEL_DSLEEP(n)   BITFIELD_VAL(REG_BUCK_VHPPA_VREF_SEL_DSLEEP, n)
#define REG_BUCK_VHPPA_VREF_SEL_NORMAL_SHIFT 2
#define REG_BUCK_VHPPA_VREF_SEL_NORMAL_MASK (0x3 << REG_BUCK_VHPPA_VREF_SEL_NORMAL_SHIFT)
#define REG_BUCK_VHPPA_VREF_SEL_NORMAL(n)   BITFIELD_VAL(REG_BUCK_VHPPA_VREF_SEL_NORMAL, n)
#define REG_PU_BUCK_VHPPA_DR                (1 << 1)
#define REG_PU_BUCK_VHPPA                   (1 << 0)

// REG_66
#define PMU1809_REG_PU_LDO_VMEM2_DSLEEP     (1 << 15)
#define PMU1809_REG_PU_LDO_VMEM2_DR         (1 << 14)
#define PMU1809_REG_PU_LDO_VMEM2            (1 << 13)
#define PMU1809_REG_LP_EN_LDO_VMEM2_DSLEEP  (1 << 12)
#define PMU1809_REG_LP_EN_LDO_VMEM2_DR      (1 << 11)
#define PMU1809_REG_LP_EN_LDO_VMEM2         (1 << 10)
#define PMU1809_REG_LDO_VMEM2_PULLDOWN      (1 << 9)
#define PMU1809_REG_LDO_VMEM2_BYPASS        (1 << 8)
#define PMU1809_REG_LDO_VMEM2_VBIT_RC_SHIFT 0
#define PMU1809_REG_LDO_VMEM2_VBIT_RC_MASK  (0xFF << PMU1809_REG_LDO_VMEM2_VBIT_RC_SHIFT)
#define PMU1809_REG_LDO_VMEM2_VBIT_RC(n)    BITFIELD_VAL(PMU1809_REG_LDO_VMEM2_VBIT_RC, n)

#define REG_PU_LDO_VMEM2_DSLEEP             PMU1809_REG_PU_LDO_VMEM2_DSLEEP
#define REG_PU_LDO_VMEM2_DR                 PMU1809_REG_PU_LDO_VMEM2_DR
#define REG_PU_LDO_VMEM2_REG                PMU1809_REG_PU_LDO_VMEM2
#define LP_EN_VMEM2_LDO_DSLEEP              PMU1809_REG_PU_LDO_VMEM2_DSLEEP
#define LP_EN_VMEM2_LDO_DR                  PMU1809_REG_LP_EN_LDO_VMEM2_DR
#define LP_EN_VMEM2_LDO_REG                 PMU1809_REG_LP_EN_LDO_VMEM2

// REG_67
#define REG_PU_LDO_VMEM3_DSLEEP             (1 << 15)
#define REG_PU_LDO_VMEM3_DR                 (1 << 14)
#define REG_PU_LDO_VMEM3                    (1 << 13)
#define REG_LP_EN_LDO_VMEM3_DSLEEP          (1 << 12)
#define REG_LP_EN_LDO_VMEM3_DR              (1 << 11)
#define REG_LP_EN_LDO_VMEM3                 (1 << 10)
#define REG_LDO_VMEM3_BYPASS                (1 << 9)
#define REG_LDO_VMEM3_PULLDOWN              (1 << 8)
#define REG_LDO_VMEM3_VBIT_RC_SHIFT         0
#define REG_LDO_VMEM3_VBIT_RC_MASK          (0xFF << REG_LDO_VMEM3_VBIT_RC_SHIFT)
#define REG_LDO_VMEM3_VBIT_RC(n)            BITFIELD_VAL(REG_LDO_VMEM3_VBIT_RC, n)

#define REG_PU_LDO_VMEM3_REG                REG_PU_LDO_VMEM3
#define LP_EN_VMEM3_LDO_DSLEEP              REG_LP_EN_LDO_VMEM3_DSLEEP
#define LP_EN_VMEM3_LDO_DR                  REG_LP_EN_LDO_VMEM3_DR
#define LP_EN_VMEM3_LDO_REG                 REG_LP_EN_LDO_VMEM3

// REG_68
#define REG_LDO_VMEM3_VBIT_DSLEEP_SHIFT     8
#define REG_LDO_VMEM3_VBIT_DSLEEP_MASK      (0xFF << REG_LDO_VMEM3_VBIT_DSLEEP_SHIFT)
#define REG_LDO_VMEM3_VBIT_DSLEEP(n)        BITFIELD_VAL(REG_LDO_VMEM3_VBIT_DSLEEP, n)
#define REG_LDO_VMEM3_VBIT_NORMAL_SHIFT     0
#define REG_LDO_VMEM3_VBIT_NORMAL_MASK      (0xFF << REG_LDO_VMEM3_VBIT_NORMAL_SHIFT)
#define REG_LDO_VMEM3_VBIT_NORMAL(n)        BITFIELD_VAL(REG_LDO_VMEM3_VBIT_NORMAL, n)

#define LDO_VMEM3_VBIT_NORMAL_SHIFT         REG_LDO_VMEM3_VBIT_NORMAL_SHIFT
#define LDO_VMEM3_VBIT_NORMAL_MASK          REG_LDO_VMEM3_VBIT_NORMAL_MASK
#define LDO_VMEM3_VBIT_NORMAL(n)            REG_LDO_VMEM3_VBIT_NORMAL(n)
#define LDO_VMEM3_VBIT_DSLEEP_SHIFT         REG_LDO_VMEM3_VBIT_DSLEEP_SHIFT
#define LDO_VMEM3_VBIT_DSLEEP_MASK          REG_LDO_VMEM3_VBIT_DSLEEP_MASK
#define LDO_VMEM3_VBIT_DSLEEP(n)            REG_LDO_VMEM3_VBIT_DSLEEP(n)

// REG_74
#define PMU1809_REG_MIC_BIASA_VSEL_SHIFT    9
#define PMU1809_REG_MIC_BIASA_VSEL_MASK     (0x3F << PMU1809_REG_MIC_BIASA_VSEL_SHIFT)
#define PMU1809_REG_MIC_BIASA_VSEL(n)       BITFIELD_VAL(PMU1809_REG_MIC_BIASA_VSEL_MASK, n)
#define REG_BUCK_VCORE_DSLEEP_MSK           (1 << 8)
#define REG_BUCK_VCORE_VBIT_RC_SHIFT        0
#define REG_BUCK_VCORE_VBIT_RC_MASK         (0xFF << REG_BUCK_VCORE_VBIT_RC_SHIFT)
#define REG_BUCK_VCORE_VBIT_RC(n)           BITFIELD_VAL(REG_BUCK_VCORE_VBIT_RC, n)

// REG_75
#define PMU1809_REG_MIC_BIASB_VSEL_SHIFT    9
#define PMU1809_REG_MIC_BIASB_VSEL_MASK     (0x3F << PMU1809_REG_MIC_BIASB_VSEL_SHIFT)
#define PMU1809_REG_MIC_BIASB_VSEL(n)       BITFIELD_VAL(PMU1809_REG_MIC_BIASB_VSEL, n)
#define REG_BUCK_VANA_DSLEEP_MSK            (1 << 8)
#define REG_BUCK_VANA_VBIT_RC_SHIFT         0
#define REG_BUCK_VANA_VBIT_RC_MASK          (0xFF << REG_BUCK_VANA_VBIT_RC_SHIFT)
#define REG_BUCK_VANA_VBIT_RC(n)            BITFIELD_VAL(REG_BUCK_VANA_VBIT_RC, n)

// REG_7B
#define REG_BUCK_VCORE_IS_GAIN_NORMAL_SHIFT 6
#define REG_BUCK_VCORE_IS_GAIN_NORMAL_MASK  (0x7 << REG_BUCK_VCORE_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_VCORE_IS_GAIN_NORMAL(n)    BITFIELD_VAL(REG_BUCK_VCORE_IS_GAIN_NORMAL, n)
#define REG_BUCK_VCORE_IS_GAIN_DSLEEP_SHIFT 3
#define REG_BUCK_VCORE_IS_GAIN_DSLEEP_MASK  (0x7 << REG_BUCK_VCORE_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_VCORE_IS_GAIN_DSLEEP(n)    BITFIELD_VAL(REG_BUCK_VCORE_IS_GAIN_DSLEEP, n)
#define REG_BUCK_VCORE_IS_GAIN_RC_SHIFT     0
#define REG_BUCK_VCORE_IS_GAIN_RC_MASK      (0x7 << REG_BUCK_VCORE_IS_GAIN_RC_SHIFT)
#define REG_BUCK_VCORE_IS_GAIN_RC(n)        BITFIELD_VAL(REG_BUCK_VCORE_IS_GAIN_RC, n)

// REG_7C
#define REG_BUCK_VANA_IS_GAIN_NORMAL_SHIFT  6
#define REG_BUCK_VANA_IS_GAIN_NORMAL_MASK   (0x7 << REG_BUCK_VANA_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_VANA_IS_GAIN_NORMAL(n)     BITFIELD_VAL(REG_BUCK_VANA_IS_GAIN_NORMAL, n)
#define REG_BUCK_VANA_IS_GAIN_DSLEEP_SHIFT  3
#define REG_BUCK_VANA_IS_GAIN_DSLEEP_MASK   (0x7 << REG_BUCK_VANA_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_VANA_IS_GAIN_DSLEEP(n)     BITFIELD_VAL(REG_BUCK_VANA_IS_GAIN_DSLEEP, n)
#define REG_BUCK_VANA_IS_GAIN_RC_SHIFT      0
#define REG_BUCK_VANA_IS_GAIN_RC_MASK       (0x7 << REG_BUCK_VANA_IS_GAIN_RC_SHIFT)
#define REG_BUCK_VANA_IS_GAIN_RC(n)         BITFIELD_VAL(REG_BUCK_VANA_IS_GAIN_RC, n)

// REG_7D
#define REG_BUCK_VHPPA_IS_GAIN_NORMAL_SHIFT 6
#define REG_BUCK_VHPPA_IS_GAIN_NORMAL_MASK  (0x7 << REG_BUCK_VHPPA_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_VHPPA_IS_GAIN_NORMAL(n)    BITFIELD_VAL(REG_BUCK_VHPPA_IS_GAIN_NORMAL, n)
#define REG_BUCK_VHPPA_IS_GAIN_DSLEEP_SHIFT 3
#define REG_BUCK_VHPPA_IS_GAIN_DSLEEP_MASK  (0x7 << REG_BUCK_VHPPA_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_VHPPA_IS_GAIN_DSLEEP(n)    BITFIELD_VAL(REG_BUCK_VHPPA_IS_GAIN_DSLEEP, n)
#define REG_BUCK_VHPPA_IS_GAIN_RC_SHIFT     0
#define REG_BUCK_VHPPA_IS_GAIN_RC_MASK      (0x7 << REG_BUCK_VHPPA_IS_GAIN_RC_SHIFT)
#define REG_BUCK_VHPPA_IS_GAIN_RC(n)        BITFIELD_VAL(REG_BUCK_VHPPA_IS_GAIN_RC, n)

// REG_95
#define PMU1809_REG_RESERVED_ANA_15_0_SHIFT    0
#define PMU1809_REG_RESERVED_ANA_15_0_MASK     (0xFFFF << PMU1809_REG_RESERVED_ANA_15_0_SHIFT)
#define PMU1809_REG_RESERVED_ANA_15_0(n)       BITFIELD_VAL(PMU1809_REG_RESERVED_ANA_15_0, n)

// REG_11F
#define REG_LP_BIAS_SEL_LDO_NORMAL_SHIFT    11
#define REG_LP_BIAS_SEL_LDO_NORMAL_MASK     (0x1F << REG_LP_BIAS_SEL_LDO_NORMAL_SHIFT)
#define REG_LP_BIAS_SEL_LDO_NORMAL(n)       BITFIELD_VAL(REG_LP_BIAS_SEL_LDO_NORMAL, n)
#define REG_LP_BIAS_SEL_LDO_RC_SHIFT        6
#define REG_LP_BIAS_SEL_LDO_RC_MASK         (0x1F << REG_LP_BIAS_SEL_LDO_RC_SHIFT)
#define REG_LP_BIAS_SEL_LDO_RC(n)           BITFIELD_VAL(REG_LP_BIAS_SEL_LDO_RC, n)
#define REG_LP_BIAS_SEL_LDO_DSLEEP_SHIFT    1
#define REG_LP_BIAS_SEL_LDO_DSLEEP_MASK     (0x1F << REG_LP_BIAS_SEL_LDO_DSLEEP_SHIFT)
#define REG_LP_BIAS_SEL_LDO_DSLEEP(n)       BITFIELD_VAL(REG_LP_BIAS_SEL_LDO_DSLEEP, n)
#define REG_LP_BIAS_SEL_RAMP_EN             (1 << 0)

// REG_129
#define BUCK_VHPPA_BURST_VREF_SEL           (1 << 8)
#define BUCK_VHPPA_DIRECT_PFM_MODE          (1 << 7)
#define BUCK_VHPPA_DIRECT_ULP_MODE          (1 << 6)
#define BUCK_VHPPA_ICOMP_DC_HIGH            (1 << 5)
#define BUCK_VHPPA_IRCOMP_IX2               (1 << 4)
#define BUCK_VHPPA_PFM_MODE                 (1 << 3)
#define BUCK_VHPPA_PWR_OK_SEL               (1 << 2)
#define BUCK_VHPPA_REFRESH_MODE             (1 << 1)
#define BUCK_VHPPA_ANTI_RES_DISABLE         (1 << 0)

// REG_12A
#define BUCK_VHPPA_BURST_THRESHOLD_SHIFT    10
#define BUCK_VHPPA_BURST_THRESHOLD_MASK     (0x1F << BUCK_VHPPA_BURST_THRESHOLD_SHIFT)
#define BUCK_VHPPA_BURST_THRESHOLD(n)       BITFIELD_VAL(BUCK_VHPPA_BURST_THRESHOLD, n)
#define BUCK_VHPPA_CAL_DETALV_SHIFT         4
#define BUCK_VHPPA_CAL_DETALV_MASK          (0x3F << BUCK_VHPPA_CAL_DETALV_SHIFT)
#define BUCK_VHPPA_CAL_DETALV(n)            BITFIELD_VAL(BUCK_VHPPA_CAL_DETALV, n)
#define BUCK_VHPPA_CAP_BIT_SHIFT            0
#define BUCK_VHPPA_CAP_BIT_MASK             (0xF << BUCK_VHPPA_CAP_BIT_SHIFT)
#define BUCK_VHPPA_CAP_BIT(n)               BITFIELD_VAL(BUCK_VHPPA_CAP_BIT, n)

// REG_12D
#define BUCK_VHPPA_TEST_MODE_EN             (1 << 12)
#define BUCK_VHPPA_TEST_PMOS                (1 << 11)
#define BUCK_VHPPA_TEST_POWERMOS            (1 << 10)
#define BUCK_VHPPA_ULP_MODE                 (1 << 9)
#define BUCK_VHPPA_VREF_SEL_SHIFT           6
#define BUCK_VHPPA_VREF_SEL_MASK            (0x3 << BUCK_VHPPA_VREF_SEL_SHIFT)
#define BUCK_VHPPA_VREF_SEL(n)              BITFIELD_VAL(BUCK_VHPPA_VREF_SEL, n)
#define BUCK_VHPPA_ZCD_CAP_BIT_SHIFT        1
#define BUCK_VHPPA_ZCD_CAP_BIT_MASK         (0xF << BUCK_VHPPA_ZCD_CAP_BIT_SHIFT)
#define BUCK_VHPPA_ZCD_CAP_BIT(n)           BITFIELD_VAL(BUCK_VHPPA_ZCD_CAP_BIT, n)
#define BUCK_VHPPA_DELTAV_X4                (1 << 0)

// REG_130
#define BUCK_VANA_BURST_VREF_SEL            (1 << 8)
#define BUCK_VANA_DIRECT_PFM_MODE           (1 << 7)
#define BUCK_VANA_DIRECT_ULP_MODE           (1 << 6)
#define BUCK_VANA_ICOMP_DC_HIGH             (1 << 5)
#define BUCK_VANA_IRCOMP_IX2                (1 << 4)
#define BUCK_VANA_PFM_MODE                  (1 << 3)
#define BUCK_VANA_PWR_OK_SEL                (1 << 2)
#define BUCK_VANA_REFRESH_MODE              (1 << 1)
#define BUCK_VANA_ANTI_RES_DISABLE          (1 << 0)

// REG_131
#define BUCK_VANA_BURST_THRESHOLD_SHIFT     10
#define BUCK_VANA_BURST_THRESHOLD_MASK      (0x1F << BUCK_VANA_BURST_THRESHOLD_SHIFT)
#define BUCK_VANA_BURST_THRESHOLD(n)        BITFIELD_VAL(BUCK_VANA_BURST_THRESHOLD, n)
#define BUCK_VANA_CAL_DETALV_SHIFT          4
#define BUCK_VANA_CAL_DETALV_MASK           (0x3F << BUCK_VANA_CAL_DETALV_SHIFT)
#define BUCK_VANA_CAL_DETALV(n)             BITFIELD_VAL(BUCK_VANA_CAL_DETALV, n)
#define BUCK_VANA_CAP_BIT_SHIFT             0
#define BUCK_VANA_CAP_BIT_MASK              (0xF << BUCK_VANA_CAP_BIT_SHIFT)
#define BUCK_VANA_CAP_BIT(n)                BITFIELD_VAL(BUCK_VANA_CAP_BIT, n)

// REG_136
#define BUCK_VCORE_BURST_VREF_SEL           (1 << 8)
#define BUCK_VCORE_DIRECT_PFM_MODE          (1 << 7)
#define BUCK_VCORE_DIRECT_ULP_MODE          (1 << 6)
#define BUCK_VCORE_ICOMP_DC_HIGH            (1 << 5)
#define BUCK_VCORE_IRCOMP_IX2               (1 << 4)
#define BUCK_VCORE_PFM_MODE                 (1 << 3)
#define BUCK_VCORE_PWR_OK_SEL               (1 << 2)
#define BUCK_VCORE_REFRESH_MODE             (1 << 1)
#define BUCK_VCORE_ANTI_RES_DISABLE         (1 << 0)

// REG_134
#define BUCK_VANA_TEST_MODE_EN              (1 << 12)
#define BUCK_VANA_TEST_PMOS                 (1 << 11)
#define BUCK_VANA_TEST_POWERMOS             (1 << 10)
#define BUCK_VANA_ULP_MODE                  (1 << 9)
#define BUCK_VANA_VREF_SEL_SHIFT            6
#define BUCK_VANA_VREF_SEL_MASK             (0x3 << BUCK_VANA_VREF_SEL_SHIFT)
#define BUCK_VANA_VREF_SEL(n)               BITFIELD_VAL(BUCK_VANA_VREF_SEL, n)
#define BUCK_VANA_ZCD_CAP_BIT_SHIFT         1
#define BUCK_VANA_ZCD_CAP_BIT_MASK          (0xF << BUCK_VANA_ZCD_CAP_BIT_SHIFT)
#define BUCK_VANA_ZCD_CAP_BIT(n)            BITFIELD_VAL(BUCK_VANA_ZCD_CAP_BIT, n)
#define BUCK_VANA_DELTAV_X4                 (1 << 0)

// REG_135
#define BUCK_VANA_REFRESH_CLK_SEL_SHIFT     13
#define BUCK_VANA_REFRESH_CLK_SEL_MASK      (0x3 << BUCK_VANA_REFRESH_CLK_SEL_SHIFT)
#define BUCK_VANA_REFRESH_CLK_SEL(n)        BITFIELD_VAL(BUCK_VANA_REFRESH_CLK_SEL, n)
#define BUCK_VANA_SLOPE_DOUBLE              (1 << 12)
#define BUCK_VANA_OFFSET_CAL_EN             (1 << 11)
#define BUCK_VANA_RESET_SLEEP_COUNTER       (1 << 10)
#define BUCK_VANA_RESET_ZCD                 (1 << 9)
#define BUCK_VANA_OC_DET_EN                 (1 << 8)
#define BUCK_VANA_RESERVED_SHIFT            0
#define BUCK_VANA_RESERVED_MASK             (0xFF << BUCK_VANA_RESERVED_SHIFT)
#define BUCK_VANA_RESERVED(n)               BITFIELD_VAL(BUCK_VANA_RESERVED, n)

// REG_137
#define BUCK_VCORE_BURST_THRESHOLD_SHIFT    10
#define BUCK_VCORE_BURST_THRESHOLD_MASK     (0x1F << BUCK_VCORE_BURST_THRESHOLD_SHIFT)
#define BUCK_VCORE_BURST_THRESHOLD(n)       BITFIELD_VAL(BUCK_VCORE_BURST_THRESHOLD, n)
#define BUCK_VCORE_CAL_DETALV_SHIFT         4
#define BUCK_VCORE_CAL_DETALV_MASK          (0x3F << BUCK_VCORE_CAL_DETALV_SHIFT)
#define BUCK_VCORE_CAL_DETALV(n)            BITFIELD_VAL(BUCK_VCORE_CAL_DETALV, n)
#define BUCK_VCORE_CAP_BIT_SHIFT            0
#define BUCK_VCORE_CAP_BIT_MASK             (0xF << BUCK_VCORE_CAP_BIT_SHIFT)
#define BUCK_VCORE_CAP_BIT(n)               BITFIELD_VAL(BUCK_VCORE_CAP_BIT, n)

// REG_13A
#define BUCK_VCORE_TEST_MODE_EN             (1 << 12)
#define BUCK_VCORE_TEST_PMOS                (1 << 11)
#define BUCK_VCORE_TEST_POWERMOS            (1 << 10)
#define BUCK_VCORE_ULP_MODE                 (1 << 9)
#define BUCK_VCORE_VREF_SEL_SHIFT           6
#define BUCK_VCORE_VREF_SEL_MASK            (0x3 << BUCK_VCORE_VREF_SEL_SHIFT)
#define BUCK_VCORE_VREF_SEL(n)              BITFIELD_VAL(BUCK_VCORE_VREF_SEL, n)
#define BUCK_VCORE_ZCD_CAP_BIT_SHIFT        1
#define BUCK_VCORE_ZCD_CAP_BIT_MASK         (0xF << BUCK_VCORE_ZCD_CAP_BIT_SHIFT)
#define BUCK_VCORE_ZCD_CAP_BIT(n)           BITFIELD_VAL(BUCK_VCORE_ZCD_CAP_BIT, n)
#define BUCK_VCORE_DELTAV_X4                (1 << 0)

// REG_13B
#define BUCK_VCORE_REFRESH_CLK_SEL_SHIFT    13
#define BUCK_VCORE_REFRESH_CLK_SEL_MASK     (0x3 << BUCK_VCORE_REFRESH_CLK_SEL_SHIFT)
#define BUCK_VCORE_REFRESH_CLK_SEL(n)       BITFIELD_VAL(BUCK_VCORE_REFRESH_CLK_SEL, n)
#define BUCK_VCORE_SLOPE_DOUBLE             (1 << 12)
#define BUCK_VCORE_OFFSET_CAL_EN            (1 << 11)
#define BUCK_VCORE_RESET_SLEEP_COUNTER      (1 << 10)
#define BUCK_VCORE_RESET_ZCD                (1 << 9)
#define BUCK_VCORE_OC_DET_EN                (1 << 8)
#define BUCK_VCORE_RESERVED_SHIFT           0
#define BUCK_VCORE_RESERVED_MASK            (0xFF << BUCK_VCORE_RESERVED_SHIFT)
#define BUCK_VCORE_RESERVED(n)              BITFIELD_VAL(BUCK_VCORE_RESERVED, n)

// REG_178
#define REG_CLK_RTC_SEL_XO                  (1 << 1)
#define RTC_TIME_LOAD                       (1 << 0)

// REG_197
#define REG_NOT_RESET_SHIFT                 8
#define REG_NOT_RESET_MASK                  (0xFF << REG_NOT_RESET_SHIFT)
#define REG_NOT_RESET(n)                    BITFIELD_VAL(REG_NOT_RESET, n)
#define REG_CLK_RAMP_BIAS_DIV_SHIFT         0
#define REG_CLK_RAMP_BIAS_DIV_MASK          (0x1F << REG_CLK_RAMP_BIAS_DIV_SHIFT)
#define REG_CLK_RAMP_BIAS_DIV(n)            BITFIELD_VAL(REG_CLK_RAMP_BIAS_DIV, n)

// REG_1BE
#define ABORT_SOURCE_CLR                    (1 << 7)
#define PMU_LDO_ON_SOURCE_CLR               (1 << 6)
#define ABORT_SOURCE_SHIFT                  3
#define ABORT_SOURCE_MASK                   (0x7 << ABORT_SOURCE_SHIFT)
#define ABORT_SOURCE(n)                     BITFIELD_VAL(ABORT_SOURCE, n)
#define PMU_LDO_ON_SOURCE_SHIFT             0
#define PMU_LDO_ON_SOURCE_MASK              (0x7 << PMU_LDO_ON_SOURCE_SHIFT)
#define PMU_LDO_ON_SOURCE(n)                BITFIELD_VAL(PMU_LDO_ON_SOURCE, n)

#define PMU1809_RTC_LOADED                  (1 << 8)

// power protect module
// REG_2C
#define POWER_ABORT_INTR_MSK                (1 << 9)
#define CAP_SENSOR_INTR_MSK                 (1 << 8)
#define DIG_SENSOR_INTR_MSK                 (1 << 7)
#define DBG_COARSE_TUN_CODE_SHIFT           0
#define DBG_COARSE_TUN_CODE_MASK            (0x7F << DBG_COARSE_TUN_CODE_SHIFT)
#define DBG_COARSE_TUN_CODE(n)              BITFIELD_VAL(DBG_COARSE_TUN_CODE, n)

// REG_2D
#define POWER_ABORT_INTR_PMU                (1 << 4)
#define GPADC_INTR                          (1 << 3)
#define CAP_SENSOR_INTR_PMU                 (1 << 2)
#define DIG_SENSOR_INTR_PMU                 (1 << 1)
#define PMIC_INT_PMU                        (1 << 0)

// REG_96
#define RESERVED_ANA_17_16_SHIFT            3
#define RESERVED_ANA_17_16_MASK             (0x3 << RESERVED_ANA_17_16_SHIFT)
#define RESERVED_ANA_17_16(n)               BITFIELD_VAL(RESERVED_ANA_17_16, n)
#define ABORT_INTR_18_16_SHIFT              0
#define ABORT_INTR_18_16_MASK               (0x7 << ABORT_INTR_18_16_SHIFT)
#define ABORT_INTR_18_16(n)                 BITFIELD_VAL(ABORT_INTR_18_16, n)

// REG_97
#define ABORT_INTR_15_0_SHIFT               0
#define ABORT_INTR_15_0_MASK                (0xFFFF << ABORT_INTR_15_0_SHIFT)
#define ABORT_INTR_15_0(n)                  BITFIELD_VAL(ABORT_INTR_15_0, n)

// REG_98
#define BUCK_VANA_ABORT_SOURCE_SHIFT        12
#define BUCK_VANA_ABORT_SOURCE_MASK         (0xF << BUCK_VANA_ABORT_SOURCE_SHIFT)
#define BUCK_VANA_ABORT_SOURCE(n)           BITFIELD_VAL(BUCK_VANA_ABORT_SOURCE, n)
#define BUCK_VANA_REBOOT_CNT_SHIFT          8
#define BUCK_VANA_REBOOT_CNT_MASK           (0xF << BUCK_VANA_REBOOT_CNT_SHIFT)
#define BUCK_VANA_REBOOT_CNT(n)             BITFIELD_VAL(BUCK_VANA_REBOOT_CNT, n)
#define BUCK_VCORE_ABORT_SOURCE_SHIFT       4
#define BUCK_VCORE_ABORT_SOURCE_MASK        (0xF << BUCK_VCORE_ABORT_SOURCE_SHIFT)
#define BUCK_VCORE_ABORT_SOURCE(n)          BITFIELD_VAL(BUCK_VCORE_ABORT_SOURCE, n)
#define BUCK_VCORE_REBOOT_CNT_SHIFT         0
#define BUCK_VCORE_REBOOT_CNT_MASK          (0xF << BUCK_VCORE_REBOOT_CNT_SHIFT)
#define BUCK_VCORE_REBOOT_CNT(n)            BITFIELD_VAL(BUCK_VCORE_REBOOT_CNT, n)

#define REG_BUCK_VANA_ABORT_SOURCE_SHIFT    BUCK_VANA_ABORT_SOURCE_SHIFT
#define REG_BUCK_VANA_ABORT_SOURCE_MASK     BUCK_VANA_ABORT_SOURCE_MASK
#define REG_BUCK_VCORE_ABORT_SOURCE_SHIFT   BUCK_VCORE_ABORT_SOURCE_SHIFT
#define REG_BUCK_VCORE_ABORT_SOURCE_MASK    BUCK_VCORE_ABORT_SOURCE_MASK

// REG_99
#define VBAT2VCORE_ABORT_SOURCE_SHIFT       12
#define VBAT2VCORE_ABORT_SOURCE_MASK        (0xF << VBAT2VCORE_ABORT_SOURCE_SHIFT)
#define VBAT2VCORE_ABORT_SOURCE(n)          BITFIELD_VAL(VBAT2VCORE_ABORT_SOURCE, n)
#define VBAT2VCORE_REBOOT_CNT_SHIFT         8
#define VBAT2VCORE_REBOOT_CNT_MASK          (0xF << VBAT2VCORE_REBOOT_CNT_SHIFT)
#define VBAT2VCORE_REBOOT_CNT(n)            BITFIELD_VAL(VBAT2VCORE_REBOOT_CNT, n)
#define BUCK_VHPPA_ABORT_SOURCE_SHIFT       4
#define BUCK_VHPPA_ABORT_SOURCE_MASK        (0xF << BUCK_VHPPA_ABORT_SOURCE_SHIFT)
#define BUCK_VHPPA_ABORT_SOURCE(n)          BITFIELD_VAL(BUCK_VHPPA_ABORT_SOURCE, n)
#define BUCK_VHPPA_REBOOT_CNT_SHIFT         0
#define BUCK_VHPPA_REBOOT_CNT_MASK          (0xF << BUCK_VHPPA_REBOOT_CNT_SHIFT)
#define BUCK_VHPPA_REBOOT_CNT(n)            BITFIELD_VAL(BUCK_VHPPA_REBOOT_CNT, n)

#define REG_VBAT2VCORE_ABORT_SOURCE_SHIFT   VBAT2VCORE_ABORT_SOURCE_SHIFT
#define REG_VBAT2VCORE_ABORT_SOURCE_MASK    VBAT2VCORE_ABORT_SOURCE_MASK
#define REG_BUCK_VHPPA_ABORT_SOURCE_SHIFT   BUCK_VHPPA_ABORT_SOURCE_SHIFT
#define REG_BUCK_VHPPA_ABORT_SOURCE_MASK    BUCK_VHPPA_ABORT_SOURCE_MASK

// REG_9A
#define VCORE_L1_ABORT_SOURCE_SHIFT         12
#define VCORE_L1_ABORT_SOURCE_MASK          (0xF << VCORE_L1_ABORT_SOURCE_SHIFT)
#define VCORE_L1_ABORT_SOURCE(n)            BITFIELD_VAL(VCORE_L1_ABORT_SOURCE, n)
#define VCORE_L1_REBOOT_CNT_SHIFT           8
#define VCORE_L1_REBOOT_CNT_MASK            (0xF << VCORE_L1_REBOOT_CNT_SHIFT)
#define VCORE_L1_REBOOT_CNT(n)              BITFIELD_VAL(VCORE_L1_REBOOT_CNT, n)
#define VANA_ABORT_SOURCE_SHIFT             4
#define VANA_ABORT_SOURCE_MASK              (0xF << VANA_ABORT_SOURCE_SHIFT)
#define VANA_ABORT_SOURCE(n)                BITFIELD_VAL(VANA_ABORT_SOURCE, n)
#define VANA_REBOOT_CNT_SHIFT               0
#define VANA_REBOOT_CNT_MASK                (0xF << VANA_REBOOT_CNT_SHIFT)
#define VANA_REBOOT_CNT(n)                  BITFIELD_VAL(VANA_REBOOT_CNT, n)

#define REG_VCORE_L1_ABORT_SOURCE_SHIFT     VCORE_L1_ABORT_SOURCE_SHIFT
#define REG_VCORE_L1_ABORT_SOURCE_MASK      VCORE_L1_ABORT_SOURCE_MASK
#define REG_VANA_ABORT_SOURCE_SHIFT         VANA_ABORT_SOURCE_SHIFT
#define REG_VANA_ABORT_SOURCE_MASK          VANA_ABORT_SOURCE_MASK

// REG_9B
#define VIO_ABORT_SOURCE_SHIFT              12
#define VIO_ABORT_SOURCE_MASK               (0xF << VIO_ABORT_SOURCE_SHIFT)
#define VIO_ABORT_SOURCE(n)                 BITFIELD_VAL(VIO_ABORT_SOURCE, n)
#define VIO_REBOOT_CNT_SHIFT                8
#define VIO_REBOOT_CNT_MASK                 (0xF << VIO_REBOOT_CNT_SHIFT)
#define VIO_REBOOT_CNT(n)                   BITFIELD_VAL(VIO_REBOOT_CNT, n)
#define VMEM_ABORT_SOURCE_SHIFT             4
#define VMEM_ABORT_SOURCE_MASK              (0xF << VMEM_ABORT_SOURCE_SHIFT)
#define VMEM_ABORT_SOURCE(n)                BITFIELD_VAL(VMEM_ABORT_SOURCE, n)
#define VMEM_REBOOT_CNT_SHIFT               0
#define VMEM_REBOOT_CNT_MASK                (0xF << VMEM_REBOOT_CNT_SHIFT)
#define VMEM_REBOOT_CNT(n)                  BITFIELD_VAL(VMEM_REBOOT_CNT, n)

#define REG_VIO_ABORT_SOURCE_SHIFT          VIO_ABORT_SOURCE_SHIFT
#define REG_VIO_ABORT_SOURCE_MASK           VIO_ABORT_SOURCE_MASK
#define REG_VMEM_ABORT_SOURCE_SHIFT         VMEM_ABORT_SOURCE_SHIFT
#define REG_VMEM_ABORT_SOURCE_MASK          VMEM_ABORT_SOURCE_MASK

// REG_9C
#define VMEM2_ABORT_SOURCE_SHIFT            12
#define VMEM2_ABORT_SOURCE_MASK             (0xF << VMEM2_ABORT_SOURCE_SHIFT)
#define VMEM2_ABORT_SOURCE(n)               BITFIELD_VAL(VMEM2_ABORT_SOURCE, n)
#define VMEM2_REBOOT_CNT_SHIFT              8
#define VMEM2_REBOOT_CNT_MASK               (0xF << VMEM2_REBOOT_CNT_SHIFT)
#define VMEM2_REBOOT_CNT(n)                 BITFIELD_VAL(VMEM2_REBOOT_CNT, n)
#define VUSB_ABORT_SOURCE_SHIFT             4
#define VUSB_ABORT_SOURCE_MASK              (0xF << VUSB_ABORT_SOURCE_SHIFT)
#define VUSB_ABORT_SOURCE(n)                BITFIELD_VAL(VUSB_ABORT_SOURCE, n)
#define VUSB_REBOOT_CNT_SHIFT               0
#define VUSB_REBOOT_CNT_MASK                (0xF << VUSB_REBOOT_CNT_SHIFT)
#define VUSB_REBOOT_CNT(n)                  BITFIELD_VAL(VUSB_REBOOT_CNT, n)

#define REG_VMEM2_ABORT_SOURCE_SHIFT         VMEM2_ABORT_SOURCE_SHIFT
#define REG_VMEM2_ABORT_SOURCE_MASK          VMEM2_ABORT_SOURCE_MASK
#define REG_VUSB_ABORT_SOURCE_SHIFT          VUSB_ABORT_SOURCE_SHIFT
#define REG_VUSB_ABORT_SOURCE_MASK           VUSB_ABORT_SOURCE_MASK

// REG_9D
#define VMEM3_ABORT_SOURCE_SHIFT            12
#define VMEM3_ABORT_SOURCE_MASK             (0xF << VMEM3_ABORT_SOURCE_SHIFT)
#define VMEM3_ABORT_SOURCE(n)               BITFIELD_VAL(VMEM3_ABORT_SOURCE, n)
#define VMEM3_REBOOT_CNT_SHIFT              8
#define VMEM3_REBOOT_CNT_MASK               (0xF << VMEM3_REBOOT_CNT_SHIFT)
#define VMEM3_REBOOT_CNT(n)                 BITFIELD_VAL(VMEM3_REBOOT_CNT, n)
#define VSENSOR_ABORT_SOURCE_SHIFT          4
#define VSENSOR_ABORT_SOURCE_MASK           (0xF << VSENSOR_ABORT_SOURCE_SHIFT)
#define VSENSOR_ABORT_SOURCE(n)             BITFIELD_VAL(VSENSOR_ABORT_SOURCE, n)
#define VSENSOR_REBOOT_CNT_SHIFT            0
#define VSENSOR_REBOOT_CNT_MASK             (0xF << VSENSOR_REBOOT_CNT_SHIFT)
#define VSENSOR_REBOOT_CNT(n)               BITFIELD_VAL(VSENSOR_REBOOT_CNT, n)

#define REG_VMEM3_ABORT_SOURCE_SHIFT        VMEM3_ABORT_SOURCE_SHIFT
#define REG_VMEM3_ABORT_SOURCE_MASK         VMEM3_ABORT_SOURCE_MASK
#define REG_VSENSOR_ABORT_SOURCE_SHIFT      VSENSOR_ABORT_SOURCE_SHIFT
#define REG_VSENSOR_ABORT_SOURCE_MASK       VSENSOR_ABORT_SOURCE_MASK

// REG_9E
#define VBAT_ABORT_SOURCE_SHIFT             12
#define VBAT_ABORT_SOURCE_MASK              (0xF << VBAT_ABORT_SOURCE_SHIFT)
#define VBAT_ABORT_SOURCE(n)                BITFIELD_VAL(VBAT_ABORT_SOURCE, n)
#define VBAT_REBOOT_CNT_SHIFT               8
#define VBAT_REBOOT_CNT_MASK                (0xF << VBAT_REBOOT_CNT_SHIFT)
#define VBAT_REBOOT_CNT(n)                  BITFIELD_VAL(VBAT_REBOOT_CNT, n)
#define VRTC_ABORT_SOURCE_SHIFT             4
#define VRTC_ABORT_SOURCE_MASK              (0xF << VRTC_ABORT_SOURCE_SHIFT)
#define VRTC_ABORT_SOURCE(n)                BITFIELD_VAL(VRTC_ABORT_SOURCE, n)
#define VRTC_REBOOT_CNT_SHIFT               0
#define VRTC_REBOOT_CNT_MASK                (0xF << VRTC_REBOOT_CNT_SHIFT)
#define VRTC_REBOOT_CNT(n)                  BITFIELD_VAL(VRTC_REBOOT_CNT, n)

#define REG_VBAT_ABORT_SOURCE_SHIFT         VBAT_ABORT_SOURCE_SHIFT
#define REG_VBAT_ABORT_SOURCE_MASK          VBAT_ABORT_SOURCE_MASK

// REG_A0
#define VMICA_ABORT_SOURCE_SHIFT            12
#define VMICA_ABORT_SOURCE_MASK             (0xF << VMICA_ABORT_SOURCE_SHIFT)
#define VMICA_ABORT_SOURCE(n)               BITFIELD_VAL(VMICA_ABORT_SOURCE, n)
#define VMICA_REBOOT_CNT_SHIFT              8
#define VMICA_REBOOT_CNT_MASK               (0xF << VMICA_REBOOT_CNT_SHIFT)
#define VMICA_REBOOT_CNT(n)                 BITFIELD_VAL(VMICA_REBOOT_CNT, n)
#define VMICB_ABORT_SOURCE_SHIFT            4
#define VMICB_ABORT_SOURCE_MASK             (0xF << VMICB_ABORT_SOURCE_SHIFT)
#define VMICB_ABORT_SOURCE(n)               BITFIELD_VAL(VMICB_ABORT_SOURCE, n)
#define VMICB_REBOOT_CNT_SHIFT              0
#define VMICB_REBOOT_CNT_MASK               (0xF << VMICB_REBOOT_CNT_SHIFT)
#define VMICB_REBOOT_CNT(n)                 BITFIELD_VAL(VMICB_REBOOT_CNT, n)

#define REG_VMICA_ABORT_SOURCE_SHIFT        VMICA_ABORT_SOURCE_SHIFT
#define REG_VMICA_ABORT_SOURCE_MASK         VMICA_ABORT_SOURCE_MASK
#define REG_VMICB_ABORT_SOURCE_SHIFT        VMICB_ABORT_SOURCE_SHIFT
#define REG_VMICB_ABORT_SOURCE_MASK         VMICB_ABORT_SOURCE_MASK

// REG_A1
#define OTP_ABORT_SOURCE_SHIFT              4
#define OTP_ABORT_SOURCE_MASK               (0xF << OTP_ABORT_SOURCE_SHIFT)
#define OTP_ABORT_SOURCE(n)                 BITFIELD_VAL(OTP_ABORT_SOURCE, n)
#define OTP_REBOOT_CNT_SHIFT                0
#define OTP_REBOOT_CNT_MASK                 (0xF << OTP_REBOOT_CNT_SHIFT)
#define OTP_REBOOT_CNT(n)                   BITFIELD_VAL(OTP_REBOOT_CNT, n)

#define REG_OTP_ABORT_SOURCE_SHIFT          OTP_ABORT_SOURCE_SHIFT
#define REG_OTP_ABORT_SOURCE_MASK           OTP_ABORT_SOURCE_MASK

// REG_141
#define REG_VBAT2VCORE_OSCP_FUNC_EN         (1 << 2)
#define REG_VBAT2VCORE_OCP_EN_DR            (1 << 1)
#define REG_VBAT2VCORE_OCP_EN_REG           (1 << 0)

// REG_142
#define REG_VBAT2VCORE_BREAK_TIME_SHIFT     14
#define REG_VBAT2VCORE_BREAK_TIME_MASK      (0x3 << REG_VBAT2VCORE_BREAK_TIME_SHIFT)
#define REG_VBAT2VCORE_BREAK_TIME(n)        BITFIELD_VAL(REG_VBAT2VCORE_BREAK_TIME, n)
#define REG_VBAT2VCORE_REBOOT_CYCLE_SHIFT   10
#define REG_VBAT2VCORE_REBOOT_CYCLE_MASK    (0xF << REG_VBAT2VCORE_REBOOT_CYCLE_SHIFT)
#define REG_VBAT2VCORE_REBOOT_CYCLE(n)      BITFIELD_VAL(REG_VBAT2VCORE_REBOOT_CYCLE, n)
#define REG_VBAT2VCORE_REBOOT_TRIGGER       (1 << 9)
#define REG_VBAT2VCORE_REBOOT_TRIGGER_DR    (1 << 8)
#define REG_VBAT2VCORE_LDO_OFF_DR           (1 << 7)
#define REG_VBAT2VCORE_LDO_OFF_REG          (1 << 6)
#define REG_VBAT2VCORE_PMU_OFF_DR           (1 << 5)
#define REG_VBAT2VCORE_PMU_OFF_REG          (1 << 4)
#define REG_VBAT2VCORE_PROTEC_CYCLE_SHIFT   2
#define REG_VBAT2VCORE_PROTEC_CYCLE_MASK    (0x3 << REG_VBAT2VCORE_PROTEC_CYCLE_SHIFT)
#define REG_VBAT2VCORE_PROTEC_CYCLE(n)      BITFIELD_VAL(REG_VBAT2VCORE_PROTEC_CYCLE, n)
#define REG_VBAT2VCORE_HANDLE_SEL_PMU_OFF   (1 << 1)
#define REG_VBAT2VCORE_HW_MODE              (1 << 0)

// REG_143
#define REG_VBAT2VCORE_INTR_EN              (1 << 2)
#define REG_VBAT2VCORE_INTR_MSK             (1 << 1)
#define REG_VBAT2VCORE_INTR_CLR             (1 << 0)

// REG_144
#define REG_VCORE_L1_DET_TIME_SHIFT         10
#define REG_VCORE_L1_DET_TIME_MASK          (0x1F << REG_VCORE_L1_DET_TIME_SHIFT)
#define REG_VCORE_L1_DET_TIME(n)            BITFIELD_VAL(REG_VCORE_L1_DET_TIME, n)
#define REG_VCORE_L1_DET_TRUBO_TIME_SHIFT   5
#define REG_VCORE_L1_DET_TRUBO_TIME_MASK    (0x1F << REG_VCORE_L1_DET_TRUBO_TIME_SHIFT)
#define REG_VCORE_L1_DET_TRUBO_TIME(n)      BITFIELD_VAL(REG_VCORE_L1_DET_TRUBO_TIME, n)
#define REG_VCORE_L1_ENDURE_TIME_SHIFT      0
#define REG_VCORE_L1_ENDURE_TIME_MASK       (0x1F << REG_VCORE_L1_ENDURE_TIME_SHIFT)
#define REG_VCORE_L1_ENDURE_TIME(n)         BITFIELD_VAL(REG_VCORE_L1_ENDURE_TIME, n)

// REG_145
#define REG_VCORE_L1_OSCP_FUNC_EN           (1 << 2)
#define REG_VCORE_L1_OCP_EN_DR              (1 << 1)
#define REG_VCORE_L1_OCP_EN_REG             (1 << 0)

// REG_146
#define REG_VCORE_L1_BREAK_TIME_SHIFT       14
#define REG_VCORE_L1_BREAK_TIME_MASK        (0x3 << REG_VCORE_L1_BREAK_TIME_SHIFT)
#define REG_VCORE_L1_BREAK_TIME(n)          BITFIELD_VAL(REG_VCORE_L1_BREAK_TIME, n)
#define REG_VCORE_L1_REBOOT_CYCLE_SHIFT     10
#define REG_VCORE_L1_REBOOT_CYCLE_MASK      (0xF << REG_VCORE_L1_REBOOT_CYCLE_SHIFT)
#define REG_VCORE_L1_REBOOT_CYCLE(n)        BITFIELD_VAL(REG_VCORE_L1_REBOOT_CYCLE, n)
#define REG_VCORE_L1_REBOOT_TRIGGER         (1 << 9)
#define REG_VCORE_L1_REBOOT_TRIGGER_DR      (1 << 8)
#define REG_VCORE_L1_LDO_OFF_DR             (1 << 7)
#define REG_VCORE_L1_LDO_OFF_REG            (1 << 6)
#define REG_VCORE_L1_PMU_OFF_DR             (1 << 5)
#define REG_VCORE_L1_PMU_OFF_REG            (1 << 4)
#define REG_VCORE_L1_PROTEC_CYCLE_SHIFT     2
#define REG_VCORE_L1_PROTEC_CYCLE_MASK      (0x3 << REG_VCORE_L1_PROTEC_CYCLE_SHIFT)
#define REG_VCORE_L1_PROTEC_CYCLE(n)        BITFIELD_VAL(REG_VCORE_L1_PROTEC_CYCLE, n)
#define REG_VCORE_L1_HANDLE_SEL_PMU_OFF     (1 << 1)
#define REG_VCORE_L1_HW_MODE                (1 << 0)

// REG_147
#define REG_VCORE_L1_INTR_EN                (1 << 2)
#define REG_VCORE_L1_INTR_MSK               (1 << 1)
#define REG_VCORE_L1_INTR_CLR               (1 << 0)


// REG_148
#define REG_VANA_DET_TIME_SHIFT             10
#define REG_VANA_DET_TIME_MASK              (0x1F << REG_VANA_DET_TIME_SHIFT)
#define REG_VANA_DET_TIME(n)                BITFIELD_VAL(REG_VANA_DET_TIME, n)
#define REG_VANA_DET_TRUBO_TIME_SHIFT       5
#define REG_VANA_DET_TRUBO_TIME_MASK        (0x1F << REG_VANA_DET_TRUBO_TIME_SHIFT)
#define REG_VANA_DET_TRUBO_TIME(n)          BITFIELD_VAL(REG_VANA_DET_TRUBO_TIME, n)
#define REG_VANA_ENDURE_TIME_SHIFT          0
#define REG_VANA_ENDURE_TIME_MASK           (0x1F << REG_VANA_ENDURE_TIME_SHIFT)
#define REG_VANA_ENDURE_TIME(n)             BITFIELD_VAL(REG_VANA_ENDURE_TIME, n)

// REG_149
#define REG_VANA_OSCP_FUNC_EN               (1 << 2)
#define REG_VANA_OCP_EN_DR                  (1 << 1)
#define REG_VANA_OCP_EN_REG                 (1 << 0)

// REG_14A
#define REG_VANA_BREAK_TIME_SHIFT           14
#define REG_VANA_BREAK_TIME_MASK            (0x3 << REG_VANA_BREAK_TIME_SHIFT)
#define REG_VANA_BREAK_TIME(n)              BITFIELD_VAL(REG_VANA_BREAK_TIME, n)
#define REG_VANA_REBOOT_CYCLE_SHIFT         10
#define REG_VANA_REBOOT_CYCLE_MASK          (0xF << REG_VANA_REBOOT_CYCLE_SHIFT)
#define REG_VANA_REBOOT_CYCLE(n)            BITFIELD_VAL(REG_VANA_REBOOT_CYCLE, n)
#define REG_VANA_REBOOT_TRIGGER             (1 << 9)
#define REG_VANA_REBOOT_TRIGGER_DR          (1 << 8)
#define REG_VANA_LDO_OFF_DR                 (1 << 7)
#define REG_VANA_LDO_OFF_REG                (1 << 6)
#define REG_VANA_PMU_OFF_DR                 (1 << 5)
#define REG_VANA_PMU_OFF_REG                (1 << 4)
#define REG_VANA_PROTEC_CYCLE_SHIFT         2
#define REG_VANA_PROTEC_CYCLE_MASK          (0x3 << REG_VANA_PROTEC_CYCLE_SHIFT)
#define REG_VANA_PROTEC_CYCLE(n)            BITFIELD_VAL(REG_VANA_PROTEC_CYCLE, n)
#define REG_VANA_HANDLE_SEL_PMU_OFF         (1 << 1)
#define REG_VANA_HW_MODE                    (1 << 0)

// REG_14B
#define REG_VANA_INTR_EN                    (1 << 2)
#define REG_VANA_INTR_MSK                   (1 << 1)
#define REG_VANA_INTR_CLR                   (1 << 0)

// REG_14C
#define REG_VIO_DET_TIME_SHIFT              10
#define REG_VIO_DET_TIME_MASK               (0x1F << REG_VIO_DET_TIME_SHIFT)
#define REG_VIO_DET_TIME(n)                 BITFIELD_VAL(REG_VIO_DET_TIME, n)
#define REG_VIO_DET_TRUBO_TIME_SHIFT        5
#define REG_VIO_DET_TRUBO_TIME_MASK         (0x1F << REG_VIO_DET_TRUBO_TIME_SHIFT)
#define REG_VIO_DET_TRUBO_TIME(n)           BITFIELD_VAL(REG_VIO_DET_TRUBO_TIME, n)
#define REG_VIO_ENDURE_TIME_SHIFT           0
#define REG_VIO_ENDURE_TIME_MASK            (0x1F << REG_VIO_ENDURE_TIME_SHIFT)
#define REG_VIO_ENDURE_TIME(n)              BITFIELD_VAL(REG_VIO_ENDURE_TIME, n)

// REG_14D
#define REG_VIO_OSCP_FUNC_EN                (1 << 2)
#define REG_VIO_OCP_EN_DR                   (1 << 1)
#define REG_VIO_OCP_EN_REG                  (1 << 0)

// REG_14E
#define REG_VIO_BREAK_TIME_SHIFT            14
#define REG_VIO_BREAK_TIME_MASK             (0x3 << REG_VIO_BREAK_TIME_SHIFT)
#define REG_VIO_BREAK_TIME(n)               BITFIELD_VAL(REG_VIO_BREAK_TIME, n)
#define REG_VIO_REBOOT_CYCLE_SHIFT          10
#define REG_VIO_REBOOT_CYCLE_MASK           (0xF << REG_VIO_REBOOT_CYCLE_SHIFT)
#define REG_VIO_REBOOT_CYCLE(n)             BITFIELD_VAL(REG_VIO_REBOOT_CYCLE, n)
#define REG_VIO_REBOOT_TRIGGER              (1 << 9)
#define REG_VIO_REBOOT_TRIGGER_DR           (1 << 8)
#define REG_VIO_LDO_OFF_DR                  (1 << 7)
#define REG_VIO_LDO_OFF_REG                 (1 << 6)
#define REG_VIO_PMU_OFF_DR                  (1 << 5)
#define REG_VIO_PMU_OFF_REG                 (1 << 4)
#define REG_VIO_PROTEC_CYCLE_SHIFT          2
#define REG_VIO_PROTEC_CYCLE_MASK           (0x3 << REG_VIO_PROTEC_CYCLE_SHIFT)
#define REG_VIO_PROTEC_CYCLE(n)             BITFIELD_VAL(REG_VIO_PROTEC_CYCLE, n)
#define REG_VIO_HANDLE_SEL_PMU_OFF          (1 << 1)
#define REG_VIO_HW_MODE                     (1 << 0)

// REG_14F
#define REG_VIO_INTR_EN                     (1 << 2)
#define REG_VIO_INTR_MSK                    (1 << 1)
#define REG_VIO_INTR_CLR                    (1 << 0)

// REG_150
#define REG_VMEM_DET_TIME_SHIFT             10
#define REG_VMEM_DET_TIME_MASK              (0x1F << REG_VMEM_DET_TIME_SHIFT)
#define REG_VMEM_DET_TIME(n)                BITFIELD_VAL(REG_VMEM_DET_TIME, n)
#define REG_VMEM_DET_TRUBO_TIME_SHIFT       5
#define REG_VMEM_DET_TRUBO_TIME_MASK        (0x1F << REG_VMEM_DET_TRUBO_TIME_SHIFT)
#define REG_VMEM_DET_TRUBO_TIME(n)          BITFIELD_VAL(REG_VMEM_DET_TRUBO_TIME, n)
#define REG_VMEM_ENDURE_TIME_SHIFT          0
#define REG_VMEM_ENDURE_TIME_MASK           (0x1F << REG_VMEM_ENDURE_TIME_SHIFT)
#define REG_VMEM_ENDURE_TIME(n)             BITFIELD_VAL(REG_VMEM_ENDURE_TIME, n)

// REG_151
#define REG_VMEM_OSCP_FUNC_EN               (1 << 2)
#define REG_VMEM_OCP_EN_DR                  (1 << 1)
#define REG_VMEM_OCP_EN_REG                 (1 << 0)

// REG_152
#define REG_VMEM_BREAK_TIME_SHIFT           14
#define REG_VMEM_BREAK_TIME_MASK            (0x3 << REG_VMEM_BREAK_TIME_SHIFT)
#define REG_VMEM_BREAK_TIME(n)              BITFIELD_VAL(REG_VMEM_BREAK_TIME, n)
#define REG_VMEM_REBOOT_CYCLE_SHIFT         10
#define REG_VMEM_REBOOT_CYCLE_MASK          (0xF << REG_VMEM_REBOOT_CYCLE_SHIFT)
#define REG_VMEM_REBOOT_CYCLE(n)            BITFIELD_VAL(REG_VMEM_REBOOT_CYCLE, n)
#define REG_VMEM_REBOOT_TRIGGER             (1 << 9)
#define REG_VMEM_REBOOT_TRIGGER_DR          (1 << 8)
#define REG_VMEM_LDO_OFF_DR                 (1 << 7)
#define REG_VMEM_LDO_OFF_REG                (1 << 6)
#define REG_VMEM_PMU_OFF_DR                 (1 << 5)
#define REG_VMEM_PMU_OFF_REG                (1 << 4)
#define REG_VMEM_PROTEC_CYCLE_SHIFT         2
#define REG_VMEM_PROTEC_CYCLE_MASK          (0x3 << REG_VMEM_PROTEC_CYCLE_SHIFT)
#define REG_VMEM_PROTEC_CYCLE(n)            BITFIELD_VAL(REG_VMEM_PROTEC_CYCLE, n)
#define REG_VMEM_HANDLE_SEL_PMU_OFF         (1 << 1)
#define REG_VMEM_HW_MODE                    (1 << 0)

// REG_153
#define REG_VMEM_INTR_EN                    (1 << 2)
#define REG_VMEM_INTR_MSK                   (1 << 1)
#define REG_VMEM_INTR_CLR                   (1 << 0)

// REG_154
#define REG_VUSB_DET_TIME_SHIFT             10
#define REG_VUSB_DET_TIME_MASK              (0x1F << REG_VUSB_DET_TIME_SHIFT)
#define REG_VUSB_DET_TIME(n)                BITFIELD_VAL(REG_VUSB_DET_TIME, n)
#define REG_VUSB_DET_TRUBO_TIME_SHIFT       5
#define REG_VUSB_DET_TRUBO_TIME_MASK        (0x1F << REG_VUSB_DET_TRUBO_TIME_SHIFT)
#define REG_VUSB_DET_TRUBO_TIME(n)          BITFIELD_VAL(REG_VUSB_DET_TRUBO_TIME, n)
#define REG_VUSB_ENDURE_TIME_SHIFT          0
#define REG_VUSB_ENDURE_TIME_MASK           (0x1F << REG_VUSB_ENDURE_TIME_SHIFT)
#define REG_VUSB_ENDURE_TIME(n)             BITFIELD_VAL(REG_VUSB_ENDURE_TIME, n)

// REG_155
#define REG_VUSB_OSCP_FUNC_EN               (1 << 2)
#define REG_VUSB_OCP_EN_DR                  (1 << 1)
#define REG_VUSB_OCP_EN_REG                 (1 << 0)

// REG_156
#define REG_VUSB_BREAK_TIME_SHIFT           14
#define REG_VUSB_BREAK_TIME_MASK            (0x3 << REG_VUSB_BREAK_TIME_SHIFT)
#define REG_VUSB_BREAK_TIME(n)              BITFIELD_VAL(REG_VUSB_BREAK_TIME, n)
#define REG_VUSB_REBOOT_CYCLE_SHIFT         10
#define REG_VUSB_REBOOT_CYCLE_MASK          (0xF << REG_VUSB_REBOOT_CYCLE_SHIFT)
#define REG_VUSB_REBOOT_CYCLE(n)            BITFIELD_VAL(REG_VUSB_REBOOT_CYCLE, n)
#define REG_VUSB_REBOOT_TRIGGER             (1 << 9)
#define REG_VUSB_REBOOT_TRIGGER_DR          (1 << 8)
#define REG_VUSB_LDO_OFF_DR                 (1 << 7)
#define REG_VUSB_LDO_OFF_REG                (1 << 6)
#define REG_VUSB_PMU_OFF_DR                 (1 << 5)
#define REG_VUSB_PMU_OFF_REG                (1 << 4)
#define REG_VUSB_PROTEC_CYCLE_SHIFT         2
#define REG_VUSB_PROTEC_CYCLE_MASK          (0x3 << REG_VUSB_PROTEC_CYCLE_SHIFT)
#define REG_VUSB_PROTEC_CYCLE(n)            BITFIELD_VAL(REG_VUSB_PROTEC_CYCLE, n)
#define REG_VUSB_HANDLE_SEL_PMU_OFF         (1 << 1)
#define REG_VUSB_HW_MODE                    (1 << 0)

// REG_157
#define REG_VUSB_INTR_EN                    (1 << 2)
#define REG_VUSB_INTR_MSK                   (1 << 1)
#define REG_VUSB_INTR_CLR                   (1 << 0)

// REG_158
#define REG_VMEM2_DET_TIME_SHIFT            10
#define REG_VMEM2_DET_TIME_MASK             (0x1F << REG_VMEM2_DET_TIME_SHIFT)
#define REG_VMEM2_DET_TIME(n)               BITFIELD_VAL(REG_VMEM2_DET_TIME, n)
#define REG_VMEM2_DET_TRUBO_TIME_SHIFT      5
#define REG_VMEM2_DET_TRUBO_TIME_MASK       (0x1F << REG_VMEM2_DET_TRUBO_TIME_SHIFT)
#define REG_VMEM2_DET_TRUBO_TIME(n)         BITFIELD_VAL(REG_VMEM2_DET_TRUBO_TIME, n)
#define REG_VMEM2_ENDURE_TIME_SHIFT         0
#define REG_VMEM2_ENDURE_TIME_MASK          (0x1F << REG_VMEM2_ENDURE_TIME_SHIFT)
#define REG_VMEM2_ENDURE_TIME(n)            BITFIELD_VAL(REG_VMEM2_ENDURE_TIME, n)

// REG_159
#define REG_VMEM2_OSCP_FUNC_EN              (1 << 2)
#define REG_VMEM2_OCP_EN_DR                 (1 << 1)
#define REG_VMEM2_OCP_EN_REG                (1 << 0)

// REG_15A
#define REG_VMEM2_BREAK_TIME_SHIFT          14
#define REG_VMEM2_BREAK_TIME_MASK           (0x3 << REG_VMEM2_BREAK_TIME_SHIFT)
#define REG_VMEM2_BREAK_TIME(n)             BITFIELD_VAL(REG_VMEM2_BREAK_TIME, n)
#define REG_VMEM2_REBOOT_CYCLE_SHIFT        10
#define REG_VMEM2_REBOOT_CYCLE_MASK         (0xF << REG_VMEM2_REBOOT_CYCLE_SHIFT)
#define REG_VMEM2_REBOOT_CYCLE(n)           BITFIELD_VAL(REG_VMEM2_REBOOT_CYCLE, n)
#define REG_VMEM2_REBOOT_TRIGGER            (1 << 9)
#define REG_VMEM2_REBOOT_TRIGGER_DR         (1 << 8)
#define REG_VMEM2_LDO_OFF_DR                (1 << 7)
#define REG_VMEM2_LDO_OFF_REG               (1 << 6)
#define REG_VMEM2_PMU_OFF_DR                (1 << 5)
#define REG_VMEM2_PMU_OFF_REG               (1 << 4)
#define REG_VMEM2_PROTEC_CYCLE_SHIFT        2
#define REG_VMEM2_PROTEC_CYCLE_MASK         (0x3 << REG_VMEM2_PROTEC_CYCLE_SHIFT)
#define REG_VMEM2_PROTEC_CYCLE(n)           BITFIELD_VAL(REG_VMEM2_PROTEC_CYCLE, n)
#define REG_VMEM2_HANDLE_SEL_PMU_OFF        (1 << 1)
#define REG_VMEM2_HW_MODE                   (1 << 0)

// REG_15B
#define REG_VMEM2_INTR_EN                   (1 << 2)
#define REG_VMEM2_INTR_MSK                  (1 << 1)
#define REG_VMEM2_INTR_CLR                  (1 << 0)

// REG_15C
#define REG_VMEM3_DET_TIME_SHIFT            10
#define REG_VMEM3_DET_TIME_MASK             (0x1F << REG_VMEM3_DET_TIME_SHIFT)
#define REG_VMEM3_DET_TIME(n)               BITFIELD_VAL(REG_VMEM3_DET_TIME, n)
#define REG_VMEM3_DET_TRUBO_TIME_SHIFT      5
#define REG_VMEM3_DET_TRUBO_TIME_MASK       (0x1F << REG_VMEM3_DET_TRUBO_TIME_SHIFT)
#define REG_VMEM3_DET_TRUBO_TIME(n)         BITFIELD_VAL(REG_VMEM3_DET_TRUBO_TIME, n)
#define REG_VMEM3_ENDURE_TIME_SHIFT         0
#define REG_VMEM3_ENDURE_TIME_MASK          (0x1F << REG_VMEM3_ENDURE_TIME_SHIFT)
#define REG_VMEM3_ENDURE_TIME(n)            BITFIELD_VAL(REG_VMEM3_ENDURE_TIME, n)

// REG_15D
#define REG_VMEM3_OSCP_FUNC_EN              (1 << 2)
#define REG_VMEM3_OCP_EN_DR                 (1 << 1)
#define REG_VMEM3_OCP_EN_REG                (1 << 0)

// REG_15E
#define REG_VMEM3_BREAK_TIME_SHIFT          14
#define REG_VMEM3_BREAK_TIME_MASK           (0x3 << REG_VMEM3_BREAK_TIME_SHIFT)
#define REG_VMEM3_BREAK_TIME(n)             BITFIELD_VAL(REG_VMEM3_BREAK_TIME, n)
#define REG_VMEM3_REBOOT_CYCLE_SHIFT        10
#define REG_VMEM3_REBOOT_CYCLE_MASK         (0xF << REG_VMEM3_REBOOT_CYCLE_SHIFT)
#define REG_VMEM3_REBOOT_CYCLE(n)           BITFIELD_VAL(REG_VMEM3_REBOOT_CYCLE, n)
#define REG_VMEM3_REBOOT_TRIGGER            (1 << 9)
#define REG_VMEM3_REBOOT_TRIGGER_DR         (1 << 8)
#define REG_VMEM3_LDO_OFF_DR                (1 << 7)
#define REG_VMEM3_LDO_OFF_REG               (1 << 6)
#define REG_VMEM3_PMU_OFF_DR                (1 << 5)
#define REG_VMEM3_PMU_OFF_REG               (1 << 4)
#define REG_VMEM3_PROTEC_CYCLE_SHIFT        2
#define REG_VMEM3_PROTEC_CYCLE_MASK         (0x3 << REG_VMEM3_PROTEC_CYCLE_SHIFT)
#define REG_VMEM3_PROTEC_CYCLE(n)           BITFIELD_VAL(REG_VMEM3_PROTEC_CYCLE, n)
#define REG_VMEM3_HANDLE_SEL_PMU_OFF        (1 << 1)
#define REG_VMEM3_HW_MODE                   (1 << 0)

// REG_15F
#define REG_VMEM3_INTR_EN                   (1 << 2)
#define REG_VMEM3_INTR_MSK                  (1 << 1)
#define REG_VMEM3_INTR_CLR                  (1 << 0)

// REG_160
#define REG_VSENSOR_DET_TIME_SHIFT          10
#define REG_VSENSOR_DET_TIME_MASK           (0x1F << REG_VSENSOR_DET_TIME_SHIFT)
#define REG_VSENSOR_DET_TIME(n)             BITFIELD_VAL(REG_VSENSOR_DET_TIME, n)
#define REG_VSENSOR_DET_TRUBO_TIME_SHIFT    5
#define REG_VSENSOR_DET_TRUBO_TIME_MASK     (0x1F << REG_VSENSOR_DET_TRUBO_TIME_SHIFT)
#define REG_VSENSOR_DET_TRUBO_TIME(n)       BITFIELD_VAL(REG_VSENSOR_DET_TRUBO_TIME, n)
#define REG_VSENSOR_ENDURE_TIME_SHIFT       0
#define REG_VSENSOR_ENDURE_TIME_MASK        (0x1F << REG_VSENSOR_ENDURE_TIME_SHIFT)
#define REG_VSENSOR_ENDURE_TIME(n)          BITFIELD_VAL(REG_VSENSOR_ENDURE_TIME, n)

// REG_161
#define REG_VSENSOR_OSCP_FUNC_EN            (1 << 2)
#define REG_VSENSOR_OCP_EN_DR               (1 << 1)
#define REG_VSENSOR_OCP_EN_REG              (1 << 0)

// REG_162
#define REG_VSENSOR_BREAK_TIME_SHIFT        14
#define REG_VSENSOR_BREAK_TIME_MASK         (0x3 << REG_VSENSOR_BREAK_TIME_SHIFT)
#define REG_VSENSOR_BREAK_TIME(n)           BITFIELD_VAL(REG_VSENSOR_BREAK_TIME, n)
#define REG_VSENSOR_REBOOT_CYCLE_SHIFT      10
#define REG_VSENSOR_REBOOT_CYCLE_MASK       (0xF << REG_VSENSOR_REBOOT_CYCLE_SHIFT)
#define REG_VSENSOR_REBOOT_CYCLE(n)         BITFIELD_VAL(REG_VSENSOR_REBOOT_CYCLE, n)
#define REG_VSENSOR_REBOOT_TRIGGER          (1 << 9)
#define REG_VSENSOR_REBOOT_TRIGGER_DR       (1 << 8)
#define REG_VSENSOR_LDO_OFF_DR              (1 << 7)
#define REG_VSENSOR_LDO_OFF_REG             (1 << 6)
#define REG_VSENSOR_PMU_OFF_DR              (1 << 5)
#define REG_VSENSOR_PMU_OFF_REG             (1 << 4)
#define REG_VSENSOR_PROTEC_CYCLE_SHIFT      2
#define REG_VSENSOR_PROTEC_CYCLE_MASK       (0x3 << REG_VSENSOR_PROTEC_CYCLE_SHIFT)
#define REG_VSENSOR_PROTEC_CYCLE(n)         BITFIELD_VAL(REG_VSENSOR_PROTEC_CYCLE, n)
#define REG_VSENSOR_HANDLE_SEL_PMU_OFF      (1 << 1)
#define REG_VSENSOR_HW_MODE                 (1 << 0)

// REG_163
#define REG_VSENSOR_INTR_EN                 (1 << 2)
#define REG_VSENSOR_INTR_MSK                (1 << 1)
#define REG_VSENSOR_INTR_CLR                (1 << 0)

// REG_167
#define REG_REFRESH_D1_DLY_SHIFT            8
#define REG_REFRESH_D1_DLY_MASK             (0xF << REG_REFRESH_D1_DLY_SHIFT)
#define REG_REFRESH_D1_DLY(n)               BITFIELD_VAL(REG_REFRESH_D1_DLY, n)
#define REG_REFRESH_D2_DLY_SHIFT            4
#define REG_REFRESH_D2_DLY_MASK             (0xF << REG_REFRESH_D2_DLY_SHIFT)
#define REG_REFRESH_D2_DLY(n)               BITFIELD_VAL(REG_REFRESH_D2_DLY, n)
#define REG_REFRESH_D3_DLY_SHIFT            0
#define REG_REFRESH_D3_DLY_MASK             (0xF << REG_REFRESH_D3_DLY_SHIFT)
#define REG_REFRESH_D3_DLY(n)               BITFIELD_VAL(REG_REFRESH_D3_DLY, n)

// REG_168
#define REG_BUCK_VCORE_DET_TIME_SHIFT       10
#define REG_BUCK_VCORE_DET_TIME_MASK        (0x1F << REG_BUCK_VCORE_DET_TIME_SHIFT)
#define REG_BUCK_VCORE_DET_TIME(n)          BITFIELD_VAL(REG_BUCK_VCORE_DET_TIME, n)
#define REG_BUCK_VCORE_DET_TRUBO_TIME_SHIFT 5
#define REG_BUCK_VCORE_DET_TRUBO_TIME_MASK  (0x1F << REG_BUCK_VCORE_DET_TRUBO_TIME_SHIFT)
#define REG_BUCK_VCORE_DET_TRUBO_TIME(n)    BITFIELD_VAL(REG_BUCK_VCORE_DET_TRUBO_TIME, n)
#define REG_BUCK_VCORE_ENDURE_TIME_SHIFT    0
#define REG_BUCK_VCORE_ENDURE_TIME_MASK     (0x1F << REG_BUCK_VCORE_ENDURE_TIME_SHIFT)
#define REG_BUCK_VCORE_ENDURE_TIME(n)       BITFIELD_VAL(REG_BUCK_VCORE_ENDURE_TIME, n)

// REG_169
#define REG_BUCK_VCORE_OSCP_FUNC_EN         (1 << 2)
#define REG_BUCK_VCORE_OCP_EN_DR            (1 << 1)
#define REG_BUCK_VCORE_OCP_EN_REG           (1 << 0)

// REG_16A
#define REG_BUCK_VCORE_BREAK_TIME_SHIFT     14
#define REG_BUCK_VCORE_BREAK_TIME_MASK      (0x3 << REG_BUCK_VCORE_BREAK_TIME_SHIFT)
#define REG_BUCK_VCORE_BREAK_TIME(n)        BITFIELD_VAL(REG_BUCK_VCORE_BREAK_TIME, n)
#define REG_BUCK_VCORE_REBOOT_CYCLE_SHIFT   10
#define REG_BUCK_VCORE_REBOOT_CYCLE_MASK    (0xF << REG_BUCK_VCORE_REBOOT_CYCLE_SHIFT)
#define REG_BUCK_VCORE_REBOOT_CYCLE(n)      BITFIELD_VAL(REG_BUCK_VCORE_REBOOT_CYCLE, n)
#define REG_BUCK_VCORE_REBOOT_TRIGGER       (1 << 9)
#define REG_BUCK_VCORE_REBOOT_TRIGGER_DR    (1 << 8)
#define REG_BUCK_VCORE_DCDC_OFF_DR          (1 << 7)
#define REG_BUCK_VCORE_DCDC_OFF_REG         (1 << 6)
#define REG_BUCK_VCORE_PMU_OFF_DR           (1 << 5)
#define REG_BUCK_VCORE_PMU_OFF_REG          (1 << 4)
#define REG_BUCK_VCORE_PROTEC_CYCLE_SHIFT   2
#define REG_BUCK_VCORE_PROTEC_CYCLE_MASK    (0x3 << REG_BUCK_VCORE_PROTEC_CYCLE_SHIFT)
#define REG_BUCK_VCORE_PROTEC_CYCLE(n)      BITFIELD_VAL(REG_BUCK_VCORE_PROTEC_CYCLE, n)
#define REG_BUCK_VCORE_HANDLE_SEL_PMU_OFF   (1 << 1)
#define REG_BUCK_VCORE_HW_MODE              (1 << 0)

// REG_16B
#define REG_BUCK_VCORE_INTR_EN              (1 << 2)
#define REG_BUCK_VCORE_INTR_MSK             (1 << 1)
#define REG_BUCK_VCORE_INTR_CLR             (1 << 0)

// REG_16C
#define REG_BUCK_VANA_DET_TIME_SHIFT        10
#define REG_BUCK_VANA_DET_TIME_MASK         (0x1F << REG_BUCK_VANA_DET_TIME_SHIFT)
#define REG_BUCK_VANA_DET_TIME(n)           BITFIELD_VAL(REG_BUCK_VANA_DET_TIME, n)
#define REG_BUCK_VANA_DET_TRUBO_TIME_SHIFT  5
#define REG_BUCK_VANA_DET_TRUBO_TIME_MASK   (0x1F << REG_BUCK_VANA_DET_TRUBO_TIME_SHIFT)
#define REG_BUCK_VANA_DET_TRUBO_TIME(n)     BITFIELD_VAL(REG_BUCK_VANA_DET_TRUBO_TIME, n)
#define REG_BUCK_VANA_ENDURE_TIME_SHIFT     0
#define REG_BUCK_VANA_ENDURE_TIME_MASK      (0x1F << REG_BUCK_VANA_ENDURE_TIME_SHIFT)
#define REG_BUCK_VANA_ENDURE_TIME(n)        BITFIELD_VAL(REG_BUCK_VANA_ENDURE_TIME, n)

// REG_16D
#define REG_BUCK_VANA_OSCP_FUNC_EN          (1 << 2)
#define REG_BUCK_VANA_OCP_EN_DR             (1 << 1)
#define REG_BUCK_VANA_OCP_EN_REG            (1 << 0)

// REG_16E
#define REG_BUCK_VANA_BREAK_TIME_SHIFT      14
#define REG_BUCK_VANA_BREAK_TIME_MASK       (0x3 << REG_BUCK_VANA_BREAK_TIME_SHIFT)
#define REG_BUCK_VANA_BREAK_TIME(n)         BITFIELD_VAL(REG_BUCK_VANA_BREAK_TIME, n)
#define REG_BUCK_VANA_REBOOT_CYCLE_SHIFT    10
#define REG_BUCK_VANA_REBOOT_CYCLE_MASK     (0xF << REG_BUCK_VANA_REBOOT_CYCLE_SHIFT)
#define REG_BUCK_VANA_REBOOT_CYCLE(n)       BITFIELD_VAL(REG_BUCK_VANA_REBOOT_CYCLE, n)
#define REG_BUCK_VANA_REBOOT_TRIGGER        (1 << 9)
#define REG_BUCK_VANA_REBOOT_TRIGGER_DR     (1 << 8)
#define REG_BUCK_VANA_DCDC_OFF_DR           (1 << 7)
#define REG_BUCK_VANA_DCDC_OFF_REG          (1 << 6)
#define REG_BUCK_VANA_PMU_OFF_DR            (1 << 5)
#define REG_BUCK_VANA_PMU_OFF_REG           (1 << 4)
#define REG_BUCK_VANA_PROTEC_CYCLE_SHIFT    2
#define REG_BUCK_VANA_PROTEC_CYCLE_MASK     (0x3 << REG_BUCK_VANA_PROTEC_CYCLE_SHIFT)
#define REG_BUCK_VANA_PROTEC_CYCLE(n)       BITFIELD_VAL(REG_BUCK_VANA_PROTEC_CYCLE, n)
#define REG_BUCK_VANA_HANDLE_SEL_PMU_OFF    (1 << 1)
#define REG_BUCK_VANA_HW_MODE               (1 << 0)

// REG_16F
#define REG_BUCK_VANA_INTR_EN               (1 << 2)
#define REG_BUCK_VANA_INTR_MSK              (1 << 1)
#define REG_BUCK_VANA_INTR_CLR              (1 << 0)

// REG_170
#define REG_BUCK_VHPPA_DET_TIME_SHIFT       10
#define REG_BUCK_VHPPA_DET_TIME_MASK        (0x1F << REG_BUCK_VHPPA_DET_TIME_SHIFT)
#define REG_BUCK_VHPPA_DET_TIME(n)          BITFIELD_VAL(REG_BUCK_VHPPA_DET_TIME, n)
#define REG_BUCK_VHPPA_DET_TRUBO_TIME_SHIFT 5
#define REG_BUCK_VHPPA_DET_TRUBO_TIME_MASK  (0x1F << REG_BUCK_VHPPA_DET_TRUBO_TIME_SHIFT)
#define REG_BUCK_VHPPA_DET_TRUBO_TIME(n)    BITFIELD_VAL(REG_BUCK_VHPPA_DET_TRUBO_TIME, n)
#define REG_BUCK_VHPPA_ENDURE_TIME_SHIFT    0
#define REG_BUCK_VHPPA_ENDURE_TIME_MASK     (0x1F << REG_BUCK_VHPPA_ENDURE_TIME_SHIFT)
#define REG_BUCK_VHPPA_ENDURE_TIME(n)       BITFIELD_VAL(REG_BUCK_VHPPA_ENDURE_TIME, n)

// REG_171
#define REG_BUCK_VHPPA_OSCP_FUNC_EN         (1 << 2)
#define REG_BUCK_VHPPA_OCP_EN_DR            (1 << 1)
#define REG_BUCK_VHPPA_OCP_EN_REG           (1 << 0)

// REG_172
#define REG_BUCK_VHPPA_BREAK_TIME_SHIFT     14
#define REG_BUCK_VHPPA_BREAK_TIME_MASK      (0x3 << REG_BUCK_VHPPA_BREAK_TIME_SHIFT)
#define REG_BUCK_VHPPA_BREAK_TIME(n)        BITFIELD_VAL(REG_BUCK_VHPPA_BREAK_TIME, n)
#define REG_BUCK_VHPPA_REBOOT_CYCLE_SHIFT   10
#define REG_BUCK_VHPPA_REBOOT_CYCLE_MASK    (0xF << REG_BUCK_VHPPA_REBOOT_CYCLE_SHIFT)
#define REG_BUCK_VHPPA_REBOOT_CYCLE(n)      BITFIELD_VAL(REG_BUCK_VHPPA_REBOOT_CYCLE, n)
#define REG_BUCK_VHPPA_REBOOT_TRIGGER       (1 << 9)
#define REG_BUCK_VHPPA_REBOOT_TRIGGER_DR    (1 << 8)
#define REG_BUCK_VHPPA_DCDC_OFF_DR          (1 << 7)
#define REG_BUCK_VHPPA_DCDC_OFF_REG         (1 << 6)
#define REG_BUCK_VHPPA_PMU_OFF_DR           (1 << 5)
#define REG_BUCK_VHPPA_PMU_OFF_REG          (1 << 4)
#define REG_BUCK_VHPPA_PROTEC_CYCLE_SHIFT   2
#define REG_BUCK_VHPPA_PROTEC_CYCLE_MASK    (0x3 << REG_BUCK_VHPPA_PROTEC_CYCLE_SHIFT)
#define REG_BUCK_VHPPA_PROTEC_CYCLE(n)      BITFIELD_VAL(REG_BUCK_VHPPA_PROTEC_CYCLE, n)
#define REG_BUCK_VHPPA_HANDLE_SEL_PMU_OFF   (1 << 1)
#define REG_BUCK_VHPPA_HW_MODE              (1 << 0)

// REG_173
#define REG_BUCK_VHPPA_INTR_EN              (1 << 2)
#define REG_BUCK_VHPPA_INTR_MSK             (1 << 1)
#define REG_BUCK_VHPPA_INTR_CLR             (1 << 0)

// REG_174
#define REG_BUCK_VCORE_OFFSET_DR            (1 << 12)
#define REG_BUCK_VCORE_OFFSET_SHIFT         7
#define REG_BUCK_VCORE_OFFSET_MASK          (0x1F << REG_BUCK_VCORE_OFFSET_SHIFT)
#define REG_BUCK_VCORE_OFFSET(n)            BITFIELD_VAL(REG_BUCK_VCORE_OFFSET, n)
#define REG_BUCK_VCORE_RAMP_EN              (1 << 6)
#define REG_BUCK_VCORE_OFFSET_CALIB_EN      (1 << 5)
#define REG_BUCK_VCORE_OFFSET_POL_SEL       (1 << 4)
#define REG_BUCK_VCORE_RAMP_STEP_SHIFT      0
#define REG_BUCK_VCORE_RAMP_STEP_MASK       (0xF << REG_BUCK_VCORE_RAMP_STEP_SHIFT)
#define REG_BUCK_VCORE_RAMP_STEP(n)         BITFIELD_VAL(REG_BUCK_VCORE_RAMP_STEP, n)

// REG_175
#define REG_BUCK_VANA_OFFSET_DR             (1 << 12)
#define REG_BUCK_VANA_OFFSET_SHIFT          7
#define REG_BUCK_VANA_OFFSET_MASK           (0x1F << REG_BUCK_VANA_OFFSET_SHIFT)
#define REG_BUCK_VANA_OFFSET(n)             BITFIELD_VAL(REG_BUCK_VANA_OFFSET, n)
#define REG_BUCK_VANA_RAMP_EN               (1 << 6)
#define REG_BUCK_VANA_OFFSET_CALIB_EN       (1 << 5)
#define REG_BUCK_VANA_OFFSET_POL_SEL        (1 << 4)
#define REG_BUCK_VANA_RAMP_STEP_SHIFT       0
#define REG_BUCK_VANA_RAMP_STEP_MASK        (0xF << REG_BUCK_VANA_RAMP_STEP_SHIFT)
#define REG_BUCK_VANA_RAMP_STEP(n)          BITFIELD_VAL(REG_BUCK_VANA_RAMP_STEP, n)

// REG_176
#define REG_BUCK_VHPPA_OFFSET_DR            (1 << 12)
#define REG_BUCK_VHPPA_OFFSET_SHIFT         7
#define REG_BUCK_VHPPA_OFFSET_MASK          (0x1F << REG_BUCK_VHPPA_OFFSET_SHIFT)
#define REG_BUCK_VHPPA_OFFSET(n)            BITFIELD_VAL(REG_BUCK_VHPPA_OFFSET, n)
#define REG_BUCK_VHPPA_RAMP_EN              (1 << 6)
#define REG_BUCK_VHPPA_OFFSET_CALIB_EN      (1 << 5)
#define REG_BUCK_VHPPA_OFFSET_POL_SEL       (1 << 4)
#define REG_BUCK_VHPPA_RAMP_STEP_SHIFT      0
#define REG_BUCK_VHPPA_RAMP_STEP_MASK       (0xF << REG_BUCK_VHPPA_RAMP_STEP_SHIFT)
#define REG_BUCK_VHPPA_RAMP_STEP(n)         BITFIELD_VAL(REG_BUCK_VHPPA_RAMP_STEP, n)

// REG_190
#define REG_OV_MSK_SHIFT                    0
#define REG_OV_MSK_MASK                     (0xFFFF << REG_OV_MSK_SHIFT)
#define REG_OV_MSK(n)                       BITFIELD_VAL(REG_OV_MSK, n)

// REG_191
#define REG_UV_MSK_SHIFT                    0
#define REG_UV_MSK_MASK                     (0xFFFF << REG_UV_MSK_SHIFT)
#define REG_UV_MSK(n)                       BITFIELD_VAL(REG_UV_MSK, n)

// REG_198
#define REG_RAMP_BIAS_FAST_MSK              (1 << 14)
#define REG_CLK_RAMP_DIV_SHIFT              9
#define REG_CLK_RAMP_DIV_MASK               (0x1F << REG_CLK_RAMP_DIV_SHIFT)
#define REG_CLK_RAMP_DIV(n)                 BITFIELD_VAL(REG_CLK_RAMP_DIV, n)
#define REG_RAMP_CLK_SEL_OSC                (1 << 8)
#define REG_CLK_RAMP_RC_GATE_EN             (1 << 7)
#define REG_CLK_RAMP_OSC_GATE_EN            (1 << 6)
#define REG_RAMP_CLK_TIME_SHIFT             2
#define REG_RAMP_CLK_TIME_MASK              (0xF << REG_RAMP_CLK_TIME_SHIFT)
#define REG_RAMP_CLK_TIME(n)                BITFIELD_VAL(REG_RAMP_CLK_TIME, n)
#define REG_RAMP_FAST_EN_DR                 (1 << 1)
#define REG_RAMP_FAST_EN                    (1 << 0)

// REG_199
#define REG_VBAT_HANDLE_SEL_PMU_OFF         (1 << 14)
#define REG_VBAT_HW_MODE                    (1 << 13)
#define REG_VBAT_INTR_CLR                   (1 << 12)
#define REG_VBAT_INTR_EN                    (1 << 11)
#define REG_VBAT_INTR_MSK                   (1 << 10)
#define REG_VBAT_LDO_OFF_DR                 (1 << 9)
#define REG_VBAT_LDO_OFF_REG                (1 << 8)
#define REG_VBAT_PMU_OFF_DR                 (1 << 7)
#define REG_VBAT_PMU_OFF_REG                (1 << 6)
#define REG_VBAT_REBOOT_CYCLE_SHIFT         2
#define REG_VBAT_REBOOT_CYCLE_MASK          (0xF << REG_VBAT_REBOOT_CYCLE_SHIFT)
#define REG_VBAT_REBOOT_CYCLE(n)            BITFIELD_VAL(REG_VBAT_REBOOT_CYCLE, n)
#define REG_VBAT_BREAK_TIME_SHIFT           0
#define REG_VBAT_BREAK_TIME_MASK            (0x3 << REG_VBAT_BREAK_TIME_SHIFT)
#define REG_VBAT_BREAK_TIME(n)              BITFIELD_VAL(REG_VBAT_BREAK_TIME, n)

// REG_19B
#define REG_VRTC_HANDLE_SEL_PMU_OFF         (1 << 14)
#define REG_VRTC_HW_MODE                    (1 << 13)
#define REG_VRTC_INTR_CLR                   (1 << 12)
#define REG_VRTC_INTR_EN                    (1 << 11)
#define REG_VRTC_INTR_MSK                   (1 << 10)
#define REG_VRTC_LDO_OFF_DR                 (1 << 9)
#define REG_VRTC_LDO_OFF_REG                (1 << 8)
#define REG_VRTC_PMU_OFF_DR                 (1 << 7)
#define REG_VRTC_PMU_OFF_REG                (1 << 6)
#define REG_VRTC_REBOOT_CYCLE_SHIFT         2
#define REG_VRTC_REBOOT_CYCLE_MASK          (0xF << REG_VRTC_REBOOT_CYCLE_SHIFT)
#define REG_VRTC_REBOOT_CYCLE(n)            BITFIELD_VAL(REG_VRTC_REBOOT_CYCLE, n)
#define REG_VRTC_BREAK_TIME_SHIFT           0
#define REG_VRTC_BREAK_TIME_MASK            (0x3 << REG_VRTC_BREAK_TIME_SHIFT)
#define REG_VRTC_BREAK_TIME(n)              BITFIELD_VAL(REG_VRTC_BREAK_TIME, n)

// REG_19D
#define REG_VDIG_CODEC_HANDLE_SEL_PMU_OFF   (1 << 14)
#define REG_VDIG_CODEC_HW_MODE              (1 << 13)
#define REG_VDIG_CODEC_INTR_CLR             (1 << 12)
#define REG_VDIG_CODEC_INTR_EN              (1 << 11)
#define REG_VDIG_CODEC_INTR_MSK             (1 << 10)
#define REG_VDIG_CODEC_LDO_OFF_DR           (1 << 9)
#define REG_VDIG_CODEC_LDO_OFF_REG          (1 << 8)
#define REG_VDIG_CODEC_PMU_OFF_DR           (1 << 7)
#define REG_VDIG_CODEC_PMU_OFF_REG          (1 << 6)
#define REG_VDIG_CODEC_REBOOT_CYCLE_SHIFT   2
#define REG_VDIG_CODEC_REBOOT_CYCLE_MASK    (0xF << REG_VDIG_CODEC_REBOOT_CYCLE_SHIFT)
#define REG_VDIG_CODEC_REBOOT_CYCLE(n)      BITFIELD_VAL(REG_VDIG_CODEC_REBOOT_CYCLE, n)
#define REG_VDIG_CODEC_BREAK_TIME_SHIFT     0
#define REG_VDIG_CODEC_BREAK_TIME_MASK      (0x3 << REG_VDIG_CODEC_BREAK_TIME_SHIFT)
#define REG_VDIG_CODEC_BREAK_TIME(n)        BITFIELD_VAL(REG_VDIG_CODEC_BREAK_TIME, n)

// REG_19F
#define REG_AVDD25_INF_HANDLE_SEL_PMU_OFF   (1 << 14)
#define REG_AVDD25_INF_HW_MODE              (1 << 13)
#define REG_AVDD25_INF_INTR_CLR             (1 << 12)
#define REG_AVDD25_INF_INTR_EN              (1 << 11)
#define REG_AVDD25_INF_INTR_MSK             (1 << 10)
#define REG_AVDD25_INF_LDO_OFF_DR           (1 << 9)
#define REG_AVDD25_INF_LDO_OFF_REG          (1 << 8)
#define REG_AVDD25_INF_PMU_OFF_DR           (1 << 7)
#define REG_AVDD25_INF_PMU_OFF_REG          (1 << 6)
#define REG_AVDD25_INF_REBOOT_CYCLE_SHIFT   2
#define REG_AVDD25_INF_REBOOT_CYCLE_MASK    (0xF << REG_AVDD25_INF_REBOOT_CYCLE_SHIFT)
#define REG_AVDD25_INF_REBOOT_CYCLE(n)      BITFIELD_VAL(REG_AVDD25_INF_REBOOT_CYCLE, n)
#define REG_AVDD25_INF_BREAK_TIME_SHIFT     0
#define REG_AVDD25_INF_BREAK_TIME_MASK      (0x3 << REG_AVDD25_INF_BREAK_TIME_SHIFT)
#define REG_AVDD25_INF_BREAK_TIME(n)        BITFIELD_VAL(REG_AVDD25_INF_BREAK_TIME, n)

// REG_1A1
#define REG_VMICA_HANDLE_SEL_PMU_OFF        (1 << 14)
#define REG_VMICA_HW_MODE                   (1 << 13)
#define REG_VMICA_INTR_CLR                  (1 << 12)
#define REG_VMICA_INTR_EN                   (1 << 11)
#define REG_VMICA_INTR_MSK                  (1 << 10)
#define REG_VMICA_LDO_OFF_DR                (1 << 9)
#define REG_VMICA_LDO_OFF_REG               (1 << 8)
#define REG_VMICA_PMU_OFF_DR                (1 << 7)
#define REG_VMICA_PMU_OFF_REG               (1 << 6)
#define REG_VMICA_REBOOT_CYCLE_SHIFT        2
#define REG_VMICA_REBOOT_CYCLE_MASK         (0xF << REG_VMICA_REBOOT_CYCLE_SHIFT)
#define REG_VMICA_REBOOT_CYCLE(n)           BITFIELD_VAL(REG_VMICA_REBOOT_CYCLE, n)
#define REG_VMICA_BREAK_TIME_SHIFT          0
#define REG_VMICA_BREAK_TIME_MASK           (0x3 << REG_VMICA_BREAK_TIME_SHIFT)
#define REG_VMICA_BREAK_TIME(n)             BITFIELD_VAL(REG_VMICA_BREAK_TIME, n)

// REG_1A3
#define REG_VMICB_HANDLE_SEL_PMU_OFF        (1 << 14)
#define REG_VMICB_HW_MODE                   (1 << 13)
#define REG_VMICB_INTR_CLR                  (1 << 12)
#define REG_VMICB_INTR_EN                   (1 << 11)
#define REG_VMICB_INTR_MSK                  (1 << 10)
#define REG_VMICB_LDO_OFF_DR                (1 << 9)
#define REG_VMICB_LDO_OFF_REG               (1 << 8)
#define REG_VMICB_PMU_OFF_DR                (1 << 7)
#define REG_VMICB_PMU_OFF_REG               (1 << 6)
#define REG_VMICB_REBOOT_CYCLE_SHIFT        2
#define REG_VMICB_REBOOT_CYCLE_MASK         (0xF << REG_VMICB_REBOOT_CYCLE_SHIFT)
#define REG_VMICB_REBOOT_CYCLE(n)           BITFIELD_VAL(REG_VMICB_REBOOT_CYCLE, n)
#define REG_VMICB_BREAK_TIME_SHIFT          0
#define REG_VMICB_BREAK_TIME_MASK           (0x3 << REG_VMICB_BREAK_TIME_SHIFT)
#define REG_VMICB_BREAK_TIME(n)             BITFIELD_VAL(REG_VMICB_BREAK_TIME, n)

// REG_1BF
#define REG_OTP_HW_MODE                     (1 << 13)
#define REG_OTP_INTR_CLR                    (1 << 12)
#define REG_OTP_INTR_EN                     (1 << 11)
#define REG_OTP_INTR_MSK                    (1 << 10)
#define REG_OTP_HANDLE_SEL_PMU_OFF          0

// REG_1C0
#define REG_OTP_PMU_OFF_DR                  (1 << 7)
#define REG_OTP_PMU_OFF_REG                 (1 << 6)
#define REG_OTP_REBOOT_CYCLE_SHIFT          2
#define REG_OTP_REBOOT_CYCLE_MASK           (0xF << REG_OTP_REBOOT_CYCLE_SHIFT)
#define REG_OTP_REBOOT_CYCLE(n)             BITFIELD_VAL(REG_OTP_REBOOT_CYCLE, n)
#define REG_OTP_BREAK_TIME_SHIFT            0
#define REG_OTP_BREAK_TIME_MASK             (0x3 << REG_OTP_BREAK_TIME_SHIFT)
#define REG_OTP_BREAK_TIME(n)               BITFIELD_VAL(REG_OTP_BREAK_TIME, n)

// REG_1C1
#define REG_OTP_REBOOT_TRIGGER              (1 << 1)
#define REG_OTP_REBOOT_TRIGGER_DR           (1 << 0)

// REG_1C2
#define REG_OTP_DET_TIME_SHIFT              10
#define REG_OTP_DET_TIME_MASK               (0x1F << REG_OTP_DET_TIME_SHIFT)
#define REG_OTP_DET_TIME(n)                 BITFIELD_VAL(REG_OTP_DET_TIME, n)
#define REG_OTP_DET_TRUBO_TIME_SHIFT        5
#define REG_OTP_DET_TRUBO_TIME_MASK         (0x1F << REG_OTP_DET_TRUBO_TIME_SHIFT)
#define REG_OTP_DET_TRUBO_TIME(n)           BITFIELD_VAL(REG_OTP_DET_TRUBO_TIME, n)
#define REG_OTP_ENDURE_TIME_SHIFT           0
#define REG_OTP_ENDURE_TIME_MASK            (0x1F << REG_OTP_ENDURE_TIME_SHIFT)
#define REG_OTP_ENDURE_TIME(n)              BITFIELD_VAL(REG_OTP_ENDURE_TIME, n)

// REG_1C3
#define REG_OTP_OSCP_FUNC_EN                (1 << 2)
#define REG_OTP_PROTEC_CYCLE_SHIFT          0
#define REG_OTP_PROTEC_CYCLE_MASK           (0x3 << REG_OTP_PROTEC_CYCLE_SHIFT)
#define REG_OTP_PROTEC_CYCLE(n)             BITFIELD_VAL(REG_OTP_PROTEC_CYCLE, n)

// PMU_EFUSE_PAGE_LDO_CALIB
#define PMU_EFUSE_BURST_THRESHOLD_CAL_SHIFT 10
#define PMU_EFUSE_BURST_THRESHOLD_CAL_MASK  (0x1F << PMU_EFUSE_BURST_THRESHOLD_CAL_SHIFT)
#define PMU_EFUSE_BURST_THRESHOLD_CAL(n)    BITFIELD_VAL(PMU_EFUSE_BURST_THRESHOLD_CAL, n)

// PMU_EFUSE_PAGE_RESERVED_22
#define PMU_EFUSE_FLASH_VDD_1V8             (1 << 0)

// PMU_EXT_EFUSE_PAGE_RESERVED_7
#define PMU_EXT_EFUSE_VOLTAGE_DETECT_EN     (1 << 0)
#define PMU_EXT_EFUSE_CURRENT_DETECT_EN     (1 << 1)

// ANA_REG_F6
#define REG_CODEC_LP_VCM                    (1 << 7)
#define REG_CODEC_EN_VCM                    (1 << 8)
#define REG_CODEC_VCM_EN_LPF                (1 << 9)

// ANA_REG_F9
#define REG_CODEC_VCM_LOW_VCM_SHIFT         0
#define REG_CODEC_VCM_LOW_VCM_MASK          (0xF << REG_CODEC_VCM_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_LOW_VCM(n)            BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM, n)
#define REG_CODEC_VCM_LOW_VCM_LPF_SHIFT     4
#define REG_CODEC_VCM_LOW_VCM_LPF_MASK      (0xF << REG_CODEC_VCM_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LPF(n)        BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LPF, n)
#define REG_CODEC_VCM_LOW_VCM_LP_SHIFT      8
#define REG_CODEC_VCM_LOW_VCM_LP_MASK       (0xF << REG_CODEC_VCM_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LP(n)         BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LP, n)

// PMU_ANA_REG_6D
#define REG_SEL_CAPSENOR_OSC                (1 << 0)
#define REG_CFG_CAPSENSOR_DIV_SHIFT         1
#define REG_CFG_CAPSENSOR_DIV_MASK          (0x7FF << REG_CFG_CAPSENSOR_DIV_SHIFT)
#define REG_CFG_CAPSENSOR_DIV(n)            BITFIELD_VAL(REG_CFG_CAPSENSOR_DIV, n)
#define REG_CLK_CAPSENSOR_GATE_EN           (1 << 12)
#define REG_CLK_32K_CAPSENSOR_GATE_EN       (1 << 13)
#define SOFT_RESETN_CAPSENSOR               (1 << 14)
#define SOFT_RESETN_CAPSENSOR_REG           (1 << 15)

// RF_C5
#define REG_BBPLL_DIV_PU                    (1 << 0)
#define REG_BBPLL_DIV_SWRC_SHIFT            1
#define REG_BBPLL_DIV_SWRC_MASK             (0x3 << REG_BBPLL_DIV_SWRC_SHIFT)
#define REG_BBPLL_DIV_SWRC(n)               BITFIELD_VAL(REG_BBPLL_DIV_SWRC, n)
#define REG_BBPLL_PSTDIV_SEL_DIGCLK         (1 << 3)
#define REG_PM_SEL_CLK                      (1 << 4)
#define REG_PM_SEL_PHASE_SHIFT              5
#define REG_PM_SEL_PHASE_MASK               (0x1F << REG_PM_SEL_PHASE_SHIFT)
#define REG_PM_SEL_PHASE(n)                 BITFIELD_VAL(REG_PM_SEL_PHASE, n)

// RF_F0
#define REG_BT_BBPLL_FREQ_EN                (1 << 0)

#define PMU_HW_UVP_VCORE_MV_MIN             240
#define PMU_HW_UVP_VCORE_MV_MAX             300
#define PMU_HW_UVP_VCORE_0_24V_RAW          0x0
#define PMU_HW_UVP_VCORE_0_3V_RAW           0x3

#ifndef EFUSE_BLOCK_NUM
#define EFUSE_BLOCK_NUM                     6
#endif

#define PMU_NTC_VOLT2TEMP_VTHREM_K          ((pmu_type == PMU_CHIP_TYPE_1809) ? 377 : 369)

enum PMU_REG_T {
    PMU_REG_METAL_ID            = EXTPMU_REG(0x00),
    PMU_REG_POWER_KEY_CFG       = EXTPMU_REG(0x02),
    PMU_REG_BIAS_CFG            = EXTPMU_REG(0x03),
    PMU_REG_SOFT_START_CFG      = EXTPMU_REG(0x04),
    PMU_REG_CHARGER_CFG         = EXTPMU_REG(0x05),
    PMU_REG_ANA_CFG             = EXTPMU_REG(0x07),
    PMU_REG_DIG_CFG             = EXTPMU_REG(0x08),
    PMU_REG_IO_CFG              = EXTPMU_REG(0x09),
    PMU_REG_MEM_CFG             = EXTPMU_REG(0x0A),
    PMU_REG_GP_CFG              = EXTPMU_REG(0x0B),
    PMU_REG_USB_CFG             = EXTPMU_REG(0x0C),
    PMU_REG_BAT2DIG_CFG         = EXTPMU_REG(0x0D),
    PMU_REG_HPPA_LDO_EN         = EXTPMU_REG(0x0E),
    PMU_REG_HPPA2CODEC_CFG      = EXTPMU_REG(0x0F),
    PMU_REG_CODEC_CFG           = EXTPMU_REG(0x10),
    PMU_REG_DCDC_DIG_EN         = EXTPMU_REG(0x15),
    PMU_REG_DCDC_ANA_CFG_16     = EXTPMU_REG(0x16),
    PMU_REG_DCDC_ANA_CFG_17     = EXTPMU_REG(0x17),
    PMU_REG_DCDC_ANA_CFG_18     = EXTPMU_REG(0x18),
    PMU_REG_DCDC_ANA_CFG_19     = EXTPMU_REG(0x19),
    PMU_REG_DCDC_HPPA_CFG_1A    = EXTPMU_REG(0x1A),
    PMU_REG_DCDC_HPPA_CFG_1B    = EXTPMU_REG(0x1B),
    PMU_REG_DCDC_HPPA_CFG_1C    = EXTPMU_REG(0x1C),
    PMU_REG_DCDC_HPPA_CFG_1D    = EXTPMU_REG(0x1D),
    PMU_REG_THERM               = EXTPMU_REG(0x1E),
    PMU_REG_SAR_STABLE_SEL      = EXTPMU_REG(0x1F),
    PMU_REG_GPADC_EN_DR         = EXTPMU_REG(0x20),
    PMU_REG_PWR_SEL             = EXTPMU_REG(0x21),
    PMU_REG_SAR_VREF            = EXTPMU_REG(0x25),
    PMU_REG_INT_MASK            = EXTPMU_REG(0x26),
    PMU_REG_INT_EN              = EXTPMU_REG(0x27),
    PMU_REG_CAP_MSK             = EXTPMU_REG(0x2C),
    PMU_REG_RTC_LOAD_LOW        = EXTPMU_REG(0x2D),
    PMU_REG_CAP_INTR_PMU        = EXTPMU_REG(0x2D),
    PMU_REG_RTC_LOAD_HIGH       = EXTPMU_REG(0x2E),
    PMU_REG_RTC_MATCH0_LOW      = EXTPMU_REG(0x2F),
    PMU_REG_RTC_MATCH0_HIGH     = EXTPMU_REG(0x30),
    PMU_REG_RTC_MATCH1_LOW      = EXTPMU_REG(0x31),
    PMU_REG_RTC_MATCH1_HIGH     = EXTPMU_REG(0x32),
    PMU_REG_DCDC_DIG_CFG_33     = EXTPMU_REG(0x33),
    PMU_REG_DCDC_DIG_CFG_34     = EXTPMU_REG(0x34),
    PMU_REG_DCDC_DIG_CFG_35     = EXTPMU_REG(0x35),
    PMU_REG_DCDC_DIG_CFG_36     = EXTPMU_REG(0x36),
    PMU_REG_RTC_DIV_1HZ         = EXTPMU_REG(0x37),
    PMU_REG_MIC_BIAS_E          = EXTPMU_REG(0x38),
    PMU_REG_MIC_LDO_EN          = EXTPMU_REG(0x39),
    PMU_REG_LED_CFG             = EXTPMU_REG(0x3A),
    PMU_REG_MIC_BIAS_A          = EXTPMU_REG(0x3B),
    PMU_REG_MIC_BIAS_B          = EXTPMU_REG(0x3C),
    PMU_REG_MIC_BIAS_C          = EXTPMU_REG(0x3D),
    PMU_REG_EFUSE_CTRL          = EXTPMU_REG(0x3F),
    PMU_REG_EFUSE_SEL           = EXTPMU_REG(0x40),
    PMU_REG_DCDC1_OFFSET        = EXTPMU_REG(0x41),
    PMU_REG_DCDC3_OFFSET        = EXTPMU_REG(0x42),
    PMU_REG_BUCK2ANA_CFG        = EXTPMU_REG(0x43),
    PMU_REG_SLEEP_CFG           = EXTPMU_REG(0x44),
    PMU_REG_DCDC_DIG_VOLT       = EXTPMU_REG(0x46),
    PMU_REG_DCDC_ANA_VOLT       = EXTPMU_REG(0x47),
    PMU_REG_DCDC_HPPA_VOLT      = EXTPMU_REG(0x48),
    PMU_REG_DCDC_ANA_EN         = EXTPMU_REG(0x4A),
    PMU_REG_DCDC_HPPA_EN        = EXTPMU_REG(0x4B),
    PMU_REG_PU_VSYS_DIV         = EXTPMU_REG(0x4C),
    PMU_REG_HW_DET_CFG          = EXTPMU_REG(0x4D),
    PMU_REG_POWER_OFF           = EXTPMU_REG(0x4F),
    PMU_REG_INT_STATUS          = EXTPMU_REG(0x52),
    PMU_REG_INT_MSKED_STATUS    = EXTPMU_REG(0x53),
    PMU_REG_RTC_VAL_LOW         = EXTPMU_REG(0x54),
    PMU_REG_RTC_VAL_HIGH        = EXTPMU_REG(0x55),
    PMU_REG_CHARGER_STATUS      = EXTPMU_REG(0x5E),
    PMU_REG_PWM_CLK_CFG         = EXTPMU_REG(0x10E),

#define PMU_REG_INT_CLR         ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_INT_CLR         : PMU1806_REG_INT_CLR)
#define PMU_REG_DCDC_RAMP_EN    ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_DCDC_RAMP_EN    : PMU1806_REG_DCDC_RAMP_EN)
#define PMU_REG_PWM2_TOGGLE     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_PWM2_TOGGLE     : PMU1806_REG_PWM2_TOGGLE)
#define PMU_REG_PWM3_TOGGLE     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_PWM3_TOGGLE     : PMU1806_REG_PWM3_TOGGLE)
#define PMU_REG_PWM2_ST1        ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_PWM2_ST1        : PMU1806_REG_PWM2_ST1)
#define PMU_REG_PWM3_ST1        ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_PWM3_ST1        : PMU1806_REG_PWM3_ST1)
#define PMU_REG_SUBCNT_DATA     ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_SUBCNT_DATA     : PMU1806_REG_SUBCNT_DATA)
#define PMU_REG_PWM_BR_EN       ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_PWM_BR_EN       : PMU1806_REG_PWM_BR_EN)
#define PMU_REG_PWM_EN          ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_PWM_EN          : PMU1806_REG_PWM_EN)
#define PMU_REG_WDT_TIMER       ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_WDT_TIMER       : PMU1806_REG_WDT_TIMER)
#define PMU_REG_WDT_CFG         ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_WDT_CFG         : PMU1806_REG_WDT_CFG)
#define PMU_REG_EFUSE_VAL_START ((pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) ? PMU1803_REG_EFUSE_VAL_START : PMU1806_REG_EFUSE_VAL_START)

    PMU1805_REG_LP_BIAS_SEL_LDO = EXTPMU_REG(0x121),

    PMU1806_REG_VTOI_CFG        = EXTPMU_REG(0x50),
    PMU1806_REG_INT_CLR         = EXTPMU_REG(0x52),
    PMU1806_REG_DCDC_RAMP_EN    = EXTPMU_REG(0x101),
    PMU1806_REG_PWM2_TOGGLE     = EXTPMU_REG(0x103),
    PMU1806_REG_PWM3_TOGGLE     = EXTPMU_REG(0x104),
    PMU1806_REG_PWM2_ST1        = EXTPMU_REG(0x105),
    PMU1806_REG_PWM3_ST1        = EXTPMU_REG(0x106),
    PMU1806_REG_SUBCNT_DATA     = EXTPMU_REG(0x107),
    PMU1806_REG_PWM_BR_EN       = EXTPMU_REG(0x108),
    PMU1806_REG_PWM_EN          = EXTPMU_REG(0x109),
    PMU1806_REG_WDT_TIMER       = EXTPMU_REG(0x10B),
    PMU1806_REG_WDT_CFG         = EXTPMU_REG(0x10C),
    PMU1806_REG_EFUSE_VAL_START = EXTPMU_REG(0),
    PMU1806_REG_PMIC_UART1_CFG  = EXTPMU_REG(0x110),
    PMU1806_REG_PMIC_UART2_CFG  = EXTPMU_REG(0x111),
    PMU1806_REG_DCDC1_EDGE_CON  = EXTPMU_REG(0x11C),
    PMU1806_REG_DCDC2_EDGE_CON  = EXTPMU_REG(0x11D),

    PMU1803_REG_INT_CLR         = EXTPMU_REG(0x51),
    PMU1803_REG_DCDC_RAMP_EN    = EXTPMU_REG(0x60),
    PMU1803_REG_PWM2_TOGGLE     = EXTPMU_REG(0x62),
    PMU1803_REG_PWM3_TOGGLE     = EXTPMU_REG(0x63),
    PMU1803_REG_PWM2_ST1        = EXTPMU_REG(0x64),
    PMU1803_REG_PWM3_ST1        = EXTPMU_REG(0x65),
    PMU1803_REG_SUBCNT_DATA     = EXTPMU_REG(0x66),
    PMU1803_REG_PWM_BR_EN       = EXTPMU_REG(0x67),
    PMU1803_REG_PWM_EN          = EXTPMU_REG(0x68),
    PMU1803_REG_WDT_TIMER       = EXTPMU_REG(0x6A),
    PMU1803_REG_WDT_CFG         = EXTPMU_REG(0x6B),
    PMU1803_REG_PMIC_UART_CFG   = EXTPMU_REG(0x6F),
    PMU1803_REG_EFUSE_VAL_START = EXTPMU_REG(0xA0),

    PMU1806_REG_RESETN_MODE     = EXTPMU_REG(0x11),
    PMU1806_REG_AVDD25          = EXTPMU_REG(0x25),
    PMU1806_REG_PU_VSYS_DIV     = EXTPMU_REG(0x4C),
    PMU1806_REG_XO_32K          = EXTPMU_REG(0x61),
    PMU1806_REG_XO_32K_ISEL     = EXTPMU_REG(0x62),
    PMU1806_REG_1P8_SWITCH      = EXTPMU_REG(0x63),
    PMU1806_REG_ID_DET          = EXTPMU_REG(0x65),
    PMU1806_REG_LDO_VMEM2       = EXTPMU_REG(0x66),
    PMU1806_REG_LDO_DIG_L2_CFG  = EXTPMU_REG(0x69),
    PMU1806_REG_VUSB_SWITCH     = EXTPMU_REG(0x6B),
    PMU1806_REG_CODEC_VCM       = EXTPMU_REG(0x6C),
    PMU1806_REG_XO_32K_CAP      = EXTPMU_REG(0x6F),
    PMU1806_REG_SLEEP_CFG_RC    = EXTPMU_REG(0x70),
    PMU1806_REG_DIG_L_VOLT_RC   = EXTPMU_REG(0x72),
    PMU1806_REG_USB_VOLT_RC     = EXTPMU_REG(0x73),
    PMU1806_REG_DIG_VOLT_RC     = EXTPMU_REG(0x74),
    PMU1806_REG_ANA_VOLT_RC     = EXTPMU_REG(0x75),
    PMU1806_REG_HPPA_VOLT_RC    = EXTPMU_REG(0x76),

    PMU_REG_RESERVED_PMU        = EXTPMU_REG(0x102),
    PMU1805_REG_USB_RET_LDO     = EXTPMU_REG(0x10F),
    REG_LDO_DIG_L_CFG           = EXTPMU_REG(0x115),
    PMU_REG_PU_LDO              = EXTPMU_REG(0x116),
    PMU_REG_LDO_VCORE_L         = EXTPMU_REG(0x11A),
    PMU1806_REG_MAIN_LPO        = EXTPMU_REG(0x11B),
    PMU1806_REG_SAR_MIC_BIAS_LP = EXTPMU_REG(0x122),
    PMU_REG_PWM4_EN             = EXTPMU_REG(0x123),
    PMU1805_REG_PWM4_TOGGLE     = EXTPMU_REG(0x124),
    PMU1805_REG_PWM4_ST1        = EXTPMU_REG(0x125),
    PMU1805_REG_SUBCNT_DATA     = EXTPMU_REG(0x126),
    PMU_REG_GPIO3_CONFIG        = EXTPMU_REG(0x127),
    PMU1805_REG_RTC_EN_FLAG     = EXTPMU_REG(0x128),

    PMU1806_REG_GPIO_IRQ_CONFIG = EXTPMU_REG(0x135),

    PMU1806_REG_RESERVED_METAL_1 = EXTPMU_REG(0x102),
    PMU1806_REG_METAL_1          = EXTPMU_REG(0x114),
    PMU1806_REG_LP_BIAS_SEL_LDO  = EXTPMU_REG(0x120),
    PMU1806_REG_GPIO_DRV_SEL     = EXTPMU_REG(0x129),
    PMU1806_REG_GPIO_P0_PULL_SEL = EXTPMU_REG(0x12a),
    PMU1806_REG_GPIO_P1_PULL_SEL = EXTPMU_REG(0x12f),
    PMU1806_REG_GPIO_P2_PULL_SEL = EXTPMU_REG(0x132),
    PMU1806_REG_GPIO_P3_PULL_SEL = EXTPMU_REG(0x136),
    PMU1806_REG_GPIO_P4_PULL_SEL = EXTPMU_REG(0x139),
    PMU1806_REG_GPIO_P5_PULL_SEL = EXTPMU_REG(0x13d),
    PMU1806_REG_GPIO_P6_PULL_SEL = EXTPMU_REG(0x140),
    PMU1806_REG_GPIO_P7_PULL_SEL = EXTPMU_REG(0x144),

    PMU1806_REG_GPIO_P0_P1_RX_EN = EXTPMU_REG(0x12C),
    PMU1806_REG_GPIO_P2_P3_RX_EN = EXTPMU_REG(0x134),
    PMU1806_REG_GPIO_P4_P5_RX_EN = EXTPMU_REG(0x13B),
    PMU1806_REG_GPIO_P6_P7_RX_EN = EXTPMU_REG(0x142),

    PMU1806_REG_GPIO_P0_P1_DATA_IN = EXTPMU_REG(0x145),
    PMU1806_REG_GPIO_P2_P3_DATA_IN = EXTPMU_REG(0x146),
    PMU1806_REG_GPIO_P4_P5_DATA_IN = EXTPMU_REG(0x147),
    PMU1806_REG_GPIO_P6_P7_DATA_IN = EXTPMU_REG(0x148),

    PMU1806_REG_GPIO_P0_DATA_OUT = EXTPMU_REG(0x12B),
    PMU1806_REG_GPIO_P1_DATA_OUT = EXTPMU_REG(0x130),
    PMU1806_REG_GPIO_P2_DATA_OUT = EXTPMU_REG(0x133),
    PMU1806_REG_GPIO_P3_DATA_OUT = EXTPMU_REG(0x137),
    PMU1806_REG_GPIO_P4_DATA_OUT = EXTPMU_REG(0x13A),
    PMU1806_REG_GPIO_P5_DATA_OUT = EXTPMU_REG(0x13E),
    PMU1806_REG_GPIO_P6_DATA_OUT = EXTPMU_REG(0x141),
    PMU1806_REG_GPIO_P7_DATA_OUT = EXTPMU_REG(0x144),

    PMU1806_REG_GPIO_P0_P1_INTR_EN = EXTPMU_REG(0x149),
    PMU1806_REG_GPIO_P2_P3_INTR_EN = EXTPMU_REG(0x14a),
    PMU1806_REG_GPIO_P4_P5_INTR_EN = EXTPMU_REG(0x14b),
    PMU1806_REG_GPIO_P6_P7_INTR_EN = EXTPMU_REG(0x14c),

    PMU1806_REG_GPIO_P0_P1_EDGE_SEL = EXTPMU_REG(0x14d),
    PMU1806_REG_GPIO_P2_P3_EDGE_SEL = EXTPMU_REG(0x14e),
    PMU1806_REG_GPIO_P4_P5_EDGE_SEL = EXTPMU_REG(0x14f),
    PMU1806_REG_GPIO_P6_P7_EDGE_SEL = EXTPMU_REG(0x150),

    PMU1806_REG_GPIO_P0_P1_LVL_INTR_SEL = EXTPMU_REG(0x151),
    PMU1806_REG_GPIO_P2_P3_LVL_INTR_SEL = EXTPMU_REG(0x152),
    PMU1806_REG_GPIO_P4_P5_LVL_INTR_SEL = EXTPMU_REG(0x153),
    PMU1806_REG_GPIO_P6_P7_LVL_INTR_SEL = EXTPMU_REG(0x154),

    PMU1806_REG_GPIO_P0_P1_NEDG_INTR_SEL = EXTPMU_REG(0x131),
    PMU1806_REG_GPIO_P2_P3_NEDG_INTR_SEL = EXTPMU_REG(0x138),
    PMU1806_REG_GPIO_P4_P5_NEDG_INTR_SEL = EXTPMU_REG(0x13f),
    PMU1806_REG_GPIO_P6_P7_NEDG_INTR_SEL = EXTPMU_REG(0x143),

    PMU1806_REG_GPIO_P0_P1_POS_INTR_SEL = EXTPMU_REG(0x155),
    PMU1806_REG_GPIO_P2_P3_POS_INTR_SEL = EXTPMU_REG(0x156),
    PMU1806_REG_GPIO_P4_P5_POS_INTR_SEL = EXTPMU_REG(0x157),
    PMU1806_REG_GPIO_P6_P7_POS_INTR_SEL = EXTPMU_REG(0x158),

    PMU1806_REG_GPIO_P0_P1_DB_BYPASS = EXTPMU_REG(0x159),
    PMU1806_REG_GPIO_P2_P3_DB_BYPASS = EXTPMU_REG(0x15a),
    PMU1806_REG_GPIO_P4_P5_DB_BYPASS = EXTPMU_REG(0x15b),
    PMU1806_REG_GPIO_P6_P7_DB_BYPASS = EXTPMU_REG(0x15c),

    PMU1806_REG_GPIO_P0_P1_DB_TARGET = EXTPMU_REG(0x15d),
    PMU1806_REG_GPIO_P2_P3_DB_TARGET = EXTPMU_REG(0x15e),
    PMU1806_REG_GPIO_P4_P5_DB_TARGET = EXTPMU_REG(0x15f),
    PMU1806_REG_GPIO_P6_P7_DB_TARGET = EXTPMU_REG(0x160),

    PMU1806_REG_GPIO_P0_P1_INTR_CLR = EXTPMU_REG(0x161),
    PMU1806_REG_GPIO_P2_P3_INTR_CLR = EXTPMU_REG(0x162),
    PMU1806_REG_GPIO_P4_P5_INTR_CLR = EXTPMU_REG(0x163),
    PMU1806_REG_GPIO_P6_P7_INTR_CLR = EXTPMU_REG(0x164),

    PMU1806_REG_GPIO_P0_P1_NEDG_INTR_CLR = EXTPMU_REG(0x165),
    PMU1806_REG_GPIO_P2_P3_NEDG_INTR_CLR = EXTPMU_REG(0x166),
    PMU1806_REG_GPIO_P4_P5_NEDG_INTR_CLR = EXTPMU_REG(0x167),
    PMU1806_REG_GPIO_P6_P7_NEDG_INTR_CLR = EXTPMU_REG(0x168),

    PMU1806_REG_GPIO_P0_P1_INTR_MSKED = EXTPMU_REG(0x169),
    PMU1806_REG_GPIO_P2_P3_INTR_MSKED = EXTPMU_REG(0x16a),
    PMU1806_REG_GPIO_P4_P5_INTR_MSKED = EXTPMU_REG(0x16b),
    PMU1806_REG_GPIO_P6_P7_INTR_MSKED = EXTPMU_REG(0x16c),

    PMU1806_REG_GPIO_P0_P1_NEDG_INTR_MSKED = EXTPMU_REG(0x16d),
    PMU1806_REG_GPIO_P2_P3_NEDG_INTR_MSKED = EXTPMU_REG(0x16e),
    PMU1806_REG_GPIO_P4_P5_NEDG_INTR_MSKED = EXTPMU_REG(0x16f),
    PMU1806_REG_GPIO_P6_P7_NEDG_INTR_MSKED = EXTPMU_REG(0x170),

    PMU1806_REG_GPIO_P0_P1_INTR_MSK = EXTPMU_REG(0x171),
    PMU1806_REG_GPIO_P2_P3_INTR_MSK = EXTPMU_REG(0x172),
    PMU1806_REG_GPIO_P4_P5_INTR_MSK = EXTPMU_REG(0x173),
    PMU1806_REG_GPIO_P6_P7_INTR_MSK = EXTPMU_REG(0x174),

    PMU1806_REG_GPIO_P0_P1_NEDG_INTR_MSK = EXTPMU_REG(0x175),
    PMU1806_REG_GPIO_P2_P3_NEDG_INTR_MSK = EXTPMU_REG(0x176),
    PMU1806_REG_GPIO_P4_P5_NEDG_INTR_MSK = EXTPMU_REG(0x177),
    PMU1806_REG_GPIO_P6_P7_NEDG_INTR_MSK = EXTPMU_REG(0x178),

    PMU1806_REG_RTC_MATCH_MIN_SEC   = EXTPMU_REG(0x2F),
    PMU1806_REG_RTC_MATCH_HOURS     = EXTPMU_REG(0x30),
    PMU1806_REG_RTC_MATCH_DATAS     = EXTPMU_REG(0x31),
    PMU1806_REG_RTC_MATCH_YEARS     = EXTPMU_REG(0x32),

    PMU1806_REG_RTC_LOAD_MIN_SEC_EN = EXTPMU_REG(0x179),
    PMU1806_REG_RTC_LOAD_HOURS      = EXTPMU_REG(0x17A),
    PMU1806_REG_RTC_LOAD_DATAS_EN   = EXTPMU_REG(0x17B),
    PMU1806_REG_RTC_LOAD_YEARS      = EXTPMU_REG(0x17C),

    PMU1806_REG_RTC_GET_MIN_SEC     = EXTPMU_REG(0x17D),
    PMU1806_REG_RTC_GET_HOURS       = EXTPMU_REG(0x17E),
    PMU1806_REG_RTC_GET_DATAS       = EXTPMU_REG(0x17F),
    PMU1806_REG_RTC_GET_YEARS       = EXTPMU_REG(0x180),

    PMU1808_REG_LDO_ON_SOURCE       = EXTPMU_REG(0x50),
    PMU1808_REG_ABORT_SOURCE        = EXTPMU_REG(0x78),
    PMU1808_REG_VIO_CFG_BB          = EXTPMU_REG(0xBB),
    PMU1808_REG_VUSB_RES_SEL_FINE   = EXTPMU_REG(0xBC),
    PMU1808_REG_BE                  = EXTPMU_REG(0xBE),
    PMU1808_REG_RTC_CLK_CFG         = EXTPMU_REG(0xBF),

    PMU1809_REG_INTR_MSK            = EXTPMU_REG(0x2C),
    PMU1809_REG_INTR_MSKED          = EXTPMU_REG(0x2D),
    PMU1809_REG_LDO_VMEM2_CFG       = EXTPMU_REG(0x66),
    PMU1809_REG_LDO_VMEM2_VOLT      = EXTPMU_REG(0x10),
    PMU1809_REG_LDO_VMEM3_CFG       = EXTPMU_REG(0x67),
    PMU1809_REG_LDO_VMEM3_VOLT      = EXTPMU_REG(0x68),
    PMU1809_REG_MIC_BIASA_VCORE_CFG = EXTPMU_REG(0x74),
    PMU1809_REG_MIC_BIASB_VANA_CFG  = EXTPMU_REG(0x75),
    PMU1809_REG_BUCK_VCORE_IS_GAIN  = EXTPMU_REG(0x7B),
    PMU1809_REG_BUCK_VANA_IS_GAIN   = EXTPMU_REG(0x7C),
    PMU1809_REG_BUCK_VHPPA_IS_GAIN  = EXTPMU_REG(0x7D),
    PMU1809_REG_LDO_VIO_VOLT        = EXTPMU_REG(0x88),
    PMU1809_REG_LDO_VIO_LP_VOLT     = EXTPMU_REG(0x89),
    PMU1809_REG_LDO_VUSB_VOLT       = EXTPMU_REG(0x8A),
    PMU1809_REG_LDO_VUSB_LP_VOLT    = EXTPMU_REG(0x8B),
    PMU1809_REG_VBG_VREF_CFG        = EXTPMU_REG(0x94),
    PMU1809_REG_RESERVED_ANA        = EXTPMU_REG(0x95),
    PMU1809_REG_ABORT_INTR_96       = EXTPMU_REG(0x96),
    PMU1809_REG_ABORT_INTR_97       = EXTPMU_REG(0x97),
    PMU1809_REG_ABORT_SOURCE_98     = EXTPMU_REG(0x98),
    PMU1809_REG_ABORT_SOURCE_99     = EXTPMU_REG(0x99),
    PMU1809_REG_ABORT_SOURCE_9A     = EXTPMU_REG(0x9A),
    PMU1809_REG_ABORT_SOURCE_9B     = EXTPMU_REG(0x9B),
    PMU1809_REG_ABORT_SOURCE_9C     = EXTPMU_REG(0x9C),
    PMU1809_REG_ABORT_SOURCE_9D     = EXTPMU_REG(0x9D),
    PMU1809_REG_ABORT_SOURCE_9E     = EXTPMU_REG(0x9E),
    PMU1809_REG_ABORT_SOURCE_9F     = EXTPMU_REG(0x9F),
    PMU1809_REG_ABORT_SOURCE_A0     = EXTPMU_REG(0xA0),
    PMU1809_REG_ABORT_SOURCE_A1     = EXTPMU_REG(0xA1),
    PMU1809_REG_LP_BIAS_CFG         = EXTPMU_REG(0x11F),
    PMU1809_REG_BUCK_VHPPA_CFG_12D  = EXTPMU_REG(0x12D),
    PMU1809_REG_BUCK_VCORE_CFG_137  = EXTPMU_REG(0x137),
    PMU1809_REG_BUCK_VANA_CFG_131   = EXTPMU_REG(0x131),
    PMU1809_REG_BUCK_VHPPA_CFG_12A  = EXTPMU_REG(0x12A),
    PMU1809_REG_BUCK_VCORE_CFG_136  = EXTPMU_REG(0x136),
    PMU1809_REG_BUCK_VANA_CFG_130   = EXTPMU_REG(0x130),
    PMU1809_REG_BUCK_VHPPA_CFG_129  = EXTPMU_REG(0x129),
    PMU1809_REG_BUCK_VCORE_CFG_13A  = EXTPMU_REG(0x13A),
    PMU1809_REG_BUCK_VCORE_CFG_13B  = EXTPMU_REG(0x13B),
    PMU1809_REG_BUCK_VANA_CFG_134   = EXTPMU_REG(0x134),
    PMU1809_REG_BUCK_VANA_CFG_135   = EXTPMU_REG(0x135),
    PMU1809_REG_VBAT2VCORE_PP_CFG   = EXTPMU_REG(0x141), // power_protect
    PMU1809_REG_BUCK_VCORE_PP_CFG   = EXTPMU_REG(0x169),
    PMU1809_REG_BUCK_VCORE_RAMP_CFG = EXTPMU_REG(0x174),
    PMU1809_REG_BUCK_VANA_RAMP_CFG  = EXTPMU_REG(0x175),
    PMU1809_REG_BUCK_VHPPA_RAMP_CFG = EXTPMU_REG(0x176),
    PMU1809_REG_RTC_CFG_178         = EXTPMU_REG(0x178),
    PMU1809_REG_PP_OV_MSK           = EXTPMU_REG(0x190),
    PMU1809_REG_PP_UV_MSK           = EXTPMU_REG(0x191),
    PMU1809_REG_PP_OV_UV_EN         = EXTPMU_REG(0x192),
    PMU1809_REG_NOT_RESET           = EXTPMU_REG(0x197),
    PMU1809_REG_FAST_RAMP_CFG       = EXTPMU_REG(0x198),
    PMU1809_REG_VBAT_PP_CFG         = EXTPMU_REG(0x199),
    PMU1809_REG_VRTC_PP_CFG         = EXTPMU_REG(0x19B),
    PMU1809_REG_DIG_CODEC_PP_CFG    = EXTPMU_REG(0x19D),
    PMU1809_REG_AVDD25_PP_CFG       = EXTPMU_REG(0x19F),
    PMU1809_REG_VMICA_PP_CFG        = EXTPMU_REG(0x1A1),
    PMU1809_REG_VMICB_PP_CFG        = EXTPMU_REG(0x1A3),
    PMU1809_REG_EFUSE_VAL_START     = EXTPMU_REG(0x1B4),
    PMU1809_REG_BOOT_SOURCE         = EXTPMU_REG(0x1BE),
    PMU1809_REG_OTP_PP_HW_CFG       = EXTPMU_REG(0x1BF),
    PMU1809_REG_OTP_PP_CFG          = EXTPMU_REG(0x1C3),

    PMU_REG_MODULE_START        = PMU_REG_ANA_CFG,

    PMU_REG_ANA_00              = ANA_REG(0x00),
    PMU_REG_ANA_F6              = ANA_REG(0xF6),
    PMU_REG_ANA_F9              = ANA_REG(0xF9),

    PMU_REG_1809_ANA_00         = PMU_ANA_REG(0x00),
    PMU_REG_1809_ANA_6D         = PMU_ANA_REG(0x6D),

    PMU_REG_RF_00               = RF_REG(0x00),
    PMU_REG_RF_73               = RF_REG(0x73),
    PMU_REG_RF_AB               = RF_REG(0xAB),
    PMU_REG_RF_AD               = RF_REG(0xAD),
    PMU_REG_RF_AE               = RF_REG(0xAE),
    PMU_REG_RF_C4               = RF_REG(0xC4),
    PMU_REG_RF_C5               = RF_REG(0xC5),
    PMU_REG_RF_EC               = RF_REG(0xEC),
    PMU_REG_RF_ED               = RF_REG(0xED),
    PMU_REG_RF_EE               = RF_REG(0xEE),
    PMU_REG_RF_F0               = RF_REG(0xF0),
    PMU_REG_RF_334              = RF_REG(0x334),
    PMU_REG_RF_335              = RF_REG(0x335),
    PMU_REG_RF_338              = RF_REG(0x338),
    PMU_REG_RF_33A              = RF_REG(0x33A),
    PMU_REG_RF_340              = RF_REG(0x340),
    PMU_REG_RF_341              = RF_REG(0x341),
    PMU_REG_RF_342              = RF_REG(0x342),
};

enum PMU_VCORE_REQ_T {
    PMU_VCORE_FLASH_WRITE_ENABLED   = (1 << 0),
    PMU_VCORE_FLASH_FREQ_HIGH       = (1 << 1),
    PMU_VCORE_USB_HS_ENABLED        = (1 << 2),
    PMU_VCORE_SYS_FREQ_MEDIUM_LOW   = (1 << 3),
    PMU_VCORE_SYS_FREQ_MEDIUM       = (1 << 4),
    PMU_VCORE_SYS_FREQ_HIGH         = (1 << 5),
    PMU_VCORE_RS_FREQ_HIGH          = (1 << 6),
    PMU_VCORE_RS_ADC_FREQ_HIGH      = (1 << 7),
    PMU_VCORE_IIR_ENABLE            = (1 << 8),
    PMU_VCORE_IIR_FREQ_HIGH         = (1 << 9),
    PMU_VCORE_IIR_EQ_ENABLE         = (1 << 10),
    PMU_VCORE_IIR_EQ_FREQ_HIGH      = (1 << 11),
    PMU_VCORE_FIR_ENABLE            = (1 << 12),
    PMU_VCORE_FIR_FREQ_HIGH         = (1 << 13),
    PMU_VCORE_PSRAM_FREQ_HIGH       = (1 << 14),
    PMU_VCORE_AXI_FREQ_MEDIUM_LOW   = (1 << 15),
    PMU_VCORE_AXI_FREQ_MEDIUM       = (1 << 16),
    PMU_VCORE_AXI_FREQ_HIGH         = (1 << 17),
    PMU_VCORE_PMU_RESET             = (1 << 18),
    PMU_VCORE_HW_UVP_ENABLED        = (1 << 19),
};

union BOOT_SETTINGS_T {
    struct {
        unsigned short usb_dld_dis      :1;
        unsigned short uart_dld_en      :1;
        unsigned short trace_en         :1;
        unsigned short pll_dis          :1;
        unsigned short uart_baud_div2   :1;
        unsigned short sec_freq_div2    :1;
        unsigned short crystal_freq     :2;
        unsigned short timeout_div      :2;
        unsigned short uart_connected   :1;
        unsigned short uart_1p8v        :1;
        unsigned short sec_eng_dis      :1;
        unsigned short sec_boot_struct  :1;
        unsigned short sec_check_ver    :1;
        unsigned short reserved         :1;
    };
    unsigned short reg;
};

#ifndef NO_EXT_PMU

enum PMU_MODULE_T {
    PMU_ANA,
    PMU_DIG,
    PMU_IO,
    PMU_MEM,
    PMU_GP,
    PMU_USB,
    PMU_BAT2DIG,
    PMU_HPPA2CODEC,
    PMU_CODEC,
    PMU_BUCK2ANA,
    PMU_MEM2,
    PMU_MEM3,

    PMU_MODULE_QTY,
};

struct PMU_MODULE_CFG_T {
    unsigned short pu_dr;
    unsigned short pu;
    unsigned short lp_en_dr;
    unsigned short lp_en;
    unsigned short pu_dsleep;
    unsigned short vbit_dsleep_mask;
    unsigned short vbit_dsleep_shift;
    unsigned short vbit_normal_mask;
    unsigned short vbit_normal_shift;
};

#define PMU_MOD_CFG_VAL(m)              { \
    REG_PU_LDO_V##m##_DR, REG_PU_LDO_V##m##_REG, \
    LP_EN_V##m##_LDO_DR, LP_EN_V##m##_LDO_REG, \
    REG_PU_LDO_V##m##_DSLEEP, \
    LDO_V##m##_VBIT_DSLEEP_MASK, LDO_V##m##_VBIT_DSLEEP_SHIFT, \
    LDO_V##m##_VBIT_NORMAL_MASK, LDO_V##m##_VBIT_NORMAL_SHIFT }

static struct PMU_MODULE_CFG_T pmu_module_cfg[PMU_MODULE_QTY];

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE bool vcodec_off =
#ifdef VCODEC_OFF
    true;
#else
    false;
#endif
static uint8_t ana_act_dcdc = 0;

#define PMU_GPIO_PIN_GROUP_NUM                   ((HAL_GPIO_PIN_PMU_NUM - HAL_GPIO_PIN_PMU_P0_0 + 7) / 8 / 2)
// 24m/4/(0x0a+2)=500k
// 32k bypass(no div)
#define PMU_GPIO_CLK_DIV_NORMAL                  (0x0a)
// 1s/500k=2us, 2*0x80=256us
#define PMU_GPIO_DEBOUNCE_TARGET_24M             (0x80)
// 1s/32k=31.25us, 31.25*8=250us
#define PMU_GPIO_DEBOUNCE_TARGET_32K             (0x8)

static OPT_TYPE POSSIBLY_UNUSED uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);
static OPT_TYPE POSSIBLY_UNUSED uint16_t vhppa_mv = (uint16_t)(VHPPA_VOLT * 1000);
#ifdef PMU_VMEM_PWR_SUPPLY_IO
static OPT_TYPE POSSIBLY_UNUSED uint16_t vmem_mv = (uint16_t)(VMEM_VOLT * 1000);
static POSSIBLY_UNUSED bool BOOT_BSS_LOC pmu_split_vmem_buck3_valid;
#endif

int pmu_ext_get_efuse(enum PMU_EXT_EFUSE_PAGE_T page, unsigned short *efuse);

static enum PMU_CHIP_TYPE_T BOOT_BSS_LOC pmu_type;

static enum HAL_CHIP_METAL_ID_T POSSIBLY_UNUSED BOOT_BSS_LOC pmu_metal_id;

static enum PMU_POWER_MODE_T BOOT_BSS_LOC pmu_power_mode;

static enum PMU_VCORE_REQ_T BOOT_BSS_LOC pmu_vcore_req;

#if defined(PMU_GPIO_CLK_SOURCE_24M) && defined(PMU_GPIO_CLK_SOURCE_SWITCH)
static void pmu_gpio_irq_cfg_dsleep(void);
static void pmu_gpio_irq_cfg_normal(void);
static int pmu_gpio_irq_enabled(void);
#endif

// Move all the data/bss invovled in pmu_open() to .sram_data/.sram_bss,
// so that pmu_open() can be called at the end of BootInit(),
// for data/bss is initialized after BootInit().

#if defined(DCDC_VOLT_HW_RAMP_ONLY)
SRAM_BSS_LOC
#else
const
#endif
static bool dcdc_ramp_en = false;

static uint16_t SRAM_BSS_DEF(dcdc_ramp_map);

static PMU_CHARGER_IRQ_HANDLER_T charger_irq_handler;

static PMU_CAP_IRQ_HANDLER_T capsensor_irq_handler;

static PMU_IRQ_UNIFIED_HANDLER_T pmu_irq_hdlrs[PMU_IRQ_TYPE_QTY];

static uint8_t ana_lp_dcdc = 0;

// BT might have connection drop issues if dig_lp is lower than 0.8V
static uint8_t dig_lp_ldo = 0;
static uint8_t dig_lp_dcdc = 0;

static uint16_t wdt_timer;

static uint16_t ntc_raw = 0;
static int ntc_temperature = 0;
static bool ntc_irq_busy = false;
static PMU_NTC_IRQ_HANDLER_T pmu_ntc_cb;

static bool shipment_en =
#if (defined(RTC_ENABLE) || defined(RTC_CALENDAR))
    false;
#else
    true;
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE)
static const uint16_t high_perf_freq_mhz =
#if defined(MTEST_ENABLED) && defined(MTEST_CLK_MHZ)
    MTEST_CLK_MHZ;
#else
    300;
#endif
static bool high_perf_on;
#endif

#ifdef PMU_FORCE_LP_MODE
static enum PMU_BIG_BANDGAP_USER_T big_bandgap_user_map;
#endif

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
static void pmu_hppa_dcdc_to_ldo(void);
#endif

typedef struct {
    uint16_t pmu_gpio_irq_mask[PMU_GPIO_PIN_GROUP_NUM][2];
    uint16_t pmu_gpio_irq_edge_sel[PMU_GPIO_PIN_GROUP_NUM];
    uint16_t pmu_gpio_irq_posedge_sel[PMU_GPIO_PIN_GROUP_NUM];
    uint16_t pmu_gpio_irq_lowlevel_sel[PMU_GPIO_PIN_GROUP_NUM];
} PMU_GPIO_IRQ_CFG_T;

static HAL_GPIO_PIN_IRQ_HANDLER pmu_gpio_irq_handler[HAL_GPIO_PIN_PMU_NUM - HAL_GPIO_PIN_PMU_P0_0];

static PMU_GPIO_IRQ_CFG_T pmu_gpio_irq_cfg_save;

static enum PMU_GPIO_IRQ_TYPE_T pmu_gpio_irq_type = PMU_GPIO_IRQ_NONE;

static enum PMU_BOOT_CAUSE_T BOOT_BSS_LOC pmu_boot_reason = PMU_BOOT_CAUSE_NULL;

static bool BOOT_BSS_LOC pmu_boot_first_pwr_up = false;

#if defined(_AUTO_TEST_)
static bool at_skip_shutdown = false;

void pmu_at_skip_shutdown(bool enable)
{
    at_skip_shutdown = enable;
}
#endif

#ifdef RTC_ENABLE
struct PMU_RTC_CTX_T {
    bool enabled;
    bool alarm_set;
    uint32_t alarm_val;
};

static struct PMU_RTC_CTX_T BOOT_BSS_LOC rtc_ctx;

static PMU_RTC_IRQ_HANDLER_T rtc_irq_handler;

static void BOOT_TEXT_SRAM_LOC pmu_rtc_save_context(void)
{
    if (pmu_rtc_enabled()) {
        rtc_ctx.enabled = true;
        if (pmu_rtc_alarm_status_set()) {
            rtc_ctx.alarm_set = true;
            rtc_ctx.alarm_val = pmu_rtc_get_alarm();
        }
    } else {
        rtc_ctx.enabled = false;
    }
}

static void pmu_rtc_restore_context(void)
{
    uint32_t rtc_val;

    if (rtc_ctx.enabled) {
        pmu_rtc_enable();
        if (rtc_ctx.alarm_set) {
            rtc_val = pmu_rtc_get();
            if (rtc_val - rtc_ctx.alarm_val <= 1 || rtc_ctx.alarm_val - rtc_val < 5) {
                rtc_ctx.alarm_val = rtc_val + 5;
            }
            pmu_rtc_set_alarm(rtc_ctx.alarm_val);
        }
    }
}
#endif

#ifdef RTC_CALENDAR
#define RTC_REG_VAL2CALENDAR(TENS, UNITS)                     (TENS * 10 + UNITS)
#define RTC_CALENDAR2REG_VAL_TENS(RTC_CALENDAR)               (RTC_CALENDAR / 10 % 10)
#define RTC_CALENDAR2REG_VAL_UNITS(RTC_CALENDAR)              (RTC_CALENDAR / 1 % 10)

#define EXTERNAL_CRYSTAL_FREQ                                 (32768)
#define RTC_STATUS_STABLE_TIME_MS                             (1000)

#define RTC_MONTH_MAX                                         (12)
#define RTC_MONTH_MIN                                         (1)
#define RTC_DAY_MIN                                           (1)
#define RTC_WEEK_MAX                                          (7)
#define RTC_WEEK_MIN                                          (1)
#define RTC_HOUR_MAX                                          (23)
#define RTC_HOUR_MIN                                          (0)
#define RTC_MINUTE_MAX                                        (59)
#define RTC_MINUTE_MIN                                        (0)
#define RTC_SECOND_MAX                                        (59)
#define RTC_SECOND_MIN                                        (0)

struct PMU_RTC_CALENDAR_CTX_T {
    bool rtc_cal_enabled;
    bool rtc_cal_pwr_on;
    bool rtc_cal_alarm_on;
    struct RTC_CALENDAR_FORMAT_T rtc_cal_alarm_cfg;
};

struct RTC_REG_VAL_FORMAT_T {
    uint8_t year_tens;
    uint8_t year_units;
    uint8_t data_month_tens;
    uint8_t data_month_units;
    uint8_t data_day_tens;
    uint8_t data_day_units;
    uint8_t data_week;
    uint8_t hour_tens;
    uint8_t hour_units;
    uint8_t min_tens;
    uint8_t min_units;
    uint8_t sec_tens;
    uint8_t sec_units;
};

static struct PMU_RTC_CALENDAR_CTX_T BOOT_BSS_LOC rtc_cal_ctx;

static PMU_RTC_CALENDAR_IRQ_HANDLER_T rtc_cal_irq_handler;

static bool pmu_rtc_calendar_enabled(void)
{
    uint16_t val;

    // Special reg, it will be cleared only when the battery is re-powered.
    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU1806_REG_RESETN_MODE, &val);
        return !!(val & PMU1806_RTC_LOADED);
    } else if (pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU1805_REG_RTC_EN_FLAG, &val);
        return !!(val & PMU1805_RTC_LOADED);
    } else if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1809_REG_NOT_RESET, &val);
        return !!(val & PMU1809_RTC_LOADED);
    }  else {
        return false;
    }
}

#ifndef PMU_CLK_USE_EXT_CRYSTAL
static void pmu_rtc_calendar_lpo_enable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1806_REG_MAIN_LPO, &val);
        val |= REG_PU_MAIN_LPO_DR | REG_PU_MAIN_LPO;
        pmu_write(PMU1806_REG_MAIN_LPO, val);
    }
}
#endif

static void pmu_rtc_calendar_clk_div_set(void)
{
    uint16_t div;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef SIMU
        div = 32 - 2;
#else
        div = CONFIG_SYSTICK_HZ * 2 - 2;
#endif
        pmu_write(PMU_REG_RTC_DIV_1HZ, div);
    }
}

static void pmu_rtc_calendar_reset(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU1806_REG_RESERVED_METAL_1, &val);
        val &= ~RTC_RESETN;
        pmu_write(PMU1806_REG_RESERVED_METAL_1, val);
        val |= RTC_RESETN;
        pmu_write(PMU1806_REG_RESERVED_METAL_1, val);
    }
}

static void pmu_rtc_calendar_init(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        if (pmu_type != PMU_CHIP_TYPE_1809) {
            // Reset RTC when PMU is re-powered.
            if (pmu_metal_id >= HAL_CHIP_METAL_ID_1 && pmu_rtc_calendar_enabled() == false) {
                pmu_rtc_calendar_reset();
            }
        }
        // The reason for the boot is RTC powered on.
        pmu_read(PMU1806_REG_INT_CLR, &val);
        if (!!(val & RTC_INT_0)) {
            pmu_rtc_calendar_pwron_enable(false);
        }
        pmu_rtc_calendar_clk_div_set();
    }
}

static void pmu_rtc_calendar_reinit(void)
{
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
#ifndef PMU_CLK_USE_EXT_CRYSTAL
        pmu_rtc_calendar_lpo_enable();
#endif
        pmu_rtc_calendar_clk_div_set();
    }
}

static bool pmu_rtc_calendar_pwron_active(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_POWER_KEY_CFG, &val);
        return !!(val & RTC_POWER_ON_EN);
    } else {
        return false;
    }
}

static bool pmu_rtc_calendar_alarm_active(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_INT_EN, &val);
        return !!(val & RTC_INT_EN_0);
    } else {
        return false;
    }
}

static void pmu_rtc_calendar_save_context(void)
{
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        if (pmu_rtc_calendar_enabled()) {
            rtc_cal_ctx.rtc_cal_enabled = true;
            if (pmu_rtc_calendar_pwron_active()) {
                rtc_cal_ctx.rtc_cal_pwr_on = true;
            }
            if (pmu_rtc_calendar_alarm_active()) {
                rtc_cal_ctx.rtc_cal_alarm_on = true;
                pmu_rtc_calendar_alarm_get(&rtc_cal_ctx.rtc_cal_alarm_cfg);
            }
        }
    }
}

static void pmu_rtc_calendar_restore_context(void)
{
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        if (rtc_cal_ctx.rtc_cal_enabled) {
            pmu_rtc_calendar_reinit();
            if (rtc_cal_ctx.rtc_cal_pwr_on) {
                pmu_rtc_calendar_pwron_enable(true);
            }
            if (rtc_cal_ctx.rtc_cal_alarm_on) {
                pmu_rtc_calendar_alarm_set(&rtc_cal_ctx.rtc_cal_alarm_cfg);
            }
        }
    }
}

static void pmu_rtc_calendar_alarm_intr_disable(void)
{
    uint16_t val;
    uint32_t lock;

    if (pmu_type == PMU_CHIP_TYPE_1809 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
        lock = int_lock();
        pmu_read(PMU_REG_INT_EN, &val);
        val &= ~RTC_INT_EN_0;
        pmu_write(PMU_REG_INT_EN, val);
        int_unlock(lock);
    }
}

static void pmu_rtc_calendar_alarm_intr_enable(void)
{
    uint16_t val;
    uint32_t lock;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_read(PMU_REG_INT_EN, &val);
        val |= RTC_INT_EN_0;
        pmu_write(PMU_REG_INT_EN, val);
        int_unlock(lock);
    }
}

static void pmu_rtc_calendar_data_valid_check(struct RTC_CALENDAR_FORMAT_T *calendar)
{
    char month_buf[RTC_MONTH_MAX] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        if (calendar->week > RTC_WEEK_MAX || calendar->week < RTC_WEEK_MIN) {
            ASSERT(0, "invalid week! week:%d", calendar->week);
        }

        if ((calendar->hour > RTC_HOUR_MAX || calendar->minute > RTC_MINUTE_MAX || calendar->second > RTC_SECOND_MAX)) {
            ASSERT(0, "invalid time! hour/min/sec:%d/%d/%d", calendar->hour, calendar->minute, calendar->second);
        }

        if (calendar->month == 2) {
            // leap year
            if (((calendar->year % 4 == 0) && (calendar->year % 100 != 0)) || (calendar->year % 400 == 0)) {
                month_buf[1] += 1;
            }
        }

        if (calendar->month > RTC_MONTH_MAX || calendar->month < RTC_MONTH_MIN ||
                calendar->day > month_buf[calendar->month - 1] || calendar->day < RTC_DAY_MIN) {
            ASSERT(0, "invalid data! month/day:%d/%d", calendar->month, calendar->day);
        }
    }
}

static void _pmu_rtc_calendar_alarm_irq_handler(uint16_t irq_status)
{
    struct RTC_CALENDAR_FORMAT_T alarm_cal = {0, };

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        if (irq_status & RTC_INT0_MSKED) {
            if (pmu_type == PMU_CHIP_TYPE_1809 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
                // Workaround for 1809 RTC irq, masked will enabled for 1 second, intr_en will re-enabled in pmu_rtc_calendar_alarm_set().
                pmu_rtc_calendar_alarm_intr_disable();
            }
            pmu_write(PMU_REG_INT_STATUS, RTC_INT_0);
            if (rtc_cal_irq_handler) {
                pmu_rtc_calendar_get(&alarm_cal);
                rtc_cal_irq_handler(&alarm_cal);
            }
        }
    }
}

int pmu_rtc_calendar_alarm_irq_handler_set(PMU_RTC_CALENDAR_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        rtc_cal_irq_handler = handler;

        lock = int_lock();
        pmu_read(PMU_REG_INT_MASK, &val);
        if (handler) {
            val |= RTC_INT0_MSK;
        } else {
            val &= ~RTC_INT0_MSK;
        }
        pmu_write(PMU_REG_INT_MASK, val);
        pmu_set_irq_unified_handler(PMU_IRQ_TYPE_RTC, handler ? _pmu_rtc_calendar_alarm_irq_handler : NULL);
        int_unlock(lock);
        return 0;
    } else {
        return -1;
    }
}

int pmu_rtc_calendar_pwron_enable(bool en)
{
    uint32_t lock;
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_read(PMU_REG_POWER_KEY_CFG, &val);
        if (en) {
            val |= RTC_POWER_ON_EN;
        } else {
            val &= ~RTC_POWER_ON_EN;
        }
        pmu_write(PMU_REG_POWER_KEY_CFG, val);
        int_unlock(lock);

        return 0;
    } else {
        return -1;
    }
}

int pmu_rtc_calendar_alarm_clear(void)
{
    uint16_t val;
    uint32_t lock;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_read(PMU_REG_INT_EN, &val);
        val &= ~RTC_INT_EN_0;
        pmu_write(PMU_REG_INT_EN, val);
        int_unlock(lock);
        return 0;
    } else {
        return -1;
    }
}

int pmu_rtc_calendar_alarm_set(struct RTC_CALENDAR_FORMAT_T *alarm_set)
{
    uint32_t lock;
    uint16_t val;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_rtc_calendar_data_valid_check(alarm_set);

        rtc_reg_fmt.year_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->year);
        rtc_reg_fmt.year_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->year);

        rtc_reg_fmt.data_month_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->month);
        rtc_reg_fmt.data_month_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->month);

        rtc_reg_fmt.data_day_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->day);
        rtc_reg_fmt.data_day_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->day);

        rtc_reg_fmt.data_week = alarm_set->week;

        rtc_reg_fmt.hour_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->hour);
        rtc_reg_fmt.hour_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->hour);

        rtc_reg_fmt.min_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->minute);
        rtc_reg_fmt.min_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->minute);

        rtc_reg_fmt.sec_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->second);
        rtc_reg_fmt.sec_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->second);

        // year
        pmu_read(PMU1806_REG_RTC_MATCH_YEARS, &val);
        val = SET_BITFIELD(val, RTC_MATCH_YEARS_TENS, rtc_reg_fmt.year_tens);
        val = SET_BITFIELD(val, RTC_MATCH_YEARS_UNITS, rtc_reg_fmt.year_units);
        pmu_write(PMU1806_REG_RTC_MATCH_YEARS, val);

        // data
        pmu_read(PMU1806_REG_RTC_MATCH_DATAS, &val);
        if (rtc_reg_fmt.data_month_tens) {
            val |= RTC_MATCH_DATAS_MONTH_TENS;
        } else {
            val &= ~RTC_MATCH_DATAS_MONTH_TENS;
        }
        val = SET_BITFIELD(val, RTC_MATCH_DATAS_MONTH_UNITS, rtc_reg_fmt.data_month_units);
        val = SET_BITFIELD(val, RTC_MATCH_DATAS_DAY_TENS, rtc_reg_fmt.data_day_tens);
        val = SET_BITFIELD(val, RTC_MATCH_DATAS_DAY_UNITS, rtc_reg_fmt.data_day_units);
        val = SET_BITFIELD(val, RTC_MATCH_DATAS_WEEK, rtc_reg_fmt.data_week);
        pmu_write(PMU1806_REG_RTC_MATCH_DATAS, val);

        // hour
        pmu_read(PMU1806_REG_RTC_MATCH_HOURS, &val);
        val = SET_BITFIELD(val, RTC_MATCH_HOURS_TENS, rtc_reg_fmt.hour_tens);
        val = SET_BITFIELD(val, RTC_MATCH_HOURS_UNITS, rtc_reg_fmt.hour_units);
        pmu_write(PMU1806_REG_RTC_MATCH_HOURS, val);

        // minute_second
        pmu_read(PMU1806_REG_RTC_MATCH_MIN_SEC, &val);
        val = SET_BITFIELD(val, RTC_MATCH_TIME_MIN_TENS, rtc_reg_fmt.min_tens);
        val = SET_BITFIELD(val, RTC_MATCH_TIME_MIN_UNITS, rtc_reg_fmt.min_units);
        val = SET_BITFIELD(val, RTC_MATCH_TIME_SEC_TENS, rtc_reg_fmt.sec_tens);
        val = SET_BITFIELD(val, RTC_MATCH_TIME_SEC_UNITS, rtc_reg_fmt.sec_units);
        pmu_write(PMU1806_REG_RTC_MATCH_MIN_SEC, val);

        pmu_rtc_calendar_alarm_intr_enable();

        int_unlock(lock);

        return 0;
    }else {
        return -1;
    }
}

int pmu_rtc_calendar_alarm_get(struct RTC_CALENDAR_FORMAT_T *alarm_get)
{
    uint32_t lock;
    uint16_t val;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        // year
        pmu_read(PMU1806_REG_RTC_MATCH_YEARS, &val);
        rtc_reg_fmt.year_tens = GET_BITFIELD(val, RTC_GET_YEARS_TENS);
        rtc_reg_fmt.year_units = GET_BITFIELD(val, RTC_GET_YEARS_UNITS);
        alarm_get->year = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.year_tens, rtc_reg_fmt.year_units) + 2000;

        // data
        pmu_read(PMU1806_REG_RTC_MATCH_DATAS, &val);
        if (val & RTC_GET_DATAS_MONTH_TENS) {
            rtc_reg_fmt.data_month_tens = 1;
        } else {
            rtc_reg_fmt.data_month_tens = 0;
        }
        rtc_reg_fmt.data_month_units = GET_BITFIELD(val, RTC_GET_DATAS_MONTH_UNITS);
        alarm_get->month = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_month_tens, rtc_reg_fmt.data_month_units);

        rtc_reg_fmt.data_day_tens = GET_BITFIELD(val, RTC_GET_DATAS_DAY_TENS);
        rtc_reg_fmt.data_day_units = GET_BITFIELD(val, RTC_GET_DATAS_DAY_UNITS);
        alarm_get->day = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_day_tens, rtc_reg_fmt.data_day_units);

        rtc_reg_fmt.data_week = GET_BITFIELD(val, RTC_GET_DATAS_WEEK);
        alarm_get->week = rtc_reg_fmt.data_week;

        // hour
        pmu_read(PMU1806_REG_RTC_MATCH_HOURS, &val);
        rtc_reg_fmt.hour_tens = GET_BITFIELD(val, RTC_GET_HOURS_TENS);
        rtc_reg_fmt.hour_units = GET_BITFIELD(val, RTC_GET_HOURS_UNITS);
        alarm_get->hour = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.hour_tens, rtc_reg_fmt.hour_units);

        // minute_second
        pmu_read(PMU1806_REG_RTC_MATCH_MIN_SEC, &val);
        rtc_reg_fmt.min_tens = GET_BITFIELD(val, RTC_GET_TIME_MIN_TENS);
        rtc_reg_fmt.min_units = GET_BITFIELD(val, RTC_GET_TIME_MIN_UNITS);
        alarm_get->minute = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.min_tens, rtc_reg_fmt.min_units);

        rtc_reg_fmt.sec_tens = GET_BITFIELD(val, RTC_GET_TIME_SEC_TENS);
        rtc_reg_fmt.sec_units = GET_BITFIELD(val, RTC_GET_TIME_SEC_UNITS);
        alarm_get->second = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.sec_tens, rtc_reg_fmt.sec_units);

        int_unlock(lock);

        return 0;
    } else {
        return -1;
    }
}

int pmu_rtc_calendar_set(struct RTC_CALENDAR_FORMAT_T *calendar_set)
{
    uint32_t lock;
    uint16_t val;
    bool rtc_en = false;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_rtc_calendar_data_valid_check(calendar_set);

        if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
            // If load 19:59:58/59, hours will become 30 hours.
            if (calendar_set->hour == 19 && calendar_set->minute == 59 && (calendar_set->second == 58 || calendar_set->second == 59)) {
                if (calendar_set->second == 58) {
                    // -1 second
                    calendar_set->second = 57;
                } else {
                    // +1 second
                    calendar_set->hour = 20;
                    calendar_set->minute = 0;
                    calendar_set->second = 0;
                }
            }
        }

        rtc_en = pmu_rtc_calendar_enabled();
        if (pmu_type != PMU_CHIP_TYPE_1809) {
            // Enable reg_en to set rtc
            if (rtc_en) {
                pmu_write(PMU1806_REG_RTC_LOAD_DATAS_EN, 0);
                pmu_write(PMU1806_REG_RTC_LOAD_MIN_SEC_EN, 0);
                hal_sys_timer_delay(MS_TO_TICKS(RTC_STATUS_STABLE_TIME_MS));
            }
        }

        rtc_reg_fmt.year_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->year);
        rtc_reg_fmt.year_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->year);

        if (RTC_CALENDAR2REG_VAL_TENS(calendar_set->month) != 0) {
            rtc_reg_fmt.data_month_tens = 1;
        } else {
            rtc_reg_fmt.data_month_tens = 0;
        }
        rtc_reg_fmt.data_month_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->month);

        rtc_reg_fmt.data_day_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->day);
        rtc_reg_fmt.data_day_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->day);

        rtc_reg_fmt.data_week = calendar_set->week;

        rtc_reg_fmt.hour_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->hour);
        rtc_reg_fmt.hour_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->hour);

        rtc_reg_fmt.min_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->minute);
        rtc_reg_fmt.min_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->minute);

        rtc_reg_fmt.sec_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->second);
        rtc_reg_fmt.sec_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->second);

        // year
        pmu_read(PMU1806_REG_RTC_LOAD_YEARS, &val);
        val = SET_BITFIELD(val, RTC_LOAD_YEARS_TENS, rtc_reg_fmt.year_tens);
        val = SET_BITFIELD(val, RTC_LOAD_YEARS_UNITS, rtc_reg_fmt.year_units);
        pmu_write(PMU1806_REG_RTC_LOAD_YEARS, val);

        // data
        pmu_read(PMU1806_REG_RTC_LOAD_DATAS_EN, &val);
        if (rtc_reg_fmt.data_month_tens) {
            val |= RTC_LOAD_DATAS_MONTH_TENS;
        } else {
            val &= ~RTC_LOAD_DATAS_MONTH_TENS;
        }
        val = SET_BITFIELD(val, RTC_LOAD_DATAS_MONTH_UNITS, rtc_reg_fmt.data_month_units);
        val = SET_BITFIELD(val, RTC_LOAD_DATAS_DAY_TENS, rtc_reg_fmt.data_day_tens);
        val = SET_BITFIELD(val, RTC_LOAD_DATAS_DAY_UNITS, rtc_reg_fmt.data_day_units);
        val = SET_BITFIELD(val, RTC_LOAD_DATAS_WEEK, rtc_reg_fmt.data_week);
        pmu_write(PMU1806_REG_RTC_LOAD_DATAS_EN, val);

        // hour
        pmu_read(PMU1806_REG_RTC_LOAD_HOURS, &val);
        val = SET_BITFIELD(val, RTC_LOAD_HOURS_TENS, rtc_reg_fmt.hour_tens);
        val = SET_BITFIELD(val, RTC_LOAD_HOURS_UNITS, rtc_reg_fmt.hour_units);
        pmu_write(PMU1806_REG_RTC_LOAD_HOURS, val);

        if (pmu_type != PMU_CHIP_TYPE_1809) {
            if (rtc_en == false) {
                TRACE(0, "RTC: Wait 1s for the date to take effect!");
                hal_sys_timer_delay(MS_TO_TICKS(RTC_STATUS_STABLE_TIME_MS));
            }
        }

        // minute_second
        pmu_read(PMU1806_REG_RTC_LOAD_MIN_SEC_EN, &val);
        val = SET_BITFIELD(val, RTC_LOAD_TIME_MIN_TENS, rtc_reg_fmt.min_tens);
        val = SET_BITFIELD(val, RTC_LOAD_TIME_MIN_UNITS, rtc_reg_fmt.min_units);
        val = SET_BITFIELD(val, RTC_LOAD_TIME_SEC_TENS, rtc_reg_fmt.sec_tens);
        val = SET_BITFIELD(val, RTC_LOAD_TIME_SEC_UNITS, rtc_reg_fmt.sec_units);
        pmu_write(PMU1806_REG_RTC_LOAD_MIN_SEC_EN, val);

        if (rtc_en == false) {
            if (pmu_type == PMU_CHIP_TYPE_1809) {
                pmu_read(PMU1809_REG_NOT_RESET, &val);
                val |= PMU1809_RTC_LOADED;
                pmu_write(PMU1809_REG_NOT_RESET, val);
            } else if (pmu_type == PMU_CHIP_TYPE_1806) {
                pmu_read(PMU1806_REG_RESETN_MODE, &val);
                val |= PMU1806_RTC_LOADED;
                pmu_write(PMU1806_REG_RESETN_MODE, val);
            } else {
                pmu_read(PMU1805_REG_RTC_EN_FLAG, &val);
                val |= PMU1805_RTC_LOADED;
                pmu_write(PMU1805_REG_RTC_EN_FLAG, val);
            }
        }

        if (pmu_type == PMU_CHIP_TYPE_1809) {
            pmu_read(PMU1809_REG_RTC_CFG_178, &val);
            val |= RTC_TIME_LOAD;
            pmu_write(PMU1809_REG_RTC_CFG_178, val);
        }

        int_unlock(lock);

        return 0;
    } else {
        return -1;
    }
}

int pmu_rtc_calendar_get(struct RTC_CALENDAR_FORMAT_T *calendar_get)
{
    uint32_t lock;
    uint16_t val;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        // year
        if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            pmu_read(PMU1806_REG_METAL_1, &val);
            val |= RTC_DATE_SEL;
            pmu_write(PMU1806_REG_METAL_1, val);

            pmu_read(PMU1806_REG_RTC_GET_DATAS, &val);
            rtc_reg_fmt.data_day_tens = GET_BITFIELD(val, RTC_GET_DATAS_DAY_TENS);
            rtc_reg_fmt.data_day_units = GET_BITFIELD(val, RTC_GET_DATAS_DAY_UNITS);
            calendar_get->year = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_day_tens, rtc_reg_fmt.data_day_units) + 2000;

            pmu_read(PMU1806_REG_METAL_1, &val);
            val &= ~RTC_DATE_SEL;
            pmu_write(PMU1806_REG_METAL_1, val);
        } else {
            // Can't get rtc_year for 1806 metal0 chips.
            pmu_read(PMU1806_REG_RTC_GET_YEARS, &val);
            rtc_reg_fmt.year_tens = GET_BITFIELD(val, RTC_GET_YEARS_TENS);
            rtc_reg_fmt.year_units = GET_BITFIELD(val, RTC_GET_YEARS_UNITS);
            calendar_get->year = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.year_tens, rtc_reg_fmt.year_units) + 2000;
        }

        // data
        pmu_read(PMU1806_REG_RTC_GET_DATAS, &val);
        if (val & RTC_GET_DATAS_MONTH_TENS) {
            rtc_reg_fmt.data_month_tens = 1;
        } else {
            rtc_reg_fmt.data_month_tens = 0;
        }
        rtc_reg_fmt.data_month_units = GET_BITFIELD(val, RTC_GET_DATAS_MONTH_UNITS);
        calendar_get->month = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_month_tens, rtc_reg_fmt.data_month_units);

        rtc_reg_fmt.data_day_tens = GET_BITFIELD(val, RTC_GET_DATAS_DAY_TENS);
        rtc_reg_fmt.data_day_units = GET_BITFIELD(val, RTC_GET_DATAS_DAY_UNITS);
        calendar_get->day = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_day_tens, rtc_reg_fmt.data_day_units);

        rtc_reg_fmt.data_week = GET_BITFIELD(val, RTC_GET_DATAS_WEEK);
        calendar_get->week = rtc_reg_fmt.data_week;

        // hour
        pmu_read(PMU1806_REG_RTC_GET_HOURS, &val);
        rtc_reg_fmt.hour_tens = GET_BITFIELD(val, RTC_GET_HOURS_TENS);
        rtc_reg_fmt.hour_units = GET_BITFIELD(val, RTC_GET_HOURS_UNITS);
        calendar_get->hour = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.hour_tens, rtc_reg_fmt.hour_units);

        // minute_second
        pmu_read(PMU1806_REG_RTC_GET_MIN_SEC, &val);
        rtc_reg_fmt.min_tens = GET_BITFIELD(val, RTC_GET_TIME_MIN_TENS);
        rtc_reg_fmt.min_units = GET_BITFIELD(val, RTC_GET_TIME_MIN_UNITS);
        calendar_get->minute = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.min_tens, rtc_reg_fmt.min_units);

        rtc_reg_fmt.sec_tens = GET_BITFIELD(val, RTC_GET_TIME_SEC_TENS);
        rtc_reg_fmt.sec_units = GET_BITFIELD(val, RTC_GET_TIME_SEC_UNITS);
        calendar_get->second = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.sec_tens, rtc_reg_fmt.sec_units);

        int_unlock(lock);

        return 0;
    } else {
        return -1;
    }
}
#endif

#define REG_VBAT_OSCP_FUNC_EN                                   0
#define REG_VMICA_OSCP_FUNC_EN                                  0
#define REG_VMICB_OSCP_FUNC_EN                                  0

#define REG_VBAT_OV_MSK                                         (1 << (7 - 0))
#define REG_VUSB_OV_MSK                                         (1 << (7 - 1))
#define REG_VIO_OV_MSK                                          (1 << (7 - 2))
#define REG_VMEM_OV_MSK                                         (1 << (7 - 3))
#define REG_BUCK_VHPPA_OV_MSK                                   (1 << (7 - 3))
#define REG_VSENSOR_OV_MSK                                      (1 << (7 - 4))
#define REG_VANA_OV_MSK                                         (1 << (7 - 5))
#define REG_BUCK_VANA_OV_MSK                                    (1 << (7 - 5))
#define REG_VBAT2VCORE_OV_MSK                                   (1 << (7 - 6))
#define REG_BUCK_VCORE_OV_MSK                                   (1 << (7 - 6))
#define REG_VCORE_L1_OV_MSK                                     (1 << (7 - 7))
#define REG_OTP_OV_MSK                                          0
#define REG_VMEM2_OV_MSK                                        0
#define REG_VMEM3_OV_MSK                                        0
#define REG_VMICA_OV_MSK                                        0
#define REG_VMICB_OV_MSK                                        0

#define REG_VBAT_UV_MSK                                         0
#define REG_VCORE_L1_UV_MSK                                     (1 << (15 - 1))
#define REG_VBAT2VCORE_UV_MSK                                   (1 << (15 - 2))
#define REG_BUCK_VCORE_UV_MSK                                   (1 << (15 - 2))
#define REG_VANA_UV_MSK                                         (1 << (15 - 3))
#define REG_BUCK_VANA_UV_MSK                                    (1 << (15 - 3))
#define REG_VMEM_UV_MSK                                         (1 << (15 - 4))
#define REG_BUCK_VHPPA_UV_MSK                                   (1 << (15 - 4))
#define REG_VMEM2_UV_MSK                                        (1 << (15 - 5))
#define REG_VMEM3_UV_MSK                                        (1 << (15 - 6))
#define REG_VSENSOR_UV_MSK                                      (1 << (15 - 7))
#define REG_VIO_UV_MSK                                          (1 << (15 - 8))
#define REG_VUSB_UV_MSK                                         (1 << (15 - 9))
#define REG_VRTC_UV_MSK                                         (1 << (15 - 10))
#define REG_VDIG_CODEC_UV_MSK                                   (1 << (15 - 11))
#define REG_AVDD25_INF_UV_MSK                                   (1 << (15 - 12))
#define REG_VMICA_UV_MSK                                        (1 << (15 - 13))
#define REG_VMICB_UV_MSK                                        (1 << (15 - 14))
#define REG_OTP_UV_MSK                                          0

enum PMU_POWER_PROTECT_ABORT_IRQ_RAW_T {
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_BUCK_VCORE                  = (1 << 0),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_BUCK_VANA                   = (1 << 1),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_BUCK_VHPPA                  = (1 << 2),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VBAT2VCORE              = (1 << 3),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VCORE_L1                = (1 << 4),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VANA                    = (1 << 5),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VIO                     = (1 << 6),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VMEM                    = (1 << 7),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VUSB                    = (1 << 8),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VMEM2                   = (1 << 9),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VMEM3                   = (1 << 10),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VSENSOR                 = (1 << 11),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_VBAT                        = (1 << 12),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_VMICA                       = (1 << 16),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_VMICB                       = (1 << 17),
    PMU_POWER_PROTECT_ABORT_IRQ_RAW_OTP                         = (1 << 18),
};

struct PMU_POWER_PROTECT_MODULE_CFG_T {
    unsigned short oscp_func_en;
    unsigned short ov_msk;
    unsigned short uv_msk;
    unsigned short hw_mode;
    unsigned short sel_pmu_off;
    unsigned short intr_en;
    unsigned short intr_msk;
    unsigned short intr_clr;
};

struct PMU_POWER_PROTECT_ABORT_SOURCE_VAL_T {
    unsigned short abort_source_mask;
    unsigned short abort_source_shift;
};

struct PMU_POWER_PROTECT_ABORT_IRQ_MAP_T {
    enum PMU_POWER_PROTECT_MODULE_T module;
    enum PMU_POWER_PROTECT_ABORT_IRQ_RAW_T raw;
};

#define PMU_POWER_PROTECT_MODULE_CFG_VAL(m)              { \
    REG_##m##_OSCP_FUNC_EN, \
    REG_##m##_OV_MSK, REG_##m##_UV_MSK, \
    REG_##m##_HW_MODE, REG_##m##_HANDLE_SEL_PMU_OFF, \
    REG_##m##_INTR_EN, REG_##m##_INTR_MSK, REG_##m##_INTR_CLR}

#define PMU_POWER_PROTECT_ABORT_SOURCE_VAL(m)              { \
    REG_##m##_ABORT_SOURCE_MASK, REG_##m##_ABORT_SOURCE_SHIFT}

static const struct PMU_POWER_PROTECT_MODULE_CFG_T pmu_power_protect_module_cfg[] = {
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VBAT2VCORE),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VCORE_L1),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VANA),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VIO),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VMEM),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VUSB),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VMEM2),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VMEM3),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VSENSOR),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(BUCK_VCORE),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(BUCK_VANA),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(BUCK_VHPPA),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VBAT),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VMICA),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(VMICB),
    PMU_POWER_PROTECT_MODULE_CFG_VAL(OTP),
};

static const struct PMU_POWER_PROTECT_ABORT_SOURCE_VAL_T pmu_power_protect_abort_source[] = {
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VBAT2VCORE),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VCORE_L1),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VANA),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VIO),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VMEM),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VUSB),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VMEM2),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VMEM3),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VSENSOR),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(BUCK_VCORE),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(BUCK_VANA),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(BUCK_VHPPA),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VBAT),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VMICA),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(VMICB),
    PMU_POWER_PROTECT_ABORT_SOURCE_VAL(OTP),
};

static const struct PMU_POWER_PROTECT_ABORT_IRQ_MAP_T pmu_power_protect_irq_map[] = {
    {PMU_POWER_PROTECT_MODULE_LDO_VBAT2VCORE,  PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VBAT2VCORE},
    {PMU_POWER_PROTECT_MODULE_LDO_VCORE_L1,    PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VCORE_L1  },
    {PMU_POWER_PROTECT_MODULE_LDO_VANA,        PMU_POWER_PROTECT_ABORT_IRQ_RAW_BUCK_VANA     },
    {PMU_POWER_PROTECT_MODULE_LDO_VIO,         PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VIO       },
    {PMU_POWER_PROTECT_MODULE_LDO_VMEM,        PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VMEM      },
    {PMU_POWER_PROTECT_MODULE_LDO_VUSB,        PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VUSB      },
    {PMU_POWER_PROTECT_MODULE_LDO_VMEM2,       PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VMEM2     },
    {PMU_POWER_PROTECT_MODULE_LDO_VMEM3,       PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VMEM3     },
    {PMU_POWER_PROTECT_MODULE_LDO_VSENSOR,     PMU_POWER_PROTECT_ABORT_IRQ_RAW_LDO_VSENSOR   },
    {PMU_POWER_PROTECT_MODULE_BUCK_VCORE,      PMU_POWER_PROTECT_ABORT_IRQ_RAW_BUCK_VCORE    },
    {PMU_POWER_PROTECT_MODULE_BUCK_VANA,       PMU_POWER_PROTECT_ABORT_IRQ_RAW_BUCK_VANA     },
    {PMU_POWER_PROTECT_MODULE_BUCK_VHPPA,      PMU_POWER_PROTECT_ABORT_IRQ_RAW_BUCK_VHPPA    },
    {PMU_POWER_PROTECT_MODULE_VBAT,            PMU_POWER_PROTECT_ABORT_IRQ_RAW_VBAT          },
    {PMU_POWER_PROTECT_MODULE_VMICA,           PMU_POWER_PROTECT_ABORT_IRQ_RAW_VMICA         },
    {PMU_POWER_PROTECT_MODULE_VMICB,           PMU_POWER_PROTECT_ABORT_IRQ_RAW_VMICB         },
    {PMU_POWER_PROTECT_MODULE_OTP,             PMU_POWER_PROTECT_ABORT_IRQ_RAW_OTP           },
};

static bool POSSIBLY_UNUSED pmu_pwr_protect_cur_en;
static bool POSSIBLY_UNUSED pmu_pwr_protect_volt_en;
static PMU_POWER_PROTECT_ABORT_IRQ_HANDLER_T pmu_power_protect_abort_irq_handler;

static inline uint16_t pmu_power_protect_module_config_addr_get(enum PMU_POWER_PROTECT_MODULE_T module)
{
    if (module <= PMU_POWER_PROTECT_MODULE_LDO_VSENSOR) {
        return PMU1809_REG_VBAT2VCORE_PP_CFG + ((module - PMU_POWER_PROTECT_MODULE_LDO_VBAT2VCORE) << 2);
    } else if (module >= PMU_POWER_PROTECT_MODULE_BUCK_VCORE && module <= PMU_POWER_PROTECT_MODULE_BUCK_VHPPA) {
        return PMU1809_REG_BUCK_VCORE_PP_CFG + ((module - PMU_POWER_PROTECT_MODULE_BUCK_VCORE) << 2);
    } else if (module == PMU_POWER_PROTECT_MODULE_OTP) {
        return PMU1809_REG_OTP_PP_CFG;
    } else if (module == PMU_POWER_PROTECT_MODULE_VBAT) {
        return PMU1809_REG_VBAT_PP_CFG;
    } else {
        return PMU1809_REG_VMICA_PP_CFG + ((module - PMU_POWER_PROTECT_MODULE_VMICA) << 1);
    }
}

static inline uint16_t pmu_power_protect_abort_source_addr_get(enum PMU_POWER_PROTECT_MODULE_T module)
{
    if (module == PMU_POWER_PROTECT_MODULE_BUCK_VANA || module == PMU_POWER_PROTECT_MODULE_BUCK_VCORE) {
        return PMU1809_REG_ABORT_SOURCE_98;
    } else if (module == PMU_POWER_PROTECT_MODULE_LDO_VBAT2VCORE || module == PMU_POWER_PROTECT_MODULE_BUCK_VHPPA) {
        return PMU1809_REG_ABORT_SOURCE_99;
    } else if (module == PMU_POWER_PROTECT_MODULE_LDO_VCORE_L1 || module == PMU_POWER_PROTECT_MODULE_LDO_VANA) {
        return PMU1809_REG_ABORT_SOURCE_9A;
    } else if (module == PMU_POWER_PROTECT_MODULE_LDO_VIO || module == PMU_POWER_PROTECT_MODULE_LDO_VMEM) {
        return PMU1809_REG_ABORT_SOURCE_9B;
    } else if (module == PMU_POWER_PROTECT_MODULE_LDO_VMEM2 || module == PMU_POWER_PROTECT_MODULE_LDO_VUSB) {
        return PMU1809_REG_ABORT_SOURCE_9C;
    } else if (module == PMU_POWER_PROTECT_MODULE_LDO_VMEM3 || module == PMU_POWER_PROTECT_MODULE_LDO_VSENSOR) {
        return PMU1809_REG_ABORT_SOURCE_9D;
    } else if (module == PMU_POWER_PROTECT_MODULE_VBAT) {
        return PMU1809_REG_ABORT_SOURCE_9E;
    } else if (module == PMU_POWER_PROTECT_MODULE_VMICA || module == PMU_POWER_PROTECT_MODULE_VMICB) {
        return PMU1809_REG_ABORT_SOURCE_A0;
    } else {
        // OTP
        return PMU1809_REG_ABORT_SOURCE_A1;
    }
}

static void pmu_power_protect_module_irq_clr(enum PMU_POWER_PROTECT_MODULE_T module)
{
    uint16_t module_addr;
    uint16_t intr_cfg_addr;
    uint16_t val;
    const struct PMU_POWER_PROTECT_MODULE_CFG_T *module_cfg_p = &pmu_power_protect_module_cfg[module];

    module_addr = pmu_power_protect_module_config_addr_get(module);

    if (module <= PMU_POWER_PROTECT_MODULE_BUCK_VHPPA) {
        intr_cfg_addr = module_addr + 2;
    } else if (module == PMU_POWER_PROTECT_MODULE_OTP) {
        intr_cfg_addr = PMU1809_REG_OTP_PP_HW_CFG;
    } else {
        intr_cfg_addr = module_addr;
    }

    pmu_read(intr_cfg_addr, &val);
    val |= module_cfg_p->intr_clr;
    pmu_write(intr_cfg_addr, val);
}

static void _pmu_power_protect_irq_handler(uint16_t irq_status)
{
    enum PMU_POWER_PROTECT_MODULE_T abort_module = PMU_POWER_PROTECT_MODULE_QTY;
    uint8_t abort_source = 0;
    uint32_t raw_total;
    uint16_t raw_96;
    uint16_t raw_97;

    pmu_read(PMU1809_REG_ABORT_INTR_96, &raw_96);
    pmu_read(PMU1809_REG_ABORT_INTR_97, &raw_97);

    raw_96 = GET_BITFIELD(raw_96, ABORT_INTR_18_16);
    raw_total = raw_96 << 16 | raw_97;

    for (int i = 0; i < ARRAY_SIZE(pmu_power_protect_irq_map); i++) {
        if (!!(pmu_power_protect_irq_map[i].raw & raw_total)) {
            abort_module = pmu_power_protect_irq_map[i].module;
            pmu_power_protect_module_irq_clr(abort_module);
            abort_source = pmu_power_protect_abort_source_get(abort_module);
            TRACE(0, "%s: ##abort happened! module:%d source=0x%02X", __func__, abort_module, abort_source);
        }
    }

    if (pmu_power_protect_abort_irq_handler) {
        pmu_power_protect_abort_irq_handler(abort_module, abort_source);
    }
}

static void POSSIBLY_UNUSED pmu_power_protect_irq_handler_dummy(enum PMU_POWER_PROTECT_MODULE_T module, enum PMU_POWER_PROTECT_ABORT_SOURCE_T source)
{
}

#ifdef PMU_POWER_PROTECT_FORCE_HW_PMU_OFF
static void POSSIBLY_UNUSED pmu_power_protect_module_force_hw_pmu_off_config(void)
{
    uint16_t val;
    uint16_t module_address;
    uint16_t module_address_hw_mode;
    const struct PMU_POWER_PROTECT_MODULE_CFG_T *module_cfg_p;
    enum PMU_POWER_PROTECT_MODULE_T module;

    for (module = PMU_POWER_PROTECT_MODULE_LDO_VBAT2VCORE; module < PMU_POWER_PROTECT_MODULE_QTY; module++) {
        module_cfg_p = &pmu_power_protect_module_cfg[module];
        module_address = pmu_power_protect_module_config_addr_get(module);

        if (module <= PMU_POWER_PROTECT_MODULE_BUCK_VHPPA) {
            module_address_hw_mode = module_address + 1;
        } else if (module == PMU_POWER_PROTECT_MODULE_OTP) {
            module_address_hw_mode = PMU1809_REG_OTP_PP_HW_CFG;
        } else {
            module_address_hw_mode = module_address;
        }

        pmu_read(module_address_hw_mode, &val);
        val |= module_cfg_p->hw_mode | module_cfg_p->sel_pmu_off;
        pmu_write(module_address_hw_mode, val);
    }
}
#endif

enum PMU_POWER_PROTECT_RET_T pmu_power_protect_config(enum PMU_POWER_PROTECT_MODULE_T module,
                        enum PMU_POWER_PROTECT_MODE_T mode,
                        enum PMU_POWER_PROTECT_HW_TYPE_T type,
                        const struct PMU_POWER_PROTECT_TYPE_T *cfg)
{
    uint32_t lock;
    uint16_t val;
    uint16_t module_addr;
    uint16_t hw_cfg_addr;
    uint16_t intr_cfg_addr;
    const struct PMU_POWER_PROTECT_MODULE_CFG_T *pwr_prt_cfg_p = &pmu_power_protect_module_cfg[module];

    if (pmu_type != PMU_CHIP_TYPE_1809) {
        return PMU_POWER_PROTECT_RET_INVALID_CHIP;
    }

    if (module >= PMU_POWER_PROTECT_MODULE_QTY) {
        return PMU_POWER_PROTECT_RET_INVALID_MODULE;
    }

    if ((pmu_pwr_protect_cur_en == false && cfg->osc_det_en) || (pmu_pwr_protect_volt_en == false && (cfg->ov_det_en || cfg->uv_det_en))) {
        return PMU_POWER_PROTECT_RET_INVALID_EFUSE;
    }

    if (cfg->osc_det_en && (module >= PMU_POWER_PROTECT_MODULE_VBAT && module < PMU_POWER_PROTECT_MODULE_OTP)) {
        return PMU_POWER_PROTECT_RET_INVALID_OSCP;
    }

    if (cfg->ov_det_en && (module > PMU_POWER_PROTECT_MODULE_VBAT || module == PMU_POWER_PROTECT_MODULE_LDO_VMEM2 || module == PMU_POWER_PROTECT_MODULE_LDO_VMEM3)) {
        return PMU_POWER_PROTECT_RET_INVALID_OV;
    }

    if (cfg->uv_det_en && (module == PMU_POWER_PROTECT_MODULE_VBAT || module == PMU_POWER_PROTECT_MODULE_OTP)) {
        return PMU_POWER_PROTECT_RET_INVALID_UV;
    }

    if (pmu_metal_id <= HAL_CHIP_METAL_ID_2 && (module == PMU_POWER_PROTECT_MODULE_VMICA || module == PMU_POWER_PROTECT_MODULE_VMICB)) {
        return PMU_POWER_PROTECT_RET_INVALID_TEMP;
    }

    lock = int_lock();
#ifndef PMU_POWER_PROTECT_VOLT_DISABLE
    pmu_read(PMU1809_REG_PP_OV_MSK, &val);
    if (cfg->ov_det_en) {
        val |= pwr_prt_cfg_p->ov_msk;
    } else {
        val &= ~pwr_prt_cfg_p->ov_msk;
    }
    pmu_write(PMU1809_REG_PP_OV_MSK, val);

    pmu_read(PMU1809_REG_PP_UV_MSK, &val);
    if (cfg->uv_det_en) {
        val |= pwr_prt_cfg_p->uv_msk;
    } else {
        val &= ~pwr_prt_cfg_p->uv_msk;
    }
    pmu_write(PMU1809_REG_PP_UV_MSK, val);
#endif

    module_addr = pmu_power_protect_module_config_addr_get(module);

    if (module <= PMU_POWER_PROTECT_MODULE_BUCK_VHPPA) {
        pmu_read(module_addr, &val);
        if (cfg->osc_det_en) {
            val |= pwr_prt_cfg_p->oscp_func_en;
        } else {
            val &= ~pwr_prt_cfg_p->oscp_func_en;
        }
        pmu_write(module_addr, val);

        hw_cfg_addr = module_addr + 1;
        intr_cfg_addr = module_addr + 2;
    } else if (module == PMU_POWER_PROTECT_MODULE_OTP) {
        hw_cfg_addr = PMU1809_REG_OTP_PP_HW_CFG;
        intr_cfg_addr = hw_cfg_addr;
    } else {
        hw_cfg_addr = module_addr;
        intr_cfg_addr = module_addr;
    }

#ifdef PMU_POWER_PROTECT_FORCE_HW_PMU_OFF
    mode = PMU_POWER_PROTECT_MODE_HW;
    type = PMU_POWER_PROTECT_HW_TYPE_PMU_OFF;
#endif

    pmu_read(hw_cfg_addr, &val);
    if (mode == PMU_POWER_PROTECT_MODE_HW) {
        val |= pwr_prt_cfg_p->hw_mode;
        // OTP and VBAT only support pmu off mode.
        if (!(module_addr == PMU_POWER_PROTECT_MODULE_OTP || module_addr == PMU_POWER_PROTECT_MODULE_VBAT)) {
            if (type == PMU_POWER_PROTECT_HW_TYPE_PMU_OFF) {
                // pmu off mode
                val |= pwr_prt_cfg_p->sel_pmu_off;
            } else {
                // ldo off mode
                val &= ~pwr_prt_cfg_p->sel_pmu_off;
            }
        }
    } else {
        val &= ~pwr_prt_cfg_p->hw_mode;
    }
    pmu_write(hw_cfg_addr, val);

    pmu_read(intr_cfg_addr, &val);
    if (cfg->irq_en) {
        val |= pwr_prt_cfg_p->intr_en | pwr_prt_cfg_p->intr_msk;
    } else {
        val &= ~(pwr_prt_cfg_p->intr_en | pwr_prt_cfg_p->intr_msk);
    }
    pmu_write(intr_cfg_addr, val);
    int_unlock(lock);

    return PMU_POWER_PROTECT_RET_VALID;
}

enum PMU_POWER_PROTECT_RET_T pmu_power_protect_irq_handler_set(PMU_POWER_PROTECT_ABORT_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    if (pmu_type != PMU_CHIP_TYPE_1809) {
        return PMU_POWER_PROTECT_RET_INVALID_CHIP;
    }

    if (pmu_pwr_protect_cur_en == false && pmu_pwr_protect_volt_en == false) {
        return PMU_POWER_PROTECT_RET_INVALID_EFUSE;
    }

    pmu_power_protect_abort_irq_handler = handler;

    lock = int_lock();
    if (handler)
    pmu_read(PMU1809_REG_INTR_MSK, &val);
    if (handler) {
        val |= POWER_ABORT_INTR_MSK;
    } else {
        val &= ~POWER_ABORT_INTR_MSK;
    }
    pmu_write(PMU1809_REG_INTR_MSK, val);
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_POWER_ABORT, handler ? _pmu_power_protect_irq_handler : NULL);
    int_unlock(lock);

    return PMU_POWER_PROTECT_RET_VALID;
}

enum PMU_POWER_PROTECT_ABORT_SOURCE_T pmu_power_protect_abort_source_get(enum PMU_POWER_PROTECT_MODULE_T module)
{
    uint32_t lock;
    uint16_t source_addr;
    uint16_t val;
    enum PMU_POWER_PROTECT_ABORT_SOURCE_T source;
    const struct PMU_POWER_PROTECT_ABORT_SOURCE_VAL_T *abort_source_p = &pmu_power_protect_abort_source[module];

    if (pmu_type != PMU_CHIP_TYPE_1809 || (pmu_pwr_protect_cur_en == false && pmu_pwr_protect_volt_en == false)) {
        return PMU_POWER_PROTECT_ABORT_SOURCE_UNKNOW;
    }

    lock = int_lock();
    source_addr = pmu_power_protect_abort_source_addr_get(module);

    pmu_read(source_addr, &val);
    source = (val & abort_source_p->abort_source_mask) >> abort_source_p->abort_source_shift;
#ifdef PMU_POWER_PROTECT_VOLT_DISABLE
    if (source == PMU_POWER_PROTECT_ABORT_SOURCE_UV || source == PMU_POWER_PROTECT_ABORT_SOURCE_OV) {
        source = PMU_POWER_PROTECT_ABORT_SOURCE_UNKNOW;
    }
#endif
    int_unlock(lock);

    return (enum PMU_POWER_PROTECT_ABORT_SOURCE_T)source;
}

unsigned short pmu_reg_val_add(unsigned short val, int delta, unsigned short max)
{
    int result = val + delta;

    if (result > max) {
        result = max;
    } else if (result < 0) {
        result = 0;
    }

    return (unsigned short)result;
}

#ifdef PMU_LDO_VCORE_CALIB
union LDO_DIG_COMP_T {
    struct LDO_DIG_COMP_FIELD_T {
        uint16_t dig_l_v: 5;     //vcore0p6:    bit[4:0]: 0 ~ 31
        uint16_t dig_l_f: 1;     //bit[5]  : 1: negative, 0: positive
        uint16_t dig_m_v: 5;     //vcore0p8_1:  bit[10:6]: 0 ~ 31
        uint16_t dig_m_f: 1;     //bit[11] : 1: negative, 0: positive
        uint16_t reserved:4;     //reserved:    [15:12]
    } f;
    uint16_t v;
};

static int8_t pmu_ldo_dig_m_comp = 0;
static int8_t pmu_ldo_dig_l_comp = 0;
static int8_t pmu_ldo_lp_m_comp = 0;
static int8_t pmu_ldo_lp_l_comp = 0;

void pmu_get_ldo_dig_calib_value(void)
{
    if (pmu_type == PMU_CHIP_TYPE_1805 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
        return;
    }

    union LDO_DIG_COMP_T lg;

    pmu_get_efuse(PMU_EFUSE_PAGE_LDO_CALIB, &lg.v);

    if (lg.f.dig_m_f) {
        pmu_ldo_dig_m_comp = -(int8_t)(lg.f.dig_m_v);
    } else {
        pmu_ldo_dig_m_comp =  (int8_t)(lg.f.dig_m_v);
    }

    if (lg.f.dig_l_f) {
        pmu_ldo_dig_l_comp = -(int8_t)(lg.f.dig_l_v);
    } else {
        pmu_ldo_dig_l_comp =  (int8_t)(lg.f.dig_l_v);
    }

    pmu_get_efuse(PMU_EFUSE_PAGE_LDO_SMALL_BG, &lg.v);
    if (lg.f.dig_m_f) {
        pmu_ldo_lp_m_comp = -(int8_t)(lg.f.dig_m_v);
    } else {
        pmu_ldo_lp_m_comp =  (int8_t)(lg.f.dig_m_v);
    }

    if (lg.f.dig_l_f) {
        pmu_ldo_lp_l_comp = -(int8_t)(lg.f.dig_l_v);
    } else {
        pmu_ldo_lp_l_comp =  (int8_t)(lg.f.dig_l_v);
    }

#ifdef DIG_DCDC_MODE
    pmu_ldo_dig_m_comp = 0;
#else
    if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1806) {
        // Add 50mV offset to overcome the voltage instant drop when decreasing vcore
        pmu_ldo_dig_m_comp += 2;
    }
#endif

#ifdef FORCE_BIG_BANDGAP
    pmu_ldo_lp_m_comp = pmu_ldo_dig_m_comp;
    pmu_ldo_lp_l_comp = pmu_ldo_dig_l_comp;
#elif defined(PMU_FORCE_LP_MODE)
    pmu_ldo_dig_m_comp = pmu_ldo_lp_m_comp;
    pmu_ldo_dig_l_comp = pmu_ldo_lp_l_comp;
#endif

    TRACE(0, "pmu_ldo_dig_m_comp:%d", pmu_ldo_dig_m_comp);
    TRACE(0, "pmu_ldo_dig_l_comp:%d", pmu_ldo_dig_l_comp);
    TRACE(0, "pmu_ldo_lp_m_comp:%d", pmu_ldo_lp_m_comp);
    TRACE(0, "pmu_ldo_lp_l_comp:%d", pmu_ldo_lp_l_comp);
}

#endif

#ifdef PMU_DCDC_CALIB
union VOLT_COMP_T {
    struct VOLT_COMP_FIELD_T {
        uint16_t dcdc1_v: 5; //bit[4:0]: 0 ~ 31
        uint16_t dcdc1_f: 1; //bit[5]  : 1: negative, 0: positive;
        uint16_t dcdc2_v: 4; //bit[9:6]: 0 ~ 15
        uint16_t dcdc2_f: 1; //bit[10] :
        uint16_t dcdc3_v: 4; //bit[14:11]: 0 ~ 15
        uint16_t dcdc3_f: 1; //bit[15]
    } f;
    uint16_t v;
};

static int8_t pmu_dcdc_dig_comp = 0;
static int8_t pmu_dcdc_ana_comp = 0;
static int8_t pmu_dcdc_hppa_comp = 0;

static int8_t pmu_dig_lp_comp  = 0;
static int8_t pmu_ana_lp_comp  = 0;
static int8_t pmu_hppa_lp_comp = 0;

static POSSIBLY_UNUSED void pmu_get_dcdc_calib_value(void)
{
    union VOLT_COMP_T cv;

    pmu_get_efuse(PMU_EFUSE_PAGE_DCDC_CALIB, &cv.v);
    if (cv.f.dcdc1_f) { //digital
        pmu_dcdc_dig_comp = -(int8_t)(cv.f.dcdc1_v);
    } else {
        pmu_dcdc_dig_comp = (int8_t)(cv.f.dcdc1_v);
    }
    if (cv.f.dcdc2_f) { //ana
        pmu_dcdc_ana_comp = -(int8_t)(cv.f.dcdc2_v);
    } else {
        pmu_dcdc_ana_comp = (int8_t)(cv.f.dcdc2_v);
    }
    if (cv.f.dcdc3_f) { //hppa
        pmu_dcdc_hppa_comp = -(int8_t)(cv.f.dcdc3_v);
    } else {
        pmu_dcdc_hppa_comp = (int8_t)(cv.f.dcdc3_v);
    }

    pmu_get_efuse(PMU_EFUSE_PAGE_DCDC_SMALL_BG, &cv.v);
    if (cv.f.dcdc1_f) { //digital
        pmu_dig_lp_comp = -(int8_t)(cv.f.dcdc1_v);
    } else {
        pmu_dig_lp_comp = (int8_t)(cv.f.dcdc1_v);
    }
    if (cv.f.dcdc2_f) { //ana
        pmu_ana_lp_comp = -(int8_t)(cv.f.dcdc2_v);
    } else {
        pmu_ana_lp_comp = (int8_t)(cv.f.dcdc2_v);
    }
    if (cv.f.dcdc3_f) { //hppa
        pmu_hppa_lp_comp = -(int8_t)(cv.f.dcdc3_v);
    } else {
        pmu_hppa_lp_comp = (int8_t)(cv.f.dcdc3_v);
    }

    if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
        // target volt 1.725V normal
        // target volt 1.705V lp
        pmu_hppa_lp_comp += 3;
    }

#ifdef FORCE_BIG_BANDGAP
    pmu_dig_lp_comp  = pmu_dcdc_dig_comp;
    pmu_ana_lp_comp  = pmu_dcdc_ana_comp;
    pmu_hppa_lp_comp = pmu_dcdc_hppa_comp;
#elif defined(PMU_FORCE_LP_MODE)
    pmu_dcdc_dig_comp  = pmu_dig_lp_comp;
    pmu_dcdc_ana_comp  = pmu_ana_lp_comp;
    pmu_dcdc_hppa_comp = pmu_hppa_lp_comp;
#endif

    TRACE(0, "pmu_dcdc_dig_comp:%d", pmu_dcdc_dig_comp);
    TRACE(0, "pmu_dcdc_ana_comp:%d", pmu_dcdc_ana_comp);
    TRACE(0, "pmu_dcdc_hppa_comp:%d", pmu_dcdc_hppa_comp);
    TRACE(0, "pmu_dig_lp_comp:%d", pmu_dig_lp_comp);
    TRACE(0, "pmu_ana_lp_comp:%d", pmu_ana_lp_comp);
    TRACE(0, "pmu_hppa_lp_comp:%d", pmu_hppa_lp_comp);
}
#endif

int BOOT_TEXT_SRAM_LOC pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
    return hal_cmu_get_aonpmu_efuse(page, efuse);
}

POSSIBLY_UNUSED
static int BOOT_TEXT_FLASH_LOC pmu_read_efuse_block(unsigned int block)
{
    int ret;
    unsigned short val;
    uint32_t start;
    uint32_t timeout = MS_TO_TICKS(50);

#ifdef NO_ISPI
    return 0;
#endif

    val = TSMC_EFUSE_PGENB | TSMC_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        return ret;
    }

    val = TSMC_EFUSE_STROBE_WIDTH(20) | REG_READ_STROBE_WIDTH(4) | EFUSE_SEL(block);
    ret = pmu_write(PMU_REG_EFUSE_SEL, val);
    if (ret) {
        return ret;
    }

    ret = pmu_read(PMU_REG_POWER_OFF, &val);
    if (ret) {
        return ret;
    }
    val = SET_BITFIELD(val, EFUSE_READ_TRIG, EFUSE_READ_TRIG_WORD);
    ret = pmu_write(PMU_REG_POWER_OFF, val);
    if (ret) {
        return ret;
    }

    hal_sys_timer_delay(US_TO_TICKS(500));

    start = hal_sys_timer_get();
    do {
        ret = pmu_read(PMU_REG_POWER_OFF, &val);
        if (ret) {
            return ret;
        }
        if (hal_sys_timer_get() - start > timeout) {
            return -2;
        }
    } while ((val & (EFUSE_READ_DONE | EFUSE_READ_BUSY)) != EFUSE_READ_DONE);

    val = TSMC_EFUSE_PGENB;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        return ret;
    }

    return 0;
}

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
uint32_t BOOT_TEXT_FLASH_LOC read_hw_metal_id(void)
{
    uint16_t val;
    uint32_t metal_id;
    uint32_t chip_id;

    pmu_read(PMU_REG_METAL_ID, &val);

    chip_id = GET_BITFIELD(val, CHIPID);
    if (chip_id == 0x183) {
        pmu_type = PMU_CHIP_TYPE_1803;
    } else if (chip_id == 0x185) {
        pmu_type = PMU_CHIP_TYPE_1805;
    } else if (chip_id == 0x186) {
        pmu_type = PMU_CHIP_TYPE_1806;
    } else if (chip_id == 0x188) {
        pmu_type = PMU_CHIP_TYPE_1808;
    } else if (chip_id == 0x189) {
        pmu_type = PMU_CHIP_TYPE_1809;
    }

    pmu_metal_id = GET_BITFIELD(val, REVID);

    metal_id = hal_cmu_get_aon_revision_id();
#ifndef NO_METAL_ID_1
    if (metal_id == HAL_CHIP_METAL_ID_0) {
#ifdef CHECK_METAL_ID_BY_PMU_TYPE
        if (pmu_type != PMU_CHIP_TYPE_1803)
#endif
        {
            metal_id = HAL_CHIP_METAL_ID_1;
        }
    }
#endif

    return metal_id;
}
#endif

BOOT_TEXT_SRAM_LOC
void pmu_enable_vmem2(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        hal_sys_timer_delay_us(400);
        hal_cmu_boot_flash_reset_pulse();
        hal_sys_timer_delay_us(2);
    }

    pmu_read(PMU1806_REG_LDO_VMEM2, &val);
    val &= ~PMU1806_REG_LDO_VMEM2_BYPASS;
    pmu_write(PMU1806_REG_LDO_VMEM2, val);

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        hal_sys_timer_delay_us(10000);
    }
}

static void BOOT_TEXT_FLASH_LOC pmu_power_on(bool on)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU1806_REG_RESETN_MODE, &val);
        if (on) {
            val |= PMU1806_CHIP_PWR_ON;
        } else {
            val &= ~PMU1806_CHIP_PWR_ON;
        }
        pmu_write(PMU1806_REG_RESETN_MODE, val);
    }
}

static void BOOT_TEXT_FLASH_LOC pmu_boot_cause_init_software(void)
{
    uint16_t reset_val = 0;
    uint16_t wdt_val = 0;
    uint16_t rtc_val = 0;
    uint16_t chr_val = 0;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU1806_REG_RESETN_MODE, &reset_val);
        pmu_read(PMU_REG_WDT_CFG, &wdt_val);
        pmu_read(PMU1806_REG_INT_CLR, &rtc_val);
        pmu_read(PMU_REG_CHARGER_STATUS, &chr_val);

        if (reset_val & PMU1806_PWRKEY_LONGPRESS) {
            pmu_boot_reason = PMU_BOOT_CAUSE_LONG_PRESS;
        } else if (reset_val & PMU1806_CHIP_PWR_ON) {
            if (wdt_val & (REG_WDT_RESET_EN | REG_WDT_EN)) {
                pmu_boot_reason = PMU_BOOT_CAUSE_PMU_WDT;
            } else {
                pmu_boot_reason = PMU_BOOT_CAUSE_RESET_KEY;
            }
        } else if (rtc_val & RTC_INT_0) {
            pmu_boot_reason = PMU_BOOT_CAUSE_RTC;
        } else if (chr_val & AC_ON) {
            pmu_boot_reason = PMU_BOOT_CAUSE_AC_IN;
        } else {
            pmu_boot_reason = PMU_BOOT_CAUSE_POWER_KEY;
        }
    }

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_boot_first_pwr_up = !!(reset_val & PMU1806_HW_RESET);
#if !(defined(OTA_BOOT_IMAGE) || defined(ARM_CMSE))
        reset_val &= ~(PMU1806_HW_RESET | PMU1806_PWRKEY_LONGPRESS);
        reset_val |= PMU1806_CHIP_PWR_ON;
        pmu_write(PMU1806_REG_RESETN_MODE, reset_val);
#endif
    }
}

static void BOOT_TEXT_FLASH_LOC pmu_boot_cause_init(void)
{
    uint16_t val;
    uint16_t val_reason;
    union HAL_HW_BOOTMODE_T hw_bm;

    hw_bm = hal_hw_bootmode_get();

    if (hw_bm.watchdog) {
        pmu_boot_reason = PMU_BOOT_CAUSE_DIG_WDT;
    } else if (hw_bm.global) {
        pmu_boot_reason = PMU_BOOT_CAUSE_DIG_REBOOT;
    } else {
        if (pmu_type == PMU_CHIP_TYPE_1809) {
            pmu_read(PMU1809_REG_BOOT_SOURCE, &val);
            val_reason = GET_BITFIELD(val, ABORT_SOURCE);
            if (val_reason) {
                if (val_reason == 1) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_PMU_WDT;
                } else if (val_reason == 2) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_LONG_PRESS;
                } else if (val_reason == 3) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_VCHG_COMMAND;
                } else if (val_reason == 4) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_RESET_KEY;
                }
            } else {
                val_reason = GET_BITFIELD(val, PMU_LDO_ON_SOURCE);
                if (val_reason == 1) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_POWER_KEY;
                } else if (val_reason == 2) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_RTC;
                } else if (val_reason == 3) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_AC_IN;
                } else if (val_reason == 4) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_AC_OUT;
                }
            }
            if (pmu_boot_reason != PMU_BOOT_CAUSE_NULL) {
                val = ABORT_SOURCE_CLR | PMU_LDO_ON_SOURCE_CLR;
                pmu_write(PMU1809_REG_BOOT_SOURCE, val);
                // One lpo clock
                hal_sys_timer_delay_us(30);
                pmu_write(PMU1809_REG_BOOT_SOURCE, val);
            }
        } else if (pmu_type == PMU_CHIP_TYPE_1808) {
            pmu_read(PMU1808_REG_ABORT_SOURCE, &val);
            val_reason = GET_BITFIELD(val, PMU1808_ABORT_SOURCE);
            if (val_reason) {
                if (val_reason == 1) {
                    // wdt/longpress.
                    // The long press reset function is not used to ensure that this reason is reliable.
                    pmu_boot_reason = PMU_BOOT_CAUSE_PMU_WDT;
                } else if (val_reason == 2) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_VCHG_COMMAND;
                } else if (val_reason == 3) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_RESET_KEY;
                } else if (val_reason == 4) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_VBAT_OVP;
                } else if (val_reason == 5) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_VCORE_LOW;
                }
            } else {
                pmu_read(PMU1808_REG_LDO_ON_SOURCE, &val);
                val_reason = GET_BITFIELD(val, PMU1808_LDO_ON_SOURCE);
                if (val_reason == 1) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_POWER_KEY;
                } else if (val_reason == 2) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_RTC;
                } else if (val_reason == 3) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_AC_IN;
                } else if (val_reason == 4) {
                    pmu_boot_reason = PMU_BOOT_CAUSE_AC_OUT;
                }
            }
        } else {
            return pmu_boot_cause_init_software();
        }
    }
}

enum PMU_BOOT_CAUSE_T pmu_boot_cause_get(void)
{
    return pmu_boot_reason;
}

bool pmu_boot_first_power_up(void)
{
    return pmu_boot_first_pwr_up;
}

void BOOT_TEXT_FLASH_LOC pmu_boot_init(void)
{
#ifdef NO_ISPI
    return;
#endif

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        uint16_t val;

        // Disable abort irq mask by default
        pmu_read(PMU1809_REG_INTR_MSK, &val);
        val &= ~POWER_ABORT_INTR_MSK;
        pmu_write(PMU1809_REG_INTR_MSK, val);
    }

    pmu_boot_cause_init();

#ifdef RTC_ENABLE
    // RTC will be restored in pmu_open()
    pmu_rtc_save_context();
#endif

#ifdef RTC_CALENDAR
    if (pmu_rtc_calendar_enabled()) {
        pmu_rtc_calendar_save_context();
    }
#endif

#ifdef __WATCHER_DOG_RESET__
    pmu_wdt_save_context();
#endif

#if !defined(FPGA) && !defined(PROGRAMMER)
    pmu_charger_save_context();
#endif

    // Reset PMU (to recover from a possible insane state, e.g., ESD reset)
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

#ifdef __WATCHER_DOG_RESET__
    pmu_wdt_restore_context();
#else
    pmu_wdt_stop();
#endif

    // TODO: Add 1806 efuse handling
    if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
        int ret;
        int i;

        hal_cmu_boot_dcdc_clock_enable();
        for (i = 0; i < EFUSE_BLOCK_NUM; i++) {
            ret = pmu_read_efuse_block(i);
            if (ret) {
                SAFE_PROGRAM_STOP();
            }
        }
        hal_cmu_boot_dcdc_clock_disable();
    }

    pmu_rf_ana_init();
}

#endif // !NO_EXT_PMU

void bbpll_pll_update(uint64_t pll_cfg_val)
{
#ifndef BBPLL_FIXED_FREQ
    uint16_t v[3];

    v[0] = pll_cfg_val & 0xFFFF;
    v[1] = (pll_cfg_val >> 16) & 0xFFFF;
    v[2] = (pll_cfg_val >> 32) & 0xFFFF;

    pmu_write(PMU_REG_RF_EC, v[0]);
    pmu_write(PMU_REG_RF_ED, v[1]);
    pmu_write(PMU_REG_RF_EE, v[2]);
    pmu_write(PMU_REG_RF_F0, REG_BT_BBPLL_FREQ_EN);
#endif
}

void bbpll_set_dig_div(uint32_t div)
{
#ifndef BBPLL_FIXED_FREQ
    uint16_t val;

    if (div == 1 || div == 2) {
        pmu_read(PMU_REG_RF_C5, &val);
        if (div == 1) {
            val |= REG_BBPLL_PSTDIV_SEL_DIGCLK;
        } else {
            val &= ~REG_BBPLL_PSTDIV_SEL_DIGCLK;
        }
        pmu_write(PMU_REG_RF_C5, val);
    }
#endif
}

void bbpll_set_ires(uint32_t ires)
{
    uint16_t val;
    pmu_read(PMU_REG_RF_C4, &val);
    val = (val & ~(0x1F << 5)) | ((ires & 0x1F) << 5);
    pmu_write(PMU_REG_RF_C4, val);
}

void bbpll_auto_calib_ires(uint32_t codec_freq, uint32_t div)
{
    const uint32_t sys_freq_div = 4;
    const uint32_t f_med = codec_freq * div / sys_freq_div
#ifndef AUD_PLL_DOUBLE
        / 2
#endif
        ;
    const uint32_t calc_ms = 50;
    const float deviation = (float)f_med / hal_cmu_get_crystal_freq() * (1.0f / CONFIG_FAST_SYSTICK_HZ) / (calc_ms / 1000.0f);
    const uint32_t f_min = f_med - (uint32_t)(f_med * deviation * 1.6f);
    const uint32_t f_max = f_med + (uint32_t)(f_med * deviation * 1.2f);
    uint32_t f;
    uint32_t k = 0;
    uint32_t valid_cnt = 0;
    uint32_t valid_start = 0;
    uint32_t valid_end = 0;
    uint32_t cur_cnt = 0;
    uint32_t sel;
    uint32_t lock;

    lock = int_lock();

    const enum HAL_CMU_FREQ_T org_freq = hal_sysfreq_get_hw_freq();
    // Switch to 52M to avoid using PLL
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
    // Enable PLL
#ifdef ULTRA_LOW_POWER
    hal_cmu_low_freq_mode_disable(org_freq, HAL_CMU_FREQ_208M);
#endif
    // Inc vcore voltage
    pmu_sys_freq_config(HAL_CMU_FREQ_208M);

    analog_aud_freq_pll_config(codec_freq, div);

    hal_cmu_force_sys_pll_div(sys_freq_div);

    for (k = 0x0A; k < 0x20; k++) {
        // Switch to 52M to avoid using PLL
        hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);

        bbpll_set_ires(k);

        // Switch to PLL freq
        hal_cmu_sys_set_freq(HAL_CMU_FREQ_208M);

        f = hal_sys_timer_calc_cpu_freq(calc_ms, 1);
        if (f_min < f && f < f_max) {
            cur_cnt++;
        } else {
            if (cur_cnt) {
                if (cur_cnt > valid_cnt) {
                    valid_cnt = cur_cnt;
                    valid_end = k - 1;
                }
                cur_cnt = 0;
            }
        }
        TRACE(0, "[%c%2u] f=%u min=%u max=%u", cur_cnt ? '+' : '-', k, f, f_min, f_max);
    }

    if (cur_cnt) {
        if (cur_cnt > valid_cnt) {
            valid_cnt = cur_cnt;
            valid_end = k - 1;
        }
    }
    if (valid_cnt) {
        valid_start = valid_end + 1 - valid_cnt;
        sel = (valid_start + valid_end) / 2;
        TRACE(0, "Valid ires=[%2u, %2u] cnt=%2u sel=%2u/0x%02X", valid_start, valid_end, valid_cnt, sel, sel);
    } else {
        sel = 0x1A;
        TRACE(0, "BAD: no valid ires");
    }
    bbpll_set_ires(sel);

    // Restore the sys freq
    hal_cmu_force_sys_pll_div(0);
    hal_cmu_sys_set_freq(org_freq);
    // Restore PLL status
#ifdef ULTRA_LOW_POWER
    hal_cmu_low_freq_mode_enable(HAL_CMU_FREQ_208M, org_freq);
#endif
    // Restore vcore voltage
    pmu_sys_freq_config(org_freq);

    int_unlock(lock);
}

void BOOT_TEXT_FLASH_LOC pmu_rf_ana_init(void)
{
    uint16_t val;

#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    // Reset RF
    pmu_write(PMU_REG_RF_00, 0xCAFE);
    pmu_write(PMU_REG_RF_00, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Reset ANA
    pmu_write(PMU_REG_ANA_00, 0xCAFE);
    pmu_write(PMU_REG_ANA_00, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Reset 1809_ANA
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_write(PMU_REG_1809_ANA_00, 0xCAFE);
        pmu_write(PMU_REG_1809_ANA_00, 0x5FEE);
        hal_sys_timer_delay(US_TO_TICKS(500));

        pmu_read(PMU_REG_1809_ANA_6D, &val);
        val &= ~(SOFT_RESETN_CAPSENSOR | SOFT_RESETN_CAPSENSOR_REG);
        pmu_write(PMU_REG_1809_ANA_6D, val);
        hal_sys_timer_delay(US_TO_TICKS(500));
    }
#endif

#ifdef AUD_PLL_DOUBLE
#if defined(BBPLL_MHZ) && (BBPLL_MHZ != 400)
#error "BBPLL_MHZ should be 400 when AUD_PLL_DOUBLE defined"
#endif
#ifndef BBPLL_MHZ
#define BBPLL_MHZ                           400
#endif
#endif
    // Init bbpll
#if defined(BBPLL_MHZ) && (BBPLL_MHZ != 200)
#if 0
#elif (BBPLL_MHZ == 440)
    pmu_write(PMU_REG_RF_C4, 0x9FD4);
    pmu_write(PMU_REG_RF_C5, 0x020F);
    pmu_write(PMU_REG_RF_EC, 0x0000);
    pmu_write(PMU_REG_RF_ED, 0x2000);
    pmu_write(PMU_REG_RF_EE, 0x0001);
#elif (BBPLL_MHZ == 400)
    pmu_write(PMU_REG_RF_C4, 0x9F94);
#if 1
    // 393.216M
    pmu_write(PMU_REG_RF_C5, 0x020F);
    pmu_write(PMU_REG_RF_EC, 0xDD2F);
    pmu_write(PMU_REG_RF_ED, 0x0624);
    pmu_write(PMU_REG_RF_EE, 0x0001);
#else
    // 392M
    pmu_write(PMU_REG_RF_C5, 0x020F);
    pmu_write(PMU_REG_RF_EC, 0x0000);
    pmu_write(PMU_REG_RF_ED, 0x0000);
    pmu_write(PMU_REG_RF_EE, 0x0001);
#endif
#elif (BBPLL_MHZ == 384)
    pmu_write(PMU_REG_RF_C4, 0x9F14);
    pmu_write(PMU_REG_RF_C5, 0x020F);
    pmu_write(PMU_REG_RF_EC, 0x0000);
    pmu_write(PMU_REG_RF_ED, 0x0000);
    pmu_write(PMU_REG_RF_EE, 0x0001);
#elif (BBPLL_MHZ == 360)
    pmu_write(PMU_REG_RF_C4, 0x9F14);
    pmu_write(PMU_REG_RF_C5, 0x020F);
    pmu_write(PMU_REG_RF_EC, 0x0000);
    pmu_write(PMU_REG_RF_ED, 0xF000);
    pmu_write(PMU_REG_RF_EE, 0x0000);
#elif (BBPLL_MHZ == 320)
    pmu_write(PMU_REG_RF_C4, 0x9E14);
    pmu_write(PMU_REG_RF_C5, 0x020F);
    pmu_write(PMU_REG_RF_EC, 0x5555);
    pmu_write(PMU_REG_RF_ED, 0xD555);
    pmu_write(PMU_REG_RF_EE, 0x0000);
#else
#error "Bad BBPLL_MHZ"
#endif
#else
    // BBPLL_MHZ == 200
    pmu_write(PMU_REG_RF_C4, 0x9F94);
    // reg_bbpll_pstdiv_sel_digclk=0 to enable div2
    pmu_write(PMU_REG_RF_C5, 0x0207);
    pmu_write(PMU_REG_RF_EC, 0xDD2F);
    pmu_write(PMU_REG_RF_ED, 0x0624);
    pmu_write(PMU_REG_RF_EE, 0x0001);
#endif
    pmu_write(PMU_REG_RF_F0, REG_BT_BBPLL_FREQ_EN);

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806) {
        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            // for vcm
            pmu_read(PMU_REG_RESERVED_PMU, &val);
            val &= ~PMU_REG_RESERVED_BIT9;
            pmu_write(PMU_REG_RESERVED_PMU, val);
        }
#ifdef SPLIT_VMEM_VMEM2
        uint16_t vmem2_volt;
        uint16_t pmu_vcm_reg_val;

        if (pmu_type == PMU_CHIP_TYPE_1805) {
            vmem2_volt = (vcodec_mv <= 1750) ? 0x1C : 0x1A;
        } else {
            vmem2_volt = (vcodec_mv <= 1750) ? 0x16 : 0x14;
        }

        // VCM voltage setting.
        if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            pmu_vcm_reg_val = REG_VCM_LOW_VCM(7) | REG_VCM_LOW_VCM_LP(7) | REG_VCM_LOW_VCM_LPF(7);
            pmu_write(PMU1806_REG_CODEC_VCM, pmu_vcm_reg_val);
        }
        pmu_read(PMU_REG_ANA_F9, &val);
        val |= REG_CODEC_VCM_LOW_VCM_LP(7) | REG_CODEC_VCM_LOW_VCM_LPF(7) | REG_CODEC_VCM_LOW_VCM(7);
        pmu_write(PMU_REG_ANA_F9, val);

        // VCM startup.
        pmu_read(PMU_REG_ANA_F6, &val);
        val &= ~(REG_CODEC_LP_VCM | REG_CODEC_VCM_EN_LPF);
        val |= REG_CODEC_EN_VCM;
        pmu_write(PMU_REG_ANA_F6, val);
        if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            pmu_vcm_reg_val |= REG_VCM_EN;
            pmu_write(PMU1806_REG_CODEC_VCM, pmu_vcm_reg_val);
        }
        hal_sys_timer_delay(MS_TO_TICKS(10));

        // VCM lpf mode enable.
        val |= REG_CODEC_VCM_EN_LPF;
        pmu_write(PMU_REG_ANA_F6, val);
        if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            pmu_vcm_reg_val |= REG_VCM_EN_LPF;
            pmu_write(PMU1806_REG_CODEC_VCM, pmu_vcm_reg_val);
        }

        // VMEM2 voltage setting.
        pmu_read(PMU_REG_CODEC_CFG, &val);
        val = SET_BITFIELD(val, LDO_VCODEC_VBIT_DSLEEP, vmem2_volt);
        val = SET_BITFIELD(val, LDO_VCODEC_VBIT_NORMAL, vmem2_volt);
        pmu_write(PMU_REG_CODEC_CFG, val);

        // VMEM2 startup.
        pmu_read(PMU1806_REG_LDO_VMEM2, &val);
        val |= (PMU1806_REG_PU_LDO_VMEM2_DR | PMU1806_REG_PU_LDO_VMEM2_REG);
        val |= PMU1806_REG_PU_LDO_VMEM2_RC_SLP;
        val |= PMU1806_REG_PU_LDO_VMEM2_DSLEEP;
        pmu_write(PMU1806_REG_LDO_VMEM2, val);

        pmu_enable_vmem2();
#else
        // Disable VCM by default
        val = REG_VCM_LOW_VCM(7) | REG_VCM_LOW_VCM_LP(7) | REG_VCM_LOW_VCM_LPF(7);
        if (pmu_type == PMU_CHIP_TYPE_1805 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            // 1805 metal-0 needs to separate dcdc3 and vmem2
            val |= REG_VCM_EN;
        }
        pmu_write(PMU1806_REG_CODEC_VCM, val);
#endif
    }

    return;
}

#ifdef PMU_RF_WORKAROUND
#define RF_REG_BT_RFPLL_CLK_FBC_EDGE_RX                     (1 << 15)
#define RF_PLL_CLK_EDGE_THRESHOLD_VCORE_MV                  750

static void pmu_rf_pll_clk_edge_workaround(uint32_t vcore_mv)
{
    uint16_t val;
    bool negedge;

    // 0=posedge 1=negedge
    pmu_read(PMU_REG_RF_28, &val);
    negedge = !!(val & RF_REG_BT_RFPLL_CLK_FBC_EDGE_RX);

    if (negedge && (vcore_mv < RF_PLL_CLK_EDGE_THRESHOLD_VCORE_MV)) {
        val &= ~RF_REG_BT_RFPLL_CLK_FBC_EDGE_RX;
        pmu_write(PMU_REG_RF_28, val);
    } else if (!negedge && (vcore_mv >= RF_PLL_CLK_EDGE_THRESHOLD_VCORE_MV)) {
        val |= RF_REG_BT_RFPLL_CLK_FBC_EDGE_RX;
        pmu_write(PMU_REG_RF_28, val);
    }
}
#endif

int pmu_get_security_value(union SECURITY_VALUE_T *val)
{
    int ret;

    val->reg = 0;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &val->reg);
    if (ret) {
        // Error
        return ret;
    }

    return 0;
}

enum HAL_FLASH_ID_T pmu_get_boot_flash_ctrl_id(void)
{
#ifdef FLASH1_CTRL_BASE
    union SECURITY_VALUE_T security;

    pmu_get_security_value(&security);

    return (security.root.flash_id) ? HAL_FLASH_ID_1 : HAL_FLASH_ID_0;
#else
    return HAL_FLASH_ID_0;
#endif
}

#if defined(USE_CYBERON)
static uint32_t BOOT_BSS_LOC cyb_hack_efuse_check = 0;
void cyb_efuse_check_enable(int enabled)
{
    cyb_hack_efuse_check = enabled;
}

int cyb_efuse_check_status(void)
{
    return cyb_hack_efuse_check;
}

#if 1
int BOOT_TEXT_SRAM_LOC Cyb_pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
//    PMU_DEBUG_TRACE(2,"page %x, efuse %x", page, efuse);
      if(page==0XE){
          *efuse=0x1B9C;
      }
      if(page==0xF){
          *efuse = 0x098B;
      }

      return 0;
}
#endif
#endif

#ifndef NO_EXT_PMU

static int BOOT_TEXT_SRAM_LOC pmu_ext_get_efuse_1809(enum PMU_EXT_EFUSE_PAGE_T page, unsigned short *efuse)
{
    int ret;

    if (page <= PMU_EXT_EFUSE_PAGE_RESERVED_7) {
        ret = pmu_read(PMU1809_REG_EFUSE_VAL_START + page, efuse);
    } else {
        *efuse = 0;
        ret = -2;
    }

    return ret;
}

int BOOT_TEXT_SRAM_LOC pmu_ext_get_efuse(enum PMU_EXT_EFUSE_PAGE_T page, unsigned short *efuse)
{
    int ret;

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        ret = pmu_ext_get_efuse_1809(page, efuse);
    } else if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
        ret = pmu_read(PMU_REG_EFUSE_VAL_START + page, efuse);
    } else {
        ret = -1;
    }

    return ret;
}

static void pmu_xo32k_ldo_pu_enable(void)
{
#ifndef PMU_CLK_USE_EXT_CRYSTAL
    uint16_t val;

    // Optimize the current
    if (pmu_type == PMU_CHIP_TYPE_1806) {
        //if (pmu_metal_id < HAL_CHIP_METAL_ID_3)
        {
            pmu_read(PMU1806_REG_XO_32K, &val);
            val = (val & ~REG_XO_32K_ISEL_VALUE_MASK) | REG_XO_32K_ISEL_VALUE(0) |
                REG_XO_32K_ISEL_DR | REG_XO_32K_PU_LDO_DR | REG_XO_32K_PU_LDO;
            pmu_write(PMU1806_REG_XO_32K, val);
        }
    }
#endif
}

#ifdef PMU_FORCE_LP_MODE
static void pmu_force_lp_ldo_lp_bias_enhance(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1809_REG_LP_BIAS_CFG, &val);
        val = SET_BITFIELD(val, REG_LP_BIAS_SEL_LDO_NORMAL, 0x1F);
        val = SET_BITFIELD(val, REG_LP_BIAS_SEL_LDO_RC, 0x1F);
        pmu_write(PMU1809_REG_LP_BIAS_CFG, val);
    } else  if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU1806_REG_LP_BIAS_SEL_LDO, &val);
        val = SET_BITFIELD(val, REG_1806_LP_BIAS_SEL_LDO, 0x7);
        pmu_write(PMU1806_REG_LP_BIAS_SEL_LDO, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU1805_REG_LP_BIAS_SEL_LDO, &val);
        val = SET_BITFIELD(val, REG_1805_LP_BIAS_SEL_LDO, 0xF);
        pmu_write(PMU1805_REG_LP_BIAS_SEL_LDO, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1803) {
        pmu_read(PMU_REG_HPPA_LDO_EN, &val);
        val = SET_BITFIELD(val, REG_LP_BIAS_SEL_LDO, 0x0);
        pmu_write(PMU_REG_HPPA_LDO_EN, val);
    } else {
        pmu_read(PMU_REG_HPPA_LDO_EN, &val);
        val = SET_BITFIELD(val, PMU1808_REG_LP_BIAS_SEL_LDO, 0x7);
        pmu_write(PMU_REG_HPPA_LDO_EN, val);
    }
}

static void pmu_force_lp_bandgap_config_reset(void)
{
    uint16_t val;
    uint16_t val_vtoi;
    uint16_t val_vref;

    pmu_read(PMU_REG_BIAS_CFG, &val);
    val |= PU_BIAS_LDO_DR | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_NOTCH_EN_DR | BG_VBG_SEL_DR |
        PU_BIAS_LDO_REG | BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_NOTCH_EN_REG;
    pmu_write(PMU_REG_BIAS_CFG, val);
    if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
        pmu_read(PMU1806_REG_VTOI_CFG, &val_vtoi);
        val_vtoi |= BG_VTOI_EN_DR | BG_VTOI_EN_REG;
        pmu_write(PMU1806_REG_VTOI_CFG, val_vtoi);

        if (pmu_type == PMU_CHIP_TYPE_1809) {
            pmu_read(PMU1809_REG_VBG_VREF_CFG, &val_vref);
            val_vref &= ~(BG_VREF_SEL_DR | BG_VREF_GEN_EN_DR);
            pmu_write(PMU1809_REG_VBG_VREF_CFG, val_vref);
        }
    }
    hal_sys_timer_delay_us(PMU_BIG_BG_STABLE_TIME_US);

    val |= BG_VBG_SEL_REG;
    pmu_write(PMU_REG_BIAS_CFG, val);
}

static void pmu_codec_mic_bias_noremap_force_lp_config(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1806_REG_SAR_MIC_BIAS_LP, &val);
        val |= REG_MICBIASB_LP_ENABLE | REG_MICBIASA_LP_ENABLE;
        pmu_write(PMU1806_REG_SAR_MIC_BIAS_LP, val);
    } else {
        pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
        val &= ~(REG_MIC_BIASA_IX2 | REG_MIC_BIASB_IX2 | REG_MIC_BIASC_IX2);
        pmu_write(PMU_REG_DCDC_RAMP_EN, val);

        pmu_read(PMU_REG_MIC_BIAS_A, &val);
        val = SET_BITFIELD(val, REG_MIC_BIASA_CHANSEL, 0x3);
        pmu_write(PMU_REG_MIC_BIAS_A, val);

        pmu_read(PMU_REG_MIC_BIAS_B, &val);
        val = SET_BITFIELD(val, REG_MIC_BIASB_CHANSEL, 0x3);
        pmu_write(PMU_REG_MIC_BIAS_B, val);

        pmu_read(PMU_REG_MIC_BIAS_C, &val);
        val = SET_BITFIELD(val, REG_MIC_BIASC_CHANSEL, 0x3);
        pmu_write(PMU_REG_MIC_BIAS_C, val);
    }
}
#endif

static void pmu_sys_ctrl(bool shutdown)
{
    uint16_t val;
    uint32_t lock = int_lock();

    PMU_INFO_TRACE_IMM(0, "Start pmu %s", shutdown ? "shutdown" : "reboot");

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    // Default vcore might not be high enough to support high performance mode
    pmu_high_performance_mode_enable(false);
#endif
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
    pmu_vcore_req |= PMU_VCORE_PMU_RESET;
#ifdef PMU_FORCE_LP_MODE
    pmu_force_lp_bandgap_config_reset();
#endif
    pmu_hppa_dcdc_to_ldo();
    pmu_mode_change(PMU_POWER_MODE_LDO);
    hal_sys_timer_delay(MS_TO_TICKS(1));
#endif

#ifdef RTC_ENABLE
    pmu_rtc_save_context();
#endif

#ifdef RTC_CALENDAR
    pmu_rtc_calendar_save_context();
#endif

    // Reset PMU
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    pmu_xo32k_ldo_pu_enable();

#ifdef PMU_CLK_USE_EXT_CRYSTAL
    pmu_external_crystal_ldo_ctrl(true);
    pmu_external_crystal_enable();
#endif

#ifdef RTC_ENABLE
    pmu_rtc_restore_context();
#endif

#ifdef RTC_CALENDAR
    pmu_rtc_calendar_restore_context();
#endif

    if (shutdown) {
#if defined(_AUTO_TEST_)
        if (at_skip_shutdown) {
            hal_cmu_sys_reboot();
            return;
        }
#endif

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
        if (
#if defined(RTC_ENABLE)
            rtc_ctx.enabled ||
#endif
#if defined(RTC_CALENDAR)
            pmu_rtc_calendar_enabled() ||
#endif
#if defined(CHG_OUT_PWRON)
            // ac_on will en lpo
            true ||
#endif
            false) {
            pmu_wdt_stop();
        } else {
            pmu_wdt_config(3*1000, 3*1000);
            pmu_wdt_start();
        }
#endif

        pmu_power_on(false);

        pmu_read(PMU_REG_CHARGER_CFG, &val);
        val |= REG_CHARGE_INTR_EN;
        pmu_write(PMU_REG_CHARGER_CFG,val);

        pmu_read(PMU_REG_SLEEP_CFG, &val);
#ifdef CHG_OUT_PWRON
        val |= AC_OUT_LDO_ON_EN;
#else
        val &= ~AC_OUT_LDO_ON_EN;
#endif
        pmu_write(PMU_REG_SLEEP_CFG, val);

        // Power off
        pmu_read(PMU_REG_POWER_OFF,&val);
        if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) && shipment_en) {
            val |= PMU1806_SHIPMENT_POWER_OFF;
        } else {
            val |= SOFT_POWER_OFF;
        }
        for (int i = 0; i < 100; i++) {
            pmu_write(PMU_REG_POWER_OFF,val);
            hal_sys_timer_delay(MS_TO_TICKS(5));
        }

        hal_sys_timer_delay(MS_TO_TICKS(50));

        //can't reach here
        PMU_INFO_TRACE_IMM(0, "\nError: pmu shutdown failed!\n");
        hal_sys_timer_delay(MS_TO_TICKS(5));
    } else {
#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
        // CAUTION:
        // 1) Never reset RF because system or flash might be using X2/X4, which are off by default
        // 2) Never reset RF/ANA because system or flash might be using PLL, and the reset might cause clock glitch
        // TODO:
        // Restore BBPLL settings in RF
#endif
    }

    hal_cmu_sys_reboot();

    int_unlock(lock);
}

void pmu_shutdown(void)
{
    pmu_sys_ctrl(true);
}

void pmu_reboot(void)
{
    pmu_sys_ctrl(false);
}

static inline uint16_t pmu_get_module_addr(enum PMU_MODULE_T module)
{
    if (module == PMU_HPPA2CODEC || module == PMU_CODEC) {
        return PMU_REG_MODULE_START + (enum PMU_REG_T)module + 1;
    } else if (module == PMU_BUCK2ANA) {
        return PMU_REG_BUCK2ANA_CFG;
    } else if (module == PMU_MEM2) {
        return PMU1809_REG_LDO_VMEM2_CFG;
    } else if (module == PMU_MEM3) {
        return PMU1809_REG_LDO_VMEM3_CFG;
    } else {
        return (uint16_t)PMU_REG_MODULE_START + (uint16_t)module;
    }
}

void pmu_module_config(enum PMU_MODULE_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode)
{
    unsigned short val;
    unsigned short module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        if ((module == PMU_DIG || module == PMU_GP) || (module >= PMU_HPPA2CODEC)) {
            return;
        }
    }

    if (module >= PMU_MEM2 && pmu_type != PMU_CHIP_TYPE_1809) {
        return;
    }

    module_address = pmu_get_module_addr(module);

    if (module == PMU_ANA) {
        pmu_read(PMU_REG_BIAS_CFG, &val);
        if(is_manual) {
            val |= module_cfg_p->pu_dr;
        } else {
            val &= ~module_cfg_p->pu_dr;
        }

        if(ldo_on) {
            val |= module_cfg_p->pu;
        } else {
            val &= ~module_cfg_p->pu;
        }
        pmu_write(PMU_REG_BIAS_CFG, val);
    } else if (module == PMU_DIG) {
        pmu_read(PMU_REG_DCDC_DIG_EN, &val);
        if(is_manual) {
            val |= module_cfg_p->pu_dr;
        } else {
            val &= ~module_cfg_p->pu_dr;
        }

        if(ldo_on) {
            val |= module_cfg_p->pu;
        } else {
            val &= ~module_cfg_p->pu;
        }
        pmu_write(PMU_REG_DCDC_DIG_EN, val);
    } else if (module == PMU_HPPA2CODEC) {
        pmu_read(PMU_REG_SLEEP_CFG, &val);
        if(dpmode) {
            val |= module_cfg_p->pu_dsleep;
        } else {
            val &= ~module_cfg_p->pu_dsleep;
        }
        pmu_write(PMU_REG_SLEEP_CFG, val);
    } else if (module == PMU_CODEC) {
        pmu_read(PMU_REG_HPPA2CODEC_CFG, &val);
        if(dpmode) {
            val |= module_cfg_p->pu_dsleep;
        } else {
            val &= ~module_cfg_p->pu_dsleep;
        }
        pmu_write(PMU_REG_HPPA2CODEC_CFG, val);
    }

    pmu_read(module_address, &val);

    if (module != PMU_ANA && module != PMU_DIG) {
        if(is_manual) {
            val |= module_cfg_p->pu_dr;
        } else {
            val &= ~module_cfg_p->pu_dr;
        }

        if(ldo_on) {
            val |= module_cfg_p->pu;
        } else {
            val &= ~module_cfg_p->pu;
        }
    }

#ifdef PMU_FORCE_LP_MODE
    if (pmu_power_mode != PMU_POWER_MODE_NONE) {
        val |= module_cfg_p->lp_en_dr;
        if(ldo_on) {
            val |= module_cfg_p->lp_en;
        } else {
            val &= ~module_cfg_p->lp_en;
        }
    }
#else
    if (lp_mode) {
        val &= ~module_cfg_p->lp_en_dr;
    } else {
        val = (val & ~module_cfg_p->lp_en) | module_cfg_p->lp_en_dr;
    }
#endif

    if (module != PMU_HPPA2CODEC && module != PMU_CODEC) {
        if (dpmode) {
            val |= module_cfg_p->pu_dsleep;
        } else {
            val &= ~module_cfg_p->pu_dsleep;
        }
    }

    pmu_write(module_address, val);
}

#ifdef PMU_FORCE_LP_MODE
void pmu_module_force_lp_config(void)
{
    uint16_t val, tmp;
    uint16_t module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p;
    enum PMU_MODULE_T module;
    bool ldo_on;

    for (module = 0; module < PMU_MODULE_QTY; module++) {
        if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
            if ((module == PMU_DIG || module == PMU_GP) || (module >= PMU_HPPA2CODEC)) {
                continue;
            }
        }

        if (module >= PMU_MEM2 && pmu_type != PMU_CHIP_TYPE_1809) {
            continue;
        }

        if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
            pmu_read(PMU_REG_POWER_KEY_CFG, &val);
            val |= REG_LP_EN_VHPPA_NORMAL | REG_LP_EN_VHPPA_DSLEEP;
            pmu_write(PMU_REG_POWER_KEY_CFG, val);
        }

        module_cfg_p = &pmu_module_cfg[module];
        module_address = pmu_get_module_addr(module);

        pmu_read(module_address, &val);
        if (module == PMU_ANA) {
            pmu_read(PMU_REG_BIAS_CFG, &tmp);
            ldo_on = !!(tmp & module_cfg_p->pu);
        } else if (module == PMU_DIG) {
            pmu_read(PMU_REG_DCDC_DIG_EN, &tmp);
            ldo_on = !!(tmp & module_cfg_p->pu);
        } else {
            ldo_on = !!(val & module_cfg_p->pu);
        }
        val |= module_cfg_p->lp_en_dr;
        if (ldo_on) {
            val |= module_cfg_p->lp_en;
        } else {
            val &= ~module_cfg_p->lp_en;
        }
        pmu_write(module_address, val);
    }
}
#endif

static void pmu_1808_vio_vusb_set_volt(unsigned char module, unsigned short sleep_v,unsigned short normal_v)
{
    unsigned short val;
    unsigned short volt_low;
    unsigned short volt_high;

    sleep_v = normal_v;

    if (module == PMU_IO) {
        volt_low = !!(normal_v & 0x1);
        volt_high = (normal_v & PMU1808_LDO_VUSB_VBIT_MAX) >> 1;
        pmu_read(PMU1808_REG_VIO_CFG_BB, &val);
        if (volt_low) {
            val |= REG_LDO_VIO_RES_SEL_FINE;
        } else {
            val &= ~REG_LDO_VIO_RES_SEL_FINE;
        }
        pmu_write(PMU1808_REG_VIO_CFG_BB, val);
        pmu_read(PMU_REG_IO_CFG, &val);
        val = SET_BITFIELD(val, LDO_VIO_VBIT_NORMAL, volt_high);
        val = SET_BITFIELD(val, LDO_VIO_VBIT_DSLEEP, volt_high);
        pmu_write(PMU_REG_IO_CFG, val);
    } else {
        volt_low = normal_v & 0x3;
        volt_high = (normal_v & PMU1808_LDO_VUSB_VBIT_MAX) >> 2;
        pmu_read(PMU1808_REG_VUSB_RES_SEL_FINE, &val);
        val = SET_BITFIELD(val, REG_LDO_VUSB_RES_SEL_FINE, volt_low);
        pmu_write(PMU1808_REG_VUSB_RES_SEL_FINE, val);
        pmu_read(PMU_REG_USB_CFG, &val);
        val = SET_BITFIELD(val, LDO_VUSB_VBIT_NORMAL, volt_high);
        val = SET_BITFIELD(val, LDO_VUSB_VBIT_DSLEEP, volt_high);
        pmu_write(PMU_REG_USB_CFG, val);
    }
}

void pmu_module_set_volt(unsigned char module, unsigned short sleep_v,unsigned short normal_v)
{
    unsigned short val;
    unsigned short module_address;
    unsigned short module_address_lp;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        if ((module == PMU_DIG || module == PMU_GP) || (module >= PMU_HPPA2CODEC)) {
            return;
        }
    }

    if (module >= PMU_MEM2 && pmu_type != PMU_CHIP_TYPE_1809) {
        return;
    }

    if (pmu_type == PMU_CHIP_TYPE_1808 && (module == PMU_IO || module == PMU_USB)) {
        // The working sleep voltage has multiplexed registers, so the forced working sleep voltage is equal.
        pmu_1808_vio_vusb_set_volt(module, sleep_v, normal_v);
        return;
    }

#ifdef PMU_LDO_VCORE_CALIB
    if (module == PMU_BAT2DIG) {
        normal_v = pmu_reg_val_add(normal_v, pmu_ldo_dig_m_comp, MAX_LDO_VCORE_VBIT);
        sleep_v = pmu_reg_val_add(sleep_v, pmu_ldo_lp_m_comp, MAX_LDO_VCORE_VBIT);
    }
#endif

    if (module == PMU_MEM2) {
        module_address = PMU1809_REG_LDO_VMEM2_VOLT;
    } else if (module == PMU_MEM3) {
        module_address = PMU1809_REG_LDO_VMEM3_VOLT;
    } else {
        module_address = pmu_get_module_addr((enum PMU_MODULE_T)module);
    }

    if (pmu_type == PMU_CHIP_TYPE_1809 && (module == PMU_IO || module == PMU_USB)) {
        if (module == PMU_IO) {
            module_address = PMU1809_REG_LDO_VIO_VOLT;
            module_address_lp = PMU1809_REG_LDO_VIO_LP_VOLT;
        } else {
            module_address = PMU1809_REG_LDO_VUSB_VOLT;
            module_address_lp = PMU1809_REG_LDO_VUSB_LP_VOLT;
        }
        pmu_read(module_address, &val);
        val &= ~module_cfg_p->vbit_normal_mask;
        val |= (normal_v << module_cfg_p->vbit_normal_shift) & module_cfg_p->vbit_normal_mask;
        pmu_write(module_address, val);

        pmu_read(module_address_lp, &val);
        val &= ~module_cfg_p->vbit_dsleep_mask;
        val |= (sleep_v << module_cfg_p->vbit_dsleep_shift) & module_cfg_p->vbit_dsleep_mask;
        pmu_write(module_address_lp, val);
    } else {
        pmu_read(module_address, &val);
        val &= ~module_cfg_p->vbit_normal_mask;
        val |= (normal_v << module_cfg_p->vbit_normal_shift) & module_cfg_p->vbit_normal_mask;
        val &= ~module_cfg_p->vbit_dsleep_mask;
        val |= (sleep_v << module_cfg_p->vbit_dsleep_shift) & module_cfg_p->vbit_dsleep_mask;
        pmu_write(module_address, val);
    }

    // TODO: Merge voltage setting for RC mode.
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        if (module == PMU_BAT2DIG) {
            pmu_read(PMU1806_REG_DIG_L_VOLT_RC, &val);
            val = SET_BITFIELD(val, LDO_VBAT2VCORE_VBIT_RC, normal_v);
            pmu_write(PMU1806_REG_DIG_L_VOLT_RC, val);
        } else if (module == PMU_MEM || module == PMU_USB) {
            pmu_read(PMU1806_REG_USB_VOLT_RC, &val);
            if (module == PMU_MEM) {
                val = SET_BITFIELD(val, LDO_VMEM_VBIT_RC, normal_v);
            } else {
                val = SET_BITFIELD(val, LDO_VUSB_VBIT_RC, normal_v);
            }
            pmu_write(PMU1806_REG_USB_VOLT_RC, val);
        }
    }
}

static int pmu_1808_vio_vusb_get_volt(unsigned char module, unsigned short *sleep_vp, unsigned short *normal_vp)
{
    unsigned short val;
    unsigned short volt_low;
    unsigned short volt_high;

    if (module == PMU_IO) {
        pmu_read(PMU1808_REG_VIO_CFG_BB, &val);
        volt_low = !!(val & REG_LDO_VIO_RES_SEL_FINE);
        pmu_read(PMU_REG_IO_CFG, &val);
        volt_high = GET_BITFIELD(val, LDO_VIO_VBIT_NORMAL);
        *normal_vp = (volt_high << 1) | volt_low;
    } else {
        pmu_read(PMU1808_REG_VUSB_RES_SEL_FINE, &val);
        volt_low = GET_BITFIELD(val, REG_LDO_VUSB_RES_SEL_FINE);
        pmu_read(PMU_REG_USB_CFG, &val);
        volt_high = GET_BITFIELD(val, LDO_VUSB_VBIT_NORMAL);
        *normal_vp = (volt_high << 2) | volt_low;
    }
    *sleep_vp = *normal_vp;

    return 0;
}

int pmu_module_get_volt(unsigned char module, unsigned short *sleep_vp,unsigned short *normal_vp)
{
    bool mismatch = false;
    unsigned short val;
    unsigned short module_address;
    unsigned short module_address_lp;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        if ((module == PMU_DIG || module == PMU_GP) || (module >= PMU_HPPA2CODEC)) {
            mismatch = true;
        }
    }

    if (module >= PMU_MEM2 && pmu_type != PMU_CHIP_TYPE_1809) {
        mismatch = true;
    }

    if (mismatch) {
        if (normal_vp) {
            *normal_vp = 0;
        }
        if (sleep_vp) {
            *sleep_vp = 0;
        }
        return 0;
    }

    if (pmu_type == PMU_CHIP_TYPE_1808 && (module == PMU_IO || module == PMU_USB)) {
        pmu_1808_vio_vusb_get_volt(module, sleep_vp, normal_vp);
        return 0;
    }

    if (pmu_type == PMU_CHIP_TYPE_1809 && (module == PMU_IO || module == PMU_USB)) {
        if (module == PMU_IO) {
            module_address = PMU1809_REG_LDO_VIO_VOLT;
            module_address_lp = PMU1809_REG_LDO_VIO_LP_VOLT;
        } else {
            module_address = PMU1809_REG_LDO_VUSB_VOLT;
            module_address_lp = PMU1809_REG_LDO_VUSB_LP_VOLT;
        }
        pmu_read(module_address, &val);
        if (normal_vp) {
            *normal_vp = (val & module_cfg_p->vbit_normal_mask) >> module_cfg_p->vbit_normal_shift;
        }
        pmu_read(module_address_lp, &val);
        if (sleep_vp) {
            *sleep_vp = (val & module_cfg_p->vbit_dsleep_mask) >> module_cfg_p->vbit_dsleep_shift;
        }
    } else {
        if (module == PMU_MEM2) {
            module_address = PMU1809_REG_LDO_VMEM2_VOLT;
        } else if (module == PMU_MEM3) {
            module_address = PMU1809_REG_LDO_VMEM3_VOLT;
        } else {
            module_address = pmu_get_module_addr(module);
        }

        pmu_read(module_address, &val);
        if (normal_vp) {
            *normal_vp = (val & module_cfg_p->vbit_normal_mask) >> module_cfg_p->vbit_normal_shift;
        }
        if (sleep_vp) {
            *sleep_vp = (val & module_cfg_p->vbit_dsleep_mask) >> module_cfg_p->vbit_dsleep_shift;
        }
    }

#ifdef PMU_LDO_VCORE_CALIB
    if (module == PMU_BAT2DIG) {
        if (normal_vp) {
            *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_ldo_dig_m_comp, MAX_LDO_VCORE_VBIT);
        }
        if (sleep_vp) {
            *sleep_vp = pmu_reg_val_add(*sleep_vp, -pmu_ldo_lp_m_comp, MAX_LDO_VCORE_VBIT);
        }
    }
#endif

    return 0;
}

static void pmu_module_ramp_volt(unsigned char module, unsigned short sleep_v, unsigned short normal_v)
{
    uint16_t old_normal_v;
    uint16_t old_sleep_v;
    uint8_t ramp_step;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        if ((module == PMU_DIG || module == PMU_GP) || (module >= PMU_HPPA2CODEC)) {
            return;
        }
    }

    if (module >= PMU_MEM2 && pmu_type != PMU_CHIP_TYPE_1809) {
        return;
    }

    pmu_module_get_volt(module, &old_sleep_v, &old_normal_v);

    if (pmu_type == PMU_CHIP_TYPE_1809 && (module == PMU_IO || module == PMU_USB)) {
        ramp_step = 10;
    } else {
        ramp_step = 2;
    }

    if (old_normal_v < normal_v) {
        while (old_normal_v < normal_v) {
            if (old_normal_v + ramp_step < normal_v) {
                old_normal_v += ramp_step;
            } else {
                old_normal_v = normal_v;
            }
            pmu_module_set_volt(module, sleep_v, old_normal_v);
        }
#ifndef DIG_DCDC_MODE
    } else if (old_normal_v > normal_v &&
        module == PMU_BAT2DIG &&
        (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1806)) {
        while (old_normal_v > normal_v) {
            if (old_normal_v > normal_v + ramp_step) {
                old_normal_v -= ramp_step;
            } else {
                old_normal_v = normal_v;
            }
            pmu_module_set_volt(module, sleep_v, old_normal_v);
            hal_sys_timer_delay_us(100);
        }
#endif
    } else if (old_normal_v != normal_v || old_sleep_v != sleep_v) {
        pmu_module_set_volt(module, sleep_v, normal_v);
    }
}

static void pmu_dcdc_ana_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_ANA_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_DCDC2_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_DCDC2_VBIT_DSLEEP);
    }

#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_ana_comp, MAX_DCDC2_VBIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_ana_lp_comp, MAX_DCDC2_VBIT_VAL);
    }
#endif
}

static void pmu_dcdc_ana_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_ana_comp, MAX_DCDC2_VBIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_ana_lp_comp, MAX_DCDC2_VBIT_VAL);
#endif

    pmu_read(PMU_REG_DCDC_ANA_VOLT, &val);
    val &= ~REG_DCDC2_VBIT_DSLEEP_MASK;
    val &= ~REG_DCDC2_VBIT_NORMAL_MASK;
    val |= REG_DCDC2_VBIT_DSLEEP(dsleep_v);
    val |= REG_DCDC2_VBIT_NORMAL(normal_v);
    pmu_write(PMU_REG_DCDC_ANA_VOLT, val);

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU1806_REG_ANA_VOLT_RC, &val);
        val = SET_BITFIELD(val, REG_DCDC2_VBIT_RC, normal_v);
        pmu_write(PMU1806_REG_ANA_VOLT_RC, val);
    }

    if (!dcdc_ramp_en) {
        if (normal_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC2_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC2_RAMP_EN;
        }
    }
}

static void pmu_ana_set_volt(enum PMU_POWER_MODE_T mode)
{
    uint16_t old_act_dcdc;
    uint16_t old_lp_dcdc;
    uint16_t new_act_dcdc;

    new_act_dcdc = ana_act_dcdc;

    if (mode == PMU_POWER_MODE_ANA_DCDC || mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_ana_get_volt(&old_act_dcdc, &old_lp_dcdc);
        if (old_act_dcdc < new_act_dcdc) {
            if (dcdc_ramp_en) {
                pmu_dcdc_ana_set_volt(old_act_dcdc, ana_lp_dcdc);
                hal_sys_timer_delay_us(PMU_DCDC_HW_RAMP_STABLE_TIME_US);
            } else {
                while (old_act_dcdc++ < new_act_dcdc) {
                    pmu_dcdc_ana_set_volt(old_act_dcdc, ana_lp_dcdc);
                }
                hal_sys_timer_delay_us(PMU_VANA_STABLE_TIME_US);
            }
        } else if (old_act_dcdc != new_act_dcdc || old_lp_dcdc != ana_lp_dcdc) {
            pmu_dcdc_ana_set_volt(new_act_dcdc, ana_lp_dcdc);
        }
    } else {
        pmu_dcdc_ana_set_volt(new_act_dcdc, ana_lp_dcdc);
    }
}

static void pmu_dcdc_dig_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_DIG_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_DCDC1_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_DCDC1_VBIT_DSLEEP);
    }

#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_dig_comp, MAX_DCDC1_VBIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_dig_lp_comp, MAX_DCDC1_VBIT_VAL);
    }
#endif
}

static void pmu_dcdc_dig_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_dig_comp, MAX_DCDC1_VBIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_dig_lp_comp, MAX_DCDC1_VBIT_VAL);
#endif

    pmu_read(PMU_REG_DCDC_DIG_VOLT, &val);
    val &= ~REG_DCDC1_VBIT_DSLEEP_MASK;
    val &= ~REG_DCDC1_VBIT_NORMAL_MASK;
    val |= REG_DCDC1_VBIT_DSLEEP(dsleep_v);
    val |= REG_DCDC1_VBIT_NORMAL(normal_v);
    pmu_write(PMU_REG_DCDC_DIG_VOLT, val);

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU1806_REG_DIG_VOLT_RC, &val);
        val = SET_BITFIELD(val, REG_DCDC1_VBIT_RC, normal_v);
        pmu_write(PMU1806_REG_DIG_VOLT_RC, val);
    }

    if (!dcdc_ramp_en) {
        if (normal_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC1_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC1_RAMP_EN;
        }
    }
}

#ifdef MTEST_ENABLED
#define DCDC_VOLT_LIST_QTY       31
//#define LDO_VOLT_LIST_QTY 18
static uint16_t dcdc_volt_list[DCDC_VOLT_LIST_QTY][2];
//static uint16_t ldo_volt_list[LDO_VOLT_LIST_QTY][2];
static bool volt_list_inited = false;

#define SET_VOLT_LIST(list, i, mv, val) \
    list[(i)][0] = (mv); \
    list[(i++)][1] = (val); \

static void pmu_dig_volt_list_init(void)
{
    int i;

    if (volt_list_inited) {
        return;
    }

    i = 0;
    SET_VOLT_LIST(dcdc_volt_list, i, 1100,  PMU_DCDC_DIG_1_1V );
    SET_VOLT_LIST(dcdc_volt_list, i, 1050,  PMU_DCDC_DIG_1_05V);
    SET_VOLT_LIST(dcdc_volt_list, i, 1000,  PMU_DCDC_DIG_1_0V );
    SET_VOLT_LIST(dcdc_volt_list, i, 950,   PMU_DCDC_DIG_0_95V);
    SET_VOLT_LIST(dcdc_volt_list, i, 900,   PMU_DCDC_DIG_0_9V );
    SET_VOLT_LIST(dcdc_volt_list, i, 870,   PMU_DCDC_DIG_0_87V);
    SET_VOLT_LIST(dcdc_volt_list, i, 850,   PMU_DCDC_DIG_0_85V);
    SET_VOLT_LIST(dcdc_volt_list, i, 800,   PMU_DCDC_DIG_0_8V );
    SET_VOLT_LIST(dcdc_volt_list, i, 780,   PMU_DCDC_DIG_0_78V);
    SET_VOLT_LIST(dcdc_volt_list, i, 775,   PMU_DCDC_DIG_0_775V);
    SET_VOLT_LIST(dcdc_volt_list, i, 750,   PMU_DCDC_DIG_0_75V);
    SET_VOLT_LIST(dcdc_volt_list, i, 740,   PMU_DCDC_DIG_0_74V);
    SET_VOLT_LIST(dcdc_volt_list, i, 730,   PMU_DCDC_DIG_0_73V);
    SET_VOLT_LIST(dcdc_volt_list, i, 725,   PMU_DCDC_DIG_0_725V);
    SET_VOLT_LIST(dcdc_volt_list, i, 720,   PMU_DCDC_DIG_0_72V );
    SET_VOLT_LIST(dcdc_volt_list, i, 710,   PMU_DCDC_DIG_0_71V );
    SET_VOLT_LIST(dcdc_volt_list, i, 700,   PMU_DCDC_DIG_0_7V  );
    SET_VOLT_LIST(dcdc_volt_list, i, 675,   PMU_DCDC_DIG_0_675V);
    SET_VOLT_LIST(dcdc_volt_list, i, 650,   PMU_DCDC_DIG_0_65V);
    SET_VOLT_LIST(dcdc_volt_list, i, 640,   PMU_DCDC_DIG_0_640V);
    SET_VOLT_LIST(dcdc_volt_list, i, 630,   PMU_DCDC_DIG_0_630V);
    SET_VOLT_LIST(dcdc_volt_list, i, 625,   PMU_DCDC_DIG_0_625V);
    SET_VOLT_LIST(dcdc_volt_list, i, 620,   PMU_DCDC_DIG_0_620V);
    SET_VOLT_LIST(dcdc_volt_list, i, 610,   PMU_DCDC_DIG_0_610V);
    SET_VOLT_LIST(dcdc_volt_list, i, 600,   PMU_DCDC_DIG_0_6V );
    SET_VOLT_LIST(dcdc_volt_list, i, 580,   PMU_DCDC_DIG_0_58V);
    SET_VOLT_LIST(dcdc_volt_list, i, 550,   PMU_DCDC_DIG_0_55V);
    SET_VOLT_LIST(dcdc_volt_list, i, 500,   PMU_DCDC_DIG_0_5V );
    SET_VOLT_LIST(dcdc_volt_list, i, 450,   PMU_DCDC_DIG_0_45V);
    SET_VOLT_LIST(dcdc_volt_list, i, 400,   PMU_DCDC_DIG_0_4V );
#if 0
    i = 0;
    SET_VOLT_LIST(ldo_volt_list, i, 1000, PMU_VDIG_1_0V  );
    SET_VOLT_LIST(ldo_volt_list, i, 950,  PMU_VDIG_0_95V );
    SET_VOLT_LIST(ldo_volt_list, i, 900,  PMU_VDIG_0_9V  );
    SET_VOLT_LIST(ldo_volt_list, i, 870,  PMU_VDIG_0_85V );
    SET_VOLT_LIST(ldo_volt_list, i, 850,  PMU_VDIG_0_85V );
    SET_VOLT_LIST(ldo_volt_list, i, 800,  PMU_VDIG_0_8V  );
    SET_VOLT_LIST(ldo_volt_list, i, 775,  PMU_VDIG_0_775V);
    SET_VOLT_LIST(ldo_volt_list, i, 750,  PMU_VDIG_0_75V );
    SET_VOLT_LIST(ldo_volt_list, i, 725,  PMU_VDIG_0_725V);
    SET_VOLT_LIST(ldo_volt_list, i, 700,  PMU_VDIG_0_7V  );
    SET_VOLT_LIST(ldo_volt_list, i, 675,  PMU_VDIG_0_675V);
    SET_VOLT_LIST(ldo_volt_list, i, 650,  PMU_VDIG_0_65V );
    SET_VOLT_LIST(ldo_volt_list, i, 625,  PMU_VDIG_0_625V);
    SET_VOLT_LIST(ldo_volt_list, i, 600,  PMU_VDIG_0_6V  );
    SET_VOLT_LIST(ldo_volt_list, i, 575,  PMU_VDIG_0_575V);
    SET_VOLT_LIST(ldo_volt_list, i, 550,  PMU_VDIG_0_55V );
    SET_VOLT_LIST(ldo_volt_list, i, 525,  PMU_VDIG_0_525V);
    SET_VOLT_LIST(ldo_volt_list, i, 500,  PMU_VDIG_0_5V  );
#endif
    volt_list_inited = true;
}

bool pmu_dig_set_volt_value(uint16_t dcdc_volt_mv, uint16_t logic_volt_mv, uint16_t ldo_volt_mv)
{
    uint16_t dcdc_volt = 0;
    uint16_t old_dcdc_volt;
//    uint16_t ldo_volt = 0;
    uint32_t i, find = 0;
    bool success = false;
    bool volt_inc = false;

    pmu_dig_volt_list_init();

    for (i = 0; i < ARRAY_SIZE(dcdc_volt_list); i++) {
        if (dcdc_volt_mv == dcdc_volt_list[i][0]) {
            dcdc_volt = dcdc_volt_list[i][1];
            find |= 0x1;
            break;
        }
    }
#if 0
    for (i = 0; i < ARRAY_SIZE(ldo_volt_list); i++) {
        if (ldo_volt_mv == ldo_volt_list[i][0]) {
            ldo_volt = ldo_volt_list[i][1];
            find |= 0x4;
            break;
        }
    }
#endif
    if (find == 0x3) {
        pmu_dcdc_dig_get_volt(&old_dcdc_volt, NULL);
        if (old_dcdc_volt < dcdc_volt) {
            volt_inc = true;
            while (old_dcdc_volt++ < dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_dcdc_volt, dig_lp_dcdc);
            }
        } else if (old_dcdc_volt != dcdc_volt) {
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
//        pmu_module_set_volt(PMU_BAT2DIG, dig_lp_ldo, ldo_volt);
        success = true;
    }

    if (volt_inc) {
        hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
    }

    return success;
}
#endif /* MTEST_ENABLED */

#ifdef PMU_RF_WORKAROUND
#ifdef DIG_DCDC_MODE
static uint32_t pmu_dcdc_dig_val_to_mv(uint16_t val)
{
    uint32_t mv;

    if (val >= PMU_DCDC_DIG_0_85V) {
        mv = 850;
    } else if (val >= PMU_DCDC_DIG_0_8V) {
        mv = 800;
    } else if (val >= PMU_DCDC_DIG_0_75V) {
        mv = 750;
    } else if (val >= PMU_DCDC_DIG_0_7V) {
        mv = 700;
    } else {
        mv = 650;
    }

    return mv;
}
#else
static uint32_t pmu_ldo_dig_val_to_mv(uint16_t val)
{
    uint32_t mv;

    if (val >= PMU_VDIG_0_85V) {
        mv = 850;
    } else if (val >= PMU_VDIG_0_8V) {
        mv = 800;
    } else if (val >= PMU_VDIG_0_75V) {
        mv = 750;
    } else if (val >= PMU_VDIG_0_7V) {
        mv = 700;
    } else {
        mv = 650;
    }

    return mv;
}
#endif
#endif

static void BOOT_TEXT_SRAM_LOC pmu_dig_get_target_volt(uint16_t *ldo, uint16_t *dcdc)
{
    uint16_t ldo_volt;
    uint16_t dcdc_volt;

    if (0) {
    } else if (pmu_vcore_req & PMU_VCORE_PMU_RESET) {
        ldo_volt = PMU_VDIG_DEFAULT;
        dcdc_volt = PMU_DCDC_DIG_DEFAULT - PMU_DCDC_DIG_50MV;
    } else if (pmu_vcore_req & (PMU_VCORE_SYS_FREQ_HIGH | PMU_VCORE_AXI_FREQ_HIGH)) {
#if defined(MCU_HIGH_PERFORMANCE_MODE)
        if (high_perf_freq_mhz <= 260) {
            ldo_volt = PMU_VDIG_0_9V;
            dcdc_volt = PMU_DCDC_DIG_0_9V;
        } else if (high_perf_freq_mhz <= 300) {
            ldo_volt = PMU_VDIG_1_0V;
            dcdc_volt = PMU_DCDC_DIG_1_0V;
        } else
#endif
        // freq_mhz = 350M or 400M (AUD_PLL_DOUBLE)
        {
            ldo_volt = PMU_VDIG_1_05V;
            dcdc_volt = PMU_DCDC_DIG_1_05V;
        }
    } else if (pmu_vcore_req & (PMU_VCORE_USB_HS_ENABLED | PMU_VCORE_RS_FREQ_HIGH |
            PMU_VCORE_SYS_FREQ_MEDIUM | PMU_VCORE_AXI_FREQ_MEDIUM |
            PMU_VCORE_FLASH_FREQ_HIGH | PMU_VCORE_FLASH_WRITE_ENABLED | PMU_VCORE_HW_UVP_ENABLED)) {
        ldo_volt = PMU_VDIG_0_8V;
        dcdc_volt = PMU_DCDC_DIG_0_8V;
    } else if (pmu_vcore_req & (PMU_VCORE_FIR_FREQ_HIGH | PMU_VCORE_IIR_FREQ_HIGH | PMU_VCORE_IIR_EQ_FREQ_HIGH |
                PMU_VCORE_RS_FREQ_HIGH | PMU_VCORE_RS_ADC_FREQ_HIGH | PMU_VCORE_PSRAM_FREQ_HIGH)) {
        ldo_volt = PMU_VDIG_0_725V;
        dcdc_volt = PMU_DCDC_DIG_0_71V;
    } else if (pmu_vcore_req & (PMU_VCORE_SYS_FREQ_MEDIUM_LOW | PMU_VCORE_AXI_FREQ_MEDIUM_LOW)) {
        ldo_volt = PMU_VDIG_0_675V;
        dcdc_volt = PMU_DCDC_DIG_0_675V;
    } else if (pmu_vcore_req & (PMU_VCORE_FIR_ENABLE | PMU_VCORE_IIR_ENABLE | PMU_VCORE_IIR_EQ_ENABLE)) {
        ldo_volt = PMU_VDIG_0_65V;
        dcdc_volt = PMU_DCDC_DIG_0_635V;
    } else {
        // Common cases
        ldo_volt = PMU_VDIG_0_6V;
        dcdc_volt = PMU_DCDC_DIG_0_6V;
    }

#if (defined(PROGRAMMER) || defined(__BES_OTA_MODE__)) && !defined(PMU_FULL_INIT)
    // Try to keep the same vcore voltage as ROM (hardware default)
    if (ldo_volt < PMU_VDIG_DEFAULT) {
        ldo_volt = PMU_VDIG_DEFAULT;
    }
    if (dcdc_volt < PMU_DCDC_DIG_DEFAULT) {
        dcdc_volt = PMU_DCDC_DIG_DEFAULT;
    }
#endif

#ifdef DIG_DCDC_MODE
    if (ldo_volt < PMU_VDIG_DEFAULT) {
        ldo_volt = PMU_VDIG_DEFAULT;
    }
#endif

    if (ldo) {
        *ldo = ldo_volt;
    }
    if (dcdc) {
        *dcdc = dcdc_volt;
    }
}

static void pmu_dig_set_volt(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    uint32_t delay_us;
    uint16_t dcdc_volt, old_act_dcdc, old_lp_dcdc;
    uint16_t ldo_volt, old_act_ldo, old_lp_ldo;
    bool volt_inc = false;

#ifdef NO_ISPI
    return;
#endif

    lock = int_lock();

    if (mode == PMU_POWER_MODE_NONE) {
        mode = pmu_power_mode;
    }

    pmu_dig_get_target_volt(&ldo_volt, &dcdc_volt);

    pmu_module_get_volt(PMU_BAT2DIG, &old_lp_ldo, &old_act_ldo);
    pmu_dcdc_dig_get_volt(&old_act_dcdc, &old_lp_dcdc);

    if (mode == PMU_POWER_MODE_DIG_DCDC) {
        if (old_act_dcdc < dcdc_volt) {
            volt_inc = true;
            if (dcdc_ramp_en) {
                // Ramp up 8 steps in one cycle.
                pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
            } else {
                while (old_act_dcdc++ < dcdc_volt) {
                    pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
                }
            }
        } else if (old_act_dcdc != dcdc_volt || old_lp_dcdc != dig_lp_dcdc) {
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
        // Update the voltage of the other mode
        pmu_module_set_volt(PMU_BAT2DIG, dig_lp_ldo, ldo_volt);
    } else {
        if (old_act_ldo < ldo_volt) {
            volt_inc = true;
        }
        pmu_module_ramp_volt(PMU_BAT2DIG, dig_lp_ldo, ldo_volt);
        // Update the voltage of the other mode
        pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
    }

    if (volt_inc) {
        if (dcdc_ramp_en) {
            delay_us = (dcdc_volt - old_act_dcdc + pmu_dcdc_dig_comp) / 8 * PMU_DCDC_HW_RAMP_1_CLK_TIME_US + PMU_DCDC_HW_RAMP_1_CLK_TIME_US;
            hal_sys_timer_delay_us(delay_us);
        } else {
            hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
        }
    }

#ifdef PMU_RF_WORKAROUND
    uint32_t volt_mv;

#ifdef DIG_DCDC_MODE
    volt_mv = pmu_dcdc_dig_val_to_mv(dcdc_volt);
#else
    volt_mv = pmu_ldo_dig_val_to_mv(ldo_volt);
#endif

    pmu_rf_pll_clk_edge_workaround(volt_mv);
#endif

    int_unlock(lock);
}

static void pmu_ldo_mode_en(void)
{
    unsigned short val;

    // Enable vana ldo
    pmu_module_config(PMU_ANA,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    // Enable vcore ldo
    pmu_module_config(PMU_BAT2DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#ifndef PMU_FORCE_LP_MODE
    if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id < HAL_CHIP_METAL_ID_2) {
        pmu_read(PMU1806_REG_LP_BIAS_SEL_LDO, &val);
        val = SET_BITFIELD(val, REG_1806_LP_BIAS_SEL_LDO, 0x7);
        pmu_write(PMU1806_REG_LP_BIAS_SEL_LDO, val);

        pmu_read(PMU_REG_BAT2DIG_CFG, &val);
        val |= LP_EN_VBAT2VCORE_LDO_DR | LP_EN_VBAT2VCORE_LDO;
        pmu_write(PMU_REG_BAT2DIG_CFG, val);
    }
#endif
    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        hal_sys_timer_delay(MS_TO_TICKS(5));
#ifndef PMU_FORCE_LP_MODE
        if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id < HAL_CHIP_METAL_ID_2) {
            val &= ~LP_EN_VBAT2VCORE_LDO_DR;
            pmu_write(PMU_REG_BAT2DIG_CFG, val);

            pmu_read(PMU1806_REG_LP_BIAS_SEL_LDO, &val);
            val = SET_BITFIELD(val, REG_1806_LP_BIAS_SEL_LDO, 0x1);
            pmu_write(PMU1806_REG_LP_BIAS_SEL_LDO, val);
        }
#endif
    }

    // Disable vcore dcdc
    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        val = (val & ~REG_PU_BUCK_VCORE) | REG_PU_BUCK_VCORE_DR;
    } else {
        val = (val & ~REG_PU_DCDC1) | REG_PU_DCDC1_DR;
    }
    pmu_write(PMU_REG_DCDC_DIG_EN, val);
    // Disable vana dcdc
    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        val = (val & ~REG_PU_BUCK_VANA) | REG_PU_BUCK_VANA_DR;
    } else {
        val = (val & ~REG_PU_DCDC2) | REG_PU_DCDC2_DR;
    }
    pmu_write(PMU_REG_DCDC_ANA_EN, val);
}

static void pmu_dcdc_ana_mode_en(void)
{
    unsigned short val;

    if (pmu_power_mode == PMU_POWER_MODE_DIG_DCDC) {
        // Enable vcore ldo
        pmu_module_config(PMU_BAT2DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

        hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);
        if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
            hal_sys_timer_delay(MS_TO_TICKS(5));
        }

        // Disable vcore dcdc
        pmu_read(PMU_REG_DCDC_DIG_EN, &val);
        if (pmu_type == PMU_CHIP_TYPE_1809) {
            val = (val & ~REG_PU_BUCK_VCORE) | REG_PU_BUCK_VCORE_DR;
        } else {
            val = (val & ~REG_PU_DCDC1) | REG_PU_DCDC1_DR;
        }
        pmu_write(PMU_REG_DCDC_DIG_EN, val);
    } else {
#ifdef PMU_BUCK_DIRECT_ULP_MODE
        if (pmu_type == PMU_CHIP_TYPE_1809) {
            pmu_read(PMU1809_REG_BUCK_VANA_CFG_130, &val);
            if (pmu_metal_id == HAL_CHIP_METAL_ID_0) {
                val &= ~BUCK_VANA_DIRECT_ULP_MODE;
            } else {
                val |= BUCK_VANA_DIRECT_ULP_MODE;
            }
            pmu_write(PMU1809_REG_BUCK_VANA_CFG_130, val);

            if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
                pmu_read(PMU1809_REG_BUCK_VANA_CFG_134, &val);
#ifdef PMU_DCDC_PWM
                val &= ~BUCK_VANA_ULP_MODE;
#else
                val |= BUCK_VANA_ULP_MODE;
#endif
                pmu_write(PMU1809_REG_BUCK_VANA_CFG_134, val);
            }
        }
#endif

        // Enable vana dcdc
        pmu_read(PMU_REG_DCDC_ANA_EN, &val);
        if (pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef PMU_DCDC_PWM
            val &= ~(REG_BUCK_VANA_BURST_MODE_SEL_NORMAL | REG_BUCK_VANA_BURST_MODE_SEL_RC);
            if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
                val &= ~REG_BUCK_VANA_BURST_MODE_SEL_DSLEEP;
            }
#else
            if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
                val = (val & ~(REG_BUCK_VANA_BURST_MODE_SEL_NORMAL | REG_BUCK_VANA_BURST_MODE_SEL_RC)) | REG_BUCK_VANA_BURST_MODE_SEL_DSLEEP;
            } else {
                val |= REG_BUCK_VANA_BURST_MODE_SEL_NORMAL | REG_BUCK_VANA_BURST_MODE_SEL_RC;
            }
#endif
#ifdef PMU_BUCK_DIRECT_ULP_MODE
            val |= REG_CLK_DCDC2_32K_EN;
#endif
            val |= REG_PU_BUCK_VANA_DR | REG_PU_BUCK_VANA;
        } else {
#ifdef PMU_DCDC_PWM
            val &= ~REG_DCDC2_BURST_MODE_SEL_NORMAL;
#else
            val |= REG_DCDC2_BURST_MODE_SEL_NORMAL;
#endif
            val = (val & ~REG_DCDC2_ULP_MODE_NORMAL) | REG_DCDC2_ULP_MODE_DSLEEP | REG_PU_DCDC2_DR | REG_PU_DCDC2;
        }
        pmu_write(PMU_REG_DCDC_ANA_EN, val);

        hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

        // Disable vana ldo
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    }
}

static void pmu_dcdc_dual_mode_en(void)
{
    unsigned short val;

#ifdef PMU_BUCK_DIRECT_ULP_MODE
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1809_REG_BUCK_VANA_CFG_130, &val);
        if (pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            val &= ~BUCK_VANA_DIRECT_ULP_MODE;
        } else {
            val |= BUCK_VANA_DIRECT_ULP_MODE;
        }
        pmu_write(PMU1809_REG_BUCK_VANA_CFG_130, val);

        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            pmu_read(PMU1809_REG_BUCK_VANA_CFG_134, &val);
#ifdef PMU_DCDC_PWM
            val &= ~BUCK_VANA_ULP_MODE;
#else
            val |= BUCK_VANA_ULP_MODE;
#endif
            pmu_write(PMU1809_REG_BUCK_VANA_CFG_134, val);
        }
    }
#endif

    // Enable vana dcdc
    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    if (pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef PMU_DCDC_PWM
        val &= ~(REG_BUCK_VANA_BURST_MODE_SEL_NORMAL | REG_BUCK_VANA_BURST_MODE_SEL_RC);
        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            val &= ~REG_BUCK_VANA_BURST_MODE_SEL_DSLEEP;
        }
#else
        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            val = (val & ~(REG_BUCK_VANA_BURST_MODE_SEL_NORMAL | REG_BUCK_VANA_BURST_MODE_SEL_RC)) | REG_BUCK_VANA_BURST_MODE_SEL_DSLEEP;
        } else {
            val |= REG_BUCK_VANA_BURST_MODE_SEL_NORMAL | REG_BUCK_VANA_BURST_MODE_SEL_RC;
        }
#endif
#ifdef PMU_BUCK_DIRECT_ULP_MODE
        val |= REG_CLK_DCDC2_32K_EN | REG_CLK_DCDC1_32K_EN;
#endif
        val |= REG_PU_BUCK_VANA_DR | REG_PU_BUCK_VANA;
    } else {
#ifdef PMU_DCDC_PWM
        val &= ~REG_DCDC2_BURST_MODE_SEL_NORMAL;
#else
        val |= REG_DCDC2_BURST_MODE_SEL_NORMAL;
#endif
        val = (val & ~REG_DCDC2_ULP_MODE_NORMAL) | REG_DCDC2_ULP_MODE_DSLEEP | REG_PU_DCDC2_DR | REG_PU_DCDC2;
    }
    pmu_write(PMU_REG_DCDC_ANA_EN, val);

#ifdef PMU_BUCK_DIRECT_ULP_MODE
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1809_REG_BUCK_VCORE_CFG_136, &val);
        if (pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            val &= ~BUCK_VCORE_DIRECT_ULP_MODE;
        } else {
            val |= BUCK_VCORE_DIRECT_ULP_MODE;
        }
        pmu_write(PMU1809_REG_BUCK_VCORE_CFG_136, val);

        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            pmu_read(PMU1809_REG_BUCK_VCORE_CFG_13A, &val);
#ifdef PMU_DCDC_PWM
            val &= ~BUCK_VCORE_ULP_MODE;
#else
            val |= BUCK_VCORE_ULP_MODE;
#endif
            pmu_write(PMU1809_REG_BUCK_VCORE_CFG_13A, val);
        }
    }
#endif

    // Enable vcore dcdc
    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        val &= ~REG_UVLO_SEL_MASK;
        val |= REG_UVLO_SEL(0x0);
    }
    if (pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef PMU_DCDC_PWM
        val &= ~(REG_BUCK_VCORE_BURST_MODE_SEL_NORMAL | REG_BUCK_VCORE_BURST_MODE_SEL_RC);
        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            val &= ~REG_BUCK_VCORE_BURST_MODE_SEL_DSLEEP;
        }
#else
        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            val = (val & ~(REG_BUCK_VCORE_BURST_MODE_SEL_NORMAL | REG_BUCK_VCORE_BURST_MODE_SEL_RC)) | REG_BUCK_VCORE_BURST_MODE_SEL_DSLEEP;
        } else {
            val |= REG_BUCK_VCORE_BURST_MODE_SEL_NORMAL | REG_BUCK_VCORE_BURST_MODE_SEL_RC;
        }
#endif
        val |= REG_PU_BUCK_VCORE_DR | REG_PU_BUCK_VCORE;
    } else {
#ifdef PMU_DCDC_PWM
        val &= ~REG_DCDC1_BURST_MODE_SEL_NORMAL;
#else
        val |= REG_DCDC1_BURST_MODE_SEL_NORMAL;
#endif
        val = (val & ~REG_DCDC1_ULP_MODE_NORMAL) | REG_DCDC1_ULP_MODE_DSLEEP | REG_PU_DCDC1_DR | REG_PU_DCDC1;
    }
    pmu_write(PMU_REG_DCDC_DIG_EN, val);

    hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

    // Disable vana ldo
    pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    // Disable vcore ldo
    pmu_module_config(PMU_BAT2DIG,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
}

void pmu_mode_change(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;

    if (pmu_power_mode == mode || mode == PMU_POWER_MODE_NONE) {
        return;
    }

    lock = int_lock();

    pmu_ana_set_volt(pmu_power_mode);
    pmu_dig_set_volt(pmu_power_mode);

    if (mode == PMU_POWER_MODE_ANA_DCDC) {
        pmu_dcdc_ana_mode_en();
    } else if (mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_dual_mode_en();
    } else if (mode == PMU_POWER_MODE_LDO) {
        pmu_ldo_mode_en();
    }

    pmu_power_mode = mode;

    int_unlock(lock);
}

void pmu_sleep_en(unsigned char sleep_en)
{
    unsigned short val;

    pmu_read(PMU_REG_SLEEP_CFG, &val);
    if (sleep_en) {
        val |= SLEEP_ALLOW;
    } else {
        val &= ~SLEEP_ALLOW;
    }
    pmu_write(PMU_REG_SLEEP_CFG, val);

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1806_REG_SLEEP_CFG_RC, &val);
        if (sleep_en) {
            val |= RCSLEEP_ALLOW;
        } else {
            val &= ~RCSLEEP_ALLOW;
        }
        pmu_write(PMU1806_REG_SLEEP_CFG_RC, val);
    }
}

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
static uint32_t pmu_vcodec_mv_to_val(uint16_t mv)
{
    uint32_t val;

#ifdef VHPPA2VCODEC
    if (mv == 1900) {
        val = PMU_HPPA2CODEC_1_9V;
    } else if (mv >= 1950) {
        val = PMU_HPPA2CODEC_1_95V;
    } else {
        val = PMU_HPPA2CODEC_1_7V;
    }
#else
    if (mv == 1900) {
        val = PMU_CODEC_1_9V;
    } else if (mv >= 1950) {
        val = PMU_CODEC_1_9V;
    } else {
        val = PMU_CODEC_1_7V;
    }
#endif

    return val;
}

static uint32_t pmu_ldo_hppa_mv_to_val(uint16_t mv)
{
    uint32_t val;

    if (mv == 1600) {
        val = PMU_VHPPA_1_6V;
    } else if (mv == 1700) {
        val = PMU_VHPPA_1_7V;
    } else if (mv >= 1900) {
        val = PMU_VHPPA_1_9V;
    } else {
        val = PMU_VHPPA_1_8V;
    }

    return val;
}

static void POSSIBLY_UNUSED pmu_ldo_hppa_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    uint16_t val = 0;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        if (normal_vp) {
            *normal_vp = val;
        }
        if (dsleep_vp) {
            *dsleep_vp = val;
        }
        return;
    }

    pmu_read(PMU_REG_HPPA_LDO_EN, &val);
    val = GET_BITFIELD(val, REG_RES_SEL_VHPPA);
    if (normal_vp) {
        *normal_vp = val;
    }
    if (dsleep_vp) {
        *dsleep_vp = val;
    }
}

static void pmu_ldo_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        return;
    }

    pmu_read(PMU_REG_HPPA_LDO_EN, &val);
    val = SET_BITFIELD(val, REG_RES_SEL_VHPPA, normal_v);
    pmu_write(PMU_REG_HPPA_LDO_EN, val);
}

static void pmu_ldo_hppa_en(int enable)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        return;
    }

    pmu_read(PMU_REG_HPPA_LDO_EN, &val);
    if (enable) {
        val |= (REG_PU_LDO_VHPPA_DSLEEP | REG_PU_LDO_VHPPA_EN);
    } else {
        val &= ~(REG_PU_LDO_VHPPA_DSLEEP | REG_PU_LDO_VHPPA_EN);
    }
    pmu_write(PMU_REG_HPPA_LDO_EN, val);
}

#ifndef HPPA_LDO_ON
static uint32_t pmu_dcdc_hppa_mv_to_val(uint16_t mv)
{
    uint32_t val;

    if (mv == 1350) {
        val = PMU_DCDC_HPPA_1_35V;
    } else if (mv == 1400) {
        val = PMU_DCDC_HPPA_1_4V;
    } else if (mv == 1450) {
        val = PMU_DCDC_HPPA_1_45V;
    } else if (mv == 1500) {
        val = PMU_DCDC_HPPA_1_5V;
    } else if (mv == 1550) {
        val = PMU_DCDC_HPPA_1_55V;
    } else if (mv == 1600) {
        val = PMU_DCDC_HPPA_1_6V;
    } else if (mv == 1650) {
        val = PMU_DCDC_HPPA_1_65V;
    } else if (mv == 1700) {
        val = PMU_DCDC_HPPA_1_7V;
    } else if (mv == 1750) {
        val = PMU_DCDC_HPPA_1_75V;
    } else if (mv == 1900) {
        val = PMU_DCDC_HPPA_1_9V;
    } else if (mv >= 1950) {
        val = PMU_DCDC_HPPA_1_95V;
    } else {
        val = PMU_DCDC_HPPA_1_8V;
    }

    return val;
}

static void pmu_dcdc_hppa_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_HPPA_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_DCDC3_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_DCDC3_VBIT_DSLEEP);
    }

#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_hppa_comp, MAX_DCDC3_VBIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_hppa_lp_comp, MAX_DCDC3_VBIT_VAL);
    }
#endif
}

static void pmu_dcdc_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_hppa_comp, MAX_DCDC3_VBIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_hppa_lp_comp, MAX_DCDC3_VBIT_VAL);
#endif

    pmu_read(PMU_REG_DCDC_HPPA_VOLT, &val);
    val &= ~REG_DCDC3_VBIT_DSLEEP_MASK;
    val &= ~REG_DCDC3_VBIT_NORMAL_MASK;
    val |= REG_DCDC3_VBIT_DSLEEP(dsleep_v);
    val |= REG_DCDC3_VBIT_NORMAL(normal_v);
    pmu_write(PMU_REG_DCDC_HPPA_VOLT, val);

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1806_REG_HPPA_VOLT_RC, &val);
        val |= REG_DCDC3_VBIT_RC(dsleep_v);
        if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
            val &= ~REG_DCDC3_VBIT_RC_MASK;
            val &= ~REG_DCDC3_PFM_SEL_RC;
            val |= REG_DCDC3_ULP_MODE_RC | REG_DCDC3_BURST_MODE_SEL_RC;
        }
        pmu_write(PMU1806_REG_HPPA_VOLT_RC, val);
    }

    if (!dcdc_ramp_en) {
        if (normal_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC3_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC3_RAMP_EN;
        }
    }
}

static void pmu_dcdc_hppa_en(int enable)
{
    uint16_t val;

#ifdef PMU_BUCK_DIRECT_ULP_MODE
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1809_REG_BUCK_VHPPA_CFG_129, &val);
        if (pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            val &= ~BUCK_VHPPA_DIRECT_ULP_MODE;
        } else {
            val |= BUCK_VHPPA_DIRECT_ULP_MODE;
        }
        pmu_write(PMU1809_REG_BUCK_VHPPA_CFG_129, val);

        pmu_read(PMU_REG_DCDC_ANA_EN, &val);
        val |= REG_CLK_DCDC3_32K_EN;
        pmu_write(PMU_REG_DCDC_ANA_EN, val);

        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            pmu_read(PMU1809_REG_BUCK_VHPPA_CFG_12D, &val);
#ifdef PMU_DCDC_PWM
            val &= ~BUCK_VHPPA_ULP_MODE;
#else
            val |= BUCK_VHPPA_ULP_MODE;
#endif
            pmu_write(PMU1809_REG_BUCK_VHPPA_CFG_12D, val);
        }
    }
#endif

    pmu_read(PMU_REG_DCDC_HPPA_EN, &val);
    if (enable) {
        if (pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef PMU_DCDC_PWM
            val &= ~(REG_BUCK_VHPPA_BURST_MODE_SEL_NORMAL | REG_BUCK_VHPPA_BURST_MODE_SEL_RC);
            if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
                val &= ~REG_BUCK_VHPPA_BURST_MODE_SEL_DSLEEP;
            }
#else
            if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
                val = (val & ~(REG_BUCK_VHPPA_BURST_MODE_SEL_NORMAL | REG_BUCK_VHPPA_BURST_MODE_SEL_RC)) | REG_BUCK_VHPPA_BURST_MODE_SEL_DSLEEP;
            } else {
                val |= REG_BUCK_VHPPA_BURST_MODE_SEL_NORMAL | REG_BUCK_VHPPA_BURST_MODE_SEL_RC;
            }
#endif
            val |= REG_PU_BUCK_VHPPA | REG_PU_BUCK_VHPPA_DR;
        } else {
#ifdef PMU_DCDC_PWM
            val &= ~REG_DCDC3_BURST_MODE_SEL_NORMAL;
#else
            val |= REG_DCDC3_BURST_MODE_SEL_NORMAL;
#endif
            val = (val & ~REG_DCDC3_ULP_MODE_NORMAL) | REG_DCDC3_ULP_MODE_DSLEEP | REG_PU_DCDC3_DR | REG_PU_DCDC3;
        }
    } else {
        if (pmu_type == PMU_CHIP_TYPE_1809) {
            val = (val & ~REG_PU_BUCK_VHPPA) | REG_PU_BUCK_VHPPA_DR;
        } else {
            val = (val & ~REG_PU_DCDC3) | REG_PU_DCDC3_DR;
        }
    }
    pmu_write(PMU_REG_DCDC_HPPA_EN, val);
}
#endif

static uint32_t pmu_vhppa_mv_to_val(uint16_t mv)
{
#ifdef HPPA_LDO_ON
    return pmu_ldo_hppa_mv_to_val(mv);
#else
    return pmu_dcdc_hppa_mv_to_val(mv);
#endif
}

static void POSSIBLY_UNUSED pmu_hppa_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
#ifdef HPPA_LDO_ON
    pmu_ldo_hppa_get_volt(normal_vp, dsleep_vp);
#else
    pmu_dcdc_hppa_get_volt(normal_vp, dsleep_vp);
#endif
}

static void pmu_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
#ifdef HPPA_LDO_ON
    pmu_ldo_hppa_set_volt(normal_v, dsleep_v);
#else
    pmu_dcdc_hppa_set_volt(normal_v, dsleep_v);
#endif
}

static void pmu_hppa_en(int enable)
{
#ifdef HPPA_LDO_ON
    pmu_ldo_hppa_en(enable);
#else
    pmu_dcdc_hppa_en(enable);
#endif
}

static void pmu_hppa_dcdc_to_ldo(void)
{
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
#ifndef VMEM_ON
#ifdef PMU_VMEM_PWR_SUPPLY_IO
        if (pmu_type == PMU_CHIP_TYPE_1809) {
            if (pmu_split_vmem_buck3_valid) {
                uint16_t val;

                pmu_read(PMU1809_REG_LDO_VMEM2_CFG, &val);
                val |= PMU1809_REG_LDO_VMEM2_BYPASS;
                pmu_write(PMU1809_REG_LDO_VMEM2_CFG, val);

#ifndef PMU_FORCE_LP_MODE
                pmu_read(PMU_REG_MEM_CFG, &val);
                val &= ~(LP_EN_VMEM_LDO_DR | LP_EN_VMEM_LDO_REG);
                pmu_write(PMU_REG_MEM_CFG, val);
#endif

                // buck3 volt will ramp to vmem volt.
                pmu_module_ramp_volt(PMU_MEM, PMU_VMEM_1_8V, PMU_VMEM_1_8V);
                hal_sys_timer_delay_us(PMU_VHPPA_STABLE_TIME_US);
            } else {
                pmu_module_set_volt(PMU_MEM, PMU_VMEM_1_8V, PMU_VMEM_1_8V);
                pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
                hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);
            }
        }
#else
        pmu_module_set_volt(PMU_MEM, PMU_VMEM_1_8V, PMU_VMEM_1_8V);

        pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
        hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);
        if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
            hal_sys_timer_delay(MS_TO_TICKS(5));
        }
#endif
        pmu_dcdc_hppa_en(false);
#endif
        return;
    }

#ifndef HPPA_LDO_ON
    uint16_t val;
    uint16_t act_volt;

    val = pmu_dcdc_hppa_mv_to_val(vhppa_mv);
    pmu_dcdc_hppa_get_volt(&act_volt, NULL);
    if (val < act_volt) {
        val = pmu_ldo_hppa_mv_to_val(HPPA_RAMP_UP_VOLT_MV);
    } else {
        val = pmu_ldo_hppa_mv_to_val(vhppa_mv);
    }
    pmu_ldo_hppa_set_volt(val, val);

    pmu_ldo_hppa_en(true);
    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

    pmu_dcdc_hppa_en(false);
#endif
}

#ifdef PMU_VMEM_PWR_SUPPLY_IO
static uint32_t pmu_ldo_vmem_mv_to_val(uint16_t mv)
{
    uint32_t val;

    if (mv == 1600) {
        val = PMU_VMEM_1_6V;
    } else if (mv == 1700) {
        val = PMU_VMEM_1_7V;
    } else if (mv == 1800) {
        val = PMU_VMEM_1_8V;
    } else if (mv == 1900) {
        val = PMU_VMEM_1_9V;
    }

    return val;
}
#endif

static void BOOT_TEXT_FLASH_LOC pmu_dig_init_volt(void)
{
    uint16_t ldo_volt;
    uint16_t val;

    pmu_dig_get_target_volt(&ldo_volt, NULL);

    pmu_read(PMU_REG_DIG_CFG, &val);
    if (GET_BITFIELD(val, LDO_DIG_VBIT_NORMAL) < ldo_volt) {
        val = SET_BITFIELD(val, LDO_DIG_VBIT_NORMAL, ldo_volt);
        pmu_write(PMU_REG_DIG_CFG, val);
    }
}

static void pmu_set_sar_vref(void)
{
    uint16_t efuse_val, value;

    pmu_get_efuse(PMU_EFUSE_PAGE_SAR_VREF, &efuse_val);
    TRACE(2, "%s, efuse E : 0x%x", __func__, efuse_val);

    if (0 == (efuse_val & 0x3F)) {
        return;
    }

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        efuse_val &= 0x3F;                  //[5:0] valid

        pmu_read(PMU_REG_BUCK2ANA_CFG, &value);
        if (pmu_type == PMU_CHIP_TYPE_1809) {
            value = SET_BITFIELD(value, PMU1809_SAR_VREF_BIT, efuse_val);
        } else {
            value = SET_BITFIELD(value, PMU1805_SAR_VREF_BIT, efuse_val);
        }
        pmu_write(PMU_REG_BUCK2ANA_CFG, value);
    } else if (pmu_type == PMU_CHIP_TYPE_1806) {
        efuse_val &= 0xF;                  //[3:0] valid

        pmu_read(PMU1806_REG_AVDD25, &value);
        value = SET_BITFIELD(value, SAR_VREF_BIT, efuse_val);
        pmu_write(PMU1806_REG_AVDD25, value);
    }
}

static void pmu_sar_adc_config(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1805) {
        // Increase the time from adc_start to adc sampling
        pmu_read(PMU_REG_SAR_STABLE_SEL, &val);
        val &= ~(DELAY_BEFORE_SAMP_MASK | TIMER_SAR_STABLE_SEL_MASK);
        val |= (DELAY_BEFORE_SAMP(0x3) | TIMER_SAR_STABLE_SEL(0x7));
        pmu_write(PMU_REG_SAR_STABLE_SEL, val);

        // Gpadc clock enable
        pmu_read(PMU_REG_SAR_VREF, &val);
        val |= REG_CLK_GPADC_EN;
        pmu_write(PMU_REG_SAR_VREF, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_BUCK2ANA_CFG, &val);
        val = SET_BITFIELD(val, PMU1809_SAR_INPUT_R_SEL, 0x1);
        val |= PMU1809_SAR_INPUT_C_SEL;
        pmu_write(PMU_REG_BUCK2ANA_CFG, val);

        pmu_read(PMU_REG_GPADC_EN_DR, &val);
        // Set to 100K
#ifdef PMU_LOW_POWER_CLOCK
        val = SET_BITFIELD(val, REG_CLK_GPADC_DIV_VALUE, 0xA);
#else
        val = SET_BITFIELD(val, REG_CLK_GPADC_DIV_VALUE, 0x16);
#endif
        pmu_write(PMU_REG_GPADC_EN_DR, val);
    }
}

static void pmu_codec_tx_enable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        // enable 1809 codec tx
        pmu_read(PMU1809_REG_RESERVED_ANA, &val);
        val |= PMU1809_REG_RESERVED_ANA_15_0(1 << 0);
        pmu_write(PMU1809_REG_RESERVED_ANA, val);

        // set pmu clk 24M
#ifndef PMU_LOW_POWER_CLOCK
        // if dcdc div2 is bypassed, pmu clock= OSC(24M)
        hal_cmu_dcdc_clock_bypass_div();
#endif
    }
}

int pmu_codec_volt_ramp_up(void)
{
#ifndef VMEM_ON
    unsigned short normal, dsleep;
    unsigned short target;

    // No need to ramp if VCM_LPF is enabled
    if (vcodec_off) {
        target = pmu_vhppa_mv_to_val(HPPA_RAMP_UP_VOLT_MV);
        pmu_hppa_get_volt(&normal, &dsleep);
        if (normal < target) {
            pmu_hppa_set_volt(target, dsleep);
        }
    }
#endif
    return 0;
}

int pmu_codec_volt_ramp_down(void)
{
#ifndef VMEM_ON
    unsigned short normal, dsleep;
    unsigned short target;

    // No need to ramp if VCM_LPF is enabled
    if (vcodec_off) {
        target = pmu_vhppa_mv_to_val(vhppa_mv);
        pmu_hppa_get_volt(&normal, &dsleep);
        if (normal > target) {
            pmu_hppa_set_volt(target, dsleep);
        }
    }
#endif
    return 0;
}
#endif

POSSIBLY_UNUSED
static void pmu_ana_dig_volt_init(void)
{
#ifdef VANA_1P05V
    ana_act_dcdc = PMU_DCDC_ANA_1_05V;
#elif defined(VANA_1P1V)
    ana_act_dcdc = PMU_DCDC_ANA_1_1V;
#elif defined(VANA_1P2V)
    ana_act_dcdc = PMU_DCDC_ANA_1_2V;
#elif defined(VANA_1P35V)
    ana_act_dcdc = PMU_DCDC_ANA_1_35V;
#elif defined(VANA_1P4V)
    ana_act_dcdc = PMU_DCDC_ANA_1_4V;
#elif defined(VANA_1P5V)
    ana_act_dcdc = PMU_DCDC_ANA_1_5V;
#elif defined(VANA_1P6V)
    ana_act_dcdc = PMU_DCDC_ANA_1_6V;
#else
    ana_act_dcdc = PMU_DCDC_ANA_1_3V;
#endif
    ana_lp_dcdc = ana_act_dcdc;

    dig_lp_ldo = PMU_VDIG_0_6V;
    dig_lp_dcdc = PMU_DCDC_DIG_0_6V;
}

static POSSIBLY_UNUSED void pmu_dcdc_vref_set(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU_REG_DCDC_DIG_EN, &val);
        val &= ~(REG_DCDC1_VREF_SEL_NORMAL_MASK | REG_DCDC1_VREF_SEL_DSLEEP_MASK);
        val |= REG_DCDC1_VREF_SEL_NORMAL(0) | REG_DCDC1_VREF_SEL_DSLEEP(0);
        pmu_write(PMU_REG_DCDC_DIG_EN, val);

        pmu_read(PMU_REG_DCDC_ANA_EN, &val);
        val &= ~(REG_DCDC2_VREF_SEL_NORMAL_MASK | REG_DCDC2_VREF_SEL_DSLEEP_MASK);
        val |= REG_DCDC2_VREF_SEL_NORMAL(0) | REG_DCDC2_VREF_SEL_DSLEEP(0);
        pmu_write(PMU_REG_DCDC_ANA_EN, val);

        pmu_read(PMU1806_REG_DIG_VOLT_RC, &val);
        val = SET_BITFIELD(val, REG_DCDC1_VREF_SEL_RC, 0);
        pmu_write(PMU1806_REG_DIG_VOLT_RC, val);

        pmu_read(PMU1806_REG_ANA_VOLT_RC, &val);
        val = SET_BITFIELD(val, REG_DCDC2_VREF_SEL_RC, 0);
        pmu_write(PMU1806_REG_ANA_VOLT_RC, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU_REG_BAT2DIG_CFG, &val);
        val |= LP_EN_VBAT2VCORE_LDO_DR | LP_EN_VBAT2VCORE_LDO;
        pmu_write(PMU_REG_BAT2DIG_CFG, val);

        pmu_read(PMU_REG_DCDC_DIG_EN, &val);
        val &= ~(REG_DCDC1_VREF_SEL_NORMAL_MASK | REG_DCDC1_VREF_SEL_DSLEEP_MASK);
        val |= REG_DCDC1_VREF_SEL_NORMAL(0) | REG_DCDC1_VREF_SEL_DSLEEP(0);
        pmu_write(PMU_REG_DCDC_DIG_EN, val);

        pmu_read(PMU1806_REG_DIG_VOLT_RC, &val);
        val = SET_BITFIELD(val, REG_DCDC1_VREF_SEL_RC, 0);
        pmu_write(PMU1806_REG_DIG_VOLT_RC, val);

        pmu_read(PMU_REG_BAT2DIG_CFG, &val);
        val &= ~LP_EN_VBAT2VCORE_LDO_DR;
        pmu_write(PMU_REG_BAT2DIG_CFG, val);
    }
}

static POSSIBLY_UNUSED void pmu_vusb_ret_ldo_off(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU1805_REG_USB_RET_LDO, &val);
        val = (val | REG_PU_VUSB_RET_LDO_DR) & ~REG_PU_VUSB_RET_LDO;
        pmu_write(PMU1805_REG_USB_RET_LDO, val);
    }
}

static void pmu_sar_adc_vref_sw_pu(bool pu)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_THERM, &val);
        if (pu) {
            val &= ~PMU1806_REG_PU_SAR_VREF_SW;
        } else {
            val |= PMU1806_REG_PU_SAR_VREF_SW;
        }
        pmu_write(PMU_REG_THERM, val);
    }
}

static POSSIBLY_UNUSED void pmu_1809_buck_volt_hw_ramp_step_set(uint8_t step)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        if (dcdc_ramp_map & REG_DCDC1_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VCORE_RAMP_CFG, &val);
            val = SET_BITFIELD(val, REG_BUCK_VCORE_RAMP_STEP, step);
            pmu_write(PMU1809_REG_BUCK_VCORE_RAMP_CFG, val);
        }

        if (dcdc_ramp_map & REG_DCDC2_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VANA_RAMP_CFG, &val);
            val = SET_BITFIELD(val, REG_BUCK_VANA_RAMP_STEP, step);
            pmu_write(PMU1809_REG_BUCK_VANA_RAMP_CFG, val);
        }

        if (dcdc_ramp_map & REG_DCDC3_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VHPPA_RAMP_CFG, &val);
            val = SET_BITFIELD(val, REG_BUCK_VHPPA_RAMP_STEP, step);
            pmu_write(PMU1809_REG_BUCK_VHPPA_RAMP_CFG, val);
        }
    }
}

static void pmu_buck_volt_hw_ramp_enable(uint8_t map)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        if (map & REG_DCDC1_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VCORE_RAMP_CFG, &val);
            val |= REG_BUCK_VCORE_RAMP_EN;
            pmu_write(PMU1809_REG_BUCK_VCORE_RAMP_CFG, val);
        }

        if (map & REG_DCDC2_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VANA_RAMP_CFG, &val);
            val |= REG_BUCK_VANA_RAMP_EN;
            pmu_write(PMU1809_REG_BUCK_VANA_RAMP_CFG, val);
        }

        if (map & REG_DCDC3_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VHPPA_RAMP_CFG, &val);
            val |= REG_BUCK_VHPPA_RAMP_EN;
            pmu_write(PMU1809_REG_BUCK_VHPPA_RAMP_CFG, val);
        }
    } else {
        pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
        val |= map;
        pmu_write(PMU_REG_DCDC_RAMP_EN, val);
    }
}

static void pmu_buck_volt_hw_ramp_disable(uint8_t map)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        if (map & REG_DCDC1_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VCORE_RAMP_CFG, &val);
            val &= ~REG_BUCK_VCORE_RAMP_EN;
            pmu_write(PMU1809_REG_BUCK_VCORE_RAMP_CFG, val);
        }

        if (map & REG_DCDC2_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VANA_RAMP_CFG, &val);
            val &= ~REG_BUCK_VANA_RAMP_EN;
            pmu_write(PMU1809_REG_BUCK_VANA_RAMP_CFG, val);
        }

        if (map & REG_DCDC3_RAMP_EN) {
            pmu_read(PMU1809_REG_BUCK_VHPPA_RAMP_CFG, &val);
            val &= ~REG_BUCK_VHPPA_RAMP_EN;
            pmu_write(PMU1809_REG_BUCK_VHPPA_RAMP_CFG, val);
        }
    } else {
        pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
        val &= ~map;
        pmu_write(PMU_REG_DCDC_RAMP_EN, val);
    }
}

int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#ifdef NO_ISPI
    return 0;
#endif

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))

    uint16_t val;
    enum PMU_POWER_MODE_T mode;
    enum PMU_MODULE_T codec;

    ASSERT(!vcodec_off || vcodec_mv == vhppa_mv,
        "Invalid vcodec/vhppa cfg: vcodec_off=%d vcodec_mv=%u vhppa_mv=%u", vcodec_off, vcodec_mv, vhppa_mv);
    ASSERT(vcodec_mv == 1350 || vcodec_mv == 1400 || vcodec_mv == 1450 || vcodec_mv == 1500 || vcodec_mv == 1550 ||
        vcodec_mv == 1600 || vcodec_mv == 1650 || vcodec_mv == 1700 || vcodec_mv == 1750 || vcodec_mv == 1800 ||
        vcodec_mv == 1900 || vcodec_mv == 1950,
        "Invalid vcodec cfg: vcodec_mv=%u", vcodec_mv);
    ASSERT(vhppa_mv == 1350 || vhppa_mv == 1400 || vhppa_mv == 1450 || vhppa_mv == 1500 || vhppa_mv == 1550 ||
        vhppa_mv == 1600 || vhppa_mv == 1650 || vhppa_mv == 1700 || vhppa_mv == 1750 || vhppa_mv == 1800 ||
        vhppa_mv == 1900 || vhppa_mv == 1950,
        "Invalid vhppa cfg: vhppa_mv=%u", vhppa_mv);
    TRACE(0, "vhppa_mv:%d vcodec_mv:%d", vhppa_mv, vcodec_mv);

    TRACE(0, "%s boot_cause=0x%04X first_pwron:%d", __func__, pmu_boot_reason, pmu_boot_first_pwr_up);

#ifdef PMU_VMEM_PWR_SUPPLY_IO
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_get_efuse(PMU__EFUSE_PAGE_RESERVED_22, &val);
        if ((val & PMU_EFUSE_FLASH_VDD_1V8) == 0) {
            pmu_split_vmem_buck3_valid = true;
            TRACE(0, "PMU_VMEM_PWR_SUPPLY_IO valid! vmem_mv=%d", vmem_mv);
        }
    }
#else
    ASSERT(vcodec_mv >= 1600, "Bad VCODEC_VOLT setting!");
#endif

#ifdef PMU_DCDC_PWM
    ASSERT(pmu_type == PMU_CHIP_TYPE_1809 && pmu_metal_id == HAL_CHIP_METAL_ID_0, "%s: Don't support fixed pwm mode!", __func__);
#endif

    pmu_module_cfg[PMU_ANA] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(ANA);
    pmu_module_cfg[PMU_DIG] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(CORE),
    pmu_module_cfg[PMU_IO] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(IO),
    pmu_module_cfg[PMU_MEM] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(MEM),
    pmu_module_cfg[PMU_GP] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(GP),
    pmu_module_cfg[PMU_USB] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(USB),
    pmu_module_cfg[PMU_BAT2DIG] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(BAT2VCORE),
    pmu_module_cfg[PMU_HPPA2CODEC] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(HPPA2VCODEC),
    pmu_module_cfg[PMU_CODEC] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(CODEC),
    pmu_module_cfg[PMU_BUCK2ANA] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(BUCK2ANA),
    pmu_module_cfg[PMU_MEM2] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(MEM2),
    pmu_module_cfg[PMU_MEM3] = (struct PMU_MODULE_CFG_T)PMU_MOD_CFG_VAL(MEM3),

    pmu_ana_dig_volt_init();
    // Disable and clear all PMU irqs by default
    pmu_write(PMU_REG_INT_MASK, 0);
    pmu_write(PMU_REG_INT_EN, 0);
    // PMU irqs cannot be cleared by PMU soft reset
    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    pmu_write(PMU_REG_CHARGER_STATUS, val);
    pmu_read(PMU_REG_INT_STATUS, &val);
    pmu_write(PMU_REG_INT_CLR, val);

    if (pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef PMU_POWER_PROTECT_DYNAMIC_CONFIG
        pmu_ext_get_efuse(PMU_EXT_EFUSE_PAGE_RESERVED_7, &val);
        pmu_pwr_protect_cur_en = !!(val & PMU_EXT_EFUSE_CURRENT_DETECT_EN);
        pmu_pwr_protect_volt_en = !!(val & PMU_EXT_EFUSE_VOLTAGE_DETECT_EN);
        TRACE(0, "%s power protect enabled! cur/volt:%d/%d", __func__, pmu_pwr_protect_cur_en, pmu_pwr_protect_volt_en);
#endif

        // Disable power abort irq mask for vrtc, vdig_codec, avdd25_inf and clear irq by default.
        pmu_read(PMU1809_REG_VRTC_PP_CFG, &val);
        val &= ~REG_VRTC_INTR_MSK;
        val |= REG_VRTC_INTR_CLR;
        pmu_write(PMU1809_REG_VRTC_PP_CFG, val);

        pmu_read(PMU1809_REG_DIG_CODEC_PP_CFG, &val);
        val &= ~REG_VDIG_CODEC_INTR_MSK;
        val |= REG_VDIG_CODEC_INTR_CLR;
        pmu_write(PMU1809_REG_DIG_CODEC_PP_CFG, val);

        pmu_read(PMU1809_REG_AVDD25_PP_CFG, &val);
        val &= ~REG_AVDD25_INF_INTR_MSK;
        val |= REG_AVDD25_INF_INTR_CLR;
        pmu_write(PMU1809_REG_AVDD25_PP_CFG, val);

#ifdef PMU_POWER_PROTECT_VOLT_DISABLE
        // Disable OV/UV det
        pmu_write(PMU1809_REG_PP_OV_UV_EN, 0);
#else
        // Disable power detect function for vrtc, vdig_codec, avdd25_inf and vmic.
        pmu_read(PMU1809_REG_PP_UV_MSK, &val);
        val &= ~(REG_VRTC_UV_MSK | REG_VDIG_CODEC_UV_MSK | REG_AVDD25_INF_UV_MSK);
        if (pmu_metal_id <= HAL_CHIP_METAL_ID_2) {
            val &= ~(REG_VMICA_UV_MSK | REG_VMICB_UV_MSK);
        }
        pmu_write(PMU1809_REG_PP_UV_MSK, val);
#endif

#ifdef PMU_POWER_PROTECT_FORCE_HW_PMU_OFF
        pmu_power_protect_module_force_hw_pmu_off_config();
#endif

        // Only pmu re-powered can clear abort irq.
        for (enum PMU_POWER_PROTECT_MODULE_T module = PMU_POWER_PROTECT_MODULE_LDO_VBAT2VCORE; module < PMU_POWER_PROTECT_MODULE_QTY; module++) {
            pmu_power_protect_module_irq_clr(module);
            enum PMU_POWER_PROTECT_ABORT_SOURCE_T source;

            source = pmu_power_protect_abort_source_get(module);
            if (source) {
                TRACE(0, "%s: ##abort happened! module:%d source=0x%02X", __func__, module, source);
            }
        }

#ifdef PMU_POWER_PROTECT_DYNAMIC_CONFIG
        pmu_power_protect_irq_handler_set(pmu_power_protect_irq_handler_dummy);
#endif
    }

    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    // Allow PMU to sleep when power key is pressed
    val &= ~POWERKEY_WAKEUP_OSC_EN;
    // Increase big bandgap startup time (stable time)
    val = SET_BITFIELD(val, REG_VCORE_SSTIME_MODE, 2);
    pmu_write(PMU_REG_POWER_KEY_CFG, val);

    // Set VIO VUSB vbit sel fine as default.
    if (pmu_type == PMU_CHIP_TYPE_1808) {
        pmu_read(PMU1808_REG_VIO_CFG_BB, &val);
        val |= REG_LDO_VIO_RES_SEL_FINE;
        pmu_write(PMU1808_REG_VIO_CFG_BB, val);
        pmu_read(PMU1808_REG_VUSB_RES_SEL_FINE, &val);
        val = SET_BITFIELD(val, REG_LDO_VUSB_RES_SEL_FINE, 3);
        pmu_write(PMU1808_REG_VUSB_RES_SEL_FINE, val);
    }

    // Disable fast ramp as default.
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU1809_REG_FAST_RAMP_CFG, &val);
        val = (val & ~REG_RAMP_FAST_EN) | REG_RAMP_FAST_EN_DR;
        pmu_write(PMU1809_REG_FAST_RAMP_CFG, val);
    }

#if defined(DCDC_VOLT_HW_RAMP_ONLY)
    if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
        dcdc_ramp_en = true;
        pmu_buck_volt_hw_ramp_enable(REG_DCDC1_RAMP_EN | REG_DCDC2_RAMP_EN | REG_DCDC3_RAMP_EN);
    }
#else
    pmu_buck_volt_hw_ramp_disable(REG_DCDC1_RAMP_EN | REG_DCDC2_RAMP_EN | REG_DCDC3_RAMP_EN);
#endif

#ifndef PMU_FORCE_LP_MODE
#ifdef FORCE_BIG_BANDGAP
    // Force big bandgap
    pmu_read(PMU_REG_INT_EN, &val);
    val |= REG_BG_SLEEP_MSK;
    pmu_write(PMU_REG_INT_EN, val);
    pmu_read(PMU_REG_BIAS_CFG, &val);
    val |= BG_CONSTANT_GM_BIAS_DR | BG_CONSTANT_GM_BIAS_REG;
    val |= BG_CORE_EN_DR | BG_CORE_EN_REG;
    pmu_write(PMU_REG_BIAS_CFG, val);
    if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
        uint16_t val_vtoi;

        pmu_read(PMU1806_REG_VTOI_CFG, &val_vtoi);
        val_vtoi |= BG_VTOI_EN_DR | BG_VTOI_EN_REG;
        pmu_write(PMU1806_REG_VTOI_CFG, val_vtoi);
    }
    hal_sys_timer_delay_us(20);
    val |= BG_VBG_SEL_DR | BG_VBG_SEL_REG;
    pmu_write(PMU_REG_BIAS_CFG, val);
#else
    // Allow low power bandgap
    pmu_read(PMU_REG_BIAS_CFG, &val);
    val &= ~BG_VBG_SEL_DR;
    pmu_write(PMU_REG_BIAS_CFG, val);
#endif
#endif

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU_REG_DCDC_HPPA_CFG_1B, &val);
        val = SET_BITFIELD(val, DCDC3_DELTAV, 0x20);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1B, val);

        pmu_read(PMU_REG_DCDC_ANA_CFG_17, &val);
        val = SET_BITFIELD(val, DCDC2_DELTAV, 0x20);
        pmu_write(PMU_REG_DCDC_ANA_CFG_17, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_34, &val);
        val = SET_BITFIELD(val, DCDC1_DELTAV, 0x20);
        pmu_write(PMU_REG_DCDC_DIG_CFG_34, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU_REG_DCDC_HPPA_CFG_1B, &val);
        val = SET_BITFIELD(val, DCDC3_DELTAV, 0x30);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1B, val);

        pmu_read(PMU_REG_DCDC_ANA_CFG_17, &val);
        val = SET_BITFIELD(val, DCDC2_DELTAV, 0x30);
        pmu_write(PMU_REG_DCDC_ANA_CFG_17, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_34, &val);
        val = SET_BITFIELD(val, DCDC1_DELTAV, 0x30);
        pmu_write(PMU_REG_DCDC_DIG_CFG_34, val);

        pmu_read(PMU_REG_DCDC_HPPA_CFG_1C, &val);
        val &= ~DCDC3_IHALF_IRCOMP;
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1C, val);

        pmu_read(PMU_REG_DCDC_ANA_CFG_18, &val);
        val &= ~DCDC2_IHALF_IRCOMP;
        pmu_write(PMU_REG_DCDC_ANA_CFG_18, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_35, &val);
        val &= ~DCDC1_IHALF_IRCOMP;
        pmu_write(PMU_REG_DCDC_DIG_CFG_35, val);
    }

    // Init DCDC settings part 1
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        uint16_t burst_threshold_cal;

        TRACE(0, "PMU_CHIP_TYPE_1809, pmu_metal_id:%d", pmu_metal_id);
#ifdef HPPA_LDO_ON
        ASSERT(false, "%s: No hppa ldo in 1809", __func__);
#endif

        pmu_read(PMU1809_REG_BUCK_VCORE_IS_GAIN, &val);
        val = SET_BITFIELD(val, REG_BUCK_VCORE_IS_GAIN_NORMAL, 0x4);
        pmu_write(PMU1809_REG_BUCK_VCORE_IS_GAIN, val);

        pmu_read(PMU1809_REG_BUCK_VANA_IS_GAIN, &val);
        val = SET_BITFIELD(val, REG_BUCK_VANA_IS_GAIN_NORMAL, 0x4);
        pmu_write(PMU1809_REG_BUCK_VANA_IS_GAIN, val);

        pmu_read(PMU1809_REG_BUCK_VHPPA_IS_GAIN, &val);
        val = SET_BITFIELD(val, REG_BUCK_VHPPA_IS_GAIN_NORMAL, 0x5);
        pmu_write(PMU1809_REG_BUCK_VHPPA_IS_GAIN, val);

        pmu_read(PMU1809_REG_BUCK_VCORE_CFG_137, &val);
        val = SET_BITFIELD(val, BUCK_VCORE_BURST_THRESHOLD, 0x1C);
        pmu_write(PMU1809_REG_BUCK_VCORE_CFG_137, val);

        pmu_get_efuse(PMU_EFUSE_PAGE_LDO_CALIB, &val);
        burst_threshold_cal = GET_BITFIELD(val, PMU_EFUSE_BURST_THRESHOLD_CAL);

        pmu_read(PMU1809_REG_BUCK_VANA_CFG_131, &val);
        if (burst_threshold_cal) {
            val = SET_BITFIELD(val, BUCK_VANA_BURST_THRESHOLD, burst_threshold_cal);
        } else {
            val = SET_BITFIELD(val, BUCK_VANA_BURST_THRESHOLD, 0x1C);
        }
        pmu_write(PMU1809_REG_BUCK_VANA_CFG_131, val);

        pmu_read(PMU1809_REG_BUCK_VHPPA_CFG_12A, &val);
        val = SET_BITFIELD(val, BUCK_VHPPA_BURST_THRESHOLD, 0x18);
        pmu_write(PMU1809_REG_BUCK_VHPPA_CFG_12A, val);

        pmu_read(PMU1809_REG_BUCK_VCORE_CFG_136, &val);
        val &= ~BUCK_VCORE_PWR_OK_SEL;
        pmu_write(PMU1809_REG_BUCK_VCORE_CFG_136, val);

        pmu_read(PMU1809_REG_BUCK_VANA_CFG_130, &val);
        val &= ~BUCK_VANA_PWR_OK_SEL;
        pmu_write(PMU1809_REG_BUCK_VANA_CFG_130, val);

        pmu_read(PMU1809_REG_BUCK_VHPPA_CFG_129, &val);
        val &= ~BUCK_VHPPA_PWR_OK_SEL;
        pmu_write(PMU1809_REG_BUCK_VHPPA_CFG_129, val);

        pmu_read(PMU1809_REG_BUCK_VCORE_CFG_13B, &val);
        // buck_vcore_reserved[7:0]
        val &= ~(1 << 0);
        pmu_write(PMU1809_REG_BUCK_VCORE_CFG_13B, val);

        pmu_read(PMU1809_REG_BUCK_VANA_CFG_135, &val);
        // buck_vana_reserved[7:0]
        val &= ~(1 << 0);
        pmu_write(PMU1809_REG_BUCK_VANA_CFG_135, val);

    } else if (pmu_type == PMU_CHIP_TYPE_1806) {
        TRACE(0, "PMU_CHIP_TYPE_1806, pmu_metal_id:%d", pmu_metal_id);
#ifdef HPPA_LDO_ON
        ASSERT(false, "%s: No hppa ldo in 1806", __func__);
#endif
        pmu_write(PMU1806_REG_GPIO_DRV_SEL, 0);

        // DCDC2(Vana_1.1V) config start
        // Configure DCDC2 burst and pwm switching threshold
        pmu_read(PMU1806_REG_DCDC2_EDGE_CON, &val);
        val |= DCDC2_REG_RES_FB;
        pmu_write(PMU1806_REG_DCDC2_EDGE_CON, val);

        pmu_read(PMU_REG_DCDC_ANA_CFG_16, &val);
        val &= ~(DCDC2_BURST_THRESHOLD_MASK | DCDC2_CAP_BIT_MASK);
        val |= (DCDC2_BURST_THRESHOLD(0x1F) | DCDC2_CAP_BIT(0x70));
        val &= ~DCDC2_CAP_BIT_OSC_MASK;
        val |= DCDC2_CAP_BIT_OSC(0x8);
        pmu_write(PMU_REG_DCDC_ANA_CFG_16, val);

        pmu_read(PMU_REG_DCDC_ANA_CFG_18, &val);
        val &= ~(DCDC2_IS_GAIN_PFM_MASK | DCDC2_IS_GAIN_MASK |
            DCDC2_INTERNAL_FREQ_MASK | DCDC2_HYSTERESIS_BIT_MASK);
        val |= (DCDC2_IS_GAIN_PFM(0x4) | DCDC2_IS_GAIN(0x2) |
            DCDC2_INTERNAL_FREQ(0x2) | DCDC2_HYSTERESIS_BIT(0x5));
        val |= DCDC2_IHALF_IRCOMP;
        pmu_write(PMU_REG_DCDC_ANA_CFG_18, val);
        // DCDC2(Vana_1.1V) config end

        // DCDC3(Vcodec_1.8V) config start /audio_codec/audio_pa/rf_pa
        // Configure DCDC3 burst and pwm switching threshold
        pmu_read(PMU_REG_DCDC_HPPA_CFG_1A, &val);
        val &= ~(DCDC3_BURST_THRESHOLD_MASK | DCDC3_CAP_BIT_MASK);
        val |= (DCDC3_BURST_THRESHOLD(0x1F) | DCDC3_CAP_BIT(0x70));
        val &= ~DCDC3_CAP_BIT_OSC_MASK;
        val |= DCDC3_CAP_BIT_OSC(0x8);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1A, val);

        pmu_read(PMU_REG_DCDC_HPPA_CFG_1C, &val);
        val &= ~(DCDC3_IS_GAIN_PFM_MASK | DCDC3_IS_GAIN_MASK |
            DCDC3_INTERNAL_FREQ_MASK | DCDC3_HYSTERESIS_BIT_MASK);
        val |= (DCDC3_IS_GAIN_PFM(0x4) | DCDC3_IS_GAIN(0x4) |
            DCDC3_INTERNAL_FREQ(0x2) | DCDC3_HYSTERESIS_BIT(0x5));
        val |= DCDC3_IHALF_IRCOMP;
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1C, val);
        // DCDC3(Vcodec_1.8V) config end

        // DCDC1(Vcore) config start
        // Configure DCDC1 burst and pwm switching threshold
        pmu_read(PMU1806_REG_DCDC1_EDGE_CON, &val);
        val |= DCDC1_REG_RES_FB;
        pmu_write(PMU1806_REG_DCDC1_EDGE_CON, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_33, &val);
        val &= ~(DCDC1_BURST_THRESHOLD_MASK | DCDC1_CAP_BIT_MASK);
        val |= (DCDC1_BURST_THRESHOLD(0x1F) | DCDC1_CAP_BIT(0x70));
        val &= ~DCDC1_CAP_BIT_OSC_MASK;
        val |= DCDC1_CAP_BIT_OSC(0x8);
        pmu_write(PMU_REG_DCDC_DIG_CFG_33, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_35, &val);
        val &= ~(DCDC1_IS_GAIN_PFM_MASK | DCDC1_IS_GAIN_MASK |
            DCDC1_INTERNAL_FREQ_MASK | DCDC1_HYSTERESIS_BIT_MASK);
        val |= (DCDC1_IS_GAIN_PFM(0x4) | DCDC1_IS_GAIN(0x2) |
            DCDC1_INTERNAL_FREQ(0x2) | DCDC1_HYSTERESIS_BIT(0x5));
        val |= DCDC1_IHALF_IRCOMP;
        pmu_write(PMU_REG_DCDC_DIG_CFG_35, val);
        // DCDC1(Vcore) config end

        // config DCDC offset start
        pmu_read(PMU_REG_MIC_LDO_EN, &val);
        val |= DCDC1_OFFSET_CURRENT_EN | DCDC2_OFFSET_CURRENT_EN |
            DCDC3_OFFSET_CURRENT_EN;
        pmu_write(PMU_REG_MIC_LDO_EN, val);

        pmu_read(PMU_REG_DCDC1_OFFSET, &val);
        val &= ~DCDC1_OFFSET_BIT_MASK;
        val |= DCDC1_OFFSET_BIT(0x10);
        val &= ~DCDC2_OFFSET_BIT_MASK;
        val |= DCDC2_OFFSET_BIT(0x14);
        pmu_write(PMU_REG_DCDC1_OFFSET, val);

        pmu_read(PMU_REG_DCDC3_OFFSET, &val);
        val &= ~DCDC3_OFFSET_BIT_MASK;
        val |= DCDC3_OFFSET_BIT(0x1C);
        pmu_write(PMU_REG_DCDC3_OFFSET, val);
        // config DCDC offset end

        pmu_read(PMU_REG_LDO_VCORE_L, &val);
        val |= REG_LDO_VCORE_L_BYPASS_DR;
        pmu_write(PMU_REG_LDO_VCORE_L, val);

        val = REG_PU_LDO_DIG_L_DSLEEP | REG_PU_LDO_DIG_M_DSLEEP |
            REG_PU_LDO_DIG_L_DR | REG_PU_LDO_DIG_M_DR |
            REG_PU_VMEM_RET_LDO_DR | REG_PU_VIO_RET_LDO_DR |
            REG_PU_VDCDC_RET_LDO_DR | REG_PU_VCORE_L1_RET_LDO_DR |
            REG_PU_VBAT2VCORE_RET_LDO_DR;
        pmu_write(PMU_REG_PU_LDO, val);

        pmu_xo32k_ldo_pu_enable();
    } else if (pmu_type == PMU_CHIP_TYPE_1805) {
        TRACE(0, "PMU_CHIP_TYPE_1805, pmu_metal_id:%d", pmu_metal_id);
#ifdef HPPA_LDO_ON
        ASSERT(false, "%s: No hppa ldo in 1805", __func__);
#endif
        if (pmu_metal_id == HAL_CHIP_METAL_ID_1) {
            // for dcdc2 ripple
            pmu_read(PMU_REG_RESERVED_PMU, &val);
            val = SET_BITFIELD(val, PMU_REG_RESERVED_ANA_HV1, 0x0);
            val = SET_BITFIELD(val, PMU_REG_RESERVED_ANA_HV2, 0x2);
            val = SET_BITFIELD(val, PMU_REG_RESERVED_ANA_HV3, 0x2);
            pmu_write(PMU_REG_RESERVED_PMU, val);
        } else if (pmu_metal_id >= HAL_CHIP_METAL_ID_2) {
            // for dcdc ripple, don't influence VUSB/VMEM2 startup.
            pmu_read(PMU_REG_SOFT_START_CFG, &val);
            val &= ~(REG_SOFT_START2_VUSB | REG_SOFT_START2_VMEM);
            pmu_write(PMU_REG_SOFT_START_CFG, val);

            pmu_read(PMU_REG_RESERVED_PMU, &val);
            val &= ~(DCDC2_ERR_BIAS | DCDC2_IREF_BURST0 | DCDC3_IREF_BURST0 | DCDC1_IREF_BURST0);
            val |= DCDC1_ERR_BIAS | DCDC3_ERR_BIAS | DCDC2_IREF_BURST1 | DCDC1_IREF_BURST1 | DCDC3_IREF_BURST1;
            pmu_write(PMU_REG_RESERVED_PMU, val);
        }

        pmu_read(PMU_REG_DCDC_ANA_CFG_16, &val);
        val &= ~(DCDC2_BURST_THRESHOLD_MASK | DCDC2_CAP_BIT_MASK);
        val |= (DCDC2_BURST_THRESHOLD(0x18) | DCDC2_CAP_BIT(0x70));
        val &= ~DCDC2_CAP_BIT_OSC_MASK;
        val |= DCDC2_CAP_BIT_OSC(0x8);
        pmu_write(PMU_REG_DCDC_ANA_CFG_16, val);

        pmu_read(PMU_REG_DCDC_HPPA_CFG_1A, &val);
        val &= ~(DCDC3_BURST_THRESHOLD_MASK | DCDC3_CAP_BIT_MASK);
        val |= (DCDC3_BURST_THRESHOLD(0x12) | DCDC3_CAP_BIT(0x70));
        val &= ~DCDC3_CAP_BIT_OSC_MASK;
        val |= DCDC3_CAP_BIT_OSC(0x8);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1A, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_33, &val);
        val &= ~(DCDC1_BURST_THRESHOLD_MASK | DCDC1_CAP_BIT_MASK);
        val |= (DCDC1_BURST_THRESHOLD(0x18) | DCDC1_CAP_BIT(0x70));
        val &= ~DCDC1_CAP_BIT_OSC_MASK;
        val |= DCDC1_CAP_BIT_OSC(0x8);
        pmu_write(PMU_REG_DCDC_DIG_CFG_33, val);

        // Reduce DCDC voltage ripple.
        pmu_read(PMU_REG_DCDC_DIG_CFG_36, &val);
        val |= DCDC1_IX2_ERR;
        pmu_write(PMU_REG_DCDC_DIG_CFG_36, val);

        pmu_read(PMU_REG_DCDC_ANA_CFG_19, &val);
        val |= DCDC2_IX2_ERR;
        pmu_write(PMU_REG_DCDC_ANA_CFG_19, val);

        pmu_read(PMU_REG_DCDC_HPPA_CFG_1D, &val);
        val |= DCDC3_IX2_ERR;
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1D, val);

        if (pmu_metal_id < HAL_CHIP_METAL_ID_2) {
            pmu_read(PMU1806_REG_DCDC2_EDGE_CON, &val);
            val |= DCDC2_SLEEP_DELAY;
            pmu_write(PMU1806_REG_DCDC2_EDGE_CON, val);
        }

        pmu_read(PMU_REG_PU_LDO, &val);
        val = REG_PU_LDO_DIG_L_DSLEEP | REG_PU_LDO_DIG_M_DSLEEP |
            REG_PU_LDO_DIG_L_DR | REG_PU_LDO_DIG_M_DR |
            REG_PU_VMEM_RET_LDO_DR | REG_PU_VIO_RET_LDO_DR |
            REG_PU_VDCDC_RET_LDO_DR | REG_PU_VCORE_L1_RET_LDO_DR |
            REG_PU_VBAT2VCORE_RET_LDO_DR;
        pmu_write(PMU_REG_PU_LDO, val);

        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val &= ~DCDC2_IS_GAIN_1805_MASK;
        val |= DCDC2_IS_GAIN_1805(0x10);
        val &= ~DCDC3_IS_GAIN_1805_MASK;
        val |= DCDC3_IS_GAIN_1805(0x10);
        pmu_write(PMU_REG_PU_VSYS_DIV, val);

#ifndef SPLIT_VMEM_VMEM2
        // 1805 metal-0 needs to split VMEM and VMEM2
        if (pmu_metal_id == HAL_CHIP_METAL_ID_0) {
            pmu_read(PMU1806_REG_CODEC_VCM, &val);
            val |= REG_VCM_EN;
            val &= ~(REG_VCM_EN_LPF | REG_VCM_LP_EN);
            pmu_write(PMU1806_REG_CODEC_VCM, val);
            hal_sys_timer_delay(MS_TO_TICKS(5));

            pmu_read(PMU_REG_CODEC_CFG, &val);
            val = SET_BITFIELD(val, LDO_VCODEC_VBIT_NORMAL, 0x1A);
            val = SET_BITFIELD(val, LDO_VCODEC_VBIT_DSLEEP, 0x1A);
            pmu_write(PMU_REG_CODEC_CFG, val);

            pmu_read(PMU1806_REG_LDO_VMEM2, &val);
            val |= (PMU1806_REG_PU_LDO_VMEM2_DR | PMU1806_REG_PU_LDO_VMEM2_REG);
            pmu_write(PMU1806_REG_LDO_VMEM2, val);

            pmu_read(PMU1806_REG_LDO_VMEM2, &val);
            val &= ~PMU1806_REG_LDO_VMEM2_BYPASS;
            pmu_write(PMU1806_REG_LDO_VMEM2, val);

            pmu_read(PMU1806_REG_CODEC_VCM, &val);
            val |= REG_VCM_EN_LPF;
            pmu_write(PMU1806_REG_CODEC_VCM, val);
        }
#endif
    } else if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
        if (pmu_type == PMU_CHIP_TYPE_1803) {
            TRACE(0, "PMU_CHIP_TYPE_1803, pmu_metal_id:%d", pmu_metal_id);
        } else {
            TRACE(0, "PMU_CHIP_TYPE_1808, pmu_metal_id:%d", pmu_metal_id);
        }
#ifdef DCDC_HIGH_EFFICIENCY
        val = 0x0217;
#else
        val = 0x0211;
#endif

        uint16_t val_burst_threshold = 0x8E1F;

#ifdef PMU_RELAX_BURST_THRESHOLD
        val_burst_threshold = SET_BITFIELD(val_burst_threshold, DCDC3_BURST_THRESHOLD, 0x1C);
#endif
        pmu_write(PMU_REG_DCDC_ANA_CFG_16, val_burst_threshold);
        pmu_write(PMU_REG_DCDC_ANA_CFG_17, val);
        pmu_write(PMU_REG_DCDC_ANA_CFG_18, 0xA444);
        pmu_write(PMU_REG_DCDC_ANA_CFG_19, 0xE210);

        pmu_write(PMU_REG_DCDC_HPPA_CFG_1A, val_burst_threshold);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1B, val);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1C, 0xA484);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1D, 0xE210);

        pmu_write(PMU_REG_DCDC_DIG_CFG_33, val_burst_threshold);
        pmu_write(PMU_REG_DCDC_DIG_CFG_34, val);
        pmu_write(PMU_REG_DCDC_DIG_CFG_35, 0xA484);
        pmu_write(PMU_REG_DCDC_DIG_CFG_36, 0xE210);

        // Fix working current too high, wrong default val.
        pmu_read(PMU_REG_DCDC_HPPA_CFG_1B, &val);
        val = SET_BITFIELD(val, PMU1803_DCDC3_DELTAV, 0x8);
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1B, val);

        pmu_read(PMU_REG_DCDC_ANA_CFG_17, &val);
        val = SET_BITFIELD(val, PMU1803_DCDC2_DELTAV, 0x8);
        pmu_write(PMU_REG_DCDC_ANA_CFG_17, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_34, &val);
        val = SET_BITFIELD(val, PMU1803_DCDC1_DELTAV, 0x8);
        pmu_write(PMU_REG_DCDC_DIG_CFG_34, val);

        pmu_read(PMU_REG_MIC_LDO_EN, &val);
        val |= DCDC1_OFFSET_CURRENT_EN;
        pmu_write(PMU_REG_MIC_LDO_EN, val);

        pmu_read(PMU_REG_DCDC1_OFFSET, &val);
        val = SET_BITFIELD(val, DCDC1_OFFSET_BIT, 0xE0);
        pmu_write(PMU_REG_DCDC1_OFFSET, val);
    }

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU1806_REG_1P8_SWITCH, &val);
#if defined(SYS_GPIO_SLEEP_POWER_UP)
        // Power up gpio p3~p9 during sleep
        val |= REG_EN_SW2_1P8 | REG_EN_SW2_1P8_DR;
#endif
#if defined(PSRAM_ENABLE) || defined(PSRAM_SLEEP_RET)
        // Power up psram/dsi 1.8v during sleep
        val |= REG_EN_SW3_1P8 | REG_EN_SW3_1P8_DR;
#endif
        // Power up gpio p0-p1 during sleep (gpio p2 is always on)
        val |= REG_EN_SW4_1P8 | REG_EN_SW4_1P8_DR;
        pmu_write(PMU1806_REG_1P8_SWITCH, val);
    }

#ifdef PMU_DCDC_CALIB
    pmu_get_dcdc_calib_value();
#endif

#ifdef PMU_LDO_VCORE_CALIB
    pmu_get_ldo_dig_calib_value();
#endif

#ifndef NO_SLEEP
    pmu_sleep_en(true);
#endif

    pmu_dcdc_vref_set();

    // Disable vcore ldo
    pmu_module_config(PMU_DIG,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#ifdef LBRT
    // Enable vbuck2ana
    pmu_module_config(PMU_BUCK2ANA,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    pmu_module_set_volt(PMU_BUCK2ANA, PMU_VBUCK2ANA_1_3V, PMU_VBUCK2ANA_1_3V);
    if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
        // Disable vana dcdc
        pmu_read(PMU_REG_HPPA_LDO_EN, &val);
        val &= ~REG_BYPASS_VBUCK2ANA;
        pmu_write(PMU_REG_HPPA_LDO_EN, val);
    }
#else
    // Disable vbuck2ana
    pmu_module_config(PMU_BUCK2ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif

    // Disable vhppa ldo
    pmu_ldo_hppa_en(false);

#ifdef VMEM_ON
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    pmu_module_ramp_volt(PMU_MEM, PMU_VMEM_1_8V, PMU_VMEM_1_8V);
    pmu_hppa_en(false);
    // VHPPA will be enabled/disabled dynamically. Enable soft start.
    if (pmu_type != PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_DCDC_HPPA_CFG_1D, &val);
        val |= DCDC3_SOFT_START_EN;
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1D, val);
    }
#ifdef PMU_VMEM_PWR_SUPPLY_IO
    if (pmu_type == PMU_CHIP_TYPE_1809 && pmu_split_vmem_buck3_valid) {
        ASSERT(false, "VMEM_ON conflict with PMU_VMEM_PWR_SUPPLY_IO!");
    }
#endif
#else
    // vmem and vhppa is shorted -- enable vhppa dcdc and disable vmem ldo
    val = pmu_vhppa_mv_to_val(1800);
    pmu_hppa_set_volt(val, val);
    pmu_hppa_en(true);
#ifdef PMU_VMEM_PWR_SUPPLY_IO
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        if (pmu_split_vmem_buck3_valid) {
            pmu_read(PMU1809_REG_LDO_VMEM2_CFG, &val);
            val &= ~PMU1809_REG_LDO_VMEM2_BYPASS;
            pmu_write(PMU1809_REG_LDO_VMEM2_CFG, val);
            val = pmu_ldo_vmem_mv_to_val(vmem_mv);
            pmu_module_ramp_volt(PMU_MEM, val, val);
            pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#ifndef PMU_FORCE_LP_MODE
            // Avoid voltage gap between work and deeep sleep mode.
            pmu_read(PMU_REG_MEM_CFG, &val);
            val |= LP_EN_VMEM_LDO_DR | LP_EN_VMEM_LDO_REG;
            pmu_write(PMU_REG_MEM_CFG, val);
#endif
        } else {
            pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        }
    }
#else
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif
#endif

    pmu_module_config(PMU_GP,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

#if !defined(PROGRAMMER) || (defined(MTEST_ENABLED) && defined(SLEEP_TEST))
    pmu_vusb_ret_ldo_off();
    // Disable usbphy power, and vusb if possible
    pmu_usb_config(PMU_USB_CONFIG_TYPE_NONE);
#endif

    // Disable vcodec
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    // Disable vhppa2vcodec
    pmu_module_config(PMU_HPPA2CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#ifdef VHPPA2VCODEC
    codec = PMU_HPPA2CODEC;
#else
    codec = PMU_CODEC;
#endif
    val = pmu_vcodec_mv_to_val(vcodec_mv);
    // No need to ramp because the LDO is disabled (and its soft start is enabled)
    pmu_module_set_volt(codec, val, val);

    val = pmu_vhppa_mv_to_val(vhppa_mv);
#ifdef VMEM_ON
    // No need to ramp because VHPPA is disabled (and its soft start is enabled)
    pmu_hppa_set_volt(val, val);
#else
    uint16_t old_act_volt;
    uint16_t old_lp_volt;

    // VHPPA is on. Ramp is needed.
    pmu_hppa_get_volt(&old_act_volt, &old_lp_volt);
    if (old_act_volt < val) {
#if defined(DCDC_VOLT_HW_RAMP_ONLY) && !defined(HPPA_LDO_ON)
        if (dcdc_ramp_en) {
            pmu_hppa_set_volt(val, val);
            hal_sys_timer_delay_us(PMU_DCDC_HW_RAMP_STABLE_TIME_US);
        } else {
            while (old_act_volt++ < val) {
                pmu_hppa_set_volt(old_act_volt, val);
            }
        }
#else
        while (old_act_volt++ < val) {
            pmu_hppa_set_volt(old_act_volt, val);
        }
#endif
    } else if (old_act_volt != val || old_lp_volt != val) {
        pmu_hppa_set_volt(val, val);
    }
#endif

    uint16_t mic_volt_mv;
#ifdef DIGMIC_HIGH_VOLT
    mic_volt_mv = 3300;
#else
    mic_volt_mv = 2200;
#endif
    pmu_codec_mic_bias_set_volt((AUD_VMIC_MAP_VMIC1 << MAX_VMIC_CH_NUM) - 1, mic_volt_mv);

#ifdef DIG_DCDC_MODE
    mode = PMU_POWER_MODE_DIG_DCDC;
#elif defined(ANA_DCDC_MODE)
    mode = PMU_POWER_MODE_ANA_DCDC;
#else // LDO_MODE
    mode = PMU_POWER_MODE_LDO;
#endif

    pmu_mode_change(mode);

#ifdef PMU_FORCE_LP_MODE
#ifndef DIG_DCDC_MODE
#error "VCORE LDO cannot be forced to run in LP mode"
#endif
    // Reduce LDO voltage ripple
    pmu_force_lp_ldo_lp_bias_enhance();

    // Force LP mode
    pmu_module_force_lp_config();
    pmu_codec_mic_bias_noremap_force_lp_config();
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Switch to little bandgap
    pmu_read(PMU_REG_BIAS_CFG, &val);
    val = (val & ~BG_VBG_SEL_REG) | BG_VBG_SEL_DR;
    pmu_write(PMU_REG_BIAS_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    if (big_bandgap_user_map == 0) {
        // Disable big bandgap
        val = (val & ~(PU_BIAS_LDO_REG | BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_NOTCH_EN_REG)) |
            PU_BIAS_LDO_DR | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_NOTCH_EN_DR;
        pmu_write(PMU_REG_BIAS_CFG, val);
        if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
            pmu_read(PMU1806_REG_VTOI_CFG, &val);
            val = (val & ~BG_VTOI_EN_REG) | BG_VTOI_EN_DR;
            pmu_write(PMU1806_REG_VTOI_CFG, val);

            if (pmu_type == PMU_CHIP_TYPE_1809) {
                pmu_read(PMU1809_REG_VBG_VREF_CFG, &val);
                val |= BG_VREF_SEL_DR | BG_VREF_GEN_EN_DR;
                pmu_write(PMU1809_REG_VBG_VREF_CFG, val);
            }
        }
    }
#endif

#ifdef PMU_CLK_USE_EXT_CRYSTAL
    pmu_external_crystal_ldo_ctrl(false);
    pmu_external_crystal_enable();
#endif

#ifdef RTC_ENABLE
    pmu_rtc_restore_context();
#endif

#ifdef RTC_CALENDAR
    pmu_rtc_calendar_init();
    pmu_rtc_calendar_restore_context();
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE)
    pmu_high_performance_mode_enable(true);
#endif

    if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
        analog_vcm_pmu_on(true);
    }

    // Disable sar vref output
    pmu_sar_adc_vref_sw_pu(false);
    pmu_set_sar_vref();
    pmu_sar_adc_config();

    pmu_codec_tx_enable();

#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

    return 0;
}

enum PMU_CHIP_TYPE_T BOOT_TEXT_FLASH_LOC pmu_get_chip_type(void)
{
    return pmu_type;
}

void pmu_big_bandgap_enable(enum PMU_BIG_BANDGAP_USER_T user, int enable)
{
#ifdef PMU_FORCE_LP_MODE
    uint16_t val;
    uint16_t val_vtoi;
    bool update = false;

    if (enable) {
        if (big_bandgap_user_map == 0) {
            update = true;
        }
        big_bandgap_user_map |= user;
    } else {
        if (big_bandgap_user_map & user) {
            big_bandgap_user_map &= ~user;
            if (big_bandgap_user_map == 0) {
                update = true;
            }
        }
    }

    if (!update) {
        return;
    }

    pmu_read(PMU_REG_BIAS_CFG, &val);
    val |= BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR;
    if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
        pmu_read(PMU1806_REG_VTOI_CFG, &val_vtoi);
        val_vtoi |= BG_VTOI_EN_DR;
    }
    if (enable) {
        val |= BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG;
        if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
            val_vtoi |= BG_VTOI_EN_REG;
        }
    } else {
        val &= ~(BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG);
        if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
            val_vtoi &= ~BG_VTOI_EN_REG;
        }
    }
    pmu_write(PMU_REG_BIAS_CFG, val);
    if (!(pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808)) {
        pmu_write(PMU1806_REG_VTOI_CFG, val_vtoi);
    }
    if (enable) {
        hal_sys_timer_delay_us(PMU_BIG_BG_STABLE_TIME_US);
    }
#endif
}

void pmu_codec_vad_save_power(void)
{
    uint16_t val;

    // reduce ldo power supply
    pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
    val &= ~REG_MIC_BIASA_IX2;
    pmu_write(PMU_REG_DCDC_RAMP_EN, val);

    // enable dcdc1 low power mode
    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
    val |= REG_DCDC1_ULP_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_DIG_EN, val);

    // enable dcdc2 low power mode
    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    val |= REG_DCDC2_ULP_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_ANA_EN, val);

    // enable dcdc3 low power mode
    pmu_read(PMU_REG_DCDC_HPPA_EN, &val);
    val |= REG_DCDC3_ULP_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_HPPA_EN, val);
}

void pmu_codec_vad_restore_power(void)
{
    uint16_t val;

    // restore ldo power supply
    pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
    val |= REG_MIC_BIASA_IX2;
    pmu_write(PMU_REG_DCDC_RAMP_EN, val);

    // disable dcdc1 low power mode
    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
    val &= ~REG_DCDC1_ULP_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_DIG_EN, val);

    // disable dcdc2 low power mode
    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    val &= ~REG_DCDC2_ULP_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_ANA_EN, val);

    // disable dcdc3 low power mode
    pmu_read(PMU_REG_DCDC_HPPA_EN, &val);
    val &= ~REG_DCDC3_ULP_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_HPPA_EN, val);
}

void pmu_sleep(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1808) {
        pmu_read(PMU_REG_DCDC_ANA_CFG_19, &val);
        val &= ~DCDC2_TEST_MODE_EN;
        pmu_write(PMU_REG_DCDC_ANA_CFG_19, val);

        pmu_read(PMU_REG_DCDC_HPPA_CFG_1D, &val);
        val &= ~DCDC3_TEST_MODE_EN;
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1D, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_36, &val);
        val &= ~DCDC1_TEST_MODE_EN;
        pmu_write(PMU_REG_DCDC_DIG_CFG_36, val);
    }

#ifdef PMU_BUCK_DIRECT_ULP_MODE
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_1809_buck_volt_hw_ramp_step_set(1);
    }
#endif

    if (!dcdc_ramp_en) {
        if (dcdc_ramp_map) {
            pmu_buck_volt_hw_ramp_enable(dcdc_ramp_map);
        }
    }

#if defined(PMU_GPIO_CLK_SOURCE_24M) && defined(PMU_GPIO_CLK_SOURCE_SWITCH)
    if (pmu_gpio_irq_enabled()) {
        pmu_gpio_irq_cfg_dsleep();
    }
#endif

    return;
}

void pmu_wakeup(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1808) {
        pmu_read(PMU_REG_DCDC_ANA_CFG_19, &val);
        val |= DCDC2_TEST_MODE_EN;
        pmu_write(PMU_REG_DCDC_ANA_CFG_19, val);

        pmu_read(PMU_REG_DCDC_HPPA_CFG_1D, &val);
        val |= DCDC3_TEST_MODE_EN;
        pmu_write(PMU_REG_DCDC_HPPA_CFG_1D, val);

        pmu_read(PMU_REG_DCDC_DIG_CFG_36, &val);
        val |= DCDC1_TEST_MODE_EN;
        pmu_write(PMU_REG_DCDC_DIG_CFG_36, val);
    }

#ifdef PMU_BUCK_DIRECT_ULP_MODE
    if (pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_1809_buck_volt_hw_ramp_step_set(8);
    }
#endif

    if (!dcdc_ramp_en) {
        if (dcdc_ramp_map) {
            // Disable DCDC ramp so that s/w can control the voltages freely
            pmu_buck_volt_hw_ramp_disable(dcdc_ramp_map);
        }
    }

#if defined(PMU_GPIO_CLK_SOURCE_24M) && defined(PMU_GPIO_CLK_SOURCE_SWITCH)
    if (pmu_gpio_irq_enabled()) {
        pmu_gpio_irq_cfg_normal();
    }
#endif

    return;
}

void pmu_codec_config(int enable)
{
    enum PMU_MODULE_T codec;

#ifdef VHPPA2VCODEC
    codec = PMU_HPPA2CODEC;
#else
    codec = PMU_CODEC;
#endif

    if (vcodec_off) {
#ifdef VMEM_ON
        pmu_hppa_en(enable);
#endif
    } else {
        if (enable) {
            pmu_module_config(codec,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        } else {
            pmu_module_config(codec,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        }
    }
}

void pmu_codec_hppa_enable(int enable)
{
    if (!vcodec_off) {
#ifdef VMEM_ON
        pmu_hppa_en(enable);
#endif
    }
}

uint32_t pmu_codec_mic_bias_remap(uint32_t map)
{
    if (pmu_type == PMU_CHIP_TYPE_1803) {
        // Compatible with 1800 settings
        if (map & AUD_VMIC_MAP_VMIC2) {
            map = (map & ~AUD_VMIC_MAP_VMIC2) | AUD_VMIC_MAP_VMIC3;
        }
        if (map & AUD_VMIC_MAP_VMIC5) {
            map = (map & ~AUD_VMIC_MAP_VMIC5) | AUD_VMIC_MAP_VMIC2;
        }
    }

    return map;
}

void pmu_codec_mic_bias_noremap_enable(uint32_t map, int enable)
{
    uint16_t val;
    int i;
    enum PMU_REG_T bias_reg;
    uint16_t ldo_en;
    uint16_t ldo_mask;
    uint8_t vmic_num;

#ifdef PMU_FORCE_LP_MODE
    if (pmu_type == PMU_CHIP_TYPE_1803 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
        if (map & AUD_VMIC_MAP_VMIC1) {
            pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_VMIC1, enable);
        }
#ifndef NO_VMIC2_BG_CHECK
        if (map & AUD_VMIC_MAP_VMIC2) {
            pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_VMIC2, enable);
        }
#endif
    }
#endif

    if (pmu_type == PMU_CHIP_TYPE_1808 || pmu_type == PMU_CHIP_TYPE_1803) {
        vmic_num = MAX_VMIC_CH_NUM;
    } else {
        vmic_num = MAX_EXT_VMIC_CH_NUM_1806;
    }

    ldo_en = 0;
    ldo_mask = 0;
    for (i = 0; i < vmic_num; i++) {
        if ((map & (AUD_VMIC_MAP_VMIC1 << i)) == 0) {
            continue;
        }
        bias_reg = (enum PMU_REG_T)(PMU_REG_MIC_BIAS_A + i);
        pmu_read(bias_reg, &val);
        if (enable) {
            ldo_en |= (REG_MIC_LDOA_EN >> i);
            val |= REG_MIC_BIASA_EN;
        } else {
            ldo_mask |= (REG_MIC_LDOA_EN >> i);
            val &= ~REG_MIC_BIASA_EN;
        }
        pmu_write(bias_reg, val);
    }
    if (ldo_en || ldo_mask) {
        pmu_read(PMU_REG_MIC_LDO_EN, &val);
        val = (val & ~ldo_mask) | ldo_en;
        pmu_write(PMU_REG_MIC_LDO_EN, val);
    }
}

void pmu_codec_mic_bias_enable(uint32_t map, int enable)
{
    map = pmu_codec_mic_bias_remap(map);
    pmu_codec_mic_bias_noremap_enable(map, enable);
}

void pmu_codec_mic_bias_set_volt(uint32_t map, uint32_t mv)
{
    uint16_t val;
    int i;
    enum PMU_REG_T bias_reg;
    uint8_t volt;
    uint8_t res;
    uint8_t vmic_num;
    uint8_t step;

    map = pmu_codec_mic_bias_remap(map);

    if (pmu_type == PMU_CHIP_TYPE_1808 || pmu_type == PMU_CHIP_TYPE_1803) {
        vmic_num = MAX_VMIC_CH_NUM;
    } else {
        vmic_num = MAX_EXT_VMIC_CH_NUM_1806;
    }

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        step = 30;
    } else if (pmu_type == PMU_CHIP_TYPE_1805 && pmu_metal_id == HAL_CHIP_METAL_ID_1) {
        step = 120;
    } else {
        step = 100;
    }

    if (mv <= 1600) {
        volt = PMU_VMIC_1_6V;
    } else if (mv < 1900 + step / 2) {
        volt = (mv - 1600 + step / 2) / step + PMU_VMIC_1_6V;
    } else if (mv < 2500 + step / 2) {
        volt = (mv - 2000 + step / 2) / step + PMU_VMIC_2_0V;
    } else {
        volt = (mv - 2600 + step / 2) / step + PMU_VMIC_2_6V;
    }

    if (vcodec_mv < 1800) {
        volt += 1;
    }

    if (volt > PMU_VMIC_VBIT_MAX) {
        volt = PMU_VMIC_VBIT_MAX;
    }

    res = PMU_VMIC_RES_3_3V;

    for (i = 0; i < vmic_num; i++) {
        if ((map & (AUD_VMIC_MAP_VMIC1 << i)) == 0) {
            continue;
        }
        bias_reg = (enum PMU_REG_T)(PMU_REG_MIC_BIAS_A + i);
        pmu_read(bias_reg, &val);
        val = SET_BITFIELD(val, REG_MIC_LDOA_RES, res);
        if (pmu_type != PMU_CHIP_TYPE_1809) {
            val = SET_BITFIELD(val, REG_MIC_BIASA_VSEL, volt);
        }
        pmu_write(bias_reg, val);
        if (pmu_type == PMU_CHIP_TYPE_1809) {
            if (i == 0) {
                bias_reg = PMU1809_REG_MIC_BIASA_VCORE_CFG;
            } else {
                bias_reg = PMU1809_REG_MIC_BIASB_VANA_CFG;
            }
            pmu_read(bias_reg, &val);
            val = SET_BITFIELD(val, PMU1809_REG_MIC_BIASA_VSEL, volt);
            pmu_write(bias_reg, val);
        }
    }
}

void pmu_codec_mic_bias_noremap_lowpower_mode(uint32_t map, int enable)
{
}

void pmu_codec_mic_bias_lowpower_mode(uint32_t map, int enable)
{
    map = pmu_codec_mic_bias_remap(map);
    pmu_codec_mic_bias_noremap_lowpower_mode(map, enable);
}

void pmu_codec_adc_pre_start(void)
{
}

void pmu_codec_adc_post_start(void (*delay_ms)(uint32_t))
{
}

void pmu_codec_dac_pre_start(void)
{
}

void pmu_codec_dac_post_start(void (*delay_ms)(uint32_t))
{
}

SRAM_TEXT_LOC void pmu_flash_write_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if (pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req |= PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

SRAM_TEXT_LOC void pmu_flash_read_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if ((pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) == 0) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req &= ~PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_flash_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;

    lock = int_lock();
    if (freq > 52000000) {
        // The real max freq is 120M
        //pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_FLASH_FREQ_HIGH;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void pmu_anc_config(int enable)
{
}

void pmu_fir_high_speed_config(int enable)
{
}

void pmu_fir_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_FIR_FREQ_HIGH | PMU_VCORE_FIR_ENABLE);
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_FIR_FREQ_HIGH;
    } else if (freq > 0) {
        pmu_vcore_req |= PMU_VCORE_FIR_ENABLE;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_iir_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_IIR_FREQ_HIGH | PMU_VCORE_IIR_ENABLE);
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_FREQ_HIGH;
    } else if (freq > 0) {
        pmu_vcore_req |= PMU_VCORE_IIR_ENABLE;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_iir_eq_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_IIR_EQ_FREQ_HIGH | PMU_VCORE_IIR_EQ_ENABLE);
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_EQ_FREQ_HIGH;
    } else if (freq > 0) {
        pmu_vcore_req |= PMU_VCORE_IIR_EQ_ENABLE;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_rs_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_RS_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_RS_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_rs_adc_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_RS_ADC_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_RS_ADC_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void BOOT_TEXT_SRAM_LOC pmu_sys_freq_config(enum HAL_CMU_FREQ_T freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;
    enum PMU_VCORE_REQ_T old_req;
    bool update = false;

    lock = int_lock();
    old_req = pmu_vcore_req;
    pmu_vcore_req &= ~(PMU_VCORE_SYS_FREQ_HIGH | PMU_VCORE_SYS_FREQ_MEDIUM | PMU_VCORE_SYS_FREQ_MEDIUM_LOW);
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    if (freq > HAL_CMU_FREQ_104M) {
        if (high_perf_on) {
            // The real freq is 300M
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_HIGH;
        } else {
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
        }
    } else {
#ifdef OSC_26M_X4_AUD2BB
        if (freq >= HAL_CMU_FREQ_78M) {
            // The real freq is 104M
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM_LOW;
        }
#else
        if (freq == HAL_CMU_FREQ_104M) {
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM_LOW;
        }
#endif
    }
#else // !MCU_HIGH_PERFORMANCE_MODE
    if (0) {
#ifdef AUD_PLL_DOUBLE
    } else if (freq > HAL_CMU_FREQ_208M) {
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_HIGH;
#endif
    } else if (freq > HAL_CMU_FREQ_104M) {
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
#ifdef OSC_26M_X4_AUD2BB
    } else if (freq >= HAL_CMU_FREQ_78M) {
        // The real freq is 104M
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM_LOW;
#else
    } else if (freq == HAL_CMU_FREQ_104M) {
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM_LOW;
#endif
    }
#endif // !MCU_HIGH_PERFORMANCE_MODE
    if (old_req != pmu_vcore_req) {
        update = true;
    }
    int_unlock(lock);

    if (!update) {
        // Nothing changes
        return;
    }

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void pmu_axi_freq_config(enum HAL_CMU_AXI_FREQ_T freq)
{
    uint32_t lock;
    enum PMU_VCORE_REQ_T old_req;
    bool update = false;

    lock = int_lock();
    old_req = pmu_vcore_req;
    pmu_vcore_req &= ~(PMU_VCORE_AXI_FREQ_HIGH | PMU_VCORE_AXI_FREQ_MEDIUM | PMU_VCORE_AXI_FREQ_MEDIUM_LOW);
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    if (freq > HAL_CMU_AXI_FREQ_104M) {
        if (high_perf_on) {
            // The real freq is 300M
            pmu_vcore_req |= PMU_VCORE_AXI_FREQ_HIGH;
        } else {
            pmu_vcore_req |= PMU_VCORE_AXI_FREQ_MEDIUM;
        }
    } else {
#ifdef OSC_26M_X4_AUD2BB
        if (freq >= HAL_CMU_AXI_FREQ_78M) {
            // The real freq is 104M
            pmu_vcore_req |= PMU_VCORE_AXI_FREQ_MEDIUM_LOW;
        }
#else
        if (freq == HAL_CMU_AXI_FREQ_104M) {
            pmu_vcore_req |= PMU_VCORE_AXI_FREQ_MEDIUM_LOW;
        }
#endif
    }
#else // !MCU_HIGH_PERFORMANCE_MODE
    if (0) {
#ifdef AUD_PLL_DOUBLE
    } else if (freq > HAL_CMU_AXI_FREQ_208M) {
        pmu_vcore_req |= PMU_VCORE_AXI_FREQ_HIGH;
#endif
    } else if (freq > HAL_CMU_AXI_FREQ_104M) {
        pmu_vcore_req |= PMU_VCORE_AXI_FREQ_MEDIUM;
#ifdef OSC_26M_X4_AUD2BB
    } else if (freq >= HAL_CMU_AXI_FREQ_78M) {
        // The real freq is 104M
        pmu_vcore_req |= PMU_VCORE_AXI_FREQ_MEDIUM_LOW;
#else
    } else if (freq == HAL_CMU_AXI_FREQ_104M) {
        pmu_vcore_req |= PMU_VCORE_AXI_FREQ_MEDIUM_LOW;
#endif
    }
#endif // !MCU_HIGH_PERFORMANCE_MODE
    if (old_req != pmu_vcore_req) {
        update = true;
    }
    int_unlock(lock);

    if (!update) {
        // Nothing changes
        return;
    }

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_high_performance_mode_enable(bool enable)
{
#if defined(MCU_HIGH_PERFORMANCE_MODE)
#ifdef AUD_PLL_DOUBLE
#error "MCU_HIGH_PERFORMANCE_MODE conflicts with AUD_PLL_DOUBLE"
#endif
#ifdef BBPLL_FIXED_FREQ
#error "MCU_HIGH_PERFORMANCE_MODE conflicts with BBPLL_FIXED_FREQ"
#endif
    if (high_perf_on == enable) {
        return;
    }
    high_perf_on = enable;

    if (!enable) {
        if (high_perf_freq_mhz > 300) {
            // Switch to 52M to avoid using PLL
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
            // Restore the default div
            analog_aud_pll_set_dig_div(2);
            // Restore the sys freq
            hal_cmu_sys_set_freq(hal_sysfreq_get_hw_freq());
        }
        // Restore the default PLL freq
        analog_aud_freq_pll_config(CODEC_FREQ_24P576M, CODEC_PLL_DIV);
    }

    pmu_sys_freq_config(hal_sysfreq_get_hw_freq());

    if (enable) {
        uint32_t pll_freq;

        // Change freq first, and then change divider.
        // Otherwise there will be an instant very high freq sent to digital domain.

        if (high_perf_freq_mhz <= 300) {
            pll_freq = high_perf_freq_mhz * 1000000 * 2;
        } else {
            pll_freq = high_perf_freq_mhz * 1000000;
        }
        analog_aud_freq_pll_config(pll_freq / CODEC_PLL_DIV, CODEC_PLL_DIV);

        if (high_perf_freq_mhz > 300) {
            // Switch to 52M to avoid using PLL
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
            analog_aud_pll_set_dig_div(1);
            // Restore the sys freq
            hal_cmu_sys_set_freq(hal_sysfreq_get_hw_freq());
        }
    }
#endif
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{
    uint32_t lock;
    unsigned short ldo_on, deep_sleep_on;

    if (type == PMU_USB_CONFIG_TYPE_NONE) {
#ifdef VUSB_ON
        ldo_on = PMU_LDO_ON;
        deep_sleep_on = PMU_DSLEEP_MODE_ON;
#else
        ldo_on = PMU_LDO_OFF;
        deep_sleep_on = PMU_DSLEEP_MODE_OFF;
#endif
    } else {
        ldo_on = PMU_LDO_ON;
        deep_sleep_on = PMU_DSLEEP_MODE_ON;
    }

    lock = int_lock();

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        uint16_t val;

        // Power up SW1 during sleep
        pmu_read(PMU1806_REG_1P8_SWITCH, &val);
        if (type == PMU_USB_CONFIG_TYPE_NONE) {
            val &= ~(REG_EN_SW1_1P8 | REG_EN_SW1_1P8_DR);
        } else {
            val |= REG_EN_SW1_1P8 | REG_EN_SW1_1P8_DR;
        }
        pmu_write(PMU1806_REG_1P8_SWITCH, val);

        // Power up VUSB SW during sleep
        pmu_read(PMU1806_REG_VUSB_SWITCH, &val);
        if (type == PMU_USB_CONFIG_TYPE_NONE) {
            val = (val & ~REG_EN_SW_VUSB33) | REG_EN_SW_VUSB33_DR;
        } else {
            val |= REG_EN_SW_VUSB33 | REG_EN_SW_VUSB33_DR;
        }
        pmu_write(PMU1806_REG_VUSB_SWITCH, val);
    }

#ifdef USB_HIGH_SPEED
    if (type == PMU_USB_CONFIG_TYPE_NONE) {
        pmu_vcore_req &= ~PMU_VCORE_USB_HS_ENABLED;
    } else {
        pmu_vcore_req |= PMU_VCORE_USB_HS_ENABLED;
    }
#endif

    int_unlock(lock);

#ifndef SIMU
    if (ldo_on == PMU_LDO_ON) {
        // Wait for a long time between enabling 1.8V and 3.3V
        osDelay(600);
    }
#endif

    lock = int_lock();
    pmu_module_config(PMU_USB, PMU_MANUAL_MODE, ldo_on, PMU_LP_MODE_ON, deep_sleep_on);
    int_unlock(lock);

    if (pmu_power_mode != PMU_POWER_MODE_NONE) {
        // PMU has been inited
        pmu_dig_set_volt(PMU_POWER_MODE_NONE);
    }
}

int pmu_usb_config_pin_status_check(enum PMU_USB_PIN_CHK_STATUS_T status, PMU_USB_PIN_CHK_CALLBACK callback, int enable)
{
    return 0;
}

void pmu_usb_enable_pin_status_check(void)
{
}

void pmu_usb_disable_pin_status_check(void)
{
}

#if !defined(FPGA) && !defined(PROGRAMMER)
struct PMU_CHG_CTX_T {
    uint16_t pmu_chg_status;
};

struct PMU_CHG_CTX_T BOOT_BSS_LOC pmu_chg_ctx;

void BOOT_TEXT_SRAM_LOC pmu_charger_save_context(void)
{
    pmu_read(PMU_REG_CHARGER_STATUS, &pmu_chg_ctx.pmu_chg_status);
}

enum PMU_POWER_ON_CAUSE_T pmu_charger_poweron_status(void)
{
    enum PMU_POWER_ON_CAUSE_T pmu_power_on_cause = PMU_POWER_ON_CAUSE_NONE;

    if (pmu_chg_ctx.pmu_chg_status & AC_ON_DET_OUT){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_CHARGER_ACOFF;
    }else if (pmu_chg_ctx.pmu_chg_status & AC_ON){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_CHARGER_ACON;
    }
    return pmu_power_on_cause;
}

enum PMU_POWER_ON_CAUSE_T pmu_get_power_on_cause(void)
{
    enum PMU_POWER_ON_CAUSE_T pmu_power_on_cause = PMU_POWER_ON_CAUSE_NONE;

    pmu_power_on_cause = pmu_charger_poweron_status();
    if (pmu_power_on_cause == PMU_POWER_ON_CAUSE_NONE){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_POWER_KEY;
    }

    return pmu_power_on_cause;
}
#endif

void pmu_charger_init(void)
{
    unsigned short readval_cfg;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg &= ~(REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK |
        REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN | REG_CHARGE_INTR_EN);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);

    // PMU irqs cannot be cleared by PMU soft reset
    pmu_read(PMU_REG_CHARGER_STATUS, &readval_cfg);
    pmu_write(PMU_REG_CHARGER_STATUS, readval_cfg);
    pmu_read(PMU_REG_INT_STATUS, &readval_cfg);
    pmu_write(PMU_REG_INT_CLR, readval_cfg);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg |= REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN | REG_CHARGE_INTR_EN;
    readval_cfg = SET_BITFIELD(readval_cfg, REG_AC_ON_DB_VALUE, 8);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);
}

static void pmu_charger_irq_handler(uint16_t irq_status)
{
    enum PMU_CHARGER_STATUS_T status = PMU_CHARGER_UNKNOWN;

    PMU_DEBUG_TRACE(3,"%s REG_%02X=0x%04X", __func__, PMU_REG_CHARGER_STATUS, irq_status);

    if ((irq_status & (AC_ON_DET_IN_MASKED | AC_ON_DET_OUT_MASKED)) == 0){
        PMU_DEBUG_TRACE(1,"%s SKIP", __func__);
        return;
    } else if ((irq_status & (AC_ON_DET_IN_MASKED | AC_ON_DET_OUT_MASKED)) ==
            (AC_ON_DET_IN_MASKED | AC_ON_DET_OUT_MASKED)) {
        PMU_DEBUG_TRACE(1,"%s DITHERING", __func__);
        hal_sys_timer_delay(2);
    } else {
        PMU_DEBUG_TRACE(1,"%s NORMAL", __func__);
    }

    status = pmu_charger_get_status();

    if (charger_irq_handler) {
        charger_irq_handler(status);
    }
}

void pmu_charger_set_irq_handler(PMU_CHARGER_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    charger_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &val);
    if (handler) {
        val |= REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK;
    } else {
        val &= ~(REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK);
    }
    pmu_write(PMU_REG_CHARGER_CFG, val);
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_CHARGER, handler ? pmu_charger_irq_handler : NULL);
    int_unlock(lock);
}

void pmu_charger_plugin_config(void)
{
}

void pmu_charger_plugout_config(void)
{
}

enum PMU_CHARGER_STATUS_T pmu_charger_get_status(void)
{
    unsigned short readval;
    enum PMU_CHARGER_STATUS_T status;

    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    if (readval & AC_ON)
        status = PMU_CHARGER_PLUGIN;
    else
        status = PMU_CHARGER_PLUGOUT;

    return status;
}

#if defined(RTC_ENABLE) && defined(RTC_CALENDAR)
#error "RTC_ENABLE and RTC_CALENDAR can not defined together!"
#endif

#ifdef RTC_ENABLE
void pmu_rtc_enable(void)
{
    uint16_t readval;
    uint32_t lock;

#ifdef SIMU
    // Set RTC counter to 1KHz
    pmu_write(PMU_REG_RTC_DIV_1HZ, 32 - 2);
#else
    // Set RTC counter to 1Hz
    pmu_write(PMU_REG_RTC_DIV_1HZ, CONFIG_SYSTICK_HZ * 2 - 2);
#endif

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval |= RTC_POWER_ON_EN | PU_LPO_DR | PU_LPO_REG;
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    if (pmu_type == PMU_CHIP_TYPE_1808) {
        pmu_read(PMU1808_REG_RTC_CLK_CFG, &readval);
        readval |= REG_CLK_RTC_EN;
        pmu_write(PMU1808_REG_RTC_CLK_CFG, readval);
    }
    int_unlock(lock);
}

void pmu_rtc_disable(void)
{
    uint16_t readval;
    uint32_t lock;

    pmu_rtc_clear_alarm();

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval &= ~(RTC_POWER_ON_EN | PU_LPO_DR);
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_enabled(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);

    return !!(readval & RTC_POWER_ON_EN);
}

void pmu_rtc_set(uint32_t seconds)
{
    uint16_t high, low;

    if (pmu_type != PMU_CHIP_TYPE_1803 && pmu_type != PMU_CHIP_TYPE_1808) {
        return;
    }

    // Need 3 seconds to load a new value
    seconds += 3;

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_RTC_LOAD_LOW, low);
    pmu_write(PMU_REG_RTC_LOAD_HIGH, high);
}

uint32_t pmu_rtc_get(void)
{
    uint16_t high, low, high2;

    if (pmu_type != PMU_CHIP_TYPE_1803 && pmu_type != PMU_CHIP_TYPE_1808) {
        return 0;
    }

    pmu_read(PMU_REG_RTC_VAL_HIGH, &high);
    pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    // Handle counter wrap
    pmu_read(PMU_REG_RTC_VAL_HIGH, &high2);
    if (high != high2) {
        high = high2;
        pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    }

    return (high << 16) | low;
}

void pmu_rtc_set_alarm(uint32_t seconds)
{
    uint16_t readval;
    uint16_t high, low;
    uint32_t lock;

    if (pmu_type != PMU_CHIP_TYPE_1803 && pmu_type != PMU_CHIP_TYPE_1808) {
        return;
    }

    // Need 1 second to raise the interrupt
    if (seconds > 0) {
        seconds -= 1;
    }

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_INT_CLR, RTC_INT1_MSKED);

    pmu_write(PMU_REG_RTC_MATCH1_LOW, low);
    pmu_write(PMU_REG_RTC_MATCH1_HIGH, high);

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval |= RTC_INT_EN_1;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);
}

uint32_t BOOT_TEXT_SRAM_LOC pmu_rtc_get_alarm(void)
{
    uint16_t high, low;

    if (pmu_type != PMU_CHIP_TYPE_1803 && pmu_type != PMU_CHIP_TYPE_1808) {
        return 0;
    }

    pmu_read(PMU_REG_RTC_MATCH1_LOW, &low);
    pmu_read(PMU_REG_RTC_MATCH1_HIGH, &high);

    // Compensate the alarm offset
    return (uint32_t)((high << 16) | low) + 1;
}

void pmu_rtc_clear_alarm(void)
{
    uint16_t readval;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval &= ~RTC_INT_EN_1;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);

    pmu_write(PMU_REG_INT_CLR, RTC_INT1_MSKED);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_alarm_status_set(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_EN, &readval);

    return !!(readval & RTC_INT_EN_1);
}

int pmu_rtc_alarm_alerted()
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_STATUS, &readval);

    return !!(readval & RTC_INT_1);
}

static void pmu_rtc_irq_handler(uint16_t irq_status)
{
    uint32_t seconds;

    if (irq_status & RTC_INT1_MSKED) {
        pmu_rtc_clear_alarm();

        if (rtc_irq_handler) {
            seconds = pmu_rtc_get();
            rtc_irq_handler(seconds);
        }
    }
}

void pmu_rtc_set_irq_handler(PMU_RTC_IRQ_HANDLER_T handler)
{
    uint16_t readval;
    uint32_t lock;

    if (pmu_type != PMU_CHIP_TYPE_1803 && pmu_type != PMU_CHIP_TYPE_1808) {
        return;
    }

    rtc_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_INT_MASK, &readval);
    if (handler) {
        readval |= RTC_INT1_MSK;
    } else {
        readval &= ~RTC_INT1_MSK;
    }
    pmu_write(PMU_REG_INT_MASK, readval);
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_RTC, handler ? pmu_rtc_irq_handler : NULL);
    int_unlock(lock);
}
#endif

int pmu_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type)
{
    uint16_t val;
    uint16_t int_en;

    if (type == HAL_PWRKEY_IRQ_NONE) {
        // Clear existing IRQs
        pmu_write(PMU_REG_CHARGER_STATUS, (POWER_ON_PRESS | POWER_ON_RELEASE));
        // Disable future IRQs
        int_en = 0;
    } else if (type == HAL_PWRKEY_IRQ_FALLING_EDGE) {
        int_en = POWERON_PRESS_EN;
    } else if (type == HAL_PWRKEY_IRQ_RISING_EDGE) {
        int_en = POWERON_RELEASE_EN;
    } else if (type == HAL_PWRKEY_IRQ_BOTH_EDGE) {
        int_en = POWERON_PRESS_EN | POWERON_RELEASE_EN;
    } else {
        return 1;
    }

    pmu_read(PMU_REG_SLEEP_CFG, &val);
    val = (val & ~(POWERON_PRESS_EN | POWERON_RELEASE_EN)) | int_en;
    pmu_write(PMU_REG_SLEEP_CFG, val);

    return 0;
}
int hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type) __attribute__((alias("pmu_pwrkey_set_irq")));

bool pmu_pwrkey_pressed(void)
{
    uint16_t val;

    pmu_read(PMU_REG_CHARGER_STATUS, &val);

    return !!(val & POWER_ON);
}
bool hal_pwrkey_pressed(void) __attribute__((alias("pmu_pwrkey_pressed")));

enum HAL_PWRKEY_IRQ_T pmu_pwrkey_irq_value_to_state(uint16_t irq_status)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;

    if (irq_status & POWER_ON_PRESS) {
        state |= HAL_PWRKEY_IRQ_FALLING_EDGE;
    }

    if (irq_status & POWER_ON_RELEASE) {
        state |= HAL_PWRKEY_IRQ_RISING_EDGE;
    }

    return state;
}

static void pmu_general_irq_handler(void)
{
    uint32_t lock;
    uint16_t val;
    bool pwrkey, charger, gpadc, rtc, gpio, power_abort, capsensor;

    pwrkey = false;
    charger = false;
    gpadc = false;
    rtc = false;
    gpio = false;
    power_abort = false;
    capsensor = false;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    if (val & (POWER_ON_PRESS | POWER_ON_RELEASE)) {
        pwrkey = true;
    }
    if (val & (AC_ON_DET_OUT_MASKED | AC_ON_DET_IN_MASKED)) {
        charger = true;
    }
    if (pwrkey || charger) {
        pmu_write(PMU_REG_CHARGER_STATUS, val);
    }
    int_unlock(lock);

    if (pwrkey) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_PWRKEY]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_PWRKEY](val);
        }
    }
    if (charger) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER](val);
        }
    }

    lock = int_lock();
    pmu_read(PMU_REG_INT_MSKED_STATUS, &val);
    if (val & (KEY_ERR1_INTR_MSKED | KEY_ERR0_INTR_MSKED | KEY_PRESS_INTR_MSKED | KEY_RELEASE_INTR_MSKED |
            SAMPLE_DONE_INTR_MSKED | CHAN_DATA_INTR_MSKED_MASK)) {
        gpadc = true;
    }
    if (val & (RTC_INT1_MSKED | RTC_INT0_MSKED)) {
        rtc = true;
    }
    if (gpadc || rtc) {
        pmu_write(PMU_REG_INT_CLR, val);
    }
    int_unlock(lock);

    if (gpadc) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC](val);
        }
    }
    if (rtc) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC](val);
        }
    }

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_read(PMU_REG_CAP_MSK, &val);
        if (val & (1<<8)) {
            capsensor = true;
        }
        if (capsensor) {
            pmu_write(PMU_REG_CAP_MSK, val);
        }
        int_unlock(lock);

        if (capsensor) {
            if (pmu_irq_hdlrs[PMU_IRQ_TYPE_CAPSENSOR]) {
                pmu_irq_hdlrs[PMU_IRQ_TYPE_CAPSENSOR](val);
            }
        }
    }

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        lock = int_lock();
        if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
            pmu_read(PMU1806_REG_METAL_1, &val);
            if (val & (PMU_GPIO_INTR_MASKED | PMU_GPIO_NEDG_INTR_MASKED)) {
                gpio = true;
            }
        } else {
            gpio = true;
        }
        if (gpio) {
            if (pmu_irq_hdlrs[PMU_IRQ_TYPE_GPIO]) {
                pmu_irq_hdlrs[PMU_IRQ_TYPE_GPIO](val);
                pmu_gpio_irq_type = PMU_GPIO_IRQ_NONE;
            }
        }
        int_unlock(lock);
    }

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_read(PMU1809_REG_INTR_MSKED, &val);
        if (val & POWER_ABORT_INTR_PMU) {
            power_abort = true;
        }
        if (power_abort) {
            if (pmu_irq_hdlrs[PMU_IRQ_TYPE_POWER_ABORT]) {
                pmu_irq_hdlrs[PMU_IRQ_TYPE_POWER_ABORT](val);
            }
        }
        int_unlock(lock);
    }
}

int pmu_set_irq_unified_handler(enum PMU_IRQ_TYPE_T type, PMU_IRQ_UNIFIED_HANDLER_T hdlr)
{
    bool update;
    uint32_t lock;
    int i;

    if (type >= PMU_IRQ_TYPE_QTY) {
        return 1;
    }

    update = false;

    lock = int_lock();

    for (i = 0; i < PMU_IRQ_TYPE_QTY; i++) {
        if (pmu_irq_hdlrs[i]) {
            break;
        }
    }

    pmu_irq_hdlrs[type] = hdlr;

    if (hdlr) {
        update = (i >= PMU_IRQ_TYPE_QTY);
    } else {
        if (i == type) {
            for (; i < PMU_IRQ_TYPE_QTY; i++) {
                if (pmu_irq_hdlrs[i]) {
                    break;
                }
            }
            update = (i >= PMU_IRQ_TYPE_QTY);
        }
    }

    if (update) {
        if (hdlr) {
            NVIC_SetVector(PMU_IRQn, (uint32_t)pmu_general_irq_handler);
            NVIC_SetPriority(PMU_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_ClearPendingIRQ(PMU_IRQn);
            NVIC_EnableIRQ(PMU_IRQn);
        } else {
            NVIC_DisableIRQ(PMU_IRQn);
        }
    }

    int_unlock(lock);

    return 0;
}

void pmu_viorise_req(enum PMU_VIORISE_REQ_USER_T user, bool rise)
{
}

int pmu_debug_config_ana(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_codec(uint16_t volt)
{
#ifdef ANC_PROD_TEST
    if (volt == 1600 || volt == 1700 || volt == 1800 || volt == 1900 || volt == 1950) {
        vhppa_mv = vcodec_mv = volt;
        vcodec_off = true;
    } else {
        vcodec_off = false;
        return 1;
    }
#endif
    return 0;
}

int pmu_debug_config_vcrystal(bool on)
{
    return 0;
}

int pmu_debug_config_audio_output(bool diff)
{
    return 0;
}

void pmu_debug_reliability_test(int stage)
{
    uint16_t volt;

    if (stage == 0) {
        volt = PMU_DCDC_ANA_1_2V;
    } else {
        volt = PMU_DCDC_ANA_1_3V;
    }
    pmu_dcdc_ana_set_volt(volt, ana_lp_dcdc);
}

void pmu_led_set_hiz(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val1, val2;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_MIC_LDO_EN, &val1);
        pmu_read(PMU_REG_LED_CFG, &val2);
        if (pin == HAL_GPIO_PIN_LED1) {
            val1 &= ~REG_LED_IO1_RX_EN;
            val2 = (val2 | REG_LED_IO1_PU | REG_LED_IO1_OENB) & ~(REG_LED_IO1_PUEN | REG_LED_IO1_PDEN);
        } else {
            val1 &= ~REG_LED_IO2_RX_EN;
            val2 = (val2 | REG_LED_IO2_PU | REG_LED_IO2_OENB) & ~(REG_LED_IO2_PUEN | REG_LED_IO2_PDEN);
        }
        pmu_write(PMU_REG_MIC_LDO_EN, val1);
        pmu_write(PMU_REG_LED_CFG, val2);

        if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
            pmu_read(PMU1803_REG_PMIC_UART_CFG, &val2);
            if (pin == HAL_GPIO_PIN_LED1) {
                val2 |= REG_LED_IO1_OENB_PRE;
            } else {
                val2 |= REG_LED_IO2_OENB_PRE;
            }
            pmu_write(PMU1803_REG_PMIC_UART_CFG, val2);
        }

        int_unlock(lock);
    } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_GPIO_PIN_LED3)) {
        lock = int_lock();
        pmu_read(PMU_REG_GPIO3_CONFIG, &val1);
        val1 &= ~REG_GPIO3_RX_EN;
        val1 = (val1 | REG_GPIO3_PU | REG_GPIO3_OENB) & ~(REG_GPIO3_RPU | REG_GPIO3_RPD);
        pmu_write(PMU_REG_GPIO3_CONFIG, val1);
        int_unlock(lock);
    }
}

void pmu_led_set_direction(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_DIR_T dir)
{
    uint16_t val;
    uint16_t val_rx;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        pmu_read(PMU_REG_MIC_LDO_EN, &val_rx);
        if (pin == HAL_GPIO_PIN_LED1) {
            val |= REG_LED_IO1_PU;
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO1_OENB;
                val_rx |= REG_LED_IO1_RX_EN;
            } else {
                val &= ~REG_LED_IO1_OENB;
                val_rx &= ~REG_LED_IO1_RX_EN;
            }
        } else {
            val |= REG_LED_IO2_PU;
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO2_OENB;
                val_rx |= REG_LED_IO2_RX_EN;
            } else {
                val &= ~REG_LED_IO2_OENB;
                val_rx &= ~REG_LED_IO2_RX_EN;
            }
        }
        pmu_write(PMU_REG_LED_CFG, val);
        pmu_write(PMU_REG_MIC_LDO_EN, val_rx);;

        if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
            pmu_read(PMU1803_REG_PMIC_UART_CFG, &val);
            if (pin == HAL_GPIO_PIN_LED1) {
                val |= REG_UART_LED1_SEL;
                if (dir == HAL_GPIO_DIR_IN) {
                    val |= REG_LED_IO1_OENB_PRE;
                } else {
                    val &= ~REG_LED_IO1_OENB_PRE;
                }
            } else {
                val |= REG_UART_LED2_SEL;
                if (dir == HAL_GPIO_DIR_IN) {
                    val |= REG_LED_IO2_OENB_PRE;
                } else {
                    val &= ~REG_LED_IO2_OENB_PRE;
                }
            }
            pmu_write(PMU1803_REG_PMIC_UART_CFG, val);
        }

        int_unlock(lock);
    } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_GPIO_PIN_LED3)) {
        lock = int_lock();
        pmu_read(PMU_REG_GPIO3_CONFIG, &val);
        val |= REG_GPIO3_PU;
        if (dir == HAL_GPIO_DIR_IN) {
            val |= REG_GPIO3_OENB | REG_GPIO3_RX_EN;
        } else {
            val &= ~(REG_GPIO3_OENB | REG_GPIO3_RX_EN);
        }
        pmu_write(PMU_REG_GPIO3_CONFIG, val);
        int_unlock(lock);
    }
}

enum HAL_GPIO_DIR_T pmu_led_get_direction(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
            pmu_read(PMU1803_REG_PMIC_UART_CFG, &val);
            if (pin == HAL_GPIO_PIN_LED1) {
                return (val & REG_LED_IO1_OENB_PRE) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
            } else {
                return (val & REG_LED_IO2_OENB_PRE) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
            }
        } else {
            pmu_read(PMU_REG_LED_CFG, &val);
            if (pin == HAL_GPIO_PIN_LED1) {
                return (val & REG_LED_IO1_OENB) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
            } else {
                return (val & REG_LED_IO2_OENB) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
            }
        }
    } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_GPIO_PIN_LED3)) {
        pmu_read(PMU_REG_GPIO3_CONFIG, &val);
        return (val & REG_GPIO3_OENB) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
    } else {
        return HAL_GPIO_DIR_IN;
    }
}

void pmu_led_set_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    enum PMU_LED_VOLT_T {
        PMU_LED_VOLT_VBAT,
        PMU_LED_VOLT_VMEM,
        PMU_LED_VOLT_VIO,
    };
    enum PMU_LED_VOLT_T sel;
    uint16_t val;
    uint32_t lock;

    if (volt == HAL_IOMUX_PIN_VOLTAGE_VIO) {
        sel = PMU_LED_VOLT_VIO;
    } else if (volt == HAL_IOMUX_PIN_VOLTAGE_MEM) {
        sel = PMU_LED_VOLT_VMEM;
    } else {
        sel = PMU_LED_VOLT_VBAT;
    }

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_IOMUX_PIN_LED1) {
            val = SET_BITFIELD(val, REG_LED_IO1_SEL, sel);
        } else {
            val = SET_BITFIELD(val, REG_LED_IO2_SEL, sel);
        }
        pmu_write(PMU_REG_LED_CFG, val);
        int_unlock(lock);
    } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_IOMUX_PIN_LED3)) {
        lock = int_lock();
        pmu_read(PMU_REG_GPIO3_CONFIG, &val);
        val = SET_BITFIELD(val, REG_GPIO3_VSEL, sel);
        pmu_write(PMU_REG_GPIO3_CONFIG, val);
        int_unlock(lock);
    }
}

void pmu_led_set_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_IOMUX_PIN_LED1) {
            val &= ~(REG_LED_IO1_PDEN | REG_LED_IO1_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= REG_LED_IO1_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= REG_LED_IO1_PDEN;
            }
        } else {
            val &= ~(REG_LED_IO2_PDEN | REG_LED_IO2_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= REG_LED_IO2_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= REG_LED_IO2_PDEN;
            }
        }
        pmu_write(PMU_REG_LED_CFG, val);
        int_unlock(lock);
    } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_IOMUX_PIN_LED3)) {
        lock = int_lock();
        pmu_read(PMU_REG_GPIO3_CONFIG, &val);
        val &= ~(REG_GPIO3_RPD | REG_GPIO3_RPU);
        if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
            val |= REG_GPIO3_RPU;
        } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
            val |= REG_GPIO3_RPD;
        }
        int_unlock(lock);
    }
}

void pmu_led_set_value(enum HAL_GPIO_PIN_T pin, int data)
{
    uint16_t val;
    uint32_t lock;
#ifdef PMU_LED_VIA_PWM
    uint16_t br_val;
#endif

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
#ifdef PMU_LED_VIA_PWM
        pmu_read(PMU_REG_PWM_BR_EN, &br_val);
#endif
        pmu_read(PMU_REG_PWM_EN, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
#ifdef PMU_LED_VIA_PWM
            pmu_write(PMU_REG_PWM2_TOGGLE, 0xFFFF);
            br_val &= ~REG_PWM2_BR_EN;
            val |= PWM_SELECT_EN(1 << 0);
            if (data) {
                val &= ~PWM_SELECT_INV(1 << 0);
            } else {
                val |= PWM_SELECT_INV(1 << 0);
            }
#else
            if (data) {
                val |= REG_LED0_OUT;
            } else {
                val &= ~REG_LED0_OUT;
            }
#endif
        } else {
#ifdef PMU_LED_VIA_PWM
            pmu_write(PMU_REG_PWM3_TOGGLE, 0xFFFF);
            br_val &= ~REG_PWM3_BR_EN;
            val |= PWM_SELECT_EN(1 << 1);
            if (data) {
                val &= ~PWM_SELECT_INV(1 << 1);
            } else {
                val |= PWM_SELECT_INV(1 << 1);
            }
#else
            if (data) {
                val |= REG_LED1_OUT;
            } else {
                val &= ~REG_LED1_OUT;
            }
#endif
        }
#ifdef PMU_LED_VIA_PWM
        pmu_write(PMU_REG_PWM_BR_EN, br_val);
#endif
        pmu_write(PMU_REG_PWM_EN, val);
        int_unlock(lock);
    } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_GPIO_PIN_LED3)) {
        lock = int_lock();
        pmu_read(PMU_REG_PWM4_EN, &val);
#ifdef PMU_LED_VIA_PWM
        val |= REG_PWM4_SELECT_EN;
        if (data) {
            val &= ~REG_PWM4_SELECT_INV;
        } else {
            val |= REG_PWM4_SELECT_INV;
        }
#else
        if (data) {
            val |= REG_REG_LED2_OUT;
        } else {
            val &= ~REG_REG_LED2_OUT;
        }
#endif
        pmu_write(PMU_REG_PWM4_EN, val);
        int_unlock(lock);
    }
}

int pmu_led_get_value(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;
    int data = 0;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        pmu_read(PMU_REG_POWER_OFF, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            data = REG_LED_IO1_DATA_IN;
        } else {
            data = REG_LED_IO2_DATA_IN;
        }
        data &= val;
    } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_GPIO_PIN_LED3)) {
        pmu_read(PMU_REG_POWER_OFF, &val);
        data = RD_EFUSE_REG;        //1805: gpio3_data_in
        data &= val;
    }

    return !!data;
}

void pmu_led_breathing_enable(enum HAL_IOMUX_PIN_T pin, const struct PMU_LED_BR_CFG_T *cfg)
{
    uint32_t st1;
    uint32_t st2;
    uint32_t subcnt_data;
    uint8_t tg;
    uint16_t val;
    uint32_t lock;

    if ((HAL_IOMUX_PIN_LED1 <= pin) && (pin < HAL_IOMUX_PIN_LED_NUM)) {
        st1 = MS_TO_TICKS(cfg->off_time_ms);
        if (st1 > 0xFFFF) {
            st1 = 0xFFFF;
        }
        st2 = MS_TO_TICKS(cfg->on_time_ms);
        if (st2 > 0xFFFF) {
            st2 = 0xFFFF;
        }
        subcnt_data = MS_TO_TICKS(cfg->fade_time_ms);
        subcnt_data = integer_sqrt_nearest(subcnt_data);
        if (subcnt_data > (SUBCNT_DATA2_MASK >> SUBCNT_DATA2_SHIFT)) {
            subcnt_data = (SUBCNT_DATA2_MASK >> SUBCNT_DATA2_SHIFT);
        }
        // TODO: HW bug
        if (subcnt_data > 0xFE) {
            subcnt_data = 0xFE;
        }
        tg = 1;

        pmu_led_set_direction((enum HAL_GPIO_PIN_T)pin, HAL_GPIO_DIR_OUT);
        pmu_led_set_pull_select(pin, HAL_IOMUX_PIN_NOPULL);
        pmu_led_set_voltage_domains(pin, HAL_IOMUX_PIN_VOLTAGE_VBAT);

        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_write(PMU_REG_PWM2_TOGGLE, st2);
            pmu_write(PMU_REG_PWM2_ST1, st1);

            pmu_read(PMU_REG_SUBCNT_DATA, &val);
            val = SET_BITFIELD(val, SUBCNT_DATA2, subcnt_data);
            pmu_write(PMU_REG_SUBCNT_DATA, val);

            pmu_read(PMU_REG_PWM_BR_EN, &val);
            val = (val & ~TG_SUBCNT_D2_ST_MASK) | TG_SUBCNT_D2_ST(tg) | REG_PWM2_BR_EN;
            pmu_write(PMU_REG_PWM_BR_EN, val);

            pmu_read(PMU_REG_PWM_EN, &val);
            val = (val & ~REG_CLK_PWM_DIV_MASK) | REG_CLK_PWM_DIV(0) | PWM_SELECT_EN(1 << 0);
            if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
                val |= PMU1803_REG_PWM_CLK_EN_LED1;
            }
            pmu_write(PMU_REG_PWM_EN, val);
            if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
                pmu_read(PMU_REG_PWM_CLK_CFG, &val);
                val |= PMU_REG_PWM_CLK_EN_LED1;
                pmu_write(PMU_REG_PWM_CLK_CFG, val);
            }
        } else if (pin == HAL_IOMUX_PIN_LED2) {
            pmu_write(PMU_REG_PWM3_TOGGLE, st2);
            pmu_write(PMU_REG_PWM3_ST1, st1);

            pmu_read(PMU_REG_SUBCNT_DATA, &val);
            val = SET_BITFIELD(val, SUBCNT_DATA3, subcnt_data);
            pmu_write(PMU_REG_SUBCNT_DATA, val);

            pmu_read(PMU_REG_PWM_BR_EN, &val);
            val = (val & ~TG_SUBCNT_D3_ST_MASK) | TG_SUBCNT_D3_ST(tg) | REG_PWM3_BR_EN;
            pmu_write(PMU_REG_PWM_BR_EN, val);

            pmu_read(PMU_REG_PWM_EN, &val);
            val = (val & ~REG_CLK_PWM_DIV_MASK) | REG_CLK_PWM_DIV(0) | PWM_SELECT_EN(1 << 1);
            if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
                val |= PMU1803_REG_PWM_CLK_EN_LED2;
            }
            pmu_write(PMU_REG_PWM_EN, val);
            if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
                pmu_read(PMU_REG_PWM_CLK_CFG, &val);
                val |= PMU_REG_PWM_CLK_EN_LED2;
                pmu_write(PMU_REG_PWM_CLK_CFG, val);
            }
        } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_IOMUX_PIN_LED3)) {
            pmu_write(PMU1805_REG_PWM4_TOGGLE, st2);
            pmu_write(PMU1805_REG_PWM4_ST1, st1);

            pmu_read(PMU1805_REG_SUBCNT_DATA, &val);
            val = SET_BITFIELD(val, SUBCNT_DATA4, subcnt_data);
            pmu_write(PMU1805_REG_SUBCNT_DATA, val);

            pmu_read(PMU1805_REG_SUBCNT_DATA, &val);
            val = (val & ~TG_SUBCNT_D4_ST_MASK) | TG_SUBCNT_D4_ST(tg) | REG_PWM4_BR_EN;
            pmu_write(PMU1805_REG_SUBCNT_DATA, val);

            pmu_read(PMU_REG_PWM_EN, &val);
            val = (val & ~REG_CLK_PWM_DIV_MASK) | REG_CLK_PWM_DIV(0);
            pmu_write(PMU_REG_PWM_EN, val);

            pmu_read(PMU_REG_PWM4_EN, &val);
            val |= (REG_PWM4_CLK_EN | REG_PWM4_SELECT_EN);
            pmu_write(PMU_REG_PWM4_EN, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_breathing_disable(enum HAL_IOMUX_PIN_T pin)
{
    uint16_t val;
    uint32_t lock;

    if ((HAL_IOMUX_PIN_LED1 <= pin) && (pin < HAL_IOMUX_PIN_LED_NUM)) {
        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_read(PMU_REG_PWM_BR_EN, &val);
            val &= ~REG_PWM2_BR_EN;
            pmu_write(PMU_REG_PWM_BR_EN, val);

            pmu_read(PMU_REG_PWM_EN, &val);
            val &= ~PWM_SELECT_EN(1 << 0);
            if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
                val &= ~PMU1803_REG_PWM_CLK_EN_LED1;
            }
            pmu_write(PMU_REG_PWM_EN, val);
            if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
                pmu_read(PMU_REG_PWM_CLK_CFG, &val);
                val &= ~PMU_REG_PWM_CLK_EN_LED1;
                pmu_write(PMU_REG_PWM_CLK_CFG, val);
            }
        } else if (pin == HAL_IOMUX_PIN_LED2) {
            pmu_read(PMU_REG_PWM_BR_EN, &val);
            val &= ~REG_PWM3_BR_EN;
            pmu_write(PMU_REG_PWM_BR_EN, val);

            pmu_read(PMU_REG_PWM_EN, &val);
            val &= ~PWM_SELECT_EN(1 << 1);
            if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
                val &= ~PMU1803_REG_PWM_CLK_EN_LED2;
            }
            pmu_write(PMU_REG_PWM_EN, val);
            if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
                pmu_read(PMU_REG_PWM_CLK_CFG, &val);
                val &= ~PMU_REG_PWM_CLK_EN_LED2;
                pmu_write(PMU_REG_PWM_CLK_CFG, val);
            }
        } else if ((pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) && (pin == HAL_IOMUX_PIN_LED3)) {
            pmu_read(PMU1805_REG_SUBCNT_DATA, &val);
            val &= ~REG_PWM4_BR_EN;
            pmu_write(PMU1805_REG_SUBCNT_DATA, val);

            pmu_read(PMU_REG_PWM4_EN, &val);
            val &= ~(REG_PWM4_CLK_EN | REG_PWM4_SELECT_EN);
            pmu_write(PMU_REG_PWM4_EN, val);
        }
        int_unlock(lock);
        pmu_led_set_direction((enum HAL_GPIO_PIN_T)pin, HAL_GPIO_DIR_IN);
        pmu_led_set_pull_select(pin, HAL_IOMUX_PIN_PULLUP_ENABLE);
    }
}

__STATIC_FORCEINLINE void pmu_wdt_load_value(void)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_read(PMU_REG_GP_CFG, &val);
    pmu_write(PMU_REG_GP_CFG, val);
    int_unlock(lock);
}

#ifdef __WATCHER_DOG_RESET__
struct PMU_WDT_CTX_T {
    bool enabled;
    uint16_t wdt_timer;
    uint16_t wdt_cfg;
};

static struct PMU_WDT_CTX_T BOOT_BSS_LOC wdt_ctx;

void BOOT_TEXT_SRAM_LOC pmu_wdt_save_context(void)
{
    uint16_t wdt_cfg = 0, timer = 0;
    pmu_read(PMU_REG_WDT_CFG, &wdt_cfg);
    if (wdt_cfg & (REG_WDT_RESET_EN | REG_WDT_EN)){
        wdt_ctx.enabled = true;
        wdt_ctx.wdt_cfg = wdt_cfg;
        pmu_read(PMU_REG_WDT_TIMER, &timer);
        wdt_ctx.wdt_timer = timer;
    }
}

void BOOT_TEXT_SRAM_LOC pmu_wdt_restore_context(void)
{
    if (wdt_ctx.enabled) {
        pmu_write(PMU_REG_WDT_TIMER, wdt_ctx.wdt_timer);
        pmu_write(PMU_REG_WDT_CFG, wdt_ctx.wdt_cfg);
        pmu_wdt_load_value();
    }
}
#endif

void pmu_wdt_set_irq_handler(PMU_WDT_IRQ_HANDLER_T handler)
{
}

int pmu_wdt_config(uint32_t irq_ms, uint32_t reset_ms)
{
    if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id == HAL_CHIP_METAL_ID_1) {
        return 2;
    }

    // No wdt irq on best2300
    if (irq_ms + reset_ms > 0xFFFF) {
        return 1;
    }
    wdt_timer = irq_ms + reset_ms;

    pmu_write(PMU_REG_WDT_TIMER, wdt_timer);
    pmu_wdt_load_value();

    return 0;
}

void pmu_wdt_start(void)
{
    uint16_t val;

    if (wdt_timer == 0) {
        return;
    }

    pmu_read(PMU_REG_WDT_CFG, &val);
    val |= (REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);
}

#ifndef __WATCHER_DOG_RESET__
BOOT_TEXT_SRAM_LOC
#endif
void pmu_wdt_stop(void)
{
    uint16_t val;

    pmu_read(PMU_REG_WDT_CFG, &val);
    val &= ~(REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);
}

void pmu_wdt_feed(void)
{
    if (wdt_timer == 0) {
        return;
    }

    pmu_write(PMU_REG_WDT_TIMER, wdt_timer);
    pmu_wdt_load_value();
}

void pmu_ntc_capture_enable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_THERM, &val);
        val = SET_BITFIELD(val, SAR_PWR_BIT, 0);
        val = SET_BITFIELD(val, SAR_OP_IBIT, 3);
        val = SET_BITFIELD(val, SAR_THERM_GAIN, 2);
        pmu_write(PMU_REG_THERM, val);
    } else {
        pmu_read(PMU_REG_THERM, &val);
        val = SET_BITFIELD(val, SAR_THERM_GAIN, 2);
        pmu_write(PMU_REG_THERM, val);
    }

    pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
    val |= IPTAT_EN;
    pmu_write(PMU_REG_DCDC_RAMP_EN, val);
}

void pmu_ntc_capture_disable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_THERM, &val);
        val = SET_BITFIELD(val, SAR_PWR_BIT, 2);
        val = SET_BITFIELD(val, SAR_OP_IBIT, 0);
        val = SET_BITFIELD(val, SAR_THERM_GAIN, 2);
        pmu_write(PMU_REG_THERM, val);
    } else {
        pmu_read(PMU_REG_THERM, &val);
        val = SET_BITFIELD(val, SAR_THERM_GAIN, 0);
        pmu_write(PMU_REG_THERM, val);
    }

    pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
    val &= ~IPTAT_EN;
    pmu_write(PMU_REG_DCDC_RAMP_EN, val);
}

static void pmu_ntc_irq_handler(uint16_t raw, uint16_t volt)
{
    pmu_ntc_capture_disable();

    ntc_temperature = pmu_volt2temperature(volt);
    ntc_raw = raw;

    if (pmu_ntc_cb) {
        pmu_ntc_cb(ntc_raw, ntc_temperature);
    }

    ntc_irq_busy = false;
}

int pmu_ntc_capture_start(PMU_NTC_IRQ_HANDLER_T cb)
{
    int nRet = 0;
    uint32_t lock = 0;

    lock = int_lock();
    if (ntc_irq_busy) {
        nRet = -1;
        goto exit;
    }
    pmu_ntc_cb = cb;
    ntc_irq_busy = true;
    pmu_ntc_capture_enable();
    hal_gpadc_open(HAL_GPADC_CHAN_0, HAL_GPADC_ATP_ONESHOT, pmu_ntc_irq_handler);
exit:
    int_unlock(lock);

    return nRet;
}

uint16_t pmu_ntc_temperature_reference_get(void)
{
    static uint16_t efuse_temperature_raw = 0;

    if (!efuse_temperature_raw) {
        uint16_t efuse_val;

        pmu_get_efuse(PMU_EFUSE_PAGE_TEMPERATURE, &efuse_val);
        //Bit[11:0] records the ambient temperature when the chip is turned on
        if (efuse_val) {
            efuse_temperature_raw = efuse_val & 0xFFF;
        } else {
            efuse_temperature_raw = NTC_TEMPERATURE_RAW_DEFAULT;
        }
    }

    return efuse_temperature_raw;
}

uint16_t pmu_ntc_raw_get(void)
{
    return ntc_raw;
}

uint16_t pmu_ntc_temperature_get(void)
{
    return ntc_temperature;
}

#endif // !NO_EXT_PMU

void pmu_uart_enable(void)
{
    uint16_t val = 0;

    if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
        pmu_read(PMU1803_REG_PMIC_UART_CFG, &val);
        val &= ~(REG_GPIO_I_SEL | REG_UART_LED2_SEL | REG_PMIC_UART_DR2);
        val |= (REG_PMIC_UART_DR1| REG_PMIC_UART_OENB_SEL);
        pmu_write(PMU1803_REG_PMIC_UART_CFG, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) {
        pmu_read(PMU_REG_RESERVED_PMU, &val);
        val &= ~(1 << 12);
        pmu_write(PMU_REG_RESERVED_PMU, val);
    } else {
        ASSERT(0, "%s: Bad pmu_type=%d", __func__, pmu_type);
    }
}

int pmu_get_gpadc_data_bits(void)
{
    return (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) ? 12 : 10;
}

void pmu_codec_vcm_enable(bool en)
{
    uint16_t val;

#ifdef SPLIT_VMEM_VMEM2
    return;
#endif

    if (!(pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805)) {
        return;
    }

    if (pmu_type == PMU_CHIP_TYPE_1805 && pmu_metal_id == HAL_CHIP_METAL_ID_0) {
        // pmu_vcm enabled in pmu_open and need always on.
        return;
    }

    pmu_read(PMU1806_REG_CODEC_VCM, &val);
    if (en) {
        val |= REG_VCM_EN;
    } else {
        val &= ~REG_VCM_EN;
    }
    pmu_write(PMU1806_REG_CODEC_VCM, val);
}

void pmu_codec_vcm_enable_lpf(bool en)
{
    uint16_t val;

    if (!(pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805)) {
        return;
    }

    pmu_read(PMU1806_REG_CODEC_VCM, &val);
    if (en) {
        val |= REG_VCM_EN_LPF;
    } else {
        val &= ~REG_VCM_EN_LPF;
    }
    pmu_write(PMU1806_REG_CODEC_VCM, val);
}

void pmu_codec_lp_vcm(bool en)
{
    uint16_t val;

    if (!(pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805)) {
        return;
    }

    pmu_read(PMU1806_REG_CODEC_VCM, &val);
    if (en) {
        val |= REG_VCM_LP_EN;
    } else {
        val &= ~REG_VCM_LP_EN;
    }
    pmu_write(PMU1806_REG_CODEC_VCM, val);
}

void pmu_codec_set_vcm_low_vcm_lp(uint16_t v)
{
    uint16_t val;

    if (!(pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805)) {
        return;
    }

    pmu_read(PMU1806_REG_CODEC_VCM, &val);
    val = SET_BITFIELD(val, REG_VCM_LOW_VCM_LP, v);
    pmu_write(PMU1806_REG_CODEC_VCM, val);
}

void pmu_codec_set_vcm_low_vcm_lpf(uint16_t v)
{
    uint16_t val;

    if (!(pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805)) {
        return;
    }

    pmu_read(PMU1806_REG_CODEC_VCM, &val);
    val = SET_BITFIELD(val, REG_VCM_LOW_VCM_LPF, v);
    pmu_write(PMU1806_REG_CODEC_VCM, val);
}

void pmu_codec_set_vcm_low_vcm(uint16_t v)
{
    uint16_t val;

    if (!(pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805)) {
        return;
    }

    pmu_read(PMU1806_REG_CODEC_VCM, &val);
    val = SET_BITFIELD(val, REG_VCM_LOW_VCM, v);
    pmu_write(PMU1806_REG_CODEC_VCM, val);
}

static bool pmu_gpio_clk_select_slow(bool en)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
        pmu_read(PMU1806_REG_RESERVED_METAL_1, &val);
        if (en) {
            val |= REG_GPIO_32K_SEL;
        } else {
            val &= ~REG_GPIO_32K_SEL;
        }
        pmu_write(PMU1806_REG_RESERVED_METAL_1, val);
        return true;
    } else {
        return false;
    }
}

#if defined(PMU_GPIO_CLK_SOURCE_24M) && defined(PMU_GPIO_CLK_SOURCE_SWITCH)
static int pmu_gpio_irq_enabled(void)
{
    int i;

    if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
        for (i = 0; i < PMU_GPIO_PIN_GROUP_NUM; i++) {
            if ((pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[i][0] != 0) || \
                (pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[i][1] != 0)) {
                return 1;
            }
        }
    }
    return 0;
}

static void pmu_gpio_irq_cfg_dsleep(void)
{
    uint16_t val;
    uint16_t reg;
    uint16_t shift_group;
    bool ret = false;

    if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
        ret = pmu_gpio_clk_select_slow(true);
        if (ret) {
            for (shift_group = 0; shift_group < PMU_GPIO_PIN_GROUP_NUM; shift_group++) {
                reg = PMU1806_REG_GPIO_P0_P1_DB_TARGET + shift_group;
                pmu_read(reg, &val);
                val = SET_BITFIELD(val, REG_PMU_GPIO_L_DB_TARGET, PMU_GPIO_DEBOUNCE_TARGET_32K);
                val = SET_BITFIELD(val, REG_PMU_GPIO_H_DB_TARGET, PMU_GPIO_DEBOUNCE_TARGET_32K);
                pmu_write(reg, val);
            }
        }
    }
}

static void pmu_gpio_irq_cfg_normal(void)
{
    uint16_t val;
    uint16_t reg;
    uint16_t shift_group;
    bool ret = false;

    if (pmu_type == PMU_CHIP_TYPE_1806 && pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
        ret = pmu_gpio_clk_select_slow(false);
        if (ret) {
            for (shift_group = 0; shift_group < PMU_GPIO_PIN_GROUP_NUM; shift_group++) {
                reg = PMU1806_REG_GPIO_P0_P1_DB_TARGET + shift_group;
                pmu_read(reg, &val);
                val = SET_BITFIELD(val, REG_PMU_GPIO_L_DB_TARGET, PMU_GPIO_DEBOUNCE_TARGET_24M);
                val = SET_BITFIELD(val, REG_PMU_GPIO_H_DB_TARGET, PMU_GPIO_DEBOUNCE_TARGET_24M);
                pmu_write(reg, val);
            }
        }
    }
}
#endif

enum HAL_GPIO_DIR_T pmu_gpio_pin_get_dir(enum HAL_GPIO_PIN_T pin)
{
    uint16_t reg_rx_en;
    uint16_t reg_rx_en_val;
    uint16_t rx_en_shift_pin;
    uint16_t reg_oen_enb;
    uint16_t reg_oen_enb_val;
    uint16_t data_out_shift_pin;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return HAL_GPIO_DIR_IN;
    }

    if (pin < HAL_GPIO_PIN_PMU_P1_0) {
        reg_rx_en = PMU1806_REG_GPIO_P0_P1_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);

        reg_oen_enb = PMU1806_REG_GPIO_P0_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        reg_rx_en = PMU1806_REG_GPIO_P0_P1_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);

        reg_oen_enb = PMU1806_REG_GPIO_P1_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P1_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P3_0) {
        reg_rx_en = PMU1806_REG_GPIO_P2_P3_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);

        reg_oen_enb = PMU1806_REG_GPIO_P2_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        reg_rx_en = PMU1806_REG_GPIO_P2_P3_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);

        reg_oen_enb = PMU1806_REG_GPIO_P3_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P3_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P5_0) {
        reg_rx_en = PMU1806_REG_GPIO_P4_P5_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);

        reg_oen_enb = PMU1806_REG_GPIO_P4_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        reg_rx_en = PMU1806_REG_GPIO_P4_P5_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);

        reg_oen_enb = PMU1806_REG_GPIO_P5_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P5_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P7_0) {
        reg_rx_en = PMU1806_REG_GPIO_P6_P7_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);

        reg_oen_enb = PMU1806_REG_GPIO_P6_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);
    } else {
        reg_rx_en = PMU1806_REG_GPIO_P6_P7_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);

        reg_oen_enb = PMU1806_REG_GPIO_P7_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P7_0);
    }

    // rx
    pmu_read(reg_rx_en, &reg_rx_en_val);

    // oen
    pmu_read(reg_oen_enb, &reg_oen_enb_val);

    if ((reg_rx_en_val & rx_en_shift_pin) && (reg_oen_enb_val & (data_out_shift_pin << 8))){
        return HAL_GPIO_DIR_IN;
    } else {
        return HAL_GPIO_DIR_OUT;
    }
}

void pmu_gpio_pin_set_dir(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_DIR_T dir, uint8_t val_for_out)
{
    uint16_t reg_rx_en;
    uint16_t rx_en_shift_pin;
    uint16_t reg_oen_enb;
    uint16_t data_out_shift_pin;
    uint16_t val;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return;
    }

    if (pin < HAL_GPIO_PIN_PMU_P1_0) {
        reg_rx_en = PMU1806_REG_GPIO_P0_P1_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);

        reg_oen_enb = PMU1806_REG_GPIO_P0_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        reg_rx_en = PMU1806_REG_GPIO_P0_P1_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);

        reg_oen_enb = PMU1806_REG_GPIO_P1_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P1_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P3_0) {
        reg_rx_en = PMU1806_REG_GPIO_P2_P3_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);

        reg_oen_enb = PMU1806_REG_GPIO_P2_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        reg_rx_en = PMU1806_REG_GPIO_P2_P3_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);

        reg_oen_enb = PMU1806_REG_GPIO_P3_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P3_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P5_0) {
        reg_rx_en = PMU1806_REG_GPIO_P4_P5_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);

        reg_oen_enb = PMU1806_REG_GPIO_P4_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        reg_rx_en = PMU1806_REG_GPIO_P4_P5_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);

        reg_oen_enb = PMU1806_REG_GPIO_P5_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P5_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P7_0) {
        reg_rx_en = PMU1806_REG_GPIO_P6_P7_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);

        reg_oen_enb = PMU1806_REG_GPIO_P6_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);
    } else {
        reg_rx_en = PMU1806_REG_GPIO_P6_P7_RX_EN;
        rx_en_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);

        reg_oen_enb = PMU1806_REG_GPIO_P7_DATA_OUT;
        data_out_shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P7_0);
    }

    if (dir == HAL_GPIO_DIR_OUT){
        // rx en = 0
        pmu_read(reg_rx_en, &val);
        val &= ~rx_en_shift_pin;
        pmu_write(reg_rx_en, val);

        // oen = 0 && out val
        pmu_read(reg_oen_enb, &val);
        val &= ~((data_out_shift_pin << 8) | data_out_shift_pin);
        if (val_for_out) {
            val |= data_out_shift_pin;
        }
        pmu_write(reg_oen_enb, val);
    } else {
        // rx en = 1
        pmu_read(reg_rx_en, &val);
        val |= rx_en_shift_pin;
        pmu_write(reg_rx_en, val);

        // oen = 1 && out val
        pmu_read(reg_oen_enb, &val);
        val &= ~((data_out_shift_pin << 8) | data_out_shift_pin);
        val |= (data_out_shift_pin << 8);
        if (val_for_out) {
            val |= data_out_shift_pin;
        }
        pmu_write(reg_oen_enb, val);
    }
}

void pmu_gpio_pin_set(enum HAL_GPIO_PIN_T pin)
{
    uint16_t reg;
    uint16_t val;
    uint16_t shift_pin;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return;
    }

    if (pin < HAL_GPIO_PIN_PMU_P1_0) {
        reg = PMU1806_REG_GPIO_P0_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        reg = PMU1806_REG_GPIO_P1_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P1_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P3_0) {
        reg = PMU1806_REG_GPIO_P2_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        reg = PMU1806_REG_GPIO_P3_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P3_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P5_0) {
        reg = PMU1806_REG_GPIO_P4_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        reg = PMU1806_REG_GPIO_P5_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P5_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P7_0) {
        reg = PMU1806_REG_GPIO_P6_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);
    } else {
        reg = PMU1806_REG_GPIO_P7_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P7_0);
    }

    pmu_read(reg, &val);
    val |= shift_pin;
    pmu_write(reg, val);
}

void pmu_gpio_pin_clr(enum HAL_GPIO_PIN_T pin)
{
    uint16_t reg;
    uint16_t val;
    uint16_t shift_pin;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return;
    }

    if (pin < HAL_GPIO_PIN_PMU_P1_0) {
        reg = PMU1806_REG_GPIO_P0_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        reg = PMU1806_REG_GPIO_P1_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P1_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P3_0) {
        reg = PMU1806_REG_GPIO_P2_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        reg = PMU1806_REG_GPIO_P3_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P3_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P5_0) {
        reg = PMU1806_REG_GPIO_P4_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        reg = PMU1806_REG_GPIO_P5_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P5_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P7_0) {
        reg = PMU1806_REG_GPIO_P6_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);
    } else {
        reg = PMU1806_REG_GPIO_P7_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P7_0);
    }

    pmu_read(reg, &val);
    val &= ~shift_pin;
    pmu_write(reg, val);
}

uint8_t pmu_gpio_pin_get_output_val(enum HAL_GPIO_PIN_T pin)
{
    uint16_t reg;
    uint16_t val;
    uint16_t shift_pin;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return 0xF;
    }

    if (pin < HAL_GPIO_PIN_PMU_P1_0) {
        reg = PMU1806_REG_GPIO_P0_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P0_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        reg = PMU1806_REG_GPIO_P1_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P1_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P3_0) {
        reg = PMU1806_REG_GPIO_P2_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P2_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        reg = PMU1806_REG_GPIO_P3_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P3_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P5_0) {
        reg = PMU1806_REG_GPIO_P4_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P4_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        reg = PMU1806_REG_GPIO_P5_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P5_0);
    } else if (pin < HAL_GPIO_PIN_PMU_P7_0) {
        reg = PMU1806_REG_GPIO_P6_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P6_0);
    } else {
        reg = PMU1806_REG_GPIO_P7_DATA_OUT;
        shift_pin = 1 << (pin - HAL_GPIO_PIN_PMU_P7_0);
    }

    pmu_read(reg, &val);
    return !!(val & shift_pin);
}

uint8_t pmu_gpio_pin_get_input_val(enum HAL_GPIO_PIN_T pin)
{
    uint16_t reg;
    uint16_t val;
    uint16_t io_mask = 0;
    uint16_t shift_pin;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return 0xF;
    }

    if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P0_0;
        reg = PMU1806_REG_GPIO_P0_P1_DATA_IN;
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P2_0;
        reg = PMU1806_REG_GPIO_P2_P3_DATA_IN;
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P4_0;
        reg = PMU1806_REG_GPIO_P4_P5_DATA_IN;
    } else {
        reg = PMU1806_REG_GPIO_P7_PULL_SEL;
        shift_pin = pin - HAL_GPIO_PIN_PMU_P6_0;
        reg = PMU1806_REG_GPIO_P6_P7_DATA_IN;
    }

    io_mask = 1 << shift_pin;
    pmu_read(reg, &val);
    return !!(val & io_mask);
}

uint32_t pmu_gpio_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    uint16_t reg;
    uint16_t val;
    uint16_t shift_pin;

    uint32_t pu_idx;
    uint32_t pd_idx;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return 1;
    }

    if (pin >= HAL_IOMUX_PIN_PMU_NUM || pin < HAL_IOMUX_PIN_PMU_P0_0) {
        return 2;
    }

    if (pin < HAL_IOMUX_PIN_PMU_P1_0) {
        reg = PMU1806_REG_GPIO_P0_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P0_0);
        pd_idx = shift_pin;
        pu_idx = shift_pin << 8;
    } else if (pin < HAL_IOMUX_PIN_PMU_P2_0) {
        reg = PMU1806_REG_GPIO_P1_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P1_0);
        pd_idx = shift_pin;
        pu_idx = shift_pin << 8;
    } else if (pin < HAL_IOMUX_PIN_PMU_P3_0) {
        reg = PMU1806_REG_GPIO_P2_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P2_0);
        pd_idx = shift_pin;
        pu_idx = shift_pin << 8;
    } else if (pin < HAL_IOMUX_PIN_PMU_P4_0) {
        reg = PMU1806_REG_GPIO_P3_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P3_0);
        pd_idx = shift_pin;
        pu_idx = shift_pin << 8;
    } else if (pin < HAL_IOMUX_PIN_PMU_P5_0) {
        reg = PMU1806_REG_GPIO_P4_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P4_0);
        pd_idx = shift_pin;
        pu_idx = shift_pin << 8;
    } else if (pin < HAL_IOMUX_PIN_PMU_P6_0) {
        reg = PMU1806_REG_GPIO_P5_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P5_0);
        pd_idx = shift_pin;
        pu_idx = shift_pin << 8;
    } else if (pin < HAL_IOMUX_PIN_PMU_P7_0) {
        reg = PMU1806_REG_GPIO_P6_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P6_0);
        pd_idx = shift_pin;
        pu_idx = shift_pin << 8;
    } else {
        reg = PMU1806_REG_GPIO_P7_PULL_SEL;
        shift_pin = 1 << (pin - HAL_IOMUX_PIN_PMU_P7_0);
        pd_idx = shift_pin << 8;
        pu_idx = shift_pin << 12;
    }

    pmu_read(reg, &val);
    val &= ~(pd_idx | pu_idx);
    if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
        val |= pd_idx;
    } else if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
        val |= pu_idx;
    }
    pmu_write(reg, val);

    return 0;
}

uint32_t pmu_gpio_set_io_drv(enum HAL_IOMUX_PIN_T pin, uint32_t val)
{
    uint16_t reg_val;
    uint16_t shift_bank;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return 1;
    }

    if (pin >= HAL_IOMUX_PIN_PMU_NUM || pin < HAL_IOMUX_PIN_PMU_P0_0) {
        return 2;
    }

    shift_bank = (pin - HAL_IOMUX_PIN_PMU_P0_0) / 8 * 2;

    pmu_read(PMU1806_REG_GPIO_DRV_SEL, &reg_val);
    reg_val &= ~(0x3 << shift_bank);
    reg_val |= (val << shift_bank);
    pmu_write(PMU1806_REG_GPIO_DRV_SEL, reg_val);

    return 0;
}

static enum PMU_GPIO_IRQ_TYPE_T pmu_gpio_irq1_type_match(uint16_t shift_pin, uint16_t shift_group)
{
    enum PMU_GPIO_IRQ_TYPE_T type = PMU_GPIO_IRQ_NONE;
    uint16_t val;
    uint16_t edge_sel;

    edge_sel = pmu_gpio_irq_cfg_save.pmu_gpio_irq_edge_sel[shift_group];
    if (edge_sel & shift_pin) {
        val = pmu_gpio_irq_cfg_save.pmu_gpio_irq_posedge_sel[shift_group];
        if (val & shift_pin) {
            type = PMU_GPIO_IRQ_RISING;
        } else {
            type = PMU_GPIO_IRQ_FALLING;
        }
    } else {
        val = pmu_gpio_irq_cfg_save.pmu_gpio_irq_lowlevel_sel[shift_group];
        if (val & shift_pin) {
            type = PMU_GPIO_IRQ_LOW_LEVEL;
        } else {
            type = PMU_GPIO_IRQ_HIGH_LEVEL;
        }
    }

    return type;
}

static void _pmu_gpio_irq_handler_v2(uint16_t irq_status)
{
    uint32_t pin_base;
    uint16_t shift_group;
    uint16_t shift_group_remap;
    uint16_t shift_pin;
    uint16_t reg;
    uint16_t val;
    bool irq1_masked = false;
    bool irq2_masked = false;
    bool irq1_pin_4_7_masked = false;
    bool irq2_pin_4_7_masked= false;

    irq1_masked = !!(irq_status & PMU_GPIO_INTR_MASKED);
    irq2_masked = !!(irq_status & PMU_GPIO_NEDG_INTR_MASKED);

    if (irq1_masked) {
        irq1_pin_4_7_masked = !!(irq_status & PMU_GPIO_INTR_MASKED_P4_P7);

        if (irq1_pin_4_7_masked) {
            shift_group_remap = 2;
        } else {
            shift_group_remap = 0;
        }

        for (shift_group = shift_group_remap; shift_group < PMU_GPIO_PIN_GROUP_NUM; shift_group++) {
            reg = PMU1806_REG_GPIO_P0_P1_INTR_MSKED + shift_group;
            pmu_read(reg, &val);
            if (val) {
                //clear irq
                reg = PMU1806_REG_GPIO_P0_P1_INTR_CLR + shift_group;
                pmu_write(reg, val);
                val &= pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_group][0];
                pin_base = 0;
                pin_base += shift_group * 16;
                for (uint32_t i = 0; i < 16; i++) {
                    shift_pin = 1 << i;
                    if ((val & shift_pin) && pmu_gpio_irq_handler[pin_base + i]) {
                        pmu_gpio_irq_type = pmu_gpio_irq1_type_match(shift_pin, shift_group);
                        pmu_gpio_irq_handler[pin_base + i](pin_base + i + HAL_GPIO_PIN_PMU_P0_0);
                    }
                }
            }
        }
    }

    if (irq2_masked) {
        pmu_gpio_irq_type = PMU_GPIO_IRQ_FALLING;
        irq2_pin_4_7_masked = !!(irq_status & PMU_GPIO_NEDG_INTR_MASKED_P4_P7);

        if (irq2_pin_4_7_masked) {
            shift_group_remap = 2;
        } else {
            shift_group_remap = 0;
        }

        for (shift_group = shift_group_remap; shift_group < PMU_GPIO_PIN_GROUP_NUM; shift_group++) {
            reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_MSKED + shift_group;
            pmu_read(reg, &val);
            if (val) {
                //clear irq
                reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_CLR + shift_group;
                pmu_write(reg, val);
                val &= pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_group][1];
                pin_base = 0;
                pin_base += shift_group * 16;
                for (uint32_t i = 0; i < 16; i++) {
                    shift_pin = 1 << i;
                    if ((val & shift_pin) && pmu_gpio_irq_handler[pin_base + i]) {
                        pmu_gpio_irq_handler[pin_base + i](pin_base + i + HAL_GPIO_PIN_PMU_P0_0);
                    }
                }
            }
        }
    }
}

static void _pmu_gpio_irq_handler(uint16_t irq_status)
{
    uint32_t pin_base;
    uint16_t shift_group;
    uint16_t shift_pin;
    uint16_t reg;
    uint16_t val;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return;
    }

    if (pmu_metal_id >= HAL_CHIP_METAL_ID_1) {
        _pmu_gpio_irq_handler_v2(irq_status);
        return;
    }

    for (shift_group = 0; shift_group < PMU_GPIO_PIN_GROUP_NUM; shift_group++) {
        reg = PMU1806_REG_GPIO_P0_P1_INTR_MSKED + shift_group;
        pmu_read(reg, &val);
        if (val) {
            //clear irq
            reg = PMU1806_REG_GPIO_P0_P1_INTR_CLR + shift_group;
            pmu_write(reg, val);
            val &= pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_group][0];
            pin_base = 0;
            pin_base += shift_group * 16;
            for (uint32_t i = 0; i < 16; i++) {
                shift_pin = 1 << i;
                if ((val & shift_pin) && pmu_gpio_irq_handler[pin_base + i]) {
                    pmu_gpio_irq_type = pmu_gpio_irq1_type_match(shift_pin, shift_group);
                    pmu_gpio_irq_handler[pin_base + i](pin_base + i + HAL_GPIO_PIN_PMU_P0_0);
                }
            }
        }
    }

    for (shift_group = 0; shift_group < PMU_GPIO_PIN_GROUP_NUM; shift_group++) {
        reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_MSKED + shift_group;
        pmu_read(reg, &val);
        if (val) {
            pmu_gpio_irq_type = PMU_GPIO_IRQ_FALLING;
            //clear irq
            reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_CLR + shift_group;
            pmu_write(reg, val);
            val &= pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_group][1];
            pin_base = 0;
            pin_base += shift_group * 16;
            for (uint32_t i = 0; i < 16; i++) {
                shift_pin = 1 << i;
                if ((val & shift_pin) && pmu_gpio_irq_handler[pin_base + i]) {
                    pmu_gpio_irq_handler[pin_base + i](pin_base + i + HAL_GPIO_PIN_PMU_P0_0);
                }
            }
        }
    }
}

static uint8_t pmu_gpio_setup_negedge_irq(enum HAL_GPIO_PIN_T pin, bool en)
{
    uint16_t reg;
    uint16_t val;
    uint16_t io_mask;
    uint16_t shift_pin;
    uint16_t shift_reg;
    uint16_t shift_reg_nedg_intr_sel;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return 1;
    }

    if (pin >= HAL_GPIO_PIN_PMU_NUM || pin < HAL_GPIO_PIN_PMU_P0_0) {
        return 2;
    }

    if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P0_0;
        shift_reg = 0;
        shift_reg_nedg_intr_sel = shift_reg * 7;
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P2_0;
        shift_reg = 1;
        shift_reg_nedg_intr_sel = shift_reg * 7;
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P4_0;
        shift_reg = 2;
        shift_reg_nedg_intr_sel = shift_reg * 7;
    } else {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P6_0;
        shift_reg = 3;
        shift_reg_nedg_intr_sel = 2 * 7 + 4;
    }

    io_mask = 1 << shift_pin;

    if (en) {
        // set io nedg sel
        reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_SEL + shift_reg_nedg_intr_sel;
        pmu_read(reg, &val);
        val |= io_mask;
        pmu_write(reg, val);
        // set io nedg mask
        reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_MSK + shift_reg;
        pmu_read(reg, &val);
        val |= io_mask;
        pmu_write(reg, val);
        pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_reg][1] = val;
        // clear io irq
        reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_CLR + shift_reg;
        val = io_mask;
        pmu_write(reg, val);
    } else {
        // clear io nedg sel
        reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_SEL + shift_reg_nedg_intr_sel;
        pmu_read(reg, &val);
        val &= ~io_mask;
        pmu_write(reg, val);
        // clear io mask
        reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_MSK + shift_reg;
        pmu_read(reg, &val);
        val &= ~io_mask;
        pmu_write(reg, val);
        pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_reg][1] = val;
        // clear io irq
        reg = PMU1806_REG_GPIO_P0_P1_NEDG_INTR_CLR + shift_reg;
        val = io_mask;
        pmu_write(reg, val);
    }

    return 0;
}

uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg)
{
    uint16_t reg;
    uint16_t val;
    uint16_t io_mask;
    uint16_t shift_pin;
    uint16_t shift_reg;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return 1;
    }

    if (pin >= HAL_GPIO_PIN_PMU_NUM || pin < HAL_GPIO_PIN_PMU_P0_0) {
        return 2;
    }

    if (pin < HAL_GPIO_PIN_PMU_P2_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P0_0;
        shift_reg = 0;
    } else if (pin < HAL_GPIO_PIN_PMU_P4_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P2_0;
        shift_reg = 1;
    } else if (pin < HAL_GPIO_PIN_PMU_P6_0) {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P4_0;
        shift_reg = 2;
    } else {
        shift_pin = pin - HAL_GPIO_PIN_PMU_P6_0;
        shift_reg = 3;
    }

    io_mask = 1 << shift_pin;

    // common-configuration, edge/level
    reg = PMU1806_REG_GPIO_P0_P1_EDGE_SEL + shift_reg;
    pmu_read(reg, &val);
    if (cfg->irq_type == HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE) {
        val &= ~io_mask;
    } else {
        val |= io_mask;
    }
    pmu_write(reg, val);
    pmu_gpio_irq_cfg_save.pmu_gpio_irq_edge_sel[shift_reg] = val;

    if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_EDGE_BOTH) {
        pmu_gpio_setup_negedge_irq(pin, cfg->irq_enable);
        // edge both, intr1 select posedge only.
        reg = PMU1806_REG_GPIO_P0_P1_POS_INTR_SEL + shift_reg;
        pmu_read(reg, &val);
        val |= io_mask;
        pmu_write(reg, val);
        pmu_gpio_irq_cfg_save.pmu_gpio_irq_posedge_sel[shift_reg] = val;
    } else {
        if (cfg->irq_type == HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE) {
            reg = PMU1806_REG_GPIO_P0_P1_POS_INTR_SEL + shift_reg;
            pmu_read(reg, &val);
            if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_HIGH_RISING) {
                val |= io_mask;
            } else {
                val &= ~io_mask;
            }
            pmu_write(reg, val);
            pmu_gpio_irq_cfg_save.pmu_gpio_irq_posedge_sel[shift_reg] = val;
        } else {
            reg = PMU1806_REG_GPIO_P0_P1_LVL_INTR_SEL + shift_reg;
            pmu_read(reg, &val);
            if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_HIGH_RISING) {
                val &= ~io_mask;
            } else {
                val |= io_mask;
            }
            pmu_write(reg, val);
            pmu_gpio_irq_cfg_save.pmu_gpio_irq_lowlevel_sel[shift_reg] = val;
        }
    }

    // common-configuration, debounce
    reg = PMU1806_REG_GPIO_P0_P1_DB_BYPASS + shift_reg;
    pmu_read(reg, &val);
    if (cfg->irq_debounce) {
        val &= ~io_mask;
    } else {
        val |= io_mask;
    }
    pmu_write(reg, val);

    // common-configuration, irq enable
    if (cfg->irq_enable) {
        // set io mask
        reg = PMU1806_REG_GPIO_P0_P1_INTR_MSK + shift_reg;
        pmu_read(reg, &val);
        val |= io_mask;
        pmu_write(reg, val);
        pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_reg][0] = val;
        // clear io irq
        reg = PMU1806_REG_GPIO_P0_P1_INTR_CLR + shift_reg;
        val = io_mask;
        pmu_write(reg, val);
        // enable io irq
        reg = PMU1806_REG_GPIO_P0_P1_INTR_EN + shift_reg;
        pmu_read(reg, &val);
        val |= io_mask;
        pmu_write(reg, val);
        if (cfg->irq_handler) {
            pmu_gpio_irq_handler[pin - HAL_GPIO_PIN_PMU_P0_0] = cfg->irq_handler;
        }
    } else {
        // clrea io mask
        reg = PMU1806_REG_GPIO_P0_P1_INTR_MSK + shift_reg;
        pmu_read(reg, &val);
        val &= ~io_mask;
        pmu_write(reg, val);
        pmu_gpio_irq_cfg_save.pmu_gpio_irq_mask[shift_reg][0] = val;
        // clear io irq
        reg = PMU1806_REG_GPIO_P0_P1_INTR_CLR + shift_reg;
        val = io_mask;
        pmu_write(reg, val);
        // disable io irq
        reg = PMU1806_REG_GPIO_P0_P1_INTR_EN + shift_reg;
        pmu_read(reg, &val);
        val &= ~io_mask;
        pmu_write(reg, val);
        pmu_gpio_irq_handler[pin - HAL_GPIO_PIN_PMU_P0_0] = NULL;
    }

    return 0;
}

enum PMU_GPIO_IRQ_TYPE_T pmu_gpio_irq_type_get(void)
{
    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return PMU_GPIO_IRQ_NONE;
    }

    return pmu_gpio_irq_type;
}

int pmu_gpio_irq_init(void)
{
    uint16_t reg;
    uint16_t val;
    uint16_t shift_group;
    uint16_t debounc_time;

    if (pmu_type != PMU_CHIP_TYPE_1806) {
        return 1;
    }

    hal_cmu_dcdc_clock_enable(HAL_CMU_DCDC_CLOCK_USER_PMUIO);

#ifdef PMU_GPIO_CLK_SOURCE_24M
    debounc_time = PMU_GPIO_DEBOUNCE_TARGET_24M;
#else
    debounc_time = PMU_GPIO_DEBOUNCE_TARGET_32K;
#endif

    for (shift_group = 0; shift_group < PMU_GPIO_PIN_GROUP_NUM; shift_group++){
        reg = PMU1806_REG_GPIO_P0_P1_DB_TARGET + shift_group;
        val = REG_PMU_GPIO_L_DB_TARGET(debounc_time) | REG_PMU_GPIO_H_DB_TARGET(debounc_time);
        pmu_write(reg, val);
    }

    pmu_read(PMU1806_REG_GPIO_IRQ_CONFIG, &val);
    val |= REG_PMU_GPIO_INTR_CLK_EN;
#ifdef PMU_GPIO_CLK_SOURCE_24M
    val |= REG_PMU_GPIO_CLK_DIV(PMU_GPIO_CLK_DIV_NORMAL);
    pmu_gpio_clk_select_slow(false);
#else
    pmu_gpio_clk_select_slow(true);
#endif
    pmu_write(PMU1806_REG_GPIO_IRQ_CONFIG, val);
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_GPIO, _pmu_gpio_irq_handler);

    return 0;
}

int pmu_volt2temperature(const uint16_t volt)
{
    static uint16_t room_temp_volt;
    static bool tempCalibrated = false;

    uint16_t room_temp_calib_val = 0;
    uint16_t efuse_value = 0;
    int temp_centigrade = 0;
    uint16_t room_temp_calib_val_default = 0;

    if (12 == pmu_get_gpadc_data_bits()) {
        room_temp_calib_val_default = 0xA75;
    } else {
        room_temp_calib_val_default = 0x25D;
    }

    if (!tempCalibrated) {
        pmu_get_efuse(PMU_EFUSE_PAGE_TEMPERATURE, &efuse_value);
        //Bit[11:0] records the ambient temperature when the chip is turned on
        efuse_value &= 0xFFF;
        room_temp_calib_val = (efuse_value > 0 ? efuse_value : room_temp_calib_val_default);

#if defined(GPADC_HAS_EXT_SLOPE_CAL) || defined(GPADC_HAS_EXT_SLOPE_CAL2)
        room_temp_volt = hal_gpadc_adc2volt_ext(room_temp_calib_val, HAL_GPADC_CHAN_0);
#else
        room_temp_volt = hal_gpadc_adc2volt(room_temp_calib_val);
#endif

        tempCalibrated = true;

        TRACE(4, "%s efuse:%d, calib:%d, volt:%d", __func__, efuse_value, room_temp_calib_val, room_temp_volt);
    }

    temp_centigrade = (int)(((volt - room_temp_volt) * 100 + PMU_NTC_VOLT2TEMP_VTHREM_K / 2) / PMU_NTC_VOLT2TEMP_VTHREM_K + 25);
    TRACE(2, "%s Current temperatur:%d", __func__, temp_centigrade);

    return temp_centigrade;
}

int pmu_external_crystal_ldo_ctrl(bool en)
{
    uint16_t val_sel;
    uint16_t val_dr;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU_REG_BIAS_CFG, &val_dr);
        pmu_read(PMU1806_REG_XO_32K_ISEL, &val_sel);
        if (en) {
            val_dr |= (PU_LP_BIAS_LDO_DR | PU_LP_BIAS_LDO_REG);
            val_sel |= REG_XO_32K_PWR_SEL;

        } else {
            val_dr &= ~(PU_LP_BIAS_LDO_DR | PU_LP_BIAS_LDO_REG);
            val_sel &= ~REG_XO_32K_PWR_SEL;
        }
        pmu_write(PMU1806_REG_XO_32K_ISEL, val_sel);
        pmu_write(PMU_REG_BIAS_CFG, val_dr);

        return 0;
    } else {
        return -1;
    }
}

static void pmu_external_crystal_capbit_set(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU1806_REG_XO_32K_CAP, &val);
        val = SET_BITFIELD(val, REG_XO_32K_CAPBIT, 0xEB);
        pmu_write(PMU1806_REG_XO_32K_CAP, val);
    }
}

void pmu_external_crystal_enable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_external_crystal_capbit_set();

        // power on external crystal.
        pmu_read(PMU1806_REG_XO_32K, &val);
        val = SET_BITFIELD(val, REG_XO_32K_LDO_PRECHARGE_VALUE, 0x1);
        val = SET_BITFIELD(val, REG_XO_32K_ISEL_VALUE, 0x3F);
        pmu_write(PMU1806_REG_XO_32K, val);
        pmu_read(PMU1806_REG_XO_32K, &val);
        val |= REG_XO_32K_PU_LDO | REG_XO_32K_PU_LDO_DR | REG_XO_32K_ISEL_DR;
        pmu_write(PMU1806_REG_XO_32K, val);
        hal_sys_timer_delay(MS_TO_TICKS(PMU_EXT_32K_STABLE_TIME));
#ifdef EXTERNAL_CRYSTAL_CLKOUT
        // Clock out from PB0 at 2700BP.
        pmu_read(PMU1806_REG_XO_32K, &val);
        val |= REG_XO_32K_EXT1_EN;
        pmu_write(PMU1806_REG_XO_32K, val);
        hal_sys_timer_delay(MS_TO_TICKS(PMU_EXT_32K_CLK_OUT_STABLE_TIME));
        // Clock out from PC4 at 2700BP.
        pmu_read(PMU1806_REG_XO_32K, &val);
        val |= REG_XO_32K_EXT2_EN;
        pmu_write(PMU1806_REG_XO_32K, val);
        hal_sys_timer_delay(MS_TO_TICKS(PMU_EXT_32K_CLK_OUT_STABLE_TIME));
        // Clock out from PF9 at 2700BP.
        pmu_read(PMU1806_REG_XO_32K_ISEL, &val);
        val |= REG_XO_32K_EXT3_EN;
        pmu_write(PMU1806_REG_XO_32K_ISEL, val);
        hal_sys_timer_delay(MS_TO_TICKS(PMU_EXT_32K_CLK_OUT_STABLE_TIME));
#endif
        // Pmu clock slecet to external crystal.
        pmu_read(PMU_REG_POWER_KEY_CFG, &val);
        val = SET_BITFIELD(val, CLK_32K_SEL, 0x1);
        pmu_write(PMU_REG_POWER_KEY_CFG, val);
        hal_sys_timer_delay(MS_TO_TICKS(PMU_CLK_SWITCH_STABLE_TIME));
    }
}

void pmu_sar_adc_vref_enable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef PMU_FORCE_LP_MODE
        pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_SAR_VREF_OUTPUT, true);
#endif

        // force enable
        pmu_read(PMU1806_REG_DCDC_RAMP_EN, &val);
        val |= REG_PU_AVDD25_ANA;
        pmu_write(PMU1806_REG_DCDC_RAMP_EN, val);

        pmu_read(PMU1806_REG_AVDD25, &val);
        val |= (REG_PU_SAR | REG_DR_PU_SAR);
        pmu_write(PMU1806_REG_AVDD25, val);

        // sar vref output
        pmu_sar_adc_vref_sw_pu(true);
    }
}

void pmu_sar_adc_vref_disable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
#ifdef PMU_FORCE_LP_MODE
        pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_SAR_VREF_OUTPUT, false);
#endif

        pmu_sar_adc_vref_sw_pu(false);

        pmu_read(PMU1806_REG_AVDD25, &val);
        val &= ~(REG_PU_SAR | REG_DR_PU_SAR);
        pmu_write(PMU1806_REG_AVDD25, val);

        pmu_read(PMU1806_REG_DCDC_RAMP_EN, &val);
        val &= ~REG_PU_AVDD25_ANA;
        pmu_write(PMU1806_REG_DCDC_RAMP_EN, val);
    }
}

static int pmu_vbat_div_ctrl(int enable)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    if (enable) {
        val |= REG_PU_VBAT_DIV;
    } else {
        val &= ~REG_PU_VBAT_DIV;
    }
    pmu_write(PMU_REG_POWER_KEY_CFG, val);
    int_unlock(lock);

    return 0;
}

static int pmu_vsys_div_ctrl(int enable)
{
    uint32_t lock;
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        if (enable) {
            val |= REG_PU_VSYS_DIV;
        } else {
            val &= ~REG_PU_VSYS_DIV;
        }
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
        int_unlock(lock);

        return 0;
    } else if (pmu_type == PMU_CHIP_TYPE_1808 || pmu_type == PMU_CHIP_TYPE_1803) {
        return pmu_vbat_div_ctrl(enable);
    }

    return 1;
}

static int pmu_vchgr_div_ctrl(int enable)
{
    uint32_t lock;
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        lock = int_lock();
        pmu_read(PMU1806_REG_ID_DET, &val);
        if (enable) {
            val |= REG_PU_VCHRG_DIV;
        } else {
            val &= ~REG_PU_VCHRG_DIV;
        }
        pmu_write(PMU1806_REG_ID_DET, val);
        int_unlock(lock);

        return 0;
    }

    return 1;
}

int pmu_gpadc_div_ctrl(enum HAL_GPADC_CHAN_T channel, int enable)
{
    int ret = 0;

    if (pmu_type == PMU_CHIP_TYPE_1803 || pmu_type == PMU_CHIP_TYPE_1808) {
        if (channel != HAL_GPADC_CHAN_BATTERY) {
            return ret;
        }
    }

    if (channel == HAL_GPADC_CHAN_BATTERY) {
        ret = pmu_vsys_div_ctrl(enable);
    } else if (channel == HAL_GPADC_CHAN_5) {
        ret = pmu_vbat_div_ctrl(enable);
    } else if (channel == HAL_GPADC_CHAN_6) {
        ret = pmu_vchgr_div_ctrl(enable);
    }

    return ret;
}

int pmu_gpadc_clear_chan_irq(unsigned short irq)
{
    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809) {
        return 1;
    }

    pmu_write(PMU1803_REG_INT_CLR, irq);

    return 0;
}

int pmu_gpadc_enable_chan(int ch, int enable)
{
    uint16_t val;

    if (!(pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1809)) {
        return 1;
    }

    // Bypass digital state machine for gpadc. Adc_ch selection takes effect immediately
    pmu_read(PMU_REG_GPADC_EN_DR, &val);
    if (enable) {
        val |= REG_GPADC_EN_REG(1 << ch);
    } else {
        val &= ~REG_GPADC_EN_REG(1 << ch);
    }
    val |= REG_GPADC_EN_DR;
    pmu_write(PMU_REG_GPADC_EN_DR, val);

    // 1805 also needs to configure GPADC_REG_CH_EN.
    return 1;
}

void pmu_shipment_mode_enable(bool en)
{
    shipment_en = en;
}

void pmu_ldo_vusb_off_pull_down(void)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    hal_sys_timer_delay_us(PMU_LDO_PULLDOWN_STABLE_TIME_US);

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val |= REG_PULLDOWN_VUSB;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1803) {
        pmu_read(PMU_REG_USB_CFG, &val);
        val |= REG_PULLDOWN_VUSB;
        pmu_write(PMU_REG_USB_CFG, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1808) {
        pmu_read(PMU1808_REG_BE, &val);
        val |= PMU1808_PULLDOWN_VUSB;
        pmu_write(PMU1808_REG_BE, val);
    }
    hal_sys_timer_delay_us(PMU_LDO_PULLDOWN_STABLE_TIME_US);

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1805) {
        val &= ~REG_PULLDOWN_VUSB;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1803) {
        val &= ~REG_PULLDOWN_VUSB;
        pmu_write(PMU_REG_USB_CFG, val);
    } else if (pmu_type == PMU_CHIP_TYPE_1808) {
        val &= ~PMU1808_PULLDOWN_VUSB;
        pmu_write(PMU1808_REG_BE, val);
    }
    int_unlock(lock);
}

void pmu_ldo_vio_off_pull_down(void)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_module_config(PMU_IO,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

    pmu_read(PMU_REG_HPPA_LDO_EN, &val);
    val |= REG_PULLDOWN_VIO;
    pmu_write(PMU_REG_HPPA_LDO_EN, val);
    hal_sys_timer_delay_us(PMU_LDO_PULLDOWN_STABLE_TIME_US);

    val &= ~REG_PULLDOWN_VIO;
    pmu_write(PMU_REG_HPPA_LDO_EN, val);
    int_unlock(lock);
}

void pmu_ldo_vusb_on_lp_dr(bool dr_lp)
{
    uint32_t lock;

    lock = int_lock();
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

#ifndef PMU_FORCE_LP_MODE
    uint16_t val;

    if (dr_lp) {
        pmu_read(PMU_REG_USB_CFG, &val);
        if (pmu_type == PMU_CHIP_TYPE_1803) {
            val |= PMU1803_LP_EN_VUSB_LDO_DR | PMU1803_LP_EN_VUSB_LDO_REG;
        } else {
            val |= PMU1806_LP_EN_VUSB_LDO_DR | PMU1806_LP_EN_VUSB_LDO_REG;
        }
        pmu_write(PMU_REG_USB_CFG, val);
    }
#endif

    int_unlock(lock);
}

void pmu_ldo_vio_on_lp_dr(bool dr_lp)
{
    uint32_t lock;

    lock = int_lock();
    pmu_module_config(PMU_IO,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

#ifndef PMU_FORCE_LP_MODE
    uint16_t val;

    if (dr_lp) {
        pmu_read(PMU_REG_IO_CFG, &val);
        val |= LP_EN_VIO_LDO_REG | LP_EN_VIO_LDO_DR;
        pmu_write(PMU_REG_IO_CFG, val);
    }
#endif

    int_unlock(lock);
}

void pmu_power_key_hw_reset_enable(uint8_t seconds)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806 || pmu_type == PMU_CHIP_TYPE_1809) {
        if (pmu_type == PMU_CHIP_TYPE_1806 && hal_get_chip_metal_id() < HAL_CHIP_METAL_ID_2) {
            return;
        }

        if (seconds > 60) {
            seconds = 60;
        }

        pmu_read(PMU1806_REG_WDT_CFG, &val);
        val = SET_BITFIELD(val, REG_HW_RESET_TIME, seconds) | REG_HW_RESET_EN;
        pmu_write(PMU1806_REG_WDT_CFG, val);
    }
}

void pmu_power_key_long_press(bool en)
{
    uint32_t lock;
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1806) {
        lock = int_lock();
        pmu_read(PMU1806_REG_RESETN_MODE, &val);
        if (en) {
            val |= PMU1806_PWRKEY_LONGPRESS;
        } else {
            val &= ~PMU1806_PWRKEY_LONGPRESS;
        }
        pmu_write(PMU1806_REG_RESETN_MODE, val);
        int_unlock(lock);
    }
}

int pmu_wdt_load_test(void)
{
    uint16_t val0, val1;

    pmu_read(PMU_REG_POWER_OFF, &val0);

    pmu_write(PMU_REG_WDT_TIMER, 30000);
    pmu_wdt_load_value();

    pmu_read(PMU_REG_POWER_OFF, &val1);

    TRACE(0, "wdt_load: %u -> %u", !!(val0 & REG_WDT_LOAD), !!(val1 & REG_WDT_LOAD));

    if ((val0 & REG_WDT_LOAD) == (val1 & REG_WDT_LOAD)) {
        TRACE(0, "BAD: wdt load not changed");
        return 1;
    } else {
        TRACE(0, "GOOD: wdt load changed");
        return 0;
    }
}

void pmu_hw_uvp_vcore_disable(void)
{
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806) {
        pmu_read(PMU_REG_HW_DET_CFG, &val);
        val &= ~VCORE_DET_EN;
        pmu_write(PMU_REG_HW_DET_CFG, val);
        hal_sys_timer_delay(MS_TO_TICKS(1));

        pmu_read(PMU_REG_INT_MASK, &val);
        val &= ~RESET_EN_VCORE_LOW;
        pmu_write(PMU_REG_INT_MASK, val);
    }
}

void pmu_hw_uvp_vcore_enable(uint16_t vcore_mv)
{
    uint32_t lock;
    uint16_t val;
    uint16_t vcore_sel;

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806) {
        lock = int_lock();
        pmu_vcore_req |= PMU_VCORE_HW_UVP_ENABLED;
        pmu_dig_set_volt(PMU_POWER_MODE_NONE);

        if (vcore_mv > PMU_HW_UVP_VCORE_MV_MAX) {
            vcore_mv = PMU_HW_UVP_VCORE_MV_MAX;
        } else if (vcore_mv < PMU_HW_UVP_VCORE_MV_MIN) {
            vcore_mv = PMU_HW_UVP_VCORE_MV_MIN;
        }

        vcore_sel = PMU_HW_UVP_VCORE_0_24V_RAW + ((vcore_mv - PMU_HW_UVP_VCORE_MV_MIN) / 20);

        // Enable with minimum threshold.
        pmu_read(PMU_REG_HW_DET_CFG, &val);
        val = SET_BITFIELD(val, VCORE_DET_RES_SEL, PMU_HW_UVP_VCORE_0_24V_RAW) | VCORE_DET_EN;
        pmu_write(PMU_REG_HW_DET_CFG, val);
        hal_sys_timer_delay(MS_TO_TICKS(1));

        pmu_read(PMU_REG_INT_MASK, &val);
        val |= RESET_EN_VCORE_LOW;
        pmu_write(PMU_REG_INT_MASK, val);
        hal_sys_timer_delay(MS_TO_TICKS(1));

        pmu_read(PMU_REG_HW_DET_CFG, &val);
        val = SET_BITFIELD(val, VCORE_DET_RES_SEL, vcore_sel);
        pmu_write(PMU_REG_HW_DET_CFG, val);

        pmu_vcore_req &= ~PMU_VCORE_HW_UVP_ENABLED;
        pmu_dig_set_volt(PMU_POWER_MODE_NONE);
        int_unlock(lock);
    }
}

static void pmu_capsensor_irq_handler(uint16_t irq_status)
{
    if (capsensor_irq_handler) {
        capsensor_irq_handler();
    }
}

int pmu_capsensor_set_irq_handler(PMU_CAP_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    if (pmu_type == PMU_CHIP_TYPE_1809) {
        capsensor_irq_handler = handler;

        lock = int_lock();
        pmu_read(PMU_REG_CAP_MSK, &val);
        if (handler) {
            val |= 1 << 8;
        } else {
            val &= ~(1 << 8);
        }
        pmu_write(PMU_REG_CAP_MSK, val);
        pmu_set_irq_unified_handler(PMU_IRQ_TYPE_CAPSENSOR, handler ? pmu_capsensor_irq_handler : NULL);
        int_unlock(lock);
        return 0;
    } else {
        return -1;
    }
}
