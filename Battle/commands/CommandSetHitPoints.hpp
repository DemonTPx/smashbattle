#pragma once

#include "Command.hpp"

class CommandSetHitPoints : public Command
{
public:

	CommandSetHitPoints() : Command(Command::Types::SetHitPoints) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetHitPoints() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;
		int hitpoints;
	} data;
};