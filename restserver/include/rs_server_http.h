/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Header file of the HTTP REST server
 * @file    rs_server_http.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RIOTSENSORS_SERVER_HTTP_H
#define RIOTSENSORS_RIOTSENSORS_SERVER_HTTP_H

#include <endpoint.h>
#include <router.h>
#include <rs_server.h>

using namespace Net;

/**
 * @brief Processes all received HTTP REST calls
 */
class RiotsensorsHTTPProvider : public RiotsensorsRESTProvider {
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

/**
 * @brief Start the HTTP REST server
 *
 * @param thread_ctx Unused context for pthreads
 * @return Unused return value for pthreads
 */
void *startHTTPServer(void *thread_ctx);

#endif //RIOTSENSORS_RIOTSENSORS_SERVER_HTTP_H
