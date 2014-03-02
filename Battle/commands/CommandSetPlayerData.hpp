#pragma once

#include "network/Command.hpp"

namespace network {

class CommandSetPlayerData : public Command
{
public:

	CommandSetPlayerData() : Command(Command::Types::SetPlayerData) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetPlayerData() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		short udp_sequence;
		char client_id;
		Uint32 time;
		short flags;

		Sint16 x;
		Sint16 y;


		int momentumx;
		int momentumy;

		int newmomentumx;

		bool is_running;
		bool is_duck;
		bool is_duck_forced;

		int duck_force_start;

		bool is_jumping;
		bool is_falling;
		int jump_start;
		int current_sprite;

		bool is_hit;

		//int cycle_direction;
		//int distance_walked;

	} data;
};

}