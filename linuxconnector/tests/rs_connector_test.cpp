#include <gtest/gtest.h>

extern "C" {
#include <rs_connector.h>
#include <lambda_registry.h>
}

TEST(rs_connector, handle_register) {
    struct spt_context sptctx;
    sptctx.log_in_line = false;
    rs_packet_registered_t a;
    a.base.ptype = RS_PACKET_REGISTERED;
    a.cache = RS_CACHE_NO_CACHE;
    a.ltype = RS_LAMBDA_INT;
    memcpy(a.name, "kram", 5);
    struct serial_data_packet pkt;
    pkt.data = (uint8_t *) &a;
    pkt.len = sizeof(a);
    init_lambda_registry();
    printf("\n");
    handle_received_packet(&sptctx, &pkt);
    printf("\n");
    ASSERT_NE(get_registered_lambda_by_name("kram"), (void *) NULL);
    ASSERT_EQ(get_registered_lambda_by_name("kram")->id, 0);
    ASSERT_EQ(get_registered_lambda_by_name("kram")->cache, RS_CACHE_NO_CACHE);
    ASSERT_EQ(get_registered_lambda_by_name("kram")->type, RS_LAMBDA_INT);
    free_lambda_registry();
}

TEST(rs_connector, handle_unregister) {
    struct spt_context sptctx;
    sptctx.log_in_line = false;
    rs_packet_registered_t a;
    a.base.ptype = RS_PACKET_REGISTERED;
    a.cache = RS_CACHE_NO_CACHE;
    a.ltype = RS_LAMBDA_INT;
    memcpy(a.name, "kram", 5);
    struct serial_data_packet pkt;
    pkt.data = (uint8_t *) &a;
    pkt.len = sizeof(a);
    init_lambda_registry();
    printf("\n");
    handle_received_packet(&sptctx, &pkt);
    printf("\n");
    ASSERT_NE(get_registered_lambda_by_name("kram"), (void *) NULL);
    rs_packet_unregistered_t a2;
    a2.base.ptype = RS_PACKET_UNREGISTERED;
    a2.lambda_id = get_registered_lambda_by_name("kram")->id;
    struct serial_data_packet pkt2;
    pkt2.data = (uint8_t *) &a2;
    pkt2.len = sizeof(a2);
    printf("\n");
    handle_received_packet(&sptctx, &pkt2);
    printf("\n");
    ASSERT_EQ(get_registered_lambda_by_name("kram"), (void *) NULL);
    free_lambda_registry();
}

TEST(rs_connector, handle_result) {
    struct spt_context sptctx;
    sptctx.log_in_line = false;
    rs_packet_registered_t a;
    a.base.ptype = RS_PACKET_REGISTERED;
    a.cache = RS_CACHE_NO_CACHE;
    a.ltype = RS_LAMBDA_INT;
    memcpy(a.name, "kram", 5);
    struct serial_data_packet pkt;
    pkt.data = (uint8_t *) &a;
    pkt.len = sizeof(a);
    init_lambda_registry();
    printf("\n");
    handle_received_packet(&sptctx, &pkt);
    printf("\n");
    ASSERT_NE(get_registered_lambda_by_name("kram"), (void *) NULL);
    rs_packet_lambda_result_int_t a2;
    a2.result_base.base.ptype = RS_PACKET_RESULT_INT;
    a2.result_base.lambda_id = get_registered_lambda_by_name("kram")->id;
    memcpy(a2.result_base.name, get_registered_lambda_by_name("kram")->name,
           strlen(get_registered_lambda_by_name("kram")->name));
    a2.result = 42;
    struct serial_data_packet pkt2;
    pkt2.data = (uint8_t *) &a2;
    pkt2.len = sizeof(a2);
    printf("\n");
    handle_received_packet(&sptctx, &pkt2);
    printf("\n");
    rs_linux_registered_lambda *arg = (rs_linux_registered_lambda *) get_registered_lambda_by_name("kram")->arg;
    ASSERT_EQ(arg->data_cached, true);
    ASSERT_EQ(arg->last_call_error, 0);
    ASSERT_EQ(arg->ret.ret_i, 42);
    free_lambda_registry();
}