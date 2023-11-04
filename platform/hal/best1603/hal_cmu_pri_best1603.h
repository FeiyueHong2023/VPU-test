
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
#ifndef __HAL_CMU_PRI_BEST1603_H__
#define __HAL_CMU_PRI_BEST1603_H__

#define HAL_CMU_RC_STABLE_TIME          50
#ifdef BT_RCOSC_CAL
#define HAL_CMU_OSC_STABLE_TIME         600
#else
#define HAL_CMU_OSC_STABLE_TIME         2000
#endif

#define HAL_CMU_PLL_LOCKED_TIMEOUT_US   200
#define HAL_CMU_LPU_EXTRA_TIMEOUT_US    0
#ifdef RC48M_ENABLE
#define HAL_CMU_26M_READY_TIMEOUT_US    HAL_CMU_RC_STABLE_TIME
#else
#define HAL_CMU_26M_READY_TIMEOUT_US    HAL_CMU_OSC_STABLE_TIME
#endif

#define HAL_CMU_PLL_LOCKED_TIMEOUT      US_TO_TICKS(HAL_CMU_PLL_LOCKED_TIMEOUT_US)
#define HAL_CMU_26M_READY_TIMEOUT       US_TO_TICKS(HAL_CMU_26M_READY_TIMEOUT_US)
#define HAL_CMU_LPU_EXTRA_TIMEOUT       US_TO_TICKS(HAL_CMU_LPU_EXTRA_TIMEOUT_US)

#define IS_X4_FREQ(f)                   (HAL_CMU_FREQ_48M < (f) && (f) <= HAL_CMU_FREQ_96M)
#define IS_AXI_X4_FREQ(f)               (HAL_CMU_AXI_FREQ_48M < (f) && (f) <= HAL_CMU_AXI_FREQ_96M)

struct SAVED_CMU_REGS_T;

void save_sys_cmu_regs(struct SAVED_CMU_REGS_T *sav);

void restore_sys_cmu_regs(const struct SAVED_CMU_REGS_T *sav);

#endif
