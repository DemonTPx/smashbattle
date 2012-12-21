#ifndef __INCLUDE__SERVERSTATEACCEPTCLIENTS_H__
#define __INCLUDE__SERVERSTATEACCEPTCLIENTS_H__

#include "ServerState.h"

#include <string>

#include <SDL/SDL_stdinc.h>

class ServerStateAcceptClients : public ServerState
{

public:
	ServerStateAcceptClients();

	const std::string type() const { return typeid(this).name(); }

	virtual void initialize(Server &server) const;

	virtual void execute(Server &server, Client &client) const;

private:

	friend class Server;

	mutable Uint32 gameStart_;
	mutable Uint32 gameStartSendTime_;
	mutable Uint32 gameStartForPlayers_;
};


#endif //__INCLUDE__SERVERSTATEACCEPTCLIENTS_H__