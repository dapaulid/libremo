#include "../test.h"

#include "l1_transport/transport.h"
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
TEST(Transport, SendReceive)
{
	RecyclingPool<Packet> pool;
	pool.add(new Packet());
	packet_ptr packet(pool.take());

	TcpTransport::Settings settings;
	TcpTransport transport(settings);

	std::promise<void> p;
	auto f = p.get_future();
	transport.on_accept([&p](Channel* a_channel) {
		a_channel->on_receive([&p](Channel*, packet_ptr& a_packet) {
			p.set_value();
			EXPECT_EQ(a_packet->get_size(), (size_t)5);
		});
	});

	Channel* channel = transport.connect(":1986");
	BinaryWriter writer(*packet);
	writer.write(5);
	writer.write(0);
	writer.write(0);
	writer.write(0);
	writer.write(6);
	channel->send(packet);

	f.wait();
}
