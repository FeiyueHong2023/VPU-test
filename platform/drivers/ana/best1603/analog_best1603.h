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
#ifndef __ANALOG_BEST1603_H__
#define __ANALOG_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_ANA_REG(r)                     (((r) & 0xFFF) | 0x1000)

#define MAX_ANA_MIC_CH_NUM                  6
#define ANA_VAD_MIC_CH                      AUD_CHANNEL_MAP_CH4

#define PMU_CODEC_DAC_SHIFT                 0
#define PMU_CODEC_DAC_MASK                  (0x3 << PMU_CODEC_DAC_SHIFT)
#define PMU_CODEC_DAC(n)                    BITFIELD_VAL(PMU_CODEC_DAC, n)
#define MCU_CODEC_DAC_SHIFT                 2
#define MCU_CODEC_DAC_MASK                  (0x3 << MCU_CODEC_DAC_SHIFT)
#define MCU_CODEC_DAC(n)                    BITFIELD_VAL(MCU_CODEC_DAC, n)

enum ANA_VCM_USER_T {
    ANA_VCM_USER_CODEC       = (1 << 0),
    ANA_VCM_USER_RC          = (1 << 1),
};

void analog_pu_osc(int enable);

void analog_aud_pll_set_dig_div(uint32_t div);

uint32_t analog_aud_get_max_dre_gain(void);

void analog_aud_codec_anc_boost(bool en);

int analog_debug_config_vad_mic(bool enable);

void analog_aud_vad_enable(enum AUD_VAD_TYPE_T type, bool en);

void analog_aud_vad_adc_enable(bool en);

enum ANALOG_RPC_ID_T {
    ANALOG_RPC_FREQ_PLL_CFG = 0,
    ANALOG_RPC_OSC_CLK_ENABLE,
    ANALOG_RPC_CODEC_OPEN,
    ANALOG_RPC_CODEC_ADC_ENABLE,
    ANALOG_RPC_VAD_ENABLE,
    ANALOG_RPC_VAD_ADC_ENABLE,
};

enum CODEC_DAC_DEV_T {
    CODEC_DAC_DEV_NONE    = 0,
    PMU_CODEC_DAC_L       = (1 << 0),
    PMU_CODEC_DAC_R       = (1 << 1),
    MCU_CODEC_DAC_L       = (1 << 2),
    MCU_CODEC_DAC_R       = (1 << 3),
};

struct CODEC_DAC_DRE_CFG_T {
    // C0
    uint8_t step_mode;
    uint8_t ini_ana_gain;
    uint8_t dre_delay_lh;
    uint8_t dre_delay_hl;
    uint16_t amp_high;
    // C4
    uint32_t dre_win;
    uint8_t thd_db_offset_sign;
    uint8_t thd_db_offset;
    uint8_t gain_offset;
    // 19C
    uint8_t db_high;
    uint8_t db_low;
    uint8_t top_gain;
    uint8_t delay_dc_lh;
    uint8_t delay_dc_hl;
};

struct CODEC_DAC_CI2S_CFG_T {
    int txch_en;
    int rxch_en;
    int tx_dm;
    int rx_dm;
    int tx_en_mode;
    int rx_en_mode;
    int clk_en_mode;
    int slv;
    int clk_wordsize;

    int rate_sel;
    int en_phase_sel;
    int adc_fifo_th;
    int dac_fifo_th;
    int dac_en_ch0;
    int dac_en_ch1;
    int dac_sinc_rate_sel_ch0;
    int dac_sinc_rate_sel_ch1;

    int tdm_en;
    int tdm_mode;
    int tdm_ws_neg;
    int tdm_frame_width;
    int tdm_ws_width;
    int tdm_slot_width;
    int tdm_data_offset;
    int i2s_data_offset;

    int pol_clk_ci2s_tx;
    int tdm_tx_edge;
};

#ifdef CHIP_SUBSYS_SENS

typedef int (*ANALOG_RPC_REQ_CALLBACK)(enum ANALOG_RPC_ID_T id, uint32_t param0, uint32_t param1, uint32_t param2);

void analog_aud_register_rpc_callback(ANALOG_RPC_REQ_CALLBACK cb);

#else

void analog_rpcsvr_freq_pll_config(uint32_t freq, uint32_t div);

void analog_rpcsvr_osc_clk_enable(bool en);

void analog_rpcsvr_codec_open(bool en);

void analog_rpcsvr_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en);

void analog_rpcsvr_vad_enable(enum AUD_VAD_TYPE_T type, bool en);

void analog_rpcsvr_vad_adc_enable(bool en);

#endif

void analog_aud_dre_dc_sel(bool en);

void analog_aud_dc_calib_set_dre_ana_dc(uint32_t offs, uint16_t dc_offs_l, uint16_t dc_offs_r);

int analog_aud_comparator_calib_dac_dc(void);

void analog_vcm_pmu_on(bool enable);

void analog_vcm_enable(enum ANA_VCM_USER_T user);

void analog_vcm_disable(enum ANA_VCM_USER_T user);

void analog_codec_lp_vcm(bool en);

void analog_codec_vcm_enable_lpf(bool en);

void analog_codec_set_vcm_low_vcm_lp(uint16_t v);

void analog_codec_set_vcm_low_vcm_lpf(uint16_t v);

void analog_codec_set_vcm_low_vcm(uint16_t v);

uint16_t analog_codec_get_dac_dev(void);

/*1809 hal codec*/
void analog1809_aud_dac_dc_offset_enable(int32_t dc_l, int32_t dc_r);

void analog1809_aud_set_dac_ana_gain(uint8_t ini_gain, uint8_t gain_offset);

void analog1809_aud_get_dre_cfg(uint32_t *ini_gain, uint32_t *gain_offs);

void analog1809_aud_dac_dre_init(void);

void analog1809_aud_dac_dre_enable(int32_t dac_dc_l, int32_t dac_dc_r, enum AUD_CHANNEL_MAP_T codec_dac_ch_map);

void analog1809_aud_dac_dre_disable(void);

#ifdef __cplusplus
}
#endif

#endif

