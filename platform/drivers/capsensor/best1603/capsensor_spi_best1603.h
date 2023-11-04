/***************************************************************************
 *
 * Copyright 2022-2023 BES.
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
#ifndef __CAPSENSOR_SPI_BEST1603_H__
#define __CAPSENSOR_SPI_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_gpio.h"

enum CAP_IRQ_MODE_T{
    FP_MODE_RD_INT, //0
    PRESS_INT,      //1
};

#define DURATION_HIGH       900
#define DURATION_LOW        600
#define HAL_GPIO_I2C_DELAY(duration)    hal_sys_timer_delay_ns(duration);

struct GPIO_CAPSPI_CONFIG_T {
    enum HAL_GPIO_PIN_T clk;
    enum HAL_GPIO_PIN_T cs;
    enum HAL_GPIO_PIN_T tx;
    enum HAL_GPIO_PIN_T rx;
    uint32_t speed;
    uint8_t isOpen;  // initial value is 0
};

void capsensor_baseline_dr(uint32_t* baseline_value_p, uint32_t* baseline_value_n);
void capsensor_baseline_reg_read(uint32_t* baseline_value_p, uint32_t* baseline_value_n);
void capsensor_fp_mode_set_mask(void);
void capsensor_fp_mode_clr_mask(void);
void capsensor_fp_mode_clear_irq(void);

#ifdef __cplusplus
}
#endif

#endif

