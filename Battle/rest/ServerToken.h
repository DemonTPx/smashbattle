#pragma once

#include <string>

#include "AbstractRestBase.h"

namespace rest {

class ServerToken : public AbstractRestBase {
public:

	ServerToken();

	std::string get();

};

}