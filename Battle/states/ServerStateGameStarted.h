#pragma once

#include "ServerState.h"

#include <string>

#include <SDL2/SDL_stdinc.h>

namespace network {

class ServerStateGameStarted : public ServerState
{

public:
	ServerStateGameStarted();

	const std::string type() const { return typeid(this).name(); }

	virtual void initialize(Server &server) const;

	virtual void execute(Server &server, Client &client) const;

	virtual ServerState * check_self(Server &server) const;

private:

	friend class Server;

	/**
	 * Flag used to send 'Welcome to the game' only once..
	 * (flag is reset in initialize())
	 */
	mutable bool once;
};

}
