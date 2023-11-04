/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#if !(defined(CHIP_ROLE_CP) || defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) ^ defined(BTH_AS_MAIN_MCU)))

#include "hal_trace.h"
#include "hal_timer.h"
#include "tool_msg.h"
#include "dsp_loader.h"
#include "hal_psc.h"
#include "hal_cmu.h"
#include "app_rpc_api.h"
#ifdef BTH_AS_MAIN_MCU
#include "hal_sys2bth.h"
#else
#include "hal_cache.h"
#include "hal_mcu2dsp.h"
#endif
#include "hal_sysfreq.h"
#include "cmsis.h"
#include "cmsis_nvic.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "rx_dsp_hifi4_trc.h"

#ifdef DSP_BOOT_FORM_FLASH
#define DSP_SROM_BASE     ((DSP_LOGIC_SROM_BASE | 0x04000000) + 0x10)
#elif defined(PROGRAMMER)
#define DSP_SROM_BASE     0x00880000
#else
#define DSP_SROM_BASE     0x00800000
#endif

#define DSP_DIGITAL_REG(a) *(volatile uint32_t *)(a)

#ifdef DSP_COMBINE_BIN
extern uint32_t __dsp_hifi4_code_start_flash[];
extern uint32_t __dsp_hifi4_code_end_flash[];
#endif

// #define MEM_CFG_INFO_PRINT
// #define DSP_MEM_CHECK
// #define DSP_CORE_DEEP_SLEEP

#define MEM_CFG_REG_ADDR0 (0x400800DC)
#define MEM_CFG_REG_ADDR1 (0x400800E0)

enum MEM_CFG_T {
    MEM_CFG_AXI_RAM,
    MEM_CFG_M55_ITCM,
    MEM_CFG_DSP_ITCM,
    MEM_CFG_M55_DTCM,
    MEM_CFG_DSP_DTCM
};

typedef struct {
    uint32_t blk0 : 3; //[2:0]
    uint32_t blk1 : 3; //[5:3]
    uint32_t blk2 : 3; //[8:6]
    uint32_t blk3 : 3; //[11:9]
    uint32_t blk4 : 3; //[14:12]
    uint32_t blk5 : 3; //[17:15]
    uint32_t blk6 : 3; //[20:18]
    uint32_t blk7 : 3; //[23:21]
    uint32_t blk8 : 3; //[26:24]
    uint32_t blk9 : 3; //[29:27]
} MEM_CFG_REG_DC_T;

typedef struct {
    uint32_t blk10 : 3; //[2:0]
    uint32_t blk11 : 3; //[5:3]
    uint32_t blk12 : 3; //[8:6]
    uint32_t blk13 : 3; //[11:9]
    uint32_t blk14 : 3; //[14:12]
    uint32_t blk15 : 3; //[17:15]
    uint32_t rsvd  : 14;//[31:18]
} MEM_CFG_REG_E0_T;

typedef int (*DSP_MEMORY_CONFIG)(void);

struct dsp_memory_map_t {
    uint32_t iram_start;
    uint32_t iram_end;
    uint32_t dram_start;
    uint32_t dram_end;
    uint32_t sram_start;
    uint32_t sram_end;
};

struct dsp_segments_info_t {
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t store_addr;
};

struct dsp_binary_struct_t {
    struct boot_hdr_t hdr;
    struct dsp_memory_map_t memory_map;
    struct dsp_segments_info_t segments_info[0];
};

struct dsp_memory_config_t {
    struct dsp_memory_map_t memory_map;
    DSP_MEMORY_CONFIG memory_config_fn;
};

static int dspMemoryConfig_i1d2s13(void){
    TRACE(0, "%s", __func__);
    DSP_DIGITAL_REG(0x400800e0)=0x00024000;
    DSP_DIGITAL_REG(0x400800dc)=0x00000002;
#if defined(HIFI_DISABLE_CACHE)
    DSP_DIGITAL_REG(0x50000114)=0x100003E0;
#endif
    return 0;
}

static int dspMemoryConfig_i0d0s16(void){
    TRACE(0, "%s", __func__);
    DSP_DIGITAL_REG(0x400800e0)=0x00000000;
    DSP_DIGITAL_REG(0x400800dc)=0x00000000;
#if defined(HIFI_DISABLE_CACHE)
    DSP_DIGITAL_REG(0x50000114)=0x100003E0;
#endif
    return 0;
}

static int dspMemoryConfig_i6d10s0(void){
    TRACE(0, "%s", __func__);
    DSP_DIGITAL_REG(0x400800e0)=0x00024924;
    DSP_DIGITAL_REG(0x400800dc)=0x24912492;
#if defined(HIFI_DISABLE_CACHE)
    DSP_DIGITAL_REG(0x50000114)=0x100003E0;
#endif
    return 0;
}

static int dspMemoryConfig_i3d6s0(void){
    TRACE(0, "%s", __func__);
    DSP_DIGITAL_REG(0x400800e0)=0x00024924;
    DSP_DIGITAL_REG(0x400800dc)=0x1B6D9292;
#if defined(HIFI_DISABLE_CACHE)
    DSP_DIGITAL_REG(0x50000114)=0x100003E0;
#endif
    return 0;
}

#ifdef MEM_CFG_INFO_PRINT
#define CASE_ENUM(e) case e: return "["#e"]"

static const char *mem_cfg_to_str(uint32_t cfg)
{
    switch (cfg) {
    CASE_ENUM(MEM_CFG_AXI_RAM );
    CASE_ENUM(MEM_CFG_M55_ITCM);
    CASE_ENUM(MEM_CFG_DSP_ITCM);
    CASE_ENUM(MEM_CFG_M55_DTCM);
    CASE_ENUM(MEM_CFG_DSP_DTCM);
    }
    return "[ ]";
}

void mem_cfg_info_print(void)
{
    volatile MEM_CFG_REG_DC_T *cfg0 = (volatile MEM_CFG_REG_DC_T *)MEM_CFG_REG_ADDR0;
    volatile MEM_CFG_REG_E0_T *cfg1 = (volatile MEM_CFG_REG_E0_T *)MEM_CFG_REG_ADDR1;

    TRACE(0, "cfg[%x] = %x", (uint32_t)cfg0, *(volatile uint32_t *)cfg0);
    TRACE(0, "cfg[%x] = %x", (uint32_t)cfg1, *(volatile uint32_t *)cfg1);
    TRACE(0, "cfg[%x]: blk00: [%d]: %s", (uint32_t)cfg0, cfg0->blk0,  mem_cfg_to_str(cfg0->blk0));
    TRACE(0, "cfg[%x]: blk01: [%d]: %s", (uint32_t)cfg0, cfg0->blk1,  mem_cfg_to_str(cfg0->blk1));
    TRACE(0, "cfg[%x]: blk02: [%d]: %s", (uint32_t)cfg0, cfg0->blk2,  mem_cfg_to_str(cfg0->blk2));
    TRACE(0, "cfg[%x]: blk03: [%d]: %s", (uint32_t)cfg0, cfg0->blk3,  mem_cfg_to_str(cfg0->blk3));
    TRACE(0, "cfg[%x]: blk04: [%d]: %s", (uint32_t)cfg0, cfg0->blk4,  mem_cfg_to_str(cfg0->blk4));
    TRACE(0, "cfg[%x]: blk05: [%d]: %s", (uint32_t)cfg0, cfg0->blk5,  mem_cfg_to_str(cfg0->blk5));
    TRACE(0, "cfg[%x]: blk06: [%d]: %s", (uint32_t)cfg0, cfg0->blk6,  mem_cfg_to_str(cfg0->blk6));
    TRACE(0, "cfg[%x]: blk07: [%d]: %s", (uint32_t)cfg0, cfg0->blk7,  mem_cfg_to_str(cfg0->blk7));
    TRACE(0, "cfg[%x]: blk08: [%d]: %s", (uint32_t)cfg0, cfg0->blk8,  mem_cfg_to_str(cfg0->blk8));
    TRACE(0, "cfg[%x]: blk09: [%d]: %s", (uint32_t)cfg0, cfg0->blk9,  mem_cfg_to_str(cfg0->blk9));
    TRACE(0, "cfg[%x]: blk10: [%d]: %s", (uint32_t)cfg1, cfg1->blk10, mem_cfg_to_str(cfg1->blk10));
    TRACE(0, "cfg[%x]: blk11: [%d]: %s", (uint32_t)cfg1, cfg1->blk11, mem_cfg_to_str(cfg1->blk11));
    TRACE(0, "cfg[%x]: blk12: [%d]: %s", (uint32_t)cfg1, cfg1->blk12, mem_cfg_to_str(cfg1->blk12));
    TRACE(0, "cfg[%x]: blk13: [%d]: %s", (uint32_t)cfg1, cfg1->blk13, mem_cfg_to_str(cfg1->blk13));
    TRACE(0, "cfg[%x]: blk14: [%d]: %s", (uint32_t)cfg1, cfg1->blk14, mem_cfg_to_str(cfg1->blk14));
    TRACE(0, "cfg[%x]: blk15: [%d]: %s", (uint32_t)cfg1, cfg1->blk15, mem_cfg_to_str(cfg1->blk15));
}
#endif

static int dspMemoryConfig_check(void){
    uint32_t blk0 , blk1;

    TRACE(0, "%s", __func__);

    blk0 = DSP_DIGITAL_REG(MEM_CFG_REG_ADDR0);
    blk1 = DSP_DIGITAL_REG(MEM_CFG_REG_ADDR1);

    TRACE_IMM(0, "MEM_CFG_REG[%X] = %08X", MEM_CFG_REG_ADDR0, blk0);
    TRACE_IMM(0, "MEM_CFG_REG[%X] = %08X", MEM_CFG_REG_ADDR1, blk1);
#ifdef MEM_CFG_INFO_PRINT
    mem_cfg_info_print();
#endif
    return 0;
}

static const struct dsp_memory_config_t dsp_memory_config_table[] = {
    {
        .memory_map = { .iram_start = 0x00800000,
                        .iram_end   = 0x00820000,
                        .dram_start = 0x20800000,
                        .dram_end   = 0x20840000,
                        .sram_start = 0x20220000,
                        .sram_end   = 0x203c0000,
                      },
        .memory_config_fn = dspMemoryConfig_i1d2s13,
    },
    {
        .memory_map = { .iram_start = 0x00800000,
                        .iram_end   = 0x00800000,
                        .dram_start = 0x20800000,
                        .dram_end   = 0x20800000,
                        .sram_start = 0x20200000,
                        .sram_end   = 0x20400000,
                      },
        .memory_config_fn = dspMemoryConfig_i0d0s16,
    },
    {
        .memory_map = { .iram_start = 0x00800000,
                        .iram_end   = 0x008C0000,
                        .dram_start = 0x20800000,
                        .dram_end   = 0x20940000,
                        .sram_start = 0,
                        .sram_end   = 0,
                      },
        .memory_config_fn = dspMemoryConfig_i6d10s0,
    },
    {
        .memory_map = { .iram_start = 0x00800000,
                        .iram_end   = 0x00860000,
                        .dram_start = 0x20800000,
                        .dram_end   = 0x208C0000,
                        .sram_start = 0,
                        .sram_end   = 0,
                      },
        .memory_config_fn = dspMemoryConfig_i3d6s0,
    },

};
//
static int POSSIBLY_UNUSED dsp_memory_map_config(struct dsp_memory_map_t *memory_map)
{
    int nRet = -1;
    struct dsp_memory_map_t *map_tbl = NULL;

    for (uint32_t i = 0; i < ARRAY_SIZE(dsp_memory_config_table); i++){
        map_tbl = (struct dsp_memory_map_t *)&dsp_memory_config_table[i].memory_map;
        if( map_tbl->iram_start == memory_map->iram_start
                && map_tbl->iram_end == memory_map->iram_end
                && map_tbl->dram_start == memory_map->dram_start
                && map_tbl->dram_end == memory_map->dram_end
                && map_tbl->sram_start == memory_map->sram_start
                && map_tbl->sram_end == memory_map->sram_end) {
            TRACE(0, "%s match", __func__);
            if (dsp_memory_config_table[i].memory_config_fn){
                nRet = dsp_memory_config_table[i].memory_config_fn();
            }else{
                nRet = 0;
            }
            break;
        }
    }
    return nRet;
}

static void dsp_wdt_irq_handler(void)
{
    TRACE(0, "%s", __func__);
    NVIC_ClearPendingIRQ(SYS_WDT_IRQn);
    NVIC_DisableIRQ(SYS_WDT_IRQn);
}

static void dsp_wdt_start(void)
{
    NVIC_SetVector(SYS_WDT_IRQn, (uint32_t)dsp_wdt_irq_handler);
    NVIC_SetPriority(SYS_WDT_IRQn, IRQ_PRIORITY_REALTIME);
    NVIC_ClearPendingIRQ(SYS_WDT_IRQn);
    NVIC_EnableIRQ(SYS_WDT_IRQn);;
}

static int dsp_ipc_cfg_get(struct HAL_RMT_IPC_CFG_T *cfg)
{
#ifdef BTH_AS_MAIN_MCU
    return hal_sys2bth_config_get(cfg);
#else
    return hal_mcu2dsp_config_get(cfg);
#endif
}

static int dsp_ipc_cfg_set(const struct HAL_RMT_IPC_CFG_T *cfg)
{
#ifdef BTH_AS_MAIN_MCU
    return hal_sys2bth_config_set(cfg);
#else
    return hal_mcu2dsp_config_set(cfg);
#endif
}

#if !defined(BTH_AS_MAIN_MCU) && defined(DSP_CORE_DEEP_SLEEP)
static enum HAL_CMU_FREQ_T dsp_freq;
static uint32_t dsp_irq_mask[3];

void dsp_sysfreq_req(enum HAL_CMU_FREQ_T freq)
{
    dsp_freq = freq;
    hal_sysfreq_req(HAL_SYSFREQ_USER_DSP, freq);
}

static void dsp_allow_wakeup(void)
{
#if defined(CHIP_BEST1600) || defined(CHIP_BEST1603)
    hal_cmu_hifi4_wakeup_allow_signal_set();
#else
    hal_cmu_hifi4_wakeup_allow();
#endif
    dsp_sysfreq_req(HAL_CMU_FREQ_26M);
}

static void dsp_wakeup_handler(void)
{
#if defined(CHIP_BEST1600) || defined(CHIP_BEST1603)
    uint32_t i, j;
    IRQn_Type irq;

    if (dsp_freq != HAL_CMU_FREQ_32K) {
        return;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 32; j++) {
            if (dsp_irq_mask[i] & (1 << j)) {
                irq = i * 32 + j;
                NVIC_DisableIRQ(irq);
            }
        }
    }
#else
    IRQn_Type irq;

    irq = DSPC0_IRQn;
    NVIC_DisableIRQ(irq);
#endif

    dsp_allow_wakeup();
}

static void dsp_sleep_handler(void)
{
#if (defined(CHIP_BEST1600) || defined(CHIP_BEST1603))
    uint32_t i, j;
    uint32_t mask[3];
    IRQn_Type irq;

    // Clearing irq
    hal_cmu_hifi4_sleep_signal_clear();
    hal_cmu_hifi4_wakeup_allow_signal_clear();

    // Check msg sending state
    if (hal_mcu2dsp_tx_active(HAL_MCU2DSP_ID_0, HAL_RMT_IPC_ALL_SEND_SEQ)) {
        dsp_allow_wakeup();
        return;
    }

    dsp_sysfreq_req(HAL_CMU_FREQ_32K);

    // Get wakeup irq mask
    hal_cmu_hifi4_wakeup_mask_get(mask);
    dsp_irq_mask[0] = mask[0];
    dsp_irq_mask[1] = mask[1];
    dsp_irq_mask[2] = mask[2];
    dsp_irq_mask[0] &= ~((1 << 24) | (1 << 25));
    dsp_irq_mask[0] &= ~((1 << 30) | (1 << 31));
    dsp_irq_mask[2] &= ~(1 << (69 - 64));
    dsp_irq_mask[2] &= ~(1 << (70 - 64));
    // AON timer 10/11 irq
    if (mask[0] & ((1 << 24) | (1 << 25))) {
        dsp_irq_mask[2] |= (1 << (69 - 64));
    }
    // SYS timer 30/31 irq
    if (mask[0] & ((1 << 30) | (1 << 31))) {
        dsp_irq_mask[2] |= (1 << (70 - 64));
    }
    // AON timer 00/01 irq
    if (mask[2] & (1 << (69 - 64))) {
        dsp_irq_mask[0] |= ((1 << 30) | (1 << 31));
    }
    // SYS timer 00/01 irq
    if (mask[2] & (1 << (70 - 64))) {
        dsp_irq_mask[0] |= ((1 << 24) | (1 << 25));
    }

    // Register wakeup handler
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 32; j++) {
            if (dsp_irq_mask[i] & (1 << j)) {
                irq = i * 32 + j;
                if (NVIC_GetEnableIRQ(irq)) {
                    dsp_irq_mask[i] &= ~(1 << j);
                    continue;
                }
                NVIC_SetVector(irq, (uint32_t)dsp_wakeup_handler);
                NVIC_SetPriority(irq, IRQ_PRIORITY_NORMAL);
                NVIC_ClearPendingIRQ(irq);
                NVIC_EnableIRQ(irq);
            }
        }
    }
#else
    IRQn_Type irq;

    hal_cmu_hifi4_sleep_signal_clear();

    if (hal_cmu_hifi4_in_sleep()) {
        dsp_sysfreq_req(HAL_CMU_FREQ_32K);

        irq = DSPC0_IRQn;
        NVIC_SetVector(irq, (uint32_t)dsp_wakeup_handler);
        NVIC_SetPriority(irq, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(irq);
        NVIC_EnableIRQ(irq);
    }
#endif
}

#if (defined(CHIP_BEST1600) || defined(CHIP_BEST1603))
static void mcu2dsp_send_callback(enum HAL_MCU2DSP_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    if (dsp_freq == HAL_CMU_FREQ_32K) {
        dsp_wakeup_handler();
    }
    int_unlock(lock);
}
#endif
#endif /* !BTH_AS_MAIN_MCU && DSP_CORE_DEEP_SLEEP */

static int POSSIBLY_UNUSED dsp_load_code(uint32_t load_base)
{
    uint32_t *dst, *src, *end;
    struct dsp_binary_struct_t * dsp_boot_struct;
    struct dsp_segments_info_t *segments_loader = NULL;

    dsp_boot_struct = (struct dsp_binary_struct_t *)load_base;
    segments_loader = dsp_boot_struct->segments_info;
    while(segments_loader->start_addr
            && segments_loader->end_addr
            && segments_loader->store_addr) {

        TRACE(0, "start_addr:%08x end_addr:%08x store_addr:%08x @:%08x",
                segments_loader->start_addr,
                segments_loader->end_addr,
                segments_loader->store_addr,
                load_base + (segments_loader->store_addr - DSP_LOGIC_SROM_BASE));

        if (segments_loader->start_addr < segments_loader->end_addr) {
            dst = (uint32_t *)segments_loader->start_addr;
            end = (uint32_t *)segments_loader->end_addr;
            src = (uint32_t *)(load_base + (segments_loader->store_addr -
                                                    DSP_LOGIC_SROM_BASE));
            for (; dst < end; dst++, src++) {
                *dst = *src;
                if (*dst != *src){
                    TRACE(0, "%p != %p %08x/%08x", dst, src, *dst, *src);
                }
            }
        }
        segments_loader++;
    }

    return 0;
}

int dsp_check_and_startup(uint32_t load_base, uint32_t load_size)
{
    struct dsp_binary_struct_t * dsp_boot_struct;
    struct HAL_RMT_IPC_CFG_T ipc_cfg;
    uint32_t ipc_mailbox_start = 0;

    dsp_boot_struct = (struct dsp_binary_struct_t *)load_base;
    TRACE(0, "%s base:%p size:0x%08x", __func__, dsp_boot_struct, load_size);
    TRACE(0, "binary_struct ver:%04x ", dsp_boot_struct->hdr.version);
    TRACE(0, "iram:%08x~%08x dram:%08x~%08x sram:%08x~%08x",
                            dsp_boot_struct->memory_map.iram_start,
                            dsp_boot_struct->memory_map.iram_end,
                            dsp_boot_struct->memory_map.dram_start,
                            dsp_boot_struct->memory_map.dram_end,
                            dsp_boot_struct->memory_map.sram_start,
                            dsp_boot_struct->memory_map.sram_end);

    ASSERT(dsp_boot_struct->memory_map.iram_start == HIFI4_ITCM_BASE, \
            "%s: Bad itcm s-addr=0x%X", __func__, HIFI4_ITCM_BASE);
    ASSERT(dsp_boot_struct->memory_map.iram_end == (HIFI4_ITCM_BASE + HIFI4_ITCM_SIZE), \
            "%s: Bad itcm e-addr=0x%X", __func__, (HIFI4_ITCM_BASE + HIFI4_ITCM_SIZE));

    ASSERT(dsp_boot_struct->memory_map.dram_start == HIFI4_DTCM_BASE, \
            "%s: Bad dtcm s-addr=0x%X", __func__, HIFI4_DTCM_BASE);
    ASSERT(dsp_boot_struct->memory_map.dram_end == (HIFI4_DTCM_BASE + HIFI4_DTCM_SIZE), \
            "%s: Bad dtcm e-addr=0x%X", __func__, (HIFI4_DTCM_BASE + HIFI4_DTCM_SIZE));

#ifdef HIFI4_SRAM_CHECK
    ASSERT(dsp_boot_struct->memory_map.sram_start == HIFI4_SYS_RAM_BASE, \
            "%s: Bad sram s-addr=0x%X", __func__, HIFI4_SYS_RAM_BASE);
    ASSERT(dsp_boot_struct->memory_map.sram_end == (HIFI4_SYS_RAM_BASE + HIFI4_SYS_RAM_SIZE), \
            "%s: Bad sram e-addr=0x%X", __func__, (HIFI4_SYS_RAM_BASE + HIFI4_SYS_RAM_SIZE));
#endif

    hal_psc_sys_dsp_enable();
    hal_cmu_dsp_clock_enable();
    hal_cmu_dsp_reset_clear();
    hal_cmu_dsp_cpu_reset_clear(dsp_boot_struct->memory_map.iram_start);
    osDelay(2);
#ifdef DSP_BOOT_FORM_FLASH
    dspMemoryConfig_i1d2s13();
#else
/*
    if (dsp_memory_map_config(&dsp_boot_struct->memory_map)){
        TRACE(0, "%s map_config failed", __func__);
        return -2;
    }
*/

    hal_cmu_sys_dsp_tcm_cfg(true);
    dspMemoryConfig_check();
    dsp_load_code(load_base);
#endif

#if defined(DSP_HIFI4_TRC_TO_MCU) && defined(RMT_TRC_IN_MSG_CHAN)
    ipc_mailbox_start = dsp_boot_struct->memory_map.dram_end - HIFI4_MAILBOX_SIZE + sizeof(struct HAL_RMT_IPC_CFG_T *) * 2;
#else
    ipc_mailbox_start = dsp_boot_struct->memory_map.dram_end - HIFI4_MAILBOX_SIZE;
#endif

    DSP_DIGITAL_REG(ipc_mailbox_start) = dsp_boot_struct->memory_map.dram_end;
    dsp_ipc_cfg_get(&ipc_cfg);
    ipc_cfg.peer_cfg_pp = (const struct HAL_RMT_IPC_CFG_T **)(ipc_mailbox_start);
    ipc_cfg.local_cfg_pp = (const struct HAL_RMT_IPC_CFG_T **)(ipc_mailbox_start+ sizeof(struct HAL_RMT_IPC_CFG_T *));
    dsp_ipc_cfg_set(&ipc_cfg);


#ifndef BTH_AS_MAIN_MCU
    // secure mode build error, do nothing
    /*
    dsp_sysfreq_req(HAL_CMU_FREQ_26M);

    hal_cmu_hifi4_sleep_signal_clear();
    hal_cmu_hifi4_wakeup_allow_signal_clear();

    hal_mcu2dsp_register_send_callback(mcu2dsp_send_callback);

    NVIC_SetVector(DSP2MCU_DATA2_IRQn, (uint32_t)dsp_sleep_handler);
    NVIC_SetPriority(DSP2MCU_DATA2_IRQn, IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(DSP2MCU_DATA2_IRQn);
    NVIC_EnableIRQ(DSP2MCU_DATA2_IRQn);
    */
#endif

    hal_cmu_dsp_start_cpu();

    dsp_wdt_start();

#ifdef DSP_DEBUG_PORT_MAPOUT
    hal_iomux_set_sys_uart0();
#endif
#ifdef DSP_HIFI4_TRC_TO_MCU
    dsp_hifi4_trace_server_open();
#endif

    return 0;
}

#ifdef DSP_COMBINE_BIN
int dsp_combine_bin_startup(void)
{
    dsp_check_and_startup((uint32_t)__dsp_hifi4_code_start_flash&(~0x04000000), (uint32_t)(__dsp_hifi4_code_end_flash-__dsp_hifi4_code_end_flash));
    return 0;
}
#endif

int dsp_shutdown(void)
{
#ifdef DSP_HIFI4_TRC_TO_MCU
    dsp_hifi4_trace_server_close();
#endif

    hal_cmu_dsp_reset_set();
    hal_cmu_dsp_clock_disable();
    hal_psc_sys_dsp_disable();

    return 0;
}

static bool dsp_hifi4_inited = false;
int dsp_open(void)
{
    if(dsp_hifi4_inited != false)
        return 0;
#ifdef DSP_COMBINE_BIN
    dsp_combine_bin_startup();
#else
    dsp_check_and_startup(DSP_LOGIC_SROM_BASE, 0);
#endif

#ifdef APP_RPC_ENABLE
    app_rpc_core_open(APP_RPC_CORE_BTH_DSP);
    app_rpc_ctx_init(APP_RPC_CORE_BTH_DSP);
#endif

    dsp_hifi4_inited = true;
    TRACE_IMM(0, "%s, %d", __func__, __LINE__);

    return 0;
}

int dsp_close(void)
{
    if (!dsp_hifi4_inited) {
        return 0;
    }
#ifdef DSP_HIFI4
    dsp_shutdown();
#endif
    dsp_hifi4_inited = false;
    return 0;
}

#endif
