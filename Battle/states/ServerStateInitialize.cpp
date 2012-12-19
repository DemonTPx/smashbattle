#include "ServerStates.h"
#include "Server.h"
#include <vector>

#include "log.h"

#include "Commands.hpp"
#include "Player.h"
#include "NetworkMultiplayer.h"

using std::string;

ServerStateInitialize::ServerStateInitialize(string level, int port)
	: level_(level), port_(port)
{
	log("ServerStateInitialize constructor", Logger::Priority::DEBUG);
	log(string("level = " + level), Logger::Priority::DEBUG);
	log(format("port = %d", port), Logger::Priority::DEBUG);
}

int ServerStateInitialize::test() const
{

	return 1000;
}

void ServerStateInitialize::initialize(Server &server) const
{
	server.setLevel(level_);
	server.setPort(port_);
}

// Todo : maybe more protocol related stuff should be in here,
//   and move the rest to pre and post processing in NetworkMultiplayer
void ServerStateInitialize::execute(Server &server, Client &client) const
{
	Uint32 servertime = server.getServerTime();

	if (!client.getLastLagTime()) {
		client.setLastLagTime(servertime);
		return;
	}

	//// temp hack, does not belong here at all, just want to make the network proof of concept a little more interesting :p
	static Uint32 lastlevelReset = server.getServerTime();
	// Reset level state every 30 secs
	if ((server.getServerTime() - lastlevelReset) > 30000)
	{
		server.getLevel().reset();

		CommandSetLevel lvl;
		strncpy(lvl.data.levelname, server.getLevelName().c_str(), server.getLevelName().size());
		memcpy(&lvl.data.level, &server.getLevel().level, sizeof(server.getLevel().level));
		memcpy(&lvl.data.level_hp, &server.getLevel().level_hp, sizeof(server.getLevel().level_hp));

		auto &players = *(server.getGame().players);
		for (auto i= players.begin(); i!=players.end(); i++)
		{
			auto &player = **i;

			lvl.data.your_id = player.number;
			server.getClientById(player.number).send(lvl);

			CommandSetPlayerData pd;
			level_util::set_player_start(player, server.getLevel());
			player_util::set_position_data(pd, player.number, servertime, player);
			server.sendAll(pd);
		}

		lastlevelReset = server.getServerTime();
	}
	///

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
	}
	else 
	{
		// Lag test every 1s from now on
		Uint32 lagdiff = servertime - client.getLastLagTime();
		if (lagdiff >= 1000) {
			CommandPing ping;
			ping.data.time = servertime;
			client.send(ping);
			client.setLastLagTime(client.getLastLagTime() + 1000);
		}

		switch(client.getState())
		{
			// Request character from client
			case Client::State::CONNECTING:
				{
					CommandRequestCharacter req;
					client.send(req);

					client.setState(Client::State::CHARACTER_REQUESTED);
				}
				break;
			case Client::State::CHARACTER_REQUESTED:
				break;

			// Send level to client
			case Client::State::CHARACTER_INITIALIZED:
				{
					Level &level (server.getLevel());

					// First send level with the id so he knows which one he is
					CommandSetLevel cmd;
					cmd.data.your_id = client.getClientId();
					strncpy(cmd.data.levelname, server.getLevelName().c_str(), server.getLevelName().size());
					memcpy(&cmd.data.level, &level.level, sizeof(level.level));
					memcpy(&cmd.data.level_hp, &level.level_hp, sizeof(level.level_hp));
					client.send(cmd);


					// Create player object for client, determine starting position from level
					Player *newplayer = new Player (client.getCharacter(), client.getClientId());
					GameInputStub *playerinput = new GameInputStub();
					newplayer->input = playerinput;
					level_util::set_player_start(*newplayer, level);
					newplayer->reset();

					server.getGame().add_player(newplayer);

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
							client.send(otherplayer);

							// And send others "add player" for this new client
							otherplayer.data.client_id = newplayer->number;
							otherplayer.data.character = newplayer->character;
							otherplayer.data.x = newplayer->position->x;
							otherplayer.data.y = newplayer->position->y;
							otherplayer.data.current_sprite = newplayer->current_sprite;
							server.getClientById(player.number).send(otherplayer);
						}

						// Send the client "player data"
						CommandSetPlayerData playerpos;
						player_util::set_position_data(playerpos, player.number, server.getServerTime(), player);
						client.send(playerpos);

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
						ammo.data.bombs = 3;
						client.send(ammo);
					}

					client.setState(Client::State::ACTIVE);
				}
				break;

			// For now the client is initialized (it knows the other players)
			case Client::State::ACTIVE:
				Player &player(player_util::get_player_by_id(client.getClientId()));
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
					player_util::set_position_data(pd, client.getClientId(), server.getServerTime(), player);
					server.sendAll(pd);

					player.is_dead = false;
					player.bombs = 3;

					// Send the client player's ammo
					CommandSetPlayerAmmo ammo;
					ammo.data.time = server.getServerTime();
					ammo.data.client_id = player.number;
					ammo.data.bombs = 3;
					server.sendAll(ammo);

				}
				break;
		}
	}
}