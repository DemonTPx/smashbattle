#pragma once

#include "ServerState.h"

namespace network {

class ServerStateInactive : public ServerState
{

public:
	ServerStateInactive();

	const std::string type() const { return typeid(this).name(); }
};

}