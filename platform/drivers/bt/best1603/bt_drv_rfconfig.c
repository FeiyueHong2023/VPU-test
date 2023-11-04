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
#include <string.h>
#include "pmu.h"
#include "hal_chipid.h"
#ifdef __RF_INIT_XTAL_CAP_FROM_NV__
#include "nvrecord_dev.h"
#endif
#include "bt_drv.h"
#include "bt_drv_interface.h"
#include "bt_drv_internal.h"
#include "bt_drv_reg_op.h"

//CHIP related
#include "bt_drv_1603_internal.h"
#include "bt_drv_1603_config.h"

static enum HAL_CHIP_METAL_ID_T bt_metal_id;
struct TX_PWR_SELECT_ITEM
{
    float pwr_dbm;
    uint16_t tx_flt;
    uint16_t rf_vco;
    uint8_t pa_cfg;
};

#define TX_PWR_DPD_ENABLE       (1 << 4)
#define TX_PWR_DPD_DISABLE      (0 << 4)

#define TX_PWR_0P6_PA_DOHERTY           (1 << 0)
#define TX_PWR_1P1_PA_DOHERTY           (1 << 1)
#define TX_PWR_1P8_PA                   (1 << 2)
#define TX_PWR_1P8_PA_DOHERTY           (1 << 3)

#define BT_Rf_REG_DPD_SEL                  0x3AB

static const struct TX_PWR_SELECT_ITEM tx_pwr_select_normal[] =
{
    {16,  0x0216, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //16dbm
    {15,  0x0212, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //15dbm
    {14,  0x020F, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //14dbm
    {13,  0x020D, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //13dbm
    {12,  0x020C, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //12dbm
    {11,  0x0158, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //11dbm
    {10,  0x014D, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //10dbm
    {9,   0x014B, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //9dbm
    {8,   0x0149, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //8dbm
    {7,   0x014D, 0x5387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //7dbm
    {6,   0x014A, 0x5387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //6dbm
    {5,   0x0148, 0x5387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //5dbm
    {4,   0x004F, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //4dbm
    {3,   0x0146, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //3dbm
    {2,   0x0148, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //2dbm
    {1,   0x0147, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //1dbm
    {0,   0x0145, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //0dbm
    {-1,  0x0144, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-1dbm
    {-2,  0x0047, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-2dbm
    {-3,  0x0046, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-3dbm
    {-4,  0x0045, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-4dbm
    {-5,  0x0044, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-5dbm
    {-6,  0x0043, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-6dbm
    {-7,  0x00D6, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-7dbm
    {-8,  0x00D6, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-8dbm
    {-9,  0x0042, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-9dbm
    {-10, 0x0042, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-10dbm
    {-11, 0x00CE, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-11dbm
    {-12, 0x00CC, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-12dbm
    {-13, 0x0041, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-13dbm
    {-14, 0x00C9, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-14dbm
    {-15, 0x00C8, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-15dbm
    {-16, 0x00C7, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-16dbm
    {-17, 0x00C6, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-17dbm
    {-18, 0x00C5, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-18dbm
    {-19, 0x00C4, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-19dbm
    {-20, 0x00C4, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-20dbm
    {-21, 0x00C3, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-21dbm
};

static const struct TX_PWR_SELECT_ITEM tx_pwr_select_high_freq_low_freq[] =
{
    {16,  0x0216, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //16dbm
    {15,  0x0212, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //15dbm
    {14,  0x020F, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //14dbm
    {13,  0x020D, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //13dbm
    {12,  0x020C, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA)},            //12dbm
    {11,  0x0158, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //11dbm
    {10,  0x014D, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //10dbm
    {9,   0x014B, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //9dbm
    {8,   0x0149, 0x9387, (TX_PWR_DPD_ENABLE  | TX_PWR_1P8_PA_DOHERTY)},    //8dbm
    {7,   0x014D, 0x5387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //7dbm
    {6,   0x014A, 0x5387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //6dbm
    {5,   0x0148, 0x5387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //5dbm
    {4,   0x004F, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //4dbm
    {3,   0x0146, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_1P1_PA_DOHERTY)},    //3dbm
    {2,   0x0148, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //2dbm
    {1,   0x0147, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //1dbm
    {0,   0x0145, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //0dbm
    {-1,  0x0144, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-1dbm
    {-2,  0x0047, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-2dbm
    {-3,  0x0046, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-3dbm
    {-4,  0x0045, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-4dbm
    {-5,  0x0044, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-5dbm
    {-6,  0x0043, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-6dbm
    {-7,  0x00D6, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-7dbm
    {-8,  0x00D6, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-8dbm
    {-9,  0x0042, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-9dbm
    {-10, 0x0042, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-10dbm
    {-11, 0x00CE, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-11dbm
    {-12, 0x00CC, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-12dbm
    {-13, 0x0041, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-13dbm
    {-14, 0x00C9, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-14dbm
    {-15, 0x00C8, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-15dbm
    {-16, 0x00C7, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-16dbm
    {-17, 0x00C6, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-17dbm
    {-18, 0x00C5, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-18dbm
    {-19, 0x00C4, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-19dbm
    {-20, 0x00C4, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-20dbm
    {-21, 0x00C3, 0x3387, (TX_PWR_DPD_DISABLE | TX_PWR_0P6_PA_DOHERTY)},    //-21dbm
};

static uint16_t xtal_fcap = DEFAULT_XTAL_FCAP;
static uint16_t init_xtal_fcap = DEFAULT_XTAL_FCAP;

struct RF_SYS_INIT_ITEM
{
    uint16_t reg;
    uint16_t set;
    uint16_t mask;
    uint16_t delay;
};

static const struct RF_SYS_INIT_ITEM rf_sys_init_tbl[] =
{

};

const uint16_t rf_init_tbl_1[][3] =
{
    {0x0001, 0x0101, 1},//timing t1
    {0x0006, 0x0C01, 1},//timing t1
    {0x0011, 0x4FD3, 1},//the pulse of a reference clock in a PLL
    {0x0012, 0xB2AD, 1},//rfpll bw
    {0x001F, 0x1400, 1},//rfpll_cnt_time
    {0x0014, 0x6666, 1},//vctrl initial
    {0x001F, 0x1400, 1},
    {0x0021, 0x004A, 1},//pll capbank cal init delay
    {0x0028, 0x4400, 1},//dither bypass
    {0x0037, 0x88BE, 1},
    {0x0038, 0x7F00, 1},
    {0x0041, 0x0000, 1},
    {0x0081, 0x0040, 1},
    {0x0082, 0x4010, 1},
    {0x0084, 0x0003, 1},//adc vref 11
    {0x008C, 0x0000, 1},
    {0x008D, 0x0000, 1},
    {0x008E, 0x0000, 1},
    {0X0090, 0X4DC1, 1},
    {0x0093, 0x4494, 1},// close tst buffer
    {0x0097, 0x4EDC, 1},// Optimized for wifi interference
    {0x00B1, 0x0AAC, 1},// increase tmx lo drv & tmx cap sel
    {0x00B2, 0x4006, 1},
    {0x00B3, 0x130E, 1},// padrv cap sel
    {0x0154, 0x0000, 1},
    {0x0155, 0x0000, 1},
    {0x0165, 0x2AA6, 1},//idn
    {0x0166, 0x0006, 1},//aac cal bit
    {0x0203, 0x0400, 1},
    {0x0205, 0x0203, 1},//rfpll precharge time
    {0x0207, 0x0204, 1},//rfvco precharge time
    {0x022B, 0x3540, 1},//ofst
    {0x026B, 0x0058, 1},//aac cal init value
    {0x0274, 0x0117, 1},//turn off hw agc
    {0x0369, 0x8617, 1},// tx_gain_table vco setting
    {0x0381, 0x0833, 1},//rfvco ibit dr
    {0x0379, 0xA2EF, 1},//rfvco vldo dr
    {0x037A, 0x3197, 1},//rxvco ibit
    {0x037E, 0x1400, 1},//bit[12:9] wirte 1010,Improve the drive ability of VCO divider when ADC runs 488MHz clock
    {0x0381, 0x0633, 1},//rfvco en pmos dr
    {0x0382, 0x3A10, 1},//adc op1 ibias
    {0x0383, 0x0910, 1},//adc op2 ibias
    {0x0384, 0x1DE0, 1},//dc dvdd dig
    {0x0385, 0x00DA, 1},//adc dvdd clk
    {0x0386, 0x00DA, 1},//adc dvdd dac
    {0x0388, 0x03A2, 1},//adc notch
    {0x038A, 0x3c4c, 1},//adc_cap_2M_1st
    {0x038C, 0x1E3C, 1},//adc_cap_2nd_2m
    {0x038E, 0x0038, 1},// CPLX ADC IF
    {0x038D, 0x01B6, 1},//adc cplx on
    {0x0391, 0x003E, 1},//adc iquan
    {0x0392, 0x150A, 1},//adc_s2p_2M
    {0x03A5, 0x0040, 1},//release reg_filter_cap_dr
    {0x03A6, 0x2d40, 1},// BW1M & BW2M filter cap
    {0x03AB, 0x3000, 1},
    {0x03AC, 0x6D9C, 1},//padrv vcm tune
    // ble 2M i2v_cap
    {0x0223, 0x001C, 1},
    {0x0224, 0x001C, 1},
    {0x0225, 0x001C, 1},
    {0x0226, 0x001C, 1},
    {0x0227, 0x001C, 1},
    {0x0228, 0x001C, 1},
    {0x0229, 0x001C, 1},
    {0x022A, 0x001C, 1},
#ifdef BT_ITN_SGL_LNA
    {0x0094, 0x7E80, 1},//lna_hg,lna_att_cap
    {0x0095, 0x5E13, 1},//hg_en,att,ic
    {0x0096, 0x4418, 1},//lna_ipatt
    {0x0097, 0x4ADC, 1},//MXER CURRENT
    {0x0098, 0x0151, 1},
    {0x03b0, 0x03ff, 1},
    {0x03b1, 0x2D91, 1},//comp_bypass
    {0x032f, 0x0002, 1},
#else
    {0x0094, 0x3E00, 1},//lna_hg,lna_att_cap
    {0x0095, 0x2011, 1},//hg_en,att,ic
    {0x0096, 0x440C, 1},//lna_ipatt
    {0x0097, 0x4EEC, 1},//MXER CURRENT
    {0x0098, 0x0111, 1},
    {0x03B1, 0x2D90, 1},//comp_bypass
#endif
#ifdef BT_ITN_SGL_LNA
    //bt gain table
    //rx gain table 0
    {0x0055, 0x0CC4, 1},
    {0x0056, 0x00C5, 1},
    {0x0057, 0x0CCE, 1},
    {0x0058, 0x0CCD, 1},
    {0x0059, 0x0CCC, 1},
    {0x005A, 0x0CC4, 1},
    {0x005B, 0x00C4, 1},
    {0x005C, 0x00A2, 1},
    //rx gain table 1
    {0x004D, 0x8E1C, 1},
    {0x004E, 0x882D, 1},
    {0x004F, 0x812D, 1},
    {0x0050, 0x812D, 1},
    {0x0051, 0x812D, 1},
    {0x0052, 0x8130, 1},
    {0x0053, 0x8130, 1},
    {0x0054, 0x8130, 1},
    //rx gain tzble 2
    {0x010C, 0x038B, 1},
    {0x010D, 0x038B, 1},
    {0x010E, 0x0003, 1},
    {0x010F, 0x0023, 1},
    {0x0110, 0x0043, 1},
    {0x0111, 0x0063, 1},
    {0x0112, 0x0073, 1},
    {0x0113, 0x0073, 1},
    //ble gain table
    {0x011C, 0x0CC6, 1},
    {0x011D, 0x00C4, 1},
    {0x011E, 0x00C3, 1},
    {0x011F, 0x0CC4, 1},
    {0x0120, 0x00C3, 1},
    {0x0121, 0x00BB, 1},
    {0x0122, 0x00B3, 1},
    {0x0123, 0x008A, 1},
    //rxgain 0
    {0x0124, 0x8E1C, 1},
    {0x0125, 0x8E2D, 1},
    {0x0126, 0x8E2D, 1},
    {0x0127, 0x812D, 1},
    {0x0128, 0x8130, 1},
    {0x0129, 0x8130, 1},
    {0x012A, 0x8130, 1},
    {0x012B, 0x8130, 1},
    //rxgain 1
    {0x0104, 0x038C, 1},
    {0x0105, 0x038B, 1},
    {0x0106, 0x030B, 1},
    {0x0107, 0x0003, 1},
    {0x0108, 0x0003, 1},
    {0x0109, 0x0003, 1},
    {0x010A, 0x0003, 1},
    {0x010B, 0x0003, 1},
    //rxgain 2
#else
    //bt gain table
    //rx gain table 0
    {0x0055, 0x0CC3, 1},
    {0x0056, 0x00C3, 1},
    {0x0057, 0x0CCD, 1},
    {0x0058, 0x08CD, 1},
    {0x0059, 0x04CC, 1},
    {0x005A, 0x04C4, 1},
    {0x005B, 0x04C3, 1},
    {0x005C, 0x0093, 1},
    //rx gain table 1
    {0x004D, 0x8E2D, 1},
    {0x004E, 0x8E2D, 1},
    {0x004F, 0x802D, 1},
    {0x0050, 0x802D, 1},
    {0x0051, 0x802D, 1},
    {0x0052, 0x802D, 1},
    {0x0053, 0x802D, 1},
    {0x0054, 0x802D, 1},
    //rx gain table 2
    {0x010C, 0x040B, 1},
    {0x010D, 0x040B, 1},
    {0x010E, 0x0003, 1},
    {0x010F, 0x0023, 1},
    {0x0110, 0x0023, 1},
    {0x0111, 0x0043, 1},
    {0x0112, 0x0063, 1},
    {0x0113, 0x0073, 1},
    //ble gain table
    {0x011C, 0x0CCC, 1},
    {0x011D, 0x00CC, 1},
    {0x011E, 0x00BC, 1},
    {0x011F, 0x00AC, 1},
    {0x0120, 0x00A4, 1},
    {0x0121, 0x00CC, 1},
    {0x0122, 0x00BC, 1},
    {0x0123, 0x008B, 1},
    //rxgain 0
    {0x0124, 0x8E2D, 1},
    {0x0125, 0x8E2D, 1},
    {0x0126, 0x8E2D, 1},
    {0x0127, 0x8E2D, 1},
    {0x0128, 0x862D, 1},
    {0x0129, 0x802D, 1},
    {0x012A, 0x802D, 1},
    {0x012B, 0x802D, 1},
    //rxgain 1
    {0x0104, 0x078C, 1},
    {0x0105, 0x078C, 1},
    {0x0106, 0x078C, 1},
    {0x0107, 0x078C, 1},
    {0x0108, 0x078C, 1},
    {0x0109, 0x0404, 1},
    {0x010A, 0x0404, 1},
    {0x010B, 0x0402, 1},
    //rxgain2
#endif

#ifdef __HW_AGC__
#ifdef BT_ITN_SGL_LNA
    //gain0
    {0x0288, 0x0CC4, 1},
    {0x0289, 0x8E1C, 1},
    {0x028A, 0x038B, 1},
    //gain1
    {0x028B, 0x00C5, 1},
    {0x028C, 0x882D, 1},
    {0x028D, 0x038B, 1},
    //gain2
    {0x028E, 0x0CCE, 1},
    {0x028F, 0x812D, 1},
    {0x0290, 0x0003, 1},
    //gain3
    {0x0291, 0x0CCD, 1},
    {0x0292, 0x812D, 1},
    {0x0293, 0x0023, 1},
    //gain4
    {0x0294, 0x0CCC, 1},
    {0x0295, 0x812D, 1},
    {0x0296, 0x0043, 1},
    //gain5
    {0x0297, 0x0CC4, 1},
    {0x0298, 0x8130, 1},
    {0x0299, 0x0063, 1},
    //gain6
    {0x029A, 0x00C4, 1},
    {0x029B, 0x8130, 1},
    {0x029C, 0x0073, 1},
    //gain7
    {0x029D, 0x00A2, 1},
    {0x029E, 0x8130, 1},
    {0x029F, 0x0073, 1},
    //gain offset
    {0x02A0, 0x3338, 1},
    {0x02A1, 0x2A30, 1},
    {0x02A2, 0x2025, 1},
    {0x02A3, 0x0A1D, 1},
    //jump point
    {0x02A4, 0xACA5, 1},
    {0x02A5, 0xB5B1, 1},
    {0x02A6, 0xBFBA, 1},
    {0x0287, 0xD300, 1},
    //[slna hwagc-2M-xbz]
    //gain0
    {0x02A8,0x0CC4, 1},
    {0x02A9,0x8E14, 1},
    {0x02AA,0x038B, 1},
    //gain1
    {0x02AB,0x00C5, 1},
    {0x02AC,0x8814, 1},
    {0x02AD,0x038B, 1},
    //gain2
    {0x02AE,0x0CCE, 1},
    {0x02AF,0x8114, 1},
    {0x02B0,0x0003, 1},
    //gain3
    {0x02B1,0x0CCD, 1},
    {0x02B2,0x8114, 1},
    {0x02B3,0x0023, 1},
    //gain4
    {0x02B4,0x0CCC, 1},
    {0x02B5,0x8114, 1},
    {0x02B6,0x0043, 1},
    //gain5
    {0x02B7,0x0CC4, 1},
    {0x02B8,0x8114, 1},
    {0x02B9,0x0063, 1},
    //gain6
    {0x02BA,0x00C4, 1},
    {0x02BB,0x8114, 1},
    {0x02BC,0x0073, 1},
    //gain7
    {0x02BD,0x00A2, 1},
    {0x02BE,0x8114, 1},
    {0x02BF,0x0073, 1},
    //gain offset
    {0x02C0, 0x3238, 1},
    {0x02C1, 0x2A2F, 1},
    {0x02C2, 0x2025, 1},
    {0x02C3, 0x0A1B, 1},
    //jump point
    {0x02C4, 0xACA5, 1},
    {0x02C5, 0xB5B1, 1},
    {0x02C6, 0xBFBA, 1},
    {0x02C7, 0x00D3, 1},
    //mode jump offset
    {0x0284, 0x0000, 1},
    {0x0285, 0xFD0A, 1},
    {0x0286, 0x0003, 1},
    {0x02FA, 0x0306, 1},
    {0x02FB, 0x0306, 1},
    {0x02FC, 0x0D10, 1},
    {0x02FD, 0x0303, 1},
    // hwagc control
    {0x0274, 0x8126, 1},

    {0x0276, 0x00CC, 1},    //wait fast rssi
    {0x0277, 0x0090, 1},
    {0x0278, 0x006C, 1},
    {0x0279, 0x021C, 1},
    {0x027A, 0x0006, 1},
    {0x027B, 0x0270, 1},    //wait EDR guard time，52.5us
    {0x027C, 0x00CC, 1},    //wait slow rssi
    {0x027D, 0x0090, 1},
    {0x027E, 0x006C, 1},
    {0x027F, 0x021C, 1},
    {0x0281, 0x0603, 1},    //rssi diff ble2m & lr
    //2M gain_table_sel
    {0x0283, 0x0100, 1},
    {0x02E8, 0x0378, 1},

    {0x02E9, 0x01C8, 1},    // wait rf stable, bw1m
    {0x02EA, 0x0168, 1},
    {0x02EB, 0x0168, 1},

    {0x02EC, 0x0FC1, 1},

    {0x02ED, 0x006C, 1},    //wait fast rssi
    {0x02EE, 0x00CC, 1},
    {0x02EF, 0x00CC, 1},
    {0x02F0, 0x000C, 1},
    {0x02F1, 0x0018, 1},
    {0x02F7, 0x006C, 1},    //wait slow rssi
    {0x02F8, 0x00CC, 1},
    {0x02F9, 0x00CC, 1},
#else
    //[HWAGC-RF-DLNA-R2]
    //gain0
    {0x0288, 0x0CB3, 1},
    {0x0289, 0x8E2D, 1},
    {0x028A, 0x040B, 1},
    //gain1
    {0x028B, 0x0CAB, 1},
    {0x028C, 0x802D, 1},
    {0x028D, 0x020B, 1},
    //gain2
    {0x028E, 0x0CA3, 1},
    {0x028F, 0x802D, 1},
    {0x0290, 0x010B, 1},
    //gain3
    {0x0291, 0x0CBB, 1},
    {0x0292, 0x802D, 1},
    {0x0293, 0x0013, 1},
    //gain4
    {0x0294, 0x0CBB, 1},
    {0x0295, 0x802D, 1},
    {0x0296, 0x0053, 1},
    //gain5
    {0x0297, 0x0CAB, 1},
    {0x0298, 0x802D, 1},
    {0x0299, 0x0063, 1},
    //gain6
    {0x029A, 0x0C9B, 1},
    {0x029B, 0x802D, 1},
    {0x029C, 0x0073, 1},
    //gain7
    {0x029D, 0x0C8B, 1},
    {0x029E, 0x802D, 1},
    {0x029F, 0x0073, 1},
    //gain offset
    {0x02A0, 0x2933, 1},
    {0x02A1, 0x2022, 1},
    {0x02A2, 0x161C, 1},
    {0x02A3, 0x090F, 1},
    //jump point
    {0x02A4, 0xACA8, 1},
    {0x02A5, 0xB6B1, 1},
    {0x02A6, 0xC0BB, 1},
    {0x0287, 0xC600, 1},
    //[slna hwagc-2M-xbz]
    //gain0
    {0x02A8,0x0CC4, 1},
    {0x02A9,0x8E14, 1},
    {0x02AA,0x038B, 1},
    //gain1
    {0x02AB,0x00C5, 1},
    {0x02AC,0x8814, 1},
    {0x02AD,0x038B, 1},
    //gain2
    {0x02AE,0x0CCE, 1},
    {0x02AF,0x8114, 1},
    {0x02B0,0x0003, 1},
    //gain3
    {0x02B1,0x0CCD, 1},
    {0x02B2,0x8114, 1},
    {0x02B3,0x0023, 1},
    //gain4
    {0x02B4,0x0CCC, 1},
    {0x02B5,0x8114, 1},
    {0x02B6,0x0043, 1},
    //gain5
    {0x02B7,0x0CC4, 1},
    {0x02B8,0x8114, 1},
    {0x02B9,0x0063, 1},
    //gain6
    {0x02BA,0x00C4, 1},
    {0x02BB,0x8114, 1},
    {0x02BC,0x0073, 1},
    //gain7
    {0x02BD,0x00A2, 1},
    {0x02BE,0x8114, 1},
    {0x02BF,0x0073, 1},
    //gain offset
    {0x02C0, 0x2E33, 1},
    {0x02C1, 0x2429, 1},
    {0x02C2, 0x181F, 1},
    {0x02C3, 0x02A3, 1},
    //jump point
    {0x02C4, 0xACA5, 1},
    {0x02C5, 0xB5B1, 1},
    {0x02C6, 0xBFBA, 1},
    {0x02C7, 0x00D3, 1},
    //mode jump offset
    {0x0284, 0x0000, 1},
    {0x0285, 0xFD0A, 1},
    {0x0286, 0x000A, 1},
    {0x02FA, 0x0A10, 1},
    {0x02FB, 0x0A10, 1},
    {0x02FC, 0x141A, 1},
    {0x02FD, 0x0310, 1},
#ifdef mHDT_SUPPORT
    // hwagc control
    {0x0274, 0x8116, 1},

    {0x0276, 0x00CC, 1},          //wait fast rssi
    {0x0277, 0x0000, 1},
    {0x0278, 0x0000, 1},
    {0x0279, 0x0000, 1},
    {0x027A, 0x0009, 1},
    {0x027B, 0x0270, 1},        //wait EDR guard time，52.5us
    {0x027C, 0x00CC, 1},          //wait slow rssi
    {0x027D, 0x0090, 1},
    {0x027E, 0x009C, 1},
    {0x027F, 0x021C, 1},
    {0x0280, 0x7F7F, 1},
    {0x0281, 0x0603, 1},          //rssi diff ble2m & lr
    {0x0283, 0x0000, 1},
    {0x02E8, 0x00F8, 1},

    {0x02E9, 0x01D4, 1},        // wait rf stable, bw1m
    {0x02EA, 0x03A8, 1},        // wait rf stable, bw2m
    {0x02EB, 0x0750, 1},        // wait rf stable, bw4m
    {0x02EC, 0x0FC0, 1},

    {0x02ED, 0x0000, 1},        //wait fast rssi
    {0x02EE, 0x00F0, 1},
    {0x02EF, 0x0108, 1},
    {0x02F0, 0x000C, 1},
    {0x02F1, 0x0018, 1},
    {0x02F2, 0x028E, 1},
    {0x02F3, 0x02CA, 1},
    {0x02F6, 0xFEFD, 1},
    {0x02F7, 0x00C0, 1},        //wait slow rssi
    {0x02F8, 0x0108, 1},
    {0x02F9, 0x00F0, 1},
    {0x02FA, 0x0A10, 1},
    {0x02FB, 0x0A10, 1},
    {0x02FC, 0x1016, 1},
    {0x02FD, 0x7F10, 1},
    {0x02FE, 0x7F7F, 1},
#endif  //mHDT_SUPPORT
    // hwagc control
    {0x0274, 0x8116, 1},

    {0x0276, 0x00CC, 1},        //wait fast rssi
    {0x0277, 0x0000, 1},
    {0x0278, 0x0000, 1},
    {0x0279, 0x0000, 1},
    {0x027A, 0x0009, 1},
    {0x027B, 0x0260, 1},        //wait EDR guard time，52.5us
    {0x027C, 0x00CC, 1},        //wait slow rssi
    {0x027D, 0x0090, 1},
    {0x027E, 0x009C, 1},
    {0x027F, 0x021C, 1},
    {0x0280, 0x7F7F, 1},
    {0x0281, 0x0603, 1},        //rssi diff ble2m & lr
    {0x0283, 0x0000, 1},
    {0x02E8, 0x00F8, 1},

    {0x02E9, 0x01D4, 1},        // wait rf stable, bw1m
    {0x02EA, 0x01D4, 1},        // wait rf stable, bw2m
    {0x02EB, 0x03A8, 1},        // wait rf stable, bw4m
    {0x02EC, 0x07C0, 1},

    {0x02ED, 0x0000, 1},        //wait fast rssi
    {0x02EE, 0x00F0, 1},
    {0x02EF, 0x0108, 1},
    {0x02F0, 0x0006, 1},
    {0x02F1, 0x000C, 1},
    {0x02F2, 0x027E, 1},
    {0x02F3, 0x02BA, 1},
    {0x02F6, 0xFEFD, 1},
    {0x02F7, 0x00C0, 1},        //wait slow rssi
    {0x02F8, 0x0108, 1},
    {0x02F9, 0x00F0, 1},
#endif  //BT_ITN_SGL_LNA
#endif  //__HW_AGC__

    {0x0223, 0x001c, 1},
    {0x0224, 0x001c, 1},
    {0x0225, 0x001c, 1},
    {0x0226, 0x001c, 1},
    {0x0227, 0x001c, 1},
    {0x0228, 0x001c, 1},
    {0x0229, 0x001c, 1},
    {0x022a, 0x001c, 1},

    {0x0210, 0x0014, 1},
    {0x0211, 0x0014, 1},
    {0x0212, 0x0014, 1},
    {0x0213, 0x0014, 1},
    {0x0214, 0x0014, 1},
    {0x0215, 0x0014, 1},
    {0x0216, 0x0014, 1},
    {0x0217, 0x0014, 1},

#ifdef RF_RX_LOW_POWER_MODE
    {0x0380, 0x5914, 1},
    {0x037F, 0x0EE5, 1},
#else
    {0x0380, 0x590C, 1},
    {0x037F, 0x1EEF, 1},
#endif
};

static void bt_drv_rf_xtal_cmom_set(uint16_t fcap)
{
    uint16_t val;

    btdrv_read_rf_reg(REG_BT_XTAL_CMOM_ADDR, &val);
    val = SET_BITFIELD(val, REG_BT_XTAL_CMOM, fcap);
    btdrv_write_rf_reg(REG_BT_XTAL_CMOM_ADDR, val);
}

void bt_drv_rf_set_bt_hw_agc_enable(bool enable)
{
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0xb6,&val_e2);
    if(enable)
    {
        //open rf bt hw agc mode
        val_e2 |= (1<<1);
    }
    else
    {
        //close rf bt hw agc mode
        val_e2 &= ~(1<<1);
    }
    btdrv_write_rf_reg(0xb6,val_e2);
}

void bt_drv_rf_set_ble_hw_agc_enable(bool enable)
{
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0xb7,&val_e2);
    if(enable)
    {
        //open rf ble hw agc mode
        val_e2 |= (1<<11);
    }
    else
    {
        //close rf ble hw agc mode
        val_e2 &= ~(1<<11);
    }
    btdrv_write_rf_reg(0xb7,val_e2);
}

void bt_drv_rf_set_afh_monitor_gain(void)
{
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0x1e4,&val_e2);//reg_bt_i2v_rin_afh[6:4]
    //i2v_rin_afh = 2 i2v_stb_afh=1
    val_e2 &= ~(7<<4);
    val_e2 |= (2<<4);
    val_e2 &= ~(7<<12); //reg_bt_i2v_stb_afh[14:12]
    val_e2 |= (1<<12); //reg_bt_i2v_stb_afh[14:12]
    btdrv_write_rf_reg(0x1e4,val_e2);


    btdrv_read_rf_reg(0x3cb,&val_e2); //lna_ic = 0 , i2v_gain = 7  lna_ldo_ic = 0xb
    val_e2 &= ~(0xf<<0);//reg_bt_i2v_gain_afh[3:0]
    val_e2 |= (7<<0);

    val_e2 &= ~(3<<4);//reg_bt_adc_vin_sel_afh[5:4]
    val_e2 |= (1<<4);

    val_e2 &= ~(0xF<<8);//reg_bt_lna_ldo_ic_afh[11:8]
    val_e2 |= (0x5<<8);

    val_e2 &= ~(7<<12);// reg_bt_lna_ic_afh[14:12]

    val_e2 &= ~(3<<6);//reg_bt_adc_vin_sel_normal[7:6]
    val_e2 |= (1<<6);

    btdrv_write_rf_reg(0x3cb,val_e2);


    btdrv_read_rf_reg(0x3cc,&val_e2); //flt = 7, hg_en=0, hg=0, rpass=11, rin=00,rfflt=0

    val_e2 = 0x1E0C;

    btdrv_write_rf_reg(0x3cc,val_e2);
}

void btdrv_rf_init_xtal_fcap(uint32_t fcap)
{
    xtal_fcap = SET_BITFIELD(xtal_fcap, REG_BT_XTAL_CMOM, fcap);
    init_xtal_fcap = xtal_fcap;
    bt_drv_rf_xtal_cmom_set(xtal_fcap);
}

void btdrv_rf_log_delay_cal(void)
{
    unsigned short read_value;
    unsigned short write_value;
    BT_DRV_TRACE(1,"%s", __func__);
    BTDIGITAL_REG(0xd0340020) = 0x010e01c0;
    BT_DRV_TRACE(1,"0xd0340020 =%x\n",BTDIGITAL_REG(0xd0340020) );

    btdrv_write_rf_reg(0xd4, 0x000f);
    btdrv_write_rf_reg(0xd5, 0x4000);
    btdrv_write_rf_reg(0xd2, 0x1003);
    btdrv_write_rf_reg(0xa7, 0x004e);
    btdrv_write_rf_reg(0xd4, 0x0000);
    btdrv_write_rf_reg(0xd5, 0x4002);

    BTDIGITAL_REG(0xd0340020) = 0x030e01c1;
    BT_DRV_TRACE(1,"0xd0340020 =%x\n",BTDIGITAL_REG(0xd0340020) );

    btdrv_delay(1);

    btdrv_write_rf_reg(0xd2, 0x5003);

    btdrv_delay(1);

    btdrv_read_rf_reg(0x1e2, &read_value);
    BT_DRV_TRACE(1,"0x1e2 read_value:%x\n",read_value);
    if(read_value == 0xff80)
    {
        btdrv_write_rf_reg(0xd3, 0xffff);
    }
    else
    {
        write_value = ((read_value>>7)&0x0001) | ((read_value & 0x007f)<<1) | ((read_value&0x8000)>>7) | ((read_value&0x7f00)<<1);
        BT_DRV_TRACE(1,"d3 write_value:%x\n",write_value);
        btdrv_write_rf_reg(0xd3, write_value);
    }
    btdrv_delay(1);

    BTDIGITAL_REG(0xd0340020) = 0x010e01c0;
    BT_DRV_TRACE(1,"0xd0340020 =%x\n",BTDIGITAL_REG(0xd0340020) );


    btdrv_write_rf_reg(0xd4, 0x000f);
    btdrv_write_rf_reg(0xd2, 0x1003);
    btdrv_write_rf_reg(0xd5, 0x4000);

}

void btdrv_rf_log_delay_cal_init(void)
{
    unsigned short read_value;
    uint16_t rf_f5_13_7, rf_f5_6_0 = 0;

    BT_DRV_TRACE(1,"%s", __func__);

    BTDIGITAL_REG(0xd0220c00) = 0x000A002A;

    //init
    btdrv_write_rf_reg(0x29c, 0xffb3);
    btdrv_write_rf_reg(0x29d, 0x001f);

    //cal flow
    btdrv_write_rf_reg(0xc2, 0x0);
    //bit9 : reg_rstn_log_cal
    btdrv_read_rf_reg(0xbe, &read_value);
    read_value |= (1 << 9);
    btdrv_write_rf_reg(0xbe, read_value);
    //reg_bt_log_cal_en
    btdrv_read_rf_reg(0x3af, &read_value);
    read_value |= (1 << 0);
    btdrv_write_rf_reg(0x3af, read_value);

    //log_cal_value[14:0]
    btdrv_read_rf_reg(0xf5, &read_value);
    BT_DRV_TRACE(0,"0xf5=0x%x",read_value);
    rf_f5_13_7 = (read_value & 0x3f80) >> 7;
    rf_f5_6_0  = (read_value & 0x7f);

    //0xc2 reg_fdata1[7:1] | 0xc2 reg_fdata2[15:9] | bit0:reg_fdata1_dr | bit8:reg_fdata2_dr
    btdrv_read_rf_reg(0xc2, &read_value);
    read_value = 0x0;
    read_value |= ((rf_f5_6_0 << 1) | (rf_f5_13_7 << 9) | (1<<0) | (1<<8));
    btdrv_write_rf_reg(0xc2, read_value);
    BT_DRV_TRACE(0,"0xc2=0x%x",read_value);

    //cal done
    btdrv_read_rf_reg(0xbe, &read_value);
    read_value &= ~(1 << 9);
    btdrv_write_rf_reg(0xbe, read_value);

    btdrv_read_rf_reg(0x3af, &read_value);
    read_value &= ~(1 << 0);
    btdrv_write_rf_reg(0x3af, read_value);

    BTDIGITAL_REG(0xd0220c00) = 0x0;
}

//rf Image calib
static uint16_t reg9b_val_save = 0;
static uint16_t reg9d_val_save = 0;
static bool vco_tx_currnet_maxed = false;

enum RF_VCO_DRV_CHIP_TYPE_T {
    RF_VCO_DRV_TYPE_0,
    RF_VCO_DRV_TYPE_1,
    RF_VCO_DRV_TYPE_2,
    RF_VCO_DRV_TYPE_3,

    RF_VCO_DRV_TYPE_QTY,
};

// version bit[0] = efuseD bit15, version bit[1] = efuseE bit11
struct RF_VCO_DRV_CALIIBRATION_MAP {
    enum RF_VCO_DRV_CHIP_TYPE_T type;
    uint16_t version;
};

static const struct RF_VCO_DRV_CALIIBRATION_MAP chip_tbl[] = {
    {RF_VCO_DRV_TYPE_0,     0x0},
    {RF_VCO_DRV_TYPE_1,     0x1},
    {RF_VCO_DRV_TYPE_2,     0x2},
    {RF_VCO_DRV_TYPE_3,     0x3},
};

static enum RF_VCO_DRV_CHIP_TYPE_T btdtv_rf_vco_calib_chip_match(void)
{
    int i;
    uint16_t efuseD_val;
    uint16_t efuseE_val;
    uint16_t version = 0;
    enum RF_VCO_DRV_CHIP_TYPE_T chip_type = RF_VCO_DRV_TYPE_QTY;

    pmu_get_efuse(PMU_EFUSE_PAGE_RESERVED_13, &efuseD_val);
    pmu_get_efuse(PMU_EFUSE_PAGE_SAR_VREF, &efuseE_val);
    efuseD_val = !!(efuseD_val & (1 << 15));
    efuseE_val = !!(efuseE_val & (1 << 11));

    version = efuseD_val << 0;
    version |= efuseE_val << 1;

    for (i = 0; i < ARRAY_SIZE(chip_tbl); i++) {
        if (chip_tbl[i].version == version) {
            chip_type = chip_tbl[i].type;
            break;
        }
    }
    BT_DRV_TRACE(0, "%s chip_type/version:%d/0x%x", __func__, chip_type, version);

    return chip_type;
}

void btdtv_rf_vco_calib(void)
{
    uint16_t addr = 0;
    enum RF_VCO_DRV_CHIP_TYPE_T chip_type = RF_VCO_DRV_TYPE_QTY;

    chip_type = btdtv_rf_vco_calib_chip_match();

    switch (chip_type) {
        case RF_VCO_DRV_TYPE_0:
            BT_DRV_TRACE(0, "%s Invalid efuse, use default value!", __func__);
            break;
        case RF_VCO_DRV_TYPE_1:
        case RF_VCO_DRV_TYPE_2:
        case RF_VCO_DRV_TYPE_3:
            BTRF_REG_SET_FIELD(0x9B, 0x7f, 6, 0x7f);
            BTRF_REG_SET_FIELD(0x9D, 0x7, 0, 5);
            vco_tx_currnet_maxed = true;
            break;
        default:
            break;
    }

    addr = 0x9b;
    btdrv_read_rf_reg(addr, &reg9b_val_save);
    BT_DRV_TRACE(0, "%s chk %04x=%04x", __func__, addr, reg9b_val_save);
    addr = 0x9d;
    btdrv_read_rf_reg(addr, &reg9d_val_save);
    BT_DRV_TRACE(0, "%s chk %04x=%04x", __func__, addr, reg9d_val_save);
}

void btdtv_rf_vco_drv_workaround(int temperature_celsius)
{
    static bool need_workaround = true;
    uint16_t vco_tx_ldo_vres;

    BT_DRV_TRACE(0, "%s temperature_celsius:%d, need_workaround:%d", __func__, temperature_celsius, need_workaround);
    if (temperature_celsius <= 0 && need_workaround) {
        BTRF_REG_GET_FIELD(0x9D, 0x7, 0, vco_tx_ldo_vres);
        if (vco_tx_ldo_vres < 0x7) {
            vco_tx_ldo_vres++;
        }
        BTRF_REG_SET_FIELD(0x9D, 0x7, 0, vco_tx_ldo_vres);
        if (!vco_tx_currnet_maxed) {
            BTRF_REG_SET_FIELD(0x9B, 0x7f, 6, 0x7f);
        }
        need_workaround = false;
    } else if (temperature_celsius >= 10 && !need_workaround) {
        btdrv_write_rf_reg(0x9B, reg9b_val_save);
        btdrv_write_rf_reg(0x9D, reg9d_val_save);
        need_workaround = true;
    }
}

#ifdef RX_IQ_CAL

int bt_iqimb_test_ex (int mismatch_type);
void bt_iqimb_test_init(void);
extern int iq_gain;
extern int iq_phy;

void dataSort(int *arry,int len)
{
    for (int i = 0; i < len - 1; i++){
        for (int j = 0; j < len - 1 - i; j++){
            if (arry[j] > arry[j + 1]){
                int temp = arry[j + 1];
                arry[j + 1] = arry[j];
                arry[j] = temp;
            }
        }
    }
}

const uint16_t rx_cal_rfreg_set[][2] =
{
    {0xE8, 0x36B1},
    {0xE9, 0x1175},
    {0x3AD, 0x0038},
    {0xBF, 0x0D00},
    {0xC0,0x1E17},
    {0x1E4, 0x0025},
};

const uint16_t rx_cal_rfreg_store[][1] =
{
    {0xE8},
    {0xE9},
    {0x3AD},
    {0xBF},
    {0xC0},
    {0x1E4},
};

void btdrv_rx_iq_cal(void)
{
    //uint16_t val;
    uint8_t i;
    uint16_t value = 0;
    uint32_t tx_cal_digreg_store[6];

    //const uint16_t (*rx_cal_rfreg_set_p)[2];
    const uint16_t (*rx_cal_rfreg_store_p)[1];
    //uint32_t rx_reg_set_tbl_size = 0;

    tx_cal_digreg_store[0] = BTDIGITAL_REG(0xD0350214);
    tx_cal_digreg_store[1] = BTDIGITAL_REG(0xd0350218);
    tx_cal_digreg_store[2] = BTDIGITAL_REG(0xd0350240);
    tx_cal_digreg_store[3] = BTDIGITAL_REG(0xD0350300);
    tx_cal_digreg_store[4] = BTDIGITAL_REG(0xD035031c);
    tx_cal_digreg_store[5] = BTDIGITAL_REG(0xD0350320);

    rx_cal_rfreg_store_p = &rx_cal_rfreg_store[0];
    uint32_t rx_reg_store_tbl_size = ARRAY_SIZE(rx_cal_rfreg_store);
    uint16_t rx_cal_rfreg_store[rx_reg_store_tbl_size];
    BT_DRV_TRACE(0,"rx reg_store:\n");
    for(i=0; i< rx_reg_store_tbl_size; i++)
    {
        btdrv_read_rf_reg(rx_cal_rfreg_store_p[i][0],&value);
        rx_cal_rfreg_store[i] = value;
        BT_DRV_TRACE(2,"rx reg=%x,v=%x",rx_cal_rfreg_store_p[i][0],value);
    }


    BT_DRV_TRACE(1,"0xd0350214:%x\n",tx_cal_digreg_store[0]);
    BT_DRV_TRACE(1,"0xD0350218:%x\n",tx_cal_digreg_store[1]);
    BT_DRV_TRACE(1,"0xD0350240:%x\n",tx_cal_digreg_store[2]);
    BT_DRV_TRACE(1,"0xD0350300:%x\n",tx_cal_digreg_store[3]);
    BT_DRV_TRACE(1,"0xD035031c:%x\n",tx_cal_digreg_store[4]);
    BT_DRV_TRACE(1,"0xD0350320:%x\n",tx_cal_digreg_store[5]);

    bt_iqimb_test_init();

    BTDIGITAL_REG(0xd0350218) = 0x00000583;
    BTDIGITAL_REG(0xD0350214) = 0x0;
    BTDIGITAL_REG(0xD0350320) = 0x0001000F;
    BTDIGITAL_REG_SET_FIELD(0xD0350300, 7, 0, 3);
    BTDIGITAL_REG_SET_FIELD(0xD035031c, 0xf, 0, 1);
#ifdef BT_IF_750K
    BTDIGITAL_REG_SET_FIELD(0xD0350320, 0x7f, 0, 0xc);
#elif defined(BT_IF_1P05M)
    BTDIGITAL_REG_SET_FIELD(0xD0350320, 0x7f, 0, 0xf);
#endif
    BTDIGITAL_REG_SET_FIELD(0xd0350240, 0x1, 12, 1);

    while(1) {
        BTDIGITAL_REG(0xd0220c00) = 0x0;
        btdrv_delay(1);
        BTDIGITAL_REG(0xd0220c00) = 0x001BA096;
        BT_DRV_TRACE(1, "0xd0220c00=0x%x", BTDIGITAL_REG(0xd0220c00));
        btdrv_delay(1);
        bt_iqimb_test_ex(1);
        btdrv_delay(5*1000);
    }

    BTDIGITAL_REG(0xd0220c00) = 0x0;
    BTDIGITAL_REG_SET_FIELD(0xD035025c, 0x3, 2, 0);
    BT_DRV_TRACE(0,"25c:%x\n",BTDIGITAL_REG(0xd035025c));

    BT_DRV_TRACE(0,"reg_reset:\n");

    for(i=0; i< rx_reg_store_tbl_size; i++)
    {
        btdrv_write_rf_reg(rx_cal_rfreg_store_p[i][0],rx_cal_rfreg_store[i]);
        BTRF_REG_DUMP(rx_cal_rfreg_store_p[i][0]);
    }

    BTDIGITAL_REG(0xD0350214) = tx_cal_digreg_store[0];
    BTDIGITAL_REG(0xd0350218) = tx_cal_digreg_store[1];
    BTDIGITAL_REG(0xd0350240) = tx_cal_digreg_store[2];
    BTDIGITAL_REG(0xD0350300) = tx_cal_digreg_store[3];
    BTDIGITAL_REG(0xD035031c) = tx_cal_digreg_store[4];
    BTDIGITAL_REG(0xD0350320) = tx_cal_digreg_store[5];

    BT_DRV_TRACE(1,"0xD0350214:%x\n",BTDIGITAL_REG(0xD0350214));
    BT_DRV_TRACE(1,"0xd0350218:%x\n",BTDIGITAL_REG(0xd0350218));
    BT_DRV_TRACE(1,"0xD0350240:%x\n",BTDIGITAL_REG(0xD0350240));
    BT_DRV_TRACE(1,"0xD0350300:%x\n",BTDIGITAL_REG(0xD0350300));
    BT_DRV_TRACE(1,"0xD035031c:%x\n",BTDIGITAL_REG(0xD035031c));
    BT_DRV_TRACE(1,"0xD0350320:%x\n",BTDIGITAL_REG(0xD0350320));
    //BT_DRV_TRACE(1,"0xd0350240:%x\n",BTDIGITAL_REG(0xd0350240));
}
#endif

struct btdrv_customer_rf_config_t btdrv_rf_customer_config;
void bt_drv_rf_sdk_init(void)
{
    BT_DRV_TRACE(1,"SDK default rf config,metal id=%d", bt_metal_id);
    btdrv_rf_customer_config.config_xtal_en = false;//get value from NV
    btdrv_rf_customer_config.xtal_cap_val = 0;//get value from NV

    //config TX power in dbm
    btdrv_rf_customer_config.config_tx_pwr_en = true;

    btdrv_rf_customer_config.bt_tx_page_pwr  = TX_PWR_8DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_max_pwr   = TX_PWR_16DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx4_pwr  = TX_PWR_11DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx3_pwr  = TX_PWR_7DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx2_pwr  = TX_PWR_2DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx1_pwr  = TX_PWR_N2DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx0_pwr  = TX_PWR_N5DBM;//in dbm

    btdrv_rf_customer_config.le_tx_max_pwr   = TX_PWR_16DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx4_pwr  = TX_PWR_11DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx3_pwr  = TX_PWR_7DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx2_pwr  = TX_PWR_2DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx1_pwr  = TX_PWR_N2DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx0_pwr  = TX_PWR_N5DBM;//in dbm

    btdrv_txpwr_conv_tbl[0] = btdrv_rf_customer_config.le_tx_idx0_pwr;
    btdrv_txpwr_conv_tbl[1] = btdrv_rf_customer_config.le_tx_idx1_pwr;
    btdrv_txpwr_conv_tbl[2] = btdrv_rf_customer_config.le_tx_idx2_pwr;
    btdrv_txpwr_conv_tbl[3] = btdrv_rf_customer_config.le_tx_idx3_pwr;
    btdrv_txpwr_conv_tbl[4] = btdrv_rf_customer_config.le_tx_idx4_pwr;
    btdrv_txpwr_conv_tbl[5] = btdrv_rf_customer_config.le_tx_max_pwr;
    btdrv_txpwr_conv_tbl[6] = btdrv_rf_customer_config.le_tx_max_pwr;
    btdrv_txpwr_conv_tbl[7] = btdrv_rf_customer_config.le_tx_max_pwr;
}

void bt_drv_rf_set_customer_config(struct btdrv_customer_rf_config_t* config)
{
    memset(&btdrv_rf_customer_config, 0, sizeof(struct btdrv_customer_rf_config_t));
    if(config !=NULL)
    {
        btdrv_rf_customer_config = *config;
    }
    else
    {
        bt_drv_rf_sdk_init();
    }
}

static void bt_rf_txpwr_reg_get(uint8_t idx, uint16_t *tx_flt, uint16_t *rf_vco)
{
    switch(idx)
    {
        case TX_PWR_IDX_0:
            *tx_flt = RF_BT_TX_PWR_IDX0_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BT_TX_PWR_IDX0_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_1:
            *tx_flt = RF_BT_TX_PWR_IDX1_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BT_TX_PWR_IDX1_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_2:
            *tx_flt = RF_BT_TX_PWR_IDX2_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BT_TX_PWR_IDX2_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_3:
            *tx_flt = RF_BT_TX_PWR_IDX3_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BT_TX_PWR_IDX3_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_4:
            *tx_flt = RF_BT_TX_PWR_IDX4_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BT_TX_PWR_IDX4_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_MAX:
            *tx_flt = RF_BT_TX_PWR_IDX5_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BT_TX_PWR_IDX5_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_PAGE:
            *tx_flt = RF_BT_TX_PWR_IDX6_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BT_TX_PWR_IDX6_RFVCO_REG_NORMAL;
            break;
        default:
            ASSERT(0, "BT_DRV:ERROR tx pwr idx=%d", idx);
            break;
    }
}

static void ble_rf_txpwr_reg_get(uint8_t idx, uint16_t *tx_flt, uint16_t *rf_vco)
{
    switch(idx)
    {
        case TX_PWR_IDX_0:
            *tx_flt = RF_BLE_TX_PWR_IDX0_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX0_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_1:
            *tx_flt = RF_BLE_TX_PWR_IDX1_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX1_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_2:
            *tx_flt = RF_BLE_TX_PWR_IDX2_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX2_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_3:
            *tx_flt = RF_BLE_TX_PWR_IDX3_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX3_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_4:
            *tx_flt = RF_BLE_TX_PWR_IDX4_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX4_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_MAX:
            *tx_flt = RF_BLE_TX_PWR_IDX5_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX5_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_6:
            *tx_flt = RF_BLE_TX_PWR_IDX6_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX6_RFVCO_REG_NORMAL;
            break;
        case TX_PWR_IDX_7:
            *tx_flt = RF_BLE_TX_PWR_IDX7_TX_FLT_REG_NORMAL;
            *rf_vco = RF_BLE_TX_PWR_IDX7_RFVCO_REG_NORMAL;
            break;
        default:
            ASSERT(0, "BT_DRV:ERROR tx pwr idx=%d", idx);
            break;
    }
}

int rf_reg_txpwr_val_get_normal(int8_t txpwr_dbm, uint16_t *tx_flt, uint16_t *rf_vco, uint8_t *pa_cfg)
{
    uint32_t nRet = -1;
    uint16_t i = 0;
    uint16_t tbl_size;
    tbl_size = sizeof(tx_pwr_select_normal)/sizeof(tx_pwr_select_normal[0]);
    for(i=0; i< tbl_size; i++){
        if(tx_pwr_select_normal[i].pwr_dbm == txpwr_dbm){
            *tx_flt = tx_pwr_select_normal[i].tx_flt;
            *rf_vco = tx_pwr_select_normal[i].rf_vco;
            *pa_cfg = tx_pwr_select_normal[i].pa_cfg;
            nRet = 0;
            break;
        }
    }
    return nRet;
}

int rf_reg_txpwr_val_get_high_freq_low_freq(int8_t txpwr_dbm, uint16_t *tx_flt, uint16_t *rf_vco, uint8_t *pa_cfg)
{
    uint32_t nRet = -1;
    uint16_t i = 0;
    uint16_t tbl_size;
    tbl_size = sizeof(tx_pwr_select_high_freq_low_freq)/sizeof(tx_pwr_select_high_freq_low_freq[0]);
    for(i=0; i< tbl_size; i++){
        if(tx_pwr_select_high_freq_low_freq[i].pwr_dbm == txpwr_dbm){
            *tx_flt = tx_pwr_select_high_freq_low_freq[i].tx_flt;
            *rf_vco = tx_pwr_select_high_freq_low_freq[i].rf_vco;
            *pa_cfg = tx_pwr_select_normal[i].pa_cfg;
            nRet = 0;
            break;
        }
    }
    return nRet;
}

void bt_drv_ble_rf_reg_txpwr_set(uint16_t rf_reg, uint16_t val)
{
    btdrv_write_rf_reg(rf_reg, val);
}

static void bt_drv_rf_pa_sel(uint8_t pa_cfg, uint16_t tx_flt_addr, uint16_t rf_vco_addr)
{
    uint8_t pa_sel = 0;

    pa_sel = pa_cfg & 0xF;

    switch(pa_sel)
    {
        case TX_PWR_0P6_PA_DOHERTY:
            BTRF_REG_SET_FIELD(tx_flt_addr, 0x1, 6, 0x1);
            BTRF_REG_SET_FIELD(rf_vco_addr, 0x7, 13, 0x1);
            break;
        case TX_PWR_1P1_PA_DOHERTY:
            BTRF_REG_SET_FIELD(tx_flt_addr, 0x1, 6, 0x1);
            BTRF_REG_SET_FIELD(rf_vco_addr, 0x7, 13, 0x2);
            break;
        case TX_PWR_1P8_PA:
            BTRF_REG_SET_FIELD(tx_flt_addr, 0x1, 6, 0x0);
            BTRF_REG_SET_FIELD(rf_vco_addr, 0x7, 13, 0x4);
            break;
        case TX_PWR_1P8_PA_DOHERTY:
            BTRF_REG_SET_FIELD(tx_flt_addr, 0x1, 6, 0x1);
            BTRF_REG_SET_FIELD(rf_vco_addr, 0x7, 13, 0x4);
            break;
        default:
            ASSERT(0, "BT_DRV:ERROR tx pwr pa sel");
            break;
    }
}

static void bt_drv_rf_bt_txpwr_dpd_sel(uint8_t pa_cfg, int8_t idx)
{
    uint16_t read_val = 0;
    uint8_t dpd_sel = 0;
    uint16_t dpd_sel_shift = 8;

    pa_cfg = pa_cfg >> 4;
    dpd_sel = pa_cfg & 0x1;
    dpd_sel_shift += idx;

    btdrv_read_rf_reg(BT_Rf_REG_DPD_SEL, &read_val);
    BTRF_REG_SET_FIELD(BT_Rf_REG_DPD_SEL, 0x1, dpd_sel_shift, dpd_sel);
}

static void bt_drv_rf_le_txpwr_dpd_sel(uint8_t pa_cfg, int8_t idx)
{
    uint16_t read_val = 0;
    uint8_t dpd_sel = 0;
    uint16_t dpd_sel_shift = 0;

    pa_cfg = pa_cfg >> 4;
    dpd_sel = pa_cfg & 0x1;
    dpd_sel_shift += idx;

    btdrv_read_rf_reg(BT_Rf_REG_DPD_SEL, &read_val);
    BTRF_REG_SET_FIELD(BT_Rf_REG_DPD_SEL, 0x1, dpd_sel_shift, dpd_sel);
}

static void bt_drv_rf_bt_txpwr_set(uint8_t idx, int8_t val_in_dbm)
{
    uint8_t nRet;
    uint16_t rf_vco_val = 0;
    uint16_t tx_flt_val = 0;
    uint8_t pa_cfg = 0;
    uint16_t rf_vco_addr = 0;
    uint16_t tx_flt_addr = 0;

    bt_rf_txpwr_reg_get(idx, &tx_flt_addr, &rf_vco_addr);
    nRet = rf_reg_txpwr_val_get_normal(val_in_dbm, &tx_flt_val, &rf_vco_val, &pa_cfg);
    ASSERT(!nRet, "BT_DRV:pls write correct tx pwr");

    btdrv_write_rf_reg(tx_flt_addr, tx_flt_val);
    btdrv_write_rf_reg(rf_vco_addr, rf_vco_val);

    bt_drv_rf_pa_sel(pa_cfg, tx_flt_addr, rf_vco_addr);
    bt_drv_rf_bt_txpwr_dpd_sel(pa_cfg, idx);

    BT_DRV_TRACE(2, "BT:TX pwr in:%d dbm,[reg0=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x]", val_in_dbm, tx_flt_addr, tx_flt_val, rf_vco_addr, rf_vco_val);
}

static void bt_drv_rf_ble_txpwr_set(uint8_t idx, int8_t val_in_dbm)
{
    uint8_t nRet;
    uint16_t rf_vco_val = 0;
    uint16_t tx_flt_val = 0;
    uint8_t pa_cfg = 0;
    uint16_t rf_vco_addr = 0;
    uint16_t tx_flt_addr = 0;

    ble_rf_txpwr_reg_get(idx, &tx_flt_addr, &rf_vco_addr);
    nRet = rf_reg_txpwr_val_get_normal(val_in_dbm, &tx_flt_val, &rf_vco_val, &pa_cfg);
    ASSERT(!nRet, "BT_DRV:pls write correct tx pwr");

    btdrv_write_rf_reg(tx_flt_addr, tx_flt_val);
    btdrv_write_rf_reg(rf_vco_addr, rf_vco_val);

    bt_drv_rf_pa_sel(pa_cfg, tx_flt_addr, rf_vco_addr);
    bt_drv_rf_le_txpwr_dpd_sel(pa_cfg, idx);

    BT_DRV_TRACE(2, "BLE:TX pwr in:%d dbm,[reg0=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x]", val_in_dbm, tx_flt_addr, tx_flt_val, rf_vco_addr, rf_vco_val);
}

void bt_drv_tx_pwr_init(void)
{
    struct btdrv_customer_rf_config_t cfg = btdrv_rf_customer_config;

    if(cfg.config_tx_pwr_en == true)
    {
        //BT
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_0, cfg.bt_tx_idx0_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_1, cfg.bt_tx_idx1_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_2, cfg.bt_tx_idx2_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_3, cfg.bt_tx_idx3_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_4, cfg.bt_tx_idx4_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_MAX, cfg.bt_tx_max_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_PAGE, cfg.bt_tx_page_pwr);
        //BLE
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_0, cfg.le_tx_idx0_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_1, cfg.le_tx_idx1_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_2, cfg.le_tx_idx2_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_3, cfg.le_tx_idx3_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_4, cfg.le_tx_idx4_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_MAX, cfg.le_tx_max_pwr);

    }
    else
    {
        ASSERT(0, "BT_DRV:pls check TX pwr config");
    }
}

void bt_drv_rf_init_xtal_fcap(void)
{
    uint16_t xtal_fcap_temp = DEFAULT_XTAL_FCAP;
    if(btdrv_rf_customer_config.config_xtal_en == true)
    {
        xtal_fcap_temp = btdrv_rf_customer_config.xtal_cap_val;
        BT_DRV_TRACE(1,"btdrv customer set xtal fcap=0x%x", xtal_fcap_temp);
        btdrv_rf_init_xtal_fcap(xtal_fcap_temp);
    }
    else
    {
#ifdef __RF_INIT_XTAL_CAP_FROM_NV__
        unsigned int xtal_fcap;
        if (!nvrec_dev_get_xtal_fcap(&xtal_fcap))
        {
            btdrv_rf_init_xtal_fcap(xtal_fcap);
            btdrv_delay(1);
            BT_DRV_TRACE(2,"%s val=0x%x", __func__, xtal_fcap);
        }
        else
#endif
        {
            btdrv_rf_init_xtal_fcap(DEFAULT_XTAL_FCAP);
            BT_DRV_TRACE(1,"%s failed", __func__);
        }
    }
}

void bt_drv_ble_adv_txpwr_via_advhdl(uint8_t adv_hdl, uint8_t idx, int8_t txpwr_dbm)
{
    uint8_t nRet;
    uint16_t rf_vco_val = 0;
    uint16_t tx_flt_val = 0;
    uint8_t pa_cfg = 0;
    uint16_t rf_vco_addr = 0;
    uint16_t tx_flt_addr = 0;

    BT_DRV_TRACE(2,"set adv_txpwr_via_advhdl hdl 0x%02x idx %d dbm %d", adv_hdl, idx, txpwr_dbm);
    ASSERT(idx <= 7, "BT_DRV:ERROR tx pwr idx=%d", idx);

    ble_rf_txpwr_reg_get(idx, &tx_flt_addr, &rf_vco_addr);
    nRet = rf_reg_txpwr_val_get_normal(txpwr_dbm, &tx_flt_val, &rf_vco_val, &pa_cfg);
    ASSERT(!nRet, "BT_DRV:pls write correct tx pwr");

    btdrv_write_rf_reg(tx_flt_addr, tx_flt_val);
    btdrv_write_rf_reg(rf_vco_addr, rf_vco_val);
    bt_drv_rf_pa_sel(pa_cfg, tx_flt_addr, rf_vco_addr);
    bt_drv_rf_le_txpwr_dpd_sel(pa_cfg, idx);
}

void btdrv_cap_delay_cal(void)
{
    uint16_t value = 0;;
    uint8_t tx_capdly, rx_capdly = 0;

    tx_capdly = value & 0xF;
    rx_capdly = ((value & 0xF0) >> 4);

    if ((0 == tx_capdly) || (0 == rx_capdly)) {
        BT_DRV_TRACE(0, "invalid efuse ,so use default value");
        tx_capdly = rx_capdly = 0x8;
    }

    BTRF_REG_SET_FIELD(0x295, 0xF, 6, tx_capdly);
    BTRF_REG_SET_FIELD(0x2C5, 0xF, 6, rx_capdly);
}

#ifdef __HW_AGC__
static uint16_t hw_agc_rx_gain0_i2v_reg_val;
static uint16_t hw_agc_rx_gain0_lna_reg_val;
static uint16_t hw_agc_rx_gain0_adc_reg_val;
static uint16_t hw_agc_rx_gain1_i2v_reg_val;
static uint16_t hw_agc_rx_gain1_lna_reg_val;
static uint16_t hw_agc_rx_gain1_adc_reg_val;
static uint8_t bt_drv_hw_agc_mode_status = 0xFF;

void bt_drv_rf_hw_agc_mode_init(void)
{
    btdrv_read_rf_reg(0x288, &hw_agc_rx_gain0_i2v_reg_val);
    btdrv_read_rf_reg(0x289, &hw_agc_rx_gain0_lna_reg_val);
    btdrv_read_rf_reg(0x28A, &hw_agc_rx_gain0_adc_reg_val);
    btdrv_read_rf_reg(0x28B, &hw_agc_rx_gain1_i2v_reg_val);
    btdrv_read_rf_reg(0x28C, &hw_agc_rx_gain1_lna_reg_val);
    btdrv_read_rf_reg(0x28D, &hw_agc_rx_gain1_adc_reg_val);
}

void bt_drv_rf_get_hw_agc_reg_addr(int8_t rx_idx, uint16_t *i2v_reg, uint16_t *lna_reg, uint16_t *adc_reg)
{
    *i2v_reg = 0x288 + 3 * rx_idx;
    *lna_reg = 0x289 + 3 * rx_idx;
    *adc_reg = 0x28A + 3 * rx_idx;
}

void bt_drv_rf_hw_agc_mode_set(int8_t rx_idx, int8_t hwagc_mode)
{
    uint16_t i2v_reg;
    uint16_t lna_reg;
    uint16_t adc_reg;
    if (bt_drv_hw_agc_mode_status == hwagc_mode) {
        return;
    }

    bt_drv_hw_agc_mode_status = hwagc_mode;

    bt_drv_rf_get_hw_agc_reg_addr(rx_idx, &i2v_reg, &lna_reg, &adc_reg);

    switch(rx_idx) {
        case RX_GAIN_IDX_0:
            if (hwagc_mode == HW_AGC_PWR_SAVE_MODE){
                btdrv_write_rf_reg(i2v_reg, hw_agc_rx_gain1_i2v_reg_val);
                btdrv_write_rf_reg(lna_reg, hw_agc_rx_gain1_lna_reg_val);
                btdrv_write_rf_reg(adc_reg, hw_agc_rx_gain1_adc_reg_val);
            }else if (hwagc_mode == HW_AGC_HIGH_PWEF_MODE){
                btdrv_write_rf_reg(i2v_reg, hw_agc_rx_gain0_i2v_reg_val);
                btdrv_write_rf_reg(lna_reg, hw_agc_rx_gain0_lna_reg_val);
                btdrv_write_rf_reg(adc_reg, hw_agc_rx_gain0_adc_reg_val);
            }
            break;
        default:
            ASSERT(0, "BT_DRV:ERROR rx gain idx=%d don't have low pwr mode", rx_idx);
            break;
    }
}

void bt_drv_rf_high_efficency_tx_pwr_ctrl(bool limit_tx_idx, bool limit_pa_en)
{
#ifdef HIGH_EFFICIENCY_TX_PWR_CTRL
    bool limit_hwagc_init_gain = limit_tx_idx;
    if(limit_hwagc_init_gain)
    {
        bt_drv_rf_hw_agc_mode_set(RX_GAIN_IDX_0, HW_AGC_PWR_SAVE_MODE);
    }
    else
    {
        bt_drv_rf_hw_agc_mode_set(RX_GAIN_IDX_0, HW_AGC_HIGH_PWEF_MODE);
    }
#endif
}
#endif

uint8_t btdrv_rf_init(void)
{
    bt_metal_id = hal_get_chip_metal_id();
    uint32_t pmu_id = read_hw_metal_id();
    BT_DRV_TRACE(2, "%s,metal id=%d,PMU id=%d", __func__, bt_metal_id, pmu_id);

    uint16_t value = 0;
    const uint16_t (*rf_init_tbl_p)[3] = NULL;
    uint32_t tbl_size = 0;
    uint16_t i;
    //system RF register init
    for (i = 0; i < ARRAY_SIZE(rf_sys_init_tbl); i++){
        btdrv_read_rf_reg(rf_sys_init_tbl[i].reg, &value);
        value = (value & ~rf_sys_init_tbl[i].mask) | (rf_sys_init_tbl[i].set & rf_sys_init_tbl[i].mask);
        if (rf_sys_init_tbl[i].delay){
            btdrv_delay(rf_sys_init_tbl[i].delay);
        }
        btdrv_write_rf_reg(rf_sys_init_tbl[i].reg, value);
    }
    //common RF register init
    rf_init_tbl_p = &rf_init_tbl_1[0];
    tbl_size = sizeof(rf_init_tbl_1)/sizeof(rf_init_tbl_1[0]);

    for(i=0; i< tbl_size; i++){
        btdrv_write_rf_reg(rf_init_tbl_p[i][0],rf_init_tbl_p[i][1]);
        if(rf_init_tbl_p[i][2] !=0)
            btdrv_delay(rf_init_tbl_p[i][2]);//delay
        btdrv_read_rf_reg(rf_init_tbl_p[i][0],&value);
        if (value != rf_init_tbl_p[i][1]){
            BT_DRV_TRACE(3,"write false rf reg=0x%x,write=0x%x,read=0x%x",rf_init_tbl_p[i][0],rf_init_tbl_p[i][1],value);
        }
    }

#ifdef __NEW_SWAGC_MODE__
    //open rf BT sync agc mode
    bt_drv_rf_set_bt_sync_agc_enable(true);
#else
    //close rf BT sync agc mode
    bt_drv_rf_set_bt_sync_agc_enable(false);
#endif

    bt_drv_rf_set_customer_config(NULL);
    bt_drv_tx_pwr_init();
    bt_drv_rf_init_xtal_fcap();

#ifdef __HW_AGC__
    bt_drv_rf_hw_agc_mode_init();
    bt_drv_rf_hw_agc_mode_set(RX_GAIN_IDX_0, HW_AGC_HIGH_PWEF_MODE);
#endif

    return 0;
}

void bt_drv_rf_set_bt_sync_agc_enable(bool enable)
{
    uint16_t val = 0;
    btdrv_read_rf_reg(0x36,&val);
    if(enable)
    {
        //open rf new sync agc mode
        val |= (1<<3);
    }
    else
    {
        //close rf new sync agc mode
        val &= ~(1<<3);
    }
    btdrv_write_rf_reg(0x36,val);
}

void bt_drv_rf_set_ble_sync_agc_enable(bool enable)
{
    uint16_t val = 0;
    btdrv_read_rf_reg(0x36,&val);
    if(enable)
    {
        //open rf new sync agc mode
        val |= (1<<2);
    }
    else
    {
        //close rf new sync agc mode
        val &= ~(1<<2);
    }
    btdrv_write_rf_reg(0x36,val);
}

struct TX_PWR_CALIB_ITEM
{
    uint32_t txpwr_dig_reg;
    uint16_t mask;
    uint16_t shift;
};

static const struct TX_PWR_CALIB_ITEM tx_pwr_calib[] =
{
    {0xD0350308, 0x3F,  0},
    {0xD0350308, 0x3F, 10},
    {0xD0350370, 0x3F,  0},
    {0xD0350370, 0x3F, 10},
    {0xD0350344, 0x3F,  0},
};

void btdrv_txpower_calib(void)
{
    uint16_t read_val;
    int16_t calib_val;
    uint16_t reg_val;
    int16_t bt_pwr_freql, bt_pwr_freqm, bt_pwr_freqh;
    float bt_pwr_avg = 0.0f;

    pmu_get_efuse(PMU__EFUSE_PAGE_RESERVED_24, &read_val);
    bt_pwr_freql = read_val & 0x7;
    bt_pwr_freqm = (read_val >> 4) & 0x7;
    bt_pwr_freqh = (read_val >> 8) & 0x7;

    bt_pwr_freql = getbit(read_val, 3)  ? bt_pwr_freql : (-bt_pwr_freql);
    bt_pwr_freqm = getbit(read_val, 7)  ? bt_pwr_freqm : (-bt_pwr_freqm);
    bt_pwr_freqh = getbit(read_val, 11) ? bt_pwr_freqh : (-bt_pwr_freqh);

    bt_pwr_avg = ((float)(bt_pwr_freql + bt_pwr_freqm + bt_pwr_freqh) / 3.0f);

    if (bt_pwr_avg > 0) {
        calib_val = (int16_t) (bt_pwr_avg + 0.5f);
    } else {
        calib_val = (int16_t) (bt_pwr_avg - 0.5f);
    }
    calib_val += 1;
    TRACE(3, "%s efuse D val=0x%x, calib_val=%d",__func__, read_val, calib_val);

    if (calib_val > 3) {
        calib_val = 3;
    } else if (calib_val < -4) {
        calib_val = -4;
    }

    // High transmission power reduces digital gain
    if (calib_val >= 0 && calib_val <= 3) {
        for (int i = 0; i < ARRAY_SIZE(tx_pwr_calib); i++) {
            BTDIGITAL_REG_GET_FIELD(tx_pwr_calib[i].txpwr_dig_reg, tx_pwr_calib[i].mask, tx_pwr_calib[i].shift, reg_val);
            reg_val = reg_val - calib_val;
            BTDIGITAL_REG_SET_FIELD(tx_pwr_calib[i].txpwr_dig_reg, tx_pwr_calib[i].mask, tx_pwr_calib[i].shift, reg_val);
        }
    } else if (calib_val == -1) {
        BTRF_REG_SET_FIELD(0xB2, 0x3F, 10, 0x14);
    } else if (calib_val == -2) {
        BTRF_REG_SET_FIELD(0xB2, 0x3F, 10, 0x18);
    } else if (calib_val == -3) {
        BTRF_REG_SET_FIELD(0xB2, 0x3F, 10, 0x1C);
    } else if (calib_val == -4) {
        BTRF_REG_SET_FIELD(0xB2, 0x3F, 10, 0x20);
    }
}

//factor=N/512
void btdrv_set_bdr_ble_txpower(uint8_t txpwr_idx, uint16_t n)
{
    uint32_t reg_base = 0xd03500C4;
    uint32_t reg;
    if(txpwr_idx > 7)
    {
        BT_DRV_TRACE(1, "%s txpwr idx err:%d\n", __func__, txpwr_idx);
        return;
    }

    if(n > 1023)
    {
        BT_DRV_TRACE(1, "%s n err:%d\n", __func__, n);
        return;
    }

    reg = (txpwr_idx / 3) * 4 + reg_base;

    BTDIGITAL_REG_SET_FIELD(reg, 0x3ff, (txpwr_idx % 3) * 10, n);
}
