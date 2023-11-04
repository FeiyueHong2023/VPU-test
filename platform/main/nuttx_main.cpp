/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#define main(...) _sdk_main(...)
#include "main.cpp"
#include "nuttx/init.h"
#include "hal_cmu.h"
#include "hal_iomux.h"
#include "hal_sdmmc.h"
#if defined(CONFIG_ARCH_TRUSTZONE_SECURE)
#include <arm_cmse.h>
#include "partition_ARMCM33.h"
#include "mpc.h"
#endif

#ifdef __cplusplus
#define EXTERN_C                        extern "C"
#else
#define EXTERN_C                        extern
#endif

EXTERN_C void btdrv_start_bt(void);
EXTERN_C int platform_trace_enable();
extern "C" BOOT_BSS_LOC volatile bool btdrv_init_ok = false;
extern "C" BOOT_BSS_LOC volatile bool wifidrv_init_ok = false;

static BOOT_BSS_LOC bool  is_chip_init_early_done = false;
static void trace_enable_early()
{
#if (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
#endif
#if (DEBUG_PORT == 2)
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#endif
#if (DEBUG_PORT == 3)
    hal_iomux_set_uart2();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART2);
#endif
}

EXTERN_C void bes_chip_flash_init();
EXTERN_C void bes_chip_init_early()
{
    if (is_chip_init_early_done)
        return;
    tgt_hardware_setup();
#if defined(ROM_UTILS_ON)
    rom_utils_init();
#endif
#if !defined(CONFIG_USE_SLOW_TIMER) || defined(CONFIG_BES_ONESHOT_SYSTICK)
    hwtimer_init();
#endif

    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);
    hal_audma_open();
    hal_gpdma_open();
#if defined(CONFIG_HIFI4_USE_AUDMA)
    //hal_dma_close_audma_inst();
#endif
    srand(hal_sys_timer_get());
#if defined(CHIP_SUBSYS_SENS) && defined(SENS_TRC_TO_MCU)
    extern void sens_trace_to_mcu();
    sens_trace_to_mcu();
#ifndef CONFIG_BES_LPUART
    hal_trace_open(HAL_TRACE_TRANSPORT_NULL);
#endif
    hal_trace_set_log_level(TR_LEVEL_VERBOSE);
    hal_trace_enable_log_module(0);
#endif
    is_chip_init_early_done = true;
#if defined(CONFIG_ARCH_TRUSTZONE_SECURE)
    int ret = mpc_init();
    ASSERT(ret==0, "mpc init fail. ret:%d", ret);
    TZ_SAU_Setup();
    __ISB();
    struct HAL_TRNG_CFG_T trng_cfg = {0x100, HAL_TRNG_RND_SRC_SHORTEST};
    hal_trng_open(&trng_cfg);
#endif
#if (defined(PSRAM_ENABLE) || defined(PSRAMUHS_ENABLE)) && defined(PSRAM_INIT_CHECK)
    pmu_wdt_stop();
#endif
}

EXTERN_C void nuttx_norflash_get_size(void);
EXTERN_C int factory_section_open(void);
EXTERN_C uint8_t* factory_section_get_bt_address(void);
#if defined(CONFIG_BES_HAVE_MTDSDMMC)
EXTERN_C void nuttx_sdmmc_get_size(void);
#endif

EXTERN_C void bes_chip_init_later()
{
    main_thread_tid = osThreadGetId();
    hal_iomux_ispi_access_init();
#if defined(CONFIG_MTD)
    uint8_t flash_id[HAL_NORFLASH_DEVICE_ID_LEN];
    hal_norflash_get_id(HAL_FLASH_ID_0, flash_id, ARRAY_SIZE(flash_id));
    TR_INFO(TR_MOD(MAIN), "FLASH_ID: %02X-%02X-%02X", flash_id[0], flash_id[1], flash_id[2]);
    hal_norflash_show_calib_result(HAL_FLASH_ID_0);
    ASSERT(hal_norflash_opened(HAL_FLASH_ID_0), "Failed to init flash: %d", hal_norflash_get_open_state(HAL_FLASH_ID_0));

    // Software will load the factory data and user data from the bottom TWO sectors from the flash,
    // the FLASH_SIZE defined is the common.mk must be equal or greater than the actual chip flash size,
    // otherwise the ota will load the wrong information
    uint32_t actualFlashSize = hal_norflash_get_flash_total_size(HAL_FLASH_ID_0);
    if (FLASH_SIZE > actualFlashSize)
    {
        TRACE_IMM(0,"Wrong FLASH_SIZE defined in target.mk!");
        TRACE_IMM(2,"FLASH_SIZE is defined as 0x%x while the actual chip flash size is 0x%x!", FLASH_SIZE, actualFlashSize);
        TRACE_IMM(1,"Please change the FLASH_SIZE in common.mk to 0x%x to enable the OTA feature.", actualFlashSize);
        ASSERT(false, " ");
    }
#endif
#if !defined(CONFIG_ARCH_TRUSTZONE_SECURE)
    //mpu_cfg();
    pmu_open();
    analog_open();
    srand(hal_sys_timer_get());
#endif
#if defined(CONFIG_MTD)
    norflash_api_init();
    nuttx_norflash_get_size();
#if defined(CONFIG_BES_HAVE_MTDSDMMC)
    nuttx_sdmmc_get_size();
    //sdmmc_test();
#endif
#endif
#if !defined(CONFIG_USE_BES_BT_STACK) && (defined(CONFIG_BT) || defined(CONFIG_LIB_FLUORIDE) || defined(CONFIG_BTSAK))
    factory_section_open();
    uint8_t * btaddr = NULL;
    uint8_t zero_addr[6] = {0};
    btaddr = factory_section_get_bt_address();
    if(btaddr){
        if(memcmp(btaddr, zero_addr, 6)){
            memcpy(bt_global_addr, btaddr, 6);
        }
    }
#if !defined(__BOOT_SELECT)
    btdrv_start_bt();
#endif
    btdrv_init_ok = true;
#elif !defined(CONFIG_USE_BES_BT_STACK)
    btdrv_init_ok = true;
#endif
}

EXTERN_C int hal_uart_printf_init(void);
EXTERN_C int hal_uart_printf(const char *fmt, ...);
EXTERN_C int bes_wl_initialize(void);

extern volatile int trace_enable_flag;
#include <nuttx/syslog/syslog_rpmsg.h>
#ifdef CONFIG_SYSLOG_RPMSG
static char g_logbuffer[4096];
#endif
EXTERN_C int _start(void)
{
#ifdef CONFIG_BES_LPUART
    hal_uart_printf_init();
    hal_uart_printf("system power on....");
#endif
#ifdef CONFIG_JTAG_ENABLE
    hal_iomux_set_jtag();
    hal_cmu_jtag_clock_enable();
#endif
#if defined(CONFIG_SYSLOG_RPMSG)
    syslog_rpmsg_init_early(g_logbuffer, sizeof(g_logbuffer));
#endif
    nx_start();
    return 0;
}
EXTERN_C void app_trace_rx_open(void);
EXTERN_C void a7_dsp_boot(void);
EXTERN_C void cp_boot(void);

EXTERN_C int bes_main(void)
{
    uint8_t sys_case = 0;
    bes_chip_init_early();
    bes_chip_init_later();
    hal_sleep_start_stats(5000, 5000);
#if !defined(CONFIG_BES_HAVE_DVFS)
#if defined(CONFIG_ARCH_CHIP_BES2003)
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_390M);
#else
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_52M);
#endif
#endif
#if defined(CONFIG_USE_BES_BT_STACK) || defined(__WIFI_APP_SUPPORT__)
    int ret = app_init();
    wifidrv_init_ok = true;
#else
    int ret =0;
#if !defined(CONFIG_BES_RPTUN_DELAY_BOOT)
    #if defined(CONFIG_A7_DSP_ENABLE)
     a7_dsp_boot();
    #endif
    #if defined(CONFIG_BES_DUALCORE_AMP)
     cp_boot();
    #endif
#endif
#endif
#if defined(CONFIG_BES_AUDIO_DUMP)
    app_trace_rx_open();
#endif
#if defined(__WIFI_APP_SUPPORT__)
    bes_wl_initialize();
#endif
#if defined(CONFIG_USE_BES_BT_STACK)
#if defined(USE_BT_ADAPTER)
    extern "C" void bes_bt_init(void);
    bes_bt_init();
#endif
    btdrv_init_ok = true;
#endif
    if (!ret)
    {
        while(1)
        {
            osEvent evt;
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
            osSignalClear (main_thread_tid, 0x0f);
#endif
            //wait any signal
            evt = osSignalWait(0x0, osWaitForever);

            //get role from signal value
            if(evt.status == osEventSignal)
            {
                if(evt.value.signals & 0x04)
                {
                    sys_case = 1;
                    break;
                }
                else if(evt.value.signals & 0x08)
                {
                    sys_case = 2;
                    break;
                }
            }else{
                sys_case = 1;
                break;
            }
         }
    }
#if defined(CONFIG_USE_BES_BT_STACK) || defined(__WIFI_APP_SUPPORT__)
    system_shutdown_wdt_config(10);
    app_deinit(ret);
#endif
    TR_INFO(TR_MOD(MAIN), "byebye~~~ %d\n", sys_case);
    if ((sys_case == 1)||(sys_case == 0)){
        TR_INFO(TR_MOD(MAIN), "shutdown\n");
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
        pmu_shutdown();
    }else if (sys_case == 2){
        TR_INFO(TR_MOD(MAIN), "reset\n");
        pmu_reboot();
    }    
}

osThreadDef(bes_main, (osPriorityAboveNormal), 1, (4096), "bes_main");

EXTERN_C __attribute__((weak)) void bes_app_initialize()
{
    osThreadCreate(osThread(bes_main), NULL);
}

#if __BOOT_SELECT
typedef void (*FLASH_ENTRY)(void);
typedef struct
{
    uint32_t magicNumber;
    uint32_t imageSize;
    uint32_t imageCrc;
}FLASH_OTA_BOOT_INFO_T;

#define BOOT_MAGIC_NUMBER       0xBE57EC1C
#define NORMAL_BOOT             BOOT_MAGIC_NUMBER
#define COPY_NEW_IMAGE          0x5a5a5a5a
EXTERN_C void NVIC_DisableAllIRQs(void);

FLASH_OTA_BOOT_INFO_T otaBootInfoInFlash __attribute((section(".ota_boot_info"))) = { NORMAL_BOOT, 0, 0} ;

#define EXEC_CODE_BASE (FLASH_BASE+__APP_IMAGE_FLASH_OFFSET__)
static const struct boot_struct_t * boot_struct_app =
    (const struct boot_struct_t *)EXEC_CODE_BASE;

static int app_is_otaMode(void)
{
    uint32_t bootmode = hal_sw_bootmode_get();

    return (bootmode & HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
}

EXTERN_C void boot_loader_entry_hook(void) __attribute__((alias("BootSelect")));

EXTERN_C void BootSelect(void)
{
	FLASH_ENTRY entry;

    if (app_is_otaMode())
        return;

    // Workaround for reboot: controller in standard SPI mode while FLASH in QUAD mode
    // First read will fail when FLASH in QUAD mode, but it will make FLASH roll back to standard SPI mode
    // Second read will succeed

    volatile uint32_t* ota_magic;

    ota_magic = (volatile uint32_t *)&otaBootInfoInFlash;

    // First read (and also flush the controller prefetch buffer)
    *(ota_magic + 0x400);
    if ((NORMAL_BOOT == *ota_magic) && (NORMAL_BOOT == boot_struct_app->hdr.magic))
    {
        // Disable all IRQs
        NVIC_DisableAllIRQs();
        // Ensure in thumb state

#ifdef SECURE_BOOT
        entry = (FLASH_ENTRY)(&secure_boot_struct_app->code_ver_struct + 1);
#else
        entry = (FLASH_ENTRY)(&boot_struct_app->hdr + 1);
#endif

        entry = (FLASH_ENTRY)((uint32_t)entry | 1);
        entry();
    }
}

#if defined(CONFIG_ARCH_TRUSTZONE_SECURE)
#define NS_CALL                             __attribute__((cmse_nonsecure_call))
/* typedef for non-secure callback functions */
typedef void (*funcptr_void) (void) NS_CALL;

EXTERN_C void bes_call_nonse(void)
{
    funcptr_void NonSecure_ResetHandler;
    uint32_t ns_app_start_addr = (FLASHX_BASE + NS_APP_START_OFFSET);

    /* Add user setup code for secure part here*/

    /* Set non-secure main stack (MSP_NS) */
    //__TZ_set_MSP_NS(*((uint32_t *)(ns_app_start_addr)));

    /* Get non-secure reset handler */
#ifdef SECURE_BOOT
    NonSecure_ResetHandler = (funcptr_void)((uint32_t)ns_app_start_addr + sizeof(struct boot_struct_t) + sizeof(struct code_sig_struct_t) +
                                            sizeof(struct norflash_cfg_struct_t) + sizeof(struct code_ver_struct_t));
#else
    NonSecure_ResetHandler = (funcptr_void)((uint32_t)(&((struct boot_struct_t *)ns_app_start_addr)->hdr + 1));
#endif
    if (0){//*(uint32_t *)ns_app_start_addr != BOOT_MAGIC_NUMBER) {
        TRACE(0, "nonsec image(0x%08x) magic(0x%08x) error", ns_app_start_addr, *(uint32_t *)ns_app_start_addr);
    } else {
        TRACE(0, "Call nonsec App. start addr:0x%08x", ns_app_start_addr);
        /* Start non-secure state software application */
#if 0//def DEBUG
        cmse_set_ns_start_flag(true);
#endif
        NonSecure_ResetHandler();
    }
}
#endif

#endif

