#include "../test.h"

#include "../../src/l0_system/socket.h"
#include "../../src/l0_system/error.h"

#include <string.h>

using namespace remo::sys;

//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv4Only)
{
	SockAddr addr("192.168.1.1");

	EXPECT_EQ(addr.get_family(), AddrFamily::IPv4);
	EXPECT_EQ(addr.get_port(), 0);
	EXPECT_STREQ(addr.to_string().c_str(), "192.168.1.1");
}


//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv4AndPort)
{
	SockAddr addr("127.0.0.1:1234");

	EXPECT_EQ(addr.get_family(), AddrFamily::IPv4);
	EXPECT_EQ(addr.get_port(), 1234);
	EXPECT_STREQ(addr.to_string().c_str(), "127.0.0.1:1234");
}

//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv6Only)
{
	SockAddr addr("fe80::84d7:6fc0:f355:921c");

	EXPECT_EQ(addr.get_family(), AddrFamily::IPv6);
	EXPECT_EQ(addr.get_port(), 0);
	EXPECT_STREQ(addr.to_string().c_str(), "fe80::84d7:6fc0:f355:921c");
}

//------------------------------------------------------------------------------
//
TEST(SockAddr, IPv6AndPort)
{
	SockAddr addr("[::1]:1234");

	EXPECT_EQ(addr.get_family(), AddrFamily::IPv6);
	EXPECT_EQ(addr.get_port(), 1234);
	EXPECT_STREQ(addr.to_string().c_str(), "[::1]:1234");
}

//------------------------------------------------------------------------------
//
TEST(SockAddr, Invalid)
{
	try {
        SockAddr addr("invalid");
        FAIL() << "must not be reached";
    } catch (const remo::error& e) {
        EXPECT_EQ(e.code(), remo::ErrorCode::ERR_GETADDRINFO_FAILED);
    }
}

//------------------------------------------------------------------------------
//
TEST(Socket, SendReceive_UDP)
{
	const char sendbuf [] = "Hello World!";
	char recvbuf [32];

    Socket s1(SockProto::UDP);
	Socket s2(SockProto::UDP);

	s2.bind(SockAddr::localhost);

	s1.connect(s2.get_socket_addr());
	size_t bytes_sent = s1.send(sendbuf, sizeof(sendbuf));
	EXPECT_EQ(bytes_sent, sizeof(sendbuf));

	size_t bytes_received = s2.receive(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(bytes_received, bytes_sent);
	EXPECT_TRUE(memcmp(recvbuf, sendbuf, sizeof(sendbuf)) == 0);
}

//------------------------------------------------------------------------------
//
TEST(Socket, SendReceive_TCP)
{
	const char sendbuf [] = "Hello World!";
	char recvbuf [32];

    Socket s1(SockProto::TCP);
	Socket s2(SockProto::TCP);

	s2.bind(SockAddr::localhost);
	s2.listen();

	s1.connect(s2.get_socket_addr());
	Socket s3 = s2.accept();
	
	size_t bytes_sent = s1.send(sendbuf, sizeof(sendbuf));
	EXPECT_EQ(bytes_sent, sizeof(sendbuf));

	size_t bytes_received = s3.receive(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(bytes_received, bytes_sent);
	EXPECT_TRUE(memcmp(recvbuf, sendbuf, sizeof(sendbuf)) == 0);
}


//------------------------------------------------------------------------------
//
TEST(SocketSet, Poll)
{
    Socket s1(SockProto::UDP);
	Socket s2(SockProto::UDP);

	SocketSet ss;
	ss.add(&s1);
	ss.add(&s2);

	// set receive handlers
	bool s1_ready = false;
	s1.on_receive_ready([&](Socket* s){
		s1_ready = true;
		EXPECT_EQ(s, &s1);
	});
	bool s2_ready = false;
	s2.on_receive_ready([&](Socket* s){
		s2_ready = true;
		EXPECT_EQ(s, &s2);
	});

	// connect s1 <==> s2
	s1.bind(SockAddr::localhost);
	s2.bind(SockAddr::localhost);
	s1.connect(s2.get_socket_addr());
	s2.connect(s1.get_socket_addr());

	// no socket should be ready first
	s1_ready = s2_ready = false;
	size_t num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, 0);
	EXPECT_FALSE(s1_ready);
	EXPECT_FALSE(s2_ready);

	// send data from s1 to s2
	const char data [] = "Hello";
	size_t bytes_sent = s1.send(data, sizeof(data));
	EXPECT_EQ(bytes_sent, sizeof(data));

	// s2 should be ready now
	s1_ready = s2_ready = false;
	num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, 1);
	EXPECT_FALSE(s1_ready);
	EXPECT_TRUE(s2_ready);

	// s2 should still be ready
	s1_ready = s2_ready = false;
	num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, 1);
	EXPECT_FALSE(s1_ready);
	EXPECT_TRUE(s2_ready);
}

//------------------------------------------------------------------------------
//
TEST(SocketSet, PollShutdown)
{
    Socket s1(SockProto::TCP);
	Socket s2(SockProto::TCP);

	s2.bind(SockAddr::localhost);
	s2.listen();

	s1.connect(s2.get_socket_addr());
	Socket s3 = s2.accept();

	SocketSet ss;
	ss.add(&s1);
	ss.add(&s2);
	ss.add(&s3);

	// no socket should be ready first
	size_t num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, 0);

	// shutdown s1
	s1.shutdown();

	// sockets s1 and s3 should be ready
	num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, 2);
	
	// s3 and s1 should "receive" 0 bytes -> orderly shutdown by peer
	char recvbuf [32];
	size_t bytes_received = s3.receive(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(bytes_received, 0);
	bytes_received = s1.receive(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(bytes_received, 0);

	// sockets s1 and s3 should still be ready (by observation)
	num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, 2);
}
