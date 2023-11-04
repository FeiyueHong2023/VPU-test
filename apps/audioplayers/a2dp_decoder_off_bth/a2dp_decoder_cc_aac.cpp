/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#include "hal_timer.h"
#include "cmsis_os.h"
#include "aacdecoder_lib.h"
#include "a2dp_decoder_cc_common.h"
#include "a2dp_decoder_cc_off_bth.h"

#if defined(A2DP_AAC_PLC_ENABLED)
#include "sbcplc.h"

static PLC_State *aac_plc_state = NULL;
static PLC_State *aac_plc_state1 = NULL;
static float *aac_cos_buf = NULL;
#define AAC_PACKET_LEN (1024)
#define AAC_SMOOTH_LEN (AAC_PACKET_LEN*4)
#endif

/************************private macro defination***************************/
#define CC_AAC_OUTPUT_FRAME_SAMPLES        (1024)
#define CC_AAC_MEMPOOL_SIZE                (36*1024)
#define CC_AAC_READBUF_SIZE                (900)
#define CC_AAC_DEC_TRANSPORT_TYPE          (TT_MP4_LATM_MCP1)

/**********************private function declaration*************************/
static void a2dp_decoder_cc_aac_init(void);
static void a2dp_decoder_cc_aac_deinit(void);
static bool a2dp_decoder_cc_decode_frame(a2dp_decoder_cc_media_data_t* pEncodedPacket,
    a2dp_decoder_cc_media_data_t* pPcmFrame);

/************************private variable defination************************/
static HANDLE_AACDECODER aacDec_handle = NULL;
heap_handle_t aac_memhandle = NULL;
static uint8_t *a2dp_decoder_cc_codec_mem_pool = NULL;

A2DP_AUDIO_CC_DECODER_T a2dp_audio_cc_aac_decoder_config =
{
    {44100, 1},
    a2dp_decoder_cc_aac_init,
    a2dp_decoder_cc_aac_deinit,
    a2dp_decoder_cc_decode_frame,
    NULL,
    NULL,
};

/****************************function defination****************************/
static void a2dp_decoder_cc_aac_init(void)
{
    off_bth_syspool_get_buff(&a2dp_decoder_cc_codec_mem_pool, CC_AAC_MEMPOOL_SIZE);
#if defined(A2DP_AAC_PLC_ENABLED)
    off_bth_syspool_get_buff((uint8_t **)(&aac_plc_state), sizeof(PLC_State));
    off_bth_syspool_get_buff((uint8_t **)(&aac_plc_state1), sizeof(PLC_State));
    off_bth_syspool_get_buff((uint8_t **)(&aac_cos_buf), sizeof(float)*AAC_SMOOTH_LEN);

    a2dp_plc_init(aac_plc_state, A2DP_PLC_CODEC_TYPE_AAC);
    a2dp_plc_init(aac_plc_state1, A2DP_PLC_CODEC_TYPE_AAC);
    cos_generate(aac_cos_buf, AAC_SMOOTH_LEN, AAC_PACKET_LEN);
#endif
    ASSERT(a2dp_decoder_cc_codec_mem_pool, "%s size:%d", __func__, CC_AAC_MEMPOOL_SIZE);
    aac_memhandle = heap_register(a2dp_decoder_cc_codec_mem_pool, CC_AAC_MEMPOOL_SIZE);

    if (NULL == aacDec_handle)
    {
        TRANSPORT_TYPE transportFmt = CC_AAC_DEC_TRANSPORT_TYPE;

        aacDec_handle = aacDecoder_Open(transportFmt, 1 /* nrOfLayers */);
        ASSERT(aacDec_handle, "aacDecoder_Open failed");

        aacDecoder_SetParam(aacDec_handle,AAC_PCM_LIMITER_ENABLE,0);
        aacDecoder_SetParam(aacDec_handle, AAC_DRC_ATTENUATION_FACTOR, 0);
        aacDecoder_SetParam(aacDec_handle, AAC_DRC_BOOST_FACTOR, 0);

#ifdef A2DP_AUDIO_STEREO_MIX_CTRL
        CC_DECODE_LOG_I("a2dp_decoder_cc_aac_init: stereo for mix");
        aacDecoder_DecodeFrame_Config(A2DP_AUDIO_CHANNEL_SELECT_STEREO);
#else
        CC_DECODE_LOG_I("a2dp_decoder_cc_aac_init: ch: %d", a2dp_decoder_cc_get_channel());
        aacDecoder_DecodeFrame_Config((uint8_t)a2dp_decoder_cc_get_channel());
#endif
    }
}

static void a2dp_decoder_cc_aac_deinit(void)
{
    if (aacDec_handle)
    {
        aacDecoder_Close(aacDec_handle);
        aacDec_handle = NULL;
    }
}

static void a2dp_decoder_cc_aac_reinit(void)
{
    if (aacDec_handle)
    {
        a2dp_decoder_cc_aac_deinit();
    }
    a2dp_decoder_cc_aac_init();
}

static bool a2dp_decoder_cc_decode_frame(a2dp_decoder_cc_media_data_t* pEncodedPacket,
    a2dp_decoder_cc_media_data_t* pPcmFrame)
{
    // TODO: support multiple codec types
    bool retVal = false;

    if (!aacDec_handle)
    {
        CC_DECODE_LOG_I("aac decoder is not ready");
        return false;
    }

    uint32_t aac_maxreadBytes = CC_AAC_READBUF_SIZE;
    uint32_t bufferSize = 0, bytesValid = 0;
    AAC_DECODER_ERROR decoder_err = AAC_DEC_OK;
    CStreamInfo *stream_info=NULL;
    uint32_t output_byte = 0;

    if (pEncodedPacket->data_len < 64)
    {
        aac_maxreadBytes = 64;
    }
    else if (pEncodedPacket->data_len < 128)
    {
        aac_maxreadBytes = 128;
    }
    else if (pEncodedPacket->data_len < 256)
    {
        aac_maxreadBytes = 256;
    }
    else if (pEncodedPacket->data_len < 512)
    {
        aac_maxreadBytes = 512;
    }
    else if (pEncodedPacket->data_len < 1024)
    {
        aac_maxreadBytes = 1024;
    }

    bufferSize = aac_maxreadBytes;
    bytesValid = aac_maxreadBytes;
    decoder_err = aacDecoder_Fill(aacDec_handle, &(pEncodedPacket->data), &bufferSize, &bytesValid);
    if (AAC_DEC_OK != decoder_err)
    {
        CC_DECODE_LOG_W("aacDecoder_Fill failed:0x%x", decoder_err);
        //if aac failed reopen it again
        if (is_aacDecoder_Close(aacDec_handle))
        {
            a2dp_decoder_cc_aac_reinit();
            CC_DECODE_LOG_W("aac_lc_decode reinin codec \n");
        }
        goto end_decode;
    }

    /* decode one AAC frame */
    decoder_err = aacDecoder_DecodeFrame(aacDec_handle,
        (short *)pPcmFrame->data, pPcmFrame->data_len/2, 0 /* flags */);

#if defined(A2DP_AAC_PLC_ENABLED)
    uint8_t chan;
    chan = a2dp_decoder_cc_get_channel();
    if (A2DP_AUDIO_CHANNEL_SELECT_STEREO == chan) {
        if (pEncodedPacket->isPLC) {
            a2dp_plc_bad_frame(aac_plc_state, (short *)pPcmFrame->data, (short *)pPcmFrame->data, aac_cos_buf, AAC_PACKET_LEN, 2, 0);
            a2dp_plc_bad_frame(aac_plc_state1, (short *)pPcmFrame->data, (short *)pPcmFrame->data, aac_cos_buf, AAC_PACKET_LEN, 2, 1);
        } else {
            a2dp_plc_good_frame(aac_plc_state, (short *)pPcmFrame->data, (short *)pPcmFrame->data, aac_cos_buf, AAC_PACKET_LEN, 2, 0);
            a2dp_plc_good_frame(aac_plc_state1, (short *)pPcmFrame->data, (short *)pPcmFrame->data, aac_cos_buf, AAC_PACKET_LEN, 2, 1);
        }
    } else {
        if (pEncodedPacket->isPLC) {
            CC_DECODE_LOG_W("AAC PLC bad frame");
            a2dp_plc_bad_frame(aac_plc_state, (short *)pPcmFrame->data, (short *)pPcmFrame->data, aac_cos_buf, AAC_PACKET_LEN, 2, chan-2);
            a2dp_plc_bad_frame_smooth(aac_plc_state1, (short *)pPcmFrame->data, (short *)pPcmFrame->data, 2, 3-chan);
        } else {
            a2dp_plc_good_frame(aac_plc_state, (short *)pPcmFrame->data, (short *)pPcmFrame->data, aac_cos_buf, AAC_PACKET_LEN, 2, chan-2);
            a2dp_plc_good_frame_smooth(aac_plc_state1, (short *)pPcmFrame->data, (short *)pPcmFrame->data, 2, 3-chan);
        }
    }
#endif
    CC_DECODE_LOG_D("decoder seq:%d len:%d pcm len:%u err:%x",
        pEncodedPacket->pkt_seq_nb,
        pEncodedPacket->data_len,
        pPcmFrame->data_len/2,
        decoder_err);

    if (AAC_DEC_OK != decoder_err)
    {
        CC_DECODE_LOG_I("aac_lc_decode failed:0x%x", decoder_err);
        //if aac failed reopen it again
        if (is_aacDecoder_Close(aacDec_handle))
        {
            a2dp_decoder_cc_aac_reinit();
            CC_DECODE_LOG_I("aac_lc_decode reinin codec \n");
        }
        goto end_decode;
    }

    stream_info = aacDecoder_GetStreamInfo(aacDec_handle);
    if (!stream_info || stream_info->sampleRate <= 0)
    {
        CC_DECODE_LOG_I("aac_lc_decode invalid stream info");
        goto end_decode;
    }

    output_byte = stream_info->frameSize * stream_info->numChannels * 2;//sizeof(pcm_buffer[0]);
    ASSERT(CC_AAC_OUTPUT_FRAME_SAMPLES == output_byte/4, "aac_lc_decode output mismatch samples:%d", output_byte/4);

    retVal = true;

end_decode:

    return retVal;
}

