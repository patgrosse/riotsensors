/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include "rs_server.h"
#include <signal.h>
#include <rs_rest.h>

extern "C" {
#include <spt_logger.h>
}

using namespace Net;

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
        return 0;
    }
}

void RiotsensorsHandler::setServer(Http::Endpoint *server) {
    this->server = server;
}

void RiotsensorsHandler::handleCallById(const Rest::Request &request, Http::ResponseWriter response) {
    std::string str_type = request.param(":type").as<std::string>();
    rs_lambda_type_t type = get_lambda_type_from_string(str_type);
    if (type == 0) {
        response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
    }
    int int_id = request.param(":id").as<int>();
    if (int_id < 0) {
        response.send(Http::Code::Bad_Request, "Bad lambda id\n");
    }
    lambda_id_t id = (lambda_id_t) int_id;
    spt_log_msg("web", "Calling for lambda by ID with ID %d and expected type %d...\n", id, type);
    generic_lambda_return result;
    int8_t res = call_lambda_by_id(id, type, &result);
    auto m1 = MIME(Application, Json);
    response.setMime(m1);
    rs_registered_lambda *lambda = get_registered_lambda_by_id(id);
    if (res == RS_CALL_SUCCESS) {
        response.send(Http::Code::Ok,
                      assemble_call_success_rest(lambda, false, false, &result));
    } else if (res == RS_CALL_CACHE) {
        response.send(Http::Code::Ok,
                      assemble_call_success_rest(lambda, true, false, &result));
    } else if (res == RS_CALL_CACHE_TIMEOUT) {
        response.send(Http::Code::Ok,
                      assemble_call_success_rest(lambda, true, true, &result));
    } else {
        response.send(Http::Code::Internal_Server_Error, assemble_call_error_rest_id(id, lambda, res));
    }
}

void RiotsensorsHandler::handleCallByName(const Rest::Request &request, Http::ResponseWriter response) {
    std::string str_type = request.param(":type").as<std::string>();
    rs_lambda_type_t type = get_lambda_type_from_string(str_type);
    if (type == 0) {
        response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
    }
    std::string name = request.param(":name").as<std::string>();
    spt_log_msg("web", "Calling for lambda by name with name %s and expected type %d...\n", name.c_str(), type);
    generic_lambda_return result;
    int8_t res = call_lambda_by_name(name.c_str(), type, &result);
    auto m1 = MIME(Application, Json);
    response.setMime(m1);
    rs_registered_lambda *lambda = get_registered_lambda_by_name(name.c_str());
    if (res == RS_CALL_SUCCESS) {
        response.send(Http::Code::Ok,
                      assemble_call_success_rest(lambda, false, false, &result));
    } else if (res == RS_CALL_CACHE) {
        response.send(Http::Code::Ok,
                      assemble_call_success_rest(lambda, true, false, &result));
    } else if (res == RS_CALL_CACHE_TIMEOUT) {
        response.send(Http::Code::Ok,
                      assemble_call_success_rest(lambda, true, true, &result));
    } else {
        response.send(Http::Code::Internal_Server_Error, assemble_call_error_rest_name(name, lambda, res));
    }
}

void RiotsensorsHandler::handleList(const Rest::Request &request, Http::ResponseWriter response) {
    auto typeparam = request.query().get("type");
    if (typeparam.isEmpty()) {
        spt_log_msg("web", "Listing all registered lambdas...\n");
        response.send(Http::Code::Ok, assemble_list_rest());
    } else {
        rs_lambda_type_t type = get_lambda_type_from_string(typeparam.get());
        if (type == 0) {
            response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
        } else {
            spt_log_msg("web", "Listing all registered lambdas for type %d...\n", type);
            response.send(Http::Code::Ok, assemble_list_rest_for_type(type));
        }
    }
}

void RiotsensorsHandler::handleKill(const Rest::Request &request, Http::ResponseWriter response) {
    spt_log_msg("web", "Received server kill request...\n");
    raise(SIGINT);
    response.send(Http::Code::Ok, "Kill\n");
    server->shutdown();
}

void RiotsensorsHandler::handleCache(const Rest::Request &request, Http::ResponseWriter response) {
    auto typeparam = request.query().get("type");
    if (typeparam.isEmpty()) {
        spt_log_msg("web", "Listing all registered lambdas and the cached values...\n");
        response.send(Http::Code::Ok, assemble_cache_rest());
    } else {
        rs_lambda_type_t type = get_lambda_type_from_string(typeparam.get());
        if (type == 0) {
            response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
        } else {
            spt_log_msg("web", "Listing all registered lambdas and the cached values for type %d...\n", type);
            response.send(Http::Code::Ok, assemble_cache_rest_for_type(type));
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s SERIAL_CONSOLE_DESCRIPTOR\n", argv[0]);
        return 1;
    }

    // riotsensors linux connector
    if (rs_linux_start(argv[1]) != 0) {
        fprintf(stderr, "Could not start riotsensors on serial port %s\n", argv[1]);
        return 1;
    }
    // server
    RiotsensorsHandler handler;

    Rest::Router router;
    Rest::Routes::Get(router, "/call/id/:type/:id",
                      Rest::Routes::bind(&RiotsensorsHandler::handleCallById, &handler));
    Rest::Routes::Get(router, "/call/name/:type/:name",
                      Rest::Routes::bind(&RiotsensorsHandler::handleCallByName, &handler));
    Rest::Routes::Get(router, "/list", Rest::Routes::bind(&RiotsensorsHandler::handleList, &handler));
    Rest::Routes::Get(router, "/showcache", Rest::Routes::bind(&RiotsensorsHandler::handleCache, &handler));
    Rest::Routes::Get(router, "/kill", Rest::Routes::bind(&RiotsensorsHandler::handleKill, &handler));

    Net::Address addr(Net::Ipv4::any(), Net::Port(9080));
    auto opts = Net::Http::Endpoint::options();
    opts.threads(1);
    opts.flags(Tcp::Options::InstallSignalHandler);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    handler.setServer(&server);

    server.serve();

    server.shutdown();

    rs_linux_stop();
}
