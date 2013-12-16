#include "ServerStates.h"
#include "network/Server.h"

#include "util/Log.h"

using std::string;

ServerStateInitialize::ServerStateInitialize(string level, int port)
	: level_(level), port_(port)
{
	log("ServerStateInitialize constructor", Logger::Priority::DEBUG);
	log(string("level = " + level), Logger::Priority::DEBUG);
	log(format("port = %d", port), Logger::Priority::DEBUG);
}

void ServerStateInitialize::initialize(Server &server) const
{
	server.setLevel(level_);
	server.setPort(port_);
}

void ServerStateInitialize::execute(Server &server, Client &client) const
{
}