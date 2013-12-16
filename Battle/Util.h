#pragma once

#include <string>
#include <vector>

namespace util
{

const std::string basedir(const std::string & fullpath);

void split(const std::string& s, char c, std::vector<std::string>& v);

};