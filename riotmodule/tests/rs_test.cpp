#include <gtest/gtest.h>
#include <inttypes.h>

#include <rs.h>
#include <lambda_registry.h>

rs_int_t simple_int_lambda(lambda_id_t called_id) {
    if (called_id == 0) {
        return 42;
    }
    return -1;
}

rs_double_t simple_double_lambda(lambda_id_t called_id) {
    if (called_id == 0) {
        return 42.1;
    }else if(called_id == 1){
        return 4242.1;
    }
    return -2;
}

TEST(rs, simple_int) {
    init_lambda_registry();
    lambda_id_t id;
    ASSERT_EQ(id = register_lambda_int("myInt", simple_int_lambda, RS_CACHE_NO_CACHE), 0);
    rs_int_t result;
    ASSERT_EQ(call_lambda_int(id, &result), RS_CALL_SUCCESS);
    ASSERT_EQ(result, 42);
    result = -3;
    ASSERT_EQ(call_lambda_int_by_name("myInt", &result), RS_CALL_SUCCESS);
    ASSERT_EQ(result, 42);
    free_lambda_registry();
}

TEST(rs, simple_double) {
    init_lambda_registry();
    lambda_id_t id;
    ASSERT_EQ(id = register_lambda_double("myDouble", simple_double_lambda, RS_CACHE_NO_CACHE), 0);
    rs_double_t result;
    ASSERT_EQ(call_lambda_double(id, &result), RS_CALL_SUCCESS);
    ASSERT_FLOAT_EQ(result, 42.1);
    result = -4;
    ASSERT_EQ(call_lambda_double_by_name("myDouble", &result), RS_CALL_SUCCESS);
    ASSERT_FLOAT_EQ(result, 42.1);
    free_lambda_registry();
}

TEST(rs, wrongname_int) {
    init_lambda_registry();
    ASSERT_EQ(register_lambda_int("myInt", simple_int_lambda, RS_CACHE_NO_CACHE), 0);
    rs_int_t result = -5;
    ASSERT_EQ(call_lambda_int_by_name("myInt2", &result), RS_CALL_NOTFOUND);
    ASSERT_EQ(result, -5);
    free_lambda_registry();
}

TEST(rs, wrongname_double) {
    init_lambda_registry();
    ASSERT_EQ(register_lambda_double("myDouble", simple_double_lambda, RS_CACHE_NO_CACHE), 0);
    rs_double_t result = -6;
    ASSERT_EQ(call_lambda_double_by_name("myDouble2", &result), RS_CALL_NOTFOUND);
    ASSERT_FLOAT_EQ(result, -6);
    free_lambda_registry();
}

TEST(rs, namecheck) {
    init_lambda_registry();
    ASSERT_EQ(register_lambda_double("myDoubleWAYTOOLONGWAYWAYTOOLONG", simple_double_lambda, RS_CACHE_NO_CACHE), RS_REGISTER_INVALNAME);
    ASSERT_EQ(register_lambda_double("myDouble", simple_double_lambda, RS_CACHE_NO_CACHE), 0);
    free_lambda_registry();
}

TEST(rs, unregister) {
    init_lambda_registry();
    lambda_id_t id;
    ASSERT_EQ(id = register_lambda_double("myDouble", simple_double_lambda, RS_CACHE_NO_CACHE), 0);
    ASSERT_EQ(unregister_lambda(id), RS_UNREGISTER_SUCCESS);
    ASSERT_EQ(unregister_lambda(id), RS_UNREGISTER_NOTFOUND);
    ASSERT_EQ(id = register_lambda_double("myDouble", simple_double_lambda, RS_CACHE_NO_CACHE), 1);
    rs_double_t result;
    ASSERT_EQ(call_lambda_double(id, &result), RS_CALL_SUCCESS);
    ASSERT_FLOAT_EQ(result, 4242.1);
    free_lambda_registry();
}