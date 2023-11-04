/***************************************************************************
 *
 * Copyright 2020-2021 BES.
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
#ifndef __HAL_CODEC_DSF_COEF_H__
#define __HAL_CODEC_DSF_COEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#define CODEC_ADC1_DSF_COEF_ADDR (0x40304000)
#define CODEC_ADC2_DSF_COEF_ADDR (0x40304400)

enum ADC_DSF_ID_T {
    ADC_DSF_ID_0,
    ADC_DSF_ID_1,

    ADC_DSF_ID_QTY
};

struct CODEC_ADC_DSF_PARAM_T {
    uint8_t down_sample;
    uint8_t down_sel;
    uint8_t order;  // Max order is 255
    int32_t *coef;
    uint32_t size;
};

struct CODEC_ADC_DSF_CFG_T {
    uint16_t adc1_rate_idx;
    uint16_t adc2_rate_idx;
    struct CODEC_ADC_DSF_PARAM_T *adc1_dsf_param;
    struct CODEC_ADC_DSF_PARAM_T *adc2_dsf_param;
};

struct CODEC_ADC_DSF_SAMPRATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint8_t adc_down;
    uint8_t bypass_cnt;
    uint8_t rate_sel;//0:48K,1:96K,2:192K,3:384K
    uint8_t dsf_ds;
};

static int32_t adc_dsf_coef_1[] = {
    #include "adc_dsf_coef_1.txt"
};

static int32_t adc_dsf_coef_2[] = {
    #include "adc_dsf_coef_2.txt"
};

static int32_t adc_dsf_coef_3[] = {
    #include "adc_dsf_coef_3.txt"
};

static int32_t adc_dsf_coef_4[] = {
    #include "adc_dsf_coef_4.txt"
};

static int32_t adc_dsf_coef_6[] = {
    #include "adc_dsf_coef_6.txt"
};

static int32_t adc_dsf_coef_8[] = {
    #include "adc_dsf_coef_8.txt"
};

static struct CODEC_ADC_DSF_PARAM_T adc_dsf_param[] = {
    {
        //96/1=96, 48/1=48
        .down_sample = 1,
        .down_sel = 7,
        .order = 3,
        .coef = adc_dsf_coef_1,
        .size = sizeof(adc_dsf_coef_1),
    },
    {
        // 96/2=48, 48/2=24
        .down_sample = 2,
        .down_sel = 0,
        .order = 251,
        .coef = adc_dsf_coef_2,
        .size = sizeof(adc_dsf_coef_2),
    },
    {
        // 96/3=32, 48/3=16
        .down_sample = 3,
        .down_sel = 1,
        .order = 108,
        .coef = adc_dsf_coef_3,
        .size = sizeof(adc_dsf_coef_3),
    },
    {
        //96/4=24,48/4=12
        .down_sample = 4,
        .down_sel = 2,
        .order = 120,
        .coef = adc_dsf_coef_4,
        .size = sizeof(adc_dsf_coef_4),
    },
    {
        //96/6=16, 48/6=8
        .down_sample = 6,
        .down_sel = 3,
        .order = 216,
        .coef = adc_dsf_coef_6,
        .size = sizeof(adc_dsf_coef_6),
    },
    {
        //96/8=12
        .down_sample = 8,
        .down_sel = 4,
        .order = 192,
        .coef = adc_dsf_coef_8,
        .size = sizeof(adc_dsf_coef_8),
    },
};

#ifdef __cplusplus
}
#endif

#endif
