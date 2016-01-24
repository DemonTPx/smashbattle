#pragma once

#include <string>
#include <future>

#include "AbstractRestBase.h"

class Main;

namespace rest {

class RegisterServer : public AbstractRestBase {
public:

	RegisterServer(const std::string &token);

	std::string put(Main &main);
	void update(Main &main);
	
private:
	
	std::string serverToken_;

	std::future<void> future_;

};

}