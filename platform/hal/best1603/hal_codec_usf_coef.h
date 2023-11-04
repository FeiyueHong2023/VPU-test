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
#ifndef __HAL_CODEC_USF_COEF_H__
#define __HAL_CODEC_USF_COEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#define CODEC_DAC1_USF_COEF_ADDR (0x40304800)
#define CODEC_DAC2_USF_COEF_ADDR (0x40304c00)
#define CODEC_DAC3_USF_COEF_ADDR (0x40305000)

enum HAL_CODEC_USF_T {
    HAL_CODEC_USF_DAC1,
    HAL_CODEC_USF_DAC2,
    HAL_CODEC_USF_DAC3,

    HAL_CODEC_USF_QTY,
};

struct CODEC_DAC_USF_SAMPRATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint8_t usf_up_rate; //1:x1, 2:x2, 3:x3, 4:x4, 6:x6, 8:x8
    uint8_t dac_up_rate; //0:x2, 1:x3, 2:x4, 3:x6, 4:x1
    uint8_t bypass_cnt;  //0:x8, 1:x4, 2:x2, 3:x1
    uint8_t rate_sel;
};

struct CODEC_DAC_USF_PARAM_T {
    uint8_t up_sample;
    uint8_t up_sel;
    uint8_t order;
    int32_t *coef;
    uint32_t size;
};

static int32_t dac_usf_coef_1[] = {
    #include "dac_usf_coef_1.txt"
};

static int32_t dac_usf_coef_2[] = {
    #include "dac_usf_coef_2.txt"
};

static int32_t dac_usf_coef_3[] = {
    #include "dac_usf_coef_3.txt"
};

static int32_t dac_usf_coef_4[] = {
    #include "dac_usf_coef.txt"
};

static int32_t dac_usf_coef_6[] = {
    #include "dac_usf_coef_6.txt"
};

static int32_t dac_usf_coef_8[] = {
    #include "dac_usf_coef_8.txt"
};

static struct CODEC_DAC_USF_PARAM_T dac_usf_param[] = {
    {
        .up_sample = 2, // X2
        .up_sel = 0,
        .order = 108,
        .coef = dac_usf_coef_2,
        .size = sizeof(dac_usf_coef_2),
    },
    {
        .up_sample = 3, // X3
        .up_sel = 1,
        .order = 108,
        .coef = dac_usf_coef_3,
        .size = sizeof(dac_usf_coef_3),
    },
    {
        .up_sample = 4, // X4
        .up_sel = 2,
        .order = 108,
        .coef = dac_usf_coef_4,
        .size = sizeof(dac_usf_coef_4),
    },
    {
        .up_sample = 6, // X6
        .up_sel = 3,
        .order = 216,
        .coef = dac_usf_coef_6,
        .size = sizeof(dac_usf_coef_6),
    },
    {
        .up_sample = 8, // X8
        .up_sel = 4,
        .order = 192,
        .coef = dac_usf_coef_8,
        .size = sizeof(dac_usf_coef_8),
    },
    {
        .up_sample = 1, // X1
        .up_sel = 7,
        .order = 108,
        .coef = dac_usf_coef_1,
        .size = sizeof(dac_usf_coef_1),
    },
};

#ifdef __cplusplus
}
#endif

#endif
