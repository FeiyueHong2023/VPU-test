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
#ifndef __HAL_PSC_BEST1603_H__
#define __HAL_PSC_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "hal_gpio.h"

enum HAL_PSC_STORAGE_MOD_T {
    HAL_PSC_STORAGE_MOD_USB = 0,
    HAL_PSC_STORAGE_MOD_EMMC,
    HAL_PSC_STORAGE_MOD_SDIO,
    HAL_PSC_STORAGE_MOD_SEC_ENG,

    HAL_PSC_STORAGE_MOD_QTY,
    HAL_PSC_STORAGE_MOD_ALL = HAL_PSC_STORAGE_MOD_QTY,
};

enum HAL_PSC_DISPLAY_MOD_T {
    HAL_PSC_DISPLAY_MOD_IMGDMA = 0,
    HAL_PSC_DISPLAY_MOD_GPU,
    HAL_PSC_DISPLAY_MOD_LCDC,
    HAL_PSC_DISPLAY_MOD_QSPI,
    HAL_PSC_DISPLAY_MOD_DSI,

    HAL_PSC_DISPLAY_MOD_QTY,
    HAL_PSC_DISPLAY_MOD_ALL = HAL_PSC_DISPLAY_MOD_QTY,
};

enum HAL_PSC_SYS_USER_T {
    HAL_PSC_SYS_USER_M55,
    HAL_PSC_SYS_USER_DSP,
    HAL_PSC_SYS_USER_PERIPH,

    HAL_PSC_SYS_USER_QTY,
};

void hal_psc_ram_ret_during_reset(void);

void hal_psc_jtag_select_sens(void);

void hal_psc_sys_enable(enum HAL_PSC_SYS_USER_T user);

void hal_psc_sys_disable(enum HAL_PSC_SYS_USER_T user);

void hal_psc_sys_m55_enable(void);

void hal_psc_sys_m55_disable(void);

void hal_psc_sys_dsp_enable(void);

void hal_psc_sys_dsp_disable(void);

void hal_psc_sens_sys_enable(void);

void hal_psc_sens_sys_disable(void);

void hal_psc_sens_enable(void);

void hal_psc_sens_disable(void);

void hal_psc_boot_entry_bth_enable(void);

void hal_psc_bth_enable(void);

void hal_psc_bth_disable(void);

void hal_psc_storage_enable(enum HAL_PSC_STORAGE_MOD_T mod);

void hal_psc_storage_disable(enum HAL_PSC_STORAGE_MOD_T mod);

void hal_psc_display_enable(enum HAL_PSC_DISPLAY_MOD_T mod);

void hal_psc_display_disable(enum HAL_PSC_DISPLAY_MOD_T mod);

void hal_psc_mem_enable(void);

void hal_psc_mem_disable(void);

void hal_psc_gpio_irq_enable(enum HAL_GPIO_PIN_T pin);

void hal_psc_gpio_irq_disable(enum HAL_GPIO_PIN_T pin);

uint32_t hal_psc_gpio_irq_get_status(uint32_t *status, uint32_t cnt);

uint32_t hal_psc_get_power_loop_cycle_cnt(void);

void hal_psc_core_init_ram_ret_mask(void);

void hal_psc_core_ram_pd_sleep(void);

void hal_psc_core_ram_pd_wakeup(void);

void hal_psc_core_ram_ret_sleep(void);

uint32_t hal_psc_sys_get_shared_ram_mask(uint32_t base, uint32_t size);

void hal_psc_btc_ram_pd_sleep(void);

void hal_psc_btc_ram_ret_sleep(void);

uint32_t hal_psc_btc_dpd_ram_enable(uint32_t map);

void hal_psc_btc_dpd_ram_disable(uint32_t sleep_cfg);

#ifdef __cplusplus
}
#endif

#endif

