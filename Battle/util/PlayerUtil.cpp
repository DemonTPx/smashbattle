#include "SDL/SDL.h"
#include "Player.h"

#include <stdexcept>

#include "commands/CommandSetPlayerData.hpp"
#include "network/ServerClient.h" // todo refactor this
#include "network/Server.h" // todo refactor this
#include "Gameplay.h"

namespace player_util
{
	void set_position_data(network::CommandSetPlayerData &playerpos, char client_id, Uint32 time, short udpseq, Player &player)
	{
		// todo: remove param client_id, it is always player.number (!)
		short flags = 0;
		if (player.input->is_pressed(A_LEFT))		flags |= network::ServerClient::FLAG_LEFT;
		if (player.input->is_pressed(A_RIGHT))		flags |= network::ServerClient::FLAG_RIGHT;
		if (player.input->is_pressed(A_UP))			flags |= network::ServerClient::FLAG_UP;
		if (player.input->is_pressed(A_DOWN))		flags |= network::ServerClient::FLAG_DOWN;
		if (player.input->is_pressed(A_JUMP))		flags |= network::ServerClient::FLAG_JUMP;
		if (player.input->is_pressed(A_RUN))		flags |= network::ServerClient::FLAG_RUN;
		if (player.input->is_pressed(A_SHOOT))		flags |= network::ServerClient::FLAG_SHOOT;
		if (player.input->is_pressed(A_BOMB))		flags |= network::ServerClient::FLAG_BOMB;
		if (player.input->is_pressed(A_START))		flags |= network::ServerClient::FLAG_START;

		playerpos.data.udp_sequence = udpseq;
		playerpos.data.client_id = player.number;
		playerpos.data.time = time;
		playerpos.data.flags = flags;
		playerpos.data.x = player.position->x;
		playerpos.data.y = player.position->y;
		playerpos.data.momentumx = player.momentumx;
		playerpos.data.momentumy = player.momentumy;
		playerpos.data.newmomentumx = player.newmomentumx;
		playerpos.data.is_running = player.is_running;
		playerpos.data.is_duck = player.is_duck;
		playerpos.data.is_duck_forced = player.is_duck_forced;
		playerpos.data.duck_force_start = player.duck_force_start;
		playerpos.data.is_jumping = player.is_jumping;
		playerpos.data.is_falling = player.is_falling;
		playerpos.data.jump_start = player.jump_start;
		playerpos.data.current_sprite = player.current_sprite;
		playerpos.data.is_hit = player.is_hit;
		//playerpos.data.cycle_direction = player.cycle_direction;
		//playerpos.data.distance_walked = player.distance_walked;
	}

	void set_player_data(Player &player, network::CommandSetPlayerData &command, bool skip_input)
	{
		//data for this client
		if (!skip_input) {
			memset(player.input->pressed, 0x00, sizeof(player.input->pressed));
			short flags = command.data.flags;
			if (flags & network::ServerClient::FLAG_LEFT)	player.input->pressed[A_LEFT] = true;
			if (flags & network::ServerClient::FLAG_RIGHT) 	player.input->pressed[A_RIGHT] = true;
			if (flags & network::ServerClient::FLAG_UP)		player.input->pressed[A_UP] = true;
			if (flags & network::ServerClient::FLAG_DOWN)	player.input->pressed[A_DOWN] = true;
			if (flags & network::ServerClient::FLAG_JUMP)	player.input->pressed[A_JUMP] = true;
			if (flags & network::ServerClient::FLAG_RUN)		player.input->pressed[A_RUN] = true;
			if (flags & network::ServerClient::FLAG_SHOOT)	player.input->pressed[A_SHOOT] = true;
			if (flags & network::ServerClient::FLAG_BOMB)	player.input->pressed[A_BOMB] = true;
			if (flags & network::ServerClient::FLAG_START)	player.input->pressed[A_START] = true;
		}

		player.position->x =command.data.x;
		player.position->y =command.data.y;

		player.momentumx = command.data.momentumx;
		player.momentumy = command.data.momentumy;
		player.newmomentumx = command.data.newmomentumx;
		player.is_running = command.data.is_running;
		player.is_duck = command.data.is_duck;
		player.is_duck_forced = command.data.is_duck_forced;
		player.duck_force_start = command.data.duck_force_start;
		player.is_jumping = command.data.is_jumping;
		player.is_falling = command.data.is_falling;
		player.current_sprite = command.data.current_sprite;
		player.is_hit = command.data.is_hit;
		//player.cycle_direction = command.data.cycle_direction;
		//player.distance_walked = command.data.distance_walked;
	}

	Player &get_player_by_id(char client_id)
	{
		Gameplay * game = NULL;
		if (Main::runmode == MainRunModes::CLIENT)
			game = &network::ServerClient::getInstance().getGame();
		else if (Main::runmode == MainRunModes::SERVER)
			game = &network::Server::getInstance().getGame();
		else
			throw std::runtime_error("unsupported game type");

		auto &players = *(*game).players;
		for (auto i = players.begin(); i != players.end(); i++) {
			auto &player = **i;
			if (player.number == client_id)
				return player;
		}
		throw std::runtime_error("player not found by id");
	}

	void unset_input(Player &player)
	{
		player.input->pressed[A_LEFT] = false;
		player.input->pressed[A_RIGHT] = false;
		player.input->pressed[A_UP] = false;
		player.input->pressed[A_DOWN] = false;
		player.input->pressed[A_JUMP] = false;
		player.input->pressed[A_RUN] = false;
		player.input->pressed[A_SHOOT] = false;
		player.input->pressed[A_BOMB] = false;
		player.input->pressed[A_START] = false;
	}
}
