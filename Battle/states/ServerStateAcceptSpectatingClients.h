#pragma once

#include "ServerState.h"

#include <string>

#include <SDL/SDL_stdinc.h>

class Player;

namespace network {

/**
 * \brief Server state where it's accepting new clients as spectators in an existing game.
 * 
 * This state is used in "GameStarted", so it's actually a "parallel" state that can exist 
 * when new clients connect while a game is already existent.
 */
class ServerStateAcceptSpectatingClients : public ServerStateAcceptClients
{

public:
	ServerStateAcceptSpectatingClients();

	const std::string type() const { return typeid(this).name(); }

	virtual void initialize(Server &server) const;
	virtual void transform_spectators_into_valid_players(Server &server) const;

	virtual void execute(Server &server, Client &client) const;

protected:

	void execute_gamestart(Uint32 servertime, Server &server, Client &client, size_t numActiveClients) const;

	virtual void server_add_player(Server &server, Client &client, Player * const player) const;

private:

	friend class Server;

	mutable bool once;

};

}