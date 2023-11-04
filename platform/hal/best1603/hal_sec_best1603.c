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
#if !(defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH))
#include "cmsis.h"
#include "plat_addr_map.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_sec.h"
#include "hal_trace.h"
#include "reg_page_spy.h"

#include CHIP_SPECIFIC_HDR(reg_sec)
#include CHIP_SPECIFIC_HDR(reg_aonsec)

extern uint32_t __nsc_text_start__[];
extern uint32_t __nsc_text_end__[];

//these sec regs can only be accessed in security state
static struct SEC_T *const sec = (struct SEC_T *)SEC_CTRL_BASE;

static struct AONSEC_T *const aon_sec = (struct AONSEC_T *)AON_SEC_CTRL_BASE;

void hal_sec_set_m_i2c_debug(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        aon_sec->NONSEC_I2C |= AON_SEC_CFG_NONSEC_I2C_SLV;
    else
        aon_sec->NONSEC_I2C &= ~AON_SEC_CFG_NONSEC_I2C_SLV;
}

static void hal_sec_set_m_sens2aon(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        aon_sec->NONSEC_I2C |= AON_SEC_CFG_NONSEC_SENS;
    else
        aon_sec->NONSEC_I2C &= ~AON_SEC_CFG_NONSEC_SENS;
}

static void hal_sec_set_m_bth2aon(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        aon_sec->NONSEC_I2C |= AON_SEC_CFG_NONSEC_BTH;
    else
        aon_sec->NONSEC_I2C &= ~AON_SEC_CFG_NONSEC_BTH;
    //TRACE(0, "%s, 0x%x", __func__, aon_sec->NONSEC_I2C);
}

static void hal_sec_init_s_aon()
{
    aon_sec->SEC_BOOT_ACC |= AON_SEC_CFG_NONSEC_APB_PROT;
    aon_sec->NONSEC_BYPASS_PROT |= AON_SEC_CFG_NONSEC_BYPASS_PROT_APB0(0xFF);
    //TRACE(0, "%s, 0x%x", __func__, aon_sec->NONSEC_BYPASS_PROT);
}

static void hal_sec_set_m_bcm(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_024 |= SEC_CFG_NONSEC_BCM;
    else
        sec->REG_024 &= ~SEC_CFG_NONSEC_BCM;
}

static void hal_sec_set_m_usb(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_024 |= SEC_CFG_NONSEC_USB;
    else
        sec->REG_024 &= ~SEC_CFG_NONSEC_USB;
}

static void hal_sec_set_m_emmc(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_024 |= SEC_CFG_NONSEC_EMMC;
    else
        sec->REG_024 &= ~SEC_CFG_NONSEC_EMMC;
}

static void hal_sec_set_m_bth_flash(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_024 |= SEC_CFG_NONSEC_BTH_FLS;
    else
        sec->REG_024 &= ~SEC_CFG_NONSEC_BTH_FLS;
}

static void hal_sec_set_m_bth(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_HNONSEC_BTH_H2X;
    else
        sec->REG_028 &= ~SEC_HNONSEC_BTH_H2X;
}

static void hal_sec_set_m_btc(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_HNONSEC_BTC_H2X;
    else
        sec->REG_028 &= ~SEC_HNONSEC_BTC_H2X;
}

static void hal_sec_set_m_dsp(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_ANONSEC_DSP_M;
    else
        sec->REG_028 &= ~SEC_ANONSEC_DSP_M;
}

static void hal_sec_set_m_display(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_ANONSEC_DISPLAY_M;
    else
        sec->REG_028 &= ~SEC_ANONSEC_DISPLAY_M;
}

static void hal_sec_set_m_dma2d(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_ANONSEC_DMA2D_M;
    else
        sec->REG_028 &= ~SEC_ANONSEC_DMA2D_M;
}

static void hal_sec_set_m_gpu(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_ANONSEC_GPU_M;
    else
        sec->REG_028 &= ~SEC_ANONSEC_GPU_M;
}

static void hal_sec_set_m_syshdma_nosec(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= (SEC_HNONSEC_HDMA0_M | SEC_HNONSEC_HDMA1_M);
    else
        sec->REG_028 &= ~(SEC_HNONSEC_HDMA0_M | SEC_HNONSEC_HDMA1_M);
}

static void hal_sec_set_m_sysxdma_nosec(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS) {
        sec->REG_028 ^= ~SEC_CFG_WR_SEC_DMA_MASK;
        sec->REG_028 ^= ~SEC_CFG_RD_SEC_DMA_MASK;
    } else {
        sec->REG_028 |= SEC_CFG_WR_SEC_DMA_MASK;
        sec->REG_028 |= SEC_CFG_RD_SEC_DMA_MASK;
    }
}

static void hal_sec_set_m_sens(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_HNONSEC_SES_H2X;
    else
        sec->REG_028 &= ~SEC_HNONSEC_SES_H2X;
}

static void hal_sec_set_m_spi_slave(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_028 |= SEC_HNONSEC_SPI_SLV;
    else
        sec->REG_028 &= ~SEC_HNONSEC_SPI_SLV;
}

static void hal_sec_set_s_bth(enum HAL_SEC_TYPE_T sec_type)
{
    struct PAGE_SPY_T *ppc_base = (struct PAGE_SPY_T *)PPC_SPY_BTH_X2H_BASE;
    uint32_t prev_mode = hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_BTH, true);
    if (sec_type == HAL_SEC_TYPE_NS)
        ppc_base->REG_000 |= PAGE_SPY_CFG_NONSEC_BYPASS;
    else
        ppc_base->REG_000 &= ~PAGE_SPY_CFG_NONSEC_BYPASS;
    ppc_base->REG_000 |= PAGE_SPY_CFG_SEC_RESP;
    //TRACE(0, "%s, 0x%x", __func__, ppc_base->REG_000);
    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_BTH, prev_mode);
}

static void hal_sec_set_s_btc(enum HAL_SEC_TYPE_T sec_type)
{
    struct PAGE_SPY_T *ppc_base = (struct PAGE_SPY_T *)PPC_SPY_BTC_X2H_BASE;
    uint32_t prev_mode = hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_BTC, true);
    if (sec_type == HAL_SEC_TYPE_NS)
        ppc_base->REG_000 |= PAGE_SPY_CFG_NONSEC_BYPASS;
    else
        ppc_base->REG_000 &= ~PAGE_SPY_CFG_NONSEC_BYPASS;
    ppc_base->REG_000 |= PAGE_SPY_CFG_SEC_RESP;
    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_BTC, prev_mode);
}

static void hal_sec_set_s_sens(enum HAL_SEC_TYPE_T sec_type)
{
    struct PAGE_SPY_T *ppc_base = (struct PAGE_SPY_T *)PPC_SPY_SENS_X2H_BASE;
    uint32_t prev_mode = hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SES, true);
    if (sec_type == HAL_SEC_TYPE_NS)
        ppc_base->REG_000 |= PAGE_SPY_CFG_NONSEC_BYPASS;
    else
        ppc_base->REG_000 &= ~PAGE_SPY_CFG_NONSEC_BYPASS;
    ppc_base->REG_000 |= PAGE_SPY_CFG_SEC_RESP;
    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SES, prev_mode);
}

static void hal_sec_set_s_codec(enum HAL_SEC_TYPE_T sec_type)
{
    struct PAGE_SPY_T *ppc_base = (struct PAGE_SPY_T *)PPC_SPY_CODEC_BASE;
    uint32_t prev_mode = hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_AHB, true);
    if (sec_type == HAL_SEC_TYPE_NS)
        ppc_base->REG_000 |= PAGE_SPY_CFG_NONSEC_BYPASS;
    else
        ppc_base->REG_000 &= ~PAGE_SPY_CFG_NONSEC_BYPASS;
    ppc_base->REG_000 |= PAGE_SPY_CFG_SEC_RESP;
    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_AHB, prev_mode);
}

static void hal_sec_set_s_gpu(enum HAL_SEC_TYPE_T sec_type)
{
    struct PAGE_SPY_T *ppc_base = (struct PAGE_SPY_T *)PPC_SPY_DMA_S_BASE;
    uint32_t prev_mode = hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_GPU, true);
    if (sec_type == HAL_SEC_TYPE_NS)
        ppc_base->REG_000 |= PAGE_SPY_CFG_NONSEC_BYPASS;
    else
        ppc_base->REG_000 &= ~PAGE_SPY_CFG_NONSEC_BYPASS;
    ppc_base->REG_000 |= PAGE_SPY_CFG_SEC_RESP;
    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_GPU, prev_mode);
}

static void hal_sec_set_s_dsp(enum HAL_SEC_TYPE_T sec_type)
{
    struct PAGE_SPY_T *ppc_base = (struct PAGE_SPY_T *)PPC_SPY_DSP_S_BASE;
    uint32_t prev_mode = hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_DSP, true);
    if (sec_type == HAL_SEC_TYPE_NS)
        ppc_base->REG_000 |= PAGE_SPY_CFG_NONSEC_BYPASS;
    else
        ppc_base->REG_000 &= ~PAGE_SPY_CFG_NONSEC_BYPASS;
    ppc_base->REG_000 |= PAGE_SPY_CFG_SEC_RESP;
    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_DSP, prev_mode);
}

static void hal_sec_set_s_xdma(enum HAL_SEC_TYPE_T sec_type)
{
    struct PAGE_SPY_T *ppc_base = (struct PAGE_SPY_T *)PPC_SPY_BTH_X2H_BASE;
    uint32_t prev_mode = hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_AHB, true);
    if (sec_type == HAL_SEC_TYPE_NS)
        ppc_base->REG_000 |= PAGE_SPY_CFG_NONSEC_BYPASS;
    else
        ppc_base->REG_000 &= ~PAGE_SPY_CFG_NONSEC_BYPASS;
    ppc_base->REG_000 |= PAGE_SPY_CFG_SEC_RESP;
    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_AHB, prev_mode);
}

static void hal_sec_init_s_apb0()
{
    sec->REG_004 = SEC_CFG_NONSEC_BYPASS_APB0_MASK;
}

static void hal_sec_init_s_ahb1()
{
    sec->REG_010 = SEC_CFG_NONSEC_BYPASS_AHB1_MASK;
}

static void hal_sec_set_s_flashctrl(enum HAL_SEC_TYPE_T sec_type)
{
    sec->REG_010 &= ~SEC_CFG_NONSEC_BYPASS_AHB1(1 << 9);
    if (sec_type == HAL_SEC_TYPE_NS) {
        sec->REG_00C |= SEC_CFG_NONSEC_AHB1(1 << 9);
    } else {
        sec->REG_00C &= ~SEC_CFG_NONSEC_AHB1(1 << 9);
    }
    //TRACE(0, "%s, 0x%x", __func__, sec->REG_00C);
    //TRACE(0, "%s, 0x%x", __func__, sec->REG_010);
}

static void hal_sec_idau_set_nsc(uint32_t addr)
{
    // length: 1KB
    sec->REG_034 = SEC_IDAU_CALL_ADDR(addr);
    __DSB();
}

static void hal_sec_idau_enable(bool enable)
{
    if (enable) {
        sec->REG_038 |= SEC_IDAU_EN;
    } else {
        sec->REG_038 &= ~SEC_IDAU_EN;
    }
    __DSB();
}

void BOOT_TEXT_FLASH_LOC hal_sec_init(void)
{
    hal_cmu_clock_enable(HAL_CMU_MOD_P_TZC);
    hal_cmu_clock_enable(HAL_CMU_AON_A_TZC);
    hal_cmu_reset_clear(HAL_CMU_MOD_P_TZC);
    hal_cmu_reset_clear(HAL_CMU_AON_A_TZC);

    hal_sec_idau_set_nsc((uint32_t)__nsc_text_start__);
    hal_sec_idau_enable(true);

    // aon master
    hal_sec_set_m_i2c_debug(HAL_SEC_TYPE_NS);
    hal_sec_set_m_sens2aon(HAL_SEC_TYPE_NS);
    hal_sec_set_m_bth2aon(HAL_SEC_TYPE_NS);

    // aon slave
    hal_sec_init_s_aon(); // bypass all

    // axi bus matrix master
    hal_sec_set_m_bcm(HAL_SEC_TYPE_NS);
    hal_sec_set_m_usb(HAL_SEC_TYPE_NS);
    hal_sec_set_m_emmc(HAL_SEC_TYPE_NS);
    hal_sec_set_m_bth_flash(HAL_SEC_TYPE_NS);
    hal_sec_set_m_bth(HAL_SEC_TYPE_NS);
    hal_sec_set_m_btc(HAL_SEC_TYPE_NS);
    hal_sec_set_m_dsp(HAL_SEC_TYPE_NS);
    hal_sec_set_m_display(HAL_SEC_TYPE_NS);
    hal_sec_set_m_dma2d(HAL_SEC_TYPE_NS);
    hal_sec_set_m_gpu(HAL_SEC_TYPE_NS);
    hal_sec_set_m_syshdma_nosec(HAL_SEC_TYPE_NS);
    hal_sec_set_m_sysxdma_nosec(HAL_SEC_TYPE_NS);
    hal_sec_set_m_sens(HAL_SEC_TYPE_NS);
    hal_sec_set_m_spi_slave(HAL_SEC_TYPE_NS);

    // axi bus matrix slave
    hal_sec_set_s_bth(HAL_SEC_TYPE_NS);
    hal_sec_set_s_btc(HAL_SEC_TYPE_NS);
    hal_sec_set_s_sens(HAL_SEC_TYPE_NS);
    hal_sec_set_s_codec(HAL_SEC_TYPE_NS);
    hal_sec_set_s_gpu(HAL_SEC_TYPE_NS);
    hal_sec_set_s_dsp(HAL_SEC_TYPE_NS);
    hal_sec_set_s_xdma(HAL_SEC_TYPE_NS);

    // aph/ahb slave
    hal_sec_init_s_apb0(); // bypass all
    hal_sec_init_s_ahb1(); // bypass all
    hal_sec_set_s_flashctrl(HAL_SEC_TYPE_S);
}

#ifdef CORE_SLEEP_POWER_DOWN
SRAM_RODATA_LOC
static const uint32_t ppc_addr[] = {
    PPC_SPY_BTH_X2H_BASE, PPC_SPY_BTC_X2H_BASE, PPC_SPY_SENS_X2H_BASE,
    PPC_SPY_CODEC_BASE, PPC_SPY_DMA_S_BASE, PPC_SPY_DSP_S_BASE,
};

static uint32_t ppc_regs[ARRAY_SIZE(ppc_addr)];

static struct SEC_T saved_sec_regs;

void SRAM_TEXT_LOC hal_sec_sleep(void)
{
    uint32_t i;
    uint32_t mode;

    mode = hal_cmu_axi_clock_mode_all_manual();
    __DSB();
    for (i = 0; i < ARRAY_SIZE(ppc_addr); i++) {
        ppc_regs[i] = *(volatile uint32_t *)ppc_addr[i];
    }
    __DSB();
    hal_cmu_axi_clock_mode_restore(mode);

    saved_sec_regs.REG_000 = sec->REG_000;
    saved_sec_regs.REG_004 = sec->REG_004;
    saved_sec_regs.REG_008 = sec->REG_008;
    saved_sec_regs.REG_00C = sec->REG_00C;
    saved_sec_regs.REG_010 = sec->REG_010;
    saved_sec_regs.REG_014 = sec->REG_014;
    saved_sec_regs.REG_018 = sec->REG_018;
    saved_sec_regs.REG_01C = sec->REG_01C;
    saved_sec_regs.REG_020 = sec->REG_020;
    saved_sec_regs.REG_024 = sec->REG_024;
    saved_sec_regs.REG_028 = sec->REG_028;
    saved_sec_regs.REG_02C = sec->REG_02C;
    saved_sec_regs.REG_030 = sec->REG_030;
    saved_sec_regs.REG_034 = sec->REG_034;
    saved_sec_regs.REG_038 = sec->REG_038;
}

void SRAM_TEXT_LOC hal_sec_wakeup(void)
{
    uint32_t i;
    uint32_t mode;

    mode = hal_cmu_axi_clock_mode_all_manual();
    __DSB();
    for (i = 0; i < ARRAY_SIZE(ppc_addr); i++) {
        *(volatile uint32_t *)ppc_addr[i] = ppc_regs[i];
    }
    __DSB();
    hal_cmu_axi_clock_mode_restore(mode);

    sec->REG_000 = saved_sec_regs.REG_000;
    sec->REG_004 = saved_sec_regs.REG_004;
    sec->REG_008 = saved_sec_regs.REG_008;
    sec->REG_00C = saved_sec_regs.REG_00C;
    sec->REG_010 = saved_sec_regs.REG_010;
    sec->REG_014 = saved_sec_regs.REG_014;
    sec->REG_018 = saved_sec_regs.REG_018;
    sec->REG_01C = saved_sec_regs.REG_01C;
    sec->REG_020 = saved_sec_regs.REG_020;
    sec->REG_024 = saved_sec_regs.REG_024;
    sec->REG_028 = saved_sec_regs.REG_028;
    sec->REG_02C = saved_sec_regs.REG_02C;
    sec->REG_030 = saved_sec_regs.REG_030;
    sec->REG_034 = saved_sec_regs.REG_034;
    sec->REG_038 = saved_sec_regs.REG_038;
}
#endif

void hal_sec_get_spy_base_list(uint32_t **base, uint32_t *cnt)
{
    static uint32_t spy_base[] = {
        PPC_SPY_BTH_X2H_BASE, PPC_SPY_BTC_X2H_BASE, PPC_SPY_SENS_X2H_BASE, SPY_M55_S_BASE,
        PPC_SPY_CODEC_BASE, PPC_SPY_DMA_S_BASE, PPC_SPY_DSP_S_BASE, SPY_FLASH_BASE,
        SPY_PSRAM_BASE, SPY_SRAM0_BASE, SPY_SRAM1_BASE, SPY_SRAM2_BASE,
        SPY_SRAM3_BASE,
    };

    *base = spy_base;
    *cnt = ARRAY_SIZE(spy_base);

    // Enable clock for all PAGE SPY modules
    hal_cmu_axi_clock_mode_all_manual();
}

#endif
