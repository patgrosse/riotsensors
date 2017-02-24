/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_connector.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include <serial_io.h>
#include <spt.h>
#include <unused.h>
#include <spt_logger.h>
#include <tty_utils.h>
#include <rs_packets.h>

struct serial_io_context linux_sictx;
struct spt_context linux_sptctx;

void handle_received_packet(struct spt_context *sptctx, struct serial_data_packet *packet) {
    if (sptctx->log_in_line) {
        putchar('\n');
    }
    log_msg("packet", "Received packet with length %d\n", packet->len);
    char packetdatabuf[packet->len + 1];
    memcpy(packetdatabuf, packet->data, packet->len);
    packetdatabuf[packet->len] = '\0';
    log_msg("packet", "Data of packet is %s\n", packetdatabuf);
    if (sptctx->log_in_line) {
        log_msg("data", "");
    }
}

int rs_linux_start(const char *serial_file) {
    int serialfd = connect_serial(serial_file);
    if (serialfd < 0) {
        return -1;
    }
    if (init_serial_connection(serialfd) != 0) {
        return -1;
    }
    serial_io_context_init(&linux_sictx, serialfd, serialfd);
    spt_init_context(&linux_sptctx, &linux_sictx, handle_received_packet);
    log_msg("main", "Starting SPT...\n");
    spt_start(&linux_sptctx);
    return 0;
}

int rs_linux_stop() {
    spt_stop(&linux_sptctx);
    return 0;
}

void call_lambda_by_id(lambda_id_t id, rs_lambda_type_t expected_type) {
    rs_packet_call_by_id_t *mypkt = malloc(sizeof(rs_packet_call_by_id_t));
    mypkt->base.ptype = RS_PACKET_CALL_BY_ID;
    mypkt->lambda_id = id;
    mypkt->expected_type = expected_type;
    hton_rs_packet_call_by_id_t(mypkt);
    struct serial_data_packet pkt;
    pkt.data = (char *) mypkt;
    pkt.len = sizeof(*mypkt);
    spt_send_packet(&linux_sptctx, &pkt);
    free(mypkt);
}

void call_lambda_by_name(const char *name, rs_lambda_type_t expected_type) {
    rs_packet_call_by_name_t *mypkt = malloc(sizeof(rs_packet_call_by_name_t));
    mypkt->base.ptype = RS_PACKET_CALL_BY_ID;
    strcpy(mypkt->name, name);
    mypkt->expected_type = expected_type;
    hton_rs_packet_call_by_name_t(mypkt);
    struct serial_data_packet pkt;
    pkt.data = (char *) mypkt;
    pkt.len = sizeof(*mypkt);
    spt_send_packet(&linux_sptctx, &pkt);
    free(mypkt);
}
