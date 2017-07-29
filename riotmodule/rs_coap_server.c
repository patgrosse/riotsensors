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

#include <net/af.h>
#include <net/sock/udp.h>
#include <coap.h>

#ifdef MICROCOAP_DEBUG
#define ENABLE_DEBUG (1)
#else
#define ENABLE_DEBUG (0)
#endif

#ifndef RIOT_BOARD
#define RIOT_BOARD "native"
#endif

#define COAP_SERVER_PORT (5683)
#define MAIN_QUEUE_SIZE (8)
#define MAX_RESPONSE_LEN 500

#include <debug.h>

#define INITIALIZE_COAP_KEY_VALUE_FROM_OPT(opt) \
    ssize_t split_at = index_of((const char *) (opt)->buf.p, '='); \
    size_t key_len = split_at == -1 ? (opt)->buf.len : (size_t) (split_at); \
    size_t val_len = split_at == -1 ? 0 : (opt)->buf.len - split_at - 1; \
    char key[key_len + 1]; \
    memcpy(key, (opt)->buf.p, key_len); \
    key[key_len] = '\0'; \
    char value[val_len + 1]; \
    memcpy(value, (opt)->buf.p + key_len + 1, val_len); \
    value[val_len] = '\0'

static uint8_t response[MAX_RESPONSE_LEN] = {0};
static char buf[200];
static uint8_t _udp_buf[512];   /* udp read buffer (max udp payload size) */
static uint8_t scratch_raw[1024];      /* microcoap scratch buffer */
static coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};
static char _stack[THREAD_STACKSIZE_MAIN];
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static kernel_pid_t rs_coap_thread_pid = KERNEL_PID_UNDEF;
static bool running = true;

/* import "ifconfig" shell command, used for printing addresses */
extern int _netif_config(int argc, char **argv);

static int handle_get_well_known_core(coap_rw_buffer_t *scratch,
                                      const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                      uint8_t id_hi, uint8_t id_lo);

static int handle_get_riot_board(coap_rw_buffer_t *scratch,
                                 const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                 uint8_t id_hi, uint8_t id_lo);

static int handle_list(coap_rw_buffer_t *scratch,
                       const coap_packet_t *inpkt, coap_packet_t *outpkt,
                       uint8_t id_hi, uint8_t id_lo);

static int handle_call_name(coap_rw_buffer_t *scratch,
                            const coap_packet_t *inpkt, coap_packet_t *outpkt,
                            uint8_t id_hi, uint8_t id_lo);

static int handle_call_id(coap_rw_buffer_t *scratch,
                          const coap_packet_t *inpkt, coap_packet_t *outpkt,
                          uint8_t id_hi, uint8_t id_lo);

void print_ifconfig(void) {
    puts("Configured network interfaces:");
    _netif_config(0, NULL);
}

static void *run_coap_server(void *arg) {
    UNUSED(arg);
    static const sock_udp_ep_t local = {
            .family = AF_INET6,
            .port = COAP_SERVER_PORT
    };
    sock_udp_ep_t remote;

    sock_udp_t sock;

    int rc = sock_udp_create(&sock, &local, NULL, 0);

    if (rc != 0) {
        printf("Error while opening UDP socket!");
        return NULL;
    }

    while (running) {
        DEBUG("Waiting for incoming UDP packet...\n");
        rc = sock_udp_recv(&sock, (char *) _udp_buf, sizeof(_udp_buf),
                           SOCK_NO_TIMEOUT, &remote);
        if (rc < 0) {
            DEBUG("Error in conn_udp_recvfrom(). rc=%u\n", rc);
            continue;
        }

        size_t n = (size_t) rc;

        coap_packet_t pkt;
#ifdef MICROCOAP_DEBUG
        DEBUG("Received packet: ");
        coap_dump(_udp_buf, n, true);
        DEBUG("\n");
#endif

        /* parse UDP packet to CoAP */
        rc = coap_parse(&pkt, _udp_buf, n);
        if (rc != 0) {
            DEBUG("Bad packet rc=%d\n", rc);
        } else {
            coap_packet_t rsppkt;
#ifdef MICROCOAP_DEBUG
            DEBUG("content:\n");
            coap_dumpPacket(&pkt);
#endif
            /* handle CoAP request */
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            /* build reply */
            size_t rsplen = sizeof(_udp_buf);
            rc = coap_build(_udp_buf, &rsplen, &rsppkt);
            if (rc != 0) {
                DEBUG("coap_build failed rc=%d\n", rc);
            } else {
#ifdef MICROCOAP_DEBUG
                DEBUG("Sending packet: ");
                coap_dump(_udp_buf, rsplen, true);
                DEBUG("\n");
                DEBUG("content:\n");
                coap_dumpPacket(&rsppkt);
#endif
                /* send reply via UDP */
                rc = sock_udp_send(&sock, _udp_buf, rsplen, &remote);
                if (rc < 0) {
                    DEBUG("Error sending CoAP reply via udp; %u\n", rc);
                }
            }
        }
    }
    return NULL;
}

void rs_start_coap_server(void) {
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    print_ifconfig();
    rs_coap_thread_pid = thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                                       run_coap_server, NULL, "rs_coap");
}


static const coap_endpoint_path_t path_well_known_core = {2, {".well-known", "core"}};

static const coap_endpoint_path_t path_riot_board = {2, {"riot", "board"}};

static const coap_endpoint_path_t path_list = {1, {"list"}};

static const coap_endpoint_path_t path_call_name = {2, {"call", "name"}};

static const coap_endpoint_path_t path_call_id = {2, {"call", "id"}};

const coap_endpoint_t endpoints[] =
        {
                {COAP_METHOD_GET, handle_get_well_known_core,
                        &path_well_known_core, "ct=40"},
                {COAP_METHOD_GET, handle_get_riot_board,
                        &path_riot_board,      "ct=0"},
                {COAP_METHOD_GET, handle_list,
                        &path_list,            "ct=50"},
                {COAP_METHOD_GET, handle_call_name,
                        &path_call_name,       "ct=50"},
                {COAP_METHOD_GET, handle_call_id,
                        &path_call_id,         "ct=50"},
                /* marks the end of the endpoints array: */
                {(coap_method_t) 0, NULL, NULL, NULL}
        };

static int handle_get_well_known_core(coap_rw_buffer_t *scratch,
                                      const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                      uint8_t id_hi, uint8_t id_lo) {
    char *rsp = (char *) response;
    /* resetting the content of response message */
    memset(response, 0, sizeof(response));
    size_t len = sizeof(response);
    const coap_endpoint_t *ep = endpoints;
    int i;

    len--; // Null-terminated string

    while (NULL != ep->handler) {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }

        if (0 < strlen(rsp)) {
            strncat(rsp, ",", len);
            len--;
        }

        strncat(rsp, "<", len);
        len--;

        for (i = 0; i < ep->path->count; i++) {
            strncat(rsp, "/", len);
            len--;

            strncat(rsp, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }

        strncat(rsp, ">;", len);
        len -= 2;

        strncat(rsp, ep->core_attr, len);
        len -= strlen(ep->core_attr);

        ep++;
    }

    return coap_make_response(scratch, outpkt, (const uint8_t *) rsp,
                              strlen(rsp), id_hi, id_lo, &inpkt->tok,
                              COAP_RSPCODE_CONTENT,
                              COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

static int handle_get_riot_board(coap_rw_buffer_t *scratch,
                                 const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                 uint8_t id_hi, uint8_t id_lo) {
    const char *riot_name = RIOT_BOARD;
    size_t len = strlen(RIOT_BOARD);

    memcpy(response, riot_name, len);

    return coap_make_response(scratch, outpkt, (const uint8_t *) response, len,
                              id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT,
                              COAP_CONTENTTYPE_TEXT_PLAIN);
}

static int handle_list(coap_rw_buffer_t *scratch,
                       const coap_packet_t *inpkt, coap_packet_t *outpkt,
                       uint8_t id_hi, uint8_t id_lo) {
    uint8_t querycount = 0;
    const coap_option_t *opt = coap_findOptions(inpkt, COAP_OPTION_URI_QUERY, &querycount);
    rs_lambda_type_t type = 0;
    for (uint8_t i = 0; i < querycount; i++) {
        INITIALIZE_COAP_KEY_VALUE_FROM_OPT(&opt[i]);
        if (strcmp(key, "type") == 0) {
            type = get_lambda_type_from_string(value);
            break;
        }
    }
    if (type == (rs_lambda_type_t) -1) {
        return coap_make_response(scratch, outpkt, (const uint8_t *) "Unknown lambda type", 20,
                                  id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST,
                                  COAP_CONTENTTYPE_TEXT_PLAIN);
    } else {
        char *rsp = (char *) response;
        /* resetting the content of response message */
        memset(response, 0, sizeof(response));
        size_t len = sizeof(response);
        strncat(rsp, "{\"lambdas\":{", len);
        len -= 12;
        uint8_t count = 0;
        for (lambda_id_t i = 0; i < get_number_of_registered_lambdas(); i++) {
            rs_registered_lambda *lambda = get_registered_lambda_by_id(i);
            if (lambda != NULL) {
                if (type != 0 && lambda->type != type) {
                    continue;
                }
                count++;
                sprintf(buf, "\"%" PRIu8 "\": {\"id\": %" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "}", lambda->id, lambda->id,
                        lambda->name, lambda->type, lambda->cache);
                strncat(rsp, buf, len);
                len -= strlen(buf);
            }
        }
        sprintf(buf, "},\"count\":%" PRIu8 "}", count);
        strncat(rsp, buf, len);
        len -= strlen(buf);
        (void) len;
        return coap_make_response(scratch, outpkt, (const uint8_t *) rsp, strlen(rsp),
                                  id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT,
                                  COAP_CONTENTTYPE_APPLICATION_JSON);
    }
}

static void perform_coap_lambda_call(rs_lambda_type_t type, rs_registered_lambda *lambda) {
    int8_t call_res;
    generic_lambda_return result;
    switch (type) {
        case RS_LAMBDA_INT:
            call_res = call_lambda_int(lambda->id, &result.ret_i);
            if (call_res >= 0) {
                sprintf(buf,
                        "{\"success\": true,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"result\":%" PRId32 "}",
                        lambda->id, lambda->name, lambda->type, lambda->cache, result.ret_i);
            }
            break;
        case RS_LAMBDA_DOUBLE:
            call_res = call_lambda_double(lambda->id, &result.ret_d);
            if (call_res >= 0) {
                sprintf(buf,
                        "{\"success\": true,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"result\":%f}",
                        lambda->id, lambda->name, lambda->type, lambda->cache, result.ret_d);
            }
            break;
        case RS_LAMBDA_STRING:
            call_res = call_lambda_string(lambda->id, &result.ret_s);
            if (call_res >= 0) {
                sprintf(buf,
                        "{\"success\": true,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"result\":\"%s\"}",
                        lambda->id, lambda->name, lambda->type, lambda->cache, result.ret_s);
            }
            break;
        default:
            call_res = RS_CALL_WRONGTYPE;
    }
    if (call_res < 0) {
        sprintf(buf,
                "{\"success\": false,\"lambda\":{\"id\":%" PRIu8 ",\"name\":\"%s\",\"type\":%" PRIu8 ",\"cache\":%" PRIu8 "},\"error\":%" PRId8 "}",
                lambda->id, lambda->name, lambda->type, lambda->cache, call_res);
    }
}

static int handle_call_name(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi,
                            uint8_t id_lo) {
    uint8_t querycount = 0;
    const coap_option_t *opt = coap_findOptions(inpkt, COAP_OPTION_URI_QUERY, &querycount);
    char name[MAX_LAMBDA_NAME_LENGTH];
    name[0] = '\0';
    rs_lambda_type_t type = 0;
    for (uint8_t i = 0; i < querycount; i++) {
        INITIALIZE_COAP_KEY_VALUE_FROM_OPT(&opt[i]);
        if (strcmp(key, "type") == 0) {
            type = get_lambda_type_from_string(value);
        } else if (strcmp(key, "name") == 0) {
            strcpy(name, value);
        }
    }
    if (type == (rs_lambda_type_t) -1 || type == 0) {
        return coap_make_response(scratch, outpkt, (const uint8_t *) "Unknown/missing lambda type", 28,
                                  id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST,
                                  COAP_CONTENTTYPE_TEXT_PLAIN);
    } else if (strlen(name) == 0) {
        return coap_make_response(scratch, outpkt, (const uint8_t *) "Wrong/missing lambda name", 26,
                                  id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST,
                                  COAP_CONTENTTYPE_TEXT_PLAIN);
    } else {
        rs_registered_lambda *lambda = get_registered_lambda_by_name(name);
        if (lambda == NULL) {
            sprintf(buf,
                    "{\"success\": false,\"lambda\":{\"id\":-1,\"name\":\"%s\",\"type\":-1,\"cache\":-1},\"error\":%d}",
                    name, RS_CALL_NOTFOUND);
        } else {
            perform_coap_lambda_call(type, lambda);
        }
        return coap_make_response(scratch, outpkt, (const uint8_t *) buf, strlen(buf), id_hi, id_lo,
                                  &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
    }
}

static int handle_call_id(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi,
                          uint8_t id_lo) {
    uint8_t querycount = 0;
    const coap_option_t *opt = coap_findOptions(inpkt, COAP_OPTION_URI_QUERY, &querycount);
    rs_lambda_type_t type = 0;
    lambda_id_t id = (lambda_id_t) -1;
    for (uint8_t i = 0; i < querycount; i++) {
        INITIALIZE_COAP_KEY_VALUE_FROM_OPT(&opt[i]);
        if (strcmp(key, "type") == 0) {
            type = get_lambda_type_from_string(value);
        } else if (strcmp(key, "id") == 0) {
            char *endparsed;
            long num = strtol(value, &endparsed, 10);
            if (endparsed == value + strlen(value)) {
                id = (lambda_id_t) num;
            }
        }
    }
    if (type == (rs_lambda_type_t) -1 || type == 0) {
        return coap_make_response(scratch, outpkt, (const uint8_t *) "Unknown/missing lambda type", 28,
                                  id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST,
                                  COAP_CONTENTTYPE_TEXT_PLAIN);
    } else if (id == (lambda_id_t) -1) {
        return coap_make_response(scratch, outpkt, (const uint8_t *) "Wrong/missing lambda id", 24,
                                  id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST,
                                  COAP_CONTENTTYPE_TEXT_PLAIN);
    } else {
        rs_registered_lambda *lambda = get_registered_lambda_by_id(id);
        if (lambda == NULL) {
            sprintf(buf,
                    "{\"success\": false,\"lambda\":{\"id\":%d,\"name\":\"unknown\",\"type\":-1,\"cache\":-1},\"error\":%d}",
                    id, RS_CALL_NOTFOUND);
        } else {
            perform_coap_lambda_call(type, lambda);
        }
        return coap_make_response(scratch, outpkt, (const uint8_t *) buf, strlen(buf), id_hi, id_lo,
                                  &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
    }
}

#else
typedef int make_iso_compilers_happy;
#endif
