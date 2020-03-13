#include "../test.h"

#include "../../src/l0_system/socket.h"
#include "../../src/l0_system/error.h"

#include <future>
#include <string.h>
#include <chrono>

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------
//
static const int POLL_TIMEOUT = 100; // ms
static const auto ASYNC_NO_WAIT = std::chrono::milliseconds(0);
static const auto ASYNC_PSEUDO_NO_WAIT = std::chrono::milliseconds(10);


//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------
//
void sleep(int a_timeout_ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(a_timeout_ms));
}


//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------
//
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
TEST(SockAddr, AnyAddr)
{
	SockAddr addr(":1234");

	EXPECT_EQ(addr.get_port(), 1234);
	EXPECT_EQ(addr.get_family(), SockAddr::localhost.get_family());
	if (addr.get_family() == AddrFamily::IPv4) {
		EXPECT_STREQ(addr.to_string().c_str(), "0.0.0.0:1234");
	} else {
		EXPECT_STREQ(addr.to_string().c_str(), "[::]:1234");
	}
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
TEST(Socket, NonBlockingReceive)
{
	char recvbuf [32];

	Socket s1(SockProto::UDP);
	s1.connect(SockAddr("localhost:12345"));
	s1.set_blocking(false);

	// attempt to receive any data should block
	Socket::IOResult recv_result = s1.recv(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(recv_result, Socket::IOResult::WouldBlock);
}

//------------------------------------------------------------------------------
//
TEST(Socket, SendReceive_UDP)
{
	const char sendbuf [] = "Hello World!";
	char recvbuf [sizeof(sendbuf)];

    Socket s1(SockProto::UDP);
	Socket s2(SockProto::UDP);

	s2.bind(SockAddr::localhost);

	s1.connect(s2.get_socket_addr());
	Socket::IOResult send_result = s1.send(sendbuf, sizeof(sendbuf));
	EXPECT_EQ(send_result, Socket::IOResult::Success);

	Socket::IOResult recv_result = s2.recv(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(recv_result, Socket::IOResult::Success);
	EXPECT_TRUE(memcmp(recvbuf, sendbuf, sizeof(sendbuf)) == 0);
}

//------------------------------------------------------------------------------
//
TEST(Socket, SendReceive_TCP)
{
	const char sendbuf [] = "Hello World!";
	char recvbuf [sizeof(sendbuf)];

    Socket s1(SockProto::TCP);
	Socket s2(SockProto::TCP);

	s2.bind(SockAddr::localhost);
	s2.listen();

	s1.connect(s2.get_socket_addr());
	Socket s3 = s2.accept();
	
	Socket::IOResult send_result = s1.send(sendbuf, sizeof(sendbuf));
	EXPECT_EQ(send_result, Socket::IOResult::Success);

	Socket::IOResult recv_result = s3.recv(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(recv_result, Socket::IOResult::Success);
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
	s1.on_receive_ready([&](){
		s1_ready = true;
	});
	bool s2_ready = false;
	s2.on_receive_ready([&](){
		s2_ready = true;
	});

	// connect s1 <==> s2
	s1.bind(SockAddr::localhost);
	s2.bind(SockAddr::localhost);
	s1.connect(s2.get_socket_addr());
	s2.connect(s1.get_socket_addr());

	// no socket should be ready first
	s1_ready = s2_ready = false;
	size_t num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, (size_t)0);
	EXPECT_FALSE(s1_ready);
	EXPECT_FALSE(s2_ready);

	// send data from s1 to s2
	const char data [] = "Hello";
	Socket::IOResult send_result = s1.send(data, sizeof(data));
	EXPECT_EQ(send_result, Socket::IOResult::Success);

	// s2 should be ready now
	s1_ready = s2_ready = false;
	num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, (size_t)1);
	EXPECT_FALSE(s1_ready);
	EXPECT_TRUE(s2_ready);

	// s2 should still be ready
	s1_ready = s2_ready = false;
	num_ready = ss.poll(NO_WAIT);
	EXPECT_EQ(num_ready, (size_t)1);
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
	EXPECT_EQ(num_ready, (size_t)0);

	// shutdown s1
	s1.shutdown();

	// socket s3 should be ready
	num_ready = ss.poll(NO_WAIT);
	//EXPECT_GT(num_ready, (size_t)0); // 1 under Windows, 2 under Linux...
	
	// s3 should "receive" orderly shutdown by peer
	char recvbuf [32];
	Socket::IOResult recv_result = s3.recv(recvbuf, sizeof(recvbuf));
	EXPECT_EQ(recv_result, Socket::IOResult::PeerShutdown);

	// socket s3 should still be ready (by observation)
	num_ready = ss.poll(NO_WAIT);
	//EXPECT_GT(num_ready, (size_t)0); // 1 under Windows, 2 under Linux...
}

//------------------------------------------------------------------------------
//
TEST(SocketSet, PollShutdown_async)
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
	EXPECT_EQ(num_ready, (size_t)0);

	// do a blocking poll in a separate thread
	std::future<size_t> a = std::async(std::launch::async, [&](){
		return ss.poll(POLL_TIMEOUT);
	});

	// shutdown s1 in the middle of asynchronous poll
	sleep(POLL_TIMEOUT / 2);
	std::future_status status = a.wait_for(ASYNC_NO_WAIT);
	EXPECT_EQ(status, std::future_status::timeout);
	s1.shutdown();
	
	// asynchronous poll should return
	status = a.wait_for(ASYNC_PSEUDO_NO_WAIT);
	EXPECT_EQ(status, std::future_status::ready);

	// socket s3 should be ready
	num_ready = a.get();
	//EXPECT_GT(num_ready, (size_t)0); // 1 under Windows, 2 under Linux...
}
