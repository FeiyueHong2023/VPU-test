cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
    -Iplatform/hal \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/norflash \
    -Iservices/audio_bt \
    -Iservices/norflash_api \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Imultimedia/inc/audio/codec/sbc/inc \
    -Iservices/ota \
    -Iservices/ibrt_middleware/inc \
    -Iutils/crc32 \
    -Iutils/cqueue \
    -Iapps/common \
    -Iapps/key \
    -Iapps/main \
    -Iservices/audio_manager

ifeq ($(BISTO_ENABLE),1)
subdir-ccflags-y += \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_target_api_read_only
endif

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/ibrt_ui_v2/inc
endif
endif
