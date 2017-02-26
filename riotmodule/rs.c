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
#include <rs_packets.h>
#include <lambda_registry.h>

/**
 * @brief Function header for a generic lambda, internal use only
 */
typedef void (*lambda_generic_t)(lambda_id_t);

struct serial_io_context rs_sictx;
struct spt_context rs_sptctx;
bool rs_spt_started = false;

int8_t
register_lambda(const char *name, lambda_generic_t lambda, const rs_lambda_type_t type, const rs_cache_type_t cache) {
    int8_t res = lambda_registry_register(name, type, cache, lambda);
    if (res < 0) {
        return res;
    }

    if (rs_spt_started) {
        rs_packet_registered_t *pkt = malloc(sizeof(rs_packet_registered_t));
        pkt->base.ptype = RS_PACKET_REGISTERED;
        strcpy(pkt->name, name);
        pkt->ltype = type;
        pkt->cache = cache;
        hton_rs_packet_registered_t(pkt);
        struct serial_data_packet sdpkt;
        sdpkt.data = (char *) pkt;
        sdpkt.len = sizeof(*pkt);
        spt_send_packet(&rs_sptctx, &sdpkt);
        free(pkt);
    }
    return res;
}

void populate_resultbase_from_lambda(rs_packet_lambda_result_t *base, const rs_registered_lambda *lambda) {
    base->lambda_id = lambda->id;
    strcpy(base->name, lambda->name);
}

int8_t register_lambda_int(const char *name, lambda_int_t lambda, const rs_cache_type_t cache) {
    return register_lambda(name, (lambda_generic_t) lambda, RS_LAMBDA_INT, cache);
}

int8_t call_lambda_int(const lambda_id_t id, rs_int_t *result) {
    rs_registered_lambda *reg_lambda = get_registered_lambda_by_id(id);
    if (reg_lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    if (reg_lambda->type != RS_LAMBDA_INT) {
        return RS_CALL_WRONGTYPE;
    }
    lambda_int_t lambda = (lambda_int_t) reg_lambda->arg;
    *result = lambda(id);
    return RS_CALL_SUCCESS;
}

int8_t call_lambda_int_by_name(const char *name, rs_int_t *result) {
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    return call_lambda_int(lambda->id, result);
}

int8_t register_lambda_double(const char *name, lambda_double_t lambda, const rs_cache_type_t cache) {
    return register_lambda(name, (lambda_generic_t) lambda, RS_LAMBDA_DOUBLE, cache);
}

int8_t call_lambda_double(const lambda_id_t id, rs_double_t *result) {
    rs_registered_lambda *reg_lambda = get_registered_lambda_by_id(id);
    if (reg_lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    if (reg_lambda->type != RS_LAMBDA_DOUBLE) {
        return RS_CALL_WRONGTYPE;
    }
    lambda_double_t lambda = (lambda_double_t) reg_lambda->arg;
    *result = lambda(id);
    return RS_CALL_SUCCESS;
}

int8_t call_lambda_double_by_name(const char *name, rs_double_t *result) {
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    return call_lambda_double(lambda->id, result);
}

int8_t register_lambda_string(const char *name, lambda_string_t lambda, const rs_cache_type_t cache) {
    return register_lambda(name, (lambda_generic_t) lambda, RS_LAMBDA_STRING, cache);
}

int8_t call_lambda_string(const lambda_id_t id, rs_string_t *result) {
    rs_registered_lambda *reg_lambda = get_registered_lambda_by_id(id);
    if (reg_lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    if (reg_lambda->type != RS_LAMBDA_STRING) {
        return RS_CALL_WRONGTYPE;
    }
    lambda_string_t lambda = (lambda_string_t) reg_lambda->arg;
    *result = lambda(id);
    return RS_CALL_SUCCESS;
}

int8_t call_lambda_string_by_name(const char *name, rs_string_t *result) {
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_CALL_NOTFOUND;
    }
    return call_lambda_string(lambda->id, result);
}

int8_t send_result_lambda_int(const lambda_id_t id, rs_int_t result) {
    rs_registered_lambda *reg_lambda = get_registered_lambda_by_id(id);
    if (reg_lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    if (reg_lambda->type != RS_LAMBDA_INT) {
        return RS_RESULT_WRONGTYPE;
    }
    if (rs_spt_started) {
        rs_packet_lambda_result_int_t *pkt = malloc(sizeof(rs_packet_lambda_result_int_t));
        pkt->result_base.base.ptype = RS_PACKET_RESULT_INT;
        populate_resultbase_from_lambda(&pkt->result_base, reg_lambda);
        pkt->result = result;
        hton_rs_packet_lambda_result_int_t(pkt);
        struct serial_data_packet sdpkt;
        sdpkt.data = (char *) pkt;
        sdpkt.len = sizeof(*pkt);
        spt_send_packet(&rs_sptctx, &sdpkt);
        free(pkt);
    }
    return RS_RESULT_SUCCESS;
}

int8_t send_result_lambda_int_by_name(const char *name, rs_int_t result) {
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    return send_result_lambda_int(lambda->id, result);
}

int8_t send_result_lambda_double(const lambda_id_t id, rs_double_t result) {
    rs_registered_lambda *reg_lambda = get_registered_lambda_by_id(id);
    if (reg_lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    if (reg_lambda->type != RS_LAMBDA_DOUBLE) {
        return RS_RESULT_WRONGTYPE;
    }
    if (rs_spt_started) {
        rs_packet_lambda_result_double_t *pkt = malloc(sizeof(rs_packet_lambda_result_double_t));
        pkt->result_base.base.ptype = RS_PACKET_RESULT_DOUBLE;
        populate_resultbase_from_lambda(&pkt->result_base, reg_lambda);
        pkt->result = result;
        hton_rs_packet_lambda_result_double_t(pkt);
        struct serial_data_packet sdpkt;
        sdpkt.data = (char *) pkt;
        sdpkt.len = sizeof(*pkt);
        spt_send_packet(&rs_sptctx, &sdpkt);
        free(pkt);
    }
    return RS_RESULT_SUCCESS;
}

int8_t send_result_lambda_double_by_name(const char *name, rs_double_t result) {
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    return send_result_lambda_double(lambda->id, result);
}

int8_t send_result_lambda_string(const lambda_id_t id, rs_string_t result) {
    rs_registered_lambda *reg_lambda = get_registered_lambda_by_id(id);
    if (reg_lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    if (reg_lambda->type != RS_LAMBDA_STRING) {
        return RS_RESULT_WRONGTYPE;
    }
    if (rs_spt_started) {
        rs_packet_lambda_result_string_t *pkt = malloc(sizeof(rs_packet_lambda_result_string_t));
        pkt->result_base.base.ptype = RS_PACKET_RESULT_STRING;
        populate_resultbase_from_lambda(&pkt->result_base, reg_lambda);
        pkt->result = result;
        hton_rs_packet_lambda_result_string_t(pkt);
        struct serial_data_packet sdpkt;
        sdpkt.data = (char *) pkt;
        sdpkt.len = sizeof(*pkt);
        spt_send_packet(&rs_sptctx, &sdpkt);
        free(pkt);
    }
    return RS_RESULT_SUCCESS;
}

int8_t send_result_lambda_string_by_name(const char *name, rs_string_t result) {
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        return RS_RESULT_NOTFOUND;
    }
    return send_result_lambda_string(lambda->id, result);
}


int8_t unregister_lambda(const lambda_id_t id) {
    rs_registered_lambda *reg_lambda = get_registered_lambda_by_id(id);
    if (reg_lambda == NULL) {
        return RS_UNREGISTER_NOTFOUND;
    }

    if (rs_spt_started) {
        rs_packet_unregistered_t *pkt = malloc(sizeof(rs_packet_unregistered_t));
        pkt->base.ptype = RS_PACKET_REGISTERED;
        pkt->lambda_id = id;
        strcpy(pkt->name, reg_lambda->name);
        hton_rs_packet_unregistered_t(pkt);
        struct serial_data_packet sdpkt;
        sdpkt.data = (char *) pkt;
        sdpkt.len = sizeof(*pkt);
        spt_send_packet(&rs_sptctx, &sdpkt);
        free(pkt);
    }
    lambda_registry_unregister(id);
    return RS_UNREGISTER_SUCCESS;
}

lambda_id_t get_lambda_id_from_name(const char *name) {
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        return (lambda_id_t) -1;
    }
    return lambda->id;
}

void handle_call_lambda(lambda_id_t id, rs_lambda_type_t expected_type) {
    int8_t call_res;
    if (expected_type == RS_LAMBDA_INT) {
        rs_int_t result;
        call_res = call_lambda_int(id, &result);
        if (call_res == RS_CALL_SUCCESS) {
            send_result_lambda_int(id, result);
            return;
        }
    } else if (expected_type == RS_LAMBDA_DOUBLE) {
        rs_double_t result;
        call_res = call_lambda_double(id, &result);
        if (call_res == RS_CALL_SUCCESS) {
            send_result_lambda_double(id, result);
            return;
        }
    } else if (expected_type == RS_LAMBDA_STRING) {
        rs_string_t result;
        call_res = call_lambda_string(id, &result);
        if (call_res == RS_CALL_SUCCESS) {
            send_result_lambda_string(id, result);
            return;
        }
    } else {
        fprintf(stderr, "Called lambda with id %d but unknown type %d\n", id, expected_type);
        return;
    }
    fprintf(stderr, "Error on lambda call with id %d and expected type %d: code %d\n", id, expected_type, call_res);
    if (rs_spt_started) {
        rs_packet_lambda_result_error_t *pkt = malloc(sizeof(rs_packet_lambda_result_error_t));
        pkt->result_base.base.ptype = RS_PACKET_RESULT_ERROR;
        pkt->result_base.lambda_id = id;
        char *nfname = "unknown";
        strcpy(pkt->result_base.name, nfname);
        pkt->error_code = call_res;
        struct serial_data_packet sdpkt;
        sdpkt.data = (char *) pkt;
        sdpkt.len = sizeof(*pkt);
        spt_send_packet(&rs_sptctx, &sdpkt);
        free(pkt);
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
        printf("Received call by id for lambda id %d with expected type %d\n", mypkt.lambda_id, mypkt.expected_type);
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
        printf("Received call by name for lambda id %s with expected type %d\n", mypkt.name, mypkt.expected_type);
        handle_call_lambda(id, mypkt.expected_type);
    } else {
        fprintf(stderr,
                "Received packet of unknown/unprocessable type %d with size %d\n",
                ptype,
                packet->len);
    }
}

void rs_start() {
    init_lambda_registry();
    serial_io_context_init(&rs_sictx, STDIN_FILENO, STDOUT_FILENO);
    spt_init_context(&rs_sptctx, &rs_sictx, handle_received_packet);
    spt_start(&rs_sptctx);
    rs_spt_started = true;
}

void rs_stop() {
    rs_spt_started = false;
    free_lambda_registry();
    spt_stop(&rs_sptctx);
}