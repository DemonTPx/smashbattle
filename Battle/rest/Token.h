#pragma once

#include <string>

#include "AbstractRestBase.h"

namespace rest {

class Token : public AbstractRestBase {
public:

	Token();

	std::string get();

};

}