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
#include "analog.h"
#include CHIP_SPECIFIC_HDR(reg_analog)
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_codec.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "string.h"
#include "tgt_hardware.h"
#include "analog1809_best1603.h"

#if !defined(SLEEP_PD_CURRENT_TEST) || defined(RC48M_ENABLE)
#define VCM_ON
#endif

// Not using 1uF
#define VCM_CAP_100NF

#define DAC_DC_CALIB_BIT_WIDTH              14

#define DAC_DC_ADJUST_STEP                  90

#define CODEC_TX_EN_ANA_LDAC                RESERVED_ANA_15_0(1 << 2)

#define DAC_DC_SET_VALID_CH(c)              (DAC_DC_VALID_MARK | ((c) & 0xF))
#define DAC_DC_GET_VALID_CH(v)              ((v) & 0xF)
#define DAC_DC_GET_VALID_MARK(v)            ((v) & (~(0xF)))
#define CODEC_COMP_DIV                      12 * 8
#define COMP_VAL_GET_SUM_CNT                101

enum ANA_CODEC_USER_T {
    ANA_CODEC_USER_DAC          = (1 << 0),
    ANA_CODEC_USER_ADC          = (1 << 1),

    ANA_CODEC_USER_CODEC        = (1 << 2),
    ANA_CODEC_USER_MICKEY       = (1 << 3),

    ANA_CODEC_USER_ANC_FF       = (1 << 4),
    ANA_CODEC_USER_ANC_FB       = (1 << 5),
    ANA_CODEC_USER_ANC_TT       = (1 << 6),

    ANA_CODEC_USER_VAD          = (1 << 7),
    ANA_CODEC_USER_RPCSVR_ADC   = (1 << 8),

    ANA_CODEC_USER_ADC2         = (1 << 9),
};

struct ANALOG_PLL_CFG_T {
    uint32_t freq;
    uint8_t div;
    uint64_t val;
};

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static bool anc_calib_mode;

static enum ANA_CODEC_USER_T codec_common_map;
static enum ANA_CODEC_USER_T adda_common_map;
static enum ANA_CODEC_USER_T vcodec_map;

static enum ANA_AUD_PLL_USER_T ana_aud_pll_map;

static BOOT_BSS_LOC bool vcm_pmu_on;

static enum ANA_VCM_USER_T vcm_map;

static uint16_t codec_dac_dev;

#ifdef ANC_APP
#ifndef DYN_ADC_GAIN
#define DYN_ADC_GAIN
#endif
#endif

#if defined (VOICE_DETECTOR_SENS_EN) || defined (VOICE_DETECTOR_EN)
#error "The best1603 sensor don`t have VAD or CODEC!"
#endif

static bool rpcsvr_osc_enable = false;

static bool rc_osc_en;
static bool codec_osc_en;

// Max allowed total tune ratio (5000ppm)
#define MAX_TOTAL_TUNE_RATIO                0.005000

static struct ANALOG_PLL_CFG_T ana_pll_cfg[2];
static int pll_cfg_idx;

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div)
{
    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / OSC) * (1 << 28)

    int i, j;
    uint64_t PLL_cfg_val;
    uint32_t crystal;

    if (pll_cfg_idx < ARRAY_SIZE(ana_pll_cfg) &&
            ana_pll_cfg[pll_cfg_idx].freq == freq &&
            ana_pll_cfg[pll_cfg_idx].div == div) {
        return;
    }

    crystal = hal_cmu_get_crystal_freq();

    j = ARRAY_SIZE(ana_pll_cfg);
    for (i = 0; i < ARRAY_SIZE(ana_pll_cfg); i++) {
        if (ana_pll_cfg[i].freq == freq && ana_pll_cfg[i].div == div) {
            break;
        }
        if (j == ARRAY_SIZE(ana_pll_cfg) && ana_pll_cfg[i].freq == 0) {
            j = i;
        }
    }

    if (i < ARRAY_SIZE(ana_pll_cfg)) {
        pll_cfg_idx = i;
        PLL_cfg_val = ana_pll_cfg[pll_cfg_idx].val;
    } else {
        if (j < ARRAY_SIZE(ana_pll_cfg)) {
            pll_cfg_idx = j;
        } else {
            pll_cfg_idx = 0;
        }

        PLL_cfg_val = ((uint64_t)(1 << 28) * freq * div + crystal / 2) / crystal;

        ana_pll_cfg[pll_cfg_idx].freq = freq;
        ana_pll_cfg[pll_cfg_idx].div = div;
        ana_pll_cfg[pll_cfg_idx].val = PLL_cfg_val;
    }

    bbpll_pll_update(PLL_cfg_val);
}

void analog_aud_pll_tune(float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    if (hal_cmu_get_audio_resample_status()) {
        return;
    }
#endif

    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)
    // Delta = ((SampleDiff / Fs) / TimeDiff) * PLL_cfg_val

    int64_t delta, new_pll;

    if (pll_cfg_idx >= ARRAY_SIZE(ana_pll_cfg) ||
            ana_pll_cfg[pll_cfg_idx].freq == 0) {
        ANALOG_INFO_TRACE(1,"%s: WARNING: aud pll config cache invalid. Skip tuning", __FUNCTION__);
        return;
    }

    if (ABS(ratio) > MAX_TOTAL_TUNE_RATIO) {
        ANALOG_INFO_TRACE(1,"\n------\nWARNING: TUNE: ratio=%d is too large and will be cut\n------\n", FLOAT_TO_PPB_INT(ratio));
        if (ratio > 0) {
            ratio = MAX_TOTAL_TUNE_RATIO;
        } else {
            ratio = -MAX_TOTAL_TUNE_RATIO;
        }
    }

    ANALOG_INFO_TRACE(2,"%s: ratio=%d", __FUNCTION__, FLOAT_TO_PPB_INT(ratio));

    new_pll = (int64_t)ana_pll_cfg[pll_cfg_idx].val;
    delta = (int64_t)(new_pll * ratio);

    new_pll += delta;

    bbpll_pll_update(new_pll);
}

void analog_aud_pll_set_dig_div(uint32_t div)
{
    bbpll_set_dig_div(div);
}

static void analog1603_pu_osc(int enable)
{
    uint16_t val;
    uint32_t lock;

    lock = int_lock();
    rc_osc_en = !!enable;
    if (!codec_osc_en) {
        analog_read(ANA_REG_6C, &val);
        if (enable) {
            val |= REG_PU_OSC;
        } else {
            val &= ~REG_PU_OSC;
        }
        analog_write(ANA_REG_6C, val);
    }
    int_unlock(lock);
}

void analog_pu_osc(int enable)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_pu_osc(enable);
    } else {
        analog1603_pu_osc(enable);
    }
}

static void analog1603_aud_osc_clk_enable(bool enable)
{
    uint16_t val;
    uint32_t lock;
    uint16_t bfv;

    lock = int_lock();
    codec_osc_en = enable;
    if (enable) {
        analog_read(ANA_REG_6C, &val);
        val |= REG_PU_OSC | REG_CRYSTAL_SEL;
        analog_write(ANA_REG_6C, val);
#if 0
        analog_read(ANA_REG_75, &val);
        val |= CFG_TX_CLK_INV;
        analog_write(ANA_REG_75, val);
#endif
    } else {
#if 0
        analog_read(ANA_REG_75, &val);
        val &= ~CFG_TX_CLK_INV;
        analog_write(ANA_REG_75, val);
#endif
        analog_read(ANA_REG_6C, &val);
        if (rc_osc_en) {
            bfv = REG_CRYSTAL_SEL;
        } else {
            bfv = REG_PU_OSC | REG_CRYSTAL_SEL;
        }
        val &= ~bfv;
        analog_write(ANA_REG_6C, val);
    }
    int_unlock(lock);
}

void analog_aud_osc_clk_enable(int enable)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_osc_clk_enable(enable);
    }
    analog1603_aud_osc_clk_enable(enable);
}

void analog_aud_pll_clk_enable(bool enable)
{
    uint16_t val_d9, val_6c, val_81;

    analog_read(ANA_REG_D9, &val_d9);
    analog_read(ANA_REG_6C, &val_6c);
    analog_read(ANA_REG_81, &val_81);

    if (enable) {
        val_d9 |= REG_PU_OSC_PLL24MADC | REG_PU_OSC_PLL24MDAC;
        val_6c |= REG_EXTPLL_SEL;
        val_81 |= CODEC_TX_EN_ANA_LDAC;
        //val_6c &= ~REG_CRYSTAL_SEL_LV;
        //analog_write(ANA_REG_6C, val_6c);
    } else {
        val_d9 &= ~(REG_PU_OSC_PLL24MADC | REG_PU_OSC_PLL24MDAC);
        val_6c &= ~REG_EXTPLL_SEL;
        val_81 &= ~CODEC_TX_EN_ANA_LDAC;
    }
    analog_write(ANA_REG_6C, val_6c);
    analog_write(ANA_REG_D9, val_d9);
    analog_write(ANA_REG_81, val_81);
}

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    int update_pll = false;

    if (user >= ANA_AUD_PLL_USER_END) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {
        analog_aud_osc_clk_enable(true);
        return;
    }
#endif

#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if ((user == ANA_AUD_PLL_USER_I2S || user == ANA_AUD_PLL_USER_SPDIF) &&
            (ana_aud_pll_map & (ANA_AUD_PLL_USER_I2S | ANA_AUD_PLL_USER_SPDIF)) == 0) {
        hal_cmu_axi_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
    if (!(user == ANA_AUD_PLL_USER_I2S || user == ANA_AUD_PLL_USER_SPDIF) &&
            (ana_aud_pll_map & ~(ANA_AUD_PLL_USER_I2S | ANA_AUD_PLL_USER_SPDIF)) == 0) {
        update_pll = true;
    }
#else
    update_pll = (ana_aud_pll_map == 0);
#endif
    if (update_pll) {
        analog_aud_pll_clk_enable(true);
        // TODO: If BTH_CODEC_I2C_SLAVE defined, PLL on sys should be enabled when IIR/RS uses PLL
        hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
    ana_aud_pll_map |= user;
}

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    int update_pll = false;

    if (user >= ANA_AUD_PLL_USER_END) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {

        analog_aud_osc_clk_enable(false);
        return;
    }
#endif

    ana_aud_pll_map &= ~user;
#if defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU) && defined(BTH_USE_SYS_PERIPH)
    if ((user == ANA_AUD_PLL_USER_I2S || user == ANA_AUD_PLL_USER_SPDIF) &&
            (ana_aud_pll_map & (ANA_AUD_PLL_USER_I2S | ANA_AUD_PLL_USER_SPDIF)) == 0) {
        hal_cmu_axi_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
    if (!(user == ANA_AUD_PLL_USER_I2S || user == ANA_AUD_PLL_USER_SPDIF) &&
            (ana_aud_pll_map & ~(ANA_AUD_PLL_USER_I2S | ANA_AUD_PLL_USER_SPDIF)) == 0) {
        update_pll = true;
    }
#else
    update_pll = (ana_aud_pll_map == 0);
#endif
    if (update_pll) {
        hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
        analog_aud_pll_clk_enable(false);
    }
}

#ifdef RC48M_ENABLE
void analog_rc48m_enable(void)
{
}
#endif

static void analog_aud_enable_dac_with_classab(uint32_t dac, bool switch_pa)
{
    uint16_t val_6d;
    uint16_t val_71;
    uint16_t val_74;
    uint16_t val_75;
    uint16_t val_81;
    uint16_t val_73;
    uint16_t val_d7;

    analog_read(ANA_REG_6D, &val_6d);
    analog_read(ANA_REG_71, &val_71);
    analog_read(ANA_REG_73, &val_73);
    analog_read(ANA_REG_74, &val_74);
    analog_read(ANA_REG_75, &val_75);
    analog_read(ANA_REG_81, &val_81);
    analog_read(ANA_REG_D7, &val_d7);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_6d |= REG_CODEC_TX_EAR_ENBIAS | REG_CODEC_TX_EAR_LPBIAS;
        analog_write(ANA_REG_6D, val_6d);
        osDelay(1);
        val_75 |= CFG_TX_TREE_EN;
        analog_write(ANA_REG_75, val_75);
        osDelay(1);

        val_74 |= REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_LCLK;
        if (!switch_pa) {
            val_73 |= REG_CODEC_TX_EN_LDAC;
            val_75 |= REG_CODEC_TX_EN_LDAC_ANA;
        }
        if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
            // CODEC_TX_EN_ANA_LDAC 1809 fail
            val_81 |= CODEC_TX_EN_ANA_LDAC;
        }
        analog_write(ANA_REG_D7, val_d7);
        analog_write(ANA_REG_73, val_73);
        analog_write(ANA_REG_74, val_74);
        val_71 |= REG_CODEC_TX_EN_DACLDO;
        analog_write(ANA_REG_71, val_71);
        val_75 |= REG_CODEC_TX_EN_LPPA;
        analog_write(ANA_REG_75, val_75);
        osDelay(1);
        val_74 |= REG_CODEC_TX_EN_S1PA;
        analog_write(ANA_REG_74, val_74);
        analog_write(ANA_REG_81, val_81);
        // Ensure 1ms delay before enabling dac_pa
        osDelay(1);
    } else {
        // Ensure 1ms delay after disabling dac_pa
        osDelay(1);
        val_74 &= ~REG_CODEC_TX_EN_S1PA;
        analog_write(ANA_REG_74, val_74);
        osDelay(1);
        val_74 &= ~(REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_LCLK);
        if (!switch_pa) {
            val_73 &= ~REG_CODEC_TX_EN_LDAC;
            val_75 &= ~REG_CODEC_TX_EN_LDAC_ANA;
        }
        analog_write(ANA_REG_D7, val_d7);
        analog_write(ANA_REG_73, val_73);
        analog_write(ANA_REG_74, val_74);
        if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
            val_81 &= ~CODEC_TX_EN_ANA_LDAC;
        }
        analog_write(ANA_REG_81, val_81);
        val_71 &= ~REG_CODEC_TX_EN_DACLDO;
        analog_write(ANA_REG_71, val_71);
        val_75 &= ~REG_CODEC_TX_EN_LPPA;
        analog_write(ANA_REG_75, val_75);
        osDelay(1);

        val_75 &= ~CFG_TX_TREE_EN;
        analog_write(ANA_REG_75, val_75);
        osDelay(1);

        val_6d &= ~(REG_CODEC_TX_EAR_ENBIAS | REG_CODEC_TX_EAR_LPBIAS);
        analog_write(ANA_REG_6D, val_6d);
    }
}

static void analog_aud_enable_dac_pa_classab(uint32_t dac)
{
    uint16_t val_74;

    analog_read(ANA_REG_74, &val_74);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_74 |= REG_CODEC_TX_EN_S4PA;
    } else {
        val_74 &= ~REG_CODEC_TX_EN_S4PA;
    }
    analog_write(ANA_REG_74, val_74);
}

static void analog_aud_enable_dac(uint32_t dac)
{
    analog_aud_enable_dac_with_classab(dac, false);
}

static void analog_aud_enable_dac_pa_internal(uint32_t dac)
{
    analog_aud_enable_dac_pa_classab(dac);
}

static void analog_aud_enable_dac_pa(uint32_t dac)
{
    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        analog_aud_enable_dac_pa_internal(dac);

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_clear();
#endif
    } else {
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_set();
#endif

        analog_aud_enable_dac_pa_internal(dac);
    }
}

static void analog_codec_enable_vcm_buffer(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_F7, &val);
    if (en) {
        val |= REG_CODEC_EN_VCMBUFFER;
    } else {
        val &= ~REG_CODEC_EN_VCMBUFFER;
    }
    analog_write(ANA_REG_F7, val);
}

static void analog_codec_enable_bias(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_F6, &val);
    if (en) {
        val |= REG_CODEC_EN_BIAS;
    } else {
        val &= ~REG_CODEC_EN_BIAS;
    }
    analog_write(ANA_REG_F6, val);
}

void analog_aud_set_adc_gain_direct(enum AUD_CHANNEL_MAP_T ch_map, int gain)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_set_adc_gain_direct(ch_map, gain);
    }
}

#ifdef ANC_APP

void analog_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_apply_anc_adc_gain_offset(type, offset_l, offset_r);
    }
}

void analog_aud_restore_anc_dyn_adc_gain(enum ANC_TYPE_T type)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_restore_anc_dyn_adc_gain(type);
    }
}
#endif

#ifdef DYN_ADC_GAIN
void analog_aud_apply_dyn_adc_gain(enum AUD_CHANNEL_MAP_T ch_map, int16_t gain)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_apply_dyn_adc_gain(ch_map, gain);
    }
}
#endif

void analog_aud_set_dac_gain(int32_t v)
{
}

uint32_t analog_codec_get_dac_gain(void)
{
    return 0;
}

uint32_t analog_codec_dac_gain_to_db(int32_t gain)
{
    return 0;
}

int32_t analog_codec_dac_max_attn_db(void)
{
    return 0;
}

void static set_codec_dac_dev(void)
{
    uint16_t efuse_val;
    pmu_get_efuse(PMU__EFUSE_PAGE_RESERVED_22, &efuse_val);
    codec_dac_dev = (0x1E & efuse_val) >> 1;

    TRACE(1, "get_codec_dac_dev: 0x%x",codec_dac_dev);
    if (codec_dac_dev == 0) {
        codec_dac_dev = MCU_CODEC_DAC_L | PMU_CODEC_DAC_R;
    }
    TRACE(1, "set_codec_dac_dev: 0x%x",codec_dac_dev);
}

uint16_t analog_codec_get_dac_dev(void)
{
    return codec_dac_dev;
}

static uint16_t POSSIBLY_UNUSED ana_dc_calib_check_valid(void)
{
    int i;
    uint16_t efuse_val;

    for (i = 0; i < 2; i++) {
        pmu_get_efuse(PMU__EFUSE_PAGE_RESERVED_20 + i, &efuse_val);
        TRACE(1, "ana_dc_calib_check_valid: ana_dc=0x%x", efuse_val&0x3fff);
    }

    return 0;
}

static uint16_t POSSIBLY_UNUSED ana_dc_calib_get_dig_gain(void)
{
    uint16_t efuse_val;
    pmu_get_efuse(PMU__EFUSE_PAGE_RESERVED_19, &efuse_val);

    TRACE(1, "ana_dc_calib_get_dig_gain: ana_gain=%d", (int)efuse_val);
    return 0;
}

static int POSSIBLY_UNUSED dc_calib_checksum_valid(uint32_t efuse)
{
    int i;
    uint32_t cnt = 0;
    uint32_t chksum_mask = (1 << (16 - DAC_DC_CALIB_BIT_WIDTH)) - 1;

    for (i = 0; i < DAC_DC_CALIB_BIT_WIDTH; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & chksum_mask) == ((efuse >> DAC_DC_CALIB_BIT_WIDTH) & chksum_mask));
}

bool analog_aud_dc_calib_valid(void)
{
    return false;
}

uint16_t analog_aud_dac_dc_diff_to_val(int32_t diff)
{
    uint16_t val;

    // BIT 13: SIGN
    // BIT 12: x256
    // BIT 11: x128
    // BIT 10: x64
    // BIT  9: x64
    // BIT  8: x32
    // BIT  7: x16
    // BIT  6: x8
    // BIT  5: x8
    // BIT  4: x4
    // BIT  3: x2
    // BIT  2: x2
    // BIT  1: x1
    // BIT  1: x1

    val = 0;
    if (diff < 0) {
        val |= (1 << 13);
        diff = -diff;
    }
    if (diff & (1 << 8)) {
        val |= (1 << 12);
    }
    if (diff & (1 << 7)) {
        val |= (1 << 11);
    }
    if (diff & (1 << 6)) {
        val |= (1 << 9);
    }
    if (diff & (1 << 5)) {
        val |= (1 << 8);
    }
    if (diff & (1 << 4)) {
        val |= (1 << 7);
    }
    if (diff & (1 << 3)) {
        val |= (1 << 5);
    }
    if (diff & (1 << 2)) {
        val |= (1 << 4);
    }
    if (diff & (1 << 1)) {
        val |= (1 << 2);
    }
    if (diff & (1 << 0)) {
        val |= (1 << 0);
    }

    return val;
}

uint16_t analog_aud_dc_calib_val_to_efuse(uint16_t val)
{
    int i;
    uint32_t cnt = 0;
    uint32_t chksum_mask = (1 << (16 - DAC_DC_CALIB_BIT_WIDTH)) - 1;
    uint32_t val_mask = (1 << DAC_DC_CALIB_BIT_WIDTH) - 1;

    for (i = 0; i < DAC_DC_CALIB_BIT_WIDTH; i++) {
        if (val & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & chksum_mask) << DAC_DC_CALIB_BIT_WIDTH) | (val & val_mask);
}

int16_t analog1603_aud_dac_dc_get_step(void)
{
    return DAC_DC_ADJUST_STEP;
}

int16_t analog_aud_dac_dc_get_step(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        return analog1809_aud_dac_dc_get_step();
    }
    // 1603 dac should not use AUDIO_OUTPUT_DC_CALIB_ANA
    return 0;
}

void analog_aud_save_dc_calib(uint16_t val)
{
}

static void analog1603_aud_dc_calib_set_value(uint16_t dc_l, uint16_t dc_r)
{
    uint16_t val;

    analog_read(ANA_REG_71, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL_TMP, dc_l);
    analog_write(ANA_REG_71, val);
}

void analog_aud_dc_calib_set_value(uint16_t dc_l, uint16_t dc_r)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_dc_calib_set_value(dc_l, dc_r);
    }
}

void analog_aud_dc_calib_get_cur_value(uint16_t *dc_l, uint16_t *dc_r)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_dc_calib_get_cur_value(dc_l, dc_r);
    }
}

void analog_aud_dc_calib_get_fixed_comp_value(uint32_t dre_gain, int32_t *dc_l, int32_t *dc_r)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_dc_calib_get_fixed_comp_value(dre_gain, dc_l, dc_r);
    }
}

#ifdef AUDIO_ANA_DC_RANGE_SEL
static void analog_aud_dc_calib_ana_dc_range_sel(void)
{
    uint16_t val;
    // improve ana_dc conpensation accuracy(max ana_dc_val conpensat 1.6mv)
    analog_read(ANA_REG_7A, &val);
    val |= REG_CODEC_TX_OC_PATH;
    analog_write(ANA_REG_7A, val);
}
#endif

bool analog1603_aud_dc_calib_get_large_ana_dc_value(int16_t *ana_dc, int cur_dig_dc, int tgt_dig_dc,
    int chan, bool init)
{
#define DAC_LARGE_ANA_DC_WEIGHT_INIT_R (0x0)
#define DAC_LARGE_ANA_DC_WEIGHT_INIT_L (0x20)  //DC=230uv
#define DAC_LARGE_ANA_DC_WEIGHT_STEP   (0x10)  //DC=100uv
#define DAC_DIG_DC_THRES_MIN    (3000)

    bool success = false;
    int16_t dc;
    int comp_dig_dc;
    if (!ana_dc) {
        return success;
    }
    comp_dig_dc = tgt_dig_dc - cur_dig_dc;
    if (init) {
        if (chan == 0) {
            *ana_dc = DAC_LARGE_ANA_DC_WEIGHT_INIT_L;
        } else {
            *ana_dc = DAC_LARGE_ANA_DC_WEIGHT_INIT_R;
        }

        dc = ABS(*ana_dc);
        if (comp_dig_dc > 0) {
            dc = -dc;
        }
        *ana_dc = dc;
    } else {
        if (ABS(comp_dig_dc) >= DAC_DIG_DC_THRES_MIN) {
            success = true;
        } else {
            dc = ABS(*ana_dc);
            dc += DAC_LARGE_ANA_DC_WEIGHT_STEP;
            if (comp_dig_dc > 0) {
                dc = -dc;
            }
            *ana_dc = dc;
        }
    }
    return success;
}

bool analog_aud_dc_calib_get_large_ana_dc_value(int16_t *ana_dc, int cur_dig_dc, int tgt_dig_dc,
    int chan, bool init)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        return analog1809_aud_dc_calib_get_large_ana_dc_value(ana_dc, cur_dig_dc, tgt_dig_dc, chan, init);
    }
    return 0;
}

static void analog1603_aud_dc_calib_enable(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_71, &val);
    if (en) {
        val |= REG_CODEC_TX_EAR_OFFEN;
    } else {
        val &= ~REG_CODEC_TX_EAR_OFFEN;
    }
    analog_write(ANA_REG_71, val);
}

void analog_aud_dc_calib_enable(bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        if (GET_BITFIELD(codec_dac_dev, PMU_CODEC_DAC)) {
            analog1809_aud_dc_calib_enable(en);
        }
    }
}

void analog_aud_dac_dc_auto_calib_enable(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_dac_dc_auto_calib_enable();
    }
}

void analog_aud_dac_dc_auto_calib_disable(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_dac_dc_auto_calib_disable();
    }
}

void analog_aud_dac_dc_auto_calib_set_mode(enum ANA_DAC_DC_CALIB_MODE_T mode)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_dac_dc_auto_calib_set_mode(mode);
    }
}

static void analog_aud_vcodec_enable(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (vcodec_map == 0) {
            set = true;
        }
        vcodec_map |= user;
    } else {
        vcodec_map &= ~user;
        if (vcodec_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        pmu_codec_config(!!vcodec_map);
    }
}

static void analog_aud_enable_common_internal(enum ANA_CODEC_USER_T user, bool en)
{
#ifdef SPLIT_VMEM_VMEM2
    enum PMU_CHIP_TYPE_T pmu_type = pmu_get_chip_type();

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806) {
        return;
    }
#endif

    uint32_t lock;
    bool set = false;
    bool vcm_en = false;
    bool vcm_en_lpf = false;

    lock = int_lock();
    if (en) {
        if (codec_common_map == 0) {
            set = true;
        }
        codec_common_map |= user;
    } else {
        codec_common_map &= ~user;
        if (codec_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        if (codec_common_map) {
            vcm_en = true;
        } else {
            vcm_en = false;
            vcm_en_lpf = false;
        }
        if (codec_common_map) {
            // RTOS application startup time is long enougth for VCM charging
#if !(defined(VCM_ON) && defined(RTOS)) || defined(RC48M_ENABLE)
            // VCM fully stable time is about 60ms/1.95V or 150ms/1.7V
            // Quick startup:
            // 1) Disable VCM LPF and target to a higher voltage than the required one
            // 2) Wait for a short time when VCM is in quick charge (high voltage)
            // 3) Enable VCM LPF and target to the required VCM LPF voltage
            analog_codec_set_vcm_low_vcm(0);
            analog_vcm_enable(ANA_VCM_USER_CODEC);

            uint32_t delay;

#if defined(VCM_CAP_100NF)
            if (vcodec_mv >= 1900) {
                delay = 6;
            } else {
                delay = 10;
            }
#else
            if (vcodec_mv >= 1900) {
                delay = 25;
            } else {
                delay = 100;
            }
#endif
            osDelay(delay);
#if 0
            // Target to a voltage near the required one
            analog_write(ANA_REG_6B, val_6b);
            osDelay(10);
#endif
#endif // !(VCM_ON && RTOS)
            vcm_en_lpf = true;
        }
        if (vcm_en) {
            analog_vcm_enable(ANA_VCM_USER_CODEC);
        } else {
            analog_vcm_disable(ANA_VCM_USER_CODEC);
        }
        analog_codec_vcm_enable_lpf(vcm_en_lpf);
    }
}

static void analog_aud_enable_codec_common(enum ANA_CODEC_USER_T user, bool en)
{
#ifndef VCM_ON
    analog_aud_enable_common_internal(user, en);
#endif
}

static void analog_aud_enable_adda_common(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t reg53_val;
    bool set = false;
    bool en_bias = false;

    lock = int_lock();
    if (en) {
        if (adda_common_map == 0) {
            set = true;
        }
        adda_common_map |= user;
    } else {
        adda_common_map &= ~user;
        if (adda_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        /* bypass or pu tx regulator */
        analog_read(ANA_REG_53, &reg53_val);
        if (adda_common_map) {
            reg53_val |= REG_BYPASS_TX_REGULATOR;
            en_bias = true;
        } else {
            reg53_val &= ~REG_BYPASS_TX_REGULATOR;
            en_bias = false;
        }
        analog_codec_enable_bias(en_bias);

#if defined(CODEC_TX_PEAK_DETECT)
        analog_codec_enable_vcm_buffer(!!adda_common_map);
#endif
        analog_write(ANA_REG_53, reg53_val);
    }
}

uint32_t analog_aud_get_max_dre_gain(void)
{
    // From REG_CODEC_TX_EAR_DRE_GAIN_L
    return 0x10;
}

void static analog_codec_low_power_mode(void)
{
    uint16_t val;
    if (vcodec_mv <=1500) {
        analog_read(ANA_REG_DD, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_ST1_LP_L, 2);
        analog_write(ANA_REG_DD, val);

        analog_read(ANA_REG_6F, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_COMP1_L, 0x12);
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_COMP2_L, 0xC3);
        analog_write(ANA_REG_6F, val);
    }
}

int analog_reset(void)
{
    return 0;
}

void analog_open(void)
{
    uint16_t val;

    analog_read(ANA_REG_F8, &val);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL, 0x8);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL_TX, 0x8);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL_VOICE, 0x8);
    analog_write(ANA_REG_F8, val);

    val = RESERVED_DIG_32;
    analog_write(ANA_REG_50, val);

#ifdef SPLIT_VMEM_VMEM2
    enum PMU_CHIP_TYPE_T pmu_type = pmu_get_chip_type();

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806) {
    } else
#else
    {
        analog_codec_set_vcm_low_vcm(0x7);
        analog_codec_set_vcm_low_vcm_lp(0x7);
        analog_codec_set_vcm_low_vcm_lpf(0x7);
    }
#endif

    analog_read(ANA_REG_F8, &val);
    val = SET_BITFIELD(val, REG_CODEC_BUF_LOWVCM, 0x4);
    analog_write(ANA_REG_F8, val);

    val = REG_TX_REGULATOR_BIT(8);
#ifdef CODEC_TX_PEAK_DETECT
    val |= REG_CODEC_TX_PEAK_NL_EN | REG_CODEC_TX_PEAK_PL_EN;
#endif
    analog_write(ANA_REG_6C, val);

    val = REG_CODEC_TX_EAR_DRE_GAIN_L(0x10)
#ifdef DAC_DRE_GAIN_DC_UPDATE
        | REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE
#endif
        | REG_CODEC_TX_EAR_OCEN;
    analog_write(ANA_REG_6D, val);

    val = REG_CODEC_TX_DAC_VREF_L(7);
    analog_write(ANA_REG_6E, val);

    val = REG_CODEC_TX_EAR_COMP2_L(0x8B) | REG_CODEC_TX_EAR_COMP1_L(0x0F);
    analog_write(ANA_REG_6F, val);

    val = REG_CODEC_TX_EAR_LOWGAINL(3);
    analog_write(ANA_REG_70, val);

    val = REG_CODEC_TX_EAR_OUTPUTSEL_L(1) | REG_CODEC_TX_EAR_SOFTSTART(0x38) | REG_CODEC_TX_EAR_DR_ST_L(2);
    analog_write(ANA_REG_73, val);

    val = REG_CODEC_DAC_CLK_EDGE_SEL | REG_CODEC_TX_EAR_GAIN_TMP(1);
    analog_write(ANA_REG_74, val);

    val = REG_CODEC_TX_EAR_VCM_L(6)
        | REG_CODEC_TX_OUTPUT_L_LP_DR;
    // DAC dre gain select dig ctrl mode
    val |= DRE_GAIN_SEL_L;
    analog_write(ANA_REG_75, val);

    val = REG_CODEC_DAC_LOWGAIN(1)
        |REG_CODEC_TX_VREFBUF_CSEL_L(3);
    analog_write(ANA_REG_D7, val);

    val = REG_CODEC_CLKMUX_VREFGEN_EN
        | REG_CODEC_TX_REG_LP(1)
        | REG_CODEC_TX_OCP_SEL_L(3);
    analog_write(ANA_REG_D8, val);

    val = REG_CODEC_TX_ST2_LP_L(2)
        | REG_CODEC_TX_CASP_L(2)
        | REG_CODEC_TX_CASN_L(2);
    analog_write(ANA_REG_DB, val);

    val = REG_CODEC_TX_VREFBUF_LP(7) | REG_CODEC_TX_EAR_ST1_LP_L(0);
    analog_write(ANA_REG_DD, val);

    // DAC dre dc select ana ctrl mode
    analog_aud_dre_dc_sel(true);
    // enbale ana dc composition
    analog1603_aud_dc_calib_enable(true);

    analog_codec_low_power_mode();
#ifdef AUDIO_ANA_DC_RANGE_SEL
    analog_aud_dc_calib_ana_dc_range_sel();
#endif

#ifdef CODEC_TX_ZERO_CROSSING_EN_GAIN
    val = REG_PU_ZERO_DET_L | REG_PU_ZERO_DET_R | REG_ZERO_DETECT_CHANGE;
    analog_write(ANA_REG_7A, val);
#endif

#ifdef VCM_ON
    analog_aud_enable_common_internal(ANA_CODEC_USER_DAC, true);
#endif

#ifdef RC48M_ENABLE
#ifndef VCM_ON
#error "RC48M_ENABLE should work with VCM_ON"
#endif
    analog_rc48m_enable();
#endif

    set_codec_dac_dev();

    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog_vcm_enable(ANA_VCM_USER_CODEC);
        analog_codec_lp_vcm(true);
        analog1809_open();
    }
}

static void analog1603_sleep(void)
{
#ifndef VCM_ON
    if (codec_common_map)
#endif
    {
        analog_codec_lp_vcm(true);
    }
}

void analog_sleep(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_sleep();
    }
    analog1603_sleep();
}

static void analog1603_wakeup(void)
{
#ifndef VCM_ON
    if (codec_common_map)
#endif
    {
        analog_codec_lp_vcm(false);
    }
}

void analog_wakeup(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_wakeup();
    }
    analog1603_wakeup();
}

void analog_aud_codec_anc_enable(enum ANC_TYPE_T type, bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_codec_anc_enable(type, en);
    }
}

void analog_aud_codec_anc_boost(bool en)
{
}

void analog_aud_mickey_enable(bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_mickey_enable(en);
    }
}

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_codec_adc_enable(input_path, ch_map, en);
    }
}

void analog_aud_codec_adc2_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_aud_codec_adc2_enable(input_path, ch_map, en);
    }
}

static void analog_aud_codec_config_speaker(void)
{
    bool en;

    if (ana_spk_req && !ana_spk_muted) {
        en = true;
    } else {
        en = false;
    }

    if (ana_spk_enabled != en) {
        ana_spk_enabled = en;
        if (en) {
            analog_aud_enable_dac_pa(GET_BITFIELD(codec_dac_dev, MCU_CODEC_DAC));
        } else {
            analog_aud_enable_dac_pa(0);
        }
    }
}

static void analog1603_aud_codec_speaker_enable(bool en)
{
    ana_spk_req = en;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_speaker_enable(bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        if (GET_BITFIELD(codec_dac_dev, PMU_CODEC_DAC)) {
            analog1809_aud_codec_speaker_enable(en);
        }
    }
    if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV & GET_BITFIELD(codec_dac_dev, MCU_CODEC_DAC)) {
        analog1603_aud_codec_speaker_enable(en);
    }
}

static void analog1603_aud_codec_dac_enable(bool en)
{
    if (en) {
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, true);
        pmu_codec_hppa_enable(1);
        analog_aud_enable_dac(GET_BITFIELD(codec_dac_dev, MCU_CODEC_DAC));
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        osDelay(1);
        analog1603_aud_codec_speaker_enable(true);
#endif
    } else {
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        analog1603_aud_codec_speaker_enable(false);
        osDelay(1);
#endif
        analog_aud_enable_dac(0);
        pmu_codec_hppa_enable(0);
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, false);
    }
}

void analog_aud_codec_dac_enable(bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        if (GET_BITFIELD(codec_dac_dev, PMU_CODEC_DAC)) {
            analog1809_aud_codec_dac_enable(en);
        }
    }
    if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV & GET_BITFIELD(codec_dac_dev, MCU_CODEC_DAC)) {
        analog1603_aud_codec_dac_enable(en);
    }
}

static void analog1603_aud_codec_open(void)
{
    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, true);
}

void analog_aud_codec_open(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        if (GET_BITFIELD(codec_dac_dev, PMU_CODEC_DAC)) {
            analog1809_aud_codec_open();
        }
    }
    if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV & GET_BITFIELD(codec_dac_dev, MCU_CODEC_DAC)) {
        analog1603_aud_codec_open();
    }
}

static void analog1603_aud_codec_close(void)
{
    analog1603_aud_codec_speaker_enable(false);
    osDelay(1);
    analog1603_aud_codec_dac_enable(false);

    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, false);
}

void analog_aud_codec_close(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        if (GET_BITFIELD(codec_dac_dev, PMU_CODEC_DAC)) {
            analog1809_aud_codec_close();
        }
    }
    if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV & GET_BITFIELD(codec_dac_dev, MCU_CODEC_DAC)) {
        analog1603_aud_codec_close();
    }
}

static void analog1603_aud_codec_mute(void)
{
#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_codec_tx_pa_gain_sel(0);
#endif

    ana_spk_muted = true;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_mute(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        if (GET_BITFIELD(codec_dac_dev, PMU_CODEC_DAC)) {
            analog1809_aud_codec_mute();
        }
    }
    analog1603_aud_codec_mute();
}

static void analog1603_aud_codec_nomute(void)
{
    ana_spk_muted = false;
    analog_aud_codec_config_speaker();

#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_aud_set_dac_gain(dac_gain);
#endif
}

void analog_aud_codec_nomute(void)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        if (GET_BITFIELD(codec_dac_dev, PMU_CODEC_DAC)) {
            analog1809_aud_codec_nomute();
        }
    }
    analog1603_aud_codec_nomute();
}

int analog_debug_config_audio_output(bool diff)
{
    return 0;
}

int analog1603_debug_config_codec(uint16_t mv)
{
#ifdef ANC_PROD_TEST
    int ret;

    ret = pmu_debug_config_codec(mv);
    if (ret) {
        return ret;
    }

    vcodec_mv = mv;
#endif
    return 0;
}

int analog_debug_config_codec(uint16_t mv)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_debug_config_codec(mv);
    }
    return 0;
}

int analog_debug_config_low_power_adc(bool enable)
{
    return 0;
}

void analog_debug_config_anc_calib_mode(bool enable)
{
    anc_calib_mode = enable;
}

bool analog_debug_get_anc_calib_mode(void)
{
    return anc_calib_mode;
}

int analog_debug_config_vad_mic(bool enable)
{
    return 0;
}

#if 1//def RPC_DEBUG_EN
#define RPC_DBG TRACE
#else
#define RPC_DBG(...) do{}while(0)
#endif

static void analog1603_rpcsvr_freq_pll_config(uint32_t freq, uint32_t div)
{
    RPC_DBG(1, "%s: freq=%d, div=%d", __func__, freq, div);
    analog_aud_freq_pll_config(freq, div);
}

void analog_rpcsvr_freq_pll_config(uint32_t freq, uint32_t div)
{
    analog1603_rpcsvr_freq_pll_config(freq, div);
}

static void analog1603_rpcsvr_osc_clk_enable(bool en)
{
    RPC_DBG(1, "%s: en=%d", __func__, en);
    rpcsvr_osc_enable = en;
    analog_aud_osc_clk_enable(en);
}

void analog_rpcsvr_osc_clk_enable(bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_rpcsvr_osc_clk_enable(en);
    }
    analog1603_rpcsvr_osc_clk_enable(en);
}

static void analog1603_rpcsvr_codec_open(bool en)
{
    RPC_DBG(1, "%s: en=%d", __func__, en);
    analog_aud_vcodec_enable(ANA_CODEC_USER_VAD, en);
}

void analog_rpcsvr_codec_open(bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_rpcsvr_codec_open(en);
    }
    analog1603_rpcsvr_codec_open(en);
}

void analog_rpcsvr_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
        analog1809_rpcsvr_codec_adc_enable(input_path, ch_map, en);
    }
}

void analog_rpcsvr_vad_enable(enum AUD_VAD_TYPE_T type, bool en)
{
    RPC_DBG(1, "%s: type=%d, en=%d", __func__, type, en);
}

void analog_rpcsvr_vad_adc_enable(bool en)
{
    RPC_DBG(1, "%s: en=%d", __func__, en);
}

/**only 1806 need open pmu vcm**/
void analog_vcm_pmu_on(bool enable)
{
    vcm_pmu_on = enable;
}

void analog_vcm_enable(enum ANA_VCM_USER_T user)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (vcm_map == 0) {
        analog_read(ANA_REG_F6, &val);
        val |= REG_CODEC_EN_VCM;
        analog_write(ANA_REG_F6, val);
        if (vcm_pmu_on) {
            pmu_codec_vcm_enable(true);
        }
    }
    vcm_map |= user;
    int_unlock(lock);
}

void analog_vcm_disable(enum ANA_VCM_USER_T user)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (vcm_map == user) {
        analog_read(ANA_REG_F6, &val);
        val &= ~REG_CODEC_EN_VCM;
        analog_write(ANA_REG_F6, val);
        if (vcm_pmu_on) {
            pmu_codec_vcm_enable(false);
        }
    }
    vcm_map &= ~user;
    int_unlock(lock);
}

void analog_codec_lp_vcm(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_F6, &val);
    if (en) {
        val |= REG_CODEC_LP_VCM;
    } else {
        val &= ~REG_CODEC_LP_VCM;
    }
    analog_write(ANA_REG_F6, val);

    if (vcm_pmu_on) {
        pmu_codec_lp_vcm(en);
    }
}

void analog_codec_vcm_enable_lpf(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_F6, &val);
    if (en) {
        val |= REG_CODEC_VCM_EN_LPF;
    } else {
        val &= ~REG_CODEC_VCM_EN_LPF;
    }
    analog_write(ANA_REG_F6, val);

    if (vcm_pmu_on) {
        pmu_codec_vcm_enable_lpf(en);
    }
}

void analog_codec_set_vcm_low_vcm_lp(uint16_t v)
{
    uint16_t val;

    analog_read(ANA_REG_F9, &val);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM_LP, v);
    analog_write(ANA_REG_F9, val);

    if (vcm_pmu_on) {
        pmu_codec_set_vcm_low_vcm_lp(v);
    }
}

void analog_codec_set_vcm_low_vcm_lpf(uint16_t v)
{
    uint16_t val;

    analog_read(ANA_REG_F9, &val);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM_LPF, v);
    analog_write(ANA_REG_F9, val);

    if (vcm_pmu_on) {
        pmu_codec_set_vcm_low_vcm_lpf(v);
    }
}

void analog_codec_set_vcm_low_vcm(uint16_t v)
{
    uint16_t val;

    analog_read(ANA_REG_F9, &val);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM, v);
    analog_write(ANA_REG_F9, val);

    if (vcm_pmu_on) {
        pmu_codec_set_vcm_low_vcm(v);
    }
}

// DAC dc calib use comp
void analog_aud_dre_dc_sel(bool en)
{
    uint16_t val;
    analog_read(ANA_REG_D4, &val);
    if (en) {
        val |= ANADRE_DC_SEL;
    } else {
        val &= ~ANADRE_DC_SEL;
    }
    analog_write(ANA_REG_D4, val);
}

void analog_aud_dc_calib_set_dre_ana_dc(uint32_t offs, uint16_t dc_offs_l, uint16_t dc_offs_r)
{
    uint16_t val;
    uint16_t chip_val = 0;

    if(codec_dac_dev & MCU_CODEC_DAC_R) {
        chip_val = dc_offs_r;
    } else if(codec_dac_dev & MCU_CODEC_DAC_L) {
        chip_val = dc_offs_l;
    }
    analog_read(ANA_REG_D0 + (uint16_t)offs, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL_1, chip_val);
    analog_write(ANA_REG_D0 + (uint16_t)offs, val);
}

static void analog_aud_codec_set_dre_gain(uint16_t gain_val_l)
{
    uint16_t val;

    analog_read(ANA_REG_6D, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DRE_GAIN_L, gain_val_l);
    analog_write(ANA_REG_6D, val);
}

static void analog_aud_codec_comp_enable(bool on)
{
    uint16_t val;

    analog_codec_enable_vcm_buffer(on);

    // dac analog enable default mute
    analog1603_aud_codec_dac_enable(on);
    if (on) {
        analog_aud_enable_dac(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
        osDelay(200);
        analog_aud_enable_dac_pa(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    } else {
        analog_aud_enable_dac_pa(0);
        analog_aud_enable_dac(0);
    }

    // dre_gain_sel ctrl
    analog_read(ANA_REG_75, &val);
    if(on) {
        val &= ~DRE_GAIN_SEL_L;
    } else {
        val |= DRE_GAIN_SEL_L;
    }
    analog_write(ANA_REG_75, val);

    // comparator enable
    analog_read(ANA_REG_F4, &val);
    if(on) {
        val |= REG_CODEC_COMP_EN;
    } else {
        val &= ~REG_CODEC_COMP_EN;
    }
    analog_write(ANA_REG_F4, val);

    // comparator clock enable
    analog_read(ANA_REG_F4, &val);
    if(on) {
        val |= REG_DC_CLK_SEL_OSC;
    } else {
        val &= ~REG_DC_CLK_SEL_OSC;
    }
    analog_write(ANA_REG_F4, val);

    // comparator speed
    analog_read(ANA_REG_F5, &val);
    val = SET_BITFIELD(val, REG_CODEC_COMP_CNT, CODEC_COMP_DIV);
    analog_write(ANA_REG_F5, val);

    // comparator IBSLE
    analog_read(ANA_REG_F4, &val);
    val = SET_BITFIELD(val, REG_CODEC_COMP_IBSEL, 0x5);
    analog_write(ANA_REG_F4, val);
}

static void analog_aud_codec_get_comp_vop(bool *comp_vop)
{
    uint16_t val;
    uint16_t pos = 0, neg = 0;
    for (int i = COMP_VAL_GET_SUM_CNT; i > 0; i--) {
        analog_read(ANA_REG_F4, &val);
        val |= REG_CODEC_COMP_GEN_PULSE;
        analog_write(ANA_REG_F4, val);
        osDelay(1);

        analog_read(ANA_REG_F4, &val);
        *comp_vop = !!(val & REG_CODEC_COMP_VOP);
        if (*comp_vop) {
            pos++;
        } else {
            neg++;
        }
    }

    if (pos > neg) {
        *comp_vop = true;
    } else {
        *comp_vop = false;
    }
}

void analog_aud_dac_dc_do_calib(uint32_t *ana_dc_offset_l, uint32_t *ana_dc_offset_r)
{
#define ANA_DC_SIGN_BIT             (13)
#define ANA_DC_REG_MSB_POS          (12)
#define ANA_DC_REG_BIT_MAX_WIGHT    (8) //BIT 12: x256(1<<8)

    uint16_t regval_l = 0, regval_r = 0;
    int16_t wight_val_l = 0;
    bool sign, comp_vop;
    int16_t wight_a = -511; //(a, b) = (-0x1ff, 0x1ff)
    int16_t wight_b = 511;

    // set ana dc wight = 511
    regval_l = analog_aud_dac_dc_diff_to_val(wight_b);
    analog1603_aud_dc_calib_set_value(regval_l, 0);
    osDelay(1);

    // get first sign
    analog_aud_codec_get_comp_vop(&comp_vop);
    sign = comp_vop;
    for (int i = ANA_DC_REG_BIT_MAX_WIGHT; i + 1 >= 0; i--) {
        wight_val_l = (wight_a + wight_b)/2;
        TRACE(0,"wight_val_l = %d", wight_val_l);
        regval_l = analog_aud_dac_dc_diff_to_val(wight_val_l);
        analog1603_aud_dc_calib_set_value(regval_l, regval_r);
        osDelay(1);

        analog_aud_codec_get_comp_vop(&comp_vop);
        TRACE(0,"regval_l = 0x%x, regval_r = 0x%x comp_vop = %d", regval_l, regval_r, comp_vop);
        if (comp_vop == sign) {
            wight_b = wight_val_l;
        } else {
            wight_a = wight_val_l;
        }

        TRACE(0,"wight_a = %d, wight_b = %d ", wight_a, wight_b);
    }

    analog1603_aud_dc_calib_set_value(0, 0);
    regval_l = analog_aud_dac_dc_diff_to_val((wight_a + wight_b)/2);
    *ana_dc_offset_l = regval_l;
}

int analog_aud_comparator_calib_dac_dc(void)
{
    uint32_t ana_dc_l = 0, ana_dc_r = 0;
    uint32_t num;
    int i;
    int ret = 0;
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg, *comp_calib_cfg;

    if (GET_BITFIELD(codec_dac_dev, MCU_CODEC_DAC) == 0) {
        TRACE(1, "%s: codec_dac_dev=%x should not calib 1603 dac", __func__, codec_dac_dev);
        goto _exit0;
    }

    // clk enable
    analog1603_aud_osc_clk_enable(true);

    // dac mute
    analog_aud_codec_comp_enable(true);
    osDelay(200);

    // enbale ana dc composition
    analog1603_aud_dc_calib_enable(true);

    // disable ana dre sel
    analog_aud_dre_dc_sel(false);

    hal_codec_dac_dre_init_calib_cfg();
    comp_calib_cfg = hal_codec_dac_dre_get_calib_cfg(&num);
    if (comp_calib_cfg == NULL || num == 0) {
        TRACE(1, "%s: invalid calib_cfg=%x or num=%d", __func__, (int)comp_calib_cfg, num);
        ret = -1;
        goto _exit0;
    }

    for (i = 0, cfg = comp_calib_cfg; i < num; i++, cfg++) {
        // set ana gain step
        analog_aud_codec_set_dre_gain(cfg->ana_gain);
        TRACE(0,"cfg->ana_gain = 0x%x", cfg->ana_gain);
        osDelay(1);

        // do calib
        analog_aud_dac_dc_do_calib(&ana_dc_l, &ana_dc_r);
        if (codec_dac_dev & MCU_CODEC_DAC_L) {
            cfg->ana_dc_l = (uint16_t)ana_dc_l;
            cfg->valid |= DAC_DC_SET_VALID_CH(1<<0);
        }
        if (codec_dac_dev & MCU_CODEC_DAC_R) {
            cfg->ana_dc_r = (uint16_t)ana_dc_l;
            cfg->valid |= DAC_DC_SET_VALID_CH(1<<1);
        }
        analog1603_aud_dc_calib_set_value(0, 0);
        TRACE(0,"ana_dc_l=0x%x, ana_dc_r=0x%x", ana_dc_l, ana_dc_r);

        // cfg->valid |= DAC_DC_SET_VALID_CH(1<<1);
    }

    // ana_dc_calib_check_valid();
    for (i = 0, cfg = comp_calib_cfg; i < num; i++, cfg++) {
        float gain_comp_l;

        if (i == 0) {
            gain_comp_l = 1;
        } else {
            gain_comp_l = 12;
        }
        if (codec_dac_dev & MCU_CODEC_DAC_L) {
            cfg->dc_l = 0;
            cfg->gain_l = gain_comp_l;
            cfg->valid |= DAC_DC_SET_VALID_CH(1<<2);
        }
        if (codec_dac_dev & MCU_CODEC_DAC_R) {
            cfg->dc_r = 0;
            cfg->gain_r = gain_comp_l;
            cfg->valid |= DAC_DC_SET_VALID_CH(1<<3);
        }
        // cfg->valid |= DAC_DC_SET_VALID_CH(1<<3);
        TRACE(1, "AUTO CALIB DAC GAIN: ana_gain=%d, L*1000=%d ",
            cfg->ana_gain, (int)(gain_comp_l*1000));
    }
    // ana_dc_calib_get_dig_gain();

_exit0:
    // enable ana dre sel
    analog_aud_dre_dc_sel(true);

    // close dac
    analog_aud_codec_comp_enable(false);

    // clk disable
    analog1603_aud_osc_clk_enable(false);
    return ret;
}
