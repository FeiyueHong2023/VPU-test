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
#ifndef __BT_DRV_1603_CONFIG_H__
#define __BT_DRV_1603_CONFIG_H__
/***************************************************************************
 *BTC sleep and wakeup configration
 *align BTC wakeup timing with MCU config (HAL_CMU_OSC_STABLE_TIME)
 ****************************************************************************/
#include CHIP_SPECIFIC_HDR(hal_cmu_pri)

/***************************************************************************
 *some vendor software version record
 ****************************************************************************/
#define IBRT_MULTI_SCH_HCI_SUPPORT_VERSION (0x230922)

/*
*   TWOSC: Time to wake-up osc_en before deepsleep_time expiration
*   TWRM  : Time to wake-up radio module(no used)
*   TWEXT : Time to wake-up osc_en on external wake-up request
*/

//BTC power off section define
#define BTC_LP_OFF       (0x0000)
#define LOGIC_POWEROFF_EN      (0x0004)
#define EM0_POWEROFF_EN       (0x0008)
#define EM1_POWEROFF_EN       (0x0010)
#define RAM0_POWEROFF_EN      (0x0020)
#define RAM1_POWEROFF_EN      (0x0040)
#define RAM2_POWEROFF_EN      (0x0080)
#define RAM3_POWEROFF_EN      (0x0100)
#define RAM4_POWEROFF_EN      (0x0200)
#define RAM5_POWEROFF_EN      (0x0400)
#define EM2_POWEROFF_EN       (0x0800)
#define EM3_POWEROFF_EN       (0x1000)

#define IP_PROG_DELAY_DFT (3)

#ifdef BT_LOG_POWEROFF
#define BTC_LP_MODE         (LOGIC_POWEROFF_EN)
#else
#define BTC_LP_MODE         (BTC_LP_OFF)
#endif

#define BT_CMU_26M_READY_TIMEOUT_US    (HAL_CMU_OSC_STABLE_TIME)
#define BT_CMU_OSC_READY_TIMEOUT_US    (HAL_CMU_OSC_STABLE_TIME + 100)
#define BT_CMU_WEXT_READY_TIMEOUT_US   (HAL_CMU_OSC_STABLE_TIME)

/***************************************************************************
 *HW AGC configration
 ****************************************************************************/
#ifdef __HW_AGC__
#define BTC_HW_AGC_ENABLE_FLAG 0x01
#else
#define BTC_HW_AGC_ENABLE_FLAG 0x00
#endif

#define GAIN_TBL_SIZE           0x0F
/***************************************************************************
 *BT phy configration
 ****************************************************************************/

#define PHY_TX_DELAY (0x20)
#define PHY_RX_DELAY (0x1E)

/***************************************************************************
 *XTAL cap default val
 ****************************************************************************/
#define DEFAULT_XTAL_FCAP                       (0xC8)

/***************************************************************************
 *TX power configration
 ****************************************************************************/

#define BT_MAX_TX_PWR_IDX    (5)
#define BT_INIT_TX_PWR_IDX    (BT_MAX_TX_PWR_IDX - 2)

#ifdef __HW_AGC__
#define BTC_HW_AGC_ENABLE_FLAG 0x01
#else
#define BTC_HW_AGC_ENABLE_FLAG 0x00
#endif

#ifdef __HW_AGC__
#define HW_AGC_HIGH_PWEF_MODE   (0x0)
#define HW_AGC_PWR_SAVE_MODE     (0x1)
#endif

//RF BT TX register
#define RF_BT_TX_PWR_IDX0_TX_FLT_REG_NORMAL (0x5D)
#define RF_BT_TX_PWR_IDX1_TX_FLT_REG_NORMAL (0x5E)
#define RF_BT_TX_PWR_IDX2_TX_FLT_REG_NORMAL (0x5F)
#define RF_BT_TX_PWR_IDX3_TX_FLT_REG_NORMAL (0x60)
#define RF_BT_TX_PWR_IDX4_TX_FLT_REG_NORMAL (0x61)
#define RF_BT_TX_PWR_IDX5_TX_FLT_REG_NORMAL (0x62)
#define RF_BT_TX_PWR_IDX6_TX_FLT_REG_NORMAL (0x63)
#define RF_BT_TX_PWR_IDX7_TX_FLT_REG_NORMAL (0x64)

#define RF_BT_TX_PWR_IDX0_RFVCO_REG_NORMAL  (0x369)
#define RF_BT_TX_PWR_IDX1_RFVCO_REG_NORMAL  (0x36A)
#define RF_BT_TX_PWR_IDX2_RFVCO_REG_NORMAL  (0x36B)
#define RF_BT_TX_PWR_IDX3_RFVCO_REG_NORMAL  (0x36C)
#define RF_BT_TX_PWR_IDX4_RFVCO_REG_NORMAL  (0x36D)
#define RF_BT_TX_PWR_IDX5_RFVCO_REG_NORMAL  (0x36E)
#define RF_BT_TX_PWR_IDX6_RFVCO_REG_NORMAL  (0x36F)
#define RF_BT_TX_PWR_IDX7_RFVCO_REG_NORMAL  (0x370)

//RF BLE TX register
#define RF_BLE_TX_PWR_IDX0_TX_FLT_REG_NORMAL (0x13c)
#define RF_BLE_TX_PWR_IDX1_TX_FLT_REG_NORMAL (0x13d)
#define RF_BLE_TX_PWR_IDX2_TX_FLT_REG_NORMAL (0x13e)
#define RF_BLE_TX_PWR_IDX3_TX_FLT_REG_NORMAL (0x13f)
#define RF_BLE_TX_PWR_IDX4_TX_FLT_REG_NORMAL (0x140)
#define RF_BLE_TX_PWR_IDX5_TX_FLT_REG_NORMAL (0x141)
#define RF_BLE_TX_PWR_IDX6_TX_FLT_REG_NORMAL (0x142)
#define RF_BLE_TX_PWR_IDX7_TX_FLT_REG_NORMAL (0x143)

#define RF_BLE_TX_PWR_IDX0_RFVCO_REG_NORMAL  (0x371)
#define RF_BLE_TX_PWR_IDX1_RFVCO_REG_NORMAL  (0x372)
#define RF_BLE_TX_PWR_IDX2_RFVCO_REG_NORMAL  (0x373)
#define RF_BLE_TX_PWR_IDX3_RFVCO_REG_NORMAL  (0x374)
#define RF_BLE_TX_PWR_IDX4_RFVCO_REG_NORMAL  (0x375)
#define RF_BLE_TX_PWR_IDX5_RFVCO_REG_NORMAL  (0x376)
#define RF_BLE_TX_PWR_IDX6_RFVCO_REG_NORMAL  (0x377)
#define RF_BLE_TX_PWR_IDX7_RFVCO_REG_NORMAL  (0x378)

//RF DIGITAL TX POWER register
// txpwr digital gain
// dig_gain=(nom/(2^den))
#define RF_DIGTAL_TX_GFSK_DEN                (0x5)
#define RF_DIGTAL_TX_GFSK_NOM                (0x1E)
#define RF_DIGTAL_TX_PSK_DEN                 (0x5)
#define RF_DIGTAL_TX_PSK_NOM                 (0x1E)
#define RF_DIGTAL_TX_2M3M_GFSK_ENABLE        (0x1)
#define RF_DIGTAL_TX_2M3M_GFSK_DISABLE       (0x0)
#define RF_DIGTAL_TX_2M3M_GFSK_DEN           (0x5)
//TX POWER CONFIG FOR GFSK - DPSK
//for example
//if use RF_DIGTAL_TX_2M3M_GFSK_NOM_3P5DBM
//GFSK = 16dbm   DPSK = 12.5dbm      GFSK - DPSK = 3.5dbm
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_3P5DBM    (0x20)
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_3DBM      (0x1E)
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_2P5DBM    (0x1C)
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_2DBM      (0x1A)
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_1P5DBM    (0x19)
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_1DBM      (0x18)
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_0P5DBM    (0x16)
#define RF_DIGTAL_TX_2M3M_GFSK_NOM_0DBM      (0x15)

/***************************************************************************
 *RX gain configration
 ****************************************************************************/

/***************************************************************************
 *Fast ack configration
 ****************************************************************************/
#ifdef mHDT_SUPPORT
#define FA_RX_WIN_SIZE      (0x4)
#else
#define FA_RX_WIN_SIZE      (0xA)
#endif
#define FA_RXPWRUP_TIMING   (0x37)
#define FA_TXPWRUP_TIMING   (0x28)
#define FA_BW2M_RXPWRUP_TIMING   (0x2F)
#define FA_BW2M_TXPWRUP_TIMING   (0x28)

#ifdef mHDT_SUPPORT
#define FA_CNT_PKT_US   (0x74)
#else
#define FA_CNT_PKT_US   (0x24)
#endif

#define FA_FIX_TX_GIAN_IDX  (0x3)
#define FA_FIX_RX_GIAN_IDX  (0x1)
#define FA_NEW_CORR_VALUE   (0x40)
//lager than 2 will causes error synchronization
#define FA_OLD_CORR_VALUE   (0x1)
#define FA_MULTI_TX_COUNT   (0x3)
#define INVALID_FA_FREQ_TABLE       (0)
/***************************************************************************
 *BTC activity number configration
 ****************************************************************************/
#define BLE_ACTIVITY_MAX   20
#define MAX_NB_ACTIVE_ACL                   (5)
#define HCI_HANDLE_MIN         (0x80)
#define HCI_HANDLE_MAX         (HCI_HANDLE_MIN + MAX_NB_ACTIVE_ACL - 1)

/***************************************************************************
 *BEStrx RF timing configration
 ****************************************************************************/
#define BESTRX_OFFSET   (0x32)
#define BESTRX_TXPWRUP_CNT                   (0x20)
#define BESTRX_RXPWRUP_CNT         (0x32)
#define BESTRX_MASTER_RX_WINSIZE         (0XF)
#define BESTRX_TIFS (150)

/***************************************************************************
 *HDT trx RF timing configration
 ****************************************************************************/
#define HDT_RXPWRUP_CNT (0x41)
#define HDT_TXPWRUP_CNT (0x2A)

#endif //__BT_DRV_1603_CONFIG_H__