cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += -Iplatform/drivers/ana \
                    $(BLUETOOTH_ADAPTER_INCLUDES)


ifeq ($(VOICE_PROMPT),1)
CFLAGS_app_battery.o += -DMEDIA_PLAYER_SUPPORT
endif

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc
endif
endif

ifeq ($(CHARGER_1802),1)
CFLAGS_app_battery.o += -DCHARGER_1802
endif

ccflags-y += \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iutils/cqueue \
    -Iservices/audio_bt

ifeq ($(SBC_REDUCE_SIZE),1)
ccflags-y += \
	-Imultimedia/inc/audio/codec/sbc_reduce/inc
else
ccflags-y += \
	-Imultimedia/inc/audio/codec/sbc/inc \
	-Imultimedia/audio/codec/sbc/src/inc
endif
