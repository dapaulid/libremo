#include "../test.h"

#include "utils/logger.h"

using namespace remo;

//------------------------------------------------------------------------------
//
TEST(Logger, example)
{
    Logger logger("LoggerTest");

	REMO_FATAL("This is an example fatal message");
	REMO_ERROR("This is an example error message");
	REMO_WARN("This is an example warning message");
	REMO_NOTE("This is an example notice message");
	REMO_INFO("This is an example info message");
	REMO_VERB("This is an example verbose message");
}

//------------------------------------------------------------------------------
//
TEST(Logger, filter)
{
    Logger logger("LoggerTest");

	logger.set_global_level(LogLevel::eLogWarn);
	REMO_INFO("This info message should not be logged");
}

//------------------------------------------------------------------------------
//
TEST(Logger, custom)
{
    Logger logger("LoggerTest");

	logger.set_global_level(LogLevel::eLogAll);
	REMO_LOG(99, "This is an example custom message");
}
