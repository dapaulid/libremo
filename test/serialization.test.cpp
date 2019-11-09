#include "test.h"

#include "packet.h"
#include "reader.h"
#include "writer.h"
#include "dynamic_call.h"

static bool func_called = false;

uint32_t simple_func(uint32_t a1)
{
    EXPECT_EQ(a1, 1234);
    func_called = true;
    return 4321;
}

void no_params_func()
{
    func_called = true;
}

TEST(serialization, simple)
{
    remo::Packet packet;
    remo::BinaryWriter writer(packet);
    remo::BinaryReader reader(packet);

    writer.write_call("simple_func", (uint32_t)1234);
    // ... transmit packet ...
    reader.read_call();

    func_called = false;
    TEST_PRINTF("calling %s\n", packet.to_string().c_str());
    uint32_t result = remo::dynamic_call(&simple_func, reader.get_args());
    ASSERT_TRUE(func_called);
    ASSERT_EQ(4321, result);
}

TEST(serialization, no_params)
{
    remo::Packet packet;
    remo::BinaryWriter writer(packet);
    remo::BinaryReader reader(packet);

    writer.write_call("no_params");
    // ... transmit packet ...
    reader.read_call();

    func_called = false;
    TEST_PRINTF("calling %s\n", packet.to_string().c_str());
    remo::dynamic_call(&no_params_func, reader.get_args());
    ASSERT_TRUE(func_called);
}