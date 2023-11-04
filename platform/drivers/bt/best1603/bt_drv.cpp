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
#include "string.h"
#ifndef NOSTD
#include "math.h"
#endif
#include "bt_drv.h"
#include "bt_drv_internal.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "hal_intersys.h"
#include "hal_trace.h"
#include "hal_psc.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "pmu.h"
#include "nvrecord_dev.h"
#if defined (RX_IQ_CAL) || defined(DPD_ONCHIP_CAL) || defined (TX_IQ_CAL)
#include "iqcorrect.h"
#endif
#ifdef DPD_ONCHIP_CAL
#include "bt_drv_dpd_mem.h"
#endif
#include "hal_btdump.h"
#include "bt_drv_internal.h"

//CHIP related
#include "bt_drv_1603_internal.h"
#include "bt_drv_1603_config.h"
#ifdef __BT_RAMRUN_NEW__
#include "bt_drv_ramrun_symbol_1603.h"
#include "../../bt_controller/bt_controller.h"
#endif
#ifdef BTC_CPUDUMP_BASE
#include "bt_drv_cpudump.h"
#endif

bool btdrv_dut_mode_enable = false;

static volatile uint32_t btdrv_tx_flag = 1;
void btdrv_tx(const unsigned char *data, unsigned int len)
{
    BT_DRV_TRACE(0,"tx");
    btdrv_tx_flag = 1;
}

#ifdef NORMAL_TEST_MODE_SWITCH
typedef int (*btdrv_testmode_nonsig_rx_process_func)(const unsigned char *data, unsigned int len);
btdrv_testmode_nonsig_rx_process_func btdrv_nonsig_test_result_callback = NULL;
void btdrv_testmode_register_nonsig_rx_process(btdrv_testmode_nonsig_rx_process_func cb)
{
    btdrv_nonsig_test_result_callback = cb;
}

static void btdrv_testmode_nonsig_rx_handler(const unsigned char *data, unsigned int len)
{
    if(btdrv_nonsig_test_result_callback !=NULL)
    {
        btdrv_nonsig_test_result_callback(data, len);
    }
}
#endif

void btdrv_reduce_edge_chl_txpwr_signal_mode(const unsigned char *data)
{
    if (data[0] == 0x01 && data[1] == 0x87 && data[2] == 0xfc && data[3] == 0x1c && data[4] == 01) {
        BTRF_REG_SET_FIELD(0x2E8, 0x1, 7, 0);
    } else if (data[0] == 0x01 && data[1] == 0x87 && data[2] == 0xfc && data[3] == 0x1c && data[4] == 02) {
        BTRF_REG_SET_FIELD(0x2E8, 0x1, 7, 1);
    }
}

void btdrv_dut_accessible_mode_manager(const unsigned char *data);

static unsigned int btdrv_rx(const unsigned char *data, unsigned int len)
{
    hal_intersys_stop_recv(HAL_INTERSYS_ID_0);
    if(len > 5 && data[0] == 0x04 && data[1] == 0xff &&
        data[3] == 0x01)
    {
        BT_DRV_TRACE(2,"%s", &data[5]);
    }
    else
    {
        BT_DRV_TRACE(2,"%s len:%d", __func__, len);
        BT_DRV_DUMP("%02x ", data, len>7?7:len);
    }

    bt_tester_cmd_receive_evt_analyze(data, len);

#ifdef NORMAL_TEST_MODE_SWITCH
    btdrv_testmode_nonsig_rx_handler(data, len);
#endif
    btdrv_dut_accessible_mode_manager(data);
    hal_intersys_start_recv(HAL_INTERSYS_ID_0);

    return len;
}

////open intersys interface for hci data transfer
static bool hci_has_opened = false;

void btdrv_SendData(const uint8_t *buff,uint8_t len)
{
    if(hci_has_opened)
    {
        btdrv_tx_flag = 0;

        BT_DRV_TRACE(1,"%s", __func__);
        BT_DRV_DUMP("%02x ", buff, len);
        hal_intersys_send(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, buff, len);

        while( (btdrv_dut_mode_enable==0) && btdrv_tx_flag == 0);
    }
    else
    {
        //only for bridge mode
        btdrv_bridge_send_data(buff, len);
        btdrv_delay(50);
    }
}


void btdrv_hciopen(void)
{
    int ret = 0;

    if (hci_has_opened)
    {
        return;
    }

    hci_has_opened = true;

    ret = hal_intersys_open(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, btdrv_rx, btdrv_tx, false);

    if (ret)
    {
        BT_DRV_TRACE(0,"Failed to open intersys");
        return;
    }

    hal_intersys_start_recv(HAL_INTERSYS_ID_0);
}

////open intersys interface for hci data transfer
void btdrv_hcioff(void)
{
    if (!hci_has_opened)
    {
        return;
    }
    hci_has_opened = false;

    hal_intersys_close(HAL_INTERSYS_ID_0,HAL_INTERSYS_MSG_HCI);
}

/*  btdrv power on or off the bt controller*/
void btdrv_poweron(uint8_t en)
{
    if (en) {
        //power on BTC
        hal_psc_bt_enable();
        hal_cmu_bt_clock_enable();
        hal_cmu_bt_sys_reset_clear();
        hal_cmu_bt_module_init();
        btdrv_delay(10);
    } else {
        //power off BTC
        hal_cmu_bt_reset_set();
        hal_cmu_bt_clock_disable();
        hal_psc_bt_disable();
    }
}

void bt_drv_extra_config_after_init(void)
{
}

void btdrv_rxbb_rccal(void)
{
    uint16_t value = 0;
    uint16_t rccal_count = 0;
    const uint16_t base_count = 0x80C;
    float quotient = 0.0f;
    float factor;
    uint16_t filter_cap;
    uint16_t i2v_cap;
    uint16_t adc_cap;

    uint16_t rf_store;
    uint32_t dig_store;

    dig_store = BTDIGITAL_REG(0xD0220C00);
    btdrv_read_rf_reg(0x102, &rf_store);

    BTDIGITAL_REG_WR(0xD0220C00, 0x000A0080);       //rx on
    btdrv_delay(10);

    BTRF_REG_SET_FIELD(0x34B, 0x1, 13, 1);           //reg_rcosc_pu
    btdrv_delay(1);
    BTRF_REG_SET_FIELD(0x102, 0x1, 4, 1);           //reg_rccal_en
    btdrv_delay(10);

    btdrv_read_rf_reg(0x103, &value);
    BT_DRV_TRACE(2, "%s rf_103=0x%x", __func__, value);
    //bit12: dig_rccal_finish
    if (getbit(value, 12)) {
        rccal_count = value & 0xFFF;                //dig_rccal_count[11:0]
        BT_DRV_TRACE(2, "%s rccal_count=0x%x", __func__, rccal_count);
    } else {
        BT_DRV_TRACE(1, "%s dig rccal not finish!!!", __func__);
        return;
    }

    quotient = (float)((float)rccal_count / (float)base_count);
    //TRACE(1, "quotient=%f", (double)quotient);
    if ((quotient > 0.7f) && (quotient < 1.3f)) {
        factor = quotient;
    } else {
        // use efuse calib
        BT_DRV_TRACE(0, "quotient not in range [0.7, 1.3]");
        factor = 1.0f;
    }
    // flt cap
    BTRF_REG_GET_FIELD(0x3A6, 0x7F, 7, filter_cap);
    filter_cap = (uint16_t)((filter_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x3A6, 0x7F, 7, filter_cap);  //1m
    BTRF_REG_GET_FIELD(0x3A6, 0x7F, 0, filter_cap);
    filter_cap = (uint16_t)((filter_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x3A6, 0x7F, 0, filter_cap); //2m
    BTRF_REG_GET_FIELD(0x3A5, 0x7F, 1, filter_cap);
    filter_cap = (uint16_t)((filter_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x3A5, 0x7F, 1, filter_cap); //4m

    //bt_table
    BTRF_REG_GET_FIELD(0x4D, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x4D, 0x7F, 0, i2v_cap);  //agc gain 0
    BTRF_REG_GET_FIELD(0x4E, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x4E, 0x7F, 7, i2v_cap);  //agc gain 1
    BTRF_REG_GET_FIELD(0x4F, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x4F, 0x7F, 0, i2v_cap);  //agc gain 2
    BTRF_REG_GET_FIELD(0x50, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x50, 0x7F, 7, i2v_cap);  //agc gain 3
    BTRF_REG_GET_FIELD(0x51, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x51, 0x7F, 0, i2v_cap);  //agc gain 4
    BTRF_REG_GET_FIELD(0x52, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x52, 0x7F, 7, i2v_cap);  //agc gain 5
    BTRF_REG_GET_FIELD(0x53, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x53, 0x7F, 0, i2v_cap);  //agc gain 6
    BTRF_REG_GET_FIELD(0x54, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x54, 0x7F, 7, i2v_cap);  //agc gain 7

    //ble_table
    BTRF_REG_GET_FIELD(0x124, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x124, 0x7F, 0, i2v_cap);  //agc gain 0
    BTRF_REG_GET_FIELD(0x125, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x125, 0x7F, 7, i2v_cap);  //agc gain 1
    BTRF_REG_GET_FIELD(0x126, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x126, 0x7F, 0, i2v_cap);  //agc gain 2
    BTRF_REG_GET_FIELD(0x127, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x127, 0x7F, 7, i2v_cap);  //agc gain 3
    BTRF_REG_GET_FIELD(0x128, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x128, 0x7F, 0, i2v_cap);  //agc gain 4
    BTRF_REG_GET_FIELD(0x129, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x129, 0x7F, 7, i2v_cap);  //agc gain 5
    BTRF_REG_GET_FIELD(0x12A, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x12A, 0x7F, 0, i2v_cap);  //agc gain 6
    BTRF_REG_GET_FIELD(0x12B, 0x7F, 7, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x12B, 0x7F, 7, i2v_cap);  //agc gain 7

    //ble_2M_table
    BTRF_REG_GET_FIELD(0x223, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x223, 0x7F, 0, i2v_cap);  //agc gain 0
    BTRF_REG_GET_FIELD(0x224, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x224, 0x7F, 0, i2v_cap);  //agc gain 1
    BTRF_REG_GET_FIELD(0x225, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x225, 0x7F, 0, i2v_cap);  //agc gain 2
    BTRF_REG_GET_FIELD(0x226, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x226, 0x7F, 0, i2v_cap);  //agc gain 3
    BTRF_REG_GET_FIELD(0x227, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x227, 0x7F, 0, i2v_cap);  //agc gain 4
    BTRF_REG_GET_FIELD(0x228, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x228, 0x7F, 0, i2v_cap);  //agc gain 5
    BTRF_REG_GET_FIELD(0x229, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x229, 0x7F, 0, i2v_cap);  //agc gain 6
    BTRF_REG_GET_FIELD(0x22A, 0x7F, 0, i2v_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x22A, 0x7F, 0, i2v_cap);  //agc gain 7

    //adc cap
    BTRF_REG_GET_FIELD(0x38A, 0xFF, 8, adc_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x38A, 0xFF, 8, adc_cap);  //1M_1st
    BTRF_REG_GET_FIELD(0x38C, 0x7F, 7, adc_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x38C, 0x7F, 7, adc_cap);  //1M_2nd
    BTRF_REG_GET_FIELD(0x38A, 0xFF, 0, adc_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x38A, 0xFF, 0, adc_cap);  //2M_1st
    BTRF_REG_GET_FIELD(0x38C, 0x7F, 0, adc_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x38C, 0x7F, 0, adc_cap);  //2M_2nd
    BTRF_REG_GET_FIELD(0x389, 0xFF, 1, adc_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x378, 0xFF, 1, adc_cap);  //4M_1st
    BTRF_REG_GET_FIELD(0x38B, 0x7F, 1, adc_cap);
    i2v_cap = (uint16_t)((i2v_cap/factor)+0.5);
    BTRF_REG_SET_FIELD(0x38B, 0x7F, 1, adc_cap);  //4M_2nd

    //reset
    BTDIGITAL_REG_WR(0xD0220C00, dig_store);
    btdrv_write_rf_reg(0x102, rf_store);
}

#ifdef __PWR_FLATNESS__
#define PWR_FLATNESS_CONST_VAL                         0x11
void btdrv_channel_pwr_flatness(void)
{
    uint8_t rf_8c_val = 0;
    uint16_t rf_ed_val = 0;
    uint8_t RF_ED_9_6[3] = {0};
    uint8_t RF_2A1_9_6 = 0;
    uint32_t Channel[3] = {0xA0000, 0xA0027, 0xA004E};
    uint8_t average_value = 0;

    BTRF_REG_GET_FIELD(0x8c, 0xffff,0,rf_8c_val);
    TRACE(2, "original 0x8C val=0x%x",rf_8c_val);
    BTRF_REG_SET_FIELD(0x8c, 0x1,0,1);          //reg_bt_pa_ldo_pu_dr

    for(int i=0; i<3; i++)
    {
        BTDIGITAL_REG(0xD0220C00)  = Channel[i];
        btdrv_delay(1);

        BTRF_REG_GET_FIELD(0xED, 0xffff, 0, rf_ed_val);
        if (0x0400 & rf_ed_val) //bit10 txpll_capbank_done
        {
            BTRF_REG_GET_FIELD(0xED, 0xf, 6, RF_ED_9_6[i]);
            TRACE(2, "RF_ED_9_6[%d]=0x%x",i,RF_ED_9_6[i]);
        }
        else
        {
            TRACE(2, "[%d]txpll capbank not done,and 0xED=0x%x", i, rf_ed_val);
            return;
        }
    }

    average_value = (RF_ED_9_6[0] + RF_ED_9_6[1] + RF_ED_9_6[2]) / 3 ;
    RF_2A1_9_6 = PWR_FLATNESS_CONST_VAL - average_value;

    if(average_value > PWR_FLATNESS_CONST_VAL)
        RF_2A1_9_6 = 8;

    TRACE(1, "average_value=0x%x and RF_2A1_9_6=0x%x.",average_value,RF_2A1_9_6);
    BTRF_REG_SET_FIELD(0x2A1, 0xf, 0,RF_2A1_9_6);

    BTRF_REG_SET_FIELD(0x8c, 0xffff,0,rf_8c_val);
    BTDIGITAL_REG(0xD0220C00) = 0;
}
#endif

void btdrv_i2v_dccal(void)
{
    uint16_t read_value;
    uint8_t rf_f8_11_6, rf_f8_5_0 = 0;
    uint8_t reg_dc = 0;

    BTRF_REG_SET_FIELD(0xC4,0x3f,2,0x32);
    btdrv_read_rf_reg(0xC4, &read_value);
    BT_DRV_TRACE(1, "write reg 0xC4 val=%x", read_value);

    BTRF_REG_SET_FIELD(0xBD,0x3,4,0x2);
    btdrv_read_rf_reg(0xBD,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBD val=%x",read_value);

    BTRF_REG_SET_FIELD(0x8C,0xf,6,0xD);
    btdrv_read_rf_reg(0x8C,&read_value);
    BT_DRV_TRACE(1,"write reg 0x8C val=%x",read_value);

    BTDIGITAL_REG(0xd0220c00) = 0x000A0080;
    btdrv_delay(1);

    //reg_bt_i2v_dccal_ready_dr
    BTRF_REG_SET_FIELD(0xBD,0x1,0,0);
    btdrv_read_rf_reg(0xBD,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBD val=%x",read_value);

    //reg_rxgain_dr
    BTRF_REG_SET_FIELD(0xB8,0x1,0,1);
    btdrv_read_rf_reg(0xB8,&read_value);
    BT_DRV_TRACE(1,"write reg 0xB8 val=%x",read_value);

    //reg_bt_trx_cal_en
    BTRF_REG_SET_FIELD(0xBE,0x1,0,1);
    btdrv_read_rf_reg(0xBE,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBE val=%x",read_value);

    for(int i=0; i<8; i++)
    {
        //reg_i2v_dc_cal_en
        BTRF_REG_SET_FIELD(0xC4,0x1,0,0);
        btdrv_read_rf_reg(0xC4, &read_value);
        BT_DRV_TRACE(1, "write reg 0xC4 val=%x", read_value);

        btdrv_delay(1);
        BTRF_REG_SET_FIELD(0xB8,0x7,1,i);
        btdrv_read_rf_reg(0xB8, &read_value);
        BT_DRV_TRACE(1, "write reg 0xB8 val=%x", read_value);

        //reg_bt_rxlowifdc_cal_resetn
        BTRF_REG_SET_FIELD(0xBE,0x1,8,1);
        btdrv_read_rf_reg(0xBE, &read_value);
        BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

        BTRF_REG_SET_FIELD(0xBE,0x1,8,0);
        btdrv_read_rf_reg(0xBE, &read_value);
        BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

        //reg_i2v_dc_cal_en
        BTRF_REG_SET_FIELD(0xC4,0x1,0,1);
        btdrv_read_rf_reg(0xC4, &read_value);
        BT_DRV_TRACE(1, "write reg 0xC4 val=%x", read_value);

        BTRF_REG_SET_FIELD(0xBE,0x1,8,1);
        btdrv_read_rf_reg(0xBE, &read_value);
        BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

        btdrv_delay(1);
        //i2v_dccal_value[12:0]
        btdrv_read_rf_reg(0xF8, &read_value);
        BT_DRV_TRACE(1, "read reg 0xF8 val=%x", read_value);
        if(read_value & 0x1000)
        {
            rf_f8_5_0 = (read_value & 0x3f);
            rf_f8_11_6 = ((read_value & 0xfc0) >> 6);
            BT_DRV_TRACE(2, "rf_f8_5_0=0x%x, rf_f8_11_6=0x%x", rf_f8_5_0,rf_f8_11_6);

            reg_dc = 0xc5 + i;
            BTRF_REG_SET_FIELD(reg_dc,0x3f,0,rf_f8_11_6);
            btdrv_read_rf_reg(reg_dc, &read_value);
            BT_DRV_TRACE(2, "write reg 0x%x[5:0] val=%x",reg_dc ,read_value);

            BTRF_REG_SET_FIELD(reg_dc,0x3f,6,rf_f8_5_0);
            btdrv_read_rf_reg(reg_dc, &read_value);
            BT_DRV_TRACE(2, "write reg 0x%x[11:6] val=%x",reg_dc ,read_value);
        }
    }

    //reg_bt_i2v_dccal_ready_dr
    BTRF_REG_SET_FIELD(0xBD,0x1,0,1);
    btdrv_read_rf_reg(0xBD,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBD val=%x",read_value);

    BTRF_REG_SET_FIELD(0xB8,0xf,0,0);
    btdrv_read_rf_reg(0xB8,&read_value);
    BT_DRV_TRACE(1,"write reg 0xB8 val=%x",read_value);

    BTRF_REG_SET_FIELD(0xBE,0x1,8,0);
    btdrv_read_rf_reg(0xBE, &read_value);
    BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

    BTRF_REG_SET_FIELD(0xBE,0x1,0,0);
    btdrv_read_rf_reg(0xBE,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBE val=%x",read_value);

    BTRF_REG_SET_FIELD(0x8C,0xf,6,0);
    btdrv_read_rf_reg(0x8C,&read_value);
    BT_DRV_TRACE(1,"write reg 0x8C val=%x",read_value);

    BTDIGITAL_REG(0xd0220c00) = 0;
}

const uint16_t tx_dac_rf_store[] =
{
    0x0c,
    0x0d,
    0xaf,
    0x93,
    0xb0,
    0xa0,
    0x68,
    0x69,
    0x8b,
    0x3c,
};

const uint32_t tx_dac_dig_store[] =
{
    0xd0350324,
    0xd03500b4,
    0xd0220c00,
};

#ifndef NOSTD
static int16_t* ri;
static int16_t* rq;
static bool tx_dac_flag = true;
#define TX_DAC_CAL_TIME         31

void btdrv_tx_dac_datawrite(void)
{
    if (!tx_dac_flag) {
        return;
    }

    for (int i=0; i<TX_DAC_CAL_TIME; i++) {
        if (i < 30) {
            BTDIGITAL_REG_SET_FIELD((0xd0350080+((5*i/30)*4)), 0x1f, (i%6)*5, ri[i]);
            BTDIGITAL_REG_SET_FIELD((0xd0350094+((5*i/30)*4)), 0x1f, (i%6)*5, rq[i]);
        } else {
            BTDIGITAL_REG_SET_FIELD(0xd03500a8, 0x1f, 0, ri[i]);
            BTDIGITAL_REG_SET_FIELD(0xd03500a8, 0x1f, 5, rq[i]);
        }
    }

    free(ri);
    free(rq);
}

void btdrv_tx_dac_cal(void)
{
    uint16_t value = 0;
    uint32_t i, j = 0;
    uint16_t reg_dac_cal[2] = {0x68, 0x69};
    uint32_t set_value[2] = {0x00000001, 0xfff01fff};
    uint16_t rf8b_set_value[2] = {0};
    uint16_t tx_dac_rf_store_value[12] = {0};
    uint32_t tx_dac_dig_store_value[6] = {0};
    uint16_t ci[2] = {0};
    uint16_t cq[2] = {0};
    uint32_t tbl_size = 0;
    uint16_t shift = 0;
    uint16_t index = 0;
    tx_dac_flag = true;

    ri = (int16_t *) malloc(TX_DAC_CAL_TIME * sizeof(int16_t));
    rq = (int16_t *) malloc(TX_DAC_CAL_TIME * sizeof(int16_t));

    tbl_size = ARRAY_SIZE(tx_dac_rf_store);
    for(i = 0; i < tbl_size; i++) {
        btdrv_read_rf_reg(tx_dac_rf_store[i],&value);
        tx_dac_rf_store_value[i] = value;
        //BT_DRV_TRACE(2,"rx reg=0sx%x,v=0x%x",tx_dac_rf_store[i],tx_dac_rf_store_value[i]);
    }

    tbl_size = ARRAY_SIZE(tx_dac_dig_store);
    for(i = 0; i < tbl_size; i++) {
        tx_dac_dig_store_value[i] = BTDIGITAL_REG(tx_dac_dig_store[i]);
        //BT_DRV_TRACE(2,"dig reg=0x%x,v=0x%x",tx_dac_dig_store[i],tx_dac_dig_store_value[i]);
    }

    //[dac cal config]
    btdrv_write_rf_reg(0x0c, 0x0001);//pa pd
    btdrv_write_rf_reg(0x0d, 0x04FC);//txdac,flt pu tmx pd
    btdrv_write_rf_reg(0xaf, 0xC100);//txflt testen max gain dr
    btdrv_write_rf_reg(0x93, 0x45B5);//tstbuf pu choose txflt
    btdrv_write_rf_reg(0xb0, 0x7c00);//txflt cap cmp pu
    btdrv_write_rf_reg(0xa0, 0x1220);//tx dc

    BTDIGITAL_REG(0xD0350324) = 0x00000009;//dacfifo_bypass
    BTDIGITAL_REG(0xD03500B4) = 0x00000001;//dac_manual
    BTDIGITAL_REG(0xd0220c00) = 0x000a0000;

    btdrv_read_rf_reg(0x8b, &rf8b_set_value[0]);
    rf8b_set_value[1] = rf8b_set_value[0];
    rf8b_set_value[0] &= ~(1 << 10);
    rf8b_set_value[1] |= (1 << 10);

    for(i = 0; i < TX_DAC_CAL_TIME; i++) {
        if(i == 0)
            btdrv_write_rf_reg(0x69, 0x7fff);
        else if(i == 16)
            btdrv_write_rf_reg(0x68, 0x7fff);

        for(j = 0; j < 2; j++) {
            index = (i < 16) ? 0 : 1;
            shift = (i < 16) ? i : (i-16);
            value = (j < 1) ? (1 << shift) : 0;
            btdrv_write_rf_reg(reg_dac_cal[index], value);
            btdrv_write_rf_reg(0x8b, rf8b_set_value[j]);
            BTDIGITAL_REG(0xD03500B4) = set_value[j];//dac_all_0/dac_all_1

            //[dac cal toggle]
            btdrv_write_rf_reg(0x3c, 0x31fd);
            btdrv_delay_us(500);
            btdrv_write_rf_reg(0x3c, 0x39fd);
            btdrv_delay_us(500);
            btdrv_read_rf_reg(0x144, &value);
            //BT_DRV_TRACE(2, "%s i:%d, j:%d, rf_144=0x%x", __func__, i, j, value);
            if(getbit(value, 10)){
                ci[j] = value & 0x3FF;//0x144[9:0], i
                btdrv_read_rf_reg(0x145, &value);
                cq[j] = value & 0x3FF;//0x145[9:0], q
            }else{
                BT_DRV_TRACE(4, "%s not finish!!! i:%d, j:%d, rf_144=0x%x", __func__, i, j, value);
                tx_dac_flag = false;
                free(ri);
                free(rq);
                goto reg_reset;
            }
        }
        ri[i] = ci[1] - ci[0];
        rq[i] = cq[1] - cq[0];

        ri[i] &= 0x1f;
        rq[i] &= 0x1f;
        //TRACE(3, "[%d] ri=%d | rq=%d", i, ri[i] , rq[i] );
    }

reg_reset:
    tbl_size = ARRAY_SIZE(tx_dac_rf_store);
    for(i = 0; i < tbl_size; i++) {
        btdrv_write_rf_reg(tx_dac_rf_store[i],tx_dac_rf_store_value[i]);
        //BTRF_REG_DUMP(tx_dac_rf_store[i]);
    }
    tbl_size = ARRAY_SIZE(tx_dac_dig_store);
    for(i = 0; i < tbl_size; i++) {
        BTDIGITAL_REG(tx_dac_dig_store[i]) = tx_dac_dig_store_value[i];
    }
}
#endif

const uint16_t tx_iq_rf_set[][3] =
{
    // [dc iq cal]
    {0x0038, 0x7f70, 0},
    {0x005d, 0x0b58, 0},//default tx gain table=0
    {0x0076, 0x5524, 0},//pd pa when debug settle down
    {0x00b2, 0x3007, 0},
    {0x00b3, 0x97eb, 0},
    {0x00b4, 0xe217, 0},// iqcal max gain for high irr
    {0x00b5, 0x1000, 0},// att less for hifh irr
    {0x008e, 0x65dc, 0},
    {0x008d, 0x2419, 0},
    {0x008c, 0x3500, 0},
    // {0x00af, 0x4130, 0},
    {0x032a, 0x0030, 0},//i2v pu
    {0x0093, 0x4497, 0},//tst buf gain
    {0x0091, 0x028d, 0},
    {0x0095, 0x2012, 0},//mixer passive mode
    {0x000f, 0x0040, 0},//pd mixer
    {0x03ac, 0x0d9c, 0},//filter 1m bw gain

    // [ADC CAL LOOP]
    {0x000c, 0x0030, 0},
    {0x0041, 0x0800, 1},
    {0x0041, 0x1800, 0},
    {0x0081, 0x004b, 0},
    {0x037e, 0x2e00, 0},
    {0x0309, 0x080f, 1},
    {0x0309, 0x080b, 0},
};

const uint16_t tx_iq_rf_store[][1] =
{
    {0x0038},
    {0x005d},
    {0x0076},
    {0x00b2},
    {0x00b3},
    {0x00b4},
    {0x00b5},
    {0x008e},
    {0x008d},
    {0x008c},
    // {0x00af},
    {0x032a},
    {0x0093},
    {0x0091},
    {0x0095},
    {0x000f},
    {0x03ac},
    {0x000c},
    {0x0041},
    {0x0081},
    {0x037e},
    {0x0309},
};

void btdrv_tx_iq_manual_cal(void)
{
    uint32_t tx_iq_dig_store[10];
    uint32_t i;
    uint16_t value = 0;
    const uint16_t (*tx_iq_rf_set_p)[3];
    const uint16_t (*tx_iq_rf_store_p)[1];
    uint32_t rf_set_tbl_size = 0;
    uint16_t rf_local[ARRAY_SIZE(tx_iq_rf_store)];
    uint32_t rf_store_tbl_size = ARRAY_SIZE(tx_iq_rf_store);

    tx_iq_dig_store[0] = BTDIGITAL_REG(0xD0220C00);
    tx_iq_dig_store[1] = BTDIGITAL_REG(0xD0350028);
    tx_iq_dig_store[2] = BTDIGITAL_REG(0xD035002C);
    tx_iq_dig_store[3] = BTDIGITAL_REG(0xD0350250);
    tx_iq_dig_store[4] = BTDIGITAL_REG(0xD0330058);
    tx_iq_dig_store[5] = BTDIGITAL_REG(0xD0330060);
    tx_iq_dig_store[6] = BTDIGITAL_REG(0xD0310000);
    tx_iq_dig_store[7] = BTDIGITAL_REG(0xD0330064);
    tx_iq_dig_store[8] = BTDIGITAL_REG(0xD0350334);
    tx_iq_dig_store[9] = BTDIGITAL_REG(0xD0340020);
    tx_iq_rf_store_p = &tx_iq_rf_store[0];
    rf_store_tbl_size = ARRAY_SIZE(tx_iq_rf_store);
    BT_DRV_TRACE(0,"rf reg_store:\n");
    for (i=0; i<rf_store_tbl_size; i++) {
        btdrv_read_rf_reg(tx_iq_rf_store_p[i][0],&value);
        rf_local[i] = value;
        BT_DRV_TRACE(2,"rf reg=%x,v=%x",tx_iq_rf_store_p[i][0],value);
    }

    BTDIGITAL_REG_WR(0xD0220C00, 0x000A0027);
    BTDIGITAL_REG_WR(0xD0350028, 0x03000012);       //one tone set gain
    BTDIGITAL_REG_WR(0xD035002C, 0x3);              //187.5k tone
    // rx modem on
    BTDIGITAL_REG_WR(0xD0340020, 0x030E01C0);
    osDelay(10);
    BTDIGITAL_REG_WR(0xD0220C00, 0x800A0027);
    BTDIGITAL_REG_WR(0xd0350240, 0x0000A801);

    tx_iq_rf_set_p = &tx_iq_rf_set[0];
    rf_set_tbl_size = ARRAY_SIZE(tx_iq_rf_set);
    BT_DRV_TRACE(0,"rf reg_set:\n");
    for (i=0; i<rf_set_tbl_size; i++) {
        btdrv_write_rf_reg(tx_iq_rf_set_p[i][0],tx_iq_rf_set_p[i][1]);
        osDelay(tx_iq_rf_set_p[i][2]);
        //BTRF_REG_DUMP(tx_iq_rf_set_p[i][0]);
    }


    BTDIGITAL_REG_WR(0xD0350248, 0xdf800168);

    //dc_iq_calib
#ifdef TX_IQ_CAL
    dc_iq_calib_1603();
#endif

    BT_DRV_TRACE(0,"reg_reset:\n");

    for (i=0; i<rf_store_tbl_size; i++) {
        btdrv_write_rf_reg(tx_iq_rf_store_p[i][0],rf_local[i]);
        // BT_DRV_TRACE(2,"rf reg=%x,v=%x",tx_iq_rf_store_p[i][0],rf_local[i]);
    }

    BTDIGITAL_REG_WR(0xD0220C00, tx_iq_dig_store[0]);
    BTDIGITAL_REG_WR(0xD0350028, tx_iq_dig_store[1]);
    BTDIGITAL_REG_WR(0xD035002C, tx_iq_dig_store[2]);
//    BTDIGITAL_REG_WR(0xD0350250, tx_iq_dig_store[3]);
    BTDIGITAL_REG_WR(0xD0330058, tx_iq_dig_store[4]);
    BTDIGITAL_REG_WR(0xD0330060, tx_iq_dig_store[5]);
//    BTDIGITAL_REG_WR(0xD0310000, tx_iq_dig_store[6]);
    BTDIGITAL_REG_WR(0xD0330064, tx_iq_dig_store[7]);
    BTDIGITAL_REG_WR(0xD0350334, tx_iq_dig_store[8]);
    BTDIGITAL_REG_WR(0xD0340020, tx_iq_dig_store[9]);
}

#ifdef DPD_ONCHIP_CAL

static bool dpd_mem_data_exist_flag = false;

int bt_drv_tx_dpd_write_memory(void)
{
    if (NULL == bt_drv_dpd_mem) {
        TRACE(0, "dpd mem data is empty.");
        return 0;
    }

    uint32_t i = 0;
    const int32_t * dpd_mem_p = bt_drv_dpd_mem;
    uint32_t dpd_mem_size = ARRAY_SIZE(bt_drv_dpd_mem);

    for (i=0; i<dpd_mem_size; i++) {
        BTDIGITAL_REG(0xC0520000+i*4) = dpd_mem_p[i];
    }

    for (i=0; i<10; i++) {
        if (BTDIGITAL_REG(0xC0520000+i*4) != (uint32_t)dpd_mem_p[i]) {
            TRACE(0, "dpd mem data invalid.");
            return 0;
        }
    }

    dpd_mem_data_exist_flag = true;
    return 1;
}

const uint16_t dpd_cal_rf_set[][3] =
{
    // [dpd config]
    {0x0062, 0x0216, 0},
    {0x036E, 0x9387, 0},
    {0x00c6, 0x6880, 0},//txen rx disable
    {0x008e, 0x0070, 0},//i2v_filter max gain
    {0x008d, 0x2575, 0},//i2v max gain
    {0x008c, 0x2500, 0},//i2v bw
    {0x032a, 0x0030, 0},//i2v pu
    //[tst_buf_config]//
    {0x0093, 0x4495, 0},// tst buf pu  4494&TST BUF MAX GAIN=449F
    {0x0091, 0x0295, 0},//dpd path tstbuf filter out
    {0x0094, 0x7C80, 0},
    {0x0095, 0x2012, 0},//mixer passive mode
    {0x000f, 0x0040, 0},//pd mixer
    {0x00b7, 0x10e7, 0},//phase alter& rf att
    {0x00b6, 0x4e72, 0},//dpd bias
    {0x00bf, 0x20a2, 0},//dpd lo en
    {0x037f, 0x1EF7, 0},//RFVCO_DPD_EN
    {0x000c, 0x0030, 0},
    {0x0081, 0x004b, 0},
    {0x037e, 0x2e00, 0},
    {0x0309, 0x080f, 1},
    {0x0309, 0x080b, 0},
    {0x0038, 0x7f70, 0},//gain table dr
    {0x0384, 0x3FE0, 0},
    {0x00ea, 0x0000, 0},
    {0x0038, 0x7FB0, 0},
    {0x037B, 0x0540, 0},
    {0x038D, 0x0248, 0},
    {0x0041, 0x0000, 0},
    {0x0097, 0x4C00, 0},
    {0x03A9, 0x0004, 0},
    {0x03AA, 0x0000, 0},//improve pa vcm
};

const uint16_t dpd_cal_rf_store[][1] =
{
    {0x0062},
    {0x036E},
    {0x00c6},
    {0x008e},
    {0x008d},
    {0x008c},
    {0x032a},
    {0x0093},
    {0x0091},
    {0x0094},
    {0x0095},
    {0x000f},
    {0x00b7},
    {0x00b6},
    {0x00bf},
    {0x037f},
    {0x000c},
    {0x0081},
    {0x037e},
    {0x0309},
    {0x0309},
    {0x0038},
    {0x0384},
    {0x00ea},
    {0x0038},
    {0x037B},
    {0x038D},
    {0x0041},
    {0x0097},
    {0x03A9},
    {0x03AA},
};

void btdrv_dpd_cal_setup(void)
{
    uint32_t i = 0;
    uint16_t value = 0;
    uint32_t dpd_dig_store[8];

    const uint16_t (*dpd_cal_rf_set_p)[3];
    const uint16_t (*dpd_cal_rf_store_p)[1];
    uint32_t set_tbl_size = 0;
    uint32_t store_tbl_size = 0;

    dpd_dig_store[0] = BTDIGITAL_REG(0xD0340080);
    dpd_dig_store[1] = BTDIGITAL_REG(0xD0340084);
    dpd_dig_store[2] = BTDIGITAL_REG(0xD0350024);
    dpd_dig_store[3] = BTDIGITAL_REG(0xD0350218);
    dpd_dig_store[4] = BTDIGITAL_REG(0xD0220C00);
    dpd_dig_store[5] = BTDIGITAL_REG(0xD0330038);
    dpd_dig_store[6] = BTDIGITAL_REG(0xD0350240);
    dpd_dig_store[7] = BTDIGITAL_REG(0xD0340020);

    dpd_cal_rf_store_p = &dpd_cal_rf_store[0];
    store_tbl_size = ARRAY_SIZE(dpd_cal_rf_store);
    uint16_t rf_backup[store_tbl_size];

    BT_DRV_TRACE(1,"%s reg_store:\n", __func__);
    for (i=0; i<store_tbl_size; i++) {
        btdrv_read_rf_reg(dpd_cal_rf_store_p[i][0], &value);
        rf_backup[i] = value;
        BT_DRV_TRACE(2,"reg=%x,v=%x",dpd_cal_rf_store_p[i][0],value);
    }

    dpd_cal_rf_set_p = &dpd_cal_rf_set[0];
    set_tbl_size = ARRAY_SIZE(dpd_cal_rf_set);
    BT_DRV_TRACE(1,"%s reg_set:\n", __func__);
    for (i=0; i<set_tbl_size; i++) {
        btdrv_write_rf_reg(dpd_cal_rf_set_p[i][0], dpd_cal_rf_set_p[i][1]);
        osDelay( dpd_cal_rf_set_p[i][2]);
        //BTRF_REG_DUMP(dpd_cal_rf_set_p[i][0]);
    }

    BTDIGITAL_REG_WR(0xD0330038, 0x00A8108C);
    BTDIGITAL_REG_WR(0xD0350240, 0x0000A801);
    // reset adc
    btdrv_write_rf_reg(0x41, 0x0800);
    osDelay(1);
    btdrv_write_rf_reg(0x41, 0x1800);
    //[tx from memory]
    BTDIGITAL_REG_SET_FIELD(0xd0350024, 0x1, 20, 1);
    BTDIGITAL_REG_WR(0xD0350218, 0x583);
    osDelay(10);
    BTDIGITAL_REG_WR(0xD0340020, 0x030E01C0);
    btdrv_dpd_cal_entity();

    for (i=0; i<store_tbl_size; i++) {
        btdrv_write_rf_reg(dpd_cal_rf_store_p[i][0],rf_backup[i]);
    }

    BTDIGITAL_REG_WR(0xD0340080, dpd_dig_store[0]);
    BTDIGITAL_REG_WR(0xD0340084, dpd_dig_store[1]);
    BTDIGITAL_REG_WR(0xD0350024, dpd_dig_store[2]);
    BTDIGITAL_REG_WR(0xD0350218, dpd_dig_store[3]);
    BTDIGITAL_REG_WR(0xD0220C00, dpd_dig_store[4]);
    BTDIGITAL_REG_WR(0xD0330038, dpd_dig_store[5]);
    BTDIGITAL_REG_WR(0xD0350240, dpd_dig_store[6]);
    BTDIGITAL_REG_WR(0xD0340020, dpd_dig_store[7]);
}

void btdrv_dpd_cal(void)
{
    uint32_t ram_sel, sleep_cfg;

    // TODO: If M55 is main MCU, this should be done in M55 startup process (not in BTH startup process)

    hal_cmu_btc_dpd_ram_enable(&ram_sel, &sleep_cfg);

    if (bt_drv_tx_dpd_write_memory()) {
        hal_btdump_clk_enable();
        btdrv_dpd_cal_setup();
        hal_btdump_clk_disable();
    }

    hal_cmu_btc_dpd_ram_disable(ram_sel, sleep_cfg);
}
#endif

void bt_drv_calibration_init(void)
{
#ifndef NOSTD
    // btdrv_txpower_calib();
    // btdrv_logen_cap_cal();
    // btdrv_tx_dac_cal();
    btdrv_tx_iq_manual_cal();
    //TODO
    // btdrv_rxbb_rccal();
    //btdrv_i2v_dccal();

#ifdef RX_IQ_CAL
    //hal_btdump_clk_enable();
    //bt_iq_calibration_setup();
    //hal_btdump_clk_disable();
#endif

    //TO DO: RF calibration
#ifdef __PWR_FLATNESS__
    //btdrv_channel_pwr_flatness();
#endif
#endif
}

static uint16_t initial_temperature = 0;

static void compensate_irqhandler(uint16_t current_temp, HAL_GPADC_MV_T volt)
{
    static bool initial = false;
    static enum PMU_CHIP_TYPE_T pmu_type;
    int16_t delta_temp = 0;
    uint16_t efuse_val = 0;
    static uint8_t bt_txgain_initial = 0;
    uint8_t bt_txgain_final = 0;
    const uint16_t dig_gain_initial = 512;
    uint16_t dig_gain = 0;
    int temperature_celsius = 0;

    if (!initial) {
        pmu_get_efuse(PMU_EFUSE_PAGE_TEMPERATURE, &efuse_val);
        //Bit[11:0] records the ambient temperature when the chip is turned on
        initial_temperature = efuse_val & 0xFFF;

        pmu_type = pmu_get_chip_type();

        BTRF_REG_GET_FIELD(0x62, 0x3F, 0, bt_txgain_initial);

        initial = true;
    }

    delta_temp = current_temp - initial_temperature;
    TRACE(4, "initial_temperature=%d, current_temp=%d, bt_txgain_initial=%d, delta_temp=%d.",
        initial_temperature, current_temp, bt_txgain_initial, delta_temp);

    bt_txgain_final = bt_txgain_initial;
    dig_gain = dig_gain_initial;

    if (PMU_CHIP_TYPE_1803 == pmu_type) {
        if (delta_temp < 0) {
            dig_gain = (uint16_t)((float)dig_gain_initial * (float)(1.0 + (float)delta_temp*14/8400));
        } else {
            bt_txgain_final = bt_txgain_initial + (int8_t)((float)(3*delta_temp)/70 + 0.5f);
        }
    } else if (PMU_CHIP_TYPE_1805 == pmu_type) {
        if (delta_temp < 0) {
            dig_gain = (uint16_t)((float)dig_gain_initial * (float)(1.0 + (float)delta_temp/3000));
        } else {
            bt_txgain_final = bt_txgain_initial + (int8_t)((float)(3*delta_temp)/290 + 0.5f);
        }
    }

    TRACE(2, "bt_txgain_final=%d, dig_gain=%d", (bt_txgain_final & 0x3F), dig_gain);
    BTRF_REG_SET_FIELD(0x62, 0x3F, 0, (bt_txgain_final & 0x3F));

    btdrv_temperature_dig_comp(dig_gain);

    temperature_celsius = pmu_volt2temperature(volt);
    btdtv_rf_vco_drv_workaround(temperature_celsius);

    pmu_ntc_capture_disable();
}

static POSSIBLY_UNUSED void btdrv_compensate_power_based_on_temperature(void)
{
    pmu_ntc_capture_enable();
    hal_gpadc_open(HAL_GPADC_CHAN_0, HAL_GPADC_ATP_ONESHOT, compensate_irqhandler);
}

void btdrv_start_bt(void)
{
    enum HAL_IOMUX_ISPI_OWNER_T ispi_owner;

#ifdef CHIP_SUBSYS_BTH
    ispi_owner = HAL_IOMUX_ISPI_OWNER_BTH;
#else
    ispi_owner = HAL_IOMUX_ISPI_OWNER_SYS;
#endif
    hal_iomux_ispi_access_enable(HAL_IOMUX_ISPI_MOD_RF, ispi_owner);

    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_52M);

    btdrv_common_init();

#if INTERSYS_DEBUG
    btdrv_trace_config(BT_CONTROLER_TRACE_TYPE_INTERSYS   |
                       BT_CONTROLER_TRACE_TYPE_CONTROLLER |
                       BT_CONTROLER_FILTER_TRACE_TYPE_A2DP_STREAM |
                       BT_CONTROLER_TRACE_TYPE_SPUV_HCI_BUFF |
                       BT_CONTROLER_TRACE_TYPE_ACL_PACKET);

#ifdef IS_HCI_ISO_PACKET_TRACE_ENABLED
    btdrv_set_iso_packet_trace_enable();
#else
    btdrv_set_iso_packet_trace_disable();
#endif
#endif

    //Symbol init should be done before powering on BTC
    bt_drv_reg_op_global_symbols_init();
    //power on BTC
    btdrv_poweron(BT_POWERON);

#ifndef SIMU
    //RF module
    btdrv_rf_init();
#endif
    //Digital module
    btdrv_digital_init();

#if defined (BT_EXT_LNA_PA) || defined(BT_ITN_SGL_LNA)
    int LNA_flag = 0,PA_flag = 0;
#ifdef BT_EXT_LNA
    LNA_flag = 1;
#endif
#if defined (BT_EXT_PA) || defined(BT_ITN_SGL_LNA)
    PA_flag = 1;
#endif
    btdrv_enable_rf_sw(LNA_flag,PA_flag);
#endif

#ifndef SIMU
    //calibration module init
    bt_drv_calibration_init();
#endif

#ifdef DPD_ONCHIP_CAL
    btdrv_dpd_cal();
#endif

#ifdef BTH_AS_MAIN_MCU
    // btdrv_register_temp_compensation_callback(btdrv_compensate_power_based_on_temperature);
#endif
    // Clear calib state by resetting the whole bt domain
    hal_cmu_bt_reset_set();
    btdrv_delay(2);
    hal_cmu_bt_sys_reset_clear();
    btdrv_delay(2);
#ifdef __BT_RAMRUN_NEW__
    bt_controller_open(NULL, NULL);
    BT_DRV_TRACE(1,"bt ramrun %s", BT_CONTROLLER_COMMIT_ID);
    BT_DRV_TRACE(1,"bt ramrun %s", BT_CONTROLLER_COMMIT_DATE);
    btdrv_delay(100);
#else
    hal_cmu_bt_cpu_reset_clear();
    btdrv_delay(2);
    //ROM patch init
    btdrv_ins_patch_init();
#endif
    //Select intersys mode before first HCI
#if defined(__SYS_AS_MAIN__)
    bt_drv_reg_op_sel_btc_intersys(1);
#endif
    while(!btdrv_reg_op_check_btc_boot_finish());

    btdrv_config_init();
#if !defined(PROGRAMMER) && !defined(SIMU)
    btdrv_cal_data_write();
#endif

#ifdef BTC_CPUDUMP_BASE
    btdrv_cpudump_clk_enable();
    btdrv_cpudump_enable();
#endif
    btdrv_hciopen();
    btdrv_hciprocess();
    btdrv_hcioff();

    bt_drv_reg_op_init();
    //BT driver init end
    btdrv_config_end();

    /*reg controller crash dump*/
    hal_trace_crash_dump_register(HAL_TRACE_CRASH_DUMP_MODULE_BT, btdrv_btc_fault_dump);
    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_32K);
}

const uint8_t hci_cmd_enable_dut[] =
{
    0x01,0x03, 0x18, 0x00
};
const uint8_t hci_cmd_enable_allscan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x03
};
const uint8_t hci_cmd_disable_scan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x00
};
const uint8_t hci_cmd_enable_pagescan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x02
};
const uint8_t hci_cmd_autoaccept_connect[] =
{
    0x01,0x05, 0x0c, 0x03, 0x02, 0x00, 0x02
};
const uint8_t hci_cmd_hci_reset[] =
{
    0x01,0x03,0x0c,0x00
};

const uint8_t hci_cmd_inquiry_scan_activity[] =
{
    0x01,0x1e,0x0c,0x04,0x40,0x00,0x12,0x00
};

const uint8_t hci_cmd_page_scan_activity[] =
{
    0x01,0x1c,0x0c,0x04,0x40,0x00,0x12,0x00
};


const uint8_t hci_cmd_nonsig_tx_dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x04, 0x1b, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_2dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x04, 0x36, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_3dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x04, 0x53, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_2dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x04, 0x6f, 0x01,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_3dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x04, 0x28, 0x02,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};

const uint8_t hci_cmd_nonsig_rx_dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x00, 0x1b, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_2dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x00, 0x36, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_3dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x00, 0x53, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_2dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x00, 0x6f, 0x01,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_3dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x00, 0x28, 0x02,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};

//vco test
const uint8_t hci_cmd_start_bt_vco_test[] =
{
    0x01, 0xaa, 0xfc, 0x02, 0x00,0x02
};
const uint8_t hci_cmd_stop_bt_vco_test[] =
{
    0x01, 0xaa, 0xfc, 0x02, 0x00,0x04
};

const uint8_t hci_cmd_enable_ibrt_test[] =
{
    0x01, 0xb4, 0xfc, 0x01, 0x01
};

const uint8_t hci_cmd_set_ibrt_mode[] =
{
    0x01, 0xa2, 0xfc, 0x02, 0x01,0x00
};

#ifdef __PA_UNSTABLE_WORKAROUND__
static void btdrv_pa_issue_workaround(bool en)
{
    if(en)
    {
        btdrv_spi_trig_init();
        btdrv_set_spi_trig_enable(0);//spi0 select
        //set btsw txen and dr
        BTRF_REG_SET_FIELD(REG_RF_BTSW_CNTL_ADDR, 0x3, 13, 0x3);
    }
    else
    {
        btdrv_clear_spi_trig_enable(0);//spi0 deinit
        btdrv_clear_spi_trig_pos_enable();
        //clear btsw txen and dr
        BTRF_REG_SET_FIELD(REG_RF_BTSW_CNTL_ADDR, 0x3, 13, 0x0);
    }
}
#endif

void btdrv_testmode_start(void)
{
#ifdef __BLE_NEW_SWAGC_MODE__
    //BLE sync agc can only be used in BLE test mode
    bt_drv_reg_op_ble_sync_agc_mode_set(true);
#endif
#ifdef __PA_UNSTABLE_WORKAROUND__
    if (hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_2)
    {
        btdrv_pa_issue_workaround(true);
    }
#endif
#ifdef __HW_AGC__
    bt_drv_rf_hw_agc_mode_set(RX_GAIN_IDX_0, HW_AGC_HIGH_PWEF_MODE);
#endif

}

void btdrv_testmode_end(void)
{
#ifdef __BLE_NEW_SWAGC_MODE__
    bt_drv_reg_op_ble_sync_agc_mode_set(false);
#endif
#ifdef __PA_UNSTABLE_WORKAROUND__
    if (hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_2)
    {
        btdrv_pa_issue_workaround(false);
    }
#endif
}

void btdrv_write_localinfo(const char *name, uint8_t len, const uint8_t *addr)
{
    uint8_t hci_cmd_write_addr[5+6] =
    {
        0x01, 0x72, 0xfc, 0x07, 0x00
    };

    uint8_t hci_cmd_write_name[248+4] =
    {
        0x01, 0x13, 0x0c, 0xF8
    };
    memset(&hci_cmd_write_name[4], 0, sizeof(hci_cmd_write_name)-4);
    memcpy(&hci_cmd_write_name[4], name, len);
    btdrv_SendData(hci_cmd_write_name, sizeof(hci_cmd_write_name));
    btdrv_delay(50);
    memcpy(&hci_cmd_write_addr[5], addr, 6);
    btdrv_SendData(hci_cmd_write_addr, sizeof(hci_cmd_write_addr));
    btdrv_delay(20);
}

void btdrv_enable_dut(void)
{
    btdrv_SendData(hci_cmd_enable_dut, sizeof(hci_cmd_enable_dut));
    btdrv_delay(100);
    btdrv_SendData(hci_cmd_enable_allscan, sizeof(hci_cmd_enable_allscan));
    btdrv_delay(100);
    btdrv_SendData(hci_cmd_autoaccept_connect, sizeof(hci_cmd_autoaccept_connect));
    btdrv_delay(100);
    btdrv_dut_mode_enable = true;
}

void btdrv_enable_dut_again(void)
{
    btdrv_SendData(hci_cmd_enable_allscan, sizeof(hci_cmd_enable_allscan));
    btdrv_delay(20);
    btdrv_dut_mode_enable = true;
}

void btdrv_enable_ibrt_test(void)
{
    btdrv_SendData(hci_cmd_enable_ibrt_test, sizeof(hci_cmd_enable_ibrt_test));
    btdrv_delay(20);
    btdrv_SendData(hci_cmd_set_ibrt_mode, sizeof(hci_cmd_set_ibrt_mode));
    btdrv_delay(20);
}

void btdrv_connect_ibrt_device(uint8_t *addr)
{
    uint8_t hci_cmd_connect_device[17] =
    {
        0x01, 0x05, 0x04, 0x0D, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0x18, 0xCC,
        0x01, 0x00, 0x00, 0x00, 0x00
    };
    memcpy(&hci_cmd_connect_device[4],addr,6);
    btdrv_SendData(hci_cmd_connect_device, sizeof(hci_cmd_connect_device));
    btdrv_delay(50);

}

void btdrv_disable_scan(void)
{
    btdrv_SendData(hci_cmd_disable_scan, sizeof(hci_cmd_disable_scan));
    btdrv_delay(20);
}

static uint32_t dut_connect_status = DUT_CONNECT_STATUS_DISCONNECTED;

uint32_t btdrv_dut_get_connect_status(void)
{
    return dut_connect_status;
}

void btdrv_enable_dut_again(void);
void btdrv_dut_accessible_mode_manager(const unsigned char *data)
{
    if(btdrv_dut_mode_enable)
    {
        if(data[0]==0x04&&data[1]==0x03&&data[2]==0x0b&&data[3]==0x00)
        {
#ifdef __IBRT_IBRT_TESTMODE__
            if(memcmp(&data[6],bt_addr,6))
            {
                btdrv_disable_scan();
            }
#else
            btdrv_disable_scan();
#endif
            dut_connect_status = DUT_CONNECT_STATUS_CONNECTED;
        }
        else if(data[0]==0x04&&data[1]==0x05&&data[2]==0x04&&data[3]==0x00)
        {
            btdrv_enable_dut_again();
            dut_connect_status = DUT_CONNECT_STATUS_DISCONNECTED;
        }
    }
}

void btdrv_hci_reset(void)
{
    btdrv_SendData(hci_cmd_hci_reset, sizeof(hci_cmd_hci_reset));
    btdrv_delay(350);
}

void btdrv_enable_nonsig_tx(uint8_t index)
{
    BT_DRV_TRACE(1,"%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh1_pn9_t0, sizeof(hci_cmd_nonsig_tx_2dh1_pn9_t0));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh1_pn9_t0, sizeof(hci_cmd_nonsig_tx_3dh1_pn9_t0));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh3_pn9_t0, sizeof(hci_cmd_nonsig_tx_2dh1_pn9_t0));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh3_pn9_t0, sizeof(hci_cmd_nonsig_tx_3dh1_pn9_t0));
    else
        btdrv_SendData(hci_cmd_nonsig_tx_dh1_pn9_t0, sizeof(hci_cmd_nonsig_tx_dh1_pn9_t0));

    btdrv_delay(20);

}

void btdrv_enable_nonsig_rx(uint8_t index)
{
    BT_DRV_TRACE(1,"%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh1_pn9_t0, sizeof(hci_cmd_nonsig_rx_2dh1_pn9_t0));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh1_pn9_t0, sizeof(hci_cmd_nonsig_rx_3dh1_pn9_t0));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh3_pn9_t0, sizeof(hci_cmd_nonsig_rx_2dh1_pn9_t0));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh3_pn9_t0, sizeof(hci_cmd_nonsig_rx_3dh1_pn9_t0));
    else
        btdrv_SendData(hci_cmd_nonsig_rx_dh1_pn9_t0, sizeof(hci_cmd_nonsig_rx_dh1_pn9_t0));

    btdrv_delay(20);
}

static bool btdrv_vco_test_running = false;
static uint16_t vco_test_reg_val_309 = 0;
static uint16_t vco_test_reg_val_38 = 0;
#ifdef VCO_TEST_TOOL
static unsigned short vco_test_hack_flag = 0;
static unsigned short vco_test_channel = 0xff;
static unsigned short set_xatl_facp_hack_flag = 0;
static unsigned short vco_test_xtal_fcap = 0xffff;

unsigned short btdrv_get_vco_test_process_flag(void)
{
    return vco_test_hack_flag;
}

bool btdrv_vco_test_bridge_intsys_callback(const unsigned char *data)
{
    bool status = false;
    if(data[0]==0x01 &&data[1]==0xaa&&data[2]==0xfc &&data[3]==0x02)
    {
        status = true;
        vco_test_hack_flag = data[5];
        vco_test_channel = data[4];
    }

    return status;
}

void btdrv_vco_test_process(uint8_t op)
{
    if(op == 0x02)//vco test start
    {
        if(vco_test_channel != 0xff)
            btdrv_vco_test_start(vco_test_channel);
    }
    else if(op ==0x04)//vco test stop
    {
        btdrv_vco_test_stop();
    }
    vco_test_channel =0xff;
    vco_test_hack_flag = 0;
}

unsigned short btdrv_get_vco_test_set_xtal_fcap_process_flag(void)
{
    return set_xatl_facp_hack_flag;
}

bool btdrv_vco_test_set_xtal_fcap_bridge_intsys_callback(const unsigned char *data)
{
    bool status = false;
    if(data[0]==0x01 && data[1]==0xab && data[2]==0xfc && data[3]==0x03)
    {
        status = true;
        vco_test_xtal_fcap = (data[5] << 8) | data[4];
        set_xatl_facp_hack_flag = data[6];
    }

    return status;
}

void btdrv_vco_test_set_xtal_fcap_process(uint8_t op)
{
    if(op == 0x02)// set_xtal_fcap
    {
        btdrv_rf_init_xtal_fcap(vco_test_xtal_fcap);
    }
    else if (op == 0x04)// save_xtal_fcap
    {
        nvrec_dev_set_xtal_fcap(vco_test_xtal_fcap);
    }
    vco_test_xtal_fcap = 0xffff;
    set_xatl_facp_hack_flag = 0;
}
#endif

void btdrv_vco_test_start(uint8_t chnl)
{
    if (!btdrv_vco_test_running)
    {
        btdrv_vco_test_running = true;
        btdrv_read_rf_reg(0x309, &vco_test_reg_val_309);
        btdrv_read_rf_reg(0x38, &vco_test_reg_val_38);

        btdrv_reg_op_vco_test_start(chnl);

        BTRF_REG_SET_FIELD(0x309, 0x1, 11, 0);   //Transmit signal without modulation
        BTRF_REG_SET_FIELD(0x38, 0xF, 4, 9);     //tx gain dr idx5
    }
}

void btdrv_vco_test_stop(void)
{
    if (btdrv_vco_test_running)
    {
        btdrv_vco_test_running = false;
        btdrv_reg_op_vco_test_stop();

        if(0 != vco_test_reg_val_309)
        {
            btdrv_write_rf_reg(0x309, vco_test_reg_val_309);
        }
        if(0 != vco_test_reg_val_38)
        {
            btdrv_write_rf_reg(0x38, vco_test_reg_val_38);
        }
    }
}

void btdrv_stop_bt(void)
{
    btdrv_poweron(BT_POWEROFF);
}

void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length)
{
    uint8_t buff[256];
    if(length ==0 || length >128)
        return;
    buff[0] = 0x01;
    buff[1] = 0x02;
    buff[2] = 0xfc;
    buff[3] = length + 6;
    buff[4] = address & 0xff;
    buff[5] = (address &0xff00)>>8;
    buff[6] = (address &0xff0000)>>16;
    buff[7] = address>>24;
    buff[8] = wr_type;
    buff[9] = length;
    memcpy(&buff[10],value,length);
    btdrv_SendData(buff,length+10);
    btdrv_delay(2);


}

void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param)
{
    uint8_t buff[256];
    buff[0] = 0x01;
    buff[1] = opcode & 0xff;
    buff[2] = (opcode &0xff00)>>8;
    buff[3] = cmdlen;
    if(cmdlen>0)
        memcpy(&buff[4],param,cmdlen);
    btdrv_SendData(buff,cmdlen+4);
}

//[26:0] 0x07ffffff
//[27:0] 0x0fffffff

uint32_t btdrv_syn_get_curr_ticks(void)
{
    uint32_t value;

    value = btdrv_reg_op_syn_get_curr_ticks();
    return value;
}

uint32_t btdrv_syn_get_cis_curr_time(void)
{
    uint32_t value;

    value = btdrv_reg_op_syn_get_cis_curr_time();
    return value;
}

#ifdef __SW_TRIG__

uint16_t Tbit_M_ori_flag = 200;
#define RF_DELAY  18

#define ROUNDDOWN(x)   ((int)(x))
#define ROUNDUP(x)     ((int)(x) + ((x-(int)(x)) > 0 ? 1 : 0))
#define ROUND(x)       ((int)((x) + ((x) > 0 ? 0.5 : -0.5)))

uint16_t btdrv_Tbit_M_h_get(uint32_t Tclk_M, int16_t Tbit_M_h_ori)
{
    uint16_t Tbit_M_h;
    Tbit_M_h = Tbit_M_h_ori;
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tbit_M_h);
    return Tbit_M_h;
}


uint16_t btdrv_Tbit_M_get(uint32_t Tclk_M, uint16_t Tbit_M_h)
{
    uint16_t Tbit_M;
    if(Tbit_M_h % 2)///if Tbit_M_h is odd
    {
        Tbit_M = (uint16_t)ROUNDUP(Tbit_M_h/2) + 1;
    }
    else
    {
        Tbit_M = (uint16_t)ROUNDUP(Tbit_M_h/2);
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tbit_M);
    return Tbit_M;
}


extern "C" int32_t bt_syn_get_clkoffset(uint16_t conhdl)
{
    int32_t offset;

    if(conhdl>=0x80)
        offset = bt_drv_reg_op_get_clkoffset(conhdl-0x80);
    else
        offset = 0;

    return offset;
}

int32_t btdrv_slotoff_get(uint16_t conhdl,int32_t clkoff)
{
    int32_t slotoff;

    if(clkoff < 0)
    {
        slotoff = -(int32_t)ROUNDDOWN((-clkoff)/2);
    }
    else
    {
        if(clkoff % 2)
        {
            slotoff = (int32_t)ROUNDUP(clkoff/2) + 1;
        }
        else
        {
            slotoff = (int32_t)ROUNDUP(clkoff/2);
        }
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,slotoff);
    return slotoff;
}

int16_t btdrv_bitoff_get(uint16_t rxbit_1us,int32_t clkoff)
{
    int16_t bitoff;

    bitoff = rxbit_1us - 68 - RF_DELAY;
    BT_DRV_TRACE(1,"[%s] bitoff=%d\n",__func__,bitoff);
    if((bitoff < 0) && (clkoff % 2))
    {
        bitoff += 624;
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,bitoff);
    return bitoff;
}


uint8_t btdrv_clk_adj_M_get(uint16_t Tbit_M,int16_t bitoff)
{
    uint8_t clk_adj_M;

    if(bitoff >= Tbit_M)
    {
        clk_adj_M = 1;
    }
    else
    {
        clk_adj_M = 0;
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,clk_adj_M);
    return clk_adj_M;
}

uint8_t btdrv_clk_adj_S_get(uint8_t clk_adj_M,uint16_t Tbit_M,int16_t bitoff)
{
    uint8_t clk_adj_S;

    if(clk_adj_M > 0)
    {
        int16_t temp = (bitoff - Tbit_M)*2;
        if(temp > 624)
        {
            clk_adj_S = 1;
        }
        else
        {
            clk_adj_S = 0;
        }
    }
    else
    {
        clk_adj_S = 0;
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,clk_adj_S);
    return clk_adj_S;
}

uint32_t btdrv_Tclk_S_get(uint32_t Tclk_M, uint8_t clk_adj_M,
                                uint8_t clk_adj_S, int32_t slotoff)
{
    uint32_t Tclk_S;
    Tclk_S = Tclk_M - slotoff*2 + clk_adj_M + clk_adj_S;
    Tclk_S &= 0x0fffffff;
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tclk_S);
    return Tclk_S;
}


uint16_t btdrv_Tclk_S_h_get(uint8_t clk_adj_M,uint8_t clk_adj_S,
                                    uint16_t Tbit_M,int16_t bitoff)
{
    uint16_t Tclk_S_h;
    if(clk_adj_M == 0)
    {
        Tclk_S_h = (Tbit_M - bitoff)*2;
    }
    else
    {
        if(clk_adj_S == 1)
        {
            Tclk_S_h = (1248-(bitoff-Tbit_M)*2);
        }
        else
        {
            Tclk_S_h = (624-(bitoff-Tbit_M)*2);
        }
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tclk_S_h);
    return Tclk_S_h;
}

void btdrv_sw_trig_tg_finecnt_set(uint16_t tg_bitcnt, uint8_t trig_route)
{
    btdrv_reg_op_sw_trig_tg_finecnt_set(tg_bitcnt,trig_route);
}

uint16_t btdrv_sw_trig_tg_finecnt_get(uint8_t trig_route)
{
    uint16_t finecnt = 0;
    finecnt = btdrv_reg_op_sw_trig_tg_finecnt_get(trig_route);
    return finecnt;
}

void btdrv_sw_trig_tg_clkncnt_set(uint32_t num, uint8_t trig_route)
{
    btdrv_reg_op_sw_trig_tg_clkncnt_set(num,trig_route);
}

void btdrv_sw_trig_slave_calculate_and_set(uint16_t conhdl, uint32_t Tclk_M, uint16_t Tbit_M_h_ori, uint8_t trig_route)
{
    uint16_t Tbit_M_h;
    uint16_t Tbit_M;
    uint16_t rxbit_1us;
    int16_t bitoff;
    int32_t clkoff;
    int32_t slotoff;
    uint8_t clk_adj_M;
    uint8_t clk_adj_S;
    uint32_t Tclk_S;
    uint32_t Tclk_S_h;

    Tbit_M_h = Tbit_M_ori_flag;
    Tbit_M = btdrv_Tbit_M_get(Tclk_M,Tbit_M_h);
    rxbit_1us = bt_drv_reg_op_rxbit_1us_get(conhdl);
    clkoff = bt_syn_get_clkoffset(conhdl);
    slotoff = btdrv_slotoff_get(conhdl,clkoff);
    bitoff = btdrv_bitoff_get(rxbit_1us,clkoff);
    clk_adj_M = btdrv_clk_adj_M_get(Tbit_M,bitoff);
    clk_adj_S = btdrv_clk_adj_S_get(clk_adj_M,Tbit_M,bitoff);
    Tclk_S = btdrv_Tclk_S_get(Tclk_M,clk_adj_M,clk_adj_S,slotoff);
    Tclk_S_h = btdrv_Tclk_S_h_get(clk_adj_M,clk_adj_S,Tbit_M,bitoff);

    btdrv_sw_trig_tg_clkncnt_set(Tclk_S,trig_route);
    btdrv_sw_trig_tg_finecnt_set(Tclk_S_h,trig_route);
    btdrv_sw_trig_en_set(trig_route);
    btdrv_reg_op_sw_trig_route_extra_set(trig_route);
}

void btdrv_sw_trig_master_set(uint32_t Tclk_M, uint16_t Tbit_M_h_ori, uint8_t trig_route)
{
    uint16_t Tbit_M_h;

    btdrv_sw_trig_tg_clkncnt_set(Tclk_M,trig_route);
    Tbit_M_h = btdrv_Tbit_M_h_get(Tclk_M,Tbit_M_h_ori);
    btdrv_sw_trig_tg_finecnt_set(Tbit_M_h,trig_route);
    btdrv_sw_trig_en_set(trig_route);
    btdrv_reg_op_sw_trig_route_extra_set(trig_route);
}

#endif


void bt_syn_cancel_tg_ticks(uint8_t trig_route)
{
#ifdef __SW_TRIG__
    btdrv_sw_trig_disable_set(trig_route);
#endif
}

static void btdrv_syn_set_tg_ticks(uint32_t num, uint8_t mode, uint16_t conhdl, uint8_t trig_route)
{
    if (mode == BT_TRIG_MASTER_ROLE)
    {
#ifdef __SW_TRIG__
        BT_DRV_TRACE(1,"[%s] __SW_TRIG__ conhdl=0x%x num=%d trig_route=%d\n",__func__,conhdl,num,trig_route);
        btdrv_sw_trig_master_set(num, Tbit_M_ori_flag, trig_route);
#else
        btdrv_reg_op_syn_set_tg_ticks_master_role();
#endif
    }
    else
    {
#ifdef __SW_TRIG__
        //uint16_t conhdl = btdrv_sync_sw_trig_get_conhdl();
        BT_DRV_TRACE(1,"[%s] __SW_TRIG__ conhdl=0x%x num=%d trig_route=%d\n",__func__,conhdl,num,trig_route);
        btdrv_sw_trig_slave_calculate_and_set(conhdl, num, Tbit_M_ori_flag,trig_route);
#else
        btdrv_reg_op_syn_set_tg_ticks_slave_role(num);
#endif
    }
}

// Can be used by master or slave
// Ref: Master bt clk
uint32_t bt_syn_get_curr_ticks(uint16_t conhdl)
{
    int32_t curr,offset;

    curr = btdrv_syn_get_curr_ticks();

    if (btdrv_is_link_index_valid(btdrv_conhdl_to_linkid(conhdl)))
        offset = bt_drv_reg_op_get_clkoffset(btdrv_conhdl_to_linkid(conhdl));
    else
        offset = 0;
//    BT_DRV_TRACE(4,"[%s] curr(%d) + offset(%d) = %d", __func__, curr , offset,curr + offset);
    return (curr + offset) & 0x0fffffff;
}

void bt_syn_trig_checker(uint16_t conhdl)
{
}

// Can be used by master or slave
// Ref: Master bt clk
void bt_syn_set_tg_ticks(uint32_t val,uint16_t conhdl, uint8_t mode, uint8_t trig_route, bool no_link_trig)
{
    int32_t offset;
    uint8_t link_id = btdrv_conhdl_to_linkid(conhdl);
    if(no_link_trig == false)
    {
        if(HCI_LINK_INDEX_INVALID == link_id)
        {
            BT_DRV_TRACE(3,"%s,ERR INVALID CONHDL 0x%x!! ca=%p",__func__,conhdl, __builtin_return_address(0));
            return;
        }
    }
    if (btdrv_is_link_index_valid(link_id))
    {
        offset = bt_drv_reg_op_get_clkoffset(link_id);
    }
    else
    {
        offset = 0;
    }

#if !defined(__SW_TRIG__)
    btdrv_reg_op_syn_set_tg_ticks_linkid(link_id);
#endif

    if ((mode == BT_TRIG_MASTER_ROLE) && (offset !=0))
    {
        BT_DRV_TRACE(0,"ERROR OFFSET !!");
    }

#if !defined(__SW_TRIG__)
    val = val>>1;
    val = val<<1;
    val += 1;
#endif

    bt_syn_cancel_tg_ticks(trig_route);
    BT_DRV_TRACE(4,"bt_syn_set_tg_ticks val:%d num:%d mode:%d conhdl:%02x", val, val - offset, mode, conhdl);
    btdrv_syn_set_tg_ticks(val, mode, conhdl, trig_route);
    bt_syn_trig_checker(conhdl);
}

void bt_syn_ble_set_tg_ticks(uint32_t val,uint8_t trig_route)
{
#ifdef __SW_TRIG__
    uint32_t clkncnt = 0;
    uint16_t finecnt = 0;

    btdrv_syn_clr_trigger(trig_route);
    btdrv_reg_op_bts_to_bt_time(val, &clkncnt, &finecnt);

    btdrv_sw_trig_tg_clkncnt_set(clkncnt,trig_route);
    btdrv_sw_trig_tg_finecnt_set(finecnt,trig_route);
    btdrv_sw_trig_en_set(trig_route);
    btdrv_reg_op_sw_trig_route_extra_set(trig_route);
#endif
}

uint32_t bt_syn_ble_bt_time_to_bts(uint32_t hs, uint16_t hus)
{
    return btdrv_reg_op_bt_time_to_bts(hs, hus);
}

void btdrv_enable_playback_triggler(uint8_t triggle_mode)
{
    btdrv_reg_op_enable_playback_triggler(triggle_mode);
}


void btdrv_play_trig_mode_set(uint8_t mode)
{
    btdrv_reg_op_play_trig_mode_set(mode);
}


void btdrv_disable_playback_triggler(void)
{
    btdrv_reg_op_disable_playback_triggler();
}

//pealse use btdrv_is_link_index_valid() check link index whether valid
uint8_t btdrv_conhdl_to_linkid(uint16_t connect_hdl)
{
    //invalid hci handle,such as link disconnected
    if(connect_hdl < HCI_HANDLE_MIN || connect_hdl > HCI_HANDLE_MAX)
    {
        TRACE(2, "ERROR Connect Handle=0x%x ca=%p", connect_hdl, __builtin_return_address(0));
        return HCI_LINK_INDEX_INVALID;
    }
    else
    {
        return (connect_hdl - HCI_HANDLE_MIN);
    }
}

void btdrv_pcm_enable(void)
{
    btdrv_reg_op_pcm_enable();
}

void btdrv_pcm_disable(void)
{
    btdrv_reg_op_pcm_disable();
}

// Trace tport
static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_tport[] =
{
    {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
};

int btdrv_host_gpio_tport_open(void)
{
    uint32_t i;

    for (i=0; i<sizeof(pinmux_tport)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP); i++)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&pinmux_tport[i], 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_tport[i].pin, HAL_GPIO_DIR_OUT, 0);
    }
    return 0;
}

int btdrv_gpio_port_set(int port)
{
    hal_gpio_pin_set((enum HAL_GPIO_PIN_T)pinmux_tport[port].pin);
    return 0;
}

int btdrv_gpio_tport_clr(int port)
{
    hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)pinmux_tport[port].pin);
    return 0;
}

#if defined(TX_RX_PCM_MASK)
uint8_t  btdrv_is_pcm_mask_enable(void)
{
    return 1;
}
#endif

#ifdef PCM_FAST_MODE
void btdrv_open_pcm_fast_mode_enable(void)
{
    btdrv_reg_op_open_pcm_fast_mode_enable();
}
void btdrv_open_pcm_fast_mode_disable(void)
{
    btdrv_reg_op_open_pcm_fast_mode_disable();
}
#endif

#if defined(CVSD_BYPASS)
void btdrv_cvsd_bypass_enable(void)
{
    btdrv_reg_op_cvsd_bypass_enable();
}
#endif

void btdrv_enable_rf_sw(int rx_on, int tx_on)
{
    hal_iomux_set_bt_rf_sw(rx_on, tx_on);
}

const uint16_t logen_cap_rf_store[] =
{
    0xA1,
    0xD0,
    0xD1,
    0x161,
    0x160,
    0x2F,
    0xD9,
    0xCF,
    0x2A,
    0xD5,
    0x152,
    0x2A,
    0x309,
    0x15,
    0x17,
    0x19,
    0x163,
    0xCF,
    //0xE6,
    //0xE7,
};

void btdrv_logen_cap_cal(void)
{
    uint32_t cnt_max, cnt_min = 0;
    uint16_t vco_cali_cnt0_high, vco_cali_cnt0_low = 0;
    const double Lvco0_Cpex_vco0 = 3.36807E-11;
    double Lvco1_Cpex_vco1 = 0.0;
    double beta1 = 0.0;
    int delta_beta = 0;
    double temp1, temp2;
    const int logen_cap_base = 16;
    int logen_cap_v = 0;
    int sign = 0;
    uint16_t txlogen_mixer_buf_cap = 0;

    uint32_t time_start = hal_sys_timer_get();

    uint32_t tbl_size = ARRAY_SIZE(logen_cap_rf_store);
    uint16_t local_v[tbl_size];
    for (uint32_t i=0; i<tbl_size;i++) {
        btdrv_read_rf_reg(logen_cap_rf_store[i], &local_v[i]);
        //BTRF_REG_DUMP(logen_cap_rf_store[i]);
    }

    //[cal_ol_prepare]
    btdrv_write_rf_reg(0xA1, 0x94CC);
    btdrv_write_rf_reg(0xD0, 0x4800);   //2 data_vco to be calibrated
    btdrv_write_rf_reg(0xD1, 0x3800);   //[13:12] switch sdm_clk to clk_polar
    btdrv_write_rf_reg(0x161, 0x8571);  //init freqword & data_vco
    btdrv_write_rf_reg(0x160, 0xC71C);
    btdrv_write_rf_reg(0x2F, 0x4000);
    btdrv_write_rf_reg(0xD9, 0x4004);
    btdrv_write_rf_reg(0xCF, 0x1040);   //cal_en=0
    btdrv_write_rf_reg(0x2A, 0x2000);   //pll off
    btdrv_write_rf_reg(0xD5, 0x0400);   //set cnt time
    btdrv_write_rf_reg(0x152, 0x8B82);  //modclk en
    osDelay(2);
    btdrv_write_rf_reg(0x2A, 0x2400);   //txpll manual on
    btdrv_write_rf_reg(0x309, 0x0);
    osDelay(2);
    btdrv_write_rf_reg(0x15, 0x0C00);   //switch sdm_clk to fref
    btdrv_write_rf_reg(0x17, 0x8000);   //open loop
    btdrv_write_rf_reg(0x19, 0x0907);
    //[change capbank1]
    btdrv_write_rf_reg(0x163, 0x3FFA);
    //[cal_en]  bit0:reg_vco_gain_cal_en
    btdrv_write_rf_reg(0xCF, 0x1040);
    btdrv_write_rf_reg(0xCF, 0x1041);
    osDelay(1);

    btdrv_read_rf_reg(0xE6, &vco_cali_cnt0_low);
    btdrv_read_rf_reg(0xE7, &vco_cali_cnt0_high);

    cnt_max = vco_cali_cnt0_low | ((vco_cali_cnt0_high & 0x3) << 16);
    TRACE(4, "[capbank1] RF_E6=0x%x | RF_E7=0x%x | cnt_max=0x%x | cnt_max=%d.", vco_cali_cnt0_low, vco_cali_cnt0_high, cnt_max, cnt_max);

    //[change capbank2]
    btdrv_write_rf_reg(0x163, 0x0A);
    //[cal_en]  bit0:reg_vco_gain_cal_en
    btdrv_write_rf_reg(0xCF, 0x1040);
    btdrv_write_rf_reg(0xCF, 0x1041);
    osDelay(1);

    btdrv_read_rf_reg(0xE6, &vco_cali_cnt0_low);
    btdrv_read_rf_reg(0xE7, &vco_cali_cnt0_high);

    cnt_min = vco_cali_cnt0_low | ((vco_cali_cnt0_high & 0x3) << 16);
    TRACE(4, "[capbank2] RF_E6=0x%x | RF_E7=0x%x | cnt_min=0x%x | cnt_min=%d.", vco_cali_cnt0_low, vco_cali_cnt0_high, cnt_min, cnt_min);

    //Lvco1_Cpex_vco1 = (double)((double)1212/(cnt_max*cnt_max) - (double)212/(cnt_min*cnt_min))/1000;
    temp1 = (double)((double)1.212/(double)cnt_max);
    temp1 = temp1 / (double)cnt_max;
    //TRACE(1, "temp1=%e ",temp1);
    temp2 = (double)((double)0.212/(double)cnt_min);
    temp2 = temp2 / (double)cnt_min;
    //TRACE(1, "temp2=%e ",temp2);
    Lvco1_Cpex_vco1 = temp1-temp2;
    //TRACE(1, "Lvco1_Cpex_vco1=%e",Lvco1_Cpex_vco1);

    beta1 = Lvco1_Cpex_vco1 / Lvco0_Cpex_vco0;
    //TRACE(1, "beta1=%lf",beta1);

    delta_beta = ABS((int)(((double)1.0-beta1) / (double)0.01));
    TRACE(1, "delta_beta=%d", delta_beta);

    if (beta1 < 1) {
        sign = 1;
    }

    if (1 == sign) {
        logen_cap_v = logen_cap_base + 6*delta_beta;
        if (delta_beta >= 7) {
            logen_cap_v = 54;
        }
    } else if(0 == sign) {
        logen_cap_v = logen_cap_base - 6*delta_beta;
        if (delta_beta >= 3) {
            logen_cap_v = 0;
        }
    }

    TRACE(2, "%s Final logen_cap_v=%d", __func__, logen_cap_v);

    for (int i=0; i<16; i++) {
        if (i<4) {
            txlogen_mixer_buf_cap = logen_cap_v+4;
        } else if ((i >=4) && (i<8)) {
            txlogen_mixer_buf_cap = logen_cap_v+3;
        } else if ((i >=8) && (i<12)) {
            txlogen_mixer_buf_cap = logen_cap_v+2;
        } else {
            txlogen_mixer_buf_cap = logen_cap_v;
        }
        //TRACE(2, "i=%d, txlogen_mixer_buf_cap=%d",i,txlogen_mixer_buf_cap);
        BTRF_REG_SET_FIELD((0x1D6+i), 0x7F, 7, txlogen_mixer_buf_cap);
    }

    for (uint32_t i=0; i<tbl_size;i++) {
        btdrv_write_rf_reg(logen_cap_rf_store[i], local_v[i]);
        //BTRF_REG_DUMP(logen_cap_rf_store[i]);
    }

    BTRF_REG_SET_FIELD(0x9F, 1, 15, 0);     //reg_txlogen_mixer_buf_cap_dr must be 0, btdrv_logen_cap_cal can take effect

    TRACE(2,"%s use time: %d ms", __func__,__TICKS_TO_MS(hal_sys_timer_get()-time_start));
}
void btdrv_restart_bt(void)
{
    btdrv_hcioff();
    btdrv_poweron(BT_POWEROFF);
    osDelay(20);
    btdrv_poweron(BT_POWERON);
}

#ifndef NOSTD
void btdrv_cal_data_write(void)
{
    // btdrv_tx_dac_datawrite();
#ifdef TX_IQ_CAL
    btdrv_dc_datawrite();
    btdrv_iq_datawrite();
#endif
#ifdef DPD_ONCHIP_CAL
    if (dpd_mem_data_exist_flag) {
        btdrv_dpd_datawrite();
    }
#endif
}
#endif
