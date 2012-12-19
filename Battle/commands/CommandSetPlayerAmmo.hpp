#ifndef __COMMAND_SET_PLAYER_AMMO__HPP__
#define __COMMAND_SET_PLAYER_AMMO__HPP__

#include "Command.hpp"

class CommandSetPlayerAmmo : public Command
{
public:

	CommandSetPlayerAmmo() : Command(Command::Types::SetPlayerAmmo) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetPlayerAmmo() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;

		int bombs;

	} data;
};


#endif //__COMMAND_SET_PLAYER_AMMO__HPP__