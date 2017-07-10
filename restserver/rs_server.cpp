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
#include <argp.h>

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

/*
 * ==============================
 * ARGP options and main function
 * ==============================
 */

const char *argp_program_version =
        "riotsensors server 1.0";

const char *argp_program_bug_address =
        "<patrick.grosse@uni-muenster.de>";

static struct argp_option options[] =
        {
                {"serial", 's', "FILE", 0, "file descriptor of serial console device (default /dev/ttyUSB0)"},
                {"http",   'h', "PORT", 0, "port for the HTTP server (default 9080)"},
                {"coap",   'c', "PORT", 0, "port for the CoAP server (default 5683)"},
                {0}
        };

static error_t
parse_opt(int key, char *arg, struct argp_state *state) {
    struct riotsensors_start_opts *arguments = (struct riotsensors_start_opts *) state->input;

    switch (key) {
        case 's':
            arguments->serial = arg;
            break;
        case 'h':
            arguments->http_port = (uint16_t) std::stoul(arg);
            break;
        case 'c':
            arguments->coap_port = (uint16_t) std::stoul(arg);
            break;
        case ARGP_KEY_END:
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static char doc[] = "riotsensors REST server - a HTTP and CoAP REST server for riotsensors";

static struct argp argp = {options, parse_opt, "", doc};

int main(int argc, char *argv[]) {
    struct riotsensors_start_opts *arguments = new struct riotsensors_start_opts;
    arguments->serial = (char *) "/dev/ttyUSB0";
    arguments->http_port = 9080;
    arguments->coap_port = 5683;
    argp_parse(&argp, argc, argv, 0, 0, arguments);

    if (rs_linux_start(arguments->serial) != 0) {
        fprintf(stderr, "Could not start riotsensors on serial port %s\n", arguments->serial);
        return 1;
    }

    spt_log_msg("main", "Starting HTTP server on port %d...\n", arguments->http_port);
    pthread_create(&http_thread, NULL, startHTTPServer, arguments);
    spt_log_msg("main", "Starting CoAP server on port %d...\n", arguments->coap_port);
    pthread_create(&coap_thread, NULL, startCoAPServer, arguments);

    pthread_join(http_thread, NULL);
    pthread_join(coap_thread, NULL);

    rs_linux_stop();

    delete arguments;
}