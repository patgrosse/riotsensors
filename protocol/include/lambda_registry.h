/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Registry for lambdas that are available
 * @file    lambda_registry.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_LAMBDA_REGISTRY_H
#define RIOTSENSORS_LAMBDA_REGISTRY_H

#include <stddef.h>
#include <string.h>
#include <malloc.h>

#include <rs_packets.h>
#include <rs_constants.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generic function that can be stored within the lambda registry
 */
typedef void *(*function)(void);

/**
 * @brief Lambda argument that can be stored with a specific lambda
 *
 * Replacement for void* for ISO compilers
 */
typedef union {
    void *obj;
    function func;
} lambda_arg;

/**
 * @brief A known and registered lambda in the registry
 */
typedef struct {
    /** @brief ID of the lambda */
    lambda_id_t id;
    /** @brief Name of the lambda */
    char name[MAX_LAMBDA_NAME_LENGTH];
    /** @brief Type of the lambda */
    rs_lambda_type_t type;
    /** @brief Cache policy of this lambda */
    rs_cache_type_t cache;
    /** @brief User defined argument to be stored with the lambda */
    lambda_arg arg;
} rs_registered_lambda;

/**
 * @brief Get the amount of registered lambdas
 *
 * @return Amount of registered lambdas
 */
lambda_id_t get_number_of_registered_lambdas(void);

/**
 * @brief Initialize the internal lambda registry
 */
void init_lambda_registry(void);

/**
 * @brief Unregister all lambdas and free the allocated memory
 */
void free_lambda_registry(void);

/**
 * @brief Get a registered lambda and it's properties by it's ID
 *
 * @param id ID of the registered lambda
 * @return Registered lambda struct or NULL if not found
 */
rs_registered_lambda *get_registered_lambda_by_id(const lambda_id_t id);

/**
 * @brief Get a registered lambda and it's properties by it's name
 *
 * @param name Name of the lambda
 * @return Registered lambda struct or NULL if not found
 */
rs_registered_lambda *get_registered_lambda_by_name(const char *name);

/**
 * @brief Register a new lambda
 *
 * @param name Name of the lambda
 * @param type Lambda type
 * @param cache Cache policy for this lambda
 * @param arg A user specific argument to be stored in the properties
 * @return A value greater/equals to zero containing the new ID on success, a negative RS_REGISTER_* value on failure
 */
int8_t
lambda_registry_register(const char *name, const rs_lambda_type_t type, const rs_cache_type_t cache, lambda_arg arg);

/**
 * @brief Unregister a lambda and free the allocated resources
 *
 * @param id ID of the lambda
 * @return A RS_UNREGISTER_* constant
 */
int8_t lambda_registry_unregister(const lambda_id_t id);

/**
 * @brief Get the lambda type from a string containing the lambda type as integer
 *
 * @param str String with the lambda type
 * @return RS_LAMBDA_* constant on success, (rs_lambda_type_t) -1 otherwise
 */
rs_lambda_type_t get_lambda_type_from_string(const char* str);

#ifdef __cplusplus
}
#endif

#endif //RIOTSENSORS_LAMBDA_REGISTRY_H
