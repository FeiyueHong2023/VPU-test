/***************************************************************************
 *
 * Copyright 2020-2022 BES.
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
#ifndef __PLAT_ADDR_MAP_SYS_BEST1603_H__
#define __PLAT_ADDR_MAP_SYS_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAIN_RAM_USE_SYS_RAM
#define MAIN_RAM_USE_TCM
#endif

#define RAM_SECURITY_FLAG                       0x04000000

#ifdef MAIN_RAM_USE_TCM
#define RAM0_BASE                               M55_DTCM_BASE
#define RAM0_SIZE                               M55_DTCM_SIZE

#define RAMX0_BASE                              RAMX_BASE
#define RAMX0_SIZE                              RAMX_SIZE

#define RAM_TOTAL_SIZE                          (RAM0_BASE + RAM0_SIZE - RAM0_BASE)

#define RAM_NORM_BASE                           M55_DTCM_BASE
#ifdef BTH_AS_MAIN_MCU
#define RAM_NORM_SIZE                           (M55_MAILBOX_BASE - RAM_NORM_BASE)
#else
#define RAM_NORM_SIZE                           M55_DTCM_SIZE
#endif
#define RAMX_NORM_BASE                          M55_ITCM_BASE
#define RAMX_NORM_SIZE                          M55_ITCM_SIZE

#define EXTRA_RAM0_NORM_BASE                    M55_SYS_RAM_BASE
#define EXTRA_RAM0_NORM_SIZE                    M55_SYS_RAM_SIZE

#else // !MAIN_RAM_USE_TCM
#define RAM0_BASE                               M55_SYS_RAM_BASE
#define RAM0_SIZE                               M55_SYS_RAM_SIZE

#define RAM_TOTAL_SIZE                          (RAM0_BASE + RAM0_SIZE - RAM0_BASE)

#define RAM_NC_BASE                             (RAM_NORM_BASE + RAM_NORM_SIZE)
#define RAM_NC_TOTAL_SIZE                       0x00004000
#ifdef BTH_AS_MAIN_MCU
#define RAM_NC_SIZE                             (RAM_NC_TOTAL_SIZE - M55_MAILBOX_SIZE)
#else
#define RAM_NC_SIZE                             RAM_NC_TOTAL_SIZE
#endif

#define RAM_NORM_BASE                           M55_SYS_RAM_BASE
#define RAM_NORM_SIZE                           (M55_SYS_RAM_SIZE - RAM_NC_TOTAL_SIZE)
#define RAMX_NORM_BASE                          RAM_NORM_BASE
#define RAMX_NORM_SIZE                          RAM_NORM_SIZE

#define EXTRA_RAM0_NORM_BASE                    M55_ITCM_BASE
#define EXTRA_RAM0_NORM_SIZE                    M55_ITCM_SIZE

#define EXTRA_RAM1_NORM_BASE                    M55_DTCM_BASE
#define EXTRA_RAM1_NORM_SIZE                    M55_DTCM_SIZE

#endif // !MAIN_RAM_USE_TCM

#if defined(ARM_CMSE) || defined(ARM_CMNS)
#define RAM_S_BASE                              RAM_NS_TO_S(RAM_NORM_BASE)
#define RAM_S_SIZE                              0x00010000
#define RAMX_S_BASE                             RAM_NS_TO_S(RAMX_NORM_BASE)
#define RAMX_S_SIZE                             0x00010000

#define RAM_NS_BASE                             RAM_S_TO_NS(RAM_NORM_BASE + RAM_S_SIZE)
#define RAM_NS_SIZE                             (RAM_NORM_SIZE - RAM_S_SIZE)
#define RAMX_NS_BASE                            RAM_S_TO_NS(RAMX_NORM_BASE + RAMX_S_SIZE)
#define RAMX_NS_SIZE                            (RAMX_NORM_SIZE - RAMX_S_SIZE)

#ifndef SYS_RAM0_NS_BASE
#define SYS_RAM0_NS_BASE                        SYS_RAM0_BASE
#endif
#ifndef SYS_RAM0_NS_SIZE
#define SYS_RAM0_NS_SIZE                        (SYS_RAM1_BASE - SYS_RAM0_BASE)
#endif
#ifndef SYS_RAM1_NS_BASE
#define SYS_RAM1_NS_BASE                        SYS_RAM1_BASE
#endif
#ifndef SYS_RAM1_NS_SIZE
#define SYS_RAM1_NS_SIZE                        (SYS_RAM2_BASE - SYS_RAM1_BASE)
#endif
#ifndef SYS_RAM2_NS_BASE
#define SYS_RAM2_NS_BASE                        SYS_RAM2_BASE
#endif
#ifndef SYS_RAM2_NS_SIZE
#define SYS_RAM2_NS_SIZE                        (SYS_RAM3_BASE - SYS_RAM2_BASE)
#endif
#ifndef SYS_RAM3_NS_BASE
#define SYS_RAM3_NS_BASE                        SYS_RAM3_BASE
#endif
#ifndef SYS_RAM3_NS_SIZE
#define SYS_RAM3_NS_SIZE                        (SYS_RAM_END - SYS_RAM0_BASE)
#endif

#define SYS_PSRAM_S_BASE                        SYS_PSRAM_BASE
#define SYS_PSRAM_NC_S_BASE                     SYS_PSRAM_NC_BASE
#ifndef SYS_PSRAM_S_SIZE
#define SYS_PSRAM_S_SIZE                        0
#endif
#define SYS_PSRAM_NS_BASE                       (SYS_PSRAM_BASE + SYS_PSRAM_S_SIZE)
#define SYS_PSRAM_NC_NS_BASE                    (SYS_PSRAM_NC_BASE + SYS_PSRAM_S_SIZE)
#define SYS_PSRAM_NS_SIZE                       (PSRAM_SIZE - SYS_PSRAM_S_SIZE)

#ifndef FLASH_S_SIZE
#define FLASH_S_SIZE                            0x00040000
#endif

#if ((RAM_S_SIZE) & (0x10000-1))
#error "RAM_S_SIZE should be 0x10000 aligned"
#endif
#if ((RAMX_S_SIZE) & (0x10000-1))
#error "RAMX_S_SIZE should be 0x10000 aligned"
#endif
#if ((SYS_PSRAM_S_SIZE) & (0x40000-1))
#error "SYS_PSRAM_S_SIZE should be 0x40000 aligned"
#endif
#if (FLASH_S_SIZE & (0x40000-1))
#error "FLASH_S_SIZE should be 0x40000 aligned"
#endif

#if defined(ARM_CMSE)
#ifndef NS_APP_START_OFFSET
#define NS_APP_START_OFFSET                     (FLASH_S_SIZE)
#endif
#ifndef FLASH_REGION_SIZE
#define FLASH_REGION_SIZE                       FLASH_S_SIZE
#endif
#endif /* ARM_CMSE */
#else // ARM_CMSE || ARM_CMNS
#define RAM_S_BASE                              RAM_NS_TO_S(RAM_NORM_BASE)
#define RAM_S_SIZE                              RAM_NORM_SIZE
#define RAMX_S_BASE                             RAM_NS_TO_S(RAMX_NORM_BASE)
#define RAMX_S_SIZE                             RAMX_NORM_SIZE
#define SYS_PSRAM_S_BASE                        SYS_PSRAM_BASE
#define SYS_PSRAM_NC_S_BASE                     SYS_PSRAM_NC_BASE

#define RAM_NS_BASE                             RAM_S_TO_NS(RAM_NORM_BASE)
#define RAM_NS_SIZE                             RAM_NORM_SIZE
#define RAMX_NS_BASE                            RAM_S_TO_NS(RAMX_NORM_BASE)
#define RAMX_NS_SIZE                            RAMX_NORM_SIZE
#define SYS_PSRAM_NS_BASE                       SYS_PSRAM_BASE
#define SYS_PSRAM_NC_NS_BASE                    SYS_PSRAM_NC_BASE
#endif // ARM_CMSE || ARM_CMNS

#ifdef PROGRAMMER_LOAD_RAM
#ifndef MAIN_RAM_USE_TCM
#error "PROGRAMMER_LOAD_RAM requires MAIN_RAM_USE_TCM"
#endif
#define RAM_BASE                                M55_SYS_RAM_BASE
#define RAM_SIZE                                M55_SYS_RAM_SIZE
#define RAMX_BASE                               RAM_BASE
#define RAMX_SIZE                               RAM_SIZE
#define LOAD_RAM_BASE                           RAM_NS_TO_S(M55_ITCM_BASE)
#define LOAD_RAM_SIZE                           M55_ITCM_SIZE
#define PSRAM_BASE                              SYS_PSRAM_S_BASE
#define PSRAM_NC_BASE                           SYS_PSRAM_NC_S_BASE
#define PSRAMX_BASE                             SYS_PSRAM_S_BASE
#define PSRAMX_NC_BASE                          SYS_PSRAM_NC_S_BASE
#define PSRAM_REGION_SIZE                       SYS_PSRAM_S_SIZE
#elif defined(NO_TRUSTZONE) || defined(ARM_CMNS) || \
        !(defined(ARM_CMSE) || defined(ROM_BUILD) || defined(PROGRAMMER))
#define RAM_BASE                                RAM_NS_BASE
#define RAM_SIZE                                RAM_NS_SIZE
#define RAMX_BASE                               RAMX_NS_BASE
#define RAMX_SIZE                               RAMX_NS_SIZE
#define PSRAM_BASE                              SYS_PSRAM_NS_BASE
#define PSRAM_NC_BASE                           SYS_PSRAM_NC_NS_BASE
#define PSRAMX_BASE                             SYS_PSRAM_NS_BASE
#define PSRAMX_NC_BASE                          SYS_PSRAM_NC_NS_BASE
#ifdef SYS_PSRAM_NS_SIZE
#define PSRAM_REGION_SIZE                       SYS_PSRAM_NS_SIZE
#endif
#else
#define RAM_BASE                                RAM_S_BASE
#define RAM_SIZE                                RAM_S_SIZE
#define RAMX_BASE                               RAMX_S_BASE
#define RAMX_SIZE                               RAMX_S_SIZE
#define PSRAM_BASE                              SYS_PSRAM_S_BASE
#define PSRAM_NC_BASE                           SYS_PSRAM_NC_S_BASE
#define PSRAMX_BASE                             SYS_PSRAM_S_BASE
#define PSRAMX_NC_BASE                          SYS_PSRAM_NC_S_BASE
#define PSRAM_REGION_SIZE                       SYS_PSRAM_S_SIZE
#endif

#define EXTRA_RAM0_BASE                         EXTRA_RAM0_NORM_BASE
#define EXTRA_RAM0_SIZE                         EXTRA_RAM0_NORM_SIZE
#ifdef EXTRA_RAM1_NORM_BASE
#define EXTRA_RAM1_BASE                         EXTRA_RAM1_NORM_BASE
#define EXTRA_RAM1_SIZE                         EXTRA_RAM1_NORM_SIZE
#endif

#ifdef ALT_BOOT_FLASH
/* Skip BTH cache by using BTH uncached address space of BTH flash */
#define FLASH_BASE                              BTH_FLASHX_NC_BASE
#define FLASH_NC_BASE                           BTH_FLASH_NC_BASE
#define FLASHX_BASE                             BTH_FLASHX_NC_BASE
#define FLASHX_NC_BASE                          BTH_FLASH_NC_BASE

/* SYS only needs 2 kind of address space for flash */
#define FLASH1_BASE                             SYS_FLASH_BASE
#define FLASH1_NC_BASE                          SYS_FLASH_NC_BASE
#define FLASH1X_BASE                            SYS_FLASH_BASE
#define FLASH1X_NC_BASE                         SYS_FLASH_NC_BASE

#define NOMINAL_FLASH_CTRL_BASE                 BTH_FLASH_CTRL_BASE
#define NOMINAL_SEC_FLASH_CTRL_BASE             BTH_FLASH_CTRL_BASE
#define NOMINAL_FLASH1_CTRL_BASE                SYS_FLASH_CTRL_BASE

#else // !ALT_BOOT_FLASH
#if defined(PROGRAMMER) && !defined(PROGRAMMER_INFLASH)
/* To program SYS flash images */
#define FLASH_BASE                              SYS_FLASH_BASE
#define FLASH_NC_BASE                           SYS_FLASH_NC_BASE
#define FLASHX_BASE                             SYS_FLASHX_BASE
#define FLASHX_NC_BASE                          SYS_FLASHX_NC_BASE

/* To program BTH flash images */
#define FLASH1_BASE                             BTH_FLASH_BASE
#define FLASH1_NC_BASE                          BTH_FLASH_NC_BASE
#define FLASH1X_BASE                            BTH_FLASHX_BASE
#define FLASH1X_NC_BASE                         BTH_FLASHX_NC_BASE
#else
/* SYS only needs 2 kind of address space for flash */
#define FLASH_BASE                              SYS_FLASH_BASE
#define FLASH_NC_BASE                           SYS_FLASH_NC_BASE
#define FLASHX_BASE                             SYS_FLASH_BASE
#define FLASHX_NC_BASE                          SYS_FLASH_NC_BASE

/* Skip BTH cache by using BTH uncached address space of BTH flash */
#define FLASH1_BASE                             BTH_FLASHX_NC_BASE
#define FLASH1_NC_BASE                          BTH_FLASH_NC_BASE
#define FLASH1X_BASE                            BTH_FLASHX_NC_BASE
#define FLASH1X_NC_BASE                         BTH_FLASH_NC_BASE
#endif

#define NOMINAL_FLASH_CTRL_BASE                 SYS_FLASH_CTRL_BASE
#define NOMINAL_SEC_FLASH_CTRL_BASE             RAM_NS_TO_S(SYS_FLASH_CTRL_BASE)
#define NOMINAL_FLASH1_CTRL_BASE                BTH_FLASH_CTRL_BASE

#endif // !ALT_BOOT_FLASH

#define USB_BASE                                SYS_USB_BASE
#define SEDMA_BASE                              SYS_SEDMA_BASE
#define SDMMC0_BASE                             SYS_SDMMC_BASE

#define WDT_BASE                                AON_WDT_BASE
#define TIMER0_BASE                             AON_TIMER0_BASE
#define TIMER1_BASE                             SYS_TIMER0_BASE
#define TIMER2_BASE                             SYS_TIMER1_BASE
#define I2C0_BASE                               SYS_I2C0_BASE
#define I2C1_BASE                               SYS_I2C1_BASE
#define SPI_BASE                                SYS_SPI_BASE
#define ISPI_BASE                               SYS_ISPI_BASE
#define UART0_BASE                              SYS_UART0_BASE
#define UART1_BASE                              SYS_UART1_BASE
#define TRNG_BASE                               SYS_TRNG_BASE
#define I2S0_BASE                               SYS_I2S0_BASE
#define I2S1_BASE                               SYS_I2S1_BASE
#define SPDIF0_BASE                             SYS_SPDIF0_BASE
#define GPIO_BASE                               AON_GPIO_BASE
#define GPIO1_BASE                              AON_GPIO1_BASE
#define SEC_ENG_BASE                            SYS_SEC_ENG_BASE
#define TZC_BASE                                SYS_TZC_BASE
#define BTPCM_BASE                              SYS_BTPCM_BASE
#define CPUDUMP_BASE                            SYS_CPUDUMP_BASE
/*
#define BTDUMP_BASE                             BTH_BTDUMP_BASE
*/
#define PWM_BASE                                SYS_PWM_BASE

#define SEC_CTRL_BASE                           SYS_SEC_CTRL_BASE

#if defined(NO_TRUSTZONE) || defined(ARM_CMNS) || \
        !(defined(ARM_CMSE) || defined(ROM_BUILD) || defined(PROGRAMMER))
#define FLASH_CTRL_BASE                         NOMINAL_FLASH_CTRL_BASE
#else
#define FLASH_CTRL_BASE                         NOMINAL_SEC_FLASH_CTRL_BASE
#endif
#if ((defined(ROM_BUILD) || defined(PROGRAMMER)) && !defined(SIMU)) || defined(SYS_USE_BTH_FLASH)
#define FLASH1_CTRL_BASE                        NOMINAL_FLASH1_CTRL_BASE
#endif
#define AUDMA_BASE                              SYS_AUDMA_BASE
#if !(defined(CHIP_DMA_CFG_IDX) && ((CHIP_DMA_CFG_IDX == 1) || (CHIP_DMA_CFG_IDX == 2)))
#define GPDMA_BASE                              SYS_GPDMA_BASE
#endif

#define SLOW_TIMER_SUB_ID_WITH_IRQ              0

#ifdef RAM_NC_BASE
#define MPU_RAM_NC_ENTRY                        \
    { RAM_NC_BASE, RAM_NC_TOTAL_SIZE, MPU_ATTR_READ_WRITE, MEM_ATTR_NORMAL_NON_CACHEABLE, },
#else
#define MPU_RAM_NC_ENTRY
#endif

/* TODO: Generate mpu table for:
   (1) NO_MPU_DEFAULT_MAP
   (2) ARM_CMSE
   (3) ARM_CMNS
*/
#define MPU_RAM_NORMAL_CODE                     \
    { RAM_S_TO_NS(ROM_BASE), (M55_ITCM_END - RAM_S_TO_NS(ROM_BASE)), MPU_ATTR_READ_WRITE_EXEC, MEM_ATTR_NORMAL_WRITE_THROUGH, },
#define MPU_RAM_NORMAL_DATA                     \
    { RAM_S_TO_NS(M55_MIN_DTCM_BASE), (SYS_RAM_END - RAM_S_TO_NS(M55_MIN_DTCM_BASE)), MPU_ATTR_READ_WRITE_EXEC, MEM_ATTR_NORMAL_WRITE_BACK, },

#define MPU_INIT_TABLE                          \
    /* SYS RAM NC */ \
    MPU_RAM_NC_ENTRY \
    /* DSP-ITCM */ \
    { HIFI4_MIN_ITCM_BASE, (HIFI4_ITCM_END - HIFI4_MIN_ITCM_BASE), MPU_ATTR_READ_WRITE, MEM_ATTR_NORMAL_NON_CACHEABLE, }, \
    /* BTH RAM, SENS RAM, DSP-DTCM */ \
    { BTH_MIN_RAM_BASE, (HIFI4_DTCM_END - BTH_MIN_RAM_BASE), MPU_ATTR_READ_WRITE, MEM_ATTR_NORMAL_NON_CACHEABLE, }, \
    /* SYS FLASH NC */ \
    { SYS_FLASH_NC_BASE, (SYS_FLASH_BASE - SYS_FLASH_NC_BASE), MPU_ATTR_READ_WRITE_EXEC, MEM_ATTR_NORMAL_NON_CACHEABLE, }, \
    /* BTH FLASH NC */ \
    { BTH_FLASH_NC_BASE, (BTH_FLASH_BASE - BTH_FLASH_NC_BASE), MPU_ATTR_READ_WRITE, MEM_ATTR_NORMAL_NON_CACHEABLE, }, \
    /* SYS PSRAM NC */ \
    { SYS_PSRAM_NC_BASE, (SYS_PSRAM_BASE - SYS_PSRAM_NC_BASE), MPU_ATTR_READ_WRITE_EXEC, MEM_ATTR_NORMAL_NON_CACHEABLE, }, \
    /* BT RAM */ \
    { BT_RAM_BASE, BT_RAM_SIZE, MPU_ATTR_READ_WRITE, MEM_ATTR_NORMAL_NON_CACHEABLE, }, \
    /* BT EXCH RAM */ \
    { BT_EXCH_MEM_BASE, BT_EXCH_MEM_SIZE, MPU_ATTR_READ_WRITE, MEM_ATTR_NORMAL_NON_CACHEABLE, }, \

#ifdef __cplusplus
}
#endif

#endif
