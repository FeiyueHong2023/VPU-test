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
// Standard C Included Files
#include "cmsis.h"
#include "cmsis_os.h"
#include "plat_types.h"
#include <string.h>
#include "heap_api.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "bluetooth_bt_api.h"
#include "app_bt_media_manager.h"
#include "app_bt.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "app_audio.h"
#include "codec_sbc.h"
#include "a2dp_api.h"
#include "a2dp_decoder.h"
#include "a2dp_decoder_internal.h"
#include "app_bt.h"
#if defined(IBRT)
#include "btapp.h"
#include "app_ibrt_if.h"
#include "app_tws_ctrl_thread.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_tws_ibrt_audio_analysis.h"
#include "app_tws_ibrt_audio_sync.h"
#include "app_ibrt_customif_cmd.h"
#endif
#include "crc32_c.h"
#include "audio_prompt_sbc.h"
#ifdef __AI_VOICE__
#include "app_ai_voice.h"
#endif
#ifdef __BIXBY
#include "app_bixby_thirdparty_if.h"
#endif

#ifdef A2DP_DECODER_CROSS_CORE
#include "a2dp_decoder_cc_common.h"
#include "a2dp_decoder_cc_bth.h"
#endif
#ifdef A2DP_DECODER_CROSS_CORE_USE_M55
#include "mcu_dsp_m55_app.h"
#include "app_dsp_m55.h"
#endif

#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
// NOTE: If sinTable_f32 in RAM, will consume 2k RAM. Can define local arm_cos_f32 to free 2k RAM.
#include "arm_math.h"
#ifndef PI
#define PI      3.14159265358979f
#endif

static float *g_mix_cos_coeff = NULL;
static uint32_t g_mix_coeff_len = 0;
#endif

#ifdef A2DP_CP_ACCEL
#define A2DP_HEAP_API_LOC                               SRAM_TEXT_LOC
#else
#define A2DP_HEAP_API_LOC
#endif

#ifndef A2DP_AUDIO_MEMPOOL_SIZE
#if defined(A2DP_LHDC_ON) || defined(A2DP_LHDCV5_ON)
#define A2DP_AUDIO_MEMPOOL_SIZE                         (86*1024)  //TODO
#elif defined(A2DP_LDAC_ON)
#define A2DP_AUDIO_MEMPOOL_SIZE                         (76*1024)
#elif defined(A2DP_LC3_ON)
#define A2DP_AUDIO_MEMPOOL_SIZE                        (120*1024)
#elif defined(A2DP_DECODER_CROSS_CORE_USE_M55)
#define A2DP_AUDIO_MEMPOOL_SIZE                         (70*1024)
#else
#define A2DP_AUDIO_MEMPOOL_SIZE                         (72*1024)
#endif
#endif
#define A2DP_AUDIO_WAIT_TIMEOUT_MS                      (500)
#define A2DP_ADUIO_MUTE_JITTER_BUFFER_THRESHOLD         (6)
#define A2DP_ADUIO_MUTE_LOW_LATENCY_RECOVER_FRAME       (1)
#define A2DP_ADUIO_MUTE_NORMAL_RECOVER_FRAME            (3)
#if defined(BT_DONT_PLAY_MUTE_WHEN_A2DP_STUCK_PATCH)
#define A2DP_AUDIO_MUTE_FRAME_CNT_AFTER_NO_CACHE        (0)
#define A2DP_ADUIO_FORCE_RETRIGGER_TIMEOUT              (1000)
#define A2DP_ADUIO_FORCE_RETRIGGER_MAX_CNT              (25)
#else
#define A2DP_AUDIO_MUTE_FRAME_CNT_AFTER_NO_CACHE        (25)
#endif
#define A2DP_AUDIO_SKIP_FRAME_LIMIT_AFTER_NO_CACHE      (50)

#define A2DP_AUDIO_REFILL_AFTER_NO_CACHE                (1)

#ifdef __A2DP_AUDIO_SYNC_FIX_DIFF_NOPID__
#define A2DP_AUDIO_SYNC_INTERVAL                        (25)
#else
#define A2DP_AUDIO_SYNC_INTERVAL                        (1000)
#endif

#define A2DP_AUDIO_LATENCY_LOW_FACTOR                   (1.0f)
#define A2DP_AUDIO_LATENCY_HIGH_FACTOR                  (1.6f)
#define A2DP_AUDIO_LATENCY_MORE_FACTOR                  (1.2f)

#define A2DP_AUDIO_SYNC_FACTOR_REFERENCE                (1.0f)
#define A2DP_AUDIO_SYNC_FACTOR_FAST_LIMIT               ( 0.00015f)
#define A2DP_AUDIO_SYNC_FACTOR_SLOW_LIMIT               (-0.00035f)
#define A2DP_AUDIO_SYNC_FACTOR_NEED_FAST_CACHE          (-0.001f)

#define  A2DP_AUDIO_UNDERFLOW_CAUSE_AUDIO_RETRIGGER     (1)

extern const A2DP_AUDIO_DECODER_T a2dp_audio_sbc_decoder_config;
#if defined(A2DP_AAC_ON)
extern const A2DP_AUDIO_DECODER_T a2dp_audio_aac_lc_decoder_config;
#endif
#if defined(A2DP_LHDC_ON)
extern const A2DP_AUDIO_DECODER_T a2dp_audio_lhdc_decoder_config;
#endif

#if defined(A2DP_LHDCV5_ON)
extern const A2DP_AUDIO_DECODER_T a2dp_audio_lhdcv5_decoder_config;
#endif

#if defined(A2DP_LDAC_ON)
extern const A2DP_AUDIO_DECODER_T a2dp_audio_ldac_decoder_config;
#endif


#if defined(A2DP_SCALABLE_ON)
extern const A2DP_AUDIO_DECODER_T a2dp_audio_scalable_decoder_config;
extern bool a2dp_audio_scalable_latency_custom_tune(uint8_t device_id,
    float averageChunkCnt, uint16_t expectedChunkCnt);
extern void audio_audio_scalable_get_sync_custom_data(uint8_t* custom_data);
#endif

#if defined(A2DP_LC3_ON)
extern const A2DP_AUDIO_DECODER_T a2dp_audio_lc3_decoder_config;
#endif

osMutexDef(audio_buffer_mutex);
osMutexDef(audio_status_mutex);
osMutexDef(audio_stop_mutex);

#ifdef ADVANCE_FILL_ENABLED
bool is_doing_pre_fill = false;
#define pingpang_cnt    2
#endif

#ifdef A2DP_DECODER_CROSS_CORE
extern void a2dp_decoder_bth_set_first_run(A2DP_AUDIO_STARTFRAME_INFO_T headframe);
extern uint16_t a2dp_decoder_get_off_bth_encoded_list_len(void);
#endif

extern uint32_t app_bt_stream_get_dma_buffer_delay_us(void);

#ifdef __A2DP_AUDIO_SYNC_FIX_DIFF_NOPID__
//#define A2DP_AUDIO_SYNC_FIX_DIFF_INTERVA_PRINT_FLOAT (1)

#define A2DP_AUDIO_SYNC_FIX_DIFF_INTERVAL                        (640)
#define A2DP_AUDIO_SYNC_FIX_DIFF_FAST_LIMIT    ( 0.003f)
#define A2DP_AUDIO_SYNC_FIX_DIFF_SLOW_LIMIT    (-0.003f)

extern uint32_t app_bt_stream_get_dma_buffer_samples(void);
static int a2dp_audio_sync_fix_diff_proc(uint32_t tick);
static int a2dp_audio_sync_fix_diff_stop(uint32_t tick);
static int32_t a2dp_audio_sync_fix_diff_reset(void);
static int32_t a2dp_audio_sync_fix_diff_start(uint32_t tick);

typedef struct {
    uint8_t device_id;
    btif_media_header_t header;
    uint8_t data[2*1024];
    uint32_t len;
}A2DP_AUDIO_LAST_PACKET_T;

A2DP_AUDIO_LAST_PACKET_T a2dp_audio_packet_last_cache;

typedef enum{
    A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_IDLE,
    A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_START,
    A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_STOP,
}A2DP_AUDIO_SYNC_FIX_DIFF_STATUS;

typedef struct {
    int32_t tick;
    A2DP_AUDIO_SYNC_FIX_DIFF_STATUS status;
}A2DP_AUDIO_SYNC_FIX_DIFF_T;

static A2DP_AUDIO_SYNC_FIX_DIFF_T a2dp_audio_sync_fix_diff;
#endif

A2DP_AUDIO_CONTEXT_T a2dp_audio_context = {
#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
    .chnl_sel_new = A2DP_AUDIO_CHANNEL_SELECT_LRMERGE,
#endif
    .custom_delay_ms = A2DP_INVALID_DELAY_MS,
};

static heap_handle_t a2dp_audio_heap;

static A2DP_AUDIO_LASTFRAME_INFO_T a2dp_audio_lastframe_info;

static A2DP_AUDIO_DETECT_NEXT_PACKET_CALLBACK a2dp_audio_detect_next_packet_callback = NULL;
static A2DP_AUDIO_DETECT_NEXT_PACKET_CALLBACK a2dp_audio_store_packet_callback = NULL;

static float a2dp_audio_latency_factor = A2DP_AUDIO_LATENCY_LOW_FACTOR;
static float sync_tune_dest_ratio = 1.0f;

static uint32_t store_packet_history_loctime = 0;

static uint32_t check_sum_seed = 0;

static uint32_t start_force_retrigger_ms = 0;

static bool isInPostChoppingPeriod = false;

static int a2dp_audio_internal_lastframe_info_ptr_get(A2DP_AUDIO_LASTFRAME_INFO_T **lastframe_info);

float a2dp_audio_get_sample_reference(void);
int8_t a2dp_audio_get_current_buf_size(void);
extern int *a2dp_decoder_bth_get_cc_sbm_param(void);

static void sbm_default_jitter_buf_level_update_completed_callback(
    SBM_JITTER_BUF_LEVEL_UPDATE_COMPLETED_EVENT_E type,
    int32_t target_ms, int32_t current_average_ms);

SBM_JITTER_BUF_LEVEL_UPDATE_COMPLETED_FUNC sbmJitterBufLevelUpdatedCompletedCallback = sbm_default_jitter_buf_level_update_completed_callback;

void a2dp_audio_heap_init(void *begin_addr, uint32_t size)
{
    a2dp_audio_heap = heap_register(begin_addr,size);
}

A2DP_HEAP_API_LOC
void *a2dp_audio_heap_malloc(uint32_t size)
{
    void *ptr = heap_malloc(a2dp_audio_heap, size);
    if (!ptr)
    {
        TRACE(0, "[%s] ptr=%p size=%u user=%p left/mini left=%u/%u",
            __func__, ptr, size, __builtin_return_address(0),
            heap_free_size(a2dp_audio_heap),
            heap_minimum_free_size(a2dp_audio_heap));
    }
    ASSERT_A2DP_DECODER(ptr, "%s size:%d", __func__, size);
#ifdef A2DP_HEAP_DEBUG
    TRACE(0, "[%s] ptr=%p size=%u user=%p left/mini left=%u/%u",
        __func__, ptr, size, __builtin_return_address(0),
        heap_free_size(a2dp_audio_heap),
        heap_minimum_free_size(a2dp_audio_heap));
#endif
    return ptr;
}

A2DP_HEAP_API_LOC
void *a2dp_audio_heap_cmalloc(uint32_t size)
{
    void *ptr = heap_malloc(a2dp_audio_heap, size);
    ASSERT_A2DP_DECODER(ptr, "%s size:%d", __func__, size);
    memset(ptr, 0, size);
#ifdef A2DP_HEAP_DEBUG
    TRACE(0, "[%s] ptr=%p size=%u user=%p left/mini left=%u/%u",
        __func__, ptr, size, __builtin_return_address(0),
        heap_free_size(a2dp_audio_heap),
        heap_minimum_free_size(a2dp_audio_heap));
#endif
    return ptr;
}

A2DP_HEAP_API_LOC
void *a2dp_audio_heap_realloc(void *rmem, uint32_t newsize)
{
    void *ptr = heap_realloc(a2dp_audio_heap, rmem, newsize);
    ASSERT_A2DP_DECODER(ptr, "%s rmem:%p size:%d", __func__, rmem,newsize);
#ifdef A2DP_HEAP_DEBUG
    TRACE(0, "[%s] ptr=%p/%p size=%u user=%p left/mini left=%u/%u",
        __func__, rmem, ptr, newsize, __builtin_return_address(0),
        heap_free_size(a2dp_audio_heap),
        heap_minimum_free_size(a2dp_audio_heap));
#endif
    return ptr;
}

A2DP_HEAP_API_LOC
void a2dp_audio_heap_free(void *rmem)
{
#ifdef A2DP_HEAP_DEBUG
    TRACE(0, "[%s] ptr=%p user=%p", __func__, rmem, __builtin_return_address(0));
#endif
    ASSERT_A2DP_DECODER(rmem, "%s rmem:%p", __func__, rmem);
    heap_free(a2dp_audio_heap,rmem);
}

void a2dp_audio_heap_info(size_t *total, size_t *used, size_t *max_used)
{
    multi_heap_info_t info;
    heap_get_info(a2dp_audio_heap, &info);

    if (total != NULL)
        *total = info.total_bytes;

    if (used  != NULL)
        *used = info.total_allocated_bytes;

    if (max_used != NULL)
        *max_used = info.total_bytes - info.minimum_free_bytes;
}

int inline a2dp_audio_buffer_mutex_init(void)
{
    if (a2dp_audio_context.audio_buffer_mutex == NULL){
        a2dp_audio_context.audio_buffer_mutex = osMutexCreate((osMutex(audio_buffer_mutex)));
    }
    return 0;
}

int inline a2dp_audio_buffer_mutex_lock(void)
{
    osMutexWait((osMutexId)a2dp_audio_context.audio_buffer_mutex, osWaitForever);
    return 0;
}

int inline a2dp_audio_buffer_mutex_unlock(void)
{
    osMutexRelease((osMutexId)a2dp_audio_context.audio_buffer_mutex);
    return 0;
}

int inline a2dp_audio_status_mutex_init(void)
{
    if (a2dp_audio_context.audio_status_mutex == NULL){
        a2dp_audio_context.audio_status_mutex = osMutexCreate((osMutex(audio_status_mutex)));
    }
    return 0;
}

int inline a2dp_audio_status_mutex_lock(void)
{
    osMutexWait((osMutexId)a2dp_audio_context.audio_status_mutex, osWaitForever);
    return 0;
}

int inline a2dp_audio_status_mutex_unlock(void)
{
    osMutexRelease((osMutexId)a2dp_audio_context.audio_status_mutex);
    return 0;
}

list_node_t *a2dp_audio_list_begin(const list_t *list)
{
    a2dp_audio_buffer_mutex_lock();
    list_node_t *node = list_begin(list);
    a2dp_audio_buffer_mutex_unlock();
    return node;
}

list_node_t *a2dp_audio_list_end(const list_t *list)
{
    a2dp_audio_buffer_mutex_lock();
    list_node_t *node = list_end(list);
    a2dp_audio_buffer_mutex_unlock();
    return node;
}

uint32_t a2dp_audio_list_length(const list_t *list)
{
    a2dp_audio_buffer_mutex_lock();
    uint32_t length = list_length(list);
    a2dp_audio_buffer_mutex_unlock();
    return length;
}

void *a2dp_audio_list_node(const list_node_t *node)
{
    a2dp_audio_buffer_mutex_lock();
    void *data = list_node(node);
    a2dp_audio_buffer_mutex_unlock();
    return data;
}

list_node_t *a2dp_audio_list_next(const list_node_t *node)
{
    a2dp_audio_buffer_mutex_lock();
    list_node_t *next =list_next(node);
    a2dp_audio_buffer_mutex_unlock();
    return next;
}

bool a2dp_audio_list_remove_generic(list_t *list, void *data)
{
    a2dp_audio_buffer_mutex_lock();
    bool nRet = list_remove(list, data);
#ifdef A2DP_DECODER_CROSS_CORE
    if (nRet)
    {
        a2dp_audio_list_feed_off_core();
    }
#endif
    a2dp_audio_buffer_mutex_unlock();
    return nRet;
}

#ifdef A2DP_DECODER_CROSS_CORE
void a2dp_audio_list_feed_off_core(void)
{
    list_t *list = a2dp_audio_context.audio_datapath.input_raw_packet_list;
    list_node_t *node = a2dp_audio_list_begin(list);
    A2DP_COMMON_MEDIA_FRAME_HEADER_T* pFrameHeader = NULL;
    bool ret = true;
    uint8_t cnt = 0;

    if (node == NULL) {
        return;
    }

    do {
        pFrameHeader = (A2DP_COMMON_MEDIA_FRAME_HEADER_T *)list_node(node);
        if (!pFrameHeader->isFeedoffCore) {
            if (a2dp_decoder_get_off_bth_encoded_list_len() + cnt > 200)
            {
                TRACE(0, "off bth too many packet");
                break;
            }

            ret = a2dp_decoder_bth_feed_encoded_data_into_off_core(
                (uint8_t *)pFrameHeader,
                pFrameHeader->timestamp, pFrameHeader->sequenceNumber,
                pFrameHeader->totalSubSequenceNumber,
                pFrameHeader->curSubSequenceNumber,
                pFrameHeader->dataLen,
                pFrameHeader->ptrData);
            if (!ret) {
                break;
            }
            pFrameHeader->isFeedoffCore = true;
            cnt++;
        }
        node = a2dp_audio_list_next(node);
    } while(node);
}

bool a2dp_audio_list_remove_special(list_t *list, void *data)
{
    A2DP_COMMON_MEDIA_FRAME_HEADER_T header =
        *(A2DP_COMMON_MEDIA_FRAME_HEADER_T *)data;
    bool is_remove_off_core = header.isFeedoffCore;
    bool nRet = a2dp_audio_list_remove_generic(list, data);

    if (nRet&&is_remove_off_core)
    {
        TRACE(2, "%s, seq:%u-%u-%u, ca:%p", __func__,
            header.sequenceNumber, header.curSubSequenceNumber,
            header.totalSubSequenceNumber,
             __builtin_return_address(0));

        A2DP_DECODER_CC_FLUSH_T flushInfo;
        flushInfo.flushFrame = data;
        flushInfo.seqNum = header.sequenceNumber;
        flushInfo.totalSubSeq = header.totalSubSequenceNumber;
        flushInfo.subSeq = header.curSubSequenceNumber;

        a2dp_decoder_bth_remove_specifc_frame(flushInfo);
    }

    return nRet;
}

void a2dp_decoder_remove_from_list(void *data)
{
    a2dp_audio_list_remove_generic(
        a2dp_audio_context.audio_datapath.input_raw_packet_list, data);
}

bool a2dp_audio_ibrt_is_slave(void)
{
    bool result = false;
    uint8_t device_id = app_bt_audio_get_curr_a2dp_device();
    struct BT_DEVICE_T * curr_device = app_bt_get_device(device_id);
    if (APP_IBRT_SLAVE_IBRT_LINK_CONNECTED(&curr_device->remote))
    {
        result = true;
    }
    return result;
}

void a2dp_audio_send_master_refill(uint8_t *refill)
{
    tws_ctrl_send_cmd(APP_TWS_CMD_REFILL_FRAMES, \
                   refill, \
                   sizeof(uint8_t));
}

void a2dp_decoder_update_last_frame_info(
    uint16_t sequenceNumber,
    uint32_t timestamp,
    uint16_t curSubSequenceNumber,
    uint16_t totalSubSequenceNumber)
{
    uint32_t lock = int_lock();
    a2dp_audio_lastframe_info.sequenceNumber = sequenceNumber;
    a2dp_audio_lastframe_info.timestamp = timestamp;
    a2dp_audio_lastframe_info.curSubSequenceNumber = curSubSequenceNumber;
    a2dp_audio_lastframe_info.totalSubSequenceNumber = totalSubSequenceNumber;
    a2dp_audio_lastframe_info.decoded_frames++;
    int_unlock(lock);
}
#endif

bool a2dp_audio_list_remove(list_t *list, void *data)
{
#ifdef A2DP_DECODER_CROSS_CORE
    bool nRet = a2dp_audio_list_remove_special(list, data);
#else
    bool nRet = a2dp_audio_list_remove_generic(list, data);
#endif

    return nRet;
}

bool a2dp_audio_list_append(list_t *list, void *data)
{
    a2dp_audio_buffer_mutex_lock();
    bool nRet = list_append(list, data);
#ifdef A2DP_DECODER_CROSS_CORE
    if (nRet)
    {
        A2DP_COMMON_MEDIA_FRAME_HEADER_T* pFrameHeader =
            (A2DP_COMMON_MEDIA_FRAME_HEADER_T *)data;
        pFrameHeader->isFeedoffCore = false;
        a2dp_audio_list_feed_off_core();
    }
#endif
    a2dp_audio_buffer_mutex_unlock();

    return nRet;
}

void a2dp_audio_list_clear(list_t *list)
{
    a2dp_audio_buffer_mutex_lock();
    list_clear(list);
    a2dp_audio_buffer_mutex_unlock();
}

void a2dp_audio_list_free(list_t *list)
{
    a2dp_audio_buffer_mutex_lock();
    list_free(list);
    a2dp_audio_buffer_mutex_unlock();
}

list_t *a2dp_audio_list_new(list_free_cb callback, list_mempool_zmalloc zmalloc, list_mempool_free free)
{
    a2dp_audio_buffer_mutex_lock();
    list_t *list =list_new(callback, zmalloc, free);
    a2dp_audio_buffer_mutex_unlock();
    return list;
}

uint32_t a2dp_audio_get_passed(uint32_t curr_ticks, uint32_t prev_ticks, uint32_t max_ticks)
{
    if(curr_ticks < prev_ticks)
        return ((max_ticks  - prev_ticks + 1) + curr_ticks);
    else
        return (curr_ticks - prev_ticks);
}

#ifdef A2DP_CP_ACCEL
extern "C" uint32_t get_in_cp_frame_cnt(void);
extern "C" uint32_t get_in_cp_frame_delay(void);
#else
static uint32_t get_in_cp_frame_cnt(void)
{
    return 0;
}

static uint32_t get_in_cp_frame_delay(void)
{
    return 0;
}
#endif

#define THROUGHPUT_CHECKER_INTERVAL      (10000)
static void a2dp_audio_throughput_checker_timehandler(void const *param);
osTimerDef (A2DP_AUDIO_THROUGHPUT_CHECKER, a2dp_audio_throughput_checker_timehandler);
static uint64_t throughputTestBufCnt = 0;
osTimerId  a2dp_audio_throughput_timer = NULL;

static void a2dp_audio_throughput_checker_timehandler(void const *param)
{
    uint64_t crrThroughputTestBufCnt = 0;
    int64_t crrThroughput_bps = 0;

    crrThroughputTestBufCnt = throughputTestBufCnt;
    throughputTestBufCnt = 0;
    crrThroughput_bps = crrThroughputTestBufCnt*8/THROUGHPUT_CHECKER_INTERVAL;

    TRACE_A2DP_DECODER_I("[INPUT][THROUGHPUT] cnt:%d/%dK Bbps/%d kbps",
                        (uint32_t)crrThroughputTestBufCnt,
                        (uint32_t)(crrThroughputTestBufCnt/THROUGHPUT_CHECKER_INTERVAL),
                        (uint32_t)crrThroughput_bps);

    osTimerStart(a2dp_audio_throughput_timer, THROUGHPUT_CHECKER_INTERVAL);
}

static void a2dp_audio_throughput_checker_handler(unsigned int len)
{
    throughputTestBufCnt += len;
}

int a2dp_audio_throughput_checker_start(void)
{
    if (a2dp_audio_throughput_timer == NULL){
        a2dp_audio_throughput_timer = osTimerCreate (osTimer(A2DP_AUDIO_THROUGHPUT_CHECKER), osTimerOnce, NULL);
    }
    throughputTestBufCnt = 0;
    osTimerStart(a2dp_audio_throughput_timer, THROUGHPUT_CHECKER_INTERVAL);
    return 0;
}

int a2dp_audio_throughput_checker_stop(void)
{
    osTimerStop(a2dp_audio_throughput_timer);
    throughputTestBufCnt = 0;
    return 0;
}

int inline a2dp_audio_set_status(enum A2DP_AUDIO_DECODER_STATUS decoder_status)
{
    a2dp_audio_status_mutex_lock();
    TRACE(2, "a2dp audio status update:%d->%d", a2dp_audio_context.audio_decoder_status, decoder_status);
    a2dp_audio_context.audio_decoder_status = decoder_status;
    a2dp_audio_status_mutex_unlock();

    return 0;
}

enum A2DP_AUDIO_DECODER_STATUS inline a2dp_audio_get_status(void)
{
    enum A2DP_AUDIO_DECODER_STATUS decoder_status;
    a2dp_audio_status_mutex_lock();
    decoder_status = a2dp_audio_context.audio_decoder_status;
    a2dp_audio_status_mutex_unlock();

    return decoder_status;
}

bool a2dp_audio_is_started(void)
{
    if (A2DP_AUDIO_DECODER_STATUS_START == a2dp_audio_get_status()) {
        return true;
    }
    return false;
}

int inline a2dp_audio_set_store_packet_status(enum A2DP_AUDIO_DECODER_STORE_PACKET_STATUS store_packet_status)
{
    a2dp_audio_status_mutex_lock();
    a2dp_audio_context.store_packet_status = store_packet_status;
    a2dp_audio_status_mutex_unlock();

    return 0;
}

enum A2DP_AUDIO_DECODER_STORE_PACKET_STATUS inline a2dp_audio_get_store_packet_status(void)
{
    enum A2DP_AUDIO_DECODER_STORE_PACKET_STATUS store_packet_status;
    a2dp_audio_status_mutex_lock();
    store_packet_status = a2dp_audio_context.store_packet_status;
    a2dp_audio_status_mutex_unlock();

    return store_packet_status;
}

int inline a2dp_audio_set_playback_status(enum A2DP_AUDIO_DECODER_PLAYBACK_STATUS playback_status)
{
    a2dp_audio_status_mutex_lock();
    a2dp_audio_context.playback_status = playback_status;
    a2dp_audio_status_mutex_unlock();

    return 0;
}

enum A2DP_AUDIO_DECODER_PLAYBACK_STATUS inline a2dp_audio_get_playback_status(void)
{
    enum A2DP_AUDIO_DECODER_PLAYBACK_STATUS playback_status;
    a2dp_audio_status_mutex_lock();
    playback_status = a2dp_audio_context.playback_status;
    a2dp_audio_status_mutex_unlock();

    return playback_status;
}

static void a2dp_decoder_info_checker(void)
{
//    app_bt_start_custom_function_in_app_thread(0, 0, (uint32_t)bt_drv_reg_op_bt_info_checker);
}

int a2dp_audio_sync_pid_config(void)
{
    A2DP_AUDIO_SYNC_T *audio_sync = &a2dp_audio_context.audio_sync;
    A2DP_AUDIO_SYNC_PID_T *pid = &audio_sync->pid;

    pid->proportiongain = 0.4f;
    pid->integralgain =   0.1f;
    pid->derivativegain = 0.6f;

    return 0;
}

int a2dp_audio_sync_reset_data(void)
{
    A2DP_AUDIO_SYNC_T *audio_sync = &a2dp_audio_context.audio_sync;

    a2dp_audio_status_mutex_lock();
    audio_sync->tick = 0;
    audio_sync->cnt = 0;
    a2dp_audio_sync_pid_config();
#ifdef __A2DP_AUDIO_SYNC_FIX_DIFF_NOPID__
    a2dp_audio_sync_fix_diff_reset();
#endif
    a2dp_audio_status_mutex_unlock();

    TRACE_A2DP_DECODER_I("[SYNC]reset_data");
    return 0;
}

int a2dp_audio_sync_init(double ratio)
{
#ifdef __A2DP_AUDIO_SYNC_FIX_DIFF_NOPID__
    a2dp_audio_sync_fix_diff_reset();
#endif
    a2dp_audio_sync_reset_data();
    a2dp_audio_sync_tune_sample_rate(ratio);
    sync_tune_dest_ratio = (float)ratio;
    return 0;
}

int a2dp_audio_sync_tune_sample_rate(double ratio)
{
    float resample_rate_ratio;
    bool need_tune = false;

    float convertedRatio = (float)ratio;
    a2dp_audio_status_mutex_lock();
    if (a2dp_audio_context.output_cfg.factor_reference != convertedRatio){
        a2dp_audio_context.output_cfg.factor_reference = convertedRatio;

        resample_rate_ratio = convertedRatio - 1.0f;
        need_tune = true;
    }

    //a2dp_audio_status_mutex_unlock();

    if (need_tune){
        app_audio_manager_tune_samplerate_ratio(AUD_STREAM_PLAYBACK, resample_rate_ratio);
        TRACE_A2DP_DECODER_I("[SYNC] ppb:%d ratio:%08d",
                                                   (int32_t)(resample_rate_ratio * 10000000),
                                                   (int32_t)(ratio * 10000000));
    }
    a2dp_audio_status_mutex_unlock();

    return 0;
}

int a2dp_audio_sync_direct_tune_sample_rate(double ratio)
{
    float resample_rate_ratio;
    bool need_tune = false;

    a2dp_audio_status_mutex_lock();
    if (a2dp_audio_context.output_cfg.factor_reference != (float)ratio){
        a2dp_audio_context.output_cfg.factor_reference = (float)ratio;
        resample_rate_ratio = (float)(ratio - (double)1.0);
        need_tune = true;
    }

    if (need_tune){
        af_codec_direct_tune(AUD_STREAM_PLAYBACK, resample_rate_ratio);

        TRACE_A2DP_DECODER_I("[SYNC] ppb:%d ratio:%08d",
                                       (int32_t)(resample_rate_ratio * 10000000),
                                       (int32_t)(ratio * 10000000));
    }
    a2dp_audio_status_mutex_unlock();

    return 0;
}

bool a2dp_audio_sync_tune_onprocess(void)
{
    bool nRet = false;;
    if (a2dp_audio_context.output_cfg.factor_reference != sync_tune_dest_ratio){
        nRet = true;
    }
    return nRet;
}

int a2dp_audio_sync_tune(uint8_t device_id, float ratio)
{
    int nRet = 0;
    POSSIBLY_UNUSED struct BT_DEVICE_T *curr_device = NULL;

    if (sync_tune_dest_ratio == ratio){
        goto exit;
    }

    sync_tune_dest_ratio = ratio;
#if defined(IBRT)
    curr_device = app_bt_get_device(device_id);
    if (APP_IBRT_IS_PROFILE_EXCHANGED(&curr_device->remote)){
        if (APP_IBRT_MOBILE_LINK_CONNECTED(&curr_device->remote)){
            APP_TWS_IBRT_AUDIO_SYNC_TUNE_T sync_tune;
            sync_tune.factor_reference = ratio;
#ifdef IBRT_V2_MULTIPOINT
            sync_tune.mobile_addr = curr_device->remote;
#endif
            if (!app_tws_ibrt_audio_sync_tune_need_skip()){
                int ret = tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_TUNE, (uint8_t*)&sync_tune, sizeof(APP_TWS_IBRT_AUDIO_SYNC_TUNE_T));
                if (0 != ret)
                {
                    a2dp_audio_sync_tune_cancel();
                    nRet = -1;
                }

            }else{
                a2dp_audio_sync_tune_cancel();
                nRet = -1;
            }
        }
    }else{
        a2dp_audio_sync_tune_sample_rate(ratio);
    }
#else
    a2dp_audio_sync_tune_sample_rate(ratio);
#endif
exit:
    return nRet;
}

int a2dp_audio_sync_tune_cancel(void)
{
    sync_tune_dest_ratio = a2dp_audio_context.output_cfg.factor_reference;
    return 0;
}

float a2dp_audio_sync_pid_calc(A2DP_AUDIO_SYNC_PID_T *pid, float diff)
{
    float increment;
    float pError,dError,iError;

    pid->error[0] = diff;
    pError=pid->error[0]-pid->error[1];
    iError=pid->error[0];
    dError=pid->error[0]-2*pid->error[1]+pid->error[2];

    increment = pid->proportiongain*pError+pid->integralgain*iError+pid->derivativegain*dError;

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];

    pid->result += increment;

    return pid->result;
}

#ifdef __A2DP_AUDIO_SYNC_FIX_DIFF_NOPID__
static int a2dp_audio_sync_fix_diff_proc(uint32_t tick)
{
    if (a2dp_audio_sync_fix_diff.status == A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_START){
        if (a2dp_audio_sync_fix_diff.tick > 0){
            a2dp_audio_sync_fix_diff.tick--;
        }else{
            a2dp_audio_sync_fix_diff_stop(0);
        }
    }
    return 0;
}

static int32_t a2dp_audio_sync_fix_diff_start(uint32_t tick)
{
    TRACE_A2DP_DECODER_I("[SYNC] fix diff start");

    a2dp_audio_sync_fix_diff.status = A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_START;
    a2dp_audio_sync_fix_diff.tick = tick;
    return 0;
}

static int a2dp_audio_sync_fix_diff_stop(uint32_t tick)
{
    TRACE_A2DP_DECODER_I("[SYNC] fix diff stop");

    a2dp_audio_sync_fix_diff.status = A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_STOP;
    a2dp_audio_sync_fix_diff.tick = 0;

    return 0;
}

static int32_t a2dp_audio_sync_fix_diff_reset(void)
{
    TRACE_A2DP_DECODER_I("[SYNC] fix diff reset");

    a2dp_audio_sync_fix_diff.status = A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_IDLE;
    a2dp_audio_sync_fix_diff.tick = 0;

    return 0;
}

static A2DP_AUDIO_SYNC_FIX_DIFF_STATUS a2dp_audio_sync_fix_diff_status_get(void)
{
    return a2dp_audio_sync_fix_diff.status;
}

static int a2dp_audio_sync_handler(uint8_t device_id, uint8_t *buffer, uint32_t buffer_bytes)
{
    A2DP_AUDIO_LASTFRAME_INFO_T *lastframe_info = NULL;
    A2DP_AUDIO_SYNC_T *audio_sync = &a2dp_audio_context.audio_sync;
    float diff_mtu = 0;
    bool need_tune = false;
    bool force_slow = false;
    bool force_fast = false;
    A2DP_AUDIO_SYNC_FIX_DIFF_STATUS fix_diff_status;
    bool a2dp_tune_onprocess;
#if defined(IBRT)
    bool ibrt_tune_onprocess;
    ibrt_tune_onprocess = app_tws_ibrt_audio_sync_tune_onprocess();
    POSSIBLY_UNUSED struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    if (!APP_IBRT_MOBILE_LINK_CONNECTED(&curr_device->remote)){
        return -1;
    }
#endif
    if (a2dp_audio_internal_lastframe_info_ptr_get(&lastframe_info) < 0){
        return -1;
    }

    a2dp_audio_sync_fix_diff_proc(audio_sync->tick);
    fix_diff_status = a2dp_audio_sync_fix_diff_status_get();
    a2dp_tune_onprocess = a2dp_audio_sync_tune_onprocess();
    if (fix_diff_status == A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_STOP){
#if defined(IBRT)
        if (!ibrt_tune_onprocess &&
            !a2dp_tune_onprocess &&
            !app_ibrt_if_is_tws_role_switch_on()
#else
        if (!a2dp_tune_onprocess
#endif
           ){
            int sync_tune_result = 0;
            if (a2dp_audio_context.output_cfg.factor_reference != a2dp_audio_context.init_factor_reference){
                sync_tune_result = a2dp_audio_sync_tune(device_id, a2dp_audio_context.init_factor_reference);
            }
            if (!sync_tune_result){
                a2dp_audio_sync_fix_diff_reset();
#ifdef A2DP_AUDIO_SYNC_FIX_DIFF_INTERVA_PRINT_FLOAT
                TRACE_A2DP_DECODER_I("[SYNC] tune ratio normal %f mut:%5.3f->%d",
                                    (double)a2dp_audio_context.init_factor_reference,
                                    (double)a2dp_audio_context.average_packet_mut,
                                    a2dp_audio_context.dest_packet_mut);
#else
                TRACE_A2DP_DECODER_I("[SYNC] tune ratio normal %d mut:%d->%d",
                                    (int32_t)(a2dp_audio_context.init_factor_reference * 10000000),
                                    (int32_t)(a2dp_audio_context.average_packet_mut+0.5f),
                                    a2dp_audio_context.dest_packet_mut);
#endif
            }else{
                TRACE_A2DP_DECODER_I("[SYNC] tune ratio normal busy");
            }

        }else{
#ifdef A2DP_AUDIO_SYNC_FIX_DIFF_INTERVA_PRINT_FLOAT
                TRACE_A2DP_DECODER_I("[SYNC] tune ratio busy %f mut:%5.3f->%d",
                                    (double)a2dp_audio_context.init_factor_reference,
                                    (double)a2dp_audio_context.average_packet_mut,
                                    a2dp_audio_context.dest_packet_mut);
#else
                TRACE_A2DP_DECODER_I("[SYNC] tune ratio busy %d mut:%d->%d",
                                    (int32_t)(a2dp_audio_context.init_factor_reference * 10000000),
                                    (int32_t)(a2dp_audio_context.average_packet_mut+0.5f),
                                    a2dp_audio_context.dest_packet_mut);
#endif

        #if defined(IBRT)
            TRACE_A2DP_DECODER_I("[SYNC] tune busy reason %d-%d-%d",
              app_tws_ibrt_audio_sync_tune_onprocess(),
              a2dp_audio_sync_tune_onprocess(),
              app_ibrt_if_is_tws_role_switch_on());
        #endif
        }
        goto exit;
    }

    if (audio_sync->tick%A2DP_AUDIO_SYNC_INTERVAL == 0){
        diff_mtu = a2dp_audio_context.average_packet_mut-(float)a2dp_audio_context.dest_packet_mut;
        TRACE(0, "average_packet_mut %d dest_packet_mut %d diff_mtu %d",
            (uint32_t)a2dp_audio_context.average_packet_mut, a2dp_audio_context.dest_packet_mut, (int32_t)diff_mtu*10);
        if (ABS(diff_mtu) < 0.6f){
#ifdef A2DP_AUDIO_SYNC_FIX_DIFF_INTERVA_PRINT_FLOAT
            TRACE_A2DP_DECODER_I("[SYNC] skip mut:%5.3f", (double)diff_mtu);
#else
            TRACE_A2DP_DECODER_I("[SYNC] skip mut:0.%d", (int32_t)(diff_mtu*10));
#endif
            sbm_target_jitter_buffer_updating_completed();
            goto exit;
        }else if (diff_mtu/a2dp_audio_context.dest_packet_mut < -0.2f){
            float curr_ratio = a2dp_audio_context.output_cfg.factor_reference;
            float ref_ratio  = a2dp_audio_context.init_factor_reference;
            if (curr_ratio != (ref_ratio + A2DP_AUDIO_SYNC_FIX_DIFF_SLOW_LIMIT)){
#ifdef A2DP_AUDIO_SYNC_FIX_DIFF_INTERVA_PRINT_FLOAT
                TRACE_A2DP_DECODER_I("[SYNC] force slow mut:%5.3f", (double)diff_mtu);
#else
                TRACE_A2DP_DECODER_I("[SYNC] force slow mut:0.%d", (int32_t)(diff_mtu*10));
#endif
                force_slow = true;
            }
            sbm_set_auto_adjustment();
        } else if (diff_mtu/a2dp_audio_context.dest_packet_mut > 0.2f) {
            float curr_ratio = a2dp_audio_context.output_cfg.factor_reference;
            float ref_ratio  = a2dp_audio_context.init_factor_reference;
            if (curr_ratio != (ref_ratio + A2DP_AUDIO_SYNC_FIX_DIFF_FAST_LIMIT)){
#ifdef A2DP_AUDIO_SYNC_FIX_DIFF_INTERVA_PRINT_FLOAT
                TRACE_A2DP_DECODER_I("[SYNC] force fast mut:%5.3f", (double)diff_mtu);
#else
                TRACE_A2DP_DECODER_I("[SYNC] force fast mut:0.%d", (int32_t)(diff_mtu*10));
#endif
                force_fast = true;
            }
            sbm_set_auto_adjustment();
        }
        if (fix_diff_status == A2DP_AUDIO_SYNC_FIX_DIFF_STATUS_IDLE || force_slow || force_fast){
            uint32_t dma_buffer_samples, samples, dma_interval;
            double ratio = 1.0, limit_ratio = 1.0;
            float sampleRate, ref_ratio, curr_ratio;
            float ref_us  = 0;
            float dest_us  = 0;
            float sample_us =0;

            dma_buffer_samples = app_bt_stream_get_dma_buffer_samples()/2;
            dma_interval = A2DP_AUDIO_SYNC_FIX_DIFF_INTERVAL;

            ref_ratio  = a2dp_audio_context.init_factor_reference;
            curr_ratio = a2dp_audio_context.output_cfg.factor_reference;

            samples = dma_interval*dma_buffer_samples;
            sampleRate = lastframe_info->stream_info.sample_rate * ref_ratio;

            sample_us = 1e6 / sampleRate;
            ref_us = sample_us * (float)samples;
            dest_us = sample_us * (float)(lastframe_info->frame_samples * diff_mtu);
            ratio = ref_us/(ref_us - dest_us)*curr_ratio;

            if (ratio > (double)(ref_ratio + A2DP_AUDIO_SYNC_FIX_DIFF_FAST_LIMIT)){
                limit_ratio = ref_ratio + A2DP_AUDIO_SYNC_FIX_DIFF_FAST_LIMIT;
            }else if (ratio < double(ref_ratio + A2DP_AUDIO_SYNC_FIX_DIFF_SLOW_LIMIT)){
                limit_ratio = ref_ratio + A2DP_AUDIO_SYNC_FIX_DIFF_SLOW_LIMIT;
            }else{
                limit_ratio = ratio;
            }
            TRACE(0, "limit_ratio %d", (int32_t)(ref_ratio * 10000000));
            TRACE_A2DP_DECODER_I("[SYNC] sampleRate:%d ref_ratio:%d samples:%d %d->%d", (int32_t)sampleRate,
                                                                                               (int32_t)(ref_ratio * 10000000),
                                                                                               samples,
                                                                                               (int32_t)ref_us,
                                                                                               (int32_t)dest_us);
            need_tune = true;
#if defined(IBRT)
            if (!ibrt_tune_onprocess &&
                !a2dp_tune_onprocess &&
                !app_ibrt_if_is_tws_role_switch_on() &&
#else
            if (!a2dp_tune_onprocess &&
#endif
                need_tune){
                if (!a2dp_audio_sync_tune(device_id, (float)limit_ratio)){
                    a2dp_audio_sync_fix_diff_start(dma_interval);
#ifdef A2DP_AUDIO_SYNC_FIX_DIFF_INTERVA_PRINT_FLOAT
                    TRACE_A2DP_DECODER_I("[SYNC] tune ratio %f  mut:%5.3f->%d", limit_ratio,
                                                                               (double)a2dp_audio_context.average_packet_mut,
                                                                               a2dp_audio_context.dest_packet_mut);
#else
                    TRACE_A2DP_DECODER_I("[SYNC] tune ratio %d  mut:%d->%d", (int32_t)(limit_ratio * 10000000),
                                                                               (int32_t)(a2dp_audio_context.average_packet_mut+0.5f),
                                                                               a2dp_audio_context.dest_packet_mut);
#endif
                }else{
                    TRACE_A2DP_DECODER_I("[SYNC] tune ratio busy");
                }
            }
        }else{
            TRACE_A2DP_DECODER_I("[SYNC] avg_mut:%d dest_mtu:%d", (int32_t)(a2dp_audio_context.average_packet_mut+0.5f),
                                                                                      a2dp_audio_context.dest_packet_mut);
        }
    }else{

    }
exit:
    audio_sync->tick++;
    return 0;
}
#else
static int a2dp_audio_sync_handler(uint8_t device_id, uint8_t *buffer, uint32_t buffer_bytes)
{
    A2DP_AUDIO_LASTFRAME_INFO_T *lastframe_info = NULL;
    A2DP_AUDIO_SYNC_T *audio_sync = &a2dp_audio_context.audio_sync;
    float dest_pid_result = .0f;
    float diff_mtu = 0;
    int32_t frame_mtu = 0;
    int32_t total_mtu = 0;
    float diff_factor = 0;
    bool a2dp_tune_onprocess;

#if defined(IBRT)
    bool ibrt_tune_onprocess;
    POSSIBLY_UNUSED struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    if (!APP_IBRT_MOBILE_LINK_CONNECTED(&curr_device->remote)){
        return -1;
    }
    ibrt_tune_onprocess = app_tws_ibrt_audio_sync_tune_onprocess();
#endif
    if (a2dp_audio_internal_lastframe_info_ptr_get(&lastframe_info) < 0){
        return -1;
    }

    a2dp_tune_onprocess = a2dp_audio_sync_tune_onprocess();

    if (audio_sync->tick++%A2DP_AUDIO_SYNC_INTERVAL == 0){
        list_t *list = a2dp_audio_context.audio_datapath.input_raw_packet_list;
        A2DP_AUDIO_SYNC_PID_T *pid = &audio_sync->pid;
        //valid limter 0x80000
        if (audio_sync->cnt < 0x80000){
            audio_sync->cnt += A2DP_AUDIO_SYNC_INTERVAL;
        }
        frame_mtu = lastframe_info->stream_info.frame_samples/lastframe_info->frame_samples;
        total_mtu = audio_sync->cnt * frame_mtu;
        diff_mtu = a2dp_audio_context.average_packet_mut-(float)a2dp_audio_context.dest_packet_mut;
#if 1
        TRACE_A2DP_DECODER_I("[SYNC] sample:%d/%d diff:%d/%d/%d/%d curr:%d", lastframe_info->frame_samples,
                                                             lastframe_info->stream_info.frame_samples,
                                                             (int32_t)(diff_mtu+0.5f),
                                                             (int32_t)(a2dp_audio_context.average_packet_mut+0.5f),
                                                             a2dp_audio_context.dest_packet_mut,
                                                             total_mtu,
                                                             a2dp_audio_list_length(list) +  get_in_cp_frame_cnt());
#else
        TRACE_A2DP_DECODER_I("[SYNC] diff:%10.9f/%10.9f frame_mut:%d dest:%d total:%d curr:%d", (double)diff_mtu, (double)a2dp_audio_context.average_packet_mut, frame_mtu,
                                                                                      a2dp_audio_context.dest_packet_mut, total_mtu, a2dp_audio_list_length(list));
        TRACE_A2DP_DECODER_I("[SYNC] try tune:%d, %10.9f %10.9f",  diff_mtu != 0.f && audio_sync->tick != 1 ? ABS(diff_mtu) > ((float)frame_mtu * 0.2f): 0,
                                                        (double)ABS(diff_mtu), (double)((float)frame_mtu * 0.2f));
#endif
        //TRACE(2,"audio_sync tune %d/%d tick", ibrt_tune_onprocess, a2dp_tune_onprocess, audio_sync->tick);

#if defined(IBRT)
        if ((!ibrt_tune_onprocess &&
             !a2dp_tune_onprocess) &&
             !app_ibrt_if_is_tws_role_switch_on() &&
#else
        if (!a2dp_tune_onprocess &&
#endif
             diff_mtu != 0.f && audio_sync->tick != 1 &&
            ((ABS(diff_mtu) > ((float)frame_mtu * 0.25f) && diff_mtu > 0)||
             (ABS(diff_mtu) > ((float)frame_mtu * 0.1f) && diff_mtu < 0))){
            diff_factor = diff_mtu/a2dp_audio_context.average_packet_mut;
            sbm_set_auto_adjustment();
            if (a2dp_audio_sync_pid_calc(pid, diff_factor)){
                dest_pid_result = a2dp_audio_context.output_cfg.factor_reference + pid->result;

                if (dest_pid_result > (A2DP_AUDIO_SYNC_FACTOR_REFERENCE + A2DP_AUDIO_SYNC_FACTOR_FAST_LIMIT)){
                    dest_pid_result = A2DP_AUDIO_SYNC_FACTOR_REFERENCE + A2DP_AUDIO_SYNC_FACTOR_FAST_LIMIT;
                }else if (dest_pid_result < (A2DP_AUDIO_SYNC_FACTOR_REFERENCE + A2DP_AUDIO_SYNC_FACTOR_SLOW_LIMIT)){
                    dest_pid_result = A2DP_AUDIO_SYNC_FACTOR_REFERENCE + A2DP_AUDIO_SYNC_FACTOR_SLOW_LIMIT;
                }

                if (a2dp_audio_context.output_cfg.factor_reference != dest_pid_result){
                    if (!a2dp_audio_sync_tune(device_id, dest_pid_result)){
                        audio_sync->cnt = 0;
                    }
                    TRACE_A2DP_DECODER_I("[SYNC] tune diff_factor:%10.9f pid:%10.9f tune:%10.9f", (double)diff_factor, (double)pid->result, (double)dest_pid_result);
                }else{
                    TRACE_A2DP_DECODER_I("[SYNC] tune skip same");
                }
            }
        }else{
            sbm_target_jitter_buffer_updating_completed();
/*
            TRACE_A2DP_DECODER_I("[SYNC] tune busy skip proc:%d/%d mtu:%d tick:%d >0:%d <0:%d tick:%d", ibrt_tune_onprocess,
                                                a2dp_tune_onprocess,
                                                diff_mtu != 0.f,
                                                audio_sync->tick != 1,
                                                (ABS(diff_mtu) > ((float)frame_mtu * 0.25f) && diff_mtu > 0),
                                                (ABS(diff_mtu) > ((float)frame_mtu * 0.1f) && diff_mtu < 0),
                                                audio_sync->tick);
*/
        }
    }else{
        bool need_tune = false;
        if (lastframe_info->undecode_min_frames*10 <= a2dp_audio_context.dest_packet_mut*10/3){
            dest_pid_result = a2dp_audio_context.init_factor_reference + A2DP_AUDIO_SYNC_FACTOR_NEED_FAST_CACHE;
            need_tune = true;
        }else if (lastframe_info->undecode_min_frames*10 <= a2dp_audio_context.dest_packet_mut*20/3){
            dest_pid_result = a2dp_audio_context.init_factor_reference + A2DP_AUDIO_SYNC_FACTOR_SLOW_LIMIT;
            need_tune = true;
        }
#if defined(IBRT)
        if (!ibrt_tune_onprocess &&
            !a2dp_tune_onprocess &&
            !app_ibrt_if_is_tws_role_switch_on() &&
#else
        if (!a2dp_tune_onprocess &&
#endif

            need_tune){
            if (a2dp_audio_context.output_cfg.factor_reference != dest_pid_result){
                a2dp_audio_sync_reset_data();
                a2dp_audio_sync_tune(device_id, dest_pid_result);
                TRACE_A2DP_DECODER_I("[SYNC] tune ratio force slow %d/%d->%d", lastframe_info->undecode_min_frames,
                                                                    lastframe_info->undecode_max_frames,
                                                                    a2dp_audio_context.dest_packet_mut);
            }
        }
    }
    return 0;
}
#endif

#if A2DP_DECODER_HISTORY_SEQ_SAVE
static int a2dp_audio_reset_history_seq(void)
{
    a2dp_audio_status_mutex_lock();
    for (uint8_t i=0; i<A2DP_DECODER_HISTORY_SEQ_SAVE; i++){
        a2dp_audio_context.historySeq[i] = 0;
#ifdef A2DP_DECODER_HISTORY_LOCTIME_SAVE
        a2dp_audio_context.historyLoctime[i] = 0;
#endif
#ifdef A2DP_DECODER_HISTORY_CHECK_SUM_SAVE
        a2dp_audio_context.historyChecksum[i] = 0;
#endif

    }
    a2dp_audio_context.historySeq_idx = 0;
    a2dp_audio_status_mutex_unlock();

    return 0;
}

static int a2dp_audio_save_history_seq(btif_media_header_t *header, unsigned char *buf, unsigned int len)
{
    uint16_t historySeqPre = 0;
    uint8_t historySeq_idx = 0;
    uint8_t ret = 0;
    int16_t diff_seq = 0;

    a2dp_audio_status_mutex_lock();
    historySeq_idx = a2dp_audio_context.historySeq_idx;
    if (historySeq_idx){
        historySeq_idx = (historySeq_idx-1)%A2DP_DECODER_HISTORY_SEQ_SAVE;
        historySeqPre = a2dp_audio_context.historySeq[historySeq_idx];
        diff_seq = header->sequenceNumber - historySeqPre;
        if (diff_seq!= 1){
            if ((a2dp_audio_context.max_seq_num==UINT16_MAX)||(a2dp_audio_context.max_seq_num==UINT8_MAX&&diff_seq!=UINT8_MAX))
            {
                TRACE_A2DP_DECODER_W("[INPUT] SEQ ERR %d/%d",historySeqPre, header->sequenceNumber);
                app_start_post_chopping_timer();
                a2dp_audio_show_history_seq();
            }
        }
    }
    if (a2dp_audio_context.historySeq[historySeq_idx] == header->sequenceNumber)
    {
        TRACE_A2DP_DECODER_W("[INPUT] receive same seq");
        ret = A2DP_DECODER_HISTORY_SAME_SEQ;
    }
    historySeq_idx = a2dp_audio_context.historySeq_idx%A2DP_DECODER_HISTORY_SEQ_SAVE;
    a2dp_audio_context.historySeq[historySeq_idx] = header->sequenceNumber;
#ifdef A2DP_DECODER_HISTORY_LOCTIME_SAVE
    a2dp_audio_context.historyLoctime[historySeq_idx] = hal_fast_sys_timer_get();
#endif
#ifdef A2DP_DECODER_HISTORY_CHECK_SUM_SAVE
    a2dp_audio_context.historyChecksum[historySeq_idx] = crc32(0, buf, len);
#endif
    a2dp_audio_context.historySeq_idx++;
    a2dp_audio_status_mutex_unlock();

    return ret;
}

int a2dp_audio_show_history_seq(void)
{
    uint8_t i = 0, j = 1;
    uint16_t reordHistorySeq[A2DP_DECODER_HISTORY_SEQ_SAVE];
#ifdef A2DP_DECODER_HISTORY_LOCTIME_SAVE
    int32_t diff_max_idx = 0;
    int32_t diff_max_ms = 0;
    int64_t diff_avg_ms = 0;
    int32_t diff_avg_cnt = 0;
    uint32_t historyLoctime[A2DP_DECODER_HISTORY_SEQ_SAVE];
#endif
#ifdef A2DP_DECODER_HISTORY_CHECK_SUM_SAVE
    uint32_t historyChecksum[A2DP_DECODER_HISTORY_SEQ_SAVE];
#endif

    a2dp_audio_status_mutex_lock();
    for (i=0; i<A2DP_DECODER_HISTORY_SEQ_SAVE; i++) {
            reordHistorySeq[i] = a2dp_audio_context.historySeq[i];
#ifdef A2DP_DECODER_HISTORY_LOCTIME_SAVE
            historyLoctime[i] = a2dp_audio_context.historyLoctime[i];
#endif
#ifdef A2DP_DECODER_HISTORY_CHECK_SUM_SAVE
            historyChecksum[i] = a2dp_audio_context.historyChecksum[i];
#endif

    }

    for (i=0; i<A2DP_DECODER_HISTORY_SEQ_SAVE-1; i++) {
        for (j=0; j<A2DP_DECODER_HISTORY_SEQ_SAVE-1-i; j++) {
            if (reordHistorySeq[j] > reordHistorySeq[j+1]) {
                uint16_t temp_seq = reordHistorySeq[j];
                reordHistorySeq[j] = reordHistorySeq[j+1];
                reordHistorySeq[j+1] = temp_seq;
#ifdef A2DP_DECODER_HISTORY_LOCTIME_SAVE
                uint32_t temp_Loctime = historyLoctime[j];
                historyLoctime[j] = historyLoctime[j+1];
                historyLoctime[j+1] = temp_Loctime;
#endif
#ifdef A2DP_DECODER_HISTORY_CHECK_SUM_SAVE
                uint32_t temp_Checksum = historyChecksum[j];
                historyChecksum[j] = historyChecksum[j+1];
                historyChecksum[j+1] = temp_Checksum;
#endif
            }
        }
    }
#ifdef A2DP_DECODER_HISTORY_LOCTIME_SAVE
    for (i = 0, j = 1; i<A2DP_DECODER_HISTORY_SEQ_SAVE-1; i++, j++){
        int32_t tmp_ms = historyLoctime[j] - historyLoctime[i];
        diff_avg_ms += tmp_ms;
        diff_avg_cnt++;
        if (tmp_ms > diff_max_ms){
            diff_max_ms = tmp_ms;
            diff_max_idx = i;
        }
        if (tmp_ms > (int32_t)MS_TO_FAST_TICKS(50)){
            TRACE_A2DP_DECODER_I("[INPUT] > 30ms seq:%d diff:%d :%d / %d", reordHistorySeq[i], FAST_TICKS_TO_MS(tmp_ms), historyLoctime[i], historyLoctime[j]);
        }
    }
    diff_avg_ms /= diff_avg_cnt;
    TRACE_A2DP_DECODER_I("[INPUT] max_diff:%dms idx:%d avg:%dus", FAST_TICKS_TO_MS(diff_max_ms), diff_max_idx, FAST_TICKS_TO_US(diff_avg_ms));
#endif
//    DUMP16("%d ", reordHistorySeq, A2DP_DECODER_HISTORY_SEQ_SAVE);
#ifdef A2DP_DECODER_HISTORY_LOCTIME_SAVE
    DUMP32("%x ", historyLoctime, A2DP_DECODER_HISTORY_SEQ_SAVE);
#endif
#ifdef A2DP_DECODER_HISTORY_CHECK_SUM_SAVE
    DUMP32("%x ", historyChecksum, A2DP_DECODER_HISTORY_SEQ_SAVE);
#endif
    a2dp_audio_status_mutex_unlock();

    return 0;
}

#endif

/*
1,  2^1
3,  2^2
7,  2^3
15, 2^4
31, 2^5
*/
#define AUDIO_ALPHA_PRAMS_1 (3)
#define AUDIO_ALPHA_PRAMS_2 (4)

static inline float a2dp_audio_alpha_filter(float y, float x)
{
    if (y){
        y = ((AUDIO_ALPHA_PRAMS_1*y)+x)/AUDIO_ALPHA_PRAMS_2;
    }else{
        y = x;
    }
    return y;
}

static void inline a2dp_audio_convert_16bit_to_24bit(int32_t *out, int16_t *in, int len)
{
    for (int i = len - 1; i >= 0; i--) {
        out[i] = ((int32_t)in[i] << 8);
    }
}

#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
void a2dp_audio_stereo_mix_init(uint32_t smooth_len)
{
    TRACE(3, "[%s] smooth_len: %d, chnl_sel: %d", __func__,
        smooth_len,
        a2dp_audio_context.chnl_sel);

    g_mix_cos_coeff = (float *)a2dp_audio_heap_cmalloc(smooth_len * sizeof(float));
    for (uint32_t i=0; i<smooth_len; i++) {
        g_mix_cos_coeff[i] = arm_cos_f32(PI * i / smooth_len) / 2 + 0.5;
    }

    g_mix_coeff_len = smooth_len;
}

template<typename PcmType>
void stereo_mix_smooth(PcmType *pcm_buf, uint32_t frame_len, uint32_t ch)
{
    float alpha = 1.0f;

    ASSERT(ch < 2, "[%s] ch(%d) is invalid.", __func__, ch);
    ASSERT(frame_len == g_mix_coeff_len, "[%s] frame_len(%d) is invalid.", __func__, frame_len);

    for (uint32_t i=0; i<frame_len; i++) {
        alpha = 0.5 + 0.5 * g_mix_cos_coeff[i];
        pcm_buf[2 * i] = (PcmType)(pcm_buf[2 * i + ch] * alpha + pcm_buf[2 * i + 1 - ch] * (1.0f - alpha));
        pcm_buf[2 * i + 1] = pcm_buf[2 * i];
    }
}

template<typename PcmType>
void stereo_demix_smooth(PcmType *pcm_buf, uint32_t frame_len, uint32_t ch)
{
    float alpha = 0.0f;

    ASSERT(ch < 2, "[%s] ch(%d) is invalid.", __func__, ch);
    ASSERT(frame_len == g_mix_coeff_len, "[%s] frame_len(%d) is invalid.", __func__, frame_len);

    for (uint32_t i=0; i<frame_len; i++) {
        alpha = 1.0 - 0.5 * g_mix_cos_coeff[i];
        pcm_buf[2 * i] = (PcmType)(pcm_buf[2 * i + ch] * alpha + pcm_buf[2 * i + 1 - ch] * (1.0f - alpha));
        pcm_buf[2 * i + 1] = pcm_buf[2 * i];
    }
}
#endif

bool a2dp_audio_stereo_is_mixed(void)
{
#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
    return (a2dp_audio_context.chnl_sel == A2DP_AUDIO_CHANNEL_SELECT_LRMERGE);
#else
    return false;
#endif
}

void a2dp_audio_stereo_set_mix(bool enable)
{
#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
    a2dp_audio_context.chnl_sel_new = enable ? A2DP_AUDIO_CHANNEL_SELECT_LRMERGE : (A2DP_AUDIO_CHANNEL_SELECT_E)app_tws_ibrt_get_bt_ctrl_ctx()->audio_chnl_sel;

    TRACE(4, "[%s] enable: %d, channel: %d --> %d", __func__,
        enable,
        a2dp_audio_context.chnl_sel,
        a2dp_audio_context.chnl_sel_new);
#endif
}

template<typename PcmType>
static void inline a2dp_audio_channel_select(A2DP_AUDIO_CHANNEL_SELECT_E chnl_sel, PcmType *pcm_buf, uint32_t pcm_len)
{
    uint32_t i;
    uint32_t ch_num = a2dp_audio_context.output_cfg.num_channels;
    uint32_t frame_len = pcm_len / ch_num;

#ifdef PLAYBACK_FORCE_48K
    // When resample is enabled, data depth conversion should be done after resample as resample only support 16bit
    ch_num = a2dp_audio_context.audio_decoder.stream_info.num_channels;
#endif

    ASSERT_A2DP_DECODER(ch_num == 2, "%s num_channels:%d", __func__, ch_num);

#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
    bool chnl_sel_changed = false;
    uint32_t lock = int_lock_global();
    if (a2dp_audio_context.chnl_sel != a2dp_audio_context.chnl_sel_new) {
        a2dp_audio_context.chnl_sel = a2dp_audio_context.chnl_sel_new;
        chnl_sel = a2dp_audio_context.chnl_sel;
        chnl_sel_changed = true;
    }
    int_unlock_global(lock);

    if (chnl_sel_changed == false) {
#endif
        PcmType *pcm_buf_l_p = pcm_buf;
        PcmType *pcm_buf_r_p = pcm_buf + 1;
        switch (chnl_sel) {
            case A2DP_AUDIO_CHANNEL_SELECT_LRMERGE:
                for (i = 0; i<frame_len; i++, pcm_buf_l_p+=ch_num, pcm_buf_r_p+=ch_num){
                    PcmType tmp_sample = ((int32_t)*pcm_buf_l_p + (int32_t)*pcm_buf_r_p) >> 1;
                    *pcm_buf_l_p = tmp_sample;
                    *pcm_buf_r_p = tmp_sample;
                }
                break;
            case A2DP_AUDIO_CHANNEL_SELECT_LCHNL:
                for (i = 0; i<frame_len; i++, pcm_buf_l_p+=ch_num, pcm_buf_r_p+=ch_num){
                    *pcm_buf_r_p = *pcm_buf_l_p;
                }
                break;
            case A2DP_AUDIO_CHANNEL_SELECT_RCHNL:
                for (i = 0; i<frame_len; i++, pcm_buf_l_p+=ch_num, pcm_buf_r_p+=ch_num){
                    *pcm_buf_l_p = *pcm_buf_r_p;
                }
                break;
            case A2DP_AUDIO_CHANNEL_SELECT_STEREO:
#ifdef __AUDIO_REVERB__
                if (app_tws_ibrt_get_bt_ctrl_ctx()->nv_role == IBRT_SLAVE) {
                    for (i = 0; i<frame_len; i++, pcm_buf_l_p+=ch_num, pcm_buf_r_p+=ch_num){
                        int32_t tmp = *pcm_buf_r_p;
                        *pcm_buf_r_p = *pcm_buf_l_p;
                        *pcm_buf_l_p = tmp;
                    }
                }
                break;
#endif
            default:
                break;
        }
#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
    } else {
        switch (chnl_sel) {
            case A2DP_AUDIO_CHANNEL_SELECT_LRMERGE:
                if (app_tws_ibrt_get_bt_ctrl_ctx()->audio_chnl_sel == A2DP_AUDIO_CHANNEL_SELECT_LCHNL) {
                    TRACE(1, "[%s] Left mix", __func__);
                    stereo_mix_smooth<PcmType>(pcm_buf, frame_len, 0);
                } else {
                    TRACE(1, "[%s] Right mix", __func__);
                    stereo_mix_smooth<PcmType>(pcm_buf, frame_len, 1);
                }
                break;
            case A2DP_AUDIO_CHANNEL_SELECT_LCHNL:
                TRACE(1, "[%s] Left demix", __func__);
                stereo_demix_smooth<PcmType>(pcm_buf, frame_len, 0);
                break;
            case A2DP_AUDIO_CHANNEL_SELECT_RCHNL:
                TRACE(1, "[%s] Right demix", __func__);
                stereo_demix_smooth<PcmType>(pcm_buf, frame_len, 1);
                break;
            case A2DP_AUDIO_CHANNEL_SELECT_STEREO:
            default:
                break;
        }
    }
#endif
}


#ifdef A2DP_CP_ACCEL
extern uint32_t app_bt_stream_get_dma_buffer_samples(void);

static uint32_t get_cp_frame_mtus(A2DP_AUDIO_LASTFRAME_INFO_T*  info)
{
    uint32_t cp_frame_mtus = app_bt_stream_get_dma_buffer_samples()/2;
    if (cp_frame_mtus %(info->frame_samples) ){
        cp_frame_mtus = cp_frame_mtus /(info->frame_samples) +1;
    }else{
        cp_frame_mtus = cp_frame_mtus /(info->frame_samples);
    }

    TRACE_A2DP_DECODER_I("[CP] cp_frame_mtus:%d", cp_frame_mtus);

    return cp_frame_mtus;
}

#else
bool is_cp_need_reset(void)
{
    return false;
}


#endif

#define  A2DP_AUDIO_SYSFREQ_BOOST_RESUME_CNT                   (20)
static uint32_t a2dp_audio_sysfreq_cnt = UINT32_MAX;
static uint32_t a2dp_audio_sysfreq_dest_boost_cnt = 0;
static APP_SYSFREQ_FREQ_T a2dp_audio_sysfreq_normalfreq = APP_SYSFREQ_52M;

#ifdef A2DP_DECODER_CROSS_CORE
static const enum APP_SYSFREQ_FREQ_T a2dp_audio_freq_table[] =  {
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_ONE_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_HIGH_RSSI_CASE
    APP_SYSFREQ_26M,
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_ONE_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_LOW_RSSI_CASE
    APP_SYSFREQ_52M,
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_TWO_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_HIGH_RSSI_CASE
    APP_SYSFREQ_26M,
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_TWO_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_LOW_RSSI_CASE
    APP_SYSFREQ_52M,
    //A2DP_AUDIO_BOOST_MODE_FREQ*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_HIGH_RSSI_CASE
    APP_SYSFREQ_104M,
    //A2DP_AUDIO_BOOST_MODE_FREQ*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_LOW_RSSI_CASE
    APP_SYSFREQ_104M,
};
#else
static const enum APP_SYSFREQ_FREQ_T a2dp_audio_freq_table[] =  {
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_ONE_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_HIGH_RSSI_CASE
    APP_SYSFREQ_26M,
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_ONE_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_LOW_RSSI_CASE
    APP_SYSFREQ_52M,
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_TWO_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_HIGH_RSSI_CASE
    APP_SYSFREQ_52M,
    //A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_TWO_MOBILE_LINK*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_LOW_RSSI_CASE
    APP_SYSFREQ_52M,
    //A2DP_AUDIO_BOOST_MODE_FREQ*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_HIGH_RSSI_CASE
    APP_SYSFREQ_104M,
    //A2DP_AUDIO_BOOST_MODE_FREQ*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+A2DP_AUDIO_DYNAMIC_FREQ_LOW_RSSI_CASE
    APP_SYSFREQ_104M,
};
#endif

static A2DP_AUDIO_FREQ_USER_CASE_INDEX_E a2dp_audio_current_freq_user_case = A2DP_AUDIO_FREQ_USER_CASE_INVALID;
static A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_INDEX_E a2dp_audio_current_freq_rssi_case = A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_INVALID;
static enum APP_SYSFREQ_FREQ_T a2dp_audio_current_sysfreq = APP_SYSFREQ_32K;

#define A2DP_AUDIO_ACCUMULCATED_CONTINUOUS_HIGH_RSSI_COUNT_THR_TO_SWITCH_FREQ_CASE   100
static uint32_t a2dp_audio_accumulated_continuous_high_rssi_count = 0;

static void a2dp_audio_sysfreq_refesh(void)
{
    enum APP_SYSFREQ_FREQ_T freq;
    enum APP_SYSFREQ_FREQ_T miniFreq =
        a2dp_audio_freq_table[a2dp_audio_current_freq_user_case*A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_CNT+a2dp_audio_current_freq_rssi_case];
    if (a2dp_audio_sysfreq_normalfreq > miniFreq)
    {
        freq = a2dp_audio_sysfreq_normalfreq;
    }
    else
    {
        freq = miniFreq;
    }

    if (a2dp_audio_current_sysfreq != freq)
    {
        app_audio_set_a2dp_freq(freq);
    }
}

void a2dp_audio_set_freq_user_case(A2DP_AUDIO_FREQ_USER_CASE_INDEX_E userCase)
{
    if (a2dp_audio_current_freq_user_case != userCase)
    {
        a2dp_audio_current_freq_user_case = userCase;
        a2dp_audio_sysfreq_refesh();

#ifdef A2DP_DECODER_CROSS_CORE
        if (a2dp_audio_current_freq_user_case == A2DP_AUDIO_BOOST_MODE_FREQ) {
            a2dp_decoder_bth_a2dp_boost_freq_req();
        }
#endif
    }
}

void a2dp_audio_set_freq_rssi_case(A2DP_AUDIO_DYNAMIC_FREQ_RSSI_CASE_INDEX_E rssiCase)
{
    if (a2dp_audio_current_freq_rssi_case != rssiCase)
    {
        //TRACE(0, "switch to rssi freq case %d", rssiCase);
        a2dp_audio_current_freq_rssi_case = rssiCase;
        a2dp_audio_accumulated_continuous_high_rssi_count = 0;
        a2dp_audio_sysfreq_refesh();

#ifdef A2DP_DECODER_CROSS_CORE
        if (a2dp_audio_current_freq_rssi_case == A2DP_AUDIO_DYNAMIC_FREQ_LOW_RSSI_CASE) {
            a2dp_decoder_bth_a2dp_boost_freq_req();
        }
#endif

    }
}

int a2dp_audio_sysfreq_boost_init(uint32_t normalfreq)
{
    a2dp_audio_sysfreq_cnt = UINT32_MAX;
    a2dp_audio_sysfreq_dest_boost_cnt = 0;
    a2dp_audio_sysfreq_normalfreq = (APP_SYSFREQ_FREQ_T)normalfreq;
    TRACE_A2DP_DECODER_I("[BOOST] freq:%d", normalfreq);
    a2dp_audio_sysfreq_reset();
    return 0;
}

void a2dp_audio_sysfreq_update_normalfreq(void)
{
    if (A2DP_AUDIO_BOOST_MODE_FREQ != a2dp_audio_current_freq_user_case)
    {
        if (!a2dp_audio_sysfreq_boost_running())
        {
            if (1 == app_bt_audio_count_straming_mobile_links())
            {
                a2dp_audio_set_freq_user_case(A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_ONE_MOBILE_LINK);
            }
            else if (app_bt_audio_count_straming_mobile_links() > 1)
            {
                a2dp_audio_set_freq_user_case(A2DP_AUDIO_MINIMUM_NORMAL_FREQ_WITH_TWO_MOBILE_LINK);
            }
        }
        else
        {
            a2dp_audio_set_freq_user_case(A2DP_AUDIO_BOOST_MODE_FREQ);
        }
    }
}

void a2dp_audio_sysfreq_reset(void)
{
    a2dp_audio_set_freq_user_case(A2DP_AUDIO_FREQ_USER_CASE_INVALID);
    a2dp_audio_set_freq_rssi_case(A2DP_AUDIO_DYNAMIC_FREQ_HIGH_RSSI_CASE);
    a2dp_audio_sysfreq_update_normalfreq();
}

int a2dp_audio_sysfreq_boost_init_start(uint32_t boost_cnt)
{
    enum APP_SYSFREQ_FREQ_T sysfreq = APP_SYSFREQ_104M;

    a2dp_audio_sysfreq_cnt = 0;
    a2dp_audio_sysfreq_dest_boost_cnt = boost_cnt;
    TRACE_A2DP_DECODER_I("[BOOST] init freq:%d cnt:%d", sysfreq, boost_cnt);
    app_audio_set_a2dp_freq(sysfreq);
    return 0;
}

int a2dp_audio_sysfreq_boost_start(uint32_t boost_cnt,bool is_init_boost)
{
    a2dp_audio_sysfreq_cnt = 0;
    a2dp_audio_sysfreq_dest_boost_cnt = boost_cnt;

    a2dp_audio_set_freq_user_case(A2DP_AUDIO_BOOST_MODE_FREQ);
    return 0;
}

static int a2dp_audio_sysfreq_boost_porc(void)
{
    if (a2dp_audio_sysfreq_cnt == UINT32_MAX){
        //do nothing
    }else if (a2dp_audio_sysfreq_cnt >= a2dp_audio_sysfreq_dest_boost_cnt){
            a2dp_audio_sysfreq_cnt = UINT32_MAX;
            a2dp_audio_set_freq_user_case(A2DP_AUDIO_FREQ_USER_CASE_INVALID);
            a2dp_audio_sysfreq_update_normalfreq();
    }else{
        a2dp_audio_sysfreq_cnt++;
    }
    return 0;
}

int a2dp_audio_sysfreq_boost_running(void)
{
    return a2dp_audio_sysfreq_cnt == UINT32_MAX ? 0 : 1;
}

int a2dp_audio_store_packet_checker_start(void)
{
    store_packet_history_loctime = 0;
    return 0;
}

int a2dp_audio_store_packet_checker(btif_media_header_t *header)
{
    bool show_info = false;
    uint32_t fast_sys_tick = hal_fast_sys_timer_get();
    int32_t tmp_ms = 0;

    if (store_packet_history_loctime){
        tmp_ms = fast_sys_tick - store_packet_history_loctime;
        if (tmp_ms > (int32_t)MS_TO_FAST_TICKS(50)){
            show_info = true;
        }
    }

    if (show_info){
        //TRACE_A2DP_DECODER_I("[INPUT] >50ms seq:%d diff:%d", header->sequenceNumber, FAST_TICKS_TO_MS(tmp_ms));
        a2dp_decoder_info_checker();
    }

    store_packet_history_loctime = fast_sys_tick;

    if (a2dp_audio_context.historySeq_idx &&
        ((a2dp_audio_context.historySeq_idx-1)%A2DP_DECODER_HISTORY_SEQ_SAVE == 0)){
        a2dp_audio_show_history_seq();
    }

    return 0;
}
#ifdef A2DP_DECODER_CROSS_CORE
void a2dp_audio_store_fake_packet(uint16_t sequencenumber)
{
    uint8_t device_id = a2dp_audio_packet_last_cache.device_id;
    btif_media_header_t *header = &a2dp_audio_packet_last_cache.header;
    header->sequenceNumber = sequencenumber;
    header->timestamp = UINT32_MAX;
    unsigned char *buf = a2dp_audio_packet_last_cache.data;
    unsigned int len = a2dp_audio_packet_last_cache.len;
    TRACE(0, "fill fake frame seq %d",header->sequenceNumber);
    a2dp_audio_store_packet(device_id,header,buf,len);
}
#endif
//#define DECODER_ERR_TEST
int a2dp_audio_store_packet(uint8_t device_id, btif_media_header_t * header, unsigned char *buf, unsigned int len)
{
    int nRet = A2DP_DECODER_NO_ERROR;
#ifdef DECODER_ERR_TEST
    static uint16_t cnt = 0;
    static uint16_t limit = 500;
    cnt++;
    if(cnt%limit==0){
        cnt = 0;
        limit = rand()%500;
        uint16_t len2 = rand()%len;
        memset(buf+len-len2,0,len2);
        TRACE_A2DP_DECODER_I("[INPUT] Fill Err!!!! seq:%d", header->sequenceNumber);
    }
#endif

#if A2DP_DECODER_HISTORY_SEQ_SAVE
    uint8_t ret = a2dp_audio_save_history_seq(header, buf, len);
    if (ret == A2DP_DECODER_HISTORY_SAME_SEQ)
    {
        return nRet;
    }
#endif
    a2dp_audio_set_store_packet_status(A2DP_AUDIO_DECODER_STORE_PACKET_STATUS_BUSY);
    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_throughput_checker_handler(len);
        a2dp_audio_store_packet_checker(header);
        if (a2dp_audio_context.need_detect_first_packet){
            a2dp_audio_context.need_detect_first_packet = false;
            a2dp_audio_context.audio_decoder.audio_decoder_preparse_packet(header, buf, len);
        }

        nRet = a2dp_audio_context.audio_decoder.audio_decoder_store_packet(header, buf, len);
#ifdef A2DP_DECODER_CROSS_CORE
        int lock = int_lock();
        a2dp_audio_packet_last_cache.device_id = device_id;
        memcpy(&a2dp_audio_packet_last_cache.header, header, sizeof(btif_media_header_t));
        memcpy(a2dp_audio_packet_last_cache.data, buf, len);
        a2dp_audio_packet_last_cache.len = len;
        int_unlock(lock);
#endif
        if (a2dp_audio_detect_next_packet_callback){
            a2dp_audio_detect_next_packet_callback(device_id, header, buf, len);
        }

        A2DP_AUDIO_HEADFRAME_INFO_T headframe_info;
        a2dp_audio_decoder_headframe_info_get(&headframe_info);

#if defined(IBRT)
        if (A2DP_DECODER_NON_CONSECTIVE_SEQ == nRet)
        {
            app_ibrt_if_force_audio_retrigger(RETRIGGER_BY_NON_CONSECTIVE_SEQ);
        }

        if (is_cp_need_reset()){
            TRACE_A2DP_DECODER_I("[CP] find cp error need restart");
            app_ibrt_if_force_audio_retrigger(RETRIGGER_BY_DECODE_ERROR);

        }
        if (nRet == A2DP_DECODER_MTU_LIMTER_ERROR){
            POSSIBLY_UNUSED struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
            if (APP_IBRT_MOBILE_LINK_CONNECTED(&curr_device->remote)){
                // try again
                //a2dp_audio_semaphore_wait(A2DP_AUDIO_WAIT_TIMEOUT_MS);
                nRet = a2dp_audio_context.audio_decoder.audio_decoder_store_packet(header, buf, len);
            }
            if (nRet == A2DP_DECODER_MTU_LIMTER_ERROR){
                if (headframe_info.sequenceNumber > (a2dp_audio_lastframe_info.sequenceNumber+1))
                {
                    app_ibrt_if_force_audio_retrigger(RETRIGGER_BY_DECODE_STATUS_ERROR);
                }
                int dest_discards_samples = 0;
                ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
                if(p_ibrt_ctrl == NULL)
                {
                    return A2DP_DECODER_NOT_SUPPORT;
                }
                if (APP_IBRT_MOBILE_LINK_CONNECTED(&curr_device->remote)){
                    bt_syn_trig_checker(APP_IBRT_UI_GET_MOBILE_CONNHANDLE(&curr_device->remote));
                }else if (APP_IBRT_SLAVE_IBRT_LINK_CONNECTED(&curr_device->remote)){
                    bt_syn_trig_checker(APP_IBRT_UI_GET_IBRT_HANDLE(&curr_device->remote));
                }
                dest_discards_samples = app_bt_stream_get_dma_buffer_samples()/2;
                a2dp_audio_discards_samples(dest_discards_samples*2);
                a2dp_audio_context.audio_decoder.audio_decoder_store_packet(header, buf, len);
                TRACE_A2DP_DECODER_I("[INPUT] MTU_LIMTER so discards_packet");
            }
        }
#else
        if (A2DP_DECODER_NON_CONSECTIVE_SEQ == nRet)
        {
            TRACE_A2DP_DECODER_I("Find non-consective frame sequence number, do retrigger!");
            app_audio_decode_err_force_trigger();
        }

        if (is_cp_need_reset()){
            TRACE_A2DP_DECODER_I("[CP] find cp error need restart");
            app_audio_decode_err_force_trigger();
        }

        if (nRet == A2DP_DECODER_MTU_LIMTER_ERROR){
            a2dp_audio_synchronize_dest_packet_mut(a2dp_audio_context.dest_packet_mut);
            a2dp_audio_context.audio_decoder.audio_decoder_store_packet(header, buf, len);
            TRACE_A2DP_DECODER_W("[INPUT] MTU_LIMTER so discards_packet");
        }
#endif
    }else{
        //TRACE(2,"[INPUT] skip packet status:%d", a2dp_audio_get_status());
    }

    if (a2dp_audio_store_packet_callback){
        a2dp_audio_store_packet_callback(device_id, header, buf, len);
    }

    a2dp_audio_set_store_packet_status(A2DP_AUDIO_DECODER_STORE_PACKET_STATUS_IDLE);
    return 0;
}

void increase_dst_mtu(void)
{
    int32_t currentTargetBufLen = sbm_get_current_target_jitter_buf_length();
    currentTargetBufLen += 30;
    sbm_update_target_jitter_buf_length(currentTargetBufLen);
    TRACE(0, "increase dst to %d", a2dp_audio_context.dest_packet_mut);
}

void decrease_dst_mtu(void)
{
    int32_t currentTargetBufLen = sbm_get_current_target_jitter_buf_length();

    if (currentTargetBufLen >= 60)
    {
        currentTargetBufLen -= 30;
        sbm_update_target_jitter_buf_length(currentTargetBufLen);
    }
    TRACE(0, "decrease dst to %d", a2dp_audio_context.dest_packet_mut);
}

void change_dst_mtu(const char* cmd, uint32_t cmd_len)
{
    do {
        int targetms = 0;
        sscanf(cmd, "%d", &targetms);

        TRACE(0, "[%s] targetms %d", __func__, targetms);

        sbm_update_target_jitter_buf_length(targetms);
    } while(0);
}

uint32_t a2dp_audio_playback_handler(uint8_t device_id, uint8_t *buffer, uint32_t buffer_bytes)
{
    uint32_t len = buffer_bytes;
    int nRet = A2DP_DECODER_NO_ERROR;
    A2DP_AUDIO_LASTFRAME_INFO_T *lastframe_info = NULL;
    list_t *list = a2dp_audio_context.audio_datapath.input_raw_packet_list;
    enum A2DP_AUDIO_DECODER_STATUS status;

    a2dp_audio_set_playback_status(A2DP_AUDIO_DECODER_PLAYBACK_STATUS_BUSY);
    status = a2dp_audio_get_status();
    if (status != A2DP_AUDIO_DECODER_STATUS_START){
        TRACE_A2DP_DECODER_W("[PLAYBACK] skip handler status:%d", status);
        goto exit;
    }

    a2dp_audio_sysfreq_boost_porc();
    if (a2dp_audio_context.average_packet_mut == 0){
    }else{
        if (!a2dp_audio_refill_packet()){
            uint16_t packet_mut = 0;
            if (!a2dp_audio_internal_lastframe_info_ptr_get(&lastframe_info)){
                packet_mut = a2dp_audio_list_length(list);
                a2dp_audio_context.average_packet_mut = a2dp_audio_alpha_filter((float)a2dp_audio_context.average_packet_mut, (float)packet_mut);
                //TRACE(0, "average_packet_mut %d dest mtu %d", (uint8_t)a2dp_audio_context.average_packet_mut, a2dp_audio_context.dest_packet_mut);
#ifdef ADVANCE_FILL_ENABLED
                if (!is_doing_pre_fill)
#endif
                {
                    bool isProcessedByCodecAlg = false;
                    if (a2dp_audio_context.audio_decoder.audio_decoder_custom_latency_tuning)
                    {
                        if (a2dp_audio_context.max_seq_num == UINT16_MAX)
                        {
                            isProcessedByCodecAlg = a2dp_audio_context.audio_decoder.audio_decoder_custom_latency_tuning(device_id,
                                a2dp_audio_context.average_packet_mut, a2dp_audio_context.dest_packet_mut);
                        }
                    }

                    // sbm is not processed by codec algorithm, shall be done by SDK
                    if (!isProcessedByCodecAlg)
                    {
                        a2dp_audio_status_mutex_lock();
                        a2dp_audio_sync_handler(device_id, buffer, buffer_bytes);
                        a2dp_audio_status_mutex_unlock();
                    }
                }
            }
        }
#if defined(HIGH_EFFICIENCY_TX_PWR_CTRL)||defined(IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING)
        app_audio_rssi_based_env_ajdustment_handler();
#endif
    }
#if defined(A2DP_AUDIO_REFILL_AFTER_NO_CACHE)
    if (a2dp_audio_context.skip_frame_cnt_after_no_cache){
#if defined(A2DP_CP_ACCEL)
        uint32_t cp_delay_mtus = get_in_cp_frame_delay();
        uint32_t list_len = a2dp_audio_list_length(list);

        cp_delay_mtus  *= get_cp_frame_mtus(&a2dp_audio_lastframe_info);
        if (list_len >= (a2dp_audio_context.dest_packet_mut - cp_delay_mtus)){
            a2dp_audio_context.skip_frame_cnt_after_no_cache = 0;
            a2dp_audio_context.underflow_inprocess = false;
        }
#else  // A2DP_CP_ACCEL
        A2DP_AUDIO_HEADFRAME_INFO_T headframe_info;
        uint32_t list_len = 0;
        a2dp_audio_decoder_headframe_info_get(&headframe_info);
        A2DP_AUDIO_SYNCFRAME_INFO_T sync_info;
        sync_info.sequenceNumber = a2dp_audio_context.mute_frame_seq + a2dp_audio_context.skip_mute_cnt;
        if (a2dp_audio_context.max_seq_num == UINT8_MAX)
        {
            sync_info.sequenceNumber &= 0x00FF;
        }
        uint8_t recover_list_len = a2dp_audio_context.dest_packet_mut;

        TRACE_A2DP_DECODER_I("exp_seq %d skip_cnt:%d rec_len:%d, dstMTU:%d head %d",
            sync_info.sequenceNumber, a2dp_audio_context.skip_mute_cnt,
            recover_list_len, a2dp_audio_context.dest_packet_mut, headframe_info.sequenceNumber);

        if (1 != a2dp_audio_context.skip_frame_cnt_after_no_cache)
        {
            sync_info.sequenceNumber++; // The currently expected packet won't be playback
            if (a2dp_audio_context.max_seq_num == UINT8_MAX)
            {
                sync_info.sequenceNumber &= 0x00FF;
            }
            a2dp_audio_synchronize_packet(&sync_info, A2DP_AUDIO_SYNCFRAME_MASK_SEQ);
        }

        list_len = a2dp_audio_list_length(list);
        TRACE_A2DP_DECODER_I("[PLAYBACK] decode refill skip_cnt:%d, list:%d",
                a2dp_audio_context.skip_frame_cnt_after_no_cache, list_len);

        if (list_len >= recover_list_len){
            // When the length in the audio list matches the recovery length,
            // start normal playback
#ifdef A2DP_DECODER_CROSS_CORE
            A2DP_AUDIO_STARTFRAME_INFO_T start_frame_info;
            start_frame_info.sequenceNumber = sync_info.sequenceNumber;
            start_frame_info.curSubSequenceNumber = 0;
            if (1 == a2dp_audio_context.skip_frame_cnt_after_no_cache)
            {
                start_frame_info.is_ignore = true;
            }
            else
            {
                start_frame_info.is_ignore = false;
            }
            a2dp_decoder_bth_set_first_run(start_frame_info);
            a2dp_audio_internal_lastframe_info_ptr_get(&lastframe_info);
            if (start_frame_info.is_ignore)
            {
                lastframe_info->sequenceNumber = headframe_info.sequenceNumber-1;
            }
            else
            {
                lastframe_info->sequenceNumber = sync_info.sequenceNumber-1;
            }
#endif
            a2dp_audio_context.skip_frame_cnt_after_no_cache = 0;
            a2dp_audio_context.skip_mute_cnt = 0;
#ifdef IBRT
            TRACE_A2DP_DECODER_I("start playback seq;%d, analysisTick:%d",
                sync_info.sequenceNumber, app_tws_ibrt_audio_analysis_tick_get());
#endif
            app_start_post_chopping_timer();
        }
#endif // A2DP_CP_ACCEL
        memset(buffer, 0, buffer_bytes);
        a2dp_decoder_info_checker();
        if (a2dp_audio_context.skip_frame_cnt_after_no_cache > 1){
            a2dp_audio_context.skip_frame_cnt_after_no_cache--;
            a2dp_audio_context.skip_mute_cnt++;
        } else {
            a2dp_audio_context.mute_frame_cnt_after_no_cache = A2DP_AUDIO_MUTE_FRAME_CNT_AFTER_NO_CACHE;
#if defined(BT_DONT_PLAY_MUTE_WHEN_A2DP_STUCK_PATCH)
            a2dp_audio_sysfreq_boost_init_start(A2DP_AUDIO_SYSFREQ_BOOST_RESUME_CNT);
#endif
        }
    }else
#endif
    {
#ifdef ADVANCE_FILL_ENABLED
        if (!is_doing_pre_fill)
#endif
        {
            if (a2dp_audio_context.average_packet_mut == 0){
                A2DP_AUDIO_HEADFRAME_INFO_T headframe_info;
                a2dp_audio_decoder_headframe_info_get(&headframe_info);
                // currently filled pcm data will be played in one chunk later,
                // so add 1 frame owned mtu count here
                // also on streaming started, cp delay needs to be taken into account
                a2dp_audio_context.average_packet_mut =
                    a2dp_audio_list_length(list) +  get_in_cp_frame_cnt();
                TRACE_A2DP_DECODER_I("[PLAYBACK] main mcu list cnt %d; dma playback delay %d",
                    a2dp_audio_list_length(list), a2dp_audio_context.output_cfg.mtu_per_frame);
                TRACE_A2DP_DECODER_I("[PLAYBACK] init average_packet_mut:%d seq:%d",
                    (uint16_t)(a2dp_audio_context.average_packet_mut+0.5f), headframe_info.sequenceNumber);
                POSSIBLY_UNUSED struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
                if (curr_device->rsv_avdtp_start_signal)
                {
                    sbm_target_jitter_buffer_initiative_approached();
                    curr_device->rsv_avdtp_start_signal = false;
                }
                sbm_set_manual_adjustment_on_going_flag();
            }
        }
        if (a2dp_audio_context.output_cfg.bits_depth == 24 &&
            a2dp_audio_context.audio_decoder.stream_info.bits_depth == 16){

#if !defined(PLAYBACK_FORCE_48K)
            len = len / (sizeof(int32_t) / sizeof(int16_t));
#endif

#ifdef A2DP_DECODER_CROSS_CORE
            nRet = a2dp_decoder_bth_fetch_pcm_data(
                a2dp_audio_lastframe_info.sequenceNumber,
                a2dp_audio_lastframe_info.totalSubSequenceNumber,
                a2dp_audio_lastframe_info.curSubSequenceNumber,
                buffer, len);
#else
            nRet = a2dp_audio_context.audio_decoder.audio_decoder_decode_frame(buffer, len);
            if  (nRet < 0 || a2dp_audio_context.mute_frame_cnt_after_no_cache){
                TRACE_A2DP_DECODER_I("[PLAYBACK] decode failed nRet=%d mute_cnt:%d", nRet, a2dp_audio_context.mute_frame_cnt_after_no_cache);
                a2dp_decoder_info_checker();
                if (nRet == -5){
                    struct BT_DEVICE_T *curr_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
                    if (curr_device){
                        bt_drv_rx_status_dump(curr_device->acl_conn_hdl);
                    }
                }
                //mute frame
                memset(buffer, 0, len);
            }
#endif

#if !defined(PLAYBACK_FORCE_48K)
            a2dp_audio_convert_16bit_to_24bit((int32_t *)buffer, (int16_t *)buffer, len / sizeof(int16_t));

            // Restore len to 24-bit sample buffer length
            len = len * (sizeof(int32_t) / sizeof(int16_t));
#endif

        }else if (a2dp_audio_context.output_cfg.bits_depth ==
                  a2dp_audio_context.audio_decoder.stream_info.bits_depth) {
#ifdef A2DP_DECODER_CROSS_CORE
            nRet = a2dp_decoder_bth_fetch_pcm_data(
                a2dp_audio_lastframe_info.sequenceNumber,
                a2dp_audio_lastframe_info.totalSubSequenceNumber,
                a2dp_audio_lastframe_info.curSubSequenceNumber,
                buffer, len);

#else
            nRet = a2dp_audio_context.audio_decoder.audio_decoder_decode_frame(buffer, len);
            if  (nRet < 0 || a2dp_audio_context.mute_frame_cnt_after_no_cache){
                //mute frame
                TRACE_A2DP_DECODER_I("[PLAYBACK] decode failed nRet=%d mute_cnt:%d", nRet, a2dp_audio_context.mute_frame_cnt_after_no_cache);
                a2dp_decoder_info_checker();
                if (nRet == -5){
                    struct BT_DEVICE_T *curr_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
                    if (curr_device){
                        bt_drv_rx_status_dump(curr_device->acl_conn_hdl);
                    }
                }
                memset(buffer, 0, len);
            }
#endif

        }else{
            ASSERT_A2DP_DECODER(0, "[PLAYBACK] output bit_depth(%d) not matched with stream bit_depth(%d)",
                a2dp_audio_context.output_cfg.bits_depth,
                a2dp_audio_context.audio_decoder.stream_info.bits_depth);
        }
#ifdef PLAYBACK_FORCE_48K
        ASSERT(a2dp_audio_context.audio_decoder.stream_info.bits_depth == 16, "[%s] bits != 16", __func__);
        a2dp_audio_channel_select<int16_t>(a2dp_audio_context.chnl_sel, (int16_t *)buffer, buffer_bytes / sizeof(int16_t));
#else
        if (a2dp_audio_context.output_cfg.bits_depth == 24) {
            a2dp_audio_channel_select<int32_t>(a2dp_audio_context.chnl_sel, (int32_t *)buffer, buffer_bytes / sizeof(int32_t));
        } else {
            a2dp_audio_channel_select<int16_t>(a2dp_audio_context.chnl_sel, (int16_t *)buffer, buffer_bytes / sizeof(int16_t));
        }
#endif
/*
        uint8_t codec_type = BT_A2DP_CODEC_TYPE_SBC;
        uint8_t nonType = 0;
        if(app_stream_get_a2dp_param_callback)
        {
            uint8_t device_id = app_bt_audio_get_curr_a2dp_device();
            codec_type = app_stream_get_a2dp_param_callback(device_id);
        }
        else
        {
            codec_type = bt_sbc_player_get_codec_type();
        }

        nonType = bes_aud_bt->a2dp_lhdc_get_non_type_by_device_id(device_id);

        if( (nonType != A2DP_NON_CODEC_TYPE_LHDCV5) || (codec_type != BT_A2DP_CODEC_TYPE_NON_A2DP))
        {
            if (a2dp_audio_context.output_cfg.bits_depth == 24) {
                a2dp_audio_channel_select(a2dp_audio_context.chnl_sel, (int32_t *)buffer, buffer_bytes / sizeof(int32_t));
            } else {
                a2dp_audio_channel_select(a2dp_audio_context.chnl_sel, (int16_t *)buffer, buffer_bytes / sizeof(int16_t));
            }
       }
       */
    }

    if ((nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR) || (nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR_SYS_BUSY))
    {
        if (a2dp_audio_internal_lastframe_info_ptr_get(&lastframe_info) < 0)
        {
            goto exit;
        }
        TRACE(2,"CACHE_UNDERFLOW lastseq:%d ftick:%d", lastframe_info->sequenceNumber, hal_fast_sys_timer_get());
        app_bt_print_buff_status();
        a2dp_audio_show_history_seq();
        uint32_t mute_frames = A2DP_AUDIO_MUTE_FRAME_CNT_AFTER_NO_CACHE;
        uint32_t skip_frames =  A2DP_AUDIO_SKIP_FRAME_LIMIT_AFTER_NO_CACHE - get_in_cp_frame_delay();
        a2dp_audio_context.mute_frame_cnt_after_no_cache = (uint32_t)((float)mute_frames *
                                                                        a2dp_audio_latency_factor_get());
        a2dp_audio_context.skip_frame_cnt_after_no_cache = (uint32_t)((float)skip_frames *
                                                                        a2dp_audio_latency_factor_get());
        a2dp_audio_context.skip_mute_cnt = 1;
        a2dp_audio_context.average_packet_mut = 0;
        a2dp_audio_context.mute_frame_seq = lastframe_info->sequenceNumber + 1;
        lastframe_info->sequenceNumber = UINT16_MAX;
        a2dp_audio_sync_reset_data();
#if defined(IBRT)

        a2dp_audio_sync_tune_sample_rate(app_tws_ibrt_audio_sync_config_factor_reference_get());
#else
        a2dp_audio_sync_tune_sample_rate(a2dp_audio_context.init_factor_reference);
#endif
        a2dp_audio_sync_tune_cancel();
        a2dp_decoder_info_checker();
    }else{
        if (a2dp_audio_context.mute_frame_cnt_after_no_cache > 0){
            a2dp_audio_context.mute_frame_cnt_after_no_cache--;
            a2dp_audio_context.average_packet_mut = 0;
            if (a2dp_audio_context.mute_frame_cnt_after_no_cache >= 1){
                a2dp_audio_synchronize_dest_packet_mut(a2dp_audio_context.dest_packet_mut);
            }
        }
    }
    if (!a2dp_audio_internal_lastframe_info_ptr_get(&lastframe_info)){
        lastframe_info->stream_info.factor_reference = a2dp_audio_context.output_cfg.factor_reference;
        lastframe_info->average_frames = (uint32_t)(a2dp_audio_context.average_packet_mut + 0.5f);
    }
exit:
    a2dp_audio_set_playback_status(A2DP_AUDIO_DECODER_PLAYBACK_STATUS_IDLE);
    if (a2dp_audio_context.preparse_stop){
        nRet = A2DP_DECODER_NO_ERROR;
        A2DP_AUDIO_HEADFRAME_INFO_T headframe_info;
        uint32_t list_size = 0;
        a2dp_audio_decoder_headframe_info_get(&headframe_info);
        list_size = a2dp_audio_list_length(list);
        TRACE_A2DP_DECODER_I("[PREPARSE_STOP][PLAYBACK]list:%d seq:%d", list_size, headframe_info.sequenceNumber);
    }
#if defined(IBRT)
    POSSIBLY_UNUSED struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    if ((nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR) || (nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR_SYS_BUSY)){
#if defined(A2DP_AUDIO_UNDERFLOW_CAUSE_AUDIO_RETRIGGER)
        bool force_audio_retrigger = true;
#else
        bool force_audio_retrigger = false;
#endif
        if (a2dp_audio_latency_factor_get() == A2DP_AUDIO_LATENCY_LOW_FACTOR &&
            APP_IBRT_MOBILE_LINK_CONNECTED(&curr_device->remote)){
            a2dp_audio_latency_factor_sethigh();
            if (app_tws_ibrt_tws_link_connected() &&
                APP_IBRT_IS_PROFILE_EXCHANGED(&curr_device->remote)){
                float latency_factor = a2dp_audio_latency_factor_get();
                tws_ctrl_send_cmd(APP_TWS_CMD_SET_LATENCYFACTOR, (uint8_t*)&latency_factor, sizeof(latency_factor));
                force_audio_retrigger = true;
            }
        }
        if (force_audio_retrigger && !a2dp_audio_context.underflow_inprocess){
            a2dp_audio_context.underflow_inprocess = true;
            a2dp_audio_context.force_audio_retrigger_cnt++;
#if defined(BT_DONT_PLAY_MUTE_WHEN_A2DP_STUCK_PATCH)
            uint32_t passedTimerMs;
            uint32_t currentTimerMs = GET_CURRENT_MS();
            if (0 == start_force_retrigger_ms)
            {
               start_force_retrigger_ms = currentTimerMs;
               passedTimerMs = 0;
            }
            else
            {
               if (currentTimerMs >= start_force_retrigger_ms)
               {
                   passedTimerMs = currentTimerMs - start_force_retrigger_ms;
               }
               else
               {
                   passedTimerMs = TICKS_TO_MS(0xFFFFFFFF) - start_force_retrigger_ms + currentTimerMs + 1;
               }
            }
            if (passedTimerMs > A2DP_ADUIO_FORCE_RETRIGGER_TIMEOUT)
            {
               a2dp_audio_context.force_audio_retrigger_cnt = 0;
               start_force_retrigger_ms = currentTimerMs;
            }

            if (a2dp_audio_context.force_audio_retrigger_cnt>A2DP_ADUIO_FORCE_RETRIGGER_MAX_CNT)
            {
                app_ibrt_if_force_audio_retrigger((nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR) ? RETRIGGER_BY_LOW_BUFFER : RETRIGGER_BY_SYS_BUSY);
#if defined(__AUDIO_RETRIGGER_REPORT__)
                app_ibrt_if_report_audio_retrigger((nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR) ? RETRIGGER_BY_LOW_BUFFER : RETRIGGER_BY_SYS_BUSY);
#endif
            }
#else
            app_ibrt_if_force_audio_retrigger((nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR) ? RETRIGGER_BY_LOW_BUFFER : RETRIGGER_BY_SYS_BUSY);
#endif
        }
    }
#else
    if ((nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR) || (nRet == A2DP_DECODER_CACHE_UNDERFLOW_ERROR_SYS_BUSY)){
        app_audio_decode_err_force_trigger();
    }
#endif
    return len;
}

static void a2dp_audio_packet_free(void *packet)
{
    if (a2dp_audio_context.audio_decoder.audio_decoder_packet_free){
        a2dp_audio_context.audio_decoder.audio_decoder_packet_free(packet);
    }else{
        a2dp_audio_heap_free(packet);
    }
}

void a2dp_audio_clear_input_raw_packet_list(void)
{
    //just clean the packet list to start receive ai data again
    if(a2dp_audio_context.audio_datapath.input_raw_packet_list)
        a2dp_audio_list_clear(a2dp_audio_context.audio_datapath.input_raw_packet_list);
}

#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_CP__) || defined(__VIRTUAL_SURROUND_HWFIR__) || defined(__VIRTUAL_SURROUND_STEREO__)
int stereo_surround_freq_controller(uint32_t freq){
    a2dp_audio_sysfreq_boost_init(freq);
    a2dp_audio_sysfreq_boost_init_start(A2DP_AUDIO_SYSFREQ_BOOST_RESUME_CNT);
    return 0;
}
#endif

int a2dp_audio_init(uint32_t sysfreq, A2DP_AUDIO_CODEC_TYPE codec_type, A2DP_AUDIO_OUTPUT_CONFIG_T *config, A2DP_AUDIO_CHANNEL_SELECT_E chnl_sel, uint16_t dest_packet_mut, uint16_t chopping_dest_mut)
{
#ifdef A2DP_DECODER_CROSS_CORE
#ifdef A2DP_DECODER_CROSS_CORE_USE_M55
    if (!a2dp_audio_retrigger_is_on_process()) {
        a2dp_decoder_cc_sema_init();
        app_dsp_m55_init(APP_DSP_M55_USER_A2DP);
    } else {
        app_tws_ibrt_request_modify_tws_bandwidth(TWS_TIMING_CONTROL_USER_A2DP, false);
        app_tws_ibrt_request_modify_tws_bandwidth(TWS_TIMING_CONTROL_USER_FAST_COMMUNICATION, true);
    }
#endif
#endif
    uint8_t *heap_buff = NULL;
    uint32_t heap_size = 0;
    double ratio = 0;
#ifdef ADVANCE_FILL_ENABLED
    is_doing_pre_fill = false;
#endif
    A2DP_AUDIO_OUTPUT_CONFIG_T decoder_output_config;
    TRACE_A2DP_DECODER_I("[INIT] freq:%d codec:%d chnl:%d", sysfreq, codec_type, chnl_sel);
    TRACE_A2DP_DECODER_I("[INIT] out:%d-%d-%d smp:%d dest:%d dest2:%d",
                                                               config->sample_rate,
                                                               config->num_channels,
                                                               config->bits_depth,
                                                               config->frame_samples,
                                                               dest_packet_mut,
                                                               chopping_dest_mut);
    a2dp_audio_sysfreq_boost_init(sysfreq);
    a2dp_audio_sysfreq_boost_init_start(A2DP_AUDIO_SYSFREQ_BOOST_RESUME_CNT);
    a2dp_audio_buffer_mutex_init();
    a2dp_audio_status_mutex_init();

#if defined(HIGH_EFFICIENCY_TX_PWR_CTRL)||defined(IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING)
    app_audio_reset_rssi_based_env_ajustment(true);
#endif

    a2dp_audio_status_mutex_lock();

    a2dp_audio_detect_next_packet_callback_register(NULL);
    a2dp_audio_detect_store_packet_callback_register(NULL);

    heap_size = A2DP_AUDIO_MEMPOOL_SIZE;

    app_audio_mempool_get_buff(&heap_buff, heap_size);
    ASSERT_A2DP_DECODER(heap_buff, "%s size:%d", __func__, heap_size);
    a2dp_audio_heap_init(heap_buff, heap_size);

#ifdef A2DP_CP_ACCEL
    a2dp_cp_heap_init();
#endif

    if ((codec_type==A2DP_AUDIO_CODEC_TYPE_SCALABL)&&(config->sample_rate==96000))
    {
        a2dp_audio_context.max_seq_num = UINT8_MAX;
    }
    else
    {
        a2dp_audio_context.max_seq_num = UINT16_MAX;
    }

#ifdef A2DP_DECODER_CROSS_CORE
    a2dp_decoder_bth_start_stream((A2DP_DECODER_CODEC_TYPE_E)codec_type,
        (A2DP_DECODER_CHANNEL_SELECT_E)chnl_sel, (A2DP_AUDIO_CC_OUTPUT_CONFIG_T *)config, a2dp_audio_context.max_seq_num);
#endif

    memset(&a2dp_audio_lastframe_info, 0, sizeof(A2DP_AUDIO_LASTFRAME_INFO_T));

    a2dp_audio_context.audio_datapath.input_raw_packet_list = a2dp_audio_list_new(a2dp_audio_packet_free,
                                                       (list_mempool_zmalloc)a2dp_audio_heap_cmalloc,
                                                       (list_mempool_free)a2dp_audio_heap_free);

    a2dp_audio_context.audio_datapath.output_pcm_packet_list = a2dp_audio_list_new(a2dp_audio_packet_free,
                                                       (list_mempool_zmalloc)a2dp_audio_heap_cmalloc,
                                                       (list_mempool_free)a2dp_audio_heap_free);

    memcpy(&(a2dp_audio_context.output_cfg), config, sizeof(A2DP_AUDIO_OUTPUT_CONFIG_T));
    ratio = a2dp_audio_context.output_cfg.factor_reference;
    a2dp_audio_context.output_cfg.factor_reference = 0;

    a2dp_audio_context.init_factor_reference = config->factor_reference;
#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
    a2dp_audio_context.chnl_sel = a2dp_audio_context.chnl_sel_new;
    a2dp_audio_stereo_mix_init(config->frame_samples);
#else
    a2dp_audio_context.chnl_sel = chnl_sel;
#endif
    a2dp_audio_context.audio_decoder_status = A2DP_AUDIO_DECODER_STATUS_READY;
    if (A2DP_INVALID_DELAY_MS == a2dp_audio_context.custom_delay_ms)
    {
        TRACE_A2DP_DECODER_I("[INIT] default mtu %d", dest_packet_mut);
        a2dp_audio_context.dest_packet_mut = dest_packet_mut;
    }
    else
    {
        uint8_t custom_mtu = sbm_convert_targetMs_to_mtu(a2dp_audio_context.custom_delay_ms);
        if (custom_mtu)
        {
            a2dp_audio_context.dest_packet_mut = custom_mtu;
        }
        else
        {
            a2dp_audio_context.dest_packet_mut = dest_packet_mut;
        }
        TRACE_A2DP_DECODER_I("[INIT] custom mtu %d", a2dp_audio_context.dest_packet_mut);
    }

    a2dp_audio_context.not_chopping_dest_mut = a2dp_audio_context.dest_packet_mut;
    if (chopping_dest_mut)
    {
        a2dp_audio_context.dest_packet_mut = chopping_dest_mut;
        TRACE_A2DP_DECODER_I("[INIT] chopping mtu %d", a2dp_audio_context.dest_packet_mut);
    }

    a2dp_audio_context.average_packet_mut = 0;

    switch (codec_type)
    {
        case A2DP_AUDIO_CODEC_TYPE_SBC:
            decoder_output_config.sample_rate = config->sample_rate;
            decoder_output_config.num_channels = 2;
            decoder_output_config.bits_depth = bt_sbc_player_get_sample_bit();
            decoder_output_config.frame_samples = config->frame_samples;
            decoder_output_config.factor_reference = 1.0f;
            memcpy(&(a2dp_audio_context.audio_decoder), &a2dp_audio_sbc_decoder_config, sizeof(A2DP_AUDIO_DECODER_T));
            break;
#if defined(A2DP_AAC_ON)
        case A2DP_AUDIO_CODEC_TYPE_MPEG2_4_AAC:
            decoder_output_config.sample_rate = config->sample_rate;
            decoder_output_config.num_channels = 2;
            decoder_output_config.bits_depth = bt_sbc_player_get_sample_bit();
            decoder_output_config.frame_samples = config->frame_samples;
            decoder_output_config.factor_reference = 1.0f;
            memcpy(&(a2dp_audio_context.audio_decoder), &a2dp_audio_aac_lc_decoder_config, sizeof(A2DP_AUDIO_DECODER_T));
            break;
#endif
#if defined(A2DP_SCALABLE_ON)
        case A2DP_AUDIO_CODEC_TYPE_SCALABL:
            decoder_output_config.sample_rate = config->sample_rate;
            decoder_output_config.num_channels = 2;
            decoder_output_config.bits_depth = bt_sbc_player_get_sample_bit();
            decoder_output_config.frame_samples = config->frame_samples;
            decoder_output_config.factor_reference = 1.0f;
            memcpy(&(a2dp_audio_context.audio_decoder), &a2dp_audio_scalable_decoder_config, sizeof(A2DP_AUDIO_DECODER_T));
       break;
#endif
#if defined(A2DP_LHDC_ON)
        case A2DP_AUDIO_CODEC_TYPE_LHDC:
            decoder_output_config.sample_rate = config->sample_rate;
            decoder_output_config.num_channels = 2;
            decoder_output_config.bits_depth = bt_sbc_player_get_sample_bit();
            decoder_output_config.frame_samples = config->frame_samples;
            decoder_output_config.factor_reference = 1.0f;
            memcpy(&(a2dp_audio_context.audio_decoder), &a2dp_audio_lhdc_decoder_config, sizeof(A2DP_AUDIO_DECODER_T));
            break;
#endif
#if defined(A2DP_LHDCV5_ON)
        case A2DP_AUDIO_CODEC_TYPE_LHDCV5:
            decoder_output_config.sample_rate = config->sample_rate;
            decoder_output_config.num_channels = 2;
            decoder_output_config.bits_depth = bt_sbc_player_get_sample_bit();
            decoder_output_config.frame_samples = config->frame_samples;
            decoder_output_config.factor_reference = 1.0f;
            memcpy(&(a2dp_audio_context.audio_decoder), &a2dp_audio_lhdcv5_decoder_config, sizeof(A2DP_AUDIO_DECODER_T));
            break;
#endif
#if defined(A2DP_LDAC_ON)
        case A2DP_AUDIO_CODEC_TYPE_LDAC:
            decoder_output_config.sample_rate = config->sample_rate;
            decoder_output_config.num_channels = 2;
            decoder_output_config.bits_depth = bt_sbc_player_get_sample_bit();
            decoder_output_config.frame_samples = config->frame_samples;
            decoder_output_config.factor_reference = 1.0f;
            memcpy(&(a2dp_audio_context.audio_decoder), &a2dp_audio_ldac_decoder_config, sizeof(A2DP_AUDIO_DECODER_T));
            break;
#endif
#if defined(A2DP_LC3_ON)
        case A2DP_AUDIO_CODEC_TYPE_LC3:
            decoder_output_config.sample_rate = config->sample_rate;
            decoder_output_config.num_channels = 2;
            decoder_output_config.bits_depth = bt_sbc_player_get_sample_bit();
            decoder_output_config.frame_samples = config->frame_samples;
            decoder_output_config.factor_reference = 1.0f;
            memcpy(&(a2dp_audio_context.audio_decoder), &a2dp_audio_lc3_decoder_config, sizeof(A2DP_AUDIO_DECODER_T));
            break;
#endif
        default:
            ASSERT_A2DP_DECODER(0, "%s invalid codec_type:%d", __func__, codec_type);
            break;
    }

    memcpy(&(a2dp_audio_context.audio_decoder.stream_info), &decoder_output_config, sizeof(A2DP_AUDIO_OUTPUT_CONFIG_T));

    a2dp_audio_context.audio_decoder.audio_decoder_init(&decoder_output_config, (void *)&a2dp_audio_context);
    a2dp_audio_context.need_detect_first_packet = true;
    a2dp_audio_context.underflow_inprocess = false;
    a2dp_audio_context.force_audio_retrigger_cnt = 0;
    start_force_retrigger_ms = 0;
    a2dp_audio_context.preparse_stop = false;
    a2dp_audio_context.skip_frame_cnt_after_no_cache = 0;
    a2dp_audio_context.mute_frame_cnt_after_no_cache = 0;
    a2dp_audio_context.skip_mute_cnt = 0;

    a2dp_audio_context.store_packet_status = A2DP_AUDIO_DECODER_STORE_PACKET_STATUS_IDLE;
    a2dp_audio_context.playback_status = A2DP_AUDIO_DECODER_PLAYBACK_STATUS_IDLE;

    a2dp_audio_sync_init(ratio);

#if A2DP_DECODER_HISTORY_SEQ_SAVE
    a2dp_audio_reset_history_seq();
#endif
    a2dp_audio_store_packet_checker_start();

    a2dp_audio_status_mutex_unlock();

    return 0;
}

int a2dp_audio_deinit(void)
{
    TRACE_A2DP_DECODER_I("[DEINIT]");

    sbm_release_cpu_freq_during_speed_tuning();

    a2dp_audio_status_mutex_lock();

    a2dp_audio_detect_next_packet_callback_register(NULL);
    a2dp_audio_detect_store_packet_callback_register(NULL);
#ifdef A2DP_DECODER_CROSS_CORE
    a2dp_decoder_bth_stop_stream();
#ifdef A2DP_DECODER_CROSS_CORE_USE_M55
    if (!a2dp_audio_retrigger_is_on_process()) {
        app_dsp_m55_deinit(APP_DSP_M55_USER_A2DP);
        a2dp_decoder_cc_sema_deinit();
    }
#endif
#endif
    sbm_reset_target_jitter_adjustment_env();
    a2dp_audio_context.audio_decoder.audio_decoder_deinit();
    memset(&(a2dp_audio_context.audio_decoder), 0, sizeof(A2DP_AUDIO_DECODER_T));
    memset(&(a2dp_audio_context.output_cfg), 0, sizeof(A2DP_AUDIO_OUTPUT_CONFIG_T));
    a2dp_audio_list_clear(a2dp_audio_context.audio_datapath.input_raw_packet_list);
    a2dp_audio_list_free(a2dp_audio_context.audio_datapath.input_raw_packet_list);
    a2dp_audio_context.audio_datapath.input_raw_packet_list = NULL;
    a2dp_audio_list_clear(a2dp_audio_context.audio_datapath.output_pcm_packet_list);
    a2dp_audio_list_free(a2dp_audio_context.audio_datapath.output_pcm_packet_list);
    a2dp_audio_context.audio_datapath.output_pcm_packet_list = NULL;

#ifndef IS_ALWAYS_KEEP_A2DP_TARGET_BUFFER_COUNT
    a2dp_audio_context.custom_delay_ms = A2DP_INVALID_DELAY_MS;
#endif

    size_t total = 0, used = 0, max_used = 0;
    a2dp_audio_heap_info(&total, &used, &max_used);
    TRACE_A2DP_DECODER_I("[DEINIT] heap info: total - %d, used - %d, max_used - %d.", total, used, max_used);
    //ASSERT_A2DP_DECODER(used == 0, "[%s] used != 0", __func__);

    a2dp_audio_set_store_packet_status(A2DP_AUDIO_DECODER_STORE_PACKET_STATUS_IDLE);
    a2dp_audio_set_playback_status(A2DP_AUDIO_DECODER_PLAYBACK_STATUS_IDLE);
    a2dp_audio_set_status(A2DP_AUDIO_DECODER_STATUS_NULL);
#if A2DP_DECODER_HISTORY_SEQ_SAVE
    a2dp_audio_reset_history_seq();
#endif
    a2dp_audio_status_mutex_unlock();

#if defined(HIGH_EFFICIENCY_TX_PWR_CTRL)||defined(IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING)
    app_audio_reset_rssi_based_env_ajustment(false);
#endif

    return 0;
}

int a2dp_audio_preparse_stop(void)
{
    int cnt = 0;
    a2dp_audio_sysfreq_boost_start(5,false);
    cnt = 50;
    do {
        if (a2dp_audio_get_playback_status() == A2DP_AUDIO_DECODER_PLAYBACK_STATUS_IDLE){
            a2dp_audio_context.preparse_stop = true;
            TRACE_A2DP_DECODER_I("[PREPARSE_STOP] PLAYBACK_STATUS_IDLE cnt:%d", cnt);
            A2DP_AUDIO_HEADFRAME_INFO_T headframe_info;
            list_t *list = a2dp_audio_context.audio_datapath.input_raw_packet_list;
            uint32_t list_size = 0;
            a2dp_audio_decoder_headframe_info_get(&headframe_info);
            list_size = a2dp_audio_list_length(list);
            TRACE_A2DP_DECODER_I("[PREPARSE_STOP] list:%d seq:%d", list_size, headframe_info.sequenceNumber);
            break;
        }else{
            osDelay(1);
        }
    }while(--cnt > 0);

    return 0;
}

int a2dp_audio_stop(void)
{
    TRACE_A2DP_DECODER_I("[STOP]");
    int cnt = 0;

    a2dp_audio_throughput_checker_stop();
    a2dp_audio_set_status(A2DP_AUDIO_DECODER_STATUS_STOP);

    cnt = 50;
    do {
        if (a2dp_audio_get_playback_status() == A2DP_AUDIO_DECODER_PLAYBACK_STATUS_IDLE){
            TRACE_A2DP_DECODER_I("[DEINIT]PLAYBACK_STATUS_IDLE cnt:%d", cnt);
            break;
        }else{
            osDelay(1);
        }
    }while(--cnt > 0);
    cnt = 50;
    do {
        if (a2dp_audio_get_store_packet_status() == A2DP_AUDIO_DECODER_STORE_PACKET_STATUS_IDLE){
            TRACE_A2DP_DECODER_I("[DEINIT] STORE_PACKET_STATUS_IDLE cnt:%d", cnt);
            break;
        }else{
            osDelay(1);
        }
    }while(--cnt > 0);

    return 0;
}

int a2dp_audio_start(void)
{
    TRACE_A2DP_DECODER_I("[START]");
    a2dp_audio_throughput_checker_start();
    a2dp_audio_status_mutex_lock();
    a2dp_audio_set_status(A2DP_AUDIO_DECODER_STATUS_START);
    a2dp_audio_status_mutex_unlock();
    return 0;
}

int a2dp_audio_detect_next_packet_callback_register(A2DP_AUDIO_DETECT_NEXT_PACKET_CALLBACK callback)
{
    a2dp_audio_status_mutex_lock();
    a2dp_audio_detect_next_packet_callback = callback;
    a2dp_audio_status_mutex_unlock();

    return 0;
}

int a2dp_audio_detect_store_packet_callback_register(A2DP_AUDIO_DETECT_NEXT_PACKET_CALLBACK callback)
{
    a2dp_audio_status_mutex_lock();
    a2dp_audio_store_packet_callback = callback;
    a2dp_audio_status_mutex_unlock();

    return 0;
}

int a2dp_audio_detect_first_packet(void)
{
    a2dp_audio_status_mutex_lock();
    a2dp_audio_context.need_detect_first_packet = true;
    a2dp_audio_status_mutex_unlock();
    return 0;
}

int a2dp_audio_detect_first_packet_clear(void)
{
    a2dp_audio_status_mutex_lock();
    a2dp_audio_context.need_detect_first_packet = false;
    a2dp_audio_status_mutex_unlock();
    return 0;
}

int a2dp_audio_discards_packet(uint32_t packets)
{
    int nRet = 0;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_status_mutex_lock();
        nRet = a2dp_audio_context.audio_decoder.audio_decoder_discards_packet(packets);
        a2dp_audio_status_mutex_unlock();
    }else{
        nRet = -1;
    }

    return nRet;
}

int a2dp_audio_synchronize_dest_packet_mut(uint32_t mtu)
{
    int nRet = 0;
    int cnt = 50;
    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        do {
            if (a2dp_audio_get_playback_status() == A2DP_AUDIO_DECODER_PLAYBACK_STATUS_IDLE){
                nRet = a2dp_audio_context.audio_decoder.audio_decoder_synchronize_dest_packet_mut(mtu);
                break;
            }else{
                osThreadYield();
            }
        }while(--cnt > 0);
    }else{
        nRet = -1;
    }

    return nRet;
}

int a2dp_audio_discards_samples(uint32_t samples)
{
    return a2dp_audio_context.audio_decoder.a2dp_audio_discards_samples(samples);
}

int a2dp_audio_convert_list_to_samples(uint32_t *samples)
{
    return a2dp_audio_context.audio_decoder.a2dp_audio_convert_list_to_samples(samples);
}

int a2dp_audio_get_packet_samples(void)
{
    A2DP_AUDIO_LASTFRAME_INFO_T lastframe_info;
    uint32_t packet_samples = 0;
    uint16_t totalSubSequenceNumber = 1;

    int ret = a2dp_audio_lastframe_info_get(&lastframe_info);
    if (-1 == ret)
    {
        return 0;
    }

    if (lastframe_info.totalSubSequenceNumber){
        totalSubSequenceNumber = lastframe_info.totalSubSequenceNumber;
    }

    packet_samples = totalSubSequenceNumber * lastframe_info.frame_samples;

    return packet_samples;
}

static int a2dp_audio_internal_lastframe_info_ptr_get(A2DP_AUDIO_LASTFRAME_INFO_T **lastframe_info)
{
    int nRet = 0;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        *lastframe_info = &a2dp_audio_lastframe_info;
    }else{
        *lastframe_info = NULL;
        nRet = -1;
    }

    return nRet;
}

int a2dp_audio_lastframe_info_get(A2DP_AUDIO_LASTFRAME_INFO_T *lastframe_info)
{
    int nRet = 0;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_buffer_mutex_lock();
        memcpy(lastframe_info, &a2dp_audio_lastframe_info, sizeof(A2DP_AUDIO_LASTFRAME_INFO_T));
        a2dp_audio_buffer_mutex_unlock();
    }else{
        memset(lastframe_info, 0, sizeof(A2DP_AUDIO_LASTFRAME_INFO_T));
        nRet = -1;
    }

    return nRet;
}

int a2dp_audio_decoder_internal_check_sum_reset(void)
{
    check_sum_seed = 0;
    return 0;
}

uint32_t a2dp_audio_decoder_internal_check_sum_generate(const uint8_t *buf, uint32_t len)
{
#if A2DP_DECODER_CHECKER
    check_sum_seed = crc32(check_sum_seed, buf, len);
#else
    check_sum_seed = 0;
#endif
    return check_sum_seed;
}

int a2dp_audio_lastframe_info_reset_undecodeframe(void)
{
    int nRet = 0;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_buffer_mutex_lock();
        a2dp_audio_lastframe_info.undecode_frames = 0;
        a2dp_audio_lastframe_info.undecode_max_frames = 0;
        a2dp_audio_lastframe_info.undecode_min_frames = 0xffff;
        a2dp_audio_decoder_internal_check_sum_reset();
        a2dp_audio_lastframe_info.check_sum = 0;
        a2dp_audio_buffer_mutex_unlock();
    }else{
        nRet = -1;
    }

    return nRet;
}

int a2dp_audio_decoder_internal_lastframe_info_set(A2DP_AUDIO_DECODER_LASTFRAME_INFO_T *lastframe_info)
{
    a2dp_audio_buffer_mutex_lock();
    a2dp_audio_lastframe_info.sequenceNumber        = lastframe_info->sequenceNumber;
    a2dp_audio_lastframe_info.timestamp             = lastframe_info->timestamp;
    a2dp_audio_lastframe_info.curSubSequenceNumber  = lastframe_info->curSubSequenceNumber;
    a2dp_audio_lastframe_info.totalSubSequenceNumber= lastframe_info->totalSubSequenceNumber;
    a2dp_audio_lastframe_info.frame_samples         = lastframe_info->frame_samples;
    a2dp_audio_lastframe_info.list_samples          = lastframe_info->list_samples;
    a2dp_audio_lastframe_info.decoded_frames        = lastframe_info->decoded_frames;
    a2dp_audio_lastframe_info.undecode_frames       = lastframe_info->undecode_frames;

    a2dp_audio_lastframe_info.undecode_max_frames =
        MAX(a2dp_audio_lastframe_info.undecode_frames, a2dp_audio_lastframe_info.undecode_max_frames);
    a2dp_audio_lastframe_info.undecode_min_frames =
        MIN(a2dp_audio_lastframe_info.undecode_frames, a2dp_audio_lastframe_info.undecode_min_frames);

    a2dp_audio_lastframe_info.stream_info           = lastframe_info->stream_info;
    a2dp_audio_lastframe_info.check_sum             = lastframe_info->check_sum;
    a2dp_audio_buffer_mutex_unlock();

    return 0;
}

int a2dp_audio_synchronize_packet(A2DP_AUDIO_SYNCFRAME_INFO_T *sync_info, uint32_t mask)
{
    int nRet = 0;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_status_mutex_lock();
        nRet = a2dp_audio_context.audio_decoder.audio_decoder_synchronize_packet(sync_info, mask);
        if (nRet == A2DP_DECODER_NOT_SUPPORT){
            //can't support synchronize packet, so return fake val;
            nRet = A2DP_DECODER_NO_ERROR;
        }
        a2dp_audio_status_mutex_unlock();
    }else{
        nRet = -1;
    }

    return nRet;
}

int a2dp_audio_decoder_headframe_info_get(A2DP_AUDIO_HEADFRAME_INFO_T *headframe_info)
{
    int nRet = 0;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        nRet = a2dp_audio_context.audio_decoder.audio_decoder_headframe_info_get(headframe_info);
    }else{
        memset(headframe_info, 0, sizeof(A2DP_AUDIO_HEADFRAME_INFO_T));
        nRet = -1;
    }

    return nRet;
}

int a2dp_audio_refill_packet(void)
{
    int refill_cnt = 0;

#if defined(A2DP_AUDIO_REFILL_AFTER_NO_CACHE)
    refill_cnt += a2dp_audio_context.skip_frame_cnt_after_no_cache;
#endif
    refill_cnt += a2dp_audio_context.mute_frame_cnt_after_no_cache;

    return refill_cnt;
}

bool a2dp_audio_auto_synchronize_support(void)
{
    bool nRet = 0;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_buffer_mutex_lock();
        nRet = a2dp_audio_context.audio_decoder.auto_synchronize_support > 0 ? true: false;
        a2dp_audio_buffer_mutex_unlock();
    }else{
        nRet = 0;
    }

    return nRet;
}

A2DP_AUDIO_OUTPUT_CONFIG_T *a2dp_audio_get_output_config(void)
{
    A2DP_AUDIO_OUTPUT_CONFIG_T *output_config = NULL;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_buffer_mutex_lock();
        output_config = &a2dp_audio_context.output_cfg;
        a2dp_audio_buffer_mutex_unlock();
    }else{
        output_config = NULL;
    }

    return output_config;
}

int a2dp_audio_latency_factor_setlow(void)
{
    a2dp_audio_latency_factor = A2DP_AUDIO_LATENCY_LOW_FACTOR;
    return 0;
}

int a2dp_audio_latency_factor_sethigh(void)
{
    a2dp_audio_latency_factor = A2DP_AUDIO_LATENCY_HIGH_FACTOR;
    app_start_post_chopping_timer();
    return 0;
}

float a2dp_audio_latency_factor_get(void)
{
    return a2dp_audio_latency_factor;
}

int a2dp_audio_latency_factor_set(float factor)
{
    a2dp_audio_latency_factor = factor;
    if (a2dp_audio_latency_factor == A2DP_AUDIO_LATENCY_HIGH_FACTOR)
    {
        app_start_post_chopping_timer();
    }
    return 0;
}

int a2dp_audio_latency_factor_status_get(A2DP_AUDIO_LATENCY_STATUS_E *latency_status, float *more_latency_factor)
{
    if (a2dp_audio_latency_factor == A2DP_AUDIO_LATENCY_HIGH_FACTOR){
        *latency_status = A2DP_AUDIO_LATENCY_STATUS_HIGH;
        *more_latency_factor = A2DP_AUDIO_LATENCY_MORE_FACTOR;
    }else{
        *latency_status = A2DP_AUDIO_LATENCY_STATUS_LOW;
        *more_latency_factor = 1.0f;
    }
    return 0;
}

int a2dp_audio_frame_delay_get(void)
{
    return get_in_cp_frame_delay();
}

int a2dp_audio_dest_packet_mut_get(void)
{
    return a2dp_audio_context.dest_packet_mut;
}

void a2dp_audio_dest_packet_mut_set(uint16_t custom_mtu)
{
    a2dp_audio_context.dest_packet_mut = custom_mtu;
}

int a2dp_audio_custom_delay_ms_get(void)
{
    return a2dp_audio_context.custom_delay_ms;
}

void a2dp_audio_custom_delay_ms_set(uint16_t custom_delay_ms)
{
    a2dp_audio_context.custom_delay_ms = custom_delay_ms;
}

#ifdef A2DP_DECODER_CROSS_CORE
void a2dp_audio_start_seq_get(A2DP_AUDIO_STARTFRAME_INFO_T *headframe_info)
{
    headframe_info->sequenceNumber = a2dp_audio_context.start_seq_header.sequenceNumber;
    headframe_info->curSubSequenceNumber = a2dp_audio_context.start_seq_header.curSubSequenceNumber;
    headframe_info->is_ignore = a2dp_audio_context.start_seq_header.is_ignore;
}

void a2dp_audio_start_seq_set(A2DP_AUDIO_STARTFRAME_INFO_T headframe_info)
{
    a2dp_audio_context.start_seq_header.sequenceNumber = headframe_info.sequenceNumber;
    a2dp_audio_context.start_seq_header.curSubSequenceNumber = headframe_info.curSubSequenceNumber;
    a2dp_audio_context.start_seq_header.is_ignore = headframe_info.is_ignore;
}
#endif

int a2dp_audio_set_channel_select(A2DP_AUDIO_CHANNEL_SELECT_E chnl_sel)
{
    int nRet = A2DP_DECODER_NO_ERROR;

    if (a2dp_audio_get_status() == A2DP_AUDIO_DECODER_STATUS_START){
        a2dp_audio_context.chnl_sel = chnl_sel;
        if (a2dp_audio_context.audio_decoder.audio_decoder_channel_select) {
             nRet = a2dp_audio_context.audio_decoder.audio_decoder_channel_select(chnl_sel);
        }
    }

    return nRet;
}

float a2dp_audio_get_sample_reference(void)
{
    TRACE(1,"a2dp_audio_get_sample_reference:%d", (int32_t)(a2dp_audio_context.output_cfg.factor_reference * 10000000));
    return a2dp_audio_context.output_cfg.factor_reference;
}

int8_t a2dp_audio_get_current_buf_size(void)
{
    TRACE(1,"a2dp_audio_get_current_buf_size:%d",(int8_t)(a2dp_audio_context.average_packet_mut+0.5f));
    return (int8_t)(a2dp_audio_context.average_packet_mut+0.5f);
}

void a2dp_audio_update_latency_chunk_count(uint16_t chunkCount)
{
    a2dp_audio_context.dest_packet_mut = chunkCount;
}

#if defined(IBRT)
void app_ibrt_send_custom_play_speed_tuning_req(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_SEND_CUSTOM_PLAY_SPEED_TUNING_REQ, p_buff, length);
}

void app_ibrt_request_peer_custom_play_speed_tuning(void)
{
    if (a2dp_audio_context.audio_decoder.audio_decoder_get_sync_custom_data)
    {
        AUDIO_SYNC_CUSTOM_DATA_T req;
        a2dp_audio_context.audio_decoder.audio_decoder_get_sync_custom_data(
            req.custom_data);

        tws_ctrl_send_cmd_high_priority(APP_TWS_CMD_SEND_CUSTOM_PLAY_SPEED_TUNING_REQ,
            (uint8_t *)&req, sizeof(AUDIO_SYNC_CUSTOM_DATA_T));
    }
}

void app_ibrt_custom_play_speed_tuning_req_handler(uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen)
{
    if (a2dp_audio_context.audio_decoder.audio_decoder_push_sync_custom_data)
    {
        a2dp_audio_context.audio_decoder.audio_decoder_push_sync_custom_data(
            ptrParam);
    }

#ifdef A2DP_DECODER_CROSS_CORE
#ifdef A2DP_SCALABLE_ON
        SBM_OPERATION_ENV_T sbm_cc_env;
        audio_audio_scalable_get_sync_custom_data((uint8_t *)&sbm_cc_env);
        app_dsp_m55_bridge_send_cmd(
                             CROSS_CORE_TASK_CMD_A2DP_SCALABLE_SBM_TO_OFF_BTH, \
                             (uint8_t *)&sbm_cc_env, sizeof(SBM_OPERATION_ENV_T));
#endif
#endif
}

void app_ibrt_sync_target_buf_ms_req(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_SYNC_TARGET_BUF_MS_REQ, p_buff, length);
}

void app_ibrt_request_peer_sync_target_buf_ms(void)
{
    uint16_t target_buf_ms = a2dp_audio_context.custom_delay_ms;
    TRACE(0, "%s send targetms %d", __func__, target_buf_ms);
    tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_TARGET_BUF_MS_REQ,
        (uint8_t *)&target_buf_ms, sizeof(target_buf_ms));
}

void app_ibrt_sync_target_buf_ms_req_handler(uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen)
{
    sbm_update_local_target_jitter_buf_ms(*(uint16_t *)ptrParam);
}
#endif

static bool sbm_is_manual_ajdusting_on_going = false;
static bool sbm_is_auto_ajdusting_on_going = false;
static bool sbm_is_initial_jitter_buf_level_reported = false;
static bool sbm_is_algorithm_adjusting_enabled = true;

static void sbm_default_jitter_buf_level_update_completed_callback(
    SBM_JITTER_BUF_LEVEL_UPDATE_COMPLETED_EVENT_E type,
    int32_t target_ms, int32_t current_average_ms)
{
    TRACE(0, "SBM jitter buf level updated completed: Type %d (0:init,1:manual,2:auto)", type);
    TRACE(0, "Target jitter buf is %d ms, current average jitter buf is %d ms",
        target_ms, current_average_ms);
}

static int32_t sbm_convert_jitter_buf_count_to_ms(float bufCount)
{
    uint32_t currentSampleRate = (uint32_t)bt_get_sbc_sample_rate();
    uint32_t sampleCountPerBuf;

    uint8_t codecType = bt_sbc_player_get_codec_type();

    switch (codecType)
    {
        case BTIF_AVDTP_CODEC_TYPE_SBC:
            sampleCountPerBuf = 128;
            break;
        case BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC:
            sampleCountPerBuf = 1024;
            break;
        case BTIF_AVDTP_CODEC_TYPE_NON_A2DP:
#ifdef A2DP_SCALABLE_ON
            sampleCountPerBuf = 864;
#else
            sampleCountPerBuf = 256;
#endif
            break;
        default:
            return -1;
    }

    int32_t jitterBufInMs =
        ((int32_t)(bufCount*sampleCountPerBuf*1000))/currentSampleRate;

    return jitterBufInMs;
}

int32_t sbm_get_current_target_jitter_buf_length(void)
{
    return sbm_convert_jitter_buf_count_to_ms(
        a2dp_audio_context.dest_packet_mut);
}

int32_t sbm_get_current_average_jitter_buf_length(void)
{
    if (A2DP_AUDIO_DECODER_STATUS_START == a2dp_audio_get_status())
    {
        return sbm_convert_jitter_buf_count_to_ms(
            a2dp_audio_context.average_packet_mut);
    }
    else
    {
        return -1;
    }
}

void sbm_register_jittr_buf_level_update_completed_event_cb(SBM_JITTER_BUF_LEVEL_UPDATE_COMPLETED_FUNC func)
{
    sbmJitterBufLevelUpdatedCompletedCallback = func;
}

void sbm_reset_target_jitter_adjustment_env(void)
{
    TRACE(0, "Reset SBM env.");
    uint32_t lock = int_lock();
    sbm_clear_manual_adjustment_on_going_flag();
    sbm_is_auto_ajdusting_on_going = false;
    sbm_is_initial_jitter_buf_level_reported = false;
    sbm_is_algorithm_adjusting_enabled = true;
    int_unlock(lock);
}

bool sbm_is_manual_adjustment_on_going(void)
{
    return sbm_is_manual_ajdusting_on_going;
}

void sbm_clear_manual_adjustment_on_going_flag(void)
{
    sbm_is_manual_ajdusting_on_going = false;
}

void sbm_set_manual_adjustment_on_going_flag(void)
{
    sbm_is_manual_ajdusting_on_going = true;
}

bool sbm_is_algorithm_adjustment_enabled(void)
{
    return sbm_is_algorithm_adjusting_enabled;
}

static bool sbm_is_auto_adjustment_on_going(void)
{
    return sbm_is_auto_ajdusting_on_going;
}

void sbm_set_auto_adjustment(void)
{
    if (A2DP_AUDIO_DECODER_STATUS_START != a2dp_audio_get_status())
    {
        return;
    }

    uint32_t lock = int_lock();
    sbm_is_auto_ajdusting_on_going = true;
    int_unlock(lock);
}

void sbm_target_jitter_buffer_initiative_approached(void)
{
    if (!sbm_is_initial_jitter_buf_level_reported)
    {
        sbm_is_initial_jitter_buf_level_reported = true;
    int32_t target_ms =
        sbm_convert_jitter_buf_count_to_ms(a2dp_audio_context.dest_packet_mut);
    int32_t average_ms =
        sbm_convert_jitter_buf_count_to_ms(a2dp_audio_context.average_packet_mut);

    sbmJitterBufLevelUpdatedCompletedCallback(
                    SBM_INITIAL_BUF_CNT_APPROACHED_AFTER_STREAM_STARTED,
                    target_ms, average_ms);
    }
}

void sbm_bump_cpu_freq_during_speed_tuning(void)
{
    TRACE(0, "set sbm manual flag and bump up freq.");
    sbm_set_manual_adjustment_on_going_flag();
#ifndef A2DP_DECODER_CROSS_CORE
    app_sysfreq_req(APP_SYSFREQ_USER_SBM, APP_SYSFREQ_208M);
#endif
}

void sbm_release_cpu_freq_during_speed_tuning(void)
{
#ifndef A2DP_DECODER_CROSS_CORE
    app_sysfreq_req(APP_SYSFREQ_USER_SBM, APP_SYSFREQ_32K);
#endif
}

void sbm_target_jitter_buffer_updating_completed(void)
{
    SBM_JITTER_BUF_LEVEL_UPDATE_COMPLETED_EVENT_E type;
    if (sbm_is_manual_adjustment_on_going())
    {
        type = SBM_MANUAL_TARGET_BUF_CNT_APPROACHED;
    }
    else if (sbm_is_auto_adjustment_on_going())
    {
        type = SBM_AUTO_TARGET_BUF_RESTORD;
    }
    else
    {
        return;
    }

    TRACE(0, "SBM updating completed.");
    sbm_release_cpu_freq_during_speed_tuning();
    uint32_t lock = int_lock();
    sbm_clear_manual_adjustment_on_going_flag();
    sbm_is_auto_ajdusting_on_going = false;
    int_unlock(lock);

    int32_t target_ms =
        sbm_convert_jitter_buf_count_to_ms(a2dp_audio_context.dest_packet_mut);
    int32_t average_ms =
        sbm_convert_jitter_buf_count_to_ms(a2dp_audio_context.average_packet_mut);

    sbmJitterBufLevelUpdatedCompletedCallback(
        type,
        target_ms,
        average_ms);
}

bool sbm_update_local_target_jitter_buf_ms(uint16_t targetMs)
{
    a2dp_audio_context.custom_delay_ms = targetMs;

    if (A2DP_AUDIO_DECODER_STATUS_READY >= a2dp_audio_get_status())
    {
        TRACE(0,"ADUIO_STATUS <= READY ");
        TRACE(0, "start updating jitter buf ms to new dst buffer ms %d", targetMs);
        sbm_bump_cpu_freq_during_speed_tuning();
        uint32_t lock = int_lock();
        sbm_set_manual_adjustment_on_going_flag();
        int_unlock(lock);

        return true;
    }
    else
    {
        TRACE(0, "start updating jitter buf ms to new dst buffer ms %d", targetMs);
        uint8_t targetCount = sbm_convert_targetMs_to_mtu(targetMs);
        TRACE(0, "current average jitter buf count %d and former dst jitter buffer count %d",
            (uint8_t)a2dp_audio_context.average_packet_mut, a2dp_audio_context.dest_packet_mut);
        TRACE(0, "start updating jitter buf count to new dst buffer count %d", targetCount);
        if ((uint8_t)a2dp_audio_context.average_packet_mut != targetCount)
        {
            sbm_bump_cpu_freq_during_speed_tuning();
            uint32_t lock = int_lock_global();
            sbm_set_manual_adjustment_on_going_flag();
            a2dp_audio_context.not_chopping_dest_mut = targetCount;
            if (!a2dp_audio_chopping_is_in_post())
            {
                a2dp_audio_context.dest_packet_mut = targetCount;
            }
            int_unlock_global(lock);
            return true;
        }
        else
        {
            a2dp_audio_context.dest_packet_mut = targetCount;
            TRACE(0, "average_packet_mut = targetCount, do not need update");
            int32_t target_ms =
                sbm_convert_jitter_buf_count_to_ms(a2dp_audio_context.dest_packet_mut);
            int32_t average_ms =
                sbm_convert_jitter_buf_count_to_ms(a2dp_audio_context.average_packet_mut);

            sbmJitterBufLevelUpdatedCompletedCallback(
                SBM_MANUAL_TARGET_BUF_CNT_APPROACHED,
                target_ms,
                average_ms);
            return false;
        }
    }
}

uint8_t sbm_convert_targetMs_to_mtu(uint16_t targetMs)
{
    uint32_t currentSampleRate = (uint32_t)bt_get_sbc_sample_rate();
    uint32_t targetBufCount;
    uint32_t sampleCountPerBuf;

    uint8_t codecType = bt_sbc_player_get_codec_type();

    switch (codecType)
    {
        case BTIF_AVDTP_CODEC_TYPE_SBC:
            sampleCountPerBuf = 128;
            TRACE(0, "Current Codec: SBC, sample rate %d", currentSampleRate);
            break;
        case BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC:
            sampleCountPerBuf = 1024;
            TRACE(0, "Current Codec: AAC, sample rate %d", currentSampleRate);
            break;
        case BTIF_AVDTP_CODEC_TYPE_NON_A2DP:
            sampleCountPerBuf = 864;
            TRACE(0, "Current Codec: Scalable, sample rate %d", currentSampleRate);
            break;
        default:
            return a2dp_audio_context.dest_packet_mut;
    }

    targetBufCount = (targetMs*currentSampleRate/1000)/sampleCountPerBuf;

    uint8_t min_targetBufCount = 0;

#ifdef ADVANCE_FILL_ENABLED
    min_targetBufCount = 5;
#else
    min_targetBufCount = 3;
#endif

    if (targetBufCount < min_targetBufCount)
    {
        targetBufCount = min_targetBufCount;
    }

    TRACE(0, "Converted target buf count %d", targetBufCount);
    TRACE(0, "Actual target jitter buffer %d ms",
        (targetBufCount*sampleCountPerBuf*1000)/currentSampleRate);

    return targetBufCount;
}

void sbm_update_target_jitter_buf_length(uint16_t targetMs)
{
    TRACE(0, "[%s] targetMs %d", __func__, targetMs);
    a2dp_audio_context.custom_delay_ms = targetMs;
    if (A2DP_AUDIO_DECODER_STATUS_READY >= a2dp_audio_get_status())
    {
        TRACE(0,"ADUIO_STATUS <= READY ");

        sbm_bump_cpu_freq_during_speed_tuning();
        uint32_t lock = int_lock_global();
        sbm_set_manual_adjustment_on_going_flag();
        int_unlock_global(lock);
#ifdef IBRT
        app_ibrt_request_peer_sync_target_buf_ms();
#endif
        return;
   }

    TRACE(0,"ADUIO_STATUS > READY ");
#ifdef IBRT
    if (sbm_update_local_target_jitter_buf_ms(targetMs))
    {
        app_ibrt_request_peer_sync_target_buf_ms();
    }
#endif
}

static a2dp_audio_status_updated_cb a2dp_audio_status_updated_cb_func = NULL;
int32_t app_a2dp_audio_status_updated_callback_register(a2dp_audio_status_updated_cb cb)
{
    a2dp_audio_status_updated_cb_func = cb;
    return 0;
}

void a2dp_audio_status_updated_callback(bool isA2dpAudioOn)
{
    if (a2dp_audio_status_updated_cb_func)
    {
        if (isA2dpAudioOn)
        {
            TRACE(0, "a2dp audio is started.");
            a2dp_audio_status_updated_cb_func(A2DP_AUDIO_STARTED);
        }
        else
        {
            TRACE(0, "a2dp audio is stopped.");
            a2dp_audio_status_updated_cb_func(A2DP_AUDIO_STOPPED);
        }
    }
}

static sbm_feature_updated_callback_func_t sbm_feature_updated_cb = NULL;

void sbm_control_local_scalable_sbm_feature_handler(uint8_t device_id, bool isEnable)
{
    struct BT_DEVICE_T* curr_device = app_bt_get_device(device_id);
    if (curr_device)
    {
        if (isEnable != curr_device->isUseLocalSBM)
        {
            TRACE(0, "isUseLocalSBM of device %d is changed from %d to %d",
                device_id, !isEnable, isEnable);
        }
        curr_device->isUseLocalSBM = isEnable;
    }
}

void sbm_local_scalable_sbm_feature_updated_callback(uint8_t device_id, bool isEnable, bool isSuccessful)
{
    TRACE(0, "SBM feature on dev %d updated to isEnable %d isSuccessful %d",
        device_id, isEnable, isSuccessful);
    if (isSuccessful)
    {
        sbm_control_local_scalable_sbm_feature_handler(device_id, isEnable);
    }

    if (sbm_feature_updated_cb)
    {
        sbm_feature_updated_cb(device_id, isEnable, isSuccessful);
    }
}

void sbm_register_local_scalable_sbm_feature_updated_callback(sbm_feature_updated_callback_func_t callback_func)
{
    sbm_feature_updated_cb = callback_func;
}

void sbm_control_local_scalable_sbm_feature(uint8_t device_id, bool isEnable)
{
#ifdef IBRT
    if (app_ibrt_if_is_tws_link_connected())
    {
        SBM_CONTROL_REQ_T req;
        req.device_id = device_id;
        req.isEnable = isEnable;

        tws_ctrl_send_cmd(APP_TWS_CMD_CONTROL_SBM, (uint8_t *)&req, sizeof(req));
    }
    else
    {
        sbm_local_scalable_sbm_feature_updated_callback(device_id, isEnable, true);
    }
#else
    sbm_local_scalable_sbm_feature_updated_callback(device_id, isEnable, true);
#endif
}

bool sbm_is_local_scalable_sbm_enabled(uint8_t device_id)
{
    struct BT_DEVICE_T* curr_device = app_bt_get_device(device_id);
    if (curr_device)
    {
        return curr_device->isUseLocalSBM;
    }

    return false;
}

void a2dp_audio_scalable_update_local_scalable_sbm_feature(bool isEnable)
{
#if defined(A2DP_SCALABLE_ON)
    if (isEnable)
    {
        TRACE(0, "Enable local scalable SBM.");
        sbm_is_algorithm_adjusting_enabled = true;
    }
    else
    {
        TRACE(0, "Disable local scalable SBM.");
        sbm_is_algorithm_adjusting_enabled = false;
    }
#endif
}

#if defined(HIGH_EFFICIENCY_TX_PWR_CTRL)||defined(IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING)
static bool isSwitchedToLowRssiTxGain = true;
static bool isUseLowerA2dpFreq = true;
void app_audio_reset_rssi_based_env_ajustment(bool music_on)
{
    app_bt_reset_rssi_collector();

    isUseLowerA2dpFreq = true;
    // reset to low power tx gain mode
    isSwitchedToLowRssiTxGain = true;

#if defined(HIGH_EFFICIENCY_TX_PWR_CTRL)
    if(music_on)
    {
        //music on set to low power mode
        bt_drv_rf_high_efficency_tx_pwr_ctrl(true, true);
    }
    else
    {
        //music off set to default value
        bt_drv_rf_high_efficency_tx_pwr_ctrl(false, true);
    }
#endif
}

void app_a2dp_audio_use_lower_frequency_flag_set(bool lower_freq_flag)
{
    isUseLowerA2dpFreq = lower_freq_flag;
}

#define APP_AUDIO_LOWER_RSSI_THRESHOLD  (-70)
#define APP_AUDIO_UPPER_RSSI_THRESHOLD  (-45)

#define APP_AUDIO_LOW_POWER_TX_PA_RSSI_THRESHOLD    (-70)

#ifdef IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING
static bool app_audio_is_mixing_freq_activated(void)
{
    // limit the mixing frequency coordinating to be activated only when the normal frequency is 26M
    return (APP_SYSFREQ_26M == a2dp_audio_sysfreq_normalfreq);
}
#endif

void app_audio_rssi_based_env_ajdustment_handler(void)
{
    struct BT_DEVICE_T *curr_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());

    if (curr_device)
    {
        rx_agc_t mobile_agc;
        bool ret = bt_drv_reg_op_read_rssi_in_dbm(curr_device->acl_conn_hdl, &mobile_agc);
        if (ret)
        {
            int32_t average_rssi = app_bt_tx_rssi_analyzer(mobile_agc.rssi);
            //TRACE(0, "A:    %d  db:  %d  db", average_rssi, mobile_agc.rssi);

        #if defined(HIGH_EFFICIENCY_TX_PWR_CTRL)
            if ((average_rssi < APP_AUDIO_LOW_POWER_TX_PA_RSSI_THRESHOLD) && isSwitchedToLowRssiTxGain)
            {
                TRACE(0, "switch to normal mode tx gain as rssi is %d", average_rssi);
                isSwitchedToLowRssiTxGain = false;

                bt_drv_rf_high_efficency_tx_pwr_ctrl(false, false);
            }
            else if ((average_rssi >= APP_AUDIO_LOW_POWER_TX_PA_RSSI_THRESHOLD) && !isSwitchedToLowRssiTxGain)
            {
                TRACE(0, "switch to low power mode tx gain as rssi is %d", average_rssi);
                isSwitchedToLowRssiTxGain = true;

                bt_drv_rf_high_efficency_tx_pwr_ctrl(true, true);
            }
        #endif

        #if defined(IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING)
            if (app_audio_is_mixing_freq_activated())
            {
                if ((average_rssi < APP_AUDIO_LOWER_RSSI_THRESHOLD) && isUseLowerA2dpFreq)
                {
                    isUseLowerA2dpFreq = false;
                    app_audio_set_a2dp_freq(APP_SYSFREQ_52M);
                }
                else if ((average_rssi >= APP_AUDIO_UPPER_RSSI_THRESHOLD) && !isUseLowerA2dpFreq)
                {
                    isUseLowerA2dpFreq = true;
                    app_audio_set_a2dp_freq(APP_SYSFREQ_26M);
                }
            }
        #endif
        }
    }
}
#endif

void app_audio_set_a2dp_freq(uint32_t freq)
{
    enum APP_SYSFREQ_FREQ_T configuredFreq = (enum APP_SYSFREQ_FREQ_T)freq;

    if (isInPostChoppingPeriod)
    {
        configuredFreq = APP_SYSFREQ_104M;
    }
    else
    {
    #ifdef IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING
        if (!isUseLowerA2dpFreq)
        {
            configuredFreq = APP_SYSFREQ_52M;
        }
    #endif
    }

    app_sysfreq_req(APP_SYSFREQ_USER_BT_A2DP, configuredFreq);
}

extern "C" uint8_t is_a2dp_mode(void);
static void app_post_chopping_timer_handler(void const *param);
osTimerDef (APP_POST_CHOPPING_TIMER, app_post_chopping_timer_handler);
osTimerId app_post_chopping_timer_id = NULL;
static void app_post_chopping_timer_handler(void const *param)
{
    TRACE(0, "%s", __func__);
    isInPostChoppingPeriod = false;
    a2dp_audio_latency_factor_setlow();
    a2dp_audio_context.dest_packet_mut = a2dp_audio_context.not_chopping_dest_mut;
    if (!is_a2dp_mode())
    {
        return;
    }

    a2dp_audio_sysfreq_reset();
}

void app_start_post_chopping_timer(void)
{
    if (NULL == app_post_chopping_timer_id)
    {
        app_post_chopping_timer_id =
            osTimerCreate(osTimer(APP_POST_CHOPPING_TIMER), osTimerOnce, NULL);
        ASSERT(app_post_chopping_timer_id, "os timer is not enough!");
    }

    isInPostChoppingPeriod = true;
    TRACE(0, "%s", __func__);
    a2dp_audio_set_freq_user_case(A2DP_AUDIO_BOOST_MODE_FREQ);
    osTimerStart(app_post_chopping_timer_id, 30000);
}

bool a2dp_audio_chopping_is_in_post(void)
{
    return isInPostChoppingPeriod;
}

bool a2dp_audio_retrigger_is_on_process(void)
{
    return a2dp_audio_context.retrigger_onprocess;
}

void a2dp_audio_retrigger_set_on_process(bool flag)
{
    a2dp_audio_status_mutex_lock();
    a2dp_audio_context.retrigger_onprocess = flag;
    a2dp_audio_status_mutex_unlock();
}

bool a2dp_audio_is_underflow_in_processing(void)
{
    return a2dp_audio_context.underflow_inprocess;
}

uint32_t a2dp_audio_list_length_TWP(void)
{
    list_t *list = a2dp_audio_context.audio_datapath.input_raw_packet_list;
    if(list == NULL)
    {
        return 0;
    }
    return  a2dp_audio_list_length(list);
}

void a2dp_audio_update_mtu_per_frame(uint32_t frameCnt)
{
    a2dp_audio_context.output_cfg.mtu_per_frame = frameCnt;
}

