#pragma once

#include <string>

#include "AbstractRestBase.h"

namespace rest {

class ClientToken : public AbstractRestBase {
public:

	ClientToken();

	std::string get();

};

}