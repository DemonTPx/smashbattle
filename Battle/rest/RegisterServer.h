#pragma once

#include <string>

#include "AbstractRestBase.h"

class Main;

namespace rest {

class RegisterServer : public AbstractRestBase {
public:

	RegisterServer(const std::string &token);

	std::string put(Main &main);
	
private:
	
	std::string serverToken_;

};

}