#pragma once

#include "ServerState.h"

class ServerStateInactive : public ServerState
{

public:
	ServerStateInactive();

	const std::string type() const { return typeid(this).name(); }
};
