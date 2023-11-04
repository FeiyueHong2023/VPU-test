/***************************************************************************
 *
 * Copyright 2022-2023 BES.
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

#include "plat_types.h"
#include "cmsis.h"
#include "hal_analogif.h"
#include "hal_location.h"
#include "hal_spi.h"
#include "hal_trace.h"
#include "hal_iomux.h"

#include "hal_psc.h"
#include "hal_timer.h"
#include "capsensor_spi_best1603.h"
#include "reg_capsensor_best1603.h"
#include "capsensor_driver.h"

#ifdef SENS_SPI_ARBITRATOR_ENABLE
// Min padding OSC cycles needed: BT=0 MCU=6
// When OSC=26M and SPI=6.5M, min padding SPI cycles is BT=0 MCU=2
#define PADDING_CYCLES                  2
#else
#define PADDING_CYCLES                  0
#endif

#define CAPSENSE_READ_CMD(r)                 (((1 << 24) | (((r) & 0xFF) << 16)) << PADDING_CYCLES)
#define CAPSENSE_WRITE_CMD(r, v)             (((((r) & 0xFF) << 16) | ((v) & 0xFFFF)) << PADDING_CYCLES)
#define CAPSENSE_READ_VAL(v)                 (((v) >> PADDING_CYCLES) & 0xFFFF)
#define CAPSENSE_REG_CHIP_ID                 ITNPMU_REG(0x0A)

#define SENS_PAGE_QTY                    5

#define SENS_SPI_REG_CS(r)                  ((r) >> 12)
#define SENS_SPI_REG_PAGE(r)                (((r) >> 8) & 0xF)
#define SENS_SPI_REG_OFFSET(r)              ((r) & 0xFF)

typedef int (*CAPSENSOR_CALLBACK_HAL_RX)(const void *cmd, void *data, uint32_t len);
typedef int (*CAPSENSOR_CALLBACK_HAL_TX)(const void *data, uint32_t len);
typedef void (*CAPSENSOR_TRANS_FUNC)(void);

struct CAPSENSOR_TRANS_FUNC {
    CAPSENSOR_CALLBACK_HAL_RX hal_rx_handler;
    CAPSENSOR_CALLBACK_HAL_TX hal_tx_handler;
};

static struct CAPSENSOR_TRANS_FUNC g_capsensor_trans_cfg;

static uint8_t BOOT_BSS_LOC sens_cs;
static const struct HAL_SPI_CFG_T spi_cfg = {
    .clk_delay_half = false,
    .clk_polarity = false,
    .slave = false,
    .dma_rx = false,
    .dma_tx = false,
    .rx_sep_line = true,
    .cs = 0,
    .rate = 6000000,
    .tx_bits = 25 + PADDING_CYCLES,
    .rx_bits = 25 + PADDING_CYCLES,
    .rx_frame_bits = 0,
};

static int capsensorif_rawread(unsigned short reg, unsigned short *val)
{
    int ret;
    unsigned int data;
    unsigned int cmd;

    data = 0;
    cmd = CAPSENSE_READ_CMD(reg);
    ret = g_capsensor_trans_cfg.hal_rx_handler(&cmd, &data, 4);

    if (ret) {
        return ret;
    }

    *val = CAPSENSE_READ_VAL(data);
    return 0;
}

static int capsensorif_rawwrite(unsigned short reg, unsigned short val)
{
    int ret;
    unsigned int cmd;

    cmd = CAPSENSE_WRITE_CMD(reg, val);

    ret = g_capsensor_trans_cfg.hal_tx_handler(&cmd, 4);
    if (ret) {
        return ret;
    }

    return 0;
}

int capsensorif_reg_read(unsigned short reg, unsigned short *val)
{
    int ret;
    uint32_t lock;
    uint8_t page;

    page = SENS_SPI_REG_PAGE(reg);
    reg = SENS_SPI_REG_OFFSET(reg);

    if (page >= SENS_PAGE_QTY) {
        return -1;
    }

    lock = int_lock();

    ret = capsensorif_rawread(reg, val);

    int_unlock(lock);

    return ret;
}

int capsensorif_reg_write(unsigned short reg, unsigned short val)
{
    int ret;
    uint32_t lock;
    uint8_t page;

    page = SENS_SPI_REG_PAGE(reg);
    reg = SENS_SPI_REG_OFFSET(reg);

    if (page >= SENS_PAGE_QTY) {
        return -1;
    }

    lock = int_lock();

    ret = capsensorif_rawwrite(reg, val);

    int_unlock(lock);

    return ret;
}

int BOOT_TEXT_FLASH_LOC capsensorif_open(void)
{
    int ret;

#ifdef CHIP_SUBSYS_SENS
    ret = hal_spi_open(&spi_cfg);
#else
    ret = hal_spilcd_open(&spi_cfg);
#endif

    if (ret) {
        return ret;
    }

    sens_cs = 0;
#ifdef CHIP_SUBSYS_SENS
    hal_spi_activate_cs(sens_cs);
#else
    hal_spilcd_activate_cs(sens_cs);
#endif

    return 0;
}

int BOOT_TEXT_FLASH_LOC capsenseif_close(void)
{
    int ret;

    sens_cs = 0;
#ifdef CHIP_SUBSYS_SENS
    ret = hal_spi_close(sens_cs);
#else
    ret = hal_spilcd_close(sens_cs);
#endif

    if (ret) {
        return ret;
    }

#ifdef CHIP_SUBSYS_SENS
    hal_spi_busy();
#else
    hal_spilcd_busy();
#endif

    return 0;
}

static struct GPIO_CAPSPI_CONFIG_T gpio_spi_cfg = {
    .clk = HAL_IOMUX_PIN_P1_4,
    .cs = HAL_IOMUX_PIN_P1_5,
    .tx = HAL_IOMUX_PIN_P1_7,
    .rx = HAL_IOMUX_PIN_P1_6,
    .speed = 0,
    .isOpen = 0,
};

__STATIC_FORCEINLINE void GPIO_InitIO(uint8_t port, uint8_t direction, uint8_t val_for_out)
{
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)port, (enum HAL_GPIO_DIR_T)direction, val_for_out);
}

__STATIC_FORCEINLINE void GPIO_WriteIO(uint8_t port, uint8_t data)
{
    if(data){
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)port);
    }else{
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)port);
    }
}
__STATIC_FORCEINLINE uint8_t GPIO_ReadIO(uint8_t port)
{
    uint8_t level = 0;
    level = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)port);
    return level;
}

int capsensor_gpio_capspi_initialize(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP hal_gpio_capspi_iomux_cfg =
    {HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE};

    struct GPIO_CAPSPI_CONFIG_T *cfg = &gpio_spi_cfg;

    // Set the GPIO pin to output status
    hal_gpio_capspi_iomux_cfg.pin = cfg->clk;
    hal_iomux_init(&hal_gpio_capspi_iomux_cfg, 1);
    GPIO_InitIO(cfg->clk, 1, 0);

    hal_gpio_capspi_iomux_cfg.pin = cfg->tx;
    hal_iomux_init(&hal_gpio_capspi_iomux_cfg, 1);
    GPIO_InitIO(cfg->tx, 1, 1);

    hal_gpio_capspi_iomux_cfg.pin = cfg->cs;
    hal_iomux_init(&hal_gpio_capspi_iomux_cfg, 1);
    GPIO_InitIO(cfg->cs, 1, 1);

    hal_gpio_capspi_iomux_cfg.pin = cfg->rx;
    hal_iomux_init(&hal_gpio_capspi_iomux_cfg, 1);
    GPIO_InitIO(cfg->rx, 0, 1);

    return 0;
}

int gpio_capspi_send(const void *data, uint32_t len) /* return 0 --> ack */
{
    uint32_t i;
    struct GPIO_CAPSPI_CONFIG_T *cfg = &gpio_spi_cfg;
    uint32_t * data1 = (uint32_t *)data;

    //add cs L
    GPIO_WriteIO(cfg->cs,0);

    HAL_GPIO_I2C_DELAY(DURATION_LOW);
    for(i=24; (i>=0)&&(i<=24); i--)
    {
        GPIO_WriteIO(cfg->clk,0); /* low */
        HAL_GPIO_I2C_DELAY(DURATION_LOW);
        GPIO_WriteIO(cfg->tx,((*data1>>i)&0x01));
        HAL_GPIO_I2C_DELAY(DURATION_LOW/2);
        GPIO_WriteIO(cfg->clk, 1); /* high */
        HAL_GPIO_I2C_DELAY(DURATION_HIGH);
    }
    GPIO_WriteIO(cfg->clk,0); /* low */
    HAL_GPIO_I2C_DELAY(DURATION_LOW);

    //add cs H
    GPIO_WriteIO(cfg->cs,1);
    return 0;
}

int gpio_capspi_recv(const void *cmd, void *data, uint32_t len) /* return 0 --> ack */
{
    uint32_t i;
    struct GPIO_CAPSPI_CONFIG_T *cfg = &gpio_spi_cfg;
    uint32_t dataCache = 0;
    uint32_t * cmd1 = (uint32_t *)cmd;
    uint32_t * data1 = (uint32_t *)data;

    //add cs L
    GPIO_WriteIO(cfg->cs,0);

    HAL_GPIO_I2C_DELAY(DURATION_LOW);
    for(i=24; (i>=16)&&(i<=24); i--)
    {
        GPIO_WriteIO(cfg->clk,0); /* low */
        HAL_GPIO_I2C_DELAY(DURATION_LOW);
        GPIO_WriteIO(cfg->tx,((*cmd1>>i)&0x01));
        HAL_GPIO_I2C_DELAY(DURATION_LOW/2);
        GPIO_WriteIO(cfg->clk, 1); /* high */
        HAL_GPIO_I2C_DELAY(DURATION_HIGH);
    }

    for(i=15; (i>=0)&&(i<=15); i--)
    {
        GPIO_WriteIO(cfg->clk,0); /* low */
        HAL_GPIO_I2C_DELAY(DURATION_LOW);
        GPIO_WriteIO(cfg->tx,((*cmd1>>i)&0x01));
        HAL_GPIO_I2C_DELAY(DURATION_LOW/2);
        GPIO_WriteIO(cfg->clk, 1); /* high */
        HAL_GPIO_I2C_DELAY(DURATION_HIGH/2);
        dataCache |= (GPIO_ReadIO(cfg->rx )<<i);
        HAL_GPIO_I2C_DELAY(DURATION_HIGH/2);
    }
    GPIO_WriteIO(cfg->clk,0); /* low */
    HAL_GPIO_I2C_DELAY(DURATION_LOW);

    //add cs H
    GPIO_WriteIO(cfg->cs,1);
    *data1 = dataCache;
    return 0;
}

int BOOT_TEXT_FLASH_LOC capsensor_spi_init(void)
{
#if defined(CAPSENSOR_HAL_SPI)
#ifdef CHIP_SUBSYS_SENS
    hal_iomux_set_sens_spi();
    g_capsensor_trans_cfg.hal_rx_handler = hal_spi_recv;
    g_capsensor_trans_cfg.hal_tx_handler = hal_spi_send;
#else
    hal_iomux_set_bth_spi_capsensor();
    g_capsensor_trans_cfg.hal_rx_handler = hal_spilcd_recv;
    g_capsensor_trans_cfg.hal_tx_handler = hal_spilcd_send;
#endif
    capsensorif_open();
#elif defined(CAPSENSOR_GPIO_SPI)
    capsensor_gpio_capspi_initialize();
    g_capsensor_trans_cfg.hal_rx_handler = gpio_capspi_recv;
    g_capsensor_trans_cfg.hal_tx_handler = gpio_capspi_send;
#endif
    return 0;
}

static void capsensor_sw_control_baseline_en(void)
{
    uint16_t value;
    for (int i = 0; i < 8; i++)
    {
        capsensorif_reg_read(CAP_REG_0C + i, &value);
        value |= CDC_BASELINE_P_BIT_IN_CH0_DR;
        capsensorif_reg_write(CAP_REG_0C + i, value);
    }

    capsensorif_reg_read(CAP_REG_3D, &value);
    value |= CDC_BASELINE_N_BIT_IN_CH0_DR | CDC_BASELINE_N_BIT_IN_CH1_DR | CDC_BASELINE_N_BIT_IN_CH2_DR | CDC_BASELINE_N_BIT_IN_CH3_DR
        | CDC_BASELINE_N_BIT_IN_CH4_DR | CDC_BASELINE_N_BIT_IN_CH5_DR | CDC_BASELINE_N_BIT_IN_CH6_DR | CDC_BASELINE_N_BIT_IN_CH7_DR;
    capsensorif_reg_write(CAP_REG_3D, value);
}

void capsensor_baseline_reg_read(uint32_t* baseline_value_p, uint32_t* baseline_value_n)
{
    uint16_t baseline_val[8] = {0};

    for (int i = 0; i < 8; i++)
    {
        capsensorif_reg_read(CAP_REG_14 + i, &baseline_val[i]);
        // TRACE(0, "baseline[%d]:%x", i, baseline_val[i]);

        baseline_value_p[i] = baseline_val[i];
    }

    baseline_value_n[0] = baseline_val[1] << 8 | baseline_val[0];
    baseline_value_n[1] = baseline_val[3] << 8 | baseline_val[2];
    baseline_value_n[2] = baseline_val[5] << 8 | baseline_val[4];
    baseline_value_n[3] = baseline_val[7] << 8 | baseline_val[6];
}

void capsensor_baseline_dr(uint32_t* baseline_value_p, uint32_t* baseline_value_n)
{
    for (int i = 0; i < 8; i++)
    {
        capsensorif_reg_write(CAP_REG_0C + i, baseline_value_p[i]);
    }

    capsensorif_reg_write(CAP_REG_1C, baseline_value_n[0]);
    capsensorif_reg_write(CAP_REG_1D, baseline_value_n[1]);
    capsensorif_reg_write(CAP_REG_1E, baseline_value_n[2]);
    capsensorif_reg_write(CAP_REG_1F, baseline_value_n[3]);

    capsensor_sw_control_baseline_en();
}

static float formula_convert_sar_vtoc(uint32_t sar_val)
{
    float sar_value = 0;

    sar_value += ((sar_val>>9&0x1)*39/10.0);
    sar_value += ((sar_val>>8&0x1)*39/10.0/2);
    sar_value += ((sar_val>>7&0x1)*39/10.0/4);
    sar_value += ((sar_val>>6&0x1)*39/10.0/4);
    sar_value += ((sar_val>>5&0x1)*39/10.0/8);
    sar_value += ((sar_val>>4&0x1)*39/10.0/16);
    sar_value += ((sar_val>>3&0x1)*39/10.0/32);
    sar_value += ((sar_val>>2&0x1)*39/10.0/32);
    sar_value += ((sar_val>>1&0x1)*39/10.0/64);
    sar_value += ((sar_val>>0&0x1)*39/10.0/128);
    return sar_value;
}

void capsensor_fp_mode_int(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_88, &value);
    value &= ~(FP_MODE_RD_INT_MASK);//fp_mode_rd_int_mask
    capsensorif_reg_write(CAP_REG_88, value);

    capsensorif_reg_read(CAP_REG_87, &value);
    value |= FP_MODE_RD_INT_RAW_EN_REG;//fp_mode_rd_int_raw_en_reg
    capsensorif_reg_write(CAP_REG_87, value);
}

void capsensor_press_int(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_88, &value);
    value &= ~(PRESS_INT_MASK);//press_int_mask
    capsensorif_reg_write(CAP_REG_88, value);

    capsensorif_reg_read(CAP_REG_87, &value);
    value |= PRESS_INT_EN_REG;//press_int_en_reg
    capsensorif_reg_write(CAP_REG_87, value);

    capsensorif_reg_read(CAP_REG_02, &value);
    value |= POWER_MODE_REG; //power_mode_reg
    capsensorif_reg_write(CAP_REG_02, value);

    capsensorif_reg_write(CAP_REG_93, 0xFFFF); //threshold_value_ch0
    capsensorif_reg_write(CAP_REG_94, 0xFFFF); //threshold_value_ch1
    capsensorif_reg_write(CAP_REG_95, 0xFFFF); //threshold_value_ch2
    capsensorif_reg_write(CAP_REG_96, 0xFFFF); //threshold_value_ch3
    capsensorif_reg_write(CAP_REG_97, 0xFFFF); //threshold_value_ch4
}

static void capsensor_fifo_num_and_capture_config(uint8_t cap_ch_num, uint8_t cap_conversion_num, uint16_t cap_samp_fs)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_80, &value);
    value |= FIFO_DATA_SDM_ONLY; //fifo_data_sdm_only
    value &= ~(FIFO_TAG_EN); //fifo_tag_en
    capsensorif_reg_write(CAP_REG_80, value);

    capsensorif_reg_read(CAP_REG_02, &value);
    value = SET_BITFIELD(value, ACTIVE_CH_NUM, cap_ch_num); //active_ch_num
    capsensorif_reg_write(CAP_REG_02, value);

    capsensorif_reg_read(CAP_REG_3E, &value);
    value = SET_BITFIELD(value, FIFO_DATA_NUM_TH, (cap_ch_num+1)*cap_conversion_num); //fifo_data_num_th
    capsensorif_reg_write(CAP_REG_3E, value);

    capsensorif_reg_read(CAP_REG_3C, &value);
    value = SET_BITFIELD(value, LP_SLEEP_TIME, (cap_samp_fs*33)); //sleep time 10ms 0x14D  , 20ms 0x29A, 50ms 0x681
    capsensorif_reg_write(CAP_REG_3C, value);
}

static void capsensor_ch_map_config(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_55, &value);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_0, 0x0);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_1, 0x1);
    capsensorif_reg_write(CAP_REG_55, value);
    capsensorif_reg_read(CAP_REG_56, &value);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_2, 0x2);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_3, 0x3);
    capsensorif_reg_write(CAP_REG_56, value);
    capsensorif_reg_read(CAP_REG_57, &value);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_4, 0x4);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_5, 0x5);
    capsensorif_reg_write(CAP_REG_57, value);
}

static void capsensor_down_sel_and_cin_config(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_60, &value);
    value = SET_BITFIELD(value, DOWN_SEL_CH0, 0x0);
    value = SET_BITFIELD(value, DOWN_SEL_CH1, 0x0);
    value = SET_BITFIELD(value, DOWN_SEL_CH2, 0x0);
    value = SET_BITFIELD(value, DOWN_SEL_CH3, 0x0);
    value = SET_BITFIELD(value, DOWN_SEL_CH4, 0x0);
    capsensorif_reg_write(CAP_REG_60, value);
    capsensorif_reg_read(CAP_REG_61, &value);
    value = SET_BITFIELD(value, DOWN_SEL_CH5, 0x0);
    value = SET_BITFIELD(value, DOWN_SEL_CH6, 0x0);
    value = SET_BITFIELD(value, DOWN_SEL_CH7, 0x0);
    capsensorif_reg_write(CAP_REG_61, value);

    capsensorif_reg_read(CAP_REG_66, &value);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH0_REG, 0x1);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH1_REG, 0x1);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH2_REG, 0x1);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH3_REG, 0x1);
    capsensorif_reg_write(CAP_REG_66, value);

    capsensorif_reg_read(CAP_REG_67, &value);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH4_REG, 0x1);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH5_REG, 0x1);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH6_REG, 0x1);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH7_REG, 0x1);
    capsensorif_reg_write(CAP_REG_67, value);
}

static void capsensor_fir_filter_and_smaple_point_config(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_5F, &value);
    value = 0;  //enable fir0 & fir1
    capsensorif_reg_write(CAP_REG_5F, value);

    capsensorif_reg_read(CAP_REG_04, &value);
    value = SET_BITFIELD(value, CH_IDX_0_SAMPLE_POINT, 0xa);
    value = SET_BITFIELD(value, CH_IDX_1_SAMPLE_POINT, 0xa);
    capsensorif_reg_write(CAP_REG_04, value);
    capsensorif_reg_read(CAP_REG_05, &value);
    value = SET_BITFIELD(value, CH_IDX_2_SAMPLE_POINT, 0xa);
    value = SET_BITFIELD(value, CH_IDX_3_SAMPLE_POINT, 0xa);
    capsensorif_reg_write(CAP_REG_05, value);
    capsensorif_reg_read(CAP_REG_06, &value);
    value = SET_BITFIELD(value, CH_IDX_4_SAMPLE_POINT, 0xa);
    value = SET_BITFIELD(value, CH_IDX_5_SAMPLE_POINT, 0xa);
    capsensorif_reg_write(CAP_REG_06, value);
    capsensorif_reg_read(CAP_REG_07, &value);
    value = SET_BITFIELD(value, CH_IDX_6_SAMPLE_POINT, 0xa);
    value = SET_BITFIELD(value, CH_IDX_7_SAMPLE_POINT, 0xa);
    capsensorif_reg_write(CAP_REG_07, value);

    capsensorif_reg_read(CAP_REG_7F, &value);
    value = SET_BITFIELD(value, CH_SAMPLE_START, 0x9);
    capsensorif_reg_write(CAP_REG_7F, value);
}

void capsensor_setup_cfg(struct CAPSENSOR_CFG_T * cap_cfg)
{
    uint16_t value = 0;

    capsensor_spi_init();

    capsensorif_reg_read(CAP_REG_84, &value);
    value &= ~(PU_OSC_CAP_SENSOR_DR);//pu_osc_cap_sensor_dr
    capsensorif_reg_write(CAP_REG_84, value);

#ifdef CAPSENSOR_FP_MODE
    capsensor_fp_mode_int();
#elif CAPSENSOR_PRESS_INT
    capsensor_press_int();
#endif

    capsensor_fifo_num_and_capture_config(cap_cfg->ch_num, cap_cfg->conversion_num, cap_cfg->samp_fs);
    capsensor_ch_map_config();
    capsensor_down_sel_and_cin_config();
    capsensor_fir_filter_and_smaple_point_config();

    capsensorif_reg_read(CAP_REG_01, &value);
    value |= CDC_PU_VREF0P7 | CDC_PU_LDO | CDC_PU_CDC_REG;
    capsensorif_reg_write(CAP_REG_01, value);
    hal_sys_timer_delay(MS_TO_TICKS(50));
    value |= FSM_EN;
    capsensorif_reg_write(CAP_REG_01, value);
}

int get_sample(struct capsensor_sample_data *sample, int i)
{
    uint32_t sdm_low;
    uint32_t sdm_high;
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_63, &value);
    capsensorif_reg_read(CAP_REG_43, &value);
    sample[i].ch = value & (~0xfff8); //0x43 [2:0]  ch_count
    sdm_low = (value>>4)&0x0F; //0x43 [7:4]  sdm[3:0]
    capsensorif_reg_read(CAP_REG_44, &value);
    sdm_high = value; //0x44 [15:0] sdm[19:4]
    sample[i].sdm = (sdm_high<<4) | sdm_low; //sdm(high,low)

    return 0;
}

int stop_hw_wr_start_sw_rd(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_54, &value);
    value |= FIFO_CLK_DR | FIFO_RD_DIRECTION_REG | FIFO_RD_START_REG; //fifo_clk_dr ,fifo_rd_start_reg
    value &= ~FIFO_RD_CLK_DR; //fifo_rd_clk_dr , fifo_rd_direction_reg:0 add,1 reduce
    capsensorif_reg_write(CAP_REG_54, value);

    capsensorif_reg_read(CAP_REG_8A, &value); //rd_ptr_dr
    value &= ~RD_PTR_DR;
    capsensorif_reg_write(CAP_REG_8A, value);

    return 0;
}

int stop_sw_rd_start_hw_wr(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_82, &value); //rd_addr_ptr[15:8]
    value = SET_BITFIELD(value, RD_ADDR_PTR, 0x1);
    capsensorif_reg_write(CAP_REG_8A, value);

    capsensorif_reg_read(CAP_REG_84, &value); //pu_osc_cap_sensor_dr
    value &= ~(PU_OSC_CAP_SENSOR_DR | PU_OSC_CAP_SENSOR_REG);
    capsensorif_reg_write(CAP_REG_84, value);

    capsensorif_reg_read(CAP_REG_54, &value);
    value &= ~(FIFO_CLK_DR| FIFO_RD_START_REG);
    capsensorif_reg_write(CAP_REG_54, value);

    return 0;
}

void select_sort_sample_data(struct capsensor_sample_data *sample, int chnum)
{
    uint32_t swap;
    for (int i = 0; i < chnum; i++)
    {
        for(int j = i+1 ; j < chnum; j++)
        {
                if(sample[j].ch < sample[i].ch)
                {
                    swap = sample[i].ch;
                    sample[i].ch = sample[j].ch;
                    sample[j].ch = swap;

                    swap = sample[i].sdm;
                    sample[i].sdm = sample[j].sdm;
                    sample[j].sdm = swap;
                }
        }
    }
}

void count_sar_value(struct capsensor_sample_data *sample_value)
{
    float sar_value = 0;

    sar_value = formula_convert_sar_vtoc(sample_value->sar);
    sample_value->sar_int = (int)sar_value;
    sample_value->sar_float = (int)((sar_value-(float)(sample_value->sar_int))*1000000);
}

void count_sdm_value(struct capsensor_sample_data *sample_value)
{
    float sdm_value = 0;

    sdm_value += ((sample_value->sdm)/131072.0);  //131072
    sample_value->sdm_int = (int)sdm_value;
    sample_value->sdm_float = (int)((sdm_value-(float)(sample_value->sdm_int))*1000000);
}

void count_sar_sdm_value(struct capsensor_sample_data *sample_value)
{
    float sar_value = 0;
    float sdm_value = 0;

    sar_value = formula_convert_sar_vtoc(sample_value->sar);
    sdm_value += ((sample_value->sdm)/131072.0);  //131072
    sample_value->sar_sdm_int = (int)(sar_value + sdm_value);
    sample_value->sar_sdm_float = (int)((sar_value + sdm_value-(float)(sample_value->sar_sdm_int))*1000000);
}

void calculate_sample_data(struct capsensor_sample_data *sample, uint8_t cap_num) //calculate V->C
{
    for(int data = 0; data < cap_num; data++)
    {
        count_sar_value(&sample[data]);
        count_sdm_value(&sample[data]);
        count_sar_sdm_value(&sample[data]);
    }
}

int capsensor_irq_type_judge(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_88, &value);
    if((value & (1<<12)) == (1<<12)) { //FP_MODE_RD
        value |= 1<<15; //fp_mode_rd_int_clr_reg
        capsensorif_reg_write(CAP_REG_88, value);
        return FP_MODE_RD_INT;
    } else if((value & (1<<8)) == (1<<8)) { //press mode
        return PRESS_INT;
    }else {
        return -1;
    }
}

void capsensor_baseline_read(uint8_t num)
{
    uint16_t value = 0;
    for (int i = 0; i < num; i++)
    {
        capsensorif_reg_read(CAP_REG_14 + i, &value);
        TRACE(0, "baseline[%d]:%x ", i, value);
    }
}

void capsensor_get_raw_data(struct capsensor_sample_data *sample, int num)
{
    int cap_irq_mode = 0;

    cap_irq_mode = capsensor_irq_type_judge();

    switch(cap_irq_mode) {
        case FP_MODE_RD_INT:
            stop_hw_wr_start_sw_rd();
            for(int i=0; i<num; i++) {
                get_sample(sample, i);
            }
            stop_sw_rd_start_hw_wr();

            select_sort_sample_data(sample, num);
#if 0
            capsensor_baseline_read(num);
            calculate_sample_data(sample, num);

            for (int k = 0; k < num; k++) {
                TRACE_IMM(0 ,"cap_data:%d\t%x\t%d.%06d\t%x\t%d.%06d\t%d.%06d", sample[k].ch, sample[k].sar,
                    sample[k].sar_int, sample[k].sar_float,
                    sample[k].sdm, sample[k].sdm_int, sample[k].sdm_float,
                    sample[k].sar_sdm_int, sample[k].sar_sdm_float);
            }
#endif
            break;

        case PRESS_INT:
            break;

        default:
            break;
    }
}

void capsensor_fp_mode_set_mask(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_88, &value); //fp_mode_rd_int_mask
    value |= FP_MODE_RD_INT_MASK;
    capsensorif_reg_write(CAP_REG_88, value);
}

void capsensor_fp_mode_clr_mask(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_88, &value); //fp_mode_rd_int_mask
    value &= ~FP_MODE_RD_INT_MASK;
    capsensorif_reg_write(CAP_REG_88, value);
}

void capsensor_fp_mode_clear_irq(void)
{
    uint16_t value = 0;

    capsensorif_reg_read(CAP_REG_88, &value); //fp_mode_rd_int_clr_reg
    value |= FP_MODE_RD_INT_CLR_REG;
    capsensorif_reg_write(CAP_REG_88, value);
}
