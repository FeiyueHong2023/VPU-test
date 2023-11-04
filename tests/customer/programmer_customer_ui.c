// #include "hwtest.h"
// #include "hal_i2c.h"
// #include "string.h"
#include "programmer_customer_ui.h"
#include "sys_api_programmer.h"
#include "hal_location.h"
#include "hal_iomux.h"

// #define I2C_DEV_ADDR (0x17)
// #define I2C_SPEED     100000

// static int i2c_init_test(void);
// static int i2c_write_test(uint32_t dev_addr, uint8_t *reg_addr, uint32_t reg_len,
//                             uint8_t *pd, uint32_t dlen);
// struct i2c_dev {
//     const char *name;
//     enum HAL_I2C_ID_T bus;
//     uint32_t addr; //device address
//     int speed;
// };

// static struct i2c_dev active = {
//     .name = "bes2x00",
//     .bus = HAL_I2C_ID_1,
//     .addr = I2C_DEV_ADDR,
//     .speed = I2C_SPEED,
// };



void BOOT_TEXT_FLASH_LOC charge_ic_init(void)
{

    // Ensure flash is idle
    //hal_sys_timer_delay(MS_TO_TICKS(2));
    //hal_iomux_set_analog_i2c();
    //hal_iomux_set_uart2();

    //TRACE(0,"CAN_TEST %s",__func__);
    // uint32_t dev_addr = I2C_DEV_ADDR;
    // uint32_t reg, val, vlen = 1;
    // if(!i2c_init_test())
    // {
    //     if(0)
    //         i2c_write_test(dev_addr, (uint8_t *)&reg, 1, (uint8_t *)&val, vlen);
    // }

}

BOOT_TEXT_FLASH_LOC CHARGE_IC_INIT_CALLBACK custom_charge_ic_init_cb = {
        .charge_ic_init_callback          = charge_ic_init,
    };

void BOOT_TEXT_FLASH_LOC app_charge_ic_callback_init(void) 
{
    app_charge_ic_init_callback(&custom_charge_ic_init_cb);
}



// static int i2c_write_test(uint32_t dev_addr, uint8_t *reg_addr, uint32_t reg_len,
//                             uint8_t *pd, uint32_t dlen)
// {
//     struct i2c_dev *pdev = &active;
//     uint8_t txbuf[32];
//     uint32_t reqlen = 0;
//     int ret;

//     if (pdev->addr != dev_addr) {
//         //TODO: probe again
//         TRACE(2,"%s, dev addr 0x%x is not active", __func__, dev_addr);
//         return -1;
//     }

//     if (reg_addr) {
//         memcpy(&txbuf[reqlen], reg_addr, reg_len);
//         reqlen += reg_len;
//     }
//     if (pd) {
//         memcpy(&txbuf[reqlen], pd, dlen);
//         reqlen += dlen;
//     }

//     TRACE(5,"%s, bus:%d, addr:0x%x, txbuf:0x%x, reqlen:%d", __func__,
//             pdev->bus, pdev->addr, (int)txbuf, reqlen);

// #if 0
//     ret = hal_i2c_task_send(pdev->bus, pdev->addr, txbuf, reqlen, 0, NULL);
// #else
//     ret = hal_i2c_simple_send(pdev->bus, pdev->addr, txbuf, reqlen);
// #endif
//     if (ret) {
//         TRACE(4,"%s, send failed, ret=0x%x, reg_len=%d, dlen=%d",
//                 __func__, ret, reg_len, dlen);
//     }
//     TRACE(1,"%s, done", __func__);
//     return ret;
// }


// static int i2c_init_test(void)
// {

//     int ret;
//     struct HAL_I2C_CONFIG_T cfg;

//     memset(&cfg, 0, sizeof(cfg));
// #if 0
//     cfg.mode = HAL_I2C_API_MODE_TASK;
// #else
//     cfg.mode = HAL_I2C_API_MODE_SIMPLE;
// #endif
//     cfg.use_dma =
// #if 0
//         1;
// #else
//         0;
// #endif
//     cfg.use_sync = 1;
//     cfg.speed = I2C_SPEED;
//     cfg.as_master = 1;

//     ret = hal_i2c_open(HAL_I2C_ID_1, &cfg);
//     if (ret) {
//         TRACE(2,"%s, probe failed %d", __func__, ret);
//     }
//     return ret;
// }