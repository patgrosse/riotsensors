/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Header file of the CoAP REST server
 * @file    rs_server_coap.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RIOTSENSORS_SERVER_COAP_H
#define RIOTSENSORS_RIOTSENSORS_SERVER_COAP_H

#include <rs_server.h>
#include <coap/coap.h>

/**
 * @brief Processes all received CoAP REST calls
 */
class RiotsensorsCoAPProvider : public RiotsensorsRESTProvider {
public:
    /**
     * @brief Handle a REST call for a lambda identified by it's ID
     *
     * @param ctx CoAP context
     * @param resource CoAP resource
     * @param local_interface CoAP local interface
     * @param peer CoAP peer endpoint
     * @param request CoAP request
     * @param token CoAP token
     * @param response CoAP response to send
     */
    static void handleCallById(coap_context_t *ctx, struct coap_resource_t *resource,
                               const coap_endpoint_t *local_interface, coap_address_t *peer,
                               coap_pdu_t *request, str *token, coap_pdu_t *response);

    /**
     * @brief Handle a REST call for a lambda identified by it's name
     *
     * @param ctx CoAP context
     * @param resource CoAP resource
     * @param local_interface CoAP local interface
     * @param peer CoAP peer endpoint
     * @param request CoAP request
     * @param token CoAP token
     * @param response CoAP response to send
     */
    static void handleCallByName(coap_context_t *ctx, struct coap_resource_t *resource,
                                 const coap_endpoint_t *local_interface, coap_address_t *peer,
                                 coap_pdu_t *request, str *token, coap_pdu_t *response);

    /**
     * @brief Handle a REST call to list all registered lambdas
     *
     * @param ctx CoAP context
     * @param resource CoAP resource
     * @param local_interface CoAP local interface
     * @param peer CoAP peer endpoint
     * @param request CoAP request
     * @param token CoAP token
     * @param response CoAP response to send
     */
    static void handleList(coap_context_t *ctx, struct coap_resource_t *resource,
                           const coap_endpoint_t *local_interface, coap_address_t *peer,
                           coap_pdu_t *request, str *token, coap_pdu_t *response);

    /**
     * @brief Handle a REST call to list all registered lambdas and their cached results
     *
     * @param ctx CoAP context
     * @param resource CoAP resource
     * @param local_interface CoAP local interface
     * @param peer CoAP peer endpoint
     * @param request CoAP request
     * @param token CoAP token
     * @param response CoAP response to send
     */
    static void handleCache(coap_context_t *ctx, struct coap_resource_t *resource,
                            const coap_endpoint_t *local_interface, coap_address_t *peer,
                            coap_pdu_t *request, str *token, coap_pdu_t *response);

    /**
     * @brief Handle a REST call to kill the server
     *
     * @param ctx CoAP context
     * @param resource CoAP resource
     * @param local_interface CoAP local interface
     * @param peer CoAP peer endpoint
     * @param request CoAP request
     * @param token CoAP token
     * @param response CoAP response to send
     */
    static void handleKill(coap_context_t *ctx, struct coap_resource_t *resource,
                           const coap_endpoint_t *local_interface, coap_address_t *peer,
                           coap_pdu_t *request, str *token, coap_pdu_t *response);
};

/**
 * @brief Start the CoAP REST server
 *
 * @param thread_ctx Unused context for pthreads
 * @return Unused return value for pthreads
 */
void *startCoAPServer(void *thread_ctx);

#endif //RIOTSENSORS_RIOTSENSORS_SERVER_COAP_H
