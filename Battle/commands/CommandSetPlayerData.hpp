#ifndef __COMMAND_SET_PLAYER_DATA__HPP__
#define __COMMAND_SET_PLAYER_DATA__HPP__

#include "Command.hpp"

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
		//int current_sprite;

		//int cycle_direction;
		//int distance_walked;

	} data;
};


#endif //__COMMAND_SET_PLAYER_DATA__HPP__