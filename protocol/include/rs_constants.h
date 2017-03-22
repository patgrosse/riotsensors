/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Different constants to be used with riotsensors cross systems
 * @file    rs_constants.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_CONSTANTS_H
#define RIOTSENSORS_RS_CONSTANTS_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum number of registered lambdas */
#define MAX_LAMBDAS 255

/** @brief Maximum length of a lambda name */
#define MAX_LAMBDA_NAME_LENGTH 12

/** @brief Name does not match the requirements */
#define RS_REGISTER_INVALNAME -1
/** @brief A lambda with the given name already exists */
#define RS_REGISTER_DUPLICATE -2
/** @brief Maximum number of lambda registrations reached */
#define RS_REGISTER_LIMIT_REACHED -3
/** @brief Could not allocate the required memory */
#define RS_REGISTER_NOMEM -4
/** @brief A parameter/a combination of parameters is invalid */
#define RS_REGISTER_INVALPARAM -5

/**
 * Get a human readable string for a register result
 * @param c RS_REGISTER_* constant
 * @return A string or NULL
 */
const char *stringify_rs_register_result(int8_t c);

/** @brief Unregister was successful */
#define RS_UNREGISTER_SUCCESS 0
/** @brief Lambda with the given ID was not found */
#define RS_UNREGISTER_NOTFOUND -1

/**
 * Get a human readable string for a unregister result
 * @param c RS_UNREGISTER_* constant
 * @return A string or NULL
 */
const char *stringify_rs_unregister_result(int8_t c);

/** @brief The call was successfully */
#define RS_CALL_SUCCESS 0
/** @brief Result has been retrieved from cache */
#define RS_CALL_CACHE 1
/** @brief Result has been retrieved from cache after timeout */
#define RS_CALL_CACHE_TIMEOUT 2
/** @brief Lambda with the given ID was not found */
#define RS_CALL_NOTFOUND -1
/** @brief Lambda called with wrong return type */
#define RS_CALL_WRONGTYPE -2
/** @brief Result could not be retrieved in time */
#define RS_CALL_TIMEOUT -3
/** @brief Result could not be found in cache */
#define RS_CALL_CACHE_EMPTY -4
/** @brief Result could not be retrieved in time and was not found in cache */
#define RS_CALL_CACHE_TIMEOUT_EMPTY -5

/**
 * Get a human readable string for a call result
 * @param c RS_CALL_* constant
 * @return A string or NULL
 */
const char *stringify_rs_call_result(int8_t c);

#ifdef __cplusplus
}
#endif

#endif //RIOTSENSORS_RS_CONSTANTS_H
