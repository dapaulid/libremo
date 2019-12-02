#include "../test.h"

#include "../../src/l0_system/logger.h"

//------------------------------------------------------------------------------
//
TEST(Logger, example)
{
    remo::Logger logger("LoggerTest");

	logger.fatal("This is an example fatal message");
	logger.error("This is an example error message");
	logger.warning("This is an example warning message");
	logger.info("This is an example info message");
	logger.verbose("This is an example verbose message");
}

//------------------------------------------------------------------------------
//
TEST(Logger, filter)
{
    remo::Logger logger("LoggerTest");

	logger.set_global_level(remo::LogLevel::eLogWarning);
	logger.info("This info message should not be logged");
}

//------------------------------------------------------------------------------
//
TEST(Logger, custom)
{
    remo::Logger logger("LoggerTest");

	logger.set_global_level(remo::eLogAll);
	logger.custom(99, "This is an example custom message");
}
