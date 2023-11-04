ifeq ($(DMA_AUDIO_APP),1)

#-----------------------------------------
# DMA AUDIO COMMON CONFIG
#-----------------------------------------
export DMA_RPC_SVR_CORE              ?=M55
#export DMA_RPC_SVR_CORE              ?=HIFI
#export DMA_RPC_SVR_CORE              ?=CP
#export DMA_RPC_SVR_CORE              ?=SENS

ifeq ($(DMA_RPC_SVR_CORE), M55)
export DSP_M55_TRC_TO_MCU            ?= 1
ifeq ($(DSP_M55_TRC_TO_MCU),1)
export RMT_TRC_IN_MSG_CHAN           ?= 1
endif
endif

export BTH_AS_MAIN_MCU               := 1
export BTH_USE_SYS_PERIPH            := 1
export BTH_USE_SYS_GPDMA             := 1
export CHIP_DMA_CFG_IDX              := 1
export DMA_RPC_DEBUG                 ?= 0
export DMA_AUDIO_USE_DIGMIC          ?= 0
export DMA_AUDIO_LOW_LATENCY         ?= 1
export DMA_AUDIO_USE_TDM0_SLAVE      ?= 0
export DMA_AUDIO_USE_TDM1_SLAVE      ?= 0
export DMA_AUDIO_USE_TDM0_MASTER     ?= 0
export DMA_AUDIO_USE_TDM1_MASTER     ?= 0
export DMA_RPC_STREAM_API            ?= 1
export DMA_AUDIO_APP_DYN_ON          ?= 1

ifneq ($(DMA_AUDIO_USE_TDM0_SLAVE)-$(DMA_AUDIO_USE_TDM1_SLAVE),0-0)
export DMA_AUDIO_SYNC_CLOCK          ?= 1
export AUDIO_RESAMPLE                ?= 1
ifeq ($(DMA_AUDIO_TDM_CFG),1)
export DMA_AUDIO_48K                 ?= 1
export DMA_AUDIO_16BIT               ?= 1
export DMA_AUDIO_8CH                 ?= 1
else
export DMA_AUDIO_96K                 ?= 1
export DMA_AUDIO_24BIT               ?= 1
export DMA_AUDIO_2CH                 ?= 1
endif
else ifneq ($(DMA_AUDIO_USE_TDM0_MASTER)-$(DMA_AUDIO_USE_TDM1_MASTER),0-0)
export DMA_AUDIO_96K                 ?= 1
export DMA_AUDIO_24BIT               ?= 1
export DMA_AUDIO_2CH                 ?= 1
else # !TDM
# DMA AUDIO SAMPLE RATE OPTION #
#export DMA_AUDIO_8K                  ?= 1
#export DMA_AUDIO_16K                 ?= 1
#export DMA_AUDIO_32K                 ?= 1
#export DMA_AUDIO_96K                 ?= 1
#export DMA_AUDIO_8P463K              ?= 1
#export DMA_AUDIO_16P927K             ?= 1
#export DMA_AUDIO_50P781K             ?= 1
export DMA_AUDIO_101P562K            ?= 1
#export DMA_AUDIO_48K                 ?= 1

# DMA AUDIO SAMPLE BIT OPTION #
#export DMA_AUDIO_16BIT               ?= 1
#export DMA_AUDIO_32BIT               ?= 1
export DMA_AUDIO_24BIT               ?= 1

# DMA AUDIO ADC SAMPLE CHANNEL OPTION #
#export DMA_AUDIO_1CH                 ?= 1
#export DMA_AUDIO_3CH                 ?= 1
#export DMA_AUDIO_4CH                 ?= 1
export DMA_AUDIO_2CH                 ?= 1
endif

ifeq ($(DMA_RPC_SVR),1)
#-----------------------------------------
# DMA AUDIO RPC SERVER CONFIG
#-----------------------------------------
export DMA_RPC_SVR                   ?= 1
export DAUD_DEBUG                    ?= 0
export DAUD_WARNING                  ?= 0
ifeq ($(AUDIO_RESAMPLE),1)
ifeq ($(DMA_AUDIO_101P562K),1)
export DMA_AUDIO_SYNC_CLOCK          ?= 0
else
export DMA_AUDIO_SYNC_CLOCK          ?= 1
endif
endif

# DMA AUDIO DEBUG OPTION #
export DMA_AUDIO_MIPS_STAT           ?= 0
export DMA_AUDIO_MIPS_STAT_IMM       ?= 0
export DMA_AUDIO_SYNC_DEBUG_VERBOSE  ?= 1
else
#-----------------------------------------
# DMA AUDIO RPC CLIENT CONFIG
#-----------------------------------------
ifeq ($(NOAPP),1)
export HWTEST                   ?= 1
export DSP_M55_TEST             ?= 1
export DMA_AUDIO_TEST           ?= 1
endif
export CHIP_SUBSYS              := bth
export DSP_M55                  ?= 1
export BTH_CODEC_I2C_SLAVE      ?= 1
#export MCU_I2C_SLAVE            ?= 1
export AUDIO_OUTPUT_DAC2        ?= 1
export AUDIO_OUTPUT_DAC3        ?= 1
export CODEC_SW_SYNC            ?= 1
export CODEC_ADC1_DSF           ?= 1
export CODEC_ADC2_DSF           ?= 1
export DMA_RPC_CLI              ?= 1
ifneq ($(DMA_AUDIO_USE_TDM0_SLAVE)-$(DMA_AUDIO_USE_TDM1_SLAVE),0-0)
ifeq ($(DMA_AUDIO_USE_TDM0_SLAVE),1)
export I2S0_IOMUX_INDEX         ?= 45
endif
export AF_DEVICE_TDM            ?= 1
export CODEC_TIMER              := 1
endif # TDM
endif
endif # DMA_AUDIO_APP

