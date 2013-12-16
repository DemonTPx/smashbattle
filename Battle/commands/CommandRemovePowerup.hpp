#pragma once

#include "Command.hpp"

class CommandRemovePowerup : public Command
{
public:

	CommandRemovePowerup() : Command(Command::Types::RemovePowerup) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandRemovePowerup() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		short powerup_id;
	} data;
};