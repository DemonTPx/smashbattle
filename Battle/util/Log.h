#pragma once

#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>

#include <cstdarg>

#undef ERROR

extern bool LoggerLogToStdOut;

class Logger {
public:

	enum Priority {
		DEBUG = 10,
		INFO = 20,
		WARNING = 30,
		ERROR = 40,
		FATAL = 50,
		CONSOLE = 100
	};

	static Priority currentprio;

	static std::vector<std::string> console;

	static void log(std::string filename, long line, std::string log, Logger::Priority priority);

private:

	static std::string toString(Logger::Priority prio) {
		switch (prio) {
			case Logger::Priority::DEBUG:
				return "DEBUG";
				break;
			case Logger::Priority::INFO:
				return "INFO";
				break;
			case Logger::Priority::WARNING:
				return "WARNING";
				break;
			case Logger::Priority::ERROR:
				return "ERROR";
				break;
			case Logger::Priority::FATAL:
				return "FATAL";
				break;
			case Logger::Priority::CONSOLE:
				return "CONSOLE";
				break;
		}
		throw std::runtime_error("invalid Logger::Priority given");
	}
};

#define log(str, prio) Logger::log(__FILE__, __LINE__, str, prio);


// for if you are too lazy using boost::format in your project

extern std::string format(const char *format, ...);

#include "stringutils.hpp"