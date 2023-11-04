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
#ifndef __HAL_DMACFG_BTH_CFG2_BEST1603_H__
#define __HAL_DMACFG_BTH_CFG2_BEST1603_H__

#define AUDMA_PERIPH_NUM                        16
#define GPDMA_PERIPH_NUM                        16

#define AUDMA_CHAN_NUM                          8
#define GPDMA_CHAN_NUM                          4

#define AUDMA_CHAN_START                        (0)
#define GPDMA_CHAN_START                        (0)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    0,
    0,
    BTPCM_BASE + 0x1C0,     // BTPCM RX
    BTPCM_BASE + 0x1C8,     // BTPCM TX
    0,
    0,
    UART0_BASE + 0x000,     // UART0 RX
    UART0_BASE + 0x000,     // UART0 TX
    UART1_BASE + 0x000,     // UART1 RX
    UART1_BASE + 0x000,     // UART1 TX
    0,
    CODEC_BASE + 0x740,     // CODEC TX3
    BTDUMP_BASE + 0x34,     // BTDUMP
    0,
    0,                      // FIR RX
    0,                      // FIR TX
};

static const enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_AUDMA_BTPCM_RX,
    HAL_AUDMA_BTPCM_TX,
    HAL_AUDMA_CODEC_RX2,
    HAL_DMA_PERIPH_NULL,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_DMA_PERIPH_NULL,
    HAL_AUDMA_CODEC_TX3,
    HAL_AUDMA_BTDUMP,
    HAL_DMA_PERIPH_NULL,
    HAL_AUDMA_FIR_RX,
    HAL_AUDMA_FIR_TX,
};

static const uint32_t gpdma_fifo_addr[GPDMA_PERIPH_NUM] = {
    0,
    SDMMC0_BASE + 0x200,     // SDMMC
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static const enum HAL_DMA_PERIPH_T gpdma_fifo_periph[GPDMA_PERIPH_NUM] = {
    HAL_DMA_PERIPH_NULL,
    HAL_GPDMA_SDMMC0,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
};

#endif
