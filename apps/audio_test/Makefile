
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
    -Iservices/audio_bt \
    -Iservices/bone_sensor \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iservices/resources \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iutils/retention_ram \
    -Iutils/stream_mcps/ \
    -Iutils/signal_generator \
    -Iapps/anc/inc \
    -Iservices/audio_manager

ifeq ($(SPEECH_ALGO_DSP_TEST),1)
CFLAGS_audio_test_speech_algo_dsp_test.o += -DSPEECH_ALGO_DSP_TEST
ccflags-y += -Iservices/speech_algo_dsp_test
endif

ifneq ($(SPEECH_ALGO_DSP),)
CFLAGS_audio_test_speech_algo_dsp.o += -DSPEECH_ALGO_DSP
ccflags-y += -Iservices/mcpp \
    -Iutils/kfifo
endif

ifeq ($(DMA_AUDIO_APP),1)
ccflags-y += -Iservices/dma_audio/app
CFLAGS_audio_test_cmd.o += -DDMA_AUDIO_APP
endif

ifeq ($(PSAP_SW_APP),1)
ccflags-y += -Imultimedia/inc/audio/process/limiter/include \
             -Iapps/psap_sw \
             -Imultimedia/inc/speech/inc
endif
