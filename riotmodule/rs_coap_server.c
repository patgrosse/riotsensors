/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RS_NO_COAP

#include <rs_coap_server.h>

#include <rs_coap_utils.h>
#include <rs_packets.h>
#include <lambda_registry.h>
#include <unused.h>
#include <rs.h>

#include <net/sock/udp.h>
#include <net/gcoap.h>

#ifndef RIOT_BOARD
#define RIOT_BOARD "native"
#endif

#define MAIN_QUEUE_SIZE (8)
#define GCOAP_BUF_LENGTH 500

static uint8_t buf[GCOAP_BUF_LENGTH];
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* import "ifconfig" shell command, used for printing addresses */
extern int _netif_config(int argc, char **argv);

static ssize_t handle_get_riot_board(coap_pkt_t *pkt, uint8_t *buf, size_t len);

static ssize_t handle_list(coap_pkt_t *pkt, uint8_t *buf, size_t len);

static ssize_t handle_call_name(coap_pkt_t *pkt, uint8_t *buf, size_t len);

static ssize_t handle_call_id(coap_pkt_t *pkt, uint8_t *buf, size_t len);

void print_ifconfig(void) {
    puts("Configured network interfaces:");
    _netif_config(0, NULL);
}

static const coap_resource_t _resources[] = {
        {"/board",     COAP_GET, handle_get_riot_board},
        {"/call/id",   COAP_GET, handle_call_id},
        {"/call/name", COAP_GET, handle_call_name},
        {"/list",      COAP_GET, handle_list},
};

static gcoap_listener_t _listener = {
        (coap_resource_t *) &_resources[0],
        sizeof(_resources) / sizeof(_resources[0]),
        NULL
};

void rs_start_coap_server(void) {
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    print_ifconfig();
    gcoap_register_listener(&_listener);
}

static ssize_t handle_get_riot_board(coap_pkt_t *pkt, uint8_t *buf2, size_t len2) {
    UNUSED(buf2);
    UNUSED(len2);
    gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_CONTENT);

    const char *riot_name = RIOT_BOARD;
    size_t board_len = strlen(RIOT_BOARD);
    memcpy(pkt->payload, riot_name, board_len);

    return gcoap_finish(pkt, board_len, COAP_FORMAT_TEXT);
}

static ssize_t handle_list(coap_pkt_t *pkt, uint8_t *buf2, size_t len2) {
    UNUSED(buf2);
    UNUSED(len2);
    rs_lambda_type_t type = 0;
    struct coap_queries queries;
    split_coap_query(pkt->payload_len, (const char *) pkt->payload, &queries);
    struct coap_query *query = queries.first;
    while (query != NULL) {
        if (strcmp(query->key, "type") == 0) {
            type = get_lambda_type_from_string(query->value);
            break;
        }
        query = query->next;
    }
    free_coap_queries(&queries);
    if (type == (rs_lambda_type_t) -1) {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_BAD_REQUEST);
        static const char *resp_text = "Unknown lambda type";
        size_t resp_text_len = strlen(resp_text);
        memcpy(pkt->payload, resp_text, resp_text_len);
        return gcoap_finish(pkt, resp_text_len, COAP_FORMAT_TEXT);
    } else {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_CONTENT);
        strcpy((char *) pkt->payload, "{\"lambdas\":{");
        size_t len = 12;
        uint8_t count = 0;
        for (lambda_id_t i = 0; i < get_number_of_registered_lambdas(); i++) {
            rs_registered_lambda *lambda = get_registered_lambda_by_id(i);
            if (lambda != NULL) {
                if (type != 0 && lambda->type != type) {
                    continue;
                }
                count++;
                len += sprintf((char *) pkt->payload + len,
                                      "\"%" PRIu8 "\": {\"id\": %" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "}",
                                      lambda->id, lambda->id, lambda->name, lambda->type, lambda->cache);
            }
        }
        len += sprintf((char *) pkt->payload + len, "},\"count\":%" PRIu8 "}", count);
        return gcoap_finish(pkt, len, COAP_FORMAT_JSON);
    }
}

static int perform_coap_lambda_call(rs_lambda_type_t type, rs_registered_lambda *lambda, char *buf) {
    int8_t call_res;
    generic_lambda_return result;
    switch (type) {
        case RS_LAMBDA_INT:
            call_res = call_lambda_int(lambda->id, &result.ret_i);
            if (call_res >= 0) {
                return sprintf(buf,
                               "{\"success\": true,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"result\":%" PRId32 "}",
                               lambda->id, lambda->name, lambda->type, lambda->cache, result.ret_i);
            }
            break;
        case RS_LAMBDA_DOUBLE:
            call_res = call_lambda_double(lambda->id, &result.ret_d);
            if (call_res >= 0) {
                return sprintf(buf,
                               "{\"success\": true,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"result\":%f}",
                               lambda->id, lambda->name, lambda->type, lambda->cache, result.ret_d);
            }
            break;
        case RS_LAMBDA_STRING:
            call_res = call_lambda_string(lambda->id, &result.ret_s);
            if (call_res >= 0) {
                return sprintf(buf,
                               "{\"success\": true,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"result\":\"%s\"}",
                               lambda->id, lambda->name, lambda->type, lambda->cache, result.ret_s);
            }
            break;
        default:
            call_res = RS_CALL_WRONGTYPE;
    }
    if (call_res < 0) {
        return sprintf(buf,
                       "{\"success\": false,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"error\":%" PRId8 "}",
                       lambda->id, lambda->name, lambda->type, lambda->cache, call_res);
    }
    return -1;
}

static ssize_t handle_call_name(coap_pkt_t *pkt, uint8_t *buf2, size_t len2) {
    UNUSED(buf2);
    UNUSED(len2);
    char name[MAX_LAMBDA_NAME_LENGTH];
    name[0] = '\0';
    rs_lambda_type_t type = 0;
    struct coap_queries queries;
    split_coap_query(pkt->payload_len, (const char *) pkt->payload, &queries);
    struct coap_query *query = queries.first;
    while (query != NULL) {
        printf("query %s %s\n", query->key, query->value);
        if (strcmp(query->key, "type") == 0) {
            type = get_lambda_type_from_string(query->value);
        } else if (strcmp(query->key, "name") == 0) {
            strcpy(name, query->value);
        }
        query = query->next;
    }
    free_coap_queries(&queries);
    if (type == (rs_lambda_type_t) -1 || type == 0) {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_BAD_REQUEST);
        static const char *resp_text = "Unknown/missing lambda type";
        size_t resp_text_len = strlen(resp_text);
        memcpy(pkt->payload, resp_text, resp_text_len);
        return gcoap_finish(pkt, resp_text_len, COAP_FORMAT_TEXT);
    } else if (strlen(name) == 0) {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_BAD_REQUEST);
        static const char *resp_text = "Wrong/missing lambda name";
        size_t resp_text_len = strlen(resp_text);
        memcpy(pkt->payload, resp_text, resp_text_len);
        return gcoap_finish(pkt, resp_text_len, COAP_FORMAT_TEXT);
    } else {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_CONTENT);
        rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
        int len;
        if (lambda == NULL) {
            len = sprintf((char *) pkt->payload,
                          "{\"success\": false,\"lambda\":{\"id\":-1,\"name\":\"%s\",\"type\":-1,\"cache\":-1},\"error\":%d}",
                          name, RS_CALL_NOTFOUND);
        } else {
            len = perform_coap_lambda_call(type, lambda, (char *) pkt->payload);
        }
        return gcoap_finish(pkt, (size_t) len, COAP_FORMAT_JSON);
    }
}

static ssize_t handle_call_id(coap_pkt_t *pkt, uint8_t *buf2, size_t len2) {
    UNUSED(buf2);
    UNUSED(len2);
    rs_lambda_type_t type = 0;
    lambda_id_t id = (lambda_id_t) -1;
    struct coap_queries queries;
    split_coap_query(pkt->payload_len, (const char *) pkt->payload, &queries);
    struct coap_query *query = queries.first;
    while (query != NULL) {
        if (strcmp(query->key, "type") == 0) {
            type = get_lambda_type_from_string(query->value);
        } else if (strcmp(query->key, "id") == 0) {
            char *endparsed;
            long num = strtol(query->value, &endparsed, 10);
            if (endparsed == query->value + strlen(query->value)) {
                id = (lambda_id_t) num;
            }
        }
        query = query->next;
    }
    free_coap_queries(&queries);
    if (type == (rs_lambda_type_t) -1 || type == 0) {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_BAD_REQUEST);
        static const char *resp_text = "Unknown/missing lambda type";
        size_t resp_text_len = strlen(resp_text);
        memcpy(pkt->payload, resp_text, resp_text_len);
        return gcoap_finish(pkt, resp_text_len, COAP_FORMAT_TEXT);
    } else if (id == (lambda_id_t) -1) {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_BAD_REQUEST);
        static const char *resp_text = "Wrong/missing lambda id";
        size_t resp_text_len = strlen(resp_text);
        memcpy(pkt->payload, resp_text, resp_text_len);
        return gcoap_finish(pkt, resp_text_len, COAP_FORMAT_TEXT);
    } else {
        gcoap_resp_init(pkt, buf, GCOAP_BUF_LENGTH, COAP_CODE_CONTENT);
        rs_registered_lambda *lambda = get_registered_lambda_by_id(id);
        int len;
        if (lambda == NULL) {
            len = sprintf((char *) pkt->payload,
                          "{\"success\": false,\"lambda\":{\"id\":%d,\"name\":\"unknown\",\"type\":-1,\"cache\":-1},\"error\":%d}",
                          id, RS_CALL_NOTFOUND);
        } else {
            len = perform_coap_lambda_call(type, lambda, (char *) pkt->payload);
        }
        return gcoap_finish(pkt, (size_t) len, COAP_FORMAT_JSON);
    }
}

#else
typedef int make_iso_compilers_happy;
#endif
