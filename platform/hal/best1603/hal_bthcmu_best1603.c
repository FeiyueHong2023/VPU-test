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
#ifdef CHIP_SUBSYS_BTH

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
#include "hal_chipid.h"
#include "hal_codec.h"
#include "hal_iomux.h"
#include "hal_location.h"
#ifdef SUBSYS_FLASH_BOOT
#include "hal_norflash.h"
#endif
#include "hal_psc.h"
#include "hal_sleep_core_pd.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "cmsis_nvic.h"
#include "pmu.h"
#include "system_cp.h"
#include "hal_cpudump.h"

#ifdef USB_HIGH_SPEED
#ifndef USB_CLK_SRC_PLL
#define USB_CLK_SRC_PLL
#endif
#endif

#ifdef AUDIO_USE_USBPLL
#define AUDIO_PLL_SEL                   HAL_CMU_PLL_USB
#else
#define AUDIO_PLL_SEL                   HAL_CMU_PLL_BB
#endif

#ifdef CORE_SLEEP_POWER_DOWN
#define TIMER1_SEL_LOC                  BOOT_TEXT_SRAM_LOC
#else
#define TIMER1_SEL_LOC                  BOOT_TEXT_FLASH_LOC
#endif

#define CMU_RES1_EMMC_IRQ_S43_C0        (1 << 0)
#define CMU_RES1_FPU_IRQ_S0             (1 << 1)
#define CMU_RES1_GPU_IRQ_S49_C8         (1 << 2)
#define CMU_RES1_WAKEUP_IRQ_S8          (1 << 3)
#define CMU_RES1_SDMMC_IRQ_S19          (1 << 4)
#define CMU_RES1_SDMMC_IRQ_S43          (1 << 16)
#define CMU_RES1_DMA2D_IRQ_S49          (1 << 17)

/* #define WDT_NMI_DISABLED */
#define BTH_WDT_NMI_BTH_WDT             (1 << 0)
#define BTH_WDT_NMI_AON_WDT             (1 << 1)

void hal_cmu_osc_x4_enable_internal(void);
void hal_cmu_osc_x4_disable_internal(void);

enum BTH_CMU_DEBUG_REG_SEL_T {
    BTH_CMU_DEBUG_REG_SEL_MCU_PC    = 0,
    BTH_CMU_DEBUG_REG_SEL_MCU_LR    = 1,
    BTH_CMU_DEBUG_REG_SEL_MCU_SP    = 2,
    BTH_CMU_DEBUG_REG_SEL_CP_PC     = 3,
    BTH_CMU_DEBUG_REG_SEL_CP_LR     = 4,
    BTH_CMU_DEBUG_REG_SEL_CP_SP     = 5,
    BTH_CMU_DEBUG_REG_SEL_DEBUG     = 7,
};

enum BTH_CMU_DMA_REQ_T {
    BTH_CMU_DMA_REQ_PCM_RX          = 0,
    BTH_CMU_DMA_REQ_PCM_TX          = 1,
#ifdef FPGA
    BTH_CMU_DMA_REQ_I2S0_RX         = 2,
    BTH_CMU_DMA_REQ_I2S0_TX         = 3,
#else
    BTH_CMU_DMA_REQ_RESERVED02      = 2,
    BTH_CMU_DMA_REQ_RESERVED03      = 3,
#endif
    BTH_CMU_DMA_REQ_I2C0_RX         = 4,
    BTH_CMU_DMA_REQ_I2C0_TX         = 5,
    BTH_CMU_DMA_REQ_SPI_ITN_RX      = 6,
    BTH_CMU_DMA_REQ_SPI_ITN_TX      = 7,
    BTH_CMU_DMA_REQ_UART0_RX        = 8,
    BTH_CMU_DMA_REQ_UART0_TX        = 9,
    BTH_CMU_DMA_REQ_UART1_RX        = 10,
    BTH_CMU_DMA_REQ_UART1_TX        = 11,
    BTH_CMU_DMA_REQ_CODEC_RX        = 12,
    BTH_CMU_DMA_REQ_CODEC_TX        = 13,
    BTH_CMU_DMA_REQ_FIR_RX          = 14,
    BTH_CMU_DMA_REQ_FIR_TX          = 15,
    BTH_CMU_DMA_REQ_CODEC_TX2       = 16,
    BTH_CMU_DMA_REQ_BTDUMP          = 17,
    BTH_CMU_DMA_REQ_CODEC_MC        = 18,
    BTH_CMU_DMA_REQ_DSD_RX          = 19,
    BTH_CMU_DMA_REQ_SDMMC           = BTH_CMU_DMA_REQ_DSD_RX,
    BTH_CMU_DMA_REQ_DSD_TX          = 20,
    BTH_CMU_DMA_REQ_FLS0            = 21,
    BTH_CMU_DMA_REQ_CODEC_TX3       = 22,
    BTH_CMU_DMA_REQ_CODEC_RX2       = 23,
    BTH_CMU_DMA_REQ_I2C1_RX         = 24,
    BTH_CMU_DMA_REQ_I2C1_TX         = 25,
    BTH_CMU_DMA_REQ_SPI0_RX         = 26,
    BTH_CMU_DMA_REQ_SPI0_TX         = 27,
    BTH_CMU_DMA_REQ_SPI1_RX         = 28,
    BTH_CMU_DMA_REQ_SPI1_TX         = 29,

    BTH_CMU_DMA_REQ_QTY,
    BTH_CMU_DMA_REQ_NULL            = BTH_CMU_DMA_REQ_QTY,
};

static uint32_t cp_entry;

static struct BTHCMU_T * const bthcmu = (struct BTHCMU_T *)BTH_CMU_BASE;

static struct AONCMU_T * const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

static struct AONSEC_T * const POSSIBLY_UNUSED aonsec = (struct AONSEC_T *)AON_SEC_CTRL_BASE;

static struct BTCMU_T * const POSSIBLY_UNUSED btcmu = (struct BTCMU_T *)BT_CMU_BASE;

static struct CMU_T * const POSSIBLY_UNUSED cmu = (struct CMU_T *)SYS_CMU_BASE;

//static struct SENSCMU_T * const senscmu = (struct SENSCMU_T *)SENS_CMU_BASE;

static uint16_t BOOT_BSS_LOC pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(pll_user_map[0]) * 8, "Too many PLL users");

static const enum HAL_CMU_PLL_T sys_pll_sel = HAL_CMU_PLL_BB;

#ifdef LOW_SYS_FREQ
static enum HAL_CMU_FREQ_T BOOT_BSS_LOC cmu_sys_freq;
#endif
static uint8_t BOOT_BSS_LOC force_sys_div;

#ifdef RC48M_ENABLE
#ifndef BTH_AS_MAIN_MCU
const
#endif
static bool BOOT_DATA_LOC rc_enabled =
#ifndef BTH_AS_MAIN_MCU
    true;
#else
    false;
#endif
#endif

static HAL_CMU_BT_TRIGGER_HANDLER_T bt_trig_hdlr[HAL_CMU_BT_TRIGGER_SRC_QTY];

__STATIC_FORCEINLINE void aocmu_reg_update_wait(void)
{
    // Make sure AOCMU (26M clock domain) write opertions finish before return
    aoncmu->CHIP_ID;
}

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id)
{
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if ((id >= HAL_CMU_AON_MCU && id <= HAL_CMU_AON_GLOBAL) ||
            id >= HAL_CMU_MOD_QTY) {
        return 1;
    }
#else
    if (id >= HAL_CMU_AON_MCU) {
        return 1;
    }
#endif

    if (id < HAL_CMU_MOD_P_CMU) {
        bthcmu->HCLK_ENABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        bthcmu->PCLK_ENABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        bthcmu->OCLK_ENABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->MOD_CLK_ENABLE = (1 << (id - HAL_CMU_AON_A_CMU));
        aocmu_reg_update_wait();
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        cmu->HCLK_ENABLE = (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        cmu->PCLK_ENABLE = (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        cmu->OCLK_ENABLE = (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        cmu->QCLK_ENABLE = (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        cmu->XCLK_ENABLE = (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }

    return 0;
}

int hal_cmu_clock_disable(enum HAL_CMU_MOD_ID_T id)
{
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if ((id >= HAL_CMU_AON_MCU && id <= HAL_CMU_AON_GLOBAL) ||
            id >= HAL_CMU_MOD_QTY) {
        return 1;
    }
#else
    if (id >= HAL_CMU_AON_MCU) {
        return 1;
    }
#endif

    if (id < HAL_CMU_MOD_P_CMU) {
        bthcmu->HCLK_DISABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        bthcmu->PCLK_DISABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        bthcmu->OCLK_DISABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->MOD_CLK_DISABLE = (1 << (id - HAL_CMU_AON_A_CMU));
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        cmu->HCLK_DISABLE = (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        cmu->PCLK_DISABLE = (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        cmu->OCLK_DISABLE = (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        cmu->QCLK_DISABLE = (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        cmu->XCLK_DISABLE = (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }

    return 0;
}

enum HAL_CMU_CLK_STATUS_T hal_cmu_clock_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status = 0;

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if ((id >= HAL_CMU_AON_MCU && id <= HAL_CMU_AON_GLOBAL) ||
            id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_CLK_DISABLED;
    }
#else
    if (id >= HAL_CMU_AON_MCU) {
        return HAL_CMU_CLK_DISABLED;
    }
#endif

    if (id < HAL_CMU_MOD_P_CMU) {
        status = bthcmu->HCLK_ENABLE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = bthcmu->PCLK_ENABLE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = bthcmu->OCLK_ENABLE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        status = aoncmu->MOD_CLK_ENABLE & (1 << (id - HAL_CMU_AON_A_CMU));
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        status = cmu->HCLK_DISABLE & (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        status = cmu->PCLK_DISABLE & (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        status = cmu->OCLK_DISABLE & (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        status = cmu->QCLK_DISABLE & (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        status = cmu->XCLK_DISABLE & (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }

    return status ? HAL_CMU_CLK_ENABLED : HAL_CMU_CLK_DISABLED;
}

int hal_cmu_clock_set_mode(enum HAL_CMU_MOD_ID_T id, enum HAL_CMU_CLK_MODE_T mode)
{
    __IO uint32_t *reg = NULL;
    uint32_t val;
    uint32_t lock;

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if ((id >= HAL_CMU_AON_MCU && id <= HAL_CMU_AON_GLOBAL) ||
            id >= HAL_CMU_MOD_QTY) {
        return 1;
    }
#else
    if (id >= HAL_CMU_AON_MCU) {
        return 1;
    }
#endif

    if (id < HAL_CMU_MOD_P_CMU) {
        reg = &bthcmu->HCLK_MODE;
        val = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        reg = &bthcmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        reg = &bthcmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        reg = &aoncmu->MOD_CLK_MODE;
        val = (1 << (id - HAL_CMU_AON_A_CMU));
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        reg = &cmu->HCLK_MODE;
        val = (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        reg = &cmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        reg = &cmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        reg = &cmu->QCLK_MODE;
        val = (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        reg = &cmu->XCLK_MODE;
        val = (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }

    if (reg) {
        lock = int_lock();
        if (mode == HAL_CMU_CLK_MANUAL) {
            *reg |= val;
        } else {
            *reg &= ~val;
        }
        int_unlock(lock);
    }

    return 0;
}

enum HAL_CMU_CLK_MODE_T hal_cmu_clock_get_mode(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t mode = 0;

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if ((id >= HAL_CMU_AON_MCU && id <= HAL_CMU_AON_GLOBAL) ||
            id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_CLK_AUTO;
    }
#else
    if (id >= HAL_CMU_AON_MCU) {
        return HAL_CMU_CLK_AUTO;
    }
#endif

    if (id < HAL_CMU_MOD_P_CMU) {
        mode = bthcmu->HCLK_MODE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        mode = bthcmu->PCLK_MODE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        mode = bthcmu->OCLK_MODE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        mode = aoncmu->MOD_CLK_MODE & (1 << (id - HAL_CMU_AON_A_CMU));
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        mode = cmu->HCLK_MODE & (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        mode = cmu->PCLK_MODE & (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        mode = cmu->OCLK_MODE & (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        mode = cmu->QCLK_MODE & (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        mode = cmu->XCLK_MODE & (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }

    return mode ? HAL_CMU_CLK_MANUAL : HAL_CMU_CLK_AUTO;
}

int hal_cmu_reset_set(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        bthcmu->HRESET_SET = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        bthcmu->PRESET_SET = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        bthcmu->ORESET_SET = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_SET = (1 << (id - HAL_CMU_AON_A_CMU));
    } else if (id <= HAL_CMU_AON_GLOBAL) {
        if (id == HAL_CMU_MOD_GLOBAL) {
            hal_psc_ram_ret_during_reset();
        }
        aoncmu->GBL_RESET_SET = (1 << (id - HAL_CMU_AON_MCU));
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        cmu->HRESET_SET = (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        cmu->PRESET_SET = (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        cmu->ORESET_SET = (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        cmu->QRESET_SET = (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        cmu->XRESET_SET = (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }

    return 0;
}

int hal_cmu_reset_clear(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        bthcmu->HRESET_CLR = (1 << id);
        asm volatile("nop; nop;");
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        bthcmu->PRESET_CLR = (1 << (id - HAL_CMU_MOD_P_CMU));
        asm volatile("nop; nop; nop; nop;");
    } else if (id < HAL_CMU_AON_A_CMU) {
        bthcmu->ORESET_CLR = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_CLR = (1 << (id - HAL_CMU_AON_A_CMU));
        aocmu_reg_update_wait();
    } else if (id <= HAL_CMU_AON_GLOBAL) {
        aoncmu->GBL_RESET_CLR = (1 << (id - HAL_CMU_AON_MCU));
        aocmu_reg_update_wait();
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        cmu->HRESET_CLR = (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        cmu->PRESET_CLR = (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        cmu->ORESET_CLR = (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        cmu->QRESET_CLR = (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        cmu->XRESET_CLR = (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if (HAL_CMU_SYS_H_AHB0 <= id && id <= HAL_CMU_SYS_X_PSRAM) {
        cmu->HCLK_ENABLE;
    }
#endif

    return 0;
}

enum HAL_CMU_RST_STATUS_T hal_cmu_reset_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status = 0;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_RST_SET;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = bthcmu->HRESET_SET & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = bthcmu->PRESET_SET & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = bthcmu->ORESET_SET & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        status = aoncmu->RESET_SET & (1 << (id - HAL_CMU_AON_A_CMU));
    } else if (id <= HAL_CMU_AON_GLOBAL) {
        status = aoncmu->GBL_RESET_SET & (1 << (id - HAL_CMU_AON_MCU));
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        status = cmu->HRESET_SET & (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        status = cmu->PRESET_SET & (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        status = cmu->ORESET_SET & (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        status = cmu->QRESET_SET & (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        status = cmu->XRESET_SET & (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
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
        bthcmu->HRESET_PULSE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        bthcmu->PRESET_PULSE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        bthcmu->ORESET_PULSE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_MCU) {
        aoncmu->RESET_PULSE = (1 << (id - HAL_CMU_AON_A_CMU));
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
    } else if (id <= HAL_CMU_AON_GLOBAL) {
        aoncmu->GBL_RESET_PULSE = (1 << (id - HAL_CMU_AON_MCU));
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    } else if (id < HAL_CMU_SYS_P_CMU) {
        cmu->HRESET_PULSE = (1 << (id - HAL_CMU_SYS_H_AHB0));
    } else if (id < HAL_CMU_SYS_O_SLEEP) {
        cmu->PRESET_PULSE = (1 << (id - HAL_CMU_SYS_P_CMU));
    } else if (id < HAL_CMU_SYS_Q_BTH2SYS) {
        cmu->ORESET_PULSE = (1 << (id - HAL_CMU_SYS_O_SLEEP));
    } else if (id < HAL_CMU_SYS_X_CORE0) {
        cmu->QRESET_PULSE = (1 << (id - HAL_CMU_SYS_Q_BTH2SYS));
    } else if (id <= HAL_CMU_SYS_X_PSRAM) {
        cmu->XRESET_PULSE = (1 << (id - HAL_CMU_SYS_X_CORE0));
#endif
    }
    // Delay 5+ PCLK cycles (10+ HCLK cycles)
    for (i = 0; i < 3; i++);

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
    bthcmu->PERIPH_CLK |= (1 << (BTH_CMU_SEL_TIMER_FAST_SHIFT + 0));
    int_unlock(lock);
}

void TIMER1_SEL_LOC hal_cmu_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    bthcmu->PERIPH_CLK &= ~(1 << (BTH_CMU_SEL_TIMER_FAST_SHIFT + 0));
    int_unlock(lock);
}

void TIMER1_SEL_LOC hal_cmu_timer2_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    bthcmu->PERIPH_CLK |= (1 << (BTH_CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void TIMER1_SEL_LOC hal_cmu_timer2_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    bthcmu->PERIPH_CLK &= ~(1 << (BTH_CMU_SEL_TIMER_FAST_SHIFT + 1));
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
    bthcmu->PERIPH_CLK = SET_BITFIELD(bthcmu->PERIPH_CLK, BTH_CMU_CFG_TIMER_FAST, val);

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

void hal_cmu_force_sys_pll_div(uint32_t div)
{
    force_sys_div = div;
}

int hal_cmu_sys_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    int div;
    uint32_t sub_div;
    uint32_t lock;

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
        enable = BTH_CMU_SEL_OSCX2_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE | BTH_CMU_SEL_OSC_2_BTH_DISABLE | BTH_CMU_SEL_OSC_4_BTH_DISABLE |
            BTH_CMU_SEL_SLOW_BTH_DISABLE | BTH_CMU_SEL_FAST_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE | BTH_CMU_BYPASS_DIV_BTH_DISABLE;
        break;
    case HAL_CMU_FREQ_6M:
#if defined(LOW_SYS_FREQ) && defined(LOW_SYS_FREQ_6P5M)
        enable = BTH_CMU_SEL_OSC_4_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE | BTH_CMU_SEL_SLOW_BTH_DISABLE |
            BTH_CMU_SEL_FAST_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE | BTH_CMU_BYPASS_DIV_BTH_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_12M:
#ifdef LOW_SYS_FREQ
        enable = BTH_CMU_SEL_OSC_2_BTH_ENABLE | BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE |
            BTH_CMU_SEL_FAST_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE | BTH_CMU_BYPASS_DIV_BTH_DISABLE;
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
        enable = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE | BTH_CMU_SEL_OSC_2_BTH_DISABLE |
            BTH_CMU_SEL_FAST_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE | BTH_CMU_BYPASS_DIV_BTH_DISABLE;
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
        enable = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE | BTH_CMU_SEL_FAST_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE | BTH_CMU_BYPASS_DIV_BTH_DISABLE;
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
        enable = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_FAST_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE | BTH_CMU_SEL_OSCX2_BTH_DISABLE | BTH_CMU_BYPASS_DIV_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE;
        break;
#else
        enable = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_RSTN_DIV_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE | BTH_CMU_SEL_PLL_BTH_ENABLE;
        disable = BTH_CMU_BYPASS_DIV_BTH_DISABLE;
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
        enable = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_RSTN_DIV_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE | BTH_CMU_SEL_PLL_BTH_ENABLE;
        disable = BTH_CMU_BYPASS_DIV_BTH_DISABLE;
        div = 0;
#else
        enable = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE | BTH_CMU_BYPASS_DIV_BTH_ENABLE | BTH_CMU_SEL_PLL_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE;
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
        enable = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE | BTH_CMU_BYPASS_DIV_BTH_ENABLE | BTH_CMU_SEL_PLL_BTH_ENABLE;
        disable = BTH_CMU_RSTN_DIV_BTH_DISABLE;
        break;
#endif
    };

    uint8_t cur_sys_div = force_sys_div;
    if (cur_sys_div) {
        if (cur_sys_div == 1) {
            enable = (enable & ~BTH_CMU_RSTN_DIV_BTH_ENABLE) | BTH_CMU_BYPASS_DIV_BTH_ENABLE;
            disable = (disable & ~BTH_CMU_BYPASS_DIV_BTH_DISABLE) | BTH_CMU_RSTN_DIV_BTH_DISABLE;
        } else {
            div = cur_sys_div - 2;
            enable = (enable & ~BTH_CMU_BYPASS_DIV_BTH_ENABLE) | BTH_CMU_RSTN_DIV_BTH_ENABLE;
            disable = (disable & ~BTH_CMU_RSTN_DIV_BTH_DISABLE) | BTH_CMU_BYPASS_DIV_BTH_DISABLE;
        }
    }

    lock = int_lock();
    bthcmu->SYS_DIV = SET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_SEL_SYS_GT, sub_div);
    if (div >= 0) {
        bthcmu->SYS_DIV = SET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_CFG_DIV_BTH, div);
    }
    int_unlock(lock);

    if (enable & BTH_CMU_SEL_PLL_BTH_ENABLE) {
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_RSTN_DIV_BTH_ENABLE;
        if (enable & BTH_CMU_BYPASS_DIV_BTH_ENABLE) {
            bthcmu->SYS_CLK_ENABLE = BTH_CMU_BYPASS_DIV_BTH_ENABLE;
        } else {
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_BYPASS_DIV_BTH_DISABLE;
        }
    }
    bthcmu->SYS_CLK_ENABLE = enable;
    if (enable & BTH_CMU_SEL_PLL_BTH_ENABLE) {
        bthcmu->SYS_CLK_DISABLE = disable;
    } else {
        bthcmu->SYS_CLK_DISABLE = disable & ~(BTH_CMU_RSTN_DIV_BTH_DISABLE | BTH_CMU_BYPASS_DIV_BTH_DISABLE);
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_BYPASS_DIV_BTH_DISABLE;
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_RSTN_DIV_BTH_DISABLE;
    }

    return 0;
}

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_sys_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = bthcmu->SYS_CLK_ENABLE;

    if (sys_clk & BTH_CMU_SEL_PLL_BTH_ENABLE) {
        if (sys_clk & BTH_CMU_BYPASS_DIV_BTH_ENABLE) {
            return HAL_CMU_FREQ_192M;
        } else {
            div = GET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_CFG_DIV_BTH);
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
    } else if ((sys_clk & (BTH_CMU_SEL_FAST_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE)) ==
            (BTH_CMU_SEL_FAST_BTH_ENABLE)) {
        return HAL_CMU_FREQ_96M;
    } else if ((sys_clk & (BTH_CMU_SEL_FAST_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE)) ==
            (BTH_CMU_SEL_FAST_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE)) {
        return HAL_CMU_FREQ_48M;
    } else if (sys_clk & BTH_CMU_SEL_SLOW_BTH_ENABLE) {
        return HAL_CMU_FREQ_24M;
    } else {
        return HAL_CMU_FREQ_32K;
    }
}

int BOOT_TEXT_FLASH_LOC hal_cmu_sys_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_audio_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll)
{
    return !!(bthcmu->SYS_CLK_ENABLE & BTH_CMU_EN_PLL_ENABLE);
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
        en_val1 = AON_CMU_EN_CLK_PLLBB_BTH_ENABLE;
    } else if (pll == HAL_CMU_PLL_DSI) {
        // For SYS DSI
        pu_val = AON_CMU_PU_PLLDSI_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDSI_ENABLE;
        en_val1 = AON_CMU_EN_CLK_PLLDSI_MCU_ENABLE;
    } else {
        // For SYS USB
        pu_val = AON_CMU_PU_PLLUSB_ENABLE;
        en_val = 0; //AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
        en_val1 = 0;
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
            bthcmu->SYS_CLK_ENABLE = BTH_CMU_PU_OSCX2_ENABLE;
            hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
        }
    }
#endif
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        if (pll == sys_pll_sel) {
            bthcmu->SYS_CLK_ENABLE = BTH_CMU_PU_PLL_ENABLE;
        } else {
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
        if (pll == sys_pll_sel) {
            if (bthcmu->SYS_CLK_ENABLE & BTH_CMU_EN_PLL_ENABLE) {
                break;
            }
        } else {
            if (aoncmu->TOP_CLK_ENABLE & en_val) {
                break;
            }
        }
    } while ((hal_sys_timer_get() - start) < timeout);

    if (pll == sys_pll_sel) {
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_PLL_ENABLE;
    } else {
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
        en_val1 = AON_CMU_EN_CLK_PLLBB_BTH_DISABLE;
    } else if (pll == HAL_CMU_PLL_DSI) {
        // For SYS DSI
        pu_val = AON_CMU_PU_PLLDSI_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDSI_DISABLE;
        en_val1 = AON_CMU_EN_CLK_PLLDSI_MCU_DISABLE;
    } else {
        // For SYS USB
        pu_val = AON_CMU_PU_PLLUSB_DISABLE;
        en_val = 0; //AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
        en_val1 = 0;
    }

    lock = int_lock();
    if (user < HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] &= ~(1 << user);
    }
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        aoncmu->TOP_CLK1_DISABLE = en_val1;
        if (pll == sys_pll_sel) {
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_EN_PLL_DISABLE;
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_PU_PLL_DISABLE;
        } else {
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
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_PU_OSCX2_DISABLE;
            hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
        }
    }
#endif
    int_unlock(lock);

    return 0;
}

void hal_cmu_low_freq_mode_enable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
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
}

void hal_cmu_low_freq_mode_disable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
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
}

void BOOT_TEXT_FLASH_LOC hal_cmu_init_pll_selection(void)
{
    enum HAL_CMU_PLL_T sys;

    // Disable the PLL which might be enabled in ROM
    hal_cmu_pll_disable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_ALL);

    sys = sys_pll_sel;
    hal_cmu_sys_select_pll(sys);

#if !(defined(ULTRA_LOW_POWER) || defined(OSC_26M_X4_AUD2BB))
    hal_cmu_pll_enable(sys, HAL_CMU_PLL_USER_SYS);
#endif

#if defined(FLASH_HIGH_SPEED) || \
        (defined(FLASH_SRC_FREQ_MHZ) && (FLASH_SRC_FREQ_MHZ > 104)) || \
        !(defined(FLASH_LOW_SPEED) || defined(OSC_26M_X4_AUD2BB))
    enum HAL_CMU_PLL_T flash;

    flash = HAL_CMU_PLL_BB;
    hal_cmu_flash_all_select_pll(flash);
    hal_cmu_pll_enable(flash, HAL_CMU_PLL_USER_FLASH);
#endif

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    hal_cmu_init_axi_pll_selection();
#endif
}

void hal_cmu_codec_set_fault_mask2(uint32_t msk0, uint32_t msk1)
{
    uint32_t lock;

    lock = int_lock();
    // If bit set 1, DAC will be muted when some faults occur
    bthcmu->PERIPH_CLK = SET_BITFIELD(bthcmu->PERIPH_CLK, BTH_CMU_MASK_OBS, msk0);
    int_unlock(lock);
}

void hal_cmu_pcm_set_slave_mode(int clk_pol)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t cfg;

    mask = BTH_CMU_POL_CLK_PCM_IN;

    if (clk_pol) {
        cfg = BTH_CMU_POL_CLK_PCM_IN;
    } else {
        cfg = 0;
    }

    lock = int_lock();
    bthcmu->PERIPH_CLK = (bthcmu->PERIPH_CLK & ~mask) | cfg;
    int_unlock(lock);
}

void hal_cmu_pcm_set_master_mode(void)
{
}

void hal_cmu_pcm_clock_enable(void)
{
}

void hal_cmu_pcm_clock_disable(void)
{
}

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
void hal_cmu_emmc_clock_enable(void)
{
    hal_cmu_axi_freq_req(HAL_CMU_AXI_FREQ_USER_EMMC, HAL_CMU_AXI_FREQ_48M);
    hal_cmu_sys_emmc_clock_enable();
}

void hal_cmu_emmc_clock_disable(void)
{
    hal_cmu_sys_emmc_clock_disable();
    // Release axi freq
    hal_cmu_axi_freq_req(HAL_CMU_AXI_FREQ_USER_EMMC, HAL_CMU_AXI_FREQ_32K);
}

#ifdef CHIP_HAS_USB
void hal_cmu_usb_clock_enable(void)
{
    hal_cmu_axi_freq_req(HAL_CMU_AXI_FREQ_USER_USB, HAL_CMU_AXI_FREQ_48M);

    hal_cmu_sys_usb_clock_enable();
}

void hal_cmu_usb_clock_disable(void)
{
    hal_cmu_sys_usb_clock_disable();
    // Release axi freq
    hal_cmu_axi_freq_req(HAL_CMU_AXI_FREQ_USER_USB, HAL_CMU_AXI_FREQ_32K);
}
#endif
#endif

#ifdef BTH_USE_SYS_PERIPH
void hal_cmu_sdmmc0_clock_enable(void)
{
#ifdef BTH_AS_MAIN_MCU
    hal_cmu_axi_freq_req(HAL_CMU_AXI_FREQ_USER_SDIO, HAL_CMU_AXI_FREQ_48M);
    hal_cmu_sys_sdmmc0_clock_enable();
#else
    // TODO: Send rpcMsg to M55
#endif
}

void hal_cmu_sdmmc0_clock_disable(void)
{
#ifdef BTH_AS_MAIN_MCU
    hal_cmu_sys_sdmmc0_clock_disable();
    // Release axi freq
    hal_cmu_axi_freq_req(HAL_CMU_AXI_FREQ_USER_SDIO, HAL_CMU_AXI_FREQ_32K);
#else
    // TODO: Send rpcMsg to M55
#endif
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_apb_init_div(void)
{
    // Divider defaults to 2 (reg_val = div - 2)
    //bthcmu->SYS_DIV = SET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_CFG_DIV_PCLK, 0);
}

int hal_cmu_periph_set_div(uint32_t div)
{
    uint32_t lock;
    int ret = 0;

    if (div == 0 || div > ((BTH_CMU_CFG_DIV_PER_MASK >> BTH_CMU_CFG_DIV_PER_SHIFT) + 2)) {
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_RSTN_DIV_PER_DISABLE;
        if (div > ((BTH_CMU_CFG_DIV_PER_MASK >> BTH_CMU_CFG_DIV_PER_SHIFT) + 2)) {
            ret = 1;
        }
    } else {
        lock = int_lock();
        if (div == 1) {
            bthcmu->SYS_CLK_ENABLE = BTH_CMU_BYPASS_DIV_PER_ENABLE;
        } else {
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_BYPASS_DIV_PER_DISABLE;
            div -= 2;
            bthcmu->PERIPH_CLK = SET_BITFIELD(bthcmu->PERIPH_CLK, BTH_CMU_CFG_DIV_PER, div);
        }
        int_unlock(lock);
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_RSTN_DIV_PER_ENABLE;
    }

    return ret;
}

#define PERPH_SET_DIV_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_div(uint32_t div) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (div < 2 || div > ((BTH_CMU_CFG_DIV_ ##F## _MASK >> BTH_CMU_CFG_DIV_ ##F## _SHIFT) + 2)) { \
        bthcmu->r &= ~(BTH_CMU_SEL_OSCX2_ ##F | BTH_CMU_SEL_PLL_ ##F | BTH_CMU_EN_PLL_ ##F); \
        ret = 1; \
    } else { \
        div -= 2; \
        bthcmu->r = (bthcmu->r & ~(BTH_CMU_CFG_DIV_ ##F## _MASK)) | BTH_CMU_SEL_OSCX2_ ##F | BTH_CMU_SEL_PLL_ ##F | \
            BTH_CMU_CFG_DIV_ ##F(div); \
        bthcmu->r |= BTH_CMU_EN_PLL_ ##F; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_DIV_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_DIV_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_DIV_FUNC(spi, SPI0, SPI_CLK);
PERPH_SET_DIV_FUNC(slcd, SPI1,SPI_CLK);
PERPH_SET_DIV_FUNC(i2c0, I2C0, I2C_CLK);
PERPH_SET_DIV_FUNC(i2c1, I2C1, I2C_CLK);

#define PERPH_SET_FREQ_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (freq == HAL_CMU_PERIPH_FREQ_26M) { \
        bthcmu->r &= ~(BTH_CMU_SEL_OSCX2_ ##F | BTH_CMU_SEL_PLL_ ##F | BTH_CMU_EN_PLL_ ##F); \
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) { \
        bthcmu->r = (bthcmu->r & ~(BTH_CMU_SEL_PLL_ ##F | BTH_CMU_EN_PLL_ ##F)) | BTH_CMU_SEL_OSCX2_ ##F; \
    } else { \
        ret = 1; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_FREQ_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_FREQ_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_FREQ_FUNC(spi, SPI0, SPI_CLK);
PERPH_SET_FREQ_FUNC(slcd, SPI1,SPI_CLK);
PERPH_SET_FREQ_FUNC(i2c0, I2C0, I2C_CLK);
PERPH_SET_FREQ_FUNC(i2c1, I2C1, I2C_CLK);

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        bthcmu->PERIPH_CLK &= ~BTH_CMU_SEL_OSCX2_SPI2;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        bthcmu->PERIPH_CLK |= BTH_CMU_SEL_OSCX2_SPI2;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x2_enable(void)
{
#ifdef RC48M_ENABLE
    if (!rc_enabled)
#endif
    {
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_PU_OSCX2_ENABLE;
        hal_sys_timer_delay(US_TO_TICKS(60));
    }
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_OSCX2_ENABLE;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x4_enable_internal(void)
{
#ifdef ANA_26M_X4_ENABLE
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_PU_OSCX4_ENABLE;
#ifdef RC48M_ENABLE
    if (rc_enabled) {
        hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
    } else
#endif
    {
        hal_sys_timer_delay(US_TO_TICKS(60));
    }
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_OSCX4_ENABLE;
    aoncmu->TOP_CLK1_ENABLE = AON_CMU_EN_CLK_OSCX4_BTH_ENABLE;
#endif
#if defined(OSC_26M_X4_AUD2BB) && defined(BTH_AS_MAIN_MCU)
    aoncmu->CLK_SELECT &= ~AON_CMU_SEL_X4_DIG;
#endif
}

void hal_cmu_osc_x4_disable_internal(void)
{
    aoncmu->TOP_CLK1_DISABLE = AON_CMU_EN_CLK_OSCX4_BTH_DISABLE;
    bthcmu->SYS_CLK_DISABLE = BTH_CMU_EN_OSCX4_DISABLE;
    bthcmu->SYS_CLK_DISABLE = BTH_CMU_PU_OSCX4_DISABLE;
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
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_PU_OSCX2_ENABLE;
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
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_PU_OSCX2_DISABLE;
    }
    hal_cmu_osc_x4_disable_internal();
    rc_enabled = true;
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_bth_periph_dma_cfg0_req_init(void)
{
    // DMA channel config
    bthcmu->ADMA_CH0_4_REQ =
        // codec
        BTH_CMU_GDMA_CH0_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_RX) | BTH_CMU_GDMA_CH1_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_TX) |
#ifdef CODEC_DSD
        // codec_dsd
        BTH_CMU_GDMA_CH2_REQ_IDX(BTH_CMU_DMA_REQ_DSD_RX) | BTH_CMU_GDMA_CH3_REQ_IDX(BTH_CMU_DMA_REQ_DSD_TX) |
#else
        // btpcm
        BTH_CMU_GDMA_CH2_REQ_IDX(BTH_CMU_DMA_REQ_PCM_RX) | BTH_CMU_GDMA_CH3_REQ_IDX(BTH_CMU_DMA_REQ_PCM_TX) |
#endif
        // codec2
        BTH_CMU_GDMA_CH4_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_RX2);
    bthcmu->ADMA_CH5_9_REQ =
        // codec2
        BTH_CMU_GDMA_CH5_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_TX2) |
        // uart0
        BTH_CMU_GDMA_CH6_REQ_IDX(BTH_CMU_DMA_REQ_UART0_RX) | BTH_CMU_GDMA_CH7_REQ_IDX(BTH_CMU_DMA_REQ_UART0_TX) |
        // uart1
        BTH_CMU_GDMA_CH8_REQ_IDX(BTH_CMU_DMA_REQ_UART1_RX) | BTH_CMU_GDMA_CH9_REQ_IDX(BTH_CMU_DMA_REQ_UART1_TX);
    bthcmu->ADMA_CH10_14_REQ =
#ifdef BTH_USE_SYS_PERIPH
        // sdmmc
        BTH_CMU_GDMA_CH10_REQ_IDX(BTH_CMU_DMA_REQ_SDMMC) |
#else
        BTH_CMU_GDMA_CH10_REQ_IDX(BTH_CMU_DMA_REQ_NULL) |
#endif
        // codec3
        BTH_CMU_GDMA_CH11_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_TX3) |
        // btdump
        BTH_CMU_GDMA_CH12_REQ_IDX(BTH_CMU_DMA_REQ_BTDUMP) |
        // mc
        BTH_CMU_GDMA_CH13_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_MC) |
        // fir
        BTH_CMU_GDMA_CH14_REQ_IDX(BTH_CMU_DMA_REQ_FIR_RX);
    bthcmu->ADMA_CH15_REQ =
        // fir
        SET_BITFIELD(bthcmu->ADMA_CH15_REQ, BTH_CMU_GDMA_CH15_REQ_IDX, BTH_CMU_DMA_REQ_FIR_TX);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_bth_periph_dma_cfg1_req_init(void)
{
    // DMA channel config
    bthcmu->ADMA_CH0_4_REQ =
        // codec
        BTH_CMU_GDMA_CH0_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_RX) | BTH_CMU_GDMA_CH1_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_TX) |
#ifdef CODEC_DSD
        // codec_dsd
        BTH_CMU_GDMA_CH2_REQ_IDX(BTH_CMU_DMA_REQ_DSD_RX) | BTH_CMU_GDMA_CH3_REQ_IDX(BTH_CMU_DMA_REQ_DSD_TX) |
#else
        // btpcm
        BTH_CMU_GDMA_CH2_REQ_IDX(BTH_CMU_DMA_REQ_PCM_RX) | BTH_CMU_GDMA_CH3_REQ_IDX(BTH_CMU_DMA_REQ_PCM_TX) |
#endif
        // codec2
        BTH_CMU_GDMA_CH4_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_RX2);
    bthcmu->ADMA_CH5_9_REQ =
        // null
        BTH_CMU_GDMA_CH5_REQ_IDX(BTH_CMU_DMA_REQ_NULL) |
        // uart0
        BTH_CMU_GDMA_CH6_REQ_IDX(BTH_CMU_DMA_REQ_UART0_RX) | BTH_CMU_GDMA_CH7_REQ_IDX(BTH_CMU_DMA_REQ_UART0_TX) |
        // uart1
        BTH_CMU_GDMA_CH8_REQ_IDX(BTH_CMU_DMA_REQ_UART1_RX) | BTH_CMU_GDMA_CH9_REQ_IDX(BTH_CMU_DMA_REQ_UART1_TX);
    bthcmu->ADMA_CH10_14_REQ =
#ifdef BTH_USE_SYS_PERIPH
        // sdmmc
        BTH_CMU_GDMA_CH10_REQ_IDX(BTH_CMU_DMA_REQ_SDMMC) |
#else
        BTH_CMU_GDMA_CH10_REQ_IDX(BTH_CMU_DMA_REQ_NULL) |
#endif
        // codec3
        BTH_CMU_GDMA_CH11_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_TX3) |
        // btdump
        BTH_CMU_GDMA_CH12_REQ_IDX(BTH_CMU_DMA_REQ_BTDUMP) |
        // mc
        BTH_CMU_GDMA_CH13_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_MC) |
        // fir
        BTH_CMU_GDMA_CH14_REQ_IDX(BTH_CMU_DMA_REQ_FIR_RX);
    bthcmu->ADMA_CH15_REQ =
        // fir
        SET_BITFIELD(bthcmu->ADMA_CH15_REQ, BTH_CMU_GDMA_CH15_REQ_IDX, BTH_CMU_DMA_REQ_FIR_TX);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_bth_periph_dma_cfg2_req_init(void)
{
    // DMA channel config
    bthcmu->ADMA_CH0_4_REQ =
        // null
        BTH_CMU_GDMA_CH0_REQ_IDX(BTH_CMU_DMA_REQ_NULL) | BTH_CMU_GDMA_CH1_REQ_IDX(BTH_CMU_DMA_REQ_NULL) |
        // btpcm
        BTH_CMU_GDMA_CH2_REQ_IDX(BTH_CMU_DMA_REQ_PCM_RX) | BTH_CMU_GDMA_CH3_REQ_IDX(BTH_CMU_DMA_REQ_PCM_TX) |
        // null
        BTH_CMU_GDMA_CH4_REQ_IDX(BTH_CMU_DMA_REQ_NULL);
    bthcmu->ADMA_CH5_9_REQ =
        // null
        BTH_CMU_GDMA_CH5_REQ_IDX(BTH_CMU_DMA_REQ_NULL) |
        // uart0
        BTH_CMU_GDMA_CH6_REQ_IDX(BTH_CMU_DMA_REQ_UART0_RX) | BTH_CMU_GDMA_CH7_REQ_IDX(BTH_CMU_DMA_REQ_UART0_TX) |
        // uart1
        BTH_CMU_GDMA_CH8_REQ_IDX(BTH_CMU_DMA_REQ_UART1_RX) | BTH_CMU_GDMA_CH9_REQ_IDX(BTH_CMU_DMA_REQ_UART1_TX);
    bthcmu->ADMA_CH10_14_REQ =
#ifdef BTH_USE_SYS_PERIPH
        // sdmmc
        BTH_CMU_GDMA_CH10_REQ_IDX(BTH_CMU_DMA_REQ_SDMMC) |
#else
        BTH_CMU_GDMA_CH10_REQ_IDX(BTH_CMU_DMA_REQ_NULL) |
#endif
        // codec3
        BTH_CMU_GDMA_CH11_REQ_IDX(BTH_CMU_DMA_REQ_CODEC_TX3) |
        // btdump
        BTH_CMU_GDMA_CH12_REQ_IDX(BTH_CMU_DMA_REQ_BTDUMP) |
        // null
        BTH_CMU_GDMA_CH13_REQ_IDX(BTH_CMU_DMA_REQ_NULL) |
        // fir
        BTH_CMU_GDMA_CH14_REQ_IDX(BTH_CMU_DMA_REQ_FIR_RX);
    bthcmu->ADMA_CH15_REQ =
        // fir
        SET_BITFIELD(bthcmu->ADMA_CH15_REQ, BTH_CMU_GDMA_CH15_REQ_IDX, BTH_CMU_DMA_REQ_FIR_TX);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_module_init_state(void)
{
    hal_psc_core_init_ram_ret_mask();

    // Init IRQ assignments
    bthcmu->MISC = (bthcmu->MISC & (CMU_RES1_EMMC_IRQ_S43_C0 | CMU_RES1_FPU_IRQ_S0 |
        CMU_RES1_GPU_IRQ_S49_C8 | CMU_RES1_WAKEUP_IRQ_S8)) |
        CMU_RES1_SDMMC_IRQ_S43 |
#ifdef BTH_USE_SYS_PERIPH
        CMU_RES1_SDMMC_IRQ_S19 |
#endif
        CMU_RES1_DMA2D_IRQ_S49;

    // Init DMA periph req
#if defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 1)
    hal_cmu_bth_periph_dma_cfg1_req_init();
#elif defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 2)
    hal_cmu_bth_periph_dma_cfg2_req_init();
#else
    hal_cmu_bth_periph_dma_cfg0_req_init();
#endif

#if 1 //ndef SIMU
    bthcmu->ORESET_SET = BTH_ORST_TIMER1 | BTH_ORST_UART0 | BTH_ORST_UART1 |
        BTH_ORST_PCM | BTH_ORST_I2C0 | BTH_ORST_I2S0 |
#ifndef SUBSYS_WDT_ENABLE
        BTH_ORST_WDT |
#endif
#ifdef NO_ISPI
        BTH_ORST_SPI_ITN |
#endif
        0;
    bthcmu->PRESET_SET = BTH_PRST_TIMER1 | BTH_PRST_UART0 | BTH_PRST_UART1 |
        BTH_PRST_PCM | BTH_PRST_I2C0 | BTH_PRST_I2S0 |
#ifndef SUBSYS_WDT_ENABLE
        BTH_PRST_WDT |
#endif
#ifdef NO_ISPI
        BTH_PRST_SPI_ITN |
#endif
        0;
    bthcmu->HRESET_SET = BTH_HRST_BT_TPORT | BTH_HRST_BT_DUMP | BTH_HRST_CODEC;

    bthcmu->OCLK_DISABLE = BTH_OCLK_TIMER1 | BTH_OCLK_UART0 | BTH_OCLK_UART1 |
        BTH_OCLK_PCM | BTH_OCLK_I2C0 | BTH_OCLK_I2S0 |
#ifndef SUBSYS_WDT_ENABLE
        BTH_OCLK_WDT |
#endif
#ifdef NO_ISPI
        BTH_OCLK_SPI_ITN |
#endif
        0;
    bthcmu->PCLK_DISABLE = BTH_PCLK_TIMER1 | BTH_PCLK_UART0 | BTH_PCLK_UART1 |
        BTH_PCLK_PCM | BTH_PCLK_I2C0 | BTH_PCLK_I2S0 |
#ifndef SUBSYS_WDT_ENABLE
        BTH_PCLK_WDT |
#endif
#ifdef NO_ISPI
        BTH_PCLK_SPI_ITN |
#endif
        0;
    bthcmu->HCLK_DISABLE = BTH_HCLK_BT_TPORT | BTH_HCLK_BT_DUMP | BTH_HCLK_CODEC;

#ifdef ALT_BOOT_FLASH
    bthcmu->ORESET_SET = BTH_ORST_FLASH1;
    bthcmu->HRESET_SET = BTH_HRST_FLASH1;
    bthcmu->OCLK_DISABLE = BTH_OCLK_FLASH1;
    bthcmu->HCLK_DISABLE = BTH_HCLK_FLASH1;
#elif !defined(FLASH1_CTRL_BASE)
    bthcmu->HRESET_SET = BTH_HRST_FLASH0;
    bthcmu->HCLK_DISABLE = BTH_HCLK_FLASH0;
#endif

    bthcmu->PCLK_MODE &= ~(BTH_PCLK_CMU | BTH_PCLK_WDT | BTH_PCLK_TIMER0 | BTH_PCLK_TIMER1);

    //bthcmu->HCLK_MODE = 0;
    //bthcmu->PCLK_MODE = BTH_PCLK_UART0 | BTH_PCLK_UART1;
    //bthcmu->OCLK_MODE = 0;
#endif

#ifndef WDT_NMI_DISABLED
    // Watchdog NMI setting
    uint32_t nmi;

#ifdef BTH_AS_MAIN_MCU
    nmi = BTH_WDT_NMI_AON_WDT;
#else
    nmi = BTH_WDT_NMI_BTH_WDT;
#endif
    bthcmu->SYS_DIV = SET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_WDT_NMI_MASK, nmi);
#endif

#ifdef CORE_SLEEP_POWER_DOWN
    hal_cmu_set_wakeup_vector(SCB->VTOR);
    NVIC_SetResetHandler(hal_sleep_core_power_up);
#endif

#if defined(__1600_FPGA_NEED_DEL_IT__)
    return;
#endif

#ifdef BTH_AS_MAIN_MCU
    hal_cmu_main_module_init();
#endif

#ifdef CPU_PC_DUMP
    hal_cpudump_enable();
#endif
}

void hal_cmu_lpu_wait_26m_ready(void)
{
    while ((bthcmu->WAKEUP_CLK_CFG & (BTH_CMU_LPU_AUTO_SWITCH26 | BTH_CMU_LPU_STATUS_26M)) ==
            BTH_CMU_LPU_AUTO_SWITCH26);
}

int hal_cmu_lpu_busy(void)
{
    if ((bthcmu->WAKEUP_CLK_CFG & (BTH_CMU_LPU_AUTO_SWITCH26 | BTH_CMU_LPU_STATUS_26M)) ==
            BTH_CMU_LPU_STATUS_26M) {
        return 1;
    }
    if ((bthcmu->WAKEUP_CLK_CFG & (BTH_CMU_LPU_AUTO_SWITCHPLL | BTH_CMU_LPU_STATUS_PLL)) ==
            BTH_CMU_LPU_STATUS_PLL) {
        return 1;
    }
    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg)
{
    uint32_t lpu_clk;
    uint32_t timer_26m;
    uint32_t timer_pll;
    uint32_t overhead_26m;

#ifdef NO_SLEEP_POWER_DOWN
    // None of MCU/BT/SENS enables sleep power down
    overhead_26m = hal_cmu_get_osc_switch_overhead();
#else
    // Any SUBSYS enables sleep power down
    overhead_26m = 0;
#endif

    timer_26m = LPU_TIMER_US(HAL_CMU_26M_READY_TIMEOUT_US);
    timer_pll = LPU_TIMER_US(HAL_CMU_PLL_LOCKED_TIMEOUT_US);

    if (timer_26m && timer_26m > overhead_26m) {
        timer_26m -= overhead_26m;
    } else {
        timer_26m = 1;
    }
    if (timer_pll == 0) {
        timer_pll = 1;
    }

    if (cfg >= HAL_CMU_LPU_CLK_QTY) {
        return 1;
    }
    if ((timer_26m & (BTH_CMU_TIMER_WT26_MASK >> BTH_CMU_TIMER_WT26_SHIFT)) != timer_26m) {
        return 2;
    }
    if ((timer_pll & (BTH_CMU_TIMER_WTPLL_MASK >> BTH_CMU_TIMER_WTPLL_SHIFT)) != timer_pll) {
        return 3;
    }
    if (hal_cmu_lpu_busy()) {
        return -1;
    }

    if (cfg == HAL_CMU_LPU_CLK_26M) {
        lpu_clk = BTH_CMU_LPU_AUTO_SWITCH26;
    } else if (cfg == HAL_CMU_LPU_CLK_PLL) {
        lpu_clk = BTH_CMU_LPU_AUTO_SWITCHPLL | BTH_CMU_LPU_AUTO_SWITCH26;
    } else {
        lpu_clk = 0;
    }

    if (lpu_clk & BTH_CMU_LPU_AUTO_SWITCH26) {
        // Disable RAM wakeup early
        bthcmu->MCU_TIMER &= ~BTH_CMU_RAM_RETN_UP_EARLY;
        // MCU/ROM/RAM auto clock gating (which depends on RAM gating signal)
        uint32_t val = BTH_HCLK_CORE0 | BTH_HCLK_CACHE0 |
            BTH_HCLK_RAM0 | BTH_HCLK_RAM1 | BTH_HCLK_RAM2 | BTH_HCLK_RAM3 | BTH_HCLK_RAM4 |
            BTH_HCLK_RAM5 | BTH_HCLK_RAM6;
#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM5_BASE)
        val &= ~BTH_HCLK_RAM5;
        bthcmu->HCLK_ENABLE = BTH_HCLK_RAM5;
        bthcmu->HCLK_MODE |= BTH_HCLK_RAM5;
#endif
        bthcmu->HCLK_MODE &= ~val;
#ifdef BTH_AS_MAIN_MCU
        // AON_CMU enable auto switch 26M (AON_CMU must have selected 26M and disabled 52M/32K already)
        aoncmu->CLK_SELECT |= AON_CMU_LPU_AUTO_SWITCH26;
    } else {
        // AON_CMU disable auto switch 26M
        aoncmu->CLK_SELECT &= ~AON_CMU_LPU_AUTO_SWITCH26;
#endif
    }

#ifdef BTH_AS_MAIN_MCU
    aoncmu->CLK_SELECT = SET_BITFIELD(aoncmu->CLK_SELECT, AON_CMU_TIMER_WT24, timer_26m) |
        AON_CMU_TIMER_WT24_EN | AON_CMU_OSC_READY_MODE | AON_CMU_OSC_READY_BYPASS_SYNC |
#ifdef SLOW_SYS_BYPASS
        AON_CMU_SEL_SLOW_SYS_BYPASS;
#else
        0;
#endif
#endif
    bthcmu->WAKEUP_CLK_CFG = BTH_CMU_TIMER_WT26(timer_26m) | BTH_CMU_TIMER_WTPLL(0) | lpu_clk;
    if (timer_pll) {
        hal_sys_timer_delay(US_TO_TICKS(60));
        bthcmu->WAKEUP_CLK_CFG = BTH_CMU_TIMER_WT26(timer_26m) | BTH_CMU_TIMER_WTPLL(timer_pll) | lpu_clk;
    }
    return 0;
}

#ifdef CORE_SLEEP_POWER_DOWN
SRAM_TEXT_LOC
static void save_cmu_regs(struct SAVED_BTHCMU_REGS_T *sav)
{
    sav->HCLK_ENABLE            = bthcmu->HCLK_ENABLE;
    sav->PCLK_ENABLE            = bthcmu->PCLK_ENABLE;
    sav->OCLK_ENABLE            = bthcmu->OCLK_ENABLE;
    sav->HCLK_MODE              = bthcmu->HCLK_MODE;
    sav->PCLK_MODE              = bthcmu->PCLK_MODE;
    sav->OCLK_MODE              = bthcmu->OCLK_MODE;
    sav->REG_RAM_CFG0           = bthcmu->REG_RAM_CFG0;
    sav->HRESET_CLR             = bthcmu->HRESET_CLR;
    sav->PRESET_CLR             = bthcmu->PRESET_CLR;
    sav->ORESET_CLR             = bthcmu->ORESET_CLR;
    sav->MCU_TIMER              = bthcmu->MCU_TIMER;
    sav->SLEEP                  = bthcmu->SLEEP;
    sav->PERIPH_CLK             = bthcmu->PERIPH_CLK;
    sav->SYS_CLK_ENABLE         = bthcmu->SYS_CLK_ENABLE;
    sav->SYS_DIV                = bthcmu->SYS_DIV;
    sav->REG_RAM_CFG1           = bthcmu->REG_RAM_CFG1;
    sav->UART_CLK               = bthcmu->UART_CLK;
    sav->CPU_CFG                = bthcmu->CPU_CFG;
    sav->MCU2SENS_MASK0         = bthcmu->MCU2SENS_MASK0;
    sav->MCU2SENS_MASK1         = bthcmu->MCU2SENS_MASK1;
    sav->WAKEUP_CLK_CFG         = bthcmu->WAKEUP_CLK_CFG;
    sav->MCU2BT_INTMASK0        = bthcmu->MCU2BT_INTMASK0;
    sav->MCU2BT_INTMASK1        = bthcmu->MCU2BT_INTMASK1;
    sav->ADMA_CH0_4_REQ         = bthcmu->ADMA_CH0_4_REQ;
    sav->ADMA_CH5_9_REQ         = bthcmu->ADMA_CH5_9_REQ;
    sav->ADMA_CH10_14_REQ       = bthcmu->ADMA_CH10_14_REQ;
    sav->ADMA_CH15_REQ          = bthcmu->ADMA_CH15_REQ;
    sav->MISC                   = bthcmu->MISC;
    sav->SIMU_RES               = bthcmu->SIMU_RES;
    sav->MISC_0F8               = bthcmu->MISC_0F8;
    sav->REG_RAM_CFG2           = bthcmu->REG_RAM_CFG2;
    sav->TPORT_IRQ_LEN          = bthcmu->TPORT_IRQ_LEN;
    sav->TPORT_CUR_ADDR         = bthcmu->TPORT_CUR_ADDR;
    sav->TPORT_START            = bthcmu->TPORT_START;
    sav->TPORT_END              = bthcmu->TPORT_END;
    sav->TPORT_CTRL             = bthcmu->TPORT_CTRL;
    sav->BTH2SYS_MASK0          = bthcmu->BTH2SYS_MASK0;
    sav->BTH2SYS_MASK1          = bthcmu->BTH2SYS_MASK1;
    sav->BT_TOG_IRQ             = bthcmu->BT_TOG_IRQ;
    sav->BT_TOG_IRQ_CLR         = bthcmu->BT_TOG_IRQ_CLR;
}

SRAM_TEXT_LOC
static void restore_cmu_regs(const struct SAVED_BTHCMU_REGS_T *sav)
{
    bthcmu->HRESET_SET          = ~sav->HRESET_CLR;
    bthcmu->PRESET_SET          = ~sav->PRESET_CLR;
    bthcmu->ORESET_SET          = ~sav->ORESET_CLR;
    bthcmu->HCLK_DISABLE        = ~sav->HCLK_ENABLE;
    bthcmu->PCLK_DISABLE        = ~sav->PCLK_ENABLE;
    bthcmu->OCLK_DISABLE        = ~sav->OCLK_ENABLE;

    bthcmu->HCLK_ENABLE         = sav->HCLK_ENABLE;
    bthcmu->PCLK_ENABLE         = sav->PCLK_ENABLE;
    bthcmu->OCLK_ENABLE         = sav->OCLK_ENABLE;
    bthcmu->HCLK_MODE           = sav->HCLK_MODE;
    bthcmu->PCLK_MODE           = sav->PCLK_MODE;
    bthcmu->OCLK_MODE           = sav->OCLK_MODE;
    bthcmu->REG_RAM_CFG0        = sav->REG_RAM_CFG0;
    bthcmu->HRESET_CLR          = sav->HRESET_CLR;
    bthcmu->PRESET_CLR          = sav->PRESET_CLR;
    bthcmu->ORESET_CLR          = sav->ORESET_CLR;
    bthcmu->MCU_TIMER           = sav->MCU_TIMER;
    bthcmu->SLEEP               = sav->SLEEP;
    bthcmu->PERIPH_CLK          = sav->PERIPH_CLK;
    bthcmu->SYS_CLK_ENABLE      = sav->SYS_CLK_ENABLE;
    bthcmu->SYS_DIV             = sav->SYS_DIV;
    bthcmu->REG_RAM_CFG1        = sav->REG_RAM_CFG1;
    bthcmu->UART_CLK            = sav->UART_CLK;
    bthcmu->CPU_CFG             = sav->CPU_CFG;
    bthcmu->MCU2SENS_MASK0      = sav->MCU2SENS_MASK0;
    bthcmu->MCU2SENS_MASK1      = sav->MCU2SENS_MASK1;
    bthcmu->WAKEUP_CLK_CFG      = sav->WAKEUP_CLK_CFG;
    bthcmu->MCU2BT_INTMASK0     = sav->MCU2BT_INTMASK0;
    bthcmu->MCU2BT_INTMASK1     = sav->MCU2BT_INTMASK1;
    bthcmu->ADMA_CH0_4_REQ      = sav->ADMA_CH0_4_REQ;
    bthcmu->ADMA_CH5_9_REQ      = sav->ADMA_CH5_9_REQ;
    bthcmu->ADMA_CH10_14_REQ    = sav->ADMA_CH10_14_REQ;
    bthcmu->ADMA_CH15_REQ       = sav->ADMA_CH15_REQ;
    bthcmu->MISC                = sav->MISC;
    bthcmu->SIMU_RES            = sav->SIMU_RES;
    bthcmu->MISC_0F8            = sav->MISC_0F8;
    bthcmu->REG_RAM_CFG2        = sav->REG_RAM_CFG2;
    bthcmu->TPORT_IRQ_LEN       = sav->TPORT_IRQ_LEN;
    bthcmu->TPORT_CUR_ADDR      = sav->TPORT_CUR_ADDR;
    bthcmu->TPORT_START         = sav->TPORT_START;
    bthcmu->TPORT_END           = sav->TPORT_END;
    bthcmu->TPORT_CTRL          = sav->TPORT_CTRL;
    bthcmu->BTH2SYS_MASK0       = sav->BTH2SYS_MASK0;
    bthcmu->BTH2SYS_MASK1       = sav->BTH2SYS_MASK1;
    bthcmu->BT_TOG_IRQ          = sav->BT_TOG_IRQ;
    bthcmu->BT_TOG_IRQ_CLR      = sav->BT_TOG_IRQ_CLR;
}

POSSIBLY_UNUSED
static int SRAM_TEXT_LOC hal_cmu_lpu_sleep_pd(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_hclk;
    uint32_t saved_oclk;
    uint32_t saved_clk_cfg;
    uint32_t cpu_regs[50];
    struct SAVED_BTHCMU_REGS_T cmu_regs;

    hal_psc_core_ram_pd_sleep();

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    struct SAVED_CMU_REGS_T sys_cmu_regs;

    hal_iomux_sleep();
    save_sys_cmu_regs(&sys_cmu_regs);
    hal_psc_sys_disable(HAL_PSC_SYS_USER_PERIPH);
#endif

    NVIC_PowerDownSleep(cpu_regs, ARRAY_SIZE(cpu_regs));
    save_cmu_regs(&cmu_regs);

    saved_hclk = cmu_regs.HCLK_ENABLE;
    saved_oclk = cmu_regs.OCLK_ENABLE;
    saved_clk_cfg = cmu_regs.SYS_CLK_ENABLE;

#ifdef BTH_AS_MAIN_MCU
    uint32_t saved_top_clk;

    saved_top_clk = aoncmu->TOP_CLK_ENABLE;
#endif

    // Reset sub div
    bthcmu->SYS_DIV = SET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_SEL_SYS_GT, 3);

    // Disable memory/flash clock
    bthcmu->OCLK_DISABLE = BTH_OCLK_FLASH1;
    bthcmu->HCLK_DISABLE = BTH_HCLK_FLASH0 | BTH_HCLK_FLASH1;

#ifdef BTH_AS_MAIN_MCU
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
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE;
    bthcmu->SYS_CLK_DISABLE = BTH_CMU_SEL_OSC_2_BTH_DISABLE | BTH_CMU_SEL_FAST_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE;
    bthcmu->SYS_CLK_DISABLE = BTH_CMU_BYPASS_DIV_BTH_DISABLE;
    bthcmu->SYS_CLK_DISABLE = BTH_CMU_RSTN_DIV_BTH_DISABLE;
    // Shutdown system PLL
    if (saved_clk_cfg & BTH_CMU_PU_PLL_ENABLE) {
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_EN_PLL_DISABLE;
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_PU_PLL_DISABLE;
    }

    hal_sleep_core_power_down(false);

    while ((bthcmu->WAKEUP_CLK_CFG & BTH_CMU_LPU_STATUS_26M) == 0);

    // Switch system freq to 26M
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_SEL_SLOW_BTH_ENABLE |
        (saved_clk_cfg & (BTH_CMU_PU_OSCX2_ENABLE | BTH_CMU_PU_OSCX4_ENABLE));
    bthcmu->SYS_CLK_DISABLE = BTH_CMU_SEL_OSC_2_BTH_DISABLE;
    // System freq is 26M now and will be restored later

    hal_sys_timer_wakeup();

    if (saved_clk_cfg & BTH_CMU_PU_PLL_ENABLE) {
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_PU_PLL_ENABLE;
        start = hal_sys_timer_get();
        timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
        while ((hal_sys_timer_get() - start) < timeout);
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_PLL_ENABLE;
    }

    // Disable memory/flash clock
    bthcmu->OCLK_DISABLE = BTH_OCLK_FLASH1;
    bthcmu->HCLK_DISABLE = BTH_HCLK_FLASH0 | BTH_HCLK_FLASH1;

#ifdef BTH_AS_MAIN_MCU
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
    }
#endif

    // Restore system freq
    bthcmu->SYS_CLK_ENABLE = saved_clk_cfg &
        (BTH_CMU_RSTN_DIV_FLS0_ENABLE | BTH_CMU_RSTN_DIV_BTH_ENABLE);
    bthcmu->SYS_CLK_ENABLE = saved_clk_cfg &
        (BTH_CMU_BYPASS_DIV_FLS0_ENABLE | BTH_CMU_BYPASS_DIV_BTH_ENABLE);
    bthcmu->SYS_CLK_ENABLE = saved_clk_cfg;
    bthcmu->SYS_CLK_DISABLE = ~saved_clk_cfg;

    if (saved_hclk & (BTH_HCLK_FLASH0 | BTH_HCLK_FLASH1)) {
        // Enable memory/flash clock
        bthcmu->HCLK_ENABLE = saved_hclk;
        bthcmu->OCLK_ENABLE = saved_oclk;
        // Wait until memory/flash clock ready
        hal_sys_timer_delay_us(2);
    }

    restore_cmu_regs(&cmu_regs);
    NVIC_PowerDownWakeup(cpu_regs, ARRAY_SIZE(cpu_regs));

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    hal_psc_sys_enable(HAL_PSC_SYS_USER_PERIPH);

    hal_sys_timer_delay_us(HAL_CMU_26M_READY_TIMEOUT_US + 100);
    restore_sys_cmu_regs(&sys_cmu_regs);
    hal_iomux_wakeup();
#endif

    hal_psc_core_ram_pd_wakeup();

    // TODO:
    // 1) Register pm notif handler for all hardware modules, e.g., sdmmc
    // 2) Recover system timer in rt_suspend() and rt_resume()
    // 3) Dynamically select 32K sleep or power down sleep

    return 0;
}
#endif

static int SRAM_TEXT_LOC hal_cmu_lpu_sleep_normal(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_hclk;
    uint32_t saved_oclk;
    uint32_t wakeup_cfg;
    uint32_t saved_clk_cfg;
    uint32_t saved_sys_div;
#if defined(CORE_SLEEP_POWER_DOWN) && defined(CPU_PD_SUBSYS_PU)
    uint32_t cpu_regs[50];
#endif

    hal_psc_core_ram_ret_sleep();

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    uint32_t sys_saved_hclk;
    uint32_t sys_saved_oclk;
    uint32_t sys_saved_clk_cfg;
    uint32_t sys_saved_uart_clk;
    uint32_t sys_wakeup_cfg;
    uint32_t sys_sleep_time;

    sys_saved_hclk = cmu->HCLK_ENABLE;
    sys_saved_oclk = cmu->OCLK_ENABLE;
    sys_saved_clk_cfg = cmu->SYS_CLK_ENABLE;
    sys_saved_uart_clk = cmu->UART_CLK;

    // Reset sub div
    cmu->UART_CLK = SET_BITFIELD(cmu->UART_CLK, CMU_SEL_SYS_GT, 3);

    // Disable memory/flash clock
    cmu->OCLK_DISABLE = SYS_OCLK_FLASH | SYS_OCLK_PSRAM;
    cmu->HCLK_DISABLE = SYS_HCLK_FLASH | SYS_HCLK_PSRAM;

    // Setup wakeup mask (BTH_IRQn only)
    uint32_t bth_irq_mask0, bth_irq_mask1;
    bth_irq_mask0 = NVIC->ISER[0];
    bth_irq_mask1 = NVIC->ISER[1];
    bthcmu->BTH2SYS_MASK0 = bth_irq_mask0;
    bthcmu->BTH2SYS_MASK1 = bth_irq_mask1;

    uint32_t sys_irq_mask0, sys_irq_mask1, sys_irq_mask2;
    sys_irq_mask0 = (1 << 5);
    sys_irq_mask1 = 0;
    sys_irq_mask2 = 0;
    if (bth_irq_mask0 & (1 << SYS_USB_PIN_IRQn)) {
        sys_irq_mask0 |= (1 << 17);
    }
    if (bth_irq_mask0 & (1 << SYS_GPIO_IRQn)) {
        sys_irq_mask1 |= (1 << (62 - 32));
    }
    cmu->SYS2BTH_IRQ_SET = CMU_BTH_ALLIRQ_MASK_SET;
    cmu->WAKEUP_MASK0 = sys_irq_mask0;
    cmu->WAKEUP_MASK1 = sys_irq_mask1;
    cmu->SYS2OTH_MASK2 = SET_BITFIELD(cmu->SYS2OTH_MASK2, CMU_WAKEUP_IRQ_MASK2, sys_irq_mask2);

    // Shutdown system PLL
    if (sys_saved_clk_cfg & CMU_PU_PLL_ENABLE) {
        cmu->SYS_CLK_DISABLE = CMU_EN_PLL_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_PU_PLL_DISABLE;
    }

    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        sys_wakeup_cfg = cmu->WAKEUP_CLK_CFG;
    } else {
        sys_wakeup_cfg = 0;
    }
    if (sys_wakeup_cfg & (CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26)) {
        // Enable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_MANUAL_RAM_RETN) |
            CMU_DEEPSLEEP_EN | CMU_DEEPSLEEP_ROMRAM_EN | CMU_DEEPSLEEP_START;
    } else {
        // Disable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_DEEPSLEEP_ROMRAM_EN) |
            CMU_DEEPSLEEP_EN | CMU_MANUAL_RAM_RETN | CMU_DEEPSLEEP_START;
        // Switch system freq to 32K
        cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_4_SYS_DISABLE | CMU_SEL_SLOW_SYS_DISABLE;
    }
    sys_sleep_time = hal_sys_timer_get();
#endif

#ifdef BTH_AS_MAIN_MCU
    uint32_t saved_top_clk;
    uint32_t saved_aon_clk;
    bool pd_bb_pll;
    bool pd_usb_pll;
    bool wait_pll_locked;

    pd_bb_pll = true;
    pd_usb_pll = true;

    saved_aon_clk = aoncmu->AON_CLK;
    saved_top_clk = aoncmu->TOP_CLK_ENABLE;
#endif

    saved_hclk = bthcmu->HCLK_ENABLE;
    saved_oclk = bthcmu->OCLK_ENABLE;
    saved_clk_cfg = bthcmu->SYS_CLK_ENABLE;
    saved_sys_div = bthcmu->SYS_DIV;

    if (0) {
#if defined(CORE_SLEEP_POWER_DOWN) && defined(CPU_PD_SUBSYS_PU)
    } else if (mode == HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K) {
        wakeup_cfg = bthcmu->WAKEUP_CLK_CFG;
        ASSERT(wakeup_cfg & (BTH_CMU_LPU_AUTO_SWITCHPLL | BTH_CMU_LPU_AUTO_SWITCH26),
            "%s: Bad LPU mode: 0x%08X", __func__, wakeup_cfg);
#endif
    } else if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        wakeup_cfg = bthcmu->WAKEUP_CLK_CFG;
    } else {
        // SYS_32K
        wakeup_cfg = 0;
    }

#ifdef BTH_AS_MAIN_MCU
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
    }
#endif

    // Reset sub div
    bthcmu->SYS_DIV = SET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_SEL_SYS_GT, 3);

    // Disable memory/flash clock
    bthcmu->OCLK_DISABLE = BTH_OCLK_FLASH1;
    bthcmu->HCLK_DISABLE = BTH_HCLK_FLASH0 | BTH_HCLK_FLASH1;

    // Setup wakeup mask
    bthcmu->WAKEUP_MASK0 = NVIC->ISER[0];
    bthcmu->WAKEUP_MASK1 = NVIC->ISER[1];

    if (wakeup_cfg & (BTH_CMU_LPU_AUTO_SWITCHPLL | BTH_CMU_LPU_AUTO_SWITCH26)) {
        // Enable auto memory retention
        bthcmu->SLEEP = (bthcmu->SLEEP & ~BTH_CMU_MANUAL_RAM_RETN) |
            BTH_CMU_DEEPSLEEP_EN | BTH_CMU_DEEPSLEEP_ROMRAM_EN | BTH_CMU_DEEPSLEEP_START;
    } else {
        // Disable auto memory retention
        bthcmu->SLEEP = (bthcmu->SLEEP & ~BTH_CMU_DEEPSLEEP_ROMRAM_EN) |
            BTH_CMU_DEEPSLEEP_EN | BTH_CMU_MANUAL_RAM_RETN | BTH_CMU_DEEPSLEEP_START;
    }

#ifdef BTH_AS_MAIN_MCU
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

    if (wakeup_cfg & BTH_CMU_LPU_AUTO_SWITCHPLL) {
        // Do nothing
        // Hardware will switch system freq to 32K and shutdown PLLs automatically
    } else {
        // Switch system freq to 26M
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_SEL_SLOW_BTH_ENABLE | BTH_CMU_SEL_OSCX2_BTH_ENABLE;
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_SEL_OSC_2_BTH_DISABLE | BTH_CMU_SEL_FAST_BTH_DISABLE | BTH_CMU_SEL_PLL_BTH_DISABLE;
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_BYPASS_DIV_BTH_DISABLE;
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_RSTN_DIV_BTH_DISABLE;
        // Shutdown system PLL
        if (saved_clk_cfg & BTH_CMU_PU_PLL_ENABLE) {
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_EN_PLL_DISABLE;
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_PU_PLL_DISABLE;
        }
        if (wakeup_cfg & BTH_CMU_LPU_AUTO_SWITCH26) {
            // Do nothing
            // Hardware will switch system freq to 32K automatically
        } else {
#ifdef BTH_AS_MAIN_MCU
            // Manually switch AON_CMU clock to 32K
            aoncmu->AON_CLK = saved_aon_clk & ~AON_CMU_SEL_CLK_OSC;
#endif
            // Switch system freq to 32K
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_SEL_OSC_4_BTH_DISABLE | BTH_CMU_SEL_SLOW_BTH_DISABLE;
        }
    }

#if defined(CORE_SLEEP_POWER_DOWN) && defined(CPU_PD_SUBSYS_PU)
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

    if (wakeup_cfg & BTH_CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_PLL_LOCKED_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
        while ((bthcmu->WAKEUP_CLK_CFG & BTH_CMU_LPU_STATUS_PLL) == 0 &&
            (hal_sys_timer_get() - start) < timeout);
        // !!! CAUTION !!!
        // Hardware will switch system freq to PLL divider and enable PLLs automatically
    } else {
        // Wait for 26M ready
        if (wakeup_cfg & BTH_CMU_LPU_AUTO_SWITCH26) {
            start = hal_sys_timer_get();
            timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
            while ((bthcmu->WAKEUP_CLK_CFG & BTH_CMU_LPU_STATUS_26M) == 0 &&
                (hal_sys_timer_get() - start) < timeout);
            // Hardware will switch system freq to 26M automatically
        } else {
            if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
                timeout = HAL_CMU_26M_READY_TIMEOUT;
                hal_sys_timer_delay(timeout);
            }
            // Switch system freq to 26M
            bthcmu->SYS_CLK_ENABLE = BTH_CMU_SEL_SLOW_BTH_ENABLE;
            bthcmu->SYS_CLK_DISABLE = BTH_CMU_SEL_OSC_2_BTH_DISABLE;
#ifdef BTH_AS_MAIN_MCU
            // Restore AON_CMU clock
            aoncmu->AON_CLK = saved_aon_clk;
#endif
        }
        // System freq is 26M now and will be restored later
        if (saved_clk_cfg & BTH_CMU_PU_PLL_ENABLE) {
            bthcmu->SYS_CLK_ENABLE = BTH_CMU_PU_PLL_ENABLE;
            start = hal_sys_timer_get();
            timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
            while ((hal_sys_timer_get() - start) < timeout);
            bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_PLL_ENABLE;
        }
    }

#ifdef BTH_AS_MAIN_MCU
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
    }
#endif

    // Restore system freq
    bthcmu->SYS_CLK_ENABLE = saved_clk_cfg & BTH_CMU_RSTN_DIV_BTH_ENABLE;
    bthcmu->SYS_CLK_ENABLE = saved_clk_cfg & BTH_CMU_BYPASS_DIV_BTH_ENABLE;
    bthcmu->SYS_CLK_ENABLE = saved_clk_cfg;
    bthcmu->SYS_CLK_DISABLE = ~saved_clk_cfg;
    bthcmu->SYS_DIV = saved_sys_div;

#ifdef BTH_AS_MAIN_MCU
    if (mode != HAL_CMU_LPU_SLEEP_MODE_CHIP && mode != HAL_CMU_LPU_SLEEP_MODE_CPU_PD_SUBSYS_32K) {
        // Restore the original top clock settings
        aoncmu->TOP_CLK_DISABLE = ~saved_top_clk;
    }
#endif

    if (saved_hclk & (BTH_HCLK_FLASH0 | BTH_HCLK_FLASH1)) {
        // Enable memory/flash clock
        bthcmu->HCLK_ENABLE = saved_hclk;
        bthcmu->OCLK_ENABLE = saved_oclk;
        // Wait until memory/flash clock ready
        hal_sys_timer_delay_us(2);
    }

#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    // Avoid error when SYS enters sleep while BTH skips sleep
    // SYS needs 3.5 cycles of 32K clock to enter sleep (and then LPU_STATUS is valid)
    while (hal_sys_timer_get() - sys_sleep_time < 3) {}

    if (sys_wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        timeout = US_TO_TICKS(300);
        while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_PLL) == 0 &&
            (hal_sys_timer_get() - start) < timeout);
    } else {
        // Wait for 26M ready
        if (sys_wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            start = hal_sys_timer_get();
            timeout = US_TO_TICKS(300);
            while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0 &&
                (hal_sys_timer_get() - start) < timeout);
            // Hardware will switch system freq to 26M automatically
        } else {
            // Switch system freq to 26M
            cmu->SYS_CLK_ENABLE = CMU_SEL_SLOW_SYS_ENABLE;
            cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_2_SYS_DISABLE;
        }
    }
    // System freq is 26M now and will be restored later
    if (sys_saved_clk_cfg & CMU_PU_PLL_ENABLE) {
        cmu->SYS_CLK_ENABLE = CMU_PU_PLL_ENABLE;
        start = hal_sys_timer_get();
        timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
        while ((hal_sys_timer_get() - start) < timeout);
        cmu->SYS_CLK_ENABLE = CMU_EN_PLL_ENABLE;
    }
    // Restore system freq
    cmu->SYS_CLK_ENABLE = sys_saved_clk_cfg & CMU_RSTN_DIV_SYS_ENABLE;
    cmu->SYS_CLK_ENABLE = sys_saved_clk_cfg & CMU_BYPASS_DIV_SYS_ENABLE;
    cmu->SYS_CLK_ENABLE = sys_saved_clk_cfg;
    cmu->SYS_CLK_DISABLE = ~sys_saved_clk_cfg;
    cmu->UART_CLK = sys_saved_uart_clk;

    if (sys_saved_oclk & (SYS_OCLK_FLASH | SYS_OCLK_PSRAM)) {
        // Enable memory/flash clock
        cmu->HCLK_ENABLE = sys_saved_hclk;
        cmu->OCLK_ENABLE = sys_saved_oclk;
    }
#endif

    return 0;
}

int SRAM_TEXT_LOC hal_cmu_lpu_sleep(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
#ifdef CORE_SLEEP_POWER_DOWN
    if (mode == HAL_CMU_LPU_SLEEP_MODE_POWER_DOWN) {
        return hal_cmu_lpu_sleep_pd(mode);
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
        if (bthcmu->ISIRQ_SET & (BTH_CMU_BT_PLAYTIME_STAMP_INTR_MSK << i)) {
            bthcmu->ISIRQ_CLR = (BTH_CMU_BT_PLAYTIME_STAMP_INTR_CLR << i);
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
    bthcmu->ISIRQ_CLR = (BTH_CMU_BT_PLAYTIME_STAMP_INTR_CLR << src);
    val = bthcmu->PERIPH_CLK;
    bthcmu->PERIPH_CLK = val | (BTH_CMU_BT_PLAYTIME_STAMP_MASK << src);
    for (i = 0; i < HAL_CMU_BT_TRIGGER_SRC_QTY; i++) {
        if (val & (BTH_CMU_BT_PLAYTIME_STAMP_MASK << i)) {
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
    val = bthcmu->PERIPH_CLK;
    val &= ~(BTH_CMU_BT_PLAYTIME_STAMP_MASK << src);
    bthcmu->PERIPH_CLK = val;
    for (i = 0; i < HAL_CMU_BT_TRIGGER_SRC_QTY; i++) {
        if (val & (BTH_CMU_BT_PLAYTIME_STAMP_MASK << i)) {
            break;
        }
    }
    if (i >= HAL_CMU_BT_TRIGGER_SRC_QTY) {
        NVIC_DisableIRQ(BT_STAMP_IRQn);
    }
    int_unlock(lock);

    return 0;
}

void hal_cmu_cp_enable(uint32_t sp, uint32_t entry)
{
    struct CORE_STARTUP_CFG_T * cp_cfg;
    uint32_t cfg_addr;

    // Use (sp - 128) as the default vector. The Address must aligned to 128-byte boundary.
    cfg_addr = (sp - (1 << 7)) & ~((1 << 7) - 1);

    bthcmu->CP_VTOR = (bthcmu->CP_VTOR & ~CMU_VTOR_CORE1_MASK) | (cfg_addr & CMU_VTOR_CORE1_MASK);
    cp_cfg = (struct CORE_STARTUP_CFG_T *)cfg_addr;

    cp_cfg->stack = sp;
    cp_cfg->reset_hdlr = (uint32_t)system_cp_reset_handler;
    cp_entry = entry;

    __DMB();
    hal_cmu_clock_enable(HAL_CMU_MOD_H_CP);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CP);
}

void hal_cmu_cp_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_CP);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_CP);
}

void hal_cmu_beco_enable(void)
{
    cmu->HCLK_ENABLE = BTH_HCLK_CP0;
    cmu->HRESET_CLR  = BTH_HRST_CP0;
    aocmu_reg_update_wait();
}

void hal_cmu_beco_disable(void)
{
    cmu->HRESET_SET   = BTH_HCLK_CP0;
    cmu->HCLK_DISABLE = BTH_HRST_CP0;
}

void hal_cmu_cp_beco_enable(void)
{
    cmu->XCLK_ENABLE = BTH_HCLK_CP1;
    cmu->XRESET_CLR  = BTH_HRST_CP1;
    aocmu_reg_update_wait();
}

void hal_cmu_cp_beco_disable(void)
{
    cmu->XRESET_SET   = BTH_HCLK_CP1;
    cmu->XCLK_DISABLE = BTH_HRST_CP1;
}

uint32_t hal_cmu_cp_get_entry_addr(void)
{
    return cp_entry;
}

void hal_cmu_set_wakeup_vector(uint32_t vector)
{
    aoncmu->BTH_VTOR = (aoncmu->BTH_VTOR & ~AON_CMU_VTOR_CORE_BTH_MASK) | (vector & AON_CMU_VTOR_CORE_BTH_MASK);
}

void BOOT_TEXT_FLASH_LOC hal_cpudump_clock_enable(void)
{
    bthcmu->HCLK_ENABLE = BTH_HCLK_DUMPCPU;
    bthcmu->HRESET_CLR  = BTH_HRST_DUMPCPU;
}

void hal_cpudump_clock_disable(void)
{
    bthcmu->HRESET_SET   = BTH_HCLK_DUMPCPU;
    bthcmu->HCLK_DISABLE = BTH_HRST_DUMPCPU;
}

uint32_t hal_cmu_cpudump_get_last_addr(void)
{
    uint32_t addr = 0x100;
    if(aoncmu->LAST_PC_INFO & AON_CMU_BTH_LAST_PC_EN)
        addr = GET_BITFIELD(aoncmu->LAST_PC_INFO, AON_CMU_BTH_LAST_PC_ADDR);

    return addr;
}

#ifdef BTH_AS_MAIN_MCU
void BOOT_TEXT_SRAM_LOC hal_cmu_boot_flash_reset_pulse(void)
{
#ifdef ALT_BOOT_FLASH
    cmu->ORESET_SET = SYS_ORST_FLASH;
    bthcmu->HRESET_SET = BTH_HRST_FLASH0;
    hal_sys_timer_delay_us(2);
    cmu->ORESET_CLR = SYS_ORST_FLASH;
    bthcmu->HRESET_CLR = BTH_HRST_FLASH0;
#else
    bthcmu->ORESET_SET = BTH_ORST_FLASH1;
    bthcmu->HRESET_SET = BTH_HRST_FLASH1;
    hal_sys_timer_delay_us(2);
    bthcmu->ORESET_CLR = BTH_ORST_FLASH1;
    bthcmu->HRESET_CLR = BTH_HRST_FLASH1;
#endif
}

#ifdef SWITCH_MAIN_MCU
void BOOT_TEXT_FLASH_LOC NORETURN hal_cmu_boot_start_bth_cpu(uint32_t sp, uint32_t entry)
{
    volatile struct CORE_STARTUP_CFG_T *core_cfg;
    uint32_t cfg_addr;

    // Use (sp - 128) as the default vector. The Address must aligned to 128-byte boundary.
    cfg_addr = (sp - (1 << 7)) & ~((1 << 7) - 1);

    aoncmu->BTH_VTOR = (aoncmu->BTH_VTOR & ~AON_CMU_VTOR_CORE_BTH_MASK) | (cfg_addr & AON_CMU_VTOR_CORE_BTH_MASK);
    core_cfg = (volatile struct CORE_STARTUP_CFG_T *)cfg_addr;

    core_cfg->stack = sp;
    core_cfg->reset_hdlr = entry;
    // Flush previous write operation
    __DSB();
    core_cfg->reset_hdlr;

#if defined(__1600_FPGA_NEED_DEL_IT__)
#define DIGITAL_REG(a)                        (*(volatile uint32_t *)(a))

    DIGITAL_REG(0x40000034) = 1;
    DIGITAL_REG(0x40000038) = 1;
#else
    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_BTHCPU_CLR;
#endif
    while (1) {
        __WFI();
    }
}

void BOOT_TEXT_FLASH_LOC NORETURN hal_cmu_boot_m55_start_bth(void)
{
#ifdef JUMP_TO_ALT_FLASH
    if (*(volatile uint32_t *)FLASH1_NC_BASE != ~0UL) {
        (*(void (*)(void))(FLASH1_BASE + 0x11))();
    }
    while (1) {}
#else
    uint32_t code_start;
    uint32_t entry;
    uint32_t sp;
    extern uint32_t __StackTop[];

    code_start = (uint32_t)__builtin_return_address(0);
#if defined(PROGRAMMER) && !defined(PROGRAMMER_INFLASH)
    code_start = RAM_S_TO_NS(code_start);
#endif

    hal_cmu_bth_ram_cfg();

    sp = (uint32_t)__StackTop;
    entry = (code_start | 1);

    hal_cmu_boot_start_bth_cpu(sp, entry);
#endif
}
void NORETURN boot_loader_entry_hook(void) __attribute__((weak, alias("hal_cmu_boot_m55_start_bth")));

#else // !SWITCH_MAIN_MCU

void BOOT_TEXT_FLASH_LOC hal_cmu_boot_bth_setup_ram_cfg(void)
{
    hal_cmu_bth_ram_cfg();
}
void boot_loader_entry_hook(void) __attribute__((alias("hal_cmu_boot_bth_setup_ram_cfg")));

#endif // !SWITCH_MAIN_MCU
#endif // BTH_AS_MAIN_MCU

#endif
