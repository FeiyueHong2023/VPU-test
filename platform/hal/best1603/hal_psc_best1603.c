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
#include "plat_addr_map.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_gpio.h"
#include "hal_location.h"
#include "hal_psc.h"
#ifdef ARM_CMNS
#include "hal_sleep.h"
#include "hal_sleep_core_pd.h"
#endif
#include "hal_timer.h"
#include CHIP_SPECIFIC_HDR(reg_psc)

#if defined(JTAG_BTH) || defined(JTAG_SENS) || defined(JTAG_BT)
#define PSC_M55_ALWAYS_ON
#endif

#define PSC_WRITE_ENABLE                    0xCAFE0000

#define MAIN_STATE_IDLE                     1
#define MAIN_STATE_POWERDN                  3
#define MAIN_STATE_POWERUP                  2
#define MAIN_STATE_IDLE_DUMMY               0

enum PSC_INTR_MASK2_T {
    PSC_INTR_MASK2_GPIO1_15_0           = (0xFFFF << 0),
};

enum PSC_WAKEUP_INTR_MASK_T {
    PSC_WAKEUP_INTR_MASK_GPIO           = (1 << 0),
    PSC_WAKEUP_INTR_MASK_TIMER11        = (1 << 1),
    PSC_WAKEUP_INTR_MASK_TIMER10        = (1 << 2),
    PSC_WAKEUP_INTR_MASK_TIMER00        = (1 << 3),
    PSC_WAKEUP_INTR_MASK_TIMER01        = (1 << 4),
    PSC_WAKEUP_INTR_MASK_INTPMU         = (1 << 5),
    PSC_WAKEUP_INTR_MASK_WDT            = (1 << 6),
    PSC_WAKEUP_INTR_MASK_EXTPMU         = (1 << 7),
    PSC_WAKEUP_INTR_MASK_VAD            = (1 << 8),
    PSC_WAKEUP_INTR_MASK_CODEC          = (1 << 9),
    PSC_WAKEUP_INTR_MASK_SPI_WAKEUP     = (1 << 10),
    PSC_WAKEUP_INTR_MASK_WAKEUP_BTC     = (1 << 11),
    PSC_WAKEUP_INTR_MASK_WAKEUP_BTH     = (1 << 12),
    PSC_WAKEUP_INTR_MASK_WAKEUP_SES     = (1 << 13),
    PSC_WAKEUP_INTR_MASK_WAKEUP_SYS     = (1 << 14),
    PSC_WAKEUP_INTR_MASK_USB_PIN        = (1 << 15),
};

enum HAL_PSC_RET_RAM_OWNER_T {
    HAL_PSC_RET_RAM_SYS0    = (1 << 0),
    HAL_PSC_RET_RAM_SYS1    = (1 << 1),
    HAL_PSC_RET_RAM_BTC     = (1 << 2),
    HAL_PSC_RET_RAM_BTH     = (1 << 3),
    HAL_PSC_RET_RAM_SENS    = (1 << 4),
};

enum HAL_PSC_SHR_RAM_PD_CFG_T {
    HAL_PSC_SHR_RAM_FORCE_PU = 0,
    HAL_PSC_SHR_RAM_FORCE_PD,
    HAL_PSC_SHR_RAM_SLEEP_PU,
    HAL_PSC_SHR_RAM_SLEEP_PD,
};

void hal_psc_sens_auto_power_down(int core_only);

void hal_psc_shr_ram_sleep_cfg(uint32_t map, enum HAL_PSC_SHR_RAM_PD_CFG_T cfg);

static struct AONPSC_T * const psc = (struct AONPSC_T *)AON_PSC_BASE;

static uint8_t dis_mod_map;
STATIC_ASSERT(sizeof(dis_mod_map) * 8 >= HAL_PSC_DISPLAY_MOD_QTY, "dis_mod_map size too small");

static uint8_t store_mod_map;
STATIC_ASSERT(sizeof(store_mod_map) * 8 >= HAL_PSC_STORAGE_MOD_QTY, "store_mod_map size too small");

static uint16_t BOOT_BSS_LOC m55_tcm_ram_map;
static uint16_t BOOT_BSS_LOC m55_shr_ram_map;
static uint16_t BOOT_BSS_LOC dsp_tcm_ram_map;
static uint16_t BOOT_BSS_LOC dsp_shr_ram_map;

static uint16_t BOOT_BSS_LOC m55_tcm_pu;
static uint16_t BOOT_BSS_LOC m55_tcm_pd;
static uint16_t BOOT_BSS_LOC m55_shr_pu;
static uint16_t BOOT_BSS_LOC m55_shr_pd;

static uint16_t BOOT_BSS_LOC dsp_tcm_pu;
static uint16_t BOOT_BSS_LOC dsp_tcm_pd;
static uint16_t BOOT_BSS_LOC dsp_shr_pu;
static uint16_t BOOT_BSS_LOC dsp_shr_pd;

static uint16_t BOOT_BSS_LOC sens_shr_pu;
static uint16_t BOOT_BSS_LOC sens_shr_pd;

static uint16_t BOOT_BSS_LOC bth_shr_pu;
static uint16_t BOOT_BSS_LOC bth_shr_pd;

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
BOOT_BSS_LOC
static uint8_t sys_mod_map;
STATIC_ASSERT(sizeof(sys_mod_map) * 8 >= HAL_PSC_SYS_USER_QTY, "sys_mod_map size too small");
#endif

//==============================================================
// Start of AON CMU sleep ram cfg

#include CHIP_SPECIFIC_HDR(reg_aoncmu)

#define SLP_SYS_RAM0                    (1 << 0)
#define SLP_SYS_RAM1                    (1 << 1)
#define SLP_SYS_RAM2                    (1 << 2)
#define SLP_SYS_RAM3                    (1 << 3)
#define SLP_SYS_RAM4                    (1 << 4)
#define SLP_SYS_RAM5                    (1 << 5)
#define SLP_SYS_RAM6                    (1 << 6)
#define SLP_SYS_RAM7                    (1 << 7)
#define SLP_SYS_RAM8                    (1 << 8)
#define SLP_SYS_RAM9                    (1 << 9)
#define SLP2_SYS_RAM10                  (1 << 0)
#define SLP2_SYS_RAM11                  (1 << 1)
#define SLP2_SYS_RAM12                  (1 << 2)
#define SLP2_SYS_RAM13                  (1 << 3)
#define SLP2_SYS_RAM14                  (1 << 4)
#define SLP2_SYS_RAM15                  (1 << 5)
#define SLP2_BTH_RAM0_0                 (1 << 6)
#define SLP2_BTH_RAM0_1                 (1 << 7)
#define SLP2_BTH_RAM0_2                 (1 << 8)
#define SLP2_BTH_RAM0_3                 (1 << 9)
#define SLP3_BTH_RAM0_4                 (1 << 0)
#define SLP3_BTH_RAM0_5                 (1 << 1)
#define SLP3_BTH_RAM0_6                 (1 << 2)
#define SLP3_BTH_RAM0_7                 (1 << 3)
#define SLP3_BTH_RAM1_0                 (1 << 4)
#define SLP3_BTH_RAM1_1                 (1 << 5)
#define SLP3_BTH_RAM1_2                 (1 << 6)
#define SLP3_BTH_RAM1_3                 (1 << 7)
#define SLP3_BTH_CACHE                  (1 << 8)
#define SLP3_BTC_RAM0                   (1 << 9)
#define SLP4_BTC_RAM1                   (1 << 0)
#define SLP4_BTC_RAM2                   (1 << 1)
#define SLP4_BTC_RAM3                   (1 << 2)
#define SLP4_BTC_RAM4                   (1 << 3)
#define SLP4_BTC_RAM5                   (1 << 4)
#define SLP4_BTC_EM_RAM0                (1 << 5)
#define SLP4_BTC_EM_RAM1                (1 << 6)
#define SLP4_BTC_EM_RAM2                (1 << 7)
#define SLP4_BTC_MDM_RAM                (1 << 8)
#define SLP4_BTC_PATCH_RAM              (1 << 9)
#define SLP4_BTC_EM_RAM3                (1 << 0)
#define SLP4_BTC_EM_RAM4                (1 << 1)
#define SLP4_BTC_EM_RAM5                (1 << 2)
#define SLP4_BTC_EM_RAM6                (1 << 3)
#define SLP5_SENS_RAM4_0                (1 << 0)
#define SLP5_SENS_RAM4_1                (1 << 1)
#define SLP5_SENS_RAM4_2                (1 << 2)
#define SLP5_SENS_RAM4_3                (1 << 3)
#define SLP5_SENS_RAM5_0                (1 << 4)
#define SLP5_SENS_RAM5_1                (1 << 5)
#define SLP5_SENS_RAM5_2                (1 << 6)
#define SLP5_SENS_RAM5_3                (1 << 7)
#define SLP5_SHR_RAM0_0                 (1 << 8)
#define SLP5_SHR_RAM0_1                 (1 << 9)
#define SLP6_SHR_RAM0_2                 (1 << 0)
#define SLP6_SHR_RAM0_3                 (1 << 1)
#define SLP6_SHR_RAM1_0                 (1 << 2)
#define SLP6_SHR_RAM1_1                 (1 << 3)
#define SLP6_SHR_RAM1_2                 (1 << 4)
#define SLP6_SHR_RAM1_3                 (1 << 5)
#define SLP6_SHR_RAM2                   (1 << 6)
#define SLP6_SHR_RAM3                   (1 << 7)
#define SLP6_SHR_RAM4                   (1 << 8)
#define SLP6_SHR_RAM5                   (1 << 9)
#define SLP7_SHR_RAM6                   (1 << 0)
#define SLP7_SHR_RAM7                   (1 << 1)
#define SLP7_SHR_RAM8                   (1 << 2)
#define SLP7_SHR_RAM9                   (1 << 3)
#define SLP7_SHR_RAM10_0                (1 << 4)
#define SLP7_SHR_RAM10_1                (1 << 5)
#define SLP7_SHR_RAM10_2                (1 << 6)
#define SLP7_SHR_RAM10_3                (1 << 7)

static struct AONCMU_T * const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

BOOT_TEXT_SRAM_LOC
void hal_cmu_sys_ram_active_cfg(uint32_t map, int pd)
{
    uint32_t bfv;
    uint32_t val;
    uint32_t mask;

    bfv = 0;
    mask = SLP_SYS_RAM0 | SLP_SYS_RAM1 | SLP_SYS_RAM2 | SLP_SYS_RAM3 |
        SLP_SYS_RAM4 | SLP_SYS_RAM5 | SLP_SYS_RAM6 | SLP_SYS_RAM7;
    mask |= SLP_SYS_RAM8 | SLP_SYS_RAM9;
    bfv = map & mask;
    if (bfv) {
        if (pd) {
            val = AON_CMU_RAM_SD1(bfv);
            mask = AON_CMU_RAM_DSLP1(bfv) | AON_CMU_RAM_SLP1(bfv);
        } else {
            val = 0;
            mask = AON_CMU_RAM_SD1(bfv) | AON_CMU_RAM_DSLP1(bfv) | AON_CMU_RAM_SLP1(bfv);
        }
        aoncmu->RAM_SLP1 = (aoncmu->RAM_SLP1 & ~mask) | val;
    }

    bfv = 0;
    mask = SLP2_SYS_RAM10 | SLP2_SYS_RAM11 | SLP2_SYS_RAM12 |
        SLP2_SYS_RAM13 | SLP2_SYS_RAM14 | SLP2_SYS_RAM15;
    bfv = (map >> 10) & mask;
    if (bfv) {
        if (pd) {
            val = AON_CMU_RAM2_SD1(bfv);
            mask = AON_CMU_RAM2_DSLP1(bfv) | AON_CMU_RAM2_SLP1(bfv);
        } else {
            val = 0;
            mask = AON_CMU_RAM2_SD1(bfv) | AON_CMU_RAM2_DSLP1(bfv) | AON_CMU_RAM2_SLP1(bfv);
        }
        aoncmu->RAM2_SLP1 = (aoncmu->RAM2_SLP1 & ~mask) | val;
    }
}

BOOT_TEXT_SRAM_LOC
void hal_cmu_sys_ram_sleep_cfg(uint32_t map, int pd)
{
    uint32_t bfv;
    uint32_t val;
    uint32_t mask;

    bfv = 0;
    mask = SLP_SYS_RAM0 | SLP_SYS_RAM1 | SLP_SYS_RAM2 | SLP_SYS_RAM3 |
        SLP_SYS_RAM4 | SLP_SYS_RAM5 | SLP_SYS_RAM6 | SLP_SYS_RAM7;
    mask |= SLP_SYS_RAM8 | SLP_SYS_RAM9;
    bfv = map & mask;
    if (bfv) {
        if (pd) {
            val = AON_CMU_RAM_SD0(bfv);
            mask = AON_CMU_RAM_DSLP0(bfv) | AON_CMU_RAM_SLP0(bfv);
        } else {
            val = AON_CMU_RAM_DSLP0(bfv);
            mask = AON_CMU_RAM_SD0(bfv) | AON_CMU_RAM_SLP0(bfv);
        }
        aoncmu->RAM_SLP0 = (aoncmu->RAM_SLP0 & ~mask) | val;
    }

    bfv = 0;
    mask = SLP2_SYS_RAM10 | SLP2_SYS_RAM11 | SLP2_SYS_RAM12 |
        SLP2_SYS_RAM13 | SLP2_SYS_RAM14 | SLP2_SYS_RAM15;
    bfv = (map >> 10) & mask;
    if (bfv) {
        if (pd) {
            val = AON_CMU_RAM2_SD0(bfv);
            mask = AON_CMU_RAM2_DSLP0(bfv) | AON_CMU_RAM2_SLP0(bfv);
        } else {
            val = AON_CMU_RAM2_DSLP0(bfv);
            mask = AON_CMU_RAM2_SD0(bfv) | AON_CMU_RAM2_SLP0(bfv);
        }
        aoncmu->RAM2_SLP0 = (aoncmu->RAM2_SLP0 & ~mask) | val;
    }
}

SRAM_TEXT_LOC
void hal_cmu_btc_ram_sleep_cfg(int pd)
{
    uint32_t bfv;
    uint32_t val;
    uint32_t mask;

    bfv = SLP3_BTC_RAM0;
    if (pd) {
        val = AON_CMU_RAM3_SD0(bfv);
        mask = AON_CMU_RAM3_DSLP0(bfv) | AON_CMU_RAM3_SLP0(bfv);
    } else {
        val = AON_CMU_RAM3_DSLP0(bfv);
        mask = AON_CMU_RAM3_SD0(bfv) | AON_CMU_RAM3_SLP0(bfv);
    }
    aoncmu->RAM3_SLP0 = (aoncmu->RAM3_SLP0 & ~mask) | val;

    bfv = SLP4_BTC_RAM1 | SLP4_BTC_RAM2 | SLP4_BTC_RAM3 | SLP4_BTC_RAM4 |
        SLP4_BTC_RAM5 | SLP4_BTC_EM_RAM0 | SLP4_BTC_EM_RAM1 | SLP4_BTC_EM_RAM2 |
        SLP4_BTC_MDM_RAM | SLP4_BTC_PATCH_RAM;
    if (pd) {
        val = AON_CMU_RAM4_SD0(bfv);
        mask = AON_CMU_RAM4_DSLP0(bfv) | AON_CMU_RAM4_SLP0(bfv);
    } else {
        val = AON_CMU_RAM4_DSLP0(bfv);
        mask = AON_CMU_RAM4_SD0(bfv) | AON_CMU_RAM4_SLP0(bfv);
    }
    aoncmu->RAM4_SLP0 = (aoncmu->RAM4_SLP0 & ~mask) | val;

    bfv = SLP4_BTC_EM_RAM3 | SLP4_BTC_EM_RAM4 | SLP4_BTC_EM_RAM5 | SLP4_BTC_EM_RAM6;
    if (pd) {
        val = AON_CMU_RAM8_SD0(bfv);
        mask = AON_CMU_RAM8_DSLP0(bfv) | AON_CMU_RAM8_SLP0(bfv);
    } else {
        val = AON_CMU_RAM8_DSLP0(bfv);
        mask = AON_CMU_RAM8_SD0(bfv) | AON_CMU_RAM8_SLP0(bfv);
    }
    aoncmu->RAM8_SLP0 = (aoncmu->RAM8_SLP0 & ~mask) | val;
}

SRAM_TEXT_LOC
void hal_cmu_bth_ram_sleep_cfg(int pd)
{
    uint32_t bfv;
    uint32_t val;
    uint32_t mask;

    bfv = SLP2_BTH_RAM0_0 | SLP2_BTH_RAM0_1 | SLP2_BTH_RAM0_2 | SLP2_BTH_RAM0_3;
    if (pd) {
        val = AON_CMU_RAM2_SD0(bfv);
        mask = AON_CMU_RAM2_DSLP0(bfv) | AON_CMU_RAM2_SLP0(bfv);
    } else {
        val = AON_CMU_RAM2_DSLP0(bfv);
        mask = AON_CMU_RAM2_SD0(bfv) | AON_CMU_RAM2_SLP0(bfv);
    }
    aoncmu->RAM2_SLP0 = (aoncmu->RAM2_SLP0 & ~mask) | val;

    bfv = SLP3_BTH_RAM0_4 | SLP3_BTH_RAM0_5 | SLP3_BTH_RAM0_6 | SLP3_BTH_RAM0_7 |
        SLP3_BTH_RAM1_0 | SLP3_BTH_RAM1_1 | SLP3_BTH_RAM1_2 | SLP3_BTH_RAM1_3 |
        SLP3_BTH_CACHE;
    if (pd) {
        val = AON_CMU_RAM3_SD0(bfv);
        mask = AON_CMU_RAM3_DSLP0(bfv) | AON_CMU_RAM3_SLP0(bfv);
    } else {
        val = AON_CMU_RAM3_DSLP0(bfv);
        mask = AON_CMU_RAM3_SD0(bfv) | AON_CMU_RAM3_SLP0(bfv);
    }
    aoncmu->RAM3_SLP0 = (aoncmu->RAM3_SLP0 & ~mask) | val;
}

SRAM_TEXT_LOC
void hal_cmu_sens_ram_sleep_cfg(int pd)
{
    uint32_t bfv;
    uint32_t val;
    uint32_t mask;

    bfv = SLP5_SENS_RAM4_0 | SLP5_SENS_RAM4_1 | SLP5_SENS_RAM4_2 | SLP5_SENS_RAM4_3 |
        SLP5_SENS_RAM5_0 | SLP5_SENS_RAM5_1 | SLP5_SENS_RAM5_2 | SLP5_SENS_RAM5_3;
    if (pd) {
        val = AON_CMU_RAM5_SD0(bfv);
        mask = AON_CMU_RAM5_DSLP0(bfv) | AON_CMU_RAM5_SLP0(bfv);
    } else {
        val = AON_CMU_RAM5_DSLP0(bfv);
        mask = AON_CMU_RAM5_SD0(bfv) | AON_CMU_RAM5_SLP0(bfv);
    }
    aoncmu->RAM5_SLP0 = (aoncmu->RAM5_SLP0 & ~mask) | val;
}

BOOT_TEXT_SRAM_LOC
void hal_cmu_shr_ram_sleep_cfg(uint32_t map, int pd)
{
    uint32_t bfv;
    uint32_t val;
    uint32_t mask;

    bfv = 0;
    if (map & (1 << 0)) {
        bfv |= SLP5_SHR_RAM0_0 | SLP5_SHR_RAM0_1;
    }
    if (bfv) {
        if (pd) {
            val = AON_CMU_RAM5_SD0(bfv);
            mask = AON_CMU_RAM5_DSLP0(bfv) | AON_CMU_RAM5_SLP0(bfv);
        } else {
            val = AON_CMU_RAM5_DSLP0(bfv);
            mask = AON_CMU_RAM5_SD0(bfv) | AON_CMU_RAM5_SLP0(bfv);
        }
        aoncmu->RAM5_SLP0 = (aoncmu->RAM5_SLP0 & ~mask) | val;
    }

    bfv = 0;
    if (map & (1 << 0)) {
        bfv |= SLP6_SHR_RAM0_2 | SLP6_SHR_RAM0_3;
    }
    if (map & (1 << 1)) {
        bfv |= SLP6_SHR_RAM1_0 | SLP6_SHR_RAM1_1 | SLP6_SHR_RAM1_2 | SLP6_SHR_RAM1_3;
    }
    if (map & (1 << 2)) {
        bfv |= SLP6_SHR_RAM2;
    }
    if (map & (1 << 3)) {
        bfv |= SLP6_SHR_RAM3;
    }
    if (map & (1 << 4)) {
        bfv |= SLP6_SHR_RAM4;
    }
    if (map & (1 << 5)) {
        bfv |= SLP6_SHR_RAM5;
    }
    if (bfv) {
        if (pd) {
            val = AON_CMU_RAM6_SD0(bfv);
            mask = AON_CMU_RAM6_DSLP0(bfv) | AON_CMU_RAM6_SLP0(bfv);
        } else {
            val = AON_CMU_RAM6_DSLP0(bfv);
            mask = AON_CMU_RAM6_SD0(bfv) | AON_CMU_RAM6_SLP0(bfv);
        }
        aoncmu->RAM6_SLP0 = (aoncmu->RAM6_SLP0 & ~mask) | val;
    }

    bfv = 0;
    if (map & (1 << 6)) {
        bfv |= SLP7_SHR_RAM6;
    }
    if (map & (1 << 7)) {
        bfv |= SLP7_SHR_RAM7;
    }
    if (map & (1 << 8)) {
        bfv |= SLP7_SHR_RAM8;
    }
    if (map & (1 << 9)) {
        bfv |= SLP7_SHR_RAM9;
    }
    if (map & (1 << 10)) {
        bfv |= SLP7_SHR_RAM10_0 | SLP7_SHR_RAM10_1 | SLP7_SHR_RAM10_2 | SLP7_SHR_RAM10_3;
    }
    if (bfv) {
        if (pd) {
            val = AON_CMU_RAM7_SD0(bfv);
            mask = AON_CMU_RAM7_DSLP0(bfv) | AON_CMU_RAM7_SLP0(bfv);
        } else {
            val = AON_CMU_RAM7_DSLP0(bfv);
            mask = AON_CMU_RAM7_SD0(bfv) | AON_CMU_RAM7_SLP0(bfv);
        }
        aoncmu->RAM7_SLP0 = (aoncmu->RAM7_SLP0 & ~mask) | val;
    }
}

void hal_psc_ram_ret_during_reset(void)
{
    // Sleep retention
    aoncmu->RAM_SLP0 = AON_CMU_RAM_DSLP0_MASK;
    aoncmu->RAM2_SLP0 = AON_CMU_RAM2_DSLP0_MASK;
    aoncmu->RAM3_SLP0 = AON_CMU_RAM3_DSLP0_MASK;
    aoncmu->RAM4_SLP0 = AON_CMU_RAM4_DSLP0_MASK;
    aoncmu->RAM5_SLP0 = AON_CMU_RAM5_DSLP0_MASK;
    aoncmu->RAM6_SLP0 = AON_CMU_RAM6_DSLP0_MASK;
    aoncmu->RAM7_SLP0 = AON_CMU_RAM7_DSLP0_MASK;
    aoncmu->RAM7_SLP0;
    __DSB();

    // Not pd with subsys
    psc->REG_168 = (PSC_WRITE_ENABLE & 0xFF000000) | 0;
    //psc->REG_16C = (PSC_WRITE_ENABLE & 0xFF000000) | 0;
    //psc->REG_170 = (PSC_WRITE_ENABLE & 0xFF000000) | 0;

    // Sleep pu
    psc->REG_174 = 0;
    psc->REG_174;
    __DSB();
}

// End of AON CMU sleep ram cfg
//==============================================================

BOOT_TEXT_FLASH_LOC
static void hal_psc_jtag_select_sys(void)
{
    psc->REG_060 = (psc->REG_060 & ~PSC_AON_CODEC_RESERVED(1 << 3)) |
        PSC_AON_CODEC_RESERVED(1 << 2);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_jtag_select_sys_m55(void)
{
#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
    hal_cmu_jtag_select_sys_m55();
#endif
    hal_psc_jtag_select_sys();
}

BOOT_TEXT_FLASH_LOC
void hal_psc_jtag_select_sys_dsp(void)
{
#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
    hal_cmu_jtag_select_sys_dsp();
#endif
    hal_psc_jtag_select_sys();
}

BOOT_TEXT_FLASH_LOC
void hal_psc_jtag_select_sys_dual(void)
{
#if !(defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU))
    hal_cmu_jtag_select_sys_dual();
#endif
    hal_psc_jtag_select_sys();
}

BOOT_TEXT_FLASH_LOC
void hal_psc_jtag_select_bth(void)
{
    psc->REG_060 = (psc->REG_060 | PSC_AON_CODEC_RESERVED(1 << 2) |
        PSC_AON_CODEC_RESERVED(1 << 3));
}

BOOT_TEXT_FLASH_LOC
void hal_psc_jtag_select_bt(void)
{
    psc->REG_060 = (psc->REG_060 & ~(PSC_AON_CODEC_RESERVED(1 << 2) |
        PSC_AON_CODEC_RESERVED(1 << 3)));
}

BOOT_TEXT_FLASH_LOC
void hal_psc_jtag_select_sens(void)
{
    psc->REG_060 = (psc->REG_060 & ~PSC_AON_CODEC_RESERVED(1 << 2)) |
        PSC_AON_CODEC_RESERVED(1 << 3);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_jtag_select_mcu(void)
{
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
    hal_psc_jtag_select_bth();
#else
    hal_psc_jtag_select_sys_m55();
#endif
}

static inline uint32_t gen_ram_mask(uint32_t start, uint32_t end)
{
    uint32_t idx, cnt;

    idx = start / SHR_RAM_BLK_SIZE;
    cnt = (end - start) / SHR_RAM_BLK_SIZE;
    return ((1 << cnt) - 1) << idx;
}

BOOT_TEXT_FLASH_LOC
uint32_t gen_m55_itcm_ram_mask(uint32_t ram_start, uint32_t ram_size)
{
    uint32_t start, end;

    start = ram_start - M55_MIN_ITCM_BASE;
    end = ram_start + ram_size - M55_MIN_ITCM_BASE;
    return gen_ram_mask(start, end);
}

BOOT_TEXT_FLASH_LOC
uint32_t gen_m55_dtcm_ram_mask(uint32_t ram_start, uint32_t ram_size)
{
    uint32_t start, end;
    uint32_t sys_ram_size = SYS_RAM2_BASE - SYS_RAM0_BASE;

    start = ram_start - M55_MIN_DTCM_BASE;
    end = ram_start + ram_size - M55_MIN_DTCM_BASE;
    return gen_ram_mask(sys_ram_size - end, sys_ram_size - start);
}

BOOT_TEXT_FLASH_LOC
uint32_t gen_hifi4_itcm_ram_mask(uint32_t ram_start, uint32_t ram_size)
{
    uint32_t start, end;

    start = ram_start - HIFI4_MIN_ITCM_BASE;
    end = ram_start + ram_size - HIFI4_MIN_ITCM_BASE;
    return gen_ram_mask(start, end);
}

BOOT_TEXT_FLASH_LOC
uint32_t gen_hifi4_dtcm_ram_mask(uint32_t ram_start, uint32_t ram_size)
{
    uint32_t start, end;
    uint32_t sys_ram_size = SYS_RAM2_BASE - SYS_RAM0_BASE;

    start = ram_start - HIFI4_MIN_DTCM_BASE;
    end = ram_start + ram_size - HIFI4_MIN_DTCM_BASE;
    return gen_ram_mask(sys_ram_size - end, sys_ram_size - start);
}

BOOT_TEXT_FLASH_LOC
uint32_t gen_sys_ram_mask(uint32_t ram_start, uint32_t ram_size)
{
    uint32_t start, end;

    if (ram_start >= SYS_RAM2_BASE) {
        return 0;
    }
    start = ram_start - SYS_RAM0_BASE;
    if (ram_start + ram_size < SYS_RAM2_BASE) {
        end = ram_start + ram_size - SYS_RAM0_BASE;
    } else {
        end = SYS_RAM2_BASE - SYS_RAM0_BASE;
    }
    return gen_ram_mask(start, end);
}

BOOT_TEXT_FLASH_LOC
uint32_t hal_psc_sys_get_shared_ram_mask(uint32_t base, uint32_t size)
{
    uint32_t start, end, idx, cnt;
    uint32_t mask;

    if (base <= SYS_RAM2_BASE) {
        start = 0;
    } else {
        start = base - SYS_RAM2_BASE;
    }

    if (base + size <= SYS_RAM2_BASE) {
        end = 0;
    } else if (base + size <= SYS_RAM_END) {
        end = base + size - SYS_RAM2_BASE;
    } else {
        end = SYS_RAM_END - SYS_RAM2_BASE;
    }

    idx = start / SHR_RAM_BLK_SIZE;
    cnt = (end - start) / SHR_RAM_BLK_SIZE;
    mask = ((1 << cnt) - 1) << idx;
    if ((base <= SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 2) && (SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 2 < base + size) &&
            (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM2_BASE + SHR_RAM_BLK_SIZE * 2)) {
        mask = (mask & ~(1 << 2)) | (1 << 10);
    }
    if ((base <= SYS_RAM2_BASE + SHR_RAM_BLK_SIZE) && (SYS_RAM2_BASE + SHR_RAM_BLK_SIZE < base + size) &&
            (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM2_BASE + SHR_RAM_BLK_SIZE)) {
        mask = (mask & ~(1 << 1)) | (1 << 9);
    }
    if ((base <= SYS_RAM2_BASE) && (SYS_RAM2_BASE < base + size) &&
            (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM2_BASE)) {
        mask = (mask & ~(1 << 0)) | (1 << 8);
    }

    return mask;
}

BOOT_TEXT_FLASH_LOC
uint32_t gen_sens_shr_ram_mask(uint32_t base, uint32_t size)
{
    uint32_t end, idx, cnt;
    uint32_t mask, start;

    if (base < SENS_RAM4_BASE) {
        start = base - SENS_RAM0_BASE;
        if (base + size < SENS_RAM4_BASE) {
            end = base + size - SENS_RAM0_BASE;
        } else {
            end = SENS_RAM4_BASE - SENS_RAM0_BASE;
        }
    } else {
        start = 0;
        end = 0;
    }

    idx = start / SHR_RAM_BLK_SIZE + 5;
    cnt = (end - start) / SHR_RAM_BLK_SIZE;
    mask = ((1 << cnt) - 1) << idx;

    return mask;
}

BOOT_TEXT_FLASH_LOC
uint32_t gen_bth_shr_ram_mask(uint32_t base, uint32_t size)
{
    uint32_t start, end, idx, cnt;
    uint32_t mask;

    if (base <= BTH_RAM2_BASE) {
        start = 0;
    } else {
        start = base - BTH_RAM2_BASE;
    }

    if (base + size <= BTH_RAM2_BASE) {
        end = 0;
    } else {
        end = base + size - BTH_RAM2_BASE;
    }

    idx = start / SHR_RAM_BLK_SIZE;
    cnt = (end - start) / SHR_RAM_BLK_SIZE;
    mask = ((1 << cnt) - 1) << idx;

    return mask;
}

BOOT_TEXT_FLASH_LOC
uint32_t hal_cmu_btc_get_shared_ram_mask(uint32_t base, uint32_t size)
{
    uint32_t start, end, idx, cnt;
    uint32_t mask;
    uint32_t btc_shr_end = BTC_MIN_ROM_RAM_BASE + SHR_RAM_BLK_SIZE * 10;

    if (base <= BTC_MIN_ROM_RAM_BASE) {
        start = 0;
    } else {
        start = base - BTC_MIN_ROM_RAM_BASE;
    }

    if (base + size <= BTC_MIN_ROM_RAM_BASE) {
        end = 0;
    } else if (base + size < btc_shr_end) {
        end = base + size - BTC_MIN_ROM_RAM_BASE;
    } else {
        end = btc_shr_end - BTC_MIN_ROM_RAM_BASE;
    }

    idx = start / SHR_RAM_BLK_SIZE + 1;
    cnt = (end - start) / SHR_RAM_BLK_SIZE;
    mask = ((1 << cnt) - 1) << idx;

    return mask;
}

static void BOOT_TEXT_FLASH_LOC hal_psc_init_ret_ram_cfg(void)
{
    uint32_t ram_map;

    // M55 ITCM
    if (M55_ITCM_SIZE) {
        m55_tcm_ram_map |= gen_m55_itcm_ram_mask(M55_ITCM_BASE, M55_ITCM_SIZE);
    }
    // M55 DTCM
    if (M55_DTCM_SIZE) {
        m55_tcm_ram_map |= gen_m55_dtcm_ram_mask(M55_DTCM_BASE, M55_DTCM_SIZE);
    }
    // M55 SYS RAM
    if (M55_SYS_RAM_SIZE && M55_SYS_RAM_BASE < SYS_RAM2_BASE) {
        m55_tcm_ram_map |= gen_sys_ram_mask(M55_SYS_RAM_BASE, M55_SYS_RAM_SIZE);
    }
    // M55 SHR RAM
    m55_shr_ram_map = hal_psc_sys_get_shared_ram_mask(M55_SYS_RAM_BASE, M55_SYS_RAM_SIZE);
    // DSP ITCM
    if (HIFI4_ITCM_SIZE) {
        dsp_tcm_ram_map |= gen_hifi4_itcm_ram_mask(HIFI4_ITCM_BASE, HIFI4_ITCM_SIZE);
    }
    // DSP DTCM
    if (HIFI4_DTCM_SIZE) {
        dsp_tcm_ram_map |= gen_hifi4_dtcm_ram_mask(HIFI4_DTCM_BASE, HIFI4_DTCM_SIZE);
    }
    // DSP SYS RAM
    if (HIFI4_SYS_RAM_SIZE && HIFI4_SYS_RAM_BASE < SYS_RAM2_BASE) {
        dsp_tcm_ram_map |= gen_sys_ram_mask(HIFI4_SYS_RAM_BASE, HIFI4_SYS_RAM_SIZE);
    }
    // DSP SHR RAM
    dsp_shr_ram_map = hal_psc_sys_get_shared_ram_mask(HIFI4_SYS_RAM_BASE, HIFI4_SYS_RAM_SIZE);

    // Init all the ram to pd with domain
    uint32_t val = 0x7FF;
    psc->REG_168 = (PSC_WRITE_ENABLE & 0xFF000000) | val;
    //psc->REG_16C = (PSC_WRITE_ENABLE & 0xFF000000) | val;
    //psc->REG_170 = (PSC_WRITE_ENABLE & 0xFF000000) | val;
    psc->REG_174 = PSC_AON_SYS_RETMEM0_PDEN | PSC_AON_SYS_RETMEM1_PDEN |
        PSC_AON_BTC_RETMEM_PDEN |PSC_AON_BTH_RETMEM_PDEN | PSC_AON_SES_RETMEM_PDEN;

    // Power down unused sys ram
    ram_map = 0xFFFF & ~(m55_tcm_ram_map | dsp_tcm_ram_map);
    hal_cmu_sys_ram_active_cfg(ram_map, true);
    hal_cmu_sys_ram_sleep_cfg(ram_map, true);

    // Power down unused shared ram
    ram_map = m55_shr_ram_map | dsp_shr_ram_map;
    ram_map |= gen_sens_shr_ram_mask(SENS_RAM_BASE, SENS_RAM_SIZE);
    ram_map |= gen_bth_shr_ram_mask(BTH_RAM_BASE, BTH_RAM_SIZE);
    ram_map |= hal_cmu_btc_get_shared_ram_mask(BTC_ROM_RAM_BASE, BTC_ROM_RAM_SIZE);
    ram_map = 0x7FF & ~ram_map;
    hal_psc_shr_ram_sleep_cfg(ram_map, HAL_PSC_SHR_RAM_FORCE_PD);
}

void BOOT_TEXT_FLASH_LOC hal_psc_init(void)
{
#if !defined(ARM_CMNS)
    uint32_t mcu_wake_mask;

    mcu_wake_mask =
        PSC_WAKEUP_INTR_MASK_GPIO |
        PSC_WAKEUP_INTR_MASK_INTPMU |
        PSC_WAKEUP_INTR_MASK_WDT |
        PSC_WAKEUP_INTR_MASK_EXTPMU |
        PSC_WAKEUP_INTR_MASK_CODEC |
        PSC_WAKEUP_INTR_MASK_SPI_WAKEUP |
        PSC_WAKEUP_INTR_MASK_USB_PIN;

    // Setup SYS interrupt mask
    psc->REG_0FC = 0;
    psc->REG_100 = PSC_WAKEUP_INTR_MASK_TIMER10 | PSC_WAKEUP_INTR_MASK_TIMER00 | PSC_WAKEUP_INTR_MASK_WAKEUP_SYS;

    // Setup BTC interrupt mask
    psc->REG_114 = 0;
    psc->REG_118 = PSC_WAKEUP_INTR_MASK_WAKEUP_BTC;

    // Setup BTH interrupt mask
    psc->REG_12C = 0;
    psc->REG_130 = PSC_WAKEUP_INTR_MASK_TIMER01 | PSC_WAKEUP_INTR_MASK_WAKEUP_BTH;

    // Setup SES interrupt mask
    psc->REG_144 = 0;
    psc->REG_148 = PSC_WAKEUP_INTR_MASK_TIMER11 | PSC_WAKEUP_INTR_MASK_VAD | PSC_WAKEUP_INTR_MASK_WAKEUP_SES;
#endif // !ARM_CMNS

    hal_psc_init_ret_ram_cfg();

#if !defined(ARM_CMNS)
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
    // This is BTH
    psc->REG_130 |= mcu_wake_mask;

    sys_mod_map = (1 << HAL_PSC_SYS_USER_M55) | (1 << HAL_PSC_SYS_USER_DSP);
#if defined(BTH_USE_SYS_PERIPH) || defined(BTH_CODEC_I2C_SLAVE)
    sys_mod_map |= (1 << HAL_PSC_SYS_USER_PERIPH);
#endif
    // Init sys to manual on
    psc->REG_018 = PSC_WRITE_ENABLE |
        PSC_AON_SYS_MEM_PSW_EN_DR |
        PSC_AON_SYS_PSW_EN_DR |
        PSC_AON_SYS_RESETN_ASSERT_DR |
        PSC_AON_SYS_ISO_EN_DR |
        PSC_AON_SYS_CLK_STOP_DR;

    hal_psc_sys_m55_disable();
#else
    // This is M55
    psc->REG_100 |= mcu_wake_mask;
#if !(defined(FLASH1_CTRL_BASE) && (!defined(FORCE_FLASH1_SLEEP) || defined(PROGRAMMER)))
    hal_psc_bth_disable();
#endif
#endif

    hal_psc_bt_disable();

    hal_psc_sys_dsp_disable();

#ifndef MCU_I2C_SLAVE
    // Power down sens
    hal_psc_sens_disable();
#endif

#ifndef PSRAM_ENABLE
    hal_psc_mem_disable();
#endif

    hal_psc_display_disable(HAL_PSC_DISPLAY_MOD_ALL);
    hal_psc_storage_disable(HAL_PSC_STORAGE_MOD_ALL);

    // Jtag select main mcu
    hal_psc_jtag_select_mcu();

    psc->REG_008 = PSC_AON_SYS_POWERDN_TIMER5(0);
    psc->REG_00C = PSC_AON_SYS_POWERUP_TIMER2(0);
    psc->REG_180 |= PSC_AON_SYS_PWRON_PRE_SEL |
        PSC_AON_SYS_PWRDN_PRE_SEL |
        PSC_AON_SES_PWRON_PRE_SEL |
        PSC_AON_SES_PWRDN_PRE_SEL;
#endif // !ARM_CMNS
}

uint32_t hal_psc_get_power_loop_cycle_cnt(void)
{
    return 14;
}

static inline void psc_reg_update_wait_us(uint32_t us)
{
    psc->REG_000;
    __DSB();
    if (us) {
        hal_sys_timer_delay_us(us);
    }
}

static inline void psc_reg_update_wait(uint32_t ticks)
{
    psc->REG_000;
    __DSB();
    if (ticks) {
        hal_sys_timer_delay(ticks);
    }
}

static inline void psc_reg_update_boot_entry_wait(uint32_t ms)
{
    psc->REG_000;
    __DSB();
    if (ms) {
        flash_hal_sys_timer_delay(__SLIM_MS_TO_TICKS(ms));
    }
}

static inline void psc_reg_update_os_wait(uint32_t ms)
{
    psc->REG_000;
    __DSB();
    if (ms) {
        osDelay(ms);
    }
}

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
#if defined(BTH_USE_SYS_PERIPH) && defined(CORE_SLEEP_POWER_DOWN)
BOOT_TEXT_SRAM_LOC
#else
BOOT_TEXT_FLASH_LOC
#endif
void hal_psc_sys_enable(enum HAL_PSC_SYS_USER_T user)
{
    uint32_t mem_cfg;
    uint32_t val;

    if (user > HAL_PSC_SYS_USER_QTY) {
        return;
    }
    if (user < HAL_PSC_SYS_USER_QTY) {
        if (sys_mod_map & (1 << user)) {
            return;
        }
        if (sys_mod_map & ((1 << HAL_PSC_SYS_USER_M55) | (1 << HAL_PSC_SYS_USER_DSP))) {
            sys_mod_map |= (1 << user);
            return;
        }
    }

    if (sys_mod_map & (1 << HAL_PSC_SYS_USER_PERIPH)) {
        psc->REG_018 = PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR |
            PSC_AON_SYS_PSW_EN_DR |
            PSC_AON_SYS_RESETN_ASSERT_DR |
            PSC_AON_SYS_ISO_EN_DR |
            PSC_AON_SYS_CLK_STOP_DR;
        psc_reg_update_wait(MS_TO_TICKS(1));
    } else {
        // sys_mod_map == 0

        val = psc->REG_018;
        if (user == HAL_PSC_SYS_USER_PERIPH) {
            mem_cfg = PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG;
        } else {
            mem_cfg = PSC_AON_SYS_MEM_PSW_EN_DR;

            if (val & PSC_AON_SYS_MEM_PSW_EN_REG) {
                psc->REG_018 = PSC_WRITE_ENABLE |
                    mem_cfg |
                    PSC_AON_SYS_PSW_EN_DR | PSC_AON_SYS_PSW_EN_REG |
                    PSC_AON_SYS_RESETN_ASSERT_DR | PSC_AON_SYS_RESETN_ASSERT_REG |
                    PSC_AON_SYS_ISO_EN_DR | PSC_AON_SYS_ISO_EN_REG |
                    PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
                psc_reg_update_wait(MS_TO_TICKS(1));
            }
        }

        if (val & PSC_AON_SYS_PSW_EN_REG) {
            psc->REG_018 = PSC_WRITE_ENABLE |
                mem_cfg |
                PSC_AON_SYS_PSW_EN_DR |
                PSC_AON_SYS_RESETN_ASSERT_DR | PSC_AON_SYS_RESETN_ASSERT_REG |
                PSC_AON_SYS_ISO_EN_DR | PSC_AON_SYS_ISO_EN_REG |
                PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
            psc_reg_update_wait(MS_TO_TICKS(1));
        }
        if (val & PSC_AON_SYS_RESETN_ASSERT_REG) {
            psc->REG_018 = PSC_WRITE_ENABLE |
                mem_cfg |
                PSC_AON_SYS_PSW_EN_DR |
                PSC_AON_SYS_RESETN_ASSERT_DR |
                PSC_AON_SYS_ISO_EN_DR | PSC_AON_SYS_ISO_EN_REG |
                PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
        }
        if (val & PSC_AON_SYS_ISO_EN_REG) {
            psc->REG_018 = PSC_WRITE_ENABLE |
                mem_cfg |
                PSC_AON_SYS_PSW_EN_DR |
                PSC_AON_SYS_RESETN_ASSERT_DR |
                PSC_AON_SYS_ISO_EN_DR |
                PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
        }
        if (val & PSC_AON_SYS_CLK_STOP_REG) {
            psc->REG_018 = PSC_WRITE_ENABLE |
                mem_cfg |
                PSC_AON_SYS_PSW_EN_DR |
                PSC_AON_SYS_RESETN_ASSERT_DR |
                PSC_AON_SYS_ISO_EN_DR |
                PSC_AON_SYS_CLK_STOP_DR;
        }
        psc_reg_update_wait(0);
    }

    if (user < HAL_PSC_SYS_USER_QTY) {
        sys_mod_map |= (1 << user);
    }
}

#if defined(BTH_USE_SYS_PERIPH) && defined(CORE_SLEEP_POWER_DOWN)
BOOT_TEXT_SRAM_LOC
#else
BOOT_TEXT_FLASH_LOC
#endif
void hal_psc_sys_disable(enum HAL_PSC_SYS_USER_T user)
{
    uint32_t val;

    if (user > HAL_PSC_SYS_USER_QTY) {
        return;
    }
    if (user < HAL_PSC_SYS_USER_QTY) {
        if ((sys_mod_map & (1 << user)) == 0) {
            return;
        }
        sys_mod_map &= ~(1 << user);
        if (sys_mod_map & ((1 << HAL_PSC_SYS_USER_M55) | (1 << HAL_PSC_SYS_USER_DSP))) {
            return;
        }
    }

    val = psc->REG_018;
    if (user < HAL_PSC_SYS_USER_QTY && sys_mod_map == (1 << HAL_PSC_SYS_USER_PERIPH)) {
        psc->REG_018 = val | PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG;
    } else {
        psc->REG_018 = val | PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR |
            PSC_AON_SYS_PSW_EN_DR |
            PSC_AON_SYS_RESETN_ASSERT_DR |
            PSC_AON_SYS_ISO_EN_DR |
            PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
        psc->REG_018 = val | PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR |
            PSC_AON_SYS_PSW_EN_DR |
            PSC_AON_SYS_RESETN_ASSERT_DR |
            PSC_AON_SYS_ISO_EN_DR | PSC_AON_SYS_ISO_EN_REG |
            PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
        psc->REG_018 = val | PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR |
            PSC_AON_SYS_PSW_EN_DR |
            PSC_AON_SYS_RESETN_ASSERT_DR | PSC_AON_SYS_RESETN_ASSERT_REG |
            PSC_AON_SYS_ISO_EN_DR | PSC_AON_SYS_ISO_EN_REG |
            PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
        psc->REG_018 = val | PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR |
            PSC_AON_SYS_PSW_EN_DR | PSC_AON_SYS_PSW_EN_REG |
            PSC_AON_SYS_RESETN_ASSERT_DR | PSC_AON_SYS_RESETN_ASSERT_REG |
            PSC_AON_SYS_ISO_EN_DR | PSC_AON_SYS_ISO_EN_REG |
            PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
        psc->REG_018 = val | PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG |
            PSC_AON_SYS_PSW_EN_DR | PSC_AON_SYS_PSW_EN_REG |
            PSC_AON_SYS_RESETN_ASSERT_DR | PSC_AON_SYS_RESETN_ASSERT_REG |
            PSC_AON_SYS_ISO_EN_DR | PSC_AON_SYS_ISO_EN_REG |
            PSC_AON_SYS_CLK_STOP_DR | PSC_AON_SYS_CLK_STOP_REG;
    }
}
#endif

void SRAM_TEXT_LOC hal_psc_sys_core_ram_power_down(enum HAL_PSC_RET_RAM_OWNER_T owner)
{
    psc->REG_018 |= PSC_WRITE_ENABLE | PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG;
}

void SRAM_TEXT_LOC hal_psc_sys_core_ram_power_up(enum HAL_PSC_RET_RAM_OWNER_T owner)
{
    uint32_t val;

    val = psc->REG_018;
    if (val & PSC_AON_SYS_MEM_PSW_EN_REG) {
        psc->REG_018 = (val & ~PSC_AON_SYS_MEM_PSW_EN_REG) | PSC_WRITE_ENABLE | PSC_AON_SYS_MEM_PSW_EN_DR;
        psc_reg_update_wait_us(30);
    }
}

void SRAM_TEXT_LOC hal_psc_sys_auto_power_down(int core_only)
{
    uint32_t val;
    bool mem_pd;

    val = psc->REG_018;
    if ((val & (PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG)) ==
            (PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG)) {
        val = PSC_WRITE_ENABLE | PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG;
        mem_pd = true;
    } else {
        val = PSC_WRITE_ENABLE | 0;
        mem_pd = false;
    }
    psc->REG_018 = val;

    if (core_only) {
        val = PSC_WRITE_ENABLE | 0;
    } else {
        val = PSC_WRITE_ENABLE |
            PSC_AON_SYS_PG_AUTO_EN |
            PSC_AON_SYS_PG_HW_EN |
#ifdef WAIT_SYS_BUS_IDLE
            PSC_AON_SYS_BUS_SLP_ENABLE |
#endif
            0;
    }
    if (mem_pd) {
        val |= PSC_AON_SYS_BYPASS_PWR_MEM;
    }
    psc->REG_000 = val;
}

void BOOT_TEXT_FLASH_LOC hal_psc_sys_m55_enable(void)
{
#ifndef PSC_M55_ALWAYS_ON
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
    hal_psc_sys_enable(HAL_PSC_SYS_USER_M55);
#endif
#endif

    hal_cmu_sys_ram_active_cfg(m55_tcm_ram_map, false);
    hal_cmu_sys_ram_sleep_cfg(m55_tcm_ram_map, true);
    hal_psc_shr_ram_sleep_cfg(m55_shr_ram_map, HAL_PSC_SHR_RAM_SLEEP_PD);

#ifndef PSC_M55_ALWAYS_ON
    uint32_t val;

    val = psc->REG_094;
    if (val & PSC_AON_SYSCPU0_MEM_PSW_EN_REG) {
        psc->REG_094 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU0_PSW_EN_DR | PSC_AON_SYSCPU0_PSW_EN_REG |
            PSC_AON_SYSCPU0_RESETN_ASSERT_DR | PSC_AON_SYSCPU0_RESETN_ASSERT_REG |
            PSC_AON_SYSCPU0_ISO_EN_DR | PSC_AON_SYSCPU0_ISO_EN_REG |
            PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_SYSCPU0_PSW_EN_REG) {
        psc->REG_094 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU0_PSW_EN_DR |
            PSC_AON_SYSCPU0_RESETN_ASSERT_DR | PSC_AON_SYSCPU0_RESETN_ASSERT_REG |
            PSC_AON_SYSCPU0_ISO_EN_DR | PSC_AON_SYSCPU0_ISO_EN_REG |
            PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_SYSCPU0_RESETN_ASSERT_REG) {
        psc->REG_094 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU0_PSW_EN_DR |
            PSC_AON_SYSCPU0_RESETN_ASSERT_DR |
            PSC_AON_SYSCPU0_ISO_EN_DR | PSC_AON_SYSCPU0_ISO_EN_REG |
            PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
    }
    if (val & PSC_AON_SYSCPU0_ISO_EN_REG) {
        psc->REG_094 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU0_PSW_EN_DR |
            PSC_AON_SYSCPU0_RESETN_ASSERT_DR |
            PSC_AON_SYSCPU0_ISO_EN_DR |
            PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
    }
    if (val & PSC_AON_SYSCPU0_CLK_STOP_REG) {
        psc->REG_094 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU0_PSW_EN_DR |
            PSC_AON_SYSCPU0_RESETN_ASSERT_DR |
            PSC_AON_SYSCPU0_ISO_EN_DR |
            PSC_AON_SYSCPU0_CLK_STOP_DR;
    }
    psc_reg_update_wait(0);
#endif

#ifdef JTAG_M55
#ifdef JTAG_DSP
    hal_psc_jtag_select_sys_dual();
#else
    hal_psc_jtag_select_sys_m55();
#endif
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_sys_m55_disable(void)
{
#ifdef JTAG_M55
    hal_psc_jtag_select_mcu();
#endif

#ifndef PSC_M55_ALWAYS_ON
    uint32_t val;

    val = psc->REG_094;
    psc->REG_094 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU0_PSW_EN_DR |
        PSC_AON_SYSCPU0_RESETN_ASSERT_DR |
        PSC_AON_SYSCPU0_ISO_EN_DR |
        PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
    psc->REG_094 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU0_PSW_EN_DR |
        PSC_AON_SYSCPU0_RESETN_ASSERT_DR |
        PSC_AON_SYSCPU0_ISO_EN_DR | PSC_AON_SYSCPU0_ISO_EN_REG |
        PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
    psc->REG_094 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU0_PSW_EN_DR |
        PSC_AON_SYSCPU0_RESETN_ASSERT_DR | PSC_AON_SYSCPU0_RESETN_ASSERT_REG |
        PSC_AON_SYSCPU0_ISO_EN_DR | PSC_AON_SYSCPU0_ISO_EN_REG |
        PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
    psc->REG_094 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU0_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU0_PSW_EN_DR | PSC_AON_SYSCPU0_PSW_EN_REG |
        PSC_AON_SYSCPU0_RESETN_ASSERT_DR | PSC_AON_SYSCPU0_RESETN_ASSERT_REG |
        PSC_AON_SYSCPU0_ISO_EN_DR | PSC_AON_SYSCPU0_ISO_EN_REG |
        PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
    psc->REG_094 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU0_MEM_PSW_EN_DR | PSC_AON_SYSCPU0_MEM_PSW_EN_REG |
        PSC_AON_SYSCPU0_PSW_EN_DR | PSC_AON_SYSCPU0_PSW_EN_REG |
        PSC_AON_SYSCPU0_RESETN_ASSERT_DR | PSC_AON_SYSCPU0_RESETN_ASSERT_REG |
        PSC_AON_SYSCPU0_ISO_EN_DR | PSC_AON_SYSCPU0_ISO_EN_REG |
        PSC_AON_SYSCPU0_CLK_STOP_DR | PSC_AON_SYSCPU0_CLK_STOP_REG;
#endif

    hal_cmu_sys_ram_active_cfg(m55_tcm_ram_map, true);
    hal_cmu_sys_ram_sleep_cfg(m55_tcm_ram_map, true);
    hal_psc_shr_ram_sleep_cfg(m55_shr_ram_map, HAL_PSC_SHR_RAM_FORCE_PD);

#ifndef PSC_M55_ALWAYS_ON
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
    hal_psc_sys_disable(HAL_PSC_SYS_USER_M55);
#endif
#endif
}

void SRAM_TEXT_LOC hal_psc_sys_m55_auto_power_down(void)
{
    psc->REG_094 = PSC_WRITE_ENABLE | 0;
    while (GET_BITFIELD(psc->REG_088, PSC_AON_SYSCPU0_MAIN_STATE) != MAIN_STATE_IDLE) {}
    psc->REG_084 = PSC_WRITE_ENABLE | PSC_AON_SYSCPU0_PG_AUTO_EN;
    psc->REG_08C = PSC_WRITE_ENABLE | PSC_AON_SYSCPU0_POWERDN_START;
}

void BOOT_TEXT_FLASH_LOC hal_psc_sys_dsp_enable(void)
{
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
    hal_psc_sys_enable(HAL_PSC_SYS_USER_DSP);
#endif

    hal_cmu_sys_ram_active_cfg(dsp_tcm_ram_map, false);
    hal_cmu_sys_ram_sleep_cfg(dsp_tcm_ram_map, true);
    hal_psc_shr_ram_sleep_cfg(dsp_shr_ram_map, HAL_PSC_SHR_RAM_SLEEP_PD);

    uint32_t val;

    val = psc->REG_0A8;
    if (val & PSC_AON_SYSCPU1_MEM_PSW_EN_REG) {
        psc->REG_0A8 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU1_PSW_EN_DR | PSC_AON_SYSCPU1_PSW_EN_REG |
            PSC_AON_SYSCPU1_RESETN_ASSERT_DR | PSC_AON_SYSCPU1_RESETN_ASSERT_REG |
            PSC_AON_SYSCPU1_ISO_EN_DR | PSC_AON_SYSCPU1_ISO_EN_REG |
            PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_SYSCPU1_PSW_EN_REG) {
        psc->REG_0A8 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU1_PSW_EN_DR |
            PSC_AON_SYSCPU1_RESETN_ASSERT_DR | PSC_AON_SYSCPU1_RESETN_ASSERT_REG |
            PSC_AON_SYSCPU1_ISO_EN_DR | PSC_AON_SYSCPU1_ISO_EN_REG |
            PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_SYSCPU1_RESETN_ASSERT_REG) {
        psc->REG_0A8 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU1_PSW_EN_DR |
            PSC_AON_SYSCPU1_RESETN_ASSERT_DR |
            PSC_AON_SYSCPU1_ISO_EN_DR | PSC_AON_SYSCPU1_ISO_EN_REG |
            PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
    }
    if (val & PSC_AON_SYSCPU1_ISO_EN_REG) {
        psc->REG_0A8 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU1_PSW_EN_DR |
            PSC_AON_SYSCPU1_RESETN_ASSERT_DR |
            PSC_AON_SYSCPU1_ISO_EN_DR |
            PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
    }
    if (val & PSC_AON_SYSCPU1_CLK_STOP_REG) {
        psc->REG_0A8 = PSC_WRITE_ENABLE |
            PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
            PSC_AON_SYSCPU1_PSW_EN_DR |
            PSC_AON_SYSCPU1_RESETN_ASSERT_DR |
            PSC_AON_SYSCPU1_ISO_EN_DR |
            PSC_AON_SYSCPU1_CLK_STOP_DR;
    }
    psc_reg_update_wait(0);

#ifdef JTAG_DSP
#ifdef JTAG_M55
    hal_psc_jtag_select_sys_dual();
#else
    hal_psc_jtag_select_sys_dsp();
#endif
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_sys_dsp_disable(void)
{
#ifdef JTAG_DSP
    hal_psc_jtag_select_mcu();
#endif

    uint32_t val;

    val = psc->REG_0A8;
    psc->REG_0A8 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU1_PSW_EN_DR |
        PSC_AON_SYSCPU1_RESETN_ASSERT_DR |
        PSC_AON_SYSCPU1_ISO_EN_DR |
        PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
    psc->REG_0A8 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU1_PSW_EN_DR |
        PSC_AON_SYSCPU1_RESETN_ASSERT_DR |
        PSC_AON_SYSCPU1_ISO_EN_DR | PSC_AON_SYSCPU1_ISO_EN_REG |
        PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
    psc->REG_0A8 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU1_PSW_EN_DR |
        PSC_AON_SYSCPU1_RESETN_ASSERT_DR | PSC_AON_SYSCPU1_RESETN_ASSERT_REG |
        PSC_AON_SYSCPU1_ISO_EN_DR | PSC_AON_SYSCPU1_ISO_EN_REG |
        PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
    psc->REG_0A8 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU1_MEM_PSW_EN_DR |
        PSC_AON_SYSCPU1_PSW_EN_DR | PSC_AON_SYSCPU1_PSW_EN_REG |
        PSC_AON_SYSCPU1_RESETN_ASSERT_DR | PSC_AON_SYSCPU1_RESETN_ASSERT_REG |
        PSC_AON_SYSCPU1_ISO_EN_DR | PSC_AON_SYSCPU1_ISO_EN_REG |
        PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;
    psc->REG_0A8 = val | PSC_WRITE_ENABLE |
        PSC_AON_SYSCPU1_MEM_PSW_EN_DR | PSC_AON_SYSCPU1_MEM_PSW_EN_REG |
        PSC_AON_SYSCPU1_PSW_EN_DR | PSC_AON_SYSCPU1_PSW_EN_REG |
        PSC_AON_SYSCPU1_RESETN_ASSERT_DR | PSC_AON_SYSCPU1_RESETN_ASSERT_REG |
        PSC_AON_SYSCPU1_ISO_EN_DR | PSC_AON_SYSCPU1_ISO_EN_REG |
        PSC_AON_SYSCPU1_CLK_STOP_DR | PSC_AON_SYSCPU1_CLK_STOP_REG;

    hal_cmu_sys_ram_active_cfg(dsp_tcm_ram_map, true);
    hal_cmu_sys_ram_sleep_cfg(dsp_tcm_ram_map, true);
    hal_psc_shr_ram_sleep_cfg(dsp_shr_ram_map, HAL_PSC_SHR_RAM_FORCE_PD);

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
    hal_psc_sys_disable(HAL_PSC_SYS_USER_DSP);
#endif
}

void SRAM_TEXT_LOC hal_psc_sys_dsp_auto_power_down(void)
{
    psc->REG_0A8 = PSC_WRITE_ENABLE | 0;
    while (GET_BITFIELD(psc->REG_09C, PSC_AON_SYSCPU1_MAIN_STATE) != MAIN_STATE_IDLE) {}
    psc->REG_098 = PSC_WRITE_ENABLE | PSC_AON_SYSCPU1_PG_AUTO_EN;
    psc->REG_0A0 = PSC_WRITE_ENABLE | PSC_AON_SYSCPU1_POWERDN_START;
}

__STATIC_FORCEINLINE void __psc_bth_enable(int jtag_en)
{
    uint32_t val;

    val = psc->REG_044;
    if (val & PSC_AON_BTH_MEM_PSW_EN_REG) {
        psc->REG_044 = PSC_WRITE_ENABLE |
            PSC_AON_BTH_MEM_PSW_EN_DR |
            PSC_AON_BTH_PSW_EN_DR | PSC_AON_BTH_PSW_EN_REG |
            PSC_AON_BTH_RESETN_ASSERT_DR | PSC_AON_BTH_RESETN_ASSERT_REG |
            PSC_AON_BTH_ISO_EN_DR | PSC_AON_BTH_ISO_EN_REG |
            PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
        psc_reg_update_boot_entry_wait(1);
    }
    if (val & PSC_AON_BTH_PSW_EN_REG) {
        psc->REG_044 = PSC_WRITE_ENABLE |
            PSC_AON_BTH_MEM_PSW_EN_DR |
            PSC_AON_BTH_PSW_EN_DR |
            PSC_AON_BTH_RESETN_ASSERT_DR | PSC_AON_BTH_RESETN_ASSERT_REG |
            PSC_AON_BTH_ISO_EN_DR | PSC_AON_BTH_ISO_EN_REG |
            PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
        psc_reg_update_boot_entry_wait(1);
    }
    if (val & PSC_AON_BTH_RESETN_ASSERT_REG) {
        psc->REG_044 = PSC_WRITE_ENABLE |
            PSC_AON_BTH_MEM_PSW_EN_DR |
            PSC_AON_BTH_PSW_EN_DR |
            PSC_AON_BTH_RESETN_ASSERT_DR |
            PSC_AON_BTH_ISO_EN_DR | PSC_AON_BTH_ISO_EN_REG |
            PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
    }
    if (val & PSC_AON_BTH_ISO_EN_REG) {
        psc->REG_044 = PSC_WRITE_ENABLE |
            PSC_AON_BTH_MEM_PSW_EN_DR |
            PSC_AON_BTH_PSW_EN_DR |
            PSC_AON_BTH_RESETN_ASSERT_DR |
            PSC_AON_BTH_ISO_EN_DR |
            PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
    }
    if (val & PSC_AON_BTH_CLK_STOP_REG) {
        psc->REG_044 = PSC_WRITE_ENABLE |
            PSC_AON_BTH_MEM_PSW_EN_DR |
            PSC_AON_BTH_PSW_EN_DR |
            PSC_AON_BTH_RESETN_ASSERT_DR |
            PSC_AON_BTH_ISO_EN_DR |
            PSC_AON_BTH_CLK_STOP_DR;
    }

#ifdef JTAG_BTH
    if (jtag_en) {
        hal_psc_jtag_select_bth();
    }
#endif

    psc_reg_update_boot_entry_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_boot_entry_bth_enable(void)
{
    __psc_bth_enable(false);
}

void BOOT_TEXT_FLASH_LOC hal_psc_bth_enable(void)
{
    __psc_bth_enable(true);
}

void BOOT_TEXT_FLASH_LOC hal_psc_bth_disable(void)
{
#ifdef JTAG_BTH
    hal_psc_jtag_select_mcu();
#endif

    uint32_t val;

    val = psc->REG_044;
    psc->REG_044 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTH_MEM_PSW_EN_DR |
        PSC_AON_BTH_PSW_EN_DR |
        PSC_AON_BTH_RESETN_ASSERT_DR |
        PSC_AON_BTH_ISO_EN_DR |
        PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
    psc->REG_044 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTH_MEM_PSW_EN_DR |
        PSC_AON_BTH_PSW_EN_DR |
        PSC_AON_BTH_RESETN_ASSERT_DR |
        PSC_AON_BTH_ISO_EN_DR | PSC_AON_BTH_ISO_EN_REG |
        PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
    psc->REG_044 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTH_MEM_PSW_EN_DR |
        PSC_AON_BTH_PSW_EN_DR |
        PSC_AON_BTH_RESETN_ASSERT_DR | PSC_AON_BTH_RESETN_ASSERT_REG |
        PSC_AON_BTH_ISO_EN_DR | PSC_AON_BTH_ISO_EN_REG |
        PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
    psc->REG_044 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTH_MEM_PSW_EN_DR |
        PSC_AON_BTH_PSW_EN_DR | PSC_AON_BTH_PSW_EN_REG |
        PSC_AON_BTH_RESETN_ASSERT_DR | PSC_AON_BTH_RESETN_ASSERT_REG |
        PSC_AON_BTH_ISO_EN_DR | PSC_AON_BTH_ISO_EN_REG |
        PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
    psc->REG_044 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTH_MEM_PSW_EN_DR | PSC_AON_BTH_MEM_PSW_EN_REG |
        PSC_AON_BTH_PSW_EN_DR | PSC_AON_BTH_PSW_EN_REG |
        PSC_AON_BTH_RESETN_ASSERT_DR | PSC_AON_BTH_RESETN_ASSERT_REG |
        PSC_AON_BTH_ISO_EN_DR | PSC_AON_BTH_ISO_EN_REG |
        PSC_AON_BTH_CLK_STOP_DR | PSC_AON_BTH_CLK_STOP_REG;
}

void SRAM_TEXT_LOC hal_psc_bth_auto_power_down(void)
{
    psc->REG_044 = PSC_WRITE_ENABLE | 0;
    while (GET_BITFIELD(psc->REG_038, PSC_AON_BTH_MAIN_STATE) != MAIN_STATE_IDLE) {}
    psc->REG_034 = PSC_WRITE_ENABLE | PSC_AON_BTH_PG_AUTO_EN;
    psc->REG_03C = PSC_WRITE_ENABLE | PSC_AON_BTH_POWERDN_START;
}

void SRAM_TEXT_LOC hal_psc_core_auto_power_down(int core_only)
{
#ifdef CHIP_SUBSYS_SENS
    hal_psc_sens_auto_power_down(core_only);
#elif defined(CHIP_SUBSYS_BTH)
    hal_psc_bth_auto_power_down();
#elif defined(CHIP_SUBSYS_DSP)
    hal_psc_sys_dsp_auto_power_down();
#else
#ifdef ARM_CMNS
    TZ_hal_sleep_deep_sleep_S();
#else
    hal_psc_sys_auto_power_down(core_only);
    hal_psc_sys_m55_auto_power_down();
#endif
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_enable(void)
{
    uint32_t val;

    val = psc->REG_06C;
    if (val & PSC_AON_CODEC_MEM_PSW_EN_REG) {
        psc->REG_06C = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_MEM_PSW_EN_DR |
            PSC_AON_CODEC_PSW_EN_DR | PSC_AON_CODEC_PSW_EN_REG |
            PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_CODEC_PSW_EN_REG) {
        psc->REG_06C = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_MEM_PSW_EN_DR |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_CODEC_RESETN_ASSERT_REG) {
        psc->REG_06C = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_MEM_PSW_EN_DR |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    }
    if (val & PSC_AON_CODEC_ISO_EN_REG) {
        psc->REG_06C = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_MEM_PSW_EN_DR |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    }
    if (val & PSC_AON_CODEC_CLK_STOP_REG) {
        psc->REG_06C = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_MEM_PSW_EN_DR |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR |
            PSC_AON_CODEC_CLK_STOP_DR;
    }
    psc_reg_update_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_disable(void)
{
    uint32_t val;

    val = psc->REG_06C;
    psc->REG_06C = val | PSC_WRITE_ENABLE |
        PSC_AON_CODEC_MEM_PSW_EN_DR |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_06C = val | PSC_WRITE_ENABLE |
        PSC_AON_CODEC_MEM_PSW_EN_DR |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_06C = val | PSC_WRITE_ENABLE |
        PSC_AON_CODEC_MEM_PSW_EN_DR |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_06C = val | PSC_WRITE_ENABLE |
        PSC_AON_CODEC_MEM_PSW_EN_DR |
        PSC_AON_CODEC_PSW_EN_DR | PSC_AON_CODEC_PSW_EN_REG |
        PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_06C = val | PSC_WRITE_ENABLE |
        PSC_AON_CODEC_MEM_PSW_EN_DR | PSC_AON_CODEC_MEM_PSW_EN_REG |
        PSC_AON_CODEC_PSW_EN_DR | PSC_AON_CODEC_PSW_EN_REG |
        PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_enable(void)
{
    uint32_t val, iso_en_bit;

    hal_psc_btc_ram_ret_sleep();

    val = psc->REG_030;

    iso_en_bit = PSC_AON_BTC_ISO_EN_REG;
    //if (hal_cmu_get_aon_revision_id() < HAL_CHIP_METAL_ID_2)
    {
        val &= ~(PSC_AON_BTC_ISO_EN_REG);
        iso_en_bit = 0;
    }

    if (val & PSC_AON_BTC_MEM_PSW_EN_REG) {
        psc->REG_030 = PSC_WRITE_ENABLE |
            PSC_AON_BTC_MEM_PSW_EN_DR |
            PSC_AON_BTC_PSW_EN_DR | PSC_AON_BTC_PSW_EN_REG |
            PSC_AON_BTC_RESETN_ASSERT_DR | PSC_AON_BTC_RESETN_ASSERT_REG |
            PSC_AON_BTC_ISO_EN_DR | iso_en_bit |
            PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_BTC_PSW_EN_REG) {
        psc->REG_030 = PSC_WRITE_ENABLE |
            PSC_AON_BTC_MEM_PSW_EN_DR |
            PSC_AON_BTC_PSW_EN_DR |
            PSC_AON_BTC_RESETN_ASSERT_DR | PSC_AON_BTC_RESETN_ASSERT_REG |
            PSC_AON_BTC_ISO_EN_DR | iso_en_bit |
            PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_BTC_RESETN_ASSERT_REG) {
        psc->REG_030 = PSC_WRITE_ENABLE |
            PSC_AON_BTC_MEM_PSW_EN_DR |
            PSC_AON_BTC_PSW_EN_DR |
            PSC_AON_BTC_RESETN_ASSERT_DR |
            PSC_AON_BTC_ISO_EN_DR | iso_en_bit |
            PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
    }
    if (val & iso_en_bit) {
        psc->REG_030 = PSC_WRITE_ENABLE |
            PSC_AON_BTC_MEM_PSW_EN_DR |
            PSC_AON_BTC_PSW_EN_DR |
            PSC_AON_BTC_RESETN_ASSERT_DR |
            PSC_AON_BTC_ISO_EN_DR |
            PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
    }
    if (val & PSC_AON_BTC_CLK_STOP_REG) {
        psc->REG_030 = PSC_WRITE_ENABLE |
            PSC_AON_BTC_MEM_PSW_EN_DR |
            PSC_AON_BTC_PSW_EN_DR |
            PSC_AON_BTC_RESETN_ASSERT_DR |
            PSC_AON_BTC_ISO_EN_DR |
            PSC_AON_BTC_CLK_STOP_DR;
    }

#ifdef JTAG_BT
    hal_psc_jtag_select_bt();
#endif

    psc_reg_update_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_disable(void)
{
#ifdef JTAG_BT
    hal_psc_jtag_select_mcu();
#endif

    uint32_t val, iso_en_bit;

    hal_psc_btc_ram_pd_sleep();

    val = psc->REG_030;

    iso_en_bit = PSC_AON_BTC_ISO_EN_REG;
    //if (hal_cmu_get_aon_revision_id() < HAL_CHIP_METAL_ID_2)
    {
        val &= ~(PSC_AON_BTC_ISO_EN_REG);
        iso_en_bit = 0;
    }

    psc->REG_030 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTC_MEM_PSW_EN_DR |
        PSC_AON_BTC_PSW_EN_DR |
        PSC_AON_BTC_RESETN_ASSERT_DR |
        PSC_AON_BTC_ISO_EN_DR |
        PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
    psc->REG_030 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTC_MEM_PSW_EN_DR |
        PSC_AON_BTC_PSW_EN_DR |
        PSC_AON_BTC_RESETN_ASSERT_DR |
        PSC_AON_BTC_ISO_EN_DR | iso_en_bit |
        PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
    psc->REG_030 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTC_MEM_PSW_EN_DR |
        PSC_AON_BTC_PSW_EN_DR |
        PSC_AON_BTC_RESETN_ASSERT_DR | PSC_AON_BTC_RESETN_ASSERT_REG |
        PSC_AON_BTC_ISO_EN_DR | iso_en_bit |
        PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
    psc->REG_030 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTC_MEM_PSW_EN_DR |
        PSC_AON_BTC_PSW_EN_DR | PSC_AON_BTC_PSW_EN_REG |
        PSC_AON_BTC_RESETN_ASSERT_DR | PSC_AON_BTC_RESETN_ASSERT_REG |
        PSC_AON_BTC_ISO_EN_DR | iso_en_bit |
        PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
    psc->REG_030 = val | PSC_WRITE_ENABLE |
        PSC_AON_BTC_MEM_PSW_EN_DR | PSC_AON_BTC_MEM_PSW_EN_REG |
        PSC_AON_BTC_PSW_EN_DR | PSC_AON_BTC_PSW_EN_REG |
        PSC_AON_BTC_RESETN_ASSERT_DR | PSC_AON_BTC_RESETN_ASSERT_REG |
        PSC_AON_BTC_ISO_EN_DR | iso_en_bit |
        PSC_AON_BTC_CLK_STOP_DR | PSC_AON_BTC_CLK_STOP_REG;
}

void hal_psc_bt_enable_auto_power_down(void)
{
    psc->REG_020 = PSC_WRITE_ENABLE | PSC_AON_BTC_PG_AUTO_EN | PSC_AON_BTC_PG_HW_EN;
    psc->REG_030 = PSC_WRITE_ENABLE | 0;

    psc->REG_024 |= PSC_AON_BTC_SLEEP_NO_WFI;
}

void BOOT_TEXT_FLASH_LOC hal_psc_sens_sys_enable(void)
{
    uint32_t val;

    val = psc->REG_058;
    if (val & PSC_AON_SES_MEM_PSW_EN_REG) {
        psc->REG_058 = PSC_WRITE_ENABLE |
            PSC_AON_SES_MEM_PSW_EN_DR |
            PSC_AON_SES_PSW_EN_DR | PSC_AON_SES_PSW_EN_REG |
            PSC_AON_SES_RESETN_ASSERT_DR | PSC_AON_SES_RESETN_ASSERT_REG |
            PSC_AON_SES_ISO_EN_DR | PSC_AON_SES_ISO_EN_REG |
            PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_SES_PSW_EN_REG) {
        psc->REG_058 = PSC_WRITE_ENABLE |
            PSC_AON_SES_MEM_PSW_EN_DR |
            PSC_AON_SES_PSW_EN_DR |
            PSC_AON_SES_RESETN_ASSERT_DR | PSC_AON_SES_RESETN_ASSERT_REG |
            PSC_AON_SES_ISO_EN_DR | PSC_AON_SES_ISO_EN_REG |
            PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_SES_RESETN_ASSERT_REG) {
        psc->REG_058 = PSC_WRITE_ENABLE |
            PSC_AON_SES_MEM_PSW_EN_DR |
            PSC_AON_SES_PSW_EN_DR |
            PSC_AON_SES_RESETN_ASSERT_DR |
            PSC_AON_SES_ISO_EN_DR | PSC_AON_SES_ISO_EN_REG |
            PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
    }
    if (val & PSC_AON_SES_ISO_EN_REG) {
        psc->REG_058 = PSC_WRITE_ENABLE |
            PSC_AON_SES_MEM_PSW_EN_DR |
            PSC_AON_SES_PSW_EN_DR |
            PSC_AON_SES_RESETN_ASSERT_DR |
            PSC_AON_SES_ISO_EN_DR |
            PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
    }
    if (val & PSC_AON_SES_CLK_STOP_REG) {
        psc->REG_058 = PSC_WRITE_ENABLE |
            PSC_AON_SES_MEM_PSW_EN_DR |
            PSC_AON_SES_PSW_EN_DR |
            PSC_AON_SES_RESETN_ASSERT_DR |
            PSC_AON_SES_ISO_EN_DR |
            PSC_AON_SES_CLK_STOP_DR;
    }
    psc_reg_update_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_sens_enable(void)
{
    hal_psc_sens_sys_enable();

    uint32_t val;

    val = psc->REG_080;
    if (val & PSC_AON_SESCPU_PSW_EN_REG) {
        psc->REG_080 = PSC_WRITE_ENABLE |
            PSC_AON_SESCPU_PSW_EN_DR |
            PSC_AON_SESCPU_RESETN_ASSERT_DR | PSC_AON_SESCPU_RESETN_ASSERT_REG |
            PSC_AON_SESCPU_ISO_EN_DR | PSC_AON_SESCPU_ISO_EN_REG |
            PSC_AON_SESCPU_CLK_STOP_DR | PSC_AON_SESCPU_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_SESCPU_RESETN_ASSERT_REG) {
        psc->REG_080 = PSC_WRITE_ENABLE |
            PSC_AON_SESCPU_PSW_EN_DR |
            PSC_AON_SESCPU_RESETN_ASSERT_DR |
            PSC_AON_SESCPU_ISO_EN_DR | PSC_AON_SESCPU_ISO_EN_REG |
            PSC_AON_SESCPU_CLK_STOP_DR | PSC_AON_SESCPU_CLK_STOP_REG;
    }
    if (val & PSC_AON_SESCPU_ISO_EN_REG) {
        psc->REG_080 = PSC_WRITE_ENABLE |
            PSC_AON_SESCPU_PSW_EN_DR |
            PSC_AON_SESCPU_RESETN_ASSERT_DR |
            PSC_AON_SESCPU_ISO_EN_DR |
            PSC_AON_SESCPU_CLK_STOP_DR | PSC_AON_SESCPU_CLK_STOP_REG;
    }
    if (val & PSC_AON_SESCPU_CLK_STOP_REG) {
        psc->REG_080 = PSC_WRITE_ENABLE |
            PSC_AON_SESCPU_PSW_EN_DR |
            PSC_AON_SESCPU_RESETN_ASSERT_DR |
            PSC_AON_SESCPU_ISO_EN_DR |
            PSC_AON_SESCPU_CLK_STOP_DR;
    }

#ifdef JTAG_SENS
    hal_psc_jtag_select_sens();
#endif

    psc_reg_update_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_sens_disable(void)
{
#ifdef JTAG_SENS
    hal_psc_jtag_select_mcu();
#endif

    uint32_t val;

    val = psc->REG_080;
    psc->REG_080 = val | PSC_WRITE_ENABLE |
        PSC_AON_SESCPU_PSW_EN_DR |
        PSC_AON_SESCPU_RESETN_ASSERT_DR |
        PSC_AON_SESCPU_ISO_EN_DR |
        PSC_AON_SESCPU_CLK_STOP_DR | PSC_AON_SESCPU_CLK_STOP_REG;
    psc->REG_080 = val | PSC_WRITE_ENABLE |
        PSC_AON_SESCPU_PSW_EN_DR |
        PSC_AON_SESCPU_RESETN_ASSERT_DR |
        PSC_AON_SESCPU_ISO_EN_DR | PSC_AON_SESCPU_ISO_EN_REG |
        PSC_AON_SESCPU_CLK_STOP_DR | PSC_AON_SESCPU_CLK_STOP_REG;
    psc->REG_080 = val | PSC_WRITE_ENABLE |
        PSC_AON_SESCPU_PSW_EN_DR |
        PSC_AON_SESCPU_RESETN_ASSERT_DR | PSC_AON_SESCPU_RESETN_ASSERT_REG |
        PSC_AON_SESCPU_ISO_EN_DR | PSC_AON_SESCPU_ISO_EN_REG |
        PSC_AON_SESCPU_CLK_STOP_DR | PSC_AON_SESCPU_CLK_STOP_REG;
    psc->REG_080 = val | PSC_WRITE_ENABLE |
        PSC_AON_SESCPU_PSW_EN_DR | PSC_AON_SESCPU_PSW_EN_REG |
        PSC_AON_SESCPU_RESETN_ASSERT_DR | PSC_AON_SESCPU_RESETN_ASSERT_REG |
        PSC_AON_SESCPU_ISO_EN_DR | PSC_AON_SESCPU_ISO_EN_REG |
        PSC_AON_SESCPU_CLK_STOP_DR | PSC_AON_SESCPU_CLK_STOP_REG;

    hal_psc_sens_sys_disable();
}

void BOOT_TEXT_FLASH_LOC hal_psc_sens_sys_disable(void)
{
    uint32_t val;

    val = psc->REG_058;
    psc->REG_058 = val | PSC_WRITE_ENABLE |
        PSC_AON_SES_MEM_PSW_EN_DR |
        PSC_AON_SES_PSW_EN_DR |
        PSC_AON_SES_RESETN_ASSERT_DR |
        PSC_AON_SES_ISO_EN_DR |
        PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
    psc->REG_058 = val | PSC_WRITE_ENABLE |
        PSC_AON_SES_MEM_PSW_EN_DR |
        PSC_AON_SES_PSW_EN_DR |
        PSC_AON_SES_RESETN_ASSERT_DR |
        PSC_AON_SES_ISO_EN_DR | PSC_AON_SES_ISO_EN_REG |
        PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
    psc->REG_058 = val | PSC_WRITE_ENABLE |
        PSC_AON_SES_MEM_PSW_EN_DR |
        PSC_AON_SES_PSW_EN_DR |
        PSC_AON_SES_RESETN_ASSERT_DR | PSC_AON_SES_RESETN_ASSERT_REG |
        PSC_AON_SES_ISO_EN_DR | PSC_AON_SES_ISO_EN_REG |
        PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
    psc->REG_058 = val | PSC_WRITE_ENABLE |
        PSC_AON_SES_MEM_PSW_EN_DR |
        PSC_AON_SES_PSW_EN_DR | PSC_AON_SES_PSW_EN_REG |
        PSC_AON_SES_RESETN_ASSERT_DR | PSC_AON_SES_RESETN_ASSERT_REG |
        PSC_AON_SES_ISO_EN_DR | PSC_AON_SES_ISO_EN_REG |
        PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
    psc->REG_058 = val | PSC_WRITE_ENABLE |
        PSC_AON_SES_MEM_PSW_EN_DR | PSC_AON_SES_MEM_PSW_EN_REG |
        PSC_AON_SES_PSW_EN_DR | PSC_AON_SES_PSW_EN_REG |
        PSC_AON_SES_RESETN_ASSERT_DR | PSC_AON_SES_RESETN_ASSERT_REG |
        PSC_AON_SES_ISO_EN_DR | PSC_AON_SES_ISO_EN_REG |
        PSC_AON_SES_CLK_STOP_DR | PSC_AON_SES_CLK_STOP_REG;
}

void SRAM_TEXT_LOC hal_psc_sens_auto_power_down(int core_only)
{
    psc->REG_058 = PSC_WRITE_ENABLE | 0;
    if (core_only) {
        psc->REG_048 = PSC_WRITE_ENABLE | 0;
    } else {
        psc->REG_048 = PSC_WRITE_ENABLE | PSC_AON_SES_PG_AUTO_EN | PSC_AON_SES_PG_HW_EN;
    }

    psc->REG_080 = PSC_WRITE_ENABLE | 0;
    while (GET_BITFIELD(psc->REG_074, PSC_AON_SESCPU_MAIN_STATE) != MAIN_STATE_IDLE) {}
    psc->REG_070 = PSC_WRITE_ENABLE | PSC_AON_SESCPU_PG_AUTO_EN;
    psc->REG_078 = PSC_WRITE_ENABLE | PSC_AON_SESCPU_POWERDN_START;
}

void BOOT_TEXT_FLASH_LOC hal_psc_display_enable(enum HAL_PSC_DISPLAY_MOD_T mod)
{
    uint32_t lock;
    bool set = true;
    uint32_t val;

    if (mod > HAL_PSC_DISPLAY_MOD_QTY) {
        return;
    }

    lock = int_lock();
    if (mod < HAL_PSC_DISPLAY_MOD_ALL) {
        if (dis_mod_map) {
            set = false;
        }
        dis_mod_map |= (1 << mod);
    }
    int_unlock(lock);

    if (!set) {
        while (psc->REG_0BC & PSC_AON_DIS_CLK_STOP_REG) {
            psc_reg_update_os_wait(1);
        }
        return;
    }

    val = psc->REG_0BC;
    if (val & PSC_AON_DIS_MEM_PSW_EN_REG) {
        psc->REG_0BC = PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR | PSC_AON_DIS_PSW_EN_REG |
            PSC_AON_DIS_RESETN_ASSERT_DR | PSC_AON_DIS_RESETN_ASSERT_REG |
            PSC_AON_DIS_ISO_EN_DR | PSC_AON_DIS_ISO_EN_REG |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
        psc_reg_update_os_wait(1);
    }
    if (val & PSC_AON_DIS_PSW_EN_REG) {
        psc->REG_0BC = PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR |
            PSC_AON_DIS_RESETN_ASSERT_DR | PSC_AON_DIS_RESETN_ASSERT_REG |
            PSC_AON_DIS_ISO_EN_DR | PSC_AON_DIS_ISO_EN_REG |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
        psc_reg_update_os_wait(1);
    }
    if (val & PSC_AON_DIS_RESETN_ASSERT_REG) {
        psc->REG_0BC = PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR |
            PSC_AON_DIS_RESETN_ASSERT_DR |
            PSC_AON_DIS_ISO_EN_DR | PSC_AON_DIS_ISO_EN_REG |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
    }
    if (val & PSC_AON_DIS_ISO_EN_REG) {
        psc->REG_0BC = PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR |
            PSC_AON_DIS_RESETN_ASSERT_DR |
            PSC_AON_DIS_ISO_EN_DR |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
    }
    if (val & PSC_AON_DIS_CLK_STOP_REG) {
        psc->REG_0BC = PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR |
            PSC_AON_DIS_RESETN_ASSERT_DR |
            PSC_AON_DIS_ISO_EN_DR |
            PSC_AON_DIS_CLK_STOP_DR;
    }
    psc_reg_update_os_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_display_disable(enum HAL_PSC_DISPLAY_MOD_T mod)
{
    uint32_t lock;
    bool set = true;

    if (mod > HAL_PSC_DISPLAY_MOD_QTY) {
        return;
    }

    lock = int_lock();

    if (mod < HAL_PSC_DISPLAY_MOD_ALL) {
        dis_mod_map &= ~(1 << mod);
        if (dis_mod_map) {
            set = false;
        }
    }

    if (set) {
        uint32_t val;

        val = psc->REG_0BC;
        psc->REG_0BC = val | PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR |
            PSC_AON_DIS_RESETN_ASSERT_DR |
            PSC_AON_DIS_ISO_EN_DR |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
        psc->REG_0BC = val | PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR |
            PSC_AON_DIS_RESETN_ASSERT_DR |
            PSC_AON_DIS_ISO_EN_DR | PSC_AON_DIS_ISO_EN_REG |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
        psc->REG_0BC = val | PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR |
            PSC_AON_DIS_RESETN_ASSERT_DR | PSC_AON_DIS_RESETN_ASSERT_REG |
            PSC_AON_DIS_ISO_EN_DR | PSC_AON_DIS_ISO_EN_REG |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
        psc->REG_0BC = val | PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR |
            PSC_AON_DIS_PSW_EN_DR | PSC_AON_DIS_PSW_EN_REG |
            PSC_AON_DIS_RESETN_ASSERT_DR | PSC_AON_DIS_RESETN_ASSERT_REG |
            PSC_AON_DIS_ISO_EN_DR | PSC_AON_DIS_ISO_EN_REG |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
        psc->REG_0BC = val | PSC_WRITE_ENABLE |
            PSC_AON_DIS_MEM_PSW_EN_DR | PSC_AON_DIS_MEM_PSW_EN_REG |
            PSC_AON_DIS_PSW_EN_DR | PSC_AON_DIS_PSW_EN_REG |
            PSC_AON_DIS_RESETN_ASSERT_DR | PSC_AON_DIS_RESETN_ASSERT_REG |
            PSC_AON_DIS_ISO_EN_DR | PSC_AON_DIS_ISO_EN_REG |
            PSC_AON_DIS_CLK_STOP_DR | PSC_AON_DIS_CLK_STOP_REG;
    }

    int_unlock(lock);
}

void BOOT_TEXT_FLASH_LOC hal_psc_mem_enable(void)
{
    uint32_t val;

    val = psc->REG_0D0;
    if (val & PSC_AON_PSRAM_PSW_EN_REG) {
        psc->REG_0D0 = PSC_WRITE_ENABLE |
            PSC_AON_PSRAM_PSW_EN_DR |
            PSC_AON_PSRAM_RESETN_ASSERT_DR | PSC_AON_PSRAM_RESETN_ASSERT_REG |
            PSC_AON_PSRAM_ISO_EN_DR | PSC_AON_PSRAM_ISO_EN_REG |
            PSC_AON_PSRAM_CLK_STOP_DR | PSC_AON_PSRAM_CLK_STOP_REG;
        psc_reg_update_wait(MS_TO_TICKS(1));
    }
    if (val & PSC_AON_PSRAM_RESETN_ASSERT_REG) {
        psc->REG_0D0 = PSC_WRITE_ENABLE |
            PSC_AON_PSRAM_PSW_EN_DR |
            PSC_AON_PSRAM_RESETN_ASSERT_DR |
            PSC_AON_PSRAM_ISO_EN_DR | PSC_AON_PSRAM_ISO_EN_REG |
            PSC_AON_PSRAM_CLK_STOP_DR | PSC_AON_PSRAM_CLK_STOP_REG;
    }
    if (val & PSC_AON_PSRAM_ISO_EN_REG) {
        psc->REG_0D0 = PSC_WRITE_ENABLE |
            PSC_AON_PSRAM_PSW_EN_DR |
            PSC_AON_PSRAM_RESETN_ASSERT_DR |
            PSC_AON_PSRAM_ISO_EN_DR |
            PSC_AON_PSRAM_CLK_STOP_DR | PSC_AON_PSRAM_CLK_STOP_REG;
    }
    if (val & PSC_AON_PSRAM_CLK_STOP_REG) {
        psc->REG_0D0 = PSC_WRITE_ENABLE |
            PSC_AON_PSRAM_PSW_EN_DR |
            PSC_AON_PSRAM_RESETN_ASSERT_DR |
            PSC_AON_PSRAM_ISO_EN_DR |
            PSC_AON_PSRAM_CLK_STOP_DR;
    }
    psc_reg_update_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_mem_disable(void)
{
    uint32_t val;

    val = psc->REG_0D0;
    psc->REG_0D0 = val | PSC_WRITE_ENABLE |
        PSC_AON_PSRAM_PSW_EN_DR |
        PSC_AON_PSRAM_RESETN_ASSERT_DR |
        PSC_AON_PSRAM_ISO_EN_DR |
        PSC_AON_PSRAM_CLK_STOP_DR | PSC_AON_PSRAM_CLK_STOP_REG;
    psc->REG_0D0 = val | PSC_WRITE_ENABLE |
        PSC_AON_PSRAM_PSW_EN_DR |
        PSC_AON_PSRAM_RESETN_ASSERT_DR |
        PSC_AON_PSRAM_ISO_EN_DR | PSC_AON_PSRAM_ISO_EN_REG |
        PSC_AON_PSRAM_CLK_STOP_DR | PSC_AON_PSRAM_CLK_STOP_REG;
    psc->REG_0D0 = val | PSC_WRITE_ENABLE |
        PSC_AON_PSRAM_PSW_EN_DR |
        PSC_AON_PSRAM_RESETN_ASSERT_DR | PSC_AON_PSRAM_RESETN_ASSERT_REG |
        PSC_AON_PSRAM_ISO_EN_DR | PSC_AON_PSRAM_ISO_EN_REG |
        PSC_AON_PSRAM_CLK_STOP_DR | PSC_AON_PSRAM_CLK_STOP_REG;
    psc->REG_0D0 = val | PSC_WRITE_ENABLE |
        PSC_AON_PSRAM_PSW_EN_DR | PSC_AON_PSRAM_PSW_EN_REG |
        PSC_AON_PSRAM_RESETN_ASSERT_DR | PSC_AON_PSRAM_RESETN_ASSERT_REG |
        PSC_AON_PSRAM_ISO_EN_DR | PSC_AON_PSRAM_ISO_EN_REG |
        PSC_AON_PSRAM_CLK_STOP_DR | PSC_AON_PSRAM_CLK_STOP_REG;
}

void BOOT_TEXT_FLASH_LOC hal_psc_storage_enable(enum HAL_PSC_STORAGE_MOD_T mod)
{
    uint32_t lock;
    bool set = true;
    uint32_t val;

    if (mod > HAL_PSC_STORAGE_MOD_QTY) {
        return;
    }

    lock = int_lock();
    if (mod < HAL_PSC_STORAGE_MOD_ALL) {
        if (store_mod_map) {
            set = false;
        }
        store_mod_map |= (1 << mod);
    }
    int_unlock(lock);

    if(mod == HAL_PSC_STORAGE_MOD_USB){
        val = psc->REG_018;
        if(val & PSC_AON_SYS_MEM_PSW_EN_REG){
            psc->REG_018 = PSC_WRITE_ENABLE |
                PSC_AON_SYS_MEM_PSW_EN_DR |
                PSC_AON_SYS_ISO_EN_DR |
                PSC_AON_SYS_RESETN_ASSERT_DR |
                PSC_AON_SYS_PSW_EN_DR |
                PSC_AON_SYS_CLK_STOP_DR;
            psc_reg_update_os_wait(1);
        }
    }

    if (!set) {
        while (psc->REG_0E4 & PSC_AON_STORE_CLK_STOP_REG) {
            psc_reg_update_os_wait(1);
        }
        return;
    }

    val = psc->REG_0E4;
    if (val & PSC_AON_STORE_MEM_PSW_EN_REG) {
        psc->REG_0E4 = PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR | PSC_AON_STORE_PSW_EN_REG |
            PSC_AON_STORE_RESETN_ASSERT_DR | PSC_AON_STORE_RESETN_ASSERT_REG |
            PSC_AON_STORE_ISO_EN_DR | PSC_AON_STORE_ISO_EN_REG |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
        psc_reg_update_os_wait(1);
    }
    if (val & PSC_AON_STORE_PSW_EN_REG) {
        psc->REG_0E4 = PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR |
            PSC_AON_STORE_RESETN_ASSERT_DR | PSC_AON_STORE_RESETN_ASSERT_REG |
            PSC_AON_STORE_ISO_EN_DR | PSC_AON_STORE_ISO_EN_REG |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
        psc_reg_update_os_wait(1);
    }
    if (val & PSC_AON_STORE_RESETN_ASSERT_REG) {
        psc->REG_0E4 = PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR |
            PSC_AON_STORE_RESETN_ASSERT_DR |
            PSC_AON_STORE_ISO_EN_DR | PSC_AON_STORE_ISO_EN_REG |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
    }
    if (val & PSC_AON_STORE_ISO_EN_REG) {
        psc->REG_0E4 = PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR |
            PSC_AON_STORE_RESETN_ASSERT_DR |
            PSC_AON_STORE_ISO_EN_DR |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
    }
    if (val & PSC_AON_STORE_CLK_STOP_REG) {
        psc->REG_0E4 = PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR |
            PSC_AON_STORE_RESETN_ASSERT_DR |
            PSC_AON_STORE_ISO_EN_DR |
            PSC_AON_STORE_CLK_STOP_DR;
    }
    psc_reg_update_os_wait(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_storage_disable(enum HAL_PSC_STORAGE_MOD_T mod)
{
    uint32_t lock;
    bool set = true;
    uint32_t val;

    if (mod > HAL_PSC_STORAGE_MOD_QTY) {
        return;
    }

    lock = int_lock();

    if (mod < HAL_PSC_STORAGE_MOD_ALL) {
        store_mod_map &= ~(1 << mod);
        if (store_mod_map) {
            set = false;
        }
    }

    if(mod == HAL_PSC_STORAGE_MOD_USB){
        val = psc->REG_018;
        psc->REG_018 = val | PSC_WRITE_ENABLE |
            PSC_AON_SYS_MEM_PSW_EN_DR | PSC_AON_SYS_MEM_PSW_EN_REG |
            PSC_AON_SYS_ISO_EN_DR |
            PSC_AON_SYS_RESETN_ASSERT_DR |
            PSC_AON_SYS_PSW_EN_DR |
            PSC_AON_SYS_CLK_STOP_DR;
        }

    if (set) {
        val = psc->REG_0E4;
        psc->REG_0E4 = val | PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR |
            PSC_AON_STORE_RESETN_ASSERT_DR |
            PSC_AON_STORE_ISO_EN_DR |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
        psc->REG_0E4 = val | PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR |
            PSC_AON_STORE_RESETN_ASSERT_DR |
            PSC_AON_STORE_ISO_EN_DR | PSC_AON_STORE_ISO_EN_REG |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
        psc->REG_0E4 = val | PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR |
            PSC_AON_STORE_RESETN_ASSERT_DR | PSC_AON_STORE_RESETN_ASSERT_REG |
            PSC_AON_STORE_ISO_EN_DR | PSC_AON_STORE_ISO_EN_REG |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
        psc->REG_0E4 = val | PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR |
            PSC_AON_STORE_PSW_EN_DR | PSC_AON_STORE_PSW_EN_REG |
            PSC_AON_STORE_RESETN_ASSERT_DR | PSC_AON_STORE_RESETN_ASSERT_REG |
            PSC_AON_STORE_ISO_EN_DR | PSC_AON_STORE_ISO_EN_REG |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
        psc->REG_0E4 = val | PSC_WRITE_ENABLE |
            PSC_AON_STORE_MEM_PSW_EN_DR | PSC_AON_STORE_MEM_PSW_EN_REG |
            PSC_AON_STORE_PSW_EN_DR | PSC_AON_STORE_PSW_EN_REG |
            PSC_AON_STORE_RESETN_ASSERT_DR | PSC_AON_STORE_RESETN_ASSERT_REG |
            PSC_AON_STORE_ISO_EN_DR | PSC_AON_STORE_ISO_EN_REG |
            PSC_AON_STORE_CLK_STOP_DR | PSC_AON_STORE_CLK_STOP_REG;
    }

    int_unlock(lock);
}

void hal_psc_m55_gpio_irq_enable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_P4_0) {
        psc->REG_0FC |= (1 << pin);
    }else if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_184 |= (1 << (pin - HAL_GPIO_PIN_P4_0));
    }
}

void hal_psc_m55_gpio_irq_disable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_P4_0) {
        psc->REG_0FC &= ~(1 << pin);
    }else if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_184 &= ~(1 << (pin - HAL_GPIO_PIN_P4_0));
    }
}

uint32_t hal_psc_m55_gpio_irq_get_status(uint32_t *status, uint32_t cnt)
{
    volatile uint32_t * const irq_status[] = {
        &psc->REG_108,
        &psc->REG_198,
    };
    const uint32_t irq_tab_sz = ARRAY_SIZE(irq_status);
    uint32_t i;

    if (cnt > irq_tab_sz) {
        cnt = irq_tab_sz;
    }
    for (i = 0; i < cnt; i++) {
        status[i] = *irq_status[i];
        if (i == (irq_tab_sz - 1))
            status[i] &= PSC_INTR_MASK2_GPIO1_15_0;
    }
    return cnt;
}

void hal_psc_bth_gpio_irq_enable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_P4_0) {
        psc->REG_12C |= (1 << pin);
    }else if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_18C |= (1 << (pin - HAL_GPIO_PIN_P4_0));
    }
}

void hal_psc_bth_gpio_irq_disable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_P4_0) {
        psc->REG_12C &= ~(1 << pin);
    }else if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_18C &= ~(1 << (pin - HAL_GPIO_PIN_P4_0));
    }
}

uint32_t hal_psc_bth_gpio_irq_get_status(uint32_t *status, uint32_t cnt)
{
    volatile uint32_t * const irq_status[] = {
        &psc->REG_138,
        &psc->REG_1A0,
    };
    const uint32_t irq_tab_sz = ARRAY_SIZE(irq_status);
    uint32_t i;

    if (cnt > irq_tab_sz) {
        cnt = irq_tab_sz;
    }
    for (i = 0; i < cnt; i++) {
        status[i] = *irq_status[i];
        if (i == (irq_tab_sz - 1))
            status[i] &= PSC_INTR_MASK2_GPIO1_15_0;
    }
    return cnt;
}

void hal_psc_sens_gpio_irq_enable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_P4_0) {
        psc->REG_144 |= (1 << pin);
    }else if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_190 |= (1 << (pin - HAL_GPIO_PIN_P4_0));
    }
}

void hal_psc_sens_gpio_irq_disable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_P4_0) {
        psc->REG_144 &= ~(1 << pin);
    }else if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_190 &= ~(1 << (pin - HAL_GPIO_PIN_P4_0));
    }
}

uint32_t hal_psc_sens_gpio_irq_get_status(uint32_t *status, uint32_t cnt)
{
    volatile uint32_t * const irq_status[] = {
        &psc->REG_150,
        &psc->REG_1A4,
    };
    const uint32_t irq_tab_sz = ARRAY_SIZE(irq_status);
    uint32_t i;

    if (cnt > irq_tab_sz) {
        cnt = irq_tab_sz;
    }
    for (i = 0; i < cnt; i++) {
        status[i] = *irq_status[i];
        if (i == (irq_tab_sz - 1))
            status[i] &= PSC_INTR_MASK2_GPIO1_15_0;
    }
    return cnt;
}

void hal_psc_gpio_irq_enable(enum HAL_GPIO_PIN_T pin)
{
#ifdef CHIP_SUBSYS_SENS
    hal_psc_sens_gpio_irq_enable(pin);
#elif defined(CHIP_SUBSYS_BTH)
    hal_psc_bth_gpio_irq_enable(pin);
#else
    hal_psc_m55_gpio_irq_enable(pin);
#endif
}

void hal_psc_gpio_irq_disable(enum HAL_GPIO_PIN_T pin)
{
#ifdef CHIP_SUBSYS_SENS
    hal_psc_sens_gpio_irq_disable(pin);
#elif defined(CHIP_SUBSYS_BTH)
    hal_psc_bth_gpio_irq_disable(pin);
#else
    hal_psc_m55_gpio_irq_disable(pin);
#endif
}

uint32_t hal_psc_gpio_irq_get_status(uint32_t *status, uint32_t cnt)
{
#ifdef CHIP_SUBSYS_SENS
    return hal_psc_sens_gpio_irq_get_status(status, cnt);
#elif defined(CHIP_SUBSYS_BTH)
    return hal_psc_bth_gpio_irq_get_status(status, cnt);
#else
    return hal_psc_m55_gpio_irq_get_status(status, cnt);
#endif
}

BOOT_TEXT_SRAM_LOC
void hal_psc_core_ram_sleep_pd(enum HAL_PSC_RET_RAM_OWNER_T owner)
{
    psc->REG_174 |= owner;

    if (owner & (HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1)) {
        uint32_t map = 0;
        if (owner & HAL_PSC_RET_RAM_SYS0) {
            map |= 0x00FF;
        }
        if (owner & HAL_PSC_RET_RAM_SYS1) {
            map |= 0xFF00;
        }
        hal_cmu_sys_ram_sleep_cfg(map, true);
    }
    if (owner & HAL_PSC_RET_RAM_BTC) {
        hal_cmu_btc_ram_sleep_cfg(true);
    }
    if (owner & HAL_PSC_RET_RAM_BTH) {
        hal_cmu_bth_ram_sleep_cfg(true);
    }
    if (owner & HAL_PSC_RET_RAM_SENS) {
        hal_cmu_sens_ram_sleep_cfg(true);
    }
}

SRAM_TEXT_LOC
void hal_psc_core_ram_sleep_pu(enum HAL_PSC_RET_RAM_OWNER_T owner)
{
    psc->REG_174 &= ~owner;

    if (owner & (HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1)) {
        // Allow to control the sleep state of each 128K block
    }
    if (owner & HAL_PSC_RET_RAM_BTC) {
        hal_cmu_btc_ram_sleep_cfg(false);
    }
    if (owner & HAL_PSC_RET_RAM_BTH) {
        hal_cmu_bth_ram_sleep_cfg(false);
    }
    if (owner & HAL_PSC_RET_RAM_SENS) {
        hal_cmu_sens_ram_sleep_cfg(false);
    }
}

BOOT_TEXT_SRAM_LOC
void hal_psc_shr_ram_sleep_cfg(uint32_t map, enum HAL_PSC_SHR_RAM_PD_CFG_T cfg)
{
    uint32_t val;
    uint32_t dr_mask;

    map &= 0x7FF;
    dr_mask = map << 11;

    val = psc->REG_168;
    if (cfg == HAL_PSC_SHR_RAM_FORCE_PU) {
        val = (val & ~map) | dr_mask;
    } else if (cfg == HAL_PSC_SHR_RAM_FORCE_PD) {
        val |= dr_mask | map;
    } else if (cfg == HAL_PSC_SHR_RAM_SLEEP_PU) {
        hal_cmu_shr_ram_sleep_cfg(map, false);
        val &= ~(dr_mask | map);
    } else {
        hal_cmu_shr_ram_sleep_cfg(map, true);
        val = (val & ~dr_mask) | map;
    }
    psc->REG_168 = (PSC_WRITE_ENABLE & 0xFF000000) | val;
    //psc->REG_16C = (PSC_WRITE_ENABLE & 0xFF000000) | val;
    //psc->REG_170 = (PSC_WRITE_ENABLE & 0xFF000000) | val;
}

uint32_t hal_psc_btc_dpd_ram_enable(uint32_t map)
{
    uint32_t val;
    uint32_t old_val;
    uint32_t dr_mask;

    map &= 0x7FF;
    dr_mask = map << 11;

    val = psc->REG_168;
    old_val = val;

    val &= ~(dr_mask | map);
    psc->REG_168 = (PSC_WRITE_ENABLE & 0xFF000000) | val;

    return old_val;
}

void hal_psc_btc_dpd_ram_disable(uint32_t sleep_cfg)
{
    psc->REG_168 = (PSC_WRITE_ENABLE & 0xFF000000) | sleep_cfg;
}

BOOT_TEXT_FLASH_LOC
void hal_psc_sens_init_ram_ret_mask(void)
{
    uint32_t mask;

    mask = gen_sens_shr_ram_mask(SENS_RAM_BASE, SENS_RAM_SIZE);
#if defined(CORE_SLEEP_POWER_DOWN) && defined(SENS_RAMRET_SIZE)
    sens_shr_pu = gen_sens_shr_ram_mask(SENS_RAMRET_BASE, SENS_RAMRET_SIZE);
#else
    sens_shr_pu = mask;
#endif
    sens_shr_pd = mask & ~sens_shr_pu;
}

SRAM_TEXT_LOC
void hal_psc_sens_ram_pd_sleep(void)
{
#if defined(SENS_RAMRET_SIZE) && \
        ((SENS_RAMRET_SIZE == 0) || (SENS_RAMRET_BASE + SENS_RAMRET_SIZE <= SENS_RAM4_BASE))
    hal_psc_core_ram_sleep_pd(HAL_PSC_RET_RAM_SENS);
#else
    hal_psc_core_ram_sleep_pu(HAL_PSC_RET_RAM_SENS);
#endif
    hal_psc_shr_ram_sleep_cfg(sens_shr_pu, HAL_PSC_SHR_RAM_SLEEP_PU);
    hal_psc_shr_ram_sleep_cfg(sens_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PD);
}

SRAM_TEXT_LOC
void hal_psc_sens_ram_ret_sleep(void)
{
    hal_psc_core_ram_sleep_pu(HAL_PSC_RET_RAM_SENS);
    hal_psc_shr_ram_sleep_cfg(sens_shr_pu | sens_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PU);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_bth_init_ram_ret_mask(void)
{
    uint32_t mask;

    mask = gen_bth_shr_ram_mask(BTH_RAM_BASE, BTH_RAM_SIZE);
#if defined(CORE_SLEEP_POWER_DOWN) && defined(BTH_RAMRET_SIZE)
    bth_shr_pu = gen_bth_shr_ram_mask(BTH_RAMRET_BASE, BTH_RAMRET_SIZE);
#else
    bth_shr_pu = mask;
#endif
    bth_shr_pd = mask & ~bth_shr_pu;
}

SRAM_TEXT_LOC
void hal_psc_bth_ram_pd_sleep(void)
{
#if defined(BTH_RAMRET_SIZE) && \
        ((BTH_RAMRET_SIZE == 0) || (BTH_RAMRET_BASE >= BTH_RAM2_BASE))
    hal_psc_core_ram_sleep_pd(HAL_PSC_RET_RAM_BTH);
#else
    hal_psc_core_ram_sleep_pu(HAL_PSC_RET_RAM_BTH);
#endif
    hal_psc_shr_ram_sleep_cfg(bth_shr_pu, HAL_PSC_SHR_RAM_SLEEP_PU);
    hal_psc_shr_ram_sleep_cfg(bth_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PD);
}

SRAM_TEXT_LOC
void hal_psc_bth_ram_ret_sleep(void)
{
    hal_psc_core_ram_sleep_pu(HAL_PSC_RET_RAM_BTH);
    hal_psc_shr_ram_sleep_cfg(bth_shr_pu | bth_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PU);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_hifi4_init_ram_ret_mask(void)
{
    uint32_t mask;
    uint32_t tcm_mask;

    tcm_mask = 0;
    tcm_mask |= gen_hifi4_itcm_ram_mask(HIFI4_ITCM_BASE, HIFI4_ITCM_SIZE);
    tcm_mask |= gen_hifi4_dtcm_ram_mask(HIFI4_DTCM_BASE, HIFI4_DTCM_SIZE);
    tcm_mask |= gen_sys_ram_mask(HIFI4_SYS_RAM_BASE, HIFI4_SYS_RAM_SIZE);
    mask = hal_psc_sys_get_shared_ram_mask(HIFI4_SYS_RAM_BASE, HIFI4_SYS_RAM_SIZE);

#if defined(CORE_SLEEP_POWER_DOWN) && \
        (defined(HIFI4_ITCMRET_SIZE) || defined(HIFI4_DTCMRET_SIZE) || defined(HIFI4_SYS_RAMRET_SIZE))
    dsp_tcm_pu = 0;
#ifdef HIFI4_ITCMRET_SIZE
    dsp_tcm_pu |= gen_hifi4_itcm_ram_mask(HIFI4_ITCMRET_BASE, HIFI4_ITCMRET_SIZE);
#endif
#ifdef HIFI4_DTCMRET_SIZE
    dsp_tcm_pu |= gen_hifi4_dtcm_ram_mask(HIFI4_DTCMRET_BASE, HIFI4_DTCMRET_SIZE);
#endif
#ifdef HIFI4_SYS_RAMRET_SIZE
    dsp_tcm_pu |= gen_hifi4_dsp_ram_mask(HIFI4_SYS_RAMRET_BASE, HIFI4_SYS_RAMRET_SIZE);
#else
    dsp_shr_pu = 0;
#endif
#else
    dsp_tcm_pu = tcm_mask;
    dsp_shr_pu = mask;
#endif
    dsp_tcm_pd = tcm_mask & ~dsp_tcm_pu;
    dsp_shr_pd = mask & ~dsp_shr_pu;
}

SRAM_TEXT_LOC
void hal_psc_hifi4_ram_pd_sleep(void)
{
    hal_psc_shr_ram_sleep_cfg(dsp_shr_pu, HAL_PSC_SHR_RAM_SLEEP_PU);
    hal_psc_shr_ram_sleep_cfg(dsp_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PD);

#ifdef NEVER_USE_M55
    enum HAL_PSC_RET_RAM_OWNER_T mask = HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1;
    enum HAL_PSC_RET_RAM_OWNER_T owner_pu = 0;
    enum HAL_PSC_RET_RAM_OWNER_T owner_pd = mask;

    if (dsp_tcm_pu & 0x00FF) {
        owner_pu |= HAL_PSC_RET_RAM_SYS0;
    }
    if (dsp_tcm_pu & 0xFF00) {
        owner_pu |= HAL_PSC_RET_RAM_SYS1;
    }
    owner_pd &= ~owner_pu;
    if (owner_pu) {
        hal_psc_core_ram_sleep_pu(owner_pu);
        hal_cmu_sys_ram_sleep_cfg(dsp_tcm_pu, false);
        hal_cmu_sys_ram_sleep_cfg(dsp_tcm_pd, true);
    }
    if (owner_pd) {
        hal_psc_core_ram_sleep_pd(owner_pd);
        hal_psc_sys_core_ram_power_down(owner_pd);
    }
#else
    hal_cmu_sys_ram_sleep_cfg(dsp_tcm_pu, false);
    hal_cmu_sys_ram_sleep_cfg(dsp_tcm_pd, true);
#endif
}

SRAM_TEXT_LOC
void hal_psc_hifi4_ram_pd_wakeup(void)
{
#ifdef NEVER_USE_M55
    hal_psc_sys_core_ram_power_up(HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1);
#endif
}

SRAM_TEXT_LOC
void hal_psc_hifi4_ram_ret_sleep(void)
{
#ifdef NEVER_USE_M55
    hal_psc_core_ram_sleep_pu(HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1);
#endif
    hal_cmu_sys_ram_sleep_cfg(dsp_tcm_pu | dsp_tcm_pd, false);
    hal_psc_shr_ram_sleep_cfg(dsp_shr_pu | dsp_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PU);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_m55_init_ram_ret_mask(void)
{
    uint32_t mask;
    uint32_t tcm_mask;

    tcm_mask = 0;
    tcm_mask |= gen_m55_itcm_ram_mask(M55_ITCM_BASE, M55_ITCM_SIZE);
    tcm_mask |= gen_m55_dtcm_ram_mask(M55_DTCM_BASE, M55_DTCM_SIZE);
    tcm_mask |= gen_sys_ram_mask(M55_SYS_RAM_BASE, M55_SYS_RAM_SIZE);
    mask = hal_psc_sys_get_shared_ram_mask(M55_SYS_RAM_BASE, M55_SYS_RAM_SIZE);

#if defined(CORE_SLEEP_POWER_DOWN) && \
        (defined(M55_ITCMRET_SIZE) || defined(M55_DTCMRET_SIZE) || defined(M55_SYS_RAMRET_SIZE))
    m55_tcm_pu = 0;
#ifdef M55_ITCMRET_SIZE
    m55_tcm_pu |= gen_m55_itcm_ram_mask(M55_ITCMRET_BASE, M55_ITCMRET_SIZE);
#endif
#ifdef M55_DTCMRET_SIZE
    m55_tcm_pu |= gen_m55_dtcm_ram_mask(M55_DTCMRET_BASE, M55_DTCMRET_SIZE);
#endif
#ifdef M55_SYS_RAMRET_SIZE
    m55_tcm_pu |= gen_sys_ram_mask(M55_SYS_RAMRET_BASE, M55_SYS_RAMRET_SIZE);
    m55_shr_pu = hal_psc_sys_get_shared_ram_mask(M55_SYS_RAMRET_BASE, M55_SYS_RAMRET_SIZE);
#else
    m55_shr_pu = 0;
#endif
#else
    m55_tcm_pu = tcm_mask;
    m55_shr_pu = mask;
#endif
    m55_tcm_pd = tcm_mask & ~m55_tcm_pu;
    m55_shr_pd = mask & ~m55_shr_pu;
}

SRAM_TEXT_LOC
void hal_psc_m55_ram_pd_sleep(void)
{
    hal_psc_shr_ram_sleep_cfg(m55_shr_pu, HAL_PSC_SHR_RAM_SLEEP_PU);
    hal_psc_shr_ram_sleep_cfg(m55_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PD);

#ifdef BTH_AS_MAIN_MCU
    // BTH is main MCU
    hal_cmu_sys_ram_sleep_cfg(m55_tcm_pu, false);
    hal_cmu_sys_ram_sleep_cfg(m55_tcm_pd, true);
#else
    // M55 is main MCU
    enum HAL_PSC_RET_RAM_OWNER_T mask = HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1;
    enum HAL_PSC_RET_RAM_OWNER_T owner_pu = 0;
    enum HAL_PSC_RET_RAM_OWNER_T owner_pd = mask;

    if (aoncmu->GBL_RESET_CLR & AON_CMU_SOFT_RSTN_MCUDSP_CLR) {
        owner_pu = mask;
    } else {
        if (m55_tcm_pu & 0x00FF) {
            owner_pu |= HAL_PSC_RET_RAM_SYS0;
        }
        if (m55_tcm_pu & 0xFF00) {
            owner_pu |= HAL_PSC_RET_RAM_SYS1;
        }
    }
    owner_pd &= ~owner_pu;
    if (owner_pu) {
        hal_psc_core_ram_sleep_pu(owner_pu);
        hal_cmu_sys_ram_sleep_cfg(m55_tcm_pu, false);
        hal_cmu_sys_ram_sleep_cfg(m55_tcm_pd, true);
    }
    if (owner_pd) {
        hal_psc_core_ram_sleep_pd(owner_pd);
        hal_psc_sys_core_ram_power_down(owner_pd);
    }
#endif
}

SRAM_TEXT_LOC
void hal_psc_m55_ram_pd_wakeup(void)
{
#ifndef BTH_AS_MAIN_MCU
    // M55 is main MCU
    hal_psc_sys_core_ram_power_up(HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1);
#endif
}

SRAM_TEXT_LOC
void hal_psc_m55_ram_ret_sleep(void)
{
    hal_psc_core_ram_sleep_pu(HAL_PSC_RET_RAM_SYS0 | HAL_PSC_RET_RAM_SYS1);
    hal_cmu_sys_ram_sleep_cfg(m55_tcm_pu | m55_tcm_pd, false);
    hal_psc_shr_ram_sleep_cfg(m55_shr_pu | m55_shr_pd, HAL_PSC_SHR_RAM_SLEEP_PU);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_core_init_ram_ret_mask(void)
{
#ifdef CHIP_SUBSYS_SENS
    hal_psc_sens_init_ram_ret_mask();
#elif defined(CHIP_SUBSYS_BTH)
    hal_psc_bth_init_ram_ret_mask();
#elif defined(CHIP_SUBSYS_DSP)
    hal_psc_hifi4_init_ram_ret_mask();
#else
    hal_psc_m55_init_ram_ret_mask();
#endif
}

SRAM_TEXT_LOC
void hal_psc_core_ram_pd_sleep(void)
{
    uint32_t lock;

    lock = iomux_lock();

#ifdef CHIP_SUBSYS_SENS
    hal_psc_sens_ram_pd_sleep();
#elif defined(CHIP_SUBSYS_BTH)
    hal_psc_bth_ram_pd_sleep();
#elif defined(CHIP_SUBSYS_DSP)
    hal_psc_hifi4_ram_pd_sleep();
#else
    hal_psc_m55_ram_pd_sleep();
#endif

    iomux_unlock(lock);
}

SRAM_TEXT_LOC
void hal_psc_core_ram_pd_wakeup(void)
{
    uint32_t lock;

    lock = iomux_lock();

#ifdef CHIP_SUBSYS_SENS
#elif defined(CHIP_SUBSYS_BTH)
#elif defined(CHIP_SUBSYS_DSP)
    hal_psc_hifi4_ram_pd_wakeup();
#else
    hal_psc_m55_ram_pd_wakeup();
#endif

    iomux_unlock(lock);
}

SRAM_TEXT_LOC
void hal_psc_core_ram_ret_sleep(void)
{
    uint32_t lock;

    lock = iomux_lock();

#ifdef CHIP_SUBSYS_SENS
    hal_psc_sens_ram_ret_sleep();
#elif defined(CHIP_SUBSYS_BTH)
    hal_psc_bth_ram_ret_sleep();
#elif defined(CHIP_SUBSYS_DSP)
    hal_psc_hifi4_ram_ret_sleep();
#else
    hal_psc_m55_ram_ret_sleep();
#endif

    iomux_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_btc_ram_pd_sleep(void)
{
    uint32_t mask;
    uint32_t lock;

    lock = iomux_lock();

    hal_psc_core_ram_sleep_pd(HAL_PSC_RET_RAM_BTC);

    mask = hal_cmu_btc_get_shared_ram_mask(BTC_ROM_RAM_BASE, BTC_ROM_RAM_SIZE);
    hal_psc_shr_ram_sleep_cfg(mask, HAL_PSC_SHR_RAM_SLEEP_PD);

    iomux_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_psc_btc_ram_ret_sleep(void)
{
    uint32_t mask;
    uint32_t lock;

    lock = iomux_lock();

    hal_psc_core_ram_sleep_pu(HAL_PSC_RET_RAM_BTC);

    mask = hal_cmu_btc_get_shared_ram_mask(BTC_ROM_RAM_BASE, BTC_ROM_RAM_SIZE);
    hal_psc_shr_ram_sleep_cfg(mask, HAL_PSC_SHR_RAM_SLEEP_PU);

    iomux_unlock(lock);
}
