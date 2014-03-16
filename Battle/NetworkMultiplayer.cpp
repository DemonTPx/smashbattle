#include "SDL/SDL.h"

#include "NetworkMultiplayer.h"
#include "Player.h"
#include "network/Server.h"
#include "network/Commands.hpp"

#include <map>
#include <string>
using std::string;
using std::map;

#include "states/ServerStateAcceptClients.h"

#include "HealthPowerUp.h"
#include "AmmoPowerUp.h"
#include "DoubleDamagePowerUp.h"
#include "InstantKillBulletPowerUp.h"
#include "BombPowerUp.h"
#include "MinePowerUp.h"
#include "AirstrikePowerUp.h"
#include "LaserBeamPowerUp.h"
#include "ShieldPowerUp.h"
#include "RandomPowerUp.h"
#include "Main.h"

NetworkMultiplayer::NetworkMultiplayer (Main &main) 
: LocalMultiplayer(main)
{
	main.setGameplay(this);
}

void NetworkMultiplayer::on_game_reset()
{
	countdown = false;

	if (main_.getServer().gameStarted())
		LocalMultiplayer::on_game_reset();
	else
		round++;


	// Synchronize level and positions across players
	auto &server = main_.getServer();
	auto &vec = *players;
	for (auto i = vec.begin(); i != vec.end(); i++) {
		auto &player(**i);

		player_util::unset_input(player);

		// First send level with the id so he knows which one he is
		network::CommandSetLevel lvl;
		lvl.data.your_id = player.number;
		strncpy(lvl.data.levelname, server.getLevelName().c_str(), server.getLevelName().size());
		memcpy(&lvl.data.level, &level->level, sizeof(level->level));
		memcpy(&lvl.data.level_hp, &level->level_hp, sizeof(level->level_hp));
		server.getClientById(player.number)->send(lvl);

		network::CommandSetPlayerData pd;
		level_util::set_player_start(player, *level);
		player_util::set_position_data(pd, player.number, server.getServerTime(), server.getUdpSeq(), player);
		server.sendAll(pd);

		network::CommandSetHitPoints points;
		points.data.time = server.getServerTime();
		points.data.client_id = player.number;
		points.data.hitpoints = player.hitpoints;
		server.sendAll(points);

		network::CommandSetPlayerAmmo ammo;
		ammo.data.time = server.getServerTime();
		ammo.data.client_id = player.number;
		ammo.data.bombs = player.bombs;
		server.sendAll(ammo);

		network::CommandSetPlayerDeath alive;
		alive.data.time = server.getServerTime();
		alive.data.client_id = player.number;
		alive.data.is_dead = player.spectating();
		server.sendAll(alive);
	}

	if (!game_running) {
		// dirty restart
		game_running = true;
		initialize();
		round = 1;
		server.setState(new network::ServerStateAcceptClients());
	}
}

void NetworkMultiplayer::on_post_processing()
{
	if (!main_.getServer().gameStarted()) {
		auto &vec = *players;
		for (auto i = vec.begin(); i != vec.end(); i++) {
			auto &player(**i);


			if (player.hitpoints <= 0)
				player.is_dead = true;
		}
	}
	else {
		auto &server = main_.getServer();

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

				network::CommandSetPlayerDeath died;
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

				if (winner != NULL) {
					// Send game end
					network::CommandSetGameEnd end;
					end.data.time = server.getServerTime();
					end.data.winner_id = winner->number;
					end.data.is_draw = draw;
					server.sendAll(end);

					if (game_running) {
						// Send game resume instruction
						network::CommandSetGameStart start;
						start.data.time = server.getServerTime();
						start.data.delay = we_have_a_winner() ? 0 : 3000;
						server.sendAll(start);
					}
				}

				// Send player score, new player positions
				for (auto i = vec.begin(); i != vec.end(); i++) {
					auto &player(**i);
					network::CommandSetPlayerScore score;
					score.data.time = server.getServerTime();
					score.data.client_id = player.number;
					score.data.score = player.score;
					server.sendAll(score);

					// Unset input's for players, do not change location yet
					player_util::unset_input(player);
					network::CommandSetPlayerData pd;
					player_util::set_position_data(pd, player.number, server.getServerTime(), server.getUdpSeq(), player);
					server.sendAll(pd);
				}
			}
		} else {
			once = true;
		}
	}
}

GameplayObject *NetworkMultiplayer::generate_powerup(bool force)
{
	GameplayObject *powerup = LocalMultiplayer::generate_powerup(force);
	if (powerup)
	{
		network::CommandGeneratePowerup genpow;
		genpow.data.time = main_.getServer().getServerTime();
		genpow.data.powerupid = powerup->id();
		powerup->copyTo(genpow);

		main_.getServer().sendAll(genpow);
	}
	return powerup;
}
