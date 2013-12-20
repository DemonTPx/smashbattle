#pragma once

#include <string>

#include "AbstractRestBase.h"

namespace json {
	class Array;
}

namespace rest {

class ServerList : public AbstractRestBase {
public:
	ServerList(const std::string &token);
	virtual ~ServerList();

	json::Array list();

private:

	std::string token_;

};

}