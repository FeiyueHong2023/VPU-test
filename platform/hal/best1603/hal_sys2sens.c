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
#if !defined(CHIP_SUBSYS_BTH) && !defined(CHIP_SUBSYS_DSP)

#include "plat_addr_map.h"
#include "cmsis_nvic.h"
#include "hal_sys2sens.h"
#include "hal_ipc_notify_api.h"
#include "hal_ipc_notify.h"
#include "hal_trace.h"
#include CHIP_SPECIFIC_HDR(reg_cmu)
#include CHIP_SPECIFIC_HDR(reg_senscmu)

#ifndef SYS2SENS_CHAN_REC_CNT
#define SYS2SENS_CHAN_REC_CNT               3
#endif

#define HAL_BUS_WAKE_LOCK_USER_SYS2SENS     HAL_BUS_WAKE_LOCK_USER_9

#define SYS_CFG_PTR_LOC                     (SENS_MAILBOX_BASE + 8)
#define SENS_CFG_PTR_LOC                    (SYS_CFG_PTR_LOC + sizeof(struct HAL_RMT_IPC_CFG_T *))

STATIC_ASSERT((SENS_CFG_PTR_LOC + sizeof(struct HAL_RMT_IPC_CFG_T *)) <= (SENS_MAILBOX_BASE + SENS_MAILBOX_SIZE), "SENS_MAILBOX_SIZE too small");

static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;
static struct SENSCMU_T * const senscmu = (struct SENSCMU_T *)SENS_CMU_BASE;

static const IRQn_Type rx_irq_id[HAL_SYS2SENS_ID_QTY] = {
    SENS2SYS_DATA_IRQn,
    SENS2SYS_DATA1_IRQn,
};

static const IRQn_Type tx_irq_id[HAL_SYS2SENS_ID_QTY] = {
    SYS2SENS_DONE_IRQn,
    SYS2SENS_DONE1_IRQn,
};

RMT_IPC_OPEN_CFG_QUAL
static struct HAL_RMT_IPC_CFG_T sys2sens_cfg;

static struct HAL_RMT_IPC_CH_CFG_T sys2sens_chan_cfg[HAL_SYS2SENS_ID_QTY];
static struct HAL_RMT_IPC_SEND_RECORD_T sys2sens_send_record[HAL_SYS2SENS_ID_QTY * SYS2SENS_CHAN_REC_CNT];

static bool busy_now;

static int hal_sys2sens_peer_irq_set(enum HAL_SYS2SENS_ID_T id, enum HAL_RMT_IPC_IRQ_TYPE_T type)
{
    uint32_t value;

    if (type != HAL_RMT_IPC_IRQ_DATA_IND) {
        return 1;
    }

#ifdef CHIP_SUBSYS_SENS
    if (id == HAL_SYS2SENS_ID_0) {
        value = SENS_CMU_SENS2SYS_DATA_IND_SET;
    } else {
        value = SENS_CMU_SENS2SYS_DATA1_IND_SET;
    }

    senscmu->SENS2SYS_IRQ_SET = value;
#else
    if (id == HAL_SYS2SENS_ID_0) {
        value = CMU_SYS2SES_DATA_IND_SET;
    } else {
        value = CMU_SYS2SES_DATA1_IND_SET;
    }

    cmu->SYS2SENS_IRQ_SET = value;
#endif

    return 0;
}

static int hal_sys2sens_local_irq_clear(enum HAL_SYS2SENS_ID_T id, enum HAL_RMT_IPC_IRQ_TYPE_T type)
{
    uint32_t value;

#ifdef CHIP_SUBSYS_SENS
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SYS2SES_DATA_IND_CLR;
        } else {
            value = SENS_CMU_SYS2SENS_DATA_DONE_CLR;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SYS2SES_DATA1_IND_CLR;
        } else {
            value = SENS_CMU_SYS2SENS_DATA1_DONE_CLR;
        }
    }

    if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
        cmu->SYS2SENS_IRQ_CLR = value;
        // Flush the clear operation immediately
        cmu->SYS2SENS_IRQ_CLR;
    } else {
        senscmu->SENS2SYS_IRQ_CLR = value;
    }
#else
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SENS2SYS_DATA_IND_CLR;
        } else {
            value = CMU_SES2SYS_DATA_DONE_CLR;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SENS2SYS_DATA1_IND_CLR;
        } else {
            value = CMU_SES2SYS_DATA1_DONE_CLR;
        }
    }

    if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
        senscmu->SENS2SYS_IRQ_CLR = value;
        // Flush the clear operation immediately
        senscmu->SENS2SYS_IRQ_CLR;
    } else {
        cmu->SYS2SENS_IRQ_CLR = value;
    }
#endif

    return 0;
}

static int hal_sys2sens_local_irq_mask_set(enum HAL_SYS2SENS_ID_T id, enum HAL_RMT_IPC_IRQ_TYPE_T type)
{
    uint32_t value;

#ifdef CHIP_SUBSYS_SENS
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SYS2SENS_DATA_MSK_SET;
        } else {
            value = SENS_CMU_SENS2SYS_DATA_MSK_SET;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SYS2SENS_DATA1_MSK_SET;
        } else {
            value = SENS_CMU_SENS2SYS_DATA1_MSK_SET;
        }
    }

    senscmu->SENS2SYS_IRQ_SET = value;
#else
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SES2SYS_DATA_MSK_SET;
        } else {
            value = CMU_SYS2SES_DATA_MSK_SET;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SES2SYS_DATA1_MSK_SET;
        } else {
            value = CMU_SYS2SES_DATA1_MSK_SET;
        }
    }

    cmu->SYS2SENS_IRQ_SET = value;
#endif

    return 0;
}

static int hal_sys2sens_local_irq_mask_clear(enum HAL_SYS2SENS_ID_T id, enum HAL_RMT_IPC_IRQ_TYPE_T type)
{
    uint32_t value;

#ifdef CHIP_SUBSYS_SENS
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SYS2SENS_DATA_MSK_CLR;
        } else {
            value = SENS_CMU_SENS2SYS_DATA_MSK_CLR;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SYS2SENS_DATA1_MSK_CLR;
        } else {
            value = SENS_CMU_SENS2SYS_DATA1_MSK_CLR;
        }
    }

    senscmu->SENS2SYS_IRQ_CLR = value;
#else
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SES2SYS_DATA_MSK_CLR;
        } else {
            value = CMU_SYS2SES_DATA_MSK_CLR;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SES2SYS_DATA1_MSK_CLR;
        } else {
            value = CMU_SYS2SES_DATA1_MSK_CLR;
        }
    }

    cmu->SYS2SENS_IRQ_CLR = value;
#endif

    return 0;
}

int hal_sys2sens_rx_irq_pending(enum HAL_SYS2SENS_ID_T id)
{
    uint32_t value;

#ifdef CHIP_SUBSYS_SENS
    if (id == HAL_SYS2SENS_ID_0) {
        value = SENS_CMU_SYS2SENS_DATA_INTR;
    } else {
        value = SENS_CMU_SYS2SENS_DATA1_INTR;
    }

    return !!(senscmu->SENS2SYS_IRQ_SET & value);
#else
    if (id == HAL_SYS2SENS_ID_0) {
        value = CMU_SES2SYS_DATA_INTR;
    } else {
        value = CMU_SES2SYS_DATA1_INTR;
    }

    return !!(cmu->SYS2SENS_IRQ_SET & value);
#endif
}

static void hal_sys2sens_irq_init(uint32_t id)
{
#ifdef CHIP_SUBSYS_SENS
    if (id == HAL_SYS2SENS_ID_0) {
        senscmu->SENS2SYS_IRQ_CLR = SENS_CMU_SYS2SENS_DATA_DONE_CLR | SENS_CMU_SENS2SYS_DATA_IND_CLR;
    } else {
        senscmu->SENS2SYS_IRQ_CLR = SENS_CMU_SYS2SENS_DATA1_DONE_CLR | SENS_CMU_SENS2SYS_DATA1_IND_CLR;
    }
#else
    if (id == HAL_SYS2SENS_ID_0) {
        cmu->SYS2SENS_IRQ_CLR = CMU_SES2SYS_DATA_DONE_CLR | CMU_SYS2SES_DATA_IND_CLR;
    } else {
        cmu->SYS2SENS_IRQ_CLR = CMU_SES2SYS_DATA1_DONE_CLR | CMU_SYS2SES_DATA1_IND_CLR;
    }
#endif

    hal_sys2sens_local_irq_mask_set(id, HAL_RMT_IPC_IRQ_DATA_IND);
    hal_sys2sens_local_irq_mask_set(id, HAL_RMT_IPC_IRQ_DATA_DONE);
}

static void hal_sys2sens_peer_tx_irq_set(uint32_t id32)
{
    enum HAL_SYS2SENS_ID_T id = (enum HAL_SYS2SENS_ID_T)id32;

    hal_sys2sens_peer_irq_set(id, HAL_RMT_IPC_IRQ_DATA_IND);
}

static void hal_sys2sens_local_tx_irq_clear(uint32_t id32)
{
    enum HAL_SYS2SENS_ID_T id = (enum HAL_SYS2SENS_ID_T)id32;

    hal_sys2sens_local_irq_clear(id, HAL_RMT_IPC_IRQ_DATA_DONE);
}

static void hal_sys2sens_rx_irq_suspend(uint32_t id32)
{
    enum HAL_SYS2SENS_ID_T id = (enum HAL_SYS2SENS_ID_T)id32;

    hal_sys2sens_local_irq_mask_clear(id, HAL_RMT_IPC_IRQ_DATA_IND);
}

static void hal_sys2sens_rx_irq_resume(uint32_t id32)
{
    enum HAL_SYS2SENS_ID_T id = (enum HAL_SYS2SENS_ID_T)id32;

    hal_sys2sens_local_irq_mask_set(id, HAL_RMT_IPC_IRQ_DATA_IND);
}

static void hal_sys2sens_rx_done_id32(uint32_t id32)
{
    enum HAL_SYS2SENS_ID_T id = (enum HAL_SYS2SENS_ID_T)id32;

    hal_sys2sens_local_irq_clear(id, HAL_RMT_IPC_IRQ_DATA_IND);
    hal_sys2sens_local_irq_mask_set(id, HAL_RMT_IPC_IRQ_DATA_IND);
}

static int hal_sys2sens_irq_active(uint32_t id, uint32_t type)
{
    uint32_t value;

#ifdef CHIP_SUBSYS_SENS
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SYS2SENS_DATA_INTR_MSK;
        } else {
            value = SENS_CMU_SENS2SYS_DATA_INTR_MSK;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = SENS_CMU_SYS2SENS_DATA1_INTR_MSK;
        } else {
            value = SENS_CMU_SENS2SYS_DATA1_INTR_MSK;
        }
    }

    return !!(senscmu->SENS2SYS_IRQ_SET & value);
#else
    if (id == HAL_SYS2SENS_ID_0) {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SES2SYS_DATA_INTR_MSK;
        } else {
            value = CMU_SYS2SES_DATA_INTR_MSK;
        }
    } else {
        if (type == HAL_RMT_IPC_IRQ_DATA_IND) {
            value = CMU_SES2SYS_DATA1_INTR_MSK;
        } else {
            value = CMU_SYS2SES_DATA1_INTR_MSK;
        }
    }

    return !!(cmu->SYS2SENS_IRQ_SET & value);
#endif
}

static void hal_sys2sens_rx_irq_handler(void)
{
    hal_rmt_ipc_rx_irq_handler(&sys2sens_cfg);
}

static void hal_sys2sens_tx_irq_handler(void)
{
    hal_rmt_ipc_tx_irq_handler(&sys2sens_cfg);
}

#ifdef CORE_SLEEP_POWER_DOWN
static int hal_sys2sens_pm_notif_handler(enum HAL_PM_STATE_T state)
{
    return hal_rmt_ipc_pm_notif_handler(&sys2sens_cfg, state);
}
#endif

RMT_IPC_OPEN_CFG_QUAL
static struct HAL_RMT_IPC_CFG_T sys2sens_cfg = {
    .name = "SYS2SENS",
#ifdef CHIP_SUBSYS_SENS
    .peer_cfg_pp = (const struct HAL_RMT_IPC_CFG_T **)SYS_CFG_PTR_LOC,
    .local_cfg_pp = (const struct HAL_RMT_IPC_CFG_T **)SENS_CFG_PTR_LOC,
#else
    .peer_cfg_pp = (const struct HAL_RMT_IPC_CFG_T **)SENS_CFG_PTR_LOC,
    .local_cfg_pp = (const struct HAL_RMT_IPC_CFG_T **)SYS_CFG_PTR_LOC,
#endif

    .irq_init = hal_sys2sens_irq_init,
    .peer_tx_irq_set = hal_sys2sens_peer_tx_irq_set,
    .local_tx_irq_clear = hal_sys2sens_local_tx_irq_clear,
    .rx_irq_suspend = hal_sys2sens_rx_irq_suspend,
    .rx_irq_resume = hal_sys2sens_rx_irq_resume,
    .rx_done = hal_sys2sens_rx_done_id32,
    .irq_active = hal_sys2sens_irq_active,
    .rx_irq_entry = hal_sys2sens_rx_irq_handler,
    .tx_irq_entry = hal_sys2sens_tx_irq_handler,
#ifdef CORE_SLEEP_POWER_DOWN
    .pm_notif_handler = hal_sys2sens_pm_notif_handler,
#endif

    .wake_lock = HAL_BUS_WAKE_LOCK_USER_SYS2SENS,
    .chan_num = HAL_SYS2SENS_ID_QTY,
    .rec_num_per_chan = ARRAY_SIZE(sys2sens_send_record) / HAL_SYS2SENS_ID_QTY,
    .chan_cfg = &sys2sens_chan_cfg[0],
    .send_record = &sys2sens_send_record[0],
    .rx_irq_id = &rx_irq_id[0],
    .tx_irq_id = &tx_irq_id[0],
    .busy_p = &busy_now,
};

int hal_sys2sens_open(enum HAL_SYS2SENS_ID_T id, HAL_SYS2SENS_RX_IRQ_HANDLER rxhandler, HAL_SYS2SENS_TX_IRQ_HANDLER txhandler, int rx_flowctrl)
{
    return hal_rmt_ipc_open(&sys2sens_cfg, id, rxhandler, txhandler, rx_flowctrl);
}

int hal_sys2sens_close(enum HAL_SYS2SENS_ID_T id)
{
    return hal_rmt_ipc_close(&sys2sens_cfg, id);
}

int hal_sys2sens_start_recv(enum HAL_SYS2SENS_ID_T id)
{
    return hal_rmt_ipc_start_recv(&sys2sens_cfg, id);
}

int hal_sys2sens_stop_recv(enum HAL_SYS2SENS_ID_T id)
{
    return hal_rmt_ipc_stop_recv(&sys2sens_cfg, id);
}

int hal_sys2sens_send_seq(enum HAL_SYS2SENS_ID_T id, const void *data, unsigned int len, unsigned int *seq)
{
    return hal_rmt_ipc_send_seq(&sys2sens_cfg, id, data, len, seq);
}

int hal_sys2sens_send(enum HAL_SYS2SENS_ID_T id, const void *data, unsigned int len)
{
    return hal_sys2sens_send_seq(id, data, len, NULL);
}

int hal_sys2sens_rx_done(enum HAL_SYS2SENS_ID_T id)
{
    return hal_rmt_ipc_rx_done(&sys2sens_cfg, id);
}

int hal_sys2sens_tx_irq_run(enum HAL_SYS2SENS_ID_T id)
{
    return hal_rmt_ipc_tx_irq_run(&sys2sens_cfg, id);
}

int hal_sys2sens_tx_active(enum HAL_SYS2SENS_ID_T id, unsigned int seq)
{
    return hal_rmt_ipc_tx_active(&sys2sens_cfg, id, seq);
}

int hal_sys2sens_opened(enum HAL_SYS2SENS_ID_T id)
{
    return hal_rmt_ipc_opened(&sys2sens_cfg, id);
}

static struct HAL_IPC_NOTIFY_CH_CFG_T sys2sens_ipc_notify_chan_cfg[HAL_SYS2SENS_ID_QTY];
static void hal_sys2sens_ipc_notify_rx_irq_handler(void);
static void hal_sys2sens_ipc_notify_tx_irq_handler(void);


static const struct HAL_IPC_NOTIFY_CFG_T sys2sens_ipc_notify_cfg = {
    .name = "SYS2SENS",
    .core = HAL_IPC_NOTIFY_CORE_SYS_SENS,

    .irq_init = hal_sys2sens_irq_init,
    .peer_tx_irq_set = hal_sys2sens_peer_tx_irq_set,
    .local_tx_irq_clear = hal_sys2sens_local_tx_irq_clear,

    .rx_done = hal_sys2sens_rx_done_id32,
    .irq_active = hal_sys2sens_irq_active,
    .rx_irq_entry = hal_sys2sens_ipc_notify_rx_irq_handler,
    .tx_irq_entry = hal_sys2sens_ipc_notify_tx_irq_handler,
    .chan_num = HAL_SYS2SENS_ID_QTY,
    .chan_cfg = &sys2sens_ipc_notify_chan_cfg[0],

    .rx_irq_id = &rx_irq_id[0],
    .tx_irq_id = &tx_irq_id[0],

};

static void hal_sys2sens_ipc_notify_rx_irq_handler(void)
{
    hal_ipc_notify_internal_rx_irq_handler(&sys2sens_ipc_notify_cfg);
}

static void hal_sys2sens_ipc_notify_tx_irq_handler(void)
{
    hal_ipc_notify_internal_tx_irq_handler(&sys2sens_ipc_notify_cfg);
}

int hal_sys2sens_ipc_notify_open(enum HAL_SYS2SENS_ID_T id, HAL_SYS2SENS_NOTIFY_IRQ_HANDLER rxhandler, HAL_SYS2SENS_NOTIFY_IRQ_HANDLER txhandler)
{
    return hal_ipc_notify_internal_open(&sys2sens_ipc_notify_cfg, id, rxhandler, txhandler);
}

int hal_sys2sens_ipc_notify_start_recv(enum HAL_SYS2SENS_ID_T id)
{
    return hal_ipc_notify_internal_start_recv(&sys2sens_ipc_notify_cfg, id);
}

int hal_sys2sens_ipc_notify_interrupt_core(enum HAL_SYS2SENS_ID_T id)
{
    return hal_ipc_notify_internal_peer_irq_set(&sys2sens_ipc_notify_cfg, (uint32_t)id);
}

int hal_sys2sens_ipc_notify_close(enum HAL_SYS2SENS_ID_T id)
{
    return hal_ipc_notify_internal_close(&sys2sens_ipc_notify_cfg, (uint32_t)id);
}

#endif

