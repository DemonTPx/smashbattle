#pragma once

// for converting lower/uppercase 
// from "C++ Cookbook"
#include <locale>
#include <cctype>
#include <cwctype>

template<typename C>
void to_upper(std::basic_string<C>& s, const std::locale& loc = std::locale()) {
	typename std::basic_string<C>::iterator p;
	for (p = s.begin(); p != s.end(); ++p) {
		*p = std::use_facet<std::ctype<C> >(loc).toupper(*p);
	}
}

template<typename C>
void to_lower(std::basic_string<C>& s, const std::locale& loc = std::locale()) {
	typename std::basic_string<C>::iterator p;
	for (p = s.begin(); p != s.end(); ++p) {
		*p = std::use_facet<std::ctype<C> >(loc).tolower(*p);
	}
}

inline std::string string_to_hex(const std::string& input) {
	static const char* const lut = "0123456789abcdef";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i) {
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}