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
#ifdef __HAL_IPC_NOTIFY_SUPPORT__
#include "hal_ipc_notify_api.h"
#include "hal_ipc_notify.h"
#include "hal_trace.h"
#include "hal_sys2bth.h"
#include "hal_mcu2btc.h"
#include "hal_mcu2sens.h"
#include "hal_mcu2dsp.h"
#include "hal_bth2sens.h"

int hal_ipc_notify_interrupt_core(enum HAL_IPC_NOTIFY_CORE_TYPE_T core, enum HAL_IPC_NOTIFY_SUB_ID_T id)
{
    if (core >= HAL_IPC_NOTIFY_CORE_QTY) {
        return 1;
    }

    if (id >= HAL_IPC_NOTIFY_SUB_ID_QTY) {
        return 1;
    }

    switch(core)
    {
#if !defined(CHIP_SUBSYS_SENS)
        case HAL_IPC_NOTIFY_CORE_SYS_BTH:
            hal_sys2bth_ipc_notify_interrupt_core(id);
            break;
#endif
#if !defined(CHIP_SUBSYS_SENS) && !defined(CHIP_SUBSYS_BTH)
        case HAL_IPC_NOTIFY_CORE_SYS_DSP:
            hal_mcu2dsp_ipc_notify_interrupt_core(id);
            break;
#endif
        case HAL_IPC_NOTIFY_CORE_SYS_BTC:
            break;
        case HAL_IPC_NOTIFY_CORE_SYS_SENS:
            hal_mcu2sens_ipc_notify_interrupt_core(id);
            break;
#if defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH)
        case HAL_IPC_NOTIFY_CORE_BTH_SENS:
            hal_bth2sens_ipc_notify_interrupt_core(id);
            break;
#endif
#if !defined(CHIP_SUBSYS_SENS)
        case HAL_IPC_NOTIFY_CORE_BTH_BTC:
            hal_mcu2btc_ipc_notify_interrupt_core(id);
            break;
#endif
        default:
            break;
    }
    return 0;
}


int hal_ipc_notify_open(enum HAL_IPC_NOTIFY_CORE_TYPE_T core, enum HAL_IPC_NOTIFY_SUB_ID_T id, struct HAL_IPC_NOTIFY_IRQ_CB_T irq_cb)
{
    TRACE_HAL_IPC_NOTIFY_I("%s core:%d id:%d", __func__, core, id);

    if (core >= HAL_IPC_NOTIFY_CORE_QTY) {
        return 1;
    }

    if (id >= HAL_IPC_NOTIFY_SUB_ID_QTY) {
        return 1;
    }

    switch(core)
    {
#if !defined(CHIP_SUBSYS_SENS)
        case HAL_IPC_NOTIFY_CORE_SYS_BTH:
            hal_sys2bth_ipc_notify_open(id, irq_cb.rx_handler, irq_cb.tx_handler);
            hal_sys2bth_ipc_notify_start_recv(id);
            break;
#endif
#if !defined(CHIP_SUBSYS_SENS) && !defined(CHIP_SUBSYS_BTH)
        case HAL_IPC_NOTIFY_CORE_SYS_DSP:
            hal_mcu2dsp_ipc_notify_open(id, irq_cb.rx_handler, irq_cb.tx_handler);
            hal_mcu2dsp_ipc_notify_start_recv(id);
            break;
#endif
        case HAL_IPC_NOTIFY_CORE_SYS_BTC:
            break;
        case HAL_IPC_NOTIFY_CORE_SYS_SENS:
            hal_mcu2sens_ipc_notify_open(id, irq_cb.rx_handler, irq_cb.tx_handler);
            hal_mcu2sens_ipc_notify_start_recv(id);
            break;
#if defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH)
        case HAL_IPC_NOTIFY_CORE_BTH_SENS:
            hal_bth2sens_ipc_notify_open(id, irq_cb.rx_handler, irq_cb.tx_handler);
            hal_bth2sens_ipc_notify_start_recv(id);
            break;
#endif
#if !defined(CHIP_SUBSYS_SENS)
        case HAL_IPC_NOTIFY_CORE_BTH_BTC:
            hal_mcu2btc_ipc_notify_open(id, irq_cb.rx_handler, irq_cb.tx_handler);
            hal_mcu2btc_ipc_notify_start_recv(id);
#endif
            break;
        default:
            break;
    }

    return 0;
}


int hal_ipc_notify_close(enum HAL_IPC_NOTIFY_CORE_TYPE_T core, enum HAL_IPC_NOTIFY_SUB_ID_T id)
{
    TRACE_HAL_IPC_NOTIFY_I("%s core:%d id:%d", __func__, core, id);

    if (core >= HAL_IPC_NOTIFY_CORE_QTY) {
        return 1;
    }

    if (id >= HAL_IPC_NOTIFY_SUB_ID_QTY) {
        return 1;
    }

    switch(core)
    {
#if !defined(CHIP_SUBSYS_SENS)
        case HAL_IPC_NOTIFY_CORE_SYS_BTH:
            hal_sys2bth_ipc_notify_close(id);
            break;
#endif
#if !defined(CHIP_SUBSYS_SENS) && !defined(CHIP_SUBSYS_BTH)
        case HAL_IPC_NOTIFY_CORE_SYS_DSP:
            hal_mcu2dsp_ipc_notify_close(id);
            break;
#endif
        case HAL_IPC_NOTIFY_CORE_SYS_BTC:
            break;
        case HAL_IPC_NOTIFY_CORE_SYS_SENS:
            hal_mcu2sens_ipc_notify_close(id);
            break;
#if defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH)
        case HAL_IPC_NOTIFY_CORE_BTH_SENS:
            hal_bth2sens_ipc_notify_close(id);
            break;
#endif
#if !defined(CHIP_SUBSYS_SENS)
        case HAL_IPC_NOTIFY_CORE_BTH_BTC:
            hal_mcu2btc_ipc_notify_close(id);
            break;
#endif
        default:
            break;
    }

    return 0;
}
#endif
