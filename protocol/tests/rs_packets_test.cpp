#include <gtest/gtest.h>

#include <rs_packets.h>

TEST(rs_packets, rs_packet_base_t) {
    rs_packet_base_t pkt;
    rs_packet_base_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_base_t(&pkt);
    ntoh_rs_packet_base_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_registered_t) {
    rs_packet_registered_t pkt;
    rs_packet_registered_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_registered_t(&pkt);
    ntoh_rs_packet_registered_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_unregistered_t) {
    rs_packet_unregistered_t pkt;
    rs_packet_unregistered_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_unregistered_t(&pkt);
    ntoh_rs_packet_unregistered_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_lambda_result_t) {
    rs_packet_lambda_result_t pkt;
    rs_packet_lambda_result_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_lambda_result_t(&pkt);
    ntoh_rs_packet_lambda_result_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_lambda_result_error_t) {
    rs_packet_lambda_result_error_t pkt;
    rs_packet_lambda_result_error_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_lambda_result_error_t(&pkt);
    ntoh_rs_packet_lambda_result_error_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_lambda_result_int_t) {
    rs_packet_lambda_result_int_t pkt;
    rs_packet_lambda_result_int_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_lambda_result_int_t(&pkt);
    ntoh_rs_packet_lambda_result_int_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_lambda_result_double_t) {
    rs_packet_lambda_result_double_t pkt;
    // because of IEEE754 conversion a valid double is required
    pkt.result = 42;
    rs_packet_lambda_result_double_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_lambda_result_double_t(&pkt);
    ntoh_rs_packet_lambda_result_double_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_lambda_result_string_t) {
    rs_packet_lambda_result_string_t pkt;
    rs_packet_lambda_result_string_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_lambda_result_string_t(&pkt);
    ntoh_rs_packet_lambda_result_string_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_call_by_id_t) {
    rs_packet_call_by_id_t pkt;
    rs_packet_call_by_id_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_call_by_id_t(&pkt);
    ntoh_rs_packet_call_by_id_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}

TEST(rs_packets, rs_packet_call_by_name_t) {
    rs_packet_call_by_name_t pkt;
    rs_packet_call_by_name_t copy;
    memcpy(&copy, &pkt, sizeof(pkt));
    hton_rs_packet_call_by_name_t(&pkt);
    ntoh_rs_packet_call_by_name_t(&pkt);
    ASSERT_EQ(memcmp(&pkt, &copy, sizeof(pkt)), 0);
}