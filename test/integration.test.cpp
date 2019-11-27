#include "test.h"

#include "local_endpoint.h"
#include "remote_endpoint.h"
#include "types.h" // TypedValue

#include <limits>

static bool free_func_called = false;

static uint32_t simple_func(uint32_t a1)
{
    EXPECT_EQ(a1, (uint32_t)1234);
    free_func_called = true;
    return 4321;
}

//------------------------------------------------------------------------------
//
template<typename T>
struct Integration : public testing::Test
{
    using MyParamType = T;
};

//------------------------------------------------------------------------------
//
using NumericTypes = testing::Types<
    uint8_t, uint16_t, uint32_t, uint64_t,
    int8_t, int16_t, int32_t, int64_t,
    float, double, bool
>;
TYPED_TEST_SUITE(Integration, NumericTypes);

//------------------------------------------------------------------------------
//
TYPED_TEST(Integration, func_with_one_numeric_inparam)
{
    using NumericType  = typename TestFixture::MyParamType;

    // create endpoint
    remo::LocalEndpoint endpoint;
    remo::RemoteEndpoint* remote = endpoint.connect(".");

    bool func_called = false;

    // register function
    endpoint.bind("test_func", [&](NumericType a1) {
        func_called = true;
        EXPECT_EQ(a1, std::numeric_limits<NumericType>::max());
        return std::numeric_limits<NumericType>::min();
    });

    // call function
    NumericType a1 = std::numeric_limits<NumericType>::max();
    remo::TypedValue result = remote->call("test_func", a1);
    ASSERT_TRUE(func_called);
    EXPECT_EQ(result.get<NumericType>(), std::numeric_limits<NumericType>::min());
}

//------------------------------------------------------------------------------
//
TYPED_TEST(Integration, func_with_one_numeric_outparam)
{
    using NumericType  = typename TestFixture::MyParamType;

    // create endpoint
    remo::LocalEndpoint endpoint;
    remo::RemoteEndpoint* remote = endpoint.connect(".");

    bool func_called = false;

    // register function
    endpoint.bind("test_func", [&](NumericType* a1) {
        func_called = true;
        EXPECT_EQ(*a1, std::numeric_limits<NumericType>::max());
        // modify outparam
        *a1 = std::numeric_limits<NumericType>::min();
        return (NumericType)42;
    });

    // call function
    NumericType a1 = std::numeric_limits<NumericType>::max();
    remo::TypedValue result = remote->call("test_func", &a1);
    ASSERT_TRUE(func_called);
    EXPECT_EQ(result.get<NumericType>(), (NumericType)42);
    EXPECT_EQ(a1, std::numeric_limits<NumericType>::min());
}

//------------------------------------------------------------------------------
//
TEST(Integration, func_with_one_string_inparam)
{
    // create endpoint
    remo::LocalEndpoint endpoint;
    remo::RemoteEndpoint* remote = endpoint.connect(".");

    bool func_called = false;

    // register function
    endpoint.bind("test_func", [&](const char* a1) {
        func_called = true;
        EXPECT_STREQ(a1, "hello world");
        return (uint32_t)0;
    });

    // call function
    remote->call("test_func", "hello world");
    ASSERT_TRUE(func_called);
}

//------------------------------------------------------------------------------
//
TYPED_TEST(Integration, func_with_two_numeric_inparams)
{
    using NumericType  = typename TestFixture::MyParamType;

    // create endpoint
    remo::LocalEndpoint endpoint;
    remo::RemoteEndpoint* remote = endpoint.connect(".");

    bool func_called = false;

    // register function
    endpoint.bind("test_func", [&](NumericType a1, uint32_t a2) {
        func_called = true;
        EXPECT_EQ(a1, std::numeric_limits<NumericType>::max());
        EXPECT_EQ(a2, (uint32_t)12345678);
        return std::numeric_limits<NumericType>::min();
    });

    // call function
    NumericType a1 = std::numeric_limits<NumericType>::max();
    remo::TypedValue result = remote->call("test_func", a1, (uint32_t)12345678);
    ASSERT_TRUE(func_called);
    EXPECT_EQ(result.get<NumericType>(), std::numeric_limits<NumericType>::min());
}

//------------------------------------------------------------------------------
//
TEST(Integration, func_with_no_params_and_no_result)
{
    // create endpoint
    remo::LocalEndpoint endpoint;
    remo::RemoteEndpoint* remote = endpoint.connect(".");

    bool func_called = false;

    // register function
    endpoint.bind("test_func", [&]() {
        func_called = true;
    });

    // call function
    remote->call("test_func");
    ASSERT_TRUE(func_called);
}

//------------------------------------------------------------------------------
//
TEST(integration, simple)
{
    remo::LocalEndpoint endpoint;

    // register function
    endpoint.bind("simple_func", &simple_func);

    // get loopback
    remo::RemoteEndpoint* remote = endpoint.connect(".");

    // call function
    free_func_called = false;
    uint32_t result = remote->call("simple_func", (uint32_t)1234).get<uint32_t>();
    ASSERT_TRUE(free_func_called);
    ASSERT_EQ(result, (uint32_t)4321);
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
