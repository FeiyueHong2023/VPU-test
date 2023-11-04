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
#ifndef __HAL_MAINCMU_BEST1603_H__
#define __HAL_MAINCMU_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#ifndef HAL_CMU_DEFAULT_CRYSTAL_FREQ
#define HAL_CMU_DEFAULT_CRYSTAL_FREQ        24000000
#endif

#define HAL_CMU_USB_ROM_SELECT_CLOCK_SOURCE

enum HAL_CMU_AXI_FREQ_T {
    HAL_CMU_AXI_FREQ_32K,
    HAL_CMU_AXI_FREQ_3M,
    HAL_CMU_AXI_FREQ_3P25M = HAL_CMU_AXI_FREQ_3M,
    HAL_CMU_AXI_FREQ_6M,
    HAL_CMU_AXI_FREQ_6P5M = HAL_CMU_AXI_FREQ_6M,
    HAL_CMU_AXI_FREQ_12M,
    HAL_CMU_AXI_FREQ_13M = HAL_CMU_AXI_FREQ_12M,
    HAL_CMU_AXI_FREQ_15M,
    HAL_CMU_AXI_FREQ_18M,
    HAL_CMU_AXI_FREQ_21M,
    HAL_CMU_AXI_FREQ_24M,
    HAL_CMU_AXI_FREQ_26M = HAL_CMU_AXI_FREQ_24M,
    HAL_CMU_AXI_FREQ_30M,
    HAL_CMU_AXI_FREQ_36M,
    HAL_CMU_AXI_FREQ_42M,
    HAL_CMU_AXI_FREQ_48M,
    HAL_CMU_AXI_FREQ_52M = HAL_CMU_AXI_FREQ_48M,
    HAL_CMU_AXI_FREQ_60M,
    HAL_CMU_AXI_FREQ_72M,
    HAL_CMU_AXI_FREQ_78M = HAL_CMU_AXI_FREQ_72M,
    HAL_CMU_AXI_FREQ_84M,
    HAL_CMU_AXI_FREQ_96M,
    HAL_CMU_AXI_FREQ_104M = HAL_CMU_AXI_FREQ_96M,
    HAL_CMU_AXI_FREQ_120M,
    HAL_CMU_AXI_FREQ_144M,
    HAL_CMU_AXI_FREQ_168M,
    HAL_CMU_AXI_FREQ_192M,
    HAL_CMU_AXI_FREQ_208M = HAL_CMU_AXI_FREQ_192M,
#ifdef AUD_PLL_DOUBLE
    HAL_CMU_AXI_FREQ_240M,
    HAL_CMU_AXI_FREQ_290M,
    HAL_CMU_AXI_FREQ_340M,
    HAL_CMU_AXI_FREQ_390M,
#endif

    HAL_CMU_AXI_FREQ_QTY,
};

enum HAL_CMU_CLOCK_OUT_ID_T {
    HAL_CMU_CLOCK_OUT_AON_32K           = 0x00,
    HAL_CMU_CLOCK_OUT_AON_OSC           = 0x01,
    HAL_CMU_CLOCK_OUT_AON_OSCX2         = 0x02,
    HAL_CMU_CLOCK_OUT_AON_DIG_OSCX2     = 0x03,
    HAL_CMU_CLOCK_OUT_AON_DIG_OSCX4     = 0x04,
    HAL_CMU_CLOCK_OUT_AON_DCDC          = 0x07,
    HAL_CMU_CLOCK_OUT_AON_SYS           = 0x08,

    HAL_CMU_CLOCK_OUT_BT_NONE           = 0x40,
    HAL_CMU_CLOCK_OUT_BT_32K            = 0x41,
    HAL_CMU_CLOCK_OUT_BT_SYS            = 0x42,
    HAL_CMU_CLOCK_OUT_BT_48M            = 0x43,
    HAL_CMU_CLOCK_OUT_BT_12M            = 0x44,
    HAL_CMU_CLOCK_OUT_BT_ADC            = 0x45,
    HAL_CMU_CLOCK_OUT_BT_ADCD3          = 0x46,
    HAL_CMU_CLOCK_OUT_BT_DAC            = 0x47,
    HAL_CMU_CLOCK_OUT_BT_PM             = 0x48,
    HAL_CMU_CLOCK_OUT_BT_AM             = 0x49,

    HAL_CMU_CLOCK_OUT_SYS_32K           = 0x60,
    HAL_CMU_CLOCK_OUT_SYS_SYS           = 0x61,
    HAL_CMU_CLOCK_OUT_SYS_FLASH0        = 0x62,
    HAL_CMU_CLOCK_OUT_SYS_USB           = 0x63,
    HAL_CMU_CLOCK_OUT_SYS_PCLK          = 0x64,
    HAL_CMU_CLOCK_OUT_SYS_I2S0          = 0x65,
    HAL_CMU_CLOCK_OUT_SYS_CORE          = 0x66,
    HAL_CMU_CLOCK_OUT_SYS_SPDIF0        = 0x67,
    HAL_CMU_CLOCK_OUT_SYS_SDMMC         = 0x68,
    HAL_CMU_CLOCK_OUT_SYS_ISPI          = 0x69,
    HAL_CMU_CLOCK_OUT_SYS_SPI1          = 0x6A,
    HAL_CMU_CLOCK_OUT_SYS_SPI0          = 0x6B,
    HAL_CMU_CLOCK_OUT_SYS_PSRAM         = 0x6C,
    HAL_CMU_CLOCK_OUT_SYS_PIX           = 0x6D,
    HAL_CMU_CLOCK_OUT_SYS_DSI           = 0x6E,
    HAL_CMU_CLOCK_OUT_SYS_EMMC          = 0x6F,
    HAL_CMU_CLOCK_OUT_SYS_GPU           = 0x70,
    HAL_CMU_CLOCK_OUT_SYS_LCDCP         = 0x71,
    HAL_CMU_CLOCK_OUT_SYS_QSPI          = 0x72,
    HAL_CMU_CLOCK_OUT_SYS_PCM           = 0x73,
    HAL_CMU_CLOCK_OUT_SYS_I2S1          = 0x74,

    HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA     = 0x80,
    HAL_CMU_CLOCK_OUT_CODEC_CODEC       = 0x81,
    HAL_CMU_CLOCK_OUT_CODEC_ANC_IIR     = 0x82,
    HAL_CMU_CLOCK_OUT_CODEC_RS_DAC      = 0x83,
    HAL_CMU_CLOCK_OUT_CODEC_RS_ADC      = 0x84,
    HAL_CMU_CLOCK_OUT_CODEC_FIR         = 0x85,
    HAL_CMU_CLOCK_OUT_CODEC_PSAP        = 0x86,
    HAL_CMU_CLOCK_OUT_CODEC_EQ_IIR      = 0x87,
    HAL_CMU_CLOCK_OUT_CODEC_ADC_DSF     = 0x88,
    HAL_CMU_CLOCK_OUT_CODEC_DAC_USF     = 0x89,
    HAL_CMU_CLOCK_OUT_CODEC_HCLK        = 0x8A,

    HAL_CMU_CLOCK_OUT_SENS_32K          = 0xA0,
    HAL_CMU_CLOCK_OUT_SENS_SYS          = 0xA1,
    HAL_CMU_CLOCK_OUT_SENS_PCLK         = 0xA2,
    HAL_CMU_CLOCK_OUT_SENS_UART2        = 0xA3,
    HAL_CMU_CLOCK_OUT_SENS_ISPI         = 0xA4,
    HAL_CMU_CLOCK_OUT_SENS_SPI0         = 0xA5,
    HAL_CMU_CLOCK_OUT_SENS_SPI1         = 0xA6,
    HAL_CMU_CLOCK_OUT_SENS_I2C          = 0xA7,
    HAL_CMU_CLOCK_OUT_SENS_UART0        = 0xA8,
    //HAL_CMU_CLOCK_OUT_SENS_UART1        = 0xA8,
    HAL_CMU_CLOCK_OUT_SENS_JTAG         = 0xA9,
    HAL_CMU_CLOCK_OUT_SENS_VAD_RAM_HCLK = 0xAA,
    HAL_CMU_CLOCK_OUT_SENS_ADC_ANA      = 0xAB,
    HAL_CMU_CLOCK_OUT_SENS_SAR_ADC_ANA  = 0xAC,
    HAL_CMU_CLOCK_OUT_SENS_CODEC        = 0xAD,
    HAL_CMU_CLOCK_OUT_SENS_CODEC_HCLK   = 0xAE,
    HAL_CMU_CLOCK_OUT_SENS_I2S          = 0xAF,

    HAL_CMU_CLOCK_OUT_BTH_32K           = 0xC0,
    HAL_CMU_CLOCK_OUT_BTH_SYS           = 0xC1,
    HAL_CMU_CLOCK_OUT_BTH_FLASH0        = 0xC2,
    HAL_CMU_CLOCK_OUT_BTH_PCLK          = 0xC3,
    HAL_CMU_CLOCK_OUT_BTH_PCM           = 0xC4,
    HAL_CMU_CLOCK_OUT_BTH_ISPI          = 0xC5,
};

#ifndef HAL_CMU_I2S_MCLK_ID_T
enum HAL_CMU_I2S_MCLK_ID_T {
    HAL_CMU_I2S_MCLK_PER                = 0x00,
    HAL_CMU_I2S_MCLK_OSCD2              = 0x01,
    HAL_CMU_I2S_MCLK_PLLI2S0            = 0x02,
    HAL_CMU_I2S_MCLK_PLLI2S1            = 0x03,
    HAL_CMU_I2S_MCLK_OSC                = 0x04,
    HAL_CMU_I2S_MCLK_PLLSPDIF0          = 0x05,
};
#define HAL_CMU_I2S_MCLK_ID_T               HAL_CMU_I2S_MCLK_ID_T
#endif

#ifndef HAL_I2S_ID_T
enum HAL_I2S_ID_T {
    HAL_I2S_ID_0 = 0,
    HAL_I2S_ID_1,

    HAL_I2S_ID_QTY,
};
#define HAL_I2S_ID_T                        HAL_I2S_ID_T
#endif

enum HAL_CMU_ANC_CLK_USER_T {
    HAL_CMU_ANC_CLK_USER_ANC,

    HAL_CMU_ANC_CLK_USER_QTY
};

enum HAL_CMU_RAM_CFG_SEL_T {
    HAL_CMU_RAM_CFG_SEL_BTH = 0,
    HAL_CMU_RAM_CFG_SEL_BTC,
    HAL_CMU_RAM_CFG_SEL_SENS,
    HAL_CMU_RAM_CFG_SEL_SYS,
};

#ifndef HAL_CMU_PLL_T
enum HAL_CMU_PLL_T {
    HAL_CMU_PLL_BB = 0,
    HAL_CMU_PLL_AUD = HAL_CMU_PLL_BB,
    HAL_CMU_PLL_USB,
    HAL_CMU_PLL_DSI,

    HAL_CMU_PLL_QTY
};
#define HAL_CMU_PLL_T                       HAL_CMU_PLL_T
#endif

#ifndef HAL_CMU_PLL_USER_T
enum HAL_CMU_PLL_USER_T {
    HAL_CMU_PLL_USER_SYS,
    HAL_CMU_PLL_USER_AUD,
    HAL_CMU_PLL_USER_USB,
    HAL_CMU_PLL_USER_EMMC,
    HAL_CMU_PLL_USER_GPU,
    HAL_CMU_PLL_USER_LCDC,
    HAL_CMU_PLL_USER_DSI,
    HAL_CMU_PLL_USER_IMGDMA,
    HAL_CMU_PLL_USER_FLASH,
    HAL_CMU_PLL_USER_FLASH1,
    HAL_CMU_PLL_USER_PSRAM,

    HAL_CMU_PLL_USER_QTY,
    HAL_CMU_PLL_USER_ALL = HAL_CMU_PLL_USER_QTY,
};
#define HAL_CMU_PLL_USER_T                  HAL_CMU_PLL_USER_T
#endif

enum HAL_CMU_DCDC_CLOCK_USER_T {
    HAL_CMU_DCDC_CLOCK_USER_GPADC,
    HAL_CMU_DCDC_CLOCK_USER_ADCKEY,
    HAL_CMU_DCDC_CLOCK_USER_EFUSE,
    HAL_CMU_DCDC_CLOCK_USER_PMUIO,
    HAL_CMU_DCDC_CLOCK_USER_CODEC,

    HAL_CMU_DCDC_CLOCK_USER_QTY,
};

enum HAL_CMU_BT_TRIGGER_SRC_T {
    HAL_CMU_BT_TRIGGER_SRC_0,
    HAL_CMU_BT_TRIGGER_SRC_1,
    HAL_CMU_BT_TRIGGER_SRC_2,
    HAL_CMU_BT_TRIGGER_SRC_3,

    HAL_CMU_BT_TRIGGER_SRC_QTY,
};

typedef void (*HAL_CMU_BT_TRIGGER_HANDLER_T)(enum HAL_CMU_BT_TRIGGER_SRC_T src);

enum HAL_CMU_AXI_CLK_ID_T {
    HAL_CMU_AXI_CLK_AHB                  = 0,
    HAL_CMU_AXI_CLK_BTH                  = 1,
    HAL_CMU_AXI_CLK_SES                  = 2,
    HAL_CMU_AXI_CLK_DISPLAY              = 3,
    HAL_CMU_AXI_CLK_FLASH                = 4,
    HAL_CMU_AXI_CLK_GPU                  = 5,
    HAL_CMU_AXI_CLK_GPV                  = 6,
    HAL_CMU_AXI_CLK_DSP                  = 7,
    HAL_CMU_AXI_CLK_M55                  = 8,
    HAL_CMU_AXI_CLK_MAIN                 = 9,
    HAL_CMU_AXI_CLK_PSRAM                = 10,
    HAL_CMU_AXI_CLK_STORE                = 11,
    HAL_CMU_AXI_CLK_ROM                  = 12,
    HAL_CMU_AXI_CLK_SRAM0                = 13,
    HAL_CMU_AXI_CLK_SRAM1                = 14,
    HAL_CMU_AXI_CLK_SRAM2                = 15,
    HAL_CMU_AXI_CLK_SRAM3                = 16,
    HAL_CMU_AXI_CLK_BTC                  = 17,
    HAL_CMU_AXI_CLK_M55_S                = 18,
    HAL_CMU_AXI_CLK_STORE_NIC            = 19,
    HAL_CMU_AXI_CLK_AON                  = 22,
    HAL_CMU_AXI_CLK_PSRAM_MC             = 23,

    HAL_CMU_AXI_CLK_QTY,
};

enum HAL_CMU_CLK_SEL_OSC_USER_T {
    HAL_CMU_CLK_SEL_OSC_USER_SYS        = (1 << 0),
    HAL_CMU_CLK_SEL_OSC_USER_SENS       = (1 << 1),
    HAL_CMU_CLK_SEL_OSC_USER_CODEC      = (1 << 2),
    HAL_CMU_CLK_SEL_OSC_USER_BTH        = (1 << 3),
    HAL_CMU_CLK_SEL_OSC_USER_BTC        = (1 << 4),
};

enum HAL_CMU_AXI_FREQ_USER_T {
    HAL_CMU_AXI_FREQ_USER_USB,
    HAL_CMU_AXI_FREQ_USER_EMMC,
    HAL_CMU_AXI_FREQ_USER_SDIO,
    HAL_CMU_AXI_FREQ_USER_DSP,
    HAL_CMU_AXI_FREQ_USER_APP_0,

    HAL_CMU_AXI_FREQ_USER_QTY,
};

enum HAL_CMU_CODEC_DMA_ACK_T {
    HAL_CMU_CODEC_ADC1_DMA_ACK = 0x01,
    HAL_CMU_CODEC_ADC2_DMA_ACK = 0x02,
    HAL_CMU_CODEC_DAC1_DMA_ACK = 0x04,
    HAL_CMU_CODEC_DAC2_DMA_ACK = 0x08,
    HAL_CMU_CODEC_DAC3_DMA_ACK = 0x10,

    HAL_CMU_CODEC_DMA_ACK_ALL  = 0x1F,
};

enum HAL_CMU_CPU_IDX_T {
    HAL_CMU_CPU_SYS_CORE0,
    HAL_CMU_CPU_BTH_CORE0,
    HAL_CMU_CPU_SENS_CORE0,

    HAL_CMU_CPU_IDX_QTY,
#ifdef BTH_AS_MAIN_MCU
    HAL_CMU_CPU_MCU_CORE0 = HAL_CMU_CPU_BTH_CORE0,
#else
    HAL_CMU_CPU_MCU_CORE0 = HAL_CMU_CPU_SYS_CORE0,
#endif
};

enum HAL_CMU_CPU_REG_IDX_T {
    HAL_CMU_CPU_REG_PC,
    HAL_CMU_CPU_REG_LR,
    HAL_CMU_CPU_REG_SP,

    HAL_CMU_CPU_REG_IDX_QTY,
};

int hal_cmu_axi_freq_req(enum HAL_CMU_AXI_FREQ_USER_T user, enum HAL_CMU_AXI_FREQ_T freq);

enum HAL_CMU_AXI_FREQ_T hal_cmu_axi_freq_get(void);

int hal_cmu_fast_timer_init(void);

enum HAL_CMU_PLL_T hal_cmu_get_audio_pll(void);

void hal_cmu_anc_enable(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_anc_disable(enum HAL_CMU_ANC_CLK_USER_T user);

int hal_cmu_anc_get_status(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_mcu_pdm_clock_out(uint32_t clk_map);

void hal_cmu_sens_pdm_clock_out(uint32_t clk_map);

void hal_cmu_mcu_pdm_data_in(uint32_t data_map);

void hal_cmu_sens_pdm_data_in(uint32_t data_map);

uint32_t hal_cmu_get_aon_chip_id(void);

uint32_t hal_cmu_get_aon_revision_id(void);

void hal_cmu_cp_enable(uint32_t sp, uint32_t entry);

void hal_cmu_cp_disable(void);

uint32_t hal_cmu_cp_get_entry_addr(void);

uint32_t hal_cmu_get_ram_boot_addr(void);

void hal_cmu_dcdc_clock_bypass_div(void);

void hal_cmu_sens_clock_enable(void);

void hal_cmu_sens_clock_disable(void);

void hal_cmu_sens_reset_set(void);

void hal_cmu_sens_reset_clear(void);

void hal_cmu_sens_start_cpu(uint32_t sp, uint32_t entry);

void hal_cmu_sens_ram_cfg(void);

void hal_cmu_bth_clock_enable(void);

void hal_cmu_bth_clock_disable(void);

void hal_cmu_bth_reset_set(void);

void hal_cmu_bth_reset_clear(void);

void hal_cmu_bth_start_cpu(uint32_t sp, uint32_t entry);

void hal_cmu_bth_ram_cfg(void);

void hal_cmu_btc_dpd_ram_enable(uint32_t *old_ram_sel, uint32_t *old_sleep_cfg);

void hal_cmu_btc_dpd_ram_disable(uint32_t old_ram_sel, uint32_t old_sleep_cfg);

void hal_cmu_btc_ram_cfg(void);

void hal_cmu_btc_start_cpu(uint32_t sp, uint32_t entry);

void hal_cmu_ram_cfg_sel_update(uint32_t map, enum HAL_CMU_RAM_CFG_SEL_T sel);

uint32_t hal_cmu_btc_get_shared_ram_mask(uint32_t base, uint32_t size);

int hal_cmu_sys_is_using_audpll(void);

int hal_cmu_flash_is_using_audpll(void);

void hal_cmu_sys_flash_io_enable(void);

void hal_cmu_bth_flash_io_enable(void);

void hal_cmu_codec_set_fault_mask2(uint32_t msk0, uint32_t msk1);

void hal_cmu_codec_dma_ack_select(enum HAL_CMU_CODEC_DMA_ACK_T ack, bool sys_dma);

void hal_cmu_dcdc_clock_enable(enum HAL_CMU_DCDC_CLOCK_USER_T user);

void hal_cmu_dcdc_clock_disable(enum HAL_CMU_DCDC_CLOCK_USER_T user);

void hal_cmu_auto_switch_rc_enable(void);

void hal_cmu_auto_switch_rc_disable(void);

void hal_cmu_select_rc_clock(enum HAL_CMU_CLK_SEL_OSC_USER_T user);

void hal_cmu_select_osc_clock(enum HAL_CMU_CLK_SEL_OSC_USER_T user);

uint32_t hal_cmu_pu_osc_enable(void);

void hal_cmu_pu_osc_disable(uint32_t val);

uint32_t hal_cmu_get_osc_ready_cycle_cnt(void);

uint32_t hal_cmu_get_osc_switch_overhead(void);

void hal_cmu_boot_dcdc_clock_enable(void);

void hal_cmu_boot_dcdc_clock_disable(void);

int hal_cmu_bt_trigger_set_handler(enum HAL_CMU_BT_TRIGGER_SRC_T src, HAL_CMU_BT_TRIGGER_HANDLER_T hdlr);

int hal_cmu_bt_trigger_enable(enum HAL_CMU_BT_TRIGGER_SRC_T src);

int hal_cmu_bt_trigger_disable(enum HAL_CMU_BT_TRIGGER_SRC_T src);

void hal_cmu_main_module_init(void);

void hal_cmu_bth_ram_cfg(void);

void hal_cmu_jtag_select_sys_m55(void);

void hal_cmu_jtag_select_sys_dsp(void);

void hal_cmu_jtag_select_sys_dual(void);

void hal_cmu_sys_m55_itcm_cfg(int enable);

void hal_cmu_sys_m55_dtcm_cfg(int enable);

void hal_cmu_sys_dsp_tcm_cfg(int enable);

void hal_cmu_m55_clock_enable(void);

void hal_cmu_m55_clock_disable(void);

void hal_cmu_m55_reset_set(void);

void hal_cmu_m55_reset_clear(void);

void hal_cmu_m55_cpu_reset_set(void);

void hal_cmu_m55_cpu_reset_clear(uint32_t vector);

void hal_cmu_m55_start_cpu(void);

void hal_cmu_dsp_clock_enable(void);

void hal_cmu_dsp_clock_disable(void);

void hal_cmu_dsp_reset_set(void);

void hal_cmu_dsp_reset_clear(void);

void hal_cmu_dsp_cpu_reset_set(void);

void hal_cmu_dsp_cpu_reset_clear(uint32_t entry);

void hal_cmu_dsp_start_cpu(void);

void hal_cmu_dsp_set_vector(uint32_t entry);

void hal_cmu_psram_dll_clock_enable(void);

void hal_cmu_psram_dll_clock_disable(void);

void hal_cmu_psram_dll_reset_set(void);

void hal_cmu_psram_dll_reset_clear(void);

void hal_cmu_psram_phy_init(int dual);

void hal_cmu_psram_phy_sleep(void);

void hal_cmu_psram_phy_wakeup(int dual);

void hal_cmu_psram_auto_lp_cfg_set(void);

void hal_cmu_psram_auto_lp_cfg_clear(void);

void hal_cmu_dsi_phy_reset_set(void);

void hal_cmu_dsi_phy_reset_clear(void);

void hal_cmu_dsi_clock_enable(void);

void hal_cmu_dsi_clock_disable(void);

void hal_cmu_dsi_sleep(void);

void hal_cmu_dsi_wakeup(void);

void hal_cmu_lcdc_sleep(void);

void hal_cmu_lcdc_wakeup(void);

void hal_cmu_sys_periph_dma_cfg0_req_init(void);

void hal_cmu_sys_module_init(void);

void hal_cmu_sys_usb_clock_enable(void);

void hal_cmu_sys_usb_clock_disable(void);

void hal_cmu_sys_emmc_clock_enable(void);

void hal_cmu_sys_emmc_clock_disable(void);

void hal_cmu_sys_sdmmc0_clock_enable(void);

void hal_cmu_sys_sdmmc0_clock_disable(void);

int hal_cmu_axi_sys_set_freq(enum HAL_CMU_AXI_FREQ_T freq);

enum HAL_CMU_AXI_FREQ_T hal_cmu_axi_sys_get_freq(void);

enum HAL_CMU_PLL_T hal_cmu_get_axi_sys_pll(void);

int hal_cmu_axi_sys_select_pll(enum HAL_CMU_PLL_T pll);

void hal_cmu_init_axi_pll_selection(void);

uint32_t hal_cmu_axi_clock_set_mode(enum HAL_CMU_AXI_CLK_ID_T id, uint32_t manual);

void hal_cmu_sens_force_sleep(void);

void hal_cmu_sens_force_wakeup(void);

void hal_cmu_acquire_x2_x4(void);

void hal_cmu_release_x2_x4(void);

void hal_cmu_boot_flash_reset_pulse(void);

void hal_cmu_force_sys_pll_div(uint32_t div);

uint32_t hal_cmu_axi_clock_mode_all_manual(void);

void hal_cmu_axi_clock_mode_restore(uint32_t val);

int hal_cmu_axi_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user);

int hal_cmu_axi_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user);

uint32_t hal_cmu_get_cpu_reg(enum HAL_CMU_CPU_IDX_T core, enum HAL_CMU_CPU_REG_IDX_T reg);

int hal_cmu_get_aonpmu_efuse(uint32_t page, unsigned short *efuse);

void hal_cmu_aonpmu_sleep_en(unsigned char sleep_en);

void hal_cmu_sys_pvt_enable(uint32_t time_us);

void hal_cmu_bth_pvt_enable(uint32_t time_us);

void hal_cmu_btc_pvt_enable(uint32_t time_us);

void hal_cmu_sens_pvt_enable(uint32_t time_us);

void hal_cmu_aon_pvt_enable(uint32_t time_us);

void hal_cmu_sys_pvt_disable(void);

void hal_cmu_bth_pvt_disable(void);

void hal_cmu_btc_pvt_disable(void);

void hal_cmu_sens_pvt_disable(void);

void hal_cmu_aon_pvt_disable(void);

uint32_t hal_cmu_sys_pvt_cnt_get(void);

uint32_t hal_cmu_bth_pvt_cnt_get(void);

uint32_t hal_cmu_btc_pvt_cnt_get(void);

uint32_t hal_cmu_sens_pvt_cnt_get(void);

uint32_t hal_cmu_aon_pvt_cnt_get(void);

void  hal_cpudump_clock_enable(void);

void hal_cpudump_clock_disable(void);

uint32_t hal_cmu_cpudump_get_last_addr(void);

void hal_cmu_beco_enable(void);

void hal_cmu_beco_disable(void);

void hal_cmu_cp_beco_enable(void);

void hal_cmu_cp_beco_disable(void);

#ifdef __cplusplus
}
#endif

#endif

