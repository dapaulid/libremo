#include "../test.h"

#include "l1_transport/url.h"

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------
//
using namespace remo::trans;

//------------------------------------------------------------------------------
//
TEST(Url, Empty)
{
	const std::string str = "";

	Url url(str);
	EXPECT_TRUE(url.proto.empty());
	EXPECT_EQ(url.host, "");
	EXPECT_EQ(url.path, "");
	EXPECT_EQ(url.to_string(), str);
}

//------------------------------------------------------------------------------
//
TEST(Url, ProtoHost)
{
	const std::string str = "remo://some-endpoint.com:1986";
	
	Url url(str);
	ASSERT_EQ(url.proto.size(), 1);
	EXPECT_EQ(url.proto[0], "remo");
	EXPECT_EQ(url.host, "some-endpoint.com:1986");
	EXPECT_EQ(url.path, "");
	EXPECT_EQ(url.to_string(), str);
}

//------------------------------------------------------------------------------
//
TEST(Url, ProtoHostPath)
{
	const std::string str = "remo://some-endpoint.com:1986/some/path";
	
	Url url(str);
	ASSERT_EQ(url.proto.size(), 1);
	EXPECT_EQ(url.proto[0], "remo");
	EXPECT_EQ(url.host, "some-endpoint.com:1986");
	EXPECT_EQ(url.path, "/some/path");
	EXPECT_EQ(url.to_string(), str);
}
