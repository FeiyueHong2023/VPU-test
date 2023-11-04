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
#ifndef __HAL_DMACFG_BEST1603_H__
#define __HAL_DMACFG_BEST1603_H__

#include "plat_addr_map.h"

/* SENS DMA CONFIG FILE */
#ifdef CHIP_SUBSYS_SENS

#include CHIP_SPECIFIC_HDR(hal_dmacfg_sens)

/* BTH DMA CONFIG FILE */
#elif defined(CHIP_SUBSYS_BTH)
#if defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 1)
#include CHIP_SPECIFIC_HDR(hal_dmacfg_bth_cfg1)
#elif defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 2)
#include CHIP_SPECIFIC_HDR(hal_dmacfg_bth_cfg2)
#else
#include CHIP_SPECIFIC_HDR(hal_dmacfg_bth_cfg0)
#endif

/* SYS DMA CONFIG FILE */
#else
#if defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 1)
#include CHIP_SPECIFIC_HDR(hal_dmacfg_sys_cfg1)
#elif defined(CHIP_DMA_CFG_IDX) && (CHIP_DMA_CFG_IDX == 2)
#include CHIP_SPECIFIC_HDR(hal_dmacfg_sys_cfg2)
#else
#include CHIP_SPECIFIC_HDR(hal_dmacfg_sys_cfg0)
#endif

#endif

#endif

