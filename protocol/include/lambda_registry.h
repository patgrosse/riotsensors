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

#include <rs_packets.h>
#include <stddef.h>
#include <string.h>
#include <rs_constants.h>
#include <malloc.h>

typedef struct {
    /** ID of the lambda */
    lambda_id_t id;
    /** Name of the lambda */
    char name[MAX_LAMBDA_NAME_LENGTH];
    /** Type of the lambda */
    rs_lambda_type_t type;
    void *arg;
} rs_registered_lambda;

/**
 * @brief Initialize the internal lambda registry
 */
void init_lambda_registry();

/**
 * @brief Unregister all lambdas and free the allocated memory
 */
void free_lambda_registry();

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
 * @param arg A user specific argument to be stored in the properties
 * @return A value greater/equals to zero containing the new ID on success, a negative RS_REGISTER_* value on failure
 */
int8_t lambda_registry_register(const char *name, const rs_lambda_type_t type, void *arg);

/**
 * @brief Unregister a lambda and free the allocated resources
 *
 * @param id ID of the lambda
 * @return A RS_UNREGISTER_* constant
 */
int8_t lambda_registry_unregister(const lambda_id_t id);

#endif //RIOTSENSORS_LAMBDA_REGISTRY_H