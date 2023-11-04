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
#ifndef __REG_AONSEC_BEST1603_H__
#define __REG_AONSEC_BEST1603_H__

#include "plat_types.h"

struct AONSEC_T {
    __IO uint32_t NONSEC_I2C;           // 0x00
    __IO uint32_t NONSEC_PROT;          // 0x04
    __IO uint32_t NONSEC_BYPASS_PROT;   // 0x08
    __IO uint32_t SEC2NSEC_PROT;        // 0x0C
    __IO uint32_t SEC_BOOT_ACC;         // 0x10
    __IO uint32_t CORE_VTOR;            // 0x14
};

// reg_00
#define AON_SEC_CFG_NONSEC_I2C_SLV                          (1 << 0)
#define AON_SEC_CFG_NONSEC_SENS                             (1 << 1)
#define AON_SEC_CFG_NONSEC_BTH                              (1 << 2)

// reg_04
#define AON_SEC_CFG_NONSEC_PROT_APB0(n)                     (((n) & 0xFFFFFFFF) << 0)
#define AON_SEC_CFG_NONSEC_PROT_APB0_MASK                   (0xFFFFFFFF << 0)
#define AON_SEC_CFG_NONSEC_PROT_APB0_SHIFT                  (0)

// reg_08
#define AON_SEC_CFG_NONSEC_BYPASS_PROT_APB0(n)              (((n) & 0xFFFFFFFF) << 0)
#define AON_SEC_CFG_NONSEC_BYPASS_PROT_APB0_MASK            (0xFFFFFFFF << 0)
#define AON_SEC_CFG_NONSEC_BYPASS_PROT_APB0_SHIFT           (0)

// reg_0c
#define AON_SEC_CFG_SEC2NSEC_PROT_APB0(n)                   (((n) & 0xFFFFFFFF) << 0)
#define AON_SEC_CFG_SEC2NSEC_PROT_APB0_MASK                 (0xFFFFFFFF << 0)
#define AON_SEC_CFG_SEC2NSEC_PROT_APB0_SHIFT                (0)

// reg_10
#define AON_SEC_SECURE_BOOT_JTAG                            (1 << 0)
#define AON_SEC_SECURE_BOOT_I2C                             (1 << 1)
#define AON_SEC_CFG_NONSEC_APB_PROT                         (1 << 2)

// reg_14
#define AON_SEC_VTOR_CORE_SYS(n)                            (((n) & 0x1FFFFFF) << 7)
#define AON_SEC_VTOR_CORE_SYS_MASK                          (0x1FFFFFF << 7)
#define AON_SEC_VTOR_CORE_SYS_SHIFT                         (7)

#endif

