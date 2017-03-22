/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_server.h>

#include <rs_rest.h>
#include <rs_server_coap.h>
#include <rs_server_http.h>
#include <spt_logger.h>

static pthread_t http_thread;
static pthread_t coap_thread;

rest_response_info RiotsensorsRESTHandler::handleCallById(rs_lambda_type_t type, lambda_id_t id) {
    spt_log_msg("web", "Calling for lambda by ID with ID %d and expected type %d...\n", id, type);
    generic_lambda_return result;
    int8_t res = call_lambda_by_id(id, type, &result);
    rs_registered_lambda *lambda = get_registered_lambda_by_id(id);
    if (res == RS_CALL_SUCCESS) {
        return std::make_pair(Http::Code::Ok,
                              assemble_call_success_rest(lambda, false, false, &result));
    } else if (res == RS_CALL_CACHE) {
        return std::make_pair(Http::Code::Ok,
                              assemble_call_success_rest(lambda, true, false, &result));
    } else if (res == RS_CALL_CACHE_TIMEOUT) {
        return std::make_pair(Http::Code::Ok,
                              assemble_call_success_rest(lambda, true, true, &result));
    } else {
        return std::make_pair(Http::Code::Not_Found, assemble_call_error_rest_id(id, lambda, res));
    }
}

rest_response_info RiotsensorsRESTHandler::handleCallByName(rs_lambda_type_t type, std::string name) {
    spt_log_msg("web", "Calling for lambda by name with name %s and expected type %d...\n", name.c_str(), type);
    generic_lambda_return result;
    int8_t res = call_lambda_by_name(name.c_str(), type, &result);
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name.c_str());
    if (res == RS_CALL_SUCCESS) {
        return std::make_pair(Http::Code::Ok,
                              assemble_call_success_rest(lambda, false, false, &result));
    } else if (res == RS_CALL_CACHE) {
        return std::make_pair(Http::Code::Ok,
                              assemble_call_success_rest(lambda, true, false, &result));
    } else if (res == RS_CALL_CACHE_TIMEOUT) {
        return std::make_pair(Http::Code::Ok,
                              assemble_call_success_rest(lambda, true, true, &result));
    } else {
        return std::make_pair(Http::Code::Not_Found, assemble_call_error_rest_name(name, lambda, res));
    }
}

rest_response_info RiotsensorsRESTHandler::handleList(rs_lambda_type_t type) {
    if (type == 0) {
        spt_log_msg("web", "Listing all registered lambdas...\n");
        return std::make_pair(Http::Code::Ok, assemble_list_rest());
    } else {
        spt_log_msg("web", "Listing all registered lambdas for type %d...\n", type);
        return std::make_pair(Http::Code::Ok, assemble_list_rest_for_type(type));
    }
}

rest_response_info RiotsensorsRESTHandler::handleCache(rs_lambda_type_t type) {
    if (type == 0) {
        spt_log_msg("web", "Listing all registered lambdas and the cached values...\n");
        return std::make_pair(Http::Code::Ok, assemble_cache_rest());
    } else {
        spt_log_msg("web", "Listing all registered lambdas and the cached values for type %d...\n", type);
        return std::make_pair(Http::Code::Ok, assemble_cache_rest_for_type(type));
    }
}

rs_lambda_type_t get_lambda_type_from_string(std::string str) {
    std::stringstream ss(str);
    int strAsInt;
    ss >> strAsInt;
    if (strAsInt == RS_LAMBDA_INT) {
        return RS_LAMBDA_INT;
    } else if (strAsInt == RS_LAMBDA_DOUBLE) {
        return RS_LAMBDA_DOUBLE;
    } else if (strAsInt == RS_LAMBDA_STRING) {
        return RS_LAMBDA_STRING;
    } else {
        return (rs_lambda_type_t) -1;
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s SERIAL_CONSOLE_DESCRIPTOR\n", argv[0]);
        return 1;
    }

    if (rs_linux_start(argv[1]) != 0) {
        fprintf(stderr, "Could not start riotsensors on serial port %s\n", argv[1]);
        return 1;
    }

    spt_log_msg("main", "Starting HTTP server...\n");
    pthread_create(&http_thread, NULL, startHTTPServer, NULL);
    spt_log_msg("main", "Starting CoAP server...\n");
    pthread_create(&coap_thread, NULL, startCoAPServer, NULL);

    pthread_join(http_thread, NULL);
    pthread_join(coap_thread, NULL);

    rs_linux_stop();
}