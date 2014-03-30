#pragma once

#include "network/Command.hpp"

namespace network{

class CommandKeepAlive : public Command
{
public:

	CommandKeepAlive () : Command(Command::Types::KeepAlive) { }
	~CommandKeepAlive() {	}

	virtual void * getData() { return &data; };
	virtual size_t getDataLen() { return sizeof(data); };

	struct
	{
		Uint32 time;
	} data;

private:
};

}