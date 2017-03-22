/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Interface for Linux tools that communicate with the RIOT device
 * @file    rs_connector.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_CONNECTOR_H
#define RIOTSENSORS_RS_CONNECTOR_H

#include <pthread.h>
#include <stdbool.h>

#include <spt.h>
#include <rs_packets.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides enough space to store every return type of a lambda
 */
typedef union {
    /** @brief Int return type */
    rs_int_t ret_i;
    /** @brief Double return type */
    rs_double_t ret_d;
    /** @brief String return type */
    rs_string_t ret_s;
} generic_lambda_return;

/**
 * @brief Additional data to store with a lambda in the registry
 */
typedef struct {
    pthread_cond_t wait_result;
    bool data_cached;
    int8_t last_call_error;
    generic_lambda_return ret;
} rs_linux_registered_lambda;

/**
 * @brief Start listening to the given serial connection
 *
 * @param serial_file Device name to the serial connection (eg. /dev/ttyUSB0)
 * @return 0 on success
 */
int rs_linux_start(const char *serial_file);

/**
 * @brief Stop listening to a serial connection (initiated by rs_linux_start())
 *
 * @return 0 on success
 */
int rs_linux_stop(void);

/**
 * @brief Send a packet to call a lambda by it's
 *
 * @param id ID of the packet
 * @param expected_type expected return type
 * @param result Where to store the result
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_by_id(lambda_id_t id, rs_lambda_type_t expected_type, generic_lambda_return *result);

/**
 * @brief Send a packet to call a lambda by it's name
 *
 * @param name Name of the packet
 * @param expected_type Expected return type
 * @param result Where to store the result
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_by_name(const char *name, rs_lambda_type_t expected_type, generic_lambda_return *result);

/**
 * @brief Handle an incoming binary data packet
 * Should not be used in user programs, internal use only
 *
 * @param sptctx Context of libspt
 * @param packet Received packet
 */
void handle_received_packet(struct spt_context *sptctx, struct serial_data_packet *packet);

#ifdef __cplusplus
}
#endif

#endif //RIOTSENSORS_RS_CONNECTOR_H
