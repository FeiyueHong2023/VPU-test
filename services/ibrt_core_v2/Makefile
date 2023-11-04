
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

IBRT_LIB_NAME := libtws_ibrt

ifeq ($(ENHANCED_STACK),1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_enhanced_stack
endif

ifeq ($(BLE), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_ble
endif

ifeq ($(SUPPORT_REMOTE_COD), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_cod
endif

ifeq ($(USE_TRACE_ID), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_log_compressed
endif

ifeq ($(BLE_AUDIO_ENABLED), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_aob
endif

ifeq ($(AI_VOICE),1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_ai
endif

ifeq ($(GFPS_ENABLE),1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_gfps
endif

$(IBRT_LIB_NAME)-y := $(src_obj)

obj-y :=  $(IBRT_LIB_NAME).a

CFLAGS_app_tws_ibrt.o += -DBESLIB_INFO=$(BESLIB_INFO)

subdir-ccflags-y += \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    -Iservices/overlay \
    -Iservices/resources \
    -Imultimedia/inc/audio/process/resample/include \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/list \
    -Iutils/heap \
    -Iservices/audioflinger \
    -Iutils/lockcqueue \
    -Iutils/intersyshci \
    -Iapps \
    -Iapps/key \
    -Iapps/main \
    -Iapps/battery \
    -Iapps/common \
    -Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
    -Iapps/factory \
    -Iapps/bt_sync \
    -Iservices/audio_bt \
    -Iservices/ai_voice/manager \
    -Iservices/ibrt_middleware/inc \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/bt_app/a2dp_codecs/include \
    -Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
    -Iutils/hwtimer_list \
    -Iservices/voicepath	\
    -Iservices/custom_api \
    -Iservices/app_ai/inc \
    -Iservices/app_debug_info_system/inc \
    -Iservices/lea_player/inc \
    -Iutils/list \
    -Iservices/tota_v2 \
    -Iservices/osif \
    -Iservices/audio_manager \
    -Iapps/audioplayers/audio_trigger/version1

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
    -Iservices/bt_app \
    -Iservices/app_ibrt_v2/inc
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/custom_api \
    -Iservices/ibrt_ui_v2/inc \
    -Iutils/hsm
endif
endif

ifeq ($(BLE_AUDIO_ENABLED),1)
subdir-ccflags-y += \
    -Iservices/audio_manager
endif

ifeq ($(TWS_RS_WITHOUT_MOBILE),1)
subdir-ccflags-y += \
    -Iapps/audioplayers/audio_trigger/version1
endif

ifeq ($(TWS_DO_NOT_ENTER_SNIFF),1)
subdir-ccflags-y += -D__TWS_DO_NOT_ENTER_SNIFF__
endif

CFLAGS_app_tws_ibrt.o += -DBESLIB_INFO=$(BESLIB_INFO)

ifeq ($(AUDIO_PROMPT_USE_DAC2_ENABLED),1)
CFLAGS_app_ibrt_voice_report.o += -DAUDIO_PROMPT_USE_DAC2_ENABLED
endif

ifeq ($(GFPS_ENABLE),1)
ccflags-y += \
   -Iservices/ble_app/app_gfps \
   -Iservices/gfps/inc
endif
