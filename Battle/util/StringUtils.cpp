#include "util/StringUtils.h"

#include <functional>
#include <iostream>

using std::string;
using std::vector;

namespace util
{

const string basedir(const string &fullpath)
{
#ifdef _WIN32
	char pathsep = '\\';
#else
	char pathsep = '/';
#endif
	vector<string> tokens;
	util::split(fullpath, pathsep, tokens);
	string cwd;
	for (vector<string>::iterator i = tokens.begin(); i!=tokens.end(); i++) {
		if (cwd != "")
			cwd.append(std::string(1, pathsep)); // implode tokens with \'s
		if (i + 1 == tokens.end())
			break; // discard last tokenized (executable name)
		cwd.append(*i);
	}
	return cwd;
}

// From "The C++ Cookbook"
void split(const string& s, char c, vector<string>& v)
{
	string::size_type i = 0;
	string::size_type j = s.find(c);

	while (j != string::npos) {
		v.push_back(s.substr(i, j-i));
		i = ++j;
		j = s.find(c, j);

		if (j == string::npos)
			v.push_back(s.substr(i, s.length( )));
	}
}

}
