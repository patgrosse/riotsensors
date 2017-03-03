/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Packet definitions for communication between RIOT OS and Linux
 * @file    rs_packets.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_PACKETS_H
#define RIOTSENSORS_PACKETS_H

#include <stdint.h>

#include <rs_constants.h>

/*
 * Packet identifier
 */

/**
 * @brief A lambda has been registered
 */
#define RS_PACKET_REGISTERED 1
/**
 * @brief A lambda has been unregistered
 */
#define RS_PACKET_UNREGISTERED 2
/**
 * @brief Call a lambda by it's ID
 */
#define RS_PACKET_CALL_BY_ID 3
/**
 * @brief Call a lambda by it's name
 */
#define RS_PACKET_CALL_BY_NAME 4
/**
 * @brief Lambda call produced an error
 */
#define RS_PACKET_RESULT_ERROR 5
/**
 * @brief Integer lambda has been evaluated successfully
 */
#define RS_PACKET_RESULT_INT 6
/**
 * @brief Double lambda has been evaluated successfully
 */
#define RS_PACKET_RESULT_DOUBLE 7
/**
 * @brief String lambda has been evaluated successfully
 */
#define RS_PACKET_RESULT_STRING 8

/**
 * @brief Identifier of a packet type (RS_PACKET_* constants)
 */
typedef uint8_t rs_packet_type_t;

/**
 * Get a human readable string for a packet type
 * @param c Packet type
 * @return A string or NULL
 */
const char *stringify_rs_packet_type_t(rs_packet_type_t c);

/*
 * Type identifier
 */

/**
 * @brief Identifies an integer lambda
 */
#define RS_LAMBDA_INT 1
/**
 * @brief Identifies a double lambda
 */
#define RS_LAMBDA_DOUBLE 2
/**
 * @brief Identifies a string lambda
 */
#define RS_LAMBDA_STRING 3

/**
 * @brief Identifier of a lambda (return) type (RS_LAMBDA_* constants)
 */
typedef uint8_t rs_lambda_type_t;

/**
 * Get a human readable string for a lambda type
 * @param c Lambda type
 * @return A string or NULL
 */
const char *stringify_rs_lambda_type_t(rs_lambda_type_t c);

/*
 * Cache policies
 */

/**
 * @brief Never retrieve a value from cache
 */
#define RS_CACHE_NO_CACHE 1
/**
 * @brief Call the lambda once and retrieve the value from now on from cache
 */
#define RS_CACHE_CALL_ONCE 2
/**
 * @brief Never call the lambda manually always retrieve from cache
 */
#define RS_CACHE_ONLY 3
/**
 * @brief Use the cache if a timeout occurs
 */
#define RS_CACHE_ON_TIMEOUT 4

/**
 * @brief Identifier of a lambda cache policy (RS_CACHE_* constants)
 */
typedef uint8_t rs_cache_type_t;

/**
 * Get a human readable string for a cache type
 * @param c Cache type
 * @return A string or NULL
 */
const char *stringify_rs_cache_type_t(rs_cache_type_t c);

/*
 * Internal types
 */

/**
 * @brief Lambda identifier
 */
typedef uint8_t lambda_id_t;
/**
 * @brief Used integer type
 */
typedef int32_t rs_int_t;
/**
 * @brief Used double type
 */
typedef double rs_double_t;
/**
 * @brief Used string type
 */
typedef char *rs_string_t;


#ifndef __packed
#define __packed __attribute__((packed))
#endif

/*
 * Packet definitions from RIOT to Linux
 */

/**
 * @brief riotsensors base packet
 */
typedef struct __packed {
    rs_packet_type_t ptype;
} rs_packet_base_t;

/**
 * @brief riotsensors packet when a lambda gets registered
 */
typedef struct __packed {
    rs_packet_base_t base;
    char name[MAX_LAMBDA_NAME_LENGTH];
    rs_lambda_type_t ltype;
    rs_cache_type_t cache;
} rs_packet_registered_t;

/**
 * @brief riotsensors packet when a lambda gets unregistered
 */
typedef struct __packed {
    rs_packet_base_t base;
    lambda_id_t lambda_id;
    char name[MAX_LAMBDA_NAME_LENGTH];
} rs_packet_unregistered_t;

/**
 * @brief riotsensors base packet for lambda call results
 */
typedef struct __packed {
    rs_packet_base_t base;
    lambda_id_t lambda_id;
    char name[MAX_LAMBDA_NAME_LENGTH];
} rs_packet_lambda_result_t;

/**
 * @brief riotsensors packet for errors occurred in calls
 */
typedef struct __packed {
    rs_packet_lambda_result_t result_base;
    int8_t error_code;
} rs_packet_lambda_result_error_t;

/**
 * @brief riotsensors packet for integer results
 */
typedef struct __packed {
    rs_packet_lambda_result_t result_base;
    rs_int_t result;
} rs_packet_lambda_result_int_t;

/**
 * @brief riotsensors packet for double results
 */
typedef struct __packed {
    rs_packet_lambda_result_t result_base;
    rs_double_t result;
} rs_packet_lambda_result_double_t;

/**
 * @brief riotsensors packet for string results
 *
 * Warning: sizeof() must not be used because the result is mostly longer that one char
 */
typedef struct __packed {
    rs_packet_lambda_result_t result_base;
    uint16_t result_length;
    char result;
} rs_packet_lambda_result_string_t;

/*
 * Packet definitions from Linux to RIOT
 */

/**
 * @brief riotsensors packet to call a lambda by it's id
 */
typedef struct __packed {
    rs_packet_base_t base;
    lambda_id_t lambda_id;
    rs_lambda_type_t expected_type;
} rs_packet_call_by_id_t;

/**
 * @brief riotsensors packet to call a lambda by it's name
 */
typedef struct __packed {
    rs_packet_base_t base;
    char name[MAX_LAMBDA_NAME_LENGTH];
    rs_lambda_type_t expected_type;
} rs_packet_call_by_name_t;

/*
 * network operations - hton
 * most operations are just for legacy reasons and do not do anything at the moment
 */

/**
 * @brief convert a rs_packet_base_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_base_t(rs_packet_base_t *pkt);

/**
 * @brief convert a rs_packet_registered_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_registered_t(rs_packet_registered_t *pkt);

/**
 * @brief convert a rs_packet_unregistered_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_unregistered_t(rs_packet_unregistered_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_lambda_result_t(rs_packet_lambda_result_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_error_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_lambda_result_error_t(rs_packet_lambda_result_error_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_int_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_lambda_result_int_t(rs_packet_lambda_result_int_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_double_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_lambda_result_double_t(rs_packet_lambda_result_double_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_string_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_lambda_result_string_t(rs_packet_lambda_result_string_t *pkt);

/**
 * @brief convert a rs_packet_call_by_id_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_call_by_id_t(rs_packet_call_by_id_t *pkt);

/**
 * @brief convert a rs_packet_call_by_name_t packet from host to network byte order
 *
 * @param pkt Packet
 */
void hton_rs_packet_call_by_name_t(rs_packet_call_by_name_t *pkt);

/*
 * network operations - ntoh
 * most operations are just for legacy reasons and do not do anything at the moment
 */

/**
 * @brief convert a rs_packet_base_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_base_t(rs_packet_base_t *pkt);

/**
 * @brief convert a rs_packet_registered_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_registered_t(rs_packet_registered_t *pkt);

/**
 * @brief convert a rs_packet_unregistered_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_unregistered_t(rs_packet_unregistered_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_lambda_result_t(rs_packet_lambda_result_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_error_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_lambda_result_error_t(rs_packet_lambda_result_error_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_int_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_lambda_result_int_t(rs_packet_lambda_result_int_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_double_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_lambda_result_double_t(rs_packet_lambda_result_double_t *pkt);

/**
 * @brief convert a rs_packet_lambda_result_string_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_lambda_result_string_t(rs_packet_lambda_result_string_t *pkt);

/**
 * @brief convert a rs_packet_call_by_id_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_call_by_id_t(rs_packet_call_by_id_t *pkt);

/**
 * @brief convert a rs_packet_call_by_name_t packet from network to host byte order
 *
 * @param pkt Packet
 */
void ntoh_rs_packet_call_by_name_t(rs_packet_call_by_name_t *pkt);

#endif //RIOTSENSORS_PACKETS_H
