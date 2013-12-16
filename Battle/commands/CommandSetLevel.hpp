#pragma once

#include "Command.hpp"
#include "Level.h"

class CommandSetLevel : public Command
{
public:

	CommandSetLevel () : Command(Command::Types::SetLevel) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetLevel() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		char your_id;
		char levelname[80 + 1];

		int level[TILE_COUNT];
		int level_hp[TILE_COUNT];
	} data;

private:
};