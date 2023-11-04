cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))


obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iplatform/drivers/bt  \
    -Iplatform/hal  \
    -Iservices/bt_app/a2dp_codecs/include 	\
    -Iservices/ibrt_middleware/inc  \
    -Iutils/cqueue \
    -Iservices/audio_manager

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
ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/custom_api
endif
ifeq ($(IBRT_UI),1)
ccflags-y += \
    -Iservices/ibrt_ui_v2/inc
endif
endif
