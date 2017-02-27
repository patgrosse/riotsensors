/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_connector.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include <serial_io.h>
#include <spt.h>
#include <unused.h>
#include <spt_logger.h>
#include <tty_utils.h>
#include <rs_packets.h>
#include <lambda_registry.h>
#include <errno.h>
#include <time.h>

struct serial_io_context linux_sictx;
struct spt_context linux_sptctx;

pthread_mutex_t accessing_registry = PTHREAD_MUTEX_INITIALIZER;

void handle_received_packet(struct spt_context *sptctx, struct serial_data_packet *packet) {
    UNUSED(sptctx);
    if (sptctx->log_in_line) {
        putchar('\n');
    }
    rs_packet_type_t ptype;
    if (packet->len < sizeof(ptype)) {
        fprintf(stderr, "Packet with size %d is too small for packet type detection (min size %d)\n", packet->len,
                (int) sizeof(ptype));
    } else {
        ptype = (rs_packet_type_t) *packet->data;
        if (ptype == RS_PACKET_REGISTERED) {
            if (packet->len != sizeof(rs_packet_registered_t)) {
                fprintf(stderr,
                        "Packet with size %d has the wrong size for packet type rs_packet_registered_t (size %d)\n",
                        packet->len,
                        (int) sizeof(rs_packet_registered_t));
            } else {
                rs_packet_registered_t mypkt;
                memcpy(&mypkt, packet->data, sizeof(rs_packet_registered_t));
                ntoh_rs_packet_registered_t(&mypkt);
                rs_linux_registered_lambda *arg = malloc(sizeof(rs_linux_registered_lambda));
                arg->data_cached = false;
                pthread_cond_init(&arg->wait_result, NULL);
                int8_t res = lambda_registry_register(mypkt.name, mypkt.ltype, mypkt.cache, arg);
                if (res < 0) {
                    fprintf(stderr, "Error while registering lambda with name %s and type %d: code %d\n", mypkt.name,
                            mypkt.ltype, res);
                } else {
                    spt_log_msg("packet", "Registered lambda with name %s and type %d: id %d\n", mypkt.name,
                                mypkt.ltype, res);
                }
            }
        } else if (ptype == RS_PACKET_UNREGISTERED) {
            if (packet->len != sizeof(rs_packet_unregistered_t)) {
                fprintf(stderr,
                        "Packet with size %d has the wrong size for packet type rs_packet_unregistered_t (size %d)\n",
                        packet->len,
                        (int) sizeof(rs_packet_unregistered_t));
            } else {
                rs_packet_unregistered_t mypkt;
                memcpy(&mypkt, packet->data, sizeof(rs_packet_unregistered_t));
                ntoh_rs_packet_unregistered_t(&mypkt);
                rs_registered_lambda *lambda = get_registered_lambda_by_id(mypkt.lambda_id);
                if (lambda == NULL) {
                    fprintf(stderr, "Error while unregistering packet with id %d: lambda unknown\n", mypkt.lambda_id);
                } else {
                    rs_linux_registered_lambda *arg = lambda->arg;
                    pthread_cond_destroy(&arg->wait_result);
                    int8_t res = lambda_registry_unregister(mypkt.lambda_id);
                    if (res == RS_UNREGISTER_SUCCESS) {
                        spt_log_msg("packet", "Unregistered lambda with id %d\n", mypkt.lambda_id);
                    } else {
                        fprintf(stderr, "Error while unregistering packet with id %d: code %d\n", mypkt.lambda_id, res);
                    }
                }
            }
        } else if (ptype == RS_PACKET_RESULT_INT) {
            if (packet->len != sizeof(rs_packet_lambda_result_int_t)) {
                fprintf(stderr,
                        "Packet with size %d has the wrong size for packet type rs_packet_lambda_result_int_t (size %d)\n",
                        packet->len,
                        (int) sizeof(rs_packet_lambda_result_int_t));
            } else {
                rs_packet_lambda_result_int_t mypkt;
                memcpy(&mypkt, packet->data, sizeof(rs_packet_lambda_result_int_t));
                ntoh_rs_packet_lambda_result_int_t(&mypkt);
                rs_registered_lambda *lambda = get_registered_lambda_by_id(mypkt.result_base.lambda_id);
                if (lambda == NULL) {
                    fprintf(stderr, "Error while processing int result packet of lambda with id %d: lambda unknown\n",
                            mypkt.result_base.lambda_id);
                } else {
                    rs_linux_registered_lambda *arg = lambda->arg;
                    arg->ret.ret_i = mypkt.result;
                    arg->data_cached = true;
                    pthread_cond_broadcast(&arg->wait_result);
                    spt_log_msg("packet", "Received int result of lambda with id %d\n", mypkt.result_base.lambda_id);
                }
            }
        } else if (ptype == RS_PACKET_RESULT_DOUBLE) {
            if (packet->len != sizeof(rs_packet_lambda_result_double_t)) {
                fprintf(stderr,
                        "Packet with size %d has the wrong size for packet type rs_packet_lambda_result_double_t (size %d)\n",
                        packet->len,
                        (int) sizeof(rs_packet_lambda_result_double_t));
            } else {
                rs_packet_lambda_result_double_t mypkt;
                memcpy(&mypkt, packet->data, sizeof(rs_packet_lambda_result_double_t));
                ntoh_rs_packet_lambda_result_double_t(&mypkt);
                rs_registered_lambda *lambda = get_registered_lambda_by_id(mypkt.result_base.lambda_id);
                if (lambda == NULL) {
                    fprintf(stderr,
                            "Error while processing double result packet of lambda with id %d: lambda unknown\n",
                            mypkt.result_base.lambda_id);
                } else {
                    rs_linux_registered_lambda *arg = lambda->arg;
                    arg->ret.ret_d = mypkt.result;
                    arg->data_cached = true;
                    pthread_cond_broadcast(&arg->wait_result);
                    spt_log_msg("packet", "Received double result of lambda with id %d\n", mypkt.result_base.lambda_id);
                }
            }
        } else if (ptype == RS_PACKET_RESULT_STRING) {
            if (packet->len < sizeof(rs_packet_lambda_result_string_t)) {
                fprintf(stderr,
                        "Packet with size %d has the wrong size for packet type rs_packet_lambda_result_string_t (min size %d)\n",
                        packet->len,
                        (int) sizeof(rs_packet_lambda_result_string_t));
            } else {
                rs_packet_lambda_result_string_t *mypkt = malloc(packet->len);
                memcpy(mypkt, packet->data, packet->len);
                ntoh_rs_packet_lambda_result_string_t(mypkt);
                rs_registered_lambda *lambda = get_registered_lambda_by_id(mypkt->result_base.lambda_id);
                if (lambda == NULL) {
                    fprintf(stderr,
                            "Error while processing string result packet of lambda with id %d: lambda unknown\n",
                            mypkt->result_base.lambda_id);
                } else {
                    rs_linux_registered_lambda *arg = lambda->arg;
                    if (arg->data_cached) {
                        free(arg->ret.ret_s);
                    }
                    char *result = malloc(mypkt->result_length);
                    memcpy(result, &mypkt->result, mypkt->result_length);
                    arg->ret.ret_s = result;
                    arg->data_cached = true;
                    pthread_cond_broadcast(&arg->wait_result);
                    spt_log_msg("packet", "Received string result of lambda with id %d\n",
                                mypkt->result_base.lambda_id);
                }
            }
        } else if (ptype == RS_PACKET_RESULT_ERROR) {
            if (packet->len != sizeof(rs_packet_lambda_result_error_t)) {
                fprintf(stderr,
                        "Packet with size %d has the wrong size for packet type rs_packet_lambda_result_error_t (size %d)\n",
                        packet->len,
                        (int) sizeof(rs_packet_lambda_result_error_t));
            } else {
                rs_packet_lambda_result_error_t mypkt;
                memcpy(&mypkt, packet->data, sizeof(rs_packet_lambda_result_error_t));
                ntoh_rs_packet_lambda_result_error_t(&mypkt);
                rs_registered_lambda *lambda = get_registered_lambda_by_id(mypkt.result_base.lambda_id);
                if (lambda == NULL) {
                    fprintf(stderr,
                            "Error while processing error result packet of lambda with id %d: lambda unknown\n",
                            mypkt.result_base.lambda_id);
                } else {
                    rs_linux_registered_lambda *arg = lambda->arg;
                    arg->last_call_error = mypkt.error_code;
                    pthread_cond_broadcast(&arg->wait_result);
                    spt_log_msg("packet", "Received error result of lambda with id %d: code %d\n",
                                mypkt.result_base.lambda_id, mypkt.error_code);
                }
            }
        } else {
            fprintf(stderr,
                    "Received packet of unknown/unprocessable type %d with size %d\n",
                    ptype,
                    packet->len);
        }
    }
    if (sptctx->log_in_line) {
        spt_log_msg(SPT_LOG_STANDARD_CATEGORY, "");
    }
}

int rs_linux_start(const char *serial_file) {
    int serialfd = connect_serial(serial_file);
    if (serialfd < 0) {
        return -1;
    }
    if (init_serial_connection(serialfd) != 0) {
        return -1;
    }
    init_lambda_registry();
    serial_io_context_init(&linux_sictx, serialfd, serialfd);
    spt_init_context(&linux_sptctx, &linux_sictx, handle_received_packet);
    spt_log_msg("main", "Starting SPT...\n");
    spt_start(&linux_sptctx);
    return 0;
}

int rs_linux_stop() {
    spt_stop(&linux_sptctx);
    free_lambda_registry();
    return 0;
}

int8_t wait_lambda_result(rs_registered_lambda *lambda, generic_lambda_return *result) {
    rs_linux_registered_lambda *arg = lambda->arg;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    spec.tv_sec += 1;
    if (pthread_cond_timedwait(&arg->wait_result, &accessing_registry, &spec) == ETIMEDOUT) {
        if (arg->last_call_error != 0) {
            int8_t error_code = arg->last_call_error;
            arg->last_call_error = 0;
            pthread_mutex_unlock(&accessing_registry);
            return error_code;
        }
        if (lambda->cache == RS_CACHE_ON_TIMEOUT) {
            if (arg->data_cached) {
                *result = arg->ret;
                pthread_mutex_unlock(&accessing_registry);
                return RS_CALL_CACHE_TIMEOUT;
            } else {
                pthread_mutex_unlock(&accessing_registry);
                return RS_CALL_CACHE_TIMEOUT_EMPTY;
            }
        } else {
            pthread_mutex_unlock(&accessing_registry);
            return RS_CALL_TIMEOUT;
        }
    }
    memcpy(result, &arg->ret, sizeof(generic_lambda_return));
    pthread_mutex_unlock(&accessing_registry);
    return RS_CALL_SUCCESS;
}

int8_t check_lambda_cache(rs_registered_lambda *lambda, generic_lambda_return *result) {
    rs_linux_registered_lambda *arg = lambda->arg;
    switch (lambda->cache) {
        case RS_CACHE_NO_CACHE:
            return RS_CALL_SUCCESS;
        case RS_CACHE_CALL_ONCE:
            if (arg->data_cached) {
                *result = arg->ret;
                return RS_CALL_CACHE;
            } else {
                return RS_CALL_SUCCESS;
            }
        case RS_CACHE_ONLY:
            if (arg->data_cached) {
                *result = arg->ret;
                return RS_CALL_CACHE;
            } else {
                return RS_CALL_CACHE_EMPTY;
            }
        default:
            return RS_CALL_SUCCESS;
    }
}

int8_t call_lambda_by_id(lambda_id_t id, rs_lambda_type_t expected_type, generic_lambda_return *result) {
    UNUSED(result);
    pthread_mutex_lock(&accessing_registry);
    rs_registered_lambda *lambda = get_registered_lambda_by_id(id);
    if (lambda == NULL) {
        pthread_mutex_unlock(&accessing_registry);
        return RS_CALL_NOTFOUND;
    }
    if (lambda->type != expected_type) {
        pthread_mutex_unlock(&accessing_registry);
        return RS_CALL_WRONGTYPE;
    }
    int8_t cache_result = check_lambda_cache(lambda, result);
    if (cache_result != 0) {
        pthread_mutex_unlock(&accessing_registry);
        return cache_result;
    }
    spt_log_msg("packet", "Calling for lambda by ID with ID %d and expected type %d...\n", id, expected_type);
    rs_packet_call_by_id_t *mypkt = malloc(sizeof(rs_packet_call_by_id_t));
    mypkt->base.ptype = RS_PACKET_CALL_BY_ID;
    mypkt->lambda_id = id;
    mypkt->expected_type = expected_type;
    hton_rs_packet_call_by_id_t(mypkt);
    struct serial_data_packet pkt;
    pkt.data = (char *) mypkt;
    pkt.len = sizeof(*mypkt);
    spt_send_packet(&linux_sptctx, &pkt);
    free(mypkt);
    return wait_lambda_result(lambda, result);
}

int8_t call_lambda_by_name(const char *name, rs_lambda_type_t expected_type, generic_lambda_return *result) {
    UNUSED(result);
    pthread_mutex_lock(&accessing_registry);
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
    if (lambda == NULL) {
        pthread_mutex_unlock(&accessing_registry);
        return RS_CALL_NOTFOUND;
    }
    if (lambda->type != expected_type) {
        pthread_mutex_unlock(&accessing_registry);
        return RS_CALL_WRONGTYPE;
    }
    int8_t cache_result = check_lambda_cache(lambda, result);
    if (cache_result != 0) {
        pthread_mutex_unlock(&accessing_registry);
        return cache_result;
    }
    spt_log_msg("packet", "Calling for lambda by name with name %s and expected type %d...\n", name, expected_type);
    rs_packet_call_by_name_t *mypkt = malloc(sizeof(rs_packet_call_by_name_t));
    mypkt->base.ptype = RS_PACKET_CALL_BY_NAME;
    strcpy(mypkt->name, name);
    mypkt->expected_type = expected_type;
    hton_rs_packet_call_by_name_t(mypkt);
    struct serial_data_packet pkt;
    pkt.data = (char *) mypkt;
    pkt.len = sizeof(*mypkt);
    spt_send_packet(&linux_sptctx, &pkt);
    free(mypkt);
    return wait_lambda_result(lambda, result);
}
