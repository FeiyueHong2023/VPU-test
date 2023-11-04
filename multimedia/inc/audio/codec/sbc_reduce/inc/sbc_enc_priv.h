/*
 * sbc_dec_priv.h
 *
 *  Created on: 2021��2��28��
 *      Author: DELL
 */

#ifndef __SBC_ENC_PRIV_H__
#define __SBC_ENC_PRIV_H__
#include "sbc_priv.h"
#include "bs.h"

#include "sbc_com.h"

typedef struct _sbc_encoder_t {
    sbc_info_t streamInfo;
    //
    int16_t XX[2][8 * 16 + 72];
	uint16_t scale_factors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint16_t bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	//
	int32_t as[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t bs[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	//
	int32_t Y[16];
	uint32_t  joins;
} sbc_encoder_t;

EXTERNC void sbc_filter_4band_analysis(sbc_encoder_t* Encoder, pcm_s16_t* PcmData);

EXTERNC void sbc_filter_8band_analysis(sbc_encoder_t* Encoder, pcm_s16_t* PcmData);

EXTERNC void sbc_header_make_x(sbc_encoder_t* Encoder, bs_t*bs);

EXTERNC void sbc_samples_quant_x(sbc_encoder_t* Encoder, bs_t* bs);

EXTERNC void sbc_factors_calc_x(sbc_encoder_t*Encoder);

#endif /*  */
