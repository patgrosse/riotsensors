/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   API of riotsensors
 * @file    rs.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_H
#define RIOTSENSORS_RS_H

#include <inttypes.h>

/** @brief Maximum number of registered lambdas */
#define MAX_LAMBDAS 255
/** @brief Maximum length of a lambda name */
#define MAX_LAMBDA_NAME_LENGTH 12

/** @brief Name does not match the requirements */
#define RS_REGISTER_INVALNAME -1
/** @brief A lambda with the given name already exists */
#define RS_REGISTER_DUPLICATE -2

/** @brief The call was successfully */
#define RS_CALL_SUCCESS 0
/** @brief Lambda with the given ID was not found */
#define RS_CALL_NOTFOUND -1
/** @brief Lambda called with wrong return type */
#define RS_CALL_WRONGTYPE -2

/** @brief Unregister was successful */
#define RS_UNREGISTER_SUCCESS 0
/** @brief Lambda with the given ID was not found */
#define RS_UNREGISTER_NOTFOUND -1

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
 * @brief Function header for a generic lambda, internal use only
 */
typedef void (*lambda_generic_t)(lambda_id_t);

typedef enum {
    RS_LAMBDA_INT,
    RS_LAMBDA_DOUBLE,
    RS_LAMBDA_STRING,
    __RS_LAMBDA_MAX
} lambda_type_t;

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
