/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   CoAP server on the RIOT-OS board
 * @file    rs_coap_server.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_COAP_SERVER_H
#define RIOTSENSORS_RS_COAP_SERVER_H

#ifndef RS_NO_COAP

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start the CoAP server thread
 */
void rs_start_coap_server(void);

/**
 * @brief Print the output of the ifconfig command
 */
void print_ifconfig(void);

#ifdef __cplusplus
}
#endif

#endif

#endif //RIOTSENSORS_RS_COAP_SERVER_H
