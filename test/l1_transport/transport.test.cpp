#include "../test.h"

#include "l1_transport/transport.h"
#include "l1_transport/reader.h"
#include "l1_transport/writer.h"
#include "l1_transport/tcp/tcp-transport.h"

#include "utils/recycling.h"

#include <future>

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------
//
using namespace remo::trans;
using namespace remo;

//------------------------------------------------------------------------------
//
static void TestSendReceive(size_t a_payload_size)
{
	TcpTransport::Settings settings;
	settings.listen_addr = SockAddr("localhost:1986");
	TcpTransport transport(settings);

	std::promise<void> p;
	auto f = p.get_future();
	transport.on_accept([&p, a_payload_size](Channel* a_channel) {
		a_channel->on_receive([&p, a_payload_size](Channel*, packet_ptr& a_packet) {
			p.set_value();
			// check packet size
			EXPECT_EQ(a_packet->get_size(), a_payload_size);
			// check packet contents
			Reader reader(a_packet->get_payload());
			for (size_t i = 0; i < a_payload_size; i++)	{
				EXPECT_EQ(reader.read<uint8_t>(), (uint8_t)(i & 0xff));
			}
		});
	});

	// connect to remote
	Channel* channel = transport.connect("localhost:1986");
	// get a fresh packet
	packet_ptr packet = transport.take_packet();
	// write content
	Writer writer(packet->get_payload());
	for (size_t i = 0; i < a_payload_size; i++)	{
		writer.write<uint8_t>(i & 0xff);
	}
	// send the packet
	channel->send(packet);

	f.wait();
}

//------------------------------------------------------------------------------
//
TEST(Transport, SendReceive)
{
	// just send some bytes
	TestSendReceive(32);
}

//------------------------------------------------------------------------------
//
TEST(Transport, SendReceive_EmptyPayload)
{
	TestSendReceive(0);
}

//------------------------------------------------------------------------------
//
TEST(Transport, SendReceive_MaxPayload)
{
	TestSendReceive(REMO_MAX_PACKET_PAYLOAD_SIZE);
}

//------------------------------------------------------------------------------
//
TEST(Transport, SendReceive_Bad_TooLarge)
{
	try {
		TestSendReceive(REMO_MAX_PACKET_PAYLOAD_SIZE + 1);		
        FAIL() << "must throw an exception";
    } catch (const remo::error& e) {
        EXPECT_EQ(e.code(), remo::ErrorCode::ERR_PACKET_FULL);
    }
}
