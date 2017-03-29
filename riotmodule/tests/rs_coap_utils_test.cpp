#include <gtest/gtest.h>

#include <rs_coap_utils.h>

TEST(rs_coap_utils, simple_query) {
    std::string query = "name=test&key=test2";
    const char *cquery = query.c_str();
    struct coap_queries result;
    split_query(query.length(), cquery, &result);
    ASSERT_EQ(result.num, 2);
    ASSERT_NE(result.first, (void *) NULL);
    ASSERT_STREQ(result.first->key, "name");
    ASSERT_STREQ(result.first->value, "test");
    ASSERT_STREQ(result.first->next->key, "key");
    ASSERT_STREQ(result.first->next->value, "test2");
    free_coap_query(&result);
}

TEST(rs_coap_utils, empty_query) {
    std::string query = "";
    const char *cquery = query.c_str();
    struct coap_queries result;
    split_query(query.length(), cquery, &result);
    ASSERT_EQ(result.num, 0);
    ASSERT_EQ(result.first, (void *) NULL);
    free_coap_query(&result);
}

TEST(rs_coap_utils, value_empty) {
    std::string query = "name&key=test2";
    const char *cquery = query.c_str();
    struct coap_queries result;
    split_query(query.length(), cquery, &result);
    ASSERT_EQ(result.num, 2);
    ASSERT_NE(result.first, (void *) NULL);
    ASSERT_STREQ(result.first->key, "name");
    ASSERT_STREQ(result.first->value, "");
    ASSERT_STREQ(result.first->next->key, "key");
    ASSERT_STREQ(result.first->next->value, "test2");
    free_coap_query(&result);
}

TEST(rs_coap_utils, index_of) {
    ASSERT_EQ(index_of("key=value", '='), 3);
    ASSERT_EQ(index_of("longkey=value", '='), 7);
    ASSERT_EQ(index_of("=value", '='), 0);
    ASSERT_EQ(index_of("value", '='), -1);
    ASSERT_EQ(index_of("", '='), -1);
}