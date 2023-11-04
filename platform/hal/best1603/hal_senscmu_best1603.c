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
#ifdef CHIP_SUBSYS_SENS

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_aoncmu)
#include CHIP_SPECIFIC_HDR(reg_senscmu)
#include CHIP_SPECIFIC_HDR(hal_cmu_pri)
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_bootmode.h"
#include "hal_chipid.h"
#include "hal_codec.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_psc.h"
#include "hal_sleep_core_pd.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_cpudump.h"
#include "cmsis_nvic.h"
#include "pmu.h"

#ifdef VOICE_DETECTOR_EN
#error "The best1603 sensor don`t have VAD or CODEC!"
#endif

void hal_cmu_osc_x4_enable_internal(void);
void hal_cmu_osc_x4_disable_internal(void);

enum SENS_CMU_DEBUG_REG_SEL_T {
    SENS_CMU_DEBUG_REG_SEL_MCU_PC   = 0,
    SENS_CMU_DEBUG_REG_SEL_MCU_LR   = 1,
    SENS_CMU_DEBUG_REG_SEL_MCU_SP   = 2,
    SENS_CMU_DEBUG_REG_SEL_DEBUG    = 7,
};

enum SENS_CMU_I2S_MCLK_SEL_T {
    SENS_CMU_I2S_MCLK_SEL_MCU       = 0,
    SENS_CMU_I2S_MCLK_SEL_SENS      = 1,
    SENS_CMU_I2S_MCLK_SEL_CODEC     = 2,
};

enum SENS_DMA_REQ_T {
    SENS_DMA_REQ_UART2_RX           = 0,
    SENS_DMA_REQ_UART2_TX           = 1,
    SENS_DMA_REQ_RESERVED_02        = 2,
    SENS_DMA_REQ_RESERVED_03        = 3,
    SENS_DMA_REQ_RESERVED_04        = 4,
    SENS_DMA_REQ_RESERVED_05        = 5,
    SENS_DMA_REQ_VAD_RX             = 6,
    SENS_DMA_REQ_RESERVED_07        = 7,
    SENS_DMA_REQ_RESERVED_08        = 8,
    SENS_DMA_REQ_I2C4_RX            = 9,
    SENS_DMA_REQ_I2C4_TX            = 10,
    SENS_DMA_REQ_I2C0_RX            = 11,
    SENS_DMA_REQ_I2C0_TX            = 12,
    SENS_DMA_REQ_SPILCD0_RX         = 13,
    SENS_DMA_REQ_SPILCD0_TX         = 14,
    SENS_DMA_REQ_SPILCD1_RX         = 15,
    SENS_DMA_REQ_SPILCD1_TX         = 16,
    SENS_DMA_REQ_UART0_RX           = 17,
    SENS_DMA_REQ_UART0_TX           = 18,
    SENS_DMA_REQ_I2C1_RX            = 19,
    SENS_DMA_REQ_I2C1_TX            = 20,
    SENS_DMA_REQ_SPI_ITN_RX         = 21,
    SENS_DMA_REQ_SPI_ITN_TX         = 22,
    SENS_DMA_REQ_I2S0_RX            = 23,
    SENS_DMA_REQ_I2S0_TX            = 24,
    SENS_DMA_REQ_I2C2_RX            = 25,
    SENS_DMA_REQ_I2C2_TX            = 26,
    SENS_DMA_REQ_I2C3_RX            = 27,
    SENS_DMA_REQ_I2C3_TX            = 28,
    SENS_DMA_REQ_UART1_RX           = 29,
    SENS_DMA_REQ_UART1_TX           = 30,

    SENS_DMA_REQ_QTY,
    SENS_DMA_REQ_NULL               = SENS_DMA_REQ_QTY,
};

static struct SENSCMU_T * const senscmu = (struct SENSCMU_T *)SENS_CMU_BASE;

static struct AONCMU_T * const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

static uint8_t BOOT_BSS_LOC pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(pll_user_map[0]) * 8, "Too many PLL users");

#ifdef LOW_SYS_FREQ
static enum HAL_CMU_FREQ_T BOOT_BSS_LOC cmu_sys_freq;
#endif

#ifdef __AUDIO_RESAMPLE__
static bool aud_resample_en = true;
#endif

void hal_cmu_audio_resample_enable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = true;
#endif
}

void hal_cmu_audio_resample_disable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = false;
#endif
}

int hal_cmu_get_audio_resample_status(void)
{
#ifdef __AUDIO_RESAMPLE__
    return aud_resample_en;
#else
    return false;
#endif
}

static inline void aocmu_reg_update_wait(void)
{
    // Make sure AOCMU (26M clock domain) write opertions finish before return
    aoncmu->CHIP_ID;
}

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_AON_MCU) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        senscmu->HCLK_ENABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        senscmu->PCLK_ENABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        senscmu->OCLK_ENABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        aoncmu->MOD_CLK_ENABLE = (1 << (id - HAL_CMU_AON_A_CMU));
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
        senscmu->HCLK_DISABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        senscmu->PCLK_DISABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        senscmu->OCLK_DISABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
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
        status = senscmu->HCLK_ENABLE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = senscmu->PCLK_ENABLE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = senscmu->OCLK_ENABLE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        status = aoncmu->MOD_CLK_ENABLE & (1 << (id - HAL_CMU_AON_A_CMU));
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
        reg = &senscmu->HCLK_MODE;
        val = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        reg = &senscmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        reg = &senscmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_O_SLEEP));
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
        mode = senscmu->HCLK_MODE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        mode = senscmu->PCLK_MODE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        mode = senscmu->OCLK_MODE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
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
        senscmu->HRESET_SET = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        senscmu->PRESET_SET = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        senscmu->ORESET_SET = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_SET = (1 << (id - HAL_CMU_AON_A_CMU));
    } else {
        aoncmu->GBL_RESET_SET = (1 << (id - HAL_CMU_AON_MCU));
    }

    return 0;
}

int hal_cmu_reset_clear(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        senscmu->HRESET_CLR = (1 << id);
        asm volatile("nop; nop;");
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        senscmu->PRESET_CLR = (1 << (id - HAL_CMU_MOD_P_CMU));
        asm volatile("nop; nop; nop; nop;");
    } else if (id < HAL_CMU_AON_A_CMU) {
        senscmu->ORESET_CLR = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_CLR = (1 << (id - HAL_CMU_AON_A_CMU));
        aocmu_reg_update_wait();
    } else {
        aoncmu->GBL_RESET_CLR = (1 << (id - HAL_CMU_AON_MCU));
        aocmu_reg_update_wait();
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
        status = senscmu->HRESET_SET & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = senscmu->PRESET_SET & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = senscmu->ORESET_SET & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        status = aoncmu->RESET_SET & (1 << (id - HAL_CMU_AON_A_CMU));
    } else {
        status = aoncmu->GBL_RESET_SET & (1 << (id - HAL_CMU_AON_MCU));
    }

    return status ? HAL_CMU_RST_CLR : HAL_CMU_RST_SET;
}

int hal_cmu_reset_pulse(enum HAL_CMU_MOD_ID_T id)
{
    volatile int i;

    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (hal_cmu_reset_get_status(id) == HAL_CMU_RST_SET) {
        return hal_cmu_reset_clear(id);
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        senscmu->HRESET_PULSE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        senscmu->PRESET_PULSE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        senscmu->ORESET_PULSE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_PULSE = (1 << (id - HAL_CMU_AON_A_CMU));
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
    } else {
        aoncmu->GBL_RESET_PULSE = (1 << (id - HAL_CMU_AON_MCU));
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
    }
    // Delay 5+ PCLK cycles (10+ HCLK cycles)
    for (i = 0; i < 3; i++);

    return 0;
}

void hal_cmu_timer0_select_fast(void)
{
}

void hal_cmu_timer0_select_slow(void)
{
}

void hal_cmu_timer1_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    senscmu->CLK_OUT |= (1 << (SENS_CMU_SEL_TIMER_FAST_SHIFT + 0));
    int_unlock(lock);
}

void hal_cmu_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    senscmu->CLK_OUT &= ~(1 << (SENS_CMU_SEL_TIMER_FAST_SHIFT + 0));
    int_unlock(lock);
}

void hal_cmu_timer2_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    senscmu->CLK_OUT |= (1 << (SENS_CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void hal_cmu_timer2_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    senscmu->CLK_OUT &= ~(1 << (SENS_CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

int hal_cmu_fast_timer_init(void)
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
    senscmu->CLK_OUT = SET_BITFIELD(senscmu->CLK_OUT, SENS_CMU_CFG_TIMER_FAST, val);

    return 0;
}

uint32_t hal_cmu_get_fast_timer_freq(void)
{
    uint32_t div = 4;

#if defined(LOW_SYS_FREQ) && defined(LOW_SYS_FREQ_6P5M)
    div = 8;
#endif
    return hal_cmu_get_crystal_freq() / div;
}

int hal_cmu_sys_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    int div;
    uint32_t sub_div;
    uint32_t lock;

    ASSERT(freq <= HAL_CMU_FREQ_48M, "sensor hub freq can not > 48MHz");

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }

#ifdef LOW_SYS_FREQ
    cmu_sys_freq = freq;
#endif

    div = -1;
    sub_div = 3;

    switch (freq) {
    case HAL_CMU_FREQ_32K:
        enable = SENS_CMU_SEL_OSCX2_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE | SENS_CMU_SEL_OSC_2_SYS_DISABLE | SENS_CMU_SEL_OSC_4_SYS_DISABLE |
            SENS_CMU_SEL_SLOW_SYS_DISABLE | SENS_CMU_SEL_FAST_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE | SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        break;
    case HAL_CMU_FREQ_6M:
#if defined(LOW_SYS_FREQ) && defined(LOW_SYS_FREQ_6P5M)
        enable = SENS_CMU_SEL_OSC_4_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE | SENS_CMU_SEL_SLOW_SYS_DISABLE |
            SENS_CMU_SEL_FAST_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE | SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_12M:
#ifdef LOW_SYS_FREQ
        enable = SENS_CMU_SEL_OSC_2_SYS_ENABLE | SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE |
            SENS_CMU_SEL_FAST_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE | SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_15M:
    case HAL_CMU_FREQ_18M:
    case HAL_CMU_FREQ_21M:
    case HAL_CMU_FREQ_24M:
        if (freq == HAL_CMU_FREQ_15M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_FREQ_18M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_FREQ_21M) {
            sub_div = 2;
        }
        enable = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE | SENS_CMU_SEL_OSC_2_SYS_DISABLE |
            SENS_CMU_SEL_FAST_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE | SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        break;
    case HAL_CMU_FREQ_30M:
    case HAL_CMU_FREQ_36M:
    case HAL_CMU_FREQ_42M:
    case HAL_CMU_FREQ_48M:
        if (freq == HAL_CMU_FREQ_30M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_FREQ_36M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_FREQ_42M) {
            sub_div = 2;
        }
        enable = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE | SENS_CMU_SEL_FAST_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE | SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        break;
    case HAL_CMU_FREQ_60M:
    case HAL_CMU_FREQ_72M:
    case HAL_CMU_FREQ_84M:
    case HAL_CMU_FREQ_96M:
        if (freq == HAL_CMU_FREQ_60M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_FREQ_72M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_FREQ_84M) {
            sub_div = 2;
        }
#ifdef OSC_26M_X4_AUD2BB
        enable = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_FAST_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE | SENS_CMU_SEL_OSCX2_SYS_DISABLE | SENS_CMU_BYPASS_DIV_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE;
        break;
#else
        enable = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_RSTN_DIV_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE | SENS_CMU_SEL_PLL_SYS_ENABLE;
        disable = SENS_CMU_BYPASS_DIV_SYS_DISABLE;
#ifdef AUD_PLL_DOUBLE
        div = 2;
#else
        div = 0;
#endif
        break;
#endif
    case HAL_CMU_FREQ_120M:
    case HAL_CMU_FREQ_144M:
    case HAL_CMU_FREQ_168M:
    case HAL_CMU_FREQ_192M:
#ifndef AUD_PLL_DOUBLE
    default:
#endif
        if (freq == HAL_CMU_FREQ_120M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_FREQ_144M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_FREQ_168M) {
            sub_div = 2;
        }
#ifdef AUD_PLL_DOUBLE
        enable = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_RSTN_DIV_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE | SENS_CMU_SEL_PLL_SYS_ENABLE;
        disable = SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        div = 0;
#else
        enable = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE | SENS_CMU_BYPASS_DIV_SYS_ENABLE | SENS_CMU_SEL_PLL_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE;
#endif
        break;
#ifdef AUD_PLL_DOUBLE
    case HAL_CMU_FREQ_240M:
    case HAL_CMU_FREQ_290M:
    case HAL_CMU_FREQ_340M:
    case HAL_CMU_FREQ_390M:
    default:
        if (freq == HAL_CMU_FREQ_240M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_FREQ_290M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_FREQ_340M) {
            sub_div = 2;
        }
        enable = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE | SENS_CMU_BYPASS_DIV_SYS_ENABLE | SENS_CMU_SEL_PLL_SYS_ENABLE;
        disable = SENS_CMU_RSTN_DIV_SYS_DISABLE;
        break;
#endif
    };

    lock = int_lock();
    senscmu->CLK_OUT = SET_BITFIELD(senscmu->CLK_OUT, SENS_CMU_SEL_SYS_GT, sub_div);
    if (div >= 0) {
        senscmu->SYS_DIV = SET_BITFIELD(senscmu->SYS_DIV, SENS_CMU_CFG_DIV_SYS, div);
    }
    int_unlock(lock);

    if (enable & SENS_CMU_SEL_PLL_SYS_ENABLE) {
        senscmu->SYS_CLK_ENABLE = SENS_CMU_RSTN_DIV_SYS_ENABLE;
        if (enable & SENS_CMU_BYPASS_DIV_SYS_ENABLE) {
            senscmu->SYS_CLK_ENABLE = SENS_CMU_BYPASS_DIV_SYS_ENABLE;
        } else {
            senscmu->SYS_CLK_DISABLE = SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        }
    }
    senscmu->SYS_CLK_ENABLE = enable;
    if (enable & SENS_CMU_SEL_PLL_SYS_ENABLE) {
        senscmu->SYS_CLK_DISABLE = disable;
    } else {
        senscmu->SYS_CLK_DISABLE = disable & ~(SENS_CMU_RSTN_DIV_SYS_DISABLE | SENS_CMU_BYPASS_DIV_SYS_DISABLE);
        senscmu->SYS_CLK_DISABLE = SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        senscmu->SYS_CLK_DISABLE = SENS_CMU_RSTN_DIV_SYS_DISABLE;
    }

    return 0;
}

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_sys_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = senscmu->SYS_CLK_ENABLE;

    if (sys_clk & SENS_CMU_SEL_PLL_SYS_ENABLE) {
        if (sys_clk & SENS_CMU_BYPASS_DIV_SYS_ENABLE) {
            return HAL_CMU_FREQ_192M;
        } else {
            div = GET_BITFIELD(senscmu->SYS_DIV, SENS_CMU_CFG_DIV_SYS);
            if (div == 0) {
                return HAL_CMU_FREQ_96M;
            } else if (div == 1) {
                // (div == 1): 64M
                return HAL_CMU_FREQ_78M;
            } else {
                // (div == 2): 52M
                // (div == 3): 42M
                return HAL_CMU_FREQ_48M;
            }
        }
    } else if ((sys_clk & (SENS_CMU_SEL_FAST_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE)) ==
            (SENS_CMU_SEL_FAST_SYS_ENABLE)) {
        return HAL_CMU_FREQ_96M;
    } else if ((sys_clk & (SENS_CMU_SEL_FAST_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE)) ==
            (SENS_CMU_SEL_FAST_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE)) {
        return HAL_CMU_FREQ_48M;
    } else if (sys_clk & SENS_CMU_SEL_SLOW_SYS_ENABLE) {
        return HAL_CMU_FREQ_24M;
    } else {
        return HAL_CMU_FREQ_32K;
    }
}

int hal_cmu_sys_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_audio_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll)
{
    return !!(senscmu->SYS_CLK_ENABLE & SENS_CMU_EN_PLL_ENABLE);
}

int hal_cmu_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t lock;
    uint32_t start;
    uint32_t timeout;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    lock = int_lock();
#ifdef RC48M_ENABLE
    uint32_t k;

    for (k = 0; k < HAL_CMU_PLL_QTY; k++) {
        if (pll_user_map[k]) {
            break;
        }
    }
    if (k >= HAL_CMU_PLL_QTY) {
        // Enable OSC by pu OSCX2
        senscmu->SYS_CLK_ENABLE = SENS_CMU_PU_OSCX2_ENABLE;
        hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
    }
#endif
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        senscmu->SYS_CLK_ENABLE = SENS_CMU_PU_PLL_ENABLE;
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
        if (senscmu->SYS_CLK_ENABLE & SENS_CMU_EN_PLL_ENABLE) {
            break;
        }
    } while ((hal_sys_timer_get() - start) < timeout);

    senscmu->SYS_CLK_ENABLE = SENS_CMU_EN_PLL_ENABLE;
    aoncmu->TOP_CLK1_ENABLE = AON_CMU_EN_CLK_PLLBB_SENS_ENABLE;

    return 0;
}

int hal_cmu_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t lock;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    lock = int_lock();
    if (user < HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] &= ~(1 << user);
    }
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        aoncmu->TOP_CLK1_DISABLE = AON_CMU_EN_CLK_PLLBB_SENS_DISABLE;
        senscmu->SYS_CLK_DISABLE = SENS_CMU_EN_PLL_DISABLE;
        senscmu->SYS_CLK_DISABLE = SENS_CMU_PU_PLL_DISABLE;
    }
#ifdef RC48M_ENABLE
    uint32_t k;

    for (k = 0; k < HAL_CMU_PLL_QTY; k++) {
        if (pll_user_map[k]) {
            break;
        }
    }
    if (k >= HAL_CMU_PLL_QTY) {
        // Disable OSC by pd OSCX2
        senscmu->SYS_CLK_DISABLE = SENS_CMU_PU_OSCX2_DISABLE;
    }
#endif
    int_unlock(lock);

    return 0;
}

void hal_cmu_low_freq_mode_enable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T low_freq;

    ASSERT(new_freq <= HAL_CMU_FREQ_48M, "sensor hub new_freq can not > 48MHz");

    pll = HAL_CMU_PLL_BB;
#ifdef OSC_26M_X4_AUD2BB
    low_freq = HAL_CMU_FREQ_96M;
#else
    low_freq = HAL_CMU_FREQ_48M;
#endif

    if (old_freq > low_freq && new_freq <= low_freq) {
        hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_SYS);
    }
#if defined(OSC_26M_X4_AUD2BB) && defined(RC48M_ENABLE)
    if (IS_X4_FREQ(old_freq) && !IS_X4_FREQ(new_freq)) {
        hal_cmu_osc_x4_disable_internal();
    }
#endif
}

void hal_cmu_low_freq_mode_disable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T low_freq;

    ASSERT(new_freq <= HAL_CMU_FREQ_48M, "sensor hub new_freq can not > 48MHz");

    pll = HAL_CMU_PLL_BB;
#ifdef OSC_26M_X4_AUD2BB
    low_freq = HAL_CMU_FREQ_96M;
#else
    low_freq = HAL_CMU_FREQ_48M;
#endif

    if (old_freq <= low_freq && new_freq > low_freq) {
        hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_SYS);
    }
#if defined(OSC_26M_X4_AUD2BB) && defined(RC48M_ENABLE)
    if (!IS_X4_FREQ(old_freq) && IS_X4_FREQ(new_freq)) {
        hal_cmu_osc_x4_enable_internal();
    }
#endif
}

void hal_cmu_init_pll_selection(void)
{
#if !defined(ULTRA_LOW_POWER) && !defined(OSC_26M_X4_AUD2BB)
    enum HAL_CMU_PLL_T sys;

    sys = HAL_CMU_PLL_BB;

    hal_cmu_pll_enable(sys, HAL_CMU_PLL_USER_SYS);
#endif
}

enum HAL_CMU_PLL_T hal_cmu_get_audio_pll(void)
{
    return HAL_CMU_PLL_BB;
}

void hal_cmu_codec_clock_enable(void)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;

#if defined(CODEC_VAD_CFG_BUF_SIZE)
    hal_cmu_clock_enable(HAL_CMU_MOD_H_VAD);
#endif
    hal_cmu_clock_enable(HAL_CMU_MOD_O_ADC_FREE);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_ADC_CH0);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_ADC_CH1);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_ADC_ANA);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_SADC_ANA);

    mask = 0;
    val = SENS_CMU_POL_ADC_ANA | SENS_CMU_POL_SARADC_ANA;
#ifdef CODEC_CLK_FROM_ANA
    mask |= SENS_CMU_SEL_CODEC_CLK_OSC;
#else
    val |=  SENS_CMU_SEL_CODEC_CLK_OSC;
#endif

    lock = int_lock();
    senscmu->I2C_CLK = (senscmu->I2C_CLK & ~mask) | val;
    int_unlock(lock);
}

void hal_cmu_codec_clock_disable(void)
{
    hal_cmu_clock_disable(HAL_CMU_MOD_O_ADC_FREE);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_ADC_CH0);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_ADC_CH1);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_ADC_ANA);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_SADC_ANA);
#if defined(CODEC_VAD_CFG_BUF_SIZE)
    hal_cmu_clock_disable(HAL_CMU_MOD_H_VAD);
#endif
}

void hal_cmu_codec_vad_clock_enable(int type)
{
    aoncmu->TOP_CLK1_ENABLE = AON_CMU_EN_CLK_32K_SENS_ENABLE;
    if (type == AUD_VAD_TYPE_DIG) {
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_FREE, HAL_CMU_CLK_MANUAL);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_CH0,  HAL_CMU_CLK_MANUAL);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_CH1,  HAL_CMU_CLK_MANUAL);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_ANA,  HAL_CMU_CLK_MANUAL);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_SADC_ANA, HAL_CMU_CLK_MANUAL);
    } else {
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_FREE, HAL_CMU_CLK_AUTO);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_CH0,  HAL_CMU_CLK_AUTO);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_CH1,  HAL_CMU_CLK_AUTO);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_ANA,  HAL_CMU_CLK_AUTO);
        hal_cmu_clock_set_mode(HAL_CMU_MOD_O_SADC_ANA, HAL_CMU_CLK_AUTO);
    }
}

void hal_cmu_codec_vad_clock_disable(int type)
{
    hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_FREE, HAL_CMU_CLK_MANUAL);
    hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_CH0,  HAL_CMU_CLK_MANUAL);
    hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_CH1,  HAL_CMU_CLK_MANUAL);
    hal_cmu_clock_set_mode(HAL_CMU_MOD_O_ADC_ANA,  HAL_CMU_CLK_MANUAL);
    hal_cmu_clock_set_mode(HAL_CMU_MOD_O_SADC_ANA, HAL_CMU_CLK_MANUAL);
}

void hal_cmu_codec_reset_set(void)
{
#if defined(CODEC_VAD_CFG_BUF_SIZE)
    hal_cmu_reset_set(HAL_CMU_MOD_H_VAD);
#endif
    hal_cmu_reset_set(HAL_CMU_MOD_O_ADC_FREE);
    hal_cmu_reset_set(HAL_CMU_MOD_O_ADC_CH0);
    hal_cmu_reset_set(HAL_CMU_MOD_O_ADC_CH1);
    hal_cmu_reset_set(HAL_CMU_MOD_O_ADC_ANA);
    hal_cmu_reset_set(HAL_CMU_MOD_O_SADC_ANA);
}

void hal_cmu_codec_reset_clear(void)
{
    hal_cmu_reset_clear(HAL_CMU_MOD_O_ADC_FREE);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_ADC_CH0);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_ADC_CH1);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_ADC_ANA);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_SADC_ANA);
#if defined(CODEC_VAD_CFG_BUF_SIZE)
    hal_cmu_reset_clear(HAL_CMU_MOD_H_VAD);
#endif
}

void hal_cmu_codec_set_fault_mask2(uint32_t msk0, uint32_t msk1)
{
    uint32_t lock;

    lock = int_lock();
    // If bit set 1, DAC will be muted when some faults occur
    senscmu->CLK_OUT = SET_BITFIELD(senscmu->CLK_OUT, SENS_CMU_MASK_OBS, msk0);
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_EN_CLK_I2S_OUT;

    lock = int_lock();
    senscmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_EN_CLK_I2S_OUT;

    lock = int_lock();
    senscmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_slave_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_SEL_I2S_CLKIN;

    lock = int_lock();
    senscmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_master_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_SEL_I2S_CLKIN;

    lock = int_lock();
    senscmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    senscmu->UART_CLK |= SENS_CMU_EN_CLK_PLL_I2S0;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    senscmu->UART_CLK &= ~SENS_CMU_EN_CLK_PLL_I2S0;
    int_unlock(lock);
}

void hal_cmu_i2s_resample_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_SEL_I2S0_OSC;

    lock = int_lock();
    senscmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_resample_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_SEL_I2S0_OSC;

    lock = int_lock();
    senscmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_select_external(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_SEL_I2S0_IN;

    lock = int_lock();
    senscmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_select_internal(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    val = SENS_CMU_SEL_I2S0_IN;

    lock = int_lock();
    senscmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

int hal_cmu_i2s_set_div(enum HAL_I2S_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (SENS_CMU_CFG_DIV_I2S0_MASK >> SENS_CMU_CFG_DIV_I2S0_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    senscmu->UART_CLK = SET_BITFIELD(senscmu->UART_CLK, SENS_CMU_CFG_DIV_I2S0, div);
    int_unlock(lock);

    return 0;
}

void hal_cmu_apb_init_div(void)
{
    // Divider defaults to 2 (reg_val = div - 2)
    //senscmu->SYS_DIV = SET_BITFIELD(senscmu->SYS_DIV, SENS_CMU_CFG_DIV_PCLK, 0);
}

#define PERPH_SET_FREQ_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (freq == HAL_CMU_PERIPH_FREQ_26M) { \
        senscmu->r &= ~(SENS_CMU_SEL_OSCX2_ ##F); \
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) { \
        senscmu->r |= (SENS_CMU_SEL_OSCX2_ ##F); \
    } else { \
        ret = 1; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_FREQ_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_FREQ_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_FREQ_FUNC(uart2, UART2, UART_CLK);
PERPH_SET_FREQ_FUNC(spi, SPI0, SYS_DIV);
PERPH_SET_FREQ_FUNC(slcd, SPI1, SYS_DIV);
PERPH_SET_FREQ_FUNC(i2c, I2C, I2C_CLK);

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        senscmu->SYS_DIV &= ~SENS_CMU_SEL_OSCX2_SPI2;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        senscmu->SYS_DIV |= SENS_CMU_SEL_OSCX2_SPI2;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

int hal_cmu_i2s_mclk_enable(enum HAL_CMU_I2S_MCLK_ID_T id)
{
    uint32_t lock;

    lock = iomux_lock();
    senscmu->I2C_CLK = SET_BITFIELD(senscmu->I2C_CLK, SENS_CMU_SEL_I2S_MCLK, id) | SENS_CMU_EN_I2S_MCLK;
    aoncmu->AON_CLK = (aoncmu->AON_CLK & ~AON_CMU_SEL_I2S_MCLK_MASK) |
        AON_CMU_SEL_I2S_MCLK(SENS_CMU_I2S_MCLK_SEL_SENS) | AON_CMU_EN_I2S_MCLK;
    iomux_unlock(lock);

    return 0;
}

void hal_cmu_i2s_mclk_disable(void)
{
    uint32_t lock;

    lock = iomux_lock();
    senscmu->I2C_CLK &= ~SENS_CMU_EN_I2S_MCLK;
    aoncmu->AON_CLK &= ~AON_CMU_EN_I2S_MCLK;
    iomux_unlock(lock);
}

void hal_cmu_mcu_pdm_clock_out(uint32_t clk_map)
{
    uint32_t lock;
    uint32_t val = 0;

    if (clk_map & (1 << 0)) {
        val |= AON_CMU_SEL_PDM_CLKOUT0_SENS;
    }
    if (clk_map & (1 << 1)) {
        val |= AON_CMU_SEL_PDM_CLKOUT1_SENS;
    }

    lock = iomux_lock();
    aoncmu->AON_CLK &= ~val;
    iomux_unlock(lock);
}

void hal_cmu_sens_pdm_clock_out(uint32_t clk_map)
{
    uint32_t lock;
    uint32_t val = 0;

    if (clk_map & (1 << 0)) {
        val |= AON_CMU_SEL_PDM_CLKOUT0_SENS;
    }
    if (clk_map & (1 << 1)) {
        val |= AON_CMU_SEL_PDM_CLKOUT1_SENS;
    }

    lock = iomux_lock();
    aoncmu->AON_CLK |= val;
    iomux_unlock(lock);
}

void hal_cmu_mcu_pdm_data_in(uint32_t data_map)
{
    uint32_t lock;
    uint32_t val = 0;

    if (data_map & (1 << 0)) {
        val &= ~AON_CMU_SEL_PDM_DATAIN0_SYS;
    }

    lock = iomux_lock();
    aoncmu->AON_CLK |= val;
    iomux_unlock(lock);
}

void hal_cmu_sens_pdm_data_in(uint32_t data_map)
{
    uint32_t lock;
    uint32_t val = 0;

    if (data_map & (1 << 0)) {
        val |= AON_CMU_SEL_PDM_DATAIN0_SYS;
    }

    lock = iomux_lock();
    aoncmu->AON_CLK &= ~val;
    iomux_unlock(lock);
}

void hal_cmu_osc_x2_enable(void)
{
#ifndef RC48M_ENABLE
    senscmu->SYS_CLK_ENABLE = SENS_CMU_PU_OSCX2_ENABLE;
    hal_sys_timer_delay(US_TO_TICKS(60));
#endif
    senscmu->SYS_CLK_ENABLE = SENS_CMU_EN_OSCX2_ENABLE;
}

void hal_cmu_osc_x4_enable_internal(void)
{
#ifdef ANA_26M_X4_ENABLE
    senscmu->SYS_CLK_ENABLE = SENS_CMU_PU_OSCX4_ENABLE;
#ifdef RC48M_ENABLE
    hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
#else
    hal_sys_timer_delay(US_TO_TICKS(60));
#endif
    senscmu->SYS_CLK_ENABLE = SENS_CMU_EN_OSCX4_ENABLE;
#endif
}

void hal_cmu_osc_x4_disable_internal(void)
{
#ifdef ANA_26M_X4_ENABLE
    senscmu->SYS_CLK_DISABLE = SENS_CMU_EN_OSCX4_DISABLE;
    senscmu->SYS_CLK_DISABLE = SENS_CMU_PU_OSCX4_DISABLE;
#endif
}

void hal_cmu_osc_x4_enable(void)
{
#ifndef RC48M_ENABLE
    hal_cmu_osc_x4_enable_internal();
#endif
}

void hal_cmu_module_init_state(void)
{
    hal_psc_core_init_ram_ret_mask();

    // DMA channel config
    senscmu->ADMA_CH0_4_REQ =
        // i2c0
        SENS_CMU_SDMA_CH0_REQ_IDX(SENS_DMA_REQ_I2C0_RX) | SENS_CMU_SDMA_CH1_REQ_IDX(SENS_DMA_REQ_I2C0_TX) |
        // i2c1
        SENS_CMU_SDMA_CH2_REQ_IDX(SENS_DMA_REQ_I2C1_RX) | SENS_CMU_SDMA_CH3_REQ_IDX(SENS_DMA_REQ_I2C1_TX) |
        // i2c2
        SENS_CMU_SDMA_CH4_REQ_IDX(SENS_DMA_REQ_I2C2_RX);
    senscmu->ADMA_CH5_9_REQ =
        // i2c2
        SENS_CMU_SDMA_CH5_REQ_IDX(SENS_DMA_REQ_I2C2_TX) |
        // spi0
        SENS_CMU_SDMA_CH6_REQ_IDX(SENS_DMA_REQ_SPILCD0_RX) | SENS_CMU_SDMA_CH7_REQ_IDX(SENS_DMA_REQ_SPILCD0_TX) |
        // spi1
        SENS_CMU_SDMA_CH8_REQ_IDX(SENS_DMA_REQ_SPILCD1_RX) | SENS_CMU_SDMA_CH9_REQ_IDX(SENS_DMA_REQ_SPILCD1_TX);
    senscmu->ADMA_CH10_14_REQ =
        //i2s0
        SENS_CMU_SDMA_CH10_REQ_IDX(SENS_DMA_REQ_I2S0_RX) | SENS_CMU_SDMA_CH11_REQ_IDX(SENS_DMA_REQ_I2S0_TX) |
        // vad rx and uart0 tx
        SENS_CMU_SDMA_CH12_REQ_IDX(SENS_DMA_REQ_VAD_RX) | SENS_CMU_SDMA_CH13_REQ_IDX(SENS_DMA_REQ_UART0_TX) |
#if defined(SENS_DMA_UART0_RX_EN)
        // uart0
        SENS_CMU_SDMA_CH14_REQ_IDX(SENS_DMA_REQ_UART0_RX);
#else
        // uart1
        SENS_CMU_SDMA_CH14_REQ_IDX(SENS_DMA_REQ_UART1_RX);
#endif
    senscmu->ADMA_CH15_REQ =
        // uart1
        SET_BITFIELD(senscmu->ADMA_CH15_REQ, SENS_CMU_SDMA_CH15_REQ_IDX, SENS_DMA_REQ_UART1_TX);

#if 1 //ndef SIMU
    senscmu->ORESET_SET = SENS_ORST_I2C0 | SENS_ORST_I2C1 | SENS_ORST_I2C2 | SENS_ORST_I2C3 |
        SENS_ORST_I2C4 | SENS_ORST_SPI0 | SENS_ORST_SPI1 |
        SENS_ORST_UART0 | SENS_ORST_UART1 | SENS_ORST_UART2 | SENS_ORST_I2S |
        SENS_ORST_ADC_FREE | SENS_ORST_ADC_CH0 | SENS_ORST_ADC_CH1 |
        SENS_ORST_ADC_ANA | SENS_ORST_SADC_ANA | SENS_ORST_CAL32K | SENS_ORST_CAL26M |
#ifndef SUBSYS_WDT_ENABLE
        SENS_ORST_WDT |
#endif
#ifdef NO_ISPI
        SENS_ORST_SPI_ITN |
#endif
        0;
    senscmu->PRESET_SET = SENS_PRST_I2C0 | SENS_PRST_I2C1 | SENS_PRST_I2C2 | SENS_PRST_I2C3 |
        SENS_PRST_I2C4 | SENS_PRST_SPI0 | SENS_PRST_SPI1 | SENS_PRST_UART0 | SENS_PRST_UART1 |
        SENS_PRST_UART2 | SENS_PRST_TCNT | SENS_PRST_I2S0 |
#ifndef SUBSYS_WDT_ENABLE
        SENS_PRST_WDT |
#endif
#ifdef NO_ISPI
        SENS_PRST_SPI_ITN |
#endif
        0;
    senscmu->HRESET_SET = SENS_HRST_SENSOR_ENG0 | SENS_HRST_SENSOR_ENG1 |
        SENS_HRST_SENSOR_ENG2 | SENS_HRST_SENSOR_ENG3 | SENS_HRST_SPI_AHB;
#if defined(CODEC_VAD_CFG_BUF_SIZE)
    senscmu->HRESET_SET = SENS_HRST_VAD;
#endif

    senscmu->OCLK_DISABLE = SENS_OCLK_I2C0 | SENS_OCLK_I2C1 | SENS_OCLK_I2C2 | SENS_OCLK_I2C3 |
        SENS_OCLK_I2C4 | SENS_OCLK_SPI0 | SENS_OCLK_SPI1 |
        SENS_OCLK_UART0 | SENS_OCLK_UART1 | SENS_OCLK_UART2 | SENS_OCLK_I2S |
        SENS_OCLK_ADC_FREE | SENS_OCLK_ADC_CH0 | SENS_OCLK_ADC_CH1 |
        SENS_OCLK_ADC_ANA | SENS_OCLK_SADC_ANA | SENS_OCLK_CAL32K | SENS_OCLK_CAL26M |
#ifndef SUBSYS_WDT_ENABLE
        SENS_OCLK_WDT |
#endif
#ifdef NO_ISPI
        SENS_OCLK_SPI_ITN |
#endif
        0;
    senscmu->PCLK_DISABLE = SENS_PCLK_I2C0 | SENS_PCLK_I2C1 | SENS_PCLK_I2C2 | SENS_PCLK_I2C3 |
        SENS_PCLK_I2C4 | SENS_PCLK_SPI0 | SENS_PCLK_SPI1 | SENS_PCLK_UART0 | SENS_PCLK_UART1 |
        SENS_PCLK_UART2 | SENS_PCLK_TCNT | SENS_PCLK_I2S0 |
#ifndef SUBSYS_WDT_ENABLE
        SENS_PCLK_WDT |
#endif
#ifdef NO_ISPI
        SENS_PCLK_SPI_ITN |
#endif
        0;
    senscmu->HCLK_DISABLE = SENS_HCLK_SENSOR_ENG0 | SENS_HCLK_SENSOR_ENG1 |
        SENS_HCLK_SENSOR_ENG2 | SENS_HCLK_SENSOR_ENG3 | SENS_HCLK_SPI_AHB;
#if defined(CODEC_VAD_CFG_BUF_SIZE)
    senscmu->HCLK_DISABLE = SENS_HCLK_VAD;
#endif

    //senscmu->HCLK_MODE = 0;
    //senscmu->PCLK_MODE = SENS_PCLK_UART0 | SENS_PCLK_UART1 | SENS_PCLK_UART2;
    //senscmu->OCLK_MODE = 0;

    uint32_t mask;

    mask = SENS_CMU_SENS_ENG0_SLEEP_MASK | SENS_CMU_SENS_ENG1_SLEEP_MASK |
        SENS_CMU_SENS_ENG2_SLEEP_MASK | SENS_CMU_SENS_ENG3_SLEEP_MASK;
#ifdef SENSOR_ENG_AUTO_SLEEP
    senscmu->ADMA_CH15_REQ &= ~mask;
#else
    senscmu->ADMA_CH15_REQ |= mask;
#endif
#endif

#ifdef CPU_PC_DUMP
    hal_cpudump_enable();
#endif
}

void hal_cmu_lpu_wait_26m_ready(void)
{
    while ((senscmu->WAKEUP_CLK_CFG & (SENS_CMU_LPU_AUTO_SWITCH26 | SENS_CMU_LPU_STATUS_26M)) ==
            SENS_CMU_LPU_AUTO_SWITCH26);
}

int hal_cmu_lpu_busy(void)
{
    if ((senscmu->WAKEUP_CLK_CFG & (SENS_CMU_LPU_AUTO_SWITCH26 | SENS_CMU_LPU_STATUS_26M)) ==
            SENS_CMU_LPU_AUTO_SWITCH26) {
        return 1;
    }
    if ((senscmu->WAKEUP_CLK_CFG & (SENS_CMU_LPU_AUTO_SWITCHPLL | SENS_CMU_LPU_STATUS_PLL)) ==
            SENS_CMU_LPU_AUTO_SWITCHPLL) {
        return 1;
    }
    return 0;
}

int hal_cmu_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg)
{
    uint32_t lpu_clk;
    uint32_t timer_26m;
    uint32_t timer_pll;

    timer_26m = LPU_TIMER_US(TICKS_TO_US(HAL_CMU_26M_READY_TIMEOUT));
    timer_pll = LPU_TIMER_US(TICKS_TO_US(HAL_CMU_PLL_LOCKED_TIMEOUT));

    if (cfg >= HAL_CMU_LPU_CLK_QTY) {
        return 1;
    }
    if ((timer_26m & (SENS_CMU_TIMER_WT26_MASK >> SENS_CMU_TIMER_WT26_SHIFT)) != timer_26m) {
        return 2;
    }
    if ((timer_pll & (SENS_CMU_TIMER_WTPLL_MASK >> SENS_CMU_TIMER_WTPLL_SHIFT)) != timer_pll) {
        return 3;
    }
    if (hal_cmu_lpu_busy()) {
        return -1;
    }

    if (cfg == HAL_CMU_LPU_CLK_26M) {
        lpu_clk = SENS_CMU_LPU_AUTO_SWITCH26;
    } else if (cfg == HAL_CMU_LPU_CLK_PLL) {
        lpu_clk = SENS_CMU_LPU_AUTO_SWITCHPLL | SENS_CMU_LPU_AUTO_SWITCH26;
    } else {
        lpu_clk = 0;
    }

    if (lpu_clk & SENS_CMU_LPU_AUTO_SWITCH26) {
        // Disable RAM wakeup early
        senscmu->SENS_TIMER &= ~SENS_CMU_RAM_RETN_UP_EARLY;
        // MCU/ROM/RAM auto clock gating (which depends on RAM gating signal)
        senscmu->HCLK_MODE &= ~(SENS_HCLK_MCU | SENS_HCLK_RAM0 | SENS_HCLK_RAM1 | SENS_HCLK_RAM2 | SENS_HCLK_RAM3 |
            SENS_HCLK_RAM4 | SENS_HCLK_RAM5);
    }

    senscmu->WAKEUP_CLK_CFG = SENS_CMU_TIMER_WT26(timer_26m) | SENS_CMU_TIMER_WTPLL(0) | lpu_clk;
    if (timer_pll) {
        hal_sys_timer_delay(US_TO_TICKS(60));
        senscmu->WAKEUP_CLK_CFG = SENS_CMU_TIMER_WT26(timer_26m) | SENS_CMU_TIMER_WTPLL(timer_pll) | lpu_clk;
    }
    return 0;
}

#ifdef CORE_SLEEP_POWER_DOWN
static void save_senscmu_regs(struct SAVED_SENSCMU_REGS_T *sav)
{
    sav->HCLK_ENABLE            = senscmu->HCLK_ENABLE;
    sav->PCLK_ENABLE            = senscmu->PCLK_ENABLE;
    sav->OCLK_ENABLE            = senscmu->OCLK_ENABLE;
    sav->HCLK_MODE              = senscmu->HCLK_MODE;
    sav->PCLK_MODE              = senscmu->PCLK_MODE;
    sav->OCLK_MODE              = senscmu->OCLK_MODE;
    sav->HRESET_CLR             = senscmu->HRESET_CLR;
    sav->PRESET_CLR             = senscmu->PRESET_CLR;
    sav->ORESET_CLR             = senscmu->ORESET_CLR;
    sav->REMAP_CODEC2BTH        = senscmu->REMAP_CODEC2BTH;
    sav->SENS_TIMER             = senscmu->SENS_TIMER;
    sav->SLEEP                  = senscmu->SLEEP;
    sav->CLK_OUT                = senscmu->CLK_OUT;
    sav->SYS_CLK_ENABLE         = senscmu->SYS_CLK_ENABLE;
    sav->ADMA_CH15_REQ          = senscmu->ADMA_CH15_REQ;
    sav->UART_CLK               = senscmu->UART_CLK;
    sav->I2C_CLK                = senscmu->I2C_CLK;
    sav->SENS2MCU_MASK0         = senscmu->SENS2MCU_MASK0;
    sav->SENS2MCU_MASK1         = senscmu->SENS2MCU_MASK1;
    sav->WAKEUP_CLK_CFG         = senscmu->WAKEUP_CLK_CFG;
    sav->CPU_CFG                = senscmu->CPU_CFG;
    sav->OSC_EXT_TIMER          = senscmu->OSC_EXT_TIMER;
    sav->SYS_DIV                = senscmu->SYS_DIV;
    sav->SLEEP_TARGET           = senscmu->SLEEP_TARGET;
    sav->SENS2BT_INTMASK0       = senscmu->SENS2BT_INTMASK0;
    sav->SENS2BT_INTMASK1       = senscmu->SENS2BT_INTMASK1;
    sav->CALIB_TARGET           = senscmu->CALIB_TARGET;
    sav->ADMA_CH0_4_REQ         = senscmu->ADMA_CH0_4_REQ;
    sav->ADMA_CH5_9_REQ         = senscmu->ADMA_CH5_9_REQ;
    sav->ADMA_CH10_14_REQ       = senscmu->ADMA_CH10_14_REQ;
    sav->RAM_ECC_CTRL           = senscmu->RAM_ECC_CTRL;
    sav->ECC0_ERR_ADDR          = senscmu->ECC0_ERR_ADDR;
    sav->ECC1_ERR_ADDR          = senscmu->ECC1_ERR_ADDR;
    sav->CALIB_TIMER_VAL        = senscmu->CALIB_TIMER_VAL;
    sav->MISC                   = senscmu->MISC;
    sav->SIMU_RES               = senscmu->SIMU_RES;
    sav->TIMER32K_VAL           = senscmu->TIMER32K_VAL;
}

static void restore_senscmu_regs(const struct SAVED_SENSCMU_REGS_T *sav)
{
    senscmu->HRESET_SET         = ~sav->HRESET_CLR;
    senscmu->PRESET_SET         = ~sav->PRESET_CLR;
    senscmu->ORESET_SET         = ~sav->ORESET_CLR;
    senscmu->HCLK_DISABLE       = ~sav->HCLK_ENABLE;
    senscmu->PCLK_DISABLE       = ~sav->PCLK_ENABLE;
    senscmu->OCLK_DISABLE       = ~sav->OCLK_ENABLE;

    senscmu->HCLK_ENABLE        = sav->HCLK_ENABLE;
    senscmu->PCLK_ENABLE        = sav->PCLK_ENABLE;
    senscmu->OCLK_ENABLE        = sav->OCLK_ENABLE;
    senscmu->HCLK_MODE          = sav->HCLK_MODE;
    senscmu->PCLK_MODE          = sav->PCLK_MODE;
    senscmu->OCLK_MODE          = sav->OCLK_MODE;
    senscmu->HRESET_CLR         = sav->HRESET_CLR;
    senscmu->PRESET_CLR         = sav->PRESET_CLR;
    senscmu->ORESET_CLR         = sav->ORESET_CLR;
    senscmu->REMAP_CODEC2BTH    = sav->REMAP_CODEC2BTH;
    senscmu->SENS_TIMER         = sav->SENS_TIMER;
    senscmu->SLEEP              = sav->SLEEP;
    senscmu->CLK_OUT            = sav->CLK_OUT;
    senscmu->SYS_CLK_ENABLE     = sav->SYS_CLK_ENABLE;
    senscmu->ADMA_CH15_REQ      = sav->ADMA_CH15_REQ;
    senscmu->UART_CLK           = sav->UART_CLK;
    senscmu->I2C_CLK            = sav->I2C_CLK;
    senscmu->SENS2MCU_MASK0     = sav->SENS2MCU_MASK0;
    senscmu->SENS2MCU_MASK1     = sav->SENS2MCU_MASK1;
    senscmu->WAKEUP_CLK_CFG     = sav->WAKEUP_CLK_CFG;
    senscmu->CPU_CFG            = sav->CPU_CFG;
    senscmu->OSC_EXT_TIMER      = sav->OSC_EXT_TIMER;
    senscmu->SYS_DIV            = sav->SYS_DIV;
    senscmu->SLEEP_TARGET       = sav->SLEEP_TARGET;
    senscmu->SENS2BT_INTMASK0   = sav->SENS2BT_INTMASK0;
    senscmu->SENS2BT_INTMASK1   = sav->SENS2BT_INTMASK1;
    senscmu->CALIB_TARGET       = sav->CALIB_TARGET;
    senscmu->ADMA_CH0_4_REQ     = sav->ADMA_CH0_4_REQ;
    senscmu->ADMA_CH5_9_REQ     = sav->ADMA_CH5_9_REQ;
    senscmu->ADMA_CH10_14_REQ   = sav->ADMA_CH10_14_REQ;
    senscmu->RAM_ECC_CTRL       = sav->RAM_ECC_CTRL;
    senscmu->ECC0_ERR_ADDR      = sav->ECC0_ERR_ADDR;
    senscmu->ECC1_ERR_ADDR      = sav->ECC1_ERR_ADDR;
    senscmu->CALIB_TIMER_VAL    = sav->CALIB_TIMER_VAL;
    senscmu->MISC               = sav->MISC;
    senscmu->SIMU_RES           = sav->SIMU_RES;
    senscmu->TIMER32K_VAL       = sav->TIMER32K_VAL;
}

static int hal_cmu_lpu_sleep_pd(void)
{
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_clk_cfg;
    uint32_t cpu_regs[50];
    struct SAVED_SENSCMU_REGS_T senscmu_regs;

    hal_psc_core_ram_pd_sleep();

    NVIC_PowerDownSleep(cpu_regs, ARRAY_SIZE(cpu_regs));
    save_senscmu_regs(&senscmu_regs);

    saved_clk_cfg = senscmu_regs.SYS_CLK_ENABLE;

#if defined(CODEC_VAD_CFG_BUF_SIZE) && (CODEC_VAD_CFG_BUF_SIZE > 0)
    // Switch VAD clock to AON
    // (VAD buffer will not survive power down -- stack and sleep codes cannot be located in VAD buffer)
    senscmu->I2C_CLK |= SENS_CMU_SEL_CODEC_HCLK_IN;
#endif

    // Reset sub div
    senscmu->CLK_OUT = SET_BITFIELD(senscmu->CLK_OUT, SENS_CMU_SEL_SYS_GT, 3);

    // Switch system freq to 26M
    senscmu->SYS_CLK_ENABLE = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE;
    senscmu->SYS_CLK_DISABLE = SENS_CMU_SEL_OSC_2_SYS_DISABLE | SENS_CMU_SEL_FAST_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE;
    senscmu->SYS_CLK_DISABLE = SENS_CMU_BYPASS_DIV_SYS_DISABLE;
    senscmu->SYS_CLK_DISABLE = SENS_CMU_RSTN_DIV_SYS_DISABLE;
    // Shutdown system PLL
    if (saved_clk_cfg & SENS_CMU_PU_PLL_ENABLE) {
        senscmu->SYS_CLK_DISABLE = SENS_CMU_EN_PLL_DISABLE;
        senscmu->SYS_CLK_DISABLE = SENS_CMU_PU_PLL_DISABLE;
    }

    hal_sleep_core_power_down(false);

    while ((senscmu->WAKEUP_CLK_CFG & SENS_CMU_LPU_STATUS_26M) == 0);

    // Switch system freq to 26M
    senscmu->SYS_CLK_ENABLE = SENS_CMU_SEL_SLOW_SYS_ENABLE |
        (saved_clk_cfg & (SENS_CMU_PU_OSCX2_ENABLE | SENS_CMU_PU_OSCX4_ENABLE));
    senscmu->SYS_CLK_DISABLE = SENS_CMU_SEL_OSC_2_SYS_DISABLE;

    hal_sys_timer_wakeup();

    // System freq is 26M now and will be restored later
    if (saved_clk_cfg & SENS_CMU_PU_PLL_ENABLE) {
        senscmu->SYS_CLK_ENABLE = SENS_CMU_PU_PLL_ENABLE;
        start = hal_sys_timer_get();
        timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
        while ((hal_sys_timer_get() - start) < timeout);
        senscmu->SYS_CLK_ENABLE = SENS_CMU_EN_PLL_ENABLE;
    }

    // Restore system freq
    senscmu->SYS_CLK_ENABLE = saved_clk_cfg & (SENS_CMU_RSTN_DIV_SYS_ENABLE);
    senscmu->SYS_CLK_ENABLE = saved_clk_cfg & (SENS_CMU_BYPASS_DIV_SYS_ENABLE);
    senscmu->SYS_CLK_ENABLE = saved_clk_cfg;
    senscmu->SYS_CLK_DISABLE = ~saved_clk_cfg;

#if defined(CODEC_VAD_CFG_BUF_SIZE) && (CODEC_VAD_CFG_BUF_SIZE > 0)
    // Switch VAD clock to SENS
    senscmu->I2C_CLK &= ~SENS_CMU_SEL_CODEC_HCLK_IN;
#endif

    restore_senscmu_regs(&senscmu_regs);
    NVIC_PowerDownWakeup(cpu_regs, ARRAY_SIZE(cpu_regs));

    hal_psc_core_ram_pd_wakeup();

    // TODO:
    // 1) Register pm notif handler for all hardware modules, e.g., sdmmc
    // 2) Recover system timer in rt_suspend() and rt_resume()
    // 3) Dynamically select 32K sleep or power down sleep

    return 0;
}

#endif

static int hal_cmu_lpu_sleep_normal(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_clk_cfg;
    uint32_t saved_clk_out;
    uint32_t wakeup_cfg;
#ifdef CORE_SLEEP_POWER_DOWN
    uint32_t cpu_regs[50];
#endif

    hal_psc_core_ram_ret_sleep();

    saved_clk_cfg = senscmu->SYS_CLK_ENABLE;
    saved_clk_out = senscmu->CLK_OUT;

    if (0) {
#ifdef CORE_SLEEP_POWER_DOWN
    } else if (mode == HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K) {
        wakeup_cfg = senscmu->WAKEUP_CLK_CFG;
        ASSERT(wakeup_cfg & (SENS_CMU_LPU_AUTO_SWITCHPLL | SENS_CMU_LPU_AUTO_SWITCH26),
            "%s: Bad LPU mode: 0x%08X", __func__, wakeup_cfg);
#endif
    } else if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        wakeup_cfg = senscmu->WAKEUP_CLK_CFG;
    } else {
        wakeup_cfg = 0;
    }

    // Setup wakeup mask
    senscmu->WAKEUP_MASK0 = NVIC->ISER[0];
    senscmu->WAKEUP_MASK1 = NVIC->ISER[1];

    if (wakeup_cfg & (SENS_CMU_LPU_AUTO_SWITCHPLL | SENS_CMU_LPU_AUTO_SWITCH26)) {
        // Enable auto memory retention
        senscmu->SLEEP = (senscmu->SLEEP & ~SENS_CMU_MANUAL_RAM_RETN) |
            SENS_CMU_DEEPSLEEP_EN | SENS_CMU_DEEPSLEEP_ROMRAM_EN | SENS_CMU_DEEPSLEEP_START;
    } else {
        // Disable auto memory retention
        senscmu->SLEEP = (senscmu->SLEEP & ~SENS_CMU_DEEPSLEEP_ROMRAM_EN) |
            SENS_CMU_DEEPSLEEP_EN | SENS_CMU_MANUAL_RAM_RETN | SENS_CMU_DEEPSLEEP_START;
    }

    if (wakeup_cfg & SENS_CMU_LPU_AUTO_SWITCHPLL) {
        // Do nothing
        // Hardware will switch system freq to 32K and shutdown PLLs automatically
    } else {
        // Switch system freq to 26M
        senscmu->SYS_CLK_ENABLE = SENS_CMU_SEL_SLOW_SYS_ENABLE | SENS_CMU_SEL_OSCX2_SYS_ENABLE;
        senscmu->SYS_CLK_DISABLE = SENS_CMU_SEL_OSC_2_SYS_DISABLE | SENS_CMU_SEL_FAST_SYS_DISABLE | SENS_CMU_SEL_PLL_SYS_DISABLE;
        senscmu->SYS_CLK_DISABLE = SENS_CMU_BYPASS_DIV_SYS_DISABLE;
        senscmu->SYS_CLK_DISABLE = SENS_CMU_RSTN_DIV_SYS_DISABLE;
        // Shutdown system PLL
        if (saved_clk_cfg & SENS_CMU_PU_PLL_ENABLE) {
            senscmu->SYS_CLK_DISABLE = SENS_CMU_EN_PLL_DISABLE;
            senscmu->SYS_CLK_DISABLE = SENS_CMU_PU_PLL_DISABLE;
        }
        if (wakeup_cfg & SENS_CMU_LPU_AUTO_SWITCH26) {
            // Do nothing
            // Hardware will switch system/memory/flash freq to 32K automatically
        } else {
            // Switch system freq to 32K
            senscmu->SYS_CLK_DISABLE = SENS_CMU_SEL_OSC_4_SYS_DISABLE | SENS_CMU_SEL_SLOW_SYS_DISABLE;
        }
    }

#if defined(CODEC_VAD_CFG_BUF_SIZE) && (CODEC_VAD_CFG_BUF_SIZE > 0)
#if (RAM_BASE + RAM_SIZE > SENS_RAM5_BASE)
#error "VAD buffer conflicts with CPU RAM in clock ctrl"
#endif
    uint32_t i2c_clk_cfg;

    i2c_clk_cfg = senscmu->I2C_CLK;
    // Switch VAD clock to AON
    // (Avoid using VAD buffer from now on. If stack or codes are in VAD buffer, they should not be accessed either)
    senscmu->I2C_CLK |= SENS_CMU_SEL_CODEC_HCLK_IN;
    __DSB();
    __NOP();
#endif

#ifdef CORE_SLEEP_POWER_DOWN
    if (mode == HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K || mode == HAL_CMU_LPU_SLEEP_MODE_CPU_PD_BUS_32K) {
        NVIC_PowerDownSleep(cpu_regs, ARRAY_SIZE(cpu_regs));

        hal_sleep_core_power_down(true);

        NVIC_PowerDownWakeup(cpu_regs, ARRAY_SIZE(cpu_regs));
    } else
#endif
    {
        if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
            SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
        } else {
            SCB->SCR = 0;
        }
        __DSB();
        __WFI();
    }

#if defined(CODEC_VAD_CFG_BUF_SIZE) && (CODEC_VAD_CFG_BUF_SIZE > 0)
    // Switch VAD clock to SENS
    // (Wait until the clock switch finishes)
    senscmu->I2C_CLK &= ~SENS_CMU_SEL_CODEC_HCLK_IN;
    __DSB();
    __NOP();
    __NOP();
    __NOP();
    senscmu->I2C_CLK = i2c_clk_cfg;
#endif

    // Reset sub div
    senscmu->CLK_OUT = SET_BITFIELD(senscmu->CLK_OUT, SENS_CMU_SEL_SYS_GT, 3);

    if (wakeup_cfg & SENS_CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_PLL_LOCKED_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
        while ((senscmu->WAKEUP_CLK_CFG & SENS_CMU_LPU_STATUS_PLL) == 0 &&
            (hal_sys_timer_get() - start) < timeout);
        // !!! CAUTION !!!
        // Hardware will switch system freq to PLL divider and enable PLLs automatically
    } else {
        // Wait for 26M ready
        if (wakeup_cfg & SENS_CMU_LPU_AUTO_SWITCH26) {
            start = hal_sys_timer_get();
            timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
            while ((senscmu->WAKEUP_CLK_CFG & SENS_CMU_LPU_STATUS_26M) == 0 &&
                (hal_sys_timer_get() - start) < timeout);
            // Hardware will switch system/memory/flash freq to 26M automatically
        } else {
            if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
                timeout = HAL_CMU_26M_READY_TIMEOUT;
                hal_sys_timer_delay(timeout);
            }
            // Switch system freq to 26M
            senscmu->SYS_CLK_ENABLE = SENS_CMU_SEL_SLOW_SYS_ENABLE;
            senscmu->SYS_CLK_DISABLE = SENS_CMU_SEL_OSC_2_SYS_DISABLE;
        }
        // System freq is 26M now and will be restored later
        if (saved_clk_cfg & SENS_CMU_PU_PLL_ENABLE) {
            senscmu->SYS_CLK_ENABLE = SENS_CMU_PU_PLL_ENABLE;
            start = hal_sys_timer_get();
            timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
            while ((hal_sys_timer_get() - start) < timeout);
            senscmu->SYS_CLK_ENABLE = SENS_CMU_EN_PLL_ENABLE;
        }
    }

    // Restore system/memory/flash freq
    senscmu->SYS_CLK_ENABLE = saved_clk_cfg & (SENS_CMU_RSTN_DIV_SYS_ENABLE);
    senscmu->SYS_CLK_ENABLE = saved_clk_cfg & (SENS_CMU_BYPASS_DIV_SYS_ENABLE);
    senscmu->SYS_CLK_ENABLE = saved_clk_cfg;
    senscmu->SYS_CLK_DISABLE = ~saved_clk_cfg;
    senscmu->CLK_OUT = saved_clk_out;

    return 0;
}

int hal_cmu_lpu_sleep(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
#ifdef CORE_SLEEP_POWER_DOWN
    if (mode == HAL_CMU_LPU_SLEEP_MODE_POWER_DOWN) {
#if defined(SENSOR_ENG_AUTO_SLEEP)
        if (senscmu->HRESET_CLR & (SENS_HRST_SENSOR_ENG0 | SENS_HRST_SENSOR_ENG1 |
                SENS_HRST_SENSOR_ENG2 | SENS_HRST_SENSOR_ENG3)) {
            mode = HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K;
        }
#endif
    }
    if (mode == HAL_CMU_LPU_SLEEP_MODE_POWER_DOWN) {
        return hal_cmu_lpu_sleep_pd();
    }
#endif
    return hal_cmu_lpu_sleep_normal(mode);
}

void hal_cmu_set_wakeup_vector(uint32_t vector)
{
    aoncmu->SENS_VTOR = (aoncmu->SENS_VTOR & ~AON_CMU_VTOR_CORE_SENS_MASK) | (vector & AON_CMU_VTOR_CORE_SENS_MASK);
}

void BOOT_TEXT_FLASH_LOC hal_cpudump_clock_enable(void)
{
    senscmu->HCLK_ENABLE = SENS_HCLK_DUMPCPU;
    senscmu->HRESET_CLR  = SENS_HRST_DUMPCPU;
}

void hal_cpudump_clock_disable(void)
{
    senscmu->HRESET_SET   = SENS_HCLK_DUMPCPU;
    senscmu->HCLK_DISABLE = SENS_HRST_DUMPCPU;
}

uint32_t hal_cmu_cpudump_get_last_addr(void)
{
    uint32_t addr = 0x100;
    if(aoncmu->LAST_PC_INFO & AON_CMU_SENS_LAST_PC_EN)
        addr = GET_BITFIELD(aoncmu->LAST_PC_INFO, AON_CMU_SENS_LAST_PC_ADDR);

    return addr;
}

void hal_cmu_beco_enable(void)
{
}

void hal_cmu_beco_disable(void)
{
}

void hal_codec_crash_mute(void)
{
    return;
}

#endif
