 /***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "stdio.h"
#include <stdlib.h>
#include "hal_trace.h"
#include "hal_aud.h"
#include "plat_types.h"
#include "string.h"
#include "audio_test_defs.h"
#include "audio_test_cmd.h"
#include "app_trace_rx.h"
#include "app_key.h"
#include "bluetooth_bt_api.h"
#include "app_media_player.h"
#if defined(ANC_APP)
#include "app_anc.h"
#include "app_anc_utils.h"
#endif

#ifdef DMA_AUDIO_APP
#include "dma_audio_cli.h"
#endif

#ifdef PSAP_SW_APP
#include "psap_sw_app.h"
extern void psap_sw_app_ctrl_test(const char *cmd);
#endif

#define AUDIO_TEST_LOG_I(str, ...)      TR_INFO(TR_MOD(TEST), "[AUDIO_TEST]" str, ##__VA_ARGS__)

/**
 * Usage:
 *  1. CMD Format: e.g. [audio_test,anc_switch]
 **/

typedef void (*func_handler_t)(const char *cmd);

typedef struct {
    const char *name;
    func_handler_t handler;
} audio_test_func_t;

extern void app_debug_tool_printf(const char *fmt, ...);

static void audio_test_anc_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);

#if defined(ANC_APP)
    app_anc_loop_switch();
#endif
}

#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_CP__) || defined(__VIRTUAL_SURROUND_STEREO__)
extern int32_t audio_process_stereo_surround_onoff(int32_t onoff);
int open_flag = 0;
int32_t stereo_surround_status = 0;
#include "app_bt_stream.h"
extern int32_t audio_process_stereo_set_yaw(float yaw);
extern int32_t audio_process_stereo_set_pitch(float pitch);
static float pitch_angle = 0;
static float yaw_angle = 0;
#endif

static void audio_test_virtual_surround_on(const char *cmd){
#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_CP__) || defined(__VIRTUAL_SURROUND_STEREO__)
    open_flag = 1;
    // app_anc_switch(open_flag);
    stereo_surround_status = open_flag;
    audio_process_stereo_surround_onoff(open_flag);
#endif
}

static void audio_test_virtual_surround_off(const char *cmd){
#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_CP__) || defined(__VIRTUAL_SURROUND_STEREO__)
    open_flag = 0;
    // app_anc_switch(open_flag);
    stereo_surround_status = open_flag;
    audio_process_stereo_surround_onoff(open_flag);
#endif
}

static void audio_test_virtual_surround_test_yaw(const char *cmd){
#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_CP__) || defined(__VIRTUAL_SURROUND_STEREO__)
    yaw_angle += 10.0;
    if(yaw_angle > 180){
        yaw_angle = -180;
    }
    // TRACE(0,"!!!!!!!!!!!!!!!!!!!!! yaw_angle = %d",(int)(yaw_angle));
    audio_process_stereo_set_yaw(yaw_angle);
#endif
}

static void audio_test_virtual_surround_test_pitch(const char *cmd){
#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_CP__) || defined(__VIRTUAL_SURROUND_STEREO__)
    
    pitch_angle += 10.0;
    if(pitch_angle > 80){
        pitch_angle = -80;
    }
    audio_process_stereo_set_pitch(pitch_angle);
#endif
}

static void audio_test_anc_gain_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);

#if defined(ANC_APP)
    static bool flag = false;
    if (flag) {
        app_anc_set_gain_f32(ANC_GAIN_USER_TUNING, PSAP_FEEDFORWARD, 1.0, 1.0);
    } else {
        app_anc_set_gain_f32(ANC_GAIN_USER_TUNING, PSAP_FEEDFORWARD, 0.0, 0.0);
    }

    flag = !flag;
#endif
}

extern int32_t audio_test_stream(void);
static void audio_test_stream_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);

#if defined(AUDIO_TEST_STREAM)
    audio_test_stream();
#endif
}

#if defined(ANC_ASSIST_ENABLED)
#include "app_anc_assist.h"
#endif
static void audio_test_anc_assist_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);
#if defined(ANC_ASSIST_ENABLED)
    static bool flag = true;
    if (flag) {
        app_anc_assist_open(ANC_ASSIST_USER_ANC);
    } else {
        app_anc_assist_close(ANC_ASSIST_USER_ANC);
    }

    flag = !flag;
#endif
}

#if defined(ANC_ASSIST_ENABLED)
#include "app_voice_assist_ai_voice.h"
#include "audioflinger.h"
#endif

static void audio_test_kws_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);
#if defined(ANC_ASSIST_ENABLED)

    struct AF_STREAM_CONFIG_T kws_stream_cfg;
    kws_stream_cfg.sample_rate    = AUD_SAMPRATE_16000;
    kws_stream_cfg.channel_num    = AUD_CHANNEL_NUM_1;
    kws_stream_cfg.bits           = AUD_BITS_16;
    kws_stream_cfg.device         = AUD_STREAM_USE_INT_CODEC;
    kws_stream_cfg.io_path        = AUD_INPUT_PATH_ASRMIC;
    kws_stream_cfg.vol            = 12;
    kws_stream_cfg.handler        = NULL;
    kws_stream_cfg.data_ptr       = NULL;
    kws_stream_cfg.data_size      = 0;
    TRACE(0,"111");
    static bool flag = true;
    if (flag) {
        // FIXME: Create a stream_cfg
        app_voice_assist_ai_voice_open(ASSIST_AI_VOICE_MODE_KWS, &kws_stream_cfg, NULL);
    } else {
        app_voice_assist_ai_voice_close();
    }

    flag = !flag;
#endif
}

#if defined(ANC_ASSIST_ENABLED)
int32_t app_voice_assist_wd_open(void);
int32_t app_voice_assist_wd_close(void);
#endif
static void audio_test_wd_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);
#if defined(ANC_ASSIST_ENABLED)
    static bool flag = true;
    if (flag) {
        app_voice_assist_wd_open();
    } else {
        app_voice_assist_wd_close();
    }

    flag = !flag;
#endif
}

#if defined(SPEECH_BONE_SENSOR)
#include "speech_bone_sensor.h"
#endif
static void audio_test_bone_sensor_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);
#if defined(SPEECH_BONE_SENSOR)
    static bool flag = true;
    if (flag) {
        speech_bone_sensor_open(16000, 120);
        speech_bone_sensor_start();
    } else {
        speech_bone_sensor_stop();
        speech_bone_sensor_close();
    }

    flag = !flag;
#endif
}

static void audio_test_prompt_switch(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);
#if 1//defined(ANC_ASSIST_ENABLED)
    int prompt_buf[6] = {14, 15, 16, 17, 18, 19};
    static int prompt_cnt = 0;

    AUD_ID_ENUM id_tmp = (AUD_ID_ENUM)prompt_buf[prompt_cnt];
    TRACE(1,"[%s],id = 0x%x",__func__,(int)id_tmp);
    media_PlayAudio(id_tmp, 0);

    prompt_cnt++;

    if(6<= prompt_cnt)
        prompt_cnt = 0;

#endif
}

extern int32_t bt_sco_chain_bypass_tx_algo(uint32_t sel_ch);
static void audio_test_bypass_tx_algo(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);

    uint32_t ch = 0;
    sscanf(cmd, "ch=%d", &ch);

    AUDIO_TEST_LOG_I("[%s] ch: %d", __func__, ch);
    bt_sco_chain_bypass_tx_algo(ch);
}

static void audio_test_input_param(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);

    int32_t val1 = 0;
    int32_t val2 = 0;
    sscanf(cmd, "val1=%d val2=%d", &val1, &val2);

    AUDIO_TEST_LOG_I("[%s] val1: %d, val2: %d", __func__, val1, val2);
}

static void audio_test_call_algo_cfg(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);

}

static void audio_test_hifi(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);

#if defined(AUDIO_TEST_HIFI)
    extern int32_t audio_test_hifi_tx_data(void);
    audio_test_hifi_tx_data();
#endif
}

static void audio_test_psap_sw(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);
#ifdef PSAP_SW_APP
    if(!strcmp(cmd, "on")){
        psap_sw_app_open();
    }else if(!strcmp(cmd, "off")){
        psap_sw_app_close();
    }else{
        AUDIO_TEST_LOG_I("invalid cmd");
    }
#else
    AUDIO_TEST_LOG_I("please define PSAP_SW_APP first!!!");
#endif
}

/*
 * audio_test_dma_audio - The test command handler for DMA AUDIO APP.
 * The test command usage:
 *      audio_test:dma_audio on                  - enable dma audio app
 *      audio_test:dma_audio loop_on             - enable adda loop stream
 *      audio_test:dma_audio dac1_on             - enable dac1 stream
 *      audio_test:dma_audio dac2_on             - enable dac2 stream
 *      audio_test:dma_audio adc1_on             - enable adc1 stream

 *      audio_test:dma_audio off                 - disable dma audio app
 *      audio_test:dma_audio loop_off            - disable adda loop stream
 *      audio_test:dma_audio dac1_off            - disable dac1 stream
 *      audio_test:dma_audio dac2_off            - disable dac2 stream
 *      audio_test:dma_audio adc1_off            - disable dac2 stream
 *
 */
static void audio_test_dma_audio(const char *cmd)
{
    if (cmd == NULL) {
        AUDIO_TEST_LOG_I("null cmd");
        return;
    }
#ifdef DMA_AUDIO_APP
    AUDIO_TEST_LOG_I("[%s]: cmd: %s", __func__, cmd);

    if (!strcmp(cmd, "on")) {
        dma_audio_cli_key_on(DAUD_CLI_KEY_NORMAL);
    } else if (!strcmp(cmd, "off")) {
        dma_audio_cli_key_off(DAUD_CLI_KEY_NORMAL);
    } else if (!strcmp(cmd, "loop_on")) {
        dma_audio_cli_key_on(DAUD_CLI_KEY_TEST_LOOP);
    } else if (!strcmp(cmd, "loop_off")) {
        dma_audio_cli_key_off(DAUD_CLI_KEY_TEST_LOOP);
    } else if (!strcmp(cmd, "dac1_on")) {
        dma_audio_cli_key_on(DAUD_CLI_KEY_TEST_PLAY1);
    } else if (!strcmp(cmd, "dac1_off")) {
        dma_audio_cli_key_off(DAUD_CLI_KEY_TEST_PLAY1);
    } else if (!strcmp(cmd, "dac2_on")) {
        dma_audio_cli_key_on(DAUD_CLI_KEY_TEST_PLAY2);
    } else if (!strcmp(cmd, "dac2_off")) {
        dma_audio_cli_key_off(DAUD_CLI_KEY_TEST_PLAY2);
    } else if (!strcmp(cmd, "adc1_on")) {
        dma_audio_cli_key_on(DAUD_CLI_KEY_TEST_CAP1);
    } else if (!strcmp(cmd, "adc1_off")) {
        dma_audio_cli_key_off(DAUD_CLI_KEY_TEST_CAP1);
    } else {
        AUDIO_TEST_LOG_I("invalid cmd: %s", cmd);
        AUDIO_TEST_LOG_I("cmd usage: audio_test:dma_audio on");
        AUDIO_TEST_LOG_I("cmd usage: audio_test:dma_audio off");
    }
#else
    AUDIO_TEST_LOG_I("[%s]: dummy cmd: %s", __func__, cmd);
#endif

}

static void audio_test_psap_sw_ctrl(const char *cmd)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);
#ifdef PSAP_SW_APP
    psap_sw_app_ctrl_test(cmd);
#endif
}

static void audio_test_switch_speech_algo_dsp_test(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);

#if defined(AUDIO_TEST_SPEECH_ALGO_DSP_TEST)
    extern int32_t audio_test_speech_algo_dsp_test(void);
    audio_test_speech_algo_dsp_test();
#endif
}

static void audio_test_switch_speech_algo_dsp(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);

#ifdef AUDIO_TEST_SPEECH_ALGO_DSP
    extern int32_t audio_test_speech_algo_dsp(void);
    audio_test_speech_algo_dsp();
#endif
}

extern void set_adj_eq_state(int val);
static void audio_test_switch_adj_eq(const char *cmd)
{
#ifdef AUDIO_ADJ_EQ
    AUDIO_TEST_LOG_I("[%s] %s", __func__, cmd);
    if(!strcmp(cmd, "on")){
        //TRACE(0, "[%s] adj eq on", __func__);
        set_adj_eq_state(1);
        //app_debug_tool_printf("cmd, %s", "ADJ_EQ_ON");
    }else if(!strcmp(cmd, "off")){
        //TRACE(0, "[%s] adj eq off", __func__);
        set_adj_eq_state(0);
        //app_debug_tool_printf("cmd, %s", "ADJ_EQ_OFF");
    }else{
        app_debug_tool_printf("cmd, invalid cmd");
    }
#endif
}

static void audio_test_le_stream_start(const char *cmd)
{
    AUDIO_TEST_LOG_I("[%s] ...", __func__);

#ifdef AOB_MOBILE_ENABLED
    uint32_t playback_sample_rate = 0;
    uint32_t capture_sample_rate = 0;
    uint32_t playback_context = 0;
    uint32_t capture_context = 0;
    sscanf(cmd, "%d:%d;%d:%d", &playback_context, &playback_sample_rate, &capture_context, &capture_sample_rate);

    AUDIO_TEST_LOG_I("[%s] playback_sample_rate: %d, capture_sample_rate: %d, capture_context %d", __func__, playback_sample_rate, capture_sample_rate, capture_context);

    extern void aob_start_stream_for_audio_test(uint32_t playback_context, uint32_t playback_sample_rate, uint32_t capture_context, uint16_t capture_sample_rate);
    aob_start_stream_for_audio_test(playback_context, playback_sample_rate, capture_context, capture_sample_rate);
#endif
}

const audio_test_func_t audio_test_func[]= {
    {"anc_switch",          audio_test_anc_switch},
    {"stream_switch",       audio_test_stream_switch},
    {"anc_assist_switch",   audio_test_anc_assist_switch},
    {"kws_switch",          audio_test_kws_switch},
    {"wd_switch",           audio_test_wd_switch},
    {"anc_gain_switch",     audio_test_anc_gain_switch},
    {"bone_sensor_switch",  audio_test_bone_sensor_switch},
    {"prompt_switch",       audio_test_prompt_switch},
    {"bypass_tx_algo",      audio_test_bypass_tx_algo},
    {"input_param",         audio_test_input_param},
    {"call_algo_cfg",       audio_test_call_algo_cfg},
    {"hifi",                audio_test_hifi},
    {"speech_algo_dsp",     audio_test_switch_speech_algo_dsp},
    {"speech_algo_dsp_test",audio_test_switch_speech_algo_dsp_test},
    {"adj_eq_switch",       audio_test_switch_adj_eq},
    {"psap_sw",             audio_test_psap_sw},
    {"dma_audio",           audio_test_dma_audio},
    {"le_stream_start",     audio_test_le_stream_start},
    {"psap_sw_ctrl",        audio_test_psap_sw_ctrl},
	{"virtual_surround_on",     audio_test_virtual_surround_on},
    {"virtual_surround_off",    audio_test_virtual_surround_off},
    {"virtual_surround_test_yaw",      audio_test_virtual_surround_test_yaw},
    {"virtual_surround_test_pitch",    audio_test_virtual_surround_test_pitch},
};

static uint32_t audio_test_cmd_callback(uint8_t *buf, uint32_t len)
{
    uint32_t index = 0, i;
    char *cmd = (char *)buf;
    func_handler_t func_handler = NULL;

    //TRACE(1, "[%s]: cmd=%s, len=%d, strlen(cmd)=%d", __func__, cmd, len, strlen(cmd));

    // filter ' ' before function
    for (i = 0; i < strlen(cmd); i++) {
        if (cmd[i] != ' ') {
            cmd += i;
            break;
        }
    }
    //TRACE(1, "cmd=%s, strlen(cmd)=%d", cmd, strlen(cmd));

    // Separate function and value with ' '
    for (index = 0; index < strlen(cmd); index++) {
        if (cmd[index] == ' ' || cmd[index] == '\r' || cmd[index] == '\n') {
            break;
        }
    }

    for (i = 0; i < ARRAY_SIZE(audio_test_func); i++) {
        if (strncmp((char *)cmd, audio_test_func[i].name, index) == 0) {
            func_handler = audio_test_func[i].handler;
            break;
        }
    }

    // filter ' ' before value
    for (; index<strlen(cmd); index++) {
        if (cmd[index] != ' ') {
            break;
        }
    }

    if (strncmp(cmd + index, "bin ", strlen("bin ")) == 0) {
        index += strlen("bin ");
    }

    if (func_handler) {
        func_handler(cmd + index);
        AUDIO_TEST_LOG_I("[audio_test] cmd: OK!");
        TRACE(0, "[CMD] res : 0;");
        app_debug_tool_printf("cmd, Process %s", cmd);
    } else {
        AUDIO_TEST_LOG_I("[audio_test] cmd: Can not found cmd: %s", cmd);
        TRACE(0, "[CMD] res : 1; info : Can not found cmd(%s);", cmd);
        app_debug_tool_printf("cmd, Invalid %s", cmd);
    }

    return 0;
}

int32_t audio_test_cmd_init(void)
{
    app_trace_rx_register("audio_test", audio_test_cmd_callback);

#if defined(AUDIO_TEST_HIFI)
    extern int32_t audio_test_hifi_init(void);
    audio_test_hifi_init();
#endif

    return 0;
}
