cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

TOTA_LIB_NAME := libtota

ifeq ($(USE_TRACE_ID), 1)
TOTA_LIB_NAME := $(TOTA_LIB_NAME)_log_compressed
endif

$(TOTA_LIB_NAME)-y := $(src_obj)

obj-y += $(TOTA_LIB_NAME).a

export BUDSODIN2_TOTA ?= 0
ifeq ($(BUDSODIN2_TOTA),1)
subdir-ccflags-y += -DBUDSODIN2_TOTA
endif

ifeq ($(TOTA_EQ_TUNING), 1)
CFLAGS_app_tota_audio_EQ.o += -DTOTA_EQ_TUNING
endif

CFLAGS_app_tota_general.o += $(LDS_SECTION_FLAGS)

CFLAGS_app_tota.o += -DBESLIB_INFO=$(BESLIB_INFO)

subdir-ccflags-y += -Iplatform/drivers/ana \
                    -Iplatform/hal \
                    -Iservices/bluedroid/adapter/src \
                    -Iapps/key \
                    -Iservices/ibrt_middleware/inc \
                    -Iservices/audio_bt \
                    -Iservices/audio_eq \
                    -Iservices/fs/fat \
                    -Iservices/fs/sd \
                    -Iservices/fs/fat/ChaN \
                    $(BLUETOOTH_ADAPTER_INCLUDES) \
                    -Iservices/overlay \
                    -Iservices/resources \
                    -Imultimedia/inc/audio/codec/sbc/inc \
                    -Iservices/bt_app/a2dp_codecs/include \
                    -Iplatform/drivers/uarthci \
                    -Iplatform/drivers/ana \
                    -Iplatform/drivers/bt \
                    -Iutils/cqueue \
                    -Iutils/heap \
                    -Iutils/retention_ram \
                    -Iservices/audioflinger \
                    -Iutils/lockcqueue \
                    -Iutils/intersyshci \
                    -Iutils/boot_struct \
                    -Iutils \
                    -Iutils/sha256 \
                    -Iutils/kfifo \
                    -Iapps/main \
                    -Iapps/common \
                    -Iapps/audioplayers \
                    -Iservices/app_ai/inc \
                    -Iapps/factory \
                    -Iservices/bridge/ \
                    -Iservices/bt_app \
                    -Imultimedia/inc/speech/codec/codecs/adpcm \
                    -Iservices/anc_spp_tool \
                    -Iservices/tws/inc \
                    -Iservices/app_ibrt_v2/inc \
                    -Iservices/ibrt_core_v2/inc \
                    -Iservices/ibrt_ui_v2/inc \
                    -Iutils/crc32 \
                    -Ithirdparty/userapi \
                    -Iapps/battery \
                    -Iservices/nv_section/factory_section \
                    -Iservices/ \
                    -Iservices/ibrt_middleware/inc/ \
                    -Iservices/ota/bes_ota/inc/ \
                    -Iservices/ota/ \
                    -Iservices/audio_manager \
                    -Iservices/audio_processs \
                    -Iservices/norflash_api
ifeq ($(USB_OTA_ENABLE),1)
subdir-ccflags-y += \
    -Iplatform/drivers/usb/usb_dev/inc
endif

ifeq ($(APP_CHIP_BRIDGE_MODULE),1)
ccflags-y += -Iapps/common/chip_bridge
endif