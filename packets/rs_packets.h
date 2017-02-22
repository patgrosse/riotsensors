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

/** @brief Maximum length of a lambda name */
#define MAX_LAMBDA_NAME_LENGTH 12

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
#define RS_PACKET_CALL_BY_NAME 3
/**
 * @brief Lambda call produced an error
 */
#define RS_PACKET_RESULT_ERROR 4
/**
 * @brief Integer lambda has been evaluated successfully
 */
#define RS_PACKET_RESULT_INT 5
/**
 * @brief Double lambda has been evaluated successfully
 */
#define RS_PACKET_RESULT_DOUBLE 6
/**
 * @brief String lambda has been evaluated successfully
 */
#define RS_PACKET_RESULT_STRING 7

/**
 * @brief Identifier of a packet type (RS_PACKET_* constants)
 */
typedef uint8_t rs_packet_type_t;

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

/*
 * Packet definitions from RIOT to Linux
 */

typedef struct {
    rs_packet_type_t ptype;
} rs_packet_base_t;

typedef struct {
    rs_packet_base_t base;
    char name[MAX_LAMBDA_NAME_LENGTH];
    rs_lambda_type_t ltype;
} rs_packet_registered_t;

typedef struct {
    rs_packet_base_t base;
    char name[MAX_LAMBDA_NAME_LENGTH];
} rs_packet_unregistered_t;

typedef struct {
    rs_packet_base_t base;
    lambda_id_t lambda_id;
    char name[MAX_LAMBDA_NAME_LENGTH];
} rs_packet_lambda_result_t;

typedef struct {
    rs_packet_lambda_result_t result_base;
    int8_t error_code;
} rs_packet_lambda_result_error_t;

typedef struct {
    rs_packet_lambda_result_t result_base;
    rs_int_t result;
} rs_packet_lambda_result_int_t;

typedef struct {
    rs_packet_lambda_result_t result_base;
    rs_double_t result;
} rs_packet_lambda_result_double_t;

typedef struct {
    rs_packet_lambda_result_t result_base;
    uint16_t result_length;
    rs_string_t result;
} rs_packet_lambda_result_string_t;

/*
 * Packet definitions from Linux to RIOT
 */

typedef struct {
    rs_packet_base_t base;
    lambda_id_t lambda_id;
    rs_lambda_type_t expected_type;
} rs_packet_call_by_id_t;

typedef struct {
    rs_packet_base_t base;
    char name[MAX_LAMBDA_NAME_LENGTH];
    rs_lambda_type_t expected_type;
} rs_packet_call_by_name_t;

#endif //RIOTSENSORS_PACKETS_H
