#pragma once

#include "network/Command.hpp"

namespace network {

class CommandBombDropped : public Command
{
public:

	CommandBombDropped() : Command(Command::Types::BombDropped) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandBombDropped() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		char client_id;
		int current_sprite;
		Sint16 x;
		Sint16 y;
	} data;
};

}