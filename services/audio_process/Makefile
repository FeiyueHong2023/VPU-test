
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Imultimedia/inc/audio/process/filters/include \
	-Imultimedia/inc/audio/process/common/include \
	-Imultimedia/inc/audio/process/drc/include \
	-Imultimedia/inc/audio/process/limiter/include \
	-Imultimedia/inc/audio/process/reverb/include \
	-Imultimedia/inc/audio/process/virtual_surround_process/include \
	-Imultimedia/inc/speech/inc \
	-Iplatform/drivers/bt \
	-Iservices/bt_app/a2dp_codecs/include \
	-Iutils/cqueue \
	-Iapps/anc/inc \
	-Iapps/key \
	-Iservices/bt_app \
	-Iservices/ibrt_middleware/inc \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Iservices/hw_dsp/inc \
	-Iservices/audio_dump/include \
	-Iservices/audioflinger \
	-Iservices/mcpp \
	-Iutils/kfifo \
	-Iutils/heap \
	-Iinclude/cmsis_dsp \
	-Iapps/common/ \
	-Iservices/audio_manager \
	-Iapps/audioplayers/a2dp_decoder/ \
	-Iservices/audio_bt/

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
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/ibrt_ui_v2/inc
endif
endif

ifeq ($(HW_FIR_EQ_PROCESS),1)
ccflags-y += -D__HW_FIR_EQ_PROCESS__
endif

ifeq ($(HW_IIR_EQ_PROCESS),1)
ccflags-y += -D__HW_IIR_EQ_PROCESS__
endif

ifeq ($(SW_IIR_EQ_PROCESS),1)
ccflags-y += -D__SW_IIR_EQ_PROCESS__
endif

ifeq ($(AUDIO_DRC),1)
ccflags-y += -D__AUDIO_DRC__
endif

ifeq ($(AUDIO_LIMITER),1)
ccflags-y += -D__AUDIO_LIMITER__
endif

ifeq ($(AUDIO_REVERB),1)
ccflags-y += -D__AUDIO_REVERB__
endif

ifeq ($(AUDIO_RESAMPLE),1)
ccflags-y += -D__AUDIO_RESAMPLE__
endif

ifeq ($(AUDIO_HW_LIMITER),1)
ccflags-y += -D__AUDIO_HW_LIMITER__
endif

ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
ccflags-y += -D__HW_DAC_IIR_EQ_PROCESS__
ifneq ($(filter best2300p best1400 best3001 best2300 best2000 best1000,$(CHIP)),)
ifneq ($(CHIP_SUBTYPE),best1402)
ifeq ($(ANC_APP),1)
$(error "error: ANC and Hardware codec iir can't be used in the same")
endif
endif
endif
endif

ifeq ($(FIR_HIGHSPEED),1)
ccflags-y += -DFIR_HIGHSPEED
endif

ifeq ($(PC_CMD_UART),1)
CFLAGS_audio_process.o += -D__PC_CMD_UART__
CFLAGS_audio_process.o += -DAUDIO_EQ_TUNING
endif

ifeq ($(TOTA_EQ_TUNING), 1)
CFLAGS_audio_process.o += -DTOTA_EQ_TUNING
CFLAGS_audio_process.o += -DAUDIO_EQ_TUNING
endif

ifeq ($(USB_EQ_TUNING), 1)
ccflags-y += -DUSB_EQ_TUNING
CFLAGS_audio_process.o += -DAUDIO_EQ_TUNING
endif

ifeq ($(USB_AUDIO_APP), 1)
CFLAGS_audio_process.o += -DUSB_AUDIO_APP
endif

ifeq ($(AUDIO_ADAPTIVE_IIR_EQ),1)
CFLAGS_audio_process.o += -DAUDIO_ADAPTIVE_IIR_EQ
CFLAGS_audio_process.o += -DAUDIO_ADAPTIVE_EQ
endif

ifeq ($(AUDIO_ADAPTIVE_FIR_EQ),1)
CFLAGS_audio_process.o += -DAUDIO_ADAPTIVE_FIR_EQ
CFLAGS_audio_process.o += -DAUDIO_ADAPTIVE_EQ
endif

ifeq ($(SPEECH_LIB),1)
CFLAGS_audio_spectrum.o += -DSPEECH_LIB
endif

CFLAGS_audio_spectrum.o += -DUSE_MED_MALLOC

ifeq ($(AUDIO_OUTPUT_SW_GAIN),1)
CFLAGS_audio_process.o += -DAUDIO_OUTPUT_SW_GAIN
endif

ifeq ($(AUDIO_OUTPUT_SW_GAIN_BEFORE_DRC),1)
CFLAGS_audio_process.o += -DAUDIO_OUTPUT_SW_GAIN_BEFORE_DRC
endif

ifeq ($(AUDIO_CUSTOM_EQ),1)
CFLAGS_audio_process.o += -DAUDIO_CUSTOM_EQ
endif