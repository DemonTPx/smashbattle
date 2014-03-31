#include "util/Log.h"

#undef log

Logger::Priority Logger::currentprio = Logger::Priority::INFO;

std::vector<std::string> Logger::console;

bool LoggerLogToStdOut = false;

std::string format(const char *format, ...)
{
	char buffer[4086] = {0x00};

	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	return std::string(buffer);
}

void Logger::log(std::string filename, long line, std::string log, Logger::Priority priority)
{
	if (LoggerLogToStdOut && priority >= Logger::currentprio)
		std::cout << Logger::toString(priority) << " " << log << std::endl;

	if (priority == Logger::Priority::CONSOLE)
		Logger::console.push_back(log);
}