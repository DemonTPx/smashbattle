#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>
#include <vector>
#include <string>

#include <cstdarg>

#undef ERROR

class Logger
{
public:

	enum Priority
	{
		DEBUG = 10,
		INFO = 20,
		WARNING = 30,
		ERROR = 40,
		FATAL = 50,
		CONSOLE = 100
	};

	static Priority currentprio;

	static std::vector<std::string> console;

	static void log(std::string filename, long line, std::string log, Logger::Priority priority)
	{
		if (priority >= Logger::currentprio)
			std::cout << Logger::toString(priority) << " " << log << std::endl;
		
		if (priority == Logger::Priority::CONSOLE)
			Logger::console.push_back(log);

	}

private:

	static std::string toString(Logger::Priority prio)
	{
		switch (prio)
		{
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

// for converting lower/uppercase 
// from "C++ Cookbook"
#include <locale>
#include <cctype>
#include <cwctype>

template<typename C>
void to_upper(std::basic_string<C>& s, const std::locale& loc = std::locale( )) {
	typename std::basic_string<C>::iterator p;
	for (p = s.begin( ); p != s.end( ); ++p) {
		*p = std::use_facet<std::ctype<C> >(loc).toupper(*p);
	}
}

template<typename C>
void to_lower(std::basic_string<C>& s, const std::locale& loc = std::locale( )) {
	typename std::basic_string<C>::iterator p;
	for (p = s.begin( ); p != s.end( ); ++p) {
		*p = std::use_facet<std::ctype<C> >(loc).tolower(*p);
	}
}

#endif //__LOG_HPP__