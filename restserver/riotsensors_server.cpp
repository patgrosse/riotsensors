/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include "riotsensors_server.h"
#include <endpoint.h>
#include <router.h>

extern "C" {
#include <rs_connector.h>
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

class RiotsensorsHandler {
public:

    void handleCallById(const Rest::Request &request, Http::ResponseWriter response) {
        rs_lambda_type_t type = request.param(":type").as<rs_lambda_type_t>();
        lambda_id_t id = request.param(":id").as<lambda_id_t>();
        call_lambda_by_id(id, type);
        response.send(Http::Code::Ok, "Call by id\n");
    }

    void handleCallByName(const Rest::Request &request, Http::ResponseWriter response) {
        rs_lambda_type_t type = request.param(":type").as<rs_lambda_type_t>();
        std::string name = request.param(":name").as<std::string>();
        call_lambda_by_name(name.c_str(), type);
        response.send(Http::Code::Ok, "Call by name\n");
    }

    void handleList(const Rest::Request &request, Http::ResponseWriter response) {
        auto typeparam = request.query().get("type");
        if (typeparam.isEmpty()) {
            response.send(Http::Code::Ok, "List method all\n");
        } else {
            rs_lambda_type_t type = get_lambda_type_from_string(typeparam.get());
            if (type == 0) {
                response.send(Http::Code::Bad_Request, "Unknown lambda type\n");
            } else {
                response.send(Http::Code::Ok, "List method just on lambda\n");
            }
        }
    }
};

int main() {
    // riotsensors linux connector
    std::string serial_port = "/dev/ttyUSB0";
    rs_linux_start(serial_port.c_str());

    // server
    RiotsensorsHandler handler;

    Rest::Router router;
    Rest::Routes::Get(router, "/call/id/:type/:id", Rest::Routes::bind(&RiotsensorsHandler::handleCallById, &handler));
    Rest::Routes::Get(router, "/call/name/:type/:name",
                      Rest::Routes::bind(&RiotsensorsHandler::handleCallByName, &handler));
    Rest::Routes::Get(router, "/list", Rest::Routes::bind(&RiotsensorsHandler::handleList, &handler));

    Net::Address addr(Net::Ipv4::any(), Net::Port(9080));
    auto opts = Net::Http::Endpoint::options().threads(1);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve();

    server.shutdown();

    rs_linux_stop();
}
