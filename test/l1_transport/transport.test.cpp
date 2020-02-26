#include "../test.h"

#include "l1_transport/transport.h"
#include "l1_transport/tcp/tcp-transport.h"


//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------
//
using namespace remo::trans;

//------------------------------------------------------------------------------
//
TEST(Transport, Foo)
{
	TcpTransport::Settings settings;
	TcpTransport transport(settings);

	//Channel* channel = transport.connect(":1986");
	//(void)channel;

	//transport.join();
}
