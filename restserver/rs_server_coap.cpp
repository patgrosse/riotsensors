/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_server_coap.h>
#include <event.h>

extern "C" {
#include <unused.h>
}

static rs_lambda_type_t coap_parse_type(coap_pdu_t *request) {
    coap_opt_iterator_t opt_iter;
    coap_opt_filter_t f = {};
    coap_opt_t *q;
    coap_option_filter_clear(f);
    coap_option_setb(f, COAP_OPTION_URI_QUERY);
    coap_option_iterator_init(request, &opt_iter, f);
    while ((q = coap_option_next(&opt_iter)) != nullptr) {
        unsigned char *value = coap_opt_value(q);
        size_t length = coap_opt_length(q);
        if (!memcmp("type", value, (size_t) std::min(4, (const int &) length))) {
            size_t arg_len = length - 5;
            std::string typestr((char *) (value + 5), arg_len);
            return get_lambda_type_from_string(typestr);
        }
    }
    return 0;
}

static void coap_transfer_data_from_response_info(coap_pdu_t *response, rest_response_info answer) {
    unsigned char buf[3];
    coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_JSON),
                    buf);
    response->hdr->code = COAP_RESPONSE_CODE((unsigned int) answer.first);
    coap_add_data(response, (unsigned int) answer.second.length(), (unsigned char *) answer.second.c_str());
}

static void coap_answer_with_unknown_type(coap_pdu_t *response) {
    static std::string unknown_type_text = "Unknown lambda type";
    response->hdr->code = COAP_RESPONSE_CODE(400);
    coap_add_data(response, (unsigned int) unknown_type_text.length(),
                  (unsigned char *) unknown_type_text.c_str());
}

void RiotsensorsCoAPProvider::handleCallById(coap_context_t *ctx, struct coap_resource_t *resource,
                                             const coap_endpoint_t *local_interface, coap_address_t *peer,
                                             coap_pdu_t *request, str *token, coap_pdu_t *response) {
    UNUSED(ctx);
    UNUSED(resource);
    UNUSED(local_interface);
    UNUSED(peer);
    UNUSED(token);
    coap_opt_iterator_t opt_iter;
    coap_opt_filter_t f = {};
    coap_opt_t *q;
    coap_option_filter_clear(f);
    coap_option_setb(f, COAP_OPTION_URI_QUERY);
    coap_option_iterator_init(request, &opt_iter, f);
    bool type_found = false;
    bool id_found = false;
    rs_lambda_type_t type = 0;
    lambda_id_t id = 0;
    while ((q = coap_option_next(&opt_iter)) != nullptr) {
        unsigned char *value = coap_opt_value(q);
        size_t length = coap_opt_length(q);
        if (!memcmp("type", value, (size_t) std::min(4, (const int &) length))) {
            size_t arg_len = length - 5;
            std::string typestr((char *) (value + 5), arg_len);
            type = get_lambda_type_from_string(typestr);
            type_found = true;
        } else if (!memcmp("id", coap_opt_value(q), (size_t) std::min(2, (const int &) length))) {
            size_t arg_len = length - 3;
            std::string idstr((char *) (value + 3), arg_len);
            try {
                id = (lambda_id_t) std::stoi(idstr);
            } catch (std::invalid_argument e) {
                id = (lambda_id_t) -1;
            }
            id_found = true;
        }
    }
    if (!id_found) {
        static std::string missing_id_text = "Missing id query parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_id_text.length(),
                      (unsigned char *) missing_id_text.c_str());
        return;
    }
    if (!type_found) {
        static std::string missing_type_text = "Missing type query parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_type_text.length(),
                      (unsigned char *) missing_type_text.c_str());
        return;
    }
    if (id == (lambda_id_t) -1) {
        static std::string missing_id_text = "Illegal id parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_id_text.length(),
                      (unsigned char *) missing_id_text.c_str());
        return;
    }
    if (type == (rs_lambda_type_t) -1) {
        static std::string missing_type_text = "Illegal type parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_type_text.length(),
                      (unsigned char *) missing_type_text.c_str());
        return;
    }
    rest_response_info answer = RiotsensorsRESTHandler::handleCallById(type, id);
    coap_transfer_data_from_response_info(response, answer);
}

void RiotsensorsCoAPProvider::handleCallByName(coap_context_t *ctx, struct coap_resource_t *resource,
                                               const coap_endpoint_t *local_interface, coap_address_t *peer,
                                               coap_pdu_t *request, str *token, coap_pdu_t *response) {
    UNUSED(ctx);
    UNUSED(resource);
    UNUSED(local_interface);
    UNUSED(peer);
    UNUSED(token);
    coap_opt_iterator_t opt_iter;
    coap_opt_filter_t f = {};
    coap_opt_t *q;
    coap_option_filter_clear(f);
    coap_option_setb(f, COAP_OPTION_URI_QUERY);
    coap_option_iterator_init(request, &opt_iter, f);
    bool type_found = false;
    bool name_found = false;
    rs_lambda_type_t type = 0;
    char name[MAX_LAMBDA_NAME_LENGTH + 1];
    while ((q = coap_option_next(&opt_iter)) != nullptr) {
        unsigned char *value = coap_opt_value(q);
        size_t length = coap_opt_length(q);
        if (!memcmp("type", value, (size_t) std::min(4, (const int &) length))) {
            size_t arg_len = length - 5;
            std::string typestr((char *) (value + 5), arg_len);
            type = get_lambda_type_from_string(typestr);
            type_found = true;
        } else if (!memcmp("name", value, (size_t) std::min(4, (const int &) length))) {
            size_t arg_len = length - 5;
            memcpy(name, value + 5, (size_t) std::min(MAX_LAMBDA_NAME_LENGTH,
                                                      (const int &) arg_len));
            name[arg_len] = '\0';
            name_found = true;
        }
    }
    if (!name_found) {
        static std::string missing_name_text = "Missing name query parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_name_text.length(),
                      (unsigned char *) missing_name_text.c_str());
        return;
    }
    if (!type_found) {
        static std::string missing_type_text = "Missing type query parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_type_text.length(),
                      (unsigned char *) missing_type_text.c_str());
        return;
    }
    if (strlen(name) == 0) {
        static std::string missing_id_text = "Illegal name parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_id_text.length(),
                      (unsigned char *) missing_id_text.c_str());
        return;
    }
    if (type == (rs_lambda_type_t) -1) {
        static std::string missing_type_text = "Illegal type parameter";
        response->hdr->code = COAP_RESPONSE_CODE(400);
        coap_add_data(response, (unsigned int) missing_type_text.length(),
                      (unsigned char *) missing_type_text.c_str());
        return;
    }
    rest_response_info answer = RiotsensorsRESTHandler::handleCallByName(type, name);
    coap_transfer_data_from_response_info(response, answer);
}

void RiotsensorsCoAPProvider::handleList(coap_context_t *ctx, struct coap_resource_t *resource,
                                         const coap_endpoint_t *local_interface, coap_address_t *peer,
                                         coap_pdu_t *request, str *token, coap_pdu_t *response) {
    UNUSED(ctx);
    UNUSED(resource);
    UNUSED(local_interface);
    UNUSED(peer);
    UNUSED(token);
    rs_lambda_type_t type = coap_parse_type(request);
    if (type == (rs_lambda_type_t) -1) {
        coap_answer_with_unknown_type(response);
        return;
    }
    rest_response_info answer = RiotsensorsRESTHandler::handleList(type);
    coap_transfer_data_from_response_info(response, answer);
}

void RiotsensorsCoAPProvider::handleCache(coap_context_t *ctx, struct coap_resource_t *resource,
                                          const coap_endpoint_t *local_interface, coap_address_t *peer,
                                          coap_pdu_t *request, str *token, coap_pdu_t *response) {
    UNUSED(ctx);
    UNUSED(resource);
    UNUSED(local_interface);
    UNUSED(peer);
    UNUSED(token);
    rs_lambda_type_t type = coap_parse_type(request);
    if (type == (rs_lambda_type_t) -1) {
        coap_answer_with_unknown_type(response);
        return;
    }
    rest_response_info answer = RiotsensorsRESTHandler::handleCache(type);
    coap_transfer_data_from_response_info(response, answer);
}

void RiotsensorsCoAPProvider::handleKill(coap_context_t *ctx, struct coap_resource_t *resource,
                                         const coap_endpoint_t *local_interface, coap_address_t *peer,
                                         coap_pdu_t *request, str *token, coap_pdu_t *response) {
    UNUSED(ctx);
    UNUSED(resource);
    UNUSED(local_interface);
    UNUSED(peer);
    UNUSED(request);
    UNUSED(token);
    static std::string kill_text = "Successfully killed server";
    response->hdr->code = COAP_RESPONSE_CODE(200);
    coap_add_data(response, (unsigned int) kill_text.length(),
                  (unsigned char *) kill_text.c_str());
}

void handle_read_event(int fd, short what, void *ctx) {
    UNUSED(fd);
    UNUSED(what);
    coap_read((coap_context_t *) ctx);
}

void *startCoAPServer(void *thread_ctx) {
    UNUSED(thread_ctx);

    coap_context_t *ctx;
    coap_address_t serv_addr;
    coap_resource_t *callbyid_resource;
    coap_resource_t *callbyname_resource;
    coap_resource_t *handlelist_resource;
    coap_resource_t *handlecache_resource;
    coap_resource_t *kill_resource;

    /* Prepare the CoAP server socket */
    coap_address_init(&serv_addr);
    serv_addr.addr.sin.sin_family = AF_INET;
    serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
    serv_addr.addr.sin.sin_port = htons(5683); //default port
    ctx = coap_new_context(&serv_addr);
    if (!ctx) {
        exit(EXIT_FAILURE);
    }

    /* Initialize the resources */
    callbyid_resource = coap_resource_init((unsigned char *) "call/id", 7, 0);
    callbyname_resource = coap_resource_init((unsigned char *) "call/name", 9, 0);
    handlelist_resource = coap_resource_init((unsigned char *) "list", 4, 0);
    handlecache_resource = coap_resource_init((unsigned char *) "cache", 5, 0);
    kill_resource = coap_resource_init((unsigned char *) "kill", 4, 0);

    /* Register handler */
    coap_register_handler(callbyid_resource, COAP_REQUEST_GET, RiotsensorsCoAPProvider::handleCallById);
    coap_register_handler(callbyname_resource, COAP_REQUEST_GET, RiotsensorsCoAPProvider::handleCallByName);
    coap_register_handler(handlelist_resource, COAP_REQUEST_GET, RiotsensorsCoAPProvider::handleList);
    coap_register_handler(handlecache_resource, COAP_REQUEST_GET, RiotsensorsCoAPProvider::handleCache);
    coap_register_handler(kill_resource, COAP_REQUEST_GET, RiotsensorsCoAPProvider::handleKill);

    /* Add resources */
    coap_add_resource(ctx, callbyid_resource);
    coap_add_resource(ctx, callbyname_resource);
    coap_add_resource(ctx, handlelist_resource);
    coap_add_resource(ctx, handlecache_resource);
    coap_add_resource(ctx, kill_resource);

    struct event_base *ev_base = event_base_new();
    struct event *ev_cmd = event_new(ev_base, ctx->sockfd, EV_READ | EV_PERSIST,
                                     handle_read_event, ctx);
    event_add(ev_cmd, NULL);
    event_base_dispatch(ev_base);
    event_base_free(ev_base);
    event_free(ev_cmd);
    return NULL;
}
