/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include "lambda_registry.h"

/**
 * All registered lambdas
 */
static rs_registered_lambda *lambda_registry[MAX_LAMBDAS];

/**
 * ID of the next lambda to be registered
 */
static lambda_id_t lambda_counter = 0;

void init_lambda_registry() {
    for (lambda_id_t i = 0; i < MAX_LAMBDAS; i++) {
        lambda_registry[i] = NULL;
    }
}

void free_lambda_registry() {
    for (lambda_id_t i = 0; i < lambda_counter; i++) {
        if (lambda_registry[i] != NULL) {
            free(lambda_registry[i]);
            lambda_registry[i] = NULL;
        }
    }
    lambda_counter = 0;
}

rs_registered_lambda *get_registered_lambda_by_id(const lambda_id_t id) {
    if (id >= MAX_LAMBDAS || id >= lambda_counter) {
        return NULL;
    } else {
        return lambda_registry[id];
    }
}

rs_registered_lambda *get_registered_lambda_by_name(const char *name) {
    for (lambda_id_t i = 0; i < lambda_counter; i++) {
        if (lambda_registry[i] != NULL) {
            if (strcmp(lambda_registry[i]->name, name) == 0) {
                return lambda_registry[i];
            }
        }
    }
    return NULL;
}

int8_t lambda_registry_register(const char *name, const rs_lambda_type_t type, const rs_cache_type_t cache, void *arg) {
    if (lambda_counter >= MAX_LAMBDAS) {
        return RS_REGISTER_LIMIT_REACHED;
    }
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len > MAX_LAMBDA_NAME_LENGTH) {
        return RS_REGISTER_INVALNAME;
    }
    if (get_registered_lambda_by_name(name) != NULL) {
        return RS_REGISTER_DUPLICATE;
    }
    lambda_id_t myid = lambda_counter;
    lambda_registry[myid] = malloc(sizeof(rs_registered_lambda));
    if (lambda_registry[myid] == NULL) {
        return RS_REGISTER_NOMEM;
    }
    lambda_registry[myid]->id = myid;
    strcpy(lambda_registry[myid]->name, name);
    if (lambda_registry[myid]->name == NULL) {
        return RS_REGISTER_NOMEM;
    }
    lambda_registry[myid]->type = type;
    lambda_registry[myid]->cache = cache;
    lambda_registry[myid]->arg = arg;
    lambda_counter++;
    return myid;
}

int8_t lambda_registry_unregister(const lambda_id_t id) {
    if (id >= MAX_LAMBDAS || id >= lambda_counter) {
        return RS_UNREGISTER_NOTFOUND;
    }
    free(lambda_registry[id]);
    lambda_registry[id] = NULL;
    return RS_UNREGISTER_SUCCESS;
}