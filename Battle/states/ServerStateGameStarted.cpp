#include "ServerStates.h"
#include "network/Server.h"
#include <vector>

#include "util/Log.h"

#include "network/Commands.hpp"
#include "Player.h"
#include "NetworkMultiplayer.h"
#include "network/Server.h"

using std::string;

namespace network {

ServerStateGameStarted::ServerStateGameStarted()
{
}

void ServerStateGameStarted::initialize(Server &server) const
{
	once = true;

	auto &players = *(server.getGame().players);
	for (auto i = players.begin(); i!=players.end(); i++)
	{
		auto &player = **i;
		auto client = server.getClientById(player.number);

		server.ignoreClientInputFor(3000);

		player.hitpoints = 100;
		player.score = 0;
		player_util::unset_input(player);
		level_util::set_player_start(player, server.getLevel());

		// Use game end to lock all activity at clients
		CommandSetGameEnd ge;
		ge.data.time = server.getServerTime();
		ge.data.is_draw = false;
		ge.data.winner_id = -1;
		client->send(ge);

		// Reset level
		Level &level (server.getLevel());
		level.reset();
		CommandSetLevel cmd;
		cmd.data.your_id = player.number; // This is the first time the client learns about it's id on the server
		strncpy(cmd.data.levelname, server.getLevelName().c_str(), server.getLevelName().size());
		memcpy(&cmd.data.level, &level.level, sizeof(level.level));
		memcpy(&cmd.data.level_hp, &level.level_hp, sizeof(level.level_hp));
		client->send(cmd);

		// Reset there health
		CommandSetHitPoints hp;
		hp.data.time = server.getServerTime();
		hp.data.client_id = player.number;
		hp.data.hitpoints = player.hitpoints;
		client->send(hp);

		// Set their correct positions
		CommandSetPlayerData pd;
		player_util::set_position_data(pd, player.number, server.getServerTime(), server.getUdpSeq(), player);
		server.sendAll(pd);

		CommandSetPlayerAmmo ammo;
		ammo.data.time = server.getServerTime();
		ammo.data.client_id = player.number;
		ammo.data.bombs = player.bombs;
		server.sendAll(ammo);

		CommandSetPlayerScore score;
		score.data.time = server.getServerTime();
		score.data.client_id = player.number;
		score.data.score = player.score;
		server.sendAll(score);

		// Make them countdown
		CommandSetGameStart gs;
		gs.data.time = server.getServerTime();
		gs.data.delay = 1000;
		server.sendAll(gs);
	}
}

// Todo : maybe more protocol related stuff should be in here,
//   and move the rest to pre and post processing in NetworkMultiplayer
void ServerStateGameStarted::execute(Server &server, Client &client) const
{
	Uint32 servertime = server.getServerTime();

	size_t numActiveClients = server.numActiveClients();
	if (numActiveClients == 0)
		return;
	
	// Lag test every 1s
	Uint32 lagdiff = servertime - client.getLastLagTime();
	if (lagdiff >= 1000) {
		CommandPing ping;
		ping.data.time = servertime;
		client.send(ping);
		client.setLastLagTime(servertime);
	}


	// If there were/are pending clients not yet fully initialized, they are too late,
	//  the game is already started. 
	// Though we continue processing them as in "AcceptClients", only we make sure
	//  we accept them as spectators only.
	if (client.getState() < Client::State::ACTIVE) {
		ServerStateAcceptSpectatingClients temp;
		temp.initialize(server);
		temp.execute(server, client);
		return;
	}

	if (once) {
		once = false;
		CommandSetBroadcastText broadcast;
		broadcast.data.time = server.getServerTime();
		string text("WELCOME YOU ARE NOW IN THE GAME");
		strncpy(broadcast.data.text, text.c_str() , text.length());
		broadcast.data.duration = 2000;
		server.sendAll(broadcast);
	}
}

ServerState * ServerStateGameStarted::check_self(Server &server) const
{
	ServerState *newstate = NULL;
	if (server.numActiveClients() == 0) {
		newstate = new ServerStateAcceptClients();
	}
	else if (server.numUndeadActiveClients() == 1) {
		newstate = new ServerStateAcceptClients();

		CommandSetBroadcastText broadcast;
		broadcast.data.time = server.getServerTime();
		string text("YOU WIN");
		strncpy(broadcast.data.text, text.c_str() , text.length());
		broadcast.data.duration = 2000;
		server.sendAll(broadcast);
	}

	return newstate;
}

}
