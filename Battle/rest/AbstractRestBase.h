#pragma once

#include <string>

namespace json {
	class Object;
}

namespace rest {

class AbstractRestBase {
public:
	AbstractRestBase();
	virtual ~AbstractRestBase();

	json::Object request(const std::string &url, const std::string &token = "");

private:

};

}