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
#ifndef __HAL_BTH2SENS_H__
#define __HAL_BTH2SENS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_rmt_ipc.h"

#define HAL_BTH2SENS_ID_T                   HAL_RMT_IPC_ID_T
#define HAL_BTH2SENS_ID_0                   HAL_RMT_IPC_ID_0
#define HAL_BTH2SENS_ID_1                   HAL_RMT_IPC_ID_1
#define HAL_BTH2SENS_ID_QTY                 (HAL_RMT_IPC_ID_1 + 1)

typedef HAL_RMT_IPC_RX_IRQ_HANDLER HAL_BTH2SENS_RX_IRQ_HANDLER;

typedef HAL_RMT_IPC_TX_IRQ_HANDLER HAL_BTH2SENS_TX_IRQ_HANDLER;

typedef void (*HAL_BTH2SENS_NOTIFY_IRQ_HANDLER)(uint32_t core, uint32_t id);

int hal_bth2sens_open(enum HAL_BTH2SENS_ID_T id, HAL_BTH2SENS_RX_IRQ_HANDLER rxhandler, HAL_BTH2SENS_TX_IRQ_HANDLER txhandler, int rx_flowctrl);

int hal_bth2sens_close(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_start_recv(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_stop_recv(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_send_seq(enum HAL_BTH2SENS_ID_T id, const void *data, unsigned int len, unsigned int *seq);

int hal_bth2sens_send(enum HAL_BTH2SENS_ID_T id, const void *data, unsigned int len);

int hal_bth2sens_rx_done(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_tx_irq_run(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_tx_active(enum HAL_BTH2SENS_ID_T id, unsigned int seq);

int hal_bth2sens_opened(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_rx_irq_pending(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_ipc_notify_open(enum HAL_BTH2SENS_ID_T id, HAL_BTH2SENS_NOTIFY_IRQ_HANDLER rxhandler, HAL_BTH2SENS_NOTIFY_IRQ_HANDLER txhandler);

int hal_bth2sens_ipc_notify_start_recv(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_ipc_notify_interrupt_core(enum HAL_BTH2SENS_ID_T id);

int hal_bth2sens_ipc_notify_close(enum HAL_BTH2SENS_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

