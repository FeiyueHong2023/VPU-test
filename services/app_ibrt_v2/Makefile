
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

IBRT_LIB_NAME := libtws_ibrt_app

ifeq ($(AI_VOICE)), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_ai
endif

ifeq ($(USE_TRACE_ID), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_log_compressed
endif

$(IBRT_LIB_NAME)-y := $(src_obj)

obj-y :=  $(IBRT_LIB_NAME).a

subdir-ccflags-y += \
    -Iservices/osif \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    -Iservices/overlay \
    -Iservices/norflash_api \
    -Iservices/resources \
    -Imultimedia/inc/audio/process/resample/include \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iservices/nv_section/factory_section \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iservices/audioflinger \
    -Iutils/lockcqueue \
    -Iutils/intersyshci \
    -Iutils/list \
    -Iapps \
    -Iapps/key \
    -Iapps/main \
    -Iapps/common \
    -Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
    -Iapps/anc/inc \
    -Iapps/factory \
    -Iapps/psap/inc \
    -Iservices/interconnection/green \
    -Iutils/hwtimer_list \
    -Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
    -Iservices/nv_section/log_section \
    -Iservices/ai_voice/manager \
    -Iservices/ai_voice/transport \
    -Iservices/app_ai/inc \
    -Iapps/battery/ \
    -Iutils/crc16 \
    -Iutils/crc32 \
    -Iutils/rom_utils \
    -Iservices/audio_bt/ \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/voicepath \
    -Iservices/voicepath/gsound/gsound_target \
    -Iservices/ibrt_middleware/inc \
    -Iservices/ai_voice/protocol/gma/gma_manager \
    -Iservices/ai_voice/protocol/gma/gma_crypto \
    -Iservices/ai_voice/protocol/dual_mic_recording/voice_manager \
    -Iservices/ai_voice/protocol/bixbyvoice \
    -Iservices/ai_voice/protocol/bixbyvoice/bixbyvoice_manager \
    -Iservices/ai_voice/protocol/dual_mic_recording/voice_manager \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_target_api_read_only \
    -Iservices/ai_voice/codec/compression \
    -Iservices/ai_voice/codec/opus121/include \
    -Iapps/anc/inc \
    -Iservices/ota \
    -Iservices/bt_app \
    -Iapps/sensorhub \
    -Iservices/audio_manager/test \
    -Iservices/audio_manager/ \
    -Iapps/bt_sync \
    -Iplatform/drivers/usb/usb_dev/inc

ifeq ($(BT_SOURCE),1)
ccflags-y += \
	-Iservices/ibrt_core_v2/inc
endif

ifeq ($(SBC_REDUCE_SIZE),1)
ccflags-y += \
	-Imultimedia/inc/audio/codec/sbc_reduce/inc
else
ccflags-y += \
	-Imultimedia/inc/audio/codec/sbc/inc \
	-Imultimedia/audio/codec/sbc/src/inc
endif

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iutils/hsm \
    -Iservices/custom_api \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc
endif
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iservices/ibrt_ui_v2/inc
endif
endif

ifeq ($(BES_OTA),1)
subdir-ccflags-y += \
    -Iservices/ota/bes_ota/inc
endif

ifeq ($(AI_OTA), 1)
subdir-ccflags-y += \
    -Iservices/ota/ai_ota
endif

ifeq ($(GFPS_ENABLE),1)
ccflags-y += \
   -Iservices/ble_app/app_gfps \
   -Iservices/gfps/inc
endif

ifeq ($(ANC_APP),1)
CFLAGS_app_ibrt_ui_test.o += -DANC_APP
endif

ifeq ($(A2DP_SBC_PLC_ENABLED),1)
CFLAGS_app_tws_ibrt_ui_test.o += -DA2DP_SBC_PLC_ENABLED
endif

ifeq ($(BLE_AUDIO_ENABLED),1)
subdir-ccflags-y += \
    -Iservices/log_manager \
    -Iservices/audio_manager
endif
ifeq ($(A2DP_AUDIO_STEREO_MIX_CTRL),1)
CFLAGS_app_ibrt_customif_ui.o += -DA2DP_AUDIO_STEREO_MIX_CTRL
endif

ifeq ($(SPA_AUDIO_ENABLE),1)
subdir-ccflags-y += -Ithirdparty/userapi/spa_dummy_app/inc
ifeq ($(SPA_AUDIO_SEC),1)
ccflags-y += -Ithirdparty/userapi/spa_dummy_app/sec
endif
endif

ifeq ($(APP_UART_MODULE),1)
ccflags-y += -Iapps/common/uart_dma
endif

ifeq ($(APP_CHIP_BRIDGE_MODULE),1)
ccflags-y += -Iapps/common/chip_bridge
endif
