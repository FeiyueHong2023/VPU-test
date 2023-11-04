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
#include "app_anc_assist_core.h"
#include "assist/anc_assist_mic.h"
#include "hal_trace.h"
#include "string.h"
#ifdef ANC_ASSIST_ON_DSP
#include "app_dsp_m55.h"
#include "mcu_dsp_m55_app.h"
//#include "app_overlay.h"
#endif
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include <string.h>

// #define ANC_ASSIST_CORE_MUTEX

#ifdef ANC_ASSIST_ON_DSP
static uint32_t g_core_user_status = 0;
static uint32_t g_fifo_total_cnt = 0;
static uint32_t g_fifo_overflow_cnt = 0;

#define APP_ANC_ASSIST_TIMEROUT_MS (2000)

static anc_assist_fifo_t *g_assist_fifo = NULL;

osSemaphoreDef(app_anc_assist_sema);
osSemaphoreId app_anc_assist_sema_id = NULL;

#ifdef ANC_ASSIST_CORE_MUTEX
osMutexId _anc_assist_core_mutex_id = NULL;
osMutexDef(_anc_assist_core_mutex);
#endif

#define COMM_ANC_ASSIST_WAIT_RSP_TIMEOUT_MS (500)
typedef void(*app_dsp_m55_anc_assist_rsp_received_handler_t)(unsigned char * ptr, short len);
app_dsp_m55_anc_assist_rsp_received_handler_t g_anc_assist_rsp_done_handler = NULL;

static void comm_bth_anc_assist_send_cmd_to_m55_handler(uint8_t* ptr, uint16_t len)
{
    app_dsp_m55_bridge_send_data_with_waiting_rsp(MCU_DSP_M55_ANC_ASSIST_CMD, ptr, len);
}

static void comm_bth_anc_assist_receive_data_from_m55_handler(uint8_t* ptr, uint16_t len)
{
#ifdef ANC_APP
    app_anc_assist_core_to_bth_data_t *receive_data = (app_anc_assist_core_to_bth_data_t *)ptr;
    //TRACE(2, "BTH<----M55 assist_user:%d len:%d", receive_data->user, receive_data->len);
    if (app_anc_assist_get_result_register(receive_data->user) != NULL) {
        app_anc_assist_get_result_register(receive_data->user)(receive_data->buf, receive_data->len,
                                                                NULL, receive_data->sub_cmd);
    }
#endif
}

static void comm_bth_anc_assist_tx_done_handler(uint16_t cmdCode,
    uint8_t* ptr, uint16_t len)
{

}

static void comm_bth_anc_assist_wait_rsp_timeout(uint8_t* ptr, uint16_t len)
{
    TRACE(0, "Warning %s", __func__);
}

static void comm_bth_anc_assist_received_rsp_handler(uint8_t* ptr, uint16_t len)
{
    if (g_anc_assist_rsp_done_handler) {
        g_anc_assist_rsp_done_handler(ptr, len);
    }
}

void app_dsp_m55_core_anc_assist_rsp_received_cb_register(app_dsp_m55_anc_assist_rsp_received_handler_t handler)
{
    g_anc_assist_rsp_done_handler = handler;
}

M55_CORE_BRIDGE_TASK_COMMAND_TO_ADD(MCU_DSP_M55_ANC_ASSIST_CMD,
                                "ANC Assist CMD",
                                comm_bth_anc_assist_send_cmd_to_m55_handler,
                                comm_bth_anc_assist_receive_data_from_m55_handler,
                                COMM_ANC_ASSIST_WAIT_RSP_TIMEOUT_MS,
                                comm_bth_anc_assist_wait_rsp_timeout,
                                comm_bth_anc_assist_received_rsp_handler,
                                comm_bth_anc_assist_tx_done_handler);

static void comm_bth_anc_send_data_to_m55_handler(uint8_t* ptr, uint16_t len)
{
    app_dsp_m55_bridge_send_data_without_waiting_rsp(MCU_DSP_M55_ANC_ASSIST_DATA, ptr, len);
}

M55_CORE_BRIDGE_TASK_COMMAND_TO_ADD(MCU_DSP_M55_ANC_ASSIST_DATA,
                                "ANC Assist Data",
                                comm_bth_anc_send_data_to_m55_handler,
                                NULL,
                                0,
                                NULL,
                                NULL,
                                NULL);

static void _anc_assist_core_create_lock(void)
{
#ifdef ANC_ASSIST_CORE_MUTEX
    if (_anc_assist_core_mutex_id == NULL) {
        _anc_assist_core_mutex_id = osMutexCreate((osMutex(_anc_assist_core_mutex)));
    }
#endif
}

static void _anc_assist_core_lock(void)
{
#ifdef ANC_ASSIST_CORE_MUTEX
    osMutexWait(_anc_assist_core_mutex_id, osWaitForever);
#endif
}

static void _anc_assist_core_unlock(void)
{
#ifdef ANC_ASSIST_CORE_MUTEX
    osMutexRelease(_anc_assist_core_mutex_id);
#endif
}

static int app_anc_assist_sema_init(void)
{
    if (app_anc_assist_sema_id == NULL) {
        app_anc_assist_sema_id = osSemaphoreCreate(osSemaphore(app_anc_assist_sema), 0);
    }
    ASSERT(app_anc_assist_sema_id, "create app_anc_assist_sema_id fail!");

    return 0;
}

static int app_anc_assist_sema_deinit(void)
{
    int ret;
    if(app_anc_assist_sema_id) {
        ret = osSemaphoreDelete(app_anc_assist_sema_id);
        app_anc_assist_sema_id = NULL;
    }

    return ret;
}

static int app_anc_assist_sema_wait(uint32_t timeout)
{
    int ret = 0;
    if(app_anc_assist_sema_id) {
        ret = osSemaphoreAcquire(app_anc_assist_sema_id, timeout);
    }

    return ret;
}

static void app_anc_assist_sema_process_done(unsigned char *ptr, short len)
{
    if ((ptr != NULL) && (len != 0)) {
        ASSERT(len == sizeof(app_anc_assist_core_to_bth_rsp_t), "[%s] Invalid len: %d", __func__, len);
        app_anc_assist_core_to_bth_rsp_t *rsp = (app_anc_assist_core_to_bth_rsp_t *)ptr;
        TRACE(3, "[%s] user: %d, cmd: %d", __func__, rsp->user, rsp->cmd);
        if (rsp->cmd == APP_ANC_ASSIST_CORE_OPEN) {
            g_assist_fifo = rsp->fifo_ptr;
            TRACE(3, "[%s] fifo addr: %p, size: %d", __func__, g_assist_fifo, g_assist_fifo->talk_fifo.size);
        }
    }

    if(app_anc_assist_sema_id) {
        osSemaphoreRelease(app_anc_assist_sema_id);
    }
}

static int app_anc_assist_rpc_init(void)
{
    app_anc_assist_sema_init();
    app_dsp_m55_core_anc_assist_rsp_received_cb_register(app_anc_assist_sema_process_done);

    return 0;
}


static int app_anc_assist_rpc_deinit(void)
{
    app_dsp_m55_core_anc_assist_rsp_received_cb_register(NULL);
    app_anc_assist_sema_deinit();

    return 0;
}

/* If talk_len != other_len happen always, it's better to clear fifo on M55*/
static void check_fifo_pcm_len(void)
{
    uint32_t talk_len = 0;
    uint32_t other_len = 0;

    if (g_fifo_total_cnt % 400 == 0) {
        talk_len = kfifo_get_free_space(&g_assist_fifo->talk_fifo);

        other_len = kfifo_get_free_space(&g_assist_fifo->ff_mic_fifo);
        if (talk_len != other_len) {
            TRACE(3, "[%s] WARNING: ff_mic_fifo %d != %d", __func__, talk_len, other_len);
        }

        other_len = kfifo_get_free_space(&g_assist_fifo->fb_mic_fifo);
        if (talk_len != other_len) {
            TRACE(3, "[%s] WARNING: fb_mic_fifo %d != %d", __func__, talk_len, other_len);
        }

        other_len = kfifo_get_free_space(&g_assist_fifo->ref_fifo);
        if (talk_len != other_len) {
            TRACE(3, "[%s] WARNING: ref_fifo %d != %d", __func__, talk_len, other_len);
        }

#ifdef ANC_ASSIST_VPU
        other_len = kfifo_get_free_space(&g_assist_fifo->vpu_fifo);
        if (talk_len != other_len) {
            TRACE(3, "[%s] WARNING: vpu_fifo %d != %d", __func__, talk_len, other_len);
        }
#endif
    }
}

void app_anc_assist_process_put_data(float *in_buf[], uint32_t frame_len)
{
    if (g_assist_fifo == NULL) {
        TRACE(1, "[%s] g_assist_fifo is NULL", __func__);
        return;
    }

    uint32_t len = frame_len * sizeof(float);
    if (kfifo_get_free_space(&g_assist_fifo->talk_fifo) >= len) {
        kfifo_put(&g_assist_fifo->ff_mic_fifo, (uint8_t *)in_buf[MIC_INDEX_FF], len);
        kfifo_put(&g_assist_fifo->fb_mic_fifo, (uint8_t *)in_buf[MIC_INDEX_FB], len);
        kfifo_put(&g_assist_fifo->ref_fifo, (uint8_t *)in_buf[MIC_INDEX_REF], len);
#ifdef ANC_ASSIST_VPU
        kfifo_put(&g_assist_fifo->vpu_fifo, (uint8_t *)in_buf[MIC_INDEX_VPU], len);
#endif
        // BTH put talk fifo in the end, M55 use this fifo to check length
        kfifo_put(&g_assist_fifo->talk_fifo, (uint8_t *)in_buf[MIC_INDEX_TALK], len);
    } else {
        TRACE(0, "[%s] Input buffer is overflow", __FUNCTION__);
        g_fifo_overflow_cnt++;
    }
    g_fifo_total_cnt++;

    check_fifo_pcm_len();
}

void app_anc_assist_send_data_to_m55(float *in_buf[], uint32_t frame_len)
{
    app_anc_assist_process_put_data(in_buf, frame_len);
#ifdef ANC_ASSIST_TRIGGER_DSP_PROCESS
    app_dsp_m55_bridge_send_cmd(MCU_DSP_M55_ANC_ASSIST_DATA, NULL, 0);
#endif
}

void app_anc_assist_send_cmd_to_m55(anc_assist_user_t user, app_anc_assist_core_cmd_t cmd)
{
    app_anc_assist_core_to_m55_data_t tx_cmd;
    uint32_t send_len = sizeof(app_anc_assist_core_to_m55_data_t) - sizeof(tx_cmd.ctrl_buf);

    memset(&tx_cmd, 0, sizeof(tx_cmd));
    tx_cmd.user = user;
    tx_cmd.cmd = cmd;

    app_dsp_m55_bridge_send_cmd(MCU_DSP_M55_ANC_ASSIST_CMD, (uint8_t *)&tx_cmd, send_len);
    int res = app_anc_assist_sema_wait(APP_ANC_ASSIST_TIMEROUT_MS);
    if (res != osOK) {
        TRACE(2,"%s err = %d",__func__,res);
    }

    TRACE(3, "[%s] user: %d, cmd: %d. OK", __func__, user, cmd);
}

void app_anc_assist_send_ctrl_to_m55(anc_assist_user_t user, app_anc_assist_core_cmd_t cmd, uint32_t ctrl, uint8_t *buf, uint32_t len)
{
    app_anc_assist_core_to_m55_data_t tx_cmd;
    uint32_t send_len = sizeof(app_anc_assist_core_to_m55_data_t) - sizeof(tx_cmd.ctrl_buf) + len;

    memset(&tx_cmd, 0, sizeof(tx_cmd));
    tx_cmd.user = user;
    tx_cmd.cmd = cmd;
    tx_cmd.ctrl = ctrl;
    tx_cmd.ctrl_buf_len = len;
    if (len > 0) {
        memcpy(tx_cmd.ctrl_buf, buf, len);
    }

    app_dsp_m55_bridge_send_cmd(MCU_DSP_M55_ANC_ASSIST_CMD, (uint8_t *)&tx_cmd, send_len);
    int res = app_anc_assist_sema_wait(APP_ANC_ASSIST_TIMEROUT_MS);
    if (res != osOK) {
        TRACE(2,"%s err = %d",__func__,res);
    }

    TRACE(4, "[%s] user: %d, cmd: %d, ctrl: %d. OK", __func__, user, cmd, ctrl);
}


uint32_t app_anc_assist_core_get_status(void)
{
    return g_core_user_status;
}

void app_anc_assist_core_open(app_anc_assist_core_t core, anc_assist_user_t user)
{
    TRACE(4, "[%s] core: %d, user: %d, status: 0x%x", __func__, core, user, g_core_user_status);

    _anc_assist_core_lock();
    if ((g_core_user_status & (0x1 << user)) != 0) {
        TRACE(2, "[%s] WARNING: user(%d) has been opend", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (app_anc_assist_core_user_is_suspend(user)) {
        TRACE(2, "[%s] WARNING: user(%d) is suspending", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (core == APP_ANC_ASSIST_CORE_BTH_M55) {
        if (g_core_user_status == 0) {
            g_fifo_total_cnt = 0;
            g_fifo_overflow_cnt = 0;
            app_anc_assist_rpc_init();
            app_dsp_m55_init(APP_DSP_M55_USER_VOICE_ASSIST);
        }

        uint32_t wait_cnt = 0;

        while (app_dsp_m55_is_running() == false) {
            osDelay(10);

            if (wait_cnt++ > 300000) {      // 3s
                ASSERT(0, "%s dsp is hung", __func__);
            }
        } 
 
        // Check overlay case
        if (user > ANC_ASSIST_USER_ALGO_DSP) {
         //   app_second_overlay_subsys_select(APP_OVERLAY_M55, APP_SECOND_OVERLAY_SUBSYS_NON_CALL);
        }

        app_anc_assist_send_cmd_to_m55(user, APP_ANC_ASSIST_CORE_OPEN);
        g_core_user_status = g_core_user_status | (0x1 << user);
    } else {
        ASSERT(0, "[%s] core (%d) not supported", __FUNCTION__, core);
    }
    _anc_assist_core_unlock();
}

void app_anc_assist_core_close(app_anc_assist_core_t core, anc_assist_user_t user)
{
    TRACE(4, "[%s] core: %d, user: %d, status: 0x%x", __func__, core, user, g_core_user_status);

    _anc_assist_core_lock();
    if ((g_core_user_status & (0x1 << user)) == 0) {
        TRACE(2, "[%s] WARNING: user(%d) has been closed", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (app_anc_assist_core_user_is_suspend(user)) {
        TRACE(2, "[%s] WARNING: user(%d) is suspending", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (core == APP_ANC_ASSIST_CORE_BTH_M55) {
        app_anc_assist_send_cmd_to_m55(user, APP_ANC_ASSIST_CORE_CLOSE);
        g_core_user_status = g_core_user_status & ~(0x1 << user);
        if (g_core_user_status == 0) {
            app_dsp_m55_deinit(APP_DSP_M55_USER_VOICE_ASSIST);
            app_anc_assist_rpc_deinit();
            TRACE(3, "[%s] total_cnt: %d, overflow_cnt: %d", __func__, g_fifo_total_cnt, g_fifo_overflow_cnt);
        }
    } else {
        ASSERT(0, "[%s] core (%d) not supported", __FUNCTION__, core);
    }
    _anc_assist_core_unlock();
}

void app_anc_assist_core_reset(app_anc_assist_core_t core, anc_assist_user_t user)
{
    TRACE(4, "[%s] core: %d, user: %d, status: 0x%x", __func__, core, user, g_core_user_status);

    _anc_assist_core_lock();

    if ((g_core_user_status & (0x1 << user)) == 0) {
        TRACE(2, "[%s] WARNING: user(%d) has been closed, can not reset", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (app_anc_assist_core_user_is_suspend(user)) {
        TRACE(2, "[%s] WARNING: user(%d) is suspending", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (core == APP_ANC_ASSIST_CORE_BTH_M55) {
        app_anc_assist_send_cmd_to_m55(user, APP_ANC_ASSIST_CORE_RESET);
    } else {
        ASSERT(0, "[%s] core (%d) not supported", __FUNCTION__, core);
    }
    _anc_assist_core_unlock();
}

void app_anc_assist_core_ctrl(app_anc_assist_core_t core, anc_assist_user_t user, uint32_t ctrl, uint8_t *buf, uint32_t len)
{
    TRACE(5, "[%s] core: %d, user: %d, status: 0x%x, ctrl: %d", __func__, core, user, g_core_user_status, ctrl);

    _anc_assist_core_lock();

    if ((g_core_user_status & (0x1 << user)) == 0) {
        TRACE(2, "[%s] WARNING: user(%d) has been closed, can not set ctrl", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (app_anc_assist_core_user_is_suspend(user)) {
        TRACE(2, "[%s] WARNING: user(%d) is suspending", __func__, user);
        _anc_assist_core_unlock();
        return;
    }

    if (core == APP_ANC_ASSIST_CORE_BTH_M55) {
        app_anc_assist_send_ctrl_to_m55(user, APP_ANC_ASSIST_CORE_CTRL, ctrl, buf, len);
    } else {
        ASSERT(0, "[%s] core (%d) not supported", __FUNCTION__, core);
    }
    _anc_assist_core_unlock();
}

void app_anc_assist_core_process(app_anc_assist_core_t core, float *in_buf[],  uint32_t frame_len)
{
    // _anc_assist_core_lock();
    if ((g_core_user_status == 0) || (g_core_user_status == (g_core_user_status & (0x1 << ANC_ASSIST_USER_KWS)))) {
        // _anc_assist_core_unlock();
        return;
    }
    if (core == APP_ANC_ASSIST_CORE_BTH_M55) {
        app_anc_assist_send_data_to_m55(in_buf, frame_len);
    } else {
        ASSERT(0, "[%s] core (%d) not supported", __FUNCTION__, core);
    }
    // _anc_assist_core_unlock();
}

static uint32_t g_suspend_users = 0;

static bool g_core_suspend_users_flag = false;
static uint32_t g_core_suspend_users_cache_status = 0;

bool app_anc_assist_core_user_is_suspend(anc_assist_user_t user)
{
    return (g_core_suspend_users_flag && (g_suspend_users & (0x1 << user)));
}

void app_anc_assist_core_suspend_users(bool en, uint32_t users)
{
    uint32_t user = 0;
    uint32_t suspend_users_tab = 0;
    TRACE(3, "[%s] en: %d, status: 0x%x", __func__, en, g_core_user_status);

    if (g_core_suspend_users_flag == en) {
        TRACE(2, "[%s] WARNING: Invalid suspend: %d", __func__, en);
        return;
    }

    if (en) {
        g_core_suspend_users_cache_status = 0;

        user = 0;
        suspend_users_tab = users;
        while (suspend_users_tab) {
            if (users & g_core_user_status & (0x1 << user)) {
                app_anc_assist_core_close(APP_ANC_ASSIST_CORE_BTH_M55, user);
                g_core_suspend_users_cache_status |= (0x1 << user);
            }
            suspend_users_tab &= ~(0x1 << user);
            user++;
        }

        g_core_suspend_users_flag = true;
    } else {
        g_core_suspend_users_flag = false;

        user = 0;
        suspend_users_tab = g_suspend_users;
        while (suspend_users_tab) {
            if (g_core_suspend_users_cache_status & (0x1 << user)) {
                if (g_core_user_status & (0x1 << user)) {
                    TRACE(2, "[%s] WARNING: Invalid user(%d) anti-suspend", __func__, user);
                } else {
                    app_anc_assist_core_open(APP_ANC_ASSIST_CORE_BTH_M55, user);
                }
            }
            suspend_users_tab &= ~(0x1 << user);
            user++;
        }

        g_core_suspend_users_cache_status = 0;
    }
    g_suspend_users = users;
}

void app_anc_assist_core_init(void)
{
    _anc_assist_core_create_lock();
}

void app_anc_assist_core_deinit(void)
{
    ;
}
#endif
