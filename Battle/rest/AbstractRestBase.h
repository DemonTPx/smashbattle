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

	json::Object request(const std::string &method, const std::string &url, const std::string &token = "", const std::string &jsonString = "");

private:

};

}