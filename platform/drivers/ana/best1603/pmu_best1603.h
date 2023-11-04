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
#ifndef __PMU_BEST1603_H__
#define __PMU_BEST1603_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_cmu.h"
#include "hal_gpio.h"

#define EXTPMU_REG(r)                       (((r) & 0xFFF) | 0x0000)
#define ANA_REG(r)                          (((r) & 0xFFF) | 0x1000)
#define RF_REG(r)                           (((r) & 0xFFF) | 0x2000)
#define PMU_ANA_REG(r)                      (((r) & 0xFFF) | 0x4000)

#define MAX_ITN_VMIC_CH_NUM                 2
#define MAX_EXT_VMIC_CH_NUM                 3
#define MAX_EXT_VMIC_CH_NUM_1806            2

#ifdef NO_EXT_PMU
#define MAX_VMIC_CH_NUM                     MAX_ITN_VMIC_CH_NUM
#else
#define MAX_VMIC_CH_NUM                     MAX_EXT_VMIC_CH_NUM
#endif

enum PMU_EFUSE_PAGE_T {
    PMU_EFUSE_PAGE_SECURITY         = 0,
    PMU_EFUSE_PAGE_BOOT             = 1,
    PMU_EFUSE_PAGE_FEATURE          = 2,
    PMU_EFUSE_PAGE_BATTER_LV        = 3,

    PMU_EFUSE_PAGE_BATTER_HV        = 4,
    PMU_EFUSE_PAGE_SW_CFG           = 5,
    PMU_EFUSE_PAGE_CODE_VER         = 6,
    PMU_EFUSE_PAGE_DCDC_CALIB       = 7,

    PMU_EFUSE_PAGE_LDO_CALIB        = 8,
    PMU_EFUSE_PAGE_DCDC_SMALL_BG    = 9,
    PMU_EFUSE_PAGE_LDO_SMALL_BG     = 10,
    PMU_EFUSE_PAGE_DCCALIB_L        = 11,

    PMU_EFUSE_PAGE_DCCALIB_L_LP     = 12,
    PMU_EFUSE_PAGE_RESERVED_13      = 13,
    PMU_EFUSE_PAGE_SAR_VREF         = 14,
    PMU_EFUSE_PAGE_TEMPERATURE      = 15,

    PMU_EFUSE_PAGE_GPADC_LV         = 0x10,
    PMU_EFUSE_PAGE_GPADC_MV         = 0x11,
    PMU_EFUSE_PAGE_GPADC_HV         = 0x12,
    PMU__EFUSE_PAGE_RESERVED_19     = 0x13,

    PMU__EFUSE_PAGE_RESERVED_20     = 0x14,
    PMU__EFUSE_PAGE_RESERVED_21     = 0x15,
    PMU__EFUSE_PAGE_RESERVED_22     = 0x16,
    PMU__EFUSE_PAGE_RESERVED_23     = 0x17,

    PMU__EFUSE_PAGE_RESERVED_24     = 0x18,
    PMU__EFUSE_PAGE_XTAL_BUF_RC     = 0x19,
    PMU__EFUSE_PAGE_XTAL_X2X4_RC    = 0x1A,
    PMU__EFUSE_PAGE_RESERVED_27     = 0x1B,

    PMU__EFUSE_PAGE_XTAL_BUF_RC_SSC = 0x1C,
    PMU__EFUSE_PAGE_RESERVED_29     = 0x1D,
    PMU__EFUSE_PAGE_RESERVED_30     = 0x1E,
    PMU__EFUSE_PAGE_RESERVED_31     = 0x1F,
};

enum PMU_EXT_EFUSE_PAGE_T {
    PMU_EXT_EFUSE_PAGE_RESERVED_0   = 0,
    PMU_EXT_EFUSE_PAGE_RESERVED_1   = 1,
    PMU_EXT_EFUSE_PAGE_RESERVED_2   = 2,
    PMU_EXT_EFUSE_PAGE_RESERVED_3   = 3,

    PMU_EXT_EFUSE_PAGE_RESERVED_4   = 4,
    PMU_EXT_EFUSE_PAGE_RESERVED_5   = 5,
    PMU_EXT_EFUSE_PAGE_RESERVED_6   = 6,
    PMU_EXT_EFUSE_PAGE_RESERVED_7   = 7,

    PMU_EXT_EFUSE_PAGE_LDO_VCORE    = 8,
    PMU_EXT_EFUSE_PAGE_RESERVED_9   = 9,
    PMU_EXT_EFUSE_PAGE_RESERVED_10  = 10,
    PMU_EXT_EFUSE_PAGE_RESERVED_11  = 11,

    PMU_EXT_EFUSE_PAGE_RESERVED_12  = 12,
    PMU_EXT_EFUSE_PAGE_RESERVED_13  = 13,
    PMU_EXT_EFUSE_PAGE_RESERVED_14  = 14,
    PMU_EXT_EFUSE_PAGE_RESERVED_15  = 15,

    PMU_EXT_EFUSE_PAGE_RESERVED_16  = 16,
    PMU_EXT_EFUSE_PAGE_RESERVED_17  = 17,
    PMU_EXT_EFUSE_PAGE_RESERVED_18  = 18,
    PMU_EXT_EFUSE_PAGE_RESERVED_19  = 19,

    PMU_EXT_EFUSE_PAGE_RESERVED_20  = 20,
    PMU_EXT_EFUSE_PAGE_RESERVED_21  = 21,
    PMU_EXT_EFUSE_PAGE_RESERVED_22  = 22,
    PMU_EXT_EFUSE_PAGE_RESERVED_23  = 23,
};

enum PMU_IRQ_TYPE_T {
    PMU_IRQ_TYPE_PWRKEY,
    PMU_IRQ_TYPE_GPADC,
    PMU_IRQ_TYPE_RTC,
    PMU_IRQ_TYPE_CHARGER,
    PMU_IRQ_TYPE_GPIO,
    PMU_IRQ_TYPE_POWER_ABORT,
    PMU_IRQ_TYPE_CAPSENSOR,

    PMU_IRQ_TYPE_QTY
};

enum PMU_PLL_DIV_TYPE_T {
    PMU_PLL_DIV_DIG,
    PMU_PLL_DIV_CODEC,
};

enum PMU_CHIP_TYPE_T {
    PMU_CHIP_TYPE_1803,
    PMU_CHIP_TYPE_1805,
    PMU_CHIP_TYPE_1806,
    PMU_CHIP_TYPE_1808,
    PMU_CHIP_TYPE_1809,
};

enum PMU_BIG_BANDGAP_USER_T {
    PMU_BIG_BANDGAP_USER_GPADC              = (1 << 0),
    PMU_BIG_BANDGAP_USER_VMIC1              = (1 << 1),
    PMU_BIG_BANDGAP_USER_VMIC2              = (1 << 2),
    PMU_BIG_BANDGAP_USER_SAR_VREF_OUTPUT    = (1 << 3),
};

enum PMU_GPIO_IRQ_TYPE_T {
    PMU_GPIO_IRQ_FALLING = 0,
    PMU_GPIO_IRQ_RISING,
    PMU_GPIO_IRQ_LOW_LEVEL,
    PMU_GPIO_IRQ_HIGH_LEVEL,
    PMU_GPIO_IRQ_NONE,
};

enum PMU_POWER_PROTECT_MODULE_T {
    PMU_POWER_PROTECT_MODULE_LDO_VBAT2VCORE = 0,
    PMU_POWER_PROTECT_MODULE_LDO_VCORE_L1   = 1,
    PMU_POWER_PROTECT_MODULE_LDO_VANA       = 2,
    PMU_POWER_PROTECT_MODULE_LDO_VIO        = 3,
    PMU_POWER_PROTECT_MODULE_LDO_VMEM       = 4,
    PMU_POWER_PROTECT_MODULE_LDO_VUSB       = 5,
    PMU_POWER_PROTECT_MODULE_LDO_VMEM2      = 6,
    PMU_POWER_PROTECT_MODULE_LDO_VMEM3      = 7,
    PMU_POWER_PROTECT_MODULE_LDO_VSENSOR    = 8,
    PMU_POWER_PROTECT_MODULE_BUCK_VCORE     = 9,
    PMU_POWER_PROTECT_MODULE_BUCK_VANA      = 10,
    PMU_POWER_PROTECT_MODULE_BUCK_VHPPA     = 11,
    PMU_POWER_PROTECT_MODULE_VBAT           = 12,
    PMU_POWER_PROTECT_MODULE_VMICA          = 13,
    PMU_POWER_PROTECT_MODULE_VMICB          = 14,
    PMU_POWER_PROTECT_MODULE_OTP            = 15,

    PMU_POWER_PROTECT_MODULE_QTY,
};

enum PMU_POWER_PROTECT_MODE_T {
    PMU_POWER_PROTECT_MODE_HW,
    PMU_POWER_PROTECT_MODE_SW,
};

enum PMU_POWER_PROTECT_HW_TYPE_T {
    PMU_POWER_PROTECT_HW_TYPE_PMU_OFF,
    PMU_POWER_PROTECT_HW_TYPE_LDO_OFF,
};

enum PMU_POWER_PROTECT_ABORT_SOURCE_T {
    PMU_POWER_PROTECT_ABORT_SOURCE_UNKNOW   = 0,
    PMU_POWER_PROTECT_ABORT_SOURCE_OC       = (1 << 0),
    PMU_POWER_PROTECT_ABORT_SOURCE_SC       = (1 << 1),
    PMU_POWER_PROTECT_ABORT_SOURCE_UV       = (1 << 2),
    PMU_POWER_PROTECT_ABORT_SOURCE_OV       = (1 << 3),
};

enum PMU_POWER_PROTECT_RET_T {
    PMU_POWER_PROTECT_RET_VALID             = 0,
    PMU_POWER_PROTECT_RET_INVALID_CHIP      = -1,
    PMU_POWER_PROTECT_RET_INVALID_EFUSE     = -2,
    PMU_POWER_PROTECT_RET_INVALID_MODULE    = -3,
    PMU_POWER_PROTECT_RET_INVALID_OSCP      = -4,
    PMU_POWER_PROTECT_RET_INVALID_UV        = -5,
    PMU_POWER_PROTECT_RET_INVALID_OV        = -6,
    PMU_POWER_PROTECT_RET_INVALID_TEMP      = -7,
};

union SECURITY_VALUE_T {
    struct {
        unsigned short security_en      :1;
        unsigned short mode             :1;
        unsigned short sig_type         :1;
        unsigned short reserved         :1;
        unsigned short key_id           :2;
        unsigned short vendor_id        :5;
        unsigned short flash_id         :1;
        unsigned short chksum           :4;
    } root;
    struct {
        unsigned short security_en      :1;
        unsigned short mode             :1;
        unsigned short sig_type         :1;
        unsigned short skip_romkey      :1;
        unsigned short key_id           :2;
        unsigned short reg_base         :2;
        unsigned short reg_size         :2;
        unsigned short reg_offset       :1;
        unsigned short flash_id         :1;
        unsigned short chksum           :4;
    } otp;
    unsigned short reg;
};
#define SECURITY_VALUE_T                    SECURITY_VALUE_T

struct RTC_CALENDAR_FORMAT_T {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

struct PMU_POWER_PROTECT_TYPE_T {
    bool ov_det_en;
    bool uv_det_en;
    bool osc_det_en;
    bool irq_en;
};

enum HAL_FLASH_ID_T pmu_get_boot_flash_ctrl_id(void);

enum PMU_CHIP_TYPE_T pmu_get_chip_type(void);

void pmu_codec_hppa_enable(int enable);

uint32_t pmu_codec_mic_bias_remap(uint32_t map);

void pmu_codec_mic_bias_enable(uint32_t map, int enable);

void pmu_codec_mic_bias_noremap_enable(uint32_t map, int enable);

void pmu_codec_mic_bias_set_volt(uint32_t map, uint32_t mv);

void pmu_codec_mic_bias_lowpower_mode(uint32_t map, int enable);

void pmu_codec_mic_bias_noremap_lowpower_mode(uint32_t map, int enable);

int pmu_codec_volt_ramp_up(void);

int pmu_codec_volt_ramp_down(void);

void pmu_codec_vad_save_power(void);

void pmu_codec_vad_restore_power(void);

void pmu_led_set_hiz(enum HAL_GPIO_PIN_T pin);

void pmu_rf_ana_init(void);

void pmu_big_bandgap_enable(enum PMU_BIG_BANDGAP_USER_T user, int enable);

typedef void (*PMU_NTC_IRQ_HANDLER_T)(uint16_t raw_val, int temperature);

int pmu_ntc_capture_start(PMU_NTC_IRQ_HANDLER_T cb);

uint16_t pmu_ntc_temperature_reference_get(void);

uint16_t pmu_ntc_efuse_temperature_get(void);

uint16_t pmu_ntc_raw_get(void);

uint16_t pmu_ntc_temperature_get(void);

void pmu_vcorev_dig_enable(void);

void pmu_vcorev_dig_disable(void);

bool pmu_dig_set_volt_value(uint16_t dcdc_volt_mv, uint16_t logic_volt_mv, uint16_t ldo_volt_mv);

void bbpll_pll_update(uint64_t pll_cfg_val);

void bbpll_set_dig_div(uint32_t div);

void bbpll_auto_calib_ires(uint32_t codec_freq, uint32_t div);

void pmu_codec_vcm_enable(bool en);

void pmu_codec_vcm_enable_lpf(bool en);

void pmu_codec_lp_vcm(bool en);

void pmu_codec_set_vcm_low_vcm_lp(uint16_t v);

void pmu_codec_set_vcm_low_vcm_lpf(uint16_t v);

void pmu_codec_set_vcm_low_vcm(uint16_t v);

int pmu_volt2temperature(const uint16_t volt);

int pmu_external_crystal_ldo_ctrl(bool en);

void pmu_external_crystal_enable(void);

void pmu_sar_adc_vref_enable(void);

void pmu_sar_adc_vref_disable(void);

int pmu_gpadc_clear_chan_irq(unsigned short irq);

int pmu_gpadc_enable_chan(int ch, int enable);

void pmu_shipment_mode_enable(bool en);

void pmu_ldo_vusb_off_pull_down(void);

void pmu_ldo_vio_off_pull_down(void);

void pmu_ldo_vusb_on_lp_dr(bool dr_lp);

void pmu_ldo_vio_on_lp_dr(bool dr_lp);

void pmu_itn_psram_pu_ce(int dual);

void pmu_axi_freq_config(enum HAL_CMU_AXI_FREQ_T freq);

void pmu_vcore_logic_bypass_enable(bool en);

void pmu_power_key_hw_reset_enable(uint8_t seconds);

void pmu_power_key_long_press(bool en);

bool pmu_boot_first_power_up(void);

int pmu_wdt_load_test(void);

void pmu_hw_uvp_vcore_disable(void);

void pmu_hw_uvp_vcore_enable(uint16_t vcore_mv);

//=========================================================================
// APIs for pmu gpio
//=========================================================================

enum HAL_GPIO_DIR_T pmu_gpio_pin_get_dir(enum HAL_GPIO_PIN_T pin);

void pmu_gpio_pin_set_dir(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_DIR_T dir, uint8_t val_for_out);

void pmu_gpio_pin_set(enum HAL_GPIO_PIN_T pin);

void pmu_gpio_pin_clr(enum HAL_GPIO_PIN_T pin);

uint8_t pmu_gpio_pin_get_input_val(enum HAL_GPIO_PIN_T pin);

uint8_t pmu_gpio_pin_get_output_val(enum HAL_GPIO_PIN_T pin);

uint32_t pmu_gpio_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel);

uint32_t pmu_gpio_set_io_drv(enum HAL_IOMUX_PIN_T pin, uint32_t val);

int pmu_gpio_irq_init(void);

uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg);

enum PMU_GPIO_IRQ_TYPE_T pmu_gpio_irq_type_get(void);

//=========================================================================
// APIs for pmu rtc
//=========================================================================

typedef void (*PMU_RTC_CALENDAR_IRQ_HANDLER_T)(struct RTC_CALENDAR_FORMAT_T *calendar);

int pmu_rtc_calendar_alarm_irq_handler_set(PMU_RTC_CALENDAR_IRQ_HANDLER_T handler);

int pmu_rtc_calendar_alarm_clear(void);

int pmu_rtc_calendar_alarm_set(struct RTC_CALENDAR_FORMAT_T *alarm_set);

int pmu_rtc_calendar_alarm_get(struct RTC_CALENDAR_FORMAT_T *alarm_get);

int pmu_rtc_calendar_set(struct RTC_CALENDAR_FORMAT_T *calendar_set);

int pmu_rtc_calendar_get(struct RTC_CALENDAR_FORMAT_T *calendar_get);

int pmu_rtc_calendar_pwron_enable(bool en);

//=========================================================================
// APIs for pmu power protect
//=========================================================================

typedef void (*PMU_POWER_PROTECT_ABORT_IRQ_HANDLER_T)(enum PMU_POWER_PROTECT_MODULE_T module, enum PMU_POWER_PROTECT_ABORT_SOURCE_T source);

enum PMU_POWER_PROTECT_RET_T pmu_power_protect_config(enum PMU_POWER_PROTECT_MODULE_T module,
                            enum PMU_POWER_PROTECT_MODE_T mode,
                            enum PMU_POWER_PROTECT_HW_TYPE_T type,
                            const struct PMU_POWER_PROTECT_TYPE_T *cfg);

enum PMU_POWER_PROTECT_RET_T pmu_power_protect_irq_handler_set(PMU_POWER_PROTECT_ABORT_IRQ_HANDLER_T handler);

enum PMU_POWER_PROTECT_ABORT_SOURCE_T pmu_power_protect_abort_source_get(enum PMU_POWER_PROTECT_MODULE_T module);

#ifdef __cplusplus
}
#endif

#endif

