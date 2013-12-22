#include "ServerStates.h"
#include "network/Server.h"

#include "util/Log.h"

using std::string;

ServerStateInitialize::ServerStateInitialize(string level, int port, std::string servername)
	: level_(level), port_(port), servername_(servername)
{
	log("ServerStateInitialize constructor", Logger::Priority::DEBUG);
	log(string("level = " + level), Logger::Priority::DEBUG);
	log(format("port = %d", port), Logger::Priority::DEBUG);
	log(string("servername = " + servername), Logger::Priority::DEBUG);
}

void ServerStateInitialize::initialize(Server &server) const
{
	server.setLevel(level_);
	server.setPort(port_);
	server.setName(servername_);
}

void ServerStateInitialize::execute(Server &server, Client &client) const
{
}