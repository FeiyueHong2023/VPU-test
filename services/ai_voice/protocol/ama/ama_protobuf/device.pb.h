/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6 */

#ifndef PB_DEVICE_PB_H_INCLUDED
#define PB_DEVICE_PB_H_INCLUDED
#include <pb_0_4_6.h>
#include "common.pb.h"
#include "speech.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _ConnectionStatus { 
    ConnectionStatus_CONNECTION_STATUS_UNKNOWN = 0, 
    ConnectionStatus_CONNECTION_STATUS_CONNECTED = 1, 
    ConnectionStatus_CONNECTION_STATUS_DISCONNECTED = 2 
} ConnectionStatus;

typedef enum _DevicePresence { 
    DevicePresence_DEVICE_PRESENCE_UNKNOWN = 0, 
    DevicePresence_DEVICE_PRESENCE_ACTIVE = 1, 
    DevicePresence_DEVICE_PRESENCE_INACTIVE = 2, 
    DevicePresence_DEVICE_PRESENCE_ACCESSIBLE = 3 
} DevicePresence;

typedef enum _DeviceBattery_Status { 
    DeviceBattery_Status_UNKNOWN = 0, 
    DeviceBattery_Status_CHARGING = 1, 
    DeviceBattery_Status_DISCHARGING = 2, 
    DeviceBattery_Status_FULL = 3 
} DeviceBattery_Status;

/* Struct definitions */
typedef struct _DeviceInformation_MetadataEntry { 
    pb_callback_t key;
    pb_callback_t value;
} DeviceInformation_MetadataEntry;

typedef struct _GetDeviceConfiguration { 
    char dummy_field;
} GetDeviceConfiguration;

typedef struct _GetDeviceFeatures { 
    char dummy_field;
} GetDeviceFeatures;

typedef struct _StartSetup { 
    char dummy_field;
} StartSetup;

typedef struct _CompleteSetup { 
    ErrorCode error_code;
} CompleteSetup;

typedef struct _DeviceBattery { 
    uint32_t level;
    uint32_t scale;
    DeviceBattery_Status status;
} DeviceBattery;

typedef struct _DeviceConfiguration { 
    bool needs_assistant_override;
    bool needs_setup;
} DeviceConfiguration;

typedef struct _DeviceStatus { 
    ConnectionStatus link;
    ConnectionStatus nfmi;
    DevicePresence presence;
} DeviceStatus;

typedef struct _FeatureProperties { 
    uint32_t feature_id;
    uint32_t envelope_version;
} FeatureProperties;

typedef struct _GetDeviceInformation { 
    uint32_t device_id;
} GetDeviceInformation;

typedef struct _OverrideAssistant { 
    ErrorCode error_code;
} OverrideAssistant;

typedef struct _UpdateDeviceInformation { 
    pb_callback_t name;
    uint32_t device_id;
} UpdateDeviceInformation;

typedef struct _DeviceFeatures { 
    uint32_t features;
    uint32_t device_attributes;
    pb_size_t feature_properties_count;
    FeatureProperties feature_properties[8];
} DeviceFeatures;

typedef struct _DeviceInformation { 
    char serial_number[20];
    char name[20];
    pb_size_t supported_transports_count;
    Transport supported_transports[4];
    char device_type[20];
    uint32_t device_id;
    bool has_battery;
    DeviceBattery battery;
    bool has_status;
    DeviceStatus status;
    uint32_t product_color;
    pb_size_t associated_devices_count;
    uint32_t associated_devices[16];
    pb_size_t supported_speech_initiations_count;
    SpeechInitiationType supported_speech_initiations[5];
    pb_size_t supported_wakewords_count;
    char supported_wakewords[5][16];
    pb_callback_t metadata;
} DeviceInformation;

typedef struct _NotifyDeviceConfiguration { 
    bool has_device_configuration;
    DeviceConfiguration device_configuration;
} NotifyDeviceConfiguration;

typedef struct _NotifyDeviceInformation { 
    bool has_device_information;
    DeviceInformation device_information;
} NotifyDeviceInformation;


/* Helper constants for enums */
#define _ConnectionStatus_MIN ConnectionStatus_CONNECTION_STATUS_UNKNOWN
#define _ConnectionStatus_MAX ConnectionStatus_CONNECTION_STATUS_DISCONNECTED
#define _ConnectionStatus_ARRAYSIZE ((ConnectionStatus)(ConnectionStatus_CONNECTION_STATUS_DISCONNECTED+1))

#define _DevicePresence_MIN DevicePresence_DEVICE_PRESENCE_UNKNOWN
#define _DevicePresence_MAX DevicePresence_DEVICE_PRESENCE_ACCESSIBLE
#define _DevicePresence_ARRAYSIZE ((DevicePresence)(DevicePresence_DEVICE_PRESENCE_ACCESSIBLE+1))

#define _DeviceBattery_Status_MIN DeviceBattery_Status_UNKNOWN
#define _DeviceBattery_Status_MAX DeviceBattery_Status_FULL
#define _DeviceBattery_Status_ARRAYSIZE ((DeviceBattery_Status)(DeviceBattery_Status_FULL+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define DeviceBattery_init_default               {0, 0, _DeviceBattery_Status_MIN}
#define DeviceStatus_init_default                {_ConnectionStatus_MIN, _ConnectionStatus_MIN, _DevicePresence_MIN}
#define DeviceInformation_init_default           {"", "", 0, {_Transport_MIN, _Transport_MIN, _Transport_MIN, _Transport_MIN}, "", 0, false, DeviceBattery_init_default, false, DeviceStatus_init_default, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {_SpeechInitiationType_MIN, _SpeechInitiationType_MIN, _SpeechInitiationType_MIN, _SpeechInitiationType_MIN, _SpeechInitiationType_MIN}, 0, {"", "", "", "", ""}, {{NULL}, NULL}}
#define DeviceInformation_MetadataEntry_init_default {{{NULL}, NULL}, {{NULL}, NULL}}
#define GetDeviceInformation_init_default        {0}
#define DeviceConfiguration_init_default         {0, 0}
#define GetDeviceConfiguration_init_default      {0}
#define OverrideAssistant_init_default           {_ErrorCode_MIN}
#define StartSetup_init_default                  {0}
#define CompleteSetup_init_default               {_ErrorCode_MIN}
#define NotifyDeviceConfiguration_init_default   {false, DeviceConfiguration_init_default}
#define UpdateDeviceInformation_init_default     {{{NULL}, NULL}, 0}
#define NotifyDeviceInformation_init_default     {false, DeviceInformation_init_default}
#define FeatureProperties_init_default           {0, 0}
#define DeviceFeatures_init_default              {0, 0, 0, {FeatureProperties_init_default, FeatureProperties_init_default, FeatureProperties_init_default, FeatureProperties_init_default, FeatureProperties_init_default, FeatureProperties_init_default, FeatureProperties_init_default, FeatureProperties_init_default}}
#define GetDeviceFeatures_init_default           {0}
#define DeviceBattery_init_zero                  {0, 0, _DeviceBattery_Status_MIN}
#define DeviceStatus_init_zero                   {_ConnectionStatus_MIN, _ConnectionStatus_MIN, _DevicePresence_MIN}
#define DeviceInformation_init_zero              {"", "", 0, {_Transport_MIN, _Transport_MIN, _Transport_MIN, _Transport_MIN}, "", 0, false, DeviceBattery_init_zero, false, DeviceStatus_init_zero, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {_SpeechInitiationType_MIN, _SpeechInitiationType_MIN, _SpeechInitiationType_MIN, _SpeechInitiationType_MIN, _SpeechInitiationType_MIN}, 0, {"", "", "", "", ""}, {{NULL}, NULL}}
#define DeviceInformation_MetadataEntry_init_zero {{{NULL}, NULL}, {{NULL}, NULL}}
#define GetDeviceInformation_init_zero           {0}
#define DeviceConfiguration_init_zero            {0, 0}
#define GetDeviceConfiguration_init_zero         {0}
#define OverrideAssistant_init_zero              {_ErrorCode_MIN}
#define StartSetup_init_zero                     {0}
#define CompleteSetup_init_zero                  {_ErrorCode_MIN}
#define NotifyDeviceConfiguration_init_zero      {false, DeviceConfiguration_init_zero}
#define UpdateDeviceInformation_init_zero        {{{NULL}, NULL}, 0}
#define NotifyDeviceInformation_init_zero        {false, DeviceInformation_init_zero}
#define FeatureProperties_init_zero              {0, 0}
#define DeviceFeatures_init_zero                 {0, 0, 0, {FeatureProperties_init_zero, FeatureProperties_init_zero, FeatureProperties_init_zero, FeatureProperties_init_zero, FeatureProperties_init_zero, FeatureProperties_init_zero, FeatureProperties_init_zero, FeatureProperties_init_zero}}
#define GetDeviceFeatures_init_zero              {0}

/* Field tags (for use in manual encoding/decoding) */
#define DeviceInformation_MetadataEntry_key_tag  1
#define DeviceInformation_MetadataEntry_value_tag 2
#define CompleteSetup_error_code_tag             1
#define DeviceBattery_level_tag                  1
#define DeviceBattery_scale_tag                  2
#define DeviceBattery_status_tag                 3
#define DeviceConfiguration_needs_assistant_override_tag 1
#define DeviceConfiguration_needs_setup_tag      2
#define DeviceStatus_link_tag                    1
#define DeviceStatus_nfmi_tag                    2
#define DeviceStatus_presence_tag                3
#define FeatureProperties_feature_id_tag         1
#define FeatureProperties_envelope_version_tag   2
#define GetDeviceInformation_device_id_tag       1
#define OverrideAssistant_error_code_tag         1
#define UpdateDeviceInformation_name_tag         1
#define UpdateDeviceInformation_device_id_tag    2
#define DeviceFeatures_features_tag              1
#define DeviceFeatures_device_attributes_tag     2
#define DeviceFeatures_feature_properties_tag    3
#define DeviceInformation_serial_number_tag      1
#define DeviceInformation_name_tag               2
#define DeviceInformation_supported_transports_tag 3
#define DeviceInformation_device_type_tag        4
#define DeviceInformation_device_id_tag          5
#define DeviceInformation_battery_tag            6
#define DeviceInformation_status_tag             7
#define DeviceInformation_product_color_tag      8
#define DeviceInformation_associated_devices_tag 9
#define DeviceInformation_supported_speech_initiations_tag 10
#define DeviceInformation_supported_wakewords_tag 11
#define DeviceInformation_metadata_tag           12
#define NotifyDeviceConfiguration_device_configuration_tag 1
#define NotifyDeviceInformation_device_information_tag 1

/* Struct field encoding specification for nanopb */
#define DeviceBattery_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   level,             1) \
X(a, STATIC,   SINGULAR, UINT32,   scale,             2) \
X(a, STATIC,   SINGULAR, UENUM,    status,            3)
#define DeviceBattery_CALLBACK NULL
#define DeviceBattery_DEFAULT NULL

#define DeviceStatus_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    link,              1) \
X(a, STATIC,   SINGULAR, UENUM,    nfmi,              2) \
X(a, STATIC,   SINGULAR, UENUM,    presence,          3)
#define DeviceStatus_CALLBACK NULL
#define DeviceStatus_DEFAULT NULL

#define DeviceInformation_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   serial_number,     1) \
X(a, STATIC,   SINGULAR, STRING,   name,              2) \
X(a, STATIC,   REPEATED, UENUM,    supported_transports,   3) \
X(a, STATIC,   SINGULAR, STRING,   device_type,       4) \
X(a, STATIC,   SINGULAR, UINT32,   device_id,         5) \
X(a, STATIC,   OPTIONAL, MESSAGE,  battery,           6) \
X(a, STATIC,   OPTIONAL, MESSAGE,  status,            7) \
X(a, STATIC,   SINGULAR, UINT32,   product_color,     8) \
X(a, STATIC,   REPEATED, UINT32,   associated_devices,   9) \
X(a, STATIC,   REPEATED, UENUM,    supported_speech_initiations,  10) \
X(a, STATIC,   REPEATED, STRING,   supported_wakewords,  11) \
X(a, CALLBACK, REPEATED, MESSAGE,  metadata,         12)
#define DeviceInformation_CALLBACK pb_default_field_callback_0_4_6
#define DeviceInformation_DEFAULT NULL
#define DeviceInformation_battery_MSGTYPE DeviceBattery
#define DeviceInformation_status_MSGTYPE DeviceStatus
#define DeviceInformation_metadata_MSGTYPE DeviceInformation_MetadataEntry

#define DeviceInformation_MetadataEntry_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, STRING,   key,               1) \
X(a, CALLBACK, SINGULAR, STRING,   value,             2)
#define DeviceInformation_MetadataEntry_CALLBACK pb_default_field_callback_0_4_6
#define DeviceInformation_MetadataEntry_DEFAULT NULL

#define GetDeviceInformation_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   device_id,         1)
#define GetDeviceInformation_CALLBACK NULL
#define GetDeviceInformation_DEFAULT NULL

#define DeviceConfiguration_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     needs_assistant_override,   1) \
X(a, STATIC,   SINGULAR, BOOL,     needs_setup,       2)
#define DeviceConfiguration_CALLBACK NULL
#define DeviceConfiguration_DEFAULT NULL

#define GetDeviceConfiguration_FIELDLIST(X, a) \

#define GetDeviceConfiguration_CALLBACK NULL
#define GetDeviceConfiguration_DEFAULT NULL

#define OverrideAssistant_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    error_code,        1)
#define OverrideAssistant_CALLBACK NULL
#define OverrideAssistant_DEFAULT NULL

#define StartSetup_FIELDLIST(X, a) \

#define StartSetup_CALLBACK NULL
#define StartSetup_DEFAULT NULL

#define CompleteSetup_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    error_code,        1)
#define CompleteSetup_CALLBACK NULL
#define CompleteSetup_DEFAULT NULL

#define NotifyDeviceConfiguration_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  device_configuration,   1)
#define NotifyDeviceConfiguration_CALLBACK NULL
#define NotifyDeviceConfiguration_DEFAULT NULL
#define NotifyDeviceConfiguration_device_configuration_MSGTYPE DeviceConfiguration

#define UpdateDeviceInformation_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, STRING,   name,              1) \
X(a, STATIC,   SINGULAR, UINT32,   device_id,         2)
#define UpdateDeviceInformation_CALLBACK pb_default_field_callback_0_4_6
#define UpdateDeviceInformation_DEFAULT NULL

#define NotifyDeviceInformation_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  device_information,   1)
#define NotifyDeviceInformation_CALLBACK NULL
#define NotifyDeviceInformation_DEFAULT NULL
#define NotifyDeviceInformation_device_information_MSGTYPE DeviceInformation

#define FeatureProperties_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   feature_id,        1) \
X(a, STATIC,   SINGULAR, UINT32,   envelope_version,   2)
#define FeatureProperties_CALLBACK NULL
#define FeatureProperties_DEFAULT NULL

#define DeviceFeatures_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   features,          1) \
X(a, STATIC,   SINGULAR, UINT32,   device_attributes,   2) \
X(a, STATIC,   REPEATED, MESSAGE,  feature_properties,   3)
#define DeviceFeatures_CALLBACK NULL
#define DeviceFeatures_DEFAULT NULL
#define DeviceFeatures_feature_properties_MSGTYPE FeatureProperties

#define GetDeviceFeatures_FIELDLIST(X, a) \

#define GetDeviceFeatures_CALLBACK NULL
#define GetDeviceFeatures_DEFAULT NULL

extern const pb_msgdesc_t DeviceBattery_msg;
extern const pb_msgdesc_t DeviceStatus_msg;
extern const pb_msgdesc_t DeviceInformation_msg;
extern const pb_msgdesc_t DeviceInformation_MetadataEntry_msg;
extern const pb_msgdesc_t GetDeviceInformation_msg;
extern const pb_msgdesc_t DeviceConfiguration_msg;
extern const pb_msgdesc_t GetDeviceConfiguration_msg;
extern const pb_msgdesc_t OverrideAssistant_msg;
extern const pb_msgdesc_t StartSetup_msg;
extern const pb_msgdesc_t CompleteSetup_msg;
extern const pb_msgdesc_t NotifyDeviceConfiguration_msg;
extern const pb_msgdesc_t UpdateDeviceInformation_msg;
extern const pb_msgdesc_t NotifyDeviceInformation_msg;
extern const pb_msgdesc_t FeatureProperties_msg;
extern const pb_msgdesc_t DeviceFeatures_msg;
extern const pb_msgdesc_t GetDeviceFeatures_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define DeviceBattery_fields &DeviceBattery_msg
#define DeviceStatus_fields &DeviceStatus_msg
#define DeviceInformation_fields &DeviceInformation_msg
#define DeviceInformation_MetadataEntry_fields &DeviceInformation_MetadataEntry_msg
#define GetDeviceInformation_fields &GetDeviceInformation_msg
#define DeviceConfiguration_fields &DeviceConfiguration_msg
#define GetDeviceConfiguration_fields &GetDeviceConfiguration_msg
#define OverrideAssistant_fields &OverrideAssistant_msg
#define StartSetup_fields &StartSetup_msg
#define CompleteSetup_fields &CompleteSetup_msg
#define NotifyDeviceConfiguration_fields &NotifyDeviceConfiguration_msg
#define UpdateDeviceInformation_fields &UpdateDeviceInformation_msg
#define NotifyDeviceInformation_fields &NotifyDeviceInformation_msg
#define FeatureProperties_fields &FeatureProperties_msg
#define DeviceFeatures_fields &DeviceFeatures_msg
#define GetDeviceFeatures_fields &GetDeviceFeatures_msg

/* Maximum encoded size of messages (where known) */
/* DeviceInformation_size depends on runtime parameters */
/* DeviceInformation_MetadataEntry_size depends on runtime parameters */
/* UpdateDeviceInformation_size depends on runtime parameters */
/* NotifyDeviceInformation_size depends on runtime parameters */
#define CompleteSetup_size                       2
#define DeviceBattery_size                       14
#define DeviceConfiguration_size                 4
#define DeviceFeatures_size                      124
#define DeviceStatus_size                        6
#define FeatureProperties_size                   12
#define GetDeviceConfiguration_size              0
#define GetDeviceFeatures_size                   0
#define GetDeviceInformation_size                6
#define NotifyDeviceConfiguration_size           6
#define OverrideAssistant_size                   2
#define StartSetup_size                          0

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
