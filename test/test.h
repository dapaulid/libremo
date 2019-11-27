#pragma once

#include "gtest/gtest.h"

// hacky way to pretty-print additional traces during test
// https://stackoverflow.com/questions/16491675/how-to-send-custom-message-in-google-c-testing-framework

namespace testing
{
 namespace internal
 {

  extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
 }
}
#define TEST_PRINTF(...)  do { testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] "); testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__); } while(0)

// C++ stream interface
class TestCout : public std::stringstream
{
public:
    ~TestCout()
    {
        TEST_PRINTF("%s",str().c_str());
    }
};

#define TEST_COUT  TestCout()
