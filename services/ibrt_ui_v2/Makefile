
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

IBRT_LIB_NAME := libtws_ibrt_ui

export APP_UI_MONITOR ?= 0
ifeq ($(APP_UI_MONITOR),1)
KBUILD_CPPFLAGS += -DAPP_UI_MONITOR

export APP_UI_MONITOR_GUARD_TIMEOUT ?= 45000
ccflags-y += -DAPP_UI_MONITOR_GUARD_TIMEOUT=$(APP_UI_MONITOR_GUARD_TIMEOUT)

# /* record buff size(power of 2!) */
export APP_UI_MONITOR_RECORD_BUF_SIZE ?= 1024
ccflags-y += -DAPP_UI_MONITOR_RECORD_BUF_SIZE=$(APP_UI_MONITOR_RECORD_BUF_SIZE)
endif

export APP_UI_TEST_MODE_ENABLE ?= 0
ifeq ($(APP_UI_TEST_MODE_ENABLE),1)
KBUILD_CPPFLAGS += -DAPP_UI_TEST_MODE_ENABLE
endif

ifneq ($(APP_UI_THREAD_SIZE),)
KBUILD_CPPFLAGS += -DAPP_UI_THREAD_SIZE=$(APP_UI_THREAD_SIZE)
endif

ifeq ($(AI_VOICE), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_ai
endif

ifeq ($(ENHANCED_STACK),1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_enhanced_stack
endif

ifeq ($(BLE), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_ble
endif

ifeq ($(USE_TRACE_ID), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_log_compressed
endif

ifeq ($(BLE_AUDIO_ENABLED), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_aob
endif

ifeq ($(GFPS_ENABLE),1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_gfps
endif

$(IBRT_LIB_NAME)-y := $(src_obj)

obj-y :=  $(IBRT_LIB_NAME).a

CFLAGS_app_ui_mgr.o += -DBESLIB_INFO=$(BESLIB_INFO)

subdir-ccflags-y += \
    -Iservices/resources \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iservices/audioflinger \
    -Iutils/lockcqueue \
    -Iutils/intersyshci \
    -Iapps \
    -Iapps/key \
    -Iapps/main \
    -Iapps/battery \
    -Iapps/common \
    -Iapps/anc/inc	\
    -Iapps/audioplayers/a2dp_decoder \
    -Iservices/ai_voice/manager \
    -Iservices/ibrt_middleware/inc \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/voicepath	\
    -Iservices/app_ai/inc \
    -Iservices/ibrt_ui_v2/inc \
    -Iservices/audio_manager

ifeq ($(SBC_REDUCE_SIZE),1)
subdir-ccflags-y += \
	-Imultimedia/inc/audio/codec/sbc_reduce/inc
else
subdir-ccflags-y += \
	-Imultimedia/inc/audio/codec/sbc/inc \
	-Imultimedia/audio/codec/sbc/src/inc
endif

ifeq ($(IBRT),1)
subdir-ccflags-y += \
    -Iservices/bt_app
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/custom_api \
    -Iutils/fsm \
    -Iservices/osif \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/ibrt_ui_v2/inc\
    -Iutils/hsm \
    -Iutils/kfifo
endif
endif

ifeq ($(BLE_AUDIO_ENABLED),1)
subdir-ccflags-y += \
    -I$(BLE_AUDIO_CORE_DIR_PATH)/inc \
    -I$(BLE_AOB_APP_DIR_PATH)/gaf_app
endif

subdir-ccflags-y += -Wno-date-time
