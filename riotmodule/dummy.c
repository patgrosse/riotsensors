#include <dummy.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <serial_io.h>
#include <spt.h>
#include <spt_logger.h>

void handle_received_packet(struct spt_context *sptctx, struct serial_data_packet *packet) {
    if (sptctx->log_in_line) {
        printf("\n");
    }
    log_msg("packet", "Received packet with length %d\n", packet->len);
    if (sptctx->log_in_line) {
        log_msg("data", "");
    }
}

int startthings() {
    struct serial_io_context sictx;
    serial_io_context_init(&sictx, STDIN_FILENO, STDOUT_FILENO);
    struct spt_context sptctx;
    spt_init_context(&sptctx, &sictx, handle_received_packet);
    log_msg("main", "Starting SPT...\n");
    spt_start(&sptctx);
    spt_stop(&sptctx);
    return EXIT_SUCCESS;
}