include config/best1603_ibrt/target.mk

#### ANC DEFINE START ######
export ANC_APP ?= 1

#### ANC CONFIG ######
export ANC_FF_ENABLED	    ?= 1
export ANC_FB_ENABLED	    ?= 1
export AUDIO_ANC_FB_MC      ?= 0
export AUDIO_ANC_FB_MC_HW   ?= 1
export AUDIO_ANC_FB_ADJ_MC  ?= 0
export AUDIO_SECTION_SUPPT  ?= 1
export AUDIO_ANC_SPKCALIB_HW ?= 0
ifeq ($(ANC_APP),1)
export ANC_IIR_LINEAR_SMOOTH_MODE ?= 0
endif
##### ANC DEFINE END ######

#### ANC ASSIST CONFIG ######
export ANC_ASSIST_LE_AUDIO_SUPPORT ?= 0
export ANC_ASSIST_ENABLED   ?= 0
export ANC_ASSIST_ON_DSP   ?= 0
export ANC_ASSIST_VPU       ?= 0
export ANC_ASSIST_ALGO_ON_DSP   ?= 0
##### ANC ASSIST END ######

export PSAP_APP  ?= 0
ifeq ($(PSAP_APP),1)
export PSAP_IIR_LINEAR_SMOOTH_MODE ?= 0
endif

export ANC_ASSIST_PROCESS_THREAD ?= 1

export PROMPT_CTRL_ANC ?= 0
export ANC_PROMPT_CTRL_MUSIC ?= 0

APP_ANC_TEST ?= 1
ifeq ($(APP_ANC_TEST),1)
export TOTA_v2 := 1
endif

ifeq ($(ANC_APP),1)
KBUILD_CPPFLAGS += \
    -DANC_APP \
    -D__BT_ANC_KEY__
endif
