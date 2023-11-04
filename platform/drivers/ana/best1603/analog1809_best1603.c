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
#include "analog.h"
#include "analog1809_best1603.h"
#include "reg_analog1809_best1603.h"
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

#if !defined(SLEEP_PD_CURRENT_TEST) || defined(RC48M_ENABLE)
#define VCM_ON
#endif

// Not using 1uF
#define VCM_CAP_100NF

#define DAC_DC_CALIB_BIT_WIDTH              14

#define DAC_DC_ADJUST_STEP                  90

#define DEFAULT_ANC_FF_ADC_GAIN_DB          9
#define DEFAULT_ANC_FB_ADC_GAIN_DB          9
#define DEFAULT_ANC_TT_ADC_GAIN_DB          9
#define DEFAULT_VOICE_ADC_GAIN_DB           12

#ifndef ANALOG_ADC_A_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH0) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH0))
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH0) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH0))
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#elif defined(ANC_APP) && (defined(AUDIO_ANC_TT_HW)||defined(PSAP_APP)) && ((ANC_TT_MIC_CH_L == AUD_CHANNEL_MAP_CH0) || (ANC_TT_MIC_CH_R == AUD_CHANNEL_MAP_CH0))
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_ANC_TT_ADC_GAIN_DB
#else
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_B_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH1) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH1))
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH1) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH1))
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#elif defined(ANC_APP) && (defined(AUDIO_ANC_TT_HW)||defined(PSAP_APP)) && ((ANC_TT_MIC_CH_L == AUD_CHANNEL_MAP_CH1) || (ANC_TT_MIC_CH_R == AUD_CHANNEL_MAP_CH1))
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_ANC_TT_ADC_GAIN_DB
#else
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_C_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH2) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH2))
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH2) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH2))
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#elif defined(ANC_APP) && (defined(AUDIO_ANC_TT_HW)||defined(PSAP_APP))  && ((ANC_TT_MIC_CH_L == AUD_CHANNEL_MAP_CH2) || (ANC_TT_MIC_CH_R == AUD_CHANNEL_MAP_CH2))
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_ANC_TT_ADC_GAIN_DB
#else
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef LINEIN_ADC_GAIN_DB
#define LINEIN_ADC_GAIN_DB                  0
#endif

#ifndef CFG_HW_AUD_MICKEY_DEV
#define CFG_HW_AUD_MICKEY_DEV               (AUD_VMIC_MAP_VMIC1)
#endif

#ifndef ANC_VMIC_CFG
#define ANC_VMIC_CFG                        (AUD_VMIC_MAP_VMIC1)
#endif

// #define CODEC_TX_EN_ANA_LDAC                RESERVED_ANA_15_0(1 << 2)
#define CODEC_ANA_DAC_IF_CLOCK_GATE              RESERVED_ANA_15_0(1 << 5)
#define CODEC_ANA_ADC_IF_CLOCK_GATE              RESERVED_ANA_15_0(1 << 6)

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
static uint8_t ana_ci2s_idx = 0;

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static bool anc_calib_mode;

static bool vtoi_enable = false;

static enum ANA_CODEC_USER_T POSSIBLY_UNUSED adc_map[MAX_ANA1809_MIC_CH_NUM];
static enum ANA_CODEC_USER_T vmic_map[MAX_VMIC_CH_NUM];
static enum ANA_CODEC_USER_T codec_common_map;
static enum ANA_CODEC_USER_T adda_common_map;
static enum ANA_CODEC_USER_T vcodec_map;
static enum ANA_CODEC_USER_T if_clk_common_map;

static enum ANA1809_VCM_USER_T vcm_map;

#ifdef ANC_APP
#ifndef DYN_ADC_GAIN
#define DYN_ADC_GAIN
#endif
#if defined(ANC_FF_MIC_CH_L)
#if defined(ANC_TT_MIC_CH_L)
static int8_t anc_tt_gain_db_l;
#endif
#endif
#if defined(ANC_FF_MIC_CH_R)
#if defined(ANC_TT_MIC_CH_R)
static int8_t anc_tt_gain_db_r;
#endif
#endif
#endif

#if defined (VOICE_DETECTOR_SENS_EN) || defined (VOICE_DETECTOR_EN)
#error "The best1603 sensor don`t have VAD or CODEC!"
#endif

static bool rpcsvr_osc_enable = false;
#ifdef SAR_ADC_ENABLE
static enum ANA_CODEC_USER_T sar_adc_map = 0;
#endif

static bool rc_osc_en;
static bool codec_osc_en;

static const int8_t adc_db[] = { -9, -6, -3, 0, 3, 6, 9, 12, };

static const int8_t tgt_adc_db[MAX_ANA1809_MIC_CH_NUM] = {
    ANALOG_ADC_A_GAIN_DB, ANALOG_ADC_B_GAIN_DB,
    ANALOG_ADC_C_GAIN_DB,
};

#ifdef DYN_ADC_GAIN
static int8_t dyn_adc_gain_db[MAX_ANA1809_MIC_CH_NUM];
STATIC_ASSERT(sizeof(dyn_adc_gain_db) == sizeof(tgt_adc_db), "dyn_adc_gain_db should have the same type and size as tgt_adc_db");
#endif

extern struct CODEC_DAC_DRE_CFG_T dac_dre_cfg;
void hal_codec_dac_dre_get_max_ini_ana_gain(uint8_t *p_ini_gain);

void analog1809_pu_osc(int enable)
{
    uint16_t val;
    uint32_t lock;

    lock = int_lock();
    rc_osc_en = !!enable;
    if (!codec_osc_en) {
        pmu_ana_read(ANA_REG_10D, &val);
        if (enable) {
            val |= REG_PU_OSC;
        } else {
            val &= ~REG_PU_OSC;
        }
        pmu_ana_write(ANA_REG_10D, val);
    }
    int_unlock(lock);
}

void analog1809_aud_osc_clk_enable(bool enable)
{
    uint16_t val;
    uint32_t lock;
    uint16_t bfv;

    lock = int_lock();
    codec_osc_en = enable;
    if (enable) {
        pmu_ana_read(ANA_REG_10D, &val);
        val |= REG_PU_OSC | REG_CRYSTAL_SEL;
        pmu_ana_write(ANA_REG_10D, val);
#if 0
        pmu_ana_read(ANA_REG_75, &val);
        val |= CFG_TX_CLK_INV;
        pmu_ana_write(ANA_REG_75, val);
#endif
    } else {
#if 0
        pmu_ana_read(ANA_REG_75, &val);
        val &= ~CFG_TX_CLK_INV;
        pmu_ana_write(ANA_REG_75, val);
#endif
        pmu_ana_read(ANA_REG_10D, &val);
        if (rc_osc_en) {
            bfv = REG_CRYSTAL_SEL;
        } else {
            bfv = REG_PU_OSC | REG_CRYSTAL_SEL;
        }
        val &= ~bfv;
        pmu_ana_write(ANA_REG_10D, val);
    }
    int_unlock(lock);
}

static void analog_aud_enable_if_clk_common(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    bool set = false;
    uint16_t val;

    lock = int_lock();
    if (en) {
        if (if_clk_common_map == 0) {
            set = true;
        }
        if_clk_common_map |= user;
    } else {
        if_clk_common_map &= ~user;
        if (if_clk_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        pmu_ana_read(ANA_REG_12A, &val);
        if (if_clk_common_map) {
            val |= CODEC_ANA_ADC_IF_CLOCK_GATE | CODEC_ANA_DAC_IF_CLOCK_GATE;
        } else {
            val &= ~(CODEC_ANA_ADC_IF_CLOCK_GATE | CODEC_ANA_DAC_IF_CLOCK_GATE);
        }
        pmu_ana_write(ANA_REG_12A, val);
    }
}

#ifdef RC48M_ENABLE
void analog1809_rc48m_enable(void)
{
}
#endif

static uint32_t analog1809_aud_pmu_dac_dev_convert(uint32_t dac)
{
    uint32_t dac_dev = analog_codec_get_dac_dev();
    uint32_t pmu_dac_dev = 0;
    switch (dac_dev) {
        case MCU_CODEC_DAC_L:
            break;
        case (MCU_CODEC_DAC_L | PMU_CODEC_DAC_R):
            if (dac & AUD_CHANNEL_MAP_CH1) {
                pmu_dac_dev |= AUD_CHANNEL_MAP_CH1;
            }
            break;
        case (MCU_CODEC_DAC_L | PMU_CODEC_DAC_L):
            if (dac & AUD_CHANNEL_MAP_CH1) {
                pmu_dac_dev |= AUD_CHANNEL_MAP_CH0;
            }
            break;
        case (MCU_CODEC_DAC_R | PMU_CODEC_DAC_L):
            if (dac & AUD_CHANNEL_MAP_CH0) {
                pmu_dac_dev |= AUD_CHANNEL_MAP_CH0;
            }
            break;
        case (PMU_CODEC_DAC_L | PMU_CODEC_DAC_R):
            if (dac & AUD_CHANNEL_MAP_CH0) {
                pmu_dac_dev |= AUD_CHANNEL_MAP_CH0;
            }
            if (dac & AUD_CHANNEL_MAP_CH1) {
                pmu_dac_dev |= AUD_CHANNEL_MAP_CH1;
            }
            break;
    }
    TRACE(0, "dac_dev(0x%x) convert pmu_dac_dev(0x%x)", dac_dev, pmu_dac_dev);
    return pmu_dac_dev;
}

static void analog1809_aud_enable_dac_with_classab(uint32_t dac, bool switch_pa)
{
    uint16_t val_10c;
    uint16_t val_10e;
    uint16_t val_10f;
    uint16_t val_112;
    uint16_t val_115;
    uint16_t val_116;
    uint16_t val_11e;
    uint16_t val_14f;

    pmu_ana_read(ANA_REG_10C, &val_10c);
    pmu_ana_read(ANA_REG_10F, &val_10f);
    pmu_ana_read(ANA_REG_10E, &val_10e);
    pmu_ana_read(ANA_REG_112, &val_112);
    pmu_ana_read(ANA_REG_115, &val_115);
    pmu_ana_read(ANA_REG_116, &val_116);
    pmu_ana_read(ANA_REG_11E, &val_11e);
    // pmu_ana_read(ANA_REG_12A, &val_12a);
    pmu_ana_read(ANA_REG_14F, &val_14f);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        analog_aud_enable_if_clk_common(ANA_CODEC_USER_DAC, true);
        val_10e |= REG_CODEC_TX_EAR_ENBIAS | REG_CODEC_TX_EAR_LPBIAS;
        pmu_ana_write(ANA_REG_10E, val_10e);

        osDelay(1);
        val_116 |= CFG_TX_TREE_EN;
        pmu_ana_write(ANA_REG_116, val_116);
        osDelay(1);

        if (dac & AUD_CHANNEL_MAP_CH0) {
            val_115 |= REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_LCLK;
            if (vcodec_mv > 1350) {
                val_10e |= REG_CODEC_TX_EAR_DR_EN_L;
            }
            if (!switch_pa) {
                val_115 |= REG_CODEC_TX_EN_LDAC_ANA;
            }
            val_10f = SET_BITFIELD(val_10f, REG_OCP_SEL_L, 3);
        }
        if (dac & AUD_CHANNEL_MAP_CH1) {
            val_115 |= REG_CODEC_TX_EN_RCLK;
            val_11e |= REG_CODEC_TX_EN_EARPA_R;
            if (vcodec_mv > 1350) {
                val_14f |= REG_CODEC_TX_EAR_DR_EN_R;
            }
            if (!switch_pa) {
                val_115 |= REG_CODEC_TX_EN_RDAC_ANA;
            }
            val_10c = SET_BITFIELD(val_10c, REG_OCP_SEL_R, 3);
        }

        pmu_ana_write(ANA_REG_10C, val_10c);
        pmu_ana_write(ANA_REG_10F, val_10f);
        pmu_ana_write(ANA_REG_11E, val_11e);
        pmu_ana_write(ANA_REG_115, val_115);
        pmu_ana_write(ANA_REG_10E, val_10e);
        pmu_ana_write(ANA_REG_14F, val_14f);
        val_112 |= REG_CODEC_TX_EN_DACLDO;
        pmu_ana_write(ANA_REG_112, val_112);
        val_116 |= REG_CODEC_TX_EN_LPPA;
        pmu_ana_write(ANA_REG_116, val_116);
        osDelay(1);
        if (dac & AUD_CHANNEL_MAP_CH0) {
            val_115 |= REG_CODEC_TX_EN_S1PA_L;
            pmu_ana_write(ANA_REG_115, val_115);
        }
        if (dac & AUD_CHANNEL_MAP_CH1) {
            val_14f |= REG_CODEC_TX_EN_S1PA_R;
            pmu_ana_write(ANA_REG_14F, val_14f);
        }
        // Ensure 1ms delay before enabling dac_pa
        osDelay(1);
    } else {
        // Ensure 1ms delay after disabling dac_pa
        osDelay(1);
        val_115 &= ~REG_CODEC_TX_EN_S1PA_L;
        val_14f &= ~REG_CODEC_TX_EN_S1PA_R;
        pmu_ana_write(ANA_REG_115, val_115);
        pmu_ana_write(ANA_REG_14F, val_14f);
        osDelay(1);
        val_115 &= ~(REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_LCLK);
        if (!switch_pa) {
            val_115 &= ~REG_CODEC_TX_EN_LDAC_ANA;
        }
        val_115 &= ~REG_CODEC_TX_EN_RCLK;
        val_11e &= ~REG_CODEC_TX_EN_EARPA_R;
        if (!switch_pa) {
            val_115 &= ~REG_CODEC_TX_EN_RDAC_ANA;
        }
        pmu_ana_write(ANA_REG_11E, val_11e);
        pmu_ana_write(ANA_REG_115, val_115);

        val_112 &= ~REG_CODEC_TX_EN_DACLDO;
        pmu_ana_write(ANA_REG_112, val_112);
        val_116 &= ~REG_CODEC_TX_EN_LPPA;
        pmu_ana_write(ANA_REG_116, val_116);
        osDelay(1);

        val_116 &= ~CFG_TX_TREE_EN;
        pmu_ana_write(ANA_REG_116, val_116);
        osDelay(1);

        val_14f &= ~REG_CODEC_TX_EAR_DR_EN_R;
        pmu_ana_write(ANA_REG_14F, val_14f);

        val_10e &= ~(REG_CODEC_TX_EAR_ENBIAS | REG_CODEC_TX_EAR_LPBIAS | REG_CODEC_TX_EAR_DR_EN_L);
        pmu_ana_write(ANA_REG_10E, val_10e);

        analog_aud_enable_if_clk_common(ANA_CODEC_USER_DAC, false);
        val_10f = SET_BITFIELD(val_10f, REG_OCP_SEL_L, 0);
        val_10c = SET_BITFIELD(val_10c, REG_OCP_SEL_R, 0);
        pmu_ana_write(ANA_REG_10F, val_10f);
        pmu_ana_write(ANA_REG_10C, val_10c);
    }
}

static void analog1809_aud_enable_dac_pa_classab(uint32_t dac)
{
    uint16_t val_115;
    uint16_t val_14f;

    pmu_ana_read(ANA_REG_115, &val_115);
    if (dac & (AUD_CHANNEL_MAP_CH0)) {
        val_115 |= REG_CODEC_TX_EN_S4PA_L;
    } else {
        val_115 &= ~REG_CODEC_TX_EN_S4PA_L;
    }
    pmu_ana_write(ANA_REG_115, val_115);

    pmu_ana_read(ANA_REG_14F, &val_14f);
    if (dac & (AUD_CHANNEL_MAP_CH1)) {
        val_14f |= REG_CODEC_TX_EN_S4PA_R;
    } else {
        val_14f &= ~REG_CODEC_TX_EN_S4PA_R;
    }
    pmu_ana_write(ANA_REG_14F, val_14f);
}

static void analog1809_aud_enable_dac(uint32_t dac)
{
    analog1809_aud_enable_dac_with_classab(dac, false);
}

static void analog1809_aud_enable_dac_pa_internal(uint32_t dac)
{
    analog1809_aud_enable_dac_pa_classab(dac);
}

static void analog1809_aud_enable_dac_pa(uint32_t dac)
{
    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        analog1809_aud_enable_dac_pa_internal(dac);

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_clear();
#endif
    } else {
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_set();
#endif

        analog1809_aud_enable_dac_pa_internal(dac);
    }
}

#ifdef SAR_ADC_ENABLE
static void analog1809_aud_enable_sar_adc(enum ANA_CODEC_USER_T user, bool enable)
{
    uint16_t val;
    bool set = false;
    enum ANA_CODEC_USER_T old_map = sar_adc_map;

    if (enable) {
        sar_adc_map |= user;
        if (old_map == 0) {
            set = true;
        }
    } else {
        sar_adc_map &= ~user;
        if ((old_map != 0) && (sar_adc_map == 0)) {
            set = true;
        }
    }
    if (set) {
        if (enable) {
            // enable clock
            val = REG_SAR_CLK_EN | REG_SAR_EN_PREAMP | REG_SAR_DELAY_BIT(1);
            pmu_ana_write(ANA_REG_8B, val);

            val = REG_SAR_CALI_LSB_SCRN
#if !defined(RC48M_ENABLE)
                | REG_CLK_SAR_MUX
#endif
                | REG_SAR_CALI_CNT(2)
                | REG_SAR_INIT_CALI_BIT(9);
            pmu_ana_write(ANA_REG_86, val);

            val = REG_SAR_PU_START_DLY_CNT(0x32) | REG_SAR_PU_VREF_IN;
            pmu_ana_write(ANA_REG_8E, val);
#if 1
#define SAR_ADC_CLK_DIV (512*3-2)
            val = REG_SAR_CLOSE_ON_DET | REG_SAR_TO_SIGN | SEL_SARCLK_DIV2;
            pmu_ana_write(ANA_REG_CF, val);

            //sample rate=24M/SAR_CLK_OUT_DIV = 16K
            val = REG_SAR_CLK_OUT_DIV(SAR_ADC_CLK_DIV);
            pmu_ana_write(ANA_REG_8D, val);
#endif
            val = REG_SAR_RESULT_SEL
                | REG_SAR_CLK_INT_DIV(0x16)
                | REG_SAR_OFFSET_CALI_CNT(2);
            pmu_ana_write(ANA_REG_8A, val);
            // sar adc on
            val |= REG_SAR_ADC_ON;
            pmu_ana_write(ANA_REG_8A, val);
        } else {
            pmu_ana_read(ANA_REG_8A, &val);
            val &= ~REG_SAR_ADC_ON;
            pmu_ana_write(ANA_REG_8A, val);

            pmu_ana_read(ANA_REG_8B, &val);
            val &= ~REG_SAR_CLK_EN;
            pmu_ana_write(ANA_REG_8B, val);
        }
        rc_codec_enable_bias_lp(enable);
    }
}
#endif

static void POSSIBLY_UNUSED analog1809_codec_enable_vcm_buffer(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_52, &val);
    if (en) {
        val |= REG_CODEC_EN_VCMBUFFER;
    } else {
        val &= ~REG_CODEC_EN_VCMBUFFER;
    }
    pmu_ana_write(ANA_REG_52, val);
}

static void analog1809_codec_enable_bias(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_50, &val);
    if (en) {
        val |= REG_CODEC_EN_BIAS;
    } else {
        val &= ~REG_CODEC_EN_BIAS;
    }
    pmu_ana_write(ANA_REG_50, val);
}

static void analog1809_codec_enable_capsensor_osc(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_6D, &val);
    if (en) {
        val |= (REG_CLK_CAPSENSOR_GATE_EN | REG_CLK_32K_CAPSENSOR_GATE_EN | SOFT_RESETN_CAPSENSOR | SOFT_RESETN_CAPSENSOR_REG);
        val = SET_BITFIELD(val, REG_CFG_CAPSENSOR_DIV, 0x4E);
    } else {
        val &= ~(REG_CLK_CAPSENSOR_GATE_EN | REG_CLK_32K_CAPSENSOR_GATE_EN | SOFT_RESETN_CAPSENSOR | SOFT_RESETN_CAPSENSOR_REG);
        val = SET_BITFIELD(val, REG_CFG_CAPSENSOR_DIV, 0x0);
    }
    pmu_ana_write(ANA_REG_6D, val);
}

static void analog1809_codec_enable_pu_rc(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_55, &val);
    if (en) {
        val |= (REG_PU_RC_DR | REG_PU_RC);
        val = SET_BITFIELD(val, REG_RCOSC_COUNTER_SEL, 0x2);
    } else {
        val &= ~(REG_PU_RC_DR | REG_PU_RC);
        val = SET_BITFIELD(val, REG_RCOSC_COUNTER_SEL, 0x0);
    }
    pmu_ana_write(ANA_REG_55, val);
}

static void analog1809_codec_enable_bias_lp(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_50, &val);
    if (en) {
        val |= REG_CODEC_EN_BIAS_LP;
    } else {
        val &= ~REG_CODEC_EN_BIAS_LP;
    }
    pmu_ana_write(ANA_REG_50, val);
}

void analog1809_codec_vtoi_enable(void)
{
    uint16_t val_06c;
    uint16_t val_150;
    uint16_t adc_user = 0;
    bool vtoi_enable_old;

    //check adc ch enable
    for (int i = 0; i < MAX_ANA1809_MIC_CH_NUM; i++) {
        if (adc_map[i]) {
            adc_user |= AUD_CHANNEL_MAP_CH0 << i;
        }
    }

    vtoi_enable_old = vtoi_enable;

    if (adc_user) {
        vtoi_enable = true;
    } else {
        vtoi_enable = false;
    }

    if (vtoi_enable != vtoi_enable_old) {
        // vtoi enable
        pmu_ana_read(ANA_REG_6C, &val_06c);
        if (vtoi_enable) {
            val_06c |= REG_CODEC_EN_VTOI_INT;
        } else {
            val_06c &= ~REG_CODEC_EN_VTOI_INT;
        }
        pmu_ana_write(ANA_REG_6C, val_06c);

        // rx regulator
        pmu_ana_read(ANA_REG_150, &val_150);
        if (vtoi_enable) {
            val_150 |= REG_PU_RX_REGULATOR;
        } else {
            val_150 &= ~REG_PU_RX_REGULATOR;
        }
        pmu_ana_write(ANA_REG_150, val_150);

        // adc interface clock gate
        analog_aud_enable_if_clk_common(ANA_CODEC_USER_ADC, vtoi_enable);
        analog1809_codec_ci2s_adc_en(vtoi_enable);
    }
}

static void analog1809_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    int i;
    uint16_t reg;
    uint16_t val;
    enum ANA_CODEC_USER_T old_map;
    bool set;
#if !defined(CODEC_TX_PEAK_DETECT)
    bool global_update = false;
#endif

    ANALOG_DEBUG_TRACE(3,"[%s] user=%d ch_map=0x%x", __func__, user, ch_map);

    /* enable normal analog adc */
    for (i = 0; i < MAX_ANA1809_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            set = false;
            if (en) {
                if (adc_map[i] == 0) {
                    set = true;
                }
                adc_map[i] |= user;
            } else {
                old_map = adc_map[i];
                adc_map[i] &= ~user;
                if (old_map != 0 && adc_map[i] == 0) {
                    set = true;
                }
            }
            if (set) {
#if !defined(CODEC_TX_PEAK_DETECT)
                if (!global_update) {
                    global_update = true;
                    if (en) {
                        analog1809_codec_enable_vcm_buffer(true);
                    }
                }
#endif
                reg = ANA_REG_01 + 0x10 * i;
                pmu_ana_read(reg, &val);
                if (adc_map[i]) {
                    val |= REG_CODEC_EN_ADCA;
                } else {
                    val &= ~REG_CODEC_EN_ADCA;
                }
                pmu_ana_write(reg, val);

                reg = ANA_REG_0F + 0x10 * i;
                pmu_ana_read(reg, &val);
                if (adc_map[i]) {
                    val = SET_BITFIELD(val, REG_ADCA_INPUT_CM_MODE, 4);
                    val |= REG_CODEC_CLK_SW_IN_ADCA_INT;
                } else {
                    val = SET_BITFIELD(val, REG_ADCA_INPUT_CM_MODE, 0);
                    val &= ~REG_CODEC_CLK_SW_IN_ADCA_INT;
                }
                pmu_ana_write(reg, val);
            }
        }
    }

#if !defined(CODEC_TX_PEAK_DETECT)
    if (global_update && !en) {
        for (i = 0; i < MAX_ANA1809_MIC_CH_NUM; i++) {
            if (adc_map[i]) {
                break;
            }
        }
        if (i >= MAX_ANA1809_MIC_CH_NUM) {
            analog1809_codec_enable_vcm_buffer(false);
        }
    }
#endif

    analog1809_codec_vtoi_enable();
}

static uint32_t db_to_adc_gain(int db)
{
    int i;
    uint8_t cnt;
    const int8_t *list;

    list = adc_db;
    cnt = ARRAY_SIZE(adc_db);

    for (i = 0; i < cnt - 1; i++) {
        if (db < list[i + 1]) {
            break;
        }
    }

    if (i == cnt - 1) {
        return i;
    }
    else if (db * 2 < list[i] + list[i + 1]) {
        return i;
    } else {
        return i + 1;
    }
}

static int8_t get_chan_adc_gain(uint32_t i)
{
    int8_t gain;

#ifdef DYN_ADC_GAIN
    gain = dyn_adc_gain_db[i];
#else
    gain = tgt_adc_db[i];
#endif

    return gain;
}

static void analog1809_aud_set_adc_gain(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map)
{
    int i;
    int gain;
    uint16_t gain_val;
    uint16_t reg;
    uint16_t val;

    for (i = 0; i < MAX_ANA1809_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (0) {
#ifdef ANC_APP
#ifdef ANC_FF_ENABLED
            } else if ((ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R) & (AUD_CHANNEL_MAP_CH0 << i)) {
                gain = get_chan_adc_gain(i);
#endif
#ifdef ANC_FB_ENABLED
            } else if ((ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R) & (AUD_CHANNEL_MAP_CH0 << i)) {
                gain = get_chan_adc_gain(i);
#endif
#endif
            } else if (input_path == AUD_INPUT_PATH_LINEIN) {
                gain = LINEIN_ADC_GAIN_DB;
            } else {
                gain = get_chan_adc_gain(i);
            }
            gain_val = db_to_adc_gain(gain);
            reg = ANA_REG_01 + 0x10 * i;
            pmu_ana_read(reg, &val);
            val = SET_BITFIELD(val, REG_CODEC_ADCA_GAIN_BIT, gain_val);
            pmu_ana_write(reg, val);
        }
    }
}

void analog1809_aud_set_adc_gain_direct(enum AUD_CHANNEL_MAP_T ch_map, int gain)
{
    int i;
    uint16_t gain_val;
    uint16_t reg;
    uint16_t val;

    for (i = 0; i < MAX_ANA1809_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            gain_val = db_to_adc_gain(gain);
            reg = ANA_REG_01 + 0x10 * i;
            pmu_ana_read(reg, &val);
            val = SET_BITFIELD(val, REG_CODEC_ADCA_GAIN_BIT, gain_val);
            pmu_ana_write(reg, val);
        }
    }
}

#ifdef ANC_APP
static void _update_anc_dyn_adc_gain(bool set, enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r)
{
    enum ANC_TYPE_T single_type;
    enum AUD_CHANNEL_MAP_T ch_map;
    uint32_t l, r;
    int8_t org_l, adj_l;
    int8_t org_r, adj_r;
    int8_t gain_l, gain_r;

    if (set) {
        // qdb to db
        gain_l = DEFAULT_ANC_FF_ADC_GAIN_DB + offset_l / 4;
        gain_r = DEFAULT_ANC_FF_ADC_GAIN_DB + offset_r / 4;
    } else {
        gain_l = gain_r = 0;
    }

    while (type) {
        l = get_msb_pos(type);
        single_type = (1 << l);
        type &= ~single_type;

        ch_map = 0;
        l = r = 32;
        if (0) {
#if defined(ANC_FF_MIC_CH_L) && defined(ANC_FF_MIC_CH_R)
        } else if (single_type == ANC_FEEDFORWARD) {
            if (ANC_FF_MIC_CH_L) {
                ch_map |= ANC_FF_MIC_CH_L;
                l = get_msb_pos(ANC_FF_MIC_CH_L);
            }
            if (ANC_FF_MIC_CH_R) {
                ch_map |= ANC_FF_MIC_CH_R;
                r = get_msb_pos(ANC_FF_MIC_CH_R);
            }
#endif
#if defined(ANC_FB_MIC_CH_L) && defined(ANC_FB_MIC_CH_R)
        } else if (single_type == ANC_FEEDBACK) {
            if (ANC_FB_MIC_CH_L) {
                ch_map |= ANC_FB_MIC_CH_L;
                l = get_msb_pos(ANC_FB_MIC_CH_L);
            }
            if (ANC_FB_MIC_CH_R) {
                ch_map |= ANC_FB_MIC_CH_R;
                r = get_msb_pos(ANC_FB_MIC_CH_R);
            }
#endif
#if defined(ANC_TT_MIC_CH_L) && defined(ANC_TT_MIC_CH_R)
        } else if (single_type == ANC_TALKTHRU) {
            if (ANC_TT_MIC_CH_L) {
                ch_map |= ANC_TT_MIC_CH_L;
                l = get_msb_pos(ANC_TT_MIC_CH_L);
            }
            if (ANC_TT_MIC_CH_R) {
                ch_map |= ANC_TT_MIC_CH_R;
                r = get_msb_pos(ANC_TT_MIC_CH_R);
            }
#if defined(ANC_FF_MIC_CH_L) && defined(ANC_FF_MIC_CH_R)
            if (set) {
                anc_tt_gain_db_l = gain_l;
                anc_tt_gain_db_r = gain_r;
            }
            if (adda_common_map & ANA_CODEC_USER_ANC_FF) {
                if (ANC_TT_MIC_CH_L & (ANC_FF_MIC_CH_L)) {
                    ch_map &= ~ANC_TT_MIC_CH_L;
                    l = 32;
                }
                if (ANC_TT_MIC_CH_R & (ANC_FF_MIC_CH_R)) {
                    ch_map &= ~ANC_TT_MIC_CH_R;
                    r = 32;
                }
            }
#endif
#endif
        } else {
            continue;
        }

        if (set) {
            ANALOG_INFO_TRACE(0, "ana: set anc adc gain: type=%d", single_type);
        } else {
            if (l >= MAX_ANA1809_MIC_CH_NUM && r >= MAX_ANA1809_MIC_CH_NUM) {
                continue;
            }
            if (l < MAX_ANA1809_MIC_CH_NUM) {
                gain_l = tgt_adc_db[l];
#if defined(ANC_TT_MIC_CH_L)
#if defined(ANC_FF_MIC_CH_L)
                if ((single_type & ANC_FEEDFORWARD) && (adda_common_map & ANA_CODEC_USER_ANC_TT) &&
                        (ANC_TT_MIC_CH_L & (ANC_FF_MIC_CH_L))) {
                    gain_l = anc_tt_gain_db_l;
                }
#endif
#endif
            }
            if (r < MAX_ANA1809_MIC_CH_NUM) {
                gain_r = tgt_adc_db[r];
#if defined(ANC_TT_MIC_CH_R)
#if defined(ANC_FF_MIC_CH_R)
                if ((single_type & ANC_FEEDFORWARD) && (adda_common_map & ANA_CODEC_USER_ANC_TT) &&
                        (ANC_TT_MIC_CH_R & (ANC_FF_MIC_CH_R))) {
                    gain_r = anc_tt_gain_db_r;
                }
#endif
#endif
            }
            ANALOG_INFO_TRACE(0, "ana: clear anc adc gain: type=%d", single_type);
        }

        if ((l >= MAX_ANA1809_MIC_CH_NUM || dyn_adc_gain_db[l] == gain_l) &&
                (r >= MAX_ANA1809_MIC_CH_NUM || dyn_adc_gain_db[r] == gain_r)) {
            continue;
        }
        if (l < MAX_ANA1809_MIC_CH_NUM) {
            dyn_adc_gain_db[l] = gain_l;
        }
        if (r < MAX_ANA1809_MIC_CH_NUM) {
            dyn_adc_gain_db[r] = gain_r;
        }
        ANALOG_INFO_TRACE(0, "ana: update anc adc gain: type=%d gain=%d/%d", single_type, gain_l, gain_r);

        org_l = adj_l = 0;
        if (l < MAX_ANA1809_MIC_CH_NUM) {
            if (dyn_adc_gain_db[l] != tgt_adc_db[l]) {
                org_l = adc_db[db_to_adc_gain(tgt_adc_db[l])];
                adj_l = adc_db[db_to_adc_gain(dyn_adc_gain_db[l])];
            }
        }

        org_r = adj_r = 0;
        if (r < MAX_ANA1809_MIC_CH_NUM) {
            if (dyn_adc_gain_db[r] != tgt_adc_db[r]) {
                org_r = adc_db[db_to_adc_gain(tgt_adc_db[r])];
                adj_r = adc_db[db_to_adc_gain(dyn_adc_gain_db[r])];
            }
        }

        hal_codec_apply_anc_adc_gain_offset(single_type, (org_l - adj_l), (org_r - adj_r));
        analog1809_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
    }
}

void analog1809_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r)
{
    _update_anc_dyn_adc_gain(true, type, offset_l, offset_r);
}

void analog1809_aud_restore_anc_dyn_adc_gain(enum ANC_TYPE_T type)
{
    _update_anc_dyn_adc_gain(false, type, 0, 0);
}
#endif

#ifdef DYN_ADC_GAIN
void analog1809_aud_apply_dyn_adc_gain(enum AUD_CHANNEL_MAP_T ch_map, int16_t gain)
{
    enum AUD_CHANNEL_MAP_T map;
    int i;

#ifdef ANC_APP
#ifdef ANC_FF_ENABLED
    ch_map &= ~(ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R);
#endif
#ifdef ANC_FB_ENABLED
    ch_map &= ~(ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R);
#endif
#if defined(ANC_TT_MIC_CH_L)
    ch_map &= ~ANC_TT_MIC_CH_L;
#endif
#if defined(ANC_TT_MIC_CH_R)
    ch_map &= ~ANC_TT_MIC_CH_R;
#endif
#endif

    if (ch_map) {
        map = ch_map;

        while (map) {
            i = get_msb_pos(map);
            map &= ~(1 << i);
            if (i < MAX_ANA1809_MIC_CH_NUM) {
                dyn_adc_gain_db[i] = gain;
            }
        }

        ANALOG_INFO_TRACE(2,"ana: apply adc gain: ch_map=0x%X gain=%d", ch_map, gain);

        analog1809_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
    }
}
#endif

void analog1809_aud_set_dac_gain(int32_t v)
{
}

uint32_t analog1809_codec_get_dac_gain(void)
{
    return 0;
}

uint32_t analog1809_codec_dac_gain_to_db(int32_t gain)
{
    return 0;
}

int32_t analog1809_codec_dac_max_attn_db(void)
{
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

bool analog1809_aud_dc_calib_valid(void)
{
    return false;
}

uint16_t analog1809_aud_dac_dc_diff_to_val(int32_t diff)
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

uint16_t analog1809_aud_dc_calib_val_to_efuse(uint16_t val)
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

int16_t analog1809_aud_dac_dc_get_step(void)
{
    return DAC_DC_ADJUST_STEP;
}

void analog1809_aud_save_dc_calib(uint16_t val)
{
}

void analog1809_aud_dc_calib_set_value(uint16_t dc_l, uint16_t dc_r)
{
    uint16_t val;
    uint16_t dac_dev = analog_codec_get_dac_dev();
    uint32_t pmu_dc_val_l = dc_l, pmu_dc_val_r = dc_r;
    if(dac_dev == (MCU_CODEC_DAC_L | PMU_CODEC_DAC_L))
    {
        pmu_dc_val_l = dc_r;
        pmu_dc_val_r = dc_l;
    }
    pmu_ana_read(ANA_REG_112, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL_TMP, pmu_dc_val_l);
    pmu_ana_write(ANA_REG_112, val);

    pmu_ana_read(ANA_REG_113, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITR_TMP, pmu_dc_val_r);
    pmu_ana_write(ANA_REG_113, val);
}

void analog1809_aud_dc_calib_get_cur_value(uint16_t *dc_l, uint16_t *dc_r)
{
    uint16_t val;

    if (dc_l) {
        pmu_ana_read(ANA_REG_112, &val);
        *dc_l = GET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL_TMP);
    }
    if (dc_r) {
        pmu_ana_read(ANA_REG_113, &val);
        *dc_r = GET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITR_TMP);
    }
}

void analog1809_aud_dc_calib_get_fixed_comp_value(uint32_t dre_gain, int32_t *dc_l, int32_t *dc_r)
{
    int gain_idx = 0;
    int dc_val[][2] = {
        {-100, 0},
        {-100, 0},
    };

    if (dre_gain > 8) {
        gain_idx = 1;
    }
    if (dc_l) {
        *dc_l = (int32_t)dc_val[gain_idx][0];
    }
    if (dc_r) {
        *dc_r = (int32_t)dc_val[gain_idx][1];
    }
}

bool analog1809_aud_dc_calib_get_large_ana_dc_value(int16_t *ana_dc, int cur_dig_dc, int tgt_dig_dc,
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

void analog1809_aud_dc_calib_enable(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_112, &val);
    if (en) {
        val |= REG_CODEC_TX_EAR_OFFEN_L;
    } else {
        val &= ~REG_CODEC_TX_EAR_OFFEN_L;
    }
    pmu_ana_write(ANA_REG_112, val);

    pmu_ana_read(ANA_REG_150, &val);
    if (en) {
        val |= REG_CODEC_TX_EAR_OFFEN_R;
    } else {
        val &= ~REG_CODEC_TX_EAR_OFFEN_R;
    }
    pmu_ana_write(ANA_REG_150, val);
}

static void POSSIBLY_UNUSED analog1809_aud_dac_dc_backup_regs(bool save)
{
    uint32_t i;
    uint16_t regaddr[] = {
        ANA_REG_01,
        ANA_REG_11,
        ANA_REG_06,
        ANA_REG_16,
        ANA_REG_08,
        ANA_REG_18};

    static uint16_t regval[ARRAY_SIZE(regaddr)] = {0};
    static bool regs_saved = false;

    if (save) {
        if (!regs_saved) {
            for(i = 0; i < ARRAY_SIZE(regaddr); i++) {
                pmu_ana_read(regaddr[i], &regval[i]);
            }
            regs_saved = true;
        }
    } else {
        if (regs_saved) {
            for(i = 0; i < ARRAY_SIZE(regaddr); i++) {
                pmu_ana_write(regaddr[i], regval[i]);
            }
            regs_saved = false;
        }
    }
}

#define analog1809_aud_dac_dc_save_regs()    analog1809_aud_dac_dc_backup_regs(true)
#define analog1809_aud_dac_dc_restore_regs() analog1809_aud_dac_dc_backup_regs(false)

void analog1809_aud_dac_dc_auto_calib_enable(void)
{
    uint16_t val;
    uint32_t dac;

    analog1809_aud_dac_dc_save_regs();

    dac = analog1809_aud_pmu_dac_dev_convert(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    analog1809_aud_enable_dac(dac);
    analog1809_aud_enable_dac_pa(dac);
    analog1809_aud_set_adc_gain_direct((AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1), -3);
    analog1809_aud_enable_adc(ANA_CODEC_USER_ADC, (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1), false);
    analog1809_aud_enable_adc(ANA_CODEC_USER_ADC, (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1), true);

    // Force ADC precharge = 1
    pmu_ana_read(ANA_REG_01, &val);
    val |= REG_CODEC_ADCA_PRE_CHARGE;
    pmu_ana_write(ANA_REG_01, val);

    pmu_ana_read(ANA_REG_11, &val);
    val |= REG_CODEC_ADCB_PRE_CHARGE;
    pmu_ana_write(ANA_REG_11, val);

    // precharge DR = 1
    pmu_ana_read(ANA_REG_01, &val);
    val |= CFG_PRE_CHARGE_ADCA_DR;
    pmu_ana_write(ANA_REG_01, val);

    pmu_ana_read(ANA_REG_11, &val);
    val |= CFG_PRE_CHARGE_ADCB_DR;
    pmu_ana_write(ANA_REG_11, val);

    osDelay(200);

    // precharge DR = 0
    pmu_ana_read(ANA_REG_01, &val);
    val &= ~CFG_PRE_CHARGE_ADCA_DR;
    pmu_ana_write(ANA_REG_01, val);

    pmu_ana_read(ANA_REG_11, &val);
    val &= ~CFG_PRE_CHARGE_ADCB_DR;
    pmu_ana_write(ANA_REG_11, val);

    // Force ADC precharge = 0
    pmu_ana_read(ANA_REG_01, &val);
    val &= ~REG_CODEC_ADCA_PRE_CHARGE;
    pmu_ana_write(ANA_REG_01, val);

    pmu_ana_read(ANA_REG_11, &val);
    val &= ~REG_CODEC_ADCB_PRE_CHARGE;
    pmu_ana_write(ANA_REG_11, val);
}

void analog1809_aud_dac_dc_auto_calib_disable(void)
{
    analog1809_aud_dac_dc_auto_calib_set_mode(ANA_DAC_DC_CALIB_MODE_NORMAL);

    analog1809_aud_enable_adc(ANA_CODEC_USER_ADC, (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1), false);
    analog1809_aud_enable_dac_pa(0);
    analog1809_aud_enable_dac(0);

    analog1809_aud_dac_dc_restore_regs();
}

void analog1809_aud_dac_dc_auto_calib_set_mode(enum ANA_DAC_DC_CALIB_MODE_T mode)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_06, &val);
    if (mode == ANA_DAC_DC_CALIB_MODE_ADC_ONLY) {
        val = SET_BITFIELD(val, REG_ADCA_OFFSET_CAL, 0x06);
    } else if (mode == ANA_DAC_DC_CALIB_MODE_DAC_TO_ADC) {
        val = SET_BITFIELD(val, REG_ADCA_OFFSET_CAL, 0x12);
    } else {
        val = SET_BITFIELD(val, REG_ADCA_OFFSET_CAL, 0x12);
    }
    pmu_ana_write(ANA_REG_06, val);

    pmu_ana_read(ANA_REG_01, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADCA_CH_SEL_INT, 2);
    pmu_ana_write(ANA_REG_01, val);

    pmu_ana_read(ANA_REG_16, &val);
    if (mode == ANA_DAC_DC_CALIB_MODE_ADC_ONLY) {
        val = SET_BITFIELD(val, REG_ADCB_OFFSET_CAL, 0x06);
    } else if (mode == ANA_DAC_DC_CALIB_MODE_DAC_TO_ADC) {
        val = SET_BITFIELD(val, REG_ADCB_OFFSET_CAL, 0x12);
    } else {
        val = SET_BITFIELD(val, REG_ADCB_OFFSET_CAL, 0x12);
    }
    pmu_ana_write(ANA_REG_16, val);

    pmu_ana_read(ANA_REG_11, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADCB_CHANNEL_SEL, 2);
    pmu_ana_write(ANA_REG_11, val);

    osDelay(10);
}

/* hal codec 1809 dre config*/
void analog1809_aud_dac_dc_offset_enable(int32_t dc_l, int32_t dc_r)
{
    uint16_t val;
    if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
        return ;
    }
    pmu_ana_read(ANA_REG_30C, &val);
    val &= ~(CODEC_DAC_DC_UPDATE_CH0 | CODEC_DAC_DC_UPDATE_CH1);
    pmu_ana_write(ANA_REG_30C, val);

    pmu_ana_read(ANA_REG_30A, &val);
    val = SET_BITFIELD(val, CODEC_DAC_DC_CH0_15_0, dc_l & 0xffff);
    pmu_ana_write(ANA_REG_30A, val);

    pmu_ana_read(ANA_REG_30B, &val);
    val = SET_BITFIELD(val, CODEC_DAC_DC_CH1_15_0, dc_r & 0xffff);
    pmu_ana_write(ANA_REG_30B, val);

    pmu_ana_read(ANA_REG_30C, &val);
    val = SET_BITFIELD(val, CODEC_DAC_DC_CH0_18_16, (dc_l >> 16) & 0xffff);
    val = SET_BITFIELD(val, CODEC_DAC_DC_CH1_18_16, (dc_r >> 16) & 0xffff);
    val |= CODEC_DAC_DC_UPDATE_CH0 | CODEC_DAC_DC_UPDATE_CH1;
    pmu_ana_write(ANA_REG_30C, val);
    TRACE(0, "DC_L = %d, 15_0:0x%x, 18_16:0x%x", dc_l, dc_l & 0xffff, (dc_l >> 16) & 0x7000);
    TRACE(0, "DC_R = %d, 15_0:0x%x, 18_16:0x%x", dc_r, dc_r & 0xffff, (dc_r >> 16) & 0x7000);
}

void analog1809_aud_set_dac_ana_gain(uint8_t ini_gain, uint8_t gain_offset)
{
    uint16_t val;
    uint8_t max_gain = analog1809_aud_get_max_dre_gain();
    if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
        return ;
    }
    pmu_ana_read(ANA_REG_30D, &val);
    val = SET_BITFIELD(val, CODEC_DRE_INI_ANA_GAIN_CH0, ini_gain);
    pmu_ana_write(ANA_REG_30D, val);
    pmu_ana_read(ANA_REG_30E, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_OFFSET_CH0, gain_offset);
    pmu_ana_write(ANA_REG_30E, val);
    pmu_ana_read(ANA_REG_30F, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_TOP_CH0, max_gain);
    pmu_ana_write(ANA_REG_30F, val);

    pmu_ana_read(ANA_REG_314, &val);
    val = SET_BITFIELD(val, CODEC_DRE_INI_ANA_GAIN_CH1, ini_gain);
    pmu_ana_write(ANA_REG_314, val);
    pmu_ana_read(ANA_REG_315, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_OFFSET_CH1, gain_offset);
    pmu_ana_write(ANA_REG_315, val);
    pmu_ana_read(ANA_REG_316, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_TOP_CH1, max_gain);
    pmu_ana_write(ANA_REG_316, val);

    pmu_ana_read(ANA_REG_35E, &val);
    val = SET_BITFIELD(val, CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0, 0);
    val = SET_BITFIELD(val, CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1, 0);
    pmu_ana_write(ANA_REG_35E, val);
    TRACE(0, "[%s] max_gain=0x%x, ini_gain=0x%x, gain_offset=0x%x", __func__, max_gain, ini_gain, gain_offset);
}

void analog1809_aud_dac_dre_init(void)
{
    uint8_t max_gain, ini_gain, dre_offset;
    uint16_t val;
    ini_gain = 0;
    max_gain = analog1809_aud_get_max_dre_gain();
    if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
        return ;
    }
    if (max_gain >= 0xF) {
        dre_offset = max_gain - 0xF;
    } else {
        dre_offset = 32 - (0xF - max_gain);
    }
    pmu_ana_read(ANA_REG_30D, &val);
    val = SET_BITFIELD(val, CODEC_DRE_INI_ANA_GAIN_CH0, ini_gain);
    pmu_ana_write(ANA_REG_30D, val);
    pmu_ana_read(ANA_REG_30E, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_OFFSET_CH0, dre_offset);
    pmu_ana_write(ANA_REG_30E, val);
    pmu_ana_read(ANA_REG_30F, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_TOP_CH0, max_gain);
    pmu_ana_write(ANA_REG_30F, val);

    pmu_ana_read(ANA_REG_314, &val);
    val = SET_BITFIELD(val, CODEC_DRE_INI_ANA_GAIN_CH1, ini_gain);
    pmu_ana_write(ANA_REG_314, val);
    pmu_ana_read(ANA_REG_315, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_OFFSET_CH1, dre_offset);
    pmu_ana_write(ANA_REG_315, val);
    pmu_ana_read(ANA_REG_316, &val);
    val = SET_BITFIELD(val, CODEC_DRE_GAIN_TOP_CH1, max_gain);
    pmu_ana_write(ANA_REG_316, val);

    pmu_ana_read(ANA_REG_35E, &val);
    val = SET_BITFIELD(val, CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0, 0);
    val = SET_BITFIELD(val, CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1, 0);
    pmu_ana_write(ANA_REG_35E, val);
}

void analog1809_aud_get_dre_cfg(uint32_t *ini_gain, uint32_t *gain_offs)
{
    uint16_t val;
    if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
        return ;
    }
    pmu_ana_read(ANA_REG_314, &val);
    *ini_gain = GET_BITFIELD(val, CODEC_DRE_INI_ANA_GAIN_CH0);

    pmu_ana_read(ANA_REG_30E, &val);
    *gain_offs = GET_BITFIELD(val, CODEC_DRE_GAIN_OFFSET_CH0);
}

#ifdef DAC_DRE_ENABLE
#define DRE_MAX_INI_ANA_GAIN_FIRST
bool static analog1809_aud_dac_dre_setup_calib_param(struct CODEC_DAC_DRE_CFG_T *cfg,
                                                int32_t dac_dc_l, int32_t dac_dc_r)
{
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cal;
    uint32_t num = 0, i;
    uint16_t val;
    uint32_t dac_dev = analog_codec_get_dac_dev();

    cal = hal_codec_dac_dre_get_calib_cfg(&num);

    cfg->step_mode    = cal->step_mode;
    cfg->ini_ana_gain = cal->ini_ana_gain;
    cfg->gain_offset  = cal->gain_offset;
    cfg->top_gain     = cal->top_gain;

    for (i = 0; i < num; i++, cal++) {
        if (dac_dev == (MCU_CODEC_DAC_L | PMU_CODEC_DAC_L)) {
            if (cal->valid & 0x2) {
                int32_t dc_r = (int32_t)(cal->dc_r - dac_dc_r);
                uint8_t dc_step = cal->ini_ana_gain;
                int32_t gain_r = (int32_t)(cal->gain_r * (float)2048);
                uint32_t dc_ch_shift[]={0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14};

                val = CODEC_DAC_DRE_DC0_CH0_15_0(dc_r & 0xffff);
                pmu_ana_write(ANA_REG_33B + dc_step, val);
                pmu_ana_read(ANA_REG_369 + dc_step / 8, &val);
                val |= CODEC_DAC_DRE_DC0_CH0_17_16((dc_r >> 16) & 0xffff) << dc_ch_shift[dc_step] ;
                pmu_ana_write(ANA_REG_369 + dc_step / 8, val);

                val = SET_BITFIELD(val, CODEC_DAC_DRE_GAIN_STEP0_CH0, gain_r);
                pmu_ana_write(ANA_REG_31B + dc_step, val);
            }
        } else {
            if (cal->valid & 0x1) {
                int32_t dc_l = (int32_t)(cal->dc_l - dac_dc_l);
                uint8_t dc_step = cal->ini_ana_gain;
                int32_t gain_l = (int32_t)(cal->gain_l * (float)2048);
                uint32_t dc_ch_shift[]={0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14};

                val = CODEC_DAC_DRE_DC0_CH0_15_0(dc_l & 0xffff);
                pmu_ana_write(ANA_REG_33B + dc_step, val);
                pmu_ana_read(ANA_REG_369 + dc_step / 8, &val);
                val |= CODEC_DAC_DRE_DC0_CH0_17_16((dc_l >> 16) & 0xffff) << dc_ch_shift[dc_step] ;
                pmu_ana_write(ANA_REG_369 + dc_step / 8, val);

                val = SET_BITFIELD(val, CODEC_DAC_DRE_GAIN_STEP0_CH0, gain_l);
                pmu_ana_write(ANA_REG_31B + dc_step, val);
            }
            if (cal->valid & 0x2) {
                int32_t dc_r = (int32_t)((cal->dc_r) - dac_dc_r);
                uint8_t dc_step = cal->ini_ana_gain;
                int32_t gain_r = (int32_t)(cal->gain_r * (float)2048);
                uint32_t dc_ch_shift[]={0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14};

                val = CODEC_DAC_DRE_DC0_CH1_15_0(dc_r & 0xffff);
                pmu_ana_write(ANA_REG_34B + dc_step, val);
                pmu_ana_read(ANA_REG_36B + dc_step / 8, &val);
                val |= CODEC_DAC_DRE_DC0_CH1_17_16((dc_r >> 16) & 0xffff) << dc_ch_shift[dc_step];
                pmu_ana_write(ANA_REG_36B + dc_step / 8, val);

                val = SET_BITFIELD(val, CODEC_DAC_DRE_GAIN_STEP0_CH1, gain_r);
                pmu_ana_write(ANA_REG_32B + dc_step, val);
            }
        }
    }
    return true;
}

void analog1809_aud_dac_dre_enable(int32_t dac_dc_l, int32_t dac_dc_r, enum AUD_CHANNEL_MAP_T codec_dac_ch_map)
{
    uint16_t val = 0;
    struct CODEC_DAC_DRE_CFG_T *cfg = &dac_dre_cfg;
    uint8_t ini_ana_gain = cfg->ini_ana_gain;
    if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
        return ;
    }
    analog1809_aud_dac_dre_setup_calib_param(cfg, dac_dc_l, dac_dc_r);
#ifdef DRE_MAX_INI_ANA_GAIN_FIRST
    hal_codec_dac_dre_get_max_ini_ana_gain(&ini_ana_gain);
#endif
    // enable DAC CH0 DRE
    if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH0) {
        pmu_ana_read(ANA_REG_30D, &val);
        val |= CODEC_DRE_DELAY_MODE_CH0;
        val = SET_BITFIELD(val, CODEC_DRE_STEP_MODE_CH0, cfg->step_mode);
        val = SET_BITFIELD(val, CODEC_DRE_INI_ANA_GAIN_CH0, ini_ana_gain);
        val = SET_BITFIELD(val, CODEC_DRE_THD_DB_OFFSET_CH0, cfg->thd_db_offset);
        if (cfg->thd_db_offset_sign) {
            val |= CODEC_DRE_THD_DB_OFFSET_SIGN_CH0;
        }
        pmu_ana_write(ANA_REG_30D, val);

        pmu_ana_read(ANA_REG_30E, &val);
        val = SET_BITFIELD(val, CODEC_DRE_WINDOW_CH0_20_16, (cfg->dre_win >> 16) & 0xffff);
        val = SET_BITFIELD(val, CODEC_DRE_GAIN_OFFSET_CH0, cfg->gain_offset);
        pmu_ana_write(ANA_REG_30E, val);

        pmu_ana_read(ANA_REG_30F, &val);
        val = SET_BITFIELD(val, CODEC_DRE_DB_HIGH_CH0, cfg->db_high);
        val = SET_BITFIELD(val, CODEC_DRE_DB_LOW_CH0, cfg->db_low);
        val = SET_BITFIELD(val, CODEC_DRE_GAIN_TOP_CH0, cfg->top_gain);
        pmu_ana_write(ANA_REG_30F, val);

        pmu_ana_read(ANA_REG_310, &val);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_HL_CH0, cfg->dre_delay_hl);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_LH_CH0, cfg->dre_delay_lh);
        pmu_ana_write(ANA_REG_310, val);

        pmu_ana_read(ANA_REG_311, &val);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_DC_LH_CH0, cfg->delay_dc_lh);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_DC_HL_CH0, cfg->delay_dc_hl);
        pmu_ana_write(ANA_REG_311, val);

        pmu_ana_read(ANA_REG_312, &val);
        val = SET_BITFIELD(val, CODEC_DRE_AMP_HIGH_CH0, cfg->amp_high);
        pmu_ana_write(ANA_REG_312, val);

        pmu_ana_read(ANA_REG_313, &val);
        val = SET_BITFIELD(val, CODEC_DRE_WINDOW_CH0_15_0, cfg->dre_win & 0xffff);
        pmu_ana_write(ANA_REG_313, val);

        pmu_ana_read(ANA_REG_30D, &val);
        val |= CODEC_DRE_ENABLE_CH0;
        pmu_ana_write(ANA_REG_30D, val);
    }
    // enable DAC CH1 DRE
    if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
        pmu_ana_read(ANA_REG_314, &val);
        val |= CODEC_DRE_DELAY_MODE_CH1;
        val = SET_BITFIELD(val, CODEC_DRE_STEP_MODE_CH1, cfg->step_mode);
        val = SET_BITFIELD(val, CODEC_DRE_INI_ANA_GAIN_CH1, ini_ana_gain);
        val = SET_BITFIELD(val, CODEC_DRE_THD_DB_OFFSET_CH1, cfg->thd_db_offset);
        if (cfg->thd_db_offset_sign) {
            val |= CODEC_DRE_THD_DB_OFFSET_SIGN_CH1;
        }
        pmu_ana_write(ANA_REG_314, val);

        pmu_ana_read(ANA_REG_315, &val);
        val = SET_BITFIELD(val, CODEC_DRE_WINDOW_CH1_20_16, (cfg->dre_win >> 16) & 0xffff);
        val = SET_BITFIELD(val, CODEC_DRE_GAIN_OFFSET_CH1, cfg->gain_offset);
        pmu_ana_write(ANA_REG_315, val);

        pmu_ana_read(ANA_REG_316, &val);
        val = SET_BITFIELD(val, CODEC_DRE_DB_HIGH_CH1, cfg->db_high);
        val = SET_BITFIELD(val, CODEC_DRE_DB_LOW_CH1, cfg->db_low);
        val = SET_BITFIELD(val, CODEC_DRE_GAIN_TOP_CH1, cfg->top_gain);
        pmu_ana_write(ANA_REG_316, val);

        pmu_ana_read(ANA_REG_317, &val);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_HL_CH1, cfg->dre_delay_hl);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_LH_CH1, cfg->dre_delay_lh);
        pmu_ana_write(ANA_REG_317, val);

        pmu_ana_read(ANA_REG_318, &val);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_DC_LH_CH1, cfg->delay_dc_lh);
        val = SET_BITFIELD(val, CODEC_DRE_DELAY_DC_HL_CH1, cfg->delay_dc_hl);
        pmu_ana_write(ANA_REG_318, val);

        pmu_ana_read(ANA_REG_319, &val);
        val = SET_BITFIELD(val, CODEC_DRE_AMP_HIGH_CH1, cfg->amp_high);
        pmu_ana_write(ANA_REG_319, val);

        pmu_ana_read(ANA_REG_31A, &val);
        val = SET_BITFIELD(val, CODEC_DRE_WINDOW_CH1_15_0, cfg->dre_win & 0xffff);
        pmu_ana_write(ANA_REG_31A, val);

        pmu_ana_read(ANA_REG_314, &val);
        val |= CODEC_DRE_ENABLE_CH1;
        pmu_ana_write(ANA_REG_314, val);
    }
}

void analog1809_aud_dac_dre_disable(void)
{
    uint16_t val = 0;
    if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
        return ;
    }
    // disable DAC CH0 DRE
    pmu_ana_read(ANA_REG_30D, &val);
    val &= ~CODEC_DRE_ENABLE_CH0;
    pmu_ana_write(ANA_REG_30D, val);
    // disable DAC CH1 DRE
    pmu_ana_read(ANA_REG_314, &val);
    val &= ~CODEC_DRE_ENABLE_CH1;
    pmu_ana_write(ANA_REG_314, val);
}
#endif

static void analog1809_aud_vcodec_enable(enum ANA_CODEC_USER_T user, bool en)
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

static void analog1809_aud_enable_common_internal(enum ANA_CODEC_USER_T user, bool en)
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
            analog1809_codec_set_vcm_low_vcm(0);
            analog1809_vcm_enable(ANA1809_VCM_USER_CODEC);

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
            pmu_ana_write(ANA_REG_6B, val_6b);
            osDelay(10);
#endif
#endif // !(VCM_ON && RTOS)
            vcm_en_lpf = true;
        }
        if (vcm_en) {
            analog1809_vcm_enable(ANA1809_VCM_USER_CODEC);
        } else {
            analog1809_vcm_disable(ANA1809_VCM_USER_CODEC);
        }
        analog1809_codec_vcm_enable_lpf(vcm_en_lpf);
    }
}

static void analog1809_aud_enable_codec_common(enum ANA_CODEC_USER_T user, bool en)
{
#ifndef VCM_ON
    analog1809_aud_enable_common_internal(user, en);
#endif
}

static void analog1809_aud_enable_adda_common(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t reg115_val;
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
        pmu_ana_read(ANA_REG_115, &reg115_val);
        if (adda_common_map) {
            reg115_val |= REG_BYPASS_CODEC_TX_REGULATOR;
            en_bias = true;
        } else {
            reg115_val &= ~REG_BYPASS_CODEC_TX_REGULATOR;
            en_bias = false;
        }
        analog1809_codec_enable_bias(en_bias);

#if defined(CODEC_TX_PEAK_DETECT)
        analog1809_codec_enable_vcm_buffer(!!adda_common_map);
#endif
        pmu_ana_write(ANA_REG_115, reg115_val);
    }
}

static void analog1809_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev, bool en)
{
    uint32_t lock;
    enum ANA_CODEC_USER_T old_map;
    bool set = false;
    bool global_update = false;
    int i;
    uint32_t pmu_map = 0;

    lock = int_lock();

    dev = pmu_codec_mic_bias_remap(dev);

    for (i = 0; i < MAX_VMIC_CH_NUM; i++) {
        if ((dev & (AUD_VMIC_MAP_VMIC1 << i)) == 0) {
            continue;
        }
        set = false;
        if (en) {
            if (vmic_map[i] == 0) {
                set = true;
            }
            vmic_map[i] |= user;
        } else {
            old_map = vmic_map[i];
            vmic_map[i] &= ~user;
            if (old_map != 0 && vmic_map[i] == 0) {
                set = true;
            }
        }

        if (set) {
            pmu_map |= (AUD_VMIC_MAP_VMIC1 << i);
            global_update = true;
        }
    }

    int_unlock(lock);

    if (global_update) {
        pmu_codec_mic_bias_noremap_enable(pmu_map, en);
#ifdef VOICE_DETECTOR_EN
        pmu_codec_mic_bias_noremap_lowpower_mode(pmu_map, en);
#endif
        if (pmu_map) {
            osDelay(1);
        }
    }
}

static void analog1809_codec_vtoi_config(void)
{
    uint16_t val;
    pmu_ana_read(ANA_REG_6C, &val);
    val = REG_CODEC_VTOI_VCS_SEL(0xc) | REG_CODEC_VTOI_IDAC_SEL(0xc) | REG_CODEC_VTOI_I_DAC2(4);
    pmu_ana_write(ANA_REG_6C, val);
}

static void analog1809_aud_adc_clk_sw_config(void)
{
    uint16_t i;
    uint16_t val;
    uint32_t offs;
    for (i = 0; i < MAX_ANA1809_MIC_CH_NUM; i++) {
        offs = 0x2 * i;   //adcA ~ adcC
        val = CFG_CLK_SW_ADCA_TIME(0x3C);
        pmu_ana_write(ANA_REG_372 + offs, val);
    }
    val = CFG_CLK_SW_PRD(0x40) | CFG_CLK_SW_EN;
    pmu_ana_write(ANA_REG_370, val);
}

static void analog1809_aud_adc_chan_init(void)
{
    uint16_t i;
    uint16_t val;
    uint32_t offs;

    for (i = 0; i < MAX_ANA1809_MIC_CH_NUM; i++) {
        offs = 0x10 * i;   //adcA ~ adcC

        val = REG_CODEC_ADCA_CH_SEL_INT(1);
        pmu_ana_write(ANA_REG_01 + offs, val);

        val = REG_CODEC_ADCA_CAP_BIT1(0xf) | REG_CODEC_ADCA_CAP_BIT2(0xf);
        pmu_ana_write(ANA_REG_02 + offs, val);

        if (vcodec_mv > 1350) {
            val = REG_CODEC_ADCA_CAP_BIT3(0xf) | REG_CODEC_ADCA_OP1_IBIT(0x1) \
                | REG_CODEC_ADCA_OP4_IBIT(0x1);
        } else {
            val = REG_CODEC_ADCA_CAP_BIT3(0xf) | REG_CODEC_ADCA_OP1_IBIT(0x1) \
                | REG_CODEC_ADCA_OP4_IBIT(0x0);
        }
        pmu_ana_write(ANA_REG_03 + offs, val);

        if (vcodec_mv > 1350) {
            val = REG_CODEC_ADCA_IBSEL_OFFSET(8) | REG_CODEC_ADCA_IBSEL_OP1(8) \
                | REG_CODEC_ADCA_IBSEL_OP2(4) | REG_CODEC_ADCA_IBSEL_OP3(8);
        } else {
            val = REG_CODEC_ADCA_IBSEL_OFFSET(8) | REG_CODEC_ADCA_IBSEL_OP1(8) \
                | REG_CODEC_ADCA_IBSEL_OP2(5) | REG_CODEC_ADCA_IBSEL_OP3(8);
        }
        pmu_ana_write(ANA_REG_04 + offs, val);

        if (vcodec_mv > 1350) {
            val = REG_CODEC_ADCA_IBSEL_OP4(8) | REG_CODEC_ADCA_IBSEL_REG(4) \
                | REG_CODEC_ADCA_IBSEL_VCOMP(4) | REG_CODEC_ADCA_IBSEL_VREF(0xa);
        } else {
            val = REG_CODEC_ADCA_IBSEL_OP4(9) | REG_CODEC_ADCA_IBSEL_REG(8) \
                | REG_CODEC_ADCA_IBSEL_VCOMP(0xa) | REG_CODEC_ADCA_IBSEL_VREF(0xb);
        }
        pmu_ana_write(ANA_REG_05 + offs, val);

        val = REG_CODEC_ADCA_IBSEL_DAC2(0xa);
        pmu_ana_write(ANA_REG_06 + offs, val);

        pmu_ana_read(ANA_REG_07 + offs, &val);
        val |= REG_ADCA_DVDD_SEL;
        pmu_ana_write(ANA_REG_07 + offs, val);

        if (vcodec_mv > 1350) {
            val = REG_CODEC_ADCA_VREF_SEL(1) | ADCA_TIMER_RSTN_DLY(0x10);
        } else {
            val = REG_CODEC_ADCA_VREF_SEL(2) | ADCA_TIMER_RSTN_DLY(0x10);
        }
        pmu_ana_write(ANA_REG_08 + offs, val);

        val = ADCA_PRE_CHARGE_TIMER_DLY(0x14);
        pmu_ana_write(ANA_REG_09 + offs, val);

        val = REG_CODEC_ADCA_IBSEL_RES_VOLT(4) | REG_CODEC_ADCA_IBSEL_BIAS_BUFFER(4) | REG_CODEC_ADCA_BUFFERP_IBIT(0) | REG_CODEC_ADCA_BUFFERN_IBIT(0);
        pmu_ana_write(ANA_REG_0D + offs, val);

        val = REG_CODEC_ADCA_VCM_DAC_IBIT(0) | REG_CODEC_ADCA_IBSEL_VCM_DAC(4) | REG_CODEC_ADCA_JUMP_EN | REG_CODEC_ADCA_JUMP_BIT(4);
        pmu_ana_write(ANA_REG_0E + offs, val);

        if (vcodec_mv > 1350) {
            val = REG_CODEC_ADCA_REG_VSEL(8);
        } else {
            val = REG_CODEC_ADCA_REG_VSEL(8) | REG_CODEC_CLK_SW_IN_ADCA_DR;
        }
        pmu_ana_write(ANA_REG_0F + offs, val);
    }

    analog1809_codec_vtoi_config();
    analog1809_aud_adc_clk_sw_config();
}

uint32_t analog1809_aud_get_max_dre_gain(void)
{
    // From REG_CODEC_TX_EAR_DRE_GAIN_L
    return 0x10;
}

void analog1809_codec_low_power_dac(void)
{
    uint16_t val;
    if (vcodec_mv >= 1700) {
        pmu_ana_read(ANA_REG_51, &val);
        val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL_TX, 0x3);
        pmu_ana_write(ANA_REG_51, val);

        pmu_ana_read(ANA_REG_14F, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_IB_SEL_ST2_R, 0);
        pmu_ana_write(ANA_REG_14F, val);

        pmu_ana_read(ANA_REG_11B, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_IB_SEL_ST2_L, 0);
        pmu_ana_write(ANA_REG_11B, val);
    }
}

void analog1809_codec_ci2s_slv_inf(void);
void analog1809_open(void)
{
    uint16_t val;

#ifdef DYN_ADC_GAIN
    memcpy(dyn_adc_gain_db, tgt_adc_db, sizeof(dyn_adc_gain_db));
#endif

    pmu_ana_read(ANA_REG_51, &val);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL, 0x8);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL_TX, 0x8);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL_VOICE, 0x8);
    pmu_ana_write(ANA_REG_51, val);

    val = RESERVED_DIG_32;
    pmu_ana_write(ANA_REG_53, val);

    val = REG_CODEC_LP_VCM_DR;
    pmu_ana_write(ANA_REG_52, val);

    analog1809_aud_adc_chan_init();
#ifdef SPLIT_VMEM_VMEM2
    enum PMU_CHIP_TYPE_T pmu_type = pmu_get_chip_type();

    if (pmu_type == PMU_CHIP_TYPE_1805 || pmu_type == PMU_CHIP_TYPE_1806) {
    } else
#else
    {
        analog1809_codec_set_vcm_low_vcm(0x7);
        analog1809_codec_set_vcm_low_vcm_lp(0x7);
        analog1809_codec_set_vcm_low_vcm_lpf(0x7);
    }
#endif

    pmu_ana_read(ANA_REG_53, &val);
    val = SET_BITFIELD(val, REG_CODEC_BUF_LOWVCM, 0x4);
    pmu_ana_write(ANA_REG_53, val);

    val = REG_CODEC_TX_REGULATOR_BIT(7);
#ifdef CODEC_TX_PEAK_DETECT
    val |= REG_CODEC_TX_PEAK_NL_EN | REG_CODEC_TX_PEAK_PL_EN;
#endif
    pmu_ana_write(ANA_REG_10D, val);


    val = REG_CODEC_IO_DRV_SEL(0Xf); //SET SPI DRV(ONLY FOR CI2S)
#ifdef PMU_LOW_POWER_CLOCK
    val |= REG_CLKMUX_LOWF_IN;
#endif
    pmu_ana_write(ANA_REG_11E, val);

    val = REG_CODEC_TX_EAR_DRE_GAIN_L_TMP(0x10) | REG_CODEC_TX_EAR_DRE_GAIN_R_TMP(0x10)
#ifdef DAC_DRE_GAIN_DC_UPDATE
        | REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE | REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE
#endif
        | REG_CODEC_TX_EAR_OCEN_L;
    pmu_ana_write(ANA_REG_10E, val);

    if (vcodec_mv > 1350) {
        val = REG_CODEC_TX_DAC_VREF_L(7) | REG_CODEC_TX_DAC_VREF_R(7);
    } else {
        val = REG_CODEC_TX_DAC_VREF_L(6) | REG_CODEC_TX_DAC_VREF_R(6);
    }

    pmu_ana_write(ANA_REG_10F, val);

    val = REG_CODEC_TX_EAR_COMP_L(0x10) | REG_CODEC_TX_EAR_COMP_R(0x10);
    pmu_ana_write(ANA_REG_110, val);

    val = REG_CODEC_TX_EAR_IBSEL_L(1) | REG_CODEC_TX_EAR_IBSEL_R(1) | REG_CODEC_TX_EAR_LOWGAINL(0) | REG_CODEC_TX_EAR_LOWGAINR(0);
    pmu_ana_write(ANA_REG_111, val);

    val = REG_CODEC_TX_EAR_OUTPUTSEL_L(1) | REG_CODEC_TX_EAR_SOFTSTART(0x8) | REG_CODEC_TX_EAR_DR_ST_L(7) | REG_CODEC_TX_EAR_DR_ST_R(7);
    pmu_ana_write(ANA_REG_114, val);

    val = REG_CODEC_TX_EAR_OCEN_R | REG_CODEC_TX_EAR_OUTPUTSEL_R(1) | REG_CODEC_TX_IB_SEL_ST2_R(1);
    pmu_ana_write(ANA_REG_14F, val);

    val = REG_BYPASS_CODEC_TX_REGULATOR | REG_CODEC_TX_EAR_GAIN_L | REG_CODEC_TX_EAR_GAIN_R;
    pmu_ana_write(ANA_REG_115, val);

    if (vcodec_mv > 1350) {
        val = REG_CODEC_TX_EAR_VCM_R(2) | REG_CODEC_TX_EAR_VCM_L(2);
    } else {
        val = REG_CODEC_TX_EAR_VCM_R(1) | REG_CODEC_TX_EAR_VCM_L(1);
    }
    // DAC dre gain select dig ctrl mode
    val |= (DRE_GAIN_SEL_L | DRE_GAIN_SEL_R);
    pmu_ana_write(ANA_REG_116, val);

    val = REG_CODEC_TX_IB_SEL_ST2_L(1) | REG_CODEC_TX_CASP_L(3) | REG_CODEC_TX_CASP_R(3)
        | REG_CODEC_TX_CASN_L(3) | REG_CODEC_TX_CASN_R(3);
    pmu_ana_write(ANA_REG_11B, val);

    val = REG_CODEC_TX_VREFBUF_L_LP(0) | REG_CODEC_TX_VREFBUF_R_LP(0) | REG_VSEL_CLKMUX(2);
    pmu_ana_write(ANA_REG_120, val);

    val = REG_RX_REGULAOTR_BIT(8);
    pmu_ana_write(ANA_REG_150, val);

#ifdef CODEC_TX_ZERO_CROSSING_EN_GAIN
    val = REG_PU_ZERO_DET_L | REG_PU_ZERO_DET_R | REG_ZERO_DETECT_CHANGE;
    pmu_ana_write(ANA_REG_7A, val);
#endif

#ifdef VCM_ON
    analog1809_aud_enable_common_internal(ANA_CODEC_USER_DAC, true);
#endif

#ifdef RC48M_ENABLE
#ifndef VCM_ON
#error "RC48M_ENABLE should work with VCM_ON"
#endif
    analog1809_rc48m_enable();
#endif

    analog1809_codec_ci2s_slv_inf();
}

void analog1809_sleep(void)
{
#ifndef VCM_ON
    if (codec_common_map)
#endif
    {
        analog1809_codec_lp_vcm(true);
    }
}

void analog1809_wakeup(void)
{
#ifndef VCM_ON
    if (codec_common_map)
#endif
    {
        analog1809_codec_lp_vcm(false);
    }
}

void analog1809_aud_codec_anc_enable(enum ANC_TYPE_T type, bool en)
{
    enum ANA_CODEC_USER_T user;
    enum AUD_CHANNEL_MAP_T ch_map;

    user = (enum ANA_CODEC_USER_T)0;
    ch_map = (enum AUD_CHANNEL_MAP_T)0;
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
    if (type & ANC_FEEDFORWARD) {
        user |= ANA_CODEC_USER_ANC_FF;
        ch_map |= ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R;
    }
#endif
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
    if (type & ANC_FEEDBACK) {
        user |= ANA_CODEC_USER_ANC_FB;
        ch_map |= ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R;
    }
#endif
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
    if (type & ANC_TALKTHRU) {
        user |= ANA_CODEC_USER_ANC_TT;
        ch_map |= ANC_TT_MIC_CH_L | ANC_TT_MIC_CH_R;
    }
#endif

    ANALOG_DEBUG_TRACE(0, "%s: type=%d en=%d ch_map=0x%x", __func__, type, en, ch_map);

    if (en) {
        analog1809_aud_enable_vmic(user, ANC_VMIC_CFG, true);
        analog1809_aud_enable_codec_common(user, true);
        analog1809_aud_enable_adda_common(user, true);
        analog1809_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
        analog1809_aud_enable_adc(user, ch_map, true);
    } else {
        // analog1809_aud_restore_anc_dyn_adc_gain(type);
        analog1809_aud_enable_adc(user, ch_map, false);
        analog1809_aud_enable_adda_common(user, false);
        analog1809_aud_enable_codec_common(user, false);
        analog1809_aud_enable_vmic(user, ANC_VMIC_CFG, false);
    }
}

void analog1809_aud_codec_anc_boost(bool en)
{
    float attn;

    if (vcodec_mv != 1700) {
        return;
    }

    if (en) {
    } else {
    }

    if (en) {
        pmu_codec_volt_ramp_up();
        // 1700 mV ==> 1750 mV: -0.25 dB or 0.971627952f
        //             1800 mV: -0.5  dB or 0.944060876f
        //             1950 mV: -0.7  dB or 0.922571427f
        attn = 0.944060876f;
    } else {
        pmu_codec_volt_ramp_down();
        // 0 dB
        attn = 1.0f;
    }
    hal_codec_set_anc_boost_gain_attn(attn);
}

void analog1809_aud_mickey_enable(bool en)
{
    if (en) {
        analog1809_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, true);
        analog1809_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV, true);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, true);
    } else {
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, false);
        analog1809_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV, false);
        analog1809_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, false);
    }
}

void analog1809_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    dev = hal_codec_get_input_path_cfg(input_path);

    if (en) {
        // Enable vmic first to overlap vmic stable time with codec vcm stable time
        analog1809_aud_enable_vmic(ANA_CODEC_USER_ADC, dev, true);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_ADC, true);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_ADC, true);
        analog1809_aud_set_adc_gain(input_path, ch_map);
        analog1809_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, true);
    } else {
        analog1809_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, false);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_ADC, false);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_ADC, false);
        analog1809_aud_enable_vmic(ANA_CODEC_USER_ADC, dev, false);
    }
}

void analog1809_aud_codec_adc2_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    dev = hal_codec_get_input_path_cfg(input_path);

    if (en) {
        // Enable vmic first to overlap vmic stable time with codec vcm stable time
        analog1809_aud_enable_vmic(ANA_CODEC_USER_ADC2, dev, true);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_ADC2, true);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_ADC2, true);
        analog1809_aud_set_adc_gain(input_path, ch_map);
        analog1809_aud_enable_adc(ANA_CODEC_USER_ADC2, ch_map, true);
    } else {
        analog1809_aud_enable_adc(ANA_CODEC_USER_ADC2, ch_map, false);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_ADC2, false);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_ADC2, false);
        analog1809_aud_enable_vmic(ANA_CODEC_USER_ADC2, dev, false);
    }
}

static void analog1809_aud_codec_config_speaker(void)
{
    bool en;
    uint32_t dac;

    if (ana_spk_req && !ana_spk_muted) {
        en = true;
    } else {
        en = false;
    }

    if (ana_spk_enabled != en) {
        ana_spk_enabled = en;
        if (en) {
            dac = analog1809_aud_pmu_dac_dev_convert(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);
            analog1809_aud_enable_dac_pa(dac);
        } else {
            analog1809_aud_enable_dac_pa(0);
        }
    }
}

void analog1809_aud_codec_speaker_enable(bool en)
{
    ana_spk_req = en;
    analog1809_aud_codec_config_speaker();
}

void analog1809_aud_codec_dac_enable(bool en)
{
    uint32_t dac;
    if (en) {
        dac = analog1809_aud_pmu_dac_dev_convert(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);
        analog1809_codec_ci2s_dac_en(true);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_DAC, true);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_DAC, true);
        pmu_codec_hppa_enable(1);
        analog1809_aud_enable_dac(dac);
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        osDelay(1);
        analog1809_aud_codec_speaker_enable(true);
#endif
    } else {
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        analog1809_aud_codec_speaker_enable(false);
        osDelay(1);
#endif
        analog1809_aud_enable_dac(0);
        pmu_codec_hppa_enable(0);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_DAC, false);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_DAC, false);
        analog1809_codec_ci2s_dac_en(false);
    }
}

void analog1809_aud_codec_open(void)
{
    analog1809_aud_vcodec_enable(ANA_CODEC_USER_CODEC, true);
}

void analog1809_aud_codec_close(void)
{
    analog1809_aud_codec_speaker_enable(false);
    osDelay(1);
    analog1809_aud_codec_dac_enable(false);

    analog1809_aud_vcodec_enable(ANA_CODEC_USER_CODEC, false);
}

void analog1809_aud_codec_mute(void)
{
#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog1809_codec_tx_pa_gain_sel(0);
#endif

    ana_spk_muted = true;
    analog1809_aud_codec_config_speaker();
}

void analog1809_aud_codec_nomute(void)
{
    ana_spk_muted = false;
    analog1809_aud_codec_config_speaker();

#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog1809_aud_set_dac_gain(dac_gain);
#endif
}

int analog1809_debug_config_audio_output(bool diff)
{
    return 0;
}

int analog1809_debug_config_codec(uint16_t mv)
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

int analog1809_debug_config_low_power_adc(bool enable)
{
    return 0;
}

void analog1809_debug_config_anc_calib_mode(bool enable)
{
    anc_calib_mode = enable;
}

bool analog1809_debug_get_anc_calib_mode(void)
{
    return anc_calib_mode;
}

int analog1809_debug_config_vad_mic(bool enable)
{
    return 0;
}

#if 1//def RPC_DEBUG_EN
#define RPC_DBG TRACE
#else
#define RPC_DBG(...) do{}while(0)
#endif

void analog1809_vcm_enable(enum ANA1809_VCM_USER_T user)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (vcm_map == 0) {
        pmu_ana_read(ANA_REG_52, &val);
        val |= REG_CODEC_EN_VCM;
        pmu_ana_write(ANA_REG_52, val);
    }
    vcm_map |= user;
    int_unlock(lock);
}

void analog1809_vcm_disable(enum ANA1809_VCM_USER_T user)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (vcm_map == user) {
        pmu_ana_read(ANA_REG_52, &val);
        val &= ~REG_CODEC_EN_VCM;
        pmu_ana_write(ANA_REG_52, val);
    }
    vcm_map &= ~user;
    int_unlock(lock);
}

void analog1809_codec_lp_vcm(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_52, &val);
    if (en) {
        val |= REG_CODEC_LP_VCM;
    } else {
        val &= ~REG_CODEC_LP_VCM;
    }
    pmu_ana_write(ANA_REG_52, val);
}

void analog1809_codec_vcm_enable_lpf(bool en)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_53, &val);
    if (en) {
        val |= REG_CODEC_VCM_EN_LPF;
    } else {
        val &= ~REG_CODEC_VCM_EN_LPF;
    }
    pmu_ana_write(ANA_REG_53, val);
}

void analog1809_codec_set_vcm_low_vcm_lp(uint16_t v)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_52, &val);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM_LP, v);
    pmu_ana_write(ANA_REG_52, val);
}

void analog1809_codec_set_vcm_low_vcm_lpf(uint16_t v)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_52, &val);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM_LPF, v);
    pmu_ana_write(ANA_REG_52, val);
}

void analog1809_codec_set_vcm_low_vcm(uint16_t v)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_52, &val);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM, v);
    pmu_ana_write(ANA_REG_52, val);
}

/***************************************************************/
static struct CODEC_DAC_CI2S_CFG_T ana_ci2s_cfg = {
    .txch_en = 0xf,
    .rxch_en = 0xf,
    .tx_dm = 1,
    .rx_dm = 1,
    .slv = 0,
    .clk_wordsize = 0,
    // 0:3M 1:1.5M
    .rate_sel = 1,
    .en_phase_sel = 0,
    .adc_fifo_th = 0,
    .dac_fifo_th = 0,
    .dac_en_ch0 = 1,
    .dac_en_ch1 = 1,
    .dac_sinc_rate_sel_ch0 = 1,
    .dac_sinc_rate_sel_ch1 = 1,

    .tdm_en = 1,
    .tdm_mode = 0,
    .tdm_ws_neg = 0,
    .tdm_frame_width = 1,
    .tdm_ws_width = 0,
    .tdm_slot_width = 1,
    .tdm_data_offset = 0,
    .i2s_data_offset = 0,

    .pol_clk_ci2s_tx = 1,
    .tdm_tx_edge = 1
};

static void analog1809_codec_ci2s_enable(uint8_t ci2s_sel)
{
    uint16_t val;

    TRACE(0, "[%s] ci2s_sel = %x", __func__, ci2s_sel);

    if (ci2s_sel & CI2S_TX_EN) {
        pmu_ana_read(ANA_REG_35B, &val);
        val |= CODEC_CI2S_TX_EN;
        pmu_ana_write(ANA_REG_35B, val);
    } else {
        pmu_ana_read(ANA_REG_35B, &val);
        val &= ~CODEC_CI2S_TX_EN;
        pmu_ana_write(ANA_REG_35B, val);
    }

    if (ci2s_sel & CI2S_RX_EN) {
        pmu_ana_read(ANA_REG_35B, &val);
        val |= CODEC_CI2S_RX_EN;
        pmu_ana_write(ANA_REG_35B, val);
    } else {
        pmu_ana_read(ANA_REG_35B, &val);
        val &= ~CODEC_CI2S_RX_EN;
        pmu_ana_write(ANA_REG_35B, val);
    }

    if (ci2s_sel) {
        pmu_ana_read(ANA_REG_35B, &val);
        val |= CODEC_CI2S_EN;
        pmu_ana_write(ANA_REG_35B, val);
    } else {
        pmu_ana_read(ANA_REG_35B, &val);
        val &= ~CODEC_CI2S_EN;
        pmu_ana_write(ANA_REG_35B, val);
    }
}

static void analog1809_codec_ci2s_tx_en(bool enable)
{
    TRACE(0,"%s", __func__);
    if (enable) {
        ana_ci2s_idx |= CI2S_TX_EN;
    } else {
        ana_ci2s_idx &= ~CI2S_TX_EN;
    }
    analog1809_codec_ci2s_enable(ana_ci2s_idx);
}

static void analog1809_codec_ci2s_rx_en(bool enable)
{
    TRACE(0,"%s", __func__);
    if (enable) {
        ana_ci2s_idx |= CI2S_RX_EN;
    } else {
        ana_ci2s_idx &= ~CI2S_RX_EN;
    }
    analog1809_codec_ci2s_enable(ana_ci2s_idx);
}

void analog1809_codec_ci2s_adc_en(bool enable)
{
    hal_codec_ci2s_rx_enable(enable);
    analog1809_codec_ci2s_tx_en(enable);
}

void analog1809_codec_ci2s_dac_en(bool enable)
{
    hal_codec_ci2s_tx_enable(enable);
    analog1809_codec_ci2s_rx_en(enable);
}

void analog1809_codec_ci2s_slv_inf(void)
{
    struct CODEC_DAC_CI2S_CFG_T * ci2s = &ana_ci2s_cfg;
    uint32_t dac_dev = analog_codec_get_dac_dev();
    uint16_t val;

    if (ci2s->rate_sel == 0) {
        if (dac_dev == (PMU_CODEC_DAC_L | PMU_CODEC_DAC_R)) {
            ci2s->tdm_frame_width = 1;
            ci2s->tx_dm = 1;
            ci2s->rx_dm = 1;
            ci2s->pol_clk_ci2s_tx = 1;
            ci2s->tdm_tx_edge = 1;
        } else {
            ci2s->tdm_frame_width = 0;
            ci2s->tx_dm = 0;
            ci2s->rx_dm = 0;
            ci2s->pol_clk_ci2s_tx = 0;
            ci2s->tdm_tx_edge = 1;
        }
    } else if (ci2s->rate_sel == 1) {
        ci2s->tdm_frame_width = 1;
        ci2s->tx_dm = 1;
        ci2s->rx_dm = 1;
        ci2s->pol_clk_ci2s_tx = 0;
        ci2s->tdm_tx_edge = 1;
    }

    pmu_ana_read(ANA_REG_6E, &val);
    val |= SOFT_RESETN_CODEC_ADC | SOFT_RESETN_CODEC_DAC;
    pmu_ana_write(ANA_REG_6E, val);

    val = CFG_DIV_CI2S(0) | BYPASS_DIV_CI2S | SEL_CI2S_CLKIN | EN_CLK_CI2S | SOFT_RSTN_CI2S;
    // val &= ~(POL_CLK_CI2S_IN | EN_CLK_CI2S_OUT | EN_SCLK_CI2S | POL_CLK_CI2S_OUT);
    pmu_ana_write(ANA_REG_361, val);

    pmu_ana_read(ANA_REG_301, &val);
    val |= CODEC_ADC_EN_CH0 | CODEC_ADC_EN_CH1 | CODEC_ADC_EN_CH2;
    pmu_ana_write(ANA_REG_301, val);

    pmu_ana_read(ANA_REG_302, &val);
    val |= CODEC_ADC_EN_MODE;
    pmu_ana_write(ANA_REG_302, val);

    pmu_ana_read(ANA_REG_306, &val);
    val = SET_BITFIELD(val, CODEC_ADC_START_CNT ,0);
    pmu_ana_write(ANA_REG_306, val);

    pmu_ana_read(ANA_REG_307, &val);
    val |= CODEC_DAC_EN_CH0 | CODEC_DAC_EN_CH1;
    pmu_ana_write(ANA_REG_307, val);

    // codec ci2s cfg low delay
    pmu_ana_read(ANA_REG_308, &val);
    val = SET_BITFIELD(val, CODEC_DAC_SINC_RATE_SEL, 2);
    val |= CODEC_DAC_SINC_CAS_CH0 | CODEC_DAC_SINC_CAS_CH1;
    pmu_ana_write(ANA_REG_308, val);

    pmu_ana_read(ANA_REG_309, &val);
    val = SET_BITFIELD(val, CODEC_DAC_START_CNT, 2);
    val |= CODEC_DAC_EN_MODE;
    pmu_ana_write(ANA_REG_309, val);

    pmu_ana_read(ANA_REG_35B, &val);
    val |= CODEC_CI2S_TXCH_EN(ci2s->txch_en) | CODEC_CI2S_RXCH_EN(ci2s->txch_en) | CODEC_CI2S_MSTN_SLV_MODE;
    if (ci2s->tdm_tx_edge == 1) {
        val |= CODEC_CI2S_TDM_TX_EDGE;
    }
    pmu_ana_write(ANA_REG_35B, val);

    val = CODEC_CI2S_TDM_EN | CODEC_CI2S_TDM_FRAME_WIDTH(ci2s->tdm_frame_width)
        | CODEC_CI2S_TDM_WS_WIDTH(ci2s->tdm_ws_width) | CODEC_CI2S_TDM_SLOT_WIDTH(ci2s->tdm_slot_width);
    pmu_ana_write(ANA_REG_35C, val);

    val = CODEC_CI2S_RX_DM(ci2s->rx_dm) | CODEC_CI2S_TX_DM(ci2s->tx_dm) | CODEC_CI2S_RATE_SEL(ci2s->rate_sel);
    pmu_ana_write(ANA_REG_35D, val);

    pmu_ana_read(ANA_REG_35F, &val);
    val |= SOFT_RSTN_ADC_ANA(0xf) | SOFT_RSTN_ADC(0xf) | EN_CLK_ADC_ANA_MASK | EN_CLK_ADC_MASK;
    pmu_ana_write(ANA_REG_35F, val);

    pmu_ana_read(ANA_REG_360, &val);
    val |= POL_ADC_ANA(7) | EN_CLK_DAC | SOFT_RSTN_DAC;
    if (ci2s->pol_clk_ci2s_tx == 1) {
        val |= POL_CLK_CI2S_TX;
    }
#ifdef PMU_LOW_POWER_CLOCK
    val |= SEL_CLK_CODEC;
#endif
    pmu_ana_write(ANA_REG_360, val);

    pmu_ana_read(ANA_REG_301, &val);
    val |= CODEC_ADC_EN;
    pmu_ana_write(ANA_REG_301, val);

    pmu_ana_read(ANA_REG_307, &val);
    val |= CODEC_DAC_EN;
    pmu_ana_write(ANA_REG_307, val);

    pmu_ana_read(ANA_REG_116, &val);
    val |= CFG_TX_TREE_EN;
    pmu_ana_write(ANA_REG_116, val);
}
/***************************************************************/

void analog1809_rpcsvr_freq_pll_config(uint32_t freq, uint32_t div)
{
    RPC_DBG(1, "%s: freq=%d, div=%d", __func__, freq, div);
}

void analog1809_rpcsvr_osc_clk_enable(bool en)
{
    RPC_DBG(1, "%s: en=%d", __func__, en);
    rpcsvr_osc_enable = en;
    analog1809_aud_osc_clk_enable(en);
}

void analog1809_rpcsvr_codec_open(bool en)
{
    RPC_DBG(1, "%s: en=%d", __func__, en);
    analog1809_aud_vcodec_enable(ANA_CODEC_USER_VAD, en);
}

void analog1809_rpcsvr_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    RPC_DBG(1, "%s: input_path=%d, ch_map=0x%x, en=%d", __func__, input_path, ch_map, en);
    dev = hal_codec_get_input_path_cfg(input_path);
#if defined(VAD_USE_SAR_ADC)
    ASSERT(ch_map==AUD_CHANNEL_MAP_CH5, "%s: bad ch_map=%x", __func__, ch_map);
    dev = (dev & (~AUD_CHANNEL_MAP_ANA_ALL)) | ch_map;
#endif

    if (en) {
        // Enable vmic first to overlap vmic stable time with codec vcm stable time
        analog1809_aud_enable_vmic(ANA_CODEC_USER_RPCSVR_ADC, dev, true);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_RPCSVR_ADC, true);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_RPCSVR_ADC, true);
        analog1809_aud_set_adc_gain(input_path, ch_map);
        analog1809_aud_enable_adc(ANA_CODEC_USER_RPCSVR_ADC, ch_map, true);
    } else {
        analog1809_aud_enable_adc(ANA_CODEC_USER_RPCSVR_ADC, ch_map, false);
        analog1809_aud_enable_adda_common(ANA_CODEC_USER_RPCSVR_ADC, false);
        analog1809_aud_enable_codec_common(ANA_CODEC_USER_RPCSVR_ADC, false);
        analog1809_aud_enable_vmic(ANA_CODEC_USER_RPCSVR_ADC, dev, false);
    }
}

void analog1809_rpcsvr_vad_enable(enum AUD_VAD_TYPE_T type, bool en)
{
    RPC_DBG(1, "%s: type=%d, en=%d", __func__, type, en);
}

void analog1809_rpcsvr_vad_adc_enable(bool en)
{
    RPC_DBG(1, "%s: en=%d", __func__, en);
}

void analog1809_capsensor_open(void) //0x52, 0x7779 \ 0x53, 0x1081
{
    analog1809_codec_enable_bias_lp(true);  //0x50, 0x4000
    analog1809_codec_enable_pu_rc(true);    //0x55, 0x0298
    analog1809_codec_enable_capsensor_osc(true); //0x6D, 0xF09C
}

void analog1809_capsensor_reset_set(void)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_6D, &val);
    val &= ~(SOFT_RESETN_CAPSENSOR | SOFT_RESETN_CAPSENSOR_REG);
    pmu_ana_write(ANA_REG_6D, val);
}

void analog1809_capsensor_reset_clear(void)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_6D, &val);
    val |= SOFT_RESETN_CAPSENSOR | SOFT_RESETN_CAPSENSOR_REG;
    pmu_ana_write(ANA_REG_6D, val);
}

void analog1809_capsensor_clk_gate_on(void)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_6D, &val);
    val &= ~(REG_CLK_CAPSENSOR_GATE_EN | REG_CLK_32K_CAPSENSOR_GATE_EN);
    pmu_ana_write(ANA_REG_6D, val);
}

void analog1809_capsensor_clk_gate_off(void)
{
    uint16_t val;

    pmu_ana_read(ANA_REG_6D, &val);
    val |= REG_CLK_CAPSENSOR_GATE_EN | REG_CLK_32K_CAPSENSOR_GATE_EN;
    pmu_ana_write(ANA_REG_6D, val);
}
