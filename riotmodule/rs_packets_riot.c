/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <memory.h>
#include <malloc.h>
#include <unused.h>
#include <stdio.h>
#include <unistd.h>

#include <rs_packets_riot.h>
#include <spt.h>

static rs_packet_callback_call_id call_id_reg = NULL;
static rs_packet_callback_call_name call_name_reg = NULL;
static struct serial_io_context rs_sictx;
static struct spt_context rs_sptctx;

void handle_received_packet(struct spt_context *sptctx, struct serial_data_packet *packet) {
    UNUSED(sptctx);
    rs_packet_type_t ptype;
    if (packet->len < sizeof(ptype)) {
        fprintf(stderr, "Packet with size %d is too small for packet type detection (min size %d)\n", packet->len,
                sizeof(ptype));
        return;
    }
    ptype = (rs_packet_type_t) *packet->data;
    if (ptype == RS_PACKET_CALL_BY_ID) {
        if (packet->len != sizeof(rs_packet_call_by_id_t)) {
            fprintf(stderr,
                    "Packet with size %d has the wrong size for packet type rs_packet_call_by_id_t (size %d)\n",
                    packet->len,
                    sizeof(rs_packet_call_by_id_t));
            return;
        }
        rs_packet_call_by_id_t *mypkt = malloc(sizeof(rs_packet_call_by_id_t));
        memcpy(mypkt, packet->data, sizeof(rs_packet_call_by_id_t));
        if (call_id_reg != NULL) {
            call_id_reg(mypkt);
        }
        free(mypkt);
    } else if (ptype == RS_PACKET_CALL_BY_NAME) {
        if (packet->len != sizeof(rs_packet_call_by_name_t)) {
            fprintf(stderr,
                    "Packet with size %d has the wrong size for packet type rs_packet_call_by_name_t (size %d)\n",
                    packet->len,
                    sizeof(rs_packet_call_by_name_t));
            return;
        }
        rs_packet_call_by_name_t *mypkt = malloc(sizeof(rs_packet_call_by_name_t));
        memcpy(mypkt, packet->data, sizeof(rs_packet_call_by_name_t));
        if (call_name_reg != NULL) {
            call_name_reg(mypkt);
        }
        free(mypkt);
    } else {
        fprintf(stderr,
                "Received packet of unknown/unprocessable type %d with size %d\n",
                ptype,
                packet->len);
    }
}

void rs_packets_riot_init(rs_packet_callback_call_id call_id, rs_packet_callback_call_name call_name) {
    call_id_reg = call_id;
    call_name_reg = call_name;
    serial_io_context_init(&rs_sictx, STDIN_FILENO, STDOUT_FILENO);
    spt_init_context(&rs_sptctx, &rs_sictx, handle_received_packet);
}

void rs_packets_riot_start() {
    spt_start(&rs_sptctx);
}

void rs_packets_riot_stop() {
    spt_stop(&rs_sptctx);
}