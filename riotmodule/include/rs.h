/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   RIOT-OS API of riotsensors
 * @file    rs.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_H
#define RIOTSENSORS_RS_H

#include <inttypes.h>

#include <rs_packets.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The sending of a result was successfully */
#define RS_RESULT_SUCCESS 0
/** @brief Lambda with the given ID was not found */
#define RS_RESULT_NOTFOUND -1
/** @brief Result has a wrong return type */
#define RS_RESULT_WRONGTYPE -2

/**
 * @brief Function header for an integer lambda
 */
typedef rs_int_t (*lambda_int_t)(lambda_id_t);

/**
 * @brief Function header for a double lambda
 */
typedef rs_double_t (*lambda_double_t)(lambda_id_t);

/**
 * @brief Function header for a string lambda
 */
typedef rs_string_t (*lambda_string_t)(lambda_id_t);

/**
 * @brief Initialize the internal lambda registry and start packet processing
 */
void rs_start(void);

/**
 * @brief Unregister all lambdas, stop the packet processing and free the allocated memory
 */
void rs_stop(void);

/**
 * @brief Register an integer lambda
 *
 * @param name Name of the lambda
 * @param lambda Function to evaluate when called
 * @param cache Cache policy to use with the lambda
 * @return A lambda_id_t on success, RS_REGISTER_* constant on error
 */
int8_t register_lambda_int(const char *name, lambda_int_t lambda, const rs_cache_type_t cache);

/**
 * @brief Call an integer lambda
 *
 * @param id ID of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_int(const lambda_id_t id, rs_int_t *result);

/**
 * @brief Call an integer lambda
 *
 * @param name Name of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_int_by_name(const char *name, rs_int_t *result);

/**
 * @brief Register a double lambda
 *
 * @param name Name of the lambda
 * @param lambda Function to evaluate when called
 * @param cache Cache policy to use with the lambda
 * @return A lambda_id_t on success, RS_REGISTER_* constant on error
 */
int8_t register_lambda_double(const char *name, lambda_double_t lambda, const rs_cache_type_t cache);

/**
 * @brief Call a double lambda
 *
 * @param id ID of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_double(const lambda_id_t id, rs_double_t *result);

/**
 * @brief Call a double lambda
 *
 * @param name Name of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_double_by_name(const char *name, rs_double_t *result);

/**
 * @brief Register a string lambda
 *
 * The evaluated function has to return a pointer to a malloc'ed memory area that will be free'd automatically.
 *
 * @param name Name of the lambda
 * @param lambda Function to evaluate when called
 * @param cache Cache policy to use with the lambda
 * @return A lambda_id_t on success, RS_REGISTER_* constant on error
 */
int8_t register_lambda_string(const char *name, lambda_string_t lambda, const rs_cache_type_t cache);

/**
 * @brief Call a string lambda
 *
 * In result a pointer to a malloc'ed memory area is stored. The memory has to be free'd after the result has been
 * handled.
 *
 * @param id ID of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 * @see register_lambda_string()
 */
int8_t call_lambda_string(const lambda_id_t id, rs_string_t *result);

/**
 * @brief Call a string lambda
 *
 * In result a pointer to a malloc'ed memory area is stored. The memory has to be free'd after the result has been
 * handled.
 *
 * @param name Name of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 * @see register_lambda_string()
 */
int8_t call_lambda_string_by_name(const char *name, rs_string_t *result);

/**
 * @brief Manually send an evaluation result for an integer lambda
 *
 * @param id The ID of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_int(const lambda_id_t id, rs_int_t result);

/**
 * @brief Manually send an evaluation result for an integer lambda
 *
 * @param name The name of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_int_by_name(const char *name, rs_int_t result);

/**
 * @brief Manually send an evaluation result for a double lambda
 *
 * @param id The ID of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_double(const lambda_id_t id, rs_double_t result);

/**
 * @brief Manually send an evaluation result for a double lambda
 *
 * @param name The name of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_double_by_name(const char *name, rs_double_t result);

/**
 * @brief Manually send an evaluation result for a string lambda
 *
 * @param id The ID of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_string(const lambda_id_t id, rs_string_t result);

/**
 * @brief Manually send an evaluation result for a string lambda
 *
 * @param name The name of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_string_by_name(const char *name, rs_string_t result);

/**
 * @brief Unregister a lambda
 *
 * @param id ID of the lambda to unregister
 * @return A RS_UNREGISTER_* constant
 */
int8_t unregister_lambda(const lambda_id_t id);

/**
 * @brief Get the lambda ID from a given lambda name
 *
 * @param name Name of the lambda
 * @return The lambda ID on success or (lambda_id_t) -1 if not found
 */
lambda_id_t get_lambda_id_from_name(const char *name);

#ifdef __cplusplus
}
#endif

#endif //RIOTSENSORS_RS_H
