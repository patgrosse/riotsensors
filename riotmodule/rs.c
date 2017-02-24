/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs.h>
#include <memory.h>
#include <malloc.h>
#include <unused.h>
#include <unistd.h>
#include <serial_io.h>
#include <spt.h>
#include <stdio.h>

/**
 * @brief Function header for a generic lambda, internal use only
 */
typedef void (*lambda_generic_t)(lambda_id_t);

/**
 * @brief Stores data for a registered lambda
 */
typedef struct registered_lambda {
    /** ID of the lambda */
    lambda_id_t id;
    /** Name of the lambda */
    char *name;
    /** Type of the lambda */
    rs_lambda_type_t type;
    /** Function to be evaluated */
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

struct serial_io_context rs_sictx;
struct spt_context rs_sptctx;

registered_lambda *get_lambda_by_name(const char *name) {
    for (lambda_id_t i = 0; i < lambda_counter; i++) {
        if (lambda_registry[i] != NULL) {
            if (strcmp(lambda_registry[i]->name, name) == 0) {
                return lambda_registry[i];
            }
        }
    }
    return NULL;
}

int8_t register_lambda(const char *name, lambda_generic_t lambda, rs_lambda_type_t type) {
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
    strcpy(lambda_registry[myid]->name, name);
    if (lambda_registry[myid]->name == NULL) {
        return RS_REGISTER_NOMEM;
    }
    lambda_registry[myid]->type = type;
    lambda_registry[myid]->lambda = lambda;
    lambda_counter++;

    rs_packet_registered_t *pkt = malloc(sizeof(rs_packet_registered_t));
    pkt->base.ptype = RS_PACKET_REGISTERED;
    strcpy(pkt->name, name);
    pkt->ltype = type;
    hton_rs_packet_registered_t(pkt);
    // TODO send packet
    free(pkt);
    return myid;
}

void populate_resultbase_from_lambda(rs_packet_lambda_result_t *base, const registered_lambda *lambda) {
    base->lambda_id = lambda->id;
    strcpy(base->name, lambda->name);
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

int8_t send_result_lambda_int(const lambda_id_t id, rs_int_t result) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    if (lambda_registry[id]->type != RS_LAMBDA_INT) {
        return RS_RESULT_WRONGTYPE;
    }
    rs_packet_lambda_result_int_t *pkt = malloc(sizeof(rs_packet_lambda_result_int_t));
    pkt->result_base.base.ptype = RS_PACKET_RESULT_INT;
    populate_resultbase_from_lambda(&pkt->result_base, lambda_registry[id]);
    pkt->result = result;
    hton_rs_packet_lambda_result_int_t(pkt);
    // TODO send packet
    free(pkt);
    return RS_RESULT_SUCCESS;
}

int8_t send_result_lambda_int_by_name(const char *name, rs_int_t result) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    return send_result_lambda_int(lambda->id, result);
}

int8_t send_result_lambda_double(const lambda_id_t id, rs_double_t result) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    if (lambda_registry[id]->type != RS_LAMBDA_DOUBLE) {
        return RS_RESULT_WRONGTYPE;
    }
    rs_packet_lambda_result_double_t *pkt = malloc(sizeof(rs_packet_lambda_result_double_t));
    pkt->result_base.base.ptype = RS_PACKET_RESULT_DOUBLE;
    populate_resultbase_from_lambda(&pkt->result_base, lambda_registry[id]);
    pkt->result = result;
    hton_rs_packet_lambda_result_double_t(pkt);
    // TODO send packet
    free(pkt);
    return RS_RESULT_SUCCESS;
}

int8_t send_result_lambda_double_by_name(const char *name, rs_double_t result) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    return send_result_lambda_double(lambda->id, result);
}

int8_t send_result_lambda_string(const lambda_id_t id, rs_string_t result) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    if (lambda_registry[id]->type != RS_LAMBDA_STRING) {
        return RS_RESULT_WRONGTYPE;
    }
    rs_packet_lambda_result_string_t *pkt = malloc(sizeof(rs_packet_lambda_result_string_t));
    pkt->result_base.base.ptype = RS_PACKET_RESULT_STRING;
    populate_resultbase_from_lambda(&pkt->result_base, lambda_registry[id]);
    pkt->result = result;
    hton_rs_packet_lambda_result_string_t(pkt);
    // TODO send packet
    free(pkt);
    return RS_RESULT_SUCCESS;
}

int8_t send_result_lambda_string_by_name(const char *name, rs_string_t result) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    return send_result_lambda_string(lambda->id, result);
}


int8_t unregister_lambda(const lambda_id_t id) {
    if (id >= MAX_LAMBDAS || lambda_registry[id] == NULL) {
        return RS_UNREGISTER_NOTFOUND;
    }

    rs_packet_unregistered_t *pkt = malloc(sizeof(rs_packet_unregistered_t));
    pkt->base.ptype = RS_PACKET_REGISTERED;
    strcpy(pkt->name, lambda_registry[id]->name);
    hton_rs_packet_unregistered_t(pkt);
    // TODO send packet
    free(pkt);

    free(lambda_registry[id]->name);
    free(lambda_registry[id]);
    lambda_registry[id] = NULL;
    return RS_UNREGISTER_SUCCESS;
}

lambda_id_t get_lambda_id_from_name(const char *name) {
    registered_lambda *lambda = get_lambda_by_name(name);
    if (lambda == NULL) {
        return (lambda_id_t) -1;
    }
    return lambda->id;
}

void handle_call_lambda(lambda_id_t id, rs_lambda_type_t expected_type) {
    if (expected_type == RS_LAMBDA_INT) {
        rs_int_t result;
        call_lambda_int(id, &result);
        // TODO answer
    } else if (expected_type == RS_LAMBDA_DOUBLE) {
        rs_double_t result;
        call_lambda_double(id, &result);
        // TODO answer
    } else if (expected_type == RS_LAMBDA_STRING) {
        rs_string_t result;
        call_lambda_string(id, &result);
        // TODO answer
    } else {
        fprintf(stderr, "Called lambda with id %d but unknown type %d\n", id, expected_type);
    }
}

void handle_received_packet(struct spt_context *sptctx, struct serial_data_packet *packet) {
    UNUSED(sptctx);
    rs_packet_type_t ptype;
    if (packet->len < sizeof(ptype)) {
        fprintf(stderr, "Packet with size %d is too small for packet type detection (min size %d)\n", packet->len,
                (int) sizeof(ptype));
        return;
    }
    ptype = (rs_packet_type_t) *packet->data;
    if (ptype == RS_PACKET_CALL_BY_ID) {
        if (packet->len != sizeof(rs_packet_call_by_id_t)) {
            fprintf(stderr,
                    "Packet with size %d has the wrong size for packet type rs_packet_call_by_id_t (size %d)\n",
                    packet->len,
                    (int) sizeof(rs_packet_call_by_id_t));
            return;
        }
        rs_packet_call_by_id_t mypkt;
        memcpy(&mypkt, packet->data, sizeof(rs_packet_call_by_id_t));
        ntoh_rs_packet_call_by_id_t(&mypkt);
        handle_call_lambda(mypkt.lambda_id, mypkt.expected_type);
    } else if (ptype == RS_PACKET_CALL_BY_NAME) {
        if (packet->len != sizeof(rs_packet_call_by_name_t)) {
            fprintf(stderr,
                    "Packet with size %d has the wrong size for packet type rs_packet_call_by_name_t (size %d)\n",
                    packet->len,
                    (int) sizeof(rs_packet_call_by_name_t));
            return;
        }
        rs_packet_call_by_name_t mypkt;
        memcpy(&mypkt, packet->data, sizeof(rs_packet_call_by_name_t));
        ntoh_rs_packet_call_by_name_t(&mypkt);
        lambda_id_t id = get_lambda_id_from_name(mypkt.name);
        if (id == (lambda_id_t) -1) {
            fprintf(stderr,
                    "Could not find lambda with name %s from rs_packet_call_by_name_t\n",
                    mypkt.name);
            return;
        }
        handle_call_lambda(id, mypkt.expected_type);
    } else {
        fprintf(stderr,
                "Received packet of unknown/unprocessable type %d with size %d\n",
                ptype,
                packet->len);
    }
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

void rs_start() {
    init_lambda_registry();
    serial_io_context_init(&rs_sictx, STDIN_FILENO, STDOUT_FILENO);
    spt_init_context(&rs_sptctx, &rs_sictx, handle_received_packet);
    spt_start(&rs_sptctx);
}

void rs_stop() {
    free_lambda_registry();
    spt_stop(&rs_sptctx);
}