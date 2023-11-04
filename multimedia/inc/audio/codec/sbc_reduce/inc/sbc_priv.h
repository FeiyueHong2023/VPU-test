/*
 * sbc_priv.h
 *
 *  Created on: 2021��2��28��
 *      Author: DELL
 */

#ifndef __SBC_PRIV_H__
#define __SBC_PRIV_H__
#include "sbc_com.h"
#include "sbc_msbc.h"

//
#define SBC_SYNC_WORD 0x9C
//
#define SBC_MAX_NUM_BLK     16
#define SBC_MAX_NUM_SB       8
#define SBC_MAX_NUM_CHNL     2
//
#define SBC_MAX_PCM_DATA  512
//
#define ONE_F       (0x00008000)
#define ONE_F_P     (ONE_F << 15)

//
typedef struct{
    /* === Internal use only === */
    //uint16_t  bitOffset;
    //uint8_t   crc;
    //uint8_t   fcs;
    uint32_t  joins;
    //uint8_t   join[SBC_MAX_NUM_SB];
    uint16_t   scale_factors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    //int32_t  scaleFactors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    //uint16_t  levels[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    //int8_t   bitNeed0[SBC_MAX_NUM_SB];
    //int8_t   bitNeed1[SBC_MAX_NUM_SB];
    uint8_t   bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    int sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    //uint8_t   fcs_bak;
} sbc_codec_priv_t;

void sbc_info_print(sbc_info_t*info);
//
bool sbc_crc_check(sbc_info_t *sbc,void*buff);
void sbc_crc_calc(sbc_info_t *sbc,void*buff);
//uint8_t sbc_4bit_crc_calc(uint8_t input,uint8_t fcs);
//
uint16_t sbc_frame_length_get(sbc_info_t * StreamInfo);
void sbc_bits_for_mono_alloc(sbc_info_t * StreamInfo, uint16_t* bits,uint16_t* scale_factors);
void sbc_bits_for_stereo_alloc(sbc_info_t * StreamInfo, uint16_t (*bits)[8],uint16_t (*scale_factors)[8]);
//
void msbc_info_init(sbc_info_t*info);
//
void sbc_info_update(sbc_info_t*info);
#endif /*  */
