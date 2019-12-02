#include "test.h"

#include "remo.h"


//------------------------------------------------------------------------------
//
TEST(Failure, bind_with_bad_name)
{
    remo::LocalEndpoint endpoint;
    
    try {
        endpoint.bind("name with space", [](){});
        FAIL() << "must throw an exception";
    } catch (const remo::error& e) {
        EXPECT_EQ(e.code(), remo::ErrorCode::ERR_INVALID_IDENTIFIER);
    }
}

//------------------------------------------------------------------------------
//
TEST(Failure, call_with_param_type)
{
    remo::LocalEndpoint endpoint;
    remo::RemoteEndpoint* remote = endpoint.connect(".");

    endpoint.bind("testfunc", [](uint32_t){});

    try {
        remote->call("testfunc", 1234.5678);
        FAIL() << "must throw an exception";
    } catch (const remo::error& e) {
        EXPECT_EQ(e.code(), remo::ErrorCode::ERR_PARAM_TYPE_MISMATCH);
    }
}


//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
