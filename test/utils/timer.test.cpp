#include "../test.h"

#include "utils/timer.h"

#include <future>

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------
//
using namespace remo::utils;

//------------------------------------------------------------------------------
//
TEST(Timer, single_shot)
{
	TimerThread tt;

	std::promise<void> p;
	auto f = p.get_future();

	int t1_called = 0;
	Timer t1(0.0, [&](){
		++t1_called;
		t1.cancel();
		p.set_value();
	}, &tt);

	f.wait();

	EXPECT_EQ(t1_called, 1);
}

//------------------------------------------------------------------------------
//
TEST(Timer, multi_shot)
{
	TimerThread tt;

	std::promise<void> p;
	auto f = p.get_future();

	int t1_called = 0;
	Timer t1(0.0, [&](){
		++t1_called;
		if (t1_called >= 3) {
			t1.cancel();
			p.set_value();
		}
	}, &tt);

	f.wait();

	EXPECT_EQ(t1_called, 3);
}

//------------------------------------------------------------------------------
//
TEST(Timer, multiple_timers)
{
	TimerThread tt;

	std::promise<void> p;
	auto f = p.get_future();

	int t1_called = 0;
	Timer t1(0.0, [&](){
		++t1_called;
		t1.cancel();
	}, &tt);

	int t2_called = 0;
	Timer t2(0.0, [&](){
		++t2_called;
		t2.cancel();
		p.set_value();
	}, &tt);	

	f.wait();

	EXPECT_EQ(t1_called, 1);
	EXPECT_EQ(t2_called, 1);
}

