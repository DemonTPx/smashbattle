#ifndef __INCLUDE__SERVERSTATE_H__
#define __INCLUDE__SERVERSTATE_H__

#include <string>

class Server;
class Client;

class ServerState
{
public:

	ServerState();
	virtual ~ServerState() = 0;

	virtual int test() const = 0;
	virtual const std::string type() const = 0;

	virtual void initialize(Server &server) const {};
	virtual void execute(Server &server, Client &client) const {};
};


#endif //__INCLUDE__SERVERSTATE_H__