/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.3 */

#ifndef PB_GOOGLE_ASSISTANT_DEVICES_HEADPHONE_V1_CURRENT_DEVICE_STATE_PB_H_INCLUDED
#define PB_GOOGLE_ASSISTANT_DEVICES_HEADPHONE_V1_CURRENT_DEVICE_STATE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _google_assistant_devices_headphone_v1_BatteryDetails_DeviceType {
    google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_DEVICE_TYPE_UNSPECIFIED = 0,
    google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_LEFT_EAR_BUD = 1,
    google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_RIGHT_EAR_BUD = 2,
    google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_EAR_BUD_CASE = 3,
    google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_SINGLE_BATTERY_DEVICE = 4
} google_assistant_devices_headphone_v1_BatteryDetails_DeviceType;

/* Struct definitions */
typedef struct _google_assistant_devices_headphone_v1_BatteryDetails {
    int32_t remaining_battery_percentage;
    bool connected;
    google_assistant_devices_headphone_v1_BatteryDetails_DeviceType device_type;
} google_assistant_devices_headphone_v1_BatteryDetails;

typedef struct _google_assistant_devices_headphone_v1_CurrentDeviceStatus {
    pb_size_t battery_details_count;
    google_assistant_devices_headphone_v1_BatteryDetails battery_details[10];
    int32_t noise_cancellation_level;
    int32_t ambient_mode_level;
} google_assistant_devices_headphone_v1_CurrentDeviceStatus;


/* Helper constants for enums */
#define _google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_MIN google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_DEVICE_TYPE_UNSPECIFIED
#define _google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_MAX google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_SINGLE_BATTERY_DEVICE
#define _google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_ARRAYSIZE ((google_assistant_devices_headphone_v1_BatteryDetails_DeviceType)(google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_SINGLE_BATTERY_DEVICE+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_init_default {0, {google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default, google_assistant_devices_headphone_v1_BatteryDetails_init_default}, 0, 0}
#define google_assistant_devices_headphone_v1_BatteryDetails_init_default {0, 0, _google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_MIN}
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_init_zero {0, {google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero, google_assistant_devices_headphone_v1_BatteryDetails_init_zero}, 0, 0}
#define google_assistant_devices_headphone_v1_BatteryDetails_init_zero {0, 0, _google_assistant_devices_headphone_v1_BatteryDetails_DeviceType_MIN}

/* Field tags (for use in manual encoding/decoding) */
#define google_assistant_devices_headphone_v1_BatteryDetails_remaining_battery_percentage_tag 1
#define google_assistant_devices_headphone_v1_BatteryDetails_connected_tag 2
#define google_assistant_devices_headphone_v1_BatteryDetails_device_type_tag 3
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_battery_details_tag 1
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_noise_cancellation_level_tag 2
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_ambient_mode_level_tag 3

/* Struct field encoding specification for nanopb */
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_FIELDLIST(X, a) \
X(a, STATIC,   REPEATED, MESSAGE,  battery_details,   1) \
X(a, STATIC,   SINGULAR, INT32,    noise_cancellation_level,   2) \
X(a, STATIC,   SINGULAR, INT32,    ambient_mode_level,   3)
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_CALLBACK NULL
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_DEFAULT NULL
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_battery_details_MSGTYPE google_assistant_devices_headphone_v1_BatteryDetails

#define google_assistant_devices_headphone_v1_BatteryDetails_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    remaining_battery_percentage,   1) \
X(a, STATIC,   SINGULAR, BOOL,     connected,         2) \
X(a, STATIC,   SINGULAR, UENUM,    device_type,       3)
#define google_assistant_devices_headphone_v1_BatteryDetails_CALLBACK NULL
#define google_assistant_devices_headphone_v1_BatteryDetails_DEFAULT NULL

extern const pb_msgdesc_t google_assistant_devices_headphone_v1_CurrentDeviceStatus_msg;
extern const pb_msgdesc_t google_assistant_devices_headphone_v1_BatteryDetails_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_fields &google_assistant_devices_headphone_v1_CurrentDeviceStatus_msg
#define google_assistant_devices_headphone_v1_BatteryDetails_fields &google_assistant_devices_headphone_v1_BatteryDetails_msg

/* Maximum encoded size of messages (where known) */
#define google_assistant_devices_headphone_v1_CurrentDeviceStatus_size 192
#define google_assistant_devices_headphone_v1_BatteryDetails_size 15

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif