/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef __ANALOG1809_BEST1603_H__
#define __ANALOG1809_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_ANA_REG(r)                     (((r) & 0xFFF) | 0x1000)
#define PMU_ANA_REG(r)                      (((r) & 0xFFF) | 0x4000)

#define pmu_ana_read(reg, val)              hal_analogif_reg_read(PMU_ANA_REG(reg), val)
#define pmu_ana_write(reg, val)             hal_analogif_reg_write(PMU_ANA_REG(reg), val)

#define MAX_ANA1809_MIC_CH_NUM              3
#define ANA_VAD_MIC_CH                      AUD_CHANNEL_MAP_CH4

enum ANA1809_VCM_USER_T {
    ANA1809_VCM_USER_CODEC       = (1 << 0),
    ANA1809_VCM_USER_RC          = (1 << 1),
};

void analog1809_pu_osc(int enable);

uint32_t analog1809_aud_get_max_dre_gain(void);

void analog_aud_codec_anc_boost(bool en);

int analog_debug_config_vad_mic(bool enable);

void analog1809_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r);

void analog1809_aud_restore_anc_dyn_adc_gain(enum ANC_TYPE_T type);

void analog1809_aud_apply_dyn_adc_gain(enum AUD_CHANNEL_MAP_T ch_map, int16_t gain);

void analog1809_aud_set_adc_gain_direct(enum AUD_CHANNEL_MAP_T ch_map, int gain);

int16_t analog1809_aud_dac_dc_get_step(void);

void analog1809_aud_dc_calib_set_value(uint16_t dc_l, uint16_t dc_r);

void analog1809_aud_dc_calib_get_cur_value(uint16_t *dc_l, uint16_t *dc_r);

void analog1809_aud_dc_calib_get_fixed_comp_value(uint32_t dre_gain, int32_t *dc_l, int32_t *dc_r);

bool analog1809_aud_dc_calib_get_large_ana_dc_value(int16_t *ana_dc, int cur_dig_dc, int tgt_dig_dc,
    int chan, bool init);

void analog1809_aud_dc_calib_enable(bool en);

void analog1809_aud_dac_dc_auto_calib_enable(void);

void analog1809_aud_dac_dc_auto_calib_disable(void);

void analog1809_sleep(void);

void analog1809_wakeup(void);

int analog1809_debug_config_codec(uint16_t mv);

void analog1809_vcm_pmu_on(bool enable);

void analog1809_vcm_enable(enum ANA1809_VCM_USER_T user);

void analog1809_vcm_disable(enum ANA1809_VCM_USER_T user);

void analog1809_codec_lp_vcm(bool en);

void analog1809_codec_vcm_enable_lpf(bool en);

void analog1809_codec_set_vcm_low_vcm_lp(uint16_t v);

void analog1809_codec_set_vcm_low_vcm_lpf(uint16_t v);

void analog1809_codec_set_vcm_low_vcm(uint16_t v);

void analog1809_codec_ci2s_adc_en(bool enable);

void analog1809_codec_ci2s_dac_en(bool enable);

void analog1809_aud_dac_dc_auto_calib_set_mode(enum ANA_DAC_DC_CALIB_MODE_T mode);

void analog1809_open(void);

void analog1809_aud_codec_dac_enable(bool en);

void analog1809_aud_codec_speaker_enable(bool en);

void analog1809_aud_osc_clk_enable(bool enable);

void analog1809_aud_mickey_enable(bool en);

void analog1809_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en);

void analog1809_aud_codec_adc2_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en);

void analog1809_aud_codec_open(void);

void analog1809_aud_codec_close(void);

void analog1809_aud_codec_mute(void);

void analog1809_aud_codec_nomute(void);

void analog1809_aud_codec_anc_enable(enum ANC_TYPE_T type, bool en);

void analog1809_codec_adc_low_delay(void);

void analog1809_rpcsvr_freq_pll_config(uint32_t freq, uint32_t div);

void analog1809_rpcsvr_osc_clk_enable(bool en);

void analog1809_rpcsvr_codec_open(bool en);

void analog1809_rpcsvr_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en);

void analog1809_rpcsvr_vad_enable(enum AUD_VAD_TYPE_T type, bool en);

void analog1809_rpcsvr_vad_adc_enable(bool en);

void analog1809_capsensor_open(void);

void analog1809_capsensor_reset_set(void);

void analog1809_capsensor_reset_clear(void);

void analog1809_capsensor_clk_gate_on(void);

void analog1809_capsensor_clk_gate_off(void);

#ifdef __cplusplus
}
#endif

#endif