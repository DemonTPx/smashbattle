#pragma once

#include "network/Command.hpp"

namespace network{

class CommandSetBroadcastText : public Command
{
public:

	CommandSetBroadcastText() : Command(Command::Types::SetBroadcastText) 
	{
		memset(&data, 0x00, sizeof(data));
	}
	~CommandSetBroadcastText() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;

		char text[80+1];
		Uint32 duration;

	} data;
};

}