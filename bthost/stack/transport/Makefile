
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)../common/*.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hci_transport.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hci.c))

ifeq ($(IBRT), 1)
BTHCI_LIB_NAME := ibrt_libbt_hci
else
BTHCI_LIB_NAME := $(CHIP)_libbt_hci
endif

ifeq ($(HFP_1_6_ENABLE), 1)
BTHCI_LIB_NAME := $(BTHCI_LIB_NAME)_sbc_enc
endif

ifeq ($(ROM_UTILS_ON),1)
BTHCI_LIB_NAME := $(BTHCI_LIB_NAME)_romaac
endif

ifeq ($(BLE),1)
BTHCI_LIB_NAME := $(BTHCI_LIB_NAME)_ble
endif

ifeq ($(USE_TRACE_ID), 1)
BTHCI_LIB_NAME := $(BTHCI_LIB_NAME)_log_compressed
endif

ifeq ($(BLE_AUDIO_ENABLED), 1)
BTHCI_LIB_NAME := $(BTHCI_LIB_NAME)_aob
endif

$(BTHCI_LIB_NAME)-y := $(obj_c:.c=.o)

obj-y := $(BTHCI_LIB_NAME).a

CFLAGS_hci.o += -DBESLIB_INFO=$(BESLIB_INFO)

ccflags-y += -DBLUETOOTH_BT_IMPL

ccflags-y += \
	-Iutils/intersyshci/ \
	-Iutils/cqueue/ \
	-Iplatform/drivers/bt/ \
	-Iservices/osif/ \
	-Iapps/common/ \
	-Iutils/heap/ \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	$(BES_BT_IMPL_INCLUDES) \
	$(BLE_STACK_INCLUDES)


