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
#ifndef __HAL_DMACFG_BTH_BEST1603_H__
#define __HAL_DMACFG_BTH_BEST1603_H__

#define AUDMA_PERIPH_NUM                        16

#define AUDMA_CHAN_NUM                          8

#define AUDMA_CHAN_START                        (0)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x01C,     // CODEC RX
    CODEC_BASE + 0x01C,     // CODEC TX
#ifdef CODEC_DSD
    CODEC_BASE + 0x034,     // DSD RX
    CODEC_BASE + 0x034,     // DSD TX
#else
    BTPCM_BASE + 0x1C0,     // BTPCM RX
    BTPCM_BASE + 0x1C8,     // BTPCM TX
#endif
    CODEC_BASE + 0x71C,     // CODEC RX2
    CODEC_BASE + 0x71C,     // CODEC TX2
    UART0_BASE + 0x000,     // UART0 RX
    UART0_BASE + 0x000,     // UART0 TX
    UART1_BASE + 0x000,     // UART1 RX
    UART1_BASE + 0x000,     // UART1 TX
#ifdef BTH_USE_SYS_PERIPH
    SDMMC0_BASE + 0x200,    // SDMMC
#else
    0,
#endif
    CODEC_BASE + 0x740,     // CODEC TX3
    BTDUMP_BASE + 0x34,     // BTDUMP
    CODEC_BASE + 0x03C,     // MC RX
    0,                      // FIR RX
    0,                      // FIR TX
};

static const enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
#ifdef CODEC_DSD
    HAL_AUDMA_DSD_RX,
    HAL_AUDMA_DSD_TX,
#else
    HAL_AUDMA_BTPCM_RX,
    HAL_AUDMA_BTPCM_TX,
#endif
    HAL_AUDMA_CODEC_RX2,
    HAL_AUDMA_CODEC_TX2,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
#ifdef BTH_USE_SYS_PERIPH
    HAL_GPDMA_SDMMC0,
#else
    HAL_DMA_PERIPH_NULL,
#endif
    HAL_AUDMA_CODEC_TX3,
    HAL_AUDMA_BTDUMP,
    HAL_AUDMA_MC_RX,
    HAL_AUDMA_FIR_RX,
    HAL_AUDMA_FIR_TX,
};

#endif
