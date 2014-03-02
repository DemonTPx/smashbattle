#pragma once

#include <string>

namespace network {
class Server;
class Client;

class ServerState
{
public:

	ServerState();
	virtual ~ServerState() = 0;

	virtual const std::string type() const = 0;

	virtual void initialize(Server &server) const {};
	virtual void execute(Server &server, Client &client) const {};
};

}