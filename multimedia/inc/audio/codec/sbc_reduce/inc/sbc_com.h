/*
 * sbc_def.h
 *
 *  Created on: 2021��2��28��
 *      Author: DELL
 */
#pragma once
#ifndef __SBC_COM_H__
#define __SBC_COM_H__
#include <stdint.h>
#include <stdbool.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif
//
enum pcm_s16_flags_e{
	PCM_S16_FLAGS_INTERLACE = 1<<0,
	PCM_S16_FLAGS_MUTE_FRAME = 1<<1,
	PCM_S16_FLAGS_PLC_BAD_FRAME = 1<<2,
	PCM_S16_FLAGS_FIRST_FRAME = 1<<3,
};
//
typedef struct pcm_s16_t {
	int16_t *ch0,*ch1;
    uint16_t samples;
    uint16_t flags;//pcm_s16_flags_e
} pcm_s16_t;
//
typedef struct sbc_info_t {
    uint8_t bitPool;
    uint8_t sampleFreq;
    uint8_t channelMode;
    uint8_t allocMethod;
    uint8_t numBlocks;
    uint8_t numSubBands;
    uint8_t numChannels;
	uint8_t mSbcFlag;
	uint32_t sampleRate;

	uint16_t vlc_size;
	uint16_t pcm_size;
	uint16_t pcm_samples;
} sbc_info_t;

//samele rate
#define SBC_CHNL_SAMPLE_FREQ_16    0
#define SBC_CHNL_SAMPLE_FREQ_32    1
#define SBC_CHNL_SAMPLE_FREQ_44_1  2
#define SBC_CHNL_SAMPLE_FREQ_48    3
//channels mode
#define SBC_CHNL_MODE_MONO          0
#define SBC_CHNL_MODE_DUAL_CHNL     1
#define SBC_CHNL_MODE_STEREO        2
#define SBC_CHNL_MODE_JOINT_STEREO  3
//alloc method
#define SBC_ALLOC_METHOD_LOUDNESS   0
#define SBC_ALLOC_METHOD_SNR        1
//
EXTERNC int sbc_get_vlc_size(sbc_info_t* info);
EXTERNC int sbc_get_pcm_size(sbc_info_t* info);
EXTERNC void msbc_get_info(sbc_info_t*info);
EXTERNC int msbc_get_vlc_size(sbc_info_t* info);
EXTERNC int msbc_get_pcm_size(sbc_info_t* info);
EXTERNC void gsbc_get_info(sbc_info_t*info);
EXTERNC int gsbc_get_vlc_size(sbc_info_t* info);
EXTERNC int gsbc_get_pcm_size(sbc_info_t* info);

//
#endif /*  */
