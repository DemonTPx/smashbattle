#pragma once

#include <string>

#include "AbstractRestBase.h"

namespace rest {

class RegisterServer : public AbstractRestBase {
public:

	RegisterServer(const std::string &token);

	std::string put();
	
private:
	
	std::string serverToken_;

};

}