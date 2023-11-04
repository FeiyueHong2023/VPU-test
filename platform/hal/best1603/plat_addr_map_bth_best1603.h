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
#ifndef __PLAT_ADDR_MAP_BTH_BEST1603_H__
#define __PLAT_ADDR_MAP_BTH_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PROGRAMMER_LOAD_RAM
#define RAM_SECURITY_FLAG                       0x04000000
#define LOAD_RAM_BASE                           RAM_NS_TO_S(M55_ITCM_BASE)
#define LOAD_RAM_SIZE                           M55_ITCM_SIZE
#endif

#define RAM0_BASE                               BTH_RAM_BASE
#define RAM0_SIZE                               BTH_RAM_SIZE

#define RAM_TOTAL_SIZE                          (RAM0_BASE + RAM0_SIZE - RAM0_BASE)

#define RAM_BASE                                BTH_RAM_BASE
#ifdef BTH_AS_MAIN_MCU
#if defined(CP_IN_SAME_EE)
#ifndef RAMCPX_SIZE
#define RAMCPX_SIZE                             (BTH_RAM7_BASE - BTH_RAM6_BASE)
#endif
#define RAMCPX_BASE                             (BTH_RAM6_BASE - BTH_RAM0_BASE + BTH_RAMX0_BASE)

#ifndef RAMCP_SIZE
#define RAMCP_SIZE                              BTH_RAM7_SIZE
#endif
#define RAMCP_BASE                              BTH_RAM7_BASE

#define RAM_SIZE                                BTH_RAM_SIZE - RAMCPX_SIZE - RAMCPX_SIZE
#else
#define RAM_SIZE                                BTH_RAM_SIZE
#endif
#else
#define RAM_SIZE                                (BTH_MAILBOX_BASE - RAM_BASE)
#endif

#define RAMX_BASE                               (BTH_RAM_BASE - BTH_RAM0_BASE + BTH_RAMX0_BASE)

#ifdef ALT_BOOT_FLASH
#define FLASH_BASE                              (SYS_FLASH_NC_BASE | 0x4000000)
#define FLASH_NC_BASE                           SYS_FLASH_NC_BASE
#define FLASHX_BASE                             (SYS_FLASHX_NC_BASE | 0x4000000)
#define FLASHX_NC_BASE                          SYS_FLASHX_NC_BASE

#define FLASH1_BASE                             SYS_FLASH_BASE
#define FLASH1_NC_BASE                          SYS_FLASH_NC_BASE
#define FLASH1X_BASE                            SYS_FLASHX_BASE
#define FLASH1X_NC_BASE                         SYS_FLASHX_NC_BASE
#else
#define FLASH_BASE                              BTH_FLASH_BASE
#define FLASH_NC_BASE                           BTH_FLASH_NC_BASE
#define FLASHX_BASE                             BTH_FLASHX_BASE
#define FLASHX_NC_BASE                          BTH_FLASHX_NC_BASE

#define FLASH1_BASE                             SYS_FLASH_BASE
#define FLASH1_NC_BASE                          SYS_FLASH_NC_BASE
#define FLASH1X_BASE                            SYS_FLASHX_BASE
#define FLASH1X_NC_BASE                         SYS_FLASHX_NC_BASE
#endif

#define WDT_BASE                                AON_WDT_BASE
#define TIMER0_BASE                             AON_TIMER0_BASE
#define TIMER1_BASE                             BTH_TIMER0_BASE
#define TIMER2_BASE                             BTH_TIMER1_BASE
#define I2C0_BASE                               BTH_I2C0_BASE
#define I2C1_BASE                               BTH_I2C1_BASE
#define ISPI_BASE                               BTH_ISPI_BASE
#define UART0_BASE                              BTH_UART0_BASE
#define UART1_BASE                              BTH_UART1_BASE
#define BTPCM_BASE                              BTH_BTPCM_BASE
#define BTDUMP_BASE                             BTH_BTDUMP_BASE
#define SPI_BASE                                BTH_SPI_BASE
#define SPILCD_BASE                             BTH_SPILCD_BASE
#define CPUDUMP_BASE                            BTH_CPUDUMP_BASE
#ifdef FPGA
#define I2S0_BASE                               BTH_I2S0_BASE
#endif

#define ICACHE_CTRL_BASE                        BTH_ICACHE_CTRL_BASE

#define GPIO_BASE                               AON_GPIO_BASE
#define GPIO1_BASE                              AON_GPIO1_BASE

#ifdef ALT_BOOT_FLASH
#define FLASH_CTRL_BASE                         SYS_FLASH_CTRL_BASE
#else
#define FLASH_CTRL_BASE                         BTH_FLASH_CTRL_BASE
#endif

#define AUDMA_BASE                              BTH_AUDMA_BASE

#ifdef BTH_USE_SYS_PERIPH
#define SDMMC0_BASE                             SYS_SDMMC_BASE

#if defined(CHIP_DMA_CFG_IDX) && ((CHIP_DMA_CFG_IDX == 1) || (CHIP_DMA_CFG_IDX == 2))
#define GPDMA_BASE                              SYS_GPDMA_BASE
#endif

#ifdef BTH_AS_MAIN_MCU
#define USB_BASE                                SYS_USB_BASE
#define EMMC_BASE                               SYS_EMMC_BASE
#define SEDMA_BASE                              SYS_SEDMA_BASE

#define UART2_BASE                              SYS_UART0_BASE
#define TRNG_BASE                               SYS_TRNG_BASE
#define I2S0_BASE                               SYS_I2S0_BASE
#define I2S1_BASE                               SYS_I2S1_BASE
#define SPDIF0_BASE                             SYS_SPDIF0_BASE
#define SEC_ENG_BASE                            SYS_SEC_ENG_BASE
#define PWM_BASE                                SYS_PWM_BASE

#ifdef BTH_USE_SYS_SENS_PERIPH
#define I2C2_BASE                               SENS_I2C2_BASE
#define I2C3_BASE                               SENS_I2C4_BASE
#endif
#endif /* BTH_AS_MAIN_MCU */
#endif /* BTH_USE_SYS_PERIPH */

#ifdef BTH_USE_SYS_FLASH
#define FLASH1_CTRL_BASE                        SYS_FLASH_CTRL_BASE
#endif

#define DSP_M55_MIN_ITCM_BASE                   M55_MIN_ITCM_BASE
#define DSP_M55_MIN_DTCM_BASE                   M55_MIN_DTCM_BASE
#define DSP_M55_ITCM_BASE                       M55_ITCM_BASE
#define DSP_M55_ITCM_SIZE                       M55_ITCM_SIZE
#define DSP_M55_DTCM_BASE                       M55_DTCM_BASE
#define DSP_M55_DTCM_SIZE                       M55_DTCM_SIZE
#define DSP_M55_SYS_RAM_BASE                    M55_SYS_RAM_BASE
#define DSP_M55_SYS_RAM_SIZE                    M55_SYS_RAM_SIZE
#define DSP_M55_MAILBOX_BASE                    M55_MAILBOX_BASE

#ifdef __cplusplus
}
#endif

#endif
