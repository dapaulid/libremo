#include "../test.h"

#include "utils/active.h"

#include <future>

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------
//
using namespace remo::utils;


//------------------------------------------------------------------------------
//
TEST(Active, dispatch)
{
	class MyActiveObject: public ActiveObject
	{
	public:
		MyActiveObject(bool& a_called): 
			ActiveObject(), m_called(a_called) {}
	public:
		// outer function (wrapper) to be called by any thread
		ActiveFunction<MyActiveObject, void, int, const char*> foo{this, &MyActiveObject::i_foo};
	protected:
		// inner function to be called by our own thread
		void i_foo(int a, const char* b) {
			EXPECT_TRUE(is_self());
			EXPECT_EQ(a, 1234);
			EXPECT_STREQ(b, "hello");
			m_called = true;
		}
	private:
		bool& m_called;
	};

	bool called = false;
	{
		MyActiveObject ao(called);
	
		ao.foo(1234, "hello");
	}
	EXPECT_TRUE(called);
}

