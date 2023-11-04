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
#include CHIP_SPECIFIC_HDR(reg_aoncmu)
#include CHIP_SPECIFIC_HDR(reg_aonsec)
#include CHIP_SPECIFIC_HDR(reg_aonpmu)
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
#include "hal_norflash.h"
#include "hal_psc.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "cmsis_nvic.h"
#include "pmu.h"

#define SYS_FLASH_CFG_LOC               BOOT_TEXT_SRAM_LOC
#define BTH_FLASH_CFG_LOC               BOOT_TEXT_SRAM_LOC

#ifdef CHIP_SUBSYS_BTH
// This is BTH
#if defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_FLASH)
// BTH is main MCU and using sys flash
#define SYS_FLASH_IN_USE
#endif

#if defined(CHIP_DMA_CFG_IDX) && ((CHIP_DMA_CFG_IDX == 1) || (CHIP_DMA_CFG_IDX == 2))
#define BTH_CODEC_REQ_SYS_DMA
#endif

#elif !defined(ALT_BOOT_FLASH)
// This is M55
#ifdef BTH_AS_MAIN_MCU
// M55 is dsp
#ifdef SUBSYS_FLASH_BOOT
// M55 is dsp and running in sys flash
#define SYS_FLASH_IN_USE
#endif
#else
// M55 is main MCU
#define SYS_FLASH_IN_USE
#endif
#endif

#ifdef MEM_USE_BBPLL
#define MEM_PLL_SEL                     HAL_CMU_PLL_BB
#else
// Same as AXI-SYS PLL
#define MEM_PLL_SEL                     HAL_CMU_PLL_QTY
#endif

#ifdef EMMC_USE_BBPLL
#define EMMC_PLL_SEL                    HAL_CMU_PLL_BB
#elif defined(EMMC_USE_USBPLL)
#define EMMC_PLL_SEL                    HAL_CMU_PLL_USB
#elif defined(EMMC_USE_DSIPLL)
#define EMMC_PLL_SEL                    HAL_CMU_PLL_DSI
#else
// OSCX4
#define EMMC_PLL_SEL                    HAL_CMU_PLL_QTY
#endif

#ifdef AUDIO_USE_USBPLL
#define AUDIO_PLL_SEL                   HAL_CMU_PLL_USB
#else
#define AUDIO_PLL_SEL                   HAL_CMU_PLL_BB
#endif

#define AON_CMU_SENS_RAM0_ECC_EN        (1 << 4)
#define AON_CMU_SENS_RAM1_ECC_EN        (1 << 5)

#define CMU_RES3_SDMMC_IRQ_S19          (1 << 2)
#define CMU_RES3_UART0_IRQ_S6           (1 << 18)

#define SYS_WDT_NMI_SYS_WDT             (1 << 0)
#define SYS_WDT_NMI_AON_WDT             (1 << 1)

enum SYS_TCM_CFG_TYPE_T {
    SYS_TCM_CFG_AXI_RAM             = 0,
    SYS_TCM_CFG_M55_ITCM,
    SYS_TCM_CFG_HIFI4_ITCM,
    SYS_TCM_CFG_M55_DTCM,
    SYS_TCM_CFG_HIFI4_DTCM,
};

enum AXI_MOD_USER_T {
    AXI_MOD_USER_M55                = 0,
    AXI_MOD_USER_DSP,
    AXI_MOD_USER_BTH,

    AXI_MOD_USER_QTY,
    AXI_MOD_USER_ALL                = AXI_MOD_USER_QTY,
};

enum CMU_USB_CLK_SRC_T {
    CMU_USB_CLK_SRC_OSC_24M_X2      = 0,
    CMU_USB_CLK_SRC_PLL             = 1,
};

enum CMU_I2S_MCLK_SEL_T {
    CMU_I2S_MCLK_SEL_MCU            = 0,
    CMU_I2S_MCLK_SEL_SENS           = 1,
    CMU_I2S_MCLK_SEL_CODEC          = 2,
};

enum CMU_DMA_REQ_T {
    CMU_DMA_REQ_CODEC_RX            = 0,
    CMU_DMA_REQ_CODEC_TX            = 1,
    CMU_DMA_REQ_I2S0_RX             = 2,
    CMU_DMA_REQ_I2S0_TX             = 3,
    CMU_DMA_REQ_FIR_RX              = 4,
    CMU_DMA_REQ_FIR_TX              = 5,
    CMU_DMA_REQ_SPDIF0_RX           = 6,
    CMU_DMA_REQ_SPDIF0_TX           = 7,
    CMU_DMA_REQ_CODEC_MC            = 8,
    CMU_DMA_REQ_CODEC_TX2           = 9,
    CMU_DMA_REQ_DSD_RX              = 10,
    CMU_DMA_REQ_DSD_TX              = 11,
    CMU_DMA_REQ_FLS0                = 12,
    CMU_DMA_REQ_SDMMC0              = 13,
    CMU_DMA_REQ_I2C0_RX             = 14,
    CMU_DMA_REQ_I2C0_TX             = 15,
    CMU_DMA_REQ_SPILCD0_RX          = 16,
    CMU_DMA_REQ_SPILCD0_TX          = 17,
    CMU_DMA_REQ_UART0_RX            = 18,
    CMU_DMA_REQ_UART0_TX            = 19,
    CMU_DMA_REQ_UART1_RX            = 20,
    CMU_DMA_REQ_UART1_TX            = 21,
    CMU_DMA_REQ_SPI_ITN_RX          = 22,
    CMU_DMA_REQ_SPI_ITN_TX          = 23,
    CMU_DMA_REQ_I2S1_RX             = 24,
    CMU_DMA_REQ_I2S1_TX             = 25,
    CMU_DMA_REQ_I2C1_RX             = 26,
    CMU_DMA_REQ_I2C1_TX             = 27,
    CMU_DMA_REQ_SPILCD1_RX          = 28,
    CMU_DMA_REQ_SPILCD1_TX          = 29,
    CMU_DMA_REQ_BTH_PCM_RX          = 30,
    CMU_DMA_REQ_BTH_PCM_TX          = 31,
    CMU_DMA_REQ_CODEC_TX3           = 32,
    CMU_DMA_REQ_CODEC_RX2           = 33,
    CMU_DMA_REQ_PCM_RX              = 34,
    CMU_DMA_REQ_PCM_TX              = 35,

    CMU_DMA_REQ_QTY,
    CMU_DMA_REQ_NULL                = CMU_DMA_REQ_QTY,
};

static struct CMU_T * POSSIBLY_UNUSED const cmu = (struct CMU_T *)SYS_CMU_BASE;

static struct AONCMU_T * POSSIBLY_UNUSED const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

static struct AONPMU_T * POSSIBLY_UNUSED const aonpmu = (struct AONPMU_T *)AON_PMU_BASE;

static struct AONSEC_T * POSSIBLY_UNUSED const aonsec = (struct AONSEC_T *)AON_SEC_CTRL_BASE;

static struct BTCMU_T * const POSSIBLY_UNUSED btcmu = (struct BTCMU_T *)BT_CMU_BASE;

static struct BTHCMU_T * const POSSIBLY_UNUSED bthcmu = (struct BTHCMU_T *)BTH_CMU_BASE;

static struct SENSCMU_T * const POSSIBLY_UNUSED senscmu = (struct SENSCMU_T *)SENS_CMU_BASE;

__STATIC_FORCEINLINE void aocmu_reg_update_wait(void)
{
    // Make sure AOCMU (26M clock domain) write opertions finish before return
    aoncmu->CHIP_ID;
}

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) && !defined(BTH_AS_MAIN_MCU)))

#ifdef DCDC_CLOCK_CONTROL
static uint8_t dcdc_user_map;
STATIC_ASSERT(HAL_CMU_DCDC_CLOCK_USER_QTY <= sizeof(dcdc_user_map) * 8, "Too many DCDC clock users");
#endif

#ifdef LOW_SYS_FREQ
static enum HAL_CMU_AXI_FREQ_T BOOT_BSS_LOC cmu_axi_sys_freq;
#endif
#if !(defined(BTH_AS_MAIN_MCU) || defined(CHIP_SUBSYS_BTH))
static uint8_t BOOT_BSS_LOC force_axi_sys_div;
#endif

static bool anc_enabled;

static uint8_t store_mod_map;
STATIC_ASSERT(sizeof(store_mod_map) * 8 >= HAL_PSC_STORAGE_MOD_QTY, "store_mod_map size too small");

BOOT_BSS_LOC static uint8_t axi_clk_map;
STATIC_ASSERT(sizeof(axi_clk_map) * 8 >= AXI_MOD_USER_QTY, "axi_clk_map size too small");
BOOT_BSS_LOC static uint8_t axi_rst_map;
STATIC_ASSERT(sizeof(axi_rst_map) * 8 >= AXI_MOD_USER_QTY, "axi_rst_map size too small");

#ifdef __AUDIO_RESAMPLE__
static bool aud_resample_en = true;
#endif

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
static uint16_t axi_pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(axi_pll_user_map[0]) * 8, "Too many PLL users");

static uint32_t axifreq_bundle[(HAL_CMU_AXI_FREQ_USER_QTY + 3) / 4];
static uint8_t * const axifreq_per_user = (uint8_t *)&axifreq_bundle[0];
static enum HAL_CMU_AXI_FREQ_USER_T axi_top_user = HAL_CMU_AXI_FREQ_USER_QTY;
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

void hal_cmu_anc_enable(enum HAL_CMU_ANC_CLK_USER_T user)
{
    anc_enabled = true;
}

void hal_cmu_anc_disable(enum HAL_CMU_ANC_CLK_USER_T user)
{
    anc_enabled = false;
}

int hal_cmu_anc_get_status(enum HAL_CMU_ANC_CLK_USER_T user)
{
    return anc_enabled;
}

void hal_cmu_i2s_clock_out_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_I2S0_OUT;
    } else {
        val = CMU_EN_CLK_I2S1_OUT;
    }

    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_I2S0_OUT;
    } else {
        val = CMU_EN_CLK_I2S1_OUT;
    }

    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_slave_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_CLKIN;
    } else {
        val = CMU_SEL_I2S1_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_master_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_CLKIN;
    } else {
        val = CMU_SEL_I2S1_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;
    volatile uint32_t *reg;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_PLL_I2S0;
        reg = &cmu->I2S0_CLK;
    } else {
        val = CMU_EN_CLK_PLL_I2S1;
        reg = &cmu->I2S1_CLK;
    }

    lock = int_lock();
    *reg |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;
    volatile uint32_t *reg;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_PLL_I2S0;
        reg = &cmu->I2S0_CLK;
    } else {
        val = CMU_EN_CLK_PLL_I2S1;
        reg = &cmu->I2S1_CLK;
    }

    lock = int_lock();
    *reg &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_resample_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_OSC;
    } else {
        val = CMU_SEL_I2S1_OSC;
    }
    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_resample_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_OSC;
    } else {
        val = CMU_SEL_I2S1_OSC;
    }
    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_select_external(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_IN;
    } else {
        val = CMU_SEL_I2S1_IN;
    }

    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_select_internal(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_IN;
    } else {
        val = CMU_SEL_I2S1_IN;
    }

    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

int hal_cmu_i2s_set_div(enum HAL_I2S_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (CMU_CFG_DIV_I2S0_MASK >> CMU_CFG_DIV_I2S0_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    if (id == HAL_I2S_ID_0) {
        cmu->I2S0_CLK = SET_BITFIELD(cmu->I2S0_CLK, CMU_CFG_DIV_I2S0, div);
    } else {
        cmu->I2S1_CLK = SET_BITFIELD(cmu->I2S1_CLK, CMU_CFG_DIV_I2S1, div);
    }
    int_unlock(lock);

    return 0;
}

int hal_cmu_spdif_clock_enable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;
    uint32_t mask;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    mask = CMU_EN_CLK_PLL_SPDIF0;

    lock = int_lock();
    cmu->I2S1_CLK |= mask;
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_disable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;
    uint32_t mask;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    mask = CMU_EN_CLK_PLL_SPDIF0;

    lock = int_lock();
    cmu->I2S1_CLK &= ~mask;
    int_unlock(lock);

    return 0;
}

int hal_cmu_spdif_set_div(enum HAL_SPDIF_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    if (div < 2) {
        return 2;
    }

    div -= 2;
    if ((div & (CMU_CFG_DIV_SPDIF0_MASK >> CMU_CFG_DIV_SPDIF0_SHIFT)) != div) {
        return 2;
    }

    lock = int_lock();
    cmu->I2S1_CLK = SET_BITFIELD(cmu->I2S1_CLK, CMU_CFG_DIV_SPDIF0, div);
    int_unlock(lock);
    return 0;
}

int hal_cmu_i2s_mclk_enable(enum HAL_CMU_I2S_MCLK_ID_T id)
{
    uint32_t lock;

    lock = iomux_lock();
    cmu->I2C_CLK = SET_BITFIELD(cmu->I2C_CLK, CMU_SEL_I2S_MCLK, id) | CMU_EN_I2S_MCLK;
    aoncmu->AON_CLK = (aoncmu->AON_CLK & ~AON_CMU_SEL_I2S_MCLK_MASK) |
        AON_CMU_SEL_I2S_MCLK(CMU_I2S_MCLK_SEL_MCU) | AON_CMU_EN_I2S_MCLK;
    iomux_unlock(lock);

    return 0;
}

void hal_cmu_i2s_mclk_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK &= ~CMU_EN_I2S_MCLK;
    aoncmu->AON_CLK &= ~AON_CMU_EN_I2S_MCLK;
    int_unlock(lock);
}

int hal_cmu_pwm_set_freq(enum HAL_PWM_ID_T id, uint32_t freq)
{
    uint32_t lock;
    int clk_32k;
    uint32_t div;

    if (id >= HAL_PWM_ID_QTY) {
        return 1;
    }

    if (freq == 0) {
        clk_32k = 1;
        div = 0;
    } else {
        clk_32k = 0;
        div = hal_cmu_get_crystal_freq() / freq;
        if (div < 2) {
            return 1;
        }

        div -= 2;
        if ((div & (CMU_CFG_DIV_PWM0_MASK >> CMU_CFG_DIV_PWM0_SHIFT)) != div) {
            return 1;
        }
    }

    lock = int_lock();
    if (id == HAL_PWM_ID_0) {
        cmu->PWM01_CLK = (cmu->PWM01_CLK & ~(CMU_CFG_DIV_PWM0_MASK | CMU_SEL_OSC_PWM0 | CMU_EN_OSC_PWM0)) |
            CMU_CFG_DIV_PWM0(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM0 | CMU_EN_OSC_PWM0));
    } else if (id == HAL_PWM_ID_1) {
        cmu->PWM01_CLK = (cmu->PWM01_CLK & ~(CMU_CFG_DIV_PWM1_MASK | CMU_SEL_OSC_PWM1 | CMU_EN_OSC_PWM1)) |
            CMU_CFG_DIV_PWM1(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM1 | CMU_EN_OSC_PWM1));
    } else if (id == HAL_PWM_ID_2) {
        cmu->PWM23_CLK = (cmu->PWM23_CLK & ~(CMU_CFG_DIV_PWM2_MASK | CMU_SEL_OSC_PWM2 | CMU_EN_OSC_PWM2)) |
            CMU_CFG_DIV_PWM2(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM2 | CMU_EN_OSC_PWM2));
    } else {
        cmu->PWM23_CLK = (cmu->PWM23_CLK & ~(CMU_CFG_DIV_PWM3_MASK | CMU_SEL_OSC_PWM3 | CMU_EN_OSC_PWM3)) |
            CMU_CFG_DIV_PWM3(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM3 | CMU_EN_OSC_PWM3));
    }
    int_unlock(lock);
    return 0;
}

void hal_cmu_storage_domain_enable(enum HAL_PSC_STORAGE_MOD_T mod)
{
    uint32_t lock;

    lock = int_lock();
    if (mod > HAL_PSC_STORAGE_MOD_QTY) {
        return;
    }

    if (store_mod_map == 0) {
        hal_cmu_clock_enable(HAL_CMU_MOD_X_EMMC);
        hal_cmu_reset_clear(HAL_CMU_MOD_X_EMMC);
    }
    store_mod_map |= (1 << mod);
    int_unlock(lock);
}

void hal_cmu_storage_domain_disable(enum HAL_PSC_STORAGE_MOD_T mod)
{
    uint32_t lock;

    lock = int_lock();
    if (mod > HAL_PSC_STORAGE_MOD_QTY) {
        return;
    }

    if (store_mod_map == (1 << mod)) {
        hal_cmu_reset_set(HAL_CMU_MOD_X_EMMC);
        hal_cmu_clock_disable(HAL_CMU_MOD_X_EMMC);
    }
    store_mod_map &= ~(1 << mod);
    int_unlock(lock);
}

#ifdef CHIP_HAS_USB
void hal_cmu_usb_set_device_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->SYS_DIV |= CMU_USB_ID;
    int_unlock(lock);
}

void hal_cmu_usb_set_host_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->SYS_DIV &= ~CMU_USB_ID;
    int_unlock(lock);
}

static uint32_t hal_cmu_usb_get_clock_source(void)
{
    uint32_t src;

#ifdef USB_HIGH_SPEED
    src = CMU_USB_CLK_SRC_PLL;
#else
    src = CMU_USB_CLK_SRC_OSC_24M_X2;
#endif

    return src;
}

void hal_cmu_sys_usb_clock_enable(void)
{
    enum HAL_CMU_PLL_T pll;
    uint32_t lock;
    uint32_t src;

    hal_psc_storage_enable(HAL_PSC_STORAGE_MOD_USB);
    hal_cmu_storage_domain_enable(HAL_PSC_STORAGE_MOD_USB);

    pll = HAL_CMU_PLL_USB;
    src = hal_cmu_usb_get_clock_source();

    if (src == CMU_USB_CLK_SRC_PLL) {
        hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_USB);
    }

    lock = int_lock();
    if (src == CMU_USB_CLK_SRC_PLL) {
        cmu->SYS_DIV &= ~CMU_SEL_USB_SRC;
    } else {
        cmu->SYS_DIV |= CMU_SEL_USB_SRC;
    }
    int_unlock(lock);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_USBC);
#ifdef USB_HIGH_SPEED
    hal_cmu_clock_enable(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_clock_enable(HAL_CMU_MOD_O_USB32K);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_sys_timer_delay(US_TO_TICKS(60));
    hal_cmu_reset_clear(HAL_CMU_MOD_H_USBC);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_clear(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_clear(HAL_CMU_MOD_O_USB32K);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_USB);
}

void hal_cmu_sys_usb_clock_disable(void)
{
    enum HAL_CMU_PLL_T pll;
    uint32_t src;

    pll = HAL_CMU_PLL_USB;
    src = hal_cmu_usb_get_clock_source();

    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_USB);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_clock_disable(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_clock_disable(HAL_CMU_MOD_H_USBC);

    if (src == CMU_USB_CLK_SRC_PLL) {
        hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_USB);
    }

    hal_cmu_storage_domain_disable(HAL_PSC_STORAGE_MOD_USB);
    hal_psc_storage_disable(HAL_PSC_STORAGE_MOD_USB);
}

#ifndef CHIP_SUBSYS_BTH
void hal_cmu_usb_clock_enable(void) __attribute__((alias("hal_cmu_sys_usb_clock_enable")));
void hal_cmu_usb_clock_disable(void) __attribute__((alias("hal_cmu_sys_usb_clock_disable")));
#endif
#endif

#define SYS_PERIPH_SET_DIV_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_div(uint32_t div) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (div < 2 || div > ((CMU_CFG_DIV_ ##F## _MASK >> CMU_CFG_DIV_ ##F## _SHIFT) + 2)) { \
        cmu->r &= ~(CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F); \
        ret = 1; \
    } else { \
        div -= 2; \
        cmu->r = (cmu->r & ~(CMU_CFG_DIV_ ##F## _MASK)) | CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | \
            CMU_CFG_DIV_ ##F(div); \
        cmu->r |= CMU_EN_PLL_ ##F; \
    } \
    int_unlock(lock); \
    return ret; \
}

SYS_PERIPH_SET_DIV_FUNC(sys_uart0, UART0, UART_CLK);
SYS_PERIPH_SET_DIV_FUNC(sys_uart1, UART1, UART_CLK);
SYS_PERIPH_SET_DIV_FUNC(sys_spi, SPI0, SYS_DIV);
SYS_PERIPH_SET_DIV_FUNC(sys_slcd, SPI1, SYS_DIV);
SYS_PERIPH_SET_DIV_FUNC(sdmmc0, SDMMC, PERIPH_CLK);
SYS_PERIPH_SET_DIV_FUNC(i2c, I2C, I2C_CLK);

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_USE_SYS_PERIPH)
int hal_cmu_uart2_set_div(uint32_t div) __attribute__((alias("hal_cmu_sys_uart0_set_div")));
#elif !defined(CHIP_SUBSYS_BTH)
int hal_cmu_uart0_set_div(uint32_t div) __attribute__((alias("hal_cmu_sys_uart0_set_div")));
int hal_cmu_uart1_set_div(uint32_t div) __attribute__((alias("hal_cmu_sys_uart1_set_div")));
int hal_cmu_spi_set_div(uint32_t div) __attribute__((alias("hal_cmu_sys_spi_set_div")));
int hal_cmu_slcd_set_div(uint32_t div) __attribute__((alias("hal_cmu_sys_slcd_set_div")));
#endif

int hal_cmu_emmc_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint32_t sel;

    if (pll == HAL_CMU_PLL_BB) {
        sel = 0;
    } else if (pll == HAL_CMU_PLL_USB) {
        sel = 1;
    } else if (pll == HAL_CMU_PLL_DSI) {
        sel = 4;
    } else {
        // OSCX4
        sel = 6;
    }

    lock = int_lock();
    cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_SRC_PLL_EMMC, sel);
    cmu->PERIPH_CLK |= CMU_SEL_PLL_EMMC | CMU_EN_PLL_EMMC;
    int_unlock(lock);

    return 0;
}

#define SYS_PERIPH_SET_FREQ_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (freq == HAL_CMU_PERIPH_FREQ_26M) { \
        cmu->r &= ~(CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F); \
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) { \
        cmu->r = (cmu->r & ~(CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F)) | CMU_SEL_OSCX2_ ##F; \
    } else { \
        ret = 1; \
    } \
    int_unlock(lock); \
    return ret; \
}

SYS_PERIPH_SET_FREQ_FUNC(sys_uart0, UART0, UART_CLK);
SYS_PERIPH_SET_FREQ_FUNC(sys_uart1, UART1, UART_CLK);
SYS_PERIPH_SET_FREQ_FUNC(sys_spi, SPI0, SYS_DIV);
SYS_PERIPH_SET_FREQ_FUNC(sys_slcd, SPI1, SYS_DIV);
SYS_PERIPH_SET_FREQ_FUNC(sdmmc0, SDMMC, PERIPH_CLK);
SYS_PERIPH_SET_FREQ_FUNC(emmc, EMMC, PERIPH_CLK);
SYS_PERIPH_SET_FREQ_FUNC(i2c, I2C, I2C_CLK);

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_USE_SYS_PERIPH)
int hal_cmu_uart2_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) __attribute__((alias("hal_cmu_sys_uart0_set_freq")));
#elif !defined(CHIP_SUBSYS_BTH)
int hal_cmu_uart0_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) __attribute__((alias("hal_cmu_sys_uart0_set_freq")));
int hal_cmu_uart1_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) __attribute__((alias("hal_cmu_sys_uart1_set_freq")));
int hal_cmu_spi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) __attribute__((alias("hal_cmu_sys_spi_set_freq")));
int hal_cmu_slcd_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) __attribute__((alias("hal_cmu_sys_slcd_set_freq")));
#endif

void hal_cmu_sys_emmc_clock_enable(void)
{
    hal_psc_storage_enable(HAL_PSC_STORAGE_MOD_EMMC);
    hal_cmu_storage_domain_enable(HAL_PSC_STORAGE_MOD_EMMC);

    if (EMMC_PLL_SEL < HAL_CMU_PLL_QTY) {
        hal_cmu_pll_enable(EMMC_PLL_SEL, HAL_CMU_PLL_USER_EMMC);
    }
    hal_cmu_emmc_select_pll(EMMC_PLL_SEL);

    hal_cmu_clock_enable(HAL_CMU_MOD_O_EMMC);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_EMMC);
}

void hal_cmu_sys_emmc_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_O_EMMC);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_EMMC);

    if (EMMC_PLL_SEL < HAL_CMU_PLL_QTY) {
        hal_cmu_pll_disable(EMMC_PLL_SEL, HAL_CMU_PLL_USER_EMMC);
    }

    hal_cmu_storage_domain_disable(HAL_PSC_STORAGE_MOD_EMMC);
    hal_psc_storage_disable(HAL_PSC_STORAGE_MOD_EMMC);
}

#ifndef CHIP_SUBSYS_BTH
void hal_cmu_emmc_clock_enable(void) __attribute__((alias("hal_cmu_sys_emmc_clock_enable")));
void hal_cmu_emmc_clock_disable(void) __attribute__((alias("hal_cmu_sys_emmc_clock_disable")));
#endif

void hal_cmu_emmc_clock_continue(void)
{
    hal_cmu_clock_enable(HAL_CMU_MOD_O_EMMC);
}

void hal_cmu_emmc_clock_pause(void)
{
    hal_cmu_clock_set_mode(HAL_CMU_MOD_O_EMMC, HAL_CMU_CLK_MANUAL);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_EMMC);
}

void hal_cmu_sys_sdmmc0_clock_enable(void)
{
    hal_psc_storage_enable(HAL_PSC_STORAGE_MOD_SDIO);
    hal_cmu_storage_domain_enable(HAL_PSC_STORAGE_MOD_SDIO);

    hal_cmu_clock_enable(HAL_CMU_MOD_O_SDMMC);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_SDMMC);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_SDMMC);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_SDMMC);
}

void hal_cmu_sys_sdmmc0_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_SDMMC);
    hal_cmu_reset_set(HAL_CMU_MOD_O_SDMMC);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_SDMMC);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_SDMMC);

    hal_cmu_storage_domain_disable(HAL_PSC_STORAGE_MOD_SDIO);
    hal_psc_storage_disable(HAL_PSC_STORAGE_MOD_SDIO);
}

#ifndef CHIP_SUBSYS_BTH
void hal_cmu_sdmmc0_clock_enable(void) __attribute__((alias("hal_cmu_sys_sdmmc0_clock_enable")));
void hal_cmu_sdmmc0_clock_disable(void) __attribute__((alias("hal_cmu_sys_sdmmc0_clock_disable")));
#endif

void hal_cmu_sec_eng_clock_enable(void)
{
    hal_psc_storage_enable(HAL_PSC_STORAGE_MOD_SEC_ENG);
    hal_cmu_storage_domain_enable(HAL_PSC_STORAGE_MOD_SEC_ENG);

    hal_cmu_clock_enable(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_clock_enable(HAL_CMU_MOD_P_SEC_ENG);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_reset_clear(HAL_CMU_MOD_P_SEC_ENG);
}

void hal_cmu_sec_eng_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_reset_set(HAL_CMU_MOD_P_SEC_ENG);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_clock_disable(HAL_CMU_MOD_P_SEC_ENG);

    hal_cmu_storage_domain_disable(HAL_PSC_STORAGE_MOD_SEC_ENG);
    hal_psc_storage_disable(HAL_PSC_STORAGE_MOD_SEC_ENG);
}

void hal_cmu_codec_dma_ack_select(enum HAL_CMU_CODEC_DMA_ACK_T ack, bool sys_dma)
{
    uint32_t lock;

    lock = iomux_lock();

    uint32_t sel = GET_BITFIELD(aoncmu->RESERVED_03C, AON_CMU_RESERVED_CODEC_DMA_SEL);
    if (sys_dma) {
        sel |= ack;
    } else {
        sel &= ~ack;
    }
    aoncmu->RESERVED_03C = SET_BITFIELD(aoncmu->RESERVED_03C, AON_CMU_RESERVED_CODEC_DMA_SEL, sel);

    if (sys_dma) {
        // SYS DMA handle codec REQ/ACK
        aoncmu->AON_CLK |= AON_CMU_SEL_CODEC_DMA_SENS;
    } else {
        // BTH DMA handle codec REQ/ACK
        aoncmu->AON_CLK &= ~AON_CMU_SEL_CODEC_DMA_SENS;
    }
    iomux_unlock(lock);
}

void hal_cmu_codec_clock_enable(void)
{
    uint32_t clk;
    uint32_t lock;
    bool sys_dma_sel = true;

    lock = iomux_lock();
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
#ifdef BTH_CODEC_I2C_SLAVE
    //enable codec xclk
    cmu->XCLK_ENABLE |= SYS_XCLK_CODEC;
    //codec select sys clock
    aoncmu->CLK_OUT &= ~AON_CMU_SEL_CODEC_HCLK_SENS_AON;
#else
    // codec select bth clock
    aoncmu->CLK_OUT |= AON_CMU_SEL_CODEC_HCLK_SENS_AON;
#endif

#ifdef BTH_CODEC_REQ_SYS_DMA
    // SYS DMA handle codec REQ/ACK
    sys_dma_sel = true;
#else
    // BTH DMA handle codec REQ/ACK
    sys_dma_sel = false;
#endif /* BTH_CODEC_REQ_SYS_DMA */

#else /* !defined(BTH_AS_MAIN_MCU) */
    aoncmu->CLK_OUT &= ~AON_CMU_SEL_CODEC_HCLK_SENS_AON;

    // SYS DMA handle codec REQ/ACK
    sys_dma_sel = true;
#endif
    iomux_unlock(lock);

    hal_cmu_codec_dma_ack_select(HAL_CMU_CODEC_DMA_ACK_ALL, sys_dma_sel);

    clk = AON_CMU_EN_CLK_OSC_CODEC_ENABLE | AON_CMU_EN_CLK_OSCX2_CODEC_ENABLE;
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
    if (hal_cmu_get_audio_resample_status()) {
        clk |= AON_CMU_EN_CLK_OSCX4_CODEC_ENABLE;
    }
    else
#endif
    {
        if (AUDIO_PLL_SEL == HAL_CMU_PLL_USB) {
            clk |= AON_CMU_EN_CLK_PLLUSB_CODEC_ENABLE;
        } else {
            clk |= AON_CMU_EN_CLK_PLLBB_CODEC_ENABLE;
        }
    }
    aoncmu->TOP_CLK1_ENABLE = clk;

    hal_cmu_clock_enable(HAL_CMU_MOD_H_CODEC);
}

void hal_cmu_codec_clock_disable(void)
{
    uint32_t clk;

    hal_cmu_clock_disable(HAL_CMU_MOD_H_CODEC);

    clk = AON_CMU_EN_CLK_OSC_CODEC_DISABLE | AON_CMU_EN_CLK_OSCX2_CODEC_DISABLE;
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
    if (hal_cmu_get_audio_resample_status()) {
        clk |= AON_CMU_EN_CLK_OSCX4_CODEC_DISABLE;
    }
    else
#endif
    {
        if (AUDIO_PLL_SEL == HAL_CMU_PLL_USB) {
            clk |= AON_CMU_EN_CLK_PLLUSB_CODEC_DISABLE;
        } else {
            clk |= AON_CMU_EN_CLK_PLLBB_CODEC_DISABLE;
        }
    }
    aoncmu->TOP_CLK1_DISABLE = clk;
}

void hal_cmu_codec_reset_set(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_CODEC);
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_CODEC_SET;
}

void hal_cmu_codec_reset_clear(void)
{
    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_CODEC_CLR;
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CODEC);
    aocmu_reg_update_wait();
}

int BOOT_TEXT_FLASH_LOC hal_cmu_axi_audio_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint8_t sel;

    if (pll == HAL_CMU_PLL_BB) {
        sel = 2;
    } else if (pll == HAL_CMU_PLL_USB) {
        sel = 3;
    } else {
        return 1;
    }

    lock = int_lock();
    cmu->I2S0_CLK = SET_BITFIELD(cmu->I2S0_CLK, CMU_SEL_PLL_AUD, sel);
    int_unlock(lock);

    return 0;
}

enum HAL_CMU_PLL_T hal_cmu_get_audio_pll(void)
{
    return AUDIO_PLL_SEL;
}

void hal_cmu_jtag_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    aonsec->SEC_BOOT_ACC &= ~(AON_SEC_SECURE_BOOT_JTAG | AON_SEC_SECURE_BOOT_I2C);
    int_unlock(lock);
}

void hal_cmu_jtag_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    aonsec->SEC_BOOT_ACC |= (AON_SEC_SECURE_BOOT_JTAG | AON_SEC_SECURE_BOOT_I2C);
    int_unlock(lock);
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
        val |= AON_CMU_SEL_PDM_DATAIN0_SYS;
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

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_m55_itcm_cfg(int enable)
{
    uint32_t val0, val1;
    uint32_t mask0, mask1;
    POSSIBLY_UNUSED uint32_t i;
    POSSIBLY_UNUSED uint32_t start, end;
    POSSIBLY_UNUSED enum SYS_TCM_CFG_TYPE_T type;

    val0 = val1 = 0;
    mask0 = mask1 = 0;

#if (M55_ITCM_SIZE > 0)
    start = (M55_ITCM_BASE - M55_MIN_ITCM_BASE) / SHR_RAM_BLK_SIZE;
    end = (M55_ITCM_BASE + M55_ITCM_SIZE - M55_MIN_ITCM_BASE) / SHR_RAM_BLK_SIZE;
    if (enable) {
        type = SYS_TCM_CFG_M55_ITCM;
    } else {
        type = SYS_TCM_CFG_AXI_RAM;
    }
    for (i = start; i < 10 && i < end; i++) {
        val0 |= AON_CMU_SYS_RAM_BLK0_SEL(type) << (3 * i);
        mask0 |= AON_CMU_SYS_RAM_BLK0_SEL_MASK << (3 * i);
    }
    for (; i < 16 && i < end; i++) {
        val1 |= AON_CMU_SYS_RAM_BLK10_SEL(type) << (3 * (i - 10));
        mask1 |= AON_CMU_SYS_RAM_BLK10_SEL_MASK << (3 * (i - 10));
    }
#endif

    aoncmu->SYS_RAM_SEL0 = (aoncmu->SYS_RAM_SEL0 & ~mask0) | val0;
    aoncmu->SYS_RAM_SEL1 = (aoncmu->SYS_RAM_SEL1 & ~mask1) | val1;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_m55_dtcm_cfg(int enable)
{
    uint32_t val0, val1;
    uint32_t mask0, mask1;
    POSSIBLY_UNUSED uint32_t i;
    POSSIBLY_UNUSED uint32_t start, end;
    POSSIBLY_UNUSED enum SYS_TCM_CFG_TYPE_T type;

    val0 = val1 = 0;
    mask0 = mask1 = 0;

#if (M55_DTCM_SIZE > 0)
    start = (M55_DTCM_BASE - M55_MIN_DTCM_BASE) / SHR_RAM_BLK_SIZE;
    end = (M55_DTCM_BASE + M55_DTCM_SIZE - M55_MIN_DTCM_BASE) / SHR_RAM_BLK_SIZE;
    if (enable) {
        type = SYS_TCM_CFG_M55_DTCM;
    } else {
        type = SYS_TCM_CFG_AXI_RAM;
    }
    for (i = start; i < 6 && i < end; i++) {
        val1 |= AON_CMU_SYS_RAM_BLK10_SEL(type) << (3 * (5 - i));
        mask1 |= AON_CMU_SYS_RAM_BLK10_SEL_MASK << (3 * (5 - i));
    }
    for (; i < 16 && i < end; i++) {
        val0 |= AON_CMU_SYS_RAM_BLK0_SEL(type) << (3 * (15 - i));
        mask0 |= AON_CMU_SYS_RAM_BLK0_SEL_MASK << (3 * (15 - i));
    }
#endif

    aoncmu->SYS_RAM_SEL0 = (aoncmu->SYS_RAM_SEL0 & ~mask0) | val0;
    aoncmu->SYS_RAM_SEL1 = (aoncmu->SYS_RAM_SEL1 & ~mask1) | val1;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_dsp_tcm_cfg(int enable)
{
    uint32_t val0, val1;
    uint32_t mask0, mask1;
    POSSIBLY_UNUSED uint32_t i;
    POSSIBLY_UNUSED uint32_t start, end;
    POSSIBLY_UNUSED enum SYS_TCM_CFG_TYPE_T type;

    val0 = val1 = 0;
    mask0 = mask1 = 0;

#if (HIFI4_ITCM_SIZE > 0)
    start = (HIFI4_ITCM_BASE - HIFI4_MIN_ITCM_BASE) / SHR_RAM_BLK_SIZE;
    end = (HIFI4_ITCM_BASE + HIFI4_ITCM_SIZE - HIFI4_MIN_ITCM_BASE) / SHR_RAM_BLK_SIZE;
    if (enable) {
        type = SYS_TCM_CFG_HIFI4_ITCM;
    } else {
        type = SYS_TCM_CFG_AXI_RAM;
    }
    for (i = start; i < 10 && i < end; i++) {
        val0 |= AON_CMU_SYS_RAM_BLK0_SEL(type) << (3 * i);
        mask0 |= AON_CMU_SYS_RAM_BLK0_SEL_MASK << (3 * i);
    }
    for (; i < 16 && i < end; i++) {
        val1 |= AON_CMU_SYS_RAM_BLK10_SEL(type) << (3 * (i - 10));
        mask1 |= AON_CMU_SYS_RAM_BLK10_SEL_MASK << (3 * (i - 10));
    }
#endif

#if (HIFI4_DTCM_SIZE > 0)
    start = (HIFI4_DTCM_BASE - HIFI4_MIN_DTCM_BASE) / SHR_RAM_BLK_SIZE;
    end = (HIFI4_DTCM_BASE + HIFI4_DTCM_SIZE - HIFI4_MIN_DTCM_BASE) / SHR_RAM_BLK_SIZE;
    if (enable) {
        type = SYS_TCM_CFG_HIFI4_DTCM;
    } else {
        type = SYS_TCM_CFG_AXI_RAM;
    }
    for (i = start; i < 6 && i < end; i++) {
        val1 |= AON_CMU_SYS_RAM_BLK10_SEL(type) << (3 * (5 - i));
        mask1 |= AON_CMU_SYS_RAM_BLK10_SEL_MASK << (3 * (5 - i));
    }
    for (; i < 16 && i < end; i++) {
        val0 |= AON_CMU_SYS_RAM_BLK0_SEL(type) << (3 * (15 - i));
        mask0 |= AON_CMU_SYS_RAM_BLK0_SEL_MASK << (3 * (15 - i));
    }
#endif

    aoncmu->SYS_RAM_SEL0 = (aoncmu->SYS_RAM_SEL0 & ~mask0) | val0;
    aoncmu->SYS_RAM_SEL1 = (aoncmu->SYS_RAM_SEL1 & ~mask1) | val1;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_ram_cfg(void)
{
#if (M55_SYS_RAM_SIZE > 0)

    // TCM ram cfg
#if (M55_SYS_RAM_BASE < SYS_RAM2_BASE)
    uint32_t val0, val1;
    uint32_t mask0, mask1;
    uint32_t i;
    uint32_t start, end;
    enum SYS_TCM_CFG_TYPE_T type;

    val0 = val1 = 0;
    mask0 = mask1 = 0;
    type = SYS_TCM_CFG_AXI_RAM;

    start = (M55_SYS_RAM_BASE - SYS_MIN_RAM_BASE) / SHR_RAM_BLK_SIZE;
#if (M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE < SYS_RAM2_BASE)
    end = (M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE - SYS_MIN_RAM_BASE) / SHR_RAM_BLK_SIZE;
#else
    end = 16;
#endif
    for (i = start; i < 10 && i < end; i++) {
        val0 |= AON_CMU_SYS_RAM_BLK0_SEL(type) << (3 * i);
        mask0 |= AON_CMU_SYS_RAM_BLK0_SEL_MASK << (3 * i);
    }
    for (; i < 16 && i < end; i++) {
        val1 |= AON_CMU_SYS_RAM_BLK10_SEL(type) << (3 * (i - 10));
        mask1 |= AON_CMU_SYS_RAM_BLK10_SEL_MASK << (3 * (i - 10));
    }

    aoncmu->SYS_RAM_SEL0 = (aoncmu->SYS_RAM_SEL0 & ~mask0) | val0;
    aoncmu->SYS_RAM_SEL1 = (aoncmu->SYS_RAM_SEL1 & ~mask1) | val1;
#endif

    // Shared ram cfg
#if (M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE >= SYS_RAM2_BASE)
    uint32_t map;

    map = hal_psc_sys_get_shared_ram_mask(M55_SYS_RAM_BASE, M55_SYS_RAM_SIZE);
    hal_cmu_ram_cfg_sel_update(map, HAL_CMU_RAM_CFG_SEL_SYS);
#endif

#endif
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_axi_clock_enable(enum AXI_MOD_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    if (axi_clk_map == 0) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_32K_MCU_ENABLE;
        aocmu_reg_update_wait();
    }
    if (user < AXI_MOD_USER_QTY) {
        axi_clk_map |= (1 << user);
    }
    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_axi_clock_disable(enum AXI_MOD_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    if (user == AXI_MOD_USER_ALL || axi_clk_map == (1 << user)) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_32K_MCU_DISABLE;
    }
    if (user < AXI_MOD_USER_QTY) {
        axi_clk_map &= ~(1 << user);
    }
    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_axi_reset_set(enum AXI_MOD_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    if (user == AXI_MOD_USER_ALL || axi_rst_map == (1 << user)) {
        aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_MCU_SET;
    }
    if (user < AXI_MOD_USER_QTY) {
        axi_rst_map &= ~(1 << user);
    }
    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_axi_reset_clear(enum AXI_MOD_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    if (axi_rst_map == 0) {
        aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_MCU_CLR;
        aocmu_reg_update_wait();
    }
    if (user < AXI_MOD_USER_QTY) {
        axi_rst_map |= (1 << user);
    }
    int_unlock(lock);
}

POSSIBLY_UNUSED
static void hal_cmu_subsys_axi_enable(void)
{
    hal_cmu_axi_clock_enable(AXI_MOD_USER_BTH);
    hal_cmu_axi_reset_clear(AXI_MOD_USER_BTH);
    cmu->SYS_CLK_ENABLE = CMU_EN_OSCX2_ENABLE;
    cmu->SYS_CLK_ENABLE = CMU_EN_OSCX4_ENABLE;
}

POSSIBLY_UNUSED
static void hal_cmu_subsys_bth_enable(void)
{
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_BTHCPU_SET;
    hal_cmu_bth_clock_enable();
    hal_cmu_bth_reset_clear();
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_OSCX2_ENABLE;
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_OSCX4_ENABLE;
}

#ifdef PROGRAMMER
void hal_cmu_programmer_enable_pll(void)
{
#ifndef PROGRAMMER_HAL_FULL_INIT
#ifdef CHIP_SUBSYS_BTH
#ifndef BTH_USE_SYS_FLASH
#error "BTH programmer (OTA in flash) must enable BTH_USE_SYS_FLASH"
#endif
    hal_cmu_subsys_axi_enable();
#else
    hal_cmu_subsys_bth_enable();
#endif
#endif

    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_SYS);

#ifndef PROGRAMMER_HAL_FULL_INIT
#ifdef CHIP_SUBSYS_BTH
    cmu->SYS_CLK_ENABLE = CMU_EN_PLL_ENABLE;
#else
    bthcmu->SYS_CLK_ENABLE = BTH_CMU_EN_PLL_ENABLE;
#endif
#endif
}
#endif

void hal_cmu_sys_flash_io_enable(void)
{
    uint32_t lock = int_lock();
    aoncmu->AON_CLK |= AON_CMU_PU_FLASH_IO;
    aoncmu->FLASH_IODRV &= ~AON_CMU_FLASH0_IOEN_DR;
    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_sys_flash_io_disable(void)
{
    // Power up flash pins (to make CEB pin become high)
    aoncmu->AON_CLK |= AON_CMU_PU_FLASH_IO;
    // Force IO pins to output 0 (to avoid floating leakage)
    aoncmu->FLASH_IODRV = (aoncmu->FLASH_IODRV & ~AON_CMU_FLASH0_IOEN_MASK) |
        AON_CMU_FLASH0_IOEN_DR | AON_CMU_FLASH0_IOEN(0);
}

void hal_cmu_bth_flash_io_enable(void)
{
    uint32_t lock = int_lock();
    aoncmu->AON_CLK |= AON_CMU_PU_FLASH1_IO;
    aoncmu->FLASH_IODRV &= ~AON_CMU_FLASH1_IOEN_DR;
    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_bth_flash_io_disable(void)
{
    // Power up flash pins (to make CEB pin become high)
    aoncmu->AON_CLK |= AON_CMU_PU_FLASH1_IO;
    // Force IO pins to output 0 (to avoid floating leakage)
    aoncmu->FLASH_IODRV = (aoncmu->FLASH_IODRV & ~AON_CMU_FLASH1_IOEN_MASK) |
        AON_CMU_FLASH1_IOEN_DR | AON_CMU_FLASH1_IOEN(0);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_main_module_init(void)
{
    // Avoid half 32K cycle between pu_osc off and on
    aoncmu->PD_STAB_TIMER &= ~AON_CMU_OSC_PSC_ON;

#if !defined(ARM_CMNS)
    aoncmu->AON_CLK |= AON_CMU_SEL_CLK_OSC;
    // Disable AON OSC always on
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_OSC_DISABLE;

    // Slow down PMU fast clock
#ifdef PMU_LOW_POWER_CLOCK
    aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~(AON_CMU_BYPASS_DIV_DCDC
                                           | AON_CMU_CFG_DIV_DCDC_MASK
                                           | AON_CMU_CLK_DCDC_DRV_MASK
                                           | AON_CMU_CLK_DCDC_DRV_HI_MASK))
                      | AON_CMU_CFG_DIV_DCDC(0)
                      | AON_CMU_CLK_DCDC_DRV(0)
                      | AON_CMU_CLK_DCDC_DRV_HI(0);
#else
    aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~(AON_CMU_BYPASS_DIV_DCDC | AON_CMU_CFG_DIV_DCDC_MASK)) | AON_CMU_CFG_DIV_DCDC(2);
#endif
    // Init ram sleep config (all sleep pd)
#if 0
    aoncmu->RAM_SLP0 = AON_CMU_RAM_DSLP0_MASK;
    aoncmu->RAM2_SLP0 = AON_CMU_RAM2_DSLP0_MASK;
    aoncmu->RAM3_SLP0 = AON_CMU_RAM3_DSLP0_MASK;
    aoncmu->RAM4_SLP0 = AON_CMU_RAM4_DSLP0_MASK;
    aoncmu->RAM5_SLP0 = AON_CMU_RAM5_DSLP0_MASK;
    aoncmu->RAM6_SLP0 = AON_CMU_RAM6_DSLP0_MASK;
    aoncmu->RAM7_SLP0 = AON_CMU_RAM7_DSLP0_MASK;
#else
    aoncmu->RAM_SLP0 = AON_CMU_RAM_SD0_MASK;
    aoncmu->RAM2_SLP0 = AON_CMU_RAM2_SD0_MASK;
    aoncmu->RAM3_SLP0 = AON_CMU_RAM3_SD0_MASK;
    aoncmu->RAM4_SLP0 = AON_CMU_RAM4_SD0_MASK;
    aoncmu->RAM5_SLP0 = AON_CMU_RAM5_SD0_MASK;
    aoncmu->RAM6_SLP0 = AON_CMU_RAM6_SD0_MASK;
    aoncmu->RAM7_SLP0 = AON_CMU_RAM7_SD0_MASK;
#endif
#ifdef DCDC_CLOCK_CONTROL
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_DCDC0_DISABLE;
#endif
#ifdef NO_RAM_CEN_MASK
    aoncmu->CEN_MSK &= ~(AON_CMU_SYS_CEN_MSK_EN | AON_CMU_BTH_CEN_MSK_EN | AON_CMU_BTC_CEN_MSK_EN | AON_CMU_SES_CEN_MSK_EN);
#endif
#ifdef SENS_RAM_ECC
    aoncmu->RESERVED_03C |= AON_CMU_SENS_RAM0_ECC_EN | AON_CMU_SENS_RAM1_ECC_EN;
#endif

#ifdef BTH_AS_MAIN_MCU
    hal_cmu_sys_m55_dtcm_cfg(true);
    hal_cmu_sys_m55_itcm_cfg(true);
    hal_cmu_sys_ram_cfg();
#else
    hal_cmu_bth_ram_cfg();
#endif
    hal_cmu_sys_dsp_tcm_cfg(true);
    hal_cmu_sens_ram_cfg();
    hal_cmu_btc_ram_cfg();

    aoncmu->RESET_SET = AON_CMU_ORESETN_SET(AON_ORST_BTAON);
    aoncmu->MOD_CLK_DISABLE = AON_CMU_MANUAL_OCLK_DISABLE(AON_OCLK_BTAON);

#ifndef MCU_I2C_SLAVE
    senscmu->I2C_CLK |= SENS_CMU_FORCE_PU_OFF;
    senscmu->SYS_CLK_DISABLE = SENS_CMU_SEL_SLOW_SYS_DISABLE;
    aoncmu->TOP_CLK1_DISABLE = AON_CMU_EN_CLK_32K_SENS_DISABLE;
#endif

#if defined(FLASH1_CTRL_BASE) && defined(FORCE_FLASH1_SLEEP)
    hal_norflash_force_sleep(HAL_FLASH_ID_1);
#endif

#ifdef CHIP_SUBSYS_BTH
    // BTH
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_MCUCPU_SET;

#if defined(FLASH1_CTRL_BASE) && defined(FORCE_FLASH1_SLEEP)
    bthcmu->HRESET_SET = BTH_HRST_FLASH0;
    bthcmu->HCLK_DISABLE = BTH_HCLK_FLASH0;
#endif

#if defined(BTH_USE_SYS_PERIPH) || defined(BTH_CODEC_I2C_SLAVE)
#ifdef BTH_USE_SYS_PERIPH
    hal_cmu_subsys_axi_enable();
#endif
    hal_cmu_sys_module_init();
#else
    hal_cmu_axi_reset_set(AXI_MOD_USER_ALL);
    hal_cmu_axi_clock_disable(AXI_MOD_USER_ALL);
#endif
#if !defined(FLASH1_CTRL_BASE) || defined(FORCE_FLASH1_SLEEP)
    hal_cmu_sys_flash_io_disable();
#endif

#else
    // M55
#if defined(FLASH1_CTRL_BASE) && (!defined(FORCE_FLASH1_SLEEP) || defined(PROGRAMMER))
    hal_cmu_subsys_bth_enable();
#ifdef FORCE_FLASH1_SLEEP
    hal_cmu_bth_flash_io_disable();
#endif
#else
    hal_cmu_bth_flash_io_disable();
    hal_cmu_bth_reset_set();
    hal_cmu_bth_clock_disable();
#endif

#endif // end of M55

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_OSCX2_DISABLE | AON_CMU_EN_CLK_TOP_OSC_DISABLE |
        AON_CMU_EN_CLK_32K_BT_DISABLE | AON_CMU_EN_X2_DIG_DISABLE | AON_CMU_EN_X4_DIG_DISABLE | AON_CMU_EN_CLK_TOP_OSCX4_DISABLE |
        AON_CMU_EN_BT_CLK_SYS_DISABLE | AON_CMU_EN_CLK_32K_CODEC_DISABLE;

    aoncmu->MOD_CLK_MODE = ~AON_CMU_MODE_ACLK(AON_ACLK_CMU | AON_ACLK_WDT |
        AON_ACLK_TIMER0 | AON_ACLK_PSC | AON_ACLK_IOMUX | AON_ACLK_TIMER1);

    aoncmu->FLASH_IODRV = SET_BITFIELD(aoncmu->FLASH_IODRV, AON_CMU_FLASH0_IODRV, 2);
    aoncmu->FLASH_IODRV = SET_BITFIELD(aoncmu->FLASH_IODRV, AON_CMU_FLASH1_IODRV, 2);
#endif // !ARM_CMNS

    hal_psc_init();

    hal_cmu_aonpmu_sleep_en(1);
}

void hal_cmu_dcdc_clock_bypass_div(void)
{
    aoncmu->CLK_OUT |= AON_CMU_BYPASS_DIV_DCDC;
}

void hal_cmu_sens_clock_enable(void)
{
    aoncmu->TOP_CLK1_ENABLE = AON_CMU_EN_CLK_32K_SENS_ENABLE;
    aocmu_reg_update_wait();
}

void hal_cmu_sens_clock_disable(void)
{
    aoncmu->TOP_CLK1_DISABLE = AON_CMU_EN_CLK_32K_SENS_DISABLE;
}

void hal_cmu_sens_reset_set(void)
{
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_SENS_SET | AON_CMU_SOFT_RSTN_SENSCPU_SET;
}

void hal_cmu_sens_reset_clear(void)
{
#ifndef MCU_I2C_SLAVE
    // Resume reg
    hal_cmu_sens_reset_set();
    aocmu_reg_update_wait();
#endif
    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_SENS_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_sens_start_cpu(uint32_t sp, uint32_t entry)
{
    volatile struct CORE_STARTUP_CFG_T *core_cfg;
    uint32_t cfg_addr;

    // Use (sp - 128) as the default vector. The Address must aligned to 128-byte boundary.
    cfg_addr = (sp - (1 << 7)) & ~((1 << 7) - 1);

    aoncmu->SENS_VTOR = (aoncmu->SENS_VTOR & ~AON_CMU_VTOR_CORE_SENS_MASK) | (cfg_addr & AON_CMU_VTOR_CORE_SENS_MASK);
    core_cfg = (volatile struct CORE_STARTUP_CFG_T *)cfg_addr;

    core_cfg->stack = sp;
    core_cfg->reset_hdlr = entry;
    // Flush previous write operation
    __DSB();

    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_SENSCPU_CLR;
    aocmu_reg_update_wait();
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_sens_ram_cfg(void)
{
#if (SENS_RAM_SIZE > 0 && SENS_RAM_BASE < SENS_RAM4_BASE)
    uint32_t map = 0;

    map |= (1 << 10);
#if (SENS_RAM_BASE < SENS_RAM3_BASE + SHR_RAM_BLK_SIZE)
    map |= (1 << 9);
#endif
#if (SENS_RAM_BASE < SENS_RAM3_BASE)
    map |= (1 << 8);
#endif
#if (SENS_RAM_BASE < SENS_RAM2_BASE + SHR_RAM_BLK_SIZE)
    map |= (1 << 7);
#endif
#if (SENS_RAM_BASE < SENS_RAM2_BASE)
    map |= (1 << 6);
#endif
#if (SENS_RAM_BASE < SENS_RAM1_BASE)
    map |= (1 << 5);
#endif
    hal_cmu_ram_cfg_sel_update(map, HAL_CMU_RAM_CFG_SEL_SENS);
#endif
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_bth_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_32K_BTH_ENABLE;
    aoncmu->TOP_CLK1_ENABLE = AON_CMU_EN_CLK_OSCX4_BTH_ENABLE | AON_CMU_EN_CLK_PLLBB_BTH_ENABLE;
    aocmu_reg_update_wait();
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_bth_clock_disable(void)
{
    aoncmu->TOP_CLK1_DISABLE = AON_CMU_EN_CLK_OSCX4_BTH_DISABLE | AON_CMU_EN_CLK_PLLBB_BTH_DISABLE;
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_32K_BTH_DISABLE;
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_bth_reset_set(void)
{
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_BTH_SET | AON_CMU_SOFT_RSTN_BTHCPU_SET;
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_bth_reset_clear(void)
{
    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_BTH_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_bth_start_cpu(uint32_t sp, uint32_t entry)
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

    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_BTHCPU_CLR;
    aocmu_reg_update_wait();
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_bth_ram_cfg(void)
{
#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM2_BASE)
    uint32_t map;

    map = (1 << 0);
#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM3_BASE)
    map |= (1 << 1);
#endif
#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM4_BASE)
    map |= (1 << 2);
#endif
#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM5_BASE)
    map |= (1 << 3);
#endif
#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM6_BASE)
    map |= (1 << 4);
#endif
#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM7_BASE)
    map |= (1 << 5);
#endif
    hal_cmu_ram_cfg_sel_update(map, HAL_CMU_RAM_CFG_SEL_BTH);
#endif
}

static void hal_cmu_m55_sleep_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    // Init deep sleep checking masks
    cmu->CPU_IDLE_MSK = (cmu->CPU_IDLE_MSK & ~CMU_CPU_IDLE_MASK) | CMU_BUS_DEEPSLEEP_MASK;
    int_unlock(lock);
}

static void hal_cmu_m55_sleep_disable(void)
{
    uint32_t cur;
    uint32_t clr = 0;
    uint32_t lock;

    lock = int_lock();

    cur = cmu->CPU_IDLE_MSK;

    if (cur & CMU_DSP_IDLE_MASK) {
        clr = CMU_BUS_DEEPSLEEP_MASK;
    }
    cmu->CPU_IDLE_MSK = (cmu->CPU_IDLE_MSK & ~clr) | CMU_CPU_IDLE_MASK;
    int_unlock(lock);
}

void hal_cmu_m55_clock_enable(void)
{
    hal_cmu_axi_clock_enable(AXI_MOD_USER_M55);
    aocmu_reg_update_wait();
}

void hal_cmu_m55_clock_disable(void)
{
    hal_cmu_axi_clock_disable(AXI_MOD_USER_M55);
}

void hal_cmu_m55_reset_set(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->CFG_FORCE_LP |= CMU_CFG_FORCE_LP_M55;
    int_unlock(lock);
    while ((cmu->FORCE_ACK & CMU_FORCE_ACK_M55) == 0) {}

    hal_cmu_m55_sleep_disable();
    lock = int_lock();
    cmu->OCLK_MODE |= SYS_OCLK_CORE0 | SYS_OCLK_CORETCM | SYS_OCLK_COREROM | SYS_OCLK_COREDBG;
    int_unlock(lock);
    cmu->ORESET_SET = SYS_ORST_CORE0 | SYS_ORST_CORETCM | SYS_ORST_COREROM | SYS_ORST_COREDBG;
    cmu->OCLK_DISABLE = SYS_OCLK_CORE0 | SYS_OCLK_CORETCM | SYS_OCLK_COREROM | SYS_OCLK_COREDBG;
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_MCUCPU_SET;
    hal_cmu_axi_reset_set(AXI_MOD_USER_M55);

    lock = int_lock();
    cmu->CFG_FORCE_LP &= ~CMU_CFG_FORCE_LP_M55;
    int_unlock(lock);
}

void hal_cmu_m55_reset_clear(void)
{
    uint32_t lock;

    hal_cmu_axi_reset_clear(AXI_MOD_USER_M55);
    aocmu_reg_update_wait();
    lock = int_lock();
    cmu->OCLK_MODE |= SYS_OCLK_CORE0 | SYS_OCLK_CORETCM | SYS_OCLK_COREROM | SYS_OCLK_COREDBG;
    int_unlock(lock);
    cmu->OCLK_ENABLE = SYS_OCLK_CORE0 | SYS_OCLK_CORETCM | SYS_OCLK_COREROM | SYS_OCLK_COREDBG;
    cmu->ORESET_CLR = SYS_ORST_CORE0 | SYS_ORST_CORETCM | SYS_ORST_COREROM | SYS_ORST_COREDBG;
    hal_cmu_m55_sleep_enable();
    __DSB();
    aocmu_reg_update_wait();
    lock = int_lock();
    cmu->OCLK_MODE &= ~(SYS_OCLK_CORE0 | SYS_OCLK_CORETCM | SYS_OCLK_COREROM | SYS_OCLK_COREDBG);
    int_unlock(lock);
}

void hal_cmu_m55_cpu_reset_set(void)
{
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_MCUCPU_SET;
}

void hal_cmu_m55_cpu_reset_clear(uint32_t vector)
{
    uint32_t cfg_addr;

    // Use (sp - 128) as the default vector. The Address must aligned to 128-byte boundary.
    cfg_addr = vector & ~((1 << 7) - 1);
    ASSERT(vector == cfg_addr, "%s: Bad vector=0x%X (should be 0x%X)", __func__, vector, cfg_addr);

    aonsec->CORE_VTOR = (aonsec->CORE_VTOR & ~AON_SEC_VTOR_CORE_SYS_MASK) | (cfg_addr & AON_SEC_VTOR_CORE_SYS_MASK);

    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_MCUCPU_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_m55_start_cpu(void)
{
}

static void hal_cmu_dsp_sleep_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    // Init deep sleep checking masks
    cmu->CPU_IDLE_MSK = (cmu->CPU_IDLE_MSK & ~CMU_DSP_IDLE_MASK) | CMU_BUS_DEEPSLEEP_MASK;
    int_unlock(lock);
}

static void hal_cmu_dsp_sleep_disable(void)
{
    uint32_t cur;
    uint32_t clr = 0;
    uint32_t lock;

    lock = int_lock();
    cur = cmu->CPU_IDLE_MSK;
    if (cur & CMU_CPU_IDLE_MASK) {
        clr = CMU_BUS_DEEPSLEEP_MASK;
    }
    cmu->CPU_IDLE_MSK = (cur & ~clr) | CMU_DSP_IDLE_MASK;

    int_unlock(lock);
}

void hal_cmu_dsp_clock_enable(void)
{
    hal_cmu_axi_clock_enable(AXI_MOD_USER_DSP);
    aocmu_reg_update_wait();
}

void hal_cmu_dsp_clock_disable(void)
{
    hal_cmu_axi_clock_disable(AXI_MOD_USER_DSP);
}

void hal_cmu_dsp_reset_set(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->CFG_FORCE_LP |= CMU_CFG_FORCE_LP_DSP;
    int_unlock(lock);
    while ((cmu->FORCE_ACK & CMU_FORCE_ACK_DSP) == 0) {}

    hal_cmu_dsp_cpu_reset_set();

    hal_cmu_dsp_sleep_disable();
    lock = int_lock();
    cmu->OCLK_MODE |= SYS_OCLK_DSP | SYS_OCLK_DSPTCM;
    int_unlock(lock);
    cmu->ORESET_SET = SYS_ORST_DSP | SYS_ORST_DSPTCM;
    cmu->OCLK_DISABLE = SYS_OCLK_DSP | SYS_OCLK_DSPTCM;
    cmu->ORESET_SET = SYS_ORST_WDT;
    cmu->PRESET_SET = SYS_PRST_WDT;
    cmu->OCLK_DISABLE = SYS_OCLK_WDT;
    cmu->PCLK_DISABLE = SYS_PCLK_WDT;
    hal_cmu_axi_reset_set(AXI_MOD_USER_DSP);

    lock = int_lock();
    cmu->CFG_FORCE_LP &= ~CMU_CFG_FORCE_LP_DSP;
    int_unlock(lock);
}

void hal_cmu_dsp_reset_clear(void)
{
    uint32_t lock;

    hal_cmu_axi_reset_clear(AXI_MOD_USER_DSP);
    aocmu_reg_update_wait();
    lock = int_lock();
    cmu->OCLK_MODE |= SYS_OCLK_DSP | SYS_OCLK_DSPTCM;
    int_unlock(lock);
    cmu->OCLK_ENABLE = SYS_OCLK_DSP | SYS_OCLK_DSPTCM;
    cmu->ORESET_CLR = SYS_ORST_DSP | SYS_ORST_DSPTCM;
    cmu->OCLK_ENABLE = SYS_OCLK_WDT;
    cmu->ORESET_CLR = SYS_ORST_WDT;
    cmu->PRESET_CLR = SYS_PRST_WDT;
    cmu->PCLK_ENABLE = SYS_PCLK_WDT;
    hal_cmu_dsp_sleep_enable();
    __DSB();
    aocmu_reg_update_wait();
    lock = int_lock();
    cmu->OCLK_MODE &= ~(SYS_OCLK_DSP | SYS_OCLK_DSPTCM);
    int_unlock(lock);
}

void hal_cmu_dsp_cpu_reset_set(void)
{
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_MCUDSP_SET;
}

void hal_cmu_dsp_cpu_reset_clear(uint32_t entry)
{
    aoncmu->DSP_RUNSTALL |= AON_CMU_DSP_STATVECTORSEL | AON_CMU_DSP_RUNSTALL;
    hal_cmu_dsp_set_vector(entry);
    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_MCUDSP_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_dsp_start_cpu(void)
{
    aoncmu->DSP_RUNSTALL &= ~AON_CMU_DSP_RUNSTALL;
}

void hal_cmu_dsp_set_vector(uint32_t entry)
{
    aoncmu->DSP_VTOR = (aoncmu->DSP_VTOR & ~AON_CMU_DSP_VTOR_MASK) | (entry & AON_CMU_DSP_VTOR_MASK);
}

#ifdef DCDC_CLOCK_CONTROL
void hal_cmu_dcdc_clock_enable(enum HAL_CMU_DCDC_CLOCK_USER_T user)
{
    if (user >= HAL_CMU_DCDC_CLOCK_USER_QTY) {
        return;
    }

    if (user == HAL_CMU_DCDC_CLOCK_USER_GPADC) {
        pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_GPADC, true);
    }

    if (dcdc_user_map == 0) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_DCDC0_ENABLE;
    }
    dcdc_user_map |= (1 << user);
}

void hal_cmu_dcdc_clock_disable(enum HAL_CMU_DCDC_CLOCK_USER_T user)
{
    if (user >= HAL_CMU_DCDC_CLOCK_USER_QTY) {
        return;
    }

    if (dcdc_user_map) {
        dcdc_user_map &= ~(1 << user);
        if (dcdc_user_map == 0) {
            aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_DCDC0_DISABLE;
        }
    }

    if (user == HAL_CMU_DCDC_CLOCK_USER_GPADC) {
        pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_GPADC, false);
    }
}

void BOOT_TEXT_FLASH_LOC hal_cmu_boot_dcdc_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_DCDC0_ENABLE;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_boot_dcdc_clock_disable(void)
{
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_DCDC0_DISABLE;
}
#endif

void hal_cmu_jtag_select_sys_m55(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->PERIPH_CLK &= ~CMU_JTAG_SEL_CP;
    int_unlock(lock);
}

void hal_cmu_jtag_select_sys_dsp(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->PERIPH_CLK |= CMU_JTAG_SEL_CP;
    int_unlock(lock);
}

void hal_cmu_jtag_select_sys_dual(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->SYS_DIV |= CMU_JTAG_SEL_DUAL;
    cmu->PERIPH_CLK &= ~CMU_JTAG_SEL_CP;
    int_unlock(lock);
}

int hal_cmu_sys_lpu_busy(void)
{
    if ((cmu->WAKEUP_CLK_CFG & (CMU_LPU_AUTO_SWITCH26 | CMU_LPU_STATUS_26M)) ==
            CMU_LPU_AUTO_SWITCH26) {
        return 1;
    }
    if ((cmu->WAKEUP_CLK_CFG & (CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_STATUS_PLL)) ==
            CMU_LPU_AUTO_SWITCHPLL) {
        return 1;
    }
    return 0;
}
#ifndef CHIP_SUBSYS_BTH
int hal_cmu_lpu_busy(void) __attribute__((alias("hal_cmu_sys_lpu_busy")));
#endif

int BOOT_TEXT_FLASH_LOC hal_cmu_sys_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg)
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
    if ((timer_26m & (CMU_TIMER_WT26_MASK >> CMU_TIMER_WT26_SHIFT)) != timer_26m) {
        return 2;
    }
    if ((timer_pll & (CMU_TIMER_WTPLL_MASK >> CMU_TIMER_WTPLL_SHIFT)) != timer_pll) {
        return 3;
    }
    if (hal_cmu_sys_lpu_busy()) {
        return -1;
    }

    if (cfg == HAL_CMU_LPU_CLK_26M) {
        lpu_clk = CMU_LPU_AUTO_SWITCH26;
    } else if (cfg == HAL_CMU_LPU_CLK_PLL) {
        lpu_clk = CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26;
    } else {
        lpu_clk = 0;
    }

    if (lpu_clk & CMU_LPU_AUTO_SWITCH26) {
        // Disable RAM wakeup early
        cmu->MCU_TIMER &= ~CMU_RAM_RETN_UP_EARLY;
        // MCU/ROM/RAM auto clock gating (which depends on RAM gating signal)
        cmu->HCLK_MODE &= ~(SYS_HCLK_AHB0 | SYS_HCLK_AH2H_AON);
        cmu->OCLK_MODE &= ~(SYS_OCLK_CORE0 | SYS_OCLK_CORETCM | SYS_OCLK_COREROM |
            SYS_OCLK_COREDBG | SYS_OCLK_DSP | SYS_OCLK_DSPTCM);
        cmu->XCLK_MODE &= ~(SYS_XCLK_CORE0 |
            SYS_XCLK_RAM0 | SYS_XCLK_RAM1 | SYS_XCLK_RAM2 | SYS_XCLK_RAM3 |
            SYS_XCLK_RAM4 | SYS_XCLK_RAM5);
#if !(defined(CHIP_SUBSYS_BTH) || defined(BTH_AS_MAIN_MCU))
        // AON_CMU enable auto switch 26M (AON_CMU must have selected 26M and disabled 52M/32K already)
        aoncmu->CLK_SELECT |= AON_CMU_LPU_AUTO_SWITCH26;
    } else {
        // AON_CMU disable auto switch 26M
        aoncmu->CLK_SELECT &= ~AON_CMU_LPU_AUTO_SWITCH26;
#endif
    }

#if !(defined(CHIP_SUBSYS_BTH) || defined(BTH_AS_MAIN_MCU))
    aoncmu->CLK_SELECT = SET_BITFIELD(aoncmu->CLK_SELECT, AON_CMU_TIMER_WT24, timer_26m) |
        AON_CMU_TIMER_WT24_EN | AON_CMU_OSC_READY_MODE | AON_CMU_OSC_READY_BYPASS_SYNC |
#ifdef SLOW_SYS_BYPASS
        AON_CMU_SEL_SLOW_SYS_BYPASS;
#else
        0;
#endif
#endif
    cmu->WAKEUP_CLK_CFG = CMU_TIMER_WT26(timer_26m) | CMU_TIMER_WTPLL(0) | lpu_clk;
    if (timer_pll) {
        hal_sys_timer_delay(US_TO_TICKS(60));
        cmu->WAKEUP_CLK_CFG = CMU_TIMER_WT26(timer_26m) | CMU_TIMER_WTPLL(timer_pll) | lpu_clk;
    }
    return 0;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_periph_dma_cfg0_req_init(void)
{
    // DMA channel config
    cmu->ADMA_CH0_4_REQ =
        // codec
        CMU_DMA0_CH0_REQ_IDX(CMU_DMA_REQ_CODEC_RX) | CMU_DMA0_CH1_REQ_IDX(CMU_DMA_REQ_CODEC_TX) |
#ifdef CODEC_DSD
        // codec_dsd
        CMU_DMA0_CH2_REQ_IDX(CMU_DMA_REQ_DSD_RX) | CMU_DMA0_CH3_REQ_IDX(CMU_DMA_REQ_DSD_TX) |
#else
        // btpcm
        CMU_DMA0_CH2_REQ_IDX(CMU_DMA_REQ_PCM_RX) | CMU_DMA0_CH3_REQ_IDX(CMU_DMA_REQ_PCM_TX) |
#endif
        // i2s0
        CMU_DMA0_CH4_REQ_IDX(CMU_DMA_REQ_I2S0_RX);
    cmu->ADMA_CH5_9_REQ =
        // i2s0
        CMU_DMA0_CH5_REQ_IDX(CMU_DMA_REQ_I2S0_TX) |
        // FIR
        CMU_DMA0_CH6_REQ_IDX(CMU_DMA_REQ_FIR_RX) | CMU_DMA0_CH7_REQ_IDX(CMU_DMA_REQ_FIR_TX) |
        // spdif
        CMU_DMA0_CH8_REQ_IDX(CMU_DMA_REQ_SPDIF0_RX) | CMU_DMA0_CH9_REQ_IDX(CMU_DMA_REQ_SPDIF0_TX);
    cmu->ADMA_CH10_14_REQ =
        // codec2
        CMU_DMA0_CH10_REQ_IDX(CMU_DMA_REQ_CODEC_RX2) | CMU_DMA0_CH11_REQ_IDX(CMU_DMA_REQ_CODEC_TX2) |
        // null
        CMU_DMA0_CH12_REQ_IDX(CMU_DMA_REQ_NULL) |
        // mc
        CMU_DMA0_CH13_REQ_IDX(CMU_DMA_REQ_CODEC_MC) |
        // i2s1
        CMU_DMA0_CH14_REQ_IDX(CMU_DMA_REQ_I2S1_RX);
    cmu->ADMA_CH15_REQ =
        // i2s1
        SET_BITFIELD(cmu->ADMA_CH15_REQ, CMU_DMA0_CH15_REQ_IDX, CMU_DMA_REQ_I2S1_TX);
    cmu->GDMA_CH0_4_REQ =
        // flash
        CMU_DMA1_CH0_REQ_IDX(CMU_DMA_REQ_FLS0) |
        // sdmmc
        CMU_DMA1_CH1_REQ_IDX(CMU_DMA_REQ_SDMMC0) |
        // i2c0
        CMU_DMA1_CH2_REQ_IDX(CMU_DMA_REQ_I2C0_RX) | CMU_DMA1_CH3_REQ_IDX(CMU_DMA_REQ_I2C0_TX) |
        // spi
        CMU_DMA1_CH4_REQ_IDX(CMU_DMA_REQ_SPILCD0_RX);
    cmu->GDMA_CH5_9_REQ =
        // spi
        CMU_DMA1_CH5_REQ_IDX(CMU_DMA_REQ_SPILCD0_TX) |
        // spilcd
        CMU_DMA1_CH6_REQ_IDX(CMU_DMA_REQ_SPILCD1_RX) | CMU_DMA1_CH7_REQ_IDX(CMU_DMA_REQ_SPILCD1_TX) |
        // uart0
        CMU_DMA1_CH8_REQ_IDX(CMU_DMA_REQ_UART0_RX) | CMU_DMA1_CH9_REQ_IDX(CMU_DMA_REQ_UART0_TX);
    cmu->GDMA_CH10_14_REQ =
        // uart1
        CMU_DMA1_CH10_REQ_IDX(CMU_DMA_REQ_UART1_RX) | CMU_DMA1_CH11_REQ_IDX(CMU_DMA_REQ_UART1_TX) |
        // i2c1
        CMU_DMA1_CH12_REQ_IDX(CMU_DMA_REQ_I2C1_RX) | CMU_DMA1_CH13_REQ_IDX(CMU_DMA_REQ_I2C1_TX) |
        // codec3
        CMU_DMA1_CH14_REQ_IDX(CMU_DMA_REQ_CODEC_TX3);
    cmu->GDMA_CH15_REQ =
        // null
        SET_BITFIELD(cmu->GDMA_CH15_REQ, CMU_DMA1_CH15_REQ_IDX, CMU_DMA_REQ_NULL);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_periph_dma_cfg1_req_init(void)
{
    // DMA channel config
    cmu->ADMA_CH0_4_REQ =
        // uart0
        CMU_DMA0_CH0_REQ_IDX(CMU_DMA_REQ_UART0_RX) | CMU_DMA0_CH1_REQ_IDX(CMU_DMA_REQ_UART0_TX) |
#ifdef CODEC_DSD
        // codec_dsd
        CMU_DMA0_CH2_REQ_IDX(CMU_DMA_REQ_DSD_RX) | CMU_DMA0_CH3_REQ_IDX(CMU_DMA_REQ_DSD_TX) |
#else
        // btpcm
        CMU_DMA0_CH2_REQ_IDX(CMU_DMA_REQ_PCM_RX) | CMU_DMA0_CH3_REQ_IDX(CMU_DMA_REQ_PCM_TX) |
#endif
        // i2s0
        CMU_DMA0_CH4_REQ_IDX(CMU_DMA_REQ_I2S0_RX);
    cmu->ADMA_CH5_9_REQ =
        // i2s0
        CMU_DMA0_CH5_REQ_IDX(CMU_DMA_REQ_I2S0_TX) |
        // FIR
        CMU_DMA0_CH6_REQ_IDX(CMU_DMA_REQ_FIR_RX) | CMU_DMA0_CH7_REQ_IDX(CMU_DMA_REQ_FIR_TX) |
        // codec3
        CMU_DMA0_CH8_REQ_IDX(CMU_DMA_REQ_NULL) | CMU_DMA0_CH9_REQ_IDX(CMU_DMA_REQ_CODEC_TX3);
    cmu->ADMA_CH10_14_REQ =
        // codec2 RX
        CMU_DMA0_CH10_REQ_IDX(CMU_DMA_REQ_CODEC_RX2) |
        // null
        CMU_DMA0_CH11_REQ_IDX(CMU_DMA_REQ_NULL) |
        // null
        CMU_DMA0_CH12_REQ_IDX(CMU_DMA_REQ_NULL) |
        // mc
        CMU_DMA0_CH13_REQ_IDX(CMU_DMA_REQ_CODEC_MC) |
        // uart1 RX
        CMU_DMA0_CH14_REQ_IDX(CMU_DMA_REQ_UART1_RX);
    cmu->ADMA_CH15_REQ =
        // uart1 TX
        SET_BITFIELD(cmu->ADMA_CH15_REQ, CMU_DMA0_CH15_REQ_IDX, CMU_DMA_REQ_UART1_TX);

    // GDMA used by BTH
    cmu->GDMA_CH0_4_REQ =
        // flash
        CMU_DMA1_CH0_REQ_IDX(CMU_DMA_REQ_FLS0) |
        // sdmmc
        CMU_DMA1_CH1_REQ_IDX(CMU_DMA_REQ_SDMMC0) |
        // i2c0
        CMU_DMA1_CH2_REQ_IDX(CMU_DMA_REQ_I2C0_RX) | CMU_DMA1_CH3_REQ_IDX(CMU_DMA_REQ_I2C0_TX) |
        // spi
        CMU_DMA1_CH4_REQ_IDX(CMU_DMA_REQ_SPILCD0_RX);
    cmu->GDMA_CH5_9_REQ =
        // spi
        CMU_DMA1_CH5_REQ_IDX(CMU_DMA_REQ_SPILCD0_TX) |
        // spilcd
        CMU_DMA1_CH6_REQ_IDX(CMU_DMA_REQ_SPILCD1_RX) | CMU_DMA1_CH7_REQ_IDX(CMU_DMA_REQ_SPILCD1_TX) |
        // codec
        CMU_DMA1_CH8_REQ_IDX(CMU_DMA_REQ_CODEC_RX) | CMU_DMA1_CH9_REQ_IDX(CMU_DMA_REQ_CODEC_TX);
    cmu->GDMA_CH10_14_REQ =
        // i2s1 rx
        CMU_DMA1_CH10_REQ_IDX(CMU_DMA_REQ_I2S1_RX) |
        // codec2 tx
        CMU_DMA1_CH11_REQ_IDX(CMU_DMA_REQ_CODEC_TX2) |
        // i2c1
        CMU_DMA1_CH12_REQ_IDX(CMU_DMA_REQ_I2C1_RX) | CMU_DMA1_CH13_REQ_IDX(CMU_DMA_REQ_I2C1_TX) |
        // i2s0 rx
        CMU_DMA1_CH14_REQ_IDX(CMU_DMA_REQ_I2S0_RX);
    cmu->GDMA_CH15_REQ =
        // i2s0 tx
        SET_BITFIELD(cmu->GDMA_CH15_REQ, CMU_DMA1_CH15_REQ_IDX, CMU_DMA_REQ_I2S0_TX);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_periph_dma_cfg2_req_init(void)
{
    // DMA channel config
    cmu->ADMA_CH0_4_REQ =
        // codec
        CMU_DMA0_CH0_REQ_IDX(CMU_DMA_REQ_CODEC_RX) | CMU_DMA0_CH1_REQ_IDX(CMU_DMA_REQ_CODEC_TX) |
#ifdef CODEC_DSD
        // codec_dsd
        CMU_DMA0_CH2_REQ_IDX(CMU_DMA_REQ_DSD_RX) | CMU_DMA0_CH3_REQ_IDX(CMU_DMA_REQ_DSD_TX) |
#else
        // btpcm
        CMU_DMA0_CH2_REQ_IDX(CMU_DMA_REQ_PCM_RX) | CMU_DMA0_CH3_REQ_IDX(CMU_DMA_REQ_PCM_TX) |
#endif
        // i2s0
        CMU_DMA0_CH4_REQ_IDX(CMU_DMA_REQ_I2S0_RX);
    cmu->ADMA_CH5_9_REQ =
        // i2s0
        CMU_DMA0_CH5_REQ_IDX(CMU_DMA_REQ_I2S0_TX) |
        // uart0
        CMU_DMA0_CH6_REQ_IDX(CMU_DMA_REQ_UART0_RX) | CMU_DMA0_CH7_REQ_IDX(CMU_DMA_REQ_UART0_TX) |
        // spdif
        CMU_DMA0_CH8_REQ_IDX(CMU_DMA_REQ_SPDIF0_RX) | CMU_DMA0_CH9_REQ_IDX(CMU_DMA_REQ_SPDIF0_TX);
    cmu->ADMA_CH10_14_REQ =
        // codec2
        CMU_DMA0_CH10_REQ_IDX(CMU_DMA_REQ_CODEC_RX2) | CMU_DMA0_CH11_REQ_IDX(CMU_DMA_REQ_CODEC_TX2) |
        // uart1
        CMU_DMA0_CH12_REQ_IDX(CMU_DMA_REQ_UART1_RX) | CMU_DMA0_CH13_REQ_IDX(CMU_DMA_REQ_UART1_TX) |
        // i2s1
        CMU_DMA0_CH14_REQ_IDX(CMU_DMA_REQ_I2S1_RX);
    cmu->ADMA_CH15_REQ =
        // i2s1
        SET_BITFIELD(cmu->ADMA_CH15_REQ, CMU_DMA0_CH15_REQ_IDX, CMU_DMA_REQ_I2S1_TX);

    // GDMA used by BTH
    cmu->GDMA_CH0_4_REQ =
        // null
        CMU_DMA1_CH0_REQ_IDX(CMU_DMA_REQ_NULL) |
        // sdmmc
        CMU_DMA1_CH1_REQ_IDX(CMU_DMA_REQ_SDMMC0) |
        // null
        CMU_DMA1_CH2_REQ_IDX(CMU_DMA_REQ_NULL) | CMU_DMA1_CH3_REQ_IDX(CMU_DMA_REQ_NULL) |
        // null
        CMU_DMA1_CH4_REQ_IDX(CMU_DMA_REQ_NULL);
    cmu->GDMA_CH5_9_REQ =
        // null
        CMU_DMA1_CH5_REQ_IDX(CMU_DMA_REQ_NULL) |
        // null
        CMU_DMA1_CH6_REQ_IDX(CMU_DMA_REQ_NULL) | CMU_DMA1_CH7_REQ_IDX(CMU_DMA_REQ_NULL) |
        // null
        CMU_DMA1_CH8_REQ_IDX(CMU_DMA_REQ_NULL) | CMU_DMA1_CH9_REQ_IDX(CMU_DMA_REQ_NULL);
    cmu->GDMA_CH10_14_REQ =
        // null
        CMU_DMA1_CH10_REQ_IDX(CMU_DMA_REQ_NULL) | CMU_DMA1_CH11_REQ_IDX(CMU_DMA_REQ_NULL) |
        // null
        CMU_DMA1_CH12_REQ_IDX(CMU_DMA_REQ_NULL) | CMU_DMA1_CH13_REQ_IDX(CMU_DMA_REQ_NULL) |
        // null
        CMU_DMA1_CH14_REQ_IDX(CMU_DMA_REQ_NULL);
    cmu->GDMA_CH15_REQ =
        // null
        SET_BITFIELD(cmu->GDMA_CH15_REQ, CMU_DMA1_CH15_REQ_IDX, CMU_DMA_REQ_NULL);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sys_module_init(void)
{
    // Init AXI bus clock auto gating count
    cmu->CFG_LP_NUMBER = CMU_CFG_LP_NUMBER(0x40);

    // Init DMA request channel
#if defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 1)
    hal_cmu_sys_periph_dma_cfg1_req_init();
#elif defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 2)
    hal_cmu_sys_periph_dma_cfg2_req_init();
#else
    hal_cmu_sys_periph_dma_cfg0_req_init();
#endif

#ifdef NO_LPU_26M
    while (hal_cmu_sys_lpu_init(HAL_CMU_LPU_CLK_NONE) == -1);
#else
    while (hal_cmu_sys_lpu_init(HAL_CMU_LPU_CLK_26M) == -1);
#endif

#if 1 //ndef SIMU
    cmu->XRESET_SET = SYS_XRST_GPU | SYS_XRST_DMAX | SYS_XRST_DMA2D | SYS_XRST_DISP |
        SYS_XRST_DISPB | SYS_XRST_EMMC | SYS_XRST_CODEC |
#ifndef PSRAM_ENABLE
        SYS_XRST_PSRAM |
#endif
#if defined(CHIP_SUBSYS_BTH) || defined(CHIP_SUBSYS_DSP)
        SYS_XCLK_CORE0 |
#endif
#ifndef CHIP_SUBSYS_DSP
        SYS_XCLK_DSP |
#endif
        0;
    cmu->QRESET_SET = SYS_QRST_DISPN2D | SYS_QRST_DISTV2D | SYS_QRST_LCDC_P |
        SYS_QRST_DIS_PN | SYS_QRST_DIS_TV | SYS_QRST_DIS_PIX | SYS_QRST_DIS_DSI |
        SYS_QRST_DIS_QSPI | SYS_QRST_PWM0 | SYS_QRST_PWM1 | SYS_QRST_PWM2 |
        SYS_QRST_PWM3 | SYS_QRST_PCM |
#if !defined(SYS_FLASH_IN_USE) //!(defined(SYS_FLASH_IN_USE) && defined(CHIP_SUBSYS_BTH))
        SYS_QRST_BTH2FLS |
#endif
        0;
    cmu->ORESET_SET = SYS_ORST_USB | SYS_ORST_SDMMC | SYS_ORST_WDT | SYS_ORST_TIMER1 |
        SYS_ORST_TIMER2 | SYS_ORST_TIMER3 |
        SYS_ORST_I2C0 | SYS_ORST_I2C1 | SYS_ORST_SPI0 | SYS_ORST_SPI1 |
        SYS_ORST_UART0 | SYS_ORST_UART1 | SYS_ORST_EMMC |
        SYS_ORST_I2S0 | SYS_ORST_I2S1 | SYS_ORST_SPDIF0 |
        SYS_ORST_USB32K | SYS_ORST_GPUX1 |
#ifndef PSRAM_ENABLE
        SYS_ORST_PSRAM |
#endif
#if defined(CHIP_SUBSYS_BTH) || defined(CHIP_SUBSYS_DSP)
        SYS_ORST_CORE0 | SYS_ORST_CORETCM | SYS_ORST_COREROM | SYS_ORST_COREDBG |
#endif
#ifndef CHIP_SUBSYS_DSP
        SYS_ORST_DSP | SYS_ORST_DSPTCM |
#endif
#ifndef SYS_FLASH_IN_USE
        SYS_ORST_FLASH |
#endif
#ifdef NO_ISPI
        SYS_ORST_SPI_ITN |
#endif
        0;
    cmu->PRESET_SET = SYS_PRST_WDT |
        SYS_PRST_TIMER1 | SYS_PRST_TIMER2 | SYS_PRST_TIMER3 |
        SYS_PRST_I2C0 | SYS_PRST_I2C1 |
        SYS_PRST_SPI0 | SYS_PRST_SPI1 |
        SYS_PRST_UART0 | SYS_PRST_UART1 | SYS_PRST_TRNG |
        SYS_PRST_I2S0 | SYS_PRST_I2S1 | SYS_PRST_SPDIF0 | SYS_PRST_BCM |
        SYS_PRST_PCM | SYS_PRST_IRQCTL | SYS_PRST_PWM | SYS_PRST_TCNT |
#ifdef NO_ISPI
        SYS_PRST_SPI_ITN |
#endif
        0;
    cmu->HRESET_SET = SYS_HRST_SDMMC | SYS_HRST_USBC | SYS_HRST_USBH |
        SYS_HRST_CODEC | SYS_HRST_BCM | SYS_HRST_GPU | SYS_HRST_LCDC |
        SYS_HRST_DISPPRE | SYS_HRST_DISP | SYS_HRST_DMA2D |
#ifndef PSRAM_ENABLE
        SYS_HRST_PSRAM |
#endif
#ifndef SYS_FLASH_IN_USE
        SYS_HRST_FLASH |
#endif
        0;

    cmu->XCLK_DISABLE = SYS_XCLK_GPU | SYS_XCLK_DMAX | SYS_XCLK_DMA2D | SYS_XCLK_DISP |
        SYS_XCLK_DISPB | SYS_XCLK_EMMC | SYS_XCLK_CODEC |
#ifndef PSRAM_ENABLE
        SYS_XCLK_PSRAM |
#endif
#if defined(CHIP_SUBSYS_BTH) || defined(CHIP_SUBSYS_DSP)
        SYS_XCLK_CORE0 |
#endif
#ifndef CHIP_SUBSYS_DSP
        SYS_XCLK_DSP |
#endif
        0;
    cmu->QCLK_DISABLE = SYS_QCLK_DISPN2D | SYS_QCLK_DISTV2D | SYS_QCLK_LCDC_P |
        SYS_QCLK_DIS_PN | SYS_QCLK_DIS_TV | SYS_QCLK_DIS_PIX | SYS_QCLK_DIS_DSI |
        SYS_QCLK_DIS_QSPI | SYS_QCLK_PWM0 | SYS_QCLK_PWM1 | SYS_QCLK_PWM2 |
        SYS_QCLK_PWM3 | SYS_QCLK_PCM |
#if !defined(SYS_FLASH_IN_USE) //!(defined(SYS_FLASH_IN_USE) && defined(CHIP_SUBSYS_BTH))
        SYS_QCLK_BTH2FLS |
#endif
        0;
    cmu->OCLK_DISABLE = SYS_OCLK_USB | SYS_OCLK_SDMMC | SYS_OCLK_WDT | SYS_OCLK_TIMER1 |
        SYS_OCLK_TIMER2 | SYS_OCLK_TIMER3 |
        SYS_OCLK_I2C0 | SYS_OCLK_I2C1 | SYS_OCLK_SPI0 | SYS_OCLK_SPI1 |
        SYS_OCLK_UART0 | SYS_OCLK_UART1 | SYS_OCLK_EMMC |
        SYS_OCLK_I2S0 | SYS_OCLK_I2S1 | SYS_OCLK_SPDIF0 |
        SYS_OCLK_USB32K | SYS_OCLK_GPUX1 |
#ifndef PSRAM_ENABLE
        SYS_OCLK_PSRAM |
#endif
#if defined(CHIP_SUBSYS_BTH) || defined(CHIP_SUBSYS_DSP)
        SYS_OCLK_CORE0 | SYS_OCLK_CORETCM | SYS_OCLK_COREROM | SYS_OCLK_COREDBG |
#endif
#ifndef CHIP_SUBSYS_DSP
        SYS_OCLK_DSP | SYS_OCLK_DSPTCM |
#endif
#ifndef SYS_FLASH_IN_USE
        SYS_OCLK_FLASH |
#endif
#ifdef NO_ISPI
        SYS_OCLK_SPI_ITN |
#endif
        0;
    cmu->PCLK_DISABLE = SYS_PCLK_WDT |
        SYS_PCLK_TIMER1 | SYS_PCLK_TIMER2 | SYS_PCLK_TIMER3 |
        SYS_PCLK_I2C0 | SYS_PCLK_I2C1 |
        SYS_PCLK_SPI0 | SYS_PCLK_SPI1 |
        SYS_PCLK_UART0 | SYS_PCLK_UART1 | SYS_PCLK_TRNG |
        SYS_PCLK_I2S0 | SYS_PCLK_I2S1 | SYS_PCLK_SPDIF0 | SYS_PCLK_BCM |
        SYS_PCLK_PCM | SYS_PCLK_IRQCTL | SYS_PCLK_PWM | SYS_PCLK_TCNT |
#ifdef NO_ISPI
        SYS_PCLK_SPI_ITN |
#endif
        0;
    cmu->HCLK_DISABLE = SYS_HCLK_SDMMC | SYS_HCLK_USBC | SYS_HCLK_USBH |
        SYS_HCLK_CODEC | SYS_HCLK_BCM | SYS_HCLK_GPU | SYS_HCLK_LCDC |
        SYS_HCLK_DISPPRE | SYS_HCLK_DISP | SYS_HCLK_DMA2D |
#ifndef PSRAM_ENABLE
        SYS_HCLK_PSRAM |
#endif
#ifndef SYS_FLASH_IN_USE
        SYS_HCLK_FLASH |
#endif
        0;

#if !defined(MCU_SPI_SLAVE) && !defined(CODEC_APP)
    cmu->HRESET_SET = SYS_HRST_SPI_AHB;
    cmu->HCLK_DISABLE = SYS_HCLK_SPI_AHB;
#endif

    cmu->PCLK_MODE &= ~(SYS_PCLK_CMU | SYS_PCLK_WDT | SYS_PCLK_TIMER0 | SYS_PCLK_TIMER1 | SYS_PCLK_TIMER2 |
        SYS_PCLK_TIMER3 | SYS_PCLK_IOMUX);

    //cmu->HCLK_MODE = 0;
    //cmu->PCLK_MODE = SYS_PCLK_UART0 | SYS_PCLK_UART1;
    //cmu->OCLK_MODE = 0;
#endif

#ifndef WDT_NMI_DISABLED
    // Watchdog NMI setting
    uint32_t nmi;

#ifdef BTH_AS_MAIN_MCU
    nmi = SYS_WDT_NMI_SYS_WDT;
#else
    nmi = SYS_WDT_NMI_AON_WDT;
#endif
    cmu->CPU_IDLE_MSK = SET_BITFIELD(cmu->CPU_IDLE_MSK, CMU_WDT_NMI_MASK, nmi);
#endif

#ifdef CHIP_SUBSYS_BTH
    cmu->CPU_IDLE_MSK = (cmu->CPU_IDLE_MSK & ~CMU_BUS_DEEPSLEEP_MASK) | CMU_CPU_IDLE_MASK | CMU_DSP_IDLE_MASK;
#ifdef BTH_USE_SYS_PERIPH
    cmu->MISC_0F8 = cmu->MISC_0F8 | CMU_RES3_SDMMC_IRQ_S19 | CMU_RES3_UART0_IRQ_S6;
#endif
#else
    cmu->CPU_IDLE_MSK = (cmu->CPU_IDLE_MSK & ~CMU_CPU_IDLE_MASK) | CMU_DSP_IDLE_MASK | CMU_BUS_DEEPSLEEP_MASK;
#endif

#if 1
    cmu->AXI_MANUAL_CLK = 0;
    //cmu->AXI_MANUAL_CLK = CMU_BYPASS_CG_ROM | CMU_BYPASS_CG_RAM;
#else
    cmu->AXI_MANUAL_CLK = ~0;
    cmu->CFG_LP_ENABLE = 0;
#endif

    // PSRAM
    cmu->AXI_MANUAL_CLK |= CMU_MANUAL_EN_PSRAM_MC;
    cmu->CFG_LP_ENABLE &= ~CMU_CFG_LP_ENABLE_PSRAM_MC;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_init_axi_pll_selection(void)
{
    hal_cmu_axi_sys_select_pll(hal_cmu_get_axi_sys_pll());
    hal_cmu_axi_audio_select_pll(AUDIO_PLL_SEL);
}

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
int hal_cmu_axi_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    enum HAL_CMU_PLL_T sys_pll_sel;
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

    sys_pll_sel = hal_cmu_get_axi_sys_pll();

    lock = int_lock();
#ifdef RC48M_ENABLE
    uint32_t k;

    for (k = 0; k < ARRAY_SIZE(axi_pll_user_map); k++) {
        if (axi_pll_user_map[k]) {
            break;
        }
    }
    if (k >= ARRAY_SIZE(axi_pll_user_map)) {
        // Enable OSC by pu OSCX2
        cmu->SYS_CLK_ENABLE = CMU_PU_OSCX2_ENABLE;
        hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
    }
#endif
    if (axi_pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        if (pll == sys_pll_sel) {
            cmu->SYS_CLK_ENABLE = CMU_PU_PLL_ENABLE;
        } else {
            aoncmu->TOP_CLK_ENABLE = pu_val;
        }
        // Wait at least 10us for clock ready
        hal_sys_timer_delay_us(20);
    }
    if (user < HAL_CMU_PLL_USER_QTY) {
        axi_pll_user_map[pll] |= (1 << user);
    }
    int_unlock(lock);

    start = hal_sys_timer_get();
    timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
    do {
        if (pll == sys_pll_sel) {
            if (cmu->SYS_CLK_ENABLE & CMU_EN_PLL_ENABLE) {
                break;
            }
        } else {
            if (aoncmu->TOP_CLK_ENABLE & en_val) {
                break;
            }
        }
    } while ((hal_sys_timer_get() - start) < timeout);

    if (pll == sys_pll_sel) {
        cmu->SYS_CLK_ENABLE = CMU_EN_PLL_ENABLE;
    } else {
        aoncmu->TOP_CLK_ENABLE = en_val;
    }
    aoncmu->TOP_CLK1_ENABLE = en_val1;

    return 0;
}

int hal_cmu_axi_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    enum HAL_CMU_PLL_T sys_pll_sel;
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

    sys_pll_sel = hal_cmu_get_axi_sys_pll();

    lock = int_lock();
    if (user < HAL_CMU_PLL_USER_ALL) {
        axi_pll_user_map[pll] &= ~(1 << user);
    }
    if (axi_pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        aoncmu->TOP_CLK1_DISABLE = en_val1;
        if (pll == sys_pll_sel) {
            cmu->SYS_CLK_DISABLE = CMU_EN_PLL_DISABLE;
            cmu->SYS_CLK_DISABLE = CMU_PU_PLL_DISABLE;
        } else {
            aoncmu->TOP_CLK_DISABLE = en_val;
            aoncmu->TOP_CLK_DISABLE = pu_val;
        }
    }
#ifdef RC48M_ENABLE
    uint32_t k;

    for (k = 0; k < ARRAY_SIZE(axi_pll_user_map); k++) {
        if (axi_pll_user_map[k]) {
            break;
        }
    }
    if (k >= ARRAY_SIZE(axi_pll_user_map)) {
        // Disable OSC by pd OSCX2
        cmu->SYS_CLK_DISABLE = CMU_PU_OSCX2_DISABLE;
        hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
    }
#endif
    int_unlock(lock);

    return 0;
}

void hal_cmu_axi_osc_x4_enable(void)
{
#ifdef ANA_26M_X4_ENABLE
    cmu->SYS_CLK_ENABLE = CMU_PU_OSCX4_ENABLE;
#ifdef RC48M_ENABLE
    hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
#endif
    cmu->SYS_CLK_ENABLE = CMU_EN_OSCX4_ENABLE;
#endif
}

void hal_cmu_axi_osc_x4_disable(void)
{
    cmu->SYS_CLK_DISABLE = CMU_EN_OSCX4_DISABLE;
    cmu->SYS_CLK_DISABLE = CMU_PU_OSCX4_DISABLE;
}

void hal_cmu_axi_low_freq_mode_enable(enum HAL_CMU_AXI_FREQ_T old_freq, enum HAL_CMU_AXI_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_AXI_FREQ_T low_freq;

#ifdef OSC_26M_X4_AUD2BB
    low_freq = HAL_CMU_AXI_FREQ_96M;
#else
    low_freq = HAL_CMU_AXI_FREQ_48M;
#endif

    if (old_freq > low_freq && new_freq <= low_freq) {
        pll = hal_cmu_get_axi_sys_pll();
        hal_cmu_axi_pll_disable(pll, HAL_CMU_PLL_USER_SYS);
    }
#if defined(OSC_26M_X4_AUD2BB) && defined(RC48M_ENABLE)
    if (IS_AXI_X4_FREQ(old_freq) && !IS_AXI_X4_FREQ(new_freq)) {
        hal_cmu_axi_osc_x4_disable();
    }
#endif
}

void hal_cmu_axi_low_freq_mode_disable(enum HAL_CMU_AXI_FREQ_T old_freq, enum HAL_CMU_AXI_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_AXI_FREQ_T low_freq;

#ifdef OSC_26M_X4_AUD2BB
    low_freq = HAL_CMU_AXI_FREQ_96M;
#else
    low_freq = HAL_CMU_AXI_FREQ_48M;
#endif

    if (old_freq <= low_freq && new_freq > low_freq) {
        pll = hal_cmu_get_axi_sys_pll();
        hal_cmu_axi_pll_enable(pll, HAL_CMU_PLL_USER_SYS);
    }
#if defined(OSC_26M_X4_AUD2BB) && defined(RC48M_ENABLE)
    if (!IS_AXI_X4_FREQ(old_freq) && IS_AXI_X4_FREQ(new_freq)) {
        hal_cmu_axi_osc_x4_enable();
    }
#endif
}

static enum HAL_CMU_AXI_FREQ_T hal_axifreq_revise_freq(enum HAL_CMU_AXI_FREQ_T freq)
{
    if (freq == HAL_CMU_AXI_FREQ_32K) {
        freq = HAL_CMU_AXI_FREQ_24M;
    }
    return freq;
}

int hal_cmu_axi_freq_req(enum HAL_CMU_AXI_FREQ_USER_T user, enum HAL_CMU_AXI_FREQ_T freq)
{
    uint32_t lock;
    enum HAL_CMU_AXI_FREQ_T cur_freq;
    enum HAL_CMU_AXI_FREQ_T real_freq;
    enum HAL_CMU_AXI_FREQ_T real_cur_freq;
    int i;

    if (user >= HAL_CMU_AXI_FREQ_USER_QTY) {
        return 1;
    }
    if (freq >= HAL_CMU_AXI_FREQ_QTY) {
        return 2;
    }

    lock = int_lock();

    cur_freq = hal_cmu_axi_freq_get();

    axifreq_per_user[user] = freq;

    if (freq == cur_freq) {
        axi_top_user = user;
    } else if (freq > cur_freq) {
        axi_top_user = user;
        real_freq = hal_axifreq_revise_freq(freq);
        real_cur_freq = hal_axifreq_revise_freq(cur_freq);
        // It is possible that revised freq <= revised cur_freq (e.g., when cur_freq=32K)
        if (real_freq != real_cur_freq) {
            pmu_axi_freq_config(real_freq);
#ifdef ULTRA_LOW_POWER
            // Enable PLL if required
            hal_cmu_axi_low_freq_mode_disable(real_cur_freq, real_freq);
#endif
            hal_cmu_axi_sys_set_freq(real_freq);
        }
    } else /* if (freq < cur_freq) */ {
        if (axi_top_user == user || axi_top_user == HAL_CMU_AXI_FREQ_USER_QTY) {
            if (axi_top_user == user) {
                freq = axifreq_per_user[0];
                user = 0;
                for (i = 1; i < HAL_CMU_AXI_FREQ_USER_QTY; i++) {
                    if (freq < axifreq_per_user[i]) {
                        freq = axifreq_per_user[i];
                        user = i;
                    }
                }
            }
            axi_top_user = user;
            if (freq != cur_freq) {
                real_freq = hal_axifreq_revise_freq(freq);
                real_cur_freq = hal_axifreq_revise_freq(cur_freq);
                // It is possible that revised freq >= revised cur_freq (e.g., when freq=32K)
                if (real_freq != real_cur_freq) {
                    hal_cmu_axi_sys_set_freq(real_freq);
#ifdef ULTRA_LOW_POWER
                    // Disable PLL if capable
                    hal_cmu_axi_low_freq_mode_enable(real_cur_freq, real_freq);
#endif
                    pmu_axi_freq_config(real_freq);
                }
            }
        }
    }

    int_unlock(lock);

    return 0;
}

enum HAL_CMU_AXI_FREQ_T hal_cmu_axi_freq_get(void)
{
    if (axi_top_user < HAL_CMU_AXI_FREQ_USER_QTY) {
        return axifreq_per_user[axi_top_user];
    } else {
        return HAL_CMU_AXI_FREQ_32K;
    }
}
#endif

#if !(defined(BTH_AS_MAIN_MCU) || defined(CHIP_SUBSYS_BTH))
void hal_cmu_force_sys_pll_div(uint32_t div)
{
    force_axi_sys_div = div;
}
#endif

int hal_cmu_axi_sys_set_freq(enum HAL_CMU_AXI_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    int div;
    POSSIBLY_UNUSED uint32_t sub_div;
    uint32_t lock;

    if (freq >= HAL_CMU_AXI_FREQ_QTY) {
        return 1;
    }

#ifdef LOW_SYS_FREQ
    cmu_axi_sys_freq = freq;
#endif

    div = -1;
    sub_div = 3;

    switch (freq) {
    case HAL_CMU_AXI_FREQ_32K:
        enable = CMU_SEL_OSCX2_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE | CMU_SEL_OSC_2_SYS_DISABLE | CMU_SEL_OSC_4_SYS_DISABLE |
            CMU_SEL_OSC_8_SYS_DISABLE |
            CMU_SEL_SLOW_SYS_DISABLE | CMU_SEL_FAST_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE;
        break;
    case HAL_CMU_AXI_FREQ_3M:
#if defined(LOW_SYS_FREQ) && defined(LOW_SYS_FREQ_3P25M)
        enable = CMU_SEL_OSC_8_SYS_ENABLE | CMU_SEL_OSC_4_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE | CMU_SEL_SLOW_SYS_DISABLE |
            CMU_SEL_FAST_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE;
        break;
#endif
    case HAL_CMU_AXI_FREQ_6M:
#ifdef LOW_SYS_FREQ
        enable = CMU_SEL_OSC_4_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE | CMU_SEL_OSC_8_SYS_DISABLE | CMU_SEL_SLOW_SYS_DISABLE |
            CMU_SEL_FAST_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE;
        break;
#endif
    case HAL_CMU_AXI_FREQ_12M:
#ifdef LOW_SYS_FREQ
        enable = CMU_SEL_OSC_2_SYS_ENABLE | CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE |
            CMU_SEL_FAST_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE;
        break;
#endif
    case HAL_CMU_AXI_FREQ_15M:
    case HAL_CMU_AXI_FREQ_18M:
    case HAL_CMU_AXI_FREQ_21M:
    case HAL_CMU_AXI_FREQ_24M:
        if (freq == HAL_CMU_AXI_FREQ_15M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_AXI_FREQ_18M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_AXI_FREQ_21M) {
            sub_div = 2;
        }
        enable = CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE | CMU_SEL_OSC_2_SYS_DISABLE |
            CMU_SEL_FAST_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE;
        break;
    case HAL_CMU_AXI_FREQ_30M:
    case HAL_CMU_AXI_FREQ_36M:
    case HAL_CMU_AXI_FREQ_42M:
    case HAL_CMU_AXI_FREQ_48M:
        if (freq == HAL_CMU_AXI_FREQ_30M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_AXI_FREQ_36M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_AXI_FREQ_42M) {
            sub_div = 2;
        }
        enable = CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE | CMU_SEL_FAST_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE;
        break;
    case HAL_CMU_AXI_FREQ_60M:
    case HAL_CMU_AXI_FREQ_72M:
    case HAL_CMU_AXI_FREQ_84M:
    case HAL_CMU_AXI_FREQ_96M:
        if (freq == HAL_CMU_AXI_FREQ_60M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_AXI_FREQ_72M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_AXI_FREQ_84M) {
            sub_div = 2;
        }
#ifdef OSC_26M_X4_AUD2BB
        enable = CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_FAST_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE | CMU_SEL_OSCX2_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE;
        break;
#else
        enable = CMU_SEL_SLOW_SYS_ENABLE | CMU_RSTN_DIV_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE | CMU_SEL_PLL_SYS_ENABLE;
        disable = CMU_BYPASS_DIV_SYS_DISABLE;
#ifdef AUD_PLL_DOUBLE
        div = 2;
#else
        div = 0;
#endif
        break;
#endif
    case HAL_CMU_AXI_FREQ_120M:
    case HAL_CMU_AXI_FREQ_144M:
    case HAL_CMU_AXI_FREQ_168M:
    case HAL_CMU_AXI_FREQ_192M:
#ifndef AUD_PLL_DOUBLE
    default:
#endif
        if (freq == HAL_CMU_AXI_FREQ_120M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_AXI_FREQ_144M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_AXI_FREQ_168M) {
            sub_div = 2;
        }
#ifdef AUD_PLL_DOUBLE
        enable = CMU_SEL_SLOW_SYS_ENABLE | CMU_RSTN_DIV_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE | CMU_SEL_PLL_SYS_ENABLE;
        disable = CMU_BYPASS_DIV_SYS_DISABLE;
        div = 0;
#else
        enable = CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE | CMU_BYPASS_DIV_SYS_ENABLE | CMU_SEL_PLL_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE;
#endif
        break;
#ifdef AUD_PLL_DOUBLE
    case HAL_CMU_AXI_FREQ_240M:
    case HAL_CMU_AXI_FREQ_290M:
    case HAL_CMU_AXI_FREQ_340M:
    case HAL_CMU_AXI_FREQ_390M:
    default:
        if (freq == HAL_CMU_AXI_FREQ_240M) {
            sub_div = 0;
        } else if (freq == HAL_CMU_AXI_FREQ_290M) {
            sub_div = 1;
        } else if (freq == HAL_CMU_AXI_FREQ_340M) {
            sub_div = 2;
        }
        enable = CMU_SEL_SLOW_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE | CMU_BYPASS_DIV_SYS_ENABLE | CMU_SEL_PLL_SYS_ENABLE;
        disable = CMU_RSTN_DIV_SYS_DISABLE;
        break;
#endif
    };

#if !(defined(BTH_AS_MAIN_MCU) || defined(CHIP_SUBSYS_BTH))
    uint8_t cur_sys_div = force_axi_sys_div;
    if (cur_sys_div) {
        if (cur_sys_div == 1) {
            enable = (enable & ~CMU_RSTN_DIV_SYS_ENABLE) | CMU_BYPASS_DIV_SYS_ENABLE;
            disable = (disable & ~CMU_BYPASS_DIV_SYS_DISABLE) | CMU_RSTN_DIV_SYS_DISABLE;
        } else {
            div = cur_sys_div - 2;
            enable = (enable & ~CMU_BYPASS_DIV_SYS_ENABLE) | CMU_RSTN_DIV_SYS_ENABLE;
            disable = (disable & ~CMU_RSTN_DIV_SYS_DISABLE) | CMU_BYPASS_DIV_SYS_DISABLE;
        }
    }
#endif

    lock = int_lock();
#if !(defined(PSRAM_ENABLE) && (PSRAM_SPEED > 160))
    cmu->UART_CLK = SET_BITFIELD(cmu->UART_CLK, CMU_SEL_SYS_GT, sub_div);
#endif
    if (div >= 0) {
        cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_SYS, div);
    }
    int_unlock(lock);

    if (enable & CMU_SEL_PLL_SYS_ENABLE) {
        cmu->SYS_CLK_ENABLE = CMU_RSTN_DIV_SYS_ENABLE;
        if (enable & CMU_BYPASS_DIV_SYS_ENABLE) {
            cmu->SYS_CLK_ENABLE = CMU_BYPASS_DIV_SYS_ENABLE;
        } else {
            cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_SYS_DISABLE;
        }
    }
    cmu->SYS_CLK_ENABLE = enable;
    if (enable & CMU_SEL_PLL_SYS_ENABLE) {
        cmu->SYS_CLK_DISABLE = disable;
    } else {
        cmu->SYS_CLK_DISABLE = disable & ~(CMU_RSTN_DIV_SYS_DISABLE | CMU_BYPASS_DIV_SYS_DISABLE);
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_SYS_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_SYS_DISABLE;
    }

    return 0;
}

#ifndef CHIP_SUBSYS_BTH
int hal_cmu_sys_set_freq(enum HAL_CMU_AXI_FREQ_T freq) __attribute__((alias("hal_cmu_axi_sys_set_freq")));
#endif

enum HAL_CMU_AXI_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_axi_sys_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = cmu->SYS_CLK_ENABLE;

    if (sys_clk & CMU_SEL_PLL_SYS_ENABLE) {
        if (sys_clk & CMU_BYPASS_DIV_SYS_ENABLE) {
            return HAL_CMU_AXI_FREQ_192M;
        } else {
            div = GET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_SYS);
            if (div == 0) {
                return HAL_CMU_AXI_FREQ_96M;
            } else if (div == 1) {
                // (div == 1): 64M
                return HAL_CMU_AXI_FREQ_78M;
            } else {
                // (div == 2): 48M
                // (div == 3): 38M
                return HAL_CMU_AXI_FREQ_48M;
            }
        }
    } else if ((sys_clk & (CMU_SEL_FAST_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE)) ==
            (CMU_SEL_FAST_SYS_ENABLE)) {
        return HAL_CMU_AXI_FREQ_96M;
    } else if ((sys_clk & (CMU_SEL_FAST_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE)) ==
            (CMU_SEL_FAST_SYS_ENABLE | CMU_SEL_OSCX2_SYS_ENABLE)) {
        return HAL_CMU_AXI_FREQ_48M;
    } else if (sys_clk & CMU_SEL_SLOW_SYS_ENABLE) {
        return HAL_CMU_AXI_FREQ_24M;
    } else {
        return HAL_CMU_AXI_FREQ_32K;
    }
}

#ifndef CHIP_SUBSYS_BTH
enum HAL_CMU_AXI_FREQ_T hal_cmu_sys_get_freq(void) __attribute__((alias("hal_cmu_axi_sys_get_freq")));
#endif

enum HAL_CMU_PLL_T BOOT_TEXT_FLASH_LOC hal_cmu_get_axi_sys_pll(void)
{
    enum HAL_CMU_PLL_T sys;

#ifdef SYS_USE_USBPLL
    sys = HAL_CMU_PLL_USB;
#elif defined(SYS_USE_DSIPLL)
    sys = HAL_CMU_PLL_DSI;
#else
    sys = HAL_CMU_PLL_BB;
#if defined(MCU_HIGH_PERFORMANCE_MODE) && !defined(__AUDIO_RESAMPLE__)
#error "AUDIO_RESAMPLE should be used with MCU_HIGH_PERFORMANCE_MODE"
#endif
#endif

    return sys;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_axi_sys_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint8_t sel;

    // 0:bbpll, 1:dsipll, 2:audpll, 3:usbpll
    if (pll == HAL_CMU_PLL_BB) {
        sel = 0;
    } else if (pll == HAL_CMU_PLL_DSI) {
        sel = 1;
    } else if (pll == HAL_CMU_PLL_USB) {
        sel = 3;
    } else {
        return 1;
    }

    lock = int_lock();
    // For PLL clock selection
    cmu->UART_CLK = SET_BITFIELD(cmu->UART_CLK, CMU_SEL_PLL_SOURCE, sel);
    int_unlock(lock);

    return 0;
}

int hal_cmu_mem_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_BB) {
        cmu->SYS_DIV |= CMU_SEL_PLLBB_PSRAM;
    } else {
        cmu->SYS_DIV &= ~CMU_SEL_PLLBB_PSRAM;
    }
    int_unlock(lock);

    return 0;
}

static void hal_cmu_mem_pll_enable(void)
{
    enum HAL_CMU_PLL_T mem;

    mem = MEM_PLL_SEL;
    hal_cmu_mem_select_pll(mem);

    if (mem >= HAL_CMU_PLL_QTY) {
        mem = hal_cmu_get_axi_sys_pll();
    }
    hal_cmu_pll_enable(mem, HAL_CMU_PLL_USER_PSRAM);
}

static void hal_cmu_mem_pll_disable(void)
{
    enum HAL_CMU_PLL_T mem;

    mem = MEM_PLL_SEL;
    if (mem >= HAL_CMU_PLL_QTY) {
        mem = hal_cmu_get_axi_sys_pll();
    }
    hal_cmu_pll_disable(mem, HAL_CMU_PLL_USER_PSRAM);
}

int hal_cmu_mem_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    bool clk_en;
    uint32_t prev_en;
    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }
    if (freq <= HAL_CMU_FREQ_24M) {
        enable = 0;
        disable = CMU_SEL_OSCX2_PSRAM_DISABLE | CMU_SEL_OSCX4_PSRAM_DISABLE |
            CMU_SEL_PLL_PSRAM_DISABLE;
    } else if (freq <= HAL_CMU_FREQ_48M) {
        enable = CMU_SEL_OSCX2_PSRAM_ENABLE;
        disable = CMU_SEL_OSCX4_PSRAM_DISABLE | CMU_SEL_PLL_PSRAM_DISABLE;
#ifdef OSC_26M_X4_AUD2BB
    } else if (freq <= HAL_CMU_FREQ_96M) {
        enable = CMU_SEL_OSCX4_PSRAM_ENABLE;
        disable = CMU_SEL_PLL_PSRAM_DISABLE;
#endif
    } else {
        enable = CMU_SEL_PLL_PSRAM_ENABLE;
        disable = 0;
    }
#ifdef PSRAM_CLK_FROM_DIG
    enable |= CMU_RSTN_DIV_PSRAM_ENABLE;
#else
    // Psram clk from phy
    disable |= CMU_RSTN_DIV_PSRAM_DISABLE;
#endif
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_PSRAM);
    if (clk_en) {
        cmu->OCLK_DISABLE = SYS_OCLK_PSRAM;
        cmu->HCLK_DISABLE = SYS_HCLK_PSRAM;
        cmu->HCLK_DISABLE;
        __DSB();
    }
    prev_en = cmu->SYS_CLK_ENABLE;
    if ((enable & CMU_SEL_PLL_PSRAM_ENABLE) && (prev_en & CMU_SEL_PLL_PSRAM_ENABLE) == 0) {
        hal_cmu_mem_pll_enable();
        __DSB();
    }
    cmu->SYS_CLK_ENABLE = enable;
    cmu->SYS_CLK_DISABLE = disable;
    if (clk_en) {
        cmu->HCLK_ENABLE = SYS_HCLK_PSRAM;
        cmu->OCLK_ENABLE = SYS_OCLK_PSRAM;
    }
    cmu->SYS_CLK_ENABLE;
    __DSB();
    if ((enable & CMU_SEL_PLL_PSRAM_ENABLE) == 0 && (prev_en & CMU_SEL_PLL_PSRAM_ENABLE)) {
        hal_cmu_mem_pll_disable();
        __DSB();
    }
    return 0;
}

void hal_cmu_psram_dll_clock_enable(void)
{
    aoncmu->PSRAM_PHY_CTRL |= AON_CMU_REG_PSRAM_DLL_CK_RDY;
}

void hal_cmu_psram_dll_clock_disable(void)
{
    aoncmu->PSRAM_PHY_CTRL &= ~AON_CMU_REG_PSRAM_DLL_CK_RDY;
}

void hal_cmu_psram_dll_reset_set(void)
{
    aoncmu->PSRAM_PHY_CTRL &= ~AON_CMU_REG_PSRAM_DLL_RESETB;
}

void hal_cmu_psram_dll_reset_clear(void)
{
    aoncmu->PSRAM_PHY_CTRL |= AON_CMU_REG_PSRAM_DLL_RESETB;
}

void hal_cmu_psram_phy_init(int dual)
{
    uint32_t dr, val;

    aoncmu->PSRAM_PHY_CTRL = (aoncmu->PSRAM_PHY_CTRL & ~(
        AON_CMU_REG_PSRAM_LDO_RES_MASK | AON_CMU_REG_PSRAM_SWRC_MASK | AON_CMU_REG_PSRAM_ITUNE_MASK |
        AON_CMU_REG_PSRAM_RX_BIAS_MASK | AON_CMU_REG_PSRAM_RX_BIAS2_MASK)) |
        AON_CMU_REG_PSRAM_LDO_RES(6) | AON_CMU_REG_PSRAM_SWRC(1) | AON_CMU_REG_PSRAM_ITUNE(2) |
        AON_CMU_REG_PSRAM_RX_BIAS(2) | AON_CMU_REG_PSRAM_RX_BIAS2(2);
    aoncmu->PSRAM_PHY_CTRL |= AON_CMU_REG_PSRAM_LDO_PU | AON_CMU_REG_PSRAM_LDO_PRECHARGE;
    hal_sys_timer_delay_us(10);
    aoncmu->PSRAM_PHY_CTRL &= ~AON_CMU_REG_PSRAM_LDO_PRECHARGE;
    aoncmu->PSRAM_PHY_CTRL |= AON_CMU_REG_PSRAM_PU_DVDD;
    hal_sys_timer_delay_us(2);
    val = AON_CMU_REG_PSRAM_PU;
    if (dual) {
        val |= AON_CMU_REG_PSRAM_PU2;
    }
    aoncmu->PSRAM_PHY_CTRL |= val;
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2) {
        pmu_itn_psram_pu_ce(dual);
    }
    hal_sys_timer_delay_us(2);
    dr = AON_CMU_REG_PSRAM_PHY_RSTN_DR;
    val = AON_CMU_REG_PSRAM_PHY_RSTN;
    if (dual) {
        dr |= AON_CMU_REG_PSRAM_PHY_RSTN2_DR;
        val |= AON_CMU_REG_PSRAM_PHY_RSTN2;
    }
    aoncmu->PSRAM_PHY_CTRL = (aoncmu->PSRAM_PHY_CTRL & ~val) | dr;
    hal_sys_timer_delay_us(2);
    aoncmu->PSRAM_PHY_CTRL |= val | dr;
    hal_sys_timer_delay_us(2);
}

void hal_cmu_psram_phy_wakeup(int dual)
{
    uint32_t val;

    aoncmu->PSRAM_PHY_CTRL |= AON_CMU_REG_PSRAM_LDO_PU | AON_CMU_REG_PSRAM_LDO_PRECHARGE;
    hal_sys_timer_delay_us(10);
    aoncmu->PSRAM_PHY_CTRL &= ~AON_CMU_REG_PSRAM_LDO_PRECHARGE;
    aoncmu->PSRAM_PHY_CTRL |= AON_CMU_REG_PSRAM_PU_DVDD;
    hal_sys_timer_delay_us(2);
    val = AON_CMU_REG_PSRAM_PU;
    if (dual) {
        val |= AON_CMU_REG_PSRAM_PU2;
    }
    aoncmu->PSRAM_PHY_CTRL |= val;
    hal_sys_timer_delay_us(2);
}

void hal_cmu_psram_phy_sleep(void)
{
    aoncmu->PSRAM_PHY_CTRL &= ~(AON_CMU_REG_PSRAM_LDO_PU | AON_CMU_REG_PSRAM_LDO_PRECHARGE |
        AON_CMU_REG_PSRAM_PU_DVDD | AON_CMU_REG_PSRAM_PU | AON_CMU_REG_PSRAM_PU2);
}

void hal_cmu_psram_auto_lp_cfg_set(void)
{
    uint32_t lock;

    lock = iomux_lock();
    aoncmu->RESERVED_03C &= ~(AON_CMU_RESERVED_PSRAM_LDO_PU_AUTO_SW | AON_CMU_RESERVED_PSRAM_LDO_PRECHARGE_AUTO_SW |
        AON_CMU_RESERVED_PSRAM_PU_DVDD_AUTO_SW | AON_CMU_RESERVED_PSRAM_PU_AUTO_SW | AON_CMU_RESERVED_PSRAM_PU2_AUTO_SW);
    // aoncmu->RESERVED_03C &= ~(AON_CMU_RESERVED_PSRAM_PU_DVDD_AUTO_SW |
    //    AON_CMU_RESERVED_PSRAM_PU_AUTO_SW | AON_CMU_RESERVED_PSRAM_PU2_AUTO_SW);
    // aoncmu->RESERVED_03C &= ~AON_CMU_RESERVED_PSRAM_DLL_CK_RDY_AUTO_SW;
    iomux_unlock(lock);
}

void hal_cmu_psram_auto_lp_cfg_clear(void)
{
    uint32_t lock;

    lock = iomux_lock();
    aoncmu->RESERVED_03C |= (AON_CMU_RESERVED_PSRAM_LDO_PU_AUTO_SW | AON_CMU_RESERVED_PSRAM_LDO_PRECHARGE_AUTO_SW |
        AON_CMU_RESERVED_PSRAM_PU_DVDD_AUTO_SW | AON_CMU_RESERVED_PSRAM_PU_AUTO_SW |
        AON_CMU_RESERVED_PSRAM_PU2_AUTO_SW | AON_CMU_RESERVED_PSRAM_DLL_CK_RDY_AUTO_SW);
    iomux_unlock(lock);
}

uint32_t hal_cmu_axi_clock_set_mode(enum HAL_CMU_AXI_CLK_ID_T id, uint32_t manual)
{
    uint32_t ret;

    if (id >= HAL_CMU_AXI_CLK_QTY)
        return 1;

    ret = cmu->AXI_MANUAL_CLK;
    if (manual) {
        cmu->AXI_MANUAL_CLK = ret | (1 << id);
    } else {
        cmu->AXI_MANUAL_CLK = ret & ~(1 << id);
    }
    __DSB();
    return (ret & (1 << id));
}

SRAM_TEXT_LOC
uint32_t hal_cmu_axi_clock_mode_all_manual(void)
{
    uint32_t ret = cmu->AXI_MANUAL_CLK;
    cmu->AXI_MANUAL_CLK = ~0UL;
    return ret;
}

SRAM_TEXT_LOC
void hal_cmu_axi_clock_mode_restore(uint32_t val)
{
    cmu->AXI_MANUAL_CLK = val;
}

#ifdef CORE_SLEEP_POWER_DOWN
SRAM_TEXT_LOC
void save_sys_cmu_regs(struct SAVED_CMU_REGS_T *sav)
{
    sav->HCLK_ENABLE        = cmu->HCLK_ENABLE;
    sav->PCLK_ENABLE        = cmu->PCLK_ENABLE;
    sav->OCLK_ENABLE        = cmu->OCLK_ENABLE;
    sav->HCLK_MODE          = cmu->HCLK_MODE;
    sav->PCLK_MODE          = cmu->PCLK_MODE;
    sav->OCLK_MODE          = cmu->OCLK_MODE;
    sav->REG_RAM_CFG0       = cmu->REG_RAM_CFG0;
    sav->HRESET_CLR         = cmu->HRESET_CLR;
    sav->PRESET_CLR         = cmu->PRESET_CLR;
    sav->ORESET_CLR         = cmu->ORESET_CLR;
    sav->MCU_TIMER          = cmu->MCU_TIMER;
    sav->SLEEP              = cmu->SLEEP;
    sav->PERIPH_CLK         = cmu->PERIPH_CLK;
    sav->SYS_CLK_ENABLE     = cmu->SYS_CLK_ENABLE;
    sav->ADMA_CH15_REQ      = cmu->ADMA_CH15_REQ;
    sav->REG_RAM_CFG1       = cmu->REG_RAM_CFG1;
    sav->UART_CLK           = cmu->UART_CLK;
    sav->I2C_CLK            = cmu->I2C_CLK;
    sav->SYS2SENS_MASK0     = cmu->SYS2SENS_MASK0;
    sav->SYS2SENS_MASK1     = cmu->SYS2SENS_MASK1;
    sav->WAKEUP_CLK_CFG     = cmu->WAKEUP_CLK_CFG;
    sav->SYS_DIV            = cmu->SYS_DIV;
    sav->SYS2BTH_INTMASK0   = cmu->SYS2BTH_INTMASK0;
    sav->SYS2BTH_INTMASK1   = cmu->SYS2BTH_INTMASK1;
    sav->ADMA_CH0_4_REQ     = cmu->ADMA_CH0_4_REQ;
    sav->ADMA_CH5_9_REQ     = cmu->ADMA_CH5_9_REQ;
    sav->ADMA_CH10_14_REQ   = cmu->ADMA_CH10_14_REQ;
    sav->GDMA_CH0_4_REQ     = cmu->GDMA_CH0_4_REQ;
    sav->GDMA_CH5_9_REQ     = cmu->GDMA_CH5_9_REQ;
    sav->GDMA_CH10_14_REQ   = cmu->GDMA_CH10_14_REQ;
    sav->GDMA_CH15_REQ      = cmu->GDMA_CH15_REQ;
    sav->MISC               = cmu->MISC;
    sav->SIMU_RES           = cmu->SIMU_RES;
    sav->MISC_0F8           = cmu->MISC_0F8;
    sav->SYS2OTH_MASK2      = cmu->SYS2OTH_MASK2;
    sav->DSI_CLK_ENABLE     = cmu->DSI_CLK_ENABLE;
    sav->I2S0_CLK           = cmu->I2S0_CLK;
    sav->I2S1_CLK           = cmu->I2S1_CLK;
    sav->REG_RAM_CFG2       = cmu->REG_RAM_CFG2;
    sav->M55_TCM_CFG        = cmu->M55_TCM_CFG;
    sav->PWM01_CLK          = cmu->PWM01_CLK;
    sav->PWM23_CLK          = cmu->PWM23_CLK;
    sav->M55_CODEC_OBS0     = cmu->M55_CODEC_OBS0;
    sav->M55_GPIO_OBS0      = cmu->M55_GPIO_OBS0;
    sav->LCDC_CLK           = cmu->LCDC_CLK;
    sav->SYS2BTC_INTMASK0   = cmu->SYS2BTC_INTMASK0;
    sav->SYS2BTC_INTMASK1   = cmu->SYS2BTC_INTMASK1;
    sav->AXI_MANUAL_CLK     = cmu->AXI_MANUAL_CLK;
    sav->DSP_WAKEUP_MASK0   = cmu->DSP_WAKEUP_MASK0;
    sav->DSP_WAKEUP_MASK1   = cmu->DSP_WAKEUP_MASK1;
    sav->DSP_WAKEUP_MASK2   = cmu->DSP_WAKEUP_MASK2;
    sav->QCLK_ENABLE        = cmu->QCLK_ENABLE;
    sav->QCLK_MODE          = cmu->QCLK_MODE;
    sav->QRESET_CLR         = cmu->QRESET_CLR;
    sav->XCLK_ENABLE        = cmu->XCLK_ENABLE;
    sav->XCLK_MODE          = cmu->XCLK_MODE;
    sav->XRESET_CLR         = cmu->XRESET_CLR;
    sav->RF2P_CFG           = cmu->RF2P_CFG;
    sav->CPU_IDLE_MSK       = cmu->CPU_IDLE_MSK;
    sav->M55_COREPACCEPT    = cmu->M55_COREPACCEPT;
    sav->CFG_LP_ENABLE      = cmu->CFG_LP_ENABLE;
    sav->CFG_FORCE_LP       = cmu->CFG_FORCE_LP;
    sav->CFG_BYPASS_CLKEN   = cmu->CFG_BYPASS_CLKEN;
    sav->CFG_LP_NUMBER      = cmu->CFG_LP_NUMBER;
    sav->FORCE_ACK          = cmu->FORCE_ACK;
    sav->RD_TIMER_TRIG      = cmu->RD_TIMER_TRIG;
    sav->RD_TIMER_TRIG_I2S  = cmu->RD_TIMER_TRIG_I2S;
}

SRAM_TEXT_LOC
void restore_sys_cmu_regs(const struct SAVED_CMU_REGS_T *sav)
{
    cmu->HRESET_SET         = ~sav->HRESET_CLR;
    cmu->PRESET_SET         = ~sav->PRESET_CLR;
    cmu->ORESET_SET         = ~sav->ORESET_CLR;
    cmu->QRESET_SET         = ~sav->QRESET_CLR;
    cmu->XRESET_SET         = ~sav->XRESET_CLR;
    cmu->HCLK_DISABLE       = ~sav->HCLK_ENABLE;
    cmu->PCLK_DISABLE       = ~sav->PCLK_ENABLE;
    cmu->OCLK_DISABLE       = ~sav->OCLK_ENABLE;
    cmu->QCLK_DISABLE       = ~sav->QCLK_ENABLE;
    cmu->XCLK_DISABLE       = ~sav->XCLK_ENABLE;

    cmu->HCLK_ENABLE        = sav->HCLK_ENABLE;
    cmu->PCLK_ENABLE        = sav->PCLK_ENABLE;
    cmu->OCLK_ENABLE        = sav->OCLK_ENABLE;
    cmu->HCLK_MODE          = sav->HCLK_MODE;
    cmu->PCLK_MODE          = sav->PCLK_MODE;
    cmu->OCLK_MODE          = sav->OCLK_MODE;
    cmu->REG_RAM_CFG0       = sav->REG_RAM_CFG0;
    cmu->HRESET_CLR         = sav->HRESET_CLR;
    cmu->PRESET_CLR         = sav->PRESET_CLR;
    cmu->ORESET_CLR         = sav->ORESET_CLR;
    cmu->MCU_TIMER          = sav->MCU_TIMER;
    cmu->SLEEP              = sav->SLEEP;
    cmu->PERIPH_CLK         = sav->PERIPH_CLK;
    cmu->SYS_CLK_ENABLE     = sav->SYS_CLK_ENABLE;
    cmu->ADMA_CH15_REQ      = sav->ADMA_CH15_REQ;
    cmu->REG_RAM_CFG1       = sav->REG_RAM_CFG1;
    cmu->UART_CLK           = sav->UART_CLK;
    cmu->I2C_CLK            = sav->I2C_CLK;
    cmu->SYS2SENS_MASK0     = sav->SYS2SENS_MASK0;
    cmu->SYS2SENS_MASK1     = sav->SYS2SENS_MASK1;
    cmu->WAKEUP_CLK_CFG     = sav->WAKEUP_CLK_CFG;
    cmu->SYS_DIV            = sav->SYS_DIV;
    cmu->SYS2BTH_INTMASK0   = sav->SYS2BTH_INTMASK0;
    cmu->SYS2BTH_INTMASK1   = sav->SYS2BTH_INTMASK1;
    cmu->ADMA_CH0_4_REQ     = sav->ADMA_CH0_4_REQ;
    cmu->ADMA_CH5_9_REQ     = sav->ADMA_CH5_9_REQ;
    cmu->ADMA_CH10_14_REQ   = sav->ADMA_CH10_14_REQ;
    cmu->GDMA_CH0_4_REQ     = sav->GDMA_CH0_4_REQ;
    cmu->GDMA_CH5_9_REQ     = sav->GDMA_CH5_9_REQ;
    cmu->GDMA_CH10_14_REQ   = sav->GDMA_CH10_14_REQ;
    cmu->GDMA_CH15_REQ      = sav->GDMA_CH15_REQ;
    cmu->MISC               = sav->MISC;
    cmu->SIMU_RES           = sav->SIMU_RES;
    cmu->MISC_0F8           = sav->MISC_0F8;
    cmu->SYS2OTH_MASK2      = sav->SYS2OTH_MASK2;
    cmu->DSI_CLK_ENABLE     = sav->DSI_CLK_ENABLE;
    cmu->I2S0_CLK           = sav->I2S0_CLK;
    cmu->I2S1_CLK           = sav->I2S1_CLK;
    cmu->REG_RAM_CFG2       = sav->REG_RAM_CFG2;
    cmu->M55_TCM_CFG        = sav->M55_TCM_CFG;
    cmu->PWM01_CLK          = sav->PWM01_CLK;
    cmu->PWM23_CLK          = sav->PWM23_CLK;
    cmu->M55_CODEC_OBS0     = sav->M55_CODEC_OBS0;
    cmu->M55_GPIO_OBS0      = sav->M55_GPIO_OBS0;
    cmu->LCDC_CLK           = sav->LCDC_CLK;
    cmu->SYS2BTC_INTMASK0   = sav->SYS2BTC_INTMASK0;
    cmu->SYS2BTC_INTMASK1   = sav->SYS2BTC_INTMASK1;
    cmu->AXI_MANUAL_CLK     = sav->AXI_MANUAL_CLK;
    cmu->DSP_WAKEUP_MASK0   = sav->DSP_WAKEUP_MASK0;
    cmu->DSP_WAKEUP_MASK1   = sav->DSP_WAKEUP_MASK1;
    cmu->DSP_WAKEUP_MASK2   = sav->DSP_WAKEUP_MASK2;
    cmu->QCLK_ENABLE        = sav->QCLK_ENABLE;
    cmu->QCLK_MODE          = sav->QCLK_MODE;
    cmu->QRESET_CLR         = sav->QRESET_CLR;
    cmu->XCLK_ENABLE        = sav->XCLK_ENABLE;
    cmu->XCLK_MODE          = sav->XCLK_MODE;
    cmu->XRESET_CLR         = sav->XRESET_CLR;
    cmu->RF2P_CFG           = sav->RF2P_CFG;
    cmu->CPU_IDLE_MSK       = sav->CPU_IDLE_MSK;
    cmu->M55_COREPACCEPT    = sav->M55_COREPACCEPT;
    cmu->CFG_LP_ENABLE      = sav->CFG_LP_ENABLE;
    cmu->CFG_FORCE_LP       = sav->CFG_FORCE_LP;
    cmu->CFG_BYPASS_CLKEN   = sav->CFG_BYPASS_CLKEN;
    cmu->CFG_LP_NUMBER      = sav->CFG_LP_NUMBER;
    cmu->FORCE_ACK          = sav->FORCE_ACK;
    cmu->RD_TIMER_TRIG      = sav->RD_TIMER_TRIG;
    cmu->RD_TIMER_TRIG_I2S  = sav->RD_TIMER_TRIG_I2S;
}
#endif // CORE_SLEEP_POWER_DOWN

#endif // !(CHIP_SUBSYS_SENS || (CHIP_SUBSYS_BTH && !BTH_AS_MAIN_MCU))

#ifndef CHIP_SUBSYS_SENS

SYS_FLASH_CFG_LOC
int hal_cmu_sys_flash_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t lock;
    uint32_t enable;
    uint32_t disable;
    bool clk_en;
    int div = -1;
    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }
    if (freq <= HAL_CMU_FREQ_24M) {
        enable = 0;
        disable = CMU_SEL_OSCX2_FLS0_DISABLE | CMU_SEL_OSCX4_FLS0_DISABLE |
            CMU_SEL_PLL_FLS0_DISABLE | CMU_RSTN_DIV_FLS0_DISABLE | CMU_BYPASS_DIV_FLS0_DISABLE;
    } else if (freq <= HAL_CMU_FREQ_48M) {
        enable = CMU_SEL_OSCX2_FLS0_ENABLE;
        disable = CMU_SEL_OSCX4_FLS0_DISABLE | CMU_SEL_PLL_FLS0_DISABLE |
            CMU_RSTN_DIV_FLS0_DISABLE | CMU_BYPASS_DIV_FLS0_DISABLE;
    } else if (freq <= HAL_CMU_FREQ_78M) {
#if defined(OSC_26M_X4_AUD2BB) && !defined(FREQ_78M_USE_PLL)
        enable = CMU_SEL_OSCX4_FLS0_ENABLE;
        disable = CMU_SEL_PLL_FLS0_DISABLE | CMU_RSTN_DIV_FLS0_DISABLE | CMU_BYPASS_DIV_FLS0_DISABLE;
#else
        enable = CMU_SEL_PLL_FLS0_ENABLE | CMU_RSTN_DIV_FLS0_ENABLE;
        disable = CMU_BYPASS_DIV_FLS0_DISABLE;
#ifdef AUD_PLL_DOUBLE
        div = 3;
#else
        div = 1;
#endif
#endif
    } else if (freq <= HAL_CMU_FREQ_96M) {
#ifdef OSC_26M_X4_AUD2BB
        enable = CMU_SEL_OSCX4_FLS0_ENABLE;
        disable = CMU_SEL_PLL_FLS0_DISABLE | CMU_RSTN_DIV_FLS0_DISABLE | CMU_BYPASS_DIV_FLS0_DISABLE;
#else
        enable = CMU_SEL_PLL_FLS0_ENABLE | CMU_RSTN_DIV_FLS0_ENABLE;
        disable = CMU_BYPASS_DIV_FLS0_DISABLE;
#ifdef AUD_PLL_DOUBLE
        div = 2;
#else
        div = 0;
#endif
#endif
#ifdef AUD_PLL_DOUBLE
    } else if (freq <= HAL_CMU_FREQ_192M) {
        enable = CMU_SEL_PLL_FLS0_ENABLE | CMU_RSTN_DIV_FLS0_ENABLE;
        disable = CMU_BYPASS_DIV_FLS0_DISABLE;
        if (freq <= HAL_CMU_FREQ_144M) {
            div = 1;
        } else {
            div = 0;
        }
#endif
    } else {
        enable = CMU_SEL_PLL_FLS0_ENABLE | CMU_BYPASS_DIV_FLS0_ENABLE;
        disable = CMU_RSTN_DIV_FLS0_DISABLE;
    }
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_FLASH);
    if (clk_en) {
        cmu->OCLK_DISABLE = SYS_OCLK_FLASH;
        cmu->HCLK_DISABLE = SYS_HCLK_FLASH;
        cmu->HCLK_DISABLE;
        __DSB();
    }
    if (div >= 0) {
        lock = int_lock();
        cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_FLS0, div);
        int_unlock(lock);
    }
    cmu->SYS_CLK_ENABLE = enable;
    cmu->SYS_CLK_DISABLE = disable;
    if (clk_en) {
        cmu->HCLK_ENABLE = SYS_HCLK_FLASH;
        cmu->OCLK_ENABLE = SYS_OCLK_FLASH;
    }
    cmu->SYS_CLK_ENABLE;
    __DSB();
    return 0;
}

BTH_FLASH_CFG_LOC
int hal_cmu_bth_flash_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t lock;
    uint32_t enable;
    uint32_t disable;
    bool clk_en;
    int div = -1;
    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }
    if (freq <= HAL_CMU_FREQ_24M) {
        enable = 0;
        disable = BTH_CMU_SEL_OSCX2_FLS0_DISABLE | BTH_CMU_SEL_OSCX4_FLS0_DISABLE |
            BTH_CMU_SEL_PLL_FLS0_DISABLE | BTH_CMU_RSTN_DIV_FLS0_DISABLE | BTH_CMU_BYPASS_DIV_FLS0_DISABLE;
    } else if (freq <= HAL_CMU_FREQ_48M) {
        enable = BTH_CMU_SEL_OSCX2_FLS0_ENABLE;
        disable = BTH_CMU_SEL_OSCX4_FLS0_DISABLE | BTH_CMU_SEL_PLL_FLS0_DISABLE |
            BTH_CMU_RSTN_DIV_FLS0_DISABLE | BTH_CMU_BYPASS_DIV_FLS0_DISABLE;
    } else if (freq <= HAL_CMU_FREQ_78M) {
#ifdef OSC_26M_X4_AUD2BB
        enable = BTH_CMU_SEL_OSCX4_FLS0_ENABLE;
        disable = BTH_CMU_SEL_PLL_FLS0_DISABLE | BTH_CMU_RSTN_DIV_FLS0_DISABLE | BTH_CMU_BYPASS_DIV_FLS0_DISABLE;
#else
        enable = BTH_CMU_SEL_PLL_FLS0_ENABLE | BTH_CMU_RSTN_DIV_FLS0_ENABLE;
        disable = BTH_CMU_BYPASS_DIV_FLS0_DISABLE;
#ifdef AUD_PLL_DOUBLE
        div = 3;
#else
        div = 1;
#endif
#endif
    } else if (freq <= HAL_CMU_FREQ_96M) {
#ifdef OSC_26M_X4_AUD2BB
        enable = BTH_CMU_SEL_OSCX4_FLS0_ENABLE;
        disable = BTH_CMU_SEL_PLL_FLS0_DISABLE | BTH_CMU_RSTN_DIV_FLS0_DISABLE | BTH_CMU_BYPASS_DIV_FLS0_DISABLE;
#else
        enable = BTH_CMU_SEL_PLL_FLS0_ENABLE | BTH_CMU_RSTN_DIV_FLS0_ENABLE;
        disable = BTH_CMU_BYPASS_DIV_FLS0_DISABLE;
#ifdef AUD_PLL_DOUBLE
        div = 2;
#else
        div = 0;
#endif
#endif
#ifdef AUD_PLL_DOUBLE
    } else if (freq <= HAL_CMU_FREQ_192M) {
        enable = BTH_CMU_SEL_PLL_FLS0_ENABLE | BTH_CMU_RSTN_DIV_FLS0_ENABLE;
        disable = BTH_CMU_BYPASS_DIV_FLS0_DISABLE;
        if (freq <= HAL_CMU_FREQ_144M) {
            div = 1;
        } else {
            div = 0;
        }
#endif
    } else {
        enable = BTH_CMU_SEL_PLL_FLS0_ENABLE | BTH_CMU_BYPASS_DIV_FLS0_ENABLE;
        disable = BTH_CMU_RSTN_DIV_FLS0_DISABLE;
    }
    clk_en = !!(bthcmu->OCLK_DISABLE & BTH_OCLK_FLASH1);
    if (clk_en) {
        bthcmu->OCLK_DISABLE = BTH_OCLK_FLASH1;
        bthcmu->HCLK_DISABLE = BTH_HCLK_FLASH1;
        bthcmu->HCLK_DISABLE;
        __DSB();
    }
    if (div >= 0) {
        lock = int_lock();
        bthcmu->SYS_DIV = SET_BITFIELD(bthcmu->SYS_DIV, BTH_CMU_CFG_DIV_FLS0, div);
        int_unlock(lock);
    }
    bthcmu->SYS_CLK_ENABLE = enable;
    bthcmu->SYS_CLK_DISABLE = disable;
    if (clk_en) {
        bthcmu->HCLK_ENABLE = BTH_HCLK_FLASH1;
        bthcmu->OCLK_ENABLE = BTH_OCLK_FLASH1;
    }
    bthcmu->SYS_CLK_ENABLE;
    __DSB();
    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_sys_flash_select_pll(enum HAL_CMU_PLL_T pll)
{
    if (pll == HAL_CMU_PLL_BB) {
        cmu->SYS_CLK_ENABLE = CMU_SEL_PLL_FLS0_FAST_ENABLE;
    } else {
        cmu->SYS_CLK_DISABLE = CMU_SEL_PLL_FLS0_FAST_DISABLE;
    }
    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_bth_flash_select_pll(enum HAL_CMU_PLL_T pll)
{
#ifdef FLASH1_CTRL_BASE
#if 0
    if (pll == HAL_CMU_PLL_BB) {
        bthcmu->SYS_CLK_ENABLE = BTH_CMU_SEL_PLL_FLS0_FAST_ENABLE;
    } else {
        bthcmu->SYS_CLK_DISABLE = BTH_CMU_SEL_PLL_FLS0_FAST_DISABLE;
    }
#endif
#endif
    return 0;
}

#if defined(CHIP_SUBSYS_BTH)
#if defined(ALT_BOOT_FLASH)
int hal_cmu_flash_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_sys_flash_set_freq")));
int hal_cmu_flash1_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_bth_flash_set_freq")));
int hal_cmu_flash_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_sys_flash_select_pll")));
int hal_cmu_flash1_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_bth_flash_select_pll")));
#else
int hal_cmu_flash_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_bth_flash_set_freq")));
int hal_cmu_flash1_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_sys_flash_set_freq")));
int hal_cmu_flash_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_bth_flash_select_pll")));
int hal_cmu_flash1_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_sys_flash_select_pll")));
#endif
#else
#if defined(ALT_BOOT_FLASH)
int hal_cmu_flash_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_bth_flash_set_freq")));
int hal_cmu_flash1_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_sys_flash_set_freq")));
int hal_cmu_flash_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_bth_flash_select_pll")));
int hal_cmu_flash1_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_sys_flash_select_pll")));
#else
int hal_cmu_flash_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_sys_flash_set_freq")));
int hal_cmu_flash1_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_bth_flash_set_freq")));
int hal_cmu_flash_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_sys_flash_select_pll")));
int hal_cmu_flash1_select_pll(enum HAL_CMU_PLL_T pll) __attribute__((alias("hal_cmu_bth_flash_select_pll")));
#endif
#endif

#endif // CHIP_SUBSYS_SENS

uint32_t hal_cmu_get_aon_chip_id(void)
{
    return aoncmu->CHIP_ID;
}

uint32_t BOOT_TEXT_FLASH_LOC hal_cmu_get_aon_revision_id(void)
{
    return GET_BITFIELD(aoncmu->CHIP_ID, AON_CMU_REVISION_ID);
}

int hal_cmu_clock_out_enable(enum HAL_CMU_CLOCK_OUT_ID_T id)
{
    uint32_t lock;
    uint32_t sel;
    uint32_t cfg;

    enum CMU_CLK_OUT_SEL_T {
        CMU_CLK_OUT_SEL_NONE    = 0,
        CMU_CLK_OUT_SEL_CODEC   = 1,
        CMU_CLK_OUT_SEL_BT      = 2,
        CMU_CLK_OUT_SEL_SYS     = 3,
        CMU_CLK_OUT_SEL_SENS    = 4,
        CMU_CLK_OUT_SEL_BTH     = 5,
        CMU_CLK_OUT_SEL_AON     = 6,

        CMU_CLK_OUT_SEL_QTY
    };

    sel = CMU_CLK_OUT_SEL_QTY;
    cfg = 0;

    if (HAL_CMU_CLOCK_OUT_AON_32K <= id && id <= HAL_CMU_CLOCK_OUT_AON_SYS) {
        sel = CMU_CLK_OUT_SEL_AON;
        cfg = id - HAL_CMU_CLOCK_OUT_AON_32K;
    } else if (HAL_CMU_CLOCK_OUT_SYS_32K <= id && id <= HAL_CMU_CLOCK_OUT_SYS_I2S1) {
        sel = CMU_CLK_OUT_SEL_SYS;
        lock = int_lock();
        cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_SYS_32K);
        int_unlock(lock);
#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))
    } else if (HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA <= id && id <= HAL_CMU_CLOCK_OUT_CODEC_HCLK) {
        sel = CMU_CLK_OUT_SEL_CODEC;
        hal_codec_select_clock_out(id - HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA);
#endif
    } else if (HAL_CMU_CLOCK_OUT_BT_NONE <= id && id <= HAL_CMU_CLOCK_OUT_BT_AM) {
        sel = CMU_CLK_OUT_SEL_BT;
        btcmu->CLK_OUT = SET_BITFIELD(btcmu->CLK_OUT, BT_CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_BT_NONE);
    } else if (HAL_CMU_CLOCK_OUT_SENS_32K <= id && id <= HAL_CMU_CLOCK_OUT_SENS_I2S) {
        sel = CMU_CLK_OUT_SEL_SENS;
        senscmu->CLK_OUT = SET_BITFIELD(senscmu->CLK_OUT, SENS_CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_SENS_32K);
    } else if (HAL_CMU_CLOCK_OUT_BTH_32K <= id && id <= HAL_CMU_CLOCK_OUT_BTH_ISPI) {
        sel = CMU_CLK_OUT_SEL_BTH;
        bthcmu->PERIPH_CLK = SET_BITFIELD(bthcmu->PERIPH_CLK, BTH_CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_BTH_32K);
    }

    if (sel < CMU_CLK_OUT_SEL_QTY) {
        lock = iomux_lock();
        aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~(AON_CMU_SEL_CLK_OUT_MASK | AON_CMU_CFG_CLK_OUT_MASK)) |
            AON_CMU_SEL_CLK_OUT(sel) | AON_CMU_CFG_CLK_OUT(cfg) | AON_CMU_EN_CLK_OUT;
        iomux_unlock(lock);

        return 0;
    }

    return 1;
}

void hal_cmu_clock_out_disable(void)
{
    uint32_t lock;

    lock = iomux_lock();
    aoncmu->CLK_OUT &= ~AON_CMU_EN_CLK_OUT;
    iomux_unlock(lock);
}

void hal_cmu_jtag_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_JTAG_ENABLE;
}

void hal_cmu_jtag_clock_disable(void)
{
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_JTAG_DISABLE;
}

void hal_cmu_select_rc_clock(enum HAL_CMU_CLK_SEL_OSC_USER_T user)
{
    uint32_t val_sel = 0;
    uint32_t val_pu = 0;
    uint32_t cur_sel;
    uint32_t lock;
    const uint32_t all_sel =
        AON_CMU_SEL_CLK_OSC_MCU_ENABLE |
        AON_CMU_SEL_CLK_OSC_SEN_ENABLE |
        AON_CMU_SEL_CLK_OSC_CDC_ENABLE |
        AON_CMU_SEL_CLK_OSC_BTH_ENABLE |
        AON_CMU_SEL_CLK_OSC_BTC_ENABLE;

    lock = iomux_lock();

    aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_RC_ENABLE | AON_CMU_PU_OSC_ENABLE;
    hal_sys_timer_delay_us(HAL_CMU_RC_STABLE_TIME);

    if (user & HAL_CMU_CLK_SEL_OSC_USER_SYS) {
        val_sel |= AON_CMU_SEL_CLK_OSC_MCU_DISABLE;
        val_pu |= AON_CMU_OSC_MCU_ON;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_SENS) {
        val_sel |= AON_CMU_SEL_CLK_OSC_SEN_DISABLE;
        val_pu |= AON_CMU_OSC_SENS_ON;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_CODEC) {
        val_sel |= AON_CMU_SEL_CLK_OSC_CDC_DISABLE;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_BTH) {
        val_sel |= AON_CMU_SEL_CLK_OSC_BTH_DISABLE;
        val_pu |= AON_CMU_OSC_BTH_ON;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_BTC) {
        val_sel |= AON_CMU_SEL_CLK_OSC_BTC_DISABLE;
        val_pu |= AON_CMU_OSC_BTC_ON;
    }

    if (val_sel) {
        // Disable AON peripheral OSC
        val_sel |= AON_CMU_SEL_CLK_OSC_PRE_DISABLE;
        val_pu |= /* AON_CMU_OSC_PSC_ON | */ AON_CMU_OSC_OTHR_ON;
    }

    aoncmu->TOP_CLK_DISABLE = val_sel;
    aoncmu->PD_STAB_TIMER &= ~val_pu;
    hal_sys_timer_delay_us(2);
    aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_OSC_DISABLE;
    cur_sel = aoncmu->TOP_CLK_ENABLE;
    if ((cur_sel & all_sel) == all_sel || (cur_sel & all_sel) == 0) {
        // Disable AON-RC only if all domains select RC or OSC
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_RC_DISABLE;
    }

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU))) && defined(RC48M_ENABLE)
    hal_cmu_release_x2_x4();
#endif

    iomux_unlock(lock);
}

void hal_cmu_select_osc_clock(enum HAL_CMU_CLK_SEL_OSC_USER_T user)
{
    uint32_t val_sel = 0;
    uint32_t val_pu = 0;
    uint32_t cur_sel;
    uint32_t lock;
    const uint32_t all_sel =
        AON_CMU_SEL_CLK_OSC_MCU_ENABLE |
        AON_CMU_SEL_CLK_OSC_SEN_ENABLE |
        AON_CMU_SEL_CLK_OSC_CDC_ENABLE |
        AON_CMU_SEL_CLK_OSC_BTH_ENABLE |
        AON_CMU_SEL_CLK_OSC_BTC_ENABLE;

    lock = iomux_lock();

    cur_sel = aoncmu->TOP_CLK_ENABLE;

    aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_RC_ENABLE | AON_CMU_PU_OSC_ENABLE;
    hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU))) && defined(RC48M_ENABLE)
    hal_cmu_acquire_x2_x4();
#endif

    if (user & HAL_CMU_CLK_SEL_OSC_USER_SYS) {
        val_sel |= AON_CMU_SEL_CLK_OSC_MCU_ENABLE;
        val_pu |= AON_CMU_OSC_MCU_ON;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_SENS) {
        val_sel |= AON_CMU_SEL_CLK_OSC_SEN_ENABLE;
        val_pu |= AON_CMU_OSC_SENS_ON;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_CODEC) {
        val_sel |= AON_CMU_SEL_CLK_OSC_CDC_ENABLE;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_BTH) {
        val_sel |= AON_CMU_SEL_CLK_OSC_BTH_ENABLE;
        val_pu |= AON_CMU_OSC_BTH_ON;
    }
    if (user & HAL_CMU_CLK_SEL_OSC_USER_BTC) {
        val_sel |= AON_CMU_SEL_CLK_OSC_BTC_ENABLE;
        val_pu |= AON_CMU_OSC_BTC_ON;
    }

    if (((cur_sel | val_sel) & all_sel) == all_sel) {
        // Enable AON peripheral OSC
        val_sel |= AON_CMU_SEL_CLK_OSC_PRE_ENABLE;
        val_pu |= /* AON_CMU_OSC_PSC_ON | */ AON_CMU_OSC_OTHR_ON;
    }

    aoncmu->TOP_CLK_ENABLE = val_sel;
    aoncmu->PD_STAB_TIMER |= val_pu;
    hal_sys_timer_delay_us(2);
    aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_OSC_DISABLE;
    cur_sel = aoncmu->TOP_CLK_ENABLE;
    if ((cur_sel & all_sel) == all_sel || (cur_sel & all_sel) == 0) {
        // Disable AON-RC only if all domains select RC or OSC
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_RC_DISABLE;
    }

    iomux_unlock(lock);
}

uint32_t hal_cmu_pu_osc_enable(void)
{
    uint32_t old_val;

    old_val = aoncmu->TOP_CLK_ENABLE;
    if (old_val & AON_CMU_PU_OSC_ENABLE) {
        return old_val;
    }
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)
    if (old_val & AON_CMU_SEL_CLK_OSC_BTH_ENABLE) {
        return old_val;
    }
#else
    if (old_val & AON_CMU_SEL_CLK_OSC_MCU_ENABLE) {
        return old_val;
    }
#endif

    aoncmu->TOP_CLK_ENABLE = AON_CMU_PU_OSC_ENABLE;
    hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
    return old_val;
}

void hal_cmu_pu_osc_disable(uint32_t val)
{
    if (val & AON_CMU_PU_OSC_ENABLE) {
        return;
    }
    aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_OSC_DISABLE;
}

uint32_t hal_cmu_get_osc_ready_cycle_cnt(void)
{
    uint32_t cnt;

    // Always return OSC stable time because BTC only uses OSC (not RC)
    cnt = LPU_TIMER_US(HAL_CMU_OSC_STABLE_TIME);

    return cnt;
}

uint32_t hal_cmu_get_osc_switch_overhead(void)
{
    return 0;
}

void hal_cmu_bt_clock_enable(void)
{
    aoncmu->AON_CLK &= ~AON_CMU_BT_OSC_AUTOSW;
    aoncmu->MOD_CLK_ENABLE = AON_CMU_MANUAL_OCLK_ENABLE(AON_OCLK_BTAON);
    aocmu_reg_update_wait();
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_32K_BT_ENABLE;
    aocmu_reg_update_wait();
}

void hal_cmu_bt_clock_disable(void)
{
    aoncmu->MOD_CLK_DISABLE = AON_CMU_MANUAL_OCLK_DISABLE(AON_OCLK_BTAON);
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_32K_BT_DISABLE;
}

void hal_cmu_bt_reset_set(void)
{
    aoncmu->RESET_SET = AON_CMU_ORESETN_SET(AON_ORST_BTAON);
    aoncmu->GBL_RESET_SET = AON_CMU_SOFT_RSTN_BT_SET | AON_CMU_SOFT_RSTN_BTCPU_SET;
}

void hal_cmu_bt_reset_clear(void)
{
    hal_cmu_bt_sys_reset_clear();
    hal_cmu_bt_cpu_reset_clear();
}

void hal_cmu_bt_sys_reset_clear(void)
{
    aoncmu->RESET_CLR = AON_CMU_ORESETN_CLR(AON_ORST_BTAON);
    aocmu_reg_update_wait();
    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_BT_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_bt_cpu_reset_clear(void)
{
    aoncmu->GBL_RESET_CLR = AON_CMU_SOFT_RSTN_BTCPU_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_btc_start_cpu(uint32_t sp, uint32_t entry)
{
    volatile struct CORE_STARTUP_CFG_T *core_cfg;
    uint32_t cfg_addr;

    // Use (sp - 128) as the default vector. The Address must aligned to 128-byte boundary.
    cfg_addr = (sp - (1 << 7)) & ~((1 << 7) - 1);
    aoncmu->BTC_VTOR = (aoncmu->BTC_VTOR & ~AON_CMU_VTOR_CORE_BTC_MASK) | (cfg_addr & AON_CMU_VTOR_CORE_BTC_MASK);
    core_cfg = (volatile struct CORE_STARTUP_CFG_T *)cfg_addr;
    core_cfg->stack = sp;
    core_cfg->reset_hdlr = entry;
    // Flush previous write operation
    __DSB();
    hal_cmu_bt_cpu_reset_clear();
}

void hal_cmu_bt_module_init(void)
{
#ifdef SIMU
    btcmu->CLK_MODE |= (1 << 8);
#endif
#if (BTC_ROM_RAM_SIZE > 0)
    btcmu->CLK_MODE |= (1 << 9);
#endif
}

void hal_cmu_bt_sys_set_freq(enum HAL_CMU_FREQ_T freq)
{
}

void hal_cmu_bt_sys_clock_force_on(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_BT_CLK_SYS_ENABLE;
    aoncmu->CEN_MSK |= AON_CMU_DSLP_FORCE_ON_BT;
    int_unlock(lock);
    aocmu_reg_update_wait();
}

void hal_cmu_bt_sys_clock_auto(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_BT_CLK_SYS_DISABLE;
    aoncmu->CEN_MSK &= ~AON_CMU_DSLP_FORCE_ON_BT;
    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC
void hal_cmu_btc_ram_cfg(void)
{
#if (BTC_ROM_RAM_SIZE > 0)
    uint32_t map;
    uint32_t idx, cnt;
    uint32_t start, end;

    start = BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE;
    end = start + BTC_ROM_RAM_SIZE;

    idx = start / SHR_RAM_BLK_SIZE + 1;
    cnt = (end - start) / SHR_RAM_BLK_SIZE;
    map = ((1 << cnt) - 1) << idx;

    hal_cmu_ram_cfg_sel_update(map, HAL_CMU_RAM_CFG_SEL_BTC);
#endif
}

void hal_cmu_btc_dpd_ram_enable(uint32_t *old_ram_sel, uint32_t *old_sleep_cfg)
{
    uint32_t map;

    *old_ram_sel = aoncmu->RAM_CFG_SEL;

    map = (1 << 10);
    hal_cmu_ram_cfg_sel_update(map, HAL_CMU_RAM_CFG_SEL_BTC);
    btcmu->CLK_MODE |= (1 << 9);

    *old_sleep_cfg = hal_psc_btc_dpd_ram_enable(map);
}

void hal_cmu_btc_dpd_ram_disable(uint32_t old_ram_sel, uint32_t old_sleep_cfg)
{
    aoncmu->RAM_CFG_SEL = old_ram_sel;
    hal_psc_btc_dpd_ram_disable(old_sleep_cfg);

    aocmu_reg_update_wait();
    aocmu_reg_update_wait();
}

void BOOT_TEXT_FLASH_LOC hal_cmu_ram_cfg_sel_update(uint32_t map, enum HAL_CMU_RAM_CFG_SEL_T sel)
{
    uint32_t val;
    int i;

    val = aoncmu->RAM_CFG_SEL;
    for (i = 0; i < 11; i++) {
        if (map & (1 << i)) {
            val = (val & ~(AON_CMU_SRAM0_CFG_SEL_MASK << (2 * i))) |
                (AON_CMU_SRAM0_CFG_SEL(sel) << (2 * i));
        }
    }
    if (map & (1 << 11)) {
        if (sel == HAL_CMU_RAM_CFG_SEL_BTC) {
            val |= AON_CMU_SENS_SRAM4_CFG_SEL;
        } else if (sel == HAL_CMU_RAM_CFG_SEL_SENS) {
            val &= ~AON_CMU_SENS_SRAM4_CFG_SEL;
        }
    }
    if (map & (1 << 12)) {
        if (sel == HAL_CMU_RAM_CFG_SEL_BTC) {
            val |= AON_CMU_SENS_SRAM5_CFG_SEL;
        } else if (sel == HAL_CMU_RAM_CFG_SEL_SENS) {
            val &= ~AON_CMU_SENS_SRAM5_CFG_SEL;
        }
    }
    aoncmu->RAM_CFG_SEL = val;
    aocmu_reg_update_wait();
    aocmu_reg_update_wait();
}

void hal_cmu_sens_force_sleep(void)
{
    uint32_t mask;

    while ((senscmu->WAKEUP_CLK_CFG & (SENS_CMU_LPU_AUTO_SWITCH26 | SENS_CMU_LPU_STATUS_26M)) ==
            SENS_CMU_LPU_AUTO_SWITCH26) {}

    // Disable RAM wakeup early
    senscmu->SENS_TIMER &= ~SENS_CMU_RAM_RETN_UP_EARLY;
    // MCU/ROM/RAM auto clock gating (which depends on RAM gating signal)
    senscmu->HCLK_MODE &= ~(SENS_HCLK_MCU | SENS_HCLK_RAM0 | SENS_HCLK_RAM1 | SENS_HCLK_RAM2 | SENS_HCLK_RAM3 |
        SENS_HCLK_RAM4 | SENS_HCLK_RAM5);
    senscmu->WAKEUP_CLK_CFG = SENS_CMU_TIMER_WT26(0) | SENS_CMU_TIMER_WTPLL(0) | SENS_CMU_LPU_AUTO_SWITCH26;

#ifdef CHIP_SUBSYS_BTH
    senscmu->SENS2BTH_IRQ_SET = SENS_CMU_BTH2SENS_DATA_MSK_SET;
    mask = (1 << 24);
#else
    senscmu->SENS2SYS_IRQ_SET = SENS_CMU_SYS2SENS_DATA_MSK_SET;
    mask = (1 << 26);
#endif
    senscmu->WAKEUP_MASK0 = mask;
    senscmu->WAKEUP_MASK1 = 0;
    senscmu->SLEEP = (senscmu->SLEEP & ~SENS_CMU_MANUAL_RAM_RETN) |
        SENS_CMU_DEEPSLEEP_EN | SENS_CMU_DEEPSLEEP_ROMRAM_EN | SENS_CMU_DEEPSLEEP_START;

#ifdef SENS_FORCE_SLEEP_DISABLE_CLOCK
    while ((senscmu->SLEEP & SENS_CMU_DEEPSLEEP_MODE) == 0) {}
    hal_cmu_sens_clock_disable();
#endif
}

void hal_cmu_sens_force_wakeup(void)
{
#ifdef SENS_FORCE_SLEEP_DISABLE_CLOCK
    hal_cmu_sens_clock_enable();
#endif

#ifdef CHIP_SUBSYS_BTH
    bthcmu->MCU2SENS_IRQ_SET = BTH_CMU_MCU2SENS_DATA_IND_SET;
#else
    cmu->SYS2SENS_IRQ_SET = CMU_SYS2SES_DATA_IND_SET;
#endif

    while ((senscmu->WAKEUP_CLK_CFG & (SENS_CMU_LPU_AUTO_SWITCH26 | SENS_CMU_LPU_STATUS_26M)) ==
            SENS_CMU_LPU_AUTO_SWITCH26) {}

#ifdef CHIP_SUBSYS_BTH
    bthcmu->MCU2SENS_IRQ_CLR = BTH_CMU_MCU2SENS_DATA_IND_CLR;
#else
    cmu->SYS2SENS_IRQ_CLR = CMU_SYS2SES_DATA_IND_CLR;
#endif
}

uint32_t hal_cmu_get_cpu_reg(enum HAL_CMU_CPU_IDX_T core, enum HAL_CMU_CPU_REG_IDX_T reg)
{
    if (core == HAL_CMU_CPU_SYS_CORE0) {
        cmu->MCU_TIMER = SET_BITFIELD(cmu->MCU_TIMER, CMU_DEBUG_REG_SEL, reg);
        return cmu->SIMU_RES;
    } else if (core == HAL_CMU_CPU_BTH_CORE0) {
        bthcmu->MCU_TIMER = SET_BITFIELD(bthcmu->MCU_TIMER, BTH_CMU_DEBUG_REG_SEL, reg);
        return bthcmu->SIMU_RES;
    } else if (core == HAL_CMU_CPU_SENS_CORE0) {
        senscmu->SENS_TIMER = SET_BITFIELD(senscmu->SENS_TIMER, SENS_CMU_DEBUG_REG_SEL, reg);
        return senscmu->SIMU_RES;
    }
    return 0;
}

#if defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU))
uint32_t BOOT_TEXT_FLASH_LOC hal_cmu_read_subsys_metal_id(void)
{
    return (aoncmu->SW_INFO & 0xFF);
}
uint32_t read_hw_metal_id(void) __attribute__((alias("hal_cmu_read_subsys_metal_id")));

#else
void BOOT_TEXT_FLASH_LOC hal_cmu_save_subsys_metal_id(uint32_t id)
{
    aoncmu->SW_INFO = (aoncmu->SW_INFO & ~0xFF) | (id & 0xFF);
}
void save_hw_metal_id(uint32_t id) __attribute__((alias("hal_cmu_save_subsys_metal_id")));

#endif

int hal_cmu_get_aonpmu_efuse(uint32_t page, unsigned short *efuse)
{
    int ret = 0;
    uint32_t value = 0;
    unsigned short tmp[2];

    // Enable CLK_EN
    aonpmu->EFUSE_CTRL = AON_PMU_REG_EFUSE_CLK_EN;

    // Enable TURN_ON
    aonpmu->EFUSE_CTRL |= AON_PMU_REG_EFUSE_TURN_ON;

    // Write Address
#ifdef PMU_EFUSE_NO_REDUNDANCY
    aonpmu->EFUSE_CTRL |= SET_BITFIELD(aonpmu->EFUSE_CTRL, AON_PMU_REG_EFUSE_ADDRESS, (page / 2));
#else
    aonpmu->EFUSE_CTRL |= SET_BITFIELD(aonpmu->EFUSE_CTRL, AON_PMU_REG_EFUSE_ADDRESS, (page));
#endif
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Set Strobe Trigger = 1
    aonpmu->EFUSE_CTRL |= AON_PMU_REG_EFUSE_STROBE_TRIGGER;

    // set Strobe Trigger = 0
    aonpmu->EFUSE_CTRL &= ~AON_PMU_REG_EFUSE_STROBE_TRIGGER;
    hal_sys_timer_delay(US_TO_TICKS(300));

    // Read Efuse
    value = aonpmu->EFUSE_DATA;
    tmp[0] = (unsigned short)value;
    tmp[1] = (unsigned short)(value >> 16);

#ifdef PMU_EFUSE_NO_REDUNDANCY
    *efuse = tmp[page % 2];
#else
    *efuse = (tmp[0] | tmp[1]); //redundancy
#endif

    // Disable TURN_ON
    aonpmu->EFUSE_CTRL &= ~(AON_PMU_REG_EFUSE_TURN_ON | AON_PMU_REG_EFUSE_ADDRESS_MASK);;
    hal_sys_timer_delay(US_TO_TICKS(100));

    // Disable CLK_EN
    aonpmu->EFUSE_CTRL &= ~AON_PMU_REG_EFUSE_CLK_EN;
    hal_sys_timer_delay(US_TO_TICKS(100));

    return ret;
}

void hal_cmu_aonpmu_sleep_en(unsigned char sleep_en)
{
    if (sleep_en) {
        aonpmu->SLEEP_CFG |= AON_PMU_SLEEP_ALLOW;
    } else {
        aonpmu->SLEEP_CFG &= ~AON_PMU_SLEEP_ALLOW;
    }
}

#define PVT_CNT_UPDATE_TIMEOUT                                  (US_TO_FAST_TICKS(10))

void hal_cmu_sys_pvt_enable(uint32_t time_us)
{
    // test_time = time_us + 1
    cmu->PVT_CFG = SET_BITFIELD(cmu->PVT_CFG, CMU_PVT_REG_TIMESET, time_us);
    cmu->PVT_CFG |= CMU_PVT_RO_EN | CMU_PVT_COUNT_START_EN | CMU_PVT_OSC_CLK_EN | CMU_PVT_RSTN;
}

void hal_cmu_bth_pvt_enable(uint32_t time_us)
{
    // test_time = time_us + 1
    bthcmu->PVT_CFG = SET_BITFIELD(bthcmu->PVT_CFG, BTH_CMU_PVT_REG_TIMESET, time_us);
    bthcmu->PVT_CFG |= BTH_CMU_PVT_RO_EN | BTH_CMU_PVT_COUNT_START_EN | BTH_CMU_PVT_OSC_CLK_EN | BTH_CMU_PVT_RSTN;
}

void hal_cmu_btc_pvt_enable(uint32_t time_us)
{
    // test_time = time_us + 1
    btcmu->PVT_CFG = SET_BITFIELD(btcmu->PVT_CFG, BT_CMU_PVT_REG_TIMESET, time_us);
    btcmu->PVT_CFG |= BT_CMU_PVT_RO_EN | BT_CMU_PVT_COUNT_START_EN | BT_CMU_PVT_OSC_CLK_EN | BT_CMU_PVT_RSTN;
}

void hal_cmu_sens_pvt_enable(uint32_t time_us)
{
    // test_time = time_us + 1
    senscmu->PVT_CFG = SET_BITFIELD(senscmu->PVT_CFG, SENS_CMU_PVT_REG_TIMESET, time_us);
    senscmu->PVT_CFG |= SENS_CMU_PVT_RO_EN | SENS_CMU_PVT_COUNT_START_EN | SENS_CMU_PVT_OSC_CLK_EN | SENS_CMU_PVT_RSTN;
}

void hal_cmu_aon_pvt_enable(uint32_t time_us)
{
    // test_time = time_us + 1
    aoncmu->PVT_CFG = SET_BITFIELD(aoncmu->PVT_CFG, AON_CMU_PVT_REG_TIMESET, time_us);
    aoncmu->PVT_CFG |= AON_CMU_PVT_RO_EN | AON_CMU_PVT_COUNT_START_EN | AON_CMU_PVT_OSC_CLK_EN | AON_CMU_PVT_RSTN;
}

void hal_cmu_sys_pvt_disable(void)
{
    cmu->PVT_CFG &= ~(CMU_PVT_RO_EN | CMU_PVT_RSTN);
}

void hal_cmu_bth_pvt_disable(void)
{
    bthcmu->PVT_CFG &= ~(BTH_CMU_PVT_RO_EN | BTH_CMU_PVT_RSTN);
}

void hal_cmu_btc_pvt_disable(void)
{
    btcmu->PVT_CFG &= ~(BT_CMU_PVT_RO_EN | BT_CMU_PVT_RSTN);
}

void hal_cmu_sens_pvt_disable(void)
{
    senscmu->PVT_CFG &= ~(SENS_CMU_PVT_RO_EN | SENS_CMU_PVT_RSTN);
}

void hal_cmu_aon_pvt_disable(void)
{
    aoncmu->PVT_CFG &= ~(AON_CMU_PVT_RO_EN | AON_CMU_PVT_RSTN);
}

uint32_t hal_cmu_sys_pvt_cnt_get(void)
{
    uint32_t stime;

    stime = hal_fast_sys_timer_get();
    do {
        if ((hal_fast_sys_timer_get() - stime) >= PVT_CNT_UPDATE_TIMEOUT) {
            TRACE(0, "%s timeout", __func__);
        }
    } while(!(!!(cmu->PVT_COUNT & CMU_PVT_COUNT_DONE)) && ((hal_fast_sys_timer_get() - stime) < PVT_CNT_UPDATE_TIMEOUT));

    return GET_BITFIELD(cmu->PVT_COUNT, CMU_PVT_COUNT_VALUE);
}

uint32_t hal_cmu_bth_pvt_cnt_get(void)
{
    uint32_t stime;

    stime = hal_fast_sys_timer_get();
    do {
        if ((hal_fast_sys_timer_get() - stime) >= PVT_CNT_UPDATE_TIMEOUT) {
            TRACE(0, "%s timeout", __func__);
        }
    } while(!(!!(bthcmu->PVT_COUNT & BTH_CMU_PVT_COUNT_DONE)) && ((hal_fast_sys_timer_get() - stime) < PVT_CNT_UPDATE_TIMEOUT));

    return GET_BITFIELD(bthcmu->PVT_COUNT, BTH_CMU_PVT_COUNT_VALUE);
}

uint32_t hal_cmu_btc_pvt_cnt_get(void)
{
    uint32_t stime;

    stime = hal_fast_sys_timer_get();
    do {
        if ((hal_fast_sys_timer_get() - stime) >= PVT_CNT_UPDATE_TIMEOUT) {
            TRACE(0, "%s timeout", __func__);
        }
    } while(!(!!(btcmu->PVT_COUNT & BT_CMU_PVT_COUNT_DONE)) && ((hal_fast_sys_timer_get() - stime) < PVT_CNT_UPDATE_TIMEOUT));

    return GET_BITFIELD(btcmu->PVT_COUNT, BT_CMU_PVT_COUNT_VALUE);
}

uint32_t hal_cmu_sens_pvt_cnt_get(void)
{
    uint32_t stime;

    stime = hal_fast_sys_timer_get();
    do {
        if ((hal_fast_sys_timer_get() - stime) >= PVT_CNT_UPDATE_TIMEOUT) {
            TRACE(0, "%s timeout", __func__);
        }
    } while(!(!!(senscmu->PVT_COUNT & SENS_CMU_PVT_COUNT_DONE)) && ((hal_fast_sys_timer_get() - stime) < PVT_CNT_UPDATE_TIMEOUT));

    return GET_BITFIELD(senscmu->PVT_COUNT, SENS_CMU_PVT_COUNT_VALUE);
}

uint32_t hal_cmu_aon_pvt_cnt_get(void)
{
    uint32_t stime;

    stime = hal_fast_sys_timer_get();
    do {
        if ((hal_fast_sys_timer_get() - stime) >= PVT_CNT_UPDATE_TIMEOUT) {
            TRACE(0, "%s timeout", __func__);
        }
    } while(!(!!(aoncmu->PVT_COUNT & AON_CMU_PVT_COUNT_DONE)) && ((hal_fast_sys_timer_get() - stime) < PVT_CNT_UPDATE_TIMEOUT));

    return GET_BITFIELD(aoncmu->PVT_COUNT, AON_CMU_PVT_COUNT_VALUE);
}

