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
#if !(defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH))

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_aoncmu)
#include CHIP_SPECIFIC_HDR(reg_aonsec)
#include CHIP_SPECIFIC_HDR(reg_btcmu)
#define CMU_HDR_CHECK_SUBSYS
#include CHIP_SPECIFIC_HDR(reg_bthcmu)
#include CHIP_SPECIFIC_HDR(reg_cmu)
#include CHIP_SPECIFIC_HDR(hal_cmu_pri)
#include CHIP_SPECIFIC_HDR(reg_senscmu)
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_bootmode.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "hal_codec.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_psc.h"
#ifdef ARM_CMNS
#include "hal_sleep.h"
#endif
#include "hal_sleep_core_pd.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sec.h"
#include "hal_cpudump.h"
#include "cmsis_nvic.h"
#include "pmu.h"

#if (HIFI4_ITCM_SIZE) || (HIFI4_DTCM_SIZE) || (HIFI4_SYS_RAM_SIZE)
#define HIFI4_USE_SYS_RAM
#endif

#ifdef CORE_SLEEP_POWER_DOWN
#define TIMER1_SEL_LOC                  BOOT_TEXT_SRAM_LOC
#else
#define TIMER1_SEL_LOC                  BOOT_TEXT_FLASH_LOC
#endif

#define CMU_RES3_EMMC_IRQ_S4_C19        (1 << 0)
#define CMU_RES3_FPU_IRQ_S19            (1 << 1)
#define CMU_RES3_SDMMC_IRQ_S4           (1 << 16)

void hal_cmu_osc_x4_enable_internal(void);
void hal_cmu_osc_x4_disable_internal(void);

enum CMU_DEBUG_REG_SEL_T {
    CMU_DEBUG_REG_SEL_MCU_PC        = 0,
    CMU_DEBUG_REG_SEL_MCU_LR        = 1,
    CMU_DEBUG_REG_SEL_MCU_SP        = 2,
    CMU_DEBUG_REG_SEL_CP_PC         = 3,
    CMU_DEBUG_REG_SEL_CP_LR         = 4,
    CMU_DEBUG_REG_SEL_CP_SP         = 5,
    CMU_DEBUG_REG_SEL_DEBUG         = 7,
};

static struct CMU_T * const cmu = (struct CMU_T *)SYS_CMU_BASE;

static struct AONCMU_T * const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

static struct AONSEC_T * const aonsec = (struct AONSEC_T *)AON_SEC_CTRL_BASE;

static struct BTCMU_T * const POSSIBLY_UNUSED btcmu = (struct BTCMU_T *)BT_CMU_BASE;

static struct BTHCMU_T * const POSSIBLY_UNUSED bthcmu = (struct BTHCMU_T *)BTH_CMU_BASE;

static struct SENSCMU_T * const POSSIBLY_UNUSED senscmu = (struct SENSCMU_T *)SENS_CMU_BASE;

static uint16_t BOOT_BSS_LOC pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(pll_user_map[0]) * 8, "Too many PLL users");

#ifndef ROM_BUILD
static enum HAL_CMU_PLL_T BOOT_DATA_LOC sys_pll_sel = HAL_CMU_PLL_QTY;
#endif

#ifdef RC48M_ENABLE
#ifdef BTH_AS_MAIN_MCU
const
#endif
static bool BOOT_DATA_LOC rc_enabled =
#ifdef BTH_AS_MAIN_MCU
    true;
#else
    false;
#endif
#endif

enum HAL_CMU_LCDC2D_USER_T {
    HAL_CMU_LCDC2D_USER_LCDC = 0,
    HAL_CMU_LCDC2D_USER_DMA2D,
};

static uint8_t lcdc2d_clk_map;

static uint32_t g_pixmhz_speed;

static HAL_CMU_BT_TRIGGER_HANDLER_T bt_trig_hdlr[HAL_CMU_BT_TRIGGER_SRC_QTY];

__STATIC_FORCEINLINE void aocmu_reg_update_wait(void)
{
    // Make sure AOCMU (26M clock domain) write opertions finish before return
    aoncmu->CHIP_ID;
    __DSB();
}

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_AON_MCU) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_ENABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_ENABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        cmu->OCLK_ENABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        cmu->QCLK_ENABLE = (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->XCLK_ENABLE = (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else {
        aoncmu->MOD_CLK_ENABLE = (1 << (id - HAL_CMU_AON_A_CMU));
    }

    if (id < HAL_CMU_AON_A_CMU) {
        cmu->HCLK_ENABLE;
        __DSB();
    } else {
        aocmu_reg_update_wait();
    }

    return 0;
}

int hal_cmu_clock_disable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_AON_MCU) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_DISABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_DISABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        cmu->OCLK_DISABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        cmu->QCLK_DISABLE = (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->XCLK_DISABLE = (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else {
        aoncmu->MOD_CLK_DISABLE = (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return 0;
}

enum HAL_CMU_CLK_STATUS_T hal_cmu_clock_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_AON_MCU) {
        return HAL_CMU_CLK_DISABLED;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HCLK_DISABLE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PCLK_DISABLE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        status = cmu->OCLK_DISABLE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        status = cmu->QCLK_DISABLE & (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = cmu->XCLK_DISABLE & (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else {
        status = aoncmu->MOD_CLK_DISABLE & (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return status ? HAL_CMU_CLK_ENABLED : HAL_CMU_CLK_DISABLED;
}

int hal_cmu_clock_set_mode(enum HAL_CMU_MOD_ID_T id, enum HAL_CMU_CLK_MODE_T mode)
{
    __IO uint32_t *reg;
    uint32_t val;
    uint32_t lock;

    if (id >= HAL_CMU_AON_MCU) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        reg = &cmu->HCLK_MODE;
        val = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        reg = &cmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        reg = &cmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        reg = &cmu->QCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        reg = &cmu->XCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else {
        reg = &aoncmu->MOD_CLK_MODE;
        val = (1 << (id - HAL_CMU_AON_A_CMU));
    }

    lock = int_lock();
    if (mode == HAL_CMU_CLK_MANUAL) {
        *reg |= val;
    } else {
        *reg &= ~val;
    }
    int_unlock(lock);

    return 0;
}

enum HAL_CMU_CLK_MODE_T hal_cmu_clock_get_mode(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t mode;

    if (id >= HAL_CMU_AON_MCU) {
        return HAL_CMU_CLK_AUTO;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        mode = cmu->HCLK_MODE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        mode = cmu->PCLK_MODE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        mode = cmu->OCLK_MODE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        mode = cmu->QCLK_MODE & (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        mode = cmu->XCLK_MODE & (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else {
        mode = aoncmu->MOD_CLK_MODE & (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return mode ? HAL_CMU_CLK_MANUAL : HAL_CMU_CLK_AUTO;
}

int hal_cmu_reset_set(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_SET = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_SET = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        cmu->ORESET_SET = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        cmu->QRESET_SET = (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->XRESET_SET = (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_SET = (1 << (id - HAL_CMU_AON_A_CMU));
    } else if (id < HAL_CMU_BTH_H_FLASH1) {
        if (id == HAL_CMU_MOD_GLOBAL) {
            hal_psc_ram_ret_during_reset();
        }
        aoncmu->GBL_RESET_SET = (1 << (id - HAL_CMU_AON_MCU));
    } else {
        if (id == HAL_CMU_BTH_H_FLASH1) {
            bthcmu->HRESET_SET = BTH_HRST_FLASH1;
        } else {
            bthcmu->ORESET_SET = BTH_ORST_FLASH1;
        }
    }

    return 0;
}

int hal_cmu_reset_clear(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_CLR = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_CLR = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        cmu->ORESET_CLR = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        cmu->QRESET_CLR = (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->XRESET_CLR = (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_CLR = (1 << (id - HAL_CMU_AON_A_CMU));
    } else if (id < HAL_CMU_BTH_H_FLASH1) {
        aoncmu->GBL_RESET_CLR = (1 << (id - HAL_CMU_AON_MCU));
    } else {
        if (id == HAL_CMU_BTH_H_FLASH1) {
            bthcmu->HRESET_CLR = BTH_HRST_FLASH1;
        } else {
            bthcmu->ORESET_CLR = BTH_ORST_FLASH1;
        }
    }

    if (id < HAL_CMU_AON_A_CMU) {
        cmu->HCLK_ENABLE;
        __DSB();
    } else if (id < HAL_CMU_BTH_H_FLASH1) {
        aocmu_reg_update_wait();
    } else {
        bthcmu->HCLK_ENABLE;
        __DSB();
    }

    return 0;
}

enum HAL_CMU_RST_STATUS_T hal_cmu_reset_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_RST_SET;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HRESET_SET & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PRESET_SET & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        status = cmu->ORESET_SET & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        status = cmu->QRESET_SET & (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = cmu->XRESET_SET & (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else if (id < HAL_CMU_AON_MCU) {
        status = aoncmu->RESET_SET & (1 << (id - HAL_CMU_AON_A_CMU));
    } else if (id < HAL_CMU_BTH_H_FLASH1) {
        status = aoncmu->GBL_RESET_SET & (1 << (id - HAL_CMU_AON_MCU));
    } else {
        if (id == HAL_CMU_BTH_H_FLASH1) {
            status = bthcmu->HRESET_SET & BTH_HRST_FLASH1;
        } else {
            status = bthcmu->ORESET_SET & BTH_ORST_FLASH1;
        }
    }

    return status ? HAL_CMU_RST_CLR : HAL_CMU_RST_SET;
}

int hal_cmu_reset_pulse(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (hal_cmu_reset_get_status(id) == HAL_CMU_RST_SET) {
        return hal_cmu_reset_clear(id);
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_PULSE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_PULSE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_BTH2SYS) {
        cmu->ORESET_PULSE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_MOD_X_CORE0) {
        cmu->QRESET_PULSE = (1 << (id - HAL_CMU_MOD_Q_BTH2SYS));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->XRESET_PULSE = (1 << (id - HAL_CMU_MOD_X_CORE0));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_PULSE = (1 << (id - HAL_CMU_AON_A_CMU));
    } else if (id < HAL_CMU_BTH_H_FLASH1) {
        aoncmu->GBL_RESET_PULSE = (1 << (id - HAL_CMU_AON_MCU));
    } else {
        if (id == HAL_CMU_BTH_H_FLASH1) {
            bthcmu->HRESET_PULSE = BTH_HRST_FLASH1;
        } else {
            bthcmu->ORESET_PULSE = BTH_ORST_FLASH1;
        }
    }

    __DSB();
    if (id < HAL_CMU_AON_A_CMU) {
        cmu->HCLK_ENABLE;
        cmu->HCLK_ENABLE;
        cmu->HCLK_ENABLE;
    } else if (id < HAL_CMU_BTH_H_FLASH1) {
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
    } else {
        bthcmu->HCLK_ENABLE;
        bthcmu->HCLK_ENABLE;
        bthcmu->HCLK_ENABLE;
    }
    __DSB();

    return 0;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_timer0_select_fast(void)
{
}

void BOOT_TEXT_FLASH_LOC hal_cmu_timer0_select_slow(void)
{
}

void TIMER1_SEL_LOC hal_cmu_timer1_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->PERIPH_CLK |= (1 << (CMU_SEL_TIMER_FAST_SHIFT + 0));
    int_unlock(lock);
}

void TIMER1_SEL_LOC hal_cmu_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << (CMU_SEL_TIMER_FAST_SHIFT + 0));
    int_unlock(lock);
}

void TIMER1_SEL_LOC hal_cmu_timer2_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->PERIPH_CLK |= (1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void TIMER1_SEL_LOC hal_cmu_timer2_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

int TIMER1_SEL_LOC hal_cmu_fast_timer_init(void)
{
    uint32_t val = 0;

#ifdef LOW_SYS_FREQ
#ifdef LOW_SYS_FREQ_3P25M
    // Fast timer div=16
    val = 2;
#else
    // Fast timer div=8
    val = 1;
#endif
#endif
    //aoncmu->CLK_OUT = SET_BITFIELD(aoncmu->CLK_OUT, AON_CMU_CFG_TIMER_FAST, val);
    cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_CFG_TIMER_FAST, val);

    return 0;
}

uint32_t BOOT_TEXT_SRAM_LOC hal_cmu_get_fast_timer_freq(void)
{
    uint32_t div = 4;

#ifdef LOW_SYS_FREQ
#ifdef LOW_SYS_FREQ_3P25M
    div = 16;
#else
    div = 8;
#endif
#endif
    return hal_cmu_get_crystal_freq() / div;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_sys_select_pll(enum HAL_CMU_PLL_T pll)
{
    int ret;

    ret = hal_cmu_axi_sys_select_pll(pll);
    if (ret) {
        return ret;
    }
#ifndef ROM_BUILD
    sys_pll_sel = pll;
#endif
    return ret;
}

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll)
{
    if (0) {
#ifndef ROM_BUILD
    } else if (pll == sys_pll_sel) {
        return !!(cmu->SYS_CLK_ENABLE & CMU_EN_PLL_ENABLE);
#endif
    } else if (pll == HAL_CMU_PLL_BB) {
        return !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLBB_ENABLE);
    } else if (pll == HAL_CMU_PLL_DSI) {
        return !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE);
    } else if (pll == HAL_CMU_PLL_USB) {
        return !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE);
    }

    return 0;
}

int hal_cmu_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t pu_val;
    uint32_t en_val;
    uint32_t en_val1;
    uint32_t lock;
    uint32_t start;
    uint32_t timeout;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    if (pll == HAL_CMU_PLL_BB) {
        pu_val = AON_CMU_PU_PLLBB_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
        en_val1 = AON_CMU_EN_CLK_PLLBB_MCU_ENABLE;
    } else if (pll == HAL_CMU_PLL_DSI) {
        pu_val = AON_CMU_PU_PLLDSI_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE;
        en_val1 = AON_CMU_EN_CLK_PLLDSI_MCU_ENABLE;
    } else {
        pu_val = AON_CMU_PU_PLLUSB_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
        en_val1 = AON_CMU_EN_CLK_PLLUSB_MCU_ENABLE;
    }

    lock = int_lock();
#ifdef RC48M_ENABLE
    if (rc_enabled) {
        uint32_t k;

        for (k = 0; k < ARRAY_SIZE(pll_user_map); k++) {
            if (pll_user_map[k]) {
                break;
            }
        }
        if (k >= ARRAY_SIZE(pll_user_map)) {
            // Enable OSC by pu OSCX2
            cmu->SYS_CLK_ENABLE = CMU_PU_OSCX2_ENABLE;
            hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
        }
    }
#endif
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
#ifndef ROM_BUILD
        if (pll == sys_pll_sel) {
            cmu->SYS_CLK_ENABLE = CMU_PU_PLL_ENABLE;
        } else
#endif
        {
            aoncmu->TOP_CLK_ENABLE = pu_val;
        }
        // Wait at least 10us for clock ready
        hal_sys_timer_delay_us(20);
    }
    if (user < HAL_CMU_PLL_USER_QTY) {
        pll_user_map[pll] |= (1 << user);
    }
    int_unlock(lock);

    start = hal_sys_timer_get();
    timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
    do {
#ifndef ROM_BUILD
        if (pll == sys_pll_sel) {
            if (cmu->SYS_CLK_ENABLE & CMU_EN_PLL_ENABLE) {
                break;
            }
        } else
#endif
        {
            if (aoncmu->TOP_CLK_ENABLE & en_val) {
                break;
            }
        }
    } while ((hal_sys_timer_get() - start) < timeout);

#ifndef ROM_BUILD
    if (pll == sys_pll_sel) {
        cmu->SYS_CLK_ENABLE = CMU_EN_PLL_ENABLE;
    } else
#endif
    {
        aoncmu->TOP_CLK_ENABLE = en_val;
    }
    aoncmu->TOP_CLK1_ENABLE = en_val1;

    return 0;
}

int hal_cmu_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t pu_val;
    uint32_t en_val;
    uint32_t en_val1;
    uint32_t lock;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    if (pll == HAL_CMU_PLL_BB) {
        pu_val = AON_CMU_PU_PLLBB_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_DISABLE;
        en_val1 = AON_CMU_EN_CLK_PLLBB_MCU_DISABLE;
    } else if (pll == HAL_CMU_PLL_DSI) {
        pu_val = AON_CMU_PU_PLLDSI_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDSI_DISABLE;
        en_val1 = AON_CMU_EN_CLK_PLLDSI_MCU_DISABLE;
    } else {
        pu_val = AON_CMU_PU_PLLUSB_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
        en_val1 = AON_CMU_EN_CLK_PLLUSB_MCU_DISABLE;
    }

    lock = int_lock();
    if (user < HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] &= ~(1 << user);
    }
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        aoncmu->TOP_CLK1_DISABLE = en_val1;
#ifndef ROM_BUILD
        if (pll == sys_pll_sel) {
            cmu->SYS_CLK_DISABLE = CMU_EN_PLL_DISABLE;
            cmu->SYS_CLK_DISABLE = CMU_PU_PLL_DISABLE;
        } else
#endif
        {
            aoncmu->TOP_CLK_DISABLE = en_val;
            aoncmu->TOP_CLK_DISABLE = pu_val;
        }
    }
#ifdef RC48M_ENABLE
    if (rc_enabled) {
        uint32_t k;

        for (k = 0; k < ARRAY_SIZE(pll_user_map); k++) {
            if (pll_user_map[k]) {
                break;
            }
        }
        if (k >= ARRAY_SIZE(pll_user_map)) {
            // Disable OSC by pd OSCX2
            cmu->SYS_CLK_DISABLE = CMU_PU_OSCX2_DISABLE;
            hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
        }
    }
#endif
    int_unlock(lock);

    return 0;
}

void hal_cmu_low_freq_mode_enable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
#ifndef ROM_BUILD
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T low_freq;

    pll = sys_pll_sel;
#ifdef OSC_26M_X4_AUD2BB
    low_freq = HAL_CMU_FREQ_96M;
#else
    low_freq = HAL_CMU_FREQ_48M;
#endif

    if (old_freq > low_freq && new_freq <= low_freq) {
        hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_SYS);
    }
#if defined(OSC_26M_X4_AUD2BB) && defined(RC48M_ENABLE)
    if (rc_enabled) {
        if (IS_X4_FREQ(old_freq) && !IS_X4_FREQ(new_freq)) {
            hal_cmu_osc_x4_disable_internal();
        }
    }
#endif
#endif
}

void hal_cmu_low_freq_mode_disable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
#ifndef ROM_BUILD
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T low_freq;

    pll = sys_pll_sel;
#ifdef OSC_26M_X4_AUD2BB
    low_freq = HAL_CMU_FREQ_96M;
#else
    low_freq = HAL_CMU_FREQ_48M;
#endif

    if (old_freq <= low_freq && new_freq > low_freq) {
        hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_SYS);
    }
#if defined(OSC_26M_X4_AUD2BB) && defined(RC48M_ENABLE)
    if (rc_enabled) {
        if (!IS_X4_FREQ(old_freq) && IS_X4_FREQ(new_freq)) {
            hal_cmu_osc_x4_enable_internal();
        }
    }
#endif
#endif
}

static void hal_cmu_disppre_hclk_enable(bool ena)
{
    static int hcount = 0;
    uint32_t lock;
    lock = int_lock();
    if (ena) {
        hcount += 1;
        if (hcount == 1) {
            hal_cmu_clock_enable(HAL_CMU_MOD_H_DISPPRE);
        }
    } else {
        hcount = hcount > 0 ? hcount - 1: 0;
    }
    if (hcount == 0) {
        hal_cmu_clock_disable(HAL_CMU_MOD_H_DISPPRE);
    }
    int_unlock(lock);
}

#define GPU_USE_PLL
void hal_cmu_gpu_clock_enable(void)
{
    #ifdef GPU_USE_PLL
    //while(0)
    {
    // CMU_CFG_DIV_GPU(value), valid value range is  0x0 ~ 0x3, gpu_clk = pll_clk/(value + 2)
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_GPU);
    cmu->UART_CLK |= CMU_SEL_PLL_GPU|CMU_CFG_DIV_GPU(0x0);    //200M
    //cmu->UART_CLK |= CMU_SEL_PLL_GPU|CMU_CFG_DIV_GPU(0x2);  //100M
    //cmu->UART_CLK |= CMU_SEL_PLL_GPU|CMU_CFG_DIV_GPU(0x3);   //80M
    }

    cmu->UART_CLK &=~(CMU_BYPASS_DIV_GPU);
    cmu->UART_CLK |=(CMU_RSTN_DIV_GPU);
    #else
    //cmu->UART_CLK |= CMU_SEL_SLOW_GPU; //24M
    //cmu->UART_CLK |= CMU_SEL_SLOW_GPU | CMU_SEL_OSC_2_GPU ; //12M
    cmu->UART_CLK |= CMU_SEL_FAST_GPU | CMU_SEL_OSCX2_GPU ; //48M
    #endif

    hal_cmu_disppre_hclk_enable(true);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_GPU);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_GPU);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_GPUX1);

    hal_cmu_reset_set(HAL_CMU_MOD_H_GPU);
    hal_sys_timer_delay_us(100);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_GPU);
    hal_cmu_reset_set(HAL_CMU_MOD_X_GPU);
    hal_sys_timer_delay_us(100);
    hal_cmu_reset_clear(HAL_CMU_MOD_X_GPU);

    hal_cmu_reset_set(HAL_CMU_MOD_O_GPUX1);
    hal_sys_timer_delay_us(200);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_GPUX1);
}

void hal_cmu_gpu_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_GPU);
    hal_cmu_reset_set(HAL_CMU_MOD_X_GPU);
    hal_cmu_reset_set(HAL_CMU_MOD_O_GPUX1);

    hal_cmu_disppre_hclk_enable(false);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_GPU);
    hal_cmu_clock_disable(HAL_CMU_MOD_X_GPU);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_GPUX1);
    hal_cmu_pll_disable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_GPU);
}

#define LCDC2D_USE_PLL_BB
#ifndef LCDC2D_USE_PLL_BB
#define LCDC2D_USE_OSCX1
//#define LCDC2D_USE_OSCX2
//#define LCDC2D_USE_OSCX4
#endif

static void hal_cmu_lcdc2d_mod_clock_enable(enum HAL_CMU_LCDC2D_USER_T user)
{
    uint8_t map = (1 << user);
    bool update = true;

    if (lcdc2d_clk_map) {
        update = false;
    }
    lcdc2d_clk_map |= map;

    if (!update) {
        return;
    }

    cmu->LCDC_CLK = (cmu->LCDC_CLK & ~(CMU_RSTN_DIV_LCDCP | CMU_SEL_PLL_LCDCP)) | CMU_SEL_OSCX4_LCDCP;

#ifdef LCDC2D_USE_PLL_BB
    uint32_t div = 0;
    uint32_t bbmhz = 440;
#if defined(BBPLL_MHZ)
    bbmhz = BBPLL_MHZ;
#endif
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_LCDC);

    if (g_pixmhz_speed != 0)
        div = (bbmhz / g_pixmhz_speed) - 2;

    cmu->I2S1_CLK = SET_BITFIELD(cmu->I2S1_CLK, CMU_CFG_DIV_PIX, div);

    cmu->DSI_CLK_ENABLE = CMU_SEL_PLL_PIX_ENABLE | CMU_RSTN_DIV_PIX_ENABLE;
    cmu->DSI_CLK_DISABLE = CMU_BYPASS_DIV_PIX_DISABLE;

#else // use OSC CLK
    uint32_t enable = 0;
    uint32_t disable = 0;

    disable |= CMU_SEL_PLL_PIX_DISABLE | CMU_RSTN_DIV_PIX_DISABLE;
#ifdef LCDC2D_USE_OSCX4
    enable |= CMU_SEL_OSCX4_PIX_ENABLE;
#elif defined(LCDC2D_USE_OSCX2)
    disable |= CMU_SEL_OSCX4_PIX_DISABLE;
    enable |= CMU_SEL_OSCX2_PIX_ENABLE;
#elif defined(LCDC2D_USE_OSCX1)
    disable |= CMU_SEL_OSCX4_PIX_DISABLE | CMU_SEL_OSCX2_PIX_DISABLE;
#endif

    cmu->DSI_CLK_DISABLE = disable;
    cmu->DSI_CLK_ENABLE = enable;
#endif
}

static void hal_cmu_lcdc2d_mod_clock_disable(enum HAL_CMU_LCDC2D_USER_T user)
{
    uint8_t map = (1 << user);

    lcdc2d_clk_map &= ~map;

    if (lcdc2d_clk_map) {
        return;
    }

    cmu->DSI_CLK_DISABLE = CMU_SEL_PLL_PIX_DISABLE | CMU_RSTN_DIV_PIX_DISABLE | CMU_BYPASS_DIV_PIX_DISABLE |
        CMU_SEL_OSCX4_PIX_DISABLE | CMU_SEL_OSCX2_PIX_DISABLE;

#ifdef LCDC2D_USE_PLL_BB
    hal_cmu_pll_disable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_LCDC);
#endif
}

void hal_cmu_dma2d_clock_enable(void)
{
    hal_cmu_lcdc2d_mod_clock_enable(HAL_CMU_LCDC2D_USER_DMA2D);

    hal_cmu_disppre_hclk_enable(true);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_DISP);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_DMA2D);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_DMA2D);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_DISPB);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_DISPB);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DISPN2D);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DISTV2D);

    hal_cmu_reset_set(HAL_CMU_MOD_H_DISPPRE);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_DISPPRE);

    hal_cmu_reset_set(HAL_CMU_MOD_H_DMA2D);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_DMA2D);

    hal_cmu_reset_set(HAL_CMU_MOD_X_DMA2D);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_X_DMA2D);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DISPN2D);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DISPN2D);

    hal_cmu_reset_set(HAL_CMU_MOD_X_DISPB);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_X_DISPB);
}

void hal_cmu_dma2d_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_DISPPRE);
    hal_cmu_reset_set(HAL_CMU_MOD_H_DMA2D);
    hal_cmu_reset_set(HAL_CMU_MOD_X_DMA2D);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DISPN2D);
    hal_cmu_reset_set(HAL_CMU_MOD_X_DISPB);

    hal_cmu_disppre_hclk_enable(false);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_DMA2D);
    hal_cmu_clock_disable(HAL_CMU_MOD_X_DMA2D);
    hal_cmu_clock_disable(HAL_CMU_MOD_X_DISPB);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DISPN2D);

    hal_cmu_lcdc2d_mod_clock_disable(HAL_CMU_LCDC2D_USER_DMA2D);
}

void hal_cmu_dsi_phy_reset_set(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_Q_LCDC_P);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_PIX);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_DSI);
}

void hal_cmu_dsi_phy_reset_clear(void)
{
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_LCDC_P);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DIS_PIX);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DIS_DSI);
}

void hal_cmu_dsi_clock_enable(void)
{
    hal_psc_display_enable(HAL_PSC_DISPLAY_MOD_DSI);
    hal_cmu_pll_enable(HAL_CMU_PLL_DSI, HAL_CMU_PLL_USER_DSI);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_DSI);

    hal_cmu_dsi_phy_reset_clear();
}

void hal_cmu_dsi_clock_disable(void)
{
    hal_cmu_dsi_phy_reset_set();

    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_DSI);
    hal_cmu_pll_disable(HAL_CMU_PLL_DSI, HAL_CMU_PLL_USER_DSI);
    hal_psc_display_disable(HAL_PSC_DISPLAY_MOD_DSI);
}

void hal_cmu_dsi_sleep(void)
{
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_DSI);
    hal_cmu_pll_disable(HAL_CMU_PLL_DSI, HAL_CMU_PLL_USER_DSI);
}

void hal_cmu_dsi_wakeup(void)
{
    hal_cmu_pll_enable(HAL_CMU_PLL_DSI, HAL_CMU_PLL_USER_DSI);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_DSI);
}

void hal_cmu_lcdc_clock_enable(uint32_t pixmhz_speed)
{
    g_pixmhz_speed = pixmhz_speed;
    hal_cmu_lcdc2d_mod_clock_enable(HAL_CMU_LCDC2D_USER_LCDC);

    hal_cmu_disppre_hclk_enable(true);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_DISP);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_DISP);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_LCDC_P);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_PN);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_TV);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_PIX);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_DISPB);

    hal_cmu_reset_set(HAL_CMU_MOD_H_DISPPRE);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_DISPPRE);

    hal_cmu_reset_set(HAL_CMU_MOD_H_LCDC);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_LCDC);

    hal_cmu_reset_set(HAL_CMU_MOD_H_DISP);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_DISP);

    hal_cmu_reset_set(HAL_CMU_MOD_X_DISP);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_X_DISP);

    hal_cmu_reset_set(HAL_CMU_MOD_X_DISPB);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_X_DISPB);

    hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_PN);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_TV);
    hal_sys_timer_delay(1);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DIS_PN);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DIS_TV);
}

void hal_cmu_lcdc_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_DISPPRE);
    hal_cmu_reset_set(HAL_CMU_MOD_H_LCDC);
    hal_cmu_reset_set(HAL_CMU_MOD_H_DISP);
    hal_cmu_reset_set(HAL_CMU_MOD_X_DISP);
    hal_cmu_reset_set(HAL_CMU_MOD_X_DISPB);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_PN);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_TV);

    hal_cmu_disppre_hclk_enable(false);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_DISP);
    hal_cmu_clock_disable(HAL_CMU_MOD_X_DISP);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_LCDC_P);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_PN);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_TV);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_PIX);
    hal_cmu_lcdc2d_mod_clock_disable(HAL_CMU_LCDC2D_USER_LCDC);
    hal_psc_display_disable(HAL_PSC_DISPLAY_MOD_LCDC);
}

void hal_cmu_lcdc_sleep(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_X_DISPB);

    /*
     * reset set  HAL_CMU_MOD_Q_DIS_PN HAL_CMU_MOD_Q_DIS_TV will
     * cause dsi happen some error
     */
    // hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_PN);
    // hal_cmu_reset_set(HAL_CMU_MOD_Q_DIS_TV);

    hal_cmu_disppre_hclk_enable(false);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_DISP);
    hal_cmu_clock_disable(HAL_CMU_MOD_X_DISP);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_LCDC_P);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_PN);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_TV);
    hal_cmu_clock_disable(HAL_CMU_MOD_Q_DIS_PIX);
    hal_cmu_lcdc2d_mod_clock_disable(HAL_CMU_LCDC2D_USER_LCDC);
}

void hal_cmu_lcdc_wakeup(void)
{
    hal_cmu_lcdc2d_mod_clock_enable(HAL_CMU_LCDC2D_USER_LCDC);
    hal_cmu_disppre_hclk_enable(true);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_DISP);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_DISP);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_LCDC_P);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_PN);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_TV);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DIS_PIX);
    hal_cmu_clock_enable(HAL_CMU_MOD_X_DISPB);

    hal_cmu_reset_clear(HAL_CMU_MOD_X_DISPB);

    /*
     * reset set  HAL_CMU_MOD_Q_DIS_PN HAL_CMU_MOD_Q_DIS_TV will
     * cause dsi happen some error
     */
    // hal_cmu_reset_clear(HAL_CMU_MOD_Q_DIS_PN);
    // hal_cmu_reset_clear(HAL_CMU_MOD_Q_DIS_TV);
}

void hal_cmu_dma2d_reset_set(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_DMA2D);
    hal_cmu_reset_set(HAL_CMU_MOD_X_DMA2D);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DISPN2D);
}

void hal_cmu_dma2d_reset_clear(void)
{
    hal_cmu_reset_clear(HAL_CMU_MOD_H_DMA2D);
    hal_cmu_reset_clear(HAL_CMU_MOD_X_DMA2D);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DISPN2D);
}

void hal_cmu_rom_enable_pll(void)
{
    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_SYS);
    // Enable PLL on BTH as well
#ifdef SIMU
    if ((aoncmu->GBL_RESET_CLR & AON_CMU_SOFT_RSTN_BTH_CLR) == 0) {
        return;
    }
#endif
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_PLL_ENABLE;
    aoncmu->TOP_CLK1_ENABLE = AON_CMU_EN_CLK_PLLBB_BTH_ENABLE;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_init_pll_selection(void)
{
    enum HAL_CMU_PLL_T sys;

    // Disable the PLL which might be enabled in ROM
    hal_cmu_pll_disable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_ALL);

    hal_cmu_init_axi_pll_selection();

    // Select twice to set sys_pll_sel
    sys = hal_cmu_get_axi_sys_pll();
    hal_cmu_sys_select_pll(sys);

#if !(defined(ULTRA_LOW_POWER) || defined(OSC_26M_X4_AUD2BB))
    hal_cmu_pll_enable(sys, HAL_CMU_PLL_USER_SYS);
#endif

    // Always select one flash pll -- CMU_PU_PLL_ENABLE will check flash pll sel as well
    enum HAL_CMU_PLL_T flash;
#ifdef ROM_BUILD
    flash = HAL_CMU_PLL_BB;
#elif 0 //defined(MCU_HIGH_PERFORMANCE_MODE)
    // System is using AUDPLL (alias of BBPLL).
    // CAUTION: If sys is changed to use BBPLL as well, flash needs a large divider
    //          and high performance mode controlling codes should be located in RAM
    flash = (sys == HAL_CMU_PLL_AUD) ? HAL_CMU_PLL_DSI : HAL_CMU_PLL_AUD;
#else
    flash = sys;
#endif
    hal_cmu_flash_select_pll(flash);

#if defined(FLASH_HIGH_SPEED) || \
        (defined(FLASH_SRC_FREQ_MHZ) && (FLASH_SRC_FREQ_MHZ > 104)) || \
        !(defined(FLASH_LOW_SPEED) || defined(OSC_26M_X4_AUD2BB))
    hal_cmu_pll_enable(flash, HAL_CMU_PLL_USER_FLASH);
#endif
}

uint32_t hal_cmu_sys_get_gpio0_irq_tick()
{
    return cmu->RD_TIMER_TRIG_GPIO0;
}

uint32_t hal_cmu_sys_get_gpio1_irq_tick()
{
    return cmu->RD_TIMER_TRIG_GPIO1;
}

void hal_cmu_codec_set_fault_mask2(uint32_t msk0, uint32_t msk1)
{
    uint32_t lock;

    lock = int_lock();
    // If bit set 1, DAC will be muted when some faults occur
    cmu->M55_CODEC_OBS0 = SET_BITFIELD(cmu->M55_CODEC_OBS0, CMU_MASK_OBS0_CODEC_M55, msk0);
    cmu->M55_GPIO_OBS0 = SET_BITFIELD(cmu->M55_GPIO_OBS0, CMU_MASK_OBS0_GPIO_M55, msk0);
    cmu->ADMA_CH15_REQ = (cmu->ADMA_CH15_REQ &
        ~(CMU_MASK_OBS1_CODEC_M55_MASK | CMU_MASK_OBS1_GPIO_M55_MASK)) |
        CMU_MASK_OBS1_CODEC_M55(msk1) | CMU_MASK_OBS1_GPIO_M55(msk1);
    int_unlock(lock);
}

void hal_cmu_pcm_set_slave_mode(int clk_pol)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t cfg;

    mask = CMU_POL_CLK_PCM_IN;

    if (clk_pol) {
        cfg = CMU_POL_CLK_PCM_IN;
    } else {
        cfg = 0;
    }

    lock = int_lock();
    cmu->I2C_CLK = (cmu->I2C_CLK & ~mask) | cfg;
    int_unlock(lock);
}

void hal_cmu_pcm_clock_enable(void)
{
}

void hal_cmu_pcm_clock_disable(void)
{
}

#if defined(CHIP_HAS_USB) && defined(ROM_BUILD)
enum HAL_CMU_USB_CLOCK_SEL_T hal_cmu_usb_rom_select_clock_source(int pll_en, unsigned int crystal)
{
    return HAL_CMU_USB_CLOCK_SEL_24M_X2;
}

void hal_cmu_usb_rom_set_clock_source(enum HAL_CMU_USB_CLOCK_SEL_T sel)
{
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_apb_init_div(void)
{
    // Divider defaults to 2 (reg_val = div - 2)
    //cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_PCLK, 0);
}

int hal_cmu_periph_set_div(uint32_t div)
{
    uint32_t lock;
    int ret = 0;

    if (div == 0 || div > ((CMU_CFG_DIV_PER_MASK >> CMU_CFG_DIV_PER_SHIFT) + 2)) {
        cmu->DSI_CLK_DISABLE = CMU_RSTN_DIV_PER_DISABLE;
        if (div > ((CMU_CFG_DIV_PER_MASK >> CMU_CFG_DIV_PER_SHIFT) + 2)) {
            ret = 1;
        }
    } else {
        lock = int_lock();
        if (div == 1) {
            cmu->DSI_CLK_ENABLE = CMU_BYPASS_DIV_PER_ENABLE;
        } else {
            cmu->DSI_CLK_DISABLE = CMU_BYPASS_DIV_PER_DISABLE;
            div -= 2;
            cmu->I2S0_CLK = SET_BITFIELD(cmu->I2S0_CLK, CMU_CFG_DIV_PER, div);
        }
        int_unlock(lock);
        cmu->DSI_CLK_ENABLE = CMU_RSTN_DIV_PER_ENABLE;
    }

    return ret;
}

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        cmu->SYS_DIV &= ~CMU_SEL_OSCX2_SPI2;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        cmu->SYS_DIV |= CMU_SEL_OSCX2_SPI2;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

void hal_cmu_rom_clock_init(void)
{
    aoncmu->AON_CLK |= AON_CMU_SEL_CLK_OSC | AON_CMU_EN_CLK_SPI_ARB;
    // Enable PMU fast clock
    aoncmu->CLK_OUT |= AON_CMU_BYPASS_DIV_DCDC;
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_DCDC0_ENABLE;
    // Ignore subsystem 26M ready timer
    aoncmu->CLK_SELECT |= AON_CMU_OSC_READY_MODE;
    // Enable efuse clk
    aoncmu->EFUSE_CFG |= AON_CMU_EN_CLK_EFUSE;

    // Debug Select CMU REG F4
    cmu->MCU_TIMER = SET_BITFIELD(cmu->MCU_TIMER, CMU_DEBUG_REG_SEL, CMU_DEBUG_REG_SEL_DEBUG);

    hal_cmu_sys_periph_dma_cfg0_req_init();
}

void hal_cmu_init_chip_feature(uint16_t feature)
{
    aoncmu->CHIP_FEATURE = feature | AON_CMU_EFUSE_LOCK;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x2_enable(void)
{
#ifdef RC48M_ENABLE
    if (!rc_enabled)
#endif
    {
#ifdef DIG_OSC_X2_ENABLE
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_X2_DIG_ENABLE;
        aoncmu->CLK_SELECT |= AON_CMU_SEL_OSCX2_DIG;
#else
        cmu->SYS_CLK_ENABLE = CMU_PU_OSCX2_ENABLE;
        // 10 cycles of OSC clock are enough
        hal_sys_timer_delay(US_TO_TICKS(60));
#endif
    }
    cmu->SYS_CLK_ENABLE = CMU_EN_OSCX2_ENABLE;
#ifdef ROM_BUILD
    // Enable X2 on BTH as well
#ifdef SIMU
    if ((aoncmu->GBL_RESET_CLR & AON_CMU_SOFT_RSTN_BTH_CLR) == 0) {
        return;
    }
#endif
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_OSCX2_ENABLE;
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x4_enable_internal(void)
{
#ifdef ANA_26M_X4_ENABLE
    cmu->SYS_CLK_ENABLE = CMU_PU_OSCX4_ENABLE;
#ifdef RC48M_ENABLE
    if (rc_enabled) {
        hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
    } else
#endif
    {
        // 10 cycles of OSC clock are enough
        hal_sys_timer_delay(US_TO_TICKS(60));
    }
    cmu->SYS_CLK_ENABLE = CMU_EN_OSCX4_ENABLE;
#endif
#if defined(OSC_26M_X4_AUD2BB) && !defined(BTH_AS_MAIN_MCU)
    aoncmu->CLK_SELECT &= ~AON_CMU_SEL_X4_DIG;
#endif
}

void hal_cmu_osc_x4_disable_internal(void)
{
    cmu->SYS_CLK_DISABLE = CMU_EN_OSCX4_DISABLE;
    cmu->SYS_CLK_DISABLE = CMU_PU_OSCX4_DISABLE;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x4_enable(void)
{
#ifdef RC48M_ENABLE
    if (!rc_enabled)
#endif
    {
        hal_cmu_osc_x4_enable_internal();
    }
}

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU))) && defined(RC48M_ENABLE)
void hal_cmu_acquire_x2_x4(void)
{
    hal_cmu_osc_x4_enable_internal();
    cmu->SYS_CLK_ENABLE = CMU_PU_OSCX2_ENABLE;
    rc_enabled = false;
}

void hal_cmu_release_x2_x4(void)
{
    uint32_t k;

    for (k = 0; k < HAL_CMU_PLL_QTY; k++) {
        if (pll_user_map[k]) {
            break;
        }
    }
    if (k >= HAL_CMU_PLL_QTY) {
#ifdef DIG_OSC_X2_ENABLE
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_X2_DIG_ENABLE;
        aoncmu->CLK_SELECT &= ~AON_CMU_SEL_OSCX2_DIG;
#endif
        cmu->SYS_CLK_DISABLE = CMU_PU_OSCX2_DISABLE;
    }
    hal_cmu_osc_x4_disable_internal();
    rc_enabled = true;
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_module_init_state(void)
{
    // Init IRQ assignments
    cmu->MISC_0F8 = (cmu->MISC_0F8 & ~(CMU_RES3_EMMC_IRQ_S4_C19 | CMU_RES3_FPU_IRQ_S19)) | CMU_RES3_SDMMC_IRQ_S4;

    hal_psc_core_init_ram_ret_mask();

#if !defined(ARM_CMNS)
#if !(defined(BTH_AS_MAIN_MCU) && (defined(BTH_USE_SYS_PERIPH) || defined(BTH_CODEC_I2C_SLAVE)))
    hal_cmu_sys_module_init();
#endif

#if defined(__1600_FPGA_NEED_DEL_IT__)
    return;
#endif

#ifdef CORE_SLEEP_POWER_DOWN
    hal_cmu_set_wakeup_vector(SCB->VTOR);
    NVIC_SetResetHandler(hal_sleep_core_power_up);
#endif
#endif

#ifndef BTH_AS_MAIN_MCU
    hal_cmu_main_module_init();
#endif

#ifdef CPU_PC_DUMP
    hal_cpudump_enable();
#endif
}

void hal_cmu_lpu_wait_26m_ready(void)
{
    while ((cmu->WAKEUP_CLK_CFG & (CMU_LPU_AUTO_SWITCH26 | CMU_LPU_STATUS_26M)) ==
            CMU_LPU_AUTO_SWITCH26);
}

int BOOT_TEXT_FLASH_LOC hal_cmu_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg)
{
    // Done in hal_cmu_sys_module_init();
    return 0;
}

#ifdef CORE_SLEEP_POWER_DOWN
POSSIBLY_UNUSED
static int SRAM_TEXT_LOC hal_cmu_lpu_sleep_pd(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
#ifndef ARM_CMNS
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_hclk;
    uint32_t saved_oclk;
    uint32_t saved_clk_cfg;
    struct SAVED_CMU_REGS_T cmu_regs;
#endif
    const uint32_t cpu_reg_cnt =
#ifdef ARM_CMSE
        70;
#else
        50;
#endif
    uint32_t cpu_regs[cpu_reg_cnt];

#ifndef ARM_CMNS
    hal_psc_core_ram_pd_sleep();
#endif

    NVIC_PowerDownSleep(cpu_regs, ARRAY_SIZE(cpu_regs));

#ifndef ARM_CMNS
#ifdef ARM_CMSE
    hal_sec_sleep();
#endif
    hal_iomux_sleep();
    save_sys_cmu_regs(&cmu_regs);

    saved_hclk = cmu_regs.HCLK_ENABLE;
    saved_oclk = cmu_regs.OCLK_ENABLE;

    saved_clk_cfg = cmu_regs.SYS_CLK_ENABLE;

#ifndef BTH_AS_MAIN_MCU
    uint32_t saved_top_clk;

    saved_top_clk = aoncmu->TOP_CLK_ENABLE;
#endif

    // Force FLASH IO to output status (and outputing 0)
    aoncmu->FLASH_IODRV = (aoncmu->FLASH_IODRV & ~(AON_CMU_FLASH0_IOEN_MASK | AON_CMU_FLASH1_IOEN_MASK)) |
        AON_CMU_FLASH0_IOEN_DR | AON_CMU_FLASH1_IOEN_DR |
        AON_CMU_FLASH0_IOEN(0) | AON_CMU_FLASH1_IOEN(0);

    // Reset sub div
    cmu->UART_CLK = SET_BITFIELD(cmu->UART_CLK, CMU_SEL_SYS_GT, 3);

    // Disable memory/flash clock
    cmu->OCLK_DISABLE = SYS_OCLK_FLASH | SYS_OCLK_PSRAM;
    cmu->HCLK_DISABLE = SYS_HCLK_FLASH | SYS_HCLK_PSRAM | SYS_HCLK_LCDC;
    cmu->HCLK_DISABLE;
    __DSB();

#ifndef BTH_AS_MAIN_MCU
    // Shutdown PLLs
    if (saved_top_clk & AON_CMU_PU_PLLBB_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLBB_DISABLE;
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_PLLBB_DISABLE;
    }
    if (saved_top_clk & AON_CMU_PU_PLLUSB_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_PLLUSB_DISABLE;
    }
    if (saved_top_clk & AON_CMU_PU_PLLDSI_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLDSI_DISABLE;
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_PLLDSI_DISABLE;
    }
#endif

    // Switch system freq to 26M
    cmu->SYS_CLK_ENABLE = CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE;
    cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_2_SYS_DISABLE | CMU_SEL_FAST_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE;
    cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_SYS_DISABLE;
    cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_SYS_DISABLE;
    // Shutdown system PLL
    if (saved_clk_cfg & CMU_PU_PLL_ENABLE) {
        cmu->SYS_CLK_DISABLE = CMU_EN_PLL_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_PU_PLL_DISABLE;
    }

    cmu->SYS_CLK_ENABLE;
    __DSB();
#endif // !ARM_CMNS

    hal_sleep_core_power_down(false);

#ifndef ARM_CMNS
    while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0);

    // Switch system freq to 26M
    cmu->SYS_CLK_ENABLE = CMU_SEL_SLOW_SYS_ENABLE |
        (saved_clk_cfg & (CMU_PU_OSCX2_ENABLE | CMU_PU_OSCX4_ENABLE));
    cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_2_SYS_DISABLE;
    cmu->SYS_CLK_ENABLE;
    __DSB();
    // System freq is 26M now and will be restored later

    hal_sys_timer_wakeup();

    hal_cache_wakeup(HAL_CMU_LPU_SLEEP_MODE_SUBSYS_PD);

    if (saved_clk_cfg & CMU_PU_PLL_ENABLE) {
        cmu->SYS_CLK_ENABLE = CMU_PU_PLL_ENABLE;
        cmu->SYS_CLK_ENABLE;
        __DSB();
        start = hal_sys_timer_get();
        timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
        while ((hal_sys_timer_get() - start) < timeout);
        cmu->SYS_CLK_ENABLE = CMU_EN_PLL_ENABLE;
    }

    // Disable memory/flash clock
    cmu->OCLK_DISABLE = SYS_OCLK_FLASH | SYS_OCLK_PSRAM;
    cmu->HCLK_DISABLE = SYS_HCLK_FLASH | SYS_HCLK_PSRAM;
    cmu->HCLK_DISABLE;
    __DSB();

#ifndef BTH_AS_MAIN_MCU
    // Restore PLLs
    if (saved_top_clk & (AON_CMU_PU_PLLUSB_ENABLE | AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLDSI_ENABLE)) {
        bool wait_pll_locked;

        wait_pll_locked = false;
        if (saved_top_clk & AON_CMU_PU_PLLBB_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLBB_ENABLE;
            wait_pll_locked = true;
        }
        if (saved_top_clk & AON_CMU_PU_PLLUSB_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLUSB_ENABLE;
            wait_pll_locked = true;
        }
        if (saved_top_clk & AON_CMU_PU_PLLDSI_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLDSI_ENABLE;
            wait_pll_locked = true;
        }
        if (wait_pll_locked) {
            start = hal_sys_timer_get();
            timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
            while ((hal_sys_timer_get() - start) < timeout);
        }
        if (saved_top_clk & AON_CMU_EN_CLK_TOP_PLLBB_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
        }
        if (saved_top_clk & AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
        }
        if (saved_top_clk & AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE;
        }
        aoncmu->TOP_CLK_ENABLE;
        __DSB();
    }
#endif

    // Restore system freq
    cmu->SYS_CLK_ENABLE = saved_clk_cfg &
        (CMU_RSTN_DIV_FLS0_ENABLE | CMU_RSTN_DIV_PSRAM_ENABLE |
        CMU_RSTN_DIV_DSI_ENABLE | CMU_RSTN_DIV_SYS_ENABLE);
    cmu->SYS_CLK_ENABLE = saved_clk_cfg &
        (CMU_BYPASS_DIV_FLS0_ENABLE | CMU_BYPASS_DIV_PSRAM_ENABLE |
        CMU_BYPASS_DIV_DSI_ENABLE | CMU_BYPASS_DIV_SYS_ENABLE);
    cmu->SYS_CLK_ENABLE = saved_clk_cfg;
    cmu->SYS_CLK_DISABLE = ~saved_clk_cfg;

    if (saved_oclk & (SYS_OCLK_FLASH | SYS_OCLK_PSRAM)) {
        // Enable memory/flash clock
        cmu->HCLK_ENABLE = saved_hclk;
        cmu->OCLK_ENABLE = saved_oclk;
        cmu->OCLK_ENABLE;
        __DSB();
        // Wait until memory/flash clock ready
        hal_sys_timer_delay_us(2);
    }

    // Let FLASH CONTROLLER take the control of FLASH IO
    aoncmu->FLASH_IODRV &=  ~(AON_CMU_FLASH0_IOEN_MASK | AON_CMU_FLASH1_IOEN_MASK |
        AON_CMU_FLASH0_IOEN_DR | AON_CMU_FLASH1_IOEN_DR);

    restore_sys_cmu_regs(&cmu_regs);
    hal_iomux_wakeup();

#ifdef ARM_CMSE
    hal_sec_wakeup();
#endif
#endif // !ARM_CMNS

    NVIC_PowerDownWakeup(cpu_regs, ARRAY_SIZE(cpu_regs));

#ifndef ARM_CMNS
    hal_psc_core_ram_pd_wakeup();
#endif

    __DSB();

    // TODO:
    // 1) Register pm notif handler for all hardware modules, e.g., sdmmc
    // 2) Recover system timer in rt_suspend() and rt_resume()
    // 3) Dynamically select 32K sleep or power down sleep

    return 0;
}
#endif

static int SRAM_TEXT_LOC hal_cmu_lpu_sleep_normal(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
    POSSIBLY_UNUSED int32_t start;
    POSSIBLY_UNUSED int32_t timeout;
    POSSIBLY_UNUSED int32_t saved_hclk;
    POSSIBLY_UNUSED int32_t saved_oclk;
    POSSIBLY_UNUSED int32_t saved_clk_cfg;
    POSSIBLY_UNUSED int32_t saved_uart_clk;
    POSSIBLY_UNUSED int32_t wakeup_cfg;
#if defined(CORE_SLEEP_POWER_DOWN) && defined(CPU_PD_SUBSYS_PU)
    uint32_t cpu_regs[50];
#endif

#ifndef ARM_CMNS
    hal_psc_core_ram_ret_sleep();
#endif

#ifndef BTH_AS_MAIN_MCU
    POSSIBLY_UNUSED uint32_t saved_aon_clk;
    POSSIBLY_UNUSED uint32_t saved_top_clk;
    POSSIBLY_UNUSED bool pd_bb_pll;
    POSSIBLY_UNUSED bool pd_usb_pll;
    POSSIBLY_UNUSED bool wait_pll_locked;

    pd_bb_pll = true;
    pd_usb_pll = true;

    saved_aon_clk = aoncmu->AON_CLK;
    saved_top_clk = aoncmu->TOP_CLK_ENABLE;
#endif

    saved_hclk = cmu->HCLK_ENABLE;
    saved_oclk = cmu->OCLK_ENABLE;
    saved_clk_cfg = cmu->SYS_CLK_ENABLE;
    saved_uart_clk = cmu->UART_CLK;

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    // Disable memory/flash clock
#if defined(BTH_USE_SYS_FLASH) && !defined(FORCE_FLASH1_SLEEP)
    cmu->OCLK_DISABLE = SYS_OCLK_PSRAM;
    cmu->HCLK_DISABLE = SYS_HCLK_PSRAM;
#else
    cmu->OCLK_DISABLE = SYS_OCLK_FLASH | SYS_OCLK_PSRAM;
    cmu->HCLK_DISABLE = SYS_HCLK_FLASH | SYS_HCLK_PSRAM;
#endif
    cmu->HCLK_DISABLE;
    __DSB();

    // Light sleep only
    SCB->SCR = 0;
    __DSB();
    __WFI();

    if (saved_oclk & (SYS_OCLK_FLASH | SYS_OCLK_PSRAM)) {
        // Enable memory/flash clock
        cmu->HCLK_ENABLE = saved_hclk;
        cmu->OCLK_ENABLE = saved_oclk;
        cmu->OCLK_ENABLE;
        __DSB();
        // Wait until memory/flash clock ready
        hal_sys_timer_delay_us(2);
    }

    return 0;
#endif

#ifndef ARM_CMNS
    if (0) {
#if defined(CORE_SLEEP_POWER_DOWN) && defined(CPU_PD_SUBSYS_PU)
    } else if (mode == HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K) {
        wakeup_cfg = cmu->WAKEUP_CLK_CFG;
        ASSERT(wakeup_cfg & (CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26),
            "%s: Bad LPU mode: 0x%08X", __func__, wakeup_cfg);
#endif
    } else if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        wakeup_cfg = cmu->WAKEUP_CLK_CFG;
    } else {
        // SYS_32K
        wakeup_cfg = 0;
    }

#ifndef BTH_AS_MAIN_MCU
    if (mode != HAL_CMU_LPU_SLEEP_MODE_CHIP && mode != HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K) {
        if (pll_user_map[HAL_CMU_PLL_BB] & (1 << HAL_CMU_PLL_USER_AUD)) {
            pd_bb_pll = false;
#ifndef ROM_BUILD
            if (sys_pll_sel == HAL_CMU_PLL_BB) {
                aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
            }
#endif
        }
        if (pll_user_map[HAL_CMU_PLL_USB] & (1 << HAL_CMU_PLL_USER_AUD)) {
            pd_usb_pll = false;
#ifndef ROM_BUILD
            if (sys_pll_sel == HAL_CMU_PLL_USB) {
                aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLUSB_ENABLE | AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
            }
#endif
        }
        // Avoid auto-gating OSC and OSCX2
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX2_ENABLE | AON_CMU_EN_CLK_TOP_OSC_ENABLE;
        aoncmu->TOP_CLK_ENABLE;
        __DSB();
    }
#endif

    // Reset sub div
    cmu->UART_CLK = SET_BITFIELD(cmu->UART_CLK, CMU_SEL_SYS_GT, 3);

    // Disable memory/flash clock
    cmu->OCLK_DISABLE = SYS_OCLK_FLASH | SYS_OCLK_PSRAM;
    cmu->HCLK_DISABLE = SYS_HCLK_FLASH | SYS_HCLK_PSRAM;
    cmu->HCLK_DISABLE;
    __DSB();

    // Setup wakeup mask
    uint32_t wake_mask[3];
#ifdef ARM_CMSE
    wake_mask[0] = NVIC->ISER[0] | NVIC_NS->ISER[0];
    wake_mask[1] = NVIC->ISER[1] | NVIC_NS->ISER[1];
    wake_mask[2] = NVIC->ISER[2] | NVIC_NS->ISER[2];
#else
    wake_mask[0] = NVIC->ISER[0];
    wake_mask[1] = NVIC->ISER[1];
    wake_mask[2] = NVIC->ISER[2];
#endif
    cmu->WAKEUP_MASK0 = wake_mask[0];
    cmu->WAKEUP_MASK1 = wake_mask[1];
    cmu->SYS2OTH_MASK2 = SET_BITFIELD(cmu->SYS2OTH_MASK2, CMU_WAKEUP_IRQ_MASK2,
        (wake_mask[2] & (CMU_WAKEUP_IRQ_MASK2_MASK >> CMU_WAKEUP_IRQ_MASK2_SHIFT)));

#if !(defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH))
    if (wakeup_cfg & (CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26)) {
        // Enable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_MANUAL_RAM_RETN) |
            CMU_DEEPSLEEP_EN | CMU_DEEPSLEEP_ROMRAM_EN | CMU_DEEPSLEEP_START;
    } else {
        // Disable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_DEEPSLEEP_ROMRAM_EN) |
            CMU_DEEPSLEEP_EN | CMU_MANUAL_RAM_RETN | CMU_DEEPSLEEP_START;
    }
#endif

#ifndef BTH_AS_MAIN_MCU
    // Shutdown PLLs
    if (pd_bb_pll && (saved_top_clk & AON_CMU_PU_PLLBB_ENABLE)) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLBB_DISABLE;
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_PLLBB_DISABLE;
    }
    if (pd_usb_pll && (saved_top_clk & AON_CMU_PU_PLLUSB_ENABLE)) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_PLLUSB_DISABLE;
    }
    if (saved_top_clk & AON_CMU_PU_PLLDSI_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLDSI_DISABLE;
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_PLLDSI_DISABLE;
    }
#endif

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        // Do nothing
        // Hardware will switch system freq to 32K and shutdown PLLs automatically
    } else {
        // Switch system freq to 26M
        cmu->SYS_CLK_ENABLE = CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE;
        cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_2_SYS_DISABLE | CMU_SEL_FAST_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_SYS_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_SYS_DISABLE;
        // Shutdown system PLL
        if (saved_clk_cfg & CMU_PU_PLL_ENABLE) {
            cmu->SYS_CLK_DISABLE = CMU_EN_PLL_DISABLE;
            cmu->SYS_CLK_DISABLE = CMU_PU_PLL_DISABLE;
        }
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            // Do nothing
            // Hardware will switch system freq to 32K automatically
        } else {
#ifndef BTH_AS_MAIN_MCU
            // Manually switch AON_CMU clock to 32K
            aoncmu->AON_CLK = saved_aon_clk & ~AON_CMU_SEL_CLK_OSC;
            aoncmu->AON_CLK;
#endif
            // Switch system freq to 32K
            cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_4_SYS_DISABLE | CMU_SEL_SLOW_SYS_DISABLE;
        }
        cmu->SYS_CLK_ENABLE;
        __DSB();
    }
#endif // !ARM_CMNS

#if defined(CORE_SLEEP_POWER_DOWN) && defined(CPU_PD_SUBSYS_PU)
    if (mode == HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K || mode == HAL_CMU_LPU_SLEEP_MODE_CPU_PD_BUS_32K) {
        NVIC_PowerDownSleep(cpu_regs, ARRAY_SIZE(cpu_regs));

        hal_sleep_core_power_down(true);

        NVIC_PowerDownWakeup(cpu_regs, ARRAY_SIZE(cpu_regs));
    } else
#endif
    {
#ifdef ARM_CMNS
        TZ_hal_sleep_deep_sleep_S();
#else
        if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
            SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
        } else {
            SCB->SCR = 0;
        }
        __DSB();
        __WFI();
#endif
    }

#ifndef ARM_CMNS
    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_PLL_LOCKED_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
        while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_PLL) == 0 &&
            (hal_sys_timer_get() - start) < timeout);
        // !!! CAUTION !!!
        // Hardware will switch system freq to PLL divider and enable PLLs automatically
    } else {
        // Wait for 26M ready
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            start = hal_sys_timer_get();
            timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
            while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0 &&
                (hal_sys_timer_get() - start) < timeout);
            // Hardware will switch system freq to 26M automatically
        } else {
            if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
                timeout = HAL_CMU_26M_READY_TIMEOUT;
                hal_sys_timer_delay(timeout);
            }
            // Switch system freq to 26M
            cmu->SYS_CLK_ENABLE = CMU_SEL_SLOW_SYS_ENABLE;
            cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_2_SYS_DISABLE;
            cmu->SYS_CLK_ENABLE;
#ifndef BTH_AS_MAIN_MCU
            // Restore AON_CMU clock
            aoncmu->AON_CLK = saved_aon_clk;
            aoncmu->AON_CLK;
#endif
            __DSB();
        }
        // System freq is 26M now and will be restored later
        if (saved_clk_cfg & CMU_PU_PLL_ENABLE) {
            cmu->SYS_CLK_ENABLE = CMU_PU_PLL_ENABLE;
            cmu->SYS_CLK_ENABLE;
            __DSB();
            start = hal_sys_timer_get();
            timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
            while ((hal_sys_timer_get() - start) < timeout);
            cmu->SYS_CLK_ENABLE = CMU_EN_PLL_ENABLE;
            cmu->SYS_CLK_ENABLE;
            __DSB();
        }
    }

#ifndef BTH_AS_MAIN_MCU
    // Restore PLLs
    if (saved_top_clk & (AON_CMU_PU_PLLUSB_ENABLE | AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLDSI_ENABLE)) {
        wait_pll_locked = false;
        if (pd_bb_pll && (saved_top_clk & AON_CMU_PU_PLLBB_ENABLE)) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLBB_ENABLE;
            wait_pll_locked = true;
        }
        if (pd_usb_pll && (saved_top_clk & AON_CMU_PU_PLLUSB_ENABLE)) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLUSB_ENABLE;
            wait_pll_locked = true;
        }
        if (saved_top_clk & AON_CMU_PU_PLLDSI_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_PLLDSI_ENABLE;
            wait_pll_locked = true;
        }
        if (wait_pll_locked) {
            start = hal_sys_timer_get();
            timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
            while ((hal_sys_timer_get() - start) < timeout);
        }
        if (pd_bb_pll && (saved_top_clk & AON_CMU_EN_CLK_TOP_PLLBB_ENABLE)) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
        }
        if (pd_usb_pll && (saved_top_clk & AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE)) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
        }
        if (saved_top_clk & AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE) {
            aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE;
        }
        aoncmu->TOP_CLK_ENABLE;
        __DSB();
    }
#endif

    // Restore system freq
    cmu->SYS_CLK_ENABLE = saved_clk_cfg & CMU_RSTN_DIV_SYS_ENABLE;
    cmu->SYS_CLK_ENABLE = saved_clk_cfg & CMU_BYPASS_DIV_SYS_ENABLE;
    cmu->SYS_CLK_ENABLE = saved_clk_cfg;
    cmu->SYS_CLK_DISABLE = ~saved_clk_cfg;
    cmu->UART_CLK = saved_uart_clk;
    cmu->UART_CLK;
    __DSB();

#ifndef BTH_AS_MAIN_MCU
    if (mode != HAL_CMU_LPU_SLEEP_MODE_CHIP && mode != HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K) {
        // Restore the original top clock settings
        aoncmu->TOP_CLK_DISABLE = ~saved_top_clk;
        aoncmu->TOP_CLK_DISABLE;
        __DSB();
    }
#endif

    if (saved_oclk & (SYS_OCLK_FLASH | SYS_OCLK_PSRAM)) {
        // Enable memory/flash clock
        cmu->HCLK_ENABLE = saved_hclk;
        cmu->OCLK_ENABLE = saved_oclk;
        cmu->OCLK_ENABLE;
        __DSB();
        // Wait until memory/flash clock ready
        hal_sys_timer_delay_us(2);
    }
#endif // !ARM_CMNS

    return 0;
}

int SRAM_TEXT_LOC hal_cmu_lpu_sleep(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
#ifdef CORE_SLEEP_POWER_DOWN
    if (mode == HAL_CMU_LPU_SLEEP_MODE_POWER_DOWN) {
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
#ifdef CPU_PD_SUBSYS_PU
        mode = HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K;
#else
        mode = HAL_CMU_LPU_SLEEP_MODE_SUBSYS_32K;
#endif
#else
        return hal_cmu_lpu_sleep_pd(mode);
#endif
    }
#endif
    return hal_cmu_lpu_sleep_normal(mode);
}

int hal_cmu_bt_trigger_set_handler(enum HAL_CMU_BT_TRIGGER_SRC_T src, HAL_CMU_BT_TRIGGER_HANDLER_T hdlr)
{
    if (src >= HAL_CMU_BT_TRIGGER_SRC_QTY) {
        return 1;
    }

    bt_trig_hdlr[src] = hdlr;
    return 0;
}

static void hal_cmu_bt_trigger_irq_handler(void)
{
    for (uint32_t i = 0; i < HAL_CMU_BT_TRIGGER_SRC_QTY; i++) {
        if (cmu->ISIRQ_SET & (CMU_BT_PLAYTIME_STAMP_INTR_MSK << i)) {
            cmu->ISIRQ_CLR = (CMU_BT_PLAYTIME_STAMP_INTR_CLR << i);
            if (bt_trig_hdlr[i]) {
                bt_trig_hdlr[i]((enum HAL_CMU_BT_TRIGGER_SRC_T)i);
            }
        }
    }
}

int hal_cmu_bt_trigger_enable(enum HAL_CMU_BT_TRIGGER_SRC_T src)
{
    uint32_t lock;
    uint32_t val;
    uint32_t i;

    if (src >= HAL_CMU_BT_TRIGGER_SRC_QTY) {
        return 1;
    }

    lock = int_lock();
    cmu->ISIRQ_CLR = (CMU_BT_PLAYTIME_STAMP_INTR_CLR << src);
    val = cmu->PERIPH_CLK;
    cmu->PERIPH_CLK = val | (CMU_BT_PLAYTIME_STAMP_MASK << src);
    for (i = 0; i < HAL_CMU_BT_TRIGGER_SRC_QTY; i++) {
        if (val & (CMU_BT_PLAYTIME_STAMP_MASK << i)) {
            break;
        }
    }
    if (i >= HAL_CMU_BT_TRIGGER_SRC_QTY) {
        NVIC_SetVector(BT_STAMP_IRQn, (uint32_t)hal_cmu_bt_trigger_irq_handler);
        NVIC_SetPriority(BT_STAMP_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(BT_STAMP_IRQn);
        NVIC_EnableIRQ(BT_STAMP_IRQn);
    }
    int_unlock(lock);

    return 0;
}

int hal_cmu_bt_trigger_disable(enum HAL_CMU_BT_TRIGGER_SRC_T src)
{
    uint32_t lock;
    uint32_t val;
    uint32_t i;

    if (src >= HAL_CMU_BT_TRIGGER_SRC_QTY) {
        return 1;
    }

    lock = int_lock();
    val = cmu->PERIPH_CLK;
    val &= ~(CMU_BT_PLAYTIME_STAMP_MASK << src);
    cmu->PERIPH_CLK = val;
    for (i = 0; i < HAL_CMU_BT_TRIGGER_SRC_QTY; i++) {
        if (val & (CMU_BT_PLAYTIME_STAMP_MASK << i)) {
            break;
        }
    }
    if (i >= HAL_CMU_BT_TRIGGER_SRC_QTY) {
        NVIC_DisableIRQ(BT_STAMP_IRQn);
    }
    int_unlock(lock);

    return 0;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_set_wakeup_vector(uint32_t vector)
{
    aonsec->CORE_VTOR = (aonsec->CORE_VTOR & ~AON_SEC_VTOR_CORE_SYS_MASK) | (vector & AON_SEC_VTOR_CORE_SYS_MASK);
}

void BOOT_TEXT_FLASH_LOC hal_cpudump_clock_enable(void)
{
    cmu->HCLK_ENABLE = SYS_HCLK_DUMPCPU;
    cmu->HRESET_CLR  = SYS_HRST_DUMPCPU;
}

void hal_cpudump_clock_disable(void)
{
    cmu->HRESET_SET   = SYS_HCLK_DUMPCPU;
    cmu->HCLK_DISABLE = SYS_HRST_DUMPCPU;
}

uint32_t hal_cmu_cpudump_get_last_addr(void)
{
    uint32_t addr = 0x100;
    if(aoncmu->SYS_LAST_PC_INFO & AON_CMU_SYS_LAST_PC_EN)
        addr = GET_BITFIELD(aoncmu->SYS_LAST_PC_INFO, AON_CMU_SYS_LAST_PC_ADDR);

    return addr;
}

#ifndef BTH_AS_MAIN_MCU
void BOOT_TEXT_SRAM_LOC hal_cmu_boot_flash_reset_pulse(void)
{
#ifdef ALT_BOOT_FLASH
    bthcmu->ORESET_SET = BTH_ORST_FLASH1;
    bthcmu->HRESET_SET = BTH_HRST_FLASH1;
    hal_sys_timer_delay_us(2);
    bthcmu->ORESET_CLR = BTH_ORST_FLASH1;
    bthcmu->HRESET_CLR = BTH_HRST_FLASH1;
#else
    cmu->ORESET_SET = SYS_ORST_FLASH;
    cmu->HRESET_SET = SYS_HRST_FLASH;
    hal_sys_timer_delay_us(2);
    cmu->ORESET_CLR = SYS_ORST_FLASH;
    cmu->HRESET_CLR = SYS_HRST_FLASH;
#endif
}

void hal_cmu_hifi4_sleep_signal_set(void)
{
    cmu->DSP2M55_IRQ_SET = CMU_DSP2M55_DATA2_IND_SET;
}

void hal_cmu_hifi4_sleep_signal_clear(void)
{
    cmu->DSP2M55_IRQ_CLR = CMU_DSP2M55_DATA2_IND_CLR;
}

int hal_cmu_hifi4_sleep_signal_get(void)
{
    return !!(cmu->DSP2M55_IRQ_SET & CMU_DSP2M55_DATA2_IND_SET);
}

void hal_cmu_hifi4_wakeup_allow_signal_set(void)
{
    cmu->CPU_IDLE_MSK |= CMU_DSP_NMI_INT;
    cmu->M552DSP_IRQ_SET = CMU_DSP2M55_DATA2_DONE_SET;
}

void hal_cmu_hifi4_wakeup_allow_signal_clear(void)
{
    cmu->CPU_IDLE_MSK &= ~CMU_DSP_NMI_INT;
    cmu->M552DSP_IRQ_CLR = CMU_DSP2M55_DATA2_DONE_CLR;
}

int hal_cmu_hifi4_wakeup_allow_signal_get(void)
{
    return !!(cmu->M552DSP_IRQ_SET & CMU_DSP2M55_DATA2_DONE_SET);
}

void hal_cmu_hifi4_wakeup_mask_get(uint32_t mask[3])
{
    mask[0] = cmu->DSP_WAKEUP_MASK0;
    mask[1] = cmu->DSP_WAKEUP_MASK1;
    mask[2] = cmu->DSP_WAKEUP_MASK2;
}

void NAKED BOOT_TEXT_FLASH_LOC hal_cmu_boot_setup_sp_ram(void)
{
#ifdef JUMP_TO_ALT_FLASH
    if (*(volatile uint32_t *)FLASH1_NC_BASE != ~0UL) {
        (*(void (*)(void))(FLASH1_BASE + 0x11))();
    }
    while (1) {}
#else
    asm volatile(
        "movs r4, lr;"
        // Use BTH dedicated ram as temporary sp
        "bl hal_psc_boot_entry_bth_enable;"
        "bl hal_cmu_bth_clock_enable;"
        "bl hal_cmu_bth_reset_clear;"
        "ldr r0, =" TO_STRING(BTH_RAM2_BASE) ";"
        "msr msp, r0;"
        "ldr r0, =" TO_STRING(BTH_RAM1_BASE) ";"
        "msr msplim, r0;"
        // Init dtcm cfg
        "movs r0, #1;"
        "bl hal_cmu_sys_m55_dtcm_cfg;"
        // Init itcm cfg
        "movs r0, #1;"
        "bl hal_cmu_sys_m55_itcm_cfg;"
        // Init sys ram cfg
        "bl hal_cmu_sys_ram_cfg;"
        // Return
        "bx r4;"
    );
#endif
}

void NORETURN boot_loader_entry_hook(void) __attribute__((alias("hal_cmu_boot_setup_sp_ram")));

#endif

#endif
