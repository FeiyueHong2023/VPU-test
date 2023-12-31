cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

ifeq ($(BISTO_ENABLE),0)
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)voice_sbc/*.cpp))
else
ifneq ($(filter 1, $(SMART_VOICE) $(BIXBY_VOICE)),)
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)voice_sbc/*.cpp))
endif
endif

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += -Iapps/audioplayers \
                    -Iapps/common \
                    -Iapps/key \
                    -Iapps/main \
                    -Iplatform/drivers/bt \
                    -Iplatform/drivers/cp_accel \
                    -Iplatform/hal \
		    -Iservices/audio_bt \
                    -Iservices/ai_voice/audio \
                    -Iservices/ai_voice/transport \
                    -Iservices/ai_voice/manager \
                    -Iservices/ai_voice/protocol/ama/ama_stream \
                    -Iservices/audio_dump/include \
                    -Iservices/audio_manager \
                    -Iservices/app_ai/inc \
                    -Iservices/ibrt_middleware/inc \
                    $(BLUETOOTH_ADAPTER_INCLUDES) \
                    -Imultimedia/inc/audio/codec/sbc/inc \
                    -Imultimedia/inc/speech/inc \
                    -Iservices/norflash_api \
                    -Iservices/ibrt_ui/inc \
                    -Iservices/resources \
                    -Iservices/through_put \
                    -Iservices/voicepath \
                    -Iservices/voicepath/gsound/gsound_custom/inc \
                    -Iservices/voicepath/gsound/gsound_target \
                    -Iservices/voicepath/gsound/gsound_target_api_read_only \
                    -Ithirdparty/userapi \
                    -Iutils/cqueue \
                    -Iutils/heap

ifeq ($(BIXBY_VOICE),1)
subdir-ccflags-y += \
                    -Iservices/ai_voice/protocol/bixbyvoice/ \
                    -Iservices/ai_voice/protocol/bixbyvoice/bixbyvoice_manager
endif

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc
endif
ifeq ($(IBRT_UI),1)
ccflags-y += \
    -Iservices/ibrt_ui_v2/inc
endif
endif

ifeq ($(GFPS_ENABLE),1)
ccflags-y += \
   -Iservices/gfps/inc
endif