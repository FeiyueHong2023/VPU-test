/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6 */

#ifndef PB_FILES_CALLING_PB_H_INCLUDED
#define PB_FILES_CALLING_PB_H_INCLUDED
#include <pb_0_4_6.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _ForwardATCommand { 
    char command[64];
} ForwardATCommand;

typedef struct _IncomingCall { 
    char caller_number[34];
    char caller_name[128];
} IncomingCall;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define ForwardATCommand_init_default            {""}
#define IncomingCall_init_default                {"", ""}
#define ForwardATCommand_init_zero               {""}
#define IncomingCall_init_zero                   {"", ""}

/* Field tags (for use in manual encoding/decoding) */
#define ForwardATCommand_command_tag             1
#define IncomingCall_caller_number_tag           1
#define IncomingCall_caller_name_tag             2

/* Struct field encoding specification for nanopb */
#define ForwardATCommand_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   command,           1)
#define ForwardATCommand_CALLBACK NULL
#define ForwardATCommand_DEFAULT NULL

#define IncomingCall_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   caller_number,     1) \
X(a, STATIC,   SINGULAR, STRING,   caller_name,       2)
#define IncomingCall_CALLBACK NULL
#define IncomingCall_DEFAULT NULL

extern const pb_msgdesc_t ForwardATCommand_msg;
extern const pb_msgdesc_t IncomingCall_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define ForwardATCommand_fields &ForwardATCommand_msg
#define IncomingCall_fields &IncomingCall_msg

/* Maximum encoded size of messages (where known) */
#define ForwardATCommand_size                    65
#define IncomingCall_size                        165

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
