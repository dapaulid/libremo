#include "../test.h"

#include "../../src/l0_system/logger.h"

//------------------------------------------------------------------------------
//
TEST(Logger, example)
{
    remo::Logger logger("LoggerTest");

	REMO_FATAL("This is an example fatal message");
	REMO_ERROR("This is an example error message");
	REMO_WARN("This is an example warning message");
	REMO_INFO("This is an example info message");
	REMO_VERB("This is an example verbose message");
}

//------------------------------------------------------------------------------
//
TEST(Logger, filter)
{
    remo::Logger logger("LoggerTest");

	logger.set_global_level(remo::LogLevel::eLogWarning);
	REMO_INFO("This info message should not be logged");
}

//------------------------------------------------------------------------------
//
TEST(Logger, custom)
{
    remo::Logger logger("LoggerTest");

	logger.set_global_level(remo::eLogAll);
	REMO_LOG(99, "This is an example custom message");
}
