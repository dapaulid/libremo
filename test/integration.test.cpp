#include "test.h"

#include "local_endpoint.h"
#include "remote_endpoint.h"

static bool func_called = false;

static uint32_t simple_func(uint32_t a1)
{
    EXPECT_EQ(a1, (uint32_t)1234);
    func_called = true;
    return 4321;
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
    func_called = false;
    uint32_t result = remote->call<uint32_t>("simple_func", (uint32_t)1234);
    ASSERT_TRUE(func_called);
    ASSERT_EQ(result, (uint32_t)4321);
}


//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
