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
#include <besbt_string.h>
#include "hal_chipid.h"
#include "bt_drv.h"
#include "bt_drv_interface.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_internal.h"

//CHIP related
#include "bt_drv_1603_internal.h"
#include "bt_1603_reg_map.h"
#include "bt_drv_1603_config.h"
typedef struct
{
    uint8_t is_act;
    uint16_t opcode;
    uint8_t parlen;
    const uint8_t *param;

} BTDRV_CFG_TBL_STRUCT;


#define BTDRV_CONFIG_ACTIVE   1
#define BTDRV_CONFIG_INACTIVE 0
#define BTDRV_INVALID_TRACE_LEVEL  0xFF
/*
[0][0] = 63, [0][1] = 0,[0][2] = (-80),           472d
[1][0] = 51, [2][1] = 0,[2][2] = (-80),          472b
[2][0] = 42, [4][1] = 0,[4][2] = (-75),           4722
[3][0] = 36, [6][1] = 0,[6][2] = (-55),           c712
[4][0] = 30, [8][1] = 0,[8][2] = (-40),           c802
[5][0] = 21,[10][1] = 0,[10][2] = 0x7f,         c102
[6][0] = 12,[11][1] = 0,[11][2] = 0x7f,       c142
[7][0] = 3,[13][1] = 0,[13][2] = 0x7f,        c1c2
[8][0] = -3,[14][1] = 0,[14][2] = 0x7f};      c0c2
*/
const int8_t btdrv_rf_env_1603[]=
{
    0x01,0x00,  //rf api
    0x01,   //rf env
    185,     //rf length
    BT_MAX_TX_PWR_IDX,     //txpwr_max
    -1,    ///rssi high thr
    -2,   //rssi low thr
    -100,  //rssi interf thr
    0xf,  //rssi interf gain thr
    2,  //wakeup delay
    0xe, 0, //skew
    0xe8,0x3,    //ble agc inv thr
    BTC_HW_AGC_ENABLE_FLAG,
    0xff,//sw gain set
    0xff,    //sw gain set
    -85,//bt_inq_page_iscan_pscan_dbm
    0x7f,//ble_scan_adv_dbm
    1,      //sw gain reset factor
    1,    //bt sw gain cntl enable
    1,   //ble sw gain cntl en
    1,  //bt interfere  detector en
    0,  //ble interfere detector en

    0,0,0,
    3,3,12,
    6,6,28,
    9,9,28,
    12,12,28,
    15,15,28,
    18,18,28,
    21,21,28,
    0x7f,24,0x7f,
    0x7f,27,0x7f,
    0x7f,30,0x7f,
    0x7f,33,0x7f,
    0x7f,36,0x7f,
    0x7f,39,0x7f,
    0x7f,42,0x7f,  //rx hwgain tbl ptr

#ifdef BT_ITN_SGL_LNA
    56,0,-80,
    50,0,-80,
    47,0,-80,
    42,0,-80,
    37,0,-80,
    32,0,-80,
    27,0,-80,
    10,0,-80,
#else
    58,0,-80,
    49,0,-80,
    46,0,-80,
    40,0,-80,
    34,0,-80,
    29,0,-80,
    25,0,-80,
    5,0,-80,
#endif

    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,  //rx gain tbl ptr

#ifdef BT_ITN_SGL_LNA
    -91,-91,
    -84,-84,
    -79,-79,
    -75,-75,
    -70,-70,
    -65,-65,
    -45,-45,
#else
    -92,-92,
    -84,-84,
    -78,-78,
    -75,-75,
    -70,-70,
    -65,-65,
    -43,-43,
#endif

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr

    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,1,
    0,2,
    0,2,
    0,2,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //flpha filter factor ptr
    -23,-20,-17,-14,-11,-8,-5,-2,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,   //tx pw onv tbl ptr
};

const int8_t btdrv_rxgain_gain_ths_3m[] = {
#ifdef BT_ITN_SGL_LNA
    -83,-83,
    -76,-76,
    -71,-71,
    -67,-67,
    -62,-62,
    -57,-57,
    -37,-37,
#else
    -84,-84,
    -76,-76,
    -70,-70,
    -67,-67,
    -62,-62,
    -57,-57,
    -35,-35,
#endif

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_ths_tbl_le[0xf * 2] = {
#ifdef BT_ITN_SGL_LNA
    -91,-91,
    -84,-84,
    -79,-79,
    -75,-75,
    -70,-70,
    -65,-65,
    -45,-45,
#else
    -92,-92,
    -84,-84,
    -78,-78,
    -75,-75,
    -70,-70,
    -65,-65,
    -43,-43,
#endif

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f, //ble rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_ths_tbl_le_2m[0xf * 2] = {
#ifdef BT_ITN_SGL_LNA
    -83,-83,
    -76,-76,
    -71,-71,
    -67,-67,
    -62,-62,
    -57,-57,
    -37,-37,
#else
    -84,-84,
    -76,-76,
    -70,-70,
    -67,-67,
    -62,-62,
    -57,-57,
    -35,-35,
#endif

    0x7f,0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f, //ble rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_ths_tbl_ecc[0xf * 2] = {
#ifdef BT_ITN_SGL_LNA
    -91,-91,
    -84,-84,
    -79,-79,
    -75,-75,
    -70,-70,
    -65,-65,
    -45,-45,
#else
    -92,-92,
    -84,-84,
    -78,-78,
    -75,-75,
    -70,-70,
    -65,-65,
    -43,-43,
#endif

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //ECC rx gain ths tbl ptr
};

//ble txpwr convert
int8_t  btdrv_txpwr_conv_tbl[8] = {
        [0] = -4,
        [1] = 0,
        [2] = 3,
        [3] = 7,
        [4] = 11,
        [5] = 16,
        [6] = 16,
        [7] = 16
};

const int8_t btdrv_afh_env[] =
{
    0x02,0x00,   //afh env
    0x00,      //ignore
    33,          //length
    5,   //nb_reass_chnl
    10,  //win_len
    -70,  //rf_rssi_interf_thr
    10,  //per_thres_bad
    20,  //reass_int
    20,   //n_min
    20,   //afh_rep_intv_max
    96,    //ths_min
    2,   //chnl_assess_report_style
    15,  //chnl_assess_diff_thres
    60, // chnl_assess_interfere_per_thres_bad
    9,  //chnl_assess_stat_cnt_max
    -9,  //chnl_assess_stat_cnt_min
    1,2,3,2,1,   //chnl_assess_stat_cnt_inc_mask[5]
    1,2,3,2,1,    //chnl_assess_stat_cnt_dec_mask
    0xd0,0x7,      //chnl_assess_timer
    -48,        //chnl_assess_min_rssi
    0x64,0,   //chnl_assess_nb_pkt
    0x32,0,     //chnl_assess_nb_bad_pkt
    6,    //chnl_reassess_cnt_val
    0x3c,0,     //chnl_assess_interfere_per_thres_bad
};

const uint8_t lpclk_drift_jitter[] =
{
    0xfa,0x00,  //  drift  250ppm
    0x0a,0x00    //jitter  +-10us

};

uint8_t  sleep_param[] =
{
    0,    // sleep_en;
    1,    // exwakeup_en;
    0xd0,0x7,    //  lpo_calib_interval;   lpo calibration interval
    0x32,0,0,0,    // lpo_calib_time;  lpo count lpc times
};

const uint8_t  sync_config[] =
{
    1,1,   //sco path config   0:hci  1:pcm
    0,      //sync use max buff length   0:sync data length= packet length 1:sync data length = host sync buff len
    0,        //cvsd bypass     0:cvsd2pcm   1:cvsd transparent
};

//MHDT
#ifdef __MHDT_SWAGC__
const int8_t btdrv_rxgain_gain_ths_mhdt_bt2m_1m[0xf * 2] = {
    -88,-88,
    -81,-81,
    -77,-77,
    -75,-75,
    -71,-71,
    -62,-62,
    -42,-42,

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_gain_ths_mhdt_bt2m_2m[0xf * 2] = {
    -85,-85,
    -77,-77,
    -70,-70,
    -69,-69,
    -65,-65,
    -62,-62,
    -38,-38,

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_gain_ths_mhdt_bt2m_3m[0xf * 2] = {
    -77,-77,
    -69,-69,
    -62,-62,
    -59,-59,
    -52,-52,
    -46,-46,
    -30,-30,

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_gain_ths_mhdt_bt4m_1m[0xf * 2] = {
    -76,-76,
    -74,-74,
    -65,-65,
    -60,-60,
    -55,-55,
    -46,-46,
    -27,-27,

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_gain_ths_mhdt_bt4m_2m[0xf * 2] = {
    -76,-76,
    -74,-74,
    -65,-65,
    -60,-60,
    -55,-55,
    -46,-46,
    -27,-27,

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_gain_ths_mhdt_bt4m_3m[0xf * 2] = {
    -80,-80,
    -75,-75,
    -70,-70,
    -66,-66,
    -62,-62,
    -57,-57,
    -27,-27,

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};
#endif

//pcm general ctrl
#define PCM_PCMEN_POS            15
#define PCM_LOOPBCK_POS          14
#define PCM_MIXERDSBPOL_POS      11
#define PCM_MIXERMODE_POS        10
#define PCM_STUTTERDSBPOL_POS    9
#define PCM_STUTTERMODE_POS      8
#define PCM_CHSEL_POS            6
#define PCM_MSTSLV_POS           5
#define PCM_PCMIRQEN_POS         4
#define PCM_DATASRC_POS          0


//pcm phy ctrl
#define PCM_LRCHPOL_POS     15
#define PCM_CLKINV_POS      14
#define PCM_IOM_PCM_POS     13
#define PCM_BUSSPEED_LSB    10
#define PCM_SLOTEN_MASK     ((uint32_t)0x00000380)
#define PCM_SLOTEN_LSB      7
#define PCM_WORDSIZE_MASK   ((uint32_t)0x00000060)
#define PCM_WORDSIZE_LSB    5
#define PCM_DOUTCFG_MASK    ((uint32_t)0x00000018)
#define PCM_DOUTCFG_LSB     3
#define PCM_FSYNCSHP_MASK   ((uint32_t)0x00000007)
#define PCM_FSYNCSHP_LSB    0

/// Enumeration of PCM status
enum PCM_STAT
{
    PCM_DISABLE = 0,
    PCM_ENABLE
};

/// Enumeration of PCM channel selection
enum PCM_CHANNEL
{
    PCM_CH_0 = 0,
    PCM_CH_1
};

/// Enumeration of PCM role
enum PCM_MSTSLV
{
    PCM_SLAVE = 0,
    PCM_MASTER
};

/// Enumeration of PCM data source
enum PCM_SRC
{
    PCM_SRC_DPV = 0,
    PCM_SRC_REG
};

/// Enumeration of PCM left/right channel selection versus frame sync polarity
enum PCM_LR_CH_POL
{
    PCM_LR_CH_POL_RIGHT_LEFT = 0,
    PCM_LR_CH_POL_LEFT_RIGHT
};

/// Enumeration of PCM clock inversion
enum PCM_CLK_INV
{
    PCM_CLK_RISING_EDGE = 0,
    PCM_CLK_FALLING_EDGE
};

/// Enumeration of PCM mode selection
enum PCM_MODE
{
    PCM_MODE_PCM = 0,
    PCM_MODE_IOM
};

/// Enumeration of PCM bus speed
enum PCM_BUS_SPEED
{
    PCM_BUS_SPEED_128k = 0,
    PCM_BUS_SPEED_256k,
    PCM_BUS_SPEED_512k,
    PCM_BUS_SPEED_1024k,
    PCM_BUS_SPEED_2048k
};

/// Enumeration of PCM slot enable
enum PCM_SLOT
{
    PCM_SLOT_NONE = 0,
    PCM_SLOT_0,
    PCM_SLOT_0_1,
    PCM_SLOT_0_2,
    PCM_SLOT_0_3
};

/// Enumeration of PCM word size
enum PCM_WORD_SIZE
{
    PCM_8_BITS = 0,
    PCM_13_BITS,
    PCM_14_BITS,
    PCM_16_BITS
};

/// Enumeration of PCM DOUT pad configuration
enum PCM_DOUT_CFG
{
    PCM_OPEN_DRAIN = 0,
    PCM_PUSH_PULL_HZ,
    PCM_PUSH_PULL_0
};

/// Enumeration of PCM FSYNC physical shape
enum PCM_FSYNC
{
    PCM_FSYNC_LF = 0,
    PCM_FSYNC_FR,
    PCM_FSYNC_FF,
    PCM_FSYNC_LONG,
    PCM_FSYNC_LONG_16
};

/// Enumeration of IQDUMP data choice
enum IQDUMP_DATA_CHOICE_FLAG_T{
    MHDT_RXFE_OUTPUT = 0x1,
    FSC_INPUT,
    RC_DIGGAIN_OUTPUT,
    MHDT_RXFE_OUTPUT_2,
    RX_FRONT_END_OUTPUT,
    GFSK_LPF_OUTPUT,
    DPSK_RRC_OUTPUT,
    DPSK_RRC_INPUT,
    DPSK_COC_OUTPUT,
    DGP_RECOC_CORR1_IQ,
    DGP_DFD_ROT_IQ,
    RXDATAIQ_DGP_DFD_RES_SUM_IQ,
    FSC_OUTPUT = 0xf,
    IQIMB_OUTPUT,
    DCC_OUTPUT,
};

const uint32_t pcm_setting[] =
{
//pcm_general_ctrl
    (PCM_DISABLE<<PCM_PCMEN_POS) |                      //enable auto
    (PCM_DISABLE << PCM_LOOPBCK_POS)  |                 //LOOPBACK test
    (PCM_DISABLE << PCM_MIXERDSBPOL_POS)  |
    (PCM_DISABLE << PCM_MIXERMODE_POS)  |
    (PCM_DISABLE <<PCM_STUTTERDSBPOL_POS) |
    (PCM_DISABLE <<PCM_STUTTERMODE_POS) |
    (PCM_CH_0<< PCM_CHSEL_POS) |
    (PCM_MASTER<<PCM_MSTSLV_POS) |                      //BT clock
    (PCM_DISABLE << PCM_PCMIRQEN_POS) |
    (PCM_SRC_DPV<<PCM_DATASRC_POS),

//pcm_phy_ctrl
    (PCM_LR_CH_POL_RIGHT_LEFT << PCM_LRCHPOL_POS) |
    (PCM_CLK_FALLING_EDGE << PCM_CLKINV_POS) |
    (PCM_MODE_PCM << PCM_IOM_PCM_POS) |
    (PCM_BUS_SPEED_2048k << PCM_BUSSPEED_LSB) |         //8k sample rate; 2048k = slot_num * sample_rate * bit= 16 * 8k * 16
    (PCM_SLOT_0_1 << PCM_SLOTEN_LSB) |
    (PCM_16_BITS << PCM_WORDSIZE_LSB) |
    (PCM_PUSH_PULL_0 << PCM_DOUTCFG_LSB) |
    (PCM_FSYNC_LF << PCM_FSYNCSHP_LSB),
};

const uint8_t local_feature[] =
{
#if defined(__3M_PACK__)
    0xBF, 0xeE, 0xCD,0xFe,0xdb,0xFd,0x7b,0x87
#else
    0xBF, 0xeE, 0xCD,0xFa,0xdb,0xbd,0x7b,0x87

    //0xBF,0xFE,0xCD,0xFa,0xDB,0xFd,0x73,0x87   // disable simple pairing
#endif
};

const uint8_t local_ex_feature_page1[] =
{
    1,   //page
#if (defined(CTKD_ENABLE)&&defined(IS_CTKD_OVER_BR_EDR_ENABLED))
    0x0F,0,0,0,0,0,0,0,   //page 1 feature
#else
    2,0,0,0,0,0,0,0,   //page 1 feature
#endif
};

const uint8_t local_ex_feature_page2[] =
{
    2,   //page
    0x1f,0x03,0x00,0x00,0x00,0x00,0x00,0x00,   //page 2 feature
};

const uint8_t bt_rf_timing[] =
{
    0x37,// rxpwrupct;
    0x08,// txpwrdnct;
    0x1E,// txpwrupct;
    0x00,// rxpathdly;
    0x10,// txpathdly;
    0x00,// sync_position;
    0x12,// edr_rxgrd_timeout;
};

// LE 1M:uncoded PHY at 1Mbps
// LE 2M:uncoded PHY at 2Mbps
const uint8_t ble_rf_timing[] =
{
    0x00,  //LE 1M syncposition0
    0x37,  //LE 1M rxpwrup0
    0x08,  //LE 1M txpwrdn0
    0x1E,  //LE 1M txpwrup0

    0x00,  //LE 2M syncposition1
    0x37,  //LE 2M rxpwrup1
    0x08,  //LE 2M txpwrdn1
    0x1E,  //LE 2M txpwrup1

    0x00,  //coded PHY at 125kbps and 500kbps syncposition2
    0x41,  //coded PHY at 125kbps and 500kbps rxpwrup2
    0x08,  //coded PHY at 125kbps txpwrdn2
    0x1E,  //coded PHY at 125kbp txpwrup2

    0x08,  //coded PHY at 500kbps txpwrdn3
    0x1E,  //coded PHY at 500kbps txpwrup3

    0x00,  //LE 1M rfrxtmda0
    0x04,  //LE 1M rxpathdly0
    0x0c,  //LE 1M txpathdly0

    0x00,  //LE 2M rfrxtmda1
    0x02,  //LE 2M rxpathdly1
    0x0c,  //LE 2M txpathdly1

    0x85,  //coded PHY at 125kbps rxflushpathdly2
    0xa0,  //coded PHY at 125kbps rfrxtmda2
    0x14,  //coded PHY at 125kbps rxpathdly2
    0x0c,  //coded PHY at 125kbps txpathdly2

    0x26,  //coded PHY at 500kbps rxflushpathdly3
    0x00,  //coded PHY at 500kbps rfrxtmda3
    0x0c,  //coded PHY at 500kbps txpathdly3

    /*   AoA/AoD Registers */
    0x08,  //rxsampstinst01us
    0x18,  //rxswstinst01us
    0x19,  //txswstinst01us

    0x08,  //rxsampstinst02us
    0x18,  //rxswstinst02us
    0x19,  //txswstinst02us

    0x08,  //rxsampstinst11us
    0x18,  //rxswstinst11us
    0x19,  //txswstinst11us

    0x08,  //rxsampstinst12us
    0x18,  //rxswstinst12us
    0x19,  //txswstinst12us

    0x00,  //rxprimidcntlen
    0x00,  //rxprimantid
    0x00,  //txprimidcntlen
    0x00,  //txprimantid
};

const uint8_t bt_common_setting_1603[64] =
{
#ifdef CURRENT_TEST
    0x00, //trace_level
#else
    0x03, //trace_level
#endif
    0x01, //trace_output
    0x00,0x00, //tports_level
    0x01, //power_control_type
    0x00, //close_loopbacken_flag
    0x00, //force_max_slot
    0x06, //wesco_nego
    0x05, //force_max_slot_in_sco
    0x01, //esco_retx_after_establish
    0x00,0x00, //sco_start_delay
    0x01, //msbc_pcmdout_zero_flag
    0x01, //master_2_poll
    0x01, //pca_disable_in_nosync
    BT53_VERSION, //version_major
    0x00, //version_minor
    0x16, //version_build
    0xb0,0x02, //comp_id
    0x00, //ptt_check
    0x00, //address_reset
    0x02,0x00, //bestrx_sch_traffic
    0x00, //key_gen_after_reset
    0x01, //intersys_1_wakeup
    0x05,0x00, //lmp_to_before_complete
    0x08, //fastpcm_interval
    0x01, //lm_env_reset_local_name
    0x00, //detach_directly
    0x00, //clk_off_force_even
    0xc8, //seq_error_num
    0x03, //delay_process_lmp_type
    0x0a, //delay_process_lmp_to 10*100 halt slot
    0x01, //ignore_pwr_ctrl_sm_state
    0x00, //support_hiaudio (sco audio policy)
    0x01, //pscan_coex_en
    0x01, //iscan_coex_en
    0x00, //drift_limited
    #ifdef mHDT_SUPPORT
    0x01, //dev_mode=mHDT
    #else
    0x00, //dev_mode=BT
    #endif
    0x00, //reserve0
    0x00, //reserve1
    0x00, //reserve2
    0x1e,0x00,0x00,0x00, //lmp_rsp_to
    0x01, //hec_error_no_update_last_ts
    0x96, //acl_rssi_avg_nb_pkt
    0xff,0xff, //sniff_win_lim
    0x01, //iso_host_to_controller_flow
    0x03, //enable_assert
    0x00, //sw_seq_filter_en
    0x00, //ble_aux_adv_ind_update
    0x00, //sco_start_delay_flag
    0x00, //send_evt_when_wakeup
    0x00, //signal_test_en
    0x06, //ble_cis_conn_event_cnt_distance (in connect interval)
    0x0c, //ble_ci_alarm_init_distance (in half slots)
    0x01, //sync_force_max_slot
    0x00,0x08, //max_hdc_adv_dur in slots
};

const uint8_t bt_sche_setting_1603[40] =
{
#ifdef __PA_UNSTABLE_WORKAROUND__
    0x01, //hwspi_bt_cs_en
#else
    0x00, //hwspi_bt_cs_en
#endif
    0x01, //two_slave_sched_en
    0xff, //music_playing_link
#ifdef __PA_UNSTABLE_WORKAROUND__
    0x01, //hwspi_ble_cs_en
#else
    0x00, //hwspi_ble_cs_en
#endif
    0x00, //reconnecting_flag
    0x08, //default_tpoll
    0x08, //acl_slot_in_ibrt_mode
    0x34, //ibrt_start_estb_interval
    0x9c,0x00, //acl_interv_in_ibrt_sco_mode
    0x68,0x00, //acl_interv_in_ibrt_normal_mode
    0x40,0x06, //acl_switch_to_threshold
    0x90,0x01, //stopbitoffthd
    0x0a, //delay_schd_min
    0x02, //stopthrmin
    0x78, //acl_margin
    0x50, //sco_margin
    0x01, //adv_after_sco_retx
    0x01, //ble_wrong_packet_lantency
    0x60,0x00, //sniff_priority_interv_thd
    0x0c, //scan_hslot_in_sco 12 half slot in 2ev3
    0x02, //double_pscan_in_sco
    0x06, //reduce_rext_for_sniff_thd
    0x06, //reduce_att_for_sco_thd
    0x01, //reduce_att_space_adjust
    0x01, //bandwidth_check_ignore_retx
    0x00, //check_acl_ble_en
    0x01, //check_sco_ble_en
    0x00, //sch_plan_slave_disable
    0x0a, //sco_anchor_start_add
    0x02, //ble_slot
    0x02, //sniff_max_frm_time
    0x06, //ble_con_in_sco_duration
    0x01, //unsniff_schdule_more;
    0x64,0x00, //unsniff_trans_interval;
};

const uint8_t bt_ibrt_setting_1603[29] =
{
    0x01, //hci_auto_accept_tws_link_en
    0x00, //send_profile_via_ble
    0x00, //force_rx_error
    0x6c, //sync_win_size hus
    0x16, //magic_cal_bitoff
    0x3f, //role_switch_packet_br
    0x2b, //role_switch_packet_edr
    0x01, //only_support_slave_role
    0x01, //msg_filter_en
    0x00, //relay_sam_info_in_start_snoop
    0x00, //snoop_switch_interval_num
    0x02, //slave_rx_traffic_siam
    0x05, //ibrt_lmp_to
    0x00, //fa_use_twslink_table
    0x00, //fa_use_fix_channel
    0x06, //ibrt_afh_instant_adjust
    0x0c, //ibrt_detach_send_instant
    0x03, //ibrt_detach_receive_instant
    0x03, //ibrt_detach_txcfm_instant
    0x01, //mobile_enc_change
    0x01, //ibrt_auto_accept_sco
    0x01, //ibrt_second_sco_decision
    0x02, //fa_ok_thr_in_sco
    0x02, //fa_ok_thr_for_acl
    0x01, //accep_remote_bt_roleswitch
    0x01, //accept_remote_enter_sniff
    0x0f, //start_ibrt_tpoll
    0x00, //update_all_saved_queue
    0x00, //not_pro_saved_msg_in_ibrt_switch
};

const uint8_t bt_hw_feat_setting_1603[39] =
{
    0x01, //rxheader_int_en
    0x00, //rxdone_bt_int_en
    0x00, //txdone_bt_int_en
    0x01, //rxsync_bt_int_en
    0x01, //rxsync_ble_int_en
    0x00, //pscan_hwagc_flag
    0x00, //iscan_hwagc_flag
    0x00, //sniff_hwagc_flag
    0x00, //blescan_hwagc_flag
    0x00, //bleinit_hwagc_flag
#ifdef __NEW_SWAGC_MODE__
    0x01, //bt_sync_swagc_en
#else
    0x00, //bt_sync_swagc_en
#endif
    0x00, //le_sync_swagc_en
    0x00, //fa_to_en
#ifdef __BES_FA_MODE__
    0x01, //fa_dsb_en
#else
    0x00, //fa_dsb_en
#endif
    0x0F, //fa_to_num
#ifdef __FIX_FA_RX_GAIN___
    0x01, //fa_rxgain
#else
    0xff, //fa_rxgain
#endif
    0x55,0x00,0x00,0x00, //fa_to_type
    0x50,0x00,0x00,0x00, //fa_disable_type
    0xff, //fa_txpwr
    0x00, //ecc_data_flag
    0x00, //softbit_data_flag
    0x00, //rx_noise_chnl_assess_en
    0x14, //rx_noise_thr_good
    0x0a, //rx_noise_thr_bad
    0x00, //snr_chnl_assess_en
    0x14, //snr_good_thr_value
    0x28, //snr_bad_thr_value
    0x00, //rssi_maxhold_record_en
    0x00, //new_agc_adjust_dbm
    0x0f, //ble_rssi_noise_thr
    0x01, //ble_rxgain_adjust_once
    0x00, //trig_open_pcm_flag
    0x00, //sco_sw_mute_en
};

const uint8_t bt_common_setting_1603_t1[12] =
{
    120,//tws_acl_prio_in_sco
    144,//tws_acl_prio_in_normal
    10,//scan_evt_win_slot_in_a2dp
    32,//pscan_gap_slot_in_a2dp
    20,//page_gap_slot_in_a2dp
    0,//send_tws_interval_to_peer
    0,//softbit_enable
    1,//ble_adv_buf_malloc
#ifdef __AFH_ASSESS__
    0,//afh_assess_old
#else
    1,//afh_assess_old
#endif
    1,//cpu_idle_set
    0x40,0x05,//sniff_interval_max
};

const uint8_t bt_txrx_gain_setting[] =
{
    BT_INIT_TX_PWR_IDX,//bt_init_txpwr
    0x01,//bt_inq_rxgain
    0x01,//bt_page_rxgain
    0x06,//bt_page_txpwr
    0x01,//bt_iscan_rxgain
    0xff,//bt_iscan_txpwr
    0x01,//bt_pscan_rxgain
    0xff,//bt_pscan_txpwr
    0x00,//bt_ibrt_rxgain
    0xff,//ble_adv_txpwr
    0x00,//ble_adv_rxgain
    0xff,//ble_bis_txpwr
    0x00,//ble_bis_rxgain
    0xff,//ble_adv_per_txpwr
    0x00,//ble_adv_per_rxgain
    0xff,//ble_testmode_txpwr
    0x00,//ble_testmode_rxgain
    0xff,//ble_con_txpwr
    0x00,//ble_con_rxgain
    0xff,//ble_con_init_txpwr
    0x00,//ble_con_init_rxgain
    0xff,//ble_scan_txpwr
    0x00,//ble_scan_rxgain
};

const uint8_t bt_peer_txpwr_dft_thr[]=
{
    0x64,00,//uint16_t rssi_avg_nb_pkt;
    -1,//rssi_high_thr;
    -2,//rssi_low_thr;
    5,//rssi_below_low_thr;
    50,//unused rssi_interf_thr;
};

const struct rssi_txpower_link_thd tws_link_txpwr_thd =
{
    0x14,//uint16_t rssi_avg_nb_pkt;
    -40,//rssi_high_thr;
    -50,//rssi_low_thr;
    5,//rssi_below_low_thr;
    50,//unused rssi_interf_thr;
};

const uint8_t bt_sw_rssi_setting[] =
{
     0,  //.sw_rssi_en = false
    80,00,00,00,//.link_agc_thd_mobile = 80,
    100,00,00,00,//.link_agc_thd_mobile_time = 100,
    80,00,00,00,//.link_agc_thd_tws = 80,
    100,00,00,00,//.link_agc_thd_tws_time = 100,
    3,//.rssi_mobile_step = 3,
    3,//.rssi_tws_step = 3,
    -100,//.rssi_min_value_mobile = -100,
    -100,//.rssi_min_value_tws = -100,

    0,//.ble_sw_rssi_en = 0,
    80,00,00,00,//.ble_link_agc_thd = 80,
    100,00,00,00,//.ble_link_agc_thd_time = 100,//(in BT half-slots)
    3,//.ble_rssi_step = 3,
    -100,//.ble_rssidbm_min_value = -100,

    1,//.bt_no_sync_en = 1,
    -90,//.bt_link_no_sync_rssi= -90,
    80,00,//.bt_link_no_snyc_thd = 0x50,
    200,00,//.bt_link_no_sync_timeout = 200,

    1,//.ble_no_sync_en = 1,
    -90,//.ble_link_no_sync_rssi= -90,
    20,00,//.ble_link_no_snyc_thd = 20,
    0x20,0x03,//.ble_link_no_sync_timeout = 800,
};

struct bt_cmd_chip_config_t g_bt_drv_btstack_chip_config = {
    HCI_DBG_SET_SYNC_CONFIG_CMD_OPCODE,
    HCI_DBG_SET_SCO_SWITCH_CMD_OPCODE,
};

#ifdef __BESTRX_SUPPORT__
static void btdrv_bestrx_en(void)
{
    struct hci_dbg_set_bestrx_en_cmd param;

    param.enable = true;
    param.mode = 0;

    btdrv_send_cmd(HCI_DBG_SET_BESTRX_EN_CMD_OPCODE, sizeof(struct hci_dbg_set_bestrx_en_cmd),(const uint8_t *)&param);
}

void btdrv_bestrx_rf_timin_config(void)
{
    struct hci_dbg_set_bestrx_rf_setting_cmd param;
    param.offset = BESTRX_OFFSET;
    param.txsetuptime = BESTRX_TXPWRUP_CNT;
    param.rxsetuptime = BESTRX_RXPWRUP_CNT;
    param.rxwin = BESTRX_MASTER_RX_WINSIZE;
    param.ifs = BESTRX_TIFS;

    btdrv_send_cmd(HCI_DBG_SET_BESTRX_RF_SETTING_CMD_OPCODE, sizeof(struct hci_dbg_set_bestrx_rf_setting_cmd),(const uint8_t *)&param);
}

void btdrv_bestrx_config_init(void)
{
    btdrv_bestrx_en();
    btdrv_bestrx_rf_timin_config();
}
#endif

const uint8_t normal_sync_pos[]=
{
    34,//normal_sync_pos;
};

static const BTDRV_CFG_TBL_STRUCT btdrv_cfg_tbl[] = {
    {BTDRV_CONFIG_ACTIVE,HCI_RD_LOCAL_VER_INFO_CMD_OPCODE, 0, NULL},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE,sizeof(local_feature),local_feature},
#if (defined(CTKD_ENABLE)&&defined(IS_CTKD_OVER_BR_EDR_ENABLED))
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_LOCAL_EX_FEATURE_CMD_OPCODE,sizeof(local_ex_feature_page1),local_ex_feature_page1},
#endif
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_SETTING_CMD_OPCODE,sizeof(bt_common_setting_1603),bt_common_setting_1603},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_SCHE_SETTING_CMD_OPCODE,sizeof(bt_sche_setting_1603),bt_sche_setting_1603},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_IBRT_SETTING_CMD_OPCODE,sizeof(bt_ibrt_setting_1603),bt_ibrt_setting_1603},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_HW_FEAT_SETTING_CMD_OPCODE,sizeof(bt_hw_feat_setting_1603),bt_hw_feat_setting_1603},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,189,(uint8_t *)&btdrv_rf_env_1603},
#ifdef _SCO_BTPCM_CHANNEL_
    {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_SYNC_CONFIG_CMD_OPCODE,sizeof(sync_config),(uint8_t *)&sync_config},
    {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_PCM_SETTING_CMD_OPCODE,sizeof(pcm_setting),(uint8_t *)&pcm_setting},
#endif
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_LOCAL_EX_FEATURE_CMD_OPCODE,sizeof(local_ex_feature_page2),(uint8_t *)&local_ex_feature_page2},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_RF_TIMING_CMD_OPCODE,sizeof(bt_rf_timing),(uint8_t *)&bt_rf_timing},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BLE_RF_TIMING_CMD_OPCODE,sizeof(ble_rf_timing),(uint8_t *)&ble_rf_timing},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_RSSI_TX_POWER_DFT_THR_CMD_OPCODE,sizeof(bt_peer_txpwr_dft_thr),(uint8_t *)&bt_peer_txpwr_dft_thr},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_BT_COMMON_SETTING_T1_CMD_OPCODE,sizeof(bt_common_setting_1603_t1),(uint8_t *)&bt_common_setting_1603_t1},
#ifdef __BW2M_ENABLE__
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_NORMAL_SYNC_POS_CMD_OPCODE,sizeof(normal_sync_pos),(uint8_t *)&normal_sync_pos},
#endif
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_BLE_TXRX_GAIN_CMD_OPCODE,sizeof(bt_txrx_gain_setting),(uint8_t *)bt_txrx_gain_setting},
};

/*
*   TWOSC: Time to wake-up osc_en before deepsleep_time expiration
*   TWRM  : Time to wake-up radio module(no used)
*   TWEXT : Time to wake-up osc_en on external wake-up request
*/
static void btdrv_hci_init_sleep_wakeup_param(void)
{
    uint16_t wait_26m_cnt = BT_CMU_26M_READY_TIMEOUT_US;
    uint16_t twosc_cnt = BT_CMU_OSC_READY_TIMEOUT_US;
    uint16_t twext_cnt = BT_CMU_WEXT_READY_TIMEOUT_US;

    struct hci_dbg_set_sleep_para_cmd sleep_wakeup_param;
    sleep_wakeup_param.twrm = twosc_cnt; //no used
    sleep_wakeup_param.twosc = twosc_cnt;
    sleep_wakeup_param.twext = twext_cnt;
    sleep_wakeup_param.rwip_prog_delay = IP_PROG_DELAY_DFT;
    sleep_wakeup_param.clk_corr = 2;
    sleep_wakeup_param.wait_26m_cnt_us = wait_26m_cnt;
    sleep_wakeup_param.cpu_idle_en = 1;
    sleep_wakeup_param.reserved = 0;//no used
    sleep_wakeup_param.poweroff_flag = BTC_LP_MODE;

    btdrv_send_cmd(HCI_DBG_SET_WAKEUP_TIME_CMD_OPCODE, sizeof(struct hci_dbg_set_sleep_para_cmd),(const uint8_t *)&sleep_wakeup_param);

    BT_DRV_TRACE(1, "%s,wait26m cycle=%d,twosc=%d",__func__, wait_26m_cnt, twosc_cnt);
}

void btdrv_fa_txpwrup_timing_setting(uint8_t txpwrup)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL2_ADDR, 0xff, 24, txpwrup);
}

void btdrv_fa_rxpwrup_timig_setting(uint8_t rxpwrup)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL2_ADDR, 0xff, 16, rxpwrup);
}

void btdrv_fa_margin_timig_setting(uint8_t margin)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL2_ADDR, 0x1f, 11, (margin&0x1f));
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 0x7, 27, ((margin&0xe0)>>5));
}

void bt_fa_sync_invert_en_setf(uint8_t faacinven)
{
    BTDIGITAL_REG_WR(BT_BES_FACNTL0_ADDR,
                     (BTDIGITAL_REG(BT_BES_FACNTL0_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)faacinven << 17));
}

void btdrv_fast_lock_en_setf(uint8_t rxonextenden)
{
    BTDIGITAL_REG_WR(BT_BES_FACNTL0_ADDR,
                     (BTDIGITAL_REG(BT_BES_FACNTL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)rxonextenden << 30));
}

void btdrv_fatx_pll_pre_on_en(uint8_t en)
{
    BTDIGITAL_REG_WR(BT_BES_CNTL5_ADDR,
                     (BTDIGITAL_REG(BT_BES_CNTL5_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)en << 31));
}

void btdrv_2m_band_wide_sel(uint8_t fa_2m_mode)
{
   BTDIGITAL_REG_WR(BT_BES_FACNTL0_ADDR,
                    (BTDIGITAL_REG(BT_BES_FACNTL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)fa_2m_mode << 0));
}

void btdrv_48m_sys_enable(void)
{
    //BT select 48M system,osc x2
    BTDIGITAL_REG(CMU_CLKREG_ADDR)|=(1<<7);
}

void btdrv_24m_sys_enable(void)
{
    //BT select 24M system,osc
    BTDIGITAL_REG(CMU_CLKREG_ADDR) &= ~(1<<7);
}

void btdrv_fa_new_mode_corr_thr(uint8_t corr_thr)
{
    BTDIGITAL_REG_SET_FIELD(0xd0350228, 0x7f, 16, (corr_thr & 0x7f));
}

void btdrv_fa_old_mode_corr_thr(uint8_t corr_thr)
{
    BTDIGITAL_REG_SET_FIELD(0xd0350368, 0xf, 4, (corr_thr & 0xf));
}


void btdrv_fa_config_vendor_syncword_en(bool enable)
{
    if(enable)
    {
        BTDIGITAL_REG(BT_BES_FACNTL1_ADDR) |= (1<<31);
    }
    else
    {
        BTDIGITAL_REG(BT_BES_FACNTL1_ADDR) &= ~(1<<31);
    }
}

void btdrv_fa_config_vendor_syncword_content(uint32_t syncword_high, uint32_t syncword_low, uint8_t syncword_len)
{
    if(syncword_low == INVALID_SYNC_WORD || syncword_high == INVALID_SYNC_WORD)
    {
        BT_DRV_TRACE(0,"BT_DRV:vendor syncword invalid");
        return;
    }

    if(syncword_len == FA_SYNCWORD_32BIT)
    {
        BTDIGITAL_REG(BT_BES_FA_SWREG1_ADDR) = syncword_low;
    }
    else if(syncword_len == FA_SYNCWORD_64BIT)
    {
        BTDIGITAL_REG(BT_BES_FA_SWREG1_ADDR) = syncword_high;
        BTDIGITAL_REG(BT_BES_FA_SWREG0_ADDR) = syncword_low;
    }
}

void btdrv_fa_syncword_phy_setting(uint8_t syncword_len)
{
    //set FA RX delay in modem
    BTDIGITAL_REG_SET_FIELD(0xD035020C, 0XFF, 8, PHY_RX_DELAY);
    //set FA TX delay in modem
    BTDIGITAL_REG_SET_FIELD(0xD0350210, 0XFF, 8, PHY_TX_DELAY);

    if (syncword_len == FA_SYNCWORD_32BIT)
    {
        //PHY using 32 bit FA
        BTDIGITAL_REG_SET_FIELD(0xD0350228, 0x3, 29, 0x3);
        BTDIGITAL_REG_SET_FIELD(0xD03501C8, 0xf, 24, 0x2);
        BTDIGITAL_REG_SET_FIELD(0xD03501C8, 0x1ff, 15, 0x96);
        BTDIGITAL_REG_SET_FIELD(0xD03501C8, 0x7ff, 0, 0x32);
        BTDIGITAL_REG_WR(0xD0350048, 0x024B0032);
    }
    else if (syncword_len == FA_SYNCWORD_64BIT)
    {
        //PHY using 64 bit FA
        BTDIGITAL_REG_SET_FIELD(0xD0350228, 0x3, 29, 0x0);
        BTDIGITAL_REG_SET_FIELD(0xD03501C8, 0xf, 24, 0x8);
        BTDIGITAL_REG_SET_FIELD(0xD03501C8, 0x1ff, 15, 0x12C);
        BTDIGITAL_REG_SET_FIELD(0xD03501C8, 0x7ff, 0, 0x64);
        BTDIGITAL_REG_WR(0xD0350048, 0x38960064);
    }
    else
    {
        ASSERT(0, "[%s] len=%d", __func__, syncword_len);
    }
}

void btdrv_fa_config_syncword_mode(uint8_t syncword_len)
{
    if(syncword_len == FA_SYNCWORD_32BIT)
    {
        BTDIGITAL_REG(0xd0220c8c) &= ~(1<<28);//syncword 32bit
    }
    else if(syncword_len == FA_SYNCWORD_64BIT)
    {
        BTDIGITAL_REG(0xd0220c8c) |= (1<<28);//syncword 64bit
    }
}

void btdrv_fa_config_tx_gain(bool ecc_tx_gain_en, uint8_t ecc_tx_gain_idx)//false :disable tx gain
{
    if(ecc_tx_gain_en == true)
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) |= (1<<29);
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xf, 25, (ecc_tx_gain_idx & 0xf));
    }
    else
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) &= ~(1<<29);
    }
}

void btdrv_fa_config_rx_gain(bool ecc_rx_gain_en, uint8_t ecc_rx_gain_idx)//false: disable rx gain
{
    if(ecc_rx_gain_en == true)
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) |= (1<<30);
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xf, 21, (ecc_rx_gain_idx & 0xf));
    }
    else
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) &= ~(1<<30);
    }
}

void btdrv_fa_rx_winsz(uint8_t ecc_rx_winsz)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (ecc_rx_winsz & 0x7f));
}

void btdrv_fa_tx_preamble_enable(bool fa_tx_preamble_en)
{
    if(fa_tx_preamble_en)
    {
        BTDIGITAL_REG(0xD0220C18) |= (1<<16); // fa tx 8 bit preamble
    }
    else
    {
        BTDIGITAL_REG(0xD0220C18) &= ~(1<<16);
    }
}

void btdrv_fa_freq_table_enable(bool fa_freq_table_en)
{
    if(fa_freq_table_en)
    {
        BTDIGITAL_REG(0xD0220C80) |= (1<<14);
    }
    else
    {
        BTDIGITAL_REG(0xD0220C80) &= ~(1<<14);
    }
}

void btdrv_fa_multi_mode0_enable(bool fa_multi_mode0_en)
{
    if(fa_multi_mode0_en)
    {
        BTDIGITAL_REG(0xD0220C18) |= (1<<24);
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (0x66 & 0x7f));
    }
    else
    {
        BTDIGITAL_REG(0xD0220C18) &= ~(1<<24);
    }
}

void btdrv_fa_multi_mode1_enable(bool fa_multi_mode1_en, uint8_t fa_multi_tx_count)
{
    if(fa_multi_mode1_en)
    {
        BTDIGITAL_REG(0xD0220C18) |= (1<<25);
        if(fa_multi_tx_count == 2)
        {
            BTDIGITAL_REG(0xD0220C18) |= 0x00800000;
            BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (0x31 & 0x7f));
        }

        if(fa_multi_tx_count == 3)
        {
            BTDIGITAL_REG(0xD0220C18) |= 0x00c00000;
            BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (0x5b & 0x7f));
        }
    }
    else
    {
        BTDIGITAL_REG(0xD0220C18) &= ~(1<<25);
    }
}

void btdrv_fa_freq_table_config(uint32_t fa_freq_table0,uint32_t fa_freq_table1,uint32_t fa_freq_table2)
{
    BTDIGITAL_REG(0xD0220C40)  = fa_freq_table0; // freq 0~31
    BTDIGITAL_REG(0xD0220C44)  = fa_freq_table1; // freq 32~63
    BTDIGITAL_REG(0xD0220C48) |= fa_freq_table2; // freq 64~78
}

void btdrv_fa_basic_config(btdrv_fa_basic_config_t* p_fa_basic_config)
{
    if(p_fa_basic_config != NULL)
    {
        //fast ack TX power gain set
        btdrv_fa_config_tx_gain(p_fa_basic_config->fa_tx_gain_en, p_fa_basic_config->fa_tx_gain_idx);
#ifdef __FIX_FA_RX_GAIN___
        //fix fast ack rx gain
        btdrv_fa_config_rx_gain(p_fa_basic_config->fa_rx_gain_en, p_fa_basic_config->fa_rx_gain_idx);
#endif
        // fa 2M mode select
        btdrv_2m_band_wide_sel(p_fa_basic_config->fa_2m_mode);
        //fa syncword mode
        btdrv_fa_config_syncword_mode(p_fa_basic_config->syncword_len);
        //fa phy setting
        btdrv_fa_syncword_phy_setting(p_fa_basic_config->syncword_len);

        //ECC vendor syncword mode
        if(p_fa_basic_config->fa_vendor_syncword_en)
        {
            btdrv_fa_config_vendor_syncword_en(true);
            btdrv_fa_config_vendor_syncword_content(p_fa_basic_config->syncword_high,
                                                    p_fa_basic_config->syncword_low,
                                                    p_fa_basic_config->syncword_len);
        }
        else
        {
            btdrv_fa_config_vendor_syncword_en(false);
        }
        //fa win size
        btdrv_fa_rx_winsz(p_fa_basic_config->fa_rx_winsz);

        if(p_fa_basic_config->is_new_corr)
        {
            btdrv_fa_new_mode_corr_thr(p_fa_basic_config->new_mode_corr_thr);
        }
        else
        {
            btdrv_fa_old_mode_corr_thr(p_fa_basic_config->old_mode_corr_thr);
        }

        if(p_fa_basic_config->fa_tx_preamble_en)
        {
            btdrv_fa_tx_preamble_enable(true);
        }
        else
        {
            btdrv_fa_tx_preamble_enable(false);
        }

        if(p_fa_basic_config->fa_freq_table_en)
        {
            btdrv_fa_freq_table_enable(true);
            btdrv_fa_freq_table_config(p_fa_basic_config->fa_freq_table0,
                                       p_fa_basic_config->fa_freq_table1,
                                       p_fa_basic_config->fa_freq_table2);
        }
        else
        {
            btdrv_fa_freq_table_enable(false);
        }

        if(p_fa_basic_config->fa_multi_mode0_en)
        {
            btdrv_fa_multi_mode0_enable(true);
        }
        else
        {
            btdrv_fa_multi_mode0_enable(false);
        }

        if(p_fa_basic_config->fa_multi_mode1_en)
        {
            btdrv_fa_multi_mode1_enable(true, p_fa_basic_config->fa_multi_tx_count);
        }
        else
        {
            btdrv_fa_multi_mode1_enable(false,p_fa_basic_config->fa_multi_tx_count);
        }
    }
    //enable fa invert
    bt_fa_sync_invert_en_setf(BT_FA_INVERT_EN);
}

#ifdef __FASTACK_ECC_ENABLE__
//ecc usert data
void btdrv_ecc_config_len_mode_sel(uint8_t ecc_len_mode) //only for ecc 1 block
{
    if(ecc_len_mode == ECC_8_BYTE_MODE)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7, 0, 1);
    }
    else if(ecc_len_mode == ECC_16_BYTE_MODE)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7, 0, 0);
    }
    else if(ecc_len_mode == ECC_12_BYTE_MODE)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7, 0, 2);
    }
}

void btdrv_ecc_config_usr_tx_dat_set(ecc_trx_dat_t* p_ecc_trx_dat)
{
    BTDIGITAL_REG(0xd0220cd0) = p_ecc_trx_dat->trx_dat.dat_arr[0];
    BTDIGITAL_REG(0xd0220cd4) = p_ecc_trx_dat->trx_dat.dat_arr[1];
    BTDIGITAL_REG(0xd0220cd8) = p_ecc_trx_dat->trx_dat.dat_arr[2];
    BTDIGITAL_REG(0xd0220cdc) = p_ecc_trx_dat->trx_dat.dat_arr[3];
}

void btdrv_ecc_config_usr_rx_dat_get(ecc_trx_dat_t* p_ecc_trx_dat)
{
    p_ecc_trx_dat->trx_dat.dat_arr[0] = BTDIGITAL_REG(0xd0220cd0);
    p_ecc_trx_dat->trx_dat.dat_arr[1] = BTDIGITAL_REG(0xd0220cd4);
    p_ecc_trx_dat->trx_dat.dat_arr[2] = BTDIGITAL_REG(0xd0220cd8);
    p_ecc_trx_dat->trx_dat.dat_arr[3] = BTDIGITAL_REG(0xd0220cdc);
}


void btdrv_ecc_disable_spec_pkt_type(uint32_t ptk_type) // 1 -> disable FA, ptk type enum in bt_drv_1600_internal.h
{
    BTDIGITAL_REG(0xd0220c30) |= (1<<20);
    BTDIGITAL_REG(0xd0220c30) = (ptk_type & 0xfffff);
}


void btdrv_ecc_config_blk_mode(uint8_t ecc_blk_mode)
{
    if (ecc_blk_mode == ECC_1BLOCK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, 0xef);
    }
    else if (ecc_blk_mode == ECC_2BLOCK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, 0x1de);
    }
    else if (ecc_blk_mode == ECC_3BLOCK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, 0x2cd);
    }
}

void btdrv_ecc_config_modu_mode_acl(uint8_t ecc_modu_mode_acl)
{
    if(ecc_modu_mode_acl == ECC_8PSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, 3); //ECC 8PSK
    }
    else if(ecc_modu_mode_acl == ECC_DPSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, 2); //ECC DPSK
    }
    else if(ecc_modu_mode_acl == ECC_GFSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, 1); //ECC GFSK
    }
}

void btdrv_ecc_config_modu_mode_sco(uint8_t ecc_modu_mode_sco)
{
    if(ecc_modu_mode_sco == ECC_8PSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 3, 1, 3); //ECC 8PSK
    }
    else if(ecc_modu_mode_sco == ECC_DPSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 3, 1, 2); //ECC DPSK
    }
    else if(ecc_modu_mode_sco == ECC_GFSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 3, 1, 1); //ECC GFSK
    }
}

void btdrv_sco_ecc_enable(bool enable)
{
    if(enable == true)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 1, 8, 1);//sco ecc flag, disabled by default
    }
    else
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 1, 8, 0);//sco ecc flag, disabled by default
    }
}

void btdrv_acl_ecc_enable(bool enable)
{
    if(enable == true)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL5_ADDR, 1, 1, 1);//acl ecc flag, disabled by default
    }
    else
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL5_ADDR, 1, 1, 0);//acl ecc flag, disabled by default
    }
}


void btdrv_ecc_enable(bool enable)
{
    //ecc all flag, initialize all configurations
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL5_ADDR, 1, 1, 0);//acl ecc flag, disabled by default
    //to enable acl ecc, please use btdrv_acl_ecc_enable later
    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 1, 8, 0);//sco ecc flag, disabled by default
    //to enable sco ecc, please use btdrv_sco_ecc_enable later

    if(enable == true)
    {
        BTDIGITAL_REG(BT_BES_CNTL5_ADDR) |= 1;//ecc enable
#ifdef __FASTACK_SCO_ECC_ENABLE__
        btdrv_sco_ecc_enable(true);
#endif
#ifdef __FASTACK_ACL_ECC_ENABLE__
        btdrv_acl_ecc_enable(true);
#endif
    }
    else
    {
        BTDIGITAL_REG(BT_BES_CNTL5_ADDR) &= ~1;//disable
    }
}

void btdrv_ecc_basic_config(btdrv_ecc_basic_config_t* p_ecc_basic_config)
{
    if((p_ecc_basic_config != NULL) && (p_ecc_basic_config->ecc_mode_enable == true))
    {
        btdrv_ecc_enable(true);
        btdrv_ecc_config_modu_mode_acl(p_ecc_basic_config->ecc_modu_mode_acl);
        btdrv_ecc_config_modu_mode_sco(p_ecc_basic_config->ecc_modu_mode_sco);
        btdrv_ecc_config_blk_mode(p_ecc_basic_config->ecc_blk_mode);
        btdrv_ecc_config_len_mode_sel(p_ecc_basic_config->ecc_len_mode_sel);
    }
}


void btdrv_bid_ecc_2dh5_enable(void)
{
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 1, 15, 1);
}

void btdrv_bid_ecc_cnt_act_1(uint8_t regcnteccact1)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xff, 0, regcnteccact1);
}

void btdrv_bid_ecc_cnt_act_2(uint8_t regcnteccact2)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xff, 8, regcnteccact2);
}

void btdrv_bid_ecc_frame_len(uint16_t regeccframelen)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, regeccframelen);
}

void btdrv_bid_ecc_bid_ecc_blk_en(uint8_t bideccen)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_ENHPCM_CNTL_ADDR, 1, 5, bideccen);
}

void btdrv_ecc_bid_enable(void)
{
    BTDIGITAL_REG_SET_FIELD(0xd0220c8c, 1, 29, 1);
}

void btdrv_ecc_bid_pkt_2dh_5_reduce_byte(uint8_t pkt2dh5reducebyte)
{
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 0x7f, 18, pkt2dh5reducebyte);
}

void btdrv_ecc_bid_fa_rate_mode_acl(uint8_t regfaratemodeacl)
{

    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, regfaratemodeacl);
}

void btdrv_bid_ecc_basic_config(void)
{
    btdrv_bid_ecc_2dh5_enable();
    btdrv_bid_ecc_cnt_act_1(205);
    btdrv_bid_ecc_cnt_act_2(250);
    btdrv_bid_ecc_frame_len(690);
    btdrv_ecc_bid_pkt_2dh_5_reduce_byte(78);
    btdrv_bid_ecc_bid_ecc_blk_en(1);
    btdrv_ecc_bid_fa_rate_mode_acl(3);
    btdrv_ecc_bid_enable();
}

void btdrv_ecc_content_config(void)
{
    btdrv_ecc_basic_config_t ecc_config;
    //ECC  config
    ecc_config.ecc_mode_enable = true;
    ecc_config.ecc_modu_mode_acl = ECC_MODU_MODE;
    ecc_config.ecc_modu_mode_sco = ECC_8PSK;
    ecc_config.ecc_blk_mode = ECC_BLK_MODE;
    ecc_config.ecc_len_mode_sel = ECC_8_BYTE_MODE;
    //setting
    btdrv_ecc_basic_config(&ecc_config);

#if __BT_BID_ECC_EN__
    btdrv_bid_ecc_basic_config();
#endif
}
#endif

void btdrv_fast_ack_config(void)
{
    btdrv_fa_basic_config_t fa_config;
    //fast ack config
#ifdef mHDT_SUPPORT
    fa_config.syncword_len = FA_SYNCWORD_32BIT;
    fa_config.fa_2m_mode = true;
#else
    fa_config.syncword_len = FA_SYNCWORD_64BIT;
    fa_config.fa_2m_mode = false;
#endif

    if(fa_config.fa_2m_mode)
    {
        btdrv_fa_txpwrup_timing_setting(FA_BW2M_TXPWRUP_TIMING);
        btdrv_fa_rxpwrup_timig_setting(FA_BW2M_RXPWRUP_TIMING);
    }
    else
    {
        btdrv_fa_txpwrup_timing_setting(FA_TXPWRUP_TIMING);
        btdrv_fa_rxpwrup_timig_setting(FA_RXPWRUP_TIMING);
    }
    fa_config.fa_vendor_syncword_en = false;
    fa_config.syncword_high = INVALID_SYNC_WORD;
    fa_config.syncword_low = INVALID_SYNC_WORD;
    fa_config.is_new_corr = false;
    fa_config.old_mode_corr_thr = FA_OLD_CORR_VALUE;
    fa_config.new_mode_corr_thr = FA_NEW_CORR_VALUE;
    fa_config.fa_tx_gain_en = true;
    fa_config.fa_tx_gain_idx = FA_FIX_TX_GIAN_IDX;
    fa_config.fa_rx_winsz = FA_RX_WIN_SIZE;
#ifdef __FIX_FA_RX_GAIN___
    fa_config.fa_rx_gain_en = true;
    fa_config.fa_rx_gain_idx = FA_FIX_RX_GIAN_IDX;
#endif
    fa_config.enhance_fa_mode_en = false;
    fa_config.fa_tx_preamble_en = true;
    fa_config.fa_freq_table_en = false;
    fa_config.fa_freq_table0 = INVALID_FA_FREQ_TABLE;
    fa_config.fa_freq_table1 = INVALID_FA_FREQ_TABLE;
    fa_config.fa_freq_table2 = INVALID_FA_FREQ_TABLE;
    fa_config.fa_multi_mode0_en = false;
    fa_config.fa_multi_mode1_en = false;
    fa_config.fa_multi_tx_count = FA_MULTI_TX_COUNT;

    //setting
    btdrv_fa_basic_config(&fa_config);
}

void btdrv_fa_timing_init(void)
{
    btdrv_fa_margin_timig_setting(FA_CNT_PKT_US);
    BTDIGITAL_REG(BT_BES_CNTL5_ADDR) &= ~1;//disable
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 7, 15, 0);//disable bid ecc en
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 1, 1, 1);//trig_lock_mode
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 7, 9, 0);//clear fa mt en
    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 1, 29, 0);//clear bid ecc en
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL5_ADDR, 1, 8, 0);//clear sbc en
    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 1, 30, 0);//clear farx always on
    TRACE(0,"fa timing=0x%x",BTDIGITAL_REG(BT_TRIGREG_ADDR));
}

void btdrv_ecc_config(void)
{
    btdrv_fa_timing_init();

    btdrv_fast_ack_config();
#ifdef __FASTACK_ECC_ENABLE__
    btdrv_ecc_content_config();
#endif
}

void btdrv_afh_monitor_config(void)
{
    BTDIGITAL_REG_SET_FIELD(BT_AFH_MONITOR_ADDR,0x7f,16,1);//win_spacing
    BTDIGITAL_REG_SET_FIELD(BT_AFH_MONITOR_ADDR,0x7f,8,50);//win_size
}

static void btdrv_enable_mcu2bt1_isr_en(void)
{
    //[3]:mcu2bt1 isr en
    BTDIGITAL_REG_SET_FIELD(0xD03300a0,1,3,1);
}

static void btdrv_enable_slave_loop_p_en(void)
{
    //[30]:crash after skipping CS update in slave sniff
    BTDIGITAL_REG_SET_FIELD(0xD0220C38,1,30,1);//enable slave loop avoid controll crash
}

static inline void btdrv_rampdn_cnt(void)
{
    BTDIGITAL_REG_SET_FIELD(0xD0220C14, 0x7F, 0, 0x6);
}

/**
 * @brief HDT_4M_TXPATHDLY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23       mode_timing_en   0
 *  22:16         bt_txpwrdn4m   0x0
 *  15:08         bt_txpwrup4m   0x0
 *  06:00       bt_txpathdly4m   0x0
 * </pre>
 */
#define BT_HDT_4M_TXPATHDLY_ADDR   0xD0220C60
__INLINE void btdrv_bt_hdt_4m_txpathdly_pack(uint8_t modetimingen, uint8_t bttxpwrdn4m, uint8_t bttxpwrup4m, uint8_t bttxpathdly4m)
{
    ASSERT_ERR((((uint32_t)modetimingen << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)bttxpwrdn4m << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)bttxpwrup4m << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)bttxpathdly4m << 0) & ~((uint32_t)0x0000007F)) == 0);
    BTDIGITAL_REG_WR(BT_HDT_4M_TXPATHDLY_ADDR,  ((uint32_t)modetimingen << 23) | ((uint32_t)bttxpwrdn4m << 16) | ((uint32_t)bttxpwrup4m << 8) | ((uint32_t)bttxpathdly4m << 0));
}

/**
 * @brief HDT_4M_RXPATHDLY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16         bt_syncpos4m   0x0
 *  15:08         bt_rxpwrup4m   0x0
 *  06:00       bt_rxpathdly4m   0x0
 * </pre>
 */
#define BT_HDT_4M_RXPATHDLY_ADDR   0xD0220C64
__INLINE void btdrv_bt_hdt_4m_rxpathdly_pack(uint8_t btsyncpos4m, uint8_t btrxpwrup4m, uint8_t btrxpathdly4m)
{
    ASSERT_ERR((((uint32_t)btsyncpos4m << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)btrxpwrup4m << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)btrxpathdly4m << 0) & ~((uint32_t)0x0000007F)) == 0);
    BTDIGITAL_REG_WR(BT_HDT_4M_RXPATHDLY_ADDR,  ((uint32_t)btsyncpos4m << 16) | ((uint32_t)btrxpwrup4m << 8) | ((uint32_t)btrxpathdly4m << 0));
}


/**
 * @brief HDT_2M_TXPATHDLY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  22:16         bt_txpwrdn2m   0x0
 *  15:08         bt_txpwrup2m   0x0
 *  06:00       bt_txpathdly2m   0x0
 * </pre>
 */
#define BT_HDT_2M_TXPATHDLY_ADDR   0xD0220C68
__INLINE void btdrv_bt_hdt_2m_txpathdly_pack(uint8_t bttxpwrdn2m, uint8_t bttxpwrup2m, uint8_t bttxpathdly2m)
{
    ASSERT_ERR((((uint32_t)bttxpwrdn2m << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)bttxpwrup2m << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)bttxpathdly2m << 0) & ~((uint32_t)0x0000007F)) == 0);
    BTDIGITAL_REG_WR(BT_HDT_2M_TXPATHDLY_ADDR,  ((uint32_t)bttxpwrdn2m << 16) | ((uint32_t)bttxpwrup2m << 8) | ((uint32_t)bttxpathdly2m << 0));
}

/**
 * @brief HDT_2M_RXPATHDLY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16         bt_syncpos2m   0x0
 *  15:08         bt_rxpwrup2m   0x0
 *  06:00       bt_rxpathdly2m   0x0
 * </pre>
 */
#define BT_HDT_2M_RXPATHDLY_ADDR   0xD0220C6C
__INLINE void btdrv_bt_hdt_2m_rxpathdly_pack(uint8_t btsyncpos2m, uint8_t btrxpwrup2m, uint8_t btrxpathdly2m)
{
    ASSERT_ERR((((uint32_t)btsyncpos2m << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)btrxpwrup2m << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)btrxpathdly2m << 0) & ~((uint32_t)0x0000007F)) == 0);
    BTDIGITAL_REG_WR(BT_HDT_2M_RXPATHDLY_ADDR,  ((uint32_t)btsyncpos2m << 16) | ((uint32_t)btrxpwrup2m << 8) | ((uint32_t)btrxpathdly2m << 0));
}

/**
 * @brief BLE_4M_TXPATHDLY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23       mode_timing_en   0
 *  15:08         le_txpwrdn4m   0x0
 *  06:00       le_txpathdly4m   0x0
 * </pre>
 */
#define BLE_HDT_4M_TXPATHDLY_ADDR   0xD0220D78
__INLINE void btdrv_ble_hdt_4m_txpathdly_pack(uint8_t modetimingen, uint8_t bttxpwrup4m, uint8_t bttxpathdly4m)
{
    ASSERT_ERR((((uint32_t)modetimingen << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)bttxpwrup4m << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)bttxpathdly4m << 0) & ~((uint32_t)0x0000007F)) == 0);
    BTDIGITAL_REG_WR(BLE_HDT_4M_TXPATHDLY_ADDR,  ((uint32_t)modetimingen << 23) | ((uint32_t)bttxpwrup4m << 8) | ((uint32_t)bttxpathdly4m << 0));
}

/**
 * @brief BLE_4M_RXPATHDLY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:24        le_rxrate4cfg   0x0
 *  23:16       le_rxpathdly4m   0x0
 *  15:08         le_rxpwrup4m   0x0
 *  06:00         le_syncpos4m   0x0
 * </pre>
 */
#define BLE_HDT_4M_RXPATHDLY_ADDR   0xD0220D7C
__INLINE void btdrv_ble_hdt_4m_rxpathdly_pack(uint8_t unknown, uint8_t btsyncpos4m, uint8_t btrxpwrup4m, uint8_t btrxpathdly4m)
{
    ASSERT_ERR((((uint32_t)unknown << 24) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)btsyncpos4m << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)btrxpwrup4m << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)btrxpathdly4m << 0) & ~((uint32_t)0x0000007F)) == 0);
    BTDIGITAL_REG_WR(BLE_HDT_4M_RXPATHDLY_ADDR,  (((uint32_t)unknown << 24) | (uint32_t)btsyncpos4m << 16) | ((uint32_t)btrxpwrup4m << 8) | ((uint32_t)btrxpathdly4m << 0));
}

void btdrv_digital_common_config(void)
{
#if defined(__HOST_GEN_ECDH_KEY__)
    BTDIGITAL_REG(0xd0220050) = 0x99b1;//close BTC gerate public key
#endif
    //[21] tx pwr dr
    BTDIGITAL_REG_SET_FIELD(0xD0220C04,1,21,0);//disable tx power dr
    //[22] swagc gain dr
    BTDIGITAL_REG_SET_FIELD(0xD0220C04,1,22,0);//disable swagc gain dr

    //[28] null ack stop sco retx en
    BTDIGITAL_REG_SET_FIELD(0xD0220C18, 0x1, 28, 1);

    btdrv_bt_hdt_4m_txpathdly_pack(1, 0x08, HDT_TXPWRUP_CNT, 0x06);            // BW4M TX ON & RX TIMING
    btdrv_bt_hdt_4m_rxpathdly_pack(0, HDT_RXPWRUP_CNT, 0);                     // BW4M TX ON & RX TIMING
    btdrv_bt_hdt_2m_txpathdly_pack(0x08, HDT_TXPWRUP_CNT, 0x08);               // BW2M TX ON & RX TIMING
    btdrv_bt_hdt_2m_rxpathdly_pack(0, HDT_RXPWRUP_CNT, 0);                     // BW2M TX ON & RX TIMING
    btdrv_ble_hdt_4m_txpathdly_pack(1, 0x2A, 0x08);                 // LE4M TX ON & RX TIMING
    btdrv_ble_hdt_4m_rxpathdly_pack(1, 0x0A, 0x41, 0);              // LE4M TX ON & RX TIMING

#ifdef __NEW_SWAGC_MODE__
    //sync agc guard update en
    BTDIGITAL_REG_SET_FIELD(0xD0220C2C,1,25,1);
    //sync agc guard update time
    BTDIGITAL_REG_SET_FIELD(0xD0220C2C,0x3f,26,20);
#endif

    btdrv_rampdn_cnt();

#ifdef BT_LOG_POWEROFF
    hal_psc_bt_enable_auto_power_down();
#endif
    btdrv_enable_mcu2bt1_isr_en();
    //enable slave loop avoid controll crash
    btdrv_enable_slave_loop_p_en();

    BTDIGITAL_REG_SET_FIELD(BLE_RADIOCNTL3_ADDR,3,14,0);
    BTDIGITAL_REG_SET_FIELD(BLE_RADIOCNTL3_ADDR,3,12,0);
#ifdef __AFH_ASSESS__
    btdrv_afh_monitor_config();
#endif
    //dac dec en
    BTDIGITAL_REG_SET_FIELD(0xD03500B0, 1, 0, 1);
    BTDIGITAL_REG_SET_FIELD(0xD0330038, 1, 12, 1);  //use trailing edge get adc number
}

void btdrv_ble_modem_config(void)
{
    //add ble modem config here
}

static void btdrv_digital_tx_2M3M_gfsk_power_config(uint8_t enable, uint16_t gsg_den, uint16_t gsg_nom)
{
    enable = !!enable;

    // txpwr digital gain
    // ig_gain=(nom/(2^den))
    BTDIGITAL_REG_SET_FIELD(0xd0350370, 0x1,   23, enable);             //[23]    gsg_edr_en
    BTDIGITAL_REG_SET_FIELD(0xd0350370, 0x7,   20, gsg_den);            //[22:20] gsg_edr_den
    BTDIGITAL_REG_SET_FIELD(0xd0350370, 0x3ff, 10, gsg_nom);            //[19:10] gsg_edr_nom_q
    BTDIGITAL_REG_SET_FIELD(0xd0350370, 0x3ff,  0, gsg_nom);            //[9:0]   gsg_edr_nom_i
}

static void btdrv_digital_tx_gfsk_power_config(uint16_t gsg_den, uint16_t gsg_nom)
{
    BTDIGITAL_REG_SET_FIELD(0xd0350300, 0xfff, 20, 0x3ff);
    BTDIGITAL_REG_SET_FIELD(0xd0350308, 0xfff, 20, 0x401);

    BTDIGITAL_REG_SET_FIELD(0xd0350300, 0x7,   0,  gsg_den);            //[3:0]     gsg_edr_den
    BTDIGITAL_REG_SET_FIELD(0xd0350308, 0x3ff, 0,  gsg_nom);            //[9:0]     gsg_psk_nom
    BTDIGITAL_REG_SET_FIELD(0xd0350308, 0x3ff, 10, gsg_nom);            //[19:10]   gsg_psk_nom
}

static void btdrv_digital_tx_psk_power_config(uint16_t dsg_den, uint16_t dsg_nom)
{
    BTDIGITAL_REG_SET_FIELD(0xd0350340, 0x7,   0, dsg_den);            //[3:0] dsg_edr_den
    BTDIGITAL_REG_SET_FIELD(0xd0350344, 0x3ff, 0, dsg_nom);            //[9:0] dsg_psk_nom
}

static void btdrv_digital_tx_power_config(void)
{
    btdrv_digital_tx_2M3M_gfsk_power_config(RF_DIGTAL_TX_2M3M_GFSK_ENABLE, RF_DIGTAL_TX_2M3M_GFSK_DEN, RF_DIGTAL_TX_2M3M_GFSK_NOM_0DBM);
    btdrv_digital_tx_gfsk_power_config(RF_DIGTAL_TX_GFSK_DEN, RF_DIGTAL_TX_GFSK_NOM);
    btdrv_digital_tx_psk_power_config(RF_DIGTAL_TX_PSK_DEN, RF_DIGTAL_TX_PSK_NOM);
}

void btdrv_bt_modem_config(void)
{
    BT_DRV_TRACE(1,"%s",__func__);
    //[16]:iqswap
    BTDIGITAL_REG_SET_FIELD(0xd0350214, 0x1, 16, 0);
    //[18]:txqsigned
    BTDIGITAL_REG_SET_FIELD(0xd0350214, 0x1, 18, 0);
    //[19]:txisigned
    BTDIGITAL_REG_SET_FIELD(0xd0350214, 0x1, 19, 0);

    //bit[3]=1'b1 txdac_fifo_bypass
    BTDIGITAL_REG_SET_FIELD(0xd0350324, 0x1, 3, 1);

    BTDIGITAL_REG_SET_FIELD(0xD0350210, 0xFFFF, 0, 0x2016);
    BTDIGITAL_REG_SET_FIELD(0xD0350254, 0x1F, 0, 0xB);

    // iq_swap
    BTDIGITAL_REG_SET_FIELD(0xD0350240, 0x1, 2, 1);

    // set 1603 Sync by ZZD
    BTDIGITAL_REG_WR(0xD03501BC, 0x381F8064);
    BTDIGITAL_REG_WR(0xD03501C0, 0xE1A1072C);
    BTDIGITAL_REG_WR(0xD03501C4, 0x334B0032);
    BTDIGITAL_REG_WR(0xD03501D0, 0x02A8C314);
    BTDIGITAL_REG_WR(0xD03501B4, 0x04B2584B);

    BTDIGITAL_REG_WR(0xD0350010, 0x3F100064);
    BTDIGITAL_REG_WR(0xD0350014, 0xE1A1412C);
    BTDIGITAL_REG_WR(0xD0350044, 0x334B0032);
    BTDIGITAL_REG_WR(0xD03500DC, 0x04B0A2CC);

    BTDIGITAL_REG_SET_FIELD(0xD0350004, 0x7, 28, 0x5);  //modify k1 parameter of AHI demod.

    BTDIGITAL_REG_SET_FIELD(0xD03502C8, 0x7F, 0, 0x66);
    BTDIGITAL_REG_SET_FIELD(0xD035036C, 0xffff, 0, 0x1000);
    BTDIGITAL_REG_SET_FIELD(0xD035036C, 0xffff, 16, 0x0480);

    //rx iq cal
    BTDIGITAL_REG_WR(0xD0350040, 0x00810115);
    BTDIGITAL_REG_WR(0xD035005C, 0x014E1010);
    BTDIGITAL_REG_WR(0xD0350120, 0x00C80032);

    BTDIGITAL_REG_SET_FIELD(0xD03502C8, 0x7F, 0, 0x66);

    //set adc clk 244MHz
    BTDIGITAL_REG_WR(0xd0350248, 0x00401B4F);       //BW1M, 244M
    BTDIGITAL_REG_WR(0xd0350240, 0x0DA7A801);
    BTDIGITAL_REG_WR(0xd0350244, 0x02AB1555);       // 0xD0350244[15:0] = 0x1555
    BTDIGITAL_REG_WR(0xd035023c, 0x0006D3A0);       // 0xD0350240[31:16] = 0x0DA7
    BTDIGITAL_REG_WR(0xD035015C, 0x00000029);       //244m-488m-DIG-OSR12

    btdrv_digital_tx_power_config();
    // rx bw2M
    BTDIGITAL_REG_WR(0xD035014C, 0x00401B4F);       //BW2M, 488M
    BTDIGITAL_REG_WR(0xD0350154, 0x0006D3A0);
    BTDIGITAL_REG_WR(0xD0350148, 0x1000147B);

    BTDIGITAL_REG_WR(0xd03500D0, 0x10000A3E);       //OSR12
    BTDIGITAL_REG_WR(0xD0350140, 0x00000000);       //OSR12
    BTDIGITAL_REG_WR(0xD0350150, 0x00401B4F);       //BW4M, 488M, 0SR12
    BTDIGITAL_REG_WR(0xD0350158, 0x0006D3A0);
    BTDIGITAL_REG_WR(0xD03500C0, 0x00000007);       // BT psd filter enable，0xD03500C0[3] = 0x0;
    BTDIGITAL_REG_WR(0xD03500D4, 0x038A0312);       // BW2M&4M EDR time，
    BTDIGITAL_REG_WR(0xD03502C4, 0x08E1FAD6);       // BW1M EDR time，0xD03502C4[10:0] = 0x2D6;
#ifdef mHDT_SUPPORT
    BTDIGITAL_REG_WR(0xd03500D0, 0x1000147B);       //OSR12
    BTDIGITAL_REG_WR(0xD0350140, 0x00000002);       //OSR12
    BTDIGITAL_REG_WR(0xD0350150, 0x00000DA7);       //BW4M, 488M, 0SR12
    BTDIGITAL_REG_WR(0xD0350158, 0x000369D0);
#endif  //mHDT_SUPPORT

#ifdef __HW_AGC__
    BTDIGITAL_REG_WR(0xD03502C0, 0x00100087);       // 0xD03502C0[20] = 0x1,  enable HWAGC RRCGAIN

    BTDIGITAL_REG_WR(0xD03500E0, 0x03C63087);
    BTDIGITAL_REG_WR(0xD0350138, 0x039C0180);
    BTDIGITAL_REG_WR(0xD035013C, 0x054005DC);
    BTDIGITAL_REG_WR(0xD0350180, 0x04B00360);
    BTDIGITAL_REG_WR(0xD03500E4, 0x10000090);
    BTDIGITAL_REG_WR(0xD03500EC, 0x20000060);
    BTDIGITAL_REG_WR(0xD03500F0, 0x20004055);
    BTDIGITAL_REG_WR(0xD03500F4, 0x20000060);
    BTDIGITAL_REG_WR(0xD03500F8, 0x200040AB);
    BTDIGITAL_REG_WR(0xD03500FC, 0x08000060);
    BTDIGITAL_REG_WR(0xD0350100, 0x08004000);
    BTDIGITAL_REG_WR(0xD0350160, 0x10000090);
    BTDIGITAL_REG_WR(0xD0350164, 0x103FF836);
    BTDIGITAL_REG_WR(0xD0350168, 0x10000090);
    BTDIGITAL_REG_WR(0xD035016C, 0x103FF836);
    BTDIGITAL_REG_WR(0xD0350178, 0x20000060);
    BTDIGITAL_REG_WR(0xD035017C, 0x200040AB);

    BTDIGITAL_REG_WR(0xD0350104, 0x001441D4);
    BTDIGITAL_REG_WR(0xD0350108, 0x005E80E4);
    BTDIGITAL_REG_WR(0xD03501AC, 0x000001F8);
#ifdef mHDT_SUPPORT
    BTDIGITAL_REG_WR(0xD03500E0, 0x03C63087);
    BTDIGITAL_REG_WR(0xD0350108, 0x005E81C8);
    BTDIGITAL_REG_WR(0xD03501A8, 0x042002E8);
    BTDIGITAL_REG_WR(0xD03501AC, 0x000003F0);
#endif  //mHDT_SUPPORT
#endif  //__HW_AGC__

#ifdef __MHDT_SWAGC__
    BTDIGITAL_REG_SET_FIELD(0xD0350150, 0xff, 0, 0xa7);
    BTDIGITAL_REG_SET_FIELD(0xD0350150, 0xff, 8, 0x0d);
    BTDIGITAL_REG_SET_FIELD(0xD0350150, 0xff, 16, 0x0);
    BTDIGITAL_REG_SET_FIELD(0xD0350150, 1, 24, 0);

    BTDIGITAL_REG_SET_FIELD(0xD0350158, 0xff, 0, 0xd0);
    BTDIGITAL_REG_SET_FIELD(0xD0350158, 0xff, 8, 0x69);
    BTDIGITAL_REG_SET_FIELD(0xD0350158, 0xf, 16, 0x3);

    BTDIGITAL_REG_SET_FIELD(0xD0350148, 0xff, 0, 0x7b);
    BTDIGITAL_REG_SET_FIELD(0xD0350148, 0xff, 8, 0x14);
    BTDIGITAL_REG_SET_FIELD(0xD0350148, 0xff, 16, 0);
    BTDIGITAL_REG_SET_FIELD(0xD0350148, 0xff, 24, 0x10);
#endif
}


void bt_dump_data_config(bool data_sign, bool data_swap , bool output_enable)
{
    BTDIGITAL_REG(0xD0350218) &= ~((1 << 8) | (1 << 12) | (1 << 7));

    if(data_sign){
        BTDIGITAL_REG(0xD0350218) |= (1 << 8);
    }
    if(data_swap){
        BTDIGITAL_REG(0xD0350218) |= (1 << 12);
    }
    if(output_enable){
        BTDIGITAL_REG(0xD0350218) |= (1 << 7);
    }
}

void iq_dump_data_choice(enum IQDUMP_DATA_CHOICE_FLAG_T flag)
{
    uint8_t temp, bit16 = 0;
    temp = flag & 0xF;
    bit16 = ((flag & 0x10000) >> 16);

    BTDIGITAL_REG_SET_FIELD(0xD0350218, 0x1, 10, 1);
    BTDIGITAL_REG_SET_FIELD(0xD0350218, 0x1, 16, bit16);
    BTDIGITAL_REG_SET_FIELD(0xD0350218, 0xF, 0, temp);
}

const uint32_t data_backup_tbl[] =
{
    0xd0350210,
//  0xd0350214,
//  0xD0350240,
    0xD0350254,
    0xd0350324,
};

void btdrv_config_end(void)
{
    BTDIGITAL_REG(0xD0330024) &= (~(1<<5));
    BTDIGITAL_REG(0xD0330024) |= (1<<18);
#ifdef BT_LOG_POWEROFF
    bt_drv_reg_op_data_bakeup_init();
    bt_drv_reg_op_data_backup_write(&data_backup_tbl[0],sizeof(data_backup_tbl)/sizeof(data_backup_tbl[0]));
#endif
#ifdef BT_ACTIVE_OUTPUT
    hal_iomux_set_bt_active_out();
#endif
#ifdef BT_SYSTEM_52M
    btdrv_48m_sys_enable();
#else
    btdrv_24m_sys_enable();
#endif
}

void btdrv_hciprocess(void)
{
    BT_DRV_TRACE(1,"%s", __func__);

    btdrv_hci_init_sleep_wakeup_param();

    for(uint8_t i=0; i<sizeof(btdrv_cfg_tbl)/sizeof(btdrv_cfg_tbl[0]); i++){
        //BT other config
        if(btdrv_cfg_tbl[i].is_act == BTDRV_CONFIG_ACTIVE){
            btdrv_send_cmd(btdrv_cfg_tbl[i].opcode,btdrv_cfg_tbl[i].parlen,btdrv_cfg_tbl[i].param);
#ifdef NORMAL_TEST_MODE_SWITCH
            btdrv_delay(20);
#else
            btdrv_delay(1);
#endif
        }
    }
#ifdef __BESTRX_SUPPORT__
    btdrv_bestrx_config_init();
#endif
}

void btdrv_digital_init(void)
{
    BT_DRV_TRACE(1,"%s", __func__);

    btdrv_digital_common_config();

    btdrv_bt_modem_config();

    btdrv_ble_modem_config();

    btdrv_ecc_config();

    btdrv_txpower_calib();
}

void btdrv_config_init(void)
{
    BT_DRV_TRACE(1,"%s", __func__);

    btdrv_digital_init();
}

bool btdrv_is_ecc_enable(void)
{
    bool ret = false;
#ifdef  __FASTACK_ECC_ENABLE__
    ret = true;
#endif
    return ret;
}

////////////////////////////////////////test mode////////////////////////////////////////////

void btdrv_sleep_config(uint8_t sleep_en)
{
    sleep_param[0] = sleep_en;
    btdrv_send_cmd(HCI_DBG_SET_SLEEP_SETTING_CMD_OPCODE,8,sleep_param);
    btdrv_delay(1);
}

void btdrv_feature_default(void)
{
#ifdef __EBQ_TEST__
    const uint8_t feature[] = {0xBF, 0xFE, 0xCF,0xFe,0xdb,0xFF,0x5b,0x87};
#else
    const uint8_t feature[] = {0xBF, 0xeE, 0xCD,0xFe,0xdb,0xFf,0x7b,0x87};
#endif
    btdrv_send_cmd(HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE,8,feature);
    btdrv_delay(1);
}

const struct rssi_txpower_link_thd* btdrv_get_tws_link_txpwr_thd_ptr(void)
{
    return &tws_link_txpwr_thd;
}
