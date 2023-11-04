/***************************************************************************
 *
 * Copyright 2015-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "cmsis.h"
#include "plat_types.h"
#include <string.h>
#include "heap_api.h"
#include "hal_location.h"
#include "codec_sbc.h"
#include "a2dp_decoder_internal.h"
#include "hal_timer.h"
#include "cmsis_os.h"
#include "a2dp_decoder_cc_common.h"
#include "a2dp_decoder_cc_off_bth.h"

#if defined(A2DP_SBC_PLC_ENABLED)
#include "sbcplc.h"

static PLC_State *sbc_plc_state = NULL;
static PLC_State *sbc_plc_state1 = NULL;
static float *sbc_cos_buf = NULL;
#define SBC_PACKET_LEN (128*5)
#define SBC_SMOOTH_LEN (SBC_PACKET_LEN*4)
#endif
/************************private macro defination***************************/

/************************private type defination****************************/
static btif_sbc_decoder_t *cc_sbc_decoder = NULL;

/**********************private function declaration*************************/
static void a2dp_audio_cc_sbc_init(void)
{
    CC_DECODE_LOG_I("%s", __func__);

    off_bth_syspool_get_buff((uint8_t **)(&cc_sbc_decoder), sizeof(btif_sbc_decoder_t));
#if defined(A2DP_SBC_PLC_ENABLED)
    off_bth_syspool_get_buff((uint8_t **)(&sbc_plc_state), sizeof(PLC_State));
    off_bth_syspool_get_buff((uint8_t **)(&sbc_plc_state1), sizeof(PLC_State));
    off_bth_syspool_get_buff((uint8_t **)(&sbc_cos_buf), sizeof(float)*SBC_SMOOTH_LEN);

    a2dp_plc_init(sbc_plc_state, A2DP_PLC_CODEC_TYPE_SBC);
    a2dp_plc_init(sbc_plc_state1, A2DP_PLC_CODEC_TYPE_SBC);
    cos_generate(sbc_cos_buf, SBC_SMOOTH_LEN, SBC_PACKET_LEN);
#endif
    ASSERT(cc_sbc_decoder, "%s", __func__);

    btif_sbc_init_decoder(cc_sbc_decoder);
    cc_sbc_decoder->maxPcmLen = CC_DECODE_SBC_PCMLEN_DEFAULT;
}

void a2dp_audio_cc_sbc_deinit(void)
{
    memset(cc_sbc_decoder, 0, sizeof(btif_sbc_decoder_t));
}

static bool a2dp_audio_cc_sbc_decode_frame(a2dp_decoder_cc_media_data_t* pEncodedPacket,
    a2dp_decoder_cc_media_data_t* pPcmFrame)
{
    bt_status_t ret = BT_STS_SUCCESS;
    uint16_t bytes_parsed = 0;
    float sbc_subbands_gain[8] = {1,1,1,1,1,1,1,1};
    uint32_t buffer_bytes = CC_DECODE_SBC_PCMLEN_DEFAULT;
    btif_sbc_pcm_data_t pcm_data = {0};
    pcm_data.data = pPcmFrame->data;


    ret = btif_sbc_decode_frames(cc_sbc_decoder, pEncodedPacket->data, pEncodedPacket->data_len,
                                &bytes_parsed, &pcm_data, buffer_bytes, sbc_subbands_gain);

#if defined(A2DP_SBC_PLC_ENABLED)
    uint8_t chan;
    chan = a2dp_decoder_cc_get_channel();
    if (A2DP_AUDIO_CHANNEL_SELECT_STEREO == chan) {
        if (pEncodedPacket->isPLC) {
            a2dp_plc_bad_frame(sbc_plc_state, (short *)pPcmFrame->data, (short *)pPcmFrame->data, sbc_cos_buf, SBC_PACKET_LEN, 2, 0);
            a2dp_plc_bad_frame(sbc_plc_state1, (short *)pPcmFrame->data, (short *)pPcmFrame->data, sbc_cos_buf, SBC_PACKET_LEN, 2, 1);
        } else {
            a2dp_plc_good_frame(sbc_plc_state, (short *)pPcmFrame->data, (short *)pPcmFrame->data, sbc_cos_buf, SBC_PACKET_LEN, 2, 0);
            a2dp_plc_good_frame(sbc_plc_state1, (short *)pPcmFrame->data, (short *)pPcmFrame->data, sbc_cos_buf, SBC_PACKET_LEN, 2, 1);
        }
    } else {
        if(pEncodedPacket->isPLC){
            TRACE(0,"SBC PLC bad frame");
            a2dp_plc_bad_frame(sbc_plc_state, (short *)pcm_data.data, (short *)pcm_data.data, sbc_cos_buf, SBC_PACKET_LEN, 2, chan-2);
            a2dp_plc_bad_frame_smooth(sbc_plc_state1, (short *)pcm_data.data, (short *)pcm_data.data, 2, 3-chan);
        }else{
            a2dp_plc_good_frame(sbc_plc_state, (short *)pcm_data.data, (short *)pcm_data.data, sbc_cos_buf, SBC_PACKET_LEN, 2, chan-2);
            a2dp_plc_good_frame_smooth(sbc_plc_state1, (short *)pcm_data.data, (short *)pcm_data.data, 2, 3-chan);
        }
    }
#endif
    //TRACE(0, "chan = %d, len = %d", chan, pcm_data.dataLen);
    pPcmFrame->data_len = pcm_data.dataLen;

    return  ret == BT_STS_SUCCESS ? true: false;
}

A2DP_AUDIO_CC_DECODER_T a2dp_audio_cc_sbc_decoder_config =
{
    {44100, 1},
    a2dp_audio_cc_sbc_init,
    a2dp_audio_cc_sbc_deinit,
    a2dp_audio_cc_sbc_decode_frame,
    NULL,
    NULL,
};

