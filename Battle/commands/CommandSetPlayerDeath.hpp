#ifndef __COMMAND_SET_PLAYER_DEATH__HPP__
#define __COMMAND_SET_PLAYER_DEATH__HPP__

#include "Command.hpp"

class CommandSetPlayerDeath : public Command
{
public:

	CommandSetPlayerDeath() : Command(Command::Types::SetPlayerDeath) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetPlayerDeath() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;
		bool is_dead;
	} data;
};


#endif //__COMMAND_SET_PLAYER_DEATH__HPP__