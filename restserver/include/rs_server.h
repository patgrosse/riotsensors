/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Header file of the server executable
 * @file    rs_server.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RIOTSENSORS_SERVER_H
#define RIOTSENSORS_RIOTSENSORS_SERVER_H

#include <endpoint.h>
#include <router.h>

using namespace Net;

/**
 * @brief Main function of the REST server
 *
 * @return 0 on successful execution
 */
int main();

/**
 * @brief Processes all received REST calls
 */
class RiotsensorsHandler {
private:
    Http::Endpoint *server;
public:
    /**
     * @brief Set the endpoint instance this handler is associated to
     *
     * @param server Pistache endpoint
     */
    void setServer(Http::Endpoint *server);

    /**
     * @brief Handle a REST call for a lambda identified by it's ID
     *
     * @param request Received request
     * @param response Response to send
     */
    void handleCallById(const Rest::Request &request, Http::ResponseWriter response);

    /**
     * @brief Handle a REST call for a lambda identified by it's name
     *
     * @param request Received request
     * @param response Response to send
     */
    void handleCallByName(const Rest::Request &request, Http::ResponseWriter response);

    /**
     * @brief Handle a REST call to list all registered lambdas
     *
     * @param request Received request
     * @param response Response to send
     */
    void handleList(const Rest::Request &request, Http::ResponseWriter response);

    /**
     * @brief Handle a REST call to list all registered lambdas and their cached results
     *
     * @param request Received request
     * @param response Response to send
     */
    void handleCache(const Rest::Request &request, Http::ResponseWriter response);

    /**
     * @brief Handle a REST call to kill the server
     *
     * @param request Received request
     * @param response Response to send
     */
    void handleKill(const Rest::Request &request, Http::ResponseWriter response);
};

#endif //RIOTSENSORS_RIOTSENSORS_SERVER_H
