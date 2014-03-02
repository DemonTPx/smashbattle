#pragma once

#include "network/Command.hpp"

namespace network{

class CommandSetGameStart : public Command
{
public:

	CommandSetGameStart() : Command(Command::Types::SetGameStart) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetGameStart() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		short delay;
	} data;
};

}