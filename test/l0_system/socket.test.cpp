#include "../test.h"

#include "../../src/l0_system/socket.h"
#include "../../src/l0_system/error.h"

//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv4Only)
{
	remo::sys::SockAddr addr("192.168.1.1");

	EXPECT_EQ(addr.get_family(), remo::sys::AddrFamily::IPv4);
	EXPECT_EQ(addr.get_port(), 0);
	EXPECT_STREQ(addr.to_string().c_str(), "192.168.1.1");
}


//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv4AndPort)
{
	remo::sys::SockAddr addr("127.0.0.1:1234");

	EXPECT_EQ(addr.get_family(), remo::sys::AddrFamily::IPv4);
	EXPECT_EQ(addr.get_port(), 1234);
	EXPECT_STREQ(addr.to_string().c_str(), "127.0.0.1:1234");
}

//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv6Only)
{
	remo::sys::SockAddr addr("fe80::84d7:6fc0:f355:921c");

	EXPECT_EQ(addr.get_family(), remo::sys::AddrFamily::IPv6);
	EXPECT_EQ(addr.get_port(), 0);
	EXPECT_STREQ(addr.to_string().c_str(), "fe80::84d7:6fc0:f355:921c");
}

//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv6AndPort)
{
	remo::sys::SockAddr addr("[::1]:1234");

	EXPECT_EQ(addr.get_family(), remo::sys::AddrFamily::IPv6);
	EXPECT_EQ(addr.get_port(), 1234);
	EXPECT_STREQ(addr.to_string().c_str(), "[::1]:1234");
}

//------------------------------------------------------------------------------
//
TEST(SockAddr, Invalid)
{
	try {
        remo::sys::SockAddr addr("invalid");
        FAIL() << "must not be reached";
    } catch (const remo::error& e) {
        EXPECT_EQ(e.code(), remo::ErrorCode::ERR_GETADDRINFO_FAILED);
    }
}
