/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */


#include <netinet/in.h>
#include <ieee754_network.h>
#include <rs_packets.h>

void htons_w(uint16_t *in) {
    *in = htons(*in);
}

void hton_rs_packet_base_t(rs_packet_base_t *pkt) {
    (void) pkt;
}

void hton_rs_packet_registered_t(rs_packet_registered_t *pkt) {
    hton_rs_packet_base_t(&pkt->base);
}

void hton_rs_packet_unregistered_t(rs_packet_unregistered_t *pkt) {
    hton_rs_packet_base_t(&pkt->base);
}

void hton_rs_packet_lambda_result_t(rs_packet_lambda_result_t *pkt) {
    hton_rs_packet_base_t(&pkt->base);
}

void hton_rs_packet_lambda_result_error_t(rs_packet_lambda_result_error_t *pkt) {
    hton_rs_packet_lambda_result_t(&pkt->result_base);
}

void hton_rs_packet_lambda_result_int_t(rs_packet_lambda_result_int_t *pkt) {
    hton_rs_packet_lambda_result_t(&pkt->result_base);
}

void hton_rs_packet_lambda_result_double_t(rs_packet_lambda_result_double_t *pkt) {
    hton_rs_packet_lambda_result_t(&pkt->result_base);
    pkt->result = pack754_64(pkt->result);
}

void hton_rs_packet_lambda_result_string_t(rs_packet_lambda_result_string_t *pkt) {
    hton_rs_packet_lambda_result_t(&pkt->result_base);
    htons_w(&pkt->result_length);
}

void hton_rs_packet_call_by_id_t(rs_packet_call_by_id_t *pkt) {
    hton_rs_packet_base_t(&pkt->base);
}

void hton_rs_packet_call_by_name_t(rs_packet_call_by_name_t *pkt) {
    hton_rs_packet_base_t(&pkt->base);
}

void ntohs_w(uint16_t *in) {
    *in = ntohs(*in);
}

void ntoh_rs_packet_base_t(rs_packet_base_t *pkt) {
    (void) pkt;
}

void ntoh_rs_packet_registered_t(rs_packet_registered_t *pkt) {
    ntoh_rs_packet_base_t(&pkt->base);
}

void ntoh_rs_packet_unregistered_t(rs_packet_unregistered_t *pkt) {
    ntoh_rs_packet_base_t(&pkt->base);
}

void ntoh_rs_packet_lambda_result_t(rs_packet_lambda_result_t *pkt) {
    ntoh_rs_packet_base_t(&pkt->base);
}

void ntoh_rs_packet_lambda_result_error_t(rs_packet_lambda_result_error_t *pkt) {
    ntoh_rs_packet_lambda_result_t(&pkt->result_base);
}

void ntoh_rs_packet_lambda_result_int_t(rs_packet_lambda_result_int_t *pkt) {
    ntoh_rs_packet_lambda_result_t(&pkt->result_base);
}

void ntoh_rs_packet_lambda_result_double_t(rs_packet_lambda_result_double_t *pkt) {
    ntoh_rs_packet_lambda_result_t(&pkt->result_base);
    pkt->result = (rs_double_t) unpack754_64((uint64_t) pkt->result);
}

void ntoh_rs_packet_lambda_result_string_t(rs_packet_lambda_result_string_t *pkt) {
    ntoh_rs_packet_lambda_result_t(&pkt->result_base);
    ntohs_w(&pkt->result_length);
}

void ntoh_rs_packet_call_by_id_t(rs_packet_call_by_id_t *pkt) {
    ntoh_rs_packet_base_t(&pkt->base);
}

void ntoh_rs_packet_call_by_name_t(rs_packet_call_by_name_t *pkt) {
    ntoh_rs_packet_base_t(&pkt->base);
}

