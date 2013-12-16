#pragma once

#include "network/Command.hpp"

class CommandShotFired : public Command
{
public:

	CommandShotFired() : Command(Command::Types::ShotFired) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandShotFired() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;
		int current_sprite;
		Sint16 x;
		Sint16 y;
		int distance_travelled;
	} data;
};