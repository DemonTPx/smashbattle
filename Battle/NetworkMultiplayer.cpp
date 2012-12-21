#include "SDL/SDL.h"

#include "NetworkMultiplayer.h"
#include "Player.h"
#include "Server.h"
#include "Commands.hpp"

#include <map>
#include <string>
using std::string;
using std::map;

#include "states/ServerStateAcceptClients.h"

void NetworkMultiplayer::on_game_reset()
{
	countdown = false;

	if (Server::gameStarted())
		LocalMultiplayer::on_game_reset();
	else
		round++;


	// Synchronize level and positions across players
	auto &server = Server::getInstance();
	auto &vec = *players;
	for (auto i = vec.begin(); i != vec.end(); i++) {
		auto &player(**i);

		player_util::unset_input(player);

		// First send level with the id so he knows which one he is
		CommandSetLevel lvl;
		lvl.data.your_id = player.number;
		strncpy(lvl.data.levelname, server.getLevelName().c_str(), server.getLevelName().size());
		memcpy(&lvl.data.level, &level->level, sizeof(level->level));
		memcpy(&lvl.data.level_hp, &level->level_hp, sizeof(level->level_hp));
		server.getClientById(player.number).send(lvl);

		CommandSetPlayerData pd;
		player_util::set_position_data(pd, player.number, server.getServerTime(), player);
		server.sendAll(pd);

		CommandSetHitPoints points;
		points.data.time = server.getServerTime();
		points.data.client_id = player.number;
		points.data.hitpoints = player.hitpoints;
		server.sendAll(points);

		CommandSetPlayerAmmo ammo;
		ammo.data.time = server.getServerTime();
		ammo.data.client_id = player.number;
		ammo.data.bombs = player.bombs;
		server.sendAll(ammo);

		CommandSetPlayerDeath alive;
		alive.data.time = server.getServerTime();
		alive.data.client_id = player.number;
		alive.data.is_dead = false;
		server.sendAll(alive);
	}

	if (!game_running) {
		// dirty restart
		game_running = true;
		initialize();
		round = 1;
		server.setState(new ServerStateAcceptClients());
	}
}

void NetworkMultiplayer::on_post_processing()
{
	if (!Server::gameStarted()) {
		auto &vec = *players;
		for (auto i = vec.begin(); i != vec.end(); i++) {
			auto &player(**i);


			if (player.hitpoints <= 0)
				player.is_dead = true;
		}
	}
	else {
		auto &server = Server::getInstance();

		map<char, bool> playerWasDead;
		auto &vec = *players;
		for (auto i = vec.begin(); i != vec.end(); i++) {
			auto &player(**i);
			playerWasDead[player.number] = player.is_dead;
		}

		// do normal post processing
		LocalMultiplayer::on_post_processing();

		// now see if anyone died in this processing cycle
		for (auto i = vec.begin(); i != vec.end(); i++) {
			auto &player(**i);
			if (!playerWasDead[player.number] && player.is_dead) { 

				CommandSetPlayerDeath died;
				died.data.time = server.getServerTime();
				died.data.client_id = player.number;
				died.data.is_dead = player.is_dead;
				server.sendAll(died);
			}
		}

		static bool once = true;
		if (ended) {
			if (once) {
				once = false;

				// Send player score
				for (auto i = vec.begin(); i != vec.end(); i++) {
					auto &player(**i);
					CommandSetPlayerScore score;
					score.data.time = server.getServerTime();
					score.data.client_id = player.number;
					score.data.score = player.score;
					server.sendAll(score);
				}

				// Send game end
				CommandSetGameEnd end;
				end.data.time = server.getServerTime();
				end.data.winner_id = winner->number;
				end.data.is_draw = draw;
				server.sendAll(end);

				// Send game resume instruction
				CommandSetGameStart start;
				start.data.time = server.getServerTime();
				start.data.delay = 5000;
				server.sendAll(start);
			}
		} else {
			once = true;
		}
	}
}