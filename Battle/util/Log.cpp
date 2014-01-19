#include "util/Log.h"


Logger::Priority Logger::currentprio = Logger::Priority::WARNING;

std::vector<std::string> Logger::console;

std::string format(const char *format, ...)
{
	char buffer[4086] = {0x00};

	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	return std::string(buffer);
}

