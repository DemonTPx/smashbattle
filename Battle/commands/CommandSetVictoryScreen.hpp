#pragma once

#include "network/Command.hpp"

namespace network{

class CommandSetVictoryScreen : public Command
{
public:

	CommandSetVictoryScreen () : Command(Command::Types::SetVictoryScreen) { }
	~CommandSetVictoryScreen() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
		Uint32 duration;
	} data;

private:
};

}