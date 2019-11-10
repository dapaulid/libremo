#include "test.h"

#include "packet.h"
#include "reader.h"
#include "writer.h"
#include "function.h"
#include "dynamic_call.h"

static bool func_called = false;

uint32_t simple_func(uint32_t a1)
{
    EXPECT_EQ(a1, 1234);
    func_called = true;
    return 4321;
}

uint32_t multiple_params_func(uint32_t a1, double a2, const char* a3, bool a4)
{
    EXPECT_EQ(a1, 1234);
    EXPECT_EQ(a2, 123.456);
    EXPECT_STREQ(a3, "hello world");
    EXPECT_EQ(a4, true);
    func_called = true;
    return 1000;
}

void no_params_func()
{
    func_called = true;
}

//------------------------------------------------------------------------------

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
    remo::bound_function func("simple_func", &simple_func);
    uint32_t result = std::any_cast<uint32_t>(func.call(reader.get_args()));
    ASSERT_TRUE(func_called);
    ASSERT_EQ(4321, result);
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

TEST(serialization, multiple_params)
{
    remo::Packet packet;
    remo::BinaryWriter writer(packet);
    remo::BinaryReader reader(packet);

    writer.write_call("multiple_params", (uint32_t)1234, 123.456, "hello world", true);
    // ... transmit packet ...
    reader.read_call();

    func_called = false;
    TEST_PRINTF("calling %s\n", packet.to_string().c_str());
    uint32_t result = remo::dynamic_call(&multiple_params_func, reader.get_args());
    ASSERT_TRUE(func_called);
    ASSERT_EQ(1000, result);
}

//------------------------------------------------------------------------------
