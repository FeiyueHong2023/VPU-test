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

#include "tota_stream_data_transfer.h"
#include "app_tota_cmd_code.h"
#include "app_tota_custom.h"
#include "bluetooth_bt_api.h"
#include "app_bt_stream.h"
#include "app_audio.h"
#include "nvrecord_bt.h"
#include "nvrecord_env.h"
#include "app_tota.h"
#include "app_tota_cmd_handler.h"
#include "apps.h"
#include "cmsis_os.h"
#include "app_ble_cmd_handler.h"
#include "app_ble_custom_cmd.h"
#include "app_datapath_server.h"
#include "app_datapath_client.h"
#include "app_tota_custom.h"
#include "app_tota_common.h"
#include "ota_control.h"
#include "app_flash_api.h"
#ifdef APP_CHIP_BRIDGE_MODULE
#include "app_chip_bridge.h"
#endif
#include "bes_gap_api.h"
#include "ota_control.h"
#include "app_tota_multi_chip_updata.h"

#ifdef TOTA_CROSS_CHIP_OTA
#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
static APP_CROSS_CHIP_OTA_ROLE_E tota_cross_chip_ota_role = CROSS_CHIP_OTA_ROLE_IS_DONGLE;
#else
static APP_CROSS_CHIP_OTA_ROLE_E tota_cross_chip_ota_role = CROSS_CHIP_OTA_ROLE_IS_MASTER;
#endif
extern uint32_t __ota_upgrade_log_start[];
FLASH_OTA_UPGRADE_LOG_FLASH_T *otaUpgradeLogInFlash_p = (FLASH_OTA_UPGRADE_LOG_FLASH_T *)__ota_upgrade_log_start;
#define otaUpgradeLog   (*otaUpgradeLogInFlash_p)

static void dongle_tota_receive_data_from_headset(uint32_t funcCode, uint8_t *p_buff, uint32_t bufLength);
static void headset_tota_receive_data(uint32_t funcCode, uint8_t *p_buff, uint32_t bufLength);

BLE_CUSTOM_COMMAND_TO_ADD(OP_DONGLE_TOTA_RECEIVE_DATA,
                        dongle_tota_receive_data_from_headset,
                        false,
                        0,
                        NULL);
BLE_CUSTOM_COMMAND_TO_ADD(OP_HEADSET_TOTA_RECEIVE_DATA,
                        headset_tota_receive_data,
                        false,
                        0,
                        NULL);

static void send_tota_data_to_headset_by_datapath(uint8_t* ptrParam, uint32_t paramLen)
{
    int8_t tmp_buf[paramLen];
    memset((void *)tmp_buf, 0, paramLen);
    memcpy(&tmp_buf[0], ptrParam, paramLen);
    BLE_CUSTOM_CMD_RET_STATUS_E ret;
    ret = BLE_send_custom_command(OP_HEADSET_TOTA_RECEIVE_DATA,
                                    (uint8_t*)tmp_buf,
                                    paramLen,
                                    TRANSMISSION_VIA_WRITE_CMD);
    TRACE(0,"%s ret:%d",__func__,ret);
}

static void send_tota_data_to_dongle_by_datapath(uint8_t* ptrParam, uint32_t paramLen)
{
    BLE_CUSTOM_CMD_RET_STATUS_E ret;
    ret = BLE_send_custom_command(OP_DONGLE_TOTA_RECEIVE_DATA,
                                    (uint8_t*)ptrParam,
                                    paramLen,
                                    TRANSMISSION_VIA_NOTIFICATION);
    TRACE(0,"%s ret:%d",__func__,ret);
    TRACE(0,"data:");
    DUMP8("0x%02x ", ptrParam, paramLen);
}

static void headset_tota_receive_data(uint32_t funcCode, uint8_t *p_buff, uint32_t bufLength)
{
    tota_set_connect_path(APP_TOTA_VIA_SPP);
    tota_set_connect_status(TOTA_CONNECTED);
    ota_control_register_transmitter(send_tota_data_to_dongle_by_datapath);
    _custom_cmd_handle(OP_TOTA_OTA, p_buff, bufLength);
}

static void dongle_tota_receive_data_from_headset(uint32_t funcCode,
                                                            uint8_t *p_buff, uint32_t bufLength)
{
    TRACE(0,"%s bufLength %d  ",__func__,bufLength);
    DUMP8("0x%02x ", p_buff, bufLength);
    TOTA_OTA_IMAGE_SIZE_T temp_tota_ota_image_size;
    uint32_t tempAccumulateCurrentReciveNumber = 0;
    if (*p_buff == CROSS_CHIP_OTA_ROLE_IS_MASTER)
    {
        temp_tota_ota_image_size.dongle_image_size = app_tota_get_dongle_image_size_via_flash();

        uint32_t headset_master_offset;
        memcpy(&headset_master_offset, p_buff + sizeof(CROSS_CHIP_OTA_ROLE_IS_MASTER),
                                                        sizeof(headset_master_offset));
        tempAccumulateCurrentReciveNumber = temp_tota_ota_image_size.dongle_image_size +
                                        headset_master_offset;
        app_tota_store_written_sum_image_size_to_flash(tempAccumulateCurrentReciveNumber);
        updata_tota_ota_current_recive_number(tempAccumulateCurrentReciveNumber);
        TRACE(0,"headset_master_offset %d sumbinSize %d dongle_size %d",headset_master_offset,
            tempAccumulateCurrentReciveNumber,temp_tota_ota_image_size.dongle_image_size);
    }
    else if (*p_buff == CROSS_CHIP_OTA_ROLE_IS_SLAVE)
    {
        temp_tota_ota_image_size.dongle_image_size = app_tota_get_dongle_image_size_via_flash();
        temp_tota_ota_image_size.headset_master_image_size = app_tota_get_headset_master_image_size_via_flash();

        uint32_t headset_slave_offset;
        memcpy(&headset_slave_offset, p_buff + sizeof(CROSS_CHIP_OTA_ROLE_IS_SLAVE),
                                                        sizeof(headset_slave_offset));
        tempAccumulateCurrentReciveNumber = temp_tota_ota_image_size.dongle_image_size +
                                        temp_tota_ota_image_size.headset_master_image_size +
                                        headset_slave_offset;
        app_tota_store_written_sum_image_size_to_flash(tempAccumulateCurrentReciveNumber);
        updata_tota_ota_current_recive_number(tempAccumulateCurrentReciveNumber);
        TRACE(0," headset_slave_offset %d sumbinSize %d dongle_size %d headset_m_size %d",headset_slave_offset,
            tempAccumulateCurrentReciveNumber,temp_tota_ota_image_size.dongle_image_size,
            temp_tota_ota_image_size.headset_master_image_size);
    }
    else if (*p_buff == OTA_RSP_RESUME_VERIFY)
    {
        TRACE(0,"OTA_TLV_PACKET_HEADER_LEN %d ",OTA_TLV_PACKET_HEADER_LEN);
        tempAccumulateCurrentReciveNumber = get_tota_ota_current_recive_number();
        memcpy((p_buff + OTA_TLV_PACKET_HEADER_LEN),
                (uint8_t *)&tempAccumulateCurrentReciveNumber, sizeof(uint32_t));
    }
    ota_spp_tota_send_data(p_buff, bufLength);
}

void app_tota_save_image_size_to_flash(uint8_t* ptrParam)
{
    TOTA_OTA_IMAGE_SIZE_T tota_ota_image_size;
    OTA_LEGACY_PACKET_T *packet = NULL;
    uint8_t* dataBufferForBurning = ota_voicepath_get_common_ota_databuf();
    APP_CROSS_CHIP_OTA_ROLE_E temp_role = (APP_CROSS_CHIP_OTA_ROLE_E)ptrParam[0];
    NORFLASH_API_MODULE_ID_T mod = NORFLASH_API_MODULE_ID_UPGRADE_LOG;
    switch (temp_role) {
        case CROSS_CHIP_OTA_ROLE_IS_DONGLE:
            memcpy((uint8_t *)&(tota_ota_image_size.dongle_image_size),
                ptrParam+sizeof(CROSS_CHIP_OTA_ROLE_IS_DONGLE)+OTA_PACKET_LENGTH_HEADER_LEN+sizeof(packet->payload.cmdOtaStart.magicCode),
                sizeof(tota_ota_image_size.dongle_image_size));
            app_flash_read(mod,
                            0,
                            dataBufferForBurning,
                            OTAUPLOG_HEADSIZE);
            memcpy(dataBufferForBurning+OTAUPLOG_HEADSIZE,
                (uint8_t *)&(tota_ota_image_size.dongle_image_size),
                sizeof(tota_ota_image_size.dongle_image_size));
            app_flash_sector_erase(mod, 0);
            app_flash_program(mod,
                            0,
                            dataBufferForBurning,
                            OTAUPLOG_HEADSIZE+sizeof(tota_ota_image_size.dongle_image_size),
                            false);

            TOTA_OTA_IMAGE_SIZE_T temp_dongle_image_size1;
            app_flash_read(mod,
                    OTAUPLOG_HEADSIZE,
                    (uint8_t *)&temp_dongle_image_size1.dongle_image_size,
                    sizeof(tota_ota_image_size.dongle_image_size));
            break;
        case CROSS_CHIP_OTA_ROLE_IS_MASTER:
            memcpy((uint8_t *)&(tota_ota_image_size.headset_master_image_size),
                ptrParam+sizeof(CROSS_CHIP_OTA_ROLE_IS_MASTER)+OTA_PACKET_LENGTH_HEADER_LEN+sizeof(packet->payload.cmdOtaStart.magicCode),
                sizeof(tota_ota_image_size.headset_master_image_size));
            app_flash_read(mod,
                            0,
                            dataBufferForBurning,
                            OTAUPLOG_HEADSIZE+sizeof(tota_ota_image_size.dongle_image_size));
            memcpy(dataBufferForBurning+OTAUPLOG_HEADSIZE+
                                                       sizeof(tota_ota_image_size.dongle_image_size),
                  (uint8_t *)&(tota_ota_image_size.headset_master_image_size),
                  sizeof(tota_ota_image_size.headset_master_image_size));
            app_flash_sector_erase(mod,
                                   0);
            app_flash_program(mod,
                            0,
                            dataBufferForBurning,
                            OTAUPLOG_HEADSIZE+sizeof(tota_ota_image_size.dongle_image_size)+
                                              sizeof(tota_ota_image_size.headset_master_image_size),
                            false);
            break;
        case CROSS_CHIP_OTA_ROLE_IS_SLAVE:
            memcpy((uint8_t *)&(tota_ota_image_size.headset_slave_image_size),
                ptrParam+sizeof(CROSS_CHIP_OTA_ROLE_IS_SLAVE)+OTA_PACKET_LENGTH_HEADER_LEN+sizeof(packet->payload.cmdOtaStart.magicCode),
                sizeof(tota_ota_image_size.headset_slave_image_size));
            app_flash_read(mod,
                            0,
                            dataBufferForBurning,
                            OTAUPLOG_HEADSIZE+sizeof(tota_ota_image_size.dongle_image_size)+
                                              sizeof(tota_ota_image_size.headset_master_image_size));
            memcpy(dataBufferForBurning+OTAUPLOG_HEADSIZE+
                                                       sizeof(tota_ota_image_size.dongle_image_size)+
                                                       sizeof(tota_ota_image_size.headset_master_image_size),
                  (uint8_t *)&(tota_ota_image_size.headset_slave_image_size),
                  sizeof(tota_ota_image_size.headset_slave_image_size));
            app_flash_sector_erase(mod,
                                   0);
            app_flash_program(mod,
                            0,
                            dataBufferForBurning,
                            OTAUPLOG_HEADSIZE+sizeof(tota_ota_image_size.dongle_image_size)+
                                              sizeof(tota_ota_image_size.headset_master_image_size)+
                                              sizeof(tota_ota_image_size.headset_slave_image_size),
                            false);
            break;
        default:
            break;
    }
}

uint32_t app_tota_get_dongle_image_size_via_flash(void)
{
    TRACE(0,"%s  ",__func__);
    TOTA_OTA_IMAGE_SIZE_T temp_tota_ota_image_size;
    NORFLASH_API_MODULE_ID_T mod = NORFLASH_API_MODULE_ID_UPGRADE_LOG;
    app_flash_read(mod,
                OTAUPLOG_HEADSIZE,
                (uint8_t*)&(temp_tota_ota_image_size.dongle_image_size),
                sizeof(temp_tota_ota_image_size.dongle_image_size));
    return temp_tota_ota_image_size.dongle_image_size;
}

uint32_t app_tota_get_headset_master_image_size_via_flash(void)
{
    TRACE(0,"%s  ",__func__);
    TOTA_OTA_IMAGE_SIZE_T temp_tota_ota_image_size;
    NORFLASH_API_MODULE_ID_T mod = NORFLASH_API_MODULE_ID_UPGRADE_LOG;
    app_flash_read(mod,
                OTAUPLOG_HEADSIZE+sizeof(temp_tota_ota_image_size.dongle_image_size),
                (uint8_t*)&(temp_tota_ota_image_size.headset_master_image_size),
                sizeof(temp_tota_ota_image_size.headset_master_image_size));
    return temp_tota_ota_image_size.headset_master_image_size;
}

uint32_t app_tota_get_written_sum_image_size_via_flash(void)
{
    TRACE(0,"%s  ",__func__);
    TOTA_OTA_IMAGE_SIZE_T temp_tota_ota_image_size;
    NORFLASH_API_MODULE_ID_T mod = NORFLASH_API_MODULE_ID_UPGRADE_LOG;
    app_flash_read(mod,
                OTAUPLOG_HEADSIZE+sizeof(temp_tota_ota_image_size.dongle_image_size)+
                                  sizeof(temp_tota_ota_image_size.headset_master_image_size)+
                                  sizeof(temp_tota_ota_image_size.headset_slave_image_size),
                (uint8_t*)&(temp_tota_ota_image_size.written_sum_image_size),
                sizeof(temp_tota_ota_image_size.written_sum_image_size));
    updata_tota_ota_current_recive_number(temp_tota_ota_image_size.written_sum_image_size);
    return temp_tota_ota_image_size.written_sum_image_size;
}

void app_tota_store_written_sum_image_size_to_flash(uint32_t written_sum_zise)
{
    TOTA_OTA_IMAGE_SIZE_T tota_ota_image_size;
    uint8_t* dataBufferForBurning = ota_voicepath_get_common_ota_databuf();
    memcpy((uint8_t *)&(tota_ota_image_size.written_sum_image_size), (uint8_t *)&written_sum_zise,
            sizeof(tota_ota_image_size.written_sum_image_size));
    NORFLASH_API_MODULE_ID_T mod = NORFLASH_API_MODULE_ID_UPGRADE_LOG;
    app_flash_read(mod,
                    0,
                    dataBufferForBurning,
                    OTAUPLOG_HEADSIZE+sizeof(tota_ota_image_size.dongle_image_size)+
                                        sizeof(tota_ota_image_size.headset_master_image_size)+
                                        sizeof(tota_ota_image_size.headset_slave_image_size));
    memcpy(dataBufferForBurning+OTAUPLOG_HEADSIZE+
                                sizeof(tota_ota_image_size.dongle_image_size)+
                                sizeof(tota_ota_image_size.headset_master_image_size)+
                                sizeof(tota_ota_image_size.headset_slave_image_size),
        (uint8_t *)&(tota_ota_image_size.written_sum_image_size),
        sizeof(tota_ota_image_size.written_sum_image_size));

    app_flash_sector_erase(mod,
                            0);
    app_flash_program(mod,
                        0,
                        dataBufferForBurning,
                        OTAUPLOG_HEADSIZE+sizeof(tota_ota_image_size.dongle_image_size)+
                                            sizeof(tota_ota_image_size.headset_master_image_size)+
                                            sizeof(tota_ota_image_size.headset_slave_image_size)+
                                            sizeof(tota_ota_image_size.written_sum_image_size),
                        false);
}

void app_handle_multi_chip_tota_data(uint8_t* ptrParam, uint32_t paramLen)
{
    uint32_t tempAccumulateCurrentReciveNumber = get_tota_ota_current_recive_number();
    do {
        if (ptrParam[0] == OTA_COMMAND_IMAGE_APPLY)
        {
            uint8_t _ptrParam[12]={0};;
            memcpy(_ptrParam+sizeof(uint8_t), ptrParam, paramLen);
            uint32_t _paramLen = paramLen+sizeof(uint8_t);
            _ptrParam[0] = CROSS_CHIP_OTA_ROLE_IS_MASTER;
            send_tota_data_to_headset_by_datapath(_ptrParam, _paramLen);
            osDelay(500);
            _ptrParam[0] = CROSS_CHIP_OTA_ROLE_IS_SLAVE;
            send_tota_data_to_headset_by_datapath(_ptrParam, _paramLen);
        }

        if (ptrParam[0] == OTA_COMMAND_RESUME_VERIFY)
        {
            uint8_t zero_temp_buf[32]={0};
            if (memcmp(otaUpgradeLog.randomCode,ptrParam+OTA_PACKET_LENGTH_HEADER_LEN+sizeof(uint32_t),sizeof(zero_temp_buf)) &&
                !memcmp(zero_temp_buf, ptrParam+OTA_PACKET_LENGTH_HEADER_LEN+sizeof(uint32_t), sizeof(zero_temp_buf)))
            {
                TRACE(2, "[%s]  ...", __func__);
                tempAccumulateCurrentReciveNumber = 0;
                updata_tota_ota_current_recive_number(tempAccumulateCurrentReciveNumber);
            }
            else
            {
                OTA_LEGACY_PACKET_T packet;
                uint32_t dongle_image_size = app_tota_get_dongle_image_size_via_flash();
                uint32_t headset_master_image_size = app_tota_get_headset_master_image_size_via_flash();
                uint8_t _ptrParam[sizeof(packet.payload.cmdResumeBreakpoint)+2*sizeof(uint32_t)];
                memset(_ptrParam, 0 , sizeof(packet.payload.cmdResumeBreakpoint)+2*sizeof(uint32_t));
                memcpy(_ptrParam+sizeof(uint8_t), ptrParam, paramLen);
                uint32_t _paramLen = paramLen+sizeof(uint8_t);

                tempAccumulateCurrentReciveNumber = app_tota_get_written_sum_image_size_via_flash();
                TRACE(0,"%s tempAccumulateCurrentReciveNumber %d",__func__,tempAccumulateCurrentReciveNumber);
                if((dongle_image_size != 0xFFFFFFFF) && (tempAccumulateCurrentReciveNumber>0) &&
                    !((tempAccumulateCurrentReciveNumber-dongle_image_size) & (MIN_SEG_ALIGN - 1)))//Minimum segment alignment.
                {
                    TRACE(0,"%s  CROSS_CHIP_OTA_ROLE_IS_MASTER",__func__);
                    _ptrParam[0] = CROSS_CHIP_OTA_ROLE_IS_MASTER;
                    send_tota_data_to_headset_by_datapath(_ptrParam, _paramLen);
                    break;
                }
                else if((tempAccumulateCurrentReciveNumber>0) && !((tempAccumulateCurrentReciveNumber-dongle_image_size-headset_master_image_size) &
                    (MIN_SEG_ALIGN - 1)) && (dongle_image_size != 0xFFFFFFFF) && (headset_master_image_size != 0xFFFFFFFF))
                {
                    TRACE(0,"%s  CROSS_CHIP_OTA_ROLE_IS_SLAVE",__func__);
                    _ptrParam[0] = CROSS_CHIP_OTA_ROLE_IS_SLAVE;
                    send_tota_data_to_headset_by_datapath(_ptrParam, _paramLen);
                    break;
                }
                if(tempAccumulateCurrentReciveNumber == 0xFFFFFFFF)
                {
                    tempAccumulateCurrentReciveNumber = 0;
                    updata_tota_ota_current_recive_number(tempAccumulateCurrentReciveNumber);
                }
            }
        }

        if ((ptrParam[1] == OTA_COMMAND_START))
        {
            //save dongle/headset master/heaset slave image size to flash
            app_tota_save_image_size_to_flash(ptrParam);
#if !defined(BLE_USB_AUDIO_IS_DONGLE_ROLE) && defined(UART_TOGGLE_MODE)
            if( CROSS_CHIP_OTA_ROLE_IS_SLAVE == *ptrParam ){
                uint8_t uart_rx_keep_awake[] = "normal";
                app_chip_bridge_send_cmd(UART_MODE_SWITCH_CMD, uart_rx_keep_awake, strlen((char*)uart_rx_keep_awake));
                osDelay(100);
            }
#endif
        }

        if (*ptrParam == tota_cross_chip_ota_role && *ptrParam != CROSS_CHIP_OTA_ROLE_IS_IVAILD)
        {
            ptrParam += sizeof(uint8_t);
            --paramLen;
        }
        else if ((*ptrParam == CROSS_CHIP_OTA_ROLE_IS_MASTER || *ptrParam == CROSS_CHIP_OTA_ROLE_IS_SLAVE)
            && tota_cross_chip_ota_role == CROSS_CHIP_OTA_ROLE_IS_DONGLE)
        {
            //send data to headset master via datapath
            send_tota_data_to_headset_by_datapath(ptrParam, paramLen);
            break;
        }
        else if (*ptrParam == CROSS_CHIP_OTA_ROLE_IS_SLAVE && tota_cross_chip_ota_role == CROSS_CHIP_OTA_ROLE_IS_MASTER)
        {

#ifndef BLE_USB_AUDIO_IS_DONGLE_ROLE
#ifdef APP_CHIP_BRIDGE_MODULE
            //send data to headset slave via uart
            app_chip_bridge_send_cmd(SET_OTA_MODE_CMD, (uint8_t *)ptrParam, paramLen);
            uart_wait_send_sync_msg_done();
#endif
#endif

            break;
        }
        else if (*ptrParam == CROSS_CHIP_OTA_ROLE_IS_IVAILD || tota_cross_chip_ota_role == CROSS_CHIP_OTA_ROLE_IS_IVAILD)
        {
            ASSERT(0, " CROSS_CHIP_OTA_ROLE_IS_IVAILD !!!!!!!!!!!!!");
        }

        app_ota_over_tota_receive_data(ptrParam, paramLen);
    } while(0);
}

#ifdef APP_CHIP_BRIDGE_MODULE

/*
* tb ota case for UART1 communication
*
*/
typedef enum {
    CMD_STATUS_IDLE = 0,
    CMD_STATUS_SENDING,
    CMD_STATUS_TIMEOUT,
    CMD_STATUS_DONE
} cmd_status_t;
static cmd_status_t g_sync_cmd_status = CMD_STATUS_IDLE;

static void app_ota_ota_mode_cmd_handler(uint8_t *p_buff, uint16_t length)
{
    TRACE(0, "[%s] len = %d", __func__, length);
    //TRACE(0,"%s data:",__func__);
    //DUMP8("0x%02x ", p_buff, length);
     g_sync_cmd_status = CMD_STATUS_SENDING;
    app_chip_bridge_send_data_with_waiting_rsp(SET_OTA_MODE_CMD, p_buff, length);
}

static void app_set_ota_mode_cmd_receive_handler(uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s length %d", __func__,length);
    //TRACE(0,"%s data:",__func__);
    //DUMP8("0x%02x ", p_buff, length);
    BLE_send_custom_command(OP_DONGLE_TOTA_RECEIVE_DATA,
                                (uint8_t*)p_buff,
                                length,
                                TRANSMISSION_VIA_NOTIFICATION);
}

POSSIBLY_UNUSED static void app_set_ota_mode_cmd_wait_rsp_timeout(uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
    g_sync_cmd_status = CMD_STATUS_TIMEOUT;
}

int32_t uart_wait_send_sync_msg_done(void)
{
    uint32_t wait_cnt = 0;

    while (g_sync_cmd_status != CMD_STATUS_DONE) {
        osDelay(10);

        if (wait_cnt++ > 100) {     // 1s
            break;
        }
    }
    ASSERT(wait_cnt <= 100, "[%s] Failed to send sync cmd. Status: %d", __func__, g_sync_cmd_status);
    g_sync_cmd_status = CMD_STATUS_IDLE;
    return 0;
}
POSSIBLY_UNUSED static void app_set_ota_mode_cmd_send_rsp_handler(uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
    g_sync_cmd_status = CMD_STATUS_DONE;
}

CHIP_BRIDGE_TASK_COMMAND_TO_ADD(SET_OTA_MODE_CMD,
                                "ota for chip bridge",
                                app_ota_ota_mode_cmd_handler,
                                app_set_ota_mode_cmd_receive_handler,
                                APP_CHIP_BRIDGE_DEFAULT_WAIT_RSP_TIMEOUT_MS,
                                app_set_ota_mode_cmd_wait_rsp_timeout,
                                app_set_ota_mode_cmd_send_rsp_handler,
                                NULL);
#endif
#endif
