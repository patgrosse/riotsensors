/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs.h>
#include <memory.h>
#include <malloc.h>

/**
 * @brief Function header for a generic lambda, internal use only
 */
typedef void (*lambda_generic_t)(lambda_id_t);

/**
 * @brief Stores data for a registered lambda
 */
typedef struct registered_lambda {
    lambda_id_t id;
    char *name;
    lambda_type_t type;
    lambda_generic_t lambda;
} registered_lambda;

/**
 * All registered lambdas
 */
registered_lambda *lambda_registry[MAX_LAMBDAS];

/**
 * ID of the next lambda to be registered
 */
lambda_id_t lambda_counter = 0;

registered_lambda *get_lambda_by_name(const char *name) {
    for (lambda_id_t i = 0; i < lambda_counter; i++) {
        if (strcmp(lambda_registry[i]->name, name) == 0) {
            return lambda_registry[i];
        }
    }
    return NULL;
}

int8_t register_lambda(const char *name, lambda_generic_t lambda, lambda_type_t type) {
    if (lambda_counter >= MAX_LAMBDAS) {
        return RS_REGISTER_LIMIT_REACHED;
    }
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len > MAX_LAMBDA_NAME_LENGTH) {
        return RS_REGISTER_INVALNAME;
    }
    if (get_lambda_by_name(name) != NULL) {
        return RS_REGISTER_DUPLICATE;
    }
    lambda_id_t myid = lambda_counter;
    lambda_registry[myid] = malloc(sizeof(registered_lambda));
    if (lambda_registry[myid] == NULL) {
        return RS_REGISTER_NOMEM;
    }
    lambda_registry[myid]->id = myid;
    lambda_registry[myid]->name = malloc(name_len + 1);
    memcpy(lambda_registry[myid]->name, name, name_len + 1);
    if (lambda_registry[myid]->name == NULL) {
        return RS_REGISTER_NOMEM;
    }
    lambda_registry[myid]->type = type;
    lambda_registry[myid]->lambda = lambda;
    lambda_counter++;
    return myid;
}

void init_lambda_registry() {
    for (lambda_id_t i = 0; i < MAX_LAMBDAS; i++) {
        lambda_registry[i] = NULL;
    }
}

void free_lambda_registry() {
    for (lambda_id_t i = 0; i < lambda_counter; i++) {
        if (lambda_registry[i] != NULL) {
            free(lambda_registry[i]->name);
            free(lambda_registry[i]);
            lambda_registry[i] = NULL;
        }
    }
    lambda_counter = 0;
}

int8_t register_lambda_int(const char *name, lambda_int_t lambda) {
    return register_lambda(name, (lambda_generic_t) lambda, RS_LAMBDA_INT);
}

int8_t call_lambda_int(const lambda_id_t id, rs_int_t *result) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_CALL_NOTFOUND;
    }
    if (lambda_registry[id]->type != RS_LAMBDA_INT) {
        return RS_CALL_WRONGTYPE;
    }
    lambda_int_t lambda = (lambda_int_t) lambda_registry[id]->lambda;
    *result = lambda(id);
    return RS_CALL_SUCCESS;
}

int8_t call_lambda_int_by_name(const char *name, rs_int_t *result) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    return call_lambda_int(lambda->id, result);
}

int8_t register_lambda_double(const char *name, lambda_double_t lambda) {
    return register_lambda(name, (lambda_generic_t) lambda, RS_LAMBDA_DOUBLE);
}

int8_t call_lambda_double(const lambda_id_t id, rs_double_t *result) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_CALL_NOTFOUND;
    }
    if (lambda_registry[id]->type != RS_LAMBDA_DOUBLE) {
        return RS_CALL_WRONGTYPE;
    }
    lambda_double_t lambda = (lambda_double_t) lambda_registry[id]->lambda;
    *result = lambda(id);
    return RS_CALL_SUCCESS;
}

int8_t call_lambda_double_by_name(const char *name, rs_double_t *result) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    return call_lambda_double(lambda->id, result);
}

int8_t register_lambda_string(const char *name, lambda_string_t lambda) {
    return register_lambda(name, (lambda_generic_t) lambda, RS_LAMBDA_STRING);
}

int8_t call_lambda_string(const lambda_id_t id, rs_string_t *result) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_CALL_NOTFOUND;
    }
    if (lambda_registry[id]->type != RS_LAMBDA_STRING) {
        return RS_CALL_WRONGTYPE;
    }
    lambda_string_t lambda = (lambda_string_t) lambda_registry[id]->lambda;
    *result = lambda(id);
    return RS_CALL_SUCCESS;
}

int8_t call_lambda_string_by_name(const char *name, rs_string_t *result) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    return call_lambda_string(lambda->id, result);
}

int8_t unregister_lambda(const lambda_id_t id) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_UNREGISTER_NOTFOUND;
    }
    free(lambda_registry[id]->name);
    free(lambda_registry[id]);
    return RS_UNREGISTER_SUCCESS;
}

lambda_id_t get_lambda_id_from_name(const char *name) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return (lambda_id_t) -1;
    }
    return lambda->id;
}

