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
		// NOTE in C++17 it is not necessary to specify template parameters,
		// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0091r3.html
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
	
		// NOTE cast seems necessary under MSVC 19.25.28610.4
		ao.foo(1234, (const char*)"hello");
	}
	EXPECT_TRUE(called);
}

