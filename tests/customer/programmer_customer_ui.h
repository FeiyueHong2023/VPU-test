#include "hal_location.h"

typedef struct {
    void (*charge_ic_init_callback)(void);
} charge_ic_init_cb_t;


typedef charge_ic_init_cb_t CHARGE_IC_INIT_CALLBACK;


void BOOT_TEXT_FLASH_LOC app_charge_ic_callback_init(void); 

void BOOT_TEXT_FLASH_LOC app_charge_ic_init_callback(CHARGE_IC_INIT_CALLBACK *cbs);