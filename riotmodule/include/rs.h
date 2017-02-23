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

/** @brief Maximum number of registered lambdas */
#define MAX_LAMBDAS 255

/** @brief Name does not match the requirements */
#define RS_REGISTER_INVALNAME -1
/** @brief A lambda with the given name already exists */
#define RS_REGISTER_DUPLICATE -2
/** @brief Maximum number of lambda registrations reached */
#define RS_REGISTER_LIMIT_REACHED -3
/** @brief Could not allocate the required memory */
#define RS_REGISTER_NOMEM -4

/** @brief The call was successfully */
#define RS_CALL_SUCCESS 0
/** @brief Lambda with the given ID was not found */
#define RS_CALL_NOTFOUND -1
/** @brief Lambda called with wrong return type */
#define RS_CALL_WRONGTYPE -2

/** @brief The sending of a result was successfully */
#define RS_RESULT_SUCCESS 0
/** @brief Lambda with the given ID was not found */
#define RS_RESULT_NOTFOUND -1
/** @brief Result has a wrong return type */
#define RS_RESULT_WRONGTYPE -2

/** @brief Unregister was successful */
#define RS_UNREGISTER_SUCCESS 0
/** @brief Lambda with the given ID was not found */
#define RS_UNREGISTER_NOTFOUND -1

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
 * @brief Initialize the internal lambda registry
 */
void init_lambda_registry();

/**
 * @brief Unregister all lambdas and free the allocated memory
 */
void free_lambda_registry();

/**
 * @brief Register an integer lambda
 *
 * @param name Name of the lambda
 * @param lambda Function to evaluate when called
 * @return A lambda_id_t on success, RS_REGISTER_* constant on error
 */
int8_t register_lambda_int(const char *name, lambda_int_t lambda);

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
 * @return A lambda_id_t on success, RS_REGISTER_* constant on error
 */
int8_t register_lambda_double(const char *name, lambda_double_t lambda);

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
 * @return A lambda_id_t on success, RS_REGISTER_* constant on error
 */
int8_t register_lambda_string(const char *name, lambda_string_t lambda);

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
 * Manually send an evaluation result for an integer lambda
 * @param id The ID of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_int(const lambda_id_t id, rs_int_t result);

/**
 * Manually send an evaluation result for an integer lambda
 * @param name The name of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_int_by_name(const char *name, rs_int_t result);

/**
 * Manually send an evaluation result for a double lambda
 * @param id The ID of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_double(const lambda_id_t id, rs_double_t result);

/**
 * Manually send an evaluation result for a double lambda
 * @param name The name of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_double_by_name(const char *name, rs_double_t result);

/**
 * Manually send an evaluation result for a string lambda
 * @param id The ID of the lambda
 * @param result The result of the lambda
 * @return A RS_RESULT_* constant
 */
int8_t send_result_lambda_string(const lambda_id_t id, rs_string_t result);

/**
 * Manually send an evaluation result for a string lambda
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
 * Get the lambda ID from a given lambda name
 * @param name Name of the lambda
 * @return The lambda ID on success or (lambda_id_t) -1 if not found
 */
lambda_id_t get_lambda_id_from_name(const char *name);

#endif //RIOTSENSORS_RS_H
