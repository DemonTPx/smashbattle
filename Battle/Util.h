#ifndef __INCLUDE_UTIL_H__
#define __INCLUDE_UTIL_H__

#include <string>
#include <vector>

namespace util
{

const std::string basedir(const std::string & fullpath);

void split(const std::string& s, char c, std::vector<std::string>& v);

};

#endif // __INCLUDE_UTIL_H__