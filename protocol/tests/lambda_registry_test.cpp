#include <gtest/gtest.h>

#include <lambda_registry.h>

static int testval1 = 1;
static int testval2 = 2;

TEST(lambda_registry, simple_int) {
    init_lambda_registry();
    lambda_id_t id;
    lambda_arg larg;
    larg.obj = &testval1;
    ASSERT_EQ(id = lambda_registry_register("myInt", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, larg), 0);
    ASSERT_EQ(get_registered_lambda_by_id(id)->arg.obj, &testval1);
    ASSERT_EQ(get_registered_lambda_by_name("myInt")->arg.obj, &testval1);
    free_lambda_registry();
}

TEST(lambda_registry, wrongname_int) {
    init_lambda_registry();
    lambda_arg larg;
    larg.obj = &testval1;
    ASSERT_EQ(lambda_registry_register("myInt", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, larg), 0);
    // workaround because NULL is interpreted as integer
    ASSERT_EQ(get_registered_lambda_by_name("myInt2"), (void *) NULL);
    free_lambda_registry();
}

TEST(lambda_registry, namecheck) {
    init_lambda_registry();
    lambda_arg larg;
    larg.obj = &testval1;
    ASSERT_EQ(lambda_registry_register("myIntWAYTOOLONGWAYWAYTOOLONG", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, larg),
              RS_REGISTER_INVALNAME);
    ASSERT_EQ(lambda_registry_register("myInt", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, larg), 0);
    ASSERT_EQ(lambda_registry_register("my\1illChar", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, larg),
              RS_REGISTER_INVALNAME);
    free_lambda_registry();
}

TEST(lambda_registry, unregister) {
    init_lambda_registry();
    lambda_id_t id;
    lambda_arg larg;
    larg.obj = &testval1;
    ASSERT_EQ(id = lambda_registry_register("myDouble", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, larg), 0);
    ASSERT_EQ(lambda_registry_unregister(id), RS_UNREGISTER_SUCCESS);
    ASSERT_EQ(lambda_registry_unregister(id), RS_UNREGISTER_NOTFOUND);
    lambda_arg larg2;
    larg2.obj = &testval2;
    ASSERT_EQ(id = lambda_registry_register("myDouble", RS_LAMBDA_INT, RS_CACHE_NO_CACHE, larg2), 1);
    ASSERT_EQ(get_registered_lambda_by_id(id)->arg.obj, &testval2);
    free_lambda_registry();
}