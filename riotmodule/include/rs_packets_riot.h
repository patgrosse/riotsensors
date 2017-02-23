/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   Methods for sending packets from RIOT-OS to the receiving Linux
 * @file    rs_packets_riot.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_PACKETS_RIOT_H
#define RIOTSENSORS_RS_PACKETS_RIOT_H

#include <rs_packets.h>

/**
 * Function signature for the callback for a rs_packet_call_by_id_t packet
 */
typedef void (*rs_packet_callback_call_id)(rs_packet_call_by_id_t *);

/**
 * Function signature for the callback for a rs_packet_call_by_name_t packet
 */
typedef void (*rs_packet_callback_call_name)(rs_packet_call_by_name_t *);

/**
 * Initialize the packet transfer with default values
 * @param call_id Callback for a rs_packet_call_by_id_t packet
 * @param call_name Callback for a rs_packet_call_by_name_t packet
 */
void rs_packets_riot_init(rs_packet_callback_call_id call_id, rs_packet_callback_call_name call_name);

/**
 * Start receiving packets
 */
void rs_packets_riot_start();

/**
 * Stop receiving packets
 */
void rs_packets_riot_stop();

#endif //RIOTSENSORS_RS_PACKETS_RIOT_H
