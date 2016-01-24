#pragma once

#include "network/Command.hpp"

namespace network {

class CommandUpdateTile : public Command
{
public:

	CommandUpdateTile() : Command(Command::Types::UpdateTile) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandUpdateTile() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		int tile;
		int tile_hp;
	} data;
};

}