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
#ifndef CHIP_SUBSYS_SENS

#include "plat_addr_map.h"
#include "analog.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_psc.h"
#include "hal_aud.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "pmu.h"
#include CHIP_SPECIFIC_HDR(reg_codec)
#include "string.h"
#include "tgt_hardware.h"

#if defined(CODEC_ADC1_DSF) || defined(CODEC_ADC2_DSF)
#include "hal_codec_dsf_coef.h"
#endif

//#define AUDIO_OUTPUT_DAC2_MIX_BEFORE_ANC
//#define AUDIO_OUTPUT_DAC3_MIX_BEFORE_ANC

#if defined(AUDIO_OUTPUT_DAC2_MIX_BEFORE_ANC) || defined(AUDIO_OUTPUT_DAC3_MIX_BEFORE_ANC)
#ifndef CODEC_DAC1_USF
#define CODEC_DAC1_USF
#endif
#if !defined(CODEC_DAC2_USF) && defined(AUDIO_OUTPUT_DAC2_MIX_BEFORE_ANC)
#define CODEC_DAC2_USF
#endif
#if !defined(CODEC_DAC3_USF) && defined(AUDIO_OUTPUT_DAC3_MIX_BEFORE_ANC)
#define CODEC_DAC3_USF
#endif
#ifndef CODEC_TRIG_DAC_EN_ENABLE
#define CODEC_TRIG_DAC_EN_ENABLE
#endif
#endif

#if defined(CODEC_DAC1_USF) || defined(CODEC_DAC2_USF) || defined(CODEC_DAC3_USF)
#include "hal_codec_usf_coef.h"
#endif

#define CODEC_CLK_FROM_ANA

#define NO_DAC_RESET

//#define SDM_MUTE_NOISE_SUPPRESSION
//#define SDM_MUTE_NOISE_SUPPRESSION_V2

// #define DAC_ZERO_CROSSING_GAIN
#define DAC_RAMP_GAIN

#define CODEC_PLL_CLOCK                     (CODEC_FREQ_48K_SERIES * CODEC_CMU_DIV)

#ifndef CODEC_OUTPUT_DEV
#define CODEC_OUTPUT_DEV                    CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV
#endif
#if ((CODEC_OUTPUT_DEV & CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV) == 0)
#ifndef AUDIO_OUTPUT_SWAP
#define AUDIO_OUTPUT_SWAP
#endif
#endif

// Recudce 0.57dB in 100Hz@16kHz SampleRate
// #define CODEC_ADC_DC_FILTER_FACTOR          (6)

// For 44.1K/48K sample rate
#ifndef CODEC_DAC_GAIN_RAMP_INTERVAL
#define CODEC_DAC_GAIN_RAMP_INTERVAL        CODEC_DAC_GAIN_RAMP_INTVL_4_SAMP
#endif

#ifndef CODEC_DAC2_GAIN_RAMP_INTERVAL
#define CODEC_DAC2_GAIN_RAMP_INTERVAL       CODEC_DAC_GAIN_RAMP_INTVL_4_SAMP
#endif

#ifndef CODEC_DAC3_GAIN_RAMP_INTERVAL
#define CODEC_DAC3_GAIN_RAMP_INTERVAL       CODEC_DAC_GAIN_RAMP_INTVL_4_SAMP
#endif

#if (defined(__TWS__) || defined(IBRT)) && defined(ANC_APP)
//#define CODEC_TIMER
#endif

#ifdef TDM_RELAY
#define CODEC_TIMER
#endif

#ifdef CODEC_DSD
#ifdef ANC_APP
#error "ANC_APP conflicts with CODEC_DSD"
#endif
#ifdef AUDIO_ANC_FB_MC
#error "AUDIO_ANC_FB_MC conflicts with CODEC_DSD"
#endif
#ifdef __AUDIO_RESAMPLE__
#error "__AUDIO_RESAMPLE__ conflicts with CODEC_DSD"
#endif
#endif

#ifdef AUDIO_ANC_FB_MC
#ifdef PSAP_APP
#error "PSAP_APP conflicts with AUDIO_ANC_FB_MC"
#endif
#endif

//#define AUDIO_ANC_LOW_DELAY

#define RS_CLOCK_FACTOR                     200

// Trigger DMA request when TX-FIFO *empty* count > threshold
#define HAL_CODEC_TX_FIFO_TRIGGER_LEVEL     (3)
// Trigger DMA request when RX-FIFO count >= threshold
#define HAL_CODEC_RX_FIFO_TRIGGER_LEVEL     (4)

#define MAX_DIG_DBVAL                       (50)
#define ZERODB_DIG_DBVAL                    (0)
#define MIN_DIG_DBVAL                       (-99)

#define MAX_SIDETONE_DBVAL                  (30)
#define MIN_SIDETONE_DBVAL                  (-30)
#define SIDETONE_DBVAL_STEP                 (-2)

#define MAX_SIDETONE_REGVAL                 (0)
#define MIN_SIDETONE_REGVAL                 (30)
#define MUTE_SIDETONE_REGVAL                (31)

#ifndef MC_DELAY_COMMON
#define MC_DELAY_COMMON                     28
#endif

#ifndef CODEC_DIGMIC_PHASE
#define CODEC_DIGMIC_PHASE                  3
#endif

#define BT_TRIGGER_NUM                      4

#define ADC_IIR_CH_NUM                      4

#define MAX_DIG_MIC_CH_NUM                  6

#define NORMAL_ADC_CH_NUM                   4
// Echo cancel ADC channel number
#define EC_ADC_CH_NUM                       2
#define MAX_ADC_CH_NUM                      (NORMAL_ADC_CH_NUM + EC_ADC_CH_NUM)
#define MAX_ADC2_CH_NUM                     (NORMAL_ADC_CH_NUM + EC_ADC_CH_NUM)

#define MAX_DAC_CH_NUM                      2
#define MAX_DAC2_CH_NUM                     2
#define MAX_DAC3_CH_NUM                     2

#ifdef CODEC_DSD
#define NORMAL_MIC_MAP                      (AUD_CHANNEL_MAP_NORMAL_ALL & ~(AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7 | \
                                                AUD_CHANNEL_MAP_DIGMIC_CH6 | AUD_CHANNEL_MAP_DIGMIC_CH7 | \
                                                AUD_CHANNEL_MAP_DIGMIC_CH2 | AUD_CHANNEL_MAP_DIGMIC_CH3))
#else
#define NORMAL_MIC_MAP                      (AUD_CHANNEL_MAP_NORMAL_ALL & ~(AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7 | \
                                                AUD_CHANNEL_MAP_DIGMIC_CH6 | AUD_CHANNEL_MAP_DIGMIC_CH7))
#endif
#define NORMAL_ADC_MAP                      (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | \
                                                AUD_CHANNEL_MAP_CH3)

#define EC_MIC_MAP                          (AUD_CHANNEL_MAP_ECMIC_CH0 | AUD_CHANNEL_MAP_ECMIC_CH1)
#define EC_ADC_MAP                          (AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5)

#define VALID_MIC_MAP                       (NORMAL_MIC_MAP | EC_MIC_MAP)
#define VALID_ADC_MAP                       (NORMAL_ADC_MAP | EC_ADC_MAP)

#define VALID_SPK_MAP                       (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define VALID_DAC_MAP                       VALID_SPK_MAP
#define VALID_DAC2_MAP                      (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define VALID_DAC3_MAP                      (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)

#if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV & ~VALID_SPK_MAP)
#error "Invalid CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV"
#endif
#if (CODEC_OUTPUT_DEV & ~VALID_SPK_MAP)
#error "Invalid CODEC_OUTPUT_DEV"
#endif

#define RSTN_ADC_FREE_RUNNING_CLK           CODEC_SOFT_RSTN_ADC(1 << (MAX_ADC_CH_NUM + 1))

#if defined(SPEECH_SIDETONE) && \
        (defined(CFG_HW_AUD_SIDETONE_MIC_DEV) && (CFG_HW_AUD_SIDETONE_MIC_DEV)) && \
        defined(CFG_HW_AUD_SIDETONE_GAIN_DBVAL)
#define SIDETONE_ENABLE
#if (CFG_HW_AUD_SIDETONE_GAIN_DBVAL > MAX_SIDETONE_DBVAL) || \
        (CFG_HW_AUD_SIDETONE_GAIN_DBVAL < MIN_SIDETONE_DBVAL) || \
        defined(CFG_HW_AUD_SIDETONE_IIR_INDEX) || \
        defined(CFG_HW_AUD_SIDETONE_GAIN_RAMP)
#define SIDETONE_DEDICATED_ADC_CHAN
#define SIDETONE_RESERVED_ADC_CHAN
#endif
#endif

enum CODEC_ADC_EN_REQ_T {
    CODEC_ADC_EN_REQ_STREAM,
    CODEC_ADC_EN_REQ_MC,
    CODEC_ADC_EN_REQ_DSD,

    CODEC_ADC_EN_REQ_QTY,
};

enum CODEC_IRQ_TYPE_T {
    CODEC_IRQ_TYPE_BT_TRIGGER,
    CODEC_IRQ_TYPE_VAD,
    CODEC_IRQ_TYPE_ANC_FB_CHECK,
    CODEC_IRQ_TYPE_EVENT_TRIGGER,
    CODEC_IRQ_TYPE_TIMER_TRIGGER,

    CODEC_IRQ_TYPE_QTY,
};

enum CODEC_DAC_GAIN_RAMP_INTVL_T {
    CODEC_DAC_GAIN_RAMP_INTVL_1_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_2_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_4_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_8_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_16_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_32_SAMP,

    CODEC_DAC_GAIN_RAMP_INTVL_QTY,
};

enum DAC_RS_CLK_USER {
    RS_CLK_USER_DAC1 = 0,
    RS_CLK_USER_DAC2,
    RS_CLK_USER_DAC3,

    RS_CLK_USER_QTY,
};

enum ADC_RS_CLK_USER {
    RS_CLK_USER_ADC1 = 0,
    RS_CLK_USER_ADC2,

    RS_CLK_USER_QTY2,
};

enum CODEC_VOLT_RAMP_USER_T {
    CODEC_VOLT_RAMP_USER_DIG_MIC    = (1 << 0),
    CODEC_VOLT_RAMP_USER_ANC        = (1 << 1),
};

enum CODEC_HW_MOD_T {
    CODEC_HW_MOD_DAC1 = 0,
    CODEC_HW_MOD_DAC2,
    CODEC_HW_MOD_DAC3,
    CODEC_HW_MOD_ADC1,
    CODEC_HW_MOD_ADC2,

    CODEC_HW_MOD_DAC_QTY = CODEC_HW_MOD_ADC1,
    CODEC_HW_MOD_ADC_QTY = (CODEC_HW_MOD_ADC2 - CODEC_HW_MOD_DAC3),
};

#define ADC_MOD_IDX(m) (m-CODEC_HW_MOD_ADC1)
#define DAC_MOD_IDX(m) (m-CODEC_HW_MOD_DAC1)

enum {
    DRE_STEP_MODE_STEP_1,
    DRE_STEP_MODE_STEP_2,
    DRE_STEP_MODE_STEP_4,
    DRE_STEP_MODE_STEP_8,
    DRE_STEP_MODE_STEP_16,
};

struct CODEC_DAC_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t cmu_div;
    uint8_t dac_up;
    uint8_t bypass_cnt;
    uint8_t mc_delay;
};

static const struct CODEC_DAC_SAMPLE_RATE_T codec_dac_sample_rate[] = {
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,      CODEC_FREQ_CRYSTAL,               1,             1, 6, 0, MC_DELAY_COMMON + 160},
    {AUD_SAMPRATE_16927,     CODEC_FREQ_CRYSTAL,               1,             1, 3, 0, MC_DELAY_COMMON +  85},
    {AUD_SAMPRATE_50781,     CODEC_FREQ_CRYSTAL,               1,             1, 1, 0, MC_DELAY_COMMON +  29},
    {AUD_SAMPRATE_101562,    CODEC_FREQ_CRYSTAL,               1,             1, 1, 1, MC_DELAY_COMMON +  29},
#endif
    {AUD_SAMPRATE_7350,   CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0, MC_DELAY_COMMON + 174},
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0, MC_DELAY_COMMON + 168}, // T
    {AUD_SAMPRATE_14700,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0, MC_DELAY_COMMON +  71},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0, MC_DELAY_COMMON +  88}, // T
    {AUD_SAMPRATE_22050,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 2, 0, MC_DELAY_COMMON +  60},
    {AUD_SAMPRATE_24000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 2, 0, MC_DELAY_COMMON +  58},
    {AUD_SAMPRATE_32000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 1, MC_DELAY_COMMON +  48}, // T
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0, MC_DELAY_COMMON +  31}, // T
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0, MC_DELAY_COMMON +  30}, // T
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1, MC_DELAY_COMMON +  12},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1, MC_DELAY_COMMON +  12}, // T
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2, MC_DELAY_COMMON +   5},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2, MC_DELAY_COMMON +   5},
    {AUD_SAMPRATE_352800, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3, MC_DELAY_COMMON +   2},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3, MC_DELAY_COMMON +   2},
};

struct CODEC_ADC_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t cmu_div;
    uint8_t adc_down;
    uint8_t bypass_cnt;
};

static const struct CODEC_ADC_SAMPLE_RATE_T codec_adc_sample_rate[] = {
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,      CODEC_FREQ_CRYSTAL,               1,             1, 6, 0},
    {AUD_SAMPRATE_16927,     CODEC_FREQ_CRYSTAL,               1,             1, 3, 0},
    {AUD_SAMPRATE_50781,     CODEC_FREQ_CRYSTAL,               1,             1, 1, 0},
    {AUD_SAMPRATE_101562,    CODEC_FREQ_CRYSTAL,               1,             1, 1, 1},
#endif
    {AUD_SAMPRATE_7350,   CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0},
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0},
    {AUD_SAMPRATE_14700,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0},
    {AUD_SAMPRATE_24000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 2, 0},
    {AUD_SAMPRATE_32000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 1},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0},
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0},
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1},
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2},
    {AUD_SAMPRATE_352800, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3},
};

const CODEC_ADC_VOL_T WEAK codec_adc_vol[TGT_ADC_VOL_LEVEL_QTY] = {
    -99, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28,
};

static struct CODEC_T * const codec = (struct CODEC_T *)CODEC_BASE;

#define CODEC_FIR_CH0_BASE                              (CODEC_BASE + 0x9000)
#define CODEC_FIR_CH1_BASE                              (CODEC_BASE + 0xB000)
#define CODEC_FIR_CH2_BASE                              (CODEC_BASE + 0xD000)
#define CODEC_FIR_CH3_BASE                              (CODEC_BASE + 0xE000)

#define CODEC_FIR_HISTORY_CH0_BASE                      (CODEC_FIR_CH0_BASE - 0x1000)
#define CODEC_FIR_HISTORY_CH1_BASE                      (CODEC_FIR_CH1_BASE - 0x1000)
#define CODEC_FIR_HISTORY_CH2_BASE                      (CODEC_FIR_CH2_BASE - 0x1000)
#define CODEC_FIR_HISTORY_CH3_BASE                      (CODEC_FIR_CH3_BASE - 0x1000)

#ifdef CODEC_MIN_PHASE
static volatile int32_t * const codec_fir_ch0 = (int32_t *)CODEC_FIR_CH0_BASE;
static volatile int32_t * const codec_fir_ch1 = (int32_t *)CODEC_FIR_CH1_BASE;
static volatile int32_t * const codec_fir_ch2 = (int32_t *)CODEC_FIR_CH2_BASE;
static volatile int32_t * const codec_fir_ch3 = (int32_t *)CODEC_FIR_CH3_BASE;
static volatile int32_t * const codec_fir_history0 = (int32_t *)CODEC_FIR_HISTORY_CH0_BASE;
static volatile int32_t * const codec_fir_history1 = (int32_t *)CODEC_FIR_HISTORY_CH1_BASE;
static volatile int32_t * const codec_fir_history2 = (int32_t *)CODEC_FIR_HISTORY_CH2_BASE;
static volatile int32_t * const codec_fir_history3 = (int32_t *)CODEC_FIR_HISTORY_CH3_BASE;

#define MAX_FIR_ORDERS (512)

#define FIR_LOW_FREQ

#ifdef FIR_LOW_FREQ
#define FIR_DAC_ORDERS (220)
#define FIR_ADC_ORDERS (220)
#else
#define FIR_DAC_ORDERS (512)
#define FIR_ADC_ORDERS (512)
#endif

STATIC_ASSERT(FIR_DAC_ORDERS <= MAX_FIR_ORDERS, "FIR_DAC_ORDERS too large");
STATIC_ASSERT(FIR_ADC_ORDERS <= MAX_FIR_ORDERS, "FIR_ADC_ORDERS too large");

POSSIBLY_UNUSED const static int32_t fir_coef_linear[FIR_DAC_ORDERS] = {
    524287,
};

POSSIBLY_UNUSED const static int32_t fir_coef_minimun[FIR_DAC_ORDERS] = {
#ifdef FIR_LOW_FREQ
    171,1077,4203,12582,31473,68652,133899,237244,385800,579602,807418,1044189,1251727,1383990,1396843,1260694,972640,563948,
    99101,-335381,-650923,-780973,-701927,-443652,-84578,268808,512951,579186,455906,192655,-116310,-364122,-468309,-399443,
    -190845,74646,296174,392908,334154,149860,-82526,-269615,-339392,-269328,-94001,110604,259377,293401,203556,32780,-143605,
    -250723,-245944,-136029,27365,171442,234497,192404,67901,-80955,-187545,-206431,-132784,-3152,122291,187500,165597,69954,
    -53253,-147114,-170196,-114838,-9444,95806,152719,137136,59154,-42785,-120684,-139533,-93024,-5397,80987,126029,110487,
    44131,-39952,-101741,-113290,-71331,2185,71419,104055,86354,28846,-39643,-86369,-90279,-51459,9830,63615,84824,64943,15296,
    -39213,-72539,-69912,-34308,15781,55890,67480,46479,4496,-37461,-59466,-52097,-20389,19303,47689,51882,31200,-3160,-34103,
    -47104,-36994,-9893,20334,39133,38235,19235,-7752,-29414,-35781,-24769,-2693,19264,30682,26817,10499,-9713,-23968,-25906,
    -15440,1619,16733,22882,17788,4668,-9693,-18412,-17788,-8810,3652,13454,16181,11101,1226,-8416,-13305,-11533,-4481,4101,
    10062,10834,6508,-446,-6545,-9016,-7036,-1934,3627,7021,6880,3613,-961,-4595,-5701,-4025,-619,2769,4591,4179,1965,-839,
    -2887,-3327,-2140,-46,1903,2850,2488,1144,-458,-1565,-1740,-1020,170,1245,1754,1547,815,-53,-656,-761,-376,288,927,1290,
    1257,878,307,-256,-663,-840,-809,-642,-427,-237,-103,-33,
#else
    358,2134,7870,22383,53324,110934,206479,349044,541000,773159,1021370,1246556,1399740,1432299,1309745,1025462,609633,129002,
    -324731,-656532,-794417,-712592,-443054,-70120,292346,535048,587990,443750,160328,-158614,-400055,-481834,-382027,
    -146176,130807,341515,407598,309695,93804,-148511,-318068,-348626,-233320,-26063,182493,304084,290262,152119,-45991,
    -215703,-283797,-223953,-67174,113414,236243,248077,147280,-16729,-167204,-235765,-193530,-63427,91650,198950,209932,
    122330,-19887,-148250,-202614,-159367,-41765,92191,178176,176391,90055,-37106,-142792,-176569,-123965,-12686,101744,
    163572,143971,54527,-59287,-141093,-151499,-87091,18628,112527,148481,110037,17958,-80830,-137074,-123736,-49016,48438,
    119432,129027,73750,-17278,-97589,-127045,-91900,-11220,73360,119064,103602,36056,-48313,-106407,-109292,-56617,23738,
    90354,109596,72600,-665,-72105,-105274,-83967,-20135,52732,97141,90877,38119,-33174,-86035,-93649,-52950,14213,72765,
    92702,64465,3511,-58099,-88538,-72655,-19509,42731,81690,77624,33421,-27283,-72714,-79582,-45018,12285,62151,78801,54174,
    1818,-50528,-75615,-60867,-14676,38325,70381,65146,26015,-25988,-63481,-67140,-35653,13901,55293,67020,43473,-2402,-46194,
    -65008,-49433,-8237,36534,61346,53542,17793,-26648,-56304,-55870,-26107,16829,50150,56518,33065,-7346,-43163,-55631,-38609,
    -1582,35606,53375,42719,9762,-27735,-49938,-45423,-17053,19783,45514,46773,23343,-11967,-40312,-46863,-28563,4471,34530,45798,
    32673,2539,-28370,-43713,-35673,-8936,22017,40746,37582,14610,-15648,-37053,-38453,-19489,9418,32783,38351,23519,-3470,-28096,
    -37370,-26681,-2079,23137,35604,28971,7128,-18054,-33170,-30413,-11603,12974,30179,31045,15443,-8023,-26756,-30924,-18617,3302,
    23015,30118,21105,1094,-19077,-28707,-22913,-5093,15046,26776,24054,8635,-11031,-24419,-24565,-11680,7118,21726,24486,14198,-3394,
    -18792,-23876,-16181,-76,15704,22793,17629,3230,-12551,-21309,-18559,-6028,9408,19490,18994,8433,-6352,-17414,-18974,-10431,3440,
    15149,18537,12008,-731,-12769,-17737,-13172,-1735,10336,16622,13932,3920,-7916,-15252,-14311,-5805,5559,13680,14335,7371,-3319,-11967,
    -14040,-8615,1232,10161,13463,9538,664,-8320,-12648,-10154,-2348,6485,11634,10474,3797,-4703,-10470,-10527,-5006,3007,9196,10334,5966,
    -1433,-7856,-9930,-6686,1,6486,9343,7168,1266,-5126,-8610,-7432,-2359,3803,7762,7491,3267,-2551,-6835,-7369,-3991,1387,5858,7087,4531,
    -335,-4863,-6673,-4898,-597,3875,6149,5100,1395,-2920,-5545,-5154,-2058,2015,4882,5073,2582,-1181,-4187,-4878,-2974,427,3479,4586,3237,
    234,-2781,-4219,-3383,-799,2107,3793,3420,1262,-1476,-3331,-3363,-1628,894,2845,3224,1895,-375,-2357,-3021,-2074,-79,1875,2764,2167,459,
    -1416,-2470,-2186,-771,987,2151,2139,1009,-599,-1823,-2040,-1182,254,1492,1894,1290,40,-1172,-1717,-1342,-286,869,1516,1343,480,-592,-1303,
    -1303,-626,342,1084,1226,725,-127,-870,-1124,-784,-55,664,1001,803,200,-475,-869,-794,-313,303,729,756,392,-154,-592,-701,-443,28,458,630,468,74,
    -337,-553,-473,-155,226,471,462,215,-130,-394,-443,-263,45,319,420,302,31,-253,-404,-349,-115,181,394,418,237,-74,-375,-536,-472,-196,214,626,935,
    1062,1013,824,581,341,166,55,
#endif
};

static uint8_t min_phase_cfg;
#endif

#ifdef PSAP_APP
static const struct HAL_CODEC_PSAP_CFG_T psap_cfg = {
    .mode = HAL_CODEC_PSAP_MODE_ADC,
    .din0_samp_delay = 0,
    .din0_gain = 1.0f,
    .din1_sign = 0,
};
#endif

static bool codec_init = false;
static bool codec_opened = false;

static int8_t digdac_gain[MAX_DAC_CH_NUM];
static int8_t digadc_gain[NORMAL_ADC_CH_NUM];

static bool codec_mute[AUD_STREAM_NUM];

static int8_t digdac2_gain[MAX_DAC2_CH_NUM];
static int8_t digdac3_gain[MAX_DAC3_CH_NUM];
static bool codec_mute2;
static bool codec_mute3;

#ifdef AUDIO_OUTPUT_SWAP
static bool output_swap = true;
#endif

#ifdef ANC_APP
static float anc_boost_gain_attn;
static int8_t anc_adc_gain_offset[NORMAL_ADC_CH_NUM];
static enum AUD_CHANNEL_MAP_T anc_adc_gain_offset_map;
#endif
#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
static bool codec_nr_enabled;
static int8_t digdac_gain_offset_nr;
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB
static int32_t dac_dc_l;
static int32_t dac_dc_r;
static float dac_dc_gain_attn;
#endif
#ifdef __AUDIO_RESAMPLE__
static uint8_t resample_rate_idx[AUD_STREAM_NUM];
static uint8_t dac2_resample_rate_idx;
static uint8_t dac3_resample_rate_idx;
static uint8_t adc2_resample_rate_idx;
#endif
#ifdef CODEC_TIMER
static uint32_t cur_codec_freq;
#endif

static uint8_t codec_rate_idx[AUD_STREAM_NUM];

static uint8_t codec_adc2_rate_idx;
static uint8_t codec_dac2_rate_idx;
static uint8_t codec_dac3_rate_idx;

static uint8_t dac_rs_clk_user_map = 0;
static uint8_t adc_rs_clk_user_map = 0;

static uint32_t dac_rs_clk_speed[RS_CLK_USER_QTY] = {0};
static uint32_t adc_rs_clk_speed[RS_CLK_USER_QTY2] = {0};

//static HAL_CODEC_DAC_RESET_CALLBACK dac_reset_callback;

static uint8_t codec_ci2s_idx = 0;

static uint8_t codec_irq_map;
STATIC_ASSERT(sizeof(codec_irq_map) * 8 >= CODEC_IRQ_TYPE_QTY, "codec_irq_map size too small");
static HAL_CODEC_IRQ_CALLBACK2 codec_irq_callback[CODEC_IRQ_TYPE_QTY];

static enum AUD_CHANNEL_MAP_T codec_dac_ch_map;
static enum AUD_CHANNEL_MAP_T codec_dac2_ch_map;
static enum AUD_CHANNEL_MAP_T codec_dac3_ch_map;
static enum AUD_CHANNEL_MAP_T codec_adc_ch_map;
static enum AUD_CHANNEL_MAP_T codec_adc2_ch_map;
static enum AUD_CHANNEL_MAP_T anc_adc_ch_map;
static enum AUD_CHANNEL_MAP_T codec_mic_ch_map;
static enum AUD_CHANNEL_MAP_T codec_mic2_ch_map;
static enum AUD_CHANNEL_MAP_T anc_mic_ch_map;
#ifdef SIDETONE_DEDICATED_ADC_CHAN
static enum AUD_CHANNEL_MAP_T sidetone_adc_ch_map;
static int8_t sidetone_adc_gain;
static int8_t sidetone_gain_offset;
#ifdef CFG_HW_AUD_SIDETONE_GAIN_RAMP
static float sidetone_ded_chan_coef;
#endif
#endif

#if defined(DIG_MIC_DYN_CODEC_VOLT) && defined(ANC_DYN_CODEC_VOLT)
enum CODEC_VOLT_RAMP_USER_T vcodec_ramp_user;
#endif

#if defined(CODEC_ADC1_DSF) || defined(CODEC_ADC2_DSF)
static int adc_dsf_clk_speed[ADC_DSF_ID_QTY];
static struct CODEC_ADC_DSF_CFG_T adc_dsf_cfg;

static void hal_codec_adc_dsf_clock_enable(enum ADC_DSF_ID_T id, bool enable);
static int hal_codec_adc_dsf_set_speed(enum ADC_DSF_ID_T id, int speed);
static uint32_t hal_codec_adc_dsf_get_speed(enum ADC_DSF_ID_T id);

#ifdef CODEC_ADC1_DSF
static bool adc1_dsf_status = false;
static struct CODEC_ADC_DSF_SAMPRATE_T adc1_dsf_sample_rate[] = {
    {AUD_SAMPRATE_NULL,  0, 0, 0, 0},
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_101562, 1, 1, 0, 1}, //384 / 1 / 4 = 96 / 1 = 96K
    {AUD_SAMPRATE_50781,  1, 0, 0, 1}, //384 / 1 / 8 = 48 / 1 = 48K
    {AUD_SAMPRATE_16927,  1, 0, 0, 3}, //384 / 1 / 8 = 48 / 3 = 16K
    {AUD_SAMPRATE_8463,   1, 0, 0, 6}, //384 / 1 / 8 = 48 / 6 = 8K
#endif
    {AUD_SAMPRATE_8000,   1, 0, 0, 6}, //384 / 1 / 8 = 48 / 6 = 8K
    {AUD_SAMPRATE_16000,  1, 0, 0, 3}, //384 / 1 / 8 = 48 / 3 = 16K
    {AUD_SAMPRATE_32000,  1, 1, 0, 3}, //384 / 1 / 4 = 96 / 3 = 32K
    {AUD_SAMPRATE_44100,  1, 0, 0, 1}, //384 / 1 / 8 = 48 / 1 = 48K
    {AUD_SAMPRATE_48000,  1, 0, 0, 1}, //384 / 1 / 8 = 48 / 1 = 48K
    {AUD_SAMPRATE_96000,  1, 1, 0, 1}, //384 / 1 / 4 = 96 / 1 = 96K
};

static void hal_codec_adc1_dsf_setup_param(enum AUD_SAMPRATE_T sample_rate);
static void hal_codec_adc1_dsf_start_interface(void);
static void hal_codec_adc1_dsf_stop_interface(void);
#endif

#ifdef CODEC_ADC2_DSF
static bool adc2_dsf_status = false;
static struct CODEC_ADC_DSF_SAMPRATE_T adc2_dsf_sample_rate[] = {
    {AUD_SAMPRATE_NULL,  0, 0, 0, 0},
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_101562, 1, 1, 1, 1}, // 384 / 1 / 4 = 96 / 1 = 96K, rate_sel=1, use 96K
    {AUD_SAMPRATE_50781,  1, 1, 1, 2}, // 384 / 1 / 4 = 96 / 2 = 48K
    {AUD_SAMPRATE_16927,  1, 1, 1, 6}, // 384 / 1 / 4 = 96 / 6 = 16K
#endif
    {AUD_SAMPRATE_16000,  1, 1, 1, 6}, // 384 / 1 / 4 = 96 / 6 = 16K
    {AUD_SAMPRATE_32000,  1, 1, 1, 3}, // 384 / 1 / 4 = 96 / 3 = 32K
    {AUD_SAMPRATE_48000,  1, 1, 1, 2}, // 384 / 1 / 4 = 96 / 2 = 48K
    {AUD_SAMPRATE_96000,  1, 1, 1, 1}, // 384 / 1 / 4 = 96 / 1 = 96K, rate_sel=1, use 96K
    {AUD_SAMPRATE_192000, 1, 0, 2, 1}, // 384 / 1 / 8 = 48, 192 / 1 = 192K, rate_sel=2,use 192K
    {AUD_SAMPRATE_384000, 1, 0, 3, 1}, // 384 / 1 / 8 = 48, 384 / 1 = 384K, rate_sel=3,use 384K
};

static void hal_codec_adc2_dsf_setup_param(enum AUD_SAMPRATE_T sample_rate);
static void hal_codec_adc2_dsf_start_interface(void);
static void hal_codec_adc2_dsf_stop_interface(void);
#ifdef CODEC_ADC2_DUMP_DATA
static int hal_codec_adc2_setup_data_path(int path);
#endif
#endif
#endif /* #if defined(CODEC_ADC1_DSF) || defined(CODEC_ADC2_DSF) */

#if defined(CODEC_DAC1_USF) || defined(CODEC_DAC2_USF) || defined(CODEC_DAC3_USF)
static int dac_usf_clk_cur_speed = 0;
static int dac_usf_clk_speed[HAL_CODEC_USF_QTY];
#endif

#if defined(CODEC_DAC1_USF)
static struct CODEC_DAC_USF_SAMPRATE_T dac1_usf_sample_rate[] = {
    {AUD_SAMPRATE_NULL,  0, 0, 0, 0},
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_101562, 1, 1, 1, 0}, //48*1*1*4
    {AUD_SAMPRATE_50781, 1, 1, 0, 0}, //48*1*1*8
    {AUD_SAMPRATE_44100, 1, 1, 0, 0}, //48*1*1*8
#endif
    {AUD_SAMPRATE_8000,  6, 1, 0, 0}, //8*6*1*8
    {AUD_SAMPRATE_16000, 3, 1, 0, 0}, //16*3*1*8
    {AUD_SAMPRATE_32000, 3, 1, 1, 0}, //32*3*1*4
    {AUD_SAMPRATE_48000, 1, 1, 0, 0}, //48*1*1*8
    {AUD_SAMPRATE_96000, 1, 1, 1, 0}, //96*1*1*4
};

static int dac1_usf_sr_idx = 0;
static bool dac1_usf_status = false;
#endif

#if defined(CODEC_DAC2_USF)
static struct CODEC_DAC_USF_SAMPRATE_T dac2_usf_sample_rate[] = {
    {AUD_SAMPRATE_NULL,  0, 0, 0, 0},
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_44100, 1, 1, 0, 0}, //48*1*1*8
    {AUD_SAMPRATE_50781, 1, 1, 0, 0}, //48*1*1*8
    {AUD_SAMPRATE_101562, 1, 1, 1, 0}, //48*1*1*4
#endif
    {AUD_SAMPRATE_8000,  6, 1, 0, 0}, //8*3*1*8
    {AUD_SAMPRATE_16000, 3, 1, 0, 0}, //16*3*1*8
    {AUD_SAMPRATE_48000, 1, 1, 0, 0}, //48*1*1*8
};

static int dac2_usf_sr_idx = 0;
static bool dac2_usf_status = false;
#endif

#if defined(CODEC_DAC3_USF)
static struct CODEC_DAC_USF_SAMPRATE_T dac3_usf_sample_rate[] = {
    {AUD_SAMPRATE_NULL,  0, 0, 0, 0},
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_44100, 1, 1, 0, 0}, //48*1*1*8
    {AUD_SAMPRATE_50781, 1, 1, 0, 0}, //48*1*1*8
    {AUD_SAMPRATE_101562, 1, 1, 1, 0}, //48*1*1*4
#endif
    {AUD_SAMPRATE_8000,  6, 1, 0, 0}, //8*3*1*8
    {AUD_SAMPRATE_16000, 3, 1, 0, 0}, //16*3*1*8
    {AUD_SAMPRATE_48000, 1, 1, 0, 0}, //48*1*1*8
};

static int dac3_usf_sr_idx = 0;
static bool dac3_usf_status = false;
#endif

enum CODEC_DAC_LH_USER_T {
    CODEC_DAC_LH_USER_PSAP,
    CODEC_DAC_LH_USER_MC,
    CODEC_DAC_LH_USER_DAC1,
    CODEC_DAC_LH_USER_DAC2,
    CODEC_DAC_LH_USER_DAC3,

    CODEC_DAC_LH_USER_QTY
};

static uint8_t codec_dac_lh_user_map = 0;

enum DIG_MIC_USER_T {
    DIG_MIC_USER_ADC1,
    DIG_MIC_USER_ADC2,
    DIG_MIC_USER_ANC,
};

static uint8_t dig_mic_user_map = 0;

#if defined(ANC_APP) && defined(ANC_TT_MIC_CH_L)
static enum AUD_CHANNEL_MAP_T anc_tt_adc_ch_l;
static enum AUD_CHANNEL_MAP_T anc_tt_adc_ch_r;
static enum ANC_TYPE_T anc_tt_mic_l_map;
static enum ANC_TYPE_T anc_tt_mic_r_map;
#endif

#ifdef ANC_PROD_TEST
#define FORCE_TT_ADC_ALLOC              true
#define OPT_TYPE
#else
#define FORCE_TT_ADC_ALLOC              false
#define OPT_TYPE                        const
#endif

static OPT_TYPE uint8_t codec_digmic_phase = CODEC_DIGMIC_PHASE;

#if defined(AUDIO_ANC_FB_MC) && defined(__AUDIO_RESAMPLE__)
#error "Music cancel cannot work with audio resample"
#endif
#ifdef AUDIO_ANC_FB_MC
static bool mc_enabled;
static bool mc_started;
static enum AUD_CHANNEL_NUM_T mc_chan_num;
static enum AUD_BITS_T mc_bits;
#endif

#ifdef CODEC_DSD
static bool dsd_enabled;
static uint8_t dsd_rate_idx;
#endif

#if defined(AUDIO_ANC_FB_MC) || defined(CODEC_DSD)
static uint8_t adc_en_map;
STATIC_ASSERT(sizeof(adc_en_map) * 8 >= CODEC_ADC_EN_REQ_QTY, "adc_en_map size too small");
#endif

#ifdef PERF_TEST_POWER_KEY
static enum HAL_CODEC_PERF_TEST_POWER_T cur_perft_power;
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
static int8_t swdac_gain;
static HAL_CODEC_SW_OUTPUT_COEF_CALLBACK sw_output_coef_callback;
#endif

#ifdef AUDIO_OUTPUT_DAC2_SW_GAIN
static int8_t swdac2_gain;
static HAL_CODEC_SW_OUTPUT_COEF_CALLBACK sw_output_coef_callback2;
#endif

#ifdef AUDIO_OUTPUT_DAC3_SW_GAIN
static int8_t swdac3_gain;
static HAL_CODEC_SW_OUTPUT_COEF_CALLBACK sw_output_coef_callback3;
#endif

static HAL_CODEC_BT_TRIGGER_CALLBACK bt_trigger_callback[BT_TRIGGER_NUM];
static HAL_CODEC_EVENT_TRIGGER_CALLBACK event_trigger_callback = NULL;
static HAL_CODEC_TIMER_TRIGGER_CALLBACK timer_trigger_callback = NULL;
static HAL_CODEC_IRQ_CALLBACK anc_fb_callback = NULL;

#if defined(DAC_CLASSG_ENABLE)
static struct dac_classg_cfg _dac_classg_cfg = {
    .thd2 = 0xC0,
    .thd1 = 0x10,
    .thd0 = 0x10,
    .lr = 1,
    .step_4_3n = 0,
    .quick_down = 1,
    .wind_width = 0x400,
};
#endif

#ifdef DAC_DRE_ENABLE
//#define DRE_DBG
#define DRE_MAX_INI_ANA_GAIN_FIRST
struct CODEC_DAC_DRE_CFG_T dac_dre_cfg = {
    // 350
    .step_mode = DRE_STEP_MODE_STEP_16,
    .ini_ana_gain = 0,
    .dre_delay_lh = 0x15,
    .dre_delay_hl = 0x15,
    .amp_high = 0x800,
    // 354
    .dre_win = 0x3000, //16 ms, 1.536Mhz
    .thd_db_offset_sign = 0,
    .thd_db_offset = 0,
    .gain_offset = 2,
    // 358
    .db_high = 48, //-42-0.75*48 = -78 dBFS
    .db_low  = 48, //-42-0.75*48 = -78 dBFS
    .top_gain = 0xF,
    .delay_dc_lh = 8,
    .delay_dc_hl = 8,
};
#endif

#if defined(DAC_DRE_ENABLE) || (defined(AUDIO_OUTPUT_DC_CALIB) && defined(AUDIO_OUTPUT_DC_AUTO_CALIB))
static struct HAL_CODEC_DAC_DRE_CALIB_CFG_T dac_dre_calib_cfg[] = {
    {
        .valid = 0,
        .dc_l = 0,
        .dc_r = 0,
        .gain_l = 1.0,
        .gain_r = 1.0,
        .ana_dc_l = 0,
        .ana_dc_r = 0,
        .ana_gain = 0x13,
        .ini_ana_gain = 0,
        .gain_offset = 4,
        .step_mode = DRE_STEP_MODE_STEP_16,
        .top_gain = 15,
    },
    {
        .valid = 0,
        .dc_l = 0,
        .dc_r = 0,
        .gain_l = 3.652,
        .gain_r = 3.652,
        .ana_dc_l = 0,
        .ana_dc_r = 0,
        .ana_gain = 4,
        .ini_ana_gain = 15,
        .gain_offset = 4,
        .step_mode = DRE_STEP_MODE_STEP_16,
        .top_gain = 15,
    },
};
#endif

#ifdef CODEC_SW_SYNC
static bool codec_sw_sync_play_status[CODEC_HW_MOD_DAC_QTY] = {false,false,false};
static bool codec_sw_sync_cap_status[CODEC_HW_MOD_ADC_QTY] = {false,false};
#endif

static void hal_codec_set_dig_adc_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain);
static void hal_codec_set_dig_dac_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain);
static void hal_codec_set_dig_dac2_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain);
static void hal_codec_set_dig_dac3_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain);
static void hal_codec_restore_dig_dac_gain(void);
static void hal_codec_restore_dig_dac2_gain(void);
static void hal_codec_restore_dig_dac3_gain(void);
static void hal_codec_set_dac_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val);
static void hal_codec_set_dac2_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val);
static void hal_codec_set_dac3_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val);
static int hal_codec_set_adc_down(enum AUD_CHANNEL_MAP_T map, uint32_t val);
static int hal_codec_set_adc_hbf_bypass_cnt(enum AUD_CHANNEL_MAP_T map, uint32_t cnt);
static uint32_t hal_codec_get_adc_chan(enum AUD_CHANNEL_MAP_T mic_map,
    enum AUD_CHANNEL_MAP_T mic_chan_map, enum AUD_CHANNEL_MAP_T adc_chan_map, uint8_t adc_chan_num);


#ifdef AUDIO_OUTPUT_SW_GAIN
static void hal_codec_set_sw_gain(int32_t gain);
#endif
#ifdef AUDIO_OUTPUT_DAC2_SW_GAIN
static void hal_codec_set_dac2_sw_gain(int32_t gain);
#endif
#ifdef AUDIO_OUTPUT_DAC3_SW_GAIN
static void hal_codec_set_dac3_sw_gain(int32_t gain);
#endif
#ifdef __AUDIO_RESAMPLE__
static float get_playback_resample_phase(void);
static float get_playback2_resample_phase(void);
static float get_playback3_resample_phase(void);
static float get_capture_resample_phase(void);
static float get_capture2_resample_phase(void);
static uint32_t resample_phase_float_to_value(float phase);
static float resample_phase_value_to_float(uint32_t value);
#endif
#ifdef AUDIO_OUTPUT_DC_AUTO_CALIB
static bool hal_codec_get_dig_dc_calib_value(int32_t *dc_l, int32_t *dc_r);
static bool hal_codec_set_ana_dc_calib_value(void);
#endif
static void hal_codec_dac_dre_init(void);
static void hal_codec_ec_gain_init(void);

static void hal_codec_reg_update_delay(void)
{
    hal_sys_timer_delay_us(2);
}

#if defined(DAC_CLASSG_ENABLE)
void hal_codec_classg_config(const struct dac_classg_cfg *cfg)
{
    _dac_classg_cfg = *cfg;
}

static void hal_codec_classg_enable(bool en)
{
    struct dac_classg_cfg *config;

    if (en) {
        config = &_dac_classg_cfg;

        codec->REG_14C = SET_BITFIELD(codec->REG_14C, CODEC_CODEC_CLASSG_THD2, config->thd2);
        codec->REG_14C = SET_BITFIELD(codec->REG_14C, CODEC_CODEC_CLASSG_THD1, config->thd1);
        codec->REG_14C = SET_BITFIELD(codec->REG_14C, CODEC_CODEC_CLASSG_THD0, config->thd0);

        // Make class-g set the lowest gain after several samples.
        // Class-g gain will have impact on dc.
        codec->REG_148 = SET_BITFIELD(codec->REG_148, CODEC_CODEC_CLASSG_WINDOW, 6);

        if (config->lr)
            codec->REG_148 |= CODEC_CODEC_CLASSG_LR;
        else
            codec->REG_148 &= ~CODEC_CODEC_CLASSG_LR;

        if (config->step_4_3n)
            codec->REG_148 |= CODEC_CODEC_CLASSG_STEP_4_3N;
        else
            codec->REG_148 &= ~CODEC_CODEC_CLASSG_STEP_4_3N;

        if (config->quick_down)
            codec->REG_148 |= CODEC_CODEC_CLASSG_QUICK_DOWN;
        else
            codec->REG_148 &= ~CODEC_CODEC_CLASSG_QUICK_DOWN;

        codec->REG_148 |= CODEC_CODEC_CLASSG_EN;

        // Restore class-g window after the gain has been updated
        hal_codec_reg_update_delay();
        codec->REG_148 = SET_BITFIELD(codec->REG_148, CODEC_CODEC_CLASSG_WINDOW, config->wind_width);
    } else {
        codec->REG_148 &= ~CODEC_CODEC_CLASSG_QUICK_DOWN;
    }
}
#endif

void hal_codec_adc_dc_offset_enable(uint32_t adc_chan, int32_t dc)
{
    uint32_t i;

    for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (adc_chan & (1<<i)) {
            volatile uint32_t *reg = (volatile uint32_t *)&(codec->REG_18C) + i;
            *reg = *reg & (~(CODEC_CODEC_ADC_DC_UPDATE_CH0));
            hal_codec_reg_update_delay();
            *reg = SET_BITFIELD(*reg, CODEC_CODEC_ADC_DC_DIN_CH0, dc) | CODEC_CODEC_ADC_DC_UPDATE_CH0;
        }
    }
}

void hal_codec_adc_dc_offset_init(void)
{
#ifdef AUDIO_INPUT_CH0_DC_OFFSET
    hal_codec_adc_dc_offset_enable(AUD_CHANNEL_MAP_CH0, AUDIO_INPUT_CH0_DC_OFFSET);
#endif
#ifdef AUDIO_INPUT_CH1_DC_OFFSET
    hal_codec_adc_dc_offset_enable(AUD_CHANNEL_MAP_CH1, AUDIO_INPUT_CH1_DC_OFFSET);
#endif
#ifdef AUDIO_INPUT_CH2_DC_OFFSET
    hal_codec_adc_dc_offset_enable(AUD_CHANNEL_MAP_CH2, AUDIO_INPUT_CH2_DC_OFFSET);
#endif
#ifdef AUDIO_INPUT_CH3_DC_OFFSET
    hal_codec_adc_dc_offset_enable(AUD_CHANNEL_MAP_CH3, AUDIO_INPUT_CH3_DC_OFFSET);
#endif
#ifdef AUDIO_INPUT_CH4_DC_OFFSET
    hal_codec_adc_dc_offset_enable(AUD_CHANNEL_MAP_CH4, AUDIO_INPUT_CH4_DC_OFFSET);
#endif
#ifdef AUDIO_INPUT_CH5_DC_OFFSET
    hal_codec_adc_dc_offset_enable(AUD_CHANNEL_MAP_CH5, AUDIO_INPUT_CH5_DC_OFFSET);
#endif
}

void hal_codec_dac_dc_offset_enable(int32_t dc_l, int32_t dc_r)
{
    uint16_t dac_dev = analog_codec_get_dac_dev();
    uint32_t chip_dc_val = 0;
    uint32_t pmu_dc_val_l = dc_l, pmu_dc_val_r = dc_r;

    if (GET_BITFIELD(dac_dev, PMU_CODEC_DAC)) {
        if(dac_dev == (MCU_CODEC_DAC_L | PMU_CODEC_DAC_L))
        {
            pmu_dc_val_l = dc_r;
            pmu_dc_val_r = dc_l;
        }
        analog1809_aud_dac_dc_offset_enable(pmu_dc_val_l, pmu_dc_val_r);
    }
    if (GET_BITFIELD(dac_dev, MCU_CODEC_DAC)) {
        codec->REG_1B8 &= ~CODEC_CODEC_DAC_DC_UPDATE_CH0;
        hal_codec_reg_update_delay();
        if (dac_dev & MCU_CODEC_DAC_L) {
            chip_dc_val = dc_l;
        } else {
            chip_dc_val = dc_r;
        }
        codec->REG_1B8 = SET_BITFIELD(codec->REG_1B8, CODEC_CODEC_DAC_DC_CH0, chip_dc_val) | CODEC_CODEC_DAC_DC_UPDATE_CH0;
    }
}

int hal_codec_adc_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    codec->REG_070 = SET_BITFIELD(codec->REG_070, CODEC_CFG_DIV_CODEC, div);
    int_unlock(lock);

    return 0;
}

uint32_t hal_codec_adc_get_div(void)
{
    return GET_BITFIELD(codec->REG_070, CODEC_CFG_DIV_CODEC) + 2;
}

int hal_codec_dac_set_div(uint32_t div)
{
    return hal_codec_adc_set_div(div);
}

uint32_t hal_codec_dac_get_div(void)
{
    return hal_codec_adc_get_div();;
}

static void hal_codec_select_audio_pll(void)
{
    uint32_t val;
    enum HAL_CMU_PLL_T pll;

    pll = hal_cmu_get_audio_pll();
    if (pll == HAL_CMU_PLL_BB) {
        val = 0;
#ifndef CHIP_SUBSYS_SENS
    } else if (pll == HAL_CMU_PLL_USB) {
        val = 1;
#endif
    } else {
        val = 2;
    }
    codec->REG_074 = SET_BITFIELD(codec->REG_074, CODEC_SEL_PLL_AUD, val);
}

static void hal_codec_select_audio_clock(void)
{
#ifdef CODEC_CLK_FROM_ANA
    // CODEC use clk from analog module
    codec->REG_074 &= ~CODEC_SEL_CLK_PLL_CODEC;
#else
#ifdef __AUDIO_RESAMPLE__
    if (hal_cmu_get_audio_resample_status()) {
        codec->REG_070 &= ~CODEC_SEL_PLL_CODEC;
        // Select 12M mode by default
        codec->REG_074 |= CODEC_SEL_OSC_CODEC;

    }
    else
#endif
    {
        codec->REG_070 |= CODEC_SEL_PLL_CODEC;
    }
    // CODEC use clock from digital module
    codec->REG_074 |= CODEC_SEL_CLK_PLL_CODEC;
#endif
}

void hal_codec_high_speed_enable(void)
{
    uint32_t div;

    if ((codec->REG_07C & CODEC_CODEC_CLKX2_EN) == 0) {
        codec->REG_07C |= CODEC_CODEC_CLKX2_EN;
        codec->REG_074 &= ~CODEC_SEL_OSC_CODEC;

        div = hal_codec_adc_get_div();
        hal_codec_adc_set_div(div / 2);
    }
}

void hal_codec_high_speed_disable(void)
{
    uint32_t div;

    if (codec->REG_07C & CODEC_CODEC_CLKX2_EN) {
        codec->REG_07C &= ~CODEC_CODEC_CLKX2_EN;
        codec->REG_074 |= CODEC_SEL_OSC_CODEC;

        div = hal_codec_adc_get_div();
        hal_codec_adc_set_div(div * 2);
    }
}

void hal_codec_iir_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;
    uint32_t cfg_speed = 0;

    mask = CODEC_SEL_OSC_ANCIIR | CODEC_SEL_OSCX2_ANCIIR | CODEC_SEL_OSCX4_ANCIIR | CODEC_BYPASS_DIV_CODEC_ANCIIR;
    val = 0;

    if (speed <= 24000000) {
        val |= CODEC_SEL_OSC_ANCIIR | CODEC_SEL_OSCX2_ANCIIR;
        cfg_speed = 24000000;
    } else if (speed <= 48000000) {
        val |= CODEC_SEL_OSCX2_ANCIIR;
        cfg_speed = 48000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (hal_cmu_get_audio_resample_status()) {
            val |= CODEC_SEL_OSCX4_ANCIIR;
            cfg_speed = 96000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = CODEC_PLL_CLOCK / speed;
            if (div >= 2) {
                lock = int_lock();
                codec->REG_074 = SET_BITFIELD(codec->REG_074, CODEC_CFG_DIV_CODEC_ANCIIR, (div - 2));
                int_unlock(lock);
                cfg_speed = CODEC_PLL_CLOCK / div;
            } else {
                val |= CODEC_BYPASS_DIV_CODEC_ANCIIR;
                cfg_speed = CODEC_PLL_CLOCK;
            }
        }
    }
    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    pmu_iir_freq_config(cfg_speed);

    lock = int_lock();
    codec->REG_074 = (codec->REG_074 & ~mask) | val;
    codec->REG_06C |= CODEC_EN_CLK_IIR_ANC_MASK;
    codec->REG_07C |= CODEC_SOFT_RSTN_IIR_ANC_MASK;
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

void hal_codec_iir_disable(void)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;

    mask = CODEC_SEL_OSC_ANCIIR | CODEC_SEL_OSCX2_ANCIIR | CODEC_SEL_OSCX4_ANCIIR | CODEC_BYPASS_DIV_CODEC_ANCIIR;
    val = CODEC_SEL_OSC_ANCIIR | CODEC_SEL_OSCX2_ANCIIR;

    lock = int_lock();
    codec->REG_07C &= ~CODEC_SOFT_RSTN_IIR_ANC_MASK;
    codec->REG_06C &= ~CODEC_EN_CLK_IIR_ANC_MASK;
    codec->REG_074 = (codec->REG_074 & ~mask) | val;
    int_unlock(lock);

    pmu_iir_freq_config(0);
}

void hal_codec_iir_eq_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;
    uint32_t cfg_speed = 0;

    mask = CODEC_SEL_OSCX4_EQIIR | CODEC_SEL_OSCX2_EQIIR | CODEC_SEL_OSC_EQIIR | CODEC_BYPASS_DIV_CODEC_EQIIR | CODEC_SEL_OSC_4_EQIIR | CODEC_SEL_OSC_2_EQIIR;
    val = 0;

    if (speed <= 6000000) {
        val |= CODEC_SEL_OSC_4_EQIIR | CODEC_SEL_OSC_2_EQIIR | CODEC_SEL_OSCX2_EQIIR;
        cfg_speed = 6000000;
    } else if (speed <= 12000000) {
        val |= CODEC_SEL_OSC_2_EQIIR | CODEC_SEL_OSCX2_EQIIR;
        cfg_speed = 12000000;
    } else if (speed <= 24000000) {
        val |= CODEC_SEL_OSC_EQIIR | CODEC_SEL_OSCX2_EQIIR;
        cfg_speed = 24000000;
    } else if (speed <= 48000000) {
        val |= CODEC_SEL_OSCX2_EQIIR;
        cfg_speed = 48000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (hal_cmu_get_audio_resample_status()) {
            val |= CODEC_SEL_OSCX4_EQIIR;
            cfg_speed = 96000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = CODEC_PLL_CLOCK / speed;
            if (div >= 2) {
                lock = int_lock();
                codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_CFG_DIV_CODEC_EQIIR, (div - 2));
                int_unlock(lock);
                cfg_speed = CODEC_PLL_CLOCK / div;
            } else {
                val |= CODEC_BYPASS_DIV_CODEC_EQIIR;
                cfg_speed = CODEC_PLL_CLOCK;
            }
        }
    }

    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    pmu_iir_eq_freq_config(cfg_speed);

    lock = int_lock();
    codec->REG_06C = (codec->REG_06C & ~mask) | val;
    codec->REG_06C |= CODEC_EN_CLK_IIR_EQ;
    codec->REG_07C |= CODEC_SOFT_RSTN_IIR_EQ;
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

void hal_codec_iir_eq_disable(void)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;

    mask = CODEC_SEL_OSC_EQIIR | CODEC_SEL_OSCX2_EQIIR | CODEC_SEL_OSCX4_EQIIR | CODEC_BYPASS_DIV_CODEC_EQIIR;
    val = CODEC_SEL_OSC_EQIIR | CODEC_SEL_OSCX2_EQIIR;

    lock = int_lock();
    codec->REG_07C &= ~CODEC_SOFT_RSTN_IIR_EQ;
    codec->REG_06C &= ~CODEC_EN_CLK_IIR_EQ;
    codec->REG_06C = (codec->REG_06C & ~mask) | val;
    int_unlock(lock);

    pmu_iir_eq_freq_config(0);
}

void hal_codec_fir_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;
    uint32_t cfg_speed = 0;

    mask = CODEC_SEL_OSC_FIR | CODEC_SEL_OSCX2_FIR | CODEC_SEL_OSCX4_FIR | CODEC_BYPASS_DIV_CODEC_FIR;
    val = 0;

    if (speed <= 24000000) {
        val |= CODEC_SEL_OSC_FIR | CODEC_SEL_OSCX2_FIR;
        cfg_speed = 24000000;
    } else if (speed <= 48000000) {
        val |= CODEC_SEL_OSCX2_FIR;
        cfg_speed = 48000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (hal_cmu_get_audio_resample_status()) {
            val |= CODEC_SEL_OSCX4_FIR;
            cfg_speed = 96000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = CODEC_PLL_CLOCK / speed;
            if (div >= 2) {
                lock = int_lock();
                codec->REG_074 = SET_BITFIELD(codec->REG_074, CODEC_CFG_DIV_CODEC_FIR, (div - 2));
                int_unlock(lock);
                cfg_speed = CODEC_PLL_CLOCK / div;
            } else {
                val |= CODEC_BYPASS_DIV_CODEC_FIR;
                cfg_speed = CODEC_PLL_CLOCK;
            }
        }
    }

    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    pmu_fir_freq_config(cfg_speed);

    lock = int_lock();
    codec->REG_074 = (codec->REG_074 & ~mask) | val;
    codec->REG_06C |= CODEC_EN_CLK_FIR_MASK;
    codec->REG_07C |= CODEC_SOFT_RSTN_FIR_MASK;
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

void hal_codec_fir_disable(void)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;

    mask = CODEC_SEL_OSC_FIR | CODEC_SEL_OSCX2_FIR | CODEC_SEL_OSCX4_FIR | CODEC_BYPASS_DIV_CODEC_FIR;
    val = CODEC_SEL_OSC_FIR | CODEC_SEL_OSCX2_FIR;

    lock = int_lock();
    codec->REG_07C &= ~CODEC_SOFT_RSTN_FIR_MASK;
    codec->REG_06C &= ~CODEC_EN_CLK_FIR_MASK;
    codec->REG_074 = (codec->REG_074 & ~mask) | val;
    int_unlock(lock);

    pmu_fir_freq_config(0);
}

void hal_codec_fir_select_sys_clock(void)
{
    // 400800F4 bit 0: 1 -- sens hclk; 0 -- mcu hclk
    codec->REG_074 |= CODEC_SEL_HCLK_FIR;
}

void hal_codec_fir_select_own_clock(void)
{
    codec->REG_074 &= ~CODEC_SEL_HCLK_FIR;
}

void hal_codec_rs_enable(uint32_t speed, enum DAC_RS_CLK_USER user)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;
    uint32_t cfg_speed = 0;
    uint32_t i;
    uint32_t old_map = dac_rs_clk_user_map;

    dac_rs_clk_user_map |= (1<<user);
    if (old_map == dac_rs_clk_user_map) {
        return;
    }
    dac_rs_clk_speed[user] = speed;
    for (i = 0; i < RS_CLK_USER_QTY; i++) {
        if (speed < dac_rs_clk_speed[i]) {
            speed = dac_rs_clk_speed[i];
        }
    }

    mask = CODEC_SEL_OSC_RSDAC | CODEC_SEL_OSCX2_RSDAC | CODEC_SEL_OSCX4_RSDAC | CODEC_BYPASS_DIV_CODEC_RSDAC;
    val = 0;

    if (speed <= 24000000) {
        val |= CODEC_SEL_OSC_RSDAC | CODEC_SEL_OSCX2_RSDAC;
        cfg_speed = 24000000;
    } else if (speed <= 48000000) {
        val |= CODEC_SEL_OSCX2_RSDAC;
        cfg_speed = 48000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (hal_cmu_get_audio_resample_status()) {
            val |= CODEC_SEL_OSCX4_RSDAC;
            cfg_speed = 96000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = CODEC_PLL_CLOCK / speed;
            if (div >= 2) {
                lock = int_lock();
                codec->REG_074 = SET_BITFIELD(codec->REG_074, CODEC_CFG_DIV_CODEC_RSDAC, (div - 2));
                int_unlock(lock);
                cfg_speed = CODEC_PLL_CLOCK / div;
            } else {
                val |= CODEC_BYPASS_DIV_CODEC_RSDAC;
                cfg_speed = CODEC_PLL_CLOCK;
            }
        }
    }

    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    pmu_rs_freq_config(cfg_speed);

    lock = int_lock();
    codec->REG_074 = (codec->REG_074 & ~mask) | val;
    codec->REG_060 |= CODEC_EN_CLK_RSDAC(1 << user);
    codec->REG_064 |= CODEC_SOFT_RSTN_RS_DAC(1 << user);
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

void hal_codec_rs_disable(enum DAC_RS_CLK_USER user)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t old_map = dac_rs_clk_user_map;

    dac_rs_clk_speed[user] = 0;
    dac_rs_clk_user_map &= ~(1<<user);
    if (old_map == dac_rs_clk_user_map) {
        return;
    }

    mask = CODEC_SEL_OSC_RSDAC | CODEC_SEL_OSCX2_RSDAC | CODEC_SEL_OSCX4_RSDAC | CODEC_BYPASS_DIV_CODEC_RSDAC;
    val = CODEC_SEL_OSC_RSDAC | CODEC_SEL_OSCX2_RSDAC;

    lock = int_lock();
    codec->REG_064 &= ~CODEC_SOFT_RSTN_RS_DAC(1 << user);
    codec->REG_060 &= ~CODEC_EN_CLK_RSDAC(1 << user);
    if (dac_rs_clk_user_map == 0) {
        codec->REG_074 = (codec->REG_074 & ~mask) | val;
    }
    int_unlock(lock);

    if (dac_rs_clk_user_map == 0) {
        pmu_rs_freq_config(0);
    }
}

void hal_codec_rs_adc_enable(uint32_t speed, enum ADC_RS_CLK_USER user)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;
    uint32_t cfg_speed = 0;
    uint32_t old_map = adc_rs_clk_user_map;
    uint32_t i;

    adc_rs_clk_user_map |= (1<<user);
    if (old_map == adc_rs_clk_user_map) {
        return;
    }
    adc_rs_clk_speed[user] = speed;
    for (i = 0; i < RS_CLK_USER_QTY2; i++) {
        if (speed < adc_rs_clk_speed[i]) {
            speed = adc_rs_clk_speed[i];
        }
    }

    mask = CODEC_SEL_OSC_RSADC | CODEC_SEL_OSCX2_RSADC | CODEC_SEL_OSCX4_RSADC | CODEC_BYPASS_DIV_CODEC_RSADC;
    val = 0;

    // FIXME: forcely use 48MHz clock for ADC RS
    speed = 48000000;

    if (speed <= 24000000) {
        val |= CODEC_SEL_OSC_RSADC | CODEC_SEL_OSCX2_RSADC;
        cfg_speed = 24000000;
    } else if (speed <= 48000000) {
        val |= CODEC_SEL_OSCX2_RSADC;
        cfg_speed = 48000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (hal_cmu_get_audio_resample_status()) {
            val |= CODEC_SEL_OSCX4_RSADC;
            cfg_speed = 96000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = CODEC_PLL_CLOCK / speed;
            if (div >= 2) {
                lock = int_lock();
                codec->REG_074 = SET_BITFIELD(codec->REG_074, CODEC_CFG_DIV_CODEC_RSADC, (div - 2));
                int_unlock(lock);
                cfg_speed = CODEC_PLL_CLOCK / div;
            } else {
                val |= CODEC_BYPASS_DIV_CODEC_RSADC;
                cfg_speed = CODEC_PLL_CLOCK;
            }
        }
    }

    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    pmu_rs_adc_freq_config(cfg_speed);

    lock = int_lock();
    codec->REG_074 = (codec->REG_074 & ~mask) | val;
    codec->REG_060 |= CODEC_EN_CLK_RSADC(1 << user);
    codec->REG_064 |= CODEC_SOFT_RSTN_RS_ADC(1 << user);
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

void hal_codec_rs_adc_disable(enum ADC_RS_CLK_USER user)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t old_map = adc_rs_clk_user_map;

    adc_rs_clk_speed[user] = 0;
    adc_rs_clk_user_map &= ~(1<<user);
    if (old_map == adc_rs_clk_user_map) {
        return;
    }

    mask = CODEC_SEL_OSC_RSADC | CODEC_SEL_OSCX2_RSADC | CODEC_SEL_OSCX4_RSADC | CODEC_BYPASS_DIV_CODEC_RSADC;
    val = CODEC_SEL_OSC_RSADC | CODEC_SEL_OSCX2_RSADC;

    lock = int_lock();
    codec->REG_064 &= ~CODEC_SOFT_RSTN_RS_ADC(1 << user);
    codec->REG_060 &= ~CODEC_EN_CLK_RSADC(1 << user);
    if (adc_rs_clk_user_map == 0) {
        codec->REG_074 = (codec->REG_074 & ~mask) | val;
    }
    int_unlock(lock);

    if (adc_rs_clk_user_map == 0) {
        pmu_rs_adc_freq_config(0);
    }
}

void hal_codec_psap_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t cfg_speed = 0;

    mask = CODEC_SEL_OSC_PSAP;
    //val = CODEC_EN_CLK_PSAP((1 << 0) | (1 << 1));
    val = CODEC_EN_CLK_PSAP((1 << 0)); //The right psap can't work.

    if (speed <= 24000000) {
        val |= CODEC_SEL_OSC_PSAP;
        cfg_speed = 24000000;
    } else if (speed <= 48000000) {
        cfg_speed = 48000000;
    }

    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    lock = int_lock();
    codec->REG_06C = (codec->REG_06C & ~mask) | val;
    //codec->REG_07C |= CODEC_SOFT_RSTN_PSAP((1 << 0) | (1 << 1));
    codec->REG_07C |= CODEC_SOFT_RSTN_PSAP((1 << 0)); ////The right psap can't work.
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

void hal_codec_psap_disable(void)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;

    mask = CODEC_SEL_OSC_PSAP | CODEC_EN_CLK_PSAP((1 << 0) | (1 << 1));
    val = CODEC_SEL_OSC_PSAP;

    lock = int_lock();
    codec->REG_07C &= ~CODEC_SOFT_RSTN_PSAP((1 << 0) | (1 << 1));
    codec->REG_06C = (codec->REG_06C & ~mask) | val;
    int_unlock(lock);
}

void hal_codec_psap_setup(enum AUD_CHANNEL_MAP_T ch_map, const struct HAL_CODEC_PSAP_CFG_T *cfg)
{
    uint32_t val;
    uint32_t gain_val;
    uint8_t din0_sel;
    uint8_t din1_sel;
    enum HAL_CODEC_PSAP_MODE_T mode;

    if ((ch_map & AUD_CHANNEL_MAP_CH0) == 0) {
        return;
    }

    mode=cfg->mode;
    if (mode == HAL_CODEC_PSAP_MODE_DAC) {
        mode = HAL_CODEC_PSAP_MODE_ADC_DAC;
        din0_sel = 7; // MUTE
    } else {
#if defined(ANC_APP) && defined(ANC_TT_MIC_CH_L)
        din0_sel = get_lsb_pos(anc_tt_adc_ch_l);
#else
        din0_sel = 7; // MUTE
#endif
    }
    din1_sel = 7; // MUTE

    codec->REG_400 = (codec->REG_400 & ~(CODEC_PSAP_ADC0_SEL_CH0_MASK | CODEC_PSAP_ADC1_SEL_CH0_MASK |
        CODEC_PSAP_MIX_MODE_MASK)) | CODEC_PSAP_ADC0_SEL_CH0(din0_sel) | CODEC_PSAP_ADC1_SEL_CH0(din1_sel) | CODEC_PSAP_MIX_MODE(mode);

    val = CODEC_MUTE_GAIN_UPDATE_DIN0_PSAP_CH0 | CODEC_DIN0_PSAP_FIFO_BYPASS;

    // GAIN Format: 3.9
    int32_t s_val = (int32_t)(cfg->din0_gain * (1 << 9));
    gain_val = __SSAT(s_val, 12);

    val |= CODEC_MUTE_GAIN_COEF_DIN0_PSAP_CH0(gain_val);

    if (cfg->din0_samp_delay) {
        val |= CODEC_DIN0_PSAP_DELAY_CH0(cfg->din0_samp_delay);
        codec->REG_480 = val;
        hal_codec_reg_update_delay();
        val |= CODEC_DIN0_PSAP_DELAY_UPDATE;
        codec->REG_480 = val;
        val = (val & ~CODEC_DIN0_PSAP_FIFO_BYPASS) | CODEC_DIN0_PSAP_FIFO_EN;
        codec->REG_480 = val;
    }

    if (cfg->din1_sign) {
        val |= CODEC_DIN1_PSAP_INV_CH0;
    }

    codec->REG_480 = val;
}

#ifdef CODEC_MIN_PHASE
static void hal_codec_min_phase_init(void)
{
    int i;

    hal_codec_fir_enable(48000000);

    codec->REG_06C |= CODEC_EN_CLK_FIR((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));

    // DAC
    codec->REG_108 &= ~(CODEC_STREAM0_FIR1_CH0);
    codec->REG_108 = SET_BITFIELD(codec->REG_108,CODEC_FIR_MODE_CH0,0);
    codec->REG_108 = SET_BITFIELD(codec->REG_108,CODEC_FIR_ORDER_CH0,FIR_DAC_ORDERS);

    codec->REG_10C = SET_BITFIELD(codec->REG_10C,CODEC_FIR_BURST_LENGTH_CH0,4);
    codec->REG_10C = SET_BITFIELD(codec->REG_10C,CODEC_FIR_GAIN_SEL_CH0,4);

    // ADC
    codec->REG_110 &= ~(CODEC_STREAM0_FIR1_CH1);
    codec->REG_110 = SET_BITFIELD(codec->REG_110,CODEC_FIR_MODE_CH1,0);
    codec->REG_110 = SET_BITFIELD(codec->REG_110,CODEC_FIR_ORDER_CH1,FIR_ADC_ORDERS);

    codec->REG_114 = SET_BITFIELD(codec->REG_114,CODEC_FIR_GAIN_SEL_CH1,6);

    codec->REG_118 &= ~(CODEC_STREAM0_FIR1_CH2);
    codec->REG_118 = SET_BITFIELD(codec->REG_118,CODEC_FIR_MODE_CH2,0);
    codec->REG_118 = SET_BITFIELD(codec->REG_118,CODEC_FIR_ORDER_CH2,FIR_ADC_ORDERS);

    codec->REG_11C = SET_BITFIELD(codec->REG_11C,CODEC_FIR_GAIN_SEL_CH2,6);

    codec->REG_120 &= ~(CODEC_STREAM0_FIR1_CH3);
    codec->REG_120 = SET_BITFIELD(codec->REG_120,CODEC_FIR_MODE_CH3,0);
    codec->REG_120 = SET_BITFIELD(codec->REG_120,CODEC_FIR_ORDER_CH3,FIR_ADC_ORDERS);

    codec->REG_124 = SET_BITFIELD(codec->REG_124,CODEC_FIR_GAIN_SEL_CH3,6);

    hal_codec_fir_select_sys_clock();

    // DAC
    codec->REG_100 |= CODEC_FIR_UPSAMPLE_CH0;

    for (i = 0; i < FIR_DAC_ORDERS; i++) {
        codec_fir_ch0[i] = fir_coef_minimun[i];
    }

    // ADC
    codec->REG_100 &= ~(CODEC_FIR_UPSAMPLE_CH1 | CODEC_FIR_UPSAMPLE_CH2 | CODEC_FIR_UPSAMPLE_CH3);

    for (i = 0; i < FIR_DAC_ORDERS; i++) {
        codec_fir_ch1[i] = fir_coef_minimun[i];
        codec_fir_ch2[i] = fir_coef_minimun[i];
        codec_fir_ch3[i] = fir_coef_minimun[i];
    }

    // Init history buffer
    for (i=0; i < MAX_FIR_ORDERS; i++) {
        codec_fir_history0[i] = 0;
        codec_fir_history1[i] = 0;
        codec_fir_history2[i] = 0;
        codec_fir_history3[i] = 0;
    }

    hal_codec_fir_select_own_clock();
}

static void hal_codec_min_phase_term(void)
{
    codec->REG_06C &= ~CODEC_EN_CLK_FIR((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
    hal_codec_fir_disable();
}
#endif

static int hal_codec_ci2s_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    // to confirm ci2s clk work on 24.576M , div = pll_freq/24.576M - 2
    codec->REG_760 = SET_BITFIELD(codec->REG_760, CODEC_CFG_DIV_CI2S, div);
    int_unlock(lock);

    return 0;
}

static void hal_codec_ci2s_clock_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div = 0;
    uint32_t cfg_speed = 0;

    mask = CODEC_BYPASS_DIV_CI2S | CODEC_SEL_CI2S_OSCX2 | CODEC_SEL_CI2S_OSC
         | CODEC_SOFT_RSTN_CI2S | CODEC_EN_CLK_CI2S | CODEC_EN_CLK_CI2S_OUT;
    val = 0;

#if defined(__AUDIO_RESAMPLE__)
    if (hal_cmu_get_audio_resample_status()) {
        if (speed <= 12000000) {
            val |= CODEC_SEL_CI2S_OSC | CODEC_SEL_CI2S_OSCX2;
            div = 2;
            cfg_speed = 12000000;
        } else if (speed <= 24000000) {
            val |= CODEC_SEL_CI2S_OSC | CODEC_SEL_CI2S_OSCX2;
            cfg_speed = 24000000;
        } else if (speed <= 48000000) {
            val |= CODEC_SEL_CI2S_OSCX2;
            cfg_speed = 48000000;
        }
    }
    else
#endif
    {
        // Assume audio stream is one of 48K series
        div = CODEC_PLL_CLOCK / speed;
        // val |= CODEC_SOFT_RSTN_CI2S;
        if (div >= 2) {
            // hal_codec_ci2s_set_div(div);
            cfg_speed = CODEC_PLL_CLOCK / div;
        } else {
            // val |= CODEC_BYPASS_DIV_CI2S;
            cfg_speed = CODEC_PLL_CLOCK;
        }
    }

    if (div >= 2) {
        hal_codec_ci2s_set_div(div);
    } else {
        val |= CODEC_BYPASS_DIV_CI2S;
    }
    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    lock = int_lock();
    codec->REG_760 = (codec->REG_760 & ~mask) | val;
    // ci2s clk gate, ci2s master clock
    codec->REG_760 |= CODEC_EN_CLK_CI2S | CODEC_EN_CLK_CI2S_OUT | CODEC_SOFT_RSTN_CI2S;
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

static void hal_codec_ci2s_clock_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    // ci2s clk gate, ci2s master clock
    codec->REG_760 &= ~(CODEC_EN_CLK_CI2S | CODEC_EN_CLK_CI2S_OUT | CODEC_SOFT_RSTN_CI2S);
    int_unlock(lock);

    hal_codec_reg_update_delay();
}

static struct CODEC_DAC_CI2S_CFG_T ci2s_cfg = {
    .txch_en = 0xf,                 //4ch
    .rxch_en = 0xf,                 //4ch
    .tx_dm = 1,                     //sdi tx data mode
    .rx_dm = 1,                     //sdi rx data mode
    .tx_en_mode = 1,
    .rx_en_mode = 1,
    .clk_en_mode = 1,
    .slv = 0,                       //0:master mode 1:slave mode
    .clk_wordsize = 0,              //tdm_en=0, sck lens: 0:16 1:24 2:32
    // 0:3M 1:1.5M
    .rate_sel = 1,
    .en_phase_sel = 0,
    .adc_fifo_th = 0,
    .dac_fifo_th = 0,
    .dac_en_ch0 = 1,
    .dac_en_ch1 = 1,
    .dac_sinc_rate_sel_ch0 = 1,
    .dac_sinc_rate_sel_ch1 = 1,

    .tdm_en = 1,                    //0:i2s mode 1:tmd mode
    .tdm_mode = 0,                  //0:ModeB(data sync with ws) 1:ModeA(Data behind by one cycle of ws)
    .tdm_ws_neg = 0,                //0:TDM_FS_EDGE_POSEDGE 1:TDM_FS_EDGE_NEGEDGE
    .tdm_frame_width = 1,           //frame lens: 0:8 1:16 2:32 3:64 4:128 5:256 default:256
    .tdm_ws_width = 0,              //ws lens: 0:1 1:8 2:16 3:32 4:64 5:128
    .tdm_slot_width = 1,
    .tdm_data_offset = 0,
    .i2s_data_offset = 0
};

int hal_codec_ci2s_config(void)
{
    struct CODEC_DAC_CI2S_CFG_T * ci2s = &ci2s_cfg;
    uint32_t map;
    uint32_t dac_dev = analog_codec_get_dac_dev();

    if (dac_dev == (PMU_CODEC_DAC_L | PMU_CODEC_DAC_R) || ci2s->rate_sel == 1) {
        ci2s->tdm_frame_width = 1;
        ci2s->tx_dm = 1;
        ci2s->rx_dm = 1;
    } else {
        ci2s->tdm_frame_width = 0;
        ci2s->tx_dm = 0;
        ci2s->rx_dm = 0;
    }

    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_RATE_SEL, ci2s->rate_sel);
    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_EN_CI2S_PHASE_SEL, ci2s->en_phase_sel);
    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_FIFO_THRESHOLD, ci2s->adc_fifo_th);
    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_DAC_FIFO_THRESHOLD, ci2s->dac_fifo_th);

    // fifo flush
    map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3;
    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            codec->REG_7F0 |= (CODEC_CI2S_ADC_FIFO_FLUSH_CH0 << i);
            hal_codec_reg_update_delay();
            codec->REG_7F0 &= ~(CODEC_CI2S_ADC_FIFO_FLUSH_CH0 << i);
        }
    }

    //ci2s mode sel
    codec->REG_7F4 = SET_BITFIELD(codec->REG_7F4, CODEC_CI2S_TXCH_EN, ci2s->txch_en);
    codec->REG_7F4 = SET_BITFIELD(codec->REG_7F4, CODEC_CI2S_RXCH_EN, ci2s->rxch_en);
    codec->REG_7F4 = SET_BITFIELD(codec->REG_7F4, CODEC_CI2S_CLK_WORD_SIZE, ci2s->clk_wordsize);
    codec->REG_7F4 = SET_BITFIELD(codec->REG_7F4, CODEC_CI2S_CLK_GATE, 0);
    codec->REG_7F4 = SET_BITFIELD(codec->REG_7F4, CODEC_CI2S_RX_DM, ci2s->rx_dm);
    codec->REG_7F4 = SET_BITFIELD(codec->REG_7F4, CODEC_CI2S_TX_DM, ci2s->tx_dm);
    if (ci2s->tx_en_mode) {
        codec->REG_7F4 |= CODEC_CI2S_TX_EN_MODE;
    }
    if (ci2s->rx_en_mode) {
        codec->REG_7F4 |= CODEC_CI2S_RX_EN_MODE;
    }
    if (ci2s->clk_en_mode) {
        codec->REG_7F4 |= CODEC_CI2S_CLK_EN_MODE;
    }
    if (ci2s->slv) {
        codec->REG_7F4 |= CODEC_CI2S_MSTN_SLV_MODE;
    }
    if (ci2s->dac_en_ch0) {
        codec->REG_7F4 |= CODEC_CI2S_DAC_ENABLE_CH0;
        codec->REG_7F8 = SET_BITFIELD(codec->REG_7F8, CODEC_DAC_SINC_RATE_SEL_CH0, ci2s->dac_sinc_rate_sel_ch0);
    }
    if (ci2s->dac_en_ch1) {
        codec->REG_7F4 |= CODEC_CI2S_DAC_ENABLE_CH1;
        codec->REG_7F8 = SET_BITFIELD(codec->REG_7F8, CODEC_DAC_SINC_RATE_SEL_CH1, ci2s->dac_sinc_rate_sel_ch1);
    }

    // tdm en sel
    codec->REG_7F8 = SET_BITFIELD(codec->REG_7F8, CODEC_CI2S_TDM_FRAME_WIDTH, ci2s->tdm_frame_width);
    codec->REG_7F8 = SET_BITFIELD(codec->REG_7F8, CODEC_CI2S_TDM_WS_WIDTH, ci2s->tdm_ws_width);
    codec->REG_7F8 = SET_BITFIELD(codec->REG_7F8, CODEC_CI2S_TDM_SLOT_WIDTH, ci2s->tdm_slot_width);
    codec->REG_7F8 = SET_BITFIELD(codec->REG_7F8, CODEC_CI2S_TDM_DATA_OFFSET, ci2s->tdm_data_offset);
    codec->REG_7F8 = SET_BITFIELD(codec->REG_7F8, CODEC_CI2S_DATA_OFFSET, ci2s->i2s_data_offset);

    if (ci2s->tdm_en) {
        codec->REG_7F8 |= CODEC_CI2S_TDM_EN;
    }
    if (ci2s->tdm_mode) {
        codec->REG_7F8 |= CODEC_CI2S_TDM_MODE;
    }
    if (ci2s->tdm_ws_neg) {
        codec->REG_7F8 |= CODEC_CI2S_TDM_WS_NEGEDGE;
    }

    return 0;
}

int static hal_codec_ci2s_enable(uint8_t ci2s_sel)
{
    TRACE(0, "[%s] ci2s_sel = %x", __func__, ci2s_sel);
    if (ci2s_sel & CI2S_TX_EN) {
        codec->REG_7F4 |= CODEC_CI2S_TX_EN;
    } else {
        codec->REG_7F4 &= ~CODEC_CI2S_TX_EN;
    }

    if (ci2s_sel & CI2S_RX_EN) {
        codec->REG_7F4 |= CODEC_CI2S_RX_EN;
    } else {
        codec->REG_7F4 &= ~CODEC_CI2S_RX_EN;
    }

    if (ci2s_sel) {
        codec->REG_7F4 |= CODEC_CI2S_EN;
        codec->REG_7F4 |= CODEC_CI2S_CLK_EN;
#ifdef DCDC_CLOCK_CONTROL
        TRACE(0, "hal_cmu_dcdc_clock_enable");
        hal_cmu_dcdc_clock_enable(HAL_CMU_DCDC_CLOCK_USER_CODEC);
#endif
    } else {
        codec->REG_7F4 &= ~CODEC_CI2S_EN;
        codec->REG_7F4 &= ~CODEC_CI2S_CLK_EN;
#ifdef DCDC_CLOCK_CONTROL
        TRACE(0, "hal_cmu_dcdc_clock_disable");
        hal_cmu_dcdc_clock_disable(HAL_CMU_DCDC_CLOCK_USER_CODEC);
#endif
    }

    return 0;
}

void hal_codec_ci2s_low_delay(void)
{
    codec->REG_0B4 |= CODEC_CODEC_DAC_SINC_MODE_CH0 | CODEC_CODEC_DAC_SINC_MODE_CH1;
    codec->REG_0A4 |= CODEC_CODEC_ADC_SINC2_MODE_CH0 | CODEC_CODEC_ADC_SINC2_MODE_CH1
                    | CODEC_CODEC_ADC_SINC2_MODE_CH2 | CODEC_CODEC_ADC_SINC2_MODE_CH3;
}

void hal_codec_ci2s_init(void)
{
    struct CODEC_DAC_CI2S_CFG_T * ci2s = &ci2s_cfg;
    uint32_t dac_dev = analog_codec_get_dac_dev();

    hal_codec_ci2s_config();
    hal_codec_ci2s_low_delay();

    if (dac_dev == (PMU_CODEC_DAC_L | PMU_CODEC_DAC_R) && ci2s->rate_sel == 0) {
        hal_codec_ci2s_clock_enable(48000000);
    } else {
        hal_codec_ci2s_clock_enable(24000000);
    }
}

void hal_codec_ci2s_tx_enable(bool enable)
{
    if (pmu_get_chip_type() != PMU_CHIP_TYPE_1809) {
        return ;
    }
    TRACE(0,"%s", __func__);
    if (analog_codec_get_dac_dev() & (PMU_CODEC_DAC_L | PMU_CODEC_DAC_R)) {
        if (enable) {
            codec_ci2s_idx |= CI2S_TX_EN;
        } else {
            codec_ci2s_idx &= ~CI2S_TX_EN;
        }
    }
    hal_codec_ci2s_enable(codec_ci2s_idx);
}

void hal_codec_ci2s_rx_enable(bool enable)
{
    TRACE(0,"%s", __func__);
    if (enable) {
        codec_ci2s_idx |= CI2S_RX_EN;
    } else {
        codec_ci2s_idx &= ~CI2S_RX_EN;
    }
    hal_codec_ci2s_enable(codec_ci2s_idx);
}

int hal_codec_open(enum HAL_CODEC_ID_T id)
{
    int i;
    bool first_open;

#ifdef CODEC_POWER_DOWN
    first_open = true;
#else
    first_open = !codec_init;
#endif

    analog_aud_pll_open(ANA_AUD_PLL_USER_CODEC);

    if (!codec_init) {
        for (i = 0; i < CFG_HW_AUD_INPUT_PATH_NUM; i++) {
            if (cfg_audio_input_path_cfg[i].cfg & AUD_CHANNEL_MAP_ALL & ~VALID_MIC_MAP) {
                ASSERT(false, "Invalid input path cfg: i=%d io_path=%d cfg=0x%X",
                    i, cfg_audio_input_path_cfg[i].io_path, cfg_audio_input_path_cfg[i].cfg);
            }
        }
#ifdef ANC_APP
        anc_boost_gain_attn = 1.0f;
#endif
        codec_init = true;
    }
    if (first_open) {
        // Codec will be powered down first
        hal_psc_codec_enable();
    }
    hal_cmu_codec_clock_enable();
    hal_cmu_codec_reset_clear();
    hal_codec_ci2s_init();

    codec_opened = true;

    codec->REG_060 |= CODEC_EN_CLK_ADC_MASK | CODEC_EN_CLK_ADC_ANA_MASK | CODEC_POL_ADC_ANA_MASK;
    codec->REG_064 |= CODEC_POL_DAC_OUT
                    | CODEC_SOFT_RSTN_RS_DAC_MASK | CODEC_SOFT_RSTN_DAC
                    | CODEC_SOFT_RSTN_RS_ADC_MASK | CODEC_SOFT_RSTN_ADC_MASK
                    | CODEC_SOFT_RSTN_ADC_ANA_MASK | CODEC_SOFT_RSTN_ADC_DSF_MASK
                    | CODEC_SOFT_RSTN_DAC_USF_MASK;
    codec->REG_07C |= CODEC_SOFT_RSTN_IIR_ANC_MASK | CODEC_SOFT_RSTN_IIR_EQ;
    codec->REG_000 = 0;
    codec->REG_04C &= ~CODEC_MC_ENABLE;
    codec->REG_004 = ~0UL;
    hal_codec_reg_update_delay();
    codec->REG_004 = 0;
    codec->REG_000 |= CODEC_CODEC_IF_EN;

    codec->REG_054 |= CODEC_FAULT_MUTE_DAC_ENABLE;
    codec->REG_748 |= CODEC_FAULT_MUTE_DAC_ENABLE_SND;

    // codec dac buffer swap
    if (analog_codec_get_dac_dev() & MCU_CODEC_DAC_R) {
        codec->REG_0B0 |= CODEC_CODEC_DAC_LR_SWAP;
        codec->REG_784 |= CODEC_CODEC_DAC_LR_SWAP_SND;
        codec->REG_798 |= CODEC_CODEC_DAC_LR_SWAP_TRI;
        codec->REG_7F0 |= CODEC_CI2S_DAC_OUT_SWAP;
    }

    // chip 2 pmu codec ci2s data swap
    if (analog_codec_get_dac_dev() == (MCU_CODEC_DAC_L | PMU_CODEC_DAC_L)) {
        codec->REG_7F0 |= CODEC_CI2S_DAC_OUT_SWAP;
    }
#ifdef AUDIO_OUTPUT_SWAP
    if (output_swap) {
        codec->REG_0B8 |= CODEC_CODEC_DAC_OUT_SWAP;
    } else {
        codec->REG_0B8 &= ~CODEC_CODEC_DAC_OUT_SWAP;
    }
#endif

#ifdef CODEC_TIMER
    // Enable codec timer and record time by bt event instead of gpio event
    codec->REG_054 = (codec->REG_054 & ~CODEC_EVENT_SEL) | CODEC_EVENT_FOR_CAPTURE;
    codec->REG_060 |= CODEC_EN_CLK_DAC;
#endif

    if (first_open) {
        hal_codec_select_audio_pll();

        hal_codec_select_audio_clock();

#ifdef AUDIO_ANC_FB_MC
        codec->REG_04C = CODEC_DMACTRL_MC | CODEC_MC_EN_SEL | CODEC_MC_RATE_SRC_SEL;
#endif
        // ANC zero-crossing
        codec->REG_0D4 |= (CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1);
        codec->REG_0D8 |= (CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1);

        // Enable ADC zero-crossing gain adjustment
        for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
            *(&codec->REG_084 + i) |= CODEC_CODEC_ADC_GAIN_SEL_CH0;
        }

        hal_codec_dac_dre_init();

#ifdef ANC_PROD_TEST
#ifdef AUDIO_ANC_FB_MC
        // Enable ADC + music cancel.
        codec->REG_130 |= CODEC_CODEC_FB_CHECK_KEEP_CH0;
#elif defined(AUDIO_ANC_FB_MC_HW)
        // Enable ADC + music cancel.
        codec->REG_130 |= CODEC_CODEC_FB_CHECK_KEEP_CH0;
#endif
#endif
        // init EC mic gain
        hal_codec_ec_gain_init();

#if defined(FIXED_CODEC_ADC_VOL) && defined(SINGLE_CODEC_ADC_VOL)
        const CODEC_ADC_VOL_T *adc_gain_db;

        adc_gain_db = hal_codec_get_adc_volume(CODEC_SADC_VOL);
        if (adc_gain_db) {
            hal_codec_set_dig_adc_gain(NORMAL_ADC_MAP, *adc_gain_db);
#ifdef SIDETONE_DEDICATED_ADC_CHAN
            sidetone_adc_gain = *adc_gain_db;
#endif
        }
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
#ifdef AUDIO_OUTPUT_DC_AUTO_CALIB
        if (hal_codec_set_ana_dc_calib_value()) {
            hal_codec_get_dig_dc_calib_value(&dac_dc_l, &dac_dc_r);
        }
#endif
        hal_codec_dac_dc_offset_enable(dac_dc_l, dac_dc_r);
#elif defined(SDM_MUTE_NOISE_SUPPRESSION)
        hal_codec_dac_dc_offset_enable(1, 1);
#endif /* AUDIO_OUTPUT_DC_CALIB */

        hal_codec_adc_dc_offset_init();

#if defined(AUDIO_OUTPUT_SW_GAIN) || defined(AUDIO_OUTPUT_DAC2_SW_GAIN) \
    || defined(AUDIO_OUTPUT_DAC2_SW_GAIN)
        const struct CODEC_DAC_VOL_T *vol_tab_ptr;
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
        // Init gain settings
        vol_tab_ptr = hal_codec_get_dac_volume(0);
        if (vol_tab_ptr) {
            analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
            hal_codec_set_dig_dac_gain(VALID_DAC_MAP, ZERODB_DIG_DBVAL);
        }
#else
#ifdef DAC_ZERO_CROSSING_GAIN
        // Enable DAC zero-crossing gain adjustment
        codec->REG_0B4 |= CODEC_CODEC_DAC_GAIN_SEL_CH0;
        codec->REG_0B8 |= CODEC_CODEC_DAC_GAIN_SEL_CH1;
#elif defined(DAC_RAMP_GAIN)
        // Enable DAC ramp gain (adjust 2^-14 on each sample)
        codec->REG_154 = CODEC_CODEC_RAMP_STEP_CH0(1) | CODEC_CODEC_RAMP_EN_CH0 | CODEC_CODEC_RAMP_INTERVAL_CH0(0);
        codec->REG_158 = CODEC_CODEC_RAMP_STEP_CH1(1) | CODEC_CODEC_RAMP_EN_CH1 | CODEC_CODEC_RAMP_INTERVAL_CH1(0);

        codec->REG_790 = CODEC_CODEC_RAMP_STEP_CH0(1) | CODEC_CODEC_RAMP_EN_SND_CH0 | CODEC_CODEC_RAMP_INTERVAL_CH0(0);
        codec->REG_794 = CODEC_CODEC_RAMP_STEP_CH1(1) | CODEC_CODEC_RAMP_EN_SND_CH1 | CODEC_CODEC_RAMP_INTERVAL_CH1(0);

        codec->REG_7A8 = CODEC_CODEC_RAMP_STEP_CH0(1) | CODEC_CODEC_RAMP_EN_TRI_CH0 | CODEC_CODEC_RAMP_INTERVAL_CH0(0);
        codec->REG_7AC = CODEC_CODEC_RAMP_STEP_CH1(1) | CODEC_CODEC_RAMP_EN_TRI_CH1 | CODEC_CODEC_RAMP_INTERVAL_CH1(0);
#endif
#endif /* AUDIO_OUTPUT_SW_GAIN */

#ifdef AUDIO_OUTPUT_DAC2_SW_GAIN
        // Init gain settings
        vol_tab_ptr = hal_codec_get_dac_volume(0);
        if (vol_tab_ptr) {
            analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
            hal_codec_set_dig_dac2_gain(VALID_DAC_MAP, ZERODB_DIG_DBVAL);
        }
#else
        // FIXME: Complete hardware tuning voulme
#endif /* AUDIO_OUTPUT_DAC2_SW_GAIN */

#ifdef AUDIO_OUTPUT_DAC3_SW_GAIN
        // Init gain settings
        vol_tab_ptr = hal_codec_get_dac_volume(0);
        if (vol_tab_ptr) {
            analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
            hal_codec_set_dig_dac3_gain(VALID_DAC_MAP, ZERODB_DIG_DBVAL);
        }
#else
        // FIXME: Complete hardware tuning voulme
#endif /* AUDIO_OUTPUT_DAC3_SW_GAIN */

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        // Reset SDM
        hal_codec_set_dac_gain_value((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, 0);
        codec->REG_0BC |= CODEC_CODEC_DAC_SDM_CLOSE;
#endif

        codec->REG_0B0 = SET_BITFIELD(codec->REG_0B0, CODEC_CODEC_DAC_SDM_GAIN, 3);
#ifdef SDM_MUTE_NOISE_SUPPRESSION
        codec->REG_0B0 = SET_BITFIELD(codec->REG_0B0, CODEC_CODEC_DAC_DITHER_GAIN, 0x10);
#endif

#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~(CODEC_CODEC_RESAMPLE_DAC_ENABLE
            | CODEC_CODEC_RESAMPLE_ADC_ENABLE
            | CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE
            | CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE);

        codec->REG_7C0 &= ~(CODEC_CODEC_RESAMPLE_DAC_ENABLE_SND
            | CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRI
            | CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND
            | CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI
            | CODEC_CODEC_RESAMPLE_ADC_SND_ENABLE
            | CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE);
#endif

#ifdef CODEC_DSD
        for(i = 0; i < ARRAY_SIZE(codec_adc_sample_rate); i++) {
            if(codec_adc_sample_rate[i].sample_rate == AUD_SAMPRATE_44100) {
                break;
            }
        }
        hal_codec_set_adc_down((AUD_CHANNEL_MAP_CH2), codec_adc_sample_rate[i].adc_down);
        hal_codec_set_adc_hbf_bypass_cnt((AUD_CHANNEL_MAP_CH2), codec_adc_sample_rate[i].bypass_cnt);
#endif

        // Mute DAC when cpu fault occurs
#if defined(CHIP_SUBSYS_BTH) || defined(CHIP_SUBSYS_SENS)
        hal_cmu_codec_set_fault_mask2(0x2F, 0);
#else
        hal_cmu_codec_set_fault_mask2(~0, 0x7FF);
#endif

#ifdef CODEC_TIMER
        // Disable sync stamp auto clear to avoid impacting codec timer capture
        codec->REG_054 &= ~CODEC_STAMP_CLR_USED;
#else
        // Enable sync stamp auto clear
        codec->REG_054 |= CODEC_STAMP_CLR_USED;
#endif
    }

#ifdef CODEC_MIN_PHASE
    if (min_phase_cfg) {
        hal_codec_min_phase_init();
    }
#endif

    return 0;
}

int hal_codec_close(enum HAL_CODEC_ID_T id)
{
#ifdef CODEC_MIN_PHASE
    if (min_phase_cfg) {
        hal_codec_min_phase_term();
    }
#endif

#ifdef CODEC_TIMER
    // Reset codec timer
    codec->REG_054 &= ~CODEC_EVENT_FOR_CAPTURE;
    codec->REG_060 &= ~CODEC_EN_CLK_DAC;
#endif

    codec->REG_054 &= ~CODEC_FAULT_MUTE_DAC_ENABLE;
    codec->REG_748 &= ~CODEC_FAULT_MUTE_DAC_ENABLE_SND;

    codec->REG_000 = 0;
    codec->REG_064 = 0;
    codec->REG_060 = 0;

    codec_opened = false;

#ifdef CODEC_POWER_DOWN
    hal_cmu_codec_reset_set();
    hal_cmu_codec_clock_disable();
    hal_psc_codec_disable();
#else
    // NEVER reset or power down CODEC registers, for the CODEC driver expects that last configurations
    // still exist in the next stream setup
    hal_cmu_codec_clock_disable();
#endif
    hal_codec_ci2s_clock_disable();
    analog_aud_pll_close(ANA_AUD_PLL_USER_CODEC);

    // clear ADC1 channel map
    codec_adc_ch_map = 0;
    codec_mic_ch_map = 0;

    // clear ADC2 channel map
    codec_adc2_ch_map = 0;
    codec_mic2_ch_map = 0;

    return 0;
}

void hal_codec_crash_mute(void)
{
    if (codec_opened) {
        codec->REG_000 &= ~CODEC_CODEC_IF_EN;
    }
}

#ifdef AUDIO_OUTPUT_DC_AUTO_CALIB
static bool dac_dc_calib_status = false;

void hal_codec_set_dac_calib_status(bool status)
{
    dac_dc_calib_status = status;
}

void hal_codec_dac_dre_init_calib_cfg(void)
{
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg = &dac_dre_calib_cfg[0];
    uint8_t max_gain, ini_gain, gain_offset, top_gain;
    uint8_t gain_step;

    max_gain = analog_aud_get_max_dre_gain();
    if (max_gain < 0xF) {
        gain_step = max_gain;
    } else {
        gain_step = 16;
    }

    TRACE(1, "%s: max_gain=0x%x, gain_step=%d", __func__, max_gain, gain_step);
    if (cfg[0].ana_gain != max_gain) {
        ini_gain = 0;
        if (max_gain >= 0xF) {
            gain_offset = max_gain - 0xF;
        } else {
            gain_offset = 32 - (0xF - max_gain);
        }
        cfg[0].ana_gain = max_gain;
        cfg[0].ini_ana_gain = ini_gain;
        cfg[0].gain_offset = gain_offset;
        cfg[1].ana_gain = cfg[0].ana_gain - (gain_step - 1);
        if (max_gain >= 0xF) {
            cfg[1].ini_ana_gain = 0xF - (cfg[1].ana_gain - gain_offset);
        } else {
            cfg[1].ini_ana_gain = cfg[0].ana_gain - cfg[1].ana_gain;
        }
        cfg[1].gain_offset = cfg[0].gain_offset;
        top_gain = cfg[0].ana_gain - cfg[1].ana_gain;
        cfg[1].top_gain = cfg[0].top_gain = top_gain;
        TRACE(1, "update max_gain:%d, ini=%d,offs=%d,top=%d", max_gain, ini_gain, gain_offset, top_gain);
    }
}

struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *hal_codec_dac_dre_get_calib_cfg(uint32_t *nr)
{
    *nr = ARRAY_SIZE(dac_dre_calib_cfg);
    return dac_dre_calib_cfg;
}

int hal_codec_dac_dre_check_calib_cfg(struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg)
{
    int error = 0;

    if ((cfg->valid & 0xF) == 0) {
        error |= 0x1;
    }
    if ((cfg->valid & (~(0xF))) != DAC_DC_VALID_MARK) {
        error |= 0x2;
    }
    if (cfg->ana_gain > 0x1F) {
        error |= 0x4;
    }
    if (cfg->ini_ana_gain > 0xF) {
        error |= 0x8;
    }
    if (cfg->gain_offset > 0x1F) {
        error |= 0x10;
    }
    if (cfg->step_mode > 4) {
        error |= 0x20;
    }
    if (cfg->top_gain > 0xF) {
        error |= 0x40;
    }
    return error;
}

void hal_codec_set_dac_ana_gain(uint8_t ini_gain, uint8_t gain_offset)
{
    uint16_t dac_dev = analog_codec_get_dac_dev();
    if (GET_BITFIELD(dac_dev, PMU_CODEC_DAC)) {
        analog1809_aud_set_dac_ana_gain(ini_gain, gain_offset);
    }
    if (GET_BITFIELD(dac_dev, MCU_CODEC_DAC)) {
      uint8_t max_gain = analog_aud_get_max_dre_gain();

      codec->REG_350 = CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(ini_gain);
      codec->REG_354 = CODEC_CODEC_DRE_GAIN_OFFSET_CH0(gain_offset);
      codec->REG_1B8 = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0(0);
      codec->REG_358 = CODEC_CODEC_DRE_GAIN_TOP_CH0(max_gain);

      codec->REG_35C = CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(ini_gain);
      codec->REG_360 = CODEC_CODEC_DRE_GAIN_OFFSET_CH1(gain_offset);
      codec->REG_1BC = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1(0);
      codec->REG_364 = CODEC_CODEC_DRE_GAIN_TOP_CH1(max_gain);
    }
}

bool hal_codec_get_dig_dc_calib_value_high_dre_gain(int32_t *dc_l, int32_t *dc_r)
{
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg = &dac_dre_calib_cfg[0];

    if (dc_l) {
        *dc_l = (cfg->valid & (1<<0)) ? cfg->dc_l : 0;
    }
    if (dc_r) {
        *dc_r = (cfg->valid & (1<<1)) ? cfg->dc_r : 0;
    }
    return true;
}

int hal_codec_check_dac_dc_offset(bool major, int range_idx, int32_t dc_l, int32_t dc_r)
{
#define MAJOR_DC_BITS     (19)
#define MAJOR_DC_MAX      ((1<<(MAJOR_DC_BITS-1))-1)
#define MAJOR_DC_MIN      (-(1<<(MAJOR_DC_BITS-1)))
#define MINOR_DC_BITS     (16)
#define MINOR_DC_MAX      ((1<<(MINOR_DC_BITS-1))-1)
#define MINOR_DC_MIN      (-(1<<(MINOR_DC_BITS-1)))

#define MAJOR_DET_DC      (200)
#define PROD_MAJOR_DC_MAX (MAJOR_DC_MAX - MAJOR_DET_DC)
#define PROD_MAJOR_DC_MIN (MAJOR_DC_MIN + MAJOR_DET_DC)
#define MINOR_DET_DC      (200)
#define PROD_MINOR_DC_MAX (MINOR_DC_MAX - MINOR_DET_DC)
#define PROD_MINOR_DC_MIN (MINOR_DC_MIN + MINOR_DET_DC)

    int r = 0;
    int32_t max_dc;
    int32_t min_dc;

    if (range_idx == 0) {
        if (major) {
            max_dc = MAJOR_DC_MAX;
            min_dc = MAJOR_DC_MIN;
        } else {
            max_dc = MINOR_DC_MAX;
            min_dc = MINOR_DC_MIN;
        }
    } else {
        if (major) {
            max_dc = PROD_MAJOR_DC_MAX;
            min_dc = PROD_MAJOR_DC_MIN;
        } else {
            max_dc = PROD_MINOR_DC_MAX;
            min_dc = PROD_MINOR_DC_MIN;
        }
    }
    if (dc_l > max_dc) {
        r |= 0x01;
    } else if (dc_l < min_dc) {
        r |= 0x02;
    }
    if (dc_r > max_dc) {
        r |= 0x04;
    } else if (dc_r < min_dc) {
        r |= 0x08;
    }
    if (r) {
        TRACE(0, "[%s]: Invalid DAC DC, major=%d, dc_l=%d, dc_r=%d, min=%d, max=%d",
            __func__, major, dc_l, dc_r, min_dc, max_dc);
    }
    return r;
}

static bool hal_codec_get_dig_dc_calib_value(int32_t *dc_l, int32_t *dc_r)
{
    int32_t dc_val_l = 0, dc_val_r = 0;
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg = dac_dre_calib_cfg;

#ifdef DAC_DRE_ENABLE
    dc_val_l = ((int32_t)(cfg[0].dc_l) + (int32_t)(cfg[1].dc_l)) / 2;
    dc_val_r = ((int32_t)(cfg[0].dc_r) + (int32_t)(cfg[1].dc_r)) / 2;
#else
    dc_val_l = (int32_t)(cfg[0].dc_l);
    dc_val_r = (int32_t)(cfg[0].dc_r);
#endif

    int r = hal_codec_check_dac_dc_offset(true, 0, dc_val_l, dc_val_r);
    ASSERT(r==0,"[%s]: check dc offset error, r=%d", __func__, r);
    if (dc_l) {
        *dc_l = dc_val_l;
    }
    if (dc_r) {
        *dc_r = dc_val_r;
    }

    TRACE(1, "CALIB_DIG_DC: L=0x%x, R=0x%x",dc_l?(*dc_l):0, dc_r?(*dc_r):0);
    return true;
}

static bool hal_codec_set_ana_dc_calib_value(void)
{
    bool success = false;
    uint16_t ana_dc_l = 0, ana_dc_r = 0;
    uint32_t i, ini_gain, gain_offs, ana_gain;
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg = dac_dre_calib_cfg;
    uint16_t dac_dev = analog_codec_get_dac_dev();

    if (GET_BITFIELD(dac_dev, PMU_CODEC_DAC)) {
        analog1809_aud_get_dre_cfg(&ini_gain, &gain_offs);
    }
    if (GET_BITFIELD(dac_dev, MCU_CODEC_DAC)) {
        ini_gain  = GET_BITFIELD(codec->REG_350, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0);
        gain_offs = GET_BITFIELD(codec->REG_354, CODEC_CODEC_DRE_GAIN_OFFSET_CH0);
    }
    if (gain_offs <= 0xF) { //[0,+15]
        ana_gain  = 0xF - ini_gain + gain_offs;
    } else if (gain_offs > 0x10) { //(-16,-1]
        ana_gain =  0xF - ini_gain - (32 - gain_offs);
        ASSERT(ana_gain <= 30, "[%s]: bad ana_gain=%d,ini_gain=%d,gain_offs=%d",
            __func__, ana_gain, ini_gain, gain_offs);
    } else { //-16
        ASSERT(false, "[%s]: invalid gain_offs=%d, ini_gain=%d", __func__, gain_offs, ini_gain);
    }
#if defined(AUDIO_ANA_DC_CALIB_USE_COMP_1603)
    for (i = 0; i < ARRAY_SIZE(dac_dre_calib_cfg); i++, cfg++) {
        if (cfg->valid & (1<<0)) {
            ana_dc_l = cfg->ana_dc_l;
        }
        if (cfg->valid & (1<<1)) {
            ana_dc_r = cfg->ana_dc_r;
        }
        success = true;
        analog_aud_dc_calib_set_dre_ana_dc(i, ana_dc_l, ana_dc_r);
        TRACE(0, "cfg[%d], ana_dc_l = %x, ana_dc_r = %x", i, ana_dc_l, ana_dc_r);
    }
#else
    for (i = 0; i < ARRAY_SIZE(dac_dre_calib_cfg); i++, cfg++) {
        if (ana_gain == cfg->ana_gain) {
            if (cfg->valid & (1<<0)) {
                ana_dc_l = cfg->ana_dc_l;
            }
            if (cfg->valid & (1<<1)) {
                ana_dc_r = cfg->ana_dc_r;
            }
            success = true;
            break;
        }
    }
#endif
    analog_aud_dc_calib_set_value(ana_dc_l, ana_dc_r);
    analog_aud_dc_calib_enable(true);
    TRACE(1, "CALIB_ANA_DC: L=0x%x, R=0x%x, gain=0x%x",ana_dc_l, ana_dc_r, ana_gain);
    return success;
}
#endif /* AUDIO_OUTPUT_DC_AUTO_CALIB */

static void hal_codec_dac_dre_init(void)
{
    if (analog_codec_get_dac_dev() & (PMU_CODEC_DAC_L | PMU_CODEC_DAC_R)) {
        analog1809_aud_dac_dre_init();
    }
    uint8_t max_gain, ini_gain, dre_offset;

    ini_gain = 0;
    max_gain = analog_aud_get_max_dre_gain();
    if (max_gain >= 0xF) {
        dre_offset = max_gain - 0xF;
    } else {
        dre_offset = 32 - (0xF - max_gain);
    }
    codec->REG_350 = CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(ini_gain);
    codec->REG_354 = CODEC_CODEC_DRE_GAIN_OFFSET_CH0(dre_offset);
    codec->REG_1B8 = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0(0);
    codec->REG_358 = CODEC_CODEC_DRE_GAIN_TOP_CH0(max_gain);

    codec->REG_35C = CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(ini_gain);
    codec->REG_360 = CODEC_CODEC_DRE_GAIN_OFFSET_CH1(dre_offset);
    codec->REG_1BC = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1(0);
    codec->REG_364 = CODEC_CODEC_DRE_GAIN_TOP_CH1(max_gain);
}

#ifdef DAC_DRE_ENABLE

#ifdef DRE_MAX_INI_ANA_GAIN_FIRST
void hal_codec_dac_dre_get_max_ini_ana_gain(uint8_t *p_ini_gain)
{
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cal = dac_dre_calib_cfg;
    uint8_t max_val = cal->ini_ana_gain;
    uint32_t i;

    for (i = 0; i < ARRAY_SIZE(dac_dre_calib_cfg); i++, cal++) {
        if (cal->ini_ana_gain > max_val){
            max_val = cal->ini_ana_gain;
        }
    }
    if (p_ini_gain) {
        *p_ini_gain = max_val;
    }
}
#endif

static bool hal_codec_dac_dre_setup_calib_param(struct CODEC_DAC_DRE_CFG_T *cfg)
{
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cal = dac_dre_calib_cfg;
    uint32_t i;
    uint16_t dac_dev = analog_codec_get_dac_dev();
    int32_t dc_l;
    int32_t gain_l;
    uint32_t reg_offs[]={0,0,4,4,8,8,12,12,16,16,20,20,24,24,28,28};
    uint32_t mask_flag, dc_reg_addr, gain_reg_addr;
    bool set = false;

    cfg->step_mode    = cal->step_mode;
    cfg->ini_ana_gain = cal->ini_ana_gain;
    cfg->gain_offset  = cal->gain_offset;
    cfg->top_gain     = cal->top_gain;

    for (i = 0; i < ARRAY_SIZE(dac_dre_calib_cfg); i++, cal++) {
        if (cal->valid & 0x1) {
            if (dac_dev & MCU_CODEC_DAC_L) {
                dc_l = (int32_t)(cal->dc_l - dac_dc_l);
                gain_l = (int32_t)(cal->gain_l * (float)2048);
                set = true;
            } else if (dac_dev & MCU_CODEC_DAC_R) {
                dc_l = (int32_t)((int32_t)(cal->dc_r) - dac_dc_r);
                gain_l = (int32_t)(cal->gain_r * (float)2048);
                set = true;
            }
        }
        if (set) {
            uint8_t dc_step = cal->ini_ana_gain;
            ASSERT(dc_step < ARRAY_SIZE(reg_offs), "%s:invalid dc_step=%d",__func__,dc_step);
            mask_flag = dc_step % 2;
            dc_reg_addr = (uint32_t)&(codec->REG_3A8) + reg_offs[dc_step];
            gain_reg_addr = (uint32_t)&(codec->REG_368) + reg_offs[dc_step];

            int r = hal_codec_check_dac_dc_offset(false, 0, dc_l, 0);
            ASSERT(r==0,"[%s]: check dc offset error, r=%d", __func__, r);

            if (mask_flag) {
                *(volatile uint32_t *)dc_reg_addr = CODEC_CODEC_DAC_DRE_DC1_CH0(dc_l);
                *(volatile uint32_t *)gain_reg_addr = CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH0(gain_l);
            } else {
                *(volatile uint32_t *)dc_reg_addr = CODEC_CODEC_DAC_DRE_DC0_CH0(dc_l);
                *(volatile uint32_t *)gain_reg_addr = CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH0(gain_l);
            }
#ifdef DRE_DBG
            TRACE(1,"[%s]: dc_step=%d, mask_flag=%d, ana_gain=%d, ini_gain=%d, gain_offset=%d", __func__,
                dc_step, mask_flag, cal->ana_gain, cal->ini_ana_gain, cal->gain_offset);
            TRACE(1,"DAC_L: dc [%x]=%x, gain [%x]=%x", dc_reg_addr, dc_l, gain_reg_addr, gain_l);
#endif
        }
    }
    return true;
}

void hal_codec_dac_dre_enable(void)
{
    uint32_t dac_dev = analog_codec_get_dac_dev();
    if (GET_BITFIELD(dac_dev, PMU_CODEC_DAC)) {
        analog1809_aud_dac_dre_enable(dac_dc_l, dac_dc_r, codec_dac_ch_map);
    }
    struct CODEC_DAC_DRE_CFG_T *cfg = &dac_dre_cfg;
    uint8_t ini_ana_gain = cfg->ini_ana_gain;

    hal_codec_dac_dre_setup_calib_param(cfg);
#ifdef DRE_MAX_INI_ANA_GAIN_FIRST
    hal_codec_dac_dre_get_max_ini_ana_gain(&ini_ana_gain);
#endif
    // enable DAC CH0 DRE
    if (GET_BITFIELD(dac_dev, MCU_CODEC_DAC) & CODEC_OUTPUT_DEV) {
        codec->REG_34C = SET_BITFIELD(codec->REG_34C, CODEC_CODEC_DRE_DELAY_HL_CH0, cfg->dre_delay_hl)
                        | CODEC_CODEC_DRE_DELAY_MODE_CH0;
        codec->REG_350 = CODEC_CODEC_DRE_STEP_MODE_CH0(cfg->step_mode)
                        | CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(ini_ana_gain)
                        | CODEC_CODEC_DRE_DELAY_LH_CH0(cfg->dre_delay_lh)
                        | CODEC_CODEC_DRE_AMP_HIGH_CH0(cfg->amp_high);

        codec->REG_354 = CODEC_CODEC_DRE_WINDOW_CH0(cfg->dre_win)
                        | CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(cfg->thd_db_offset)
                        | CODEC_CODEC_DRE_GAIN_OFFSET_CH0(cfg->gain_offset);
        if (cfg->thd_db_offset_sign) {
            codec->REG_354 |= CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH0;
        }

        codec->REG_358 = CODEC_CODEC_DRE_DB_HIGH_CH0(cfg->db_high)
                        | CODEC_CODEC_DRE_DB_LOW_CH0(cfg->db_low)
                        | CODEC_CODEC_DRE_GAIN_TOP_CH0(cfg->top_gain)
                        | CODEC_CODEC_DRE_DELAY_DC_LH_CH0(cfg->delay_dc_lh)
                        | CODEC_CODEC_DRE_DELAY_DC_HL_CH0(cfg->delay_dc_hl);

        codec->REG_350 |= CODEC_CODEC_DRE_ENABLE_CH0;
    }
}

void hal_codec_dac_dre_disable(void)
{
    if (analog_codec_get_dac_dev() & (PMU_CODEC_DAC_L | PMU_CODEC_DAC_R)) {
        analog1809_aud_dac_dre_disable();
    }
    // disable DAC CH0 DRE
    if (CODEC_OUTPUT_DEV & AUD_CHANNEL_MAP_CH0) {
        codec->REG_350 &= ~CODEC_CODEC_DRE_ENABLE_CH0;
    }
    // disable DAC CH1 DRE
    if (CODEC_OUTPUT_DEV & AUD_CHANNEL_MAP_CH1) {
        codec->REG_35C &= ~CODEC_CODEC_DRE_ENABLE_CH1;
    }
}
#endif

#ifdef PERF_TEST_POWER_KEY
static void hal_codec_update_perf_test_power(void)
{
    int32_t nominal_vol;
    uint32_t ini_ana_gain;
    int32_t dac_vol;

    if (!codec_opened) {
        return;
    }

    dac_vol = 0;
    if (cur_perft_power == HAL_CODEC_PERF_TEST_30MW) {
        nominal_vol = 0;
        ini_ana_gain = 0;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_10MW) {
        nominal_vol = -4;
        ini_ana_gain = 6;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_5MW) {
        nominal_vol = -7;
        ini_ana_gain = 0xA;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_M60DB) {
        nominal_vol = -60;
        ini_ana_gain = 0xF; // about -11 dB
        dac_vol = -49;
    } else {
        return;
    }

    if (codec->REG_350 & CODEC_CODEC_DRE_ENABLE_CH0) {
        dac_vol = nominal_vol;
    } else {
        codec->REG_350 = SET_BITFIELD(codec->REG_350, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0, ini_ana_gain);
        // keep DAC CH1 gain is same with DAC CH0
        codec->REG_35C = SET_BITFIELD(codec->REG_35C, CODEC_CODEC_DRE_INI_ANA_GAIN_CH1, ini_ana_gain);
    }

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(dac_vol);
#else
    hal_codec_set_dig_dac_gain(VALID_DAC_MAP, dac_vol);
#endif

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    if (codec_nr_enabled) {
        codec_nr_enabled = false;
        hal_codec_set_noise_reduction(true);
    }
#endif
}

void hal_codec_dac_gain_m60db_check(enum HAL_CODEC_PERF_TEST_POWER_T type)
{
    cur_perft_power = type;

    if (!codec_opened || (codec->REG_0BC & CODEC_CODEC_DAC_UH_EN) == 0) {
        return;
    }

    hal_codec_update_perf_test_power();
}
#endif

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
void hal_codec_set_noise_reduction(bool enable)
{
    uint32_t ini_ana_gain;

    if (codec_nr_enabled == enable) {
        // Avoid corrupting digdac_gain_offset_nr or using an invalid one
        return;
    }

    codec_nr_enabled = enable;

    if (!codec_opened) {
        return;
    }

    // ini_ana_gain=0   -->   0dB
    // ini_ana_gain=0xF --> -11dB
    if (enable) {
        ini_ana_gain = GET_BITFIELD(codec->REG_350, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0);
        digdac_gain_offset_nr = ((0xF - ini_ana_gain) * 11 + 0xF / 2) / 0xF;
        ini_ana_gain = 0xF;
    } else {
        ini_ana_gain = 0xF - (digdac_gain_offset_nr * 0xF + 11 / 2) / 11;
        digdac_gain_offset_nr = 0;
    }

    codec->REG_350 = SET_BITFIELD(codec->REG_350, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0, ini_ana_gain);
    // keep DAC CH1 gain is same with DAC CH0
    codec->REG_35C = SET_BITFIELD(codec->REG_35C, CODEC_CODEC_DRE_INI_ANA_GAIN_CH1, ini_ana_gain);

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(swdac_gain);
#else
    hal_codec_restore_dig_dac_gain();
#endif
}
#endif

#ifdef CODEC_SW_SYNC
#define CODEC_DAC_HW_ASSERT(id) ASSERT(((id)==CODEC_HW_MOD_DAC1) \
    ||((id)==CODEC_HW_MOD_DAC2) \
    ||((id)==CODEC_HW_MOD_DAC3),"%s: invalid id %d", __func__, id);

#define CODEC_ADC_HW_ASSERT(id) ASSERT(((id)==CODEC_HW_MOD_ADC1) \
    ||((id)==CODEC_HW_MOD_ADC2), "%s: invalid id %d", __func__, id);

static void hal_codec_int_sw_sync_play_open(enum CODEC_HW_MOD_T id)
{
    CODEC_DAC_HW_ASSERT(id);
    codec_sw_sync_play_status[DAC_MOD_IDX(id)] = true;
}

static void hal_codec_int_sw_sync_play_close(enum CODEC_HW_MOD_T id)
{
    CODEC_DAC_HW_ASSERT(id);
    codec_sw_sync_play_status[DAC_MOD_IDX(id)] = false;
}

static void hal_codec_int_sw_sync_play_enable(enum CODEC_HW_MOD_T id)
{
    CODEC_DAC_HW_ASSERT(id);

    if (codec_sw_sync_play_status[DAC_MOD_IDX(id)] == true) {
        switch (id) {
        case CODEC_HW_MOD_DAC1:
            codec->REG_000 |= CODEC_DAC_ENABLE;
            break;
        case CODEC_HW_MOD_DAC2:
            codec->REG_700 |= CODEC_DAC_ENABLE_SND;
            break;
        case CODEC_HW_MOD_DAC3:
            codec->REG_798 |= CODEC_CODEC_DAC_EN_TRI;
            break;
        default:
            break;
        }
    }
}

void hal_codec_sw_sync_play_open(void)
{
    hal_codec_int_sw_sync_play_open(CODEC_HW_MOD_DAC1);
}

void hal_codec_sw_sync_play_close(void)
{
    hal_codec_int_sw_sync_play_close(CODEC_HW_MOD_DAC1);
}

void hal_codec_sw_sync_play_enable(void)
{
    hal_codec_int_sw_sync_play_enable(CODEC_HW_MOD_DAC1);
}

void hal_codec_sw_sync_play2_open(void)
{
    hal_codec_int_sw_sync_play_open(CODEC_HW_MOD_DAC2);
}

void hal_codec_sw_sync_play2_close(void)
{
    hal_codec_int_sw_sync_play_close(CODEC_HW_MOD_DAC2);
}

void hal_codec_sw_sync_play2_enable(void)
{
    hal_codec_int_sw_sync_play_enable(CODEC_HW_MOD_DAC2);
}

void hal_codec_sw_sync_play3_open(void)
{
    hal_codec_int_sw_sync_play_open(CODEC_HW_MOD_DAC3);
}

void hal_codec_sw_sync_play3_close(void)
{
    hal_codec_int_sw_sync_play_close(CODEC_HW_MOD_DAC3);
}

void hal_codec_sw_sync_play3_enable(void)
{
    hal_codec_int_sw_sync_play_enable(CODEC_HW_MOD_DAC3);
}

static void hal_codec_int_sw_sync_cap_open(enum CODEC_HW_MOD_T id)
{
    CODEC_ADC_HW_ASSERT(id);
    codec_sw_sync_cap_status[ADC_MOD_IDX(id)] = true;
}

static void hal_codec_int_sw_sync_cap_close(enum CODEC_HW_MOD_T id)
{
    CODEC_ADC_HW_ASSERT(id);
    codec_sw_sync_cap_status[ADC_MOD_IDX(id)] = false;
}

static void hal_codec_int_sw_sync_cap_enable(enum CODEC_HW_MOD_T id)
{
    CODEC_ADC_HW_ASSERT(id);

    if (codec_sw_sync_cap_status[ADC_MOD_IDX(id)] == true) {
        switch (id) {
        case CODEC_HW_MOD_ADC1:
            codec->REG_000 |= CODEC_ADC_ENABLE;
            break;
        case CODEC_HW_MOD_ADC2:
            codec->REG_700 |= CODEC_ADC_SND_ENABLE;
            break;
        default:
            break;
        }
    }
}

void hal_codec_sw_sync_cap_open(void)
{
    hal_codec_int_sw_sync_cap_open(CODEC_HW_MOD_ADC1);
}

void hal_codec_sw_sync_cap_close(void)
{
    hal_codec_int_sw_sync_cap_close(CODEC_HW_MOD_ADC1);
}

void hal_codec_sw_sync_cap_enable(void)
{
    hal_codec_int_sw_sync_cap_enable(CODEC_HW_MOD_ADC1);
}

void hal_codec_sw_sync_cap2_open(void)
{
    hal_codec_int_sw_sync_cap_open(CODEC_HW_MOD_ADC2);
}

void hal_codec_sw_sync_cap2_close(void)
{
    hal_codec_int_sw_sync_cap_close(CODEC_HW_MOD_ADC2);
}

void hal_codec_sw_sync_cap2_enable(void)
{
    hal_codec_int_sw_sync_cap_enable(CODEC_HW_MOD_ADC2);
}

void hal_codec_sw_sync_low_latency_enable(uint32_t intv_us)
{
    uint32_t lock;

    lock = int_lock();

    // ADC2 enable
    codec->REG_700 |= CODEC_ADC_SND_ENABLE;
    hal_sys_timer_delay_us(intv_us);
    // DAC3 enable
    codec->REG_798 |= CODEC_CODEC_DAC_EN_TRI;

    int_unlock(lock);
}
#endif

#ifdef AUDIO_ANC_LOW_DELAY
static void hal_codec_dac_low_delay(bool enable)
{
    enum AUD_CHANNEL_MAP_T map = codec_dac_ch_map;

    if (enable) {
        if (map & AUD_CHANNEL_MAP_CH0) {
            codec->REG_0B4 |= CODEC_CODEC_DAC_SINC_CAS_CH0;
        }
        if (map & AUD_CHANNEL_MAP_CH1) {
            codec->REG_0B4 |= CODEC_CODEC_DAC_SINC_CAS_CH1;
        }
    } else {
        if (map & AUD_CHANNEL_MAP_CH0) {
            codec->REG_0B4 &= ~CODEC_CODEC_DAC_SINC_CAS_CH0;
        }
        if (map & AUD_CHANNEL_MAP_CH1) {
            codec->REG_0B4 &= ~CODEC_CODEC_DAC_SINC_CAS_CH1;
        }
    }
}

static void hal_codec_adc_low_delay(enum AUD_CHANNEL_MAP_T map, bool enable)
{
    uint32_t i, j, regs[] = {0,0};

    regs[0] = codec->REG_0A4;
    regs[1] = codec->REG_0A8;
    for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            j = ((i + 1) * 6) / 32;
            ASSERT(j < ARRAY_SIZE(regs), "%s: invalid idx %d", __func__, j);
            if (enable) {
                regs[j] |= ((CODEC_CODEC_ADC_SINC1_CAS_CH0(2)
                            |CODEC_CODEC_ADC_SINC3_CAS_CH0(2)) << (i*6));
            } else {
                regs[j] &= ~((CODEC_CODEC_ADC_SINC1_CAS_CH0(3)
                             |CODEC_CODEC_ADC_SINC3_CAS_CH0(3)) << (i*6));
            }
        }
    }
    codec->REG_0A4 = regs[0];
    codec->REG_0A8 = regs[1];
}
#endif /* AUDIO_ANC_LOW_DELAY */

static void hal_codec_dac_lh_enable(enum CODEC_DAC_LH_USER_T user)
{
    if (codec_dac_lh_user_map == 0) {
        codec->REG_0BC |= CODEC_CODEC_DAC_LH_EN;
    }
    if (!(codec_dac_lh_user_map & (1 << user))) {
        codec_dac_lh_user_map |= (1 << user);
    }
}

static void hal_codec_dac_lh_disable(enum CODEC_DAC_LH_USER_T user)
{
    if (codec_dac_lh_user_map & (1 << user)) {
        codec_dac_lh_user_map &= ~(1 << user);
    }
    if (!codec_dac_lh_user_map) {
        codec->REG_0BC &= ~CODEC_CODEC_DAC_LH_EN;
    }
}

void hal_codec_stop_playback_stream(enum HAL_CODEC_ID_T id)
{
#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!(defined(__TWS__) || defined(IBRT)) || defined(ANC_APP) || defined(AUDIO_OUTPUT_DAC2) || defined(AUDIO_OUTPUT_DAC3))
    // Disable PA
    analog_aud_codec_speaker_enable(false);
#endif
    codec->REG_0B0 &= ~(CODEC_CODEC_DAC_EN_CH0 | CODEC_CODEC_DAC_EN_CH1);

    codec->REG_0BC &= ~CODEC_CODEC_DAC_UH_EN;

#if defined(PSAP_APP)
    hal_codec_dac_lh_disable(CODEC_DAC_LH_USER_PSAP);
#endif

#ifdef AUDIO_ANC_FB_MC
    hal_codec_dac_lh_disable(CODEC_DAC_LH_USER_MC);
#endif

#ifdef AUDIO_ANC_LOW_DELAY
    hal_codec_dac_low_delay(false);
#endif

#ifdef DAC_DRE_ENABLE
    hal_codec_dac_dre_disable();
#endif

#if defined(DAC_CLASSG_ENABLE)
    hal_codec_classg_enable(false);
#endif

#ifndef NO_DAC_RESET
    // Reset DAC
    // Avoid DAC outputing noise after it is disabled
    codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC;
    codec->REG_064 |= CODEC_SOFT_RSTN_DAC;
#endif
#ifndef CODEC_TIMER
    codec->REG_0B4 &= ~CODEC_CODEC_DAC_GAIN_UPDATE;
    codec->REG_788 &= ~CODEC_CODEC_DAC_GAIN_UPDATE_SND;
    codec->REG_79C &= ~CODEC_CODEC_DAC_GAIN_UPDATE_TRI;
    codec->REG_1B8 &= ~CODEC_CODEC_DAC_DC_UPDATE_CH0;
    hal_codec_reg_update_delay();
    codec->REG_060 &= ~CODEC_EN_CLK_DAC;
#endif
}

void hal_codec_start_playback_stream(enum HAL_CODEC_ID_T id)
{
#ifndef CODEC_TIMER
    codec->REG_060 |= CODEC_EN_CLK_DAC;
#endif
#ifndef NO_DAC_RESET
    // Reset DAC
    codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC;
    codec->REG_064 |= CODEC_SOFT_RSTN_DAC;
#endif

#ifdef DAC_DRE_ENABLE
    if (
            //(codec->REG_044 & CODEC_MODE_16BIT_DAC) == 0 &&
#ifdef ANC_APP
            anc_adc_ch_map == 0 &&
#endif
            1
            )
    {
        hal_codec_dac_dre_enable();
    }
#endif

#ifdef PERF_TEST_POWER_KEY
    hal_codec_update_perf_test_power();
#endif

#if defined(DAC_CLASSG_ENABLE)
    hal_codec_classg_enable(true);
#endif

#ifdef AUDIO_ANC_LOW_DELAY
    hal_codec_dac_low_delay(true);
#endif

    codec->REG_0B0 |= (CODEC_CODEC_DAC_EN_CH0 | CODEC_CODEC_DAC_EN_CH1);

    codec->REG_0BC |= CODEC_CODEC_DAC_UH_EN;

#if defined(PSAP_APP)
    hal_codec_dac_lh_enable(CODEC_DAC_LH_USER_PSAP);
#endif

#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!(defined(__TWS__) || defined(IBRT)) || defined(ANC_APP) || defined(AUDIO_OUTPUT_DAC2) || defined(AUDIO_OUTPUT_DAC3))
#ifdef AUDIO_OUTPUT_DC_CALIB
    // At least delay 4ms for 8K-sample-rate mute data to arrive at DAC PA
    osDelay(5);
#endif

    // Enable PA
    analog_aud_codec_speaker_enable(true);

#ifdef AUDIO_ANC_FB_MC
    if (mc_started) {
        uint32_t lock;
        lock = int_lock();
        // MC FIFO and DAC FIFO must be started at the same time
        codec->REG_04C |= CODEC_MC_ENABLE;
        hal_codec_dac_lh_enable(CODEC_DAC_LH_USER_MC);
#ifdef CODEC_SW_SYNC
        if (!codec_sw_sync_play_status[DAC_MOD_IDX(CODEC_HW_MOD_DAC1)])
#endif
        {
            codec->REG_000 |= CODEC_DAC_ENABLE;
        }
        int_unlock(lock);
    }
#endif
#endif
}

#ifdef AF_ADC_I2S_SYNC
static bool _hal_codec_capture_enable_delay = false;

void hal_codec_capture_enable_delay(void)
{
    _hal_codec_capture_enable_delay = true;
}

void hal_codec_capture_enable(void)
{
    codec->REG_080 |= CODEC_CODEC_ADC_EN;
}
#endif

int hal_codec_start_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        // Reset and start DAC
        hal_codec_start_playback_stream(id);
    } else {
#if defined(AUDIO_ANC_FB_MC) || defined(CODEC_DSD)
        adc_en_map |= (1 << CODEC_ADC_EN_REQ_STREAM);
        if (adc_en_map == (1 << CODEC_ADC_EN_REQ_STREAM))
#endif
        {
#ifdef AF_ADC_I2S_SYNC
            if (_hal_codec_capture_enable_delay)
            {
                _hal_codec_capture_enable_delay = false;
            }
            else
            {
                hal_codec_capture_enable();
            }
#else
            codec->REG_080 |= CODEC_CODEC_ADC_EN;
#endif
        }
    }

    return 0;
}

int hal_codec_stop_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        // Stop and reset DAC
        hal_codec_stop_playback_stream(id);
    } else {
#if defined(AUDIO_ANC_FB_MC) || defined(CODEC_DSD)
        adc_en_map &= ~(1 << CODEC_ADC_EN_REQ_STREAM);
        if (adc_en_map == 0)
#endif
        {
            codec->REG_080 &= ~CODEC_CODEC_ADC_EN;
#ifdef AF_ADC_I2S_SYNC
            _hal_codec_capture_enable_delay = false;
#endif
        }
    }

    return 0;
}

#ifdef CODEC_DSD
void hal_codec_dsd_enable(void)
{
    dsd_enabled = true;
}

void hal_codec_dsd_disable(void)
{
    dsd_enabled = false;
}

static void hal_codec_dsd_cfg_start(void)
{
#if !(defined(FIXED_CODEC_ADC_VOL) && defined(SINGLE_CODEC_ADC_VOL))
    uint32_t vol;
    const CODEC_ADC_VOL_T *adc_gain_db;

    vol = hal_codec_get_mic_chan_volume_level(AUD_CHANNEL_MAP_DIGMIC_CH2);
    adc_gain_db = hal_codec_get_adc_volume(vol);
    if (adc_gain_db) {
        hal_codec_set_dig_adc_gain(AUD_CHANNEL_MAP_CH2, *adc_gain_db);
    }
#endif

    codec->REG_004 |= CODEC_DSD_RX_FIFO_FLUSH | CODEC_DSD_TX_FIFO_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_004 &= ~(CODEC_DSD_RX_FIFO_FLUSH | CODEC_DSD_TX_FIFO_FLUSH);

    codec->REG_150 = CODEC_CODEC_DSD_ENABLE_L | CODEC_CODEC_DSD_ENABLE_R | CODEC_CODEC_DSD_SAMPLE_RATE(dsd_rate_idx);
    codec->REG_048 = CODEC_DSD_IF_EN | CODEC_DSD_ENABLE | CODEC_DSD_DUAL_CHANNEL | CODEC_MODE_24BIT_DSD |
        /* CODEC_DMACTRL_RX_DSD | */ CODEC_DMACTRL_TX_DSD | CODEC_DSD_IN_16BIT;

    codec->REG_080 = (codec->REG_080 & ~CODEC_CODEC_LOOP_SEL_L_MASK) |
        CODEC_CODEC_ADC_LOOP | CODEC_CODEC_LOOP_SEL_L(2);

    codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH2, 2);
    codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH2;

    codec->REG_080 |= CODEC_CODEC_ADC_EN_CH2;

    if (adc_en_map == 0) {
        codec->REG_080 |= CODEC_CODEC_ADC_EN;
    }
    adc_en_map |= (1 << CODEC_ADC_EN_REQ_DSD);
}

static void hal_codec_dsd_cfg_stop(void)
{
    adc_en_map &= ~(1 << CODEC_ADC_EN_REQ_DSD);
    if (adc_en_map == 0) {
        codec->REG_080 &= ~CODEC_CODEC_ADC_EN;
    }

    codec->REG_080 &= ~CODEC_CODEC_ADC_EN_CH2;
    codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH2;
    codec->REG_048 = 0;
    codec->REG_150 = 0;

    codec->REG_080 &= ~CODEC_CODEC_ADC_LOOP;
}
#endif

#ifdef __AUDIO_RESAMPLE__
void hal_codec_resample_clock_enable(enum AUD_STREAM_T stream)
{
    uint32_t clk;

    // 192K-24BIT requires 52M clock, and 384K-24BIT requires 104M clock
    if (stream == AUD_STREAM_PLAYBACK) {
        clk = codec_dac_sample_rate[resample_rate_idx[AUD_STREAM_PLAYBACK]].sample_rate * RS_CLOCK_FACTOR;
        hal_codec_rs_enable(clk, RS_CLK_USER_DAC1);
    } else {
        clk = codec_adc_sample_rate[resample_rate_idx[AUD_STREAM_CAPTURE]].sample_rate * RS_CLOCK_FACTOR;
        hal_codec_rs_adc_enable(clk, RS_CLK_USER_ADC1);
    }
}

void hal_codec_resample_clock_disable(enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        hal_codec_rs_disable(RS_CLK_USER_DAC1);
    } else {
        hal_codec_rs_adc_disable(RS_CLK_USER_ADC1);
    }
}

void hal_codec_adc2_resample_clock_enable(enum AUD_STREAM_T stream)
{
    uint32_t clk;

    if (stream == AUD_STREAM_CAPTURE) {
        clk = codec_adc_sample_rate[adc2_resample_rate_idx].sample_rate * RS_CLOCK_FACTOR;
        hal_codec_rs_adc_enable(clk, RS_CLK_USER_ADC2);
    }
}

void hal_codec_adc2_resample_clock_disable(enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_CAPTURE) {
        hal_codec_rs_adc_disable(RS_CLK_USER_ADC2);
    }
}

void hal_codec_dac2_resample_clock_enable(enum AUD_STREAM_T stream)
{
    uint32_t clk;

    // 192K-24BIT requires 52M clock, and 384K-24BIT requires 104M clock
    if (stream == AUD_STREAM_PLAYBACK) {
        clk = codec_dac_sample_rate[dac2_resample_rate_idx].sample_rate * RS_CLOCK_FACTOR;
        hal_codec_rs_enable(clk, RS_CLK_USER_DAC2);
    }
}

void hal_codec_dac3_resample_clock_enable(enum AUD_STREAM_T stream)
{
    uint32_t clk;

    // 192K-24BIT requires 52M clock, and 384K-24BIT requires 104M clock
    if (stream == AUD_STREAM_PLAYBACK) {
        clk = codec_dac_sample_rate[dac3_resample_rate_idx].sample_rate * RS_CLOCK_FACTOR;
        hal_codec_rs_enable(clk, RS_CLK_USER_DAC3);
    }
}

void hal_codec_dac2_resample_clock_disable(enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        hal_codec_rs_disable(RS_CLK_USER_DAC2);
    }
}

void hal_codec_dac3_resample_clock_disable(enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        hal_codec_rs_disable(RS_CLK_USER_DAC3);
    }
}
#endif

#if defined(DIG_MIC_DYN_CODEC_VOLT) || defined(ANC_DYN_CODEC_VOLT)
static void hal_codec_codec_volt_ramp_up(enum CODEC_VOLT_RAMP_USER_T user)
{
#if defined(DIG_MIC_DYN_CODEC_VOLT) && defined(ANC_DYN_CODEC_VOLT)
    bool update = true;

    if (vcodec_ramp_user) {
        update = false;
    }
    vcodec_ramp_user |= user;
    if (!update) {
        return;
    }
#endif

    pmu_codec_volt_ramp_up();
}

static void hal_codec_codec_volt_ramp_down(enum CODEC_VOLT_RAMP_USER_T user)
{
#if defined(DIG_MIC_DYN_CODEC_VOLT) && defined(ANC_DYN_CODEC_VOLT)
    vcodec_ramp_user &= ~user;
    if (vcodec_ramp_user) {
        return;
    }
#endif

    pmu_codec_volt_ramp_down();
}
#endif

static void hal_codec_enable_dig_mic(enum DIG_MIC_USER_T user, enum AUD_CHANNEL_MAP_T mic_map)
{
    uint32_t phase = 0;
    uint32_t line_map = 0;
    int set = 0;

    if (dig_mic_user_map == 0) {
        set = 1;
    }
    dig_mic_user_map |= (1 << user);

//    TRACE(1, "[%s]: user=%d, mic_map=%x, set=%d, map=%x", __func__, user, mic_map, set, dig_mic_user_map);

    if (set) {
#ifdef DIG_MIC_DYN_CODEC_VOLT
        if ((codec->REG_0C4 & CODEC_CODEC_PDM_ENABLE) == 0) {
            hal_codec_codec_volt_ramp_up(CODEC_VOLT_RAMP_USER_DIG_MIC);
        }
#endif
        phase = codec->REG_0C8;
        for (int i = 0; i < MAX_DIG_MIC_CH_NUM; i++) {
            if (mic_map & (AUD_CHANNEL_MAP_DIGMIC_CH0 << i)) {
                line_map |= (1 << (i / 2));
            }
            phase = (phase & ~(CODEC_CODEC_PDM_CAP_PHASE_CH0_MASK << (i * 2))) |
                (CODEC_CODEC_PDM_CAP_PHASE_CH0(codec_digmic_phase) << (i * 2));
        }
        codec->REG_0C8 = phase;
        codec->REG_0C4 |= CODEC_CODEC_PDM_ENABLE;
        hal_iomux_set_dig_mic(line_map);
    }
}

static void hal_codec_disable_dig_mic(enum DIG_MIC_USER_T user)
{
    int set = 0;

    dig_mic_user_map &= ~(1 << user);
    if (dig_mic_user_map == 0) {
        set = 1;
    }
//    TRACE(1, "[%s]: user=%d, set=%d, map=%x", __func__, user, set, dig_mic_user_map);

    if (set) {
        codec->REG_0C4 &= ~CODEC_CODEC_PDM_ENABLE;
#ifdef DIG_MIC_DYN_CODEC_VOLT
        hal_codec_codec_volt_ramp_down(CODEC_VOLT_RAMP_USER_DIG_MIC);
#endif
    }
}

static void hal_codec_ec_gain_init(void)
{
    // 0dB gain for EC0 & EC1 channel
    codec->REG_09C = (codec->REG_09C & ~(CODEC_CODEC_EC_GAIN_SEL_CH0 | CODEC_CODEC_EC_GAIN_CH0_MASK)) |
        CODEC_CODEC_EC_GAIN_CH0(1 << 12);
    codec->REG_0A0 = (codec->REG_0A0 & ~(CODEC_CODEC_EC_GAIN_SEL_CH1 | CODEC_CODEC_EC_GAIN_CH1_MASK)) |
        CODEC_CODEC_EC_GAIN_CH1(1 << 12);
    codec->REG_0B4 &= ~(CODEC_CODEC_EC_GAIN_UPDATE_CH0 | CODEC_CODEC_EC_GAIN_UPDATE_CH1);
    hal_codec_reg_update_delay();
    codec->REG_0B4 |= (CODEC_CODEC_EC_GAIN_UPDATE_CH0 | CODEC_CODEC_EC_GAIN_UPDATE_CH1);
}

static void hal_codec_ec_enable(enum AUD_CHANNEL_MAP_T map)
{
    if (map & AUD_CHANNEL_MAP_CH4) {
        codec->REG_09C |= CODEC_CODEC_ECHO_ENABLE_CH0;
    }
    if (map & AUD_CHANNEL_MAP_CH5) {
        codec->REG_0A0 |= CODEC_CODEC_ECHO_ENABLE_CH1;
    }
}

static void hal_codec_ec_disable(enum AUD_CHANNEL_MAP_T map)
{
    if (map & AUD_CHANNEL_MAP_CH4) {
        codec->REG_09C &= ~CODEC_CODEC_ECHO_ENABLE_CH0;
    }
    if (map & AUD_CHANNEL_MAP_CH5) {
        codec->REG_0A0 &= ~CODEC_CODEC_ECHO_ENABLE_CH1;
    }
}

uint32_t hal_codec_get_echo_path(void)
{
    return GET_BITFIELD(codec->REG_0BC, CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0);
}

uint32_t hal_codec_get_echo1_path(void)
{
    return GET_BITFIELD(codec->REG_0BC, CODEC_CODEC_DAC_ECHO_DATA_SEL_CH1);
}
void hal_codec_set_echo_rate(enum HAL_CODEC_ECHO_RATE_T rate)
{
    codec->REG_09C = SET_BITFIELD(codec->REG_09C, CODEC_CODEC_ECHO_RATE_SEL_CH0, rate);
}

void hal_codec_set_echo_ch1_rate(enum HAL_CODEC_ECHO_RATE_T rate)
{
    codec->REG_0A0 = SET_BITFIELD(codec->REG_0A0, CODEC_CODEC_ECHO_RATE_SEL_CH1, rate);
}

//NOTE: hal_codec_set_echo_ch: chan=4/5, set ch1;
void hal_codec_set_echo_ch(enum HAL_CODEC_ECHO_CHAN_T chan)
{
    codec->REG_09C = SET_BITFIELD(codec->REG_09C, CODEC_CODEC_ECHO_SEL_CH0, chan);
}

//NOTE: hal_codec_set_echo_ch1: chan=4/5, set ch0;
void hal_codec_set_echo_ch1(enum HAL_CODEC_ECHO_CHAN_T chan)
{
    codec->REG_0A0 = SET_BITFIELD(codec->REG_0A0, CODEC_CODEC_ECHO_SEL_CH1, chan);
}

void hal_codec_set_echo_path(enum HAL_CODEC_ECHO_PATH_T path)
{
    codec->REG_0BC = SET_BITFIELD(codec->REG_0BC, CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0, path);
}

void hal_codec_set_echo1_path(enum HAL_CODEC_ECHO_PATH_T path)
{
    codec->REG_0BC = SET_BITFIELD(codec->REG_0BC, CODEC_CODEC_DAC_ECHO_DATA_SEL_CH1, path);
}

#if defined(CODEC_DAC1_USF) || defined(CODEC_DAC2_USF) || defined(CODEC_DAC3_USF)
static void hal_codec_dac_usf_clock_enable(enum HAL_CODEC_USF_T usf, bool enable)
{
    uint32_t mask = 1 << usf;

    if (enable) {
        codec->REG_060 |= CODEC_EN_CLK_DAC_USF(mask);
        codec->REG_064 |= CODEC_SOFT_RSTN_DAC_USF(mask);
    } else {
        codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC_USF(mask);
        codec->REG_060 &= ~CODEC_EN_CLK_DAC_USF(mask);
    }
}

static int hal_codec_dac_usf_set_speed(enum HAL_CODEC_USF_T usf, int speed)
{
    uint32_t mask;
    uint32_t val;
    uint32_t div = 1;
    int max_speed = 0;
    bool set = false;
    uint32_t i;

    dac_usf_clk_speed[usf] = speed;
    for (i = 0; i < ARRAY_SIZE(dac_usf_clk_speed); i++) {
        if (max_speed < dac_usf_clk_speed[i]) {
            max_speed = dac_usf_clk_speed[i];
        }
    }

    if (max_speed != dac_usf_clk_cur_speed) {
        dac_usf_clk_cur_speed = max_speed;
        set = true;
    }

    if (set) {
        speed = dac_usf_clk_cur_speed;
        mask = CODEC_SEL_OSC_DAC_USF
            | CODEC_SEL_OSCX2_DAC_USF
            | CODEC_SEL_OSC_2_DAC_USF
            | CODEC_CFG_DIV_CODEC_DAC_USF_MASK;

        if (speed <= 12000000) {
            val = CODEC_SEL_OSC_DAC_USF | CODEC_SEL_OSCX2_DAC_USF | CODEC_SEL_OSC_2_DAC_USF;
        } else if (speed <= 24000000) {
            val = CODEC_SEL_OSC_DAC_USF | CODEC_SEL_OSCX2_DAC_USF;
        } else if (speed <= 48000000) {
            val = CODEC_SEL_OSCX2_DAC_USF;
        } else {
            div = 1;
            val = CODEC_CFG_DIV_CODEC_DAC_USF(div);
        }

        codec->REG_07C = (codec->REG_07C & ~mask) | val;
    }

    return 0;
}

static int hal_codec_dac_usf_get_speed(enum HAL_CODEC_USF_T usf)
{
    int speed = 0, max_speed = 0;
    uint32_t i;

    for (i = 0; i < ARRAY_SIZE(dac_usf_clk_speed); i++) {
        if (max_speed < dac_usf_clk_speed[i]) {
            max_speed = dac_usf_clk_speed[i];
        }
    }
    if (max_speed == 0) {
#ifdef __AUDIO_RESAMPLE__
        speed = 48*1000000;
#else
        speed = 96*1000000;
#endif
    } else {
        speed = max_speed;
    }
    return speed;
}
#endif

#if defined(CODEC_DAC1_USF)
void hal_codec_dac1_usf_enable(uint32_t ch_map)
{
    codec->REG_60C |= (CODEC_CODEC_DAC_USF_CH_ENABLE(ch_map) | CODEC_CODEC_DAC_USF_ENABLE);
}

void hal_codec_dac1_usf_disable(uint32_t ch_map)
{
    codec->REG_60C &= ~(CODEC_CODEC_DAC_USF_CH_ENABLE(ch_map) | CODEC_CODEC_DAC_USF_ENABLE);
}

void hal_codec_dac1_usf_start_interface(void)
{
    if (dac1_usf_status) {
        hal_codec_dac1_usf_enable(codec_dac_ch_map);
    }
}

void hal_codec_dac1_usf_stop_interface(void)
{
    if (dac1_usf_status) {
        hal_codec_dac1_usf_disable(codec_dac_ch_map);
        hal_codec_dac_usf_set_speed(HAL_CODEC_USF_DAC1, 0);
        hal_codec_dac_usf_clock_enable(HAL_CODEC_USF_DAC1, false);
        dac1_usf_status = false;
        dac1_usf_sr_idx = 0;
    }
}

static int hal_codec_dac1_usf_setup(uint32_t up_sample)
{
    uint32_t speed, i;
    uint32_t addr = CODEC_DAC1_USF_COEF_ADDR;
    struct CODEC_DAC_USF_PARAM_T *param = NULL;

    for (i = 0; i < ARRAY_SIZE(dac_usf_param); i++) {
        if (dac_usf_param[i].up_sample == up_sample) {
            param = &dac_usf_param[i];
            break;
        }
    }
    ASSERT(param != NULL, "%s: invalid up_sample = %d", __func__, up_sample);

    hal_codec_dac_usf_clock_enable(HAL_CODEC_USF_DAC1, true);

    speed = hal_codec_dac_usf_get_speed(HAL_CODEC_USF_DAC1);

    hal_codec_dac_usf_set_speed(HAL_CODEC_USF_DAC1, speed);

    if (codec->REG_60C & CODEC_CODEC_DAC_USF_ENABLE) {
        codec->REG_60C &= ~CODEC_CODEC_DAC_USF_ENABLE;
    }

    // set USF coef
    volatile int32_t *pt = (volatile int32_t *)addr;
    volatile int32_t *ps = (volatile int32_t *)param->coef;
    for (i = 0; i < param->size / 4; i++) {
        pt[i] = ps[i];
    }

    codec->REG_60C = SET_BITFIELD(codec->REG_60C, CODEC_CODEC_DAC_USF_ORDER, param->order);
    codec->REG_60C = SET_BITFIELD(codec->REG_60C, CODEC_CODEC_UP_SEL_FIR, param->up_sel);
    codec->REG_60C = SET_BITFIELD(codec->REG_60C, CODEC_CODEC_DAC_USF_UP_RATIO, param->up_sample);
    codec->REG_60C |= CODEC_CODEC_DAC_USF_UPSAMPLE;

    return 0;
}

static int hal_codec_set_dac_up(uint32_t val);
static int hal_codec_set_dac_hbf_bypass_cnt(uint32_t cnt, int head_first);

static void hal_codec_dac1_usf_setup_param(int sample_rate)
{
    uint8_t usf_up_rate = 0;
    uint8_t dac_up_rate = 0;
    uint8_t bypass_cnt = 0;
    struct CODEC_DAC_USF_SAMPRATE_T *p_usf_sr = dac1_usf_sample_rate;
    uint32_t num = ARRAY_SIZE(dac1_usf_sample_rate);
    uint32_t i;

#ifdef ANC_APP
    if (sample_rate == AUD_SAMPRATE_50781) {
        return;
    }
#endif
    if(!dac1_usf_status || (sample_rate != dac1_usf_sample_rate[dac1_usf_sr_idx].sample_rate)) {
        for (i = 0; i < num; i++) {
            if (p_usf_sr[i].sample_rate == sample_rate) {
                usf_up_rate = p_usf_sr[i].usf_up_rate;
                dac_up_rate = p_usf_sr[i].dac_up_rate;
                bypass_cnt  = p_usf_sr[i].bypass_cnt;
                dac1_usf_sr_idx = i;
                break;
            }
        }
        ASSERT(i < num, "[%s]: invalid sample_rate=%d", __func__, sample_rate);
        TRACE(0, "DAC1 USF: sample_rate=%d, usf_up_rate=%d, dac_up_rate=%d, bypass_cnt=%d",
            sample_rate, usf_up_rate, dac_up_rate, bypass_cnt);
        hal_codec_dac1_usf_setup(usf_up_rate);
        hal_codec_set_dac_up(dac_up_rate);
        hal_codec_set_dac_hbf_bypass_cnt(bypass_cnt, false);
        dac1_usf_status = true;
    }
}
#endif /* CODEC_DAC1_USF */

#if defined(CODEC_DAC2_USF)
void hal_codec_dac2_usf_status(uint32_t ch_map)
{
    codec->REG_7B4 |= (CODEC_CODEC_DAC_SND_USF_CH_ENABLE(ch_map) | CODEC_CODEC_DAC_SND_USF_ENABLE);
}

void hal_codec_dac2_usf_disable(uint32_t ch_map)
{
    codec->REG_7B4 &= ~(CODEC_CODEC_DAC_SND_USF_CH_ENABLE(ch_map) | CODEC_CODEC_DAC_SND_USF_ENABLE);
}

void hal_codec_dac2_usf_start_interface()
{
    if (dac2_usf_status) {
        hal_codec_dac2_usf_status(codec_dac2_ch_map);
    }
}

void hal_codec_dac2_usf_stop_interface()
{
    if (dac2_usf_status) {
        hal_codec_dac2_usf_disable(codec_dac2_ch_map);
        hal_codec_dac_usf_set_speed(HAL_CODEC_USF_DAC2, 0);
        hal_codec_dac_usf_clock_enable(HAL_CODEC_USF_DAC2, false);
        dac2_usf_status = false;
        dac2_usf_sr_idx = 0;
    }
}

static int hal_codec_dac2_usf_setup(uint32_t up_sample)
{
    uint32_t speed, i;
    uint32_t addr = CODEC_DAC2_USF_COEF_ADDR;
    struct CODEC_DAC_USF_PARAM_T *param = NULL;

    for (i = 0; i < ARRAY_SIZE(dac_usf_param); i++) {
        if (dac_usf_param[i].up_sample == up_sample) {
            param = &dac_usf_param[i];
            break;
        }
    }
    ASSERT(param != NULL, "%s: invalid up_sample = %d", __func__, up_sample);

    hal_codec_dac_usf_clock_enable(HAL_CODEC_USF_DAC2, true);

    speed = hal_codec_dac_usf_get_speed(HAL_CODEC_USF_DAC2);

    hal_codec_dac_usf_set_speed(HAL_CODEC_USF_DAC2, speed);

    if (codec->REG_7B4 & CODEC_CODEC_DAC_SND_USF_ENABLE) {
        codec->REG_7B4 &= ~CODEC_CODEC_DAC_SND_USF_ENABLE;
    }

    // set USF coef
    volatile int32_t *pt = (volatile int32_t *)addr;
    volatile int32_t *ps = (volatile int32_t *)param->coef;
    for (i = 0; i < param->size / 4; i++) {
        pt[i] = ps[i];
    }

    TRACE(0, "[%s]: order=%d, up_sel=%d, up_sample=%d", __func__,
        param->order, param->up_sel, param->up_sample);

    codec->REG_7B4 = SET_BITFIELD(codec->REG_7B4, CODEC_CODEC_DAC_SND_USF_ORDER, param->order);
    codec->REG_7B4 = SET_BITFIELD(codec->REG_7B4, CODEC_CODEC_UP_SEL_FIR_SND, param->up_sel);
    codec->REG_7B4 = SET_BITFIELD(codec->REG_7B4, CODEC_CODEC_DAC_SND_USF_UP_RATIO, param->up_sample);
    codec->REG_7B4 |= CODEC_CODEC_DAC_SND_USF_UPSAMPLE;

    return 0;
}

static int hal_codec_set_dac2_up(uint32_t val);
static int hal_codec_set_dac2_hbf_bypass_cnt(uint32_t cnt);

static void hal_codec_dac2_usf_setup_param(int sample_rate)
{
    uint8_t usf_up_rate = 0;
    uint8_t dac_up_rate = 0;
    uint8_t bypass_cnt = 0;
    struct CODEC_DAC_USF_SAMPRATE_T *p_usf_sr = dac2_usf_sample_rate;
    uint32_t num = ARRAY_SIZE(dac2_usf_sample_rate);
    uint32_t i;

    if (!dac2_usf_status || (sample_rate != dac2_usf_sample_rate[dac2_usf_sr_idx].sample_rate)) {
        for (i = 0; i < num; i++) {
            if (p_usf_sr[i].sample_rate == sample_rate) {
                usf_up_rate = p_usf_sr[i].usf_up_rate;
                dac_up_rate = p_usf_sr[i].dac_up_rate;
                bypass_cnt  = p_usf_sr[i].bypass_cnt;
                dac2_usf_sr_idx = i;
                break;
            }
        }
        ASSERT(i < num, "[%s]: invalid sample_rate=%d", __func__, sample_rate);
        TRACE(0, "DAC2 USF: sample_rate=%d, usf_up_rate=%d, dac_up_rate=%d, bypass_cnt=%d",
            sample_rate, usf_up_rate, dac_up_rate, bypass_cnt);

        hal_codec_dac2_usf_setup(usf_up_rate);
        hal_codec_set_dac2_up(dac_up_rate);
        hal_codec_set_dac2_hbf_bypass_cnt(bypass_cnt);
        dac2_usf_status = true;
    }
}
#endif /* CODEC_DAC2_USF */

#if defined(CODEC_DAC3_USF)
void hal_codec_dac3_usf_status(uint32_t ch_map)
{
    codec->REG_7B8 |= (CODEC_CODEC_DAC_TRI_USF_CH_ENABLE(ch_map) | CODEC_CODEC_DAC_TRI_USF_ENABLE);
}

void hal_codec_dac3_usf_disable(uint32_t ch_map)
{
    codec->REG_7B8 &= ~(CODEC_CODEC_DAC_TRI_USF_CH_ENABLE(ch_map) | CODEC_CODEC_DAC_TRI_USF_ENABLE);
}

void hal_codec_dac3_usf_start_interface()
{
    if (dac3_usf_status) {
        hal_codec_dac3_usf_status(codec_dac3_ch_map);
    }
}

void hal_codec_dac3_usf_stop_interface()
{
    if (dac3_usf_status) {
        hal_codec_dac3_usf_disable(codec_dac3_ch_map);
        hal_codec_dac_usf_set_speed(HAL_CODEC_USF_DAC3, 0);
        hal_codec_dac_usf_clock_enable(HAL_CODEC_USF_DAC3, false);
        dac3_usf_status = false;
        dac3_usf_sr_idx = 0;
    }
}

static int hal_codec_dac3_usf_setup(uint32_t up_sample)
{
    uint32_t speed, i;
    uint32_t addr = CODEC_DAC3_USF_COEF_ADDR;
    struct CODEC_DAC_USF_PARAM_T *param = NULL;

    for (i = 0; i < ARRAY_SIZE(dac_usf_param); i++) {
        if (dac_usf_param[i].up_sample == up_sample) {
            param = &dac_usf_param[i];
            break;
        }
    }
    ASSERT(param != NULL, "%s: invalid up_sample = %d", __func__, up_sample);

    hal_codec_dac_usf_clock_enable(HAL_CODEC_USF_DAC3, true);

    speed = hal_codec_dac_usf_get_speed(HAL_CODEC_USF_DAC3);

    hal_codec_dac_usf_set_speed(HAL_CODEC_USF_DAC3, speed);

    if (codec->REG_7B8 & CODEC_CODEC_DAC_TRI_USF_ENABLE) {
        codec->REG_7B8 &= ~CODEC_CODEC_DAC_TRI_USF_ENABLE;
    }

    // set USF coef
    volatile int32_t *pt = (volatile int32_t *)addr;
    volatile int32_t *ps = (volatile int32_t *)param->coef;
    for (i = 0; i < param->size / 4; i++) {
        pt[i] = ps[i];
    }

    TRACE(0, "[%s]: order=%d, up_sel=%d, up_sample=%d", __func__,
        param->order, param->up_sel, param->up_sample);

    codec->REG_7B8 = SET_BITFIELD(codec->REG_7B8, CODEC_CODEC_DAC_TRI_USF_ORDER, param->order);
    codec->REG_7B8 = SET_BITFIELD(codec->REG_7B8, CODEC_CODEC_UP_SEL_FIR_TRI, param->up_sel);
    codec->REG_7B8 = SET_BITFIELD(codec->REG_7B8, CODEC_CODEC_DAC_TRI_USF_UP_RATIO, param->up_sample);
    codec->REG_7B8 |= CODEC_CODEC_DAC_TRI_USF_UPSAMPLE;

    return 0;
}

static int hal_codec_set_dac3_up(uint32_t val);
static int hal_codec_set_dac3_hbf_bypass_cnt(uint32_t cnt);

static void hal_codec_dac3_usf_setup_param(int sample_rate)
{
    uint8_t usf_up_rate = 0;
    uint8_t dac_up_rate = 0;
    uint8_t bypass_cnt = 0;
    struct CODEC_DAC_USF_SAMPRATE_T *p_usf_sr = dac3_usf_sample_rate;
    uint32_t num = ARRAY_SIZE(dac3_usf_sample_rate);
    uint32_t i;

    if (!dac3_usf_status || (sample_rate != dac3_usf_sample_rate[dac3_usf_sr_idx].sample_rate)) {
        for (i = 0; i < num; i++) {
            if (p_usf_sr[i].sample_rate == sample_rate) {
                usf_up_rate = p_usf_sr[i].usf_up_rate;
                dac_up_rate = p_usf_sr[i].dac_up_rate;
                bypass_cnt  = p_usf_sr[i].bypass_cnt;
                dac3_usf_sr_idx = i;
                break;
            }
        }
        ASSERT(i < num, "[%s]: invalid sample_rate=%d", __func__, sample_rate);
        TRACE(0, "DAC3 USF: sample_rate=%d, usf_up_rate=%d, dac_up_rate=%d, bypass_cnt=%d",
            sample_rate, usf_up_rate, dac_up_rate, bypass_cnt);

        hal_codec_dac3_usf_setup(usf_up_rate);
        hal_codec_set_dac3_up(dac_up_rate);
        hal_codec_set_dac3_hbf_bypass_cnt(bypass_cnt);
        dac3_usf_status = true;
    }
}
#endif /* CODEC_DAC3_USF */

#if defined(CODEC_DAC1_USF) || defined(CODEC_DAC2_USF) || defined(CODEC_DAC3_USF)
void hal_codec_dac_usf_start_interface(enum HAL_CODEC_USF_T usf)
{
    switch (usf) {
#if defined(CODEC_DAC1_USF)
    case HAL_CODEC_USF_DAC1:
        hal_codec_dac1_usf_start_interface();
        break;
#endif
#if defined(CODEC_DAC2_USF)
    case HAL_CODEC_USF_DAC2:
        hal_codec_dac2_usf_start_interface();
        break;
#endif
#if defined(CODEC_DAC3_USF)
    case HAL_CODEC_USF_DAC3:
        hal_codec_dac3_usf_start_interface();
        break;
#endif
    default:
        ASSERT(0, "[%s]: Invalid CODEC USF: %d", __func__, usf);
        break;
    }
}

void hal_codec_dac_usf_stop_interface(enum HAL_CODEC_USF_T usf)
{
    switch (usf) {
#if defined(CODEC_DAC1_USF)
    case HAL_CODEC_USF_DAC1:
        hal_codec_dac1_usf_stop_interface();
        break;
#endif
#if defined(CODEC_DAC2_USF)
    case HAL_CODEC_USF_DAC2:
        hal_codec_dac2_usf_stop_interface();
        break;
#endif
#if defined(CODEC_DAC3_USF)
    case HAL_CODEC_USF_DAC3:
        hal_codec_dac3_usf_stop_interface();
        break;
#endif
    default:
        ASSERT(0, "[%s]: Invalid CODEC USF: %d", __func__, usf);
        break;
    }
}

void hal_codec_dac_usf_setup_stream(enum HAL_CODEC_USF_T usf, enum AUD_SAMPRATE_T sample_rate)
{
    switch (usf) {
#if defined(CODEC_DAC1_USF)
    case HAL_CODEC_USF_DAC1:
        hal_codec_dac1_usf_setup_param(sample_rate);
        break;
#endif
#if defined(CODEC_DAC2_USF)
    case HAL_CODEC_USF_DAC2:
        hal_codec_dac2_usf_setup_param(sample_rate);
        break;
#endif
#if defined(CODEC_DAC3_USF)
    case HAL_CODEC_USF_DAC3:
        hal_codec_dac3_usf_setup_param(sample_rate);
        break;
#endif
    default:
        ASSERT(0, "[%s]: Invalid CODEC USF: %d", __func__, usf);
        break;
    }
}
#endif /* defined(CODEC_DAC1_USF) || defined(CODEC_DAC2_USF) || defined(CODEC_DAC3_USF) */

int hal_codec_start_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, int dma)
{
    uint32_t fifo_flush = 0;

    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef CODEC_DSD
        if (dsd_enabled) {
            hal_codec_dsd_cfg_start();
        }
#endif
#ifdef CODEC_MIN_PHASE
        if (min_phase_cfg & (1 << AUD_STREAM_PLAYBACK)) {
            if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH0) {
                codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH0;
                codec->REG_0B0 |= CODEC_CODEC_DAC_L_FIR_UPSAMPLE;
            }
            if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
                codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH1;
                codec->REG_0B0 |= CODEC_CODEC_DAC_R_FIR_UPSAMPLE;
            }
        }
#endif
#ifdef __AUDIO_RESAMPLE__
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) {
            hal_codec_resample_clock_enable(stream);
#if (defined(__TWS__) || defined(IBRT))
            enum HAL_CODEC_SYNC_TYPE_T sync_type;

            sync_type = GET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL);
            if (sync_type != HAL_CODEC_SYNC_TYPE_NONE) {
                codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
                codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                hal_codec_reg_update_delay();
                codec->REG_0EC = resample_phase_float_to_value(1.0f);
                hal_codec_reg_update_delay();
                codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                hal_codec_reg_update_delay();
                codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL, sync_type);
                hal_codec_reg_update_delay();
                codec->REG_0EC = resample_phase_float_to_value(get_playback_resample_phase());
                codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
            }
#endif
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE;
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_ENABLE;
        }
#endif
#if defined(CODEC_DAC1_USF)
        hal_codec_dac_usf_start_interface(HAL_CODEC_USF_DAC1);
#endif
        if ((codec->REG_000 & CODEC_ADC_ENABLE) && (codec_adc_ch_map & EC_ADC_MAP)) {
            enum AUD_CHANNEL_MAP_T ec_ch = (enum AUD_CHANNEL_MAP_T)(codec_adc_ch_map & EC_ADC_MAP);
            hal_codec_ec_enable(ec_ch);
        }
#ifdef DAC_RAMP_GAIN
        if (codec->REG_154 & CODEC_CODEC_RAMP_EN_CH0) {
            codec->REG_154 &= ~CODEC_CODEC_RAMP_EN_CH0;
            codec->REG_158 &= ~CODEC_CODEC_RAMP_EN_CH1;
            hal_codec_reg_update_delay();
            hal_codec_set_dac_gain_value(VALID_DAC_MAP, 0);
            hal_codec_reg_update_delay();
            codec->REG_154 |= CODEC_CODEC_RAMP_EN_CH0;
            codec->REG_158 |= CODEC_CODEC_RAMP_EN_CH1;
            hal_codec_reg_update_delay();
            hal_codec_restore_dig_dac_gain();
        }
#endif
#ifdef AUDIO_ANC_FB_MC
        fifo_flush |= CODEC_MC_FIFO_FLUSH;
#endif
        fifo_flush |= CODEC_TX_FIFO_FLUSH;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        if (dma) {
            codec->REG_008 = SET_BITFIELD(codec->REG_008, CODEC_CODEC_TX_THRESHOLD, HAL_CODEC_TX_FIFO_TRIGGER_LEVEL);
            codec->REG_000 |= CODEC_DMACTRL_TX;
            // Delay a little time for DMA to fill the TX FIFO before sending
            for (volatile int i = 0; i < 50; i++);
        }
#ifdef AUDIO_ANC_FB_MC
        if (mc_enabled) {
            if (mc_chan_num == AUD_CHANNEL_NUM_2) {
                codec->REG_04C |= CODEC_DUAL_CHANNEL_MC;
            } else {
                codec->REG_04C &= ~CODEC_DUAL_CHANNEL_MC;
            }
            if (mc_bits == AUD_BITS_16) {
                codec->REG_04C = (codec->REG_04C & ~CODEC_MODE_32BIT_MC) | CODEC_MODE_16BIT_MC;
            } else if (mc_bits == AUD_BITS_24) {
                codec->REG_04C &= ~(CODEC_MODE_16BIT_MC | CODEC_MODE_32BIT_MC);
            } else if (mc_bits == AUD_BITS_32) {
                codec->REG_04C = (codec->REG_04C & ~CODEC_MODE_16BIT_MC) | CODEC_MODE_32BIT_MC;
            } else {
                ASSERT(false, "%s: Bad mc bits: %u", __func__, mc_bits);
            }
            if (adc_en_map == 0) {
                codec->REG_080 |= CODEC_CODEC_ADC_EN;
            }
            adc_en_map |= (1 << CODEC_ADC_EN_REQ_MC);
            // If codec function has been enabled, start FIFOs directly;
            // otherwise, start FIFOs after PA is enabled
            if (codec->REG_0BC & CODEC_CODEC_DAC_UH_EN) {
                uint32_t lock;
                lock = int_lock();
                // MC FIFO and DAC FIFO must be started at the same time
                codec->REG_04C |= CODEC_MC_ENABLE;
#ifdef CODEC_SW_SYNC
                if (!codec_sw_sync_play_status[DAC_MOD_IDX(CODEC_HW_MOD_DAC1)])
#endif
                {
                    codec->REG_000 |= CODEC_DAC_ENABLE;
                }
                hal_codec_dac_lh_enable(CODEC_DAC_LH_USER_DAC1);
                int_unlock(lock);
            }
            mc_started = true;
        }
        else
#endif
        {
#ifdef CODEC_SW_SYNC
            if (!codec_sw_sync_play_status[DAC_MOD_IDX(CODEC_HW_MOD_DAC1)])
#endif
            {
                codec->REG_000 |= CODEC_DAC_ENABLE;
            }
#ifndef PSAP_APP
            hal_codec_dac_lh_enable(CODEC_DAC_LH_USER_DAC1);
#endif
        }
    } else {
#ifdef CODEC_MIN_PHASE
        if (min_phase_cfg & (1 << AUD_STREAM_CAPTURE)) {
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH1) {
                codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH1;
                codec->REG_0DC |= CODEC_CODEC_ADC_FIR_DS_EN_CH1;
            }
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH2) {
                codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH2;
                codec->REG_0D0 |= CODEC_CODEC_ADC_FIR_DS_EN_CH2;
            }
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH3) {
                codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH3;
                codec->REG_0D0 |= CODEC_CODEC_ADC_FIR_DS_EN_CH3;
            }
        }
#endif
#ifdef __AUDIO_RESAMPLE__
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) {
            hal_codec_resample_clock_enable(stream);
#if (defined(__TWS__) || defined(IBRT)) && defined(ANC_APP)
            enum HAL_CODEC_SYNC_TYPE_T sync_type;

            sync_type = GET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL);
            if (sync_type != HAL_CODEC_SYNC_TYPE_NONE) {
                codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
                codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                hal_codec_reg_update_delay();
                codec->REG_0F0 = resample_phase_float_to_value(1.0f);
                hal_codec_reg_update_delay();
                codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                hal_codec_reg_update_delay();
                codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL, sync_type);
                hal_codec_reg_update_delay();
                codec->REG_0F0 = resample_phase_float_to_value(get_capture_resample_phase());
                codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
            }
#endif
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_ENABLE;
        }
#endif
        if (codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            hal_codec_enable_dig_mic(DIG_MIC_USER_ADC1, codec_mic_ch_map);
        }
        if (codec_adc_ch_map & EC_ADC_MAP) {
            enum AUD_CHANNEL_MAP_T ec_ch = (enum AUD_CHANNEL_MAP_T)(codec_adc_ch_map & EC_ADC_MAP);
            hal_codec_ec_enable(ec_ch);
        }
        int adc_cnt = 0;
        for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                if ((codec->REG_080 & (CODEC_CODEC_ADC_EN_CH0 << i)) == 0) {
                    if (i < NORMAL_ADC_CH_NUM &&
                            (codec->REG_780 & (CODEC_CODEC_ADC_SND_EN_CH0 << i)) == 0) {
                        codec->REG_0AC |= (CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << i);
                        hal_codec_reg_update_delay();
                        codec->REG_0AC &= ~(CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << i);
                    }
                    codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << i);
                }
                codec->REG_000 |= (CODEC_ADC_ENABLE_CH0 << adc_cnt);
                adc_cnt++;
            }
        }

#ifdef CODEC_ADC1_DSF
        hal_codec_adc1_dsf_start_interface();
#endif

        fifo_flush = CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1
            | CODEC_RX_FIFO_FLUSH_CH2 | CODEC_RX_FIFO_FLUSH_CH3
            | CODEC_RX_FIFO_FLUSH_CH4 | CODEC_RX_FIFO_FLUSH_CH5;

        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        if (dma) {
            codec->REG_008 = SET_BITFIELD(codec->REG_008, CODEC_CODEC_RX_THRESHOLD, HAL_CODEC_RX_FIFO_TRIGGER_LEVEL);
            codec->REG_000 |= CODEC_DMACTRL_RX;
        }
#ifdef CODEC_SW_SYNC
        if (!codec_sw_sync_cap_status[ADC_MOD_IDX(CODEC_HW_MOD_ADC1)])
#endif
        {
            codec->REG_000 |= CODEC_ADC_ENABLE;
        }
    }

    return 0;
}

#if defined(CODEC_ADC1_DSF) || defined(CODEC_ADC2_DSF)
static void hal_codec_adc_dsf_clock_enable(enum ADC_DSF_ID_T id, bool enable)
{
    uint32_t mask = 1 << id;

    if (enable) {
        codec->REG_060 |= CODEC_EN_CLK_ADC_DSF(mask);
        codec->REG_064 |= CODEC_SOFT_RSTN_ADC_DSF(mask);
    } else {
        codec->REG_064 &= ~CODEC_SOFT_RSTN_ADC_DSF(mask);
        codec->REG_060 &= ~CODEC_EN_CLK_ADC_DSF(mask);
    }
}

static int hal_codec_adc_dsf_set_speed(enum ADC_DSF_ID_T id, int speed)
{
    uint32_t mask;
    uint32_t val;
    uint32_t div = 1;
    int max_speed = 0;
    uint32_t i;

    adc_dsf_clk_speed[id] = speed;
    for (i = 0; i < ARRAY_SIZE(adc_dsf_clk_speed); i++) {
        if (max_speed < adc_dsf_clk_speed[i]) {
            max_speed = adc_dsf_clk_speed[i];
        }
    }
    if (max_speed != speed) {
        speed = max_speed;
    }

    mask = CODEC_SEL_OSC_ADC_DSF
        | CODEC_SEL_OSCX2_ADC_DSF
        | CODEC_SEL_OSC_2_ADC_DSF
        | CODEC_CFG_DIV_CODEC_ADC_DSF_MASK;
    if (speed <= 12000000) {
        val = CODEC_SEL_OSC_ADC_DSF | CODEC_SEL_OSCX2_ADC_DSF | CODEC_SEL_OSC_2_ADC_DSF;
    } else if (speed <= 24000000) {
        val = CODEC_SEL_OSC_ADC_DSF | CODEC_SEL_OSCX2_ADC_DSF;
    } else if (speed <= 48000000) {
        val = CODEC_SEL_OSCX2_ADC_DSF;
    } else {
        // dsf freq = pll / (2 + div);
        div = 1;
        val = CODEC_CFG_DIV_CODEC_ADC_DSF(div);
    }
    codec->REG_070 = (codec->REG_070 & ~mask) | val;
    return 0;
}

static uint32_t hal_codec_adc_dsf_get_speed(enum ADC_DSF_ID_T id)
{
    uint32_t speed = 0;

#ifdef __AUDIO_RESAMPLE__
    speed = 48*1000000;
#else
    speed = 96*1000000;
#endif

    return speed;
}

#endif

#ifdef CODEC_ADC1_DSF
static int hal_codec_set_dsf_adc_hbf_bypass_cnt(uint32_t cnt)
{
    uint32_t bypass_mask = CODEC_CODEC_ADC_HBF3_BYPASS_FREE
        | CODEC_CODEC_ADC_HBF2_BYPASS_FREE
        | CODEC_CODEC_ADC_HBF1_BYPASS_FREE;

    uint32_t bypass = 0;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_ADC_HBF3_BYPASS_FREE;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_ADC_HBF3_BYPASS_FREE | CODEC_CODEC_ADC_HBF2_BYPASS_FREE;
    } else {
        bypass = CODEC_CODEC_ADC_HBF3_BYPASS_FREE | CODEC_CODEC_ADC_HBF2_BYPASS_FREE | CODEC_CODEC_ADC_HBF1_BYPASS_FREE;
    }
    codec->REG_60C = (codec->REG_60C & (~bypass_mask)) | bypass;
    return 0;
}

static void hal_codec_set_dsf_adc_down(uint32_t val)
{
    uint32_t sel = 0;

    if (val == 3) {
        sel = 0;
    } else if (val == 6) {
        sel = 1;
    } else if (val == 1) {
        sel = 2;
    } else {
        ASSERT(false, "%s: Invalid adc down: %u", __FUNCTION__, val);
    }
    codec->REG_608 = SET_BITFIELD(codec->REG_608, CODEC_CODEC_DOWN_SEL_FREE, sel);
}

static int hal_codec_adc1_dsf_setup(uint32_t down_sample)
{
    uint32_t speed, i;
    uint32_t addr = CODEC_ADC1_DSF_COEF_ADDR;
    struct CODEC_ADC_DSF_PARAM_T *param = NULL;

    for (i = 0; i < ARRAY_SIZE(adc_dsf_param); i++) {
        if (adc_dsf_param[i].down_sample == down_sample) {
            param = &adc_dsf_param[i];
            break;
        }
    }
    ASSERT(param != NULL, "%s: invalid down_sample = %d", __func__, down_sample);

    adc_dsf_cfg.adc1_dsf_param = param;
    hal_codec_adc_dsf_clock_enable(ADC_DSF_ID_0, true);

    uint8_t adc_down, bypass_cnt;
    uint8_t rate_idx = adc_dsf_cfg.adc1_rate_idx;

    adc_down = adc1_dsf_sample_rate[rate_idx].adc_down;
    bypass_cnt = adc1_dsf_sample_rate[rate_idx].bypass_cnt;
    hal_codec_set_dsf_adc_down(adc_down);
    hal_codec_set_dsf_adc_hbf_bypass_cnt(bypass_cnt);

    speed = hal_codec_adc_dsf_get_speed(ADC_DSF_ID_0);

    hal_codec_adc_dsf_set_speed(ADC_DSF_ID_0, speed);

    if (codec->REG_608 & CODEC_CODEC_ADC_DSF_ENABLE) {
        codec->REG_608 &= ~CODEC_CODEC_ADC_DSF_ENABLE;
    }
    // set DSF coef
    volatile int32_t *pt = (volatile int32_t *)addr;
    volatile int32_t *ps = (volatile int32_t *)param->coef;
    for (i = 0; i < param->size / 4; i++) {
        pt[i] = ps[i];
    }
    codec->REG_608 = SET_BITFIELD(codec->REG_608, CODEC_CODEC_ADC_DSF_ORDER, param->order);
    codec->REG_608 = SET_BITFIELD(codec->REG_608, CODEC_CODEC_DOWN_SEL_FIR, param->down_sel);
    return 0;
}

static void hal_codec_adc1_dsf_enable(uint32_t ch_map)
{
    codec->REG_608 |= (CODEC_CODEC_ADC_DSF_CH_ENABLE(ch_map) | CODEC_CODEC_ADC_DSF_ENABLE);
}

static void hal_codec_adc1_dsf_disable(uint32_t ch_map)
{
    codec->REG_608 &= ~(CODEC_CODEC_ADC_DSF_CH_ENABLE(ch_map) | CODEC_CODEC_ADC_DSF_ENABLE);
}

static void hal_codec_adc1_dsf_start_interface(void)
{
    if (adc1_dsf_status) {
        hal_codec_adc1_dsf_enable(codec_adc_ch_map);
    }
}

static void hal_codec_adc1_dsf_stop_interface(void)
{
    if (adc1_dsf_status) {
        hal_codec_adc1_dsf_disable(0xFF);
        hal_codec_adc_dsf_set_speed(ADC_DSF_ID_0, 0);
        hal_codec_adc_dsf_clock_enable(ADC_DSF_ID_0, false);
        adc1_dsf_status = false;
        adc_dsf_cfg.adc1_rate_idx = 0;
    }
}

static void hal_codec_adc1_dsf_setup_param(enum AUD_SAMPRATE_T sample_rate)
{
    enum AUD_SAMPRATE_T cur_sample_rate;

    cur_sample_rate = adc1_dsf_sample_rate[adc_dsf_cfg.adc1_rate_idx].sample_rate;

    if (!adc1_dsf_status || (sample_rate != cur_sample_rate)) {
        uint32_t i;
        uint8_t dsf_ds = 0, adc_down = 0, bypass_cnt = 0;
        struct CODEC_ADC_DSF_SAMPRATE_T *dsf_sr = adc1_dsf_sample_rate;
        uint32_t num = ARRAY_SIZE(adc1_dsf_sample_rate);
        enum AUD_CHANNEL_MAP_T ch_map = codec_adc_ch_map;;

        TRACE(0, "ADC1 DSF SETUP: sample_rate=%d, ch_map=%d", sample_rate, ch_map);
        for (i = 0; i < num; i++) {
            if (dsf_sr->sample_rate == sample_rate) {
                break;
            }
            dsf_sr++;
        }
        ASSERT(i < num, "[%s]: Bad sample rate %d", __func__, sample_rate);
        adc_dsf_cfg.adc1_rate_idx = i;
        adc_down   = dsf_sr->adc_down;
        bypass_cnt = dsf_sr->bypass_cnt;
        dsf_ds     = dsf_sr->dsf_ds;
        TRACE(0, "adc_down=%d,bypass_cnt=%d,dsf_ds=%d", adc_down, bypass_cnt, dsf_ds);

        hal_codec_set_adc_down(ch_map, adc_down);
        hal_codec_set_adc_hbf_bypass_cnt(ch_map, bypass_cnt);
        hal_codec_adc1_dsf_setup(dsf_ds);
        adc1_dsf_status = true;
    }
}
#endif /* CODEC_ADC1_DSF */

#ifdef CODEC_ADC2_DSF
static int hal_codec_adc2_dsf_setup(uint32_t down_sample)
{
    uint32_t speed, i;
    uint32_t addr = CODEC_ADC2_DSF_COEF_ADDR;
    struct CODEC_ADC_DSF_PARAM_T *param = NULL;

    for (i = 0; i < ARRAY_SIZE(adc_dsf_param); i++) {
        if (adc_dsf_param[i].down_sample == down_sample) {
            param = &adc_dsf_param[i];
            break;
        }
    }
    ASSERT(param != NULL, "%s: invalid down_sample = %d", __func__, down_sample);

    hal_codec_adc_dsf_clock_enable(ADC_DSF_ID_1, true);
    adc_dsf_cfg.adc2_dsf_param = param;

    speed = hal_codec_adc_dsf_get_speed(ADC_DSF_ID_0);

    hal_codec_adc_dsf_set_speed(ADC_DSF_ID_1, speed);

    if (codec->REG_7B0 & CODEC_CODEC_ADC_SND_DSF_ENABLE) {
        codec->REG_7B0 &= ~CODEC_CODEC_ADC_SND_DSF_ENABLE;
    }
    // set DSF coef
    volatile int32_t *pt = (volatile int32_t *)addr;
    volatile int32_t *ps = (volatile int32_t *)param->coef;
    for (i = 0; i < param->size / 4; i++) {
        pt[i] = ps[i];
    }
    codec->REG_7B0 = SET_BITFIELD(codec->REG_7B0, CODEC_CODEC_ADC_SND_DSF_ORDER, param->order);
    codec->REG_7B0 = SET_BITFIELD(codec->REG_7B0, CODEC_CODEC_DOWN_SEL_FIR_SND, param->down_sel);
    return 0;
}

static void hal_codec_adc2_dsf_enable(uint32_t ch_map)
{
    codec->REG_7B0 |= (CODEC_CODEC_ADC_SND_DSF_CH_ENABLE(ch_map) | CODEC_CODEC_ADC_SND_DSF_ENABLE);
}

static void hal_codec_adc2_dsf_disable(uint32_t ch_map)
{
    codec->REG_7B0 &= ~(CODEC_CODEC_ADC_SND_DSF_CH_ENABLE(ch_map) | CODEC_CODEC_ADC_SND_DSF_ENABLE);
}

static void hal_codec_adc2_dsf_start_interface(void)
{
    if (adc2_dsf_status) {
        hal_codec_adc2_dsf_enable(codec_adc2_ch_map);
    }
}

static void hal_codec_adc2_dsf_stop_interface(void)
{
    if (adc2_dsf_status) {
        hal_codec_adc2_dsf_disable(0xFF);
        hal_codec_adc_dsf_set_speed(ADC_DSF_ID_1, 0);
        hal_codec_adc_dsf_clock_enable(ADC_DSF_ID_1, false);
        adc2_dsf_status = false;
        adc_dsf_cfg.adc2_rate_idx = 0;
    }
}

static void hal_codec_adc2_dsf_setup_param(enum AUD_SAMPRATE_T sample_rate)
{
    enum AUD_SAMPRATE_T cur_sample_rate = adc2_dsf_sample_rate[adc_dsf_cfg.adc2_rate_idx].sample_rate;

    if (!adc2_dsf_status || (sample_rate != cur_sample_rate)) {
        uint32_t i = 0;
        struct CODEC_ADC_DSF_SAMPRATE_T *dsf_sr = adc2_dsf_sample_rate;
        uint32_t num = ARRAY_SIZE(adc2_dsf_sample_rate);
        uint8_t rate_sel = 0, dsf_ds = 0;
        uint8_t adc_down = 0, bypass_cnt = 0;
        enum AUD_CHANNEL_MAP_T ch_map = codec_adc2_ch_map;

        TRACE(0, "ADC2 DSF SETUP: sample_rate=%d, ch_map=%d", sample_rate, ch_map);
        for (i = 0; i < num; i++) {
            if (dsf_sr->sample_rate == sample_rate) {
                break;
            }
            dsf_sr++;
        }
        ASSERT(i < num, "[%s]: Bad sample rate %d", __func__, sample_rate);
        adc_dsf_cfg.adc2_rate_idx = i;
        adc_down   = dsf_sr->adc_down;
        bypass_cnt = dsf_sr->bypass_cnt;
        dsf_ds     = dsf_sr->dsf_ds;
        rate_sel   = dsf_sr->rate_sel;
        TRACE(0, "adc_down=%d,bypass_cnt=%d,dsf_ds=%d,rate_sel=%d",
            adc_down, bypass_cnt, dsf_ds, rate_sel);

        hal_codec_set_adc_down(ch_map, adc_down);
        hal_codec_set_adc_hbf_bypass_cnt(ch_map, bypass_cnt);
        codec->REG_780 = SET_BITFIELD(codec->REG_780, CODEC_CODEC_ADC_RATE_SEL_SND, rate_sel);
        hal_codec_adc2_dsf_setup(dsf_ds);
#ifdef CODEC_ADC2_DUMP_DATA
        hal_codec_adc2_setup_data_path(1);
#endif
        adc2_dsf_status = true;
    }
}

#ifdef CODEC_ADC2_DUMP_DATA
static int hal_codec_adc2_setup_data_path(int path)
{
#define SEL_PATH_LIST_NUM 2
    uint32_t adc_rate_sel = 4;
    uint32_t val = 0;
    uint8_t *ptr_path;
    uint8_t sel_path[SEL_PATH_LIST_NUM][6] = {
        {0,0,0,0,0,0},
        {4,3,3,0,0,5},
    };

    ASSERT(path < SEL_PATH_LIST_NUM, "%s: invalid path %d", __func__, path);
    ptr_path = (uint8_t *)&sel_path[path];

    val = codec->REG_780;
    val &= ~(CODEC_CODEC_ADC_RATE_SEL_SND_MASK
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH0_MASK
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH1_MASK
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH2_MASK
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH3_MASK);
    val |= (CODEC_CODEC_ADC_RATE_SEL_SND(adc_rate_sel)
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH0(ptr_path[0])
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH1(ptr_path[1])
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH2(ptr_path[2])
            |CODEC_CODEC_ADC_DATA_SEL_SND_CH3(ptr_path[3]));
    codec->REG_780 = val;
    return 0;
}
#endif /* CODEC_ADC2_DUMP_DATA */
#endif /* CODEC_ADC2_DSF */

static void hal_codec_mic_remap_to_adc_chan(bool ana_mic, uint8_t adc_chan, uint8_t mic_chan)
{
    if (ana_mic) {
        volatile uint32_t *reg = (volatile uint32_t *)&codec->REG_084 + adc_chan;
        if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
            uint32_t pos = 3 * adc_chan;
            // ci2s adc ch sel
            codec->REG_7F0 = (codec->REG_7F0 & ~(CODEC_CI2S_ADC_IN_SEL_CH0_MASK << pos))
                                | (CODEC_CI2S_ADC_IN_SEL_CH0(mic_chan) << pos);
        }
        *reg = SET_BITFIELD(*reg, CODEC_CODEC_ADC_IN_SEL_CH0, mic_chan);
        codec->REG_0C4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << adc_chan);
        TRACE(2, "adc chan[%d] = amic[%d]", adc_chan, mic_chan);
    } else {
        uint32_t pos = 3 * adc_chan;

        codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << pos))
                            | (CODEC_CODEC_PDM_MUX_CH0(mic_chan) << pos);
        codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << adc_chan;
        TRACE(2, "adc chan[%d] = dmic[%d]", adc_chan, mic_chan);
    }
}

#ifdef ANC_APP
static void hal_codec_anc_chan_config(
    enum AUD_CHANNEL_MAP_T *p_mic_map,
    enum AUD_CHANNEL_MAP_T *p_reserv_map,
    enum AUD_CHANNEL_MAP_T *p_adc_chan_map)
{
    enum AUD_CHANNEL_MAP_T mic_map      = *p_mic_map;      //mic chan setting value
    enum AUD_CHANNEL_MAP_T reserv_map   = *p_reserv_map;   //adc chan reserved value
    enum AUD_CHANNEL_MAP_T adc_chan_map = *p_adc_chan_map; //adc chan config value
    uint8_t ch_idx = 0;

/* check and config FF mic for ANC */
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
#ifdef ANC_PROD_TEST
    if ((ANC_FF_MIC_CH_L & ~NORMAL_MIC_MAP) || (ANC_FF_MIC_CH_L & (ANC_FF_MIC_CH_L - 1))) {
        ASSERT(false, "Invalid ANC_FF_MIC_CH_L: 0x%04X", ANC_FF_MIC_CH_L);
    }
    if ((ANC_FF_MIC_CH_R & ~NORMAL_MIC_MAP) || (ANC_FF_MIC_CH_R & (ANC_FF_MIC_CH_R - 1))) {
        ASSERT(false, "Invalid ANC_FF_MIC_CH_R: 0x%04X", ANC_FF_MIC_CH_R);
    }
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
    if ((ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_L)
        || (ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_R)
        || (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_L)
        || (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_R)) {
        ASSERT(false, "Conflicted FF MIC (0x%04X/0x%04X) and FB MIC (0x%04X/0x%04X)",
            ANC_FF_MIC_CH_L, ANC_FF_MIC_CH_R, ANC_FB_MIC_CH_L, ANC_FB_MIC_CH_R);
    }
#endif
#else // !ANC_PROD_TEST
#if (ANC_FF_MIC_CH_L & ~NORMAL_MIC_MAP) || (ANC_FF_MIC_CH_L & (ANC_FF_MIC_CH_L - 1))
#error "Invalid ANC_FF_MIC_CH_L"
#endif

#if (ANC_FF_MIC_CH_R & ~NORMAL_MIC_MAP) || (ANC_FF_MIC_CH_R & (ANC_FF_MIC_CH_R - 1))
#error "Invalid ANC_FF_MIC_CH_R"
#endif

#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
#if (ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_L) \
    || (ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_R) \
    || (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_L) \
    || (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_R)
#error "Conflicted ANC_FF_MIC_CH_L, ANC_FB_MIC_CH_L, ANC_FF_MIC_CH_R and ANC_FB_MIC_CH_R"
#endif
#endif

#endif // !ANC_PROD_TEST
    /* ANC FF L MIC configuration */
    if (mic_map & ANC_FF_MIC_CH_L) {
        adc_chan_map |= AUD_CHANNEL_MAP_CH0;
        mic_map &= ~ANC_FF_MIC_CH_L;
        ch_idx = get_msb_pos(ANC_FF_MIC_CH_L);
        if (ANC_FF_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
            codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH0, ch_idx);
            codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0;
        } else {
            if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH0, ch_idx);
            }
            codec->REG_084 = SET_BITFIELD(codec->REG_084, CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
            codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH0;
        }
    } else if (ANC_FF_MIC_CH_L & AUD_CHANNEL_MAP_ALL) {
        reserv_map |= AUD_CHANNEL_MAP_CH0;
    }

    /* ANC FF R MIC configuration */
    if (mic_map & ANC_FF_MIC_CH_R) {
        adc_chan_map |= AUD_CHANNEL_MAP_CH1;
        mic_map &= ~ANC_FF_MIC_CH_R;
        ch_idx = get_msb_pos(ANC_FF_MIC_CH_R);
        if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
            codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH1, ch_idx);
            codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH1;
        } else {
            if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH1, ch_idx);
            }
            codec->REG_088 = SET_BITFIELD(codec->REG_088, CODEC_CODEC_ADC_IN_SEL_CH1, ch_idx);
            codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH1;
        }
    } else if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_ALL) {
        reserv_map |= AUD_CHANNEL_MAP_CH1;
    }

#if defined(SIDETONE_ENABLE) && !defined(SIDETONE_DEDICATED_ADC_CHAN)
    if (CFG_HW_AUD_SIDETONE_MIC_DEV == ANC_FF_MIC_CH_L) {
        hal_codec_set_sidetone_adc_chan(AUD_CHANNEL_MAP_CH0);
    }
#endif
#endif /* defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R) */

/* check and config FB mic for ANC */
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_CH_R)
#ifdef ANC_PROD_TEST
    if ((ANC_FB_MIC_CH_L & ~NORMAL_MIC_MAP) || (ANC_FB_MIC_CH_L & (ANC_FB_MIC_CH_L - 1))) {
        ASSERT(false, "Invalid ANC_FB_MIC_CH_L: 0x%04X", ANC_FB_MIC_CH_L);
    }
    if ((ANC_FB_MIC_CH_R & ~NORMAL_MIC_MAP) || (ANC_FB_MIC_CH_R & (ANC_FB_MIC_CH_R - 1))) {
        ASSERT(false, "Invalid ANC_FB_MIC_CH_R: 0x%04X", ANC_FB_MIC_CH_R);
    }
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
    if (ANC_TT_MIC_CH_L & ANC_FB_MIC_CH_L) {
        ASSERT(false, "Conflicted TT L MIC (0x%04X) and FB L MIC (0x%04X)",
            ANC_TT_MIC_CH_L, ANC_FB_MIC_CH_L);
    }
    if (ANC_TT_MIC_CH_R & ANC_FB_MIC_CH_R) {
        ASSERT(false, "Conflicted TT R MIC (0x%04X) and FB R MIC (0x%04X)",
            ANC_TT_MIC_CH_R, ANC_FB_MIC_CH_R);
    }
#endif
#else // !ANC_PROD_TEST

#if (ANC_FB_MIC_CH_L & ~NORMAL_MIC_MAP) || (ANC_FB_MIC_CH_L & (ANC_FB_MIC_CH_L - 1))
#error "Invalid ANC_FB_MIC_CH_L"
#endif

#if (ANC_FB_MIC_CH_R & ~NORMAL_MIC_MAP) || (ANC_FB_MIC_CH_R & (ANC_FB_MIC_CH_R - 1))
#error "Invalid ANC_FB_MIC_CH_R"
#endif

#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
#if (ANC_TT_MIC_CH_L & ANC_FB_MIC_CH_L) || (ANC_TT_MIC_CH_R & ANC_FB_MIC_CH_R)
#error "Conflicted ANC_TT_MIC_CH_L and ANC_FB_MIC_CH_L, or ANC_TT_MIC_CH_R and ANC_FB_MIC_CH_R"
#endif
#endif

#endif // !ANC_PROD_TEST
    /* ANC FB L MIC configuration */
    if (mic_map & ANC_FB_MIC_CH_L) {
        adc_chan_map |= AUD_CHANNEL_MAP_CH2;
        mic_map &= ~ANC_FB_MIC_CH_L;
        ch_idx = get_msb_pos(ANC_FB_MIC_CH_L);
        if (ANC_FB_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
            codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH2, ch_idx);
            codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH2;
        } else {
            if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH2, ch_idx);
            }
            codec->REG_08C = SET_BITFIELD(codec->REG_08C, CODEC_CODEC_ADC_IN_SEL_CH2, ch_idx);
            codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH2;
        }
    } else if (ANC_FB_MIC_CH_L & AUD_CHANNEL_MAP_ALL) {
        reserv_map |= AUD_CHANNEL_MAP_CH2;
    }
    /* ANC FB R MIC configuration */
    if (mic_map & ANC_FB_MIC_CH_R) {
        adc_chan_map |= AUD_CHANNEL_MAP_CH3;
        mic_map &= ~ANC_FB_MIC_CH_R;
        ch_idx = get_msb_pos(ANC_FB_MIC_CH_R);
        if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
            codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH3, ch_idx);
            codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH3;
        } else {
            if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH3, ch_idx);
            }
            codec->REG_090 = SET_BITFIELD(codec->REG_090, CODEC_CODEC_ADC_IN_SEL_CH3, ch_idx);
            codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH3;
        }
    } else if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_ALL) {
        reserv_map |= AUD_CHANNEL_MAP_CH3;
    }

#if defined(SIDETONE_ENABLE) && !defined(SIDETONE_DEDICATED_ADC_CHAN)
    if (CFG_HW_AUD_SIDETONE_MIC_DEV == ANC_FB_MIC_CH_L) {
        hal_codec_set_sidetone_adc_chan(AUD_CHANNEL_MAP_CH2);
    }
#endif
#endif
    reserv_map |= adc_chan_map;

/* ANC TT MIC checking */
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
#ifdef ANC_PROD_TEST
    if ((ANC_TT_MIC_CH_L & ~NORMAL_MIC_MAP) || (ANC_TT_MIC_CH_L & (ANC_TT_MIC_CH_L - 1))) {
        ASSERT(false, "Invalid ANC_TT_MIC_CH_L: 0x%04X", ANC_TT_MIC_CH_L);
    }
    if ((ANC_TT_MIC_CH_R & ~NORMAL_MIC_MAP) || (ANC_TT_MIC_CH_R & (ANC_TT_MIC_CH_R - 1))) {
        ASSERT(false, "Invalid ANC_TT_MIC_CH_R: 0x%04X", ANC_TT_MIC_CH_R);
    }
#else // !ANC_PROD_TEST
#if (ANC_TT_MIC_CH_L & ~NORMAL_MIC_MAP) || (ANC_TT_MIC_CH_L & (ANC_TT_MIC_CH_L - 1))
#error "Invalid ANC_TT_MIC_CH_L"
#endif
#if (ANC_TT_MIC_CH_R & ~NORMAL_MIC_MAP) || (ANC_TT_MIC_CH_R & (ANC_TT_MIC_CH_R - 1))
#error "Invalid ANC_TT_MIC_CH_R"
#endif
#endif // !ANC_PROD_TEST

    /* ANC TT L MIC configuration */
    if (ANC_TT_MIC_CH_L && (FORCE_TT_ADC_ALLOC || anc_tt_adc_ch_l == 0)) {
        if (ANC_TT_MIC_CH_L == ANC_FF_MIC_CH_L) {
            anc_tt_adc_ch_l = AUD_CHANNEL_MAP_CH0;
        } else {
            anc_tt_adc_ch_l = NORMAL_ADC_MAP & ~reserv_map;
            ASSERT(anc_tt_adc_ch_l, "%s: Cannot alloc TT CH L adc: reserv_map=0x%X", __func__, reserv_map);
            anc_tt_adc_ch_l = (1 << get_lsb_pos(anc_tt_adc_ch_l));
            reserv_map |= anc_tt_adc_ch_l;
        }
    }
    /* ANC TT R MIC configuration */
    if (ANC_TT_MIC_CH_R && (FORCE_TT_ADC_ALLOC || anc_tt_adc_ch_r == 0)) {
        if (ANC_TT_MIC_CH_R == ANC_FF_MIC_CH_R) {
            anc_tt_adc_ch_r = AUD_CHANNEL_MAP_CH1;
        } else {
            anc_tt_adc_ch_r = NORMAL_ADC_MAP & ~reserv_map;
            ASSERT(anc_tt_adc_ch_r, "%s: Cannot alloc TT CH R adc: reserv_map=0x%X", __func__, reserv_map);
            anc_tt_adc_ch_r = (1 << get_lsb_pos(anc_tt_adc_ch_r));
            reserv_map |= anc_tt_adc_ch_r;
        }
    }

    uint32_t i = 0;
    if (mic_map & ANC_TT_MIC_CH_L) {
        adc_chan_map |= anc_tt_adc_ch_l;
        mic_map &= ~ANC_TT_MIC_CH_L;
        i = get_lsb_pos(anc_tt_adc_ch_l);
        ch_idx = get_lsb_pos(ANC_TT_MIC_CH_L);
        if (ANC_TT_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
            codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) |
                (CODEC_CODEC_PDM_MUX_CH0(ch_idx) << (3 * i));
            codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << i;
        } else {
            if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                uint32_t pos = 3 * i;
                // ci2s adc ch sel
                codec->REG_7F0 = (codec->REG_7F0 & ~(CODEC_CI2S_ADC_IN_SEL_CH0_MASK << pos))
                                    | (CODEC_CI2S_ADC_IN_SEL_CH0(ch_idx) << pos);
            }
            *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
            codec->REG_0C4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << i);
        }
    } else if (ANC_TT_MIC_CH_L) {
        reserv_map |= anc_tt_adc_ch_l;
    }
    if (mic_map & ANC_TT_MIC_CH_R) {
        adc_chan_map |= anc_tt_adc_ch_r;
        mic_map &= ~ANC_TT_MIC_CH_R;
        i = get_lsb_pos(anc_tt_adc_ch_r);
        ch_idx = get_lsb_pos(ANC_TT_MIC_CH_R);
        if (ANC_TT_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
            codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) |
                (CODEC_CODEC_PDM_MUX_CH0(ch_idx) << (3 * i));
            codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << i;
        } else {
            if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                uint32_t pos = 3 * i;
                // ci2s adc ch sel
                codec->REG_7F0 = (codec->REG_7F0 & ~(CODEC_CI2S_ADC_IN_SEL_CH0_MASK << pos))
                                    | (CODEC_CI2S_ADC_IN_SEL_CH0(ch_idx) << pos);
            }
            *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
            codec->REG_0C4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << i);
        }
    } else if (ANC_TT_MIC_CH_R) {
        reserv_map |= anc_tt_adc_ch_r;
    }
#endif /* defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R) */

    *p_mic_map      = mic_map;
    *p_reserv_map   = reserv_map;
    *p_adc_chan_map = adc_chan_map;
}
#endif /* ANC_APP */

static void hal_codec_ec_chan_config(
    enum AUD_CHANNEL_MAP_T *p_mic_map,
    enum AUD_CHANNEL_MAP_T *p_reserv_map,
    enum AUD_CHANNEL_MAP_T *p_adc_chan_map)
{
    enum AUD_CHANNEL_MAP_T mic_map      = *p_mic_map;      //mic chan setting value
    enum AUD_CHANNEL_MAP_T reserv_map   = *p_reserv_map;   //adc chan reserved value
    enum AUD_CHANNEL_MAP_T adc_chan_map = *p_adc_chan_map; //adc chan config value

    // reserve ADC chan for EC0, EC1
    if (mic_map & AUD_CHANNEL_MAP_ECMIC_CH0) {
        adc_chan_map |= AUD_CHANNEL_MAP_CH4;
        mic_map &= ~AUD_CHANNEL_MAP_ECMIC_CH0;
    }
    if (mic_map & AUD_CHANNEL_MAP_ECMIC_CH1) {
        adc_chan_map |= AUD_CHANNEL_MAP_CH5;
        mic_map &= ~AUD_CHANNEL_MAP_ECMIC_CH1;
    }

    *p_mic_map      = mic_map;
    *p_reserv_map   = reserv_map;
    *p_adc_chan_map = adc_chan_map;
}

#ifdef SIDETONE_ENABLE
static void hal_codec_side_tone_chan_config(
    enum AUD_CHANNEL_MAP_T *p_mic_map,
    enum AUD_CHANNEL_MAP_T *p_reserv_map,
    enum AUD_CHANNEL_MAP_T *p_adc_chan_map)
{
#if defined(SIDETONE_DEDICATED_ADC_CHAN) || defined(SIDETONE_RESERVED_ADC_CHAN)
    enum AUD_CHANNEL_MAP_T mic_map      = *p_mic_map;      //mic chan setting value
    enum AUD_CHANNEL_MAP_T reserv_map   = *p_reserv_map;   //adc chan reserved value
    enum AUD_CHANNEL_MAP_T adc_chan_map = *p_adc_chan_map; //adc chan config value
    uint8_t ch_idx;
    uint32_t i;

    if (mic_map & CFG_HW_AUD_SIDETONE_MIC_DEV) {
        enum AUD_CHANNEL_MAP_T st_map;

        // Alloc sidetone adc chan
        st_map = NORMAL_ADC_MAP & ~reserv_map;
        ASSERT(st_map, "%s: Cannot alloc dedicated sidetone adc: reserv_map=0x%X", __func__, reserv_map);

        i = get_lsb_pos(st_map);
        st_map = (1 << i);

        // Associate mic and sidetone adc
        hal_codec_set_sidetone_adc_chan(st_map);
        ch_idx = get_lsb_pos(CFG_HW_AUD_SIDETONE_MIC_DEV);
        if ((1 << ch_idx) & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
            codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) |
                (CODEC_CODEC_PDM_MUX_CH0(ch_idx) << (3 * i));
            codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << i;
        } else {
            *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
            codec->REG_0C4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << i);
        }
#ifdef SIDETONE_DEDICATED_ADC_CHAN
        sidetone_adc_ch_map = st_map;
#else
        mic_map &= ~(1 << ch_idx);
        adc_chan_map |= st_map;
#endif
        // Mark sidetone adc as used
        reserv_map |= st_map;
    }
    *p_mic_map      = mic_map;
    *p_reserv_map   = reserv_map;
    *p_adc_chan_map = adc_chan_map;
#endif
}
#endif /*SIDETONE_ENABLE */

static void hal_codec_adc_chan_map_unify(
    enum AUD_CHANNEL_MAP_T *p_mic_map, enum AUD_CHANNEL_MAP_T *p_reserv_map,
    enum AUD_CHANNEL_MAP_T mic_chan_map, enum AUD_CHANNEL_MAP_T adc_chan_map,
    enum AUD_CHANNEL_MAP_T local_chan_map)
{
    enum AUD_CHANNEL_MAP_T mic_map    = *p_mic_map;      //mic chan setting value
    enum AUD_CHANNEL_MAP_T reserv_map = *p_reserv_map;   //adc chan reserved value

    if (adc_chan_map & VALID_ADC_MAP) {
        reserv_map |= adc_chan_map;
        mic_map &= ~mic_chan_map;
    }
    reserv_map |= local_chan_map;

    *p_mic_map    = mic_map;
    *p_reserv_map = reserv_map;
}

static void hal_codec_adc_chan_config(
    enum AUD_CHANNEL_MAP_T *p_mic_map,
    enum AUD_CHANNEL_MAP_T *p_reserv_map,
    enum AUD_CHANNEL_MAP_T *p_adc_chan_map)
{
    enum AUD_CHANNEL_MAP_T mic_map      = *p_mic_map;      //mic chan setting value
    enum AUD_CHANNEL_MAP_T reserv_map   = *p_reserv_map;   //adc chan reserved value
    enum AUD_CHANNEL_MAP_T adc_ch_map   = *p_adc_chan_map; //adc chan config value
    uint32_t i = 0;
    uint8_t ch_idx;

    TRACE(1, "mic_map=%x,reserv_map=%x,adc_ch_map=%x",mic_map, reserv_map, adc_ch_map);

    while (mic_map && i < NORMAL_ADC_CH_NUM) {
        ASSERT(i < MAX_ANA_MIC_CH_NUM || (mic_map & AUD_CHANNEL_MAP_DIGMIC_ALL),
            "%s: Not enough ana cap chan: mic_map=0x%X adc_map=0x%X reserv_map=0x%X",
            __func__, mic_map, adc_ch_map, reserv_map);
        ch_idx = get_lsb_pos(mic_map);
        while ((reserv_map & (AUD_CHANNEL_MAP_CH0 << i)) && i < NORMAL_ADC_CH_NUM) {
            i++;
        }
        if (i < NORMAL_ADC_CH_NUM) {
            adc_ch_map |= (AUD_CHANNEL_MAP_CH0 << i);
            reserv_map |= adc_ch_map;
            mic_map &= ~(1 << ch_idx);
            if ((1 << ch_idx) & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
                hal_codec_mic_remap_to_adc_chan(false, i, ch_idx);
            } else {
                hal_codec_mic_remap_to_adc_chan(true, i, ch_idx);
#if defined(CODEC_ADC_DC_FILTER_FACTOR)
                hal_codec_enable_adc_dc_filter((1 << i), true);
#endif
            }
            i++;
        }
    }
    ASSERT(mic_map == 0, "%s: Bad cap chan map: 0x%X reserv_map=0x%X", __func__, mic_map, reserv_map);
    *p_mic_map      = mic_map;
    *p_reserv_map   = reserv_map;
    *p_adc_chan_map = adc_ch_map;
}

int hal_codec_adc2_setup_stream(enum HAL_CODEC_ID_T id, const struct HAL_CODEC_CONFIG_T *cfg)
{
    enum AUD_CHANNEL_MAP_T mic_map;
    enum AUD_CHANNEL_MAP_T reserv_map;
    uint8_t cnt;
    uint8_t ch_idx;
    uint32_t cfg_set_mask;
    uint32_t cfg_clr_mask;
    bool alloc_adc = false;
    uint32_t i;
    enum AUD_SAMPRATE_T sample_rate;
    int rate_idx;
    uint32_t ana_dig_div;

    mic_map = AUD_CHANNEL_MAP_NULL;
    if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
        alloc_adc = true;
    }
#if defined(ANC_APP) && (defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R))
    if (ANC_TT_MIC_CH_L && (FORCE_TT_ADC_ALLOC || anc_tt_adc_ch_l == 0)) {
        alloc_adc = true;
    }
    if (ANC_TT_MIC_CH_R && (FORCE_TT_ADC_ALLOC || anc_tt_adc_ch_r == 0)) {
        alloc_adc = true;
    }
#endif

    if (alloc_adc) {
        mic_map = cfg->channel_map;
        codec_mic2_ch_map = mic_map;
        codec_adc2_ch_map = AUD_CHANNEL_MAP_NULL;
        reserv_map = AUD_CHANNEL_MAP_NULL;

        // reserve ADC chan for DSD
#ifdef CODEC_DSD
        reserv_map |= AUD_CHANNEL_MAP_CH2;
#endif

#ifdef ANC_APP
        hal_codec_anc_chan_config(&mic_map, &reserv_map, &codec_adc2_ch_map);
#endif
        hal_codec_ec_chan_config(&mic_map, &reserv_map, &codec_adc2_ch_map);
#ifdef SIDETONE_ENABLE
        hal_codec_side_tone_chan_config(&mic_map, &reserv_map, &codec_adc2_ch_map);
#endif /* SIDETONE_ENABLED */

        hal_codec_adc_chan_map_unify(&mic_map, &reserv_map, codec_mic_ch_map, codec_adc_ch_map, codec_adc2_ch_map);

        hal_codec_adc_chan_config(&mic_map, &reserv_map, &codec_adc2_ch_map);
    }

    // sample bits config
    if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
        cfg_set_mask = 0;
        cfg_clr_mask = CODEC_MODE_16BIT_ADC_SND | CODEC_MODE_24BIT_ADC_SND | CODEC_MODE_32BIT_ADC_SND;
        if (cfg->bits == AUD_BITS_16) {
            cfg_set_mask |= CODEC_MODE_16BIT_ADC_SND;
        } else if (cfg->bits == AUD_BITS_24) {
            cfg_set_mask |= CODEC_MODE_24BIT_ADC_SND;
        } else if (cfg->bits == AUD_BITS_32) {
            cfg_set_mask |= CODEC_MODE_32BIT_ADC_SND;
        } else {
            ASSERT(false, "%s: Bad cap bits: %d", __func__, cfg->bits);
        }
        codec->REG_744 = (codec->REG_744 & ~cfg_clr_mask) | cfg_set_mask;
    }

    cnt = 0;
    enum AUD_CHANNEL_MAP_T full_map = codec_adc2_ch_map | codec_adc_ch_map;
    for (i = 0; i < MAX_ADC2_CH_NUM; i++) {
        if (full_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            cnt++;
        }
    }

    ASSERT(cnt >= cfg->channel_num, "%s: %d, bad cap stream chan cfg: mic2_map=0x%X adc2_map=0x%X reserv_map=%X, num=%d, cnt=%d", __func__, __LINE__,
        codec_mic2_ch_map, codec_adc2_ch_map, reserv_map, cfg->channel_num, cnt);

    // sample rate config
    if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
        sample_rate = cfg->sample_rate;

        for(i = 0; i < ARRAY_SIZE(codec_adc_sample_rate); i++) {
            if(codec_adc_sample_rate[i].sample_rate == sample_rate) {
                break;
            }
        }
        ASSERT(i < ARRAY_SIZE(codec_adc_sample_rate), "%s: Invalid capture sample rate: %d", __func__, sample_rate);
        rate_idx = i;
        ana_dig_div = codec_adc_sample_rate[rate_idx].codec_div / codec_adc_sample_rate[rate_idx].cmu_div;
        ASSERT(ana_dig_div * codec_adc_sample_rate[rate_idx].cmu_div == codec_adc_sample_rate[rate_idx].codec_div,
            "%s: Invalid catpure div for rate %u: codec_div=%u cmu_div=%u", __func__, sample_rate,
            codec_adc_sample_rate[rate_idx].codec_div, codec_adc_sample_rate[rate_idx].cmu_div);

        TRACE(2,"[%s] capture sample_rate=%d", __func__, sample_rate);

        codec_adc2_rate_idx = rate_idx;

#ifdef __AUDIO_RESAMPLE__
        if (hal_cmu_get_audio_resample_status() && codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL) {
            if ((codec->REG_7C0 & CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE) == 0 ||
                    adc2_resample_rate_idx != rate_idx) {
                adc2_resample_rate_idx = rate_idx;
                codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
                hal_codec_reg_update_delay();
                codec->REG_7D0 = resample_phase_float_to_value(get_capture2_resample_phase());
                hal_codec_reg_update_delay();
                codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
            }

            codec->REG_7C0 = SET_BITFIELD(codec->REG_7C0, CODEC_CODEC_RESAMPLE_ADC_SND_CH_CNT, cfg->channel_num - 1);
        }
#endif
        uint32_t val;
        uint8_t adc_chan;
        uint32_t ec_mic;

        val = 0;
        i = 0;
        mic_map = codec_mic2_ch_map;
        TRACE(1, "codec_mic_ch_map[%x,%x]", codec_mic_ch_map, codec_mic2_ch_map);
        while (mic_map && i < MAX_ADC2_CH_NUM) {
            ch_idx = get_lsb_pos(mic_map);
            mic_map &= ~(1 << ch_idx);
            ec_mic = (1 << ch_idx) & EC_MIC_MAP;
            if (ec_mic == AUD_CHANNEL_MAP_ECMIC_CH0) {
                adc_chan = 6;
            } else if (ec_mic == AUD_CHANNEL_MAP_ECMIC_CH1) {
                adc_chan = 7;
            } else {
                adc_chan = hal_codec_get_adc_chan((enum AUD_CHANNEL_MAP_T)(1 << ch_idx),
                    codec_mic2_ch_map, codec_adc2_ch_map, NORMAL_ADC_CH_NUM);

                // if adc_chan is invalid, try to find ADC1's chan map
                if (adc_chan >= NORMAL_ADC_CH_NUM) {
                    adc_chan = hal_codec_get_adc_chan((enum AUD_CHANNEL_MAP_T)(1 << ch_idx),
                        codec_mic_ch_map, codec_adc_ch_map, NORMAL_ADC_CH_NUM);

                    ASSERT(adc_chan < NORMAL_ADC_CH_NUM,
                        "%s: Bad remap1 mic ch=%u mic_map=0x%X adc_map=0x%X",
                    __func__, ch_idx, codec_mic_ch_map, codec_adc_ch_map);
                }
            }
            TRACE(1, "remap ADC chan[%d]->[%d]", adc_chan, i);
            codec_adc2_ch_map |= (1 << adc_chan); //set adc2 chan map
            val |= CODEC_CODEC_RESAMPLE_ADC_SND_CH0_SEL(adc_chan) << (4 * i);
            i++;
        }
        ASSERT(mic_map == 0, "%s: Bad cap remap map: 0x%X", __func__, mic_map);
        codec->REG_7C4 = val;
        codec->REG_7C0 |= CODEC_CODEC_ADC_SND_REMAP_ENABLE;

        // enable pll and set adc div
#ifdef __AUDIO_RESAMPLE__
        if (!hal_cmu_get_audio_resample_status())
#endif
        {
#ifdef __AUDIO_RESAMPLE__
            ASSERT(codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL,
                "%s: capture sample rate %u is for resample only", __func__, sample_rate);
#endif
            analog_aud_freq_pll_config(codec_adc_sample_rate[rate_idx].codec_freq, codec_adc_sample_rate[rate_idx].codec_div);
            hal_codec_adc_set_div(codec_adc_sample_rate[rate_idx].cmu_div * CODEC_FREQ_EXTRA_DIV);
            hal_codec_ci2s_set_div(codec_adc_sample_rate[rate_idx].cmu_div);
        }

#ifdef CODEC_ADC2_DSF
        hal_codec_adc2_dsf_setup_param(sample_rate);
#else
        ASSERT(sample_rate==48000, "%s: ADC2 DSF should be enabled for %d", __func__, sample_rate);
#endif /* CODEC_ADC2_DSF */
    }

    if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
        // TODO: ADC2 software gain ?
    }

    return 0;
}

static void hal_codec_adc2_flush_fifo(void)
{
    uint32_t fifo_flush = 0;

    fifo_flush = CODEC_RX_FIFO_FLUSH_SND_CH0 | CODEC_RX_FIFO_FLUSH_SND_CH1
        | CODEC_RX_FIFO_FLUSH_SND_CH2 | CODEC_RX_FIFO_FLUSH_SND_CH3
        | CODEC_RX_FIFO_FLUSH_SND_CH4 | CODEC_RX_FIFO_FLUSH_SND_CH5;

    codec->REG_704 |= fifo_flush;
    hal_codec_reg_update_delay();
    codec->REG_704 &= ~fifo_flush;
}

int hal_codec_adc2_start_interface(enum HAL_CODEC_ID_T id, int dma)
{
#ifdef __AUDIO_RESAMPLE__
    if (codec->REG_7C0 & CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE) {
            hal_codec_adc2_resample_clock_enable(AUD_STREAM_CAPTURE);
#if (defined(__TWS__) || defined(IBRT)) && defined(ANC_APP)
        enum HAL_CODEC_SYNC_TYPE_T sync_type;

        sync_type = GET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL);
        if (sync_type != HAL_CODEC_SYNC_TYPE_NONE) {
            codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            codec->REG_7D0 = resample_phase_float_to_value(1.0f);
            hal_codec_reg_update_delay();
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
            codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL, sync_type);
            hal_codec_reg_update_delay();
            codec->REG_7D0 = resample_phase_float_to_value(get_capture2_resample_phase());
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
        }
#endif
        codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_ADC_SND_ENABLE;
    }
#endif
    // enable dig mic
    if (codec_mic2_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
        hal_codec_enable_dig_mic(DIG_MIC_USER_ADC2, codec_mic2_ch_map);
    }
    // enable ec mic
    if (codec_adc2_ch_map & EC_ADC_MAP) {
        enum AUD_CHANNEL_MAP_T ec_ch = (enum AUD_CHANNEL_MAP_T)(codec_adc2_ch_map & EC_ADC_MAP);
        hal_codec_ec_enable(ec_ch);
    }
    // enable adc channel and dma interface
    uint32_t i = 0, adc_cnt = 0;
    for (i = 0; i < MAX_ADC2_CH_NUM; i++) {
        if (codec_adc2_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if ((codec->REG_780 & (CODEC_CODEC_ADC_SND_EN_CH0 << i)) == 0) {
                if (i < NORMAL_ADC_CH_NUM &&
                        (codec->REG_080 & (CODEC_CODEC_ADC_EN_CH0 << i)) == 0) {
                    codec->REG_0AC |= (CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << i);
                    hal_codec_reg_update_delay();
                    codec->REG_0AC &= ~(CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << i);
                }
                codec->REG_780 |= (CODEC_CODEC_ADC_SND_EN_CH0 << i);
            }
            codec->REG_700 |= (CODEC_ADC_SND_ENABLE_CH0 << adc_cnt);
            adc_cnt++;
        }
    }

#ifdef CODEC_ADC2_DSF
    hal_codec_adc2_dsf_start_interface();
#endif

    // flush fifo
    hal_codec_adc2_flush_fifo();

    if (dma) {
        codec->REG_708 = SET_BITFIELD(codec->REG_708, CODEC_CODEC_RX_THRESHOLD_SND, HAL_CODEC_RX_FIFO_TRIGGER_LEVEL);
        codec->REG_700 |= CODEC_DMACTRL_RX_SND;
    }
    // enable adc2 hardware
#ifdef CODEC_SW_SYNC
    if (!codec_sw_sync_cap_status[ADC_MOD_IDX(CODEC_HW_MOD_ADC2)])
#endif
    {
        codec->REG_700 |= CODEC_ADC_SND_ENABLE;
    }
    return 0;
}

int hal_codec_adc2_stop_interface(enum HAL_CODEC_ID_T id)
{
    //disable adc dma interface
    codec->REG_700 &= ~(CODEC_ADC_SND_ENABLE
        | CODEC_ADC_SND_ENABLE_CH0
        | CODEC_ADC_SND_ENABLE_CH1
        | CODEC_ADC_SND_ENABLE_CH2
        | CODEC_ADC_SND_ENABLE_CH3
        | CODEC_ADC_SND_ENABLE_CH4
        | CODEC_ADC_SND_ENABLE_CH5);

    codec->REG_700 &= ~CODEC_DMACTRL_RX_SND;

    // disable adc channel
    for (int i = 0; i < MAX_ADC2_CH_NUM; i++) {
        if (codec_adc2_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            codec->REG_780 &= ~(CODEC_CODEC_ADC_SND_EN_CH0 << i);
        }
    }

#ifdef CODEC_ADC2_DSF
    hal_codec_adc2_dsf_stop_interface();
#endif

    // disable echo mic
    if (codec_adc2_ch_map & EC_ADC_MAP) {
        enum AUD_CHANNEL_MAP_T ec_ch = (enum AUD_CHANNEL_MAP_T)(codec_adc2_ch_map & EC_ADC_MAP);
        hal_codec_ec_disable(ec_ch);
    }
    // disable dig mic
    if ((codec_mic2_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL)) {
        hal_codec_disable_dig_mic(DIG_MIC_USER_ADC2);
    }
#ifdef __AUDIO_RESAMPLE__
    // disable resample
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_ADC_SND_ENABLE;
    hal_codec_adc2_resample_clock_disable(AUD_STREAM_CAPTURE);
#endif

    // flush adc2 fifo
    hal_codec_adc2_flush_fifo();

    // Cancel adc sync request
    hal_codec_sync_adc2_disable();
    hal_codec_sync_adc2_resample_rate_disable();
    hal_codec_sync_adc2_gain_disable();

    return 0;
}

void hal_codec_adc2_mute(bool mute)
{

}

int hal_codec_adc2_set_chan_vol(enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol)
{
    return 0;
}

int hal_codec_dac2_start_interface(enum HAL_CODEC_ID_T id, int dma)
{
    uint32_t fifo_flush = 0;

    TRACE(1,"%s: id=%d, dma=%d", __func__, id, dma);
#ifdef __AUDIO_RESAMPLE__
    if (codec->REG_7C0 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND) {
        hal_codec_dac2_resample_clock_enable(AUD_STREAM_PLAYBACK);
#if (defined(__TWS__) || defined(IBRT))
        enum HAL_CODEC_SYNC_TYPE_T sync_type;

        sync_type = GET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND);
        if (sync_type != HAL_CODEC_SYNC_TYPE_NONE) {
            codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND, HAL_CODEC_SYNC_TYPE_NONE);
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
            hal_codec_reg_update_delay();
            codec->REG_7C8 = resample_phase_float_to_value(1.0f);
            hal_codec_reg_update_delay();
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
            hal_codec_reg_update_delay();
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
            codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND, sync_type);
            hal_codec_reg_update_delay();
            codec->REG_7C8 = resample_phase_float_to_value(get_playback2_resample_phase());
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
        }
#endif
        codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE_SND;
        codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_ENABLE_SND;
    }
#endif
#ifdef DAC_RAMP_GAIN
    if (codec->REG_790 & CODEC_CODEC_RAMP_EN_SND_CH0) {
        codec->REG_790 &= ~CODEC_CODEC_RAMP_EN_SND_CH0;
        codec->REG_794 &= ~CODEC_CODEC_RAMP_EN_SND_CH1;
        hal_codec_reg_update_delay();
        hal_codec_set_dac2_gain_value(VALID_DAC_MAP, 0);
        hal_codec_reg_update_delay();
        codec->REG_790 |= CODEC_CODEC_RAMP_EN_SND_CH0;
        codec->REG_794 |= CODEC_CODEC_RAMP_EN_SND_CH1;
        hal_codec_reg_update_delay();
        hal_codec_restore_dig_dac2_gain();
    }
#endif

#if defined(CODEC_DAC2_USF)
    hal_codec_dac_usf_start_interface(HAL_CODEC_USF_DAC2);
#endif

    fifo_flush |= CODEC_TX_FIFO_FLUSH_SND;
    codec->REG_704 |= fifo_flush;
    hal_codec_reg_update_delay();
    codec->REG_704 &= ~fifo_flush;
    if (dma) {
        codec->REG_708 = SET_BITFIELD(codec->REG_708, CODEC_CODEC_TX_THRESHOLD_SND, HAL_CODEC_TX_FIFO_TRIGGER_LEVEL);
        codec->REG_700 |= CODEC_DMACTRL_TX_SND;
        // Delay a little time for DMA to fill the TX FIFO before sending
        for (volatile int i = 0; i < 50; i++);
    }
#ifdef CODEC_SW_SYNC
    if (!codec_sw_sync_play_status[DAC_MOD_IDX(CODEC_HW_MOD_DAC2)])
#endif
    {
        codec->REG_700 |= CODEC_DAC_ENABLE_SND;
    }

    // DAC2 data mix selection:
    // 4: DAC2 will mix with DAC1 before ANC, pass MC
    // 5: DAC2 will mix with DAC1 after ANC, not pass MC
    uint8_t mix_sel;
#ifdef AUDIO_OUTPUT_DAC2_MIX_BEFORE_ANC
    mix_sel = 4;
#else
    mix_sel = 5;
#endif
    codec->REG_784 = SET_BITFIELD(codec->REG_784, CODEC_CODEC_DAC_SND_MIX_SEL_CH0, mix_sel);
    codec->REG_784 = SET_BITFIELD(codec->REG_784, CODEC_CODEC_DAC_SND_MIX_SEL_CH1, mix_sel);
    if (mix_sel == 4) {
        hal_codec_dac_lh_enable(CODEC_DAC_LH_USER_DAC2);
    }

    if (codec_dac2_ch_map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_784 |= CODEC_CODEC_DAC_EN_SND_CH0;
    }
    if (codec_dac2_ch_map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_784 |= CODEC_CODEC_DAC_EN_SND_CH1;
    }
    codec->REG_784 |= CODEC_CODEC_DAC_EN_SND;

    return 0;
}

int hal_codec_dac3_start_interface(enum HAL_CODEC_ID_T id, int dma)
{
    uint32_t fifo_flush = 0;

    TRACE(1,"%s: id=%d, dma=%d", __func__, id, dma);

#ifdef __AUDIO_RESAMPLE__
    if (codec->REG_7C0 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI) {
        hal_codec_dac3_resample_clock_enable(AUD_STREAM_PLAYBACK);
#if (defined(__TWS__) || defined(IBRT))
        enum HAL_CODEC_SYNC_TYPE_T sync_type;

        sync_type = GET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI);
        if (sync_type != HAL_CODEC_SYNC_TYPE_NONE) {
            codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI, HAL_CODEC_SYNC_TYPE_NONE);
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
            hal_codec_reg_update_delay();
            codec->REG_7CC = resample_phase_float_to_value(1.0f);
            hal_codec_reg_update_delay();
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
            hal_codec_reg_update_delay();
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
            codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI, sync_type);
            hal_codec_reg_update_delay();
            codec->REG_7CC = resample_phase_float_to_value(get_playback3_resample_phase());
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
        }
#endif
        codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE_TRI;
        codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRI;
    }
#endif
#ifdef DAC_RAMP_GAIN
    if (codec->REG_7A8 & CODEC_CODEC_RAMP_EN_TRI_CH0) {
        codec->REG_7A8 &= ~CODEC_CODEC_RAMP_EN_TRI_CH0;
        codec->REG_7AC &= ~CODEC_CODEC_RAMP_EN_TRI_CH1;
        hal_codec_reg_update_delay();
        hal_codec_set_dac3_gain_value(VALID_DAC_MAP, 0);
        hal_codec_reg_update_delay();
        codec->REG_7A8 |= CODEC_CODEC_RAMP_EN_TRI_CH0;
        codec->REG_7AC |= CODEC_CODEC_RAMP_EN_TRI_CH1;
        hal_codec_reg_update_delay();
        hal_codec_restore_dig_dac3_gain();
    }
#endif

#if defined(CODEC_DAC3_USF)
    hal_codec_dac_usf_start_interface(HAL_CODEC_USF_DAC3);
#endif

    // clear TX fifo
    fifo_flush |= CODEC_TX_FIFO_FLUSH_TRI;
    codec->REG_704 |= fifo_flush;
    hal_codec_reg_update_delay();
    codec->REG_704 &= ~fifo_flush;
    // enable dma
    if (dma) {
        codec->REG_708 = SET_BITFIELD(codec->REG_708, CODEC_CODEC_TX_THRESHOLD_TRI, HAL_CODEC_TX_FIFO_TRIGGER_LEVEL);
        codec->REG_700 |= CODEC_DMACTRL_TX_TRI;
        // Delay a little time for DMA to fill the TX FIFO before sending
        for (volatile int i = 0; i < 50; i++);
    }
    codec->REG_700 |= CODEC_DAC_ENABLE_TRI;

    // DAC3 data mix selection:
    // 4: DAC3 will mix with DAC1 before ANC, pass MC
    // 5: DAC3 will mix with DAC1 after ANC, not pass MC
    uint8_t mix_sel;
#ifdef AUDIO_OUTPUT_DAC3_MIX_BEFORE_ANC
    mix_sel = 4;
#else
    mix_sel = 5;
#endif
    codec->REG_798 = SET_BITFIELD(codec->REG_798, CODEC_CODEC_DAC_TRI_MIX_SEL_CH0, mix_sel);
    codec->REG_798 = SET_BITFIELD(codec->REG_798, CODEC_CODEC_DAC_TRI_MIX_SEL_CH1, mix_sel);
    if (mix_sel == 4) {
        hal_codec_dac_lh_enable(CODEC_DAC_LH_USER_DAC3);
    }

    if (codec_dac3_ch_map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_798 |= CODEC_CODEC_DAC_EN_TRI_CH0;
    }
    if (codec_dac3_ch_map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_798 |= CODEC_CODEC_DAC_EN_TRI_CH1;
    }
    // enable DAC3 interface
#ifdef CODEC_SW_SYNC
    if (!codec_sw_sync_play_status[CODEC_HW_MOD_DAC3])
#endif
    {
        codec->REG_798 |= CODEC_CODEC_DAC_EN_TRI;
    }

    return 0;
}

#ifdef DAC_RESAMPLE_FIFO_CLEAR_WORKAROUND
static void clear_playback_fifo_workaround(void)
{
    codec->REG_004 |= CODEC_TX_FIFO_FLUSH;

#ifdef __AUDIO_RESAMPLE__
    uint32_t resample_value = 0;

    codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
    hal_codec_reg_update_delay();
    resample_value = codec->REG_0EC;
    codec->REG_0EC = resample_phase_float_to_value(0.0f);
    hal_codec_reg_update_delay();
    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
    hal_codec_reg_update_delay();
#endif

    // Wait at least 1 sample time
    osDelay(1);

#ifdef __AUDIO_RESAMPLE__
    codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE;
#endif

    // Wait at least 1 sample time
    osDelay(1);

#ifdef __AUDIO_RESAMPLE__
    codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
    hal_codec_reg_update_delay();
    codec->REG_0EC = resample_value;
    hal_codec_reg_update_delay();
    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
    hal_codec_reg_update_delay();
#endif

    return;
}
#endif

int hal_codec_stop_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t fifo_flush = 0;

    if (stream == AUD_STREAM_PLAYBACK) {
        codec->REG_000 &= ~CODEC_DAC_ENABLE;
        codec->REG_000 &= ~CODEC_DMACTRL_TX;

#ifdef DAC_RESAMPLE_FIFO_CLEAR_WORKAROUND
        clear_playback_fifo_workaround();
#endif

#if defined(CODEC_DAC1_USF)
        hal_codec_dac_usf_stop_interface(HAL_CODEC_USF_DAC1);
#endif

#if !defined(PSAP_APP)
        hal_codec_dac_lh_disable(CODEC_DAC_LH_USER_DAC1);
#endif

#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE;
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE;
        hal_codec_resample_clock_disable(stream);
#endif
#ifdef CODEC_MIN_PHASE
        if (min_phase_cfg & (1 << AUD_STREAM_PLAYBACK)) {
            if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH0) {
                codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH0;
                codec->REG_0B0 &= ~CODEC_CODEC_DAC_L_FIR_UPSAMPLE;
            }
            if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
                codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH1;
                codec->REG_0B0 &= ~CODEC_CODEC_DAC_R_FIR_UPSAMPLE;
            }
        }
#endif
#ifdef CODEC_DSD
        hal_codec_dsd_cfg_stop();
        dsd_enabled = false;
#endif
#ifdef AUDIO_ANC_FB_MC
        mc_started = false;
        codec->REG_04C &= ~CODEC_MC_ENABLE;
        adc_en_map &= ~(1 << CODEC_ADC_EN_REQ_MC);
        if (adc_en_map == 0) {
            codec->REG_080 &= ~CODEC_CODEC_ADC_EN;
        }
        fifo_flush |= CODEC_MC_FIFO_FLUSH;
#endif
        fifo_flush |= CODEC_TX_FIFO_FLUSH;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        // Cancel dac sync request
        hal_codec_sync_dac_disable();
        hal_codec_sync_dac_resample_rate_disable();
        hal_codec_sync_dac_gain_disable();
    } else {
        codec->REG_000 &= ~(CODEC_ADC_ENABLE | CODEC_ADC_ENABLE_CH0 | CODEC_ADC_ENABLE_CH1 | CODEC_ADC_ENABLE_CH2 |
            CODEC_ADC_ENABLE_CH3 | CODEC_ADC_ENABLE_CH4 | CODEC_ADC_ENABLE_CH5 );
        codec->REG_000 &= ~CODEC_DMACTRL_RX;
        for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
            if ((codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) &&
                    (anc_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) == 0) {
                codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << i);
            }
        }

#ifdef CODEC_ADC1_DSF
        hal_codec_adc1_dsf_stop_interface();
#endif

        if (codec_adc_ch_map & EC_ADC_MAP) {
            enum AUD_CHANNEL_MAP_T ec_ch = (enum AUD_CHANNEL_MAP_T)(codec_adc_ch_map & EC_ADC_MAP);
            hal_codec_ec_disable(ec_ch);
        }
        if ((codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) &&
                (anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) == 0) {
            hal_codec_disable_dig_mic(DIG_MIC_USER_ADC1);
        }
#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_ENABLE;
        hal_codec_resample_clock_disable(stream);
#endif
#ifdef CODEC_MIN_PHASE
        if (min_phase_cfg & (1 << AUD_STREAM_CAPTURE)) {
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH1)) {
                codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH1;
                codec->REG_0DC &= ~CODEC_CODEC_ADC_FIR_DS_EN_CH1;
            }
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH2)) {
                codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH2;
                codec->REG_0D0 &= ~CODEC_CODEC_ADC_FIR_DS_EN_CH2;
            }
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH3)) {
                codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH3;
                codec->REG_0D0 &= ~CODEC_CODEC_ADC_FIR_DS_EN_CH3;
            }
        }
#endif
        fifo_flush = CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1
            | CODEC_RX_FIFO_FLUSH_CH2 | CODEC_RX_FIFO_FLUSH_CH3
            | CODEC_RX_FIFO_FLUSH_CH4 | CODEC_RX_FIFO_FLUSH_CH5;

        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        // Cancel adc sync request
        hal_codec_sync_adc_disable();
        hal_codec_sync_adc_resample_rate_disable();
        hal_codec_sync_adc_gain_disable();
    }

    return 0;
}

#ifdef DAC_RESAMPLE_FIFO_CLEAR_WORKAROUND
static void clear_dac2_playback_fifo_workaround(void)
{
    codec->REG_704 |= CODEC_TX_FIFO_FLUSH_SND;

#ifdef __AUDIO_RESAMPLE__
    uint32_t resample_value = 0;

    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
    hal_codec_reg_update_delay();
    resample_value = codec->REG_7C8;
    codec->REG_7C8 = resample_phase_float_to_value(0.0f);
    hal_codec_reg_update_delay();
    codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
    hal_codec_reg_update_delay();
#endif

    // Wait at least 1 sample time
    osDelay(1);

#ifdef __AUDIO_RESAMPLE__
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE_SND;
#endif

    // Wait at least 1 sample time
    osDelay(1);

#ifdef __AUDIO_RESAMPLE__
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
    hal_codec_reg_update_delay();
    codec->REG_7C8 = resample_value;
    hal_codec_reg_update_delay();
    codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
    hal_codec_reg_update_delay();
#endif
}
#endif

int hal_codec_dac2_stop_interface(enum HAL_CODEC_ID_T id)
{
    uint32_t fifo_flush = 0;

    TRACE(1,"%s: id=%d", __func__, id);

    codec->REG_700 &= ~CODEC_DAC_ENABLE_SND;
    codec->REG_700 &= ~CODEC_DMACTRL_TX_SND;

#if defined(CODEC_DAC2_USF)
    hal_codec_dac_usf_stop_interface(HAL_CODEC_USF_DAC2);
#endif

#if defined(AUDIO_OUTPUT_DAC2_MIX_BEFORE_ANC)
    hal_codec_dac_lh_disable(CODEC_DAC_LH_USER_DAC2);
#endif

#ifdef DAC_RESAMPLE_FIFO_CLEAR_WORKAROUND
    clear_dac2_playback_fifo_workaround();
#endif

    if (codec_dac2_ch_map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_784 &= ~CODEC_CODEC_DAC_EN_SND_CH0;
    }
    if (codec_dac2_ch_map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_784 &= ~CODEC_CODEC_DAC_EN_SND_CH1;
    }
    codec->REG_784 &= ~CODEC_CODEC_DAC_EN_SND;
#ifdef __AUDIO_RESAMPLE__
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE_SND;
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE_SND;
    hal_codec_dac2_resample_clock_disable(AUD_STREAM_PLAYBACK);
#endif
    fifo_flush |= CODEC_TX_FIFO_FLUSH_SND;
    codec->REG_704 |= fifo_flush;
    hal_codec_reg_update_delay();
    codec->REG_704 &= ~fifo_flush;
    // Cancel dac sync request
    hal_codec_sync_dac2_disable();
    hal_codec_sync_dac2_resample_rate_disable();
    hal_codec_sync_dac2_gain_disable();
    return 0;
}

#ifdef DAC_RESAMPLE_FIFO_CLEAR_WORKAROUND
static void clear_dac3_playback_fifo_workaround(void)
{
    codec->REG_704 |= CODEC_TX_FIFO_FLUSH_TRI;

#ifdef __AUDIO_RESAMPLE__
    uint32_t resample_value = 0;

    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
    hal_codec_reg_update_delay();
    resample_value = codec->REG_7CC;
    codec->REG_7CC = resample_phase_float_to_value(0.0f);
    hal_codec_reg_update_delay();
    codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
    hal_codec_reg_update_delay();
#endif

    // Wait at least 1 sample time
    osDelay(1);

#ifdef __AUDIO_RESAMPLE__
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRI;
#endif

    // Wait at least 1 sample time
    osDelay(1);

#ifdef __AUDIO_RESAMPLE__
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
    hal_codec_reg_update_delay();
    codec->REG_7CC = resample_value;
    hal_codec_reg_update_delay();
    codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
    hal_codec_reg_update_delay();
#endif
}
#endif

int hal_codec_dac3_stop_interface(enum HAL_CODEC_ID_T id)
{
    uint32_t fifo_flush = 0;

    TRACE(1,"%s: id=%d", __func__, id);

    // disable DMA and DAC3 interface
    codec->REG_700 &= ~CODEC_DAC_ENABLE_TRI;
    codec->REG_700 &= ~CODEC_DMACTRL_TX_TRI;

#if defined(CODEC_DAC3_USF)
    hal_codec_dac_usf_stop_interface(HAL_CODEC_USF_DAC3);
#endif

#if defined(AUDIO_OUTPUT_DAC3_MIX_BEFORE_ANC)
    hal_codec_dac_lh_disable(CODEC_DAC_LH_USER_DAC3);
#endif

    // disable DAC3 chan
#ifdef DAC_RESAMPLE_FIFO_CLEAR_WORKAROUND
    clear_dac3_playback_fifo_workaround();
#endif
    codec->REG_798 &= ~CODEC_CODEC_DAC_EN_TRI_CH0;
    codec->REG_798 &= ~CODEC_CODEC_DAC_EN_TRI_CH1;
    codec->REG_798 &= ~CODEC_CODEC_DAC_EN_TRI;

    // disable DAC resample
#ifdef __AUDIO_RESAMPLE__
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRI;
    codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE_TRI;
    hal_codec_dac3_resample_clock_disable(AUD_STREAM_PLAYBACK);
#endif
    // clear TX fifo
    fifo_flush |= CODEC_TX_FIFO_FLUSH_TRI;
    codec->REG_704 |= fifo_flush;
    hal_codec_reg_update_delay();
    codec->REG_704 &= ~fifo_flush;

    // Cancel dac sync request
    hal_codec_sync_dac3_disable();
    hal_codec_sync_dac3_resample_rate_disable();
    hal_codec_sync_dac3_gain_disable();

    return 0;
}

static void hal_codec_set_dac_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    codec->REG_0B4 &= ~CODEC_CODEC_DAC_GAIN_UPDATE;
    hal_codec_reg_update_delay();
    if (map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_0B4 = SET_BITFIELD(codec->REG_0B4, CODEC_CODEC_DAC_GAIN_CH0, val);
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_0B8 = SET_BITFIELD(codec->REG_0B8, CODEC_CODEC_DAC_GAIN_CH1, val);
    }
    codec->REG_0B4 |= CODEC_CODEC_DAC_GAIN_UPDATE;
}

static void hal_codec_set_dac2_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    codec->REG_788 &= ~CODEC_CODEC_DAC_GAIN_UPDATE_SND;
    hal_codec_reg_update_delay();
    if (map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_788 = SET_BITFIELD(codec->REG_788, CODEC_CODEC_DAC_GAIN_SND_CH0, val);
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_78C = SET_BITFIELD(codec->REG_78C, CODEC_CODEC_DAC_GAIN_SND_CH1, val);
    }
    codec->REG_788 |= CODEC_CODEC_DAC_GAIN_UPDATE_SND;
}

static void hal_codec_set_dac3_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    codec->REG_79C &= ~CODEC_CODEC_DAC_GAIN_UPDATE_TRI;
    hal_codec_reg_update_delay();
    if (map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_79C = SET_BITFIELD(codec->REG_79C, CODEC_CODEC_DAC_GAIN_TRI_CH0, val);
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_7A0 = SET_BITFIELD(codec->REG_7A0, CODEC_CODEC_DAC_GAIN_TRI_CH1, val);
    }
    codec->REG_79C |= CODEC_CODEC_DAC_GAIN_UPDATE_TRI;
}

void hal_codec_get_dac_gain(float *left_gain, float *right_gain)
{
    struct DAC_GAIN_T {
        int32_t v : 20;
    };

    struct DAC_GAIN_T left;

    left.v  = GET_BITFIELD(codec->REG_0B4, CODEC_CODEC_DAC_GAIN_CH0);

    *left_gain = left.v;

    // Gain format: 6.14
    *left_gain /= (1 << 14);
    if (right_gain) {
        *right_gain = *left_gain;
    }
}

void hal_codec_dac_mute(bool mute)
{
    codec_mute[AUD_STREAM_PLAYBACK] = mute;

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(swdac_gain);
#else
    if (mute) {
        hal_codec_set_dac_gain_value((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, 0);
    } else {
        hal_codec_restore_dig_dac_gain();
    }
#endif
}

void hal_codec_dac2_mute(bool mute)
{
    codec_mute2 = mute;

#ifdef AUDIO_OUTPUT_DAC2_SW_GAIN
    hal_codec_set_dac2_sw_gain(swdac2_gain);
#else
    if (mute) {
        hal_codec_set_dac2_gain_value((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, 0);
    } else {
        hal_codec_restore_dig_dac2_gain();
    }
#endif
}

void hal_codec_dac3_mute(bool mute)
{
    codec_mute3 = mute;

#ifdef AUDIO_OUTPUT_DAC3_SW_GAIN
    hal_codec_set_dac3_sw_gain(swdac3_gain);
#else
    if (mute) {
        hal_codec_set_dac3_gain_value((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, 0);
    } else {
        hal_codec_restore_dig_dac3_gain();
    }
#endif
}

static float db_to_amplitude_ratio(int32_t db)
{
    float coef;

    if (db == ZERODB_DIG_DBVAL) {
        coef = 1;
    } else if (db <= MIN_DIG_DBVAL) {
        coef = 0;
    } else {
        if (db > MAX_DIG_DBVAL) {
            db = MAX_DIG_DBVAL;
        }
        coef = db_to_float(db);
    }

    return coef;
}

static float digdac_gain_to_float(int32_t gain)
{
    float coef;

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    gain += digdac_gain_offset_nr;
#endif

    coef = db_to_amplitude_ratio(gain);

#ifdef AUDIO_OUTPUT_DC_CALIB
    coef *= dac_dc_gain_attn;
#endif

#ifdef ANC_APP
    coef *= anc_boost_gain_attn;
#endif

    return coef;
}

#ifdef AUDIO_OUTPUT_DC_AUTO_CALIB
void hal_codec_set_dig_dac_gain_dr(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;

    coef = db_to_amplitude_ratio(gain);

    // Gain format: 6.14
    int32_t s_val = (int32_t)(coef * (1 << 14));
    val = __SSAT(s_val, 20);
    hal_codec_set_dac_gain_value(map, val);
}
#endif

static void hal_codec_set_dig_dac_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;
    bool mute;

    if (map & AUD_CHANNEL_MAP_CH0) {
        digdac_gain[0] = gain;
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        digdac_gain[1] = gain;
    }

#ifdef AUDIO_OUTPUT_SW_GAIN
    mute = false;
#else
    mute = codec_mute[AUD_STREAM_PLAYBACK];
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    if (codec->REG_0BC & CODEC_CODEC_DAC_SDM_CLOSE) {
        mute = true;
    }
#endif

    if (mute) {
        val = 0;
    } else {
        coef = digdac_gain_to_float(gain);

        // Gain format: 6.14
        int32_t s_val = (int32_t)(coef * (1 << 14));
        val = __SSAT(s_val, 20);
    }

    hal_codec_set_dac_gain_value(map, val);
}

POSSIBLY_UNUSED static void hal_codec_restore_dig_dac_gain(void)
{
    if (digdac_gain[0] == digdac_gain[1]) {
        hal_codec_set_dig_dac_gain((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, digdac_gain[0]);
    } else {
        hal_codec_set_dig_dac_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0, digdac_gain[0]);
        hal_codec_set_dig_dac_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH1, digdac_gain[1]);
    }
}

POSSIBLY_UNUSED static void hal_codec_restore_dig_dac2_gain(void)
{
    if (digdac2_gain[0] == digdac2_gain[1]) {
        hal_codec_set_dig_dac2_gain((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, digdac2_gain[0]);
    } else {
        hal_codec_set_dig_dac2_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0, digdac2_gain[0]);
        hal_codec_set_dig_dac2_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH1, digdac2_gain[1]);
    }
}

POSSIBLY_UNUSED static void hal_codec_restore_dig_dac3_gain(void)
{
    if (digdac3_gain[0] == digdac3_gain[1]) {
        hal_codec_set_dig_dac3_gain((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, digdac3_gain[0]);
    } else {
        hal_codec_set_dig_dac3_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0, digdac3_gain[0]);
        hal_codec_set_dig_dac3_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH1, digdac3_gain[1]);
    }
}

static void hal_codec_set_dig_dac2_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;
    bool mute;

    if (map & AUD_CHANNEL_MAP_CH0) {
        digdac2_gain[0] = gain;
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        digdac2_gain[1] = gain;
    }

#ifdef AUDIO_OUTPUT_DAC2_SW_GAIN
    mute = false;
#else
    mute = codec_mute2;
#endif

    if (mute) {
        val = 0;
    } else {
        coef = digdac_gain_to_float(gain);

        // Gain format: 6.14
        int32_t s_val = (int32_t)(coef * (1 << 14));
        val = __SSAT(s_val, 20);
    }

    hal_codec_set_dac2_gain_value(map, val);
}

static void hal_codec_set_dig_dac3_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;
    bool mute;

    if (map & AUD_CHANNEL_MAP_CH0) {
        digdac3_gain[0] = gain;
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        digdac3_gain[1] = gain;
    }

#ifdef AUDIO_OUTPUT_DAC3_SW_GAIN
    mute = false;
#else
    mute = codec_mute3;
#endif

    if (mute) {
        val = 0;
    } else {
        coef = digdac_gain_to_float(gain);

        // Gain format: 6.14
        int32_t s_val = (int32_t)(coef * (1 << 14));
        val = __SSAT(s_val, 20);
    }

    hal_codec_set_dac3_gain_value(map, val);
}

#ifdef AUDIO_OUTPUT_SW_GAIN
static void hal_codec_set_sw_gain(int32_t gain)
{
    float coef;
    bool mute;

    swdac_gain = gain;

    mute = codec_mute[AUD_STREAM_PLAYBACK];

    if (mute) {
        coef = 0;
    } else {
        coef = digdac_gain_to_float(gain);
    }

    if (sw_output_coef_callback) {
        sw_output_coef_callback(coef);
    }
}

void hal_codec_set_sw_output_coef_callback(HAL_CODEC_SW_OUTPUT_COEF_CALLBACK callback)
{
    sw_output_coef_callback = callback;
}
#endif

#ifdef AUDIO_OUTPUT_DAC2_SW_GAIN
static void hal_codec_set_dac2_sw_gain(int32_t gain)
{
    float coef;
    bool mute;

    swdac2_gain = gain;

    mute = codec_mute2;

    if (mute) {
        coef = 0;
    } else {
        coef = digdac_gain_to_float(gain);
    }

    if (sw_output_coef_callback2) {
        sw_output_coef_callback2(coef);
    }
}

void hal_codec_set_dac2_sw_output_coef_callback(HAL_CODEC_SW_OUTPUT_COEF_CALLBACK callback)
{
    sw_output_coef_callback2 = callback;
}
#endif

#ifdef AUDIO_OUTPUT_DAC3_SW_GAIN
static void hal_codec_set_dac3_sw_gain(int32_t gain)
{
    float coef;
    bool mute;

    swdac3_gain = gain;

    mute = codec_mute3;

    if (mute) {
        coef = 0;
    } else {
        coef = digdac_gain_to_float(gain);
    }

    if (sw_output_coef_callback3) {
        sw_output_coef_callback3(coef);
    }
}

void hal_codec_set_dac3_sw_output_coef_callback(HAL_CODEC_SW_OUTPUT_COEF_CALLBACK callback)
{
    sw_output_coef_callback3 = callback;
}
#endif

#if defined(CODEC_ADC_DC_FILTER_FACTOR)
void hal_codec_enable_adc_dc_filter(enum AUD_CHANNEL_MAP_T map, uint32_t enable)
{
    uint32_t val = codec->REG_0C0;

    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (enable) {
                val &= ~(CODEC_ADC_UDC_CH0_MASK << (4 * i));
                codec->REG_0C0 = val | CODEC_ADC_UDC_CH0(CODEC_ADC_DC_FILTER_FACTOR) << (4 * i);

                codec->REG_0BC &= ~(CODEC_ADC_DCF_BYPASS_CH0 << i);
            } else {
                codec->REG_0BC |= (CODEC_ADC_DCF_BYPASS_CH0 << i);
            }
        }
    }
}
#endif

static void hal_codec_set_adc_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    uint32_t gain_update = 0;

    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_GAIN_CH0, val);
            gain_update |= (CODEC_CODEC_ADC_GAIN_UPDATE_CH0 << i);
        }
    }
    codec->REG_0B4 &= ~gain_update;
    hal_codec_reg_update_delay();
    codec->REG_0B4 |= gain_update;
}

static void hal_codec_set_dig_adc_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;
    bool mute;
    int i;
    int32_t s_val;

    for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (1 << i)) {
            digadc_gain[i] = gain;
        }
    }

    mute = codec_mute[AUD_STREAM_CAPTURE];

    if (mute) {
        val = 0;
    } else {
#ifdef ANC_APP
        enum AUD_CHANNEL_MAP_T adj_map;
        int32_t anc_gain;

        adj_map = map & anc_adc_gain_offset_map;
        while (adj_map) {
            i = get_msb_pos(adj_map);
            adj_map &= ~(1 << i);
            anc_gain = gain + anc_adc_gain_offset[i];
            coef = db_to_amplitude_ratio(anc_gain);
            coef /= anc_boost_gain_attn;
            // Gain format: 8.12
            s_val = (int32_t)(coef * (1 << 12));
            val = __SSAT(s_val, 20);
            hal_codec_set_adc_gain_value((1 << i), val);
        }

        map &= ~anc_adc_gain_offset_map;
#endif

        if (map) {
            coef = db_to_amplitude_ratio(gain);
#ifdef ANC_APP
            coef /= anc_boost_gain_attn;
#endif
            // Gain format: 8.12
            s_val = (int32_t)(coef * (1 << 12));
            val = __SSAT(s_val, 20);
        } else {
            val = 0;
        }
    }

    hal_codec_set_adc_gain_value(map, val);
}

static void hal_codec_restore_dig_adc_gain(void)
{
    int i;

    for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        hal_codec_set_dig_adc_gain((enum AUD_CHANNEL_MAP_T)(1 << i), digadc_gain[i]);
    }
}

POSSIBLY_UNUSED static void hal_codec_get_adc_gain(enum AUD_CHANNEL_MAP_T map, float *gain)
{
    struct ADC_GAIN_T {
        int32_t v : 20;
    };

    struct ADC_GAIN_T adc_val;

    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            adc_val.v = GET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_GAIN_CH0);

            *gain = adc_val.v;
            // Gain format: 8.12
            *gain /= (1 << 12);
            return;
        }
    }

    *gain = 0;
}

void hal_codec_adc_mute(bool mute)
{
    codec_mute[AUD_STREAM_CAPTURE] = mute;

    if (mute) {
        hal_codec_set_adc_gain_value((enum AUD_CHANNEL_MAP_T)NORMAL_ADC_MAP, 0);
    } else {
        hal_codec_restore_dig_adc_gain();
    }
}

int hal_codec_set_chan_vol(enum AUD_STREAM_T stream, enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol)
{
    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef AUDIO_OUTPUT_SW_GAIN
        ASSERT(false, "%s: Cannot set play chan vol with AUDIO_OUTPUT_SW_GAIN", __func__);
#else
#ifdef SINGLE_CODEC_DAC_VOL
        ASSERT(false, "%s: Cannot set play chan vol with SINGLE_CODEC_DAC_VOL", __func__);
#else
        const struct CODEC_DAC_VOL_T *vol_tab_ptr;

        vol_tab_ptr = hal_codec_get_dac_volume(vol);
        if (vol_tab_ptr) {
            if (ch_map & AUD_CHANNEL_MAP_CH0) {
                hal_codec_set_dig_dac_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0, vol_tab_ptr->sdac_volume);
            }
            if (ch_map & AUD_CHANNEL_MAP_CH1) {
                hal_codec_set_dig_dac_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH1, vol_tab_ptr->sdac_volume);
            }
        }
#endif
#endif
    } else {
#ifdef SINGLE_CODEC_ADC_VOL
        ASSERT(false, "%s: Cannot set cap chan vol with SINGLE_CODEC_ADC_VOL", __func__);
#else
        uint8_t mic_ch, adc_ch;
        enum AUD_CHANNEL_MAP_T map;
        const CODEC_ADC_VOL_T *adc_gain_db;

        adc_gain_db = hal_codec_get_adc_volume(vol);
        if (adc_gain_db) {
            map = (enum AUD_CHANNEL_MAP_T)(ch_map & ~EC_MIC_MAP);
            while (map) {
                mic_ch = get_lsb_pos(map);
                map &= ~(1 << mic_ch);
                adc_ch = hal_codec_get_adc_chan((enum AUD_CHANNEL_MAP_T)(1 << mic_ch),
                        codec_mic_ch_map, codec_adc_ch_map, NORMAL_ADC_CH_NUM);
                ASSERT(adc_ch < NORMAL_ADC_CH_NUM, "%s: Bad cap ch_map=0x%X (ch=%u)", __func__, ch_map, mic_ch);
                hal_codec_set_dig_adc_gain((enum AUD_CHANNEL_MAP_T)(1 << adc_ch), *adc_gain_db);
            }
        }
#endif
    }

    return 0;
}

int hal_codec_dac2_set_chan_vol(enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol)
{
#ifdef SINGLE_CODEC_DAC_VOL
    ASSERT(false, "%s: Cannot set play chan vol with SINGLE_CODEC_DAC_VOL", __func__);
#else
    const struct CODEC_DAC_VOL_T *vol_tab_ptr;

    vol_tab_ptr = hal_codec_get_dac_volume(vol);
    if (vol_tab_ptr) {
        if (ch_map & AUD_CHANNEL_MAP_CH0) {
            hal_codec_set_dig_dac2_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0, vol_tab_ptr->sdac_volume);
        }
        if (ch_map & AUD_CHANNEL_MAP_CH1) {
            hal_codec_set_dig_dac2_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH1, vol_tab_ptr->sdac_volume);
        }
    }
#endif
    return 0;
}

int hal_codec_dac3_set_chan_vol(enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol)
{
#ifdef SINGLE_CODEC_DAC_VOL
    ASSERT(false, "%s: Cannot set play chan vol with SINGLE_CODEC_DAC_VOL", __func__);
#else
    const struct CODEC_DAC_VOL_T *vol_tab_ptr;

    vol_tab_ptr = hal_codec_get_dac_volume(vol);
    if (vol_tab_ptr) {
        if (ch_map & AUD_CHANNEL_MAP_CH0) {
            hal_codec_set_dig_dac3_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0, vol_tab_ptr->sdac_volume);
        }
        if (ch_map & AUD_CHANNEL_MAP_CH1) {
            hal_codec_set_dig_dac3_gain((enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH1, vol_tab_ptr->sdac_volume);
        }
    }
#endif
    return 0;
}

static int hal_codec_set_dac_hbf_bypass_cnt(uint32_t cnt, int head_first)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_DAC_HBF1_BYPASS | CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;

    if (cnt == 0) {
    } else if (cnt == 1) {
        if (head_first) {
            bypass = CODEC_CODEC_DAC_HBF1_BYPASS;
        } else {
            bypass = CODEC_CODEC_DAC_HBF3_BYPASS;
        }
    } else if (cnt == 2) {
        if (head_first) {
            bypass = CODEC_CODEC_DAC_HBF1_BYPASS | CODEC_CODEC_DAC_HBF2_BYPASS;
        } else {
            bypass = CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;
        }
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_DAC_HBF1_BYPASS | CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;
    } else {
        ASSERT(false, "%s: Invalid dac bypass cnt: %u", __FUNCTION__, cnt);
    }

    // OSR is fixed to 512
    //codec->REG_0B0 = SET_BITFIELD(codec->REG_0B0, CODEC_CODEC_DAC_OSR_SEL, 2);

    codec->REG_0B0 = (codec->REG_0B0 & ~bypass_mask) | bypass;
    return 0;
}

static int hal_codec_set_dac2_hbf_bypass_cnt(uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_DAC_HBF1_BYPASS_SND | CODEC_CODEC_DAC_HBF2_BYPASS_SND | CODEC_CODEC_DAC_HBF3_BYPASS_SND;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_DAC_HBF3_BYPASS_SND;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_DAC_HBF2_BYPASS_SND | CODEC_CODEC_DAC_HBF3_BYPASS_SND;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_DAC_HBF1_BYPASS_SND | CODEC_CODEC_DAC_HBF2_BYPASS_SND | CODEC_CODEC_DAC_HBF3_BYPASS_SND;
    } else {
        ASSERT(false, "%s: Invalid dac bypass cnt: %u", __FUNCTION__, cnt);
    }

    codec->REG_784 = (codec->REG_784 & ~bypass_mask) | bypass;
    return 0;
}

static int hal_codec_set_dac3_hbf_bypass_cnt(uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_DAC_HBF1_BYPASS_TRI
                        | CODEC_CODEC_DAC_HBF2_BYPASS_TRI
                        | CODEC_CODEC_DAC_HBF3_BYPASS_TRI;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_DAC_HBF3_BYPASS_TRI;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_DAC_HBF2_BYPASS_TRI
            | CODEC_CODEC_DAC_HBF3_BYPASS_TRI;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_DAC_HBF1_BYPASS_TRI
                | CODEC_CODEC_DAC_HBF2_BYPASS_TRI
                | CODEC_CODEC_DAC_HBF3_BYPASS_TRI;
    } else {
        ASSERT(false, "%s: Invalid dac bypass cnt: %u", __FUNCTION__, cnt);
    }

    codec->REG_798 = (codec->REG_798 & ~bypass_mask) | bypass;
    return 0;
}

static int hal_codec_set_dac_up(uint32_t val)
{
    uint32_t sel = 0;

    if (val == 2) {
        sel = 0;
    } else if (val == 3) {
        sel = 1;
    } else if (val == 4) {
        sel = 2;
    } else if (val == 6) {
        sel = 3;
    } else if (val == 1) {
        sel = 4;
    } else {
        ASSERT(false, "%s: Invalid dac up: %u", __FUNCTION__, val);
    }
    codec->REG_0B0 = SET_BITFIELD(codec->REG_0B0, CODEC_CODEC_DAC_UP_SEL, sel);
    return 0;
}

static int hal_codec_set_dac2_up(uint32_t val)
{
    uint32_t sel = 0;

    if (val == 2) {
        sel = 0;
    } else if (val == 3) {
        sel = 1;
    } else if (val == 4) {
        sel = 2;
    } else if (val == 6) {
        sel = 3;
    } else if (val == 1) {
        sel = 4;
    } else {
        ASSERT(false, "%s: Invalid dac up: %u", __FUNCTION__, val);
    }
    codec->REG_784 = SET_BITFIELD(codec->REG_784, CODEC_CODEC_DAC_UP_SEL_SND, sel);
    return 0;
}

static int hal_codec_set_dac3_up(uint32_t val)
{
    uint32_t sel = 0;

    if (val == 2) {
        sel = 0;
    } else if (val == 3) {
        sel = 1;
    } else if (val == 4) {
        sel = 2;
    } else if (val == 6) {
        sel = 3;
    } else if (val == 1) {
        sel = 4;
    } else {
        ASSERT(false, "%s: Invalid dac up: %u", __FUNCTION__, val);
    }
    codec->REG_798 = SET_BITFIELD(codec->REG_798, CODEC_CODEC_DAC_UP_SEL_TRI, sel);
    return 0;
}

POSSIBLY_UNUSED static uint32_t hal_codec_get_dac_up(void)
{
    uint32_t sel;

    sel = GET_BITFIELD(codec->REG_0B0, CODEC_CODEC_DAC_UP_SEL);
    if (sel == 0) {
        return 2;
    } else if (sel == 1) {
        return 3;
    } else if (sel == 2) {
        return 4;
    } else if (sel == 3) {
        return 6;
    } else {
        return 1;
    }
}

POSSIBLY_UNUSED static uint32_t hal_codec_get_dac2_up(void)
{
    uint32_t sel;

    sel = GET_BITFIELD(codec->REG_784, CODEC_CODEC_DAC_UP_SEL_SND);
    if (sel == 0) {
        return 2;
    } else if (sel == 1) {
        return 3;
    } else if (sel == 2) {
        return 4;
    } else if (sel == 3) {
        return 6;
    } else {
        return 1;
    }
}

static int hal_codec_set_adc_down(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    uint32_t sel = 0;

    if (val == 3) {
        sel = 0;
    } else if (val == 6) {
        sel = 1;
    } else if (val == 1) {
        sel = 2;
    } else {
        ASSERT(false, "%s: Invalid adc down: %u", __FUNCTION__, val);
    }
    // Normal ADC Channel
    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_DOWN_SEL_CH0, sel);
        }
    }
    // EC0 ADC Channel
    if (map & AUD_CHANNEL_MAP_CH4) {
        codec->REG_09C = SET_BITFIELD(codec->REG_09C, CODEC_CODEC_EC_DOWN_SEL_CH0, sel);
    }
    // EC1 ADC Channel
    if (map & AUD_CHANNEL_MAP_CH5) {
        codec->REG_0A0 = SET_BITFIELD(codec->REG_0A0, CODEC_CODEC_EC_DOWN_SEL_CH1, sel);
    }
    return 0;
}

static int hal_codec_set_adc_hbf_bypass_cnt(enum AUD_CHANNEL_MAP_T map, uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_ADC_HBF1_BYPASS_CH0 | CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
    uint32_t bypass_ec = 0;
    uint32_t bypass_mask_ec = 0;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
        bypass_ec = CODEC_CODEC_EC_HBF3_BYPASS_CH0;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
        bypass_ec = CODEC_CODEC_EC_HBF2_BYPASS_CH0 | CODEC_CODEC_EC_HBF3_BYPASS_CH0;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_ADC_HBF1_BYPASS_CH0 | CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
        bypass_ec = CODEC_CODEC_EC_HBF1_BYPASS_CH0 | CODEC_CODEC_EC_HBF2_BYPASS_CH0 | CODEC_CODEC_EC_HBF3_BYPASS_CH0;
    } else {
        ASSERT(false, "%s: Invalid bypass cnt: %u", __FUNCTION__, cnt);
    }
    // Normal ADC Channel
    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            *(&codec->REG_084 + i) = (*(&codec->REG_084 + i) & ~bypass_mask) | bypass;
        }
    }
    // EC0 ADC Channel
    if (map & AUD_CHANNEL_MAP_CH4) {
        bypass_mask_ec = CODEC_CODEC_EC_HBF1_BYPASS_CH0 | CODEC_CODEC_EC_HBF2_BYPASS_CH0 | CODEC_CODEC_EC_HBF3_BYPASS_CH0;
        codec->REG_09C = (codec->REG_09C & ~bypass_mask_ec) | bypass_ec;
    }
    // EC1 ADC Channel
    if (map & AUD_CHANNEL_MAP_CH5) {
        bypass_mask_ec = CODEC_CODEC_EC_HBF1_BYPASS_CH1 | CODEC_CODEC_EC_HBF2_BYPASS_CH1 | CODEC_CODEC_EC_HBF3_BYPASS_CH1;
        codec->REG_0A0 = (codec->REG_0A0 & ~bypass_mask_ec) | bypass_ec;
    }
    return 0;
}

#ifdef __AUDIO_RESAMPLE__
static float get_playback3_resample_phase(void)
{
    return (float)codec_dac_sample_rate[dac3_resample_rate_idx].codec_freq / hal_cmu_get_crystal_freq();
}

static float get_playback2_resample_phase(void)
{
    return (float)codec_dac_sample_rate[dac2_resample_rate_idx].codec_freq / hal_cmu_get_crystal_freq();
}

static float get_playback_resample_phase(void)
{
    return (float)codec_dac_sample_rate[resample_rate_idx[AUD_STREAM_PLAYBACK]].codec_freq / hal_cmu_get_crystal_freq();
}

static float get_capture_resample_phase(void)
{
    return (float)hal_cmu_get_crystal_freq() / codec_adc_sample_rate[resample_rate_idx[AUD_STREAM_CAPTURE]].codec_freq;
}

static float get_capture2_resample_phase(void)
{
    return (float)hal_cmu_get_crystal_freq() / codec_adc_sample_rate[adc2_resample_rate_idx].codec_freq;
}

static uint32_t resample_phase_float_to_value(float phase)
{
    if (phase >= 4.0) {
        return (uint32_t)-1;
    } else {
        // Phase format: 2.30
        return (uint32_t)(phase * (1 << 30));
    }
}

POSSIBLY_UNUSED static float resample_phase_value_to_float(uint32_t value)
{
    // Phase format: 2.30
    return (float)value / (1 << 30);
}
#endif

#ifdef SIDETONE_ENABLE
POSSIBLY_UNUSED static void hal_codec_set_sidetone_adc_chan(enum AUD_CHANNEL_MAP_T chan_map)
{
    uint8_t ch;
    ch = get_msb_pos(chan_map);
    codec->REG_080 = SET_BITFIELD(codec->REG_080, CODEC_CODEC_SIDE_TONE_MIC_SEL, ch);
}
#endif

static void hal_codec_set_dac_gain_ramp_interval(enum AUD_SAMPRATE_T rate)
{
#ifdef DAC_RAMP_GAIN
    int ramp_intvl = CODEC_DAC_GAIN_RAMP_INTERVAL;

    if ((codec->REG_154 & CODEC_CODEC_RAMP_EN_CH0) == 0) {
        return;
    }

    if (rate >= AUD_SAMPRATE_44100 * 8) {
        ramp_intvl += 3;
    } else if (rate >= AUD_SAMPRATE_44100 * 4) {
        ramp_intvl += 2;
    } else if (rate >= AUD_SAMPRATE_44100 * 2) {
        ramp_intvl += 1;
    } else if (rate >= AUD_SAMPRATE_44100) {
        ramp_intvl += 0;
    } else if (rate >= AUD_SAMPRATE_44100 / 2) {
        ramp_intvl -= 1;
    } else if (rate >= AUD_SAMPRATE_44100 / 4) {
        ramp_intvl -= 2;
    } else {
        ramp_intvl -= 3;
    }
    if (ramp_intvl < 0) {
        ramp_intvl = 0;
    } else if (ramp_intvl >= CODEC_DAC_GAIN_RAMP_INTVL_QTY) {
        ramp_intvl = CODEC_DAC_GAIN_RAMP_INTVL_QTY - 1;
    }
    codec->REG_154 = SET_BITFIELD(codec->REG_154, CODEC_CODEC_RAMP_INTERVAL_CH0, ramp_intvl);
    codec->REG_158 = SET_BITFIELD(codec->REG_158, CODEC_CODEC_RAMP_INTERVAL_CH0, ramp_intvl);
#endif
}

int hal_codec_setup_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, const struct HAL_CODEC_CONFIG_T *cfg)
{
    int i;
    int rate_idx;
    uint32_t ana_dig_div;
    enum AUD_SAMPRATE_T sample_rate;
    POSSIBLY_UNUSED uint32_t mask, val;

    if (stream == AUD_STREAM_PLAYBACK) {
        if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
            if (cfg->channel_num == AUD_CHANNEL_NUM_2) {
                if (cfg->channel_map != (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
                    TRACE(2,"\n!!! WARNING:%s: Bad play stereo ch map: 0x%X\n", __func__, cfg->channel_map);
                }
                codec->REG_044 |= CODEC_DUAL_CHANNEL_DAC;
            } else {
                ASSERT(cfg->channel_num == AUD_CHANNEL_NUM_1, "%s: Bad play ch num: %u", __func__, cfg->channel_num);
                // Allow to DMA one channel but output 2 channels
                ASSERT((cfg->channel_map & ~(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) == 0,
                    "%s: Bad play mono ch map: 0x%X", __func__, cfg->channel_map);
                codec->REG_044 &= ~CODEC_DUAL_CHANNEL_DAC;
            }
            codec_dac_ch_map = AUD_CHANNEL_MAP_NULL;
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH0) {
                codec_dac_ch_map |= AUD_CHANNEL_MAP_CH0;
            }
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH1) {
                codec_dac_ch_map |= AUD_CHANNEL_MAP_CH1;
            }
        }

        if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
            if (cfg->bits == AUD_BITS_16) {
                codec->REG_044 = (codec->REG_044 & ~CODEC_MODE_32BIT_DAC) | CODEC_MODE_16BIT_DAC;
            } else if (cfg->bits == AUD_BITS_24) {
                codec->REG_044 &= ~(CODEC_MODE_16BIT_DAC | CODEC_MODE_32BIT_DAC);
            } else if (cfg->bits == AUD_BITS_32) {
                codec->REG_044 = (codec->REG_044 & ~CODEC_MODE_16BIT_DAC) | CODEC_MODE_32BIT_DAC;
            } else {
                ASSERT(false, "%s: Bad play bits: %u", __func__, cfg->bits);
            }
        }

        if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
            sample_rate = cfg->sample_rate;
#ifdef CODEC_DSD
            if (dsd_enabled) {
                if (sample_rate == AUD_SAMPRATE_176400) {
                    dsd_rate_idx = 0;
                } else if (sample_rate == AUD_SAMPRATE_352800) {
                    dsd_rate_idx = 1;
                } else if (sample_rate == AUD_SAMPRATE_705600) {
                    dsd_rate_idx = 2;
                } else {
                    ASSERT(false, "%s: Bad DSD sample rate: %u", __func__, sample_rate);
                }
                sample_rate = AUD_SAMPRATE_44100;
            }
#endif

            for (i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++) {
                if (codec_dac_sample_rate[i].sample_rate == sample_rate) {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %u", __func__, sample_rate);
            rate_idx = i;
            ana_dig_div = codec_dac_sample_rate[rate_idx].codec_div / codec_dac_sample_rate[rate_idx].cmu_div;
            ASSERT(ana_dig_div * codec_dac_sample_rate[rate_idx].cmu_div == codec_dac_sample_rate[rate_idx].codec_div,
                "%s: Invalid playback div for rate %u: codec_div=%u cmu_div=%u", __func__, sample_rate,
                codec_dac_sample_rate[rate_idx].codec_div, codec_dac_sample_rate[rate_idx].cmu_div);

            TRACE(2,"[%s] playback sample_rate=%d", __func__, sample_rate);

#ifdef CODEC_TIMER
            cur_codec_freq = codec_dac_sample_rate[rate_idx].codec_freq;
#endif

            codec_rate_idx[AUD_STREAM_PLAYBACK] = rate_idx;

#ifdef __AUDIO_RESAMPLE__
            if (hal_cmu_get_audio_resample_status() && codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL) {
#ifdef CODEC_TIMER
                cur_codec_freq = CODEC_FREQ_CRYSTAL;
#endif
                if ((codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) == 0 ||
                        resample_rate_idx[AUD_STREAM_PLAYBACK] != rate_idx) {
                    resample_rate_idx[AUD_STREAM_PLAYBACK] = rate_idx;
                    codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                    hal_codec_reg_update_delay();
                    codec->REG_0EC = resample_phase_float_to_value(get_playback_resample_phase());
                    hal_codec_reg_update_delay();
                    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                }

                mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE;
                val = 0;
                if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH0) {
                    val |= CODEC_CODEC_RESAMPLE_DAC_L_ENABLE;
                }
                if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
                    val |= CODEC_CODEC_RESAMPLE_DAC_R_ENABLE;
                }
            } else {
                mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE | CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                val = 0;
            }
            codec->REG_0E4 = (codec->REG_0E4 & ~mask) | val;
#endif

#ifdef __AUDIO_RESAMPLE__
            if (!hal_cmu_get_audio_resample_status())
#endif
            {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL,
                    "%s: playback sample rate %u is for resample only", __func__, sample_rate);
#endif
                analog_aud_freq_pll_config(codec_dac_sample_rate[rate_idx].codec_freq, codec_dac_sample_rate[rate_idx].codec_div);
                hal_codec_dac_set_div(codec_dac_sample_rate[rate_idx].cmu_div * CODEC_FREQ_EXTRA_DIV);
                hal_codec_ci2s_set_div(codec_dac_sample_rate[rate_idx].cmu_div);
            }

            hal_codec_set_dac_gain_ramp_interval(cfg->sample_rate);

#ifdef PSAP_APP
            uint32_t ratio;
            uint32_t bypass;
            uint32_t bypass_cnt = 0;
            bool head_first = false;

            mask = CODEC_PSAP_RATE_SEL | CODEC_DAC_PSAP_MODE | CODEC_UP_SEL_PSAP_MASK;
            val = 0;

            ratio = codec_dac_sample_rate[codec_rate_idx[AUD_STREAM_PLAYBACK]].dac_up;
            bypass = codec_dac_sample_rate[codec_rate_idx[AUD_STREAM_PLAYBACK]].bypass_cnt;
            if ((ratio == 3 || ratio == 6) && bypass == 0) {
                // 16K or 8K
                val |= CODEC_UP_SEL_PSAP((ratio == 3) ? 1 : 3) | CODEC_DAC_PSAP_MODE;
            } else if (ratio == 3 && bypass == 1) {
                // 32K
#if !defined(CODEC_DAC1_USF)
                ASSERT(false, "[%s]: CODEC_DAC1_USF should be dedfined for 32K", __func__);

                head_first = true;
                val |= CODEC_UP_SEL_PSAP(1) | CODEC_DAC_PSAP_MODE;
#endif
                bypass_cnt = 1;
                val |= CODEC_PSAP_RATE_SEL;
            } else if (ratio == 1 && bypass == 0) {
                // 48K
                //val |= CODEC_UP_SEL_PSAP(4) | CODEC_DAC_PSAP_MODE;
                val |= 0;
            } else if (ratio == 1 && bypass == 1) {
                // 96K
                bypass_cnt = 1;
                val |= CODEC_PSAP_RATE_SEL;
            } else {
                ASSERT(false, "PSAP not support dac rate: %u", codec_dac_sample_rate[codec_rate_idx[AUD_STREAM_PLAYBACK]].sample_rate);
            }
            codec->REG_400 = (codec->REG_400 & ~mask) | val;

            hal_codec_set_dac_up(1);
            hal_codec_set_dac_hbf_bypass_cnt(bypass_cnt, head_first);
#else
#if !defined(CODEC_DAC1_USF)
            hal_codec_set_dac_up(codec_dac_sample_rate[rate_idx].dac_up);
            hal_codec_set_dac_hbf_bypass_cnt(codec_dac_sample_rate[rate_idx].bypass_cnt, false);
#endif
#endif

#if defined(CODEC_DAC1_USF)
            hal_codec_dac_usf_setup_stream(HAL_CODEC_USF_DAC1, sample_rate);
#endif

#ifdef AUDIO_ANC_FB_MC
            codec->REG_04C = SET_BITFIELD(codec->REG_04C, CODEC_MC_DELAY, codec_dac_sample_rate[rate_idx].mc_delay);
#endif

        }

        if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
            const struct CODEC_DAC_VOL_T *vol_tab_ptr;

            vol_tab_ptr = hal_codec_get_dac_volume(cfg->vol);
            if (vol_tab_ptr) {
#ifdef AUDIO_OUTPUT_SW_GAIN
                hal_codec_set_sw_gain(vol_tab_ptr->sdac_volume);
#else
                analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
                hal_codec_set_dig_dac_gain((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, vol_tab_ptr->sdac_volume);
#endif
#ifdef PERF_TEST_POWER_KEY
                // Update performance test power after applying new dac volume
                hal_codec_update_perf_test_power();
#endif
            }
        }
    } else {
        enum AUD_CHANNEL_MAP_T mic_map;
        enum AUD_CHANNEL_MAP_T reserv_map;
        uint8_t cnt;
        uint8_t ch_idx;
        uint32_t cfg_set_mask;
        uint32_t cfg_clr_mask;
        bool alloc_adc = false;

        mic_map = AUD_CHANNEL_MAP_NULL;
        if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
            alloc_adc = true;
        }
#if defined(ANC_APP) && (defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R))
        if (ANC_TT_MIC_CH_L && (FORCE_TT_ADC_ALLOC || anc_tt_adc_ch_l == 0)) {
            alloc_adc = true;
        }
        if (ANC_TT_MIC_CH_R && (FORCE_TT_ADC_ALLOC || anc_tt_adc_ch_r == 0)) {
            alloc_adc = true;
        }
#endif
#if defined(AUDIO_OUTPUT_DC_CALIB) && defined(AUDIO_OUTPUT_DC_AUTO_CALIB)
        if (dac_dc_calib_status) {
            alloc_adc = false;
            mic_map = cfg->channel_map;
            codec_mic_ch_map = mic_map;
            codec_adc_ch_map = 0;
            for (i = 0; i < 2 && mic_map; i++) {
                ch_idx = get_lsb_pos(mic_map);
                mic_map &= ~(1 << ch_idx);
                codec_adc_ch_map |= (AUD_CHANNEL_MAP_CH0 << i);
                *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
            }
            codec->REG_0C4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 | CODEC_CODEC_PDM_ADC_SEL_CH1);
        }
#endif

        if (alloc_adc) {
            mic_map = cfg->channel_map;
            codec_mic_ch_map = mic_map;
            codec_adc_ch_map = AUD_CHANNEL_MAP_NULL;
#ifdef SIDETONE_DEDICATED_ADC_CHAN
            sidetone_adc_ch_map = AUD_CHANNEL_MAP_NULL;
#endif
            reserv_map = AUD_CHANNEL_MAP_NULL;

#ifdef CODEC_DSD
            reserv_map |= AUD_CHANNEL_MAP_CH2;
#endif

#ifdef ANC_APP
            hal_codec_anc_chan_config(&mic_map, &reserv_map, &codec_adc_ch_map);
#endif

            hal_codec_ec_chan_config(&mic_map, &reserv_map, &codec_adc_ch_map);
#ifdef SIDETONE_ENABLE
            hal_codec_side_tone_chan_config(&mic_map, &reserv_map, &codec_adc_ch_map);
#endif /* SIDETONE_ENABLED */

            hal_codec_adc_chan_map_unify(&mic_map, &reserv_map, codec_mic2_ch_map, codec_adc2_ch_map, codec_adc_ch_map);

            hal_codec_adc_chan_config(&mic_map, &reserv_map, &codec_adc_ch_map);
        }

        if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
            cfg_set_mask = 0;
            cfg_clr_mask = CODEC_MODE_16BIT_ADC | CODEC_MODE_24BIT_ADC | CODEC_MODE_32BIT_ADC;
            if (cfg->bits == AUD_BITS_16) {
                cfg_set_mask |= CODEC_MODE_16BIT_ADC;
            } else if (cfg->bits == AUD_BITS_24) {
                cfg_set_mask |= CODEC_MODE_24BIT_ADC;
            } else if (cfg->bits == AUD_BITS_32) {
                cfg_set_mask |= CODEC_MODE_32BIT_ADC;
            } else {
                ASSERT(false, "%s: Bad cap bits: %d", __func__, cfg->bits);
            }
            codec->REG_040 = (codec->REG_040 & ~cfg_clr_mask) | cfg_set_mask;
        }

        cnt = 0;
        enum AUD_CHANNEL_MAP_T full_map = codec_adc_ch_map | codec_adc2_ch_map;
        for (i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (full_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                cnt++;
            }
        }
        ASSERT(cnt >= cfg->channel_num, "%s: %d, bad cap stream chan cfg: mic_map=0x%X adc_map=0x%X reserv_map=%X, num=%d, cnt=%d", __func__, __LINE__,
            codec_mic_ch_map, codec_adc_ch_map, reserv_map, cfg->channel_num, cnt);

        if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
            sample_rate = cfg->sample_rate;

            for(i = 0; i < ARRAY_SIZE(codec_adc_sample_rate); i++) {
                if(codec_adc_sample_rate[i].sample_rate == sample_rate) {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_adc_sample_rate), "%s: Invalid capture sample rate: %d", __func__, sample_rate);
            rate_idx = i;
            ana_dig_div = codec_adc_sample_rate[rate_idx].codec_div / codec_adc_sample_rate[rate_idx].cmu_div;
            ASSERT(ana_dig_div * codec_adc_sample_rate[rate_idx].cmu_div == codec_adc_sample_rate[rate_idx].codec_div,
                "%s: Invalid catpure div for rate %u: codec_div=%u cmu_div=%u", __func__, sample_rate,
                codec_adc_sample_rate[rate_idx].codec_div, codec_adc_sample_rate[rate_idx].cmu_div);

            TRACE(2,"[%s] capture sample_rate=%d", __func__, sample_rate);

#ifdef CODEC_TIMER
            cur_codec_freq = codec_adc_sample_rate[rate_idx].codec_freq;
#endif

            codec_rate_idx[AUD_STREAM_CAPTURE] = rate_idx;

#ifdef __AUDIO_RESAMPLE__
            if (hal_cmu_get_audio_resample_status() && codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL) {
#ifdef CODEC_TIMER
                cur_codec_freq = CODEC_FREQ_CRYSTAL;
#endif
                if ((codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) == 0 ||
                        resample_rate_idx[AUD_STREAM_CAPTURE] != rate_idx) {
                    resample_rate_idx[AUD_STREAM_CAPTURE] = rate_idx;
                    codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                    hal_codec_reg_update_delay();
                    codec->REG_0F0 = resample_phase_float_to_value(get_capture_resample_phase());
                    hal_codec_reg_update_delay();
                    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                }

                codec->REG_0E4 = SET_BITFIELD(codec->REG_0E4, CODEC_CODEC_RESAMPLE_ADC_CH_CNT, cfg->channel_num - 1);
            }
#endif

            uint32_t val;
            uint8_t adc_chan;
            uint32_t ec_mic;

            val = 0;
            i = 0;
            mic_map = codec_mic_ch_map;
            TRACE(1, "codec_mic_ch_map=%x", codec_mic_ch_map);
            while (mic_map && i < MAX_ADC_CH_NUM) {
#if defined(AUDIO_OUTPUT_DC_CALIB) && defined(AUDIO_OUTPUT_DC_AUTO_CALIB)
                if(dac_dc_calib_status && analog_codec_get_dac_dev() == (MCU_CODEC_DAC_L | PMU_CODEC_DAC_L)) {
                    //change adc ch sel in order to conferm dac_calib logic
                    ch_idx = get_msb_pos(mic_map);
                } else
#endif
                {
                    ch_idx = get_lsb_pos(mic_map);
                }
                mic_map &= ~(1 << ch_idx);
                ec_mic = (1 << ch_idx) & EC_MIC_MAP;
                if (ec_mic == AUD_CHANNEL_MAP_ECMIC_CH0) {
                    adc_chan = 6;
                } else if (ec_mic == AUD_CHANNEL_MAP_ECMIC_CH1) {
                    adc_chan = 7;
                } else {
                    adc_chan = hal_codec_get_adc_chan(1 << ch_idx,
                        codec_mic_ch_map, codec_adc_ch_map, NORMAL_ADC_CH_NUM);

                    if (adc_chan >= NORMAL_ADC_CH_NUM) {
                        adc_chan = hal_codec_get_adc_chan(1<<ch_idx,
                            codec_mic2_ch_map, codec_adc2_ch_map, NORMAL_ADC_CH_NUM);

                        ASSERT(adc_chan < NORMAL_ADC_CH_NUM, "%s: Bad remap mic ch=%u mic_map=0x%X adc_map=0x%X",
                            __func__, ch_idx, codec_mic_ch_map, codec_adc_ch_map);
                    }
                }
                TRACE(1, "remap1 ADC chan[%d]->[%d]", adc_chan, i);
                val |= CODEC_CODEC_RESAMPLE_ADC_CH0_SEL(adc_chan) << (4 * i);
                if (codec_adc2_ch_map > 0) {
                    codec_adc_ch_map |= (1 << adc_chan); //set ADC1 chan map
                }
                i++;
            }
            ASSERT(mic_map == 0, "%s: Bad cap remap map: 0x%X", __func__, mic_map);
            codec->REG_0E8 = val;
            codec->REG_0E4 |= CODEC_CODEC_ADC_REMAP_ENABLE;

#ifdef __AUDIO_RESAMPLE__
            if (!hal_cmu_get_audio_resample_status())
#endif
            {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL,
                    "%s: capture sample rate %u is for resample only", __func__, sample_rate);
#endif
                analog_aud_freq_pll_config(codec_adc_sample_rate[rate_idx].codec_freq, codec_adc_sample_rate[rate_idx].codec_div);
                hal_codec_adc_set_div(codec_adc_sample_rate[rate_idx].cmu_div * CODEC_FREQ_EXTRA_DIV);
                hal_codec_ci2s_set_div(codec_adc_sample_rate[rate_idx].cmu_div);
            }

#ifdef PSAP_APP
            uint32_t ratio;
            uint32_t bypass;

            mask = CODEC_PSAP_ADC_RATE_SEL | CODEC_ADC_PSAP_MODE | CODEC_DOWN_SEL_PSAP_MASK;
            val = 0;

            ratio = codec_adc_sample_rate[codec_rate_idx[AUD_STREAM_CAPTURE]].adc_down;
            bypass = codec_adc_sample_rate[codec_rate_idx[AUD_STREAM_CAPTURE]].bypass_cnt;
            if ((ratio == 3 || ratio == 6) && bypass == 0) {
                // 16K or 8K
                val |= CODEC_DOWN_SEL_PSAP((ratio == 3) ? 0 : 1) | CODEC_ADC_PSAP_MODE;
            } else if (ratio == 1 && bypass == 0) {
                // 48K
#if 1
                val |= CODEC_DOWN_SEL_PSAP(2) | CODEC_ADC_PSAP_MODE;
#else
                val |= 0;
            } else if (ratio == 1 && bypass == 1) {
                // 96K
                val |= CODEC_PSAP_ADC_RATE_SEL;
#endif
            } else {
                ASSERT(false, "PSAP not support adc rate: %u", codec_adc_sample_rate[codec_rate_idx[AUD_STREAM_CAPTURE]].sample_rate);
            }
            codec->REG_400 = (codec->REG_400 & ~mask) | val;

            hal_codec_set_adc_down(codec_adc_ch_map & NORMAL_ADC_MAP, 1);
            hal_codec_set_adc_hbf_bypass_cnt(codec_adc_ch_map & NORMAL_ADC_MAP, 0);
            if (codec_adc_ch_map & EC_ADC_MAP) {
                hal_codec_set_adc_down(codec_adc_ch_map & EC_ADC_MAP, codec_adc_sample_rate[rate_idx].adc_down);
                hal_codec_set_adc_hbf_bypass_cnt(codec_adc_ch_map & EC_ADC_MAP, codec_adc_sample_rate[rate_idx].bypass_cnt);
            }
#else /* !defined(PSAP_APP) */
            hal_codec_set_adc_down(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].adc_down);
            hal_codec_set_adc_hbf_bypass_cnt(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].bypass_cnt);
#endif

#ifdef CODEC_ADC1_DSF
            hal_codec_adc1_dsf_setup_param(sample_rate);
#endif /* CODEC_ADC1_DSF */

        }

#if !(defined(FIXED_CODEC_ADC_VOL) && defined(SINGLE_CODEC_ADC_VOL))
        if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
#ifdef SINGLE_CODEC_ADC_VOL
            const CODEC_ADC_VOL_T *adc_gain_db;
            adc_gain_db = hal_codec_get_adc_volume(cfg->vol);
            if (adc_gain_db) {
                hal_codec_set_dig_adc_gain(NORMAL_ADC_MAP, *adc_gain_db);
#ifdef SIDETONE_DEDICATED_ADC_CHAN
                sidetone_adc_gain = *adc_gain_db;
                hal_codec_set_dig_adc_gain(sidetone_adc_ch_map, sidetone_adc_gain + sidetone_gain_offset);
#endif
            }
#else // !SINGLE_CODEC_ADC_VOL
            uint32_t vol;

            mic_map = codec_mic_ch_map;
            while (mic_map) {
                ch_idx = get_lsb_pos(mic_map);
                mic_map &= ~(1 << ch_idx);
                vol = hal_codec_get_mic_chan_volume_level(1 << ch_idx);
                hal_codec_set_chan_vol(AUD_STREAM_CAPTURE, (enum AUD_CHANNEL_MAP_T)(1 << ch_idx), vol);
            }
#ifdef SIDETONE_DEDICATED_ADC_CHAN
            if (codec_mic_ch_map & CFG_HW_AUD_SIDETONE_MIC_DEV) {
                const CODEC_ADC_VOL_T *adc_gain_db;

                vol = hal_codec_get_mic_chan_volume_level(CFG_HW_AUD_SIDETONE_MIC_DEV);
                adc_gain_db = hal_codec_get_adc_volume(vol);
                if (adc_gain_db) {
                    sidetone_adc_gain = *adc_gain_db;
                    hal_codec_set_dig_adc_gain(sidetone_adc_ch_map, sidetone_adc_gain + sidetone_gain_offset);
                }
            }
#endif
#endif // !SINGLE_CODEC_ADC_VOL
        }
#endif
    }

    return 0;
}

static void hal_codec_set_dac2_gain_ramp_interval(enum AUD_SAMPRATE_T rate)
{
#ifdef DAC_RAMP_GAIN
    int ramp_intvl = CODEC_DAC2_GAIN_RAMP_INTERVAL;

    if ((codec->REG_790 & CODEC_CODEC_RAMP_EN_SND_CH0) == 0) {
        return;
    }

    if (rate >= AUD_SAMPRATE_44100 * 8) {
        ramp_intvl += 3;
    } else if (rate >= AUD_SAMPRATE_44100 * 4) {
        ramp_intvl += 2;
    } else if (rate >= AUD_SAMPRATE_44100 * 2) {
        ramp_intvl += 1;
    } else if (rate >= AUD_SAMPRATE_44100) {
        ramp_intvl += 0;
    } else if (rate >= AUD_SAMPRATE_44100 / 2) {
        ramp_intvl -= 1;
    } else if (rate >= AUD_SAMPRATE_44100 / 4) {
        ramp_intvl -= 2;
    } else {
        ramp_intvl -= 3;
    }
    if (ramp_intvl < 0) {
        ramp_intvl = 0;
    } else if (ramp_intvl >= CODEC_DAC_GAIN_RAMP_INTVL_QTY) {
        ramp_intvl = CODEC_DAC_GAIN_RAMP_INTVL_QTY - 1;
    }
    codec->REG_790 = SET_BITFIELD(codec->REG_790, CODEC_CODEC_RAMP_INTERVAL_SND_CH0, ramp_intvl);
    codec->REG_794 = SET_BITFIELD(codec->REG_794, CODEC_CODEC_RAMP_INTERVAL_SND_CH1, ramp_intvl);
#endif
}

static void hal_codec_set_dac3_gain_ramp_interval(enum AUD_SAMPRATE_T rate)
{
#ifdef DAC_RAMP_GAIN
    int ramp_intvl = CODEC_DAC3_GAIN_RAMP_INTERVAL;

    if ((codec->REG_7A8 & CODEC_CODEC_RAMP_EN_TRI_CH0) == 0) {
        return;
    }

    if (rate >= AUD_SAMPRATE_44100 * 8) {
        ramp_intvl += 3;
    } else if (rate >= AUD_SAMPRATE_44100 * 4) {
        ramp_intvl += 2;
    } else if (rate >= AUD_SAMPRATE_44100 * 2) {
        ramp_intvl += 1;
    } else if (rate >= AUD_SAMPRATE_44100) {
        ramp_intvl += 0;
    } else if (rate >= AUD_SAMPRATE_44100 / 2) {
        ramp_intvl -= 1;
    } else if (rate >= AUD_SAMPRATE_44100 / 4) {
        ramp_intvl -= 2;
    } else {
        ramp_intvl -= 3;
    }
    if (ramp_intvl < 0) {
        ramp_intvl = 0;
    } else if (ramp_intvl >= CODEC_DAC_GAIN_RAMP_INTVL_QTY) {
        ramp_intvl = CODEC_DAC_GAIN_RAMP_INTVL_QTY - 1;
    }
    codec->REG_7A8 = SET_BITFIELD(codec->REG_7A8, CODEC_CODEC_RAMP_INTERVAL_TRI_CH0, ramp_intvl);
    codec->REG_7AC = SET_BITFIELD(codec->REG_7AC, CODEC_CODEC_RAMP_INTERVAL_TRI_CH1, ramp_intvl);
#endif
}

int hal_codec_dac2_setup_stream(enum HAL_CODEC_ID_T id, const struct HAL_CODEC_CONFIG_T *cfg)
{
    int i;
    int rate_idx;
    uint32_t ana_dig_div;
    enum AUD_SAMPRATE_T sample_rate;
    POSSIBLY_UNUSED uint32_t mask, val;

    TRACE(1, "%s:id=%d,cfg->set_flag=%x", __func__, id, cfg->set_flag);

    if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
        if (cfg->channel_num == AUD_CHANNEL_NUM_2) {
            if (cfg->channel_map != (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
                TRACE(2,"\n!!! WARNING:%s: Bad play stereo ch map: 0x%X\n", __func__, cfg->channel_map);
            }
            codec->REG_744 |= CODEC_DUAL_CHANNEL_DAC_SND;
        } else {
            ASSERT(cfg->channel_num == AUD_CHANNEL_NUM_1, "%s: Bad play ch num: %u", __func__, cfg->channel_num);
            // Allow to DMA one channel but output 2 channels
            ASSERT((cfg->channel_map & ~(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) == 0,
                "%s: Bad play mono ch map: 0x%X", __func__, cfg->channel_map);
            codec->REG_744 &= ~CODEC_DUAL_CHANNEL_DAC_SND;
        }
        codec_dac2_ch_map = AUD_CHANNEL_MAP_NULL;
        if (cfg->channel_map & AUD_CHANNEL_MAP_CH0) {
            codec_dac2_ch_map |= AUD_CHANNEL_MAP_CH0;
        }
        if (cfg->channel_map & AUD_CHANNEL_MAP_CH1) {
            codec_dac2_ch_map |= AUD_CHANNEL_MAP_CH1;
        }
    }

    if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
        if (cfg->bits == AUD_BITS_16) {
            codec->REG_744 = (codec->REG_744 & ~CODEC_MODE_32BIT_DAC_SND) | CODEC_MODE_16BIT_DAC_SND;
        } else if (cfg->bits == AUD_BITS_24) {
            codec->REG_744 &= ~(CODEC_MODE_16BIT_DAC_SND | CODEC_MODE_32BIT_DAC_SND);
        } else if (cfg->bits == AUD_BITS_32) {
            codec->REG_744 = (codec->REG_744 & ~CODEC_MODE_16BIT_DAC_SND) | CODEC_MODE_32BIT_DAC_SND;
        } else {
            ASSERT(false, "%s: Bad play bits: %u", __func__, cfg->bits);
        }
        TRACE(1, "update sampbits: %d", cfg->bits);
    }

    if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
        sample_rate = cfg->sample_rate;
        TRACE(1, "update samprate: %d", cfg->sample_rate);
        for (i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++) {
            if (codec_dac_sample_rate[i].sample_rate == sample_rate) {
                break;
            }
        }
        ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %u", __func__, sample_rate);
        rate_idx = i;
        ana_dig_div = codec_dac_sample_rate[rate_idx].codec_div / codec_dac_sample_rate[rate_idx].cmu_div;
        ASSERT(ana_dig_div * codec_dac_sample_rate[rate_idx].cmu_div == codec_dac_sample_rate[rate_idx].codec_div,
            "%s: Invalid playback div for rate %u: codec_div=%u cmu_div=%u", __func__, sample_rate,
            codec_dac_sample_rate[rate_idx].codec_div, codec_dac_sample_rate[rate_idx].cmu_div);

        TRACE(2,"[%s] playback sample_rate=%d", __func__, sample_rate);

        codec_dac2_rate_idx = rate_idx;

#ifdef __AUDIO_RESAMPLE__
        if (hal_cmu_get_audio_resample_status() && codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL) {
            if ((codec->REG_7C0 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND) == 0 ||
                    dac2_resample_rate_idx != rate_idx) {
                dac2_resample_rate_idx = rate_idx;
                codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
                hal_codec_reg_update_delay();
                codec->REG_7C8 = resample_phase_float_to_value(get_playback2_resample_phase());
                hal_codec_reg_update_delay();
                codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
            }

            mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_SND | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_SND;
            val = 0;
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH0) {
                val |= CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_SND;
            }
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH1) {
                val |= CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_SND;
            }
        } else {
            mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_SND
                | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_SND
                | CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
            val = 0;
        }
        codec->REG_7C0 = (codec->REG_7C0 & ~mask) | val;
#endif

#ifdef __AUDIO_RESAMPLE__
        if (!hal_cmu_get_audio_resample_status())
#endif
        {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL,
                    "%s: playback sample rate %u is for resample only", __func__, sample_rate);
#endif
            analog_aud_freq_pll_config(codec_dac_sample_rate[rate_idx].codec_freq, codec_dac_sample_rate[rate_idx].codec_div);
            hal_codec_dac_set_div(codec_dac_sample_rate[rate_idx].cmu_div * CODEC_FREQ_EXTRA_DIV);
            hal_codec_ci2s_set_div(codec_dac_sample_rate[rate_idx].cmu_div);
        }

        hal_codec_set_dac2_gain_ramp_interval(cfg->sample_rate);
        hal_codec_set_dac2_up(codec_dac_sample_rate[rate_idx].dac_up);
        hal_codec_set_dac2_hbf_bypass_cnt(codec_dac_sample_rate[rate_idx].bypass_cnt);

#if defined(CODEC_DAC2_USF)
        hal_codec_dac_usf_setup_stream(HAL_CODEC_USF_DAC2, cfg->sample_rate);
#endif

    }


    if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
        const struct CODEC_DAC_VOL_T *vol_tab_ptr;

        vol_tab_ptr = hal_codec_get_dac_volume(cfg->vol);
        if (vol_tab_ptr) {
#ifdef AUDIO_OUTPUT_DAC2_SW_GAIN
            hal_codec_set_dac2_sw_gain(vol_tab_ptr->sdac_volume);
#else
            analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
            hal_codec_set_dig_dac2_gain((enum AUD_CHANNEL_MAP_T)VALID_DAC2_MAP, vol_tab_ptr->sdac_volume);
            TRACE(1, "update vol: %d, sdac_volume: %d", cfg->vol, vol_tab_ptr->sdac_volume);
#endif
        }
    }
    return 0;
}

int hal_codec_dac3_setup_stream(enum HAL_CODEC_ID_T id, const struct HAL_CODEC_CONFIG_T *cfg)
{
    int i;
    int rate_idx;
    uint32_t ana_dig_div;
    enum AUD_SAMPRATE_T sample_rate;
    POSSIBLY_UNUSED uint32_t mask, val;

    TRACE(1, "%s:id=%d,cfg->set_flag=%x", __func__, id, cfg->set_flag);

    if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
        if (cfg->channel_num == AUD_CHANNEL_NUM_2) {
            if (cfg->channel_map != (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
                TRACE(2,"\n!!! WARNING:%s: Bad play stereo ch map: 0x%X\n", __func__, cfg->channel_map);
            }
            codec->REG_744 |= CODEC_DUAL_CHANNEL_DAC_TRI;
        } else {
            ASSERT(cfg->channel_num == AUD_CHANNEL_NUM_1, "%s: Bad play ch num: %u", __func__, cfg->channel_num);
            // Allow to DMA one channel but output 2 channels
            ASSERT((cfg->channel_map & ~(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) == 0,
                "%s: Bad play mono ch map: 0x%X", __func__, cfg->channel_map);
            codec->REG_744 &= ~CODEC_DUAL_CHANNEL_DAC_TRI;
        }
        codec_dac3_ch_map = 0;
        if (cfg->channel_map & AUD_CHANNEL_MAP_CH0) {
            codec_dac3_ch_map |= AUD_CHANNEL_MAP_CH0;
        }
        if (cfg->channel_map & AUD_CHANNEL_MAP_CH1) {
            codec_dac3_ch_map |= AUD_CHANNEL_MAP_CH1;
        }
    }

    if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
        if (cfg->bits == AUD_BITS_16) {
            codec->REG_744 = (codec->REG_744 & ~CODEC_MODE_32BIT_DAC_TRI) | CODEC_MODE_16BIT_DAC_TRI;
        } else if (cfg->bits == AUD_BITS_24) {
            codec->REG_744 &= ~(CODEC_MODE_16BIT_DAC_TRI | CODEC_MODE_32BIT_DAC_TRI);
        } else if (cfg->bits == AUD_BITS_32) {
            codec->REG_744 = (codec->REG_744 & ~CODEC_MODE_16BIT_DAC_TRI) | CODEC_MODE_32BIT_DAC_TRI;
        } else {
            ASSERT(false, "%s: Bad play bits: %u", __func__, cfg->bits);
        }
        TRACE(1, "update sampbits: %d", cfg->bits);
    }

    if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
        sample_rate = cfg->sample_rate;
        TRACE(1, "update samprate: %d", cfg->sample_rate);
        for (i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++) {
            if (codec_dac_sample_rate[i].sample_rate == sample_rate) {
                break;
            }
        }
        ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %u", __func__, sample_rate);
        rate_idx = i;
        ana_dig_div = codec_dac_sample_rate[rate_idx].codec_div / codec_dac_sample_rate[rate_idx].cmu_div;
        ASSERT(ana_dig_div * codec_dac_sample_rate[rate_idx].cmu_div == codec_dac_sample_rate[rate_idx].codec_div,
            "%s: Invalid playback div for rate %u: codec_div=%u cmu_div=%u", __func__, sample_rate,
            codec_dac_sample_rate[rate_idx].codec_div, codec_dac_sample_rate[rate_idx].cmu_div);

        TRACE(2,"[%s] playback sample_rate=%d", __func__, sample_rate);

        codec_dac3_rate_idx = rate_idx;

#ifdef __AUDIO_RESAMPLE__
        if (hal_cmu_get_audio_resample_status() && codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL) {
            if ((codec->REG_7C0 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI) == 0 ||
                    dac3_resample_rate_idx != rate_idx) {
                dac3_resample_rate_idx = rate_idx;
                codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
                hal_codec_reg_update_delay();
                codec->REG_7CC = resample_phase_float_to_value(get_playback3_resample_phase());
                hal_codec_reg_update_delay();
                codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
            }

            mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_TRI | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_TRI;
            val = 0;
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH0) {
                val |= CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_TRI;
            }
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH1) {
                val |= CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_TRI;
            }
        } else {
            mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_TRI
                | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_TRI
                | CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
            val = 0;
        }
        codec->REG_7C0 = (codec->REG_7C0 & ~mask) | val;
#endif

#ifdef __AUDIO_RESAMPLE__
        if (!hal_cmu_get_audio_resample_status())
#endif
        {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL,
                    "%s: playback sample rate %u is for resample only", __func__, sample_rate);
#endif
            analog_aud_freq_pll_config(codec_dac_sample_rate[rate_idx].codec_freq, codec_dac_sample_rate[rate_idx].codec_div);
            hal_codec_dac_set_div(codec_dac_sample_rate[rate_idx].cmu_div * CODEC_FREQ_EXTRA_DIV);
            hal_codec_ci2s_set_div(codec_dac_sample_rate[rate_idx].cmu_div);
        }

        hal_codec_set_dac3_gain_ramp_interval(cfg->sample_rate);
        hal_codec_set_dac3_up(codec_dac_sample_rate[rate_idx].dac_up);
        hal_codec_set_dac3_hbf_bypass_cnt(codec_dac_sample_rate[rate_idx].bypass_cnt);

#if defined(CODEC_DAC3_USF)
        hal_codec_dac_usf_setup_stream(HAL_CODEC_USF_DAC3, cfg->sample_rate);
#endif

    }

    if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
        const struct CODEC_DAC_VOL_T *vol_tab_ptr;

        vol_tab_ptr = hal_codec_get_dac_volume(cfg->vol);
        if (vol_tab_ptr) {
#ifdef AUDIO_OUTPUT_DAC3_SW_GAIN
            hal_codec_set_dac3_sw_gain(vol_tab_ptr->sdac_volume);
#else
            analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
            hal_codec_set_dig_dac3_gain((enum AUD_CHANNEL_MAP_T)VALID_DAC3_MAP, vol_tab_ptr->sdac_volume);
            TRACE(1, "update vol: %d, sdac_volume: %d", cfg->vol, vol_tab_ptr->sdac_volume);
#endif
        }
    }
    return 0;
}

void hal_codec_anc_adc_data_dump_enable(enum ANC_TYPE_T type, enum AUD_CHANNEL_MAP_T chan)
{
#ifdef ANC_APP
    if (type & ANC_FEEDFORWARD) {
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
        if (ANC_FF_MIC_CH_L) {
            if (chan == ANC_FF_MIC_CH_L) {
                // ANC_FF_MIC_CH_L(FF CH0) only can be dumped from ADC CH1
                codec->REG_0DC |= CODEC_CODEC_FF_DBG_KEEP_CH0;
            }
        }
        if (ANC_FF_MIC_CH_R) {
            if (chan == ANC_FF_MIC_CH_R) {
                // ANC_FF_MIC_CH_R(FF CH1) only can be dumped from ADC CH0
                codec->REG_0DC |= CODEC_CODEC_FF_DBG_KEEP_CH1;
            }
        }
#else
        ASSERT(false, "No anc ff ch defined");
#endif
    }
    if (type & ANC_FEEDBACK) {
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
        if (ANC_FB_MIC_CH_L) {
            if (chan == ANC_FB_MIC_CH_L) {
                // ANC_FB_MIC_CH_L(FB CH0) only can be dumped from ADC CH3
                codec->REG_0DC |= CODEC_CODEC_FB_DBG_KEEP_CH0;
            }
        }
        if (ANC_FB_MIC_CH_R) {
            if (chan == ANC_FB_MIC_CH_R) {
                // ANC_FB_MIC_CH_R(FB CH1) only can be dumped from ADC CH2
                codec->REG_0DC |= CODEC_CODEC_FB_DBG_KEEP_CH1;
            }
        }
#else
        ASSERT(false, "No anc fb ch defined");
#endif
    }

    if (type & ANC_TALKTHRU) {
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
        if (ANC_TT_MIC_CH_L) {
            if (chan == ANC_TT_MIC_CH_L) {
                // ANC_FB_MIC_CH_L(TT CH0) only can be dumped from ADC CH5
                codec->REG_0DC |= CODEC_CODEC_TT_DBG_KEEP_CH0;
            }
        }
        if (ANC_TT_MIC_CH_R) {
            if (chan == ANC_TT_MIC_CH_R) {
                // ANC_FB_MIC_CH_R(TT CH1) only can be dumped from ADC CH4
                codec->REG_0DC |= CODEC_CODEC_TT_DBG_KEEP_CH1;
            }
        }
#else
        ASSERT(false, "No anc tt ch defined");
#endif
    }
#endif /* ANC_APP */
}

void hal_codec_anc_adc_data_dump_disable(enum ANC_TYPE_T type, enum AUD_CHANNEL_MAP_T chan)
{
#ifdef ANC_APP
    if (type & ANC_FEEDFORWARD) {
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
        if (ANC_FF_MIC_CH_L) {
            if (chan == ANC_FF_MIC_CH_L) {
                // ANC_FF_MIC_CH_L(FF CH0) only can be dumped from ADC CH1
                codec->REG_0DC &= ~CODEC_CODEC_FF_DBG_KEEP_CH0;
            }
        }
        if (ANC_FF_MIC_CH_R) {
            if (chan == ANC_FF_MIC_CH_R) {
                // ANC_FF_MIC_CH_R(FF CH1) only can be dumped from ADC CH0
                codec->REG_0DC &= ~CODEC_CODEC_FF_DBG_KEEP_CH1;
            }
        }
#else
        ASSERT(false, "No anc ff ch defined");
#endif
    }
    if (type & ANC_FEEDBACK) {
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
        if (ANC_FB_MIC_CH_L) {
            if (chan == ANC_FB_MIC_CH_L) {
                // ANC_FB_MIC_CH_L(FB CH0) only can be dumped from ADC CH3
                codec->REG_0DC &= ~CODEC_CODEC_FB_DBG_KEEP_CH0;
            }
        }
        if (ANC_FB_MIC_CH_R) {
            if (chan == ANC_FB_MIC_CH_R) {
                // ANC_FB_MIC_CH_R(FB CH1) only can be dumped from ADC CH2
                codec->REG_0DC &= ~CODEC_CODEC_FB_DBG_KEEP_CH1;
            }
        }
#else
        ASSERT(false, "No anc fb ch defined");
#endif
    }

    if (type & ANC_TALKTHRU) {
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
        if (ANC_TT_MIC_CH_L) {
            if (chan == ANC_TT_MIC_CH_L) {
                // ANC_FB_MIC_CH_L(TT CH0) only can be dumped from ADC CH5
                codec->REG_0DC &= ~CODEC_CODEC_TT_DBG_KEEP_CH0;
            }
        }
        if (ANC_TT_MIC_CH_R) {
            if (chan == ANC_TT_MIC_CH_R) {
                // ANC_FB_MIC_CH_R(TT CH1) only can be dumped from ADC CH4
                codec->REG_0DC &= ~CODEC_CODEC_TT_DBG_KEEP_CH1;
            }
        }
#else
        ASSERT(false, "No anc tt ch defined");
#endif
    }
#endif /* ANC_APP */
}


int hal_codec_anc_adc_enable(enum ANC_TYPE_T type)
{
#ifdef ANC_APP
    enum AUD_CHANNEL_MAP_T map;
    enum AUD_CHANNEL_MAP_T mic_map;
    uint8_t ch_idx;

    map = 0;
    mic_map = 0;
    if (type & ANC_FEEDFORWARD) {
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
        if (ANC_FF_MIC_CH_L) {
            ch_idx = get_msb_pos(ANC_FF_MIC_CH_L);
            if (ANC_FF_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
                codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH0, ch_idx);
                codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0;
            } else {
                if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH0, ch_idx);
                }
                codec->REG_084 = SET_BITFIELD(codec->REG_084, CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
                codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH0;
            }
            map |= AUD_CHANNEL_MAP_CH0;
            mic_map |= ANC_FF_MIC_CH_L;
        }
        if (ANC_FF_MIC_CH_R) {
            ch_idx = get_msb_pos(ANC_FF_MIC_CH_R);
            if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
                codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH1, ch_idx);
                codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH1;
            } else {
                if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH1, ch_idx);
                }
                codec->REG_088 = SET_BITFIELD(codec->REG_088, CODEC_CODEC_ADC_IN_SEL_CH1, ch_idx);
                codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH1;
            }
            map |= AUD_CHANNEL_MAP_CH1;
            mic_map |= ANC_FF_MIC_CH_R;
        }
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
        if (ANC_FF_MIC_CH_L && (ANC_FF_MIC_CH_L == ANC_TT_MIC_CH_L)) {
            anc_tt_mic_l_map |= ANC_FEEDFORWARD;
        }
        if (ANC_FF_MIC_CH_R && (ANC_FF_MIC_CH_R == ANC_TT_MIC_CH_R)) {
            anc_tt_mic_r_map |= ANC_FEEDFORWARD;
        }
#endif
#else
        ASSERT(false, "No ana adc ff ch defined");
#endif
    }
    if (type & ANC_FEEDBACK) {
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
        if (ANC_FB_MIC_CH_L) {
            ch_idx = get_msb_pos(ANC_FB_MIC_CH_L);
            if (ANC_FB_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
                codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH2, ch_idx);
                codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH2;
            } else {
                if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH2, ch_idx);
                }
                codec->REG_08C = SET_BITFIELD(codec->REG_08C, CODEC_CODEC_ADC_IN_SEL_CH2, ch_idx);
                codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH2;
            }
            map |= AUD_CHANNEL_MAP_CH2;
            mic_map |= ANC_FB_MIC_CH_L;
        }
        if (ANC_FB_MIC_CH_R) {
            ch_idx = get_msb_pos(ANC_FB_MIC_CH_R);
            if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
                codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_PDM_MUX_CH3, ch_idx);
                codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH3;
            } else {
                if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                    codec->REG_7F0 = SET_BITFIELD(codec->REG_7F0, CODEC_CI2S_ADC_IN_SEL_CH3, ch_idx);
                }
                codec->REG_090 = SET_BITFIELD(codec->REG_090, CODEC_CODEC_ADC_IN_SEL_CH3, ch_idx);
                codec->REG_0C4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH3;
            }
            map |= AUD_CHANNEL_MAP_CH3;
            mic_map |= ANC_FB_MIC_CH_R;
        }
#else
        ASSERT(false, "No ana adc fb ch defined");
#endif
    }

#if defined( AUDIO_ANC_TT_HW) || defined(PSAP_APP)
    if (type & ANC_TALKTHRU) {
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
        int i;

        if (ANC_TT_MIC_CH_L) {
            ch_idx = get_msb_pos(ANC_TT_MIC_CH_L);
            i = get_msb_pos(anc_tt_adc_ch_l);
            if (ANC_TT_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                ch_idx -= get_msb_pos(AUD_CHANNEL_MAP_DIGMIC_CH0);
                codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) |
                    (CODEC_CODEC_PDM_MUX_CH0(ch_idx) << (3 * i));
                codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << i;
            } else {
                if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                    uint32_t pos = 3 * i;
                    // ci2s adc ch sel
                    codec->REG_7F0 = (codec->REG_7F0 & ~(CODEC_CI2S_ADC_IN_SEL_CH0_MASK << pos))
                                        | (CODEC_CI2S_ADC_IN_SEL_CH0(ch_idx) << pos);
                }
                *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
                codec->REG_0C4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << i);
            }
            codec->REG_22C = SET_BITFIELD(codec->REG_22C, CODEC_CODEC_TT_ADC_SEL_CH0, i);
#ifdef PSAP_APP
            hal_codec_psap_setup(AUD_CHANNEL_MAP_CH0, &psap_cfg);
#endif
            map |= anc_tt_adc_ch_l;
            mic_map |= ANC_TT_MIC_CH_L;
        }
        if (ANC_TT_MIC_CH_R) {
            ch_idx = get_msb_pos(ANC_TT_MIC_CH_R);
            i = get_msb_pos(anc_tt_adc_ch_r);
            if (ANC_TT_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                ch_idx -= get_msb_pos(AUD_CHANNEL_MAP_DIGMIC_CH0);
                codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) |
                    (CODEC_CODEC_PDM_MUX_CH0(ch_idx) << (3 * i));
                codec->REG_0C4 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << i;
            } else {
                if (pmu_get_chip_type() == PMU_CHIP_TYPE_1809) {
                    uint32_t pos = 3 * i;
                    // ci2s adc ch sel
                    codec->REG_7F0 = (codec->REG_7F0 & ~(CODEC_CI2S_ADC_IN_SEL_CH0_MASK << pos))
                                        | (CODEC_CI2S_ADC_IN_SEL_CH0(ch_idx) << pos);
                }
                *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
                codec->REG_0C4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << i);
            }
            codec->REG_22C = SET_BITFIELD(codec->REG_22C, CODEC_CODEC_TT_ADC_SEL_CH1, i);
#ifdef PSAP_APP
            hal_codec_psap_setup(AUD_CHANNEL_MAP_CH0, &psap_cfg);
#endif
            map |= anc_tt_adc_ch_r;
            mic_map |= ANC_TT_MIC_CH_R;
        }

#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
        if (ANC_TT_MIC_CH_L && (ANC_TT_MIC_CH_L == ANC_FF_MIC_CH_L)) {
            anc_tt_mic_l_map |= ANC_TALKTHRU;
        }
        if (ANC_TT_MIC_CH_R && (ANC_TT_MIC_CH_R == ANC_FF_MIC_CH_R)) {
            anc_tt_mic_r_map |= ANC_TALKTHRU;
        }
#endif
#endif
    }
#endif

#ifdef ANC_DYN_CODEC_VOLT
    if (anc_adc_ch_map == 0) {
        hal_codec_codec_volt_ramp_up(CODEC_VOLT_RAMP_USER_ANC);
    }
#endif

    anc_adc_ch_map |= map;
    anc_mic_ch_map |= mic_map;

    // enable ADC low delay for ANC
#ifdef AUDIO_ANC_LOW_DELAY
    hal_codec_adc_low_delay(map, true);
#endif

#ifdef ANC_FF_MIC_DUMP_CH_L
    hal_codec_anc_adc_data_dump_enable(ANC_FEEDFORWARD, ANC_FF_MIC_CH_L);
#endif
#ifdef ANC_FF_MIC_DUMP_CH_R
    hal_codec_anc_adc_data_dump_enable(ANC_FEEDFORWARD, ANC_FF_MIC_CH_R);
#endif
#ifdef ANC_FB_MIC_DUMP_CH_L
    hal_codec_anc_adc_data_dump_enable(ANC_FEEDBACK, ANC_FB_MIC_CH_L);
#endif
#ifdef ANC_FB_MIC_DUMP_CH_R
    hal_codec_anc_adc_data_dump_enable(ANC_FEEDBACK, ANC_FB_MIC_CH_R);
#endif
#ifdef ANC_TT_MIC_DUMP_CH_L
    hal_codec_anc_adc_data_dump_enable(ANC_TALKTHRU, ANC_TT_MIC_CH_L);
#endif
#ifdef ANC_TT_MIC_DUMP_CH_R
    hal_codec_anc_adc_data_dump_enable(ANC_TALKTHRU, ANC_TT_MIC_CH_R);
#endif

    if (anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
        hal_codec_enable_dig_mic(DIG_MIC_USER_ANC, anc_mic_ch_map);
    }

    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if ((codec->REG_780 & (CODEC_CODEC_ADC_SND_EN_CH0 << i)) == 0) {
                codec->REG_0AC |= (CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << i);
                hal_codec_reg_update_delay();
                codec->REG_0AC &= ~(CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << i);
            }
            codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << i);
        }
    }
#endif

    return 0;
}

int hal_codec_anc_adc_disable(enum ANC_TYPE_T type)
{
#ifdef ANC_APP
    enum AUD_CHANNEL_MAP_T map;
    enum AUD_CHANNEL_MAP_T mic_map;

    map = 0;
    mic_map = 0;
    if (type & ANC_FEEDFORWARD) {
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
        if (ANC_FF_MIC_CH_L) {
#if defined(ANC_TT_MIC_CH_L)
            if (ANC_FF_MIC_CH_L && (ANC_FF_MIC_CH_L == ANC_TT_MIC_CH_L)) {
                anc_tt_mic_l_map &= ~ANC_FEEDFORWARD;
            }
            if (anc_tt_mic_l_map == 0)
#endif
            {
                map |= AUD_CHANNEL_MAP_CH0;
                mic_map |= ANC_FF_MIC_CH_L;
            }
        }
        if (ANC_FF_MIC_CH_R) {
#if defined(ANC_TT_MIC_CH_R)
            if (ANC_FF_MIC_CH_R && (ANC_FF_MIC_CH_R == ANC_TT_MIC_CH_R)) {
                anc_tt_mic_r_map &= ~ANC_FEEDFORWARD;
            }
            if (anc_tt_mic_r_map == 0)
#endif
            {
                map |= AUD_CHANNEL_MAP_CH1;
                mic_map |= ANC_FF_MIC_CH_R;
            }
        }
#endif
    }
    if (type & ANC_FEEDBACK) {
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
        if (ANC_FB_MIC_CH_L) {
            map |= AUD_CHANNEL_MAP_CH2;
            mic_map |= ANC_FB_MIC_CH_L;
        }
        if (ANC_FB_MIC_CH_R) {
            map |= AUD_CHANNEL_MAP_CH3;
            mic_map |= ANC_FB_MIC_CH_R;
        }
#endif
    }

#if defined(AUDIO_ANC_TT_HW) || defined(PSAP_APP)
    if (type & ANC_TALKTHRU) {
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
        if (ANC_TT_MIC_CH_L) {
#if defined(ANC_FF_MIC_CH_L)
            if (ANC_TT_MIC_CH_L && (ANC_TT_MIC_CH_L == ANC_FF_MIC_CH_L)) {
                anc_tt_mic_l_map &= ~ANC_TALKTHRU;
            }
            if (anc_tt_mic_l_map == 0)
#endif
            {
                map |= anc_tt_adc_ch_l;
                mic_map |= ANC_TT_MIC_CH_L;
            }
        }
        if (ANC_TT_MIC_CH_R) {
#if defined(ANC_FF_MIC_CH_R)
            if (ANC_TT_MIC_CH_R && (ANC_TT_MIC_CH_R == ANC_FF_MIC_CH_R)) {
                anc_tt_mic_r_map &= ~ANC_TALKTHRU;
            }
            if (anc_tt_mic_r_map == 0)
#endif
            {
                map |= anc_tt_adc_ch_r;
                mic_map |= ANC_TT_MIC_CH_R;
            }
        }
#endif /* defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R) */
    }
#endif

    anc_adc_ch_map &= ~map;
    anc_mic_ch_map &= ~mic_map;

#ifdef ANC_DYN_CODEC_VOLT
    if (anc_adc_ch_map == 0) {
        hal_codec_codec_volt_ramp_down(CODEC_VOLT_RAMP_USER_ANC);
    }
#endif

    if ((anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) == 0 &&
            ((codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) == 0 || (codec->REG_000 & CODEC_ADC_ENABLE) == 0)) {
        hal_codec_disable_dig_mic(DIG_MIC_USER_ANC);
    }

    for (int i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if ((map & (AUD_CHANNEL_MAP_CH0 << i)) == 0) {
            continue;
        }
        if (codec->REG_000 & CODEC_ADC_ENABLE) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                continue;
            }
        }
        codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << i);
    }

    // enable ADC low delay for ANC
#ifdef AUDIO_ANC_LOW_DELAY
    hal_codec_adc_low_delay(map, false);
#endif

#ifdef ANC_FF_MIC_DUMP_CH_L
    hal_codec_anc_adc_data_dump_disable(ANC_FEEDFORWARD, ANC_FF_MIC_CH_L);
#endif
#ifdef ANC_FF_MIC_DUMP_CH_R
    hal_codec_anc_adc_data_dump_disable(ANC_FEEDFORWARD, ANC_FF_MIC_CH_R);
#endif
#ifdef ANC_FB_MIC_DUMP_CH_L
    hal_codec_anc_adc_data_dump_disable(ANC_FEEDBACK, ANC_FB_MIC_CH_L);
#endif
#ifdef ANC_FB_MIC_DUMP_CH_R
    hal_codec_anc_adc_data_dump_disable(ANC_FEEDBACK, ANC_FB_MIC_CH_R);
#endif
#ifdef ANC_TT_MIC_DUMP_CH_L
    hal_codec_anc_adc_data_dump_disable(ANC_TALKTHRU, ANC_TT_MIC_CH_L);
#endif
#ifdef ANC_TT_MIC_DUMP_CH_R
    hal_codec_anc_adc_data_dump_disable(ANC_TALKTHRU, ANC_TT_MIC_CH_R);
#endif

#endif

    return 0;
}

enum AUD_SAMPRATE_T hal_codec_anc_convert_rate(enum AUD_SAMPRATE_T rate)
{
    if (hal_cmu_get_audio_resample_status()) {
        return AUD_SAMPRATE_50781;
    } else if (CODEC_FREQ_48K_SERIES / rate * rate == CODEC_FREQ_48K_SERIES) {
        return AUD_SAMPRATE_48000;
    } else /* if (CODEC_FREQ_44_1K_SERIES / rate * rate == CODEC_FREQ_44_1K_SERIES) */ {
        return AUD_SAMPRATE_44100;
    }
}

int hal_codec_anc_dma_enable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

int hal_codec_anc_dma_disable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

int hal_codec_aux_mic_dma_enable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

int hal_codec_aux_mic_dma_disable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

uint32_t hal_codec_get_alg_dac_shift(void)
{
    return 0;
}

#ifdef ANC_APP
void hal_codec_set_anc_boost_gain_attn(float attn)
{
    anc_boost_gain_attn = attn;

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(swdac_gain);
#else
    hal_codec_restore_dig_dac_gain();
#endif
    hal_codec_restore_dig_adc_gain();
}

void hal_codec_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int8_t offset_l, int8_t offset_r)
{
    enum AUD_CHANNEL_MAP_T map_l;
    enum AUD_CHANNEL_MAP_T ch_map;
    uint8_t ch_idx;

    if (analog_debug_get_anc_calib_mode()) {
        return;
    }

    map_l = 0;

#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
    if (type & ANC_FEEDFORWARD) {
        if (ANC_FF_MIC_CH_L) {
            map_l |= AUD_CHANNEL_MAP_CH0;
        }
        if (ANC_FF_MIC_CH_R) {
            map_l |= AUD_CHANNEL_MAP_CH1;
        }
    }
#endif
#if defined(ANC_FB_MIC_CH_L) || defined(ANC_FB_MIC_CH_R)
    if (type & ANC_FEEDBACK) {
        if (ANC_FB_MIC_CH_L) {
            map_l |= AUD_CHANNEL_MAP_CH2;
        }
        if (ANC_FB_MIC_CH_R) {
            map_l |= AUD_CHANNEL_MAP_CH3;
        }
    }
#endif
#if defined(ANC_TT_MIC_CH_L) || defined(ANC_TT_MIC_CH_R)
    if (type & ANC_TALKTHRU) {
        if (ANC_FB_MIC_CH_L) {
            map_l |= anc_tt_adc_ch_l;
        }
        if (ANC_FB_MIC_CH_R) {
            map_l |= anc_tt_adc_ch_r;
        }
    }
#endif

    if (map_l) {
        ch_map = map_l;
        while (ch_map) {
            ch_idx = get_msb_pos(ch_map);
            ch_map &= ~(1 << ch_idx);
            anc_adc_gain_offset[ch_idx] = offset_l;
        }
        if (offset_l) {
            anc_adc_gain_offset_map |= map_l;
        } else {
            anc_adc_gain_offset_map &= ~map_l;
        }
    }
    if (map_l) {
        hal_codec_restore_dig_adc_gain();
    }
}
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
void hal_codec_set_dac_dc_gain_attn(float attn)
{
    dac_dc_gain_attn = attn;
}

void hal_codec_set_dac_dc_offset(int16_t dc_l, int16_t dc_r)
{
    // DC calib values are based on 16-bit, but hardware compensation is based on 24-bit
    dac_dc_l = dc_l << 8;
    dac_dc_r = dc_r << 8;
#ifdef SDM_MUTE_NOISE_SUPPRESSION
    if (dac_dc_l == 0) {
        dac_dc_l = 1;
    }
    if (dac_dc_r == 0) {
        dac_dc_r = 1;
    }
#endif
}
#endif

void hal_codec_set_dac_reset_callback(HAL_CODEC_DAC_RESET_CALLBACK callback)
{
    //dac_reset_callback = callback;
}

POSSIBLY_UNUSED static uint32_t hal_codec_get_adc_chan(enum AUD_CHANNEL_MAP_T mic_map,
    enum AUD_CHANNEL_MAP_T mic_chan_map, enum AUD_CHANNEL_MAP_T adc_chan_map, uint8_t adc_chan_num)
{
    uint8_t adc_ch;
    uint8_t mic_ch;
    uint8_t digmic_ch0;
    uint8_t en_ch;
    bool digmic;
    int i;

    adc_ch = MAX_ADC2_CH_NUM;

    // checking mic validation
    mic_ch = get_lsb_pos(mic_map);
    if (((1 << mic_ch) & mic_chan_map) == 0) {
        return adc_ch;
    }
    // checking digmic channel
    digmic_ch0 = get_lsb_pos(AUD_CHANNEL_MAP_DIGMIC_CH0);
    if (mic_ch >= digmic_ch0) {
        mic_ch -= digmic_ch0;
        digmic = true;
    } else {
        digmic = false;
    }
    // find the available ADC channel for dig mic or ana mic
    for (i = 0; i < adc_chan_num; i++) {
        if (adc_chan_map & (1 << i)) {
            if (digmic ^ !!(codec->REG_0C4 & (CODEC_CODEC_PDM_ADC_SEL_CH0 << i))) {
                continue;
            }
            if (digmic) {
                en_ch = (codec->REG_0C8 & (CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) >> (CODEC_CODEC_PDM_MUX_CH0_SHIFT + 3 * i);
            } else {
                en_ch = GET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0);
            }
            if (mic_ch == en_ch) {
                adc_ch = i;
                break;
            }
        }
    }
    return adc_ch;
}

void hal_codec_sidetone_enable(void)
{
#ifdef SIDETONE_ENABLE
#if (CFG_HW_AUD_SIDETONE_MIC_DEV & (CFG_HW_AUD_SIDETONE_MIC_DEV - 1))
#error "Invalid CFG_HW_AUD_SIDETONE_MIC_DEV: only 1 mic can be defined"
#endif
#if (CFG_HW_AUD_SIDETONE_MIC_DEV == 0) || (CFG_HW_AUD_SIDETONE_MIC_DEV & ~NORMAL_MIC_MAP)
#error "Invalid CFG_HW_AUD_SIDETONE_MIC_DEV: bad mic channel"
#endif
    int gain = CFG_HW_AUD_SIDETONE_GAIN_DBVAL;
    uint32_t val;

#ifdef SIDETONE_DEDICATED_ADC_CHAN
    sidetone_gain_offset = 0;
    if (gain > MAX_SIDETONE_DBVAL) {
        sidetone_gain_offset = gain - MAX_SIDETONE_DBVAL;
    } else if (gain < MIN_SIDETONE_DBVAL) {
        sidetone_gain_offset = gain - MIN_SIDETONE_DBVAL;
    }
#endif

    if (gain > MAX_SIDETONE_DBVAL) {
        gain = MAX_SIDETONE_DBVAL;
    } else if (gain < MIN_SIDETONE_DBVAL) {
        gain = MIN_SIDETONE_DBVAL;
    }

    val = MIN_SIDETONE_REGVAL + (gain - MIN_SIDETONE_DBVAL) / SIDETONE_DBVAL_STEP;

    codec->REG_080 = SET_BITFIELD(codec->REG_080, CODEC_CODEC_SIDE_TONE_GAIN, val);

#ifdef SIDETONE_DEDICATED_ADC_CHAN
    uint8_t adc_ch;
    uint8_t a;

    adc_ch = get_lsb_pos(sidetone_adc_ch_map);
    if (adc_ch >= NORMAL_ADC_CH_NUM) {
        ASSERT(false, "%s: Bad sidetone_adc_ch_map=0x%X", __func__, sidetone_adc_ch_map);
        return;
    }

    a = codec_rate_idx[AUD_STREAM_CAPTURE];
    hal_codec_set_adc_down(sidetone_adc_ch_map, codec_adc_sample_rate[a].adc_down);
    hal_codec_set_adc_hbf_bypass_cnt(sidetone_adc_ch_map, codec_adc_sample_rate[a].bypass_cnt);

    hal_codec_set_dig_adc_gain(sidetone_adc_ch_map, sidetone_adc_gain + sidetone_gain_offset);
#ifdef CFG_HW_AUD_SIDETONE_GAIN_RAMP
    hal_codec_get_adc_gain(sidetone_adc_ch_map, &sidetone_ded_chan_coef);
    hal_codec_set_dig_adc_gain(sidetone_adc_ch_map, MIN_DIG_DBVAL);
#endif
    codec->REG_0AC |= (CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << adc_ch);
    hal_codec_reg_update_delay();
    codec->REG_0AC &= ~(CODEC_CODEC_ADC_FIFO_FLUSH_CH0 << adc_ch);
    codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << adc_ch);

#ifdef CFG_HW_AUD_SIDETONE_IIR_INDEX
#if (CFG_HW_AUD_SIDETONE_IIR_INDEX >= ADC_IIR_CH_NUM + 0UL)
#error "Invalid CFG_HW_AUD_SIDETONE_IIR_INDEX"
#endif
    uint32_t mask;

    mask = CODEC_CODEC_ADC_IIR_CH0_SEL_MASK << (4 * CFG_HW_AUD_SIDETONE_IIR_INDEX);
    val = CODEC_CODEC_ADC_IIR_CH0_SEL(4) << (4 * CFG_HW_AUD_SIDETONE_IIR_INDEX);
    codec->REG_0E0 = (codec->REG_0E0 & ~mask) | val;
#endif
#endif
#endif
}

void hal_codec_sidetone_disable(void)
{
#ifdef SIDETONE_ENABLE
    codec->REG_080 = SET_BITFIELD(codec->REG_080, CODEC_CODEC_SIDE_TONE_GAIN, MUTE_SIDETONE_REGVAL);
#ifdef SIDETONE_DEDICATED_ADC_CHAN
    if (sidetone_adc_ch_map) {
        uint8_t adc_ch;

        adc_ch = get_lsb_pos(sidetone_adc_ch_map);
        if (adc_ch >= NORMAL_ADC_CH_NUM) {
            ASSERT(false, "%s: Bad sidetone_adc_ch_map=0x%X", __func__, sidetone_adc_ch_map);
            return;
        }
        codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << adc_ch);
    }
#endif
#endif
}

int hal_codec_sidetone_gain_ramp_up(float step)
{
    int ret = 0;
#ifdef CFG_HW_AUD_SIDETONE_GAIN_RAMP
    float coef;
    uint32_t val;

    hal_codec_get_adc_gain(sidetone_adc_ch_map, &coef);
    coef += step;
    if (coef >= sidetone_ded_chan_coef) {
        coef = sidetone_ded_chan_coef;
        ret = 1;
    }
    // Gain format: 8.12
    int32_t s_val = (int32_t)(coef * (1 << 12));
    val = __SSAT(s_val, 20);
    hal_codec_set_adc_gain_value(sidetone_adc_ch_map, val);
#endif
    return ret;
}

int hal_codec_sidetone_gain_ramp_down(float step)
{
    int ret = 0;
#ifdef CFG_HW_AUD_SIDETONE_GAIN_RAMP
    float coef;
    uint32_t val;

    hal_codec_get_adc_gain(sidetone_adc_ch_map, &coef);
    coef -= step;
    if (coef <= 0) {
        coef = 0;
        ret = 1;
    }

    // Gain format: 8.12
    int32_t s_val = (int32_t)(coef * (1 << 12));
    val = __SSAT(s_val, 20);
    hal_codec_set_adc_gain_value(sidetone_adc_ch_map, val);
#endif
    return ret;
}

void hal_codec_select_adc_iir_mic(uint32_t index, enum AUD_CHANNEL_MAP_T mic_map)
{
    uint32_t mask, val;
    uint8_t adc_ch;

    ASSERT(index < ADC_IIR_CH_NUM, "%s: Bad index=%u", __func__, index);
    ASSERT(mic_map && (mic_map & (mic_map - 1)) == 0, "%s: Bad mic_map=0x%X", __func__, mic_map);
#ifdef CFG_HW_AUD_SIDETONE_IIR_INDEX
    ASSERT(index != CFG_HW_AUD_SIDETONE_IIR_INDEX, "%s: Adc iir index conflicts with sidetone", __func__);
#endif

    // TODO: How to select iir adc index?
    adc_ch = hal_codec_get_adc_chan(mic_map, codec_mic_ch_map, codec_adc_ch_map, NORMAL_ADC_CH_NUM);
    if (adc_ch < NORMAL_ADC_CH_NUM) {
        mask = CODEC_CODEC_ADC_IIR_CH0_SEL_MASK << (4 * index);
        val = CODEC_CODEC_ADC_IIR_CH0_SEL(adc_ch) << (4 * index);
        codec->REG_0E0 = (codec->REG_0E0 & ~mask) | val;
    }
}

void hal_codec_min_phase_mode_enable(enum AUD_STREAM_T stream)
{
#ifdef CODEC_MIN_PHASE
    if (min_phase_cfg == 0 && codec_opened) {
        hal_codec_min_phase_init();
    }

    min_phase_cfg |= (1 << stream);
#endif
}

void hal_codec_min_phase_mode_disable(enum AUD_STREAM_T stream)
{
#ifdef CODEC_MIN_PHASE
    min_phase_cfg &= ~(1 << stream);

    if (min_phase_cfg == 0 && codec_opened) {
        hal_codec_min_phase_term();
    }
#endif
}

void hal_codec_sync_dac_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
#if defined(PSAP_APP) || defined(CODEC_TRIG_DAC_EN_ENABLE)
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_DAC_ENABLE_SEL, type);
#else
    //hal_codec_sync_dac_resample_rate_enable(type);

    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_DAC_LH_ENABLE_SEL, type);
#endif
}

void hal_codec_sync_dac_disable(void)
{
#if defined(PSAP_APP) || defined(CODEC_TRIG_DAC_EN_ENABLE)
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_DAC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#else
    //hal_codec_sync_dac_resample_rate_disable();
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_DAC_LH_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#endif
}

void hal_codec_sync_dac2_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    //hal_codec_sync_dac2_resample_rate_enable(type);
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_DAC_ENABLE_SEL_SND, type);
}

void hal_codec_sync_dac3_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    //hal_codec_sync_dac3_resample_rate_enable(type);
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_DAC_ENABLE_SEL_TRI, type);
}

void hal_codec_sync_dac2_disable(void)
{
    //hal_codec_sync_dac2_resample_rate_disable();
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_DAC_ENABLE_SEL_SND, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac3_disable(void)
{
    //hal_codec_sync_dac2_resample_rate_disable();
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_DAC_ENABLE_SEL_TRI, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_adc_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
#if defined(ANC_APP)
    //hal_codec_sync_adc_resample_rate_enable(type);
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_ADC_ENABLE_SEL, type);
#else
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_ADC_ENABLE_SEL, type);
#endif
}

void hal_codec_sync_adc_disable(void)
{
#if defined(ANC_APP)
    //hal_codec_sync_adc_resample_rate_disable();
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_ADC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#else
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_ADC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#endif
}

void hal_codec_sync_adc2_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    //hal_codec_sync_adc_resample_rate_enable(type);
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_ADC_SND_ENABLE_SEL, type);
}

void hal_codec_sync_adc2_disable(void)
{
    //hal_codec_sync_adc_resample_rate_disable();
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_ADC_SND_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac_resample_rate_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL, type);
}

void hal_codec_sync_dac_resample_rate_disable(void)
{
    codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac2_resample_rate_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND, type);
}

void hal_codec_sync_dac3_resample_rate_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI, type);
}

void hal_codec_sync_dac2_resample_rate_disable(void)
{
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac3_resample_rate_disable(void)
{
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_adc_resample_rate_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL, type);
}

void hal_codec_sync_adc_resample_rate_disable(void)
{
    codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_adc2_resample_rate_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL, type);
}

void hal_codec_sync_adc2_resample_rate_disable(void)
{
    codec->REG_748 = SET_BITFIELD(codec->REG_748, CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac_gain_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL, type);
}

void hal_codec_sync_dac_gain_disable(void)
{
    codec->REG_044 = SET_BITFIELD(codec->REG_044, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac2_gain_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_788 = SET_BITFIELD(codec->REG_788, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND, type);
}

void hal_codec_sync_dac3_gain_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#ifdef CODEC_APP
    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
#endif
    codec->REG_79C = SET_BITFIELD(codec->REG_79C, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_TRI, type);
}

void hal_codec_sync_dac2_gain_disable(void)
{
    codec->REG_788 = SET_BITFIELD(codec->REG_788, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac3_gain_disable(void)
{
    codec->REG_79C = SET_BITFIELD(codec->REG_79C, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_TRI, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_adc_gain_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
}

void hal_codec_sync_adc_gain_disable(void)
{
}

void hal_codec_sync_adc2_gain_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
}

void hal_codec_sync_adc2_gain_disable(void)
{
}

void hal_codec_set_dac_bt_sync_source(uint32_t src)
{
    codec->REG_078 = (codec->REG_078 & ~(
        CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK |
        CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(src) |
        CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL(src);

    codec->REG_048 = (codec->REG_048 & ~(
        CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_set_adc_bt_sync_source(uint32_t src)
{
    codec->REG_078 = (codec->REG_078 & ~(
        CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK |
        CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(src) |
        CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL(src);

    codec->REG_048 = (codec->REG_048 & ~(
        CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_set_adc2_bt_sync_source(uint32_t src)
{
    codec->REG_744 = (codec->REG_744 & ~(
        CODEC_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL_MASK | //CODEC_CODEC_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL_MASK |
        CODEC_RESAMPLE_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL(src) | //CODEC_CODEC_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL(src) |
        CODEC_RESAMPLE_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_set_dac2_bt_sync_source(uint32_t src)
{
    codec->REG_74C = (codec->REG_74C & ~(
        CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK |
        CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(src) |
        CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_set_dac3_bt_sync_source(uint32_t src)
{
    codec->REG_74C = (codec->REG_74C & ~(
        CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_MASK |
        CODEC_RESAMPLE_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL(src) |
        CODEC_RESAMPLE_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_pll_bt_sync_source(uint32_t src)
{
    codec->REG_078 = (codec->REG_078 & ~(CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_gpio_trigger_debounce_enable(void)
{
    if (codec_opened) {
        codec->REG_054 |= CODEC_GPIO_TRIGGER_DB_ENABLE;
    }
}

void hal_codec_gpio_trigger_debounce_disable(void)
{
    if (codec_opened) {
        codec->REG_054 &= ~CODEC_GPIO_TRIGGER_DB_ENABLE;
    }
}

void hal_codec_trigger_by_ws_enable(void)
{
    codec->REG_490 |= CODEC_TRIG_BT0_I2S1;
}

void hal_codec_trigger_by_ws_disable(void)
{
    codec->REG_490 &= ~CODEC_TRIG_BT0_I2S1;
}

void hal_codec_trigger_delay_cnt_by_ws(uint32_t cnt)
{
    ASSERT(cnt <= 0xFFFF, "[%s] cnt(%d) > 0xFFFF", __func__, cnt);
    cnt = 0x100000 - cnt;

    // WS trigger CODEC
    // 0: Periodic, 1: One short
    // codec->REG_490 |= CODEC_WS_TRIG_MODE;
    /**
     * 0x0: ws_cnt[5:0]
     * 0x1: ws_cnt[6:0]
     * 0xd: ws_cnt[18:0]
     * else: ws_cnt[19:0]
     */
    // codec->REG_490 = SET_BITFIELD(codec->REG_490, CODEC_WS_TRIGGER_INTERVAL, 0xF);

    codec->REG_490 = SET_BITFIELD(codec->REG_490, CODEC_WS_CNT_INI, cnt);

    codec->REG_490 |= CODEC_WS_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_490 &= ~CODEC_WS_FLUSH;
}

#ifdef CODEC_TIMER
int hal_codec_timer_trig_i2s_enable(enum HAL_CODEC_TIMER_TRIG_MODE_T mode, uint32_t ticks, bool periodic)
{
    uint32_t val;

    if (!codec_opened) {
        return 1;
    }

    if (periodic) {
        val = get_msb_pos(ticks);
        if (val < 31) {
            ticks += (1 << val) / 2;
        }
        val = get_msb_pos(ticks);
        if (val <= 16 || val >= 32) {
            val = 0;
        } else {
            val -= 16;
        }
        codec->REG_490 = SET_BITFIELD(codec->REG_490, CODEC_CALIB_INTERVAL, val);
    } else {
        ticks >>= 5;
        if (ticks == 0) {
            ticks = 1;
        }

        // reset codec timer
        codec->REG_054 &= ~CODEC_EVENT_FOR_CAPTURE;
        hal_codec_reg_update_delay();
        codec->REG_054 |= CODEC_EVENT_FOR_CAPTURE;

        // 0: DAC, 1: ADC, 2/3: DAC or ADC
        codec->REG_0BC = SET_BITFIELD(codec->REG_0BC, CODEC_EN_48KX64_MODE, mode);
    }

    codec->REG_070 = (codec->REG_070 & ~(CODEC_TRIG_TIME_MASK | CODEC_TRIG_MODE)) |
        CODEC_TRIG_TIME(ticks) | CODEC_TRIG_TIME_ENABLE | (periodic ? CODEC_TRIG_MODE : 0);

    return 0;
}

int hal_codec_timer_trig_i2s_disable(void)
{
    if (!codec_opened) {
        return 1;
    }

    codec->REG_070 &= ~CODEC_TRIG_TIME_ENABLE;

    return 0;
}

uint32_t hal_codec_timer_get(void)
{
    if (codec_opened) {
        return codec->REG_050;
    }

    return 0;
}

uint32_t hal_codec_timer_ticks_to_us(uint32_t ticks)
{
    uint32_t timer_freq;

    timer_freq = cur_codec_freq / 4 / CODEC_FREQ_EXTRA_DIV;

    return (uint32_t)((float)ticks * 1000000 / timer_freq);
}

uint32_t hal_codec_timer_us_to_ticks(uint32_t us)
{
    uint32_t timer_freq;

    timer_freq = cur_codec_freq / 4 / CODEC_FREQ_EXTRA_DIV;

    return (uint32_t)((float)us * timer_freq / 1000000);
}

void hal_codec_timer_trigger_read(void)
{
    if (codec_opened) {
        codec->REG_070 ^= CODEC_GET_CNT_TRIG;
        hal_codec_reg_update_delay();
    }
}

void hal_codec_timer_set_ws_trigger_cnt(uint32_t cnt)
{
    uint32_t val;

    val = get_msb_pos(cnt);
    if (val < 31) {
        cnt += (1 << val) / 2;
    }
    val = get_msb_pos(cnt);
    if (val <= 6 || val >= 32) {
        val = 0;
    } else {
        val -= 6;
        if (val > 13) {
            val = 13;
        }
    }
    codec->REG_490 = SET_BITFIELD(codec->REG_490, CODEC_WS_TRIGGER_INTERVAL, val);
}
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
int hal_codec_dac_sdm_reset_set(void)
{
    if (codec_opened) {
#ifdef DAC_RAMP_GAIN
        bool ramp = false;

        if (codec->REG_154 & CODEC_CODEC_RAMP_EN_CH0) {
            codec->REG_154 &= ~CODEC_CODEC_RAMP_EN_CH0;
            hal_codec_reg_update_delay();
            ramp = true;
        }
#endif
#ifndef ANC_PROD_TEST
        hal_codec_set_dac_gain_value((enum AUD_CHANNEL_MAP_T)VALID_DAC_MAP, 0);
#endif
#ifdef DAC_RAMP_GAIN
        if (ramp) {
            hal_codec_reg_update_delay();
            codec->REG_154 |= CODEC_CODEC_RAMP_EN_CH0;
        }
#endif
#if 0
        if (codec->REG_0BC & CODEC_CODEC_DAC_UH_EN) {
            osDelay(dac_delay_ms);
        }
#endif
#ifdef SDM_MUTE_NOISE_SUPPRESSION_V2
        for (int i = 0x200; i >= 0; i -= 0x100) {
            hal_codec_dac_dc_offset_enable(i, i);
            osDelay(1);
        }
#endif
        codec->REG_0BC |= CODEC_CODEC_DAC_SDM_CLOSE;
        osDelay(1);
    }

    return 0;
}

int hal_codec_dac_sdm_reset_clear(void)
{
    if (codec_opened) {
        osDelay(1);
        codec->REG_0BC &= ~CODEC_CODEC_DAC_SDM_CLOSE;
#ifdef SDM_MUTE_NOISE_SUPPRESSION_V2
        for (int i = 0x100; i <= 0x300; i += 0x100) {
            hal_codec_dac_dc_offset_enable(i, i);
            osDelay(1);
        }
#endif
        hal_codec_restore_dig_dac_gain();
    }

    return 0;
}
#endif

void hal_codec_tune_resample_rate(enum AUD_STREAM_T stream, float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    uint32_t val;

    if (!codec_opened) {
        return;
    }

    if (stream == AUD_STREAM_PLAYBACK) {
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) {
            codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            val = resample_phase_float_to_value(get_playback_resample_phase());
            val += (int)(val * ratio);
            codec->REG_0EC = val;
            hal_codec_reg_update_delay();
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
        }
    } else {
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) {
            codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            val = resample_phase_float_to_value(get_capture_resample_phase());
            val -= (int)(val * ratio);
            codec->REG_0F0 = val;
            hal_codec_reg_update_delay();
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
        }
    }
#endif
}

void hal_codec_tune_adc2_resample_rate(enum AUD_STREAM_T stream, float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    uint32_t val;

    if (!codec_opened) {
        return;
    }

    if (stream == AUD_STREAM_CAPTURE) {
        if (codec->REG_7C0 & CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE) {
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            val = resample_phase_float_to_value(get_capture2_resample_phase());
            val += (int)(val * ratio);
            codec->REG_7D0 = val;
            hal_codec_reg_update_delay();
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE;
        }
    }
#endif
}

void hal_codec_tune_dac2_resample_rate(enum AUD_STREAM_T stream, float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    uint32_t val;

    if (!codec_opened) {
        return;
    }

    if (stream == AUD_STREAM_PLAYBACK) {
        if (codec->REG_7C0 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND) {
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
            hal_codec_reg_update_delay();
            val = resample_phase_float_to_value(get_playback2_resample_phase());
            val += (int)(val * ratio);
            codec->REG_7C8 = val;
            hal_codec_reg_update_delay();
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND;
        }
    }
#endif
}

void hal_codec_tune_dac3_resample_rate(enum AUD_STREAM_T stream, float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    uint32_t val;

    if (!codec_opened) {
        return;
    }

    if (stream == AUD_STREAM_PLAYBACK) {
        if (codec->REG_7C0 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI) {
            codec->REG_7C0 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
            hal_codec_reg_update_delay();
            val = resample_phase_float_to_value(get_playback3_resample_phase());
            val += (int)(val * ratio);
            codec->REG_7CC = val;
            hal_codec_reg_update_delay();
            codec->REG_7C0 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI;
        }
    }
#endif
}

void hal_codec_tune_both_resample_rate(float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    bool update[2];
    uint32_t val[2];
    uint32_t lock;

    if (!codec_opened) {
        return;
    }

    update[0] = !!(codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE);
    update[1] = !!(codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE);

    val[0] = val[1] = 0;

    if (update[0]) {
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
        val[0] = resample_phase_float_to_value(get_playback_resample_phase());
        val[0] += (int)(val[0] * ratio);
    }
    if (update[1]) {
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
        val[1] = resample_phase_float_to_value(get_capture_resample_phase());
        val[1] -= (int)(val[1] * ratio);
    }

    hal_codec_reg_update_delay();

    if (update[0]) {
        codec->REG_0EC = val[0];
    }
    if (update[1]) {
        codec->REG_0F0 = val[1];
    }

    hal_codec_reg_update_delay();

    lock = int_lock();
    if (update[0]) {
        codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
    }
    if (update[1]) {
        codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
    }
    int_unlock(lock);
#endif
}

int hal_codec_select_clock_out(uint32_t cfg)
{
    uint32_t lock;
    int ret = 1;

    lock = int_lock();

    if (codec_opened) {
        codec->REG_07C = SET_BITFIELD(codec->REG_07C, CODEC_CFG_CLK_OUT, cfg);
        ret = 0;
    }

    int_unlock(lock);

    return ret;
}

#ifdef AUDIO_ANC_FB_MC
void hal_codec_mc_enable(void)
{
    mc_enabled = true;
}

void hal_codec_mc_disable(void)
{
    mc_enabled = false;
}

void hal_codec_setup_mc(enum AUD_CHANNEL_NUM_T channel_num, enum AUD_BITS_T bits)
{
    mc_chan_num = channel_num;
    mc_bits = bits;
}
#endif

void hal_codec_swap_output(bool swap)
{
#ifdef AUDIO_OUTPUT_SWAP
    output_swap = swap;

    if (codec_opened) {
        if (output_swap) {
            codec->REG_0B8 |= CODEC_CODEC_DAC_OUT_SWAP;
        } else {
            codec->REG_0B8 &= ~CODEC_CODEC_DAC_OUT_SWAP;
        }
    }
#endif
}

int hal_codec_config_digmic_phase(uint8_t phase)
{
#ifdef ANC_PROD_TEST
    codec_digmic_phase = phase;
#endif
    return 0;
}

static void hal_codec_general_irq_handler(void)
{
    uint32_t status1;
    uint32_t status2;

    status1 = codec->REG_00C;
    codec->REG_00C = status1;

    status1 &= codec->REG_010;

    status2 = codec->REG_478;
    codec->REG_478 = status2;

    status2 &= codec->REG_47C;

    for (int i = 0; i < CODEC_IRQ_TYPE_QTY; i++) {
        if (codec_irq_callback[i]) {
            codec_irq_callback[i](status1, status2);
        }
    }
}

static void hal_codec_set_irq_handler(enum CODEC_IRQ_TYPE_T type, HAL_CODEC_IRQ_CALLBACK2 cb)
{
    uint32_t lock;

    ASSERT(type < CODEC_IRQ_TYPE_QTY, "%s: Bad type=%d", __func__, type);

    lock = int_lock();

    codec_irq_callback[type] = cb;

    if (cb) {
        if (codec_irq_map == 0) {
            NVIC_SetVector(CODEC_IRQn, (uint32_t)hal_codec_general_irq_handler);
            NVIC_SetPriority(CODEC_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_ClearPendingIRQ(CODEC_IRQn);
            NVIC_EnableIRQ(CODEC_IRQn);
        }
        codec_irq_map |= (1 << type);
    } else {
        codec_irq_map &= ~(1 << type);
        if (codec_irq_map == 0) {
            NVIC_DisableIRQ(CODEC_IRQn);
            NVIC_ClearPendingIRQ(CODEC_IRQn);
        }
    }

    int_unlock(lock);
}

static void hal_codec_anc_fb_isr(uint32_t irq_status1, uint32_t irq_status2)
{
    if (irq_status1 & (CODEC_FB_CHECK_ERROR_TRIG_CH0 | CODEC_FB_CHECK_ERROR_TRIG_CH1)) {
        if (anc_fb_callback) {
            anc_fb_callback(irq_status1);
        }
    }
}

void hal_codec_anc_fb_check_set_irq_handler(HAL_CODEC_IRQ_CALLBACK cb)
{
    anc_fb_callback = cb;
    if (cb) {
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_ANC_FB_CHECK, hal_codec_anc_fb_isr);
    } else {
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_ANC_FB_CHECK, NULL);
    }
}

int hal_codec_dac_dc_auto_calib_enable(void)
{
    enum AUD_CHANNEL_MAP_T map;
    uint32_t val;

    map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    // 0dB
    val = (1 << 12);
    hal_codec_set_adc_gain_value(map, val);

#if defined(DAC_RAMP_GAIN)
    codec->REG_154 &= ~CODEC_CODEC_RAMP_EN_CH0;
    codec->REG_158 &= ~CODEC_CODEC_RAMP_EN_CH1;
#endif
#ifdef DAC_DRE_ENABLE
    hal_codec_dac_dre_disable();
#endif
#if defined(CODEC_ADC_DC_FILTER_FACTOR)
    hal_codec_enable_adc_dc_filter(map, false);
#endif
    return 0;
}

int hal_codec_dac_dc_auto_calib_disable(void)
{
    return 0;
}

//********************BT trigger functions: START********************
static void hal_codec_bt_trigger_isr(uint32_t irq_status1, uint32_t irq_status2)
{
    uint32_t index;

    for (index = 0; index < BT_TRIGGER_NUM; index++) {
        if (irq_status2 & (CODEC_BT_TRIGGER << index)) {
            if (bt_trigger_callback[index]) {
                TRACE(1,"[%s] bt_trigger_callback[%u] start", __func__, index);
                bt_trigger_callback[index]();
            } else {
                TRACE(1,"[%s] bt_trigger_callback[%u] = NULL", __func__, index);
            }
        }
    }
}

static inline void hal_codec_bt_trigger_irq_en(uint32_t index, int enable)
{
    if (enable)
        codec->REG_710 |= (CODEC_BT_TRIGGER_MASK << index);
    else
        codec->REG_710 &= ~(CODEC_BT_TRIGGER_MASK << index);

    codec->REG_70C = (CODEC_BT_TRIGGER << index);
}

void hal_codec_set_bt_trigger_ex_callback(uint32_t index, HAL_CODEC_BT_TRIGGER_CALLBACK callback)
{
    if (index >= BT_TRIGGER_NUM) {
        return;
    }

    bt_trigger_callback[index] = callback;
}

int hal_codec_bt_trigger_ex_start(uint32_t index)
{
    uint32_t lock;

    TRACE(1,"[%s] Start: index=%d", __func__, index);

    if (index >= BT_TRIGGER_NUM) {
        return 1;
    }

    lock = int_lock();

    hal_codec_set_irq_handler((enum CODEC_IRQ_TYPE_T)(CODEC_IRQ_TYPE_BT_TRIGGER + index), hal_codec_bt_trigger_isr);
    hal_codec_bt_trigger_irq_en(index, 1);

    int_unlock(lock);

    return 0;
}

int hal_codec_bt_trigger_ex_stop(uint32_t index)
{
    uint32_t lock;

    TRACE(1,"[%s] Stop", __func__);

    if (index >= BT_TRIGGER_NUM) {
        return 1;
    }

    lock = int_lock();

    hal_codec_bt_trigger_irq_en(index, 0);
    hal_codec_set_irq_handler((enum CODEC_IRQ_TYPE_T)(CODEC_IRQ_TYPE_BT_TRIGGER + index), NULL);

    int_unlock(lock);

    return 0;
}

void hal_codec_set_bt_trigger_callback(HAL_CODEC_BT_TRIGGER_CALLBACK callback)
{
    hal_codec_set_bt_trigger_ex_callback(0, callback);
}

int hal_codec_bt_trigger_start(void)
{
    return hal_codec_bt_trigger_ex_start(0);
}

int hal_codec_bt_trigger_stop(void)
{
    return hal_codec_bt_trigger_ex_stop(0);
}

//********************BT trigger functions: END********************

static void hal_codec_event_trigger_isr(uint32_t irq_status1, uint32_t irq_status2)
{
    if ((irq_status1 & CODEC_EVENT_TRIGGER) == 0) {
        return;
    }

    if (event_trigger_callback) {
        event_trigger_callback();
    }
}

static inline void hal_codec_event_trigger_irq_en(int enable)
{
    if (enable) {
        codec->REG_010 |= CODEC_EVENT_TRIGGER_MASK;
    } else {
        codec->REG_010 &= ~CODEC_EVENT_TRIGGER_MASK;
    }

    codec->REG_00C = CODEC_EVENT_TRIGGER;
}

void hal_codec_set_event_trigger_callback(HAL_CODEC_EVENT_TRIGGER_CALLBACK callback)
{
    uint32_t lock;

    event_trigger_callback = callback;

    lock = int_lock();
    if (callback) {
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_EVENT_TRIGGER, hal_codec_event_trigger_isr);
        hal_codec_event_trigger_irq_en(1);
    } else {
        hal_codec_event_trigger_irq_en(0);
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_EVENT_TRIGGER, NULL);
    }
    int_unlock(lock);
}

static void hal_codec_timer_trigger_isr(uint32_t irq_status1, uint32_t irq_status2)
{
    if ((irq_status1 & CODEC_TIME_TRIGGER) == 0) {
        return;
    }

    if (timer_trigger_callback) {
        timer_trigger_callback();
    }
}

static inline void hal_codec_timer_trigger_irq_en(int enable)
{
    if (enable) {
        codec->REG_010 |= CODEC_TIME_TRIGGER_MASK;
    } else {
        codec->REG_010 &= ~CODEC_TIME_TRIGGER_MASK;
    }

    codec->REG_00C = CODEC_TIME_TRIGGER;
}

void hal_codec_set_timer_trigger_callback(HAL_CODEC_TIMER_TRIGGER_CALLBACK callback)
{
    uint32_t lock;

    timer_trigger_callback = callback;

    lock = int_lock();
    if (callback) {
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_TIMER_TRIGGER, hal_codec_timer_trigger_isr);
        hal_codec_timer_trigger_irq_en(1);
    } else {
        hal_codec_timer_trigger_irq_en(0);
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_TIMER_TRIGGER, NULL);
    }
    int_unlock(lock);
}

#endif

