
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bt/*/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bt/*/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bt/*/*.cpp))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gap/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gap/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gap/*.cpp))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gatt/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gatt/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gatt/*.cpp))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)l2cap/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)l2cap/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)l2cap/*.cpp))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sdp/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sdp/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sdp/*.cpp))

ifeq ($(IBRT), 1)
BTPROFILE_LIB_NAME := ibrt_libbt_profiles
else
BTPROFILE_LIB_NAME := $(CHIP)_libbt_profiles
endif

ifeq ($(HFP_1_6_ENABLE), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_sbc_enc
endif

ifeq ($(ROM_UTILS_ON),1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_romaac
endif

ifeq ($(BLE),1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_ble
endif

ifeq ($(BT_RF_PREFER), 2M)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_2m
endif

ifeq ($(BT_RF_PREFER), 3M)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_3m
endif

ifeq ($(SUPPORT_REMOTE_COD), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_cod
endif

ifeq ($(USE_TRACE_ID), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_log_compressed
endif

ifeq ($(GATT_OVER_BR_EDR), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_btgatt
endif

ifeq ($(BLE_AUDIO_ENABLED), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_aob
endif

$(BTPROFILE_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(BTPROFILE_LIB_NAME).a

CFLAGS_a2dp.o += -O3

CFLAGS_btm.o += -DBESLIB_INFO=$(BESLIB_INFO)

ccflags-y += -DBLUETOOTH_BT_IMPL

subdir-ccflags-y += \
    -Iservices/osif/ \
    -Iservices/auto_test \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BES_BT_IMPL_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iutils/intersyshci \
    -Iservices/audioflinger \
    -Iservices/resources \
    -Iservices/ibrt_core/inc \
    -Iutils/lockcqueue \
    -Iapps/key \
    -Imultimedia/inc/audio/codec/sbc/inc \
    -Iapps/audioplayers

ifeq ($(BLE_AUDIO_ENABLED),1)
subdir-ccflags-y += \
    -Iservices/audio_manager
endif
