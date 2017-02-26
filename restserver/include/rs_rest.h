/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_REST_H
#define RIOTSENSORS_RS_REST_H

#include <iostream>

extern "C" {
#include <lambda_registry.h>
#include <rs_connector.h>
};

std::string
assemble_call_success_rest(rs_registered_lambda *lambda, bool cache_retrieved, bool timeout,
                           generic_lambda_return *result);

std::string assemble_call_error_rest_id(lambda_id_t id, const rs_registered_lambda *lambda, int8_t error);

std::string assemble_call_error_rest_name(std::string name, const rs_registered_lambda *lambda, int8_t error);

#endif //RIOTSENSORS_RS_REST_H
