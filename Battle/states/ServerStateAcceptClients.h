#pragma once

#include "ServerState.h"

#include <string>

#include <SDL2/SDL_stdinc.h>

class Player;

namespace network {

/**
 * \brief Server state where it's accepting new clients to make a game possible.
 * 
 * Once there are two players the countdown will begin before the state is changed from "AcceptClients" into "GameStarted".
 */
class ServerStateAcceptClients : public ServerState
{

public:
	ServerStateAcceptClients();

	const std::string type() const { return typeid(this).name(); }

	virtual void initialize(Server &server) const;
	virtual void transform_spectators_into_valid_players(Server &server) const;

	virtual void execute(Server &server, Client &client) const;

protected:

	virtual void execute_connecting(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_commtoken_requested(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_calculating_lag(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_initializing(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_character_requested(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_character_initialized(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_serverside_ready(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_ready_for_positional_data(Uint32 servertime, Server &server, Client &client) const;
	virtual void execute_active(Uint32 servertime, Server &server, Client &client) const;

	virtual void execute_gamestart(Uint32 servertime, Server &server, Client &client, size_t numActiveClients) const;
	virtual void execute_gameupdate(Uint32 servertime, Server &server, Client &client, size_t numActiveClients) const;

	virtual void server_add_player(Server &server, Client &client, Player * const newplayer) const;
	
private:

	friend class Server;

	mutable Uint32 gameStart_;
	mutable Uint32 gameStartSendTime_;
	mutable Uint32 gameStartForPlayers_;
};

}
