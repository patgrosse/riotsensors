/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
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

rs_registered_lambda *get_registered_lambda_by_id(const lambda_id_t id);

rs_registered_lambda *get_registered_lambda_by_name(const char *name);

int8_t lambda_registry_register(const char *name, const rs_lambda_type_t type, void *arg);

int8_t lambda_registry_unregister(const lambda_id_t id);

#endif //RIOTSENSORS_LAMBDA_REGISTRY_H
