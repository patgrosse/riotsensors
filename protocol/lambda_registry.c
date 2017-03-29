/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <lambda_registry.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * All registered lambdas
 */
static rs_registered_lambda *lambda_registry[MAX_LAMBDAS];

/**
 * ID of the next lambda to be registered
 */
static lambda_id_t lambda_counter = 0;

lambda_id_t get_number_of_registered_lambdas(void) {
    return lambda_counter;
}

void init_lambda_registry(void) {
    for (lambda_id_t i = 0; i < MAX_LAMBDAS; i++) {
        lambda_registry[i] = NULL;
    }
}

void free_lambda_registry(void) {
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

int8_t
lambda_registry_register(const char *name, const rs_lambda_type_t type, const rs_cache_type_t cache, lambda_arg arg) {
    if (lambda_counter >= MAX_LAMBDAS) {
        return RS_REGISTER_LIMIT_REACHED;
    }
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len > MAX_LAMBDA_NAME_LENGTH) {
        fprintf(stderr, "String is empty or too long for lambda name: %s\n", name);
        return RS_REGISTER_INVALNAME;
    }
    for (size_t i = 0; i < name_len; i++) {
        if (!isalnum(name[i])) {
            fprintf(stderr, "String contains a not alphanumeric character '%c': %s\n", name[i], name);
            return RS_REGISTER_INVALNAME;
        }
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
    if (lambda_registry[id] == NULL) {
        return RS_UNREGISTER_NOTFOUND;
    }
    free(lambda_registry[id]);
    lambda_registry[id] = NULL;
    return RS_UNREGISTER_SUCCESS;
}

rs_lambda_type_t get_lambda_type_from_string(const char *str) {
    char *endparsed;
    long num = strtol(str, &endparsed, 10);
    if (endparsed != str + strlen(str)) {
        return (rs_lambda_type_t) -1;
    }
    if (num == RS_LAMBDA_INT) {
        return RS_LAMBDA_INT;
    } else if (num == RS_LAMBDA_DOUBLE) {
        return RS_LAMBDA_DOUBLE;
    } else if (num == RS_LAMBDA_STRING) {
        return RS_LAMBDA_STRING;
    } else {
        return (rs_lambda_type_t) -1;
    }
}
