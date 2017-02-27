/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Functions to create RESTful JSON answers for the server
 * @file    rs_rest.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_REST_H
#define RIOTSENSORS_RS_REST_H

#include <iostream>

extern "C" {
#include <lambda_registry.h>
#include <rs_connector.h>
};

/**
 * @brief Create a JSON string for successful calls
 *
 * @param lambda Called lambda
 * @param cache_retrieved If the result was retrieved from cache
 * @param timeout If a timeout occurred
 * @param result Result (has to match the type of the lambda)
 * @return A JSON string
 */
std::string
assemble_call_success_rest(rs_registered_lambda *lambda, bool cache_retrieved, bool timeout,
                           generic_lambda_return *result);

/**
 * @brief Create a JSON string for failed calls by id
 *
 * @param id ID of the lambda
 * @param lambda Lambda if found, NULL otherwise
 * @param error Occurred error code (RS_CALL_* constant)
 * @return A JSON string
 */
std::string assemble_call_error_rest_id(lambda_id_t id, const rs_registered_lambda *lambda, int8_t error);

/**
 * @brief Create a JSON string for failed calls by name
 *
 * @param name Name of the lambda
 * @param lambda Lambda if found, NULL otherwise
 * @param error Occurred error code (RS_CALL_* constant)
 * @return A JSON string
 */
std::string assemble_call_error_rest_name(std::string name, const rs_registered_lambda *lambda, int8_t error);

#endif //RIOTSENSORS_RS_REST_H
