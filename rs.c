/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs.h>
#include <memory.h>

typedef struct registered_lambda {
    lambda_id_t id;
    const char *name;
    lambda_type_t type;
    lambda_generic_t lambda;
} registered_lambda;

registered_lambda *lambda_registry[MAX_LAMBDAS];
lambda_id_t lambda_counter = 0;

registered_lambda *get_lambda_by_name(const char *name) {
    for (lambda_id_t i = 0; i < lambda_counter; i++) {
        if (strcmp(lambda_registry[i]->name, name)) {
            return lambda_registry[i];
        }
    }
    return NULL;
}

int8_t register_lambda_int(const char *name, lambda_int_t lambda) {
    if (strlen(name) > MAX_LAMBDA_NAME_LENGTH) {
        return RS_REGISTER_INVALNAME;
    }
    if (get_lambda_by_name(name) != NULL) {
        return RS_REGISTER_DUPLICATE;
    }
    lambda_id_t myid = lambda_counter;
    lambda_counter++;
    lambda_registry[myid]->id = myid;
    lambda_registry[myid]->name = name;
    lambda_registry[myid]->type = RS_LAMBDA_INT;
    lambda_registry[myid]->lambda = (lambda_generic_t) lambda;
    return myid;
}

int8_t call_lambda_int(const lambda_id_t id, rs_int_t *result) {
    return 0;
}

int8_t call_lambda_int_by_name(const char *name, rs_int_t *result) {
    return 0;
}

int8_t register_lambda_double(const char *name, lambda_double_t lambda) {
    if (strlen(name) > MAX_LAMBDA_NAME_LENGTH) {
        return RS_REGISTER_INVALNAME;
    }
    if (get_lambda_by_name(name) != NULL) {
        return RS_REGISTER_DUPLICATE;
    }
    lambda_id_t myid = lambda_counter;
    lambda_counter++;
    lambda_registry[myid]->id = myid;
    lambda_registry[myid]->name = name;
    lambda_registry[myid]->type = RS_LAMBDA_DOUBLE;
    lambda_registry[myid]->lambda = (lambda_generic_t) lambda;
    return myid;
}

int8_t call_lambda_double(const lambda_id_t id, rs_double_t *result) {
    return 0;
}

int8_t call_lambda_double_by_name(const char *name, rs_double_t *result) {
    return 0;
}

int8_t register_lambda_string(const char *name, lambda_string_t lambda) {
    if (strlen(name) > MAX_LAMBDA_NAME_LENGTH) {
        return RS_REGISTER_INVALNAME;
    }
    if (get_lambda_by_name(name) != NULL) {
        return RS_REGISTER_DUPLICATE;
    }
    lambda_id_t myid = lambda_counter;
    lambda_counter++;
    lambda_registry[myid]->id = myid;
    lambda_registry[myid]->name = name;
    lambda_registry[myid]->type = RS_LAMBDA_STRING;
    lambda_registry[myid]->lambda = (lambda_generic_t) lambda;
    return myid;
}

int8_t call_lambda_string(const lambda_id_t id, rs_string_t *result) {
    return 0;
}

int8_t call_lambda_string_by_name(const char *name, rs_string_t *result) {
    return 0;
}

int8_t unregister_lambda(const lambda_id_t id) {
    return 0;
}

lambda_id_t get_lambda_id_from_name(const char *name) {
    return 0;
}

