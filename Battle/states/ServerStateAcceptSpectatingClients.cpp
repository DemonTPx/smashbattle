#include "ServerStates.h"
#include "network/Server.h"
#include <vector>

#include "util/Log.h"

#include "network/Commands.hpp"
#include "Player.h"
#include "NetworkMultiplayer.h"

using std::string;

namespace network {

ServerStateAcceptSpectatingClients::ServerStateAcceptSpectatingClients()
	: ServerStateAcceptClients()
{
}

void ServerStateAcceptSpectatingClients::initialize(Server &server) const
{
	ServerStateAcceptClients::initialize(server);
}

void ServerStateAcceptSpectatingClients::transform_spectators_into_valid_players(Server &server) const
{
}

// Todo : maybe more protocol related stuff should be in here,
//   and move the rest to pre and post processing in NetworkMultiplayer
void ServerStateAcceptSpectatingClients::execute(Server &server, Client &client) const
{
	ServerStateAcceptClients::execute(server, client);
}

void ServerStateAcceptSpectatingClients::execute_gamestart(Uint32 servertime, Server &server, Client &client, size_t numActiveClients) const
{
	if (client.getState() != Client::State::ACTIVE)
		return;

	CommandSetPlayerDeath spectatorisdead;
	spectatorisdead.data.time = server.getServerTime();
	spectatorisdead.data.client_id = client.getClientId();
	spectatorisdead.data.is_dead = true;
	server.sendAll(spectatorisdead);

	CommandSetSpectating msgspec;
	msgspec.data.time = server.getServerTime();
	msgspec.data.is_spectating = true;
	client.send(msgspec);

	CommandSetBroadcastText broadcast;
	broadcast.data.time = server.getServerTime();
	string text("SPECTATOR JOINED!");
	strncpy(broadcast.data.text, text.c_str() , text.length());
	broadcast.data.duration = 1000;
	server.sendAll(broadcast);

	CommandSetBroadcastText msg;
	msg.data.time = server.getServerTime();

	int currentRound = 1;
	auto game = dynamic_cast<NetworkMultiplayer *>(&server.getGame());
	if (game) {
		currentRound = game->get_round();
	}
	text.assign(format("PLEASE WAIT FOR GAME END (CURRENT ROUND %d/5)", currentRound));
	strncpy(msg.data.text, text.c_str() , text.length());
	msg.data.duration = 10000;
	client.send(msg);

	client.setState(Client::State::SPECTATING);

}

void ServerStateAcceptSpectatingClients::server_add_player(Server &server, Client &client, Player * const newplayer) const
{
	newplayer->spectate();
	newplayer->is_dead = true;
	newplayer->hitpoints = 0;

	// Send client all other player positions
	auto &players = *(server.getGame().players);
	for (auto i= players.begin(); i!=players.end(); i++)
	{
		auto &player = **i;

		if (player.number != client.getClientId())
		{
			// Send the client "add player" for others
			CommandAddPlayer otherplayer;
			otherplayer.data.time = server.getServerTime();
			otherplayer.data.client_id = player.number;
			otherplayer.data.character = player.character;
			otherplayer.data.x = player.position->x;
			otherplayer.data.y = player.position->y;
			otherplayer.data.current_sprite = player.current_sprite;
			otherplayer.data.is_spectating = player.spectating();
			client.send(otherplayer);

			// And send others "add player" for this new client
			otherplayer.data.client_id = newplayer->number;
			otherplayer.data.character = newplayer->character;
			otherplayer.data.x = newplayer->position->x;
			otherplayer.data.y = newplayer->position->y;
			otherplayer.data.current_sprite = newplayer->current_sprite;
			otherplayer.data.is_spectating = newplayer->spectating();
			server.getClientById(player.number)->send(otherplayer);
		}
	}
}

}