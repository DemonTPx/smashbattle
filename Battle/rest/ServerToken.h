#pragma once

#include <string>

#include "AbstractRestBase.h"

#define SKIP_SERVER_TOKEN_PROCESS

namespace rest {

class ServerToken : public AbstractRestBase {
public:

	ServerToken();

	std::string get();

};

}