#include <gtest/gtest.h>

extern "C" {
#include <lambda_registry.h>
}

static int testval1 = 1;
static int testval2 = 2;

TEST(lambda_registry, simple_int) {
    init_lambda_registry();
    lambda_id_t id;
    ASSERT_EQ(id = lambda_registry_register("myInt", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, &testval1), 0);
    ASSERT_EQ(get_registered_lambda_by_id(id)->arg, &testval1);
    ASSERT_EQ(get_registered_lambda_by_name("myInt")->arg, &testval1);
    free_lambda_registry();
}

TEST(lambda_registry, wrongname_int) {
    init_lambda_registry();
    ASSERT_EQ(lambda_registry_register("myInt", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, &testval1), 0);
    // workaround because NULL is interpreted as integer
    ASSERT_EQ(get_registered_lambda_by_name("myInt2"), (void *) NULL);
    free_lambda_registry();
}

TEST(lambda_registry, namecheck) {
    init_lambda_registry();
    ASSERT_EQ(lambda_registry_register("myIntWAYTOOLONGWAYWAYTOOLONG", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, &testval1),
              RS_REGISTER_INVALNAME);
    ASSERT_EQ(lambda_registry_register("myInt", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, &testval1), 0);
    ASSERT_EQ(lambda_registry_register("my\1illChar", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, &testval1),
              RS_REGISTER_INVALNAME);
    free_lambda_registry();
}

TEST(lambda_registry, unregister) {
    init_lambda_registry();
    lambda_id_t id;
    ASSERT_EQ(id = lambda_registry_register("myDouble", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, &testval1), 0);
    ASSERT_EQ(lambda_registry_unregister(id), RS_UNREGISTER_SUCCESS);
    ASSERT_EQ(lambda_registry_unregister(id), RS_UNREGISTER_NOTFOUND);
    ASSERT_EQ(id = lambda_registry_register("myDouble", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, &testval2), 1);
    ASSERT_EQ(get_registered_lambda_by_id(id)->arg, &testval2);
    free_lambda_registry();
}