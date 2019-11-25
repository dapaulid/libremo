#include "test.h"

#include "local_endpoint.h"
#include "remote_endpoint.h"

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
TYPED_TEST_CASE(Integration, NumericTypes);

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
    NumericType result = remote->call<NumericType>("test_func", a1);
    ASSERT_TRUE(func_called);
    EXPECT_EQ(result, std::numeric_limits<NumericType>::min());
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
    NumericType result = remote->call<NumericType>("test_func", &a1);
    ASSERT_TRUE(func_called);
    EXPECT_EQ(result, (NumericType)42);
    EXPECT_EQ(a1, std::numeric_limits<NumericType>::min());
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
    uint32_t result = remote->call<uint32_t>("simple_func", (uint32_t)1234);
    ASSERT_TRUE(free_func_called);
    ASSERT_EQ(result, (uint32_t)4321);
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
