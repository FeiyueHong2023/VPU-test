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
#ifdef CHIP_SUBSYS_SENS
#include "capsensor_sens_driver_best1603.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "capsensor_spi_best1603.h"
#include "tgt_hardware_capsensor.h"

/***************************************************************************
 * @brief capsensor baseline and sar init control function
 *
 ***************************************************************************/
void capsensor_sar_baseline_init(void)
{
}

uint8_t capsensor_ch_num_get(void)
{
    return CAP_CHNUM;
}

uint8_t capsensor_conversion_num_get(void)
{
    return CAP_REPNUM;
}

uint16_t capsensor_samp_fs_get(void)
{
    return CAP_SAMP_FS;
}

uint8_t capsensor_ch_map_get(void)
{
    return 0;
}

void capsensor_clk_calib(void)
{
}

void capsensor_driver_baseline_dr(uint32_t* baseline_value_p, uint32_t* baseline_value_n)
{
    capsensor_baseline_dr(baseline_value_p, baseline_value_n);
}

void capsensor_driver_baseline_reg_read(uint32_t* baseline_value_p, uint32_t* baseline_value_n)
{
    capsensor_baseline_reg_read(baseline_value_p, baseline_value_n);
}

#endif
