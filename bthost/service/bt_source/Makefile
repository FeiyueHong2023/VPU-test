
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/a2dp_encoder_cp/*.c))

ifeq ($(APP_BT_SOURCE_TEST),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)test/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)test/*.cpp))
endif

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(A2DP_AAC_ON),1)
ifeq ($(FDKAAC_VERSION),2)
AAC_INCLUDES =  \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libAACdec/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libAACenc/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libFDK/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libMpegTPDec/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libMpegTPEnc/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libPCMutils/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libSBRdec/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libSBRenc/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec_2_0_1/libSYS/include
else
AAC_INCLUDES =  \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libAACdec/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libAACenc/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libFDK/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libMpegTPDec/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libMpegTPEnc/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libPCMutils/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libSBRdec/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libSBRenc/include \
	-Imultimedia/inc/audio/codec/fdkaac_codec/libSYS/include
endif
else
AAC_INCLUDES =
endif

ccflags-y += -DBLUETOOTH_BT_IMPL

subdir-ccflags-y += \
	-Iservices/overlay \
	-Iservices/resources \
	-Iservices/audio_bt \
	-Iservices/audioflinger \
	-Iservices/audio_process \
	-Iservices/audio_manager \
	-Iapps/audioplayers \
	-Iapps/audioplayers/a2dp_decoder \
	-Imultimedia/inc/audio/process/resample/include \
	-Imultimedia/inc/audio/process/filters/include \
	-Imultimedia/inc/audio/codec/sbc/inc \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	$(BES_BT_IMPL_INCLUDES) \
	-Iplatform/drivers/bt \
	-Iutils/cqueue \
	-Iutils/heap \
	-Iutils/lockcqueue \
	-Iutils/intersyshci \
	-Iapps/main \
	-Iapps/key \
	-Iapps/common \
	-Iapps/battery \
	-Iservices/ibrt_middleware/inc \
	-Iservices/app_bt_source/inc \
	-Iservices/app_bt_source/src/a2dp_encoder_cp \
	-Iservices/osif/ \
	-Iservices/tota/ \
	-Ithirdparty/audio_codec_lib/liblhdc-enc/inc \
	-Ithirdparty/audio_codec_lib/liblhdcv5-enc/inc \
	$(AAC_INCLUDES) \
	-Iplatform/drivers/cp_accel \
	-Iservices/norflash_api \
	-Itests/anc_usb

ifeq ($(BT_FA_ECC),1)
subdir-ccflags-y += -D__FASTACK_ECC_ENABLE__
endif

ifeq ($(APP_BT_SOURCE_TEST),1)
subdir-ccflags-y += -DAPP_BT_SOURCE_TEST
subdir-ccflags-y += \
	$(ESHELL_INCLUDES)
endif

