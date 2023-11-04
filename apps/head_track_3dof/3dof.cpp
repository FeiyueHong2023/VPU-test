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
 * Application layer function of head tracking 3dof with lsm6dsl sensor
 ****************************************************************************/
#ifdef HEAD_TRACK_ENABLE
#include "hal_trace.h"
#include "hal_i2c.h"
#include "hal_timer.h"
#include "hal_spi.h"
#include "string.h"
#include "lsm6dsl_reg.h"
#include "stdlib.h"

#ifdef RTOS
#include "cmsis_os.h"
#ifdef KERNEL_RTX
#include "rt_Time.h"
#endif
#endif

#include "3dof.h"
#include "head_track_3dof.h"

#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND___) || defined(__VIRTUAL_SURROUND_STEREO__)
#include "app_bt_stream.h"
extern "C" int32_t stereo_surround_set_yaw(float direction);
extern "C" int32_t stereo_surround_set_pitch(float direction);
#endif

#define HEAD_TRACK_SIGNAL_ALGO          (0x01)
#define HEAD_TRACK_TIMER_INTERVAL       (10)

#define     sensor_printf       TRACE

static int head_track_inited = 0;
static IMU_DATA_FETCHER_FUNC imu_data_fetcher = NULL;

#define HEAD_TRACK_THREAD_STACK_SIZE    4096
static void head_track_task_thread(void const* argument);
static osThreadId head_track_thread_id;
static osThreadDef(head_track_task_thread, osPriorityNormal, 1, HEAD_TRACK_THREAD_STACK_SIZE, "head_track_thread");

static struct POSE_S pose_s;
static void head_track_task_thread(void const*argument)
{
    osEvent evt;
    uint32_t signals = 0;
    static struct SENSOR_IMU sensor;

    while (1) {
        evt = osSignalWait(0x0, osWaitForever);
        signals = evt.value.signals;

        if (evt.status == osEventSignal) {
            if (signals & HEAD_TRACK_SIGNAL_ALGO & imu_data_fetcher) {
                /* get current imu sensor data */
                sensor = imu_data_fetcher();

                /* a demo usage to call algo_head_track */
                head_track_3dof_algo_with_fix_param(&pose_s, &sensor);

                /*
                    set spatial audio with 3dof angle if necessary,
                    the spatial audio APIs might be different,
                    here is just a demo usage.
                */
#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND___) || defined(__VIRTUAL_SURROUND_STEREO__)
                stereo_surround_set_yaw((float)pose_s.yaw);
                stereo_surround_set_pitch((float)pose_s.pitch);
#endif
            }
        }
    }
}

static int head_track_task_init(void)
{
    TRACE(0, "%s(true),%d \n\r", __FUNCTION__, __LINE__);

    if (!head_track_inited) {
        head_track_thread_id = osThreadCreate(osThread(head_track_task_thread), NULL);
        if (head_track_thread_id == NULL) {
            TRACE(0, "create head_track_thread failed");
            return -1;
        }
        head_track_inited = 1;
    }
    return 0;
}

void imu_timer_handler(void const *param);
osTimerDef (imu_timer, imu_timer_handler);
osTimerId imu_timer_id = NULL;

void imu_timer_handler(void const *param)
{
    osSignalSet(head_track_thread_id, HEAD_TRACK_SIGNAL_ALGO);
}

void head_angle_reset(void)
{
    head_track_3dof_reset();
}

void imusensor_init(IMU_DATA_FETCHER_FUNC imu_data_fetcher_func)
{
    imu_data_fetcher = imu_data_fetcher_func;

    /* Init head track task  */
    head_track_task_init();

    /* Create imu timer*/
    imu_timer_id = osTimerCreate(osTimer(imu_timer), osTimerPeriodic, NULL);

    /* start head track timer to release signals */
    if (imu_timer_id != NULL) {
        osTimerStart(imu_timer_id, HEAD_TRACK_TIMER_INTERVAL);
    }

    return;
}

#endif