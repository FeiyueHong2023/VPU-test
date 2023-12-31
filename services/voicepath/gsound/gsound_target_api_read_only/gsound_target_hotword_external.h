// Copyright 2018 Google LLC.
// Libgsound version: bb3b118
#ifndef GSOUND_TARGET_HOTWORD_EXTERNAL_H
#define GSOUND_TARGET_HOTWORD_EXTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This header file was generated by Google GSound Services,
 * it should not be altered in any way. Please treat this as
 * a read-only file.
 *
 * Each function listed below must
 * be implemented for each specific
 * platform / SDK
 *
 */
#include "gsound.h"
#include "gsound_target_audio_config.h"
#include "gsound_target_hotword_common.h"

/*******************************************************************************
 * External hotword
 *
 * This file is only used by targets choosing to implement external hotword
 * detection, where Google hotword detection is handled by the target, not by
 * GSound.
 *
 * To support external hotword detection, call
 * GSoundServiceHotwordInitExternal. This will cause the external hotword
 * objects to be referenced and included at link time.
 *
 ******************************************************************************/

/**
 * This header defines the interface between GSound and the target for the
 * *external hotword* case, meaning:
 *    - Google hotword detection is performed by the target (e.g. on a separate
 *      core, DSP, or on the same core but handled by the target outside of
 *      GSound),
 *    - Target runs the Google hotword algorithm
 *    - Target notifies GSound when a hotword is detected.
 *    - GSound requests data from the target's hotword backlog buffer.
 *    - GSound indicates when the user is done speaking, and tells the target to
 *      stop flushing its hotword audio buffer, but continue performing hotword
 *      detection.
 *
 * NOTE: the target should not start hotword detection by default.
 * Hotword detection should only be enabled when
 * GSoundTargetHotwordStartDetection is called, and disabled every time
 * GSoundTargetHotwordStopDetection is called, and every power cycle.
 *
 * The following sequence diagram illustrates a normal hotword query:
 *
 *     [GSound]                                      [target]
 *         |                                            |
 *         | <--------------gsound_hotword_detected()<--|
 *         |                                            |
 *         | -->GSoundTargetHotwordRequestForData()---->|
 *         | <-----------------------------------data<--|
 *         |  ( "data" means:                         ) |
 *         |  (  * sync case: hw_data_msg_out and     ) |
 *         |  (        sync_out output parameters of  ) |
 *         |  (        RequestForData populated.      ) |
 *         |  (  * async case: gsound_on_hotword_data ) |
 *         |  (        called in the target's context ) |
 *         |  (        after RequestForData is called ) |
 *         |                                            |
 *         | -->GSoundTargetHotwordDataConsumed()------>|
 *         |                                            |
 *         |  (GSound has copied the data into its    ) |
 *         |  (TX buffer or transmitted it over BT/BLE) |
 *         |                                            |
 *         |                                            |
 *         | (Now, GSound has space available in its )  |
 * [tx   ] | (tx buffer, so it is ready to receive   )  |
 * [buff ] | (more hotword audio data                )  |
 * [avail] |                                            |
 *         | -->GSoundTargetHotwordRequestForData()---->|
 *         | <-----------------------------------data<--|
 *         | -->GSoundTargetHotwordDataConsumed()------>|
 *         |                                            |
 *         |                                            |
 *         | (more TX buffer space available, same   )  |
 * [tx   ] | (sequence                               )  |
 * [buff ] |                                            |
 * [avail] | -->GSoundTargetHotwordRequestForData()---->|
 *         | <-----------------------------------data<--|
 *         | -->GSoundTargetHotwordDataConsumed()------>|
 *         |                                            |
 *         |   ... repeats until target's hotword       |
 *         |             buffer is empty ...            |
 *         |                                            |
 *         |                                            |
 *         |                                            |
 *         | (When the hotword buffer is empty, the )   |
 *         | (target should still respond with the  )   |
 *         | (data message, but with data1 and data2)   |
 *         | (set to zero.                          )   |
 *         | (                                      )   |
 *         | (GSound will respond with              )   |
 *         | (GSoundTargetHotwordDataConsumed       )   |
 *         | (and args NULL/zero.                   )   |
 *         | (                                      )   |
 *         | (Then it is up to the target to        )   |
 *         | (call gsound_on_hotword_data_available )   |
 *         | (when additional audio is added to the )   |
 *         | (hotword buffer.                       )   |
 *         |                                            | [new audio]
 *         |                                            | [added to ]
 *         |                                            | [hw buff  ]
 *         | <-----gsound_on_hotword_data_available()<--|
 *         |                                            |
 *         | -->GSoundTargetHotwordRequestForData()---->|
 *         | <-----------------------------------data<--|
 *         | -->GSoundTargetHotwordDataConsumed()------>|
 *         |                                            |
 *         |                                            |
 *         |                                            |
 *         |                                            | [new audio]
 *         |                                            | [added to ]
 *         |                                            | [hw buff  ]
 *         | <-----gsound_on_hotword_data_available()<--|
 *         |                                            |
 *         | -->GSoundTargetHotwordRequestForData()---->|
 *         | <-----------------------------------data<--|
 *         | -->GSoundTargetHotwordDataConsumed()------>|
 *         |                                            |
 *         |   ... repeats until GSound calls           |
 *         |      GSoundTargetHotwordStopStream ...     |
 *         |                                            |
 *         |                                            |
 *         |                                            |
 *         | (GSound receives a message from the      ) |
 *         | (assistant app on the phone indicating   ) |
 *         | (that the user is done performing the    ) |
 *         | (query.                                  ) |
 * [query] |                                            |
 * [done ] |                                            |
 *         | ---->GSoundTargetHotwordStopStream()------>|
 *         |                                            |
 *
 **/

/**
 * Maximum SBC frame size for the hotword backlog
 * Bitpool 24, blocks 16 has a headerless frame size of 52 bytes per 128.
 */
#define GSOUND_TARGET_MAX_SBC_FRAME_LEN (52)

/**
 * Maximum length of the hotword buffer, in time (milliseconds)
 */
#define GSOUND_TARGET_MAX_HOTWORD_BUF_LEN_MS (2000)

/**
 * This structure is used to:
 *
 *  - send up to two chunks of contiguous audio data from the target's
 *    hotword buffer, either synchronously (as an output parameter of
 *    GSoundTargetHotwordRequestForData), or asynchronously (by calling
 *    gsound_on_hotword_data, in response to a
 *    GSoundTargetHotwordRequestForData), and
 *
 *  - for GSound to tell the target how much data was consumed via
 *    GSoundTargetHotwordDataConsumed. In this case, num_bytes1 and
 *    num_bytes2 indicate how much of data1 and data2 were
 *    consumed. data1 will always be entirely consumed before data2 is
 *    consumed.
 */
typedef struct {
  /* Pointer to first hotword audio data available in buffer.
   * Set to NULL if no data is available
   */
  const uint8_t *data1;

  /* Number of bytes pointed to by data1.
   * Set to zero if no data is available
   */
  uint16_t num_bytes1;

  /* Pointer to any additional hotword data available that is not contiguous
   * with data1. (e.g. if the target's hotword audio is stored in a circular
   * buffer, data2 can be used to send two separate chunks, for the
   * wraparound case)
   *
   * Only valid if data1 is provided (num_bytes1 > 0).
   * Only needed as an optimization to avoid two context switches to flush two
   * sides of a circular buffer.
   */
  const uint8_t *data2;

  /* Number of bytes pointed to by data2.
   * Must be zero if num_bytes1 is zero.
   */
  uint16_t num_bytes2;

  /* Number of bytes per audio frame. num_bytes1 and num_bytes2 must be a
   * multiple of this. GSound will only consume integer multiples of
   * `bytes_per_frame` from `data1` and `data2`.
   * (e.g. this is the number of bytes per headerless SBC frame, and SBC frame
   * will never be partially consumed)
   */
  uint8_t bytes_per_frame;
} GSoundTargetHotwordDataMsg;

/*******************************************************************************
 * ...
 *
 ******************************************************************************/
/******************************************************************************
 * Preloaded Models
 *****************************************************************************/

/**
 * This struct contains pointers to libgsound buffers that the Target should
 * populate. Target must null-terminate each string in the structure below
 */
typedef struct {
  /**
   * Version of the hotword library as reported by GoogleHotwordVersion. This
   * should be filled in as a null-terminated decimal integer string. ex:
   * "2549082"
   */
  char *hotword_library_version;

  /**
   * Null-terminated string for the architecture of the hotword library. Contact
   * Google to get the architecture ID for your DSP.
   */
  char *hotword_library_architecture;

  /**
   * Contains a list of models present on the device. The current active model
   * should be included. The format is a single null terminated string of model
   * IDs separated by new lines.  "<model_id 1>\n<model_id 2>\n...<model_id N>".
   */
  char *supported_models;

  /**
   * A null-terminated string of the model ID of the currently active model. If
   * no model is active, this should be set to an empty string. Note: it is
   * expected that Target initialize their active_model to an empty string on
   * bootup.
   */
  char *active_model;

  /*
   * Lengths of the above buffers in bytes. These are set by libgsound.
   */
  uint8_t version_length;
  uint8_t arch_length;
  uint8_t supported_model_length;
  uint8_t active_model_length;
} GSoundHotwordLibInfo;

typedef struct {
  /**
   * Called when 1st stage hotword is detected.
   * Any new audio frames generated after hotword detection must be appended
   * to the backlog and flushed serially.
   */
  GSoundStatus (*gsound_on_hotword_detected)(uint16_t sbc_bitpool);

  /**
   *
   *********************************************************
   * Synchronous case
   *********************************************************
   *
   * If the target can synchronously pass its hotword audio data buffer contents
   * through the output parameter of GSoundTargetHotwordRequestForData, then
   * the `gsound_on_hotword_data` callback should *not* be used.
   *
   *********************************************************
   * Asynchronous case
   *********************************************************
   * If the target flushes its hotword audio data asynchronously, this API is
   * called to send GSound hotword audio data from backlog, in response to a
   * GSoundTargetHotwordRequestForData. Must be called exactly once per call of
   * GSoundTargetHotwordRequestForData except the case noted below.
   *
   * If no data is available, the target must still call this with data and
   * num_bytes set to NULL and 0 respectively.
   *
   * Note that, when the target is sending hotword audio data from the backlog
   * asynchronously, there can be more than one outstanding
   * GSoundTargetHotwordRequestForData in flight at the same time.  If the
   * target gets a second request for data before it responds to the first one
   * then the target should only respond to the second one.
   *
   * `data1` must be the audio at the head of the hotword buffer, i.e. the audio
   * occurring the earliest in time.
   * `data2` is optional, and if provided must be audio that occurred later in
   * time than the end of `data1`.
   * `data2` only needs to be set if the target has two separate contiguous
   * chunks of hotword data, and wishes to provide both so that GSound may
   * consume data2 if it was able to consume all of data1.
   * e.g. if the hotword is buffered in a circular buffer, data1 can be the
   * read pointer and data2 can be the wrapped around pointer to the beginning
   * of the circular buffer.
   *
   * Giving zero data1 with non zero data2 is an error.
   *
   * Target must reserve data content at both `data1` and `data2` until GSound
   * has called GSoundTargetHotwordDataConsumed.
   *
   * param[in] hw_data_msg:   see definition of this structure for more info.
   *
   * return: GSOUND_STATUS_OK if successful
   */
  GSoundStatus (*gsound_on_hotword_data)(
      const GSoundTargetHotwordDataMsg *hw_data_msg);

  /**
   * After hotword is detected, target must call this each time audio data is
   * added to the hotword backlog.
   *
   * Must not be called between GSoundTargetHotwordRequestForData and
   * gsound_on_hotword_data async response.
   *
   * param[in] bytes_available: audio data available in buffer. It is expected
   *                            that only whole SBC frames are buffered, so
   *                            `bytes_available` should be an integer multiple
   *                            of the SBC frame size.
   */
  GSoundStatus (*gsound_on_hotword_data_available)(uint32_t bytes_available);

  /**
   * Target must call this after activating the hotword model asynchronously.
   *
   * If the Target sets ready_now_out to false in
   * GSoundTargetHotwordModelActivate, the activation is considered async and
   * Target must call this function once activated.
   *
   * param[in] success: whether the model activation was successful.
   */
  GSoundStatus (*gsound_on_hotword_model_activated)(bool success);

} GSoundHotwordInterface;

/*******************************************************************************
 * ...
 *
 ******************************************************************************/
typedef struct {
  /**
   * Size of hotword audio backlog in miliseconds.
   * Will be a maximum of `GSOUND_TARGET_MAX_HOTWORD_BUF_LEN_MS` milliseconds.
   * Round up to the nearest integer frame (e.g. SBC) when converting this to
   * buffer size.
   */
  uint32_t audio_backlog_ms;

} GSoundTargetHotwordConfig;

/*******************************************************************************
 *
 * Target APIs
 *
 ******************************************************************************/

/**
 * Start hotword processing. The target must buffer all audio (raw, or SBC
 * encoded without SBC header) in a rolling buffer storing exactly
 * config->audio_backlog_frames.
 *
 * NOTE: must store exactly `config->audio_backlog_frames` SBC frames of data.
 * - Storing fewer frames will cause hotword detection to fail due to critical
 *   portions of the hotword being lost (including silence, used for noise
 *   removal)
 * - More than this will cause issues because the phone is only waiting to
 *   receive `config->audio_backlog_frames`. If it doesn't detect a hotword
 *   within those frames, it will stop the query immediately without alerting
 *   the user (2nd stage hotword detection failed).
 *
 * Note if this API is called while hotword detection is already in progress,
 * target must restart hotword detection and use updated configuration.
 *
 */
GSoundStatus GSoundTargetHotwordStartDetection(
    const GSoundTargetAudioInSettings *audio_config,
    const GSoundTargetHotwordConfig *hw_config);

/**
 * GSound calls this when it has buffer space available to consume additional
 * audio frames from Target. Typically, this occurs when bluetooth/BLE TX
 * buffer space becomes available, or after the target gives GSound data that
 * it is able to consume entirely.
 *
 * The target should provide pointers to encoded
 * hotword audio, matching the `config` parameters in
 * GSoundTargetHotwordStartDetection.
 *
 * If the target can synchronously access its hotword buffer from within this
 * function call:
 *     the target must populate `sync_hw_data_out` and set `ready_now_out` to
 *     true. (NOTE: do not call the gsound_on_hotword_data callback in this
 *     case)
 * Otherwise, for an asynchronous response:
 *     the target must call the gsound_on_hotword_data callback outside of this
 *     function call, and set `ready_now_out` to false. Note that, if the target
 *     performs asynchronous response, there can be more than one outstanding
 *     GSoundTargetHotwordRequestForData in flight at the same time. If the
 *     target gets a second request for data before it responds to the first one
 *     then the target should only respond to the second one.
 *
 * In both the synchronous or asynchronous cases above, if the hotword buffer is
 * empty, the target must still respond, but with data1 and data2 set to NULL,
 * and num_bytes1 and num_bytes2 set to zero.
 *
 * Target must send an integer number of frames to GSound. The frame size will
 * match the configuration parameters sent by GSoundTargetHotwordStartDetection,
 * but `bytes_per_frame` is provided here for sanity checking.
 * GSound will not consume a non integer multiple of `bytes_per_frame`.
 *
 * GSound will consume no more than `max_num_bytes`. Providing more than this is
 * okay, but any data not consumed must be provided in the next call of
 * GSoundTargetHotwordRequestForData
 *
 * param[in]  max_num_bytes:    the maxinum number of bytes that GSound is able
 *                              to consume
 *
 * param[in]  bytes_per_frame:  number of bytes per frame that GSound expects.
 *
 * param[out] sync_hw_data_out: populate this this structure only if the target
 *                              can synchronously retrieve hotword data
 *                              from its buffer (and set ready_now_out to true).
 *
 * param[out] ready_now_out:    set to true if sync_hw_data_out is populated.
 *
 */
GSoundStatus GSoundTargetHotwordRequestForData(
    uint32_t max_num_bytes, uint32_t bytes_per_frame,
    GSoundTargetHotwordDataMsg *sync_hw_data_out, bool *ready_now_out);

/**
 * Stop sending hotword audio data-- but continue running hotword detection.
 * The hotword audio buffer should be cleared.
 *
 * This indicates the completion of a hotword query session, whether
 * it was successful or not.
 */
GSoundStatus GSoundTargetHotwordStopStream(void);

/**
 * Stop running hotword detection entirely, and
 * stop sending hotword data.
 *
 * Any data in the backlog should be flushed. Target should no longer send
 * backlog data
 *
 * If the `keep_voice_ready` parameter is true, GSound is switching from a
 * hotword query to a non hotword query (e.g. push to talk, "remote query"
 * (follow up query where the phone initiates it)). The target is free to keep
 * their SBC encoding and audio path active, knowing that GSound will soon be
 * calling GSoundTargetAudioInOpen.
 *
 * This API may be called when Hotword detection is not running. In this case
 * the target must do nothing and return GSOUND_STATUS_OK
 *
 * Note: Target should not alter their active model when this API is called.
 *
 * param[in] keep_voice_ready     true  if we are interrupting hotword with a
 *                                      different kind of query (e.g. push to
 *                                      talk, remote query).
 *
 *                                false if we are not, hotword is simply being
 *                                      disabled
 */
GSoundStatus GSoundTargetHotwordStopDetection(bool keep_voice_ready);

/**
 * GSoundTargetHotwordReset means the target should reset their audio buffer,
 * and update their configuration if necessary.
 * This is similar to a stop and start, but it is not necessary to rebuild the
 * audio path if there are no configuration parameter changes.
 */
GSoundStatus GSoundTargetHotwordReset(
    const GSoundTargetAudioInSettings *audio_config,
    const GSoundTargetHotwordConfig *hw_config);

/**
 * GSound has consumed some of the data sent by gsound_on_hotword_data, target
 * can free it from their buffer.
 *
 * `data1` and `data2` in this API will always be equal to what the target last
 * provided in gsound_on_hotword_data (async case) or
 * GSoundTargetHotwordRequestForDat (sync case), it is provided for convenience
 * of the target.
 *
 * `num_bytes1` and `num_bytes2` corresponds to the number of bytes consumed by
 * GSound from data1 and data2 sent in `gsound_on_hotword_data`.
 * GSound will only consume integer multiples of `bytes_per_frame`.
 * `data1` is consumed first. `data2` is only looked at if all of data1 is
 * consumed.
 *
 * Any data not consumed must be sent again on the next gsound_on_hotword_data
 *
 * param[in] data1:      pointer to data that has been consumed. Only provided
 *                       for convenience, should always be the same as what was
 *                       given to GSound as `data` in gsound_on_hotword_data or
 *                       synchronously through GSoundTargetHotwordRequestForData
 *
 * param[in] num_bytes1: the number of bytes that have been consumed starting at
 *                       `data1`
 *
 * param[in] data2:      pointer to data that has been consumed. Only provided
 *                       for convenience, should always be the same as what was
 *                       given to GSound as `data2` in gsound_on_hotword_data or
 *                       synchronously through GSoundTargetHotwordRequestForData
 *
 * param[in] num_bytes2: the number of bytes that have been consumed starting at
 *                       `data2`. Will only ever be greater than zero if all of
 *                       `data1` has been consumed.
 *
 * param[in] bytes_per_frame  the number of bytes per frame, only provided here
 *                            for sanity.
 */
GSoundStatus GSoundTargetHotwordDataConsumed(
    GSoundTargetHotwordDataMsg *hw_data);

/**
 * Get information about the hotword library and models on the device.
 *
 * Return:
 *  GSOUND_STATUS_ERROR if copying fails (e.g. buffers are too small)
 *  GSOUND_STATUS_OK on success
 *
 */
GSoundStatus GSoundTargetGetHotwordLibInfo(
    const GSoundHotwordLibInfo *const info_out);

/**
 * Activate the given model by loading the mmaps and initializing the hotword
 * library. Target has the option to activate the model asynchronously or
 * synchronously using the ready_now_out output argument.
 *
 * Note: Target must fully consume the model id input string before returning.
 *
 * Note: Clearing the active model (model_id is NULL) must be done
 *       synchronously (i.e. ready_now_out must be set to true).
 *
 * param[in] model_id: String ID of the desired model. String may not be NULL
 * terminated. If model_id is NULL, Target must clear out its active model
 * disabling hotword until a valid model is provided.
 *
 * param[in] model_length: Number of characters in the model_id. Expect 0 here
 * if model_id is NULL.
 *
 * param[out] ready_now_out: Whether the model was activated synchronously or
 * asynchronously. If model activation blocks the Libgsound thread for a
 * significant amount of time, set ready_now_out to false, activate the model
 * asynchronously, and all gsound_on_hotword_model_activated once complete. Must
 * never be false when model_id is NULL.
 *
 * return: GSOUND_STATUS_OK on success. Any other return type will be considered
 *         an error. In case of error, Target must clear out its active model.
 *
 ******************************************************************************
 * This is a representation of the model stored in flash. It consists of a
 * header followed by a byte array for each mmap. The header specifies any
 * padding between the mmaps.
 *
 * hw_model { GSoundHotwordModelHeader model_header;
 *   uint8 model_data[];
 * }
 *
 * This is a representation of the header stored in flash. Assume no padding in
 * the structure. All fields are 32-bits stored in big-endian format.
 * This structure will be at the beginning of a hotword model OTA'd file. Use it
 * to parse and load the models on GSoundTargetHotwordModelActivate
 * GSoundHotwordModelHeader {
 *   // Bytes 0 - 3
 *   // Version of the format for this header. This format is version 0
 *   header_version
 *
 *   // Bytes 4 - 7
 *   // Version of the compatible hotword library. Note: int not uint
 *   library_version
 *
 *   // Bytes 8 - 11
 *   // Length of the architecture string
 *   architecture_length
 *
 *   // Bytes 12 - 27
 *   // Architecture string. Only the first architecture_length bytes are valid
 *   (char array)architecture
 *
 *   // Bytes 28 - 31
 *   // Count of valid mmap_headers
 *   mmap_count
 *
 *   // Array of headers for parsing the mmaps for a model. The mmap data must
 *   // be passed to GoogleHotwordDspMultiBankInit() in memory_banks in the same
 *   // order that they are stored here.
 *   GSoundHotwordMmapHeader mmap_headers[mmap_count]
 * }
 *
 * GSoundHotwordMmap Header {
 *   // Bytes N - N+3
 *   // RO/RW/RW uninitialized memory. This is 32-bits big endian and is a
 *   // member of GSoundHotwordMmapType defined in
 *   // gsound_target_hotword_common.h
 *   mmap_type
 *
 *   // Bytes N+4 - N+7
 *   // Bytes from the beginning of the model blob to start of mmap section.
 *   // (includes header)
 *   offset_bytes
 *
 *   // Bytes N+8 - N+11
 *   // Number of bytes this memory occupies. For the TEXT and DATA types this
 *   // length indicates the size of initialized data. For the BSS type this
 *   // length indicates the memory that must be allocated.
 *   length_bytes
 * }
 * ***********************************************************************
 */
GSoundStatus GSoundTargetHotwordModelActivate(const char *model_id,
                                              uint8_t model_length,
                                              bool *ready_now_out);

/**
 * Called at boot-up. GSound provide callbacks which Target must execute
 *
 * param[in] handlers: callbacks which target should store and call as
 *                     documented in the definition of GSoundHotwordInterface
 *                     above
 *
 * return: GSOUND_STATUS_OK for success, otherwise libgsound will assert
 */
GSoundStatus GSoundTargetHotwordInit(const GSoundHotwordInterface *handlers);

#ifdef __cplusplus
}
#endif

#endif  // GSOUND_TARGET_HOTWORD_EXTERNAL_H
