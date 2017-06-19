/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_server_http.h>

#include <signal.h>
#include <rs_rest.h>
#include <spt_logger.h>
#include <unused.h>

using namespace Net;

void RiotsensorsHTTPProvider::setServer(Http::Endpoint *server) {
    this->server = server;
}

void RiotsensorsHTTPProvider::handleCallById(const Rest::Request &request, Http::ResponseWriter response) {
    std::string str_type = request.param(":type").as<std::string>();
    rs_lambda_type_t type = get_lambda_type_from_string(str_type.c_str());
    if (type == (rs_lambda_type_t) -1) {
        response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
    }
    int int_id = request.param(":id").as<int>();
    if (int_id < 0) {
        response.send(Http::Code::Bad_Request, "Bad lambda id\n");
    }
    lambda_id_t id = (lambda_id_t) int_id;
    auto m1 = MIME(Application, Json);
    response.setMime(m1);
    rest_response_info answer = RiotsensorsRESTHandler::handleCallById(type, id);
    response.send(answer.first, answer.second);
}

void RiotsensorsHTTPProvider::handleCallByName(const Rest::Request &request, Http::ResponseWriter response) {
    std::string str_type = request.param(":type").as<std::string>();
    rs_lambda_type_t type = get_lambda_type_from_string(str_type.c_str());
    if (type == (rs_lambda_type_t) -1) {
        response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
    }
    std::string name = request.param(":name").as<std::string>();
    auto m1 = MIME(Application, Json);
    response.setMime(m1);
    rest_response_info answer = RiotsensorsRESTHandler::handleCallByName(type, name);
    response.send(answer.first, answer.second);
}

void RiotsensorsHTTPProvider::handleList(const Rest::Request &request, Http::ResponseWriter response) {
    auto typeparam = request.query().get("type");
    rs_lambda_type_t type = 0;
    if (!typeparam.isEmpty()) {
        type = get_lambda_type_from_string(typeparam.get().c_str());
        if (type == (rs_lambda_type_t) -1) {
            response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
            return;
        }
    }
    auto m1 = MIME(Application, Json);
    response.setMime(m1);
    rest_response_info answer = RiotsensorsRESTHandler::handleList(type);
    response.send(answer.first, answer.second);
}

void RiotsensorsHTTPProvider::handleCache(const Rest::Request &request, Http::ResponseWriter response) {
    auto typeparam = request.query().get("type");
    rs_lambda_type_t type = 0;
    if (!typeparam.isEmpty()) {
        type = get_lambda_type_from_string(typeparam.get().c_str());
        if (type == (rs_lambda_type_t) -1) {
            response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
            return;
        }
    }
    auto m1 = MIME(Application, Json);
    response.setMime(m1);
    rest_response_info answer = RiotsensorsRESTHandler::handleCache(type);
    response.send(answer.first, answer.second);
}

void RiotsensorsHTTPProvider::handleKill(const Rest::Request &request, Http::ResponseWriter response) {
    spt_log_msg("web", "Received server kill request...\n");
    raise(SIGINT);
    response.send(Http::Code::Ok, "Kill\n");
    server->shutdown();
}

void *startHTTPServer(void *thread_ctx) {
    struct riotsensors_start_opts *arguments = (struct riotsensors_start_opts *) thread_ctx;

    RiotsensorsHTTPProvider provider;

    Rest::Router router;
    Rest::Routes::Get(router, "/v1/call/id/:type/:id",
                      Rest::Routes::bind(&RiotsensorsHTTPProvider::handleCallById, &provider));
    Rest::Routes::Get(router, "/v1/call/name/:type/:name",
                      Rest::Routes::bind(&RiotsensorsHTTPProvider::handleCallByName, &provider));
    Rest::Routes::Get(router, "/v1/list", Rest::Routes::bind(&RiotsensorsHTTPProvider::handleList, &provider));
    Rest::Routes::Get(router, "/v1/showcache", Rest::Routes::bind(&RiotsensorsHTTPProvider::handleCache, &provider));
    Rest::Routes::Get(router, "/v1/kill", Rest::Routes::bind(&RiotsensorsHTTPProvider::handleKill, &provider));

    Net::Address addr(Net::Ipv4::any(), Net::Port(arguments->http_port));
    auto opts = Net::Http::Endpoint::options();
    opts.threads(1);
    opts.flags(Tcp::Options::InstallSignalHandler);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    provider.setServer(&server);

    server.serve();

    server.shutdown();
}
