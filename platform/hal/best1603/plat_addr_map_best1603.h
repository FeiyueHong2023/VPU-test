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
#ifndef __PLAT_ADDR_MAP_BEST1603_H__
#define __PLAT_ADDR_MAP_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Hardware definitions */

#if defined(NO_TRUSTZONE) || defined(CHIP_SUBSYS_BTH) || defined(CHIP_SUBSYS_SENS)
#define ROM_BASE                                0x00020000
#define ROMX_BASE                               0x00020000
#else
#define ROM_BASE                                0x04020000
#define ROMX_BASE                               0x04020000
#endif

#ifndef ROM_SIZE
#define ROM_SIZE                                0x00018000
#endif
#define ROM_EXT_SIZE                            0x00000000

#define SHR_RAM_BLK_SIZE                        0x00020000
#define SHR_RAM_TOTAL_SIZE                      (SHR_RAM_BLK_SIZE * 11)

/* For MPC config */
#define M55_ITCM_START                          0x0
#define M55_DTCM_START                          0x20000000

#define M55_ITCM_END                            0x00200000
#define M55_DTCM_END                            0x20200000

#define M55_MIN_ITCM_BASE                       (M55_ITCM_START + 0x00040000)
#define M55_MAX_ITCM_BASE                       (M55_ITCM_END - SHR_RAM_BLK_SIZE)
#define M55_MIN_ITCM_SIZE                       0x00000000
#define M55_MAX_ITCM_SIZE                       0x000C0000

#define M55_MIN_DTCM_BASE                       M55_DTCM_START
#define M55_MAX_DTCM_BASE                       (M55_DTCM_END - SHR_RAM_BLK_SIZE)
#define M55_MIN_DTCM_SIZE                       0x00000000
#define M55_MAX_DTCM_SIZE                       0x00200000

#define HIFI4_ITCM_END                          0x008C0000
#define HIFI4_DTCM_END                          0x20A00000

#define HIFI4_MIN_ITCM_BASE                     0x00800000
#define HIFI4_MAX_ITCM_BASE                     (HIFI4_ITCM_END - SHR_RAM_BLK_SIZE)
#define HIFI4_MIN_ITCM_SIZE                     0x00000000
#define HIFI4_MAX_ITCM_SIZE                     0x000C0000

#define HIFI4_MIN_DTCM_BASE                     0x20800000
#define HIFI4_MAX_DTCM_BASE                     (HIFI4_DTCM_END - SHR_RAM_BLK_SIZE * 2)
#define HIFI4_MIN_DTCM_SIZE                     0x00000000
#define HIFI4_MAX_DTCM_SIZE                     0x00200000

/* SYS RAM0/1 share with M55/HIFI4 I/DTCM */
#define SYS_RAM0_BASE                           0x20200000
#define SYS_RAMX0_BASE                          0x00200000
#define SYS_RAM1_BASE                           0x20300000
#define SYS_RAM2_BASE                           0x20400000
#define SYS_RAM3_BASE                           0x20480000

#define SYS_RAM3_SIZE                           0x00080000
#define SYS_RAM_END                             (SYS_RAM3_BASE + SYS_RAM3_SIZE)

#define SYS_MIN_RAM_BASE                        SYS_RAM0_BASE
#define SYS_MAX_RAM_BASE                        (SYS_RAM_END - SHR_RAM_BLK_SIZE)
#define SYS_MIN_RAM_SIZE                        0x00000000
#define SYS_MAX_RAM_SIZE                        (SYS_RAM_END - SYS_RAM0_BASE)

#define BTH_RAM0_BASE                           0x20500000
#define BTH_RAMX0_BASE                          0x00500000
#define BTH_RAM1_BASE                           0x20540000
#define BTH_RAM2_BASE                           0x20560000
#define BTH_RAM3_BASE                           0x20580000
#define BTH_RAM4_BASE                           0x205A0000
#define BTH_RAM5_BASE                           0x205C0000
#define BTH_RAM6_BASE                           0x205E0000
#define BTH_RAM7_BASE                           0x20600000

#define BTH_RAM7_SIZE                           0x00020000
#define BTH_RAM_END                             (BTH_RAM7_BASE + BTH_RAM7_SIZE)

#define BTH_MIN_RAM_BASE                        BTH_RAM0_BASE
#define BTH_MAX_RAM_BASE                        BTH_RAM_END /*BTH_RAM0_BASE*/
#define BTH_MIN_RAM_SIZE                        0 /*(BTH_RAM2_BASE - BTH_RAM0_BASE)*/
#define BTH_MAX_RAM_SIZE                        (BTH_RAM_END - BTH_RAM0_BASE)

#define SENS_RAM0_BASE                          0x20600000
#define SENS_RAMX0_BASE                         0x00600000
#define SENS_RAM1_BASE                          0x20620000
#define SENS_RAM2_BASE                          0x20640000
#define SENS_RAM3_BASE                          0x20680000
#define SENS_RAM4_BASE                          0x206C0000
#define SENS_RAM5_BASE                          0x206E0000

#define SENS_RAM5_SIZE                          0x00020000

#if defined(CODEC_VAD_CFG_BUF_SIZE) && (CODEC_VAD_CFG_BUF_SIZE > 0)
#error "The chip best1603 don`t have vad!"
#else
#define SENS_RAM_END                            (SENS_RAM5_BASE + SENS_RAM5_SIZE)
#endif

#define SENS_MIN_RAM_BASE                       SENS_RAM0_BASE
#define SENS_MAX_RAM_BASE                       SENS_RAM_END /*SENS_RAM4_BASE*/
#define SENS_MIN_RAM_SIZE                       0 /*(SENS_RAM_END - SENS_RAM4_BASE)*/
#define SENS_MAX_RAM_SIZE                       (SENS_RAM_END - SENS_RAM0_BASE)

#define BTC_MIN_ROM_RAM_BASE                    0xC0400000
#define BTC_MIN_ROM_RAM_SIZE                    0
#define BTC_MAX_ROM_RAM_SIZE                    (SHR_RAM_BLK_SIZE * 12 + BT_RAM_SIZE)

#define SYS_PSRAM_BASE                          0x3C000000
#define SYS_PSRAM_NC_BASE                       0x38000000
#define SYS_PSRAMX_BASE                         0x1C000000
#define SYS_PSRAMX_NC_BASE                      0x18000000

#define SYS_FLASH_BASE                          0x2C000000
#define SYS_FLASH_NC_BASE                       0x28000000
#define SYS_FLASHX_BASE                         0x0C000000
#define SYS_FLASHX_NC_BASE                      0x08000000

#define BTH_FLASH_BASE                          0x34000000
#define BTH_FLASH_NC_BASE                       0x30000000
#define BTH_FLASHX_BASE                         0x14000000
#define BTH_FLASHX_NC_BASE                      0x10000000

#define BTH_ICACHE_CTRL_BASE                    0x07FFA000

#define BTH_AUDMA_BASE                          0x40130000
#define BTH_FLASH_CTRL_BASE                     0x40140000
#define BTH_BTDUMP_BASE                         0x40150000
#define BTH_CPUDUMP_BASE                        0x40160000

#define BTH_CMU_BASE                            0x40000000
#define BTH_WDT_BASE                            0x40001000
#define BTH_TIMER0_BASE                         0x40002000
#define BTH_TIMER1_BASE                         0x40003000
#define BTH_I2C0_BASE                           0x40005000
#define BTH_I2C1_BASE                           0x40006000
#define BTH_SPI_BASE                            0x40007000
#define BTH_SPILCD_BASE                         0x40008000
#define BTH_ISPI_BASE                           0x40009000
#define BTH_UART0_BASE                          0x4000B000
#define BTH_UART1_BASE                          0x4000C000
#define BTH_BTPCM_BASE                          0x4000E000
#ifdef FPGA
#define BTH_I2S0_BASE                           0x4000F000
#endif

#ifdef ARM_CMSE
#define AON_CMU_BASE                            0x44080000
#else
#define AON_CMU_BASE                            0x40080000
#endif
#define AON_WDT_BASE                            0x40082000
#define AON_TIMER0_BASE                         0x40083000
#define AON_TIMER1_BASE                         0x40084000
#ifdef ARM_CMSE
#define AON_PSC_BASE                            0x44085000
#else
#define AON_PSC_BASE                            0x40085000
#endif
#define AON_IOMUX_BASE                          0x40086000
#define I2C_SLAVE_BASE                          0x40087000
#ifdef ARM_CMSE
#define AON_SEC_CTRL_BASE                       0x44088000
#else
#define AON_SEC_CTRL_BASE                       0x40088000
#endif
#define AON_GPIO_BASE                           0x40089000
#define AON_GPIO1_BASE                          0x4008A000
#define AON_PMU_BASE                            0x4008B000

#define XDMA_BASE                               0x50400000
#define AXI_GPV_BASE                            0x50500000

#define CODEC_BASE                              0x40300000

#define SYS_USB_BASE                            0x50100000
#define SYS_SEDMA_BASE                          0x50160000
#define SYS_SDMMC_BASE                          0x50170000
#ifdef ARM_CMSE
#define SYS_FLASH_CTRL_BASE                     0x54190000
#else
#define SYS_FLASH_CTRL_BASE                     0x50190000
#endif
#define SYS_NANDFLASH_CTRL_BASE                 0x50198000
#define SYS_AUDMA_BASE                          0x501A0000
#define SYS_GPDMA_BASE                          0x501B0000

#define CMU_BASE                                SYS_CMU_BASE
#define SYS_CMU_BASE                            0x50000000
#define SYS_WDT_BASE                            0x50001000
#define SYS_TIMER0_BASE                         0x50002000
#define SYS_TIMER1_BASE                         0x50003000
#define SYS_TIMER2_BASE                         0x50004000
#define SYS_TIMER3_BASE                         0x50005000
#define SYS_I2C0_BASE                           0x50006000
#define SYS_I2C1_BASE                           0x50007000
#define SYS_SPI_BASE                            0x50008000
#define SYS_ISPI_BASE                           0x5000A000
#define SYS_UART0_BASE                          0x5000B000
#define SYS_UART1_BASE                          0x5000C000
#define SYS_TRNG_BASE                           0x5000D000
#define SYS_I2S0_BASE                           0x5000E000
#define SYS_I2S1_BASE                           0x5000F000
#define SYS_SPDIF0_BASE                         0x50010000
#define SYS_SEC_ENG_BASE                        0x50030000
#define SYS_TZC_BASE                            0x50040000
#define SYS_IOMUX_BASE                          0x50060000
#define SYS_BTPCM_BASE                          0x50064000
#define SYS_IRQ_CTRL_BASE                       0x50068000
#define SYS_PWM_BASE                            0x5006C000
#define SYS_TCNT_BASE                           0x50070000
#define SYS_CPUDUMP_BASE                        0x501C0000

#define SYS_SEC_CTRL_BASE                       0x54040000
#define PPC_SPY_BTH_X2H_BASE                    0x54042000
#define PPC_SPY_BTC_X2H_BASE                    0x54044000
#define PPC_SPY_SENS_X2H_BASE                   0x54046000
#define SPY_M55_S_BASE                          0x54048000
#define PPC_SPY_CODEC_BASE                      0x5404A000
#define PPC_SPY_DMA_S_BASE                      0x5404C000
#define PPC_SPY_DSP_S_BASE                      0x5404E000
#define MPC_FLASH_BASE                          0x54050000
#define SPY_FLASH_BASE                          0x54051000
#define MPC_PSRAM_BASE                          0x54052000
#define SPY_PSRAM_BASE                          0x54053000
#define MPC_SRAM0_BASE                          0x54054000
#define SPY_SRAM0_BASE                          0x54055000
#define MPC_SRAM1_BASE                          0x54056000
#define SPY_SRAM1_BASE                          0x54057000
#define MPC_SRAM2_BASE                          0x54058000
#define SPY_SRAM2_BASE                          0x54059000
#define MPC_SRAM3_BASE                          0x5405A000
#define SPY_SRAM3_BASE                          0x5405B000

#define SENS_SENSOR_ENG0_BASE                   0x58100000
#define SENS_SENSOR_ENG1_BASE                   0x58110000
#define SENS_SDMA_BASE                          0x58120000
#define SENS_SENSOR_ENG2_BASE                   0x58150000
#define SENS_SENSOR_ENG3_BASE                   0x58160000

#define SENS_CMU_BASE                           0x58000000
#define SENS_WDT_BASE                           0x58001000
#define SENS_TIMER0_BASE                        0x58002000
#define SENS_TIMER1_BASE                        0x58003000
#define SENS_I2C0_BASE                          0x58004000
#define SENS_I2C1_BASE                          0x58005000
#define SENS_I2C2_BASE                          0x58006000
#define SENS_I2C3_BASE                          0x58007000
#define SENS_I2C4_BASE                          0x58008000
#define SENS_SPI_BASE                           0x58009000
#define SENS_SPILCD_BASE                        0x5800A000
#define SENS_ISPI_BASE                          0x5800B000
#define SENS_UART0_BASE                         0x5800C000
#define SENS_UART1_BASE                         0x5800D000
#define SENS_UART2_BASE                         0x5800E000
#define SENS_TCNT_BASE                          0x5800F000
#define SENS_I2S0_BASE                          0x58010000
#define SENS_CAP_SENSOR_BASE                    0x58011000
#define SENS_CPUDUMP_BASE                       0x58170000

#ifndef BT_SUBSYS_BASE
#define BT_SUBSYS_BASE                          0xA0000000
#endif
#ifndef BT_RAM_BASE
#define BT_RAM_BASE                             0xC0000000
#endif
#ifndef BT_RAM_SIZE
#define BT_RAM_SIZE                             0x00010000
#endif
#define BT_EXCH_MEM_BASE                        0xD0210000
#define BT_EXCH_MEM_SIZE                        0x00008000
#define BT_UART_BASE                            0xD0300000
#define BT_CMU_BASE                             0xD0330000
#define BTC_CPUDUMP_BASE                        0xD0700000

/* RAM config */

#include CHIP_SPECIFIC_HDR(plat_ram_cfg)

/* Mailbox config */

#define M55_MAILBOX_SIZE                        0x20
#ifdef MAIN_RAM_USE_SYS_RAM
#define M55_MAILBOX_BASE                        (M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE - M55_MAILBOX_SIZE)
#else
#define M55_MAILBOX_BASE                        (M55_DTCM_BASE + M55_DTCM_SIZE - M55_MAILBOX_SIZE)
#endif

#define HIFI4_MAILBOX_SIZE                      0x20
#define HIFI4_MAILBOX_BASE                      (HIFI4_DTCM_BASE + HIFI4_DTCM_SIZE - HIFI4_MAILBOX_SIZE)

#define BTH_MAILBOX_SIZE                        0x20
#define BTH_MAILBOX_BASE                        (BTH_RAM_BASE + BTH_RAM_SIZE - BTH_MAILBOX_SIZE)

#define SENS_MAILBOX_SIZE                       0x20
#define SENS_MAILBOX_BASE                       (SENS_RAM_BASE + SENS_RAM_SIZE - SENS_MAILBOX_SIZE)

#ifdef LINKER_SCRIPT

/* RAM config checks (only check in lds file, the first compiled file during build) */

#if (M55_ITCM_BASE < M55_MIN_ITCM_BASE) || (M55_ITCM_BASE > M55_MAX_ITCM_BASE)
#error "Bad M55_ITCM_BASE 1"
#endif
#if (M55_ITCM_BASE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad M55_ITCM_BASE 2"
#endif
#if (M55_ITCM_BASE + M55_ITCM_SIZE > M55_ITCM_END)
#error "Bad M55_ITCM_SIZE 3"
#endif
#if (M55_ITCM_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad M55_ITCM_SIZE 4"
#endif

#if (M55_DTCM_BASE < M55_MIN_DTCM_BASE) || (M55_DTCM_BASE > M55_MAX_DTCM_BASE)
#error "Bad M55_DTCM_BASE 1"
#endif
#if (M55_DTCM_BASE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad M55_DTCM_BASE 2"
#endif
#if (M55_DTCM_BASE + M55_DTCM_SIZE > M55_DTCM_END)
#error "Bad M55_DTCM_SIZE 3"
#endif
#if (M55_DTCM_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad M55_DTCM_SIZE 4"
#endif

#if (HIFI4_ITCM_BASE < HIFI4_MIN_ITCM_BASE) || (HIFI4_ITCM_BASE > HIFI4_MAX_ITCM_BASE)
#error "Bad HIFI4_ITCM_BASE 1"
#endif
#if (HIFI4_ITCM_BASE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad HIFI4_ITCM_BASE 2"
#endif
#if (HIFI4_ITCM_BASE + HIFI4_ITCM_SIZE > HIFI4_ITCM_END)
#error "Bad HIFI4_ITCM_SIZE 3"
#endif
#if (HIFI4_ITCM_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad HIFI4_ITCM_SIZE 4"
#endif

#if (HIFI4_DTCM_BASE < HIFI4_MIN_DTCM_BASE) || (HIFI4_DTCM_BASE > HIFI4_MAX_DTCM_BASE)
#error "Bad HIFI4_DTCM_BASE 1"
#endif
#if (HIFI4_DTCM_BASE & (SHR_RAM_BLK_SIZE * 2 - 1))
#error "Bad HIFI4_DTCM_BASE 2"
#endif
#if (HIFI4_DTCM_BASE + HIFI4_DTCM_SIZE > HIFI4_DTCM_END)
#error "Bad HIFI4_DTCM_SIZE 3"
#endif
#if (HIFI4_DTCM_SIZE & (SHR_RAM_BLK_SIZE * 2 - 1))
#error "Bad HIFI4_DTCM_SIZE 4"
#endif

#define ITCM_CONV(a)                        ((a) - HIFI4_MIN_ITCM_BASE + M55_MIN_ITCM_BASE)

#if (M55_ITCM_SIZE > 0) && (HIFI4_ITCM_SIZE > 0)
#if (M55_ITCM_BASE < ITCM_CONV(HIFI4_ITCM_BASE))
#if (M55_ITCM_BASE + M55_ITCM_SIZE > ITCM_CONV(HIFI4_ITCM_BASE))
#error "M55 ITCM conflicts with HIFI4 ITCM 1"
#endif
#elif (M55_ITCM_BASE > ITCM_CONV(HIFI4_ITCM_BASE))
#if (ITCM_CONV(HIFI4_ITCM_BASE) + HIFI4_ITCM_SIZE > M55_ITCM_BASE)
#error "M55 ITCM conflicts with HIFI4 ITCM 2"
#endif
#else
#error "M55 ITCM conflicts with HIFI4 ITCM 3"
#endif
#endif

#define DTCM_CONV(a)                        ((a) - HIFI4_MIN_DTCM_BASE + M55_MIN_DTCM_BASE)

#if (M55_DTCM_SIZE > 0) && (HIFI4_DTCM_SIZE > 0)
#if (M55_DTCM_BASE < DTCM_CONV(HIFI4_DTCM_BASE))
#if (M55_DTCM_BASE + M55_DTCM_SIZE > DTCM_CONV(HIFI4_DTCM_BASE))
#error "M55 DTCM conflicts with HIFI4 DTCM 1"
#endif
#elif (M55_DTCM_BASE > DTCM_CONV(HIFI4_DTCM_BASE))
#if (DTCM_CONV(HIFI4_DTCM_BASE) + HIFI4_DTCM_SIZE > M55_DTCM_BASE)
#error "M55 DTCM conflicts with HIFI4 DTCM 2"
#endif
#else
#error "M55 DTCM conflicts with HIFI4 DTCM 3"
#endif
#endif

#if (M55_ITCM_SIZE > 0)
#define M55_ITCM_OFFSET_END                 (M55_ITCM_BASE + M55_ITCM_SIZE - M55_MIN_ITCM_BASE)
#else
#define M55_ITCM_OFFSET_END                 0
#endif
#if (HIFI4_ITCM_SIZE > 0)
#define HIFI4_ITCM_OFFSET_END               (HIFI4_ITCM_BASE + HIFI4_ITCM_SIZE - HIFI4_MIN_ITCM_BASE)
#else
#define HIFI4_ITCM_OFFSET_END               0
#endif
#if (M55_ITCM_OFFSET_END > HIFI4_ITCM_OFFSET_END)
#define ITCM_OFFSET_END                     M55_ITCM_OFFSET_END
#else
#define ITCM_OFFSET_END                     HIFI4_ITCM_OFFSET_END
#endif

#if (M55_DTCM_SIZE > 0)
#define M55_DTCM_OFFSET_END                 (M55_DTCM_BASE + M55_DTCM_SIZE - M55_MIN_DTCM_BASE)
#else
#define M55_DTCM_OFFSET_END                 0
#endif
#if (HIFI4_DTCM_SIZE > 0)
#define HIFI4_DTCM_OFFSET_END               (HIFI4_DTCM_BASE + HIFI4_DTCM_SIZE - HIFI4_MIN_DTCM_BASE)
#else
#define HIFI4_DTCM_OFFSET_END               0
#endif
#if (M55_DTCM_OFFSET_END > HIFI4_DTCM_OFFSET_END)
#define DTCM_OFFSET_END                     M55_DTCM_OFFSET_END
#else
#define DTCM_OFFSET_END                     HIFI4_DTCM_OFFSET_END
#endif

#if (ITCM_OFFSET_END > (SHR_RAM_BLK_SIZE * 16 - DTCM_OFFSET_END))
#error "M55/HIFI4 ITCM conflicts with M55/HIFI4 DTCM"
#endif

#if (M55_SYS_RAM_BASE < SYS_MIN_RAM_BASE) || (M55_SYS_RAM_BASE > SYS_MAX_RAM_BASE)
#error "Bad M55_SYS_RAM_BASE 1"
#endif
#if (M55_SYS_RAM_BASE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad M55_SYS_RAM_BASE 2"
#endif
#if (M55_SYS_RAM_SIZE + M55_SYS_RAM_SIZE > SYS_RAM_END)
#error "Bad M55_SYS_RAM_SIZE 3"
#endif
#if (M55_SYS_RAM_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad M55_SYS_RAM_SIZE 4"
#endif

#if (M55_SYS_RAM_BASE < SYS_RAM2_BASE)
#if (M55_DTCM_SIZE > 0) && ((SYS_RAM2_BASE - M55_SYS_RAM_BASE) > (M55_DTCM_BASE - M55_MIN_DTCM_BASE))
#error "M55 SYS RAM conflicts with M55 DTCM"
#endif
#if (HIFI4_DTCM_SIZE > 0) && ((SYS_RAM2_BASE - M55_SYS_RAM_BASE) > (HIFI4_DTCM_BASE - HIFI4_MIN_DTCM_BASE))
#error "M55 SYS RAM conflicts with HIFI4 DTCM"
#endif
#if (M55_DTCM_SIZE == 0) && (HIFI4_DTCM_SIZE == 0) && \
        ((SYS_RAM2_BASE - M55_SYS_RAM_BASE + M55_ITCM_SIZE + HIFI4_ITCM_SIZE) > (SHR_RAM_BLK_SIZE * 16))
#error "M55 SYS RAM conflicts with M55/HIFI4 ITCM"
#endif
#endif

#if (HIFI4_SYS_RAM_BASE < SYS_MIN_RAM_BASE) || (HIFI4_SYS_RAM_BASE > SYS_MAX_RAM_BASE)
#error "Bad HIFI4_SYS_RAM_BASE 1"
#endif
#if (HIFI4_SYS_RAM_BASE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad HIFI4_SYS_RAM_BASE 2"
#endif
#if (HIFI4_SYS_RAM_SIZE + HIFI4_SYS_RAM_SIZE > SYS_RAM_END)
#error "Bad HIFI4_SYS_RAM_SIZE 3"
#endif
#if (HIFI4_SYS_RAM_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad HIFI4_SYS_RAM_SIZE 4"
#endif

#if (HIFI4_SYS_RAM_BASE < SYS_RAM2_BASE)
#if (M55_DTCM_SIZE > 0) && ((SYS_RAM2_BASE - HIFI4_SYS_RAM_BASE) > (M55_DTCM_BASE - M55_MIN_DTCM_BASE))
#error "HIFI4 SYS RAM conflicts with M55 DTCM"
#endif
#if (HIFI4_DTCM_SIZE > 0) && ((SYS_RAM2_BASE - HIFI4_SYS_RAM_BASE) > (HIFI4_DTCM_BASE - HIFI4_MIN_DTCM_BASE))
#error "HIFI4 SYS RAM conflicts with HIFI4 DTCM"
#endif
#if (M55_DTCM_SIZE == 0) && (HIFI4_DTCM_SIZE == 0) && \
        ((SYS_RAM2_BASE - HIFI4_SYS_RAM_BASE + M55_ITCM_SIZE + HIFI4_ITCM_SIZE) > (SHR_RAM_BLK_SIZE * 16))
#error "HIFI4 SYS RAM conflicts with M55/HIFI4 ITCM"
#endif
#endif

#if (M55_SYS_RAM_SIZE > 0) && (HIFI4_SYS_RAM_SIZE > 0)
#if (M55_SYS_RAM_BASE == HIFI4_SYS_RAM_BASE)
#error "M55 SYS RAM conflicts with HIFI4 SYS RAM 1"
#elif (M55_SYS_RAM_BASE < HIFI4_SYS_RAM_BASE)
#if (M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE > HIFI4_SYS_RAM_BASE)
#error "M55 SYS RAM conflicts with HIFI4 SYS RAM 2"
#endif
#else /* (M55_SYS_RAM_BASE > HIFI4_SYS_RAM_BASE) */
#if (M55_SYS_RAM_BASE < HIFI4_SYS_RAM_BASE + HIFI4_SYS_RAM_SIZE)
#error "M55 SYS RAM conflicts with HIFI4 SYS RAM 3"
#endif
#endif
#endif

#if (BTH_RAM_BASE < BTH_MIN_RAM_BASE) || (BTH_RAM_BASE > BTH_MAX_RAM_BASE)
#error "Bad BTH_RAM_BASE 1"
#endif
#if (BTH_RAM_BASE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad BTH_RAM_BASE 2"
#endif
#if (BTH_RAM_SIZE + BTH_RAM_SIZE > BTH_RAM_END)
#error "Bad BTH_RAM_SIZE 3"
#endif
#if (BTH_RAM_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad BTH_RAM_SIZE 4"
#endif

#if (SENS_RAM_BASE < SENS_MIN_RAM_BASE) || (SENS_RAM_BASE > SENS_MAX_RAM_BASE)
#error "Bad SENS_RAM_BASE 1"
#endif
#if (SENS_RAM_BASE & (SHR_RAM_BLK_SIZE - 1))
#error "Bad SENS_RAM_BASE 2"
#endif
#if (SENS_RAM_SIZE + SENS_RAM_SIZE > SENS_RAM_END)
#error "Bad SENS_RAM_SIZE 3"
#endif

#if (M55_SYS_RAM_SIZE > 0) && (M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE > SYS_RAM2_BASE)
#if (M55_SYS_RAM_BASE < SYS_RAM2_BASE)
#define M55_SHR_RAM_START                       SYS_RAM2_BASE
#else
#define M55_SHR_RAM_START                       M55_SYS_RAM_BASE
#endif
#define M55_SHR_RAM_END                         (M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE)
#else
#define M55_SHR_RAM_START                       (SYS_RAM3_BASE + SHR_RAM_BLK_SIZE * 4)
#define M55_SHR_RAM_END                         M55_SHR_RAM_START
#endif

#if (HIFI4_SYS_RAM_SIZE > 0) && (HIFI4_SYS_RAM_BASE + HIFI4_SYS_RAM_SIZE > SYS_RAM2_BASE)
#if (HIFI4_SYS_RAM_BASE < SYS_RAM2_BASE)
#define HIFI4_SHR_RAM_START                     SYS_RAM2_BASE
#else
#define HIFI4_SHR_RAM_START                     HIFI4_SYS_RAM_BASE
#endif
#define HIFI4_SHR_RAM_END                       (HIFI4_SYS_RAM_BASE + HIFI4_SYS_RAM_SIZE)
#else
#define HIFI4_SHR_RAM_START                     (SYS_RAM3_BASE + SHR_RAM_BLK_SIZE * 4)
#define HIFI4_SHR_RAM_END                       HIFI4_SHR_RAM_START
#endif

#if (M55_SHR_RAM_START < HIFI4_SHR_RAM_START)
#define M55_HIFI4_SHR_RAM0_START                M55_SHR_RAM_START
#define M55_HIFI4_SHR_RAM0_END                  M55_SHR_RAM_END
#if (HIFI4_SHR_RAM_START == HIFI4_SHR_RAM_END)
#define M55_HIFI4_SHR_RAM1_START                M55_SHR_RAM_END
#define M55_HIFI4_SHR_RAM1_END                  M55_SHR_RAM_END
#else
#define M55_HIFI4_SHR_RAM1_START                HIFI4_SHR_RAM_START
#define M55_HIFI4_SHR_RAM1_END                  HIFI4_SHR_RAM_END
#endif
#else
#define M55_HIFI4_SHR_RAM0_START                HIFI4_SHR_RAM_START
#define M55_HIFI4_SHR_RAM0_END                  HIFI4_SHR_RAM_END
#if (M55_SHR_RAM_START == M55_SHR_RAM_END)
#define M55_HIFI4_SHR_RAM1_START                HIFI4_SHR_RAM_END
#define M55_HIFI4_SHR_RAM1_END                  HIFI4_SHR_RAM_END
#else
#define M55_HIFI4_SHR_RAM1_START                M55_SHR_RAM_START
#define M55_HIFI4_SHR_RAM1_END                  M55_SHR_RAM_END
#endif
#endif

#define SHR_RAM_USED_BY_SYS                     (M55_HIFI4_SHR_RAM0_END - M55_HIFI4_SHR_RAM0_START + \
                                                M55_HIFI4_SHR_RAM1_END - M55_HIFI4_SHR_RAM1_START)

#if (BTH_RAM_SIZE > 0) && (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM2_BASE)
#define SHR_RAM_USED_BY_BTH                     (BTH_RAM_BASE + BTH_RAM_SIZE - BTH_RAM2_BASE)
#else
#define SHR_RAM_USED_BY_BTH                     0
#endif

#if (SENS_RAM_SIZE > 0) && (SENS_RAM_BASE < SENS_RAM4_BASE)
#define SHR_RAM_USED_BY_SENS                    (SENS_RAM4_BASE - SENS_RAM_BASE)
#else
#define SHR_RAM_USED_BY_SENS                    0
#endif

#if (BTC_ROM_RAM_SIZE > 0) && ((BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE) < SHR_RAM_BLK_SIZE * 10)
#if ((BTC_ROM_RAM_BASE + BTC_ROM_RAM_SIZE - BTC_MIN_ROM_RAM_BASE) >= SHR_RAM_BLK_SIZE * 10)
#define SHR_RAM_USED_BY_BTC                     (SHR_RAM_BLK_SIZE * 10 - (BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE))
#else
#define SHR_RAM_USED_BY_BTC                     BTC_ROM_RAM_SIZE
#endif
#else
#define SHR_RAM_USED_BY_BTC                     0
#endif

#if (SHR_RAM_USED_BY_SYS + SHR_RAM_USED_BY_BTH + SHR_RAM_USED_BY_SENS + SHR_RAM_USED_BY_BTC > SHR_RAM_TOTAL_SIZE)
#error "Shared RAM not enough"
#endif

#if (SHR_RAM_USED_BY_SYS > 0)
    /* Only BTH can make SYS use alt shared ram. BTC cannot do that (for simplicity). */
    #if (M55_HIFI4_SHR_RAM0_START - SYS_RAM2_BASE >= SHR_RAM_USED_BY_BTH)
        #define ALT_SHR_RAM_USED_BY_SYS         0
    #else
        #if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM5_BASE) && \
                (((M55_HIFI4_SHR_RAM0_START < SYS_RAM3_BASE) && (M55_HIFI4_SHR_RAM0_END >= SYS_RAM3_BASE)) || \
                ((M55_HIFI4_SHR_RAM1_START < SYS_RAM3_BASE) && (M55_HIFI4_SHR_RAM1_END >= SYS_RAM3_BASE)))
            #error "M55/HIFI4 SYS shared RAM conflicts with BTH shared RAM"
        #elif (M55_HIFI4_SHR_RAM1_START < SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 3)
            /* Both shared RAM 0 and 1 are using alternative ram */
            #if (M55_HIFI4_SHR_RAM1_END < SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 3)
                #define ALT_SHR_RAM_USED_BY_SYS (M55_HIFI4_SHR_RAM1_END - M55_HIFI4_SHR_RAM0_START)
            #else
                #define ALT_SHR_RAM_USED_BY_SYS (SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 3 - M55_HIFI4_SHR_RAM0_START)
            #endif
        #elif (M55_HIFI4_SHR_RAM0_START < SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 3)
            /* Shared RAM 0 is using alternative ram */
            #if (M55_HIFI4_SHR_RAM0_END < SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 3)
                #define ALT_SHR_RAM_USED_BY_SYS (M55_HIFI4_SHR_RAM0_END - M55_HIFI4_SHR_RAM0_START)
            #else
                #define ALT_SHR_RAM_USED_BY_SYS (SYS_RAM2_BASE + SHR_RAM_BLK_SIZE * 3 - M55_HIFI4_SHR_RAM0_START)
            #endif
        #else
            #define ALT_SHR_RAM_USED_BY_SYS     0
        #endif
    #endif
    #if (ALT_SHR_RAM_USED_BY_SYS > 0)
        #if (M55_HIFI4_SHR_RAM0_START + ALT_SHR_RAM_USED_BY_SYS - SYS_RAM2_BASE + SHR_RAM_USED_BY_SENS > SHR_RAM_BLK_SIZE * 3)
            #error "M55/HIFI4 ALT SYS shared RAM conflicts with SENS shared RAM"
        #endif
    #else
        #if (M55_HIFI4_SHR_RAM1_END > SYS_RAM3_BASE + SHR_RAM_BLK_SIZE) && (SENS_RAM_BASE < SENS_RAM2_BASE + SHR_RAM_BLK_SIZE)
            #if (((M55_HIFI4_SHR_RAM1_END - (SYS_RAM3_BASE + SHR_RAM_BLK_SIZE)) + \
                            (SENS_RAM2_BASE + SHR_RAM_BLK_SIZE - SENS_RAM_BASE)) > SHR_RAM_BLK_SIZE * 3)
                #error "M55/HIFI4 SYS shared RAM conflicts with SENS shared RAM"
            #endif
        #endif
    #endif
#endif

#if (BTC_ROM_RAM_BASE < BTC_MIN_ROM_RAM_BASE || BTC_ROM_RAM_BASE > (BTC_ROM_RAM_BASE + BTC_MAX_ROM_RAM_SIZE))
#error "BTC ROM RAM should be used along with BTC ROM RAM"
#endif
#if (BTC_ROM_RAM_SIZE > BTC_MAX_ROM_RAM_SIZE)
#error "BTC ROM RAM out of range"
#endif

#if (BTC_ROM_RAM_SIZE > 0)
    #if (BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE < SHR_RAM_BLK_SIZE * 10) && \
            (BTC_ROM_RAM_BASE + BTC_ROM_RAM_SIZE - BTC_MIN_ROM_RAM_BASE < SHR_RAM_BLK_SIZE * 10) && \
            (BTC_ROM_RAM_SIZE & (SHR_RAM_BLK_SIZE - 1))
        #error "Bad BTC_ROM_RAM_SIZE"
    #endif
    #if (BTH_RAM_SIZE > 0) && (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM3_BASE)
        #if ((BTH_RAM_BASE + BTH_RAM_SIZE - BTH_RAM3_BASE) > (BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE))
            #error "BTC shared RAM conflicts BTH shared RAM"
        #endif
    #endif
    #if (ALT_SHR_RAM_USED_BY_SYS > 0)
        #if !(BTC_ROM_RAM_BASE + BTC_ROM_RAM_SIZE - BTC_MIN_ROM_RAM_BASE <= SHR_RAM_BLK_SIZE * 7 + M55_HIFI4_SHR_RAM0_START - SYS_RAM2_BASE) &&
                !(BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE >= SHR_RAM_BLK_SIZE * 7 + M55_HIFI4_SHR_RAM0_START + ALT_SHR_RAM_USED_BY_SYS - SYS_RAM2_BASE)
            #error "BTC shared RAM conflicts M55/HIFI4 ALT SYS shared RAM"
        #endif
    #else
        #if (SHR_RAM_USED_BY_SYS > 0) && (M55_HIFI4_SHR_RAM0_START >= SYS_RAM2_BASE + SHR_RAM_BLK_SIZE) && \
                ((BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE) < SHR_RAM_BLK_SIZE * 7)
            #if ((M55_HIFI4_SHR_RAM1_END - (SYS_RAM2_BASE + SHR_RAM_BLK_SIZE)) > (BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE))
                #error "BTC shared RAM conflicts M55/HIFI4 SYS shared RAM"
            #endif
        #endif
    #endif
    #if (SENS_RAM_SIZE > 0) && (SENS_RAM_BASE < SENS_RAM4_BASE) && \
            ((BTC_ROM_RAM_BASE - BTC_MIN_ROM_RAM_BASE) < SHR_RAM_BLK_SIZE * 10)
        #if ((BTC_ROM_RAM_BASE + BTC_ROM_RAM_SIZE - BTC_MIN_ROM_RAM_BASE) >= SHR_RAM_BLK_SIZE * 10)
            #error "BTC shared RAM conflicts SENS shared RAM 1"
        #elif ((SHR_RAM_BLK_SIZE * 10 - (BTC_ROM_RAM_BASE + BTC_ROM_RAM_SIZE - BTC_MIN_ROM_RAM_BASE)) < (SENS_RAM4_BASE - SENS_RAM_BASE))
            #error "BTC shared RAM conflicts SENS shared RAM 2"
        #endif
    #endif
#endif

#endif /* LINKER_SCRIPT */

/* Chip subtype definitions */

#ifdef CHIP_SUBSYS_BTH

#include CHIP_SPECIFIC_HDR(plat_addr_map_bth)

#elif defined(CHIP_SUBSYS_SENS)

#include CHIP_SPECIFIC_HDR(plat_addr_map_sens)

#else

#include CHIP_SPECIFIC_HDR(plat_addr_map_sys)

#endif

/* Retention RAM config check */

#ifdef CORE_SLEEP_POWER_DOWN
#if defined(M55_ITCMRET_SIZE) && (M55_ITCMRET_SIZE > 0)
#if (M55_ITCMRET_BASE < M55_ITCM_BASE) || (M55_ITCMRET_BASE + M55_ITCMRET_SIZE > M55_ITCM_BASE + M55_ITCM_SIZE)
#error "Bad M55_ITCMRET_BASE or M55_ITCMRET_SIZE"
#endif
#if (M55_ITCMRET_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "M55_ITCMRET_SIZE should align with SHR_RAM_BLK_SIZE"
#endif
#endif

#if defined(M55_DTCMRET_SIZE) && (M55_DTCMRET_SIZE > 0)
#if (M55_DTCMRET_BASE < M55_DTCM_BASE) || (M55_DTCMRET_BASE + M55_DTCMRET_SIZE > M55_DTCM_BASE + M55_DTCM_SIZE)
#error "Bad M55_DTCMRET_BASE or M55_DTCMRET_SIZE"
#endif
#if (M55_DTCMRET_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "M55_DTCMRET_SIZE should align with SHR_RAM_BLK_SIZE"
#endif
#endif

#if defined(HIFI4_ITCMRET_SIZE) && (HIFI4_ITCMRET_SIZE > 0)
#if (HIFI4_ITCMRET_BASE < HIFI4_ITCM_BASE) || (HIFI4_ITCMRET_BASE + HIFI4_ITCMRET_SIZE > HIFI4_ITCM_BASE + HIFI4_ITCM_SIZE)
#error "Bad HIFI4_ITCMRET_BASE or HIFI4_ITCMRET_SIZE"
#endif
#if (HIFI4_ITCMRET_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "HIFI4_ITCMRET_SIZE should align with SHR_RAM_BLK_SIZE"
#endif
#endif

#if defined(HIFI4_DTCMRET_SIZE) && (HIFI4_DTCMRET_SIZE > 0)
#if (HIFI4_DTCMRET_BASE < HIFI4_DTCM_BASE) || (HIFI4_DTCMRET_BASE + HIFI4_DTCMRET_SIZE > HIFI4_DTCM_BASE + HIFI4_DTCM_SIZE)
#error "Bad HIFI4_DTCMRET_BASE or HIFI4_DTCMRET_SIZE"
#endif
#if (HIFI4_DTCMRET_SIZE & (SHR_RAM_BLK_SIZE * 2 - 1))
#error "HIFI4_DTCMRET_SIZE should align with SHR_RAM_BLK_SIZE * 2"
#endif
#endif

#if defined(M55_SYS_RAMRET_SIZE) && (M55_SYS_RAMRET_SIZE > 0)
#if (M55_SYS_RAMRET_BASE < M55_SYS_RAM_BASE) || (M55_SYS_RAMRET_BASE + M55_SYS_RAMRET_SIZE > M55_SYS_RAM_BASE + M55_SYS_RAM_SIZE)
#error "Bad M55_SYS_RAMRET_BASE or M55_SYS_RAMRET_SIZE"
#endif
#if (M55_SYS_RAMRET_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "M55_SYS_RAMRET_SIZE should align with SHR_RAM_BLK_SIZE"
#endif
#endif

#if defined(HIFI4_SYS_RAMRET_SIZE) && (HIFI4_SYS_RAMRET_SIZE > 0)
#if (HIFI4_SYS_RAMRET_BASE < HIFI4_SYS_RAM_BASE) || (HIFI4_SYS_RAMRET_BASE + HIFI4_SYS_RAMRET_SIZE > HIFI4_SYS_RAM_BASE + HIFI4_SYS_RAM_SIZE)
#error "Bad HIFI4_SYS_RAMRET_BASE or HIFI4_SYS_RAMRET_SIZE"
#endif
#if (HIFI4_SYS_RAMRET_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "HIFI4_SYS_RAMRET_SIZE should align with SHR_RAM_BLK_SIZE"
#endif
#endif

#if defined(SENS_RAMRET_SIZE) && (SENS_RAMRET_SIZE > 0)
#if (SENS_RAMRET_BASE < SENS_RAM_BASE) || (SENS_RAMRET_BASE + SENS_RAMRET_SIZE > SENS_RAM_BASE + SENS_RAM_SIZE)
#error "Bad SENS_RAMRET_BASE or SENS_RAMRET_SIZE"
#endif
#if (SENS_RAMRET_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "SENS_RAMRET_SIZE should align with SHR_RAM_BLK_SIZE"
#endif
#endif

#if defined(BTH_RAMRET_SIZE) && (BTH_RAMRET_SIZE > 0)
#if (BTH_RAMRET_BASE < BTH_RAM_BASE) || (BTH_RAMRET_BASE + BTH_RAMRET_SIZE > BTH_RAM_BASE + BTH_RAM_SIZE)
#error "Bad BTH_RAMRET_BASE or BTH_RAMRET_SIZE"
#endif
#if (BTH_RAMRET_SIZE & (SHR_RAM_BLK_SIZE - 1))
#error "BTH_RAMRET_SIZE should align with SHR_RAM_BLK_SIZE"
#endif
#endif
#endif

/* For linker scripts */

#if defined(CHIP_SUBSYS_BTH)
#define VECTOR_SECTION_SIZE                     380
#elif defined(CHIP_SUBSYS_SENS)
#define VECTOR_SECTION_SIZE                     320
#else
#define VECTOR_SECTION_SIZE                     360
#endif
#define REBOOT_PARAM_SECTION_SIZE               64
#define ROM_BUILD_INFO_SECTION_SIZE             40
#define ROM_EXPORT_FN_SECTION_SIZE              128
#define BT_INTESYS_MEM_OFFSET                   0x00004000

/* For module features */
#define CODEC_FREQ_CRYSTAL                      CODEC_FREQ_24M
#define CODEC_FREQ_EXTRA_DIV                    2
#define CODEC_PLL_DIV                           16
#ifdef AUD_PLL_DOUBLE
#define CODEC_CMU_DIV                           16
#else
#define CODEC_CMU_DIV                           8
#endif
#define CODEC_PLAYBACK_BIT_DEPTH                24
#define CODEC_HAS_FIR
#define CODEC_ECHO_PATH_VER                     2
#define GPADC_CTRL_VER                          5
#define GPADC_DYNAMIC_DATA_BITS
#define GPADC_HAS_VSYS_DIV
#define GPADC_HAS_EXT_SLOPE_CAL2
#define GPADC_CUSTOM_CHAN_IRQ_CLEAR
#define GPADC_CUSTOM_CHAN_ENABLE
#define SEC_ENG_HAS_HASH
#define DCDC_CLOCK_CONTROL
#define CMU_FREE_TIMER
#define CMU_FAST_TIMER_FREQ
#ifndef AUD_SECTION_STRUCT_VERSION
#define AUD_SECTION_STRUCT_VERSION              4
#endif
#define RSA_RAW_KEY_ENABLE

/* For boot struct version */
#ifndef SECURE_BOOT_VER
#define SECURE_BOOT_VER                         4
#endif

/* For ROM export functions */
#define NO_MEMMOVE
#define NO_EXPORT_QSORT
#define NO_EXPORT_BSEARCH
#define NO_EXPORT_VSSCANF

#ifdef __cplusplus
}
#endif

#endif
