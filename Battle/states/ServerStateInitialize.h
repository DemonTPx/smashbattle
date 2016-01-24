#pragma once

#include "ServerState.h"

#include <string>

namespace network {

class ServerStateInitialize : public ServerState
{

public:
	ServerStateInitialize(std::string level, int port, std::string servername);

	const std::string type() const { return typeid(this).name(); }

	virtual void initialize(Server &server) const;

	virtual void execute(Server &server, Client &client) const;

private:
	std::string level_;
	int port_;
	std::string servername_;

	friend class Server;
};

}