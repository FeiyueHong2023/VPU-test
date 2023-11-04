/*
 * sbc_dec_priv.h
 *
 *  Created on: 2021��2��28��
 *      Author: DELL
 */

#ifndef __SBC_DEC_PRIV_H__
#define __SBC_DEC_PRIV_H__
#include "sbc_priv.h"
#include "bs.h"

#include "sbc_com.h"

typedef struct _sbc_decoder_t {
    sbc_info_t streamInfo;
    //
    int32_t VV[2][160+16*15];
    //
	uint16_t scale_factors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint16_t bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint32_t joins;
	uint32_t index;
    unsigned maxPcmLen;

    short mch0[8 * 16] ;
    short mch1[8 * 16] ;
    unsigned char stageBuff[512];
    uint16_t curStageOff;
    uint16_t stageLen;
} sbc_decoder_t;

EXTERNC void sbc_4band_synth_filter(sbc_decoder_t* Decoder, pcm_s16_t* pcm);

EXTERNC void sbc_8band_synth_filter(sbc_decoder_t* Decoder, pcm_s16_t* pcm);

EXTERNC char sbc_frames_parser(sbc_decoder_t *Decoder, void *Buff, uint16_t Len, uint16_t *BytesDecoded);

EXTERNC int sbc_dec_frame(sbc_decoder_t* Decoder,void* input,int size,pcm_s16_t* output);

EXTERNC void sbc_8band_synth_filter_without_output(sbc_decoder_t* Decoder);
#endif /*  */
