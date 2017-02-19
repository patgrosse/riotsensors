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

/** @brief Successfully registered lambda */
#define RS_REGISTER_SUCCESS 0
/** @brief Name does not match the requirements */
#define RS_REGISTER_INVALNAME -1
/** @brief A lambda with the given name already exists */
#define RS_REGISTER_DUPLICATE -2

/** @brief The call was successfully */
#define RS_CALL_SUCCESS 0
/** @brief Lambda with the given name was not found */
#define RS_CALL_NOTFOUND -1
/** @brief Lambda called with wrong return type */
#define RS_CALL_WRONGTYPE -2

/** @brief Unregister was successful */
#define RS_UNREGISTER_SUCCESS 0
/** @brief Lambda with the given name was not found */
#define RS_UNREGISTER_NOTFOUND -1

/**
 * @brief Register an integer lambda
 *
 * @param name Name of the lambda
 * @param lambda Function to evaluate when called
 * @return A RS_REGISTER_* constant
 */
int8_t register_lambda_int(const char *name, int32_t lambda());

/**
 * @brief Call an integer lambda
 *
 * @param name Name of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_int(const char *name, int32_t *result);

/**
 * @brief Register a double lambda
 *
 * @param name Name of the lambda
 * @param lambda Function to evaluate when called
 * @return A RS_REGISTER_* constant
 */
int8_t register_lambda_double(const char *name, double lambda());

/**
 * @brief Call a double lambda
 *
 * @param name Name of the lambda
 * @param result Where to store the result on success
 * @return A RS_CALL_* constant
 */
int8_t call_lambda_double(const char *name, double *result);

/**
 * @brief Register a string lambda
 *
 * The evaluated function has to return a pointer to a malloc'ed memory area that will be free'd automatically.
 *
 * @param name Name of the lambda
 * @param lambda Function to evaluate when called
 * @return A RS_REGISTER_* constant
 */
int8_t register_lambda_string(const char *name, char *lambda());

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
int8_t call_lambda_string(const char *name, char **result);

/**
 * @brief Unregister a lambda
 *
 * @param name Name of the lambda to unregister
 * @return A RS_UNREGISTER_* constant
 */
int8_t unregister_lambda(const char *name);


#endif //RIOTSENSORS_RS_H
