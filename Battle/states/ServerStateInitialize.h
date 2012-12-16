#ifndef __INCLUDE__SERVERSTATEINITIALIZE_H__
#define __INCLUDE__SERVERSTATEINITIALIZE_H__

#include "ServerState.h"

#include <string>

class ServerStateInitialize : public ServerState
{

public:
	ServerStateInitialize(std::string level, int port);

	int test() const ;

	const std::string type() const { return typeid(this).name(); }

	virtual void initialize(Server &server) const;

	virtual void execute(Server &server, Client &client) const;

private:
	std::string level_;
	int port_;

	friend class Server;
};


#endif //__INCLUDE__SERVERSTATEINITIALIZE_H__