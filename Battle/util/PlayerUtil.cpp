#include "SDL/SDL.h"
#include "Player.h"

#include "commands/CommandSetPlayerData.hpp"
#include "ServerClient.h" // todo refactor this
namespace player_util
{
	void set_position_data(CommandSetPlayerData &playerpos, char client_id, Uint32 time, Player &player)
	{

		// todo: remove param client_id, it is always player.number (!)
		short flags = 0;
		if (player.input->is_pressed(A_LEFT))		flags |= ServerClient::FLAG_LEFT;
		if (player.input->is_pressed(A_RIGHT))		flags |= ServerClient::FLAG_RIGHT;
		if (player.input->is_pressed(A_UP))			flags |= ServerClient::FLAG_UP;
		if (player.input->is_pressed(A_DOWN))		flags |= ServerClient::FLAG_DOWN;
		if (player.input->is_pressed(A_JUMP))		flags |= ServerClient::FLAG_JUMP;
		if (player.input->is_pressed(A_RUN))		flags |= ServerClient::FLAG_RUN;
		if (player.input->is_pressed(A_SHOOT))		flags |= ServerClient::FLAG_SHOOT;
		if (player.input->is_pressed(A_BOMB))		flags |= ServerClient::FLAG_BOMB;
		if (player.input->is_pressed(A_START))		flags |= ServerClient::FLAG_START;

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
		//int current_sprite;
		//playerpos.data.cycle_direction = player.cycle_direction;
		//playerpos.data.distance_walked = player.distance_walked;
	}

	void set_player_data(Player &player, CommandSetPlayerData &command)
	{
		//data for this client
		memset(player.input->pressed, 0x00, sizeof(player.input->pressed));
		short flags = command.data.flags;
		if (flags & ServerClient::FLAG_LEFT)	player.input->pressed[A_LEFT] = true;
		if (flags & ServerClient::FLAG_RIGHT) 	player.input->pressed[A_RIGHT] = true;
		if (flags & ServerClient::FLAG_UP)		player.input->pressed[A_UP] = true;
		if (flags & ServerClient::FLAG_DOWN)	player.input->pressed[A_DOWN] = true;
		if (flags & ServerClient::FLAG_JUMP)	player.input->pressed[A_JUMP] = true;
		if (flags & ServerClient::FLAG_RUN)		player.input->pressed[A_RUN] = true;
		if (flags & ServerClient::FLAG_SHOOT)	player.input->pressed[A_SHOOT] = true;
		if (flags & ServerClient::FLAG_BOMB)	player.input->pressed[A_BOMB] = true;
		if (flags & ServerClient::FLAG_START)	player.input->pressed[A_START] = true;

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
		//player.cycle_direction = command.data.cycle_direction;
		//player.distance_walked = command.data.distance_walked;
	}
}