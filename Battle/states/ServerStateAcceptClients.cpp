#include "ServerStates.h"
#include "network/Server.h"
#include <vector>

#include "util/Log.h"

#include "network/Commands.hpp"
#include "Player.h"
#include "NetworkMultiplayer.h"

using std::string;

namespace network {

ServerStateAcceptClients::ServerStateAcceptClients()
	: gameStart_(0), gameStartSendTime_(0), gameStartForPlayers_(0)
{
}

void ServerStateAcceptClients::initialize(Server &server) const
{
	gameStart_ = 0;
	gameStartSendTime_ = 0;
	gameStartForPlayers_ = 0;

	transform_spectators_into_valid_players(server);
}

void ServerStateAcceptClients::transform_spectators_into_valid_players(Server &server) const
{
	Uint32 servertime = server.getServerTime();
	auto &players = *(server.getGame().players);
	for (auto i= players.begin(); i!=players.end(); i++) {
		auto &player = **i;

		player.spectate(false);
		player.is_dead = false;
		server.getClientById(player.number)->setState(Client::State::ACTIVE);

		CommandSetPlayerDeath dead;
		dead.data.time = servertime;
		dead.data.is_dead = player.is_dead;
		dead.data.client_id = player.number;
		server.sendAll(dead);

		CommandSetSpectating spec;
		spec.data.time = servertime;
		spec.data.is_spectating = player.spectating();
		server.sendAll(spec);

		CommandSetPlayerData playerpos;
		player_util::set_position_data(playerpos, player.number, server.getServerTime(), server.getUdpSeq(), player);
		server.sendAll(playerpos);
	}
}

// Todo : maybe more protocol related stuff should be in here,
//   and move the rest to pre and post processing in NetworkMultiplayer
void ServerStateAcceptClients::execute(Server &server, Client &client) const
{
	Uint32 servertime = server.getServerTime();

	if (!client.getLastLagTime()) {
		client.setLastLagTime(servertime);
		return;
	}

	switch(client.getState())
	{
		// Request character from client
		case Client::State::CONNECTING:
			execute_connecting(servertime, server, client);
			break;
		case Client::State::COMMTOKEN_REQUESTED:
			execute_commtoken_requested(servertime, server, client);
			break;
		case Client::State::CALCULATING_LAG:
			execute_calculating_lag(servertime, server, client);
			break;
		case Client::State::INITIALIZING:
			execute_initializing(servertime, server, client);
			break;
		case Client::State::CHARACTER_REQUESTED:
			execute_character_requested(servertime, server, client);
			break;
		case Client::State::CHARACTER_INITIALIZED:
			execute_character_initialized(servertime, server, client);
			break;
		case Client::State::SERVERSIDE_READY:
			execute_serverside_ready(servertime, server, client);
			break;
		case Client::State::READY_FOR_POSITIONAL_DATA:
			execute_ready_for_positional_data(servertime, server, client);
			break;
		case Client::State::ACTIVE:
		case Client::State::SPECTATING:
			execute_active(servertime, server, client);
			break;
		
	}

	size_t numActiveClients = server.numActiveClients();
	if (numActiveClients >= 2) {
		if (gameStart_ == 0 || gameStartForPlayers_ != numActiveClients) {
			execute_gamestart(servertime, server, client, numActiveClients);
		}
		else {
			execute_gameupdate(servertime, server, client, numActiveClients);
		}
	}
	else
	{
		gameStart_ = 0;
		gameStartSendTime_ = 0;
	}
}

void ServerStateAcceptClients::execute_connecting(Uint32 servertime, Server &server, Client &client) const
{
	static CommandSetCommunicationToken commtok;
	commtok.data_.commToken = client.getCommToken();
	client.send(commtok);
	
	client.setState(Client::State::COMMTOKEN_REQUESTED);
}
void ServerStateAcceptClients::execute_commtoken_requested(Uint32 servertime, Server &server, Client &client) const
{
	// If the client replies with a CommandCommunicationTokenAck
	// We continue to state INITIALIZING!
}
void ServerStateAcceptClients::execute_calculating_lag(Uint32 servertime, Server &server, Client &client) const
{
	if (client.getInitialLagTests()) {
		// Make sure player has all initial lag tests
		Uint32 lagdiff = servertime - client.getLastLagTime();
		if (lagdiff >= 200) {
			CommandPing ping;
			ping.data.time = servertime;
			client.send(ping);
			client.setLastLagTime(client.getLastLagTime() + 200);
			client.setInitialLagTests(client.getInitialLagTests() - 1);
		}
	} else {
		client.setState(Client::State::INITIALIZING);
	}
}
void ServerStateAcceptClients::execute_initializing(Uint32 servertime, Server &server, Client &client) const
{
	static CommandRequestCharacter req;
	req.data.time = servertime;
	client.send(req);

	Level &level (server.getLevel());

	// First send level with the id so he knows which one he is
	CommandSetLevel cmd;
	cmd.data.your_id = client.getClientId(); // This is the first time the client learns about it's id on the server
	strncpy(cmd.data.levelname, server.getLevelName().c_str(), server.getLevelName().size());
	memcpy(&cmd.data.level, &level.level, sizeof(level.level));
	memcpy(&cmd.data.level_hp, &level.level_hp, sizeof(level.level_hp));
	client.send(cmd);


	// Create player object for client, determine starting position from level
	Player *newplayer = new Player (client.getCharacter(), client.getClientId(), server.getMain());
	GameInputStub *playerinput = new GameInputStub(server.getMain());
	newplayer->input = playerinput;
	level_util::set_player_start(*newplayer, level);
	newplayer->reset();

	server.getGame().add_player(newplayer);

	client.setState(Client::State::CHARACTER_REQUESTED);
}

void ServerStateAcceptClients::execute_character_requested(Uint32 servertime, Server &server, Client &client) const
{
}

void ServerStateAcceptClients::execute_character_initialized(Uint32 servertime, Server &server, Client &client) const
{
	auto &newplayer = player_util::get_player_by_id(server.getMain(), client.getClientId());

	// Send new player it's position et.al.
	CommandSetPlayerData playerpos;
	player_util::set_position_data(playerpos, newplayer.number, server.getServerTime(), server.getUdpSeq(), newplayer);
	client.send(playerpos);

	// Notify everyone
	server_add_player(server, client, &newplayer);
	
	CommandSetServerReady cmdrdy;
	client.send(cmdrdy);

	client.setState(Client::State::SERVERSIDE_READY);
}
void ServerStateAcceptClients::execute_serverside_ready(Uint32 servertime, Server &server, Client &client) const
{
	// Will go to client-side ready after receiving CommandSetClientReady
}
void ServerStateAcceptClients::execute_ready_for_positional_data(Uint32 servertime, Server &server, Client &client) const
{
	// Send client all other player positions
	auto &players = *(server.getGame().players);
	for (auto i= players.begin(); i!=players.end(); i++)
	{
		auto &player = **i;

		if (player.number != client.getClientId())
		{
			// Send the client "player data"
			CommandSetPlayerData playerpos;
			player_util::set_position_data(playerpos, player.number, server.getServerTime(), server.getUdpSeq(), player);
			client.send(playerpos);
		}
	}
	
	if (server.numActiveClients() < 4) {
		client.setState(Client::State::ACTIVE);
	} else {
		client.setState(Client::State::DENIED);
		CommandServerFull full;
		full.data.time = servertime;
		client.send(full);
	}
}
void ServerStateAcceptClients::execute_active(Uint32 servertime, Server &server, Client &client) const
{
	// For now the client is initialized (it knows the other players)
	Player &player(player_util::get_player_by_id(server.getMain(), client.getClientId()));
	if (player.is_dead) {

		// Do something fancy here
		player.hitpoints = 100;
		CommandSetHitPoints hp;
		hp.data.time = server.getServerTime();
		hp.data.client_id = client.getClientId();
		hp.data.hitpoints = player.hitpoints;
		server.sendAll(hp);

		Level &level (server.getLevel());
		level_util::set_player_start(player, level);

		CommandSetPlayerData pd;
		player_util::set_position_data(pd, client.getClientId(), server.getServerTime(), server.getUdpSeq(), player);
		server.sendAll(pd);

		player.is_dead = player.is_spectating;
		player.bombs = 3;

		// Send the client player's ammo
		CommandSetPlayerAmmo ammo;
		ammo.data.time = server.getServerTime();
		ammo.data.client_id = player.number;
		ammo.data.bombs = 3;
		server.sendAll(ammo);
	}
	/////////////////

	// Lag test every 1s from now on
	Uint32 lagdiff = servertime - client.getLastLagTime();
	if (lagdiff >= 1000) {
		CommandPing ping;
		ping.data.time = servertime;
		client.send(ping);
		client.setLastLagTime(client.getLastLagTime() + 1000);
	}
}

void ServerStateAcceptClients::execute_gamestart(Uint32 servertime, Server &server, Client &client, size_t numActiveClients) const
{
	gameStartForPlayers_ = numActiveClients;
	gameStart_ = servertime + 15 * 1000;
	//gameStart_ = servertime + 1 * 1000; // temp set to 1
	gameStartSendTime_ = servertime;

	CommandSetBroadcastText broadcast;
	broadcast.data.time = server.getServerTime();
	string text("GAME STARTS IN 15 SECONDS");
	strncpy(broadcast.data.text, text.c_str() , text.length());
	broadcast.data.duration = 2000;
	server.sendAll(broadcast);
}

void ServerStateAcceptClients::execute_gameupdate(Uint32 servertime, Server &server, Client &client, size_t numActiveClients) const
{
	//////////////////////////
	// Update client every second
	if (gameStart_ != 0) {
		if (servertime > gameStart_) {
			CommandSetBroadcastText broadcast;
			broadcast.data.time = server.getServerTime();
			string text(format("STARTING GAME FOR %d PLAYERS", numActiveClients));
			strncpy(broadcast.data.text, text.c_str() , text.length());
			broadcast.data.duration = 2000;
			server.sendAll(broadcast);
			server.setState(new ServerStateGameStarted());
		}
		else if ((servertime - gameStartSendTime_) > 1000) {
			gameStartSendTime_ = servertime;

			int secsToGo = ((gameStart_  - servertime) / 1000) + 1;

			CommandSetBroadcastText broadcast;
			broadcast.data.time = server.getServerTime();
			string text(format("GAME STARTS IN %d SECONDS", secsToGo));
			strncpy(broadcast.data.text, text.c_str() , text.length());
			broadcast.data.duration = 2000;
			server.sendAll(broadcast);
		}
	}
}

void ServerStateAcceptClients::server_add_player(Server &server, Client &client, Player * const newplayer) const
{
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

			// Welcome the player
			CommandSetBroadcastText broadcast;
			broadcast.data.time = server.getServerTime();
			string welcome("WE WELCOME A NEW PLAYER");
			strncpy(broadcast.data.text, welcome.c_str() , welcome.length());
			broadcast.data.duration = 2000;
			server.getClientById(player.number)->send(broadcast);
		}

		// Send the client player 's health
		CommandSetHitPoints points;
		points.data.time = server.getServerTime();
		points.data.client_id = player.number;
		points.data.hitpoints = player.hitpoints;
		client.send(points);

		// Send the client player's ammo
		CommandSetPlayerAmmo ammo;
		ammo.data.time = server.getServerTime();
		ammo.data.client_id = player.number;
		ammo.data.bombs = player.bombs;
		client.send(ammo);

		// Welcome the player
		CommandSetBroadcastText broadcast;
		broadcast.data.time = server.getServerTime();
		string welcome("WELCOME TO SERVER");
		strncpy(broadcast.data.text, welcome.c_str() , welcome.length());
		broadcast.data.duration = 2000;
		client.send(broadcast);
	}
}

}