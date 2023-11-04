/***************************************************************************
 *
 * Copyright 2021-2022 BES.
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
#ifndef __PLAT_RAM_CFG_BEST1603_H__
#define __PLAT_RAM_CFG_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

/* RAM config */

#ifdef SLEEP_PD_CURRENT_TEST
#define BTH_RAM_BASE                            BTH_RAM2_BASE
#ifndef BTH_RAM_SIZE
#define BTH_RAM_SIZE                            (SHR_RAM_BLK_SIZE * 1)
#endif
#define BTH_RAMRET_BASE                         BTH_RAM_BASE
#define BTH_RAMRET_SIZE                         BTH_RAM_SIZE

#define MAIN_RAM_USE_SYS_RAM
#define M55_SYS_RAM_BASE                        (SYS_RAM2_BASE + SHR_RAM_BLK_SIZE)
#define M55_SYS_RAM_SIZE                        (SHR_RAM_BLK_SIZE * 1)
#define M55_SYS_RAMRET_BASE                     M55_SYS_RAM_BASE
#define M55_SYS_RAMRET_SIZE                     M55_SYS_RAM_SIZE
#endif

#ifndef M55_ITCM_BASE
#define M55_ITCM_BASE                           M55_MIN_ITCM_BASE
#endif
#ifndef M55_ITCM_SIZE
#define M55_ITCM_SIZE                           0x00080000
#endif
#ifndef M55_DTCM_BASE
#define M55_DTCM_BASE                           M55_MIN_DTCM_BASE
#endif
#ifndef M55_DTCM_SIZE
#define M55_DTCM_SIZE                           (M55_DTCM_END - M55_DTCM_BASE - M55_ITCM_SIZE)
#endif

#ifndef HIFI4_ITCM_BASE
#define HIFI4_ITCM_BASE                         HIFI4_MIN_ITCM_BASE
#endif
#ifndef HIFI4_ITCM_SIZE
#define HIFI4_ITCM_SIZE                         0x00000000
#endif
#ifndef HIFI4_DTCM_BASE
#define HIFI4_DTCM_BASE                         HIFI4_MIN_DTCM_BASE
#endif
#ifndef HIFI4_DTCM_SIZE
#define HIFI4_DTCM_SIZE                         0x00000000
#endif

#ifndef BTC_ROM_RAM_BASE
#define BTC_ROM_RAM_BASE                        BTC_MIN_ROM_RAM_BASE
#endif

#ifndef BTC_ROM_RAM_SIZE
#define BTC_ROM_RAM_SIZE                        0
#endif

#ifndef BTH_RAM_BASE
#define BTH_RAM_BASE                            BTH_RAM0_BASE
#endif
#ifndef BTH_RAM_SIZE
#define BTH_RAM_SIZE                            (BTH_RAM2_BASE - BTH_RAM_BASE)
#endif

#ifndef SENS_RAM_BASE
#define SENS_RAM_BASE                           (SENS_RAM_END - SENS_RAM_SIZE)
#endif
#ifndef SENS_RAM_SIZE
#ifdef LARGE_SENS_RAM
#define SENS_RAM_SIZE                           (SENS_RAM_END - SENS_RAM3_BASE)
#elif (BTC_ROM_RAM_SIZE > 0)
#define SENS_RAM_SIZE                           (SENS_RAM_END - SENS_RAM4_BASE)
#elif (BTH_RAM_BASE + BTH_RAM_SIZE <= BTH_RAM2_BASE)
#define SENS_RAM_SIZE                           (SENS_RAM_END - (SENS_RAM2_BASE + SHR_RAM_BLK_SIZE))
#elif (BTH_RAM_BASE + BTH_RAM_SIZE - BTH_RAM2_BASE < (SHR_RAM_BLK_SIZE * 3))
#define SENS_RAM_SIZE                           (SENS_RAM_END - (SENS_RAM2_BASE + SHR_RAM_BLK_SIZE + (BTH_RAM_BASE + BTH_RAM_SIZE - BTH_RAM2_BASE)))
#else
#define SENS_RAM_SIZE                           (SENS_RAM_END - SENS_RAM4_BASE)
#endif
#endif

#if (BTH_RAM_BASE + BTH_RAM_SIZE > BTH_RAM2_BASE)
#define SYS_BTH_SHR_RAM_END                     (SYS_RAM2_BASE + (BTH_RAM_BASE + BTH_RAM_SIZE - BTH_RAM2_BASE))
#else
#define SYS_BTH_SHR_RAM_END                     SYS_RAM2_BASE
#endif

#if (SENS_RAM_BASE < SENS_RAM4_BASE)
#define SYS_SENS_SHR_RAM_START                  (SYS_RAM_END - (SENS_RAM4_BASE - SENS_RAM_BASE))
#else
#define SYS_SENS_SHR_RAM_START                  SYS_RAM_END
#endif

#ifndef M55_SYS_RAM_BASE
#define M55_SYS_RAM_BASE                        SYS_BTH_SHR_RAM_END
#endif

#ifndef M55_SYS_RAM_SIZE
#if (BTC_ROM_RAM_SIZE > 0)
#define M55_SYS_RAM_SIZE                        0
#else
#define M55_SYS_RAM_SIZE                        (SYS_SENS_SHR_RAM_START - M55_SYS_RAM_BASE)
#endif
#endif

#ifndef HIFI4_SYS_RAM_BASE
#define HIFI4_SYS_RAM_BASE                      SYS_BTH_SHR_RAM_END
#endif

#ifndef HIFI4_SYS_RAM_SIZE
#define HIFI4_SYS_RAM_SIZE                      0
#endif

#ifdef __cplusplus
}
#endif

#endif
