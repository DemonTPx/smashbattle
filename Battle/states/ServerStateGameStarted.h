#ifndef __INCLUDE__SERVERSTATEGAMESTARTED_H__
#define __INCLUDE__SERVERSTATEGAMESTARTED_H__

#include "ServerState.h"

#include <string>

#include <SDL/SDL_stdinc.h>

class ServerStateGameStarted : public ServerState
{

public:
	ServerStateGameStarted();

	const std::string type() const { return typeid(this).name(); }

	virtual void initialize(Server &server) const;

	virtual void execute(Server &server, Client &client) const;

private:

	friend class Server;
};


#endif //__INCLUDE__SERVERSTATEGAMESTARTED_H__