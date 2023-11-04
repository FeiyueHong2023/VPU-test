cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

BTC_NO_UART_BRIDGE?= 0
ifeq ($(BTC_NO_UART_BRIDGE),1)
obj-y := $(filter-out bt_drv_uart_bridge_intsys.o,$(obj-y))
else
CFLAGS_bt_drv_uart_bridge_intsys.o += $(BT_DRV_UART_BRIDGE_FLAGS)
endif

CFLAGS_bt_drv.o += $(BT_DRV_FLAGS)

CFLAGS_bt_drv_patch.o += $(BT_DRV_PATCH_FLAGS)

CFLAGS_bt_drv_config.o += $(BT_DRV_CONFIG_FLAGS)

subdir-ccflags-y += -Iplatform/drivers/ana/best1603

TX_IQ_CAL ?= 0
ifeq ($(TX_IQ_CAL),1)
subdir-ccflags-y += -DTX_IQ_CAL
endif

RX_IQ_CAL ?= 0
ifeq ($(RX_IQ_CAL),1)
subdir-ccflags-y += -DRX_IQ_CAL
endif

DPD_ONCHIP_CAL ?= 0
ifeq ($(DPD_ONCHIP_CAL),1)
subdir-ccflags-y += -DDPD_ONCHIP_CAL
endif

subdir-ccflags-y += -Iutils/crash_catcher/include
subdir-ccflags-y += -Iservices/nv_section/userdata_section
ccflags-y += $(BLUETOOTH_ADAPTER_INCLUDES)


BLE_EXT_ADV_TX_PWR_INDEPEND ?= 0
ifeq ($(BLE_EXT_ADV_TX_PWR_INDEPEND),1)
ccflags-y += -DBLE_EXT_ADV_TX_PWR_INDEPEND
endif


BT_WATCH_PATCH ?= 0
ifeq ($(BT_WATCH_PATCH),1)
ccflags-y += -DBT_WATCH_PATCH
endif

BT_ACTIVE_OUTPUT ?= 0
ifeq ($(BT_ACTIVE_OUTPUT),1)
ccflags-y += -DBT_ACTIVE_OUTPUT
endif

BESTRX_SUPPORT ?= 0
ifeq ($(BESTRX_SUPPORT),1)
ccflags-y += -D__BESTRX_SUPPORT__
endif

PA_UNSTABLE_WORKAROUND ?= 1
ifeq ($(PA_UNSTABLE_WORKAROUND),1)
ccflags-y += -D__PA_UNSTABLE_WORKAROUND__
endif

RF_RX_LOW_POWER_MODE ?= 0
ifeq ($(RF_RX_LOW_POWER_MODE),1)
ccflags-y += -DRF_RX_LOW_POWER_MODE
endif

MULTI_CIS_SCH_PLAN_ENABLE ?= 0
ifeq ($(MULTI_CIS_SCH_PLAN_ENABLE),1)
KBUILD_CPPFLAGS += -D__MULTI_CIS_SCH_PLAN_ENABLE__
endif