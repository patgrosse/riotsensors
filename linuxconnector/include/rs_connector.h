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

#include <rs_packets.h>

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
int rs_linux_stop();

/**
 * @brief Send a packet to call a lambda by it's
 *
 * @param id ID of the packet
 * @param expected_type expected return type
 * @param result Where to store the result, it's size has to match the expected type
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_by_id(lambda_id_t id, rs_lambda_type_t expected_type, void *result);

/**
 * @brief Send a packet to call a lambda by it's name
 *
 * @param name Name of the packet
 * @param expected_type Expected return type
 * @param result Where to store the result, it's size has to match the expected type
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_by_name(const char *name, rs_lambda_type_t expected_type, void *result);

#endif //RIOTSENSORS_RS_CONNECTOR_H
