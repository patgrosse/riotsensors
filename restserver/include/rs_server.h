/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Header file for generic REST server operations
 * @file    rs_server.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_SERVER_H
#define RIOTSENSORS_RS_SERVER_H

#include <pistache/endpoint.h>
#include <rs_packets.h>

using namespace Pistache;

/**
 * @brief Return type consisting of HTTP response code and response body for REST responses
 */
typedef std::pair<Http::Code, std::string> rest_response_info;

/**
 * @brief Handler functions for REST operations and responses
 */
class RiotsensorsRESTHandler {
public:
    /**
     * @brief Handle a REST call for a lambda identified by it's ID
     *
     * @param type Type of the lambda
     * @param id ID of the lambda
     * @return A pair containing the HTTP response code and the response body
     */
    static rest_response_info handleCallById(rs_lambda_type_t type, lambda_id_t id);

    /**
     * @brief Handle a REST call for a lambda identified by it's name
     *
     * @param type Type of the lambda
     * @param name Name of the lambda
     * @return A pair containing the HTTP response code and the response body
     */
    static rest_response_info handleCallByName(rs_lambda_type_t type, std::string name);

    /**
     * @brief Handle a REST call to list all registered lambdas
     *
     * @param type Type of the lambdas to be listed
     * @return A pair containing the HTTP response code and the response body
     */
    static rest_response_info handleList(rs_lambda_type_t type);

    /**
     * @brief Handle a REST call to list all registered lambdas and their cached results
     *
     * @param type Type of the lambdas to be listed
     * @return A pair containing the HTTP response code and the response body
     */
    static rest_response_info handleCache(rs_lambda_type_t type);
};

/**
 * @brief Dummy parent class for REST operation handlers/providers (HTTP, CoAP)
 */
class RiotsensorsRESTProvider {
};

/**
 * @brief Program arguments for the server runnable
 */
struct riotsensors_start_opts {
    char *serial;
    uint16_t http_port;
    uint16_t coap_port;
};

/**
 * @brief Main function of the REST server
 *
 * @param argc Argument count
 * @param argv Program arguments
 * @return 0 on successful execution
 */
int main(int argc, char *argv[]);

#endif //RIOTSENSORS_RS_SERVER_H
